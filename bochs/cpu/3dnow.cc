/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2002 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <gate@fidonet.org.il>
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_3DNOW

#include "softfloat.h"

static void prepare_softfloat_status_word
	(softfloat_status_word_t &status, int rounding_mode)
{
  status.float_detect_tininess = float_tininess_before_rounding;
  status.float_exception_flags = 0; // clear exceptions before execution
  status.float_nan_handling_mode = float_first_operand_nan;
  status.float_rounding_mode = rounding_mode;
  status.flush_underflow_to_zero = 0;
}

void BX_CPU_C::PFPNACC_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFPNACC_PqQq: 3DNow! instruction still not implemented"));
}

/* 0F 0F /r 0C */
void BX_CPU_C::PI2FW_PqQq(bxInstruction_c *i)
{
  BxPackedMmxRegister result, op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  softfloat_status_word_t status_word;
  prepare_softfloat_status_word(status_word, float_round_to_zero);

  MMXUD0(result) = 
        int32_to_float32((Bit32s)(MMXSW0(op)), status_word);
  MMXUD1(result) = 
        int32_to_float32((Bit32s)(MMXSW2(op)), status_word);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
}

/* 0F 0F /r 0D */
void BX_CPU_C::PI2FD_PqQq(bxInstruction_c *i)
{
  BxPackedMmxRegister result, op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  softfloat_status_word_t status_word;
  prepare_softfloat_status_word(status_word, float_round_to_zero);

  MMXUD0(result) = 
        int32_to_float32(MMXSD0(op), status_word);
  MMXUD1(result) = 
        int32_to_float32(MMXSD1(op), status_word);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
}

void BX_CPU_C::PF2IW_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PF2IW_PqQq: 3DNow! instruction still not implemented"));
}

/* 0F 0F /r 1D */
void BX_CPU_C::PF2ID_PqQq(bxInstruction_c *i)
{
  BxPackedMmxRegister result, op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  softfloat_status_word_t status_word;
  prepare_softfloat_status_word(status_word, float_round_to_zero);

  MMXSD0(result) = 
        float32_to_int32_round_to_zero(MMXUD0(op), status_word);
  MMXSD1(result) = 
        float32_to_int32_round_to_zero(MMXUD1(op), status_word);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
}

void BX_CPU_C::PFNACC_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFNACC_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFCMPGE_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFCMPGE_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFMIN_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFMIN_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFRCP_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFRCP_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFRSQRT_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFRSQRT_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFSUB_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFSUB_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFADD_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFADD_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFCMPGT_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFCMPGT_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFMAX_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFMAX_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFRCPIT1_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFRCPIT1_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFRSQIT1_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFRSQIT1_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFSUBR_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFSUBR_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFACC_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFACC_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFCMPEQ_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFCMPEQ_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFMUL_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFMUL_PqQq: 3DNow! instruction still not implemented"));
}

void BX_CPU_C::PFRCPIT2_PqQq(bxInstruction_c *i)
{
  BX_PANIC(("PFRCPIT2_PqQq: 3DNow! instruction still not implemented"));
}

/* 0F 0F /r B7 */
void BX_CPU_C::PMULHRW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  Bit32s product1 = Bit32s(MMXSW0(op1)) * Bit32s(MMXSW0(op2)) + 0x8000;
  Bit32s product2 = Bit32s(MMXSW1(op1)) * Bit32s(MMXSW1(op2)) + 0x8000;
  Bit32s product3 = Bit32s(MMXSW2(op1)) * Bit32s(MMXSW2(op2)) + 0x8000;
  Bit32s product4 = Bit32s(MMXSW3(op1)) * Bit32s(MMXSW3(op2)) + 0x8000;

  MMXUW0(result) = Bit16u(product1 >> 16);
  MMXUW1(result) = Bit16u(product2 >> 16);
  MMXUW2(result) = Bit16u(product3 >> 16);
  MMXUW3(result) = Bit16u(product4 >> 16);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
}

/* 0F 0F /r BB */
void BX_CPU_C::PSWAPD_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister result, op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  MMXUD0(result) = MMXUD1(op);
  MMXUD1(result) = MMXUD0(op);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
}

#endif
