/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2009-2015 Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "pc_system.h"

#if BX_SUPPORT_VMX

#include "decoder/ia_opcodes.h"

// BX_READ(0) form means nnn(), rm(); BX_WRITE(1) form means rm(), nnn()
Bit32u gen_instruction_info(bxInstruction_c *i, Bit32u reason, bool rw_form)
{
  Bit32u instr_info = 0;

  switch(reason) {
    case VMX_VMEXIT_VMREAD:
    case VMX_VMEXIT_VMWRITE:
#if BX_SUPPORT_VMX >= 2
    case VMX_VMEXIT_GDTR_IDTR_ACCESS:
    case VMX_VMEXIT_LDTR_TR_ACCESS:
    case VMX_VMEXIT_INVEPT:
    case VMX_VMEXIT_INVVPID:
    case VMX_VMEXIT_INVPCID:
#endif
      if (rw_form == BX_WRITE)
        instr_info |= i->dst() << 28;
      else
        instr_info |= i->src() << 28;
      break;

    case VMX_VMEXIT_RDRAND:
    case VMX_VMEXIT_RDSEED:
      // bits 12:11 hold operand size
      if (i->os64L())
        instr_info |= 1 << 12;
      else if (i->as32L())
        instr_info |= 1 << 11;
      break;

    default:
      break;
  }

  // --------------------------------------
  //  instruction information field format
  // --------------------------------------
  //
  // [01:00] | Memory operand scale field (encoded)
  // [02:02] | Undefined
  // [06:03] | Reg1, undefined when memory operand
  // [09:07] | Memory operand address size
  // [10:10] | Memory/Register format (0 - mem, 1 - reg)
  // [14:11] | Reserved
  // [17:15] | Memory operand segment register field
  // [21:18] | Memory operand index field
  // [22:22] | Memory operand index field invalid
  // [26:23] | Memory operand base field
  // [27:27] | Memory operand base field invalid
  // [31:28] | Reg2, if exists
  //
  if (i->modC0()) {
    // reg/reg format
    instr_info |= (1 << 10);
    if (rw_form == BX_WRITE)
      instr_info |= i->src() << 3;
    else
      instr_info |= i->dst() << 3;
  }
  else {
    // memory format
    if (i->as64L())
        instr_info |= 1 << 8;
    else if (i->as32L())
        instr_info |= 1 << 7;

    instr_info |= i->seg() << 15;

    // index field is always initialized because of gather but not always valid
    if (i->sibIndex() != BX_NIL_REGISTER && i->sibIndex() != 4)
        instr_info |= i->sibScale() | (i->sibIndex() << 18);
    else
        instr_info |= 1 << 22; // index invalid

    if (i->sibBase() != BX_NIL_REGISTER)
        instr_info |= i->sibBase() << 23;
    else
        instr_info |= 1 << 27; // base invalid
  }

  return instr_info;
}

void BX_CPP_AttrRegparmN(3) BX_CPU_C::VMexit_Instruction(bxInstruction_c *i, Bit32u reason, bool rw_form)
{
  Bit64u qualification = 0;
  Bit32u instr_info = 0;

  switch(reason) {
    case VMX_VMEXIT_VMREAD:
    case VMX_VMEXIT_VMWRITE:
    case VMX_VMEXIT_VMPTRLD:
    case VMX_VMEXIT_VMPTRST:
    case VMX_VMEXIT_VMCLEAR:
    case VMX_VMEXIT_VMXON:
#if BX_SUPPORT_VMX >= 2
    case VMX_VMEXIT_GDTR_IDTR_ACCESS:
    case VMX_VMEXIT_LDTR_TR_ACCESS:
    case VMX_VMEXIT_INVEPT:
    case VMX_VMEXIT_INVVPID:
    case VMX_VMEXIT_INVPCID:
    case VMX_VMEXIT_XSAVES:
    case VMX_VMEXIT_XRSTORS:
#endif
#if BX_SUPPORT_X86_64
      if (long64_mode()) {
        qualification = (Bit64u) i->displ32s();
        if (i->sibBase() == BX_64BIT_REG_RIP)
          qualification += RIP;
      }
      else
#endif
      {
        qualification = (Bit64u) ((Bit32u) i->displ32s());
        qualification &= i->asize_mask();
      }
      // fall through

    case VMX_VMEXIT_RDRAND:
    case VMX_VMEXIT_RDSEED:
      instr_info = gen_instruction_info(i, reason, rw_form);
      VMwrite32(VMCS_32BIT_VMEXIT_INSTRUCTION_INFO, instr_info);
      break;

    default:
      BX_PANIC(("VMexit_Instruction reason %d", reason));
  }

  VMexit(reason, qualification);
}

