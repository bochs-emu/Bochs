/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014-2026 Stanislav Shwartsman
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
#include "gui/siminterface.h"
#include "param_names.h"
#include "cpuid.h"

static const char *cpu_feature_name[BX_ISA_EXTENSION_LAST] = {
#define x86_feature(isa, feature_name) feature_name,
#include "decoder/features.h"
};
#undef x86_feature

const char *get_cpu_feature_name(unsigned feature) { return cpu_feature_name[feature]; }

int match_cpu_feature(const char *name)
{
  for (int i=0;i < BX_ISA_EXTENSION_LAST; i++)
    if (!strcmp(name, cpu_feature_name[i]))
      return i;

  return -1; // match not found
}

// every leaf never registered in the table points to this reserved leaf with 1 subleaf
static const cpuid_function_t reserved_leaf = { 0, 0, 0, 0 };

bx_cpuid_table_t::bx_cpuid_table_t(Bit32u _max_std_leaf, Bit32u _max_ext_leaf):
    max_std_leaf(_max_std_leaf), max_ext_leaf(_max_ext_leaf)
{
  std_leafs = new const cpuid_function_t* [max_std_leaf + 1];
  std_leafs_num_subleafs = new Bit32u [max_std_leaf + 1];

  for (unsigned n=0; n <= max_std_leaf; n++) {
    std_leafs[n] = &reserved_leaf;
    std_leafs_num_subleafs[n] = 1;
  }

  // models without ext leafs have max_ext_leaf below 0x80000000
  if (max_ext_leaf >= 0x80000000) {
    Bit32u num_ext_leafs = max_ext_leaf - 0x80000000 + 1;

    ext_leafs = new const cpuid_function_t* [num_ext_leafs];
    ext_leafs_num_subleafs = new Bit32u [num_ext_leafs];

    for (unsigned n=0; n < num_ext_leafs; n++) {
      ext_leafs[n] = &reserved_leaf;
      ext_leafs_num_subleafs[n] = 1;
    }
  }
  else {
    ext_leafs = NULL;
    ext_leafs_num_subleafs = NULL;
  }
}

bx_cpuid_table_t::~bx_cpuid_table_t()
{
  delete [] std_leafs;
  delete [] std_leafs_num_subleafs;
  delete [] ext_leafs;
  delete [] ext_leafs_num_subleafs;
}

void bx_cpuid_table_t::register_leaf(Bit32u function, Bit32u num_subleafs, const cpuid_function_t *subleafs)
{
  assert(num_subleafs > 0 && subleafs != NULL);

  if (function >= 0x80000000) {
    assert(ext_leafs != NULL && function <= max_ext_leaf);

    Bit32u ext_leaf = function - 0x80000000;
    assert(ext_leafs[ext_leaf] == &reserved_leaf);

    ext_leafs[ext_leaf] = subleafs;
    ext_leafs_num_subleafs[ext_leaf] = num_subleafs;
  }
  else {
    assert(function <= max_std_leaf && std_leafs[function] == &reserved_leaf);

    std_leafs[function] = subleafs;
    std_leafs_num_subleafs[function] = num_subleafs;
  }
}

void bx_cpuid_table_t::get_std_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  // unknown leaf returns the same as the highest std leaf
  if (function > max_std_leaf)
    function = max_std_leaf;

  // leaf without subleafs returns its single row regardless of the subleaf
  if (std_leafs_num_subleafs[function] == 1) {
    *leaf = std_leafs[function][0];
    return;
  }

  // subleaf beyond the last row of the leaf returns reserved (all zero)
  if (subfunction >= std_leafs_num_subleafs[function]) {
    *leaf = reserved_leaf;
    return;
  }

  *leaf = std_leafs[function][subfunction];
}

void bx_cpuid_table_t::get_ext_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  // unknown leaf returns the same as the highest std leaf
  if (ext_leafs == NULL || function < 0x80000000 || function > max_ext_leaf) {
    get_std_leaf(max_std_leaf, subfunction, leaf);
    return;
  }

  Bit32u ext_leaf = function - 0x80000000;

  // leaf without subleafs returns its single row regardless of the subleaf
  if (ext_leafs_num_subleafs[ext_leaf] == 1) {
    *leaf = ext_leafs[ext_leaf][0];
    return;
  }

  // subleaf beyond the last row of the leaf returns reserved (all zero)
  if (subfunction >= ext_leafs_num_subleafs[ext_leaf]) {
    *leaf = reserved_leaf;
    return;
  }

  *leaf = ext_leafs[ext_leaf][subfunction];
}

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

  set_max_leaf(0, 0);
}

void bx_cpuid_t::set_max_leaf(Bit32u _max_std_leaf, Bit32u _max_ext_leaf)
{
  max_std_leaf = _max_std_leaf;
  max_ext_leaf = _max_ext_leaf;
}

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

// leaf 0x00000001 - ECX
Bit32u bx_cpuid_t::get_std_cpuid_leaf_1_ecx(Bit32u extra) const
{
  Bit32u ecx = extra;

  // [0:0]   SSE3: SSE3 Instructions
  if (is_cpu_extension_supported(BX_ISA_SSE3))
    ecx |= BX_CPUID_STD1_ECX_SSE3;

  // [1:1]   PCLMULQDQ Instruction support
  if (is_cpu_extension_supported(BX_ISA_AES_PCLMULQDQ))
    ecx |= BX_CPUID_STD1_ECX_PCLMULQDQ;

  // [2:2]   DTES64: 64-bit DS area - not supported, could be enable through extra

  // [3:3]   MONITOR/MWAIT support
#if BX_SUPPORT_MONITOR_MWAIT
  if (is_cpu_extension_supported(BX_ISA_MONITOR_MWAIT))
    ecx |= BX_CPUID_STD1_ECX_MONITOR_MWAIT;
#endif

  // [4:4]   DS-CPL: CPL qualified debug store  - not supported, could be enable through extra

  // [5:5]   VMX: Virtual Machine Technology
#if BX_SUPPORT_VMX >= 2
  if (is_cpu_extension_supported(BX_ISA_VMX))
    ecx |= BX_CPUID_STD1_ECX_VMX;
#endif

  // [6:6]   SMX: Safer Mode Extensions for Trusted Execution Technology (TXT) - not supported
  // [7:7]   EST: Enhanced Intel SpeedStep Technology - not supported, could be enabled through extra
  // [8:8]   TM2: Thermal Monitor 2 - not supported, could be enabled through extra

  // [9:9]   SSSE3: SSSE3 Instructions
  if (is_cpu_extension_supported(BX_ISA_SSSE3))
    ecx |= BX_CPUID_STD1_ECX_SSSE3;

  // [10:10] CNXT-ID: L1 context ID - not supported, could be enabled through extra
  // [11:11] reserved

  // [12:12] FMA Instructions support
#if BX_SUPPORT_AVX
  if (is_cpu_extension_supported(BX_ISA_AVX_FMA))
    ecx |= BX_CPUID_STD1_ECX_FMA;
#endif

  // [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  if (is_cpu_extension_supported(BX_ISA_CMPXCHG16B))
    ecx |= BX_CPUID_STD1_ECX_CMPXCHG16B;

  // [14:14] xTPR update control - not supported, could be enabled through extra
  // [15:15] PDCM - Perfmon and Debug Capability MSR - not supported, could be enabled through extra
  // [16:16] reserved

  // [17:17] PCID: Process Context Identifiers
  if (is_cpu_extension_supported(BX_ISA_PCID))
    ecx |= BX_CPUID_STD1_ECX_PCID;

  // [18:18] DCA - Direct Cache Access - not supported, could be enabled through extra

  // [19:19] SSE4.1 Instructions
  if (is_cpu_extension_supported(BX_ISA_SSE4_1))
    ecx |= BX_CPUID_STD1_ECX_SSE4_1;

  // [20:20] SSE4.2 Instructions
  if (is_cpu_extension_supported(BX_ISA_SSE4_2))
    ecx |= BX_CPUID_STD1_ECX_SSE4_2;

  // [21:21] X2APIC
  if (is_cpu_extension_supported(BX_ISA_X2APIC))
    ecx |= BX_CPUID_STD1_ECX_X2APIC;

  // [22:22] MOVBE instruction
  if (is_cpu_extension_supported(BX_ISA_MOVBE))
    ecx |= BX_CPUID_STD1_ECX_MOVBE;

  // [23:23] POPCNT instruction
  if (is_cpu_extension_supported(BX_ISA_POPCNT))
    ecx |= BX_CPUID_STD1_ECX_POPCNT;

  // [24:24] TSC Deadline
  if (is_cpu_extension_supported(BX_ISA_TSC_DEADLINE))
    ecx |= BX_CPUID_STD1_ECX_TSC_DEADLINE;

  // [25:25] AES Instructions
  if (is_cpu_extension_supported(BX_ISA_AES_PCLMULQDQ))
    ecx |= BX_CPUID_STD1_ECX_AES;

  // [26:26] XSAVE extensions support
  if (is_cpu_extension_supported(BX_ISA_XSAVE))
    ecx |= BX_CPUID_STD1_ECX_XSAVE;

  // [27:27] OSXSAVE support
#if BX_CPU_LEVEL >= 5
  if (cpu->cr4.get_OSXSAVE())
    ecx |= BX_CPUID_STD1_ECX_OSXSAVE;
#endif

  // [28:28] AVX extensions support
  if (is_cpu_extension_supported(BX_ISA_AVX))
    ecx |= BX_CPUID_STD1_ECX_AVX;

  // [29:29] AVX F16C - Float16 conversion support
#if BX_SUPPORT_AVX
  if (is_cpu_extension_supported(BX_ISA_AVX_F16C))
    ecx |= BX_CPUID_STD1_ECX_AVX_F16C;
#endif

  // [30:30] RDRAND instruction
  if (is_cpu_extension_supported(BX_ISA_RDRAND))
    ecx |= BX_CPUID_STD1_ECX_RDRAND;

  // [31:31] reserved

  return ecx;
}

// leaf 0x00000001 - EDX[24:23,18:12,10:0]
Bit32u bx_cpuid_t::get_std_cpuid_leaf_1_edx_common(Bit32u extra) const
{
  Bit32u edx = extra;

  // [0:0]   FPU on chip
  if (is_cpu_extension_supported(BX_ISA_X87))
    edx |= BX_CPUID_STD1_EDX_X87;

  // [1:1]   VME: Virtual-8086 Mode enhancements
  if (is_cpu_extension_supported(BX_ISA_VME))
    edx |= BX_CPUID_STD1_EDX_VME;

  // [2:2]   DE: Debug Extensions (I/O breakpoints)
  if (is_cpu_extension_supported(BX_ISA_DEBUG_EXTENSIONS))
    edx |= BX_CPUID_STD1_EDX_DEBUG_EXTENSIONS;

  // [3:3]   PSE: Page Size Extensions
  if (is_cpu_extension_supported(BX_ISA_PSE))
    edx |= BX_CPUID_STD1_EDX_PSE;

  // [4:4]   TSC: Time Stamp Counter
  if (is_cpu_extension_supported(BX_ISA_PENTIUM))
    edx |= BX_CPUID_STD1_EDX_TSC;

  // [5:5]   MSR: RDMSR and WRMSR support
  if (is_cpu_extension_supported(BX_ISA_MSR))
    edx |= BX_CPUID_STD1_EDX_MSR;

  // [6:6]   PAE: Physical Address Extensions
  if (is_cpu_extension_supported(BX_ISA_PAE))
    edx |= BX_CPUID_STD1_EDX_PAE;

  // [7:7]   MCE: Machine Check Exception
  if (is_cpu_extension_supported(BX_ISA_PENTIUM))
    edx |= BX_CPUID_STD1_EDX_MCE;

  // [8:8]   CXS: CMPXCHG8B instruction
  if (is_cpu_extension_supported(BX_ISA_PENTIUM))
    edx |= BX_CPUID_STD1_EDX_CMPXCHG8B;

 // [9:9]   APIC: APIC on Chip
#if BX_SUPPORT_APIC
  if (is_cpu_extension_supported(BX_ISA_XAPIC)) {
    // if MSR_APICBASE APIC Global Enable bit has been cleared,
    // the CPUID feature flag for the APIC is set to 0.
    if (cpu->apic_global_enable_on())
      edx |= BX_CPUID_STD1_EDX_APIC; // APIC on chip
  }
#endif

  // [10:10] Reserved

  // [11:11] Not common between Intel and AMD

  // [12:12] MTRR: Memory Type Range Reg
  if (is_cpu_extension_supported(BX_ISA_MTRR))
    edx |= BX_CPUID_STD1_EDX_MTRR;

  // [13:13] PGE/PTE Global Bit
  if (is_cpu_extension_supported(BX_ISA_PGE))
    edx |= BX_CPUID_STD1_EDX_GLOBAL_PAGES;

  // [14:14] MCA: Machine Check Architecture
  if (is_cpu_extension_supported(BX_ISA_P6))
    edx |= BX_CPUID_STD1_EDX_MCA;

  // [15:15] CMOV: Cond Mov/Cmp Instructions
  if (is_cpu_extension_supported(BX_ISA_P6))
    edx |= BX_CPUID_STD1_EDX_CMOV;

  // [16:16] PAT: Page Attribute Table
  if (is_cpu_extension_supported(BX_ISA_PAT))
    edx |= BX_CPUID_STD1_EDX_PAT;

#if BX_PHY_ADDRESS_LONG
  // [17:17] PSE-36: Physical Address Extensions
  if (is_cpu_extension_supported(BX_ISA_PSE))
    edx |= BX_CPUID_STD1_EDX_PSE36;
#endif

  // [18:18] PSN: Processor Serial Number - could be enabled through extra

  // [23:23] MMX Technology
  if (is_cpu_extension_supported(BX_ISA_MMX))
    edx |= BX_CPUID_STD1_EDX_MMX;

  // [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  if (is_cpu_extension_supported(BX_ISA_SSE))
    edx |= BX_CPUID_STD1_EDX_FXSAVE_FXRSTOR;

  return edx;
}

