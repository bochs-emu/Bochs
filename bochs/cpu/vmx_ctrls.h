/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024 Stanislav Shwartsman
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

#ifndef _BX_VMX_CONTROLS_INTEL_H_
#define _BX_VMX_CONTROLS_INTEL_H_

class VmxPinBasedVmexecControls {
private:
   Bit32u pin_vmexec_ctrls;

public:
   VmxPinBasedVmexecControls(Bit32u ctrls = 0): pin_vmexec_ctrls(ctrls) {}

#define VMX_PIN_BASED_VMEXEC_CTRL_EXTERNAL_INTERRUPT_VMEXIT   (1 << 0)
#define VMX_PIN_BASED_VMEXEC_CTRL_NMI_EXITING                 (1 << 3)
#define VMX_PIN_BASED_VMEXEC_CTRL_VIRTUAL_NMI                 (1 << 5) /* Virtual NMI */
#define VMX_PIN_BASED_VMEXEC_CTRL_VMX_PREEMPTION_TIMER_VMEXIT (1 << 6) /* VMX preemption timer */
#define VMX_PIN_BASED_VMEXEC_CTRL_PROCESS_POSTED_INTERRUPTS   (1 << 7) /* Posted Interrupts */

   // sorry for capital letters; for backward compatibility with existing defines
   bool EXTERNAL_INTERRUPT_VMEXIT() const { return pin_vmexec_ctrls & VMX_PIN_BASED_VMEXEC_CTRL_EXTERNAL_INTERRUPT_VMEXIT; }
   bool NMI_EXITING() const { return pin_vmexec_ctrls & VMX_PIN_BASED_VMEXEC_CTRL_NMI_EXITING; }
   bool VIRTUAL_NMI() const { return pin_vmexec_ctrls & VMX_PIN_BASED_VMEXEC_CTRL_VIRTUAL_NMI; }
   bool VMX_PREEMPTION_TIMER_VMEXIT() const { return pin_vmexec_ctrls & VMX_PIN_BASED_VMEXEC_CTRL_VMX_PREEMPTION_TIMER_VMEXIT; }
   bool PROCESS_POSTED_INTERRUPTS() const { return pin_vmexec_ctrls & VMX_PIN_BASED_VMEXEC_CTRL_PROCESS_POSTED_INTERRUPTS; }

   bool query_any(Bit32u mask) const { return (pin_vmexec_ctrls & mask) != 0; }
   bool query_all(Bit32u mask) const { return (pin_vmexec_ctrls & mask) == mask; }

   Bit32u get() const { return pin_vmexec_ctrls; }

   Bit32u* getref() { return &pin_vmexec_ctrls; }
};

class VmxVmentryControls {
private:
   Bit32u vmentry_ctrls;

public:
   VmxVmentryControls(Bit32u ctrls = 0): vmentry_ctrls(ctrls) {}

#define VMX_VMENTRY_CTRL1_LOAD_DBG_CTRLS                    (1 <<  2) /* legacy must be '1 */
#define VMX_VMENTRY_CTRL1_X86_64_GUEST                      (1 <<  9)
#define VMX_VMENTRY_CTRL1_SMM_ENTER                         (1 << 10)
#define VMX_VMENTRY_CTRL1_DEACTIVATE_DUAL_MONITOR_TREATMENT (1 << 11)
#define VMX_VMENTRY_CTRL1_LOAD_PERF_GLOBAL_CTRL_MSR         (1 << 13) /* Perf Global Ctrl */
#define VMX_VMENTRY_CTRL1_LOAD_PAT_MSR                      (1 << 14) /* PAT */
#define VMX_VMENTRY_CTRL1_LOAD_EFER_MSR                     (1 << 15) /* EFER */
#define VMX_VMENTRY_CTRL1_LOAD_BNDCFGS                      (1 << 16) /* MPX (not implemented) */
#define VMX_VMENTRY_CTRL1_SUPPRESS_VMX_PACKETS              (1 << 17) /* Processor Trace (not implemented) */
#define VMX_VMENTRY_CTRL1_LOAD_GUEST_RTIT_CTRL              (1 << 18) // not implemented
#define VMX_VMENTRY_CTRL1_LOAD_UINV                         (1 << 19) /* UINTR */
#define VMX_VMENTRY_CTRL1_LOAD_GUEST_CET_STATE              (1 << 20) /* CET */
#define VMX_VMENTRY_CTRL1_LOAD_GUEST_LBR_CTRL               (1 << 21) // not implemented
#define VMX_VMENTRY_CTRL1_LOAD_GUEST_PKRS                   (1 << 22) /* Supervisor-Mode Protection Keys */

