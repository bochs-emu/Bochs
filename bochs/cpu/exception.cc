/////////////////////////////////////////////////////////////////////////
// $Id: exception.cc,v 1.25 2002-10-03 04:52:39 bdenney Exp $
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



#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR


/* Exception classes.  These are used as indexes into the 'is_exception_OK'
 * array below, and are stored in the 'exception' array also
 */
#define BX_ET_BENIGN       0
#define BX_ET_CONTRIBUTORY 1
#define BX_ET_PAGE_FAULT   2

#define BX_ET_DOUBLE_FAULT 10


const Boolean BX_CPU_C::is_exception_OK[3][3] = {
    { 1, 1, 1 }, /* 1st exception is BENIGN */
    { 1, 0, 1 }, /* 1st exception is CONTRIBUTORY */
    { 1, 0, 0 }  /* 1st exception is PAGE_FAULT */
    };


  void
BX_CPU_C::interrupt(Bit8u vector, Boolean is_INT, Boolean is_error_code,
                    Bit16u error_code)
{
#if BX_DEBUGGER
  if (bx_guard.special_unwind_stack) {
    BX_INFO (("interrupt() returning early because special_unwind_stack is set"));
    return;
  }
  BX_CPU_THIS_PTR show_flag |= Flag_intsig;
#if BX_DEBUG_LINUX
  if (bx_dbg.linux_syscall) {
    if (vector == 0x80) bx_dbg_linux_syscall ();
  }
#endif
#endif

//BX_DEBUG(( "::interrupt(%u)", vector ));

  BX_INSTR_INTERRUPT(CPU_ID, vector);
  invalidate_prefetch_q();

  // Discard any traps and inhibits for new context; traps will
  // resume upon return.
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR inhibit_mask = 0;

#if BX_CPU_LEVEL >= 2
//  unsigned prev_errno;

  BX_DEBUG(("interrupt(): vector = %u, INT = %u, EXT = %u",
      (unsigned) vector, (unsigned) is_INT, (unsigned) BX_CPU_THIS_PTR EXT));

BX_CPU_THIS_PTR save_cs  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS];
BX_CPU_THIS_PTR save_ss  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS];
BX_CPU_THIS_PTR save_eip = EIP;
BX_CPU_THIS_PTR save_esp = ESP;

