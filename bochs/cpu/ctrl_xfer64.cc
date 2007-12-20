/////////////////////////////////////////////////////////////////////////
// $Id: ctrl_xfer64.cc,v 1.58 2007-12-20 20:58:37 sshwarts Exp $
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


#if BX_SUPPORT_X86_64

void BX_CPU_C::RETnear64_Iw(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  Bit64u return_RIP = read_virtual_qword(BX_SEG_REG_SS, RSP);

  if (! IsCanonical(return_RIP)) {
    BX_ERROR(("RETnear64_Iw: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = return_RIP;
  RSP += 8 + i->Iw();

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, RIP);
}

void BX_CPU_C::RETnear64(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  Bit64u return_RIP = read_virtual_qword(BX_SEG_REG_SS, RSP);

  if (! IsCanonical(return_RIP)) {
    BX_ERROR(("RETnear64_Iw: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = return_RIP;
  RSP += 8;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, RIP);
}

void BX_CPU_C::RETfar64_Iw(bxInstruction_c *i)
{
  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  BX_ASSERT(protected_mode());

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  // return_protected is not RSP safe
  return_protected(i, i->Iw());

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPU_C::RETfar64(bxInstruction_c *i)
{
  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  BX_ASSERT(protected_mode());

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  // return_protected is not RSP safe
  return_protected(i, 0);

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPU_C::CALL_Jq(bxInstruction_c *i)
{
  Bit64u new_RIP = RIP + (Bit32s) i->Id();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("CALL_Jq: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* push 64 bit EA of next instruction */
  push_64(RIP);
  RIP = new_RIP;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL, RIP);
}

void BX_CPU_C::CALL_Eq(bxInstruction_c *i)
{
  Bit64u op1_64;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    op1_64 = read_virtual_qword(i->seg(), RMAddr(i));
  }

  if (! IsCanonical(op1_64))
  {
    BX_ERROR(("CALL_Eq: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  push_64(RIP);
  RIP = op1_64;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL, RIP);
}

void BX_CPU_C::CALL64_Ep(bxInstruction_c *i)
{
  Bit16u cs_raw;
  Bit32u op1_32;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  /* pointer, segment address pair */
  op1_32 = read_virtual_dword(i->seg(), RMAddr(i));
  cs_raw = read_virtual_word (i->seg(), RMAddr(i)+8);

  BX_ASSERT(protected_mode());

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  // call_protected is not RSP safe
  call_protected(i, cs_raw, op1_32);

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPU_C::JMP_Jq(bxInstruction_c *i)
{
  branch_near64(i);
  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, RIP);
}

void BX_CPU_C::JO_Jq(bxInstruction_c *i)
{
  if (get_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNO_Jq(bxInstruction_c *i)
{
  if (! get_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JB_Jq(bxInstruction_c *i)
{
  if (get_CF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNB_Jq(bxInstruction_c *i)
{
  if (! get_CF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JZ_Jq(bxInstruction_c *i)
{
  if (get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNZ_Jq(bxInstruction_c *i)
{
  if (! get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JBE_Jq(bxInstruction_c *i)
{
  if (get_CF() || get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNBE_Jq(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JS_Jq(bxInstruction_c *i)
{
  if (get_SF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNS_Jq(bxInstruction_c *i)
{
  if (! get_SF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JP_Jq(bxInstruction_c *i)
{
  if (get_PF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNP_Jq(bxInstruction_c *i)
{
  if (! get_PF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JL_Jq(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNL_Jq(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JLE_Jq(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JNLE_Jq(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    return;
  }
  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::JMP_Eq(bxInstruction_c *i)
{
  Bit64u op1_64;

  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    op1_64 = read_virtual_qword(i->seg(), RMAddr(i));
  }

  if (! IsCanonical(op1_64)) {
    BX_ERROR(("JMP_Eq: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = op1_64;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, RIP);
}

/* Far indirect jump */
void BX_CPU_C::JMP64_Ep(bxInstruction_c *i)
{
  Bit16u cs_raw;
  Bit32u op1_32;

  invalidate_prefetch_q();

  op1_32 = read_virtual_dword(i->seg(), RMAddr(i));
  cs_raw = read_virtual_word (i->seg(), RMAddr(i)+4);

  BX_ASSERT(protected_mode());

  jump_protected(i, cs_raw, op1_32);

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPU_C::IRET64(bxInstruction_c *i)
{
  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_iret;
#endif

  BX_CPU_THIS_PTR nmi_disable = 0;

  BX_ASSERT(protected_mode());

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  // long_iret is not RSP safe
  long_iret(i);

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_IRET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPU_C::JCXZ64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    if (RCX == 0) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      return;
    }
  }
  else {
    if (ECX == 0) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      return;
    }
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::LOOPNE64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    if (((--RCX) != 0) && (get_ZF()==0)) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      return;
    }
  }
  else {
    RCX = ECX - 1;
    if (RCX != 0 && (get_ZF()==0)) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      return;
    }
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::LOOPE64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    if (((--RCX)!=0) && get_ZF()) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      return;
    }
  }
  else {
    RCX = ECX - 1;
    if (RCX != 0 && get_ZF()) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      return;
    }
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

void BX_CPU_C::LOOP64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    if ((--RCX) != 0) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      return;
    }
  }
  else {
    RCX = ECX - 1;
    if (RCX != 0) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
      return;
    }
  }

  BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
}

#endif /* if BX_SUPPORT_X86_64 */
