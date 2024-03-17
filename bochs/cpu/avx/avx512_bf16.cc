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

#if BX_SUPPORT_EVEX

#include "bf16.h"
#include "simd_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNEPS2BF16_MASK_Vbf16WpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();
  dst.clear();

  Bit32u opmask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff; // mask according to fp32 source
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmm16u(n) = convert_ne_fp32_to_bfloat16(src.vmm32u(n));
  }

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &dst.vmm128(n), opmask);
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTNE2PS2BF16_MASK_Vbf16HpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst;
  unsigned len = i->getVL();
  unsigned n=0;

  // half of the elements
  for (n=0; n < DWORD_ELEMENTS(len); n++)
    dst.vmm16u(n) = convert_ne_fp32_to_bfloat16(op2.vmm32u(n));

  // other half of the elements
  for (;n < WORD_ELEMENTS(len); n++)
    dst.vmm16u(n) = convert_ne_fp32_to_bfloat16(op1.vmm32u(n));

  if (i->opmask()) {
    avx512_write_regw_masked(i, &dst, len, BX_READ_32BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  }

  BX_NEXT_INSTR(i);
}

extern float_status_t prepare_ne_softfloat_status_helper();

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDPBF16PS_MASK_VpsHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();
  Bit32u mask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff;

  // "round to nearest even" rounding mode is used when doing each accumulation of the FMA.
  // output denormals are always flushed to zero and input denormals are always treated as zero.
  float_status_t status = prepare_ne_softfloat_status_helper();
  status.denormals_are_zeros = true;

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
