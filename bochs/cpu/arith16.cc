/////////////////////////////////////////////////////////////////////////
// $Id: arith16.cc,v 1.19 2002-09-24 18:33:37 kevinlawton Exp $
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
BX_CPU_C::INC_RX(bxInstruction_c *i)
{
#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "incw %1 \n\t"
    "pushfl  \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx)
    : "1" (BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d4) | (flags32 & 0x000008d4);
  BX_CPU_THIS_PTR lf_flags_status &= 0x00000f;
#else
  Bit16u rx;
  rx = ++ BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx;

  SET_FLAGS_OSZAP_16(0, 0, rx, BX_INSTR_INC16);
#endif
}

  void
BX_CPU_C::DEC_RX(bxInstruction_c *i)
{
#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "decw %1 \n\t"
    "pushfl  \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx)
    : "1" (BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d4) | (flags32 & 0x000008d4);
  BX_CPU_THIS_PTR lf_flags_status &= 0x00000f;
#else
  Bit16u rx;

  rx = -- BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx;

  SET_FLAGS_OSZAP_16(0, 0, rx, BX_INSTR_DEC16);
#endif
}


  void
BX_CPU_C::ADD_EwGw(bxInstruction_c *i)
{
    Bit16u op2_16, op1_16, sum_16;


    /* op2_16 is a register, RMAddr(i) is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), sum_16);
      }
    else {
      write_virtual_word(i->seg(), RMAddr(i), &sum_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
}


  void
BX_CPU_C::ADD_GwEw(bxInstruction_c *i)
{
    Bit16u op1_16, op2_16, sum_16;


    /* op1_16 is a register, RMAddr(i) is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn());

    /* op2_16 is a register or memory reference */
    if (i->modC0()) {
      op2_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "addw %3, %1 \n\t"
      "pushfl      \n\t"
      "popl   %0"
      : "=g" (flags32), "=r" (sum_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
      (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d5) | (flags32 & 0x000008d5);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    sum_16 = op1_16 + op2_16;
#endif

    BX_WRITE_16BIT_REG(i->nnn(), sum_16);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
#endif
}


  void
BX_CPU_C::ADD_AXIw(bxInstruction_c *i)
{
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = i->Iw();

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
}

  void
BX_CPU_C::ADC_EwGw(bxInstruction_c *i)
{
  Boolean temp_CF;
  Bit16u op2_16, op1_16, sum_16;

  temp_CF = getB_CF();




    /* op2_16 is a register, RMAddr(i) is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), sum_16);
      }
    else {
      Write_RMW_virtual_word(sum_16);
      }

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                              temp_CF);
}


  void
BX_CPU_C::ADC_GwEw(bxInstruction_c *i)
{
  Boolean temp_CF;
  Bit16u op1_16, op2_16, sum_16;

  temp_CF = getB_CF();


    /* op1_16 is a register, RMAddr(i) is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn());

    /* op2_16 is a register or memory reference */
    if (i->modC0()) {
      op2_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    BX_WRITE_16BIT_REG(i->nnn(), sum_16);

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                             temp_CF);
}


  void
BX_CPU_C::ADC_AXIw(bxInstruction_c *i)
{
  Boolean temp_CF;
  Bit16u op1_16, op2_16, sum_16;

  temp_CF = getB_CF();

    op1_16 = AX;

    op2_16 = i->Iw();

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    AX = sum_16;

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                           temp_CF);
}




  void
BX_CPU_C::SBB_EwGw(bxInstruction_c *i)
{
  Boolean temp_CF;
  Bit16u op2_16, op1_16, diff_16;


  temp_CF = getB_CF();


    /* op2_16 is a register, RMAddr(i) is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), diff_16);
      }
    else {
      Write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
}


  void
BX_CPU_C::SBB_GwEw(bxInstruction_c *i)
{
  Boolean temp_CF;

  temp_CF = getB_CF();


    Bit16u op1_16, op2_16, diff_16;


    /* op1_16 is a register, RMAddr(i) is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn());

    /* op2_16 is a register or memory reference */
    if (i->modC0()) {
      op2_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    BX_WRITE_16BIT_REG(i->nnn(), diff_16);

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
}


  void
BX_CPU_C::SBB_AXIw(bxInstruction_c *i)
{
  Boolean temp_CF;
  Bit16u op1_16, op2_16, diff_16;

  temp_CF = getB_CF();


    op1_16 = AX;

    op2_16 = i->Iw();

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    AX = diff_16;

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
}



  void
BX_CPU_C::SBB_EwIw(bxInstruction_c *i)
{
  Boolean temp_CF;
  Bit16u op2_16, op1_16, diff_16;

  temp_CF = getB_CF();




    op2_16 = i->Iw();

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), diff_16);
      }
    else {
      Write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
}


  void
BX_CPU_C::SUB_EwGw(bxInstruction_c *i)
{
    Bit16u op2_16, op1_16, diff_16;


    /* op2_16 is a register, RMAddr(i) is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), diff_16);
      }
    else {
      Write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
}


  void
BX_CPU_C::SUB_GwEw(bxInstruction_c *i)
{
    Bit16u op1_16, op2_16, diff_16;


    /* op1_16 is a register, RMAddr(i) is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn());

    /* op2_16 is a register or memory reference */
    if (i->modC0()) {
      op2_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    BX_WRITE_16BIT_REG(i->nnn(), diff_16);

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
}

  void
BX_CPU_C::SUB_AXIw(bxInstruction_c *i)
{
    Bit16u op1_16, op2_16, diff_16;

    op1_16 = AX;

    op2_16 = i->Iw();

    diff_16 = op1_16 - op2_16;


    /* now write diff back to destination */
    AX = diff_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
}


  void
BX_CPU_C::CMP_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op1_16);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "cmpw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d5) | (flags32 & 0x000008d5);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit16u diff_16;
  diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
