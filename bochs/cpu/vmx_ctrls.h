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

class VmxVmexec1Controls {
private:
   Bit32u vmexec_ctrls;

public:
   VmxVmexec1Controls(Bit32u ctrls = 0): vmexec_ctrls(ctrls) {}

#define VMX_VM_EXEC_CTRL1_INTERRUPT_WINDOW_VMEXIT   (1 << 2)
#define VMX_VM_EXEC_CTRL1_TSC_OFFSET                (1 << 3)
#define VMX_VM_EXEC_CTRL1_HLT_VMEXIT                (1 << 7)
#define VMX_VM_EXEC_CTRL1_INVLPG_VMEXIT             (1 << 9)
#define VMX_VM_EXEC_CTRL1_MWAIT_VMEXIT              (1 << 10)
#define VMX_VM_EXEC_CTRL1_RDPMC_VMEXIT              (1 << 11)
#define VMX_VM_EXEC_CTRL1_RDTSC_VMEXIT              (1 << 12)
#define VMX_VM_EXEC_CTRL1_CR3_WRITE_VMEXIT          (1 << 15) /* legacy must be '1 */
#define VMX_VM_EXEC_CTRL1_CR3_READ_VMEXIT           (1 << 16) /* legacy must be '1 */
#define VMX_VM_EXEC_CTRL1_TERTIARY_CONTROLS         (1 << 17)
#define VMX_VM_EXEC_CTRL1_CR8_WRITE_VMEXIT          (1 << 19) /* TPR shadow */
#define VMX_VM_EXEC_CTRL1_CR8_READ_VMEXIT           (1 << 20) /* TPR shadow */
#define VMX_VM_EXEC_CTRL1_TPR_SHADOW                (1 << 21) /* TPR shadow */
#define VMX_VM_EXEC_CTRL1_NMI_WINDOW_EXITING        (1 << 22) /* Virtual NMI */
#define VMX_VM_EXEC_CTRL1_DRx_ACCESS_VMEXIT         (1 << 23)
#define VMX_VM_EXEC_CTRL1_IO_VMEXIT                 (1 << 24)
#define VMX_VM_EXEC_CTRL1_IO_BITMAPS                (1 << 25)
#define VMX_VM_EXEC_CTRL1_MONITOR_TRAP_FLAG         (1 << 27) /* Monitor Trap Flag */
#define VMX_VM_EXEC_CTRL1_MSR_BITMAPS               (1 << 28)
#define VMX_VM_EXEC_CTRL1_MONITOR_VMEXIT            (1 << 29)
#define VMX_VM_EXEC_CTRL1_PAUSE_VMEXIT              (1 << 30)
#define VMX_VM_EXEC_CTRL1_SECONDARY_CONTROLS        (1 << 31)

   bool INTERRUPT_WINDOW_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_INTERRUPT_WINDOW_VMEXIT; }
   bool TSC_OFFSET() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_TSC_OFFSET; }
   bool HLT_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_HLT_VMEXIT; }
   bool INVLPG_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_INVLPG_VMEXIT; }
   bool MWAIT_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_MWAIT_VMEXIT; }
   bool RDPMC_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_RDPMC_VMEXIT; }
   bool RDTSC_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_RDTSC_VMEXIT; }
   bool CR3_WRITE_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_CR3_WRITE_VMEXIT; }
   bool CR3_READ_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_CR3_READ_VMEXIT; }
   bool ACTIVATE_TERTIARY_CONTROLS() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_TERTIARY_CONTROLS; }
   bool CR8_WRITE_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_CR8_WRITE_VMEXIT; }
   bool CR8_READ_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_CR8_READ_VMEXIT; }
   bool TPR_SHADOW() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_TPR_SHADOW; }
   bool NMI_WINDOW_EXITING() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_NMI_WINDOW_EXITING; }
   bool DRx_ACCESS_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_DRx_ACCESS_VMEXIT; }
   bool IO_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_IO_VMEXIT; }
   bool IO_BITMAPS() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_IO_BITMAPS; }
   bool MONITOR_TRAP_FLAG() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_MONITOR_TRAP_FLAG; }
   bool MSR_BITMAPS() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_MSR_BITMAPS; }
   bool MONITOR_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_MONITOR_VMEXIT; }
   bool PAUSE_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_PAUSE_VMEXIT; }
   bool ACTIVATE_SECONDARY_CONTROLS() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL1_SECONDARY_CONTROLS; }

   bool query_any(Bit32u mask) const { return (vmexec_ctrls & mask) != 0; }
   bool query_all(Bit32u mask) const { return (vmexec_ctrls & mask) == mask; }

   Bit32u get() const { return vmexec_ctrls; }

   Bit32u* getref() { return &vmexec_ctrls; }
};

