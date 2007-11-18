/////////////////////////////////////////////////////////////////////////
// $Id: bit.cc,v 1.37 2007-11-18 18:24:45 sshwarts Exp $
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


#if BX_CPU_LEVEL >= 3

void BX_CPU_C::SETO_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETNO_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_OF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETB_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_CF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETNB_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_CF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETZ_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_ZF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETNZ_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_ZF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETBE_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_CF() || get_ZF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETNBE_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if ((get_CF()==0) && (get_ZF()==0))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETS_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_SF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETNS_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

  if (get_SF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETP_Eb(bxInstruction_c *i)
{
  Bit8u result_8;
 
  if (get_PF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETNP_Eb(bxInstruction_c *i)
{
  Bit8u result_8;

   if (get_PF() == 0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETL_Eb(bxInstruction_c *i)
{
  Bit8u result_8;
 
  if (getB_SF() != getB_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETNL_Eb(bxInstruction_c *i)
{
  Bit8u result_8;
 
  if (getB_SF() == getB_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETLE_Eb(bxInstruction_c *i)
{
  Bit8u result_8;
 
  if (get_ZF() || (getB_SF()!=getB_OF()))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::SETNLE_Eb(bxInstruction_c *i)
{
  Bit8u result_8;
 
  if ((get_ZF()==0) && (getB_SF()==getB_OF()))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
  }
  else {
    write_virtual_byte(i->seg(), RMAddr(i), &result_8);
  }
}

void BX_CPU_C::BSF_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  if (op2_16 == 0) {
    assert_ZF(); /* op1_16 undefined */
    return;
  }

  op1_16 = 0;
  while ( (op2_16 & 0x01) == 0 ) {
    op1_16++;
    op2_16 >>= 1;
  }

  SET_FLAGS_OSZAPC_RESULT_16(op1_16, BX_INSTR_BITSCAN16);

  /* now write result back to destination */
  BX_WRITE_16BIT_REG(i->nnn(), op1_16);
}

void BX_CPU_C::BSF_GdEd(bxInstruction_c *i)
{
  /* for 32 bit operand size mode */
  Bit32u op1_32, op2_32;

  /* op2_32 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  if (op2_32 == 0) {
    assert_ZF(); /* op1_32 undefined */
    return;
  }

  op1_32 = 0;
  while ( (op2_32 & 0x01) == 0 ) {
    op1_32++;
    op2_32 >>= 1;
  }

  SET_FLAGS_OSZAPC_RESULT_32(op1_32, BX_INSTR_BITSCAN32);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
}
 
#if BX_SUPPORT_X86_64
void BX_CPU_C::BSF_GqEq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64;

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  if (op2_64 == 0) {
    assert_ZF(); /* op1_64 undefined */
    return;
  }

  op1_64 = 0;
  while ( (op2_64 & 0x01) == 0 ) {
    op1_64++;
    op2_64 >>= 1;
  }

  SET_FLAGS_OSZAPC_RESULT_64(op1_64, BX_INSTR_BITSCAN64);

  /* now write result back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
}
#endif  // #if BX_SUPPORT_X86_64

void BX_CPU_C::BSR_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  if (op2_16 == 0) {
    assert_ZF(); /* op1_16 undefined */
    return;
  }

  op1_16 = 15;
  while ( (op2_16 & 0x8000) == 0 ) {
    op1_16--;
    op2_16 <<= 1;
  }

  SET_FLAGS_OSZAPC_RESULT_16(op1_16, BX_INSTR_BITSCAN16);

  /* now write result back to destination */
  BX_WRITE_16BIT_REG(i->nnn(), op1_16);
}

void BX_CPU_C::BSR_GdEd(bxInstruction_c *i)
{
  /* for 32 bit operand size mode */
  Bit32u op1_32, op2_32;

  /* op2_32 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  if (op2_32 == 0) {
    assert_ZF(); /* op1_32 undefined */
    return;
  }

  op1_32 = 31;
  while ( (op2_32 & 0x80000000) == 0 ) {
    op1_32--;
    op2_32 <<= 1;
  }

  SET_FLAGS_OSZAPC_RESULT_32(op1_32, BX_INSTR_BITSCAN32);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BSR_GqEq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64;

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  if (op2_64 == 0) {
    assert_ZF(); /* op1_64 undefined */
    return;
  }

  op1_64 = 63;
  while ( (op2_64 & BX_CONST64(0x8000000000000000)) == 0 ) {
    op1_64--;
    op2_64 <<= 1;
  }

  SET_FLAGS_OSZAPC_RESULT_64(op1_64, BX_INSTR_BITSCAN64);
  
  /* now write result back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
}
#endif

void BX_CPU_C::BSWAP_ERX(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)
  Bit32u val32, b0, b1, b2, b3;

  if (i->os32L() == 0) {
    BX_ERROR(("BSWAP with 16-bit opsize: undefined behavior !"));
  }

  val32 = BX_READ_32BIT_REG(i->opcodeReg());
  b0  = val32 & 0xff; val32 >>= 8;
  b1  = val32 & 0xff; val32 >>= 8;
  b2  = val32 & 0xff; val32 >>= 8;
  b3  = val32;
  val32 = (b0<<24) | (b1<<16) | (b2<<8) | b3;  // zero extended

  // in 64-bit mode, hi-order 32 bits are not modified
  BX_WRITE_32BIT_REGZ(i->opcodeReg(), val32);
#else
  BX_INFO(("BSWAP_ERX: required CPU >= 4, use --enable-cpu-level=4 option"));
  UndefinedOpcode(i);
#endif
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BSWAP_RRX(bxInstruction_c *i)
{
  Bit64u val64, b0, b1, b2, b3, b4, b5, b6, b7;

  val64 = BX_READ_64BIT_REG(i->opcodeReg());
  b0  = val64 & 0xff; val64 >>= 8;
  b1  = val64 & 0xff; val64 >>= 8;
  b2  = val64 & 0xff; val64 >>= 8;
  b3  = val64 & 0xff; val64 >>= 8;
  b4  = val64 & 0xff; val64 >>= 8;
  b5  = val64 & 0xff; val64 >>= 8;
  b6  = val64 & 0xff; val64 >>= 8;
  b7  = val64;
  val64 = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b4<<16) | (b4<<8) | b7;

  BX_WRITE_64BIT_REG(i->opcodeReg(), val64);
}
#endif  // #if BX_SUPPORT_X86_64

void BX_CPU_C::BT_EwGwM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  read_virtual_word(i->seg(), op1_addr, &op1_16);

  setB_CF((op1_16 >> index) & 0x01);
}

void BX_CPU_C::BT_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op2_16 &= 0x0f;
  setB_CF((op1_16 >> op2_16) & 0x01);
}

void BX_CPU_C::BT_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = RMAddr(i) + 4 * displacement32;

  /* pointer, segment address pair */
  read_virtual_dword(i->seg(), op1_addr, &op1_32);

  setB_CF((op1_32 >> index) & 0x01);
}

void BX_CPU_C::BT_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 &= 0x1f;

  setB_CF((op1_32 >> op2_32) & 0x01);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BT_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64;
  Bit64s displacement64;
  Bit64u index;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  read_virtual_qword(i->seg(), op1_addr, &op1_64);

  setB_CF((op1_64 >> index) & 0x01);
}

void BX_CPU_C::BT_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 &= 0x3f;
  setB_CF((op1_64 >> op2_64) & 0x01);
}
#endif

void BX_CPU_C::BTS_EwGwM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, bit_i, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16 & 0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);

  bit_i = (op1_16 >> index) & 0x01;
  op1_16 |= (((Bit16u) 1) << index);

  write_RMW_virtual_word(op1_16);

  setB_CF(bit_i);
}

