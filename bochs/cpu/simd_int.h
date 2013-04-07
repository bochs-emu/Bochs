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

#ifndef BX_SIMD_INT_FUNCTIONS_H
#define BX_SIMD_INT_FUNCTIONS_H

// absolute value

BX_CPP_INLINE void sse_pabsb(BxPackedXmmRegister *op)
{
  for(unsigned n=0; n<16; n++) {
    if(op->xmmsbyte(n) < 0) op->xmmubyte(n) = -op->xmmsbyte(n);
  }
}

BX_CPP_INLINE void sse_pabsw(BxPackedXmmRegister *op)
{
  for(unsigned n=0; n<8; n++) {
    if(op->xmm16s(n) < 0) op->xmm16u(n) = -op->xmm16s(n);
  }
}

BX_CPP_INLINE void sse_pabsd(BxPackedXmmRegister *op)
{
  for(unsigned n=0; n<4; n++) {
    if(op->xmm32s(n) < 0) op->xmm32u(n) = -op->xmm32s(n);
  }
}

// min/max

BX_CPP_INLINE void sse_pminsb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    if(op2->xmmsbyte(n) < op1->xmmsbyte(n)) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
}

BX_CPP_INLINE void sse_pminub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    if(op2->xmmubyte(n) < op1->xmmubyte(n)) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
}

BX_CPP_INLINE void sse_pminsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    if(op2->xmm16s(n) < op1->xmm16s(n)) op1->xmm16s(n) = op2->xmm16s(n);
  }
}

BX_CPP_INLINE void sse_pminuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    if(op2->xmm16u(n) < op1->xmm16u(n)) op1->xmm16s(n) = op2->xmm16s(n);
  }
}

BX_CPP_INLINE void sse_pminsd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    if(op2->xmm32s(n) < op1->xmm32s(n)) op1->xmm32u(n) = op2->xmm32u(n);
  }
}

BX_CPP_INLINE void sse_pminud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    if(op2->xmm32u(n) < op1->xmm32u(n)) op1->xmm32u(n) = op2->xmm32u(n);
  }
}

BX_CPP_INLINE void sse_pmaxsb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    if(op2->xmmsbyte(n) > op1->xmmsbyte(n)) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
}

BX_CPP_INLINE void sse_pmaxub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    if(op2->xmmubyte(n) > op1->xmmubyte(n)) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
}

BX_CPP_INLINE void sse_pmaxsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    if(op2->xmm16s(n) > op1->xmm16s(n)) op1->xmm16s(n) = op2->xmm16s(n);
  }
}

BX_CPP_INLINE void sse_pmaxuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    if(op2->xmm16u(n) > op1->xmm16u(n)) op1->xmm16s(n) = op2->xmm16s(n);
  }
}

BX_CPP_INLINE void sse_pmaxsd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    if(op2->xmm32s(n) > op1->xmm32s(n)) op1->xmm32u(n) = op2->xmm32u(n);
  }
}

BX_CPP_INLINE void sse_pmaxud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    if(op2->xmm32u(n) > op1->xmm32u(n)) op1->xmm32u(n) = op2->xmm32u(n);
  }
}

// unpack

BX_CPP_INLINE void sse_unpcklps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm32u(3) = op2->xmm32u(1);
  op1->xmm32u(2) = op1->xmm32u(1);
  op1->xmm32u(1) = op2->xmm32u(0);
//op1->xmm32u(0) = op1->xmm32u(0);
}

BX_CPP_INLINE void sse_unpckhps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm32u(0) = op1->xmm32u(2);
  op1->xmm32u(1) = op2->xmm32u(2);
  op1->xmm32u(2) = op1->xmm32u(3);
  op1->xmm32u(3) = op2->xmm32u(3);
}

BX_CPP_INLINE void sse_unpcklpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
//op1->xmm64u(0) = op1->xmm64u(0);
  op1->xmm64u(1) = op2->xmm64u(0);
}

BX_CPP_INLINE void sse_unpckhpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm64u(0) = op1->xmm64u(1);
  op1->xmm64u(1) = op2->xmm64u(1);
}

BX_CPP_INLINE void sse_punpcklbw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmmubyte(0xF) = op2->xmmubyte(7);
  op1->xmmubyte(0xE) = op1->xmmubyte(7);
  op1->xmmubyte(0xD) = op2->xmmubyte(6);
  op1->xmmubyte(0xC) = op1->xmmubyte(6);
  op1->xmmubyte(0xB) = op2->xmmubyte(5);
  op1->xmmubyte(0xA) = op1->xmmubyte(5);
  op1->xmmubyte(0x9) = op2->xmmubyte(4);
  op1->xmmubyte(0x8) = op1->xmmubyte(4);
  op1->xmmubyte(0x7) = op2->xmmubyte(3);
  op1->xmmubyte(0x6) = op1->xmmubyte(3);
  op1->xmmubyte(0x5) = op2->xmmubyte(2);
  op1->xmmubyte(0x4) = op1->xmmubyte(2);
  op1->xmmubyte(0x3) = op2->xmmubyte(1);
  op1->xmmubyte(0x2) = op1->xmmubyte(1);
  op1->xmmubyte(0x1) = op2->xmmubyte(0);
//op1->xmmubyte(0x0) = op1->xmmubyte(0);
}

BX_CPP_INLINE void sse_punpckhbw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmmubyte(0x0) = op1->xmmubyte(0x8);
  op1->xmmubyte(0x1) = op2->xmmubyte(0x8);
  op1->xmmubyte(0x2) = op1->xmmubyte(0x9);
  op1->xmmubyte(0x3) = op2->xmmubyte(0x9);
  op1->xmmubyte(0x4) = op1->xmmubyte(0xA);
  op1->xmmubyte(0x5) = op2->xmmubyte(0xA);
  op1->xmmubyte(0x6) = op1->xmmubyte(0xB);
  op1->xmmubyte(0x7) = op2->xmmubyte(0xB);
  op1->xmmubyte(0x8) = op1->xmmubyte(0xC);
  op1->xmmubyte(0x9) = op2->xmmubyte(0xC);
  op1->xmmubyte(0xA) = op1->xmmubyte(0xD);
  op1->xmmubyte(0xB) = op2->xmmubyte(0xD);
  op1->xmmubyte(0xC) = op1->xmmubyte(0xE);
  op1->xmmubyte(0xD) = op2->xmmubyte(0xE);
  op1->xmmubyte(0xE) = op1->xmmubyte(0xF);
  op1->xmmubyte(0xF) = op2->xmmubyte(0xF);
}

