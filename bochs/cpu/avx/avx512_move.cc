/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2023 Stanislav Shwartsman
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

#include "simd_int.h"

//////////////////////////
// masked register move //
//////////////////////////

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDQU8_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  avx512_write_regb_masked(i, &op, i->getVL(), BX_READ_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDQU16_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  avx512_write_regw_masked(i, &op, i->getVL(), BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  avx512_write_regd_masked(i, &op, i->getVL(), BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPD_MASK_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  avx512_write_regq_masked(i, &op, i->getVL(), BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

////////////////////////////////////////
// masked packed load/store - aligned //
////////////////////////////////////////

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_MASK_VpsWpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

  unsigned len = i->getVL(), len_in_bytes = BYTE_ELEMENTS(len);
  if (laddr & (len_in_bytes-1)) {
    BX_ERROR(("AVX masked read len=%d: #GP misaligned access", len_in_bytes));
    exception(BX_GP_EXCEPTION, 0);
  }

  BxPackedAvxRegister reg;
  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());
  avx_masked_load32(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPD_MASK_VpdWpdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

  unsigned len = i->getVL(), len_in_bytes = BYTE_ELEMENTS(len);
  if (laddr & (len_in_bytes-1)) {
    BX_ERROR(("AVX masked read len=%d: #GP misaligned access", len_in_bytes));
    exception(BX_GP_EXCEPTION, 0);
  }

  BxPackedAvxRegister reg;
  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());
  avx_masked_load64(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_MASK_WpsVpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

  unsigned len_in_bytes = BYTE_ELEMENTS(i->getVL());
  if (laddr & (len_in_bytes-1)) {
    BX_ERROR(("AVX masked write len=%d: #GP misaligned access", len_in_bytes));
    exception(BX_GP_EXCEPTION, 0);
  }

  avx_masked_store32(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_16BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPD_MASK_WpdVpdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

  unsigned len_in_bytes = BYTE_ELEMENTS(i->getVL());
  if (laddr & (len_in_bytes-1)) {
    BX_ERROR(("AVX masked write len=%d: #GP misaligned access", len_in_bytes));
    exception(BX_GP_EXCEPTION, 0);
  }

  avx_masked_store64(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_8BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

//////////////////////////////
// masked packed load/store //
//////////////////////////////

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDQU8_MASK_VdqWdqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

  BxPackedAvxRegister reg;
  Bit64u mask = BX_READ_OPMASK(i->opmask());
  avx_masked_load8(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 16)
      xmm_pblendb(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDQU16_MASK_VdqWdqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

  BxPackedAvxRegister reg;
  Bit32u mask = BX_READ_32BIT_OPMASK(i->opmask());
  avx_masked_load16(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPS_MASK_VpsWpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

  BxPackedAvxRegister reg;
  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());
  avx_masked_load32(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPD_MASK_VpdWpdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  unsigned len = i->getVL();

  BxPackedAvxRegister reg;
  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());
  avx_masked_load64(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDQU8_MASK_WdqVdqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDQU16_MASK_WdqVdqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store16(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPS_MASK_WpsVpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store32(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPD_MASK_WpdVpdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store64(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

//////////////////////////////
// masked scalar load/store //
//////////////////////////////

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSD_MASK_VsdWsdM(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  op.xmm64u(1) = 0;

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    op.xmm64u(0) = read_virtual_qword(i->seg(), eaddr);
  }
  else {
    if (! i->isZeroMasking()) {
      op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->dst());
    }
    else {
      op.xmm64u(0) = 0;
    }
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSS_MASK_VssWssM(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  op.xmm64u(1) = 0;

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    op.xmm64u(0) = (Bit64u) read_virtual_dword(i->seg(), eaddr);
  }
  else {
    if (! i->isZeroMasking()) {
      op.xmm64u(0) = (Bit64u) BX_READ_XMM_REG_LO_DWORD(i->dst());
    }
    else {
      op.xmm64u(0) = 0;
    }
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSD_MASK_WsdVsdM(bxInstruction_c *i)
{
  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    write_virtual_qword(i->seg(), eaddr, BX_READ_XMM_REG_LO_QWORD(i->src()));
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSS_MASK_WssVssM(bxInstruction_c *i)
{
  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    write_virtual_dword(i->seg(), eaddr, BX_READ_XMM_REG_LO_DWORD(i->src()));
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSD_MASK_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->src2());
  }
  else {
    if (! i->isZeroMasking()) {
      op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->dst());
    }
    else {
      op.xmm64u(0) = 0;
    }
  }
  op.xmm64u(1) = BX_READ_XMM_REG_HI_QWORD(i->src1());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSS_MASK_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    op.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->src2());
  }
  else {
    if (! i->isZeroMasking()) {
      op.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->dst());
    }
    else {
      op.xmm32u(0) = 0;
    }
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

////////////////////
// Half precision //
////////////////////

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVW_VshEwR(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  op.xmm64u(0) = (Bit64u) BX_READ_16BIT_REG(i->src());
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVW_EdVshR(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_XMM_REG_LO_WORD(i->src()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSH_VshWshM(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  op.xmm64u(0) = (Bit64u) read_virtual_word(i->seg(), eaddr);
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSH_MASK_VshWshM(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  op.xmm64u(1) = 0;

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    op.xmm64u(0) = (Bit64u) read_virtual_word(i->seg(), eaddr);
  }
  else {
    if (! i->isZeroMasking()) {
      op.xmm64u(0) = (Bit64u) BX_READ_XMM_REG_LO_WORD(i->dst());
    }
    else {
      op.xmm64u(0) = 0;
    }
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSH_VshHphWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src1());
  op.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->src2());
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSH_MASK_VshHphWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    op.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->src2());
  }
  else {
    if (! i->isZeroMasking()) {
      op.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->dst());
    }
    else {
      op.xmm16u(0) = 0;
    }
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSH_WshVshM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_word(i->seg(), eaddr, BX_READ_XMM_REG_LO_WORD(i->src()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSH_MASK_WshVshM(bxInstruction_c *i)
{
  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    write_virtual_word(i->seg(), eaddr, BX_READ_XMM_REG_LO_WORD(i->src()));
  }

  BX_NEXT_INSTR(i);
}

////////////////////////////////////
// masked store with down convert //
////////////////////////////////////

// quad-word to byte
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmmubyte(n) = (Bit8u) src.vmm64u(n);
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  dst.xmm64u(1) = 0;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.xmmubyte(n) = (Bit8u) src.vmm64u(n);
  }

  if (len == BX_VL128) dst.xmm16u(1) = 0;
  if (len != BX_VL512) dst.xmm32u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  dst.xmm64u(1) = 0;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmmubyte(n) = (Bit8u) src.vmm64u(n);
    else
      if (i->isZeroMasking()) dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm16u(1) = 0;
  if (len != BX_VL512) dst.xmm32u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmmsbyte(n) = SaturateQwordSToByteS(src.vmm64s(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  dst.xmm64u(1) = 0;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.xmmsbyte(n) = SaturateQwordSToByteS(src.vmm64s(n));
  }

  if (len == BX_VL128) dst.xmm16u(1) = 0;
  if (len != BX_VL512) dst.xmm32u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  dst.xmm64u(1) = 0;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmmsbyte(n) = SaturateQwordSToByteS(src.vmm64s(n));
    else
      if (i->isZeroMasking()) dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm16u(1) = 0;
  if (len != BX_VL512) dst.xmm32u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmmubyte(n) = SaturateQwordUToByteU(src.vmm64u(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  dst.xmm64u(1) = 0;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.xmmubyte(n) = SaturateQwordUToByteU(src.vmm64u(n));
  }

  if (len == BX_VL128) dst.xmm16u(1) = 0;
  if (len != BX_VL512) dst.xmm32u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  dst.xmm64u(1) = 0;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmmubyte(n) = SaturateQwordUToByteU(src.vmm64u(n));
    else
      if (i->isZeroMasking()) dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm16u(1) = 0;
  if (len != BX_VL512) dst.xmm32u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

// double-word to byte
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVDB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.vmmubyte(n) = (Bit8u) src.vmm32u(n);
  }

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVDB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.xmmubyte(n) = (Bit8u) src.vmm32u(n);
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVDB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmmubyte(n) = (Bit8u) src.vmm32u(n);
    else
      if (i->isZeroMasking()) dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSDB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.vmmsbyte(n) = SaturateDwordSToByteS(src.vmm32s(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSDB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.xmmsbyte(n) = SaturateDwordSToByteS(src.vmm32s(n));
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSDB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmmsbyte(n) = SaturateDwordSToByteS(src.vmm32s(n));
    else
      if (i->isZeroMasking()) dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSDB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.vmmubyte(n) = SaturateDwordUToByteU(src.vmm32u(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSDB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.xmmubyte(n) = SaturateDwordUToByteU(src.vmm32u(n));
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSDB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmmubyte(n) = SaturateDwordUToByteU(src.vmm32u(n));
    else
      if (i->isZeroMasking()) dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

// word to byte
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVWB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    dst.vmmubyte(n) = (Bit8u) src.vmm16u(n);
  }

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(WORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVWB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    dst.ymmubyte(n) = (Bit8u) src.vmm16u(n);
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVWB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  Bit32u mask = BX_READ_32BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymmubyte(n) = (Bit8u) src.vmm16u(n);
    else
      if (i->isZeroMasking()) dst.ymmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSWB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    dst.vmmsbyte(n) = SaturateWordSToByteS(src.vmm16s(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(WORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSWB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    dst.ymmsbyte(n) = SaturateWordSToByteS(src.vmm16s(n));
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSWB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  Bit32u mask = BX_READ_32BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymmsbyte(n) = SaturateWordSToByteS(src.vmm16s(n));
    else
      if (i->isZeroMasking()) dst.ymmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSWB_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    dst.vmmubyte(n) = SaturateWordUToByteU(src.vmm16u(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(WORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store8(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSWB_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    dst.ymmubyte(n) = SaturateWordUToByteU(src.vmm16u(n));
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSWB_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  Bit32u mask = BX_READ_32BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymmubyte(n) = SaturateWordUToByteU(src.vmm16u(n));
    else
      if (i->isZeroMasking()) dst.ymmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

// double-word to word
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVDW_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.vmm16u(n) = (Bit16u) src.vmm32u(n);
  }

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store16(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVDW_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.ymm16u(n) = (Bit16u) src.vmm32u(n);
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVDW_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymm16u(n) = (Bit16u) src.vmm32u(n);
    else
      if (i->isZeroMasking()) dst.ymm16u(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSDW_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.vmm16s(n) = SaturateDwordSToWordS(src.vmm32s(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store16(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSDW_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.ymm16s(n) = SaturateDwordSToWordS(src.vmm32s(n));
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSDW_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymm16s(n) = SaturateDwordSToWordS(src.vmm32s(n));
    else
      if (i->isZeroMasking()) dst.ymm16u(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSDW_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.vmm16u(n) = SaturateDwordUToWordU(src.vmm32u(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store16(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSDW_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    dst.ymm16u(n) = SaturateDwordUToWordU(src.vmm32u(n));
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSDW_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymm16u(n) = SaturateDwordUToWordU(src.vmm32u(n));
    else
      if (i->isZeroMasking()) dst.ymm16u(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

// quad-word to word
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQW_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm16u(n) = (Bit16u) src.vmm64u(n);
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store16(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQW_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.xmm16u(n) = (Bit16u) src.vmm64u(n);
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQW_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmm16u(n) = (Bit16u) src.vmm64u(n);
    else
      if (i->isZeroMasking()) dst.xmm16u(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQW_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm16s(n) = SaturateQwordSToWordS(src.vmm64s(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store16(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQW_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.xmm16s(n) = SaturateQwordSToWordS(src.vmm64s(n));
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQW_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmm16s(n) = SaturateQwordSToWordS(src.vmm64s(n));
    else
      if (i->isZeroMasking()) dst.xmm16u(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQW_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm16u(n) = SaturateQwordUToWordU(src.vmm64u(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store16(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQW_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.xmm16u(n) = SaturateQwordUToWordU(src.vmm64u(n));
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQW_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.xmm16u(n) = SaturateQwordUToWordU(src.vmm64u(n));
    else
      if (i->isZeroMasking()) dst.xmm16u(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

// quad-word to double-word
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQD_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm32u(n) = (Bit32u) src.vmm64u(n);
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store32(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQD_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.ymm32u(n) = (Bit32u) src.vmm64u(n);
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQD_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymm32u(n) = (Bit32u) src.vmm64u(n);
    else
      if (i->isZeroMasking()) dst.ymm32u(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQD_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm32s(n) = SaturateQwordSToDwordS(src.vmm64s(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store32(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQD_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.ymm32s(n) = SaturateQwordSToDwordS(src.vmm64s(n));
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSQD_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymm32s(n) = SaturateQwordSToDwordS(src.vmm64s(n));
    else
      if (i->isZeroMasking()) dst.ymm32u(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQD_MASK_WdqVdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src()), dst;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm32u(n) = SaturateQwordUToDwordU(src.vmm64u(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  opmask &= CUT_OPMASK_TO(QWORD_ELEMENTS(len));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store32(i, eaddr, &dst, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQD_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.ymm32u(n) = SaturateQwordUToDwordU(src.vmm64u(n));
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVUSQD_MASK_WdqVdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister dst = BX_READ_YMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, mask >>= 1) {
    if (mask & 0x1)
      dst.ymm32u(n) = SaturateQwordUToDwordU(src.vmm64u(n));
    else
      if (i->isZeroMasking()) dst.ymm32u(n) = 0;
  }

  if (len == BX_VL128) dst.ymm64u(1) = 0;
  if (len != BX_VL512) dst.ymm128(1).clear();

  BX_WRITE_YMM_REGZ(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

//////////////////////////
// load with up convert //
//////////////////////////

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBW_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++)
    result.vmm16s(n) = (Bit16s) op.ymmsbyte(n);

  avx512_write_regw_masked(i, &result, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBD_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32s(n) = (Bit32s) op.xmmsbyte(n);

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.xmmsbyte(n);

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXWD_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32s(n) = (Bit32s) op.ymm16s(n);

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXWQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.xmm16s(n);

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXDQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.ymm32s(n);

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBW_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++)
    result.vmm16u(n) = (Bit16u) op.ymmubyte(n);

  avx512_write_regw_masked(i, &result, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBD_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32u(n) = (Bit32u) op.xmmubyte(n);

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.xmmubyte(n);

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXWD_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32u(n) = (Bit32u) op.ymm16u(n);

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXWQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.xmm16u(n);

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXDQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.ymm32u(n);

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

#endif
