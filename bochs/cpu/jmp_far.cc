////////////////////////////////////////////////////////////////////////
// $Id: jmp_far.cc,v 1.2 2005-08-01 22:18:40 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64==0
// Make life easier merging cpu64 & cpu code.
#define RIP EIP
#endif


  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::jump_protected(bxInstruction_c *i, Bit16u cs_raw, bx_address disp)
{
  bx_descriptor_t  descriptor;
  bx_selector_t    selector;
  Bit32u dword1, dword2;

  /* destination selector is not null else #GP(0) */
  if ((cs_raw & 0xfffc) == 0) {
    BX_ERROR(("jump_protected: cs == 0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  parse_selector(cs_raw, &selector);

  /* destination selector index is whithin its descriptor table
     limits else #GP(selector) */
  fetch_raw_descriptor(&selector, &dword1, &dword2, BX_GP_EXCEPTION);

  /* examine AR byte of destination selector for legal values: */
  parse_descriptor(dword1, dword2, &descriptor);

  if ( descriptor.segment ) {
    check_cs(&descriptor, cs_raw, BX_SELECTOR_RPL(cs_raw));
    branch_far64(&selector, &descriptor, disp, CPL);
    return;
  }
  else {
    Bit16u          raw_tss_selector;
    bx_selector_t   tss_selector, gate_cs_selector;
    bx_descriptor_t tss_descriptor, gate_cs_descriptor;
    Bit16u gate_cs_raw;
    Bit32u temp_eIP;

#if BX_SUPPORT_X86_64
    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
      if (descriptor.type != BX_386_CALL_GATE) {
        BX_ERROR(("jump_protected: gate type %u unsupported in long mode", (unsigned) descriptor.type));
        exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
      }
    }
#endif

    // call gate DPL must be >= CPL else #GP(gate selector)
    if (descriptor.dpl < CPL) {
      BX_ERROR(("jump_protected: call gate.dpl < CPL"));
      exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
    }

    // call gate DPL must be >= gate selector RPL else #GP(gate selector)
    if (descriptor.dpl < selector.rpl) {
      BX_ERROR(("jump_protected: call gate.dpl < selector.rpl"));
      exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
    }

    switch ( descriptor.type ) {
      case BX_SYS_SEGMENT_AVAIL_286_TSS:
      case BX_SYS_SEGMENT_AVAIL_386_TSS:

        //if ( descriptor.type==BX_SYS_SEGMENT_AVAIL_286_TSS )
        //  BX_INFO(("jump to 286 TSS"));
        //else
        //  BX_INFO(("jump to 386 TSS"));

        // Task State Seg must be present, else #NP(TSS selector)
        // checked in task_switch()

        // SWITCH_TASKS _without_ nesting to TSS
        task_switch(&selector, &descriptor, BX_TASK_FROM_JUMP, dword1, dword2);

        // EIP must be in code seg limit, else #GP(0)
        if (EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
          BX_ERROR(("jump_protected: EIP not within CS limits"));
          exception(BX_GP_EXCEPTION, 0, 0);
        }
        return;

      case BX_TASK_GATE:
        // task gate must be present else #NP(gate selector)
        if (! IS_PRESENT(descriptor)) {
          BX_ERROR(("jump_protected: task gate.p == 0"));
          exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
        }

        // examine selector to TSS, given in Task Gate descriptor
        // must specify global in the local/global bit else #GP(TSS selector)
        raw_tss_selector = descriptor.u.taskgate.tss_selector;
        parse_selector(raw_tss_selector, &tss_selector);

        if (tss_selector.ti) {
          BX_ERROR(("jump_protected: tss_selector.ti=1"));
          exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
        }

        // index must be within GDT limits else #GP(TSS selector)
        fetch_raw_descriptor(&tss_selector, &dword1, &dword2, BX_GP_EXCEPTION);

        // descriptor AR byte must specify available TSS
        //   else #GP(TSS selector)
        parse_descriptor(dword1, dword2, &tss_descriptor);
        if (tss_descriptor.valid==0 || tss_descriptor.segment) {
          BX_ERROR(("jump_protected: TSS selector points to bad TSS"));
          exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
        }
        if (tss_descriptor.type!=9 && tss_descriptor.type!=1) {
          BX_ERROR(("jump_protected: TSS selector points to bad TSS"));
          exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
        }

        // task state segment must be present, else #NP(tss selector)
        if (! IS_PRESENT(tss_descriptor)) {
          BX_ERROR(("jump_protected: task descriptor.p == 0"));
          exception(BX_NP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
        }

        // SWITCH_TASKS _without_ nesting to TSS
        task_switch(&tss_selector, &tss_descriptor,
                    BX_TASK_FROM_JUMP, dword1, dword2);

        // EIP must be within code segment limit, else #GP(0)
        if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b)
          temp_eIP = EIP;
        else
          temp_eIP =  IP;

        if (temp_eIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
          BX_ERROR(("jump_protected: EIP > CS.limit"));
          exception(BX_GP_EXCEPTION, 0, 0);
        }

        break;

      case BX_286_CALL_GATE:
        BX_DEBUG(("jump_protected: JUMP TO 286 CALL GATE"));

        // gate must be present else #NP(gate selector)
        if (! IS_PRESENT(descriptor)) {
          BX_ERROR(("jump_protected: call gate not present !"));
          exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
        }

        // examine selector to code segment given in call gate descriptor
        // selector must not be null, else #GP(0)
        gate_cs_raw = descriptor.u.gate286.dest_selector;

        if ((gate_cs_raw & 0xfffc) == 0) {
          BX_ERROR(("jump_protected: CS selector null"));
          exception(BX_GP_EXCEPTION, 0, 0);
        }

        parse_selector(gate_cs_raw, &gate_cs_selector);
        // selector must be within its descriptor table limits else #GP(CS selector)
        fetch_raw_descriptor(&gate_cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
        parse_descriptor(dword1, dword2, &gate_cs_descriptor);

        // check code-segment descriptor, no need to check selector RPL
        // for non-conforming segments
        check_cs(&gate_cs_descriptor, gate_cs_raw, 0);

        branch_far32(&gate_cs_selector, &gate_cs_descriptor, 
                           descriptor.u.gate286.dest_offset, CPL);
        return;

      case BX_386_CALL_GATE:

#if BX_SUPPORT_X86_64
        if (BX_CPU_THIS_PTR msr.lma) {         // LONG MODE
          BX_PANIC(("jump to CALL_GATE64 still not implemented"));
        }
#endif

        // gate must be present else #NP(gate selector)
        if (! IS_PRESENT(descriptor)) {
          BX_ERROR(("jump_protected: task gate.p == 0"));
          exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
        }

        // examine selector to code segment given in call gate descriptor
        // selector must not be null, else #GP(0)
        gate_cs_raw = descriptor.u.gate386.dest_selector;

        if ((gate_cs_raw & 0xfffc) == 0) {
          BX_ERROR(("jump_protected: CS selector null"));
          exception(BX_GP_EXCEPTION, 0, 0);
        }

        parse_selector(gate_cs_raw, &gate_cs_selector);
        // selector must be within its descriptor table limits else #GP(CS selector)
        fetch_raw_descriptor(&gate_cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
        parse_descriptor(dword1, dword2, &gate_cs_descriptor);

/*
        STILL NOT IMPLEMENTED ...

        IF (LONG_MODE)
        {
           // in long mode, we need to read the 2nd half of a 16-byte call-gate
           // from the gdt/ldt to get the upper 32 bits of the target RIP
           temp_upper = READ_MEM.q [temp_sel+8]

           // Make sure the extended attribute bits are all zero.
           IF (temp_upper's extended attribute bits != 0)
             EXCEPTION [#GP(temp_sel)]

           // concatenate both halves of RIP
           temp_RIP = tempRIP + (temp_upper SHL 32)
        }
        // set up new CS base, attr, limits
        CS = READ_DESCRIPTOR (temp_desc.segment, clg_chk)

        IF ((64BIT_MODE) && (temp_RIP is non-canonical)
            || (!64BIT_MODE) && (temp_RIP > CS.limit))
        {
           EXCEPTION [#GP(0)]
        }

        RIP = temp_RIP
*/

        // check code-segment descriptor, no need to check selector RPL
        // for non-conforming segments
        check_cs(&gate_cs_descriptor, gate_cs_raw, 0);

        // IP must be in code segment limit else #GP(0)
        if ( descriptor.u.gate386.dest_offset >
             gate_cs_descriptor.u.segment.limit_scaled )
        {
          BX_ERROR(("jump_protected: EIP > limit"));
          exception(BX_GP_EXCEPTION, 0, 0);
        }

        // load CS:IP from call gate
        // load CS cache with new code segment
        // set rpl of CS to CPL
        load_cs(&gate_cs_selector, &gate_cs_descriptor, CPL);
        EIP = descriptor.u.gate386.dest_offset;
        return;

      default:
        BX_ERROR(("jump_protected: gate type %u unsupported", (unsigned) descriptor.type));
        exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
    }
  }
}
