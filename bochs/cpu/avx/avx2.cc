/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2023 Stanislav Shwartsman
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

#if BX_SUPPORT_AVX

#include "simd_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFHW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  Bit8u order = i->Ib();
  unsigned len = i->getVL();

  result.clear();

  for (unsigned n=0; n < len; n++)
    xmm_pshufhw(&result.vmm128(n), &op.vmm128(n), order);

  BX_WRITE_AVX_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFLW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  Bit8u order = i->Ib();
  unsigned len = i->getVL();

  result.clear();

  for (unsigned n=0; n < len; n++)
    xmm_pshuflw(&result.vmm128(n), &op.vmm128(n), order);

  BX_WRITE_AVX_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMPSADBW_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;

  result.clear();

  Bit8u control = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    xmm_mpsadbw(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n), control & 0x7);
    control >>= 3;
  }

  BX_WRITE_AVX_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBLENDW_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  for (unsigned n=0; n < len; n++)
    xmm_pblendw(&op1.vmm128(n), &op2.vmm128(n), mask);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTB_VdqWbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();
  op.clear();

  Bit8u val_8 = BX_READ_XMM_REG_LO_BYTE(i->src());

  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastb(&op.vmm128(n), val_8);

  BX_WRITE_AVX_REG(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTW_VdqWwR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();
  op.clear();

  Bit16u val_16 = BX_READ_XMM_REG_LO_WORD(i->src());

  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastw(&op.vmm128(n), val_16);

  BX_WRITE_AVX_REG(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_VdqWdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();
  op.clear();

  Bit32u val_32 = BX_READ_XMM_REG_LO_DWORD(i->src());

  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastd(&op.vmm128(n), val_32);

  BX_WRITE_AVX_REG(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_VdqWqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();
  op.clear();

  Bit64u val_64 = BX_READ_XMM_REG_LO_QWORD(i->src());

  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastq(&op.vmm128(n), val_64);

  BX_WRITE_AVX_REG(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++)
    result.vmm16s(n) = (Bit16s) op.ymmsbyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32s(n) = (Bit32s) op.xmmsbyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.xmmsbyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXWD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32s(n) = (Bit32s) op.ymm16s(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXWQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.xmm16s(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXDQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.ymm32s(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++)
    result.vmm16u(n) = (Bit16u) op.ymmubyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32u(n) = (Bit32u) op.xmmubyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.xmmubyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXWD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32u(n) = (Bit32u) op.ymm16u(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXWQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.xmm16u(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXDQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.ymm32u(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPALIGNR_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n<len; n++)
    xmm_palignr(&op2.vmm128(n), &op1.vmm128(n), i->Ib());

  BX_WRITE_AVX_REGZ(i->dst(), op2, i->getVL());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMD_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;

  for (unsigned n=0;n < 8;n++)
    result.ymm32u(n) = op2.ymm32u(op1.ymm32u(n) & 0x7);

  BX_WRITE_YMM_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMQ_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src()), result;

  ymm_vpermq(&result, &op, i->Ib());

  BX_WRITE_YMM_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

#endif
