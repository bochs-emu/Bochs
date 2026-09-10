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
 * fpatan.cc -- emulation of the Intel x87 FPATAN instruction  (atan2(y, x))
 * =======================================================================
 * y = ST(1), x = ST(0).  Table-driven arc tangent:
 *
 *  1. REDUCTION.  Let u = max(|x|,|y|), v = min, remembering whether they were
 *     swapped.  Form the integer quotient k and IEEE remainder r of  32*v  by
 *     u  (so  32v = k*u + r,  |r| <= u/2,  k in 0..32).
 *
 *  2. APPROXIMATION of arctan(v/u):
 *       k >= 2 : arctan(v/u) = arctan(k/32) + arctan(z),  where
 *                z = r / (32u + k*v)  is small.  arctan(k/32) comes from a
 *                33-entry table; arctan(z) from a short odd polynomial.
 *       k <= 1 : arctan(v/u) directly from a longer odd polynomial in v/u.
 *
 *  3. RECONSTRUCTION.  atan2 is assembled from arctan(v/u), the swap flag and
 *     the signs of x and y: the answer is  s*w  (no swap, x>0), or one of
 *     +-pi/2 +- w  /  +-pi +- w  otherwise.
 *
 * The break-point reduction runs in a rescaled fixed-point domain (the larger
 * operand normalised to [1,2)) so nothing overflows the float128_t exponent
 * range.  Every intermediate rounds to the x87 internal extended-precision
 * widths (f128_67.cc helpers); only the closing add is rounded to the
 * destination honouring the FPU mode, and it alone sets SW.C1.
 */

#define FLOAT128

#include "fpu_trans.h"
#include "fpu_constant.h"
#include "fpatan_constants.h"

