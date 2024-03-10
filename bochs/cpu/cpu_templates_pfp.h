/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2023  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_CPU_PFP_TEMPLATES_H
#define BX_CPU_PFP_TEMPLATES_H

extern softfloat_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);

template <xmm_pfp_1op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_SSE_PFP_1OP(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  (func)(&op, status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);
#endif
  BX_NEXT_INSTR(i);
}

template <xmm_pfp_2op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_SSE_PFP_2OP(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  (func)(&op1, &op2, status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG(i->dst(), op1);
#endif
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_AVX

template <xmm_pfp_1op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX_PFP_1OP(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    (func)(&op.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

template <xmm_pfp_2op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX_PFP_2OP(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    (func)(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

template <xmm_pfp_3op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX_PFP_3OP(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++)
    (func)(&op1.vmm128(n), &op2.vmm128(n), &op3.vmm128(n), status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_EVEX

#include "simd_int.h"

template <xmm_pfp_1op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_1OP_HALF(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned mask = BX_READ_32BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 8)
    (func)(&op.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), mask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

template <xmm_pfp_1op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_1OP_SINGLE(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 4)
    (func)(&op.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), mask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

template <xmm_pfp_1op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_1OP_DOUBLE(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 2)
    (func)(&op.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), mask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

template <xmm_pfp_2op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_2OP_SINGLE(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 4)
    (func)(&op1.vmm128(n), &op2.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

template <xmm_pfp_2op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_2OP_DOUBLE(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 2)
    (func)(&op1.vmm128(n), &op2.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

template <xmm_pfp_2op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_2OP_HALF(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned mask = BX_READ_32BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 8)
    (func)(&op1.vmm128(n), &op2.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

template <xmm_pfp_3op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_3OP_SINGLE(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 4)
    (func)(&op1.vmm128(n), &op2.vmm128(n), &op3.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

template <xmm_pfp_3op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_3OP_DOUBLE(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 2)
    (func)(&op1.vmm128(n), &op2.vmm128(n), &op3.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

template <xmm_pfp_3op_mask func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_MASK_PFP_3OP_HALF(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned mask = BX_READ_32BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 8)
    (func)(&op1.vmm128(n), &op2.vmm128(n), &op3.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_EVEX

#endif // BX_SUPPORT_AVX

#endif