Bit32u bx_cpuid_t::get_std_cpuid_leaf_1_edx(Bit32u extra) const
{
  Bit32u edx = get_std_cpuid_leaf_1_edx_common(extra);

  // [*] indicates common bits
  // * [0:0]   FPU on chip
  // * [1:1]   VME: Virtual-8086 Mode enhancements
  // * [2:2]   DE: Debug Extensions (I/O breakpoints)
  // * [3:3]   PSE: Page Size Extensions
  // * [4:4]   TSC: Time Stamp Counter
  // * [5:5]   MSR: RDMSR and WRMSR support
  // * [6:6]   PAE: Physical Address Extensions
  // * [7:7]   MCE: Machine Check Exception
  // * [8:8]   CXS: CMPXCHG8B instruction
  // * [9:9]   APIC: APIC on Chip
  // * [10:10] Reserved

  //   [11:11] SYSENTER/SYSEXIT support
  if (is_cpu_extension_supported(BX_ISA_SYSENTER_SYSEXIT))
    edx |= BX_CPUID_STD1_EDX_SYSENTER_SYSEXIT;

  // * [12:12] MTRR: Memory Type Range Reg
  // * [13:13] PGE/PTE Global Bit
  // * [14:14] MCA: Machine Check Architecture
  // * [15:15] CMOV: Cond Mov/Cmp Instructions
  // * [16:16] PAT: Page Attribute Table
  // * [17:17] PSE-36: Physical Address Extensions

  //   [18:18] PSN: Processor Serial Number - not implemented, could be enabled through extra

  //   [19:19] CLFLUSH: CLFLUSH Instruction support
  if (is_cpu_extension_supported(BX_ISA_CLFLUSH))
    edx |= BX_CPUID_STD1_EDX_CLFLUSH;

  //   [20:20] Reserved
  //   [21:21] DS: Debug Store - not implemented, could be enabled through extra
  //   [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities - not implemented, could be enabled through extra
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)

  //   [25:25] SSE: SSE Extensions
  if (is_cpu_extension_supported(BX_ISA_SSE))
    edx |= BX_CPUID_STD1_EDX_SSE;

  //   [26:26] SSE2: SSE2 Extensions
  if (is_cpu_extension_supported(BX_ISA_SSE2))
    edx |= BX_CPUID_STD1_EDX_SSE2;

  //   [27:27] Self Snoop - not implemented, could be enabled through extra
  //   [28:28] Hyper Threading Technology - enabled through extra if SMT is ON
  //   [29:29] TM: Thermal Monitor - not implemented, could be enabled through extra
  //   [30:30] Reserved
  //   [31:31] PBE: Pending Break Enable - not implemented, could be enabled through extra

  return edx;
}

// CPUID function 0x00000005 - MONITOR/MWAIT Leaf
void bx_cpuid_t::get_std_cpuid_monitor_mwait_leaf(cpuid_function_t *leaf) const
{
#if BX_SUPPORT_MONITOR_MWAIT
  // EDX (MWAIT sub C-states) is taken from the hardware model
  get_hardware_cpuid_leaf(0x00000005, 0, leaf);

  // EAX - Smallest monitor-line size in bytes (cache line size)
  // EBX - Largest  monitor-line size in bytes (cache line size)
  // ECX -
  //   [31:4] - reserved
  //    [3:3] - Monitorless MWAIT support
  //    [2:2] - reserved
  //  * [1:1] - exit MWAIT even with EFLAGS.IF = 0
  //  * [0:0] - MONITOR/MWAIT extensions are supported
  // EDX -
  //  [03-00] - number of C0 sub C-states supported using MWAIT
  //  [07-04] - number of C1 sub C-states supported using MWAIT
  //  [11-08] - number of C2 sub C-states supported using MWAIT
  //  [15-12] - number of C3 sub C-states supported using MWAIT
  //  [19-16] - number of C4 sub C-states supported using MWAIT
  //  [31-20] - reserved
  leaf->eax = CACHE_LINE_SIZE;
  leaf->ebx = CACHE_LINE_SIZE;
  leaf->ecx = 0x3;
  if (is_cpu_extension_supported(BX_ISA_MONITORLESS_MWAIT))
    leaf->ecx |= (1<<3);
#else
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;
#endif
}

// CPUID function 0x00000004 - Deterministic Cache Parameters
void bx_cpuid_t::get_std_cpuid_deterministic_cache_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  // EAX:
  //   [04-00] - Cache Type Field
  //             0 = No more caches
  //             1 = Data Cache
  //             2 = Instruction Cache
  //             3 = Unified Cache
  //   [07-05] - Cache Level (starts at 1)
  //      [08] - Self Initializing cache level (doesn't need software initialization)
  //      [09] - Fully Associative cache
  //   [13-10] - Reserved
  //   [25-14] - Maximum number of addressable IDs for logical processors sharing this cache - 1
  //   [31-26] - Maximum number of addressable IDs for processor cores in the physical package - 1
  // EBX:
  //   [11-00] - L = System Coherency Line Size - 1
  //   [21-12] - P = Physical Line partitions - 1
  //   [31-22] - W = Ways of associativity - 1
  // ECX: Number of Sets - 1
  // EDX:
  //      [00] - Writeback invalidate
  //      [01] - Cache Inclusiveness
  //      [02] - Complex Cache Indexing
  //   [31-03] - Reserved

  get_hardware_cpuid_leaf(0x00000004, subfunction, leaf);

  // no more caches: the terminating subleaf is all zeros
  // (AMD reports the whole leaf as reserved zeros)
  if (leaf->eax == 0) return;

  // The model tables hold the values of the real CPU, which describe the
  // real CPU topology (for example 4 cores with 2 threads each for the
  // Core i7-4770), not the configured one. Only the two topology fields
  // EAX[25-14] and EAX[31-26] are replaced, cache type, level, geometry
  // (EBX, ECX) and EDX flags are kept from the real CPU.
  //
  // The fields must agree with leaf 0x00000001 EBX[23-16], which reports
  // ncores * nthreads logical processors in the package. Guests divide one
  // by the other: macOS computes threads per core as leaf 1 logical count
  // divided by leaf 4 core count, and with the i7-4770 values (8 core IDs)
  // and a single configured CPU (1 logical) gets 0 and later faults with a
  // divide error in cpu_thread_alloc(). Exact counts are used instead of
  // the power of 2 rounding real CPUs report, because leaf 1 is not rounded
  // either (with 3 cores a rounded 4 would again give 3 / 4 = 0).
  //
  // EAX[31-26] (core IDs in the package - 1):
  //   ncores - 1
  //
  // EAX[25-14] (logical processors sharing this cache - 1):
  //   the sharing scope is taken from the real CPU. L1D (subleaf 0) is
  //   always private to a core, so its sharing count is the real number of
  //   threads per core. A cache shared by no more logical processors than
  //   L1D is private to a core (L1, and L2 on Nehalem and later), a cache
  //   shared by more is shared by the whole package (L3, and L2 on Core 2
  //   and Core Duo). The configured count for the scope is used:
  //     private to a core      -> nthreads - 1
  //     shared by the package  -> ncores * nthreads - 1
  //   Caches shared by a module of cores (some Atoms) are reported as
  //   shared by the package.

  cpuid_function_t l1d;
  get_hardware_cpuid_leaf(0x00000004, 0, &l1d);

  // compare the raw EAX[25-14] fields (both hold count - 1)
  Bit32u hw_sharing = (leaf->eax >> 14) & 0xfff;
  Bit32u hw_l1d_sharing = (l1d.eax >> 14) & 0xfff;

  Bit32u sharing = (hw_sharing <= hw_l1d_sharing) ? nthreads : ncores * nthreads;

  leaf->eax = (leaf->eax & 0x00003fff) | ((sharing - 1) << 14) | ((ncores - 1) << 26);
}

#if BX_SUPPORT_APIC

// CPUID function 0x0000000B - Extended Topology Leaf
// CPUID function 0x0000001F - V2 Extended Topology Leaf
// Bochs topology has no module/tile/die/diegrp domains, so both leaves report the same
// logical processor and core domains
void bx_cpuid_t::get_std_cpuid_extended_topology_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  // EAX:
  //   [04-00] - SHIFT_COUNT: number of bits the x2APIC ID must be shifted right
  //             to address instances of the next higher-scoped domain
  //   [31-05] - Reserved
  // EBX:
  //   [15-00] - NEXT_LEVEL_NUM_LP: number of logical processors across all
  //             instances of this domain within the next higher-scoped domain
  //   [31-16] - Reserved
  // ECX:
  //   [07-00] - LEVEL_NUM: the input sub-leaf index
  //   [15-08] - DOMAIN_TYPE:
  //             0 = Invalid (no more domains)
  //             1 = Logical Processor
  //             2 = Core
  //             3 = Module (leaf 0x1F only)
  //             4 = Tile (leaf 0x1F only)
  //             5 = Die (leaf 0x1F only)
  //             6 = DieGrp (leaf 0x1F only)
  //   [31-16] - Reserved
  // EDX: x2APIC ID of the current logical processor (same for every sub-leaf)
  //
  // A sub-leaf with invalid domain type returns zero in EAX and EBX, all higher
  // sub-leaves are invalid as well
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = subfunction & 0xff; // ECX[7:0]: LEVEL_NUM, ECX[31:16] reserved
  leaf->edx = cpu->get_apic_id();

  enum {
    BX_DOMAIN_TYPE_ID_VALUE_INVALID = 0,
    BX_DOMAIN_TYPE_ID_VALUE_SMT_THREAD = 1,
    BX_DOMAIN_TYPE_ID_VALUE_CORE = 2,
    BX_DOMAIN_TYPE_ID_VALUE_MODULE = 3,
    BX_DOMAIN_TYPE_ID_VALUE_TILE = 4,
    BX_DOMAIN_TYPE_ID_VALUE_DIE = 5,
    BX_DOMAIN_TYPE_ID_VALUE_DIEGRP = 6
  };

  // number of x2APIC ID bits allocated for threads and cores
  Bit32u thread_bits = (nthreads > 1) ? (ilog2(nthreads-1)+1) : 0;
  Bit32u core_bits = (ncores > 1) ? (ilog2(ncores-1)+1) : 0;

  // logical processor and core domains are always enumerated,
  // the package domain is implied and never enumerated
  switch(subfunction) {
  case 0:
     leaf->eax = thread_bits;
     leaf->ebx = nthreads;
     leaf->ecx |= (BX_DOMAIN_TYPE_ID_VALUE_SMT_THREAD<<8);
     break;

  case 1:
     leaf->eax = thread_bits + core_bits;
     leaf->ebx = ncores * nthreads;
     leaf->ecx |= (BX_DOMAIN_TYPE_ID_VALUE_CORE<<8);
     break;

  default:
     break;
  }
}

#endif

#if BX_CPU_LEVEL >= 6
extern XSaveRestoreStateHelper xsave_restore[];

extern Bit32u xsave_area_last_byte(Bit32u requested_feature_bitmap);
extern Bit32u xsave_area_last_byte_compaction(Bit32u xcomp_bv, Bit32u requested_feature_bitmap);

