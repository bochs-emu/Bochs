/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2009-2012 Stanislav Shwartsman
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

#if BX_SUPPORT_VMX

Bit32u gen_instruction_info(bxInstruction_c *i, Bit32u reason)
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
      instr_info |= i->nnn() << 28;
      break;

    default:
      break;
  }

  // --------------------------------------
  //  instruction information field format
  // --------------------------------------
  //
  // [.2:.0] | Memory operand scale field (encoded)
  // [.6:.3] | Reg1, undefined when memory operand
  // [.9:.7] | Memory operand address size
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
    instr_info |= (1 << 10) | (i->rm() << 3);
  }
  else {
    // memory format
    if (i->as64L())
        instr_info |= 1 << 8;
    else if (i->as32L())
        instr_info |= 1 << 7;

    instr_info |= i->seg() << 15;

    if (i->sibIndex() != BX_NIL_REGISTER)
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

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_Instruction(bxInstruction_c *i, Bit32u reason)
{
  Bit64u qualification = 0;
  Bit32u instr_info = 0;

  switch(reason) {
    case VMX_VMEXIT_VMCALL:
    case VMX_VMEXIT_VMLAUNCH:
    case VMX_VMEXIT_VMRESUME:
    case VMX_VMEXIT_VMXOFF:
#if BX_SUPPORT_VMX >= 2
    case VMX_VMEXIT_VMFUNC:
#endif
      // do not have VMEXIT instruction info
      break;
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
#endif
#if BX_SUPPORT_X86_64
      if (long64_mode()) {
        qualification = (Bit64u) i->displ32s();
        if (i->sibBase() == BX_64BIT_REG_RIP)
          qualification += RIP;
      }
      else
#endif
        qualification = (Bit64u) ((Bit32u) i->displ32s());

      instr_info = gen_instruction_info(i, reason);
      VMwrite32(VMCS_32BIT_VMEXIT_INSTRUCTION_INFO, instr_info);
      break;

    default:
      BX_PANIC(("VMexit_Instruction reason %d", reason));
  }

  VMexit(i, reason, qualification);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_PAUSE(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_PAUSE_VMEXIT)) {
    BX_ERROR(("VMEXIT: PAUSE"));
    VMexit(i, VMX_VMEXIT_PAUSE, 0);
  }

#if BX_SUPPORT_VMX >= 2
  if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_PAUSE_LOOP_VMEXIT) && CPL == 0) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    Bit64u currtime = bx_pc_system.time_ticks();
    if ((currtime - vm->last_pause_time) > vm->pause_loop_exiting_gap) {
      vm->first_pause_time = currtime;
    }
    else {
      if ((currtime - vm->first_pause_time) > vm->pause_loop_exiting_window)
        VMexit(i, VMX_VMEXIT_PAUSE, 0);
    }
    vm->last_pause_time = currtime;
  }
#endif
}

void BX_CPU_C::VMexit_ExtInterrupt(void)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_EXTERNAL_INTERRUPT_VMEXIT)) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    if (! (vm->vmexit_ctrls & VMX_VMEXIT_CTRL1_INTA_ON_VMEXIT)) {
       // interrupt wasn't acknowledged and still pending, interruption info is invalid
       VMwrite32(VMCS_32BIT_VMEXIT_INTERRUPTION_INFO, 0);
       VMexit(0, VMX_VMEXIT_EXTERNAL_INTERRUPT, 0);
    }
  }
}

#if BX_SUPPORT_VMX >= 2  
void BX_CPU_C::VMexit_PreemptionTimerExpired(void)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_VMX_PREEMPTION_TIMER_VMEXIT)) {
    BX_DEBUG(("VMEXIT: VMX Preemption Timer Expired"));
    VMexit(0, VMX_VMEXIT_VMX_PREEMPTION_TIMER_EXPIRED, 0);
  }
}
#endif

