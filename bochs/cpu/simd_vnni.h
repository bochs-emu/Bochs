/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013 Stanislav Shwartsman
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

#ifndef BX_SIMD_VNNI_FUNCTIONS_H
#define BX_SIMD_VNNI_FUNCTIONS_H

// VNNI

BX_CPP_INLINE void xmm_pdpbusd(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1word = (Bit32u) op1->xmmubyte(n*4)   * (Bit32s) op2->xmmsbyte(n*4);
    Bit32s p2word = (Bit32u) op1->xmmubyte(n*4+1) * (Bit32s) op2->xmmsbyte(n*4+1);
    Bit32s p3word = (Bit32u) op1->xmmubyte(n*4+2) * (Bit32s) op2->xmmsbyte(n*4+2);
    Bit32s p4word = (Bit32u) op1->xmmubyte(n*4+3) * (Bit32s) op2->xmmsbyte(n*4+3);

    dst->xmm32s(n) += (p1word + p2word + p3word + p4word);
  }
}

BX_CPP_INLINE void xmm_pdpbusds(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1word = (Bit32u) op1->xmmubyte(n*4)   * (Bit32s) op2->xmmsbyte(n*4);
    Bit32s p2word = (Bit32u) op1->xmmubyte(n*4+1) * (Bit32s) op2->xmmsbyte(n*4+1);
    Bit32s p3word = (Bit32u) op1->xmmubyte(n*4+2) * (Bit32s) op2->xmmsbyte(n*4+2);
    Bit32s p4word = (Bit32u) op1->xmmubyte(n*4+3) * (Bit32s) op2->xmmsbyte(n*4+3);

    Bit64s result = (Bit64s) dst->xmm32s(n) + (p1word + p2word + p3word + p4word);
    dst->xmm32s(n) = SaturateQwordSToDwordS(result);
  }
}

BX_CPP_INLINE void xmm_pdpbssd(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1word = (Bit32s) op1->xmmsbyte(n*4)   * (Bit32s) op2->xmmsbyte(n*4);
    Bit32s p2word = (Bit32s) op1->xmmsbyte(n*4+1) * (Bit32s) op2->xmmsbyte(n*4+1);
    Bit32s p3word = (Bit32s) op1->xmmsbyte(n*4+2) * (Bit32s) op2->xmmsbyte(n*4+2);
    Bit32s p4word = (Bit32s) op1->xmmsbyte(n*4+3) * (Bit32s) op2->xmmsbyte(n*4+3);

    dst->xmm32s(n) += (p1word + p2word + p3word + p4word);
  }
}

BX_CPP_INLINE void xmm_pdpbssds(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1word = (Bit32s) op1->xmmsbyte(n*4)   * (Bit32s) op2->xmmsbyte(n*4);
    Bit32s p2word = (Bit32s) op1->xmmsbyte(n*4+1) * (Bit32s) op2->xmmsbyte(n*4+1);
    Bit32s p3word = (Bit32s) op1->xmmsbyte(n*4+2) * (Bit32s) op2->xmmsbyte(n*4+2);
    Bit32s p4word = (Bit32s) op1->xmmsbyte(n*4+3) * (Bit32s) op2->xmmsbyte(n*4+3);

    Bit64s result = (Bit64s) dst->xmm32s(n) + (p1word + p2word + p3word + p4word);
    dst->xmm32s(n) = SaturateQwordSToDwordS(result);
  }
}

BX_CPP_INLINE void xmm_pdpbsud(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1word = (Bit32s) op1->xmmsbyte(n*4)   * (Bit32u) op2->xmmubyte(n*4);
    Bit32s p2word = (Bit32s) op1->xmmsbyte(n*4+1) * (Bit32u) op2->xmmubyte(n*4+1);
    Bit32s p3word = (Bit32s) op1->xmmsbyte(n*4+2) * (Bit32u) op2->xmmubyte(n*4+2);
    Bit32s p4word = (Bit32s) op1->xmmsbyte(n*4+3) * (Bit32u) op2->xmmubyte(n*4+3);

    dst->xmm32s(n) += (p1word + p2word + p3word + p4word);
  }
}

BX_CPP_INLINE void xmm_pdpbsuds(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1word = (Bit32s) op1->xmmsbyte(n*4)   * (Bit32u) op2->xmmubyte(n*4);
    Bit32s p2word = (Bit32s) op1->xmmsbyte(n*4+1) * (Bit32u) op2->xmmubyte(n*4+1);
    Bit32s p3word = (Bit32s) op1->xmmsbyte(n*4+2) * (Bit32u) op2->xmmubyte(n*4+2);
    Bit32s p4word = (Bit32s) op1->xmmsbyte(n*4+3) * (Bit32u) op2->xmmubyte(n*4+3);

    Bit64s result = (Bit64s) dst->xmm32s(n) + (p1word + p2word + p3word + p4word);
    dst->xmm32s(n) = SaturateQwordSToDwordS(result);
  }
}

