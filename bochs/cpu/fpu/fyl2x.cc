/*============================================================================
This source file is an extension to the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 2b, written for Bochs (x86 achitecture simulator)
floating point emulation.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ALL LOSSES,
COSTS, OR OTHER PROBLEMS THEY INCUR DUE TO THE SOFTWARE, AND WHO FURTHERMORE
EFFECTIVELY INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE
INSTITUTE (possibly via similar legal warning) AGAINST ALL LOSSES, COSTS, OR
OTHER PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) the source code for the derivative work includes prominent notice that
the work is derivative, and (2) the source code includes prominent notice with
these four paragraphs for those parts of this code that are retained.
=============================================================================*/

/*
 * fyl2x.cc -- emulation of the Intel x87 FYL2X and FYL2XP1 instructions
 * =======================================================================
 * FYL2X   : y * log2(x)          FYL2XP1 : y * log2(1 + x)
 *
 * log2 is table-driven.  Let  w = x  (FYL2X) or  w = 1 + x  (FYL2XP1), split
 * as  w = 2^n * m  with m in [1,2).  Then
 *     log2(w) = n + log2(m).
 * Reduce m against one of 32 break points c:  u = 2(m - c)/(m + c)  is small
 * and scale-invariant, and
 *     log2(m) = log2(c) + (1/ln2) * (u + u^3/3 + u^5/5 + ...)
 * with log2(c) stored as a lead + trail pair in a table and the odd series a
 * short polynomial.  Arguments with |x - 1| < 1/8 skip the table and use a
 * polynomial in  (x-1)/(x+1)  directly (and FYL2XP1 with |x| tiny uses just
 * the first term).  The reduction is done on m's mantissa with n carried as a
 * plain integer, keeping everything inside the float128_t exponent range.
 *
 * Every intermediate rounds to the x87 internal extended-precision widths
 * (f128_67.cc helpers); only the closing  result = log2(w) * y  is rounded to
 * the destination honouring the FPU mode, and it alone sets SW.C1.
 */

#define FLOAT128

#include "fpu_trans.h"
#include "softfloat-helpers.h"
#include "fyl2x_constants.h"

