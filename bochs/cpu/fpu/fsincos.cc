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
 * fsincos.cc -- emulation of the Intel x87 FSIN, FCOS, FSINCOS and FPTAN
 * =========================================================================
 *
 * Shared shape of all four instructions
 * -------------------------------------
 *  1. ARGUMENT REDUCTION.  Compute  |x| = n*(pi/2) + r  with  |r| <= pi/4,
 *     where pi/2 is a fixed 66-bit approximation (the x87 has never used a
 *     wider one - hence the classic large-argument inaccuracy, which the
 *     hardware and this code share).  Only n mod 4 (mod 2 for FPTAN) is kept.
 *     'sincos_reduce' does this; see its own comment for the three cases
 *     (no reduction / one subtraction / bit-serial division for |x| >= 2)
 *     and the fold that brings |r| down to pi/4.
 *
 *  2. APPROXIMATION of sin(r) and/or cos(r):
 *       |r| >= 1/4 : table-driven.  Split r against one of 8 break points c
 *                    (c keeps the top 3 significand bits of |r|), so d = |r|-c
 *                    is tiny.  Take sin(c), cos(c) from a table and combine:
 *                        sin(c+d) = sin(c)(1 + (cos(d)-1)) + cos(c) sin(d)
 *                        cos(c+d) = cos(c)(1 + (cos(d)-1)) - sin(c) sin(d)
 *                    with sin(d), cos(d)-1 short polynomials in e = d*d.
 *       |r| < 1/4  : evaluate sin(r) / cos(r) from a direct polynomial in
 *                    s = r*r.
 *
 *  3. RECONSTRUCTION from n mod 4 and sign(x) - each quadrant maps the answer
 *     to +/- sin(r) or +/- cos(r) (FPTAN: to a ratio of the two).
 *
 * Every intermediate rounds to the x87 internal extended-precision widths
 * (f128_67.cc helpers); only the closing add - for FPTAN the closing divide -
 * is rounded to the destination honouring the FPU mode, and it alone sets SW.C1.
 *
 * IMPORTANT - FSIN/FCOS and FSINCOS are NOT the same computation
 * ------------------------------------------------------------------
 * Real hardware evaluates a standalone FSIN/FCOS with a different internal
 * routine than the sin/cos lanes of FSINCOS: different polynomial layout and a
 * different way of folding the head+tail before the final rounding.  On a real
 * CPU  FSIN(x) != FSINCOS(x).sin  for roughly 1 argument in 1500 (always by one
 * ulp), likewise FCOS vs FSINCOS.cos.  So the two must stay separate:
 *   FSIN / FCOS      -> sc_fsin_fcos()   (single result)
 *   FSINCOS          -> sc_fsincos()  (both, head+tail method)
 * Collapsing them onto one path would mismatch the hardware on those cases.
 * FPTAN uses its own third variant, fptan_sin_cos().
 */

#define FLOAT128

#include <stddef.h>

#include "fpu_trans.h"
#include "softfloat-helpers.h"
#include "fpu_constant.h"
#include "fsincos_constants.h"