void BX_CPU_C::VMexit_PAUSE(void)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_PAUSE_VMEXIT)) {
    VMexit(VMX_VMEXIT_PAUSE, 0);
  }

#if BX_SUPPORT_VMX >= 2
  if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_PAUSE_LOOP_VMEXIT) && CPL == 0) {
    VMX_PLE *ple = &BX_CPU_THIS_PTR vmcs.ple;
    Bit64u currtime = bx_pc_system.time_ticks();
    if ((currtime - ple->last_pause_time) > ple->pause_loop_exiting_gap) {
      ple->first_pause_time = currtime;
    }
    else {
      if ((currtime - ple->first_pause_time) > ple->pause_loop_exiting_window)
        VMexit(VMX_VMEXIT_PAUSE, 0);
    }
    ple->last_pause_time = currtime;
  }
#endif
}

void BX_CPU_C::VMexit_ExtInterrupt(void)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_EXTERNAL_INTERRUPT_VMEXIT)) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    if (! (vm->vmexit_ctrls & VMX_VMEXIT_CTRL1_INTA_ON_VMEXIT)) {
       // interrupt wasn't acknowledged and still pending, interruption info is invalid
       VMwrite32(VMCS_32BIT_VMEXIT_INTERRUPTION_INFO, 0);
       VMexit(VMX_VMEXIT_EXTERNAL_INTERRUPT, 0);
    }
  }
}

void BX_CPU_C::VMexit_Event(unsigned type, unsigned vector, Bit16u errcode, bool errcode_valid, Bit64u qualification)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  bool vmexit = false;
  VMX_vmexit_reason reason = VMX_VMEXIT_EXCEPTION_NMI;

  switch(type) {
    case BX_EXTERNAL_INTERRUPT:
      reason = VMX_VMEXIT_EXTERNAL_INTERRUPT;
      if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_EXTERNAL_INTERRUPT_VMEXIT))
         vmexit = true;
      break;

    case BX_NMI:
      if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_NMI_EXITING))
         vmexit = true;
      break;

    case BX_PRIVILEGED_SOFTWARE_INTERRUPT:
    case BX_SOFTWARE_EXCEPTION:
    case BX_HARDWARE_EXCEPTION:
      BX_ASSERT(vector < BX_CPU_HANDLED_EXCEPTIONS);
      if (vector == BX_PF_EXCEPTION) {
         // page faults are specially treated
         bool err_match = ((errcode & vm->vm_pf_mask) == vm->vm_pf_match);
         bool bitmap = (vm->vm_exceptions_bitmap >> BX_PF_EXCEPTION) & 1;
         vmexit = (err_match == bitmap);
      }
      else {
         vmexit = (vm->vm_exceptions_bitmap >> vector) & 1;
      }
      break;

    case BX_SOFTWARE_INTERRUPT:
      break; // no VMEXIT on software interrupt

    default:
      BX_ERROR(("VMexit_Event: unknown event type %d", type));
  }

  // ----------------------------------------------------
  //              VMExit interruption info
  // ----------------------------------------------------
  // [07:00] | Interrupt/Exception vector
  // [10:08] | Interrupt/Exception type
  // [11:11] | error code pushed to the stack
  // [12:12] | NMI unblocking due to IRET
  // [30:13] | reserved
  // [31:31] | interruption info valid
  //

  if (! vmexit) {
    // record IDT vectoring information 
    vm->idt_vector_error_code = errcode;
    vm->idt_vector_info = vector | (type << 8);
    if (errcode_valid)
      vm->idt_vector_info |= (1 << 11); // error code delivered

    BX_CPU_THIS_PTR nmi_unblocking_iret = false;
    return;
  }

  BX_DEBUG(("VMEXIT: event vector 0x%02x type %d error code=0x%04x", vector, type, errcode));

  // VMEXIT is not considered to occur during event delivery if it results
  // in a double fault exception that causes VMEXIT directly
  if (vector == BX_DF_EXCEPTION)
    BX_CPU_THIS_PTR in_event = false; // clear in_event indication on #DF

  if (vector == BX_DB_EXCEPTION)  {
    // qualifcation for debug exceptions similar to debug_trap field
    qualification = BX_CPU_THIS_PTR debug_trap & 0x0000600f;
  }

  // clear debug_trap field
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR inhibit_mask = 0;

  Bit32u interruption_info = vector | (type << 8);
  if (errcode_valid)
    interruption_info |= (1 << 11); // error code delivered
  interruption_info   |= (1 << 31); // valid

  if (BX_CPU_THIS_PTR nmi_unblocking_iret)
    interruption_info |= (1 << 12);

  VMwrite32(VMCS_32BIT_VMEXIT_INTERRUPTION_INFO, interruption_info);
  VMwrite32(VMCS_32BIT_VMEXIT_INTERRUPTION_ERR_CODE, errcode);

  VMexit(reason, qualification);
}

