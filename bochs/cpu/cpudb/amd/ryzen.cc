/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2012-2017 Stanislav Shwartsman
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
#include "ryzen.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX

ryzen_t::ryzen_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  if (! BX_SUPPORT_SVM)
    BX_INFO(("WARNING: SVM support is not compiled in !"));

  if (! BX_SUPPORT_MONITOR_MWAIT)
    BX_INFO(("WARNING: MONITOR/MWAIT support is not compiled in !"));

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_MMX);
  enable_cpu_extension(BX_ISA_SYSCALL_SYSRET_LEGACY);
  enable_cpu_extension(BX_ISA_SYSENTER_SYSEXIT);
  enable_cpu_extension(BX_ISA_CLFLUSH);
  enable_cpu_extension(BX_ISA_DEBUG_EXTENSIONS);
  enable_cpu_extension(BX_ISA_VME);
  enable_cpu_extension(BX_ISA_PSE);
  enable_cpu_extension(BX_ISA_PAE);
  enable_cpu_extension(BX_ISA_PGE);
#if BX_PHY_ADDRESS_LONG
  enable_cpu_extension(BX_ISA_PSE36);
#endif
  enable_cpu_extension(BX_ISA_MTRR);
  enable_cpu_extension(BX_ISA_PAT);
  enable_cpu_extension(BX_ISA_XAPIC);
  enable_cpu_extension(BX_ISA_LONG_MODE);
  enable_cpu_extension(BX_ISA_LM_LAHF_SAHF);
  enable_cpu_extension(BX_ISA_CMPXCHG16B);
  enable_cpu_extension(BX_ISA_NX);
  enable_cpu_extension(BX_ISA_SSE);
  enable_cpu_extension(BX_ISA_SSE2);
  enable_cpu_extension(BX_ISA_SSE3);
  enable_cpu_extension(BX_ISA_SSSE3);
  enable_cpu_extension(BX_ISA_SSE4_1);
  enable_cpu_extension(BX_ISA_SSE4_2);
  enable_cpu_extension(BX_ISA_LZCNT);
  enable_cpu_extension(BX_ISA_POPCNT);
  enable_cpu_extension(BX_ISA_SSE4A);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_MONITOR_MWAIT);
#endif
  enable_cpu_extension(BX_ISA_RDTSCP);
  enable_cpu_extension(BX_ISA_XSAVE);
  enable_cpu_extension(BX_ISA_XSAVEOPT);
  enable_cpu_extension(BX_ISA_XSAVEC);
  enable_cpu_extension(BX_ISA_XSAVES);
  enable_cpu_extension(BX_ISA_AES_PCLMULQDQ);
  enable_cpu_extension(BX_ISA_AVX);
  enable_cpu_extension(BX_ISA_AVX_F16C);
  enable_cpu_extension(BX_ISA_AVX2);
  enable_cpu_extension(BX_ISA_AVX_FMA);
  enable_cpu_extension(BX_ISA_MOVBE);
  enable_cpu_extension(BX_ISA_RDRAND);
  enable_cpu_extension(BX_ISA_RDSEED);
  enable_cpu_extension(BX_ISA_BMI1);
  enable_cpu_extension(BX_ISA_BMI2);
  enable_cpu_extension(BX_ISA_FSGSBASE);
  enable_cpu_extension(BX_ISA_SMEP);
  enable_cpu_extension(BX_ISA_ADX);
  enable_cpu_extension(BX_ISA_SMAP);
  enable_cpu_extension(BX_ISA_SHA);
  enable_cpu_extension(BX_ISA_CLFLUSHOPT);
#if BX_SUPPORT_SVM
  enable_cpu_extension(BX_ISA_SVM);
#endif
  enable_cpu_extension(BX_ISA_FFXSR);
  enable_cpu_extension(BX_ISA_1G_PAGES);
  enable_cpu_extension(BX_ISA_MISALIGNED_SSE);
  enable_cpu_extension(BX_ISA_ALT_MOV_CR8);
  enable_cpu_extension(BX_ISA_XAPIC_EXT);
  enable_cpu_extension(BX_ISA_CLZERO);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_MONITORX_MWAITX);
#endif
}

