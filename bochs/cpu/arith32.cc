/////////////////////////////////////////////////////////////////////////
// $Id: arith32.cc,v 1.15 2002-09-20 03:52:58 kevinlawton Exp $
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


#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu code.
#define RAX EAX
#define RDX EDX
#endif


  void
BX_CPU_C::INC_ERX(bxInstruction_c *i)
{
  Bit32u erx;

  erx = ++ BX_CPU_THIS_PTR gen_reg[(i->b1() & 7) + i->rex_b()].dword.erx;
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR gen_reg[(i->b1() & 7) + i->rex_b()].dword.hrx = 0;
#endif

//#if BX_SUPPORT_X86_64
//  erx = ++ BX_CPU_THIS_PTR gen_reg[i->nnn()].dword.erx;
//  BX_CPU_THIS_PTR gen_reg[i->nnn()].dword.hrx = 0;
//#else
//  erx = ++ BX_CPU_THIS_PTR gen_reg[i->b1() & 0x07].dword.erx;
//#endif
  SET_FLAGS_OSZAP_32(0, 0, erx, BX_INSTR_INC32);
}

  void
BX_CPU_C::DEC_ERX(bxInstruction_c *i)
{
  Bit32u erx;

  erx = -- BX_CPU_THIS_PTR gen_reg[(i->b1() & 7) + i->rex_b()].dword.erx;
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR gen_reg[(i->b1() & 7) + i->rex_b()].dword.hrx = 0;
#endif

//#if BX_SUPPORT_X86_64
//  erx = -- BX_CPU_THIS_PTR gen_reg[i->nnn()].dword.erx;
//  BX_CPU_THIS_PTR gen_reg[i->nnn()].dword.hrx = 0;
//#else
//  erx = -- BX_CPU_THIS_PTR gen_reg[i->b1() & 0x07].dword.erx;
//#endif
  SET_FLAGS_OSZAP_32(0, 0, erx, BX_INSTR_DEC32);
}




  void
BX_CPU_C::ADD_EdGd(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    /* op2_32 is a register, RMAddr(i) is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
      }
    else {
      Write_RMW_virtual_dword(sum_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
}


  void
BX_CPU_C::ADD_GdEd(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    /* op1_32 is a register, RMAddr(i) is an index of a register */
    op1_32 = BX_READ_32BIT_REG(i->nnn());

    /* op2_32 is a register or memory reference */
    if (i->modC0()) {
      op2_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
      }

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
}


  void
BX_CPU_C::ADD_EAXId(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = i->Id();

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    RAX = sum_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
}

  void
BX_CPU_C::ADC_EdGd(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();


    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    /* op2_32 is a register, RMAddr(i) is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
      }
    else {
      Write_RMW_virtual_dword(sum_32);
      }

    SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                              temp_CF);
}


  void
BX_CPU_C::ADC_GdEd(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();


    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    /* op1_32 is a register, RMAddr(i) is an index of a register */
    op1_32 = BX_READ_32BIT_REG(i->nnn());

    /* op2_32 is a register or memory reference */
    if (i->modC0()) {
      op2_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
      }

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

    SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                             temp_CF);
}


  void
BX_CPU_C::ADC_EAXId(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();

    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = i->Id();

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    RAX = sum_32;

    SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                           temp_CF);
}




  void
BX_CPU_C::SBB_EdGd(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();


    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    /* op2_32 is a register, RMAddr(i) is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
      }
    else {
      Write_RMW_virtual_dword(diff_32);
      }

    SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
}


  void
BX_CPU_C::SBB_GdEd(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();


    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    /* op1_32 is a register, RMAddr(i) is an index of a register */
    op1_32 = BX_READ_32BIT_REG(i->nnn());

    /* op2_32 is a register or memory reference */
    if (i->modC0()) {
      op2_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
      }

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    BX_WRITE_32BIT_REGZ(i->nnn(), diff_32);

    SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
}


  void
BX_CPU_C::SBB_EAXId(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();

    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    op1_32 = EAX;

    op2_32 = i->Id();

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    RAX = diff_32;

    SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
}



  void
