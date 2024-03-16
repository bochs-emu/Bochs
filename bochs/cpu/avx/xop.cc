/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2018 Stanislav Shwartsman
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

extern float_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);

#include "simd_int.h"
#include "simd_compare.h"

typedef void (*simd_compare_method)(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2);

// comparison predicate for PCOMB
static simd_compare_method xop_compare8[8] = {
  xmm_pcmpltb,
  xmm_pcmpleb,
  xmm_pcmpgtb,
  xmm_pcmpgeb,
  xmm_pcmpeqb,
  xmm_pcmpneb,
  xmm_pcmpfalse,
  xmm_pcmptrue
};

// comparison predicate for PCOMUB
static simd_compare_method xop_compare8u[8] = {
  xmm_pcmpltub,
  xmm_pcmpleub,
  xmm_pcmpgtub,
  xmm_pcmpgeub,
  xmm_pcmpeqb,
  xmm_pcmpneb,
  xmm_pcmpfalse,
  xmm_pcmptrue
};

// comparison predicate for PCOMW
static simd_compare_method xop_compare16[8] = {
  xmm_pcmpltw,
  xmm_pcmplew,
  xmm_pcmpgtw,
  xmm_pcmpgew,
  xmm_pcmpeqw,
  xmm_pcmpnew,
  xmm_pcmpfalse,
  xmm_pcmptrue
};

// comparison predicate for PCOMUW
static simd_compare_method xop_compare16u[8] = {
  xmm_pcmpltuw,
  xmm_pcmpleuw,
  xmm_pcmpgtuw,
  xmm_pcmpgeuw,
  xmm_pcmpeqw,
  xmm_pcmpnew,
  xmm_pcmpfalse,
  xmm_pcmptrue
};

// comparison predicate for PCOMD
static simd_compare_method xop_compare32[8] = {
  xmm_pcmpltd,
  xmm_pcmpled,
  xmm_pcmpgtd,
  xmm_pcmpged,
  xmm_pcmpeqd,
  xmm_pcmpned,
  xmm_pcmpfalse,
  xmm_pcmptrue
};

// comparison predicate for PCOMUD
static simd_compare_method xop_compare32u[8] = {
  xmm_pcmpltud,
  xmm_pcmpleud,
  xmm_pcmpgtud,
  xmm_pcmpgeud,
  xmm_pcmpeqd,
  xmm_pcmpned,
  xmm_pcmpfalse,
  xmm_pcmptrue
};

// comparison predicate for PCOMQ
static simd_compare_method xop_compare64[8] = {
  xmm_pcmpltq,
  xmm_pcmpleq,
  xmm_pcmpgtq,
  xmm_pcmpgeq,
  xmm_pcmpeqq,
  xmm_pcmpneq,
  xmm_pcmpfalse,
  xmm_pcmptrue
};

// comparison predicate for PCOMUQ
static simd_compare_method xop_compare64u[8] = {
  xmm_pcmpltuq,
  xmm_pcmpleuq,
  xmm_pcmpgtuq,
  xmm_pcmpgeuq,
  xmm_pcmpeqq,
  xmm_pcmpneq,
  xmm_pcmpfalse,
  xmm_pcmptrue
};

typedef Bit8u (*vpperm_operation)(Bit8u byte);

BX_CPP_INLINE Bit8u vpperm_bit_reverse(Bit8u v8)
{
  return  (v8 >> 7) |
         ((v8 >> 5) & 0x02) |
         ((v8 >> 3) & 0x04) |
         ((v8 >> 1) & 0x08) |
         ((v8 << 1) & 0x10) |
         ((v8 << 3) & 0x20) |
         ((v8 << 5) & 0x40) |
          (v8 << 7);
}

BX_CPP_INLINE Bit8u vpperm_noop(Bit8u v8) { return v8; }
BX_CPP_INLINE Bit8u vpperm_invert(Bit8u v8) { return ~v8; }
BX_CPP_INLINE Bit8u vpperm_invert_bit_reverse(Bit8u v8) { return vpperm_bit_reverse(~v8); }
BX_CPP_INLINE Bit8u vpperm_zeros(Bit8u v8) { return 0; }
BX_CPP_INLINE Bit8u vpperm_ones(Bit8u v8) { return 0xff; }
BX_CPP_INLINE Bit8u vpperm_replicate_msb(Bit8u v8) { return (((Bit8s) v8) >> 7); }
BX_CPP_INLINE Bit8u vpperm_invert_replicate_msb(Bit8u v8) { return vpperm_replicate_msb(~v8); }

