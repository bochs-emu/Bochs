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

#ifndef BX_CPU_TEMPLATES_H
#define BX_CPU_TEMPLATES_H

template <simd_xmm_1op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_SSE_1OP(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* SSE instruction with one operand and non destructive source */
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  (func)(&op);
  BX_WRITE_XMM_REG(i->dst(), op);
#endif
  BX_NEXT_INSTR(i);
}

template <simd_xmm_2op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_SSE_2OP(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* SSE instruction with two operands */
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());

  (func)(&op1, &BX_XMM_REG(i->src()));

  BX_WRITE_XMM_REG(i->dst(), op1);
#endif
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_SSE_SHIFT_IMM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  (func)(&BX_XMM_REG(i->dst()), i->Ib());
#endif
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_SSE_PSHIFT(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->dst());

  (func)(&op, BX_READ_XMM_REG_LO_QWORD(i->src()));

  BX_WRITE_XMM_REG(i->dst(), op);
#endif
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_AVX

template <simd_xmm_1op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX_1OP(bxInstruction_c *i)
{
  /* SSE instruction with one operand and non destructive source */
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n));

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

template <simd_xmm_2op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX_2OP(bxInstruction_c *i)
{
  /* AVX instruction with two operands */
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  unsigned len = i->getVL(), src2 = i->src2();

  for (unsigned n=0; n < len; n++)
    (func)(&op1.vmm128(n), &BX_READ_AVX_REG_LANE(src2, n));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

template <simd_xmm_3op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX_3OP(bxInstruction_c *i)
{
  /* AVX instruction with three operands */
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL(), src1 = i->src1(), src2 = i->src2();

  for (unsigned n=0; n < len; n++)
    (func)(&dst.vmm128(n), &BX_READ_AVX_REG_LANE(src1, n), &BX_READ_AVX_REG_LANE(src2, n));

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX_SHIFT_IMM(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL(), imm = i->Ib();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n), imm);

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX_PSHIFT(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  Bit64u count = BX_READ_XMM_REG_LO_QWORD(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n), count);

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_EVEX

template <simd_xmm_2op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_2OP_QWORD_EL_MASK(bxInstruction_c *i)
{
  /* AVX-512 instruction with two src operands working on QWORD elements */
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  unsigned len = i->getVL(), src2 = i->src2();

  for (unsigned n=0; n < len; n++)
    (func)(&op1.vmm128(n), &BX_READ_AVX_REG_LANE(src2, n));

  avx512_write_regq_masked(i, &op1, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_2op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_2OP_DWORD_EL_MASK(bxInstruction_c *i)
{
  /* AVX-512 instruction with two src operands working on DWORD elements */
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  unsigned len = i->getVL(), src2 = i->src2();

  for (unsigned n=0; n < len; n++)
    (func)(&op1.vmm128(n), &BX_READ_AVX_REG_LANE(src2, n));

  avx512_write_regd_masked(i, &op1, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_2op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_2OP_WORD_EL_MASK(bxInstruction_c *i)
{
  /* AVX-512 instruction with two src operands working on WORD elements */
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  unsigned len = i->getVL(), src2 = i->src2();

  for (unsigned n=0; n < len; n++)
    (func)(&op1.vmm128(n), &BX_READ_AVX_REG_LANE(src2, n));

  avx512_write_regw_masked(i, &op1, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_2op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_2OP_BYTE_EL_MASK(bxInstruction_c *i)
{
  /* AVX-512 instruction with two src operands working on BYTE elements */
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  unsigned len = i->getVL(), src2 = i->src2();

  for (unsigned n=0; n < len; n++)
    (func)(&op1.vmm128(n), &BX_READ_AVX_REG_LANE(src2, n));

  avx512_write_regb_masked(i, &op1, len, BX_READ_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_3op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_3OP_QWORD_EL_MASK(bxInstruction_c *i)
{
  /* AVX-512 instruction with 3 operands working on QWORD elements */
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL(), src1 = i->src1(), src2 = i->src2();

  for (unsigned n=0; n < len; n++)
    (func)(&dst.vmm128(n), &BX_READ_AVX_REG_LANE(src1, n), &BX_READ_AVX_REG_LANE(src2, n));

  avx512_write_regq_masked(i, &dst, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_3op func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_3OP_DWORD_EL_MASK(bxInstruction_c *i)
{
  /* AVX-512 instruction with 3 operands working on DWORD elements */
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL(), src1 = i->src1(), src2 = i->src2();

  for (unsigned n=0; n < len; n++)
    (func)(&dst.vmm128(n), &BX_READ_AVX_REG_LANE(src1, n), &BX_READ_AVX_REG_LANE(src2, n));

  avx512_write_regd_masked(i, &dst, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_SHIFT_IMM_WORD_EL_MASK(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL(), imm = i->Ib();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n), imm);

  avx512_write_regw_masked(i, &op, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_SHIFT_IMM_DWORD_EL_MASK(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL(), imm = i->Ib();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n), imm);

  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_SHIFT_IMM_QWORD_EL_MASK(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL(), imm = i->Ib();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n), imm);

  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_PSHIFT_WORD_EL_MASK(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  Bit64u count = BX_READ_XMM_REG_LO_QWORD(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n), count);

  avx512_write_regw_masked(i, &op, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_PSHIFT_DWORD_EL_MASK(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  Bit64u count = BX_READ_XMM_REG_LO_QWORD(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n), count);

  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

template <simd_xmm_shift func>
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HANDLE_AVX512_PSHIFT_QWORD_EL_MASK(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  Bit64u count = BX_READ_XMM_REG_LO_QWORD(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    (func)(&op.vmm128(n), count);

  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_EVEX

#endif // BX_SUPPORT_AVX

#endif
