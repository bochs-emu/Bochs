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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_EdGdM(bxInstruction_c *i)
{
  unsigned count;
  unsigned of, cf;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_SHLD_EdGd)
    count = CL;
  else // BX_IA_SHLD_EdGdIb
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  if (count) {
    Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

    Bit32u result_32 = (op1_32 << count) | (op2_32 >> (32 - count));

    write_RMW_virtual_dword(result_32);

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);

    cf = (op1_32 >> (32 - count)) & 0x1;
    of = cf ^ (result_32 >> 31); // of = cf ^ result31
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, result_32;
  unsigned count;
  unsigned of, cf;

  if (i->getIaOpcode() == BX_IA_SHLD_EdGd)
    count = CL;
  else // BX_IA_SHLD_EdGdIb
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
  }
  else {
    op1_32 = BX_READ_32BIT_REG(i->dst());
    op2_32 = BX_READ_32BIT_REG(i->src());

    result_32 = (op1_32 << count) | (op2_32 >> (32 - count));

    BX_WRITE_32BIT_REGZ(i->dst(), result_32);

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);

    cf = (op1_32 >> (32 - count)) & 0x1;
    of = cf ^ (result_32 >> 31); // of = cf ^ result31
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_EdGdM(bxInstruction_c *i)
{
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_SHRD_EdGd)
    count = CL;
  else // BX_IA_SHRD_EdGdIb
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  if (count) {
    Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

    Bit32u result_32 = (op2_32 << (32 - count)) | (op1_32 >> count);

    write_RMW_virtual_dword(result_32);

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);

    cf = (op1_32 >> (count - 1)) & 0x1;
    of = ((result_32 << 1) ^ result_32) >> 31; // of = result30 ^ result31
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, result_32;
  unsigned count;
  unsigned cf, of;

  if (i->getIaOpcode() == BX_IA_SHRD_EdGd)
    count = CL;
  else // BX_IA_SHRD_EdGdIb
    count = i->Ib();

  count &= 0x1f; // use only 5 LSB's

  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
  }
  else {
    op1_32 = BX_READ_32BIT_REG(i->dst());
    op2_32 = BX_READ_32BIT_REG(i->src());

    result_32 = (op2_32 << (32 - count)) | (op1_32 >> count);

    BX_WRITE_32BIT_REGZ(i->dst(), result_32);

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);

    cf = (op1_32 >> (count - 1)) & 0x1;
    of = ((result_32 << 1) ^ result_32) >> 31; // of = result30 ^ result31
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_EdM(bxInstruction_c *i)
{
  unsigned count;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_ROL_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit32u result_32 = (op1_32 << count) | (op1_32 >> (32 - count));

    write_RMW_virtual_dword(result_32);

    unsigned bit0  = (result_32 & 0x1);
    unsigned bit31 = (result_32 >> 31);
    // of = cf ^ result31
    SET_FLAGS_OxxxxC(bit0 ^ bit31, bit0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_EdR(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;
  unsigned bit0, bit31;

  if (i->getIaOpcode() == BX_IA_ROL_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
  }
  else {
    op1_32 = BX_READ_32BIT_REG(i->dst());
    result_32 = (op1_32 << count) | (op1_32 >> (32 - count));
    BX_WRITE_32BIT_REGZ(i->dst(), result_32);

    bit0  = (result_32 & 0x1);
    bit31 = (result_32 >> 31);
    // of = cf ^ result31
    SET_FLAGS_OxxxxC(bit0 ^ bit31, bit0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_EdM(bxInstruction_c *i)
{
  unsigned count;
  unsigned bit31, bit30;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_ROR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit32u result_32 = (op1_32 >> count) | (op1_32 << (32 - count));

    write_RMW_virtual_dword(result_32);

    bit31 = (result_32 >> 31) & 1;
    bit30 = (result_32 >> 30) & 1;
    // of = result30 ^ result31
    SET_FLAGS_OxxxxC(bit30 ^ bit31, bit31);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_EdR(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;
  unsigned bit31, bit30;

  if (i->getIaOpcode() == BX_IA_ROR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
  }
  else {
    op1_32 = BX_READ_32BIT_REG(i->dst());
    result_32 = (op1_32 >> count) | (op1_32 << (32 - count));
    BX_WRITE_32BIT_REGZ(i->dst(), result_32);

    bit31 = (result_32 >> 31) & 1;
    bit30 = (result_32 >> 30) & 1;
    // of = result30 ^ result31
    SET_FLAGS_OxxxxC(bit30 ^ bit31, bit31);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_EdM(bxInstruction_c *i)
{
  Bit32u result_32;
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_RCL_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;
  if (!count) {
    BX_NEXT_INSTR(i);
  }

  if (count==1) {
    result_32 = (op1_32 << 1) | getB_CF();
  }
  else {
    result_32 = (op1_32 << count) | (getB_CF() << (count - 1)) |
                (op1_32 >> (33 - count));
  }

  write_RMW_virtual_dword(result_32);

  cf = (op1_32 >> (32 - count)) & 0x1;
  of = cf ^ (result_32 >> 31); // of = cf ^ result31
  SET_FLAGS_OxxxxC(of, cf);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_EdR(bxInstruction_c *i)
{
  Bit32u result_32;
  unsigned count;
  unsigned cf, of;

  if (i->getIaOpcode() == BX_IA_RCL_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;
  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
    BX_NEXT_INSTR(i);
  }

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());

  if (count==1) {
    result_32 = (op1_32 << 1) | getB_CF();
  }
  else {
    result_32 = (op1_32 << count) | (getB_CF() << (count - 1)) |
                (op1_32 >> (33 - count));
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  cf = (op1_32 >> (32 - count)) & 0x1;
  of = cf ^ (result_32 >> 31); // of = cf ^ result31
  SET_FLAGS_OxxxxC(of, cf);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_EdM(bxInstruction_c *i)
{
  Bit32u result_32;
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_RCR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (!count) {
    BX_NEXT_INSTR(i);
  }

  if (count==1) {
    result_32 = (op1_32 >> 1) | (getB_CF() << 31);
  }
  else {
    result_32 = (op1_32 >> count) | (getB_CF() << (32 - count)) |
                (op1_32 << (33 - count));
  }

  write_RMW_virtual_dword(result_32);

  cf = (op1_32 >> (count - 1)) & 0x1;
  of = ((result_32 << 1) ^ result_32) >> 31; // of = result30 ^ result31
  SET_FLAGS_OxxxxC(of, cf);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_EdR(bxInstruction_c *i)
{
  Bit32u result_32;
  unsigned count;
  unsigned cf, of;

  if (i->getIaOpcode() == BX_IA_RCR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
    BX_NEXT_INSTR(i);
  }

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());

  if (count==1) {
    result_32 = (op1_32 >> 1) | (getB_CF() << 31);
  }
  else {
    result_32 = (op1_32 >> count) | (getB_CF() << (32 - count)) |
                (op1_32 << (33 - count));
  }

  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  cf = (op1_32 >> (count - 1)) & 0x1;
  of = ((result_32 << 1) ^ result_32) >> 31; // of = result30 ^ result31
  SET_FLAGS_OxxxxC(of, cf);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_EdM(bxInstruction_c *i)
{
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_SHL_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    /* count < 32, since only lower 5 bits used */
    Bit32u result_32 = (op1_32 << count);

    write_RMW_virtual_dword(result_32);

    cf = (op1_32 >> (32 - count)) & 0x1;
    of = cf ^ (result_32 >> 31);
    SET_FLAGS_OSZAPC_LOGIC_32(result_32);
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_EdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_SHL_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
  }
  else {
    Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());

    /* count < 32, since only lower 5 bits used */
    Bit32u result_32 = (op1_32 << count);

    BX_WRITE_32BIT_REGZ(i->dst(), result_32);

    unsigned cf = (op1_32 >> (32 - count)) & 0x1;
    unsigned of = cf ^ (result_32 >> 31);

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_EdM(bxInstruction_c *i)
{
  unsigned count;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_SHR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    Bit32u result_32 = (op1_32 >> count);

    write_RMW_virtual_dword(result_32);

    unsigned cf = (op1_32 >> (count - 1)) & 0x1;
    // note, that of == result31 if count == 1 and
    //            of == 0        if count >= 2
    unsigned of = ((result_32 << 1) ^ result_32) >> 31;

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_EdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_SHR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
  }
  else {
    Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
    Bit32u result_32 = (op1_32 >> count);
    BX_WRITE_32BIT_REGZ(i->dst(), result_32);

    unsigned cf = (op1_32 >> (count - 1)) & 0x1;
    // note, that of == result31 if count == 1 and
    //            of == 0        if count >= 2
    unsigned of = ((result_32 << 1) ^ result_32) >> 31;

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);
    SET_FLAGS_OxxxxC(of, cf);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_EdM(bxInstruction_c *i)
{
  unsigned count;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);

  if (i->getIaOpcode() == BX_IA_SAR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (count) {
    /* count < 32, since only lower 5 bits used */
    Bit32u result_32 = ((Bit32s) op1_32) >> count;

    write_RMW_virtual_dword(result_32);

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);
    unsigned cf = (op1_32 >> (count - 1)) & 1;
    SET_FLAGS_OxxxxC(0, cf); /* signed overflow cannot happen in SAR instruction */
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_EdR(bxInstruction_c *i)
{
  unsigned count;

  if (i->getIaOpcode() == BX_IA_SAR_Ed)
    count = CL;
  else
    count = i->Ib();

  count &= 0x1f;

  if (!count) {
    BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register
  }
  else {
    Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());

    /* count < 32, since only lower 5 bits used */
    Bit32u result_32 = ((Bit32s) op1_32) >> count;

    BX_WRITE_32BIT_REGZ(i->dst(), result_32);

    SET_FLAGS_OSZAPC_LOGIC_32(result_32);
    unsigned cf = (op1_32 >> (count - 1)) & 1;
    SET_FLAGS_OxxxxC(0, cf); /* signed overflow cannot happen in SAR instruction */
  }

  BX_NEXT_INSTR(i);
}