BX_CPP_INLINE void sse_punpcklwd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm16u(7) = op2->xmm16u(3);
  op1->xmm16u(6) = op1->xmm16u(3);
  op1->xmm16u(5) = op2->xmm16u(2);
  op1->xmm16u(4) = op1->xmm16u(2);
  op1->xmm16u(3) = op2->xmm16u(1);
  op1->xmm16u(2) = op1->xmm16u(1);
  op1->xmm16u(1) = op2->xmm16u(0);
//op1->xmm16u(0) = op1->xmm16u(0);
}

BX_CPP_INLINE void sse_punpckhwd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm16u(0) = op1->xmm16u(4);
  op1->xmm16u(1) = op2->xmm16u(4);
  op1->xmm16u(2) = op1->xmm16u(5);
  op1->xmm16u(3) = op2->xmm16u(5);
  op1->xmm16u(4) = op1->xmm16u(6);
  op1->xmm16u(5) = op2->xmm16u(6);
  op1->xmm16u(6) = op1->xmm16u(7);
  op1->xmm16u(7) = op2->xmm16u(7);
}
 
// pack

BX_CPP_INLINE void sse_packuswb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmmubyte(0x0) = SaturateWordSToByteU(op1->xmm16s(0));
  op1->xmmubyte(0x1) = SaturateWordSToByteU(op1->xmm16s(1));
  op1->xmmubyte(0x2) = SaturateWordSToByteU(op1->xmm16s(2));
  op1->xmmubyte(0x3) = SaturateWordSToByteU(op1->xmm16s(3));
  op1->xmmubyte(0x4) = SaturateWordSToByteU(op1->xmm16s(4));
  op1->xmmubyte(0x5) = SaturateWordSToByteU(op1->xmm16s(5));
  op1->xmmubyte(0x6) = SaturateWordSToByteU(op1->xmm16s(6));
  op1->xmmubyte(0x7) = SaturateWordSToByteU(op1->xmm16s(7));

  op1->xmmubyte(0x8) = SaturateWordSToByteU(op2->xmm16s(0));
  op1->xmmubyte(0x9) = SaturateWordSToByteU(op2->xmm16s(1));
  op1->xmmubyte(0xA) = SaturateWordSToByteU(op2->xmm16s(2));
  op1->xmmubyte(0xB) = SaturateWordSToByteU(op2->xmm16s(3));
  op1->xmmubyte(0xC) = SaturateWordSToByteU(op2->xmm16s(4));
  op1->xmmubyte(0xD) = SaturateWordSToByteU(op2->xmm16s(5));
  op1->xmmubyte(0xE) = SaturateWordSToByteU(op2->xmm16s(6));
  op1->xmmubyte(0xF) = SaturateWordSToByteU(op2->xmm16s(7));
}

BX_CPP_INLINE void sse_packsswb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmmsbyte(0x0) = SaturateWordSToByteS(op1->xmm16s(0));
  op1->xmmsbyte(0x1) = SaturateWordSToByteS(op1->xmm16s(1));
  op1->xmmsbyte(0x2) = SaturateWordSToByteS(op1->xmm16s(2));
  op1->xmmsbyte(0x3) = SaturateWordSToByteS(op1->xmm16s(3));
  op1->xmmsbyte(0x4) = SaturateWordSToByteS(op1->xmm16s(4));
  op1->xmmsbyte(0x5) = SaturateWordSToByteS(op1->xmm16s(5));
  op1->xmmsbyte(0x6) = SaturateWordSToByteS(op1->xmm16s(6));
  op1->xmmsbyte(0x7) = SaturateWordSToByteS(op1->xmm16s(7));

  op1->xmmsbyte(0x8) = SaturateWordSToByteS(op2->xmm16s(0));
  op1->xmmsbyte(0x9) = SaturateWordSToByteS(op2->xmm16s(1));
  op1->xmmsbyte(0xA) = SaturateWordSToByteS(op2->xmm16s(2));
  op1->xmmsbyte(0xB) = SaturateWordSToByteS(op2->xmm16s(3));
  op1->xmmsbyte(0xC) = SaturateWordSToByteS(op2->xmm16s(4));
  op1->xmmsbyte(0xD) = SaturateWordSToByteS(op2->xmm16s(5));
  op1->xmmsbyte(0xE) = SaturateWordSToByteS(op2->xmm16s(6));
  op1->xmmsbyte(0xF) = SaturateWordSToByteS(op2->xmm16s(7));
}

BX_CPP_INLINE void sse_packusdw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm16u(0) = SaturateDwordSToWordU(op1->xmm32s(0));
  op1->xmm16u(1) = SaturateDwordSToWordU(op1->xmm32s(1));
  op1->xmm16u(2) = SaturateDwordSToWordU(op1->xmm32s(2));
  op1->xmm16u(3) = SaturateDwordSToWordU(op1->xmm32s(3));

  op1->xmm16u(4) = SaturateDwordSToWordU(op2->xmm32s(0));
  op1->xmm16u(5) = SaturateDwordSToWordU(op2->xmm32s(1));
  op1->xmm16u(6) = SaturateDwordSToWordU(op2->xmm32s(2));
  op1->xmm16u(7) = SaturateDwordSToWordU(op2->xmm32s(3));
}

BX_CPP_INLINE void sse_packssdw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm16s(0) = SaturateDwordSToWordS(op1->xmm32s(0));
  op1->xmm16s(1) = SaturateDwordSToWordS(op1->xmm32s(1));
  op1->xmm16s(2) = SaturateDwordSToWordS(op1->xmm32s(2));
  op1->xmm16s(3) = SaturateDwordSToWordS(op1->xmm32s(3));

  op1->xmm16s(4) = SaturateDwordSToWordS(op2->xmm32s(0));
  op1->xmm16s(5) = SaturateDwordSToWordS(op2->xmm32s(1));
  op1->xmm16s(6) = SaturateDwordSToWordS(op2->xmm32s(2));
  op1->xmm16s(7) = SaturateDwordSToWordS(op2->xmm32s(3));
}

