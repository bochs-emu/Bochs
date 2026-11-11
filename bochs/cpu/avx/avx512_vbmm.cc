/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2026 Stanislav Shwartsman
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

// AVX-512 BMM: Bit Matrix Multiply and Bit Reversal instructions (AMD)

#if BX_SUPPORT_EVEX

// Reverse the bit order inside every byte of a qword (8 bytes processed in parallel):
// swap adjacent bits, then adjacent bit pairs, then nibbles
BX_CPP_INLINE Bit64u reverse_bits_in_bytes(Bit64u val_64)
{
  val_64 = ((val_64 >> 1) & BX_CONST64(0x5555555555555555)) | ((val_64 & BX_CONST64(0x5555555555555555)) << 1);
  val_64 = ((val_64 >> 2) & BX_CONST64(0x3333333333333333)) | ((val_64 & BX_CONST64(0x3333333333333333)) << 2);
  val_64 = ((val_64 >> 4) & BX_CONST64(0x0F0F0F0F0F0F0F0F)) | ((val_64 & BX_CONST64(0x0F0F0F0F0F0F0F0F)) << 4);
  return val_64;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBITREV_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    op.vmm64u(n) = reverse_bits_in_bytes(op.vmm64u(n));

  if (i->opmask()) {
    avx512_write_regb_masked(i, &op, len, BX_READ_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

// Bit matrix multiply and accumulate of 16x16 bit matrices, one matrix per 256-bit lane.
// Matrix row r is word r of the lane, column c is bit c of the row:
//
//   dst[r] = dst[r] OP (OP-reduction over all k with a[r] bit k set of b[k])
//
// where OP is OR or XOR. Instead of computing it bit by bit, the 16 rows of B are split
// into 4 groups of 4 rows and for every group all 16 possible OP-combinations of its rows
// are precomputed (method of Four Russians). Each result row then needs only 4 table
// lookups indexed by the corresponding nibbles of the A row.
BX_CPP_INLINE void bmac16x16x16(BxPackedYmmRegister *dst, const BxPackedYmmRegister *a, const BxPackedYmmRegister *b, bool xor_reduction)
{
  Bit16u table[4][16];

  for (unsigned g=0; g < 4; g++) {
    table[g][0] = 0;
    for (unsigned bit=0; bit < 4; bit++) {
      Bit16u brow = b->ymm16u(g*4 + bit);
      unsigned step = 1 << bit;
      for (unsigned v=0; v < step; v++) {
        if (xor_reduction)
          table[g][step + v] = table[g][v] ^ brow;
        else
          table[g][step + v] = table[g][v] | brow;
      }
    }
  }

  for (unsigned r=0; r < 16; r++) {
    Bit16u arow = a->ymm16u(r);
    Bit16u t0 = table[0][arow & 0xf], t1 = table[1][(arow >> 4) & 0xf], t2 = table[2][(arow >> 8) & 0xf], t3 = table[3][arow >> 12];

    if (xor_reduction)
      dst->ymm16u(r) ^= t0 ^ t1 ^ t2 ^ t3;
    else
      dst->ymm16u(r) |= t0 | t1 | t2 | t3;
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBMACOR16X16X16_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned lane=0; lane < len/2; lane++)
    bmac16x16x16(&dst.vmm256(lane), &op1.vmm256(lane), &op2.vmm256(lane), false);

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBMACXOR16X16X16_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned lane=0; lane < len/2; lane++)
    bmac16x16x16(&dst.vmm256(lane), &op1.vmm256(lane), &op2.vmm256(lane), true);

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}

#endif