// CPUID Leaf 0x0000000D: XSAVE
void bx_cpuid_t::get_std_cpuid_xsave_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

  if (!is_cpu_extension_supported(BX_ISA_XSAVE))
    return;

  if (subfunction == 0) {
    // EAX - valid bits of XCR0 (lower part)
    // EBX - Maximum size (in bytes) required by enabled features
    // ECX - Maximum size (in bytes) required by CPU supported features
    // EDX - valid bits of XCR0 (upper part)
    leaf->eax = cpu->xcr0_suppmask;
    leaf->ebx = xsave_area_last_byte(cpu->xcr0.get32());
    leaf->ecx = xsave_area_last_byte(cpu->xcr0_suppmask);
    leaf->edx = 0; // xcr0 is still 32-bit
    return;
  }

  if (subfunction == 1) {
    // EAX[0] - support for the XSAVEOPT instruction
    // EAX[1] - support for compaction extensions to the XSAVE feature set
    // EAX[2] - support for execution of XGETBV with ECX = 1
    // EAX[3] - support for XSAVES, XRSTORS, and the IA32_XSS MSR
    // EAX[4] - support for XFD
    leaf->eax = 0;
    if (is_cpu_extension_supported(BX_ISA_XSAVEOPT))
      leaf->eax |= 0x1;
    if (is_cpu_extension_supported(BX_ISA_XSAVEC))
      leaf->eax |= (1<<1) | (1<<2);
    if (is_cpu_extension_supported(BX_ISA_XSAVES))
      leaf->eax |= (1<<3);

    // EBX[31:00] - The size (in bytes) of the XSAVE area containing all states enabled by (XCRO | IA32_XSS)
    leaf->ebx = 0;
    if (is_cpu_extension_supported(BX_ISA_XSAVES)) {
      Bit32u xsaves_features = cpu->xcr0.get32() | cpu->msr.ia32_xss;
      leaf->ebx = xsave_area_last_byte_compaction(xsaves_features, xsaves_features);
    }

    // ECX[31:0] - Reports the supported bits of the lower 32 bits of the IA32_XSS MSR.
    //             IA32_XSS[n]    can be set to 1 only if ECX[n] is 1
    // EDX[31:0] - Reports the supported bits of the upper 32 bits of the IA32_XSS MSR.
    //             IA32_XSS[n+32] can be set to 1 only if EDX[n] is 1
    leaf->ecx = cpu->ia32_xss_suppmask;
    leaf->edx = 0; // xcr0 is still 32-bit
    return;
  }

  if (subfunction < xcr0_t::BX_XCR0_LAST) {
    Bit32u support_mask = cpu->xcr0_suppmask | cpu->ia32_xss_suppmask;
    if (support_mask & (1 << subfunction)) {
      leaf->eax = xsave_restore[subfunction].len;
      leaf->ebx = xsave_restore[subfunction].offset;
      // ECX[0] - set if this component managed through IA32_XSS register
      // ECX[1] - set to indicate this component must be aligned to 64-byte when the compacted format of an XSAVE area is used
      // ECX[2] - XFD support for this component
      leaf->ecx = xsave_restore[subfunction].attr;
      leaf->edx = 0;
    }
  }
}
#endif

#if BX_SUPPORT_AMX
// Leaf 0x0000001D: AMX Palette info
void bx_cpuid_t::get_std_cpuid_amx_palette_info_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

  if (!is_cpu_extension_supported(BX_ISA_AMX))
    return;

  unsigned max_palette_id = is_cpu_extension_supported(BX_ISA_ACE) ? 2 : 1;
  if (subfunction > max_palette_id)
    return;

  if (subfunction == 0) {
    leaf->eax = max_palette_id;
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
    return;
  }

  // information about palette #1
  if (subfunction == 1) {
    // EAX[15:00] : Palette #1 total tile bytes = 8192
    // EAX[31:16] : Palette #1 bytes per tile = 1024
    leaf->eax = 8192 | (1024<<16);
    // EBX[15:00] : Palette #1 bytes_per_row = 64
    // EBX[31:16] : Palette #1 number of tiles = 8
    leaf->ebx = 64 | (8<<16);
    // ECX[15:00] : Palette #1 max_rows = 16
    // ECX[31:16] : Reserved
    leaf->ecx = 16;
    // EdX[31:00] : Reserved
    leaf->edx = 0;
    return;
  }

  // information about palette #2
  if (subfunction == 2) {
    // EAX[7:0] : ACE Major Version >= 1
    leaf->eax = ace_level();
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
    return;
  }
}

// Leaf 0x0000001E: AMX: TMUL Information Main leaf
void bx_cpuid_t::get_std_cpuid_amx_tmul_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

  if (!is_cpu_extension_supported(BX_ISA_AMX))
    return;

  // AMX extensions reported in sub-leaf 1 EAX:
  // ---
  //    [0] AMX-INT8
  //    [1] AMX-BF16
  //    [2] AMX-COMPLEX
  //    [3] AMX-FP16
  //    [4] AMX-FP8
  //    [5] AMX-TRANSPOSE (deprecated)
  //    [6] AMX-TF32 (deprecated)
  //    [7] AMX-AVX512
  //    [8] AMX-MOVRS
  // [31:9] reserved
  Bit32u amx_extensions = 0;
  if (is_cpu_extension_supported(BX_ISA_AMX_INT8))
    amx_extensions |= BX_CPUID_AMX_EXTENSIONS_EAX_AMX_INT8;
  if (is_cpu_extension_supported(BX_ISA_AMX_BF16))
    amx_extensions |= BX_CPUID_AMX_EXTENSIONS_EAX_AMX_BF16;
  if (is_cpu_extension_supported(BX_ISA_AMX_COMPLEX))
    amx_extensions |= BX_CPUID_AMX_EXTENSIONS_EAX_AMX_COMPLEX;
  if (is_cpu_extension_supported(BX_ISA_AMX_FP16))
    amx_extensions |= BX_CPUID_AMX_EXTENSIONS_EAX_AMX_FP16;
  if (is_cpu_extension_supported(BX_ISA_AMX_FP8))
    amx_extensions |= BX_CPUID_AMX_EXTENSIONS_EAX_AMX_FP8;
  if (is_cpu_extension_supported(BX_ISA_AMX_AVX512))
    amx_extensions |= BX_CPUID_AMX_EXTENSIONS_EAX_AMX_AVX512;
  if (is_cpu_extension_supported(BX_ISA_AMX_MOVRS))
    amx_extensions |= BX_CPUID_AMX_EXTENSIONS_EAX_AMX_MOVRS;

  switch(subfunction) {
  case 0:
    // EAX[31:00] = maximum number of sub-leaves supported in leaf 1EH,
    //              sub-leaf 1 is reported only if there is AMX extension to report
    leaf->eax = (amx_extensions != 0) ? 1 : 0;
    // EBX[07:00] = 16 TMUL_MAX_K (rows or columns)
    // EBX[23:08] = 64 TMUL_MAX_N (column bytes)
    // EBX[31:24] reserved
    leaf->ebx = 16 | (64<<8);
    break;

  case 1:
    // EAX: AMX extensions (see above)
    leaf->eax = amx_extensions;
    // EBX/ECX/EDX = 0 (reserved)
    break;

  default:
    break;
  }
}
#endif

#if BX_SUPPORT_EVEX
unsigned bx_cpuid_t::avx10_level() const
{
  if (is_cpu_extension_supported(BX_ISA_AVX10_2)) return 2;
  if (is_cpu_extension_supported(BX_ISA_AVX10_1)) return 1;
  return 0;
}

unsigned bx_cpuid_t::ace_level() const
{
  if (is_cpu_extension_supported(BX_ISA_ACE)) return 1;
  return 0;
}
#endif

// Leaf 0x00000024: AVX10 support and versioning
void bx_cpuid_t::get_std_cpuid_avx10_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

#if BX_SUPPORT_EVEX
  if (!is_cpu_extension_supported(BX_ISA_AVX10_1))
    return;

  unsigned avx10_lvl = avx10_level();

  switch (subfunction) {
  case 0:
    // EAX:
    //   [31:00]: maximum supported subleaf
    // EBX:
    //   [07:00]: AVX10 version
    //   [15:08]: reserved
    //   [16:16]: VL128 supported (always '1)
    //   [17:17]: VL256 supported (always '1)
    //   [18:18]: VL512 supported (always '1)
    //   [31:19]: reserved
    // ECX:
    //   [31:00]: reserved
    // EDX:
    //   [31:00]: reserved
    if (is_cpu_extension_supported(BX_ISA_ACE))
      leaf->eax = 1;
    else
      leaf->eax = 0;
    leaf->ebx = avx10_lvl | (1<<16) | (1<<17) | (1<<18);
    leaf->ecx = 0;
    leaf->edx = 0;
    break;

  case 1:
    // EAX:
    // EBX:
    // ECX:
    //   [2] AVX10_VNNI_INT instructions support, also known as AVX10_V1_AUX, part of AVX10.2
    //   [3] AVX10_V2_AUX, part of ACE v1
    // EDX:
    if (is_cpu_extension_supported(BX_ISA_ACE)) {
      leaf->eax = 0;
      leaf->ebx = 0;
      leaf->ecx = (avx10_lvl >= 2) ? (1<<2) : 0;
      leaf->ecx |= (1<<3);
      leaf->edx = 0;
    }
    break;

  default:
    break;
  }
#endif
}

#if BX_SUPPORT_APX
// Leaf 0x00000029: APX support and versioning
void bx_cpuid_t::get_std_cpuid_apx_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

  if (!is_cpu_extension_supported(BX_ISA_APX))
    return;

  switch (subfunction) {
  case 0:
    // EAX:
    //   [31:00]: maximum supported subleaf
    // EBX:
    //   [00:00]: APX_NCI_NDD_NF - APX new conditional ISA, NDD, NF support
    //   [31:01]: reserved
    // ECX:
    //   [31:00]: reserved
    // EDX:
    //   [31:00]: reserved
    leaf->eax = 0;
    leaf->ebx = 0x1;
    leaf->ecx = 0;
    leaf->edx = 0;
    break;

  default:
    break;
  }
}
#endif

// leaf 0x00000007, subleaf 0 - EBX
Bit32u bx_cpuid_t::get_std_cpuid_leaf_7_ebx(Bit32u extra) const
{
  Bit32u ebx = extra;

  // [0:0]    FS/GS BASE access instructions
  if (is_cpu_extension_supported(BX_ISA_FSGSBASE))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_FSGSBASE;

  // [1:1]    Support for IA32_TSC_ADJUST MSR
  if (is_cpu_extension_supported(BX_ISA_TSC_ADJUST))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_TSC_ADJUST;

  // [2:2]    SGX: Intel Software Guard Extensions - not supported

  // [3:3]    BMI1: Advanced Bit Manipulation Extensions
  if (is_cpu_extension_supported(BX_ISA_BMI1))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_BMI1;

  // [4:4]    HLE: Hardware Lock Elision - not supported

  // [5:5]    AVX2
  if (is_cpu_extension_supported(BX_ISA_AVX2))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_AVX2;

  // [6:6]    FDP Deprecation
  if (is_cpu_extension_supported(BX_ISA_FDP_DEPRECATION))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_FDP_DEPRECATION;

  // [7:7]    SMEP: Supervisor Mode Execution Protection
  if (is_cpu_extension_supported(BX_ISA_SMEP))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_SMEP;

  // [8:8]    BMI2: Advanced Bit Manipulation Extensions
  if (is_cpu_extension_supported(BX_ISA_BMI2))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_BMI2;

  // [9:9]    Support for Enhanced REP MOVSB/STOSB - no special emulation required, could be enabled through extra

  // [10:10]  Support for INVPCID instruction
  if (is_cpu_extension_supported(BX_ISA_INVPCID))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_INVPCID;

  // [11:11]  RTM: Restricted Transactional Memory - not supported
  // [12:12]  Supports Platform Quality of Service Monitoring (PQM) capability - not supported

  // [13:13]  Deprecates FPU CS and FPU DS values
  if (is_cpu_extension_supported(BX_ISA_FCS_FDS_DEPRECATION))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_DEPRECATE_FCS_FDS;

  // [14:14]  Intel Memory Protection Extensions (MPX)
#if BX_SUPPORT_MPX
  if (is_cpu_extension_supported(BX_ISA_MPX))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_MPX;
#endif

  // [15:15]  Supports Platform Quality of Service Enforcement (PQE) capability - not supported

  // [16:16]  AVX512F instructions support
  // [17:17]  AVX512DQ instructions support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_AVX512F;
    if (is_cpu_extension_supported(BX_ISA_AVX512_DQ))
      ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_AVX512DQ;
  }
