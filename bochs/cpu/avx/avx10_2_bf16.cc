/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024 Stanislav Shwartsman
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
#include "simd_bf16.h"
#include "simd_int.h"

static BX_CPP_INLINE int bf16_fpclass(bfloat16 op, int selector)
{
  extern int fpclass(softfloat_class_t op_class, int sign, int selector);

  op = bf16_denormal_to_zero(op); // always assume DAZ

  return fpclass(bf16_class(op), bf16_sign(op), selector);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFPCLASSPBF16_MASK_KGdWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned num_elements = WORD_ELEMENTS(i->getVL());

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  Bit32u result = 0;
  int selector = i->Ib();

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask) {
      if (bf16_fpclass(op.vmm16u(n), selector)) result |= mask;
    }
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETMANTPBF16_MASK_VphWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);

  int sign_ctrl = (i->Ib() >> 2) & 0x3;
  int interv = i->Ib() & 0x3;

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm16u(n) = bf16_getMant(op.vmm16u(n), sign_ctrl, interv);
    else
      op.vmm16u(n) = 0;
  }

  if (! i->isZeroMasking()) {
    simd_pblendw(&BX_READ_AVX_REG(i->dst()), &op, opmask, num_elements);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

#include "bf16-compare.h"

/* Comparison predicate for VCMPPBF16 instruction */
bfloat16_compare_method avx_compare_bf16[16] = {
  bf16_eq_ordered,
  bf16_lt_ordered,
  bf16_le_ordered,
  bf16_unordered,
  bf16_neq_unordered,
  bf16_nlt_unordered,
  bf16_nle_unordered,
  bf16_ordered,
  bf16_eq_unordered,
  bf16_nge_unordered,
  bf16_ngt_unordered,
  bf16_false,
  bf16_neq_ordered,
  bf16_ge_ordered,
  bf16_gt_ordered,
  bf16_true,
};

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPBF16_MASK_KGdHphWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned num_elements = WORD_ELEMENTS(i->getVL());
  int ib = i->Ib() & 0xF;

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  Bit32u result = 0;

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask) {
      if (avx_compare_bf16[ib](op1.vmm16u(n), op2.vmm16u(n))) result |= mask;
    }
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCOMSBF16_VshWshR(bxInstruction_c *i)
{
  bfloat16 op1 = BX_READ_XMM_REG_LO_WORD(i->dst()), op2 = BX_READ_XMM_REG_LO_WORD(i->src());

  write_eflags_fpu_compare(bf16_compare(op1, op2));

  BX_NEXT_INSTR(i);
}

#endif
