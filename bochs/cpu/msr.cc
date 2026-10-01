/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2026 Stanislav Shwartsman
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
#include "cpuid.h"
#include "msr.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_SVM
#include "svm.h"
#endif

#if BX_SUPPORT_APIC
#include "apic.h"
#endif

#include "decoder/ia_opcodes.h"

#if BX_CPU_LEVEL >= 5

// canonical width is defined by the cpu model (LA57), selected once in init_MSRs()
static MSR_Valid_Value_Check is_canonical_msr = NULL;

#if BX_SUPPORT_CET
extern bool is_valid_cet_control(Bit64u val);

// CET control MSRs must hold canonical address and valid CET control bits
static bool is_valid_cet_control_msr(Bit64u val) { return is_canonical_msr(val) && is_valid_cet_control(val); }
#endif

#if BX_CPU_LEVEL >= 6
bool isMemTypeValidMTRR(unsigned memtype);
bool isValidMSR_PAT(Bit64u pat_val);
bool isValidMSR_FixedMTRR(Bit64u fixed_mtrr_val);

// variable range MTRR base must be valid physical address, bits [11:0] are memory type and reserved bits [11:8]
static bool is_valid_mtrr_physbase(Bit64u val) { return IsValidPhyAddr(val) && isMemTypeValidMTRR(val & 0xFFF); }
static bool is_valid_mtrr_physmask(Bit64u val) { return IsValidPhyAddr(val); }
#endif

