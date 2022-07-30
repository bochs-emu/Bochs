/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2010-2020 Stanislav Shwartsman
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

class VMCS_Mapping;

class bx_cpuid_t {
public:
  bx_cpuid_t(BX_CPU_C *_cpu);
#if BX_SUPPORT_VMX
  bx_cpuid_t(BX_CPU_C *_cpu, Bit32u vmcs_revision);
  bx_cpuid_t(BX_CPU_C *_cpu, Bit32u vmcs_revision, const char *filename);
#endif
  virtual ~bx_cpuid_t() {}

  void init();

  // return CPU name
  virtual const char *get_name(void) const = 0;

  BX_CPP_INLINE void get_cpu_extensions(Bit32u *extensions) const {
    for (unsigned n=0; n < BX_ISA_EXTENSIONS_ARRAY_SIZE; n++)
       extensions[n] = ia_extensions_bitmask[n];
  }

  BX_CPP_INLINE bool is_cpu_extension_supported(unsigned extension) const {
    assert(extension < BX_ISA_EXTENSION_LAST);
    return ia_extensions_bitmask[extension / 32] & (1 << (extension % 32));
  }

#if BX_SUPPORT_VMX
  virtual Bit32u get_vmx_extensions_bitmask(void) const { return 0; }
#endif
#if BX_SUPPORT_SVM
  virtual Bit32u get_svm_extensions_bitmask(void) const { return 0; }
#endif

  virtual void get_cpuid_leaf(Bit32u function, Bit32u subfunction, cpuid_function_t *leaf) const = 0;

  virtual void dump_cpuid(void) const = 0;

  void dump_features() const;

#if BX_CPU_LEVEL >= 5
  virtual int rdmsr(Bit32u index, Bit64u *msr) { return -1; }
  virtual int wrmsr(Bit32u index, Bit64u  msr) { return -1; }
#endif

#if BX_SUPPORT_VMX
  VMCS_Mapping* get_vmcs() { return &vmcs_map; }
#endif

protected:
  BX_CPU_C *cpu;

  unsigned nprocessors;
  unsigned ncores;
  unsigned nthreads;

  Bit32u ia_extensions_bitmask[BX_ISA_EXTENSIONS_ARRAY_SIZE];

  BX_CPP_INLINE void enable_cpu_extension(unsigned extension) {
    assert(extension < BX_ISA_EXTENSION_LAST);
    ia_extensions_bitmask[extension / 32] |=  (1 << (extension % 32));
    warning_messages(extension);
  }

  BX_CPP_INLINE void disable_cpu_extension(unsigned extension) {
    assert(extension < BX_ISA_EXTENSION_LAST);
    ia_extensions_bitmask[extension / 32] &= ~(1 << (extension % 32));
  }

  void get_leaf_0(unsigned max_leaf, const char *vendor_string, cpuid_function_t *leaf, unsigned limited_max_leaf = 0x02) const;
  void get_ext_cpuid_brand_string_leaf(const char *brand_string, Bit32u function, cpuid_function_t *leaf) const;
  void get_cpuid_hidden_level(cpuid_function_t *leaf, const char *magic_string) const;

#if BX_SUPPORT_APIC
  void get_std_cpuid_extended_topology_leaf(Bit32u subfunction, cpuid_function_t *leaf) const;
#endif

#if BX_CPU_LEVEL >= 6
  void get_std_cpuid_xsave_leaf(Bit32u subfunction, cpuid_function_t *leaf) const;
#endif

  Bit32u get_std_cpuid_leaf_7_ebx(Bit32u extra = 0) const;
  Bit32u get_std_cpuid_leaf_7_ecx(Bit32u extra = 0) const;

  void get_ext_cpuid_leaf_8(cpuid_function_t *leaf) const;

  BX_CPP_INLINE void get_leaf(cpuid_function_t *leaf, Bit32u eax, Bit32u ebx, Bit32u ecx, Bit32u edx)
  {
    leaf->eax = eax;
    leaf->ebx = ebx;
    leaf->ecx = ecx;
    leaf->edx = edx;
  }

  BX_CPP_INLINE void get_reserved_leaf(cpuid_function_t *leaf) const
  {
    leaf->eax = 0;
    leaf->ebx = 0;
    leaf->ecx = 0;
    leaf->edx = 0;
  }

