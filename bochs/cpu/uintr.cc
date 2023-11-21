/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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

#if BX_SUPPORT_UINTR && BX_SUPPORT_X86_64

#include "scalar_arith.h"   // for lzcntq

void BX_CPU_C::deliver_UINTR()
{
  BX_ASSERT(BX_CPU_THIS_PTR cr4.get_UINTR() && !uintr_masked());

  if (!IsCanonical(BX_CPU_THIS_PTR uintr.ui_handler)) {
    BX_ERROR(("UINTR.UI_HANDLER is not canonical: #GP(0)"));
    exception(BX_GP_EXCEPTION, 0);
  }

  RSP_SPECULATIVE;

  Bit64u tmp_RSP = RSP;
  if (BX_CPU_THIS_PTR uintr.stack_adjust & 0x1)
    RSP  = BX_CPU_THIS_PTR uintr.stack_adjust;
  else
    RSP -= BX_CPU_THIS_PTR uintr.stack_adjust;
  RSP &= BX_CONST64(0xfffffffffffffff0);

  push_64(tmp_RSP);
  push_64(read_eflags());
  push_64(RIP);
  unsigned vector = lzcntq(BX_CPU_THIS_PTR uintr.uirr); // find #most significant bit in UIRR
  // expected to be not 0
  push_64(vector);

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(3))
    shadow_stack_push_64(RIP);
  track_indirect(3);