void BX_CPU_C::VMexit_TripleFault(void)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  // VMEXIT is not considered to occur during event delivery if it results
  // in a triple fault exception (that causes VMEXIT directly)
  BX_CPU_THIS_PTR in_event = false;

  VMexit(VMX_VMEXIT_TRIPLE_FAULT, 0);
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_TaskSwitch(Bit16u tss_selector, unsigned source)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMexit(VMX_VMEXIT_TASK_SWITCH, tss_selector | (source << 30));
}

const Bit32u BX_VMX_LO_MSR_START = 0x00000000;
const Bit32u BX_VMX_LO_MSR_END   = 0x00001FFF;
const Bit32u BX_VMX_HI_MSR_START = 0xC0000000;
const Bit32u BX_VMX_HI_MSR_END   = 0xC0001FFF;

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_MSR(unsigned op, Bit32u msr)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  bool vmexit = false;
  if (! VMEXIT(VMX_VM_EXEC_CTRL2_MSR_BITMAPS)) vmexit = true;
  else {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    Bit8u field;

    if (msr >= BX_VMX_HI_MSR_START) {
       if (msr > BX_VMX_HI_MSR_END) vmexit = true;
       else {
         // check MSR-HI bitmaps
         bx_phy_address pAddr = vm->msr_bitmap_addr + ((msr - BX_VMX_HI_MSR_START) >> 3) + 1024 + ((op == VMX_VMEXIT_RDMSR) ? 0 : 2048);
         access_read_physical(pAddr, 1, &field);
         BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_MSR_BITMAP_ACCESS, &field);
         if (field & (1 << (msr & 7)))
            vmexit = true;
       }
    }
    else {
       if (msr > BX_VMX_LO_MSR_END) vmexit = true;
       else {
         // check MSR-LO bitmaps
         bx_phy_address pAddr = vm->msr_bitmap_addr + (msr >> 3) + ((op == VMX_VMEXIT_RDMSR) ? 0 : 2048);
         access_read_physical(pAddr, 1, &field);
         BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_MSR_BITMAP_ACCESS, &field);
         if (field & (1 << (msr & 7)))
            vmexit = true;
       }
    }
  }

  if (vmexit) {
     BX_DEBUG(("VMEXIT: %sMSR 0x%08x", (op == VMX_VMEXIT_RDMSR) ? "RD" : "WR", msr));
     VMexit(op, 0);
  }
}

