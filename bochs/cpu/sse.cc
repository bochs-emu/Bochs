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

/* ********************************************** */
/* SSE Integer Operations (128bit MMX extensions) */
/* ********************************************** */

/* 66 0F 63 */
void BX_CPU_C::PACKSSWB_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PACKSSWB_VdqWq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 64 */
void BX_CPU_C::PCMPGTB_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmsbyte(j) = (op1.xmmsbyte(j) > op2.xmmsbyte(j)) ? 0xff : 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PCMPGTB_VdqWq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 65 */
void BX_CPU_C::PCMPGTW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm16s(0) = (op1.xmm16s(0) > op2.xmm16s(0)) ? 0xffff : 0;
  op1.xmm16s(1) = (op1.xmm16s(1) > op2.xmm16s(1)) ? 0xffff : 0;
  op1.xmm16s(2) = (op1.xmm16s(2) > op2.xmm16s(2)) ? 0xffff : 0;
  op1.xmm16s(3) = (op1.xmm16s(3) > op2.xmm16s(3)) ? 0xffff : 0;
  op1.xmm16s(4) = (op1.xmm16s(4) > op2.xmm16s(4)) ? 0xffff : 0;
  op1.xmm16s(5) = (op1.xmm16s(5) > op2.xmm16s(5)) ? 0xffff : 0;
  op1.xmm16s(6) = (op1.xmm16s(6) > op2.xmm16s(6)) ? 0xffff : 0;
  op1.xmm16s(7) = (op1.xmm16s(7) > op2.xmm16s(7)) ? 0xffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PCMPGTW_VdqWq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 66 */
void BX_CPU_C::PCMPGTD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm32s(0) = (op1.xmm32s(0) > op2.xmm32s(0)) ? 0xffffffff : 0;
  op1.xmm32s(1) = (op1.xmm32s(1) > op2.xmm32s(1)) ? 0xffffffff : 0;
  op1.xmm32s(2) = (op1.xmm32s(2) > op2.xmm32s(2)) ? 0xffffffff : 0;
  op1.xmm32s(3) = (op1.xmm32s(3) > op2.xmm32s(3)) ? 0xffffffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PCMPGTD_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 67 */
void BX_CPU_C::PACKUSWB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PACKUSWB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6B */
void BX_CPU_C::PACKSSDW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PACKSSDW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 74 */
void BX_CPU_C::PCMPEQB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) = (op1.xmmubyte(j) == op2.xmmubyte(j)) ? 0xff : 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PCMPEQB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 75 */
void BX_CPU_C::PCMPEQW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PCMPEQW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 76 */
void BX_CPU_C::PCMPEQD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm32u(0) = (op1.xmm32u(0) == op2.xmm32u(0)) ? 0xffffffff : 0;
  op1.xmm32u(1) = (op1.xmm32u(1) == op2.xmm32u(1)) ? 0xffffffff : 0;
  op1.xmm32u(2) = (op1.xmm32u(2) == op2.xmm32u(2)) ? 0xffffffff : 0;
  op1.xmm32u(3) = (op1.xmm32u(3) == op2.xmm32u(3)) ? 0xffffffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PCMPEQD_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F C4 */
void BX_CPU_C::PINSRW_VdqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  Bit16u op2;
  Bit8u count = i->Ib() & 0x7;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2);
  }

  op1.xmm16u(count) = op2;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PINSRW_VdqEdIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F C5 */
void BX_CPU_C::PEXTRW_VdqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit8u count = i->Ib() & 0x7;
  Bit32u result = (Bit32u) op.xmm16u(count);

  BX_WRITE_32BIT_REG(i->nnn(), result);
