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
BX_CPU_C::ADD_EbGb(BxInstruction_t *i)
{
  Bit8u op2, op1, sum;

  /* op2 is a register, i->rm_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  sum = op1 + op2;

  /* now write sum back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, sum);
    }
  else {
    write_RMW_virtual_byte(sum);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD8);
}



  void
BX_CPU_C::ADD_GbEb(BxInstruction_t *i)
{
  Bit8u op1, op2, sum;


  /* op1 is a register, i->rm_addr is an index of a register */
  op1 = BX_READ_8BIT_REG(i->nnn);

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2);
    }

  sum = op1 + op2;

  /* now write sum back to destination, which is a register */
  BX_WRITE_8BIT_REG(i->nnn, sum);

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD8);
}


  void
BX_CPU_C::ADD_ALIb(BxInstruction_t *i)
{
  Bit8u op1, op2, sum;


  op1 = AL;

  op2 = i->Ib;

  sum = op1 + op2;

  /* now write sum back to destination, which is a register */
  AL = sum;

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD8);
}


  void
BX_CPU_C::ADC_EbGb(BxInstruction_t *i)
{
  Bit8u op2, op1, sum;
  Boolean temp_CF;

  temp_CF = get_CF();


  /* op2 is a register, i->rm_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  sum = op1 + op2 + temp_CF;


  /* now write sum back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, sum);
    }
  else {
    write_RMW_virtual_byte(sum);
    }

  SET_FLAGS_OSZAPC_8_CF(op1, op2, sum, BX_INSTR_ADC8, temp_CF);
}


  void
BX_CPU_C::ADC_GbEb(BxInstruction_t *i)
{
  Bit8u op1, op2, sum;
  Boolean temp_CF;

  temp_CF = get_CF();


  /* op1 is a register, i->rm_addr is an index of a register */
  op1 = BX_READ_8BIT_REG(i->nnn);

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2);
    }

  sum = op1 + op2 + temp_CF;

  SET_FLAGS_OSZAPC_8_CF(op1, op2, sum, BX_INSTR_ADC8,
                           temp_CF);

  /* now write sum back to destination, which is a register */
  BX_WRITE_8BIT_REG(i->nnn, sum);
}


  void
BX_CPU_C::ADC_ALIb(BxInstruction_t *i)
{
  Bit8u op1, op2, sum;
  Boolean temp_CF;

  temp_CF = get_CF();


  op1 = AL;

  op2 = i->Ib;

  sum = op1 + op2 + temp_CF;

  /* now write sum back to destination, which is a register */
  AL = sum;

  SET_FLAGS_OSZAPC_8_CF(op1, op2, sum, BX_INSTR_ADC8,
                           temp_CF);
}


  void
BX_CPU_C::SBB_EbGb(BxInstruction_t *i)
{
  Bit8u op2_8, op1_8, diff_8;
  Boolean temp_CF;

  temp_CF = get_CF();


  /* op2 is a register, i->rm_addr is an index of a register */
  op2_8 = BX_READ_8BIT_REG(i->nnn);

  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  diff_8 = op1_8 - (op2_8 + temp_CF);

  /* now write diff back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, diff_8);
    }
  else {
    write_RMW_virtual_byte(diff_8);
    }

  SET_FLAGS_OSZAPC_8_CF(op1_8, op2_8, diff_8, BX_INSTR_SBB8,
                           temp_CF);
}


  void
BX_CPU_C::SBB_GbEb(BxInstruction_t *i)
{
  Bit8u op1_8, op2_8, diff_8;
  Boolean temp_CF;

  temp_CF = get_CF();


  /* op1 is a register, i->rm_addr is an index of a register */
  op1_8 = BX_READ_8BIT_REG(i->nnn);

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2_8);
    }

  diff_8 = op1_8 - (op2_8 + temp_CF);

  /* now write diff back to destination, which is a register */
  BX_WRITE_8BIT_REG(i->nnn, diff_8);

  SET_FLAGS_OSZAPC_8_CF(op1_8, op2_8, diff_8, BX_INSTR_SBB8,
                           temp_CF);
}


  void
BX_CPU_C::SBB_ALIb(BxInstruction_t *i)
{
  Bit8u op1_8, op2_8, diff_8;
  Boolean temp_CF;

  temp_CF = get_CF();


  op1_8 = AL;

  op2_8 = i->Ib;

  diff_8 = op1_8 - (op2_8 + temp_CF);

  /* now write diff back to destination, which is a register */
  AL = diff_8;

  SET_FLAGS_OSZAPC_8_CF(op1_8, op2_8, diff_8, BX_INSTR_SBB8,
                           temp_CF);
}


  void
BX_CPU_C::SBB_EbIb(BxInstruction_t *i)
{
  Bit8u op2_8, op1_8, diff_8;
  Boolean temp_CF;

  temp_CF = get_CF();

  op2_8 = i->Ib;

  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  diff_8 = op1_8 - (op2_8 + temp_CF);

  /* now write diff back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, diff_8);
    }
  else {
    write_RMW_virtual_byte(diff_8);
    }

  SET_FLAGS_OSZAPC_8_CF(op1_8, op2_8, diff_8, BX_INSTR_SBB8,
                           temp_CF);
}



  void
BX_CPU_C::SUB_EbGb(BxInstruction_t *i)
{
  Bit8u op2_8, op1_8, diff_8;


  /* op2 is a register, i->rm_addr is an index of a register */
  op2_8 = BX_READ_8BIT_REG(i->nnn);

  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  diff_8 = op1_8 - op2_8;

  /* now write diff back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, diff_8);
    }
  else {
    write_RMW_virtual_byte(diff_8);
    }

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB8);
}


  void
BX_CPU_C::SUB_GbEb(BxInstruction_t *i)
{
  Bit8u op1_8, op2_8, diff_8;


  /* op1 is a register, i->rm_addr is an index of a register */
  op1_8 = BX_READ_8BIT_REG(i->nnn);

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2_8);
    }

  diff_8 = op1_8 - op2_8;

  /* now write diff back to destination, which is a register */
  BX_WRITE_8BIT_REG(i->nnn, diff_8);

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB8);
}


  void
