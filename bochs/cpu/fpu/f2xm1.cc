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
 * f2xm1.c -- emulation of the Intel x87 F2XM1 instruction
 * ---------------------------------------------------------------
 * Intermediate arithmetic is carried out in float128_t, following the
 * extended-precision arithmetic of the real P5/P6 microcode: multiplies
 * keep a ~67-bit significand and truncate, polynomial adds deliver a
 * 64-bit significand rounded to nearest-even.
 * Constants and the 2^c-1 table are decoded at full precision from the
 * implementation-note values. The reduction step (isolating the break
 * point c and remainder r = x - c) is done in extFloat80_t because it
 * is defined bitwise on x's 64-bit significand. Only the final
 * reconstruction (d + (d+1)*poly, or s + (u+p+q)) is done at full
 * precision and rounded once, honouring the FPU rounding mode.
 */

#include "softfloat3e/include/softfloat.h"
#define FLOAT128
#include "softfloat-specialize.h"

#include "f2xm1_constants.h"

extern floatx80 softfloat_propagateNaNExtF80UI(uint16_t uiA64, uint64_t uiA0, uint16_t uiB64, uint64_t uiB0, struct softfloat_status_t *status);

/* f128_67.cc: P5/P6 extended-precision arithmetic - multiply keeps a ~67-bit
   significand and truncates; add delivers a 64-bit significand, nearest-even. */
extern float128_t f128_mul_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_add_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_mul_64_ne(float128_t a, extFloat80_t b, struct softfloat_status_t *status);
extern float128_t f128_mul_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status);

/*
   Given a number x in the range (-1, 1), the calculation of 2^x - 1 can be
   considered in two cases. (We ignore the case x = 1 or x = -1 because
   they can handled separately and easily.)

   First, suppose x is small, i.e. |x| < 1/4. Then the calculation is quite
   simple:  2^x - 1 = exp( x log 2 ) - 1. Hence, given a polynomial
   approximation  poly(y) that approximates exp(y)-1 well on the interval
   [-log2/4, log2/4], 2^x - 1 can be simply approximated by poly( x*log2 ).

   Next, suppose |x| >= 1/4. Then we can employ a table-driven technique.
   This can be described in three steps.

   REDUCTION: A break point c is so chosen that |x-c| <= 1/64. The
   difference r = x - c is then computed. In our case, the base-2
   representation of x is:

        x  =  sign * 2^k * (1.b1 b2 b3 b4 ..... b63)

   where sign is 1 or -1, and k = -1 or -2. Then c is chosen to be

        c  =  sign * 2^k * (1.b1 b2 b3 b4 1)

   Clearly, there are 64 possible c's and |x-c| <= 1/64.

   APPROXIMATION: 2^r - 1 is computed. This is accomplished by a polynomial
   approximation poly(r*log2) where poly(y) approximates exp(y)-1 well on
   the interval [-log2/64, log2/64].

   RECONSTRUCTION: 2^x-1 is reconstructed based on the mathematical
   relationships

          2^x - 1  =   2^(c+r) - 1
                   =   2^c * 2^r - 1
                   =   2^c * (2^r - 1  + 1) - 1
                   =  (2^c - 1)  +  2^c * (2^r - 1)
                   =   d  +  (d+1)*poly(r*log2)    approximately.

   where d = 2^c is a value calculated beforehand and stored in a table,
   hence the name "table-driven" algorithm.
*/

/* ------------------------------------------------------------------ */
/* Constants                                                          */
/* ------------------------------------------------------------------ */

static const floatx80 floatx80_negone = packToExtF80(1, 0x3fff, BX_CONST64(0x8000000000000000));

static const float128_t f2xm1_C1 = packFloat128(BX_CONST64(0x3FFF000000000000), 0); /* 1.0 */
static const float128_t f2xm1_L2 = packFloat128(EXP_L2_HI, EXP_L2_LO);              /* log(2) */

/* Step-5 (|x| >= 1/4) coefficients */
static const float128_t f2xm1_A1 = packFloat128(EXP_A_1_HI, EXP_A_1_LO);
static const float128_t f2xm1_A2 = packFloat128(EXP_A_2_HI, EXP_A_2_LO);
static const float128_t f2xm1_A3 = packFloat128(EXP_A_3_HI, EXP_A_3_LO);
static const float128_t f2xm1_A4 = packFloat128(EXP_A_4_HI, EXP_A_4_LO);
static const float128_t f2xm1_A5 = packFloat128(EXP_A_5_HI, EXP_A_5_LO);
static const float128_t f2xm1_A6 = packFloat128(EXP_A_6_HI, EXP_A_6_LO);

