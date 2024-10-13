/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2017-2018 Stanislav Shwartsman
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
#include "corei7_skylake-x.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX

corei7_skylake_x_t::corei7_skylake_x_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  if (BX_SUPPORT_VMX == 1)
    BX_INFO(("You must compile with --enable-vmx=2 for Intel Core i7 Skylake-X VMX configuration"));

  if (! BX_SUPPORT_MONITOR_MWAIT)
    BX_INFO(("WARNING: MONITOR/MWAIT support is not compiled in !"));

  enable_cpu_extension(BX_ISA_X87);
  enable_cpu_extension(BX_ISA_486);
  enable_cpu_extension(BX_ISA_PENTIUM);
  enable_cpu_extension(BX_ISA_P6);
  enable_cpu_extension(BX_ISA_MMX);
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
  enable_cpu_extension(BX_ISA_X2APIC);
  enable_cpu_extension(BX_ISA_LONG_MODE);
  enable_cpu_extension(BX_ISA_LM_LAHF_SAHF);
  enable_cpu_extension(BX_ISA_CMPXCHG16B);
  enable_cpu_extension(BX_ISA_NX);
  enable_cpu_extension(BX_ISA_1G_PAGES);
  enable_cpu_extension(BX_ISA_PCID);
  enable_cpu_extension(BX_ISA_TSC_ADJUST);
  enable_cpu_extension(BX_ISA_TSC_DEADLINE);
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
  enable_cpu_extension(BX_ISA_RDTSCP);
  enable_cpu_extension(BX_ISA_XSAVE);
  enable_cpu_extension(BX_ISA_XSAVEOPT);
  enable_cpu_extension(BX_ISA_XSAVEC);
  enable_cpu_extension(BX_ISA_XSAVES);
  enable_cpu_extension(BX_ISA_AES_PCLMULQDQ);
  enable_cpu_extension(BX_ISA_MOVBE);
  enable_cpu_extension(BX_ISA_AVX);
  enable_cpu_extension(BX_ISA_AVX_F16C);
  enable_cpu_extension(BX_ISA_AVX2);
  enable_cpu_extension(BX_ISA_AVX_FMA);
  enable_cpu_extension(BX_ISA_LZCNT);
  enable_cpu_extension(BX_ISA_BMI1);
  enable_cpu_extension(BX_ISA_BMI2);
  enable_cpu_extension(BX_ISA_FSGSBASE);
  enable_cpu_extension(BX_ISA_INVPCID);
  enable_cpu_extension(BX_ISA_SMEP);
  enable_cpu_extension(BX_ISA_RDRAND);
  enable_cpu_extension(BX_ISA_FCS_FDS_DEPRECATION);
  enable_cpu_extension(BX_ISA_RDSEED);
  enable_cpu_extension(BX_ISA_ADX);
  enable_cpu_extension(BX_ISA_SMAP);
  enable_cpu_extension(BX_ISA_FDP_DEPRECATION);
#if BX_SUPPORT_EVEX
  enable_cpu_extension(BX_ISA_AVX512);
  enable_cpu_extension(BX_ISA_AVX512_DQ);
  enable_cpu_extension(BX_ISA_AVX512_CD);
  enable_cpu_extension(BX_ISA_AVX512_BW);
#endif
  enable_cpu_extension(BX_ISA_CLFLUSHOPT);
  enable_cpu_extension(BX_ISA_CLWB);

  max_std_leaf = 0x16;
  max_ext_leaf = 0x80000008;
}

