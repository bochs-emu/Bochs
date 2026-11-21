/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023-2026 Stanislav Shwartsman
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
#include "cpu/cpu.h"
#include "sapphire_rapids.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX

#define VMCS_REVISION_ID 0x04

// CPUID dump of the real CPU (Intel Xeon w7-2475X), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000020, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x000806F8, 0x00800800, 0x7FFEFBFF, 0xBFEBFBFF },
  /* leaf 0x00000002 */ { 0x00FEFF01, 0x000000F0, 0x00000000, 0x00000000 },
  /* leaf 0x00000003 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000004 */ { 0xFC004121, 0x02C0003F, 0x0000003F, 0x00000000 }, // [SL 00] [L1D: 48 KB]
  /* leaf 0x00000004 */ { 0xFC004122, 0x01C0003F, 0x0000003F, 0x00000000 }, // [SL 01] [L1I: 32 KB]
  /* leaf 0x00000004 */ { 0xFC004143, 0x03C0003F, 0x000007FF, 0x00000000 }, // [SL 02] [L2U: 2 MB]
  /* leaf 0x00000004 */ { 0xFC1FC163, 0x0380003F, 0x00009FFF, 0x00000004 }, // [SL 03] [L3U: 38400 KB]
  /* leaf 0x00000005 */ { 0x00000040, 0x00000040, 0x00000003, 0x00001020 },
  /* leaf 0x00000006 */ { 0x0045CEF7, 0x00000002, 0x00000009, 0x00000000 },
  /* leaf 0x00000007 */ { 0x00000002, 0xF3BFBFFB, 0xBB417FEE, 0xFFDD4430 }, // [SL 00]
  /* leaf 0x00000007 */ { 0x00001C30, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000017 }, // [SL 02]
  /* leaf 0x00000008 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000A */ { 0x08300805, 0x00000000, 0x0000000F, 0x00008604 },
  /* leaf 0x0000000B */ { 0x00000001, 0x00000002, 0x00000100, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000B */ { 0x00000007, 0x00000028, 0x00000201, 0x00000000 }, // [SL 01]
  /* leaf 0x0000000C */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000D */ { 0x000602E7, 0x00002B00, 0x00002B00, 0x00000000 }, // [SL 00] [x87]
  /* leaf 0x0000000D */ { 0x0000001F, 0x00002A80, 0x0000DD00, 0x00000000 }, // [SL 01] [SSE]
  /* leaf 0x0000000D */ { 0x00000100, 0x00000240, 0x00000000, 0x00000000 }, // [SL 02] [AVX]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 03] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 04] not in dump
  /* leaf 0x0000000D */ { 0x00000040, 0x00000440, 0x00000000, 0x00000000 }, // [SL 05] [AVX-512 Opmask]
  /* leaf 0x0000000D */ { 0x00000200, 0x00000480, 0x00000000, 0x00000000 }, // [SL 06] [AVX-512 Hi256]
  /* leaf 0x0000000D */ { 0x00000400, 0x00000680, 0x00000000, 0x00000000 }, // [SL 07] [AVX-512 Hi16]
  /* leaf 0x0000000D */ { 0x00000080, 0x00000000, 0x00000001, 0x00000000 }, // [SL 08] [PT]
  /* leaf 0x0000000D */ { 0x00000008, 0x00000A80, 0x00000000, 0x00000000 }, // [SL 09] [PKRU]
  /* leaf 0x0000000D */ { 0x00000008, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0A]
  /* leaf 0x0000000D */ { 0x00000010, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0B]
  /* leaf 0x0000000D */ { 0x00000018, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0C]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 0D] not in dump
  /* leaf 0x0000000D */ { 0x00000030, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0E]
  /* leaf 0x0000000D */ { 0x00000328, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0F]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 10] not in dump
  /* leaf 0x0000000D */ { 0x00000040, 0x00000AC0, 0x00000002, 0x00000000 }, // [SL 11]
  /* leaf 0x0000000D */ { 0x00002000, 0x00000B00, 0x00000006, 0x00000000 }, // [SL 12]
  /* leaf 0x0000000E */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000F */ { 0x00000000, 0x0000009F, 0x00000000, 0x00000002 }, // [SL 00]
  /* leaf 0x0000000F */ { 0x00000008, 0x0000A000, 0x0000009F, 0x00000007 }, // [SL 01]
  /* leaf 0x00000010 */ { 0x00000000, 0x0000000E, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000010 */ { 0x0000000E, 0x00006000, 0x00000004, 0x0000000E }, // [SL 01]
  /* leaf 0x00000011 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000013 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000014 */ { 0x00000001, 0x0000005F, 0x00000007, 0x00000000 }, // [SL 00]
  /* leaf 0x00000014 */ { 0x02490002, 0x003F003F, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000015 */ { 0x00000002, 0x000000D8, 0x016E3600, 0x00000000 },
  /* leaf 0x00000016 */ { 0x00000A28, 0x000012C0, 0x00000064, 0x00000000 },
  /* leaf 0x00000017 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000018 */ { 0x00000008, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080001, 0x00000020, 0x00004022 }, // [SL 01]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080006, 0x00000004, 0x00004022 }, // [SL 02]
  /* leaf 0x00000018 */ { 0x00000000, 0x0010000F, 0x00000001, 0x00004125 }, // [SL 03]
  /* leaf 0x00000018 */ { 0x00000000, 0x00040001, 0x00000010, 0x00004024 }, // [SL 04]
  /* leaf 0x00000018 */ { 0x00000000, 0x00040006, 0x00000008, 0x00004024 }, // [SL 05]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080008, 0x00000001, 0x00004124 }, // [SL 06]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080007, 0x00000080, 0x00004043 }, // [SL 07]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080009, 0x00000080, 0x00004043 }, // [SL 08]
  /* leaf 0x00000019 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000001A */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000001B */ { 0x00000001, 0x00000001, 0x00000000, 0x00000000 },
  /* leaf 0x0000001C */ { 0x4000000B, 0x00000007, 0x00000007, 0x00000000 },
  /* leaf 0x0000001D */ { 0x00000001, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x0000001D */ { 0x04002000, 0x00080040, 0x00000010, 0x00000000 }, // [SL 01]
  /* leaf 0x0000001E */ { 0x00000000, 0x00004010, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x0000001F */ { 0x00000001, 0x00000002, 0x00000100, 0x00000000 }, // [SL 00]
  /* leaf 0x0000001F */ { 0x00000007, 0x00000028, 0x00000201, 0x00000000 }, // [SL 01]
  /* leaf 0x00000020 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }  // [SL 00]
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000008, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000000, 0x00000000, 0x00000121, 0x2C100000 },
  /* leaf 0x80000002 */ { 0x65746E49, 0x2952286C, 0x6F655820, 0x2952286E },
  /* leaf 0x80000003 */ { 0x2D377720, 0x35373432, 0x00000058, 0x00000000 },
  /* leaf 0x80000004 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000005 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000006 */ { 0x00000000, 0x00000000, 0x08007040, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000100 },
  /* leaf 0x80000008 */ { 0x00003934, 0x00000200, 0x00000000, 0x00000000 }
};

sapphire_rapids_t::sapphire_rapids_t(BX_CPU_C *cpu):
#if BX_SUPPORT_VMX
    bx_cpuid_t(cpu, VMCS_REVISION_ID)
#else
    bx_cpuid_t(cpu)
#endif
{
  set_max_leaf(0x20, 0x80000008);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000,  1, std_leafs +  0);
  cpuid_table->register_leaf(0x00000001,  1, std_leafs +  1);
  cpuid_table->register_leaf(0x00000002,  1, std_leafs +  2);
  cpuid_table->register_leaf(0x00000003,  1, std_leafs +  3);
  cpuid_table->register_leaf(0x00000004,  4, std_leafs +  4);
  cpuid_table->register_leaf(0x00000005,  1, std_leafs +  8);
  cpuid_table->register_leaf(0x00000006,  1, std_leafs +  9);
  cpuid_table->register_leaf(0x00000007,  3, std_leafs + 10);
  cpuid_table->register_leaf(0x00000008,  1, std_leafs + 13);
  cpuid_table->register_leaf(0x00000009,  1, std_leafs + 14);
  cpuid_table->register_leaf(0x0000000A,  1, std_leafs + 15);
  cpuid_table->register_leaf(0x0000000B,  2, std_leafs + 16);
  cpuid_table->register_leaf(0x0000000C,  1, std_leafs + 18);
  cpuid_table->register_leaf(0x0000000D, 19, std_leafs + 19);
  cpuid_table->register_leaf(0x0000000E,  1, std_leafs + 38);
  cpuid_table->register_leaf(0x0000000F,  2, std_leafs + 39);
  cpuid_table->register_leaf(0x00000010,  2, std_leafs + 41);
  cpuid_table->register_leaf(0x00000011,  1, std_leafs + 43);
  cpuid_table->register_leaf(0x00000012,  2, std_leafs + 44);
  cpuid_table->register_leaf(0x00000013,  1, std_leafs + 46);
  cpuid_table->register_leaf(0x00000014,  2, std_leafs + 47);
  cpuid_table->register_leaf(0x00000015,  1, std_leafs + 49);
  cpuid_table->register_leaf(0x00000016,  1, std_leafs + 50);
  cpuid_table->register_leaf(0x00000017,  1, std_leafs + 51);
  cpuid_table->register_leaf(0x00000018,  9, std_leafs + 52);
  cpuid_table->register_leaf(0x00000019,  1, std_leafs + 61);
  cpuid_table->register_leaf(0x0000001A,  1, std_leafs + 62);
  cpuid_table->register_leaf(0x0000001B,  1, std_leafs + 63);
  cpuid_table->register_leaf(0x0000001C,  1, std_leafs + 64);
  cpuid_table->register_leaf(0x0000001D,  2, std_leafs + 65);
  cpuid_table->register_leaf(0x0000001E,  1, std_leafs + 67);
  cpuid_table->register_leaf(0x0000001F,  2, std_leafs + 68);
  cpuid_table->register_leaf(0x00000020,  1, std_leafs + 70);

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
    BX_INFO(("You must compile with --enable-vmx=2 for Intel Core Xeon Sapphire Rapids VMX configuration"));

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
  enable_cpu_extension(BX_ISA_AES_PCLMULQDQ);
  enable_cpu_extension(BX_ISA_MOVBE);
  enable_cpu_extension(BX_ISA_AVX);
  enable_cpu_extension(BX_ISA_AVX_F16C);
  enable_cpu_extension(BX_ISA_AVX2);
  enable_cpu_extension(BX_ISA_AVX_FMA);
  enable_cpu_extension(BX_ISA_AVX_VNNI);
  enable_cpu_extension(BX_ISA_LZCNT);
  enable_cpu_extension(BX_ISA_BMI1);
  enable_cpu_extension(BX_ISA_BMI2);
  enable_cpu_extension(BX_ISA_ADX);
  enable_cpu_extension(BX_ISA_FSGSBASE);
  enable_cpu_extension(BX_ISA_INVPCID);
  enable_cpu_extension(BX_ISA_SMEP);
  enable_cpu_extension(BX_ISA_SMAP);
  enable_cpu_extension(BX_ISA_RDRAND);
  enable_cpu_extension(BX_ISA_RDSEED);
  enable_cpu_extension(BX_ISA_FDP_DEPRECATION);
  enable_cpu_extension(BX_ISA_FCS_FDS_DEPRECATION);
  enable_cpu_extension(BX_ISA_SHA);
#if BX_SUPPORT_EVEX
  enable_cpu_extension(BX_ISA_GFNI);
  enable_cpu_extension(BX_ISA_VAES_VPCLMULQDQ);
  enable_cpu_extension(BX_ISA_AVX512);
  enable_cpu_extension(BX_ISA_AVX512_DQ);
  enable_cpu_extension(BX_ISA_AVX512_CD);
  enable_cpu_extension(BX_ISA_AVX512_BW);
  enable_cpu_extension(BX_ISA_AVX512_IFMA52);
  enable_cpu_extension(BX_ISA_AVX512_VBMI);
  enable_cpu_extension(BX_ISA_AVX512_VBMI2);
  enable_cpu_extension(BX_ISA_AVX512_VNNI);
  enable_cpu_extension(BX_ISA_AVX512_BITALG);
  enable_cpu_extension(BX_ISA_AVX512_VPOPCNTDQ);
  enable_cpu_extension(BX_ISA_AVX512_BF16);
  enable_cpu_extension(BX_ISA_AVX512_FP16);
#endif
#if BX_SUPPORT_AMX
  enable_cpu_extension(BX_ISA_AMX);
  enable_cpu_extension(BX_ISA_AMX_INT8);
  enable_cpu_extension(BX_ISA_AMX_BF16);
#endif
  enable_cpu_extension(BX_ISA_CLFLUSHOPT);
  enable_cpu_extension(BX_ISA_CLWB);
//enable_cpu_extension(BX_ISA_CLDEMOTE);
  enable_cpu_extension(BX_ISA_XSAVEC);
  enable_cpu_extension(BX_ISA_XSAVES);
#if BX_SUPPORT_PKEYS
  enable_cpu_extension(BX_ISA_PKU);
  enable_cpu_extension(BX_ISA_PKS);
#endif
#if BX_SUPPORT_CET
  enable_cpu_extension(BX_ISA_CET);
#endif
  enable_cpu_extension(BX_ISA_UMIP);
  enable_cpu_extension(BX_ISA_RDPID);
  enable_cpu_extension(BX_ISA_MOVDIRI);
  enable_cpu_extension(BX_ISA_MOVDIR64B);
  enable_cpu_extension(BX_ISA_LA57);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_WAITPKG);
#endif
  enable_cpu_extension(BX_ISA_SERIALIZE);
#if BX_SUPPORT_UINTR
  enable_cpu_extension(BX_ISA_UINTR);
#endif
  enable_cpu_extension(BX_ISA_SPEC_CTRL);
}

void sapphire_rapids_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  switch(function) {
  case 0x80000000:
    cpuid_table->get_ext_leaf(function, subfunction, leaf);
    return;
  case 0x80000001:
    get_ext_cpuid_leaf_1(leaf);
    return;
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
  case 0x0000000F: // CPUID leaf 0x0000000F - Intel Resource Director Technonology
  case 0x00000010: // CPUID leaf 0x00000010 - Intel Resource Director Technonology
  case 0x00000011:
  case 0x00000012: // SGX Capability
  case 0x00000013:
  case 0x00000014: // Processor Trace
    get_reserved_leaf(leaf);
    return;
  case 0x00000015: // CPUID leaf 0x00000015 - Time Stamp Counter and Core Crystal Clock Information Leaf
  case 0x00000016: // CPUID leaf 0x00000016 - Processor Frequency Information Leaf
  case 0x00000017:
  case 0x00000018: // TLB Descriptors
    cpuid_table->get_std_leaf(function, subfunction, leaf);
    return;
  case 0x00000019: // CPUID leaf 0x00000019 - Keylocker Leaf
  case 0x0000001A: // CPUID leaf 0x0000001A - native Model ID Enumeration leaf (for Hybrid)
  case 0x0000001B: // PCONFIG Information
  case 0x0000001C: // CPUID leaf 0x0000001C - Last Branch Record (Architectural LBR) leaf
    get_reserved_leaf(leaf);
    return;
  case 0x0000001D: // AMX
  case 0x0000001E: // AMX: TMUL Information Main leaf
  case 0x0000001F: // V2 Extended Topology Enumeration leaf
    cpuid_table->get_std_leaf(function, subfunction, leaf);
    return;
  case 0x00000020: // HRESET Information
  default:
    cpuid_table->get_std_leaf(function, subfunction, leaf);
    return;
  }
}