void ryzen_t::get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  static const char* brand_string = "AMD Ryzen 7 1700 Eight-Core Processor          ";

  static bx_bool cpuid_limit_winnt = SIM->get_param_bool(BXPN_CPUID_LIMIT_WINNT)->get();
  if (cpuid_limit_winnt)
    if (function > 1 && function < 0x80000000) function = 1;

  switch(function) {
  case 0x80000000:
    get_ext_cpuid_leaf_0(leaf);
    return;
  case 0x80000001:
    get_ext_cpuid_leaf_1(leaf);
    return;
  case 0x80000002:
  case 0x80000003:
  case 0x80000004:
    get_ext_cpuid_brand_string_leaf(brand_string, function, leaf);
    return;
  case 0x80000005:
    get_ext_cpuid_leaf_5(leaf);
    return;
  case 0x80000006:
    get_ext_cpuid_leaf_6(leaf);
    return;
  case 0x80000007:
    get_ext_cpuid_leaf_7(leaf);
    return;
  case 0x80000008:
    get_ext_cpuid_leaf_8(leaf);
    return;
#if BX_SUPPORT_SVM
  case 0x8000000A:
    get_ext_cpuid_leaf_A(leaf);
    return;
#endif
  case 0x80000019:
    get_ext_cpuid_leaf_19(leaf);
    return;
  case 0x8000001A:
    get_ext_cpuid_leaf_1A(leaf);
    return;
  case 0x8000001B:
    get_ext_cpuid_leaf_1B(leaf);
    return;
  case 0x8000001C:
    get_ext_cpuid_leaf_1C(leaf);
    return;
  case 0x8000001D:
    get_ext_cpuid_leaf_1D(subfunction, leaf);
    return;
  case 0x8000001E:
    get_ext_cpuid_leaf_1E(leaf);
    return;
  case 0x8000001F:
    get_ext_cpuid_leaf_1F(leaf);
    return;
  case 0x00000000:
    get_std_cpuid_leaf_0(leaf);
    return;
  case 0x00000001:
    get_std_cpuid_leaf_1(leaf);
    return;
#if BX_SUPPORT_MONITOR_MWAIT
  case 0x00000005:
    get_std_cpuid_leaf_5(leaf);
    return;
#endif
  case 0x00000006:
    get_std_cpuid_leaf_6(leaf);
    return;
  case 0x00000007:
    get_std_cpuid_leaf_7(subfunction, leaf);
    return;
  case 0x0000000D:
    get_std_cpuid_xsave_leaf(subfunction, leaf);
    return;
  default:
    get_reserved_leaf(leaf);
    return;
  }
}

#if BX_SUPPORT_SVM
Bit32u ryzen_t::get_svm_extensions_bitmask(void) const
{
  return BX_CPUID_SVM_NESTED_PAGING |
         BX_CPUID_SVM_LBR_VIRTUALIZATION |
         BX_CPUID_SVM_SVM_LOCK |
         BX_CPUID_SVM_NRIP_SAVE | 
//       BX_CPUID_SVM_TSCRATE | // not implemented yet
//       BX_CPUID_SVM_VMCB_CLEAN_BITS | // not implemented yet
         BX_CPUID_SVM_FLUSH_BY_ASID |
//       BX_CPUID_SVM_DECODE_ASSIST | // not implemented yet
         BX_CPUID_SVM_PAUSE_FILTER;  // not implemented yet
//       BX_CPUID_SVM_PAUSE_FILTER_THRESHOLD; // not implemented yet
}
#endif

// leaf 0x00000000 //
void ryzen_t::get_std_cpuid_leaf_0(cpuid_function_t *leaf) const
{
  static const char* vendor_string = "AuthenticAMD";

  // EAX: highest std function understood by CPUID
  // EBX: vendor ID string
  // EDX: vendor ID string
  // ECX: vendor ID string
  unsigned max_leaf = 0xD;
  static bx_bool cpuid_limit_winnt = SIM->get_param_bool(BXPN_CPUID_LIMIT_WINNT)->get();
  if (cpuid_limit_winnt)
    max_leaf = 0x1;

  get_leaf_0(max_leaf, vendor_string, leaf);
}

// leaf 0x00000001 //
void ryzen_t::get_std_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [19:16] Extended Model
  //   [27:20] Extended Family
  leaf->eax = 0x00800f11;

  // EBX:
  //   [7:0]   Brand ID
  //   [15:8]  CLFLUSH cache line size (value*8 = cache line size in bytes)
  //   [23:16] Number of logical processors in one physical processor
  //   [31:24] Local Apic ID

  unsigned n_logical_processors = ncores*nthreads;
  leaf->ebx = ((CACHE_LINE_SIZE / 8) << 8) |
              (n_logical_processors << 16);
