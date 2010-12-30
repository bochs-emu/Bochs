/////////////////////////////////////////////////////////////////////////
// $Id: sse.cc,v 1.77 2010-12-30 20:35:10 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2010 Stanislav Shwartsman
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

/* ********************************************** */
/* SSE Integer Operations (128bit MMX extensions) */
/* ********************************************** */

/* 66 0F 38 00 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++)
  {
    unsigned mask = op2.xmmubyte(j);
    if (mask & 0x80)
      result.xmmubyte(j) = 0;
    else
      result.xmmubyte(j) = op1.xmmubyte(mask & 0xf);
  }

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 01 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PHADDW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm16u(0) = op1.xmm16u(0) + op1.xmm16u(1);
  result.xmm16u(1) = op1.xmm16u(2) + op1.xmm16u(3);
  result.xmm16u(2) = op1.xmm16u(4) + op1.xmm16u(5);
  result.xmm16u(3) = op1.xmm16u(6) + op1.xmm16u(7);

  result.xmm16u(4) = op2.xmm16u(0) + op2.xmm16u(1);
  result.xmm16u(5) = op2.xmm16u(2) + op2.xmm16u(3);
  result.xmm16u(6) = op2.xmm16u(4) + op2.xmm16u(5);
  result.xmm16u(7) = op2.xmm16u(6) + op2.xmm16u(7);

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 02 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PHADDD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm32u(0) = op1.xmm32u(0) + op1.xmm32u(1);
  result.xmm32u(1) = op1.xmm32u(2) + op1.xmm32u(3);
  result.xmm32u(2) = op2.xmm32u(0) + op2.xmm32u(1);
  result.xmm32u(3) = op2.xmm32u(2) + op2.xmm32u(3);

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 03 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PHADDSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(0)) + Bit32s(op1.xmm16s(1)));
  result.xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(2)) + Bit32s(op1.xmm16s(3)));
  result.xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(4)) + Bit32s(op1.xmm16s(5)));
  result.xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(6)) + Bit32s(op1.xmm16s(7)));

  result.xmm16s(4) = SaturateDwordSToWordS(Bit32s(op2.xmm16s(0)) + Bit32s(op2.xmm16s(1)));
  result.xmm16s(5) = SaturateDwordSToWordS(Bit32s(op2.xmm16s(2)) + Bit32s(op2.xmm16s(3)));
  result.xmm16s(6) = SaturateDwordSToWordS(Bit32s(op2.xmm16s(4)) + Bit32s(op2.xmm16s(5)));
  result.xmm16s(7) = SaturateDwordSToWordS(Bit32s(op2.xmm16s(6)) + Bit32s(op2.xmm16s(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 04 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMADDUBSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<8; j++)
  {
    Bit32s temp = Bit32s(op1.xmmubyte(j*2+0))*Bit32s(op2.xmmsbyte(j*2+0)) +
                  Bit32s(op1.xmmubyte(j*2+1))*Bit32s(op2.xmmsbyte(j*2+1));

    result.xmm16s(j) = SaturateDwordSToWordS(temp);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 05 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PHSUBSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(0)) - Bit32s(op1.xmm16s(1)));
  result.xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(2)) - Bit32s(op1.xmm16s(3)));
  result.xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(4)) - Bit32s(op1.xmm16s(5)));
  result.xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(6)) - Bit32s(op1.xmm16s(7)));

  result.xmm16s(4) = SaturateDwordSToWordS(Bit32s(op2.xmm16s(0)) - Bit32s(op2.xmm16s(1)));
  result.xmm16s(5) = SaturateDwordSToWordS(Bit32s(op2.xmm16s(2)) - Bit32s(op2.xmm16s(3)));
  result.xmm16s(6) = SaturateDwordSToWordS(Bit32s(op2.xmm16s(4)) - Bit32s(op2.xmm16s(5)));
  result.xmm16s(7) = SaturateDwordSToWordS(Bit32s(op2.xmm16s(6)) - Bit32s(op2.xmm16s(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 05 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PHSUBW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm16u(0) = op1.xmm16u(0) - op1.xmm16u(1);
  result.xmm16u(1) = op1.xmm16u(2) - op1.xmm16u(3);
  result.xmm16u(2) = op1.xmm16u(4) - op1.xmm16u(5);
  result.xmm16u(3) = op1.xmm16u(6) - op1.xmm16u(7);

  result.xmm16u(4) = op2.xmm16u(0) - op2.xmm16u(1);
  result.xmm16u(5) = op2.xmm16u(2) - op2.xmm16u(3);
  result.xmm16u(6) = op2.xmm16u(4) - op2.xmm16u(5);
  result.xmm16u(7) = op2.xmm16u(6) - op2.xmm16u(7);

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 06 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PHSUBD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm32u(0) = op1.xmm32u(0) - op1.xmm32u(1);
  result.xmm32u(1) = op1.xmm32u(2) - op1.xmm32u(3);
  result.xmm32u(2) = op2.xmm32u(0) - op2.xmm32u(1);
  result.xmm32u(3) = op2.xmm32u(2) - op2.xmm32u(3);

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 08 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSIGNB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    int sign = (op2.xmmsbyte(j) > 0) - (op2.xmmsbyte(j) < 0);
    op1.xmmsbyte(j) *= sign;
  }

  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 09 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSIGNW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<8; j++) {
    int sign = (op2.xmm16s(j) > 0) - (op2.xmm16s(j) < 0);
    op1.xmm16s(j) *= sign;
  }

  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 0A */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSIGND_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<4; j++) {
    int sign = (op2.xmm32s(j) > 0) - (op2.xmm32s(j) < 0);
    op1.xmm32s(j) *= sign;
  }

  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 0B */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULHRSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16u(0) = (((op1.xmm16s(0) * op2.xmm16s(0)) >> 14) + 1) >> 1;
  op1.xmm16u(1) = (((op1.xmm16s(1) * op2.xmm16s(1)) >> 14) + 1) >> 1;
  op1.xmm16u(2) = (((op1.xmm16s(2) * op2.xmm16s(2)) >> 14) + 1) >> 1;
  op1.xmm16u(3) = (((op1.xmm16s(3) * op2.xmm16s(3)) >> 14) + 1) >> 1;
  op1.xmm16u(4) = (((op1.xmm16s(4) * op2.xmm16s(4)) >> 14) + 1) >> 1;
  op1.xmm16u(5) = (((op1.xmm16s(5) * op2.xmm16s(5)) >> 14) + 1) >> 1;
  op1.xmm16u(6) = (((op1.xmm16s(6) * op2.xmm16s(6)) >> 14) + 1) >> 1;
  op1.xmm16u(7) = (((op1.xmm16s(7) * op2.xmm16s(7)) >> 14) + 1) >> 1;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 1C */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PABSB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;

  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op);
  }

  if(op.xmmsbyte(0x0) < 0) op.xmmubyte(0x0) = -op.xmmsbyte(0x0);
  if(op.xmmsbyte(0x1) < 0) op.xmmubyte(0x1) = -op.xmmsbyte(0x1);
  if(op.xmmsbyte(0x2) < 0) op.xmmubyte(0x2) = -op.xmmsbyte(0x2);
  if(op.xmmsbyte(0x3) < 0) op.xmmubyte(0x3) = -op.xmmsbyte(0x3);
  if(op.xmmsbyte(0x4) < 0) op.xmmubyte(0x4) = -op.xmmsbyte(0x4);
  if(op.xmmsbyte(0x5) < 0) op.xmmubyte(0x5) = -op.xmmsbyte(0x5);
  if(op.xmmsbyte(0x6) < 0) op.xmmubyte(0x6) = -op.xmmsbyte(0x6);
  if(op.xmmsbyte(0x7) < 0) op.xmmubyte(0x7) = -op.xmmsbyte(0x7);
  if(op.xmmsbyte(0x8) < 0) op.xmmubyte(0x8) = -op.xmmsbyte(0x8);
  if(op.xmmsbyte(0x9) < 0) op.xmmubyte(0x9) = -op.xmmsbyte(0x9);
  if(op.xmmsbyte(0xa) < 0) op.xmmubyte(0xa) = -op.xmmsbyte(0xa);
  if(op.xmmsbyte(0xb) < 0) op.xmmubyte(0xb) = -op.xmmsbyte(0xb);
  if(op.xmmsbyte(0xc) < 0) op.xmmubyte(0xc) = -op.xmmsbyte(0xc);
  if(op.xmmsbyte(0xd) < 0) op.xmmubyte(0xd) = -op.xmmsbyte(0xd);
  if(op.xmmsbyte(0xe) < 0) op.xmmubyte(0xe) = -op.xmmsbyte(0xe);
  if(op.xmmsbyte(0xf) < 0) op.xmmubyte(0xf) = -op.xmmsbyte(0xf);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
}