#endif
}


  void
BX_CPU_C::CMP_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "cmpw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d5) | (flags32 & 0x000008d5);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit16u diff_16;
  diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
#endif
}


  void
BX_CPU_C::CMP_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = AX;

  op2_16 = i->Iw();

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "cmpw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d5) | (flags32 & 0x000008d5);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit16u diff_16;
  diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
#endif
}


  void
BX_CPU_C::CBW(bxInstruction_c *i)
{
  /* CBW: no flags are effected */

  AX = (Bit8s) AL;
}

  void
BX_CPU_C::CWD(bxInstruction_c *i)
{
  /* CWD: no flags are affected */

    if (AX & 0x8000) {
      DX = 0xFFFF;
      }
    else {
      DX = 0x0000;
      }
}


  void
BX_CPU_C::XADD_EwGw(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

    Bit16u op2_16, op1_16, sum_16;

    /* XADD dst(r/m), src(r)
     * temp <-- src + dst         | sum = op2 + op1
     * src  <-- dst               | op2 = op1
     * dst  <-- tmp               | op1 = sum
     */

    /* op2 is a register, RMAddr(i) is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    if (i->modC0()) {
      // and write destination into source
      // Note: if both op1 & op2 are registers, the last one written
      //       should be the sum, as op1 & op2 may be the same register.
      //       For example:  XADD AL, AL
      BX_WRITE_16BIT_REG(i->nnn(), op1_16);
      BX_WRITE_16BIT_REG(i->rm(), sum_16);
      }
    else {
      Write_RMW_virtual_word(sum_16);
      /* and write destination into source */
      BX_WRITE_16BIT_REG(i->nnn(), op1_16);
      }


    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_XADD16);
#else
  BX_PANIC(("XADD_EvGv: not supported on < 80486"));
#endif
}



  void
BX_CPU_C::ADD_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, sum_16;

  op2_16 = i->Iw();

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "addw %3, %1 \n\t"
      "pushfl      \n\t"
      "popl   %0"
      : "=g" (flags32), "=r" (sum_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
      (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d5) | (flags32 & 0x000008d5);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    sum_16 = op1_16 + op2_16;
#endif
    BX_WRITE_16BIT_REG(i->rm(), sum_16);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;
    asm (
      "addw %3, %1 \n\t"
      "pushfl      \n\t"
      "popl   %0"
      : "=g" (flags32), "=r" (sum_16)
      : "1" (op1_16), "g" (op2_16)
      : "cc"
      );
    BX_CPU_THIS_PTR eflags.val32 =
      (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d5) | (flags32 & 0x000008d5);
    BX_CPU_THIS_PTR lf_flags_status = 0;
#else
    sum_16 = op1_16 + op2_16;
#endif
    Write_RMW_virtual_word(sum_16);
    }

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
#endif
}

  void
BX_CPU_C::ADC_EwIw(bxInstruction_c *i)
{
  Boolean temp_CF;
  Bit16u op2_16, op1_16, sum_16;

  temp_CF = getB_CF();


    op2_16 = i->Iw();

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), sum_16);
      }
    else {
      Write_RMW_virtual_word(sum_16);
      }

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                              temp_CF);
}


  void
BX_CPU_C::SUB_EwIw(bxInstruction_c *i)
{
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = i->Iw();

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), diff_16);
      }
    else {
      Write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
}

  void
BX_CPU_C::CMP_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16;

  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op1_16);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;
  asm (
    "cmpw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
  BX_CPU_THIS_PTR eflags.val32 =
    (BX_CPU_THIS_PTR eflags.val32 & ~0x000008d5) | (flags32 & 0x000008d5);
  BX_CPU_THIS_PTR lf_flags_status = 0;
#else
  Bit16u diff_16;
  diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
#endif
}



  void
BX_CPU_C::NEG_Ew(bxInstruction_c *i)
{
    Bit16u op1_16, diff_16;


    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    diff_16 = 0 - op1_16;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), diff_16);
      }
    else {
      Write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, 0, diff_16, BX_INSTR_NEG16);
}


  void
BX_CPU_C::INC_Ew(bxInstruction_c *i)
{
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    op1_16++;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), op1_16);
      }
    else {
      Write_RMW_virtual_word(op1_16);
      }

    SET_FLAGS_OSZAP_16(0, 0, op1_16, BX_INSTR_INC16);
}


  void
BX_CPU_C::DEC_Ew(bxInstruction_c *i)
{
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    op1_16--;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), op1_16);
      }
    else {
      Write_RMW_virtual_word(op1_16);
      }

    SET_FLAGS_OSZAP_16(0, 0, op1_16, BX_INSTR_DEC16);
}


  void
BX_CPU_C::CMPXCHG_EwGw(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

    Bit16u op2_16, op1_16, diff_16;

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      }

    diff_16 = AX - op1_16;

    SET_FLAGS_OSZAPC_16(AX, op1_16, diff_16, BX_INSTR_CMP16);

    if (diff_16 == 0) {  // if accumulator == dest
      // ZF = 1
      set_ZF(1);
      // dest <-- src
      op2_16 = BX_READ_16BIT_REG(i->nnn());

      if (i->modC0()) {
        BX_WRITE_16BIT_REG(i->rm(), op2_16);
        }
      else {
        Write_RMW_virtual_word(op2_16);
        }
      }
    else {
      // ZF = 0
      set_ZF(0);
      // accumulator <-- dest
      AX = op1_16;
      }

#else
  BX_PANIC(("CMPXCHG_EwGw:"));
#endif
}
