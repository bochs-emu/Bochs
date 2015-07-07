/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014-2015 Stanislav Shwartsman
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
#include "param_names.h"
#include "cpuid.h"

#define LOG_THIS cpu->
bx_cpuid_t::bx_cpuid_t(BX_CPU_C *_cpu): cpu(_cpu)
{
  init();
}

#if BX_SUPPORT_VMX
bx_cpuid_t::bx_cpuid_t(BX_CPU_C *_cpu, Bit32u vmcs_revision): 
	cpu(_cpu), vmcs_map(vmcs_revision)
{
  init();
}

bx_cpuid_t::bx_cpuid_t(BX_CPU_C *_cpu, Bit32u vmcs_revision, const char *filename): 
	cpu(_cpu), vmcs_map(vmcs_revision, filename)
{
  init();
}
#endif

void bx_cpuid_t::init()
{
#if BX_SUPPORT_SMP
  nthreads = SIM->get_param_num(BXPN_CPU_NTHREADS)->get();
  ncores = SIM->get_param_num(BXPN_CPU_NCORES)->get();
  nprocessors = SIM->get_param_num(BXPN_CPU_NPROCESSORS)->get();
#else
  nthreads = 1;
  ncores = 1;
  nprocessors = 1;
#endif

  for (unsigned n=0; n < BX_ISA_EXTENSIONS_ARRAY_SIZE; n++)
    ia_extensions_bitmask[n] = 0;

  // every cpu supported by Bochs support all 386 and earlier instructions
  ia_extensions_bitmask[0] = (1 << BX_ISA_386);
}

#if BX_SUPPORT_APIC

BX_CPP_INLINE static Bit32u ilog2(Bit32u x)
{
  Bit32u count = 0;
  while(x>>=1) count++;
  return count;
}

// leaf 0x0000000B //
void bx_cpuid_t::get_std_cpuid_extended_topology_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  // CPUID function 0x0000000B - Extended Topology Leaf
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = subfunction;
  leaf->edx = cpu->get_apic_id();

#if BX_SUPPORT_SMP
  switch(subfunction) {
  case 0:
     if (nthreads > 1) {
        leaf->eax = ilog2(nthreads-1)+1;
        leaf->ebx = nthreads;
        leaf->ecx |= (1<<8);
     }
     else if (ncores > 1) {
        leaf->eax = ilog2(ncores-1)+1;
        leaf->ebx = ncores;
        leaf->ecx |= (2<<8);
     }
     else if (nprocessors > 1) {
        leaf->eax = ilog2(nprocessors-1)+1;
        leaf->ebx = nprocessors;
     }
     else {
        leaf->eax = 1;
        leaf->ebx = 1; // number of logical CPUs at this level
     }
     break;

  case 1:
     if (nthreads > 1) {
        if (ncores > 1) {
           leaf->eax = ilog2(ncores-1)+1;
           leaf->ebx = ncores;
           leaf->ecx |= (2<<8);
        }
        else if (nprocessors > 1) {
           leaf->eax = ilog2(nprocessors-1)+1;
           leaf->ebx = nprocessors;
        }
     }
     else if (ncores > 1) {
        if (nprocessors > 1) {
           leaf->eax = ilog2(nprocessors-1)+1;
           leaf->ebx = nprocessors;
        }
     }
     break;

  case 2:
     if (nthreads > 1) {
        if (nprocessors > 1) {
           leaf->eax = ilog2(nprocessors-1)+1;
           leaf->ebx = nprocessors;
        }
     }
     break;

  default:
     break;
  }
#endif
}

#endif

void bx_cpuid_t::get_leaf_0(unsigned max_leaf, const char *vendor_string, cpuid_function_t *leaf) const
{
  // EAX: highest function understood by CPUID
  // EBX: vendor ID string
  // EDX: vendor ID string
  // ECX: vendor ID string
  leaf->eax = max_leaf;

  if (vendor_string == NULL) {
    leaf->ebx = 0;
    leaf->ecx = 0; // Reserved
    leaf->edx = 0;
    return;
  }

  // CPUID vendor string (e.g. GenuineIntel, AuthenticAMD, CentaurHauls, ...)
  memcpy(&(leaf->ebx), vendor_string,     4);
  memcpy(&(leaf->edx), vendor_string + 4, 4);
  memcpy(&(leaf->ecx), vendor_string + 8, 4);
#ifdef BX_BIG_ENDIAN
  leaf->ebx = bx_bswap32(leaf->ebx);
  leaf->ecx = bx_bswap32(leaf->ecx);
  leaf->edx = bx_bswap32(leaf->edx);
#endif
}

void bx_cpuid_t::get_ext_cpuid_brand_string_leaf(const char *brand_string, Bit32u function, cpuid_function_t *leaf) const
{
  switch(function) {
  case 0x80000002:
    memcpy(&(leaf->eax), brand_string     , 4);
    memcpy(&(leaf->ebx), brand_string +  4, 4);
    memcpy(&(leaf->ecx), brand_string +  8, 4);
    memcpy(&(leaf->edx), brand_string + 12, 4);
    break;
  case 0x80000003:
    memcpy(&(leaf->eax), brand_string + 16, 4);
    memcpy(&(leaf->ebx), brand_string + 20, 4);
    memcpy(&(leaf->ecx), brand_string + 24, 4);
    memcpy(&(leaf->edx), brand_string + 28, 4);
    break;
  case 0x80000004:
    memcpy(&(leaf->eax), brand_string + 32, 4);
    memcpy(&(leaf->ebx), brand_string + 36, 4);
    memcpy(&(leaf->ecx), brand_string + 40, 4);
    memcpy(&(leaf->edx), brand_string + 44, 4);
    break;
  default:
    break;
  }

#ifdef BX_BIG_ENDIAN
  leaf->eax = bx_bswap32(leaf->eax);
  leaf->ebx = bx_bswap32(leaf->ebx);
  leaf->ecx = bx_bswap32(leaf->ecx);
  leaf->edx = bx_bswap32(leaf->edx);
#endif
}

void bx_cpuid_t::get_cpuid_hidden_level(cpuid_function_t *leaf, const char *magic_string) const
{
  memcpy(&(leaf->eax), magic_string     , 4);
  memcpy(&(leaf->ebx), magic_string +  4, 4);
  memcpy(&(leaf->ecx), magic_string +  8, 4);
  memcpy(&(leaf->edx), magic_string + 12, 4);

#ifdef BX_BIG_ENDIAN
  leaf->eax = bx_bswap32(leaf->eax);
  leaf->ebx = bx_bswap32(leaf->ebx);
  leaf->ecx = bx_bswap32(leaf->ecx);
  leaf->edx = bx_bswap32(leaf->edx);
#endif
}

void bx_cpuid_t::dump_cpuid_leaf(unsigned function, unsigned subfunction) const
{
  struct cpuid_function_t leaf;
  get_cpuid_leaf(function, subfunction, &leaf);
  BX_INFO(("CPUID[0x%08x]: %08x %08x %08x %08x", function, leaf.eax, leaf.ebx, leaf.ecx, leaf.edx));
}

void bx_cpuid_t::dump_cpuid(unsigned max_std_leaf, unsigned max_ext_leaf) const
{
  for (unsigned std_leaf=0; std_leaf<=max_std_leaf; std_leaf++) {
    dump_cpuid_leaf(std_leaf);
  }

  if (max_ext_leaf == 0) return;

  for (unsigned ext_leaf=0x80000000; ext_leaf<=(0x8000000 + max_ext_leaf); ext_leaf++) {
    dump_cpuid_leaf(ext_leaf);
  }
}