  void dump_cpuid_leaf(unsigned function, unsigned subfunction = 0) const;
  void dump_cpuid(unsigned max_std_leaf, unsigned max_ext_leaf) const;

  void warning_messages(unsigned extension) const;

#if BX_SUPPORT_VMX
  VMCS_Mapping vmcs_map;
#endif
};

extern const char *get_cpu_feature_name(unsigned feature);

typedef bx_cpuid_t* (*bx_create_cpuid_method)(BX_CPU_C *cpu);

// cpuid VMX features
#define BX_VMX_TPR_SHADOW                       (1 <<  0)   /* TPR shadow */
#define BX_VMX_VIRTUAL_NMI                      (1 <<  1)   /* Virtual NMI */
#define BX_VMX_APIC_VIRTUALIZATION              (1 <<  2)   /* APIC Access Virtualization */
#define BX_VMX_WBINVD_VMEXIT                    (1 <<  3)   /* WBINVD VMEXIT */
#define BX_VMX_PERF_GLOBAL_CTRL                 (1 <<  4)   /* Save/Restore MSR_PERF_GLOBAL_CTRL */
#define BX_VMX_MONITOR_TRAP_FLAG                (1 <<  5)   /* Monitor trap Flag (MTF) */
#define BX_VMX_X2APIC_VIRTUALIZATION            (1 <<  6)   /* Virtualize X2APIC */
#define BX_VMX_EPT                              (1 <<  7)   /* Extended Page Tables (EPT) */
#define BX_VMX_VPID                             (1 <<  8)   /* VPID */
#define BX_VMX_UNRESTRICTED_GUEST               (1 <<  9)   /* Unrestricted Guest */
#define BX_VMX_PREEMPTION_TIMER                 (1 << 10)   /* VMX preemption timer */
#define BX_VMX_SAVE_DEBUGCTL_DISABLE            (1 << 11)   /* Disable Save/Restore of MSR_DEBUGCTL */
#define BX_VMX_PAT                              (1 << 12)   /* Save/Restore MSR_PAT */
#define BX_VMX_EFER                             (1 << 13)   /* Save/Restore MSR_EFER */
#define BX_VMX_DESCRIPTOR_TABLE_EXIT            (1 << 14)   /* Descriptor Table VMEXIT */
#define BX_VMX_PAUSE_LOOP_EXITING               (1 << 15)   /* Pause Loop Exiting */
#define BX_VMX_EPTP_SWITCHING                   (1 << 16)   /* EPTP switching (VM Function 0) */
#define BX_VMX_EPT_ACCESS_DIRTY                 (1 << 17)   /* Extended Page Tables (EPT) A/D Bits */
#define BX_VMX_VINTR_DELIVERY                   (1 << 18)   /* Virtual Interrupt Delivery */
#define BX_VMX_POSTED_INSTERRUPTS               (1 << 19)   /* Posted Interrupts support - not implemented yet */
#define BX_VMX_VMCS_SHADOWING                   (1 << 20)   /* VMCS Shadowing */
#define BX_VMX_EPT_EXCEPTION                    (1 << 21)   /* EPT Violation (#VE) exception */
#define BX_VMX_PML                              (1 << 22)   /* Page Modification Logging */
#define BX_VMX_SPP                              (1 << 23)   /* Sub Page Protection */
#define BX_VMX_TSC_SCALING                      (1 << 24)   /* TSC Scaling */
#define BX_VMX_SW_INTERRUPT_INJECTION_ILEN_0    (1 << 25)   /* Allow software interrupt injection with instruction length 0 */
#define BX_VMX_MBE_CONTROL                      (1 << 26)   /* Mode-Based Execution Control (XU/XS) */

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
// [11:11] IA32_DEBUG_INTERFACE MSR for silicon debug support
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
// [29:29] AVX F16C - Float16 conversion support
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
#define BX_CPUID_EXT_DEBUG_INTERFACE         (1 << 11)
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
#define BX_CPUID_EXT_AVX_F16C                (1 << 29)
#define BX_CPUID_EXT_RDRAND                  (1 << 30)
#define BX_CPUID_EXT_RESERVED31              (1 << 31)

// CPUID defines - EXT3 features CPUID[0x00000007].EBX
// -----------------------------