/* 66 0F 38 1D */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PABSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;

  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op);
  }

  if(op.xmm16s(0) < 0) op.xmm16u(0) = -op.xmm16s(0);
  if(op.xmm16s(1) < 0) op.xmm16u(1) = -op.xmm16s(1);
  if(op.xmm16s(2) < 0) op.xmm16u(2) = -op.xmm16s(2);
  if(op.xmm16s(3) < 0) op.xmm16u(3) = -op.xmm16s(3);
  if(op.xmm16s(4) < 0) op.xmm16u(4) = -op.xmm16s(4);
  if(op.xmm16s(5) < 0) op.xmm16u(5) = -op.xmm16s(5);
  if(op.xmm16s(6) < 0) op.xmm16u(6) = -op.xmm16s(6);
  if(op.xmm16s(7) < 0) op.xmm16u(7) = -op.xmm16s(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
}

/* 66 0F 38 1E */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PABSD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;

  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op);
  }

  if(op.xmm32s(0) < 0) op.xmm32u(0) = -op.xmm32s(0);
  if(op.xmm32s(1) < 0) op.xmm32u(1) = -op.xmm32s(1);
  if(op.xmm32s(2) < 0) op.xmm32u(2) = -op.xmm32s(2);
  if(op.xmm32s(3) < 0) op.xmm32u(3) = -op.xmm32s(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
}

/* 66 0F 38 10 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PBLENDVB_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm()),
           mask = BX_READ_XMM_REG(0); // XMM0

  for(unsigned j=0; j<16; j++)
     if (mask.xmmubyte(j) & 0x80) op1.xmmubyte(j) = op2.xmmubyte(j);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 14 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDVPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm()),
           mask = BX_READ_XMM_REG(0); // XMM0

  if (mask.xmm32u(0) & 0x80000000) op1.xmm32u(0) = op2.xmm32u(0);
  if (mask.xmm32u(1) & 0x80000000) op1.xmm32u(1) = op2.xmm32u(1);
  if (mask.xmm32u(2) & 0x80000000) op1.xmm32u(2) = op2.xmm32u(2);
  if (mask.xmm32u(3) & 0x80000000) op1.xmm32u(3) = op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 15 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDVPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm()),
           mask = BX_READ_XMM_REG(0); // XMM0

  if (mask.xmm32u(1) & 0x80000000) op1.xmm64u(0) = op2.xmm64u(0);
  if (mask.xmm32u(3) & 0x80000000) op1.xmm64u(1) = op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 17 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PTEST_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());
  unsigned result = 0;

  if ((op2.xmm64u(0) &  op1.xmm64u(0)) == 0 &&
      (op2.xmm64u(1) &  op1.xmm64u(1)) == 0) result |= EFlagsZFMask;

  if ((op2.xmm64u(0) & ~op1.xmm64u(0)) == 0 &&
      (op2.xmm64u(1) & ~op1.xmm64u(1)) == 0) result |= EFlagsCFMask;

  setEFlagsOSZAPC(result);
#endif
}

/* 66 0F 38 28 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULDQ_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->rm()), result;

  result.xmm64s(0) = Bit64s(op1.xmm32s(0)) * Bit64s(op2.xmm32s(0));
  result.xmm64s(1) = Bit64s(op1.xmm32s(2)) * Bit64s(op2.xmm32s(2));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 29 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPEQQ_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  op1.xmm64u(0) = (op1.xmm64u(0) == op2.xmm64u(0)) ?
        BX_CONST64(0xffffffffffffffff) : 0;

  op1.xmm64u(1) = (op1.xmm64u(1) == op2.xmm64u(1)) ?
        BX_CONST64(0xffffffffffffffff) : 0;

  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 2B */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PACKUSDW_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->rm()), result;

  result.xmm16u(0) = SaturateDwordSToWordU(op1.xmm32s(0));
  result.xmm16u(1) = SaturateDwordSToWordU(op1.xmm32s(1));
  result.xmm16u(2) = SaturateDwordSToWordU(op1.xmm32s(2));
  result.xmm16u(3) = SaturateDwordSToWordU(op1.xmm32s(3));
  result.xmm16u(4) = SaturateDwordSToWordU(op2.xmm32s(0));
  result.xmm16u(5) = SaturateDwordSToWordU(op2.xmm32s(1));
  result.xmm16u(6) = SaturateDwordSToWordU(op2.xmm32s(2));
  result.xmm16u(7) = SaturateDwordSToWordU(op2.xmm32s(3));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 38 37 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPGTQ_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  op1.xmm64u(0) = (op1.xmm64u(0) > op2.xmm64u(0)) ?
        BX_CONST64(0xffffffffffffffff) : 0;

  op1.xmm64u(1) = (op1.xmm64u(1) > op2.xmm64u(1)) ?
        BX_CONST64(0xffffffffffffffff) : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 38 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMINSB_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  for(unsigned j=0; j<16; j++) {
    if(op2.xmmsbyte(j) < op1.xmmsbyte(j)) op1.xmmubyte(j) = op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 39 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMINSD_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  if(op2.xmm32s(0) < op1.xmm32s(0)) op1.xmm32u(0) = op2.xmm32u(0);
  if(op2.xmm32s(1) < op1.xmm32s(1)) op1.xmm32u(1) = op2.xmm32u(1);
  if(op2.xmm32s(2) < op1.xmm32s(2)) op1.xmm32u(2) = op2.xmm32u(2);
  if(op2.xmm32s(3) < op1.xmm32s(3)) op1.xmm32u(3) = op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 3A */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMINUW_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  if(op2.xmm16u(0) < op1.xmm16u(0)) op1.xmm16u(0) = op2.xmm16u(0);
  if(op2.xmm16u(1) < op1.xmm16u(1)) op1.xmm16u(1) = op2.xmm16u(1);
  if(op2.xmm16u(2) < op1.xmm16u(2)) op1.xmm16u(2) = op2.xmm16u(2);
  if(op2.xmm16u(3) < op1.xmm16u(3)) op1.xmm16u(3) = op2.xmm16u(3);
  if(op2.xmm16u(4) < op1.xmm16u(4)) op1.xmm16u(4) = op2.xmm16u(4);
  if(op2.xmm16u(5) < op1.xmm16u(5)) op1.xmm16u(5) = op2.xmm16u(5);
  if(op2.xmm16u(6) < op1.xmm16u(6)) op1.xmm16u(6) = op2.xmm16u(6);
  if(op2.xmm16u(7) < op1.xmm16u(7)) op1.xmm16u(7) = op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 3B */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMINUD_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  if(op2.xmm32u(0) < op1.xmm32u(0)) op1.xmm32u(0) = op2.xmm32u(0);
  if(op2.xmm32u(1) < op1.xmm32u(1)) op1.xmm32u(1) = op2.xmm32u(1);
  if(op2.xmm32u(2) < op1.xmm32u(2)) op1.xmm32u(2) = op2.xmm32u(2);
  if(op2.xmm32u(3) < op1.xmm32u(3)) op1.xmm32u(3) = op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 3C */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMAXSB_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  for(unsigned j=0; j<16; j++) {
    if(op2.xmmsbyte(j) > op1.xmmsbyte(j)) op1.xmmubyte(j) = op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 3D */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMAXSD_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  if(op2.xmm32s(0) > op1.xmm32s(0)) op1.xmm32u(0) = op2.xmm32u(0);
  if(op2.xmm32s(1) > op1.xmm32s(1)) op1.xmm32u(1) = op2.xmm32u(1);
  if(op2.xmm32s(2) > op1.xmm32s(2)) op1.xmm32u(2) = op2.xmm32u(2);
  if(op2.xmm32s(3) > op1.xmm32s(3)) op1.xmm32u(3) = op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 3E */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMAXUW_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  if(op2.xmm16u(0) > op1.xmm16u(0)) op1.xmm16u(0) = op2.xmm16u(0);
  if(op2.xmm16u(1) > op1.xmm16u(1)) op1.xmm16u(1) = op2.xmm16u(1);
  if(op2.xmm16u(2) > op1.xmm16u(2)) op1.xmm16u(2) = op2.xmm16u(2);
  if(op2.xmm16u(3) > op1.xmm16u(3)) op1.xmm16u(3) = op2.xmm16u(3);
  if(op2.xmm16u(4) > op1.xmm16u(4)) op1.xmm16u(4) = op2.xmm16u(4);
  if(op2.xmm16u(5) > op1.xmm16u(5)) op1.xmm16u(5) = op2.xmm16u(5);
  if(op2.xmm16u(6) > op1.xmm16u(6)) op1.xmm16u(6) = op2.xmm16u(6);
  if(op2.xmm16u(7) > op1.xmm16u(7)) op1.xmm16u(7) = op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 3F */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMAXUD_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  if(op2.xmm32u(0) > op1.xmm32u(0)) op1.xmm32u(0) = op2.xmm32u(0);
  if(op2.xmm32u(1) > op1.xmm32u(1)) op1.xmm32u(1) = op2.xmm32u(1);
  if(op2.xmm32u(2) > op1.xmm32u(2)) op1.xmm32u(2) = op2.xmm32u(2);
  if(op2.xmm32u(3) > op1.xmm32u(3)) op1.xmm32u(3) = op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 40 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULLD_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  Bit64s product1 = Bit64s(op1.xmm32s(0)) * Bit64s(op2.xmm32s(0));
  Bit64s product2 = Bit64s(op1.xmm32s(1)) * Bit64s(op2.xmm32s(1));
  Bit64s product3 = Bit64s(op1.xmm32s(2)) * Bit64s(op2.xmm32s(2));
  Bit64s product4 = Bit64s(op1.xmm32s(3)) * Bit64s(op2.xmm32s(3));

  op1.xmm32u(0) = (Bit32u)(product1 & 0xFFFFFFFF);
  op1.xmm32u(1) = (Bit32u)(product2 & 0xFFFFFFFF);
  op1.xmm32u(2) = (Bit32u)(product3 & 0xFFFFFFFF);
  op1.xmm32u(3) = (Bit32u)(product4 & 0xFFFFFFFF);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 38 41 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PHMINPOSUW_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;

  unsigned min = 0;

  for (unsigned j=1; j < 8; j++) {
     if (op.xmm16u(j) < op.xmm16u(min)) min = j;
  }

  result.xmm16u(0) = op.xmm16u(min);
  result.xmm16u(1) = min;
  result.xmm32u(1) = 0;
  result.xmm64u(1) = 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 3A 0C */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());
  Bit8u mask = i->Ib();

  if (mask & 0x1) op1.xmm32u(0) = op2.xmm32u(0);
  if (mask & 0x2) op1.xmm32u(1) = op2.xmm32u(1);
  if (mask & 0x4) op1.xmm32u(2) = op2.xmm32u(2);
  if (mask & 0x8) op1.xmm32u(3) = op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 3A 0D */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());
  Bit8u mask = i->Ib();

  if (mask & 0x1) op1.xmm64u(0) = op2.xmm64u(0);
  if (mask & 0x2) op1.xmm64u(1) = op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 3A 0E */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PBLENDW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());
  Bit8u mask = i->Ib();

  if (mask & 0x01) op1.xmm16u(0) = op2.xmm16u(0);
  if (mask & 0x02) op1.xmm16u(1) = op2.xmm16u(1);
  if (mask & 0x04) op1.xmm16u(2) = op2.xmm16u(2);
  if (mask & 0x08) op1.xmm16u(3) = op2.xmm16u(3);
  if (mask & 0x10) op1.xmm16u(4) = op2.xmm16u(4);
  if (mask & 0x20) op1.xmm16u(5) = op2.xmm16u(5);
  if (mask & 0x40) op1.xmm16u(6) = op2.xmm16u(6);
  if (mask & 0x80) op1.xmm16u(7) = op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 3A 14 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRB_EbdVdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit8u result = op.xmmubyte(i->Ib() & 0xF);
  BX_WRITE_32BIT_REGZ(i->rm(), (Bit32u) result);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRB_EbdVdqIbM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit8u result = op.xmmubyte(i->Ib() & 0xF);

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  write_virtual_byte(i->seg(), eaddr, result);
#endif
}

