/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2026 Stanislav Shwartsman
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

#include "simd_int.h"

//
// XMM - Byte Representation of a 128-bit AES State
//
//      F E D C B A
//      1 1 1 1 1 1
//      5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//     --+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--
//      P O N M L K J I H G F E D C B A
//
//
// XMM - Matrix Representation of a 128-bit AES State
//
// | A E I M |   | S(0,0) S(0,1) S(0,2) S(0,3) |   | S(0) S(4) S(8) S(C) |
// | B F J N | = | S(1,0) S(1,1) S(1,2) S(1,3) | = | S(1) S(5) S(9) S(D) |
// | C G K O |   | S(2,0) S(2,1) S(2,2) S(2,3) |   | S(2) S(6) S(A) S(E) |
// | D H L P |   | S(3,0) S(3,1) S(2,3) S(3,3) |   | S(3) S(7) S(B) S(F) |
//

//
// AES ShiftRows transformation
//
// | A E I M |    | A E I M |
// | B F J N | => | F J N B |
// | C G K O |    | K O C G |
// | D H L P |    | P D H L |
//

BX_CPP_INLINE void AES_ShiftRows(BxPackedXmmRegister &state)
{
  BxPackedXmmRegister tmp = state;

  state.xmmubyte(0x0) = tmp.xmmubyte(0x0); // A => A
  state.xmmubyte(0x1) = tmp.xmmubyte(0x5);
  state.xmmubyte(0x2) = tmp.xmmubyte(0xA);
  state.xmmubyte(0x3) = tmp.xmmubyte(0xF);
  state.xmmubyte(0x4) = tmp.xmmubyte(0x4); // E => E
  state.xmmubyte(0x5) = tmp.xmmubyte(0x9);
  state.xmmubyte(0x6) = tmp.xmmubyte(0xE);
  state.xmmubyte(0x7) = tmp.xmmubyte(0x3);
  state.xmmubyte(0x8) = tmp.xmmubyte(0x8); // I => I
  state.xmmubyte(0x9) = tmp.xmmubyte(0xD);
  state.xmmubyte(0xA) = tmp.xmmubyte(0x2);
  state.xmmubyte(0xB) = tmp.xmmubyte(0x7);
  state.xmmubyte(0xC) = tmp.xmmubyte(0xC); // M => M
  state.xmmubyte(0xD) = tmp.xmmubyte(0x1);
  state.xmmubyte(0xE) = tmp.xmmubyte(0x6);
  state.xmmubyte(0xF) = tmp.xmmubyte(0xB);
}

//
// AES InverseShiftRows transformation
//
// | A E I M |    | A E I M |
// | B F J N | => | N B F J |
// | C G K O |    | K O C G |
// | D H L P |    | H L P D |
//

BX_CPP_INLINE void AES_InverseShiftRows(BxPackedXmmRegister &state)
{
  BxPackedXmmRegister tmp = state;

  state.xmmubyte(0x0) = tmp.xmmubyte(0x0); // A => A
  state.xmmubyte(0x1) = tmp.xmmubyte(0xD);
  state.xmmubyte(0x2) = tmp.xmmubyte(0xA);
  state.xmmubyte(0x3) = tmp.xmmubyte(0x7);
  state.xmmubyte(0x4) = tmp.xmmubyte(0x4); // E => E
  state.xmmubyte(0x5) = tmp.xmmubyte(0x1);
  state.xmmubyte(0x6) = tmp.xmmubyte(0xE);
  state.xmmubyte(0x7) = tmp.xmmubyte(0xB);
  state.xmmubyte(0x8) = tmp.xmmubyte(0x8); // I => I
  state.xmmubyte(0x9) = tmp.xmmubyte(0x5);
  state.xmmubyte(0xA) = tmp.xmmubyte(0x2);
  state.xmmubyte(0xB) = tmp.xmmubyte(0xF);
  state.xmmubyte(0xC) = tmp.xmmubyte(0xC); // M => M
  state.xmmubyte(0xD) = tmp.xmmubyte(0x9);
  state.xmmubyte(0xE) = tmp.xmmubyte(0x6);
  state.xmmubyte(0xF) = tmp.xmmubyte(0x3);
}

