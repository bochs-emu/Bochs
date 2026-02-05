/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2015 Stanislav Shwartsman
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

#ifndef _BX_SVM_AMD_H_
#define _BX_SVM_AMD_H_

#if BX_SUPPORT_SVM

#define BX_SVM_REVISION 0x01 /* FIXME: check what is real SVM revision */

enum SVM_intercept_codes {
   SVM_VMEXIT_CR0_READ  = 0x0,
   SVM_VMEXIT_CR2_READ  = 0x2,
   SVM_VMEXIT_CR3_READ  = 0x3,
   SVM_VMEXIT_CR4_READ  = 0x4,
   SVM_VMEXIT_CR8_READ  = 0x8,
   SVM_VMEXIT_CR0_WRITE = 0x10,
   SVM_VMEXIT_CR2_WRITE = 0x12,
   SVM_VMEXIT_CR3_WRITE = 0x13,
   SVM_VMEXIT_CR4_WRITE = 0x14,
   SVM_VMEXIT_CR8_WRITE = 0x18,
   SVM_VMEXIT_DR0_READ  = 0x20,
   SVM_VMEXIT_DR0_WRITE = 0x30,
   SVM_VMEXIT_EXCEPTION = 0x40,
   SVM_VMEXIT_PF_EXCEPTION = (0x40+BX_PF_EXCEPTION),
   SVM_VMEXIT_INTR = 0x60,
   SVM_VMEXIT_NMI = 0x61,
   SVM_VMEXIT_SMI = 0x62,
   SVM_VMEXIT_INIT = 0x63,
   SVM_VMEXIT_VINTR = 0x64,
   SVM_VMEXIT_CR0_SEL_WRITE = 0x65,
   SVM_VMEXIT_IDTR_READ = 0x66,
   SVM_VMEXIT_GDTR_READ = 0x67,
   SVM_VMEXIT_LDTR_READ = 0x68,
   SVM_VMEXIT_TR_READ = 0x69,
   SVM_VMEXIT_IDTR_WRITE = 0x6a,
   SVM_VMEXIT_GDTR_WRITE = 0x6b,
   SVM_VMEXIT_LDTR_WRITE = 0x6c,
   SVM_VMEXIT_TR_WRITE = 0x6d,
   SVM_VMEXIT_RDTSC = 0x6e,
   SVM_VMEXIT_RDPMC = 0x6f,
   SVM_VMEXIT_PUSHF = 0x70,
   SVM_VMEXIT_POPF = 0x71,
   SVM_VMEXIT_CPUID = 0x72,
   SVM_VMEXIT_RSM = 0x73,
   SVM_VMEXIT_IRET = 0x74,
   SVM_VMEXIT_SOFTWARE_INTERRUPT = 0x75,
   SVM_VMEXIT_INVD = 0x76,
   SVM_VMEXIT_PAUSE = 0x77,
   SVM_VMEXIT_HLT = 0x78,
   SVM_VMEXIT_INVLPG = 0x79,
   SVM_VMEXIT_INVLPGA = 0x7a,
   SVM_VMEXIT_IO = 0x7b,
   SVM_VMEXIT_MSR = 0x7c,
   SVM_VMEXIT_TASK_SWITCH = 0x7d,
   SVM_VMEXIT_FERR_FREEZE = 0x7e,
   SVM_VMEXIT_SHUTDOWN = 0x7f,
   SVM_VMEXIT_VMRUN = 0x80,
   SVM_VMEXIT_VMMCALL = 0x81,
   SVM_VMEXIT_VMLOAD = 0x82,
   SVM_VMEXIT_VMSAVE = 0x83,
   SVM_VMEXIT_STGI = 0x84,
   SVM_VMEXIT_CLGI = 0x85,
   SVM_VMEXIT_SKINIT = 0x86,
   SVM_VMEXIT_RDTSCP = 0x87,
   SVM_VMEXIT_ICEBP = 0x88,
   SVM_VMEXIT_WBINVD = 0x89,
   SVM_VMEXIT_MONITOR = 0x8a,
   SVM_VMEXIT_MWAIT = 0x8b,
   SVM_VMEXIT_MWAIT_CONDITIONAL = 0x8c,
   SVM_VMEXIT_XSETBV = 0x8d,
   SVM_VMEXIT_RDPRU = 0x8e,
   SVM_VMEXIT_EFER_WRITE_TRAP = 0x8f,
   SVM_VMEXIT_CR0_WRITE_TRAP = 0x90,
   SVM_VMEXIT_CR3_WRITE_TRAP = 0x93,
   SVM_VMEXIT_CR4_WRITE_TRAP = 0x94,
   SVM_VMEXIT_INVLPGB = 0xa0,
   SVM_VMEXIT_INVLPGB_ILLEGAL = 0xa1,
   SVM_VMEXIT_INVPCID = 0xa2,
   SVM_VMEXIT_MCOMMIT = 0xa3,
   SVM_VMEXIT_TLBSYNC = 0xa4,
   SVM_VMEXIT_BUSLOCK = 0xa5,
   SVM_VMEXIT_IDLE_HLT = 0xa6,
   SVM_VMEXIT_NPF = 0x400,
   SVM_VMEXIT_AVIC_INCOMPLETE_IPI = 0x401,
   SVM_VMEXIT_AVIC_NOACCEL = 0x402,
   SVM_VMEXIT_VMGEXIT = 0x403
};

