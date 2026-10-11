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
//   SHxD_BwEwGwR    : ND=1, dst = vvvv, src1 = rm (or TMP after LOAD_Ew), src2 = reg, flags updated unless NF=1,
//                     the upper part of the destination register is always cleared
//   APX_NF_SHxD_EwGwR : ND=0 NF=1 register form, dst = rm, src = reg, flags are not updated
//   <op>_BwEwR      : ND=1, dst = vvvv, src = rm (or TMP after LOAD_Ew), flags updated unless NF=1,
//                     the upper part of the destination register is always cleared
//   APX_NF_<op>_EwR : ND=0 NF=1 register form, dst = rm, flags are not updated

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_BwEwGwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHLD_BwEwGw)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  Bit32u op1_16 = (Bit32u) BX_READ_16BIT_REG(i->src1());
  Bit32u op2_16 = (Bit32u) BX_READ_16BIT_REG(i->src2());
  Bit16u result_16 = (Bit16u) op1_16;

  if (count) {
    Bit32u temp_32 = (op1_16 << 16) | (op2_16); // double formed by op1:op2
    Bit32u result_32 = temp_32 << count;

    // count > 16: act like P6 and later (shift op1:op2:op1, undefined behavior)
    if (count > 16)
      result_32 |= (op1_16 << (count - 16));

    result_16 = (Bit16u)(result_32 >> 16);

    if (! i->getApxEvexNF()) {
      SET_FLAGS_OSZAPC_LOGIC_16(result_16);
      unsigned cf = (temp_32 >> (32 - count)) & 0x1;
      unsigned of = cf ^ (result_16 >> 15); // of = cf ^ result15
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHLD_EwGwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHLD_EwGw)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  if (count) {
    Bit32u op1_16 = (Bit32u) BX_READ_16BIT_REG(i->dst());
    Bit32u op2_16 = (Bit32u) BX_READ_16BIT_REG(i->src());
    Bit16u result_16;

    Bit32u temp_32 = (op1_16 << 16) | (op2_16); // double formed by op1:op2
    Bit32u result_32 = temp_32 << count;

    // count > 16: act like P6 and later (shift op1:op2:op1, undefined behavior)
    if (count > 16)
      result_32 |= (op1_16 << (count - 16));

    result_16 = (Bit16u)(result_32 >> 16);

    BX_WRITE_16BIT_REG(i->dst(), result_16);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_BwEwGwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHRD_BwEwGw)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  Bit32u op1_16 = (Bit32u) BX_READ_16BIT_REG(i->src1());
  Bit32u op2_16 = (Bit32u) BX_READ_16BIT_REG(i->src2());
  Bit16u result_16 = (Bit16u) op1_16;

  if (count) {
    Bit32u temp_32 = (op2_16 << 16) | op1_16; // double formed by op2:op1
    Bit32u result_32 = temp_32 >> count;

    // count > 16: act like P6 and later (shift op1:op2:op1, undefined behavior)
    if (count > 16)
      result_32 |= (op1_16 << (32 - count));

    result_16 = (Bit16u) result_32;

    if (! i->getApxEvexNF()) {
      SET_FLAGS_OSZAPC_LOGIC_16(result_16);
      unsigned cf = (op1_16 >> (count - 1)) & 0x1;
      unsigned of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1; // of = result14 ^ result15
      if (count > 16) cf = (op2_16 >> (count - 17)) & 0x1; // undefined flags behavior matching real HW
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHRD_EwGwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHRD_EwGw)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  if (count) {
    Bit32u op1_16 = (Bit32u) BX_READ_16BIT_REG(i->dst());
    Bit32u op2_16 = (Bit32u) BX_READ_16BIT_REG(i->src());
    Bit16u result_16;

    Bit32u temp_32 = (op2_16 << 16) | op1_16; // double formed by op2:op1
    Bit32u result_32 = temp_32 >> count;

    // count > 16: act like P6 and later (shift op1:op2:op1, undefined behavior)
    if (count > 16)
      result_32 |= (op1_16 << (32 - count));

    result_16 = (Bit16u) result_32;

    BX_WRITE_16BIT_REG(i->dst(), result_16);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_BwEwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROL_BwEw)
    count = CL;
  else
    count = i->Ib();

  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit16u result_16 = op1_16;

  if (count & 0x1f) {
    count &= 0x0f; // only use bottom 4 bits
    result_16 = rol16(op1_16, count);

    if (! i->getApxEvexNF()) {
      unsigned bit0  = (result_16 & 0x1);
      unsigned bit15 = (result_16 >> 15);
      // of = cf ^ result15
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(bit0 ^ bit15, bit0);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_ROL_EwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROL_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0xf; // only the rotate count matters, flags are not updated

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
    Bit16u result_16 = rol16(op1_16, count);
    BX_WRITE_16BIT_REG(i->dst(), result_16);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_BwEwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROR_BwEw)
    count = CL;
  else
    count = i->Ib();

  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit16u result_16 = op1_16;

  if (count & 0x1f) {
    count &= 0x0f; // use only 4 LSB's
    result_16 = ror16(op1_16, count);

    if (! i->getApxEvexNF()) {
      unsigned bit14 = (result_16 >> 14) & 1;
      unsigned bit15 = (result_16 >> 15) & 1;
      // of = result14 ^ result15
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(bit14 ^ bit15, bit15);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_ROR_EwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_ROR_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0xf; // only the rotate count matters, flags are not updated

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
    Bit16u result_16 = ror16(op1_16, count);
    BX_WRITE_16BIT_REG(i->dst(), result_16);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_BwEwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_RCL_BwEw)
    count = CL;
  else
    count = i->Ib();

  count = (count & 0x1f) % 17;

  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit16u result_16 = op1_16;

  if (count) {
    unsigned temp_CF = getB_CF();

    if (count==1) {
      result_16 = (op1_16 << 1) | temp_CF;
    }
    else if (count==16) {
      result_16 = (temp_CF << 15) | (op1_16 >> 1);
    }
    else { // 2..15
      result_16 = (op1_16 << count) | (temp_CF << (count - 1)) |
                  (op1_16 >> (17 - count));
    }

    unsigned cf = (op1_16 >> (16 - count)) & 0x1;
    unsigned of = cf ^ (result_16 >> 15); // of = cf ^ result15
    BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_BwEwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_RCR_BwEw)
    count = CL;
  else
    count = i->Ib();

  count = (count & 0x1f) % 17;

  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit16u result_16 = op1_16;

  if (count) {
    unsigned temp_CF = getB_CF();

    result_16 = (op1_16 >> count) | (temp_CF << (16 - count)) |
                (op1_16 << (17 - count));

    unsigned cf = (op1_16 >> (count - 1)) & 0x1;
    unsigned of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1; // of = result15 ^ result14
    BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_BwEwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHL_BwEw)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; /* use only 5 LSB's */

  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit16u result_16 = op1_16;

  if (count) {
    unsigned of = 0, cf = 0;

    if (count <= 16) {
      result_16 = (op1_16 << count);
      cf = (op1_16 >> (16 - count)) & 0x1;
      of = cf ^ (result_16 >> 15); // of = cf ^ result15
    }
    else {
      result_16 = 0;
    }

    if (! i->getApxEvexNF()) {
      SET_FLAGS_OSZAPC_LOGIC_16(result_16);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHL_EwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHL_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
    Bit16u result_16 = 0;
    if (count <= 16)
      result_16 = (op1_16 << count);
    BX_WRITE_16BIT_REG(i->dst(), result_16);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_BwEwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHR_BwEw)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; /* use only 5 LSB's */

  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit16u result_16 = op1_16;

  if (count) {
    result_16 = (op1_16 >> count);

    if (! i->getApxEvexNF()) {
      unsigned cf = (op1_16 >> (count - 1)) & 0x1;
      // note, that of == result15 if count == 1 and
      //            of == 0        if count >= 2
      unsigned of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1;

      SET_FLAGS_OSZAPC_LOGIC_16(result_16);
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(of, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SHR_EwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SHR_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
    Bit16u result_16 = (op1_16 >> count);
    BX_WRITE_16BIT_REG(i->dst(), result_16);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_BwEwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SAR_BwEw)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;  /* use only 5 LSB's */

  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit16u result_16 = op1_16;

  if (count) {
    result_16 = ((Bit16s) op1_16) >> count;

    if (! i->getApxEvexNF()) {
      unsigned cf = (((Bit16s) op1_16) >> (count - 1)) & 0x1;

      SET_FLAGS_OSZAPC_LOGIC_16(result_16);
      /* signed overflow cannot happen in SAR instruction */
      BX_CPU_THIS_PTR oszapc.set_flags_OxxxxC(0, cf);
    }
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SAR_EwR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_APX_SAR_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
    Bit16u result_16 = ((Bit16s) op1_16) >> count;
    BX_WRITE_16BIT_REG(i->dst(), result_16);
  }

  BX_NEXT_INSTR(i);
}

#endif