class VmxVmexec2Controls {
private:
   Bit32u vmexec_ctrls;

public:
   VmxVmexec2Controls(Bit32u ctrls = 0): vmexec_ctrls(ctrls) {}

#define VMX_VM_EXEC_CTRL2_VIRTUALIZE_APIC_ACCESSES  (1 <<  0) /* APIC virtualization */
#define VMX_VM_EXEC_CTRL2_EPT_ENABLE                (1 <<  1) /* EPT */
#define VMX_VM_EXEC_CTRL2_DESCRIPTOR_TABLE_VMEXIT   (1 <<  2) /* Descriptor Table VMEXIT */
#define VMX_VM_EXEC_CTRL2_RDTSCP                    (1 <<  3)
#define VMX_VM_EXEC_CTRL2_VIRTUALIZE_X2APIC_MODE    (1 <<  4) /* Virtualize X2APIC */
#define VMX_VM_EXEC_CTRL2_VPID_ENABLE               (1 <<  5) /* VPID */
#define VMX_VM_EXEC_CTRL2_WBINVD_VMEXIT             (1 <<  6) /* WBINVD VMEXIT */
#define VMX_VM_EXEC_CTRL2_UNRESTRICTED_GUEST        (1 <<  7) /* Unrestricted Guest */
#define VMX_VM_EXEC_CTRL2_VIRTUALIZE_APIC_REGISTERS (1 <<  8)
#define VMX_VM_EXEC_CTRL2_VIRTUAL_INT_DELIVERY      (1 <<  9)
#define VMX_VM_EXEC_CTRL2_PAUSE_LOOP_VMEXIT         (1 << 10) /* PAUSE loop exiting */
#define VMX_VM_EXEC_CTRL2_RDRAND_VMEXIT             (1 << 11)
#define VMX_VM_EXEC_CTRL2_INVPCID                   (1 << 12)
#define VMX_VM_EXEC_CTRL2_VMFUNC_ENABLE             (1 << 13) /* VM Functions */
#define VMX_VM_EXEC_CTRL2_VMCS_SHADOWING            (1 << 14) /* VMCS Shadowing */
#define VMX_VM_EXEC_CTRL2_SGX_ENCLS_VMEXIT          (1 << 15) /* ENCLS/SGX (not implemented) */
#define VMX_VM_EXEC_CTRL2_RDSEED_VMEXIT             (1 << 16)
#define VMX_VM_EXEC_CTRL2_PML_ENABLE                (1 << 17) /* Page Modification Logging */
#define VMX_VM_EXEC_CTRL2_EPT_VIOLATION_EXCEPTION   (1 << 18) /* #VE Exception */
#define VMX_VM_EXEC_CTRL2_SUPPRESS_GUEST_VMX_TRACE  (1 << 19) /* Processor Trace (not implemented) */
#define VMX_VM_EXEC_CTRL2_XSAVES_XRSTORS            (1 << 20) /* XSAVES */
#define VMX_VM_EXEC_CTRL2_MBE_CTRL                  (1 << 22) /* Mode Based Execution Control */
#define VMX_VM_EXEC_CTRL2_SUBPAGE_WR_PROTECT_CTRL   (1 << 23) /* Sub-Page Write Protection Control */
#define VMX_VM_EXEC_CTRL2_PROCESSOR_TRACE_USE_GPA   (1 << 24) /* Processor Trace (not implemented) */
#define VMX_VM_EXEC_CTRL2_TSC_SCALING               (1 << 25) /* TSC Scaling */
#define VMX_VM_EXEC_CTRL2_UMWAIT_TPAUSE_VMEXIT      (1 << 26) /* WAITPKG */
#define VMX_VM_EXEC_CTRL2_PCONFIG_ENABLE            (1 << 27) // not implemented yet
#define VMX_VM_EXEC_CTRL2_SGX_ENCLV_VMEXIT          (1 << 28) /* ENCLV/SGX (not implemented) */