void BX_CPU_C::BTS_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op2_16 &= 0x0f;
  setB_CF((op1_16 >> op2_16) & 0x01);
  op1_16 |= (((Bit16u) 1) << op2_16);

  /* now write diff back to destination */
  BX_WRITE_16BIT_REG(i->rm(), op1_16);
}

void BX_CPU_C::BTS_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, bit_i, index;
  Bit32s displacement32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = RMAddr(i) + 4 * displacement32;

  /* pointer, segment address pair */
  read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);

  bit_i = (op1_32 >> index) & 0x01;
  op1_32 |= (((Bit32u) 1) << index);

  write_RMW_virtual_dword(op1_32);

  setB_CF(bit_i);
}

void BX_CPU_C::BTS_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 &= 0x1f;
  setB_CF((op1_32 >> op2_32) & 0x01);
  op1_32 |= (((Bit32u) 1) << op2_32);

  /* now write diff back to destination */
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTS_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64, index;
  Bit64s displacement64;
  Bit64u bit_i;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);

  bit_i = (op1_64 >> index) & 0x01;
  op1_64 |= (((Bit64u) 1) << index);

  write_RMW_virtual_qword(op1_64);

  setB_CF(bit_i);
}

void BX_CPU_C::BTS_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 &= 0x3f;
  setB_CF((op1_64 >> op2_64) & 0x01);
  op1_64 |= (((Bit64u) 1) << op2_64);

  /* now write diff back to destination */
  BX_WRITE_64BIT_REG(i->rm(), op1_64);
}
#endif