extern float128_t f128_mul_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_mul_e67   (float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_add_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_add_64_ne (float128_t a, float128_t b, struct softfloat_status_t *status);
extern float128_t f128_mul_64_ne (float128_t a, float128_t b, struct softfloat_status_t *status);

#define SC_SIGNBIT BX_CONST64(0x8000000000000000)

/* pi/2 approximation used for FSIN/FCOS/FSINCOS/FPTAN argument reduction */
#ifndef SC_PI2_HI
#define SC_PI2_HI FLOAT_PI_HI
#define SC_PI2_LO FLOAT_PI_LO
#endif

static const floatx80 floatx80_one = packFloatx80(0, 0x3fff, BX_CONST64(0x8000000000000000));

static const float128_t f128_one = packFloat128(BX_CONST64(0x3fff000000000000), 0);

static const float128_t sc_A[6] = {
    packFloat128(FSINCOS_A1_HI, FSINCOS_A1_LO),
    packFloat128(FSINCOS_A2_HI, FSINCOS_A2_LO),
    packFloat128(FSINCOS_A3_HI, FSINCOS_A3_LO),
    packFloat128(FSINCOS_A4_HI, FSINCOS_A4_LO),
    packFloat128(FSINCOS_A5_HI, FSINCOS_A5_LO),
    packFloat128(FSINCOS_A6_HI, FSINCOS_A6_LO)
};
static const float128_t sc_B[6] = {
    packFloat128(FSINCOS_B1_HI, FSINCOS_B1_LO),
    packFloat128(FSINCOS_B2_HI, FSINCOS_B2_LO),
    packFloat128(FSINCOS_B3_HI, FSINCOS_B3_LO),
    packFloat128(FSINCOS_B4_HI, FSINCOS_B4_LO),
    packFloat128(FSINCOS_B5_HI, FSINCOS_B5_LO),
    packFloat128(FSINCOS_B6_HI, FSINCOS_B6_LO)
};
static const float128_t sc_U[4] = {
    packFloat128(FSINCOS_U1_HI, FSINCOS_U1_LO),
    packFloat128(FSINCOS_U2_HI, FSINCOS_U2_LO),
    packFloat128(FSINCOS_U3_HI, FSINCOS_U3_LO),
    packFloat128(FSINCOS_U4_HI, FSINCOS_U4_LO)
};
static const float128_t sc_V[4] = {
    packFloat128(FSINCOS_V1_HI, FSINCOS_V1_LO),
    packFloat128(FSINCOS_V2_HI, FSINCOS_V2_LO),
    packFloat128(FSINCOS_V3_HI, FSINCOS_V3_LO),
    packFloat128(FSINCOS_V4_HI, FSINCOS_V4_LO)
};

static float128_t sc_sin_c(int i) { return packFloat128(fsincos_sin_c_hi[i], fsincos_sin_c_lo[i]); }
static float128_t sc_cos_c(int i) { return packFloat128(fsincos_cos_c_hi[i], fsincos_cos_c_lo[i]); }

static BX_CPP_INLINE float128_t f128_negate(float128_t v)
{
    v.v64 ^= SC_SIGNBIT;
    return v;
}
static BX_CPP_INLINE float128_t f128_setsign(float128_t v, int neg)
{
    v.v64 = (v.v64 & ~SC_SIGNBIT) | (neg ? SC_SIGNBIT : 0);
    return v;
}
static BX_CPP_INLINE float128_t f128_xorsign(float128_t v, int neg)
{
    if (neg) v.v64 ^= SC_SIGNBIT;
    return v;
}

/* -------------------------------------------------------------------------
 * Argument reduction:  |x| = n * (pi/2) + r,  |r| <= pi/4.
 *
 * pi/2 is SC_PI2_HI:SC_PI2_LO - a 128-bit fixed-point value in [1,2) carrying
 * 66 significant bits (this narrow constant is why x87 sin/cos loses accuracy
 * for large arguments; matching it exactly is required for bit accuracy).
 * Throughout this routine a 128-bit quantity 'hi:lo' denotes  (hi:lo) / 2^127.
 *
 * Three cases by the exponent of |x|:
 *   |x| < 1/2   : |x| is already below pi/4 - r = x, n = 0, no fold.
 *   |x| in [1/2,2) : at most one pi/2 fits - subtract it once if  x >= pi/2.
 *   |x| >= 2    : full remainder.  A restoring bit-serial division of
 *                 (aSig << expDiff) << 64  by pi/2 yields the quotient bits
 *                 (only the low ones matter) and the 128-bit remainder.
 *
 * Then the fold: the raw remainder is in [0, pi/2); if it exceeds pi/4 replace
 * it with pi/2 - remainder (now in (0, pi/4]), remember that r is negative,
 * and bump n.
 *
 * Returns n & 3.  Sets 'r' to |r| (>= 0, as float128_t) and 'rNeg' to the sign.
 * ---------------------------------------------------------------------- */
static int sincos_reduce(Bit32s aExp, Bit64u aSig, float128_t &r, int &rNeg, softfloat_status_t &status)
{
    Bit32s expDiff = aExp - FLOATX80_EXP_BIAS;
    Bit64u rSig0, rSig1;
    Bit64u q = 0;

    if (expDiff < -1) {
        /* |x| < 1/2 < pi/4 : no reduction and no fold */
        r = softfloat_normRoundPackToF128(0, aExp - 0x10, aSig, 0, &status);
        rNeg = 0;
        return 0;
    }

    if (expDiff <= 0) {
        /* |x| in [1/2, 2) : at most one pi/2 to subtract */
        Bit64u xHi, xLo;
        shortShift128Right(aSig, 0, (expDiff == 0) ? 0 : 1, &xHi, &xLo);   /* x on the /2^127 scale */
        rSig0 = xHi; rSig1 = xLo;
        if (! softfloat_lt128(rSig0, rSig1, SC_PI2_HI, SC_PI2_LO)) {
            sub128(rSig0, rSig1, SC_PI2_HI, SC_PI2_LO, &rSig0, &rSig1);
            q = 1;
        }
    }
    else {
        /* |x| >= 2 : bit-serial truncated remainder.  Dividend is
           (aSig << expDiff) << 64  (so that quotient = x / (pi/2) on the
           /2^127 scale), divisor is pi/2. */
        Bit64u y1, y0;
        shortShift128Left(0, aSig, expDiff, &y1, &y0);   /* (y1:y0) = aSig << expDiff */
        rSig0 = 0; rSig1 = 0;
        for (int idx = 0; idx < 192; idx++) {
            Bit64u dbit =
                (idx < 64)  ? ((y1 >> (63 - idx)) & 1) :
                (idx < 128) ? ((y0 >> (127 - idx)) & 1) : 0;
            Bit64u carry = rSig0 >> 63;
            rSig0 = (rSig0 << 1) | (rSig1 >> 63);
            rSig1 = (rSig1 << 1) | dbit;
            q <<= 1;
            if (carry || ! softfloat_lt128(rSig0, rSig1, SC_PI2_HI, SC_PI2_LO)) {
                sub128(rSig0, rSig1, SC_PI2_HI, SC_PI2_LO, &rSig0, &rSig1);
                q |= 1;
            }
        }
    }

    /* fold to |r| <= pi/4 : if r > pi/4 then r := (pi/2) - r, rNeg, n++ */
    Bit64u p4Sig0, p4Sig1;
    shortShift128Right(SC_PI2_HI, SC_PI2_LO, 1, &p4Sig0, &p4Sig1);

    rNeg = 0;
    if (softfloat_lt128(p4Sig0, p4Sig1, rSig0, rSig1)) {
        sub128(SC_PI2_HI, SC_PI2_LO, rSig0, rSig1, &rSig0, &rSig1);
        rNeg = 1;
        q++;
    }

    r = softfloat_normRoundPackToF128(0, FLOATX80_EXP_BIAS - 0x10, rSig0, rSig1, &status);
    return (int)(q & 3);
}

/* -------------------------------------------------------------------------
 * get_c :  c = 2^k * (1.b1 b2 1),  from s = |r| in [1/4, pi/4].
 * index  = (lsb(exp) << 2) | (b1 << 1) | b2   in 0..7
 * ---------------------------------------------------------------------- */
static void sincos_getc(float128_t s, float128_t &c, int &index)
{
    Bit32s exp = (s.v64 >> 48) & 0x7FFF;
    Bit64u b1 = (s.v64 >> 47) & 1;
    Bit64u b2 = (s.v64 >> 46) & 1;

    index = (int)(((exp & 1) << 2) | (b1 << 1) | b2);

    /* keep the integer bit + b1 + b2, force b3 = 1, zero the rest */
    c.v64 = (s.v64 & ~((BX_CONST64(1) << 45) - 1)) | (BX_CONST64(1) << 45);
    c.v0  = 0;
}

/* p ~ sin(d), q ~ cos(d)-1, for the table path (|d| small) */
static void sincos_sincosd(float128_t d, float128_t &p, float128_t &q, softfloat_status_t &status)
{
    float128_t e = f128_mul_67_chop(d, d, &status);

    float128_t p1 = sc_U[3];
    p1 = f128_add_64_ne(f128_mul_67_chop(e, p1, &status), sc_U[2], &status);
    p1 = f128_add_64_ne(f128_mul_67_chop(e, p1, &status), sc_U[1], &status);
    p1 = f128_add_64_ne(f128_mul_67_chop(e, p1, &status), sc_U[0], &status);
    p1 = f128_mul_67_chop(e, p1, &status);
    p  = f128_add_64_ne(d, f128_mul_67_chop(p1, d, &status), &status);

    float128_t q1 = sc_V[3];
    q1 = f128_add_64_ne(f128_mul_67_chop(e, q1, &status), sc_V[2], &status);
    q1 = f128_add_64_ne(f128_mul_67_chop(e, q1, &status), sc_V[1], &status);
    q1 = f128_add_64_ne(f128_mul_67_chop(e, q1, &status), sc_V[0], &status);
    q  = f128_mul_64_ne(e, q1, &status);
}

/* Round  a + b  to a 64-bit (extF80) significand honouring the FPU mode; this
   is the closing step of every sin/cos reconstruction and is the only rounding
   that reaches SW.C1 (magnitude rounded up), so the round-up state left by the
   intermediate steps is cleared first.
   Corner case: when |b| is more than ~112 bits below |a|, f128_add absorbs it
   entirely (sum == a) and roundPackToF128 never flags a round-up - but the tiny
   addend still decided the direction, so it is applied by hand here: opposite
   signs mean |a+b| < |a| rounded up to |a|.  (This is what makes sin(tiny x)
   report C1 the way the hardware does.) */
static floatx80 sc_add_e64(float128_t a, float128_t b, softfloat_status_t &status)
{
    softfloat_clearRoundingUp(&status);

    float128_t sum = f128_add(a, b, &status);

    if (sum.v64 == a.v64 && sum.v0 == a.v0 && ((b.v64 << 1) | b.v0) != 0) {
        /* a + b == a to 113 bits : |b| < 1/2 ulp, result is 'a' */
        softfloat_raiseFlags(&status, softfloat_flag_inexact);
        floatx80 res = f128_to_extF80(a, &status);
        if ((a.v64 ^ b.v64) & SC_SIGNBIT)          /* opposite signs -> |a+b| < |a|, rounded up */
            softfloat_setRoundingUp(&status);
        return res;
    }
    return f128_to_extF80(sum, &status);
}

static floatx80 sincos_finish(float128_t head, float128_t f, float128_t g, softfloat_status_t &status)
{
    float128_t fg = f128_add_67_chop(f, g, &status);
    return sc_add_e64(head, fg, status);
}

// =========================================================================
// FSIN / FCOS core.  Delivers the single result of a standalone FSIN or FCOS.
//   n1 = n mod 4 from the reduction ;  rNeg = sign of r ;
//   n2 = 0 for FSIN, 1 for FCOS  (FCOS is sin shifted by one quadrant) ;
//   k1 = sign(x)  (0 for FCOS, which reduces |x|).
//
// n mod 4 selects the quadrant:  L = n&1 picks sin(r) vs cos(r) ; k2 = (n>>1)&1
// and the argument sign combine into the overall result sign.
//   L = 0 :  result = (-1)^(k1^k2^rNeg) * sin(|r|)
//   L = 1 :  result = (-1)^(k1^k2)      * cos(|r|)
// See the file header for why this is separate from sc_fsincos().
// =========================================================================
static floatx80 sc_fsin_fcos(float128_t rabs, int rNeg, int n1, int n2, int k1, softfloat_status_t &status)
{
    int n  = (n1 + n2) & 3;
    int L  = n & 1;
    int k2 = (n >> 1) & 1;
    int s1 = k1 ^ k2;
    int s2 = rNeg;

    Bit32s rexp = (rabs.v64 >> 48) & 0x7FFF;

    if (rexp >= FLOATX80_EXP_BIAS - 2) {
        /* |r| >= 1/4 : table-driven.  sin(|r|)/cos(|r|) = sin(c+d)/cos(c+d),
           combined from the sin(c),cos(c) table and the sin(d),cos(d) polys;
           the result sign is folded into sigma/gamma, and the head + (f+g)
           add is the single destination rounding. */
        float128_t c; int index;
        sincos_getc(rabs, c, index);
        float128_t sigma = sc_sin_c(index);
        float128_t gamma = sc_cos_c(index);

        float128_t d = f128_add_67_chop(rabs, f128_negate(c), &status);   /* d = s' - c */
        float128_t p, q;
        sincos_sincosd(d, p, q, status);

        if (L == 0) {
            int s3 = s1 ^ s2;
            sigma = f128_setsign(sigma, s3);
            gamma = f128_setsign(gamma, s3);
            float128_t f = f128_mul_67_chop(sigma, q, &status);
            float128_t g = f128_mul_67_chop(gamma, p, &status);
            return sincos_finish(sigma, f, g, status);
        }
        else {
            sigma = f128_setsign(sigma, s1);
            gamma = f128_setsign(gamma, s1);
            p = f128_negate(p);
            float128_t f = f128_mul_67_chop(sigma, p, &status);
            float128_t g = f128_mul_67_chop(gamma, q, &status);
            return sincos_finish(gamma, f, g, status);
        }
    }

    /* |r| < 1/4 : direct polynomial.  Split odd/even powers into p and q over
       t = s*s, s = r*r, then
           sin(|r|) ~ |r| + |r|*(p + q)        (p,q from A1..A6)
           cos(|r|) ~ 1   + (p + q)            (p,q from B1..B6)   */
    float128_t s = f128_mul_e67(rabs, rabs, &status);       /* s = r*r  */
    float128_t t = f128_mul_e67(s, s, &status);             /* t = s*s  */

    if (L == 0) {
        /* result = (-1)^(s1^s2) * sin(|r|),  the sign folded into r */
        float128_t rr = f128_setsign(rabs, s1 ^ s2);

        /* p = s*(A1 + t*(A3 + t*A5)) ; q = t*(A2 + t*(A4 + t*A6)) */
        float128_t p = f128_mul_67_chop(t, sc_A[4], &status);
        p = f128_add_64_ne(p, sc_A[2], &status);
        p = f128_mul_67_chop(t, p, &status);
        p = f128_add_64_ne(p, sc_A[0], &status);
        p = f128_mul_67_chop(s, p, &status);

        float128_t q = f128_mul_67_chop(t, sc_A[5], &status);
        q = f128_add_64_ne(q, sc_A[3], &status);
        q = f128_mul_67_chop(t, q, &status);
        q = f128_add_64_ne(q, sc_A[1], &status);
        q = f128_mul_67_chop(t, q, &status);

        float128_t f = f128_add_64_ne(p, q, &status);
        float128_t rf = f128_mul_67_chop(rr, f, &status);
        return sc_add_e64(rr, rf, status);
    }
    else {
        /* result = (-1)^s1 * cos(|r|),  cos being even */
        /* p = s*(B1 + t*(B3 + t*B5)) ; q = t*(B2 + t*(B4 + t*B6)) */
        float128_t p = f128_mul_67_chop(t, sc_B[4], &status);
        p = f128_add_64_ne(p, sc_B[2], &status);
        p = f128_mul_67_chop(t, p, &status);
        p = f128_add_64_ne(p, sc_B[0], &status);
        p = f128_mul_67_chop(s, p, &status);

        float128_t q = f128_mul_67_chop(t, sc_B[5], &status);
        q = f128_add_64_ne(q, sc_B[3], &status);
        q = f128_mul_67_chop(t, q, &status);
        q = f128_add_64_ne(q, sc_B[1], &status);
        q = f128_mul_67_chop(t, q, &status);

        float128_t f  = f128_add_67_chop(p, q, &status);
        float128_t one = f128_setsign(f128_one, s1);
        return sc_add_e64(one, f128_xorsign(f, s1), status);   /* +-(1 + (p+q)) */
    }
}

// =========================================================================
// FSINCOS core : compute  sin(x)  AND  cos(x)  in one call, by the head+tail
// method (see the file header: this is a DIFFERENT computation from sc_fsin_fcos,
// and matches only the FSINCOS instruction, not standalone FSIN/FCOS).
//   n1 = n mod 4 ;  rNeg = sign of r ;  k1 = sign(x).
//
// Both sin(r) and cos(r) are formed as a head (the dominant term) plus a tail
// (the correction), kept separate until the final per-result rounding.  n even
// vs odd swaps which of {sin,cos}(r) feeds the sin(x) result and which feeds
// cos(x); the quadrant/argument signs give sgn_sin, sgn_cos.
// =========================================================================
static void sc_fsincos(float128_t rabs, int rNeg, int n1, int k1,
                       floatx80 &sinx, floatx80 &cosx, softfloat_status_t &status)
{
    int k2 = n1 & 1;
    int k3 = (n1 >> 1) & 1;
    int sgn_sin = k1 ^ k3;
    int sgn_cos = k2 ^ k3;

    float128_t sin_head, sin_tail, cos_head, cos_tail;

    Bit32s rexp = (rabs.v64 >> 48) & 0x7FFF;

    if (rexp >= FLOATX80_EXP_BIAS - 2) {
        /* |r| >= 1/4 : table-driven.
           sin(r): head = sigma,  tail = gamma*p + sigma*q   (then sign of r)
           cos(r): head = gamma,  tail = gamma*q - sigma*p                    */
        float128_t c; int index;
        sincos_getc(rabs, c, index);
        float128_t sigma = sc_sin_c(index);
        float128_t gamma = sc_cos_c(index);

        float128_t d = f128_add_67_chop(rabs, f128_negate(c), &status);
        float128_t p, q;
        sincos_sincosd(d, p, q, status);

        float128_t gp = f128_mul_67_chop(gamma, p, &status);
        float128_t sq = f128_mul_67_chop(sigma, q, &status);
        sin_tail = f128_add_67_chop(gp, sq, &status);
        if (rNeg) sin_tail = f128_negate(sin_tail);

        float128_t sp = f128_mul_67_chop(sigma, p, &status);
        float128_t gq = f128_mul_67_chop(gamma, q, &status);
        cos_tail = f128_add_67_chop(f128_negate(sp), gq, &status);

        sin_head = rNeg ? f128_negate(sigma) : sigma;
        cos_head = gamma;
    }
    else {
        /* |r| < 1/4 : direct polynomial (single Horner in s = r*r).
           sin(r): head = r,   tail = r * s*(A1 + s*(A2 + ... + s*A6))
           cos(r): head = 1,   tail =     s*(B1 + s*(B2 + ... + s*B6))        */
        float128_t rsigned = rNeg ? f128_negate(rabs) : rabs;
        sin_head = rsigned;
        cos_head = f128_one;

        float128_t s = f128_mul_e67(rabs, rabs, &status);   /* s = r*r */

        float128_t p = sc_A[5];
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[4], &status);
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[3], &status);
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[2], &status);
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[1], &status);
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[0], &status);
        p = f128_mul_64_ne(s, p, &status);
        sin_tail = f128_mul_67_chop(rsigned, p, &status);

        float128_t qq = sc_B[5];
        qq = f128_add_64_ne(f128_mul_67_chop(s, qq, &status), sc_B[4], &status);
        qq = f128_add_64_ne(f128_mul_67_chop(s, qq, &status), sc_B[3], &status);
        qq = f128_add_64_ne(f128_mul_67_chop(s, qq, &status), sc_B[2], &status);
        qq = f128_add_64_ne(f128_mul_67_chop(s, qq, &status), sc_B[1], &status);
        qq = f128_add_64_ne(f128_mul_67_chop(s, qq, &status), sc_B[0], &status);
        cos_tail = f128_mul_67_chop(s, qq, &status);
    }

    /* reconstruction: n even -> sin(x) from sin(r), cos(x) from cos(r);
       n odd -> they swap.  Each result is  +-head +- tail  rounded once. */
    float128_t s_head, s_tail, c_head, c_tail;
    if (k2 == 0) {
        s_head = sin_head; s_tail = sin_tail;
        c_head = cos_head; c_tail = cos_tail;
    }
    else {
        s_head = cos_head; s_tail = cos_tail;
        c_head = sin_head; c_tail = sin_tail;
    }

    sinx = sc_add_e64(f128_xorsign(s_head, sgn_sin), f128_xorsign(s_tail, sgn_sin), status);
    cosx = sc_add_e64(f128_xorsign(c_head, sgn_cos), f128_xorsign(c_tail, sgn_cos), status);
}

