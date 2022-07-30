/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2019 Stanislav Shwartsman
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

#if BX_CPU_LEVEL >= 6

//
// sha_f0(): A bit oriented logical operation that derives a new dword from three SHA1 state variables (dword).
// This function is used in SHA1 round 1 to 20 processing:
//
//     f0(B,C,D) := (B AND C) XOR ((NOT(B) AND D)
//

BX_CPP_INLINE Bit32u sha_f0(Bit32u B, Bit32u C, Bit32u D)
{
  return (B & C) ^ (~B & D);
}

//
// sha_f1(): A bit oriented logical operation that derives a new dword from three SHA1 state variables (dword).
// This function is used in SHA1 round 21 to 40 processing:
//
//     f1(B,C,D) := B XOR C XOR D
//

BX_CPP_INLINE Bit32u sha_f1(Bit32u B, Bit32u C, Bit32u D)
{
  return (B ^ C ^ D);
}

//
// sha_f2(): A bit oriented logical operation that derives a new dword from three SHA1 state variables (dword).
// This function is used in SHA1 round 41 to 60 processing:
//
//     f2(B,C,D) := (B AND C) XOR (B AND D) XOR (C AND D)
//

BX_CPP_INLINE Bit32u sha_f2(Bit32u B, Bit32u C, Bit32u D)
{
  return (B & C) ^ (B & D) ^ (C & D);
}

//
// sha_f3(): A bit oriented logical operation that derives a new dword from three SHA1 state variables (dword).
// This function is used in SHA1 round 61 to 80 processing:
//
//     f3(B,C,D) := B XOR C XOR D
//
// Yes, it is the same function as sha_f1()
//

BX_CPP_INLINE Bit32u sha_f(Bit32u B, Bit32u C, Bit32u D, unsigned index)
{
  if (index == 0)
    return sha_f0(B,C,D);
  if (index == 2)
    return sha_f2(B,C,D);

  // sha_f3() and sha_f1() are the same
  return sha_f1(B,C,D);
}

//
// sha_ch(): A bit oriented logical operation that derives a new dword from three SHA256 state variables (dword).
//
//     Ch(E,F,G) := (E AND F) XOR ((NOT E) AND G)
//
// Yes, it is the same as sha_f0()
//

#define sha_ch(E,F,G) sha_f0((E), (F), (G))

//
// sha_maj(): A bit oriented logical operation that derives a new dword from three SHA256 state variables (dword).
//
//     Maj(A,B,C) := (A AND B) XOR (A AND C) XOR (B AND C)
//
// Yes, it is the same as sha_f2()
//

#define sha_maj(A,B,C) sha_f2((A), (B), (C))

BX_CPP_INLINE Bit32u rotate_r(Bit32u val_32, unsigned count)
{
  return (val_32 >> count) | (val_32 << (32-count));
}

BX_CPP_INLINE Bit32u rotate_l(Bit32u val_32, unsigned count)
{
  return (val_32 << count) | (val_32 >> (32-count));
}

// A bit oriented logical and rotational transformation performed on a dword for SHA256
BX_CPP_INLINE Bit32u sha256_transformation_rrr(Bit32u val_32, unsigned rotate1, unsigned rotate2, unsigned rotate3)
{
  return rotate_r(val_32, rotate1) ^ rotate_r(val_32, rotate2) ^ rotate_r(val_32, rotate3);
}

BX_CPP_INLINE Bit32u sha256_transformation_rrs(Bit32u val_32, unsigned rotate1, unsigned rotate2, unsigned shr)
{
  return rotate_r(val_32, rotate1) ^ rotate_r(val_32, rotate2) ^ (val_32 >> shr);
}

