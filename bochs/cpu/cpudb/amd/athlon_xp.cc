/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2025-2026 Stanislav Shwartsman
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
#include "athlon_xp.h"

#define LOG_THIS cpu->

#if BX_CPU_LEVEL >= 6

// CPUID dump of the real CPU (taken from athlon_xp.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000001, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x00000001 */ { 0x00000680, 0x00000000, 0x00000000, 0x0383FBFF }
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000008, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x80000001 */ { 0x00000780, 0x00000000, 0x00000000, 0xC1C3FBFF },
  /* leaf 0x80000002 */ { 0x20444D41, 0x6C687441, 0x74286E6F, 0x5820296D },
  /* leaf 0x80000003 */ { 0x32322050, 0x002B3030, 0x00000000, 0x00000000 },
  /* leaf 0x80000004 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000005 */ { 0x0408FF08, 0xFF20FF10, 0x40020140, 0x40020140 },
  /* leaf 0x80000006 */ { 0x00000000, 0x41004100, 0x01008140, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000001 },
  /* leaf 0x80000008 */ { 0x00002022, 0x00000000, 0x00000000, 0x00000000 }
};

athlon_xp_t::athlon_xp_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x1, 0x80000008);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs + 0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs + 1);

  cpuid_table->register_leaf(0x80000000, 1, ext_leafs + 0);
  cpuid_table->register_leaf(0x80000001, 1, ext_leafs + 1);
  cpuid_table->register_leaf(0x80000002, 1, ext_leafs + 2);
  cpuid_table->register_leaf(0x80000003, 1, ext_leafs + 3);
  cpuid_table->register_leaf(0x80000004, 1, ext_leafs + 4);
  cpuid_table->register_leaf(0x80000005, 1, ext_leafs + 5);
  cpuid_table->register_leaf(0x80000006, 1, ext_leafs + 6);
  cpuid_table->register_leaf(0x80000007, 1, ext_leafs + 7);
  cpuid_table->register_leaf(0x80000008, 1, ext_leafs + 8);

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_AMD);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
  enable_cpu_extension(BX_ISA_SYSCALL_SYSRET_LEGACY);
#if BX_SUPPORT_3DNOW
  enable_cpu_extension(BX_ISA_3DNOW);
  enable_cpu_extension(BX_ISA_3DNOW_EXT);
#endif
  enable_cpu_extension(BX_ISA_DEBUG_EXTENSIONS);
  enable_cpu_extension(BX_ISA_VME);
  enable_cpu_extension(BX_ISA_PSE);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_PAE);
  enable_cpu_extension(BX_ISA_PGE);
  enable_cpu_extension(BX_ISA_MTRR);
  enable_cpu_extension(BX_ISA_PAT);
  enable_cpu_extension(BX_ISA_SSE);
  enable_cpu_extension(BX_ISA_XAPIC);
}

void athlon_xp_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  static const char* brand_string = "AMD Athlon(tm) XP 2200+\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

  switch(function) {
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
void athlon_xp_t::get_std_cpuid_leaf_0(cpuid_function_t *leaf) const
{
  get_leaf_0(0x1, "AuthenticAMD", leaf);
}

// leaf 0x00000001 //
void athlon_xp_t::get_std_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [19:16] Extended Model
  //   [27:20] Extended Family
  leaf->eax = 0x00000622;

  leaf->ebx = 0;
  leaf->ecx = 0;

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
  //   [19:19] CLFLUSH: CLFLUSH Instruction support
  //   [20:20] Reserved
  //   [21:21] DS: Debug Store
  //   [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  // * [25:25] SSE: SSE Extensions
  //   [26:26] SSE2: SSE2 Extensions
  //   [27:27] Self Snoop
  //   [28:28] Hyper Threading Technology
  //   [29:29] TM: Thermal Monitor
  //   [30:30] Reserved
  //   [31:31] PBE: Pending Break Enable
  leaf->edx = get_std_cpuid_leaf_1_edx();
}

// leaf 0x80000000 //
void athlon_xp_t::get_ext_cpuid_leaf_0(cpuid_function_t *leaf) const
{
  // EAX: highest extended function understood by CPUID
  // EBX: reserved
  // EDX: reserved
  // ECX: reserved
  get_leaf_0(0x80000008, NULL, leaf);
}

// leaf 0x80000001 //
void athlon_xp_t::get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information (same as 0x00000001.EAX)
  leaf->eax = 0x00000722;

  leaf->ebx = 0;
  leaf->ecx = 0;

  // EDX:
  // Many of the bits in EDX are the same as FN 0x00000001 [*] for AMD
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
  //   [20:20] No-Execute page protection
  //   [21:21] Reserved
  // * [22:22] AMD MMX Extensions
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  //   [25:25] Fast FXSAVE/FXRSTOR mode support
  //   [26:26] 1G paging support
  //   [27:27] Support RDTSCP Instruction
  //   [28:28] Reserved
  //   [29:29] Long Mode
  // * [30:30] AMD 3DNow! Extensions
  // * [31:31] AMD 3DNow! Instructions
  leaf->edx = get_ext_cpuid_leaf_1_edx_amd();
}

// leaf 0x80000005 - L1 Cache and TLB Identifiers //
// leaf 0x80000006 - L2 Cache and TLB Identifiers //
// leaf 0x80000007 - Advanced Power Management //
bx_cpuid_t *create_athlon_xp_cpuid(BX_CPU_C *cpu) { return new athlon_xp_t(cpu); }

#endif