// logical operation for VPPERM
static vpperm_operation vpperm_op[8] = {
  vpperm_noop,
  vpperm_invert,
  vpperm_bit_reverse,
  vpperm_invert_bit_reverse,
  vpperm_zeros,
  vpperm_ones,
  vpperm_replicate_msb,
  vpperm_invert_replicate_msb
};

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMOV_VdqHdqWdqVIb(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    xmm_pselect(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n));
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPPERM_VdqHdqWdqVIb(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3()), dst;

  for (unsigned n=0;n<16;n++) {
    unsigned control = op3.xmmubyte(n);

    if (control & 0x10)
      dst.xmmubyte(n) = op1.xmmubyte(control & 0xf);
    else
      dst.xmmubyte(n) = op2.xmmubyte(control & 0xf);

    dst.xmmubyte(n) = vpperm_op[control >> 5](dst.xmmubyte(n));
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);

  BX_NEXT_INSTR(i);
}

#define XOP_SHIFT_ROTATE(HANDLER, func)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)               \
  {                                                                                         \
    BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2()); \
                                                                                            \
    (func)(&op1, &op2);                                                                     \
                                                                                            \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);                                             \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

XOP_SHIFT_ROTATE(VPSHAB_VdqWdqHdq, xmm_pshab);
XOP_SHIFT_ROTATE(VPSHAW_VdqWdqHdq, xmm_pshaw);
XOP_SHIFT_ROTATE(VPSHAD_VdqWdqHdq, xmm_pshad);
XOP_SHIFT_ROTATE(VPSHAQ_VdqWdqHdq, xmm_pshaq);

XOP_SHIFT_ROTATE(VPSHLB_VdqWdqHdq, xmm_pshlb);
XOP_SHIFT_ROTATE(VPSHLW_VdqWdqHdq, xmm_pshlw);
XOP_SHIFT_ROTATE(VPSHLD_VdqWdqHdq, xmm_pshld);
XOP_SHIFT_ROTATE(VPSHLQ_VdqWdqHdq, xmm_pshlq);

