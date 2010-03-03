/////////////////////////////////////////////////////////////////////////
// $Id: vmexit.cc,v 1.14 2010-03-03 15:12:44 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2009 Stanislav Shwartsman
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

#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu32 code.
#define RIP EIP
#define RDI EDI
#define RSI ESI
#endif

#if BX_SUPPORT_VMX

Bit32u gen_instruction_info(bxInstruction_c *i, Bit32u reason)
{
  Bit32u instr_info = 0;

  switch(reason) {
    case VMX_VMEXIT_VMREAD:
    case VMX_VMEXIT_VMWRITE:
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
      // do not have VMEXIT instruction info
      break;
    case VMX_VMEXIT_VMREAD:
    case VMX_VMEXIT_VMWRITE:
    case VMX_VMEXIT_VMPTRLD:
    case VMX_VMEXIT_VMPTRST:
    case VMX_VMEXIT_VMCLEAR:
    case VMX_VMEXIT_VMXON:
      qualification = (Bit64u) ((bx_address) i->displ32s());
#if BX_SUPPORT_X86_64
      if (i->sibBase() == BX_64BIT_REG_RIP)
        qualification += RIP;
#endif
      instr_info = gen_instruction_info(i, reason);

      VMwrite32(VMCS_32BIT_VMEXIT_INSTRUCTION_INFO, instr_info);
      break;

    default:
      BX_PANIC(("VMexit_Instruction reason %d", reason));
  }

  VMexit(i, reason, qualification);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_HLT(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_HLT_VMEXIT))
  {
    BX_ERROR(("VMEXIT: HLT"));
    VMexit(i, VMX_VMEXIT_HLT, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_PAUSE(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_PAUSE_VMEXIT))
  {
    BX_ERROR(("VMEXIT: PAUSE"));
    VMexit(i, VMX_VMEXIT_PAUSE, 0);
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_INVLPG(bxInstruction_c *i, bx_address laddr)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_INVLPG_VMEXIT))
  {
    BX_ERROR(("VMEXIT: INVLPG 0x" FMT_ADDRX, laddr));
    VMexit(i, VMX_VMEXIT_INVLPG, laddr);
  }
}

Bit64s BX_CPU_C::VMX_TSC_Offset(void)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return 0;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_TSC_OFFSET))
    return (Bit64s) BX_CPU_THIS_PTR vmcs.tsc_offset;
  else
    return 0;
}


void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_RDTSC(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_RDTSC_VMEXIT))
  {
    BX_ERROR(("VMEXIT: RDTSC"));
    VMexit(i, VMX_VMEXIT_RDTSC, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_RDPMC(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_RDPMC_VMEXIT))
  {
    BX_ERROR(("VMEXIT: RDPMC"));
    VMexit(i, VMX_VMEXIT_RDPMC, 0);
  }
}

#if BX_SUPPORT_MONITOR_MWAIT
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_MONITOR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_MONITOR_VMEXIT))
  {
    BX_ERROR(("VMEXIT: MONITOR"));
    VMexit(i, VMX_VMEXIT_MONITOR, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_MWAIT(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_MWAIT_VMEXIT))
  {
    BX_ERROR(("VMEXIT: MWAIT"));
    VMexit(i, VMX_VMEXIT_MWAIT, 0);
  }
}
#endif

void BX_CPU_C::VMexit_ExtInterrupt(void)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_EXTERNAL_INTERRUPT_VMEXIT)) {
    if (! PIN_VMEXIT(VMX_VMEXIT_CTRL1_INTA_ON_VMEXIT)) {
       // interrupt wasn't acknowledged and still pending, interruption info is invalid
       VMwrite32(VMCS_32BIT_VMEXIT_INTERRUPTION_INFO, 0);
       VMexit(0, VMX_VMEXIT_EXTERNAL_INTERRUPT, 0);
    }
  }
}

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
      BX_ASSERT((vector < BX_CPU_HANDLED_EXCEPTIONS));
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

  BX_ERROR(("VMEXIT: exception 0x%02x error code = 0x%04x", vector, errcode));

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
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  BX_ERROR(("VMEXIT: task switch"));

  VMexit(i, VMX_VMEXIT_TASK_SWITCH, tss_selector | (source << 30));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_SoftwareInterrupt(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;
}