// MSR descriptors are per cpu, some of them refer to the cpu variables
void BX_CPU_C::init_MSRs()
{
#if BX_SUPPORT_X86_64
  is_canonical_msr = is_cpu_extension_supported(BX_ISA_LA57) ? IsCanonical57 : IsCanonical48;
#endif

  msr_desc = new MSR_DescriptorPtr[BX_MSR_MAX_INDEX];
  for (unsigned i=0;i < BX_MSR_MAX_INDEX; i++)
    msr_desc[i] = NULL;

  msr_desc[BX_MSR_TSC] = new MSR_Descriptor("MSR_IA32_TSC", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_PLATFORM_ID] = new ConstMSR("MSR_PLATFORM_ID", BX_ISA_PENTIUM, 0); // read only

#if BX_SUPPORT_APIC
  msr_desc[BX_MSR_APICBASE] = new MSR_Descriptor("MSR_APICBASE", BX_ISA_PENTIUM);
#endif

#if BX_SUPPORT_X86_64
  msr_desc[BX_MSR_IA32_USER_MSR_CTL] = new VarMSR("MSR_IA32_USER_MSR_CTL", BX_ISA_USER_MSR, &BX_CPU_THIS_PTR msr.ia32_user_msr_ctrl, 0, is_canonical_msr);
#endif

  msr_desc[BX_MSR_IA32_APERF] = new MSR_Descriptor("MSR_IA32_APERF", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_IA32_MPERF] = new MSR_Descriptor("MSR_IA32_MPERF", BX_ISA_PENTIUM);

#if BX_CPU_LEVEL >= 6
  msr_desc[BX_MSR_SYSENTER_CS] = new MSR_Descriptor("MSR_IA32_SYSENTER_CS", BX_ISA_SYSENTER_SYSEXIT);
  msr_desc[BX_MSR_SYSENTER_ESP] = new VarMSR("MSR_IA32_SYSENTER_ESP", BX_ISA_SYSENTER_SYSEXIT, &BX_CPU_THIS_PTR msr.sysenter_esp_msr, 0, is_canonical_msr);
  msr_desc[BX_MSR_SYSENTER_EIP] = new VarMSR("MSR_IA32_SYSENTER_EIP", BX_ISA_SYSENTER_SYSEXIT, &BX_CPU_THIS_PTR msr.sysenter_eip_msr, 0, is_canonical_msr);
#endif

#if BX_CPU_LEVEL >= 6
  msr_desc[BX_MSR_MTRRCAP] = new ConstMSR("MSR_IA32_MTRR_CAP", BX_ISA_MTRR, BX_CONST64(0x0000000000000500) | BX_NUM_VARIABLE_RANGE_MTRRS); // read only
  msr_desc[BX_MSR_MTRRPHYSBASE0] = new VarMSR("MSR_IA32_MTRRPHYSBASE0", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[0], 0, is_valid_mtrr_physbase);
  msr_desc[BX_MSR_MTRRPHYSMASK0] = new VarMSR("MSR_IA32_MTRRPHYSMASK0", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[1], BX_CONST64(0x7ff), is_valid_mtrr_physmask); // bits [10:0] are reserved
  msr_desc[BX_MSR_MTRRPHYSBASE1] = new VarMSR("MSR_IA32_MTRRPHYSBASE1", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[2], 0, is_valid_mtrr_physbase);
  msr_desc[BX_MSR_MTRRPHYSMASK1] = new VarMSR("MSR_IA32_MTRRPHYSMASK1", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[3], BX_CONST64(0x7ff), is_valid_mtrr_physmask);
  msr_desc[BX_MSR_MTRRPHYSBASE2] = new VarMSR("MSR_IA32_MTRRPHYSBASE2", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[4], 0, is_valid_mtrr_physbase);
  msr_desc[BX_MSR_MTRRPHYSMASK2] = new VarMSR("MSR_IA32_MTRRPHYSMASK2", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[5], BX_CONST64(0x7ff), is_valid_mtrr_physmask);
  msr_desc[BX_MSR_MTRRPHYSBASE3] = new VarMSR("MSR_IA32_MTRRPHYSBASE3", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[6], 0, is_valid_mtrr_physbase);
  msr_desc[BX_MSR_MTRRPHYSMASK3] = new VarMSR("MSR_IA32_MTRRPHYSMASK3", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[7], BX_CONST64(0x7ff), is_valid_mtrr_physmask);
  msr_desc[BX_MSR_MTRRPHYSBASE4] = new VarMSR("MSR_IA32_MTRRPHYSBASE4", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[8], 0, is_valid_mtrr_physbase);
  msr_desc[BX_MSR_MTRRPHYSMASK4] = new VarMSR("MSR_IA32_MTRRPHYSMASK4", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[9], BX_CONST64(0x7ff), is_valid_mtrr_physmask);
  msr_desc[BX_MSR_MTRRPHYSBASE5] = new VarMSR("MSR_IA32_MTRRPHYSBASE5", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[10], 0, is_valid_mtrr_physbase);
  msr_desc[BX_MSR_MTRRPHYSMASK5] = new VarMSR("MSR_IA32_MTRRPHYSMASK5", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[11], BX_CONST64(0x7ff), is_valid_mtrr_physmask);
  msr_desc[BX_MSR_MTRRPHYSBASE6] = new VarMSR("MSR_IA32_MTRRPHYSBASE6", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[12], 0, is_valid_mtrr_physbase);
  msr_desc[BX_MSR_MTRRPHYSMASK6] = new VarMSR("MSR_IA32_MTRRPHYSMASK6", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[13], BX_CONST64(0x7ff), is_valid_mtrr_physmask);
  msr_desc[BX_MSR_MTRRPHYSBASE7] = new VarMSR("MSR_IA32_MTRRPHYSBASE7", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[14], 0, is_valid_mtrr_physbase);
  msr_desc[BX_MSR_MTRRPHYSMASK7] = new VarMSR("MSR_IA32_MTRRPHYSMASK7", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrphys[15], BX_CONST64(0x7ff), is_valid_mtrr_physmask);

  msr_desc[BX_MSR_MTRRFIX64K_00000] = new VarMSR("MSR_IA32_MTRRFIX64K_00000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix64k.u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX16K_80000] = new VarMSR("MSR_IA32_MTRRFIX16K_80000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix16k[0].u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX16K_A0000] = new VarMSR("MSR_IA32_MTRRFIX16K_A0000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix16k[1].u64, 0, isValidMSR_FixedMTRR);

  msr_desc[BX_MSR_MTRRFIX4K_C0000] = new VarMSR("MSR_IA32_MTRRFIX4K_C0000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix4k[0].u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX4K_C8000] = new VarMSR("MSR_IA32_MTRRFIX4K_C8000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix4k[1].u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX4K_D0000] = new VarMSR("MSR_IA32_MTRRFIX4K_D0000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix4k[2].u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX4K_D8000] = new VarMSR("MSR_IA32_MTRRFIX4K_D8000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix4k[3].u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX4K_E0000] = new VarMSR("MSR_IA32_MTRRFIX4K_E0000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix4k[4].u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX4K_E8000] = new VarMSR("MSR_IA32_MTRRFIX4K_E8000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix4k[5].u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX4K_F0000] = new VarMSR("MSR_IA32_MTRRFIX4K_F0000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix4k[6].u64, 0, isValidMSR_FixedMTRR);
  msr_desc[BX_MSR_MTRRFIX4K_F8000] = new VarMSR("MSR_IA32_MTRRFIX4K_F8000", BX_ISA_MTRR, &BX_CPU_THIS_PTR msr.mtrrfix4k[7].u64, 0, isValidMSR_FixedMTRR);

  msr_desc[BX_MSR_MTRR_DEFTYPE] = new MSR_Descriptor("MSR_IA32_MTRR_DEFTYPE", BX_ISA_MTRR, BX_CONST64(0xfffffffffffff300)); // bits [63-12], [9:8] are reserved
  msr_desc[BX_MSR_PAT] = new VarMSR("MSR_IA32_PAT", BX_ISA_PAT, &BX_CPU_THIS_PTR msr.pat.u64, 0, isValidMSR_PAT);
#endif

  msr_desc[BX_MSR_TSC_ADJUST] = new MSR_Descriptor("MSR_IA32_TSC_ADJUST", BX_ISA_TSC_ADJUST);

#if BX_SUPPORT_MONITOR_MWAIT
  msr_desc[BX_MSR_IA32_UMWAIT_CONTROL] = new MSR_Descriptor("MSR_IA32_UMWAIT_CONTROL", BX_ISA_WAITPKG);
#endif

#if BX_CPU_LEVEL >= 6
  msr_desc[BX_MSR_XSS] = new VarMSR("MSR_IA32_XSS", BX_ISA_XSAVES, &BX_CPU_THIS_PTR msr.ia32_xss, ~Bit64u(get_ia32_xss_allow_mask()));
#endif

#if BX_SUPPORT_CET
  msr_desc[BX_MSR_IA32_U_CET] = new VarMSR("MSR_IA32_U_CET", BX_ISA_CET, &BX_CPU_THIS_PTR msr.ia32_cet_control[1], 0, is_valid_cet_control_msr);
  msr_desc[BX_MSR_IA32_S_CET] = new VarMSR("MSR_IA32_S_CET", BX_ISA_CET, &BX_CPU_THIS_PTR msr.ia32_cet_control[0], 0, is_valid_cet_control_msr);

  msr_desc[BX_MSR_IA32_PL0_SSP] = new VarMSR("MSR_IA32_PL0_SSP", BX_ISA_CET, &BX_CPU_THIS_PTR msr.ia32_pl_ssp[0], BX_CONST64(0x3), is_canonical_msr); // force 4-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_PL1_SSP] = new VarMSR("MSR_IA32_PL1_SSP", BX_ISA_CET, &BX_CPU_THIS_PTR msr.ia32_pl_ssp[1], BX_CONST64(0x3), is_canonical_msr); // force 4-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_PL2_SSP] = new VarMSR("MSR_IA32_PL2_SSP", BX_ISA_CET, &BX_CPU_THIS_PTR msr.ia32_pl_ssp[2], BX_CONST64(0x3), is_canonical_msr); // force 4-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_PL3_SSP] = new VarMSR("MSR_IA32_PL3_SSP", BX_ISA_CET, &BX_CPU_THIS_PTR msr.ia32_pl_ssp[3], BX_CONST64(0x3), is_canonical_msr); // force 4-byte alignment of the MSR value

  msr_desc[BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR] = new VarMSR("MSR_IA32_INTERRUPT_SSP_TABLE_ADDR", BX_ISA_CET, &BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table, 0, is_canonical_msr);
#endif

#if BX_SUPPORT_UINTR
  msr_desc[BX_MSR_IA32_UINTR_RR] = new MSR_Descriptor("MSR_IA32_UINTR_RR", BX_ISA_UINTR);
  msr_desc[BX_MSR_IA32_UINTR_HANDLER] = new VarMSR("MSR_IA32_UINTR_HANDLER", BX_ISA_UINTR, &BX_CPU_THIS_PTR uintr.ui_handler, 0, is_canonical_msr);
  msr_desc[BX_MSR_IA32_UINTR_STACKADJUST] = new VarMSR("MSR_IA32_UINTR_STACKADJUST", BX_ISA_UINTR, &BX_CPU_THIS_PTR uintr.stack_adjust, 0, is_canonical_msr);
  msr_desc[BX_MSR_IA32_UINTR_MISC] = new MSR_Descriptor("MSR_IA32_UINTR_MISC", BX_ISA_UINTR, BX_CONST64(0xffffff0000000000));
  msr_desc[BX_MSR_IA32_UINTR_PD] = new VarMSR("MSR_IA32_UINTR_PD", BX_ISA_UINTR, &BX_CPU_THIS_PTR uintr.upid_addr, BX_CONST64(0x3f), is_canonical_msr); // bits [5:0] are reserved
  msr_desc[BX_MSR_IA32_UINTR_TT] = new VarMSR("MSR_IA32_UINTR_TT", BX_ISA_UINTR, &BX_CPU_THIS_PTR uintr.uitt_addr, BX_CONST64(0x0e), is_canonical_msr); // bits [3:1] are reserved
#endif

#if BX_SUPPORT_PKEYS
  msr_desc[BX_MSR_IA32_PKRS] = new MSR_Descriptor("MSR_IA32_PKRS", BX_ISA_PKS);
#endif

#if BX_SUPPORT_FRED
  msr_desc[BX_MSR_IA32_FRED_RSP0] = new VarMSR("MSR_IA32_FRED_RSP0", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_rsp[0], BX_CONST64(0x3f), is_canonical_msr); // force 64-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_FRED_RSP1] = new VarMSR("MSR_IA32_FRED_RSP1", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_rsp[1], BX_CONST64(0x3f), is_canonical_msr); // force 64-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_FRED_RSP2] = new VarMSR("MSR_IA32_FRED_RSP2", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_rsp[2], BX_CONST64(0x3f), is_canonical_msr); // force 64-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_FRED_RSP3] = new VarMSR("MSR_IA32_FRED_RSP3", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_rsp[3], BX_CONST64(0x3f), is_canonical_msr); // force 64-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_FRED_STKLVLS] = new VarMSR("MSR_IA32_FRED_STKLVLS", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_stack_levels);
#if BX_SUPPORT_CET
  msr_desc[BX_MSR_IA32_FRED_SSP1] = new VarMSR("MSR_IA32_FRED_SSP1", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_ssp[1], BX_CONST64(0x7), is_canonical_msr); // force 8-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_FRED_SSP2] = new VarMSR("MSR_IA32_FRED_SSP2", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_ssp[2], BX_CONST64(0x7), is_canonical_msr); // force 8-byte alignment of the MSR value
  msr_desc[BX_MSR_IA32_FRED_SSP3] = new VarMSR("MSR_IA32_FRED_SSP3", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_ssp[3], BX_CONST64(0x7), is_canonical_msr); // force 8-byte alignment of the MSR value
#endif
  msr_desc[BX_MSR_IA32_FRED_CONFIG] = new VarMSR("MSR_IA32_FRED_CONFIG", BX_ISA_FRED, &BX_CPU_THIS_PTR msr.ia32_fred_cfg, BX_CONST64(0x834), is_canonical_msr);
#endif

#if BX_CPU_LEVEL >= 6
  msr_desc[BX_MSR_TSC_DEADLINE] = new MSR_Descriptor("MSR_TSC_DEADLINE", BX_ISA_TSC_DEADLINE);
#endif

  // artificial MSR for MRSLIST serialization
  msr_desc[BX_MSR_IA32_BARRIER] = new MSR_Descriptor("MSR_IA32_BARRIER", BX_ISA_MSRLIST);

  // SCA prevention MSRs
  // IA32_ARCH_CAPABILITIES MSR:
  //     [0]: RDCL_NO: The processor is not susceptible to Rogue Data Cache Load (RDCL)
  //     [1]: IBRS_ALL: The processor supports enhanced IBRS
  //     [2]: RSBA: The processor supports RSB Alternate
  //     [3]: SKIP_L1DFL_VMENTRY: indicates the hypervisor need not flush the L1D on VM entry
  //     [4]: SSB_NO: Processor is not susceptible to Speculative Store Bypass
  //  [63:5]: reserved
  msr_desc[BX_MSR_IA32_ARCH_CAPABILITIES] = new ConstMSR("MSR_IA32_ARCH_CAPABILITIES", BX_ISA_SCA_MITIGATIONS, 0x1F); // read only, set bits [4:0]
  msr_desc[BX_MSR_IA32_SPEC_CTRL] = new MSR_Descriptor("MSR_IA32_SPEC_CTRL", BX_ISA_SCA_MITIGATIONS);
  // IA32_PRED_CMD MSR:
  //    [0] - Indirect Branch Prediction Barrier (IBPB)
  // [63:1] - reserved
  msr_desc[BX_MSR_IA32_PRED_CMD] = new MSR_Descriptor("MSR_IA32_PRED_CMD", BX_ISA_SCA_MITIGATIONS, ~BX_CONST64(1));
  // IA32_FLUSH_CMD MSR:
  //    [0] - WBINVD DL1 Cache
  // [63:1] - reserved
  msr_desc[BX_MSR_IA32_FLUSH_CMD] = new MSR_Descriptor("MSR_IA32_FLUSH_CMD", BX_ISA_SCA_MITIGATIONS, ~BX_CONST64(1));

#if BX_SUPPORT_VMX
  msr_desc[BX_MSR_IA32_FEATURE_CONTROL] = new MSR_Descriptor("MSR_IA32_FEATURE_CONTROL", BX_ISA_VMX);
  // all these VMX MSRs are read only, the values are computed from vmx_cap so init_VMCS() must be called before
  msr_desc[BX_MSR_VMX_BASIC] = new ConstMSR("MSR_VMX_BASIC", BX_ISA_VMX, VMX_MSR_VMX_BASIC);
  msr_desc[BX_MSR_VMX_PINBASED_CTRLS] = new ConstMSR("MSR_VMX_PINBASED_CTRLS", BX_ISA_VMX, VMX_MSR_VMX_PINBASED_CTRLS);
  msr_desc[BX_MSR_VMX_PROCBASED_CTRLS] = new ConstMSR("MSR_VMX_PROCBASED_CTRLS", BX_ISA_VMX, VMX_MSR_VMX_PROCBASED_CTRLS);
  msr_desc[BX_MSR_VMX_VMEXIT_CTRLS] = new ConstMSR("MSR_VMX_VMEXIT_CTRLS", BX_ISA_VMX, VMX_MSR_VMX_VMEXIT_CTRLS);
  msr_desc[BX_MSR_VMX_VMEXIT_CTRLS2] = new ConstMSR("MSR_VMX_VMEXIT_CTRLS2", BX_ISA_VMX, VMX_MSR_VMX_VMEXIT_CTRLS2);
  if (! BX_CPU_THIS_PTR vmx_cap.vmx_vmexit_ctrl2_supported_bits)
    msr_desc[BX_MSR_VMX_VMEXIT_CTRLS2]->disable();
  msr_desc[BX_MSR_VMX_VMENTRY_CTRLS] = new ConstMSR("MSR_VMX_VMENTRY_CTRLS", BX_ISA_VMX, VMX_MSR_VMX_VMENTRY_CTRLS);
  msr_desc[BX_MSR_VMX_PROCBASED_CTRLS2] = new ConstMSR("MSR_VMX_PROCBASED_CTRLS2", BX_ISA_VMX, VMX_MSR_VMX_PROCBASED_CTRLS2);
  if (! BX_CPU_THIS_PTR vmx_cap.vmx_vmexec_ctrl2_supported_bits)
    msr_desc[BX_MSR_VMX_PROCBASED_CTRLS2]->disable();
  msr_desc[BX_MSR_VMX_PROCBASED_CTRLS3] = new ConstMSR("MSR_VMX_PROCBASED_CTRLS3", BX_ISA_VMX, VMX_MSR_VMX_PROCBASED_CTRLS3);
  if (! BX_CPU_THIS_PTR vmx_cap.vmx_vmexec_ctrl3_supported_bits)
    msr_desc[BX_MSR_VMX_PROCBASED_CTRLS3]->disable();
#if BX_SUPPORT_VMX >= 2
  msr_desc[BX_MSR_VMX_TRUE_PINBASED_CTRLS] = new ConstMSR("MSR_VMX_TRUE_PINBASED_CTRLS", BX_ISA_VMX, VMX_MSR_VMX_TRUE_PINBASED_CTRLS);
  msr_desc[BX_MSR_VMX_TRUE_PROCBASED_CTRLS] = new ConstMSR("MSR_VMX_TRUE_PROCBASED_CTRLS", BX_ISA_VMX, VMX_MSR_VMX_TRUE_PROCBASED_CTRLS);
  msr_desc[BX_MSR_VMX_TRUE_VMEXIT_CTRLS] = new ConstMSR("MSR_VMX_TRUE_VMEXIT_CTRLS", BX_ISA_VMX, VMX_MSR_VMX_TRUE_VMEXIT_CTRLS);
  msr_desc[BX_MSR_VMX_TRUE_VMENTRY_CTRLS] = new ConstMSR("MSR_VMX_TRUE_VMENTRY_CTRLS", BX_ISA_VMX, VMX_MSR_VMX_TRUE_VMENTRY_CTRLS);
  msr_desc[BX_MSR_VMX_EPT_VPID_CAP] = new ConstMSR("MSR_VMX_EPT_VPID_CAP", BX_ISA_VMX, VMX_MSR_VMX_EPT_VPID_CAP);
  if (VMX_MSR_VMX_EPT_VPID_CAP == 0)
    msr_desc[BX_MSR_VMX_EPT_VPID_CAP]->disable();
  msr_desc[BX_MSR_VMX_VMFUNC] = new ConstMSR("MSR_VMX_VMFUNC", BX_ISA_VMX, BX_CPU_THIS_PTR vmx_cap.vmx_vmfunc_supported_bits);
  if (! BX_CPU_THIS_PTR vmx_cap.vmx_vmfunc_supported_bits)
    msr_desc[BX_MSR_VMX_VMFUNC]->disable();
#endif
  msr_desc[BX_MSR_VMX_MISC] = new ConstMSR("MSR_VMX_MISC", BX_ISA_VMX, VMX_MSR_MISC);
  msr_desc[BX_MSR_VMX_CR0_FIXED0] = new ConstMSR("MSR_VMX_CR0_FIXED0", BX_ISA_VMX, VMX_MSR_CR0_FIXED0);
  msr_desc[BX_MSR_VMX_CR0_FIXED1] = new ConstMSR("MSR_VMX_CR0_FIXED1", BX_ISA_VMX, VMX_MSR_CR0_FIXED1);
  msr_desc[BX_MSR_VMX_CR4_FIXED0] = new ConstMSR("MSR_VMX_CR4_FIXED0", BX_ISA_VMX, VMX_MSR_CR4_FIXED0);
  // VMX_MSR_CR4_FIXED1 is cr4_suppmask which is not initialized yet
  msr_desc[BX_MSR_VMX_CR4_FIXED1] = new ConstMSR("MSR_VMX_CR4_FIXED1", BX_ISA_VMX, get_cr4_allow_mask());
  msr_desc[BX_MSR_VMX_VMCS_ENUM] = new ConstMSR("MSR_VMX_VMCS_ENUM", BX_ISA_VMX, VMX_MSR_VMCS_ENUM);
#endif

#if BX_SUPPORT_PERFMON
  msr_desc[BX_MSR_PERFEVTSEL0] = new MSR_Descriptor("MSR_IA32_PERFEVTSEL0", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_PERFEVTSEL1] = new MSR_Descriptor("MSR_IA32_PERFEVTSEL1", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_PERFEVTSEL2] = new MSR_Descriptor("MSR_IA32_PERFEVTSEL2", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_PERFEVTSEL3] = new MSR_Descriptor("MSR_IA32_PERFEVTSEL3", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_PERFEVTSEL4] = new MSR_Descriptor("MSR_IA32_PERFEVTSEL4", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_PERFEVTSEL5] = new MSR_Descriptor("MSR_IA32_PERFEVTSEL5", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_PERFEVTSEL6] = new MSR_Descriptor("MSR_IA32_PERFEVTSEL6", BX_ISA_PENTIUM);
  msr_desc[BX_MSR_PERFEVTSEL7] = new MSR_Descriptor("MSR_IA32_PERFEVTSEL7", BX_ISA_PENTIUM);
#endif

  ext_msr_desc = new MSR_DescriptorPtr[BX_EXTENDED_MSR_MAX_INDEX];
  for (unsigned i=0;i < BX_EXTENDED_MSR_MAX_INDEX; i++)
    ext_msr_desc[i] = NULL;

  ext_msr_desc[BX_MSR_EFER - 0xc0000000] = new MSR_Descriptor("MSR_EFER", BX_ISA_PENTIUM, ~Bit64u(get_efer_allow_mask()));
  // requires BX_ISA_SYSCALL_SYSRET_LEGACY or BX_ISA_LONG_MODE
  ext_msr_desc[BX_MSR_STAR - 0xc0000000] = new VarMSR("MSR_STAR", is_cpu_extension_supported(BX_ISA_LONG_MODE) ? BX_ISA_LONG_MODE : BX_ISA_SYSCALL_SYSRET_LEGACY, &BX_CPU_THIS_PTR msr.star);

#if BX_SUPPORT_X86_64
  ext_msr_desc[BX_MSR_LSTAR - 0xc0000000] = new VarMSR("MSR_LSTAR", BX_ISA_LONG_MODE, &BX_CPU_THIS_PTR msr.lstar, 0, is_canonical_msr);
  ext_msr_desc[BX_MSR_CSTAR - 0xc0000000] = new VarMSR("MSR_CSTAR", BX_ISA_LONG_MODE, &BX_CPU_THIS_PTR msr.cstar, 0, is_canonical_msr);
  ext_msr_desc[BX_MSR_FMASK - 0xc0000000] = new MSR_Descriptor("MSR_FMASK", BX_ISA_LONG_MODE);
  ext_msr_desc[BX_MSR_FSBASE - 0xc0000000] = new VarMSR("MSR_FSBASE", BX_ISA_LONG_MODE, &MSR_FSBASE, 0, is_canonical_msr);
  ext_msr_desc[BX_MSR_GSBASE - 0xc0000000] = new VarMSR("MSR_GSBASE", BX_ISA_LONG_MODE, &MSR_GSBASE, 0, is_canonical_msr);
  ext_msr_desc[BX_MSR_KERNELGSBASE - 0xc0000000] = new VarMSR("MSR_KERNELGSBASE", BX_ISA_LONG_MODE, &BX_CPU_THIS_PTR msr.kernelgsbase, 0, is_canonical_msr);
  ext_msr_desc[BX_MSR_TSC_AUX - 0xc0000000] = new MSR_Descriptor("MSR_TSC_AUX", BX_ISA_RDTSCP);
#endif
}

void BX_CPU_C::destroy_MSRs()
{
  if (msr_desc) {
    for (unsigned i=0;i < BX_MSR_MAX_INDEX; i++)
      delete msr_desc[i];

    delete [] msr_desc;
    msr_desc = NULL;
  }

  if (ext_msr_desc) {
    for (unsigned i=0;i < BX_EXTENDED_MSR_MAX_INDEX; i++)
      delete ext_msr_desc[i];

    delete [] ext_msr_desc;
    ext_msr_desc = NULL;
  }
}

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::rdmsr(Bit32u index, Bit64u *msr)
{
  Bit64u val64 = 0;

#if BX_SUPPORT_VMX >= 2
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (vm->vmexec_ctrls2.VIRTUALIZE_X2APIC_MODE()) {
      if (index >= 0x800 && index <= 0x8FF) {
        if (index == 0x808 || vm->vmexec_ctrls2.VIRTUALIZE_APIC_REGISTERS()) {
          unsigned vapic_offset = (index & 0xff) << 4;
          Bit32u msr_lo = VMX_Read_Virtual_APIC(vapic_offset);
          Bit32u msr_hi = VMX_Read_Virtual_APIC(vapic_offset + 4);
          *msr = GET64_FROM_HI32_LO32(msr_hi, msr_lo);
          return true;
        }
      }
    }
  }
#endif

#if BX_CPU_LEVEL >= 6
  if (is_cpu_extension_supported(BX_ISA_X2APIC)) {
    if (is_x2apic_msr_range(index)) {
      if (x2apic_mode())
        return BX_CPU_THIS_PTR lapic->read_x2apic(x2apic_msr_to_apic_register_index(index), msr);
      else
        return false;
    }
  }
#endif

  if (index < BX_MSR_MAX_INDEX || (index >= 0xc0000000 && index < (0xc0000000 + BX_EXTENDED_MSR_MAX_INDEX))) {
    MSR_Descriptor *msr_desciptor = (index >= 0xc0000000) ? BX_CPU_THIS_PTR ext_msr_desc[index - 0xc0000000] : BX_CPU_THIS_PTR msr_desc[index];
    if (! msr_desciptor) return handle_unknown_rdmsr(index, msr);
    if (! is_cpu_extension_supported(msr_desciptor->get_cpu_feature())) {
      BX_ERROR(("RDMSR %s: '%s' feature not enabled in the cpu model", msr_desciptor->get_name(), get_cpu_feature_name(msr_desciptor->get_cpu_feature())));
      return false;
    }
    if (! msr_desciptor->is_enabled()) {
      BX_ERROR(("RDMSR %s: not supported by the cpu model", msr_desciptor->get_name()));
      return false;
    }

    switch(index) {
    // MSRs converted to generic MSR descriptor interface
    case BX_MSR_PLATFORM_ID:                    // 0x017
#if BX_SUPPORT_X86_64
    case BX_MSR_IA32_USER_MSR_CTL:              // 0x01c
#endif
#if BX_CPU_LEVEL >= 6
    case BX_MSR_MTRRCAP:                        // 0x0fe
#endif
    case BX_MSR_IA32_ARCH_CAPABILITIES:         // 0x10a
#if BX_CPU_LEVEL >= 6
    case BX_MSR_SYSENTER_ESP:                   // 0x175
    case BX_MSR_SYSENTER_EIP:                   // 0x176
#endif
#if BX_SUPPORT_FRED
    case BX_MSR_IA32_FRED_RSP0:                 // 0x1cc
    case BX_MSR_IA32_FRED_RSP1:                 // 0x1cd
    case BX_MSR_IA32_FRED_RSP2:                 // 0x1ce
    case BX_MSR_IA32_FRED_RSP3:                 // 0x1cf
    case BX_MSR_IA32_FRED_STKLVLS:              // 0x1d0
#if BX_SUPPORT_CET
    case BX_MSR_IA32_FRED_SSP1:                 // 0x1d1
    case BX_MSR_IA32_FRED_SSP2:                 // 0x1d2
    case BX_MSR_IA32_FRED_SSP3:                 // 0x1d3
#endif
    case BX_MSR_IA32_FRED_CONFIG:               // 0x1d4
#endif
#if BX_CPU_LEVEL >= 6
    case BX_MSR_MTRRPHYSBASE0:                  // 0x200
    case BX_MSR_MTRRPHYSMASK0:                  // 0x201
    case BX_MSR_MTRRPHYSBASE1:                  // 0x202
    case BX_MSR_MTRRPHYSMASK1:                  // 0x203
    case BX_MSR_MTRRPHYSBASE2:                  // 0x204
    case BX_MSR_MTRRPHYSMASK2:                  // 0x205
    case BX_MSR_MTRRPHYSBASE3:                  // 0x206
    case BX_MSR_MTRRPHYSMASK3:                  // 0x207
    case BX_MSR_MTRRPHYSBASE4:                  // 0x208
    case BX_MSR_MTRRPHYSMASK4:                  // 0x209
    case BX_MSR_MTRRPHYSBASE5:                  // 0x20a
    case BX_MSR_MTRRPHYSMASK5:                  // 0x20b
    case BX_MSR_MTRRPHYSBASE6:                  // 0x20c
    case BX_MSR_MTRRPHYSMASK6:                  // 0x20d
    case BX_MSR_MTRRPHYSBASE7:                  // 0x20e
    case BX_MSR_MTRRPHYSMASK7:                  // 0x20f
    case BX_MSR_MTRRFIX64K_00000:               // 0x250
    case BX_MSR_MTRRFIX16K_80000:               // 0x258
    case BX_MSR_MTRRFIX16K_A0000:               // 0x259
    case BX_MSR_MTRRFIX4K_C0000:                // 0x268
    case BX_MSR_MTRRFIX4K_C8000:                // 0x269
    case BX_MSR_MTRRFIX4K_D0000:                // 0x26a
    case BX_MSR_MTRRFIX4K_D8000:                // 0x26b
    case BX_MSR_MTRRFIX4K_E0000:                // 0x26c
    case BX_MSR_MTRRFIX4K_E8000:                // 0x26d
    case BX_MSR_MTRRFIX4K_F0000:                // 0x26e
    case BX_MSR_MTRRFIX4K_F8000:                // 0x26f
    case BX_MSR_PAT:                            // 0x277
#endif
#if BX_SUPPORT_VMX
    case BX_MSR_VMX_BASIC:                      // 0x480
    case BX_MSR_VMX_PINBASED_CTRLS:             // 0x481
    case BX_MSR_VMX_PROCBASED_CTRLS:            // 0x482
    case BX_MSR_VMX_VMEXIT_CTRLS:               // 0x483
    case BX_MSR_VMX_VMENTRY_CTRLS:              // 0x484
    case BX_MSR_VMX_MISC:                       // 0x485
    case BX_MSR_VMX_CR0_FIXED0:                 // 0x486
    case BX_MSR_VMX_CR0_FIXED1:                 // 0x487
    case BX_MSR_VMX_CR4_FIXED0:                 // 0x488
    case BX_MSR_VMX_CR4_FIXED1:                 // 0x489
    case BX_MSR_VMX_VMCS_ENUM:                  // 0x48a
    case BX_MSR_VMX_PROCBASED_CTRLS2:           // 0x48b
#if BX_SUPPORT_VMX >= 2
    case BX_MSR_VMX_EPT_VPID_CAP:               // 0x48c
    case BX_MSR_VMX_TRUE_PINBASED_CTRLS:        // 0x48d
    case BX_MSR_VMX_TRUE_PROCBASED_CTRLS:       // 0x48e
    case BX_MSR_VMX_TRUE_VMEXIT_CTRLS:          // 0x48f
    case BX_MSR_VMX_TRUE_VMENTRY_CTRLS:         // 0x490
    case BX_MSR_VMX_VMFUNC:                     // 0x491
#endif
    case BX_MSR_VMX_PROCBASED_CTRLS3:           // 0x492
    case BX_MSR_VMX_VMEXIT_CTRLS2:              // 0x493
#endif
#if BX_SUPPORT_CET
    case BX_MSR_IA32_U_CET:                     // 0x6a0
    case BX_MSR_IA32_S_CET:                     // 0x6a2
    case BX_MSR_IA32_PL0_SSP:                   // 0x6a4
    case BX_MSR_IA32_PL1_SSP:                   // 0x6a5
    case BX_MSR_IA32_PL2_SSP:                   // 0x6a6
    case BX_MSR_IA32_PL3_SSP:                   // 0x6a7
    case BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR:  // 0x6a8
#endif
#if BX_SUPPORT_UINTR
    case BX_MSR_IA32_UINTR_HANDLER:             // 0x986
    case BX_MSR_IA32_UINTR_STACKADJUST:         // 0x987
    case BX_MSR_IA32_UINTR_PD:                  // 0x989
    case BX_MSR_IA32_UINTR_TT:                  // 0x98a
#endif
#if BX_CPU_LEVEL >= 6
    case BX_MSR_XSS:                            // 0xda0
#endif
    case BX_MSR_STAR:                           // 0xc0000081
#if BX_SUPPORT_X86_64
    case BX_MSR_LSTAR:                          // 0xc0000082
    case BX_MSR_CSTAR:                          // 0xc0000083
    case BX_MSR_FSBASE:                         // 0xc0000100
    case BX_MSR_GSBASE:                         // 0xc0000101
    case BX_MSR_KERNELGSBASE:                   // 0xc0000102
#endif
      if (! msr_desciptor->get(&val64)) return false;
      break;

#if BX_SUPPORT_PERFMON
    case BX_MSR_PERFEVTSEL0:
    case BX_MSR_PERFEVTSEL1:
    case BX_MSR_PERFEVTSEL2:
    case BX_MSR_PERFEVTSEL3:
    case BX_MSR_PERFEVTSEL4:
    case BX_MSR_PERFEVTSEL5:
    case BX_MSR_PERFEVTSEL6:
    case BX_MSR_PERFEVTSEL7:
      BX_INFO(("RDMSR: read of MSR_IA32_PERFEVTSEL%d", index - BX_MSR_PERFEVTSEL0));
      return handle_unknown_rdmsr(index, msr);
#endif

    case BX_MSR_IA32_APERF:
    case BX_MSR_IA32_MPERF:
      // IA32_MPERF MSR increments in proportion to a fixed frequency, which is configured when the processor is booted.
      // IA32_APERF MSR increments in proportion to actual performance, while accounting for hardware coordination of P-state and TM1/TM2; or software initiated throttling.
      //     use system (not virtualized) TSC counter
      val64 = BX_CPU_THIS_PTR get_TSC();
      break;

#if BX_CPU_LEVEL >= 6
    case BX_MSR_SYSENTER_CS:
      val64 = BX_CPU_THIS_PTR msr.sysenter_cs_msr;
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_MTRR_DEFTYPE:
      val64 = BX_CPU_THIS_PTR msr.mtrr_deftype;
      break;
#endif

    case BX_MSR_TSC:
      val64 = BX_CPU_THIS_PTR get_Virtual_TSC();
      break;
    case BX_MSR_TSC_ADJUST:
      val64 = BX_CPU_THIS_PTR tsc_adjust;
      break;

#if BX_SUPPORT_MONITOR_MWAIT
    case BX_MSR_IA32_UMWAIT_CONTROL:
      val64 = BX_CPU_THIS_PTR msr.ia32_umwait_ctrl;
      break;
#endif

#if BX_SUPPORT_APIC
    case BX_MSR_APICBASE:
      val64 = BX_CPU_THIS_PTR msr.apicbase;
      BX_DEBUG(("RDMSR: Read %08x:%08x from MSR_APICBASE", GET32H(val64), GET32L(val64)));
      break;
#endif

#if BX_SUPPORT_UINTR
    case BX_MSR_IA32_UINTR_RR:
      val64 = BX_CPU_THIS_PTR uintr.uirr;
      break;
    case BX_MSR_IA32_UINTR_MISC:
      val64 = GET64_FROM_HI32_LO32(BX_CPU_THIS_PTR uintr.uinv, BX_CPU_THIS_PTR uintr.uitt_size);
      break;
#endif

#if BX_SUPPORT_PKEYS
    case BX_MSR_IA32_PKRS:
      val64 = BX_CPU_THIS_PTR pkrs;
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_TSC_DEADLINE:
      val64 = BX_CPU_THIS_PTR lapic->get_tsc_deadline();
      break;
#endif

    // artificial MSR for MRSLIST serialization
    case BX_MSR_IA32_BARRIER:
      val64 = 0;
      break;

    // SCA prevention MSRs
    case BX_MSR_IA32_SPEC_CTRL:
      //    [0] - Enable IBRS: Indirect Branch Restricted Speculation
      //    [1] - Enable STIBP: Single Thread Indirect Branch Predictors
      //    [2] - Enable SSCB: Speculative Store Bypass Disable
      // [63:3] - reserved
#if BX_SUPPORT_VMX >= 2
      if (BX_CPU_THIS_PTR in_vmx_guest && vm->vmexec_ctrls3.VIRTUALIZE_IA32_SPEC_CTRL())
        val64 = vm->ia32_spec_ctrl_shadow;
      else
#endif
        val64 = BX_CPU_THIS_PTR msr.ia32_spec_ctrl;
      break;

    case BX_MSR_IA32_PRED_CMD:
    case BX_MSR_IA32_FLUSH_CMD:
      // write only MSRs
      return false;

#if BX_SUPPORT_VMX
/*
    case BX_MSR_IA32_SMM_MONITOR_CTRL:
      BX_PANIC(("Dual-monitor treatment of SMI and SMM is not implemented"));
      break;
*/
    case BX_MSR_IA32_FEATURE_CONTROL:
      val64 = BX_CPU_THIS_PTR msr.ia32_feature_ctrl;
      break;
#endif

    case BX_MSR_EFER:
      if (! BX_CPU_THIS_PTR efer_suppmask) {
        BX_ERROR(("RDMSR MSR_EFER: EFER MSR is not supported !"));
        return false;
      }
      val64 = BX_CPU_THIS_PTR efer.get32();
      break;

#if BX_SUPPORT_X86_64
    case BX_MSR_FMASK:
      val64 = BX_CPU_THIS_PTR msr.fmask;
      break;

    case BX_MSR_TSC_AUX:
      val64 = BX_CPU_THIS_PTR msr.tsc_aux;   // 32 bit MSR
      break;
#endif

    default:
      BX_PANIC(("RDMSR: missing MSR handling for MSR %08x", index));
      return handle_unknown_rdmsr(index, msr);
    }
  }
  else {
    switch (index) {
#if BX_SUPPORT_SVM
    case BX_SVM_VM_CR_MSR:
      if (! is_cpu_extension_supported(BX_ISA_SVM)) {
        BX_ERROR(("RDMSR SVM_VM_CR_MSR: SVM support not enabled in the cpu model"));
        return false;
      }
      val64 = BX_CPU_THIS_PTR msr.svm_vm_cr;
      break;

    case BX_SVM_HSAVE_PA_MSR:
      if (! is_cpu_extension_supported(BX_ISA_SVM)) {
        BX_ERROR(("RDMSR SVM_HSAVE_PA_MSR: SVM support not enabled in the cpu model"));
        return false;
      }
      val64 = BX_CPU_THIS_PTR msr.svm_hsave_pa;
      break;
#endif

    default:
      return handle_unknown_rdmsr(index, msr);
    }
  }

  BX_DEBUG(("RDMSR: read %08x:%08x from MSR %x", GET32H(val64), GET32L(val64), index));

  *msr = val64;
  return true;
}

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::handle_unknown_rdmsr(Bit32u index, Bit64u *msr)
{
  Bit64u val_64 = 0;

  // Try to check cpuid_t first (can implement some MSRs)
  int result = BX_CPU_THIS_PTR cpuid->rdmsr(index, &val_64);
  if (result == 0)
    return false; // #GP fault due to not supported MSR

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
        return false; // will result in #GP fault due to unknown MSR
    }
  }

  *msr = val_64;
  return true;
}

