//  Copyright (C) 2000  MandrakeSoft S.A.
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





#include "bochs.h"


  void
BX_CPU_C::INC_RX(BxInstruction_t *i)
{
  Bit16u rx;

  rx = ++ BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].word.rx;
  SET_FLAGS_OSZAP_16(0, 0, rx, BX_INSTR_INC16);
}

  void
BX_CPU_C::DEC_RX(BxInstruction_t *i)
{
  Bit16u rx;

  rx = -- BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].word.rx;
  SET_FLAGS_OSZAP_16(0, 0, rx, BX_INSTR_DEC16);
}


  void
BX_CPU_C::ADD_EwGw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, sum_16;


    /* op2_16 is a register, i->rm_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, sum_16);
      }
    else {
      write_virtual_word(i->seg, i->rm_addr, &sum_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
}


  void
BX_CPU_C::ADD_GwEw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, sum_16;


    /* op1_16 is a register, i->rm_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn);

    /* op2_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    sum_16 = op1_16 + op2_16;
    /* now write sum back to destination */

    BX_WRITE_16BIT_REG(i->nnn, sum_16);

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
}


  void
BX_CPU_C::ADD_AXIw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = i->Iw;

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
}

  void
BX_CPU_C::ADC_EwGw(BxInstruction_t *i)
{
  Boolean temp_CF;
  Bit16u op2_16, op1_16, sum_16;

  temp_CF = get_CF();




    /* op2_16 is a register, i->rm_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, sum_16);
      }
    else {
      write_RMW_virtual_word(sum_16);
      }

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                              temp_CF);
}


  void
BX_CPU_C::ADC_GwEw(BxInstruction_t *i)
{
  Boolean temp_CF;
  Bit16u op1_16, op2_16, sum_16;

  temp_CF = get_CF();


    /* op1_16 is a register, i->rm_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn);

    /* op2_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    BX_WRITE_16BIT_REG(i->nnn, sum_16);

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                             temp_CF);
}


  void
BX_CPU_C::ADC_AXIw(BxInstruction_t *i)
{
  Boolean temp_CF;
  Bit16u op1_16, op2_16, sum_16;

  temp_CF = get_CF();

    op1_16 = AX;

    op2_16 = i->Iw;

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    AX = sum_16;

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                           temp_CF);
}




  void
BX_CPU_C::SBB_EwGw(BxInstruction_t *i)
{
  Boolean temp_CF;
  Bit16u op2_16, op1_16, diff_16;


  temp_CF = get_CF();


    /* op2_16 is a register, i->rm_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, diff_16);
      }
    else {
      write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
}


  void
BX_CPU_C::SBB_GwEw(BxInstruction_t *i)
{
  Boolean temp_CF;

  temp_CF = get_CF();


    Bit16u op1_16, op2_16, diff_16;


    /* op1_16 is a register, i->rm_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn);

    /* op2_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    BX_WRITE_16BIT_REG(i->nnn, diff_16);

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
}


  void
BX_CPU_C::SBB_AXIw(BxInstruction_t *i)
{
  Boolean temp_CF;
  Bit16u op1_16, op2_16, diff_16;

  temp_CF = get_CF();


    op1_16 = AX;

    op2_16 = i->Iw;

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    AX = diff_16;

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
}



  void
BX_CPU_C::SBB_EwIw(BxInstruction_t *i)
{
  Boolean temp_CF;
  Bit16u op2_16, op1_16, diff_16;

  temp_CF = get_CF();




    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, diff_16);
      }
    else {
      write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
}


  void
BX_CPU_C::SUB_EwGw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, diff_16;


    /* op2_16 is a register, i->rm_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, diff_16);
      }
    else {
      write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
}


  void
BX_CPU_C::SUB_GwEw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, diff_16;


    /* op1_16 is a register, i->rm_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn);

    /* op2_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    BX_WRITE_16BIT_REG(i->nnn, diff_16);

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
}

  void
BX_CPU_C::SUB_AXIw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, diff_16;

    op1_16 = AX;

    op2_16 = i->Iw;

    diff_16 = op1_16 - op2_16;


    /* now write diff back to destination */
    AX = diff_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
}


  void
