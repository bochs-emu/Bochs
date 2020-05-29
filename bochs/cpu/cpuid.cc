/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014-2019 Stanislav Shwartsman
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

static const char *cpu_feature_name[] =
{
  "386ni",                  // BX_ISA_386
  "x87",                    // BX_ISA_X87
  "486ni",                  // BX_ISA_486
  "pentium_ni",             // BX_ISA_PENTIUM
  "p6ni",                   // BX_ISA_P6
  "mmx",                    // BX_ISA_MMX
  "3dnow!",                 // BX_ISA_3DNOW
  "debugext",               // BX_ISA_DEBUG_EXTENSIONS
  "vme",                    // BX_ISA_VME
  "pse",                    // BX_ISA_PSE
  "pae",                    // BX_ISA_PAE
  "pge",                    // BX_ISA_PGE
  "pse36",                  // BX_ISA_PSE36
  "mtrr",                   // BX_ISA_MTRR
  "pat",                    // BX_ISA_PAT
  "legacy_syscall_sysret",  // BX_ISA_SYSCALL_SYSRET_LEGACY
  "sysenter_sysexit",       // BX_ISA_SYSENTER_SYSEXIT
  "clflush",                // BX_ISA_CLFLUSH
  "clflushopt",             // BX_ISA_CLFLUSHOPT
  "clwb",                   // BX_ISA_CLWB
  "cldemote",               // BX_ISA_CLDEMOTE
  "sse",                    // BX_ISA_SSE
  "sse2",                   // BX_ISA_SSE2
  "sse3",                   // BX_ISA_SSE3
  "ssse3",                  // BX_ISA_SSSE3
  "sse4_1",                 // BX_ISA_SSE4_1
  "sse4_2",                 // BX_ISA_SSE4_2
  "popcnt",                 // BX_ISA_POPCNT
  "mwait",                  // BX_ISA_MONITOR_MWAIT
  "mwaitx",                 // BX_ISA_MONITORX_MWAITX
  "waitpkg",                // BX_ISA_WAITPKG
  "vmx",                    // BX_ISA_VMX
  "smx",                    // BX_ISA_SMX
  "longmode",               // BX_ISA_LONG_MODE
  "lm_lahf_sahf",           // BX_ISA_LM_LAHF_SAHF
  "nx",                     // BX_ISA_NX
  "1g_pages",               // BX_ISA_1G_PAGES
  "cmpxhg16b",              // BX_ISA_CMPXCHG16B
  "rdtscp",                 // BX_ISA_RDTSCP
  "ffxsr",                  // BX_ISA_FFXSR
  "xsave",                  // BX_ISA_XSAVE
  "xsaveopt",               // BX_ISA_XSAVEOPT
  "xsavec",                 // BX_ISA_XSAVEC
  "xsaves",                 // BX_ISA_XSAVES
  "aes_pclmulqdq",          // BX_ISA_AES_PCLMULQDQ
  "vaes_vpclmulqdq",        // BX_ISA_VAES_VPCLMULQDQ
  "movbe",                  // BX_ISA_MOVBE
  "fsgsbase",               // BX_ISA_FSGSBASE
  "invpcid",                // BX_ISA_INVPCID
  "avx",                    // BX_ISA_AVX
  "avx2",                   // BX_ISA_AVX2
  "avx_f16c",               // BX_ISA_AVX_F16C
  "avx_fma",                // BX_ISA_AVX_FMA
  "altmovcr8",              // BX_ISA_ALT_MOV_CR8
  "sse4a",                  // BX_ISA_SSE4A
  "misaligned_sse",         // BX_ISA_MISALIGNED_SSE
  "lzcnt",                  // BX_ISA_LZCNT
  "bmi1",                   // BX_ISA_BMI1
  "bmi2",                   // BX_ISA_BMI2
  "fma4",                   // BX_ISA_FMA4
  "xop",                    // BX_ISA_XOP
  "tbm",                    // BX_ISA_TBM
  "svm",                    // BX_ISA_SVM
  "rdrand",                 // BX_ISA_RDRAND
  "adx",                    // BX_ISA_ADX
  "smap",                   // BX_ISA_SMAP
  "rdseed",                 // BX_ISA_RDSEED
  "sha",                    // BX_ISA_SHA
  "gfni",                   // BX_ISA_GFNI
  "avx512",                 // BX_ISA_AVX512
  "avx512cd",               // BX_ISA_AVX512_CD
  "avx512pf",               // BX_ISA_AVX512_PF
  "avx512er",               // BX_ISA_AVX512_ER
  "avx512dq",               // BX_ISA_AVX512_DQ
  "avx512bw",               // BX_ISA_AVX512_BW
  "avx512vl",               // BX_ISA_AVX512_VL
  "avx512vbmi",             // BX_ISA_AVX512_VBMI
  "avx512vbmi2",            // BX_ISA_AVX512_VBMI2
  "avx512ifma52",           // BX_ISA_AVX512_IFMA52
  "avx512ivpopcnt",         // BX_ISA_AVX512_VPOPCNTDQ
  "avx512ivnni",            // BX_ISA_AVX512_VNNI
  "avx512ibitalg",          // BX_ISA_AVX512_BITALG
  "avx512vp2intersect",     // BX_ISA_AVX512_VP2INTERSECT
  "xapic",                  // BX_ISA_XAPIC
  "x2apic",                 // BX_ISA_X2APIC
  "xapicext",               // BX_ISA_XAPICEXT
  "pcid",                   // BX_ISA_PCID
  "smep",                   // BX_ISA_SMEP
  "tsc_adjust",             // BX_ISA_TSC_ADJUST
  "tsc_deadline",           // BX_ISA_TSC_DEADLINE
  "fopcode_deprecation",    // BX_ISA_FOPCODE_DEPRECATION
  "fcs_fds_deprecation",    // BX_ISA_FCS_FDS_DEPRECATION
  "fdp_deprecation",        // BX_ISA_FDP_DEPRECATION
  "pku",                    // BX_ISA_PKU
  "pks",                    // BX_ISA_PKS
  "umip",                   // BX_ISA_UMIP
  "rdpid",                  // BX_ISA_RDPID
  "tce",                    // BX_ISA_TCE
  "clzero",                 // BX_ISA_CLZERO
  "sca_mitigations",        // BX_ISA_SCA_MITIGATIONS
  "cet",                    // BX_ISA_CET
};

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