   bool LOAD_DBG_CTRLS() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_DBG_CTRLS; }
   bool X86_64_GUEST() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_X86_64_GUEST; }
   bool SMM_ENTER() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_SMM_ENTER; }
   bool DEACTIVATE_DUAL_MONITOR_TREATMENT() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_DEACTIVATE_DUAL_MONITOR_TREATMENT; }
   bool LOAD_PERF_GLOBAL_CTRL_MSR() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_PERF_GLOBAL_CTRL_MSR; }
   bool LOAD_PAT_MSR() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_PAT_MSR; }
   bool LOAD_EFER_MSR() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_EFER_MSR; }
   bool LOAD_BNDCFGS() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_BNDCFGS; }
   bool PTRACE_SUPPRESS_VMX_PACKETS() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_SUPPRESS_VMX_PACKETS; }
   bool LOAD_GUEST_RTIT_CTRL() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_GUEST_RTIT_CTRL; }
   bool LOAD_UINV() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_UINV; }
   bool LOAD_GUEST_CET_STATE() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_GUEST_CET_STATE; }
   bool LOAD_GUEST_LBR() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_GUEST_LBR_CTRL; }
   bool LOAD_GUEST_PKRS() const { return vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_GUEST_PKRS; }

   bool query_any(Bit32u mask) const { return (vmentry_ctrls & mask) != 0; }
   bool query_all(Bit32u mask) const { return (vmentry_ctrls & mask) == mask; }

   Bit32u get() const { return vmentry_ctrls; }

   void   set(Bit32u mask) { vmentry_ctrls |= mask; }
   void   clear(Bit32u mask) { vmentry_ctrls &= ~mask; }

   Bit32u* getref() { return &vmentry_ctrls; }
};

class BxVmexit1Controls {
private:
   Bit32u vmexit1_ctrls;

public:
   BxVmexit1Controls(Bit32u ctrls = 0): vmexit1_ctrls(ctrls) {}

#define VMX_VMEXIT_CTRL1_SAVE_DBG_CTRLS             (1 <<  2) /* legacy must be '1 */
#define VMX_VMEXIT_CTRL1_HOST_ADDR_SPACE_SIZE       (1 <<  9)
#define VMX_VMEXIT_CTRL1_LOAD_PERF_GLOBAL_CTRL_MSR  (1 << 12) /* Perf Global Control */
#define VMX_VMEXIT_CTRL1_INTA_ON_VMEXIT             (1 << 15)
#define VMX_VMEXIT_CTRL1_STORE_PAT_MSR              (1 << 18) /* PAT */
#define VMX_VMEXIT_CTRL1_LOAD_PAT_MSR               (1 << 19) /* PAT */
#define VMX_VMEXIT_CTRL1_STORE_EFER_MSR             (1 << 20) /* EFER */
#define VMX_VMEXIT_CTRL1_LOAD_EFER_MSR              (1 << 21) /* EFER */
#define VMX_VMEXIT_CTRL1_STORE_VMX_PREEMPTION_TIMER (1 << 22) /* VMX preemption timer */
#define VMX_VMEXIT_CTRL1_CLEAR_BNDCFGS              (1 << 23) /* MPX (not implemented) */
#define VMX_VMEXIT_CTRL1_SUPPRESS_HOST_VMX_TRACE    (1 << 24) /* Processor Trace (not implemented) */
#define VMX_VMEXIT_CTRL1_CLEAR_IA32_RTIT_CTRL       (1 << 25) /* Clear IA32_RTIT_CTRL MSR on vmexit (not implemented) */
#define VMX_VMEXIT_CTRL1_CLEAR_IA32_LBR_CTRL        (1 << 26) /* Clear IA32_LBR_CTRL MSR on vmexit (not implemented) */
#define VMX_VMEXIT_CTRL1_CLEAR_UINV                 (1 << 27) /* UINTR */
#define VMX_VMEXIT_CTRL1_LOAD_HOST_CET_STATE        (1 << 28) /* CET */
#define VMX_VMEXIT_CTRL1_LOAD_HOST_PKRS             (1 << 29) /* Supervisor-Mode Protection Keys */
#define VMX_VMEXIT_CTRL1_SAVE_PERF_GLOBAL_CTRL      (1 << 30) /* Save IA32_PERF_GLOBAL_CTRL on vmexit (not implemented) */
#define VMX_VMEXIT_CTRL1_ACTIVATE_SECONDARY_CTRLS   (1 << 31)