//   [0:0]    FS/GS BASE access instructions
//   [1:1]    Support for IA32_TSC_ADJUST MSR
//   [2:2]    SGX: Intel Software Guard Extensions
//   [3:3]    BMI1: Advanced Bit Manipulation Extensions
//   [4:4]    HLE: Hardware Lock Elision
//   [5:5]    AVX2
//   [6:6]    FDP Deprecation
//   [7:7]    SMEP: Supervisor Mode Execution Protection
//   [8:8]    BMI2: Advanced Bit Manipulation Extensions
//   [9:9]    Support for Enhanced REP MOVSB/STOSB
//   [10:10]  Support for INVPCID instruction
//   [11:11]  RTM: Restricted Transactional Memory
//   [12:12]  Supports Platform Quality of Service Monitoring (PQM) capability
//   [13:13]  Deprecates FPU CS and FPU DS values
//   [14:14]  Intel Memory Protection Extensions
//   [15:15]  Supports Platform Quality of Service Enforcement (PQE) capability
//   [16:16]  AVX512F instructions support
//   [17:17]  AVX512DQ instructions support
//   [18:18]  RDSEED instruction support
//   [19:19]  ADCX/ADOX instructions support
//   [20:20]  SMAP: Supervisor Mode Access Prevention
//   [21:21]  AVX512IFMA52 instructions support
//   [22:22]  Reserved
//   [23:23]  CLFLUSHOPT instruction
//   [24:24]  CLWB instruction
//   [25:25]  Intel Processor Trace
//   [26:26]  AVX512PF instructions support
//   [27:27]  AVX512ER instructions support
//   [28:28]  AVX512CD instructions support
//   [29:29]  SHA instructions support
//   [30:30]  AVX512BW instructions support
//   [31:31]  AVX512VL variable vector length support

#define BX_CPUID_EXT3_FSGSBASE               (1 <<  0)
#define BX_CPUID_EXT3_TSC_ADJUST             (1 <<  1)
#define BX_CPUID_EXT3_SGX                    (1 <<  2)
#define BX_CPUID_EXT3_BMI1                   (1 <<  3)
#define BX_CPUID_EXT3_HLE                    (1 <<  4)
#define BX_CPUID_EXT3_AVX2                   (1 <<  5)
#define BX_CPUID_EXT3_FDP_DEPRECATION        (1 <<  6)
#define BX_CPUID_EXT3_SMEP                   (1 <<  7)
#define BX_CPUID_EXT3_BMI2                   (1 <<  8)
#define BX_CPUID_EXT3_ENCHANCED_REP_STRINGS  (1 <<  9)
#define BX_CPUID_EXT3_INVPCID                (1 << 10)
#define BX_CPUID_EXT3_RTM                    (1 << 11)
#define BX_CPUID_EXT3_QOS_MONITORING         (1 << 12)
#define BX_CPUID_EXT3_DEPRECATE_FCS_FDS      (1 << 13)
#define BX_CPUID_EXT3_MPX                    (1 << 14)
#define BX_CPUID_EXT3_QOS_ENFORCEMENT        (1 << 15)
#define BX_CPUID_EXT3_AVX512F                (1 << 16)
#define BX_CPUID_EXT3_AVX512DQ               (1 << 17)
#define BX_CPUID_EXT3_RDSEED                 (1 << 18)
#define BX_CPUID_EXT3_ADX                    (1 << 19)
#define BX_CPUID_EXT3_SMAP                   (1 << 20)
#define BX_CPUID_EXT3_AVX512IFMA52           (1 << 21)
#define BX_CPUID_EXT3_RESERVED22             (1 << 22)
#define BX_CPUID_EXT3_CLFLUSHOPT             (1 << 23)
#define BX_CPUID_EXT3_CLWB                   (1 << 24)
#define BX_CPUID_EXT3_PROCESSOR_TRACE        (1 << 25)
#define BX_CPUID_EXT3_AVX512PF               (1 << 26)
#define BX_CPUID_EXT3_AVX512ER               (1 << 27)
#define BX_CPUID_EXT3_AVX512CD               (1 << 28)
#define BX_CPUID_EXT3_SHA                    (1 << 29)
#define BX_CPUID_EXT3_AVX512BW               (1 << 30)
#define BX_CPUID_EXT3_AVX512VL               (1 << 31)

// CPUID defines - EXT4 features CPUID[0x00000007].ECX
// -----------------------------