//  prev_errno = BX_CPU_THIS_PTR errorno;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR msr.lma) {
    // long mode interrupt

    Bit64u idtindex;
    Bit32u save_upper;
    Bit32u  dword1, dword2, dword3;

    bx_descriptor_t gate_descriptor, cs_descriptor;
    bx_selector_t cs_selector;

    Bit16u gate_dest_selector;
    Bit64u gate_dest_offset;

    // interrupt vector must be within IDT table limits,
    // else #GP(vector number*16 + 2 + EXT)
    idtindex = vector*16;
    if ( (idtindex + 15) > BX_CPU_THIS_PTR idtr.limit) {
      BX_DEBUG(("IDT.limit = %04x", (unsigned) BX_CPU_THIS_PTR idtr.limit));
      BX_DEBUG(("IDT.base  = %06x", (unsigned) BX_CPU_THIS_PTR idtr.base));
      BX_DEBUG(("interrupt vector must be within IDT table limits"));
      BX_DEBUG(("bailing"));
      BX_DEBUG(("interrupt(): vector > idtr.limit"));

      exception(BX_GP_EXCEPTION, vector*16 + 2, 0);
      }
    // descriptor AR byte must indicate interrupt gate, trap gate,
    // or task gate, else #GP(vector*8 + 2 + EXT)

    idtindex += BX_CPU_THIS_PTR idtr.base;

    access_linear(idtindex,     4, 0,
      BX_READ, &dword1);
    access_linear(idtindex + 4, 4, 0,
      BX_READ, &dword2);
    access_linear(idtindex + 8, 4, 0,
      BX_READ, &dword3);

    parse_descriptor(dword1, dword2, &gate_descriptor);

    if ( (gate_descriptor.valid==0) || gate_descriptor.segment) {
      BX_DEBUG(("interrupt(): gate descriptor is not valid sys seg"));
      exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      }
    switch (gate_descriptor.type) {
      //case 5: // task gate
      //case 6: // 286 interrupt gate
      //case 7: // 286 trap gate
      case 14: // 386 interrupt gate
      case 15: // 386 trap gate
        break;
      default:
        BX_DEBUG(("interrupt(): gate.type(%u) != {5,6,7,14,15}",
          (unsigned) gate_descriptor.type));
        exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
        return;
      }

    // if software interrupt, then gate descripor DPL must be >= CPL,
    // else #GP(vector * 8 + 2 + EXT)
    if (is_INT  &&  (gate_descriptor.dpl < CPL)) {
/* ??? */
      BX_DEBUG(("interrupt(): is_INT && (dpl < CPL)"));
      exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      return;
      }

    // Gate must be present, else #NP(vector * 8 + 2 + EXT)
    if (gate_descriptor.p == 0) {
      BX_DEBUG(("interrupt(): p == 0"));
      exception(BX_NP_EXCEPTION, vector*8 + 2, 0);
      }
    gate_dest_selector = gate_descriptor.u.gate386.dest_selector;
    gate_dest_offset   = ((Bit64u)dword3 << 32) +
                         gate_descriptor.u.gate386.dest_offset;

    // examine CS selector and descriptor given in gate descriptor
    // selector must be non-null else #GP(EXT)
    if ( (gate_dest_selector & 0xfffc) == 0 ) {
      BX_PANIC(("int_trap_gate(): selector null"));
      exception(BX_GP_EXCEPTION, 0, 0);
      }

    parse_selector(gate_dest_selector, &cs_selector);

    // selector must be within its descriptor table limits
    // else #GP(selector+EXT)
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
                            BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);

    // descriptor AR byte must indicate code seg
    // and code segment descriptor DPL<=CPL, else #GP(selector+EXT)
    if ( cs_descriptor.valid==0 ||
         cs_descriptor.segment==0 ||
         cs_descriptor.u.segment.executable==0 ||
         cs_descriptor.dpl>CPL
          ) {
      BX_DEBUG(("interrupt(): not code segment"));
      exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
      }
    // check that it's a 64 bit segment
    if ( cs_descriptor.u.segment.l == 0 ||
         cs_descriptor.u.segment.d_b == 1) {
      BX_DEBUG(("interrupt(): must be 64 bit segment"));
      exception(BX_GP_EXCEPTION, vector, 0);
    }

    // segment must be present, else #NP(selector + EXT)
    if ( cs_descriptor.p==0 ) {
      BX_DEBUG(("interrupt(): segment not present"));
      exception(BX_NP_EXCEPTION, cs_selector.value & 0xfffc, 0);
      }

    // if code segment is non-conforming and DPL < CPL then
    // INTERRUPT TO INNER PRIVILEGE:
    if ( cs_descriptor.u.segment.c_ed==0 && cs_descriptor.dpl<CPL ) {
      Bit16u old_SS, old_CS;
      Bit64u RSP_for_cpl_x, old_RIP, old_RSP;
      bx_descriptor_t ss_descriptor;
      bx_selector_t   ss_selector;
      int bytes;

      BX_DEBUG(("interrupt(): INTERRUPT TO INNER PRIVILEGE"));

      // check selector and descriptor for new stack in current TSS
      get_RSP_from_TSS(cs_descriptor.dpl,&RSP_for_cpl_x);
      // set up a null descriptor
      parse_selector(0,&ss_selector);
      parse_descriptor(0,0,&ss_descriptor);


      // 386 int/trap gate
      // new stack must have room for 40|48 bytes, else #SS(0)
      //if ( is_error_code )
      //  bytes = 48;
      //else
      //  bytes = 40;


      old_RSP = RSP;

      // load new RSP values from TSS

      load_ss(&ss_selector, &ss_descriptor, cs_descriptor.dpl);

      RSP = RSP_for_cpl_x;

      // load new CS:IP values from gate
      // set CPL to new code segment DPL
      // set RPL of CS to CPL

      // push long pointer to old stack onto new stack

      // align ESP

      push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
      push_64(old_RSP);

      // push EFLAGS
      push_64(read_eflags());

      // push long pointer to return address onto new stack
      push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
      push_64(RIP);
      if ( is_error_code )
        push_64(error_code);

      load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);
      RIP = gate_dest_offset;


      // if INTERRUPT GATE set IF to 0
      if ( !(gate_descriptor.type & 1) ) // even is int-gate
        BX_CPU_THIS_PTR clear_IF ();
      BX_CPU_THIS_PTR clear_TF ();
      BX_CPU_THIS_PTR clear_VM ();
      BX_CPU_THIS_PTR clear_RF ();
      BX_CPU_THIS_PTR clear_NT ();
      return;
      }

    // if code segment is conforming OR code segment DPL = CPL then
    // INTERRUPT TO SAME PRIVILEGE LEVEL:
    if ( cs_descriptor.u.segment.c_ed==1 || cs_descriptor.dpl==CPL ) {
      Bit64u old_RSP;


      BX_DEBUG(("int_trap_gate286(): INTERRUPT TO SAME PRIVILEGE"));

      old_RSP = RSP;
      // align stack
      RSP = RSP & BX_CONST64(0xfffffffffffffff0);

      // push flags onto stack
      // push current CS selector onto stack
      // push return offset onto stack
      push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
      push_64(old_RSP);
      push_64(read_eflags());
      push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
      push_64(RIP);
      if ( is_error_code )
        push_64(error_code);

      // load CS:IP from gate
      // load CS descriptor
      // set the RPL field of CS to CPL
      load_cs(&cs_selector, &cs_descriptor, CPL);
      RIP = gate_dest_offset;

      // if interrupt gate then set IF to 0
      if ( !(gate_descriptor.type & 1) ) // even is int-gate
        BX_CPU_THIS_PTR clear_IF ();
      BX_CPU_THIS_PTR clear_TF ();
      BX_CPU_THIS_PTR clear_VM ();
      BX_CPU_THIS_PTR clear_RF ();
      BX_CPU_THIS_PTR clear_NT ();
      return;
      }

    // else #GP(CS selector + ext)
    BX_DEBUG(("interrupt: bad descriptor"));
    BX_DEBUG(("c_ed=%u, descriptor.dpl=%u, CPL=%u",
      (unsigned) cs_descriptor.u.segment.c_ed,
      (unsigned) cs_descriptor.dpl,
      (unsigned) CPL));
    BX_DEBUG(("cs.segment = %u", (unsigned) cs_descriptor.segment));
    exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
    }
  else
