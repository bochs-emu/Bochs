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
#include "p4_willamette.h"

#define LOG_THIS cpu->

#if BX_CPU_LEVEL >= 6

// CPUID dump of the real CPU (taken from p4_willamette.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000002, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x00000F12, 0x00010808, 0x00000000, 0x3FEBFBFF },
  /* leaf 0x00000002 */ { 0x665B5001, 0x00000000, 0x00000000, 0x007A7040 }
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000004, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000002 */ { 0x20202020, 0x20202020, 0x20202020, 0x6E492020 },
  /* leaf 0x80000003 */ { 0x286C6574, 0x50202952, 0x69746E65, 0x52286D75 },
  /* leaf 0x80000004 */ { 0x20342029, 0x20555043, 0x30382E31, 0x007A4847 }
};

p4_willamette_t::p4_willamette_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x2, 0x80000004);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs + 0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs + 1);
  cpuid_table->register_leaf(0x00000002, 1, std_leafs + 2);

  cpuid_table->register_leaf(0x80000000, 1, ext_leafs + 0);
  cpuid_table->register_leaf(0x80000001, 1, ext_leafs + 1);
  cpuid_table->register_leaf(0x80000002, 1, ext_leafs + 2);
  cpuid_table->register_leaf(0x80000003, 1, ext_leafs + 3);
  cpuid_table->register_leaf(0x80000004, 1, ext_leafs + 4);

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
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
}

void p4_willamette_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  cpuid_table->get_leaf(function, subfunction, leaf);
}

// leaf 0x00000000 //

// leaf 0x00000001 //
// leaf 0x00000002 - Cache and TLB Descriptors //

// leaf 0x80000000 //
// leaf 0x80000001 //
// leaf 0x80000002 //
// leaf 0x80000003 //
// leaf 0x80000004 //

bx_cpuid_t *create_p4_willamette_cpuid(BX_CPU_C *cpu) { return new p4_willamette_t(cpu); }

#endif