#if BX_SUPPORT_APIC
  leaf->ebx |= ((cpu->get_apic_id() & 0xff) << 24);
#endif

  // ECX: Extended Feature Flags
  // * [0:0]   SSE3: SSE3 Instructions
  // * [1:1]   PCLMULQDQ Instruction support
  //   [2:2]   DTES64: 64-bit DS area
  // * [3:3]   MONITOR/MWAIT support
  //   [4:4]   DS-CPL: CPL qualified debug store
  //   [5:5]   VMX: Virtual Machine Technology
  //   [6:6]   SMX: Secure Virtual Machine Technology
  //   [7:7]   EST: Enhanced Intel SpeedStep Technology
  //   [8:8]   TM2: Thermal Monitor 2
  // * [9:9]   SSSE3: SSSE3 Instructions
  //   [10:10] CNXT-ID: L1 context ID
  //   [11:11] reserved
  // * [12:12] FMA Instructions support
  // * [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  //   [14:14] xTPR update control
  //   [15:15] PDCM - Perfon and Debug Capability MSR
  //   [16:16] reserved
  //   [17:17] PCID: Process Context Identifiers
  //   [18:18] DCA - Direct Cache Access
  // * [19:19] SSE4.1 Instructions
  // * [20:20] SSE4.2 Instructions
  //   [21:21] X2APIC
  // * [22:22] MOVBE instruction
  // * [23:23] POPCNT instruction
  //   [24:24] TSC Deadline
  // * [25:25] AES Instructions
  // * [26:26] XSAVE extensions support
  // * [27:27] OSXSAVE support
  // * [28:28] AVX extensions support
  // * [29:29] AVX F16C - Float16 conversion support
  // * [30:30] RDRAND instruction
  //   [31:31] reserved
  leaf->ecx = BX_CPUID_EXT_SSE3 |
              BX_CPUID_EXT_PCLMULQDQ |
#if BX_SUPPORT_MONITOR_MWAIT
              BX_CPUID_EXT_MONITOR_MWAIT |
#endif
              BX_CPUID_EXT_SSSE3 |
              BX_CPUID_EXT_FMA |
              BX_CPUID_EXT_CMPXCHG16B |
              BX_CPUID_EXT_SSE4_1 |
              BX_CPUID_EXT_SSE4_2 |
              BX_CPUID_EXT_MOVBE |
              BX_CPUID_EXT_POPCNT |
              BX_CPUID_EXT_AES |
              BX_CPUID_EXT_XSAVE |
              BX_CPUID_EXT_AVX |
              BX_CPUID_EXT_AVX_F16C |
              BX_CPUID_EXT_RDRAND;
  if (cpu->cr4.get_OSXSAVE())
    leaf->ecx |= BX_CPUID_EXT_OSXSAVE;

  // EDX: Standard Feature Flags
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
  //   [10:10] Reserved
  // * [11:11] SYSENTER/SYSEXIT support
  // * [12:12] MTRR: Memory Type Range Reg
  // * [13:13] PGE/PTE Global Bit
  // * [14:14] MCA: Machine Check Architecture
  // * [15:15] CMOV: Cond Mov/Cmp Instructions
  // * [16:16] PAT: Page Attribute Table
  // * [17:17] PSE-36: Physical Address Extensions
  //   [18:18] PSN: Processor Serial Number
  // * [19:19] CLFLUSH: CLFLUSH Instruction support
  //   [20:20] Reserved
  //   [21:21] DS: Debug Store
  //   [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  // * [25:25] SSE: SSE Extensions
  // * [26:26] SSE2: SSE2 Extensions
  //   [27:27] Self Snoop
  // * [28:28] Hyper Threading Technology
  //   [29:29] TM: Thermal Monitor
  //   [30:30] Reserved
  //   [31:31] PBE: Pending Break Enable
  leaf->edx = BX_CPUID_STD_X87 |
              BX_CPUID_STD_VME |
              BX_CPUID_STD_DEBUG_EXTENSIONS |
              BX_CPUID_STD_PSE |
              BX_CPUID_STD_TSC |
              BX_CPUID_STD_MSR |
              BX_CPUID_STD_PAE |
              BX_CPUID_STD_MCE |
              BX_CPUID_STD_CMPXCHG8B |
              BX_CPUID_STD_SYSENTER_SYSEXIT |
              BX_CPUID_STD_MTRR |
              BX_CPUID_STD_GLOBAL_PAGES |
              BX_CPUID_STD_MCA |
              BX_CPUID_STD_CMOV |
              BX_CPUID_STD_PAT |
              BX_CPUID_STD_PSE36 |
              BX_CPUID_STD_CLFLUSH |
              BX_CPUID_STD_MMX |
              BX_CPUID_STD_FXSAVE_FXRSTOR |
              BX_CPUID_STD_SSE |
              BX_CPUID_STD_SSE2
