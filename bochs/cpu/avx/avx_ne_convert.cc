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
// BF16: s|eeeeeeee|mmmmmmm
//  F16: s|eeeee|mmmmmmmmmm

float_status_t prepare_ne_softfloat_status_helper()
{
  float_status_t status;

  status.float_rounding_mode = float_round_nearest_even;
  status.float_exception_flags = 0;
  status.float_exception_masks = float_all_exceptions_mask;
  status.float_suppress_exception = float_all_exceptions_mask;
  status.float_nan_handling_mode = float_first_operand_nan;
  status.flush_underflow_to_zero = true;
  // input denormals not converted to zero and handled normally
  status.denormals_are_zeros = false;

  return status;
}

float32 convert_ne_fp16_to_fp32(float16 op)
{
  static float_status_t status = prepare_ne_softfloat_status_helper();
  return f16_to_f32(op, &status);
}

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
  float32 op = convert_ne_fp16_to_fp32(read_virtual_word(i->seg(), eaddr));

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
    reg.vmm32u(n) = convert_ne_fp16_to_fp32(reg.vmm32u(n) & 0xFFFF);

  BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNEOPH2PS_VpsWphR(bxInstruction_c *i)
{
  assert(i->src() == BX_VECTOR_TMP_REGISTER);
  BxPackedAvxRegister reg = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    reg.vmm32u(n) = convert_ne_fp16_to_fp32(reg.vmm32u(n) >> 16);

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