// =================================================
// FSINCOS / FSIN / FCOS
// =================================================

BX_CPP_INLINE void sincos_invalid(floatx80 *sin_a, floatx80 *cos_a, floatx80 a)
{
    if (sin_a) *sin_a = a;
    if (cos_a) *cos_a = a;
}

BX_CPP_INLINE void sincos_tiny_argument(floatx80 *sin_a, floatx80 *cos_a, floatx80 a)
{
    if (sin_a) *sin_a = a;
    if (cos_a) *cos_a = floatx80_one;
}

int fsincos(floatx80 a, floatx80 *sin_a, floatx80 *cos_a, softfloat_status_t &status)
{
    Bit64u aSig;
    Bit32s aExp;
    int aSign;

    // handle unsupported extended double-precision floating encodings
    if (extF80_isUnsupported(a)) {
        goto invalid;
    }

    aSig = extF80_fraction(a);
    aExp = extF80_exp(a);
    aSign = extF80_sign(a);

    if (aExp == 0x7FFF) {
        if (aSig << 1) {
            sincos_invalid(sin_a, cos_a, softfloat_propagateNaNExtF80UI(a.signExp, aSig, 0, 0, &status));
            return 0;
        }
    invalid:
        softfloat_raiseFlags(&status, softfloat_flag_invalid);
        sincos_invalid(sin_a, cos_a, floatx80_default_nan);
        return 0;
    }

    if (! aExp) {
        if (! aSig) {
            sincos_tiny_argument(sin_a, cos_a, a);
            return 0;
        }

        softfloat_raiseFlags(&status, softfloat_flag_denormal);

        struct exp32_sig64 normExpSig = softfloat_normSubnormalExtF80Sig(aSig);
        aExp = normExpSig.exp + 1;
        aSig = normExpSig.sig;

        /* true denormal (|x| < 2^-16382): sin(x) = x to well beyond working
           precision, but the result is subnormal, so deliver it through the
           standard round/pack - a masked underflow flushes it gradually, an
           unmasked one applies the 0x6000 exponent bias.  cos(x) = 1. */
        if (aExp <= 0) {
            softfloat_raiseFlags(&status, softfloat_flag_inexact);
            if (cos_a)
                *cos_a = floatx80_one;
            if (sin_a) {
                softfloat_raiseFlags(&status, softfloat_flag_underflow);
                *sin_a = softfloat_roundPackToExtF80(aSign, aExp, aSig, 0, 80, &status);
            }
            return 0;
        }
    }

    Bit32s expDiff = aExp - FLOATX80_EXP_BIAS;

    /* argument too large for reduction */
    if (expDiff >= 63)
        return -1;

    softfloat_raiseFlags(&status, softfloat_flag_inexact);

    if (expDiff < -68) {       /* |x| < 2^-68 : sin(x) = x, cos(x) = 1 */
        floatx80 xr = packFloatx80(aSign, aExp, aSig);
        sincos_tiny_argument(sin_a, cos_a, xr);
        return 0;
    }

    int rNeg;
    float128_t r;
    int n1 = sincos_reduce(aExp, aSig, r, rNeg, status);

    /* FSINCOS and standalone FSIN/FCOS use different internal routines on
       real hardware (see the file header) - dispatch accordingly. */
    if (sin_a && cos_a) {
        sc_fsincos(r, rNeg, n1, aSign, *sin_a, *cos_a, status);             /* FSINCOS */
    }
    else if (sin_a) {
        *sin_a = sc_fsin_fcos(r, rNeg, n1, /*n2*/ 0, /*k1*/ aSign, status); /* FSIN  */
    }
    else {
        *cos_a = sc_fsin_fcos(r, rNeg, n1, /*n2*/ 1, /*k1*/ 0, status);     /* FCOS  */
    }

    return 0;
}

