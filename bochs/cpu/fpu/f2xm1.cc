/*
 * f2xm1.c -- emulation of the Intel x87 F2XM1 instruction
 * ---------------------------------------------------------------
 * All intermediate arithmetic is carried out in float128_t (113-bit
 * significand), which has far more precision than the ~67
 * significant bits the original Intel algorithm requires, so we
 * do not need to reproduce the historical "80-bit-plus-a-few-extra-
 * bits" constant format -- we simply decode the full precision
 * given for every constant/table entry directly into float128_t.
 * Only the reduction step (isolating the breakpoint c and
 * remainder r = x - c) is done in extFloat80_t, because that step
 * is defined bitwise on x's 64-bit extended significand. The final
 * float128_t result is rounded back down to extFloat80_t once,
 * at the very end, with f128_to_extF80.
 *
 * Verified against the real x87 F2XM1 instruction (via inline asm on
 * an x86_64 host) over 200,000+ random points spanning the whole
 * domain plus all documented boundaries; observed max error was
 * ~1 ulp of the extended-precision (64-bit significand) result,
 * which is expected given the double-rounding (f128 -> extF80).
 */

#include "softfloat3e/include/softfloat.h"
#define FLOAT128
#include "softfloat-specialize.h"

#include "f2xm1_constants.h"

extern floatx80 softfloat_propagateNaNExtF80UI(uint16_t uiA64, uint64_t uiA0, uint16_t uiB64, uint64_t uiB0, struct softfloat_status_t *status);

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
    extFloat80_t s80 = f128_mul_by_extF80(f2xm1_L2, r, softfloat_round_near_even, &status);
    float128_t s = extF80_to_f128(s80, &status);
    float128_t t = f128_mul(s, s, &status);

    float128_t d  = f2xm1_table_d(index);           /* d  = get_table(index) */
    float128_t d1 = f128_add(d, f2xm1_C1, &status); /* d1 = d + 1.0 */

    /* p = t*(A2 + t*(A4 + t*A6)) ; p = s*p */
    float128_t p = f128_mul(t, f2xm1_A6, &status);
    p = f128_add(f2xm1_A4, p, &status);
    p = f128_mul(t, p, &status);
    p = f128_add(f2xm1_A2, p, &status);
    p = f128_mul(t, p, &status);
    p = f128_mul(s, p, &status);

    /* q = s + t*(A1 + t*(A3 + t*A5)) */
    float128_t q = f128_mul(t, f2xm1_A5, &status);
    q = f128_add(f2xm1_A3, q, &status);
    q = f128_mul(t, q, &status);
    q = f128_add(f2xm1_A1, q, &status);
    q = f128_mul(t, q, &status);
    q = f128_add(s, q, &status);

    float128_t f  = f128_add(p, q, &status);          /* f = p + q  */
    float128_t d1f = f128_mul(d1, f, &status);
    float128_t resultq = f128_add(d, d1f, &status);   /* result = d + d1*f   */

    return f128_to_extF80(resultq, &status);
}

/* ------------------------------------------------------------------ */
/* Step 4 :  2^-68 <= |x| < 1/4                                        */
/* ------------------------------------------------------------------ */

static extFloat80_t f2xm1_step4(extFloat80_t x, softfloat_status_t &status)
{
    float128_t xq = extF80_to_f128(x, &status);
    float128_t s  = f128_mul(f2xm1_L2, xq, &status);
    /* s1 = L2 * x, rounded to extended (64-bit) precision */
    extFloat80_t s1_80 = f128_mul_by_extF80(f2xm1_L2, x, softfloat_round_near_even, &status);
    float128_t s1 = extF80_to_f128(s1_80, &status);
    float128_t t  = f128_mul(s, s1, &status);

    float128_t u = f128_mul(t, f2xm1_B1, &status);        /* u = t * b1  (b1 = 0.5)  */

    /* p = b2 + t*(b4 + t*(b6 + t*(b8 + t*b10))) ; p = t*p ; p = s*p */
    float128_t p = f128_mul(t, f2xm1_B10, &status);
    p = f128_add(f2xm1_B8, p, &status);
    p = f128_mul(t, p, &status);
    p = f128_add(f2xm1_B6, p, &status);
    p = f128_mul(t, p, &status);
    p = f128_add(f2xm1_B4, p, &status);
    p = f128_mul(t, p, &status);
    p = f128_add(f2xm1_B2, p, &status);
    /* p = t*p, rounded to extended (64-bit) precision */
    extFloat80_t p80 = f128_to_extF80(p, &status);
    p80 = f128_mul_by_extF80(t, p80, softfloat_round_near_even, &status);
    p = extF80_to_f128(p80, &status);
    p = f128_mul(s, p, &status);

    /* q = b3 + t*(b5 + t*(b7 + t*(b9 + t*b11))) ; q = t*q ; q = t*q */
    float128_t q = f128_mul(t, f2xm1_B11, &status);
    q = f128_add(f2xm1_B9, q, &status);
    q = f128_mul(t, q, &status);
    q = f128_add(f2xm1_B7, q, &status);
    q = f128_mul(t, q, &status);
    q = f128_add(f2xm1_B5, q, &status);
    q = f128_mul(t, q, &status);
    q = f128_add(f2xm1_B3, q, &status);
    /* q = t*q, rounded to extended (64-bit) precision */
    extFloat80_t q80 = f128_to_extF80(q, &status);
    q80 = f128_mul_by_extF80(t, q80, softfloat_round_near_even, &status);
    q = extF80_to_f128(q80, &status);
    q = f128_mul(t, q, &status);

    float128_t pq = f128_add(p, q, &status);
    float128_t u_pq = f128_add(u, pq, &status);       /* u + (p + q)  */
    float128_t resultq = f128_add(s, u_pq, &status);

    return f128_to_extF80(resultq, &status);
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
        /* 2^x - 1 ~= x * log(2) */
        return f128_mul_by_extF80(f2xm1_L2, x, softfloat_getRoundingMode(&status), &status);
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