// shuffle

BX_CPP_INLINE void sse_pshufb(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++)
  {
    unsigned mask = op2->xmmubyte(n);
    if (mask & 0x80)
      r->xmmubyte(n) = 0;
    else
      r->xmmubyte(n) = op1->xmmubyte(mask & 0xf);
  }
}

BX_CPP_INLINE void sse_pshufhw(BxPackedXmmRegister *r, const BxPackedXmmRegister *op, Bit8u order)
{
  r->xmm64u(0) = op->xmm64u(0);
  r->xmm16u(4) = op->xmm16u(4 + ((order >> 0) & 0x3));
  r->xmm16u(5) = op->xmm16u(4 + ((order >> 2) & 0x3));
  r->xmm16u(6) = op->xmm16u(4 + ((order >> 4) & 0x3));
  r->xmm16u(7) = op->xmm16u(4 + ((order >> 6) & 0x3));
}

BX_CPP_INLINE void sse_pshuflw(BxPackedXmmRegister *r, const BxPackedXmmRegister *op, Bit8u order)
{
  r->xmm16u(0) = op->xmm16u((order >> 0) & 0x3);
  r->xmm16u(1) = op->xmm16u((order >> 2) & 0x3);
  r->xmm16u(2) = op->xmm16u((order >> 4) & 0x3);
  r->xmm16u(3) = op->xmm16u((order >> 6) & 0x3);
  r->xmm64u(1) = op->xmm64u(1);
}

BX_CPP_INLINE void sse_shufps(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit8u order)
{
  r->xmm32u(0) = op1->xmm32u((order >> 0) & 0x3);
  r->xmm32u(1) = op1->xmm32u((order >> 2) & 0x3);
  r->xmm32u(2) = op2->xmm32u((order >> 4) & 0x3);
  r->xmm32u(3) = op2->xmm32u((order >> 6) & 0x3);
}

BX_CPP_INLINE void sse_shufpd(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit8u order)
{
  r->xmm64u(0) = op1->xmm64u((order >> 0) & 0x1);
  r->xmm64u(1) = op2->xmm64u((order >> 1) & 0x1);
}

BX_CPP_INLINE void sse_permilps(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  r->xmm32u(0) = op1->xmm32u(op2->xmm32u(0) & 0x3);
  r->xmm32u(1) = op1->xmm32u(op2->xmm32u(1) & 0x3);
  r->xmm32u(2) = op1->xmm32u(op2->xmm32u(2) & 0x3);
  r->xmm32u(3) = op1->xmm32u(op2->xmm32u(3) & 0x3);
}

BX_CPP_INLINE void sse_permilpd(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  r->xmm64u(0) = op1->xmm64u((op2->xmm32u(0) >> 1) & 0x1);
  r->xmm64u(1) = op1->xmm64u((op2->xmm32u(2) >> 1) & 0x1);
}

BX_CPP_INLINE void sse_permil2ps(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, unsigned m2z)
{
  for(unsigned n=0; n < 4; n++) {
    Bit32u ctrl = op3->xmm32u(n);
    if ((m2z ^ ((ctrl >> 3) & 0x1)) == 0x3)
      r->xmm32u(n) = 0;
    else
      r->xmm32u(n) = (ctrl & 0x4) ? op1->xmm32u(ctrl & 0x3) : op2->xmm32u(ctrl & 0x3);
  }
}

BX_CPP_INLINE void sse_permil2pd(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, unsigned m2z)
{
  for(unsigned n=0; n < 2; n++) {
    Bit32u ctrl = op3->xmm32u(n*2);
    if ((m2z ^ ((ctrl >> 3) & 0x1)) == 0x3)
      r->xmm64u(n) = 0;
    else
      r->xmm64u(n) = (ctrl & 0x4) ? op1->xmm64u((ctrl >> 1) & 0x1) : op2->xmm64u((ctrl >> 1) & 0x1);
  }
}

// sign

BX_CPP_INLINE void sse_psignb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    int sign = (op2->xmmsbyte(n) > 0) - (op2->xmmsbyte(n) < 0);
    op1->xmmsbyte(n) *= sign;
  }
}

BX_CPP_INLINE void sse_psignw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    int sign = (op2->xmm16s(n) > 0) - (op2->xmm16s(n) < 0);
    op1->xmm16s(n) *= sign;
  }
}

BX_CPP_INLINE void sse_psignd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    int sign = (op2->xmm32s(n) > 0) - (op2->xmm32s(n) < 0);
    op1->xmm32s(n) *= sign;
  }
}

// mask creation

BX_CPP_INLINE Bit32u sse_pmovmskb(const BxPackedXmmRegister *op)
{
  unsigned mask = 0;

  if(op->xmmubyte(0x0) & 0x80) mask |= 0x0001;
  if(op->xmmubyte(0x1) & 0x80) mask |= 0x0002;
  if(op->xmmubyte(0x2) & 0x80) mask |= 0x0004;
  if(op->xmmubyte(0x3) & 0x80) mask |= 0x0008;
  if(op->xmmubyte(0x4) & 0x80) mask |= 0x0010;
  if(op->xmmubyte(0x5) & 0x80) mask |= 0x0020;
  if(op->xmmubyte(0x6) & 0x80) mask |= 0x0040;
  if(op->xmmubyte(0x7) & 0x80) mask |= 0x0080;
  if(op->xmmubyte(0x8) & 0x80) mask |= 0x0100;
  if(op->xmmubyte(0x9) & 0x80) mask |= 0x0200;
  if(op->xmmubyte(0xA) & 0x80) mask |= 0x0400;
  if(op->xmmubyte(0xB) & 0x80) mask |= 0x0800;
  if(op->xmmubyte(0xC) & 0x80) mask |= 0x1000;
  if(op->xmmubyte(0xD) & 0x80) mask |= 0x2000;
  if(op->xmmubyte(0xE) & 0x80) mask |= 0x4000;
  if(op->xmmubyte(0xF) & 0x80) mask |= 0x8000;

  return mask;
}

