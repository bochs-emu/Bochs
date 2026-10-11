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
//   SHxD_BdEdGdR    : ND=1, dst = vvvv, src1 = rm (or TMP after LOAD_Ed), src2 = reg, flags updated unless NF=1,
//                     the upper part of the destination register is always cleared
//   APX_NF_SHxD_EdGdR : ND=0 NF=1 register form, dst = rm, src = reg, flags are not updated
//   <op>_BdEdR      : ND=1, dst = vvvv, src = rm (or TMP after LOAD_Ed), flags updated unless NF=1,
//                     the upper part of the destination register is always cleared
//   APX_NF_<op>_EdR : ND=0 NF=1 register form, dst = rm, flags are not updated

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_BdEdGdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHLD_BdEdGd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = (op1_32 << count) | (op2_32 >> (32 - count));

    if (! i->getApxEvexNF()) {
      SET_FLAGS_OSZAPC_LOGIC_32(result_32);
      unsigned cf = (op1_32 >> (32 - count)) & 0x1;
      unsigned of = cf ^ (result_32 >> 31); // of = cf ^ result31
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHLD_EdGdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHLD_EdGd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u result_32 = op1_32;

  if (count) {
    Bit32u op2_32 = BX_READ_32BIT_REG(i->src());
    result_32 = (op1_32 << count) | (op2_32 >> (32 - count));
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_BdEdGdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHRD_BdEdGd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = (op2_32 << (32 - count)) | (op1_32 >> count);

    if (! i->getApxEvexNF()) {
      SET_FLAGS_OSZAPC_LOGIC_32(result_32);
      unsigned cf = (op1_32 >> (count - 1)) & 0x1;
      unsigned of = ((result_32 << 1) ^ result_32) >> 31; // of = result30 ^ result31
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHRD_EdGdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHRD_EdGd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u result_32 = op1_32;

  if (count) {
    Bit32u op2_32 = BX_READ_32BIT_REG(i->src());
    result_32 = (op2_32 << (32 - count)) | (op1_32 >> count);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_BdEdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROL_BdEd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = rol32(op1_32, count);

    if (! i->getApxEvexNF()) {
      unsigned bit0  = (result_32 & 0x1);
      unsigned bit31 = (result_32 >> 31);
      // of = cf ^ result31
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(bit0 ^ bit31, bit0);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_ROL_EdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROL_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = rol32(op1_32, count);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_BdEdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROR_BdEd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = ror32(op1_32, count);

    if (! i->getApxEvexNF()) {
      unsigned bit31 = (result_32 >> 31) & 1;
      unsigned bit30 = (result_32 >> 30) & 1;
      // of = result30 ^ result31
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(bit30 ^ bit31, bit31);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_ROR_EdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = ror32(op1_32, count);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_BdEdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_RCL_BdEd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = op1_32;

  if (count) {
    Bit32u temp_CF = getB_CF();

    if (count==1) {
      result_32 = (op1_32 << 1) | temp_CF;
    }
    else {
      result_32 = (op1_32 << count) | (temp_CF << (count - 1)) |
                  (op1_32 >> (33 - count));
    }

    unsigned cf = (op1_32 >> (32 - count)) & 0x1;
    unsigned of = cf ^ (result_32 >> 31); // of = cf ^ result31
    BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_BdEdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_RCR_BdEd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = op1_32;

  if (count) {
    Bit32u temp_CF = getB_CF();

    if (count==1) {
      result_32 = (op1_32 >> 1) | (temp_CF << 31);
    }
    else {
      result_32 = (op1_32 >> count) | (temp_CF << (32 - count)) |
                  (op1_32 << (33 - count));
    }

    unsigned cf = (op1_32 >> (count - 1)) & 0x1;
    unsigned of = ((result_32 << 1) ^ result_32) >> 31; // of = result30 ^ result31
    BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_BdEdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHL_BdEd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = op1_32;

  if (count) {
    /* count < 32, since only lower 5 bits used */
    result_32 = (op1_32 << count);

    if (! i->getApxEvexNF()) {
      unsigned cf = (op1_32 >> (32 - count)) & 0x1;
      unsigned of = cf ^ (result_32 >> 31);

      SET_FLAGS_OSZAPC_LOGIC_32(result_32);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHL_EdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHL_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = (op1_32 << count);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_BdEdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHR_BdEd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = (op1_32 >> count);

    if (! i->getApxEvexNF()) {
      unsigned cf = (op1_32 >> (count - 1)) & 0x1;
      // note, that of == result31 if count == 1 and
      //            of == 0        if count >= 2
      unsigned of = ((result_32 << 1) ^ result_32) >> 31;

      SET_FLAGS_OSZAPC_LOGIC_32(result_32);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHR_EdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = (op1_32 >> count);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_BdEdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SAR_BdEd)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = op1_32;

  if (count) {
    /* count < 32, since only lower 5 bits used */
    result_32 = ((Bit32s) op1_32) >> count;

    if (! i->getApxEvexNF()) {
      unsigned cf = (op1_32 >> (count - 1)) & 1;

      SET_FLAGS_OSZAPC_LOGIC_32(result_32);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(0, cf); /* signed overflow cannot happen in SAR instruction */
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SAR_EdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SAR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u result_32 = op1_32;

  if (count) {
    result_32 = ((Bit32s) op1_32) >> count;
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

#endif
