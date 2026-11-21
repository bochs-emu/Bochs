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
#include "atom_n270.h"

#define LOG_THIS cpu->

#if BX_CPU_LEVEL >= 6

// CPUID dump of the real CPU (taken from atom_n270.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x0000000A, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x000106C2, 0x00020800, 0x0040C39D, 0xBFE9FBFF },
  /* leaf 0x00000002 */ { 0x4FBA5901, 0x0E3080C0, 0x00000000, 0x00000000 },
  /* leaf 0x00000003 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000004 */ { 0x00004121, 0x0140003F, 0x0000003F, 0x00000001 }, // [SL 0]
  /* leaf 0x00000004 */ { 0x00004122, 0x01C0003F, 0x0000003F, 0x00000001 }, // [SL 1]
  /* leaf 0x00000004 */ { 0x00004143, 0x01C0003F, 0x000003FF, 0x00000001 }, // [SL 2]
  /* leaf 0x00000005 */ { 0x00000040, 0x00000040, 0x00000003, 0x00020220 },
  /* leaf 0x00000006 */ { 0x00000001, 0x00000002, 0x00000001, 0x00000000 },
  /* leaf 0x00000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000008 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000A */ { 0x07280203, 0x00000000, 0x00000000, 0x00002501 }
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000008, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000000, 0x00000000, 0x00000001, 0x00000000 },
  /* leaf 0x80000002 */ { 0x20202020, 0x20202020, 0x746E4920, 0x52286C65 },
  /* leaf 0x80000003 */ { 0x74412029, 0x54286D6F, 0x4320294D, 0x4E205550 },
  /* leaf 0x80000004 */ { 0x20303732, 0x20402020, 0x30362E31, 0x007A4847 },
  /* leaf 0x80000005 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000006 */ { 0x00000000, 0x00000000, 0x02008040, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000008 */ { 0x00002020, 0x00000000, 0x00000000, 0x00000000 }
};

atom_n270_t::atom_n270_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0xA, 0x80000008);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs +  0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs +  1);
  cpuid_table->register_leaf(0x00000002, 1, std_leafs +  2);
  cpuid_table->register_leaf(0x00000003, 1, std_leafs +  3);
  cpuid_table->register_leaf(0x00000004, 3, std_leafs +  4);
  cpuid_table->register_leaf(0x00000005, 1, std_leafs +  7);
  cpuid_table->register_leaf(0x00000006, 1, std_leafs +  8);
  cpuid_table->register_leaf(0x00000007, 1, std_leafs +  9);
  cpuid_table->register_leaf(0x00000008, 1, std_leafs + 10);
  cpuid_table->register_leaf(0x00000009, 1, std_leafs + 11);
  cpuid_table->register_leaf(0x0000000A, 1, std_leafs + 12);

  cpuid_table->register_leaf(0x80000000, 1, ext_leafs +  0);
  cpuid_table->register_leaf(0x80000001, 1, ext_leafs +  1);
  cpuid_table->register_leaf(0x80000002, 1, ext_leafs +  2);
  cpuid_table->register_leaf(0x80000003, 1, ext_leafs +  3);
  cpuid_table->register_leaf(0x80000004, 1, ext_leafs +  4);
  cpuid_table->register_leaf(0x80000005, 1, ext_leafs +  5);
  cpuid_table->register_leaf(0x80000006, 1, ext_leafs +  6);
  cpuid_table->register_leaf(0x80000007, 1, ext_leafs +  7);
  cpuid_table->register_leaf(0x80000008, 1, ext_leafs +  8);

  if (! BX_SUPPORT_MONITOR_MWAIT)
    BX_INFO(("WARNING: MONITOR/MWAIT support is not compiled in !"));

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
  enable_cpu_extension(BX_ISA_SSE);
  enable_cpu_extension(BX_ISA_SSE2);
  enable_cpu_extension(BX_ISA_SSE3);
  enable_cpu_extension(BX_ISA_SSSE3);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_MONITOR_MWAIT);
#endif
  enable_cpu_extension(BX_ISA_CLFLUSH);
  enable_cpu_extension(BX_ISA_DEBUG_EXTENSIONS);
  enable_cpu_extension(BX_ISA_VME);
  enable_cpu_extension(BX_ISA_PSE);
  enable_cpu_extension(BX_ISA_PAE);
  enable_cpu_extension(BX_ISA_PGE);
  enable_cpu_extension(BX_ISA_MTRR);
  enable_cpu_extension(BX_ISA_PAT);
  enable_cpu_extension(BX_ISA_XAPIC);
  enable_cpu_extension(BX_ISA_MOVBE);
}

unsigned atom_n270_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  return cpuid_table->get_leaf(function, subfunction, leaf);
}

bx_cpuid_t *create_atom_n270_cpuid(BX_CPU_C *cpu) { return new atom_n270_t(cpu); }

#endif