#endif  // #if BX_SUPPORT_X86_64
  if(!real_mode()) {
    Bit32u  dword1, dword2;
    bx_descriptor_t gate_descriptor, cs_descriptor;
    bx_selector_t cs_selector;

    Bit16u raw_tss_selector;
    bx_selector_t   tss_selector;
    bx_descriptor_t tss_descriptor;

    Bit16u gate_dest_selector;
    Bit32u gate_dest_offset;


    // interrupt vector must be within IDT table limits,
    // else #GP(vector number*8 + 2 + EXT)
    if ( (vector*8 + 7) > BX_CPU_THIS_PTR idtr.limit) {
      BX_DEBUG(("IDT.limit = %04x", (unsigned) BX_CPU_THIS_PTR idtr.limit));
      BX_DEBUG(("IDT.base  = %06x", (unsigned) BX_CPU_THIS_PTR idtr.base));
      BX_DEBUG(("interrupt vector must be within IDT table limits"));
      BX_DEBUG(("bailing"));
      BX_DEBUG(("interrupt(): vector > idtr.limit"));

      exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      }

    // descriptor AR byte must indicate interrupt gate, trap gate,
    // or task gate, else #GP(vector*8 + 2 + EXT)
    access_linear(BX_CPU_THIS_PTR idtr.base + vector*8,     4, 0,
      BX_READ, &dword1);
    access_linear(BX_CPU_THIS_PTR idtr.base + vector*8 + 4, 4, 0,
      BX_READ, &dword2);

    parse_descriptor(dword1, dword2, &gate_descriptor);

    if ( (gate_descriptor.valid==0) || gate_descriptor.segment) {
      BX_DEBUG(("interrupt(): gate descriptor is not valid sys seg"));
      exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      }

    switch (gate_descriptor.type) {
      case 5: // task gate
      case 6: // 286 interrupt gate
      case 7: // 286 trap gate
      case 14: // 386 interrupt gate
      case 15: // 386 trap gate
        break;
      default:
        BX_DEBUG(("interrupt(): gate.type(%u) != {5,6,7,14,15}",
          (unsigned) gate_descriptor.type));
        exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
        return;
      }

    // if software interrupt, then gate descripor DPL must be >= CPL,
    // else #GP(vector * 8 + 2 + EXT)
    if (is_INT  &&  (gate_descriptor.dpl < CPL)) {
/* ??? */
      BX_DEBUG(("interrupt(): is_INT && (dpl < CPL)"));
      exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      return;
      }

    // Gate must be present, else #NP(vector * 8 + 2 + EXT)
    if (gate_descriptor.p == 0) {
      BX_DEBUG(("interrupt(): p == 0"));
      exception(BX_NP_EXCEPTION, vector*8 + 2, 0);
      }

    switch (gate_descriptor.type) {
      case 5: // 286/386 task gate
        // examine selector to TSS, given in task gate descriptor
        raw_tss_selector = gate_descriptor.u.taskgate.tss_selector;
        parse_selector(raw_tss_selector, &tss_selector);

        // must specify global in the local/global bit,
        //      else #TS(TSS selector)
// +++
// 486/Pent books say #TSS(selector)
// PPro+ says #GP(selector)
        if (tss_selector.ti) {
          BX_PANIC(("interrupt: tss_selector.ti=1"));
          exception(BX_TS_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }

        // index must be within GDT limits, else #TS(TSS selector)
        fetch_raw_descriptor(&tss_selector, &dword1, &dword2,
          BX_TS_EXCEPTION);

        // AR byte must specify available TSS,
        //   else #TS(TSS selector)
        parse_descriptor(dword1, dword2, &tss_descriptor);
        if (tss_descriptor.valid==0 || tss_descriptor.segment) {
          BX_PANIC(("exception: TSS selector points to bad TSS"));
          exception(BX_TS_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }
        if (tss_descriptor.type!=9 && tss_descriptor.type!=1) {
          BX_PANIC(("exception: TSS selector points to bad TSS"));
          exception(BX_TS_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }


        // TSS must be present, else #NP(TSS selector)
        // done in task_switch()

        // switch tasks with nesting to TSS
        task_switch(&tss_selector, &tss_descriptor,
                    BX_TASK_FROM_CALL_OR_INT, dword1, dword2);

        // if interrupt was caused by fault with error code
        //   stack limits must allow push of 2 more bytes, else #SS(0)
        // push error code onto stack

        //??? push_16 vs push_32
        if ( is_error_code ) {
          //if (tss_descriptor.type==9)
          if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b)
            push_32(error_code);
          else
            push_16(error_code);
          }

        // instruction pointer must be in CS limit, else #GP(0)
        //if (EIP > cs_descriptor.u.segment.limit_scaled) {}
        if (EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
          BX_PANIC(("exception(): eIP > CS.limit"));
          exception(BX_GP_EXCEPTION, 0x0000, 0);
          }
        return;
        break;

      case 6: // 286 interrupt gate
      case 7: // 286 trap gate
      case 14: // 386 interrupt gate
      case 15: // 386 trap gate
        if ( gate_descriptor.type >= 14 ) { // 386 gate
          gate_dest_selector = gate_descriptor.u.gate386.dest_selector;
          gate_dest_offset   = gate_descriptor.u.gate386.dest_offset;
          }
        else { // 286 gate
          gate_dest_selector = gate_descriptor.u.gate286.dest_selector;
          gate_dest_offset   = gate_descriptor.u.gate286.dest_offset;
          }

        // examine CS selector and descriptor given in gate descriptor
        // selector must be non-null else #GP(EXT)
        if ( (gate_dest_selector & 0xfffc) == 0 ) {
          BX_PANIC(("int_trap_gate(): selector null"));
          exception(BX_GP_EXCEPTION, 0, 0);
          }

        parse_selector(gate_dest_selector, &cs_selector);

        // selector must be within its descriptor table limits
        // else #GP(selector+EXT)
        fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
                                BX_GP_EXCEPTION);
        parse_descriptor(dword1, dword2, &cs_descriptor);

        // descriptor AR byte must indicate code seg
        // and code segment descriptor DPL<=CPL, else #GP(selector+EXT)
        if ( cs_descriptor.valid==0 ||
             cs_descriptor.segment==0 ||
             cs_descriptor.u.segment.executable==0 ||
             cs_descriptor.dpl>CPL ) {
          BX_DEBUG(("interrupt(): not code segment"));
          exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
          }

        // segment must be present, else #NP(selector + EXT)
        if ( cs_descriptor.p==0 ) {
          BX_DEBUG(("interrupt(): segment not present"));
          exception(BX_NP_EXCEPTION, cs_selector.value & 0xfffc, 0);
          }

        // if code segment is non-conforming and DPL < CPL then
        // INTERRUPT TO INNER PRIVILEGE:
        if ( cs_descriptor.u.segment.c_ed==0 && cs_descriptor.dpl<CPL ) {
          Bit16u old_SS, old_CS, SS_for_cpl_x;
          Bit32u ESP_for_cpl_x, old_EIP, old_ESP;
          bx_descriptor_t ss_descriptor;
          bx_selector_t   ss_selector;
          int bytes;

          BX_DEBUG(("interrupt(): INTERRUPT TO INNER PRIVILEGE"));

          // check selector and descriptor for new stack in current TSS
          get_SS_ESP_from_TSS(cs_descriptor.dpl,
                              &SS_for_cpl_x, &ESP_for_cpl_x);

          // Selector must be non-null else #TS(EXT)
          if ( (SS_for_cpl_x & 0xfffc) == 0 ) {
            BX_PANIC(("interrupt(): SS selector null"));
            /* TS(ext) */
            exception(BX_TS_EXCEPTION, 0, 0);
            }

          // selector index must be within its descriptor table limits
          // else #TS(SS selector + EXT)
          parse_selector(SS_for_cpl_x, &ss_selector);
          // fetch 2 dwords of descriptor; call handles out of limits checks
          fetch_raw_descriptor(&ss_selector, &dword1, &dword2,
                                  BX_TS_EXCEPTION);
          parse_descriptor(dword1, dword2, &ss_descriptor);

          // selector rpl must = dpl of code segment,
          // else #TS(SS selector + ext)
          if (ss_selector.rpl != cs_descriptor.dpl) {
            BX_PANIC(("interrupt(): SS.rpl != CS.dpl"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            }

          // stack seg DPL must = DPL of code segment,
          // else #TS(SS selector + ext)
          if (ss_descriptor.dpl != cs_descriptor.dpl) {
            BX_PANIC(("interrupt(): SS.dpl != CS.dpl"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            }

          // descriptor must indicate writable data segment,
          // else #TS(SS selector + EXT)
          if (ss_descriptor.valid==0 ||
              ss_descriptor.segment==0  ||
              ss_descriptor.u.segment.executable==1  ||
              ss_descriptor.u.segment.r_w==0) {
            BX_PANIC(("interrupt(): SS not writable data segment"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            }

          // seg must be present, else #SS(SS selector + ext)
          if (ss_descriptor.p==0) {
            BX_PANIC(("interrupt(): SS not present"));
            exception(BX_SS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            }

          if (gate_descriptor.type>=14) {
            // 386 int/trap gate
            // new stack must have room for 20|24 bytes, else #SS(0)
            if ( is_error_code )
              bytes = 24;
            else
              bytes = 20;
            if (v8086_mode())
              bytes += 16;
            }
          else {
            // new stack must have room for 10|12 bytes, else #SS(0)
            if ( is_error_code )
              bytes = 12;
            else
              bytes = 10;
            if (v8086_mode()) {
              bytes += 8;
              BX_PANIC(("interrupt: int/trap gate VM"));
              }
            }

// 486,Pentium books
// new stack must have room for 10/12 bytes, else #SS(0) 486 book
// PPro+
// new stack must have room for 10/12 bytes, else #SS(seg selector)
          if ( !can_push(&ss_descriptor, ESP_for_cpl_x, bytes) ) {
            BX_PANIC(("interrupt(): new stack doesn't have room for %u bytes",
               (unsigned) bytes));
            // SS(???)
            }

          // IP must be within CS segment boundaries, else #GP(0)
          if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
            BX_PANIC(("interrupt(): gate eIP > CS.limit"));
            exception(BX_GP_EXCEPTION, 0, 0);
            }

          old_ESP = ESP;
          old_SS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
          old_EIP = EIP;
          old_CS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

          // load new SS:SP values from TSS
          load_ss(&ss_selector, &ss_descriptor, cs_descriptor.dpl);

          if (ss_descriptor.u.segment.d_b)
            ESP = ESP_for_cpl_x;
          else
            SP = ESP_for_cpl_x; // leave upper 16bits

          // load new CS:IP values from gate
          // set CPL to new code segment DPL
          // set RPL of CS to CPL
          load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);
          EIP = gate_dest_offset;

          if (gate_descriptor.type>=14) { // 386 int/trap gate
            if (v8086_mode()) {
              push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
              push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
              push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
              push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid = 0;
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value = 0;
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid = 0;
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value = 0;
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 0;
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = 0;
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
              BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = 0;
              }
            // push long pointer to old stack onto new stack
            push_32(old_SS);
            push_32(old_ESP);

            // push EFLAGS
            push_32(read_eflags());

            // push long pointer to return address onto new stack
            push_32(old_CS);
            push_32(old_EIP);

            if ( is_error_code )
              push_32(error_code);
            }
          else { // 286 int/trap gate
            if (v8086_mode()) {
              BX_PANIC(("286 int/trap gate, VM"));
              }
            // push long pointer to old stack onto new stack
            push_16(old_SS);
            push_16(old_ESP); // ignores upper 16bits

            // push FLAGS
            push_16(read_flags());

            // push return address onto new stack
            push_16(old_CS);
            push_16(old_EIP); // ignores upper 16bits

            if ( is_error_code )
              push_16(error_code);
            }

          // if INTERRUPT GATE set IF to 0
          if ( !(gate_descriptor.type & 1) ) // even is int-gate
            BX_CPU_THIS_PTR clear_IF ();
          BX_CPU_THIS_PTR clear_TF ();
          BX_CPU_THIS_PTR clear_VM ();
          BX_CPU_THIS_PTR clear_RF ();
          BX_CPU_THIS_PTR clear_NT ();
          return;
          }

        if (v8086_mode()) {
          exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
          }

        // if code segment is conforming OR code segment DPL = CPL then
        // INTERRUPT TO SAME PRIVILEGE LEVEL:
        if ( cs_descriptor.u.segment.c_ed==1 || cs_descriptor.dpl==CPL ) {
          int bytes;
          Bit32u temp_ESP;

          if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
            temp_ESP = ESP;
          else
            temp_ESP = SP;

          BX_DEBUG(("int_trap_gate286(): INTERRUPT TO SAME PRIVILEGE"));

          // Current stack limits must allow pushing 6|8 bytes, else #SS(0)
          if (gate_descriptor.type >= 14) { // 386 gate
            if ( is_error_code )
              bytes = 16;
            else
              bytes = 12;
            }
          else { // 286 gate
            if ( is_error_code )
              bytes = 8;
            else
              bytes = 6;
            }

          if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache,
                         temp_ESP, bytes) ) {
            BX_DEBUG(("interrupt(): stack doesn't have room"));
            exception(BX_SS_EXCEPTION, 0, 0);
            }

          // eIP must be in CS limit else #GP(0)
          if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
            BX_PANIC(("interrupt(): IP > cs descriptor limit"));
            exception(BX_GP_EXCEPTION, 0, 0);
            }

          // push flags onto stack
          // push current CS selector onto stack
          // push return offset onto stack
          if (gate_descriptor.type >= 14) { // 386 gate
            push_32(read_eflags());
            push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
            push_32(EIP);
            if ( is_error_code )
              push_32(error_code);
            }
          else { // 286 gate
            push_16(read_flags());
            push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
            push_16(IP);
            if ( is_error_code )
              push_16(error_code);
            }

          // load CS:IP from gate
          // load CS descriptor
          // set the RPL field of CS to CPL
          load_cs(&cs_selector, &cs_descriptor, CPL);
          EIP = gate_dest_offset;

          // if interrupt gate then set IF to 0
          if ( !(gate_descriptor.type & 1) ) // even is int-gate
            BX_CPU_THIS_PTR clear_IF ();
          BX_CPU_THIS_PTR clear_TF ();
          BX_CPU_THIS_PTR clear_NT ();
          BX_CPU_THIS_PTR clear_VM ();
          BX_CPU_THIS_PTR clear_RF ();
          return;
          }

        // else #GP(CS selector + ext)
        BX_DEBUG(("interrupt: bad descriptor"));
        BX_DEBUG(("c_ed=%u, descriptor.dpl=%u, CPL=%u",
          (unsigned) cs_descriptor.u.segment.c_ed,
          (unsigned) cs_descriptor.dpl,
          (unsigned) CPL));
        BX_DEBUG(("cs.segment = %u", (unsigned) cs_descriptor.segment));
        exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
        break;

      default:
        BX_PANIC(("bad descriptor type in interrupt()!"));
        break;
      }
    }
  else
#endif
    { /* real mode */
    Bit16u cs_selector, ip;

    if ( (vector*4+3) > BX_CPU_THIS_PTR idtr.limit )
      BX_PANIC(("interrupt(real mode) vector > limit"));

    push_16(read_flags());

    cs_selector = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
    push_16(cs_selector);
    ip = EIP;
    push_16(ip);

    access_linear(BX_CPU_THIS_PTR idtr.base + 4 * vector,     2, 0, BX_READ, &ip);
    IP = ip;
    access_linear(BX_CPU_THIS_PTR idtr.base + 4 * vector + 2, 2, 0, BX_READ, &cs_selector);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_selector);

    /* INT affects the following flags: I,T */
    BX_CPU_THIS_PTR clear_IF ();
    BX_CPU_THIS_PTR clear_TF ();
#if BX_CPU_LEVEL >= 4
    BX_CPU_THIS_PTR clear_AC ();
#endif
    BX_CPU_THIS_PTR clear_RF ();
    }
}




  void
BX_CPU_C::exception(unsigned vector, Bit16u error_code, Boolean is_INT)
  // vector:     0..255: vector in IDT
  // error_code: if exception generates and error, push this error code
{
  Boolean  push_error;
  Bit8u    exception_type;
  unsigned prev_errno;

#if BX_DEBUGGER
  if (bx_guard.special_unwind_stack) {
    BX_INFO (("exception() returning early because special_unwind_stack is set"));
    return;
  }
#endif

#if BX_EXTERNAL_DEBUGGER
#if BX_SUPPORT_X86_64
  printf ("Exception(%u) code=%08x @%08x%08x\n", vector, error_code,(Bit32u)(BX_CPU_THIS_PTR prev_eip >>32),(Bit32u)(BX_CPU_THIS_PTR prev_eip));
#else
  printf ("Exception(%u) code=%08x @%08x\n", vector, error_code,(Bit32u)(BX_CPU_THIS_PTR prev_eip));
#endif
  //trap_debugger(1);
#endif


  BX_INSTR_EXCEPTION(CPU_ID, vector);
  invalidate_prefetch_q();

  UNUSED(is_INT);

  BX_DEBUG(("exception(%02x h)", (unsigned) vector));

  // if not initial error, restore previous register values from
  // previous attempt to handle exception
  if (BX_CPU_THIS_PTR errorno) {
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS]  = BX_CPU_THIS_PTR save_cs;
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS]  = BX_CPU_THIS_PTR save_ss;
    EIP = BX_CPU_THIS_PTR save_eip;
    ESP = BX_CPU_THIS_PTR save_esp;
    }

  BX_CPU_THIS_PTR errorno++;
  if (BX_CPU_THIS_PTR errorno >= 3) {
    BX_PANIC(("exception(): 3rd (%d) exception with no resolution", vector));
    BX_ERROR(("WARNING: Any simulation after this point is completely bogus."));
#if BX_DEBUGGER
    bx_guard.special_unwind_stack = true;
#endif
    return;
    }

  /* careful not to get here with curr_exception[1]==DOUBLE_FAULT */
  /* ...index on DOUBLE_FAULT below, will be out of bounds */

  /* if 1st was a double fault (software INT?), then shutdown */
  if ( (BX_CPU_THIS_PTR errorno==2) && (BX_CPU_THIS_PTR curr_exception[0]==BX_ET_DOUBLE_FAULT) ) {
    BX_PANIC(("exception(): triple fault encountered"));
    BX_ERROR(("WARNING: Any simulation after this point is completely bogus."));
#if BX_DEBUGGER
    bx_guard.special_unwind_stack = true;
#endif
    return;
    }

  /* ??? this is not totally correct, should be done depending on
   * vector */
  /* backup IP to value before error occurred */
  EIP = BX_CPU_THIS_PTR prev_eip;
  ESP = BX_CPU_THIS_PTR prev_esp;

  // note: fault-class exceptions _except_ #DB set RF in
  //       eflags image.

  switch (vector) {
    case  0: // DIV by 0
      push_error = 0;
      exception_type = BX_ET_CONTRIBUTORY;
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case  1: // debug exceptions
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  2: // NMI
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  3: // breakpoint
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  4: // overflow
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  5: // bounds check
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case  6: // invalid opcode
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case  7: // device not available
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case  8: // double fault
      push_error = 1;
      exception_type = BX_ET_DOUBLE_FAULT;
      break;
    case  9: // coprocessor segment overrun (286,386 only)
      push_error = 0;
      exception_type = BX_ET_CONTRIBUTORY;
      BX_CPU_THIS_PTR assert_RF ();
      BX_PANIC(("exception(9): unfinished"));
      break;
    case 10: // invalid TSS
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case 11: // segment not present
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case 12: // stack fault
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case 13: // general protection
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case 14: // page fault
      push_error = 1;
      exception_type = BX_ET_PAGE_FAULT;
      // ??? special format error returned
      BX_CPU_THIS_PTR assert_RF ();
      break;
    case 15: // reserved
      BX_PANIC(("exception(15): reserved"));
      push_error = 0;     // keep compiler happy for now
      exception_type = 0; // keep compiler happy for now
      break;
    case 16: // floating-point error
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      BX_CPU_THIS_PTR assert_RF ();
      break;
#if BX_CPU_LEVEL >= 4
    case 17: // alignment check
      BX_PANIC(("exception(): alignment-check, vector 17 unimplemented"));
      push_error = 0;     // keep compiler happy for now
      exception_type = 0; // keep compiler happy for now
      BX_CPU_THIS_PTR assert_RF ();
      break;
#endif
#if BX_CPU_LEVEL >= 5
    case 18: // machine check
      BX_PANIC(("exception(): machine-check, vector 18 unimplemented"));
      push_error = 0;     // keep compiler happy for now
      exception_type = 0; // keep compiler happy for now
      break;
#endif
    default:
      BX_PANIC(("exception(%u): bad vector", (unsigned) vector));
      push_error = 0;     // keep compiler happy for now
      exception_type = 0; // keep compiler happy for now
      break;
    }

  if (exception_type != BX_ET_PAGE_FAULT) {
    // Page faults have different format
    error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
    }
  BX_CPU_THIS_PTR EXT = 1;

  /* if we've already had 1st exception, see if 2nd causes a
   * Double Fault instead.  Otherwise, just record 1st exception
   */
  if (BX_CPU_THIS_PTR errorno >= 2) {
    if (is_exception_OK[BX_CPU_THIS_PTR curr_exception[0]][exception_type])
      BX_CPU_THIS_PTR curr_exception[1] = exception_type;
    else {
      BX_CPU_THIS_PTR curr_exception[1] = BX_ET_DOUBLE_FAULT;
      vector = 8;
    }
  }
  else {
    BX_CPU_THIS_PTR curr_exception[0] = exception_type;
  }


