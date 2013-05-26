/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2013 Stanislav Shwartsman
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
   SVM_VMEXIT_CR0_READ  = 0,
   SVM_VMEXIT_CR2_READ  = 2,
   SVM_VMEXIT_CR3_READ  = 3,
   SVM_VMEXIT_CR4_READ  = 4,
   SVM_VMEXIT_CR8_READ  = 8,
   SVM_VMEXIT_CR0_WRITE = 16,
   SVM_VMEXIT_CR2_WRITE = 18,
   SVM_VMEXIT_CR3_WRITE = 19,
   SVM_VMEXIT_CR4_WRITE = 20,
   SVM_VMEXIT_CR8_WRITE = 24,
   SVM_VMEXIT_DR0_READ  = 32,
   SVM_VMEXIT_DR0_WRITE = 48,
   SVM_VMEXIT_EXCEPTION = 64,
   SVM_VMEXIT_PF_EXCEPTION = (64+BX_PF_EXCEPTION),
   SVM_VMEXIT_INTR = 96,
   SVM_VMEXIT_NMI = 97,
   SVM_VMEXIT_SMI = 98,
   SVM_VMEXIT_INIT = 99,
   SVM_VMEXIT_VINTR = 100,
   SVM_VMEXIT_CR0_SEL_WRITE = 101,
   SVM_VMEXIT_IDTR_READ = 102,
   SVM_VMEXIT_GDTR_READ = 103,
   SVM_VMEXIT_LDTR_READ = 104,
   SVM_VMEXIT_TR_READ = 105,
   SVM_VMEXIT_IDTR_WRITE = 106,
   SVM_VMEXIT_GDTR_WRITE = 107,
   SVM_VMEXIT_LDTR_WRITE = 108,
   SVM_VMEXIT_TR_WRITE = 109,
   SVM_VMEXIT_RDTSC = 110,
   SVM_VMEXIT_RDPMC = 111,
   SVM_VMEXIT_PUSHF = 112,
   SVM_VMEXIT_POPF = 113,
   SVM_VMEXIT_CPUID = 114,
   SVM_VMEXIT_RSM = 115,
   SVM_VMEXIT_IRET = 116,
   SVM_VMEXIT_SOFTWARE_INTERRUPT = 117,
   SVM_VMEXIT_INVD = 118,
   SVM_VMEXIT_PAUSE = 119,
   SVM_VMEXIT_HLT = 120,
   SVM_VMEXIT_INVLPG = 121,
   SVM_VMEXIT_INVLPGA = 122,
   SVM_VMEXIT_IO = 123,
   SVM_VMEXIT_MSR = 124,
   SVM_VMEXIT_TASK_SWITCH = 125,
   SVM_VMEXIT_FERR_FREEZE = 126,
   SVM_VMEXIT_SHUTDOWN = 127,
   SVM_VMEXIT_VMRUN = 128,
   SVM_VMEXIT_VMMCALL = 129,
   SVM_VMEXIT_VMLOAD = 130,
   SVM_VMEXIT_VMSAVE = 131,
   SVM_VMEXIT_STGI = 132,
   SVM_VMEXIT_CLGI = 133,
   SVM_VMEXIT_SKINIT = 134,
   SVM_VMEXIT_RDTSCP = 135,
   SVM_VMEXIT_ICEBP = 136,
   SVM_VMEXIT_WBINVD = 137,
   SVM_VMEXIT_MONITOR = 138,
   SVM_VMEXIT_MWAIT = 139,
   SVM_VMEXIT_MWAIT_CONDITIONAL = 140,
   SVM_VMEXIT_XSETBV = 141,
   SVM_VMEXIT_NPF = 1024,
   SVM_VMEXIT_AVIC_INCOMPLETE_IPI = 1025,
   SVM_VMEXIT_AVIC_NOACCEL = 1026
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

#define SVM_GUEST_CPL                           (0x4cb)
#define SVM_GUEST_EFER_MSR                      (0x4d0)
#define SVM_GUEST_EFER_MSR_HI                   (0x4d4)

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
#define SVM_GUEST_RAX                           (0x5f8)
#define SVM_GUEST_STAR_MSR                      (0x600)
#define SVM_GUEST_LSTAR_MSR                     (0x608)
#define SVM_GUEST_CSTAR_MSR                     (0x610)
#define SVM_GUEST_SFMASK_MSR                    (0x618)
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

typedef struct bx_SVM_HOST_STATE
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

} SVM_HOST_STATE;

typedef struct bx_SVM_GUEST_STATE
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
  Bit32u eflags;
  Bit64u rip;
  Bit64u rsp;
  Bit64u rax;

  unsigned cpl;

  bx_bool inhibit_interrupts;

} SVM_GUEST_STATE;

typedef struct bx_SVM_CONTROLS
{
  Bit16u cr_rd_ctrl;
  Bit16u cr_wr_ctrl;
  Bit16u dr_rd_ctrl;
  Bit16u dr_wr_ctrl;
  Bit32u exceptions_intercept;

  Bit32u intercept_vector[2];

  Bit32u exitintinfo;
  Bit32u exitintinfo_error_code;

  Bit32u eventinj;

  bx_phy_address iopm_base;
  bx_phy_address msrpm_base;

  Bit8u v_tpr;
  Bit8u v_intr_prio;
  bx_bool v_ignore_tpr;
  bx_bool v_intr_masking;
  Bit8u v_intr_vector;

  bx_bool nested_paging;
  Bit64u ncr3;

  Bit16u pause_filter_count;
//Bit16u pause_filter_threshold;

} SVM_CONTROLS;