void BX_CPU_C::BTR_EwGwM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);

  bx_bool temp_cf = (op1_16 >> index) & 0x01;
  op1_16 &= ~(((Bit16u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_word(op1_16);

  setB_CF(temp_cf);
}

void BX_CPU_C::BTR_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op2_16 &= 0x0f;
  setB_CF((op1_16 >> op2_16) & 0x01);
  op1_16 &= ~(((Bit16u) 1) << op2_16);

  /* now write diff back to destination */
  BX_WRITE_16BIT_REG(i->rm(), op1_16);
}

void BX_CPU_C::BTR_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = RMAddr(i) + 4 * displacement32;

  /* pointer, segment address pair */
  read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);

  bx_bool temp_cf = (op1_32 >> index) & 0x01;
  op1_32 &= ~(((Bit32u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_dword(op1_32);

  setB_CF(temp_cf);
}

void BX_CPU_C::BTR_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 &= 0x1f;
  setB_CF((op1_32 >> op2_32) & 0x01);
  op1_32 &= ~(((Bit32u) 1) << op2_32);

  /* now write diff back to destination */
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTR_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64, index;
  Bit64s displacement64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);

  bx_bool temp_cf = (op1_64 >> index) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_qword(op1_64);

  setB_CF(temp_cf);
}

void BX_CPU_C::BTR_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 &= 0x3f;
  setB_CF((op1_64 >> op2_64) & 0x01);
  op1_64 &= ~(((Bit64u) 1) << op2_64);

  /* now write diff back to destination */
  BX_WRITE_64BIT_REG(i->rm(), op1_64);
}
#endif 

void BX_CPU_C::BTC_EwGwM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index_16;
  Bit16s displacement16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index_16 = op2_16 & 0x0f;

  displacement16 = ((Bit16s) (op2_16 & 0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement16;
  read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);

  bx_bool temp_CF = (op1_16 >> index_16) & 0x01;
  op1_16 ^= (((Bit16u) 1) << index_16);  /* toggle bit */
  setB_CF(temp_CF);

  write_RMW_virtual_word(op1_16);

  setB_CF(temp_CF);
}

void BX_CPU_C::BTC_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op2_16 &= 0x0f;

  bx_bool temp_CF = (op1_16 >> op2_16) & 0x01;
  op1_16 ^= (((Bit16u) 1) << op2_16);  /* toggle bit */
  setB_CF(temp_CF);

  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  setB_CF(temp_CF);
}