#if BX_CPU_LEVEL >= 2
  if (!real_mode()) {
    prev_errno = BX_CPU_THIS_PTR errorno;
    BX_CPU_THIS_PTR interrupt(vector, 0, push_error, error_code);
//    if (BX_CPU_THIS_PTR errorno > prev_errno) {
//      BX_INFO(("segment_exception(): errorno changed"));
//      longjmp(jmp_buf_env, 1); // go back to main decode loop
//      return;
//      }

//    if (push_error) {
//      /* push error code on stack, after handling interrupt */
//      /* pushed as a word or dword depending upon default size ??? */
//      if (ss.cache.u.segment.d_b)
//        push_32((Bit32u) error_code); /* upper bits reserved */
//      else
//        push_16(error_code);
//      if (BX_CPU_THIS_PTR errorno > prev_errno) {
//        BX_PANIC(("segment_exception(): errorno changed"));
//        return;
//        }
//      }
    BX_CPU_THIS_PTR errorno = 0; // error resolved
    longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
    }
  else // real mode
#endif
    {
    // not INT, no error code pushed
    BX_CPU_THIS_PTR interrupt(vector, 0, 0, 0);
    BX_CPU_THIS_PTR errorno = 0; // error resolved
    longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
    }
}


  int
BX_CPU_C::int_number(bx_segment_reg_t *seg)
{
  if (seg == &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS])
    return(BX_SS_EXCEPTION);
  else
    return(BX_GP_EXCEPTION);
}

  void