#if BX_SUPPORT_SMP
              | BX_CPUID_STD_HT
#endif
              ;
#if BX_SUPPORT_APIC
  // if MSR_APICBASE APIC Global Enable bit has been cleared,
  // the CPUID feature flag for the APIC is set to 0.
  if (cpu->msr.apicbase & 0x800)
    leaf->edx |= BX_CPUID_STD_APIC; // APIC on chip
#endif
}

#if BX_SUPPORT_MONITOR_MWAIT

// leaf 0x00000005 //
void ryzen_t::get_std_cpuid_leaf_5(cpuid_function_t *leaf) const
{
  // CPUID function 0x00000005 - MONITOR/MWAIT Leaf

  // EAX - Smallest monitor-line size in bytes
  // EBX - Largest  monitor-line size in bytes
  // ECX -
  //   [31:2] - reserved
  //    [1:1] - exit MWAIT even with EFLAGS.IF = 0
  //    [0:0] - MONITOR/MWAIT extensions are supported
  // EDX -
  //  [03-00] - number of C0 sub C-states supported using MWAIT
  //  [07-04] - number of C1 sub C-states supported using MWAIT
  //  [11-08] - number of C2 sub C-states supported using MWAIT
  //  [15-12] - number of C3 sub C-states supported using MWAIT
  //  [19-16] - number of C4 sub C-states supported using MWAIT
  //  [31-20] - reserved (MBZ)
  leaf->eax = CACHE_LINE_SIZE;
  leaf->ebx = CACHE_LINE_SIZE;
  leaf->ecx = 3;
  leaf->edx = 0;
}

#endif

// leaf 0x00000006 //
void ryzen_t::get_std_cpuid_leaf_6(cpuid_function_t *leaf) const
{
  // CPUID function 0x00000006 - Thermal and Power Management Leaf
  leaf->eax = 0x00000004;
  leaf->ebx = 0x00000000;
  leaf->ecx = 0x00000001;
  leaf->edx = 0x00000000;
}

// leaf 0x00000007 //
void ryzen_t::get_std_cpuid_leaf_7(Bit32u subfunction, cpuid_function_t *leaf) const
{
  switch(subfunction) {
  case 0:
    leaf->eax = 0; /* report max sub-leaf that supported in leaf 7 */

    // * [0:0]   FS/GS BASE access instructions
    //   [1:1]   Support for IA32_TSC_ADJUST MSR
    //   [2:2]   reserved
    // * [3:3]   BMI1: Advanced Bit Manipulation Extensions
    //   [4:4]   HLE: Hardware Lock Elision
    // * [5:5]   AVX2
    //   [6:6]   reserved
    // * [7:7]   SMEP: Supervisor Mode Execution Protection
    // * [8:8]   BMI2: Advanced Bit Manipulation Extensions
    //   [9:9]   Support for Enhanced REP MOVSB/STOSB
    //   [10:10] Support for INVPCID instruction
    //   [11:11] RTM: Restricted Transactional Memory
    //   [12:12] Supports Quality of Service (QoS) capability
    //   [13:13] Deprecates FPU CS and FPU DS values
    //   [14:14] Intel Memory Protection Extensions
    //   [15:15] Supports Platform Quality of Service Enforcement (PQE) capability
    //   [16:16] AVX512F instructions support
    //   [17:17] reserved
    // * [18:18] RDSEED instruction support
    // * [19:19] ADCX/ADOX instructions support
    // * [20:20] SMAP: Supervisor Mode Access Prevention
    //   [21:21] reserved
    //   [22:22] reserved
    // * [23:23] CLFLUSHOPT instruction
    //   [24:24] reserved
    //   [25:25] Intel Processor Trace
    //   [26:26] AVX512PF instructions support
    //   [27:27] AVX512ER instructions support
    //   [28:28] AVX512CD instructions support
    // * [29:29] SHA instructions support
    //   [30:30] reserved
    //   [31:31] reserved
    leaf->ebx = BX_CPUID_EXT3_FSGSBASE | 
                BX_CPUID_EXT3_BMI1 | 
                BX_CPUID_EXT3_AVX2 |
                BX_CPUID_EXT3_SMEP | 
                BX_CPUID_EXT3_BMI2 | 
                BX_CPUID_EXT3_RDSEED |
                BX_CPUID_EXT3_ADX |
                BX_CPUID_EXT3_SMAP |
                BX_CPUID_EXT3_CLFLUSHOPT |
                BX_CPUID_EXT3_SHA;
    leaf->ecx = 0;
    leaf->edx = 0;
    break;
  default:
    leaf->eax = 0;
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
  }
}

