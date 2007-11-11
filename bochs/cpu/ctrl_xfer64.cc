/////////////////////////////////////////////////////////////////////////
// $Id: ctrl_xfer64.cc,v 1.51 2007-11-11 21:26:10 sshwarts Exp $
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
  Bit64u return_RIP;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  Bit16u imm16 = i->Iw();

  pop_64(&return_RIP);

  if (! IsCanonical(return_RIP)) {
    BX_ERROR(("RETnear64_Iw: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = return_RIP;
  RSP += imm16;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, RIP);
}

void BX_CPU_C::RETnear64(bxInstruction_c *i)
{
  Bit64u return_RIP;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  pop_64(&return_RIP);

  if (! IsCanonical(return_RIP)) {
    BX_ERROR(("RETnear64: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = return_RIP;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, RIP);
}

void BX_CPU_C::RETfar64_Iw(bxInstruction_c *i)
{
  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  BX_ASSERT(protected_mode());

  BX_INFO(("RETfar64_Iw instruction executed ..."));

  return_protected(i, i->Iw());

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

  BX_INFO(("RETfar64 instruction executed ..."));

  return_protected(i, 0);

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
    read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
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

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    BX_ERROR(("CALL64_Ep: op1 is a register"));
    exception(BX_UD_EXCEPTION, 0, 0);
  }

  /* pointer, segment address pair */
  read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  read_virtual_word(i->seg(), RMAddr(i)+8, &cs_raw);

  BX_ASSERT(protected_mode());

  BX_CPU_THIS_PTR call_protected(i, cs_raw, op1_32);

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

void BX_CPU_C::JMP_Jq(bxInstruction_c *i)
{
  branch_near64(i);
  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, RIP);
}

void BX_CPU_C::JCC_Jq(bxInstruction_c *i)
{
  bx_bool condition;

  switch (i->b1() & 0x0f) {
    case 0x00: /* JO */ condition = get_OF(); break;
    case 0x01: /* JNO */ condition = !get_OF(); break;
    case 0x02: /* JB */ condition = get_CF(); break;
    case 0x03: /* JNB */ condition = !get_CF(); break;
    case 0x04: /* JZ */ condition = get_ZF(); break;
    case 0x05: /* JNZ */ condition = !get_ZF(); break;
    case 0x06: /* JBE */ condition = get_CF() || get_ZF(); break;
    case 0x07: /* JNBE */ condition = !get_CF() && !get_ZF(); break;
    case 0x08: /* JS */ condition = get_SF(); break;
    case 0x09: /* JNS */ condition = !get_SF(); break;
    case 0x0A: /* JP */ condition = get_PF(); break;
    case 0x0B: /* JNP */ condition = !get_PF(); break;
    case 0x0C: /* JL */ condition = getB_SF() != getB_OF(); break;
    case 0x0D: /* JNL */ condition = getB_SF() == getB_OF(); break;
    case 0x0E: /* JLE */ condition = get_ZF() || (getB_SF() != getB_OF());
      break;
    case 0x0F: /* JNLE */ condition = (getB_SF() == getB_OF()) &&
                            !get_ZF();
      break;
    default:
      condition = 0; // For compiler...all targets should set condition.
      break;
  }

  if (condition) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPU_C::JZ_Jq(bxInstruction_c *i)
{
  if (get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPU_C::JNZ_Jq(bxInstruction_c *i)
{
  if (! get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPU_C::JMP_Eq(bxInstruction_c *i)
{
  Bit64u op1_64;

  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
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

  if (i->modC0()) {
    BX_ERROR(("JMP64_Ep(): op1 is a register"));
    exception(BX_UD_EXCEPTION, 0, 0);
  }

  read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  read_virtual_word(i->seg(), RMAddr(i)+4, &cs_raw);

  BX_ASSERT(protected_mode());
  BX_CPU_THIS_PTR jump_protected(i, cs_raw, op1_32);

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
  iret_protected(i);

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