enum {
  VMX_VMEXIT_IO_PORTIN       = (1 << 3),
  VMX_VMEXIT_IO_INSTR_STRING = (1 << 4),
  VMX_VMEXIT_IO_INSTR_REP    = (1 << 5),
  VMX_VMEXIT_IO_INSTR_IMM    = (1 << 6)
};

void BX_CPP_AttrRegparmN(3) BX_CPU_C::VMexit_IO(bxInstruction_c *i, unsigned port, unsigned len)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);
  BX_ASSERT(port <= 0xFFFF);

  bool vmexit = false;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_IO_BITMAPS)) {
     // always VMEXIT on port "wrap around" case
     if ((port + len) > 0x10000) vmexit = true;
     else {
        Bit8u bitmap[2];
        bx_phy_address pAddr;

        if ((port & 0x7fff) + len > 0x8000) {
          // special case - the IO access split cross both I/O bitmaps
          pAddr = BX_CPU_THIS_PTR vmcs.io_bitmap_addr[0] + 0xfff;
          access_read_physical(pAddr, 1, &bitmap[0]);
          BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[0]);

          pAddr = BX_CPU_THIS_PTR vmcs.io_bitmap_addr[1];
          access_read_physical(pAddr, 1, &bitmap[1]);
          BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[1]);
        }
        else {
          // access_read_physical cannot read 2 bytes cross 4K boundary :(
          pAddr = BX_CPU_THIS_PTR vmcs.io_bitmap_addr[(port >> 15) & 1] + ((port & 0x7fff) / 8);
          access_read_physical(pAddr, 1, &bitmap[0]);
          BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[0]);

          pAddr++;
          access_read_physical(pAddr, 1, &bitmap[1]);
          BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[1]);
        }

        Bit16u combined_bitmap = bitmap[1];
        combined_bitmap = (combined_bitmap << 8) | bitmap[0];

        unsigned mask = ((1 << len) - 1) << (port & 7);
        if (combined_bitmap & mask) vmexit = true;
     }
  }
  else if (VMEXIT(VMX_VM_EXEC_CTRL2_IO_VMEXIT)) vmexit = true;

  if (vmexit) {
     BX_DEBUG(("VMEXIT: I/O port 0x%04x", port));

     Bit32u qualification = 0;

     switch(i->getIaOpcode()) {
       case BX_IA_IN_ALIb:
       case BX_IA_IN_AXIb:
       case BX_IA_IN_EAXIb:
         qualification = VMX_VMEXIT_IO_PORTIN | VMX_VMEXIT_IO_INSTR_IMM;
         break;

       case BX_IA_OUT_IbAL:
       case BX_IA_OUT_IbAX:
       case BX_IA_OUT_IbEAX:
         qualification = VMX_VMEXIT_IO_INSTR_IMM;
         break;

       case BX_IA_IN_ALDX:
       case BX_IA_IN_AXDX:
       case BX_IA_IN_EAXDX:
         qualification = VMX_VMEXIT_IO_PORTIN; // no immediate
         break;

       case BX_IA_OUT_DXAL:
       case BX_IA_OUT_DXAX:
       case BX_IA_OUT_DXEAX:
         qualification = 0; // PORTOUT, no immediate
         break;

       case BX_IA_REP_INSB_YbDX:
       case BX_IA_REP_INSW_YwDX:
       case BX_IA_REP_INSD_YdDX:
         qualification = VMX_VMEXIT_IO_PORTIN | VMX_VMEXIT_IO_INSTR_STRING;
         if (i->repUsedL())
            qualification |= VMX_VMEXIT_IO_INSTR_REP;
         break;

       case BX_IA_REP_OUTSB_DXXb:
       case BX_IA_REP_OUTSW_DXXw:
       case BX_IA_REP_OUTSD_DXXd:
         qualification = VMX_VMEXIT_IO_INSTR_STRING; // PORTOUT
         if (i->repUsedL())
            qualification |= VMX_VMEXIT_IO_INSTR_REP;
         break;

       default:
         BX_PANIC(("VMexit_IO: I/O instruction %s unknown", i->getIaOpcodeNameShort()));
     }

     if (qualification & VMX_VMEXIT_IO_INSTR_STRING) {
       bx_address asize_mask = (bx_address) i->asize_mask(), laddr;

       if (qualification & VMX_VMEXIT_IO_PORTIN)
          laddr = get_laddr(BX_SEG_REG_ES, RDI & asize_mask);
       else  // PORTOUT
          laddr = get_laddr(i->seg(), RSI & asize_mask);

       VMwrite_natural(VMCS_GUEST_LINEAR_ADDR, laddr);

       Bit32u instruction_info = i->seg() << 15;
       if (i->as64L())
         instruction_info |= (1 << 8);
       else if (i->as32L())
         instruction_info |= (1 << 7);

       VMwrite32(VMCS_32BIT_VMEXIT_INSTRUCTION_INFO, instruction_info);
     }

     VMexit(VMX_VMEXIT_IO_INSTRUCTION, qualification | (len-1) | (port << 16));
  }
}

