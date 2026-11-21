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
#include "corei5_lynnfield_750.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64

// CPUID dump of the real CPU (taken from corei5_lynnfield_750.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x0000000B, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x000106E5, 0x00100800, 0x0098E3FD, 0xBFEBFBFF },
  /* leaf 0x00000002 */ { 0x55035A01, 0x00F0B0E4, 0x00000000, 0x09CA212C },
  /* leaf 0x00000003 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000004 */ { 0x1C004121, 0x01C0003F, 0x0000003F, 0x00000000 }, // [SL 00]
  /* leaf 0x00000004 */ { 0x1C004122, 0x00C0003F, 0x0000007F, 0x00000000 }, // [SL 01]
  /* leaf 0x00000004 */ { 0x1C004143, 0x01C0003F, 0x000001FF, 0x00000000 }, // [SL 02]
  /* leaf 0x00000004 */ { 0x1C03C163, 0x03C0003F, 0x00001FFF, 0x00000002 }, // [SL 03]
  /* leaf 0x00000005 */ { 0x00000040, 0x00000040, 0x00000003, 0x00001120 },
  /* leaf 0x00000006 */ { 0x00000003, 0x00000002, 0x00000001, 0x00000000 },
  /* leaf 0x00000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000008 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000A */ { 0x07300403, 0x00000044, 0x00000000, 0x00000603 },
  /* leaf 0x0000000B */ { 0x00000001, 0x00000001, 0x00000100, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000B */ { 0x00000004, 0x00000004, 0x00000201, 0x00000000 }  // [SL 01]
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000008, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000000, 0x00000000, 0x00000001, 0x28100000 },
  /* leaf 0x80000002 */ { 0x65746E49, 0x2952286C, 0x726F4320, 0x4D542865 },
  /* leaf 0x80000003 */ { 0x35692029, 0x55504320, 0x20202020, 0x20202020 },
  /* leaf 0x80000004 */ { 0x30353720, 0x20402020, 0x37362E32, 0x007A4847 },
  /* leaf 0x80000005 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000006 */ { 0x00000000, 0x00000000, 0x01006040, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000100 },
  /* leaf 0x80000008 */ { 0x00003024, 0x00000000, 0x00000000, 0x00000000 }
};

corei5_lynnfield_750_t::corei5_lynnfield_750_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0xB, 0x80000008);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs +  0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs +  1);
  cpuid_table->register_leaf(0x00000002, 1, std_leafs +  2);
  cpuid_table->register_leaf(0x00000003, 1, std_leafs +  3);
  cpuid_table->register_leaf(0x00000004, 4, std_leafs +  4);
  cpuid_table->register_leaf(0x00000005, 1, std_leafs +  8);
  cpuid_table->register_leaf(0x00000006, 1, std_leafs +  9);
  cpuid_table->register_leaf(0x00000007, 1, std_leafs + 10);
  cpuid_table->register_leaf(0x00000008, 1, std_leafs + 11);
  cpuid_table->register_leaf(0x00000009, 1, std_leafs + 12);
  cpuid_table->register_leaf(0x0000000A, 1, std_leafs + 13);
  cpuid_table->register_leaf(0x0000000B, 2, std_leafs + 14);

  cpuid_table->register_leaf(0x80000000, 1, ext_leafs + 0);
  cpuid_table->register_leaf(0x80000001, 1, ext_leafs + 1);
  cpuid_table->register_leaf(0x80000002, 1, ext_leafs + 2);
  cpuid_table->register_leaf(0x80000003, 1, ext_leafs + 3);
  cpuid_table->register_leaf(0x80000004, 1, ext_leafs + 4);
  cpuid_table->register_leaf(0x80000005, 1, ext_leafs + 5);
  cpuid_table->register_leaf(0x80000006, 1, ext_leafs + 6);
  cpuid_table->register_leaf(0x80000007, 1, ext_leafs + 7);
  cpuid_table->register_leaf(0x80000008, 1, ext_leafs + 8);

  if (BX_SUPPORT_VMX == 1)
    BX_INFO(("You must compile with --enable-vmx=2 for Intel Core i5 750 (Lynnfield) VMX configuration"));

  if (! BX_SUPPORT_MONITOR_MWAIT)
    BX_INFO(("WARNING: MONITOR/MWAIT support is not compiled in !"));

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_MSR);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT_LONGMODE);
  enable_cpu_extension(BX_ISA_SSE);
  enable_cpu_extension(BX_ISA_SSE2);
  enable_cpu_extension(BX_ISA_SSE3);
  enable_cpu_extension(BX_ISA_SSSE3);
  enable_cpu_extension(BX_ISA_SSE4_1);
  enable_cpu_extension(BX_ISA_SSE4_2);
  enable_cpu_extension(BX_ISA_POPCNT);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_MONITOR_MWAIT);
#endif
#if BX_SUPPORT_VMX >= 2
  enable_cpu_extension(BX_ISA_VMX);
#endif
  enable_cpu_extension(BX_ISA_SMX);
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
  enable_cpu_extension(BX_ISA_LM_LAHF_SAHF);
  enable_cpu_extension(BX_ISA_NX);
  enable_cpu_extension(BX_ISA_CMPXCHG16B);
  enable_cpu_extension(BX_ISA_RDTSCP);
}

unsigned corei5_lynnfield_750_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  return cpuid_table->get_leaf(function, subfunction, leaf);
}

#if BX_SUPPORT_VMX >= 2

Bit32u corei5_lynnfield_750_t::get_vmx_extensions_bitmask(void) const
{
  return BX_VMX_TPR_SHADOW |
         BX_VMX_VIRTUAL_NMI |
         BX_VMX_APIC_VIRTUALIZATION |
         BX_VMX_WBINVD_VMEXIT |
         BX_VMX_MONITOR_TRAP_FLAG |
         BX_VMX_VPID |
         BX_VMX_EPT |
         BX_VMX_SAVE_DEBUGCTL_DISABLE |
         BX_VMX_PERF_GLOBAL_CTRL |     // MSR not implemented yet
         BX_VMX_PAT |
         BX_VMX_EFER |
         BX_VMX_DESCRIPTOR_TABLE_EXIT |
         BX_VMX_X2APIC_VIRTUALIZATION |
         BX_VMX_PREEMPTION_TIMER;
}

#endif

// leaf 0x00000000 //

// leaf 0x00000001 //
// leaf 0x00000002 - Cache and TLB Descriptors //
// leaf 0x00000003 - Processor Serial Number (not supported) //

// leaf 0x00000004 - Deterministic Cache Parameters //
// leaf 0x00000005 MONITOR/MWAIT Leaf                //
// leaf 0x00000006 Thermal and Power Management Leaf //
// leaf 0x00000007 not supported                     //
// leaf 0x00000008 reserved                          //
// leaf 0x00000009 direct cache access not supported //

// leaf 0x0000000A //

// leaf 0x80000000 //

// leaf 0x80000001 //
// leaf 0x80000002 //
// leaf 0x80000003 //
// leaf 0x80000004 //

// leaf 0x80000005 - L1 Cache and TLB Identifiers (reserved for Intel)
// leaf 0x80000006 - L2 Cache and TLB Identifiers
// leaf 0x80000007 - Advanced Power Management
// leaf 0x80000008 //

bx_cpuid_t *create_corei5_lynnfield_750_cpuid(BX_CPU_C *cpu) { return new corei5_lynnfield_750_t(cpu); }

#endif