//   [0:0]    PREFETCHWT1 instruction support
//   [1:1]    AVX512 VBMI instructions support
//   [2:2]    UMIP: Supports user-mode instruction prevention
//   [3:3]    PKU: Protection keys for user-mode pages.
//   [4:4]    OSPKE: OS has set CR4.PKE to enable protection keys
//   [5:5]    WAITPKG (TPAUSE/UMONITOR/UMWAIT) support
//   [6:6]    AVX512 VBMI2 instructions support
//   [7:7]    CET_SS: Support CET Shadow Stack
//   [8:8]    GFNI instructions support
//   [9:9]    VAES instructions support
// [10:10]    VPCLMULQDQ instruction support
// [11:11]    AVX512 VNNI instructions support
// [12:12]    AVX512 BITALG instructions support
// [13:13]    TME_EN: indicates support for the following MSRs: IA32_TME_CAPABILITY, IA32_TME_ACTIVATE, IA32_TME_EXCLUDE_MASK, and IA32_TME_EXCLUDE_BASE
// [14:14]    AVX512 VPOPCNTDQ: AVX512 VPOPCNTD/VPOPCNTQ instructions
// [15:15]    reserved
// [16:16]    LA57: LA57 and 5-level paging
// [21:17]    reserved
// [22:22]    RDPID: Read Processor ID support
// [23:23]    Keylocker
// [24:24]    BUS_LOCK_DETECT: Indicates support for bus lock debug exceptions
// [25:25]    CLDEMOTE: CLDEMOTE instruction support
// [26:26]    reserved
// [27:27]    MOVDIRI: MOVDIRI instruction support
// [28:28]    MOVDIR64B: MOVDIR64B instruction support
// [29:29]    ENQCMD: Enqueue Stores support
// [30:30]    SGX_LC: SGX Launch Configuration
// [31:31]    PKS: Protection keys for supervisor-mode pages

#define BX_CPUID_EXT4_PREFETCHWT1            (1 <<  0)
#define BX_CPUID_EXT4_AVX512_VBMI            (1 <<  1)
#define BX_CPUID_EXT4_UMIP                   (1 <<  2)
#define BX_CPUID_EXT4_PKU                    (1 <<  3)
#define BX_CPUID_EXT4_OSPKE                  (1 <<  4)
#define BX_CPUID_EXT4_WAITPKG                (1 <<  5)
#define BX_CPUID_EXT4_AVX512_VBMI2           (1 <<  6)
#define BX_CPUID_EXT4_CET_SS                 (1 <<  7)
#define BX_CPUID_EXT4_GFNI                   (1 <<  8)
#define BX_CPUID_EXT4_VAES                   (1 <<  9)
#define BX_CPUID_EXT4_VPCLMULQDQ             (1 << 10)
#define BX_CPUID_EXT4_AVX512_VNNI            (1 << 11)
#define BX_CPUID_EXT4_AVX512_BITALG          (1 << 12)
#define BX_CPUID_EXT4_TME                    (1 << 13)
#define BX_CPUID_EXT4_AVX512_VPOPCNTDQ       (1 << 14)
#define BX_CPUID_EXT4_RESERVED15             (1 << 15)
#define BX_CPUID_EXT4_LA57                   (1 << 16)
#define BX_CPUID_EXT4_RESERVED17             (1 << 17)
#define BX_CPUID_EXT4_RESERVED18             (1 << 18)
#define BX_CPUID_EXT4_RESERVED19             (1 << 19)
#define BX_CPUID_EXT4_RESERVED20             (1 << 20)
#define BX_CPUID_EXT4_RESERVED21             (1 << 21)
#define BX_CPUID_EXT4_RDPID                  (1 << 22)
#define BX_CPUID_EXT4_KEYLOCKER              (1 << 23)
#define BX_CPUID_EXT4_BUS_LOCK_DETECT        (1 << 24)
#define BX_CPUID_EXT4_CLDEMOTE               (1 << 25)
#define BX_CPUID_EXT4_RESERVED26             (1 << 26)
#define BX_CPUID_EXT4_MOVDIRI                (1 << 27)
#define BX_CPUID_EXT4_MOVDIR64B              (1 << 28)
#define BX_CPUID_EXT4_ENQCMD                 (1 << 29)
#define BX_CPUID_EXT4_SGX_LAUNCH_CONFIG      (1 << 30)
#define BX_CPUID_EXT4_PKS                    (1 << 31)

