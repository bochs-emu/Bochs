/////////////////////////////////////////////////////////////////////////
// $Id: ctrl_xfer64.cc,v 1.11 2002-09-24 00:44:55 kevinlawton Exp $
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



#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR






  void
BX_CPU_C::RETnear64_Iw(bxInstruction_c *i)
{
  Bit16u imm16;
  Bit64u temp_RSP;
  Bit64u return_RIP;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  temp_RSP = RSP;

  imm16 = i->Iw();

  //if ( !can_pop(8) ) {
  //  BX_PANIC(("retnear_iw: can't pop RIP"));
  //  /* ??? #SS(0) -or #GP(0) */
  //  }

  access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_RSP + 0,
    8, CPL==3, BX_READ, &return_RIP);

  /* Pentium book says imm16 is number of words ??? */
  //if ( !can_pop(8 + imm16) ) {
  //  BX_PANIC(("retnear_iw: can't release bytes from stack"));
  //  /* #GP(0) -or #SS(0) ??? */
  //  }

  RIP = return_RIP;
  RSP += 8 + imm16; /* ??? should it be 2*imm16 ? */

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_RET, BX_CPU_THIS_PTR rip);
}

  void
BX_CPU_C::RETnear64(bxInstruction_c *i)
{
  Bit64u temp_RSP;
  Bit64u return_RIP;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  temp_RSP = RSP;

  //if ( !can_pop(8) ) {
  //  BX_PANIC(("retnear: can't pop RIP"));
  //  /* ??? #SS(0) -or #GP(0) */
  //  }

  access_linear(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.base + temp_RSP + 0,
      8, CPL==3, BX_READ, &return_RIP);

  RIP = return_RIP;
  RSP += 8;

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_RET, BX_CPU_THIS_PTR rip);
}

  void
BX_CPU_C::RETfar64_Iw(bxInstruction_c *i)
{
  Bit64u rip, rcs_raw;
  Bit16s imm16;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif
  /* ??? is imm16, number of bytes/words depending on operandsize ? */

  imm16 = i->Iw();

#if BX_CPU_LEVEL >= 2
  if (protected_mode()) {
    BX_CPU_THIS_PTR return_protected(i, imm16);
    goto done;
    }
#endif


    pop_64(&rip);
    pop_64(&rcs_raw);
    RIP = rip;
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) rcs_raw);
    RSP += imm16;
done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_RET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR rip);
}

  void
BX_CPU_C::RETfar64(bxInstruction_c *i)
{
  Bit64u rip, rcs_raw;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

#if BX_CPU_LEVEL >= 2
  if ( protected_mode() ) {
    BX_CPU_THIS_PTR return_protected(i, 0);
    goto done;
    }
#endif


  pop_64(&rip);
  pop_64(&rcs_raw); /* 64bit pop, upper 48 bits discarded */
  RIP = rip;
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], (Bit16u) rcs_raw);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_RET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR rip);
}



  void
BX_CPU_C::CALL_Aq(bxInstruction_c *i)
{
  Bit64u new_RIP;
  Bit32s disp32;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  disp32 = i->Id();

  new_RIP = RIP + disp32;

  /* push 64 bit EA of next instruction */
  push_64(BX_CPU_THIS_PTR rip);
  RIP = new_RIP;

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_CALL, BX_CPU_THIS_PTR rip);
}

  void
BX_CPU_C::CALL64_Ap(bxInstruction_c *i)
{
  Bit16u cs_raw;
  Bit32u disp32;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  disp32 = i->Id();
  cs_raw = i->Iw2();

  if (protected_mode()) {
    BX_CPU_THIS_PTR call_protected(i, cs_raw, disp32);
    goto done;
    }
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  push_64(BX_CPU_THIS_PTR rip);
  RIP = disp32;
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_CALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR rip);
}

  void
BX_CPU_C::CALL_Eq(bxInstruction_c *i)
{
  Bit64u temp_RSP;
  Bit64u op1_64;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  temp_RSP = RSP;

  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    }
  else {
    read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    }

  push_64(BX_CPU_THIS_PTR rip);
  RIP = op1_64;

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_CALL, BX_CPU_THIS_PTR rip);
}

  void
BX_CPU_C::CALL64_Ep(bxInstruction_c *i)
{
  Bit16u cs_raw;
  Bit64u op1_64;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    BX_PANIC(("CALL_Ep: op1 is a register"));
    }

  /* pointer, segment address pair */
  read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  read_virtual_word(i->seg(), RMAddr(i)+8, &cs_raw);

  if ( protected_mode() ) {
    BX_CPU_THIS_PTR call_protected(i, cs_raw, op1_64);
    goto done;
    }

  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  push_64(BX_CPU_THIS_PTR rip);

  RIP = op1_64;
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_CALL,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR rip);
}


  void
BX_CPU_C::JMP_Jq(bxInstruction_c *i)
{
  Bit64u new_RIP;

  invalidate_prefetch_q();

  RIP += (Bit32s) i->Id();
  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_JMP, new_RIP);
}

  void
BX_CPU_C::JCC_Jq(bxInstruction_c *i)
{
  Boolean condition;

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

    RIP += (Bit32s) i->Id();
    BX_INSTR_CNEAR_BRANCH_TAKEN(RIP);
    revalidate_prefetch_q();
    }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN();
    }
#endif
}

#ifdef ignore
  void
BX_CPU_C::JMP64_Ap(bxInstruction_c *i)
{
  Bit64u disp64;
  Bit16u cs_raw;

  invalidate_prefetch_q();

  if (i->os32L()) {
    disp64 = (Bit32s) i->Id();
    }
  else {
    disp64 = (Bit16s) i->Iw();
    }
  cs_raw = i->Iw2();

#if BX_CPU_LEVEL >= 2
  if (protected_mode()) {
    BX_CPU_THIS_PTR jump_protected(i, cs_raw, disp32);
    goto done;
    }
#endif

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);
  RIP = disp64;

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_JMP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR rip);
}

#endif



  void
BX_CPU_C::JMP_Eq(bxInstruction_c *i)
{
  Bit64u new_RIP;
  Bit64u op1_64;

  invalidate_prefetch_q();

  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    }
  else {
    read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    }

  RIP = op1_64;

  BX_INSTR_UCNEAR_BRANCH(BX_INSTR_IS_JMP, new_RIP);
}

  /* Far indirect jump */

  void
BX_CPU_C::JMP64_Ep(bxInstruction_c *i)
{
  Bit16u cs_raw;
  Bit64u op1_64;

  invalidate_prefetch_q();

  if (i->modC0()) {
    BX_PANIC(("JMP_Ep(): op1 is a register"));
    }

  read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  read_virtual_word(i->seg(), RMAddr(i)+8, &cs_raw);

  if ( protected_mode() ) {
    BX_CPU_THIS_PTR jump_protected(i, cs_raw, op1_64);
    goto done;
    }

  RIP = op1_64;
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_raw);

done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_JMP,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
}

  void
BX_CPU_C::IRET64(bxInstruction_c *i)
{
  Bit32u rip, ecs_raw, eflags;

  invalidate_prefetch_q();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_iret;
  BX_CPU_THIS_PTR show_eip = BX_CPU_THIS_PTR rip;
#endif

#if BX_CPU_LEVEL >= 2
  if (BX_CPU_THIS_PTR cr0.pe) {
    iret_protected(i);
    goto done;
    }
#endif


done:
  BX_INSTR_FAR_BRANCH(BX_INSTR_IS_IRET,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR rip);
}
