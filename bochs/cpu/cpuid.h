/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2010-2024 Stanislav Shwartsman
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

  bool is_empty() { return (eax | ebx | ecx | edx) == 0; }
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

#if BX_SUPPORT_AMX
  void get_std_cpuid_amx_palette_info_leaf(Bit32u subfunction, cpuid_function_t *leaf) const;
  void get_std_cpuid_amx_tmul_leaf(Bit32u subfunction, cpuid_function_t *leaf) const;
#endif

  Bit32u get_std_cpuid_leaf_1_ecx(Bit32u extra = 0) const;
  Bit32u get_std_cpuid_leaf_1_edx_common(Bit32u extra = 0) const;
  Bit32u get_std_cpuid_leaf_1_edx(Bit32u extra = 0) const;
  Bit32u get_std_cpuid_leaf_7_ebx(Bit32u extra = 0) const;
  Bit32u get_std_cpuid_leaf_7_ecx(Bit32u extra = 0) const;
  Bit32u get_std_cpuid_leaf_7_edx(Bit32u extra = 0) const;
  Bit32u get_std_cpuid_leaf_7_subleaf_1_eax(Bit32u extra = 0) const;
  Bit32u get_std_cpuid_leaf_7_subleaf_1_edx(Bit32u extra = 0) const;

  Bit32u get_ext_cpuid_leaf_1_ecx_intel(Bit32u extra = 0) const;
  Bit32u get_ext_cpuid_leaf_1_edx_amd(Bit32u extra = 0) const;
  Bit32u get_ext_cpuid_leaf_1_edx_intel() const;

  void get_ext_cpuid_leaf_8(cpuid_function_t *leaf) const;

  BX_CPP_INLINE void get_leaf(cpuid_function_t *leaf, Bit32u eax, Bit32u ebx, Bit32u ecx, Bit32u edx) const
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
#define BX_VMX_POSTED_INTERRUPTS                (1 << 19)   /* Posted Interrupts support */
#define BX_VMX_VMCS_SHADOWING                   (1 << 20)   /* VMCS Shadowing */
#define BX_VMX_EPT_EXCEPTION                    (1 << 21)   /* EPT Violation (#VE) exception */
#define BX_VMX_PML                              (1 << 22)   /* Page Modification Logging */
#define BX_VMX_SPP                              (1 << 23)   /* Sub Page Protection */
#define BX_VMX_TSC_SCALING                      (1 << 24)   /* TSC Scaling */
#define BX_VMX_SW_INTERRUPT_INJECTION_ILEN_0    (1 << 25)   /* Allow software interrupt injection with instruction length 0 */
#define BX_VMX_MBE_CONTROL                      (1 << 26)   /* Mode-Based Execution Control (XU/XS) */
#define BX_VMX_SPEC_CTRL_VIRTUALIZATION         (1 << 27)   /* Virtualize MSR IA32_SPEC_CTRL */

// CPUID defines - features CPUID[0x00000001].EDX
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

#define BX_CPUID_STD1_EDX_X87                     (1 <<  0)
#define BX_CPUID_STD1_EDX_VME                     (1 <<  1)
#define BX_CPUID_STD1_EDX_DEBUG_EXTENSIONS        (1 <<  2)
#define BX_CPUID_STD1_EDX_PSE                     (1 <<  3)
#define BX_CPUID_STD1_EDX_TSC                     (1 <<  4)
#define BX_CPUID_STD1_EDX_MSR                     (1 <<  5)
#define BX_CPUID_STD1_EDX_PAE                     (1 <<  6)
#define BX_CPUID_STD1_EDX_MCE                     (1 <<  7)
#define BX_CPUID_STD1_EDX_CMPXCHG8B               (1 <<  8)
#define BX_CPUID_STD1_EDX_APIC                    (1 <<  9)
#define BX_CPUID_STD1_EDX_RESERVED10              (1 << 10)
#define BX_CPUID_STD1_EDX_SYSENTER_SYSEXIT        (1 << 11)
#define BX_CPUID_STD1_EDX_MTRR                    (1 << 12)
#define BX_CPUID_STD1_EDX_GLOBAL_PAGES            (1 << 13)
#define BX_CPUID_STD1_EDX_MCA                     (1 << 14)
#define BX_CPUID_STD1_EDX_CMOV                    (1 << 15)
#define BX_CPUID_STD1_EDX_PAT                     (1 << 16)
#define BX_CPUID_STD1_EDX_PSE36                   (1 << 17)
#define BX_CPUID_STD1_EDX_PROCESSOR_SERIAL_NUMBER (1 << 18)
#define BX_CPUID_STD1_EDX_CLFLUSH                 (1 << 19)
#define BX_CPUID_STD1_EDX_RESERVED20              (1 << 20)
#define BX_CPUID_STD1_EDX_DEBUG_STORE             (1 << 21)
#define BX_CPUID_STD1_EDX_ACPI                    (1 << 22)
#define BX_CPUID_STD1_EDX_MMX                     (1 << 23)
#define BX_CPUID_STD1_EDX_FXSAVE_FXRSTOR          (1 << 24)
#define BX_CPUID_STD1_EDX_SSE                     (1 << 25)
#define BX_CPUID_STD1_EDX_SSE2                    (1 << 26)
#define BX_CPUID_STD1_EDX_SELF_SNOOP              (1 << 27)
#define BX_CPUID_STD1_EDX_HT                      (1 << 28)
#define BX_CPUID_STD1_EDX_THERMAL_MONITOR         (1 << 29)
#define BX_CPUID_STD1_EDX_RESERVED30              (1 << 30)
#define BX_CPUID_STD1_EDX_PBE                     (1 << 31)

