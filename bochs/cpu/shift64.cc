/////////////////////////////////////////////////////////////////////////
// $Id: shift64.cc,v 1.41 2009-12-04 16:53:12 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, result_64;
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xa4) // 0x1a4
    count = i->Ib();
  else // 0x1a5
    count = CL;

  count &= 0x3f; // use only 6 LSB's
  if (!count) return;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  result_64 = (op1_64 << count) | (op2_64 >> (64 - count));

  write_RMW_virtual_qword(result_64);

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);

  cf = (op1_64 >> (64 - count)) & 0x1;
  of = cf ^ (result_64 >> 63); // of = cf ^ result63
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLD_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, result_64;
  unsigned count;
  unsigned cf, of;

  if (i->b1() == 0xa4) // 0x1a4
    count = i->Ib();
  else // 0x1a5
    count = CL;

  count &= 0x3f; // use only 6 LSB's
  if (!count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());

  result_64 = (op1_64 << count) | (op2_64 >> (64 - count));

  BX_WRITE_64BIT_REG(i->rm(), result_64);

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);

  cf = (op1_64 >> (64 - count)) & 0x1;
  of = cf ^ (result_64 >> 63); // of = cf ^ result63
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, result_64;
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xac) // 0x1ac
    count = i->Ib();
  else // 0x1ad
    count = CL;

  count &= 0x3f; // use only 6 LSB's
  if (!count) return;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  result_64 = (op2_64 << (64 - count)) | (op1_64 >> count);

  write_RMW_virtual_qword(result_64);

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);

  cf = (op1_64 >> (count - 1)) & 0x1;
  of = ((result_64 << 1) ^ result_64) >> 63; // of = result62 ^ result63
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRD_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, result_64;
  unsigned count;
  unsigned cf, of;

  if (i->b1() == 0xac) // 0x1ac
    count = i->Ib();
  else // 0x1ad
    count = CL;

  count &= 0x3f; // use only 6 LSB's
  if (!count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());

  result_64 = (op2_64 << (64 - count)) | (op1_64 >> count);

  BX_WRITE_64BIT_REG(i->rm(), result_64);

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);

  cf = (op1_64 >> (count - 1)) & 0x1;
  of = ((result_64 << 1) ^ result_64) >> 63; // of = result62 ^ result63
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_EqM(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned bit0, bit63;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (! count) return;

  result_64 = (op1_64 << count) | (op1_64 >> (64 - count));

  write_RMW_virtual_qword(result_64);

  bit0  = (result_64 & 0x1);
  bit63 = (result_64 >> 63);
  // of = cf ^ result63
  SET_FLAGS_OxxxxC(bit0 ^ bit63, bit0);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROL_EqR(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned bit0, bit63;

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (! count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  result_64 = (op1_64 << count) | (op1_64 >> (64 - count));
  BX_WRITE_64BIT_REG(i->rm(), result_64);

  bit0  = (result_64 & 0x1);
  bit63 = (result_64 >> 63);
  // of = cf ^ result63
  SET_FLAGS_OxxxxC(bit0 ^ bit63, bit0);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_EqM(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned bit62, bit63;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (! count) return;

  result_64 = (op1_64 >> count) | (op1_64 << (64 - count));

  write_RMW_virtual_qword(result_64);

  bit63 = (result_64 >> 63) & 1;
  bit62 = (result_64 >> 62) & 1;
  // of = result62 ^ result63
  SET_FLAGS_OxxxxC(bit62 ^ bit63, bit63);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROR_EqR(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned bit62, bit63;

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (! count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  result_64 = (op1_64 >> count) | (op1_64 << (64 - count));
  BX_WRITE_64BIT_REG(i->rm(), result_64);

  bit63 = (result_64 >> 63) & 1;
  bit62 = (result_64 >> 62) & 1;
  // of = result62 ^ result63
  SET_FLAGS_OxxxxC(bit62 ^ bit63, bit63);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_EqM(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  if (count==1) {
    result_64 = (op1_64 << 1) | getB_CF();
  }
  else {
    result_64 = (op1_64 << count) | (getB_CF() << (count - 1)) |
                (op1_64 >> (65 - count));
  }

  write_RMW_virtual_qword(result_64);

  cf = (op1_64 >> (64 - count)) & 0x1;
  of = cf ^ (result_64 >> 63); // of = cf ^ result63
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCL_EqR(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned cf, of;

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());

  if (count==1) {
    result_64 = (op1_64 << 1) | getB_CF();
  }
  else {
    result_64 = (op1_64 << count) | (getB_CF() << (count - 1)) |
                (op1_64 >> (65 - count));
  }

  BX_WRITE_64BIT_REG(i->rm(), result_64);

  cf = (op1_64 >> (64 - count)) & 0x1;
  of = cf ^ (result_64 >> 63); // of = cf ^ result63
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_EqM(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned of, cf;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  if (count==1) {
    result_64 = (op1_64 >> 1) | (((Bit64u) getB_CF()) << 63);
  }
  else {
    result_64 = (op1_64 >> count) | (getB_CF() << (64 - count)) |
                (op1_64 << (65 - count));
  }

  write_RMW_virtual_qword(result_64);

  cf = (op1_64 >> (count - 1)) & 0x1;
  of = ((result_64 << 1) ^ result_64) >> 63;
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RCR_EqR(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned of, cf;

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());

  if (count==1) {
    result_64 = (op1_64 >> 1) | (((Bit64u) getB_CF()) << 63);
  }
  else {
    result_64 = (op1_64 >> count) | (getB_CF() << (64 - count)) |
                (op1_64 << (65 - count));
  }

  BX_WRITE_64BIT_REG(i->rm(), result_64);

  cf = (op1_64 >> (count - 1)) & 0x1;
  of = ((result_64 << 1) ^ result_64) >> 63;
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_EqM(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  /* count < 64, since only lower 6 bits used */
  result_64 = (op1_64 << count);
  cf = (op1_64 >> (64 - count)) & 0x1;
  of = cf ^ (result_64 >> 63);

  write_RMW_virtual_qword(result_64);

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHL_EqR(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned cf, of;

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  /* count < 64, since only lower 6 bits used */
  result_64 = (op1_64 << count);
  BX_WRITE_64BIT_REG(i->rm(), result_64);

  cf = (op1_64 >> (64 - count)) & 0x1;
  of = cf ^ (result_64 >> 63);
  SET_FLAGS_OSZAPC_LOGIC_64(result_64);
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_EqM(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned cf, of;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  result_64 = (op1_64 >> count);

  write_RMW_virtual_qword(result_64);

  cf = (op1_64 >> (count - 1)) & 0x1;
  // note, that of == result63 if count == 1 and
  //            of == 0        if count >= 2
  of = ((result_64 << 1) ^ result_64) >> 63;

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHR_EqR(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;
  unsigned cf, of;

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  result_64 = (op1_64 >> count);
  BX_WRITE_64BIT_REG(i->rm(), result_64);

  cf = (op1_64 >> (count - 1)) & 0x1;
  // note, that of == result63 if count == 1 and
  //            of == 0        if count >= 2
  of = ((result_64 << 1) ^ result_64) >> 63;

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_EqM(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  /* count < 64, since only lower 6 bits used */
  if (op1_64 & BX_CONST64(0x8000000000000000)) {
    result_64 = (op1_64 >> count) | (BX_CONST64(0xffffffffffffffff) << (64 - count));
  }
  else {
    result_64 = (op1_64 >> count);
  }

  write_RMW_virtual_qword(result_64);

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);
  set_CF((op1_64 >> (count - 1)) & 1);
  clear_OF();  /* signed overflow cannot happen in SAR instruction */
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SAR_EqR(bxInstruction_c *i)
{
  Bit64u op1_64, result_64;
  unsigned count;

  if (i->b1() == 0xd3)
    count = CL;
  else // 0xc1 or 0xd1
    count = i->Ib();

  count &= 0x3f;
  if (!count) return;

  op1_64 = BX_READ_64BIT_REG(i->rm());

  /* count < 64, since only lower 6 bits used */
  if (op1_64 & BX_CONST64(0x8000000000000000)) {
    result_64 = (op1_64 >> count) | (BX_CONST64(0xffffffffffffffff) << (64 - count));
  }
  else {
    result_64 = (op1_64 >> count);
  }

  BX_WRITE_64BIT_REG(i->rm(), result_64);

  SET_FLAGS_OSZAPC_LOGIC_64(result_64);
  set_CF((op1_64 >> (count - 1)) & 1);
  clear_OF();  /* signed overflow cannot happen in SAR instruction */
}

#endif /* if BX_SUPPORT_X86_64 */
