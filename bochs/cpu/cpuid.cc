/////////////////////////////////////////////////////////////////////////
// $Id: cpuid.cc,v 1.127 2011-01-12 20:12:05 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2010 Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "model_specific.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "param_names.h"

#if BX_SUPPORT_X86_64==0
// Make life easier for merging code.
#define RAX EAX
#define RBX EBX
#define RCX ECX
#define RDX EDX
#endif

/*
 * Get CPU version information:
 *
 * [3:0]   Stepping ID
 * [7:4]   Model: starts at 1
 * [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
 * [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
 * [31:14] Reserved
 */

Bit32u BX_CPU_C::get_cpu_version_information(void)
{
  Bit32u stepping = SIM->get_param_num(BXPN_CPUID_STEPPING)->get();

  Bit32u family = 0, model = 0;
  Bit32u extended_model = 0;
  Bit32u extended_family = 0;

#if BX_CPU_LEVEL > 3

  /* ****** */
  /*  i486  */
  /* ****** */

#if BX_CPU_LEVEL == 4
  family = 4;

#if BX_SUPPORT_FPU
  model = 1;            // 486dx
#else
  model = 2;            // 486sx
#endif

  /* **************** */
  /*  i586 (Pentium)  */
  /* **************** */

#elif BX_CPU_LEVEL == 5	
  family   = 5;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_MMX))
    model = 4; // Pentium MMX
  else
    model = 1; // Pentium 60/66

  /* ****** */
  /*  i686  */
  /* ****** */

#elif BX_CPU_LEVEL == 6

  unsigned sse_enabled = SIM->get_param_enum(BXPN_CPUID_SSE)->get();

  if (sse_enabled >= 2) {

/*
     The model, family, and processor type for the first
     processor in the Intel Pentium 4 family is as follows:
		* Model-0000B
		* Family-1111B
		* Processor Type-00B (OEM)
                * Stepping-0B
*/
    model    = 0;
    family   = 0xf;

#if BX_SUPPORT_X86_64
    model    = 2;       // Hammer returns what?
#endif

  }
  else {                // Pentium Pro/Pentium II/Pentium III processor
    family   = 6;
    model    = 8;
  }

#else
  BX_PANIC(("CPUID family ID not implemented for CPU LEVEL > 6"));
#endif

#endif  // BX_CPU_LEVEL > 3

  return (extended_family << 20) |
         (extended_model << 16) |
         (family << 8) |
         (model<<4) | stepping;
}

/* Get CPU extended feature flags. */
Bit32u BX_CPU_C::get_extended_cpuid_features(void)
{
  // [0:0]   SSE3: SSE3 Instructions
  // [1:1]   PCLMULQDQ Instruction support
  // [2:2]   DTES64: 64-bit DS area
  // [3:3]   MONITOR/MWAIT support
  // [4:4]   DS-CPL: CPL qualified debug store
  // [5:5]   VMX: Virtual Machine Technology
  // [6:6]   SMX: Secure Virtual Machine Technology
  // [7:7]   EST: Enhanced Intel SpeedStep Technology
  // [8:8]   TM2: Thermal Monitor 2
  // [9:9]   SSSE3: SSSE3 Instructions
  // [10:10] CNXT-ID: L1 context ID
  // [11:11] reserved
  // [12:12] FMA Instructions support
  // [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  // [14:14] xTPR update control
  // [15:15] PDCM - Perfon and Debug Capability MSR
  // [16:16] reserved
  // [17:17] PCID: Process Context Identifiers
  // [18:18] DCA - Direct Cache Access
  // [19:19] SSE4.1 Instructions
  // [20:20] SSE4.2 Instructions
  // [21:21] X2APIC
  // [22:22] MOVBE instruction
  // [23:23] POPCNT instruction
  // [24:24] TSC Deadline
  // [25:25] AES Instructions
  // [26:26] XSAVE extensions support
  // [27:27] OSXSAVE support
  // [28:28] AVX extensions support
  // [29:29] F16C - Float16 conversion support
  // [30:30] RDRAND instruction
  // [31:31] reserved

  Bit32u features = 0;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SSE3))
    features |= BX_CPUID_EXT_SSE3;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_AES_PCLMULQDQ))
    features |= BX_CPUID_EXT_PCLMULQDQ;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_MONITOR_MWAIT))
    features |= BX_CPUID_EXT_MONITOR_MWAIT;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_VMX))
    features |= BX_CPUID_EXT_VMX;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SSSE3))
    features |= BX_CPUID_EXT_SSSE3;

#if BX_SUPPORT_X86_64
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_X86_64))
    features |= BX_CPUID_EXT_CMPXCHG16B;

  if (SIM->get_param_bool(BXPN_CPUID_PCID)->get())
    features |= BX_CPUID_EXT_PCID;
#endif

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SSE4_1))
    features |= BX_CPUID_EXT_SSE4_1;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SSE4_2))
    features |= BX_CPUID_EXT_SSE4_2;

