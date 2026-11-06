/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2026 Stanislav Shwartsman
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
#include "athlon64_venice.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64

// CPUID dump of the real CPU (taken from athlon64_venice.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000001, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x00000001 */ { 0x00020FF2, 0x00000800, 0x00000001, 0x078BFBFF }
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000018, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x80000001 */ { 0x00020FF2, 0x00000108, 0x00000001, 0xE3D3FBFF },
  /* leaf 0x80000002 */ { 0x20444D41, 0x6C687441, 0x74286E6F, 0x3620296D },
  /* leaf 0x80000003 */ { 0x72502034, 0x7365636F, 0x20726F73, 0x30303033 },
  /* leaf 0x80000004 */ { 0x0000002B, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000005 */ { 0xFF08FF08, 0xFF20FF20, 0x40020140, 0x40020140 },
  /* leaf 0x80000006 */ { 0x00000000, 0x42004200, 0x02008140, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x0000003F },
  /* leaf 0x80000008 */ { 0x00003028, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x8000000A */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
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
  /* leaf 0x80000018 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
};

athlon64_venice_t::athlon64_venice_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x1, 0x80000018);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs + 0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs + 1);

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

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_AMD);
  enable_cpu_extension(BX_ISA_MMX);
#if BX_SUPPORT_3DNOW
  enable_cpu_extension(BX_ISA_3DNOW);
  enable_cpu_extension(BX_ISA_3DNOW_EXT);
#endif
  enable_cpu_extension(BX_ISA_SYSCALL_SYSRET_LEGACY);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_SSE);
  enable_cpu_extension(BX_ISA_SSE2);
  enable_cpu_extension(BX_ISA_SSE3);
  enable_cpu_extension(BX_ISA_CLFLUSH);
  enable_cpu_extension(BX_ISA_DEBUG_EXTENSIONS);
  enable_cpu_extension(BX_ISA_VME);
  enable_cpu_extension(BX_ISA_PSE);
  enable_cpu_extension(BX_ISA_PAE);
  enable_cpu_extension(BX_ISA_PGE);
  enable_cpu_extension(BX_ISA_MTRR);
  enable_cpu_extension(BX_ISA_PAT);
  enable_cpu_extension(BX_ISA_XAPIC);
  enable_cpu_extension(BX_ISA_LONG_MODE);
  enable_cpu_extension(BX_ISA_LM_LAHF_SAHF);
  enable_cpu_extension(BX_ISA_NX);
  enable_cpu_extension(BX_ISA_FFXSR);
}

void athlon64_venice_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  static const char* brand_string = "AMD Athlon(tm) 64 Processor 3000+\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
  static const char* magic_string = "IT'S HAMMER TIME";

  switch(function) {
  case 0x8FFFFFFF:
    get_cpuid_hidden_level(leaf, magic_string);
    return;
  case 0x80000000:
    get_ext_cpuid_leaf_0(leaf);
    return;
  case 0x80000001:
    get_ext_cpuid_leaf_1(leaf);
    return;
  case 0x80000002:
  case 0x80000003:
  case 0x80000004:
    get_ext_cpuid_brand_string_leaf(brand_string, function, leaf);
    return;
  case 0x80000005: // CPUID leaf 0x80000005 - L1 Cache and TLB Identifiers
  case 0x80000006: // CPUID leaf 0x80000006 - L2 Cache and TLB Identifiers
  case 0x80000007: // CPUID leaf 0x80000007 - Advanced Power Management
    cpuid_table->get_ext_leaf(function, 0, leaf);
    return;
  case 0x80000008:
    get_ext_cpuid_leaf_8(leaf);
    return;
  case 0x00000000:
    get_std_cpuid_leaf_0(leaf);
    return;
  case 0x00000001:
    get_std_cpuid_leaf_1(leaf);
    return;
  default:
    get_reserved_leaf(leaf);
    return;
  }
}

// leaf 0x00000000 //
void athlon64_venice_t::get_std_cpuid_leaf_0(cpuid_function_t *leaf) const
{
  get_leaf_0(0x1, "AuthenticAMD", leaf);
}

// leaf 0x00000001 //
void athlon64_venice_t::get_std_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [19:16] Extended Model
  //   [27:20] Extended Family
  leaf->eax = 0x00020FF2;

  // EBX:
  //   [7:0]   Brand ID
  //   [15:8]  CLFLUSH cache line size (value*8 = cache line size in bytes)
  //   [23:16] Number of logical processors in one physical processor
  //   [31:24] Local Apic ID

  leaf->ebx = ((CACHE_LINE_SIZE / 8) << 8);
#if BX_SUPPORT_APIC
  leaf->ebx |= ((cpu->get_apic_id() & 0xff) << 24);
#endif

  // ECX: Extended Feature Flags
  // * [0:0]   SSE3: SSE3 Instructions
  //   [1:1]   PCLMULQDQ Instruction support
  //   [2:2]   DTES64: 64-bit DS area
  //   [3:3]   MONITOR/MWAIT support
  //   [4:4]   DS-CPL: CPL qualified debug store
  //   [5:5]   VMX: Virtual Machine Technology
  //   [6:6]   SMX: Secure Virtual Machine Technology
  //   [7:7]   EST: Enhanced Intel SpeedStep Technology
  //   [8:8]   TM2: Thermal Monitor 2
  //   [9:9]   SSSE3: SSSE3 Instructions
  //   [10:10] CNXT-ID: L1 context ID
  //   [11:11] reserved
  //   [12:12] FMA Instructions support
  //   [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  //   [14:14] xTPR update control
  //   [15:15] PDCM - Perfmon and Debug Capability MSR
  //   [16:16] reserved
  //   [17:17] PCID: Process Context Identifiers
  //   [18:18] DCA - Direct Cache Access
  //   [19:19] SSE4.1 Instructions
  //   [20:20] SSE4.2 Instructions
  //   [21:21] X2APIC
  //   [22:22] MOVBE instruction
  //   [23:23] POPCNT instruction
  //   [24:24] TSC Deadline
  //   [25:25] AES Instructions
  //   [26:26] XSAVE extensions support
  //   [27:27] OSXSAVE support
  //   [28:28] AVX extensions support
  //   [29:29] AVX F16C - Float16 conversion support
  //   [30:30] RDRAND instruction
  //   [31:31] reserved
  leaf->ecx = get_std_cpuid_leaf_1_ecx();

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
  //   [21:21] DS: Debug Store
  //   [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  // * [25:25] SSE: SSE Extensions
  // * [26:26] SSE2: SSE2 Extensions
  //   [27:27] Self Snoop
  //   [28:28] Hyper Threading Technology
  //   [29:29] TM: Thermal Monitor
  //   [30:30] Reserved
  //   [31:31] PBE: Pending Break Enable
  leaf->edx = get_std_cpuid_leaf_1_edx();
}

// leaf 0x80000000 //
void athlon64_venice_t::get_ext_cpuid_leaf_0(cpuid_function_t *leaf) const
{
  get_leaf_0(0x80000018, "AuthenticAMD", leaf);
}

// leaf 0x80000001 //
void athlon64_venice_t::get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information (same as 0x00000001.EAX)
  leaf->eax = 0x00020FF2;

  // EBX:       Brand ID
  leaf->ebx = 0x00000108;

  // ECX:
  // * [0:0]   LAHF/SAHF instructions support in 64-bit mode
  //   [1:1]   CMP_Legacy: Core multi-processing legacy mode (AMD)
  //   [2:2]   SVM: Secure Virtual Machine (AMD)
  //   [3:3]   Extended APIC Space
  //   [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
  //   [5:5]   LZCNT: LZCNT instruction support
  //   [6:6]   SSE4A: SSE4A Instructions support
  //   [7:7]   Misaligned SSE support
  //   [8:8]   PREFETCHW: PREFETCHW instruction support
  //   [9:9]   OSVW: OS visible workarounds (AMD)
  //   [11:10] reserved
  //   [12:12] SKINIT support
  //   [13:13] WDT: Watchdog timer support
  //   [31:14] reserved
  leaf->ecx = BX_CPUID_EXT1_ECX_LAHF_SAHF;

  // EDX:
  // Many of the bits in EDX are the same as FN 0x00000001 for AMD
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
  // * [11:11] SYSCALL/SYSRET support
  // * [12:12] MTRR: Memory Type Range Reg
  // * [13:13] PGE/PTE Global Bit
  // * [14:14] MCA: Machine Check Architecture
  // * [15:15] CMOV: Cond Mov/Cmp Instructions
  // * [16:16] PAT: Page Attribute Table
  // * [17:17] PSE-36: Physical Address Extensions
  //   [18:18] Reserved
  //   [19:19] Reserved
  // * [20:20] No-Execute page protection
  //   [21:21] Reserved
  // * [22:22] AMD MMX Extensions
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  // * [25:25] Fast FXSAVE/FXRSTOR mode support
  //   [26:26] 1G paging support
  //   [27:27] Support RDTSCP Instruction
  //   [28:28] Reserved
  // * [29:29] Long Mode
  // * [30:30] AMD 3DNow! Extensions
  // * [31:31] AMD 3DNow! Instructions
  leaf->edx = get_ext_cpuid_leaf_1_edx_amd();
}

// leaf 0x80000002 //
// leaf 0x80000003 //
// leaf 0x80000004 //

// leaf 0x80000005 - L1 Cache and TLB Identifiers //
// leaf 0x80000006 - L2 Cache and TLB Identifiers //
// leaf 0x80000007 - Advanced Power Management //
// leaf 0x80000008 //
// leaf 0x80000009             : Reserved //
// leaf 0x8000000A             : SVM      //
// leaf 0x8000000B - 0x80000018: Reserved //

void athlon64_venice_t::dump_cpuid(void) const
{
  bx_cpuid_t::dump_cpuid(0x1, 0x80000018);
  dump_cpuid_leaf(0x8fffffff);
}

bx_cpuid_t *create_athlon64_venice_cpuid(BX_CPU_C *cpu) { return new athlon64_venice_t(cpu); }

#endif