/* 66 0F 3A 15 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_EwdVdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit16u result = op.xmm16u(i->Ib() & 7);
  BX_WRITE_32BIT_REGZ(i->rm(), (Bit32u) result);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_EwdVdqIbM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit16u result = op.xmm16u(i->Ib() & 7);

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  write_virtual_word(i->seg(), eaddr, result);
#endif
}

/* 66 0F 3A 16 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRD_EdVdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());

#if BX_SUPPORT_X86_64
  if (i->os64L())  /* 64 bit operand size mode */
  {
     Bit64u result = op.xmm64u(i->Ib() & 1);
     BX_WRITE_64BIT_REG(i->rm(), result);
  }
  else
#endif
  {
     Bit32u result = op.xmm32u(i->Ib() & 3);
     BX_WRITE_32BIT_REGZ(i->rm(), result);
  }
#endif
}

/* 66 0F 3A 16 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRD_EdVdqIbM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->os64L())  /* 64 bit operand size mode */
  {
     Bit64u result = op.xmm64u(i->Ib() & 1);
     write_virtual_qword_64(i->seg(), eaddr, result);
  }
  else
#endif
  {
     Bit32u result = op.xmm32u(i->Ib() & 3);
     write_virtual_dword(i->seg(), eaddr, result);
  }
#endif
}

/* 66 0F 3A 17 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRACTPS_EdVpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit32u result = op.xmm32u(i->Ib() & 3);
  BX_WRITE_32BIT_REGZ(i->rm(), result);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRACTPS_EdVpsIbM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit32u result = op.xmm32u(i->Ib() & 3);

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  write_virtual_dword(i->seg(), eaddr, result);
#endif
}

/* 66 0F 3A 20 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRB_VdqEbIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  Bit8u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = (Bit8u) BX_READ_16BIT_REG(i->rm()); // won't allow reading of AH/CH/BH/DH
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    op2 = read_virtual_byte(i->seg(), eaddr);
  }

  op1.xmmubyte(i->Ib() & 0xF) = op2;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 3A 21 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTPS_VpsWssIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  Bit8u control = i->Ib();
  Bit32u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    BxPackedXmmRegister temp = BX_READ_XMM_REG(i->rm());
    op2 = temp.xmm32u((control >> 6) & 3);
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    op2 = read_virtual_dword(i->seg(), eaddr);
  }

  op1.xmm32u((control >> 4) & 3) = op2;

  if (control & 1) op1.xmm32u(0) = 0;
  if (control & 2) op1.xmm32u(1) = 0;
  if (control & 4) op1.xmm32u(2) = 0;
  if (control & 8) op1.xmm32u(3) = 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 3A 22 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRD_VdqEdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());

#if BX_SUPPORT_X86_64
  if (i->os64L())  /* 64 bit operand size mode */
  {
    op1.xmm64u(i->Ib() & 1) = BX_READ_64BIT_REG(i->rm());
  }
  else
#endif
  {
    op1.xmm32u(i->Ib() & 3) = BX_READ_32BIT_REG(i->rm());
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRD_VdqEdIbM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->os64L())  /* 64 bit operand size mode */
  {
    Bit64u op2 = read_virtual_qword_64(i->seg(), eaddr);
    op1.xmm64u(i->Ib() & 1) = op2;
  }
  else
