/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2010-2011 Stanislav Shwartsman
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

#ifndef BX_CPU_MODEL_SPECIFIC
#define BX_CPU_MODEL_SPECIFIC

struct cpuid_function_t {
  Bit32u eax;
  Bit32u ebx;
  Bit32u ecx;
  Bit32u edx;
};

// CPUID defines - STD features CPUID[0x00000001].EDX
// ----------------------------

// [0:0]   FPU on chip
// [1:1]   VME: Virtual-8086 Mode enhancements
// [2:2]   DE: Debug Extensions (I/O breakpoints)
// [3:3]   PSE: Page Size Extensions
// [4:4]   TSC: Time Stamp Counter
// [5:5]   MSR: RDMSR and WRMSR support
// [6:6]   PAE: Physical Address Extensions
// [7:7]   MCE: Machine Check Exception
// [8:8]   CXS: CMPXCHG8B instruction
// [9:9]   APIC: APIC on Chip
// [10:10] Reserved
// [11:11] SYSENTER/SYSEXIT support
// [12:12] MTRR: Memory Type Range Reg
// [13:13] PGE/PTE Global Bit
// [14:14] MCA: Machine Check Architecture
// [15:15] CMOV: Cond Mov/Cmp Instructions
// [16:16] PAT: Page Attribute Table
// [17:17] PSE-36: Physical Address Extensions
// [18:18] PSN: Processor Serial Number
// [19:19] CLFLUSH: CLFLUSH Instruction support
// [20:20] Reserved
// [21:21] DS: Debug Store
// [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
// [23:23] MMX Technology
// [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
// [25:25] SSE: SSE Extensions
// [26:26] SSE2: SSE2 Extensions
// [27:27] Self Snoop
// [28:28] Hyper Threading Technology
// [29:29] TM: Thermal Monitor
// [30:30] Reserved
// [31:31] PBE: Pending Break Enable

#define BX_CPUID_STD_X87                     (1 <<  0)
#define BX_CPUID_STD_VME                     (1 <<  1)
#define BX_CPUID_STD_DEBUG_EXTENSIONS        (1 <<  2)
#define BX_CPUID_STD_PSE                     (1 <<  3)
#define BX_CPUID_STD_TSC                     (1 <<  4)
#define BX_CPUID_STD_MSR                     (1 <<  5)
#define BX_CPUID_STD_PAE                     (1 <<  6)
#define BX_CPUID_STD_MCE                     (1 <<  7)
#define BX_CPUID_STD_CMPXCHG8B               (1 <<  8)
#define BX_CPUID_STD_APIC                    (1 <<  9)
#define BX_CPUID_STD_RESERVED10              (1 << 10)
#define BX_CPUID_STD_SYSENTER_SYSEXIT        (1 << 11)
#define BX_CPUID_STD_MTRR                    (1 << 12)
#define BX_CPUID_STD_GLOBAL_PAGES            (1 << 13)
#define BX_CPUID_STD_MCA                     (1 << 14)
#define BX_CPUID_STD_CMOV                    (1 << 15)
#define BX_CPUID_STD_PAT                     (1 << 16)
#define BX_CPUID_STD_PSE36                   (1 << 17)
#define BX_CPUID_STD_PROCESSOR_SERIAL_NUMBER (1 << 18)
#define BX_CPUID_STD_CLFLUSH                 (1 << 19)
#define BX_CPUID_STD_RESERVED20              (1 << 20)
#define BX_CPUID_STD_DEBUG_STORE             (1 << 21)
#define BX_CPUID_STD_ACPI                    (1 << 22)
#define BX_CPUID_STD_MMX                     (1 << 23)
#define BX_CPUID_STD_FXSAVE_FXRSTOR          (1 << 24)
#define BX_CPUID_STD_SSE                     (1 << 25)
#define BX_CPUID_STD_SSE2                    (1 << 26)
#define BX_CPUID_STD_SELF_SNOOP              (1 << 27)
#define BX_CPUID_STD_HT                      (1 << 28)
#define BX_CPUID_STD_THERMAL_MONITOR         (1 << 29)
#define BX_CPUID_STD_RESERVED30              (1 << 30)
#define BX_CPUID_STD_PBE                     (1 << 31)