#define SVM_VMEXIT_INVALID (-1)

// =====================
//  VMCB control fields
// =====================

#define SVM_CONTROL16_INTERCEPT_CR_READ         (0x000)
#define SVM_CONTROL16_INTERCEPT_CR_WRITE        (0x002)
#define SVM_CONTROL16_INTERCEPT_DR_READ         (0x004)
#define SVM_CONTROL16_INTERCEPT_DR_WRITE        (0x006)
#define SVM_CONTROL32_INTERCEPT_EXCEPTIONS      (0x008)
#define SVM_CONTROL32_INTERCEPT1                (0x00c)
#define SVM_CONTROL32_INTERCEPT2                (0x010)
#define SVM_CONTROL32_INTERCEPT3                (0x014)

#define SVM_CONTROL16_PAUSE_FILTER_THRESHOLD    (0x03c)
#define SVM_CONTROL16_PAUSE_FILTER_COUNT        (0x03e)
#define SVM_CONTROL64_IOPM_BASE_PHY_ADDR        (0x040)
#define SVM_CONTROL64_MSRPM_BASE_PHY_ADDR       (0x048)
#define SVM_CONTROL64_TSC_OFFSET                (0x050)
#define SVM_CONTROL32_GUEST_ASID                (0x058)
#define SVM_CONTROL32_TLB_CONTROL               (0x05c)
#define SVM_CONTROL_VTPR                        (0x060)
#define SVM_CONTROL_VIRQ                        (0x061)
#define SVM_CONTROL_VINTR_PRIO_IGN_TPR          (0x062)
#define SVM_CONTROL_VINTR_MASKING               (0x063)
#define SVM_CONTROL_VINTR_VECTOR                (0x064)
#define SVM_CONTROL_INTERRUPT_SHADOW            (0x068)
#define SVM_CONTROL64_EXITCODE                  (0x070)
#define SVM_CONTROL64_EXITINFO1                 (0x078)
#define SVM_CONTROL64_EXITINFO2                 (0x080)
#define SVM_CONTROL32_EXITINTINFO               (0x088)
#define SVM_CONTROL32_EXITINTINFO_ERROR_CODE    (0x08c)
#define SVM_CONTROL_NESTED_PAGING_ENABLE        (0x090)

#define SVM_VIRTUAL_APIC_BAR                    (0x098)