#endif
  {
    Bit32u op2 = read_virtual_dword(i->seg(), eaddr);
    op1.xmm32u(i->Ib() & 3) = op2;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 3A 42 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MPSADBW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->rm()), result;

  unsigned src_offset =  (i->Ib()  & 3)      * 4;
  unsigned dst_offset = ((i->Ib() >> 2) & 1) * 4;

  for (unsigned j=0; j < 8; j++)
  {
     result.xmm16u(j) = 0;

     for (unsigned k=0; k < 4; k++) {
        Bit8u temp1 = op1.xmmubyte(j + k + dst_offset);
        Bit8u temp2 = op2.xmmubyte(    k + src_offset);
        if (temp1 > temp2)
            result.xmm16u(j) += (temp1 - temp2);
        else
            result.xmm16u(j) += (temp2 - temp1);
     }
  }

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 60 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKLBW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmmubyte(0x0) = op1.xmmubyte(0);
  result.xmmubyte(0x1) = op2.xmmubyte(0);
  result.xmmubyte(0x2) = op1.xmmubyte(1);
  result.xmmubyte(0x3) = op2.xmmubyte(1);
  result.xmmubyte(0x4) = op1.xmmubyte(2);
  result.xmmubyte(0x5) = op2.xmmubyte(2);
  result.xmmubyte(0x6) = op1.xmmubyte(3);
  result.xmmubyte(0x7) = op2.xmmubyte(3);
  result.xmmubyte(0x8) = op1.xmmubyte(4);
  result.xmmubyte(0x9) = op2.xmmubyte(4);
  result.xmmubyte(0xA) = op1.xmmubyte(5);
  result.xmmubyte(0xB) = op2.xmmubyte(5);
  result.xmmubyte(0xC) = op1.xmmubyte(6);
  result.xmmubyte(0xD) = op2.xmmubyte(6);
  result.xmmubyte(0xE) = op1.xmmubyte(7);
  result.xmmubyte(0xF) = op2.xmmubyte(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 61 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKLWD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm16u(0) = op1.xmm16u(0);
  result.xmm16u(1) = op2.xmm16u(0);
  result.xmm16u(2) = op1.xmm16u(1);
  result.xmm16u(3) = op2.xmm16u(1);
  result.xmm16u(4) = op1.xmm16u(2);
  result.xmm16u(5) = op2.xmm16u(2);
  result.xmm16u(6) = op1.xmm16u(3);
  result.xmm16u(7) = op2.xmm16u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* UNPCKLPS:     0F 14 */
/* PUNPCKLDQ: 66 0F 62 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::UNPCKLPS_VpsWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm32u(0) = op1.xmm32u(0);
  result.xmm32u(1) = op2.xmm32u(0);
  result.xmm32u(2) = op1.xmm32u(1);
  result.xmm32u(3) = op2.xmm32u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 63 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PACKSSWB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmmsbyte(0x0) = SaturateWordSToByteS(op1.xmm16s(0));
  result.xmmsbyte(0x1) = SaturateWordSToByteS(op1.xmm16s(1));
  result.xmmsbyte(0x2) = SaturateWordSToByteS(op1.xmm16s(2));
  result.xmmsbyte(0x3) = SaturateWordSToByteS(op1.xmm16s(3));
  result.xmmsbyte(0x4) = SaturateWordSToByteS(op1.xmm16s(4));
  result.xmmsbyte(0x5) = SaturateWordSToByteS(op1.xmm16s(5));
  result.xmmsbyte(0x6) = SaturateWordSToByteS(op1.xmm16s(6));
  result.xmmsbyte(0x7) = SaturateWordSToByteS(op1.xmm16s(7));

  result.xmmsbyte(0x8) = SaturateWordSToByteS(op2.xmm16s(0));
  result.xmmsbyte(0x9) = SaturateWordSToByteS(op2.xmm16s(1));
  result.xmmsbyte(0xA) = SaturateWordSToByteS(op2.xmm16s(2));
  result.xmmsbyte(0xB) = SaturateWordSToByteS(op2.xmm16s(3));
  result.xmmsbyte(0xC) = SaturateWordSToByteS(op2.xmm16s(4));
  result.xmmsbyte(0xD) = SaturateWordSToByteS(op2.xmm16s(5));
  result.xmmsbyte(0xE) = SaturateWordSToByteS(op2.xmm16s(6));
  result.xmmsbyte(0xF) = SaturateWordSToByteS(op2.xmm16s(7));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 64 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPGTB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) = (op1.xmmsbyte(j) > op2.xmmsbyte(j)) ? 0xff : 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 65 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPGTW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16u(0) = (op1.xmm16s(0) > op2.xmm16s(0)) ? 0xffff : 0;
  op1.xmm16u(1) = (op1.xmm16s(1) > op2.xmm16s(1)) ? 0xffff : 0;
  op1.xmm16u(2) = (op1.xmm16s(2) > op2.xmm16s(2)) ? 0xffff : 0;
  op1.xmm16u(3) = (op1.xmm16s(3) > op2.xmm16s(3)) ? 0xffff : 0;
  op1.xmm16u(4) = (op1.xmm16s(4) > op2.xmm16s(4)) ? 0xffff : 0;
  op1.xmm16u(5) = (op1.xmm16s(5) > op2.xmm16s(5)) ? 0xffff : 0;
  op1.xmm16u(6) = (op1.xmm16s(6) > op2.xmm16s(6)) ? 0xffff : 0;
  op1.xmm16u(7) = (op1.xmm16s(7) > op2.xmm16s(7)) ? 0xffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 66 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPGTD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm32u(0) = (op1.xmm32s(0) > op2.xmm32s(0)) ? 0xffffffff : 0;
  op1.xmm32u(1) = (op1.xmm32s(1) > op2.xmm32s(1)) ? 0xffffffff : 0;
  op1.xmm32u(2) = (op1.xmm32s(2) > op2.xmm32s(2)) ? 0xffffffff : 0;
  op1.xmm32u(3) = (op1.xmm32s(3) > op2.xmm32s(3)) ? 0xffffffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 67 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PACKUSWB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmmubyte(0x0) = SaturateWordSToByteU(op1.xmm16s(0));
  result.xmmubyte(0x1) = SaturateWordSToByteU(op1.xmm16s(1));
  result.xmmubyte(0x2) = SaturateWordSToByteU(op1.xmm16s(2));
  result.xmmubyte(0x3) = SaturateWordSToByteU(op1.xmm16s(3));
  result.xmmubyte(0x4) = SaturateWordSToByteU(op1.xmm16s(4));
  result.xmmubyte(0x5) = SaturateWordSToByteU(op1.xmm16s(5));
  result.xmmubyte(0x6) = SaturateWordSToByteU(op1.xmm16s(6));
  result.xmmubyte(0x7) = SaturateWordSToByteU(op1.xmm16s(7));

  result.xmmubyte(0x8) = SaturateWordSToByteU(op2.xmm16s(0));
  result.xmmubyte(0x9) = SaturateWordSToByteU(op2.xmm16s(1));
  result.xmmubyte(0xA) = SaturateWordSToByteU(op2.xmm16s(2));
  result.xmmubyte(0xB) = SaturateWordSToByteU(op2.xmm16s(3));
  result.xmmubyte(0xC) = SaturateWordSToByteU(op2.xmm16s(4));
  result.xmmubyte(0xD) = SaturateWordSToByteU(op2.xmm16s(5));
  result.xmmubyte(0xE) = SaturateWordSToByteU(op2.xmm16s(6));
  result.xmmubyte(0xF) = SaturateWordSToByteU(op2.xmm16s(7));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 68 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKHBW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmmubyte(0x0) = op1.xmmubyte(0x8);
  result.xmmubyte(0x1) = op2.xmmubyte(0x8);
  result.xmmubyte(0x2) = op1.xmmubyte(0x9);
  result.xmmubyte(0x3) = op2.xmmubyte(0x9);
  result.xmmubyte(0x4) = op1.xmmubyte(0xA);
  result.xmmubyte(0x5) = op2.xmmubyte(0xA);
  result.xmmubyte(0x6) = op1.xmmubyte(0xB);
  result.xmmubyte(0x7) = op2.xmmubyte(0xB);
  result.xmmubyte(0x8) = op1.xmmubyte(0xC);
  result.xmmubyte(0x9) = op2.xmmubyte(0xC);
  result.xmmubyte(0xA) = op1.xmmubyte(0xD);
  result.xmmubyte(0xB) = op2.xmmubyte(0xD);
  result.xmmubyte(0xC) = op1.xmmubyte(0xE);
  result.xmmubyte(0xD) = op2.xmmubyte(0xE);
  result.xmmubyte(0xE) = op1.xmmubyte(0xF);
  result.xmmubyte(0xF) = op2.xmmubyte(0xF);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 69 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKHWD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm16u(0) = op1.xmm16u(4);
  result.xmm16u(1) = op2.xmm16u(4);
  result.xmm16u(2) = op1.xmm16u(5);
  result.xmm16u(3) = op2.xmm16u(5);
  result.xmm16u(4) = op1.xmm16u(6);
  result.xmm16u(5) = op2.xmm16u(6);
  result.xmm16u(6) = op1.xmm16u(7);
  result.xmm16u(7) = op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* UNPCKHPS:     0F 15 */
/* PUNPCKHDQ: 66 0F 6A */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::UNPCKHPS_VpsWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm32u(0) = op1.xmm32u(2);
  result.xmm32u(1) = op2.xmm32u(2);
  result.xmm32u(2) = op1.xmm32u(3);
  result.xmm32u(3) = op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 6B */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PACKSSDW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm16s(0) = SaturateDwordSToWordS(op1.xmm32s(0));
  result.xmm16s(1) = SaturateDwordSToWordS(op1.xmm32s(1));
  result.xmm16s(2) = SaturateDwordSToWordS(op1.xmm32s(2));
  result.xmm16s(3) = SaturateDwordSToWordS(op1.xmm32s(3));

  result.xmm16s(4) = SaturateDwordSToWordS(op2.xmm32s(0));
  result.xmm16s(5) = SaturateDwordSToWordS(op2.xmm32s(1));
  result.xmm16s(6) = SaturateDwordSToWordS(op2.xmm32s(2));
  result.xmm16s(7) = SaturateDwordSToWordS(op2.xmm32s(3));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* UNPCKLPD:   66 0F 14 */
/* PUNPCKLQDQ: 66 0F 6C */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKLQDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm64u(1) = op2.xmm64u(0);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* UNPCKHPD:   66 0F 15 */
/* PUNPCKHQDQ: 66 0F 6D */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUNPCKHQDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm64u(0) = op1.xmm64u(1);
  result.xmm64u(1) = op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 70 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFD_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u order = i->Ib();

  result.xmm32u(0) = op.xmm32u((order >> 0) & 0x3);
  result.xmm32u(1) = op.xmm32u((order >> 2) & 0x3);
  result.xmm32u(2) = op.xmm32u((order >> 4) & 0x3);
  result.xmm32u(3) = op.xmm32u((order >> 6) & 0x3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* F2 0F 70 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFHW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u order = i->Ib();

  result.xmm64u(0) = op.xmm64u(0);
  result.xmm16u(4) = op.xmm16u(4 + ((order >> 0) & 0x3));
  result.xmm16u(5) = op.xmm16u(4 + ((order >> 2) & 0x3));
  result.xmm16u(6) = op.xmm16u(4 + ((order >> 4) & 0x3));
  result.xmm16u(7) = op.xmm16u(4 + ((order >> 6) & 0x3));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* F3 0F 70 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFLW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u order = i->Ib();

  result.xmm16u(0) = op.xmm16u((order >> 0) & 0x3);
  result.xmm16u(1) = op.xmm16u((order >> 2) & 0x3);
  result.xmm16u(2) = op.xmm16u((order >> 4) & 0x3);
  result.xmm16u(3) = op.xmm16u((order >> 6) & 0x3);
  result.xmm64u(1) = op.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F 74 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPEQB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) = (op1.xmmubyte(j) == op2.xmmubyte(j)) ? 0xff : 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 75 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPEQW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16u(0) = (op1.xmm16u(0) == op2.xmm16u(0)) ? 0xffff : 0;
  op1.xmm16u(1) = (op1.xmm16u(1) == op2.xmm16u(1)) ? 0xffff : 0;
  op1.xmm16u(2) = (op1.xmm16u(2) == op2.xmm16u(2)) ? 0xffff : 0;
  op1.xmm16u(3) = (op1.xmm16u(3) == op2.xmm16u(3)) ? 0xffff : 0;
  op1.xmm16u(4) = (op1.xmm16u(4) == op2.xmm16u(4)) ? 0xffff : 0;
  op1.xmm16u(5) = (op1.xmm16u(5) == op2.xmm16u(5)) ? 0xffff : 0;
  op1.xmm16u(6) = (op1.xmm16u(6) == op2.xmm16u(6)) ? 0xffff : 0;
  op1.xmm16u(7) = (op1.xmm16u(7) == op2.xmm16u(7)) ? 0xffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 76 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPEQD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm32u(0) = (op1.xmm32u(0) == op2.xmm32u(0)) ? 0xffffffff : 0;
  op1.xmm32u(1) = (op1.xmm32u(1) == op2.xmm32u(1)) ? 0xffffffff : 0;
  op1.xmm32u(2) = (op1.xmm32u(2) == op2.xmm32u(2)) ? 0xffffffff : 0;
  op1.xmm32u(3) = (op1.xmm32u(3) == op2.xmm32u(3)) ? 0xffffffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F C4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRW_VdqEwIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  Bit16u op2;
  Bit8u count = i->Ib() & 0x7;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    op2 = read_virtual_word(i->seg(), eaddr);
  }

  op1.xmm16u(count) = op2;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F C5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_GdUdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u count = i->Ib() & 0x7;
  Bit32u result = (Bit32u) op.xmm16u(count);
  BX_WRITE_32BIT_REGZ(i->nnn(), result);
