/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2002-2018 Stanislav Shwartsman
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

#if BX_SUPPORT_3DNOW

BX_CPP_INLINE void prepare_softfloat_status_word(float_status_t &status, int rounding_mode)
{
  status.float_exception_flags = 0; // clear exceptions before execution
  status.float_nan_handling_mode = float_first_operand_nan;
  status.float_rounding_mode = rounding_mode;
  status.flush_underflow_to_zero = 0;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFPNACC_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 0C */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PI2FW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister result, op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  float_status_t status;
  prepare_softfloat_status_word(status, float_round_to_zero);

  MMXUD0(result) = i32_to_f32((Bit32s)(MMXSW0(op)), &status);
  MMXUD1(result) = i32_to_f32((Bit32s)(MMXSW2(op)), &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
#endif
}

/* 0F 0F /r 0D */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PI2FD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  float_status_t status;
  prepare_softfloat_status_word(status, float_round_to_zero);

  MMXUD0(op) = i32_to_f32(MMXSD0(op), &status);
  MMXUD1(op) = i32_to_f32(MMXSD1(op), &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op);

  BX_NEXT_INSTR(i);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PF2IW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 1D */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PF2ID_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  float_status_t status;
  prepare_softfloat_status_word(status, float_round_to_zero);

  MMXSD0(op) = f32_to_i32_round_to_zero(MMXUD0(op), &status);
  MMXSD1(op) = f32_to_i32_round_to_zero(MMXUD1(op), &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op);

  BX_NEXT_INSTR(i);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFNACC_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFCMPGE_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFMIN_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRCP_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRSQRT_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFSUB_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFADD_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFCMPGT_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFMAX_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRCPIT1_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRSQIT1_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFSUBR_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFACC_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFCMPEQ_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFMUL_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRCPIT2_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_PANIC(("%s: 3DNow! instruction still not implemented", i->getIaOpcodeNameShort()));

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r B7 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULHRW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  Bit32s product1 = Bit32s(MMXSW0(op1)) * Bit32s(MMXSW0(op2)) + 0x8000;
  Bit32s product2 = Bit32s(MMXSW1(op1)) * Bit32s(MMXSW1(op2)) + 0x8000;
  Bit32s product3 = Bit32s(MMXSW2(op1)) * Bit32s(MMXSW2(op2)) + 0x8000;
  Bit32s product4 = Bit32s(MMXSW3(op1)) * Bit32s(MMXSW3(op2)) + 0x8000;

  MMXUW0(op1) = Bit16u(product1 >> 16);
  MMXUW1(op1) = Bit16u(product2 >> 16);
  MMXUW2(op1) = Bit16u(product3 >> 16);
  MMXUW3(op1) = Bit16u(product4 >> 16);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
#endif
}

/* 0F 0F /r BB */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSWAPD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister result, op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(result) = MMXUD1(op);
  MMXUD1(result) = MMXUD0(op);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
#endif
}

#endif
