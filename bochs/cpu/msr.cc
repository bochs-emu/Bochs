/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2019 Stanislav Shwartsman
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
#include "msr.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_CET
extern bool is_invalid_cet_control(bx_address val);
#endif

#if BX_CPU_LEVEL >= 5
bool BX_CPP_AttrRegparmN(2) BX_CPU_C::rdmsr(Bit32u index, Bit64u *msr)
{
  Bit64u val64 = 0;

#if BX_CPU_LEVEL >= 6
  if (is_cpu_extension_supported(BX_ISA_X2APIC)) {
    if (is_x2apic_msr_range(index)) {
      if (BX_CPU_THIS_PTR msr.apicbase & 0x400)  // X2APIC mode
        return BX_CPU_THIS_PTR lapic.read_x2apic(index, msr);
      else
        return 0;
    }
  }
#endif

  switch(index) {
#if BX_CPU_LEVEL >= 6
    case BX_MSR_SYSENTER_CS:
      if (! is_cpu_extension_supported(BX_ISA_SYSENTER_SYSEXIT)) {
        BX_ERROR(("RDMSR MSR_SYSENTER_CS: SYSENTER/SYSEXIT feature not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.sysenter_cs_msr;
      break;

    case BX_MSR_SYSENTER_ESP:
      if (! is_cpu_extension_supported(BX_ISA_SYSENTER_SYSEXIT)) {
        BX_ERROR(("RDMSR MSR_SYSENTER_ESP: SYSENTER/SYSEXIT feature not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.sysenter_esp_msr;
      break;

    case BX_MSR_SYSENTER_EIP:
      if (! is_cpu_extension_supported(BX_ISA_SYSENTER_SYSEXIT)) {
        BX_ERROR(("RDMSR MSR_SYSENTER_EIP: SYSENTER/SYSEXIT feature not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.sysenter_eip_msr;
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_MTRRCAP:   // read only MSR
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("RDMSR MSR_MTRRCAP: MTRR is not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CONST64(0x0000000000000500) | BX_NUM_VARIABLE_RANGE_MTRRS;
      break;

    case BX_MSR_MTRRPHYSBASE0:
    case BX_MSR_MTRRPHYSMASK0:
    case BX_MSR_MTRRPHYSBASE1:
    case BX_MSR_MTRRPHYSMASK1:
    case BX_MSR_MTRRPHYSBASE2:
    case BX_MSR_MTRRPHYSMASK2:
    case BX_MSR_MTRRPHYSBASE3:
    case BX_MSR_MTRRPHYSMASK3:
    case BX_MSR_MTRRPHYSBASE4:
    case BX_MSR_MTRRPHYSMASK4:
    case BX_MSR_MTRRPHYSBASE5:
    case BX_MSR_MTRRPHYSMASK5:
    case BX_MSR_MTRRPHYSBASE6:
    case BX_MSR_MTRRPHYSMASK6:
    case BX_MSR_MTRRPHYSBASE7:
    case BX_MSR_MTRRPHYSMASK7:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("RDMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.mtrrphys[index - BX_MSR_MTRRPHYSBASE0];
      break;

    case BX_MSR_MTRRFIX64K_00000:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("RDMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.mtrrfix64k.u64;
      break;
    case BX_MSR_MTRRFIX16K_80000:
    case BX_MSR_MTRRFIX16K_A0000:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("RDMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.mtrrfix16k[index - BX_MSR_MTRRFIX16K_80000].u64;
      break;

    case BX_MSR_MTRRFIX4K_C0000:
    case BX_MSR_MTRRFIX4K_C8000:
    case BX_MSR_MTRRFIX4K_D0000:
    case BX_MSR_MTRRFIX4K_D8000:
    case BX_MSR_MTRRFIX4K_E0000:
    case BX_MSR_MTRRFIX4K_E8000:
    case BX_MSR_MTRRFIX4K_F0000:
    case BX_MSR_MTRRFIX4K_F8000:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("RDMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.mtrrfix4k[index - BX_MSR_MTRRFIX4K_C0000].u64;
      break;

    case BX_MSR_MTRR_DEFTYPE:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("RDMSR MSR_MTRR_DEFTYPE: MTRR is not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.mtrr_deftype;
      break;

    case BX_MSR_PAT:
      if (! is_cpu_extension_supported(BX_ISA_PAT)) {
        BX_ERROR(("RDMSR BX_MSR_PAT: PAT is not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.pat.u64;
      break;
#endif

    case BX_MSR_TSC:
      val64 = BX_CPU_THIS_PTR get_TSC();
#if BX_SUPPORT_SVM || BX_SUPPORT_VMX
      val64 = BX_CPU_THIS_PTR get_TSC_VMXAdjust(val64);
#endif
      break;

    case BX_MSR_TSC_ADJUST:
      if (! is_cpu_extension_supported(BX_ISA_TSC_ADJUST)) {
        BX_ERROR(("RDMSR BX_MSR_TSC_ADJUST: TSC_ADJUST is not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR tsc_adjust;
      break;

#if BX_SUPPORT_APIC
    case BX_MSR_APICBASE:
      val64 = BX_CPU_THIS_PTR msr.apicbase;
      BX_INFO(("RDMSR: Read %08x:%08x from MSR_APICBASE", GET32H(val64), GET32L(val64)));
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_XSS:
      val64 = BX_CPU_THIS_PTR msr.ia32_xss;
      break;
#endif

#if BX_SUPPORT_CET
    case BX_MSR_IA32_U_CET:
    case BX_MSR_IA32_S_CET:
      if (! is_cpu_extension_supported(BX_ISA_CET)) {
        BX_ERROR(("RDMSR BX_MSR_IA32_U_CET/BX_MSR_IA32_S_CET: CET not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.ia32_cet_control[index == BX_MSR_IA32_U_CET];
      break;

    case BX_MSR_IA32_PL0_SSP:
    case BX_MSR_IA32_PL1_SSP:
    case BX_MSR_IA32_PL2_SSP:
    case BX_MSR_IA32_PL3_SSP:
      if (! is_cpu_extension_supported(BX_ISA_CET)) {
        BX_ERROR(("RDMSR BX_MSR_IA32_PLi_SSP: CET not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.ia32_pl_ssp[index - BX_MSR_IA32_PL0_SSP];
      break;

    case BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR:
      if (! is_cpu_extension_supported(BX_ISA_CET)) {
        BX_ERROR(("RDMSR BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR: CET not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table;
      break;
#endif

#if BX_SUPPORT_PKEYS
    case BX_MSR_IA32_PKRS:
      if (! is_cpu_extension_supported(BX_ISA_PKS)) {
        BX_ERROR(("RDMSR BX_MSR_IS32_PKS: Supervisor-Mode Protection Keys not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR pkrs;
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_TSC_DEADLINE:
      if (! is_cpu_extension_supported(BX_ISA_TSC_DEADLINE)) {
        BX_ERROR(("RDMSR BX_MSR_TSC_DEADLINE: TSC-Deadline not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR lapic.get_tsc_deadline();
      break;
#endif

    // SCA preention MSRs
    case BX_MSR_IA32_ARCH_CAPABILITIES:
      if (! is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS)) {
        BX_ERROR(("RDMSR IA32_ARCH_CAPABILITIES: not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      //     [0]: RDCL_NO: The processor is not susceptible to Rogue Data Cache Load (RDCL)
      //     [1]: IBRS_ALL: The processor supports enhanced IBRS
      //     [2]: RSBA: The processor supports RSB Alternate
      //     [3]: SKIP_L1DFL_VMENTRY: indicates the hypervisor need not flush the L1D on VM entry
      //     [4]: SSB_NO: Processor is not susceptible to Speculative Store Bypass
      //  [63:5]: reserved
      val64 = 0x1F; // set bits [4:0]
      break;

    case BX_MSR_IA32_SPEC_CTRL:
      if (! is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS)) {
        BX_ERROR(("WRMSR IA32_SPEC_CTRL: not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      //    [0] - Enable IBRS: Indirect Branch Restricted Speculation
      //    [1] - Enable STIBP: Single Thread Indirect Branch Predictors
      //    [2] - Enable SSCB: Speculative Store Bypass Disable
      // [63:3] - reserved
      val64 = BX_CPU_THIS_PTR msr.ia32_spec_ctrl;
      break;

    case BX_MSR_IA32_PRED_CMD:
      if (! is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS)) {
        BX_ERROR(("WRMSR IA32_PRED_CMD: not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      // write only MSR, no need to remember written value
      return 0;

    case BX_MSR_IA32_FLUSH_CMD:
      if (! is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS)) {
        BX_ERROR(("WRMSR IA32_FLUSH_CMD: not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      // write only MSR, no need to remember written value
      return 0;

#if BX_SUPPORT_VMX
/*
    case BX_MSR_IA32_SMM_MONITOR_CTL:
      BX_PANIC(("Dual-monitor treatment of SMI and SMM is not implemented"));
      break;
*/
    case BX_MSR_IA32_FEATURE_CONTROL:
      val64 = BX_CPU_THIS_PTR msr.ia32_feature_ctrl;
      break;
    case BX_MSR_VMX_BASIC:
      val64 = VMX_MSR_VMX_BASIC;
      break;
    case BX_MSR_VMX_PINBASED_CTRLS:
      val64 = VMX_MSR_VMX_PINBASED_CTRLS;
      break;
    case BX_MSR_VMX_PROCBASED_CTRLS:
      val64 = VMX_MSR_VMX_PROCBASED_CTRLS;
      break;
    case BX_MSR_VMX_VMEXIT_CTRLS:
      val64 = VMX_MSR_VMX_VMEXIT_CTRLS;
      break;
    case BX_MSR_VMX_VMENTRY_CTRLS:
      val64 = VMX_MSR_VMX_VMENTRY_CTRLS;
      break;
    case BX_MSR_VMX_PROCBASED_CTRLS2:
      if (BX_CPU_THIS_PTR vmx_cap.vmx_vmexec_ctrl2_supported_bits) {
        val64 = VMX_MSR_VMX_PROCBASED_CTRLS2;
        break;
      }
      return 0;
#if BX_SUPPORT_VMX >= 2
    case BX_MSR_VMX_TRUE_PINBASED_CTRLS:
      val64 = VMX_MSR_VMX_TRUE_PINBASED_CTRLS;
      break;
    case BX_MSR_VMX_TRUE_PROCBASED_CTRLS:
      val64 = VMX_MSR_VMX_TRUE_PROCBASED_CTRLS;
      break;
    case BX_MSR_VMX_TRUE_VMEXIT_CTRLS:
      val64 = VMX_MSR_VMX_TRUE_VMEXIT_CTRLS;
      break;
    case BX_MSR_VMX_TRUE_VMENTRY_CTRLS:
      val64 = VMX_MSR_VMX_TRUE_VMENTRY_CTRLS;
      break;
    case BX_MSR_VMX_EPT_VPID_CAP:
      if (VMX_MSR_VMX_EPT_VPID_CAP != 0) {
        val64 = VMX_MSR_VMX_EPT_VPID_CAP;
        break;
      }
      return 0;
    case BX_MSR_VMX_VMFUNC:
      if (BX_CPU_THIS_PTR vmx_cap.vmx_vmfunc_supported_bits) {
        val64 = BX_CPU_THIS_PTR vmx_cap.vmx_vmfunc_supported_bits;
        break;
      }
      return 0;
#endif
    case BX_MSR_VMX_MISC:
      val64 = VMX_MSR_MISC;
      break;
    case BX_MSR_VMX_CR0_FIXED0:
      val64 = VMX_MSR_CR0_FIXED0;
      break;
    case BX_MSR_VMX_CR0_FIXED1:
      val64 = VMX_MSR_CR0_FIXED1;
      break;
    case BX_MSR_VMX_CR4_FIXED0:
      val64 = VMX_MSR_CR4_FIXED0;
      break;
    case BX_MSR_VMX_CR4_FIXED1:
      val64 = VMX_MSR_CR4_FIXED1;
      break;
    case BX_MSR_VMX_VMCS_ENUM:
      val64 = VMX_MSR_VMCS_ENUM;
      break;
#endif

    case BX_MSR_EFER:
      if (! BX_CPU_THIS_PTR efer_suppmask) {
        BX_ERROR(("RDMSR MSR_EFER: EFER MSR is not supported !"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR efer.get32();
      break;

#if BX_SUPPORT_SVM
    case BX_SVM_HSAVE_PA_MSR:
      if (! is_cpu_extension_supported(BX_ISA_SVM)) {
        BX_ERROR(("RDMSR SVM_HSAVE_PA_MSR: SVM support not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.svm_hsave_pa;
      break;
#endif

    case BX_MSR_STAR:
      if ((BX_CPU_THIS_PTR efer_suppmask & BX_EFER_SCE_MASK) == 0) {
        BX_ERROR(("RDMSR MSR_STAR: SYSCALL/SYSRET support not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.star;
      break;

#if BX_SUPPORT_X86_64
    case BX_MSR_LSTAR:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("RDMSR MSR_LSTAR: long mode support not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.lstar;
      break;

    case BX_MSR_CSTAR:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("RDMSR MSR_CSTAR: long mode support not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.cstar;
      break;

    case BX_MSR_FMASK:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("RDMSR MSR_FMASK: long mode support not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.fmask;
      break;

    case BX_MSR_FSBASE:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("RDMSR MSR_FSBASE: long mode support not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = MSR_FSBASE;
      break;

    case BX_MSR_GSBASE:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("RDMSR MSR_GSBASE: long mode support not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = MSR_GSBASE;
      break;

    case BX_MSR_KERNELGSBASE:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("RDMSR MSR_KERNELGSBASE: long mode support not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.kernelgsbase;
      break;

    case BX_MSR_TSC_AUX:
      if (! is_cpu_extension_supported(BX_ISA_RDTSCP)) {
        BX_ERROR(("RDMSR MSR_TSC_AUX: RTDSCP feature not enabled in the cpu model"));
        return handle_unknown_rdmsr(index, msr);
      }
      val64 = BX_CPU_THIS_PTR msr.tsc_aux;   // 32 bit MSR
      break;
#endif

    default:
      return handle_unknown_rdmsr(index, msr);
  }

  BX_DEBUG(("RDMSR: read %08x:%08x from MSR %x", GET32H(val64), GET32L(val64), index));

  *msr = val64;
  return 1;
}

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::handle_unknown_rdmsr(Bit32u index, Bit64u *msr)
{
  Bit64u val_64 = 0;

  // Try to check cpuid_t first (can implement some MSRs)
  int result = BX_CPU_THIS_PTR cpuid->rdmsr(index, &val_64);
  if (result == 0)
    return 0; // #GP fault due to not supported MSR

  if (result < 0) {
    // cpuid_t have no idea about this MSR
#if BX_CONFIGURE_MSRS
    if (index < BX_MSR_MAX_INDEX && BX_CPU_THIS_PTR msrs[index]) {
      val_64 = BX_CPU_THIS_PTR msrs[index]->get64();
    }
    else
#endif
    {
      // failed to find the MSR, could #GP or ignore it silently
      BX_ERROR(("RDMSR: Unknown register %#x", index));

      if (! BX_CPU_THIS_PTR ignore_bad_msrs)
        return 0; // will result in #GP fault due to unknown MSR
    }
  }

  *msr = val_64;
  return 1;
}

#endif // BX_CPU_LEVEL >= 5

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDMSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  // CPL is always 0 in real mode
  if (/* !real_mode() && */ CPL!=0) {
    BX_ERROR(("RDMSR: CPL != 0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u index = ECX;
  Bit64u val64 = 0;

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_MSR)) SvmInterceptMSR(BX_READ, index);
  }
#endif

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest)
    VMexit_MSR(VMX_VMEXIT_RDMSR, index);
#endif

#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_X2APIC_MODE)) {
      if (index >= 0x800 && index <= 0x8FF) {
        if (index == 0x808 || SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_REGISTERS)) {
          unsigned vapic_offset = (index & 0xff) << 4;
          RAX = VMX_Read_Virtual_APIC(vapic_offset);
          RDX = VMX_Read_Virtual_APIC(vapic_offset + 4);
          BX_NEXT_INSTR(i);
        }
      }
    }
  }
#endif

  if (!rdmsr(index, &val64))
    exception(BX_GP_EXCEPTION, 0);

  RAX = GET32L(val64);
  RDX = GET32H(val64);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_CPU_LEVEL >= 6
bool isMemTypeValidMTRR(unsigned memtype)
{
  switch(memtype) {
  case BX_MEMTYPE_UC:
  case BX_MEMTYPE_WC:
  case BX_MEMTYPE_WT:
  case BX_MEMTYPE_WP:
  case BX_MEMTYPE_WB:
    return true;
  default:
    return false;
  }
}

BX_CPP_INLINE bool isMemTypeValidPAT(unsigned memtype)
{
  return (memtype == 0x07) /* UC- */ || isMemTypeValidMTRR(memtype);
}

bool isValidMSR_PAT(Bit64u pat_val)
{
  // use packed register as 64-bit value with convinient accessors
  BxPackedRegister pat_msr = pat_val;
  for (unsigned i=0; i<8; i++)
    if (! isMemTypeValidPAT(pat_msr.ubyte(i))) return false;

  return true;
}

bool isValidMSR_FixedMTRR(Bit64u fixed_mtrr_val)
{
  // use packed register as 64-bit value with convinient accessors
  BxPackedRegister fixed_mtrr_msr = fixed_mtrr_val;
  for (unsigned i=0; i<8; i++)
    if (! isMemTypeValidMTRR(fixed_mtrr_msr.ubyte(i))) return false;

  return true;
}
#endif

#if BX_CPU_LEVEL >= 5
bool BX_CPP_AttrRegparmN(2) BX_CPU_C::wrmsr(Bit32u index, Bit64u val_64)
{
  Bit32u val32_lo = GET32L(val_64);
  Bit32u val32_hi = GET32H(val_64);

  BX_INSTR_WRMSR(BX_CPU_ID, index, val_64);

  BX_DEBUG(("WRMSR: write %08x:%08x to MSR %x", val32_hi, val32_lo, index));

#if BX_CPU_LEVEL >= 6
  if (is_cpu_extension_supported(BX_ISA_X2APIC)) {
    if (is_x2apic_msr_range(index)) {
      if (BX_CPU_THIS_PTR msr.apicbase & 0x400)  // X2APIC mode
        return BX_CPU_THIS_PTR lapic.write_x2apic(index, val32_hi, val32_lo);
      else
        return 0;
    }
  }
#endif

  switch(index) {

#if BX_SUPPORT_PERFMON
    case BX_MSR_PERFEVTSEL0:
    case BX_MSR_PERFEVTSEL1:
    case BX_MSR_PERFEVTSEL2:
    case BX_MSR_PERFEVTSEL3:
    case BX_MSR_PERFEVTSEL4:
    case BX_MSR_PERFEVTSEL5:
    case BX_MSR_PERFEVTSEL6:
    case BX_MSR_PERFEVTSEL7:
      BX_INFO(("WRMSR: write into IA32_PERFEVTSEL%d MSR: %08x:%08x", index - BX_MSR_PERFEVTSEL0, val32_hi, val32_lo));
      return handle_unknown_wrmsr(index, val_64);
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_SYSENTER_CS:
      if (! is_cpu_extension_supported(BX_ISA_SYSENTER_SYSEXIT)) {
        BX_ERROR(("WRMSR MSR_SYSENTER_CS: SYSENTER/SYSEXIT feature not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_CPU_THIS_PTR msr.sysenter_cs_msr = val32_lo;
      break;

    case BX_MSR_SYSENTER_ESP:
      if (! is_cpu_extension_supported(BX_ISA_SYSENTER_SYSEXIT)) {
        BX_ERROR(("WRMSR MSR_SYSENTER_ESP: SYSENTER/SYSEXIT feature not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
#if BX_SUPPORT_X86_64
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_SYSENTER_ESP !"));
        return 0;
      }
#endif
      BX_CPU_THIS_PTR msr.sysenter_esp_msr = val_64;
      break;

    case BX_MSR_SYSENTER_EIP:
      if (! is_cpu_extension_supported(BX_ISA_SYSENTER_SYSEXIT)) {
        BX_ERROR(("WRMSR MSR_SYSENTER_EIP: SYSENTER/SYSEXIT feature not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
#if BX_SUPPORT_X86_64
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_SYSENTER_EIP !"));
        return 0;
      }
#endif
      BX_CPU_THIS_PTR msr.sysenter_eip_msr = val_64;
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_MTRRCAP:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("WRMSR MSR_MTRRCAP: MTRR is not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_ERROR(("WRMSR: MTRRCAP is read only MSR"));
      return 0;

    case BX_MSR_MTRRPHYSBASE0:
    case BX_MSR_MTRRPHYSBASE1:
    case BX_MSR_MTRRPHYSBASE2:
    case BX_MSR_MTRRPHYSBASE3:
    case BX_MSR_MTRRPHYSBASE4:
    case BX_MSR_MTRRPHYSBASE5:
    case BX_MSR_MTRRPHYSBASE6:
    case BX_MSR_MTRRPHYSBASE7:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("WRMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsValidPhyAddr(val_64)) {
        BX_ERROR(("WRMSR[0x%08x]: attempt to write invalid phy addr to variable range MTRR %08x:%08x", index, val32_hi, val32_lo));
        return 0;
      }
      // handle 8-11 reserved bits
      if (! isMemTypeValidMTRR(val32_lo & 0xFFF)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to BX_MSR_MTRRPHYSBASE"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.mtrrphys[index - BX_MSR_MTRRPHYSBASE0] = val_64;
      break;
    case BX_MSR_MTRRPHYSMASK0:
    case BX_MSR_MTRRPHYSMASK1:
    case BX_MSR_MTRRPHYSMASK2:
    case BX_MSR_MTRRPHYSMASK3:
    case BX_MSR_MTRRPHYSMASK4:
    case BX_MSR_MTRRPHYSMASK5:
    case BX_MSR_MTRRPHYSMASK6:
    case BX_MSR_MTRRPHYSMASK7:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("WRMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsValidPhyAddr(val_64)) {
        BX_ERROR(("WRMSR[0x%08x]: attempt to write invalid phy addr to variable range MTRR %08x:%08x", index, val32_hi, val32_lo));
        return 0;
      }
      // handle 10-0 reserved bits
      if (val32_lo & 0x7ff) {
        BX_ERROR(("WRMSR[0x%08x]: variable range MTRR reserved bits violation %08x:%08x", index, val32_hi, val32_lo));
        return 0;
      }
      BX_CPU_THIS_PTR msr.mtrrphys[index - BX_MSR_MTRRPHYSBASE0] = val_64;
      break;

    case BX_MSR_MTRRFIX64K_00000:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("WRMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! isValidMSR_FixedMTRR(val_64)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_MTRRFIX64K_00000 !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.mtrrfix64k = val_64;
      break;

    case BX_MSR_MTRRFIX16K_80000:
    case BX_MSR_MTRRFIX16K_A0000:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("WRMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! isValidMSR_FixedMTRR(val_64)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_MTRRFIX16K register !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.mtrrfix16k[index - BX_MSR_MTRRFIX16K_80000] = val_64;
      break;

    case BX_MSR_MTRRFIX4K_C0000:
    case BX_MSR_MTRRFIX4K_C8000:
    case BX_MSR_MTRRFIX4K_D0000:
    case BX_MSR_MTRRFIX4K_D8000:
    case BX_MSR_MTRRFIX4K_E0000:
    case BX_MSR_MTRRFIX4K_E8000:
    case BX_MSR_MTRRFIX4K_F0000:
    case BX_MSR_MTRRFIX4K_F8000:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("WRMSR: MTRR is not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! isValidMSR_FixedMTRR(val_64)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to fixed memory range MTRR !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.mtrrfix4k[index - BX_MSR_MTRRFIX4K_C0000] = val_64;
      break;

    case BX_MSR_MTRR_DEFTYPE:
      if (! is_cpu_extension_supported(BX_ISA_MTRR)) {
        BX_ERROR(("WRMSR MSR_MTRR_DEFTYPE: MTRR is not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! isMemTypeValidMTRR(val32_lo & 0xFF)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_MTRR_DEFTYPE"));
        return 0;
      }
      if (val32_hi || (val32_lo & 0xfffff300)) {
        BX_ERROR(("WRMSR: attempt to reserved bits in MSR_MTRR_DEFTYPE"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.mtrr_deftype = val32_lo;
      break;

    case BX_MSR_PAT:
      if (! is_cpu_extension_supported(BX_ISA_PAT)) {
        BX_ERROR(("WRMSR BX_MSR_PAT: PAT is not enabled !"));
        return handle_unknown_wrmsr(index, val_64);
      }

      if (! isValidMSR_PAT(val_64)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_PAT"));
        return 0;
      }
      
      BX_CPU_THIS_PTR msr.pat = val_64;
      break;
#endif

    case BX_MSR_TSC:
      BX_INFO(("WRMSR: write 0x%08x%08x to MSR_TSC", val32_hi, val32_lo));
      BX_CPU_THIS_PTR set_TSC(val_64);
      break;

    case BX_MSR_TSC_ADJUST:
      if (! is_cpu_extension_supported(BX_ISA_TSC_ADJUST)) {
        BX_ERROR(("WRMSR BX_MSR_TSC_ADJUST: TSC_ADJUST is not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_CPU_THIS_PTR tsc_adjust = (Bit64s) val_64;
      break;

#if BX_SUPPORT_APIC
    case BX_MSR_APICBASE:
      return relocate_apic(val_64);
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_XSS:
      {
        if (! is_cpu_extension_supported(BX_ISA_XSAVES)) {
          BX_ERROR(("WRMSR BX_MSR_XSS: XSAVES not enabled in the cpu model"));
          return handle_unknown_wrmsr(index, val_64);
        }
        Bit32u xss_suport_mask = get_ia32_xss_allow_mask();
        if (val_64 & ~Bit64u(xss_suport_mask)) {
          BX_ERROR(("WRMSR: attempt to set reserved/not supported bit in BX_MSR_XSS"));
          return 0;
        }
        BX_CPU_THIS_PTR msr.ia32_xss = val_64;
        break;
      }
#endif

#if BX_SUPPORT_CET
    case BX_MSR_IA32_U_CET:
    case BX_MSR_IA32_S_CET:
      if (! is_cpu_extension_supported(BX_ISA_CET)) {
        BX_ERROR(("WRMSR BX_MSR_IA32_U_CET/BX_MSR_IA32_S_CET: CET not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsCanonical(val_64) || is_invalid_cet_control(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical or invalid value to BX_MSR_IA32_U_CET/BX_MSR_IA32_S_CET !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.ia32_cet_control[index == BX_MSR_IA32_U_CET] = val_64;
      break;

    case BX_MSR_IA32_PL0_SSP:
    case BX_MSR_IA32_PL1_SSP:
    case BX_MSR_IA32_PL2_SSP:
    case BX_MSR_IA32_PL3_SSP:
      if (! is_cpu_extension_supported(BX_ISA_CET)) {
        BX_ERROR(("WRMSR BX_MSR_IA32_PLi_SSP: CET not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to BX_MSR_IA32_PLi_SSP !"));
        return 0;
      }
      if (val_64 & 0x03) {
        BX_ERROR(("WRMSR: attempt to write non 4byte-aligned address to BX_MSR_IA32_PLi_SSP !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.ia32_pl_ssp[index - BX_MSR_IA32_PL0_SSP] = val_64;
      break;

    case BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR:
      if (! is_cpu_extension_supported(BX_ISA_CET)) {
        BX_ERROR(("WRMSR BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR: CET not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table = val_64;
      break;
#endif

#if BX_SUPPORT_PKEYS
    case BX_MSR_IA32_PKRS:
      if (! is_cpu_extension_supported(BX_ISA_PKS)) {
        BX_ERROR(("WRMSR BX_MSR_IS32_PKS: Supervisor-Mode Protection Keys not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_CPU_THIS_PTR pkrs = val_64;
      set_PKeys(BX_CPU_THIS_PTR pkru, BX_CPU_THIS_PTR pkrs);
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_TSC_DEADLINE:
      if (! is_cpu_extension_supported(BX_ISA_TSC_DEADLINE)) {
        BX_ERROR(("WRMSR BX_MSR_TSC_DEADLINE: TSC-Deadline not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_CPU_THIS_PTR lapic.set_tsc_deadline(val_64);
      break;
#endif

    // SCA preention MSRs
    case BX_MSR_IA32_ARCH_CAPABILITIES:
      if (! is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS)) {
        BX_ERROR(("WRMSR IA32_ARCH_CAPABILITIES: not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_ERROR(("WRMSR: IA32_ARCH_CAPABILITIES is read only MSR"));
      return 0;

    case BX_MSR_IA32_SPEC_CTRL:
      if (! is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS)) {
        BX_ERROR(("WRMSR IA32_SPEC_CTRL: not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      //    [0] - Enable IBRS: Indirect Branch Restricted Speculation
      //    [1] - Enable STIBP: Single Thread Indirect Branch Predictors
      //    [2] - Enable SSCB: Speculative Store Bypass Disable
      // [63:3] - reserved
      if (val_64 & ~(BX_CONST64(0x7))) {
        BX_ERROR(("WRMSR: attempt to set reserved bits of IA32_SPEC_CTRL !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.ia32_spec_ctrl = val32_lo;
      break;

    case BX_MSR_IA32_PRED_CMD:
      if (! is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS)) {
        BX_ERROR(("WRMSR IA32_PRED_CMD: not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      //    [0] - Indirect Branch Prediction Barrier (IBPB)
      // [63:1] - reserved
      if (val_64 & ~(BX_CONST64(0x1))) {
        BX_ERROR(("WRMSR: attempt to set reserved bits of IA32_PRED_CMD !"));
        return 0;
      }
      // write only MSR, no need to remember written value
      break;

    case BX_MSR_IA32_FLUSH_CMD:
      if (! is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS)) {
        BX_ERROR(("WRMSR IA32_FLUSH_CMD: not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      //    [0] - WBINVD DL1 Cache
      // [63:1] - reserved
      if (val_64 & ~(BX_CONST64(0x1))) {
        BX_ERROR(("WRMSR: attempt to set reserved bits of IA32_FLUSH_CMD !"));
        return 0;
      }
      // write only MSR, no need to remember written value
      break;

#if BX_SUPPORT_VMX
    // Support only two bits: lock bit (bit 0) and VMX enable (bit 2)
    case BX_MSR_IA32_FEATURE_CONTROL:
      if (BX_CPU_THIS_PTR msr.ia32_feature_ctrl & 0x1) {
        BX_ERROR(("WRMSR: IA32_FEATURE_CONTROL_MSR VMX lock bit is set !"));
        return 0;
      }
/*
      if (val_64 & ~((Bit64u)(BX_IA32_FEATURE_CONTROL_BITS))) {
        BX_ERROR(("WRMSR: attempt to set reserved bits of IA32_FEATURE_CONTROL_MSR !"));
        return 0;
      }
*/
      BX_CPU_THIS_PTR msr.ia32_feature_ctrl = val32_lo;
      break;

    case BX_MSR_VMX_BASIC:
    case BX_MSR_VMX_PINBASED_CTRLS:
    case BX_MSR_VMX_PROCBASED_CTRLS:
    case BX_MSR_VMX_PROCBASED_CTRLS2:
    case BX_MSR_VMX_VMEXIT_CTRLS:
    case BX_MSR_VMX_VMENTRY_CTRLS:
    case BX_MSR_VMX_MISC:
    case BX_MSR_VMX_CR0_FIXED0:
    case BX_MSR_VMX_CR0_FIXED1:
    case BX_MSR_VMX_CR4_FIXED0:
    case BX_MSR_VMX_CR4_FIXED1:
    case BX_MSR_VMX_VMCS_ENUM:
    case BX_MSR_VMX_EPT_VPID_CAP:
    case BX_MSR_VMX_TRUE_PINBASED_CTRLS:
    case BX_MSR_VMX_TRUE_PROCBASED_CTRLS:
    case BX_MSR_VMX_TRUE_VMEXIT_CTRLS:
    case BX_MSR_VMX_TRUE_VMENTRY_CTRLS:
    case BX_MSR_VMX_VMFUNC:
      BX_ERROR(("WRMSR: VMX read only MSR"));
      return 0;
#endif

#if BX_SUPPORT_SVM
    case BX_SVM_HSAVE_PA_MSR:
      if (! is_cpu_extension_supported(BX_ISA_SVM)) {
        BX_ERROR(("WRMSR SVM_HSAVE_PA_MSR: SVM support not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsValidPageAlignedPhyAddr(val_64)) {
        BX_ERROR(("WRMSR SVM_HSAVE_PA_MSR: invalid or not page aligned physical address !"));
      }
      BX_CPU_THIS_PTR msr.svm_hsave_pa = val_64;
      break;
#endif

    case BX_MSR_EFER:
      if (! SetEFER(val_64)) return 0;
      break;

    case BX_MSR_STAR:
      if ((BX_CPU_THIS_PTR efer_suppmask & BX_EFER_SCE_MASK) == 0) {
        BX_ERROR(("WRMSR MSR_STAR: SYSCALL/SYSRET support not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_CPU_THIS_PTR msr.star = val_64;
      break;

#if BX_SUPPORT_X86_64
    case BX_MSR_LSTAR:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("WRMSR MSR_LSTAR: long mode support not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_LSTAR !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.lstar = val_64;
      break;

    case BX_MSR_CSTAR:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("WRMSR MSR_CSTAR: long mode support not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_CSTAR !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.cstar = val_64;
      break;

    case BX_MSR_FMASK:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("WRMSR MSR_FMASK: long mode support not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_CPU_THIS_PTR msr.fmask = (Bit32u) val_64;
      break;

    case BX_MSR_FSBASE:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("WRMSR MSR_FSBASE: long mode support not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_FSBASE !"));
        return 0;
      }
      MSR_FSBASE = val_64;
      break;

    case BX_MSR_GSBASE:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("WRMSR MSR_GSBASE: long mode support not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_GSBASE !"));
        return 0;
      }
      MSR_GSBASE = val_64;
      break;

    case BX_MSR_KERNELGSBASE:
      if (! is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
        BX_ERROR(("WRMSR MSR_KERNELGSBASE: long mode support not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      if (! IsCanonical(val_64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_KERNELGSBASE !"));
        return 0;
      }
      BX_CPU_THIS_PTR msr.kernelgsbase = val_64;
      break;

    case BX_MSR_TSC_AUX:
      if (! is_cpu_extension_supported(BX_ISA_RDTSCP)) {
        BX_ERROR(("WRMSR MSR_TSC_AUX: RDTSCP feature not enabled in the cpu model"));
        return handle_unknown_wrmsr(index, val_64);
      }
      BX_CPU_THIS_PTR msr.tsc_aux = val32_lo;
      break;
#endif  // #if BX_SUPPORT_X86_64

    default:
      return handle_unknown_wrmsr(index, val_64);
  }

  return 1;
}

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::handle_unknown_wrmsr(Bit32u index, Bit64u val_64)
{
  // Try to check cpuid_t first (can implement some MSRs)
  int result = BX_CPU_THIS_PTR cpuid->wrmsr(index, val_64);
  if (result == 0)
    return 0; // #GP fault due to not supported MSR

  if (result < 0) {
    // cpuid_t have no idea about this MSR
#if BX_CONFIGURE_MSRS
    if (index < BX_MSR_MAX_INDEX && BX_CPU_THIS_PTR msrs[index]) {
      if (! BX_CPU_THIS_PTR msrs[index]->set64(val_64)) {
        BX_ERROR(("WRMSR: Write failed to MSR %#x - #GP fault", index));
        return 0;
      }
      return 1;
    }
#endif
    // failed to find the MSR, could #GP or ignore it silently
    BX_ERROR(("WRMSR: Unknown register %#x", index));
    if (! BX_CPU_THIS_PTR ignore_bad_msrs)
      return 0; // will result in #GP fault due to unknown MSR
  }

  return 1;
}

#endif // BX_CPU_LEVEL >= 5

#if BX_SUPPORT_APIC
bool BX_CPU_C::relocate_apic(Bit64u val_64)
{
  /* MSR_APICBASE
   *  [0:7]  Reserved
   *    [8]  This is set if CPU is BSP
   *    [9]  Reserved
   *   [10]  X2APIC mode bit (1=enabled 0=disabled)
   *   [11]  APIC Global Enable bit (1=enabled 0=disabled)
   * [12:M]  APIC Base Address (physical)
   * [M:63]  Reserved
   */

  const Bit32u BX_MSR_APICBASE_RESERVED_BITS = (0x2ff | (is_cpu_extension_supported(BX_ISA_X2APIC) ? 0 : 0x400));

  if (BX_CPU_THIS_PTR msr.apicbase & 0x800) {
    Bit32u val32_hi = GET32H(val_64), val32_lo = GET32L(val_64);
    BX_INFO(("WRMSR: wrote %08x:%08x to MSR_APICBASE", val32_hi, val32_lo));
    if (! IsValidPhyAddr(val_64)) {
      BX_ERROR(("relocate_apic: invalid physical address"));
      return 0;
    }
    if (val32_lo & BX_MSR_APICBASE_RESERVED_BITS) {
      BX_ERROR(("relocate_apic: attempt to set reserved bits"));
      return 0;
    }

#if BX_CPU_LEVEL >= 6
    if (is_cpu_extension_supported(BX_ISA_X2APIC)) {
      unsigned apic_state = (BX_CPU_THIS_PTR msr.apicbase >> 10) & 3;
      unsigned new_state = (val32_lo >> 10) & 3;

      if (new_state != apic_state) {
        if (new_state == BX_APIC_STATE_INVALID) {
          BX_ERROR(("relocate_apic: attempt to set invalid apic state"));
          return 0;
        }
        if (apic_state == BX_APIC_X2APIC_MODE && new_state != BX_APIC_GLOBALLY_DISABLED) {
          BX_ERROR(("relocate_apic: attempt to switch from x2apic -> xapic"));
          return 0;
        }
      }
    }
#endif

    BX_CPU_THIS_PTR msr.apicbase = (bx_phy_address) val_64;
    BX_CPU_THIS_PTR lapic.set_base(BX_CPU_THIS_PTR msr.apicbase);
    // TLB flush is required for emulation correctness
    TLB_flush();  // don't care about performance of apic relocation
  }
  else {
    BX_INFO(("WRMSR: MSR_APICBASE APIC global enable bit cleared !"));
  }

  return 1;
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRMSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  // CPL is always 0 in real mode
  if (/* !real_mode() && */ CPL!=0) {
    BX_ERROR(("WRMSR: CPL != 0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0);
  }

  invalidate_prefetch_q();

  Bit64u val_64 = ((Bit64u) EDX << 32) | EAX;
  Bit32u index = ECX;

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_MSR)) SvmInterceptMSR(BX_WRITE, index);
  }
#endif

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest)
    VMexit_MSR(VMX_VMEXIT_WRMSR, index);
#endif

#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_X2APIC_MODE)) {
      if (Virtualize_X2APIC_Write(index, val_64))
        BX_NEXT_INSTR(i);
    }
  }
#endif

  if (! wrmsr(index, val_64))
    exception(BX_GP_EXCEPTION, 0);
#endif

  BX_NEXT_TRACE(i);
}

#if BX_CONFIGURE_MSRS

int BX_CPU_C::load_MSRs(const char *file)
{
  char line[512];
  unsigned linenum = 0;
  Bit32u index, type;
  Bit32u reset_hi, reset_lo;
  Bit32u rsrv_hi, rsrv_lo;
  Bit32u ignr_hi, ignr_lo;

  FILE *fd = fopen (file, "r");
  if (fd == NULL) return -1;
  int retval = 0;
  do {
    linenum++;
    char* ret = fgets(line, sizeof(line)-1, fd);
    line[sizeof(line) - 1] = '\0';
    size_t len = strlen(line);
    if (len>0 && line[len-1] < ' ')
      line[len-1] = '\0';

    if (ret != NULL && strlen(line)) {
      if (line[0] == '#') continue;
      retval = sscanf(line, "%x %d %08x %08x %08x %08x %08x %08x",
         &index, &type, &reset_hi, &reset_lo, &rsrv_hi, &rsrv_lo, &ignr_hi, &ignr_lo);

      if (retval < 8) {
        retval = -1;
        BX_PANIC(("%s:%d > error parsing MSRs config file!", file, linenum));
        break;  // quit parsing after first error
      }
      if (index >= BX_MSR_MAX_INDEX) {
        BX_PANIC(("%s:%d > MSR index is too big !", file, linenum));
        continue;
      }
      if (BX_CPU_THIS_PTR msrs[index]) {
        BX_PANIC(("%s:%d > MSR[0x%03x] is already defined!", file, linenum, index));
        continue;
      }
      if (type > 2) {
        BX_PANIC(("%s:%d > MSR[0x%03x] unknown type !", file, linenum, index));
        continue;
      }

      BX_INFO(("loaded MSR[0x%03x] type=%d %08x:%08x %08x:%08x %08x:%08x", index, type,
        reset_hi, reset_lo, rsrv_hi, rsrv_lo, ignr_hi, ignr_lo));

      BX_CPU_THIS_PTR msrs[index] = new MSR(index, type,
        ((Bit64u)(reset_hi) << 32) | reset_lo,
        ((Bit64u) (rsrv_hi) << 32) | rsrv_lo,
        ((Bit64u) (ignr_hi) << 32) | ignr_lo);
    }
  } while (!feof(fd));

  fclose(fd);
  return retval;
}

#endif
