/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2019  The Bochs Project
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

BX_CPP_INLINE void BX_CPP_AttrRegparmN(1) BX_CPU_C::branch_near16(Bit16u new_IP)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  // check always, not only in protected mode
  if (new_IP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled)
  {
    BX_ERROR(("branch_near16: offset outside of CS limits"));
    exception(BX_GP_EXCEPTION, 0);
  }

  EIP = new_IP;

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS == 0
  // assert magic async_event to stop trace execution
  BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
#endif
}

void BX_CPU_C::call_far16(bxInstruction_c *i, Bit16u cs_raw, Bit16u disp16)
{
  BX_INSTR_FAR_BRANCH_ORIGIN();

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  RSP_SPECULATIVE;

  if (protected_mode()) {
    call_protected(i, cs_raw, disp16);
  }
  else {
    // CS.LIMIT can't change when in real/v8086 mode
    if (disp16 > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
      BX_ERROR(("%s: instruction pointer not within code segment limits", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }

    push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
    push_16(IP);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
    EIP = (Bit32u) disp16;
  }

  RSP_COMMIT;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPU_C::jmp_far16(bxInstruction_c *i, Bit16u cs_raw, Bit16u disp16)
{
  BX_INSTR_FAR_BRANCH_ORIGIN();

  invalidate_prefetch_q();

  // jump_protected doesn't affect RSP so it is RSP safe
  if (protected_mode()) {
    jump_protected(i, cs_raw, disp16);
  }
  else {
    // CS.LIMIT can't change when in real/v8086 mode
    if (disp16 > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
      BX_ERROR(("%s: instruction pointer not within code segment limits", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
    EIP = disp16;
  }

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP_INDIRECT,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETnear16_Iw(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  RSP_SPECULATIVE;

  Bit16u return_IP = pop_16();
#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL)) {
    Bit32u shadow_IP = shadow_stack_pop_32();
    if (shadow_IP != Bit32u(return_IP))
      exception(BX_CP_EXCEPTION, BX_CP_NEAR_RET);
  }
#endif

  if (return_IP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled)
  {
    BX_ERROR(("%s: offset outside of CS limits", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  EIP = return_IP;

  Bit16u imm16 = i->Iw();

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) /* 32bit stack */
    ESP += imm16;
  else
     SP += imm16;

  RSP_COMMIT;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, PREV_RIP, EIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETfar16_Iw(bxInstruction_c *i)
{
  BX_INSTR_FAR_BRANCH_ORIGIN();

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  Bit16s imm16 = (Bit16s) i->Iw();

  RSP_SPECULATIVE;

  if (protected_mode()) {
    return_protected(i, imm16);
  }
  else {
    Bit16u ip     = pop_16();
    Bit16u cs_raw = pop_16();

    // CS.LIMIT can't change when in real/v8086 mode
    if (ip > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
      BX_ERROR(("%s: instruction pointer not within code segment limits", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
    EIP = (Bit32u) ip;

    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
      ESP += imm16;
    else
       SP += imm16;
  }

  RSP_COMMIT;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_Jw(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  RSP_SPECULATIVE;

  /* push 16 bit EA of next instruction */
  push_16(IP);
#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL) && i->Iw())
    shadow_stack_push_32(IP);
#endif

  Bit16u new_IP = IP + i->Iw();
  branch_near16(new_IP);

  RSP_COMMIT;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL, PREV_RIP, EIP);

  BX_LINK_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL16_Ap(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit16u disp16 = i->Iw();
  Bit16u cs_raw = i->Iw2();

  call_far16(i, cs_raw, disp16);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_EwR(bxInstruction_c *i)
{
  Bit16u new_IP = BX_READ_16BIT_REG(i->dst());

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  RSP_SPECULATIVE;

  /* push 16 bit EA of next instruction */
  push_16(IP);
#if BX_SUPPORT_CET
  if (ShadowStackEnabled(CPL))
    shadow_stack_push_32(IP);
#endif

  branch_near16(new_IP);

  RSP_COMMIT;

#if BX_SUPPORT_CET
  track_indirect_if_not_suppressed(i, CPL);
#endif

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL_INDIRECT, PREV_RIP, EIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL16_Ep(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

  Bit16u op1_16 = read_virtual_word(i->seg(), eaddr);
  Bit16u cs_raw = read_virtual_word(i->seg(), (eaddr+2) & i->asize_mask());

  call_far16(i, cs_raw, op1_16);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_Jw(bxInstruction_c *i)
{
  Bit16u new_IP = IP + i->Iw();
  branch_near16(new_IP);
  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, PREV_RIP, new_IP);

  BX_LINK_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JO_Jw(bxInstruction_c *i)
{
  if (get_OF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNO_Jw(bxInstruction_c *i)
{
  if (! get_OF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JB_Jw(bxInstruction_c *i)
{
  if (get_CF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNB_Jw(bxInstruction_c *i)
{
  if (! get_CF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JZ_Jw(bxInstruction_c *i)
{
  if (get_ZF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNZ_Jw(bxInstruction_c *i)
{
  if (! get_ZF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JBE_Jw(bxInstruction_c *i)
{
  if (get_CF() || get_ZF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNBE_Jw(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF())) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JS_Jw(bxInstruction_c *i)
{
  if (get_SF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNS_Jw(bxInstruction_c *i)
{
  if (! get_SF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JP_Jw(bxInstruction_c *i)
{
  if (get_PF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNP_Jw(bxInstruction_c *i)
{
  if (! get_PF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JL_Jw(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNL_Jw(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF()) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JLE_Jw(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF())) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNLE_Jw(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF())) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_INSTR(i); // trace can continue over non-taken branch
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_EwR(bxInstruction_c *i)
{
  Bit16u new_IP = BX_READ_16BIT_REG(i->dst());
  branch_near16(new_IP);
  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP_INDIRECT, PREV_RIP, new_IP);

#if BX_SUPPORT_CET
  track_indirect_if_not_suppressed(i, CPL);
#endif

  BX_NEXT_TRACE(i);
}

/* Far indirect jump */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP16_Ep(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

  Bit16u op1_16 = read_virtual_word(i->seg(), eaddr);
  Bit16u cs_raw = read_virtual_word(i->seg(), (eaddr+2) & i->asize_mask());

  jmp_far16(i, cs_raw, op1_16);

  BX_NEXT_TRACE(i);
}

#if BX_SUPPORT_SVM
#include "svm.h"
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IRET16(bxInstruction_c *i)
{
  BX_INSTR_FAR_BRANCH_ORIGIN();

  invalidate_prefetch_q();

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_IRET)) Svm_Vmexit(SVM_VMEXIT_IRET);
  }
#endif

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest)
    if (is_masked_event(BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.VIRTUAL_NMI() ? BX_EVENT_VMX_VIRTUAL_NMI : BX_EVENT_NMI))
      BX_CPU_THIS_PTR nmi_unblocking_iret = true;

  if (BX_CPU_THIS_PTR in_vmx_guest && BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.NMI_EXITING()) {
    if (BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.VIRTUAL_NMI()) unmask_event(BX_EVENT_VMX_VIRTUAL_NMI);
  }
  else
#endif
    unmask_event(BX_EVENT_NMI);

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_iret;
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_umonitor();
#endif

  RSP_SPECULATIVE;

  if (protected_mode()) {
    iret_protected(i);
  }
  else {
    if (v8086_mode()) {
      // IOPL check in stack_return_from_v86()
      iret16_stack_return_from_v86(i);
    }
    else {
      Bit16u ip     = pop_16();
      Bit16u cs_raw = pop_16(); // #SS has higher priority
      Bit16u flags  = pop_16();

      // CS.LIMIT can't change when in real/v8086 mode
      if(ip > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
        BX_ERROR(("%s: instruction pointer not within code segment limits", i->getIaOpcodeNameShort()));
        exception(BX_GP_EXCEPTION, 0);
      }

      load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
      EIP = (Bit32u) ip;
      write_flags(flags, /* change IOPL? */ 1, /* change IF? */ 1);
    }
  }

  RSP_COMMIT;

#if BX_SUPPORT_VMX
  BX_CPU_THIS_PTR nmi_unblocking_iret = false;
#endif

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_IRET,
                      FAR_BRANCH_PREV_CS, FAR_BRANCH_PREV_RIP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JCXZ_Jb(bxInstruction_c *i)
{
  // it is impossible to get this instruction in long mode
  BX_ASSERT(i->as64L() == 0);

  Bit32u temp_ECX;

  if (i->as32L())
    temp_ECX = ECX;
  else
    temp_ECX = CX;

  if (temp_ECX == 0) {
    Bit16u new_IP = IP + i->Iw();
    branch_near16(new_IP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    BX_LINK_TRACE(i);
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
  BX_NEXT_TRACE(i);
}

//
// There is some weirdness in LOOP instructions definition. If an exception
// was generated during the instruction execution (for example #GP fault
// because EIP was beyond CS segment limits) CPU state should restore the
// state prior to instruction execution.
//
// The final point that we are not allowed to decrement ECX register before
// it is known that no exceptions can happen.
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPNE16_Jb(bxInstruction_c *i)
{
  // it is impossible to get this instruction in long mode
  BX_ASSERT(i->as64L() == 0);

  if (i->as32L()) {
    Bit32u count = ECX;

    count--;
    if (count != 0 && (get_ZF()==0)) {
      Bit16u new_IP = IP + i->Iw();
      branch_near16(new_IP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    ECX = count;
  }
  else {
    Bit16u count = CX;

    count--;
    if (count != 0 && (get_ZF()==0)) {
      Bit16u new_IP = IP + i->Iw();
      branch_near16(new_IP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    CX = count;
  }

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPE16_Jb(bxInstruction_c *i)
{
  // it is impossible to get this instruction in long mode
  BX_ASSERT(i->as64L() == 0);

  if (i->as32L()) {
    Bit32u count = ECX;

    count--;
    if (count != 0 && get_ZF()) {
      Bit16u new_IP = IP + i->Iw();
      branch_near16(new_IP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    ECX = count;
  }
  else {
    Bit16u count = CX;

    count--;
    if (count != 0 && get_ZF()) {
      Bit16u new_IP = IP + i->Iw();
      branch_near16(new_IP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    CX = count;
  }

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOP16_Jb(bxInstruction_c *i)
{
  // it is impossible to get this instruction in long mode
  BX_ASSERT(i->as64L() == 0);

  if (i->as32L()) {
    Bit32u count = ECX;

    count--;
    if (count != 0) {
      Bit16u new_IP = IP + i->Iw();
      branch_near16(new_IP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    ECX = count;
  }
  else {
    Bit16u count = CX;

    count--;
    if (count != 0) {
      Bit16u new_IP = IP + i->Iw();
      branch_near16(new_IP);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, PREV_RIP, new_IP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID, PREV_RIP);
    }
#endif

    CX = count;
  }

  BX_NEXT_TRACE(i);
}