// leaf 0x0000000D - XSAVE //

// leaf 0x80000000 //
void ryzen_t::get_ext_cpuid_leaf_0(cpuid_function_t *leaf) const
{
  get_leaf_0(0x8000001F, "AuthenticAMD", leaf);
}

// leaf 0x80000001 //
void ryzen_t::get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information (same as 0x00000001.EAX)
  leaf->eax = 0x00800f11;

  // EBX:
  //  [15:00]  Brand ID
  //  [27:16]  Reserved
  //  [31:28]  Package Type
  leaf->ebx = 0x20000000;

  // ECX:
  // * [0:0]   LAHF/SAHF instructions support in 64-bit mode
  // * [1:1]   CMP_Legacy: Core multi-processing legacy mode (AMD)
  // * [2:2]   SVM: Secure Virtual Machine (AMD)
  // * [3:3]   Extended APIC Space
  // * [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
  // * [5:5]   LZCNT: LZCNT instruction support
  // * [6:6]   SSE4A: SSE4A Instructions support
  // * [7:7]   Misaligned SSE support
  // * [8:8]   PREFETCHW: PREFETCHW instruction support
  // * [9:9]   OSVW: OS visible workarounds (AMD)
  //   [10:10] IBS: Instruction based sampling
  //   [11:11] XOP: Extended Operations Support and XOP Prefix
  // * [12:12] SKINIT support
  // * [13:13] WDT: Watchdog timer support
  //   [14:14] Reserved
  //   [15:15] LWP: Light weight profiling
  //   [16:16] FMA4: Four-operand FMA instructions support
  // * [17:17] Translation Cache Extensions
  //   [18:18] Reserved
  //   [19:19] NodeId: Indicates support for NodeId MSR (0xc001100c)
  //   [20:20] Reserved
  //   [21:21] TBM: trailing bit manipulation instructions support
  // * [22:22] Topology extensions support
  // * [23:23] PerfCtrExtCore: core perf counter extensions support
  // * [24:24] PerfCtrExtNB: NB perf counter extensions support
  //   [25:25] Reserved
  // * [26:26] Data breakpoint extension. Indicates support for MSR 0xC0011027 and MSRs 0xC001101[B:9]
  //   [27:27] Performance time-stamp counter. Indicates support for MSR 0xC0010280
  // * [28:28] PerfCtrExtL2I: L2I performance counter extensions support
  // * [29:29] MONITORX/MWAITX instructions support
  //   [30:30] Reserved
  //   [31:31] Reserved
  leaf->ecx = BX_CPUID_EXT2_LAHF_SAHF |
              BX_CPUID_EXT2_CMP_LEGACY |
#if BX_SUPPORT_SVM
              BX_CPUID_EXT2_SVM |
#endif
              BX_CPUID_EXT2_EXT_APIC_SPACE |
              BX_CPUID_EXT2_ALT_MOV_CR8 |
              BX_CPUID_EXT2_LZCNT |
              BX_CPUID_EXT2_SSE4A |
              BX_CPUID_EXT2_MISALIGNED_SSE |
              BX_CPUID_EXT2_PREFETCHW |
              BX_CPUID_EXT2_OSVW |
           /* BX_CPUID_EXT2_SKINIT | */ // not implemented
              BX_CPUID_EXT2_WDT |
              BX_CPUID_EXT2_TCE |
              BX_CPUID_EXT2_TOPOLOGY_EXTENSIONS |
              BX_CPUID_EXT2_PERFCTR_EXT_CORE |
              BX_CPUID_EXT2_PERFCTR_EXT_NB |
              BX_CPUID_EXT2_DATA_BREAKPOINT_EXT |
              BX_CPUID_EXT2_PERFCTR_EXT_L2I |