BX_CPU_C::shutdown_cpu(void)
{

#if BX_CPU_LEVEL > 2
  BX_PANIC(("shutdown_cpu(): not implemented for 386"));
#endif

  invalidate_prefetch_q();
  BX_PANIC(("shutdown_cpu(): not finished"));

}

#if BX_SUPPORT_X86_64
  void
BX_CPU_C::SYSCALL(bxInstruction_c *i)
{

/* pseudo code from AMD manual.

SYSCALL_START:

  IF (MSR_EFER.SCE = 0) // Check if syscall/sysret are enabled.
    EXCEPTION [#UD]

  IF (LONG_MODE)
    SYSCALL_LONG_MODE
  ELSE // (LEGACY_MODE)
    SYSCALL_LEGACY_MODE


SYSCALL_LONG_MODE:

  RCX.q = next_RIP
  R11.q = RFLAGS // with rf cleared

  IF (64BIT_MODE)
      temp_RIP.q = MSR_LSTAR
  ELSE // (COMPATIBILITY_MODE)
      temp_RIP.q = MSR_CSTAR

  CS.sel = MSR_STAR.SYSCALL_CS AND 0xFFFC
  CS.attr = 64-bit code,dpl0 // Always switch to 64-bit mode in long mode.
  CS.base = 0x00000000
  CS.limit = 0xFFFFFFFF

  SS.sel = MSR_STAR.SYSCALL_CS + 8
  SS.attr = 64-bit stack,dpl0
  SS.base = 0x00000000
  SS.limit = 0xFFFFFFFF

  RFLAGS = RFLAGS AND ~MSR_SFMASK
  RFLAGS.RF = 0

  CPL = 0

  RIP = temp_RIP
  EXIT

SYSCALL_LEGACY_MODE:

  RCX.d = next_RIP

  temp_RIP.d = MSR_STAR.EIP

  CS.sel = MSR_STAR.SYSCALL_CS AND 0xFFFC
  CS.attr = 32-bit code,dpl0 // Always switch to 32-bit mode in legacy mode.
  CS.base = 0x00000000
  CS.limit = 0xFFFFFFFF

  SS.sel = MSR_STAR.SYSCALL_CS + 8
  SS.attr = 32-bit stack,dpl0
  SS.base = 0x00000000
  SS.limit = 0xFFFFFFFF

  RFLAGS.VM,IF,RF=0

  CPL = 0

  RIP = temp_RIP
  EXIT

*/

  bx_address temp_RIP;
  bx_descriptor_t cs_descriptor,ss_descriptor;
  bx_selector_t cs_selector,ss_selector;
  Bit32u  dword1, dword2;



  if (!BX_CPU_THIS_PTR msr.sce) {
    exception(BX_GP_EXCEPTION, 0, 0);
    }
  invalidate_prefetch_q();
  if (BX_CPU_THIS_PTR msr.lma) {

    RCX = RIP;
#warning - PRT: SYSCALL --  do we reset RF/VM before saving to R11?
    R11 = read_eflags();

    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
      temp_RIP = MSR_LSTAR;
      }
    else {
      temp_RIP = MSR_CSTAR;
      }

    parse_selector((MSR_STAR >> 32) & 0xFFFC, &cs_selector);
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);
    load_cs(&cs_selector, &cs_descriptor, 0);

    parse_selector((MSR_STAR >> 32) + 8, &ss_selector);
    fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &ss_descriptor);
    load_ss(&ss_selector, &ss_descriptor, 0);

    write_eflags(read_eflags() & MSR_FMASK,1,1,1,0);
    BX_CPU_THIS_PTR clear_RF ();
    RIP = temp_RIP;
    }
  else {
    // legacy mode

    ECX = EIP;

    temp_RIP = MSR_STAR & 0xFFFFFFFF;

    parse_selector((MSR_STAR >> 32) & 0xFFFC, &cs_selector);
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);
    load_cs(&cs_selector, &cs_descriptor, 0);

    parse_selector((MSR_STAR >> 32) + 8, &ss_selector);
    fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &ss_descriptor);
    load_ss(&ss_selector, &ss_descriptor, 0);

    BX_CPU_THIS_PTR clear_VM ();
    BX_CPU_THIS_PTR clear_IF ();
    BX_CPU_THIS_PTR clear_RF ();
    RIP = temp_RIP;
    }

}

  void
