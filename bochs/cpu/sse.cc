/////////////////////////////////////////////////6////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2023 Stanislav Shwartsman
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

#if BX_CPU_LEVEL >= 6

#include "simd_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFB_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), result;

  xmm_pshufb(&result, &op1, &BX_XMM_REG(i->src()));

  BX_WRITE_XMM_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PBLENDVB_VdqWdqR(bxInstruction_c *i)
{
  xmm_pblendvb(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDVPS_VpsWpsR(bxInstruction_c *i)
{
  xmm_blendvps(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDVPD_VpdWpdR(bxInstruction_c *i)
{
  xmm_blendvpd(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PTEST_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  clearEFlagsOSZAPC();

  if ((op2.xmm64u(0) &  op1.xmm64u(0)) == 0 &&
      (op2.xmm64u(1) &  op1.xmm64u(1)) == 0) assert_ZF();

  if ((op2.xmm64u(0) & ~op1.xmm64u(0)) == 0 &&
      (op2.xmm64u(1) & ~op1.xmm64u(1)) == 0) assert_CF();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PHMINPOSUW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  unsigned min_index = 0;

  for (unsigned j=1; j < 8; j++) {
     if (op.xmm16u(j) < op.xmm16u(min_index)) min_index = j;
  }

  op.xmm16u(0) = op.xmm16u(min_index);
  op.xmm16u(1) = min_index;
  op.xmm32u(1) = 0;
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REGZ(i->dst(), op, i->getVL());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDPS_VpsWpsIbR(bxInstruction_c *i)
{
  xmm_blendps(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDPD_VpdWpdIbR(bxInstruction_c *i)
{
  xmm_blendpd(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PBLENDW_VdqWdqIbR(bxInstruction_c *i)
{
  xmm_pblendw(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRB_EdVdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit8u result = op.xmmubyte(i->Ib() & 0xF);
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRB_MbVdqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit8u result = op.xmmubyte(i->Ib() & 0xF);

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_byte(i->seg(), eaddr, result);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_EdVdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit16u result = op.xmm16u(i->Ib() & 7);
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_MwVdqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit16u result = op.xmm16u(i->Ib() & 7);

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_word(i->seg(), eaddr, result);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRD_EdVdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit32u result = op.xmm32u(i->Ib() & 3);
  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRD_EdVdqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit32u result = op.xmm32u(i->Ib() & 3);
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_dword(i->seg(), eaddr, result);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRQ_EqVdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit64u result = op.xmm64u(i->Ib() & 1);
  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRQ_EqVdqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit64u result = op.xmm64u(i->Ib() & 1);
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
  write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), result);

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRB_VdqEbIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  op1.xmmubyte(i->Ib() & 0xF) = BX_READ_8BIT_REGL(i->src()); // won't allow reading of AH/CH/BH/DH
  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRB_VdqEbIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  op1.xmmubyte(i->Ib() & 0xF) = read_virtual_byte(i->seg(), eaddr);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRD_VdqEdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  op1.xmm32u(i->Ib() & 3) = BX_READ_32BIT_REG(i->src());
  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRD_VdqEdIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  op1.xmm32u(i->Ib() & 3) = read_virtual_dword(i->seg(), eaddr);

  BX_WRITE_XMM_REG(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRQ_VdqEqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  op1.xmm64u(i->Ib() & 1) = BX_READ_64BIT_REG(i->src());
  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRQ_VdqEqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit64u op2 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  op1.xmm64u(i->Ib() & 1) = op2;

  BX_WRITE_XMM_REG(i->dst(), op1);
  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTPS_VpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  Bit8u control = i->Ib();

  BxPackedXmmRegister tmp = BX_READ_XMM_REG(i->src());
  Bit32u op2 = tmp.xmm32u((control >> 6) & 3);

  op1.xmm32u((control >> 4) & 3) = op2;
  xmm_zero_blendps(&op1, &op1, ~control);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTPS_VpsWssIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  Bit8u control = i->Ib();

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  op1.xmm32u((control >> 4) & 3) = read_virtual_dword(i->seg(), eaddr);
  xmm_zero_blendps(&op1, &op1, ~control);

  BX_WRITE_XMM_REG(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MPSADBW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  xmm_mpsadbw(&result, &op1, &op2, i->Ib() & 0x7);

  BX_WRITE_XMM_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
}

#endif // BX_CPU_LEVEL >= 6

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFD_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  xmm_shufps(&result, &op, &op, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFHW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  xmm_pshufhw(&result, &op, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFLW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  xmm_pshuflw(&result, &op, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRW_VdqEwIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  op1.xmm16u(i->Ib() & 0x7) = BX_READ_16BIT_REG(i->src());
  BX_WRITE_XMM_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRW_VdqEwIbM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  op1.xmm16u(i->Ib() & 0x7) = read_virtual_word(i->seg(), eaddr);

  BX_WRITE_XMM_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_GdUdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit8u count = i->Ib() & 0x7;
  Bit32u result = (Bit32u) op.xmm16u(count);
  BX_WRITE_32BIT_REGZ(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHUFPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  xmm_shufps(&result, &op1, &op2, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHUFPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  xmm_shufpd(&result, &op1, &op2, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/* ************************ */
/* SSE4A (AMD) INSTRUCTIONS */
/* ************************ */

#if BX_CPU_LEVEL >= 6
BX_CPP_INLINE Bit64u xmm_extrq(Bit64u src, unsigned shift, unsigned len)
{
  len   &= 0x3f;
  shift &= 0x3f;

  src >>= shift;
  if (len) {
    Bit64u mask = (BX_CONST64(1) << len) - 1;
    return src & mask;
  }

  return src;
}

BX_CPP_INLINE Bit64u xmm_insertq(Bit64u dest, Bit64u src, unsigned shift, unsigned len)
{
  Bit64u mask;

  len   &= 0x3f;
  shift &= 0x3f;

  if (len == 0) {
    mask = BX_CONST64(0xffffffffffffffff);
  } else {
    mask = (BX_CONST64(1) << len) - 1;
  }

  return (dest & ~(mask << shift)) | ((src & mask) << shift);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRQ_UdqIbIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), xmm_extrq(BX_READ_XMM_REG_LO_QWORD(i->dst()), i->Ib2(), i->Ib()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRQ_VdqUq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u ctrl = BX_READ_XMM_REG_LO_WORD(i->src());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), xmm_extrq(BX_READ_XMM_REG_LO_QWORD(i->dst()), ctrl >> 8, ctrl));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTQ_VdqUqIbIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit64u dst = BX_READ_XMM_REG_LO_QWORD(i->dst()), src = BX_READ_XMM_REG_LO_QWORD(i->src());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), xmm_insertq(dst, src, i->Ib2(), i->Ib()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTQ_VdqUdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister src = BX_READ_XMM_REG(i->src());

  Bit64u dst = BX_READ_XMM_REG_LO_QWORD(i->dst());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), xmm_insertq(dst, src.xmm64u(0), src.xmmubyte(9), src.xmmubyte(8)));
#endif

  BX_NEXT_INSTR(i);
}