#if BX_SUPPORT_MONITOR_MWAIT
              BX_CPUID_EXT2_MONITORX_MWAITX |
#endif
              0;

  // EDX:
  // Many of the bits in EDX are the same as FN 0x00000001 for AMD
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
  //   [10:10] Reserved
  // * [11:11] SYSCALL/SYSRET support
  // * [12:12] MTRR: Memory Type Range Reg
  // * [13:13] PGE/PTE Global Bit
  // * [14:14] MCA: Machine Check Architecture
  // * [15:15] CMOV: Cond Mov/Cmp Instructions
  // * [16:16] PAT: Page Attribute Table
  // * [17:17] PSE-36: Physical Address Extensions
  //   [18:18] Reserved
  //   [19:19] Reserved
  // * [20:20] No-Execute page protection
  //   [21:21] Reserved
  // * [22:22] AMD MMX Extensions
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  // * [25:25] Fast FXSAVE/FXRSTOR mode support
  // * [26:26] 1G paging support
  // * [27:27] Support RDTSCP Instruction
  //   [28:28] Reserved
  // * [29:29] Long Mode
  //   [30:30] AMD 3DNow! Extensions
  //   [31:31] AMD 3DNow! Instructions
  leaf->edx = BX_CPUID_STD_X87 |
              BX_CPUID_STD_VME |
              BX_CPUID_STD_DEBUG_EXTENSIONS |
              BX_CPUID_STD_PSE |
              BX_CPUID_STD_TSC |
              BX_CPUID_STD_MSR |
              BX_CPUID_STD_PAE |
              BX_CPUID_STD_MCE |
              BX_CPUID_STD_CMPXCHG8B |
              BX_CPUID_STD2_SYSCALL_SYSRET |
              BX_CPUID_STD_MTRR |
              BX_CPUID_STD_GLOBAL_PAGES |
              BX_CPUID_STD_MCA |
              BX_CPUID_STD_CMOV |
              BX_CPUID_STD_PAT |
              BX_CPUID_STD_PSE36 |
              BX_CPUID_STD2_NX |
              BX_CPUID_STD2_AMD_MMX_EXT |
              BX_CPUID_STD_MMX |
              BX_CPUID_STD_FXSAVE_FXRSTOR |
              BX_CPUID_STD2_FFXSR |
              BX_CPUID_STD2_1G_PAGES |
              BX_CPUID_STD2_RDTSCP |
              BX_CPUID_STD2_LONG_MODE;
#if BX_SUPPORT_APIC
  // if MSR_APICBASE APIC Global Enable bit has been cleared,
  // the CPUID feature flag for the APIC is set to 0.
  if (cpu->msr.apicbase & 0x800)
    leaf->edx |= BX_CPUID_STD_APIC; // APIC on chip
#endif
}

// leaf 0x80000002 //
// leaf 0x80000003 //
// leaf 0x80000004 //

// leaf 0x80000005 //
void ryzen_t::get_ext_cpuid_leaf_5(cpuid_function_t *leaf) const
{
  // CPUID function 0x800000005 - L1 Cache and TLB Identifiers
  leaf->eax = 0xff40ff40;
  leaf->ebx = 0xff40ff40;
  leaf->ecx = 0x20080140;
  leaf->edx = 0x40040140;
}

// leaf 0x80000006 //
void ryzen_t::get_ext_cpuid_leaf_6(cpuid_function_t *leaf) const
{
  // CPUID function 0x800000006 - L2 Cache and TLB Identifiers      
  leaf->eax = 0x26006400;
  leaf->ebx = 0x66006400;
  leaf->ecx = 0x02006140;
  leaf->edx = 0x00808140;
}

// leaf 0x80000007 //
void ryzen_t::get_ext_cpuid_leaf_7(cpuid_function_t *leaf) const
{
  // CPUID function 0x800000007 - Advanced Power Management
  leaf->eax = 0;
  leaf->ebx = 0x0000001b;
  leaf->ecx = 0;
  leaf->edx = 0x00006599;
}

