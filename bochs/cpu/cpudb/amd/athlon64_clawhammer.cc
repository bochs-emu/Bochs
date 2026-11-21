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
#include "athlon64_clawhammer.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64

// CPUID dump of the real CPU (taken from athlon64_clawhammer.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000001, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x00000001 */ { 0x00000F48, 0x00000800, 0x00000000, 0x078BFBFF }
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000018, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x80000001 */ { 0x00000F48, 0x00000106, 0x00000000, 0xE1D3FBFF },
  /* leaf 0x80000002 */ { 0x20444D41, 0x6C687441, 0x74286E6F, 0x3620296D },
  /* leaf 0x80000003 */ { 0x72502034, 0x7365636F, 0x20726F73, 0x30303832 },
  /* leaf 0x80000004 */ { 0x0000002B, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000005 */ { 0xFF08FF08, 0xFF20FF20, 0x40020140, 0x40020140 },
  /* leaf 0x80000006 */ { 0x00000000, 0x42004200, 0x02008140, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x0000000F },
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

athlon64_clawhammer_t::athlon64_clawhammer_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x1, 0x80000018);
  set_vendor(AuthenticAMD);
  set_magic_string("IT'S HAMMER TIME");

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
  enable_cpu_extension(BX_ISA_NX);
}

unsigned athlon64_clawhammer_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  return cpuid_table->get_leaf(function, subfunction, leaf);
}

// leaf 0x00000000 //
// leaf 0x00000001 //
// leaf 0x80000000 //
// leaf 0x80000001 //
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

bx_cpuid_t *create_athlon64_clawhammer_cpuid(BX_CPU_C *cpu) { return new athlon64_clawhammer_t(cpu); }

#endif
