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
// VRCP14 approximation is computed using piecewise linear approximation
// of 1/x over 64 equal intervals of [1.0, 2.0), the coefficients are taken
// from Intel reference code for VRCP14:
//
//   i = int_floor((x-1)*64)
//   y[i] = 1 + 1/128 + i/64
//   rcp14(x) = truncate_to_17_ms_bits(a[i] - b[i]*(x-y[i]))
//
// x is truncated to 16 fraction bits before the computation.
// a[i] is 18-bit value scaled by 2^18, b[i] is 10-bit value scaled by 2^10.
//

static const Bit32u rcp14_coeff[64][2] = {
  { 260119, 1009 },   { 256148,  977 },   { 252296,  949 },   { 248558,  921 },
  { 244929,  893 },   { 241405,  869 },   { 237981,  843 },   { 234652,  821 },
  { 231416,  797 },   { 228266,  777 },   { 225202,  755 },   { 222220,  735 },
  { 219314,  717 },   { 216485,  699 },   { 213727,  681 },   { 211038,  663 },
  { 208417,  647 },   { 205859,  631 },   { 203364,  617 },   { 200929,  601 },
  { 198551,  587 },   { 196229,  573 },   { 193960,  561 },   { 191743,  547 },
  { 189576,  535 },   { 187458,  523 },   { 185387,  513 },   { 183360,  501 },
  { 181377,  491 },   { 179439,  479 },   { 177540,  469 },   { 175681,  459 },
  { 173860,  451 },   { 172077,  441 },   { 170330,  433 },   { 168618,  423 },
  { 166940,  415 },   { 165295,  407 },   { 163682,  399 },   { 162101,  391 },
  { 160550,  385 },   { 159027,  377 },   { 157535,  369 },   { 156069,  363 },
  { 154631,  357 },   { 153219,  349 },   { 151832,  343 },   { 150470,  337 },
  { 149133,  331 },   { 147819,  325 },   { 146528,  319 },   { 145260,  315 },
  { 144012,  309 },   { 142787,  303 },   { 141582,  299 },   { 140397,  293 },
  { 139232,  289 },   { 138085,  285 },   { 136959,  279 },   { 135853,  275 },
  { 134763,  271 },   { 133689,  267 },   { 132631,  263 },   { 131589,  259 }
};

// Calculate the 16-bit fraction of 2/(1.f) where f is 16-bit input fraction
static BX_CPP_INLINE Bit32u rcp14_fraction(Bit32u fraction16)
{
  const Bit32u *coeff = rcp14_coeff[fraction16 >> 10];
  // x - y[i] scaled by 2^16
  Bit32s delta = (Bit32s)(fraction16 & 0x3ff) - 512;
  // the result is 17-bit significand with implicit leading 1 in bit 16
  Bit32s sig = ((Bit32s)(coeff[0] << 8) - (Bit32s)(coeff[1]) * delta) >> 9;
  return (Bit32u)(sig) - 0x10000;
}

#include "softfloat3e/include/softfloat.h"

extern softfloat_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);
extern softfloat_status_t mxcsr_to_softfloat_status_word_fp16(bx_mxcsr_t mxcsr);

#include "fpu/softfloat-specialize.h"
#include "simd_int.h"

static Bit32u rcp14_approximate(Bit32u mant, unsigned bias, Bit16s *exp)
{
  /*
   * Calculate (1/1.yyyyyyyyyyyyyy1), the result is always rounded to the
   *  14th bit after the decimal point by round-to-nearest, regardless
   *  of the current rounding mode.
   *
   * Using piecewise linear approximation.
   */

  Bit16s r_exp = 2*bias - 1 - *exp;

  if (mant == 0) {
    /* fraction is zero this means we deal with exact power of 2,
       adjust the exponent */
    r_exp++;
  }
  else {
    /* The input to the approximation is the 16 most significant bits of the 23-bit mantissa */
    mant = rcp14_fraction(mant >> 7);
  }

  *exp = r_exp;

  return mant;
}

