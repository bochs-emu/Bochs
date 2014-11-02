/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2002-2014 Stanislav Shwartsman
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

#if BX_CPU_LEVEL >= 5

void BX_CPU_C::print_state_MMX(void)
{
  for(int i=0;i<8;i++) {
    BxPackedMmxRegister mm = BX_READ_MMX_REG(i);
    BX_DEBUG(("MM%d: %08x%08x", i, MMXUD1(mm), MMXUD0(mm)));
  }
}

#endif

#if BX_CPU_LEVEL >= 6

/* 0F 38 00 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFB_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  for(unsigned j=0; j<8; j++)
  {
    unsigned mask = op2.mmxubyte(j);
    if (mask & 0x80)
      result.mmxubyte(j) = 0;
    else
      result.mmxubyte(j) = op1.mmxubyte(mask & 0x7);
  }

  BX_WRITE_MMX_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
}

/* 0F 38 01 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PHADDW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) = MMXUW0(op1) + MMXUW1(op1);
  MMXUW1(op1) = MMXUW2(op1) + MMXUW3(op1);
  MMXUW2(op1) = MMXUW0(op2) + MMXUW1(op2);
  MMXUW3(op1) = MMXUW2(op2) + MMXUW3(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 02 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PHADDD_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = MMXUD0(op1) + MMXUD1(op1);
  MMXUD1(op1) = MMXUD0(op2) + MMXUD1(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 03 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PHADDSW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXSW0(op1) = SaturateDwordSToWordS(Bit32s(MMXSW0(op1)) + Bit32s(MMXSW1(op1)));
  MMXSW1(op1) = SaturateDwordSToWordS(Bit32s(MMXSW2(op1)) + Bit32s(MMXSW3(op1)));
  MMXSW2(op1) = SaturateDwordSToWordS(Bit32s(MMXSW0(op2)) + Bit32s(MMXSW1(op2)));
  MMXSW3(op1) = SaturateDwordSToWordS(Bit32s(MMXSW2(op2)) + Bit32s(MMXSW3(op2)));

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 04 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMADDUBSW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  for(unsigned j=0; j<4; j++)
  {
    Bit32s temp = Bit32s(op1.mmxubyte(j*2+0))*Bit32s(op2.mmxsbyte(j*2)) +
                  Bit32s(op1.mmxubyte(j*2+1))*Bit32s(op2.mmxsbyte(j*2+1));

    op1.mmx16s(j) = SaturateDwordSToWordS(temp);
  }

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 05 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PHSUBSW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXSW0(op1) = SaturateDwordSToWordS(Bit32s(MMXSW0(op1)) - Bit32s(MMXSW1(op1)));
  MMXSW1(op1) = SaturateDwordSToWordS(Bit32s(MMXSW2(op1)) - Bit32s(MMXSW3(op1)));
  MMXSW2(op1) = SaturateDwordSToWordS(Bit32s(MMXSW0(op2)) - Bit32s(MMXSW1(op2)));
  MMXSW3(op1) = SaturateDwordSToWordS(Bit32s(MMXSW2(op2)) - Bit32s(MMXSW3(op2)));

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 05 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PHSUBW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) = MMXUW0(op1) - MMXUW1(op1);
  MMXUW1(op1) = MMXUW2(op1) - MMXUW3(op1);
  MMXUW2(op1) = MMXUW0(op2) - MMXUW1(op2);
  MMXUW3(op1) = MMXUW2(op2) - MMXUW3(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 06 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PHSUBD_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = MMXUD0(op1) - MMXUD1(op1);
  MMXUD1(op1) = MMXUD0(op2) - MMXUD1(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 08 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSIGNB_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  for(unsigned j=0; j<8; j++) {
    int sign = (op2.mmxsbyte(j) > 0) - (op2.mmxsbyte(j) < 0);
    op1.mmxsbyte(j) *= sign;
  }

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 09 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSIGNW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  for(unsigned j=0; j<4; j++) {
    int sign = (op2.mmx16s(j) > 0) - (op2.mmx16s(j) < 0);
    op1.mmx16s(j) *= sign;
  }

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 0A */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSIGND_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  int sign;

  sign = (MMXSD0(op2) > 0) - (MMXSD0(op2) < 0);
  MMXSD0(op1) *= sign;
  sign = (MMXSD1(op2) > 0) - (MMXSD1(op2) < 0);
  MMXSD1(op1) *= sign;

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 0B */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULHRSW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) = (((MMXSW0(op1) * MMXSW0(op2)) >> 14) + 1) >> 1;
  MMXUW1(op1) = (((MMXSW1(op1) * MMXSW1(op2)) >> 14) + 1) >> 1;
  MMXUW2(op1) = (((MMXSW2(op1) * MMXSW2(op2)) >> 14) + 1) >> 1;
  MMXUW3(op1) = (((MMXSW3(op1) * MMXSW3(op2)) >> 14) + 1) >> 1;

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* 0F 38 1C */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PABSB_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op;

  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if (MMXSB0(op) < 0) MMXUB0(op) = -MMXSB0(op);
  if (MMXSB1(op) < 0) MMXUB1(op) = -MMXSB1(op);
  if (MMXSB2(op) < 0) MMXUB2(op) = -MMXSB2(op);
  if (MMXSB3(op) < 0) MMXUB3(op) = -MMXSB3(op);
  if (MMXSB4(op) < 0) MMXUB4(op) = -MMXSB4(op);
  if (MMXSB5(op) < 0) MMXUB5(op) = -MMXSB5(op);
  if (MMXSB6(op) < 0) MMXUB6(op) = -MMXSB6(op);
  if (MMXSB7(op) < 0) MMXUB7(op) = -MMXSB7(op);

  BX_WRITE_MMX_REG(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* 0F 38 1D */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PABSW_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op;

  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if (MMXSW0(op) < 0) MMXUW0(op) = -MMXSW0(op);
  if (MMXSW1(op) < 0) MMXUW1(op) = -MMXSW1(op);
  if (MMXSW2(op) < 0) MMXUW2(op) = -MMXSW2(op);
  if (MMXSW3(op) < 0) MMXUW3(op) = -MMXSW3(op);

  BX_WRITE_MMX_REG(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* 0F 38 1E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PABSD_PqQq(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op;

  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if (MMXSD0(op) < 0) MMXUD0(op) = -MMXSD0(op);
  if (MMXSD1(op) < 0) MMXUD1(op) = -MMXSD1(op);

  BX_WRITE_MMX_REG(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* 0F 3A 0F */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PALIGNR_PqQqIb(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  unsigned shift = i->Ib() * 8;

  if(shift == 0)
    MMXUQ(op1) = MMXUQ(op2);
  else if(shift < 64)
    MMXUQ(op1) = (MMXUQ(op2) >> shift) | (MMXUQ(op1) << (64-shift));
  else if(shift < 128)
    MMXUQ(op1) = MMXUQ(op1) >> (shift-64);
  else
    MMXUQ(op1) = 0;

  BX_WRITE_MMX_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

#endif

/* 0F 60 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKLBW_PqQd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_dword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB7(op1) = MMXUB3(op2);
  MMXUB6(op1) = MMXUB3(op1);
  MMXUB5(op1) = MMXUB2(op2);
  MMXUB4(op1) = MMXUB2(op1);
  MMXUB3(op1) = MMXUB1(op2);
  MMXUB2(op1) = MMXUB1(op1);
  MMXUB1(op1) = MMXUB0(op2);
//MMXUB0(op1) = MMXUB0(op1);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 61 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKLWD_PqQd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_dword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW3(op1) = MMXUW1(op2);
  MMXUW2(op1) = MMXUW1(op1);
  MMXUW1(op1) = MMXUW0(op2);
//MMXUW0(op1) = MMXUW0(op1);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 62 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKLDQ_PqQd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_dword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD1(op1) = MMXUD0(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 63 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PACKSSWB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXSB0(op1) = SaturateWordSToByteS(MMXSW0(op1));
  MMXSB1(op1) = SaturateWordSToByteS(MMXSW1(op1));
  MMXSB2(op1) = SaturateWordSToByteS(MMXSW2(op1));
  MMXSB3(op1) = SaturateWordSToByteS(MMXSW3(op1));

  MMXSB4(op1) = SaturateWordSToByteS(MMXSW0(op2));
  MMXSB5(op1) = SaturateWordSToByteS(MMXSW1(op2));
  MMXSB6(op1) = SaturateWordSToByteS(MMXSW2(op2));
  MMXSB7(op1) = SaturateWordSToByteS(MMXSW3(op2));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 64 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPGTB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB0(op1) = (MMXSB0(op1) > MMXSB0(op2)) ? 0xff : 0;
  MMXUB1(op1) = (MMXSB1(op1) > MMXSB1(op2)) ? 0xff : 0;
  MMXUB2(op1) = (MMXSB2(op1) > MMXSB2(op2)) ? 0xff : 0;
  MMXUB3(op1) = (MMXSB3(op1) > MMXSB3(op2)) ? 0xff : 0;
  MMXUB4(op1) = (MMXSB4(op1) > MMXSB4(op2)) ? 0xff : 0;
  MMXUB5(op1) = (MMXSB5(op1) > MMXSB5(op2)) ? 0xff : 0;
  MMXUB6(op1) = (MMXSB6(op1) > MMXSB6(op2)) ? 0xff : 0;
  MMXUB7(op1) = (MMXSB7(op1) > MMXSB7(op2)) ? 0xff : 0;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 65 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPGTW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) = (MMXSW0(op1) > MMXSW0(op2)) ? 0xffff : 0;
  MMXUW1(op1) = (MMXSW1(op1) > MMXSW1(op2)) ? 0xffff : 0;
  MMXUW2(op1) = (MMXSW2(op1) > MMXSW2(op2)) ? 0xffff : 0;
  MMXUW3(op1) = (MMXSW3(op1) > MMXSW3(op2)) ? 0xffff : 0;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 66 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPGTD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = (MMXSD0(op1) > MMXSD0(op2)) ? 0xffffffff : 0;
  MMXUD1(op1) = (MMXSD1(op1) > MMXSD1(op2)) ? 0xffffffff : 0;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 67 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PACKUSWB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB0(op1) = SaturateWordSToByteU(MMXSW0(op1));
  MMXUB1(op1) = SaturateWordSToByteU(MMXSW1(op1));
  MMXUB2(op1) = SaturateWordSToByteU(MMXSW2(op1));
  MMXUB3(op1) = SaturateWordSToByteU(MMXSW3(op1));
  MMXUB4(op1) = SaturateWordSToByteU(MMXSW0(op2));
  MMXUB5(op1) = SaturateWordSToByteU(MMXSW1(op2));
  MMXUB6(op1) = SaturateWordSToByteU(MMXSW2(op2));
  MMXUB7(op1) = SaturateWordSToByteU(MMXSW3(op2));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 68 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKHBW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB0(op1) = MMXUB4(op1);
  MMXUB1(op1) = MMXUB4(op2);
  MMXUB2(op1) = MMXUB5(op1);
  MMXUB3(op1) = MMXUB5(op2);
  MMXUB4(op1) = MMXUB6(op1);
  MMXUB5(op1) = MMXUB6(op2);
  MMXUB6(op1) = MMXUB7(op1);
  MMXUB7(op1) = MMXUB7(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 69 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKHWD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) = MMXUW2(op1);
  MMXUW1(op1) = MMXUW2(op2);
  MMXUW2(op1) = MMXUW3(op1);
  MMXUW3(op1) = MMXUW3(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 6A */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKHDQ_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = MMXUD1(op1);
  MMXUD1(op1) = MMXUD1(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 6B */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PACKSSDW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXSW0(op1) = SaturateDwordSToWordS(MMXSD0(op1));
  MMXSW1(op1) = SaturateDwordSToWordS(MMXSD1(op1));
  MMXSW2(op1) = SaturateDwordSToWordS(MMXSD0(op2));
  MMXSW3(op1) = SaturateDwordSToWordS(MMXSD1(op2));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 6E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVD_PqEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op;
  MMXUQ(op) = (Bit64u) BX_READ_32BIT_REG(i->src());
  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVD_PqEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  MMXUQ(op) = (Bit64u) read_virtual_dword(i->seg(), eaddr);

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 6E */
#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ_PqEqR(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op;
  MMXUQ(op) = BX_READ_64BIT_REG(i->src());

  BX_WRITE_MMX_REG(i->dst(), op);

  BX_NEXT_INSTR(i);
}
#endif

/* 0F 6F */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ_PqQqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BX_WRITE_MMX_REG(i->dst(), BX_READ_MMX_REG(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ_PqQqM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 70 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFW_PqQqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op, result;
  Bit8u order = i->Ib();

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(result) = op.mmx16u((order)    & 0x3);
  MMXUW1(result) = op.mmx16u((order>>2) & 0x3);
  MMXUW2(result) = op.mmx16u((order>>4) & 0x3);
  MMXUW3(result) = op.mmx16u((order>>6) & 0x3);

  BX_WRITE_MMX_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 74 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPEQB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB0(op1) = (MMXUB0(op1) == MMXUB0(op2)) ? 0xff : 0;
  MMXUB1(op1) = (MMXUB1(op1) == MMXUB1(op2)) ? 0xff : 0;
  MMXUB2(op1) = (MMXUB2(op1) == MMXUB2(op2)) ? 0xff : 0;
  MMXUB3(op1) = (MMXUB3(op1) == MMXUB3(op2)) ? 0xff : 0;
  MMXUB4(op1) = (MMXUB4(op1) == MMXUB4(op2)) ? 0xff : 0;
  MMXUB5(op1) = (MMXUB5(op1) == MMXUB5(op2)) ? 0xff : 0;
  MMXUB6(op1) = (MMXUB6(op1) == MMXUB6(op2)) ? 0xff : 0;
  MMXUB7(op1) = (MMXUB7(op1) == MMXUB7(op2)) ? 0xff : 0;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 75 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPEQW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) = (MMXUW0(op1) == MMXUW0(op2)) ? 0xffff : 0;
  MMXUW1(op1) = (MMXUW1(op1) == MMXUW1(op2)) ? 0xffff : 0;
  MMXUW2(op1) = (MMXUW2(op1) == MMXUW2(op2)) ? 0xffff : 0;
  MMXUW3(op1) = (MMXUW3(op1) == MMXUW3(op2)) ? 0xffff : 0;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 76 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPEQD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = (MMXUD0(op1) == MMXUD0(op2)) ? 0xffffffff : 0;
  MMXUD1(op1) = (MMXUD1(op1) == MMXUD1(op2)) ? 0xffffffff : 0;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 77 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::EMMS(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  FPU_TAG_WORD  = 0xffff;
  FPU_TOS = 0;        /* reset FPU Top-Of-Stack */
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 7E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVD_EdPqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->src());
  BX_WRITE_32BIT_REGZ(i->dst(), MMXUD0(op));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVD_EdPqM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->src());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  write_virtual_dword(i->seg(), eaddr, MMXUD0(op));

  // do not cause FPU2MMX transition if memory write faults
  BX_CPU_THIS_PTR prepareFPU2MMX();
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64

/* 0F 7E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ_EqPqR(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BX_WRITE_64BIT_REG(i->dst(), BX_MMX_REG(i->src()));

  BX_NEXT_INSTR(i);
}

#endif

/* 0F 7F - MOVQ_QqPqM  */
/* 0F E7 - MOVNTQ_MqPq */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ_QqPqM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  write_virtual_qword(i->seg(), eaddr, BX_MMX_REG(i->src()));

  // do not cause FPU2MMX transition if memory write faults
  BX_CPU_THIS_PTR prepareFPU2MMX();
#endif

  BX_NEXT_INSTR(i);
}

/* 0F C4 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRW_PqEwIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst());
  Bit16u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_16BIT_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    op2 = read_virtual_word(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  op1.mmx16u(i->Ib() & 0x3) = op2;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F C5 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_GdNqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->src());
  Bit32u result = (Bit32u) op.mmx16u(i->Ib() & 0x3);

  BX_WRITE_32BIT_REGZ(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F D1 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUQ(op2) > 15) MMXUQ(op1) = 0;
  else
  {
    Bit8u shift = MMXUB0(op2);

    MMXUW0(op1) >>= shift;
    MMXUW1(op1) >>= shift;
    MMXUW2(op1) >>= shift;
    MMXUW3(op1) >>= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F D2 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUQ(op2) > 31) MMXUQ(op1) = 0;
  else
  {
    Bit8u shift = MMXUB0(op2);

    MMXUD0(op1) >>= shift;
    MMXUD1(op1) >>= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F D3 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLQ_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUQ(op2) > 63) {
    MMXUQ(op1) = 0;
  }
  else {
    MMXUQ(op1) >>= MMXUB0(op2);
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F D4 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDQ_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(op1) += MMXUQ(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F D5 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULLW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  Bit32u product1 = Bit32u(MMXUW0(op1)) * Bit32u(MMXUW0(op2));
  Bit32u product2 = Bit32u(MMXUW1(op1)) * Bit32u(MMXUW1(op2));
  Bit32u product3 = Bit32u(MMXUW2(op1)) * Bit32u(MMXUW2(op2));
  Bit32u product4 = Bit32u(MMXUW3(op1)) * Bit32u(MMXUW3(op2));

  MMXUW0(op1) = product1 & 0xffff;
  MMXUW1(op1) = product2 & 0xffff;
  MMXUW2(op1) = product3 & 0xffff;
  MMXUW3(op1) = product4 & 0xffff;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F D7 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVMSKB_GdNq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->src());
  Bit32u result = 0;

  if(MMXUB0(op) & 0x80) result |= 0x01;
  if(MMXUB1(op) & 0x80) result |= 0x02;
  if(MMXUB2(op) & 0x80) result |= 0x04;
  if(MMXUB3(op) & 0x80) result |= 0x08;
  if(MMXUB4(op) & 0x80) result |= 0x10;
  if(MMXUB5(op) & 0x80) result |= 0x20;
  if(MMXUB6(op) & 0x80) result |= 0x40;
  if(MMXUB7(op) & 0x80) result |= 0x80;

  BX_WRITE_32BIT_REGZ(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F D8 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBUSB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(result) = 0;

  if(MMXUB0(op1) > MMXUB0(op2)) MMXUB0(result) = MMXUB0(op1) - MMXUB0(op2);
  if(MMXUB1(op1) > MMXUB1(op2)) MMXUB1(result) = MMXUB1(op1) - MMXUB1(op2);
  if(MMXUB2(op1) > MMXUB2(op2)) MMXUB2(result) = MMXUB2(op1) - MMXUB2(op2);
  if(MMXUB3(op1) > MMXUB3(op2)) MMXUB3(result) = MMXUB3(op1) - MMXUB3(op2);
  if(MMXUB4(op1) > MMXUB4(op2)) MMXUB4(result) = MMXUB4(op1) - MMXUB4(op2);
  if(MMXUB5(op1) > MMXUB5(op2)) MMXUB5(result) = MMXUB5(op1) - MMXUB5(op2);
  if(MMXUB6(op1) > MMXUB6(op2)) MMXUB6(result) = MMXUB6(op1) - MMXUB6(op2);
  if(MMXUB7(op1) > MMXUB7(op2)) MMXUB7(result) = MMXUB7(op1) - MMXUB7(op2);

  BX_WRITE_MMX_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F D9 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBUSW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(result) = 0;

  if(MMXUW0(op1) > MMXUW0(op2)) MMXUW0(result) = MMXUW0(op1) - MMXUW0(op2);
  if(MMXUW1(op1) > MMXUW1(op2)) MMXUW1(result) = MMXUW1(op1) - MMXUW1(op2);
  if(MMXUW2(op1) > MMXUW2(op2)) MMXUW2(result) = MMXUW2(op1) - MMXUW2(op2);
  if(MMXUW3(op1) > MMXUW3(op2)) MMXUW3(result) = MMXUW3(op1) - MMXUW3(op2);

  BX_WRITE_MMX_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F DA */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMINUB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUB0(op2) < MMXUB0(op1)) MMXUB0(op1) = MMXUB0(op2);
  if(MMXUB1(op2) < MMXUB1(op1)) MMXUB1(op1) = MMXUB1(op2);
  if(MMXUB2(op2) < MMXUB2(op1)) MMXUB2(op1) = MMXUB2(op2);
  if(MMXUB3(op2) < MMXUB3(op1)) MMXUB3(op1) = MMXUB3(op2);
  if(MMXUB4(op2) < MMXUB4(op1)) MMXUB4(op1) = MMXUB4(op2);
  if(MMXUB5(op2) < MMXUB5(op1)) MMXUB5(op1) = MMXUB5(op2);
  if(MMXUB6(op2) < MMXUB6(op1)) MMXUB6(op1) = MMXUB6(op2);
  if(MMXUB7(op2) < MMXUB7(op1)) MMXUB7(op1) = MMXUB7(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F DB */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PAND_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(op1) &= MMXUQ(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F DC */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDUSB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB0(op1) = SaturateWordSToByteU(Bit16s(MMXUB0(op1)) + Bit16s(MMXUB0(op2)));
  MMXUB1(op1) = SaturateWordSToByteU(Bit16s(MMXUB1(op1)) + Bit16s(MMXUB1(op2)));
  MMXUB2(op1) = SaturateWordSToByteU(Bit16s(MMXUB2(op1)) + Bit16s(MMXUB2(op2)));
  MMXUB3(op1) = SaturateWordSToByteU(Bit16s(MMXUB3(op1)) + Bit16s(MMXUB3(op2)));

  MMXUB4(op1) = SaturateWordSToByteU(Bit16s(MMXUB4(op1)) + Bit16s(MMXUB4(op2)));
  MMXUB5(op1) = SaturateWordSToByteU(Bit16s(MMXUB5(op1)) + Bit16s(MMXUB5(op2)));
  MMXUB6(op1) = SaturateWordSToByteU(Bit16s(MMXUB6(op1)) + Bit16s(MMXUB6(op2)));
  MMXUB7(op1) = SaturateWordSToByteU(Bit16s(MMXUB7(op1)) + Bit16s(MMXUB7(op2)));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F DD */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDUSW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) = SaturateDwordSToWordU(Bit32s(MMXUW0(op1)) + Bit32s(MMXUW0(op2)));
  MMXUW1(op1) = SaturateDwordSToWordU(Bit32s(MMXUW1(op1)) + Bit32s(MMXUW1(op2)));
  MMXUW2(op1) = SaturateDwordSToWordU(Bit32s(MMXUW2(op1)) + Bit32s(MMXUW2(op2)));
  MMXUW3(op1) = SaturateDwordSToWordU(Bit32s(MMXUW3(op1)) + Bit32s(MMXUW3(op2)));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F DE */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMAXUB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUB0(op2) > MMXUB0(op1)) MMXUB0(op1) = MMXUB0(op2);
  if(MMXUB1(op2) > MMXUB1(op1)) MMXUB1(op1) = MMXUB1(op2);
  if(MMXUB2(op2) > MMXUB2(op1)) MMXUB2(op1) = MMXUB2(op2);
  if(MMXUB3(op2) > MMXUB3(op1)) MMXUB3(op1) = MMXUB3(op2);
  if(MMXUB4(op2) > MMXUB4(op1)) MMXUB4(op1) = MMXUB4(op2);
  if(MMXUB5(op2) > MMXUB5(op1)) MMXUB5(op1) = MMXUB5(op2);
  if(MMXUB6(op2) > MMXUB6(op1)) MMXUB6(op1) = MMXUB6(op2);
  if(MMXUB7(op2) > MMXUB7(op1)) MMXUB7(op1) = MMXUB7(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F DF */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PANDN_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(op1) = ~(MMXUQ(op1)) & MMXUQ(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F E0 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PAVGB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB0(op1) = (MMXUB0(op1) + MMXUB0(op2) + 1) >> 1;
  MMXUB1(op1) = (MMXUB1(op1) + MMXUB1(op2) + 1) >> 1;
  MMXUB2(op1) = (MMXUB2(op1) + MMXUB2(op2) + 1) >> 1;
  MMXUB3(op1) = (MMXUB3(op1) + MMXUB3(op2) + 1) >> 1;
  MMXUB4(op1) = (MMXUB4(op1) + MMXUB4(op2) + 1) >> 1;
  MMXUB5(op1) = (MMXUB5(op1) + MMXUB5(op2) + 1) >> 1;
  MMXUB6(op1) = (MMXUB6(op1) + MMXUB6(op2) + 1) >> 1;
  MMXUB7(op1) = (MMXUB7(op1) + MMXUB7(op2) + 1) >> 1;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F E1 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRAW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(!MMXUQ(op2)) {
    BX_NEXT_INSTR(i);
  }

  if(MMXUQ(op2) > 15) {
    MMXUW0(op1) = (MMXSW0(op1) < 0) ? 0xffff : 0;
    MMXUW1(op1) = (MMXSW1(op1) < 0) ? 0xffff : 0;
    MMXUW2(op1) = (MMXSW2(op1) < 0) ? 0xffff : 0;
    MMXUW3(op1) = (MMXSW3(op1) < 0) ? 0xffff : 0;
  }
  else {
    Bit8u shift = MMXUB0(op2);

    MMXUW0(op1) = (Bit16u)(MMXSW0(op1) >> shift);
    MMXUW1(op1) = (Bit16u)(MMXSW1(op1) >> shift);
    MMXUW2(op1) = (Bit16u)(MMXSW2(op1) >> shift);
    MMXUW3(op1) = (Bit16u)(MMXSW3(op1) >> shift);
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F E2 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRAD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(!MMXUQ(op2)) {
    BX_NEXT_INSTR(i);
  }

  if(MMXUQ(op2) > 31) {
    MMXUD0(op1) = (MMXSD0(op1) < 0) ? 0xffffffff : 0;
    MMXUD1(op1) = (MMXSD1(op1) < 0) ? 0xffffffff : 0;
  }
  else {
    Bit8u shift = MMXUB0(op2);

    MMXUD0(op1) = (Bit32u)(MMXSD0(op1) >> shift);
    MMXUD1(op1) = (Bit32u)(MMXSD1(op1) >> shift);
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F E3 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PAVGW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) = (MMXUW0(op1) + MMXUW0(op2) + 1) >> 1;
  MMXUW1(op1) = (MMXUW1(op1) + MMXUW1(op2) + 1) >> 1;
  MMXUW2(op1) = (MMXUW2(op1) + MMXUW2(op2) + 1) >> 1;
  MMXUW3(op1) = (MMXUW3(op1) + MMXUW3(op2) + 1) >> 1;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F E4 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULHUW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  Bit32u product1 = Bit32u(MMXUW0(op1)) * Bit32u(MMXUW0(op2));
  Bit32u product2 = Bit32u(MMXUW1(op1)) * Bit32u(MMXUW1(op2));
  Bit32u product3 = Bit32u(MMXUW2(op1)) * Bit32u(MMXUW2(op2));
  Bit32u product4 = Bit32u(MMXUW3(op1)) * Bit32u(MMXUW3(op2));

  MMXUW0(op1) = (Bit16u)(product1 >> 16);
  MMXUW1(op1) = (Bit16u)(product2 >> 16);
  MMXUW2(op1) = (Bit16u)(product3 >> 16);
  MMXUW3(op1) = (Bit16u)(product4 >> 16);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F E5 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULHW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  Bit32s product1 = Bit32s(MMXSW0(op1)) * Bit32s(MMXSW0(op2));
  Bit32s product2 = Bit32s(MMXSW1(op1)) * Bit32s(MMXSW1(op2));
  Bit32s product3 = Bit32s(MMXSW2(op1)) * Bit32s(MMXSW2(op2));
  Bit32s product4 = Bit32s(MMXSW3(op1)) * Bit32s(MMXSW3(op2));

  MMXUW0(op1) = Bit16u(product1 >> 16);
  MMXUW1(op1) = Bit16u(product2 >> 16);
  MMXUW2(op1) = Bit16u(product3 >> 16);
  MMXUW3(op1) = Bit16u(product4 >> 16);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F E8 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBSB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXSB0(op1) = SaturateWordSToByteS(Bit16s(MMXSB0(op1)) - Bit16s(MMXSB0(op2)));
  MMXSB1(op1) = SaturateWordSToByteS(Bit16s(MMXSB1(op1)) - Bit16s(MMXSB1(op2)));
  MMXSB2(op1) = SaturateWordSToByteS(Bit16s(MMXSB2(op1)) - Bit16s(MMXSB2(op2)));
  MMXSB3(op1) = SaturateWordSToByteS(Bit16s(MMXSB3(op1)) - Bit16s(MMXSB3(op2)));
  MMXSB4(op1) = SaturateWordSToByteS(Bit16s(MMXSB4(op1)) - Bit16s(MMXSB4(op2)));
  MMXSB5(op1) = SaturateWordSToByteS(Bit16s(MMXSB5(op1)) - Bit16s(MMXSB5(op2)));
  MMXSB6(op1) = SaturateWordSToByteS(Bit16s(MMXSB6(op1)) - Bit16s(MMXSB6(op2)));
  MMXSB7(op1) = SaturateWordSToByteS(Bit16s(MMXSB7(op1)) - Bit16s(MMXSB7(op2)));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F E9 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBSW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXSW0(op1) = SaturateDwordSToWordS(Bit32s(MMXSW0(op1)) - Bit32s(MMXSW0(op2)));
  MMXSW1(op1) = SaturateDwordSToWordS(Bit32s(MMXSW1(op1)) - Bit32s(MMXSW1(op2)));
  MMXSW2(op1) = SaturateDwordSToWordS(Bit32s(MMXSW2(op1)) - Bit32s(MMXSW2(op2)));
  MMXSW3(op1) = SaturateDwordSToWordS(Bit32s(MMXSW3(op1)) - Bit32s(MMXSW3(op2)));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F EA */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMINSW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXSW0(op2) < MMXSW0(op1)) MMXSW0(op1) = MMXSW0(op2);
  if(MMXSW1(op2) < MMXSW1(op1)) MMXSW1(op1) = MMXSW1(op2);
  if(MMXSW2(op2) < MMXSW2(op1)) MMXSW2(op1) = MMXSW2(op2);
  if(MMXSW3(op2) < MMXSW3(op1)) MMXSW3(op1) = MMXSW3(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F EB */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POR_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(op1) |= MMXUQ(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F EC */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDSB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXSB0(op1) = SaturateWordSToByteS(Bit16s(MMXSB0(op1)) + Bit16s(MMXSB0(op2)));
  MMXSB1(op1) = SaturateWordSToByteS(Bit16s(MMXSB1(op1)) + Bit16s(MMXSB1(op2)));
  MMXSB2(op1) = SaturateWordSToByteS(Bit16s(MMXSB2(op1)) + Bit16s(MMXSB2(op2)));
  MMXSB3(op1) = SaturateWordSToByteS(Bit16s(MMXSB3(op1)) + Bit16s(MMXSB3(op2)));
  MMXSB4(op1) = SaturateWordSToByteS(Bit16s(MMXSB4(op1)) + Bit16s(MMXSB4(op2)));
  MMXSB5(op1) = SaturateWordSToByteS(Bit16s(MMXSB5(op1)) + Bit16s(MMXSB5(op2)));
  MMXSB6(op1) = SaturateWordSToByteS(Bit16s(MMXSB6(op1)) + Bit16s(MMXSB6(op2)));
  MMXSB7(op1) = SaturateWordSToByteS(Bit16s(MMXSB7(op1)) + Bit16s(MMXSB7(op2)));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F ED */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDSW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXSW0(op1) = SaturateDwordSToWordS(Bit32s(MMXSW0(op1)) + Bit32s(MMXSW0(op2)));
  MMXSW1(op1) = SaturateDwordSToWordS(Bit32s(MMXSW1(op1)) + Bit32s(MMXSW1(op2)));
  MMXSW2(op1) = SaturateDwordSToWordS(Bit32s(MMXSW2(op1)) + Bit32s(MMXSW2(op2)));
  MMXSW3(op1) = SaturateDwordSToWordS(Bit32s(MMXSW3(op1)) + Bit32s(MMXSW3(op2)));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F EE */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMAXSW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXSW0(op2) > MMXSW0(op1)) MMXSW0(op1) = MMXSW0(op2);
  if(MMXSW1(op2) > MMXSW1(op1)) MMXSW1(op1) = MMXSW1(op2);
  if(MMXSW2(op2) > MMXSW2(op1)) MMXSW2(op1) = MMXSW2(op2);
  if(MMXSW3(op2) > MMXSW3(op1)) MMXSW3(op1) = MMXSW3(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F EF */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PXOR_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(op1) ^= MMXUQ(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F1 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUQ(op2) > 15) MMXUQ(op1) = 0;
  else
  {
    Bit8u shift = MMXUB0(op2);

    MMXUW0(op1) <<= shift;
    MMXUW1(op1) <<= shift;
    MMXUW2(op1) <<= shift;
    MMXUW3(op1) <<= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F2 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUQ(op2) > 31) MMXUQ(op1) = 0;
  else
  {
    Bit8u shift = MMXUB0(op2);

    MMXUD0(op1) <<= shift;
    MMXUD1(op1) <<= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F3 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLQ_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUQ(op2) > 63) {
    MMXUQ(op1) = 0;
  }
  else {
    MMXUQ(op1) <<= MMXUB0(op2);
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F4 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULUDQ_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(op1) = Bit64u(MMXUD0(op1)) * Bit64u(MMXUD0(op2));

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F5 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMADDWD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  if(MMXUD0(op1) == 0x80008000 && MMXUD0(op2) == 0x80008000) {
    MMXUD0(op1) = 0x80000000;
  }
  else {
    MMXUD0(op1) = Bit32s(MMXSW0(op1))*Bit32s(MMXSW0(op2)) + Bit32s(MMXSW1(op1))*Bit32s(MMXSW1(op2));
  }

  if(MMXUD1(op1) == 0x80008000 && MMXUD1(op2) == 0x80008000) {
    MMXUD1(op1) = 0x80000000;
  }
  else {
    MMXUD1(op1) = Bit32s(MMXSW2(op1))*Bit32s(MMXSW2(op2)) + Bit32s(MMXSW3(op1))*Bit32s(MMXSW3(op2));
  }

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F6 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSADBW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;
  Bit16u temp = 0;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  temp += abs(MMXUB0(op1) - MMXUB0(op2));
  temp += abs(MMXUB1(op1) - MMXUB1(op2));
  temp += abs(MMXUB2(op1) - MMXUB2(op2));
  temp += abs(MMXUB3(op1) - MMXUB3(op2));
  temp += abs(MMXUB4(op1) - MMXUB4(op2));
  temp += abs(MMXUB5(op1) - MMXUB5(op2));
  temp += abs(MMXUB6(op1) - MMXUB6(op2));
  temp += abs(MMXUB7(op1) - MMXUB7(op2));

  MMXUQ(op1) = (Bit64u) temp;

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F7 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MASKMOVQ_PqNq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  bx_address rdi = RDI & i->asize_mask();
  BxPackedMmxRegister op = BX_READ_MMX_REG(i->src1()), tmp,
    mask = BX_READ_MMX_REG(i->src2());

  /* do read-modify-write for efficiency */
  MMXUQ(tmp) = read_RMW_virtual_qword(i->seg(), rdi);

  if(!MMXUQ(mask)) {
    BX_NEXT_INSTR(i);
  }

  if(MMXUB0(mask) & 0x80) MMXUB0(tmp) = MMXUB0(op);
  if(MMXUB1(mask) & 0x80) MMXUB1(tmp) = MMXUB1(op);
  if(MMXUB2(mask) & 0x80) MMXUB2(tmp) = MMXUB2(op);
  if(MMXUB3(mask) & 0x80) MMXUB3(tmp) = MMXUB3(op);
  if(MMXUB4(mask) & 0x80) MMXUB4(tmp) = MMXUB4(op);
  if(MMXUB5(mask) & 0x80) MMXUB5(tmp) = MMXUB5(op);
  if(MMXUB6(mask) & 0x80) MMXUB6(tmp) = MMXUB6(op);
  if(MMXUB7(mask) & 0x80) MMXUB7(tmp) = MMXUB7(op);

  write_RMW_virtual_qword(MMXUQ(tmp));
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F8 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB0(op1) -= MMXUB0(op2);
  MMXUB1(op1) -= MMXUB1(op2);
  MMXUB2(op1) -= MMXUB2(op2);
  MMXUB3(op1) -= MMXUB3(op2);
  MMXUB4(op1) -= MMXUB4(op2);
  MMXUB5(op1) -= MMXUB5(op2);
  MMXUB6(op1) -= MMXUB6(op2);
  MMXUB7(op1) -= MMXUB7(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F F9 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) -= MMXUW0(op2);
  MMXUW1(op1) -= MMXUW1(op2);
  MMXUW2(op1) -= MMXUW2(op2);
  MMXUW3(op1) -= MMXUW3(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F FA */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) -= MMXUD0(op2);
  MMXUD1(op1) -= MMXUD1(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F FB */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBQ_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUQ(op1) -= MMXUQ(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F FC */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUB0(op1) += MMXUB0(op2);
  MMXUB1(op1) += MMXUB1(op2);
  MMXUB2(op1) += MMXUB2(op2);
  MMXUB3(op1) += MMXUB3(op2);
  MMXUB4(op1) += MMXUB4(op2);
  MMXUB5(op1) += MMXUB5(op2);
  MMXUB6(op1) += MMXUB6(op2);
  MMXUB7(op1) += MMXUB7(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F FD */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUW0(op1) += MMXUW0(op2);
  MMXUW1(op1) += MMXUW1(op2);
  MMXUW2(op1) += MMXUW2(op2);
  MMXUW3(op1) += MMXUW3(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F FE */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op2) = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) += MMXUD0(op2);
  MMXUD1(op1) += MMXUD1(op2);

  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 71 GrpA 010 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLW_NqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->dst());
  Bit8u shift = i->Ib();

  if(shift > 15) MMXUQ(op) = 0;
  else
  {
    MMXUW0(op) >>= shift;
    MMXUW1(op) >>= shift;
    MMXUW2(op) >>= shift;
    MMXUW3(op) >>= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 71 GrpA 100 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRAW_NqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->dst());
  Bit8u shift = i->Ib();

  if(shift == 0) {
    BX_NEXT_INSTR(i);
  }

  if(shift > 15) {
    MMXUW0(op) = (MMXSW0(op) < 0) ? 0xffff : 0;
    MMXUW1(op) = (MMXSW1(op) < 0) ? 0xffff : 0;
    MMXUW2(op) = (MMXSW2(op) < 0) ? 0xffff : 0;
    MMXUW3(op) = (MMXSW3(op) < 0) ? 0xffff : 0;
  }
  else {
    MMXUW0(op) = (Bit16u)(MMXSW0(op) >> shift);
    MMXUW1(op) = (Bit16u)(MMXSW1(op) >> shift);
    MMXUW2(op) = (Bit16u)(MMXSW2(op) >> shift);
    MMXUW3(op) = (Bit16u)(MMXSW3(op) >> shift);
  }

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 71 GrpA 110 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLW_NqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->dst());
  Bit8u shift = i->Ib();

  if(shift > 15) MMXUQ(op) = 0;
  else
  {
    MMXUW0(op) <<= shift;
    MMXUW1(op) <<= shift;
    MMXUW2(op) <<= shift;
    MMXUW3(op) <<= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 72 GrpA 010 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLD_NqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->dst());
  Bit8u shift = i->Ib();

  if(shift > 31) MMXUQ(op) = 0;
  else
  {
    MMXUD0(op) >>= shift;
    MMXUD1(op) >>= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 72 GrpA 100 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRAD_NqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->dst());
  Bit8u shift = i->Ib();

  if(shift == 0) {
    BX_NEXT_INSTR(i);
  }

  if(shift > 31) {
    MMXUD0(op) = (MMXSD0(op) < 0) ? 0xffffffff : 0;
    MMXUD1(op) = (MMXSD1(op) < 0) ? 0xffffffff : 0;
  }
  else {
    MMXUD0(op) = (Bit32u)(MMXSD0(op) >> shift);
    MMXUD1(op) = (Bit32u)(MMXSD1(op) >> shift);
  }

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 72 GrpA 110 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLD_NqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->dst());
  Bit8u shift = i->Ib();

  if(shift > 31) MMXUQ(op) = 0;
  else
  {
    MMXUD0(op) <<= shift;
    MMXUD1(op) <<= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 73 GrpA 010 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLQ_NqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->dst());
  Bit8u shift = i->Ib();

  if(shift > 63) {
    MMXUQ(op) = 0;
  }
  else {
    MMXUQ(op) >>= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 73 GrpA 110 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLQ_NqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR prepareMMX();
  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->dst());
  Bit8u shift = i->Ib();

  if(shift > 63) {
    MMXUQ(op) = 0;
  }
  else {
    MMXUQ(op) <<= shift;
  }

  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}
