/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2026 Stanislav Shwartsman
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
#include "zen3_vermeer.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX

// CPUID dump of the real CPU (taken from zen3_vermeer.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000010, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x00000001 */ { 0x00A20F12, 0x00100800, 0x7EF8320B, 0x178BFBFF },
  /* leaf 0x00000002 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000003 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000004 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // not in dump
  /* leaf 0x00000005 */ { 0x00000040, 0x00000040, 0x00000003, 0x00000011 },
  /* leaf 0x00000006 */ { 0x00000004, 0x00000000, 0x00000001, 0x00000000 },
  /* leaf 0x00000007 */ { 0x00000000, 0x219C97A9, 0x0040068C, 0x00000010 }, // [SL 00]
  /* leaf 0x00000008 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000A */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000B */ { 0x00000001, 0x00000002, 0x00000100, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000B */ { 0x00000004, 0x00000010, 0x00000201, 0x00000000 }, // [SL 01]
  /* leaf 0x0000000C */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000D */ { 0x00000207, 0x00000340, 0x00000988, 0x00000000 }, // [SL 00] [x87]
  /* leaf 0x0000000D */ { 0x0000000F, 0x00000350, 0x00001800, 0x00000000 }, // [SL 01] [SSE]
  /* leaf 0x0000000D */ { 0x00000100, 0x00000240, 0x00000000, 0x00000000 }, // [SL 02] [AVX]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 03] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 04] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 05] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 06] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 07] not in dump
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 08] not in dump
  /* leaf 0x0000000D */ { 0x00000008, 0x00000980, 0x00000000, 0x00000000 }, // [SL 09] [PKRU]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // [SL 0A] not in dump
  /* leaf 0x0000000D */ { 0x00000010, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0B] [CET_U]
  /* leaf 0x0000000D */ { 0x00000018, 0x00000000, 0x00000001, 0x00000000 }, // [SL 0C] [CET_S]
  /* leaf 0x0000000E */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000F */ { 0x00000000, 0x000000FF, 0x00000000, 0x00000002 }, // [SL 00]
  /* leaf 0x0000000F */ { 0x00000000, 0x00000040, 0x000000FF, 0x00000007 }, // [SL 01]
  /* leaf 0x00000010 */ { 0x00000000, 0x00000002, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x00000010 */ { 0x0000000F, 0x00000000, 0x00000004, 0x0000000F }  // [SL 01]
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000023, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x80000001 */ { 0x00A20F12, 0x20000000, 0x75C237FF, 0x2FD3FBFF },
  /* leaf 0x80000002 */ { 0x20444D41, 0x657A7952, 0x2037206E, 0x30303835 },
  /* leaf 0x80000003 */ { 0x2D382058, 0x65726F43, 0x6F725020, 0x73736563 },
  /* leaf 0x80000004 */ { 0x2020726F, 0x20202020, 0x20202020, 0x00202020 },
  /* leaf 0x80000005 */ { 0xFF40FF40, 0xFF40FF40, 0x20080140, 0x20080140 },
  /* leaf 0x80000006 */ { 0x48002200, 0x68004200, 0x02006140, 0x01009140 },
  /* leaf 0x80000007 */ { 0x00000000, 0x0000003B, 0x00000000, 0x00006599 },
  /* leaf 0x80000008 */ { 0x00003030, 0x111EF657, 0x0000400F, 0x00010000 },
  /* leaf 0x80000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000000A */ { 0x00000001, 0x00008000, 0x00000000, 0x101BBCFF },
  /* leaf 0x8000000B */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000000C */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000000E */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000000F */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000010 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000011 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000012 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000013 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000014 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000015 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000016 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000017 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000018 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000019 */ { 0xF040F040, 0xF0400000, 0x00000000, 0x00000000 },
  /* leaf 0x8000001A */ { 0x00000006, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000001B */ { 0x000003FF, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000001C */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000001D */ { 0x00004121, 0x01C0003F, 0x0000003F, 0x00000000 }, // [SL 00] [L1D: 32 KB]
  /* leaf 0x8000001D */ { 0x00004122, 0x01C0003F, 0x0000003F, 0x00000000 }, // [SL 01] [L1I: 32 KB]
  /* leaf 0x8000001D */ { 0x00004143, 0x01C0003F, 0x000003FF, 0x00000002 }, // [SL 02] [L2U: 512 KB]
  /* leaf 0x8000001D */ { 0x0003C163, 0x03C0003F, 0x00007FFF, 0x00000001 }, // [SL 03] [L3U: 32 MB]
  /* leaf 0x8000001E */ { 0x00000000, 0x00000100, 0x00000000, 0x00000000 },
  /* leaf 0x8000001F */ { 0x0001780F, 0x00000173, 0x000001FD, 0x00000001 },
  /* leaf 0x80000020 */ { 0x00000000, 0x00000002, 0x00000000, 0x00000000 }, // [SL 00]
  /* leaf 0x80000020 */ { 0x0000000B, 0x00000000, 0x00000000, 0x0000000F }, // [SL 01]
  /* leaf 0x80000021 */ { 0x0000004D, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000022 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000023 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
};

zen3_vermeer_t::zen3_vermeer_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x10, 0x80000023);
  set_vendor(AuthenticAMD);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000,  1, std_leafs +  0);
  cpuid_table->register_leaf(0x00000001,  1, std_leafs +  1);
  cpuid_table->register_leaf(0x00000002,  1, std_leafs +  2);
  cpuid_table->register_leaf(0x00000003,  1, std_leafs +  3);
  cpuid_table->register_leaf(0x00000004,  1, std_leafs +  4);
  cpuid_table->register_leaf(0x00000005,  1, std_leafs +  5);
  cpuid_table->register_leaf(0x00000006,  1, std_leafs +  6);
  cpuid_table->register_leaf(0x00000007,  1, std_leafs +  7);
  cpuid_table->register_leaf(0x00000008,  1, std_leafs +  8);
  cpuid_table->register_leaf(0x00000009,  1, std_leafs +  9);
  cpuid_table->register_leaf(0x0000000A,  1, std_leafs + 10);
  cpuid_table->register_leaf(0x0000000B,  2, std_leafs + 11);
  cpuid_table->register_leaf(0x0000000C,  1, std_leafs + 13);
  cpuid_table->register_leaf(0x0000000D, 13, std_leafs + 14);
  cpuid_table->register_leaf(0x0000000E,  1, std_leafs + 27);
  cpuid_table->register_leaf(0x0000000F,  2, std_leafs + 28);
  cpuid_table->register_leaf(0x00000010,  2, std_leafs + 30);

  cpuid_table->register_leaf(0x80000000, 1, ext_leafs +  0);
  cpuid_table->register_leaf(0x80000001, 1, ext_leafs +  1);
  cpuid_table->register_leaf(0x80000002, 1, ext_leafs +  2);
  cpuid_table->register_leaf(0x80000003, 1, ext_leafs +  3);
  cpuid_table->register_leaf(0x80000004, 1, ext_leafs +  4);
  cpuid_table->register_leaf(0x80000005, 1, ext_leafs +  5);
  cpuid_table->register_leaf(0x80000006, 1, ext_leafs +  6);
  cpuid_table->register_leaf(0x80000007, 1, ext_leafs +  7);
  cpuid_table->register_leaf(0x80000008, 1, ext_leafs +  8);
  cpuid_table->register_leaf(0x80000009, 1, ext_leafs +  9);
  cpuid_table->register_leaf(0x8000000A, 1, ext_leafs + 10);
  cpuid_table->register_leaf(0x8000000B, 1, ext_leafs + 11);
  cpuid_table->register_leaf(0x8000000C, 1, ext_leafs + 12);
  cpuid_table->register_leaf(0x8000000D, 1, ext_leafs + 13);
  cpuid_table->register_leaf(0x8000000E, 1, ext_leafs + 14);
  cpuid_table->register_leaf(0x8000000F, 1, ext_leafs + 15);
  cpuid_table->register_leaf(0x80000010, 1, ext_leafs + 16);
  cpuid_table->register_leaf(0x80000011, 1, ext_leafs + 17);
  cpuid_table->register_leaf(0x80000012, 1, ext_leafs + 18);
  cpuid_table->register_leaf(0x80000013, 1, ext_leafs + 19);
  cpuid_table->register_leaf(0x80000014, 1, ext_leafs + 20);
  cpuid_table->register_leaf(0x80000015, 1, ext_leafs + 21);
  cpuid_table->register_leaf(0x80000016, 1, ext_leafs + 22);
  cpuid_table->register_leaf(0x80000017, 1, ext_leafs + 23);
  cpuid_table->register_leaf(0x80000018, 1, ext_leafs + 24);
  cpuid_table->register_leaf(0x80000019, 1, ext_leafs + 25);
  cpuid_table->register_leaf(0x8000001A, 1, ext_leafs + 26);
  cpuid_table->register_leaf(0x8000001B, 1, ext_leafs + 27);
  cpuid_table->register_leaf(0x8000001C, 1, ext_leafs + 28);
  cpuid_table->register_leaf(0x8000001D, 4, ext_leafs + 29);
  cpuid_table->register_leaf(0x8000001E, 1, ext_leafs + 33);
  cpuid_table->register_leaf(0x8000001F, 1, ext_leafs + 34);
  cpuid_table->register_leaf(0x80000020, 2, ext_leafs + 35);
  cpuid_table->register_leaf(0x80000021, 1, ext_leafs + 37);
  cpuid_table->register_leaf(0x80000022, 1, ext_leafs + 38);
  cpuid_table->register_leaf(0x80000023, 1, ext_leafs + 39);

  if (! BX_SUPPORT_SVM)
    BX_INFO(("WARNING: SVM support is not compiled in !"));

  if (! BX_SUPPORT_MONITOR_MWAIT)
    BX_INFO(("WARNING: MONITOR/MWAIT support is not compiled in !"));

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_AMD);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_SYSCALL_SYSRET_LEGACY);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
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
  enable_cpu_extension(BX_ISA_SSE);
  enable_cpu_extension(BX_ISA_SSE2);
  enable_cpu_extension(BX_ISA_SSE3);
  enable_cpu_extension(BX_ISA_SSSE3);
  enable_cpu_extension(BX_ISA_SSE4_1);
  enable_cpu_extension(BX_ISA_SSE4_2);
  enable_cpu_extension(BX_ISA_LZCNT);
  enable_cpu_extension(BX_ISA_POPCNT);
  enable_cpu_extension(BX_ISA_SSE4A);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_MONITOR_MWAIT);