#define SVM_CONTROL32_EVENT_INJECTION           (0x0a8)
#define SVM_CONTROL32_EVENT_INJECTION_ERRORCODE (0x0ac)
#define SVM_CONTROL64_NESTED_PAGING_HOST_CR3    (0x0b0)
#define SVM_CONTROL_LBR_VIRTUALIZATION_ENABLE   (0x0b8)
#define SVM_CONTROL32_VMCB_CLEAN_BITS           (0x0c0)
#define SVM_CONTROL64_NRIP                      (0x0c8)

#define SVM_CONTROL64_GUEST_INSTR_BYTES         (0x0d0)
#define SVM_CONTROL64_GUEST_INSTR_BYTES_HI      (0x0d8)

#define SVM_AVIC_BACKING_PAGE                   (0x0e0)
#define SVM_AVIC_LOGICAL_TABLE_PTR              (0x0f0)
#define SVM_AVIC_PHYSICAL_TABLE_PTR             (0x0f8)

// ======================
//  VMCB save state area
// ======================

// ES
#define SVM_GUEST_ES_SELECTOR                   (0x400)
#define SVM_GUEST_ES_ATTR                       (0x402)
#define SVM_GUEST_ES_LIMIT                      (0x404)
#define SVM_GUEST_ES_BASE                       (0x408)

// CS
#define SVM_GUEST_CS_SELECTOR                   (0x410)
#define SVM_GUEST_CS_ATTR                       (0x412)
#define SVM_GUEST_CS_LIMIT                      (0x414)
#define SVM_GUEST_CS_BASE                       (0x418)

// SS
#define SVM_GUEST_SS_SELECTOR                   (0x420)
#define SVM_GUEST_SS_ATTR                       (0x422)
#define SVM_GUEST_SS_LIMIT                      (0x424)
#define SVM_GUEST_SS_BASE                       (0x428)

// DS
#define SVM_GUEST_DS_SELECTOR                   (0x430)
#define SVM_GUEST_DS_ATTR                       (0x432)
#define SVM_GUEST_DS_LIMIT                      (0x434)
#define SVM_GUEST_DS_BASE                       (0x438)

// FS
#define SVM_GUEST_FS_SELECTOR                   (0x440)
#define SVM_GUEST_FS_ATTR                       (0x442)
#define SVM_GUEST_FS_LIMIT                      (0x444)
#define SVM_GUEST_FS_BASE                       (0x448)

// GS
#define SVM_GUEST_GS_SELECTOR                   (0x450)
#define SVM_GUEST_GS_ATTR                       (0x452)
#define SVM_GUEST_GS_LIMIT                      (0x454)
#define SVM_GUEST_GS_BASE                       (0x458)

// GDTR
#define SVM_GUEST_GDTR_LIMIT                    (0x464)
#define SVM_GUEST_GDTR_BASE                     (0x468)

// LDTR
#define SVM_GUEST_LDTR_SELECTOR                 (0x470)
#define SVM_GUEST_LDTR_ATTR                     (0x472)
#define SVM_GUEST_LDTR_LIMIT                    (0x474)
#define SVM_GUEST_LDTR_BASE                     (0x478)

// IDTR
#define SVM_GUEST_IDTR_LIMIT                    (0x484)
#define SVM_GUEST_IDTR_BASE                     (0x488)

// TR
#define SVM_GUEST_TR_SELECTOR                   (0x490)
#define SVM_GUEST_TR_ATTR                       (0x492)
#define SVM_GUEST_TR_LIMIT                      (0x494)
#define SVM_GUEST_TR_BASE                       (0x498)

#define SVM_GUEST_PL0_SSP                       (0x4a0)
#define SVM_GUEST_PL1_SSP                       (0x4a8)
#define SVM_GUEST_PL2_SSP                       (0x4b0)
#define SVM_GUEST_PL3_SSP                       (0x4b8)
#define SVM_GUEST_U_CET                         (0x4c0)

#define SVM_GUEST_CPL                           (0x4cb)
#define SVM_GUEST_EFER_MSR                      (0x4d0)
#define SVM_GUEST_EFER_MSR_HI                   (0x4d4)