// approximate 14-bit reciprocal of scalar half precision FP
float16 approximate_rcp14(float16 op, const softfloat_status_t &status)
{
  softfloat_class_t op_class = f16_class(op);

  int sign = f16_sign(op);
  Bit16u fraction = f16_fraction(op);
  Bit16s exp = f16_exp(op);
  struct exp8_sig16 normExpSig;

  switch(op_class) {
    case softfloat_zero:
      return packFloat16(sign, 0x1F, 0);

    case softfloat_negative_inf:
    case softfloat_positive_inf:
      return packFloat16(sign, 0, 0);

    case softfloat_SNaN:
    case softfloat_QNaN:
      return convert_to_QNaN(op);

    // the rcp14 handle denormals properly
    case softfloat_denormal:
      if (softfloat_denormalsAreZeros(&status))
        return packFloat16(sign, 0x1F, 0);

      normExpSig = softfloat_normSubnormalF16Sig(fraction);
      exp = normExpSig.exp;
      fraction = normExpSig.sig;

      fraction &= 0x3ff;
      // fall through

    case softfloat_normalized:
      break;
  }

  // Compute the single precision 23-bit mantissa from the 10-bit half
  // precision mantissa by shifting it left.
  Bit32u fraction32 = Bit32u(fraction) << 13;

  fraction32 = rcp14_approximate(fraction32, FLOAT16_EXP_BIAS, &exp);

  if (exp >= 0x1F) {
    /* overflow - the result is signed infinity */
    return packFloat16(sign, 0x1F, 0);
  }

  if (exp <= 0) {
    /* underflow */
    if (softfloat_flushUnderflowToZero(&status))
      return packFloat16(sign, 0, 0);

    // The approximated value is normalized (an implicit leading 1 before it is
    // assumed); making it a denormal result means that leading 1 becomes
    // an explicit bit of the fraction, so it must be inserted before the
    // right shift - shifting the raw approximated value alone silently drops it,
    // undercounting the result by roughly a factor of 2.
    fraction32 = (fraction32 | 0x10000) >> (1 - exp); // make denormal result
    exp = 0;
  }

  // round the 16-bit approximation result down to the 10-bit half-precision
  // fraction field, round-to-nearest (ties round up - confirmed against
  // hardware, this is not round-to-nearest-even)
  fraction32 = (fraction32 + 0x20)>>6;

  return packFloat16(sign, exp, fraction32);
}

// approximate 14-bit reciprocal of scalar single precision FP
float32 approximate_rcp14(float32 op, const softfloat_status_t &status)
{
  softfloat_class_t op_class = f32_class(op);

  int sign = f32_sign(op);
  Bit32u fraction = f32_fraction(op);
  Bit16s exp = f32_exp(op);
  struct exp16_sig32 normExpSig;

  switch(op_class) {
    case softfloat_zero:
      return packFloat32(sign, 0xFF, 0);

    case softfloat_negative_inf:
    case softfloat_positive_inf:
      return packFloat32(sign, 0, 0);

    case softfloat_SNaN:
    case softfloat_QNaN:
      return convert_to_QNaN(op);

    // the rcp14 handle denormals properly
    case softfloat_denormal:
      if (softfloat_denormalsAreZeros(&status))
        return packFloat32(sign, 0xFF, 0);

      normExpSig = softfloat_normSubnormalF32Sig(fraction);
      exp = normExpSig.exp;
      fraction = normExpSig.sig;

      fraction &= 0x7fffff;
      // fall through

    case softfloat_normalized:
      break;
  }

  fraction = rcp14_approximate(fraction, FLOAT32_EXP_BIAS, &exp) << 7;

  if (exp >= 0xFF) {
    /* overflow - the result is signed infinity */
    return packFloat32(sign, 0xFF, 0);
  }

  if (exp <= 0) {
    /* underflow */
    if (softfloat_flushUnderflowToZero(&status))
      return packFloat32(sign, 0, 0);

    // The approximated value is normalized (an implicit leading 1 before it is
    // assumed); making it a denormal result means that leading 1 becomes
    // an explicit bit of the fraction, so it must be inserted before the
    // right shift - shifting the raw approximated value alone silently drops it,
    // undercounting the result by roughly a factor of 2.
    fraction = (fraction | 0x00800000) >> (1 - exp); // make denormal result
    exp = 0;
  }

  return packFloat32(sign, exp, fraction);
}

