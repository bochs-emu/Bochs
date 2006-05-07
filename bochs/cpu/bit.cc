/////////////////////////////////////////////////////////////////////////
// $Id: bit.cc,v 1.29 2006-05-07 20:56:40 sshwarts Exp $
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
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_CPU_LEVEL >= 3

#if BX_SUPPORT_X86_64==0
// Make life easier merging cpu64 and cpu code.
#define RAX EAX
#define RBX EBX
#define RCX ECX
#define RDX EDX
#define RSP ESP
#define RSI ESI
#define RDI EDI
#define RBP EBP
#endif

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
  BX_WRITE_32BIT_REG(i->opcodeReg(), val32);
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

void BX_CPU_C::BT_EwGw(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    op2_16 &= 0x0f;
    set_CF((op1_16 >> op2_16) & 0x01);
    return;
  }

  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  read_virtual_word(i->seg(), op1_addr, &op1_16);

  set_CF((op1_16 >> index) & 0x01);
}

void BX_CPU_C::BT_EdGd(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    op2_32 &= 0x1f;
    set_CF((op1_32 >> op2_32) & 0x01);
    return;
  }

  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = RMAddr(i) + 4 * displacement32;

  /* pointer, segment address pair */
  read_virtual_dword(i->seg(), op1_addr, &op1_32);

  set_CF((op1_32 >> index) & 0x01);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BT_EqGq(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64;
  Bit64s displacement64;
  Bit64u index;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    op2_64 &= 0x3f;
    set_CF((op1_64 >> op2_64) & 0x01);
    return;
  }

  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  read_virtual_qword(i->seg(), op1_addr, &op1_64);

  set_CF((op1_64 >> index) & 0x01);
}
#endif

void BX_CPU_C::BTS_EwGw(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, bit_i, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    op2_16 &= 0x0f;
    set_CF((op1_16 >> op2_16) & 0x01);
    op1_16 |= (((Bit16u) 1) << op2_16);

    /* now write diff back to destination */
    BX_WRITE_16BIT_REG(i->rm(), op1_16);
    return;
  }

  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16 & 0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);

  bit_i = (op1_16 >> index) & 0x01;
  op1_16 |= (((Bit16u) 1) << index);

  write_RMW_virtual_word(op1_16);

  set_CF(bit_i);
}

void BX_CPU_C::BTS_EdGd(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, bit_i, index;
  Bit32s displacement32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    op2_32 &= 0x1f;
    set_CF((op1_32 >> op2_32) & 0x01);
    op1_32 |= (((Bit32u) 1) << op2_32);

    /* now write diff back to destination */
    BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
    return;
  }

  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = RMAddr(i) + 4 * displacement32;

  /* pointer, segment address pair */
  read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);

  bit_i = (op1_32 >> index) & 0x01;
  op1_32 |= (((Bit32u) 1) << index);

  write_RMW_virtual_dword(op1_32);

  set_CF(bit_i);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTS_EqGq(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64, index;
  Bit64s displacement64;
  Bit64u bit_i;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    op2_64 &= 0x3f;
    set_CF((op1_64 >> op2_64) & 0x01);
    op1_64 |= (((Bit64u) 1) << op2_64);

    /* now write diff back to destination */
    BX_WRITE_64BIT_REG(i->rm(), op1_64);
    return;
  }

  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);

  bit_i = (op1_64 >> index) & 0x01;
  op1_64 |= (((Bit64u) 1) << index);

  write_RMW_virtual_qword(op1_64);

  set_CF(bit_i);
}
#endif

