/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014-2026 Stanislav Shwartsman
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

#if BX_SUPPORT_EVEX

//
// VRSQRT14 approximation is computed using piecewise linear approximation
// of 1/sqrt(x) over 32 equal intervals of [1.0, 2.0) and 32 equal intervals
// of [2.0, 4.0), the coefficients are taken from Intel reference code for VRSQRT14:
//
//   if (x is in [1.0, 2.0))
//     i = int_floor((x-1)*32)
//     y[i] = 1 + 1/64 + i/32
//   if (x is in [2.0, 4.0))
//     i = int_floor((x-2)*16)
//     y[i] = 2 + 1/32 + i/16
//   rsqrt14(x) = truncate_to_17_ms_bits(c[i] - d[i]*(x-y[i]))
//
// x is truncated to 15 fraction bits before the computation.
// c[i] is 19-bit value scaled by 2^19, d[i] is 10-bit value scaled by 2^10
// for [1.0, 2.0) and by 2^11 for [2.0, 4.0).
//

// { c[i], d[i] } for [1.0, 2.0) followed by { c[i], d[i] } for [2.0, 4.0)
static const Bit32u rsqrt14_coeff[32][4] = {
  { 520261, 1001, 367881,  707 },   { 512437,  955, 362349,  675 },
  { 504953,  915, 357056,  647 },   { 497790,  877, 351992,  619 },
  { 490922,  841, 347136,  595 },   { 484331,  807, 342475,  571 },
  { 478001,  775, 337997,  549 },   { 471909,  747, 333693,  527 },
  { 466046,  719, 329545,  509 },   { 460397,  693, 325551,  491 },
  { 454947,  669, 321697,  473 },   { 449688,  647, 317977,  457 },
  { 444606,  625, 314385,  441 },   { 439694,  603, 310910,  427 },
  { 434939,  585, 307549,  413 },   { 430335,  567, 304295,  401 },
  { 425875,  549, 301139,  389 },   { 421551,  533, 298079,  377 },
  { 417355,  517, 295115,  365 },   { 413284,  501, 292237,  355 },
  { 409329,  487, 289439,  345 },   { 405487,  473, 286722,  335 },
  { 401748,  461, 284080,  325 },   { 398111,  449, 281508,  317 },
  { 394571,  437, 279006,  309 },   { 391127,  425, 276569,  301 },
  { 387770,  415, 274195,  293 },   { 384498,  403, 271882,  285 },
  { 381307,  393, 269625,  279 },   { 378194,  385, 267425,  271 },
  { 375155,  375, 265276,  265 },   { 372190,  367, 263178,  259 }
};

// Calculate the 16-bit fraction of 1/sqrt(1.f) (when odd_exp is true) or
// 1/sqrt(2*1.f) (when odd_exp is false), the result is scaled to [1.0, 2.0)
// range. f is 15-bit fraction and odd_exp is odd biased exponent of the input.
static BX_CPP_INLINE Bit32u rsqrt14_fraction(Bit32u fraction15, bool odd_exp)
{
  const Bit32u *coeff = rsqrt14_coeff[fraction15 >> 10] + (odd_exp ? 0 : 2);
  // x - y[i] scaled by 2^15 for [1.0, 2.0) or 2^14 for [2.0, 4.0)
  Bit32s delta = (Bit32s)(fraction15 & 0x3ff) - 512;
  // the result is 17-bit significand with implicit leading 1 in bit 16
  Bit32s sig = ((Bit32s)(coeff[0] << 7) - (Bit32s)(coeff[1]) * delta) >> 9;
  return (Bit32u)(sig) - 0x10000;
}

#include "softfloat3e/include/softfloat.h"
#include "fpu/softfloat-specialize.h"
#include "simd_int.h"