/* Step-4 (|x| < 1/4) coefficients */
static const float128_t f2xm1_B1  = packFloat128(EXP_B_1_HI,  EXP_B_1_LO);
static const float128_t f2xm1_B2  = packFloat128(EXP_B_2_HI,  EXP_B_2_LO);
static const float128_t f2xm1_B3  = packFloat128(EXP_B_3_HI,  EXP_B_3_LO);
static const float128_t f2xm1_B4  = packFloat128(EXP_B_4_HI,  EXP_B_4_LO);
static const float128_t f2xm1_B5  = packFloat128(EXP_B_5_HI,  EXP_B_5_LO);
static const float128_t f2xm1_B6  = packFloat128(EXP_B_6_HI,  EXP_B_6_LO);
static const float128_t f2xm1_B7  = packFloat128(EXP_B_7_HI,  EXP_B_7_LO);
static const float128_t f2xm1_B8  = packFloat128(EXP_B_8_HI,  EXP_B_8_LO);
static const float128_t f2xm1_B9  = packFloat128(EXP_B_9_HI,  EXP_B_9_LO);
static const float128_t f2xm1_B10 = packFloat128(EXP_B_10_HI, EXP_B_10_LO);
static const float128_t f2xm1_B11 = packFloat128(EXP_B_11_HI, EXP_B_11_LO);

/* 64-entry table of d = 2^c - 1 */
static float128_t f2xm1_table_d(int index)
{
    return packFloat128(f2xm1_table_hi[index], f2xm1_table_lo[index]);
}

/* The final reconstruction add is done at full precision and rounded once to
   the destination, honouring the FPU rounding mode.  Only that final rounding
   determines SW.C1 (round-up): the round-up state accumulated by the earlier
   intermediate roundings is discarded first. */
static extFloat80_t f2xm1_reconstruct(float128_t a, float128_t b, softfloat_status_t &status)
{
    float128_t sum = f128_add(a, b, &status);   /* final reconstruction add, full precision */
    status.softfloat_exceptionFlags &= ~RAISE_SW_C1;
    return f128_to_extF80(sum, &status);        /* single rounding, per FPU mode */
}

/* ------------------------------------------------------------------ */
/* Step 5 :  1/4 <= |x| < 1   (REDUCTION / APPROXIMATION / RECONSTRUCTION) */
/* ------------------------------------------------------------------ */

static extFloat80_t f2xm1_step5(extFloat80_t x, softfloat_status_t &status)
{
    int sign        = extF80_sign(x);
    int biasedExp   = extF80_exp(x);
    uint64_t signif = extF80_fraction(x);

    /* exponent is -1 (biasedExp == 16382) or -2 (biasedExp == 16381) */
    int kbit   = (biasedExp == (16383 - 2)) ? 1 : 0;
    int b1234  = (int)((signif >> 59) & 0xF);
    int index  = (sign << 5) | (kbit << 4) | b1234;

    /* c = sign * 2^k * (1.b1 b2 b3 b4 1) : keep top 5 significand bits
       (implicit '1' + b1..b4), force the next bit (b5) to 1, zero the rest */
    uint64_t signif_c = (signif & (~BX_CONST64(0) << 59)) | (BX_CONST64(1) << 58);
    extFloat80_t c = packToExtF80(x.signExp, signif_c);

    extFloat80_t r = extF80_sub(x, c, &status);

    /* s = L2 * r, rounded to extended (64-bit) precision */
    float128_t s = f128_mul_64_ne(f2xm1_L2, r, &status);
    float128_t t = f128_mul_67_chop(s, s, &status);

    float128_t d  = f2xm1_table_d(index);                  /* d  = get_table(index) */
    float128_t d1 = f128_add_64_ne(d, f2xm1_C1, &status);  /* d1 = d + 1.0 */

    /* p = t*(A2 + t*(A4 + t*A6)) ; p = s*p */
    float128_t p = f128_mul_67_chop(t, f2xm1_A6, &status);
    p = f128_add_64_ne(f2xm1_A4, p, &status);
    p = f128_mul_67_chop(t, p, &status);
    p = f128_add_64_ne(f2xm1_A2, p, &status);
    p = f128_mul_67_chop(t, p, &status);
    p = f128_mul_67_chop(s, p, &status);

    /* q = s + t*(A1 + t*(A3 + t*A5)) */
    float128_t q = f128_mul_67_chop(t, f2xm1_A5, &status);
    q = f128_add_64_ne(f2xm1_A3, q, &status);
    q = f128_mul_67_chop(t, q, &status);
    q = f128_add_64_ne(f2xm1_A1, q, &status);
    q = f128_mul_67_chop(t, q, &status);
    q = f128_add_64_ne(s, q, &status);

    float128_t f  = f128_add_64_ne(p, q, &status);         /* f = p + q  */
    float128_t d1f = f128_mul_67_chop(d1, f, &status);
    return f2xm1_reconstruct(d, d1f, status);              /* result = d + d1*f  */
}

/* ------------------------------------------------------------------ */
/* Step 4 :  2^-68 <= |x| < 1/4                                        */
/* ------------------------------------------------------------------ */

