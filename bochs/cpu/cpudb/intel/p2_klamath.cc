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
#include "p2_klamath.h"

#if BX_CPU_LEVEL >= 6

#define LOG_THIS cpu->

// CPUID dump of the real CPU (taken from p2_klamath.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x00000002, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x00000634, 0x00000000, 0x00000000, 0x0080FBFF },
  /* leaf 0x00000002 */ { 0x03020101, 0x00000000, 0x00000000, 0x0C040843 }
};

p2_klamath_t::p2_klamath_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0x2, 0);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs + 0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs + 1);
  cpuid_table->register_leaf(0x00000002, 1, std_leafs + 2);

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
  enable_cpu_extension(BX_ISA_DEBUG_EXTENSIONS);
  enable_cpu_extension(BX_ISA_VME);
  enable_cpu_extension(BX_ISA_PSE);
  enable_cpu_extension(BX_ISA_PAE);
  enable_cpu_extension(BX_ISA_PGE);
  enable_cpu_extension(BX_ISA_MTRR);
}

unsigned p2_klamath_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  return cpuid_table->get_leaf(function, subfunction, leaf);
}

// leaf 0x00000000 //

// leaf 0x00000001 //
// leaf 0x00000002 - Cache and TLB Descriptors //

bx_cpuid_t *create_p2_klamath_cpuid(BX_CPU_C *cpu) { return new p2_klamath_t(cpu); }

#endif
