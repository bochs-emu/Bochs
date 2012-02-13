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

#ifndef BX_SIMD_INT_COMPARE_FUNCTIONS_H
#define BX_SIMD_INT_COMPARE_FUNCTIONS_H

// compare less than (signed)

BX_CPP_INLINE void sse_pcmpltb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) < op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpltw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) < op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpltd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) < op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpltq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) < op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare less than (unsigned)

BX_CPP_INLINE void sse_pcmpltub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) < op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpltuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) < op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpltud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) < op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpltuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) < op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare less than or equal (signed)

BX_CPP_INLINE void sse_pcmpleb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) <= op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmplew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) <= op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpled(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) <= op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpleq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) <= op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare less than or equal (unsigned)

BX_CPP_INLINE void sse_pcmpleub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) <= op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpleuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) <= op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpleud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) <= op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpleuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) <= op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare greater than (signed)

BX_CPP_INLINE void sse_pcmpgtb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) > op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgtw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) > op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgtd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) > op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgtq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) > op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare greater than (unsigned)

BX_CPP_INLINE void sse_pcmpgtub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) > op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgtuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) > op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgtud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) > op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgtuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) > op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare greater than or equal (signed)

BX_CPP_INLINE void sse_pcmpgeb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmsbyte(n) >= op2->xmmsbyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16s(n) >= op2->xmm16s(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpged(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32s(n) >= op2->xmm32s(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgeq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64s(n) >= op2->xmm64s(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare greater than or equal (unsigned)

BX_CPP_INLINE void sse_pcmpgeub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) >= op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgeuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) >= op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgeud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) >= op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpgeuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) >= op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare equal

BX_CPP_INLINE void sse_pcmpeqb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) == op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpeqw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) == op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpeqd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) == op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpeqq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) == op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare not equal

BX_CPP_INLINE void sse_pcmpneb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) != op2->xmmubyte(n)) ? 0xff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpnew(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) != op2->xmm16u(n)) ? 0xffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpned(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) = (op1->xmm32u(n) != op2->xmm32u(n)) ? 0xffffffff : 0;
  }
}

BX_CPP_INLINE void sse_pcmpneq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = (op1->xmm64u(n) != op2->xmm64u(n)) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
}

// compare true/false

BX_CPP_INLINE void sse_pcmptrue(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = BX_CONST64(0xffffffffffffffff);
  }
}

BX_CPP_INLINE void sse_pcmpfalse(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) = 0;
  }
}

#endif