#else
  BX_INFO(("PEXTRW_VdqEdIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D1 */
void BX_CPU_C::PSRLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSRLW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D2 */
void BX_CPU_C::PSRLD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSRLD_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D3 */
void BX_CPU_C::PSRLQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSRLQ_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D4 */
void BX_CPU_C::PADDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) += op2.xmm64u(0);
  op1.xmm64u(1) += op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PADDQ_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D5 */
void BX_CPU_C::PMULLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  Bit32u product1 = Bit32u(op1.xmm16u(0)) * Bit32u(op2.xmm16u(0));
  Bit32u product2 = Bit32u(op1.xmm16u(1)) * Bit32u(op2.xmm16u(1));
  Bit32u product3 = Bit32u(op1.xmm16u(2)) * Bit32u(op2.xmm16u(2));
  Bit32u product4 = Bit32u(op1.xmm16u(3)) * Bit32u(op2.xmm16u(3));
  Bit32u product5 = Bit32u(op1.xmm16u(4)) * Bit32u(op2.xmm16u(4));
  Bit32u product6 = Bit32u(op1.xmm16u(5)) * Bit32u(op2.xmm16u(5));
  Bit32u product7 = Bit32u(op1.xmm16u(6)) * Bit32u(op2.xmm16u(6));
  Bit32u product8 = Bit32u(op1.xmm16u(7)) * Bit32u(op2.xmm16u(7));

  result.xmm16u(0) = product1 & 0xffff;
  result.xmm16u(1) = product2 & 0xffff;
  result.xmm16u(2) = product3 & 0xffff;
  result.xmm16u(3) = product4 & 0xffff;
  result.xmm16u(4) = product5 & 0xffff;
  result.xmm16u(5) = product6 & 0xffff;
  result.xmm16u(6) = product7 & 0xffff;
  result.xmm16u(7) = product8 & 0xffff;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMULLW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D7 */
void BX_CPU_C::PMOVMSKB_GdVRdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit32u result = 0;

  if(op.xmmubyte(0x0) & 0x80) result |= 0x0001; 
  if(op.xmmubyte(0x1) & 0x80) result |= 0x0002; 
  if(op.xmmubyte(0x2) & 0x80) result |= 0x0004; 
  if(op.xmmubyte(0x3) & 0x80) result |= 0x0008; 
  if(op.xmmubyte(0x4) & 0x80) result |= 0x0010; 
  if(op.xmmubyte(0x5) & 0x80) result |= 0x0020; 
  if(op.xmmubyte(0x6) & 0x80) result |= 0x0040; 
  if(op.xmmubyte(0x7) & 0x80) result |= 0x0080; 
  if(op.xmmubyte(0x8) & 0x80) result |= 0x0100; 
  if(op.xmmubyte(0x9) & 0x80) result |= 0x0200; 
  if(op.xmmubyte(0xA) & 0x80) result |= 0x0400; 
  if(op.xmmubyte(0xB) & 0x80) result |= 0x0800; 
  if(op.xmmubyte(0xC) & 0x80) result |= 0x1000; 
  if(op.xmmubyte(0xD) & 0x80) result |= 0x2000; 
  if(op.xmmubyte(0xE) & 0x80) result |= 0x4000; 
  if(op.xmmubyte(0xF) & 0x80) result |= 0x8000; 

  /* now write result back to destination */
  BX_WRITE_32BIT_REG(i->nnn(), result);
  
#else
  BX_INFO(("PMOVMSKB_GdVRdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D8 */
void BX_CPU_C::PSUBUSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSUBUSB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D9 */
void BX_CPU_C::PSUBUSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSUBUSW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F DA */
void BX_CPU_C::PMINUB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    if(op2.xmmubyte(j) < op1.xmmubyte(j)) op1.xmmubyte(j) = op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PMINUB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* ANDPS:    0F 54 */
/* ANDPD: 66 0F 54 */
/* PAND:  66 0F DB */

void BX_CPU_C::PAND_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) &= op2.xmm64u(0);
  op1.xmm64u(1) &= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PAND_VdqWdq: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F DC */
void BX_CPU_C::PADDUSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    result.xmmubyte(j) = SaturateWordSToByteU(Bit16s(op1.xmmubyte(j)) + Bit16s(op2.xmmubyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PADDUSB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F DD */
void BX_CPU_C::PADDUSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm16u(0) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(0)) + Bit32s(op2.xmm16u(0)));
  result.xmm16u(1) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(1)) + Bit32s(op2.xmm16u(1)));
  result.xmm16u(2) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(2)) + Bit32s(op2.xmm16u(2)));
  result.xmm16u(3) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(3)) + Bit32s(op2.xmm16u(3)));
  result.xmm16u(4) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(4)) + Bit32s(op2.xmm16u(4)));
  result.xmm16u(5) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(5)) + Bit32s(op2.xmm16u(5)));
  result.xmm16u(6) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(6)) + Bit32s(op2.xmm16u(6)));
  result.xmm16u(7) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(7)) + Bit32s(op2.xmm16u(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PADDUSW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F DE */
void BX_CPU_C::PMAXUB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    if(op2.xmmubyte(j) > op1.xmmubyte(j)) op1.xmmubyte(j) = op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PMAXUB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* ANDNPS:    0F 55 */
/* ANDNPD: 66 0F 55 */
/* PANDN:  66 0F DF */

void BX_CPU_C::PANDN_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) = ~(op1.xmm64u(0)) & op2.xmm64u(0);
  op1.xmm64u(1) = ~(op1.xmm64u(1)) & op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PANDN_VdqWdq: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E0 */
void BX_CPU_C::PAVGB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) = (op1.xmmubyte(j) + op2.xmmubyte(j) + 1) >> 1;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PAVGB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E1 */
void BX_CPU_C::PSRAW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) == 0)
  {
    BX_WRITE_XMM_REG(i->nnn(), op1);
    return;
  }

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

    result.xmm16u(0) = op1.xmm16u(0) >> shift;
    result.xmm16u(1) = op1.xmm16u(1) >> shift;
    result.xmm16u(2) = op1.xmm16u(2) >> shift;
    result.xmm16u(3) = op1.xmm16u(3) >> shift;
    result.xmm16u(4) = op1.xmm16u(4) >> shift;
    result.xmm16u(5) = op1.xmm16u(5) >> shift;
    result.xmm16u(6) = op1.xmm16u(6) >> shift;
    result.xmm16u(7) = op1.xmm16u(7) >> shift;

    if(op1.xmm16u(0) & 0x8000) result.xmm16u(0) |= (0xffff << (16 - shift));
    if(op1.xmm16u(1) & 0x8000) result.xmm16u(1) |= (0xffff << (16 - shift));
    if(op1.xmm16u(2) & 0x8000) result.xmm16u(2) |= (0xffff << (16 - shift));
    if(op1.xmm16u(3) & 0x8000) result.xmm16u(3) |= (0xffff << (16 - shift));
    if(op1.xmm16u(4) & 0x8000) result.xmm16u(4) |= (0xffff << (16 - shift));
    if(op1.xmm16u(5) & 0x8000) result.xmm16u(5) |= (0xffff << (16 - shift));
    if(op1.xmm16u(6) & 0x8000) result.xmm16u(6) |= (0xffff << (16 - shift));
    if(op1.xmm16u(7) & 0x8000) result.xmm16u(7) |= (0xffff << (16 - shift));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSRAW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E2 */
