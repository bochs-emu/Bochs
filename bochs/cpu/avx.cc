/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2013 Stanislav Shwartsman
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

#if BX_SUPPORT_AVX

#include "simd_int.h"

/* VZEROUPPER: VEX.128.0F.77 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VZEROUPPER(bxInstruction_c *i)
{
  for(unsigned index=0; index < BX_XMM_REGISTERS; index++)
  {
    if (index < 8 || long64_mode())
      BX_CLEAR_AVX_HIGH128(index);
  }

  BX_NEXT_INSTR(i);
}

/* VZEROALL: VEX.256.0F.77 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VZEROALL(bxInstruction_c *i)
{
  // prepare empty AVX register - zeroed by compiler because of static variable
  static BxPackedXmmRegister xmmnil;

  for(unsigned index=0; index < BX_XMM_REGISTERS; index++)
  {
    if (index < 8 || long64_mode())
      BX_WRITE_XMM_REG_CLEAR_HIGH(index, xmmnil);
  }

  BX_NEXT_INSTR(i);
}

/* VMOVSS: VEX.F3.0F 10 (VEX.W ignore) */ 
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src2());

  op.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->src1());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* VMOVSS: VEX.F2.0F 10 (VEX.W ignore) */ 
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->src1());
  op.xmm64u(1) = BX_READ_XMM_REG_HI_QWORD(i->src2());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* VMOVAPS: VEX    0F 28 (VEX.W ignore, VEX.VVV #UD) */ 
/* VMOVAPD: VEX.66.0F 28 (VEX.W ignore, VEX.VVV #UD) */
/* VMOVDQA: VEX.66.0F 6F (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_VpsWpsR(bxInstruction_c *i)
{
  BX_WRITE_AVX_REGZ(i->dst(), BX_READ_AVX_REG(i->src()), i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_VpsWpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  unsigned len = i->getVL();

#if BX_SUPORT_EVEX
  if (len == BX_VL512)
    read_virtual_zmmword_aligned(i->seg(), eaddr, &BX_READ_AVX_REG(i->dst()));
  else
#endif
  {
    if (len == BX_VL256) {
      read_virtual_ymmword_aligned(i->seg(), eaddr, &BX_READ_YMM_REG(i->dst()));
      BX_CLEAR_AVX_HIGH256(i->dst());
    }
    else {
      read_virtual_xmmword_aligned(i->seg(), eaddr, &BX_READ_XMM_REG(i->dst()));
      BX_CLEAR_AVX_HIGH128(i->dst());
    }
  }

  BX_NEXT_INSTR(i);
}

/* VMOVUPS: VEX    0F 10 (VEX.W ignore, VEX.VVV #UD) */ 
/* VMOVUPD: VEX.66.0F 10 (VEX.W ignore, VEX.VVV #UD) */
/* VMOVDQU: VEX.F3.0F 6F (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPS_VpsWpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  unsigned len = i->getVL();

#if BX_SUPORT_EVEX
  if (len == BX_VL512)
    read_virtual_zmmword(i->seg(), eaddr, &BX_READ_AVX_REG(i->dst()));
  else
#endif
  {
    if (len == BX_VL256) {
      read_virtual_ymmword(i->seg(), eaddr, &BX_READ_YMM_REG(i->dst()));
      BX_CLEAR_AVX_HIGH256(i->dst());
    }
    else {
      read_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(i->dst()));
      BX_CLEAR_AVX_HIGH128(i->dst());
    }
  }

  BX_NEXT_INSTR(i);
}

/* VMOVUPS: VEX    0F 11 (VEX.W ignore, VEX.VVV #UD) */ 
/* VMOVUPD: VEX.66.0F 11 (VEX.W ignore, VEX.VVV #UD) */
/* VMOVDQU: VEX.66.0F 7F (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPS_WpsVpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  unsigned len = i->getVL();

#if BX_SUPORT_EVEX
  if (len == BX_VL512)
    write_virtual_zmmword(i->seg(), eaddr, &BX_READ_AVX_REG(i->src()));
  else
#endif
  {
    if (len == BX_VL256)
      write_virtual_ymmword(i->seg(), eaddr, &BX_READ_YMM_REG(i->src()));
    else
      write_virtual_xmmword(i->seg(), eaddr, &BX_READ_XMM_REG(i->src()));
  }

  BX_NEXT_INSTR(i);
}

/* VMOVAPS: VEX    0F 29 (VEX.W ignore, VEX.VVV #UD) */ 
/* VMOVAPD: VEX.66.0F 29 (VEX.W ignore, VEX.VVV #UD) */
/* VMOVDQA: VEX.66.0F 7F (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_WpsVpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  unsigned len = i->getVL();

#if BX_SUPORT_EVEX
  if (len == BX_VL512)
    write_virtual_zmmword_aligned(i->seg(), eaddr, &BX_READ_AVX_REG(i->src()));
  else
#endif
  {
    if (len == BX_VL256)
      write_virtual_ymmword_aligned(i->seg(), eaddr, &BX_READ_YMM_REG(i->src()));
    else
      write_virtual_xmmword_aligned(i->seg(), eaddr, &BX_READ_XMM_REG(i->src()));
  }

  BX_NEXT_INSTR(i);
}

/* VEX.F2.0F 12 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDDUP_VpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n+=2) {
    op.ymm64u(n+1) = op.ymm64u(n);
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* VEX.F3.0F 12 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSLDUP_VpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (4*len); n+=2) {
    op.ymm32u(n+1) = op.ymm32u(n);
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* VEX.F3.0F 12 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSHDUP_VpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (4*len); n+=2) {
    op.ymm32u(n) = op.ymm32u(n+1);
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* VEX.0F 12 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVHLPS_VpsHpsWps(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_HI_QWORD(i->src2());
  op.xmm64u(1) = BX_READ_XMM_REG_HI_QWORD(i->src1());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* VEX.66.0F 12 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVLPD_VpdHpdMq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BxPackedXmmRegister op;

  op.xmm64u(0) = read_virtual_qword(i->seg(), eaddr);
  op.xmm64u(1) = BX_READ_XMM_REG_HI_QWORD(i->src1());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* VEX.0F 16 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVLHPS_VpsHpsWps(bxInstruction_c *i)
{
  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->src1());
  op.xmm64u(1) = BX_READ_XMM_REG_LO_QWORD(i->src2());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* VEX.66.0F 16 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVHPD_VpdHpdMq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->src1());
  op.xmm64u(1) = read_virtual_qword(i->seg(), eaddr);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* VEX.0F 50 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVMSKPS_GdUps(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();
  Bit32u mask = 0;

  for (unsigned n=0; n < len; n++)
    mask |= xmm_pmovmskd(&op.ymm128(n)) << (4*n);

  BX_WRITE_32BIT_REGZ(i->dst(), mask);

  BX_NEXT_INSTR(i);
}

/* VEX.66.0F 50 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVMSKPD_GdUpd(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();
  Bit32u mask = 0;

  for (unsigned n=0; n < len; n++)
    mask |= xmm_pmovmskq(&op.ymm128(n)) << (2*n);

  BX_WRITE_32BIT_REGZ(i->dst(), mask);

  BX_NEXT_INSTR(i);
}

/* VEX.66.0F 50 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVMSKB_GdUdq(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();
  Bit32u mask = 0;

  for (unsigned n=0; n < len; n++)
    mask |= xmm_pmovmskb(&op.ymm128(n)) << (16*n);

  BX_WRITE_32BIT_REGZ(i->dst(), mask);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.0F.C6 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_shufps(&result.ymm128(n), &op1.ymm128(n), &op2.ymm128(n), i->Ib());

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.C6 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPD_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;

  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  for (unsigned n=0; n < len; n++) {
    xmm_shufpd(&result.ymm128(n), &op1.ymm128(n), &op2.ymm128(n), order);
    order >>= 2;
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38.17 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTEST_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->dst()), op2 = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  unsigned result = EFlagsZFMask | EFlagsCFMask;

  for (unsigned n=0; n < (2*len); n++) {
    if ((op2.ymm64u(n) &  op1.ymm64u(n)) != 0) result &= ~EFlagsZFMask;
    if ((op2.ymm64u(n) & ~op1.ymm64u(n)) != 0) result &= ~EFlagsCFMask;
  }

  setEFlagsOSZAPC(result);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.256.66.0F.38.1A (VEX.W=0, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF128_VdqMdq(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  BxPackedYmmRegister dst;
  BxPackedXmmRegister src;
  
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  read_virtual_xmmword(i->seg(), eaddr, (Bit8u*) &src);

  for (unsigned n=0; n < len; n++) {
    dst.ymm64u(n*2)   = src.xmm64u(0);
    dst.ymm64u(n*2+1) = src.xmm64u(1);
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 0C (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  for (unsigned n=0; n < len; n++) {
    xmm_blendps(&op1.ymm128(n), &op2.ymm128(n), mask);
    mask >>= 4;
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 0D (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDPD_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  for (unsigned n=0; n < len; n++) {
    xmm_blendpd(&op1.ymm128(n), &op2.ymm128(n), mask);
    mask >>= 2;
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 4A (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDVPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2()),
           mask = BX_READ_YMM_REG(i->src3());

  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_blendvps(&op1.ymm128(n), &op2.ymm128(n), &mask.ymm128(n));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 4B (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDVPD_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2()),
           mask = BX_READ_YMM_REG(i->src3());

  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_blendvpd(&op1.ymm128(n), &op2.ymm128(n), &mask.ymm128(n));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 4C (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBLENDVB_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2()),
           mask = BX_READ_YMM_REG(i->src3());
           
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pblendvb(&op1.ymm128(n), &op2.ymm128(n), &mask.ymm128(n));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 18 (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VINSERTF128_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());

  op1.ymm128(i->Ib() & 1) = BX_READ_XMM_REG(i->src2());

  BX_WRITE_YMM_REGZ(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 19 (VEX.W=0, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF128_WdqVdqIbM(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_xmmword(i->seg(), eaddr, &(op.ymm128(i->Ib() & 1)));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF128_WdqVdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op.ymm128(i->Ib() & 1));

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38 0C (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_permilps(&result.ymm128(n), &op1.ymm128(n), &op2.ymm128(n));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 05 (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_permilpd(&result.ymm128(n), &op1.ymm128(n), &op2.ymm128(n));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 04 (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPS_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_shufps(&result.ymm128(n), &op1.ymm128(n), &op1.ymm128(n), i->Ib());

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 05 (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPD_VpdWpdIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src()), result;

  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  for (unsigned n=0; n < len; n++) {
    xmm_shufpd(&result.ymm128(n), &op1.ymm128(n), &op1.ymm128(n), order);
    order >>= 2;
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A 06 (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERM2F128_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;
  Bit8u order = i->Ib();

  for (unsigned n=0;n<2;n++) {

    if (order & 0x8) {
      result.ymm64u(n*2) = result.ymm64u(n*2+1) = 0;
    }
    else {
      if (order & 0x2)
        result.ymm128(n) = op2.ymm128(order & 0x1);
      else
        result.ymm128(n) = op1.ymm128(order & 0x1);
    }

    order >>= 4;
  }

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38 2C (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMASKMOVPS_VpsHpsMps(bxInstruction_c *i)
{
  BxPackedYmmRegister mask = BX_READ_YMM_REG(i->src1()), result;
  unsigned len = i->getVL();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (4*len); n++) {
       if (mask.ymm32u(n) & 0x80000000) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 4*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (int n=4*len-1; n >= 0; n--) {
    if (mask.ymm32u(n) & 0x80000000)
       result.ymm32u(n) = read_virtual_dword(i->seg(), eaddr + 4*n);
    else
       result.ymm32u(n) = 0;
  }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38 2D (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMASKMOVPD_VpdHpdMpd(bxInstruction_c *i)
{
  BxPackedYmmRegister mask = BX_READ_YMM_REG(i->src1()), result;
  unsigned len = i->getVL();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (2*len); n++) {
       if (mask.ymm32u(n*2+1) & 0x80000000) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 8*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (int n=2*len-1; n >= 0; n--) {
    if (mask.ymm32u(n*2+1) & 0x80000000)
       result.ymm64u(n) = read_virtual_qword(i->seg(), eaddr + 8*n);
    else
       result.ymm64u(n) = 0;
  }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38 2C (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMASKMOVPS_MpsHpsVps(bxInstruction_c *i)
{
  BxPackedYmmRegister mask = BX_READ_YMM_REG(i->src1()), op = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (4*len); n++) {
       if (mask.ymm32u(n) & 0x80000000) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 4*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  // see you can successfully write all the elements first
  for (int n=4*len-1; n >= 0; n--) {
    if (mask.ymm32u(n) & 0x80000000)
       read_RMW_virtual_dword(i->seg(), eaddr + 4*n);
  }

  for (unsigned n=0; n < (4*len); n++) {
    if (mask.ymm32u(n) & 0x80000000)
       write_virtual_dword(i->seg(), eaddr + 4*n, op.ymm32u(n));
  }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38 2D (VEX.W=0) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMASKMOVPD_MpdHpdVpd(bxInstruction_c *i)
{
  BxPackedYmmRegister mask = BX_READ_YMM_REG(i->src1()), op = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (2*len); n++) {
       if (mask.ymm32u(n*2+1) & 0x80000000) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 8*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  // see you can successfully write all the elements first
  for (int n=2*len-1; n >= 0; n--) {
    if (mask.ymm32u(2*n+1) & 0x80000000)
       read_RMW_virtual_qword(i->seg(), eaddr + 8*n);
  }

  for (unsigned n=0; n < (2*len); n++) {
    if (mask.ymm32u(2*n+1) & 0x80000000)
       write_virtual_qword(i->seg(), eaddr + 8*n, op.ymm64u(n));
  }

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_AVX
