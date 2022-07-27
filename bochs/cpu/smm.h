/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2006-2009 Stanislav Shwartsman
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_SMM_H
#define BX_SMM_H

/* SMM feature masks */
const Bit32u SMM_IO_INSTRUCTION_RESTART = 0x00010000;
const Bit32u SMM_SMBASE_RELOCATION      = 0x00020000;

#define SMM_SAVE_STATE_MAP_SIZE 128

//
// - For x86-64 configuration using AMD Athlon 64 512-byte SMM save state map
//   revision ID according to QEMU/Bochs BIOS
//
// - For x86-32 configuration using Intel P6 512-byte SMM save state map
//

const Bit32u SMM_REVISION_ID = ((BX_SUPPORT_X86_64 ? 0x00000064 : 0) | SMM_SMBASE_RELOCATION);

//
// Some of the CPU field must be saved and restored in order to continue the
// simulation correctly after the RSM instruction:
//
//      ---------------------------------------------------------------
//
// 1. General purpose registers: EAX-EDI, R8-R15
// 2. EIP, RFLAGS
// 3. Segment registers CS, DS, SS, ES, FS, GS
//    fields: valid      - not required, initialized according to selector value
//            p          - must be saved/restored
//            dpl        - must be saved/restored
//            segment    - must be 1 for seg registers, not required to save
//            type       - must be saved/restored
//            base       - must be saved/restored
//            limit      - must be saved/restored
//            g          - must be saved/restored
//            d_b        - must be saved/restored
//            l          - must be saved/restored
//            avl        - must be saved/restored
// 4. GDTR, IDTR
//     fields: base, limit
// 5. LDTR, TR
//     fields: base, limit, anything else ?
// 6. Debug Registers DR0-DR7, only DR6 and DR7 are saved
// 7. Control Registers: CR0, CR2 is NOT saved, CR3, CR4, EFER
// 8. SMBASE
// 9. MSR/FPU/XMM/APIC are NOT saved accoring to Intel docs
//

struct BX_SMM_State
{
  Bit32u smbase;
  Bit32u smm_revision_id;

  bx_address gen_reg[BX_GENERAL_REGISTERS];

  bx_address rip;
  Bit32u eflags;
  Bit32u dr6;
  Bit32u dr7;

  bx_cr0_t   cr0;
  bx_address cr3;
#if BX_CPU_LEVEL >= 5
  bx_cr4_t   cr4;
  bx_efer_t efer;
#endif

  Bit32u io_insruction_restart;
  Bit32u autohalt_restart;
  Bit32u nmi_mask;

  bx_global_segment_reg_t gdtr;
  bx_global_segment_reg_t idtr;

  struct {
    bx_address base;
    Bit32u limit;
    Bit32u selector_ar;
  } segreg[6], tr, ldtr;
};

#if BX_SUPPORT_X86_64

enum SMMRAM_Fields {
  SMRAM_FIELD_SMBASE_OFFSET = 0,
  SMRAM_FIELD_SMM_REVISION_ID,
  SMRAM_FIELD_RAX_HI32,
  SMRAM_FIELD_EAX,
  SMRAM_FIELD_RCX_HI32,
  SMRAM_FIELD_ECX,
  SMRAM_FIELD_RDX_HI32,
  SMRAM_FIELD_EDX,
  SMRAM_FIELD_RBX_HI32,
  SMRAM_FIELD_EBX,
  SMRAM_FIELD_RSP_HI32,
  SMRAM_FIELD_ESP,
  SMRAM_FIELD_RBP_HI32,
  SMRAM_FIELD_EBP,
  SMRAM_FIELD_RSI_HI32,
  SMRAM_FIELD_ESI,
  SMRAM_FIELD_RDI_HI32,
  SMRAM_FIELD_EDI,
  SMRAM_FIELD_R8_HI32,
  SMRAM_FIELD_R8,
  SMRAM_FIELD_R9_HI32,
  SMRAM_FIELD_R9,
  SMRAM_FIELD_R10_HI32,
  SMRAM_FIELD_R10,
  SMRAM_FIELD_R11_HI32,
  SMRAM_FIELD_R11,
  SMRAM_FIELD_R12_HI32,
  SMRAM_FIELD_R12,
  SMRAM_FIELD_R13_HI32,
  SMRAM_FIELD_R13,
  SMRAM_FIELD_R14_HI32,
  SMRAM_FIELD_R14,
  SMRAM_FIELD_R15_HI32,
  SMRAM_FIELD_R15,
  SMRAM_FIELD_RIP_HI32,
  SMRAM_FIELD_EIP,
  SMRAM_FIELD_RFLAGS_HI32,  // always zero
  SMRAM_FIELD_EFLAGS,
  SMRAM_FIELD_DR6_HI32,     // always zero
  SMRAM_FIELD_DR6,
  SMRAM_FIELD_DR7_HI32,     // always zero
  SMRAM_FIELD_DR7,
  SMRAM_FIELD_CR0_HI32,     // always zero
  SMRAM_FIELD_CR0,
  SMRAM_FIELD_CR3_HI32,     // zero when physical address size 32-bit
  SMRAM_FIELD_CR3,
  SMRAM_FIELD_CR4_HI32,     // always zero
  SMRAM_FIELD_CR4,
  SMRAM_FIELD_EFER_HI32,    // always zero
  SMRAM_FIELD_EFER,
  SMRAM_FIELD_IO_INSTRUCTION_RESTART,
  SMRAM_FIELD_AUTOHALT_RESTART,
  SMRAM_FIELD_NMI_MASK,
  SMRAM_FIELD_TR_BASE_HI32,
  SMRAM_FIELD_TR_BASE,
  SMRAM_FIELD_TR_LIMIT,
  SMRAM_FIELD_TR_SELECTOR_AR,
  SMRAM_FIELD_LDTR_BASE_HI32,
  SMRAM_FIELD_LDTR_BASE,
  SMRAM_FIELD_LDTR_LIMIT,
  SMRAM_FIELD_LDTR_SELECTOR_AR,
  SMRAM_FIELD_IDTR_BASE_HI32,
  SMRAM_FIELD_IDTR_BASE,
  SMRAM_FIELD_IDTR_LIMIT,
  SMRAM_FIELD_GDTR_BASE_HI32,
  SMRAM_FIELD_GDTR_BASE,
  SMRAM_FIELD_GDTR_LIMIT,
  SMRAM_FIELD_ES_BASE_HI32,
  SMRAM_FIELD_ES_BASE,
  SMRAM_FIELD_ES_LIMIT,
  SMRAM_FIELD_ES_SELECTOR_AR,
  SMRAM_FIELD_CS_BASE_HI32,
  SMRAM_FIELD_CS_BASE,
  SMRAM_FIELD_CS_LIMIT,
  SMRAM_FIELD_CS_SELECTOR_AR,
  SMRAM_FIELD_SS_BASE_HI32,
  SMRAM_FIELD_SS_BASE,
  SMRAM_FIELD_SS_LIMIT,
  SMRAM_FIELD_SS_SELECTOR_AR,
  SMRAM_FIELD_DS_BASE_HI32,
  SMRAM_FIELD_DS_BASE,
  SMRAM_FIELD_DS_LIMIT,
  SMRAM_FIELD_DS_SELECTOR_AR,
  SMRAM_FIELD_FS_BASE_HI32,
  SMRAM_FIELD_FS_BASE,
  SMRAM_FIELD_FS_LIMIT,
  SMRAM_FIELD_FS_SELECTOR_AR,
  SMRAM_FIELD_GS_BASE_HI32,
  SMRAM_FIELD_GS_BASE,
  SMRAM_FIELD_GS_LIMIT,
  SMRAM_FIELD_GS_SELECTOR_AR,
  SMRAM_FIELD_LAST
};