   bool VIRTUALIZE_APIC_ACCESSES() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_VIRTUALIZE_APIC_ACCESSES; }
   bool EPT_ENABLE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_EPT_ENABLE; }
   bool DESCRIPTOR_TABLE_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_DESCRIPTOR_TABLE_VMEXIT; }
   bool RDTSCP() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_RDTSCP; }
   bool VIRTUALIZE_X2APIC_MODE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_VIRTUALIZE_X2APIC_MODE; }
   bool VPID_ENABLE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_VPID_ENABLE; }
   bool WBINVD_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_WBINVD_VMEXIT; }
   bool UNRESTRICTED_GUEST() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_UNRESTRICTED_GUEST; }
   bool VIRTUALIZE_APIC_REGISTERS() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_VIRTUALIZE_APIC_REGISTERS; }
   bool VIRTUAL_INT_DELIVERY() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_VIRTUAL_INT_DELIVERY; }
   bool PAUSE_LOOP_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_PAUSE_LOOP_VMEXIT; }
   bool RDRAND_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_RDRAND_VMEXIT; }
   bool INVPCID() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_INVPCID; }
   bool VMFUNC_ENABLE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_VMFUNC_ENABLE; }
   bool VMCS_SHADOWING() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_VMCS_SHADOWING; }
   bool SGX_ENCLS_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_SGX_ENCLS_VMEXIT; }
   bool RDSEED_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_RDSEED_VMEXIT; }
   bool PML_ENABLE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_PML_ENABLE; }
   bool EPT_VIOLATION_EXCEPTION() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_EPT_VIOLATION_EXCEPTION; }
   bool PTRACE_SUPPRESS_GUEST_VMX_TRACE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_SUPPRESS_GUEST_VMX_TRACE; }
   bool XSAVES_XRSTORS() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_XSAVES_XRSTORS; }
   bool MBE_CTRL() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_MBE_CTRL; }
   bool SUBPAGE_WR_PROTECT_CTRL() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_SUBPAGE_WR_PROTECT_CTRL; }
   bool PROCESSOR_TRACE_USE_GPA() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_PROCESSOR_TRACE_USE_GPA; }
   bool TSC_SCALING() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_TSC_SCALING; }
   bool UMWAIT_TPAUSE_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_UMWAIT_TPAUSE_VMEXIT; }
   bool PCONFIG_ENABLE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_PCONFIG_ENABLE; }
   bool SGX_ENCLV_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL2_SGX_ENCLV_VMEXIT; }

   bool query_any(Bit32u mask) const { return (vmexec_ctrls & mask) != 0; }
   bool query_all(Bit32u mask) const { return (vmexec_ctrls & mask) == mask; }

   Bit32u get() const { return vmexec_ctrls; }

   Bit32u* getref() { return &vmexec_ctrls; }
};

class VmxVmexec3Controls {
private:
   Bit64u vmexec_ctrls;

public:
   VmxVmexec3Controls(Bit64u ctrls = 0): vmexec_ctrls(ctrls) {}

#define VMX_VM_EXEC_CTRL3_LOADIWKEY_VMEXIT          (1 <<  0) /* KeyLocker (not implemented) */
#define VMX_VM_EXEC_CTRL3_HLAT_ENABLE               (1 <<  1) /* HLAT (not implemented) */
#define VMX_VM_EXEC_CTRL3_EPT_PAGING_WRITE          (1 <<  2) /* HLAT (not implemented) */
#define VMX_VM_EXEC_CTRL3_GUEST_PAGING_VERIFICATION (1 <<  3) /* HLAT (not implemented) */
#define VMX_VM_EXEC_CTRL3_IPI_VIRTUALIZATION        (1 <<  4) /* IPI virtualization (not implemented) */
#define VMX_VM_EXEC_CTRL3_ENABLE_MSRLIST            (1 <<  6) /* MSRLIST */
#define VMX_VM_EXEC_CTRL3_VIRTUALIZE_IA32_SPEC_CTRL (1 <<  7)

   bool LOADIWKEY_VMEXIT() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL3_LOADIWKEY_VMEXIT; }
   bool HLAT_ENABLE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL3_HLAT_ENABLE; }
   bool EPT_PAGING_WRITE() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL3_EPT_PAGING_WRITE; }
   bool GUEST_PAGING_VERIFICATION() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL3_GUEST_PAGING_VERIFICATION; }
   bool IPI_VIRTUALIZATION() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL3_IPI_VIRTUALIZATION; }
   bool ENABLE_MSRLIST() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL3_ENABLE_MSRLIST; }
   bool VIRTUALIZE_IA32_SPEC_CTRL() const { return vmexec_ctrls & VMX_VM_EXEC_CTRL3_VIRTUALIZE_IA32_SPEC_CTRL; }

   bool query_any(Bit64u mask) const { return (vmexec_ctrls & mask) != 0; }
   bool query_all(Bit64u mask) const { return (vmexec_ctrls & mask) == mask; }

   Bit32u get32() const { return (Bit32u) vmexec_ctrls; }
   Bit64u get() const { return vmexec_ctrls; }

   Bit64u* getref() { return &vmexec_ctrls; }
};

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
