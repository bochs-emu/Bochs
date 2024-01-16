/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023 Stanislav Shwartsman
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
#include "cpu/cpu.h"
#include "gui/siminterface.h"
#include "param_names.h"
#include "sapphire_rapids.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64 && BX_SUPPORT_AVX

#define VMCS_REVISION_ID 0x04

sapphire_rapids_t::sapphire_rapids_t(BX_CPU_C *cpu):
#if BX_SUPPORT_VMX
    bx_cpuid_t(cpu, VMCS_REVISION_ID)
#else
    bx_cpuid_t(cpu)
#endif
{
  if (BX_SUPPORT_VMX == 1)
    BX_INFO(("You must compile with --enable-vmx=2 for Intel Core Xeon Sapphire Rapids VMX configuration"));

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
  enable_cpu_extension(BX_ISA_AES_PCLMULQDQ);
  enable_cpu_extension(BX_ISA_MOVBE);
  enable_cpu_extension(BX_ISA_AVX);
  enable_cpu_extension(BX_ISA_AVX_F16C);
  enable_cpu_extension(BX_ISA_AVX2);
  enable_cpu_extension(BX_ISA_AVX_FMA);
  enable_cpu_extension(BX_ISA_AVX_VNNI);
  enable_cpu_extension(BX_ISA_LZCNT);
  enable_cpu_extension(BX_ISA_BMI1);
  enable_cpu_extension(BX_ISA_BMI2);
  enable_cpu_extension(BX_ISA_ADX);
  enable_cpu_extension(BX_ISA_FSGSBASE);
  enable_cpu_extension(BX_ISA_INVPCID);
  enable_cpu_extension(BX_ISA_SMEP);
  enable_cpu_extension(BX_ISA_SMAP);
  enable_cpu_extension(BX_ISA_RDRAND);
  enable_cpu_extension(BX_ISA_RDSEED);
  enable_cpu_extension(BX_ISA_FDP_DEPRECATION);
  enable_cpu_extension(BX_ISA_FCS_FDS_DEPRECATION);
  enable_cpu_extension(BX_ISA_SHA);
#if BX_SUPPORT_EVEX
  enable_cpu_extension(BX_ISA_GFNI);
  enable_cpu_extension(BX_ISA_VAES_VPCLMULQDQ);
  enable_cpu_extension(BX_ISA_AVX512);
  enable_cpu_extension(BX_ISA_AVX512_DQ);
  enable_cpu_extension(BX_ISA_AVX512_CD);
  enable_cpu_extension(BX_ISA_AVX512_BW);
  enable_cpu_extension(BX_ISA_AVX512_IFMA52);
  enable_cpu_extension(BX_ISA_AVX512_VBMI);
  enable_cpu_extension(BX_ISA_AVX512_VBMI2);
  enable_cpu_extension(BX_ISA_AVX512_VNNI);
  enable_cpu_extension(BX_ISA_AVX512_BITALG);
  enable_cpu_extension(BX_ISA_AVX512_VPOPCNTDQ);
  enable_cpu_extension(BX_ISA_AVX512_BF16);
#endif
#if BX_SUPPORT_AMX
  enable_cpu_extension(BX_ISA_AMX);
  enable_cpu_extension(BX_ISA_AMX_INT8);
  enable_cpu_extension(BX_ISA_AMX_BF16);
#endif
  enable_cpu_extension(BX_ISA_CLFLUSHOPT);
  enable_cpu_extension(BX_ISA_CLWB);
  enable_cpu_extension(BX_ISA_CLDEMOTE);
  enable_cpu_extension(BX_ISA_XSAVEC);
  enable_cpu_extension(BX_ISA_XSAVES);
#if BX_SUPPORT_PKEYS
  enable_cpu_extension(BX_ISA_PKU);
  enable_cpu_extension(BX_ISA_PKS);
#endif
#if BX_SUPPORT_CET
  enable_cpu_extension(BX_ISA_CET);
#endif
  enable_cpu_extension(BX_ISA_UMIP);
  enable_cpu_extension(BX_ISA_RDPID);
  enable_cpu_extension(BX_ISA_MOVDIRI);
  enable_cpu_extension(BX_ISA_MOVDIR64B);
  enable_cpu_extension(BX_ISA_LA57);
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_WAITPKG);
#endif
  enable_cpu_extension(BX_ISA_SERIALIZE);