#define SVM_GUEST_XSS                           (0x540)
#define SVM_GUEST_CR4                           (0x548)
#define SVM_GUEST_CR4_HI                        (0x54c)
#define SVM_GUEST_CR3                           (0x550)
#define SVM_GUEST_CR0                           (0x558)
#define SVM_GUEST_CR0_HI                        (0x55c)
#define SVM_GUEST_DR7                           (0x560)
#define SVM_GUEST_DR7_HI                        (0x564)
#define SVM_GUEST_DR6                           (0x568)
#define SVM_GUEST_DR6_HI                        (0x56c)
#define SVM_GUEST_RFLAGS                        (0x570)
#define SVM_GUEST_RFLAGS_HI                     (0x574)
#define SVM_GUEST_RIP                           (0x578)
#define SVM_GUEST_RSP                           (0x5d8)
#define SVM_GUEST_S_CET                         (0x5e0)
#define SVM_GUEST_SSP                           (0x5e8)
#define SVM_GUEST_ISST_ADDR                     (0x5f0)
#define SVM_GUEST_RAX                           (0x5f8)
#define SVM_GUEST_STAR_MSR                      (0x600)
#define SVM_GUEST_LSTAR_MSR                     (0x608)
#define SVM_GUEST_CSTAR_MSR                     (0x610)
#define SVM_GUEST_FMASK_MSR                     (0x618)
#define SVM_GUEST_KERNEL_GSBASE_MSR             (0x620)
#define SVM_GUEST_SYSENTER_CS_MSR               (0x628)
#define SVM_GUEST_SYSENTER_ESP_MSR              (0x630)
#define SVM_GUEST_SYSENTER_EIP_MSR              (0x638)
#define SVM_GUEST_CR2                           (0x640)

#define SVM_GUEST_PAT                           (0x668) /* used only when nested paging is enabled */
#define SVM_GUEST_DBGCTL_MSR                    (0x670)
#define SVM_GUEST_BR_FROM_MSR                   (0x678)
#define SVM_GUEST_BR_TO_MSR                     (0x680)
#define SVM_GUEST_LAST_EXCEPTION_FROM_MSR       (0x688)
#define SVM_GUEST_LAST_EXCEPTION_TO_MSR         (0x690)

#define SVM_GUEST_SPEC_CTRL                     (0x6e0)
#define SVM_GUEST_PKRU                          (0x6e8)
#define SVM_GUEST_TSC_AUX                       (0x6ec)
#define SVM_GUEST_TSC_SCALE                     (0x6f0)
#define SVM_GUEST_TSC_OFFSET                    (0x6f8)

#define SVM_GUEST_RCX                           (0x708)
#define SVM_GUEST_RDX                           (0x710)
#define SVM_GUEST_RBX                           (0x718)
#define SVM_GUEST_SECURE_AVIC_CTL               (0x720)
#define SVM_GUEST_RBP                           (0x728)
#define SVM_GUEST_RSI                           (0x730)
#define SVM_GUEST_RDI                           (0x738)
#define SVM_GUEST_R8                            (0x740)
#define SVM_GUEST_R9                            (0x748)
#define SVM_GUEST_R10                           (0x750)
#define SVM_GUEST_R11                           (0x758)
#define SVM_GUEST_R12                           (0x760)
#define SVM_GUEST_R13                           (0x768)
#define SVM_GUEST_R14                           (0x770)
#define SVM_GUEST_R15                           (0x778)

struct SVM_HOST_STATE
{
  bx_segment_reg_t sregs[4];

  bx_global_segment_reg_t gdtr;
  bx_global_segment_reg_t idtr;

  bx_efer_t efer;
  bx_cr0_t cr0;
  bx_cr4_t cr4;
  bx_phy_address cr3;
  Bit32u eflags;
  Bit64u rip;
  Bit64u rsp;
  Bit64u rax;

  BxPackedRegister pat_msr;
};

struct SVM_GUEST_STATE
{
  bx_segment_reg_t sregs[4];

  bx_global_segment_reg_t gdtr;
  bx_global_segment_reg_t idtr;