extern float128_t f128_mul_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_add_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_div_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_add_64_ne (float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_mul_64_ne (float128_t a, float128_t b, struct softfloat_status_t *status);

#define FPATAN_SIGNBIT BX_CONST64(0x8000000000000000)

//////////////////////////////
// 3PI/4 constant
//////////////////////////////

#define FLOATX80_3PI4_EXP (0x4000)

// 128-bit 3PI/4 fraction
#ifdef BETTER_THAN_PENTIUM
#define FLOAT_3PI4_HI (BX_CONST64(0x96cbe3f9990e91a7))
#define FLOAT_3PI4_LO (BX_CONST64(0x9394c9e8a0a5159c))
#else
#define FLOAT_3PI4_HI (BX_CONST64(0x96cbe3f9990e91a7))
#define FLOAT_3PI4_LO (BX_CONST64(0x9000000000000000))
#endif

static const floatx80 floatx80_pi = packFloatx80(0, 0x4000, BX_CONST64(0xc90fdaa22168c235));

static const float128_t float128_zero  = packFloat128(0, 0);

static const float128_t fpatan_pi    = packFloat128(FPATAN_PI_HI,    FPATAN_PI_LO);
static const float128_t fpatan_piby2 = packFloat128(FPATAN_PIBY2_HI, FPATAN_PIBY2_LO);

static const float128_t fpatan_A1 = packFloat128(FPATAN_A1_HI, FPATAN_A1_LO);
static const float128_t fpatan_A2 = packFloat128(FPATAN_A2_HI, FPATAN_A2_LO);
static const float128_t fpatan_A3 = packFloat128(FPATAN_A3_HI, FPATAN_A3_LO);
static const float128_t fpatan_A4 = packFloat128(FPATAN_A4_HI, FPATAN_A4_LO);
static const float128_t fpatan_B1 = packFloat128(FPATAN_B1_HI, FPATAN_B1_LO);
static const float128_t fpatan_B2 = packFloat128(FPATAN_B2_HI, FPATAN_B2_LO);
static const float128_t fpatan_B3 = packFloat128(FPATAN_B3_HI, FPATAN_B3_LO);
static const float128_t fpatan_B4 = packFloat128(FPATAN_B4_HI, FPATAN_B4_LO);
static const float128_t fpatan_B5 = packFloat128(FPATAN_B5_HI, FPATAN_B5_LO);
static const float128_t fpatan_B6 = packFloat128(FPATAN_B6_HI, FPATAN_B6_LO);

static float128_t fpatan_atan_tab(unsigned k)
{
    return packFloat128(fpatan_atan_hi[k], fpatan_atan_lo[k]);
}

// closing step: result = (a +) f + g, rounded once to the destination with the
// FPU mode.  C1 reflects only that final rounding, so the round-up state left by
// the earlier intermediate roundings is discarded first.
//   sx = sign(x), sy = sign(y), sflag = 1 when |x|,|y| were swapped
static floatx80 fpatan_finish(float128_t f, float128_t g, int sflag, int sx, int sy, softfloat_status_t &status)
{
    float128_t val;

    if (! sx && ! sflag) {
        val = f128_add(f, g, &status);                 /* add_e(f, g) */
    }
    else {
        float128_t a = sflag ? fpatan_piby2 : fpatan_pi;   /* (-1)^sy * (pi/2 or pi) */
        if (sy) a.v64 ^= FPATAN_SIGNBIT;
        float128_t fg = f128_add_67_chop(f, g, &status);
        val = f128_add(a, fg, &status);                 /* add_e(a, f+g) */
    }

    status.softfloat_exceptionFlags &= ~RAISE_SW_C1;
    return f128_to_extF80(val, &status);
}

// =================================================
// FPATAN                  Compute arctan(y/x)
// =================================================

floatx80 fpatan(floatx80 a, floatx80 b, softfloat_status_t &status)
{
    // handle unsupported extended double-precision floating encodings
    if (extF80_isUnsupported(a) || extF80_isUnsupported(b)) {
        softfloat_raiseFlags(&status, softfloat_flag_invalid);
        return floatx80_default_nan;
    }

    Bit64u aSig = extF80_fraction(a);
    Bit32s aExp = extF80_exp(a);
    int aSign = extF80_sign(a);
    Bit64u bSig = extF80_fraction(b);
    Bit32s bExp = extF80_exp(b);
    int bSign = extF80_sign(b);

    if (bExp == 0x7FFF)
    {
        if (bSig<<1)
            return softfloat_propagateNaNExtF80UI(a.signExp, aSig, b.signExp, bSig, &status);

        if (aExp == 0x7FFF) {
            if (aSig<<1)
                return softfloat_propagateNaNExtF80UI(a.signExp, aSig, b.signExp, bSig, &status);

            if (aSign)     /* return 3PI/4 */
                return softfloat_roundPackToExtF80(bSign, FLOATX80_3PI4_EXP, FLOAT_3PI4_HI, FLOAT_3PI4_LO, 80, &status);
            else           /* return  PI/4 */
                return softfloat_roundPackToExtF80(bSign, FLOATX80_PI4_EXP, FLOAT_PI_HI, FLOAT_PI_LO, 80, &status);
        }

        if (aSig && ! aExp)
            softfloat_raiseFlags(&status, softfloat_flag_denormal);

        /* return PI/2 */
        return softfloat_roundPackToExtF80(bSign, FLOATX80_PI2_EXP, FLOAT_PI_HI, FLOAT_PI_LO, 80, &status);
    }
    if (aExp == 0x7FFF)
    {
        if (aSig<<1)
            return softfloat_propagateNaNExtF80UI(a.signExp, aSig, b.signExp, bSig, &status);

        if (bSig && ! bExp)
            softfloat_raiseFlags(&status, softfloat_flag_denormal);

return_PI_or_ZERO:

        if (aSign)   /* return PI */
            return softfloat_roundPackToExtF80(bSign, FLOATX80_PI_EXP, FLOAT_PI_HI, FLOAT_PI_LO, 80, &status);
        else         /* return  0 */
            return packToExtF80(bSign, 0, 0);
    }
    if (! bExp)
    {
        if (! bSig) {
             if (aSig && ! aExp) softfloat_raiseFlags(&status, softfloat_flag_denormal);
             goto return_PI_or_ZERO;
        }

        softfloat_raiseFlags(&status, softfloat_flag_denormal);
        struct exp32_sig64 normExpSig = softfloat_normSubnormalExtF80Sig(bSig);
        bExp = normExpSig.exp + 1;
        bSig = normExpSig.sig;
    }
    if (! aExp)
    {
        if (! aSig)   /* return PI/2 */
            return softfloat_roundPackToExtF80(bSign, FLOATX80_PI2_EXP, FLOAT_PI_HI, FLOAT_PI_LO, 80, &status);

        softfloat_raiseFlags(&status, softfloat_flag_denormal);
        struct exp32_sig64 normExpSig = softfloat_normSubnormalExtF80Sig(aSig);
        aExp = normExpSig.exp + 1;
        aSig = normExpSig.sig;
    }

    softfloat_raiseFlags(&status, softfloat_flag_inexact);

    /* ***** P5/P6 table-driven arc tangent ***** */
    /*   x = ST(0) = 'a' (denominator),  y = ST(1) = 'b' (numerator)          */

    int sx = aSign;
    int sy = bSign;
    int sz = sx ^ sy;
    int sflag = 0;

    /* aExp / bExp are biased and may be <= 0 after a denormal was normalized, so
       keep the exponents as plain ints and never repack them verbatim.
       Pick the larger magnitude as (uExp,uSig), the smaller as (vExp,vSig). */
    Bit32s uExp = aExp, vExp = bExp;
    Bit64u uSig = aSig, vSig = bSig;
    if (aExp < bExp || (aExp == bExp && aSig < bSig)) {
        uExp = bExp; uSig = bSig;
        vExp = aExp; vSig = aSig;
        sflag = 1;
        sz ^= 1;
    }

    /* Rescale the larger operand into [1,2) (biased exp 0x3FFF) and the smaller
       by the same shift.  The approximation depends only on the ratio v/u, so
       this is exact and keeps every value inside the float128_t exponent range. */
    Bit32s vBias = 0x3FFF + (vExp - uExp);   /* biased exp of the smaller after rescale */

    float128_t f, g;

    if (vBias < 1 && ! sx && ! sflag)
    {
        /* |y/x| below 2^-16382 and no pi term : arctan(y/x) == y/x, a genuine
           tiny value that underflows the destination.  y/x is below the float128
           exponent range, so divide the [1,2) mantissas (both operands were
           normalised above; V with vBias <= 0 could not be packed) and carry the
           exponent difference to the single closing round. */
        status.softfloat_exceptionFlags &= ~RAISE_SW_C1;
        float128_t q = f128_div_67_chop(
            extF80_to_f128(packToExtF80(sy, 0x3FFF, bSig), &status),
            extF80_to_f128(packToExtF80(0,  0x3FFF, aSig), &status), &status);
        return f128_to_extF80(q, bExp - aExp, softfloat_getRoundingMode(&status), &status);
    }

    if (vBias < 1)
    {
        /* |y/x| is below 2^-16382 : arctan(v/u) == v/u to well beyond 64 bits.
           The result is dominated by +-pi or +-pi/2 ; the arctan term vanishes. */
        f = float128_zero;
        g = float128_zero;
    }
    else
    {
        extFloat80_t U   = packToExtF80(0, (uint16_t) 0x3FFF,      uSig);
        extFloat80_t V   = packToExtF80(0, (uint16_t) vBias,       vSig);
        extFloat80_t V32 = packToExtF80(0, (uint16_t)(vBias + 5),  vSig);   /* 32*v */

        /* truncated remainder: 32v = k*u + r0 with 0 <= r0 < u, k = floor(32v/u).
           The remainder step is exact, so it must not leak exception flags. */
        unsigned savedFlags = status.softfloat_exceptionFlags;
        floatx80 r80;
        Bit64u k = 0;
        floatx80_remainder(V32, U, r80, k, &status);

        float128_t u128 = extF80_to_f128(U, &status);
        float128_t r128 = extF80_to_f128(r80, &status);

        /* fold to |r| <= u/2 : if r0 > u/2 then r := r0 - u, k := k + 1 */
        float128_t uHalf = u128;
        uHalf.v64 -= (BX_CONST64(1) << 48);
        if (r128.v64 > uHalf.v64 || (r128.v64 == uHalf.v64 && r128.v0 > uHalf.v0)) {
            r128 = f128_sub(r128, u128, &status);
            k++;
        }
        status.softfloat_exceptionFlags = savedFlags;

        if (k > 32) k = 32;

        float128_t v128 = extF80_to_f128(V, &status);

        if (k > 1)
        {
            /* Step 3.1 : table lookup + low order polynomial (32v/u >= 2) */
            float128_t u1 = u128;
            u1.v64 += (BX_CONST64(5) << 48);                          /* u1 = u * 32   */
            float128_t v1 = f128_mul_67_chop(v128, i32_to_f128((int) k), &status);
            float128_t s  = f128_add_67_chop(u1, v1, &status);        /* s = u1 + v1   */
            float128_t z  = f128_div_67_chop(r128, s, &status);       /* z = r / s     */
            float128_t c  = f128_mul_64_ne(z, z, &status);            /* c = z*z       */
            float128_t h  = f128_mul_67_chop(c, c, &status);          /* h = c*c       */
            if (sz) z.v64 ^= FPATAN_SIGNBIT;                          /* z := (-1)^sz z */
            float128_t t  = f128_mul_67_chop(z, c, &status);          /* t = z*c       */

            float128_t p = f128_mul_67_chop(h, fpatan_A3, &status);
            p = f128_add_67_chop(fpatan_A1, p, &status);              /* p = A1 + h*A3 */

            float128_t q = f128_mul_67_chop(h, fpatan_A4, &status);
            q = f128_add_64_ne(fpatan_A2, q, &status);
            q = f128_mul_67_chop(c, q, &status);                      /* q = c*(A2 + h*A4) */

            f = fpatan_atan_tab((unsigned) k);                        /* arctan(k/32)  */
            if (sz) f.v64 ^= FPATAN_SIGNBIT;

            float128_t pq = f128_add_64_ne(p, q, &status);
            g = f128_mul_67_chop(t, pq, &status);
            g = f128_add_67_chop(z, g, &status);                      /* g = z + t*(p+q) */
        }
        else
        {
            /* Step 3.2 : high order polynomial in z = v/u  (32v/u < 2) */
            float128_t z = f128_div_67_chop(v128, u128, &status);
            if (sz) z.v64 ^= FPATAN_SIGNBIT;

            if (((z.v64 >> 48) & 0x7FFF) < (Bit32u)(0x3FFF - 40)) {
                /* |z| < 2^-40 */
                f = z;
                g = float128_zero;
            }
            else {
                float128_t c = f128_mul_64_ne(z, z, &status);         /* c = z*z */
                float128_t h = f128_mul_67_chop(c, c, &status);       /* h = c*c */
                float128_t t = f128_mul_67_chop(z, c, &status);       /* t = z*c */

                float128_t p = f128_mul_67_chop(h, fpatan_B5, &status);
                p = f128_add_64_ne(p, fpatan_B3, &status);
                p = f128_mul_67_chop(h, p, &status);
                p = f128_add_67_chop(p, fpatan_B1, &status);          /* p = B1 + h*(B3 + h*B5) */

                float128_t q = f128_mul_67_chop(h, fpatan_B6, &status);
                q = f128_add_64_ne(q, fpatan_B4, &status);
                q = f128_mul_67_chop(h, q, &status);
                q = f128_add_67_chop(q, fpatan_B2, &status);
                q = f128_mul_67_chop(q, c, &status);                  /* q = (B2 + h*(B4 + h*B6))*c */

                f = z;
                float128_t pq = f128_add_64_ne(p, q, &status);
                g = f128_mul_67_chop(t, pq, &status);                 /* g = t*(p+q) */
            }
        }
    }

    return fpatan_finish(f, g, sflag, sx, sy, status);
}