void BX_CPU_C::BTC_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index_32;
  Bit32s displacement32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());
  index_32 = op2_32 & 0x1f;

  displacement32 = ((Bit32s) (op2_32 & 0xffffffe0)) / 32;
  op1_addr = RMAddr(i) + 4 * displacement32;
  read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);

  bx_bool temp_CF = (op1_32 >> index_32) & 0x01;
  op1_32 ^= (((Bit32u) 1) << index_32);  /* toggle bit */
  setB_CF(temp_CF);

  write_RMW_virtual_dword(op1_32);
}

void BX_CPU_C::BTC_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 &= 0x1f;

  bx_bool temp_CF = (op1_32 >> op2_32) & 0x01;
  op1_32 ^= (((Bit32u) 1) << op2_32);  /* toggle bit */
  setB_CF(temp_CF);

  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTC_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64;
  Bit64s displacement64;
  Bit64u index;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;
  read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);

  bx_bool temp_CF = (op1_64 >> index) & 0x01;
  op1_64 ^= (((Bit64u) 1) << index);  /* toggle bit */
  setB_CF(temp_CF);

  write_RMW_virtual_qword(op1_64);
}

void BX_CPU_C::BTC_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 &= 0x3f;

  bx_bool temp_CF = (op1_64 >> op2_64) & 0x01;
  op1_64 ^= (((Bit64u) 1) << op2_64);  /* toggle bit */
  setB_CF(temp_CF);

  BX_WRITE_64BIT_REG(i->rm(), op1_64);
}
#endif

void BX_CPU_C::BT_EwIb(bxInstruction_c *i)
{
  Bit16u op1_16;

  Bit8u op2_8 = i->Ib() & 0xf;

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  setB_CF((op1_16 >> op2_8) & 0x01);
}

void BX_CPU_C::BT_EdIb(bxInstruction_c *i)
{
  Bit32u op1_32;

  Bit8u op2_8 = i->Ib() & 0x1f;

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  setB_CF((op1_32 >> op2_8) & 0x01);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BT_EqIb(bxInstruction_c *i)
{
  Bit64u op1_64;

  Bit8u op2_8 = i->Ib() & 0x3f;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  }

  setB_CF((op1_64 >> op2_8) & 0x01);
}
#endif

void BX_CPU_C::BTS_EwIb(bxInstruction_c *i)
{
  Bit16u op1_16;

  Bit8u op2_8 = i->Ib() & 0xf;

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 |= (((Bit16u) 1) << op2_8);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), op1_16);
  }
  else {
    write_RMW_virtual_word(op1_16);
  }

  setB_CF(temp_CF);
}

void BX_CPU_C::BTS_EdIb(bxInstruction_c *i)
{
  Bit32u op1_32;

  Bit8u op2_8 = i->Ib() & 0x1f;

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 |= (((Bit32u) 1) << op2_8);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
  }
  else {
    write_RMW_virtual_dword(op1_32);
  }

  setB_CF(temp_CF);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTS_EqIb(bxInstruction_c *i)
{
  Bit64u op1_64;

  Bit8u op2_8 = i->Ib() & 0x3f;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  }

  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 |= (((Bit64u) 1) << op2_8);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_64BIT_REG(i->rm(), op1_64);
  }
  else {
    write_RMW_virtual_qword(op1_64);
  }

  setB_CF(temp_CF);
}
#endif

void BX_CPU_C::BTC_EwIb(bxInstruction_c *i)
{
  Bit16u op1_16;

  Bit8u op2_8 = i->Ib() & 0xf;

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 ^= (((Bit16u) 1) << op2_8);  /* toggle bit */
  setB_CF(temp_CF);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), op1_16);
  }
  else {
    write_RMW_virtual_word(op1_16);
  }
}

void BX_CPU_C::BTC_EdIb(bxInstruction_c *i)
{
  Bit32u op1_32;

  Bit8u op2_8 = i->Ib() & 0x1f;

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 ^= (((Bit32u) 1) << op2_8);  /* toggle bit */
  setB_CF(temp_CF);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
  }
  else {
    write_RMW_virtual_dword(op1_32);
  }
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTC_EqIb(bxInstruction_c *i)
{
  Bit64u op1_64;

  Bit8u op2_8 = i->Ib() & 0x3f;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  }

  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 ^= (((Bit64u) 1) << op2_8);  /* toggle bit */
  setB_CF(temp_CF);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_64BIT_REG(i->rm(), op1_64);
  }
  else {
    write_RMW_virtual_qword(op1_64);
  }
}
#endif