void BX_CPU_C::VMexit_Event(bxInstruction_c *i, unsigned type, unsigned vector, Bit16u errcode, bx_bool errcode_valid, Bit64u qualification)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  bx_bool vmexit = 0;
  VMX_vmexit_reason reason = VMX_VMEXIT_EXCEPTION_NMI;

  switch(type) {
    case BX_EXTERNAL_INTERRUPT:
      reason = VMX_VMEXIT_EXTERNAL_INTERRUPT;
      if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_EXTERNAL_INTERRUPT_VMEXIT))
         vmexit = 1;
      break;

    case BX_NMI:
      if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_NMI_VMEXIT))
         vmexit = 1;
      break;

    case BX_PRIVILEGED_SOFTWARE_INTERRUPT:
    case BX_SOFTWARE_EXCEPTION:
    case BX_HARDWARE_EXCEPTION:
      BX_ASSERT(vector < BX_CPU_HANDLED_EXCEPTIONS);
      if (vector == BX_PF_EXCEPTION) {
         // page faults are specially treated
         bx_bool err_match = ((errcode & vm->vm_pf_mask) == vm->vm_pf_match);
         bx_bool bitmap = (vm->vm_exceptions_bitmap >> BX_PF_EXCEPTION) & 1;
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
  // [.7:.0] | Interrupt/Exception vector
  // [10:.8] | Interrupt/Exception type
  // [11:11] | error code pushed to the stack
  // [12:12] | NMI unblocking due to IRET
  // [30:13] | reserved
  // [31:31] | interruption info valid
  //

  if (i) {
    VMwrite32(VMCS_32BIT_VMEXIT_INSTRUCTION_LENGTH, i->ilen());
  }

  if (! vmexit) {
    // record IDT vectoring information 
    vm->idt_vector_error_code = errcode;
    vm->idt_vector_info = vector | (type << 8);
    if (errcode_valid)
      vm->idt_vector_info |= (1 << 11); // error code delivered
    return;
  }

  BX_ERROR(("VMEXIT: event vector 0x%02x type %d error code=0x%04x", vector, type, errcode));

  // VMEXIT is not considered to occur during event delivery if it results
  // in a double fault exception that causes VMEXIT directly
  if (vector == BX_DF_EXCEPTION)
    BX_CPU_THIS_PTR in_event = 0; // clear in_event indication on #DF

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

  VMwrite32(VMCS_32BIT_VMEXIT_INTERRUPTION_INFO, interruption_info);
  VMwrite32(VMCS_32BIT_VMEXIT_INTERRUPTION_ERR_CODE, errcode);

  VMexit(0, reason, qualification);
}

void BX_CPU_C::VMexit_TripleFault(void)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  BX_ERROR(("VMEXIT: triple fault"));

  // VMEXIT is not considered to occur during event delivery if it results
  // in a triple fault exception (that causes VMEXIT directly)
  BX_CPU_THIS_PTR in_event = 0;

  VMexit(0, VMX_VMEXIT_TRIPLE_FAULT, 0);
}

void BX_CPP_AttrRegparmN(3) BX_CPU_C::VMexit_TaskSwitch(bxInstruction_c *i, Bit16u tss_selector, unsigned source)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  BX_ERROR(("VMEXIT: task switch"));

  VMexit(i, VMX_VMEXIT_TASK_SWITCH, tss_selector | (source << 30));
}

#define BX_VMX_LO_MSR_START  0x00000000
#define BX_VMX_LO_MSR_END    0x00001FFF
#define BX_VMX_HI_MSR_START  0xC0000000
#define BX_VMX_HI_MSR_END    0xC0001FFF