BX_CPP_INLINE Bit32u sse_pmovmskd(const BxPackedXmmRegister *op)
{
  unsigned mask = 0;

  if(op->xmm32u(0) & 0x80000000) mask |= 0x1;
  if(op->xmm32u(1) & 0x80000000) mask |= 0x2;
  if(op->xmm32u(2) & 0x80000000) mask |= 0x4;
  if(op->xmm32u(3) & 0x80000000) mask |= 0x8;

  return mask;
}

BX_CPP_INLINE Bit32u sse_pmovmskq(const BxPackedXmmRegister *op)
{
  unsigned mask = 0;

  if(op->xmm32u(1) & 0x80000000) mask |= 0x1;
  if(op->xmm32u(3) & 0x80000000) mask |= 0x2;

  return mask;
}

// blend

BX_CPP_INLINE void sse_pblendw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, unsigned mask)
{
  if (mask & 0x01) op1->xmm16u(0) = op2->xmm16u(0);
  if (mask & 0x02) op1->xmm16u(1) = op2->xmm16u(1);
  if (mask & 0x04) op1->xmm16u(2) = op2->xmm16u(2);
  if (mask & 0x08) op1->xmm16u(3) = op2->xmm16u(3);
  if (mask & 0x10) op1->xmm16u(4) = op2->xmm16u(4);
  if (mask & 0x20) op1->xmm16u(5) = op2->xmm16u(5);
  if (mask & 0x40) op1->xmm16u(6) = op2->xmm16u(6);
  if (mask & 0x80) op1->xmm16u(7) = op2->xmm16u(7);
}

BX_CPP_INLINE void sse_blendps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, unsigned mask)
{
  if (mask & 0x1) op1->xmm32u(0) = op2->xmm32u(0);
  if (mask & 0x2) op1->xmm32u(1) = op2->xmm32u(1);
  if (mask & 0x4) op1->xmm32u(2) = op2->xmm32u(2);
  if (mask & 0x8) op1->xmm32u(3) = op2->xmm32u(3);
}

BX_CPP_INLINE void sse_blendpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, unsigned mask)
{
  if (mask & 0x1) op1->xmm64u(0) = op2->xmm64u(0);
  if (mask & 0x2) op1->xmm64u(1) = op2->xmm64u(1);
}

BX_CPP_INLINE void sse_pblendvb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *mask)
{
  for(unsigned n=0; n<16; n++) {
    if (mask->xmmubyte(n) & 0x80) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
}

BX_CPP_INLINE void sse_blendvps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *mask)
{
  for(unsigned n=0; n<4; n++) {
    if (mask->xmm32u(n) & 0x80000000) op1->xmm32u(n) = op2->xmm32u(n);
  }
}

BX_CPP_INLINE void sse_blendvpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *mask)
{
  if (mask->xmm32u(1) & 0x80000000) op1->xmm64u(0) = op2->xmm64u(0);
  if (mask->xmm32u(3) & 0x80000000) op1->xmm64u(1) = op2->xmm64u(1);
}

// arithmetic (logic)

BX_CPP_INLINE void sse_andps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm64u(0) &= op2->xmm64u(0);
  op1->xmm64u(1) &= op2->xmm64u(1);
}

BX_CPP_INLINE void sse_andnps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm64u(0) = ~(op1->xmm64u(0)) & op2->xmm64u(0);
  op1->xmm64u(1) = ~(op1->xmm64u(1)) & op2->xmm64u(1);
}

BX_CPP_INLINE void sse_orps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm64u(0) |= op2->xmm64u(0);
  op1->xmm64u(1) |= op2->xmm64u(1);
}

BX_CPP_INLINE void sse_xorps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm64u(0) ^= op2->xmm64u(0);
  op1->xmm64u(1) ^= op2->xmm64u(1);
}

// arithmetic (add/sub)

BX_CPP_INLINE void sse_paddb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) += op2->xmmubyte(n);
  }
}

BX_CPP_INLINE void sse_paddw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) += op2->xmm16u(n);
  }
}

BX_CPP_INLINE void sse_paddd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) += op2->xmm32u(n);
  }
}

BX_CPP_INLINE void sse_paddq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) += op2->xmm64u(n);
  }
}

BX_CPP_INLINE void sse_psubb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) -= op2->xmmubyte(n);
  }
}

BX_CPP_INLINE void sse_psubw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) -= op2->xmm16u(n);
  }
}

BX_CPP_INLINE void sse_psubd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) -= op2->xmm32u(n);
  }
}

BX_CPP_INLINE void sse_psubq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) -= op2->xmm64u(n);
  }
}

// arithmetic (add/sub with saturation)

BX_CPP_INLINE void sse_paddsb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmsbyte(n) = SaturateWordSToByteS(Bit16s(op1->xmmsbyte(n)) + Bit16s(op2->xmmsbyte(n)));
  }
}

BX_CPP_INLINE void sse_paddsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16s(n) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(n)) + Bit32s(op2->xmm16s(n)));
  }
}

BX_CPP_INLINE void sse_paddusb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = SaturateWordSToByteU(Bit16s(op1->xmmubyte(n)) + Bit16s(op2->xmmubyte(n)));
  }
}

BX_CPP_INLINE void sse_paddusw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = SaturateDwordSToWordU(Bit32s(op1->xmm16u(n)) + Bit32s(op2->xmm16u(n)));
  }
}

BX_CPP_INLINE void sse_psubsb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmsbyte(n) = SaturateWordSToByteS(Bit16s(op1->xmmsbyte(n)) - Bit16s(op2->xmmsbyte(n)));
  }
}

BX_CPP_INLINE void sse_psubsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16s(n) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(n)) - Bit32s(op2->xmm16s(n)));
  }
}

BX_CPP_INLINE void sse_psubusb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++)
  {
    if(op1->xmmubyte(n) > op2->xmmubyte(n))
      op1->xmmubyte(n) -= op2->xmmubyte(n);
    else
      op1->xmmubyte(n) = 0;
  }
}

