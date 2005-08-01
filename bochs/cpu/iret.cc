////////////////////////////////////////////////////////////////////////
// $Id: iret.cc,v 1.1 2005-08-01 22:06:19 sshwarts Exp $
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


  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::iret_protected(bxInstruction_c *i)
{
  Bit16u raw_cs_selector, raw_ss_selector;
  bx_selector_t cs_selector, ss_selector;
  Bit32u dword1, dword2;
  bx_descriptor_t cs_descriptor, ss_descriptor;

  if ( BX_CPU_THIS_PTR get_NT () ) { /* NT = 1: RETURN FROM NESTED TASK */
    /* what's the deal with NT & VM ? */
    Bit32u base32;
    Bit16u raw_link_selector;
    bx_selector_t   link_selector;
    bx_descriptor_t tss_descriptor;

    if ( BX_CPU_THIS_PTR get_VM () )
      BX_PANIC(("iret_protected: VM sholdn't be set here !"));

    //BX_INFO(("IRET: nested task return"));

    if (BX_CPU_THIS_PTR tr.cache.valid==0)
      BX_PANIC(("IRET: TR not valid"));
    if (BX_CPU_THIS_PTR tr.cache.type == BX_SYS_SEGMENT_AVAIL_286_TSS)
      base32 = BX_CPU_THIS_PTR tr.cache.u.tss286.base;
    else if (BX_CPU_THIS_PTR tr.cache.type == BX_SYS_SEGMENT_AVAIL_386_TSS)
      base32 = BX_CPU_THIS_PTR tr.cache.u.tss386.base;
    else {
      BX_PANIC(("IRET: TR not valid"));
      base32 = 0; // keep compiler happy
    }

    // examine back link selector in TSS addressed by current TR:
    access_linear(base32, 2, 0, BX_READ, &raw_link_selector);

    // must specify global, else #TS(new TSS selector)
    parse_selector(raw_link_selector, &link_selector);

    if (link_selector.ti) {
      BX_ERROR(("iret: link selector.ti=1"));
      exception(BX_TS_EXCEPTION, raw_link_selector & 0xfffc, 0);
    }

    // index must be within GDT limits, else #TS(new TSS selector)
    fetch_raw_descriptor(&link_selector, &dword1, &dword2, BX_TS_EXCEPTION);

    // AR byte must specify TSS, else #TS(new TSS selector)
    // new TSS must be busy, else #TS(new TSS selector)
    parse_descriptor(dword1, dword2, &tss_descriptor);
    if (tss_descriptor.valid==0 || tss_descriptor.segment) {
      BX_INFO(("iret: TSS selector points to bad TSS"));
      exception(BX_TS_EXCEPTION, raw_link_selector & 0xfffc, 0);
    }
    if ((tss_descriptor.type!=11) && (tss_descriptor.type!=3)) {
      BX_INFO(("iret: TSS selector points to bad TSS"));
      exception(BX_TS_EXCEPTION, raw_link_selector & 0xfffc, 0);
    }

    // TSS must be present, else #NP(new TSS selector)
    if (! IS_PRESENT(tss_descriptor)) {
      BX_ERROR(("iret: task descriptor.p == 0"));
      exception(BX_NP_EXCEPTION, raw_link_selector & 0xfffc, 0);
    }

    // switch tasks (without nesting) to TSS specified by back link selector
    task_switch(&link_selector, &tss_descriptor,
                BX_TASK_FROM_IRET, dword1, dword2);

    // mark the task just abandoned as not busy

    // EIP must be within code seg limit, else #GP(0)
    if (EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
      BX_ERROR(("iret: EIP > CS.limit"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }

    return;
  }
#if BX_SUPPORT_X86_64
  else if (BX_CPU_THIS_PTR msr.lma) {         // LONG MODE
    //BX_DEBUG (("LONG mode IRET"));
    /* NT = 0: INTERRUPT RETURN ON STACK -or STACK_RETURN_TO_V86 */
    Bit16u top_nbytes_same, top_nbytes_outer;
    Bit64u cs_offset, ss_offset;
    Bit64u new_rip, new_rsp, temp_RSP, new_eflags;
    Bit8u prev_cpl;

    /* 64bit opsize
     * ============
     * SS     eSP+32
     * ESP    eSP+24
     * -------------
     * EFLAGS eSP+16
     * CS     eSP+8
     * EIP    eSP+0
     */

    top_nbytes_same    = 24;
    top_nbytes_outer   = 40;
    cs_offset = 8;
    ss_offset = 32;

    /* CS on stack must be within stack limits, else #SS(0) */
    if ( !can_pop(top_nbytes_same) ) {
      BX_ERROR(("iret: CS not within stack limits"));
      exception(BX_SS_EXCEPTION, 0, 0);
    }

    temp_RSP = RSP;

    access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + cs_offset,
      2, CPL==3, BX_READ, &raw_cs_selector);

    access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 0,
      8, CPL==3, BX_READ, &new_rip);
    access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 16,
      8, CPL==3, BX_READ, &new_eflags);

    // if VM=1 in flags image on stack then STACK_RETURN_TO_V86
    if (new_eflags & 0x00020000) {
      BX_PANIC(("iret: no V86 mode in x86-64 LONG mode"));
      return;
    }

    parse_selector(raw_cs_selector, &cs_selector);

    // return CS selector must be non-null, else #GP(0)
    if ( (raw_cs_selector & 0xfffc) == 0 ) {
      BX_ERROR(("iret: return CS selector null"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }

    // selector index must be within descriptor table limits,
    // else #GP(return selector)
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);

    parse_descriptor(dword1, dword2, &cs_descriptor);

    // AR byte must indicate code segment else #GP(return selector)
    if ( cs_descriptor.valid==0 ||
         cs_descriptor.segment==0  ||
         cs_descriptor.u.segment.executable==0 ) 
    {
      BX_ERROR(("iret: AR byte indicated non code segment (%x) %x:%x",
         raw_cs_selector, dword1, dword2));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
    }

    // return CS selector RPL must be >= CPL, else #GP(return selector)
    if (cs_selector.rpl < CPL) {
      BX_ERROR(("iret: return selector RPL < CPL"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
    }

    if (cs_descriptor.u.segment.c_ed) {
      // if return code seg descriptor is conforming
      //   and return code seg DPL > return code seg selector RPL
      //     then #GP(return selector)
      if (cs_descriptor.dpl > cs_selector.rpl)
      {
        BX_PANIC(("iret: conforming, DPL > cs_selector.RPL"));
        exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      }
    }
    else {
      // if return code seg descriptor is non-conforming
      //   and return code seg DPL != return code seg selector RPL
      //     then #GP(return selector)
      if (cs_descriptor.dpl != cs_selector.rpl)
      {
        BX_INFO(("iret: Return with DPL != RPL. #GP(selector)"));
        exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      }
    }

    // segment must be present else #NP(return selector)
    if (! IS_PRESENT(cs_descriptor)) {
      BX_ERROR(("iret: not present"));
      exception(BX_NP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
    }

    if (cs_selector.rpl == CPL
          && BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64) 
    { /* INTERRUPT RETURN TO SAME LEVEL */
      /* top 24 bytes on stack must be within limits, else #SS(0) */
      /* satisfied above */

      /* load CS:EIP from stack */
      /* load CS-cache with new code segment descriptor */
      load_cs(&cs_selector, &cs_descriptor, CPL);
      RIP = new_rip;

      /* load EFLAGS with 3rd doubleword from stack */
      write_eflags(new_eflags, CPL==0, CPL<=BX_CPU_THIS_PTR get_IOPL(), 0, 1);

      /* increment stack by 24 */
      RSP += top_nbytes_same;
    }
    else { /* INTERRUPT RETURN TO OUTER PRIVILEGE LEVEL or 64 bit mode */
      /* 64bit opsize
       * ============
       * SS     eSP+32
       * ESP    eSP+24
       * EFLAGS eSP+16
       * CS     eSP+8
       * EIP    eSP+0
       */

      /* top 10/20 bytes on stack must be within limits else #SS(0) */
      if ( !can_pop(top_nbytes_outer) ) {
        BX_PANIC(("iret: top 10/20 bytes not within stack limits"));
        exception(BX_SS_EXCEPTION, 0, 0);
      }

      /* examine return SS selector and associated descriptor */
      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + ss_offset,
        2, 0, BX_READ, &raw_ss_selector);

      if ( (raw_ss_selector & 0xfffc) != 0 ) {
        parse_selector(raw_ss_selector, &ss_selector);

        /* selector RPL must = RPL of return CS selector,
         * else #GP(SS selector) */
        if ( ss_selector.rpl != cs_selector.rpl) {
          BX_PANIC(("iret: SS.rpl != CS.rpl"));
          exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        }

        /* selector index must be within its descriptor table limits,
         * else #GP(SS selector) */
        fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);

        parse_descriptor(dword1, dword2, &ss_descriptor);

        /* AR byte must indicate a writable data segment,
         * else #GP(SS selector) */
        if ( ss_descriptor.valid==0 ||
             ss_descriptor.segment==0  ||
             ss_descriptor.u.segment.executable  ||
             ss_descriptor.u.segment.r_w==0 )
        {
          BX_PANIC(("iret: SS AR byte not writable code segment"));
          exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        }

        /* stack segment DPL must equal the RPL of the return CS selector,
         * else #GP(SS selector) */
        if ( ss_descriptor.dpl != cs_selector.rpl ) {
          BX_PANIC(("iret: SS.dpl != CS selector RPL"));
          exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        }

        /* SS must be present, else #NP(SS selector) */
        if (! IS_PRESENT(ss_descriptor)) {
          BX_ERROR(("iret: SS not present!"));
          exception(BX_NP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        }
      }

      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 0,
        8, 0, BX_READ, &new_rip);
      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 16,
        4, 0, BX_READ, &new_eflags);
      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 24,
        8, 0, BX_READ, &new_rsp);

      prev_cpl = CPL; /* previous CPL */

      /* set CPL to the RPL of the return CS selector */
      branch_far64(&cs_selector, &cs_descriptor, new_rip, cs_selector.rpl);

      /* load flags from stack */
      // perhaps I should always write_eflags(), thus zeroing
      // out the upper 16bits of eflags for CS.D_B==0 ???
      if (cs_descriptor.u.segment.d_b)
        write_eflags(new_eflags, prev_cpl==0, prev_cpl<=BX_CPU_THIS_PTR get_IOPL(), 0, 1);
      else
        write_flags((Bit16u) new_eflags, prev_cpl==0, prev_cpl<=BX_CPU_THIS_PTR get_IOPL());

      if ( (raw_ss_selector & 0xfffc) != 0 ) {
        // load SS:RSP from stack
        // load the SS-cache with SS descriptor
        load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);
      }
      else {
        loadSRegLMNominal(BX_SEG_REG_SS, raw_ss_selector, 0, cs_selector.rpl);
      }
      RSP = new_rsp;

      validate_seg_regs();

      return;
    }
  }