#if BX_SUPPORT_VMX >= 2
Bit32u sapphire_rapids_t::get_vmx_extensions_bitmask(void) const
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
         BX_VMX_PERF_GLOBAL_CTRL |
         BX_VMX_PAT |
         BX_VMX_EFER |
         BX_VMX_DESCRIPTOR_TABLE_EXIT |
         BX_VMX_X2APIC_VIRTUALIZATION |
         BX_VMX_PREEMPTION_TIMER |
         BX_VMX_PAUSE_LOOP_EXITING |
         BX_VMX_EPT_ACCESS_DIRTY |
         BX_VMX_VINTR_DELIVERY |
         BX_VMX_VMCS_SHADOWING |
         BX_VMX_EPTP_SWITCHING |
         BX_VMX_EPT_EXCEPTION |
         BX_VMX_PML |
         BX_VMX_SW_INTERRUPT_INJECTION_ILEN_0 |
         BX_VMX_POSTED_INTERRUPTS |
         BX_VMX_MBE_CONTROL |
         BX_VMX_SPP |
         BX_VMX_TSC_SCALING |
         BX_VMX_SPEC_CTRL_VIRTUALIZATION;
}
#endif

// leaf 0x00000000 //

// leaf 0x00000001 //
void sapphire_rapids_t::get_std_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [19:16] Extended Model
  //   [27:20] Extended Family
  leaf->eax = 0x000806f8;

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
  // ! [6:6]   SMX: Secure Virtual Machine Technology
  // * [7:7]   EST: Enhanced Intel SpeedStep Technology
  // * [8:8]   TM2: Thermal Monitor 2
  // * [9:9]   SSSE3: SSSE3 Instructions
  //   [10:10] CNXT-ID: L1 context ID
  //   [11:11] reserved
  // * [12:12] FMA Instructions support
  // * [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  // * [14:14] xTPR update control
  // * [15:15] PDCM - Perfmon and Debug Capability MSR
  //   [16:16] reserved
  // * [17:17] PCID: Process Context Identifiers
  // * [18:18] DCA - Direct Cache Access
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
                                       BX_CPUID_STD1_ECX_PDCM |
                                       BX_CPUID_STD1_ECX_DCA);

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

