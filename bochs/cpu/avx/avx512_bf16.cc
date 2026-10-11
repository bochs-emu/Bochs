/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023-2026 Stanislav Shwartsman
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

#if BX_SUPPORT_EVEX

#include "softfloat3e/include/softfloat.h"
#include "bf16.h"
#include "simd_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNEPS2BF16_MASK_VphWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);
  dst.clear();

  Bit32u opmask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff; // mask according to fp32 source
  opmask &= CUT_OPMASK_TO(num_elements);

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmm16u(n) = convert_ne_fp32_to_bfloat16(src.vmm32u(n));
  }

  // merge masking: keep original destination elements where opmask bit is clear
  if (! i->isZeroMasking()) {
    simd_pblendw(&dst, &BX_READ_AVX_REG(i->dst()), ~opmask, num_elements);
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNE2PS2BF16_MASK_VphHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst;
  unsigned len = i->getVL();
  unsigned n=0;

  unsigned half = DWORD_ELEMENTS(len);

  // lower half of the elements from src2
  for (n=0; n < half; n++)
    dst.vmm16u(n) = convert_ne_fp32_to_bfloat16(op2.vmm32u(n));

  // upper half of the elements from src1
  for (;n < WORD_ELEMENTS(len); n++)
    dst.vmm16u(n) = convert_ne_fp32_to_bfloat16(op1.vmm32u(n - half));

  if (i->opmask()) {
    avx512_write_regw_masked(i, &dst, len, BX_READ_32BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  }

  BX_NEXT_INSTR(i);
}

extern softfloat_status_t prepare_ne_softfloat_status_helper(bool denormals_are_zeros);

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDPBF16PS_MASK_VpsHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();
  Bit32u mask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff;

  // "round to nearest even" rounding mode is used when doing each accumulation of the FMA.
  // output denormals are always flushed to zero and input denormals are always treated as zero.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  for (unsigned n=0, tmp_mask = mask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1) {
      dst.vmm32u(n) = f32_mulAdd(convert_bfloat16_to_fp32(op1.vmm32u(n) >> 16), convert_bfloat16_to_fp32(op2.vmm32u(n) >> 16), dst.vmm32u(n), 0, &status);
      dst.vmm32u(n) = f32_mulAdd(convert_bfloat16_to_fp32(op1.vmm32u(n)),       convert_bfloat16_to_fp32(op2.vmm32u(n)),       dst.vmm32u(n), 0, &status);
    }
    else if (i->isZeroMasking()) {
      dst.vmm32u(n) = 0;
    }
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

#endif