#if BX_SUPPORT_X2APIC
  features |= BX_CPUID_EXT_X2APIC;
#endif

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_MOVBE))
    features |= BX_CPUID_EXT_MOVBE;

  // enable POPCNT if SSE4_2 is enabled
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SSE4_2))
    features |= BX_CPUID_EXT_POPCNT;

  // support for AES
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_AES_PCLMULQDQ))
    features |= BX_CPUID_EXT_AES;

  // support XSAVE extensions
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_XSAVE))
    features |= BX_CPUID_EXT_XSAVE | BX_CPUID_EXT_OSXSAVE;

  return features;
}

Bit32u BX_CPU_C::get_ext2_cpuid_features(void)
{
  Bit32u features = 0;

  //   [0:0]  FS/GS BASE access instructions
  //  [31:1]  Reserved
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_FSGSBASE))
    features |= 1;

  return features;
}

/* Get CPU feature flags. Returned by CPUID functions 1 and 80000001.  */
Bit32u BX_CPU_C::get_std_cpuid_features(void)
{
  //   [0:0]   FPU on chip
  //   [1:1]   VME: Virtual-8086 Mode enhancements
  //   [2:2]   DE: Debug Extensions (I/O breakpoints)
  //   [3:3]   PSE: Page Size Extensions
  //   [4:4]   TSC: Time Stamp Counter
  //   [5:5]   MSR: RDMSR and WRMSR support
  //   [6:6]   PAE: Physical Address Extensions
  //   [7:7]   MCE: Machine Check Exception
  //   [8:8]   CXS: CMPXCHG8B instruction
  //   [9:9]   APIC: APIC on Chip
  //   [10:10] Reserved
  //   [11:11] SYSENTER/SYSEXIT support
  //   [12:12] MTRR: Memory Type Range Reg
  //   [13:13] PGE/PTE Global Bit
  //   [14:14] MCA: Machine Check Architecture
  //   [15:15] CMOV: Cond Mov/Cmp Instructions
  //   [16:16] PAT: Page Attribute Table
  //   [17:17] PSE-36: Physical Address Extensions
  //   [18:18] PSN: Processor Serial Number
  //   [19:19] CLFLUSH: CLFLUSH Instruction support
  //   [20:20] Reserved
  //   [21:21] DS: Debug Store
  //   [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
  //   [23:23] MMX Technology
  //   [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  //   [25:25] SSE: SSE Extensions
  //   [26:26] SSE2: SSE2 Extensions
  //   [27:27] Self Snoop
  //   [28:28] Hyper Threading Technology
  //   [29:29] TM: Thermal Monitor
  //   [30:30] Reserved
  //   [31:31] PBE: Pending Break Enable

  Bit32u features = 0;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_X87))
    features |= BX_CPUID_STD_X87;

#if BX_CPU_LEVEL >= 5
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_PENTIUM)) {
    // Pentium only features
    features |= BX_CPUID_STD_VME;
    features |= BX_CPUID_STD_DEBUG_EXTENSIONS;
    features |= BX_CPUID_STD_PSE;
    features |= BX_CPUID_STD_TSC;
    features |= BX_CPUID_STD_MSR;
    // support Machine Check
    features |= BX_CPUID_STD_MCE | BX_CPUID_STD_MCA;
    features |= BX_CPUID_STD_CMPXCHG8B;
  }
#endif

#if BX_SUPPORT_APIC
  // if MSR_APICBASE APIC Global Enable bit has been cleared,
  // the CPUID feature flag for the APIC is set to 0.
  if (BX_CPU_THIS_PTR msr.apicbase & 0x800)
    features |= BX_CPUID_STD_APIC; // APIC on chip
#endif

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SYSENTER_SYSEXIT))
    features |= BX_CPUID_STD_SYSENTER_SYSEXIT;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_CLFLUSH))
    features |= BX_CPUID_STD_CLFLUSH;

#if BX_CPU_LEVEL >= 5
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_MMX))
    features |= BX_CPUID_STD_MMX;
#endif

#if BX_CPU_LEVEL >= 6
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_P6)) {
    features |= BX_CPUID_STD_PAE;
    features |= BX_CPUID_STD_MTRR;
    features |= BX_CPUID_STD_GLOBAL_PAGES;
    features |= BX_CPUID_STD_CMOV;
    features |= BX_CPUID_STD_PAT;
    features |= BX_CPUID_STD_PSE36;
    features |= BX_CPUID_STD_ACPI;
  }

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_FXSAVE_FXRSTOR))
    features |= BX_CPUID_STD_FXSAVE_FXRSTOR;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SSE))
    features |= BX_CPUID_STD_SSE;

  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SSE2))
    features |= BX_CPUID_STD_SSE2;
#endif

#if BX_SUPPORT_SMP
  // Intel(R) HyperThreading Technology
  if (SIM->get_param_num(BXPN_CPU_NTHREADS)->get() > 1)
    features |= BX_CPUID_STD_HT;
