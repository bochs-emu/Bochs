/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014-2024 Stanislav Shwartsman
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
#define x86_feature(isa, feature_name) #feature_name,
#include "decoder/features.h"
};
#undef x86_feature

const char *get_cpu_feature_name(unsigned feature) { return cpu_feature_name[feature]; }

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
  leaf->eax = (subfunction == 0);
  leaf->ebx = (subfunction == 0);   // number of logical CPUs at this level
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

#if BX_CPU_LEVEL >= 6
extern XSaveRestoreStateHelper xsave_restore[];

static unsigned xsave_max_size_required_by_features(Bit32u features)
{
  unsigned max_size = 512+64, size = 0;

  for (unsigned n=xcr0_t::BX_XCR0_YMM_BIT; n < xcr0_t::BX_XCR0_LAST; n++) {
    if (features & (1 << n))
      size = xsave_restore[n].offset + xsave_restore[n].len;
    if (size > max_size)
      max_size = size;
  }

  return max_size;
}

static unsigned xsave_max_size_required_by_xsaves_features(Bit32u features)
{
  unsigned max_size = 512+64;

  for (unsigned n=xcr0_t::BX_XCR0_YMM_BIT; n < xcr0_t::BX_XCR0_LAST; n++) {
    if (features & (1 << n))
      max_size += xsave_restore[n].len;
  }

  return max_size;
}

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
    leaf->ebx = xsave_max_size_required_by_features(cpu->xcr0.get32());
    leaf->ecx = xsave_max_size_required_by_features(cpu->xcr0_suppmask);
    leaf->edx = 0; // xcr0 is still 32-bit
    return;
  }

  if (subfunction == 1) {
    // EAX[0] - support for the XSAVEOPT instruction
    // EAX[1] - support for compaction extensions to the XSAVE feature set
    // EAX[2] - support for execution of XGETBV with ECX = 1
    // EAX[3] - support for XSAVES, XRSTORS, and the IA32_XSS MSR
    leaf->eax = 0;
    if (is_cpu_extension_supported(BX_ISA_XSAVEOPT))
      leaf->eax |= 0x1;
    if (is_cpu_extension_supported(BX_ISA_XSAVEC))
      leaf->eax |= (1<<1) | (1<<2);
    if (is_cpu_extension_supported(BX_ISA_XSAVES))
      leaf->eax |= (1<<3);

    // EBX[31:00] - The size (in bytes) of the XSAVE area containing all states enabled by (XCRO | IA32_XSS)
    leaf->ebx = 0;
    if (is_cpu_extension_supported(BX_ISA_XSAVES))
      leaf->ebx = xsave_max_size_required_by_xsaves_features(cpu->xcr0.get32() | cpu->msr.ia32_xss);

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
      // ECX[1] - set to indicate this component must be aligned to 64-byte
      // ECX[2] - XFD support for this component
      leaf->ecx = (cpu->ia32_xss_suppmask & (1 << subfunction)) != 0;
      leaf->edx = 0;
    }
  }
}
#endif

#if BX_SUPPORT_AMX
void bx_cpuid_t::get_std_cpuid_amx_palette_info_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

  if (!is_cpu_extension_supported(BX_ISA_AMX))
    return;

  if (subfunction == 0) {
    leaf->eax = 1; // max palette_id
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
}

void bx_cpuid_t::get_std_cpuid_amx_tmul_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

  if (!is_cpu_extension_supported(BX_ISA_AMX))
    return;

  // EBX[07:00] = 16 TMUL_MAX_K (rows or columns)
  // EBX[23:08] = 64 TMUL_MAX_N (column bytes)
  // EBX[31:24] reserved
  leaf->ebx = 16 | (64<<8);
}
#endif

