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


#if BX_SUPPORT_MMX || BX_SUPPORT_SSE || BX_SUPPORT_SSE2

#define MMX_REGFILE ((BX_CPU_THIS_PTR the_i387).mmx)

#define MMX_TWD                (MMX_REGFILE.twd)
#define MMX_SWD                (MMX_REGFILE.swd)
#define MMX_TOS                (MMX_REGFILE.tos)
#define BX_READ_MMX_REG(index) (MMX_REGFILE.mmx[index].packed_mmx_register)

#define BX_WRITE_MMX_REG(index, value) \
{                                                      \
   MMX_REGFILE.mmx[index].packed_mmx_register = value; \
   MMX_REGFILE.mmx[index].exp = 0xffff; \
}                                                      

static Bit8s SaturateWordSToByteS(Bit16s value)
{
/*
  SaturateWordSToByteS   converts   a signed 16-bit value to a
  signed  8-bit  value. If the signed 16-bit value is less than -128, it
  is  represented  by  the saturated value -128 (0x80). If it is greater
  than 127, it is represented by the saturated value 127 (0x7F).
*/
  if(value < -128) return -128;
  if(value >  127) return  127;
  return value;
}

static Bit16s SaturateDwordSToWordS(Bit32s value)
{
/*
  SaturateDwordSToWordS  converts  a  signed 32-bit value to a
  signed  16-bit  value. If the signed 32-bit value is less than -32768,
  it  is  represented  by  the saturated value -32768 (0x8000). If it is
  greater  than  32767,  it  is represented by the saturated value 32767
  (0x7FFF).
*/
  if(value < -32768) return -32768;
  if(value >  32767) return  32767;
  return value;
}

static Bit8u SaturateWordSToByteU(Bit16s value)
{
/*
  SaturateWordSToByteU  converts a signed 16-bit value to an
  unsigned  8-bit value. If the signed 16-bit value is less than zero it
  is  represented  by  the  saturated value zero (0x00).If it is greater
  than 255 it is represented by the saturated value 255 (0xFF).
*/
  if(value < 0) return 0;
  if(value > 255) return 255;
  return value;
}

static Bit16u SaturateDwordSToWordU(Bit32s value)
{
/*
  SaturateDwordSToWordU  converts  a signed 32-bit value
  to  an  unsigned  16-bit value. If the signed 32-bit value is less
  than   zero,   it   is  represented  by  the saturated value 65535
  (0x0000).  If  it  is greater  than  65535,  it  is represented by
  the saturated value 65535 (0xFFFF).
*/
  if(value < 0) return 0;
  if(value > 65535) return 65535;
  return value;
}

static Bit16u SelectMmxWord(BxPackedMmxRegister mmx, unsigned index)
{
  return (MMXUQ(mmx) >> ((index & 0x3) * 16)) & 0xffff;
}

void BX_CPU_C::printMmxRegisters(void)
{
  for(int i=0;i<8;i++) {
      BxPackedMmxRegister mm = BX_READ_MMX_REG(i);
      fprintf(stderr, "MM%d: %.16llx\n", i, MMXUQ(mm));
  }
}

void BX_CPU_C::prepareMMX(void)
{
  if(BX_CPU_THIS_PTR cr0.ts)
    exception(BX_NM_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.em)
    exception(BX_UD_EXCEPTION, 0, 0);

  MMX_TWD = 0;
  MMX_TOS = 0;        /* Each time an MMX instruction is */
  MMX_SWD &= 0xc7ff;  /*    executed, the TOS value is set to 000b */
}
#endif


/* 0F 60 */
void BX_CPU_C::PUNPCKLBW_PqQd(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), result;
  Bit32u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  MMXUB7(result) = (op2) >> 24;
  MMXUB6(result) = MMXUB3(op1);
  MMXUB5(result) = (op2 & 0x00ff0000) >> 16;
  MMXUB4(result) = MMXUB2(op1);
  MMXUB3(result) = (op2 & 0x0000ff00) >>  8;
  MMXUB2(result) = MMXUB1(op1);
  MMXUB1(result) = (op2 & 0x000000ff);
  MMXUB0(result) = MMXUB0(op1);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 61 */
void BX_CPU_C::PUNPCKLWD_PqQd(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), result;
  Bit32u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  MMXUW3(result) = (op2) >> 16;
  MMXUW2(result) = MMXUW1(op1);
  MMXUW1(result) = (op2 & 0x0000ffff);
  MMXUW0(result) = MMXUW0(op1);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 62 */
void BX_CPU_C::PUNPCKLDQ_PqQd(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn());
  Bit32u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  MMXUD1(op1) = op2;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 63 */
