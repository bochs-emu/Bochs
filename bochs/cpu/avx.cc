/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011 Stanislav Shwartsman
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

/* VZEROUPPER: VEX.128.0F.77 (VEX.W ignore, VEX.VVV #UD) */
/* VZEROALL:   VEX.256.0F.77 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VZEROUPPER(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (i->getVL() == BX_VL128) {
    for(unsigned index=0; index < BX_XMM_REGISTERS; index++)
    {
      if (index < 8 || long64_mode())
        BX_CLEAR_AVX_HIGH(index);
    }
  }
  else {
    // prepare empty AVX register - zeroed by compiler because of static variable
    static BxPackedAvxRegister nil;
    
    for(unsigned index=0; index < BX_XMM_REGISTERS; index++)
    {
      if (index < 8 || long64_mode())
        BX_WRITE_AVX_REG(index, nil);
    }
  }
#endif
}

/* VMOVSS: VEX.F3.0F 10 (VEX.W ignore) */ 
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->vvv());

  op.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->rm());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op);
#endif
}

/* VMOVSS: VEX.F2.0F 10 (VEX.W ignore) */ 
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->rm());
  op.xmm64u(1) = BX_READ_XMM_REG_HI_QWORD(i->vvv());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op);
#endif
}

/* VMOVAPS: VEX    0F 28 (VEX.W ignore, VEX.VVV #UD) */ 
/* VMOVAPD: VEX.66.0F 28 (VEX.W ignore, VEX.VVV #UD) */
/* VMOVDQA: VEX.66.0F 6F (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_WRITE_AVX_REGZ(i->nnn(), BX_READ_AVX_REG(i->rm()), i->getVL());
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_VpsWpsM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op;
  unsigned len = i->getVL();
  
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  read_virtual_vector_aligned(i->seg(), eaddr, len << 2, &op);

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* VMOVUPS: VEX    0F 10 (VEX.W ignore, VEX.VVV #UD) */ 
/* VMOVUPD: VEX.66.0F 10 (VEX.W ignore, VEX.VVV #UD) */
/* VMOVDQU: VEX.F3.0F 6F (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPS_VpsWpsM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op;
  unsigned len = i->getVL();
  
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  read_virtual_vector(i->seg(), eaddr, len << 2, &op);

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* VMOVUPS: VEX    0F 11 (VEX.W ignore, VEX.VVV #UD) */ 
/* VMOVUPD: VEX.66.0F 11 (VEX.W ignore, VEX.VVV #UD) */
/* VMOVUQA: VEX.66.0F 7F (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPS_WpsVpsM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->nnn());
  
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  write_virtual_vector(i->seg(), eaddr, i->getVL() << 2, &op);
#endif
}

/* VMOVAPS: VEX    0F 29 (VEX.W ignore, VEX.VVV #UD) */ 
/* VMOVAPD: VEX.66.0F 29 (VEX.W ignore, VEX.VVV #UD) */
/* VMOVDQA: VEX.66.0F 7F (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_WpsVpsM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->nnn());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  write_virtual_vector_aligned(i->seg(), eaddr, i->getVL() << 2, &op);
#endif
}

/* VEX.F2.0F 12 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDDUP_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n+=2) {
    op.avx64u(n+1) = op.avx64u(n);
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* VEX.F3.0F 12 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSLDUP_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (4*len); n+=2) {
    op.avx32u(n+1) = op.avx32u(n);
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* VEX.F3.0F 12 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSHDUP_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (4*len); n+=2) {
    op.avx32u(n) = op.avx32u(n+1);
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* VEX.0F 12 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVHLPS_VpsWps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_HI_QWORD(i->rm());
  op.xmm64u(1) = BX_READ_XMM_REG_HI_QWORD(i->vvv());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op);
#endif
}

/* VEX.66.0F 12 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVLPD_VpdMq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BxPackedXmmRegister op;

  op.xmm64u(0) = read_virtual_qword(i->seg(), eaddr);
  op.xmm64u(1) = BX_READ_XMM_REG_HI_QWORD(i->vvv());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op);
#endif
}


/* VEX.0F 16 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVLHPS_VpsWps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->vvv());
  op.xmm64u(1) = BX_READ_XMM_REG_LO_QWORD(i->rm());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op);
#endif
}

/* VEX.66.0F 16 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVHPD_VpdMq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->vvv());
  op.xmm64u(1) = read_virtual_qword(i->seg(), eaddr);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op);
#endif
}

/* VUNPCKLPS:  VEX.   0F 14 (VEX.W ignore) */
/* VPUNPCKLDQ: VEX.66.0F 62 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VUNPCKLPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    op1.avx32u(n*4+3) = op2.avx32u(n*4+1);
    op1.avx32u(n*4+2) = op1.avx32u(n*4+1);
    op1.avx32u(n*4+1) = op2.avx32u(n*4);
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* VUNPCKHPS:  VEX.   0F 15 (VEX.W ignore) */
/* VPUNPCKHDQ: VEX.66.0F 6A (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VUNPCKHPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    op1.avx32u(n*4+0) = op1.avx32u(n*4+2);
    op1.avx32u(n*4+1) = op2.avx32u(n*4+2);
    op1.avx32u(n*4+2) = op1.avx32u(n*4+3);
    op1.avx32u(n*4+3) = op2.avx32u(n*4+3);
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* VUNPCKLPD:   VEX.66.0F 14 (VEX.W ignore) */
/* VPUNPCKLQDQ: VEX.66.0F 6C (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VUNPCKLPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    op1.avx64u(n*2+1) = op2.avx64u(n*2);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* VUNPCKLPD:   VEX.66.0F 15 (VEX.W ignore) */
/* VPUNPCKLQDQ: VEX.66.0F 6D (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VUNPCKHPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    op1.avx64u(n*2)   = op1.avx64u(n*2+1);
    op1.avx64u(n*2+1) = op2.avx64u(n*2+1);
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* VEX.0F 50 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVMSKPS_GdVRps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  Bit32u val32 = 0;

  for (unsigned n=0; n < (4*len); n++)
    if(op.avx32u(n) & 0x80000000) val32 |= (1 << n);

  BX_WRITE_32BIT_REGZ(i->nnn(), val32);
#endif
}

/* VEX.66.0F 50 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVMSKPD_GdVRpd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  Bit32u val32 = 0;

  for (unsigned n=0; n < (2*len); n++)
    if(op.avx32u(2*n+1) & 0x80000000) val32 |= (1 << n);

  BX_WRITE_32BIT_REGZ(i->nnn(), val32);
#endif
}

/* VEX.66.0F 50 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVMSKB_GdUdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  Bit32u val32 = 0;

  for (unsigned n=0; n < (16*len); n++)
    if(op.avxubyte(n) & 0x80) val32 |= (1 << n);

  BX_WRITE_32BIT_REGZ(i->nnn(), val32);
#endif
}

/* VANDPS: VEX    0F 54 (VEX.W ignore) */
/* VANDPD: VEX.66.0F 54 (VEX.W ignore) */
/* VPAND:  VEX.66.0F DB (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VANDPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n++)
    op1.avx64u(n) = op1.avx64u(n) & op2.avx64u(n);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* VANDNPS: VEX    0F 55 (VEX.W ignore) */