int fsin(floatx80 &a, softfloat_status_t &status)
{
    return fsincos(a, &a, NULL, status);
}

int fcos(floatx80 &a, softfloat_status_t &status)
{
    return fsincos(a, NULL, &a, status);
}

// =========================================================================
// FPTAN                 Compute tan(x), then push 1.0 (the push is done by the
//                       instruction handler, not here).
//
// tan(x) = sin(x)/cos(x).  Reduce as for FSIN, form  f = sin(r), g = cos(r),
// apply sign(x) to f, and pick by n mod 2:
//     n even :  tan(x) =  f / g
//     n odd  :  tan(x) = -g / f     (tan is pi/2-periodic up to sign/reciprocal)
// The closing divide is the destination rounding and the only C1-setting step.
// f/g here are yet a third internal variant (single folded 67-bit values), so
// FPTAN gets its own sin/cos helper rather than reusing sc_fsin_fcos / sc_fsincos.
// =========================================================================

/* sin(r) and cos(r) as single (67-bit) float128 values.  'rabs' = |r| (the
   reduced argument, |r| <= pi/4), 'rNeg' set when r came out negative. */
static void fptan_sin_cos(float128_t rabs, int rNeg, float128_t &f, float128_t &g, softfloat_status_t &status)
{
    Bit32s rexp = (rabs.v64 >> 48) & 0x7FFF;

    if (rexp >= FLOATX80_EXP_BIAS - 2) {
        /* |r| >= 1/4 : table-driven, f = sin(c+d), g = cos(c+d) */
        float128_t c; int index;
        sincos_getc(rabs, c, index);
        float128_t sigma = sc_sin_c(index);
        float128_t gamma = sc_cos_c(index);

        float128_t d = f128_add_67_chop(rabs, f128_negate(c), &status);   /* d = |r| - c */
        float128_t p, q;
        sincos_sincosd(d, p, q, status);                                  /* p ~ sin(d), q ~ cos(d)-1 */

        /* f = sin(c+d) = sigma*(1+q) + gamma*p */
        f = f128_mul_67_chop(gamma, p, &status);
        f = f128_add_67_chop(f, f128_mul_67_chop(sigma, q, &status), &status);
        f = f128_add_67_chop(sigma, f, &status);
        if (rNeg) f = f128_negate(f);

        /* g = cos(c+d) = gamma*(1+q) - sigma*p */
        g = f128_negate(f128_mul_67_chop(sigma, p, &status));
        g = f128_add_67_chop(g, f128_mul_67_chop(gamma, q, &status), &status);
        g = f128_add_67_chop(gamma, g, &status);
    }
    else {
        /* |r| < 1/4 : direct polynomial */
        float128_t rsigned = rNeg ? f128_negate(rabs) : rabs;
        float128_t s = f128_mul_e67(rabs, rabs, &status);                 /* s = r*r */

        float128_t p = sc_A[5];
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[4], &status);
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[3], &status);
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[2], &status);
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[1], &status);
        p = f128_add_64_ne(f128_mul_67_chop(s, p, &status), sc_A[0], &status);
        p = f128_mul_64_ne(s, p, &status);
        f = f128_add_67_chop(rsigned, f128_mul_67_chop(rsigned, p, &status), &status);   /* r + r*p */

        float128_t q = sc_B[5];
        q = f128_add_64_ne(f128_mul_67_chop(s, q, &status), sc_B[4], &status);
        q = f128_add_64_ne(f128_mul_67_chop(s, q, &status), sc_B[3], &status);
        q = f128_add_64_ne(f128_mul_67_chop(s, q, &status), sc_B[2], &status);
        q = f128_add_64_ne(f128_mul_67_chop(s, q, &status), sc_B[1], &status);
        q = f128_add_64_ne(f128_mul_67_chop(s, q, &status), sc_B[0], &status);
        q = f128_mul_67_chop(s, q, &status);
        g = f128_add_67_chop(f128_one, q, &status);                       /* 1 + q */
    }
}