void bx_cpuid_t::get_leaf_0(unsigned max_leaf, const char *vendor_string, cpuid_function_t *leaf, unsigned limited_max_leaf) const
{
  // EAX: highest function understood by CPUID
  // EBX: vendor ID string
  // EDX: vendor ID string
  // ECX: vendor ID string
  if (max_leaf < 0x80000000 && max_leaf > 0x2) {
    // do not limit extended CPUID leafs
    static bool cpuid_limit_winnt = SIM->get_param_bool(BXPN_CPUID_LIMIT_WINNT)->get();
    if (cpuid_limit_winnt)
      max_leaf = (limited_max_leaf < 0x02) ? limited_max_leaf : 0x02;
  }
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

  // [6:6]   SMX: Secure Virtual Machine Technology - not supported
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
  if (is_cpu_extension_supported(BX_ISA_PENTIUM))
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
  if (is_cpu_extension_supported(BX_ISA_PSE36))
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

// Most of the bits in ECX are reserved for Intel
Bit32u bx_cpuid_t::get_ext_cpuid_leaf_1_ecx_intel(Bit32u extra) const
{
  Bit32u ecx = extra;

  // * [0:0]   LAHF/SAHF instructions support in 64-bit mode
  //   [1:1]   CMP_Legacy: Core multi-processing legacy mode (AMD)
  //   [2:2]   SVM: Secure Virtual Machine (AMD)
  //   [3:3]   Extended APIC Space
  //   [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
  // * [5:5]   LZCNT: LZCNT instruction support
  //   [6:6]   SSE4A: SSE4A Instructions support
  //   [7:7]   Misaligned SSE support
  // * [8:8]   PREFETCHW: PREFETCHW instruction support - can be enabled through extra
  //   [9:9]   OSVW: OS visible workarounds (AMD)
  //   [10:10] IBS: Instruction based sampling
  //   [11:11] XOP: Extended Operations Support and XOP Prefix
  //   [12:12] SKINIT support
  //   [13:13] WDT: Watchdog timer support
  //   [14:14] Reserved
  //   [15:15] LWP: Light weight profiling
  //   [16:16] FMA4: Four-operand FMA instructions support
  //   [17:17] Reserved
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
  //   [31:29] Reserved

#if BX_SUPPORT_X86_64
  if (is_cpu_extension_supported(BX_ISA_LM_LAHF_SAHF))
    ecx |= BX_CPUID_EXT1_ECX_LAHF_SAHF;
#endif

  if (is_cpu_extension_supported(BX_ISA_LZCNT))
    ecx |= BX_CPUID_EXT1_ECX_LZCNT;

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
  if (is_cpu_extension_supported(BX_ISA_SSE))
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

  // [30:30] AMD 3DNow! Extensions
  // [31:31] AMD 3DNow! Instructions
  if (is_cpu_extension_supported(BX_ISA_3DNOW))
    edx |= BX_CPUID_EXT1_EDX_3DNOW | BX_CPUID_EXT1_EDX_3DNOW_EXT;
  
  return edx;
}
 
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

  // [14:14]  Intel Memory Protection Extensions - not supported
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

  // [22:21]  AVX512IFMA52 instructions support
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

  // [11:11] AVX512 VNNI instructions support
  // [12:12] AVX512 BITALG instructions support
  // [13:13] reserved
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

  // [17:17] reserved
  // [18:18] reserved
  // [19:19] reserved
  // [20:20] reserved
  // [21:21] reserved

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
  //   [18:13]  reserved

  //   [19:19]  WRMSRNS instruction
  if (is_cpu_extension_supported(BX_ISA_WRMSRNS))
    eax |= BX_CPUID_STD7_SUBLEAF1_EAX_WRMSRNS;

  //   [20:20]  reserved

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

  //   [31:28]  reserved

  return eax;
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
  //   [16:16]  reserved
  //   [17:17]  UIRET sets UIF to the RFLAGS[1] image loaded from the stack
  //   [18:18]  CET_SSS

  return edx;
}

// leaf 0x80000008 - return Intel defaults //
void bx_cpuid_t::get_ext_cpuid_leaf_8(cpuid_function_t *leaf) const
{
  // virtual & phys address size in low 2 bytes of EAX.
  // TODO: physical address width should be 32-bit when no PSE-36 is supported
  Bit32u phy_addr_width = BX_PHY_ADDRESS_WIDTH;
  Bit32u lin_addr_width = is_cpu_extension_supported(BX_ISA_LONG_MODE) ? 
                                (is_cpu_extension_supported(BX_ISA_LA57) ? 57 : 48) : 32;

  leaf->eax = phy_addr_width | (lin_addr_width << 8);

  // [0:0] CLZERO support
  // [1:1] Instruction Retired Counter MSR available
  // [2:2] FP Error Pointers Restored by XRSTOR
  // [3:3] reserved
  // [4:4] RDPRU support
  // [5:5] reserved
  // [6:6] Memory Bandwidth Enforcement (MBE) support
  // [8:7] reserved
  // [9:9] WBNOINVD support - when not supported fall back to legacy WBINVD
  leaf->ebx = (1<<9);
  if (is_cpu_extension_supported(BX_ISA_CLZERO))
    leaf->ebx |= 0x1;

  leaf->ecx = 0; // Reserved, undefined for Intel
  leaf->edx = 0;
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
  for (unsigned std_leaf=0; std_leaf <= max_std_leaf; std_leaf++) {
    dump_cpuid_leaf(std_leaf);
  }

  if (max_ext_leaf == 0) return;

  for (unsigned ext_leaf=0x80000000; ext_leaf <= max_ext_leaf; ext_leaf++) {
    dump_cpuid_leaf(ext_leaf);
  }
}

void bx_cpuid_t::warning_messages(unsigned extension) const
{
  switch(extension) {
  case BX_ISA_3DNOW:
    BX_INFO(("WARNING: 3DNow! is not implemented yet !"));
    break;
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

void bx_cpuid_t::dump_features() const
{
  BX_INFO(("CPU Features supported:"));
  for (unsigned i=1; i<BX_ISA_EXTENSION_LAST; i++)
    if (is_cpu_extension_supported(i))
      BX_INFO(("\t\t%s", get_cpu_feature_name(i)));
}
