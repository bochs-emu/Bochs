/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2026 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
//
//   Co-Authored-By: Claude Opus 5.5
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

#include "wide_int.h"
#include "scalar_arith.h"

#if BX_HAVE_INT128 == 0

// divide 128-bit value u1:u0 by 64-bit value v, requires u1 < v so the quotient fits into 64-bit
// Knuth algorithm D with 32-bit digits (Hacker's Delight, divlu)
static Bit64u udiv128by64(Bit64u u1, Bit64u u0, Bit64u v, Bit64u *remainder)
{
  const Bit64u b = BX_CONST64(1) << 32;

  // normalize the divisor so its MSB is set
  unsigned s = lzcntq(v);
  v <<= s;
  Bit64u vn1 = v >> 32, vn0 = v & 0xffffffff;

  Bit64u un32 = s ? ((u1 << s) | (u0 >> (64 - s))) : u1;
  Bit64u un10 = u0 << s;
  Bit64u un1 = un10 >> 32, un0 = un10 & 0xffffffff;

  // compute the first quotient digit
  Bit64u q1 = un32 / vn1;
  Bit64u rhat = un32 - q1 * vn1;
  while (q1 >= b || q1 * vn0 > b * rhat + un1) {
    q1--;
    rhat += vn1;
    if (rhat >= b) break;
  }

  Bit64u un21 = un32 * b + un1 - q1 * v;

  // compute the second quotient digit
  Bit64u q0 = un21 / vn1;
  rhat = un21 - q0 * vn1;
  while (q0 >= b || q0 * vn0 > b * rhat + un0) {
    q0--;
    rhat += vn1;
    if (rhat >= b) break;
  }

  *remainder = (un21 * b + un0 - q0 * v) >> s;
  return q1 * b + q0;
}

void bx_divmod128u(Bit128u dividend, Bit128u divisor, Bit128u *quotient, Bit128u *remainder)
{
  Bit64u n_hi = dividend.hi(), n_lo = dividend.lo();
  Bit64u d_hi = divisor.hi(),  d_lo = divisor.lo();

  if (d_hi == 0) {
    if (n_hi == 0) {
      // 64-bit by 64-bit division
      *quotient  = n_lo / d_lo;
      *remainder = n_lo % d_lo;
      return;
    }

    // 128-bit by 64-bit division
    Bit64u q_hi = 0, r;
    if (n_hi >= d_lo) {
      q_hi = n_hi / d_lo;
      n_hi = n_hi % d_lo;
    }
    Bit64u q_lo = udiv128by64(n_hi, n_lo, d_lo, &r);
    *quotient  = MAKE128U(q_hi, q_lo);
    *remainder = r;
    return;
  }

  // 128-bit by 128-bit division, the divisor >= 2^64 so the quotient fits into 64-bit
  // (Hacker's Delight, divlu128)
  if (dividend < divisor) {
    *quotient  = 0;
    *remainder = dividend;
    return;
  }

  unsigned s = lzcntq(d_hi);
  Bit64u v1 = (divisor << s).hi(); // normalized divisor high bits
  Bit128u u1 = dividend >> 1;        // ensure no overflow in udiv128by64

  Bit64u r;
  Bit64u q = udiv128by64(u1.hi(), u1.lo(), v1, &r) >> (63 - s);

  // q is the quotient or the quotient + 1
  if (q != 0) q--;
  Bit128u rem = dividend - Bit128u(q) * divisor;
  if (rem >= divisor) {
    q++;
    rem -= divisor;
  }

  *quotient  = q;
  *remainder = rem;
}

// signed division truncates toward zero, the remainder has the sign of the dividend
void bx_divmod128s(Bit128s dividend, Bit128s divisor, Bit128s *quotient, Bit128s *remainder)
{
  bool dividend_neg = (dividend < 0), divisor_neg = (divisor < 0);

  Bit128u n = dividend_neg ? -Bit128u(dividend) : Bit128u(dividend);
  Bit128u d = divisor_neg  ? -Bit128u(divisor)  : Bit128u(divisor);

  Bit128u q, r;
  bx_divmod128u(n, d, &q, &r);

  *quotient  = Bit128s((dividend_neg != divisor_neg) ? -q : q);
  *remainder = Bit128s(dividend_neg ? -r : r);
}

#endif