// CPUID defines - features CPUID[0x00000001].ECX
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

#define BX_CPUID_STD1_ECX_SSE3                    (1 <<  0)
#define BX_CPUID_STD1_ECX_PCLMULQDQ               (1 <<  1)
#define BX_CPUID_STD1_ECX_DTES64                  (1 <<  2)
#define BX_CPUID_STD1_ECX_MONITOR_MWAIT           (1 <<  3)
#define BX_CPUID_STD1_ECX_DS_CPL                  (1 <<  4)
#define BX_CPUID_STD1_ECX_VMX                     (1 <<  5)
#define BX_CPUID_STD1_ECX_SMX                     (1 <<  6)
#define BX_CPUID_STD1_ECX_EST                     (1 <<  7)
#define BX_CPUID_STD1_ECX_THERMAL_MONITOR2        (1 <<  8)
#define BX_CPUID_STD1_ECX_SSSE3                   (1 <<  9)
#define BX_CPUID_STD1_ECX_CNXT_ID                 (1 << 10)
#define BX_CPUID_STD1_ECX_DEBUG_INTERFACE         (1 << 11)
#define BX_CPUID_STD1_ECX_FMA                     (1 << 12)
#define BX_CPUID_STD1_ECX_CMPXCHG16B              (1 << 13)
#define BX_CPUID_STD1_ECX_xTPR                    (1 << 14)
#define BX_CPUID_STD1_ECX_PDCM                    (1 << 15)
#define BX_CPUID_STD1_ECX_RESERVED16              (1 << 16)
#define BX_CPUID_STD1_ECX_PCID                    (1 << 17)
#define BX_CPUID_STD1_ECX_DCA                     (1 << 18)
#define BX_CPUID_STD1_ECX_SSE4_1                  (1 << 19)
#define BX_CPUID_STD1_ECX_SSE4_2                  (1 << 20)
#define BX_CPUID_STD1_ECX_X2APIC                  (1 << 21)
#define BX_CPUID_STD1_ECX_MOVBE                   (1 << 22)
#define BX_CPUID_STD1_ECX_POPCNT                  (1 << 23)
#define BX_CPUID_STD1_ECX_TSC_DEADLINE            (1 << 24)
#define BX_CPUID_STD1_ECX_AES                     (1 << 25)
#define BX_CPUID_STD1_ECX_XSAVE                   (1 << 26)
#define BX_CPUID_STD1_ECX_OSXSAVE                 (1 << 27)
#define BX_CPUID_STD1_ECX_AVX                     (1 << 28)
#define BX_CPUID_STD1_ECX_AVX_F16C                (1 << 29)
#define BX_CPUID_STD1_ECX_RDRAND                  (1 << 30)
#define BX_CPUID_STD1_ECX_RESERVED31              (1 << 31)

// CPUID defines - features CPUID[0x00000007].EBX [subleaf 0]
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

