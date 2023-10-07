/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023 Stanislav Shwartsman
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

#include "scalar_arith.h"

BX_CPP_INLINE Bit32u SM3_P1(Bit32u v32)
{
  return v32 ^ rol32(v32, 15) ^ rol32(v32, 23);
}

BX_CPP_INLINE Bit32u SM3_P0(Bit32u v32)
{
  return v32 ^ rol32(v32, 9)  ^ rol32(v32, 17);
}

BX_CPP_INLINE Bit32u SM3_FF(Bit32u x, Bit32u y, Bit32u z, unsigned round)
{
  if (round < 16)
    return (x ^ y ^ z);
  else
    return (x & y) | (x & z) | (y & z);
}

BX_CPP_INLINE Bit32u SM3_GG(Bit32u x, Bit32u y, Bit32u z, unsigned round)
{
  if (round < 16)
    return (x ^ y ^ z);
  else
    return (x & y) | (~x & z);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSM3MSG1_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2()), dst = BX_READ_XMM_REG(i->dst());

  Bit32u tmp[4];

  tmp[0] = dst.xmm32u(0) ^ op2.xmm32u(0) ^ rol32(op1.xmm32u(0), 15);
  tmp[1] = dst.xmm32u(1) ^ op2.xmm32u(1) ^ rol32(op1.xmm32u(1), 15);
  tmp[2] = dst.xmm32u(2) ^ op2.xmm32u(2) ^ rol32(op1.xmm32u(2), 15);
  tmp[3] = dst.xmm32u(3) ^ op2.xmm32u(3);

  dst.xmm32u(0) = SM3_P1(tmp[0]);
  dst.xmm32u(1) = SM3_P1(tmp[1]);
  dst.xmm32u(2) = SM3_P1(tmp[2]);
  dst.xmm32u(3) = SM3_P1(tmp[3]);

  BX_WRITE_XMM_REGZ(i->dst(), dst, i->getVL());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSM3MSG2_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2()), dst = BX_READ_XMM_REG(i->dst());

  Bit32u tmp[4];

  tmp[0] = rol32(op1.xmm32u(0), 7) ^ op2.xmm32u(0) ^ dst.xmm32u(0);
  tmp[1] = rol32(op1.xmm32u(1), 7) ^ op2.xmm32u(1) ^ dst.xmm32u(1);
  tmp[2] = rol32(op1.xmm32u(2), 7) ^ op2.xmm32u(2) ^ dst.xmm32u(2);
  tmp[3] = rol32(op1.xmm32u(3), 7) ^ op2.xmm32u(3) ^ dst.xmm32u(3);
  tmp[3] = tmp[3] ^ rol32(tmp[0], 6) ^ rol32(tmp[0], 15) ^ rol32(tmp[0], 30);

  dst.xmm32u(0) = tmp[0];
  dst.xmm32u(1) = tmp[1];
  dst.xmm32u(2) = tmp[2];
  dst.xmm32u(3) = tmp[3];

  BX_WRITE_XMM_REGZ(i->dst(), dst, i->getVL());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSM3RNDS2_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2()), dst = BX_READ_XMM_REG(i->dst());

  Bit32u A[3], B[3], C[3], D[3], E[3], F[3], G[3], H[3];
  Bit32u W[6];

  A[0] = op1.xmm32u(3);
  B[0] = op1.xmm32u(2);
  C[0] = rol32(dst.xmm32u(3), 9);
  D[0] = rol32(dst.xmm32u(2), 9);
  E[0] = op1.xmm32u(1);
  F[0] = op1.xmm32u(0);
  G[0] = rol32(dst.xmm32u(1), 19);
  H[0] = rol32(dst.xmm32u(0), 19);

  W[0] = op2.xmm32u(0);
  W[1] = op2.xmm32u(1);
  W[4] = op2.xmm32u(2);
  W[5] = op2.xmm32u(3);

  unsigned round = i->Ib() & 0x3e; // even numbers 0..62
  Bit32u magic_const = (round < 16) ? 0x79cc4519 : 0x7a879d8a;
         magic_const = rol32(magic_const, round);

  for (unsigned i=0; i <= 1; i++) {
    Bit32u S1 = rol32((rol32(A[i], 12) + E[i] + magic_const), 7);
    Bit32u S2 = S1 ^ rol32(A[i], 12);
    Bit32u T1 = SM3_FF(A[i], B[i], C[i], round) + D[i] + S2 + (W[i] ^ W[i+4]);
    Bit32u T2 = SM3_GG(E[i], F[i], G[i], round) + H[i] + S1 +  W[i];
    D[i+1] = C[i];
    C[i+1] = rol32(B[i], 9);
    B[i+1] = A[i];
    A[i+1] = T1;
    H[i+1] = G[i];
    G[i+1] = rol32(F[i], 19);
    F[i+1] = E[i];
    E[i+1] = SM3_P0(T2);

    magic_const = rol32(magic_const, 1);
  }

  dst.xmm32u(3) = A[2];
  dst.xmm32u(2) = B[2];
  dst.xmm32u(1) = E[2];
  dst.xmm32u(0) = F[2];

  BX_WRITE_XMM_REGZ(i->dst(), dst, i->getVL());

  BX_NEXT_INSTR(i);
}

#endif