  bx_efer_t efer;
  bx_cr0_t cr0;
  bx_cr4_t cr4;
  bx_address cr2;
  Bit32u dr6;
  Bit32u dr7;
  bx_phy_address cr3;
  BxPackedRegister pat_msr;
  Bit32u eflags;
  Bit64u rip;
  Bit64u rsp;
  Bit64u rax;

  unsigned cpl;

  bool inhibit_interrupts;
};

struct SVM_CONTROLS
{
  Bit16u cr_rd_ctrl;
  Bit16u cr_wr_ctrl;
  Bit16u dr_rd_ctrl;
  Bit16u dr_wr_ctrl;
  Bit32u exceptions_intercept;

  Bit32u intercept_vector[3];

  Bit32u exitintinfo;
  Bit32u exitintinfo_error_code;

  Bit32u eventinj;

  bx_phy_address iopm_base;
  bx_phy_address msrpm_base;

  Bit8u v_tpr;
  Bit8u v_intr_prio;
  bool v_ignore_tpr;
  bool v_intr_masking;
  Bit8u v_intr_vector;

  bool nested_paging;
  Bit64u ncr3;

  Bit16u pause_filter_count;
//Bit16u pause_filter_threshold;
};

#if defined(NEED_CPU_REG_SHORTCUTS)

#define SVM_V_TPR          (BX_CPU_THIS_PTR vmcb->ctrls.v_tpr)
#define SVM_V_INTR_PRIO    (BX_CPU_THIS_PTR vmcb->ctrls.v_intr_prio)
#define SVM_V_IGNORE_TPR   (BX_CPU_THIS_PTR vmcb->ctrls.v_ignore_tpr)
#define SVM_V_INTR_MASKING (BX_CPU_THIS_PTR vmcb->ctrls.v_intr_masking)
#define SVM_V_INTR_VECTOR  (BX_CPU_THIS_PTR vmcb->ctrls.v_intr_vector)

#define SVM_HOST_IF (BX_CPU_THIS_PTR vmcb->host_state.eflags & EFlagsIFMask)

#endif

// VM_CR_MSR 0xc0010114
#define BX_VM_CR_MSR_DPD_MASK           (1<<0)
#define BX_VM_CR_MSR_INIT_REDIRECT_MASK (1<<1)
#define BX_VM_CR_MSR_DIS_A20M_MASK      (1<<2)
#define BX_VM_CR_MSR_LOCK_MASK          (1<<3)
#define BX_VM_CR_MSR_SVMDIS_MASK        (1<<4)

struct VMCB_CACHE
{
  SVM_HOST_STATE host_state;
  SVM_CONTROLS ctrls;
};

// ========================
//  SVM intercept controls
// ========================