static const Bit8u sbox_transformation[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
  0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
  0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
  0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
  0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
  0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
  0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
  0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
  0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
  0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
  0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
  0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
  0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
  0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
  0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
  0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
  0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const Bit8u inverse_sbox_transformation[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
  0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
  0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
  0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
  0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
  0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
  0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
  0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
  0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
  0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
  0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
  0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
  0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
  0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
  0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
  0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
  0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

BX_CPP_INLINE void AES_SubstituteBytes(BxPackedXmmRegister &state)
{
  for (int i=0; i<16; i++)
    state.xmmubyte(i) = sbox_transformation[state.xmmubyte(i)];
}

BX_CPP_INLINE void AES_InverseSubstituteBytes(BxPackedXmmRegister &state)
{
  for (int i=0; i<16; i++)
    state.xmmubyte(i) = inverse_sbox_transformation[state.xmmubyte(i)];
}

//
// MixColumns and InverseMixColumns are computed on whole columns, two columns
// in each qword of the state. Column j is dword j of the state, row r of the
// column is byte r of the dword.
//

// Galois Field GF(2^8) multiplication by 2 (modulo 0x11b) of each byte of a qword
BX_CPP_INLINE Bit64u gf_mul2_bytes(Bit64u val_64)
{
  return ((val_64 & BX_CONST64(0x7f7f7f7f7f7f7f7f)) << 1) ^ (((val_64 >> 7) & BX_CONST64(0x0101010101010101)) * 0x1b);
}

// Rotate right each dword of a qword by 'count' bits (count must be 8, 16 or 24)
BX_CPP_INLINE Bit64u ror_dwords(Bit64u val_64, unsigned count)
{
  Bit64u mask = BX_CONST64(0xffffffff) >> count;
  mask |= mask << 32;
  return ((val_64 >> count) & mask) | ((val_64 << (32 - count)) & ~mask);
}

// out[r] = 2*a[r] ^ 3*a[r+1] ^ a[r+2] ^ a[r+3] = 2*(a[r] ^ a[r+1]) ^ a[r+1] ^ a[r+2] ^ a[r+3]
BX_CPP_INLINE Bit64u AES_MixColumns2(Bit64u columns)
{
  Bit64u rot8 = ror_dwords(columns, 8);
  return gf_mul2_bytes(columns ^ rot8) ^ rot8 ^ ror_dwords(columns, 16) ^ ror_dwords(columns, 24);
}

static void AES_MixColumns(BxPackedXmmRegister &state)
{
  state.xmm64u(0) = AES_MixColumns2(state.xmm64u(0));
  state.xmm64u(1) = AES_MixColumns2(state.xmm64u(1));
}

// InvMixColumns matrix {0e,0b,0d,09} is MixColumns matrix {02,03,01,01} multiplied by {05,00,04,00},
// therefore apply a[r] = a[r] ^ 4*(a[r] ^ a[r+2]) first and then MixColumns
static void AES_InverseMixColumns(BxPackedXmmRegister &state)
{
  for (unsigned n=0; n < 2; n++) {
    Bit64u columns = state.xmm64u(n);
    columns ^= gf_mul2_bytes(gf_mul2_bytes(columns ^ ror_dwords(columns, 16)));
    state.xmm64u(n) = AES_MixColumns2(columns);
  }
}

BX_CPP_INLINE Bit32u AES_SubWord(Bit32u x)
{
  Bit8u b0 = sbox_transformation[(x)     & 0xff];
  Bit8u b1 = sbox_transformation[(x>>8)  & 0xff];
  Bit8u b2 = sbox_transformation[(x>>16) & 0xff];
  Bit8u b3 = sbox_transformation[(x>>24) & 0xff];

  return b0 | ((Bit32u)(b1) <<  8) |
              ((Bit32u)(b2) << 16) | ((Bit32u)(b3) << 24);
}

BX_CPP_INLINE Bit32u AES_RotWord(Bit32u x)
{
  return (x >> 8) | (x << 24);
}

/* 66 0F 38 DB */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESIMC_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  AES_InverseMixColumns(op);

  BX_WRITE_XMM_REGZ(i->dst(), op, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 DC */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESENC_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  AES_ShiftRows(op1);
  AES_SubstituteBytes(op1);
  AES_MixColumns(op1);

  xmm_xorps(&op1, &op2);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_AVX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VAESENC_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    AES_ShiftRows(op1.vmm128(n));
    AES_SubstituteBytes(op1.vmm128(n));
    AES_MixColumns(op1.vmm128(n));

    xmm_xorps(&op1.vmm128(n), &op2.vmm128(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}
#endif

/* 66 0F 38 DD */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESENCLAST_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  AES_ShiftRows(op1);
  AES_SubstituteBytes(op1);

  xmm_xorps(&op1, &op2);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_AVX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VAESENCLAST_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    AES_ShiftRows(op1.vmm128(n));
    AES_SubstituteBytes(op1.vmm128(n));

    xmm_xorps(&op1.vmm128(n), &op2.vmm128(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}
#endif

/* 66 0F 38 DE */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESDEC_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  AES_InverseShiftRows(op1);
  AES_InverseSubstituteBytes(op1);
  AES_InverseMixColumns(op1);

  xmm_xorps(&op1, &op2);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_AVX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VAESDEC_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    AES_InverseShiftRows(op1.vmm128(n));
    AES_InverseSubstituteBytes(op1.vmm128(n));
    AES_InverseMixColumns(op1.vmm128(n));

    xmm_xorps(&op1.vmm128(n), &op2.vmm128(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}
#endif

/* 66 0F 38 DF */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESDECLAST_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  AES_InverseShiftRows(op1);
  AES_InverseSubstituteBytes(op1);

  xmm_xorps(&op1, &op2);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_AVX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VAESDECLAST_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    AES_InverseShiftRows(op1.vmm128(n));
    AES_InverseSubstituteBytes(op1.vmm128(n));

    xmm_xorps(&op1.vmm128(n), &op2.vmm128(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}
#endif

/* 66 0F 3A DF */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AESKEYGENASSIST_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  Bit32u rcon32 = i->Ib();

  result.xmm32u(0) = AES_SubWord(op.xmm32u(1));
  result.xmm32u(1) = AES_RotWord(result.xmm32u(0)) ^ rcon32;
  result.xmm32u(2) = AES_SubWord(op.xmm32u(3));
  result.xmm32u(3) = AES_RotWord(result.xmm32u(2)) ^ rcon32;

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

// Carry-less multiplication of two 64-bit values processing 4 bits of b at once:
// precompute carry-less products of a with all 16 possible nibbles, then walk b
// from the most significant nibble, shift the result by 4 and add (XOR) table entry.
BX_CPP_INLINE void xmm_pclmulqdq(BxPackedXmmRegister *r, Bit64u a, Bit64u b)
{
  // the products are up to 67 bits wide, keep them as low qword and high (3 bits) part
  Bit64u table_lo[16], table_hi[16];

  table_lo[0] = table_hi[0] = 0;
  for (unsigned bit=0; bit < 4; bit++) {
    Bit64u a_shifted_lo = a << bit;
    Bit64u a_shifted_hi = bit ? (a >> (64 - bit)) : 0;
    unsigned step = 1 << bit;
    for (unsigned v=0; v < step; v++) {
      table_lo[step + v] = table_lo[v] ^ a_shifted_lo;
      table_hi[step + v] = table_hi[v] ^ a_shifted_hi;
    }
  }

  Bit64u result_lo = 0, result_hi = 0;
  for (int n=60; n >= 0; n-=4) {
    unsigned nibble = (unsigned)(b >> n) & 0xf;
    result_hi = ((result_hi << 4) | (result_lo >> 60)) ^ table_hi[nibble];
    result_lo = (result_lo << 4) ^ table_lo[nibble];
  }

  r->xmm64u(0) = result_lo;
  r->xmm64u(1) = result_hi;
}

/* 66 0F 3A 44 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCLMULQDQ_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister r;
  Bit8u imm8 = i->Ib();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  // Perform Carry Less Multiplication [R = A CLMUL B]
  // A determined by op1[imm8[0]]
  // B determined by op2[imm8[4]]
  xmm_pclmulqdq(&r, op1.xmm64u(imm8 & 1), op2.xmm64u((imm8 >> 4) & 1));

  BX_WRITE_XMM_REG(i->dst(), r);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_AVX
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCLMULQDQ_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister r;
  unsigned len = i->getVL();
  Bit8u imm8 = i->Ib();

  r.clear();

  for (unsigned n=0; n < len; n++) {
    BxPackedXmmRegister op1 = BX_READ_AVX_REG_LANE(i->src1(), n), op2 = BX_READ_AVX_REG_LANE(i->src2(), n);

    // Perform Carry Less Multiplication [R = A CLMUL B]
    // A determined by op1[imm8[0]]
    // B determined by op2[imm8[4]]
    xmm_pclmulqdq(&r.vmm128(n), op1.xmm64u(imm8 & 1), op2.xmm64u((imm8 >> 4) & 1));
  }

  BX_WRITE_AVX_REG(i->dst(), r);

  BX_NEXT_INSTR(i);
}
#endif

#endif
