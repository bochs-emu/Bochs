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

#ifndef BX_GENERIC_CPUID_DEFINITIONS_H
#define BX_GENERIC_CPUID_DEFINITIONS_H

#if BX_CPU_LEVEL >= 4

#include "cpuid.h"

class bx_generic_cpuid_t : public bx_cpuid_t {
public:
  bx_generic_cpuid_t(BX_CPU_C *cpu);
  virtual ~bx_generic_cpuid_t() {}

  // return CPU name
  virtual const char *get_name(void) const { return "bochs"; }

#if BX_SUPPORT_VMX
  virtual Bit32u get_vmx_extensions_bitmask(void) const { return vmx_extensions_bitmask; }
#endif
#if BX_SUPPORT_SVM
  virtual Bit32u get_svm_extensions_bitmask(void) const { return svm_extensions_bitmask; }
#endif

  virtual void get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const;

  virtual void dump_cpuid(void) const;

private:
  void init_cpu_extensions_bitmask(void);
#if BX_SUPPORT_VMX
  void init_vmx_extensions_bitmask(void);
#endif
#if BX_SUPPORT_SVM
  void init_svm_extensions_bitmask(void);
#endif

#if BX_SUPPORT_VMX
  Bit32u vmx_extensions_bitmask;
#endif
#if BX_SUPPORT_SVM
  Bit32u svm_extensions_bitmask;
#endif

  unsigned max_std_leaf;
  unsigned max_ext_leaf;

  void get_std_cpuid_leaf_0(cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_1(cpuid_function_t *leaf) const;
#if BX_CPU_LEVEL >= 6
  void get_std_cpuid_leaf_2(cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_4(Bit32u subfunction, cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_5(cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_6(cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_7(Bit32u subfunction, cpuid_function_t *leaf) const;
  void get_std_cpuid_leaf_A(cpuid_function_t *leaf) const;
  void get_std_cpuid_extended_topology_leaf(Bit32u subfunction, cpuid_function_t *leaf) const;
  void get_std_cpuid_xsave_leaf(Bit32u subfunction, cpuid_function_t *leaf) const;

  void get_ext_cpuid_leaf_0(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_5(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_6(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_7(cpuid_function_t *leaf) const;
  void get_ext_cpuid_leaf_8(cpuid_function_t *leaf) const;
#if BX_SUPPORT_SVM
  void get_ext_cpuid_leaf_A(cpuid_function_t *leaf) const;
#endif

  Bit32u get_std2_cpuid_features(void) const;
  Bit32u get_ext2_cpuid_features(void) const;
  Bit32u get_ext3_cpuid_features(void) const;
  Bit32u get_extended_cpuid_features(void) const;
#endif

  Bit32u get_cpu_version_information(void) const;
  Bit32u get_std_cpuid_features(void) const;
};

extern bx_cpuid_t *create_bx_generic_cpuid(BX_CPU_C *cpu);

#endif // BX_CPU_LEVEL >= 4

#endif
