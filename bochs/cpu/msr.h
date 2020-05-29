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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_MSR_H
#define BX_MSR_H

enum MSR_Register {
  BX_MSR_TSC            = 0x010,
  BX_MSR_APICBASE       = 0x01b,
  BX_MSR_TSC_ADJUST     = 0x03b,
  BX_MSR_TSC_DEADLINE   = 0x6E0,

  BX_MSR_IA32_SPEC_CTRL         = 0x048,
  BX_MSR_IA32_PRED_CMD          = 0x049,
  BX_MSR_IA32_ARCH_CAPABILITIES = 0x10a,
  BX_MSR_IA32_FLUSH_CMD         = 0x10b,

#if BX_CPU_LEVEL >= 6
  BX_MSR_SYSENTER_CS  = 0x174,
  BX_MSR_SYSENTER_ESP = 0x175,
  BX_MSR_SYSENTER_EIP = 0x176,
#endif

  BX_MSR_DEBUGCTLMSR      = 0x1d9,
  BX_MSR_LASTBRANCHFROMIP = 0x1db,
  BX_MSR_LASTBRANCHTOIP   = 0x1dc,
  BX_MSR_LASTINTOIP       = 0x1dd,

#if BX_CPU_LEVEL >= 6
  BX_MSR_MTRRCAP          = 0x0fe,
  BX_MSR_MTRRPHYSBASE0    = 0x200,
  BX_MSR_MTRRPHYSMASK0    = 0x201,
  BX_MSR_MTRRPHYSBASE1    = 0x202,
  BX_MSR_MTRRPHYSMASK1    = 0x203,
  BX_MSR_MTRRPHYSBASE2    = 0x204,
  BX_MSR_MTRRPHYSMASK2    = 0x205,
  BX_MSR_MTRRPHYSBASE3    = 0x206,
  BX_MSR_MTRRPHYSMASK3    = 0x207,
  BX_MSR_MTRRPHYSBASE4    = 0x208,
  BX_MSR_MTRRPHYSMASK4    = 0x209,
  BX_MSR_MTRRPHYSBASE5    = 0x20a,
  BX_MSR_MTRRPHYSMASK5    = 0x20b,
  BX_MSR_MTRRPHYSBASE6    = 0x20c,
  BX_MSR_MTRRPHYSMASK6    = 0x20d,
  BX_MSR_MTRRPHYSBASE7    = 0x20e,
  BX_MSR_MTRRPHYSMASK7    = 0x20f,
  BX_MSR_MTRRFIX64K_00000 = 0x250,
  BX_MSR_MTRRFIX16K_80000 = 0x258,
  BX_MSR_MTRRFIX16K_A0000 = 0x259,
  BX_MSR_MTRRFIX4K_C0000  = 0x268,
  BX_MSR_MTRRFIX4K_C8000  = 0x269,
  BX_MSR_MTRRFIX4K_D0000  = 0x26a,
  BX_MSR_MTRRFIX4K_D8000  = 0x26b,
  BX_MSR_MTRRFIX4K_E0000  = 0x26c,
  BX_MSR_MTRRFIX4K_E8000  = 0x26d,
  BX_MSR_MTRRFIX4K_F0000  = 0x26e,
  BX_MSR_MTRRFIX4K_F8000  = 0x26f,
  BX_MSR_PAT              = 0x277,
  BX_MSR_MTRR_DEFTYPE     = 0x2ff,
#endif

#if BX_SUPPORT_PERFMON
  BX_MSR_PMC0             = 0x0c1,  /* PERFCTR0 */
  BX_MSR_PMC1             = 0x0c2,  /* PERFCTR1 */
  BX_MSR_PMC2             = 0x0c3,
  BX_MSR_PMC3             = 0x0c4,
  BX_MSR_PMC4             = 0x0c5,
  BX_MSR_PMC5             = 0x0c6,
  BX_MSR_PMC6             = 0x0c7,
  BX_MSR_PMC7             = 0x0c8,
  BX_MSR_PERFEVTSEL0      = 0x186,
  BX_MSR_PERFEVTSEL1      = 0x187,
  BX_MSR_PERFEVTSEL2      = 0x188,
  BX_MSR_PERFEVTSEL3      = 0x189,
  BX_MSR_PERFEVTSEL4      = 0x18a,
  BX_MSR_PERFEVTSEL5      = 0x18b, 
  BX_MSR_PERFEVTSEL6      = 0x18c,
  BX_MSR_PERFEVTSEL7      = 0x18d,
  BX_MSR_PERF_FIXED_CTR0  = 0x309,  /* Fixed Performance Counter 0 (R/W): Counts Instr_Retired.Any */
  BX_MSR_PERF_FIXED_CTR1  = 0x30a,  /* Fixed Performance Counter 1 (R/W): Counts CPU_CLK_Unhalted.Core */
  BX_MSR_PERF_FIXED_CTR2  = 0x30b,  /* Fixed Performance Counter 2 (R/W): Counts CPU_CLK_Unhalted.Ref */
  BX_MSR_FIXED_CTR_CTRL   = 0x38d,  /* Fixed Performance Counter Control (R/W) */
  BX_MSR_PERF_GLOBAL_CTRL = 0x38f,  /* Global Performance Counter Control */
#endif

#if BX_SUPPORT_VMX
  BX_MSR_VMX_BASIC                = 0x480,
  BX_MSR_VMX_PINBASED_CTRLS       = 0x481,
  BX_MSR_VMX_PROCBASED_CTRLS      = 0x482,
  BX_MSR_VMX_VMEXIT_CTRLS         = 0x483,
  BX_MSR_VMX_VMENTRY_CTRLS        = 0x484,
  BX_MSR_VMX_MISC                 = 0x485,
  BX_MSR_VMX_CR0_FIXED0           = 0x486,
  BX_MSR_VMX_CR0_FIXED1           = 0x487,
  BX_MSR_VMX_CR4_FIXED0           = 0x488,
  BX_MSR_VMX_CR4_FIXED1           = 0x489,
  BX_MSR_VMX_VMCS_ENUM            = 0x48a,
  BX_MSR_VMX_PROCBASED_CTRLS2     = 0x48b,
  BX_MSR_VMX_EPT_VPID_CAP         = 0x48c,
  BX_MSR_VMX_TRUE_PINBASED_CTRLS  = 0x48d,
  BX_MSR_VMX_TRUE_PROCBASED_CTRLS = 0x48e,
  BX_MSR_VMX_TRUE_VMEXIT_CTRLS    = 0x48f,
  BX_MSR_VMX_TRUE_VMENTRY_CTRLS   = 0x490,
  BX_MSR_VMX_VMFUNC               = 0x491,
  BX_MSR_IA32_FEATURE_CONTROL     = 0x03A,
  BX_MSR_IA32_SMM_MONITOR_CTL     = 0x09B,
#endif