BX_CPU_C::SUB_ALIb(BxInstruction_t *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = AL;

  op2_8 = i->Ib;

  diff_8 = op1_8 - op2_8;

  /* now write diff back to destination, which is a register */
  AL = diff_8;

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB8);
}



  void
BX_CPU_C::CMP_EbGb(BxInstruction_t *i)
{
  Bit8u op2_8, op1_8, diff_8;


  /* op2 is a register, i->rm_addr is an index of a register */
  op2_8 = BX_READ_8BIT_REG(i->nnn);

  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMP8);
}


  void
BX_CPU_C::CMP_GbEb(BxInstruction_t *i)
{
  Bit8u op1_8, op2_8, diff_8;

  /* op1 is a register, i->rm_addr is an index of a register */
  op1_8 = BX_READ_8BIT_REG(i->nnn);

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2_8);
    }

  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMP8);
}



  void
BX_CPU_C::CMP_ALIb(BxInstruction_t *i)
{
  Bit8u op1_8, op2_8, diff_8;


  op1_8 = AL;

  op2_8 = i->Ib;

  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMP8);
}


  void
BX_CPU_C::XADD_EbGb(BxInstruction_t *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

  Bit8u op2, op1, sum;

  /* XADD dst(r/m8), src(r8)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  /* op2 is a register, i->rm_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  sum = op1 + op2;

  /* now write sum back to destination */
  if (i->mod == 0xc0) {
    // and write destination into source
    // Note: if both op1 & op2 are registers, the last one written
    //       should be the sum, as op1 & op2 may be the same register.
    //       For example:  XADD AL, AL
    BX_WRITE_8BIT_REG(i->nnn, op1);
    BX_WRITE_8BIT_REG(i->rm, sum);
    }
  else {
    write_RMW_virtual_byte(sum);
    /* and write destination into source */
    BX_WRITE_8BIT_REG(i->nnn, op1);
    }


  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_XADD8);
#else
  BX_PANIC(("XADD_EbGb: not supported on < 80486\n"));
#endif
}


  void
BX_CPU_C::ADD_EbIb(BxInstruction_t *i)
{
  Bit8u op2, op1, sum;


  op2 = i->Ib;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  sum = op1 + op2;

  /* now write sum back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, sum);
    }
  else {
    write_RMW_virtual_byte(sum);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD8);
}

  void
BX_CPU_C::ADC_EbIb(BxInstruction_t *i)
{
  Bit8u op2, op1, sum;
  Boolean temp_CF;

  temp_CF = get_CF();

  op2 = i->Ib;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  sum = op1 + op2 + temp_CF;

  /* now write sum back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, sum);
    }
  else {
    write_RMW_virtual_byte(sum);
    }

  SET_FLAGS_OSZAPC_8_CF(op1, op2, sum, BX_INSTR_ADC8,
                           temp_CF);
}


  void
BX_CPU_C::SUB_EbIb(BxInstruction_t *i)
{
  Bit8u op2_8, op1_8, diff_8;


  op2_8 = i->Ib;

  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  diff_8 = op1_8 - op2_8;

  /* now write diff back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, diff_8);
    }
  else {
    write_RMW_virtual_byte(diff_8);
    }

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB8);
}

  void
BX_CPU_C::CMP_EbIb(BxInstruction_t *i)
{
  Bit8u op2_8, op1_8, diff_8;

  op2_8 = i->Ib;

  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMP8);
}


  void
BX_CPU_C::NEG_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, diff_8;

  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  diff_8 = 0 - op1_8;

  /* now write diff back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, diff_8);
    }
  else {
    write_RMW_virtual_byte(diff_8);
    }

  SET_FLAGS_OSZAPC_8(op1_8, 0, diff_8, BX_INSTR_NEG8);
}


  void
BX_CPU_C::INC_Eb(BxInstruction_t *i)
{
  Bit8u  op1;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }


  op1++;

  /* now write sum back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, op1);
    }
  else {
    write_RMW_virtual_byte(op1);
    }

  SET_FLAGS_OSZAP_8(0, 0, op1, BX_INSTR_INC8);
}


  void
BX_CPU_C::DEC_Eb(BxInstruction_t *i)
{
  Bit8u op1_8;

  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  op1_8--;

  /* now write sum back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, op1_8);
    }
  else {
    write_RMW_virtual_byte(op1_8);
    }

  SET_FLAGS_OSZAP_8(0, 0, op1_8, BX_INSTR_DEC8);
}


  void
BX_CPU_C::CMPXCHG_EbGb(BxInstruction_t *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)
  Bit8u op2_8, op1_8, diff_8;


  /* op1_8 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  diff_8 = AL - op1_8;

  SET_FLAGS_OSZAPC_8(AL, op1_8, diff_8, BX_INSTR_CMP8);

  if (diff_8 == 0) {  // if accumulator == dest
    // ZF = 1
    set_ZF(1);
    // dest <-- src
    op2_8 = BX_READ_8BIT_REG(i->nnn);

    if (i->mod == 0xc0) {
      BX_WRITE_8BIT_REG(i->rm, op2_8);
      }
    else {
      write_RMW_virtual_byte(op2_8);
      }
    }
  else {
    // ZF = 0
    set_ZF(0);
    // accumulator <-- dest
    AL = op1_8;
    }

#else
  BX_PANIC(("CMPXCHG_EbGb:\n"));
#endif
}
