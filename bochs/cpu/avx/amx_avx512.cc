/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024-2026 Stanislav Shwartsman
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

#if BX_SUPPORT_AMX

#include "amx.h"
#include "cpu/decoder/ia_opcodes.h"

void BX_CPP_AttrRegparmN(3) BX_CPU_C::tilemov_row(bxInstruction_c *i, bool immediate_form, BxPackedAvxRegister *dst)
{
  unsigned tile_src = i->src1();
  check_tile(i, tile_src);

  unsigned row, row_chunk;
  if (immediate_form) {
    row = i->Ib() & 0x3f;
    row_chunk = i->Ib() >> 6;
  }
  else {
    row = (unsigned) BX_READ_16BIT_REG(i->src2());
    row_chunk = (unsigned) (BX_READ_32BIT_REG(i->src2()) >> 16);
  }

  unsigned tile_num_rows = BX_CPU_THIS_PTR amx->tile_num_rows(tile_src);
  if (row > tile_num_rows || row_chunk /* do not support multi-line rows yet */) {
    BX_ERROR(("%s: row=%d:%d out of range for tile %d", i->getIaOpcodeNameShort(), row, row_chunk, tile_src));
    exception(BX_GP_EXCEPTION, 0);
  }

  AMX::TILE *tsrc = &(BX_CPU_THIS_PTR amx->tile[tile_src]);
  *dst = tsrc->row[row];
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TILEMOVROW_VdqTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TILEMOVROW_VdqTrmIb, &dst);
  BX_WRITE_AVX_REG(i->dst(), dst)
  BX_CPU_THIS_PTR amx->restart();
}

#include "softfloat3e/include/softfloat.h"
#include "bf16.h"

extern softfloat_status_t prepare_ne_softfloat_status_helper(bool denormals_are_zeros);

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWD2PS_VpsTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWD2PS_VpsTrmIb, &dst);

  // "round to nearest even" rounding mode is used when doing each convertion below.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // converting the int32 source elements to fp32
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = i32_to_f32(dst.vmm32u(n), &status);

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWPS2PHL_VphTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWPS2PHL_VphTrmIb, &dst);

  // "round to nearest even" rounding mode is used when doing each convertion below.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // convert the fp32 source elements to fp16 and place them in low 16-bits of each dword
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = (Bit32u) f32_to_f16(dst.vmm32u(n), &status);

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWPS2PHH_VphTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWPS2PHH_VphTrmIb, &dst);

  // "round to nearest even" rounding mode is used when doing each convertion below.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  // convert the fp32 source elements to fp16 and place them in high 16-bits of each dword
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = ((Bit32u) f32_to_f16(dst.vmm32u(n), &status)) << 16;

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWPS2BF16L_VphTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWPS2BF16L_VphTrmIb, &dst);

  // convert the fp32 source elements to bf16 and place them in low 16-bits of each dword
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = (Bit32u) convert_ne_fp32_to_bfloat16(dst.vmm32u(n));

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TCVTROWPS2BF16H_VphTrm(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  tilemov_row(i, i->getIaOpcode() == BX_IA_EVEX_TCVTROWPS2BF16H_VphTrmIb, &dst);

  // convert the fp32 source elements to bf16 and place them in high 16-bits of each dword
  for (unsigned n=0;n < DWORD_ELEMENTS(BX_VL512); n++)
    dst.vmm32u(n) = ((Bit32u) convert_ne_fp32_to_bfloat16(dst.vmm32u(n))) << 16;

  BX_WRITE_AVX_REG(i->dst(), dst);
  BX_CPU_THIS_PTR amx->restart();
}

#endif // BX_SUPPORT_AMX
