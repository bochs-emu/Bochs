/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2012 Stanislav Shwartsman
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

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::BxResolveGatherD(bxInstruction_c *i, unsigned element)
{
  Bit32s index = BX_READ_YMM_REG(i->sibIndex()).ymm32s(element);
  
  if (i->as64L())
    return (BX_READ_64BIT_REG(i->sibBase()) + (((Bit64s) index) << i->sibScale()) + i->displ32s());
  else
    return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
}

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::BxResolveGatherQ(bxInstruction_c *i, unsigned element)
{
  Bit64s index = BX_READ_YMM_REG(i->sibIndex()).ymm64s(element);
  
  if (i->as64L())
    return (BX_READ_64BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
  else
    return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERDPS_VpsHps(bxInstruction_c *i)
{
  if (i->sibIndex() == i->src2() || i->sibIndex() == i->dst() || i->src2() == i->dst()) {
    BX_ERROR(("VGATHERDPS_VpsHps: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedYmmRegister *mask = &BX_YMM_REG(i->src2()), *dest = &BX_YMM_REG(i->dst());

  // index size = 32, element_size = 32, max vector size = 256
  // num_elements:
  //     128 bit => 4
  //     256 bit => 8

  unsigned n, num_elements = 4 * i->getVL();

  for (n=0; n < num_elements; n++) {
    if (mask->ymm32u(n) & 0x80000000)
      mask->ymm32u(n) = 0xffffffff;
    else
      mask->ymm32u(n) = 0;
  }

  for (n=0; n < 8; n++)
  {
    if (n >= num_elements) {
        mask->ymm32u(n) = 0;
        dest->ymm32u(n) = 0;
        continue;
    }

    if (mask->ymm32u(n)) {
        dest->ymm32u(n) = read_virtual_dword(i->seg(), BxResolveGatherD(i, n));
    }
    mask->ymm32u(n) = 0;
  }

  BX_CLEAR_AVX_HIGH256(i->dst());
  BX_CLEAR_AVX_HIGH256(i->src2());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERQPS_VpsHps(bxInstruction_c *i)
{
  if (i->sibIndex() == i->src2() || i->sibIndex() == i->dst() || i->src2() == i->dst()) {
    BX_ERROR(("VGATHERQPS_VpsHps: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 64, element_size = 32, max vector size = 256
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4

  BxPackedYmmRegister *mask = &BX_YMM_REG(i->src2()), *dest = &BX_YMM_REG(i->dst());
  unsigned n, num_elements = 2 * i->getVL();

  for (n=0; n < num_elements; n++) {
    if (mask->ymm32u(n) & 0x80000000)
      mask->ymm32u(n) = 0xffffffff;
    else
      mask->ymm32u(n) = 0;
  }

  for (n=0; n < 4; n++)
  {
    if (n >= num_elements) {
        mask->ymm32u(n) = 0;
        dest->ymm32u(n) = 0;
        continue;
    }

    if (mask->ymm32u(n)) {
        dest->ymm32u(n) = read_virtual_dword(i->seg(), BxResolveGatherQ(i, n));
    }
    mask->ymm32u(n) = 0;
  }

  BX_CLEAR_AVX_HIGH128(i->dst());
  BX_CLEAR_AVX_HIGH128(i->src2());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERDPD_VpdHpd(bxInstruction_c *i)
{
  if (i->sibIndex() == i->src2() || i->sibIndex() == i->dst() || i->src2() == i->dst()) {
    BX_ERROR(("VGATHERDPD_VpdHpd: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 32, element_size = 64, max vector size = 256
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4

  BxPackedYmmRegister *mask = &BX_YMM_REG(i->src2()), *dest = &BX_YMM_REG(i->dst());
  unsigned n, num_elements = 2 * i->getVL();

  for (n=0; n < num_elements; n++) {
    if (mask->ymm64u(n) & BX_CONST64(0x8000000000000000))
      mask->ymm64u(n) = BX_CONST64(0xffffffffffffffff);
    else
      mask->ymm64u(n) = 0;
  }

  for (unsigned n=0; n < 4; n++)
  {
    if (n >= num_elements) {
        mask->ymm64u(n) = 0;
        dest->ymm64u(n) = 0;
        continue;
    }

    if (mask->ymm64u(n)) {
        dest->ymm64u(n) = read_virtual_qword(i->seg(), BxResolveGatherD(i, n));
    }
    mask->ymm64u(n) = 0;
  }

  BX_CLEAR_AVX_HIGH256(i->dst());
  BX_CLEAR_AVX_HIGH256(i->src2());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERQPD_VpdHpd(bxInstruction_c *i)
{
  if (i->sibIndex() == i->src2() || i->sibIndex() == i->dst() || i->src2() == i->dst()) {
    BX_ERROR(("VGATHERQPD_VpdHpd: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 64, element_size = 64, max vector size = 256
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4

  BxPackedYmmRegister *mask = &BX_YMM_REG(i->src2()), *dest = &BX_YMM_REG(i->dst());
  unsigned n, num_elements = 2 * i->getVL();

  for (n=0; n < num_elements; n++) {
    if (mask->ymm64u(n) & BX_CONST64(0x8000000000000000))
      mask->ymm64u(n) = BX_CONST64(0xffffffffffffffff);
    else
      mask->ymm64u(n) = 0;
  }

  for (n=0; n < 4; n++)
  {
    if (n >= num_elements) {
        mask->ymm64u(n) = 0;
        dest->ymm64u(n) = 0;
        continue;
    }

    if (mask->ymm64u(n)) {
        dest->ymm64u(n) = read_virtual_qword(i->seg(), BxResolveGatherQ(i, n));
    }
    mask->ymm64u(n) = 0;
  }

  BX_CLEAR_AVX_HIGH256(i->dst());
  BX_CLEAR_AVX_HIGH256(i->src2());

  BX_NEXT_INSTR(i);
}

#endif