void BX_CPP_AttrRegparmN(3) BX_CPU_C::VMexit_MSR(bxInstruction_c *i, unsigned op, Bit32u msr)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  bx_bool vmexit = 0;
  if (! VMEXIT(VMX_VM_EXEC_CTRL2_MSR_BITMAPS)) vmexit = 1;
  else {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    Bit8u field;

    if (msr >= BX_VMX_HI_MSR_START) {
       if (msr > BX_VMX_HI_MSR_END) vmexit = 1;
       else {
         // check MSR-HI bitmaps
         bx_phy_address pAddr = vm->msr_bitmap_addr + ((msr - BX_VMX_HI_MSR_START) >> 3) + 1024 + ((op == VMX_VMEXIT_RDMSR) ? 0 : 2048);
         access_read_physical(pAddr, 1, &field);
         BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, BX_MSR_BITMAP_ACCESS, &field);
         if (field & (1 << (msr & 7)))
            vmexit = 1;
       }
    }
    else {
       if (msr > BX_VMX_LO_MSR_END) vmexit = 1;
       else {
         // check MSR-LO bitmaps
         bx_phy_address pAddr = vm->msr_bitmap_addr + (msr >> 3) + ((op == VMX_VMEXIT_RDMSR) ? 0 : 2048);
         access_read_physical(pAddr, 1, &field);
         BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, BX_MSR_BITMAP_ACCESS, &field);
         if (field & (1 << (msr & 7)))
            vmexit = 1;
       }
    }
  }

  if (vmexit) {
     BX_ERROR(("VMEXIT: %sMSR 0x%08x", (op == VMX_VMEXIT_RDMSR) ? "RD" : "WR", msr));
     VMexit(i, op, 0);
  }
}

#define VMX_VMEXIT_IO_PORTIN        (1 << 3)
#define VMX_VMEXIT_IO_INSTR_STRING  (1 << 4)
#define VMX_VMEXIT_IO_INSTR_REP     (1 << 5)
#define VMX_VMEXIT_IO_INSTR_IMM     (1 << 6)

void BX_CPP_AttrRegparmN(3) BX_CPU_C::VMexit_IO(bxInstruction_c *i, unsigned port, unsigned len)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);
  BX_ASSERT(port <= 0xFFFF);

  bool vmexit = 0;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_IO_BITMAPS)) {
     // always VMEXIT on port "wrap around" case
     if ((port + len) > 0x10000) vmexit = 1;
     else {
        Bit8u bitmap[2];
        bx_phy_address pAddr;

        if ((port & 0x7fff) + len > 0x8000) {
          // special case - the IO access split cross both I/O bitmaps
          pAddr = BX_CPU_THIS_PTR vmcs.io_bitmap_addr[0] + 0xfff;
          access_read_physical(pAddr, 1, &bitmap[0]);
          BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[0]);

          pAddr = BX_CPU_THIS_PTR vmcs.io_bitmap_addr[1];
          access_read_physical(pAddr, 1, &bitmap[1]);
          BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[1]);
        }
        else {
          // access_read_physical cannot read 2 bytes cross 4K boundary :(
          pAddr = BX_CPU_THIS_PTR vmcs.io_bitmap_addr[(port >> 15) & 1] + ((port & 0x7fff) / 8);
          access_read_physical(pAddr, 1, &bitmap[0]);
          BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[0]);

          pAddr++;
          access_read_physical(pAddr, 1, &bitmap[1]);
          BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[1]);
        }

        Bit16u combined_bitmap = bitmap[1];
        combined_bitmap = (combined_bitmap << 8) | bitmap[0];

        unsigned mask = ((1 << len) - 1) << (port & 7);
        if (combined_bitmap & mask) vmexit = 1;
     }
  }
  else if (VMEXIT(VMX_VM_EXEC_CTRL2_IO_VMEXIT)) vmexit = 1;

  if (vmexit) {
     BX_ERROR(("VMEXIT: I/O port 0x%04x", port));

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
         BX_PANIC(("VMexit_IO: I/O instruction %s unknown", i->getIaOpcodeName()));
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

     VMexit(i, VMX_VMEXIT_IO_INSTRUCTION, qualification | (len-1) | (port << 16));
  }
}