BX_CPP_INLINE void sse_psubusw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++)
  {
    if(op1->xmm16u(n) > op2->xmm16u(n))
      op1->xmm16u(n) -= op2->xmm16u(n);
    else
      op1->xmm16u(n) = 0;
  }
}

// arithmetic (horizontal add/sub)

BX_CPP_INLINE void sse_phaddw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm16u(0) = op1->xmm16u(0) + op1->xmm16u(1);
  op1->xmm16u(1) = op1->xmm16u(2) + op1->xmm16u(3);
  op1->xmm16u(2) = op1->xmm16u(4) + op1->xmm16u(5);
  op1->xmm16u(3) = op1->xmm16u(6) + op1->xmm16u(7);

  op1->xmm16u(4) = op2->xmm16u(0) + op2->xmm16u(1);
  op1->xmm16u(5) = op2->xmm16u(2) + op2->xmm16u(3);
  op1->xmm16u(6) = op2->xmm16u(4) + op2->xmm16u(5);
  op1->xmm16u(7) = op2->xmm16u(6) + op2->xmm16u(7);
}

BX_CPP_INLINE void sse_phaddd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm32u(0) = op1->xmm32u(0) + op1->xmm32u(1);
  op1->xmm32u(1) = op1->xmm32u(2) + op1->xmm32u(3);
  op1->xmm32u(2) = op2->xmm32u(0) + op2->xmm32u(1);
  op1->xmm32u(3) = op2->xmm32u(2) + op2->xmm32u(3);
}

BX_CPP_INLINE void sse_phaddsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(0)) + Bit32s(op1->xmm16s(1)));
  op1->xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(2)) + Bit32s(op1->xmm16s(3)));
  op1->xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(4)) + Bit32s(op1->xmm16s(5)));
  op1->xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(6)) + Bit32s(op1->xmm16s(7)));

  op1->xmm16s(4) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(0)) + Bit32s(op2->xmm16s(1)));
  op1->xmm16s(5) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(2)) + Bit32s(op2->xmm16s(3)));
  op1->xmm16s(6) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(4)) + Bit32s(op2->xmm16s(5)));
  op1->xmm16s(7) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(6)) + Bit32s(op2->xmm16s(7)));
}

BX_CPP_INLINE void sse_phsubw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm16u(0) = op1->xmm16u(0) - op1->xmm16u(1);
  op1->xmm16u(1) = op1->xmm16u(2) - op1->xmm16u(3);
  op1->xmm16u(2) = op1->xmm16u(4) - op1->xmm16u(5);
  op1->xmm16u(3) = op1->xmm16u(6) - op1->xmm16u(7);

  op1->xmm16u(4) = op2->xmm16u(0) - op2->xmm16u(1);
  op1->xmm16u(5) = op2->xmm16u(2) - op2->xmm16u(3);
  op1->xmm16u(6) = op2->xmm16u(4) - op2->xmm16u(5);
  op1->xmm16u(7) = op2->xmm16u(6) - op2->xmm16u(7);
}

BX_CPP_INLINE void sse_phsubd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm32u(0) = op1->xmm32u(0) - op1->xmm32u(1);
  op1->xmm32u(1) = op1->xmm32u(2) - op1->xmm32u(3);
  op1->xmm32u(2) = op2->xmm32u(0) - op2->xmm32u(1);
  op1->xmm32u(3) = op2->xmm32u(2) - op2->xmm32u(3);
}

BX_CPP_INLINE void sse_phsubsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(0)) - Bit32s(op1->xmm16s(1)));
  op1->xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(2)) - Bit32s(op1->xmm16s(3)));
  op1->xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(4)) - Bit32s(op1->xmm16s(5)));
  op1->xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(6)) - Bit32s(op1->xmm16s(7)));

  op1->xmm16s(4) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(0)) - Bit32s(op2->xmm16s(1)));
  op1->xmm16s(5) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(2)) - Bit32s(op2->xmm16s(3)));
  op1->xmm16s(6) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(4)) - Bit32s(op2->xmm16s(5)));
  op1->xmm16s(7) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(6)) - Bit32s(op2->xmm16s(7)));
}

// average

BX_CPP_INLINE void sse_pavgb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) + op2->xmmubyte(n) + 1) >> 1;
  }
}

BX_CPP_INLINE void sse_pavgw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) + op2->xmm16u(n) + 1) >> 1;
  }
}

// multiply

BX_CPP_INLINE void sse_pmullw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    Bit32u product = Bit32u(op1->xmm16u(n)) * Bit32u(op2->xmm16u(n));
    op1->xmm16u(n) = product & 0xffff;
  }
}

BX_CPP_INLINE void sse_pmulhw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    Bit32s product = Bit32s(op1->xmm16s(n)) * Bit32s(op2->xmm16s(n));
    op1->xmm16u(n) = (Bit16u)(product >> 16);
  }
}

BX_CPP_INLINE void sse_pmulhuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    Bit32u product = Bit32u(op1->xmm16u(n)) * Bit32u(op2->xmm16u(n));
    op1->xmm16u(n) = (Bit16u)(product >> 16);
  }
}

BX_CPP_INLINE void sse_pmulld(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++) {
    Bit64s product = Bit64s(op1->xmm32s(n)) * Bit64s(op2->xmm32s(n));
    op1->xmm32u(n) = (Bit32u)(product & 0xffffffff);
  }
}

BX_CPP_INLINE void sse_pmuldq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm64s(0) = Bit64s(op1->xmm32s(0)) * Bit64s(op2->xmm32s(0));
  op1->xmm64s(1) = Bit64s(op1->xmm32s(2)) * Bit64s(op2->xmm32s(2));
}

BX_CPP_INLINE void sse_pmuludq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm64u(0) = Bit64u(op1->xmm32u(0)) * Bit64u(op2->xmm32u(0));
  op1->xmm64u(1) = Bit64u(op1->xmm32u(2)) * Bit64u(op2->xmm32u(2));
}

BX_CPP_INLINE void sse_pmulhrsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (((op1->xmm16s(n) * op2->xmm16s(n)) >> 14) + 1) >> 1;
  }
}

// multiply/add

