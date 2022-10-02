/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2022 Stanislav Shwartsman
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

#if BX_SUPPORT_AVX || BX_SUPPORT_EVEX

#include "wide_int.h"

// 52-bit integer FMA

BX_CPP_INLINE Bit64u pmadd52luq_scalar(Bit64u dst, Bit64u op1, Bit64u op2)
{
  op1 &= BX_CONST64(0x000fffffffffffff);
  op2 &= BX_CONST64(0x000fffffffffffff);

  return dst + ((op1 * op2) & BX_CONST64(0x000fffffffffffff));
}

BX_CPP_INLINE Bit64u pmadd52huq_scalar(Bit64u dst, Bit64u op1, Bit64u op2)
{
  op1 &= BX_CONST64(0x000fffffffffffff);
  op2 &= BX_CONST64(0x000fffffffffffff);

  Bit128u product_128;
  long_mul(&product_128, op1, op2);

  Bit64u temp = (product_128.lo >> 52) | ((product_128.hi & BX_CONST64(0x000000ffffffffff)) << 12);

  return dst + temp;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADD52LUQ_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm64u(n) = pmadd52luq_scalar(dst.vmm64u(n), op1.vmm64u(n), op2.vmm64u(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADD52HUQ_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm64u(n) = pmadd52huq_scalar(dst.vmm64u(n), op1.vmm64u(n), op2.vmm64u(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

#endif

#if BX_SUPPORT_EVEX

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADD52LUQ_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  Bit32u mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  for (unsigned n=0, tmp_mask = mask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      dst.vmm64u(n) = pmadd52luq_scalar(dst.vmm64u(n), op1.vmm64u(n), op2.vmm64u(n));
    else if (i->isZeroMasking())
      dst.vmm64u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADD52HUQ_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  Bit32u mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  for (unsigned n=0, tmp_mask = mask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      dst.vmm64u(n) = pmadd52huq_scalar(dst.vmm64u(n), op1.vmm64u(n), op2.vmm64u(n));
    else if (i->isZeroMasking())
      dst.vmm64u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

#endif