#endif // BX_CPU_LEVEL >= 5

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDMSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  // CPL is always 0 in real mode
  if (/* !real_mode() && */ CPL!=0) {
    BX_ERROR(("%s: CPL != 0 not in real mode", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u index = ECX;
#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX
  if (i->getIaOpcode() == BX_IA_RDMSR_EqId) index = i->Id();
#endif
  Bit64u val64 = 0;

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_MSR)) SvmInterceptMSR(BX_READ, index);
  }
#endif

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    Bit32u reason = VMX_VMEXIT_RDMSR;
#if BX_SUPPORT_AVX
    if (i->getIaOpcode() == BX_IA_RDMSR_EqId) {
      reason = VMX_VMEXIT_RDMSR_IMM;
    }
#endif
    VMexit_MSR(reason, index, 0);
  }
#endif

  if (!rdmsr(index, &val64))
    exception(BX_GP_EXCEPTION, 0);

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX
  if (i->getIaOpcode() == BX_IA_RDMSR_EqId) {
    BX_WRITE_64BIT_REG(i->dst(), val64);
  }
  else
#endif
  {
    RAX = GET32L(val64);
    RDX = GET32H(val64);
  }
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

bool isValidMSR_IA32_SPEC_CTRL(Bit64u val_64)
{
  //     [0] - Enable IBRS: Indirect Branch Restricted Speculation
  //     [1] - Enable STIBP: Single Thread Indirect Branch Predictors
  //     [2] - Enable SSCB: Speculative Store Bypass Disable
  //     [3] - IPRED_DIS_U: IPRED_DIS control for CPL3
  //     [4] - IPRED_DIS_S: IPRED_DIS control for CPL0-2
  //     [5] - RRSBA_DIS_U: RRSBA_DIS control for CPL3
  //     [6] - RRSBA_DIS_S: RRSBA_DIS control for CPL0-2
  //     [7] - PSFD: Disable Fast Store Forwarding Predictor
  //     [8] - DDPD_U: Disable Data Dependent Prefetcher
  //     [9] - reserved
  //    [10] - BHI_DIS_S: Enables BHI_DIS_S behavior
  // [63:11] - reserved
  if (val_64 & ~(BX_CONST64(0x5FF)))
    return false;

  return true;
}

