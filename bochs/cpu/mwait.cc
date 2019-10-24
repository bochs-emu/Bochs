/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2019  The Bochs Project
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
#include "param_names.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "decoder/ia_opcodes.h"

#if BX_SUPPORT_MONITOR_MWAIT
bx_bool BX_CPU_C::is_monitor(bx_phy_address begin_addr, unsigned len)
{
  if (! BX_CPU_THIS_PTR monitor.armed) return 0;

  bx_phy_address monitor_begin = BX_CPU_THIS_PTR monitor.monitor_addr;
  bx_phy_address monitor_end = monitor_begin + CACHE_LINE_SIZE - 1;

  bx_phy_address end_addr = begin_addr + len;
  if (begin_addr >= monitor_end || end_addr <= monitor_begin)
    return 0;
  else
    return 1;
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
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MONITOR(bxInstruction_c *i)
{
#if BX_SUPPORT_MONITOR_MWAIT
  BX_DEBUG(("%s instruction executed EAX = 0x%08x", i->getIaOpcodeNameShort(), EAX));

  if (i->getIaOpcode() == BX_IA_MONITOR) {
    // CPL is always 0 in real mode
    if (CPL != 0) {
      BX_DEBUG(("%s: instruction not recognized when CPL != 0", i->getIaOpcodeNameShort()));
      exception(BX_UD_EXCEPTION, 0);
    }

#if BX_SUPPORT_VMX
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (VMEXIT(VMX_VM_EXEC_CTRL2_MONITOR_VMEXIT)) {
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
  if (BX_CPU_THIS_PTR address_xlation.memtype1 != BX_MEMTYPE_WB) return;
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_MONITOR)) Svm_Vmexit(SVM_VMEXIT_MONITOR);
  }
#endif

  // Set the monitor immediately. If monitor is still armed when we MWAIT,
  // the processor will stall.

  bx_pc_system.invlpg(paddr);

  BX_CPU_THIS_PTR monitor.arm(paddr);

  BX_DEBUG(("MONITOR for phys_addr=0x" FMT_PHY_ADDRX, BX_CPU_THIS_PTR monitor.monitor_addr));
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
      if (VMEXIT(VMX_VM_EXEC_CTRL2_MWAIT_VMEXIT)) {
        VMexit(VMX_VMEXIT_MWAIT, BX_CPU_THIS_PTR monitor.armed);
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
      if (BX_CPU_THIS_PTR monitor.armed) Svm_Vmexit(SVM_VMEXIT_MWAIT_CONDITIONAL);
    
    if (SVM_INTERCEPT(SVM_INTERCEPT1_MWAIT)) Svm_Vmexit(SVM_VMEXIT_MWAIT);
  }
#endif

  // If monitor has already triggered, we just return.
  if (! BX_CPU_THIS_PTR monitor.armed) {
    BX_DEBUG(("%s: the MONITOR was not armed or already triggered", i->getIaOpcodeNameShort()));
    BX_NEXT_TRACE(i);
  }

  static bx_bool mwait_is_nop = SIM->get_param_bool(BXPN_MWAIT_IS_NOP)->get();
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
  if (ECX & 1) {
#if BX_SUPPORT_VMX
    // When "interrupt window exiting" VMX control is set MWAIT instruction
    // won't cause the processor to enter sleep state with EFLAGS.IF = 0
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (VMEXIT(VMX_VM_EXEC_CTRL2_INTERRUPT_WINDOW_VMEXIT) && ! BX_CPU_THIS_PTR get_IF()) {
        BX_NEXT_TRACE(i);
      }
    }
#endif
    new_state = BX_ACTIVITY_STATE_MWAIT_IF;
  }

  BX_INSTR_MWAIT(BX_CPU_ID, BX_CPU_THIS_PTR monitor.monitor_addr, CACHE_LINE_SIZE, ECX);

  if (ECX & 2) {
    if (i->getIaOpcode() == BX_IA_MWAITX) {
      BX_CPU_THIS_PTR lapic.set_mwaitx_timer(EBX);
    }
  }

  enter_sleep_state(new_state);
#endif

  BX_NEXT_TRACE(i);
}