extern float128_t f128_mul_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_add_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_div_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_add_64_ne (float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_mul_64_ne (float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_div_64_ne (float128_t a, float128_t b, struct softfloat_status_t *status);

static const float128_t float128_one    = packFloat128(BX_CONST64(0x3fff000000000000), 0);
static const float128_t float128_two    = packFloat128(BX_CONST64(0x4000000000000000), 0);
static const float128_t float128_negone = packFloat128(BX_CONST64(0xbfff000000000000), 0);

static const float128_t fyl2x_L_inv  = packFloat128(FYL2X_L_INV_HI,  FYL2X_L_INV_LO);
static const float128_t fyl2x_L_inv2 = packFloat128(FYL2X_L_INV2_HI, FYL2X_L_INV2_LO);
static const float128_t fyl2x_A1 = packFloat128(FYL2X_A1_HI, FYL2X_A1_LO);
static const float128_t fyl2x_A2 = packFloat128(FYL2X_A2_HI, FYL2X_A2_LO);
static const float128_t fyl2x_A3 = packFloat128(FYL2X_A3_HI, FYL2X_A3_LO);
static const float128_t fyl2x_B1 = packFloat128(FYL2X_B1_HI, FYL2X_B1_LO);
static const float128_t fyl2x_B2 = packFloat128(FYL2X_B2_HI, FYL2X_B2_LO);
static const float128_t fyl2x_B3 = packFloat128(FYL2X_B3_HI, FYL2X_B3_LO);
static const float128_t fyl2x_B4 = packFloat128(FYL2X_B4_HI, FYL2X_B4_LO);
static const float128_t fyl2x_B5 = packFloat128(FYL2X_B5_HI, FYL2X_B5_LO);
static const float128_t fyl2x_B6 = packFloat128(FYL2X_B6_HI, FYL2X_B6_LO);

static float128_t fyl2x_L_lead(int index)  { return packFloat128(fyl2x_L_hi[index], fyl2x_L_lo[index]); }
static float128_t fyl2x_T_trail(int index) { return packFloat128(fyl2x_T_hi[index], fyl2x_T_lo[index]); }

// -------------------------------------------------------------------------
// |x-1| >= 1/8 : table-driven reduction + low order polynomial.
// 'wm' is the [1,2) mantissa of w (= x for FYL2X, 1+x for FYL2XP1) with its
// exponent field forced to 2^0; 'n' is w's true (wide) unbiased exponent.
// u = 2(w-c)/(w+c) is scale invariant, so the reduction runs on the mantissa
// alone and stays clear of the float128_t exponent range.  Returns log2(w).
// -------------------------------------------------------------------------
static float128_t fyl2x_step5(float128_t wm, int n, softfloat_status_t &status)
{
    // top 64 bits of the mantissa (implicit 1 + fraction[111:49])
    Bit64u wsig = (BX_CONST64(1) << 63)
                | ((wm.v64 & BX_CONST64(0x0000FFFFFFFFFFFF)) << 15)
                | (wm.v0 >> 49);

    int index = (int)((wsig >> 58) & 0x1F);

    // c = (1.b1 b2 b3 b4 b5 1) : keep the top 6 significand bits, force the
    // next bit to 1, zero the rest.
    Bit64u sigC = (wsig & (~BX_CONST64(0) << 58)) | (BX_CONST64(1) << 57);
    float128_t cm = extF80_to_f128(packFloatx80(0, 0x3FFF, sigC), &status);
    float128_t cmNeg = cm;
    cmNeg.v64 ^= (BX_CONST64(1) << 63);

    float128_t r = f128_add_64_ne(wm, cmNeg, &status);  /* r = w - c        */
    float128_t s = f128_add_64_ne(r, r, &status);       /* s = 2r           */
    float128_t t = f128_add_64_ne(wm, cm, &status);     /* t = w + c        */
    float128_t u = f128_div_64_ne(s, t, &status);       /* u = 2(w-c)/(w+c) */

    float128_t d1 = fyl2x_L_lead(index);
    float128_t d2 = fyl2x_T_trail(index);
    float128_t e  = f128_add_64_ne(i32_to_f128(n), d1, &status);  /* e = n + d1 */

    float128_t u1 = f128_mul_67_chop(fyl2x_L_inv, u, &status);   /* u1 = (1/log2) * u */
    float128_t v  = f128_mul_67_chop(u, u, &status);             /* v  = u*u          */

    /* p = u1 + u * v*(A1 + v*(A2 + v*A3)) */
    float128_t p = f128_mul_67_chop(v, fyl2x_A3, &status);
    p = f128_add_64_ne(p, fyl2x_A2, &status);
    p = f128_mul_67_chop(v, p, &status);
    p = f128_add_64_ne(p, fyl2x_A1, &status);
    p = f128_mul_67_chop(v, p, &status);
    p = f128_mul_67_chop(p, u, &status);
    p = f128_add_64_ne(u1, p, &status);

    /* z = e + (d2 + p) */
    float128_t z = f128_add_67_chop(d2, p, &status);
    z = f128_add_67_chop(e, z, &status);
    return z;
}

// -------------------------------------------------------------------------
// |x-1| < 1/8 : polynomial in u.  'sarg' is (x-1) for FYL2X or x for
// FYL2XP1; 't' is the matching denominator (x+1 or x+2).  Returns log2().
// -------------------------------------------------------------------------
static float128_t fyl2x_poly(float128_t sarg, float128_t t, softfloat_status_t &status)
{
    float128_t s = f128_mul_67_chop(fyl2x_L_inv2, sarg, &status);
    float128_t u = f128_div_67_chop(s, t, &status);
    float128_t v = f128_mul_64_ne(u, u, &status);
    float128_t w = f128_mul_67_chop(v, v, &status);

    /* p = v*(B1 + w*(B3 + w*B5)) */
    float128_t p = f128_mul_67_chop(w, fyl2x_B5, &status);
    p = f128_add_64_ne(p, fyl2x_B3, &status);
    p = f128_mul_67_chop(w, p, &status);
    p = f128_add_64_ne(p, fyl2x_B1, &status);
    p = f128_mul_67_chop(v, p, &status);

    /* q = w*(B2 + w*(B4 + w*B6)) */
    float128_t q = f128_mul_67_chop(w, fyl2x_B6, &status);
    q = f128_add_64_ne(q, fyl2x_B4, &status);
    q = f128_mul_67_chop(w, q, &status);
    q = f128_add_64_ne(q, fyl2x_B2, &status);
    q = f128_mul_67_chop(w, q, &status);

    /* z = u + u*(p + q) */
    float128_t pq = f128_add_64_ne(p, q, &status);
    float128_t z = f128_mul_67_chop(u, pq, &status);
    z = f128_add_67_chop(u, z, &status);
    return z;
}

// closing step common to both instructions: result = z * y * 2^extraExp, rounded
// to the destination honouring the FPU mode; C1 reflects only this rounding.
// z (a log value) is always a normal float128, but y and the result may be
// denormal, and z*y can sit far below the float128 range while the true x87
// result is still normal (tiny y, huge |log2 x|).  So the multiply runs on y's
// [1,2) mantissa (a normal 113-bit product) and y's exponent - plus 'extraExp'
// for the FYL2XP1 tiny-|x| path - is applied by the closing f128_to_extF80,
// which does the single destination rounding and the masked / unmasked
// (0x6000 wrap) underflow response.
static floatx80 fyl2x_finish(float128_t z, floatx80 y, Bit32s extraExp, softfloat_status_t &status)
{
    status.softfloat_exceptionFlags &= ~RAISE_SW_C1;

    int    ySign = extF80_sign(y);
    Bit32s yExp  = extF80_exp(y);
    Bit64u ySig  = extF80_fraction(y);
    if (! yExp) {
        struct exp32_sig64 n = softfloat_normSubnormalExtF80Sig(ySig);
        yExp = n.exp + 1;
        ySig = n.sig;
    }
    uint8_t roundingMode = softfloat_getRoundingMode(&status);
    float128_t prod = f128_mul_by_extF80(z, packFloatx80(ySign, FLOATX80_EXP_BIAS, ySig), roundingMode, &status);
    return f128_to_extF80(prod, (yExp - FLOATX80_EXP_BIAS) + extraExp, roundingMode, &status);
}

// the [1,2) mantissa of a normalized 64-bit significand as a float128_t
// (exponent field 2^0).
static float128_t fyl2x_mantissa128(Bit64u sig64)
{
    float128_t z;
    z.v64 = (BX_CONST64(0x3FFF) << 48) | ((sig64 >> 15) & BX_CONST64(0x0000FFFFFFFFFFFF));
    z.v0  = sig64 << 49;
    return z;
}

// =================================================
// FYL2X                   Compute y * log (x)
//                                        2
// =================================================

floatx80 fyl2x(floatx80 a, floatx80 b, softfloat_status_t &status)
{
    // handle unsupported extended double-precision floating encodings
    if (extF80_isUnsupported(a) || extF80_isUnsupported(b)) {
invalid:
        softfloat_raiseFlags(&status, softfloat_flag_invalid);
        return floatx80_default_nan;
    }

    Bit64u aSig = extF80_fraction(a);
    Bit32s aExp = extF80_exp(a);
    int aSign = extF80_sign(a);
    Bit64u bSig = extF80_fraction(b);
    Bit32s bExp = extF80_exp(b);
    int bSign = extF80_sign(b);

    int zSign = bSign ^ 1;

    if (aExp == 0x7FFF) {
        if ((aSig<<1) || ((bExp == 0x7FFF) && (bSig<<1))) {
            return softfloat_propagateNaNExtF80UI(a.signExp, aSig, b.signExp, bSig, &status);
        }
        if (aSign) goto invalid;
        else {
            if (! bExp) {
                if (! bSig) goto invalid;
                softfloat_raiseFlags(&status, softfloat_flag_denormal);
            }
            return packFloatx80(bSign, 0x7FFF, BX_CONST64(0x8000000000000000));
        }
    }
    if (bExp == 0x7FFF) {
        if (bSig << 1)
            return softfloat_propagateNaNExtF80UI(a.signExp, aSig, b.signExp, bSig, &status);
        if (aSign && (Bit64u)(aExp | aSig)) goto invalid;
        if (aSig && ! aExp)
            softfloat_raiseFlags(&status, softfloat_flag_denormal);
        if (aExp < 0x3FFF) {
            return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
        }
        if (aExp == 0x3FFF && ! (aSig<<1)) goto invalid;
        return packFloatx80(bSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (! aExp) {
        if (! aSig) {
            if ((bExp | bSig) == 0) goto invalid;
            softfloat_raiseFlags(&status, softfloat_flag_divbyzero);
            return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
        }
        if (aSign) goto invalid;
        softfloat_raiseFlags(&status, softfloat_flag_denormal);
        struct exp32_sig64 normExpSig = softfloat_normSubnormalExtF80Sig(aSig);
        aExp = normExpSig.exp + 1;
        aSig = normExpSig.sig;
    }
    if (aSign) goto invalid;
    if (! bExp) {
        if (! bSig) {
            if (aExp < 0x3FFF) return packFloatx80(zSign, 0, 0);
            return packFloatx80(bSign, 0, 0);
        }
        softfloat_raiseFlags(&status, softfloat_flag_denormal);
        struct exp32_sig64 normExpSig = softfloat_normSubnormalExtF80Sig(bSig);
        bExp = normExpSig.exp + 1;
        bSig = normExpSig.sig;
    }
    if (aExp == 0x3FFF && ! (aSig<<1))
        return packFloatx80(bSign, 0, 0);

    softfloat_raiseFlags(&status, softfloat_flag_inexact);

    /* ***** P5/P6 table-driven log2 ***** */

    bool step4 = ((aExp == 0x3FFE && aSig > BX_CONST64(0xE000000000000000)) ||
                  (aExp == 0x3FFF && aSig < BX_CONST64(0x9000000000000000)));

    float128_t z;
    if (step4) {
        float128_t xq = extF80_to_f128(packFloatx80(0, aExp, aSig), &status);
        float128_t sarg = f128_add_64_ne(xq, float128_negone, &status);   /* x - 1 */
        float128_t t    = f128_add_67_chop(xq, float128_one, &status);    /* x + 1 */
        z = fyl2x_poly(sarg, t, status);
    } else {
        z = fyl2x_step5(fyl2x_mantissa128(aSig), aExp - 0x3FFF, status);
    }

    return fyl2x_finish(z, b, 0, status);
}

// =================================================
// FYL2XP1                 Compute y * log (x + 1)
//                                        2
// =================================================

floatx80 fyl2xp1(floatx80 a, floatx80 b, softfloat_status_t &status)
{
    Bit32s aExp, bExp;
    Bit64u aSig, bSig;
    int aSign, bSign;

    // handle unsupported extended double-precision floating encodings
    if (extF80_isUnsupported(a) || extF80_isUnsupported(b)) {
invalid:
        softfloat_raiseFlags(&status, softfloat_flag_invalid);
        return floatx80_default_nan;
    }

    aSig = extF80_fraction(a);
    aExp = extF80_exp(a);
    aSign = extF80_sign(a);
    bSig = extF80_fraction(b);
    bExp = extF80_exp(b);
    bSign = extF80_sign(b);
    int zSign = aSign ^ bSign;

    if (aExp == 0x7FFF) {
        if ((aSig<<1) != 0 || ((bExp == 0x7FFF) && (bSig<<1) != 0)) {
            return softfloat_propagateNaNExtF80UI(a.signExp, aSig, b.signExp, bSig, &status);
        }
        if (aSign) goto invalid;
        else {
            if (! bExp) {
                if (! bSig) goto invalid;
                softfloat_raiseFlags(&status, softfloat_flag_denormal);
            }
            return packFloatx80(bSign, 0x7FFF, BX_CONST64(0x8000000000000000));
        }
    }
    if (bExp == 0x7FFF)
    {
        if (bSig << 1)
            return softfloat_propagateNaNExtF80UI(a.signExp, aSig, b.signExp, bSig, &status);

        if (! aExp) {
            if (! aSig) goto invalid;
            softfloat_raiseFlags(&status, softfloat_flag_denormal);
        }

        return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (! aExp) {
        if (! aSig) {
            if (bSig && ! bExp) softfloat_raiseFlags(&status, softfloat_flag_denormal);
            return packFloatx80(zSign, 0, 0);
        }
        softfloat_raiseFlags(&status, softfloat_flag_denormal);
        struct exp32_sig64 normExpSig = softfloat_normSubnormalExtF80Sig(aSig);
        aExp = normExpSig.exp + 1;
        aSig = normExpSig.sig;
    }
    if (! bExp) {
        if (! bSig) return packFloatx80(zSign, 0, 0);
        softfloat_raiseFlags(&status, softfloat_flag_denormal);
        struct exp32_sig64 normExpSig = softfloat_normSubnormalExtF80Sig(bSig);
        bExp = normExpSig.exp + 1;
        bSig = normExpSig.sig;
    }

    softfloat_raiseFlags(&status, softfloat_flag_inexact);

    if (aSign && aExp >= 0x3FFF)
        return a;

    /* ***** P5/P6 table-driven log2(1+x) ***** */

    float128_t z;
    if (aExp >= 0x3FFC) {
        // |x| >= 1/8 : reduce w = 1 + x with the table-driven path
        float128_t xq = extF80_to_f128(packFloatx80(aSign, aExp, aSig), &status);
        float128_t w = f128_add_67_chop(xq, float128_one, &status);
        int n = (int)((w.v64 >> 48) & 0x7FFF) - 0x3FFF;
        float128_t wm;
        wm.v64 = (w.v64 & BX_CONST64(0x0000FFFFFFFFFFFF)) | (BX_CONST64(0x3FFF) << 48);
        wm.v0  = w.v0;
        z = fyl2x_step5(wm, n, status);
    }
    else if (aExp < FLOATX80_EXP_BIAS - 70) {
        // |x| < 2^-70 : log2(1+x) == x/ln2 to more than 67 bits.  x/ln2 alone is
        // below the float128 exponent range for a denormal x (though y*log2(1+x)
        // is a normal number), so evaluate  L_inv * mantissa(x)  and let
        // fyl2x_finish add x's exponent back at the closing round.
        float128_t z = f128_mul_67_chop(fyl2x_L_inv,
                           extF80_to_f128(packFloatx80(aSign, FLOATX80_EXP_BIAS, aSig), &status), &status);
        return fyl2x_finish(z, b, aExp - FLOATX80_EXP_BIAS, status);
    }
    else {
        // |x| < 1/8 : polynomial,  u = 2x/(x+2)
        float128_t xq = extF80_to_f128(packFloatx80(aSign, aExp, aSig), &status);
        float128_t t = f128_add_67_chop(xq, float128_two, &status);
        z = fyl2x_poly(xq, t, status);
    }

    return fyl2x_finish(z, b, 0, status);
}