#if BX_CPU_LEVEL >= 5

//
// - An execution of WRMSR causes a #GP if it would load any of the following MSRs with a non-canonical address:
//      IA32_BNDCFGS, IA32_DS_AREA, IA32_FS_BASE, IA32_GS_BASE,
//      IA32_INTERRUPT_SSP_TABLE_ADDR, IA32_KERNEL_GS_BASE, IA32_LSTAR, IA32_PL0_SSP,
//      IA32_PL1_SSP, IA32_PL2_SSP, IA32_PL3_SSP, IA32_RTIT_ADDR0_A, IA32_RTIT_ADDR0_B,
//      IA32_RTIT_ADDR1_A, IA32_RTIT_ADDR1_B, IA32_RTIT_ADDR2_A, IA32_RTIT_ADDR2_B,
//      IA32_RTIT_ADDR3_A, IA32_RTIT_ADDR3_B, IA32_S_CET, IA32_SYSENTER_EIP, IA32_SYSENTER_ESP,
//      IA32_UINTR_HANDLER, IA32_UINTR_PD, IA32_UINTR_STACKADJUST, IA32_U_CET, and
//      IA32_UINTR_TT
// - An execution of XRSTORS causes a #GP if it would load any of the following MSRs with a non-canonical address:
//      IA32_PL0_SSP, IA32_PL1_SSP, IA32_PL2_SSP, IA32_PL3_SSP, IA32_RTIT_ADDR0_A,
//      IA32_RTIT_ADDR0_B, IA32_RTIT_ADDR1_A, IA32_RTIT_ADDR1_B, IA32_RTIT_ADDR2_A,
//      IA32_RTIT_ADDR2_B, IA32_RTIT_ADDR3_A, IA32_RTIT_ADDR3_B, IA32_U_CET,
//      IA32_UINTR_HANDLER, IA32_UINTR_PD, IA32_UINTR_STACKADJUST, or IA32_UINTR_TT
//
// With a small number of exceptions, this enforcement checks for CPU canonicality and is thus independent of the
// current paging mode. Thus, a processor that supports 5-level paging will allow the instructions mentioned
// above to load these registers with addresses that are 57-bit canonical but not 48-bit canonical, even if 4-level
// paging is active. (As a result, instructions that store these values - SGDT, SIDT, SLDT, STR, RDFSBASE,
// RDGSBASE, RDMSR, XSAVE, XSAVEC, XSAVEOPT, and XSAVES - may save addresses that are 57-bit canonical
// but not 48-bit canonical, even if 4-level paging is active)
//

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::wrmsr(Bit32u index, Bit64u val_64)
{
  Bit32u val32_lo = GET32L(val_64);
  Bit32u val32_hi = GET32H(val_64);

  BX_INSTR_WRMSR(BX_CPU_ID, index, val_64);

  BX_DEBUG(("WRMSR: write %08x:%08x to MSR %x", val32_hi, val32_lo, index));

#if BX_SUPPORT_VMX >= 2
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (vm->vmexec_ctrls2.VIRTUALIZE_X2APIC_MODE()) {
      if (Virtualize_X2APIC_Write(index, val_64))
        return true;
    }
  }