void corei7_skylake_x_t::get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  static const char* brand_string = "Intel(R) Core(TM) i7-7800X CPU @ 3.50GHz\0\0\0\0\0\0\0\0";
  static bool cpuid_limit_winnt = SIM->get_param_bool(BXPN_CPUID_LIMIT_WINNT)->get();
  if (cpuid_limit_winnt)
    if (function > 2 && function < 0x80000000) function = 2;

  switch(function) {
  case 0x80000000:
    get_leaf_0(max_ext_leaf, NULL, leaf); // highest extended function understood by CPUID
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
    get_reserved_leaf(leaf);
    return;
  case 0x80000006: // CPUID leaf 0x80000006 - L2 Cache and TLB Identifiers
    get_leaf(leaf, 0x00000000, 0x00000000, 0x01006040, 0x00000000);
    return;
  case 0x80000007: // CPUID leaf 0x80000007 - Advanced Power Management leaf
    get_leaf(leaf, 0x00000000, 0x00000000, 0x00000000, 0x00000100); // EDX[8] - invariant TSC
    return;
  case 0x80000008:
    get_ext_cpuid_leaf_8(leaf);
    return;
  case 0x00000000:
    get_leaf_0(max_std_leaf, "GenuineIntel", leaf);
    return;
  case 0x00000001:
    get_std_cpuid_leaf_1(leaf);
    return;
  case 0x00000002:
    get_std_cpuid_leaf_2(leaf);
    return;
  case 0x00000003:
    get_reserved_leaf(leaf);
    return;
  case 0x00000004:
    get_std_cpuid_leaf_4(subfunction, leaf);
    return;
  case 0x00000005:
    get_std_cpuid_monitor_mwait_leaf(leaf, 0x00002020);
    return;
  case 0x00000006: // CPUID leaf 0x00000006 - Thermal and Power Management Leaf
    get_leaf(leaf, 0x00000075, 0x00000002, 0x00000009, 0x00000000);
    return;
  case 0x00000007:
    get_std_cpuid_leaf_7(subfunction, leaf);
    return;
  case 0x00000008:
  case 0x00000009:
    get_reserved_leaf(leaf);
    return;
  case 0x0000000A:
    get_std_cpuid_leaf_A(leaf);
    return;
  case 0x0000000B:
    get_std_cpuid_extended_topology_leaf(subfunction, leaf);
    return;
  case 0x0000000C:
    get_reserved_leaf(leaf);
    return;
  case 0x0000000D:
    get_std_cpuid_xsave_leaf(subfunction, leaf);
    return;
  case 0x0000000E:
  case 0x0000000F:
  case 0x00000010:
  case 0x00000011:
  case 0x00000012:
  case 0x00000013:
  case 0x00000014:
    get_reserved_leaf(leaf);
    return;
  case 0x00000015: // CPUID leaf 0x000000015 - Time Stamp Counter and Nominal Core Crystal Clock Information
    get_leaf(leaf, 0x00000002, 0x00000124, 0x00000000, 0x00000000);
    return;
  case 0x00000016: // CPUID leaf 0x000000016 - Processor Frequency Information
  default:
    get_leaf(leaf, 0x00000dac, 0x00000fa0, 0x00000064, 0x00000000);
    return;
  }
}

#if BX_SUPPORT_VMX >= 2

// MSR 00000480: 00DA-0400-0000-0004	BX_MSR_VMX_BASIC
// MSR 00000481: 0000-00FF-0000-0016	BX_MSR_VMX_PINBASED_CTRLS
// MSR 00000482: FFF9-FFFE-0401-E172	BX_MSR_VMX_PROCBASED_CTRLS
// MSR 00000483: 01FF-FFFF-0003-6DFF	BX_MSR_VMX_VMEXIT_CTRLS
// MSR 00000484: 0003-FFFF-0000-11FF	BX_MSR_VMX_VMENTRY_CTRLS
// MSR 00000485: 0000-0000-7004-C1E7	BX_MSR_VMX_MISC
// MSR 00000486: 0000-0000-8000-0021	BX_MSR_VMX_CR0_FIXED0
// MSR 00000487: 0000-0000-FFFF-FFFF	BX_MSR_VMX_CR0_FIXED1
// MSR 00000488: 0000-0000-0000-2000	BX_MSR_VMX_CR4_FIXED0
// MSR 00000489: 0000-0000-0037-27FF	BX_MSR_VMX_CR4_FIXED1
// MSR 0000048A: 0000-0000-0000-002E	BX_MSR_VMX_VMCS_ENUM
// MSR 0000048B: 025D-3FFF-0000-0000	BX_MSR_VMX_PROCBASED_CTRLS2
// MSR 0000048C: 0000-0F01-0673-4141	BX_MSR_VMX_MSR_VMX_EPT_VPID_CAP
// MSR 0000048D: 0000-00FF-0000-0016	BX_MSR_VMX_TRUE_PINBASED_CTRLS
// MSR 0000048E: FFF9-FFFE-0400-6172    BX_MSR_VMX_TRUE_PROCBASED_CTRLS
// MSR 0000048F: 01FF-FFFF-0003-6DFB	BX_MSR_VMX_TRUE_VMEXIT_CTRLS
// MSR 00000490: 0003-FFFF-0000-11FB	BX_MSR_VMX_TRUE_VMENTRY_CTRLS

