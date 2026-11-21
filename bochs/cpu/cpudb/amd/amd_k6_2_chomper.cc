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
#include "amd_k6_2_chomper.h"

#define LOG_THIS cpu->

#if BX_CPU_LEVEL >= 5

// CPUID dump of the real CPU (taken from amd_k6_2_chomper.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000001, 0x68747541, 0x444D4163, 0x69746E65 },
  /* leaf 0x00000001 */ { 0x00000580, 0x00000000, 0x00000000, 0x008001BF }
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000005, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000680, 0x00000000, 0x00000000, 0x808009BF },
  /* leaf 0x80000002 */ { 0x2D444D41, 0x7428364B, 0x3320296D, 0x72702044 },
  /* leaf 0x80000003 */ { 0x7365636F, 0x00726F73, 0x00000000, 0x00000000 },
  /* leaf 0x80000004 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000005 */ { 0x00000000, 0x02800140, 0x20020220, 0x20020220 }
};

amd_k6_2_chomper_t::amd_k6_2_chomper_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x1, 0x80000005);
  set_vendor(AuthenticAMD);
  set_magic_string("NexGenerationAMD");

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs + 0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs + 1);

  cpuid_table->register_leaf(0x80000000, 1, ext_leafs + 0);
  cpuid_table->register_leaf(0x80000001, 1, ext_leafs + 1);
  cpuid_table->register_leaf(0x80000002, 1, ext_leafs + 2);
  cpuid_table->register_leaf(0x80000003, 1, ext_leafs + 3);
  cpuid_table->register_leaf(0x80000004, 1, ext_leafs + 4);
  cpuid_table->register_leaf(0x80000005, 1, ext_leafs + 5);

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_AMD);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_SYSCALL_SYSRET_LEGACY);
#if BX_SUPPORT_3DNOW
  enable_cpu_extension(BX_ISA_3DNOW);
#endif
  enable_cpu_extension(BX_ISA_DEBUG_EXTENSIONS);
  enable_cpu_extension(BX_ISA_VME);
  enable_cpu_extension(BX_ISA_PSE);
}

unsigned amd_k6_2_chomper_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  return cpuid_table->get_leaf(function, subfunction, leaf);
}

// leaf 0x00000000 //
// leaf 0x00000001 //
// leaf 0x80000000 //
// leaf 0x80000001 //
// leaf 0x80000005 - L1 Cache and TLB Identifiers //
bx_cpuid_t *create_amd_k6_2_chomper_cpuid(BX_CPU_C *cpu) { return new amd_k6_2_chomper_t(cpu); }

#endif