#endif

#if BX_CPU_LEVEL >= 6
  if (is_cpu_extension_supported(BX_ISA_X2APIC)) {
    if (is_x2apic_msr_range(index)) {
      if (x2apic_mode())
        return BX_CPU_THIS_PTR lapic->write_x2apic(x2apic_msr_to_apic_register_index(index), val32_hi, val32_lo);
      else
        return false;
    }
  }
#endif

  if (index < BX_MSR_MAX_INDEX || (index >= 0xc0000000 && index < (0xc0000000 + BX_EXTENDED_MSR_MAX_INDEX))) {
    MSR_Descriptor *msr_desciptor = (index >= 0xc0000000) ? BX_CPU_THIS_PTR ext_msr_desc[index - 0xc0000000] : BX_CPU_THIS_PTR msr_desc[index];
    if (! msr_desciptor) return handle_unknown_wrmsr(index, val_64);
    if (! is_cpu_extension_supported(msr_desciptor->get_cpu_feature())) {
      BX_ERROR(("WRMSR %s: '%s' feature not enabled in the cpu model", msr_desciptor->get_name(), get_cpu_feature_name(msr_desciptor->get_cpu_feature())));
      return false;
    }
    if (! msr_desciptor->is_enabled()) {
      BX_ERROR(("WRMSR %s: not supported by the cpu model", msr_desciptor->get_name()));
      return false;
    }
    if (msr_desciptor->check_reserved_bits_violation(val_64)) {
      if (msr_desciptor->read_only()) {
        BX_ERROR(("WRMSR: %s is read only", msr_desciptor->get_name()));
        return false;
      }
      else {
        BX_ERROR(("WRMSR: attempt to set reserved bits of %s", msr_desciptor->get_name()));
        return false;
      }
    }

    switch(index) {
    // MSRs converted to generic MSR descriptor interface
#if BX_SUPPORT_X86_64
    case BX_MSR_IA32_USER_MSR_CTL:              // 0x01c
#endif
#if BX_CPU_LEVEL >= 6
    case BX_MSR_SYSENTER_ESP:                   // 0x175
    case BX_MSR_SYSENTER_EIP:                   // 0x176
#endif
#if BX_SUPPORT_FRED
    case BX_MSR_IA32_FRED_RSP0:                 // 0x1cc
    case BX_MSR_IA32_FRED_RSP1:                 // 0x1cd
    case BX_MSR_IA32_FRED_RSP2:                 // 0x1ce
    case BX_MSR_IA32_FRED_RSP3:                 // 0x1cf
    case BX_MSR_IA32_FRED_STKLVLS:              // 0x1d0
#if BX_SUPPORT_CET
    case BX_MSR_IA32_FRED_SSP1:                 // 0x1d1
    case BX_MSR_IA32_FRED_SSP2:                 // 0x1d2
    case BX_MSR_IA32_FRED_SSP3:                 // 0x1d3
#endif
    case BX_MSR_IA32_FRED_CONFIG:               // 0x1d4, reserved bits already checked above
#endif
#if BX_CPU_LEVEL >= 6
    case BX_MSR_MTRRPHYSBASE0:                  // 0x200
    case BX_MSR_MTRRPHYSMASK0:                  // 0x201
    case BX_MSR_MTRRPHYSBASE1:                  // 0x202
    case BX_MSR_MTRRPHYSMASK1:                  // 0x203
    case BX_MSR_MTRRPHYSBASE2:                  // 0x204
    case BX_MSR_MTRRPHYSMASK2:                  // 0x205
    case BX_MSR_MTRRPHYSBASE3:                  // 0x206
    case BX_MSR_MTRRPHYSMASK3:                  // 0x207
    case BX_MSR_MTRRPHYSBASE4:                  // 0x208
    case BX_MSR_MTRRPHYSMASK4:                  // 0x209
    case BX_MSR_MTRRPHYSBASE5:                  // 0x20a
    case BX_MSR_MTRRPHYSMASK5:                  // 0x20b
    case BX_MSR_MTRRPHYSBASE6:                  // 0x20c
    case BX_MSR_MTRRPHYSMASK6:                  // 0x20d
    case BX_MSR_MTRRPHYSBASE7:                  // 0x20e
    case BX_MSR_MTRRPHYSMASK7:                  // 0x20f
    case BX_MSR_MTRRFIX64K_00000:               // 0x250
    case BX_MSR_MTRRFIX16K_80000:               // 0x258
    case BX_MSR_MTRRFIX16K_A0000:               // 0x259
    case BX_MSR_MTRRFIX4K_C0000:                // 0x268
    case BX_MSR_MTRRFIX4K_C8000:                // 0x269
    case BX_MSR_MTRRFIX4K_D0000:                // 0x26a
    case BX_MSR_MTRRFIX4K_D8000:                // 0x26b
    case BX_MSR_MTRRFIX4K_E0000:                // 0x26c
    case BX_MSR_MTRRFIX4K_E8000:                // 0x26d
    case BX_MSR_MTRRFIX4K_F0000:                // 0x26e
    case BX_MSR_MTRRFIX4K_F8000:                // 0x26f
    case BX_MSR_PAT:                            // 0x277
#endif
#if BX_SUPPORT_CET
    case BX_MSR_IA32_U_CET:                     // 0x6a0
    case BX_MSR_IA32_S_CET:                     // 0x6a2
    case BX_MSR_IA32_PL0_SSP:                   // 0x6a4
    case BX_MSR_IA32_PL1_SSP:                   // 0x6a5
    case BX_MSR_IA32_PL2_SSP:                   // 0x6a6
    case BX_MSR_IA32_PL3_SSP:                   // 0x6a7
    case BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR:  // 0x6a8
#endif
#if BX_SUPPORT_UINTR
    case BX_MSR_IA32_UINTR_HANDLER:             // 0x986
    case BX_MSR_IA32_UINTR_STACKADJUST:         // 0x987
    case BX_MSR_IA32_UINTR_PD:                  // 0x989, reserved bits already checked above
    case BX_MSR_IA32_UINTR_TT:                  // 0x98a, reserved bits already checked above
#endif
#if BX_CPU_LEVEL >= 6
    case BX_MSR_XSS:                            // 0xda0, reserved bits already checked above
#endif
    case BX_MSR_STAR:                           // 0xc0000081
#if BX_SUPPORT_X86_64
    case BX_MSR_LSTAR:                          // 0xc0000082
    case BX_MSR_CSTAR:                          // 0xc0000083
    case BX_MSR_FSBASE:                         // 0xc0000100
    case BX_MSR_GSBASE:                         // 0xc0000101
    case BX_MSR_KERNELGSBASE:                   // 0xc0000102
#endif
      if (! msr_desciptor->set(val_64)) {
        BX_ERROR(("WRMSR: attempt to write invalid value 0x" FMT_LL "x to %s", val_64, msr_desciptor->get_name()));
        return false;
      }
      break;

#if BX_SUPPORT_PERFMON
    case BX_MSR_PERFEVTSEL0:
    case BX_MSR_PERFEVTSEL1:
    case BX_MSR_PERFEVTSEL2:
    case BX_MSR_PERFEVTSEL3:
    case BX_MSR_PERFEVTSEL4:
    case BX_MSR_PERFEVTSEL5:
    case BX_MSR_PERFEVTSEL6:
    case BX_MSR_PERFEVTSEL7:
      BX_INFO(("WRMSR: write into MSR_IA32_PERFEVTSEL%d: %08x:%08x", index - BX_MSR_PERFEVTSEL0, val32_hi, val32_lo));
      return handle_unknown_wrmsr(index, val_64);
#endif

    case BX_MSR_IA32_APERF:
      BX_INFO(("WRMSR: ignore write into MSR IA32_APERF"));
      break;

    case BX_MSR_IA32_MPERF:
      BX_INFO(("WRMSR: ignore write into MSR IA32_MPERF"));
      break;

#if BX_CPU_LEVEL >= 6
    case BX_MSR_SYSENTER_CS:
      BX_CPU_THIS_PTR msr.sysenter_cs_msr = val32_lo;
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_MTRR_DEFTYPE:
      if (! isMemTypeValidMTRR(val32_lo & 0xFF)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_MTRR_DEFTYPE"));
        return false;
      }
      BX_CPU_THIS_PTR msr.mtrr_deftype = val32_lo;
      break;
#endif

    case BX_MSR_TSC:
      BX_INFO(("WRMSR: write 0x%08x%08x to MSR_TSC", val32_hi, val32_lo));
      BX_CPU_THIS_PTR set_TSC(val_64);
      break;

    case BX_MSR_TSC_ADJUST:
      BX_CPU_THIS_PTR tsc_adjust = (Bit64s) val_64;
      break;

#if BX_SUPPORT_MONITOR_MWAIT
    case BX_MSR_IA32_UMWAIT_CONTROL:
      BX_CPU_THIS_PTR msr.ia32_umwait_ctrl = val32_lo;
      break;
#endif

#if BX_SUPPORT_APIC
    case BX_MSR_APICBASE:
      return relocate_apic(val_64);
#endif

#if BX_SUPPORT_UINTR
    case BX_MSR_IA32_UINTR_RR:
      BX_CPU_THIS_PTR uintr.uirr = val_64;
      uintr_uirr_update(); // potentially signal or clear user-level-interrupt
      break;
    case BX_MSR_IA32_UINTR_MISC: // reserved bits already checked above
      BX_CPU_THIS_PTR uintr.uitt_size = GET32L(val_64);
      BX_CPU_THIS_PTR uintr.uinv      = GET32H(val_64);
      break;
#endif

#if BX_SUPPORT_PKEYS
    case BX_MSR_IA32_PKRS:
      BX_CPU_THIS_PTR pkrs = val_64;
      set_PKeys(BX_CPU_THIS_PTR pkru, BX_CPU_THIS_PTR pkrs);
      break;
#endif

#if BX_CPU_LEVEL >= 6
    case BX_MSR_TSC_DEADLINE:
      BX_CPU_THIS_PTR lapic->set_tsc_deadline(val_64);
      break;
#endif

    // artificial MSR for MRSLIST serialization
    case BX_MSR_IA32_BARRIER:
      return true;

    // SCA prevention MSRs
    case BX_MSR_IA32_SPEC_CTRL:
#if BX_SUPPORT_VMX >= 2
      if (BX_CPU_THIS_PTR in_vmx_guest && vm->vmexec_ctrls3.VIRTUALIZE_IA32_SPEC_CTRL())
        val_64 = (BX_CPU_THIS_PTR msr.ia32_spec_ctrl & vm->ia32_spec_ctrl_mask) | (val_64 & ~vm->ia32_spec_ctrl_mask);
#endif
      if (! isValidMSR_IA32_SPEC_CTRL(val_64)) {
        BX_ERROR(("WRMSR: attempt to set reserved bits of IA32_SPEC_CTRL: 0x" FMT_LL "x", val_64));
        return false;
      }
      BX_CPU_THIS_PTR msr.ia32_spec_ctrl = GET32L(val_64);
      break;

    case BX_MSR_IA32_PRED_CMD: // reserved bits already checked above
    case BX_MSR_IA32_FLUSH_CMD:
      // write only MSR, no need to remember written value
      break;

#if BX_SUPPORT_VMX
    // Support only two bits: lock bit (bit 0) and VMX enable (bit 2)
    case BX_MSR_IA32_FEATURE_CONTROL:
      if (BX_CPU_THIS_PTR msr.ia32_feature_ctrl & 0x1) {
        BX_ERROR(("WRMSR: IA32_FEATURE_CONTROL_MSR VMX lock bit is set !"));
        return false;
      }
      BX_CPU_THIS_PTR msr.ia32_feature_ctrl = val32_lo;
      break;
#endif

    case BX_MSR_EFER:
      if (! SetEFER(val_64)) return false;
      break;

#if BX_SUPPORT_X86_64
    case BX_MSR_FMASK:
      BX_CPU_THIS_PTR msr.fmask = (Bit32u) val_64;
      break;

    case BX_MSR_TSC_AUX:
      BX_CPU_THIS_PTR msr.tsc_aux = val32_lo;
      break;
#endif

    default:
      BX_PANIC(("WRMSR: missing MSR handling for MSR %08x", index));
      return handle_unknown_wrmsr(index, val_64);
    }
  }
  else {
    switch (index) {
#if BX_SUPPORT_SVM
    case BX_SVM_VM_CR_MSR:
      if (! is_cpu_extension_supported(BX_ISA_SVM)) {
        BX_ERROR(("WRMSR SVM_VM_CR_MSR: SVM support not enabled in the cpu model"));
        return false;
      }
      Svm_Update_VM_CR_MSR(val_64);
      break;

    case BX_SVM_HSAVE_PA_MSR:
      if (! is_cpu_extension_supported(BX_ISA_SVM)) {
        BX_ERROR(("WRMSR SVM_HSAVE_PA_MSR: SVM support not enabled in the cpu model"));
        return false;
      }
      if (! IsValidPageAlignedPhyAddr(val_64)) {
        BX_ERROR(("WRMSR SVM_HSAVE_PA_MSR: invalid or not page aligned physical address !"));
        return false;
      }
      BX_CPU_THIS_PTR msr.svm_hsave_pa = val_64;
      break;
#endif

    default:
      return handle_unknown_wrmsr(index, val_64);
    }
  }

  return true;
}

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::handle_unknown_wrmsr(Bit32u index, Bit64u val_64)
{
  // Try to check cpuid_t first (can implement some MSRs)
  int result = BX_CPU_THIS_PTR cpuid->wrmsr(index, val_64);
  if (result == 0)
    return false; // #GP fault due to not supported MSR

  if (result < 0) {
    // cpuid_t have no idea about this MSR
#if BX_CONFIGURE_MSRS
    if (index < BX_MSR_MAX_INDEX && BX_CPU_THIS_PTR msrs[index]) {
      if (! BX_CPU_THIS_PTR msrs[index]->set64(val_64)) {
        BX_ERROR(("WRMSR: Write failed to MSR %#x - #GP fault", index));
        return false;
      }
      return true;
    }
#endif
    // failed to find the MSR, could #GP or ignore it silently
    BX_ERROR(("WRMSR: Unknown register %#x", index));
    if (! BX_CPU_THIS_PTR ignore_bad_msrs)
      return false; // will result in #GP fault due to unknown MSR
  }

  return true;
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

  if (apic_global_enable_on()) {
    Bit32u val32_hi = GET32H(val_64), val32_lo = GET32L(val_64);
    BX_INFO(("WRMSR: wrote %08x:%08x to MSR_APICBASE", val32_hi, val32_lo));
    if (! IsValidPhyAddr(val_64)) {
      BX_ERROR(("relocate_apic: invalid physical address"));
      return false;
    }
    if (val32_lo & BX_MSR_APICBASE_RESERVED_BITS) {
      BX_ERROR(("relocate_apic: attempt to set reserved bits"));
      return false;
    }

#if BX_CPU_LEVEL >= 6
    if (is_cpu_extension_supported(BX_ISA_X2APIC)) {
      unsigned apic_state = (BX_CPU_THIS_PTR msr.apicbase >> 10) & 3;
      unsigned new_state = (val32_lo >> 10) & 3;

      if (new_state != apic_state) {
        if (new_state == BX_APIC_STATE_INVALID) {
          BX_ERROR(("relocate_apic: attempt to set invalid apic state"));
          return false;
        }
        if (apic_state == BX_APIC_X2APIC_MODE && new_state != BX_APIC_GLOBALLY_DISABLED) {
          BX_ERROR(("relocate_apic: attempt to switch from x2apic -> xapic"));
          return false;
        }
      }
    }
#endif

    BX_CPU_THIS_PTR msr.apicbase = (bx_phy_address) val_64;
    BX_CPU_THIS_PTR lapic->set_base(BX_CPU_THIS_PTR msr.apicbase);
    // TLB flush is required for emulation correctness
    TLB_flush();  // don't care about performance of apic relocation
  }
  else {
    BX_INFO(("WRMSR: MSR_APICBASE APIC global enable bit cleared !"));
  }

  return true;
}