/* VANDNPD: VEX.66.0F 55 (VEX.W ignore) */
/* VPANDN:  VEX.66.0F DF (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VANDNPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n++)
    op1.avx64u(n) = ~(op1.avx64u(n)) & op2.avx64u(n);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* VORPS: VEX    0F 56 (VEX.W ignore) */
/* VORPD: VEX.66.0F 56 (VEX.W ignore) */
/* VPOR:  VEX.66.0F EB (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VORPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n++)
    op1.avx64u(n) = op1.avx64u(n) | op2.avx64u(n);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* VXORPS: VEX    0F 57 (VEX.W ignore) */
/* VXORPD: VEX.66.0F 57 (VEX.W ignore) */
/* VPXOR:  VEX.66.0F EF (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VXORPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  
  for (unsigned n=0; n < (2*len); n++)
    op1.avx64u(n) = op1.avx64u(n) ^ op2.avx64u(n);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.0F.C6 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->rm()), result;

  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  for (unsigned n=0; n < (4*len); n+=4) {
    result.avx32u(n)   = op1.avx32u(n + ((order >> 0) & 0x3));
    result.avx32u(n+1) = op1.avx32u(n + ((order >> 2) & 0x3));
    result.avx32u(n+2) = op2.avx32u(n + ((order >> 4) & 0x3));
    result.avx32u(n+3) = op2.avx32u(n + ((order >> 6) & 0x3));
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
#endif
}

/* Opcode: VEX.66.0F.C6 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->rm()), result;

  unsigned len = i->getVL();
  Bit8u order = i->Ib();
  
  for (unsigned n=0; n < (2*len); n+=2) {
    result.avx64u(n)   = op1.avx64u(n + ((order >> 0) & 0x1));
    result.avx64u(n+1) = op2.avx64u(n + ((order >> 1) & 0x1));
    order >>= 2;
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
#endif
}

/* Opcode: VEX.66.0F.38.17 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTEST_VdqWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->nnn()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  unsigned result = EFlagsZFMask | EFlagsCFMask;

  for (unsigned n=0; n < (2*len); n++) {
    if ((op2.avx64u(n) &  op1.avx64u(n)) != 0) result &= ~EFlagsZFMask;
    if ((op2.avx64u(n) & ~op1.avx64u(n)) != 0) result &= ~EFlagsCFMask;
  }

  setEFlagsOSZAPC(result);
#endif
}

/* Opcode: VEX.66.0F.38.18 /r (VEX.W=0, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTSS_VpsMss(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  unsigned len = i->getVL();
  BxPackedAvxRegister op;
  
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit32u val32 = read_virtual_dword(i->seg(), eaddr);

  for (unsigned n=0; n < len; n++) {
    op.avx32u(n*4)   = val32;
    op.avx32u(n*4+1) = val32;
    op.avx32u(n*4+2) = val32;
    op.avx32u(n*4+3) = val32;
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.256.66.0F.38.19 (VEX.W=0, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTSD_VpdMsd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  unsigned len = i->getVL();
  BxPackedAvxRegister op;
  
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit64u val64 = read_virtual_qword(i->seg(), eaddr);

  for (unsigned n=0; n < len; n++) {
    op.avx64u(n*2)   = val64;
    op.avx64u(n*2+1) = val64;
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.256.66.0F.38.1A (VEX.W=0, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF128_VdqMdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  unsigned len = i->getVL();
  BxPackedAvxRegister op;
  BxPackedXmmRegister src;
  
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  read_virtual_dqword(i->seg(), eaddr, (Bit8u*) &src);

  for (unsigned n=0; n < len; n++) {
    op.avx64u(n*2)   = src.xmm64u(0);
    op.avx64u(n*2+1) = src.xmm64u(1);
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.66.0F.3A 0C (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  for (unsigned n=0; n < (4*len); n++) {
    if (mask & 0x1) op1.avx32u(n) = op2.avx32u(n);
    mask >>= 1;
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.66.0F.3A 0D (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  for (unsigned n=0; n < (2*len); n++) {
    if (mask & 0x1) op1.avx64u(n) = op2.avx64u(n);
    mask >>= 1;
  }

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.66.0F.3A 4A (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDVPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm()),
           mask = BX_READ_AVX_REG(i->Ib());

  unsigned len = i->getVL();

  for (unsigned n=0; n < (4*len); n++)
    if (mask.avx32u(n) & 0x80000000) op1.avx32u(n) = op2.avx32u(n);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.66.0F.3A 4B (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDVPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm()),
           mask = BX_READ_AVX_REG(i->Ib());

  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n++)
    if (mask.avx32u(n*2+1) & 0x80000000) op1.avx64u(n) = op2.avx64u(n);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.66.0F.3A 4C (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBLENDVB_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm()),
           mask = BX_READ_AVX_REG(i->Ib());

  unsigned len = i->getVL();

  for(unsigned n=0; n<(16*len); n++)
     if (mask.avxubyte(n) & 0x80) op1.avxubyte(n) = op2.avxubyte(n);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.66.0F.3A 18 (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VINSERTF128_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->rm());

  op1.avx128(i->Ib() & 1) = op2;

  BX_WRITE_AVX_REG(i->nnn(), op1);
}

/* Opcode: VEX.66.0F.3A 19 (VEX.W=0, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF128_WdqVdqIbM(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->nnn());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_dqword(i->seg(), eaddr, &(op.avx128(i->Ib() & 1)));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF128_WdqVdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->nnn());

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->rm(), op.avx128(i->Ib() & 1));
}

/* Opcode: VEX.66.0F.38 0C (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->rm()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < (4*len); n+=4) {
    result.avx32u(n)   = op1.avx32u(n + (op2.avx32u(n)   & 0x3));
    result.avx32u(n+1) = op1.avx32u(n + (op2.avx32u(n+1) & 0x3));
    result.avx32u(n+2) = op1.avx32u(n + (op2.avx32u(n+2) & 0x3));
    result.avx32u(n+3) = op1.avx32u(n + (op2.avx32u(n+3) & 0x3));
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
#endif
}

/* Opcode: VEX.66.0F.3A 05 (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->rm()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n+=2) {
    result.avx64u(n)   = op1.avx64u(n + ((op2.avx32u(n*2+0) >> 1) & 0x1));
    result.avx64u(n+1) = op1.avx64u(n + ((op2.avx32u(n*2+2) >> 1) & 0x1));
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
#endif
}

/* Opcode: VEX.66.0F.3A 04 (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->rm()), result;

  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  for (unsigned n=0; n < (4*len); n+=4) {
    result.avx32u(n)   = op1.avx32u(n + ((order >> 0) & 0x3));
    result.avx32u(n+1) = op1.avx32u(n + ((order >> 2) & 0x3));
    result.avx32u(n+2) = op1.avx32u(n + ((order >> 4) & 0x3));
    result.avx32u(n+3) = op1.avx32u(n + ((order >> 6) & 0x3));
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
#endif
}

/* Opcode: VEX.66.0F.3A 05 (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->rm()), result;

  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  for (unsigned n=0; n < (2*len); n+=2) {
    result.avx64u(n)   = op1.avx64u(n + ((order >> 0) & 0x1));
    result.avx64u(n+1) = op1.avx64u(n + ((order >> 1) & 0x1));
    order >>= 2;
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
#endif
}

/* Opcode: VEX.66.0F.3A 06 (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERM2F128_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->rm()), result;
  Bit8u order = i->Ib();

  for (unsigned n=0;n<2;n++) {

    if (order & 0x8) {
      result.avx64u(n*2) = result.avx64u(n*2+1) = 0;
    }
    else {
      if (order & 0x2)
        result.avx128(n) = op2.avx128(order & 0x1);
      else
        result.avx128(n) = op1.avx128(order & 0x1);
    }

    order >>= 4;
  }

  BX_WRITE_AVX_REG(i->nnn(), result);
#endif
}

/* Opcode: VEX.66.0F.38 2C (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMASKMOVPS_VpsMps(bxInstruction_c *i)
{
  BxPackedAvxRegister mask = BX_READ_AVX_REG(i->vvv()), result;
  unsigned len = i->getVL();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (4*len); n++) {
       if (mask.avx32u(n) & 0x80000000) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 4*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }
#endif

  for (int n=4*len-1; n >= 0; n--) {
    if (mask.avx32u(n) & 0x80000000)
       result.avx32u(n) = read_virtual_dword(i->seg(), (eaddr + 4*n) & i->asize_mask());
    else
       result.avx32u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
}

/* Opcode: VEX.66.0F.38 2D (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMASKMOVPD_VpdMpd(bxInstruction_c *i)
{
  BxPackedAvxRegister mask = BX_READ_AVX_REG(i->vvv()), result;
  unsigned len = i->getVL();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (2*len); n++) {
       if (mask.avx32u(n*2+1) & 0x80000000) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 8*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }
#endif

  for (int n=2*len-1; n >= 0; n--) {
    if (mask.avx32u(n*2+1) & 0x80000000)
       result.avx64u(n) = read_virtual_qword(i->seg(), (eaddr + 8*n) & i->asize_mask());
    else
       result.avx64u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
}

/* Opcode: VEX.66.0F.38 2C (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMASKMOVPS_MpsVps(bxInstruction_c *i)
{
  BxPackedAvxRegister mask = BX_READ_AVX_REG(i->vvv()), op = BX_READ_AVX_REG(i->nnn());
  unsigned len = i->getVL();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (4*len); n++) {
       if (mask.avx32u(n) & 0x80000000) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 4*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }
#endif

  // see you can successfully write all the elements first
  for (int n=4*len-1; n >= 0; n--) {
    if (mask.avx32u(n) & 0x80000000)
       read_RMW_virtual_dword(i->seg(), (eaddr + 4*n) & i->asize_mask());
  }

  for (unsigned n=0; n < (4*len); n++) {
    if (mask.avx32u(n) & 0x80000000)
       write_virtual_dword(i->seg(), (eaddr + 4*n) & i->asize_mask(), op.avx32u(n));
  }
}

/* Opcode: VEX.66.0F.38 2D (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMASKMOVPD_MpdVpd(bxInstruction_c *i)
{
  BxPackedAvxRegister mask = BX_READ_AVX_REG(i->vvv()), op = BX_READ_AVX_REG(i->nnn());
  unsigned len = i->getVL();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (2*len); n++) {
       if (mask.avx32u(n*2+1) & 0x80000000) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 8*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }
#endif

  // see you can successfully write all the elements first
  for (int n=2*len-1; n >= 0; n--) {
    if (mask.avx32u(2*n+1) & 0x80000000)
       read_RMW_virtual_qword(i->seg(), (eaddr + 8*n) & i->asize_mask());
  }

  for (unsigned n=0; n < (2*len); n++) {
    if (mask.avx32u(2*n+1) & 0x80000000)
       write_virtual_qword(i->seg(), (eaddr + 8*n) & i->asize_mask(), op.avx64u(n));
  }
}

#endif
