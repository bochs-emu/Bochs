/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2018-2026 Stanislav Shwartsman
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
#include "corei3_cnl.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX

// CPUID dump of the real CPU (taken from corei3_cnl.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000016, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x00060663, 0x00100800, 0x7FFAFBBF, 0xBFEBFBFF },
  /* leaf 0x00000002 */ { 0x76036301, 0x00F0B5FF, 0x00000000, 0x00C30000 },
  /* leaf 0x00000003 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000004 */ { 0x1C004121, 0x01C0003F, 0x0000003F, 0x00000000 }, // [SL 00]
  /* leaf 0x00000004 */ { 0x1C004122, 0x01C0003F, 0x0000003F, 0x00000000 }, // [SL 01]
  /* leaf 0x00000004 */ { 0x1C004143, 0x00C0003F, 0x000003FF, 0x00000000 }, // [SL 02]
  /* leaf 0x00000004 */ { 0x1C03C163, 0x03C0003F, 0x00000FFF, 0x00000006 }, // [SL 03]
  /* leaf 0x00000005 */ { 0x00000040, 0x00000040, 0x00000003, 0x11142020 },
  /* leaf 0x00000006 */ { 0x00002FF7, 0x00000002, 0x00000009, 0x00000000 },
  /* leaf 0x00000007 */ { 0x00000000, 0xF2BF67EB, 0x0000000E, 0x9C000000 },
  /* leaf 0x00000008 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000A */ { 0x07300404, 0x00000000, 0x00000000, 0x00000603 },
  /* leaf 0x0000000B */ { 0x00000001, 0x00000002, 0x00000100, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000B */ { 0x00000004, 0x00000004, 0x00000201, 0x00000000 }, // [SL 01]
  /* leaf 0x0000000C */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000D */ { 0x000002FF, 0x00000A80, 0x00000A88, 0x00000000 }, // [SL 00] [x87]
  /* leaf 0x0000000D */ { 0x0000000F, 0x00000A80, 0x00000100, 0x00000000 }, // [SL 01] [SSE]
  /* leaf 0x0000000D */ { 0x00000100, 0x00000240, 0x00000000, 0x00000000 }, // [SL 02] [AVX]
  /* leaf 0x0000000D */ { 0x00000040, 0x000003C0, 0x00000000, 0x00000000 }, // [SL 03] [MPX BNDREGS]
  /* leaf 0x0000000D */ { 0x00000040, 0x00000400, 0x00000000, 0x00000000 }, // [SL 04] [MPX BNDCSR]
  /* leaf 0x0000000D */ { 0x00000040, 0x00000440, 0x00000000, 0x00000000 }, // [SL 05] [AVX-512 Opmask]
  /* leaf 0x0000000D */ { 0x00000200, 0x00000480, 0x00000000, 0x00000000 }, // [SL 06] [AVX-512 Hi256]
  /* leaf 0x0000000D */ { 0x00000400, 0x00000680, 0x00000000, 0x00000000 }, // [SL 07] [AVX-512 Hi16]
  /* leaf 0x0000000D */ { 0x00000080, 0x00000000, 0x00000001, 0x00000000 }, // [SL 08] [PT]
  /* leaf 0x0000000D */ { 0x00000008, 0x00000A80, 0x00000000, 0x00000000 }, // [SL 09] [PKRU]
  /* leaf 0x0000000E */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000F */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000F */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000010 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000010 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000011 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000013 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000014 */ { 0x00000001, 0x0000000F, 0x00000007, 0x00000000 }, // [SL 00]
  /* leaf 0x00000014 */ { 0x02490002, 0x003F1FFF, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000015 */ { 0x00000002, 0x000000B8, 0x016E3600, 0x00000000 },
  /* leaf 0x00000016 */ { 0x00000898, 0x00000C80, 0x00000064, 0x00000000 }
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000008, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000000, 0x00000000, 0x00000121, 0x2C100000 },
  /* leaf 0x80000002 */ { 0x65746E49, 0x2952286C, 0x726F4320, 0x4D542865 },
  /* leaf 0x80000003 */ { 0x33692029, 0x3231382D, 0x43205531, 0x40205550 },
  /* leaf 0x80000004 */ { 0x322E3220, 0x7A484730, 0x00000000, 0x00000000 },
  /* leaf 0x80000005 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000006 */ { 0x00000000, 0x00000000, 0x01006040, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000100 },
  /* leaf 0x80000008 */ { 0x00003027, 0x00000000, 0x00000000, 0x00000000 }
};