/* 0F 38 C8 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHA1NEXTE_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  op2.xmm32u(3) += rotate_l(op1.xmm32u(3), 30);

  BX_WRITE_XMM_REG(i->dst(), op2);

  BX_NEXT_INSTR(i);
}

/* 0F 38 C9 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHA1MSG1_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  op1.xmm32u(3) ^= op1.xmm32u(1);
  op1.xmm32u(2) ^= op1.xmm32u(0);
  op1.xmm32u(1) ^= op2.xmm32u(3);
  op1.xmm32u(0) ^= op2.xmm32u(2);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 CA */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHA1MSG2_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  op1.xmm32u(3) = rotate_l(op1.xmm32u(3) ^ op2.xmm32u(2), 1);
  op1.xmm32u(2) = rotate_l(op1.xmm32u(2) ^ op2.xmm32u(1), 1);
  op1.xmm32u(1) = rotate_l(op1.xmm32u(1) ^ op2.xmm32u(0), 1);
  op1.xmm32u(0) = rotate_l(op1.xmm32u(0) ^ op1.xmm32u(3), 1);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 CB */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHA256RNDS2_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src()), wk = BX_READ_XMM_REG(0);

  Bit32u A[3], B[3], C[3], D[3], E[3], F[3], G[3], H[3];

  A[0] = op2.xmm32u(3);
  B[0] = op2.xmm32u(2);
  E[0] = op2.xmm32u(1);
  F[0] = op2.xmm32u(0);

  C[0] = op1.xmm32u(3);
  D[0] = op1.xmm32u(2);
  G[0] = op1.xmm32u(1);
  H[0] = op1.xmm32u(0);

  for (unsigned n=0; n < 2; n++) {
    Bit32u   tmp = sha_ch (E[n], F[n], G[n]) + sha256_transformation_rrr(E[n], 6, 11, 25) + wk.xmm32u(n) + H[n];
    A[n+1] = tmp + sha_maj(A[n], B[n], C[n]) + sha256_transformation_rrr(A[n], 2, 13, 22);
    B[n+1] = A[n];
    C[n+1] = B[n];
    D[n+1] = C[n];
    E[n+1] = tmp + D[n];
    F[n+1] = E[n];
    G[n+1] = F[n];
    H[n+1] = G[n];
  }

  op1.xmm32u(0) = F[2];
  op1.xmm32u(1) = E[2];
  op1.xmm32u(2) = B[2];
  op1.xmm32u(3) = A[2];

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 CC */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHA256MSG1_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  Bit32u op2 = BX_READ_XMM_REG_LO_DWORD(i->src());

  op1.xmm32u(0) += sha256_transformation_rrs(op1.xmm32u(1), 7, 18, 3);
  op1.xmm32u(1) += sha256_transformation_rrs(op1.xmm32u(2), 7, 18, 3);
  op1.xmm32u(2) += sha256_transformation_rrs(op1.xmm32u(3), 7, 18, 3);
  op1.xmm32u(3) += sha256_transformation_rrs(op2,           7, 18, 3);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 CD */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHA256MSG2_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  op1.xmm32u(0) += sha256_transformation_rrs(op2.xmm32u(2), 17, 19, 10);
  op1.xmm32u(1) += sha256_transformation_rrs(op2.xmm32u(3), 17, 19, 10);
  op1.xmm32u(2) += sha256_transformation_rrs(op1.xmm32u(0), 17, 19, 10);
  op1.xmm32u(3) += sha256_transformation_rrs(op1.xmm32u(1), 17, 19, 10);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 3A CC */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHA1RNDS4_VdqWdqIbR(bxInstruction_c *i)
{
  // SHA1 Constants dependent on immediate i
  static const Bit32u sha_Ki[4] = { 0x5A827999, 0x6ED9EBA1, 0X8F1BBCDC, 0xCA62C1D6 };
  
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());
  unsigned imm = i->Ib() & 0x3;
  Bit32u K = sha_Ki[imm];

  Bit32u W[4] = { op2.xmm32u(3), op2.xmm32u(2), op2.xmm32u(1), op2.xmm32u(0) };
  Bit32u A[5], B[5], C[5], D[5], E[5];

  A[0] = op1.xmm32u(3);
  B[0] = op1.xmm32u(2);
  C[0] = op1.xmm32u(1);
  D[0] = op1.xmm32u(0);
  E[0] = 0;

  for (unsigned n=0; n < 4; n++) {
    A[n+1] = sha_f(B[n], C[n], D[n], imm) + rotate_l(A[n], 5) + W[n] + E[n] + K;
    B[n+1] = A[n];
    C[n+1] = rotate_l(B[n], 30);
    D[n+1] = C[n];
    E[n+1] = D[n];
  }

  op1.xmm32u(0) = A[4];
  op1.xmm32u(1) = B[4];
  op1.xmm32u(2) = C[4];
  op1.xmm32u(3) = D[4];

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

#endif