// leaf 0x80000008 //
void ryzen_t::get_ext_cpuid_leaf_8(cpuid_function_t *leaf) const
{
  bx_cpuid_t::get_ext_cpuid_leaf_8(leaf);

  // [17..16] Performance time-stamp counter size.
  // [15..12] APIC ID size, Zero value indicates that legacy methods must be
  //          used to derive the maximum number of cores.
  //  [11..8] (reserved)
  //   [7..0] Number of physical cores - 1.
  leaf->ecx = ncores - 1;
}

// leaf 0x80000009 : Reserved //

#if BX_SUPPORT_SVM

// leaf 0x8000000A : SVM //
void ryzen_t::get_ext_cpuid_leaf_A(cpuid_function_t *leaf) const
{
  leaf->eax = 0x01;     /* SVM revision ID */
  leaf->ebx = 0x8000;   /* number of ASIDs */
  leaf->ecx = 0;

  // * [0:0]   NP - Nested paging support
  // * [1:1]   LBR virtualization
  // * [2:2]   SVM Lock
  // * [3:3]   NRIPS - Next RIP save on VMEXIT
  // * [4:4]   TscRate - MSR based TSC ratio control
  // * [5:5]   VMCB Clean bits support
  // * [6:6]   Flush by ASID support
  // * [7:7]   Decode assists support
  //   [8:8]   Reserved
  //   [9:9]   Reserved
  // * [10:10] Pause filter support
  //   [11:11] Reserved
  // * [12:12] Pause filter threshold support
  // * [13:13] Advanced Virtual Interrupt Controller
  leaf->edx = get_svm_extensions_bitmask();
}

#endif

// leaf 0x8000000B - 0x80000018: Reserved //

void ryzen_t::get_ext_cpuid_leaf_19(cpuid_function_t *leaf) const
{
  // CPUID function 0x800000019 - 1G Page TLB Identifiers
  leaf->eax = 0xf040f040;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;
}

void ryzen_t::get_ext_cpuid_leaf_1A(cpuid_function_t *leaf) const
{
  // CPUID function 0x80000001A - Performance Optimization Identifiers
  leaf->eax = 0x00000003;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;
}

void ryzen_t::get_ext_cpuid_leaf_1B(cpuid_function_t *leaf) const
{
  // CPUID function 0x80000001B - Instruction Based Sampling Identifiers
  leaf->eax = 0x3FF;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;

  BX_INFO(("WARNING: Instruction Based Sampling is not implemented"));
}

void ryzen_t::get_ext_cpuid_leaf_1C(cpuid_function_t *leaf) const
{
  // CPUID function 0x80000001C - Lightweight Profiling Capabilities (not implemented)
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;
}

void ryzen_t::get_ext_cpuid_leaf_1D(Bit32u subfunction, cpuid_function_t *leaf) const
{
  // CPUID function 0x80000001D - Cache Properties
  switch(subfunction) {
  case 0:
    leaf->eax = 0x00004121;
    leaf->ebx = 0x01c0003f;
    leaf->ecx = 0x0000003f;
    leaf->edx = 0x00000000;
    return;
  case 1:
    leaf->eax = 0x00004122;
    leaf->ebx = 0x00c0003f;
    leaf->ecx = 0x000000ff;
    leaf->edx = 0x00000000;
    return;
  case 2:
    leaf->eax = 0x00004143;
    leaf->ebx = 0x01c0003f;
    leaf->ecx = 0x000003FF;
    leaf->edx = 0x00000002;
    return;
  case 3:
    leaf->eax = 0x0001c163;
    leaf->ebx = 0x03c0003f;
    leaf->ecx = 0x00001fff;
    leaf->edx = 0x00000001;
    return;
  default:
    leaf->eax = 0;
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
    return;
  }
}

void ryzen_t::get_ext_cpuid_leaf_1E(cpuid_function_t *leaf) const
{
  // CPUID function 0x80000001E - Topology Extensions
  leaf->eax = 0;
  leaf->ebx = (ncores - 1) << 8;
  leaf->ecx = 0;
  leaf->edx = 0;
}

void ryzen_t::get_ext_cpuid_leaf_1F(cpuid_function_t *leaf) const
{
  leaf->eax = 0x00000007;
  leaf->ebx = 0x0000016f;
  leaf->ecx = 0x0000000f;
  leaf->edx = 0;
}

void ryzen_t::dump_cpuid(void) const
{
  bx_cpuid_t::dump_cpuid(0xD, 0x1F);
}

bx_cpuid_t *create_ryzen_cpuid(BX_CPU_C *cpu) { return new ryzen_t(cpu); }

#endif