#define BX_CPUID_STD7_SUBLEAF0_EBX_FSGSBASE               (1 <<  0)
#define BX_CPUID_STD7_SUBLEAF0_EBX_TSC_ADJUST             (1 <<  1)
#define BX_CPUID_STD7_SUBLEAF0_EBX_SGX                    (1 <<  2)
#define BX_CPUID_STD7_SUBLEAF0_EBX_BMI1                   (1 <<  3)
#define BX_CPUID_STD7_SUBLEAF0_EBX_HLE                    (1 <<  4)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX2                   (1 <<  5)
#define BX_CPUID_STD7_SUBLEAF0_EBX_FDP_DEPRECATION        (1 <<  6)
#define BX_CPUID_STD7_SUBLEAF0_EBX_SMEP                   (1 <<  7)
#define BX_CPUID_STD7_SUBLEAF0_EBX_BMI2                   (1 <<  8)
#define BX_CPUID_STD7_SUBLEAF0_EBX_ENCHANCED_REP_STRINGS  (1 <<  9)
#define BX_CPUID_STD7_SUBLEAF0_EBX_INVPCID                (1 << 10)
#define BX_CPUID_STD7_SUBLEAF0_EBX_RTM                    (1 << 11)
#define BX_CPUID_STD7_SUBLEAF0_EBX_QOS_MONITORING         (1 << 12)
#define BX_CPUID_STD7_SUBLEAF0_EBX_DEPRECATE_FCS_FDS      (1 << 13)
#define BX_CPUID_STD7_SUBLEAF0_EBX_MPX                    (1 << 14)
#define BX_CPUID_STD7_SUBLEAF0_EBX_QOS_ENFORCEMENT        (1 << 15)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX512F                (1 << 16)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX512DQ               (1 << 17)
#define BX_CPUID_STD7_SUBLEAF0_EBX_RDSEED                 (1 << 18)
#define BX_CPUID_STD7_SUBLEAF0_EBX_ADX                    (1 << 19)
#define BX_CPUID_STD7_SUBLEAF0_EBX_SMAP                   (1 << 20)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX512IFMA52           (1 << 21)
#define BX_CPUID_STD7_SUBLEAF0_EBX_RESERVED22             (1 << 22)
#define BX_CPUID_STD7_SUBLEAF0_EBX_CLFLUSHOPT             (1 << 23)
#define BX_CPUID_STD7_SUBLEAF0_EBX_CLWB                   (1 << 24)
#define BX_CPUID_STD7_SUBLEAF0_EBX_PROCESSOR_TRACE        (1 << 25)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX512PF               (1 << 26)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX512ER               (1 << 27)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX512CD               (1 << 28)
#define BX_CPUID_STD7_SUBLEAF0_EBX_SHA                    (1 << 29)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX512BW               (1 << 30)
#define BX_CPUID_STD7_SUBLEAF0_EBX_AVX512VL               (1 << 31)

// CPUID defines - features CPUID[0x00000007].ECX  [subleaf 0]
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

