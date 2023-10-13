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

// FP32: s|eeeeeeee|mmmmmmmmmmmmmmmmmmmmmmm
// BF16: s|eeeeeeee|mmmmmmmm
//  F16: s|eeeee|mmmmmmmmmmm

BX_CPP_INLINE float32 make_float32_from_float16(float16 op) { return op; } // TBD: NOT IMPLEMENTED YET, SUPPRESS ALL EXCEPTIONS MODE USED

#include "bf16.h"

#include "simd_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBCSTNEBF162PS_VpsWwM(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  unsigned len = i->getVL();
  dst.clear();

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  float32 op = convert_bfloat16_to_fp32(read_virtual_word(i->seg(), eaddr));

  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastw(&dst.vmm128(n), op);

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBCSTNESH2PS_VpsWshM(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  unsigned len = i->getVL();
  dst.clear();

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  float32 op = make_float32_from_float16(read_virtual_word(i->seg(), eaddr));

  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastw(&dst.vmm128(n), op);

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNEEBF162PS_VpsWbf16R(bxInstruction_c *i)
{
  assert(i->src() == BX_VECTOR_TMP_REGISTER);
  BxPackedAvxRegister reg = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    reg.vmm32u(n) = convert_bfloat16_to_fp32(reg.vmm32u(n) & 0xFFFF);

  BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNEOBF162PS_VpsWbf16R(bxInstruction_c *i)
{
  assert(i->src() == BX_VECTOR_TMP_REGISTER);
  BxPackedAvxRegister reg = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    reg.vmm32u(n) = convert_bfloat16_to_fp32(reg.vmm32u(n) >> 16);

  BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNEEPH2PS_VpsWphR(bxInstruction_c *i)
{
  assert(i->src() == BX_VECTOR_TMP_REGISTER);
  BxPackedAvxRegister reg = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    reg.vmm32u(n) = make_float32_from_float16(reg.vmm32u(n) & 0xFFFF);

  BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNEOPH2PS_VpsWphR(bxInstruction_c *i)
{
  assert(i->src() == BX_VECTOR_TMP_REGISTER);
  BxPackedAvxRegister reg = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    reg.vmm32u(n) = make_float32_from_float16(reg.vmm32u(n) >> 16);

  BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNEPS2BF16_Vbf16WpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();
  dst.clear();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    dst.vmm16u(n) = convert_ne_fp32_to_bfloat16(src.vmm32u(n));

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

#endif