BX_CPU_C::CMP_EwGw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, diff_16;


    /* op2_16 is a register, i->rm_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
}


  void
BX_CPU_C::CMP_GwEw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, diff_16;


    /* op1_16 is a register, i->rm_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(i->nnn);

    /* op2_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
}


  void
BX_CPU_C::CMP_AXIw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, diff_16;

    op1_16 = AX;

    op2_16 = i->Iw;

    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
}


  void
BX_CPU_C::CBW(BxInstruction_t *i)
{
  /* CBW: no flags are effected */

  AX = (Bit8s) AL;
}

  void
BX_CPU_C::CWD(BxInstruction_t *i)
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
BX_CPU_C::XADD_EwGw(BxInstruction_t *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

    Bit16u op2_16, op1_16, sum_16;

    /* XADD dst(r/m), src(r)
     * temp <-- src + dst         | sum = op2 + op1
     * src  <-- dst               | op2 = op1
     * dst  <-- tmp               | op1 = sum
     */

    /* op2 is a register, i->rm_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      // and write destination into source
      // Note: if both op1 & op2 are registers, the last one written
      //       should be the sum, as op1 & op2 may be the same register.
      //       For example:  XADD AL, AL
      BX_WRITE_16BIT_REG(i->nnn, op1_16);
      BX_WRITE_16BIT_REG(i->rm, sum_16);
      }
    else {
      write_RMW_virtual_word(sum_16);
      /* and write destination into source */
      BX_WRITE_16BIT_REG(i->nnn, op1_16);
      }


    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_XADD16);
#else
  bx_panic("XADD_EvGv: not supported on < 80486\n");
#endif
}



  void
BX_CPU_C::ADD_EwIw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, sum_16;


    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, sum_16);
      }
    else {
      write_RMW_virtual_word(sum_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
}

  void
BX_CPU_C::ADC_EwIw(BxInstruction_t *i)
{
  Boolean temp_CF;
  Bit16u op2_16, op1_16, sum_16;

  temp_CF = get_CF();


    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, sum_16);
      }
    else {
      write_RMW_virtual_word(sum_16);
      }

    SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                              temp_CF);
}


  void
BX_CPU_C::SUB_EwIw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, diff_16);
      }
    else {
      write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
}

  void
BX_CPU_C::CMP_EwIw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
}



  void
BX_CPU_C::NEG_Ew(BxInstruction_t *i)
{
    Bit16u op1_16, diff_16;


    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    diff_16 = 0 - op1_16;

    /* now write diff back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, diff_16);
      }
    else {
      write_RMW_virtual_word(diff_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, 0, diff_16, BX_INSTR_NEG16);
}


  void
BX_CPU_C::INC_Ew(BxInstruction_t *i)
{
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    op1_16++;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, op1_16);
      }
    else {
      write_RMW_virtual_word(op1_16);
      }

    SET_FLAGS_OSZAP_16(0, 0, op1_16, BX_INSTR_INC16);
}


  void
BX_CPU_C::DEC_Ew(BxInstruction_t *i)
{
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    op1_16--;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, op1_16);
      }
    else {
      write_RMW_virtual_word(op1_16);
      }

    SET_FLAGS_OSZAP_16(0, 0, op1_16, BX_INSTR_DEC16);
}


  void
BX_CPU_C::CMPXCHG_EwGw(BxInstruction_t *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

    Bit16u op2_16, op1_16, diff_16;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    diff_16 = AX - op1_16;

    SET_FLAGS_OSZAPC_16(AX, op1_16, diff_16, BX_INSTR_CMP16);

    if (diff_16 == 0) {  // if accumulator == dest
      // ZF = 1
      set_ZF(1);
      // dest <-- src
      op2_16 = BX_READ_16BIT_REG(i->nnn);

      if (i->mod == 0xc0) {
        BX_WRITE_16BIT_REG(i->rm, op2_16);
        }
      else {
        write_RMW_virtual_word(op2_16);
        }
      }
    else {
      // ZF = 0
      set_ZF(0);
      // accumulator <-- dest
      AX = op1_16;
      }

#else
  bx_panic("CMPXCHG_EwGw:\n");
#endif
}