#endif
}

/* 0F C6 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHUFPS_VpsWpsIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;
  Bit8u order = i->Ib();

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm32u(0) = op1.xmm32u((order >> 0) & 0x3);
  result.xmm32u(1) = op1.xmm32u((order >> 2) & 0x3);
  result.xmm32u(2) = op2.xmm32u((order >> 4) & 0x3);
  result.xmm32u(3) = op2.xmm32u((order >> 6) & 0x3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F C6 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHUFPD_VpdWpdIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;
  Bit8u order = i->Ib();

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm64u(0) = op1.xmm64u((order >> 0) & 0x1);
  result.xmm64u(1) = op2.xmm64u((order >> 1) & 0x1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F D1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) > 15)  /* looking only to low 64 bits */
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm16u(0) >>= shift;
    op1.xmm16u(1) >>= shift;
    op1.xmm16u(2) >>= shift;
    op1.xmm16u(3) >>= shift;
    op1.xmm16u(4) >>= shift;
    op1.xmm16u(5) >>= shift;
    op1.xmm16u(6) >>= shift;
    op1.xmm16u(7) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F D2 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) > 31)  /* looking only to low 64 bits */
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm32u(0) >>= shift;
    op1.xmm32u(1) >>= shift;
    op1.xmm32u(2) >>= shift;
    op1.xmm32u(3) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F D3 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLQ_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) > 63)  /* looking only to low 64 bits */
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm64u(0) >>= shift;
    op1.xmm64u(1) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F D4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm64u(0) += op2.xmm64u(0);
  op1.xmm64u(1) += op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F D5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULLW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  Bit32u product1 = Bit32u(op1.xmm16u(0)) * Bit32u(op2.xmm16u(0));
  Bit32u product2 = Bit32u(op1.xmm16u(1)) * Bit32u(op2.xmm16u(1));
  Bit32u product3 = Bit32u(op1.xmm16u(2)) * Bit32u(op2.xmm16u(2));
  Bit32u product4 = Bit32u(op1.xmm16u(3)) * Bit32u(op2.xmm16u(3));
  Bit32u product5 = Bit32u(op1.xmm16u(4)) * Bit32u(op2.xmm16u(4));
  Bit32u product6 = Bit32u(op1.xmm16u(5)) * Bit32u(op2.xmm16u(5));
  Bit32u product7 = Bit32u(op1.xmm16u(6)) * Bit32u(op2.xmm16u(6));
  Bit32u product8 = Bit32u(op1.xmm16u(7)) * Bit32u(op2.xmm16u(7));

  op1.xmm16u(0) = product1 & 0xffff;
  op1.xmm16u(1) = product2 & 0xffff;
  op1.xmm16u(2) = product3 & 0xffff;
  op1.xmm16u(3) = product4 & 0xffff;
  op1.xmm16u(4) = product5 & 0xffff;
  op1.xmm16u(5) = product6 & 0xffff;
  op1.xmm16u(6) = product7 & 0xffff;
  op1.xmm16u(7) = product8 & 0xffff;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F D8 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBUSB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm64u(0) = result.xmm64u(1) = 0;

  for(unsigned j=0; j<16; j++)
  {
      if(op1.xmmubyte(j) > op2.xmmubyte(j))
      {
          result.xmmubyte(j) = op1.xmmubyte(j) - op2.xmmubyte(j);
      }
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F D9 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBUSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm64u(0) = result.xmm64u(1) = 0;

  for(unsigned j=0; j<8; j++)
  {
      if(op1.xmm16u(j) > op2.xmm16u(j))
      {
           result.xmm16u(j) = op1.xmm16u(j) - op2.xmm16u(j);
      }
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F DA */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMINUB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    if(op2.xmmubyte(j) < op1.xmmubyte(j)) op1.xmmubyte(j) = op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* ANDPS:    0F 54 */
/* ANDPD: 66 0F 54 */
/* PAND:  66 0F DB */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ANDPS_VpsWps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm64u(0) &= op2.xmm64u(0);
  op1.xmm64u(1) &= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F DC */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDUSB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) = SaturateWordSToByteU(Bit16s(op1.xmmubyte(j)) + Bit16s(op2.xmmubyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F DD */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDUSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16u(0) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(0)) + Bit32s(op2.xmm16u(0)));
  op1.xmm16u(1) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(1)) + Bit32s(op2.xmm16u(1)));
  op1.xmm16u(2) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(2)) + Bit32s(op2.xmm16u(2)));
  op1.xmm16u(3) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(3)) + Bit32s(op2.xmm16u(3)));
  op1.xmm16u(4) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(4)) + Bit32s(op2.xmm16u(4)));
  op1.xmm16u(5) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(5)) + Bit32s(op2.xmm16u(5)));
  op1.xmm16u(6) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(6)) + Bit32s(op2.xmm16u(6)));
  op1.xmm16u(7) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(7)) + Bit32s(op2.xmm16u(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F DE */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMAXUB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    if(op2.xmmubyte(j) > op1.xmmubyte(j)) op1.xmmubyte(j) = op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* ANDNPS:    0F 55 */
/* ANDNPD: 66 0F 55 */
/* PANDN:  66 0F DF */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ANDNPS_VpsWps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm64u(0) = ~(op1.xmm64u(0)) & op2.xmm64u(0);
  op1.xmm64u(1) = ~(op1.xmm64u(1)) & op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F E0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PAVGB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) = (op1.xmmubyte(j) + op2.xmmubyte(j) + 1) >> 1;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F E1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRAW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) == 0) return;

  if(op2.xmm64u(0) > 15)  /* looking only to low 64 bits */
  {
    result.xmm16u(0) = (op1.xmm16u(0) & 0x8000) ? 0xffff : 0;
    result.xmm16u(1) = (op1.xmm16u(1) & 0x8000) ? 0xffff : 0;
    result.xmm16u(2) = (op1.xmm16u(2) & 0x8000) ? 0xffff : 0;
    result.xmm16u(3) = (op1.xmm16u(3) & 0x8000) ? 0xffff : 0;
    result.xmm16u(4) = (op1.xmm16u(4) & 0x8000) ? 0xffff : 0;
    result.xmm16u(5) = (op1.xmm16u(5) & 0x8000) ? 0xffff : 0;
    result.xmm16u(6) = (op1.xmm16u(6) & 0x8000) ? 0xffff : 0;
    result.xmm16u(7) = (op1.xmm16u(7) & 0x8000) ? 0xffff : 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    result.xmm16u(0) = (Bit16u)(op1.xmm16s(0) >> shift);
    result.xmm16u(1) = (Bit16u)(op1.xmm16s(1) >> shift);
    result.xmm16u(2) = (Bit16u)(op1.xmm16s(2) >> shift);
    result.xmm16u(3) = (Bit16u)(op1.xmm16s(3) >> shift);
    result.xmm16u(4) = (Bit16u)(op1.xmm16s(4) >> shift);
    result.xmm16u(5) = (Bit16u)(op1.xmm16s(5) >> shift);
    result.xmm16u(6) = (Bit16u)(op1.xmm16s(6) >> shift);
    result.xmm16u(7) = (Bit16u)(op1.xmm16s(7) >> shift);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F E2 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRAD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) == 0) return;

  if(op2.xmm64u(0) > 31)  /* looking only to low 64 bits */
  {
    result.xmm32u(0) = (op1.xmm32u(0) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(1) = (op1.xmm32u(1) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(2) = (op1.xmm32u(2) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(3) = (op1.xmm32u(3) & 0x80000000) ? 0xffffffff : 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    result.xmm32u(0) = (Bit32u)(op1.xmm32s(0) >> shift);
    result.xmm32u(1) = (Bit32u)(op1.xmm32s(1) >> shift);
    result.xmm32u(2) = (Bit32u)(op1.xmm32s(2) >> shift);
    result.xmm32u(3) = (Bit32u)(op1.xmm32s(3) >> shift);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F E3 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PAVGW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16u(0) = (op1.xmm16u(0) + op2.xmm16u(0) + 1) >> 1;
  op1.xmm16u(1) = (op1.xmm16u(1) + op2.xmm16u(1) + 1) >> 1;
  op1.xmm16u(2) = (op1.xmm16u(2) + op2.xmm16u(2) + 1) >> 1;
  op1.xmm16u(3) = (op1.xmm16u(3) + op2.xmm16u(3) + 1) >> 1;
  op1.xmm16u(4) = (op1.xmm16u(4) + op2.xmm16u(4) + 1) >> 1;
  op1.xmm16u(5) = (op1.xmm16u(5) + op2.xmm16u(5) + 1) >> 1;
  op1.xmm16u(6) = (op1.xmm16u(6) + op2.xmm16u(6) + 1) >> 1;
  op1.xmm16u(7) = (op1.xmm16u(7) + op2.xmm16u(7) + 1) >> 1;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F E4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULHUW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  Bit32u product1 = Bit32u(op1.xmm16u(0)) * Bit32u(op2.xmm16u(0));
  Bit32u product2 = Bit32u(op1.xmm16u(1)) * Bit32u(op2.xmm16u(1));
  Bit32u product3 = Bit32u(op1.xmm16u(2)) * Bit32u(op2.xmm16u(2));
  Bit32u product4 = Bit32u(op1.xmm16u(3)) * Bit32u(op2.xmm16u(3));
  Bit32u product5 = Bit32u(op1.xmm16u(4)) * Bit32u(op2.xmm16u(4));
  Bit32u product6 = Bit32u(op1.xmm16u(5)) * Bit32u(op2.xmm16u(5));
  Bit32u product7 = Bit32u(op1.xmm16u(6)) * Bit32u(op2.xmm16u(6));
  Bit32u product8 = Bit32u(op1.xmm16u(7)) * Bit32u(op2.xmm16u(7));

  op1.xmm16u(0) = (Bit16u)(product1 >> 16);
  op1.xmm16u(1) = (Bit16u)(product2 >> 16);
  op1.xmm16u(2) = (Bit16u)(product3 >> 16);
  op1.xmm16u(3) = (Bit16u)(product4 >> 16);
  op1.xmm16u(4) = (Bit16u)(product5 >> 16);
  op1.xmm16u(5) = (Bit16u)(product6 >> 16);
  op1.xmm16u(6) = (Bit16u)(product7 >> 16);
  op1.xmm16u(7) = (Bit16u)(product8 >> 16);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F E5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULHW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  Bit32s product1 = Bit32s(op1.xmm16s(0)) * Bit32s(op2.xmm16s(0));
  Bit32s product2 = Bit32s(op1.xmm16s(1)) * Bit32s(op2.xmm16s(1));
  Bit32s product3 = Bit32s(op1.xmm16s(2)) * Bit32s(op2.xmm16s(2));
  Bit32s product4 = Bit32s(op1.xmm16s(3)) * Bit32s(op2.xmm16s(3));
  Bit32s product5 = Bit32s(op1.xmm16s(4)) * Bit32s(op2.xmm16s(4));
  Bit32s product6 = Bit32s(op1.xmm16s(5)) * Bit32s(op2.xmm16s(5));
  Bit32s product7 = Bit32s(op1.xmm16s(6)) * Bit32s(op2.xmm16s(6));
  Bit32s product8 = Bit32s(op1.xmm16s(7)) * Bit32s(op2.xmm16s(7));

  op1.xmm16u(0) = (Bit16u)(product1 >> 16);
  op1.xmm16u(1) = (Bit16u)(product2 >> 16);
  op1.xmm16u(2) = (Bit16u)(product3 >> 16);
  op1.xmm16u(3) = (Bit16u)(product4 >> 16);
  op1.xmm16u(4) = (Bit16u)(product5 >> 16);
  op1.xmm16u(5) = (Bit16u)(product6 >> 16);
  op1.xmm16u(6) = (Bit16u)(product7 >> 16);
  op1.xmm16u(7) = (Bit16u)(product8 >> 16);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F E8 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBSB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmsbyte(j) = SaturateWordSToByteS(Bit16s(op1.xmmsbyte(j)) - Bit16s(op2.xmmsbyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F E9 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(0)) - Bit32s(op2.xmm16s(0)));
  op1.xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(1)) - Bit32s(op2.xmm16s(1)));
  op1.xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(2)) - Bit32s(op2.xmm16s(2)));
  op1.xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(3)) - Bit32s(op2.xmm16s(3)));
  op1.xmm16s(4) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(4)) - Bit32s(op2.xmm16s(4)));
  op1.xmm16s(5) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(5)) - Bit32s(op2.xmm16s(5)));
  op1.xmm16s(6) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(6)) - Bit32s(op2.xmm16s(6)));
  op1.xmm16s(7) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(7)) - Bit32s(op2.xmm16s(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F EA */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMINSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm16s(0) < op1.xmm16s(0)) op1.xmm16s(0) = op2.xmm16s(0);
  if(op2.xmm16s(1) < op1.xmm16s(1)) op1.xmm16s(1) = op2.xmm16s(1);
  if(op2.xmm16s(2) < op1.xmm16s(2)) op1.xmm16s(2) = op2.xmm16s(2);
  if(op2.xmm16s(3) < op1.xmm16s(3)) op1.xmm16s(3) = op2.xmm16s(3);
  if(op2.xmm16s(4) < op1.xmm16s(4)) op1.xmm16s(4) = op2.xmm16s(4);
  if(op2.xmm16s(5) < op1.xmm16s(5)) op1.xmm16s(5) = op2.xmm16s(5);
  if(op2.xmm16s(6) < op1.xmm16s(6)) op1.xmm16s(6) = op2.xmm16s(6);
  if(op2.xmm16s(7) < op1.xmm16s(7)) op1.xmm16s(7) = op2.xmm16s(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* ORPS:    0F 56 */
/* ORPD: 66 0F 56 */
/* POR:  66 0F EB */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ORPS_VpsWps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm64u(0) |= op2.xmm64u(0);
  op1.xmm64u(1) |= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F EC */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDSB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmsbyte(j) = SaturateWordSToByteS(Bit16s(op1.xmmsbyte(j)) + Bit16s(op2.xmmsbyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F ED */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(0)) + Bit32s(op2.xmm16s(0)));
  op1.xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(1)) + Bit32s(op2.xmm16s(1)));
  op1.xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(2)) + Bit32s(op2.xmm16s(2)));
  op1.xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(3)) + Bit32s(op2.xmm16s(3)));
  op1.xmm16s(4) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(4)) + Bit32s(op2.xmm16s(4)));
  op1.xmm16s(5) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(5)) + Bit32s(op2.xmm16s(5)));
  op1.xmm16s(6) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(6)) + Bit32s(op2.xmm16s(6)));
  op1.xmm16s(7) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(7)) + Bit32s(op2.xmm16s(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F EE */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMAXSW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm16s(0) > op1.xmm16s(0)) op1.xmm16s(0) = op2.xmm16s(0);
  if(op2.xmm16s(1) > op1.xmm16s(1)) op1.xmm16s(1) = op2.xmm16s(1);
  if(op2.xmm16s(2) > op1.xmm16s(2)) op1.xmm16s(2) = op2.xmm16s(2);
  if(op2.xmm16s(3) > op1.xmm16s(3)) op1.xmm16s(3) = op2.xmm16s(3);
  if(op2.xmm16s(4) > op1.xmm16s(4)) op1.xmm16s(4) = op2.xmm16s(4);
  if(op2.xmm16s(5) > op1.xmm16s(5)) op1.xmm16s(5) = op2.xmm16s(5);
  if(op2.xmm16s(6) > op1.xmm16s(6)) op1.xmm16s(6) = op2.xmm16s(6);
  if(op2.xmm16s(7) > op1.xmm16s(7)) op1.xmm16s(7) = op2.xmm16s(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* XORPS:    0F 57 */
/* XORPD: 66 0F 57 */
/* PXOR:  66 0F EF */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XORPS_VpsWps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm64u(0) ^= op2.xmm64u(0);
  op1.xmm64u(1) ^= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F F1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) > 15)  /* looking only to low 64 bits */
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm16u(0) <<= shift;
    op1.xmm16u(1) <<= shift;
    op1.xmm16u(2) <<= shift;
    op1.xmm16u(3) <<= shift;
    op1.xmm16u(4) <<= shift;
    op1.xmm16u(5) <<= shift;
    op1.xmm16u(6) <<= shift;
    op1.xmm16u(7) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F F2 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) > 31)  /* looking only to low 64 bits */
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm32u(0) <<= shift;
    op1.xmm32u(1) <<= shift;
    op1.xmm32u(2) <<= shift;
    op1.xmm32u(3) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F F3 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLQ_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) > 63)  /* looking only to low 64 bits */
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm64u(0) <<= shift;
    op1.xmm64u(1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F F4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULUDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  result.xmm64u(0) = Bit64u(op1.xmm32u(0)) * Bit64u(op2.xmm32u(0));
  result.xmm64u(1) = Bit64u(op1.xmm32u(2)) * Bit64u(op2.xmm32u(2));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F F5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMADDWD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<4; j++)
  {
    if(op1.xmm32u(j) == 0x80008000 && op2.xmm32u(j) == 0x80008000) {
        result.xmm32u(j) = 0x80000000;
    }
    else {
      result.xmm32u(j) =
        Bit32s(op1.xmm16s(2*j+0)) * Bit32s(op2.xmm16s(2*j+0)) +
        Bit32s(op1.xmm16s(2*j+1)) * Bit32s(op2.xmm16s(2*j+1));
    }
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/* 66 0F F6 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSADBW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;
  Bit16u temp1 = 0, temp2 = 0;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  temp1 += abs(op1.xmmubyte(0x0) - op2.xmmubyte(0x0));
  temp1 += abs(op1.xmmubyte(0x1) - op2.xmmubyte(0x1));
  temp1 += abs(op1.xmmubyte(0x2) - op2.xmmubyte(0x2));
  temp1 += abs(op1.xmmubyte(0x3) - op2.xmmubyte(0x3));
  temp1 += abs(op1.xmmubyte(0x4) - op2.xmmubyte(0x4));
  temp1 += abs(op1.xmmubyte(0x5) - op2.xmmubyte(0x5));
  temp1 += abs(op1.xmmubyte(0x6) - op2.xmmubyte(0x6));
  temp1 += abs(op1.xmmubyte(0x7) - op2.xmmubyte(0x7));

  temp2 += abs(op1.xmmubyte(0x8) - op2.xmmubyte(0x8));
  temp2 += abs(op1.xmmubyte(0x9) - op2.xmmubyte(0x9));
  temp2 += abs(op1.xmmubyte(0xA) - op2.xmmubyte(0xA));
  temp2 += abs(op1.xmmubyte(0xB) - op2.xmmubyte(0xB));
  temp2 += abs(op1.xmmubyte(0xC) - op2.xmmubyte(0xC));
  temp2 += abs(op1.xmmubyte(0xD) - op2.xmmubyte(0xD));
  temp2 += abs(op1.xmmubyte(0xE) - op2.xmmubyte(0xE));
  temp2 += abs(op1.xmmubyte(0xF) - op2.xmmubyte(0xF));

  op1.xmm64u(0) = Bit64u(temp1);
  op1.xmm64u(1) = Bit64u(temp2);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F F8 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) -= op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F F9 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16u(0) -= op2.xmm16u(0);
  op1.xmm16u(1) -= op2.xmm16u(1);
  op1.xmm16u(2) -= op2.xmm16u(2);
  op1.xmm16u(3) -= op2.xmm16u(3);
  op1.xmm16u(4) -= op2.xmm16u(4);
  op1.xmm16u(5) -= op2.xmm16u(5);
  op1.xmm16u(6) -= op2.xmm16u(6);
  op1.xmm16u(7) -= op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F FA */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm32u(0) -= op2.xmm32u(0);
  op1.xmm32u(1) -= op2.xmm32u(1);
  op1.xmm32u(2) -= op2.xmm32u(2);
  op1.xmm32u(3) -= op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F FB */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSUBQ_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm64u(0) -= op2.xmm64u(0);
  op1.xmm64u(1) -= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F FC */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDB_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) += op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F FD */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDW_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm16u(0) += op2.xmm16u(0);
  op1.xmm16u(1) += op2.xmm16u(1);
  op1.xmm16u(2) += op2.xmm16u(2);
  op1.xmm16u(3) += op2.xmm16u(3);
  op1.xmm16u(4) += op2.xmm16u(4);
  op1.xmm16u(5) += op2.xmm16u(5);
  op1.xmm16u(6) += op2.xmm16u(6);
  op1.xmm16u(7) += op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F FE */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PADDD_VdqWdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), eaddr, (Bit8u *) &op2);
  }

  op1.xmm32u(0) += op2.xmm32u(0);
  op1.xmm32u(1) += op2.xmm32u(1);
  op1.xmm32u(2) += op2.xmm32u(2);
  op1.xmm32u(3) += op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 71 Grp12 010 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLW_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 15) {
    op.xmm64u(0) = 0;
    op.xmm64u(1) = 0;
  }
  else {
    op.xmm16u(0) >>= shift;
    op.xmm16u(1) >>= shift;
    op.xmm16u(2) >>= shift;
    op.xmm16u(3) >>= shift;
    op.xmm16u(4) >>= shift;
    op.xmm16u(5) >>= shift;
    op.xmm16u(6) >>= shift;
    op.xmm16u(7) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op);