void BX_CPU_C::PSRAD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  if(op2.xmm64u(0) == 0)
  {
    BX_WRITE_XMM_REG(i->nnn(), op1);
    return;
  }

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

    result.xmm32u(0) = op1.xmm32u(0) >> shift;
    result.xmm32u(1) = op1.xmm32u(1) >> shift;
    result.xmm32u(2) = op1.xmm32u(2) >> shift;
    result.xmm32u(3) = op1.xmm32u(3) >> shift;

    if(op1.xmm32u(0) & 0x80000000) result.xmm32u(0) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(1) & 0x80000000) result.xmm32u(1) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(2) & 0x80000000) result.xmm32u(2) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(3) & 0x80000000) result.xmm32u(3) |= (0xffffffff << (32-shift));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSRAD_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E3 */
void BX_CPU_C::PAVGW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PAVGW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E4 */
void BX_CPU_C::PMULHUW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  Bit32u product1 = Bit32u(op1.xmm16u(0)) * Bit32u(op2.xmm16u(0));
  Bit32u product2 = Bit32u(op1.xmm16u(1)) * Bit32u(op2.xmm16u(1));
  Bit32u product3 = Bit32u(op1.xmm16u(2)) * Bit32u(op2.xmm16u(2));
  Bit32u product4 = Bit32u(op1.xmm16u(3)) * Bit32u(op2.xmm16u(3));
  Bit32u product5 = Bit32u(op1.xmm16u(4)) * Bit32u(op2.xmm16u(4));
  Bit32u product6 = Bit32u(op1.xmm16u(5)) * Bit32u(op2.xmm16u(5));
  Bit32u product7 = Bit32u(op1.xmm16u(6)) * Bit32u(op2.xmm16u(6));
  Bit32u product8 = Bit32u(op1.xmm16u(7)) * Bit32u(op2.xmm16u(7));

  result.xmm16u(0) = (Bit16u)(product1 >> 16);
  result.xmm16u(1) = (Bit16u)(product2 >> 16);
  result.xmm16u(2) = (Bit16u)(product3 >> 16);
  result.xmm16u(3) = (Bit16u)(product4 >> 16);
  result.xmm16u(4) = (Bit16u)(product5 >> 16);
  result.xmm16u(5) = (Bit16u)(product6 >> 16);
  result.xmm16u(6) = (Bit16u)(product7 >> 16);
  result.xmm16u(7) = (Bit16u)(product8 >> 16);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else                                        
  BX_INFO(("PMULHUW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E5 */
void BX_CPU_C::PMULHW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  Bit32s product1 = Bit32s(op1.xmm16s(0)) * Bit32s(op2.xmm16s(0));
  Bit32s product2 = Bit32s(op1.xmm16s(1)) * Bit32s(op2.xmm16s(1));
  Bit32s product3 = Bit32s(op1.xmm16s(2)) * Bit32s(op2.xmm16s(2));
  Bit32s product4 = Bit32s(op1.xmm16s(3)) * Bit32s(op2.xmm16s(3));
  Bit32s product5 = Bit32s(op1.xmm16s(4)) * Bit32s(op2.xmm16s(4));
  Bit32s product6 = Bit32s(op1.xmm16s(5)) * Bit32s(op2.xmm16s(5));
  Bit32s product7 = Bit32s(op1.xmm16s(6)) * Bit32s(op2.xmm16s(6));
  Bit32s product8 = Bit32s(op1.xmm16s(7)) * Bit32s(op2.xmm16s(7));

  result.xmm16u(0) = (Bit16u)(product1 >> 16);
  result.xmm16u(1) = (Bit16u)(product2 >> 16);
  result.xmm16u(2) = (Bit16u)(product3 >> 16);
  result.xmm16u(3) = (Bit16u)(product4 >> 16);
  result.xmm16u(4) = (Bit16u)(product5 >> 16);
  result.xmm16u(5) = (Bit16u)(product6 >> 16);
  result.xmm16u(6) = (Bit16u)(product7 >> 16);
  result.xmm16u(7) = (Bit16u)(product8 >> 16);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMULHW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E8 */
void BX_CPU_C::PSUBSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    result.xmmsbyte(j) = SaturateWordSToByteS(Bit16s(op1.xmmsbyte(j)) - Bit16s(op2.xmmsbyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSUBSB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E9 */
void BX_CPU_C::PSUBSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(0)) - Bit32s(op2.xmm16s(0)));
  result.xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(1)) - Bit32s(op2.xmm16s(1)));
  result.xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(2)) - Bit32s(op2.xmm16s(2)));
  result.xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(3)) - Bit32s(op2.xmm16s(3)));
  result.xmm16s(4) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(4)) - Bit32s(op2.xmm16s(4)));
  result.xmm16s(5) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(5)) - Bit32s(op2.xmm16s(5)));
  result.xmm16s(6) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(6)) - Bit32s(op2.xmm16s(6)));
  result.xmm16s(7) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(7)) - Bit32s(op2.xmm16s(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSUBSW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F EA */
void BX_CPU_C::PMINSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PMINSW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* ORPS:    0F 56 */
/* ORPD: 66 0F 56 */
/* POR:  66 0F EB */

void BX_CPU_C::POR_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) |= op2.xmm64u(0);
  op1.xmm64u(1) |= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("POR_VdqWdq: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F EC */
void BX_CPU_C::PADDSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    result.xmmsbyte(j) = SaturateWordSToByteS(Bit16s(op1.xmmsbyte(j)) + Bit16s(op2.xmmsbyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PADDSB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F ED */
void BX_CPU_C::PADDSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(0)) + Bit32s(op2.xmm16s(0)));
  result.xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(1)) + Bit32s(op2.xmm16s(1)));
  result.xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(2)) + Bit32s(op2.xmm16s(2)));
  result.xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(3)) + Bit32s(op2.xmm16s(3)));
  result.xmm16s(4) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(4)) + Bit32s(op2.xmm16s(4)));
  result.xmm16s(5) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(5)) + Bit32s(op2.xmm16s(5)));
  result.xmm16s(6) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(6)) + Bit32s(op2.xmm16s(6)));
  result.xmm16s(7) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(7)) + Bit32s(op2.xmm16s(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PADDSW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F EE */
void BX_CPU_C::PMAXSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PMAXSW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* XORPS:    0F 57 */
/* XORPD: 66 0F 57 */
/* PXOR:  66 0F EF */

void BX_CPU_C::PXOR_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) ^= op2.xmm64u(0);
  op1.xmm64u(1) ^= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PXOR_VdqWdq: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F1 */
void BX_CPU_C::PSLLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSLLW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F2 */
void BX_CPU_C::PSLLD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSLLD_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F3 */
void BX_CPU_C::PSLLQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSLLQ_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F4 */
void BX_CPU_C::PMULUDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm64u(0) = Bit64u(op1.xmm32u(0)) * Bit64u(op2.xmm32u(0));
  result.xmm64u(1) = Bit64u(op1.xmm32u(3)) * Bit64u(op2.xmm32u(3));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMULUDQ_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F5 */
void BX_CPU_C::PMADDWD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<4; j++)
  {
    if(op1.xmm32u(j) == 0x80008000 && op2.xmm32u(j) == 0x80008000) {
        result.xmm32u(j) = 0x80000000;
    }
    else
      result.xmm32u(j) = 
        Bit32s(op1.xmm16s(2*j+0)) * Bit32s(op2.xmm16s(2*j+0)) +
        Bit32s(op1.xmm16s(2*j+1)) * Bit32s(op2.xmm16s(2*j+1));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMADDWD_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F6 */
void BX_CPU_C::PSADBW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;
  Bit16u temp1 = 0, temp2 = 0;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSADBW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F8 */
void BX_CPU_C::PSUBB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) -= op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSUBB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F9 */
void BX_CPU_C::PSUBW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PSUBW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FA */
void BX_CPU_C::PSUBD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm32u(0) -= op2.xmm32u(0);
  op1.xmm32u(1) -= op2.xmm32u(1);
  op1.xmm32u(2) -= op2.xmm32u(2);
  op1.xmm32u(3) -= op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSUBD_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FB */
