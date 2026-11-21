/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2017-2026 Stanislav Shwartsman
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

#include "bochs.h"
#include "cpu.h"
#include "corei7_skylake-x.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX

// CPUID dump of the real CPU (taken from corei7_skylake-x.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000016, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x00050654, 0x00100800, 0x7FFEFBBF, 0xBFEBFBFF },
  /* leaf 0x00000002 */ { 0x76036301, 0x00F0B5FF, 0x00000000, 0x00C30000 },
  /* leaf 0x00000003 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000004 */ { 0x1C004121, 0x01C0003F, 0x0000003F, 0x00000000 }, // [SL 00]
  /* leaf 0x00000004 */ { 0x1C004122, 0x01C0003F, 0x0000003F, 0x00000000 }, // [SL 01]
  /* leaf 0x00000004 */ { 0x1C004143, 0x03C0003F, 0x000003FF, 0x00000000 }, // [SL 02]
  /* leaf 0x00000004 */ { 0x1C03C163, 0x0280003F, 0x00002FFF, 0x00000004 }, // [SL 03]
  /* leaf 0x00000005 */ { 0x00000040, 0x00000040, 0x00000003, 0x00002020 },
  /* leaf 0x00000006 */ { 0x00000075, 0x00000002, 0x00000009, 0x00000000 },
  /* leaf 0x00000007 */ { 0x00000000, 0xD39FFFFB, 0x00000000, 0x00000000 },
  /* leaf 0x00000008 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000A */ { 0x07300404, 0x00000000, 0x00000000, 0x00000603 },
  /* leaf 0x0000000B */ { 0x00000001, 0x00000002, 0x00000100, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000B */ { 0x00000004, 0x0000000C, 0x00000201, 0x00000000 }, // [SL 01]
  /* leaf 0x0000000C */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000D */ { 0x000000FF, 0x00000A80, 0x00000A80, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000D */ { 0x0000000F, 0x00000A00, 0x00000100, 0x00000000 }, // [SL 01]
  /* leaf 0x0000000D */ { 0x00000100, 0x00000240, 0x00000000, 0x00000000 }, // [SL 02]
  /* leaf 0x0000000D */ { 0x00000040, 0x000003C0, 0x00000000, 0x00000000 }, // [SL 03]
  /* leaf 0x0000000D */ { 0x00000040, 0x00000400, 0x00000000, 0x00000000 }, // [SL 04]
  /* leaf 0x0000000D */ { 0x00000040, 0x00000440, 0x00000000, 0x00000000 }, // [SL 05]
  /* leaf 0x0000000D */ { 0x00000200, 0x00000480, 0x00000000, 0x00000000 }, // [SL 06]
  /* leaf 0x0000000D */ { 0x00000400, 0x00000680, 0x00000000, 0x00000000 }, // [SL 07]
  /* leaf 0x0000000D */ { 0x00000080, 0x00000000, 0x00000001, 0x00000000 }, // [SL 08]
  /* leaf 0x0000000E */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000F */ { 0x00000000, 0x0000002F, 0x00000000, 0x00000002 }, // [SL 00]
  /* leaf 0x0000000F */ { 0x00000000, 0x00006000, 0x0000002F, 0x00000007 }, // [SL 01]
  /* leaf 0x00000010 */ { 0x00000000, 0x0000000A, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000010 */ { 0x0000000A, 0x00000600, 0x00000004, 0x0000000F }, // [SL 01]
  /* leaf 0x00000011 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000013 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000014 */ { 0x00000001, 0x0000000F, 0x00000007, 0x00000000 }, // [SL 00]
  /* leaf 0x00000014 */ { 0x02490002, 0x003F3FFF, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000015 */ { 0x00000002, 0x00000124, 0x00000000, 0x00000000 },
  /* leaf 0x00000016 */ { 0x00000DAC, 0x00000FA0, 0x00000064, 0x00000000 }
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000008, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000000, 0x00000000, 0x00000121, 0x2C100000 },
  /* leaf 0x80000002 */ { 0x65746E49, 0x2952286C, 0x726F4320, 0x4D542865 },
  /* leaf 0x80000003 */ { 0x37692029, 0x3038372D, 0x43205830, 0x40205550 },
  /* leaf 0x80000004 */ { 0x352E3320, 0x7A484730, 0x00000000, 0x00000000 },
  /* leaf 0x80000005 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000006 */ { 0x00000000, 0x00000000, 0x01006040, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000100 },
  /* leaf 0x80000008 */ { 0x0000302E, 0x00000000, 0x00000000, 0x00000000 }
};

corei7_skylake_x_t::corei7_skylake_x_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x16, 0x80000008);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs +  0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs +  1);
  cpuid_table->register_leaf(0x00000002, 1, std_leafs +  2);
  cpuid_table->register_leaf(0x00000003, 1, std_leafs +  3);
  cpuid_table->register_leaf(0x00000004, 4, std_leafs +  4);
  cpuid_table->register_leaf(0x00000005, 1, std_leafs +  8);
  cpuid_table->register_leaf(0x00000006, 1, std_leafs +  9);
  cpuid_table->register_leaf(0x00000007, 1, std_leafs + 10);
  cpuid_table->register_leaf(0x00000008, 1, std_leafs + 11);
  cpuid_table->register_leaf(0x00000009, 1, std_leafs + 12);
  cpuid_table->register_leaf(0x0000000A, 1, std_leafs + 13);
  cpuid_table->register_leaf(0x0000000B, 2, std_leafs + 14);
  cpuid_table->register_leaf(0x0000000C, 1, std_leafs + 16);
  cpuid_table->register_leaf(0x0000000D, 9, std_leafs + 17);
  cpuid_table->register_leaf(0x0000000E, 1, std_leafs + 26);
  cpuid_table->register_leaf(0x0000000F, 2, std_leafs + 27);
  cpuid_table->register_leaf(0x00000010, 2, std_leafs + 29);
  cpuid_table->register_leaf(0x00000011, 1, std_leafs + 31);
  cpuid_table->register_leaf(0x00000012, 2, std_leafs + 32);
  cpuid_table->register_leaf(0x00000013, 1, std_leafs + 34);
  cpuid_table->register_leaf(0x00000014, 2, std_leafs + 35);
  cpuid_table->register_leaf(0x00000015, 1, std_leafs + 37);
  cpuid_table->register_leaf(0x00000016, 1, std_leafs + 38);

  cpuid_table->register_leaf(0x80000000, 1, ext_leafs + 0);
  cpuid_table->register_leaf(0x80000001, 1, ext_leafs + 1);
  cpuid_table->register_leaf(0x80000002, 1, ext_leafs + 2);
  cpuid_table->register_leaf(0x80000003, 1, ext_leafs + 3);
  cpuid_table->register_leaf(0x80000004, 1, ext_leafs + 4);
  cpuid_table->register_leaf(0x80000005, 1, ext_leafs + 5);
  cpuid_table->register_leaf(0x80000006, 1, ext_leafs + 6);
  cpuid_table->register_leaf(0x80000007, 1, ext_leafs + 7);
  cpuid_table->register_leaf(0x80000008, 1, ext_leafs + 8);

  if (BX_SUPPORT_VMX == 1)
    BX_INFO(("You must compile with --enable-vmx=2 for Intel Core i7 Skylake-X VMX configuration"));

  if (! BX_SUPPORT_MONITOR_MWAIT)
    BX_INFO(("WARNING: MONITOR/MWAIT support is not compiled in !"));

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT_LONGMODE);
  enable_cpu_extension(BX_ISA_CLFLUSH);
  enable_cpu_extension(BX_ISA_DEBUG_EXTENSIONS);
  enable_cpu_extension(BX_ISA_VME);
  enable_cpu_extension(BX_ISA_PSE);
  enable_cpu_extension(BX_ISA_PAE);
  enable_cpu_extension(BX_ISA_PGE);
  enable_cpu_extension(BX_ISA_MTRR);
  enable_cpu_extension(BX_ISA_PAT);
  enable_cpu_extension(BX_ISA_XAPIC);
  enable_cpu_extension(BX_ISA_X2APIC);
  enable_cpu_extension(BX_ISA_LONG_MODE);
  enable_cpu_extension(BX_ISA_LM_LAHF_SAHF);
  enable_cpu_extension(BX_ISA_CMPXCHG16B);
  enable_cpu_extension(BX_ISA_NX);
  enable_cpu_extension(BX_ISA_1G_PAGES);
  enable_cpu_extension(BX_ISA_PCID);
  enable_cpu_extension(BX_ISA_TSC_ADJUST);
  enable_cpu_extension(BX_ISA_TSC_DEADLINE);
  enable_cpu_extension(BX_ISA_SSE);
  enable_cpu_extension(BX_ISA_SSE2);
  enable_cpu_extension(BX_ISA_SSE3);
  enable_cpu_extension(BX_ISA_SSSE3);
  enable_cpu_extension(BX_ISA_SSE4_1);
  enable_cpu_extension(BX_ISA_SSE4_2);
  enable_cpu_extension(BX_ISA_POPCNT);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_MONITOR_MWAIT);