Bit32u corei7_skylake_x_t::get_vmx_extensions_bitmask(void) const
{
  return BX_VMX_TPR_SHADOW |
         BX_VMX_VIRTUAL_NMI |
         BX_VMX_APIC_VIRTUALIZATION |
         BX_VMX_WBINVD_VMEXIT |
         BX_VMX_MONITOR_TRAP_FLAG |
         BX_VMX_VPID |
         BX_VMX_EPT |
         BX_VMX_UNRESTRICTED_GUEST |
         BX_VMX_SAVE_DEBUGCTL_DISABLE |
         BX_VMX_PERF_GLOBAL_CTRL |     // MSR not implemented yet
         BX_VMX_PAT |
         BX_VMX_EFER |
         BX_VMX_DESCRIPTOR_TABLE_EXIT |
         BX_VMX_X2APIC_VIRTUALIZATION |
         BX_VMX_PREEMPTION_TIMER |
         BX_VMX_PAUSE_LOOP_EXITING |
         BX_VMX_EPTP_SWITCHING |
         BX_VMX_EPT_ACCESS_DIRTY |
         BX_VMX_VINTR_DELIVERY |
         BX_VMX_VMCS_SHADOWING |
         BX_VMX_EPT_EXCEPTION |
         BX_VMX_SW_INTERRUPT_INJECTION_ILEN_0 |
         BX_VMX_PML |
         BX_VMX_POSTED_INTERRUPTS |
         BX_VMX_MBE_CONTROL |
         BX_VMX_TSC_SCALING;
}

#endif

// leaf 0x00000000 //

// leaf 0x00000001 //
void corei7_skylake_x_t::get_std_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [19:16] Extended Model
  //   [27:20] Extended Family
  leaf->eax = 0x00050654;

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
  // * [2:2]   DTES64: 64-bit DS area
  // * [3:3]   MONITOR/MWAIT support
  // * [4:4]   DS-CPL: CPL qualified debug store
  // * [5:5]   VMX: Virtual Machine Technology
  //   [6:6]   SMX: Secure Virtual Machine Technology
  // * [7:7]   EST: Enhanced Intel SpeedStep Technology
  // * [8:8]   TM2: Thermal Monitor 2
  // * [9:9]   SSSE3: SSSE3 Instructions
  //   [10:10] CNXT-ID: L1 context ID
  // * [11:11] reserved
  // * [12:12] FMA Instructions support
  // * [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  // * [14:14] xTPR update control
  // * [15:15] PDCM - Perfmon and Debug Capability MSR
  //   [16:16] reserved
  // * [17:17] PCID: Process Context Identifiers
  //   [18:18] DCA - Direct Cache Access
  // * [19:19] SSE4.1 Instructions
  // * [20:20] SSE4.2 Instructions
  // * [21:21] X2APIC
  // * [22:22] MOVBE instruction
  // * [23:23] POPCNT instruction
  // * [24:24] TSC Deadline
  // * [25:25] AES Instructions
  // * [26:26] XSAVE extensions support
  // * [27:27] OSXSAVE support
  // * [28:28] AVX extensions support
  // * [29:29] AVX F16C - Float16 conversion support
  // * [30:30] RDRAND instruction
  //   [31:31] reserved
  leaf->ecx = get_std_cpuid_leaf_1_ecx(BX_CPUID_STD1_ECX_DTES64 |
                                       BX_CPUID_STD1_ECX_DS_CPL |
                                       BX_CPUID_STD1_ECX_EST |
                                       BX_CPUID_STD1_ECX_THERMAL_MONITOR2 |
                                       BX_CPUID_STD1_ECX_xTPR |
                                       BX_CPUID_STD1_ECX_PDCM);

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
  // * [21:21] DS: Debug Store
  // * [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
  // * [23:23] MMX Technology
  // * [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
  // * [25:25] SSE: SSE Extensions
  // * [26:26] SSE2: SSE2 Extensions
  // * [27:27] Self Snoop
  // * [28:28] Hyper Threading Technology
  // * [29:29] TM: Thermal Monitor
  //   [30:30] Reserved
  // * [31:31] PBE: Pending Break Enable
  leaf->edx = get_std_cpuid_leaf_1_edx(BX_CPUID_STD1_EDX_DEBUG_STORE |
                                       BX_CPUID_STD1_EDX_ACPI |
                                       BX_CPUID_STD1_EDX_SELF_SNOOP |
#if BX_SUPPORT_SMP
                                       BX_CPUID_STD1_EDX_HT |
#endif
                                       BX_CPUID_STD1_EDX_THERMAL_MONITOR |
                                       BX_CPUID_STD1_EDX_PBE);
}

// leaf 0x00000002 - Cache and TLB Descriptors //
void corei7_skylake_x_t::get_std_cpuid_leaf_2(cpuid_function_t *leaf) const
{
  // CPUID function 0x00000002 - Cache and TLB Descriptors
  leaf->eax = 0x76036301;
  leaf->ebx = 0x00f0b5ff;
  leaf->ecx = 0x00000000;
  leaf->edx = 0x00c30000;
}

