/////////////////////////////////////////////////////////////////////////
// $Id: ctrl_xfer32.cc,v 1.61 2007-12-20 20:58:37 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RSP ESP
#endif

#if BX_CPU_LEVEL >= 3

void BX_CPU_C::RETnear32_Iw(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  Bit16u imm16 = i->Iw();
  Bit32u return_EIP = pop_32();
  branch_near32(return_EIP);
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    ESP += imm16;
  else
     SP += imm16;

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, EIP);
}

void BX_CPU_C::RETnear32(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  Bit32u return_EIP = pop_32();
  branch_near32(return_EIP);

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, EIP);
}

void BX_CPU_C::RETfar32_Iw(bxInstruction_c *i)
{
  Bit32u eip, cs_raw;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  Bit16u imm16 = i->Iw();

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  if (protected_mode()) {
    BX_CPU_THIS_PTR return_protected(i, imm16);
    goto done;
  }

  eip    = pop_32();
  cs_raw = pop_32();

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) cs_raw);
  EIP = eip;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    ESP += imm16;
  else
     SP += imm16;

done:
  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
}

void BX_CPU_C::RETfar32(bxInstruction_c *i)
{
  Bit32u eip, cs_raw;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  if (protected_mode()) {
    BX_CPU_THIS_PTR return_protected(i, 0);
    goto done;
  }

  eip    = pop_32();
  cs_raw = pop_32(); /* 32bit pop, MSW discarded */

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) cs_raw);
  EIP = eip;

done:
  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
}

void BX_CPU_C::CALL_Jd(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  Bit32u new_EIP = EIP + i->Id();

  if (new_EIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
    BX_ERROR(("CALL_Jd: offset outside of CS limits"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* push 32 bit EA of next instruction */
  push_32(EIP);
  EIP = new_EIP;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL, EIP);
}

void BX_CPU_C::CALL32_Ap(bxInstruction_c *i)
{
  Bit16u cs_raw;
  Bit32u disp32;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  disp32 = i->Id();
  cs_raw = i->Iw2();

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  if (protected_mode()) {
    BX_CPU_THIS_PTR call_protected(i, cs_raw, disp32);
    goto done;
  }

  push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  push_32(EIP);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
  EIP = disp32;

done:
  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
}

void BX_CPU_C::CALL_Ed(bxInstruction_c *i)
{
  Bit32u op1_32;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    op1_32 = read_virtual_dword(i->seg(), RMAddr(i));
  }

  if (op1_32 > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled)
  {
    BX_ERROR(("CALL_Ed: EIP out of CS limits!"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  push_32(EIP);
  EIP = op1_32;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL, EIP);
}

void BX_CPU_C::CALL32_Ep(bxInstruction_c *i)
{
  Bit16u cs_raw;
  Bit32u op1_32;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  /* pointer, segment address pair */
  op1_32 = read_virtual_dword(i->seg(), RMAddr(i));
  cs_raw = read_virtual_word (i->seg(), RMAddr(i)+4);

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  if (protected_mode()) {
    BX_CPU_THIS_PTR call_protected(i, cs_raw, op1_32);
    goto done;
  }

  push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  push_32(EIP);

  EIP = op1_32;
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);

done:
  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
}

void BX_CPU_C::JMP_Jd(bxInstruction_c *i)
{
  Bit32u new_EIP = EIP + (Bit32s) i->Id();
  branch_near32(new_EIP);
  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, new_EIP);
}

void BX_CPU_C::JO_Jd(bxInstruction_c *i)
{
  if (get_OF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNO_Jd(bxInstruction_c *i)
{
  if (! get_OF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JB_Jd(bxInstruction_c *i)
{
  if (get_CF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNB_Jd(bxInstruction_c *i)
{
  if (! get_CF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JZ_Jd(bxInstruction_c *i)
{
  if (get_ZF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNZ_Jd(bxInstruction_c *i)
{
  if (! get_ZF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JBE_Jd(bxInstruction_c *i)
{
  if (get_CF() || get_ZF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNBE_Jd(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF())) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JS_Jd(bxInstruction_c *i)
{
  if (get_SF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNS_Jd(bxInstruction_c *i)
{
  if (! get_SF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JP_Jd(bxInstruction_c *i)
{
  if (get_PF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNP_Jd(bxInstruction_c *i)
{
  if (! get_PF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JL_Jd(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNL_Jd(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF()) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JLE_Jd(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF())) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
    return;
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNLE_Jd(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF())) {
    Bit32u new_EIP = EIP + (Bit32s) i->Id();
    branch_near32(new_EIP);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, new_EIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPU_C::JMP_Ap(bxInstruction_c *i)
{
  Bit32u disp32;
  Bit16u cs_raw;

  invalidate_prefetch_q();

  if (i->os32L()) {
    disp32 = i->Id();
  }
  else {
    disp32 = i->Iw();
  }
  cs_raw = i->Iw2();

  // jump_protected doesn't affect RSP so it is RSP safe
  if (protected_mode()) {
    BX_CPU_THIS_PTR jump_protected(i, cs_raw, disp32);
  }
  else {
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
    EIP = disp32;
  }

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
}

void BX_CPU_C::JMP_EdM(bxInstruction_c *i)
{
  /* pointer, segment address pair */
  Bit32u new_EIP = read_virtual_dword(i->seg(), RMAddr(i));

  branch_near32(new_EIP);

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, new_EIP);
}

void BX_CPU_C::JMP_EdR(bxInstruction_c *i)
{
  Bit32u new_EIP = BX_READ_32BIT_REG(i->rm());
  branch_near32(new_EIP);
  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, new_EIP);
}

/* Far indirect jump */
void BX_CPU_C::JMP32_Ep(bxInstruction_c *i)
{
  Bit16u cs_raw;
  Bit32u op1_32;

  invalidate_prefetch_q();

  /* pointer, segment address pair */
  op1_32 = read_virtual_dword(i->seg(), RMAddr(i));
  cs_raw = read_virtual_word (i->seg(), RMAddr(i)+4);

  // jump_protected doesn't affect RSP so it is RSP safe
  if (protected_mode()) {
    BX_CPU_THIS_PTR jump_protected(i, cs_raw, op1_32);
  }
  else {
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
    EIP = op1_32;
  }

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
}

void BX_CPU_C::IRET32(bxInstruction_c *i)
{
  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_iret;
#endif

  BX_CPU_THIS_PTR nmi_disable = 0;

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  if (v8086_mode()) {
    // IOPL check in stack_return_from_v86()
    iret32_stack_return_from_v86(i);
    goto done;
  }

  if (protected_mode()) {
    iret_protected(i);
    goto done;
  }

  Bit32u eip = pop_32();

  // CS.LIMIT in real mode is 0xffff
  if (eip > 0xffff) {
    BX_ERROR(("IRETD: instruction pointer not within code segment limits"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit16u cs     = pop_32() & 0xffff;
  Bit32u eflags = pop_32();

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u)cs);
  EIP = eip;
  writeEFlags(eflags, 0x00257fd5); // VIF, VIP, VM unchanged

done:
  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_IRET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
}

#endif