// leaf 0x00000003 - Processor Serial Number (not supported) //

// leaf 0x00000004 - Deterministic Cache Parameters //
// leaf 0x00000005 MONITOR/MWAIT Leaf                //
// leaf 0x00000006 Thermal and Power Management Leaf //

// leaf 0x00000007 //
// leaf 0x00000008 reserved                          //
// leaf 0x00000009 direct cache access not supported //

// leaf 0x0000000A //

// leaf 0x0000000C - reserved //
// leaf 0x0000000D - XSAVE //
// leaf 0x0000000E - reserved //
// leaf 0x0000000F - L3 Cache Intel RDT Monitoring Capability Enumeration //
// leaf 0x00000010 - Intel Resource Director Technology (Intel RDT) Allocation Enumeration //
// leaf 0x00000011 - reserved //
// leaf 0x00000012 - Intel Software Guard Extensions //
// leaf 0x00000013 - reserved //
// leaf 0x00000014 - Intel Processor Trace Enumeration //

// leaf 0x00000015 - Time Stamp Counter and Core Crystal Clock Information Leaf //
// leaf 0x00000016 - Processor Frequency Information Leaf
// leaf 0x00000017 - System-On-Chip Vendor Attribute Enumeration //

// leaf 0x00000018 - Deterministic Address Translation Parameters //
// leaf 0x80000000 //