// CPUID defines - EXT features CPUID[0x00000001].ECX
// ----------------------------

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

#define BX_CPUID_EXT_SSE3                    (1 <<  0)
#define BX_CPUID_EXT_PCLMULQDQ               (1 <<  1)
#define BX_CPUID_EXT_DTES64                  (1 <<  2)
#define BX_CPUID_EXT_MONITOR_MWAIT           (1 <<  3)
#define BX_CPUID_EXT_DS_CPL                  (1 <<  4)
#define BX_CPUID_EXT_VMX                     (1 <<  5)
#define BX_CPUID_EXT_SMX                     (1 <<  6)
#define BX_CPUID_EXT_EST                     (1 <<  7)
#define BX_CPUID_EXT_THERMAL_MONITOR2        (1 <<  8)
#define BX_CPUID_EXT_SSSE3                   (1 <<  9)
#define BX_CPUID_EXT_CNXT_ID                 (1 << 10)
#define BX_CPUID_EXT_RESERVED11              (1 << 11)
#define BX_CPUID_EXT_FMA                     (1 << 12)
#define BX_CPUID_EXT_CMPXCHG16B              (1 << 13)
#define BX_CPUID_EXT_xTPR                    (1 << 14)
#define BX_CPUID_EXT_PDCM                    (1 << 15)
#define BX_CPUID_EXT_RESERVED16              (1 << 16)
#define BX_CPUID_EXT_PCID                    (1 << 17)
#define BX_CPUID_EXT_DCA                     (1 << 18)
#define BX_CPUID_EXT_SSE4_1                  (1 << 19)
#define BX_CPUID_EXT_SSE4_2                  (1 << 20)
#define BX_CPUID_EXT_X2APIC                  (1 << 21)
#define BX_CPUID_EXT_MOVBE                   (1 << 22)
#define BX_CPUID_EXT_POPCNT                  (1 << 23)
#define BX_CPUID_EXT_TSC_DEADLINE            (1 << 24)
#define BX_CPUID_EXT_AES                     (1 << 25)
#define BX_CPUID_EXT_XSAVE                   (1 << 26)
#define BX_CPUID_EXT_OSXSAVE                 (1 << 27)
#define BX_CPUID_EXT_AVX                     (1 << 28)
#define BX_CPUID_EXT_F16C                    (1 << 29)
#define BX_CPUID_EXT_RDRAND                  (1 << 30)
#define BX_CPUID_EXT_RESERVED31              (1 << 31)

// CPUID defines - EXT3 features CPUID[0x00000007].EBX
// -----------------------------

//   [0:0]    FS/GS BASE access instructions
//   [6:1]    reserved
//   [7:7]    SMEP: Supervisor Mode Execution Protection
//   [8:8]    reserved
//   [9:9]    Support for Enhanced REP MOVSB/STOSB
//   [31:10]  reserved

#define BX_CPUID_EXT3_FSGSBASE               (1 <<  0)
#define BX_CPUID_EXT3_RESERVED1              (1 <<  1)
#define BX_CPUID_EXT3_RESERVED2              (1 <<  2)
#define BX_CPUID_EXT3_RESERVED3              (1 <<  3)
#define BX_CPUID_EXT3_RESERVED4              (1 <<  4)
#define BX_CPUID_EXT3_RESERVED5              (1 <<  5)
#define BX_CPUID_EXT3_RESERVED6              (1 <<  6)
#define BX_CPUID_EXT3_SMEP                   (1 <<  7)
#define BX_CPUID_EXT3_RESERVED8              (1 <<  8)
#define BX_CPUID_EXT3_ENCHANCED_REP_STRINGS  (1 <<  9)
// ...


// CPUID defines - STD2 features CPUID[0x80000001].EDX
// -----------------------------

