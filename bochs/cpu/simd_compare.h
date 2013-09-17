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

BX_CPP_INLINE void xmm_pcmpltw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) < op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpltd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) < op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpltq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) < op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare less than (unsigned)

BX_CPP_INLINE void xmm_pcmpltub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) < op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpltuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) < op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpltud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) < op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpltuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) < op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare less than or equal (signed)

BX_CPP_INLINE void xmm_pcmpleb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) <= op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmplew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) <= op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpled(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) <= op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpleq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) <= op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare less than or equal (unsigned)

BX_CPP_INLINE void xmm_pcmpleub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) <= op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpleuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) <= op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpleud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) <= op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpleuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) <= op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare greater than (signed)

BX_CPP_INLINE void xmm_pcmpgtb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) > op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgtw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) > op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgtd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) > op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgtq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) > op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare greater than (unsigned)

BX_CPP_INLINE void xmm_pcmpgtub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) > op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgtuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) > op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgtud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) > op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgtuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) > op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare greater than or equal (signed)

BX_CPP_INLINE void xmm_pcmpgeb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) >= op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) >= op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpged(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) >= op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgeq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) >= op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare greater than or equal (unsigned)

BX_CPP_INLINE void xmm_pcmpgeub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) >= op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgeuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) >= op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgeud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) >= op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpgeuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) >= op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare equal

BX_CPP_INLINE void xmm_pcmpeqb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) == op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpeqw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) == op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpeqd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) == op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpeqq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) == op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare not equal

BX_CPP_INLINE void xmm_pcmpneb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) != op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpnew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) != op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpned(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) != op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void xmm_pcmpneq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) != op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
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

#endif