enum {
  // vector0[15:00]: intercept reads of CR0-CR15
  // vector0[31:16]: intercept writes of CR0-CR15
  // vector1[15:00]: intercept reads of DR0-DR15
  // vector1[31:16]: intercept writes of DR0-DR15
  // vector2[31:00]: intercept exception vectors 0-31
  // vector3[31:00]:
  SVM_INTERCEPT0_INTR = 0,
  SVM_INTERCEPT0_NMI = 1,
  SVM_INTERCEPT0_SMI = 2,
  SVM_INTERCEPT0_INIT = 3,
  SVM_INTERCEPT0_VINTR = 4,
  SVM_INTERCEPT0_CR0_WRITE_NO_TS_MP = 5,
  SVM_INTERCEPT0_IDTR_READ = 6,
  SVM_INTERCEPT0_GDTR_READ = 7,
  SVM_INTERCEPT0_LDTR_READ = 8,
  SVM_INTERCEPT0_TR_READ = 9,
  SVM_INTERCEPT0_IDTR_WRITE = 10,
  SVM_INTERCEPT0_GDTR_WRITE = 11,
  SVM_INTERCEPT0_LDTR_WRITE = 12,
  SVM_INTERCEPT0_TR_WRITE = 13,
  SVM_INTERCEPT0_RDTSC = 14,
  SVM_INTERCEPT0_RDPMC = 15,
  SVM_INTERCEPT0_PUSHF = 16,
  SVM_INTERCEPT0_POPF = 17,
  SVM_INTERCEPT0_CPUID = 18,
  SVM_INTERCEPT0_RSM = 19,
  SVM_INTERCEPT0_IRET = 20,
  SVM_INTERCEPT0_SOFTINT = 21,
  SVM_INTERCEPT0_INVD = 22,
  SVM_INTERCEPT0_PAUSE = 23,
  SVM_INTERCEPT0_HLT = 24,
  SVM_INTERCEPT0_INVLPG = 25,
  SVM_INTERCEPT0_INVLPGA = 26,
  SVM_INTERCEPT0_IO = 27,
  SVM_INTERCEPT0_MSR = 28,
  SVM_INTERCEPT0_TASK_SWITCH = 29,
  SVM_INTERCEPT0_FERR_FREEZE = 30,
  SVM_INTERCEPT0_SHUTDOWN = 31,
  // vector4[16:00]:
  // vector4[31:16]: Intercept writes of CR0-CR15 (trap)
  SVM_INTERCEPT1_VMRUN = 32,
  SVM_INTERCEPT1_VMMCALL = 33,
  SVM_INTERCEPT1_VMLOAD = 34,
  SVM_INTERCEPT1_VMSAVE = 35,
  SVM_INTERCEPT1_STGI = 36,
  SVM_INTERCEPT1_CLGI = 37,
  SVM_INTERCEPT1_SKINIT = 38,
  SVM_INTERCEPT1_RDTSCP = 39,
  SVM_INTERCEPT1_ICEBP = 40,
  SVM_INTERCEPT1_WBINVD = 41,
  SVM_INTERCEPT1_MONITOR = 42,
  SVM_INTERCEPT1_MWAIT = 43,
  SVM_INTERCEPT1_MWAIT_ARMED = 44,
  SVM_INTERCEPT1_XSETBV = 45,
  SVM_INTERCEPT1_RDPRU = 46,
  SVM_INTERCEPT1_EFER_WRITE_TRAP = 47,
  SVM_INTERCEPT1_CR0_WRITE_TRAP = 48,
  // vector5[31:00]:
  SVM_INTERCEPT2_INVLPGB = 64,
  SVM_INTERCEPT2_INVLPGB_ILLEGAL = 65,
  SVM_INTERCEPT2_INVPCID = 66,
  SVM_INTERCEPT2_MCOMMIT = 67,
  SVM_INTERCEPT2_TLBSYNC = 68
};

#define SVM_INTERCEPT(intercept_bitnum) \
  (BX_CPU_THIS_PTR vmcb->ctrls.intercept_vector[intercept_bitnum / 32] & (1 << (intercept_bitnum & 31)))

#define SVM_EXCEPTION_INTERCEPTED(vector) \
  (BX_CPU_THIS_PTR vmcb->ctrls.exceptions_intercept & (1<<(vector)))

#define SVM_CR_READ_INTERCEPTED(reg_num) \
  (BX_CPU_THIS_PTR vmcb->ctrls.cr_rd_ctrl & (1<<(reg_num)))

#define SVM_CR_WRITE_INTERCEPTED(reg_num) \
  (BX_CPU_THIS_PTR vmcb->ctrls.cr_wr_ctrl & (1<<(reg_num)))

#define SVM_DR_READ_INTERCEPTED(reg_num) \
  (BX_CPU_THIS_PTR vmcb->ctrls.dr_rd_ctrl & (1<<(reg_num)))

#define SVM_DR_WRITE_INTERCEPTED(reg_num) \
  (BX_CPU_THIS_PTR vmcb->ctrls.dr_wr_ctrl & (1<<(reg_num)))

#define SVM_NESTED_PAGING_ENABLED (BX_CPU_THIS_PTR vmcb->ctrls.nested_paging)

#endif // BX_SUPPORT_SVM

#endif // _BX_SVM_AMD_H_