void BX_CPP_AttrRegparmN(3) BX_CPU_C::VMexit_MSR(bxInstruction_c *i, unsigned op, Bit32u msr)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  bx_bool vmexit = 0;
  if (! VMEXIT(VMX_VM_EXEC_CTRL2_MSR_BITMAPS)) vmexit = 1;
  else {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    Bit8u field;

    if (msr & 0xC0000000) {
       if (msr > 0xC0001FFF) vmexit = 1;
       else {
         // check MSR-HI bitmaps
         bx_phy_address pAddr = vm->msr_bitmap_addr + (msr >> 3) + 1024 + ((op == VMX_VMEXIT_RDMSR) ? 0 : 2048);
         access_read_physical(pAddr, 1, &field);
         BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, &field);
         if (field & (1 << (msr & 7)))
            vmexit = 1;
       }
    }
    else {
       if (msr > 0x00001FFF) vmexit = 1;
       else {
         // check MSR-LO bitmaps
         bx_phy_address pAddr = vm->msr_bitmap_addr + (msr >> 3) + ((op == VMX_VMEXIT_RDMSR) ? 0 : 2048);
         access_read_physical(pAddr, 1, &field);
         BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, &field);
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
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  BX_ASSERT((port <= 0xFFFF));

  bool vmexit = 0;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_IO_BITMAPS)) {
     // always VMEXIT on port "wrap around" case
     if ((port + len) > 0x10000) vmexit = 1;
     else {
        bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.io_bitmap_addr[(port >> 15) & 1] + ((port & 0x7fff) >> 3);
        Bit16u bitmap;
        access_read_physical(pAddr, 2, (Bit8u*) &bitmap);
        BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 2, BX_READ, (Bit8u*) &bitmap);

        unsigned mask = ((1 << len) - 1) << (port & 7);
        if (bitmap & mask) vmexit = 1;
     }
  }
  else if (VMEXIT(VMX_VM_EXEC_CTRL2_IO_VMEXIT)) vmexit = 1;

  if (vmexit) {
     BX_ERROR(("VMEXIT: I/O port 0x%04x", port));

     Bit32u qualification = 0;

     switch(i->b1()) {
       case 0xe4: // IN_ALIb
       case 0xe5: // IN_AXIb, IN_EAXIb
         qualification = VMX_VMEXIT_IO_PORTIN | VMX_VMEXIT_IO_INSTR_IMM;
         break;

       case 0xe6: // OUT_IbAL
       case 0xe7: // OUT_IbAX, OUT_IbEAX
         qualification = VMX_VMEXIT_IO_INSTR_IMM;
         break;

       case 0xec: // IN_ALDX
       case 0xed: // IN_AXDX, IN_EAXDX
         qualification = VMX_VMEXIT_IO_PORTIN; // no immediate
         break;

       case 0xee: // OUT_DXAL
       case 0xef: // OUT_DXAX, OUT_DXEAX
         qualification = 0; // PORTOUT, no immediate
         break;

       case 0x6c: // INSB_YbDX
       case 0x6d: // INSW_YwDX, INSD_YdDX
         qualification = VMX_VMEXIT_IO_PORTIN | VMX_VMEXIT_IO_INSTR_STRING;
         if (i->repUsedL())
            qualification |= VMX_VMEXIT_IO_INSTR_REP;
         break;

       case 0x6e: // OUTSB_DXXb
       case 0x6f: // OUTSW_DXXw, OUTSD_DXXd
         qualification = VMX_VMEXIT_IO_INSTR_STRING; // PORTOUT
         if (i->repUsedL())
            qualification |= VMX_VMEXIT_IO_INSTR_REP;
         break;

       default:
         BX_PANIC(("VMexit_IO: I/O instruction b1()=%x unknown", i->b1()));
     }

     if (qualification & VMX_VMEXIT_IO_INSTR_STRING) {
       bx_address asize_mask = BX_CONST64(0xffffffffffffffff), laddr;
       if (! i->as64L()) {
          if (i->as32L()) asize_mask = 0xffffffff;
          else asize_mask = 0xffff;
       }

       if (qualification & VMX_VMEXIT_IO_PORTIN)
          laddr = BX_CPU_THIS_PTR get_laddr(BX_SEG_REG_ES, RDI & asize_mask);
       else  // PORTOUT
          laddr = BX_CPU_THIS_PTR get_laddr(i->seg(), RSI & asize_mask);

       VMwrite64(VMCS_GUEST_LINEAR_ADDR, laddr);

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
  if (! BX_CPU_THIS_PTR in_vmx_guest) return 0;

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
  if (! BX_CPU_THIS_PTR in_vmx_guest) return msw;

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
       VMwrite64(VMCS_GUEST_LINEAR_ADDR, BX_CPU_THIS_PTR get_laddr(i->seg(), RMAddr(i)));
    }

    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
  }

  // keep untouched all the bits set in CR0 mask
  return (BX_CPU_THIS_PTR cr0.get32() & mask) | (msw & ~mask);
}

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_CR0_Write(bxInstruction_c *i, bx_address val)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return val;

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
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR3_READ_VMEXIT)) {
    BX_ERROR(("VMEXIT: CR3 read"));

    Bit64u qualification = 3 | (VMX_VMEXIT_CR_ACCESS_CR_READ << 4);
    qualification |= (i->rm() << 8);

    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMexit_CR3_Write(bxInstruction_c *i, bx_address val)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

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
  if (! BX_CPU_THIS_PTR in_vmx_guest) return val;

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
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_CR8_READ_VMEXIT)) {
    BX_ERROR(("VMEXIT: CR8 read"));

    Bit64u qualification = 8 | (VMX_VMEXIT_CR_ACCESS_CR_READ << 4);
    qualification |= (i->rm() << 8);

    VMexit(i, VMX_VMEXIT_CR_ACCESS, qualification);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMexit_CR8_Write(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

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
  if (! BX_CPU_THIS_PTR in_vmx_guest) return;

  if (VMEXIT(VMX_VM_EXEC_CTRL2_DRx_ACCESS_VMEXIT))
  {
    BX_ERROR(("VMEXIT: DR%d %s access", i->nnn(), read ? "READ" : "WRITE"));

    Bit64u qualification = i->nnn() | (i->rm() << 8);
    if (read)
       qualification |= (1 << 4);

    VMexit(i, VMX_VMEXIT_DR_ACCESS, qualification);
  }
}

Bit32u BX_CPU_C::VMX_Read_TPR_Shadow(void)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr + 0x80;
  Bit8u tpr_shadow;

  access_read_physical(pAddr, 1, &tpr_shadow);
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_READ, &tpr_shadow);

  return (tpr_shadow >> 4) & 0xF;
}

void BX_CPU_C::VMX_Write_TPR_Shadow(Bit8u tpr_shadow)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  bx_phy_address pAddr = vm->virtual_apic_page_addr + 0x80;
  Bit8u field = tpr_shadow << 4;

  access_write_physical(pAddr, 1, &field);
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_WRITE, &field);

  if (tpr_shadow < vm->vm_tpr_threshold) {
    // commit current instruction to produce trap-like VMexit
    BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
    VMexit(0, VMX_VMEXIT_TPR_THRESHOLD, 0);
  }
}

#endif // BX_SUPPORT_VMX