void BX_CPU_C::BTR_EwIb(bxInstruction_c *i)
{
  Bit16u op1_16;

  Bit8u op2_8 = i->Ib() & 0xf;

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 &= ~(((Bit16u) 1) << op2_8);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), op1_16);
  }
  else {
    write_RMW_virtual_word(op1_16);
  }

  setB_CF(temp_CF);
}

void BX_CPU_C::BTR_EdIb(bxInstruction_c *i)
{
  Bit32u op1_32;

  Bit8u op2_8 = i->Ib() & 0x1f;

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 &= ~(((Bit32u) 1) << op2_8);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
  }
  else {
    write_RMW_virtual_dword(op1_32);
  }

  setB_CF(temp_CF);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTR_EqIb(bxInstruction_c *i)
{
  Bit64u op1_64;

  Bit8u op2_8 = i->Ib() & 0x3f;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  }

  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << op2_8);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_64BIT_REG(i->rm(), op1_64);
  }
  else {
    write_RMW_virtual_qword(op1_64);
  }

  setB_CF(temp_CF);
}
#endif

/* 0F B8 */
void BX_CPU_C::POPCNT_GwEw(bxInstruction_c *i)
{
#if BX_SUPPORT_POPCNT || (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  Bit16u op1_16, op2_16;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  op1_16 = 0;
  while (op2_16 != 0) {
    if (op2_16 & 1) op1_16++;
    op2_16 >>= 1;
  }

  Bit32u flags = op1_16 ? 0 : EFlagsZFMask;
  setEFlagsOSZAPC(flags);

  /* now write result back to destination */
  BX_WRITE_16BIT_REG(i->nnn(), op1_16);
#else
  BX_INFO(("POPCNT_GwEw: required POPCNT support, use --enable-popcnt option"));
  UndefinedOpcode(i);
#endif
}

/* 0F B8 */
void BX_CPU_C::POPCNT_GdEd(bxInstruction_c *i)
{
#if BX_SUPPORT_POPCNT || (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  Bit32u op1_32, op2_32;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  op1_32 = 0;
  while (op2_32 != 0) {
    if (op2_32 & 1) op1_32++;
    op2_32 >>= 1;
  }

  Bit32u flags = op1_32 ? 0 : EFlagsZFMask;
  setEFlagsOSZAPC(flags);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
#else
  BX_INFO(("POPCNT_GdEd: required POPCNT support, use --enable-popcnt option"));
  UndefinedOpcode(i);
#endif
}

