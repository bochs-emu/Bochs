/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2017 Stanislav Shwartsman
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

#ifndef BX_CORE_DUO_T2400_YONAH_CPUID_DEFINITIONS_H
#define BX_CORE_DUO_T2400_YONAH_CPUID_DEFINITIONS_H

#if BX_CPU_LEVEL >= 6

#include "cpu/cpuid.h"

class core_duo_t2400_yonah_t : public bx_cpuid_t {
public:
  core_duo_t2400_yonah_t(BX_CPU_C *cpu);
  virtual ~core_duo_t2400_yonah_t() {}

  // return CPU name
  virtual const char *get_name(void) const { return "core_duo_t2400_yonah"; }

#if BX_SUPPORT_VMX
  virtual Bit32u get_vmx_extensions_bitmask(void) const { return 0; } // only basic support
#endif

  virtual void get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const;

  virtual void dump_cpuid(void) const;

private:
  Bit32u max_std_leaf;
  Bit32u max_ext_leaf;

  void get_std_cpuid_leaf_1(cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_2(cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_4(Bit32u subfunction, cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_A(cpuid_function_t *leaf) const;

  void get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const;
};

extern bx_cpuid_t *create_core_duo_t2400_yonah_cpuid(BX_CPU_C *cpu);

#endif // BX_CPU_LEVEL >= 6

#endif