//
// ----------------------------------------------------------------
//                 Exit qualification for CR access
// ----------------------------------------------------------------
// [.3:.0] | Number of CR register (CR0, CR3, CR4, CR8)
// [.5:.4] | CR access type (0 - MOV to CR, 1 - MOV from CR, 2 - CLTS, 3 - LMSW)
// [.6:.6] | LMSW operand reg/mem (cleared for CR access and CLTS)
// [.7:.7] | reserved
// [11:.8] | Source Operand Register for CR access (cleared for CLTS and LMSW)
// [15:12] | reserved
// [31:16] | LMSW source data (cleared for CR access and CLTS)
// [63:32] | reserved
//

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_CLTS(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (vm->vm_cr0_mask & vm->vm_cr0_read_shadow & 0x8)
  {
    BX_ERROR(("VMEXIT: CLTS"));

    // all rest of the fields cleared to zero
    Bit64u qualification = VMX_VMEXIT_CR_ACCESS_CLTS << 4;

    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
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
  bx_bool vmexit = 0;

  if ((mask & msw & 0x1) != 0 && (vm->vm_cr0_read_shadow & 0x1) == 0)
    vmexit = 1;

  if ((mask & vm->vm_cr0_read_shadow & 0xE) != (mask & msw & 0xE))
    vmexit = 1;

  if (vmexit) {
    BX_ERROR(("VMEXIT: CR0 write by LMSW of value 0x%04x", msw));

    Bit64u qualification = VMX_VMEXIT_CR_ACCESS_LMSW << 4;
    qualification |= msw << 16;
    if (! i->modC0()) {
       qualification |= (1 << 6); // memory operand
       VMwrite_natural(VMCS_GUEST_LINEAR_ADDR, get_laddr(i->seg(), RMAddr(i)));
    }

    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
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
    BX_ERROR(("VMEXIT: CR0 write"));
    Bit64u qualification = i->rm() << 8;
    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
  }

  // keep untouched all the bits set in CR0 mask
  return (BX_CPU_THIS_PTR cr0.get32() & vm->vm_cr0_mask) | (val & ~vm->vm_cr0_mask);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_CR3_Read(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR3_READ_VMEXIT)) {
    BX_ERROR(("VMEXIT: CR3 read"));

    Bit64u qualification = 3 | (VMX_VMEXIT_CR_ACCESS_CR_READ << 4);
    qualification |= (i->rm() << 8);

    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
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

    BX_ERROR(("VMEXIT: CR3 write"));
    Bit64u qualification = 3 | (i->rm() << 8);
    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_CR4_Write(bxInstruction_c *i, bx_address val)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if ((vm->vm_cr4_mask & vm->vm_cr4_read_shadow) != (vm->vm_cr4_mask & val))
  {
    BX_ERROR(("VMEXIT: CR4 write"));
    Bit64u qualification = 4 | (i->rm() << 8);
    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
  }

  // keep untouched all the bits set in CR4 mask
  return (BX_CPU_THIS_PTR cr4.get32() & vm->vm_cr4_mask) | (val & ~vm->vm_cr4_mask);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_CR8_Read(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR8_READ_VMEXIT)) {
    BX_ERROR(("VMEXIT: CR8 read"));

    Bit64u qualification = 8 | (VMX_VMEXIT_CR_ACCESS_CR_READ << 4);
    qualification |= (i->rm() << 8);

    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_CR8_Write(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR8_WRITE_VMEXIT)) {
    BX_ERROR(("VMEXIT: CR8 write"));
    Bit64u qualification = 8 | (i->rm() << 8);
    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

//
// ----------------------------------------------------------------
//                 Exit qualification for DR access
// ----------------------------------------------------------------
// [.3:.0] | Number of DR register
// [.4:.4] | DR access type (0 - MOV to DR, 1 - MOV from DR)
// [.7:.5] | reserved
// [11:.8] | Source Operand Register
// [63:12] | reserved
//

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_DR_Access(bxInstruction_c *i, unsigned read)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_DRx_ACCESS_VMEXIT))
  {
    BX_ERROR(("VMEXIT: DR%d %s access", i->nnn(), read ? "READ" : "WRITE"));

    Bit64u qualification = i->nnn() | (i->rm() << 8);
    if (read)
       qualification |= (1 << 4);

    VMexit(i, VMX_VMEXIT_DR_ACCESS, qualification);
  }
}