// CPUID defines - EXT5 features CPUID[0x00000007].EDX
// -----------------------------
//   [1:0]    reserved
//   [2:2]    AVX512 4VNNIW instructions support
//   [3:3]    AVX512 4FMAPS instructions support
//   [4:4]    Support of Fast REP MOV instructions with short length
//   [5:5]    UINTR: User interrupts support
//   [7:6]    reserved
//   [8:8]    AVX512 VP2INTERSECT instructions support
//   [9:9]    reserved
//   [10:10]  MD clear
//   [13:11]  reserved
//   [14:14]  SERIALIZE instruction support
//   [15:15]  Hybrid
//   [16:16]  TSXLDTRK: TSX suspent load tracking support
//   [17:17]  reserved
//   [18:18]  PCONFIG 
//   [19:19]  Architectural LBRs support
//   [20:20]  CET IBT: Support CET indirect branch tracking
//   [21:21]  reserved
//   [22:22]  AMX BF16 support
//   [23:23]  AVX512_FP16 instructions support
//   [24:24]  AMX TILE architecture support
//   [25:25]  AMX INT8 support
//   [26:26]  IBRS and IBPB: Indirect branch restricted speculation (SCA)
//   [27:27]  STIBP: Single Thread Indirect Branch Predictors supported (SCA)
//   [28:28]  L1D_FLUSH supported (SCA)
//   [29:29]  MSR_IA32_ARCH_CAPABILITIES supported (SCA)
//   [30:30]  MSR_IA32_CORE_CAPABILITIES supported (SCA)
//   [31:31]  SSBD: Speculative Store Bypass Disable supported (SCA)

#define BX_CPUID_EXT5_RESERVED0              (1 <<  0)
#define BX_CPUID_EXT5_RESERVED1              (1 <<  1)
#define BX_CPUID_EXT5_AVX512_4VNNIW          (1 <<  2)
#define BX_CPUID_EXT5_AVX512_4FMAPS          (1 <<  3)
#define BX_CPUID_EXT5_FAST_SHORT_REP_MOV     (1 <<  4)
#define BX_CPUID_EXT5_UINTR                  (1 <<  5)
#define BX_CPUID_EXT5_RESERVED6              (1 <<  6)
#define BX_CPUID_EXT5_RESERVED7              (1 <<  7)
#define BX_CPUID_EXT5_AVX512_VPINTERSECT     (1 <<  8)
#define BX_CPUID_EXT5_RESERVED9              (1 <<  9)
#define BX_CPUID_EXT5_MD_CLEAR               (1 << 10)
#define BX_CPUID_EXT5_RESERVED11             (1 << 11)
#define BX_CPUID_EXT5_RESERVED12             (1 << 12)
#define BX_CPUID_EXT5_RESERVED13             (1 << 13)
#define BX_CPUID_EXT5_SERIALIZE              (1 << 14)
#define BX_CPUID_EXT5_HYBRID                 (1 << 15)
#define BX_CPUID_EXT5_TSXLDTRK               (1 << 16)
#define BX_CPUID_EXT5_RESERVED17             (1 << 17)
#define BX_CPUID_EXT5_PCONFIG                (1 << 18)
#define BX_CPUID_EXT5_ARCH_LBR               (1 << 19)
#define BX_CPUID_EXT5_CET_IBT                (1 << 20)
#define BX_CPUID_EXT5_RESERVED21             (1 << 21)
#define BX_CPUID_EXT5_AMX_BF16               (1 << 22)
#define BX_CPUID_EXT5_AVX512_FP16            (1 << 23)
#define BX_CPUID_EXT5_AMX_TILE               (1 << 24)
#define BX_CPUID_EXT5_AMX_INT8               (1 << 25)
#define BX_CPUID_EXT5_SCA_IBRS_IBPB          (1 << 26)
#define BX_CPUID_EXT5_SCA_STIBP              (1 << 27)
#define BX_CPUID_EXT5_L1D_FLUSH              (1 << 28)
#define BX_CPUID_EXT5_ARCH_CAPABILITIES_MSR  (1 << 29)
#define BX_CPUID_EXT5_CORE_CAPABILITIES_MSR  (1 << 30)
#define BX_CPUID_EXT5_SCA_SSBD               (1 << 31)

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
// [17:17] Translation Cache Extensions (reserved?)
// [18:18] reserved
// [19:19] NodeId: Indicates support for NodeId MSR (0xc001100c)
// [20:20] reserved
// [21:21] TBM: trailing bit manipulation instruction support
// [22:22] Topology extensions support
// [23:23] PerfCtrExtCore: core performance counter extensions support
// [24:24] PerfCtrExtNB: NB performance counter extensions support
// [25:25] reserved
// [26:26] Data breakpoint extension. Indicates support for MSR 0xC0011027 and MSRs 0xC001101[B:9].
// [27:27] Performance time-stamp counter. Indicates support for MSR 0xC0010280.
// [28:28] PerfCtrExtL2I: L2I performance counter extensions support.
// [29:29] MONITORX/MWAITX support
// [30:30] AddrMaskExt: address mask extension support for instruction breakpoint
// [31:31] reserved

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
#define BX_CPUID_EXT2_TCE                    (1 << 17)
#define BX_CPUID_EXT2_RESERVED18             (1 << 18)
#define BX_CPUID_EXT2_NODEID                 (1 << 19)
#define BX_CPUID_EXT2_RESERVED20             (1 << 20)
#define BX_CPUID_EXT2_TBM                    (1 << 21)
#define BX_CPUID_EXT2_TOPOLOGY_EXTENSIONS    (1 << 22)
#define BX_CPUID_EXT2_PERFCTR_EXT_CORE       (1 << 23)
#define BX_CPUID_EXT2_PERFCTR_EXT_NB         (1 << 24)
#define BX_CPUID_EXT2_RESERVED25             (1 << 25)
#define BX_CPUID_EXT2_DATA_BREAKPOINT_EXT    (1 << 26)
#define BX_CPUID_EXT2_PERF_TSC               (1 << 27)
#define BX_CPUID_EXT2_PERFCTR_EXT_L2I        (1 << 28)
#define BX_CPUID_EXT2_MONITORX_MWAITX        (1 << 29)
#define BX_CPUID_EXT2_CODEBP_ADDRMASK_EXT    (1 << 30)
#define BX_CPUID_EXT2_RESERVED31             (1 << 31)

