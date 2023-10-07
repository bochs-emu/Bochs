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

// Additional details can be found at: https://tools.ietf.org/html/draft-ribose-cfrg-sm4-10

static const Bit8u sm4_sbox[256] = {
  0xD6, 0x90, 0xE9, 0xFE, 0xCC, 0xE1, 0x3D, 0xB7, 0x16, 0xB6, 0x14, 0xC2, 0x28, 0xFB, 0x2C, 0x05,
  0x2B, 0x67, 0x9A, 0x76, 0x2A, 0xBE, 0x04, 0xC3, 0xAA, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
  0x9C, 0x42, 0x50, 0xF4, 0x91, 0xEF, 0x98, 0x7A, 0x33, 0x54, 0x0B, 0x43, 0xED, 0xCF, 0xAC, 0x62,
  0xE4, 0xB3, 0x1C, 0xA9, 0xC9, 0x08, 0xE8, 0x95, 0x80, 0xDF, 0x94, 0xFA, 0x75, 0x8F, 0x3F, 0xA6,
  0x47, 0x07, 0xA7, 0xFC, 0xF3, 0x73, 0x17, 0xBA, 0x83, 0x59, 0x3C, 0x19, 0xE6, 0x85, 0x4F, 0xA8,
  0x68, 0x6B, 0x81, 0xB2, 0x71, 0x64, 0xDA, 0x8B, 0xF8, 0xEB, 0x0F, 0x4B, 0x70, 0x56, 0x9D, 0x35,
  0x1E, 0x24, 0x0E, 0x5E, 0x63, 0x58, 0xD1, 0xA2, 0x25, 0x22, 0x7C, 0x3B, 0x01, 0x21, 0x78, 0x87,
  0xD4, 0x00, 0x46, 0x57, 0x9F, 0xD3, 0x27, 0x52, 0x4C, 0x36, 0x02, 0xE7, 0xA0, 0xC4, 0xC8, 0x9E,
  0xEA, 0xBF, 0x8A, 0xD2, 0x40, 0xC7, 0x38, 0xB5, 0xA3, 0xF7, 0xF2, 0xCE, 0xF9, 0x61, 0x15, 0xA1,
  0xE0, 0xAE, 0x5D, 0xA4, 0x9B, 0x34, 0x1A, 0x55, 0xAD, 0x93, 0x32, 0x30, 0xF5, 0x8C, 0xB1, 0xE3,
  0x1D, 0xF6, 0xE2, 0x2E, 0x82, 0x66, 0xCA, 0x60, 0xC0, 0x29, 0x23, 0xAB, 0x0D, 0x53, 0x4E, 0x6F,
  0xD5, 0xDB, 0x37, 0x45, 0xDE, 0xFD, 0x8E, 0x2F, 0x03, 0xFF, 0x6A, 0x72, 0x6D, 0x6C, 0x5B, 0x51,
  0x8D, 0x1B, 0xAF, 0x92, 0xBB, 0xDD, 0xBC, 0x7F, 0x11, 0xD9, 0x5C, 0x41, 0x1F, 0x10, 0x5A, 0xD8,
  0x0A, 0xC1, 0x31, 0x88, 0xA5, 0xCD, 0x7B, 0xBD, 0x2D, 0x74, 0xD0, 0x12, 0xB8, 0xE5, 0xB4, 0xB0,
  0x89, 0x69, 0x97, 0x4A, 0x0C, 0x96, 0x77, 0x7E, 0x65, 0xB9, 0xF1, 0x09, 0xC5, 0x6E, 0xC6, 0x84,
  0x18, 0xF0, 0x7D, 0xEC, 0x3A, 0xDC, 0x4D, 0x20, 0x79, 0xEE, 0x5F, 0x3E, 0xD7, 0xCB, 0x39, 0x48
};

BX_CPP_INLINE Bit32u lower_t(Bit32u val32)
{
  // do sbox per byte
  Bit32u tmp0 = sm4_sbox[val32 & 0xff];
  Bit32u tmp1 = sm4_sbox[(val32 >>  8) & 0xff];
  Bit32u tmp2 = sm4_sbox[(val32 >> 16) & 0xff];
  Bit32u tmp3 = sm4_sbox[(val32 >> 24) & 0xff];
  // form 32-bit value back
  return tmp0 | (tmp1 << 8) | (tmp2 << 16) | (tmp3 << 24);
}