corei3_cnl_t::corei3_cnl_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x16, 0x80000008);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000,  1, std_leafs +  0);
  cpuid_table->register_leaf(0x00000001,  1, std_leafs +  1);
  cpuid_table->register_leaf(0x00000002,  1, std_leafs +  2);
  cpuid_table->register_leaf(0x00000003,  1, std_leafs +  3);
  cpuid_table->register_leaf(0x00000004,  4, std_leafs +  4);
  cpuid_table->register_leaf(0x00000005,  1, std_leafs +  8);
  cpuid_table->register_leaf(0x00000006,  1, std_leafs +  9);
  cpuid_table->register_leaf(0x00000007,  1, std_leafs + 10);
  cpuid_table->register_leaf(0x00000008,  1, std_leafs + 11);
  cpuid_table->register_leaf(0x00000009,  1, std_leafs + 12);
  cpuid_table->register_leaf(0x0000000A,  1, std_leafs + 13);
  cpuid_table->register_leaf(0x0000000B,  2, std_leafs + 14);
  cpuid_table->register_leaf(0x0000000C,  1, std_leafs + 16);
  cpuid_table->register_leaf(0x0000000D, 10, std_leafs + 17);
  cpuid_table->register_leaf(0x0000000E,  1, std_leafs + 27);
  cpuid_table->register_leaf(0x0000000F,  2, std_leafs + 28);
  cpuid_table->register_leaf(0x00000010,  2, std_leafs + 30);
  cpuid_table->register_leaf(0x00000011,  1, std_leafs + 32);
  cpuid_table->register_leaf(0x00000012,  2, std_leafs + 33);
  cpuid_table->register_leaf(0x00000013,  1, std_leafs + 35);
  cpuid_table->register_leaf(0x00000014,  2, std_leafs + 36);
  cpuid_table->register_leaf(0x00000015,  1, std_leafs + 38);
  cpuid_table->register_leaf(0x00000016,  1, std_leafs + 39);

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
    BX_INFO(("You must compile with --enable-vmx=2 for Intel Core i3 Cannonlake VMX configuration"));

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
  enable_cpu_extension(BX_ISA_SHA);
#if BX_SUPPORT_EVEX
  enable_cpu_extension(BX_ISA_AVX512);
  enable_cpu_extension(BX_ISA_AVX512_DQ);
  enable_cpu_extension(BX_ISA_AVX512_CD);
  enable_cpu_extension(BX_ISA_AVX512_BW);
  enable_cpu_extension(BX_ISA_AVX512_IFMA52);
  enable_cpu_extension(BX_ISA_AVX512_VBMI);
#endif
  enable_cpu_extension(BX_ISA_CLFLUSHOPT);
  enable_cpu_extension(BX_ISA_UMIP);
#if BX_SUPPORT_PKEYS
  enable_cpu_extension(BX_ISA_PKU);
#endif
#if BX_SUPPORT_MPX
  enable_cpu_extension(BX_ISA_MPX);
#endif
}