#if BX_SUPPORT_UINTR
  enable_cpu_extension(BX_ISA_UINTR);
#endif
  enable_cpu_extension(BX_ISA_SCA_MITIGATIONS);

  max_std_leaf = 0x20;
  max_ext_leaf = 0x80000008;
}

void sapphire_rapids_t::get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  static const char* brand_string = "Intel(R) Xeon(R) w9-3475X\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
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
    get_leaf(leaf, 0x00000000, 0x00000000, 0x08007040, 0x00000000);
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
  case 0x00000002: // CPUID leaf 0x00000002 - Cache and TLB Descriptors
    get_leaf(leaf, 0x00feff01, 0x000000f0, 0x00000000, 0x00000000);
    return;
  case 0x00000003:
    get_reserved_leaf(leaf);
    return;
  case 0x00000004:
    get_std_cpuid_leaf_4(subfunction, leaf);
    return;
  case 0x00000005: // CPUID leaf 0x00000005 - MONITOR/MWAIT leaf
    get_std_cpuid_leaf_5(leaf);
    return;
  case 0x00000006: // CPUID leaf 0x00000006 - Thermal and Power Management Leaf
    get_leaf(leaf, 0x0045cef7, 0x00000002, 0x00000009, 0x00000000);
    return;
  case 0x00000007:
    get_std_cpuid_leaf_7(subfunction, leaf);
    return;
  case 0x00000008:
  case 0x00000009:
    get_reserved_leaf(leaf);
    return;
  case 0x0000000A: // CPUID leaf 0x0000000A - Architectural Performance Monitor
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
  case 0x0000000F: // CPUID leaf 0x0000000F - Intel Resource Director Technonology
  case 0x00000010: // CPUID leaf 0x00000010 - Intel Resource Director Technonology
  case 0x00000011:
  case 0x00000012: // SGX Capability
  case 0x00000013:
  case 0x00000014: // Processor Trace
    get_reserved_leaf(leaf);
    return;
  case 0x00000015: // CPUID leaf 0x00000015 - Time Stamp Counter and Core Crystal Clock Information Leaf
    get_leaf(leaf, 0x00000002, 0x000000b0, 0x017d7840, 0x00000000);
    return;
  case 0x00000016: // CPUID leaf 0x00000016 - Processor Frequency Information Leaf
    get_leaf(leaf, 0x00000898, 0x000012c0, 0x00000064, 0x00000000);
    return;
  case 0x00000017:
    get_reserved_leaf(leaf);
    return;
  case 0x00000018: // TLB Descriptors
    get_std_cpuid_leaf_18(subfunction, leaf);
    return;
  case 0x00000019: // CPUID leaf 0x00000019 - Keylocker Leaf
  case 0x0000001A: // CPUID leaf 0x0000001A - native Model ID Enumeration leaf (for Hybrid)
  case 0x0000001B: // PCONFIG Information
  case 0x0000001C: // CPUID leaf 0x0000001C - Last Branch Record (Architectural LBR) leaf
#if BX_SUPPORT_AMX
  case 0x0000001D: // AMX
    get_std_cpuid_amx_palette_info_leaf(subfunction, leaf);
    return;
  case 0x0000001E: // AMX: TMUL Information Main leaf
    get_std_cpuid_amx_tmul_leaf(subfunction, leaf);
    return;
#else
  case 0x0000001D: // AMX
  case 0x0000001E: // AMX: TMUL Information Main leaf
    get_reserved_leaf(leaf);
    return;
#endif
  case 0x0000001F: // V2 Extended Topology Enumberation leaf
    get_reserved_leaf(leaf); // until figured it out
    return;
  case 0x00000020: // HRESET Information
  default:
    get_reserved_leaf(leaf);
    return;
  }
}

