/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2026  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_APX

#include "scalar_arith.h"
#include "decoder/ia_opcodes.h"

// EVEX map4 shift/rotate forms:
//   <op>_BbEbR      : ND=1, dst = vvvv, src = rm (or TMP after LOAD_Eb), flags updated unless NF=1,
//                     the upper part of the destination register is always cleared
//   APX_NF_<op>_EbR : ND=0 NF=1 register form, dst = rm, flags are not updated

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_BbEbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROL_BbEb)
    count = CL;
  else
    count = i->Ib();

  Bit8u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit8u result_8 = op1_8;

  if (count & 0x1f) {
    count &= 0x7; // use only lowest 3 bits
    result_8 = rol8(op1_8, count);

    if (! i->getApxEvexNF()) {
      unsigned bit0 = (result_8 &  1);
      unsigned bit7 = (result_8 >> 7);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(bit0 ^ bit7, bit0);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_ROL_EbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROL_Eb)
    count = CL;
  else
    count = i->Ib();

  count &= 0x7; // only the rotate count matters, flags are not updated

  if (count) {
    Bit8u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
    Bit8u result_8 = rol8(op1_8, count);
    BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), result_8);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_BbEbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROR_BbEb)
    count = CL;
  else
    count = i->Ib();

  Bit8u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit8u result_8 = op1_8;

  if (count & 0x1f) {
    count &= 0x7; // use only lowest 3 bits
    result_8 = ror8(op1_8, count);

    if (! i->getApxEvexNF()) {
      unsigned bit6 = (result_8 >> 6) & 1;
      unsigned bit7 = (result_8 >> 7) & 1;
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(bit6 ^ bit7, bit7);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_ROR_EbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROR_Eb)
    count = CL;
  else
    count = i->Ib();

  count &= 0x7; // only the rotate count matters, flags are not updated

  if (count) {
    Bit8u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
    Bit8u result_8 = ror8(op1_8, count);
    BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), result_8);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_BbEbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_RCL_BbEb)
    count = CL;
  else
    count = i->Ib();

  count = (count & 0x1f) % 9;

  Bit8u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit8u result_8 = op1_8;

  if (count) {
    unsigned temp_CF = getB_CF();

    if (count==1) {
      result_8 = (op1_8 << 1) | temp_CF;
    }
    else {
      result_8 = (op1_8 << count) | (temp_CF << (count - 1)) |
                 (op1_8 >> (9 - count));
    }

    unsigned cf = (op1_8 >> (8 - count)) & 0x01;
    unsigned of = cf ^ (result_8 >> 7);  // of = cf ^ result7
    BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_BbEbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_RCR_BbEb)
    count = CL;
  else
    count = i->Ib();

  count = (count & 0x1f) % 9;

  Bit8u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit8u result_8 = op1_8;

  if (count) {
    unsigned temp_CF = getB_CF();

    result_8 = (op1_8 >> count) | (temp_CF << (8 - count)) |
               (op1_8 << (9 - count));

    unsigned cf = (op1_8 >> (count - 1)) & 0x1;
    unsigned of = (((result_8 << 1) ^ result_8) >> 7) & 0x1; // of = result6 ^ result7
    BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_BbEbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHL_BbEb)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit8u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit8u result_8 = op1_8;

  if (count) {
    unsigned of = 0, cf = 0;

    if (count <= 8) {
      result_8 = (op1_8 << count);
      cf = (op1_8 >> (8 - count)) & 0x1;
      of = cf ^ (result_8 >> 7);
    }
    else {
      result_8 = 0;
    }

    if (! i->getApxEvexNF()) {
      SET_FLAGS_OSZAPC_LOGIC_8(result_8);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHL_EbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHL_Eb)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit8u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
    Bit8u result_8 = 0;
    if (count <= 8)
      result_8 = (op1_8 << count);
    BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), result_8);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_BbEbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHR_BbEb)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit8u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit8u result_8 = op1_8;

  if (count) {
    result_8 = (op1_8 >> count);

    if (! i->getApxEvexNF()) {
      unsigned cf = (op1_8 >> (count - 1)) & 0x1;
      // note, that of == result7 if count == 1 and
      //            of == 0       if count >= 2
      unsigned of = (((result_8 << 1) ^ result_8) >> 7) & 0x1;

      SET_FLAGS_OSZAPC_LOGIC_8(result_8);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHR_EbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHR_Eb)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit8u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
    Bit8u result_8 = (op1_8 >> count);
    BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), result_8);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_BbEbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SAR_BbEb)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit8u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit8u result_8 = op1_8;

  if (count) {
    result_8 = ((Bit8s) op1_8) >> count;

    if (! i->getApxEvexNF()) {
      unsigned cf = (((Bit8s) op1_8) >> (count - 1)) & 0x1;

      SET_FLAGS_OSZAPC_LOGIC_8(result_8);
      /* signed overflow cannot happen in SAR instruction */
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(0, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SAR_EbR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SAR_Eb)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit8u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
    Bit8u result_8 = ((Bit8s) op1_8) >> count;
    BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), result_8);
  }

  BX_NEXT_INSTR(i);
}

#endif