BX_CPU_C::SYSRET(bxInstruction_c *i)
{
/* from AMD manual

SYSRET_START:

  IF (MSR_EFER.SCE = 0) // Check if syscall/sysret are enabled.
    EXCEPTION [#UD]

  IF ((!PROTECTED_MODE) || (CPL != 0))
    EXCEPTION [#GP(0)] // SYSRET requires protected mode, cpl0

  IF (64BIT_MODE)
    SYSRET_64BIT_MODE
  ELSE // (!64BIT_MODE)
    SYSRET_NON_64BIT_MODE

SYSRET_64BIT_MODE:
  IF (OPERAND_SIZE = 64) // Return to 64-bit mode.
    {
    CS.sel = (MSR_STAR.SYSRET_CS + 16) OR 3
    CS.base = 0x00000000
    CS.limit = 0xFFFFFFFF
    CS.attr = 64-bit code,dpl3
    temp_RIP.q = RCX
    }
  ELSE // Return to 32-bit compatibility mode.
    {
    CS.sel = MSR_STAR.SYSRET_CS OR 3
    CS.base = 0x00000000
    CS.limit = 0xFFFFFFFF
    CS.attr = 32-bit code,dpl3
    temp_RIP.d = RCX
    }
  SS.sel = MSR_STAR.SYSRET_CS + 8 // SS selector is changed,
  // SS base, limit, attributes unchanged.
  RFLAGS.q = R11 // RF=0,VM=0
  CPL = 3
  RIP = temp_RIP
  EXIT

SYSRET_NON_64BIT_MODE:
  CS.sel = MSR_STAR.SYSRET_CS OR 3 // Return to 32-bit legacy protected mode.
  CS.base = 0x00000000
  CS.limit = 0xFFFFFFFF
  CS.attr = 32-bit code,dpl3
  temp_RIP.d = RCX
  SS.sel = MSR_STAR.SYSRET_CS + 8 // SS selector is changed.
  // SS base, limit, attributes unchanged.
  RFLAGS.IF = 1
  CPL = 3
  RIP = temp_RIP
  EXIT

*/

  bx_address temp_RIP;
  bx_descriptor_t cs_descriptor,ss_descriptor;
  bx_selector_t cs_selector,ss_selector;
  Bit32u  dword1, dword2;

  if (!BX_CPU_THIS_PTR msr.sce) {
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  if(real_mode() || CPL != 0) {
    exception(BX_GP_EXCEPTION, 0, 0);
    }

  invalidate_prefetch_q();
  
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    if (i->os64L()) { // Return to 64-bit mode.

      parse_selector(((MSR_STAR >> 48) + 16) | 3, &cs_selector);
      fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
      parse_descriptor(dword1, dword2, &cs_descriptor);
      load_cs(&cs_selector, &cs_descriptor, 3);

      temp_RIP = RCX;

      }
    else { // Return to 32-bit compatibility mode.

      parse_selector((MSR_STAR >> 48) | 3, &cs_selector);
      fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
      parse_descriptor(dword1, dword2, &cs_descriptor);
      load_cs(&cs_selector, &cs_descriptor, 3);

      temp_RIP = ECX;

      }

    parse_selector((MSR_STAR >> 48) + 8, &ss_selector);
    fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &ss_descriptor);
    load_ss(&ss_selector, &ss_descriptor, 0);

    // SS base, limit, attributes unchanged.
    write_eflags(R11,1,1,1,1);

    RIP = temp_RIP;

    }
  else { // (!64BIT_MODE)

    parse_selector((MSR_STAR >> 48) + 16, &cs_selector);
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);
    load_cs(&cs_selector, &cs_descriptor, 3);

    temp_RIP = ECX;

    parse_selector((MSR_STAR >> 48) + 8, &ss_selector);
    fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &ss_descriptor);
    load_ss(&ss_selector, &ss_descriptor, 0);

    BX_CPU_THIS_PTR assert_IF ();

    RIP = temp_RIP;
    }

}
#endif // BX_SUPPORT_X86_64