void corei3_cnl_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  switch(function) {
  case 0x80000000:
  case 0x80000001:
  case 0x80000002:
  case 0x80000003:
  case 0x80000004:
  case 0x80000005:
  case 0x80000006: // CPUID leaf 0x80000006 - L2 Cache and TLB Identifiers
  case 0x80000007: // CPUID leaf 0x80000007 - Advanced Power Management leaf, EDX[8] - invariant TSC
  case 0x80000008: // CPUID leaf 0x80000008 - Address Sizes and Extended Feature Identifiers
    cpuid_table->get_ext_leaf(function, subfunction, leaf);
    return;
  case 0x00000000:
    cpuid_table->get_std_leaf(function, subfunction, leaf);
    return;
  case 0x00000001:
    get_std_cpuid_leaf_1(leaf);
    return;
  case 0x00000002: // CPUID leaf 0x00000002 - Cache and TLB Descriptors
  case 0x00000003:
  case 0x00000004: // CPUID leaf 0x00000004 - Deterministic Cache Parameters
  case 0x00000005: // CPUID leaf 0x00000005 - MONITOR/MWAIT leaf
  case 0x00000006: // CPUID leaf 0x00000006 - Thermal and Power Management Leaf
  case 0x00000007:
  case 0x00000008:
  case 0x00000009:
  case 0x0000000A: // CPUID leaf 0x0000000A - Architectural Performance Monitor
  case 0x0000000B:
  case 0x0000000C:
  case 0x0000000D:
    cpuid_table->get_std_leaf(function, subfunction, leaf);
    return;
  case 0x0000000E:
  case 0x0000000F: // CPUID leaf 0x0000000F - Intel Resource Director Technology
  case 0x00000010: // CPUID leaf 0x00000010 - Intel Resource Director Technology
  case 0x00000011:
  case 0x00000012: // SGX Capability
  case 0x00000013:
  case 0x00000014: // Processor Trace
    get_reserved_leaf(leaf);
    return;
  case 0x00000015: // CPUID leaf 0x00000015 - Time Stamp Counter and Core Crystal Clock Information Leaf
  case 0x00000016: // CPUID leaf 0x00000016 - Processor Frequency Information Leaf
  default:
    cpuid_table->get_std_leaf(function, subfunction, leaf);
    return;
  }
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
// MSR 00000489: 0000-0000-0037-2FFF	BX_MSR_VMX_CR4_FIXED1
// MSR 0000048A: 0000-0000-0000-002E	BX_MSR_VMX_VMCS_ENUM
// MSR 0000048B: 025F-3CFF-0000-0000	BX_MSR_VMX_PROCBASED_CTRLS2
// MSR 0000048C: 0000-0F01-0673-4141	BX_MSR_VMX_MSR_VMX_EPT_VPID_CAP
// MSR 0000048D: 0000-007F-0000-0016	BX_MSR_VMX_TRUE_PINBASED_CTRLS
// MSR 0000048E: FFF9-FFFE-0400-6172    BX_MSR_VMX_TRUE_PROCBASED_CTRLS
// MSR 0000048F: 01FF-FFFF-0003-6DFB	BX_MSR_VMX_TRUE_VMEXIT_CTRLS
// MSR 00000490: 0003-FFFF-0000-11FB	BX_MSR_VMX_TRUE_VMENTRY_CTRLS

Bit32u corei3_cnl_t::get_vmx_extensions_bitmask(void) const
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
         BX_VMX_VMCS_SHADOWING |
         BX_VMX_EPT_EXCEPTION |
         BX_VMX_SW_INTERRUPT_INJECTION_ILEN_0 |
         BX_VMX_MBE_CONTROL |
         BX_VMX_TSC_SCALING;
}

#endif

// leaf 0x00000000 //

// leaf 0x00000001 //
void corei3_cnl_t::get_std_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [19:16] Extended Model
  //   [27:20] Extended Family
  leaf->eax = 0x00060663;

  // EBX:
  //   [7:0]   Brand ID
  //   [15:8]  CLFLUSH cache line size (value*8 = cache line size in bytes)
  //   [23:16] Number of logical processors in one physical processor
  //   [31:24] Local Apic ID

  unsigned n_logical_processors = ncores*nthreads;
  leaf->ebx = ((CACHE_LINE_SIZE / 8) << 8) |
              (n_logical_processors << 16);
#if BX_SUPPORT_APIC
  leaf->ebx |= ((cpu->get_apic_id() & 0xff) << 24);