bool BX_CPU_C::apic_global_enable_on()
{
  return (BX_CPU_THIS_PTR msr.apicbase & 0x800) != 0;
}

bool BX_CPU_C::x2apic_mode()
{
  return (BX_CPU_THIS_PTR msr.apicbase & 0x400) != 0;
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRMSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  // CPL is always 0 in real mode
  if (/* !real_mode() && */ CPL!=0) {
    BX_ERROR(("%s: CPL != 0 not in real mode", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  invalidate_prefetch_q();

  Bit64u val_64;
  Bit32u index;

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX
  if (i->getIaOpcode() == BX_IA_WRMSRNS_IdEq) {
    val_64 = BX_READ_64BIT_REG(i->src());
    index = i->Id();
  }
  else
#endif
  {
    val_64 = GET64_FROM_HI32_LO32(EDX, EAX);
    index = ECX;
  }

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_MSR)) SvmInterceptMSR(BX_WRITE, index);
  }
#endif

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    Bit32u reason = VMX_VMEXIT_WRMSR;
    Bit32u qualification = 0;
    if (i->getIaOpcode() != BX_IA_WRMSR) {
      reason = VMX_VMEXIT_WRMSRNS;
      qualification = 1; // For WRMSR, the exit qualification is 0, while for WRMSRNS it is 1
    }
    VMexit_MSR(reason, index, qualification);
  }