#endif
#if BX_SUPPORT_VMX >= 2
  enable_cpu_extension(BX_ISA_VMX);
#endif
  enable_cpu_extension(BX_ISA_RDTSCP);
  enable_cpu_extension(BX_ISA_XSAVE);
  enable_cpu_extension(BX_ISA_XSAVEOPT);
  enable_cpu_extension(BX_ISA_XSAVEC);
  enable_cpu_extension(BX_ISA_XSAVES);
  enable_cpu_extension(BX_ISA_AES_PCLMULQDQ);
  enable_cpu_extension(BX_ISA_MOVBE);
  enable_cpu_extension(BX_ISA_AVX);
  enable_cpu_extension(BX_ISA_AVX_F16C);
  enable_cpu_extension(BX_ISA_AVX2);
  enable_cpu_extension(BX_ISA_AVX_FMA);
  enable_cpu_extension(BX_ISA_LZCNT);
  enable_cpu_extension(BX_ISA_BMI1);
  enable_cpu_extension(BX_ISA_BMI2);
  enable_cpu_extension(BX_ISA_FSGSBASE);
  enable_cpu_extension(BX_ISA_INVPCID);
  enable_cpu_extension(BX_ISA_SMEP);
  enable_cpu_extension(BX_ISA_RDRAND);
  enable_cpu_extension(BX_ISA_FCS_FDS_DEPRECATION);
  enable_cpu_extension(BX_ISA_RDSEED);
  enable_cpu_extension(BX_ISA_ADX);
  enable_cpu_extension(BX_ISA_SMAP);
  enable_cpu_extension(BX_ISA_FDP_DEPRECATION);