BX_CPP_INLINE Bit32u L_KEY(Bit32u val32)
{
  return val32 ^ rol32(val32, 13) ^ rol32(val32, 23);
}

BX_CPP_INLINE Bit32u T_KEY(Bit32u val32)
{
  return L_KEY(lower_t(val32));
}

BX_CPP_INLINE Bit32u F_KEY(Bit32u x0, Bit32u x1, Bit32u x2, Bit32u x3, Bit32u round_key)
{
  return x0 ^ T_KEY(x1 ^ x2 ^ x3 ^ round_key);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSM4KEY4_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister r;
  unsigned len = i->getVL();
  r.clear();

  for (unsigned n=0; n < len; n++) {
    BxPackedXmmRegister op1 = BX_READ_AVX_REG_LANE(i->src1(), n), op2 = BX_READ_AVX_REG_LANE(i->src2(), n);

    Bit32u P[4], C[4];
    P[0] = op1.xmm32u(0);
    P[1] = op1.xmm32u(1);
    P[2] = op1.xmm32u(2);
    P[3] = op1.xmm32u(3);

    C[0] = F_KEY(P[0], P[1], P[2], P[3], op2.xmm32u(0));
    C[1] = F_KEY(P[1], P[2], P[3], C[0], op2.xmm32u(1));
    C[2] = F_KEY(P[2], P[3], C[0], C[1], op2.xmm32u(2));
    C[3] = F_KEY(P[3], C[0], C[1], C[2], op2.xmm32u(3));

    r.vmm32u(4*n + 0) = C[0];
    r.vmm32u(4*n + 1) = C[1];
    r.vmm32u(4*n + 2) = C[2];
    r.vmm32u(4*n + 3) = C[3];
  }

  BX_WRITE_AVX_REG(i->dst(), r);

  BX_NEXT_INSTR(i);
}

BX_CPP_INLINE Bit32u L_RND(Bit32u val32)
{
  Bit32u tmp = val32;
  tmp ^= rol32(val32,  2);
  tmp ^= rol32(val32, 10);
  tmp ^= rol32(val32, 18);
  tmp ^= rol32(val32, 24);
  return tmp;
}

BX_CPP_INLINE Bit32u T_RND(Bit32u val32)
{
  return L_RND(lower_t(val32));
}

BX_CPP_INLINE Bit32u F_RND(Bit32u x0, Bit32u x1, Bit32u x2, Bit32u x3, Bit32u round_key)
{
  return x0 ^ T_RND(x1 ^ x2 ^ x3 ^ round_key);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSM4RNDS4_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister r;
  unsigned len = i->getVL();
  r.clear();

  for (unsigned n=0; n < len; n++) {
    BxPackedXmmRegister op1 = BX_READ_AVX_REG_LANE(i->src1(), n), op2 = BX_READ_AVX_REG_LANE(i->src2(), n);

    Bit32u P[4], C[4];
    P[0] = op1.xmm32u(0);
    P[1] = op1.xmm32u(1);
    P[2] = op1.xmm32u(2);
    P[3] = op1.xmm32u(3);

    C[0] = F_RND(P[0], P[1], P[2], P[3], op2.xmm32u(0));
    C[1] = F_RND(P[1], P[2], P[3], C[0], op2.xmm32u(1));
    C[2] = F_RND(P[2], P[3], C[0], C[1], op2.xmm32u(2));
    C[3] = F_RND(P[3], C[0], C[1], C[2], op2.xmm32u(3));

    r.vmm32u(4*n + 0) = C[0];
    r.vmm32u(4*n + 1) = C[1];
    r.vmm32u(4*n + 2) = C[2];
    r.vmm32u(4*n + 3) = C[3];
  }

  BX_WRITE_AVX_REG(i->dst(), r);

  BX_NEXT_INSTR(i);
}

#endif