//
// ----------------------------------------------------------------
//                 Exit qualification for CR access
// ----------------------------------------------------------------
// [03:00] | Number of CR register (CR0, CR3, CR4, CR8)
// [05:04] | CR access type (0 - MOV to CR, 1 - MOV from CR, 2 - CLTS, 3 - LMSW)
// [06:06] | LMSW operand reg/mem (cleared for CR access and CLTS)
// [07:07] | reserved
// [11:08] | Source Operand Register for CR access (cleared for CLTS and LMSW)
// [15:12] | reserved
// [31:16] | LMSW source data (cleared for CR access and CLTS)
// [63:32] | reserved
//

bool BX_CPU_C::VMexit_CLTS(void)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (vm->vm_cr0_mask & vm->vm_cr0_read_shadow & 0x8)
  {
    // all rest of the fields cleared to zero
    Bit64u qualification = VMX_VMEXIT_CR_ACCESS_CLTS << 4;

    VMexit(VMX_VMEXIT_CR_ACCESS, qualification);
  }

  if ((vm->vm_cr0_mask & 0x8) != 0 && (vm->vm_cr0_read_shadow & 0x8) == 0)
    return 1; /* do not clear CR0.TS */
  else
    return 0;
}

Bit32u BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_LMSW(bxInstruction_c *i, Bit32u msw)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  Bit32u mask = vm->vm_cr0_mask & 0xF; /* LMSW affects only low 4 bits */
  bool vmexit = false;

  if ((mask & msw & 0x1) != 0 && (vm->vm_cr0_read_shadow & 0x1) == 0)
    vmexit = true;

  if ((mask & vm->vm_cr0_read_shadow & 0xE) != (mask & msw & 0xE))
    vmexit = true;

  if (vmexit) {
    BX_DEBUG(("VMEXIT: CR0 write by LMSW of value 0x%04x", msw));

    Bit64u qualification = VMX_VMEXIT_CR_ACCESS_LMSW << 4;
    qualification |= msw << 16;
    if (! i->modC0()) {
       qualification |= (1 << 6); // memory operand
       VMwrite_natural(VMCS_GUEST_LINEAR_ADDR, get_laddr(i->seg(), RMAddr(i)));
    }

    VMexit(VMX_VMEXIT_CR_ACCESS, qualification);
  }

  // keep untouched all the bits set in CR0 mask
  return (BX_CPU_THIS_PTR cr0.get32() & mask) | (msw & ~mask);
}

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_CR0_Write(bxInstruction_c *i, bx_address val)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if ((vm->vm_cr0_mask & vm->vm_cr0_read_shadow) != (vm->vm_cr0_mask & val))
  {
    BX_DEBUG(("VMEXIT: CR0 write"));
    Bit64u qualification = i->src() << 8;
    VMexit(VMX_VMEXIT_CR_ACCESS, qualification);
  }

  // keep untouched all the bits set in CR0 mask
  return (BX_CPU_THIS_PTR cr0.get32() & vm->vm_cr0_mask) | (val & ~vm->vm_cr0_mask);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_CR3_Read(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR3_READ_VMEXIT)) {
    BX_DEBUG(("VMEXIT: CR3 read"));
    Bit64u qualification = 3 | (VMX_VMEXIT_CR_ACCESS_CR_READ << 4) | (i->dst() << 8);
    VMexit(VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_CR3_Write(bxInstruction_c *i, bx_address val)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR3_WRITE_VMEXIT)) {
    for (unsigned n=0; n < vm->vm_cr3_target_cnt; n++) {
      if (vm->vm_cr3_target_value[n] == val) return;
    }

    BX_DEBUG(("VMEXIT: CR3 write"));
    Bit64u qualification = 3 | (i->src() << 8);
    VMexit(VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_CR4_Write(bxInstruction_c *i, bx_address val)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if ((vm->vm_cr4_mask & vm->vm_cr4_read_shadow) != (vm->vm_cr4_mask & val))
  {
    BX_DEBUG(("VMEXIT: CR4 write"));
    Bit64u qualification = 4 | (i->src() << 8);
    VMexit(VMX_VMEXIT_CR_ACCESS, qualification);
  }

  // keep untouched all the bits set in CR4 mask
  return (BX_CPU_THIS_PTR cr4.get32() & vm->vm_cr4_mask) | (val & ~vm->vm_cr4_mask);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_CR8_Read(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR8_READ_VMEXIT)) {
    BX_DEBUG(("VMEXIT: CR8 read"));
    Bit64u qualification = 8 | (VMX_VMEXIT_CR_ACCESS_CR_READ << 4) | (i->dst() << 8);
    VMexit(VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_CR8_Write(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR8_WRITE_VMEXIT)) {
    BX_DEBUG(("VMEXIT: CR8 write"));
    Bit64u qualification = 8 | (i->src() << 8);
    VMexit(VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

//
// ----------------------------------------------------------------
//                 Exit qualification for DR access
// ----------------------------------------------------------------
// [03:00] | Number of DR register
// [04:04] | DR access type (0 - MOV to DR, 1 - MOV from DR)
// [07:05] | reserved
// [11:08] | Source Operand Register
// [63:12] | reserved
//

void BX_CPU_C::VMexit_DR_Access(unsigned read, unsigned dr, unsigned reg)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_DRx_ACCESS_VMEXIT))
  {
    BX_DEBUG(("VMEXIT: DR%d %s access", dr, read ? "READ" : "WRITE"));

    Bit64u qualification = dr | (reg << 8);
    if (read)
       qualification |= (1 << 4);

    VMexit(VMX_VMEXIT_DR_ACCESS, qualification);
  }
}

#if BX_SUPPORT_VMX >= 2
Bit16u BX_CPU_C::VMX_Get_Current_VPID(void)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest || !SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VPID_ENABLE))
    return 0;

  return BX_CPU_THIS_PTR vmcs.vpid;
}
#endif