  /* Shadow Stack */
  BX_MSR_IA32_U_CET = 0x6A0,
  BX_MSR_IA32_S_CET = 0x6A2,

  BX_MSR_IA32_PL0_SSP = 0x6A4,
  BX_MSR_IA32_PL1_SSP = 0x6A5,
  BX_MSR_IA32_PL2_SSP = 0x6A6,
  BX_MSR_IA32_PL3_SSP = 0x6A7,

  BX_MSR_IA32_INTERRUPT_SSP_TABLE_ADDR = 0x6A8,

  BX_MSR_IA32_PKRS = 0x6E1,

  BX_MSR_XSS = 0xda0,

  /* AMD MSRs */
  BX_MSR_EFER         = 0xc0000080,
  BX_MSR_STAR         = 0xc0000081,
  BX_MSR_LSTAR        = 0xc0000082,
  BX_MSR_CSTAR        = 0xc0000083,
  BX_MSR_FMASK        = 0xc0000084,
  BX_MSR_FSBASE       = 0xc0000100,
  BX_MSR_GSBASE       = 0xc0000101,
  BX_MSR_KERNELGSBASE = 0xc0000102,
  BX_MSR_TSC_AUX      = 0xc0000103,

  BX_SVM_VM_CR_MSR     = 0xc0010114,
  BX_SVM_IGNNE_MSR     = 0xc0010115,
  BX_SVM_SMM_CTL_MSR   = 0xc0010116,
  BX_SVM_HSAVE_PA_MSR  = 0xc0010117,
};

const unsigned BX_NUM_VARIABLE_RANGE_MTRRS = 8;

#endif  // #ifndef BX_MSR_H