// leaf 0x00000003 - Processor Serial Number (not supported) //

// leaf 0x00000004 //
void corei7_skylake_x_t::get_std_cpuid_leaf_4(Bit32u subfunction, cpuid_function_t *leaf) const
{
  // CPUID function 0x00000004 - Deterministic Cache Parameters

  // EAX:
  //   [04-00] - Cache Type Field
  //             0 = No more caches
  //             1 = Data Cache
  //             2 = Instruction Cache
  //             3 = Unified Cache
  //   [07-05] - Cache Level (starts at 1)]
  //      [08] - Self Initializing cache level (doesn't need software initialization)
  //      [09] - Fully Associative cache
  //   [13-10] - Reserved
  //   [25-14] - Maximum number of addressable IDs for logical processors sharing this cache
  //   [31-26] - Maximum number of addressable IDs for processor cores in the physical package - 1
  // EBX:
  //   [11-00] - L = System Coherency Line Size
  //   [21-12] - P = Physical Line partitions
  //   [31-22] - W = Ways of associativity
  // ECX: Number of Sets
  // EDX:
  //      [00] - Writeback invalidate
  //      [01] - Cache Inclusiveness
  //      [02] - Complex Cache Indexing
  //   [31-03] - Reserved

  switch(subfunction) {
  case 0:
    leaf->eax = 0x1c004121;
    leaf->ebx = 0x01c0003f;
    leaf->ecx = 0x0000003f;
    leaf->edx = 0x00000000;
    break;
  case 1:
    leaf->eax = 0x1c004122;
    leaf->ebx = 0x01c0003f;
    leaf->ecx = 0x0000003f;
    leaf->edx = 0x00000000;
    break;
  case 2:
    leaf->eax = 0x1c004143;
    leaf->ebx = 0x03c0003f;
    leaf->ecx = 0x000003ff;
    leaf->edx = 0x00000000;
    break;
  case 3:
    leaf->eax = 0x1c03c163;
    leaf->ebx = 0x0280003f;
    leaf->ecx = 0x00002fff;
    leaf->edx = 0x00000004;
    break;
  default:
    leaf->eax = 0;
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
    return;
  }
}

// leaf 0x00000005 MONITOR/MWAIT Leaf                //
// leaf 0x00000006 Thermal and Power Management Leaf //