// approximate 14-bit sqrt reciprocal of scalar single precision FP
float16 approximate_rsqrt14(float16 op, bool daz)
{
  softfloat_class_t op_class = f16_class(op);

  int sign = f16_sign(op);
  Bit16u fraction = f16_fraction(op);
  Bit16s exp = f16_exp(op);
  struct exp8_sig16 normExpSig;

  switch(op_class) {
    case softfloat_zero:
      return packFloat16(sign, 0x1F, 0);

    case softfloat_positive_inf:
      return 0;

    case softfloat_negative_inf:
      return float16_default_nan;

    case softfloat_SNaN:
    case softfloat_QNaN:
      return convert_to_QNaN(op);

    case softfloat_denormal:
      if (daz) return packFloat16(sign, 0x1F, 0);

      normExpSig = softfloat_normSubnormalF16Sig(fraction);
      exp = normExpSig.exp;
      fraction = normExpSig.sig;

      fraction &= 0x3ff;
      // fall through

    case softfloat_normalized:
      break;
  };

  if (sign == 1)
    return float16_default_nan;

  /*
   * Calculate (1/1.yyyyyyyyyyyyy1), the result is always rounded to the
   * 14th bit after the decimal point by round-to-nearest, regardless
   * of the current rounding mode.
   *
   * Using piecewise linear approximation.
   */

  bool oddExp = (exp & 1) != 0;

  exp = 0xE - ((exp - 0xF) >> 1);
  // An exact power-of-two input (fraction==0) does NOT always produce an
  // exact power-of-two result - only when the input's exponent parity makes
  // it one (handled by the odd exponent approximation naturally rounding up
  // to exactly 0x400 and carrying into the exponent below); skipping the
  // computation here for the other parity silently dropped a real, nonzero
  // result.
  fraction = rsqrt14_fraction(fraction << 5, oddExp);

  // round the 16-bit approximation result down to the 10-bit half-precision
  // fraction field, round-to-nearest (ties round up - confirmed against
  // hardware, this is not round-to-nearest-even); packFloat16 combines
  // exp and fraction with addition, so a fraction that rounds up to
  // exactly 0x400 correctly carries into the exponent.
  fraction = (fraction + 0x20)>>6;

  return packFloat16(0, exp, fraction);
}

// approximate 14-bit sqrt reciprocal of scalar single precision FP
float32 approximate_rsqrt14(float32 op, bool daz)
{
  softfloat_class_t op_class = f32_class(op);

  int sign = f32_sign(op);
  Bit32u fraction = f32_fraction(op);
  Bit16s exp = f32_exp(op);
  struct exp16_sig32 normExpSig;

  switch(op_class) {
    case softfloat_zero:
      return packFloat32(sign, 0xFF, 0);

    case softfloat_positive_inf:
      return 0;

    case softfloat_negative_inf:
      return float32_default_nan;

    case softfloat_SNaN:
    case softfloat_QNaN:
      return convert_to_QNaN(op);

    case softfloat_denormal:
      if (daz) return packFloat32(sign, 0xFF, 0);

      normExpSig = softfloat_normSubnormalF32Sig(fraction);
      exp = normExpSig.exp;
      fraction = normExpSig.sig;

      fraction &= 0x7fffff;
      // fall through

    case softfloat_normalized:
      break;
  };

  if (sign == 1)
    return float32_default_nan;

  /*
   * Calculate (1/1.yyyyyyyyyyyyy1), the result is always rounded to the
   * 14th bit after the decimal point by round-to-nearest, regardless
   * of the current rounding mode.
   *
   * Using piecewise linear approximation.
   */

  bool oddExp = (exp & 1) != 0;

  exp = 0x7E - ((exp - 0x7F) >> 1);
  // An exact power-of-two input (fraction==0) only produces an exact
  // power-of-two result for odd input exponent - for the other parity
  // the approximation is a real, nonzero value that must still be
  // computed, not skipped.
  if (fraction == 0 && oddExp)
    exp++;
  else
    fraction = rsqrt14_fraction(fraction >> 8, oddExp);

  return packFloat32(0, exp, fraction << 7);
}