#if BX_SUPPORT_X86_64
Bit32u BX_CPU_C::VMX_Read_VTPR(void)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr + 0x80;
  Bit32u vtpr;
  access_read_physical(pAddr, 4, (Bit8u*)(&vtpr));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 4, BX_READ, BX_VMX_VTPR_ACCESS, (Bit8u*)(&vtpr));
  return vtpr;
}

void BX_CPU_C::VMX_Write_VTPR(Bit8u vtpr)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  bx_phy_address pAddr = vm->virtual_apic_page_addr + 0x80;
  Bit32u field32 = vtpr;

  access_write_physical(pAddr, 4, (Bit8u*)(&field32));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 4, BX_WRITE, BX_VMX_VTPR_ACCESS, (Bit8u*)(&field32));

  Bit8u tpr_shadow = vtpr >> 4;
  if (tpr_shadow < vm->vm_tpr_threshold) {
    // commit current instruction to produce trap-like VMexit
    BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
    VMexit(0, VMX_VMEXIT_TPR_THRESHOLD, 0);
  }
}

// apic virtualization
bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::is_virtual_apic_page(bx_phy_address paddr)
{
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES))
      if (PPFOf(paddr) == PPFOf(vm->apic_access_page)) return 1;
  }

  return 0;
}

// apic virtualization
void BX_CPU_C::VMX_Virtual_Apic_Read(bx_phy_address paddr, unsigned len, void *data)
{
  BX_ASSERT(SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES));

  Bit32u offset = PAGE_OFFSET(paddr);

  // access is not instruction fetch because cpu::prefetch will crash them
  if (VMEXIT(VMX_VM_EXEC_CTRL2_TPR_SHADOW) && offset == 0x80 && len <= 4) {
     // VTPR access
     Bit32u vtpr = VMX_Read_VTPR();
     if (len == 1)
       *((Bit8u *) data) = vtpr & 0xff;
     else if (len == 2)
       *((Bit16u *) data) = vtpr & 0xffff;
     else if (len == 4)
       *((Bit32u *) data) = vtpr;
     else
       BX_PANIC(("PANIC: Unsupported Virtual APIC access len = 3 !"));
     return;
  }

  Bit32u qualification = offset | 
      (BX_CPU_THIS_PTR in_event) ? VMX_APIC_ACCESS_DURING_EVENT_DELIVERY : VMX_APIC_READ_INSTRUCTION_EXECUTION;
  VMexit(0, VMX_VMEXIT_APIC_ACCESS, qualification);
}

// apic virtualization
void BX_CPU_C::VMX_Virtual_Apic_Write(bx_phy_address paddr, unsigned len, void *data)
{
  BX_ASSERT(SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES));

  Bit32u offset = PAGE_OFFSET(paddr);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_TPR_SHADOW) && offset == 0x80 && len <= 4) {
    // VTPR access
    VMX_Write_VTPR(*((Bit8u *) data));
    return;
  }

  Bit32u qualification = offset | 
      (BX_CPU_THIS_PTR in_event) ? VMX_APIC_ACCESS_DURING_EVENT_DELIVERY : VMX_APIC_WRITE_INSTRUCTION_EXECUTION;
  VMexit(0, VMX_VMEXIT_APIC_ACCESS, qualification);
}

#endif

#if BX_SUPPORT_VMX >= 2
Bit16u BX_CPU_C::VMX_Get_Current_VPID(void)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest || !SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VPID_ENABLE))
    return 0;

  return BX_CPU_THIS_PTR vmcs.vpid;
}
#endif

#endif // BX_SUPPORT_VMX