// leaf 0x00000007 //
void corei7_skylake_x_t::get_std_cpuid_leaf_7(Bit32u subfunction, cpuid_function_t *leaf) const
{
  switch(subfunction) {
  case 0:
    leaf->eax = 0; /* report max sub-leaf that supported in leaf 7 */

    // * [0:0]   FS/GS BASE access instructions
    // * [1:1]   Support for IA32_TSC_ADJUST MSR
    //   [2:2]   SGX: Intel Software Guard Extensions
    // * [3:3]   BMI1: Advanced Bit Manipulation Extensions
    // x [4:4]   HLE: Hardware Lock Elision
    // * [5:5]   AVX2
    // * [6:6]   FDP Deprecation
    // * [7:7]   SMEP: Supervisor Mode Execution Protection
    // * [8:8]   BMI2: Advanced Bit Manipulation Extensions
    // * [9:9]   Support for Enhanced REP MOVSB/STOSB
    // * [10:10] Support for INVPCID instruction
    // x [11:11] RTM: Restricted Transactional Memory
    // x [12:12] Supports Quality of Service (QoS) capability
    // * [13:13] Deprecates FPU CS and FPU DS values
    //   [14:14] Intel Memory Protection Extensions
    // x [15:15] Supports Platform Quality of Service Enforcement (PQE) capability
    // * [16:16] AVX512F instructions support
    // * [17:17] AVX512DQ instructions support
    // * [18:18] RDSEED instruction support
    // * [19:19] ADCX/ADOX instructions support
    // * [20:20] SMAP: Supervisor Mode Access Prevention
    //   [21:21] AVX512IFMA52 instructions support
    //   [22:22] reserved
    // * [23:23] CLFLUSHOPT instruction
    // * [24:24] CLWB instruction
    // x [25:25] Intel Processor Trace
    //   [26:26] AVX512PF instructions support
    //   [27:27] AVX512ER instructions support
    // * [28:28] AVX512CD instructions support
    //   [29:29] SHA instructions support
    // * [30:30] AVX512BW instructions support
    // * [31:31] AVX512VL variable vector length support
    leaf->ebx = get_std_cpuid_leaf_7_ebx(BX_CPUID_STD7_SUBLEAF0_EBX_ENCHANCED_REP_STRINGS);
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

// leaf 0x00000008 reserved                          //
// leaf 0x00000009 direct cache access not supported //

// leaf 0x0000000A //
void corei7_skylake_x_t::get_std_cpuid_leaf_A(cpuid_function_t *leaf) const
{
  // CPUID function 0x0000000A - Architectural Performance Monitoring Leaf

  // EAX:
  //   [7:0] Version ID of architectural performance monitoring
  //  [15:8] Number of general-purpose performance monitoring counters per logical processor
  // [23:16] Bit width of general-purpose, performance monitoring counter
  // [31:24] Length of EBX bit vector to enumerate architectural performance
  //         monitoring events.

  // EBX:
  //     [0] Core cycle event not available if 1
  //     [1] Instruction retired event not available if 1
  //     [2] Reference cycles event not available if 1
  //     [3] Last-level cache reference event not available if 1
  //     [4] Last-level cache misses event not available if 1
  //     [5] Branch instruction retired event not available if 1
  //     [6] Branch mispredict retired event not available if 1
  //  [31:7] reserved

  // ECX: reserved

  // EDX:
  //   [4:0] Number of fixed performance counters (if Version ID > 1)
  //  [12:5] Bit width of fixed-function performance counters (if Version ID > 1)
  // [31:13] reserved

  leaf->eax = 0x07300404;
  leaf->ebx = 0x00000000;
  leaf->ecx = 0x00000000;
  leaf->edx = 0x00000603;

  BX_INFO(("WARNING: Architectural Performance Monitoring is not implemented"));
}

// leaf 0x0000000C reserved //

// leaf 0x0000000D - XSAVE //

// leaf 0x0000000E reserved //
// leaf 0x0000000F Intel Resource Director Technology (Intel RDT) Monitoring Enumeration (not implemented) //
// leaf 0x00000010 Intel Resource Director Technology (Intel RDT) Allocation Enumeration (not implemented) //
// leaf 0x00000011 reserved //
// leaf 0x00000012 SGX (not implemented) //
// leaf 0x00000013 reserved //
// leaf 0x00000014 Intel Processor Trace (not implemented) //
// leaf 0x00000015 Time Stamp Counter and Nominal Core Crystal Clock Information //
// leaf 0x00000016 Processor Frequency Information //

// leaf 0x80000000 //

// leaf 0x80000001 //
void corei7_skylake_x_t::get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information (reserved for Intel)
  leaf->eax = 0;

  // EBX:       Brand ID (reserved for Intel)
  leaf->ebx = 0;

  // ECX:
  // * [0:0]   LAHF/SAHF instructions support in 64-bit mode
  //   [1:1]   CMP_Legacy: Core multi-processing legacy mode (AMD)
  //   [2:2]   SVM: Secure Virtual Machine (AMD)
  //   [3:3]   Extended APIC Space
  //   [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
  // * [5:5]   LZCNT: LZCNT instruction support
  //   [6:6]   SSE4A: SSE4A Instructions support
  //   [7:7]   Misaligned SSE support
  // * [8:8]   PREFETCHW: PREFETCHW instruction support
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
  leaf->ecx = get_ext_cpuid_leaf_1_ecx_intel(BX_CPUID_EXT1_ECX_PREFETCHW);

  // EDX:
  //    [10:0] Reserved for Intel
  // * [11:11] SYSCALL/SYSRET support
  //   [19:12] Reserved for Intel
  // * [20:20] No-Execute page protection
  //   [25:21] Reserved
  // * [26:26] 1G paging support
  // * [27:27] Support RDTSCP Instruction
  //   [28:28] Reserved
  // * [29:29] Long Mode
  //   [30:30] AMD 3DNow! Extensions
  //   [31:31] AMD 3DNow! Instructions
  leaf->edx = get_ext_cpuid_leaf_1_edx_intel();
}

// leaf 0x80000002 //
// leaf 0x80000003 //
// leaf 0x80000004 //
// leaf 0x80000005 - L1 Cache and TLB Identifiers (reserved for Intel)
// leaf 0x80000006 - L2 Cache and TLB Identifiers
// leaf 0x80000007 - Advanced Power Management

void corei7_skylake_x_t::dump_cpuid(void) const
{
  bx_cpuid_t::dump_cpuid(max_std_leaf, max_ext_leaf);
}

bx_cpuid_t *create_corei7_skylake_x_cpuid(BX_CPU_C *cpu) { return new corei7_skylake_x_t(cpu); }

#endif
