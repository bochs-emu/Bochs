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
#include "gui/siminterface.h"
#include "param_names.h"
#include "core2_penryn_t9600.h"

#define LOG_THIS cpu->

#if BX_SUPPORT_X86_64

// CPUID dump of the real CPU (taken from core2_penryn_t9600.txt), several rows of the same leaf are its subleafs
static const cpuid_function_t std_leafs[] = {
  /* leaf 0x00000000 */ { 0x0000000D, 0x756E6547, 0x6C65746E, 0x49656E69 },
  /* leaf 0x00000001 */ { 0x0001067A, 0x00020800, 0x0C08E3FD, 0xBFEBFBFF },
  /* leaf 0x00000002 */ { 0x05B0B101, 0x005657F0, 0x00000000, 0x2CB4304E },
  /* leaf 0x00000003 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000004 */ { 0x04000121, 0x01C0003F, 0x0000003F, 0x00000001 }, // [SL 00]
  /* leaf 0x00000004 */ { 0x04000122, 0x01C0003F, 0x0000003F, 0x00000001 }, // [SL 01]
  /* leaf 0x00000004 */ { 0x04004143, 0x05C0003F, 0x00000FFF, 0x00000001 }, // [SL 02]
  /* leaf 0x00000005 */ { 0x00000040, 0x00000040, 0x00000003, 0x03122220 },
  /* leaf 0x00000006 */ { 0x00000003, 0x00000002, 0x00000003, 0x00000000 },
  /* leaf 0x00000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000008 */ { 0x00000400, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x00000009 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000A */ { 0x07280202, 0x00000000, 0x00000000, 0x00000503 },
  /* leaf 0x0000000B */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, // not in dump
  /* leaf 0x0000000C */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x0000000D */ { 0x00000003, 0x00000240, 0x00000240, 0x00000000 }, // [SL 00]
  /* leaf 0x0000000D */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }  // [SL 01]
};