BX_CPU_C::SBB_EdId(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();


    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = i->Id();

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
      }
    else {
      Write_RMW_virtual_dword(diff_32);
      }

    SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
}


  void
BX_CPU_C::SUB_EdGd(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    /* op2_32 is a register, RMAddr(i) is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
      }
    else {
      Write_RMW_virtual_dword(diff_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
}


  void
BX_CPU_C::SUB_GdEd(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    /* op1_32 is a register, RMAddr(i) is an index of a register */
    op1_32 = BX_READ_32BIT_REG(i->nnn());

    /* op2_32 is a register or memory reference */
    if (i->modC0()) {
      op2_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
      }

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    BX_WRITE_32BIT_REGZ(i->nnn(), diff_32);

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
}

  void
BX_CPU_C::SUB_EAXId(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    op1_32 = EAX;

    op2_32 = i->Id();

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    RAX = diff_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
}


  void
BX_CPU_C::CMP_EdGd(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    /* op2_32 is a register, RMAddr(i) is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
}


  void
BX_CPU_C::CMP_GdEd(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    /* op1_32 is a register, RMAddr(i) is an index of a register */
    op1_32 = BX_READ_32BIT_REG(i->nnn());

    /* op2_32 is a register or memory reference */
    if (i->modC0()) {
      op2_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
      }

    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
}


  void
BX_CPU_C::CMP_EAXId(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    op1_32 = EAX;

    op2_32 = i->Id();

    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
}


  void
BX_CPU_C::CWDE(bxInstruction_c *i)
{
  /* CBW: no flags are effected */
    Bit32u temp;

    temp = (Bit16s) AX;
    RAX = temp;
}

  void
BX_CPU_C::CDQ(bxInstruction_c *i)
{
  /* CDQ: no flags are affected */

    if (EAX & 0x80000000) {
      RDX = 0xFFFFFFFF;
      }
    else {
      RDX = 0x00000000;
      }
}

// Some info on the opcodes at {0F,A6} and {0F,A7}
// On 386 steps A0-B0:
//   {OF,A6} = XBTS
//   {OF,A7} = IBTS
// On 486 steps A0-B0:
//   {OF,A6} = CMPXCHG 8
//   {OF,A7} = CMPXCHG 16|32
//
// On 486 >= B steps, and further processors, the
// CMPXCHG instructions were moved to opcodes:
//   {OF,B0} = CMPXCHG 8
//   {OF,B1} = CMPXCHG 16|32

  void
BX_CPU_C::CMPXCHG_XBTS(bxInstruction_c *i)
{
  BX_INFO(("CMPXCHG_XBTS:"));
  UndefinedOpcode(i);
}

  void
BX_CPU_C::CMPXCHG_IBTS(bxInstruction_c *i)
{
  BX_INFO(("CMPXCHG_IBTS:"));
  UndefinedOpcode(i);
}


  void
BX_CPU_C::XADD_EdGd(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

    Bit32u op2_32, op1_32, sum_32;

    /* XADD dst(r/m), src(r)
     * temp <-- src + dst         | sum = op2 + op1
     * src  <-- dst               | op2 = op1
     * dst  <-- tmp               | op1 = sum
     */

    /* op2 is a register, RMAddr(i) is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn());

    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    if (i->modC0()) {
      // and write destination into source
      // Note: if both op1 & op2 are registers, the last one written
      //       should be the sum, as op1 & op2 may be the same register.
      //       For example:  XADD AL, AL
      BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
      BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
      }
    else {
      Write_RMW_virtual_dword(sum_32);
      /* and write destination into source */
      BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
      }


    SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_XADD32);
#else

#endif
}



  void