#define BX_CPUID_STD7_SUBLEAF0_ECX_PREFETCHWT1            (1 <<  0)
#define BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_VBMI            (1 <<  1)
#define BX_CPUID_STD7_SUBLEAF0_ECX_UMIP                   (1 <<  2)
#define BX_CPUID_STD7_SUBLEAF0_ECX_PKU                    (1 <<  3)
#define BX_CPUID_STD7_SUBLEAF0_ECX_OSPKE                  (1 <<  4)
#define BX_CPUID_STD7_SUBLEAF0_ECX_WAITPKG                (1 <<  5)
#define BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_VBMI2           (1 <<  6)
#define BX_CPUID_STD7_SUBLEAF0_ECX_CET_SS                 (1 <<  7)
#define BX_CPUID_STD7_SUBLEAF0_ECX_GFNI                   (1 <<  8)
#define BX_CPUID_STD7_SUBLEAF0_ECX_VAES                   (1 <<  9)
#define BX_CPUID_STD7_SUBLEAF0_ECX_VPCLMULQDQ             (1 << 10)
#define BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_VNNI            (1 << 11)
#define BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_BITALG          (1 << 12)
#define BX_CPUID_STD7_SUBLEAF0_ECX_TME                    (1 << 13)
#define BX_CPUID_STD7_SUBLEAF0_ECX_AVX512_VPOPCNTDQ       (1 << 14)
#define BX_CPUID_STD7_SUBLEAF0_ECX_RESERVED15             (1 << 15)
#define BX_CPUID_STD7_SUBLEAF0_ECX_LA57                   (1 << 16)
#define BX_CPUID_STD7_SUBLEAF0_ECX_RESERVED17             (1 << 17)
#define BX_CPUID_STD7_SUBLEAF0_ECX_RESERVED18             (1 << 18)
#define BX_CPUID_STD7_SUBLEAF0_ECX_RESERVED19             (1 << 19)
#define BX_CPUID_STD7_SUBLEAF0_ECX_RESERVED20             (1 << 20)
#define BX_CPUID_STD7_SUBLEAF0_ECX_RESERVED21             (1 << 21)
#define BX_CPUID_STD7_SUBLEAF0_ECX_RDPID                  (1 << 22)
#define BX_CPUID_STD7_SUBLEAF0_ECX_KEYLOCKER              (1 << 23)
#define BX_CPUID_STD7_SUBLEAF0_ECX_BUS_LOCK_DETECT        (1 << 24)
#define BX_CPUID_STD7_SUBLEAF0_ECX_CLDEMOTE               (1 << 25)
#define BX_CPUID_STD7_SUBLEAF0_ECX_RESERVED26             (1 << 26)
#define BX_CPUID_STD7_SUBLEAF0_ECX_MOVDIRI                (1 << 27)
#define BX_CPUID_STD7_SUBLEAF0_ECX_MOVDIR64B              (1 << 28)
#define BX_CPUID_STD7_SUBLEAF0_ECX_ENQCMD                 (1 << 29)
#define BX_CPUID_STD7_SUBLEAF0_ECX_SGX_LAUNCH_CONFIG      (1 << 30)
#define BX_CPUID_STD7_SUBLEAF0_ECX_PKS                    (1 << 31)

// CPUID defines - features CPUID[0x00000007].EDX  [subleaf 0]
// -----------------------------
//   [0:0]    reserved
//   [1:1]    SGX-KEYS: Attestation Services for SGX support
//   [2:2]    AVX512 4VNNIW instructions support
//   [3:3]    AVX512 4FMAPS instructions support
//   [4:4]    Support of Fast REP MOV instructions with short length
//   [5:5]    UINTR: User interrupts support
//   [7:6]    reserved
//   [8:8]    AVX512 VP2INTERSECT instructions support
//   [9:9]    SRBDS_CTRL: IA32_MCU_OPT_CTRL MSR
//   [10:10]  MD clear
//   [11:11]  RTM_ALWAYS_ABORT
//   [12:12]  reserved
//   [13:13]  RTM_FORCE_ABORT
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

#define BX_CPUID_STD7_SUBLEAF0_EDX_RESERVED0              (1 <<  0)
#define BX_CPUID_STD7_SUBLEAF0_EDX_SGX_KEYS               (1 <<  1)
#define BX_CPUID_STD7_SUBLEAF0_EDX_AVX512_4VNNIW          (1 <<  2)
#define BX_CPUID_STD7_SUBLEAF0_EDX_AVX512_4FMAPS          (1 <<  3)
#define BX_CPUID_STD7_SUBLEAF0_EDX_FAST_SHORT_REP_MOV     (1 <<  4)
#define BX_CPUID_STD7_SUBLEAF0_EDX_UINTR                  (1 <<  5)
#define BX_CPUID_STD7_SUBLEAF0_EDX_RESERVED6              (1 <<  6)
#define BX_CPUID_STD7_SUBLEAF0_EDX_RESERVED7              (1 <<  7)
#define BX_CPUID_STD7_SUBLEAF0_EDX_AVX512_VPINTERSECT     (1 <<  8)
#define BX_CPUID_STD7_SUBLEAF0_EDX_SRBDS_CTRL             (1 <<  9)
#define BX_CPUID_STD7_SUBLEAF0_EDX_MD_CLEAR               (1 << 10)
#define BX_CPUID_STD7_SUBLEAF0_EDX_RTM_ALWAYS_ABORT       (1 << 11)
#define BX_CPUID_STD7_SUBLEAF0_EDX_RESERVED12             (1 << 12)
#define BX_CPUID_STD7_SUBLEAF0_EDX_RTM_FORCE_ABORT        (1 << 13)
#define BX_CPUID_STD7_SUBLEAF0_EDX_SERIALIZE              (1 << 14)
#define BX_CPUID_STD7_SUBLEAF0_EDX_HYBRID                 (1 << 15)
#define BX_CPUID_STD7_SUBLEAF0_EDX_TSXLDTRK               (1 << 16)
#define BX_CPUID_STD7_SUBLEAF0_EDX_RESERVED17             (1 << 17)
#define BX_CPUID_STD7_SUBLEAF0_EDX_PCONFIG                (1 << 18)
#define BX_CPUID_STD7_SUBLEAF0_EDX_ARCH_LBR               (1 << 19)
#define BX_CPUID_STD7_SUBLEAF0_EDX_CET_IBT                (1 << 20)
#define BX_CPUID_STD7_SUBLEAF0_EDX_RESERVED21             (1 << 21)
#define BX_CPUID_STD7_SUBLEAF0_EDX_AMX_BF16               (1 << 22)
#define BX_CPUID_STD7_SUBLEAF0_EDX_AVX512_FP16            (1 << 23)
#define BX_CPUID_STD7_SUBLEAF0_EDX_AMX_TILE               (1 << 24)
#define BX_CPUID_STD7_SUBLEAF0_EDX_AMX_INT8               (1 << 25)
#define BX_CPUID_STD7_SUBLEAF0_EDX_SCA_IBRS_IBPB          (1 << 26)
#define BX_CPUID_STD7_SUBLEAF0_EDX_SCA_STIBP              (1 << 27)
#define BX_CPUID_STD7_SUBLEAF0_EDX_L1D_FLUSH              (1 << 28)
#define BX_CPUID_STD7_SUBLEAF0_EDX_ARCH_CAPABILITIES_MSR  (1 << 29)
#define BX_CPUID_STD7_SUBLEAF0_EDX_CORE_CAPABILITIES_MSR  (1 << 30)
#define BX_CPUID_STD7_SUBLEAF0_EDX_SCA_SSBD               (1 << 31)

