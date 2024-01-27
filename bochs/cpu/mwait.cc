/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2019-2023  The Bochs Project
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
#include "gui/siminterface.h"
#include "param_names.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_SVM
#include "svm.h"
#endif

#if BX_SUPPORT_APIC
#include "apic.h"
#endif

#include "pc_system.h"

#include "decoder/ia_opcodes.h"

#if BX_SUPPORT_MONITOR_MWAIT
bool BX_CPU_C::is_monitor(bx_phy_address begin_addr, unsigned len)
{
  if (! BX_CPU_THIS_PTR monitor.armed()) return false;

  bx_phy_address monitor_begin = BX_CPU_THIS_PTR monitor.monitor_addr;
  bx_phy_address monitor_end = monitor_begin + CACHE_LINE_SIZE - 1;

  bx_phy_address end_addr = begin_addr + len;
  if (begin_addr >= monitor_end || end_addr <= monitor_begin)
    return false;
  else
    return true;
}

void BX_CPU_C::check_monitor(bx_phy_address begin_addr, unsigned len)
{
  if (is_monitor(begin_addr, len)) wakeup_monitor();
}

void BX_CPU_C::wakeup_monitor(void)
{
  // wakeup from MWAIT state
  if(BX_CPU_THIS_PTR activity_state >= BX_ACTIVITY_STATE_MWAIT)
     BX_CPU_THIS_PTR activity_state = BX_ACTIVITY_STATE_ACTIVE;
  // clear monitor
  BX_CPU_THIS_PTR monitor.reset_monitor();
  // deactivate mwaitx timer if was active to avoid its redundant firing
  BX_CPU_THIS_PTR lapic->deactivate_mwaitx_timer();
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MONITOR(bxInstruction_c *i)
{
#if BX_SUPPORT_MONITOR_MWAIT
  BX_DEBUG(("%s instruction executed RAX = 0x" FMT_ADDRX, i->getIaOpcodeNameShort(), RAX));

  if (i->getIaOpcode() == BX_IA_MONITOR) {
    // CPL is always 0 in real mode
    if (CPL != 0) {
      BX_DEBUG(("%s: instruction not recognized when CPL != 0", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

#if BX_SUPPORT_VMX
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.MONITOR_VMEXIT()) {
        VMexit(VMX_VMEXIT_MONITOR, 0);
      }
    }
#endif
  }

  if (RCX != 0) {
    BX_ERROR(("%s: no optional extensions supported", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address eaddr = RAX & i->asize_mask();

  // MONITOR/MONITORX performs the same segmentation and paging checks as a 1-byte read
  tickle_read_virtual(i->seg(), eaddr);

  bx_phy_address paddr = BX_CPU_THIS_PTR address_xlation.paddress1;
#if BX_SUPPORT_MEMTYPE
  if (BX_CPU_THIS_PTR address_xlation.memtype1 != BX_MEMTYPE_WB) {
    BX_DEBUG(("%s for non-WB memory type phys_addr=0x" FMT_PHY_ADDRX, i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR monitor.monitor_addr));
    BX_NEXT_INSTR(i);
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_MONITOR)) Svm_Vmexit(SVM_VMEXIT_MONITOR);
  }
#endif

  // Set the monitor immediately. If monitor is still armed when we MWAIT,
  // the processor will stall.
  bx_pc_system.invlpg(paddr);

  BX_CPU_THIS_PTR monitor.arm(paddr, (i->getIaOpcode() == BX_IA_MONITOR) ? BX_MONITOR_ARMED_BY_MONITOR : BX_MONITOR_ARMED_BY_MONITORX);

  BX_DEBUG(("%s: for phys_addr=0x" FMT_PHY_ADDRX, i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR monitor.monitor_addr));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MWAIT(bxInstruction_c *i)
{
#if BX_SUPPORT_MONITOR_MWAIT
  BX_DEBUG(("%s instruction executed ECX = 0x%08x", i->getIaOpcodeNameShort(), ECX));

  if (i->getIaOpcode() == BX_IA_MWAIT) {
    // CPL is always 0 in real mode
    if (CPL != 0) {
      BX_DEBUG(("%s: instruction not recognized when CPL != 0", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

#if BX_SUPPORT_VMX
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.MWAIT_VMEXIT()) {
        VMexit(VMX_VMEXIT_MWAIT, BX_CPU_THIS_PTR monitor.armed_by_monitor());
      }
    }
#endif
  }

  // extension supported:
  //   ECX[0] - interrupt MWAIT even if EFLAGS.IF = 0
  //   ECX[1] - timed MWAITX
  // all other bits are reserved
  if (i->getIaOpcode() == BX_IA_MWAITX) {
    if (RCX & ~(BX_CONST64(3))) {
      BX_ERROR(("%s: incorrect optional extensions in RCX", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }
  }
  else {
    if (RCX & ~(BX_CONST64(1))) {
      BX_ERROR(("%s: incorrect optional extensions in RCX", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }
  }

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_MWAIT_ARMED))
      if (BX_CPU_THIS_PTR monitor.armed()) Svm_Vmexit(SVM_VMEXIT_MWAIT_CONDITIONAL);

    if (SVM_INTERCEPT(SVM_INTERCEPT1_MWAIT)) Svm_Vmexit(SVM_VMEXIT_MWAIT);
  }
#endif

  // If monitor has already triggered, we just return.
  bool monitor_armed = true;
  if (i->getIaOpcode() == BX_IA_MWAITX) {
    if (! BX_CPU_THIS_PTR monitor.armed_by_monitorx()) {
      monitor_armed = false;
    }
  }
  else {
    if (! BX_CPU_THIS_PTR monitor.armed_by_monitor()) {
      monitor_armed = false;
    }
  }

  if (! monitor_armed) {
    BX_DEBUG(("%s: the MONITOR was not armed or already triggered", i->getIaOpcodeNameShort()));
    BX_NEXT_TRACE(i);
  }

  static bool mwait_is_nop = SIM->get_param_bool(BXPN_MWAIT_IS_NOP)->get();
  if (mwait_is_nop) {
    BX_NEXT_TRACE(i);
  }

  // stops instruction execution and places the processor in a optimized
  // state.  Events that cause exit from MWAIT state are:
  // A store from another processor to monitored range, any unmasked
  // interrupt, including INTR, NMI, SMI, INIT or reset will resume
  // the execution. Any far control transfer between MONITOR and MWAIT
  // resets the monitoring logic.

  Bit32u new_state = BX_ACTIVITY_STATE_MWAIT;
  if (ECX & 0x1) {
#if BX_SUPPORT_VMX
    // When "interrupt window exiting" VMX control is set MWAIT instruction
    // won't cause the processor to enter sleep state with EFLAGS.IF = 0
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.INTERRUPT_WINDOW_VMEXIT() && ! BX_CPU_THIS_PTR get_IF()) {
        BX_NEXT_TRACE(i);
      }
    }
#endif
    new_state = BX_ACTIVITY_STATE_MWAIT_IF;
  }

  BX_INSTR_MWAIT(BX_CPU_ID, BX_CPU_THIS_PTR monitor.monitor_addr, CACHE_LINE_SIZE, ECX);

  if (i->getIaOpcode() == BX_IA_MWAITX) {
    if ((ECX & 0x2) != 0 && EBX != 0) {
      BX_CPU_THIS_PTR lapic->set_mwaitx_timer(EBX);
    }
  }

  enter_sleep_state(new_state);
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::UMONITOR_Eq(bxInstruction_c *i)
{
#if BX_SUPPORT_MONITOR_MWAIT

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (! BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.UMWAIT_TPAUSE_VMEXIT()) {
      BX_DEBUG(("%s: instruction is not enabled in VMX guest", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }
  }
#endif

#if BX_SUPPORT_X86_64
  bx_address eaddr = BX_READ_64BIT_REG(i->dst()) & i->asize_mask();
#else
  bx_address eaddr = BX_READ_32BIT_REG(i->dst()) & i->asize_mask();
#endif

  // UMONITOR performs the same segmentation and paging checks as a 1-byte read
  tickle_read_virtual(i->seg(), eaddr);

  bx_phy_address paddr = BX_CPU_THIS_PTR address_xlation.paddress1;
#if BX_SUPPORT_MEMTYPE
  if (BX_CPU_THIS_PTR address_xlation.memtype1 != BX_MEMTYPE_WB) {
    BX_DEBUG(("UMONITOR for non-WB memory type phys_addr=0x" FMT_PHY_ADDRX, BX_CPU_THIS_PTR monitor.monitor_addr));
    BX_NEXT_INSTR(i);
  }
#endif

  // Set the monitor immediately. If monitor is still armed when we MWAIT,
  // the processor will stall.
  bx_pc_system.invlpg(paddr);

  BX_CPU_THIS_PTR monitor.arm(paddr, BX_MONITOR_ARMED_BY_UMONITOR);

  BX_DEBUG(("UMONITOR for phys_addr=0x" FMT_PHY_ADDRX, BX_CPU_THIS_PTR monitor.monitor_addr));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::UMWAIT_Ed(bxInstruction_c *i)
{
#if BX_SUPPORT_MONITOR_MWAIT
  BX_DEBUG(("%s instruction executed EAX = 0x%08x EDX = 0x%08x", i->getIaOpcodeNameShort(), EAX, EDX));

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (! BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.UMWAIT_TPAUSE_VMEXIT()) {
      BX_DEBUG(("%s: instruction is not enabled in VMX guest", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }
  }
#endif

  if (BX_CPU_THIS_PTR cr4.get_TSD() && CPL != 0) {
    BX_ERROR(("%s: not allowed to use instruction !", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest && BX_CPU_THIS_PTR vmcs.vmexec_ctrls1.RDTSC_VMEXIT()) {
    VMexit((i->getIaOpcode() == BX_IA_TPAUSE_Ed) ? VMX_VMEXIT_TPAUSE : VMX_VMEXIT_UMWAIT, 0);
  }
#endif

  Bit32u req_sleep_state = BX_READ_32BIT_REG(i->dst());
  if (req_sleep_state & ~0x1) {
    BX_ERROR(("%s: incorrect sleep state 0x%08x - #GP(0)", i->getIaOpcodeNameShort(), req_sleep_state));
    exception(BX_GP_EXCEPTION, 0);
  }

  clearEFlagsOSZAPC();

  if (i->getIaOpcode() != BX_IA_TPAUSE_Ed) {
    // If monitor has already triggered, we just return.
    if (! BX_CPU_THIS_PTR monitor.armed_by_umonitor()) {
      BX_DEBUG(("%s: the UMONITOR was not armed or already triggered", i->getIaOpcodeNameShort()));
      BX_NEXT_TRACE(i);
    }
  }
  else {
    BX_CPU_THIS_PTR monitor.reset_umonitor();
  }

  static bool mwait_is_nop = SIM->get_param_bool(BXPN_MWAIT_IS_NOP)->get();
  if (mwait_is_nop) {
    BX_NEXT_TRACE(i);
  }

  Bit64u tsc = get_Virtual_TSC();
  Bit64u instr_deadline = GET64_FROM_HI32_LO32(EDX, EAX);
  if (instr_deadline <= tsc) {
    BX_DEBUG(("%s: requested deadline is in the past", i->getIaOpcodeNameShort()));
    BX_NEXT_TRACE(i);
  }
  Bit64u instr_delay = instr_deadline - tsc;

  Bit32u umwait_control_max_delay = (BX_CPU_THIS_PTR msr.ia32_umwait_ctrl & ~0x3);
//bool using_os_deadline = false; // FIXME
  if (umwait_control_max_delay < instr_delay) {
    instr_delay = umwait_control_max_delay;
//  using_os_deadline = true;
  }

  BX_ASSERT(instr_delay > 0);
#if BX_SUPPORT_VMX
  instr_delay = compute_physical_TSC_delay(instr_delay);
#endif

  BX_CPU_THIS_PTR lapic->set_mwaitx_timer(instr_delay);

  // An external interrupt causes the processor to exit the implementation-dependent optimized state 
  // regardless of whether maskable-interrupts are inhibited (EFLAGS.IF =0)
  Bit32u sleep_state = BX_ACTIVITY_STATE_MWAIT_IF;

  BX_INSTR_MWAIT(BX_CPU_ID, BX_CPU_THIS_PTR monitor.monitor_addr, CACHE_LINE_SIZE, sleep_state);

  enter_sleep_state(sleep_state);

//if (using_os_deadline && tsc >= deadline)
//  assert_CF();
#endif

  BX_NEXT_TRACE(i);
}