static const cpuid_function_t ext_leafs[] = {
  /* leaf 0x80000000 */ { 0x80000008, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000001 */ { 0x00000000, 0x00000000, 0x00000001, 0x20000800 },
  /* leaf 0x80000002 */ { 0x65746E49, 0x2952286C, 0x726F4320, 0x4D542865 },
  /* leaf 0x80000003 */ { 0x44203229, 0x43206F75, 0x20205550, 0x54202020 },
  /* leaf 0x80000004 */ { 0x30303639, 0x20402020, 0x30382E32, 0x007A4847 },
  /* leaf 0x80000005 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000006 */ { 0x00000000, 0x00000000, 0x18008040, 0x00000000 },
  /* leaf 0x80000007 */ { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  /* leaf 0x80000008 */ { 0x00003024, 0x00000000, 0x00000000, 0x00000000 }
};

core2_penryn_t9600_t::core2_penryn_t9600_t(BX_CPU_C *cpu): bx_cpuid_t(cpu)
{
  set_max_leaf(0xD, 0x80000008);

  cpuid_table = new bx_cpuid_table_t(max_std_leaf, max_ext_leaf);

  cpuid_table->register_leaf(0x00000000, 1, std_leafs +  0);
  cpuid_table->register_leaf(0x00000001, 1, std_leafs +  1);
  cpuid_table->register_leaf(0x00000002, 1, std_leafs +  2);
  cpuid_table->register_leaf(0x00000003, 1, std_leafs +  3);
  cpuid_table->register_leaf(0x00000004, 3, std_leafs +  4);
  cpuid_table->register_leaf(0x00000005, 1, std_leafs +  7);
  cpuid_table->register_leaf(0x00000006, 1, std_leafs +  8);
  cpuid_table->register_leaf(0x00000007, 1, std_leafs +  9);
  cpuid_table->register_leaf(0x00000008, 1, std_leafs + 10);
  cpuid_table->register_leaf(0x00000009, 1, std_leafs + 11);
  cpuid_table->register_leaf(0x0000000A, 1, std_leafs + 12);
  cpuid_table->register_leaf(0x0000000B, 1, std_leafs + 13);
  cpuid_table->register_leaf(0x0000000C, 1, std_leafs + 14);
  cpuid_table->register_leaf(0x0000000D, 2, std_leafs + 15);

  cpuid_table->register_leaf(0x80000000, 1, ext_leafs + 0);
  cpuid_table->register_leaf(0x80000001, 1, ext_leafs + 1);
  cpuid_table->register_leaf(0x80000002, 1, ext_leafs + 2);
  cpuid_table->register_leaf(0x80000003, 1, ext_leafs + 3);
  cpuid_table->register_leaf(0x80000004, 1, ext_leafs + 4);
  cpuid_table->register_leaf(0x80000005, 1, ext_leafs + 5);
  cpuid_table->register_leaf(0x80000006, 1, ext_leafs + 6);
  cpuid_table->register_leaf(0x80000007, 1, ext_leafs + 7);
  cpuid_table->register_leaf(0x80000008, 1, ext_leafs + 8);

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
#if BX_SUPPORT_MONITOR_MWAIT
  enable_cpu_extension(BX_ISA_MONITOR_MWAIT);
#endif
#if BX_SUPPORT_VMX
  enable_cpu_extension(BX_ISA_VMX);
#endif
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
  enable_cpu_extension(BX_ISA_XSAVE);
}

void core2_penryn_t9600_t::get_hardware_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const
{
  static const char* brand_string = "Intel(R) Core(TM)2 Duo CPU     T9600  @ 2.80GHz";

  static bool cpuid_limit_winnt = SIM->get_param_bool(BXPN_CPUID_LIMIT_WINNT)->get();
  if (cpuid_limit_winnt)
    if (function > 2 && function < 0x80000000) function = 2;

  switch(function) {
  case 0x80000000:
    get_leaf_0(max_ext_leaf, NULL, leaf);
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
  case 0x80000006: // CPUID leaf 0x80000006 - L2 Cache and TLB Identifiers
  case 0x80000007: // CPUID leaf 0x80000007 - Advanced Power Management
    cpuid_table->get_ext_leaf(function, 0, leaf);
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
  case 0x00000003:
    cpuid_table->get_std_leaf(function, 0, leaf);
    return;
  case 0x00000004: // CPUID leaf 0x00000004 - Deterministic Cache Parameters
    cpuid_table->get_std_leaf(function, subfunction, leaf);
    return;
  case 0x00000005:
    get_std_cpuid_monitor_mwait_leaf(leaf, 0x03122220);
    return;
  case 0x00000006: // CPUID leaf 0x00000006 - Thermal and Power Management Leaf
    cpuid_table->get_std_leaf(function, 0, leaf);
    return;
  case 0x00000007:
  case 0x00000008:
  case 0x00000009:
    get_reserved_leaf(leaf);
    return;
  case 0x0000000A: // CPUID leaf 0x0000000A - Architectural Performance Monitoring Leaf
  case 0x0000000B:
  case 0x0000000C:
    cpuid_table->get_std_leaf(function, 0, leaf);
    return;
  case 0x0000000D:
  default:
    get_std_cpuid_xsave_leaf(subfunction, leaf);
    return;
  }
}

#if BX_SUPPORT_VMX

//
// MSR 00000480 : 005A0800 0000000D	BX_MSR_VMX_BASIC
// MSR 00000481 : 0000003F 00000016	BX_MSR_VMX_PINBASED_CTRLS
// MSR 00000482 : F7F9FFFE 0401E172	BX_MSR_VMX_PROCBASED_CTRLS
// MSR 00000483 : 0003FFFF 00036DFF	BX_MSR_VMX_VMEXIT_CTRLS
// MSR 00000484 : 00003FFF 000011FF	BX_MSR_VMX_VMENTRY_CTRLS
// MSR 00000485 : 00000000 000403C0	BX_MSR_VMX_MISC
// MSR 00000486 : 00000000 80000021	BX_MSR_VMX_CR0_FIXED0
// MSR 00000487 : 00000000 FFFFFFFF	BX_MSR_VMX_CR0_FIXED1
// MSR 00000488 : 00000000 00002000	BX_MSR_VMX_CR4_FIXED0
// MSR 00000489 : 00000000 000467FF	BX_MSR_VMX_CR4_FIXED1
// MSR 0000048A : 00000000 0000002C	BX_MSR_VMX_VMCS_ENUM
// MSR 0000048B : 00000041 00000000	BX_MSR_VMX_PROCBASED_CTRLS2
//

Bit32u core2_penryn_t9600_t::get_vmx_extensions_bitmask(void) const
{
  return BX_VMX_TPR_SHADOW |
         BX_VMX_VIRTUAL_NMI |
         BX_VMX_PERF_GLOBAL_CTRL | /* not implemented */
         BX_VMX_APIC_VIRTUALIZATION |
         BX_VMX_WBINVD_VMEXIT;
}

#endif

// leaf 0x00000000 //

// leaf 0x00000001 //
void core2_penryn_t9600_t::get_std_cpuid_leaf_1(cpuid_function_t *leaf) const
{
  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [19:16] Extended Model
  //   [27:20] Extended Family
  leaf->eax = 0x0001067a;

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
  //   [1:1]   PCLMULQDQ Instruction support
  // * [2:2]   DTES64: 64-bit DS area
  // * [3:3]   MONITOR/MWAIT support
  // * [4:4]   DS-CPL: CPL qualified debug store
  // * [5:5]   VMX: Virtual Machine Technology
  // * [6:6]   SMX: Secure Virtual Machine Technology
  // * [7:7]   EST: Enhanced Intel SpeedStep Technology
  // * [8:8]   TM2: Thermal Monitor 2
  // * [9:9]   SSSE3: SSSE3 Instructions
  //   [10:10] CNXT-ID: L1 context ID
  //   [11:11] reserved
  //   [12:12] FMA Instructions support
  // * [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  // * [14:14] xTPR update control
  // * [15:15] PDCM - Perfmon and Debug Capability MSR
  //   [16:16] reserved
  //   [17:17] PCID: Process Context Identifiers
  //   [18:18] DCA - Direct Cache Access
  // * [19:19] SSE4.1 Instructions
  //   [20:20] SSE4.2 Instructions
  //   [21:21] X2APIC
  //   [22:22] MOVBE instruction
  //   [23:23] POPCNT instruction
  //   [24:24] TSC Deadline
  //   [25:25] AES Instructions
  // * [26:26] XSAVE extensions support
  // * [27:27] OSXSAVE support
  //   [28:28] AVX extensions support
  //   [29:29] AVX F16C - Float16 conversion support
  //   [30:30] RDRAND instruction
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
// leaf 0x00000003 - Processor Serial Number (not supported) //

// leaf 0x00000004 - Deterministic Cache Parameters //
// leaf 0x00000005 MONITOR/MWAIT Leaf                //
// leaf 0x00000006 Thermal and Power Management Leaf //
// leaf 0x00000007 not supported                     //
// leaf 0x00000008 reserved                          //
// leaf 0x00000009 direct cache access not supported //

// leaf 0x0000000A //

// leaf 0x0000000B not supported //
// leaf 0x0000000C reserved      //
// leaf 0x0000000D XSAVE         //

// leaf 0x80000000 //

// leaf 0x80000001 //
void core2_penryn_t9600_t::get_ext_cpuid_leaf_1(cpuid_function_t *leaf) const
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
  //   [5:5]   LZCNT: LZCNT instruction support
  //   [6:6]   SSE4A: SSE4A Instructions support
  //   [7:7]   Misaligned SSE support
  //   [8:8]   PREFETCHW: PREFETCHW instruction support
  //   [9:9]   OSVW: OS visible workarounds (AMD)
  //   [11:10] reserved
  //   [12:12] SKINIT support
  //   [13:13] WDT: Watchdog timer support
  //   [31:14] reserved
  leaf->ecx = get_ext_cpuid_leaf_1_ecx();

  // EDX:
  //    [10:0] Reserved for Intel
  // * [11:11] SYSCALL/SYSRET support
  //   [19:12] Reserved for Intel
  // * [20:20] No-Execute page protection
  //   [25:21] Reserved
  //   [26:26] 1G paging support
  //   [27:27] Support RDTSCP Instruction
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
// leaf 0x80000008 //

bx_cpuid_t *create_core2_penryn_t9600_cpuid(BX_CPU_C *cpu) { return new core2_penryn_t9600_t(cpu); }

#endif