#if BX_SUPPORT_VMX >= 2
Bit32u sapphire_rapids_t::get_vmx_extensions_bitmask(void) const
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
         BX_VMX_PERF_GLOBAL_CTRL |
         BX_VMX_PAT |
         BX_VMX_EFER |
         BX_VMX_DESCRIPTOR_TABLE_EXIT |
         BX_VMX_X2APIC_VIRTUALIZATION |
         BX_VMX_PREEMPTION_TIMER |
         BX_VMX_PAUSE_LOOP_EXITING |
         BX_VMX_EPT_ACCESS_DIRTY |
         BX_VMX_VINTR_DELIVERY |
         BX_VMX_VMCS_SHADOWING |
         BX_VMX_EPTP_SWITCHING |
         BX_VMX_EPT_EXCEPTION |
         BX_VMX_PML |
         BX_VMX_SW_INTERRUPT_INJECTION_ILEN_0 |
         BX_VMX_POSTED_INTERRUPTS |
         BX_VMX_MBE_CONTROL |
         BX_VMX_SPP |
         BX_VMX_TSC_SCALING |
         BX_VMX_SPEC_CTRL_VIRTUALIZATION;
}
#endif

// leaf 0x00000000 //

// leaf 0x00000001 //
void sapphire_rapids_t::get_std_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [19:16] Extended Model
  //   [27:20] Extended Family
  leaf->eax = 0x000806f8;

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
  // ! [6:6]   SMX: Secure Virtual Machine Technology
  // * [7:7]   EST: Enhanced Intel SpeedStep Technology
  // * [8:8]   TM2: Thermal Monitor 2
  // * [9:9]   SSSE3: SSSE3 Instructions
  //   [10:10] CNXT-ID: L1 context ID
  //   [11:11] reserved
  // * [12:12] FMA Instructions support
  // * [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  // * [14:14] xTPR update control
  // * [15:15] PDCM - Perfmon and Debug Capability MSR
  //   [16:16] reserved
  // * [17:17] PCID: Process Context Identifiers
  // * [18:18] DCA - Direct Cache Access
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
                                       BX_CPUID_STD1_ECX_PDCM |
                                       BX_CPUID_STD1_ECX_DCA);

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

// leaf 0x00000003 - Processor Serial Number (not supported) //

// leaf 0x00000004 //
void sapphire_rapids_t::get_std_cpuid_leaf_4(Bit32u subfunction, cpuid_function_t *leaf) const
{
  // CPUID function 0x00000004 - Deterministic Cache Parameters
  switch(subfunction) {
  case 0:
    leaf->eax = 0xfc004121;
    leaf->ebx = 0x02c0003f;
    leaf->ecx = 0x0000003f;
    leaf->edx = 0x00000000;
    return;
  case 1:
    leaf->eax = 0xfc004122;
    leaf->ebx = 0x01c0003f;
    leaf->ecx = 0x0000003f;
    leaf->edx = 0x00000000;
    return;
  case 2:
    leaf->eax = 0xfc004143;
    leaf->ebx = 0x03c0003f;
    leaf->ecx = 0x000007ff;
    leaf->edx = 0x00000000;
    return;
  case 3:
    leaf->eax = 0xfc1fc163;
    leaf->ebx = 0x0380003f;
    leaf->ecx = 0x00015fff;
    leaf->edx = 0x00000004;
    return;
  default:
    leaf->eax = 0;
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
  }
}

// leaf 0x00000005 - MONITOR/MWAIT Leaf //
void sapphire_rapids_t::get_std_cpuid_leaf_5(cpuid_function_t *leaf) const
{
  // CPUID function 0x00000005 - MONITOR/MWAIT Leaf

#if BX_SUPPORT_MONITOR_MWAIT
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
  leaf->edx = 0x00001020;
#else
  leaf->eax = 0;
  leaf->ebx = 0;
  leaf->ecx = 0;
  leaf->edx = 0;
#endif
}