#endif
  enable_cpu_extension(BX_ISA_RDTSCP);
  enable_cpu_extension(BX_ISA_XSAVE);
  enable_cpu_extension(BX_ISA_XSAVEOPT);
  enable_cpu_extension(BX_ISA_XSAVEC);
  enable_cpu_extension(BX_ISA_XSAVES);
  enable_cpu_extension(BX_ISA_AES_PCLMULQDQ);
  enable_cpu_extension(BX_ISA_AVX);
  enable_cpu_extension(BX_ISA_AVX_F16C);
  enable_cpu_extension(BX_ISA_AVX2);
  enable_cpu_extension(BX_ISA_AVX_FMA);
  enable_cpu_extension(BX_ISA_MOVBE);
  enable_cpu_extension(BX_ISA_RDRAND);
  enable_cpu_extension(BX_ISA_RDSEED);
  enable_cpu_extension(BX_ISA_BMI1);
  enable_cpu_extension(BX_ISA_BMI2);
  enable_cpu_extension(BX_ISA_VAES_VPCLMULQDQ);
  enable_cpu_extension(BX_ISA_FSGSBASE);
  enable_cpu_extension(BX_ISA_INVPCID);
  enable_cpu_extension(BX_ISA_SMEP);
  enable_cpu_extension(BX_ISA_ADX);
  enable_cpu_extension(BX_ISA_SMAP);
  enable_cpu_extension(BX_ISA_SHA);
  enable_cpu_extension(BX_ISA_CLFLUSHOPT);
  enable_cpu_extension(BX_ISA_CLWB);
  enable_cpu_extension(BX_ISA_UMIP);
  enable_cpu_extension(BX_ISA_RDPID);