// ...
#define BX_CPUID_STD2_SYSCALL_SYSRET         (1 << 11)
// ...
#define BX_CPUID_STD2_NX                     (1 << 20)
#define BX_CPUID_STD2_RESERVED21             (1 << 21)
#define BX_CPUID_STD2_AMD_MMX_EXT            (1 << 22)
#define BX_CPUID_STD2_RESERVED23             (1 << 23)
#define BX_CPUID_STD2_RESERVED24             (1 << 24)
#define BX_CPUID_STD2_FFXSR                  (1 << 25)
#define BX_CPUID_STD2_1G_PAGES               (1 << 26)
#define BX_CPUID_STD2_RDTSCP                 (1 << 27)
#define BX_CPUID_STD2_RESERVED28             (1 << 28)
#define BX_CPUID_STD2_LONG_MODE              (1 << 29)
#define BX_CPUID_STD2_3DNOW_EXT              (1 << 30)
#define BX_CPUID_STD2_3DNOW                  (1 << 31)

// CPUID defines - EXT2 features CPUID[0x80000001].ECX
// -----------------------------

// [0:0]   LAHF/SAHF instructions support in 64-bit mode
// [1:1]   CMP_Legacy: Core multi-processing legacy mode (AMD)
// [2:2]   SVM: Secure Virtual Machine (AMD)
// [3:3]   Extended APIC Space
// [4:4]   AltMovCR8: LOCK MOV CR0 means MOV CR8
// [5:5]   LZCNT: LZCNT instruction support
// [6:6]   SSE4A: SSE4A Instructions support
// [7:7]   Misaligned SSE support
// [8:8]   PREFETCHW: PREFETCHW instruction support
// [9:9]   OSVW: OS visible workarounds (AMD)
// [10:10] IBS: Instruction based sampling
// [11:11] XOP: Extended Operations Support and XOP Prefix
// [12:12] SKINIT support
// [13:13] WDT: Watchdog timer support
// [14:14] reserved
// [15:15] LWP: Light weight profiling
// [16:16] FMA4: Four-operand FMA instructions support
// [18:17] reserved
// [19:19] NodeId: Indicates support for NodeId MSR (0xc001100c)
// [20:20] reserved
// [21:21] TBM: trailing bit manipulation instruction support
// [22:22] Topology extensions support
// [31:23] reserved

#define BX_CPUID_EXT2_LAHF_SAHF              (1 <<  0)
#define BX_CPUID_EXT2_CMP_LEGACY             (1 <<  1)
#define BX_CPUID_EXT2_SVM                    (1 <<  2)
#define BX_CPUID_EXT2_EXT_APIC_SPACE         (1 <<  3)
#define BX_CPUID_EXT2_ALT_MOV_CR8            (1 <<  4)
#define BX_CPUID_EXT2_LZCNT                  (1 <<  5)
#define BX_CPUID_EXT2_SSE4A                  (1 <<  6)
#define BX_CPUID_EXT2_MISALIGNED_SSE         (1 <<  7)
#define BX_CPUID_EXT2_PREFETCHW              (1 <<  8)
#define BX_CPUID_EXT2_OSVW                   (1 <<  9)
#define BX_CPUID_EXT2_IBS                    (1 << 10)
#define BX_CPUID_EXT2_XOP                    (1 << 11)
#define BX_CPUID_EXT2_SKINIT                 (1 << 12)
#define BX_CPUID_EXT2_WDT                    (1 << 13)
#define BX_CPUID_EXT2_RESERVED14             (1 << 14)
#define BX_CPUID_EXT2_LWP                    (1 << 15)
#define BX_CPUID_EXT2_FMA4                   (1 << 16)
#define BX_CPUID_EXT2_RESERVED17             (1 << 17)
#define BX_CPUID_EXT2_RESERVED18             (1 << 18)
#define BX_CPUID_EXT2_NODEID                 (1 << 19)
#define BX_CPUID_EXT2_RESERVED20             (1 << 20)
#define BX_CPUID_EXT2_TBM                    (1 << 21)
#define BX_CPUID_EXT2_TOPOLOGY_EXTENSIONS    (1 << 22)

#endif