   bool SAVE_DBG_CTRLS() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_SAVE_DBG_CTRLS; }
   bool X86_64_HOST() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_HOST_ADDR_SPACE_SIZE; }
   bool LOAD_PERF_GLOBAL_CTRL_MSR() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_LOAD_PERF_GLOBAL_CTRL_MSR; }
   bool INTA_ON_VMEXIT() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_INTA_ON_VMEXIT; }
   bool STORE_PAT_MSR() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_STORE_PAT_MSR; }
   bool LOAD_PAT_MSR() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_LOAD_PAT_MSR; }
   bool STORE_EFER_MSR() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_STORE_EFER_MSR; }
   bool LOAD_EFER_MSR() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_LOAD_EFER_MSR; }
   bool STORE_VMX_PREEMPTION_TIMER() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_STORE_VMX_PREEMPTION_TIMER; }
   bool CLEAR_BNDCFGS() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_CLEAR_BNDCFGS; }
   bool PTRACE_SUPPRESS_HOST_VMX_TRACE() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_SUPPRESS_HOST_VMX_TRACE; }
   bool CLEAR_IA32_RTIT_CTRL() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_CLEAR_IA32_RTIT_CTRL; }
   bool CLEAR_IA32_LBR_CTRL() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_CLEAR_IA32_LBR_CTRL; }
   bool CLEAR_UINV() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_CLEAR_UINV; }
   bool LOAD_HOST_CET_STATE() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_LOAD_HOST_CET_STATE; }
   bool LOAD_HOST_PKRS() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_LOAD_HOST_PKRS; }
   bool SAVE_PERF_GLOBAL_CTRL() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_SAVE_PERF_GLOBAL_CTRL; }
   bool ACTIVATE_SECONDARY_CTRLS() const { return vmexit1_ctrls & VMX_VMEXIT_CTRL1_ACTIVATE_SECONDARY_CTRLS; }

   bool query_any(Bit32u mask) const { return (vmexit1_ctrls & mask) != 0; }
   bool query_all(Bit32u mask) const { return (vmexit1_ctrls & mask) == mask; }

   Bit32u get() const { return vmexit1_ctrls; }

   Bit32u* getref() { return &vmexit1_ctrls; }
};

class BxVmexit2Controls {
private:
   Bit64u vmexit2_ctrls;

public:
   BxVmexit2Controls(Bit64u ctrls = 0): vmexit2_ctrls(ctrls) {}

#define VMX_VMEXIT_CTRL2_SHADOW_STACK_BUSY_CTRL     (1 <<  2) /* Shadow stack prematurely busy */

   bool SHADOW_STACK_PREMATURELY_BUSY_CTRL() const { return vmexit2_ctrls & VMX_VMEXIT_CTRL2_SHADOW_STACK_BUSY_CTRL; }

   bool query_any(Bit64u mask) const { return (vmexit2_ctrls & mask) != 0; }
   bool query_all(Bit64u mask) const { return (vmexit2_ctrls & mask) == mask; }

   Bit32u get32() const { return (Bit32u) vmexit2_ctrls; }
   Bit64u get() const { return vmexit2_ctrls; }

   Bit64u* getref() { return &vmexit2_ctrls; }
};

#endif