void BX_CPU_C::PSUBQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) -= op2.xmm64u(0);
  op1.xmm64u(1) -= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSUBQ_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FC */
void BX_CPU_C::PADDB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) {
    op1.xmmubyte(j) += op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PADDB_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FD */
void BX_CPU_C::PADDW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
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
#else
  BX_INFO(("PADDW_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FE */
void BX_CPU_C::PADDD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm32u(0) += op2.xmm32u(0);
  op1.xmm32u(1) += op2.xmm32u(1);
  op1.xmm32u(2) += op2.xmm32u(2);
  op1.xmm32u(3) += op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PADDD_VdqWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 71 Grp12 010 */
void BX_CPU_C::PSRLW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

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
#else
  BX_INFO(("PSRLW_PdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 0F 71 Grp12 100 */
void BX_CPU_C::PSRAW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  if(shift == 0)
  {
    BX_WRITE_XMM_REG(i->nnn(), op);
    return;
  }

  if(shift > 15) {
    result.xmm16u(0) = (op.xmm16u(0) & 0x8000) ? 0xffff : 0;
    result.xmm16u(1) = (op.xmm16u(1) & 0x8000) ? 0xffff : 0;
    result.xmm16u(2) = (op.xmm16u(2) & 0x8000) ? 0xffff : 0;
    result.xmm16u(3) = (op.xmm16u(3) & 0x8000) ? 0xffff : 0;
    result.xmm16u(4) = (op.xmm16u(4) & 0x8000) ? 0xffff : 0;
    result.xmm16u(5) = (op.xmm16u(5) & 0x8000) ? 0xffff : 0;
    result.xmm16u(6) = (op.xmm16u(6) & 0x8000) ? 0xffff : 0;
    result.xmm16u(7) = (op.xmm16u(7) & 0x8000) ? 0xffff : 0;
  }
  else {
    result.xmm16u(0) = op.xmm16u(0) >> shift;
    result.xmm16u(1) = op.xmm16u(1) >> shift;
    result.xmm16u(2) = op.xmm16u(2) >> shift;
    result.xmm16u(3) = op.xmm16u(3) >> shift;
    result.xmm16u(4) = op.xmm16u(4) >> shift;
    result.xmm16u(5) = op.xmm16u(5) >> shift;
    result.xmm16u(6) = op.xmm16u(6) >> shift;
    result.xmm16u(7) = op.xmm16u(7) >> shift;

    if(op.xmm16u(0) & 0x8000) result.xmm16u(0) |= (0xffff << (16 - shift));
    if(op.xmm16u(1) & 0x8000) result.xmm16u(1) |= (0xffff << (16 - shift));
    if(op.xmm16u(2) & 0x8000) result.xmm16u(2) |= (0xffff << (16 - shift));
    if(op.xmm16u(3) & 0x8000) result.xmm16u(3) |= (0xffff << (16 - shift));
    if(op.xmm16u(4) & 0x8000) result.xmm16u(4) |= (0xffff << (16 - shift));
    if(op.xmm16u(5) & 0x8000) result.xmm16u(5) |= (0xffff << (16 - shift));
    if(op.xmm16u(6) & 0x8000) result.xmm16u(6) |= (0xffff << (16 - shift));
    if(op.xmm16u(7) & 0x8000) result.xmm16u(7) |= (0xffff << (16 - shift));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), result);
#else
  BX_INFO(("PSRAW_PdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 71 Grp12 110 */
void BX_CPU_C::PSLLW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

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
#else
  BX_INFO(("PSLLW_PdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 72 Grp13 010 */
void BX_CPU_C::PSRLD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

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
#else
  BX_INFO(("PSRLD_PdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 0F 72 Grp13 100 */
void BX_CPU_C::PSRAD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  if(shift == 0)
  {
    BX_WRITE_XMM_REG(i->nnn(), op);
    return;
  }

  if(shift > 31) {
    result.xmm32u(0) = (op.xmm32u(0) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(1) = (op.xmm32u(1) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(2) = (op.xmm32u(2) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(3) = (op.xmm32u(3) & 0x80000000) ? 0xffffffff : 0;
  }
  else {
    result.xmm32u(0) = op.xmm32u(0) >> shift;
    result.xmm32u(1) = op.xmm32u(1) >> shift;
    result.xmm32u(2) = op.xmm32u(2) >> shift;
    result.xmm32u(3) = op.xmm32u(3) >> shift;

    if(op.xmm32u(0) & 0x80000000) result.xmm32u(0) |= (0xffffffff << (32-shift));
    if(op.xmm32u(1) & 0x80000000) result.xmm32u(1) |= (0xffffffff << (32-shift));
    if(op.xmm32u(2) & 0x80000000) result.xmm32u(2) |= (0xffffffff << (32-shift));
    if(op.xmm32u(3) & 0x80000000) result.xmm32u(3) |= (0xffffffff << (32-shift));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), result);
#else
  BX_INFO(("PSRAD_PdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 72 Grp13 110 */
void BX_CPU_C::PSLLD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

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
#else
  BX_INFO(("PSLLD_PdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 73 Grp14 010 */
void BX_CPU_C::PSRLQ_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

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
#else
  BX_INFO(("PSRLQ_PdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLDQ_WdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  result.xmm64u(0) = 0;
  result.xmm64u(1) = 0;

  for(unsigned j=shift; j<16; j++) {
    result.xmmubyte(j-shift) = op.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), result);
#else
  BX_INFO(("PSRLDQ_WdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 73 Grp14 110 */
void BX_CPU_C::PSLLQ_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

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
#else
  BX_INFO(("PSLLQ_PdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 73 Grp14 111 */
void BX_CPU_C::PSLLDQ_WdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  result.xmm64u(0) = 0;
  result.xmm64u(1) = 0;

  for(unsigned j=shift; j<16; j++) {
    result.xmmubyte(j) = op.xmmubyte(j-shift);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), result);
#else
  BX_INFO(("PSLLDQ_WdqIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}