// leaf 0x80000001 //
void sapphire_rapids_t::get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information (reserved for Intel)
  leaf->eax = 0;

  // EBX:       Brand ID (reserved for Intel)
  leaf->ebx = 0;

  // ECX:
  // * [0:0]   LAHF/SAHF instructions support in 64-bit mode
  //   [1:1]   CMP_Legacy: Core multi-processing legacy mode (AMD)
  //   [2:2]   SVM: Secure Virtual Machine (AMD)
  //   [3:3]   Extended APIC Space
  //   [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
  // * [5:5]   LZCNT: LZCNT instruction support
  //   [6:6]   SSE4A: SSE4A Instructions support (deprecated?)
  //   [7:7]   Misaligned SSE support
  // * [8:8]   PREFETCHW: PREFETCHW instruction support
  //   [9:9]   OSVW: OS visible workarounds (AMD)
  //   [11:10] reserved
  //   [12:12] SKINIT support
  //   [13:13] WDT: Watchdog timer support
  //   [31:14] reserved
  leaf->ecx = get_ext_cpuid_leaf_1_ecx(BX_CPUID_EXT1_ECX_PREFETCHW);

  // EDX:
  //    [10:0] Reserved for Intel
  // * [11:11] SYSCALL/SYSRET support
  //   [19:12] Reserved for Intel
  // * [20:20] No-Execute page protection
  //   [25:21] Reserved
  // * [26:26] 1G paging support
  // * [27:27] Support RDTSCP Instruction
  //   [28:28] Reserved
  // * [29:29] Long Mode
  //   [30:30] AMD 3DNow! Extensions
  //   [31:31] AMD 3DNow! Instructions
  leaf->edx = get_ext_cpuid_leaf_1_edx_intel();
}

// leaf 0x80000002 //
// leaf 0x80000003 //
// leaf 0x80000004 //
// leaf 0x80000005 - L1 Cache and TLB Identifiers (reserved for Intel) //
// leaf 0x80000006 //
// leaf 0x80000007 - Advanced Power Management //
// leaf 0x80000008 //

bx_cpuid_t *create_sapphire_rapids_cpuid(BX_CPU_C *cpu) { return new sapphire_rapids_t(cpu); }

#endif