#endif

  RSP_COMMIT;

  BX_CPU_THIS_PTR uintr.uirr &= ~(BX_CONST64(1) << vector);
  uintr_uirr_update(); // potentially clear user-level-interrupt

  BX_CPU_THIS_PTR uintr.UIF = 0;
  BX_CPU_THIS_PTR clear_TF();
  BX_CPU_THIS_PTR clear_RF();
  RIP = BX_CPU_THIS_PTR uintr.ui_handler;

  uintr_control(); // UIF changed
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::UIRET(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_UINTR()) {
    BX_ERROR(("%s: UINTR in not enabled in CR4", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_INSTR_FAR_BRANCH_ORIGIN();

  Bit64u new_rip      = stack_read_qword(RSP + 8);
  Bit32u new_eflags   = (Bit32u) stack_read_qword(RSP + 16);
  Bit64u new_rsp      = stack_read_qword(RSP + 24);

  if (!IsCanonical(new_rip)) {
    BX_ERROR(("UIRET #GP(0): return RIP is not canonical"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL)) {
    Bit64u shadow_RIP  = shadow_stack_pop_64();
    if (new_rip != shadow_RIP) {
      BX_ERROR(("shadow_stack_restore: LIP mismatch"));
      exception(BX_CP_EXCEPTION, BX_CP_FAR_RET_IRET);
    }
  }
#endif

  RIP = new_rip;
  // update in RFLAGS only CF, PF, AF, ZF, SF, TF, DF, OF, NT, RF, AC, and ID
  writeEFlags(new_eflags, 0x254DD5);
  RSP = new_rsp;

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  BX_CPU_THIS_PTR uintr.UIF = 1;
  uintr_control(); // potentially enable user interrupt delivery

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_UIRET,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::STUI(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_UINTR()) {
    BX_ERROR(("%s: UINTR in not enabled in CR4", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }
  BX_CPU_THIS_PTR uintr.UIF = 1;
  uintr_control(); // potentially enable user interrupt delivery

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLUI(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_UINTR()) {
    BX_ERROR(("%s: UINTR in not enabled in CR4", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }
  BX_CPU_THIS_PTR uintr.UIF = 0;
  uintr_control(); // disable user interrupt delivery

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TESTUI(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_UINTR()) {
    BX_ERROR(("%s: UINTR in not enabled in CR4", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }
  clearEFlagsOSZAPC();
  BX_CPU_THIS_PTR set_CF(BX_CPU_THIS_PTR uintr.UIF);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SENDUIPI_Gq(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_UINTR()) {
    BX_ERROR(("%s: UINTR in not enabled in CR4", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (! BX_CPU_THIS_PTR uintr.senduipi_enabled()) {
    BX_ERROR(("SENDUIPI in disabled by IA32_UINTR_TT[0]"));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit64u index = BX_READ_64BIT_REG(i->src());
  if (index > BX_CPU_THIS_PTR uintr.uitt_size) {
    BX_ERROR(("SENDUIPI: value of the source operand exceeds UITT.SIZE"));
    exception(BX_GP_EXCEPTION, 0);
  }

  // ----------- UITT format --------
  // |     00 | Valid
  // |  07:01 | Reserved, MBZ
  // |  15:08 | UVector, user-level interrupt vector, must be < 64
  // |  63:16 | Reserved, MBZ
  // | 127:64 | UPID_ADDR, linear address of UPID, must be 64-byte aligned
  // --------------------------------

  Bit64u tmpUITT_entry_lo = system_read_qword(BX_CPU_THIS_PTR uintr.uitt_addr + index * 16);
  Bit64u tmpUITT_upidaddr = system_read_qword(BX_CPU_THIS_PTR uintr.uitt_addr + index * 16 + 8);
  if ((tmpUITT_entry_lo & 0x1) == 0) {
    BX_ERROR(("SENDUIPI #GP(0): invalid UITT entry"));
    exception(BX_GP_EXCEPTION, 0);
  }
  if (tmpUITT_entry_lo & BX_CONST64(0xFFFFFFFFFFFF00FE)) {
    BX_ERROR(("SENDUIPI #GP(0): invalid UITT entry, reserved bits set"));
    exception(BX_GP_EXCEPTION, 0);
  }
  unsigned uvector = (tmpUITT_entry_lo >> 8) & 0xFF;
  if (uvector >= 64) {
    BX_ERROR(("SENDUIPI #GP(0): UVector=%d >= 64", uvector));
    exception(BX_GP_EXCEPTION, 0);
  }

  // ----------- UPID format --------
  // |     00 | Outstanding notification (UPID.ON)
  // |     01 | Suppress notification    (UPID.SN)
  // |  15:02 | Reserved, MBZ
  // |  23:16 | Notification vector, 8-bit
  // |  31:24 | Reserved, MBZ
  // |  63:32 | Notification destination, 32-bit APIC/X2APIC ID
  // | 127:64 | Posted interrupt requests (PIR)
  // |        | One bit for ech user-interrupt vector
  // --------------------------------

  bool send_notify = false;
  Bit32u notification_vector = 0;
  Bit32u notification_destination = 0;

// should be done atomically using RMW
  Bit64u tmpUPID_lo = system_read_qword(tmpUITT_upidaddr);
  Bit64u tmpUPID_hi = system_read_qword(tmpUITT_upidaddr + 8);
  if (tmpUPID_lo & BX_CONST64(0xFF00FFFC)) {
    BX_ERROR(("SENDUIPI #GP(0): invalid UPID, reserved bits set"));
    exception(BX_GP_EXCEPTION, 0);
  }

  tmpUPID_hi |= (BX_CONST64(1) << uvector); // PIR[uvector] = 1
  if ((tmpUPID_lo & 0x3) == 0) {
    tmpUPID_lo |= 1;
    send_notify = true;
    notification_vector = GET32L(tmpUPID_lo) >> 16;
    notification_destination = GET32H(tmpUPID_lo);
  }

  system_write_qword(tmpUITT_upidaddr,     tmpUPID_lo);
  system_write_qword(tmpUITT_upidaddr + 8, tmpUPID_hi);
// should be done atomically using RMW

  if (send_notify) {
    // if in X2APIC mode:
    //   send IPI [vector=notification_vector] to 32-bit APIC_ID=[notification_destination]
    // else:
    //   send IPI [vector=notification_vector] to  8-bit APIC_ID=[notification_destination >> 8]
  }

  BX_NEXT_TRACE(i);
}

void BX_CPU_C::process_uintr_notification()
{
#if BX_SUPPORT_VMX
  // the User-Level Interrupt notification process looks like external interrupt with the vector UINV
  // in particular for IDT-vectoring info
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    vm->idt_vector_error_code = 0;
    vm->idt_vector_info = (BX_CPU_THIS_PTR uintr.uinv) | (BX_EXTERNAL_INTERRUPT << 8);
  }
#endif

  // Step 1: Clear UPID.ON atomically with locked AND operation

// should be done atomically using RMW
  Bit8u tmpUPID_lo8 = system_read_byte(BX_CPU_THIS_PTR uintr.upid_addr);
  tmpUPID_lo8 &= 0xFE;
  system_write_byte(BX_CPU_THIS_PTR uintr.upid_addr, tmpUPID_lo8);
// should be done atomically using RMW

  // Step 2: Read UPID.PIR into temporary PIR register and write '0 to the PIR using locked XCHG operation

// should be done atomically using RMW
  Bit64u PIR = system_read_qword(BX_CPU_THIS_PTR uintr.upid_addr + 8);
  system_write_qword(BX_CPU_THIS_PTR uintr.upid_addr + 8, 0);
// should be done atomically using RMW

  // Step 3: for any bit set in the temporary PIR register set corresponding UIRR bit
  //         recognize pending user-level interrupt
  BX_CPU_THIS_PTR uintr.uirr |= PIR;
  uintr_uirr_update(); // potentially signal user-level-interrupt
}

bool BX_CPU_C::uintr_masked()
{
  // The user-level interrupt can be delivered only if:
  //   long64_mode()
  //   UIF = 1
  //   CPL = 3
  return !long64_mode() || !BX_CPU_THIS_PTR uintr.UIF || CPL != 3;
}

// mask or unmask BX_EVENT_PENDING_UINTR according to conditions telling if it can be potentially delivered
void BX_CPU_C::uintr_uirr_update()
{
  // There is a pending user-level interrupt if UINTR.UIRR != 0
  if (BX_CPU_THIS_PTR cr4.get_UINTR() && BX_CPU_THIS_PTR uintr.uirr)
    signal_event(BX_EVENT_PENDING_UINTR);
  else
    clear_event(BX_EVENT_PENDING_UINTR);
}

void BX_CPU_C::uintr_control()
{
  if (uintr_masked())
    mask_event(BX_EVENT_PENDING_UINTR);
  else
    unmask_event(BX_EVENT_PENDING_UINTR);
}

#endif