void BX_CPU_C::PACKSSWB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXSB0(result) = SaturateWordSToByteS(MMXSW0(op1));
  MMXSB1(result) = SaturateWordSToByteS(MMXSW1(op1));
  MMXSB2(result) = SaturateWordSToByteS(MMXSW2(op1));
  MMXSB3(result) = SaturateWordSToByteS(MMXSW3(op1));
  MMXSB4(result) = SaturateWordSToByteS(MMXSW0(op2));
  MMXSB5(result) = SaturateWordSToByteS(MMXSW1(op2));
  MMXSB6(result) = SaturateWordSToByteS(MMXSW2(op2));
  MMXSB7(result) = SaturateWordSToByteS(MMXSW3(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 64 */
void BX_CPU_C::PCMPGTB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUB0(result) = (MMXSB0(op1) > MMXSB0(op2)) ? 0xff : 0;
  MMXUB1(result) = (MMXSB1(op1) > MMXSB1(op2)) ? 0xff : 0;
  MMXUB2(result) = (MMXSB2(op1) > MMXSB2(op2)) ? 0xff : 0;
  MMXUB3(result) = (MMXSB3(op1) > MMXSB3(op2)) ? 0xff : 0;
  MMXUB4(result) = (MMXSB4(op1) > MMXSB4(op2)) ? 0xff : 0;
  MMXUB5(result) = (MMXSB5(op1) > MMXSB5(op2)) ? 0xff : 0;
  MMXUB6(result) = (MMXSB6(op1) > MMXSB6(op2)) ? 0xff : 0;
  MMXUB7(result) = (MMXSB7(op1) > MMXSB7(op2)) ? 0xff : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 65 */
void BX_CPU_C::PCMPGTW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUW0(result) = (MMXSW0(op1) > MMXSW0(op2)) ? 0xffff : 0;
  MMXUW1(result) = (MMXSW1(op1) > MMXSW1(op2)) ? 0xffff : 0;
  MMXUW2(result) = (MMXSW2(op1) > MMXSW2(op2)) ? 0xffff : 0;
  MMXUW3(result) = (MMXSW3(op1) > MMXSW3(op2)) ? 0xffff : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 66 */
void BX_CPU_C::PCMPGTD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUD0(result) = (MMXSD0(op1) > MMXSD0(op2)) ? 0xffffffff : 0;
  MMXUD1(result) = (MMXSD1(op1) > MMXSD1(op2)) ? 0xffffffff : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 67 */
void BX_CPU_C::PACKUSWB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUB0(result) = SaturateWordSToByteU(MMXSW0(op1));
  MMXUB1(result) = SaturateWordSToByteU(MMXSW1(op1));
  MMXUB2(result) = SaturateWordSToByteU(MMXSW2(op1));
  MMXUB3(result) = SaturateWordSToByteU(MMXSW3(op1));
  MMXUB4(result) = SaturateWordSToByteU(MMXSW0(op2));
  MMXUB5(result) = SaturateWordSToByteU(MMXSW1(op2));
  MMXUB6(result) = SaturateWordSToByteU(MMXSW2(op2));
  MMXUB7(result) = SaturateWordSToByteU(MMXSW3(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 68 */
void BX_CPU_C::PUNPCKHBW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUB7(result) = MMXUB7(op2);
  MMXUB6(result) = MMXUB7(op1);
  MMXUB5(result) = MMXUB6(op2);
  MMXUB4(result) = MMXUB6(op1);
  MMXUB3(result) = MMXUB5(op2);
  MMXUB2(result) = MMXUB5(op1);
  MMXUB1(result) = MMXUB4(op2);
  MMXUB0(result) = MMXUB4(op1);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 69 */
void BX_CPU_C::PUNPCKHWD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUW3(result) = MMXUW3(op2);
  MMXUW2(result) = MMXUW3(op1);
  MMXUW1(result) = MMXUW2(op2);
  MMXUW0(result) = MMXUW2(op1);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 6A */
void BX_CPU_C::PUNPCKHDQ_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUD1(result) = MMXUD1(op2);
  MMXUD0(result) = MMXUD1(op1);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 6B */
void BX_CPU_C::PACKSSDW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXSW0(result) = SaturateDwordSToWordS(MMXSD0(op1));
  MMXSW1(result) = SaturateDwordSToWordS(MMXSD1(op1));
  MMXSW2(result) = SaturateDwordSToWordS(MMXSD0(op2));
  MMXSW3(result) = SaturateDwordSToWordS(MMXSD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 6E */
void BX_CPU_C::MOVD_PqEd(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op;
  MMXUD1(op) = 0;

  /* op is a register or memory reference */
  if (i->modC0()) {
    MMXUD0(op) = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &(MMXUD0(op)));
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 6F */
void BX_CPU_C::MOVQ_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 70 */
void BX_CPU_C::PSHUFW_PqQqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op, result;
  Bit16u order = i->Ib();

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  MMXUW0(result) = SelectMmxWord(op, order);
  MMXUW1(result) = SelectMmxWord(op, order >> 2);
  MMXUW2(result) = SelectMmxWord(op, order >> 4);
  MMXUW3(result) = SelectMmxWord(op, order >> 5);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else  
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PCMPEQB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUB0(result) = (MMXUB0(op1) == MMXUB0(op2)) ? 0xff : 0;
  MMXUB1(result) = (MMXUB1(op1) == MMXUB1(op2)) ? 0xff : 0;
  MMXUB2(result) = (MMXUB2(op1) == MMXUB2(op2)) ? 0xff : 0;
  MMXUB3(result) = (MMXUB3(op1) == MMXUB3(op2)) ? 0xff : 0;
  MMXUB4(result) = (MMXUB4(op1) == MMXUB4(op2)) ? 0xff : 0;
  MMXUB5(result) = (MMXUB5(op1) == MMXUB5(op2)) ? 0xff : 0;
  MMXUB6(result) = (MMXUB6(op1) == MMXUB6(op2)) ? 0xff : 0;
  MMXUB7(result) = (MMXUB7(op1) == MMXUB7(op2)) ? 0xff : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 75 */
void BX_CPU_C::PCMPEQW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUW0(result) = (MMXUW0(op1) == MMXUW0(op2)) ? 0xffff : 0;
  MMXUW1(result) = (MMXUW1(op1) == MMXUW1(op2)) ? 0xffff : 0;
  MMXUW2(result) = (MMXUW2(op1) == MMXUW2(op2)) ? 0xffff : 0;
  MMXUW3(result) = (MMXUW3(op1) == MMXUW3(op2)) ? 0xffff : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 76 */
void BX_CPU_C::PCMPEQD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUD0(result) = (MMXUD0(op1) == MMXUD0(op2)) ? 0xffffffff : 0;
  MMXUD1(result) = (MMXUD1(op1) == MMXUD1(op2)) ? 0xffffffff : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 77 */
void BX_CPU_C::EMMS(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX

  if(BX_CPU_THIS_PTR cr0.em)
    exception(BX_UD_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.ts)
    exception(BX_NM_EXCEPTION, 0, 0);

  MMX_TWD = 0xffffffff;
  MMX_TOS = 0;        /* Each time an MMX instruction is */
  MMX_SWD &= 0xc7ff;  /*    executed, the TOS value is set to 000b */

#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 7E */
void BX_CPU_C::MOVD_EdPd(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->nnn());

  /* op is a register or memory reference */
  if (i->modC0()) {
    BX_WRITE_32BIT_REG(i->rm(), MMXUD0(op));
  }
  else {
    write_virtual_dword(i->seg(), RMAddr(i), &(MMXUD0(op)));
  }
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 7F */
void BX_CPU_C::MOVQ_QqPq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->nnn());

  /* op is a register or memory reference */
  if (i->modC0()) {
    BX_WRITE_MMX_REG(i->rm(), op);
  }
  else {
    write_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F D1 */
void BX_CPU_C::PSRLW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  if(MMXUQ(op2) > 15) MMXUQ(op1) = 0;
  else
  {
    Bit8u shift = MMXUB0(op2);
 
    MMXUW0(op1) >>= shift;
    MMXUW1(op1) >>= shift;
    MMXUW2(op1) >>= shift;
    MMXUW3(op1) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F D2 */
void BX_CPU_C::PSRLD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  if(MMXUQ(op2) > 31) MMXUQ(op1) = 0;
  else
  {
    Bit8u shift = MMXUB0(op2);

    MMXUD0(op1) >>= shift;
    MMXUD1(op1) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F D3 */
void BX_CPU_C::PSRLQ_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  if(MMXUQ(op2) > 63) {
    MMXUQ(op1) = 0;
  }
  else {
    MMXUQ(op1) >>= MMXUB0(op2);
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F D4 */
void BX_CPU_C::PADDQ_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUQ(op1) += MMXUQ(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F D5 */
void BX_CPU_C::PMULLW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  Bit32u product1 = (Bit32u)(MMXUW0(op1)) * (Bit32u)(MMXUW0(op2));
  Bit32u product2 = (Bit32u)(MMXUW1(op1)) * (Bit32u)(MMXUW1(op2));
  Bit32u product3 = (Bit32u)(MMXUW2(op1)) * (Bit32u)(MMXUW2(op2));
  Bit32u product4 = (Bit32u)(MMXUW3(op1)) * (Bit32u)(MMXUW3(op2));

  MMXUW0(result) = product1 & 0xffff;
  MMXUW1(result) = product2 & 0xffff;
  MMXUW2(result) = product3 & 0xffff;
  MMXUW3(result) = product4 & 0xffff;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F D8 */
void BX_CPU_C::PSUBUSB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUQ(result) = 0;

  if(MMXUB0(op1) > MMXUB0(op2)) MMXUB0(result) = MMXUB0(op1) - MMXUB0(op2);
  if(MMXUB1(op1) > MMXUB1(op2)) MMXUB1(result) = MMXUB1(op1) - MMXUB1(op2);
  if(MMXUB2(op1) > MMXUB2(op2)) MMXUB2(result) = MMXUB2(op1) - MMXUB2(op2);
  if(MMXUB3(op1) > MMXUB3(op2)) MMXUB3(result) = MMXUB3(op1) - MMXUB3(op2);
  if(MMXUB4(op1) > MMXUB4(op2)) MMXUB4(result) = MMXUB4(op1) - MMXUB4(op2);
  if(MMXUB5(op1) > MMXUB5(op2)) MMXUB5(result) = MMXUB5(op1) - MMXUB5(op2);
  if(MMXUB6(op1) > MMXUB6(op2)) MMXUB6(result) = MMXUB6(op1) - MMXUB6(op2);
  if(MMXUB7(op1) > MMXUB7(op2)) MMXUB7(result) = MMXUB7(op1) - MMXUB7(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F D9 */
void BX_CPU_C::PSUBUSW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUQ(result) = 0;

  if(MMXUW0(op1) > MMXUW0(op2)) MMXUW0(result) = MMXUW0(op1) - MMXUW0(op2);
  if(MMXUW1(op1) > MMXUW1(op2)) MMXUW1(result) = MMXUW1(op1) - MMXUW1(op2);
  if(MMXUW2(op1) > MMXUW2(op2)) MMXUW2(result) = MMXUW2(op1) - MMXUW2(op2);
  if(MMXUW3(op1) > MMXUW3(op2)) MMXUW3(result) = MMXUW3(op1) - MMXUW3(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F DA */
void BX_CPU_C::PMINUB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
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

  MMXUB0(result) = (MMXUB0(op1) < MMXUB0(op2)) ? MMXUB0(op1) : MMXUB0(op2);
  MMXUB1(result) = (MMXUB1(op1) < MMXUB1(op2)) ? MMXUB1(op1) : MMXUB1(op2);
  MMXUB2(result) = (MMXUB2(op1) < MMXUB2(op2)) ? MMXUB2(op1) : MMXUB2(op2);
  MMXUB3(result) = (MMXUB3(op1) < MMXUB3(op2)) ? MMXUB3(op1) : MMXUB3(op2);
  MMXUB4(result) = (MMXUB4(op1) < MMXUB4(op2)) ? MMXUB4(op1) : MMXUB4(op2);
  MMXUB4(result) = (MMXUB5(op1) < MMXUB5(op2)) ? MMXUB5(op1) : MMXUB5(op2);
  MMXUB5(result) = (MMXUB6(op1) < MMXUB6(op2)) ? MMXUB6(op1) : MMXUB6(op2);
  MMXUB6(result) = (MMXUB7(op1) < MMXUB7(op2)) ? MMXUB7(op1) : MMXUB7(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else  
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F DB */
void BX_CPU_C::PAND_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUQ(op1) &= MMXUQ(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F DC */
void BX_CPU_C::PADDUSB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUB0(result) = SaturateWordSToByteU(Bit16s(MMXUB0(op1)) + Bit16s(MMXUB0(op2)));
  MMXUB1(result) = SaturateWordSToByteU(Bit16s(MMXUB1(op1)) + Bit16s(MMXUB1(op2)));
  MMXUB2(result) = SaturateWordSToByteU(Bit16s(MMXUB2(op1)) + Bit16s(MMXUB2(op2)));
  MMXUB3(result) = SaturateWordSToByteU(Bit16s(MMXUB3(op1)) + Bit16s(MMXUB3(op2)));
  MMXUB4(result) = SaturateWordSToByteU(Bit16s(MMXUB4(op1)) + Bit16s(MMXUB4(op2)));
  MMXUB5(result) = SaturateWordSToByteU(Bit16s(MMXUB5(op1)) + Bit16s(MMXUB5(op2)));
  MMXUB6(result) = SaturateWordSToByteU(Bit16s(MMXUB6(op1)) + Bit16s(MMXUB6(op2)));
  MMXUB7(result) = SaturateWordSToByteU(Bit16s(MMXUB7(op1)) + Bit16s(MMXUB7(op2)));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F DD */
void BX_CPU_C::PADDUSW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUW0(result) = SaturateDwordSToWordU(Bit32s(MMXUW0(op1)) + Bit32s(MMXUW0(op2)));
  MMXUW1(result) = SaturateDwordSToWordU(Bit32s(MMXUW1(op1)) + Bit32s(MMXUW1(op2)));
  MMXUW2(result) = SaturateDwordSToWordU(Bit32s(MMXUW2(op1)) + Bit32s(MMXUW2(op2)));
  MMXUW3(result) = SaturateDwordSToWordU(Bit32s(MMXUW3(op1)) + Bit32s(MMXUW3(op2)));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F DE */
void BX_CPU_C::PMAXUB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
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

  MMXUB0(result) = (MMXUB0(op1) > MMXUB0(op2)) ? MMXUB0(op1) : MMXUB0(op2);
  MMXUB1(result) = (MMXUB1(op1) > MMXUB1(op2)) ? MMXUB1(op1) : MMXUB1(op2);
  MMXUB2(result) = (MMXUB2(op1) > MMXUB2(op2)) ? MMXUB2(op1) : MMXUB2(op2);
  MMXUB3(result) = (MMXUB3(op1) > MMXUB3(op2)) ? MMXUB3(op1) : MMXUB3(op2);
  MMXUB4(result) = (MMXUB4(op1) > MMXUB4(op2)) ? MMXUB4(op1) : MMXUB4(op2);
  MMXUB4(result) = (MMXUB5(op1) > MMXUB5(op2)) ? MMXUB5(op1) : MMXUB5(op2);
  MMXUB5(result) = (MMXUB6(op1) > MMXUB6(op2)) ? MMXUB6(op1) : MMXUB6(op2);
  MMXUB6(result) = (MMXUB7(op1) > MMXUB7(op2)) ? MMXUB7(op1) : MMXUB7(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else  
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F DF */
void BX_CPU_C::PANDN_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXUQ(result) = ~(MMXUQ(op1)) & MMXUQ(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F E0 */
void BX_CPU_C::PAVGB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
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

  MMXUB0(result) = (MMXUB0(op1) + MMXUB0(op2) + 1) >> 1;
  MMXUB1(result) = (MMXUB1(op1) + MMXUB1(op2) + 1) >> 1;
  MMXUB2(result) = (MMXUB2(op1) + MMXUB2(op2) + 1) >> 1;
  MMXUB3(result) = (MMXUB3(op1) + MMXUB3(op2) + 1) >> 1;
  MMXUB4(result) = (MMXUB4(op1) + MMXUB4(op2) + 1) >> 1;
  MMXUB5(result) = (MMXUB5(op1) + MMXUB5(op2) + 1) >> 1;
  MMXUB6(result) = (MMXUB6(op1) + MMXUB6(op2) + 1) >> 1;
  MMXUB7(result) = (MMXUB7(op1) + MMXUB7(op2) + 1) >> 1;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else  
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F E1 */
void BX_CPU_C::PSRAW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  Bit8u shift = MMXUB0(op2);

  if(MMXUQ(op2) > 15) {
    MMXUW0(result) = (MMXUW0(op1) & 0x8000) ? 0xffff : 0;
    MMXUW1(result) = (MMXUW1(op1) & 0x8000) ? 0xffff : 0;
    MMXUW2(result) = (MMXUW2(op1) & 0x8000) ? 0xffff : 0;
    MMXUW3(result) = (MMXUW3(op1) & 0x8000) ? 0xffff : 0;
  }
  else {
    MMXUW0(result) = MMXUW0(op1) >> shift;
    MMXUW1(result) = MMXUW1(op1) >> shift;
    MMXUW2(result) = MMXUW2(op1) >> shift;
    MMXUW3(result) = MMXUW3(op1) >> shift;

    if(MMXUW0(op1) & 0x8000) MMXUW0(result) |= (0xffff << (16 - shift));
    if(MMXUW1(op1) & 0x8000) MMXUW1(result) |= (0xffff << (16 - shift));
    if(MMXUW2(op1) & 0x8000) MMXUW2(result) |= (0xffff << (16 - shift));
    if(MMXUW3(op1) & 0x8000) MMXUW3(result) |= (0xffff << (16 - shift));
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F E2 */
void BX_CPU_C::PSRAD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  Bit8u shift = MMXUB0(op2);

  if(MMXUQ(op2) > 31) {
    MMXUD0(result) = (MMXUD0(op1) & 0x80000000) ? 0xffffffff : 0;
    MMXUD1(result) = (MMXUD1(op1) & 0x80000000) ? 0xffffffff : 0;
  }
  else {
    MMXUD0(result) = MMXUD0(op1) >> shift;
    MMXUD1(result) = MMXUD1(op1) >> shift;

    if(MMXUD0(op1) & 0x80000000) 
       MMXUD0(result) |= (0xffffffff << (32 - shift));

    if(MMXUD1(op1) & 0x80000000) 
       MMXUD1(result) |= (0xffffffff << (32 - shift));
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F E3 */
void BX_CPU_C::PAVGW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
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

  MMXUW0(result) = (MMXUW0(op1) + MMXUW0(op2) + 1) >> 1;
  MMXUW1(result) = (MMXUW1(op1) + MMXUW1(op2) + 1) >> 1;
  MMXUW2(result) = (MMXUW2(op1) + MMXUW2(op2) + 1) >> 1;
  MMXUW3(result) = (MMXUW3(op1) + MMXUW3(op2) + 1) >> 1;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else  
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F E4 */
void BX_CPU_C::PMULHUW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
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

  Bit32u product1 = (Bit32u)(MMXUW0(op1)) * (Bit32u)(MMXUW0(op2));
  Bit32u product2 = (Bit32u)(MMXUW1(op1)) * (Bit32u)(MMXUW1(op2));
  Bit32u product3 = (Bit32u)(MMXUW2(op1)) * (Bit32u)(MMXUW2(op2));
  Bit32u product4 = (Bit32u)(MMXUW3(op1)) * (Bit32u)(MMXUW3(op2));

  MMXUW0(result) = (Bit16u)(product1 >> 16);
  MMXUW1(result) = (Bit16u)(product2 >> 16);
  MMXUW2(result) = (Bit16u)(product3 >> 16);
  MMXUW3(result) = (Bit16u)(product4 >> 16);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F E5 */
void BX_CPU_C::PMULHW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  Bit32s product1 = (Bit32s)(MMXSW0(op1)) * (Bit32s)(MMXSW0(op2));
  Bit32s product2 = (Bit32s)(MMXSW1(op1)) * (Bit32s)(MMXSW1(op2));
  Bit32s product3 = (Bit32s)(MMXSW2(op1)) * (Bit32s)(MMXSW2(op2));
  Bit32s product4 = (Bit32s)(MMXSW3(op1)) * (Bit32s)(MMXSW3(op2));

  MMXUW0(result) = (Bit16u)(product1 >> 16);
  MMXUW1(result) = (Bit16u)(product2 >> 16);
  MMXUW2(result) = (Bit16u)(product3 >> 16);
  MMXUW3(result) = (Bit16u)(product4 >> 16);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F E8 */
void BX_CPU_C::PSUBSB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXSB0(result) = SaturateWordSToByteS(Bit16s(MMXSB0(op1)) - Bit16s(MMXSB0(op2)));
  MMXSB1(result) = SaturateWordSToByteS(Bit16s(MMXSB1(op1)) - Bit16s(MMXSB1(op2)));
  MMXSB2(result) = SaturateWordSToByteS(Bit16s(MMXSB2(op1)) - Bit16s(MMXSB2(op2)));
  MMXSB3(result) = SaturateWordSToByteS(Bit16s(MMXSB3(op1)) - Bit16s(MMXSB3(op2)));
  MMXSB4(result) = SaturateWordSToByteS(Bit16s(MMXSB4(op1)) - Bit16s(MMXSB4(op2)));
  MMXSB5(result) = SaturateWordSToByteS(Bit16s(MMXSB5(op1)) - Bit16s(MMXSB5(op2)));
  MMXSB6(result) = SaturateWordSToByteS(Bit16s(MMXSB6(op1)) - Bit16s(MMXSB6(op2)));
  MMXSB7(result) = SaturateWordSToByteS(Bit16s(MMXSB7(op1)) - Bit16s(MMXSB7(op2)));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F E9 */
void BX_CPU_C::PSUBSW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXSW0(result) = SaturateDwordSToWordS(Bit32s(MMXSW0(op1)) - Bit32s(MMXSW0(op2)));
  MMXSW1(result) = SaturateDwordSToWordS(Bit32s(MMXSW1(op1)) - Bit32s(MMXSW1(op2)));
  MMXSW2(result) = SaturateDwordSToWordS(Bit32s(MMXSW2(op1)) - Bit32s(MMXSW2(op2)));
  MMXSW3(result) = SaturateDwordSToWordS(Bit32s(MMXSW3(op1)) - Bit32s(MMXSW3(op2)));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F EA */
void BX_CPU_C::PMINSW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
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

  MMXSW0(result) = (MMXSW0(op1) < MMXSW0(op2)) ? MMXSW0(op1) : MMXSW0(op2);
  MMXSW1(result) = (MMXSW1(op1) < MMXSW1(op2)) ? MMXSW1(op1) : MMXSW1(op2);
  MMXSW2(result) = (MMXSW2(op1) < MMXSW2(op2)) ? MMXSW2(op1) : MMXSW2(op2);
  MMXSW3(result) = (MMXSW3(op1) < MMXSW3(op2)) ? MMXSW3(op1) : MMXSW3(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else  
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F EB */
void BX_CPU_C::POR_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUQ(op1) |= MMXUQ(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F EC */
void BX_CPU_C::PADDSB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXSB0(result) = SaturateWordSToByteS(Bit16s(MMXSB0(op1)) + Bit16s(MMXSB0(op2)));
  MMXSB1(result) = SaturateWordSToByteS(Bit16s(MMXSB1(op1)) + Bit16s(MMXSB1(op2)));
  MMXSB2(result) = SaturateWordSToByteS(Bit16s(MMXSB2(op1)) + Bit16s(MMXSB2(op2)));
  MMXSB3(result) = SaturateWordSToByteS(Bit16s(MMXSB3(op1)) + Bit16s(MMXSB3(op2)));
  MMXSB4(result) = SaturateWordSToByteS(Bit16s(MMXSB4(op1)) + Bit16s(MMXSB4(op2)));
  MMXSB5(result) = SaturateWordSToByteS(Bit16s(MMXSB5(op1)) + Bit16s(MMXSB5(op2)));
  MMXSB6(result) = SaturateWordSToByteS(Bit16s(MMXSB6(op1)) + Bit16s(MMXSB6(op2)));
  MMXSB7(result) = SaturateWordSToByteS(Bit16s(MMXSB7(op1)) + Bit16s(MMXSB7(op2)));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F ED */
void BX_CPU_C::PADDSW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  MMXSW0(result) = SaturateDwordSToWordS(Bit32s(MMXSW0(op1)) + Bit32s(MMXSW0(op2)));
  MMXSW1(result) = SaturateDwordSToWordS(Bit32s(MMXSW1(op1)) + Bit32s(MMXSW1(op2)));
  MMXSW2(result) = SaturateDwordSToWordS(Bit32s(MMXSW2(op1)) + Bit32s(MMXSW2(op2)));
  MMXSW3(result) = SaturateDwordSToWordS(Bit32s(MMXSW3(op1)) + Bit32s(MMXSW3(op2)));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F EE */
void BX_CPU_C::PMAXSW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
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

  MMXSW0(result) = (MMXSW0(op1) > MMXSW0(op2)) ? MMXSW0(op1) : MMXSW0(op2);
  MMXSW1(result) = (MMXSW1(op1) > MMXSW1(op2)) ? MMXSW1(op1) : MMXSW1(op2);
  MMXSW2(result) = (MMXSW2(op1) > MMXSW2(op2)) ? MMXSW2(op1) : MMXSW2(op2);
  MMXSW3(result) = (MMXSW3(op1) > MMXSW3(op2)) ? MMXSW3(op1) : MMXSW3(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else  
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F EF */
void BX_CPU_C::PXOR_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUQ(op1) ^= MMXUQ(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F F1 */
void BX_CPU_C::PSLLW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  if(MMXUQ(op2) > 15) MMXUQ(op1) = 0;
  else
  {
    Bit8u shift = MMXUB0(op2);

    MMXUW0(op1) <<= shift;
    MMXUW1(op1) <<= shift;
    MMXUW2(op1) <<= shift;
    MMXUW3(op1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F F2 */
void BX_CPU_C::PSLLD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  if(MMXUQ(op2) > 31) MMXUQ(op1) = 0;
  else
  {
    Bit8u shift = MMXUB0(op2);

    MMXUD0(op1) <<= shift;
    MMXUD1(op1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F F3 */
void BX_CPU_C::PSLLQ_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  if(MMXUQ(op2) > 63) {
    MMXUQ(op1) = 0;
  }
  else {
    MMXUQ(op1) <<= MMXUB0(op2);
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F F4 */
void BX_CPU_C::PMULUDQ_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
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

  MMXUQ(result) = Bit64u(MMXUD0(op1)) * Bit64u(MMXUD0(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F F5 */
void BX_CPU_C::PMADDWD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
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

  if(MMXUW0(op1) == 0x80008000 && MMXUW0(op2) == 0x80008000) {
    MMXUD0(result) = 0x80000000;
  }
  else {
    MMXUD0(result) = Bit32s(MMXSW0(op1))*Bit32s(MMXSW0(op2)) + Bit32s(MMXSW1(op1))*Bit32s(MMXSW1(op2));
  }

  if(MMXUW2(op1) == 0x80008000 && MMXUW2(op2) == 0x80008000) {
    MMXUD1(result) = 0x80000000;
  }
  else {
    MMXUD1(result) = Bit32s(MMXSW2(op1))*Bit32s(MMXSW2(op2)) + Bit32s(MMXSW3(op1))*Bit32s(MMXSW3(op2));
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F F6 */
void BX_CPU_C::PSADBW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2, result;
  Bit16u temp = 0;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  temp += abs(MMXUB0(op1) - MMXUB0(op2));
  temp += abs(MMXUB1(op1) - MMXUB1(op2));
  temp += abs(MMXUB2(op1) - MMXUB2(op2));
  temp += abs(MMXUB3(op1) - MMXUB3(op2));
  temp += abs(MMXUB4(op1) - MMXUB4(op2));
  temp += abs(MMXUB5(op1) - MMXUB5(op2));
  temp += abs(MMXUB6(op1) - MMXUB6(op2));
  temp += abs(MMXUB7(op1) - MMXUB7(op2));

  MMXUW0(result) = Bit64u(temp);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), result);
#else  
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F F8 */
void BX_CPU_C::PSUBB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUB0(op1) -= MMXUB0(op2);
  MMXUB1(op1) -= MMXUB1(op2);
  MMXUB2(op1) -= MMXUB2(op2);
  MMXUB3(op1) -= MMXUB3(op2);
  MMXUB4(op1) -= MMXUB4(op2);
  MMXUB5(op1) -= MMXUB5(op2);
  MMXUB6(op1) -= MMXUB6(op2);
  MMXUB7(op1) -= MMXUB7(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F F9 */
void BX_CPU_C::PSUBW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUW0(op1) -= MMXUW0(op2);
  MMXUW1(op1) -= MMXUW1(op2);
  MMXUW2(op1) -= MMXUW2(op2);
  MMXUW3(op1) -= MMXUW3(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F FA */
void BX_CPU_C::PSUBD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUD0(op1) -= MMXUD0(op2);
  MMXUD1(op1) -= MMXUD1(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F FB */
void BX_CPU_C::PSUBQ_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUQ(op1) -= MMXUQ(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F FC */
void BX_CPU_C::PADDB_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }


  MMXUB0(op1) += MMXUB0(op2);
  MMXUB1(op1) += MMXUB1(op2);
  MMXUB2(op1) += MMXUB2(op2);
  MMXUB3(op1) += MMXUB3(op2);
  MMXUB4(op1) += MMXUB4(op2);
  MMXUB5(op1) += MMXUB5(op2);
  MMXUB6(op1) += MMXUB6(op2);
  MMXUB7(op1) += MMXUB7(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F FD */
void BX_CPU_C::PADDW_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUW0(op1) += MMXUW0(op2);
  MMXUW1(op1) += MMXUW1(op2);
  MMXUW2(op1) += MMXUW2(op2);
  MMXUW3(op1) += MMXUW3(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F FE */
void BX_CPU_C::PADDD_PqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  MMXUD0(op1) += MMXUD0(op2);
  MMXUD1(op1) += MMXUD1(op2);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->nnn(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 71 GrpA 010 */
void BX_CPU_C::PSRLW_PqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 15) MMXUQ(op1) = 0;
  else
  {
    MMXUW0(op1) >>= shift;
    MMXUW1(op1) >>= shift;
    MMXUW2(op1) >>= shift;
    MMXUW3(op1) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->rm(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 71 GrpA 100 */
void BX_CPU_C::PSRAW_PqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  if(shift > 15) {
    MMXUW0(result) = (MMXUW0(op1) & 0x8000) ? 0xffff : 0;
    MMXUW1(result) = (MMXUW1(op1) & 0x8000) ? 0xffff : 0;
    MMXUW2(result) = (MMXUW2(op1) & 0x8000) ? 0xffff : 0;
    MMXUW3(result) = (MMXUW3(op1) & 0x8000) ? 0xffff : 0;
  }
  else {
    MMXUW0(result) = MMXUW0(op1) >> shift;
    MMXUW1(result) = MMXUW1(op1) >> shift;
    MMXUW2(result) = MMXUW2(op1) >> shift;
    MMXUW3(result) = MMXUW3(op1) >> shift;

    if(MMXUW0(op1) & 0x8000) MMXUW0(result) |= (0xffff << (16 - shift));
    if(MMXUW1(op1) & 0x8000) MMXUW1(result) |= (0xffff << (16 - shift));
    if(MMXUW2(op1) & 0x8000) MMXUW2(result) |= (0xffff << (16 - shift));
    if(MMXUW3(op1) & 0x8000) MMXUW3(result) |= (0xffff << (16 - shift));
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->rm(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 71 GrpA 110 */
void BX_CPU_C::PSLLW_PqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 15) MMXUQ(op1) = 0;
  else
  {
    MMXUW0(op1) <<= shift;
    MMXUW1(op1) <<= shift;
    MMXUW2(op1) <<= shift;
    MMXUW3(op1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->rm(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}


/* 0F 72 GrpA 010 */
void BX_CPU_C::PSRLD_PqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 31) MMXUQ(op1) = 0;
  else
  {
    MMXUD0(op1) <<= shift;
    MMXUD1(op1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->rm(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 72 GrpA 100 */
void BX_CPU_C::PSRAD_PqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  if(shift > 31) {
    MMXUD0(result) = (MMXUD0(op1) & 0x80000000) ? 0xffffffff : 0;
    MMXUD1(result) = (MMXUD1(op1) & 0x80000000) ? 0xffffffff : 0;
  }
  else {
    MMXUD0(result) = MMXUD0(op1) >> shift;
    MMXUD1(result) = MMXUD1(op1) >> shift;

    if(MMXUD0(op1) & 0x80000000) 
       MMXUD0(result) |= (0xffffffff << (32 - shift));

    if(MMXUD1(op1) & 0x80000000) 
       MMXUD1(result) |= (0xffffffff << (32 - shift));
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->rm(), result);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 72 GrpA 110 */
void BX_CPU_C::PSLLD_PqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 31) MMXUQ(op1) = 0;
  else
  {
    MMXUD0(op1) <<= shift;
    MMXUD1(op1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->rm(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 73 GrpA 010 */
void BX_CPU_C::PSRLQ_PqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 63) {
    MMXUQ(op1) = 0;
  }
  else {
    MMXUQ(op1) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->rm(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 73 GrpA 110 */
void BX_CPU_C::PSLLQ_PqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_MMX
  BX_CPU_THIS_PTR prepareMMX();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->rm());
  Bit8u shift = i->Ib();

  if(shift > 63) {
    MMXUQ(op1) = 0;
  }
  else {
    MMXUQ(op1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->rm(), op1);
#else
  BX_INFO(("MMX instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}
