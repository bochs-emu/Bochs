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
  BX_CPU_THIS_PTR show_flag |= Flag_intsig;
#if BX_DEBUG_LINUX
  if (bx_dbg.linux_syscall) {
    if (vector == 0x80) bx_dbg_linux_syscall ();
  }
#endif
#endif

//BX_DEBUG(( "::interrupt(%u)\n", vector ));

  BX_INSTR_INTERRUPT(vector);
  invalidate_prefetch_q();

  // Discard any traps and inhibits for new context; traps will
  // resume upon return.
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR inhibit_mask = 0;

#if BX_CPU_LEVEL >= 2
//  unsigned prev_errno;

  if (bx_dbg.interrupts)
    BX_INFO(("interrupt(): vector = %u, INT = %u, EXT = %u\n",
      (unsigned) vector, (unsigned) is_INT, (unsigned) BX_CPU_THIS_PTR EXT));

BX_CPU_THIS_PTR save_cs  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS];
BX_CPU_THIS_PTR save_ss  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS];
BX_CPU_THIS_PTR save_eip = EIP;
BX_CPU_THIS_PTR save_esp = ESP;

//  prev_errno = BX_CPU_THIS_PTR errorno;

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
      if (bx_dbg.interrupts) {
        BX_INFO(("IDT.limit = %04x\n", (unsigned) BX_CPU_THIS_PTR idtr.limit));
        BX_INFO(("IDT.base  = %06x\n", (unsigned) BX_CPU_THIS_PTR idtr.base));
        BX_INFO(("interrupt vector must be within IDT table limits\n"));
        BX_INFO(("bailing\n"));
        }
      BX_INFO(("interrupt(): vector > idtr.limit\n"));

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
      BX_PANIC(("interrupt(): gate descriptor is not valid sys seg\n"));
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
        BX_INFO(("interrupt(): gate.type(%u) != {5,6,7,14,15}\n",
          (unsigned) gate_descriptor.type));
        exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
        return;
      }

    // if software interrupt, then gate descripor DPL must be >= CPL,
    // else #GP(vector * 8 + 2 + EXT)
    if (is_INT  &&  (gate_descriptor.dpl < CPL)) {
/* ??? */
      BX_INFO(("interrupt(): is_INT && (dpl < CPL)\n"));
      exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      return;
      }

    // Gate must be present, else #NP(vector * 8 + 2 + EXT)
    if (gate_descriptor.p == 0) {
      BX_INFO(("interrupt(): p == 0\n"));
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
          BX_PANIC(("interrupt: tss_selector.ti=1\n"));
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
          BX_PANIC(("exception: TSS selector points to bad TSS\n"));
          exception(BX_TS_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }
        if (tss_descriptor.type!=9 && tss_descriptor.type!=1) {
          BX_PANIC(("exception: TSS selector points to bad TSS\n"));
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
          BX_PANIC(("exception(): eIP > CS.limit\n"));
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
          BX_PANIC(("int_trap_gate(): selector null\n"));
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
          BX_INFO(("interrupt(): not code segment\n"));
          exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
          }

        // segment must be present, else #NP(selector + EXT)
        if ( cs_descriptor.p==0 ) {
          BX_PANIC(("interrupt(): segment not present\n"));
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

          if (bx_dbg.interrupts)
            BX_INFO(("interrupt(): INTERRUPT TO INNER PRIVILEGE\n"));

          // check selector and descriptor for new stack in current TSS
          get_SS_ESP_from_TSS(cs_descriptor.dpl,
                              &SS_for_cpl_x, &ESP_for_cpl_x);

          // Selector must be non-null else #TS(EXT)
          if ( (SS_for_cpl_x & 0xfffc) == 0 ) {
            BX_PANIC(("interrupt(): SS selector null\n"));
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
            BX_PANIC(("interrupt(): SS.rpl != CS.dpl\n"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            }

          // stack seg DPL must = DPL of code segment,
          // else #TS(SS selector + ext)
          if (ss_descriptor.dpl != cs_descriptor.dpl) {
            BX_PANIC(("interrupt(): SS.dpl != CS.dpl\n"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            }

          // descriptor must indicate writable data segment,
          // else #TS(SS selector + EXT)
          if (ss_descriptor.valid==0 ||
              ss_descriptor.segment==0  ||
              ss_descriptor.u.segment.executable==1  ||
              ss_descriptor.u.segment.r_w==0) {
            BX_PANIC(("interrupt(): SS not writable data segment\n"));
            exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            }

          // seg must be present, else #SS(SS selector + ext)
          if (ss_descriptor.p==0) {
            BX_PANIC(("interrupt(): SS not present\n"));
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
              BX_PANIC(("interrupt: int/trap gate VM\n"));
              }
            }

// 486,Pentium books
// new stack must have room for 10/12 bytes, else #SS(0) 486 book
// PPro+
// new stack must have room for 10/12 bytes, else #SS(seg selector)
          if ( !can_push(&ss_descriptor, ESP_for_cpl_x, bytes) ) {
            BX_PANIC(("interrupt(): new stack doesn't have room for %u bytes\n",
               (unsigned) bytes));
            // SS(???)
            }

          // IP must be within CS segment boundaries, else #GP(0)
          if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
            BX_PANIC(("interrupt(): gate eIP > CS.limit\n"));
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
              BX_PANIC(("286 int/trap gate, VM\n"));
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
            BX_CPU_THIS_PTR eflags.if_ = 0;
          BX_CPU_THIS_PTR eflags.tf = 0;
          BX_CPU_THIS_PTR eflags.vm = 0;
          BX_CPU_THIS_PTR eflags.rf = 0;
          BX_CPU_THIS_PTR eflags.nt = 0;
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

          if (bx_dbg.interrupts)
            BX_INFO(("int_trap_gate286(): INTERRUPT TO SAME PRIVILEGE\n"));

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
            BX_INFO(("interrupt(): stack doesn't have room\n"));
            exception(BX_SS_EXCEPTION, 0, 0);
            }

          // eIP must be in CS limit else #GP(0)
          if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
            BX_PANIC(("interrupt(): IP > cs descriptor limit\n"));
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
            BX_CPU_THIS_PTR eflags.if_ = 0;
          BX_CPU_THIS_PTR eflags.tf = 0;
          BX_CPU_THIS_PTR eflags.nt = 0;
          BX_CPU_THIS_PTR eflags.vm = 0;
          BX_CPU_THIS_PTR eflags.rf = 0;
          return;
          }

        // else #GP(CS selector + ext)
        BX_INFO(("interrupt: bad descriptor\n"));
        BX_INFO(("c_ed=%u, descriptor.dpl=%u, CPL=%u\n",
          (unsigned) cs_descriptor.u.segment.c_ed,
          (unsigned) cs_descriptor.dpl,
          (unsigned) CPL));
        BX_INFO(("cs.segment = %u\n", (unsigned) cs_descriptor.segment));
        exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
        break;

      default:
        BX_PANIC(("bad descriptor type in interrupt()!\n"));
        break;
      }
    }
  else
#endif
    { /* real mode */
    Bit16u cs_selector, ip;

    if ( (vector*4+3) > BX_CPU_THIS_PTR idtr.limit )
      BX_PANIC(("interrupt(real mode) vector > limit\n"));

    push_16(read_flags());

    cs_selector = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
    push_16(cs_selector);
    ip = BX_CPU_THIS_PTR eip;
    push_16(ip);

    access_linear(BX_CPU_THIS_PTR idtr.base + 4 * vector,     2, 0, BX_READ, &ip);
    IP = ip;
    access_linear(BX_CPU_THIS_PTR idtr.base + 4 * vector + 2, 2, 0, BX_READ, &cs_selector);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_selector);

    /* INT affects the following flags: I,T */
    BX_CPU_THIS_PTR eflags.if_ = 0;
    BX_CPU_THIS_PTR eflags.tf  = 0;
#if BX_CPU_LEVEL >= 4
    BX_CPU_THIS_PTR eflags.ac  = 0;
#endif
    BX_CPU_THIS_PTR eflags.rf = 0;
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

//BX_DEBUG(( "::exception(%u)\n", vector ));

  BX_INSTR_EXCEPTION(vector);
  invalidate_prefetch_q();

  UNUSED(is_INT);

  if (bx_dbg.exceptions)
    BX_INFO(("exception(%02x h)\n", (unsigned) vector));

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
    BX_PANIC(("exception(): 3rd exception with no resolution\n"));
    }

  /* careful not to get here with curr_exception[1]==DOUBLE_FAULT */
  /* ...index on DOUBLE_FAULT below, will be out of bounds */

  /* if 1st was a double fault (software INT?), then shutdown */
  if ( (BX_CPU_THIS_PTR errorno==2) && (BX_CPU_THIS_PTR curr_exception[0]==BX_ET_DOUBLE_FAULT) ) {
    BX_PANIC(("exception(): tripple fault encountered\n"));
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
      BX_CPU_THIS_PTR eflags.rf = 1;
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
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
    case  6: // invalid opcode
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
    case  7: // device not available
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
    case  8: // double fault
      push_error = 1;
      exception_type = BX_ET_DOUBLE_FAULT;
      break;
    case  9: // coprocessor segment overrun (286,386 only)
      push_error = 0;
      exception_type = BX_ET_CONTRIBUTORY;
      BX_CPU_THIS_PTR eflags.rf = 1;
      BX_PANIC(("exception(9): unfinished\n"));
      break;
    case 10: // invalid TSS
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
    case 11: // segment not present
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
    case 12: // stack fault
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
    case 13: // general protection
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
    case 14: // page fault
      push_error = 1;
      exception_type = BX_ET_PAGE_FAULT;
      // ??? special format error returned
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
    case 15: // reserved
      BX_PANIC(("exception(15): reserved\n"));
      push_error = 0;     // keep compiler happy for now
      exception_type = 0; // keep compiler happy for now
      break;
    case 16: // floating-point error
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
#if BX_CPU_LEVEL >= 4
    case 17: // alignment check
      BX_PANIC(("exception(): alignment-check, vector 17 unimplemented\n"));
      push_error = 0;     // keep compiler happy for now
      exception_type = 0; // keep compiler happy for now
      BX_CPU_THIS_PTR eflags.rf = 1;
      break;
#endif
#if BX_CPU_LEVEL >= 5
    case 18: // machine check
      BX_PANIC(("exception(): machine-check, vector 18 unimplemented\n"));
      push_error = 0;     // keep compiler happy for now
      exception_type = 0; // keep compiler happy for now
      break;
#endif
    default:
      BX_PANIC(("exception(%u): bad vector\n", (unsigned) vector));
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
//      BX_INFO(("segment_exception(): errorno changed\n"));
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
//        BX_PANIC(("segment_exception(): errorno changed\n"));
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
  BX_PANIC(("shutdown_cpu(): not implemented for 386\n"));
#endif

  invalidate_prefetch_q();
  BX_PANIC(("shutdown_cpu(): not finished\n"));

}