BX_CPP_INLINE void xmm_pdpbuud(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32u p1word = (Bit32u) op1->xmmubyte(n*4)   * (Bit32u) op2->xmmubyte(n*4);
    Bit32u p2word = (Bit32u) op1->xmmubyte(n*4+1) * (Bit32u) op2->xmmubyte(n*4+1);
    Bit32u p3word = (Bit32u) op1->xmmubyte(n*4+2) * (Bit32u) op2->xmmubyte(n*4+2);
    Bit32u p4word = (Bit32u) op1->xmmubyte(n*4+3) * (Bit32u) op2->xmmubyte(n*4+3);

    dst->xmm32u(n) += (p1word + p2word + p3word + p4word);
  }
}

BX_CPP_INLINE void xmm_pdpbuuds(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32u p1word = (Bit32u) op1->xmmubyte(n*4)   * (Bit32u) op2->xmmubyte(n*4);
    Bit32u p2word = (Bit32u) op1->xmmubyte(n*4+1) * (Bit32u) op2->xmmubyte(n*4+1);
    Bit32u p3word = (Bit32u) op1->xmmubyte(n*4+2) * (Bit32u) op2->xmmubyte(n*4+2);
    Bit32u p4word = (Bit32u) op1->xmmubyte(n*4+3) * (Bit32u) op2->xmmubyte(n*4+3);

    Bit64u result = (Bit64u) dst->xmm32u(n) + (p1word + p2word + p3word + p4word);
    dst->xmm32u(n) = SaturateQwordUToDwordU(result);
  }
}

BX_CPP_INLINE void xmm_pdpwssd(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1_dword = (Bit32s) op1->xmm16s(n*2)   * (Bit32s) op2->xmm16s(n*2);
    Bit32s p2_dword = (Bit32s) op1->xmm16s(n*2+1) * (Bit32s) op2->xmm16s(n*2+1);

    dst->xmm32s(n) += (p1_dword + p2_dword);
  }
}

BX_CPP_INLINE void xmm_pdpwssds(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1_dword = (Bit32s) op1->xmm16s(n*2)   * (Bit32s) op2->xmm16s(n*2);
    Bit32s p2_dword = (Bit32s) op1->xmm16s(n*2+1) * (Bit32s) op2->xmm16s(n*2+1);

    Bit64s result = (Bit64s) dst->xmm32s(n) + (p1_dword + p2_dword);
    dst->xmm32s(n) = SaturateQwordSToDwordS(result);
  }
}

BX_CPP_INLINE void xmm_pdpwsud(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1_dword = (Bit32s) op1->xmm16s(n*2)   * (Bit32u) op2->xmm16u(n*2);
    Bit32s p2_dword = (Bit32s) op1->xmm16s(n*2+1) * (Bit32u) op2->xmm16u(n*2+1);

    dst->xmm32s(n) += (p1_dword + p2_dword);
  }
}

BX_CPP_INLINE void xmm_pdpwsuds(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1_dword = (Bit32s) op1->xmm16s(n*2)   * (Bit32u) op2->xmm16u(n*2);
    Bit32s p2_dword = (Bit32s) op1->xmm16s(n*2+1) * (Bit32u) op2->xmm16u(n*2+1);

    Bit64s result = (Bit64s) dst->xmm32s(n) + (p1_dword + p2_dword);
    dst->xmm32s(n) = SaturateQwordSToDwordS(result);
  }
}

BX_CPP_INLINE void xmm_pdpwusd(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1_dword = (Bit32u) op1->xmm16u(n*2)   * (Bit32s) op2->xmm16s(n*2);
    Bit32s p2_dword = (Bit32u) op1->xmm16u(n*2+1) * (Bit32s) op2->xmm16s(n*2+1);

    dst->xmm32s(n) += (p1_dword + p2_dword);
  }
}

BX_CPP_INLINE void xmm_pdpwusds(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32s p1_dword = (Bit32u) op1->xmm16u(n*2)   * (Bit32s) op2->xmm16s(n*2);
    Bit32s p2_dword = (Bit32u) op1->xmm16u(n*2+1) * (Bit32s) op2->xmm16s(n*2+1);

    Bit64s result = (Bit64s) dst->xmm32s(n) + (p1_dword + p2_dword);
    dst->xmm32s(n) = SaturateQwordSToDwordS(result);
  }
}

BX_CPP_INLINE void xmm_pdpwuud(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32u p1_dword = (Bit32u) op1->xmm16u(n*2)   * (Bit32u) op2->xmm16u(n*2);
    Bit32u p2_dword = (Bit32u) op1->xmm16u(n*2+1) * (Bit32u) op2->xmm16u(n*2+1);

    dst->xmm32u(n) += (p1_dword + p2_dword);
  }
}

BX_CPP_INLINE void xmm_pdpwuuds(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0; n<4; n++)
  {
    Bit32u p1_dword = (Bit32u) op1->xmm16u(n*2)   * (Bit32u) op2->xmm16u(n*2);
    Bit32u p2_dword = (Bit32u) op1->xmm16u(n*2+1) * (Bit32u) op2->xmm16u(n*2+1);

    Bit64u result = (Bit64u) dst->xmm32u(n) + p1_dword + p2_dword;
    dst->xmm32u(n) = SaturateQwordUToDwordU(result);
  }
}

#endif