int fptan(floatx80 &a, softfloat_status_t &status)
{
    Bit64u aSig;
    Bit32s aExp;
    int aSign;

    // handle unsupported extended double-precision floating encodings
    if (extF80_isUnsupported(a)) {
        softfloat_raiseFlags(&status, softfloat_flag_invalid);
        a = floatx80_default_nan;
        return 0;
    }

    aSig = extF80_fraction(a);
    aExp = extF80_exp(a);
    aSign = extF80_sign(a);

    if (aExp == 0x7FFF) {
        if (aSig << 1) {
            a = softfloat_propagateNaNExtF80UI(a.signExp, aSig, 0, 0, &status);
            return 0;
        }
        softfloat_raiseFlags(&status, softfloat_flag_invalid);
        a = floatx80_default_nan;
        return 0;
    }

    if (! aExp) {
        if (! aSig) return 0;                       /* tan(+-0) = +-0 */
        softfloat_raiseFlags(&status, softfloat_flag_denormal);

        struct exp32_sig64 normExpSig = softfloat_normSubnormalExtF80Sig(aSig);
        aExp = normExpSig.exp + 1;
        aSig = normExpSig.sig;

        /* true denormal (|x| < 2^-16382): tan(x) = x, result subnormal -> deliver
           through the standard round/pack (masked = gradual, unmasked = 0x6000
           exponent bias). */
        if (aExp <= 0) {
            softfloat_raiseFlags(&status, softfloat_flag_inexact | softfloat_flag_underflow);
            a = softfloat_roundPackToExtF80(aSign, aExp, aSig, 0, 80, &status);
            return 0;
        }
    }

    Bit32s expDiff = aExp - FLOATX80_EXP_BIAS;

    /* argument too large for reduction */
    if (expDiff >= 63)
        return -1;

    softfloat_raiseFlags(&status, softfloat_flag_inexact);

    if (expDiff < -68) {                        /* |x| < 2^-68 : tan(x) = x */
        a = packFloatx80(aSign, aExp, aSig);
        return 0;
    }

    int rNeg;
    float128_t r;
    int n = sincos_reduce(aExp, aSig, r, rNeg, status);

    float128_t f, g;
    fptan_sin_cos(r, rNeg, f, g, status);

    if (aSign) f = f128_negate(f);              /* f := (-1)^sign(x) * sin(r) */

    float128_t num, den;
    if (n & 1) {                                /* n odd  : tan(x) = -cos(r) / f */
        num = f128_negate(g);
        den = f;
    }
    else {                                      /* n even : tan(x) =  sin(r) / cos(r) */
        num = f;
        den = g;
    }

    softfloat_clearRoundingUp(&status);
    float128_t z = f128_div(num, den, softfloat_getRoundingMode(&status), &status);
    a = f128_to_extF80(z, &status);

    return 0;
}