#endif

  // ECX: Extended Feature Flags
  // * [0:0]   SSE3: SSE3 Instructions
  // * [1:1]   PCLMULQDQ Instruction support
  // * [2:2]   DTES64: 64-bit DS area
  // * [3:3]   MONITOR/MWAIT support
  // * [4:4]   DS-CPL: CPL qualified debug store
  // * [5:5]   VMX: Virtual Machine Technology
  //   [6:6]   SMX: Secure Virtual Machine Technology
  // * [7:7]   EST: Enhanced Intel SpeedStep Technology
  // * [8:8]   TM2: Thermal Monitor 2
  // * [9:9]   SSSE3: SSSE3 Instructions
  //   [10:10] CNXT-ID: L1 context ID
  // * [11:11] reserved
  // * [12:12] FMA Instructions support
  // * [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  // * [14:14] xTPR update control
  // * [15:15] PDCM - Perfmon and Debug Capability MSR
  //   [16:16] reserved
  // * [17:17] PCID: Process Context Identifiers
  //   [18:18] DCA - Direct Cache Access
  // * [19:19] SSE4.1 Instructions
  // * [20:20] SSE4.2 Instructions
  // * [21:21] X2APIC
  // * [22:22] MOVBE instruction
  // * [23:23] POPCNT instruction
  // * [24:24] TSC Deadline
  // * [25:25] AES Instructions
  // * [26:26] XSAVE extensions support
  // * [27:27] OSXSAVE support
  // * [28:28] AVX extensions support
  // * [29:29] AVX F16C - Float16 conversion support
  // * [30:30] RDRAND instruction
  //   [31:31] reserved
  leaf->ecx = get_std_cpuid_leaf_1_ecx(BX_CPUID_STD1_ECX_DTES64 |
                                       BX_CPUID_STD1_ECX_DS_CPL |
                                       BX_CPUID_STD1_ECX_EST |
                                       BX_CPUID_STD1_ECX_THERMAL_MONITOR2 |
                                       BX_CPUID_STD1_ECX_xTPR |
                                       BX_CPUID_STD1_ECX_PDCM);

  // EDX: Standard Feature Flags
  // * [0:0]   FPU on chip
  // * [1:1]   VME: Virtual-8086 Mode enhancements
  // * [2:2]   DE: Debug Extensions (I/O breakpoints)
  // * [3:3]   PSE: Page Size Extensions
  // * [4:4]   TSC: Time Stamp Counter
  // * [5:5]   MSR: RDMSR and WRMSR support
  // * [6:6]   PAE: Physical Address Extensions
  // * [7:7]   MCE: Machine Check Exception
  // * [8:8]   CXS: CMPXCHG8B instruction
  // * [9:9]   APIC: APIC on Chip
  //   [10:10] Reserved
  // * [11:11] SYSENTER/SYSEXIT support
  // * [12:12] MTRR: Memory Type Range Reg
  // * [13:13] PGE/PTE Global Bit
  // * [14:14] MCA: Machine Check Architecture
  // * [15:15] CMOV: Cond Mov/Cmp Instructions
  // * [16:16] PAT: Page Attribute Table
  // * [17:17] PSE-36: Physical Address Extensions
  //   [18:18] PSN: Processor Serial Number
  // * [19:19] CLFLUSH: CLFLUSH Instruction support
  //   [20:20] Reserved
  // * [21:21] DS: Debug Store
  // * [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  // * [25:25] SSE: SSE Extensions
  // * [26:26] SSE2: SSE2 Extensions
  // * [27:27] Self Snoop
  // * [28:28] Hyper Threading Technology
  // * [29:29] TM: Thermal Monitor
  //   [30:30] Reserved
  // * [31:31] PBE: Pending Break Enable
  leaf->edx = get_std_cpuid_leaf_1_edx(BX_CPUID_STD1_EDX_DEBUG_STORE |
                                       BX_CPUID_STD1_EDX_ACPI |
                                       BX_CPUID_STD1_EDX_SELF_SNOOP |
#if BX_SUPPORT_SMP
                                       BX_CPUID_STD1_EDX_HT |
#endif
                                       BX_CPUID_STD1_EDX_THERMAL_MONITOR |
                                       BX_CPUID_STD1_EDX_PBE);
}

// leaf 0x00000002 - Cache and TLB Descriptors //
// leaf 0x00000003 - Processor Serial Number (not supported) //

// leaf 0x00000004 - Deterministic Cache Parameters //
// leaf 0x00000005 MONITOR/MWAIT Leaf                //
// leaf 0x00000006 Thermal and Power Management Leaf //

// leaf 0x00000007 //
// leaf 0x00000008 reserved                          //
// leaf 0x00000009 direct cache access not supported //

// leaf 0x0000000A //

// leaf 0x0000000C reserved //
// leaf 0x0000000D - XSAVE //
// leaf 0x0000000E reserved //
// leaf 0x0000000F Intel Resource Director Technology (Intel RDT) Monitoring Enumeration (not implemented) //
// leaf 0x00000010 Intel Resource Director Technology (Intel RDT) Allocation Enumeration (not implemented) //
// leaf 0x00000011 reserved //
// leaf 0x00000012 SGX (not implemented) //
// leaf 0x00000013 reserved //
// leaf 0x00000014 Intel Processor Trace (not implemented) //
// leaf 0x00000015 Time Stamp Counter and Nominal Core Crystal Clock Information //
// leaf 0x00000016 Processor Frequency Information //

// leaf 0x80000000 //

// leaf 0x80000001 //
// leaf 0x80000002 //
// leaf 0x80000003 //
// leaf 0x80000004 //

// leaf 0x80000005 - L1 Cache and TLB Identifiers (reserved for Intel)
// leaf 0x80000006 - L2 Cache and TLB Identifiers
// leaf 0x80000007 - Advanced Power Management

bx_cpuid_t *create_corei3_cnl_cpuid(BX_CPU_C *cpu) { return new corei3_cnl_t(cpu); }

#endif