void BX_CPU_C::BTR_EwGw(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    op2_16 &= 0x0f;
    set_CF((op1_16 >> op2_16) & 0x01);
    op1_16 &= ~(((Bit16u) 1) << op2_16);

    /* now write diff back to destination */
    BX_WRITE_16BIT_REG(i->rm(), op1_16);
    return;
  }

  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);

  bx_bool temp_cf = (op1_16 >> index) & 0x01;
  op1_16 &= ~(((Bit16u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_word(op1_16);

  set_CF(temp_cf);
}

void BX_CPU_C::BTR_EdGd(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    op2_32 &= 0x1f;
    set_CF((op1_32 >> op2_32) & 0x01);
    op1_32 &= ~(((Bit32u) 1) << op2_32);

    /* now write diff back to destination */
    BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
    return;
  }

  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = RMAddr(i) + 4 * displacement32;

  /* pointer, segment address pair */
  read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);

  bx_bool temp_cf = (op1_32 >> index) & 0x01;
  op1_32 &= ~(((Bit32u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_cf);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTR_EqGq(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64, index;
  Bit64s displacement64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    op2_64 &= 0x3f;
    set_CF((op1_64 >> op2_64) & 0x01);
    op1_64 &= ~(((Bit64u) 1) << op2_64);

    /* now write diff back to destination */
    BX_WRITE_64BIT_REG(i->rm(), op1_64);
    return;
  }

  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);

  bx_bool temp_cf = (op1_64 >> index) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_cf);
}
#endif 

void BX_CPU_C::BTC_EwGw(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index_16;
  Bit16s displacement16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index_16 = op2_16 & 0x0f;

  /* op1_16 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    op1_addr = 0; // keep compiler happy
  }
  else {
    displacement16 = ((Bit16s) (op2_16 & 0xfff0)) / 16;
    op1_addr = RMAddr(i) + 2 * displacement16;
    read_RMW_virtual_word(i->seg(), op1_addr, &op1_16);
  }

  bx_bool temp_CF = (op1_16 >> index_16) & 0x01;
  op1_16 ^= (((Bit16u) 1) << index_16);  /* toggle bit */
  set_CF(temp_CF);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), op1_16);
  }
  else {
    write_RMW_virtual_word(op1_16);
  }

  set_CF(temp_CF);
}

void BX_CPU_C::BTC_EdGd(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index_32;
  Bit32s displacement32;

  op2_32 = BX_READ_32BIT_REG(i->nnn());
  index_32 = op2_32 & 0x1f;

  /* op1_32 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
    op1_addr = 0; // keep compiler happy
  }
  else {
    displacement32 = ((Bit32s) (op2_32 & 0xffffffe0)) / 32;
    op1_addr = RMAddr(i) + 4 * displacement32;
    read_RMW_virtual_dword(i->seg(), op1_addr, &op1_32);
  }

  bx_bool temp_CF = (op1_32 >> index_32) & 0x01;
  op1_32 ^= (((Bit32u) 1) << index_32);  /* toggle bit */
  set_CF(temp_CF);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
  }
  else {
    write_RMW_virtual_dword(op1_32);
  }
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BTC_EqGq(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64;
  Bit64s displacement64;
  Bit64u index;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    op1_addr = 0; // keep compiler happy
  }
  else {
    displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
    op1_addr = RMAddr(i) + 8 * displacement64;
    read_RMW_virtual_qword(i->seg(), op1_addr, &op1_64);
  }

  bx_bool temp_CF = (op1_64 >> index) & 0x01;
  op1_64 ^= (((Bit64u) 1) << index);  /* toggle bit */
  set_CF(temp_CF);

  /* now write diff back to destination */
  if (i->modC0()) {
    BX_WRITE_64BIT_REG(i->rm(), op1_64);
  }
  else {
    write_RMW_virtual_qword(op1_64);
  }
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

  set_CF((op1_16 >> op2_8) & 0x01);
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

  set_CF((op1_32 >> op2_8) & 0x01);
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

  set_CF((op1_64 >> op2_8) & 0x01);
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

  set_CF(temp_CF);
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

  set_CF(temp_CF);
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

  set_CF(temp_CF);
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
  set_CF(temp_CF);

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
  set_CF(temp_CF);

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
  set_CF(temp_CF);

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

  set_CF(temp_CF);
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

  set_CF(temp_CF);
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

  set_CF(temp_CF);
}
#endif

#endif /* BX_CPU_LEVEL >= 3 */