BX_CPP_INLINE void sse_pmaddubsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<8; n++)
  {
    Bit32s temp = Bit32s(op1->xmmubyte(n*2))   * Bit32s(op2->xmmsbyte(n*2)) +
                  Bit32s(op1->xmmubyte(n*2+1)) * Bit32s(op2->xmmsbyte(n*2+1));

    op1->xmm16s(n) = SaturateDwordSToWordS(temp);
  }
}

BX_CPP_INLINE void sse_pmaddwd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    if((op1->xmm32u(n) & op2->xmm32u(n)) == 0x80008000) {
      op1->xmm32u(n) = 0x80000000;
    }
    else {
      op1->xmm32u(n) = Bit32s(op1->xmm16s(n*2))   * Bit32s(op2->xmm16s(n*2)) + 
                       Bit32s(op1->xmm16s(n*2+1)) * Bit32s(op2->xmm16s(n*2+1));
    }
  }
}

// broadcast

BX_CPP_INLINE void sse_pbroadcastb(BxPackedXmmRegister *op, Bit8u val_8)
{
  for(unsigned n=0; n<16; n++) {
    op->xmmubyte(n) = val_8;
  }
}

BX_CPP_INLINE void sse_pbroadcastw(BxPackedXmmRegister *op, Bit16u val_16)
{
  for(unsigned n=0; n<8; n++) {
    op->xmm16u(n) = val_16;
  }
}

BX_CPP_INLINE void sse_pbroadcastd(BxPackedXmmRegister *op, Bit32u val_32)
{
  for(unsigned n=0; n<4; n++) {
    op->xmm32u(n) = val_32;
  }
}

BX_CPP_INLINE void sse_pbroadcastq(BxPackedXmmRegister *op, Bit64u val_64)
{
  for(unsigned n=0; n<2; n++) {
    op->xmm64u(n) = val_64;
  }
}

// sum of absolute differences (SAD)

BX_CPP_INLINE void sse_psadbw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  Bit16u temp1 = 0, temp2 = 0;

  temp1 += abs(op1->xmmubyte(0x0) - op2->xmmubyte(0x0));
  temp1 += abs(op1->xmmubyte(0x1) - op2->xmmubyte(0x1));
  temp1 += abs(op1->xmmubyte(0x2) - op2->xmmubyte(0x2));
  temp1 += abs(op1->xmmubyte(0x3) - op2->xmmubyte(0x3));
  temp1 += abs(op1->xmmubyte(0x4) - op2->xmmubyte(0x4));
  temp1 += abs(op1->xmmubyte(0x5) - op2->xmmubyte(0x5));
  temp1 += abs(op1->xmmubyte(0x6) - op2->xmmubyte(0x6));
  temp1 += abs(op1->xmmubyte(0x7) - op2->xmmubyte(0x7));

  temp2 += abs(op1->xmmubyte(0x8) - op2->xmmubyte(0x8));
  temp2 += abs(op1->xmmubyte(0x9) - op2->xmmubyte(0x9));
  temp2 += abs(op1->xmmubyte(0xA) - op2->xmmubyte(0xA));
  temp2 += abs(op1->xmmubyte(0xB) - op2->xmmubyte(0xB));
  temp2 += abs(op1->xmmubyte(0xC) - op2->xmmubyte(0xC));
  temp2 += abs(op1->xmmubyte(0xD) - op2->xmmubyte(0xD));
  temp2 += abs(op1->xmmubyte(0xE) - op2->xmmubyte(0xE));
  temp2 += abs(op1->xmmubyte(0xF) - op2->xmmubyte(0xF));

  op1->xmm64u(0) = Bit64u(temp1);
  op1->xmm64u(1) = Bit64u(temp2);
}

// multiple sum of absolute differences (MSAD)

BX_CPP_INLINE void sse_mpsadbw(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit8u offset)
{
  unsigned src_offset = (offset & 0x3) * 4;
  unsigned dst_offset = ((offset >> 2) & 1) * 4;

  for (unsigned j=0; j < 8; j++)
  {
    r->xmm16u(j) = 0;

    for (unsigned k=0; k < 4; k++) {
      Bit8u temp1 = op1->xmmubyte(j + k + dst_offset);
      Bit8u temp2 = op2->xmmubyte(    k + src_offset);
      if (temp1 > temp2)
        r->xmm16u(j) += (temp1 - temp2);
      else
        r->xmm16u(j) += (temp2 - temp1);
    }
  }
}

// bitwise select

BX_CPP_INLINE void sse_pselect(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3)
{
  for(unsigned n=0;n < 2;n++) {
    op1->xmm64u(n) = (op3->xmm64u(n) & op1->xmm64u(n)) | (~op3->xmm64u(n) & op2->xmm64u(n));
  }
}

// shift

BX_CPP_INLINE void sse_psravd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 4; n++) {
    Bit32u shift = op2->xmm32u(n);
    if(shift > 31)
      op1->xmm32u(n) = (op1->xmm32u(n) & 0x80000000) ? 0xffffffff : 0;
    else    
      op1->xmm32u(n) = (Bit32u)(op1->xmm32s(n) >> shift);
  }
}

BX_CPP_INLINE void sse_psllvd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 4; n++) {
    Bit32u shift = op2->xmm32u(n);
    if(shift > 31)
      op1->xmm32u(n) = 0;
    else    
      op1->xmm32u(n) <<= shift;
  }
}

BX_CPP_INLINE void sse_psllvq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 2; n++) {
    Bit64u shift = op2->xmm64u(n);
    if(shift > 63)
      op1->xmm64u(n) = 0;
    else    
      op1->xmm64u(n) <<= shift;
  }
}

BX_CPP_INLINE void sse_psrlvd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 4; n++) {
    Bit32u shift = op2->xmm32u(n);
    if(shift > 31)
      op1->xmm32u(n) = 0;
    else    
      op1->xmm32u(n) >>= shift;
  }
}

BX_CPP_INLINE void sse_psrlvq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 2; n++) {
    Bit64u shift = op2->xmm64u(n);
    if(shift > 63)
      op1->xmm64u(n) = 0;
    else    
      op1->xmm64u(n) >>= shift;
  }
}