#endif

  // [18:18]  RDSEED instruction support
  if (is_cpu_extension_supported(BX_ISA_RDSEED))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_RDSEED;

  // [19:19]  ADCX/ADOX instructions support
  if (is_cpu_extension_supported(BX_ISA_ADX))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_ADX;

  // [20:20]  SMAP: Supervisor Mode Access Prevention
  if (is_cpu_extension_supported(BX_ISA_SMAP))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_SMAP;

  // [21:21]  AVX512IFMA52 instructions support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_IFMA52))
      ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_AVX512IFMA52;
  }
#endif

  // [22:22]  reserved

  // [23:23]  CLFLUSHOPT instruction
  // [24:24]  CLWB instruction
  if (is_cpu_extension_supported(BX_ISA_CLFLUSHOPT))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_CLFLUSHOPT;
  if (is_cpu_extension_supported(BX_ISA_CLWB))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_CLWB;

  // [25:25]  Intel Processor Trace - not supported
  // [26:26]  AVX512PF instructions support - not supported
  // [27:27]  AVX512ER instructions support - not supported
  // [28:28]  AVX512CD instructions support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_CD))
      ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_AVX512CD;
  }
#endif

  // [29:29]  SHA instructions support
  if (is_cpu_extension_supported(BX_ISA_SHA))
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_SHA;

  // [30:30]  AVX512BW instructions support
  // [31:31]  AVX512VL variable vector length support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_BW))
      ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_AVX512BW;
    ebx |= BX_CPUID_STD7_SUBLEAF0_EBX_AVX512VL;
  }
#endif

  return ebx;
}

// leaf 0x00000007, subleaf 0 - ECX
Bit32u bx_cpuid_t::get_std_cpuid_leaf_7_ecx(Bit32u extra) const
{
  Bit32u ecx = extra;

  // [0:0]   PREFETCHW1 instruction - not supported

  // [1:1]   AVX512 VBMI instructions
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_VBMI))
      ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_VBMI;
  }
#endif

  // [2:2]   UMIP: Supports user-mode instruction prevention
  if (is_cpu_extension_supported(BX_ISA_UMIP))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_UMIP;

  // [3:3]   PKU: Protection keys for user-mode pages
  // [4:4]   OSPKE: OS has set CR4.PKE to enable protection keys
#if BX_SUPPORT_PKEYS && BX_CPU_LEVEL >= 5
  if (is_cpu_extension_supported(BX_ISA_PKU)) {
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_PKU;
    if (cpu->cr4.get_PKE())
      ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_OSPKE;
 }
#endif

  // [5:5]   WAITPKG (TPAUSE/UMONITOR/UMWAIT) support
#if BX_SUPPORT_MONITOR_MWAIT
  if (is_cpu_extension_supported(BX_ISA_WAITPKG)) {
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_WAITPKG;
  }
#endif

  // [6:6]   AVX512 VBMI2 instructions support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_VBMI2))
      ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_VBMI2;
  }
#endif

  // [7:7]   CET_SS: Support CET Shadow Stack
#if BX_SUPPORT_CET
  if (is_cpu_extension_supported(BX_ISA_CET))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_CET_SS;
#endif

  // [8:8]   GFNI instructions support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_GFNI))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_GFNI;
#endif

  // [9:9]   VAES instructions support
  // [10:10] VPCLMULQDQ instruction support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_VAES_VPCLMULQDQ))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_VAES | BX_CPUID_STD7_SUBLEAF0_ECX_VPCLMULQDQ;
#endif

  // [13:13] TME_EN: indicates support for MSRs: IA32_TME_CAPABILITY, IA32_TME_ACTIVATE, IA32_TME_EXCLUDE_MASK, and IA32_TME_EXCLUDE_BASE

  // [11:11] AVX512 VNNI instructions support
  // [12:12] AVX512 BITALG instructions support
  // [14:14] AVX512 VPOPCNTDQ: AVX512 VPOPCNTD/VPOPCNTQ instructions
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_VNNI))
      ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_VNNI;
    if (is_cpu_extension_supported(BX_ISA_AVX512_BITALG))
      ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_BITALG;
    if (is_cpu_extension_supported(BX_ISA_AVX512_VPOPCNTDQ))
      ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_VPOPCNTDQ;
  }
#endif

  // [15:15] reserved

  // [16:16] LA57: LA57 and 5-level paging
#if BX_SUPPORT_X86_64
  if (is_cpu_extension_supported(BX_ISA_LA57))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_LA57;
#endif

  // [21:17] MPX MAWAU - MPX user address-width adjust

  // [22:22] RDPID: Read Processor ID support
  if (is_cpu_extension_supported(BX_ISA_RDPID))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_RDPID;

  // [23:23] Keylocker support - not supported
  // [24:24] reserved
  // [25:25] CLDEMOTE: CLDEMOTE instruction support - not supported
  // [26:26] reserved

  // [27:27] MOVDIRI: MOVDIRI instruction support
  if (is_cpu_extension_supported(BX_ISA_MOVDIRI))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_MOVDIRI;

  // [28:28] MOVDIR64: MOVDIR64 instruction support
  if (is_cpu_extension_supported(BX_ISA_MOVDIR64B))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_MOVDIR64B;

  // [29:29] ENQCMD: Enqueue Stores support - not supported
  // [30:30] SGX_LC: SGX Launch Configuration - not supported

  // [31:31] PKS: Protection keys for supervisor-mode pages
#if BX_SUPPORT_PKEYS
  if (is_cpu_extension_supported(BX_ISA_PKS))
    ecx |= BX_CPUID_STD7_SUBLEAF0_ECX_PKS;
#endif

  return ecx;
}

// leaf 0x00000007, subleaf 0 - EDX
Bit32u bx_cpuid_t::get_std_cpuid_leaf_7_edx(Bit32u extra) const
{
  Bit32u edx = extra;

  // All SCA mitigations could be enabled through extra

  // CPUID defines - features CPUID[0x00000007].EDX  [subleaf 0]
  // -----------------------------
  //   [0:0]    reserved
  //   [1:1]    SGX-KEYS: Attestation Services for SGX support - not supported
  //   [2:2]    AVX512 4VNNIW instructions support - not supported
  //   [3:3]    AVX512 4FMAPS instructions support - not supported
  //   [4:4]    Support of Fast REP MOV instructions with short length - not supported, might be enabled through extra

  //   [5:5]    UINTR: User interrupts support - not yet supported
#if BX_SUPPORT_UINTR
  if (is_cpu_extension_supported(BX_ISA_UINTR))
    edx |= BX_CPUID_STD7_SUBLEAF0_EDX_UINTR;
#endif

  //   [7:6]    reserved

  //   [8:8]    AVX512 VP2INTERSECT instructions support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_VP2INTERSECT))
      edx |= BX_CPUID_STD7_SUBLEAF0_EDX_AVX512_VPINTERSECT;
  }
#endif

  //   [9:9]    SRBDS_CTRL: IA32_MCU_OPT_CTRL MSR
  // * [10:10]  MD clear (SCA)
  //   [11:11]  RTM_ALWAYS_ABORT - not supported
  //   [12:12]  reserved
  //   [13:13]  RTM_FORCE_ABORT - not supported

  //   [14:14]  SERIALIZE instruction support
  if (is_cpu_extension_supported(BX_ISA_SERIALIZE))
    edx |= BX_CPUID_STD7_SUBLEAF0_EDX_SERIALIZE;

  //   [15:15]  Hybrid - not supported, might be enabled through extra
  //   [16:16]  TSXLDTRK: TSX suspent load tracking support - not supported
  //   [17:17]  reserved
  //   [18:18]  PCONFIG
  //   [19:19]  Architectural LBRs support

  //   [20:20]  CET IBT: Support CET indirect branch tracking
#if BX_SUPPORT_CET
  if (is_cpu_extension_supported(BX_ISA_CET))
    edx |= BX_CPUID_STD7_SUBLEAF0_EDX_CET_IBT;
#endif

  //   [21:21]  reserved

  //   [22:22]  AMX BF16 support
#if BX_SUPPORT_AMX
  if (is_cpu_extension_supported(BX_ISA_AMX)) {
    if (is_cpu_extension_supported(BX_ISA_AMX_BF16))
      edx |= BX_CPUID_STD7_SUBLEAF0_EDX_AMX_BF16;
  }
#endif

  //   [23:23]  AVX512_FP16 instructions support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_FP16))
      edx |= BX_CPUID_STD7_SUBLEAF0_EDX_AVX512_FP16;
  }
#endif

#if BX_SUPPORT_AMX
  //   [24:24]  AMX TILE architecture support
  if (is_cpu_extension_supported(BX_ISA_AMX)) {
    edx |= BX_CPUID_STD7_SUBLEAF0_EDX_AMX_TILE;

    //   [25:25]  AMX INT8 support
    if (is_cpu_extension_supported(BX_ISA_AMX_INT8))
      edx |= BX_CPUID_STD7_SUBLEAF0_EDX_AMX_INT8;
  }
#endif

  // * [26:26]  IBRS and IBPB: Indirect branch restricted speculation (SCA)
  // * [27:27]  STIBP: Single Thread Indirect Branch Predictors supported (SCA)
  // * [28:28]  L1D_FLUSH supported (SCA)
  // * [29:29]  MSR_IA32_ARCH_CAPABILITIES supported (SCA)
  // * [30:30]  MSR_IA32_CORE_CAPABILITIES supported (SCA)
  // * [31:31]  SSBD: Speculative Store Bypass Disable supported (SCA)

  return edx;
}

// leaf 0x00000007, subleaf 1 - EAX
Bit32u bx_cpuid_t::get_std_cpuid_leaf_7_subleaf_1_eax(Bit32u extra) const
{
  Bit32u eax = extra;

  // CPUID defines - features CPUID[0x00000007].EAX  [subleaf 1]
  // -----------------------------
  //   [0:0]    SHA-512 instructions support
  if (is_cpu_extension_supported(BX_ISA_SHA512))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_SHA512;

  //   [1:1]    SM3 instructions support
  if (is_cpu_extension_supported(BX_ISA_SM3))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_SM3;

  //   [2:2]    SM4 instructions support
  if (is_cpu_extension_supported(BX_ISA_SM4))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_SM4;

  //   [3:3]    RAO-INT
  if (is_cpu_extension_supported(BX_ISA_RAO_INT))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_RAO_INT;

  //   [4:4]    AVX VNNI
#if BX_SUPPORT_AVX
  if (is_cpu_extension_supported(BX_ISA_AVX_VNNI))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_AVX_VNNI;
#endif

  //   [5:5]    AVX512_BF16 conversion instructions support
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_BF16))
      eax |= BX_CPUID_STD7_SUBLEAF1_EAX_AVX512_BF16;
  }
#endif

  //   [6:6]    LASS: Linear Address Space Separation support
  if (is_cpu_extension_supported(BX_ISA_LASS))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_LASS;

  //   [7:7]    CMPCCXADD
  if (is_cpu_extension_supported(BX_ISA_CMPCCXADD))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_CMPCCXADD;

  //   [8:8]    Arch Perfmon - not supported
  //   [9:9]    reserved
  //   [10:10]  Fast zero-length REP MOVSB - not supported, can be enabled through extra
  //   [11:11]  Fast zero-length REP STOSB - not supported, can be enabled through extra
  //   [12:12]  Fast zero-length REP CMPSB/SCASB - not supported, can be enabled through extra
  //   [16:13]  reserved

  //   [17:17]  Flexible Return and Event Delivery (FRED) support
  //   [18:18]  LKGS instruction support
  if (is_cpu_extension_supported(BX_ISA_FRED))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_FRED | BX_CPUID_STD7_SUBLEAF1_EAX_LKGS;

  //   [19:19]  WRMSRNS instruction
  if (is_cpu_extension_supported(BX_ISA_WRMSRNS))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_WRMSRNS;

  //   [20:20]  NMI source reporting - not implemented yet

  //   [21:21]  AMX-FP16 support
#if BX_SUPPORT_AMX
  if (is_cpu_extension_supported(BX_ISA_AMX)) {
    if (is_cpu_extension_supported(BX_ISA_AMX_FP16))
      eax |= BX_CPUID_STD7_SUBLEAF1_EAX_AMX_FP16;
  }
#endif

  //   [22:22]  HRESET and CPUID leaf 0x20 support

  //   [23:23]  AVX IFMA support
#if BX_SUPPORT_AVX
  if (is_cpu_extension_supported(BX_ISA_AVX_IFMA))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_AVX_IFMA;