#endif  // #if BX_SUPPORT_X86_64
  else {
    /* NT = 0: INTERRUPT RETURN ON STACK -or STACK_RETURN_TO_V86 */
    Bit16u top_nbytes_same, top_nbytes_outer;
    Bit32u cs_offset, ss_offset;
    Bit32u new_eip, new_esp, temp_RSP, new_eflags;
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

    if (i->os32L()) {
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
      BX_ERROR(("iret: CS not within stack limits"));
      exception(BX_SS_EXCEPTION, 0, 0);
    }

    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
      temp_RSP = ESP;
    else
      temp_RSP = SP;

    access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + cs_offset,
      2, CPL==3, BX_READ, &raw_cs_selector);

    if (i->os32L()) {
      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 0,
        4, CPL==3, BX_READ, &new_eip);
      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 8,
        4, CPL==3, BX_READ, &new_eflags);

      // if VM=1 in flags image on stack then STACK_RETURN_TO_V86
      if (new_eflags & 0x00020000) {
        if (CPL == 0) {
          BX_CPU_THIS_PTR stack_return_to_v86(new_eip, raw_cs_selector, new_eflags);
          return;
        }
        else BX_INFO(("iret: VM set on stack, CPL!=0"));
      }
    }
    else {
      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 0,
        2, CPL==3, BX_READ, &new_ip);
      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 4,
        2, CPL==3, BX_READ, &new_flags);
    }

    parse_selector(raw_cs_selector, &cs_selector);

    // return CS selector must be non-null, else #GP(0)
    if ( (raw_cs_selector & 0xfffc) == 0 ) {
      BX_ERROR(("iret: return CS selector null"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }

    // selector index must be within descriptor table limits,
    // else #GP(return selector)
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);

    parse_descriptor(dword1, dword2, &cs_descriptor);

    // AR byte must indicate code segment else #GP(return selector)
    if ( cs_descriptor.valid==0 ||
         cs_descriptor.segment==0  ||
         cs_descriptor.u.segment.executable==0 )
    {
      BX_ERROR(("iret: AR byte indicated non code segment"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
    }

    // return CS selector RPL must be >= CPL, else #GP(return selector)
    if (cs_selector.rpl < CPL) {
      BX_ERROR(("iret: return selector RPL < CPL"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
    }

    if (cs_descriptor.u.segment.c_ed)
    {
      // if return code seg descriptor is conforming
      //   and return code seg DPL > return code seg selector RPL
      //     then #GP(return selector)
      if (cs_descriptor.dpl > cs_selector.rpl) {
        BX_PANIC(("iret: conforming, DPL > cs_selector.RPL"));
        exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      }
    }
    else {
      // if return code seg descriptor is non-conforming
      //   and return code seg DPL != return code seg selector RPL
      //     then #GP(return selector)
      if (cs_descriptor.dpl != cs_selector.rpl) {
        BX_INFO(("iret: Return with DPL != RPL. #GP(selector)"));
        exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      }
    }

    // segment must be present else #NP(return selector)
    if (! IS_PRESENT(cs_descriptor)) {
      BX_ERROR(("iret: segment not present"));
      exception(BX_NP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
    }

    if (cs_selector.rpl == CPL) { /* INTERRUPT RETURN TO SAME LEVEL */
      /* top 6/12 bytes on stack must be within limits, else #SS(0) */
      /* satisfied above */

      if (i->os32L()) {
        /* return EIP must be in code segment limit else #GP(0) */
        if ( new_eip > cs_descriptor.u.segment.limit_scaled ) {
          BX_ERROR(("iret: IP > descriptor limit"));
          exception(BX_GP_EXCEPTION, 0, 0);
        }
        /* load CS:EIP from stack */
        /* load CS-cache with new code segment descriptor */
        load_cs(&cs_selector, &cs_descriptor, CPL);
        EIP = new_eip;

        /* load EFLAGS with 3rd doubleword from stack */
        write_eflags(new_eflags, CPL==0, CPL<=BX_CPU_THIS_PTR get_IOPL (), 0, 1);
      }
      else {
        /* return IP must be in code segment limit else #GP(0) */
        if ( new_ip > cs_descriptor.u.segment.limit_scaled ) {
          BX_ERROR(("iret: IP > descriptor limit"));
          exception(BX_GP_EXCEPTION, 0, 0);
        }
        /* load CS:IP from stack */
        /* load CS-cache with new code segment descriptor */
        load_cs(&cs_selector, &cs_descriptor, CPL);
        EIP = new_ip;

        /* load flags with third word on stack */
        write_flags(new_flags, CPL==0, CPL<=BX_CPU_THIS_PTR get_IOPL ());
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
        BX_ERROR(("iret: top 10/20 bytes not within stack limits"));
        exception(BX_SS_EXCEPTION, 0, 0);
      }

      /* examine return SS selector and associated descriptor */
      access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + ss_offset,
        2, 0, BX_READ, &raw_ss_selector);

      /* selector must be non-null, else #GP(0) */
      if ( (raw_ss_selector & 0xfffc) == 0 ) {
        BX_ERROR(("iret: SS selector null"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }

      parse_selector(raw_ss_selector, &ss_selector);

      /* selector RPL must = RPL of return CS selector,
       * else #GP(SS selector) */
      if ( ss_selector.rpl != cs_selector.rpl) {
        BX_ERROR(("iret: SS.rpl != CS.rpl"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
      }

      /* selector index must be within its descriptor table limits,
       * else #GP(SS selector) */
      fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);

      parse_descriptor(dword1, dword2, &ss_descriptor);

      /* AR byte must indicate a writable data segment,
       * else #GP(SS selector) */
      if ( ss_descriptor.valid==0 ||
           ss_descriptor.segment==0  ||
           ss_descriptor.u.segment.executable  ||
           ss_descriptor.u.segment.r_w==0 )
      {
        BX_ERROR(("iret: SS AR byte not writable code segment"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
      }

      /* stack segment DPL must equal the RPL of the return CS selector,
       * else #GP(SS selector) */
      if ( ss_descriptor.dpl != cs_selector.rpl ) {
        BX_ERROR(("iret: SS.dpl != CS selector RPL"));
        exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
      }

      /* SS must be present, else #NP(SS selector) */
      if (! IS_PRESENT(ss_descriptor)) {
        BX_ERROR(("iret: SS not present!"));
        exception(BX_NP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
      }

      if (i->os32L()) {
        access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 0,
          4, 0, BX_READ, &new_eip);
        access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 8,
          4, 0, BX_READ, &new_eflags);
        access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 12,
          4, 0, BX_READ, &new_esp);
      }
      else {
        access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 0,
          2, 0, BX_READ, &new_ip);
        access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 4,
          2, 0, BX_READ, &new_flags);
        access_linear(BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_SS) + temp_RSP + 6,
          2, 0, BX_READ, &new_sp);
        new_eip = new_ip;
        new_esp = new_sp;
        new_eflags = new_flags;
      }

      /* EIP must be in code segment limit, else #GP(0) */
      if ( new_eip > cs_descriptor.u.segment.limit_scaled ) {
        BX_ERROR(("iret: IP > descriptor limit"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }

      /* load CS:EIP from stack */
      /* load the CS-cache with CS descriptor */
      /* set CPL to the RPL of the return CS selector */
      prev_cpl = CPL; /* previous CPL */
      load_cs(&cs_selector, &cs_descriptor, cs_selector.rpl);
      EIP = new_eip;

      /* load flags from stack */
      // perhaps I should always write_eflags(), thus zeroing
      // out the upper 16bits of eflags for CS.D_B==0 ???
      if (cs_descriptor.u.segment.d_b)
        write_eflags(new_eflags, prev_cpl==0, prev_cpl<=BX_CPU_THIS_PTR get_IOPL (), 0, 1);
      else
        write_flags((Bit16u) new_eflags, prev_cpl==0, prev_cpl<=BX_CPU_THIS_PTR get_IOPL ());

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

  BX_PANIC(("IRET: shouldn't get here!"));
}