#if BX_SUPPORT_VMX >= 2
bool BX_CPP_AttrRegparmN(1) BX_CPU_C::Vmexit_Vmread(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VMCS_SHADOWING)) return true;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    if (BX_READ_64BIT_REG_HIGH(i->src())) return true;
  }
#endif
  unsigned encoding = BX_READ_32BIT_REG(i->src());
  if (encoding > 0x7fff) return true;

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  Bit8u bitmap;
  bx_phy_address pAddr = vm->vmread_bitmap_addr | (encoding >> 3);
  access_read_physical(pAddr, 1, &bitmap);
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_VMREAD_BITMAP_ACCESS, &bitmap);
  
  if (bitmap & (1 << (encoding & 7)))
    return true;

  return false;
}

bool BX_CPP_AttrRegparmN(1) BX_CPU_C::Vmexit_Vmwrite(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VMCS_SHADOWING)) return true;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    if (BX_READ_64BIT_REG_HIGH(i->dst())) return true;
  }
#endif
  unsigned encoding = BX_READ_32BIT_REG(i->dst());
  if (encoding > 0x7fff) return true;

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  Bit8u bitmap;
  bx_phy_address pAddr = vm->vmwrite_bitmap_addr | (encoding >> 3);
  access_read_physical(pAddr, 1, &bitmap);
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_VMWRITE_BITMAP_ACCESS, &bitmap);
  
  if (bitmap & (1 << (encoding & 7)))
    return true;

  return false;
}