BX_CPU_C::ADD_EdId(bxInstruction_c *i)
{
    Bit32u op2_32, op1_32, sum_32;

    op2_32 = i->Id();

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

#if (defined(__i386__) && defined(__GNUC__))
  Bit32u flags32;
  asm volatile (
    "addl %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (sum_32)
    : "1" (op1_32), "g" (op2_32)
    : "memory", "cc"
    );
#else
    sum_32 = op1_32 + op2_32;
#endif

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
      }
    else {
      Write_RMW_virtual_dword(sum_32);
      }

#if (defined(__i386__) && defined(__GNUC__))
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d5) | (flags32 & 0x000008d5);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
#endif
}

  void
BX_CPU_C::ADC_EdId(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();

    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    op2_32 = i->Id();

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), sum_32);
      }
    else {
      Write_RMW_virtual_dword(sum_32);
      }

    SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                              temp_CF);
}


  void
BX_CPU_C::SUB_EdId(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = i->Id();

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
      }
    else {
      Write_RMW_virtual_dword(diff_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
}

  void
BX_CPU_C::CMP_EdId(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = i->Id();

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
}




  void
BX_CPU_C::NEG_Ed(bxInstruction_c *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, diff_32;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    diff_32 = 0 - op1_32;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), diff_32);
      }
    else {
      Write_RMW_virtual_dword(diff_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, 0, diff_32, BX_INSTR_NEG32);
}


  void
BX_CPU_C::INC_Ed(bxInstruction_c *i)
{
    Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    op1_32++;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
      }
    else {
      Write_RMW_virtual_dword(op1_32);
      }

    SET_FLAGS_OSZAP_32(0, 0, op1_32, BX_INSTR_INC32);
}


  void
BX_CPU_C::DEC_Ed(bxInstruction_c *i)
{
    Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    op1_32--;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
      }
    else {
      Write_RMW_virtual_dword(op1_32);
      }

    SET_FLAGS_OSZAP_32(0, 0, op1_32, BX_INSTR_DEC32);
}


  void
BX_CPU_C::CMPXCHG_EdGd(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

    Bit32u op2_32, op1_32, diff_32;

    /* op1_32 is a register or memory reference */
    if (i->modC0()) {
      op1_32 = BX_READ_32BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
      }

    diff_32 = EAX - op1_32;

    SET_FLAGS_OSZAPC_32(EAX, op1_32, diff_32, BX_INSTR_CMP32);

    if (diff_32 == 0) {  // if accumulator == dest
      // ZF = 1
      set_ZF(1);
      // dest <-- src
      op2_32 = BX_READ_32BIT_REG(i->nnn());

      if (i->modC0()) {
        BX_WRITE_32BIT_REGZ(i->rm(), op2_32);
        }
      else {
        Write_RMW_virtual_dword(op2_32);
        }
      }
    else {
      // ZF = 0
      set_ZF(0);
      // accumulator <-- dest
      RAX = op1_32;
      }
#else
  BX_PANIC(("CMPXCHG_EdGd:"));
#endif
}

  void
BX_CPU_C::CMPXCHG8B(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 5) || (BX_CPU_LEVEL_HACKED >= 5)

    Bit32u op1_64_lo, op1_64_hi, diff;

    if (i->modC0()) {
      BX_INFO(("CMPXCHG8B: dest is reg: #UD"));
      UndefinedOpcode(i);
      }

    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op1_64_lo);
    read_RMW_virtual_dword(i->seg(), RMAddr(i) + 4, &op1_64_hi);

    diff = EAX - op1_64_lo;
    diff |= EDX - op1_64_hi;

//     SET_FLAGS_OSZAPC_32(EAX, op1_32, diff_32, BX_INSTR_CMP32);

    if (diff == 0) {  // if accumulator == dest
      // ZF = 1
      set_ZF(1);
      // dest <-- src
      Write_RMW_virtual_dword(ECX);
      write_virtual_dword(i->seg(), RMAddr(i), &EBX);
      }
    else {
      // ZF = 0
      set_ZF(0);
      // accumulator <-- dest
      RAX = op1_64_lo;
      RDX = op1_64_hi;
      }

#else
  BX_INFO(("CMPXCHG8B: not implemented yet"));
  UndefinedOpcode(i);
#endif
}
