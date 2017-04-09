/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2013 Stanislav Shwartsman
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

#ifndef BX_SIMD_INT_COMPARE_FUNCTIONS_H
#define BX_SIMD_INT_COMPARE_FUNCTIONS_H

// compare less than (signed)

BX_CPP_INLINE void xmm_pcmpltb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) < op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpltb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmsbyte(n) < op2->xmmsbyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpltw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) < op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpltw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16s(n) < op2->xmm16s(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpltd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) < op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpltd_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32s(n) < op2->xmm32s(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpltq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) < op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpltq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64s(n) < op2->xmm64s(n)) mask |= (1 << n);
  }
  return mask;
}

// compare less than (unsigned)

BX_CPP_INLINE void xmm_pcmpltub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) < op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpltub_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmubyte(n) < op2->xmmubyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpltuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) < op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpltuw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16u(n) < op2->xmm16u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpltud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) < op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpltud_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32u(n) < op2->xmm32u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpltuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) < op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpltuq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64u(n) < op2->xmm64u(n)) mask |= (1 << n);
  }
  return mask;
}

// compare less than or equal (signed)

BX_CPP_INLINE void xmm_pcmpleb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) <= op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpleb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmsbyte(n) <= op2->xmmsbyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmplew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) <= op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmplew_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16s(n) <= op2->xmm16s(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpled(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) <= op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpled_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32s(n) <= op2->xmm32s(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpleq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) <= op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpleq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64s(n) <= op2->xmm64s(n)) mask |= (1 << n);
  }
  return mask;
}

// compare less than or equal (unsigned)

BX_CPP_INLINE void xmm_pcmpleub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) <= op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpleub_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmubyte(n) <= op2->xmmubyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpleuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) <= op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpleuw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16u(n) <= op2->xmm16u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpleud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) <= op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpleud_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32u(n) <= op2->xmm32u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpleuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) <= op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpleuq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64u(n) <= op2->xmm64u(n)) mask |= (1 << n);
  }
  return mask;
}

// compare greater than (signed)

BX_CPP_INLINE void xmm_pcmpgtb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) > op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgtb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmsbyte(n) > op2->xmmsbyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgtw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) > op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgtw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16s(n) > op2->xmm16s(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgtd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) > op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgtd_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32s(n) > op2->xmm32s(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgtq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) > op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgtq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64s(n) > op2->xmm64s(n)) mask |= (1 << n);
  }
  return mask;
}

// compare greater than (unsigned)

BX_CPP_INLINE void xmm_pcmpgtub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) > op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgtub_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmubyte(n) > op2->xmmubyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgtuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) > op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgtuw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16u(n) > op2->xmm16u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgtud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) > op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgtud_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32u(n) > op2->xmm32u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgtuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) > op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgtuq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64u(n) > op2->xmm64u(n)) mask |= (1 << n);
  }
  return mask;
}

// compare greater than or equal (signed)

BX_CPP_INLINE void xmm_pcmpgeb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) >= op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgeb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmsbyte(n) >= op2->xmmsbyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) >= op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgew_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16s(n) >= op2->xmm16s(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpged(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) >= op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpged_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32s(n) >= op2->xmm32s(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgeq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) >= op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgeq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64s(n) >= op2->xmm64s(n)) mask |= (1 << n);
  }
  return mask;
}

// compare greater than or equal (unsigned)

BX_CPP_INLINE void xmm_pcmpgeub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) >= op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgeub_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmubyte(n) >= op2->xmmubyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgeuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) >= op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgeuw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16u(n) >= op2->xmm16u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgeud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) >= op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgeud_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32u(n) >= op2->xmm32u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpgeuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) >= op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpgeuq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64u(n) >= op2->xmm64u(n)) mask |= (1 << n);
  }
  return mask;
}

// compare equal

BX_CPP_INLINE void xmm_pcmpeqb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) == op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpeqb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmubyte(n) == op2->xmmubyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpeqw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) == op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpeqw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16u(n) == op2->xmm16u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpeqd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) == op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpeqd_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32u(n) == op2->xmm32u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpeqq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) == op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpeqq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64u(n) == op2->xmm64u(n)) mask |= (1 << n);
  }
  return mask;
}

// compare not equal

BX_CPP_INLINE void xmm_pcmpneb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) != op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpneb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if (op1->xmmubyte(n) != op2->xmmubyte(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpnew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) != op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpnew_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if (op1->xmm16u(n) != op2->xmm16u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpned(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) != op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpned_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if (op1->xmm32u(n) != op2->xmm32u(n)) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE void xmm_pcmpneq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) != op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmpneq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if (op1->xmm64u(n) != op2->xmm64u(n)) mask |= (1 << n);
  }
  return mask;
}

// compare true/false

BX_CPP_INLINE void xmm_pcmptrue(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = BX_CONST64(0xffffffffffffffff);
  }
}

BX_CPP_INLINE void xmm_pcmpfalse(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE Bit32u xmm_pcmptrueb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  return 0xffff; // 16 elements
}

BX_CPP_INLINE Bit32u xmm_pcmptruew_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  return   0xff; // 8 elements
}

BX_CPP_INLINE Bit32u xmm_pcmptrued_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  return    0xf; // 4 elements
}

BX_CPP_INLINE Bit32u xmm_pcmptrueq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  return    0x3; // 2 elements
}

BX_CPP_INLINE Bit32u xmm_pcmpfalse_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  return    0x0;
}

// compare and test

BX_CPP_INLINE Bit32u xmm_ptestmb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if ((op1->xmmubyte(n) & op2->xmmubyte(n)) != 0) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE Bit32u xmm_ptestmw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if ((op1->xmm16u(n) & op2->xmm16u(n)) != 0) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE Bit32u xmm_ptestmd_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if ((op1->xmm32u(n) & op2->xmm32u(n)) != 0) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE Bit32u xmm_ptestmq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if ((op1->xmm64u(n) & op2->xmm64u(n)) != 0) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE Bit32u xmm_ptestnmb_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<16; n++) {
    if ((op1->xmmubyte(n) & op2->xmmubyte(n)) == 0) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE Bit32u xmm_ptestnmw_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<8; n++) {
    if ((op1->xmm16u(n) & op2->xmm16u(n)) == 0) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE Bit32u xmm_ptestnmd_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<4; n++) {
    if ((op1->xmm32u(n) & op2->xmm32u(n)) == 0) mask |= (1 << n);
  }
  return mask;
}

BX_CPP_INLINE Bit32u xmm_ptestnmq_mask(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit32u mask = 0;
  for(unsigned n=0; n<2; n++) {
    if ((op1->xmm64u(n) & op2->xmm64u(n)) == 0) mask |= (1 << n);
  }
  return mask;
}

#endif
