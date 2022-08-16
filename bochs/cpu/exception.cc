/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2019  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "param_names.h"
#include "iodev/iodev.h"

#if BX_SUPPORT_X86_64==0
// Make life easier merging cpu64 & cpu code.
#define RIP EIP
#define RSP ESP
#endif

#if BX_SUPPORT_X86_64
void BX_CPU_C::long_mode_int(Bit8u vector, bool soft_int, bool push_error, Bit16u error_code)
{
  bx_descriptor_t gate_descriptor, cs_descriptor;
  bx_selector_t cs_selector;

  // interrupt vector must be within IDT table limits,
  // else #GP(vector*8 + 2 + EXT)
  if ((vector*16 + 15) > BX_CPU_THIS_PTR idtr.limit) {
    BX_ERROR(("interrupt(long mode): vector must be within IDT table limits, IDT.limit = 0x%x", BX_CPU_THIS_PTR idtr.limit));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  Bit64u desctmp1 = system_read_qword(BX_CPU_THIS_PTR idtr.base + vector*16);
  Bit64u desctmp2 = system_read_qword(BX_CPU_THIS_PTR idtr.base + vector*16 + 8);

  if (desctmp2 & BX_CONST64(0x00001F0000000000)) {
    BX_ERROR(("interrupt(long mode): IDT entry extended attributes DWORD4 TYPE != 0"));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  Bit32u dword1 = GET32L(desctmp1);
  Bit32u dword2 = GET32H(desctmp1);
  Bit32u dword3 = GET32L(desctmp2);

  parse_descriptor(dword1, dword2, &gate_descriptor);

  if ((gate_descriptor.valid==0) || gate_descriptor.segment)
  {
    BX_ERROR(("interrupt(long mode): gate descriptor is not valid sys seg"));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // descriptor AR byte must indicate interrupt gate, trap gate,
  // or task gate, else #GP(vector*8 + 2 + EXT)
  if (gate_descriptor.type != BX_386_INTERRUPT_GATE &&
      gate_descriptor.type != BX_386_TRAP_GATE)
  {
    BX_ERROR(("interrupt(long mode): unsupported gate type %u",
        (unsigned) gate_descriptor.type));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // if software interrupt, then gate descriptor DPL must be >= CPL,
  // else #GP(vector * 8 + 2 + EXT)
  if (soft_int && gate_descriptor.dpl < CPL)
  {
    BX_ERROR(("interrupt(long mode): soft_int && gate.dpl < CPL"));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // Gate must be present, else #NP(vector * 8 + 2 + EXT)
  if (! IS_PRESENT(gate_descriptor)) {
    BX_ERROR(("interrupt(long mode): gate.p == 0"));
    exception(BX_NP_EXCEPTION, vector*8 + 2);
  }

  Bit16u gate_dest_selector = gate_descriptor.u.gate.dest_selector;
  Bit64u gate_dest_offset   = ((Bit64u)dword3 << 32) | gate_descriptor.u.gate.dest_offset;

  unsigned ist = gate_descriptor.u.gate.param_count & 0x7;

  // examine CS selector and descriptor given in gate descriptor
  // selector must be non-null else #GP(EXT)
  if ((gate_dest_selector & 0xfffc) == 0) {
    BX_ERROR(("int_trap_gate(long mode): selector null"));
    exception(BX_GP_EXCEPTION, 0);
  }

  parse_selector(gate_dest_selector, &cs_selector);

  // selector must be within its descriptor table limits
  // else #GP(selector+EXT)
  fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
  parse_descriptor(dword1, dword2, &cs_descriptor);

  // descriptor AR byte must indicate code seg
  // and code segment descriptor DPL<=CPL, else #GP(selector+EXT)
  if (cs_descriptor.valid==0 || cs_descriptor.segment==0 ||
      IS_DATA_SEGMENT(cs_descriptor.type) ||
      cs_descriptor.dpl > CPL)
  {
    BX_ERROR(("interrupt(long mode): not accessible or not code segment"));
    exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
  }

  // check that it's a 64 bit segment
  if (! IS_LONG64_SEGMENT(cs_descriptor) || cs_descriptor.u.segment.d_b)
  {
    BX_ERROR(("interrupt(long mode): must be 64 bit segment"));
    exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
  }

  // segment must be present, else #NP(selector + EXT)
  if (! IS_PRESENT(cs_descriptor)) {
    BX_ERROR(("interrupt(long mode): segment not present"));
    exception(BX_NP_EXCEPTION, cs_selector.value & 0xfffc);
  }
 
  Bit64u RSP_for_cpl_x;
#if BX_SUPPORT_CET
  bx_address new_SSP = 0; // keep warning silent
  unsigned old_SS_DPL = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl;
  unsigned old_CPL = CPL;
  bx_address return_LIP = get_laddr(BX_SEG_REG_CS, RIP);
  bool check_ss_token = true;
#endif

  Bit64u old_CS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  Bit64u old_RIP = RIP;
  Bit64u old_SS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  Bit64u old_RSP = RSP;

  // if code segment is non-conforming and DPL < CPL then
  // INTERRUPT TO INNER PRIVILEGE:
  if (IS_CODE_SEGMENT_NON_CONFORMING(cs_descriptor.type) && cs_descriptor.dpl < CPL)
  {
    BX_DEBUG(("interrupt(long mode): INTERRUPT TO INNER PRIVILEGE"));

    // check selector and descriptor for new stack in current TSS
    if (ist > 0) {
      BX_DEBUG(("interrupt(long mode): trap to IST, vector = %d", ist));
      RSP_for_cpl_x = get_RSP_from_TSS(ist+3);
#if BX_SUPPORT_CET
      if (ShadowStackEnabled(0)) {
        bx_address new_SSP_addr = BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table + (ist<<3);
        new_SSP = system_read_qword(new_SSP_addr);
      }
#endif
    }
    else {
      RSP_for_cpl_x = get_RSP_from_TSS(cs_descriptor.dpl);
#if BX_SUPPORT_CET
      new_SSP = BX_CPU_THIS_PTR msr.ia32_pl_ssp[cs_descriptor.dpl];
#endif
    }

    // align stack
    RSP_for_cpl_x &= BX_CONST64(0xfffffffffffffff0);

    // push old stack long pointer onto new stack
    write_new_stack_qword(RSP_for_cpl_x -  8, cs_descriptor.dpl, old_SS);
    write_new_stack_qword(RSP_for_cpl_x - 16, cs_descriptor.dpl, old_RSP);
    write_new_stack_qword(RSP_for_cpl_x - 24, cs_descriptor.dpl, read_eflags());
    // push long pointer to return address onto new stack
    write_new_stack_qword(RSP_for_cpl_x - 32, cs_descriptor.dpl, old_CS);
    write_new_stack_qword(RSP_for_cpl_x - 40, cs_descriptor.dpl, old_RIP);
    RSP_for_cpl_x -= 40;

    if (push_error) {
      RSP_for_cpl_x -= 8;
      write_new_stack_qword(RSP_for_cpl_x, cs_descriptor.dpl, error_code);
    }

    // load CS:RIP (guaranteed to be in 64 bit mode)
    branch_far(&cs_selector, &cs_descriptor, gate_dest_offset, cs_descriptor.dpl);

    // set up null SS descriptor
    load_null_selector(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], cs_descriptor.dpl);
  }
  else if(IS_CODE_SEGMENT_CONFORMING(cs_descriptor.type) || cs_descriptor.dpl==CPL) 
  {
    // if code segment is conforming OR code segment DPL = CPL then
    // INTERRUPT TO SAME PRIVILEGE LEVEL:

    BX_DEBUG(("interrupt(long mode): INTERRUPT TO SAME PRIVILEGE"));

    // check selector and descriptor for new stack in current TSS
    if (ist > 0) {
      BX_DEBUG(("interrupt(long mode): trap to IST, vector = %d", ist));
      RSP_for_cpl_x = get_RSP_from_TSS(ist+3);
#if BX_SUPPORT_CET
      if (ShadowStackEnabled(CPL)) {
        bx_address new_SSP_addr = BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table + (ist<<3);
        new_SSP = system_read_qword(new_SSP_addr);
      }
#endif
    }
    else {
      RSP_for_cpl_x = RSP;
#if BX_SUPPORT_CET
      new_SSP = SSP;
      check_ss_token = false;
#endif
    }

    // align stack
    RSP_for_cpl_x &= BX_CONST64(0xfffffffffffffff0);

    // push flags onto stack
    // push current CS selector onto stack
    // push return offset onto stack
    write_new_stack_qword(RSP_for_cpl_x - 8,  cs_descriptor.dpl, old_SS);
    write_new_stack_qword(RSP_for_cpl_x - 16, cs_descriptor.dpl, old_RSP);
    write_new_stack_qword(RSP_for_cpl_x - 24, cs_descriptor.dpl, read_eflags());
    // push long pointer to return address onto new stack
    write_new_stack_qword(RSP_for_cpl_x - 32, cs_descriptor.dpl, old_CS);
    write_new_stack_qword(RSP_for_cpl_x - 40, cs_descriptor.dpl, old_RIP);
    RSP_for_cpl_x -= 40;

    if (push_error) {
      RSP_for_cpl_x -= 8;
      write_new_stack_qword(RSP_for_cpl_x, cs_descriptor.dpl, error_code);
    }

    // set the RPL field of CS to CPL
    branch_far(&cs_selector, &cs_descriptor, gate_dest_offset, CPL);
  }
  else {
    BX_ERROR(("interrupt(long mode): bad descriptor type %u (CS.DPL=%u CPL=%u)",
      (unsigned) cs_descriptor.type, (unsigned) cs_descriptor.dpl, (unsigned) CPL));
    exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
  }

#if BX_SUPPORT_CET
  if(ShadowStackEnabled(old_CPL)) {
    if (old_CPL == 3)
      BX_CPU_THIS_PTR msr.ia32_pl_ssp[3] = SSP;
  }
  if (ShadowStackEnabled(CPL)) {
    bx_address old_SSP = SSP;
    if(check_ss_token)
      shadow_stack_switch(new_SSP);
    if (old_SS_DPL != 3)
      call_far_shadow_stack_push(old_CS, return_LIP, old_SSP);
  }
  track_indirect(CPL);
#endif

  RSP = RSP_for_cpl_x;

  // if interrupt gate then set IF to 0
  if (!(gate_descriptor.type & 1)) // even is int-gate
    BX_CPU_THIS_PTR clear_IF();
  BX_CPU_THIS_PTR clear_TF();
//BX_CPU_THIS_PTR clear_VM(); // VM is clear in long mode
  BX_CPU_THIS_PTR clear_RF();
  BX_CPU_THIS_PTR clear_NT();
}
#endif

void BX_CPU_C::protected_mode_int(Bit8u vector, bool soft_int, bool push_error, Bit16u error_code)
{
  bx_descriptor_t gate_descriptor, cs_descriptor;
  bx_selector_t cs_selector;

  Bit16u raw_tss_selector;
  bx_selector_t   tss_selector;
  bx_descriptor_t tss_descriptor;

  // interrupt vector must be within IDT table limits,
  // else #GP(vector*8 + 2 + EXT)
  if ((vector*8 + 7) > BX_CPU_THIS_PTR idtr.limit) {
    BX_ERROR(("interrupt(): vector must be within IDT table limits, IDT.limit = 0x%x", BX_CPU_THIS_PTR idtr.limit));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  Bit64u desctmp = system_read_qword(BX_CPU_THIS_PTR idtr.base + vector*8);

  Bit32u dword1 = GET32L(desctmp);
  Bit32u dword2 = GET32H(desctmp);

  parse_descriptor(dword1, dword2, &gate_descriptor);

  if ((gate_descriptor.valid==0) || gate_descriptor.segment) {
    BX_ERROR(("interrupt(): gate descriptor is not valid sys seg (vector=0x%02x)", vector));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // descriptor AR byte must indicate interrupt gate, trap gate,
  // or task gate, else #GP(vector*8 + 2 + EXT)
  switch (gate_descriptor.type) {
  case BX_TASK_GATE:
  case BX_286_INTERRUPT_GATE:
  case BX_286_TRAP_GATE:
  case BX_386_INTERRUPT_GATE:
  case BX_386_TRAP_GATE:
    break;
  default:
    BX_ERROR(("interrupt(): gate.type(%u) != {5,6,7,14,15}",
      (unsigned) gate_descriptor.type));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // if software interrupt, then gate descriptor DPL must be >= CPL,
  // else #GP(vector * 8 + 2 + EXT)
  if (soft_int && gate_descriptor.dpl < CPL) {
    BX_ERROR(("interrupt(): soft_int && (gate.dpl < CPL)"));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // Gate must be present, else #NP(vector * 8 + 2 + EXT)
  if (! IS_PRESENT(gate_descriptor)) {
    BX_ERROR(("interrupt(): gate not present"));
    exception(BX_NP_EXCEPTION, vector*8 + 2);
  }

  switch (gate_descriptor.type) {
  case BX_TASK_GATE:
    // examine selector to TSS, given in task gate descriptor
    raw_tss_selector = gate_descriptor.u.taskgate.tss_selector;
    parse_selector(raw_tss_selector, &tss_selector);

    // must specify global in the local/global bit,
    //      else #GP(TSS selector)
    if (tss_selector.ti) {
      BX_ERROR(("interrupt(): tss_selector.ti=1 from gate descriptor - #GP(tss_selector)"));
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // index must be within GDT limits, else #TS(TSS selector)
    fetch_raw_descriptor(&tss_selector, &dword1, &dword2, BX_GP_EXCEPTION);

    parse_descriptor(dword1, dword2, &tss_descriptor);

    // AR byte must specify available TSS,
    //   else #GP(TSS selector)
    if (tss_descriptor.valid==0 || tss_descriptor.segment) {
      BX_ERROR(("interrupt(): TSS selector points to invalid or bad TSS - #GP(tss_selector)"));
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    if (tss_descriptor.type!=BX_SYS_SEGMENT_AVAIL_286_TSS &&
        tss_descriptor.type!=BX_SYS_SEGMENT_AVAIL_386_TSS)
    {
      BX_ERROR(("interrupt(): TSS selector points to bad TSS - #GP(tss_selector)"));
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // TSS must be present, else #NP(TSS selector)
    if (! IS_PRESENT(tss_descriptor)) {
      BX_ERROR(("interrupt(): TSS descriptor.p == 0"));
      exception(BX_NP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // switch tasks with nesting to TSS
    task_switch(0, &tss_selector, &tss_descriptor,
                    BX_TASK_FROM_INT, dword1, dword2, push_error, error_code);
    return;

  case BX_286_INTERRUPT_GATE:
  case BX_286_TRAP_GATE:
  case BX_386_INTERRUPT_GATE:
  case BX_386_TRAP_GATE:
  {
    Bit16u gate_dest_selector = gate_descriptor.u.gate.dest_selector;
    Bit32u gate_dest_offset   = gate_descriptor.u.gate.dest_offset;

    // examine CS selector and descriptor given in gate descriptor
    // selector must be non-null else #GP(EXT)
    if ((gate_dest_selector & 0xfffc) == 0) {
      BX_ERROR(("int_trap_gate(): selector null"));
      exception(BX_GP_EXCEPTION, 0);
    }

    parse_selector(gate_dest_selector, &cs_selector);

    // selector must be within its descriptor table limits
    // else #GP(selector+EXT)
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);

    // descriptor AR byte must indicate code seg
    // and code segment descriptor DPL<=CPL, else #GP(selector+EXT)
    if (cs_descriptor.valid==0 || cs_descriptor.segment==0 ||
        IS_DATA_SEGMENT(cs_descriptor.type) ||
        cs_descriptor.dpl > CPL)
    {
      BX_ERROR(("interrupt(): not accessible or not code segment cs=0x%04x", cs_selector.value));
      exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
    }

    // segment must be present, else #NP(selector + EXT)
    if (! IS_PRESENT(cs_descriptor)) {
      BX_ERROR(("interrupt(): segment not present"));
      exception(BX_NP_EXCEPTION, cs_selector.value & 0xfffc);
    }

    Bit32u old_ESP = ESP;
    Bit16u old_SS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
    Bit32u old_EIP = EIP;
    Bit16u old_CS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

#if BX_SUPPORT_CET
    bx_address new_SSP = BX_CPU_THIS_PTR msr.ia32_pl_ssp[cs_descriptor.dpl];
    Bit32u return_LIP = get_laddr(BX_SEG_REG_CS, EIP);
    unsigned old_SS_DPL = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl;
    unsigned old_CPL = CPL;
#endif

    // if code segment is non-conforming and DPL < CPL then
    // INTERRUPT TO INNER PRIVILEGE
    if(IS_CODE_SEGMENT_NON_CONFORMING(cs_descriptor.type) && cs_descriptor.dpl < CPL)
    {
      Bit16u SS_for_cpl_x;
      Bit32u ESP_for_cpl_x;
      bx_descriptor_t ss_descriptor;
      bx_selector_t   ss_selector;
      int is_v8086_mode = v8086_mode();

      BX_DEBUG(("interrupt(): INTERRUPT TO INNER PRIVILEGE"));

      // check selector and descriptor for new stack in current TSS
      get_SS_ESP_from_TSS(cs_descriptor.dpl, &SS_for_cpl_x, &ESP_for_cpl_x);

      if (is_v8086_mode && cs_descriptor.dpl != 0) {
        // if code segment DPL != 0 then #GP(new code segment selector)
        BX_ERROR(("interrupt(): code segment DPL(%d) != 0 in v8086 mode", cs_descriptor.dpl));
        exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
      }

      // Selector must be non-null else #TS(EXT)
      if ((SS_for_cpl_x & 0xfffc) == 0) {
        BX_ERROR(("interrupt(): SS selector null"));
        exception(BX_TS_EXCEPTION, 0); /* TS(ext) */
      }

      // selector index must be within its descriptor table limits
      // else #TS(SS selector + EXT)
      parse_selector(SS_for_cpl_x, &ss_selector);
      // fetch 2 dwords of descriptor; call handles out of limits checks
      fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_TS_EXCEPTION);
      parse_descriptor(dword1, dword2, &ss_descriptor);

      // selector rpl must = dpl of code segment,
      // else #TS(SS selector + ext)
      if (ss_selector.rpl != cs_descriptor.dpl) {
        BX_ERROR(("interrupt(): SS.rpl != CS.dpl"));
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // stack seg DPL must = DPL of code segment,
      // else #TS(SS selector + ext)
      if (ss_descriptor.dpl != cs_descriptor.dpl) {
        BX_ERROR(("interrupt(): SS.dpl != CS.dpl"));
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // descriptor must indicate writable data segment,
      // else #TS(SS selector + EXT)
      if (ss_descriptor.valid==0 || ss_descriptor.segment==0 ||
           IS_CODE_SEGMENT(ss_descriptor.type) ||
          !IS_DATA_SEGMENT_WRITEABLE(ss_descriptor.type))
      {
        BX_ERROR(("interrupt(): SS is not writable data segment"));
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // seg must be present, else #SS(SS selector + ext)
      if (! IS_PRESENT(ss_descriptor)) {
        BX_ERROR(("interrupt(): SS not present"));
        exception(BX_SS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // IP must be within CS segment boundaries, else #GP(0)
      if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
        BX_ERROR(("interrupt(): gate EIP > CS.limit"));
        exception(BX_GP_EXCEPTION, 0);
      }

      // Prepare new stack segment
      bx_segment_reg_t new_stack;
      new_stack.selector = ss_selector;
      new_stack.cache = ss_descriptor;
      new_stack.selector.rpl = cs_descriptor.dpl;
      // add cpl to the selector value
      new_stack.selector.value = (0xfffc & new_stack.selector.value) | new_stack.selector.rpl;

      if (ss_descriptor.u.segment.d_b) {
        Bit32u temp_ESP = ESP_for_cpl_x;

        if (is_v8086_mode)
        {
          if (gate_descriptor.type>=14) { // 386 int/trap gate
            write_new_stack_dword(&new_stack, temp_ESP-4,  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_dword(&new_stack, temp_ESP-8,  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_dword(&new_stack, temp_ESP-12, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_dword(&new_stack, temp_ESP-16, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_ESP -= 16;
          }
          else {
            write_new_stack_word(&new_stack, temp_ESP-2, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_word(&new_stack, temp_ESP-4, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_word(&new_stack, temp_ESP-6, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_word(&new_stack, temp_ESP-8, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_ESP -= 8;
          }
        }

        if (gate_descriptor.type>=14) { // 386 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_dword(&new_stack, temp_ESP-4,  cs_descriptor.dpl, old_SS);
          write_new_stack_dword(&new_stack, temp_ESP-8,  cs_descriptor.dpl, old_ESP);
          write_new_stack_dword(&new_stack, temp_ESP-12, cs_descriptor.dpl, read_eflags());
          write_new_stack_dword(&new_stack, temp_ESP-16, cs_descriptor.dpl, old_CS);
          write_new_stack_dword(&new_stack, temp_ESP-20, cs_descriptor.dpl, old_EIP);
          temp_ESP -= 20;

          if (push_error) {
            temp_ESP -= 4;
            write_new_stack_dword(&new_stack, temp_ESP, cs_descriptor.dpl, error_code);
          }
        }
        else {                          // 286 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_word(&new_stack, temp_ESP-2,  cs_descriptor.dpl, old_SS);
          write_new_stack_word(&new_stack, temp_ESP-4,  cs_descriptor.dpl, (Bit16u) old_ESP);
          write_new_stack_word(&new_stack, temp_ESP-6,  cs_descriptor.dpl, (Bit16u) read_eflags());
          write_new_stack_word(&new_stack, temp_ESP-8,  cs_descriptor.dpl, old_CS);
          write_new_stack_word(&new_stack, temp_ESP-10, cs_descriptor.dpl, (Bit16u) old_EIP);
          temp_ESP -= 10;

          if (push_error) {
            temp_ESP -= 2;
            write_new_stack_word(&new_stack, temp_ESP, cs_descriptor.dpl, error_code);
          }
        }

        ESP = temp_ESP;
      }
      else {
        Bit16u temp_SP = (Bit16u) ESP_for_cpl_x;

        if (is_v8086_mode)
        {
          if (gate_descriptor.type>=14) { // 386 int/trap gate
            write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-12), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-16), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_SP -= 16;
          }
          else {
            write_new_stack_word(&new_stack, (Bit16u)(temp_SP-2), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_word(&new_stack, (Bit16u)(temp_SP-4), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_word(&new_stack, (Bit16u)(temp_SP-6), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_word(&new_stack, (Bit16u)(temp_SP-8), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_SP -= 8;
          }
        }

        if (gate_descriptor.type>=14) { // 386 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl, old_SS);
          write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl, old_ESP);
          write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-12), cs_descriptor.dpl, read_eflags());
          write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-16), cs_descriptor.dpl, old_CS);
          write_new_stack_dword(&new_stack, (Bit16u)(temp_SP-20), cs_descriptor.dpl, old_EIP);
          temp_SP -= 20;

          if (push_error) {
            temp_SP -= 4;
            write_new_stack_dword(&new_stack, temp_SP, cs_descriptor.dpl, error_code);
          }
        }
        else {                          // 286 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_word(&new_stack, (Bit16u)(temp_SP-2),  cs_descriptor.dpl, old_SS);
          write_new_stack_word(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl, (Bit16u) old_ESP);
          write_new_stack_word(&new_stack, (Bit16u)(temp_SP-6),  cs_descriptor.dpl, (Bit16u) read_eflags());
          write_new_stack_word(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl, old_CS);
          write_new_stack_word(&new_stack, (Bit16u)(temp_SP-10), cs_descriptor.dpl, (Bit16u) old_EIP);
          temp_SP -= 10;

          if (push_error) {
            temp_SP -= 2;
            write_new_stack_word(&new_stack, temp_SP, cs_descriptor.dpl, error_code);
          }
        }

        SP = temp_SP;
      }

      // load new CS:eIP values from gate
      // set CPL to new code segment DPL
      // set RPL of CS to CPL
      load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);

      // load new SS:eSP values from TSS
      load_ss(&ss_selector, &ss_descriptor, cs_descriptor.dpl);

#if BX_SUPPORT_CET
      if(ShadowStackEnabled(old_CPL)) {
        if (old_CPL == 3)
          BX_CPU_THIS_PTR msr.ia32_pl_ssp[3] = SSP;
      }
      if (ShadowStackEnabled(CPL)) {
        bx_address old_SSP = SSP;
        shadow_stack_switch(new_SSP);
        if (old_SS_DPL != 3) {
          call_far_shadow_stack_push(old_CS, return_LIP, old_SSP);
        }
      }
      track_indirect(CPL);
#endif

      if (is_v8086_mode)
      {
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = 0;
      }
    }
    else
    {
      BX_DEBUG(("interrupt(): INTERRUPT TO SAME PRIVILEGE"));

      if (v8086_mode() && (IS_CODE_SEGMENT_CONFORMING(cs_descriptor.type) || cs_descriptor.dpl != 0)) {
        // if code segment DPL != 0 then #GP(new code segment selector)
        BX_ERROR(("interrupt(): code segment conforming or DPL(%d) != 0 in v8086 mode", cs_descriptor.dpl));
        exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
      }

      // EIP must be in CS limit else #GP(0)
      if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
        BX_ERROR(("interrupt(): IP > CS descriptor limit"));
        exception(BX_GP_EXCEPTION, 0);
      }

      // push flags onto stack
      // push current CS selector onto stack
      // push return offset onto stack
      if (gate_descriptor.type >= 14) { // 386 gate
        push_32(read_eflags());
        push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
        push_32(EIP);
        if (push_error)
          push_32(error_code);
      }
      else { // 286 gate
        push_16((Bit16u) read_eflags());
        push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
        push_16(IP);
        if (push_error)
          push_16(error_code);
      }

#if BX_SUPPORT_CET
      if(ShadowStackEnabled(CPL)) {
        call_far_shadow_stack_push(old_CS, return_LIP, SSP);
      }
      track_indirect(CPL);
#endif

      // load CS:IP from gate
      // load CS descriptor
      // set the RPL field of CS to CPL
      load_cs(&cs_selector, &cs_descriptor, CPL);
    }

    EIP = gate_dest_offset;

    // if interrupt gate then set IF to 0
    if (!(gate_descriptor.type & 1)) // even is int-gate
      BX_CPU_THIS_PTR clear_IF();
    BX_CPU_THIS_PTR clear_TF();
    BX_CPU_THIS_PTR clear_NT();
    BX_CPU_THIS_PTR clear_VM();
    BX_CPU_THIS_PTR clear_RF();
    return;
  }
  default:
    BX_PANIC(("bad descriptor type in interrupt()!"));
    break;
  }
}

void BX_CPU_C::real_mode_int(Bit8u vector, bool push_error, Bit16u error_code)
{
  if ((vector*4+3) > BX_CPU_THIS_PTR idtr.limit) {
    BX_ERROR(("interrupt(real mode) vector > idtr.limit"));
    exception(BX_GP_EXCEPTION, 0);
  }

  push_16((Bit16u) read_eflags());
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  push_16(IP);

  Bit16u new_ip = system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector);
  // CS.LIMIT can't change when in real/v8086 mode
  if (new_ip > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
    BX_ERROR(("interrupt(real mode): instruction pointer not within code segment limits"));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit16u cs_selector = system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector + 2);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_selector);
  EIP = new_ip;

  /* INT affects the following flags: I,T */
  BX_CPU_THIS_PTR clear_IF();
  BX_CPU_THIS_PTR clear_TF();
#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR clear_AC();
#endif
  BX_CPU_THIS_PTR clear_RF();
}

void BX_CPU_C::interrupt(Bit8u vector, unsigned type, bool push_error, Bit16u error_code)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_intsig;
#if BX_DEBUG_LINUX
  if (bx_dbg.linux_syscall) {
    if (vector == 0x80) bx_dbg_linux_syscall(BX_CPU_ID);
  }
#endif
  bx_dbg_interrupt(BX_CPU_ID, vector, error_code);
#endif

  BX_INSTR_INTERRUPT(BX_CPU_ID, vector);

  invalidate_prefetch_q();

  bool soft_int = false;
  switch(type) {
    case BX_SOFTWARE_INTERRUPT:
    case BX_SOFTWARE_EXCEPTION:
      soft_int = true;
      break;
    case BX_PRIVILEGED_SOFTWARE_INTERRUPT:
    case BX_EXTERNAL_INTERRUPT:
    case BX_NMI:
    case BX_HARDWARE_EXCEPTION:
      break;

    default:
      BX_PANIC(("interrupt(): unknown exception type %d", type));
  }

  BX_DEBUG(("interrupt(): vector = %02x, TYPE = %u, EXT = %u",
      vector, type, (unsigned) BX_CPU_THIS_PTR EXT));

  // Discard any traps and inhibits for new context; traps will
  // resume upon return.
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR inhibit_mask = 0;

#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  BX_CPU_THIS_PTR in_event = true;
#endif

  RSP_SPECULATIVE;

#if BX_SUPPORT_X86_64
  if (long_mode()) {
    long_mode_int(vector, soft_int, push_error, error_code);
  }
  else
#endif
  {
    // software interrupt can be redirected in v8086 mode
    if (type != BX_SOFTWARE_INTERRUPT || !v8086_mode() || !v86_redirect_interrupt(vector))
    {
      if(real_mode()) {
        real_mode_int(vector, push_error, error_code);
      }
      else {
        protected_mode_int(vector, soft_int, push_error, error_code);
      }
    }
  }

  RSP_COMMIT;

#if BX_X86_DEBUGGER
  BX_CPU_THIS_PTR in_repeat = false;
#endif

#if BX_SUPPORT_VMX
  unmask_event(BX_EVENT_VMX_MONITOR_TRAP_FLAG);
#endif

#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  BX_CPU_THIS_PTR in_event = false;
#endif

  BX_CPU_THIS_PTR EXT = 0;
}

/* Exception classes.  These are used as indexes into the 'is_exception_OK'
 * array below, and are stored in the 'exception' array also
 */
enum {
  BX_ET_BENIGN = 0,
  BX_ET_CONTRIBUTORY = 1,
  BX_ET_PAGE_FAULT = 2,
  BX_ET_DOUBLE_FAULT = 10
};

static const bool is_exception_OK[3][3] = {
    { 1, 1, 1 }, /* 1st exception is BENIGN */
    { 1, 0, 1 }, /* 1st exception is CONTRIBUTORY */
    { 1, 0, 0 }  /* 1st exception is PAGE_FAULT */
};

enum {
  BX_EXCEPTION_CLASS_TRAP = 0,
  BX_EXCEPTION_CLASS_FAULT = 1,
  BX_EXCEPTION_CLASS_ABORT = 2
};

struct BxExceptionInfo exceptions_info[BX_CPU_HANDLED_EXCEPTIONS] = {
  /* DE */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 0 },
  /* DB */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 02 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // NMI
  /* BP */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_TRAP,  0 },
  /* OF */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_TRAP,  0 },
  /* BR */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* UD */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* NM */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* DF */ { BX_ET_DOUBLE_FAULT, BX_EXCEPTION_CLASS_FAULT, 1 },
             // coprocessor segment overrun (286,386 only)
  /* 09 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* TS */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* NP */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* SS */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* GP */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* PF */ { BX_ET_PAGE_FAULT,   BX_EXCEPTION_CLASS_FAULT, 1 },
  /* 15 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // reserved
  /* MF */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* AC */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 1 },
  /* MC */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_ABORT, 0 },
  /* XM */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* VE */ { BX_ET_PAGE_FAULT,   BX_EXCEPTION_CLASS_FAULT, 0 },
  /* CP */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* 22 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 23 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 24 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 25 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 26 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 27 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 28 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 29 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 30 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // FIXME: SVM #SF
  /* 31 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }
};

// vector:     0..255: vector in IDT
// error_code: if exception generates and error, push this error code
void BX_CPU_C::exception(unsigned vector, Bit16u error_code)
{
  unsigned exception_type = 0;
  unsigned exception_class = BX_EXCEPTION_CLASS_FAULT;
  bool push_error = false;

  if (vector < BX_CPU_HANDLED_EXCEPTIONS) {
     push_error = exceptions_info[vector].push_error;
     exception_class = exceptions_info[vector].exception_class;
     exception_type = exceptions_info[vector].exception_type;
  }
  else {
     BX_PANIC(("exception(%u): bad vector", vector));
  }

  /* Excluding page faults and double faults, error_code may not have the
   * least significant bit set correctly. This correction is applied first
   * to make the change transparent to any instrumentation.
   */
  if (vector != BX_PF_EXCEPTION && vector != BX_DF_EXCEPTION && vector != BX_CP_EXCEPTION) {
    // Page faults have different format
    error_code = (error_code & 0xfffe) | (Bit16u)(BX_CPU_THIS_PTR EXT);
  }

  BX_INSTR_EXCEPTION(BX_CPU_ID, vector, error_code);

#if BX_DEBUGGER
  bx_dbg_exception(BX_CPU_ID, vector, error_code);
#endif

  BX_DEBUG(("exception(0x%02x): error_code=%04x", vector, error_code));

#if BX_SUPPORT_VMX
  VMexit_Event(BX_HARDWARE_EXCEPTION, vector, error_code, push_error);
#endif

#if BX_SUPPORT_SVM
  SvmInterceptException(BX_HARDWARE_EXCEPTION, vector, error_code, push_error);
#endif

  if (exception_class == BX_EXCEPTION_CLASS_FAULT)
  {
    // restore RIP/RSP to value before error occurred
    RIP = BX_CPU_THIS_PTR prev_rip;
    if (BX_CPU_THIS_PTR speculative_rsp) {
      RSP = BX_CPU_THIS_PTR prev_rsp;
#if BX_SUPPORT_CET
      SSP = BX_CPU_THIS_PTR prev_ssp;
#endif
    }
    BX_CPU_THIS_PTR speculative_rsp = false;

    if (BX_CPU_THIS_PTR last_exception_type == BX_ET_DOUBLE_FAULT)
    {
      debug(BX_CPU_THIS_PTR prev_rip); // print debug information to the log
#if BX_SUPPORT_VMX
      VMexit_TripleFault();
#endif
#if BX_DEBUGGER
      // trap into debugger (the same as when a PANIC occurs)
      bx_debug_break();
#endif
      if (SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT)->get()) {
        BX_ERROR(("exception(): 3rd (%d) exception with no resolution, shutdown status is %02xh, resetting", vector, DEV_cmos_get_reg(0x0f)));
        bx_pc_system.Reset(BX_RESET_HARDWARE);
      }
      else {
        BX_PANIC(("exception(): 3rd (%d) exception with no resolution", vector));
        BX_ERROR(("WARNING: Any simulation after this point is completely bogus !"));
        shutdown();
      }
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
    }

    if (vector != BX_DB_EXCEPTION) BX_CPU_THIS_PTR assert_RF();
  }

  if (vector == BX_DB_EXCEPTION) {
    // Commit debug events to DR6: preserve DR5.BS and DR6.BD values,
    // only software can clear them
    BX_CPU_THIS_PTR dr6.val32 = (BX_CPU_THIS_PTR dr6.val32  & 0xffff6ff0) |
                                (BX_CPU_THIS_PTR debug_trap & 0x0000e00f);

    // clear GD flag in the DR7 prior entering debug exception handler
    BX_CPU_THIS_PTR dr7.set_GD(0);
  }

  BX_CPU_THIS_PTR EXT = 1;

  /* if we've already had 1st exception, see if 2nd causes a
   * Double Fault instead. Otherwise, just record 1st exception.
   */
  if (exception_type != BX_ET_DOUBLE_FAULT) {
    if (! is_exception_OK[BX_CPU_THIS_PTR last_exception_type][exception_type]) {
      exception(BX_DF_EXCEPTION, 0);
    }
  }

  BX_CPU_THIS_PTR last_exception_type = exception_type;

  if (real_mode()) {
    push_error = false; // not INT, no error code pushed
    error_code = 0;
  }

  interrupt(vector, BX_HARDWARE_EXCEPTION, push_error, error_code);

  BX_CPU_THIS_PTR last_exception_type = 0; // error resolved

  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}
