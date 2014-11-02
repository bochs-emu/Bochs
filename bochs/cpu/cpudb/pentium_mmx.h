/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2014 Stanislav Shwartsman
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

#ifndef BX_PENTIUM_MMX_CPUID_DEFINITIONS_H
#define BX_PENTIUM_MMX_CPUID_DEFINITIONS_H

#if BX_CPU_LEVEL >= 5

#include "cpu/cpuid.h"

class pentium_mmx_t : public bx_cpuid_t {
public:
  pentium_mmx_t(BX_CPU_C *cpu);
  virtual ~pentium_mmx_t() {}

  // return CPU name
  virtual const char *get_name(void) const { return "pentium_mmx"; }

  virtual void get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const;

  virtual void dump_cpuid(void) const;

private:
  void get_std_cpuid_leaf_0(cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_1(cpuid_function_t *leaf) const;
};

extern bx_cpuid_t *create_pentium_mmx_cpuid(BX_CPU_C *cpu);

#endif // BX_CPU_LEVEL >= 5 && BX_SUPPORT_X86_64 == 0

#endif
