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

BX_CPP_INLINE Bit64u s0(Bit64u val64)
{
  return ror64(val64, 1) ^ ror64(val64, 8) ^ (val64 >> 7);
}

BX_CPP_INLINE Bit64u s1(Bit64u val64)
{
  return ror64(val64, 19) ^ ror64(val64, 61) ^ (val64 >> 6);
}

BX_CPP_INLINE Bit64u cap_sigma0(Bit64u val64)
{
  return ror64(val64, 28) ^ ror64(val64, 34) ^ ror64(val64, 39);
}

BX_CPP_INLINE Bit64u cap_sigma1(Bit64u val64)
{
  return ror64(val64, 14) ^ ror64(val64, 18) ^ ror64(val64, 41);
}

BX_CPP_INLINE Bit64u sha_maj(Bit64u a, Bit64u b, Bit64u c)
{
  return (a & b) ^ (a & c) ^ (b & c);
}

BX_CPP_INLINE Bit64u sha_ch(Bit64u e, Bit64u f, Bit64u g)
{
  return (e & f) ^ (g & ~e);
}

// See https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf for more information on the SHA512 standard

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHA512MSG1_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  Bit64u src = BX_READ_XMM_REG_LO_QWORD(i->src());

  dst.ymm64u(0) += s0(dst.ymm64u(1));
  dst.ymm64u(1) += s0(dst.ymm64u(2));
  dst.ymm64u(2) += s0(dst.ymm64u(3));
  dst.ymm64u(3) += s0(src);

  BX_WRITE_YMM_REGZ(i->dst(), dst);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHA512MSG2_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst()), src = BX_READ_YMM_REG(i->src());

  dst.ymm64u(0) += s1(src.ymm64u(2));
  dst.ymm64u(1) += s1(src.ymm64u(3));
  dst.ymm64u(2) += s1(dst.ymm64u(0));
  dst.ymm64u(3) += s1(dst.ymm64u(1));

  BX_WRITE_YMM_REGZ(i->dst(), dst);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHA512RNDS2_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst()), op1 = BX_READ_YMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());

  Bit64u A[3], B[3], C[3], D[3], E[3], F[3], G[3], H[3];

  A[0] = op1.ymm64u(3);
  B[0] = op1.ymm64u(2);
  C[0] = dst.ymm64u(3);
  D[0] = dst.ymm64u(2);

  E[0] = op1.ymm64u(1);
  F[0] = op1.ymm64u(0);
  G[0] = dst.ymm64u(1);
  H[0] = dst.ymm64u(0);

  for (unsigned n=0; n < 2; n++) {
    A[n+1] = sha_ch(E[n], F[n], G[n]) + cap_sigma1(E[n]) + op2.xmm64u(n) /* WK[n] */ + H[n] + sha_maj(A[n], B[n], C[n]) + cap_sigma0(A[n]);
    B[n+1] = A[n];
    C[n+1] = B[n];
    D[n+1] = C[n];
    E[n+1] = sha_ch(E[n], F[n], G[n]) + cap_sigma1(E[n]) + op2.xmm64u(n) /* WK[n] */ + H[n] + D[n];
    F[n+1] = E[n];
    G[n+1] = F[n];
    H[n+1] = G[n];
  }

  dst.ymm64u(3) = A[2];
  dst.ymm64u(2) = B[2];
  dst.ymm64u(1) = E[2];
  dst.ymm64u(0) = F[2];

  BX_WRITE_YMM_REGZ(i->dst(), dst);

  BX_NEXT_INSTR(i);
}

#endif