XOP_SHIFT_ROTATE(VPROTB_VdqWdqHdq, xmm_protb);
XOP_SHIFT_ROTATE(VPROTW_VdqWdqHdq, xmm_protw);
XOP_SHIFT_ROTATE(VPROTD_VdqWdqHdq, xmm_protd);
XOP_SHIFT_ROTATE(VPROTQ_VdqWdqHdq, xmm_protq);

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSSWW_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  for(unsigned n=0;n<8;n++) {
    op1.xmm16s(n) = SaturateDwordSToWordS(((Bit32s) op1.xmm16s(n) * (Bit32s) op2.xmm16s(n)) + (Bit32s) op3.xmm16s(n));
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSSWD_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  op1.xmm32s(0) = SaturateQwordSToDwordS(((Bit32s) op1.xmm16s(1) * (Bit32s) op2.xmm16s(1)) + (Bit64s) op3.xmm32s(0));
  op1.xmm32s(1) = SaturateQwordSToDwordS(((Bit32s) op1.xmm16s(3) * (Bit32s) op2.xmm16s(3)) + (Bit64s) op3.xmm32s(1));
  op1.xmm32s(2) = SaturateQwordSToDwordS(((Bit32s) op1.xmm16s(5) * (Bit32s) op2.xmm16s(5)) + (Bit64s) op3.xmm32s(2));
  op1.xmm32s(3) = SaturateQwordSToDwordS(((Bit32s) op1.xmm16s(7) * (Bit32s) op2.xmm16s(7)) + (Bit64s) op3.xmm32s(3));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

BX_CPP_INLINE Bit64s add_saturate64(Bit64s a, Bit64s b)
{
  Bit64s r = a + b;
  Bit64u overflow = GET_ADD_OVERFLOW(a, b, r, BX_CONST64(0x8000000000000000));
  if (! overflow) return r;
  // signed overflow detected, saturate
  if (a > 0) overflow--;
  return (Bit64s) overflow;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSSDQL_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  Bit64s product1 = (Bit64s) op1.xmm32s(0) * (Bit64s) op2.xmm32s(0);
  Bit64s product2 = (Bit64s) op1.xmm32s(2) * (Bit64s) op2.xmm32s(2);

  op1.xmm64s(0) = add_saturate64(product1, op3.xmm64s(0));
  op1.xmm64s(1) = add_saturate64(product2, op3.xmm64s(1));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSSDD_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  for(unsigned n=0;n<4;n++) {
    op1.xmm32s(n) = SaturateQwordSToDwordS(((Bit64s) op1.xmm32s(n) * (Bit64s) op2.xmm32s(n)) + (Bit64s) op3.xmm32s(n));
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSSDQH_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  Bit64s product1 = (Bit64s) op1.xmm32s(1) * (Bit64s) op2.xmm32s(1);
  Bit64s product2 = (Bit64s) op1.xmm32s(3) * (Bit64s) op2.xmm32s(3);

  op1.xmm64s(0) = add_saturate64(product1, op3.xmm64s(0));
  op1.xmm64s(1) = add_saturate64(product2, op3.xmm64s(1));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSWW_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  for(unsigned n=0;n<8;n++) {
    op1.xmm16s(n) = ((Bit32s) op1.xmm16s(n) * (Bit32s) op2.xmm16s(n)) + (Bit32s) op3.xmm16s(n);
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSWD_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  op1.xmm32s(0) = ((Bit32s) op1.xmm16s(1) * (Bit32s) op2.xmm16s(1)) + (Bit64s) op3.xmm32s(0);
  op1.xmm32s(1) = ((Bit32s) op1.xmm16s(3) * (Bit32s) op2.xmm16s(3)) + (Bit64s) op3.xmm32s(1);
  op1.xmm32s(2) = ((Bit32s) op1.xmm16s(5) * (Bit32s) op2.xmm16s(5)) + (Bit64s) op3.xmm32s(2);
  op1.xmm32s(3) = ((Bit32s) op1.xmm16s(7) * (Bit32s) op2.xmm16s(7)) + (Bit64s) op3.xmm32s(3);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSDQL_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  Bit64s product1 = (Bit64s) op1.xmm32s(0) * (Bit64s) op2.xmm32s(0);
  Bit64s product2 = (Bit64s) op1.xmm32s(2) * (Bit64s) op2.xmm32s(2);

  op1.xmm64s(0) = product1 + op3.xmm64s(0);
  op1.xmm64s(1) = product2 + op3.xmm64s(1);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSDD_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  for(unsigned n=0;n<4;n++) {
    op1.xmm32s(n) = ((Bit64s) op1.xmm32s(n) * (Bit64s) op2.xmm32s(n)) + (Bit64s) op3.xmm32s(n);
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMACSDQH_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  Bit64s product1 = (Bit64s) op1.xmm32s(1) * (Bit64s) op2.xmm32s(1);
  Bit64s product2 = (Bit64s) op1.xmm32s(3) * (Bit64s) op2.xmm32s(3);

  op1.xmm64s(0) = product1 + op3.xmm64s(0);
  op1.xmm64s(1) = product2 + op3.xmm64s(1);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADCSSWD_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  Bit32s product[8];

  for(unsigned n=0;n < 8;n++)
    product[n] = (Bit32s) op1.xmm16s(n) * (Bit32s) op2.xmm16s(n);

  op1.xmm32s(0) = SaturateQwordSToDwordS((Bit64s) product[0] + (Bit64s) product[1] + (Bit64s) op3.xmm32s(0));
  op1.xmm32s(1) = SaturateQwordSToDwordS((Bit64s) product[2] + (Bit64s) product[3] + (Bit64s) op3.xmm32s(1));
  op1.xmm32s(2) = SaturateQwordSToDwordS((Bit64s) product[4] + (Bit64s) product[5] + (Bit64s) op3.xmm32s(2));
  op1.xmm32s(3) = SaturateQwordSToDwordS((Bit64s) product[6] + (Bit64s) product[7] + (Bit64s) op3.xmm32s(3));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADCSWD_VdqHdqWdqVIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  BxPackedXmmRegister op3 = BX_READ_XMM_REG(i->src3());

  Bit32s product[8];

  for(unsigned n=0;n < 8;n++)
    product[n] = (Bit32s) op1.xmm16s(n) * (Bit32s) op2.xmm16s(n);

  op1.xmm32s(0) = product[0] + product[1] + op3.xmm32s(0);
  op1.xmm32s(1) = product[2] + product[3] + op3.xmm32s(1);
  op1.xmm32s(2) = product[4] + product[5] + op3.xmm32s(2);
  op1.xmm32s(3) = product[6] + product[7] + op3.xmm32s(3);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPROTB_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  int count = i->Ib();

  if (count > 0) {
    // rotate left
    xmm_prolb(&op,  count);
  }
  else if (count < 0) {
    // rotate right
    xmm_prorb(&op, -count);
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPROTW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  int count = i->Ib();

  if (count > 0) {
    // rotate left
    xmm_prolw(&op,  count);
  }
  else if (count < 0) {
    // rotate right
    xmm_prorw(&op, -count);
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPROTD_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  int count = i->Ib();

  if (count > 0) {
    // rotate left
    xmm_prold(&op,  count);
  }
  else if (count < 0) {
    // rotate right
    xmm_prord(&op, -count);
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPROTQ_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  int count = i->Ib();

  if (count > 0) {
    // rotate left
    xmm_prolq(&op,  count);
  }
  else if (count < 0) {
    // rotate right
    xmm_prorq(&op, -count);
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMB_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2());

  xop_compare8[i->Ib() & 7](&op1, &op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMW_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2());

  xop_compare16[i->Ib() & 7](&op1, &op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMD_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2());

  xop_compare32[i->Ib() & 7](&op1, &op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMQ_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2());

  xop_compare64[i->Ib() & 7](&op1, &op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMUB_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2());

  xop_compare8u[i->Ib() & 7](&op1, &op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMUW_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2());

  xop_compare16u[i->Ib() & 7](&op1, &op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMUD_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2());

  xop_compare32u[i->Ib() & 7](&op1, &op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMUQ_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1()), op2 = BX_READ_XMM_REG(i->src2());

  xop_compare64u[i->Ib() & 7](&op1, &op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFRCZPS_VpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op.ymm32u(n) = f32_frc(op.ymm32u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFRCZPD_VpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    op.ymm64u(n) = f64_frc(op.ymm64u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFRCZSS_VssWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());
  BxPackedXmmRegister r;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  r.xmm64u(0) = (Bit64u) f32_frc(op, &status);
  r.xmm64u(1) = 0;

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), r);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFRCZSD_VsdWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());
  BxPackedXmmRegister r;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  r.xmm64u(0) = f64_frc(op, &status);
  r.xmm64u(1) = 0;

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), r);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDBW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm16s(0) = (Bit16s) op.xmmsbyte(0x0) + (Bit16s) op.xmmsbyte(0x1);
  op.xmm16s(1) = (Bit16s) op.xmmsbyte(0x2) + (Bit16s) op.xmmsbyte(0x3);
  op.xmm16s(2) = (Bit16s) op.xmmsbyte(0x4) + (Bit16s) op.xmmsbyte(0x5);
  op.xmm16s(3) = (Bit16s) op.xmmsbyte(0x6) + (Bit16s) op.xmmsbyte(0x7);
  op.xmm16s(4) = (Bit16s) op.xmmsbyte(0x8) + (Bit16s) op.xmmsbyte(0x9);
  op.xmm16s(5) = (Bit16s) op.xmmsbyte(0xA) + (Bit16s) op.xmmsbyte(0xB);
  op.xmm16s(6) = (Bit16s) op.xmmsbyte(0xC) + (Bit16s) op.xmmsbyte(0xD);
  op.xmm16s(7) = (Bit16s) op.xmmsbyte(0xE) + (Bit16s) op.xmmsbyte(0xF);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDBD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm32s(0) = (Bit32s) op.xmmsbyte(0x0) + (Bit32s) op.xmmsbyte(0x1) +
                 (Bit32s) op.xmmsbyte(0x2) + (Bit32s) op.xmmsbyte(0x3);
  op.xmm32s(1) = (Bit32s) op.xmmsbyte(0x4) + (Bit32s) op.xmmsbyte(0x5) +
                 (Bit32s) op.xmmsbyte(0x6) + (Bit32s) op.xmmsbyte(0x7);
  op.xmm32s(2) = (Bit32s) op.xmmsbyte(0x8) + (Bit32s) op.xmmsbyte(0x9) +
                 (Bit32s) op.xmmsbyte(0xA) + (Bit32s) op.xmmsbyte(0xB);
  op.xmm32s(3) = (Bit32s) op.xmmsbyte(0xC) + (Bit32s) op.xmmsbyte(0xD) +
                 (Bit32s) op.xmmsbyte(0xE) + (Bit32s) op.xmmsbyte(0xF);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDBQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm64s(0) = (Bit32s) op.xmmsbyte(0x0) + (Bit32s) op.xmmsbyte(0x1) +
                 (Bit32s) op.xmmsbyte(0x2) + (Bit32s) op.xmmsbyte(0x3) +
                 (Bit32s) op.xmmsbyte(0x4) + (Bit32s) op.xmmsbyte(0x5) +
                 (Bit32s) op.xmmsbyte(0x6) + (Bit32s) op.xmmsbyte(0x7);
  op.xmm64s(1) = (Bit32s) op.xmmsbyte(0x8) + (Bit32s) op.xmmsbyte(0x9) +
                 (Bit32s) op.xmmsbyte(0xA) + (Bit32s) op.xmmsbyte(0xB) +
                 (Bit32s) op.xmmsbyte(0xC) + (Bit32s) op.xmmsbyte(0xD) +
                 (Bit32s) op.xmmsbyte(0xE) + (Bit32s) op.xmmsbyte(0xF);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDWD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm32s(0) = (Bit32s) op.xmm16s(0) + (Bit32s) op.xmm16s(1);
  op.xmm32s(1) = (Bit32s) op.xmm16s(2) + (Bit32s) op.xmm16s(3);
  op.xmm32s(2) = (Bit32s) op.xmm16s(4) + (Bit32s) op.xmm16s(5);
  op.xmm32s(3) = (Bit32s) op.xmm16s(6) + (Bit32s) op.xmm16s(7);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDWQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm64s(0) = (Bit32s) op.xmm16s(0) + (Bit32s) op.xmm16s(1) +
                 (Bit32s) op.xmm16s(2) + (Bit32s) op.xmm16s(3);
  op.xmm64s(1) = (Bit32s) op.xmm16s(4) + (Bit32s) op.xmm16s(5) +
                 (Bit32s) op.xmm16s(6) + (Bit32s) op.xmm16s(7);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDDQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm64s(0) = (Bit64s) op.xmm32s(0) + (Bit64s) op.xmm32s(1);
  op.xmm64s(1) = (Bit64s) op.xmm32s(2) + (Bit64s) op.xmm32s(3);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDUBW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm16u(0) = (Bit16u) op.xmmubyte(0x0) + (Bit16u) op.xmmubyte(0x1);
  op.xmm16u(1) = (Bit16u) op.xmmubyte(0x2) + (Bit16u) op.xmmubyte(0x3);
  op.xmm16u(2) = (Bit16u) op.xmmubyte(0x4) + (Bit16u) op.xmmubyte(0x5);
  op.xmm16u(3) = (Bit16u) op.xmmubyte(0x6) + (Bit16u) op.xmmubyte(0x7);
  op.xmm16u(4) = (Bit16u) op.xmmubyte(0x8) + (Bit16u) op.xmmubyte(0x9);
  op.xmm16u(5) = (Bit16u) op.xmmubyte(0xA) + (Bit16u) op.xmmubyte(0xB);
  op.xmm16u(6) = (Bit16u) op.xmmubyte(0xC) + (Bit16u) op.xmmubyte(0xD);
  op.xmm16u(7) = (Bit16u) op.xmmubyte(0xE) + (Bit16u) op.xmmubyte(0xF);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDUBD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm32u(0) = (Bit32u) op.xmmubyte(0x0) + (Bit32s) op.xmmubyte(0x1) +
                 (Bit32u) op.xmmubyte(0x2) + (Bit32s) op.xmmubyte(0x3);
  op.xmm32u(1) = (Bit32u) op.xmmubyte(0x4) + (Bit32s) op.xmmubyte(0x5) +
                 (Bit32u) op.xmmubyte(0x6) + (Bit32s) op.xmmubyte(0x7);
  op.xmm32u(2) = (Bit32u) op.xmmubyte(0x8) + (Bit32s) op.xmmubyte(0x9) +
                 (Bit32u) op.xmmubyte(0xA) + (Bit32s) op.xmmubyte(0xB);
  op.xmm32u(3) = (Bit32u) op.xmmubyte(0xC) + (Bit32s) op.xmmubyte(0xD) +
                 (Bit32u) op.xmmubyte(0xE) + (Bit32s) op.xmmubyte(0xF);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDUBQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm64u(0) = (Bit32u) op.xmmubyte(0x0) + (Bit32u) op.xmmubyte(0x1) +
                 (Bit32u) op.xmmubyte(0x2) + (Bit32u) op.xmmubyte(0x3) +
                 (Bit32u) op.xmmubyte(0x4) + (Bit32u) op.xmmubyte(0x5) +
                 (Bit32u) op.xmmubyte(0x6) + (Bit32u) op.xmmubyte(0x7);
  op.xmm64u(1) = (Bit32u) op.xmmubyte(0x8) + (Bit32u) op.xmmubyte(0x9) +
                 (Bit32u) op.xmmubyte(0xA) + (Bit32u) op.xmmubyte(0xB) +
                 (Bit32u) op.xmmubyte(0xC) + (Bit32u) op.xmmubyte(0xD) +
                 (Bit32u) op.xmmubyte(0xE) + (Bit32u) op.xmmubyte(0xF);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDUWD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm32u(0) = (Bit32u) op.xmm16u(0) + (Bit32u) op.xmm16u(1);
  op.xmm32u(1) = (Bit32u) op.xmm16u(2) + (Bit32u) op.xmm16u(3);
  op.xmm32u(2) = (Bit32u) op.xmm16u(4) + (Bit32u) op.xmm16u(5);
  op.xmm32u(3) = (Bit32u) op.xmm16u(6) + (Bit32u) op.xmm16u(7);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDUWQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm64u(0) = (Bit32u) op.xmm16u(0) + (Bit32u) op.xmm16u(1) +
                 (Bit32u) op.xmm16u(2) + (Bit32u) op.xmm16u(3);
  op.xmm64u(1) = (Bit32u) op.xmm16u(4) + (Bit32u) op.xmm16u(5) +
                 (Bit32u) op.xmm16u(6) + (Bit32u) op.xmm16u(7);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHADDUDQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm64u(0) = (Bit64u) op.xmm32u(0) + (Bit64u) op.xmm32u(1);
  op.xmm64u(1) = (Bit64u) op.xmm32u(2) + (Bit64u) op.xmm32u(3);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHSUBBW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm16s(0) = (Bit16s) op.xmmsbyte(0x0) - (Bit16s) op.xmmsbyte(0x1);
  op.xmm16s(1) = (Bit16s) op.xmmsbyte(0x2) - (Bit16s) op.xmmsbyte(0x3);
  op.xmm16s(2) = (Bit16s) op.xmmsbyte(0x4) - (Bit16s) op.xmmsbyte(0x5);
  op.xmm16s(3) = (Bit16s) op.xmmsbyte(0x6) - (Bit16s) op.xmmsbyte(0x7);
  op.xmm16s(4) = (Bit16s) op.xmmsbyte(0x8) - (Bit16s) op.xmmsbyte(0x9);
  op.xmm16s(5) = (Bit16s) op.xmmsbyte(0xA) - (Bit16s) op.xmmsbyte(0xB);
  op.xmm16s(6) = (Bit16s) op.xmmsbyte(0xC) - (Bit16s) op.xmmsbyte(0xD);
  op.xmm16s(7) = (Bit16s) op.xmmsbyte(0xE) - (Bit16s) op.xmmsbyte(0xF);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHSUBWD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm32s(0) = (Bit32s) op.xmm16s(0) - (Bit32s) op.xmm16s(1);
  op.xmm32s(1) = (Bit32s) op.xmm16s(2) - (Bit32s) op.xmm16s(3);
  op.xmm32s(2) = (Bit32s) op.xmm16s(4) - (Bit32s) op.xmm16s(5);
  op.xmm32s(3) = (Bit32s) op.xmm16s(6) - (Bit32s) op.xmm16s(7);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPHSUBDQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm64s(0) = (Bit64s) op.xmm32s(0) - (Bit64s) op.xmm32s(1);
  op.xmm64s(1) = (Bit64s) op.xmm32s(2) - (Bit64s) op.xmm32s(3);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMIL2PS_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3()), result;
  unsigned len = i->getVL();

  result.clear();

  for (unsigned n=0; n < len; n++) {
    xmm_permil2ps(&result.ymm128(n), &op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), i->Ib() & 3);
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMIL2PD_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3()), result;
  unsigned len = i->getVL();

  result.clear();

  for (unsigned n=0; n < len; n++) {
    xmm_permil2pd(&result.ymm128(n), &op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), i->Ib() & 3);
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

#endif