#endif

  if (! wrmsr(index, val_64))
    exception(BX_GP_EXCEPTION, 0);
#endif

  BX_NEXT_TRACE(i);
}

#if BX_SUPPORT_X86_64

#include "scalar_arith.h" 

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDMSRLIST(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (! BX_CPU_THIS_PTR vmcs.vmexec_ctrls3.ENABLE_MSRLIST())
      exception(BX_UD_EXCEPTION, 0);
  }
#endif

  if (!long64_mode() || CPL!=0) {
    BX_ERROR(("RDMSRLIST: CPL != 0 cause #GP(0)"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (((ESI | EDI) & 0x7) != 0) {
    BX_ERROR(("RDMSRLIST: RSI and RDI must be 8-byte aligned"));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit64u val64;

  while (RCX != 0) {
    unsigned MSR_index = tzcntq(RCX);   // position of least significant bit set in RCX
    Bit64u MSR_mask = (BX_CONST64(1) << MSR_index);
    Bit64u MSR_address = read_linear_qword(BX_SEG_REG_DS, RSI + MSR_index*8);
    if (GET32H(MSR_address)) {
      BX_ERROR(("RDMSRLIST index=%d #GP(0): reserved bits are set in MSR address table entry", MSR_index));
      exception(BX_GP_EXCEPTION, 0);
    }

#if BX_SUPPORT_VMX >= 2
    if (BX_CPU_THIS_PTR in_vmx_guest)
      VMexit_MSR(VMX_VMEXIT_RDMSRLIST, (Bit32u) MSR_address, (Bit32u) MSR_address);
#endif

    if (!rdmsr((Bit32u) MSR_address, &val64))
      exception(BX_GP_EXCEPTION, 0);

    write_linear_qword(BX_SEG_REG_DS, RDI + MSR_index*8, val64);

    RCX &= ~MSR_mask;

    // allow delivery of any pending interrupts or traps
    if (BX_CPU_THIS_PTR async_event) {
      RIP = BX_CPU_THIS_PTR prev_rip; // loop not done, restore RIP
      break;
    }
  }

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRMSRLIST(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (! vm->vmexec_ctrls3.ENABLE_MSRLIST())
      exception(BX_UD_EXCEPTION, 0);
  }
#endif

  if (!long64_mode() || CPL!=0) {
    BX_ERROR(("WRMSRLIST: CPL != 0 cause #GP(0)"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (((ESI | EDI) & 0x7) != 0) {
    BX_ERROR(("WRMSRLIST: RSI and RDI must be 8-byte aligned"));
    exception(BX_GP_EXCEPTION, 0);
  }

  invalidate_prefetch_q();

  while (RCX != 0) {
    unsigned MSR_index = tzcntq(RCX);   // position of least significant bit set in RCX
    Bit64u MSR_mask = (BX_CONST64(1) << MSR_index);
    Bit64u MSR_address = read_linear_qword(BX_SEG_REG_DS, RSI + MSR_index*8);
    if (GET32H(MSR_address)) {
      BX_ERROR(("WRMSRLIST index=%d #GP(0): reserved bits are set in MSR address table entry", MSR_index));
      exception(BX_GP_EXCEPTION, 0);
    }

    Bit64u MSR_data = read_linear_qword(BX_SEG_REG_DS, RDI + MSR_index*8);

#if BX_SUPPORT_VMX >= 2
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      vm->msr_data = MSR_data;
      VMexit_MSR(VMX_VMEXIT_WRMSRLIST, (Bit32u) MSR_address, (Bit32u) MSR_address);
    }
#endif

    if (! wrmsr((Bit32u) MSR_address, MSR_data))
      exception(BX_GP_EXCEPTION, 0);

    RCX &= ~MSR_mask;

    // allow delivery of any pending interrupts or traps
    if (BX_CPU_THIS_PTR async_event) {
      RIP = BX_CPU_THIS_PTR prev_rip; // loop not done, restore RIP
      break;
    }
  }

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::URDMSR(bxInstruction_c *i)
{
  Bit32u index;
#if BX_SUPPORT_AVX
  if (i->getIaOpcode() == BX_IA_URDMSR_EqId) index = i->Id();
  else
#endif
    index = BX_READ_64BIT_REG(i->src());

  if ((BX_CPU_THIS_PTR msr.ia32_user_msr_ctrl & 0x1) == 0) {
    BX_ERROR(("%s: USER_MSR is disabled in IA32_USER_MSR_CTL", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (index > 0x3fff) {
    BX_ERROR(("%s: MSR %x cannot be read by instruction", i->getIaOpcodeNameShort(), index));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit8u access_control = system_read_byte(LPFOf(BX_CPU_THIS_PTR msr.ia32_user_msr_ctrl) + (index >> 3));
  if (access_control & (1 << (index & 7)))
  {
#if BX_SUPPORT_VMX
    if (BX_CPU_THIS_PTR in_vmx_guest)
      VMexit_MSR(VMX_VMEXIT_URDMSR, index, 0);
#endif

    Bit64u val_64 = 0;
    if (!rdmsr(index, &val_64))
      exception(BX_GP_EXCEPTION, 0);
    BX_WRITE_64BIT_REG(i->dst(), val_64);
  }
  else {
    BX_ERROR(("%s: MSR %x cannot be read by instruction", i->getIaOpcodeNameShort(), index));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::UWRMSR(bxInstruction_c *i)
{
  Bit32u index;
#if BX_SUPPORT_AVX
  if (i->getIaOpcode() == BX_IA_UWRMSR_IdEq) index = i->Id();
  else
#endif
    index = BX_READ_64BIT_REG(i->dst());

  if ((BX_CPU_THIS_PTR msr.ia32_user_msr_ctrl & 0x1) == 0) {
    BX_ERROR(("%s: USER_MSR is disabled in IA32_USER_MSR_CTL", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (index > 0x3fff) {
    BX_ERROR(("%s: MSR %x cannot be written by instruction", i->getIaOpcodeNameShort(), index));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit8u access_control = system_read_byte(LPFOf(BX_CPU_THIS_PTR msr.ia32_user_msr_ctrl) + (index >> 3) + 2048);
  if (access_control & (1 << (index & 7)))
  {
#if BX_SUPPORT_VMX
    if (BX_CPU_THIS_PTR in_vmx_guest)
      VMexit_MSR(VMX_VMEXIT_UWRMSR, index, 0);
#endif

    Bit64u val_64 = BX_READ_64BIT_REG(i->src());
    if (!wrmsr(index, val_64))
      exception(BX_GP_EXCEPTION, 0);
  }
  else {
    BX_ERROR(("%s: MSR %x cannot be written by instruction", i->getIaOpcodeNameShort(), index));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_NEXT_INSTR(i);
}

#endif

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

      BX_CPU_THIS_PTR msrs[index] = new MSR(this, index, type,
        GET64_FROM_HI32_LO32(reset_hi, reset_lo),
        GET64_FROM_HI32_LO32(rsrv_hi, rsrv_lo),
        GET64_FROM_HI32_LO32(ignr_hi, ignr_lo));
    }
  } while (!feof(fd));

  fclose(fd);
  return retval;
}

#endif