void BX_CPU_C::Virtualization_Exception(Bit64u qualification, Bit64u guest_physical, Bit64u guest_linear)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  // A convertible EPT violation causes a virtualization exception if the following all hold:
  //  - CR0.PE is set
  //  - the logical processor is not in the process of delivering an event through the IDT
  //  - the 32 bits at offset 4 in the virtualization-exception information area are all 0

  if (! BX_CPU_THIS_PTR cr0.get_PE() || BX_CPU_THIS_PTR in_event) return;

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  Bit32u magic;
  access_read_physical(vm->ve_info_addr + 4, 4, &magic);
#if BX_SUPPORT_MEMTYPE
  BxMemtype ve_info_memtype = resolve_memtype(vm->ve_info_addr);
#endif
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->ve_info_addr + 4, 4, MEMTYPE(ve_info_memtype), BX_READ, 0, (Bit8u*)(&magic));
  if (magic != 0) return;

  struct ve_info {
    Bit32u reason; // always VMX_VMEXIT_EPT_VIOLATION
    Bit32u magic;
    Bit64u qualification;
    Bit64u guest_linear_addr;
    Bit64u guest_physical_addr;
    Bit16u eptp_index;
  } ve_info = { VMX_VMEXIT_EPT_VIOLATION, 0xffffffff, qualification, guest_linear, guest_physical, vm->eptp_index };

  access_write_physical(vm->ve_info_addr, 4, &ve_info.reason);
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->ve_info_addr, 4, MEMTYPE(ve_info_memtype), BX_WRITE, 0, (Bit8u*)(&ve_info.reason));

  access_write_physical(vm->ve_info_addr + 4, 4, &ve_info.magic);
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->ve_info_addr + 4, 4, MEMTYPE(ve_info_memtype), BX_WRITE, 0, (Bit8u*)(&ve_info.magic));

  access_write_physical(vm->ve_info_addr + 8, 8, &ve_info.qualification);
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->ve_info_addr + 8, 8, MEMTYPE(ve_info_memtype), BX_WRITE, 0, (Bit8u*)(&ve_info.qualification));

  access_write_physical(vm->ve_info_addr + 16, 8, &ve_info.guest_linear_addr);
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->ve_info_addr + 16, 8, MEMTYPE(ve_info_memtype), BX_WRITE, 0, (Bit8u*)(&ve_info.guest_linear_addr));

  access_write_physical(vm->ve_info_addr + 24, 8, &ve_info.guest_physical_addr);
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->ve_info_addr + 24, 8, MEMTYPE(ve_info_memtype), BX_WRITE, 0, (Bit8u*)(&ve_info.guest_physical_addr));

  access_write_physical(vm->ve_info_addr + 32, 8, &ve_info.eptp_index);
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->ve_info_addr + 32, 8, MEMTYPE(ve_info_memtype), BX_WRITE, 0, (Bit8u*)(&ve_info.eptp_index));

  exception(BX_VE_EXCEPTION, 0);
}

void BX_CPU_C::vmx_page_modification_logging(Bit64u guest_paddr, unsigned dirty_update)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (vm->pml_index >= 512) {
    Bit32u vmexit_qualification = 0;
    if (BX_CPU_THIS_PTR nmi_unblocking_iret)
      vmexit_qualification |= (1 << 12);

    VMexit(VMX_VMEXIT_PML_LOGFULL, vmexit_qualification);
  }

  if (dirty_update) {
    Bit64u pAddr = vm->pml_address + 8 * vm->pml_index;
    access_write_physical(pAddr, 8, &guest_paddr);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(resolve_memtype(pAddr)), BX_WRITE, BX_VMX_PML_WRITE, (Bit8u*)(&guest_paddr));
    vm->pml_index--;
  }
}
#endif

#endif // BX_SUPPORT_VMX
