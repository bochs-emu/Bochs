/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2019 Stanislav Shwartsman
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eb(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  TMP8L = read_virtual_byte(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ew(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  TMP16 = read_virtual_word(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ed(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  TMP32 = read_virtual_dword(i->seg(), eaddr);
  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
  TMP64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  BX_CPU_CALL_METHOD(i->execute2(), (i));
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit8u val_8 = read_virtual_byte(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_BYTE(BX_VECTOR_TMP_REGISTER, val_8);

  BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Ww(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit16u val_16 = read_virtual_word(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_WORD(BX_VECTOR_TMP_REGISTER, val_16);

  BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wss(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_DWORD(BX_VECTOR_TMP_REGISTER, val_32);

  BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

#if BX_SUPPORT_EVEX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_MASK_Wss(bxInstruction_c *i)
{
  Bit32u val_32 = 0;

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    val_32 = read_virtual_dword(i->seg(), eaddr);
  }

  BX_WRITE_XMM_REG_LO_DWORD(BX_VECTOR_TMP_REGISTER, val_32);
  BX_CPU_CALL_METHOD(i->execute2(), (i));
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wsd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
  BX_WRITE_XMM_REG_LO_QWORD(BX_VECTOR_TMP_REGISTER, val_64);

  BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

#if BX_SUPPORT_EVEX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_MASK_Wsd(bxInstruction_c *i)
{
  Bit64u val_64 = 0;

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    val_64 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_WRITE_XMM_REG_LO_QWORD(BX_VECTOR_TMP_REGISTER, val_64);
  BX_CPU_CALL_METHOD(i->execute2(), (i));
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Wdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

  if (BX_CPU_THIS_PTR mxcsr.get_MM())
    read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));
  else
    read_virtual_xmmword_aligned(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));

  BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOADU_Wdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));

  BX_CPU_CALL_METHOD(i->execute2(), (i));
#endif
}

#if BX_SUPPORT_AVX

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Vector(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

#if BX_SUPPORT_EVEX
  if (len == BX_VL512) {
    read_virtual_zmmword(i->seg(), eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER));
  }
  else
#endif
  {
    if (len == BX_VL256)
      read_virtual_ymmword(i->seg(), eaddr, &BX_READ_YMM_REG(BX_VECTOR_TMP_REGISTER));
    else
      read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Half_Vector(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

#if BX_SUPPORT_EVEX
  if (len == BX_VL512) {
    read_virtual_ymmword(i->seg(), eaddr, &BX_READ_YMM_REG(BX_VECTOR_TMP_REGISTER));
  }
  else
#endif
  {
    if (len == BX_VL256) {
      read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));
    }
    else {
      Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
      BX_WRITE_XMM_REG_LO_QWORD(BX_VECTOR_TMP_REGISTER, val_64);
    }
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Quarter_Vector(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

#if BX_SUPPORT_EVEX
  if (len == BX_VL512) {
    read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));
  }
  else
#endif
  {
    if (len == BX_VL256) {
      Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
      BX_WRITE_XMM_REG_LO_QWORD(BX_VECTOR_TMP_REGISTER, val_64);
    }
    else {
      Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
      BX_WRITE_XMM_REG_LO_DWORD(BX_VECTOR_TMP_REGISTER, val_32);
    }
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_Eighth_Vector(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

#if BX_SUPPORT_EVEX
  if (len == BX_VL512) {
    Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
    BX_WRITE_XMM_REG_LO_QWORD(BX_VECTOR_TMP_REGISTER, val_64);
  }
  else
#endif
  {
    if (len == BX_VL256) {
      Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
      BX_WRITE_XMM_REG_LO_DWORD(BX_VECTOR_TMP_REGISTER, val_32);
    }
    else {
      Bit16u val_16 = read_virtual_word(i->seg(), eaddr);
      BX_WRITE_XMM_REG_LO_WORD(BX_VECTOR_TMP_REGISTER, val_16);
    }
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

#endif

#if BX_SUPPORT_EVEX

#include "simd_int.h"

// load vector of bytes, support masked fault suppression, no broadcast
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_MASK_VectorB(bxInstruction_c *i)
{
  Bit64u opmask = (i->opmask() != 0) ? BX_READ_OPMASK(i->opmask()) : BX_CONST64(0xffffffffffffffff);

  if (opmask == 0) {
    BX_CPU_CALL_METHOD(i->execute2(), (i)); // for now let execute method to deal with zero/merge masking semantics
    return;
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_load8(i, eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER), opmask);

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load vector of words, support masked fault suppression, no broadcast
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_MASK_VectorW(bxInstruction_c *i)
{
  Bit32u opmask = (i->opmask() != 0) ? BX_READ_32BIT_OPMASK(i->opmask()) : 0xffffffff;

  if (opmask == 0) {
    BX_CPU_CALL_METHOD(i->execute2(), (i)); // for now let execute method to deal with zero/merge masking semantics
    return;
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_load16(i, eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER), opmask);

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load vector of dwords, support masked fault suppression, no broadcast
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_MASK_VectorD(bxInstruction_c *i)
{
  Bit32u opmask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff;

  if (opmask == 0) {
    BX_CPU_CALL_METHOD(i->execute2(), (i)); // for now let execute method to deal with zero/merge masking semantics
    return;
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_load32(i, eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER), opmask);

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load vector of qwords, support masked fault suppression, no broadcast
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_MASK_VectorQ(bxInstruction_c *i)
{
  Bit32u opmask = (i->opmask() != 0) ? BX_READ_8BIT_OPMASK(i->opmask()) : 0xff;

  if (opmask == 0) {
    BX_CPU_CALL_METHOD(i->execute2(), (i)); // for now let execute method to deal with zero/merge masking semantics
    return;
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_load64(i, eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER), opmask);

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load vector of dwords, support broadcast, no fault suppression
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_BROADCAST_VectorD(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

  if (i->getEvexb()) {
    Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
    simd_pbroadcastd(&BX_AVX_REG(BX_VECTOR_TMP_REGISTER), val_32, len * 4);
  }
  else {
    if (len == BX_VL512)
      read_virtual_zmmword(i->seg(), eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER));
    if (len == BX_VL256)
      read_virtual_ymmword(i->seg(), eaddr, &BX_READ_YMM_REG(BX_VECTOR_TMP_REGISTER));
    else
      read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load vector of dwords, support broadcast and masked fault suppression
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_BROADCAST_MASK_VectorD(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  Bit32u opmask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff;
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  if (opmask == 0) {
    BX_CPU_CALL_METHOD(i->execute2(), (i)); // for now let execute method to deal with zero/merge masking semantics
    return;
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

  if (i->getEvexb()) {
    Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
    simd_pbroadcastd(&BX_AVX_REG(BX_VECTOR_TMP_REGISTER), val_32, len * 4);
  }
  else {
    avx_masked_load32(i, eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER), opmask);
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load vector of qwords, support broadcast, no fault suppression
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_BROADCAST_VectorQ(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

  if (i->getEvexb()) {
    Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
    simd_pbroadcastq(&BX_AVX_REG(BX_VECTOR_TMP_REGISTER), val_64, len * 2);
  }
  else {
    if (len == BX_VL512)
      read_virtual_zmmword(i->seg(), eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER));
    if (len == BX_VL256)
      read_virtual_ymmword(i->seg(), eaddr, &BX_READ_YMM_REG(BX_VECTOR_TMP_REGISTER));
    else
      read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load vector of qwords, support broadcast and masked fault suppression
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_BROADCAST_MASK_VectorQ(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  Bit32u opmask = (i->opmask() != 0) ? BX_READ_8BIT_OPMASK(i->opmask()) : 0xff;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  if (opmask == 0) {
    BX_CPU_CALL_METHOD(i->execute2(), (i)); // for now let execute method to deal with zero/merge masking semantics
    return;
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

  if (i->getEvexb()) {
    Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
    simd_pbroadcastq(&BX_AVX_REG(BX_VECTOR_TMP_REGISTER), val_64, len * 2);
  }
  else {
    avx_masked_load64(i, eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER), opmask);
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load half vector of dwords, support broadcast, no fault suppression
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_BROADCAST_Half_VectorD(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

  if (i->getEvexb()) {
    Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
    simd_pbroadcastd(&BX_AVX_REG(BX_VECTOR_TMP_REGISTER), val_32, len * 2);
  }
  else {
    if (len == BX_VL512) {
      read_virtual_ymmword(i->seg(), eaddr, &BX_READ_YMM_REG(BX_VECTOR_TMP_REGISTER));
    }
    if (len == BX_VL256) {
      read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(BX_VECTOR_TMP_REGISTER));
    }
    else {
      Bit64u val_64 = read_virtual_qword(i->seg(), eaddr);
      BX_WRITE_XMM_REG_LO_QWORD(BX_VECTOR_TMP_REGISTER, val_64);
    }
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

// load half vector of dwords, support broadcast and masked fault suppression
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOAD_BROADCAST_MASK_Half_VectorD(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  Bit32u opmask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff;
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len)-1);

  if (opmask == 0) {
    BX_CPU_CALL_METHOD(i->execute2(), (i)); // for now let execute method to deal with zero/merge masking semantics
    return;
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

  if (i->getEvexb()) {
    Bit32u val_32 = read_virtual_dword(i->seg(), eaddr);
    simd_pbroadcastd(&BX_AVX_REG(BX_VECTOR_TMP_REGISTER), val_32, len * 2);
  }
  else {
    avx_masked_load32(i, eaddr, &BX_READ_AVX_REG(BX_VECTOR_TMP_REGISTER), opmask);
  }

  BX_CPU_CALL_METHOD(i->execute2(), (i));
}

#endif
