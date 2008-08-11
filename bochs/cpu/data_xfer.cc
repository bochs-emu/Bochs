/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer.cc,v 1.4 2008-08-11 20:34:05 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eb(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP8L = read_virtual_byte(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ew(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP16 = read_virtual_word(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ed(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP32 = read_virtual_dword(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP64 = read_virtual_qword_64(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wdq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *)(&BX_READ_XMM_REG(BX_TMP_REGISTER)));
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wss(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_DWORD(BX_TMP_REGISTER, val_32);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wsd(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_QWORD(BX_TMP_REGISTER, val_64);
  BX_CPU_CALL_METHOD(i->execute2, (i));
}