// CPUID defines - features CPUID[0x00000007].EAX  [subleaf 1]
// -----------------------------
//   [0:0]    SHA-512 instructions support
//   [1:1]    SM3 instructions support
//   [2:2]    SM4 instructions support
//   [3:3]    RAO-INT
//   [4:4]    AVX VNNI
//   [5:5]    AVX512_BF16 conversion instructions support
//   [6:6]    LASS: Linear Address Space Separation support
//   [7:7]    CMPCCXADD
//   [8:8]    Arch Perfmon
//   [9:9]    reserved
//   [10:10]  Fast zero-length REP MOVSB
//   [11:11]  Fast zero-length REP STOSB
//   [12:12]  Fast zero-length REP CMPSB/SCASB
//   [16:13]  reserved
//   [17:17]  Flexible Return and Event Delivery (FRED) support
//   [18:18]  LKGS instruction support
//   [19:19]  WRMSRNS instruction
//   [20:20]  NMI source reporting
//   [21:21]  AMX-FB16 support
//   [22:22]  HRESET and CPUID leaf 0x20 support
//   [23:23]  AVX IFMA support
//   [25:24]  reserved
//   [26:26]  LAM: Linear Address Masking
//   [27:27]  MSRLIST: RDMSRLIST/WRMSRLIST instructions and the IA32_BARRIER MSR
//   [29:28]  reserved
//   [30:30]  Prevent INVD execution after BIOS is done
//   [31:28]  reserved