#endif

  //   [25:24]  reserved
  //   [26:26]  LAM: Linear Address Masking

  //   [27:27]  MSRLIST: RDMSRLIST/WRMSRLIST instructions and the IA32_BARRIER MSR
  if (is_cpu_extension_supported(BX_ISA_MSRLIST))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_MSRLIST;

  //   [29:28]  reserved
  //   [30:30]  Prevent INVD execution after BIOS is done

  //   [31:31]  MOVRS instructions
  if (is_cpu_extension_supported(BX_ISA_MOVRS))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_MOVRS;

  return eax;
}

// leaf 0x00000007, subleaf 1 - ECX
Bit32u bx_cpuid_t::get_std_cpuid_leaf_7_subleaf_1_ecx(Bit32u extra) const
{
  Bit32u ecx = extra;

  // CPUID defines - features CPUID[0x00000007].ECX  [subleaf 1]
  // -----------------------------

  //    [0:4]   reserved

  //    [5:5]   Support immediate forms of RDMSR and WRMSRNS instructions
  if (is_cpu_extension_supported(BX_ISA_MSR_IMM))
    ecx |= BX_CPUID_STD7_SUBLEAF1_ECX_MSR_IMM;

  //   [10:6]   reserved

  //  [11:11]   Support ACE instructions
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_ACE))
    ecx |= BX_CPUID_STD7_SUBLEAF1_ECX_ACE;
#endif

  //  [31:12]   reserved

  return ecx;
}

// leaf 0x00000007, subleaf 1 - EDX
Bit32u bx_cpuid_t::get_std_cpuid_leaf_7_subleaf_1_edx(Bit32u extra) const
{
  Bit32u edx = extra;

  // CPUID defines - features CPUID[0x00000007].EDX  [subleaf 1]
  // -----------------------------
  //   [3:0]    reserved

  //   [4:4]    AVX_VNNI_INT8 support
#if BX_SUPPORT_AVX
  if (is_cpu_extension_supported(BX_ISA_AVX_VNNI_INT8))
    edx |= BX_CPUID_STD7_SUBLEAF1_EDX_AVX_VNNI_INT8;

  //   [5:5]    AVX_NE_CONVERT instructions
  if (is_cpu_extension_supported(BX_ISA_AVX_NE_CONVERT))
    edx |= BX_CPUID_STD7_SUBLEAF1_EDX_AVX_NE_CONVERT;
#endif

  //   [7:6]    reserved

  //   [8:8]    AMX-COMPLEX instructions
#if BX_SUPPORT_AMX
  if (is_cpu_extension_supported(BX_ISA_AMX)) {
    if (is_cpu_extension_supported(BX_ISA_AMX_COMPLEX))
      edx |= BX_CPUID_STD7_SUBLEAF1_EDX_AMX_COMPLEX;
  }
#endif

  //   [9:9]    reserved

  //   [10:10]  AVX-VNNI-INT16 instructions
#if BX_SUPPORT_AVX
  if (is_cpu_extension_supported(BX_ISA_AVX_VNNI_INT16))
    edx |= BX_CPUID_STD7_SUBLEAF1_EDX_AVX_VNNI_INT16;
#endif

  //   [13:11]  reserved
  //   [14:14]  PREFETCHITI: PREFETCHIT0/T1 instruction

  //   [15:15]  USER_MSR: support for URDMSR/UWRMSR instructions
#if BX_SUPPORT_X86_64
  if (is_cpu_extension_supported(BX_ISA_USER_MSR))
    edx |= BX_CPUID_STD7_SUBLEAF1_EDX_USER_MSR;
#endif

  //   [16:16]  reserved

  //   [17:17]  Flexible UIRET: UIRET sets UIF to the RFLAGS[1] image loaded from the stack
  if (is_cpu_extension_supported(BX_ISA_FLEXIBLE_UIRET))
    edx |= BX_CPUID_STD7_SUBLEAF1_EDX_UIRET_UIF;

  //   [18:18]  CET_SSS

  //   [19:19]  AVX10 support and CPUID leaf 0x24
#if BX_SUPPORT_EVEX
  if (is_cpu_extension_supported(BX_ISA_AVX10_1))
    edx |= BX_CPUID_STD7_SUBLEAF1_EDX_AVX10;
#endif

  //   [20:20]  reserved

  //   [21:21]  APX support
#if BX_SUPPORT_APX
  if (is_cpu_extension_supported(BX_ISA_APX))
    edx |= BX_CPUID_STD7_SUBLEAF1_EDX_APX;
#endif

  //   [22:22]  reserved

  //   [23:23]  to be used by VMM: MWAIT and CPUID LEAF5 support (introduced with Monitorless MWAIT support)
#if BX_SUPPORT_MONITOR_MWAIT
  if (is_cpu_extension_supported(BX_ISA_MONITOR_MWAIT))
    if (is_cpu_extension_supported(BX_ISA_MONITORLESS_MWAIT))
      edx |= BX_CPUID_STD7_SUBLEAF1_EDX_MWAIT_AND_LEAF5;
#endif

  //   [31:24]  reserved

  return edx;
}

// leaf 0x00000015 - Time Stamp Counter and Nominal Core Crystal Clock Information
void bx_cpuid_t::get_freq_leaf_15(cpuid_function_t *leaf) const
{
  static unsigned cpuid_freq = SIM->get_param_enum(BXPN_CPUID_FREQ)->get();

  switch(cpuid_freq) {
  case BX_CPUID_FREQ_NONE:
    // EBX=0: TSC/crystal ratio not enumerated, ECX=0: crystal not enumerated;
    // guests fall back to their own timer calibration and measure the true rate
    get_leaf(leaf, 0, 0, 0, 0);
    break;
  case BX_CPUID_FREQ_IPS:
    // TSC frequency = core crystal clock * EBX/EAX: report the true tick rate
    // as a crystal running at 'ips' Hz with a 1/1 ratio
    get_leaf(leaf, 1, 1, (Bit32u) SIM->get_param_num(BXPN_IPS)->get(), 0);
    break;
  case BX_CPUID_FREQ_HARDWARE:
  default:
    get_hardware_cpuid_leaf(0x00000015, 0, leaf);
    break;
  }
}

// leaf 0x00000016 - Processor Frequency Information
void bx_cpuid_t::get_freq_leaf_16(cpuid_function_t *leaf) const
{
  static unsigned cpuid_freq = SIM->get_param_enum(BXPN_CPUID_FREQ)->get();

  switch(cpuid_freq) {
  case BX_CPUID_FREQ_NONE:
    // EAX=0: processor base frequency not enumerated
    get_leaf(leaf, 0, 0, 0, 0);
    break;
  case BX_CPUID_FREQ_IPS:
    {
      // report base and max frequency of the emulated tick rate in MHz
      Bit32u mhz = (Bit32u)((SIM->get_param_num(BXPN_IPS)->get() + 500000) / 1000000);
      get_leaf(leaf, mhz, mhz, 100, 0);
    }
    break;
  case BX_CPUID_FREQ_HARDWARE:
  default:
    get_hardware_cpuid_leaf(0x00000016, 0, leaf);
    break;
  }
}

// Most of the bits in ECX are reserved for Intel
Bit32u bx_cpuid_t::get_ext_cpuid_leaf_1_ecx(Bit32u extra) const
{
  Bit32u ecx = extra;

  // i [0:0]   LAHF/SAHF instructions support in 64-bit mode
  //   [1:1]   CMP_Legacy: Core multi-processing legacy mode (AMD)
  //   [2:2]   SVM: Secure Virtual Machine (AMD)
  //   [3:3]   Extended APIC Space
  //   [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
  // i [5:5]   LZCNT: LZCNT instruction support
  //   [6:6]   SSE4A: SSE4A Instructions support
  //   [7:7]   Misaligned SSE support
  // i [8:8]   PREFETCHW: PREFETCHW instruction support - can be enabled through extra
  //   [9:9]   OSVW: OS visible workarounds CPUID leaf (AMD)
  //   [10:10] IBS: Instruction based sampling (not supported in Bochs)
  //   [11:11] XOP: Extended Operations Support and XOP Prefix
  //   [12:12] SKINIT support (not supported in Bochs)
  //   [13:13] WDT: Watchdog timer support (not supported in Bochs)
  //   [14:14] Reserved
  //   [15:15] LWP: Light weight profiling (not supported in Bochs)
  //   [16:16] FMA4: Four-operand FMA instructions support
  //   [17:17] TCE: Translation Cache Extensions
  //   [18:18] Reserved
  //   [19:19] NodeId: Indicates support for NodeId MSR (0xc001100c)
  //   [20:20] Reserved
  //   [21:21] TBM: trailing bit manipulation instructions support
  //   [22:22] Topology extensions support
  //   [23:23] PerfCtrExtCore: core perf counter extensions support
  //   [24:24] PerfCtrExtNB: NB perf counter extensions support
  //   [25:25] Reserved
  //   [26:26] Data breakpoint extension. Indicates support for MSR 0xC0011027 and MSRs 0xC001101[B:9]
  //   [27:27] Performance time-stamp counter. Indicates support for MSR 0xC0010280
  //   [28:28] PerfCtrExtL2I: L2I performance counter extensions support
  //   [29:29] MONITORX/MWAITX instructions support
  //   [30:30] AddrMaskExt: address mask extension support for instruction breakpoint
  //   [31:31] Reserved

  // i - can also be reported by Intel

#if BX_SUPPORT_X86_64
  // [0:0]   LAHF/SAHF instructions support in 64-bit mode
  if (is_cpu_extension_supported(BX_ISA_LM_LAHF_SAHF))
    ecx |= BX_CPUID_EXT1_ECX_LAHF_SAHF;
#endif

  // [5:5]   LZCNT: LZCNT instruction support
  if (is_cpu_extension_supported(BX_ISA_LZCNT))
    ecx |= BX_CPUID_EXT1_ECX_LZCNT;

  // now AMD specific bits
#if BX_SUPPORT_SVM
  // [2:2]   SVM: Secure Virtual Machine (AMD)
  if (is_cpu_extension_supported(BX_ISA_SVM))
    ecx |= BX_CPUID_EXT1_ECX_SVM;
#endif

  // [3:3]   Extended APIC Space
  if (is_cpu_extension_supported(BX_ISA_XAPIC_EXT))
    ecx |= BX_CPUID_EXT1_ECX_EXT_APIC_SPACE;

  // [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
  if (is_cpu_extension_supported(BX_ISA_ALT_MOV_CR8))
    ecx |= BX_CPUID_EXT1_ECX_ALT_MOV_CR8;

  // [6:6]   SSE4A: SSE4A Instructions support
  if (is_cpu_extension_supported(BX_ISA_SSE4A))
    ecx |= BX_CPUID_EXT1_ECX_SSE4A;

  // [7:7]   Misaligned SSE support
  if (is_cpu_extension_supported(BX_ISA_MISALIGNED_SSE))
    ecx |= BX_CPUID_EXT1_ECX_MISALIGNED_SSE;

  // [11:11] XOP: Extended Operations Support and XOP Prefix
  if (is_cpu_extension_supported(BX_ISA_XOP))
    ecx |= BX_CPUID_EXT1_ECX_XOP;

  // [16:16] FMA4: Four-operand FMA instructions support
  if (is_cpu_extension_supported(BX_ISA_FMA4))
    ecx |= BX_CPUID_EXT1_ECX_FMA4;

  // [17:17] TCE: Translation Cache Extensions
  if (is_cpu_extension_supported(BX_ISA_TCE))
    ecx |= BX_CPUID_EXT1_ECX_TCE;

  // [21:21] TBM: trailing bit manipulation instructions support
  if (is_cpu_extension_supported(BX_ISA_TBM))
    ecx |= BX_CPUID_EXT1_ECX_TBM;

#if BX_SUPPORT_MONITOR_MWAIT
  // [29:29] MONITORX/MWAITX instructions support
  if (is_cpu_extension_supported(BX_ISA_MONITORX_MWAITX))
    ecx |= BX_CPUID_EXT1_ECX_MONITORX_MWAITX;
#endif

  return ecx;
}

// Most of the bits in EDX are reserved for Intel
Bit32u bx_cpuid_t::get_ext_cpuid_leaf_1_edx_intel() const
{
  Bit32u edx = 0;

  //  [10:0] Reserved for Intel

  // [11:11] SYSCALL/SYSRET support
  if (cpu->long64_mode())
    edx |= BX_CPUID_EXT1_EDX_SYSCALL_SYSRET;

  // [19:12] Reserved for Intel

  // [20:20] No-Execute page protection
  if (is_cpu_extension_supported(BX_ISA_NX))
    edx |= BX_CPUID_EXT1_EDX_NX;

  // [25:21] Reserved for Intel

  // [26:26] 1G paging support
  if (is_cpu_extension_supported(BX_ISA_1G_PAGES))
    edx |= BX_CPUID_EXT1_EDX_1G_PAGES;

  // [27:27] Support RDTSCP Instruction
  if (is_cpu_extension_supported(BX_ISA_RDTSCP))
    edx |= BX_CPUID_EXT1_EDX_RDTSCP;

  // [28:28] Reserved

  // [29:29] Long Mode
  if (is_cpu_extension_supported(BX_ISA_LONG_MODE))
    edx |= BX_CPUID_EXT1_EDX_LONG_MODE;

  // [30:30] AMD 3DNow! Extensions
  // [31:31] AMD 3DNow! Instructions

  return edx;
}