// leaf 0x00000007 //
void sapphire_rapids_t::get_std_cpuid_leaf_7(Bit32u subfunction, cpuid_function_t *leaf) const
{
  switch(subfunction) {
  case 0:
    leaf->eax = 2; /* report max sub-leaf that supported in leaf 7 */

    // * [0:0]   FS/GS BASE access instructions
    // * [1:1]   Support for IA32_TSC_ADJUST MSR
    //   [2:2]   SGX: Intel Software Guard Extensions
    // * [3:3]   BMI1: Advanced Bit Manipulation Extensions
    // ! [4:4]   HLE: Hardware Lock Elision
    // * [5:5]   AVX2
    // * [6:6]   FDP Deprecation
    // * [7:7]   SMEP: Supervisor Mode Execution Protection
    // * [8:8]   BMI2: Advanced Bit Manipulation Extensions
    // * [9:9]   Support for Enhanced REP MOVSB/STOSB
    // * [10:10] Support for INVPCID instruction
    // ! [11:11] RTM: Restricted Transactional Memory
    // ! [12:12] Supports Quality of Service (QoS) capability
    // * [13:13] Deprecates FPU CS and FPU DS values
    //   [14:14] Intel Memory Protection Extensions
    // * [15:15] Supports Platform Quality of Service Enforcement (PQE) capability
    // * [16:16] AVX512F instructions support
    // * [17:17] AVX512DQ instructions support
    // * [18:18] RDSEED instruction support
    // * [19:19] ADCX/ADOX instructions support
    // * [20:20] SMAP: Supervisor Mode Access Prevention
    // * [21:21] AVX512IFMA52 instructions support
    //   [22:22] reserved
    // * [23:23] CLFLUSHOPT instruction
    // * [24:24] CLWB instruction
    // ! [25:25] Intel Processor Trace
    //   [26:26] AVX512PF instructions support
    //   [27:27] AVX512ER instructions support
    // * [28:28] AVX512CD instructions support
    // * [29:29] SHA instructions support
    // * [30:30] AVX512BW instructions support
    // * [31:31] AVX512VL variable vector length support
    leaf->ebx = get_std_cpuid_leaf_7_ebx(BX_CPUID_STD7_SUBLEAF0_EBX_ENCHANCED_REP_STRINGS);

    //   [0:0]   PREFETCHW1 instruction
    // * [1:1]   AVX512 VBMI instructions
    // * [2:2]   UMIP: Supports user-mode instruction prevention
    // * [3:3]   PKU: Protection keys for user-mode pages
    //   [4:4]   OSPKE: OS has set CR4.PKE to enable protection keys
    // * [5:5]   WAITPKG (TPAUSE/UMONITOR/UMWAIT) support
    // * [6:6]   AVX512 VBMI2 instructions support
    // * [7:7]   CET_SS: Support CET Shadow Stack
    // * [8:8]   GFNI instructions support
    // * [9:9]   VAES instructions support
    // * [10:10] VPCLMULQDQ instruction support
    // * [11:11] AVX512 VNNI instructions support
    // * [12:12] AVX512 BITALG instructions support
    // * [13:13] TME_EN: indicates support for MSRs: IA32_TME_CAPABILITY, IA32_TME_ACTIVATE, IA32_TME_EXCLUDE_MASK, and IA32_TME_EXCLUDE_BASE
    // * [14:14] AVX512 VPOPCNTDQ: AVX512 VPOPCNTD/VPOPCNTQ instructions
    //   [15:15] reserved
    // * [16:16] LA57: LA57 and 5-level paging
    //   [21:17] reserved
    // * [22:22] RDPID: Read Processor ID support
    //   [23:23] Keylocker support
    // ! [24:24] BUS_LOCK_DETECT: Indicates support for bus lock debug exceptions
    // * [25:25] CLDEMOTE: CLDEMOTE instruction support
    //   [26:26] reserved
    // * [27:27] MOVDIRI: MOVDIRI instruction support
    // * [28:28] MOVDIR64B: MOVDIR64B instruction support
    // ! [29:29] ENQCMD: Enqueue Stores support
    //   [30:30] SGX_LC: SGX Launch Configuration
    // * [31:31] PKS: Protection keys for supervisor-mode pages
    leaf->ecx = get_std_cpuid_leaf_7_ecx();

    //   [1:0]   reserved
    //   [2:2]   AVX512_4VNNIW support
    //   [3:3]   AVX512_4FMAPS support
    // * [4:4]   Fast Short REP MOV support
    // * [5:5]   UINTR: User interrupts support
    //   [7:6]   reserved
    //   [8:8]   AVX512_VP2INTERSECT support
    //   [9:9]   reserved
    // * [10:10] MD clear support
    //   [13:11] reserved
    // * [14:14] SERIALIZE instruction support
    //   [15:15] Hybrid processor
    // ! [16:16] TSXLDTRK: TSX suspent load tracking support
    //   [17:17] reserved
    // ! [18:18] PCONFIG support
    // ! [19:19] Architectural LBRs support
    // * [20:20] CET IBT: Support CET indirect branch tracking
    //   [21:21] reserved
    // * [22:22] AMX BF16 support
    // * [23:23] AVX512_FP16 instructions support
    // * [24:24] AMX TILE architecture support
    // * [25:25] AMX INT8 support
    // * [26:26] IBRS: indirect branch restricted speculation
    // * [27:27] STIBP: single thread indirect branch predictors
    // * [28:28] L1D_FLUSH support
    // * [29:29] Support for the IA32_ARCH_CAPABILITIES MSR
    // * [30:30] Support for the IA32_CORE_CAPABILITIES MSR
    // * [31:31] SSBD: Speculative Store Bypass Disable
    leaf->edx = get_std_cpuid_leaf_7_edx(BX_CPUID_STD7_SUBLEAF0_EDX_FAST_SHORT_REP_MOV);

    if (is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS))
      leaf->edx |= BX_CPUID_STD7_SUBLEAF0_EDX_MD_CLEAR |
                   BX_CPUID_STD7_SUBLEAF0_EDX_SCA_IBRS_IBPB |
                   BX_CPUID_STD7_SUBLEAF0_EDX_SCA_STIBP |
                   BX_CPUID_STD7_SUBLEAF0_EDX_L1D_FLUSH |
                   BX_CPUID_STD7_SUBLEAF0_EDX_ARCH_CAPABILITIES_MSR |
                   BX_CPUID_STD7_SUBLEAF0_EDX_CORE_CAPABILITIES_MSR |
                   BX_CPUID_STD7_SUBLEAF0_EDX_SCA_SSBD;
    break;

  case 1:
    //   [0:0]   SHA-512 instructions support
    //   [1:1]   SM3 instructions support
    //   [2:2]   SM4 instructions support
    //   [3:3]   RAO-INT
    // * [4:4]   AVX VNNI
    // * [5:5]   AVX512_BF16 conversion instructions support
    //   [6:6]   LASS: Linear Address Space Separation support
    //   [7:7]   CMPCCXADD
    //   [8:8]   Arch Perfmon
    //   [9:9]   Reserved
    // * [10:10] Fast zero-length REP MOVSB
    // * [11:11] Fast zero-length REP STOSB
    // * [12:12] Fast zero-length REP CMPSB/SCASB
    //   [18:13] Reserved
    //   [19:19] WRMSRNS instruction
    //   [20:20] Reserved
    //   [21:21] AMX-FP16 support
    //   [22:22] HRESET and CPUID leaf 0x20 support
    //   [23:23] AVX IFMA support
    //   [25:24] Reserved
    //   [26:26] LAM: Linear Address Masking
    //   [27:27] MSRLIST: RDMSRLIST/WRMSRLIST instructions and the IA32_BARRIER MSR
    //   [31:28] Reserved
    leaf->eax = get_std_cpuid_leaf_7_subleaf_1_eax(BX_CPUID_STD7_SUBLEAF1_EAX_FAST_ZEROLEN_REP_MOVSB |
                                                   BX_CPUID_STD7_SUBLEAF1_EAX_FAST_ZEROLEN_REP_STOSB |
                                                   BX_CPUID_STD7_SUBLEAF1_EAX_FAST_ZEROLEN_REP_CMPSB);
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
    break;

  case 2:
    leaf->eax = 0;
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
    if (is_cpu_extension_supported(BX_ISA_SCA_MITIGATIONS))
      leaf->edx = 0x17;
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
void sapphire_rapids_t::get_std_cpuid_leaf_A(cpuid_function_t *leaf) const
{
  // CPUID function 0x0000000A - Architectural Performance Monitoring Leaf
  leaf->eax = 0x08300805;
  leaf->ebx = 0x00000000;
  leaf->ecx = 0x0000000f;
  leaf->edx = 0x00008604;

  BX_INFO(("WARNING: Architectural Performance Monitoring is not implemented"));
}

// leaf 0x0000000C - reserved //
// leaf 0x0000000D - XSAVE //
// leaf 0x0000000E - reserved //
// leaf 0x0000000F - L3 Cache Intel RDT Monitoring Capability Enumeration //
// leaf 0x00000010 - Intel Resource Director Technology (Intel RDT) Allocation Enumeration //
// leaf 0x00000011 - reserved //
// leaf 0x00000012 - Intel Software Guard Extensions //
// leaf 0x00000013 - reserved //
// leaf 0x00000014 - Intel Processor Trace Enumeration //

// leaf 0x00000015 - Time Stamp Counter and Core Crystal Clock Information Leaf //
// leaf 0x00000016 - Processor Frequency Information Leaf
// leaf 0x00000017 - System-On-Chip Vendor Attribute Enumeration //

// leaf 0x00000018
void sapphire_rapids_t::get_std_cpuid_leaf_18(Bit32u subfunction, cpuid_function_t *leaf) const
{
  // CPUID function 0x00000018 - Deterministic Address Translation Parameters
  switch(subfunction) {
  case 0:
    leaf->eax = 0x00000008;
    leaf->ebx = 0x00000000;
    leaf->ecx = 0x00000000;
    leaf->edx = 0x00000000;
    return;
  case 1:
    leaf->eax = 0x00000000;
    leaf->ebx = 0x00080001;
    leaf->ecx = 0x00000020;
    leaf->edx = 0x00004022;
    return;
  case 2:
    leaf->eax = 0x00000000;
    leaf->ebx = 0x00080006;
    leaf->ecx = 0x00000004;
    leaf->edx = 0x00004022;
    return;
  case 3:
    leaf->eax = 0x00000000;
    leaf->ebx = 0x0010000f;
    leaf->ecx = 0x00000001;
    leaf->edx = 0x00004125;
    return;
  case 4:
    leaf->eax = 0x00000000;
    leaf->ebx = 0x00040001;
    leaf->ecx = 0x00000010;
    leaf->edx = 0x00004024;
    return;
  case 5:
    leaf->eax = 0x00000000;
    leaf->ebx = 0x00040006;
    leaf->ecx = 0x00000008;
    leaf->edx = 0x00004024;
    return;
  case 6:
    leaf->eax = 0x00000000;
    leaf->ebx = 0x00080008;
    leaf->ecx = 0x00000001;
    leaf->edx = 0x00004124;
    return;
  case 7:
    leaf->eax = 0x00000000;
    leaf->ebx = 0x00080007;
    leaf->ecx = 0x00000080;
    leaf->edx = 0x00004043;
    return;
  case 8:
    leaf->eax = 0x00000000;
    leaf->ebx = 0x00080009;
    leaf->ecx = 0x00000080;
    leaf->edx = 0x00004043;
    return;
  default:
    leaf->eax = 0;
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
  }
}

// leaf 0x80000000 //

// leaf 0x80000001 //
void sapphire_rapids_t::get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const
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
  //   [6:6]   SSE4A: SSE4A Instructions support (deprecated?)
  //   [7:7]   Misaligned SSE support
  // * [8:8]   PREFETCHW: PREFETCHW instruction support
  //   [9:9]   OSVW: OS visible workarounds (AMD)
  //   [11:10] reserved
  //   [12:12] SKINIT support
  //   [13:13] WDT: Watchdog timer support
  //   [31:14] reserved
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
// leaf 0x80000005 - L1 Cache and TLB Identifiers (reserved for Intel) //
// leaf 0x80000006 //
// leaf 0x80000007 - Advanced Power Management //
// leaf 0x80000008 //

void sapphire_rapids_t::dump_cpuid(void) const
{
  bx_cpuid_t::dump_cpuid(max_std_leaf, max_ext_leaf);
}

bx_cpuid_t *create_sapphire_rapids_cpuid(BX_CPU_C *cpu) { return new sapphire_rapids_t(cpu); }

#endif