// approximate 14-bit reciprocal of scalar double precision FP
float64 approximate_rcp14(float64 op, const softfloat_status_t &status)
{
  softfloat_class_t op_class = f64_class(op);

  int sign = f64_sign(op);
  Bit64u fraction = f64_fraction(op);
  Bit16s exp = f64_exp(op);
  struct exp16_sig64 normExpSig;

  switch(op_class) {
    case softfloat_zero:
      return packFloat64(sign, 0x7FF, 0);

    case softfloat_negative_inf:
    case softfloat_positive_inf:
      return packFloat64(sign, 0, 0);

    case softfloat_SNaN:
    case softfloat_QNaN:
      return convert_to_QNaN(op);

    // the rcp14 handle denormals properly
    case softfloat_denormal:
      if (softfloat_denormalsAreZeros(&status))
        return packFloat64(sign, 0x7FF, 0);

      normExpSig = softfloat_normSubnormalF64Sig(fraction);
      exp = normExpSig.exp;
      fraction = normExpSig.sig;

      fraction &= BX_CONST64(0xfffffffffffff);
      // fall through

    case softfloat_normalized:
      break;
  }

  // Compute the single precision 23-bit mantissa from the 52-bit double
  // precision mantissa by shifting it right, but also leave a "sticky bit"
  // for the shifted off bits.
  fraction = (fraction >> 29) | ((fraction & 0x1fffffff) != 0);

  fraction = (Bit64u) rcp14_approximate(fraction, FLOAT64_EXP_BIAS, &exp);
  fraction <<= 36;

  if (exp >= 0x7FF) {
    /* overflow - the result is signed infinity */
    return packFloat64(sign, 0x7FF, 0);
  }

  if (exp <= 0) {
    /* underflow */
    if (softfloat_flushUnderflowToZero(&status))
      return packFloat64(sign, 0, 0);

    // The approximated value is normalized (an implicit leading 1 before it is
    // assumed); making it a denormal result means that leading 1 becomes
    // an explicit bit of the fraction, so it must be inserted before the
    // right shift - shifting the raw approximated value alone silently drops it,
    // undercounting the result by roughly a factor of 2.
    fraction = (fraction | BX_CONST64(0x0010000000000000)) >> (1 - exp); // make denormal result
    exp = 0;
  }

  return packFloat64(sign, exp, fraction);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCP14PS_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u mask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  for (unsigned n=0, tmp_mask = mask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op.vmm32u(n) = approximate_rcp14((float32) op.vmm32u(n), status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCP14PD_MASK_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u mask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = QWORD_ELEMENTS(len);

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  for (unsigned n=0, tmp_mask = mask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op.vmm64u(n) = approximate_rcp14((float64) op.vmm64u(n), status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCP14SS_MASK_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    op1.xmm32u(0) = approximate_rcp14(op2, status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCP14SD_MASK_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    op1.xmm64u(0) = approximate_rcp14(op2, status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCPSH_MASK_VshHphWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

    softfloat_status_t status = mxcsr_to_softfloat_status_word_fp16(MXCSR);
    op1.xmm16u(0) = approximate_rcp14(op2, status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCPPH_MASK_VphWphR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u mask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);

  softfloat_status_t status = mxcsr_to_softfloat_status_word_fp16(MXCSR);

  for (unsigned n=0, tmp_mask = mask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op.vmm16u(n) = approximate_rcp14((float16) op.vmm16u(n), status);
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