#if BX_CPU_LEVEL >= 6
void bx_cpuid_t::get_std_cpuid_xsave_leaf(Bit32u subfunction, cpuid_function_t *leaf) const
{
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

  if (is_cpu_extension_supported(BX_ISA_XSAVE))
  {
    switch(subfunction) {
    case 0:
      // EAX - valid bits of XCR0 (lower part)
      // EBX - Maximum size (in bytes) required by enabled features
      // ECX - Maximum size (in bytes) required by CPU supported features
      // EDX - valid bits of XCR0 (upper part)
      leaf->eax = cpu->xcr0_suppmask;

      leaf->ebx = 512+64;
#if BX_SUPPORT_AVX
      if (cpu->xcr0.get_YMM())
        leaf->ebx = XSAVE_YMM_STATE_OFFSET + XSAVE_YMM_STATE_LEN;
#endif
#if BX_SUPPORT_EVEX
      if (cpu->xcr0.get_OPMASK())
        leaf->ebx = XSAVE_OPMASK_STATE_OFFSET + XSAVE_OPMASK_STATE_LEN;
      if (cpu->xcr0.get_ZMM_HI256())
        leaf->ebx = XSAVE_ZMM_HI256_STATE_OFFSET + XSAVE_ZMM_HI256_STATE_LEN;
      if (cpu->xcr0.get_HI_ZMM())
        leaf->ebx = XSAVE_HI_ZMM_STATE_OFFSET + XSAVE_HI_ZMM_STATE_LEN;
#endif
#if BX_SUPPORT_PKEYS
      if (cpu->xcr0.get_PKRU())
        leaf->ebx = XSAVE_PKRU_STATE_OFFSET + XSAVE_PKRU_STATE_LEN;
#endif

      leaf->ecx = 512+64;
#if BX_SUPPORT_AVX
      if (cpu->xcr0_suppmask & BX_XCR0_YMM_MASK)
        leaf->ecx = XSAVE_YMM_STATE_OFFSET + XSAVE_YMM_STATE_LEN;
#endif
#if BX_SUPPORT_EVEX
      if (cpu->xcr0_suppmask & BX_XCR0_OPMASK_MASK)
        leaf->ecx = XSAVE_OPMASK_STATE_OFFSET + XSAVE_OPMASK_STATE_LEN;
      if (cpu->xcr0_suppmask & BX_XCR0_ZMM_HI256_MASK)
        leaf->ecx = XSAVE_ZMM_HI256_STATE_OFFSET + XSAVE_ZMM_HI256_STATE_LEN;
      if (cpu->xcr0_suppmask & BX_XCR0_HI_ZMM_MASK)
        leaf->ecx = XSAVE_HI_ZMM_STATE_OFFSET + XSAVE_HI_ZMM_STATE_LEN;
#endif
#if BX_SUPPORT_PKEYS
      if (cpu->xcr0_suppmask & BX_XCR0_PKRU_MASK)
        leaf->ecx = XSAVE_PKRU_STATE_OFFSET + XSAVE_PKRU_STATE_LEN;
#endif
      leaf->edx = 0;
      break;

    case 1:
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
      if (is_cpu_extension_supported(BX_ISA_XSAVES)) {
        xcr0_t xcr0_xss = cpu->xcr0;
        xcr0_xss.val32 |= cpu->msr.ia32_xss;
#if BX_SUPPORT_AVX
        if (xcr0_xss.get_YMM())
          leaf->ebx = XSAVE_YMM_STATE_OFFSET + XSAVE_YMM_STATE_LEN;
#endif
#if BX_SUPPORT_EVEX
        if (xcr0_xss.get_OPMASK())
          leaf->ebx = XSAVE_OPMASK_STATE_OFFSET + XSAVE_OPMASK_STATE_LEN;
        if (xcr0_xss.get_ZMM_HI256())
          leaf->ebx = XSAVE_ZMM_HI256_STATE_OFFSET + XSAVE_ZMM_HI256_STATE_LEN;
        if (xcr0_xss.get_HI_ZMM())
          leaf->ebx = XSAVE_HI_ZMM_STATE_OFFSET + XSAVE_HI_ZMM_STATE_LEN;
#endif
#if BX_SUPPORT_PKEYS
        if (xcr0_xss.get_PKRU())
          leaf->ebx = XSAVE_PKRU_STATE_OFFSET + XSAVE_PKRU_STATE_LEN;
#endif
      }

      // ECX[31:0] - Reports the supported bits of the lower 32 bits of the IA32_XSS MSR.
      //             IA32_XSS[n]    can be set to 1 only if ECX[n] is 1
      // EDX[31:0] - Reports the supported bits of the upper 32 bits of the IA32_XSS MSR.
      //             IA32_XSS[n+32] can be set to 1 only if EDX[n] is 1
      leaf->ecx = 0;
#if BX_SUPPPORT_CET
      leaf->ecx |= BX_XCR0_CET_U_MASK | BX_XCR0_CET_S_MASK;
#endif
      leaf->edx = 0;
      break;

#if BX_SUPPORT_AVX
    case 2: // YMM leaf
      if (cpu->xcr0_suppmask & BX_XCR0_YMM_MASK) {
        leaf->eax = XSAVE_YMM_STATE_LEN;
        leaf->ebx = XSAVE_YMM_STATE_OFFSET;
        leaf->ecx = 0;
        leaf->edx = 0;
      }
      break;
#endif

    case 3: // MPX leafs (BNDREGS, BNDCFG)
    case 4:
      break;

#if BX_SUPPORT_EVEX
    case 5: // OPMASK leaf
      if (cpu->xcr0_suppmask & BX_XCR0_OPMASK_MASK) {
        leaf->eax = XSAVE_OPMASK_STATE_LEN;
        leaf->ebx = XSAVE_OPMASK_STATE_OFFSET;
        leaf->ecx = 0;
        leaf->edx = 0;
      }
      break;
    case 6: // ZMM Hi256 leaf
      if (cpu->xcr0_suppmask & BX_XCR0_ZMM_HI256_MASK) {
        leaf->eax = XSAVE_ZMM_HI256_STATE_LEN;
        leaf->ebx = XSAVE_ZMM_HI256_STATE_OFFSET;
        leaf->ecx = 0;
        leaf->edx = 0;
      }
      break;
    case 7: // HI_ZMM leaf
      if (cpu->xcr0_suppmask & BX_XCR0_HI_ZMM_MASK) {
        leaf->eax = XSAVE_HI_ZMM_STATE_LEN;
        leaf->ebx = XSAVE_HI_ZMM_STATE_OFFSET;
        leaf->ecx = 0;
        leaf->edx = 0;
      }
      break;
#endif

    case 8: // Processor trace leaf
      break;

#if BX_SUPPPORT_PKEYS
    case 9: // Protection keys
      if (cpu->xcr0_suppmask & BX_XCR0_PKRU_MASK) {
        leaf->eax = XSAVE_PKRU_STATE_LEN;
        leaf->ebx = XSAVE_PKRU_STATE_OFFSET;
        leaf->ecx = 0;
        leaf->edx = 0;
      }
      break;
#endif

#if BX_SUPPPORT_CET
    case 10:
      if (cpu->xcr0_suppmask & BX_XCR0_CET_U_MASK) {
        leaf->eax = XSAVE_CET_U_STATE_LEN;
        leaf->ebx = 0;  // doesn't map to a valid bit in XCR0 register
        leaf->ecx = 1;  // managed through IA32_XSS register
        leaf->edx = 0;
      }
      break;
    case 11:
      if (cpu->xcr0_suppmask & BX_XCR0_CET_S_MASK) {
        leaf->eax = XSAVE_CET_S_STATE_LEN;
        leaf->ebx = 0;  // doesn't map to a valid bit in XCR0 register
        leaf->ecx = 1;  // managed through IA32_XSS register
        leaf->edx = 0;
      }
      break;
#endif

    }
  }
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

// leaf 0x80000008 - return Intel defaults //
void bx_cpuid_t::get_ext_cpuid_leaf_8(cpuid_function_t *leaf) const
{
  // virtual & phys address size in low 2 bytes of EAX.
  // TODO: physical address width should be 32-bit when no PSE-36 is supported
  Bit32u phy_addr_width = BX_PHY_ADDRESS_WIDTH;
  Bit32u lin_addr_width = is_cpu_extension_supported(BX_ISA_LONG_MODE) ? BX_LIN_ADDRESS_WIDTH : 32;

  leaf->eax = phy_addr_width | (lin_addr_width << 8);

  // [0:0] CLZERO support
  // [1:1] Instruction Retired Counter MSR available
  // [2:2] FP Error Pointers Restored by XRSTOR
  // [3:3] reserved
  // [4:4] RDPRU support
  // [5:5] reserved
  // [6:6] Memory Bandwidth Enforcement (MBE) support
  // [8:7] reserved
  // [9:9] WBNOINVD support
  leaf->ebx = 0;
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
  for (unsigned std_leaf=0; std_leaf<=max_std_leaf; std_leaf++) {
    dump_cpuid_leaf(std_leaf);
  }

  if (max_ext_leaf == 0) return;

  for (unsigned ext_leaf=0x80000000; ext_leaf<=(0x80000000 + max_ext_leaf); ext_leaf++) {
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
      BX_INFO(("\t\t%s", cpu_feature_name[i]));
}