// Many of the bits in EDX are the same as FN 0x00000001 for AMD
Bit32u bx_cpuid_t::get_ext_cpuid_leaf_1_edx_amd(Bit32u extra) const
{
  Bit32u edx = get_std_cpuid_leaf_1_edx_common(extra);

  // [*] indicates common bits
  // * [0:0]   FPU on chip
  // * [1:1]   VME: Virtual-8086 Mode enhancements
  // * [2:2]   DE: Debug Extensions (I/O breakpoints)
  // * [3:3]   PSE: Page Size Extensions
  // * [4:4]   TSC: Time Stamp Counter
  // * [5:5]   MSR: RDMSR and WRMSR support
  // * [6:6]   PAE: Physical Address Extensions
  // * [7:7]   MCE: Machine Check Exception
  // * [8:8]   CXS: CMPXCHG8B instruction
  // * [9:9]   APIC: APIC on Chip
  // * [10:10] Reserved
  //   [11:11] SYSCALL/SYSRET support
  // * [12:12] MTRR: Memory Type Range Reg
  // * [13:13] PGE/PTE Global Bit
  // * [14:14] MCA: Machine Check Architecture
  // * [15:15] CMOV: Cond Mov/Cmp Instructions
  // * [16:16] PAT: Page Attribute Table
  // * [17:17] PSE-36: Physical Address Extensions
  //   [19:18] Reserved
  if (is_cpu_extension_supported(BX_ISA_SYSCALL_SYSRET_LEGACY)) // only uncommon bit
    edx |= BX_CPUID_EXT1_EDX_SYSCALL_SYSRET;

  // [20:20] No-Execute page protection
  if (is_cpu_extension_supported(BX_ISA_NX))
    edx |= BX_CPUID_EXT1_EDX_NX;

  // [21:21] Reserved

  // [22:22] AMD MMX Extensions <- some Intel's SSE instructions were done in AMD under this name
  if (is_cpu_extension_supported(BX_ISA_SSE) || is_cpu_extension_supported(BX_ISA_3DNOW_EXT))
    edx |= BX_CPUID_EXT1_EDX_AMD_MMX_EXT;
  
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)

  // [25:25] Fast FXSAVE/FXRSTOR mode support
  if (is_cpu_extension_supported(BX_ISA_FFXSR))
    edx |= BX_CPUID_EXT1_EDX_FFXSR;

  // [26:26] 1G paging support
  if (is_cpu_extension_supported(BX_ISA_1G_PAGES))
    edx |= BX_CPUID_EXT1_EDX_1G_PAGES;

  // [27:27] Support RDTSCP Instruction
  if (is_cpu_extension_supported(BX_ISA_RDTSCP))
    edx |= BX_CPUID_EXT1_EDX_RDTSCP;

  // [28:28] Reserved

  // [29:29] Long Mode
  if (is_cpu_extension_supported(BX_ISA_LONG_MODE))
    edx |= BX_CPUID_EXT1_EDX_LONG_MODE;

  // [30:30] AMD 3DNow! Extensions (3DNow!+)
  // [31:31] AMD 3DNow! Instructions
#if BX_SUPPORT_3DNOW
  if (is_cpu_extension_supported(BX_ISA_3DNOW)) {
    edx |= BX_CPUID_EXT1_EDX_3DNOW;
    if (is_cpu_extension_supported(BX_ISA_3DNOW_EXT))
      edx |= BX_CPUID_EXT1_EDX_3DNOW_EXT;
  }
#endif
  
  return edx;
}

