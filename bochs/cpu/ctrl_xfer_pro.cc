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



#define BX_IN_CPU_METHOD 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR





#if BX_CPU_LEVEL >= 2
  void
BX_CPU_C::jump_protected(BxInstruction_t *i, Bit16u cs_raw, Bit32u disp32)
{
  bx_descriptor_t  descriptor;
  bx_selector_t    selector;
  Bit32u dword1, dword2;



  /* destination selector is not null else #GP(0) */
  if ((cs_raw & 0xfffc) == 0) {
    BX_PANIC(("jump_protected: cs == 0\n"));
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  parse_selector(cs_raw, &selector);

  /* destination selector index is whithin its descriptor table
     limits else #GP(selector) */
  fetch_raw_descriptor(&selector, &dword1, &dword2,
    BX_GP_EXCEPTION);

  /* examine AR byte of destination selector for legal values: */
  parse_descriptor(dword1, dword2, &descriptor);

  if ( descriptor.segment ) {
    if ( descriptor.u.segment.executable==0 ) {
      BX_INFO(("jump_protected: S=1: descriptor not executable\n"));
      exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
      return;
      }
    // CASE: JUMP CONFORMING CODE SEGMENT:
    if ( descriptor.u.segment.c_ed ) {
      // descripor DPL must be <= CPL else #GP(selector)
      if (descriptor.dpl > CPL) {
        BX_INFO(("jump_protected: dpl > CPL\n"));
        exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
        return;
        }

      /* segment must be PRESENT else #NP(selector) */
      if (descriptor.p == 0) {
        BX_INFO(("jump_protected: p == 0\n"));
        exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
        return;
        }

      /* instruction pointer must be in code segment limit else #GP(0) */
      if (disp32 > descriptor.u.segment.limit_scaled) {
        BX_PANIC(("jump_protected: IP > limit\n"));
        exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }

      /* Load CS:IP from destination pointer */
      /* Load CS-cache with new segment descriptor */
      /* CPL does not change for conforming code segment */
      load_cs(&selector, &descriptor, CPL);
      BX_CPU_THIS_PTR eip = disp32;
      return;
      }

    // CASE: jump nonconforming code segment:
    else {
      /* RPL of destination selector must be <= CPL else #GP(selector) */
      if (selector.rpl > CPL) {
        BX_PANIC(("jump_protected: rpl > CPL\n"));
        exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
        return;
        }

      // descriptor DPL must = CPL else #GP(selector)
      if (descriptor.dpl != CPL) {
        BX_INFO(("jump_protected: dpl != CPL\n"));
        exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
        return;
        }

      /* segment must be PRESENT else #NP(selector) */
      if (descriptor.p == 0) {
        BX_INFO(("jump_protected: p == 0\n"));
        exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
        return;
        }

      /* IP must be in code segment limit else #GP(0) */
      if (disp32 > descriptor.u.segment.limit_scaled) {
        BX_PANIC(("jump_protected: IP > limit\n"));
        exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }

      /* load CS:IP from destination pointer */
      /* load CS-cache with new segment descriptor */
      /* set RPL field of CS register to CPL */
      load_cs(&selector, &descriptor, CPL);
      BX_CPU_THIS_PTR eip = disp32;
      return;
      }
    BX_PANIC(("jump_protected: segment=1\n"));
    }

  else {
    Bit16u          raw_tss_selector;
    bx_selector_t   tss_selector, gate_cs_selector;
    bx_descriptor_t tss_descriptor, gate_cs_descriptor;
    Bit16u gate_cs_raw;
    Bit32u temp_eIP;


    switch ( descriptor.type ) {
      case  1: // 286 available TSS
      case  9: // 386 available TSS
        //if ( descriptor.type==1 )
        //  BX_INFO(("jump to 286 TSS\n"));
        //else
        //  BX_INFO(("jump to 386 TSS\n"));

        // TSS DPL must be >= CPL, else #GP(TSS selector)
        if (descriptor.dpl < CPL) {
          BX_PANIC(("jump_protected: TSS.dpl < CPL\n"));
          exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // TSS DPL must be >= TSS selector RPL, else #GP(TSS selector)
        if (descriptor.dpl < selector.rpl) {
          BX_PANIC(("jump_protected: TSS.dpl < selector.rpl\n"));
          exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // descriptor AR byte must specify available TSS,
        //   else #GP(TSS selector) */
        // this is taken care of by the 'default' case of switch statement */

        // Task State Seg must be present, else #NP(TSS selector)
        // checked in task_switch()

        // SWITCH_TASKS _without_ nesting to TSS
        task_switch(&selector, &descriptor,
          BX_TASK_FROM_JUMP, dword1, dword2);

        // IP must be in code seg limit, else #GP(0)
        if (EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
          BX_INFO(("jump_protected: TSS.p == 0\n"));
          exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }
        return;
        break;

      case  3: // Busy 286 TSS
        BX_PANIC(("jump_protected: JUMP to busy 286 TSS unsupported.\n"));
        return;
        break;

      case  4: // 286 call gate
        //BX_INFO(("jump_protected: JUMP TO 286 CALL GATE:\n"));

        // descriptor DPL must be >= CPL else #GP(gate selector)
        if (descriptor.dpl < CPL) {
          BX_INFO(("jump_protected: gate.dpl < CPL\n"));
          exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // descriptor DPL must be >= gate selector RPL else #GP(gate selector)
        if (descriptor.dpl < selector.rpl) {
          BX_INFO(("jump_protected: gate.dpl < selector.rpl\n"));
          exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // gate must be present else #NP(gate selector)
        if (descriptor.p==0) {
          BX_PANIC(("jump_protected: task gate.p == 0\n"));
          exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // examine selector to code segment given in call gate descriptor
        // selector must not be null, else #GP(0)
        gate_cs_raw = descriptor.u.gate286.dest_selector;
        if ( (gate_cs_raw & 0xfffc) == 0 ) {
          BX_PANIC(("jump_protected: CS selector null\n"));
          exception(BX_GP_EXCEPTION, 0x0000, 0);
          }
        parse_selector(gate_cs_raw, &gate_cs_selector);

        // selector must be within its descriptor table limits else #GP(CS selector)
        fetch_raw_descriptor(&gate_cs_selector, &dword1, &dword2,
          BX_GP_EXCEPTION);
        parse_descriptor(dword1, dword2, &gate_cs_descriptor);
        // descriptor AR byte must indicate code segment else #GP(CS selector)
        if ( (gate_cs_descriptor.valid==0) ||
             (gate_cs_descriptor.segment==0) ||
             (gate_cs_descriptor.u.segment.executable==0) ) {
          BX_INFO(("jump_protected: AR byte: not code segment.\n"));
          exception(BX_GP_EXCEPTION, gate_cs_raw & 0xfffc, 0);
          }

        // if non-conforming, code segment descriptor DPL must = CPL else #GP(CS selector)
        if (gate_cs_descriptor.u.segment.c_ed==0) {
          if (gate_cs_descriptor.dpl != CPL) {
            BX_INFO(("jump_protected: non-conform: code seg des DPL != CPL.\n"));
            exception(BX_GP_EXCEPTION, gate_cs_raw & 0xfffc, 0);
            }
          }
        // if conforming, then code segment descriptor DPL must <= CPL else #GP(CS selector)
        else {
          if (gate_cs_descriptor.dpl > CPL) {
            BX_INFO(("jump_protected: conform: code seg des DPL > CPL.\n"));
            exception(BX_GP_EXCEPTION, gate_cs_raw & 0xfffc, 0);
            }
          }

        // code segment must be present else #NP(CS selector)
        if (gate_cs_descriptor.p==0) {
          BX_INFO(("jump_protected: code seg not present.\n"));
          exception(BX_NP_EXCEPTION, gate_cs_raw & 0xfffc, 0);
          }

        // IP must be in code segment limit else #GP(0)
        if ( descriptor.u.gate286.dest_offset >
             gate_cs_descriptor.u.segment.limit_scaled ) {
          BX_PANIC(("jump_protected: IP > limit\n"));
          exception(BX_GP_EXCEPTION, 0x0000, 0);
          }

        // load CS:IP from call gate
        // load CS cache with new code segment
        // set rpl of CS to CPL
        load_cs(&gate_cs_selector, &gate_cs_descriptor, CPL);
        EIP = descriptor.u.gate286.dest_offset;
        return;
        break;


      case  5: // task gate
//BX_INFO(("jump_pro: task gate\n"));

        // gate descriptor DPL must be >= CPL else #GP(gate selector)
        if (descriptor.dpl < CPL) {
          BX_PANIC(("jump_protected: gate.dpl < CPL\n"));
          exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // gate descriptor DPL must be >= gate selector RPL
        //   else #GP(gate selector)
        if (descriptor.dpl < selector.rpl) {
          BX_PANIC(("jump_protected: gate.dpl < selector.rpl\n"));
          exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // task gate must be present else #NP(gate selector)
        if (descriptor.p==0) {
          BX_PANIC(("jump_protected: task gate.p == 0\n"));
          exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // examine selector to TSS, given in Task Gate descriptor
        // must specify global in the local/global bit else #GP(TSS selector)

        raw_tss_selector = descriptor.u.taskgate.tss_selector;
        parse_selector(raw_tss_selector, &tss_selector);
        if (tss_selector.ti) {
          BX_PANIC(("jump_protected: tss_selector.ti=1\n"));
          exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }

        // index must be within GDT limits else #GP(TSS selector)
        fetch_raw_descriptor(&tss_selector, &dword1, &dword2,
          BX_GP_EXCEPTION);

        // descriptor AR byte must specify available TSS
        //   else #GP(TSS selector)
        parse_descriptor(dword1, dword2, &tss_descriptor);
        if (tss_descriptor.valid==0 || tss_descriptor.segment) {
          BX_INFO(("jump_protected: TSS selector points to bad TSS\n"));
          exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          }
        if (tss_descriptor.type!=9 && tss_descriptor.type!=1) {
          BX_INFO(("jump_protected: TSS selector points to bad TSS\n"));
          exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          }


        // task state segment must be present, else #NP(tss selector)
        if (tss_descriptor.p==0) {
          BX_PANIC(("jump_protected: task descriptor.p == 0\n"));
          exception(BX_NP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          }

        // SWITCH_TASKS _without_ nesting to TSS
        task_switch(&tss_selector, &tss_descriptor,
                    BX_TASK_FROM_JUMP, dword1, dword2);

        // eIP must be within code segment limit, else #GP(0)
        if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b)
          temp_eIP = EIP;
        else
          temp_eIP =  IP;
        if (temp_eIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
          BX_PANIC(("jump_protected: eIP > cs.limit\n"));
          exception(BX_GP_EXCEPTION, 0x0000, 0);
          }

        break;

      case 11: // Busy 386 TSS
        BX_PANIC(("jump_protected: JUMP to busy 386 TSS unsupported.\n"));
        return;
        break;

      case 12: // 386 call gate
        //BX_INFO(("jump_protected: JUMP TO 386 CALL GATE:\n"));

        // descriptor DPL must be >= CPL else #GP(gate selector)
        if (descriptor.dpl < CPL) {
          BX_PANIC(("jump_protected: gate.dpl < CPL\n"));
          exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // descriptor DPL must be >= gate selector RPL else #GP(gate selector)
        if (descriptor.dpl < selector.rpl) {
          BX_PANIC(("jump_protected: gate.dpl < selector.rpl\n"));
          exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // gate must be present else #NP(gate selector)
        if (descriptor.p==0) {
          BX_PANIC(("jump_protected: task gate.p == 0\n"));
          exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // examine selector to code segment given in call gate descriptor
        // selector must not be null, else #GP(0)
        gate_cs_raw = descriptor.u.gate386.dest_selector;
        if ( (gate_cs_raw & 0xfffc) == 0 ) {
          BX_PANIC(("jump_protected: CS selector null\n"));
          exception(BX_GP_EXCEPTION, 0x0000, 0);
          }
        parse_selector(gate_cs_raw, &gate_cs_selector);

        // selector must be within its descriptor table limits else #GP(CS selector)
        fetch_raw_descriptor(&gate_cs_selector, &dword1, &dword2,
          BX_GP_EXCEPTION);
        parse_descriptor(dword1, dword2, &gate_cs_descriptor);
        // descriptor AR byte must indicate code segment else #GP(CS selector)
        if ( (gate_cs_descriptor.valid==0) ||
             (gate_cs_descriptor.segment==0) ||
             (gate_cs_descriptor.u.segment.executable==0) ) {
          BX_PANIC(("jump_protected: AR byte: not code segment.\n"));
          exception(BX_GP_EXCEPTION, gate_cs_raw & 0xfffc, 0);
          }

        // if non-conforming, code segment descriptor DPL must = CPL else #GP(CS selector)
        if (gate_cs_descriptor.u.segment.c_ed==0) {
          if (gate_cs_descriptor.dpl != CPL) {
            BX_PANIC(("jump_protected: non-conform: code seg des DPL != CPL.\n"));
            exception(BX_GP_EXCEPTION, gate_cs_raw & 0xfffc, 0);
            }
          }
        // if conforming, then code segment descriptor DPL must <= CPL else #GP(CS selector)
        else {
          if (gate_cs_descriptor.dpl > CPL) {
            BX_PANIC(("jump_protected: conform: code seg des DPL > CPL.\n"));
            exception(BX_GP_EXCEPTION, gate_cs_raw & 0xfffc, 0);
            }
          }

        // code segment must be present else #NP(CS selector)
        if (gate_cs_descriptor.p==0) {
          BX_PANIC(("jump_protected: code seg not present.\n"));
          exception(BX_NP_EXCEPTION, gate_cs_raw & 0xfffc, 0);
          }

        // IP must be in code segment limit else #GP(0)
        if ( descriptor.u.gate386.dest_offset >
             gate_cs_descriptor.u.segment.limit_scaled ) {
          BX_PANIC(("jump_protected: IP > limit\n"));
          exception(BX_GP_EXCEPTION, 0x0000, 0);
          }

        // load CS:IP from call gate
        // load CS cache with new code segment
        // set rpl of CS to CPL
        load_cs(&gate_cs_selector, &gate_cs_descriptor, CPL);
        EIP = descriptor.u.gate386.dest_offset;
        return;
        break;

      default:
        BX_INFO(("jump_protected: gate type %u unsupported\n",
          (unsigned) descriptor.type));
        exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
        return;
        break;
      }
    }
    return;
}
#endif /* if BX_CPU_LEVEL >= 2 */


#if BX_CPU_LEVEL >= 2
  void
BX_CPU_C::call_protected(BxInstruction_t *i, Bit16u cs_raw, Bit32u disp32)
{
  bx_selector_t cs_selector;
  Bit32u dword1, dword2;
  bx_descriptor_t cs_descriptor;

  /* Opsize in effect for CALL is specified by the D bit for the
   * segment containing dest & by any opsize prefix.
   * For gate descriptor, deterermined by type of call gate:
   * 4=16bit, 12=32bit
   * count field: 16bit specifies #words, 32bit specifies #dwords
   */

  /* new cs selector must not be null, else #GP(0) */
  if ( (cs_raw & 0xfffc) == 0 ) {
    BX_PANIC(("call_protected: CS selector null\n"));
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  parse_selector(cs_raw, &cs_selector);

  // check new CS selector index within its descriptor limits,
  // else #GP(new CS selector)
  fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
    BX_GP_EXCEPTION);

  parse_descriptor(dword1, dword2, &cs_descriptor);

  // examine AR byte of selected descriptor for various legal values
  if (cs_descriptor.valid==0) {
    BX_PANIC(("call_protected: invalid CS descriptor\n"));
    exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
    }

  if (cs_descriptor.segment) { // normal segment
    Bit32u temp_ESP;

    if (cs_descriptor.u.segment.executable==0) {
      BX_PANIC(("call_protected: non executable segment\n"));
      exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
      return;
      }

    if (cs_descriptor.u.segment.c_ed) { // conforming code segment
      // DPL must be <= CPL, else #GP(code seg selector)
      if (cs_descriptor.dpl > CPL) {
        BX_PANIC(("call_protected: cs.dpl > CPL\n"));
        exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
        return;
        }
      }
    else { // non-conforming code segment
      // RPL must be <= CPL, else #GP(code seg selector)
      // DPL must be = CPL, else #GP(code seg selector)
      if ( (cs_selector.rpl > CPL) ||
           (cs_descriptor.dpl != CPL) ) {
        BX_PANIC(("call_protected: cs.rpl > CPL\n"));
        exception(BX_GP_EXCEPTION, cs_raw & 0xfffc, 0);
        }
      }

    // segment must be present, else #NP(code seg selector)
    if (cs_descriptor.p == 0) {
      BX_INFO(("call_protected: cs.p = 0\n"));
      exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
      }

    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
      temp_ESP = ESP;
    else
      temp_ESP = SP;

    // stack must be big enough for return addr, else #SS(0)
    if (i->os_32) {
      if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_ESP, 8) ) {
        BX_PANIC(("call_protected: stack doesn't have room for ret addr\n"));
        exception(BX_SS_EXCEPTION, 0, 0);
        }

      // IP must be in code seg limit, else #GP(0)
      if (disp32 > cs_descriptor.u.segment.limit_scaled) {
        BX_PANIC(("call_protected: IP not in code seg limit\n"));
        exception(BX_GP_EXCEPTION, 0, 0);
        }

      // push return address onto stack (CS padded to 32bits)
      push_32((Bit32u) BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
      push_32(EIP);
      }
    else { // 16bit opsize
      if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_ESP, 4) ) {
        BX_PANIC(("call_protected: stack doesn't have room for ret addr\n"));
        exception(BX_SS_EXCEPTION, 0, 0);
        }

      // IP must be in code seg limit, else #GP(0)
      if (disp32 > cs_descriptor.u.segment.limit_scaled) {
        BX_PANIC(("call_protected: IP not in code seg limit\n"));
        exception(BX_GP_EXCEPTION, 0, 0);
        }

      push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
      push_16(IP);
      }

    // load code segment descriptor into CS cache
    // load CS with new code segment selector
    // set RPL of CS to CPL
    // load eIP with new offset
    load_cs(&cs_selector, &cs_descriptor, CPL);
    BX_CPU_THIS_PTR eip = disp32;
    if (cs_descriptor.u.segment.d_b==0)
      BX_CPU_THIS_PTR eip &= 0x0000ffff;
    return;
    }
  else { // gate & special segment
    bx_descriptor_t  gate_descriptor;
    bx_selector_t    gate_selector;
    Bit32u new_EIP;
    Bit16u dest_selector;
    Bit16u          raw_tss_selector;
    bx_selector_t   tss_selector;
    bx_descriptor_t tss_descriptor;
    Bit32u temp_eIP;

    /* 1 level of indirection via gate, switch gate & cs */
    gate_descriptor = cs_descriptor;
    gate_selector   = cs_selector;

    switch (gate_descriptor.type) {
      case 1: // available 16bit TSS
      case 9: // available 32bit TSS
        //if (gate_descriptor.type==1)
        //  BX_INFO(("call_protected: 16bit available TSS\n"));
        //else
        //  BX_INFO(("call_protected: 32bit available TSS\n"));

        // TSS DPL must be >= CPL, else #TS(TSS selector)
        if (gate_descriptor.dpl < CPL) {
          BX_PANIC(("call_protected: TSS.dpl < CPL\n"));
          exception(BX_TS_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // TSS DPL must be >= TSS selector RPL, else #TS(TSS selector)
        if (gate_descriptor.dpl < gate_selector.rpl) {
          BX_PANIC(("call_protected: TSS.dpl < selector.rpl\n"));
          exception(BX_TS_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // descriptor AR byte must specify available TSS,
        //   else #TS(TSS selector) */
        // this is taken care of by the 'default' case of switch statement */

        // Task State Seg must be present, else #NP(TSS selector)
        // checked in task_switch()

        // SWITCH_TASKS _without_ nesting to TSS
        task_switch(&gate_selector, &gate_descriptor,
          BX_TASK_FROM_CALL_OR_INT, dword1, dword2);

        // IP must be in code seg limit, else #TS(0)
        if (EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
          BX_INFO(("call_protected: TSS.p == 0\n"));
          exception(BX_TS_EXCEPTION, 0, 0);
          return;
          }
        return;
        break;

      case 5: // TASK GATE
        //BX_INFO(("call_protected: task gate\n"));
        // gate descriptor DPL must be >= CPL else #TS(gate selector)
        if (gate_descriptor.dpl < CPL) {
          BX_PANIC(("call_protected: gate.dpl < CPL\n"));
          exception(BX_TS_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // gate descriptor DPL must be >= gate selector RPL
        //   else #TS(gate selector)
        if (gate_descriptor.dpl < gate_selector.rpl) {
          BX_PANIC(("call_protected: gate.dpl < selector.rpl\n"));
          exception(BX_TS_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // task gate must be present else #NP(gate selector)
        if (gate_descriptor.p==0) {
          BX_PANIC(("call_protected: task gate.p == 0\n"));
          exception(BX_NP_EXCEPTION, cs_raw & 0xfffc, 0);
          return;
          }

        // examine selector to TSS, given in Task Gate descriptor
        // must specify global in the local/global bit else #TS(TSS selector)

        raw_tss_selector = gate_descriptor.u.taskgate.tss_selector;
        parse_selector(raw_tss_selector, &tss_selector);
        if (tss_selector.ti) {
          BX_PANIC(("call_protected: tss_selector.ti=1\n"));
          exception(BX_TS_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }

        // index must be within GDT limits else #TS(TSS selector)
        fetch_raw_descriptor(&tss_selector, &dword1, &dword2,
          BX_TS_EXCEPTION);

        // descriptor AR byte must specify available TSS
        //   else #TS(TSS selector)
        parse_descriptor(dword1, dword2, &tss_descriptor);
        if (tss_descriptor.valid==0 || tss_descriptor.segment) {
          BX_PANIC(("call_protected: TSS selector points to bad TSS\n"));
          exception(BX_TS_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          }
        if (tss_descriptor.type!=9 && tss_descriptor.type!=1) {
          BX_PANIC(("call_protected: TSS selector points to bad TSS\n"));
          exception(BX_TS_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          }


        // task state segment must be present, else #NP(tss selector)
        if (tss_descriptor.p==0) {
          BX_PANIC(("call_protected: task descriptor.p == 0\n"));
          exception(BX_NP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          }

        // SWITCH_TASKS without nesting to TSS
        task_switch(&tss_selector, &tss_descriptor,
                    BX_TASK_FROM_CALL_OR_INT, dword1, dword2);

        // eIP must be within code segment limit, else #TS(0)
        if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b)
          temp_eIP = EIP;
        else
          temp_eIP =  IP;
        if (temp_eIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
          BX_PANIC(("call_protected: eIP > cs.limit\n"));
          exception(BX_TS_EXCEPTION, 0x0000, 0);
          }

        return;
        break;

      case  4: // 16bit CALL GATE
      case 12: // 32bit CALL GATE
//if (gate_descriptor.type==4)
//  BX_INFO(("CALL: 16bit call gate\n"));
//else
//  BX_INFO(("CALL: 32bit call gate\n"));

        // call gate DPL must be >= CPL, else #GP(call gate selector)
        // call gate DPL must be >= RPL, else #GP(call gate selector)
        if ( (gate_descriptor.dpl < CPL) ||
             (gate_descriptor.dpl < gate_selector.rpl) ) {
          BX_PANIC(("call_protected: DPL < CPL or RPL\n"));
          exception(BX_GP_EXCEPTION, gate_selector.value & 0xfffc, 0);
          }

        // call gate must be present, else #NP(call gate selector)
        if (gate_descriptor.p==0) {
          BX_PANIC(("call_protected: not present\n"));
          exception(BX_NP_EXCEPTION, gate_selector.value & 0xfffc, 0);
          }

        // examine code segment selector in call gate descriptor

        if (gate_descriptor.type==4) {
          dest_selector = gate_descriptor.u.gate286.dest_selector;
          new_EIP = gate_descriptor.u.gate286.dest_offset;
          }
        else {
          dest_selector = gate_descriptor.u.gate386.dest_selector;
          new_EIP = gate_descriptor.u.gate386.dest_offset;
          }

        // selector must not be null else #GP(0)
        if ( (dest_selector & 0xfffc) == 0 ) {
          BX_PANIC(("call_protected: selector in gate null\n"));
          exception(BX_GP_EXCEPTION, 0, 0);
          }

        parse_selector(dest_selector, &cs_selector);

        // selector must be within its descriptor table limits,
        //   else #GP(code segment selector)
        fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
          BX_GP_EXCEPTION);
        parse_descriptor(dword1, dword2, &cs_descriptor);

        // AR byte of selected descriptor must indicate code segment,
        //   else #GP(code segment selector)
        // DPL of selected descriptor must be <= CPL,
        // else #GP(code segment selector)
        if (cs_descriptor.valid==0 ||
            cs_descriptor.segment==0 ||
            cs_descriptor.u.segment.executable==0 ||
            cs_descriptor.dpl > CPL) {
          BX_PANIC(("call_protected: selected desciptor not code\n"));
          exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
          }

        // CALL GATE TO MORE PRIVILEGE
        // if non-conforming code segment and DPL < CPL then
        // ??? use gate_descriptor.dpl or cs_descriptor.dpl ???
        if ( (cs_descriptor.u.segment.c_ed==0)  &&
             (cs_descriptor.dpl < CPL) ) {

          Bit16u SS_for_cpl_x;
          Bit32u ESP_for_cpl_x;
          bx_selector_t   ss_selector;
          bx_descriptor_t ss_descriptor;
          unsigned room_needed;
          Bit8u    param_count;
          Bit16u   return_SS, return_CS;
          Bit32u   return_ESP, return_EIP;
          Bit32u   return_ss_base;
          unsigned i;
          Bit16u   parameter_word[32];
          Bit32u   parameter_dword[32];
          Bit32u   temp_ESP;

//BX_INFO(("CALL: Call Gate: to more priviliged level\n"));

          // get new SS selector for new privilege level from TSS
          get_SS_ESP_from_TSS(cs_descriptor.dpl,
                              &SS_for_cpl_x, &ESP_for_cpl_x);

/* ??? use dpl or rpl ??? */

          // check selector & descriptor for new SS:
          // selector must not be null, else #TS(0)
          if ( (SS_for_cpl_x & 0xfffc) == 0 ) {
            BX_PANIC(("call_protected: new SS null\n"));
            exception(BX_TS_EXCEPTION, 0, 0);
            return;
            }

          // selector index must be within its descriptor table limits,
          //   else #TS(SS selector)
          parse_selector(SS_for_cpl_x, &ss_selector);
          fetch_raw_descriptor(&ss_selector, &dword1, &dword2,
            BX_TS_EXCEPTION);

          parse_descriptor(dword1, dword2, &ss_descriptor);

          // selector's RPL must equal DPL of code segment,
          //   else #TS(SS selector)
          if (ss_selector.rpl != cs_descriptor.dpl) {
            BX_PANIC(("call_protected: SS selector.rpl != CS descr.dpl\n"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          // stack segment DPL must equal DPL of code segment,
          //   else #TS(SS selector)
          if (ss_descriptor.dpl != cs_descriptor.dpl) {
            BX_PANIC(("call_protected: SS descr.rpl != CS descr.dpl\n"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          // descriptor must indicate writable data segment,
          //   else #TS(SS selector)
          if (ss_descriptor.valid==0 ||
              ss_descriptor.segment==0  ||
              ss_descriptor.u.segment.executable ||
              ss_descriptor.u.segment.r_w==0) {
            BX_INFO(("call_protected: ss descriptor not writable data seg\n"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          // segment must be present, else #SS(SS selector)
          if (ss_descriptor.p==0) {
            BX_PANIC(("call_protected: ss descriptor not present.\n"));
            exception(BX_SS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          if ( cs_descriptor.u.segment.d_b )
            // new stack must have room for parameters plus 16 bytes
            room_needed = 16;
          else
            // new stack must have room for parameters plus 8 bytes
            room_needed =  8;

          if (gate_descriptor.type==4) {
            // get word count from call gate, mask to 5 bits
            param_count = gate_descriptor.u.gate286.word_count & 0x1f;
            room_needed += param_count*2;
            }
          else {
            // get word count from call gate, mask to 5 bits
            param_count = gate_descriptor.u.gate386.dword_count & 0x1f;
            room_needed += param_count*4;
            }

          // new stack must have room for parameters plus return info
          //   else #SS(SS selector)

          if ( !can_push(&ss_descriptor, ESP_for_cpl_x, room_needed) ) {
            BX_INFO(("call_protected: stack doesn't have room\n"));
            exception(BX_SS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          // new eIP must be in code segment limit else #GP(0)
          if ( new_EIP > cs_descriptor.u.segment.limit_scaled ) {
            BX_PANIC(("call_protected: IP not within CS limits\n"));
            exception(BX_GP_EXCEPTION, 0, 0);
            return;
            }


          // save return SS:eSP to be pushed on new stack
          return_SS = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
          if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
            return_ESP = ESP;
          else
            return_ESP =  SP;
          return_ss_base = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base;

          // save return CS:eIP to be pushed on new stack
          return_CS = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
          if ( cs_descriptor.u.segment.d_b )
            return_EIP = EIP;
          else
            return_EIP =  IP;


          if (gate_descriptor.type==4) {
            for (i=0; i<param_count; i++) {
              access_linear(return_ss_base + return_ESP + i*2,
                2, 0, BX_READ, &parameter_word[i]);
              }
            }
          else {
            for (i=0; i<param_count; i++) {
              access_linear(return_ss_base + return_ESP + i*4,
                4, 0, BX_READ, &parameter_dword[i]);
              }
            }

          /* load new SS:SP value from TSS */
          /* load SS descriptor */
          load_ss(&ss_selector, &ss_descriptor, ss_descriptor.dpl);
          if (ss_descriptor.u.segment.d_b)
            ESP = ESP_for_cpl_x;
          else
            SP =  (Bit16u) ESP_for_cpl_x;

          /* load new CS:IP value from gate */
          /* load CS descriptor */
          /* set CPL to stack segment DPL */
          /* set RPL of CS to CPL */
          load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);
          EIP = new_EIP;

          // push pointer of old stack onto new stack
          if (gate_descriptor.type==4) {
            push_16(return_SS);
            push_16((Bit16u) return_ESP);
            }
          else {
            push_32(return_SS);
            push_32(return_ESP);
            }

          /* get word count from call gate, mask to 5 bits */
          /* copy parameters from old stack onto new stack */
          if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
            temp_ESP = ESP;
          else
            temp_ESP =  SP;

          if (gate_descriptor.type==4) {
            for (i=param_count; i>0; i--) {
              push_16(parameter_word[i-1]);
              //access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + i*2,
              //  2, 0, BX_WRITE, &parameter_word[i]);
              }
            }
          else {
            for (i=param_count; i>0; i--) {
              push_32(parameter_dword[i-1]);
              //access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + i*4,
              //  4, 0, BX_WRITE, &parameter_dword[i]);
              }
            }

          // push return address onto new stack
          if (gate_descriptor.type==4) {
            push_16(return_CS);
            push_16((Bit16u) return_EIP);
            }
          else {
            push_32(return_CS);
            push_32(return_EIP);
            }

          return;
          }

        // CALL GATE TO SAME PRIVILEGE
        else {
          Bit32u temp_ESP;

//BX_INFO(("CALL: Call Gate: to same priviliged level\n"));
          if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
            temp_ESP = ESP;
          else
            temp_ESP = SP;

          if (gate_descriptor.type==12) {
          //if (i->os_32) {}
            // stack must room for 8-byte return address (2 are padding)
            //   else #SS(0)
            if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_ESP, 8) ) {
              BX_PANIC(("call_protected: stack doesn't have room for 8 bytes\n"));
              exception(BX_SS_EXCEPTION, 0, 0);
              }
            }
          else {
            // stack must room for 4-byte return address
            //   else #SS(0)
            if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_ESP, 4) ) {
              BX_PANIC(("call_protected: stack doesn't have room for 4 bytes\n"));
              exception(BX_SS_EXCEPTION, 0, 0);
              }
            }

          // EIP must be within code segment limit, else #GP(0)
          if ( new_EIP > cs_descriptor.u.segment.limit_scaled ) {
            BX_PANIC(("call_protected: IP not within code segment limits\n"));
            exception(BX_GP_EXCEPTION, 0, 0);
            }

          if (gate_descriptor.type==12) {
            // push return address onto stack
            push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
            push_32(EIP);
            }
          else {
            // push return address onto stack
            push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
            push_16(IP);
            }

          // load CS:EIP from gate
          // load code segment descriptor into CS register
          // set RPL of CS to CPL
          load_cs(&cs_selector, &cs_descriptor, CPL);
          EIP = new_EIP;

          return;
          }

        BX_PANIC(("call_protected: call gate: should not get here\n"));
        return;

      default:
        BX_PANIC(("call_protected: type = %d\n",
          (unsigned) cs_descriptor.type));
        return;
      }
    BX_PANIC(("call_protected: gate segment unfinished\n"));
    }

  BX_PANIC(("call_protected: shouldn't get here!\n"));
  return;
}
#endif /* 286+ */


#if BX_CPU_LEVEL >= 2
  void
BX_CPU_C::return_protected(BxInstruction_t *i, Bit16u pop_bytes)
{
  Bit16u raw_cs_selector, raw_ss_selector;
  bx_selector_t cs_selector, ss_selector;
  bx_descriptor_t cs_descriptor, ss_descriptor;
  Bit32u stack_cs_offset, stack_param_offset;
  Bit32u return_EIP, return_ESP, temp_ESP;
  Bit32u dword1, dword2;
  Bit16u return_IP;


  /* + 6+N*2: SS      | +12+N*4:     SS */
  /* + 4+N*2: SP      | + 8+N*4:    ESP */
  /*          parm N  | +        parm N */
  /*          parm 3  | +        parm 3 */
  /*          parm 2  | +        parm 2 */
  /*          parm 1  | + 8:     parm 1 */
  /* + 2:     CS      | + 4:         CS */
  /* + 0:     IP      | + 0:        EIP */

#if BX_CPU_LEVEL >= 3
  if ( i->os_32 ) {
    /* operand size=32: third word on stack must be within stack limits,
     *   else #SS(0); */
    if (!can_pop(6)) {
      BX_PANIC(("return_protected: 3rd word not in stack limits\n"));
      /* #SS(0) */
      return;
      }
    stack_cs_offset = 4;
    stack_param_offset = 8;
    }
  else
#endif
    {
    /* operand size=16: second word on stack must be within stack limits,
     *   else #SS(0);
     */
    if ( !can_pop(4) ) {
      BX_PANIC(("return_protected: 2nd word not in stack limits\n"));
      /* #SS(0) */
      return;
      }
    stack_cs_offset = 2;
    stack_param_offset = 4;
    }

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) temp_ESP = ESP;
  else temp_ESP = SP;

  // return selector RPL must be >= CPL, else #GP(return selector)
  access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP +
                       stack_cs_offset, 2, CPL==3, BX_READ, &raw_cs_selector);
  parse_selector(raw_cs_selector, &cs_selector);
  if ( cs_selector.rpl < CPL ) {
    BX_INFO(("return_protected: CS.rpl < CPL\n"));
    BX_INFO(("  CS.rpl=%u CPL=%u\n", (unsigned) cs_selector.rpl,
      (unsigned) CPL));
    exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
    return;
    }

  // if return selector RPL == CPL then
  // RETURN TO SAME LEVEL
  if ( cs_selector.rpl == CPL ) {
    //BX_INFO(("return: to same level %04x:%08x\n",
    //   BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
    //   BX_CPU_THIS_PTR prev_eip));
    // return selector must be non-null, else #GP(0)
    if ( (raw_cs_selector & 0xfffc) == 0 ) {
      BX_PANIC(("return_protected: CS null\n"));
      /* #GP(0) */
      return;
      }

    // selector index must be within its descriptor table limits,
    // else #GP(selector)
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
      BX_GP_EXCEPTION);

    // descriptor AR byte must indicate code segment, else #GP(selector)
    parse_descriptor(dword1, dword2, &cs_descriptor);
    if (cs_descriptor.valid==0 ||
        cs_descriptor.segment==0 ||
        cs_descriptor.u.segment.executable==0) {
      BX_INFO(("return_protected: same: AR byte not code\n"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      }

    // if non-conforming then code segment DPL must = CPL,
    // else #GP(selector)
    if ((cs_descriptor.u.segment.c_ed==0)  && (cs_descriptor.dpl!=CPL)) {
      BX_PANIC(("return_protected: non-conforming, DPL!=CPL\n"));
      /* #GP(selector) */
      return;
      }

    // if conforming then code segment DPL must be <= CPL,
    // else #GP(selector)
    if (cs_descriptor.u.segment.c_ed  && (cs_descriptor.dpl>CPL)) {
      BX_INFO(("return_protected: conforming, DPL>CPL\n"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      }

    // code segment must be present, else #NP(selector)
    if (cs_descriptor.p==0) {
      BX_INFO(("return_protected: not present\n"));
      exception(BX_NP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      return;
      }

    // top word on stack must be within stack limits, else #SS(0)
    if ( !can_pop(stack_param_offset + pop_bytes) ) {
      BX_PANIC(("return_protected: top word not in stack limits\n"));
      /* #SS(0) */
      return;
      }

    // eIP must be in code segment limit, else #GP(0)
#if BX_CPU_LEVEL >= 3
    if (i->os_32) {
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &return_EIP);
      }
    else
#endif
      {
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
        2, CPL==3, BX_READ, &return_IP);
      return_EIP = return_IP;
      }

    if ( return_EIP > cs_descriptor.u.segment.limit_scaled ) {
      BX_PANIC(("return_protected: return IP > CS.limit\n"));
      /* #GP(0) */
      return;
      }

    // load CS:eIP from stack
    // load CS register with descriptor
    // increment eSP
    load_cs(&cs_selector, &cs_descriptor, CPL);
    BX_CPU_THIS_PTR eip = return_EIP;
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
      ESP += stack_param_offset + pop_bytes;
    else
      SP += stack_param_offset + pop_bytes;

    return;
    }

  /* RETURN TO OUTER PRIVILEGE LEVEL */
  else {
    /* + 6+N*2: SS      | +12+N*4:     SS */
    /* + 4+N*2: SP      | + 8+N*4:    ESP */
    /*          parm N  | +        parm N */
    /*          parm 3  | +        parm 3 */
    /*          parm 2  | +        parm 2 */
    /*          parm 1  | + 8:     parm 1 */
    /* + 2:     CS      | + 4:         CS */
    /* + 0:     IP      | + 0:        EIP */

//BX_INFO(("return: to outer level %04x:%08x\n",
//  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
//  BX_CPU_THIS_PTR prev_eip));

    if (i->os_32) {
      /* top 16+immediate bytes on stack must be within stack limits, else #SS(0) */
      if ( !can_pop(16 + pop_bytes) ) {
        BX_PANIC(("return_protected: 8 bytes not within stack limits\n"));
        /* #SS(0) */
        return;
        }
      }
    else {
      /* top 8+immediate bytes on stack must be within stack limits, else #SS(0) */
      if ( !can_pop(8 + pop_bytes) ) {
        BX_PANIC(("return_protected: 8 bytes not within stack limits\n"));
        /* #SS(0) */
        return;
        }
      }

    /* examine return CS selector and associated descriptor */

    /* selector must be non-null else #GP(0) */
    if ( (raw_cs_selector & 0xfffc) == 0 ) {
      BX_PANIC(("return_protected: CS selector null\n"));
      /* #GP(0) */
      return;
      }

    /* selector index must be within its descriptor table limits,
     * else #GP(selector) */
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
      BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);

    /* descriptor AR byte must indicate code segment else #GP(selector) */
    if (cs_descriptor.valid==0 ||
        cs_descriptor.segment==0  ||
        cs_descriptor.u.segment.executable==0) {
      BX_PANIC(("return_protected: AR byte not code\n"));
      /* #GP(selector) */
      return;
      }

    /* if non-conforming code then code seg DPL must equal return selector RPL
     * else #GP(selector) */
    if (cs_descriptor.u.segment.c_ed==0 &&
        cs_descriptor.dpl!=cs_selector.rpl) {
      BX_PANIC(("return_protected: non-conforming seg DPL != selector.rpl\n"));
      /* #GP(selector) */
      return;
      }

    /* if conforming then code segment DPL must be <= return selector RPL
     * else #GP(selector) */
    if (cs_descriptor.u.segment.c_ed &&
        cs_descriptor.dpl>cs_selector.rpl) {
      BX_PANIC(("return_protected: conforming seg DPL > selector.rpl\n"));
      /* #GP(selector) */
      return;
      }

    /* segment must be present else #NP(selector) */
    if (cs_descriptor.p==0) {
      BX_PANIC(("return_protected: segment not present\n"));
      /* #NP(selector) */
      return;
      }

    /* examine return SS selector and associated descriptor: */
    if (i->os_32) {
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 12 + pop_bytes,
        2, 0, BX_READ, &raw_ss_selector);
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 8 + pop_bytes,
        4, 0, BX_READ, &return_ESP);
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
        4, 0, BX_READ, &return_EIP);
      }
    else {
      Bit16u return_SP;

      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 6 + pop_bytes,
        2, 0, BX_READ, &raw_ss_selector);
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 4 + pop_bytes,
        2, 0, BX_READ, &return_SP);
      return_ESP = return_SP;
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
        2, 0, BX_READ, &return_IP);
      return_EIP = return_IP;
      }

    /* selector must be non-null else #GP(0) */
    if ( (raw_ss_selector & 0xfffc) == 0 ) {
      BX_PANIC(("return_protected: SS selector null\n"));
      /* #GP(0) */
      return;
      }

    /* selector index must be within its descriptor table limits,
     * else #GP(selector) */
    parse_selector(raw_ss_selector, &ss_selector);
    fetch_raw_descriptor(&ss_selector, &dword1, &dword2,
      BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &ss_descriptor);

    /* selector RPL must = RPL of the return CS selector,
     * else #GP(selector) */
    if (ss_selector.rpl != cs_selector.rpl) {
      BX_INFO(("return_protected: ss.rpl != cs.rpl\n"));
      exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
      return;
      }

    /* descriptor AR byte must indicate a writable data segment,
     * else #GP(selector) */
    if (ss_descriptor.valid==0 ||
        ss_descriptor.segment==0 ||
        ss_descriptor.u.segment.executable ||
        ss_descriptor.u.segment.r_w==0) {
      BX_PANIC(("return_protected: SS.AR byte not writable data\n"));
      /* #GP(selector) */
      return;
      }

    /* descriptor dpl must = RPL of the return CS selector,
     * else #GP(selector) */
    if (ss_descriptor.dpl != cs_selector.rpl) {
      BX_PANIC(("return_protected: SS.dpl != cs.rpl\n"));
      /* #GP(selector) */
      return;
      }

    /* segment must be present else #SS(selector) */
    if (ss_descriptor.p==0) {
      BX_PANIC(("ss.p == 0\n"));
      /* #NP(selector) */
      return;
      }

    /* eIP must be in code segment limit, else #GP(0) */
    if (return_EIP > cs_descriptor.u.segment.limit_scaled) {
      BX_PANIC(("return_protected: eIP > cs.limit\n"));
      /* #GP(0) */
      return;
      }

    /* set CPL to RPL of return CS selector */
    /* load CS:IP from stack */
    /* set CS RPL to CPL */
    /* load the CS-cache with return CS descriptor */
    load_cs(&cs_selector, &cs_descriptor, cs_selector.rpl);
    BX_CPU_THIS_PTR eip = return_EIP;

    /* load SS:SP from stack */
    /* load SS-cache with return SS descriptor */
    load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);
    if (ss_descriptor.u.segment.d_b)
      ESP = return_ESP;
    else
      SP  = (Bit16u) return_ESP;

    /* check ES, DS, FS, GS for validity */
    validate_seg_regs();

    return;
    }

  return;
}
#endif



#if BX_CPU_LEVEL >= 2
  void
BX_CPU_C::iret_protected(BxInstruction_t *i)
{
  Bit16u raw_cs_selector, raw_ss_selector;
  bx_selector_t cs_selector, ss_selector;
  Bit32u dword1, dword2;
  bx_descriptor_t cs_descriptor, ss_descriptor;

  if (BX_CPU_THIS_PTR eflags.nt) { /* NT = 1: RETURN FROM NESTED TASK */
    /* what's the deal with NT & VM ? */
    Bit32u base32;
    Bit16u raw_link_selector;
    bx_selector_t   link_selector;
    bx_descriptor_t tss_descriptor;

    if (BX_CPU_THIS_PTR eflags.vm)
      BX_PANIC(("IRET: vm set?\n"));

    // TASK_RETURN:

    //BX_INFO(("IRET: nested task return\n"));

    if (BX_CPU_THIS_PTR tr.cache.valid==0)
      BX_PANIC(("IRET: TR not valid\n"));
    if (BX_CPU_THIS_PTR tr.cache.type == 1)
      base32 = BX_CPU_THIS_PTR tr.cache.u.tss286.base;
    else if (BX_CPU_THIS_PTR tr.cache.type == 9)
      base32 = BX_CPU_THIS_PTR tr.cache.u.tss386.base;
    else {
      BX_PANIC(("IRET: TR not valid\n"));
      base32 = 0; // keep compiler happy
      }

    // examine back link selector in TSS addressed by current TR:
    access_linear(base32 + 0, 2, 0, BX_READ, &raw_link_selector);

    // must specify global, else #TS(new TSS selector)
    parse_selector(raw_link_selector, &link_selector);
    if (link_selector.ti) {
      BX_PANIC(("iret: link selector.ti=1\n"));
      exception(BX_TS_EXCEPTION, raw_link_selector & 0xfffc, 0);
      }

    // index must be within GDT limits, else #TS(new TSS selector)
    fetch_raw_descriptor(&link_selector, &dword1, &dword2, BX_TS_EXCEPTION);

    // AR byte must specify TSS, else #TS(new TSS selector)
    // new TSS must be busy, else #TS(new TSS selector)
    parse_descriptor(dword1, dword2, &tss_descriptor);
    if (tss_descriptor.valid==0 || tss_descriptor.segment) {
      BX_INFO(("iret: TSS selector points to bad TSS\n"));
      exception(BX_TS_EXCEPTION, raw_link_selector & 0xfffc, 0);
      }
    if ((tss_descriptor.type!=11) && (tss_descriptor.type!=3)) {
      BX_INFO(("iret: TSS selector points to bad TSS\n"));
      exception(BX_TS_EXCEPTION, raw_link_selector & 0xfffc, 0);
      }


    // TSS must be present, else #NP(new TSS selector)
    if (tss_descriptor.p==0) {
      BX_INFO(("iret: task descriptor.p == 0\n"));
      exception(BX_NP_EXCEPTION, raw_link_selector & 0xfffc, 0);
      }

    // switch tasks (without nesting) to TSS specified by back link selector
    task_switch(&link_selector, &tss_descriptor,
                BX_TASK_FROM_IRET, dword1, dword2);

    // mark the task just abandoned as not busy

    // eIP must be within code seg limit, else #GP(0)
    if (EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
      BX_PANIC(("iret: eIP > cs.limit\n"));
      exception(BX_GP_EXCEPTION, 0x0000, 0);
      }
    return;
    }

  else { /* NT = 0: INTERRUPT RETURN ON STACK -or STACK_RETURN_TO_V86 */
    Bit16u top_nbytes_same, top_nbytes_outer;
    Bit32u cs_offset, ss_offset;
    Bit32u new_eip, new_esp, temp_ESP, new_eflags;
    Bit16u new_ip, new_sp, new_flags;
    Bit8u prev_cpl;

    /* 16bit opsize  |   32bit opsize
     * ==============================
     * SS     eSP+8  |   SS     eSP+16
     * SP     eSP+6  |   ESP    eSP+12
     * -------------------------------
     * FLAGS  eSP+4  |   EFLAGS eSP+8
     * CS     eSP+2  |   CS     eSP+4
     * IP     eSP+0  |   EIP    eSP+0
     */

    if (i->os_32) {
      top_nbytes_same    = 12;
      top_nbytes_outer   = 20;
      cs_offset = 4;
      ss_offset = 16;
      }
    else {
      top_nbytes_same    = 6;
      top_nbytes_outer   = 10;
      cs_offset = 2;
      ss_offset = 8;
      }

    /* CS on stack must be within stack limits, else #SS(0) */
    if ( !can_pop(top_nbytes_same) ) {
      BX_PANIC(("iret: CS not within stack limits\n"));
      exception(BX_SS_EXCEPTION, 0, 0);
      return;
      }

    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
      temp_ESP = ESP;
    else
      temp_ESP = SP;

    access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + cs_offset,
      2, CPL==3, BX_READ, &raw_cs_selector);

    if (i->os_32) {
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &new_eip);
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 8,
        4, CPL==3, BX_READ, &new_eflags);

      // if VM=1 in flags image on stack then STACK_RETURN_TO_V86
      if (new_eflags & 0x00020000) {
        if (CPL != 0)
          BX_PANIC(("iret: VM set on stack, CPL!=0\n"));
        BX_CPU_THIS_PTR stack_return_to_v86(new_eip, raw_cs_selector, new_eflags);
        return;
        }
      }
    else {
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
        2, CPL==3, BX_READ, &new_ip);
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 4,
        2, CPL==3, BX_READ, &new_flags);
      }

    parse_selector(raw_cs_selector, &cs_selector);

    // return CS selector must be non-null, else #GP(0)
    if ( (raw_cs_selector & 0xfffc) == 0 ) {
      BX_PANIC(("iret: return CS selector null\n"));
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }

    // selector index must be within descriptor table limits,
    // else #GP(return selector)
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
      BX_GP_EXCEPTION);

    parse_descriptor(dword1, dword2, &cs_descriptor);

    // AR byte must indicate code segment else #GP(return selector)
    if ( cs_descriptor.valid==0 ||
         cs_descriptor.segment==0  ||
         cs_descriptor.u.segment.executable==0 ) {
      BX_PANIC(("iret: AR byte indicated non code segment\n"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      return;
      }

    // return CS selector RPL must be >= CPL, else #GP(return selector)
    if (cs_selector.rpl < CPL) {
      BX_PANIC(("iret: return selector RPL < CPL\n"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      return;
      }

    // if return code seg descriptor is conforming
    //   and return code seg DPL > return code seg selector RPL
    //     then #GP(return selector)
    if ( cs_descriptor.u.segment.c_ed  &&
         cs_descriptor.dpl > cs_selector.rpl ) {
      BX_PANIC(("iret: conforming, DPL > cs_selector.RPL\n"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      return;
      }

    // if return code seg descriptor is non-conforming
    //   and return code seg DPL != return code seg selector RPL
    //     then #GP(return selector)
    if ( cs_descriptor.u.segment.c_ed==0 &&
         cs_descriptor.dpl != cs_selector.rpl ) {
      BX_INFO(("(mch) iret: Return with DPL != RPL. #GP(selector)\n"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      return;
      }

    // segment must be present else #NP(return selector)
    if ( cs_descriptor.p==0 ) {
      BX_PANIC(("iret: not present\n"));
      exception(BX_NP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      return;
      }

    if (cs_selector.rpl == CPL) { /* INTERRUPT RETURN TO SAME LEVEL */
      /* top 6/12 bytes on stack must be within limits, else #SS(0) */
      /* satisfied above */

      if (i->os_32) {
        /* return EIP must be in code segment limit else #GP(0) */
        if ( new_eip > cs_descriptor.u.segment.limit_scaled ) {
          BX_PANIC(("iret: IP > descriptor limit\n"));
          exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }
        /* load CS:EIP from stack */
        /* load CS-cache with new code segment descriptor */
        load_cs(&cs_selector, &cs_descriptor, CPL);
        EIP = new_eip;

        /* load EFLAGS with 3rd doubleword from stack */
        write_eflags(new_eflags, CPL==0, CPL<=IOPL, 0, 1);
        }
      else {
        /* return IP must be in code segment limit else #GP(0) */
        if ( new_ip > cs_descriptor.u.segment.limit_scaled ) {
          BX_PANIC(("iret: IP > descriptor limit\n"));
          exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }
        /* load CS:IP from stack */
        /* load CS-cache with new code segment descriptor */
        load_cs(&cs_selector, &cs_descriptor, CPL);
        EIP = new_ip;

        /* load flags with third word on stack */
        write_flags(new_flags, CPL==0, CPL<=IOPL);
        }

      /* increment stack by 6/12 */
      if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
        ESP += top_nbytes_same;
      else
        SP += top_nbytes_same;
      return;
      }
    else { /* INTERRUPT RETURN TO OUTER PRIVILEGE LEVEL */
      /* 16bit opsize  |   32bit opsize
       * ==============================
       * SS     eSP+8  |   SS     eSP+16
       * SP     eSP+6  |   ESP    eSP+12
       * FLAGS  eSP+4  |   EFLAGS eSP+8
       * CS     eSP+2  |   CS     eSP+4
       * IP     eSP+0  |   EIP    eSP+0
       */

      /* top 10/20 bytes on stack must be within limits else #SS(0) */
      if ( !can_pop(top_nbytes_outer) ) {
        BX_PANIC(("iret: top 10/20 bytes not within stack limits\n"));
        exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }

      /* examine return SS selector and associated descriptor */
      access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + ss_offset,
        2, 0, BX_READ, &raw_ss_selector);

      /* selector must be non-null, else #GP(0) */
      if ( (raw_ss_selector & 0xfffc) == 0 ) {
        BX_PANIC(("iret: SS selector null\n"));
        exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }

      parse_selector(raw_ss_selector, &ss_selector);

      /* selector RPL must = RPL of return CS selector,
       * else #GP(SS selector) */
      if ( ss_selector.rpl != cs_selector.rpl) {
        BX_PANIC(("iret: SS.rpl != CS.rpl\n"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        return;
        }

      /* selector index must be within its descriptor table limits,
       * else #GP(SS selector) */
      fetch_raw_descriptor(&ss_selector, &dword1, &dword2,
        BX_GP_EXCEPTION);

      parse_descriptor(dword1, dword2, &ss_descriptor);

      /* AR byte must indicate a writable data segment,
       * else #GP(SS selector) */
      if ( ss_descriptor.valid==0 ||
           ss_descriptor.segment==0  ||
           ss_descriptor.u.segment.executable  ||
           ss_descriptor.u.segment.r_w==0 ) {
        BX_PANIC(("iret: SS AR byte not writable code segment\n"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        return;
        }

      /* stack segment DPL must equal the RPL of the return CS selector,
       * else #GP(SS selector) */
      if ( ss_descriptor.dpl != cs_selector.rpl ) {
        BX_PANIC(("iret: SS.dpl != CS selector RPL\n"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        return;
        }

      /* SS must be present, else #NP(SS selector) */
      if ( ss_descriptor.p==0 ) {
        BX_PANIC(("iret: SS not present!\n"));
        exception(BX_NP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        return;
        }


      if (i->os_32) {
        access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
          4, 0, BX_READ, &new_eip);
        access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 8,
          4, 0, BX_READ, &new_eflags);
        access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 12,
          4, 0, BX_READ, &new_esp);
        }
      else {
        access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 0,
          2, 0, BX_READ, &new_ip);
        access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 4,
          2, 0, BX_READ, &new_flags);
        access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_ESP + 6,
          2, 0, BX_READ, &new_sp);
        new_eip = new_ip;
        new_esp = new_sp;
        new_eflags = new_flags;
        }

      /* EIP must be in code segment limit, else #GP(0) */
      if ( new_eip > cs_descriptor.u.segment.limit_scaled ) {
        BX_PANIC(("iret: IP > descriptor limit\n"));
        exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }

      /* load CS:EIP from stack */
      /* load the CS-cache with CS descriptor */
      /* set CPL to the RPL of the return CS selector */
      prev_cpl = CPL; /* previous CPL */
      load_cs(&cs_selector, &cs_descriptor, cs_selector.rpl);
      BX_CPU_THIS_PTR eip = new_eip;

      /* load flags from stack */
      // perhaps I should always write_eflags(), thus zeroing
      // out the upper 16bits of eflags for CS.D_B==0 ???
      if (cs_descriptor.u.segment.d_b)
        write_eflags(new_eflags, prev_cpl==0, prev_cpl<=IOPL, 0, 1);
      else
        write_flags((Bit16u) new_eflags, prev_cpl==0, prev_cpl<=IOPL);

      // load SS:eSP from stack
      // load the SS-cache with SS descriptor
      load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);
      if (ss_descriptor.u.segment.d_b)
        ESP = new_esp;
      else
        SP  = new_esp;

      validate_seg_regs();

      return;
      }
    }
  BX_PANIC(("IRET: shouldn't get here!\n"));
}
#endif


#if BX_CPU_LEVEL >= 2
  void
BX_CPU_C::validate_seg_regs(void)
{
  if ( BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.dpl<CPL ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = 0;
    }
  if ( BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl<CPL ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = 0;
    }
  if ( BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.dpl<CPL ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value = 0;
    }
  if ( BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.dpl<CPL ) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid = 0;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value = 0;
    }
}
#endif