#if BX_SUPPORT_SVM
  enable_cpu_extension(BX_ISA_SVM);
#endif
#if BX_SUPPORT_PKEYS
  enable_cpu_extension(BX_ISA_PKU);
#endif
#if BX_SUPPORT_CET
  enable_cpu_extension(BX_ISA_CET);
#endif
  enable_cpu_extension(BX_ISA_FFXSR);
  enable_cpu_extension(BX_ISA_1G_PAGES);
  enable_cpu_extension(BX_ISA_MISALIGNED_SSE);
  enable_cpu_extension(BX_ISA_ALT_MOV_CR8);
  enable_cpu_extension(BX_ISA_XAPIC_EXT);
  enable_cpu_extension(BX_ISA_TCE);
  enable_cpu_extension(BX_ISA_CLZERO);
  enable_cpu_extension(BX_ISA_RDPRU);
  enable_cpu_extension(BX_ISA_SPEC_CTRL_AMD);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_MONITORX_MWAITX);
#endif
}

unsigned zen3_vermeer_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
#if BX_SUPPORT_SVM
  if (function == 0x8000000A) {
    get_ext_cpuid_leaf_A(leaf);
    return 0;
  }
#endif

  return cpuid_table->get_leaf(function, subfunction, leaf);
}

#if BX_SUPPORT_SVM
Bit32u zen3_vermeer_t::get_svm_extensions_bitmask(void) const
{
  // * [0:0]   NP - Nested paging support
  // ! [1:1]   LBR virtualization
  // ! [2:2]   SVM Lock
  // * [3:3]   NRIPS - Next RIP save on VMEXIT
  // * [4:4]   TscRate - MSR based TSC ratio control
  // ! [5:5]   VMCB Clean bits support
  // * [6:6]   Flush by ASID support
  // * [7:7]   Decode assists support
  //   [8:8]   PMC Virtualization
  //   [9:9]   Reserved
  // * [10:10] Pause filter support
  //   [11:11] Reserved
  // * [12:12] Pause filter threshold support
  // * [13:13] AVIC: Advanced Virtual Interrupt Controller support
  //   [14:14] Reserved
  // * [15:15] Nested Virtualization
  // * [16:16] Virtualize the Global Interrupt Flag
  // ! [17:17] GMET: Guest Mode Execution Trap
  //   [18:18] x2AVIC
  // ! [19:19] SSSCheck: SVM supervisor shadow stack restrictions
  // ! [20:20] SPEC_CTRL virtualization
  //   [21:21] ROGPT: Read-Only Guest Page Table feature support
  //   [22:22] Reserved
  //   [23:23] HOST MCE Override
  //   [24:24] TLBiCntrl: INVLPGB/TLBSYNC hypervisor enable in VMCB and TLBSYNC intercept support
  //   [25:25] NMI Virtualization
  //   [26:26] IBS Virtualization
  //   [27:27] Extended Virtual APIC Register Accesses
  // * [28:28] NestedVirtVmcbAddrChk: Guest VMCB Address Check againt SMM_ADDR/SMM_MASK (C001_0112h/C001_0113h), the MSRs are implemented
  //   [29:29] Bus Lock Threshold
  //   [30:30] Idle HLT intercept
  //   [31:31] Reserved
  return BX_CPUID_SVM_NESTED_PAGING |
//       BX_CPUID_SVM_LBR_VIRTUALIZATION | // not implemented yet
         BX_CPUID_SVM_SVM_LOCK |
         BX_CPUID_SVM_NRIP_SAVE |
         BX_CPUID_SVM_TSCRATE |
//       BX_CPUID_SVM_VMCB_CLEAN_BITS | // not implemented yet
         BX_CPUID_SVM_FLUSH_BY_ASID |
         BX_CPUID_SVM_DECODE_ASSIST |
         BX_CPUID_SVM_PAUSE_FILTER |
         BX_CPUID_SVM_PAUSE_FILTER_THRESHOLD |
         BX_CPUID_SVM_AVIC |
         BX_CPUID_SVM_NESTED_VIRTUALIZATION |
         BX_CPUID_SVM_VIRTUAL_GIF |
//       BX_CPUID_SVM_GMET | // not implemented yet
         BX_CPUID_SVM_SSS_CHECK |
         BX_CPUID_SVM_SPEC_CTRL_VIRT |
         BX_CPUID_SVM_NESTED_VMCB_ADDR_CHECK;
}
#endif

#if BX_SUPPORT_SVM

// leaf 0x8000000A : SVM //
void zen3_vermeer_t::get_ext_cpuid_leaf_A(cpuid_function_t *leaf) const
{
  leaf->eax = 0x01;     /* SVM revision ID */
  leaf->ebx = 0x8000;   /* number of ASIDs */
  leaf->ecx = 0;
  leaf->edx = get_svm_extensions_bitmask();
}

#endif

bx_cpuid_t *create_zen3_vermeer_cpuid(BX_CPU_C *cpu) { return new zen3_vermeer_t(cpu); }

#endif