// CPUID defines - SVM features CPUID[0x8000000A].EDX
// ----------------------------

// [0:0]   NP - Nested paging support
// [1:1]   LBR virtualization
// [2:2]   SVM Lock
// [3:3]   NRIPS - Next RIP save on VMEXIT
// [4:4]   TscRate - MSR based TSC ratio control
// [5:5]   VMCB Clean bits support
// [6:6]   Flush by ASID support
// [7:7]   Decode assists support
// [8:8]   Reserved
// [9:9]   Reserved
// [10:10] Pause filter support
// [11:11] Reserved
// [12:12] Pause filter threshold support
// [13:13] Advanced Virtual Interrupt Controller
// [14:14] Reserved
// [15:15] Nested Virtualization (virtualized VMLOAD and VMSAVE) Support
// [16:16] Virtual GIF
// [17:17] Guest Mode Execute Trap (CMET)

#define BX_CPUID_SVM_NESTED_PAGING           (1 <<  0)
#define BX_CPUID_SVM_LBR_VIRTUALIZATION      (1 <<  1)
#define BX_CPUID_SVM_SVM_LOCK                (1 <<  2)
#define BX_CPUID_SVM_NRIP_SAVE               (1 <<  3)
#define BX_CPUID_SVM_TSCRATE                 (1 <<  4)
#define BX_CPUID_SVM_VMCB_CLEAN_BITS         (1 <<  5)
#define BX_CPUID_SVM_FLUSH_BY_ASID           (1 <<  6)
#define BX_CPUID_SVM_DECODE_ASSIST           (1 <<  7)
#define BX_CPUID_SVM_RESERVED8               (1 <<  8)
#define BX_CPUID_SVM_RESERVED9               (1 <<  9)
#define BX_CPUID_SVM_PAUSE_FILTER            (1 << 10)
#define BX_CPUID_SVM_RESERVED11              (1 << 11)
#define BX_CPUID_SVM_PAUSE_FILTER_THRESHOLD  (1 << 12)
#define BX_CPUID_SVM_AVIC                    (1 << 13)
#define BX_CPUID_SVM_RESERVED14              (1 << 14)
#define BX_CPUID_SVM_NESTED_VIRTUALIZATION   (1 << 15)
#define BX_CPUID_SVM_VIRTUAL_GIF             (1 << 16)
#define BX_CPUID_SVM_CMET                    (1 << 17)

#endif