#endif

  return features;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CPUID(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit32u function    = EAX;
  Bit32u subfunction = ECX;

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    BX_ERROR(("VMEXIT: CPUID in VMX non-root operation"));
    VMexit(i, VMX_VMEXIT_CPUID, 0);
  }
#endif

  unsigned max_std_function = BX_CPU_THIS_PTR cpuid_std_function[0].eax;
  unsigned max_ext_function = BX_CPU_THIS_PTR cpuid_ext_function[0].eax;

  if(function < 0x80000000) {
    if(function <= max_std_function) {
#if BX_SUPPORT_X2APIC
      if (function == 0xb) {
        bx_cpuid_extended_topology_leaf(subfunction);
        return;
      }
#endif
#if BX_CPU_LEVEL >= 6
      if (function == 0x7) {
        bx_cpuid_extended_cpuid_leaf(subfunction);
        return;
      }
      if (function == 0xd) {
        bx_cpuid_xsave_leaf(subfunction);
        return;
      }
#endif
      RAX = BX_CPU_THIS_PTR cpuid_std_function[function].eax;
      RBX = BX_CPU_THIS_PTR cpuid_std_function[function].ebx;
      RCX = BX_CPU_THIS_PTR cpuid_std_function[function].ecx;
      RDX = BX_CPU_THIS_PTR cpuid_std_function[function].edx;
      return;
    }
  }
  else {
    if(function <= max_ext_function) {
      function -= 0x80000000;
      RAX = BX_CPU_THIS_PTR cpuid_ext_function[function].eax;
      RBX = BX_CPU_THIS_PTR cpuid_ext_function[function].ebx;
      RCX = BX_CPU_THIS_PTR cpuid_ext_function[function].ecx;
      RDX = BX_CPU_THIS_PTR cpuid_ext_function[function].edx;
      return;
    }
  }

  // unknown CPUID function - return maximum standard leaf
  RAX = BX_CPU_THIS_PTR cpuid_std_function[max_std_function].eax;
  RBX = BX_CPU_THIS_PTR cpuid_std_function[max_std_function].ebx;
  RCX = BX_CPU_THIS_PTR cpuid_std_function[max_std_function].ecx;
  RDX = BX_CPU_THIS_PTR cpuid_std_function[max_std_function].edx;
#endif
}

void BX_CPU_C::set_cpuid_defaults(void)
{
  Bit8u *vendor_string = (Bit8u *)SIM->get_param_string(BXPN_VENDOR_STRING)->getptr();
  Bit8u *brand_string = (Bit8u *)SIM->get_param_string(BXPN_BRAND_STRING)->getptr();
  bx_bool cpuid_limit_winnt = SIM->get_param_bool(BXPN_CPUID_LIMIT_WINNT)->get();
#if BX_SUPPORT_X86_64
  bx_bool xlarge_pages_enabled = SIM->get_param_bool(BXPN_CPUID_1G_PAGES)->get();
#endif

  cpuid_function_t *cpuid;
  int i;

  for (i=0;i<MAX_STD_CPUID_FUNCTION;i++) {
    BX_CPU_THIS_PTR cpuid_std_function[i].eax = 0;
    BX_CPU_THIS_PTR cpuid_std_function[i].ebx = 0;
    BX_CPU_THIS_PTR cpuid_std_function[i].ecx = 0;
    BX_CPU_THIS_PTR cpuid_std_function[i].edx = 0;
  }

  for (i=0;i<MAX_EXT_CPUID_FUNCTION;i++) {
    BX_CPU_THIS_PTR cpuid_ext_function[i].eax = 0;
    BX_CPU_THIS_PTR cpuid_ext_function[i].ebx = 0;
    BX_CPU_THIS_PTR cpuid_ext_function[i].ecx = 0;
    BX_CPU_THIS_PTR cpuid_ext_function[i].edx = 0;
  }

  // ------------------------------------------------------
  // CPUID function 0x00000000
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[0]);

  // EAX: highest input value understood by CPUID
  // EBX: vendor ID string
  // EDX: vendor ID string
  // ECX: vendor ID string

#if BX_CPU_LEVEL <= 5
  // 486 and Pentium processors
  cpuid->eax = 1;
#else
  // for Pentium Pro, Pentium II, Pentium 4 processors
  cpuid->eax = 3;

  // do not report CPUID functions above 0x3 if cpuid_limit_winnt is set
  // to workaround WinNT issue.
  if (! cpuid_limit_winnt) {
    if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_MONITOR_MWAIT))
      cpuid->eax = 0x5;
#if BX_SUPPORT_X2APIC
    cpuid->eax = 0xb;
#endif
    if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_XSAVE))
      cpuid->eax = 0xd;
  }
#endif

  // CPUID vendor string (e.g. GenuineIntel, AuthenticAMD, CentaurHauls, ...)
  memcpy(&(cpuid->ebx), vendor_string,     4);
  memcpy(&(cpuid->edx), vendor_string + 4, 4);
  memcpy(&(cpuid->ecx), vendor_string + 8, 4);