#define BX_CPUID_STD7_SUBLEAF1_EAX_SHA512                 (1 <<  0)
#define BX_CPUID_STD7_SUBLEAF1_EAX_SM3                    (1 <<  1)
#define BX_CPUID_STD7_SUBLEAF1_EAX_SM4                    (1 <<  2)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RAO_INT                (1 <<  3)
#define BX_CPUID_STD7_SUBLEAF1_EAX_AVX_VNNI               (1 <<  4)
#define BX_CPUID_STD7_SUBLEAF1_EAX_AVX512_BF16            (1 <<  5)
#define BX_CPUID_STD7_SUBLEAF1_EAX_LASS                   (1 <<  6)
#define BX_CPUID_STD7_SUBLEAF1_EAX_CMPCCXADD              (1 <<  7)
#define BX_CPUID_STD7_SUBLEAF1_EAX_ARCH_PERFMON           (1 <<  8)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED9              (1 <<  9)
#define BX_CPUID_STD7_SUBLEAF1_EAX_FAST_ZEROLEN_REP_MOVSB (1 << 10)
#define BX_CPUID_STD7_SUBLEAF1_EAX_FAST_ZEROLEN_REP_STOSB (1 << 11)
#define BX_CPUID_STD7_SUBLEAF1_EAX_FAST_ZEROLEN_REP_CMPSB (1 << 12)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED13             (1 << 13)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED14             (1 << 14)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED15             (1 << 15)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED16             (1 << 16)
#define BX_CPUID_STD7_SUBLEAF1_EAX_FRED                   (1 << 17)
#define BX_CPUID_STD7_SUBLEAF1_EAX_LKGS                   (1 << 18)
#define BX_CPUID_STD7_SUBLEAF1_EAX_WRMSRNS                (1 << 19)
#define BX_CPUID_STD7_SUBLEAF1_EAX_NMI_SOURCE_REPORTING   (1 << 20)
#define BX_CPUID_STD7_SUBLEAF1_EAX_AMX_FP16               (1 << 21)
#define BX_CPUID_STD7_SUBLEAF1_EAX_HRESET                 (1 << 22)
#define BX_CPUID_STD7_SUBLEAF1_EAX_AVX_IFMA               (1 << 23)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED24             (1 << 24)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED25             (1 << 25)
#define BX_CPUID_STD7_SUBLEAF1_EAX_LAM                    (1 << 26)
#define BX_CPUID_STD7_SUBLEAF1_EAX_MSRLIST                (1 << 27)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED28             (1 << 28)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED29             (1 << 29)
#define BX_CPUID_STD7_SUBLEAF1_EAX_INVD_DISABLE           (1 << 30)
#define BX_CPUID_STD7_SUBLEAF1_EAX_RESERVED31             (1 << 31)

// CPUID defines - features CPUID[0x00000007].EBX  [subleaf 1]
// -----------------------------
//   [0:0]    IA32_PPIN and IA32_PPIN_CTL MSRs
//   [1:1]    TSE: PBNDKB instruction and existence of the IA32_TSE_CAPABILITY MSR
//   [31:1]   reserved

// ...
#define BX_CPUID_STD7_SUBLEAF1_EBX_PPIN                   (1 <<  0)
#define BX_CPUID_STD7_SUBLEAF1_EBX_TSE                    (1 <<  1)
// ...

// CPUID defines - features CPUID[0x00000007].ECX  [subleaf 1]
// -----------------------------
//   [31:0]  reserved

// ...

// CPUID defines - features CPUID[0x00000007].EDX  [subleaf 1]
// -----------------------------
//   [3:0]    reserved
//   [4:4]    AVX_VNNI_INT8 support
//   [5:5]    AVX_NE_CONVERT instructions
//   [7:6]    reserved
//   [8:8]    AMX-COMPLEX instructions
//   [9:9]    reserved
//   [10:10]  AVX-VNNI-INT16 instructions
//   [13:11]  reserved
//   [14:14]  PREFETCHITI: PREFETCHIT0/T1 instruction
//   [15:15]  USER_MSR: support for URDMSR/UWRMSR instructions
//   [16:16]  reserved
//   [17:17]  UIRET sets UIF to the RFLAGS[1] image loaded from the stack
//   [18:18]  CET_SSS

#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED0              (1 <<  0)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED1              (1 <<  1)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED2              (1 <<  2)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED3              (1 <<  3)
#define BX_CPUID_STD7_SUBLEAF1_EDX_AVX_VNNI_INT8          (1 <<  4)
#define BX_CPUID_STD7_SUBLEAF1_EDX_AVX_NE_CONVERT         (1 <<  5)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED6              (1 <<  6)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED7              (1 <<  7)
#define BX_CPUID_STD7_SUBLEAF1_EDX_AMX_COMPLEX            (1 <<  8)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED9              (1 <<  9)
#define BX_CPUID_STD7_SUBLEAF1_EDX_AVX_VNNI_INT16         (1 << 10)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED11             (1 << 11)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED12             (1 << 12)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED13             (1 << 13)
#define BX_CPUID_STD7_SUBLEAF1_EDX_PREFETCHI              (1 << 14)
#define BX_CPUID_STD7_SUBLEAF1_EDX_USER_MSR               (1 << 15)
#define BX_CPUID_STD7_SUBLEAF1_EDX_RESERVED16             (1 << 16)
#define BX_CPUID_STD7_SUBLEAF1_EDX_UIRET_UIF              (1 << 17)
#define BX_CPUID_STD7_SUBLEAF1_EDX_CET_SSS                (1 << 18)
// ...