#else

enum SMMRAM_Fields {
  SMRAM_FIELD_SMBASE_OFFSET = 0,
  SMRAM_FIELD_SMM_REVISION_ID,
  SMRAM_FIELD_EAX,
  SMRAM_FIELD_ECX,
  SMRAM_FIELD_EDX,
  SMRAM_FIELD_EBX,
  SMRAM_FIELD_ESP,
  SMRAM_FIELD_EBP,
  SMRAM_FIELD_ESI,
  SMRAM_FIELD_EDI,
  SMRAM_FIELD_EIP,
  SMRAM_FIELD_EFLAGS,
  SMRAM_FIELD_DR6,
  SMRAM_FIELD_DR7,
  SMRAM_FIELD_CR0,
  SMRAM_FIELD_CR3,
  SMRAM_FIELD_CR4,
  SMRAM_FIELD_EFER,
  SMRAM_FIELD_IO_INSTRUCTION_RESTART,
  SMRAM_FIELD_AUTOHALT_RESTART,
  SMRAM_FIELD_NMI_MASK,
  SMRAM_FIELD_TR_SELECTOR,
  SMRAM_FIELD_TR_BASE,
  SMRAM_FIELD_TR_LIMIT,
  SMRAM_FIELD_TR_SELECTOR_AR,
  SMRAM_FIELD_LDTR_SELECTOR,
  SMRAM_FIELD_LDTR_BASE,
  SMRAM_FIELD_LDTR_LIMIT,
  SMRAM_FIELD_LDTR_SELECTOR_AR,
  SMRAM_FIELD_IDTR_BASE,
  SMRAM_FIELD_IDTR_LIMIT,
  SMRAM_FIELD_GDTR_BASE,
  SMRAM_FIELD_GDTR_LIMIT,
  SMRAM_FIELD_ES_SELECTOR,
  SMRAM_FIELD_ES_BASE,
  SMRAM_FIELD_ES_LIMIT,
  SMRAM_FIELD_ES_SELECTOR_AR,
  SMRAM_FIELD_CS_SELECTOR,
  SMRAM_FIELD_CS_BASE,
  SMRAM_FIELD_CS_LIMIT,
  SMRAM_FIELD_CS_SELECTOR_AR,
  SMRAM_FIELD_SS_SELECTOR,
  SMRAM_FIELD_SS_BASE,
  SMRAM_FIELD_SS_LIMIT,
  SMRAM_FIELD_SS_SELECTOR_AR,
  SMRAM_FIELD_DS_SELECTOR,
  SMRAM_FIELD_DS_BASE,
  SMRAM_FIELD_DS_LIMIT,
  SMRAM_FIELD_DS_SELECTOR_AR,
  SMRAM_FIELD_FS_SELECTOR,
  SMRAM_FIELD_FS_BASE,
  SMRAM_FIELD_FS_LIMIT,
  SMRAM_FIELD_FS_SELECTOR_AR,
  SMRAM_FIELD_GS_SELECTOR,
  SMRAM_FIELD_GS_BASE,
  SMRAM_FIELD_GS_LIMIT,
  SMRAM_FIELD_GS_SELECTOR_AR,
  SMRAM_FIELD_LAST
};

#endif // BX_SUPPORT_X86_64

#endif