#ifdef BX_BIG_ENDIAN
  cpuid->ebx = bx_bswap32(cpuid->ebx);
  cpuid->ecx = bx_bswap32(cpuid->ecx);
  cpuid->edx = bx_bswap32(cpuid->edx);
#endif

  BX_INFO(("CPUID[0x00000000]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // ------------------------------------------------------
  // CPUID function 0x00000001
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[1]);

  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [31:14] Reserved
  cpuid->eax = get_cpu_version_information();

  // EBX:
  //   [7:0]   Brand ID
  //   [15:8]  CLFLUSH cache line size (value*8 = cache line size in bytes)
  //   [23:16] Number of logical processors in one physical processor
  //   [31:24] Local Apic ID

  cpuid->ebx = 0;
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_CLFLUSH)) {
    cpuid->ebx |= (CACHE_LINE_SIZE / 8) << 8;
  }
  unsigned n_logical_processors = SIM->get_param_num(BXPN_CPU_NCORES)->get()*SIM->get_param_num(BXPN_CPU_NTHREADS)->get();
  if (n_logical_processors > 1)
    cpuid->ebx |= (n_logical_processors << 16);
#if BX_SUPPORT_APIC
  cpuid->ebx |= ((BX_CPU_THIS_PTR lapic.get_id() & 0xff) << 24);