#if BX_SUPPORT_X86_64
/* 0F B8 */
void BX_CPU_C::POPCNT_GqEq(bxInstruction_c *i)
{
#if BX_SUPPORT_POPCNT || (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  Bit64u op1_64, op2_64;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  op1_64 = 0;
  while (op2_64 != 0) {
    if (op2_64 & 1) op1_64++;
    op2_64 >>= 1;
  }

  Bit32u flags = op1_64 ? 0 : EFlagsZFMask;
  setEFlagsOSZAPC(flags);

  /* now write result back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
#else
  BX_INFO(("POPCNT_GqEq: required POPCNT support, use --enable-popcnt option"));
  UndefinedOpcode(i);
#endif
}
#endif // BX_SUPPORT_X86_64

// 3-byte opcodes
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

#define CRC32_POLYNOMIAL BX_CONST64(0x11edc6f41)

#if (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
// primitives for CRC32 usage
static Bit8u BitReflect8(Bit8u val8)
{
   return ((val8 & 0x80) >> 7) |
          ((val8 & 0x40) >> 5) |
          ((val8 & 0x20) >> 3) |
          ((val8 & 0x10) >> 1) |
          ((val8 & 0x08) << 1) |
          ((val8 & 0x04) << 3) |
          ((val8 & 0x02) << 5) |
          ((val8 & 0x01) << 7);
}

BX_CPP_INLINE Bit16u BitReflect16(Bit16u val16)
{
   return ((Bit16u)(BitReflect8(val16 & 0xff)) << 8) | BitReflect8(val16 >> 8);
}

BX_CPP_INLINE Bit32u BitReflect32(Bit32u val32)
{
   return ((Bit32u)(BitReflect16(val32 & 0xffff)) << 16) | BitReflect16(val32 >> 16);
}

static Bit32u mod2_64bit(Bit64u divisor, Bit64u dividend)
{
    Bit64u remainder = dividend >> 32;

    for (int bitpos=31; bitpos>=0; bitpos--)
    {
	// copy one more bit from the dividend
	remainder = (remainder << 1) | ((dividend >> bitpos) & 1);

	// if MSB is set, then XOR divisor and get new remainder
	if (((remainder >> 32) & 1) == 1)
	{
	    remainder ^= divisor;
	}
    }

    return remainder;
}
#endif // (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)

void BX_CPU_C::CRC32_GdEb(bxInstruction_c *i)
{
#if (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  Bit8u  op1;

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op1);
  }

  Bit32u op2 = BX_READ_32BIT_REG(i->nnn());
  op2 = BitReflect32(op2);

  Bit64u tmp1 = ((Bit64u) BitReflect8 (op1)) << 32;
  Bit64u tmp2 = ((Bit64u) op2) <<  8;
  Bit64u tmp3 = tmp1 ^ tmp2;
  op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), BitReflect32(op2));
#else
  BX_INFO(("CRC32_GdEb: required SSE4_2 support, required SSE4.2, use --enable-sse and --enable-sse-extension options"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CRC32_GdEv(bxInstruction_c *i)
{
#if (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)

  Bit32u op2 = BX_READ_32BIT_REG(i->nnn());
  op2 = BitReflect32(op2);

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size */
  {
    Bit64u op1;

    if (i->modC0()) {
      op1 = BX_READ_64BIT_REG(i->rm());
    }
    else {
      read_virtual_qword(i->seg(), RMAddr(i), &op1);
    }

    Bit64u tmp1 = ((Bit64u) BitReflect32(op1 & 0xffffffff)) << 32;
    Bit64u tmp2 = ((Bit64u) op2) << 32;
    Bit64u tmp3 = tmp1 ^ tmp2;
    op2  = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
    tmp1 = ((Bit64u) BitReflect32(op1 >> 32)) << 32;
    tmp2 = ((Bit64u) op2) << 32;
    tmp3 = tmp1 ^ tmp2;
    op2  = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
  }
  else
#endif
  {
    if (i->os32L()) /* 32 bit operand size */
    {
      Bit32u op1;

      if (i->modC0()) {
        op1 = BX_READ_32BIT_REG(i->rm());
      }
      else {
        read_virtual_dword(i->seg(), RMAddr(i), &op1);
      }

      Bit64u tmp1 = ((Bit64u) BitReflect32(op1)) << 32;
      Bit64u tmp2 = ((Bit64u) op2) << 32;
      Bit64u tmp3 = tmp1 ^ tmp2;
      op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
    }
    else {          /* 16 bit operand size */
      Bit16u op1;

      if (i->modC0()) {
        op1 = BX_READ_16BIT_REG(i->rm());
      }
      else {
        read_virtual_word(i->seg(), RMAddr(i), &op1);
      }

      Bit64u tmp1 = ((Bit64u) BitReflect16(op1)) << 32;
      Bit64u tmp2 = ((Bit64u) op2) << 16;
      Bit64u tmp3 = tmp1 ^ tmp2;
      op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
    }
  }

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), BitReflect32(op2));
  
#else
  BX_INFO(("CRC32_GdEv: required SSE4_2 support, required SSE4.2, use --enable-sse and --enable-sse-extension options"));
  UndefinedOpcode(i);
#endif
}

#endif // (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

#endif // (BX_CPU_LEVEL >= 3)