#endif
}

/* 0F 71 Grp12 100 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRAW_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift == 0) return;

  if(shift > 15) {
    op.xmm16u(0) = (op.xmm16u(0) & 0x8000) ? 0xffff : 0;
    op.xmm16u(1) = (op.xmm16u(1) & 0x8000) ? 0xffff : 0;
    op.xmm16u(2) = (op.xmm16u(2) & 0x8000) ? 0xffff : 0;
    op.xmm16u(3) = (op.xmm16u(3) & 0x8000) ? 0xffff : 0;
    op.xmm16u(4) = (op.xmm16u(4) & 0x8000) ? 0xffff : 0;
    op.xmm16u(5) = (op.xmm16u(5) & 0x8000) ? 0xffff : 0;
    op.xmm16u(6) = (op.xmm16u(6) & 0x8000) ? 0xffff : 0;
    op.xmm16u(7) = (op.xmm16u(7) & 0x8000) ? 0xffff : 0;
  }
  else {
    op.xmm16u(0) = (Bit16u)(op.xmm16s(0) >> shift);
    op.xmm16u(1) = (Bit16u)(op.xmm16s(1) >> shift);
    op.xmm16u(2) = (Bit16u)(op.xmm16s(2) >> shift);
    op.xmm16u(3) = (Bit16u)(op.xmm16s(3) >> shift);
    op.xmm16u(4) = (Bit16u)(op.xmm16s(4) >> shift);
    op.xmm16u(5) = (Bit16u)(op.xmm16s(5) >> shift);
    op.xmm16u(6) = (Bit16u)(op.xmm16s(6) >> shift);
    op.xmm16u(7) = (Bit16u)(op.xmm16s(7) >> shift);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op);
#endif
}

/* 66 0F 71 Grp12 110 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLW_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 15) {
    op.xmm64u(0) = 0;
    op.xmm64u(1) = 0;
  }
  else {
    op.xmm16u(0) <<= shift;
    op.xmm16u(1) <<= shift;
    op.xmm16u(2) <<= shift;
    op.xmm16u(3) <<= shift;
    op.xmm16u(4) <<= shift;
    op.xmm16u(5) <<= shift;
    op.xmm16u(6) <<= shift;
    op.xmm16u(7) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op);
#endif
}

/* 66 0F 72 Grp13 010 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLD_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 31) {
    op.xmm64u(0) = 0;
    op.xmm64u(1) = 0;
  }
  else {
    op.xmm32u(0) >>= shift;
    op.xmm32u(1) >>= shift;
    op.xmm32u(2) >>= shift;
    op.xmm32u(3) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op);
#endif
}

/* 0F 72 Grp13 100 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRAD_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift == 0) return;

  if(shift > 31) {
    op.xmm32u(0) = (op.xmm32u(0) & 0x80000000) ? 0xffffffff : 0;
    op.xmm32u(1) = (op.xmm32u(1) & 0x80000000) ? 0xffffffff : 0;
    op.xmm32u(2) = (op.xmm32u(2) & 0x80000000) ? 0xffffffff : 0;
    op.xmm32u(3) = (op.xmm32u(3) & 0x80000000) ? 0xffffffff : 0;
  }
  else {
    op.xmm32u(0) = (Bit32u)(op.xmm32s(0) >> shift);
    op.xmm32u(1) = (Bit32u)(op.xmm32s(1) >> shift);
    op.xmm32u(2) = (Bit32u)(op.xmm32s(2) >> shift);
    op.xmm32u(3) = (Bit32u)(op.xmm32s(3) >> shift);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op);
#endif
}

/* 66 0F 72 Grp13 110 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLD_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 31) {
    op.xmm64u(0) = 0;
    op.xmm64u(1) = 0;
  }
  else {
    op.xmm32u(0) <<= shift;
    op.xmm32u(1) <<= shift;
    op.xmm32u(2) <<= shift;
    op.xmm32u(3) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op);
#endif
}

/* 66 0F 73 Grp14 010 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLQ_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 63) {
    op.xmm64u(0) = 0;
    op.xmm64u(1) = 0;
  }
  else {
    op.xmm64u(0) >>= shift;
    op.xmm64u(1) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op);
#endif
}

/* 66 0F 73 Grp14 011 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSRLDQ_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  result.xmm64u(0) = result.xmm64u(1) = 0;

  for(unsigned j=shift; j<16; j++) {
    result.xmmubyte(j-shift) = op.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), result);
#endif
}

/* 66 0F 73 Grp14 110 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLQ_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 63) {
    op.xmm64u(0) = 0;
    op.xmm64u(1) = 0;
  }
  else {
    op.xmm64u(0) <<= shift;
    op.xmm64u(1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op);
#endif
}

/* 66 0F 73 Grp14 111 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSLLDQ_UdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  result.xmm64u(0) = result.xmm64u(1) = 0;

  for(unsigned j=shift; j<16; j++) {
    result.xmmubyte(j) = op.xmmubyte(j-shift);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), result);
#endif
}