// leaf 0x80000002 - 0x80000004 //
void bx_cpuid_t::get_ext_cpuid_brand_string_leaf(Bit32u function, cpuid_function_t *leaf) const
{
  static const char *brand_string = (const char *)SIM->get_param_string(BXPN_BRAND_STRING)->getptr();
  if (! brand_string || ! *brand_string) {
    // no brand string override, report the brand string of the hardware model
    get_hardware_cpuid_leaf(function, 0, leaf);
    return;
  }

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

// leaf 0x80000008 //
void bx_cpuid_t::get_ext_cpuid_leaf_8(cpuid_function_t *leaf) const
{
  // virtual & phys address size in low 2 bytes of EAX.
  // TODO: physical address width should be 32-bit when no PSE-36 is supported
  Bit32u phy_addr_width = BX_CPU_C::maxphyaddr;
  Bit32u lin_addr_width = is_cpu_extension_supported(BX_ISA_LONG_MODE) ? 
                                (is_cpu_extension_supported(BX_ISA_LA57) ? 57 : 48) : 32;

  leaf->eax = phy_addr_width | (lin_addr_width << 8);

  //   [0:0] CLZERO support
  //   [1:1] Instruction Retired Counter MSR available
  //   [2:2] FP Error Pointers Restored by XRSTOR
  //   [3:3] reserved
  //   [4:4] RDPRU support
  //   [5:5] reserved
  //   [6:6] Memory Bandwidth Enforcement (MBE) support
  //   [8:7] reserved
  //   [9:9] WBNOINVD support - when not supported fall back to legacy WBINVD
  // [11:10] reserved
  // [12:12] IBTB: Indirect Branch Prediction Barrier
  // [13:13] INT_WBINVD: WBINVD/WBNOINVD instructions are interruptible
  // [14:14] IBRS: Indirect Branch Restricted Speculation
  // [15:15] STIBP: Single Thread Indirect Branch Prediction mode
  // [16:16] IbrsAlwaysOn
  // [17:17] StibpAlwaysOn
  // [18:18] IbrsPreferred
  // [19:19] IbrsSameMode
  // [20:20] EferLmsleUnsupported: EFER.LMSLE is unsupported
  // [21:21] INVLPGBnestedPages: INVLPGB support for invalidating guest nested translations
  // [23:22] reserved
  // [24:24] SSBD: Speculative Store Bypass Disable
  // [25:25] SsbdVirtSpecCtrl: use VIRT_SPEC_CTL MSR (C001_011Fh) for SSBD
  // [26:26] SsbdNotRequired: SSBD not needed on this processor
  // [27:27] CPPC: Collaborative Processor Performance Control
  // [28:28] PSFD: Predictive Store Forward Disable
  // [29:29] BTC_NO: The processor is not affected by branch type confusion
  // [30:30] IBPB_RET: IBPB clears return address predictor
  // [31:31] reserved
  leaf->ebx = 0;
  if (is_cpu_extension_supported(BX_ISA_CLZERO))
    leaf->ebx |= BX_CPUID_EXT8_EBX_CLZERO;
  if (is_cpu_extension_supported(BX_ISA_RDPRU))
    leaf->ebx |= BX_CPUID_EXT8_EBX_RDPRU;
  if (is_cpu_extension_supported(BX_ISA_LONG_MODE))
    leaf->ebx |= BX_CPUID_EXT8_EBX_WBNOINVD; // I don't add CPUID feature for WBNOINVD but also don't want to report it for really old processors

  leaf->ecx = 0; // Reserved, undefined for Intel

  // EDX[31:16]: MaxRdpruID - maximum ECX value recognized by RDPRU (MPERF=0, APERF=1)
  leaf->edx = is_cpu_extension_supported(BX_ISA_RDPRU) ? (1 << 16) : 0;
}

void bx_cpuid_t::get_ext_cpuid_leaf_8_amd(cpuid_function_t *leaf) const
{
  bx_cpuid_t::get_ext_cpuid_leaf_8(leaf);

  // leaf 0x80000008 ECX (AMD): logical processor count and APIC ID size
  //   [17:16] PerfTscSize: performance time-stamp counter size (PTSC is not supported, report 0)
  //   [15:12] ApicIdSize: number of APIC ID bits that identify logical processor within the package,
  //                       zero means legacy method (number of logical processors = NC+1)
  //    [11:8] reserved
  //     [7:0] NC: number of logical processors (threads) in the package - 1
  Bit32u n_logical_processors = ncores * nthreads;
  Bit32u apic_id_size = 0;

#if BX_SUPPORT_SMP
  if (n_logical_processors > 1)
    apic_id_size = ilog2(n_logical_processors - 1) + 1;
#endif

  leaf->ecx = (apic_id_size << 12) | (n_logical_processors - 1);
}

// CPUID function 0x80000001E - Topology Extensions
void bx_cpuid_t::get_ext_cpuid_leaf_1E(cpuid_function_t *leaf) const
{
#if BX_SUPPORT_APIC
  Bit32u apic_id = cpu->get_apic_id();
  Bit32u core_id = (apic_id / nthreads) % ncores;

  leaf->eax = apic_id;
  leaf->ebx = ((nthreads - 1) << 8) | core_id;
  leaf->ecx = 0;
  leaf->edx = 0;
#else
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;
#endif
}

// CPUID function 0x80000026 - AMD Extended CPU Topology
void bx_cpuid_t::get_ext_cpuid_leaf_26(Bit32u subfunction, cpuid_function_t *leaf) const
{
  // EAX:
  //   [04-00] - MaskWidth: number of bits to shift Extended APIC ID right to get
  //             a unique topology ID of the current hierarchy level
  //   [28-05] - Reserved
  //      [29] - EfficiencyRankingAvailable (Core level only)
  //      [30] - HeterogeneousCores: components at this level have different core types
  //      [31] - AsymmetricTopology: components at this level have different NumLogProc
  // EBX:
  //   [15-00] - NumLogProc: number of logical processors at the current hierarchy level
  //   [23-16] - PwrEfficiencyRanking (Core level only)
  //   [27-24] - NativeModelID (Core level only)
  //   [31-28] - CoreType (Core level only)
  // ECX:
  //   [07-00] - InputEcx: input ECX[7:0]
  //   [15-08] - LevelType: hierarchy level type
  //             0 = Reserved (no more levels)
  //             1 = Core
  //             2 = Complex
  //             3 = Die
  //             4 = Socket
  //   [31-16] - Reserved
  // EDX: Extended APIC ID of the logical processor

  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = subfunction & 0xff;
#if BX_SUPPORT_APIC
  leaf->edx = cpu->get_apic_id();
#else
  leaf->edx = 0;
#endif

  enum {
    BX_AMD_LEVEL_TYPE_INVALID = 0,
    BX_AMD_LEVEL_TYPE_CORE = 1,
    BX_AMD_LEVEL_TYPE_COMPLEX = 2,
    BX_AMD_LEVEL_TYPE_DIE = 3,
    BX_AMD_LEVEL_TYPE_SOCKET = 4
  };

  // number of APIC ID bits allocated for threads and cores
  Bit32u thread_bits = (nthreads > 1) ? (ilog2(nthreads-1)+1) : 0;
  Bit32u core_bits = (ncores > 1) ? (ilog2(ncores-1)+1) : 0;

  // All four levels are enumerated like real hardware does. Bochs topology has
  // a single complex per die and a single die per socket, so the complex, die
  // and socket levels report the same values. The topology is symmetric and
  // homogeneous, EAX[31-29] and EBX[31-16] are zero.
  switch(subfunction) {
  case 0:
     leaf->eax = thread_bits;
     leaf->ebx = nthreads;
     leaf->ecx |= (BX_AMD_LEVEL_TYPE_CORE<<8);
     break;

  case 1:
     leaf->eax = thread_bits + core_bits;
     leaf->ebx = ncores * nthreads;
     leaf->ecx |= (BX_AMD_LEVEL_TYPE_COMPLEX<<8);
     break;

  case 2:
     leaf->eax = thread_bits + core_bits;
     leaf->ebx = ncores * nthreads;
     leaf->ecx |= (BX_AMD_LEVEL_TYPE_DIE<<8);
     break;

  case 3:
     leaf->eax = thread_bits + core_bits;
     leaf->ebx = ncores * nthreads;
     leaf->ecx |= (BX_AMD_LEVEL_TYPE_SOCKET<<8);
     break;

  default:
     break;
  }
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

void bx_cpuid_t::get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  // limit max std CPUID leaf to 2, do not limit extended CPUID leafs
  static bool cpuid_limit_winnt = SIM->get_param_bool(BXPN_CPUID_LIMIT_WINNT)->get();
  bool limit_winnt = cpuid_limit_winnt && max_std_leaf > 2;
  Bit32u max_leaf = limit_winnt ? 2 : max_std_leaf;

  // leafs above max leaf return data of max std leaf
  if (function < 0x80000000) {
    if (function > max_leaf) function = max_leaf;
  }
  else {
    // magic leaf is not limited by max ext leaf
    if (function > max_ext_leaf && function != 0x8FFFFFFF) function = max_leaf;
  }

  switch(function) {
  case 0x00000000: // CPUID leaf 0x00000000 - Highest Basic Leaf and Vendor Identification
  case 0x00000001: // CPUID leaf 0x00000001 - Version and Feature Information
  case 0x00000002: // CPUID leaf 0x00000002 - Cache and TLB Descriptors
  case 0x00000003: // CPUID leaf 0x00000003 - Processor Serial Number
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x00000004: // CPUID leaf 0x00000004 - Deterministic Cache Parameters
    get_std_cpuid_deterministic_cache_leaf(subfunction, leaf);
    break;
  case 0x00000005: // CPUID leaf 0x00000005 - MONITOR/MWAIT Features
    get_std_cpuid_monitor_mwait_leaf(leaf);
    break;
  case 0x00000006: // CPUID leaf 0x00000006 - Thermal and Power Management Features
  case 0x00000007: // CPUID leaf 0x00000007 - Structured Extended Feature Enumeration
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x00000008: // CPUID leaf 0x00000008 - reserved
  case 0x00000009: // CPUID leaf 0x00000009 - Direct Cache Access Information (not implemented)
    get_reserved_leaf(leaf);
    break;
  case 0x0000000A: // CPUID leaf 0x0000000A - Architectural Performance Monitoring
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x0000000B: // CPUID leaf 0x0000000B - Extended Topology Enumeration
    // the leaf is supported when hardware reports non-zero CPUID.0BH.00H:EBX,
    // the topology itself is always computed
    get_hardware_cpuid_leaf(0x0000000B, 0, leaf);
#if BX_SUPPORT_APIC
    if (leaf->ebx != 0)
      get_std_cpuid_extended_topology_leaf(subfunction, leaf);
    else
#endif
      get_reserved_leaf(leaf);
    break;
  case 0x0000000C: // CPUID leaf 0x0000000C - reserved
    get_reserved_leaf(leaf);
    break;
  case 0x0000000D: // CPUID leaf 0x0000000D - Processor Extended States Enumeration
#if BX_CPU_LEVEL >= 6
    get_std_cpuid_xsave_leaf(subfunction, leaf);
#else
    get_reserved_leaf(leaf);
#endif
    break;
  case 0x0000000E: // CPUID leaf 0x0000000E - reserved
  case 0x0000000F: // CPUID leaf 0x0000000F - Intel Resource Director Technology Monitoring (not implemented)
  case 0x00000010: // CPUID leaf 0x00000010 - Intel Resource Director Technology Allocation (not implemented)
  case 0x00000011: // CPUID leaf 0x00000011 - reserved
  case 0x00000012: // CPUID leaf 0x00000012 - Intel SGX Enumeration (not implemented)
  case 0x00000013: // CPUID leaf 0x00000013 - reserved
  case 0x00000014: // CPUID leaf 0x00000014 - Intel Processor Trace Enumeration (not implemented)
    get_reserved_leaf(leaf);
    break;
  case 0x00000015: // CPUID leaf 0x00000015 - Time Stamp Counter and Nominal Core Crystal Clock
    get_freq_leaf_15(leaf);
    break;
  case 0x00000016: // CPUID leaf 0x00000016 - Processor Frequency Information
    get_freq_leaf_16(leaf);
    break;
  case 0x00000017: // CPUID leaf 0x00000017 - System-On-Chip Vendor Attribute Enumeration
    get_reserved_leaf(leaf);
    break;
  case 0x00000018: // CPUID leaf 0x00000018 - Deterministic Address Translation Parameters
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x00000019: // CPUID leaf 0x00000019 - Key Locker (not implemented)
  case 0x0000001A: // CPUID leaf 0x0000001A - Hybrid Information (Native Model ID)
  case 0x0000001B: // CPUID leaf 0x0000001B - PCONFIG Information (not implemented)
  case 0x0000001C: // CPUID leaf 0x0000001C - Architectural Last Branch Records (not implemented)
    get_reserved_leaf(leaf);
    break;
#if BX_SUPPORT_AMX
  case 0x0000001D: // CPUID leaf 0x0000001D - AMX Tile Information
    get_std_cpuid_amx_palette_info_leaf(subfunction, leaf);
    break;
  case 0x0000001E: // CPUID leaf 0x0000001E - AMX TMUL Information
    get_std_cpuid_amx_tmul_leaf(subfunction, leaf);
    break;
#else
  case 0x0000001D: // CPUID leaf 0x0000001D - AMX Tile Information
  case 0x0000001E: // CPUID leaf 0x0000001E - AMX TMUL Information
    get_reserved_leaf(leaf);
    break;
#endif
  case 0x0000001F: // CPUID leaf 0x0000001F - V2 Extended Topology Enumeration
    // the leaf is supported when hardware reports non-zero CPUID.1FH.00H:EBX,
    // the topology itself is always computed
    get_hardware_cpuid_leaf(0x0000001F, 0, leaf);
#if BX_SUPPORT_APIC
    if (leaf->ebx != 0)
      get_std_cpuid_extended_topology_leaf(subfunction, leaf);
    else
#endif
      get_reserved_leaf(leaf);
    break;
  case 0x00000020: // CPUID leaf 0x00000020 - Processor History Reset (HRESET)
  case 0x00000021: // CPUID leaf 0x00000021 - reserved
  case 0x00000022: // CPUID leaf 0x00000022 - reserved
  case 0x00000023: // CPUID leaf 0x00000023 - Architectural Performance Monitoring Extended
    get_reserved_leaf(leaf);
    break;
  case 0x00000024: // CPUID leaf 0x00000024 - Intel AVX10 Converged Vector ISA
    get_std_cpuid_avx10_leaf(subfunction, leaf);
    break;
  case 0x00000025: // CPUID leaf 0x00000025 - reserved
  case 0x00000026: // CPUID leaf 0x00000026 - reserved
  case 0x00000027: // CPUID leaf 0x00000027 - Intel RDT Asymmetric Monitoring
  case 0x00000028: // CPUID leaf 0x00000028 - Intel RDT Asymmetric Allocation
    get_reserved_leaf(leaf);
    break;

  //
  // extended CPUID leaves 0x80000000
  //
  case 0x80000000: // CPUID leaf 0x80000000 - Highest Extended Leaf and Vendor Identification
  case 0x80000001: // CPUID leaf 0x80000001 - Extended Processor Signature and Feature Bits
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x80000002: // CPUID leaf 0x80000002 - Processor Brand String
  case 0x80000003: // CPUID leaf 0x80000003 - Processor Brand String
  case 0x80000004: // CPUID leaf 0x80000004 - Processor Brand String
    get_ext_cpuid_brand_string_leaf(function, leaf);
    break;
  case 0x80000005: // CPUID leaf 0x80000005 - L1 Cache and TLB Identifiers
  case 0x80000006: // CPUID leaf 0x80000006 - L2 Cache and TLB Identifiers
  case 0x80000007: // CPUID leaf 0x80000007 - Advanced Power Management
  case 0x80000008: // CPUID leaf 0x80000008 - Address Size and Processor Capacity
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x80000009: // CPUID leaf 0x80000009 - reserved
    get_reserved_leaf(leaf);
    break;
  case 0x8000000A: // CPUID leaf 0x8000000A - SVM Revision and Feature Identification
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x8000000B: // CPUID leaf 0x8000000B - reserved
  case 0x8000000C: // CPUID leaf 0x8000000C - reserved
  case 0x8000000D: // CPUID leaf 0x8000000D - reserved
  case 0x8000000E: // CPUID leaf 0x8000000E - reserved
  case 0x8000000F: // CPUID leaf 0x8000000F - reserved
  case 0x80000010: // CPUID leaf 0x80000010 - reserved
  case 0x80000011: // CPUID leaf 0x80000011 - reserved
  case 0x80000012: // CPUID leaf 0x80000012 - reserved
  case 0x80000013: // CPUID leaf 0x80000013 - reserved
  case 0x80000014: // CPUID leaf 0x80000014 - reserved
  case 0x80000015: // CPUID leaf 0x80000015 - reserved
  case 0x80000016: // CPUID leaf 0x80000016 - reserved
  case 0x80000017: // CPUID leaf 0x80000017 - reserved
  case 0x80000018: // CPUID leaf 0x80000018 - reserved
    get_reserved_leaf(leaf);
    break;
  case 0x80000019: // CPUID leaf 0x80000019 - 1G Page TLB Identifiers
  case 0x8000001A: // CPUID leaf 0x8000001A - Performance Optimization Identifiers
  case 0x8000001B: // CPUID leaf 0x8000001B - Instruction Based Sampling Identifiers (not implemented)
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x8000001C: // CPUID leaf 0x8000001C - Lightweight Profiling Capabilities (not implemented)
    get_reserved_leaf(leaf);
    break;
  case 0x8000001D: // CPUID leaf 0x8000001D - Cache Topology Information
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x8000001E: // CPUID leaf 0x8000001E - AMD Processor Topology Information
    get_ext_cpuid_leaf_1E(leaf);
    break;
  case 0x8000001F: // CPUID leaf 0x8000001F - SEV Capabilities (not implemented)
  case 0x80000020: // CPUID leaf 0x80000020 - PQOS Extended Features (not implemented)
    get_reserved_leaf(leaf);
    break;
  case 0x80000021: // CPUID leaf 0x80000021 - Extended Feature Identification 2
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  case 0x80000022: // CPUID leaf 0x80000022 - Extended Performance Monitoring and Debug
  case 0x80000023: // CPUID leaf 0x80000023 - Multi-Key Encrypted Memory Capabilities
  case 0x80000024: // CPUID leaf 0x80000024 - reserved
  case 0x80000025: // CPUID leaf 0x80000025 - SEV Capabilities 2
    get_reserved_leaf(leaf);
    break;
  case 0x80000026: // CPUID leaf 0x80000026 - Extended CPU Topology
    get_ext_cpuid_leaf_26(subfunction, leaf);
    break;
  case 0x8FFFFFFF: // CPUID leaf 0x8FFFFFFF - AMD magic (hidden) leaf
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  default:         // not reachable: max_std_leaf <= 0x28 and max_ext_leaf <= 0x80000026 for all models
    get_hardware_cpuid_leaf(function, subfunction, leaf);
    break;
  }

  if (limit_winnt && function == 0)
    leaf->eax = 2;

  if (function == 0xA)
    BX_INFO(("WARNING: Architectural Performance Monitoring is not implemented"));

  if (function == 0x8000001B)
    BX_INFO(("WARNING: Instruction Based Sampling is not implemented"));
}

void bx_cpuid_t::dump_cpuid_leaf(unsigned function, unsigned subfunction) const
{
  struct cpuid_function_t leaf;
  get_cpuid_leaf(function, subfunction, &leaf);
  BX_INFO(("CPUID[0x%08x]: %08x %08x %08x %08x", function, leaf.eax, leaf.ebx, leaf.ecx, leaf.edx));
}

void bx_cpuid_t::dump_cpuid(void) const
{
  dump_cpuid(max_std_leaf, max_ext_leaf);
}

void bx_cpuid_t::dump_cpuid(unsigned max_std_leaf, unsigned max_ext_leaf) const
{
  if (max_std_leaf) {
    for (unsigned std_leaf=0; std_leaf <= max_std_leaf; std_leaf++) {
      dump_cpuid_leaf(std_leaf);
    }
  }

  if (max_ext_leaf) {
    for (unsigned ext_leaf=0x80000000; ext_leaf <= max_ext_leaf; ext_leaf++) {
      dump_cpuid_leaf(ext_leaf);
    }
  }
}

void bx_cpuid_t::warning_messages(unsigned extension) const
{
  switch(extension) {
  case BX_ISA_RDRAND:
    BX_INFO(("WARNING: RDRAND would not produce true random numbers !"));
    break;
  case BX_ISA_RDSEED:
    BX_INFO(("WARNING: RDSEED would not produce true random numbers !"));
    break;
  default:
    break;
  }
}

void bx_cpuid_t::sanity_checks() const
{
  // Intel and AMD speculation control define different SPEC_CTRL MSR bits
  if (is_cpu_extension_supported(BX_ISA_SPEC_CTRL) && is_cpu_extension_supported(BX_ISA_SPEC_CTRL_AMD))
    BX_FATAL(("PANIC: Intel and AMD speculation control features (spec_ctrl and spec_ctrl_amd) cannot be enabled together !"));

  // P6 -> PENTIUM -> 486 -> 386
  if (is_cpu_extension_supported(BX_ISA_486) && ! is_cpu_extension_supported(BX_ISA_386))
    BX_FATAL(("PANIC: 80386 ISA must be enabled for any 80486 ad above CPU model !"));
  if (is_cpu_extension_supported(BX_ISA_PENTIUM) && ! is_cpu_extension_supported(BX_ISA_486))
    BX_FATAL(("PANIC: 80486 ISA must be enabled for any Pentium and above CPU model !"));
  if (is_cpu_extension_supported(BX_ISA_P6) && ! is_cpu_extension_supported(BX_ISA_PENTIUM))
    BX_FATAL(("PANIC: Pentium ISA must be enabled for any P6 and above CPU model !"));
  if (is_cpu_extension_supported(BX_ISA_PENTIUM) && ! is_cpu_extension_supported(BX_ISA_X87))
    BX_FATAL(("PANIC: FPU must be enabled for any Pentium and above CPU model !"));

  if (is_cpu_extension_supported(BX_ISA_LONG_MODE)) {
    if (! is_cpu_extension_supported(BX_ISA_PAE))
      BX_FATAL(("PANIC: Long mode is enabled when PAE is not supported !"));
    if (! is_cpu_extension_supported(BX_ISA_MSR))
      BX_FATAL(("PANIC: Long mode is enabled when MSRs are not supported !"));
  }

  if (is_cpu_extension_supported(BX_ISA_X2APIC) && !is_cpu_extension_supported(BX_ISA_MSR))
    BX_FATAL(("PANIC: X2APIC is enabled when MSRs are not supported !"));

  // 3DNow! -> MMX
  if (is_cpu_extension_supported(BX_ISA_3DNOW) && ! is_cpu_extension_supported(BX_ISA_MMX))
    BX_FATAL(("PANIC: 3dnow! ISA require MMX to be enabled !"));
  if (is_cpu_extension_supported(BX_ISA_3DNOW_EXT) && ! is_cpu_extension_supported(BX_ISA_3DNOW))
    BX_FATAL(("PANIC: 3dnow! Extensions ISA require 3dnow! to be enabled !"));

  // AVX10_VL512 or AVX10_2 -> AVX10_1 -> AVX2 -> AVX -> XSAVE -> SSE -> MMX
  if (is_cpu_extension_supported(BX_ISA_AVX10_2) && !is_cpu_extension_supported(BX_ISA_AVX10_1))
    BX_FATAL(("PANIC: AVX10_2 is reqired AVX10_1 to be enabled !"));
  if (is_cpu_extension_supported(BX_ISA_AVX10_1) && !is_cpu_extension_supported(BX_ISA_AVX2))
    BX_FATAL(("PANIC: AVX10 is enabled when AVX2 is not supported !"));
  if (is_cpu_extension_supported(BX_ISA_AVX512) && !is_cpu_extension_supported(BX_ISA_AVX2))
    BX_FATAL(("PANIC: AVX-512 is enabled when AVX2 is not supported !"));
  if (is_cpu_extension_supported(BX_ISA_AVX2) && !is_cpu_extension_supported(BX_ISA_AVX))
    BX_FATAL(("PANIC: AVX2 is enabled when AVX is not supported !"));
  if (is_cpu_extension_supported(BX_ISA_AVX) && !is_cpu_extension_supported(BX_ISA_XSAVE))
    BX_FATAL(("PANIC: AVX is enabled when XSAVE is not supported !"));
  if (is_cpu_extension_supported(BX_ISA_XSAVE) && !is_cpu_extension_supported(BX_ISA_SSE))
    BX_FATAL(("PANIC: XSAVE is enabled when SSE is not supported !"));
  if (is_cpu_extension_supported(BX_ISA_SSE) && !is_cpu_extension_supported(BX_ISA_MMX))
    BX_FATAL(("PANIC: SSE is enabled when MMX is not supported !"));

  if (is_cpu_extension_supported(BX_ISA_SSE) && !is_cpu_extension_supported(BX_ISA_P6))
    BX_FATAL(("PANIC: SSE is enabled when P6 FXSAVE/FXRSTOR instructions are not supported !"));

  // XSAVEOPT/XSAVES/XSAVEC -> XSAVE
  if (is_cpu_extension_supported(BX_ISA_XSAVEOPT) && !is_cpu_extension_supported(BX_ISA_XSAVE))
    BX_FATAL(("PANIC: XSAVEOPT is enabled when XSAVE is not supported !"));
  if (is_cpu_extension_supported(BX_ISA_XSAVES) && !is_cpu_extension_supported(BX_ISA_XSAVE))
    BX_FATAL(("PANIC: XSAVES is enabled when XSAVE is not supported !"));
  if (is_cpu_extension_supported(BX_ISA_XSAVEC) && !is_cpu_extension_supported(BX_ISA_XSAVE))
    BX_FATAL(("PANIC: XSAVEC is enabled when XSAVE is not supported !"));

  // AVX-512 extensions -> AVX-512
  if (! is_cpu_extension_supported(BX_ISA_AVX512)) {
    if (is_cpu_extension_supported(BX_ISA_AVX512_DQ) ||
        is_cpu_extension_supported(BX_ISA_AVX512_BW) ||
        is_cpu_extension_supported(BX_ISA_AVX512_CD) ||
        is_cpu_extension_supported(BX_ISA_AVX512_VBMI) ||
        is_cpu_extension_supported(BX_ISA_AVX512_VBMI2) ||
        is_cpu_extension_supported(BX_ISA_AVX512_IFMA52) ||
        is_cpu_extension_supported(BX_ISA_AVX512_VPOPCNTDQ) ||
        is_cpu_extension_supported(BX_ISA_AVX512_VNNI) ||
        is_cpu_extension_supported(BX_ISA_AVX512_BITALG) ||
        is_cpu_extension_supported(BX_ISA_AVX512_VP2INTERSECT) ||
        is_cpu_extension_supported(BX_ISA_AVX512_BF16) ||
        is_cpu_extension_supported(BX_ISA_AVX512_FP16)) 
    {
      BX_FATAL(("PANIC: AVX-512 extensions must be disabled if AVX-512 is not supported !"));
    }

    if (is_cpu_extension_supported(BX_ISA_AMX))
      BX_FATAL(("PANIC: AMX extensions must be disabled if AVX-512 is not supported !"));
  }

  if (! is_cpu_extension_supported(BX_ISA_CLFLUSH)) {
    if (is_cpu_extension_supported(BX_ISA_CLFLUSHOPT))
      BX_FATAL(("PANIC: CLFLUSHOPT enabled when CLFLUSH is not supported !"));
    if (is_cpu_extension_supported(BX_ISA_CLWB))
      BX_FATAL(("PANIC: CLWB enabled when CLFLUSH is not supported !"));
  }

  // RDPID -> RDTSCP, RDPID reads MSR_TSC_AUX which is enabled by RDTSCP
  if (is_cpu_extension_supported(BX_ISA_RDPID) && !is_cpu_extension_supported(BX_ISA_RDTSCP))
    BX_FATAL(("PANIC: RDPID is enabled when RDTSCP is not supported !"));

  // TBM -> XOP -> AVX
  if (is_cpu_extension_supported(BX_ISA_TBM) && !is_cpu_extension_supported(BX_ISA_XOP))
    BX_FATAL(("PANIC: TBM is enabled when XOP is not supported !"));
  if (is_cpu_extension_supported(BX_ISA_XOP) && !is_cpu_extension_supported(BX_ISA_AVX))
    BX_FATAL(("PANIC: XOP is enabled when AVX is not supported !"));

  // SSE2/SSE3/SSSE3/SSE4_1/SSE4_2/SSE4A/MISALIGNED_SSE -> SSE
  if (! is_cpu_extension_supported(BX_ISA_SSE)) {
    if (is_cpu_extension_supported(BX_ISA_SSE2) ||
        is_cpu_extension_supported(BX_ISA_SSE3) ||
        is_cpu_extension_supported(BX_ISA_SSSE3) ||
        is_cpu_extension_supported(BX_ISA_SSE4_1) ||
        is_cpu_extension_supported(BX_ISA_SSE4_2) ||
        is_cpu_extension_supported(BX_ISA_SSE4A) ||
        is_cpu_extension_supported(BX_ISA_MISALIGNED_SSE))
    {
      BX_FATAL(("PANIC: All SSE extensions must be disabled if SSE is not supported !"));
    }
  }

  if (! is_cpu_extension_supported(BX_ISA_AVX)) {
    if (is_cpu_extension_supported(BX_ISA_AVX_IFMA) ||
        is_cpu_extension_supported(BX_ISA_AVX_VNNI) ||
        is_cpu_extension_supported(BX_ISA_AVX_VNNI_INT8) ||
        is_cpu_extension_supported(BX_ISA_AVX_VNNI_INT16) ||
        is_cpu_extension_supported(BX_ISA_AVX_NE_CONVERT) ||
        is_cpu_extension_supported(BX_ISA_AVX_F16C) ||
        is_cpu_extension_supported(BX_ISA_AVX_FMA))
    {
      BX_FATAL(("PANIC: All AVX extensions must be disabled if AVX is not supported !"));
    }

    if (is_cpu_extension_supported(BX_ISA_FMA4))
      BX_FATAL(("PANIC: FMA4 extension must be disabled if AVX is not supported !"));
  }

  if (! is_cpu_extension_supported(BX_ISA_AMX)) {
    if (is_cpu_extension_supported(BX_ISA_AMX_INT8) ||
        is_cpu_extension_supported(BX_ISA_AMX_BF16) ||
        is_cpu_extension_supported(BX_ISA_AMX_FP16) ||
        is_cpu_extension_supported(BX_ISA_AMX_COMPLEX) ||
        is_cpu_extension_supported(BX_ISA_AMX_MOVRS) ||
        is_cpu_extension_supported(BX_ISA_AMX_AVX512) ||
        is_cpu_extension_supported(BX_ISA_ACE))
    {
      BX_FATAL(("PANIC: All AMX/TMUL extensions must be disabled if AMX is not supported !"));
    }
  }

  if (! is_cpu_extension_supported(BX_ISA_AVX10_2)) {
    if (is_cpu_extension_supported(BX_ISA_AMX_AVX512) ||
        is_cpu_extension_supported(BX_ISA_ACE))
    {
      BX_FATAL(("PANIC: AVX10_2 required to enable ACE or AMX_AVX512 !"));
    }
  }

  // ACE extends AMX_AVX512 instructions (TILEMOVROW, TCVTROW*)
  if (is_cpu_extension_supported(BX_ISA_ACE) && ! is_cpu_extension_supported(BX_ISA_AMX_AVX512))
    BX_FATAL(("PANIC: ACE requires AMX_AVX512 to be enabled !"));

  if (is_cpu_extension_supported(BX_ISA_VMX) && is_cpu_extension_supported(BX_ISA_SVM))
    BX_FATAL(("PANIC: VMX and SVM cannot be enabled in same model !"));

  // AVX 10.2 + MOVRS => AVX 10.2 MOVRS
  if (is_cpu_extension_supported(BX_ISA_AVX10_2) && is_cpu_extension_supported(BX_ISA_MOVRS))
    if (! is_cpu_extension_supported(BX_ISA_AVX10_2_MOVRS))
      BX_FATAL(("PANIC: AVX 10.2 MOVRS is expected to be enabled if MOVRS and AVX10.2 are both enabled !"));

  if (is_cpu_extension_supported(BX_ISA_AVX10_2_MOVRS))
    if (! is_cpu_extension_supported(BX_ISA_AVX10_2) || !is_cpu_extension_supported(BX_ISA_MOVRS))
      BX_FATAL(("PANIC: AVX 10.2 MOVRS expected to be enabled only if MOVRS and AVX10.2 are both enabled !"));

  if (is_cpu_extension_supported(BX_ISA_APX)) {
    if (is_cpu_extension_supported(BX_ISA_MPX))
      BX_FATAL(("PANIC: APX cannot be supported together with MPX !"));
    if (! is_cpu_extension_supported(BX_ISA_AVX512))
      BX_FATAL(("PANIC: APX require EVEX and AVX512 support !"));
  }
}

void bx_cpuid_t::dump_features() const
{
  BX_INFO(("CPU Features supported:"));
  for (unsigned i=1; i<BX_ISA_EXTENSION_LAST; i++)
    if (is_cpu_extension_supported(i))
      BX_INFO(("\t\t%s", get_cpu_feature_name(i)));
}