// CPUID defines - STD2 features CPUID[0x80000001].EDX
// -----------------------------

// ...
#define BX_CPUID_EXT1_EDX_SYSCALL_SYSRET         (1 << 11)
// ...
#define BX_CPUID_EXT1_EDX_NX                     (1 << 20)
#define BX_CPUID_EXT1_EDX_RESERVED21             (1 << 21)
#define BX_CPUID_EXT1_EDX_AMD_MMX_EXT            (1 << 22)
#define BX_CPUID_EXT1_EDX_RESERVED23             (1 << 23)
#define BX_CPUID_EXT1_EDX_RESERVED24             (1 << 24)
#define BX_CPUID_EXT1_EDX_FFXSR                  (1 << 25)
#define BX_CPUID_EXT1_EDX_1G_PAGES               (1 << 26)
#define BX_CPUID_EXT1_EDX_RDTSCP                 (1 << 27)
#define BX_CPUID_EXT1_EDX_RESERVED28             (1 << 28)
#define BX_CPUID_EXT1_EDX_LONG_MODE              (1 << 29)
#define BX_CPUID_EXT1_EDX_3DNOW_EXT              (1 << 30)
#define BX_CPUID_EXT1_EDX_3DNOW                  (1 << 31)

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

#define BX_CPUID_EXT1_ECX_LAHF_SAHF              (1 <<  0)
#define BX_CPUID_EXT1_ECX_CMP_LEGACY             (1 <<  1)
#define BX_CPUID_EXT1_ECX_SVM                    (1 <<  2)
#define BX_CPUID_EXT1_ECX_EXT_APIC_SPACE         (1 <<  3)
#define BX_CPUID_EXT1_ECX_ALT_MOV_CR8            (1 <<  4)
#define BX_CPUID_EXT1_ECX_LZCNT                  (1 <<  5)
#define BX_CPUID_EXT1_ECX_SSE4A                  (1 <<  6)
#define BX_CPUID_EXT1_ECX_MISALIGNED_SSE         (1 <<  7)
#define BX_CPUID_EXT1_ECX_PREFETCHW              (1 <<  8)
#define BX_CPUID_EXT1_ECX_OSVW                   (1 <<  9)
#define BX_CPUID_EXT1_ECX_IBS                    (1 << 10)
#define BX_CPUID_EXT1_ECX_XOP                    (1 << 11)
#define BX_CPUID_EXT1_ECX_SKINIT                 (1 << 12)
#define BX_CPUID_EXT1_ECX_WDT                    (1 << 13)
#define BX_CPUID_EXT1_ECX_RESERVED14             (1 << 14)
#define BX_CPUID_EXT1_ECX_LWP                    (1 << 15)
#define BX_CPUID_EXT1_ECX_FMA4                   (1 << 16)
#define BX_CPUID_EXT1_ECX_TCE                    (1 << 17)
#define BX_CPUID_EXT1_ECX_RESERVED18             (1 << 18)
#define BX_CPUID_EXT1_ECX_NODEID                 (1 << 19)
#define BX_CPUID_EXT1_ECX_RESERVED20             (1 << 20)
#define BX_CPUID_EXT1_ECX_TBM                    (1 << 21)
#define BX_CPUID_EXT1_ECX_TOPOLOGY_EXTENSIONS    (1 << 22)
#define BX_CPUID_EXT1_ECX_PERFCTR_EXT_CORE       (1 << 23)
#define BX_CPUID_EXT1_ECX_PERFCTR_EXT_NB         (1 << 24)
#define BX_CPUID_EXT1_ECX_RESERVED25             (1 << 25)
#define BX_CPUID_EXT1_ECX_DATA_BREAKPOINT_EXT    (1 << 26)
#define BX_CPUID_EXT1_ECX_PERF_TSC               (1 << 27)
#define BX_CPUID_EXT1_ECX_PERFCTR_EXT_L2I        (1 << 28)
#define BX_CPUID_EXT1_ECX_MONITORX_MWAITX        (1 << 29)
#define BX_CPUID_EXT1_ECX_CODEBP_ADDRMASK_EXT    (1 << 30)
#define BX_CPUID_EXT1_ECX_RESERVED31             (1 << 31)

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
