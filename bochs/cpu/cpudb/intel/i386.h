/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024 Stanislav Shwartsman
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

#ifndef BX_386_CPUID_DEFINITIONS_H
#define BX_386_CPUID_DEFINITIONS_H

#include <assert.h>

#include "cpu/cpuid.h"

class i386_t : public bx_cpuid_t {
public:
  i386_t(BX_CPU_C *cpu): bx_cpuid_t(cpu) {
    enable_cpu_extension(BX_ISA_X87);
  }
  virtual ~i386_t() {}

  // return CPU name
  virtual const char *get_name(void) const { return "i386"; }

  virtual void get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const { assert(0); }

  virtual void dump_cpuid(void) const {}

private:
  void get_std_cpuid_leaf_1(cpuid_function_t *leaf) const;
};

bx_cpuid_t *create_i386_cpuid(BX_CPU_C *cpu) { return new i386_t(cpu); }

#endif
