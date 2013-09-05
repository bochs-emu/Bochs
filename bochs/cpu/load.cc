/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2011 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eb(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP8L = read_virtual_byte(i->seg(), eaddr);
  return BX_CPU_CALL_METHOD(i->execute2(), (i));
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ew(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP16 = read_virtual_word(i->seg(), eaddr);
  return BX_CPU_CALL_METHOD(i->execute2(), (i));
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ed(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP32 = read_virtual_dword(i->seg(), eaddr);
  return BX_CPU_CALL_METHOD(i->execute2(), (i));
}

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  TMP64 = read_virtual_qword_64(i->seg(), eaddr);
  return BX_CPU_CALL_METHOD(i->execute2(), (i));
}
#endif

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit8u val_8 = read_virtual_byte(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_BYTE(BX_VECTOR_TMP_REGISTER, val_8);

  return BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ww(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit16u val_16 = read_virtual_word(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_WORD(BX_VECTOR_TMP_REGISTER, val_16);

  return BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wss(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_DWORD(BX_VECTOR_TMP_REGISTER, val_32);

  return BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wsd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_QWORD(BX_VECTOR_TMP_REGISTER, val_64);

  return BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  if (BX_CPU_THIS_PTR mxcsr.get_MM())
    read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_TMP_REGISTER));
  else
    read_virtual_xmmword_aligned(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));

  return BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOADU_Wdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));

  return BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

#if BX_SUPPORT_AVX

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Vector(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BX_ASSERT(i->getVL() != BX_VL512);

  if (i->getVL() == BX_VL256)
    read_virtual_ymmword(i->seg(), eaddr, &BX_READ_YMM_REG(BX_VECTOR_TMP_REGISTER));
  else
    read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));

  return BX_CPU_CALL_METHOD(i->execute2(), (i));
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Half_Vector(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BX_ASSERT(i->getVL() != BX_VL512);

  if (i->getVL() == BX_VL256) {
    read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));
  }
  else {
    Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
    BX_WRITE_XMM_REG_LO_QWORD(BX_VECTOR_TMP_REGISTER, val_64);
  }

  return BX_CPU_CALL_METHOD(i->execute2(), (i));
}

#endif