#if defined(NEED_CPU_REG_SHORTCUTS)

#define SVM_V_TPR          (BX_CPU_THIS_PTR vmcb.ctrls.v_tpr)
#define SVM_V_INTR_PRIO    (BX_CPU_THIS_PTR vmcb.ctrls.v_intr_prio)
#define SVM_V_IGNORE_TPR   (BX_CPU_THIS_PTR vmcb.ctrls.v_ignore_tpr)
#define SVM_V_INTR_MASKING (BX_CPU_THIS_PTR vmcb.ctrls.v_intr_masking)
#define SVM_V_INTR_VECTOR  (BX_CPU_THIS_PTR vmcb.ctrls.v_intr_vector)

#define SVM_HOST_IF (BX_CPU_THIS_PTR vmcb.host_state.eflags & EFlagsIFMask)

#endif

typedef struct bx_VMCB_CACHE
{
  SVM_HOST_STATE host_state;
  SVM_CONTROLS ctrls;
} VMCB_CACHE;

// ========================
//  SVM intercept controls
// ========================

#define SVM_INTERCEPT0_INTR               (0)
#define SVM_INTERCEPT0_NMI                (1)
#define SVM_INTERCEPT0_SMI                (2)
#define SVM_INTERCEPT0_INIT               (3)
#define SVM_INTERCEPT0_VINTR              (4)
#define SVM_INTERCEPT0_CR0_WRITE_NO_TS_MP (5)
#define SVM_INTERCEPT0_IDTR_READ          (6)
#define SVM_INTERCEPT0_GDTR_READ          (7)
#define SVM_INTERCEPT0_LDTR_READ          (8)
#define SVM_INTERCEPT0_TR_READ            (9)
#define SVM_INTERCEPT0_IDTR_WRITE         (10)
#define SVM_INTERCEPT0_GDTR_WRITE         (11)
#define SVM_INTERCEPT0_LDTR_WRITE         (12)
#define SVM_INTERCEPT0_TR_WRITE           (13)
#define SVM_INTERCEPT0_RDTSC              (14)
#define SVM_INTERCEPT0_RDPMC              (15)
#define SVM_INTERCEPT0_PUSHF              (16)
#define SVM_INTERCEPT0_POPF               (17)
#define SVM_INTERCEPT0_CPUID              (18)
#define SVM_INTERCEPT0_RSM                (19)
#define SVM_INTERCEPT0_IRET               (20)
#define SVM_INTERCEPT0_SOFTINT            (21)
#define SVM_INTERCEPT0_INVD               (22)
#define SVM_INTERCEPT0_PAUSE              (23)
#define SVM_INTERCEPT0_HLT                (24)
#define SVM_INTERCEPT0_INVLPG             (25)
#define SVM_INTERCEPT0_INVLPGA            (26)
#define SVM_INTERCEPT0_IO                 (27)
#define SVM_INTERCEPT0_MSR                (28)
#define SVM_INTERCEPT0_TASK_SWITCH        (29)
#define SVM_INTERCEPT0_FERR_FREEZE        (30)
#define SVM_INTERCEPT0_SHUTDOWN           (31)

#define SVM_INTERCEPT1_VMRUN              (32)
#define SVM_INTERCEPT1_VMMCALL            (33)
#define SVM_INTERCEPT1_VMLOAD             (34)
#define SVM_INTERCEPT1_VMSAVE             (35)
#define SVM_INTERCEPT1_STGI               (36)
#define SVM_INTERCEPT1_CLGI               (37)
#define SVM_INTERCEPT1_SKINIT             (38)
#define SVM_INTERCEPT1_RDTSCP             (39)
#define SVM_INTERCEPT1_ICEBP              (40)
#define SVM_INTERCEPT1_WBINVD             (41)
#define SVM_INTERCEPT1_MONITOR            (42)
#define SVM_INTERCEPT1_MWAIT              (43)
#define SVM_INTERCEPT1_MWAIT_ARMED        (44)
#define SVM_INTERCEPT1_XSETBV             (45)

#define SVM_INTERCEPT(intercept_bitnum) \
  (BX_CPU_THIS_PTR vmcb.ctrls.intercept_vector[intercept_bitnum / 32] & (1 << (intercept_bitnum & 31)))

#define SVM_EXCEPTION_INTERCEPTED(vector) \
  (BX_CPU_THIS_PTR vmcb.ctrls.exceptions_intercept & (1<<(vector)))

#define SVM_CR_READ_INTERCEPTED(reg_num) \
  (BX_CPU_THIS_PTR vmcb.ctrls.cr_rd_ctrl & (1<<(reg_num)))

#define SVM_CR_WRITE_INTERCEPTED(reg_num) \
  (BX_CPU_THIS_PTR vmcb.ctrls.cr_wr_ctrl & (1<<(reg_num)))

#define SVM_DR_READ_INTERCEPTED(reg_num) \
  (BX_CPU_THIS_PTR vmcb.ctrls.dr_rd_ctrl & (1<<(reg_num)))

#define SVM_DR_WRITE_INTERCEPTED(reg_num) \
  (BX_CPU_THIS_PTR vmcb.ctrls.dr_wr_ctrl & (1<<(reg_num)))

#define SVM_NESTED_PAGING_ENABLED (BX_CPU_THIS_PTR vmcb.ctrls.nested_paging)

#endif // BX_SUPPORT_SVM

#endif // _BX_SVM_AMD_H_
