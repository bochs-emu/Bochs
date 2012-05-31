/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011 Stanislav Shwartsman
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
  Bit32s index = BX_READ_AVX_REG(i->sibIndex()).avx32s(element);
  
  if (i->as64L())
    return (BX_READ_64BIT_REG(i->sibBase()) + (((Bit64s) index) << i->sibScale()) + i->displ32s());
  else
    return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
}

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::BxResolveGatherQ(bxInstruction_c *i, unsigned element)
{
  Bit64s index = BX_READ_AVX_REG(i->sibIndex()).avx64s(element);
  
  if (i->as64L())
    return (BX_READ_64BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
  else
    return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERDPS_VpsHps(bxInstruction_c *i)
{
  if (! i->as32L()) {
    BX_ERROR(("VGATHERDPS_VpsHps: Gather with 16-bit address size"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->sibIndex() == BX_NIL_REGISTER) {
    BX_ERROR(("VGATHERDPS_VpsHps: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->sibIndex() == i->vvv() || i->sibIndex() == i->nnn() || i->vvv() == i->nnn()) {
    BX_ERROR(("VGATHERDPS_VpsHps: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister *mask = &BX_AVX_REG(i->vvv()), *dest = &BX_AVX_REG(i->nnn());
  unsigned num_elements = 4 * i->getVL();

  for (unsigned n=0; n < 8; n++) // index size = 32, element_size = 32, max vector size = 256
  {
    if (n >= num_elements) {
        mask->avx32u(n) = 0;
        dest->avx32u(n) = 0;
        continue;
    }

    if (mask->avx32u(n) & 0x80000000) {
        dest->avx32u(n) = read_virtual_dword(i->seg(), BxResolveGatherD(i, n));
    }
    mask->avx32u(n) = 0;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERQPS_VpsHps(bxInstruction_c *i)
{
  if (! i->as32L()) {
    BX_ERROR(("VGATHERQPS_VpsHps: Gather with 16-bit address size"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->sibIndex() == BX_NIL_REGISTER) {
    BX_ERROR(("VGATHERQPS_VpsHps: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->sibIndex() == i->vvv() || i->sibIndex() == i->nnn() || i->vvv() == i->nnn()) {
    BX_ERROR(("VGATHERQPS_VpsHps: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister *mask = &BX_AVX_REG(i->vvv()), *dest = &BX_AVX_REG(i->nnn());
  unsigned num_elements = 2 * i->getVL();

  for (unsigned n=0; n < 8; n++) // index size = 64, element_size = 32, max vector size = 256
  {
    if (n >= num_elements) {
        mask->avx32u(n) = 0;
        dest->avx32u(n) = 0;
        continue;
    }

    if (mask->avx32u(n) & 0x80000000) {
        dest->avx32u(n) = read_virtual_dword(i->seg(), BxResolveGatherQ(i, n));
    }
    mask->avx32u(n) = 0;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERDPD_VpdHpd(bxInstruction_c *i)
{
  if (! i->as32L()) {
    BX_ERROR(("VGATHERDPD_VpdHpd: Gather with 16-bit address size"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->sibIndex() == BX_NIL_REGISTER) {
    BX_ERROR(("VGATHERDPD_VpdHpd: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->sibIndex() == i->vvv() || i->sibIndex() == i->nnn() || i->vvv() == i->nnn()) {
    BX_ERROR(("VGATHERDPD_VpdHpd: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister *mask = &BX_AVX_REG(i->vvv()), *dest = &BX_AVX_REG(i->nnn());
  unsigned num_elements = 2 * i->getVL();

  for (unsigned n=0; n < 4; n++) // index size = 32, element_size = 64, max vector size = 256
  {
    if (n >= num_elements) {
        mask->avx64u(n) = 0;
        dest->avx64u(n) = 0;
        continue;
    }

    if (mask->avx64u(n) & BX_CONST64(0x8000000000000000)) {
        dest->avx64u(n) = read_virtual_qword(i->seg(), BxResolveGatherD(i, n));
    }
    mask->avx64u(n) = 0;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERQPD_VpdHpd(bxInstruction_c *i)
{
  if (! i->as32L()) {
    BX_ERROR(("VGATHERQPD_VpdHpd: Gather with 16-bit address size"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->sibIndex() == BX_NIL_REGISTER) {
    BX_ERROR(("VGATHERQPD_VpdHpd: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->sibIndex() == i->vvv() || i->sibIndex() == i->nnn() || i->vvv() == i->nnn()) {
    BX_ERROR(("VGATHERQPD_VpdHpd: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister *mask = &BX_AVX_REG(i->vvv()), *dest = &BX_AVX_REG(i->nnn());
  unsigned num_elements = 2 * i->getVL();

  for (unsigned n=0; n < 4; n++) // index size = 64, element_size = 64, max vector size = 256
  {
    if (n >= num_elements) {
        mask->avx64u(n) = 0;
        dest->avx64u(n) = 0;
        continue;
    }

    if (mask->avx64u(n) & BX_CONST64(0x8000000000000000)) {
        dest->avx64u(n) = read_virtual_qword(i->seg(), BxResolveGatherQ(i, n));
    }
    mask->avx64u(n) = 0;
  }

  BX_NEXT_INSTR(i);
}

#endif
