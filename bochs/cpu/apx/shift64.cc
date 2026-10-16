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
//   SHxD_BqEqGqR    : ND=1, dst = vvvv, src1 = rm (or TMP after LOAD_Eq), src2 = reg, flags updated unless NF=1,
//                     the upper part of the destination register is always cleared
//                     also ND=0 NF=1 register form (def duplicates rm as dst and src1)
//   <op>_BqEqR      : ND=1, dst = vvvv, src = rm (or TMP after LOAD_Eq), flags updated unless NF=1,
//                     the upper part of the destination register is always cleared
//                     also ND=0 NF=1 register form (def duplicates rm as dst and src)

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_BqEqGqR(bxInstruction_c *i)
{
  unsigned count;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_SHLD_BqEqGq || ia_opcode == BX_IA_APX_SHLD_EqGq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f; // use only 6 LSB's

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2());
  Bit64u result_64 = op1_64;

  if (count) {
    result_64 = (op1_64 << count) | (op2_64 >> (64 - count));

    if (! i->getApxEvexNF()) {
      SET_FLAGS_OSZAPC_LOGIC_64(result_64);
      unsigned cf = (op1_64 >> (64 - count)) & 0x1;
      unsigned of = cf ^ (result_64 >> 63); // of = cf ^ result63
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_BqEqGqR(bxInstruction_c *i)
{
  unsigned count;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_SHRD_BqEqGq || ia_opcode == BX_IA_APX_SHRD_EqGq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f; // use only 6 LSB's

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2());
  Bit64u result_64 = op1_64;

  if (count) {
    result_64 = (op2_64 << (64 - count)) | (op1_64 >> count);

    if (! i->getApxEvexNF()) {
      SET_FLAGS_OSZAPC_LOGIC_64(result_64);
      unsigned cf = (op1_64 >> (count - 1)) & 0x1;
      unsigned of = ((result_64 << 1) ^ result_64) >> 63; // of = result62 ^ result63
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_BqEqR(bxInstruction_c *i)
{
  unsigned count;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_ROL_BqEq || ia_opcode == BX_IA_APX_ROL_Eq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = op1_64;

  if (count) {
    result_64 = rol64(op1_64, count);

    if (! i->getApxEvexNF()) {
      unsigned bit0  = (result_64 & 0x1);
      unsigned bit63 = (result_64 >> 63);
      // of = cf ^ result63
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(bit0 ^ bit63, bit0);
    }
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_BqEqR(bxInstruction_c *i)
{
  unsigned count;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_ROR_BqEq || ia_opcode == BX_IA_APX_ROR_Eq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = op1_64;

  if (count) {
    result_64 = ror64(op1_64, count);

    if (! i->getApxEvexNF()) {
      unsigned bit63 = (result_64 >> 63) & 1;
      unsigned bit62 = (result_64 >> 62) & 1;
      // of = result62 ^ result63
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(bit62 ^ bit63, bit63);
    }
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_BqEqR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_RCL_BqEq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = op1_64;

  if (count) {
    Bit64u temp_CF = getB_CF();

    if (count==1) {
      result_64 = (op1_64 << 1) | temp_CF;
    }
    else {
      result_64 = (op1_64 << count) | (temp_CF << (count - 1)) |
                  (op1_64 >> (65 - count));
    }

    unsigned cf = (op1_64 >> (64 - count)) & 0x1;
    unsigned of = cf ^ (result_64 >> 63); // of = cf ^ result63
    BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_BqEqR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_RCR_BqEq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = op1_64;

  if (count) {
    Bit64u temp_CF = getB_CF();

    if (count==1) {
      result_64 = (op1_64 >> 1) | (temp_CF << 63);
    }
    else {
      result_64 = (op1_64 >> count) | (temp_CF << (64 - count)) |
                  (op1_64 << (65 - count));
    }

    unsigned cf = (op1_64 >> (count - 1)) & 0x1;
    unsigned of = ((result_64 << 1) ^ result_64) >> 63; // of = result62 ^ result63
    BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_BqEqR(bxInstruction_c *i)
{
  unsigned count;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_SHL_BqEq || ia_opcode == BX_IA_APX_SHL_Eq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = op1_64;

  if (count) {
    /* count < 64, since only lower 6 bits used */
    result_64 = (op1_64 << count);

    if (! i->getApxEvexNF()) {
      unsigned cf = (op1_64 >> (64 - count)) & 0x1;
      unsigned of = cf ^ (result_64 >> 63);

      SET_FLAGS_OSZAPC_LOGIC_64(result_64);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_BqEqR(bxInstruction_c *i)
{
  unsigned count;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_SHR_BqEq || ia_opcode == BX_IA_APX_SHR_Eq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = op1_64;

  if (count) {
    result_64 = (op1_64 >> count);

    if (! i->getApxEvexNF()) {
      unsigned cf = (op1_64 >> (count - 1)) & 0x1;
      // note, that of == result63 if count == 1 and
      //            of == 0        if count >= 2
      unsigned of = ((result_64 << 1) ^ result_64) >> 63;

      SET_FLAGS_OSZAPC_LOGIC_64(result_64);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_BqEqR(bxInstruction_c *i)
{
  unsigned count;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_SAR_BqEq || ia_opcode == BX_IA_APX_SAR_Eq)
    count = CL;
  else
    count = i->Ib();

  count &= 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = op1_64;

  if (count) {
    /* count < 64, since only lower 6 bits used */
    result_64 = ((Bit64s) op1_64) >> count;

    if (! i->getApxEvexNF()) {
      unsigned cf = (op1_64 >> (count - 1)) & 1;

      SET_FLAGS_OSZAPC_LOGIC_64(result_64);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(0, cf); /* signed overflow cannot happen in SAR instruction */
    }
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

#endif