BX_CPP_INLINE void sse_psraw(BxPackedXmmRegister *op, Bit64u shift_64)
{
  if(shift_64 > 15) {
    for (unsigned n=0; n < 8; n++)
      op->xmm16u(n) = (op->xmm16u(n) & 0x8000) ? 0xffff : 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 8; n++)
      op->xmm16u(n) = (Bit16u)(op->xmm16s(n) >> shift);
  }
}

BX_CPP_INLINE void sse_psrad(BxPackedXmmRegister *op, Bit64u shift_64)
{
  if(shift_64 > 31) {
    for (unsigned n=0; n < 4; n++)
      op->xmm32u(n) = (op->xmm32u(n) & 0x80000000) ? 0xffffffff : 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 4; n++)
      op->xmm32u(n) = (Bit32u)(op->xmm32s(n) >> shift);
  }
}

BX_CPP_INLINE void sse_psrlw(BxPackedXmmRegister *op, Bit64u shift_64)
{
  if(shift_64 > 15) {
    op->xmm64u(0) = 0;
    op->xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 8; n++)
      op->xmm16u(n) >>= shift;
  }
}

BX_CPP_INLINE void sse_psrld(BxPackedXmmRegister *op, Bit64u shift_64)
{
  if(shift_64 > 31) {
    op->xmm64u(0) = 0;
    op->xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 4; n++)
      op->xmm32u(n) >>= shift;
  }
}

BX_CPP_INLINE void sse_psrlq(BxPackedXmmRegister *op, Bit64u shift_64)
{
  if(shift_64 > 64) {
    op->xmm64u(0) = 0;
    op->xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 2; n++)
      op->xmm64u(n) >>= shift;
  }
}

BX_CPP_INLINE void sse_psllw(BxPackedXmmRegister *op, Bit64u shift_64)
{
  if(shift_64 > 15) {
    op->xmm64u(0) = 0;
    op->xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 8; n++)
      op->xmm16u(n) <<= shift;
  }
}

BX_CPP_INLINE void sse_pslld(BxPackedXmmRegister *op, Bit64u shift_64)
{
  if(shift_64 > 31) {
    op->xmm64u(0) = 0;
    op->xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 4; n++)
      op->xmm32u(n) <<= shift;
  }
}

BX_CPP_INLINE void sse_psllq(BxPackedXmmRegister *op, Bit64u shift_64)
{
  if(shift_64 > 63) {
    op->xmm64u(0) = 0;
    op->xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 2; n++)
      op->xmm64u(n) <<= shift;
  }
}

BX_CPP_INLINE void sse_psrldq(BxPackedXmmRegister *op, Bit8u shift)
{
  if(shift > 15) {
    op->xmm64u(0) = 0;
    op->xmm64u(1) = 0;
  }
  else {
    if (shift > 7) {
      op->xmm64u(0) = op->xmm64u(1);
      op->xmm64u(1) = 0;
      shift -= 8;
    }

    shift <<= 3;

    if (shift != 0) {
      op->xmm64u(0) = (op->xmm64u(0) >> shift) | (op->xmm64u(1) << (64-shift));
      op->xmm64u(1) = (op->xmm64u(1) >> shift);
    }
  }
}

BX_CPP_INLINE void sse_pslldq(BxPackedXmmRegister *op, Bit8u shift)
{
  if(shift > 15) {
    op->xmm64u(0) = 0;
    op->xmm64u(1) = 0;
  }
  else {
    if (shift > 7) {
      op->xmm64u(1) = op->xmm64u(0);
      op->xmm64u(0) = 0;
      shift -= 8;
    }

    shift <<= 3;

    if (shift != 0) {
      op->xmm64u(1) = (op->xmm64u(1) << shift) | (op->xmm64u(0) >> (64-shift));
      op->xmm64u(0) = (op->xmm64u(0) << shift);
    }
  }
}

BX_CPP_INLINE void sse_palignr(BxPackedXmmRegister *op2, const BxPackedXmmRegister *op1, Bit8u shift)
{
  // op2 = [op1:op2] >> shift

  if (shift > 15) {
    *op2 = *op1;
    sse_psrldq(op2, shift - 16);
    return;
  }

  shift <<= 3;

  if (shift > 64) {
    shift -= 64;
    op2->xmm64u(0) = (op2->xmm64u(1) >> shift) | (op1->xmm64u(0) << (64-shift));
    op2->xmm64u(1) = (op1->xmm64u(0) >> shift) | (op1->xmm64u(1) << (64-shift));
  }
  else if (shift == 64) {
    op2->xmm64u(0) = op2->xmm64u(1);
    op2->xmm64u(1) = op1->xmm64u(0);
  }
  else if (shift != 0) {
    op2->xmm64u(0) = (op2->xmm64u(0) >> shift) | (op2->xmm64u(1) << (64-shift));
    op2->xmm64u(1) = (op2->xmm64u(1) >> shift) | (op1->xmm64u(0) << (64-shift));
  }
}

// rotate (right)

BX_CPP_INLINE void sse_prorb(BxPackedXmmRegister *op, int shift)
{
  shift &= 0x7;

  for(unsigned n=0;n<16;n++) {
    op->xmmubyte(n) = (op->xmmubyte(n) >> shift) | (op->xmmubyte(n) << (8 - shift));
  }
}

BX_CPP_INLINE void sse_prorw(BxPackedXmmRegister *op, int shift)
{
  shift &= 0xf;

  for(unsigned n=0;n<8;n++) {
    op->xmm16u(n) = (op->xmm16u(n) >> shift) | (op->xmm16u(n) << (16 - shift));
  }
}

BX_CPP_INLINE void sse_prord(BxPackedXmmRegister *op, int shift)
{
  shift &= 0x1f;

  for(unsigned n=0;n<4;n++) {
    op->xmm32u(n) = (op->xmm32u(n) >> shift) | (op->xmm32u(n) << (32 - shift));
  }
}

BX_CPP_INLINE void sse_prorq(BxPackedXmmRegister *op, int shift)
{
  shift &= 0x3f;

  for(unsigned n=0;n<2;n++) {
    op->xmm64u(n) = (op->xmm64u(n) >> shift) | (op->xmm64u(n) << (64 - shift));
  }
}

