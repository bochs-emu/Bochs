/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_EwGwM(bxInstruction_c *i)
{
  Bit32u temp_32, result_32;
  unsigned count;
  unsigned of, cf;

  /* op1:op2 << count.  result stored in op1 */
  if (i->getIaOpcode() == BX_IA_SHLD_EwGw)
    count = CL;
  else // BX_IA_SHLD_EwGwIb
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = (Bit32u) read_RMW_virtual_word(i->seg(), eaddr);

  if (count) {
    Bit32u op2_16 = (Bit32u) BX_READ_16BIT_REG(i->src());

    /* count < 32, since only lower 5 bits used */
    temp_32 = (op1_16 << 16) | (op2_16); // double formed by op1:op2
    result_32 = temp_32 << count;

    // hack to act like x86 SHLD when count > 16
    if (count > 16) {
      // for Pentium processor, when count > 16, actually shifting op1:op2:op2 << count,
      // it is the same as shifting op2:op2 by count-16
      // For P6 and later (CPU_LEVEL >= 6), when count > 16, actually shifting op1:op2:op1 << count,
      // which is the same as shifting op2:op1 by count-16
      // The behavior is undefined so both ways are correct, we prefer P6 way of implementation
      result_32 |= (op1_16 << (count - 16));
    }

    Bit16u result_16 = (Bit16u)(result_32 >> 16);

    write_RMW_virtual_word(result_16);

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);

    cf = (temp_32 >> (32 - count)) & 0x1;
    of = cf ^ (result_16 >> 15); // of = cf ^ result15
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_EwGwR(bxInstruction_c *i)
{
  Bit32u temp_32, result_32;
  unsigned count;
  unsigned of, cf;

  /* op1:op2 << count.  result stored in op1 */
  if (i->getIaOpcode() == BX_IA_SHLD_EwGw)
    count = CL;
  else // BX_IA_SHLD_EwGwIb
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  if (count) {
    Bit32u op1_16 = (Bit32u) BX_READ_16BIT_REG(i->dst());
    Bit32u op2_16 = (Bit32u) BX_READ_16BIT_REG(i->src());

    /* count < 32, since only lower 5 bits used */
    temp_32 = (op1_16 << 16) | (op2_16); // double formed by op1:op2
    result_32 = temp_32 << count;

    // hack to act like x86 SHLD when count > 16
    if (count > 16) {
      // for Pentium processor, when count > 16, actually shifting op1:op2:op2 << count,
      // it is the same as shifting op2:op2 by count-16
      // For P6 and later (CPU_LEVEL >= 6), when count > 16, actually shifting op1:op2:op1 << count,
      // which is the same as shifting op2:op1 by count-16
      // The behavior is undefined so both ways are correct, we prefer P6 way of implementation
      result_32 |= (op1_16 << (count - 16));
    }

    Bit16u result_16 = (Bit16u)(result_32 >> 16);

    BX_WRITE_16BIT_REG(i->dst(), result_16);

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);

    cf = (temp_32 >> (32 - count)) & 0x1;
    of = cf ^ (result_16 >> 15); // of = cf ^ result15
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_EwGwM(bxInstruction_c *i)
{
  Bit32u temp_32, result_32;
  unsigned count;
  unsigned cf, of;

  if (i->getIaOpcode() == BX_IA_SHRD_EwGw)
    count = CL;
  else // BX_IA_SHRD_EwGwIb
    count = i->Ib();

  count &= 0x1f; /* use only 5 LSB's */

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = (Bit32u) read_RMW_virtual_word(i->seg(), eaddr);

  if (count) {
    Bit32u op2_16 = (Bit32u) BX_READ_16BIT_REG(i->src());

    /* count < 32, since only lower 5 bits used */
    temp_32 = (op2_16 << 16) | op1_16; // double formed by op2:op1
    result_32 = temp_32 >> count;

    // hack to act like x86 SHRD when count > 16
    if (count > 16) {
      // for Pentium processor, when count > 16, actually shifting op2:op2:op1 >> count,
      // it is the same as shifting op2:op2 by count-16
      // For P6 and later (CPU_LEVEL >= 6), when count > 16, actually shifting op1:op2:op1 >> count,
      // which is the same as shifting op1:op2 by count-16
      // The behavior is undefined so both ways are correct, we prefer P6 way of implementation
      result_32 |= (op1_16 << (32 - count));
    }

    Bit16u result_16 = (Bit16u) result_32;

    write_RMW_virtual_word(result_16);

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);

    cf = (op1_16 >> (count - 1)) & 0x1;
    of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1; // of = result14 ^ result15
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_EwGwR(bxInstruction_c *i)
{
  Bit32u temp_32, result_32;
  unsigned count;
  unsigned cf, of;

  if (i->getIaOpcode() == BX_IA_SHRD_EwGw)
    count = CL;
  else // BX_IA_SHRD_EwGwIb
    count = i->Ib();

  count &= 0x1f; /* use only 5 LSB's */

  if (count) {
    Bit32u op1_16 = (Bit32u) BX_READ_16BIT_REG(i->dst());
    Bit32u op2_16 = (Bit32u) BX_READ_16BIT_REG(i->src());

    /* count < 32, since only lower 5 bits used */
    temp_32 = (op2_16 << 16) | op1_16; // double formed by op2:op1
    result_32 = temp_32 >> count;

    // hack to act like x86 SHRD when count > 16
    if (count > 16) {
      // for Pentium processor, when count > 16, actually shifting op2:op2:op1 >> count,
      // it is the same as shifting op2:op2 by count-16
      // For P6 and later (CPU_LEVEL >= 6), when count > 16, actually shifting op1:op2:op1 >> count,
      // which is the same as shifting op1:op2 by count-16
      // The behavior is undefined so both ways are correct, we prefer P6 way of implementation
      result_32 |= (op1_16 << (32 - count));
    }

    Bit16u result_16 = (Bit16u) result_32;

    BX_WRITE_16BIT_REG(i->dst(), result_16);

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);

    cf = (op1_16 >> (count - 1)) & 0x1;
    of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1; // of = result14 ^ result15
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_EwM(bxInstruction_c *i)
{
  unsigned count;
  unsigned bit0, bit15;

  if (i->getIaOpcode() == BX_IA_ROL_Ew)
    count = CL;
  else
    count = i->Ib();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);

  if ((count & 0x0f) == 0) {
    if (count & 0x10) {
      bit0  = (op1_16 & 0x1);
      bit15 = (op1_16 >> 15);
      // of = cf ^ result15
      SET_FLAGS_OxxxxC(bit0 ^ bit15, bit0);
    }
  }
  else {
    count &= 0x0f; // only use bottom 4 bits

    Bit16u result_16 = (op1_16 << count) | (op1_16 >> (16 - count));

    write_RMW_virtual_word(result_16);

    bit0  = (result_16 & 0x1);
    bit15 = (result_16 >> 15);
    // of = cf ^ result15
    SET_FLAGS_OxxxxC(bit0 ^ bit15, bit0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_EwR(bxInstruction_c *i)
{
  unsigned count;
  unsigned bit0, bit15;

  if (i->getIaOpcode() == BX_IA_ROL_Ew)
    count = CL;
  else
    count = i->Ib();

  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());

  if ((count & 0x0f) == 0) {
    if (count & 0x10) {
      bit0  = (op1_16 & 0x1);
      bit15 = (op1_16 >> 15);
      // of = cf ^ result15
      SET_FLAGS_OxxxxC(bit0 ^ bit15, bit0);
    }
  }
  else {
    count &= 0x0f; // only use bottom 4 bits

    Bit16u result_16 = (op1_16 << count) | (op1_16 >> (16 - count));

    BX_WRITE_16BIT_REG(i->dst(), result_16);

    bit0  = (result_16 & 0x1);
    bit15 = (result_16 >> 15);
    // of = cf ^ result15
    SET_FLAGS_OxxxxC(bit0 ^ bit15, bit0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_EwM(bxInstruction_c *i)
{
  unsigned count;
  unsigned bit14, bit15;

  if (i->getIaOpcode() == BX_IA_ROR_Ew)
    count = CL;
  else
    count = i->Ib();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);

  if ((count & 0x0f) == 0) {
    if (count & 0x10) {
      bit14 = (op1_16 >> 14) & 1;
      bit15 = (op1_16 >> 15) & 1;
      // of = result14 ^ result15
      SET_FLAGS_OxxxxC(bit14 ^ bit15, bit15);
    }
  }
  else {
    count &= 0x0f;  // use only 4 LSB's

    Bit16u result_16 = (op1_16 >> count) | (op1_16 << (16 - count));

    write_RMW_virtual_word(result_16);

    bit14 = (result_16 >> 14) & 1;
    bit15 = (result_16 >> 15) & 1;
    // of = result14 ^ result15
    SET_FLAGS_OxxxxC(bit14 ^ bit15, bit15);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_EwR(bxInstruction_c *i)
{
  unsigned count;
  unsigned bit14, bit15;

  if (i->getIaOpcode() == BX_IA_ROR_Ew)
    count = CL;
  else
    count = i->Ib();

  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());

  if ((count & 0x0f) == 0) {
    if (count & 0x10) {
      bit14 = (op1_16 >> 14) & 1;
      bit15 = (op1_16 >> 15) & 1;
      // of = result14 ^ result15
      SET_FLAGS_OxxxxC(bit14 ^ bit15, bit15);
    }
  }
  else {
    count &= 0x0f;  // use only 4 LSB's

    Bit16u result_16 = (op1_16 >> count) | (op1_16 << (16 - count));

    BX_WRITE_16BIT_REG(i->dst(), result_16);

    bit14 = (result_16 >> 14) & 1;
    bit15 = (result_16 >> 15) & 1;
    // of = result14 ^ result15
    SET_FLAGS_OxxxxC(bit14 ^ bit15, bit15);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_EwM(bxInstruction_c *i)
{
  Bit16u result_16;
  unsigned count;
  unsigned of, cf;

  if (i->getIaOpcode() == BX_IA_RCL_Ew)
    count = CL;
  else
    count = i->Ib();

  count = (count & 0x1f) % 17;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);

  if (count) {
    if (count==1) {
      result_16 = (op1_16 << 1) | getB_CF();
    }
    else if (count==16) {
      result_16 = (getB_CF() << 15) | (op1_16 >> 1);
    }
    else { // 2..15
      result_16 = (op1_16 << count) | (getB_CF() << (count - 1)) |
                  (op1_16 >> (17 - count));
    }

    write_RMW_virtual_word(result_16);

    cf = (op1_16 >> (16 - count)) & 0x1;
    of = cf ^ (result_16 >> 15); // of = cf ^ result15
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_EwR(bxInstruction_c *i)
{
  Bit16u result_16;
  unsigned count;
  unsigned of, cf;

  if (i->getIaOpcode() == BX_IA_RCL_Ew)
    count = CL;
  else
    count = i->Ib();

  count = (count & 0x1f) % 17;

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());

    if (count==1) {
      result_16 = (op1_16 << 1) | getB_CF();
    }
    else if (count==16) {
      result_16 = (getB_CF() << 15) | (op1_16 >> 1);
    }
    else { // 2..15
      result_16 = (op1_16 << count) | (getB_CF() << (count - 1)) |
                  (op1_16 >> (17 - count));
    }

    BX_WRITE_16BIT_REG(i->dst(), result_16);

    cf = (op1_16 >> (16 - count)) & 0x1;
    of = cf ^ (result_16 >> 15); // of = cf ^ result15
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_EwM(bxInstruction_c *i)
{
  unsigned count;
  unsigned of, cf;

  if (i->getIaOpcode() == BX_IA_RCR_Ew)
    count = CL;
  else
    count = i->Ib();

  count = (count & 0x1f) % 17;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);

  if (count) {
    Bit16u result_16 = (op1_16 >> count) | (getB_CF() << (16 - count)) |
                       (op1_16 << (17 - count));

    write_RMW_virtual_word(result_16);

    cf = (op1_16 >> (count - 1)) & 0x1;
    of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1; // of = result15 ^ result14
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_EwR(bxInstruction_c *i)
{
  unsigned count;
  unsigned of, cf;

  if (i->getIaOpcode() == BX_IA_RCR_Ew)
    count = CL;
  else
    count = i->Ib();

  count = (count & 0x1f) % 17;

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());

    Bit16u result_16 = (op1_16 >> count) | (getB_CF() << (16 - count)) |
                       (op1_16 << (17 - count));

    BX_WRITE_16BIT_REG(i->dst(), result_16);

    cf = (op1_16 >> (count - 1)) & 0x1;
    of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1; // of = result15 ^ result14
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_EwM(bxInstruction_c *i)
{
  Bit16u result_16;
  unsigned count;
  unsigned of = 0, cf = 0;

  if (i->getIaOpcode() == BX_IA_SHL_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; /* use only 5 LSB's */

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);

  if (count) {
    if (count <= 16) {
      result_16 = (op1_16 << count);
      cf = (op1_16 >> (16 - count)) & 0x1;
      of = cf ^ (result_16 >> 15); // of = cf ^ result15
    }
    else {
      result_16 = 0;
    }

    write_RMW_virtual_word(result_16);

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_EwR(bxInstruction_c *i)
{
  Bit16u result_16;
  unsigned count;
  unsigned of = 0, cf = 0;

  if (i->getIaOpcode() == BX_IA_SHL_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; /* use only 5 LSB's */

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());

    if (count <= 16) {
      result_16 = (op1_16 << count);
      cf = (op1_16 >> (16 - count)) & 0x1;
      of = cf ^ (result_16 >> 15); // of = cf ^ result15
    }
    else {
      result_16 = 0;
    }

    BX_WRITE_16BIT_REG(i->dst(), result_16);

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_EwM(bxInstruction_c *i)
{
  unsigned count;
  unsigned of, cf;

  if (i->getIaOpcode() == BX_IA_SHR_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; /* use only 5 LSB's */

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);

  if (count) {
    Bit16u result_16 = (op1_16 >> count);

    write_RMW_virtual_word(result_16);

    cf = (op1_16 >> (count - 1)) & 0x1;
    // note, that of == result15 if count == 1 and
    //            of == 0        if count >= 2
    of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1;

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_EwR(bxInstruction_c *i)
{
  unsigned count;
  unsigned of, cf;

  if (i->getIaOpcode() == BX_IA_SHR_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f; /* use only 5 LSB's */

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
    Bit16u result_16 = (op1_16 >> count);
    BX_WRITE_16BIT_REG(i->dst(), result_16);

    cf = (op1_16 >> (count - 1)) & 0x1;
    // note, that of == result15 if count == 1 and
    //            of == 0        if count >= 2
    of = ((Bit16u)((result_16 << 1) ^ result_16) >> 15) & 0x1;

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_EwM(bxInstruction_c *i)
{
  unsigned count, cf;

  if (i->getIaOpcode() == BX_IA_SAR_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;  /* use only 5 LSB's */

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);

  if (count) {
    Bit16u result_16 = ((Bit16s) op1_16) >> count;

    cf = (((Bit16s) op1_16) >> (count - 1)) & 0x1;

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);
    /* signed overflow cannot happen in SAR instruction */
    SET_FLAGS_OxxxxC(0, cf);

    write_RMW_virtual_word(result_16);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_EwR(bxInstruction_c *i)
{
  unsigned count, cf;

  if (i->getIaOpcode() == BX_IA_SAR_Ew)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;  /* use only 5 LSB's */

  if (count) {
    Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
    Bit16u result_16 = ((Bit16s) op1_16) >> count;
    BX_WRITE_16BIT_REG(i->dst(), result_16);

    cf = (((Bit16s) op1_16) >> (count - 1)) & 0x1;

    SET_FLAGS_OSZAPC_LOGIC_16(result_16);
    /* signed overflow cannot happen in SAR instruction */
    SET_FLAGS_OxxxxC(0, cf);
  }

  BX_NEXT_INSTR(i);
}