#endif

  // ECX: Extended Feature Flags
  cpuid->ecx = get_extended_cpuid_features();

  // EDX: Standard Feature Flags
  cpuid->edx = get_std_cpuid_features();

  BX_INFO(("CPUID[0x00000001]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

#if BX_CPU_LEVEL >= 6
  // ------------------------------------------------------
  // CPUID function 0x00000002 - Cache and TLB Descriptors
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[2]);

#if BX_CPU_VENDOR_INTEL
  cpuid->eax = 0x00410601;  // for Pentium Pro compatibility
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;
#else
  cpuid->eax = 0;           // ignore for AMD
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;
#endif

  BX_INFO(("CPUID[0x00000002]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // ------------------------------------------------------
  // CPUID function 0x00000003 - Processor Serial Number
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[3]);

  cpuid->eax = 0;
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;

  BX_INFO(("CPUID[0x00000003]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // do not report CPUID functions above 0x3 if cpuid_limit_winnt is set
  // to workaround WinNT issue.
  if (cpuid_limit_winnt) return;

  // ------------------------------------------------------
  // CPUID function 0x00000004 - Deterministic Cache Parameters
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[4]);

  cpuid->eax = 0;
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;

  BX_INFO(("CPUID[0x00000004]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

#if BX_SUPPORT_MONITOR_MWAIT
  // ------------------------------------------------------
  // CPUID function 0x00000005
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_MONITOR_MWAIT))
  {
    cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[5]);

    // EAX - Smallest monitor-line size in bytes
    // EBX - Largest  monitor-line size in bytes
    // ECX -
    //   [31:2] - reserved
    //    [1:1] - exit MWAIT even with EFLAGS.IF = 0
    //    [0:0] - MONITOR/MWAIT extensions are supported
    // EDX - Reserved
    cpuid->eax = CACHE_LINE_SIZE;
    cpuid->ebx = CACHE_LINE_SIZE;
    cpuid->ecx = 3;
    cpuid->edx = 0;

    BX_INFO(("CPUID[0x00000005]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));
  }
#endif

  // ------------------------------------------------------
  // CPUID function 0x00000007
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[7]);

  cpuid->ebx = get_ext2_cpuid_features();
  cpuid->ecx = 0;
  cpuid->edx = 0;
  if (cpuid->ebx)
    cpuid->eax = 1; /* report max sub-leaves that are supported in leaf 7 */
  else
    cpuid->eax = 0; /* leaf 7 not supported */
  
  BX_INFO(("CPUID[0x00000007]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // ------------------------------------------------------
  // CPUID function 0x0000000d
  if (BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_XSAVE))
  {
    cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[0xd]);

    // EAX - valid bits of XCR0 (lower part)
    // EBX - Maximum size (in bytes) required by enabled features
    // ECX - Maximum size (in bytes) required by CPU supported features
    // EDX - valid bits of XCR0 (upper part)
    cpuid->eax = BX_XCR0_SUPPORTED_BITS;
    cpuid->ebx = 512+64;
    cpuid->ecx = 512+64;
    cpuid->edx = 0;

    BX_INFO(("CPUID[0x0000000D]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));
  }

  // do not report Pentium 4 extended functions if not needed
  if (! BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_SSE2))
    return;

  // ------------------------------------------------------
  // CPUID function 0x80000000
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[0]);

  // EAX: highest input value understood by CPUID
  // EBX: vendor ID string
  // EDX: vendor ID string
  // ECX: vendor ID string
  cpuid->eax = BX_SUPPORT_X86_64 ? 0x80000008 : 0x80000004;
#if BX_CPU_VENDOR_INTEL
  cpuid->ebx = 0;
  cpuid->edx = 0;          // Reserved for Intel
  cpuid->ecx = 0;
#else
  memcpy(&(cpuid->ebx), vendor_string,     4);
  memcpy(&(cpuid->edx), vendor_string + 4, 4);
  memcpy(&(cpuid->ecx), vendor_string + 8, 4);
#endif

#ifdef BX_BIG_ENDIAN
  cpuid->ebx = bx_bswap32(cpuid->ebx);
  cpuid->ecx = bx_bswap32(cpuid->ecx);
  cpuid->edx = bx_bswap32(cpuid->edx);
#endif

  BX_INFO(("CPUID[0x80000000]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // ------------------------------------------------------
  // CPUID function 0x80000001
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[1]);

  // EAX:       CPU Version Information
  cpuid->eax = BX_CPU_VENDOR_INTEL ? 0 : get_cpu_version_information();

  // EBX:       Brand ID
  cpuid->ebx = 0;

  // ECX:
  //     [0:0]   LAHF/SAHF instructions support in 64-bit mode
  //     [1:1]   CMP_Legacy: Core multi-processing legacy mode (AMD)
  //     [2:2]   SVM: Secure Virtual Machine (AMD)
  //     [3:3]   Extended APIC Space
  //     [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
  //     [5:5]   LZCNT: LZCNT instruction support
  //     [6:6]   SSE4A: SSE4A Instructions support
  //     [7:7]   Misaligned SSE support
  //     [8:8]   PREFETCHW: PREFETCHW instruction support
  //     [9:9]   OSVW: OS visible workarounds (AMD)
  //     [11:10] reserved
  //     [12:12] SKINIT support
  //     [13:13] WDT: Watchdog timer support
  //     [31:14] reserved
#if BX_SUPPORT_X86_64
  cpuid->ecx = BX_CPUID_EXT2_LAHF_SAHF;
#endif
#if BX_SUPPORT_MISALIGNED_SSE
  cpuid->ecx |= BX_CPUID_EXT2_MISALIGNED_SSE;
#endif

  // EDX:
  // Many of the bits in EDX are the same as EAX [*] for AMD
  // [*] [0:0]   FPU on chip
  // [*] [1:1]   VME: Virtual-8086 Mode enhancements
  // [*] [2:2]   DE: Debug Extensions (I/O breakpoints)
  // [*] [3:3]   PSE: Page Size Extensions
  // [*] [4:4]   TSC: Time Stamp Counter
  // [*] [5:5]   MSR: RDMSR and WRMSR support
  // [*] [6:6]   PAE: Physical Address Extensions
  // [*] [7:7]   MCE: Machine Check Exception
  // [*] [8:8]   CXS: CMPXCHG8B instruction
  // [*] [9:9]   APIC: APIC on Chip
  //     [10:10] Reserved
  //     [11:11] SYSCALL/SYSRET support
  // [*] [12:12] MTRR: Memory Type Range Reg
  // [*] [13:13] PGE/PTE Global Bit
  // [*] [14:14] MCA: Machine Check Architecture
  // [*] [15:15] CMOV: Cond Mov/Cmp Instructions
  // [*] [16:16] PAT: Page Attribute Table
  // [*] [17:17] PSE-36: Physical Address Extensions
  //     [18:19] Reserved
  //     [20:20] No-Execute page protection
  //     [21:21] Reserved
  //     [22:22] AMD MMX Extensions
  // [*] [23:23] MMX Technology
  // [*] [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  //     [25:25] Fast FXSAVE/FXRSTOR mode support
  //     [26:26] 1G paging support
  //     [27:27] Support RDTSCP Instruction
  //     [28:28] Reserved
  //     [29:29] Long Mode
  //     [30:30] AMD 3DNow! Extensions
  //     [31:31] AMD 3DNow! Instructions
  unsigned features  = BX_CPU_VENDOR_INTEL ? 0 : get_std_cpuid_features();
  features &= 0x0183F3FF;
#if BX_SUPPORT_3DNOW
  // only AMD is interesting in AMD MMX extensions
  features |= BX_CPUID_STD2_AMD_MMX_EXT | BX_CPUID_STD2_3DNOW_EXT | BX_CPUID_STD2_3DNOW;
#endif
#if BX_SUPPORT_X86_64
  features |= BX_CPUID_STD2_SYSCALL_SYSRET |
              BX_CPUID_STD2_NX |
              BX_CPUID_STD2_FFXSR |
              BX_CPUID_STD2_RDTSCP | BX_CPUID_STD2_LONG_MODE;
  if (xlarge_pages_enabled)
    features |= BX_CPUID_STD2_1G_PAGES;
#endif
  cpuid->edx = features;

  BX_INFO(("CPUID[0x80000001]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // Processor Brand String, use the value that is returned
  // by the first processor in the Pentium 4 family
  // (according to Intel manual or the AMD manual)

  // ------------------------------------------------------
  // CPUID function 0x80000002

  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[2]);
  memcpy(&(cpuid->eax), brand_string     , 4);
  memcpy(&(cpuid->ebx), brand_string +  4, 4);
  memcpy(&(cpuid->ecx), brand_string +  8, 4);
  memcpy(&(cpuid->edx), brand_string + 12, 4);
#ifdef BX_BIG_ENDIAN
  cpuid->eax = bx_bswap32(cpuid->eax);
  cpuid->ebx = bx_bswap32(cpuid->ebx);
  cpuid->ecx = bx_bswap32(cpuid->ecx);
  cpuid->edx = bx_bswap32(cpuid->edx);
#endif

  BX_INFO(("CPUID[0x80000002]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // CPUID function 0x80000003
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[3]);
  memcpy(&(cpuid->eax), brand_string + 16, 4);
  memcpy(&(cpuid->ebx), brand_string + 20, 4);
  memcpy(&(cpuid->ecx), brand_string + 24, 4);
  memcpy(&(cpuid->edx), brand_string + 28, 4);
#ifdef BX_BIG_ENDIAN
  cpuid->eax = bx_bswap32(cpuid->eax);
  cpuid->ebx = bx_bswap32(cpuid->ebx);
  cpuid->ecx = bx_bswap32(cpuid->ecx);
  cpuid->edx = bx_bswap32(cpuid->edx);
#endif

  BX_INFO(("CPUID[0x80000003]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // CPUID function 0x80000004
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[4]);
  memcpy(&(cpuid->eax), brand_string + 32, 4);
  memcpy(&(cpuid->ebx), brand_string + 36, 4);
  memcpy(&(cpuid->ecx), brand_string + 40, 4);
  memcpy(&(cpuid->edx), brand_string + 44, 4);
#ifdef BX_BIG_ENDIAN
  cpuid->eax = bx_bswap32(cpuid->eax);
  cpuid->ebx = bx_bswap32(cpuid->ebx);
  cpuid->ecx = bx_bswap32(cpuid->ecx);
  cpuid->edx = bx_bswap32(cpuid->edx);
#endif

  BX_INFO(("CPUID[0x80000004]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

#if BX_SUPPORT_X86_64
  // ------------------------------------------------------
  // CPUID function 0x80000005

#if BX_CPU_VENDOR_INTEL == 0
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[5]);

  /* cache info (L1 cache) */
  cpuid->eax = 0x01ff01ff;
  cpuid->ebx = 0x01ff01ff;
  cpuid->ecx = 0x40020140;
  cpuid->edx = 0x40020140;

  BX_INFO(("CPUID[0x80000005]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));
#endif

  // ------------------------------------------------------
  // CPUID function 0x80000006
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[6]);

  /* cache info (L2 cache) */
  cpuid->eax = 0;
  cpuid->ebx = 0x42004200;
  cpuid->ecx = 0x02008140;
  cpuid->edx = 0;

  BX_INFO(("CPUID[0x80000006]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // ------------------------------------------------------
  // CPUID function 0x00000007
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[7]);

  cpuid->eax = 0;
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;

  BX_INFO(("CPUID[0x80000007]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

  // ------------------------------------------------------
  // CPUID function 0x80000008
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[8]);

  // virtual & phys address size in low 2 bytes.
  cpuid->eax = BX_PHY_ADDRESS_WIDTH | (BX_LIN_ADDRESS_WIDTH << 8);
  cpuid->ebx = 0;
  cpuid->ecx = 0; // Reserved, undefined
  cpuid->edx = 0;

  BX_INFO(("CPUID[0x80000008]: %08x %08x %08x %08x", cpuid->eax, cpuid->ebx, cpuid->ecx, cpuid->edx));

#endif // BX_SUPPORT_X86_64

#endif // BX_CPU_LEVEL >= 6
}

#if BX_SUPPORT_X2APIC

Bit32u ilog2(Bit32u x)
{
  Bit32u count = 0;
  while(x>>=1) count++;
  return count;
}

void BX_CPU_C::bx_cpuid_extended_topology_leaf(Bit32u subfunction)
{
  static int nthreads = SIM->get_param_num(BXPN_CPU_NTHREADS)->get();
  static int ncores = SIM->get_param_num(BXPN_CPU_NCORES)->get();
  static int nprocessors = SIM->get_param_num(BXPN_CPU_NPROCESSORS)->get();

  switch(subfunction) {
  case 0:
     if (nthreads > 1) {
        RAX = ilog2(nthreads-1)+1;
        RBX = nthreads;
        RCX = subfunction | (1<<8);
     }
     else if (ncores > 1) {
        RAX = ilog2(ncores-1)+1;
        RBX = ncores;
        RCX = subfunction | (2<<8);
     }
     else if (nprocessors > 1) {
        RAX = ilog2(nprocessors-1)+1;
        RBX = nprocessors;
        RCX = subfunction;
     }
     else {
        RAX = 0;
        RBX = 0;
        RCX = subfunction;
     }
     break;

  case 1:
     if (nthreads > 1) {
        if (ncores > 1) {
           RAX = ilog2(ncores-1)+1;
           RBX = ncores;
           RCX = subfunction | (2<<8);
        }
        else if (nprocessors > 1) {
           RAX = ilog2(nprocessors-1)+1;
           RBX = nprocessors;
           RCX = subfunction;
        }
        else {
           RAX = 0;
           RBX = 0;
           RCX = subfunction;
        }
     }
     else if (ncores > 1) {
        if (nprocessors > 1) {
           RAX = ilog2(nprocessors-1)+1;
           RBX = nprocessors;
           RCX = subfunction;
        }
        else {
           RAX = 0;
           RBX = 0;
           RCX = subfunction;
        }
     } else {
        RAX = 0;
        RBX = 0;
        RCX = subfunction;
     }
     break;

  case 2:
     if (nthreads > 1) {
        if (nprocessors > 1) {
           RAX = ilog2(nprocessors-1)+1;
           RBX = nprocessors;
        }
        else {
           RAX = 0;
           RBX = 0;
        }
     }
     else {
        RAX = 0;
        RBX = 0;
     }
     RCX = subfunction;
     break;

   default:
     RAX = 0;
     RBX = 0;
     RCX = subfunction;
     break;
   }

   RDX = BX_CPU_THIS_PTR lapic.get_id(); // x2apic ID
}

#endif

#if BX_CPU_LEVEL >= 6
void BX_CPU_C::bx_cpuid_extended_cpuid_leaf(Bit32u subfunction)
{
  if (subfunction == 0) {
    RAX = BX_CPU_THIS_PTR cpuid_std_function[0x7].eax;
    RBX = BX_CPU_THIS_PTR cpuid_std_function[0x7].ebx;
    RCX = BX_CPU_THIS_PTR cpuid_std_function[0x7].ecx;
    RDX = BX_CPU_THIS_PTR cpuid_std_function[0x7].edx;
  }
  else {
    RAX = 0; // reserved
    RBX = 0; // reserved
    RCX = 0; // reserved
    RDX = 0; // reserved
  }
}

void BX_CPU_C::bx_cpuid_xsave_leaf(Bit32u subfunction)
{
  BX_ASSERT(BX_CPU_SUPPORT_ISA_EXTENSION(BX_CPU_XSAVE));
  if (subfunction == 0) {
    RAX = BX_CPU_THIS_PTR cpuid_std_function[0xd].eax;
    RBX = BX_CPU_THIS_PTR cpuid_std_function[0xd].ebx;
    RCX = BX_CPU_THIS_PTR cpuid_std_function[0xd].ecx;
    RDX = BX_CPU_THIS_PTR cpuid_std_function[0xd].edx;
  }
  else {
    RAX = 0; // reserved
    RBX = 0; // reserved
    RCX = 0; // reserved
    RDX = 0; // reserved
  }
}
#endif

void BX_CPU_C::init_isa_features_bitmask(void)
{
  Bit32u features_bitmask = 0;

  bx_bool mmx_enabled = 0, movbe_enabled = 0;
  bx_bool sep_enabled = 0, xsave_enabled = 0;
  bx_bool aes_enabled = 0, xapic_enabled = 0;
  unsigned sse_enabled = 0;

#if BX_CPU_LEVEL >= 5
  mmx_enabled = SIM->get_param_bool(BXPN_CPUID_MMX)->get();
#endif
#if BX_CPU_LEVEL >= 6
  sep_enabled = SIM->get_param_bool(BXPN_CPUID_SEP)->get();
  aes_enabled = SIM->get_param_bool(BXPN_CPUID_AES)->get();
  movbe_enabled = SIM->get_param_bool(BXPN_CPUID_MOVBE)->get();
  xsave_enabled = SIM->get_param_bool(BXPN_CPUID_XSAVE)->get();
  xapic_enabled = SIM->get_param_bool(BXPN_CPUID_XAPIC)->get();
  sse_enabled = SIM->get_param_enum(BXPN_CPUID_SSE)->get();
#if BX_SUPPORT_X86_64
  bx_bool fsgsbase_enabled = SIM->get_param_bool(BXPN_CPUID_FSGSBASE)->get();
#endif
#if BX_SUPPORT_MONITOR_MWAIT
  bx_bool mwait_enabled = SIM->get_param_bool(BXPN_CPUID_MWAIT)->get();
#endif
#endif // BX_CPU_LEVEL >= 6

  // sanity checks
#if BX_SUPPORT_3DNOW
  if (! mmx_enabled) {
    BX_PANIC(("PANIC: 3DNOW emulation requires MMX support !"));
    return;
  }
#endif

  if (aes_enabled) {
     // AES required 3-byte opcode (SSS3E support or more)
     if (sse_enabled < BX_CPUID_SUPPORT_SSSE3) {
       BX_PANIC(("PANIC: AES support requires SSSE3 or higher !"));
       return;
     }
  }

  if (movbe_enabled) {
     // MOVBE required 3-byte opcode (SSS3E support or more)
     if (sse_enabled < BX_CPUID_SUPPORT_SSSE3) {
       BX_PANIC(("PANIC: MOVBE support requires SSSE3 or higher !"));
       return;
     }
  }

  if (xapic_enabled) {
     if (! BX_SUPPORT_APIC) {
       BX_PANIC(("PANIC: XAPIC enabled when APIC is not compiled in !"));
       return;
     }
  }

  if (sse_enabled) {
     if (mmx_enabled == 0 || BX_CPU_LEVEL < 6) {
       BX_PANIC(("PANIC: SSE support requires P6 emulation with MMX enabled !"));
       return;
     }

     if (sse_enabled >= BX_CPUID_SUPPORT_SSE2) {
       if (! xapic_enabled) {
         BX_PANIC(("PANIC: SSE2 is enabled and without XAPIC !"));
         return;
       }
     }
  }
  else {
     if (xsave_enabled) {
       BX_PANIC(("PANIC: XSAVE emulation requires SSE support !"));
       return;
     }
  }

#if BX_SUPPORT_X86_64
  if (sse_enabled < BX_CPUID_SUPPORT_SSE2) {
    BX_PANIC(("PANIC: x86-64 emulation requires SSE2 support !"));
    return;
  }
  if (! xapic_enabled) {
    BX_PANIC(("PANIC: x86-64 emulation requires XAPIC support !"));
    return;
  }
  if (! sep_enabled) {
    BX_PANIC(("PANIC: x86-64 emulation requires SYSENTER/SYSEXIT support !"));
    return;
  }
#endif

#if BX_SUPPORT_VMX
  if (! sep_enabled) {
    BX_PANIC(("PANIC: VMX emulation requires SYSENTER/SYSEXIT support !"));
    return;
  }
#endif

#if BX_SUPPORT_FPU
  features_bitmask |= BX_CPU_X87;
#endif
#if BX_CPU_LEVEL >= 4
  features_bitmask |= BX_CPU_486;
#endif
#if BX_CPU_LEVEL >= 5
  features_bitmask |= BX_CPU_PENTIUM;

  if (mmx_enabled)
    features_bitmask |= BX_CPU_MMX;
#endif
#if BX_SUPPORT_3DNOW
  features_bitmask |= BX_CPU_3DNOW;
#endif

#if BX_CPU_LEVEL >= 6
  features_bitmask |= BX_CPU_P6;

#if BX_SUPPORT_MONITOR_MWAIT
  if (mwait_enabled)
    features_bitmask |= BX_CPU_MONITOR_MWAIT;
#endif

  // FXSAVE/FXRSTOR support come with Pentium II
  if (mmx_enabled)
    features_bitmask |= BX_CPU_FXSAVE_FXRSTOR;

  // enabled CLFLUSH only when SSE2 or higher is enabled
  if (sse_enabled >= BX_CPUID_SUPPORT_SSE2)
    features_bitmask |= BX_CPU_CLFLUSH;

  // determine SSE in runtime
  switch (sse_enabled) {
    case BX_CPUID_SUPPORT_SSE4_2:
      features_bitmask |= BX_CPU_SSE4_2;
    case BX_CPUID_SUPPORT_SSE4_1:
      features_bitmask |= BX_CPU_SSE4_1;
    case BX_CPUID_SUPPORT_SSSE3:
      features_bitmask |= BX_CPU_SSSE3;
    case BX_CPUID_SUPPORT_SSE3:
      features_bitmask |= BX_CPU_SSE3;
    case BX_CPUID_SUPPORT_SSE2:
      features_bitmask |= BX_CPU_SSE2;
    case BX_CPUID_SUPPORT_SSE:
      features_bitmask |= BX_CPU_SSE;
    case BX_CPUID_SUPPORT_NOSSE:
    default:
      break;
  };

  if (sep_enabled)
    features_bitmask |= BX_CPU_SYSENTER_SYSEXIT;

  if (xsave_enabled)
    features_bitmask |= BX_CPU_XSAVE;

  if (aes_enabled)
    features_bitmask |= BX_CPU_AES_PCLMULQDQ;

  if (movbe_enabled)
    features_bitmask |= BX_CPU_MOVBE;
#endif

#if BX_SUPPORT_VMX
  features_bitmask |= BX_CPU_VMX;
#endif

#if BX_SUPPORT_X86_64
  features_bitmask |= BX_CPU_X86_64;

  if (fsgsbase_enabled)
    features_bitmask |= BX_CPU_FSGSBASE;
#endif

  BX_CPU_THIS_PTR isa_extensions_bitmask = features_bitmask;
}