static extFloat80_t f2xm1_step4(extFloat80_t x, softfloat_status_t &status)
{
    float128_t xq = extF80_to_f128(x, &status);
    float128_t s  = f128_mul_67_chop(f2xm1_L2, xq, &status);
    /* s1 = L2 * x, rounded to extended (64-bit) precision */
    float128_t s1 = f128_mul_64_ne(f2xm1_L2, x, &status);
    float128_t t  = f128_mul_67_chop(s, s1, &status);

    float128_t u = f128_mul_67_chop(t, f2xm1_B1, &status);   /* u = t * b1  (b1 = 0.5)  */

    /* p = b2 + t*(b4 + t*(b6 + t*(b8 + t*b10))) ; p = t*p ; p = s*p */
    float128_t p = f128_mul_67_chop(t, f2xm1_B10, &status);
    p = f128_add_64_ne(f2xm1_B8, p, &status);
    p = f128_mul_67_chop(t, p, &status);
    p = f128_add_64_ne(f2xm1_B6, p, &status);
    p = f128_mul_67_chop(t, p, &status);
    p = f128_add_64_ne(f2xm1_B4, p, &status);
    p = f128_mul_67_chop(t, p, &status);
    p = f128_add_64_ne(f2xm1_B2, p, &status);
    /* p = t*p, rounded to extended (64-bit) precision */
    p = f128_mul_64_ne(t, p, &status);
    p = f128_mul_67_chop(s, p, &status);

    /* q = b3 + t*(b5 + t*(b7 + t*(b9 + t*b11))) ; q = t*q ; q = t*q */
    float128_t q = f128_mul_67_chop(t, f2xm1_B11, &status);
    q = f128_add_64_ne(f2xm1_B9, q, &status);
    q = f128_mul_67_chop(t, q, &status);
    q = f128_add_64_ne(f2xm1_B7, q, &status);
    q = f128_mul_67_chop(t, q, &status);
    q = f128_add_64_ne(f2xm1_B5, q, &status);
    q = f128_mul_67_chop(t, q, &status);
    q = f128_add_64_ne(f2xm1_B3, q, &status);
    /* q = t*q, rounded to extended (64-bit) precision */
    q = f128_mul_64_ne(t, q, &status);
    q = f128_mul_67_chop(t, q, &status);

    float128_t pq = f128_add_64_ne(p, q, &status);
    float128_t u_pq = f128_add_64_ne(u, pq, &status);   /* u + (p + q)  */
    return f2xm1_reconstruct(s, u_pq, status);          /* result = s + (u+(p+q)) */
}

/* ------------------------------------------------------------------ */
/* Top level: Steps 1-3 (special cases, domain split, tiny-x shortcut)  */
/* ------------------------------------------------------------------ */

extFloat80_t f2xm1(extFloat80_t x, softfloat_status_t &status)
{
    // handle unsupported extended double-precision floating encodings
    if (extF80_isUnsupported(x)) {
        softfloat_raiseFlags(&status, softfloat_flag_invalid);
        return floatx80_default_nan;
    }

    int sign        = extF80_sign(x);
    int biasedExp   = extF80_exp(x);
    uint64_t signif = extF80_fraction(x);

    if (! biasedExp) {
        if (! signif) return x;

        softfloat_raiseFlags(&status, softfloat_flag_denormal | softfloat_flag_inexact);

    tiny_argument:
        {
            /* 2^x - 1 ~= x * log(2) ; mul_e64 rounds to the destination, FPU mode */
            float128_t prod = f128_mul_by_extF80(f2xm1_L2, x, softfloat_getRoundingMode(&status), &status);
            return f128_to_extF80(prod, &status);
        }
    }

    /* ---- Step 1: NaN, infinities, |x| >= 1 -------------------------- */
    if (biasedExp == 0x7FFF) {
        /* NaN: raise invalid, quiet it if it was signaling, pass through */
        if (signif << 1)
            return softfloat_propagateNaNExtF80UI(x.signExp, signif, 0, 0, &status);

        /* +-infinity: 2^(+inf)-1 = +inf ; 2^(-inf)-1 = -1 */
        if (sign) {
            return floatx80_negone; /* -1.0 */
        }
        return x; /* +inf, unchanged */
    }

    softfloat_raiseFlags(&status, softfloat_flag_inexact);

    if (biasedExp >= 16383) {
        /* |x| >= 1: outside F2XM1's documented [-1,+1] domain. x == +-1
           exactly are well-defined and are computed (not special-cased
           in silicon -- they just happen to round to an exact value),
           which is why hardware still raises PE for them. Per the
           Intel SDM, behaviour for |x| > 1 is architecturally
           undefined; empirically, real hardware returns the operand
           unchanged and raises only PE (never invalid) for that case,
           so that's what we replicate here. */
        if (biasedExp == 16383 && signif == BX_CONST64(0x8000000000000000)) {
            return sign ? packToExtF80(0xBFFE, BX_CONST64(0x8000000000000000))   /* -0.5 */
                        : packToExtF80(0x3FFF, BX_CONST64(0x8000000000000000));  /* +1.0 */
        }
        return x;
    }

    /* ---- Step 2: branch on |x| >= 1/4  (exponent >= -2) -------------- */
    if (biasedExp >= 16383 - 2) {
        return f2xm1_step5(x, status);
    }

    /* ---- Step 3: |x| < 1/4 -------------------------------------------- */
    if (biasedExp <= 16383 - 69) {
        /* |x| < 2^-68 : 2^x - 1 ~= x * log(2), computed directly */
        goto tiny_argument;
    }

    return f2xm1_step4(x, status);
}
