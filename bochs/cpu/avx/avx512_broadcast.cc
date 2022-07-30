/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2019 Stanislav Shwartsman
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

#if BX_SUPPORT_EVEX

#include "simd_int.h"

// broadcast from vector register or memory

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTB_MASK_VdqWbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastb(&op, BX_READ_XMM_REG_LO_BYTE(i->src()), BYTE_ELEMENTS(len));
  avx512_write_regb_masked(i, &op, len, BX_READ_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTB_MASK_VdqWbM(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit64u opmask = BX_READ_OPMASK(i->opmask()) & CUT_OPMASK_TO(BYTE_ELEMENTS(len));
  Bit8u val_8 = 0;

  if (opmask) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    val_8 = read_virtual_byte(i->seg(), eaddr);
  }

  simd_pbroadcastb(&op, val_8, BYTE_ELEMENTS(len));
  avx512_write_regb_masked(i, &op, len, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTW_MASK_VdqWwR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastw(&op, BX_READ_XMM_REG_LO_WORD(i->src()), WORD_ELEMENTS(len));
  avx512_write_regw_masked(i, &op, len, BX_READ_32BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTW_MASK_VdqWwM(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit32u opmask = BX_READ_32BIT_OPMASK(i->opmask()) & CUT_OPMASK_TO(WORD_ELEMENTS(len));
  Bit16u val_16 = 0;

  if (opmask) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    val_16 = read_virtual_word(i->seg(), eaddr);
  }

  simd_pbroadcastw(&op, val_16, WORD_ELEMENTS(len));
  avx512_write_regw_masked(i, &op, len, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_MASK_VdqWdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastd(&op, BX_READ_XMM_REG_LO_DWORD(i->src()), DWORD_ELEMENTS(len));
  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_MASK_VdqWdM(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask()) & CUT_OPMASK_TO(DWORD_ELEMENTS(len));
  Bit32u val_32 = 0;

  if (opmask) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    val_32 = read_virtual_dword(i->seg(), eaddr);
  }

  simd_pbroadcastd(&op, val_32, DWORD_ELEMENTS(len));
  avx512_write_regd_masked(i, &op, len, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_MASK_VdqWqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastq(&op, BX_READ_XMM_REG_LO_QWORD(i->src()), QWORD_ELEMENTS(len));
  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_MASK_VdqWqM(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask()) & CUT_OPMASK_TO(QWORD_ELEMENTS(len));
  Bit64u val_64 = 0;

  if (opmask) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    val_64 = read_virtual_qword(i->seg(), eaddr);
  }

  simd_pbroadcastq(&op, val_64, QWORD_ELEMENTS(len));
  avx512_write_regq_masked(i, &op, len, opmask);

  BX_NEXT_INSTR(i);
}

// broadcast from register

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTB_VdqEbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastb(&op, BX_READ_8BIT_REGL(i->src()), BYTE_ELEMENTS(len));
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTB_MASK_VdqEbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastb(&op, BX_READ_8BIT_REGL(i->src()), BYTE_ELEMENTS(len));
  avx512_write_regb_masked(i, &op, len, BX_READ_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTW_VdqEwR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastw(&op, BX_READ_16BIT_REG(i->src()), WORD_ELEMENTS(len));
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTW_MASK_VdqEwR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastw(&op, BX_READ_16BIT_REG(i->src()), WORD_ELEMENTS(len));
  avx512_write_regw_masked(i, &op, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_VdqEdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastd(&op, BX_READ_32BIT_REG(i->src()), DWORD_ELEMENTS(len));
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_MASK_VdqEdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastd(&op, BX_READ_32BIT_REG(i->src()), DWORD_ELEMENTS(len));
  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_VdqEqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastq(&op, BX_READ_64BIT_REG(i->src()), QWORD_ELEMENTS(len));
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_MASK_VdqEqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastq(&op, BX_READ_64BIT_REG(i->src()), QWORD_ELEMENTS(len));
  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

// broadcast dword tupple from vector register or memory
// writing to destination according to dword mask

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF32x2_MASK_VpsWqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastq(&op, BX_READ_XMM_REG_LO_QWORD(i->src()), QWORD_ELEMENTS(len));
  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF32x2_MASK_VpsWqM(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask()) & CUT_OPMASK_TO(DWORD_ELEMENTS(len));
  Bit64u val_64 = 0;

  if (opmask) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    val_64 = read_virtual_qword(i->seg(), eaddr);
  }

  simd_pbroadcastq(&op, val_64, QWORD_ELEMENTS(len));
  avx512_write_regd_masked(i, &op, len, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF32x4_MASK_VpsMps(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  BxPackedXmmRegister src;
  unsigned len = i->getVL();

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask()) & CUT_OPMASK_TO(DWORD_ELEMENTS(len));
  if (opmask != 0) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    read_virtual_xmmword(i->seg(), eaddr, &src);

    for (unsigned n=0; n < len; n++) {
      dst.vmm128(n) = src;
    }

    avx512_write_regd_masked(i, &dst, len, opmask);
  }
  else {
    if (i->isZeroMasking()) {
      BX_CLEAR_AVX_REG(i->dst());
    }
    else {
      BX_CLEAR_AVX_REGZ(i->dst(), len);
    }
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF64x2_MASK_VpdMpd(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  BxPackedXmmRegister src;
  unsigned len = i->getVL();

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask()) & CUT_OPMASK_TO(QWORD_ELEMENTS(len));
  if (opmask != 0) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    read_virtual_xmmword(i->seg(), eaddr, &src);

    for (unsigned n=0; n < len; n++) {
      dst.vmm128(n) = src;
    }

    avx512_write_regq_masked(i, &dst, len, opmask);
  }
  else {
    if (i->isZeroMasking()) {
      BX_CLEAR_AVX_REG(i->dst());
    }
    else {
      BX_CLEAR_AVX_REGZ(i->dst(), len);
    }
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF64x4_VpdMpd(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  BxPackedYmmRegister src;

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  read_virtual_ymmword(i->seg(), eaddr, &src);

  dst.vmm256(0) = src;
  dst.vmm256(1) = src;

  BX_WRITE_AVX_REG(i->dst(), dst);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF64x4_MASK_VpdMpd(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  BxPackedYmmRegister src;

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask());
  if (opmask != 0) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    read_virtual_ymmword(i->seg(), eaddr, &src);

    dst.vmm256(0) = src;
    dst.vmm256(1) = src;

    avx512_write_regq_masked(i, &dst, BX_VL512, opmask);
  }
  else {
    if (i->isZeroMasking())
      BX_CLEAR_AVX_REG(i->dst());
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF32x8_MASK_VpsMps(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  BxPackedYmmRegister src;

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask());
  if (opmask != 0) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    read_virtual_ymmword(i->seg(), eaddr, &src);

    dst.vmm256(0) = src;
    dst.vmm256(1) = src;

    avx512_write_regd_masked(i, &dst, BX_VL512, opmask);
  }
  else {
    if (i->isZeroMasking())
      BX_CLEAR_AVX_REG(i->dst());
  }

  BX_NEXT_INSTR(i);
}

#endif
