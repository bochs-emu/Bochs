/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024-2026 Stanislav Shwartsman
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
#include "arrow_lake.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX

#define VMCS_REVISION_ID 0x04

// CPUID dump of the real CPU (taken from arrow_lake.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000023, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x000C0662, 0x00800800, 0x7FFAFBFF, 0xBFEBFBFF },
  /* leaf 0x00000002 */ { 0x00FEFF01, 0x000000F0, 0x00000000, 0x00000000 },
  /* leaf 0x00000003 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000004 */ { 0xFC004121, 0x02C0003F, 0x0000003F, 0x00000000 }, // [SL 00] [L1D: 48 KB]
  /* leaf 0x00000004 */ { 0xFC004122, 0x03C0003F, 0x0000003F, 0x00000000 }, // [SL 01] [L1I: 64 KB]
  /* leaf 0x00000004 */ { 0xFC01C143, 0x02C0003F, 0x00000FFF, 0x00000000 }, // [SL 02] [L2U: 3 MB]
  /* leaf 0x00000004 */ { 0xFC1FC163, 0x02C0003F, 0x00007FFF, 0x00000004 }, // [SL 03] [L3U: 24 MB]
  /* leaf 0x00000005 */ { 0x00000040, 0x00000040, 0x00000003, 0x10102020 },
  /* leaf 0x00000006 */ { 0x00880075, 0x00000002, 0x00000401, 0x00000003 },
  /* leaf 0x00000007 */ { 0x00000002, 0x239CA7EB, 0x994027AC, 0xFC18C430 }, // [SL 00]
  /* leaf 0x00000007 */ { 0x44C009D7, 0x00000001, 0x00000000, 0x00040430 }, // [SL 01]
  /* leaf 0x00000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x000000BF }, // [SL 02]
  /* leaf 0x00000008 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000A */ { 0x0D300806, 0x00000280, 0x00000007, 0x00008603 },
  /* leaf 0x0000000B */ { 0x00000001, 0x00000001, 0x00000100, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000B */ { 0x00000007, 0x0000000E, 0x00000201, 0x00000000 }, // [SL 01]
  /* leaf 0x0000000C */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000D */ { 0x00000207, 0x00000340, 0x00000A88, 0x00000000 }, // [SL 00] [x87]
  /* leaf 0x0000000D */ { 0x0000000F, 0x000003D0, 0x0000D900, 0x00000000 }, // [SL 01] [SSE]
  /* leaf 0x0000000D */ { 0x00000100, 0x00000240, 0x00000000, 0x00000000 }, // [SL 02] [AVX]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 03] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 04] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 05] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 06] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 07] not in dump
  /* leaf 0x0000000D */ { 0x00000080, 0x00000000, 0x00000001, 0x00000000 }, // [SL 08] [PT]
  /* leaf 0x0000000D */ { 0x00000008, 0x00000A80, 0x00000000, 0x00000000 }, // [SL 09] [PKRU]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 0A] not in dump
  /* leaf 0x0000000D */ { 0x00000010, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0B] [CET_U]
  /* leaf 0x0000000D */ { 0x00000018, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0C] [CET_S]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 0D] not in dump
  /* leaf 0x0000000D */ { 0x00000030, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0E] [UINTR]
  /* leaf 0x0000000D */ { 0x00000328, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0F] [LBR]
  /* leaf 0x0000000E */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000F */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000F */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000010 */ { 0x00000000, 0x00000040, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000010 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000011 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000013 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000014 */ { 0x00000001, 0x0000015F, 0x00000007, 0x00000000 }, // [SL 00]
  /* leaf 0x00000014 */ { 0x02490002, 0x003F003F, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000015 */ { 0x00000002, 0x000000DA, 0x0249F000, 0x00000000 },
  /* leaf 0x00000016 */ { 0x00001068, 0x00001450, 0x00000064, 0x00000000 },
  /* leaf 0x00000017 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000018 */ { 0x00000008, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080001, 0x00000020, 0x00004022 }, // [SL 01]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080006, 0x00000004, 0x00004022 }, // [SL 02]
  /* leaf 0x00000018 */ { 0x00000000, 0x0010000F, 0x00000001, 0x00004125 }, // [SL 03]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080001, 0x00000010, 0x00004024 }, // [SL 04]
  /* leaf 0x00000018 */ { 0x00000000, 0x00040006, 0x00000008, 0x00004024 }, // [SL 05]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080008, 0x00000001, 0x00004124 }, // [SL 06]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080007, 0x00000080, 0x00004043 }, // [SL 07]
  /* leaf 0x00000018 */ { 0x00000000, 0x00080009, 0x00000080, 0x00004043 }, // [SL 08]
  /* leaf 0x00000019 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000001A */ { 0x40000003, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000001B */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // not in dump
  /* leaf 0x0000001C */ { 0x4000000B, 0x00000007, 0x000F0007, 0x00000000 },
  /* leaf 0x0000001D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x0000001E */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x0000001F */ { 0x00000001, 0x00000001, 0x00000100, 0x00000000 }, // [SL 00]
  /* leaf 0x0000001F */ { 0x00000003, 0x00000001, 0x00000201, 0x00000000 }, // [SL 01]
  /* leaf 0x0000001F */ { 0x00000007, 0x0000000E, 0x00000302, 0x00000000 }, // [SL 02]
  /* leaf 0x00000020 */ { 0x00000000, 0x00000001, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000021 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000022 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000023 */ { 0x0000000B, 0x00000003, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000023 */ { 0x000003FF, 0x0000000F, 0x00000000, 0x00000000 }, // [SL 01]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 02]
  /* leaf 0x00000023 */ { 0x00001DFF, 0x00000000, 0x00000000, 0x00000000 }, // [SL 03]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 04]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 05]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 06]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 07]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 08]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 09]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 0A]
  /* leaf 0x00000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }  // [SL 0B]
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000008, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000000, 0x00000000, 0x00000121, 0x2C100000 },
  /* leaf 0x80000002 */ { 0x65746E49, 0x2952286C, 0x726F4320, 0x4D542865 },
  /* leaf 0x80000003 */ { 0x6C552029, 0x20617274, 0x34322035, 0x00004B35 },
  /* leaf 0x80000004 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000005 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000006 */ { 0x00000000, 0x00000000, 0x0C007040, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000100 },
  /* leaf 0x80000008 */ { 0x0000302E, 0x00000200, 0x00000000, 0x00000000 }
};

arrow_lake_t::arrow_lake_t(BX_CPU_C *cpu):
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
  cpuid_table->register_leaf(0x0000000D, 16, std_leafs + 19);
  cpuid_table->register_leaf(0x0000000E,  1, std_leafs + 35);
  cpuid_table->register_leaf(0x0000000F,  2, std_leafs + 36);
  cpuid_table->register_leaf(0x00000010,  2, std_leafs + 38);
  cpuid_table->register_leaf(0x00000011,  1, std_leafs + 40);
  cpuid_table->register_leaf(0x00000012,  2, std_leafs + 41);
  cpuid_table->register_leaf(0x00000013,  1, std_leafs + 43);
  cpuid_table->register_leaf(0x00000014,  2, std_leafs + 44);
  cpuid_table->register_leaf(0x00000015,  1, std_leafs + 46);
  cpuid_table->register_leaf(0x00000016,  1, std_leafs + 47);
  cpuid_table->register_leaf(0x00000017,  1, std_leafs + 48);
  cpuid_table->register_leaf(0x00000018,  9, std_leafs + 49);
  cpuid_table->register_leaf(0x00000019,  1, std_leafs + 58);
  cpuid_table->register_leaf(0x0000001A,  1, std_leafs + 59);
  cpuid_table->register_leaf(0x0000001B,  1, std_leafs + 60);
  cpuid_table->register_leaf(0x0000001C,  1, std_leafs + 61);
  cpuid_table->register_leaf(0x0000001D,  1, std_leafs + 62);
  cpuid_table->register_leaf(0x0000001E,  1, std_leafs + 63);
  cpuid_table->register_leaf(0x0000001F,  3, std_leafs + 64);
  cpuid_table->register_leaf(0x00000020,  1, std_leafs + 67);
  // leafs 0x00000021 .. 0x00000023 are in the dump only, above max_std_leaf

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
    BX_INFO(("You must compile with --enable-vmx=2 for Intel Core Arrow Lake VMX configuration"));

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
  enable_cpu_extension(BX_ISA_AVX_IFMA);
  enable_cpu_extension(BX_ISA_AVX_VNNI_INT8);
  enable_cpu_extension(BX_ISA_AVX_VNNI_INT16);
  enable_cpu_extension(BX_ISA_AVX_NE_CONVERT);
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
  enable_cpu_extension(BX_ISA_CLFLUSHOPT);
  enable_cpu_extension(BX_ISA_CLWB);
  enable_cpu_extension(BX_ISA_XSAVEC);
  enable_cpu_extension(BX_ISA_XSAVES);
  enable_cpu_extension(BX_ISA_GFNI);
  enable_cpu_extension(BX_ISA_VAES_VPCLMULQDQ);
  enable_cpu_extension(BX_ISA_SHA512);
  enable_cpu_extension(BX_ISA_SM3);
  enable_cpu_extension(BX_ISA_SM4);
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
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_WAITPKG);
#endif
  enable_cpu_extension(BX_ISA_SERIALIZE);
  enable_cpu_extension(BX_ISA_CMPCCXADD);
  enable_cpu_extension(BX_ISA_LASS);
#if BX_SUPPORT_UINTR
  enable_cpu_extension(BX_ISA_UINTR);
#endif
  enable_cpu_extension(BX_ISA_SPEC_CTRL);
}

unsigned arrow_lake_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  return cpuid_table->get_leaf(function, subfunction, leaf);
}

#if BX_SUPPORT_VMX >= 2
Bit32u arrow_lake_t::get_vmx_extensions_bitmask(void) const
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
// leaf 0x80000002 //
// leaf 0x80000003 //
// leaf 0x80000004 //
// leaf 0x80000005 - L1 Cache and TLB Identifiers (reserved for Intel) //
// leaf 0x80000006 //
// leaf 0x80000007 - Advanced Power Management //
// leaf 0x80000008 //

bx_cpuid_t *create_arrow_lake_cpuid(BX_CPU_C *cpu) { return new arrow_lake_t(cpu); }

#endif