#if BX_SUPPORT_EVEX
  enable_cpu_extension(BX_ISA_AVX512);
  enable_cpu_extension(BX_ISA_AVX512_DQ);
  enable_cpu_extension(BX_ISA_AVX512_CD);
  enable_cpu_extension(BX_ISA_AVX512_BW);
#endif
  enable_cpu_extension(BX_ISA_CLFLUSHOPT);
  enable_cpu_extension(BX_ISA_CLWB);
}

unsigned corei7_skylake_x_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  return cpuid_table->get_leaf(function, subfunction, leaf);
}

#if BX_SUPPORT_VMX >= 2

// MSR 00000480: 00DA-0400-0000-0004	BX_MSR_VMX_BASIC
// MSR 00000481: 0000-00FF-0000-0016	BX_MSR_VMX_PINBASED_CTRLS
// MSR 00000482: FFF9-FFFE-0401-E172	BX_MSR_VMX_PROCBASED_CTRLS
// MSR 00000483: 01FF-FFFF-0003-6DFF	BX_MSR_VMX_VMEXIT_CTRLS
// MSR 00000484: 0003-FFFF-0000-11FF	BX_MSR_VMX_VMENTRY_CTRLS
// MSR 00000485: 0000-0000-7004-C1E7	BX_MSR_VMX_MISC
// MSR 00000486: 0000-0000-8000-0021	BX_MSR_VMX_CR0_FIXED0
// MSR 00000487: 0000-0000-FFFF-FFFF	BX_MSR_VMX_CR0_FIXED1
// MSR 00000488: 0000-0000-0000-2000	BX_MSR_VMX_CR4_FIXED0
// MSR 00000489: 0000-0000-0037-27FF	BX_MSR_VMX_CR4_FIXED1
// MSR 0000048A: 0000-0000-0000-002E	BX_MSR_VMX_VMCS_ENUM
// MSR 0000048B: 025D-3FFF-0000-0000	BX_MSR_VMX_PROCBASED_CTRLS2
// MSR 0000048C: 0000-0F01-0673-4141	BX_MSR_VMX_MSR_VMX_EPT_VPID_CAP
// MSR 0000048D: 0000-00FF-0000-0016	BX_MSR_VMX_TRUE_PINBASED_CTRLS
// MSR 0000048E: FFF9-FFFE-0400-6172    BX_MSR_VMX_TRUE_PROCBASED_CTRLS
// MSR 0000048F: 01FF-FFFF-0003-6DFB	BX_MSR_VMX_TRUE_VMEXIT_CTRLS
// MSR 00000490: 0003-FFFF-0000-11FB	BX_MSR_VMX_TRUE_VMENTRY_CTRLS

Bit32u corei7_skylake_x_t::get_vmx_extensions_bitmask(void) const
{
  return BX_VMX_TPR_SHADOW |
         BX_VMX_VIRTUAL_NMI |
         BX_VMX_APIC_VIRTUALIZATION |
         BX_VMX_WBINVD_VMEXIT |
         BX_VMX_MONITOR_TRAP_FLAG |
         BX_VMX_VPID |
         BX_VMX_EPT |
         BX_VMX_UNRESTRICTED_GUEST |
         BX_VMX_SAVE_DEBUGCTL_DISABLE |
         BX_VMX_PERF_GLOBAL_CTRL |     // MSR not implemented yet
         BX_VMX_PAT |
         BX_VMX_EFER |
         BX_VMX_DESCRIPTOR_TABLE_EXIT |
         BX_VMX_X2APIC_VIRTUALIZATION |
         BX_VMX_PREEMPTION_TIMER |
         BX_VMX_PAUSE_LOOP_EXITING |
         BX_VMX_EPTP_SWITCHING |
         BX_VMX_EPT_ACCESS_DIRTY |
         BX_VMX_VINTR_DELIVERY |
         BX_VMX_VMCS_SHADOWING |
         BX_VMX_EPT_EXCEPTION |
         BX_VMX_SW_INTERRUPT_INJECTION_ILEN_0 |
         BX_VMX_PML |
         BX_VMX_POSTED_INTERRUPTS |
         BX_VMX_MBE_CONTROL |
         BX_VMX_TSC_SCALING;
}

#endif

bx_cpuid_t *create_corei7_skylake_x_cpuid(BX_CPU_C *cpu) { return new corei7_skylake_x_t(cpu); }

#endif