// approximate 14-bit sqrt reciprocal of scalar double precision FP
float64 approximate_rsqrt14(float64 op, bool daz)
{
  softfloat_class_t op_class = f64_class(op);

  int sign = f64_sign(op);
  Bit64u fraction = f64_fraction(op);
  Bit16s exp = f64_exp(op);
  struct exp16_sig64 normExpSig;

  switch(op_class) {
    case softfloat_zero:
      return packFloat64(sign, 0x7FF, 0);

    case softfloat_positive_inf:
      return 0;

    case softfloat_negative_inf:
      return float64_default_nan;

    case softfloat_SNaN:
    case softfloat_QNaN:
      return convert_to_QNaN(op);

    case softfloat_denormal:
      if (daz) return packFloat64(sign, 0x7FF, 0);

      normExpSig = softfloat_normSubnormalF64Sig(fraction);
      exp = normExpSig.exp;
      fraction = normExpSig.sig;

      fraction &= BX_CONST64(0xfffffffffffff);
      // fall through

    case softfloat_normalized:
      break;
  };

  if (sign == 1)
    return float64_default_nan;

  // Compute the single precision 23-bit mantissa from the 52-bit double
  // precision mantissa by shifting it right, but also leave a "sticky bit"
  // for the shifted off bits.
  fraction = (fraction >> 29) | ((fraction & 0x1fffffff) != 0);

  bool oddExp = (exp & 1) != 0;

  exp = 0x3FE - ((exp - 0x3FF) >> 1);
  // An exact power-of-two input (fraction==0) only produces an exact
  // power-of-two result for odd input exponent - for the other parity
  // the approximation is a real, nonzero value that must still be
  // computed, not skipped.
  if (fraction == 0 && oddExp)
    exp++;
  else
    fraction = rsqrt14_fraction((Bit32u)fraction >> 8, oddExp);

  return packFloat64(0, exp, fraction << 36);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRT14PS_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u mask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);

  for (unsigned n=0, tmp_mask = mask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op.vmm32u(n) = approximate_rsqrt14((float32) op.vmm32u(n), MXCSR.get_DAZ());
    else
      op.vmm32u(n) = 0;
  }

  if (! i->isZeroMasking()) {
    simd_blendps(&BX_READ_AVX_REG(i->dst()), &op, mask, num_elements);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRT14PD_MASK_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u mask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = QWORD_ELEMENTS(len);

  for (unsigned n=0, tmp_mask = mask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op.vmm64u(n) = approximate_rsqrt14((float64) op.vmm64u(n), MXCSR.get_DAZ());
    else
      op.vmm64u(n) = 0;
  }

  if (! i->isZeroMasking()) {
    simd_blendpd(&BX_READ_AVX_REG(i->dst()), &op, mask, num_elements);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRT14SS_MASK_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());
    op1.xmm32u(0) = approximate_rsqrt14(op2, MXCSR.get_DAZ());
  }
  else {
    if (i->isZeroMasking())
      op1.xmm32u(0) = 0;
    else
      op1.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRT14SD_MASK_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());
    op1.xmm64u(0) = approximate_rsqrt14(op2, MXCSR.get_DAZ());
  }
  else {
    if (i->isZeroMasking())
      op1.xmm64u(0) = 0;
    else
      op1.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRTSH_MASK_VshHphWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());
    op1.xmm16u(0) = approximate_rsqrt14(op2, false); // MXCSR.DAZ is ignored for FP16 inputs
  }
  else {
    if (i->isZeroMasking())
      op1.xmm16u(0) = 0;
    else
      op1.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRTPH_MASK_VphWphR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u mask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);

  for (unsigned n=0, tmp_mask = mask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op.vmm16u(n) = approximate_rsqrt14((float16) op.vmm16u(n), false); // MXCSR.DAZ is ignored for FP16 inputs
    else
      op.vmm16u(n) = 0;
  }

  if (! i->isZeroMasking()) {
    simd_pblendw(&BX_READ_AVX_REG(i->dst()), &op, mask, num_elements);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

#endif