// rotate (left)

BX_CPP_INLINE void sse_prolb(BxPackedXmmRegister *op, int shift)
{
  shift &= 0x7;

  for(unsigned n=0;n<16;n++) {
    op->xmmubyte(n) = (op->xmmubyte(n) << shift) | (op->xmmubyte(n) >> (8 - shift));
  }
}

BX_CPP_INLINE void sse_prolw(BxPackedXmmRegister *op, int shift)
{
  shift &= 0xf;

  for(unsigned n=0;n<8;n++) {
    op->xmm16u(n) = (op->xmm16u(n) << shift) | (op->xmm16u(n) >> (16 - shift));
  }
}

BX_CPP_INLINE void sse_prold(BxPackedXmmRegister *op, int shift)
{
  shift &= 0x1f;

  for(unsigned n=0;n<4;n++) {
    op->xmm32u(n) = (op->xmm32u(n) << shift) | (op->xmm32u(n) >> (32 - shift));
  }
}

BX_CPP_INLINE void sse_prolq(BxPackedXmmRegister *op, int shift)
{
  shift &= 0x3f;

  for(unsigned n=0;n<2;n++) {
    op->xmm64u(n) = (op->xmm64u(n) << shift) | (op->xmm64u(n) >> (64 - shift));
  }
}

// variable shift/rotate (XOP)

BX_CPP_INLINE void sse_protb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 16;n++) {
    int shift = op2->xmmsbyte(n);
    if (shift > 0) {
      // rotate left
      shift &= 0x7;
      op1->xmmubyte(n) = (op1->xmmubyte(n) << shift) | (op1->xmmubyte(n) >> (8 - shift));
    }
    else if (shift < 0) {
      // rotate right
      shift = -shift & 0x7;
      op1->xmmubyte(n) = (op1->xmmubyte(n) >> shift) | (op1->xmmubyte(n) << (8 - shift));
    }
  }
}

BX_CPP_INLINE void sse_protw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 8;n++) {
    int shift = op2->xmmsbyte(n*2);
    if (shift > 0) {
      // rotate left
      shift &= 0xf;
      op1->xmm16u(n) = (op1->xmm16u(n) << shift) | (op1->xmm16u(n) >> (16 - shift));
    }
    else if (shift < 0) {
      // rotate right
      shift = -shift & 0xf;
      op1->xmm16u(n) = (op1->xmm16u(n) >> shift) | (op1->xmm16u(n) << (16 - shift));
    }
  }
}

BX_CPP_INLINE void sse_protd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 4;n++) {
    int shift = op2->xmmsbyte(n*4);
    if (shift > 0) {
      // rotate left
      shift &= 0x1f;
      op1->xmm32u(n) = (op1->xmm32u(n) << shift) | (op1->xmm32u(n) >> (32 - shift));
    }
    else if (shift < 0) {
      // rotate right
      shift = -shift & 0x1f;
      op1->xmm32u(n) = (op1->xmm32u(n) >> shift) | (op1->xmm32u(n) << (32 - shift));
    }
  }
}

BX_CPP_INLINE void sse_protq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 2;n++) {
    int shift = op2->xmmsbyte(n*8);
    if (shift > 0) {
      // rotate left
      shift &= 0x3f;
      op1->xmm64u(n) = (op1->xmm64u(n) << shift) | (op1->xmm64u(n) >> (64 - shift));
    }
    else if (shift < 0) {
      // rotate right
      shift = -shift & 0x3f;
      op1->xmm64u(n) = (op1->xmm64u(n) >> shift) | (op1->xmm64u(n) << (64 - shift));
    }
  }
}

BX_CPP_INLINE void sse_pshab(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 16;n++) {
    int shift = op2->xmmsbyte(n);
    if (shift > 0) {
      // shift left
      op1->xmmsbyte(n) <<= (shift & 0x7);
    }
    else if (shift < 0) {
      // shift right
      op1->xmmsbyte(n) >>= (-shift & 0x7);
    }
  }
}

BX_CPP_INLINE void sse_pshaw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 8;n++) {
    int shift = op2->xmmsbyte(n*2);
    if (shift > 0) {
      // shift left
      op1->xmm16s(n) <<= (shift & 0xf);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm16s(n) >>= (-shift & 0xf);
    }
  }
}

BX_CPP_INLINE void sse_pshad(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 4;n++) {
    int shift = op2->xmmsbyte(n*4);
    if (shift > 0) {
      // shift left
      op1->xmm32s(n) <<= (shift & 0x1f);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm32s(n) >>= (-shift & 0x1f);
    }
  }
}

BX_CPP_INLINE void sse_pshaq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 2;n++) {
    int shift = op2->xmmsbyte(n*8);
    if (shift > 0) {
      // shift left
      op1->xmm64s(n) <<= (shift & 0x3f);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm64s(n) >>= (-shift & 0x3f);
    }
  }
}

BX_CPP_INLINE void sse_pshlb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 16;n++) {
    int shift = op2->xmmsbyte(n);
    if (shift > 0) {
      // shift left
      op1->xmmubyte(n) <<= (shift & 0x7);
    }
    else if (shift < 0) {
      // shift right
      op1->xmmubyte(n) >>= (-shift & 0x7);
    }
  }
}

BX_CPP_INLINE void sse_pshlw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 8;n++) {
    int shift = op2->xmmubyte(n*2);
    if (shift > 0) {
      // shift left
      op1->xmm16u(n) <<= (shift & 0xf);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm16u(n) >>= (-shift & 0xf);
    }
  }
}

BX_CPP_INLINE void sse_pshld(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 4;n++) {
    int shift = op2->xmmsbyte(n*4);
    if (shift > 0) {
      // shift left
      op1->xmm32u(n) <<= (shift & 0x1f);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm32u(n) >>= (-shift & 0x1f);
    }
  }
}

BX_CPP_INLINE void sse_pshlq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 2;n++) {
    int shift = op2->xmmsbyte(n*8);
    if (shift > 0) {
      // shift left
      op1->xmm64u(n) <<= (shift & 0x3f);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm64u(n) >>= (-shift & 0x3f);
    }
  }
}

#endif
