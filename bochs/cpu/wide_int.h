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

#ifndef BX_WIDE_INT_H
#define BX_WIDE_INT_H

#include "config.h"

//
// 128-bit integer types Bit128u and Bit128s.
//
// When the compiler supports native unsigned __int128 / __int128 the types are
// simple typedefs, otherwise they are implemented by the classes below which
// mimic the native types behavior (interface similar to absl::uint128).
//
// The code using Bit128u / Bit128s must compile with both implementations:
//   - use regular C operators: + - * / % & | ^ ~ << >> == != < <= > >= and
//     compound assignments += -= ... (no increment / decrement operators)
//   - convert from any builtin integer type, either implicitly or by cast
//   - convert to builtin integer type only by explicit cast, e.g. (Bit64u) x
//     (truncates to the low bits)
//   - shift count must be in range 0..127
//   - access the 64-bit halves only through GET128L() / GET128H()
//   - build 128-bit value from 64-bit halves only through MAKE128U() / MAKE128S()
//

#ifndef BX_HAVE_INT128
#define BX_HAVE_INT128 0
#endif

#if BX_HAVE_INT128

typedef unsigned __int128 Bit128u;
typedef   signed __int128 Bit128s;

BX_CPP_INLINE Bit64u GET128L(Bit128u x) { return (Bit64u) x; }
BX_CPP_INLINE Bit64u GET128H(Bit128u x) { return (Bit64u)(x >> 64); }
BX_CPP_INLINE Bit64u GET128L(Bit128s x) { return (Bit64u) x; }
BX_CPP_INLINE Bit64s GET128H(Bit128s x) { return (Bit64s)(x >> 64); }

BX_CPP_INLINE Bit128u MAKE128U(Bit64u hi, Bit64u lo) { return ((Bit128u) hi << 64) | lo; }
BX_CPP_INLINE Bit128s MAKE128S(Bit64s hi, Bit64u lo) { return (Bit128s) MAKE128U((Bit64u) hi, lo); }

#else // !BX_HAVE_INT128

#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#endif

// 64x64 -> 128 bit unsigned multiply, returns low 64 bits of the product
BX_CPP_INLINE Bit64u bx_mul64x64(Bit64u a, Bit64u b, Bit64u *hi)
{
#if defined(_MSC_VER) && defined(_M_X64)
  return _umul128(a, b, hi);
#else
  Bit64u a_lo = a & 0xffffffff, a_hi = a >> 32;
  Bit64u b_lo = b & 0xffffffff, b_hi = b >> 32;

  Bit64u p0 = a_lo * b_lo;
  Bit64u p1 = a_lo * b_hi;
  Bit64u p2 = a_hi * b_lo;
  Bit64u p3 = a_hi * b_hi;

  Bit64u mid = (p0 >> 32) + (p1 & 0xffffffff) + (p2 & 0xffffffff);
  *hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
  return (mid << 32) | (p0 & 0xffffffff);
#endif
}

class Bit128u;
class Bit128s;

// division helpers implemented in wide_int.cc, for internal use only
extern void bx_divmod128u(Bit128u dividend, Bit128u divisor, Bit128u *quotient, Bit128u *remainder);
extern void bx_divmod128s(Bit128s dividend, Bit128s divisor, Bit128s *quotient, Bit128s *remainder);

class Bit128s {
public:
  Bit128s() {} // uninitialized, same as native integer type

  // implicit conversions from builtin integer types, signed values are sign-extended
  Bit128s(int v)                { m_lo = (Bit64u) v; m_hi = (v < 0) ? -1 : 0; }
  Bit128s(unsigned v)           { m_lo = v; m_hi = 0; }
  Bit128s(long v)               { m_lo = (Bit64u) v; m_hi = (v < 0) ? -1 : 0; }
  Bit128s(unsigned long v)      { m_lo = v; m_hi = 0; }
  Bit128s(long long v)          { m_lo = (Bit64u) v; m_hi = (v < 0) ? -1 : 0; }
  Bit128s(unsigned long long v) { m_lo = v; m_hi = 0; }

  // explicit conversions to builtin integer types, truncate to low bits
  explicit operator bool() const               { return (m_lo | (Bit64u) m_hi) != 0; }
  explicit operator char() const               { return (char) m_lo; }
  explicit operator signed char() const        { return (signed char) m_lo; }
  explicit operator unsigned char() const      { return (unsigned char) m_lo; }
  explicit operator short() const              { return (short) m_lo; }
  explicit operator unsigned short() const     { return (unsigned short) m_lo; }
  explicit operator int() const                { return (int) m_lo; }
  explicit operator unsigned() const           { return (unsigned) m_lo; }
  explicit operator long() const               { return (long) m_lo; }
  explicit operator unsigned long() const      { return (unsigned long) m_lo; }
  explicit operator long long() const          { return (long long) m_lo; }
  explicit operator unsigned long long() const { return (unsigned long long) m_lo; }

  // the arithmetic is done in two's complement, the high half is computed as unsigned
  Bit128s operator~() const { return Bit128s(~m_hi, ~m_lo); }
  Bit128s operator-() const { return Bit128s((Bit64s)(~(Bit64u) m_hi + (m_lo == 0)), 0 - m_lo); }

  Bit128s& operator+=(Bit128s b) {
    Bit64u lo = m_lo + b.m_lo;
    m_hi = (Bit64s)((Bit64u) m_hi + (Bit64u) b.m_hi + (lo < m_lo));
    m_lo = lo;
    return *this;
  }

  Bit128s& operator-=(Bit128s b) {
    Bit64u lo = m_lo - b.m_lo;
    m_hi = (Bit64s)((Bit64u) m_hi - (Bit64u) b.m_hi - (m_lo < b.m_lo));
    m_lo = lo;
    return *this;
  }

  Bit128s& operator*=(Bit128s b) {
    Bit64u hi, lo = bx_mul64x64(m_lo, b.m_lo, &hi);
    m_hi = (Bit64s)(hi + (Bit64u) m_hi * b.m_lo + m_lo * (Bit64u) b.m_hi);
    m_lo = lo;
    return *this;
  }

  Bit128s& operator/=(Bit128s b) { Bit128s r; bx_divmod128s(*this, b, this, &r); return *this; }
  Bit128s& operator%=(Bit128s b) { Bit128s q; bx_divmod128s(*this, b, &q, this); return *this; }

  Bit128s& operator&=(Bit128s b) { m_lo &= b.m_lo; m_hi &= b.m_hi; return *this; }
  Bit128s& operator|=(Bit128s b) { m_lo |= b.m_lo; m_hi |= b.m_hi; return *this; }
  Bit128s& operator^=(Bit128s b) { m_lo ^= b.m_lo; m_hi ^= b.m_hi; return *this; }

  Bit128s& operator<<=(int amount) {
    if (amount >= 64) {
      m_hi = (Bit64s)(m_lo << (amount - 64));
      m_lo = 0;
    }
    else if (amount > 0) {
      m_hi = (Bit64s)(((Bit64u) m_hi << amount) | (m_lo >> (64 - amount)));
      m_lo <<= amount;
    }
    return *this;
  }

  // arithmetic shift right
  Bit128s& operator>>=(int amount) {
    if (amount >= 64) {
      m_lo = (Bit64u)(m_hi >> (amount - 64));
      m_hi >>= 63;
    }
    else if (amount > 0) {
      m_lo = (m_lo >> amount) | ((Bit64u) m_hi << (64 - amount));
      m_hi >>= amount;
    }
    return *this;
  }

  // access to 64-bit halves, not available for native type, use GET128L() / GET128H() instead
  Bit64u lo() const { return m_lo; }
  Bit64s hi() const { return m_hi; }

  friend Bit128s MAKE128S(Bit64s hi, Bit64u lo);

private:
  // build from 64-bit halves, not available for native type, use MAKE128S() instead
  Bit128s(Bit64s h, Bit64u l) { m_lo = l; m_hi = h; }

#ifdef BX_LITTLE_ENDIAN
  Bit64u m_lo;
  Bit64s m_hi;
#else
  Bit64s m_hi;
  Bit64u m_lo;
#endif
};

BX_CPP_INLINE Bit64u GET128L(Bit128s x) { return x.lo(); }
BX_CPP_INLINE Bit64s GET128H(Bit128s x) { return x.hi(); }

BX_CPP_INLINE Bit128s MAKE128S(Bit64s hi, Bit64u lo) { return Bit128s(hi, lo); }

class Bit128u {
public:
  Bit128u() {} // uninitialized, same as native integer type

  // implicit conversions from builtin integer types, signed values are sign-extended
  Bit128u(int v)                { m_lo = (Bit64u) v; m_hi = (v < 0) ? ~BX_CONST64(0) : 0; }
  Bit128u(unsigned v)           { m_lo = v; m_hi = 0; }
  Bit128u(long v)               { m_lo = (Bit64u) v; m_hi = (v < 0) ? ~BX_CONST64(0) : 0; }
  Bit128u(unsigned long v)      { m_lo = v; m_hi = 0; }
  Bit128u(long long v)          { m_lo = (Bit64u) v; m_hi = (v < 0) ? ~BX_CONST64(0) : 0; }
  Bit128u(unsigned long long v) { m_lo = v; m_hi = 0; }

  // implicit conversion from Bit128s and explicit conversion to Bit128s, same as native types
  Bit128u(Bit128s v) { m_lo = v.lo(); m_hi = (Bit64u) v.hi(); }
  explicit operator Bit128s() const { return MAKE128S((Bit64s) m_hi, m_lo); }

  // explicit conversions to builtin integer types, truncate to low bits
  explicit operator bool() const               { return (m_lo | m_hi) != 0; }
  explicit operator char() const               { return (char) m_lo; }
  explicit operator signed char() const        { return (signed char) m_lo; }
  explicit operator unsigned char() const      { return (unsigned char) m_lo; }
  explicit operator short() const              { return (short) m_lo; }
  explicit operator unsigned short() const     { return (unsigned short) m_lo; }
  explicit operator int() const                { return (int) m_lo; }
  explicit operator unsigned() const           { return (unsigned) m_lo; }
  explicit operator long() const               { return (long) m_lo; }
  explicit operator unsigned long() const      { return (unsigned long) m_lo; }
  explicit operator long long() const          { return (long long) m_lo; }
  explicit operator unsigned long long() const { return (unsigned long long) m_lo; }

  Bit128u operator~() const { return Bit128u(~m_hi, ~m_lo); }
  Bit128u operator-() const { return Bit128u(~m_hi + (m_lo == 0), 0 - m_lo); }

  Bit128u& operator+=(Bit128u b) {
    Bit64u lo = m_lo + b.m_lo;
    m_hi += b.m_hi + (lo < m_lo);
    m_lo = lo;
    return *this;
  }

  Bit128u& operator-=(Bit128u b) {
    Bit64u lo = m_lo - b.m_lo;
    m_hi -= b.m_hi + (m_lo < b.m_lo);
    m_lo = lo;
    return *this;
  }

  Bit128u& operator*=(Bit128u b) {
    Bit64u hi, lo = bx_mul64x64(m_lo, b.m_lo, &hi);
    m_hi = hi + m_hi * b.m_lo + m_lo * b.m_hi;
    m_lo = lo;
    return *this;
  }

  Bit128u& operator/=(Bit128u b) { Bit128u r; bx_divmod128u(*this, b, this, &r); return *this; }
  Bit128u& operator%=(Bit128u b) { Bit128u q; bx_divmod128u(*this, b, &q, this); return *this; }

  Bit128u& operator&=(Bit128u b) { m_lo &= b.m_lo; m_hi &= b.m_hi; return *this; }
  Bit128u& operator|=(Bit128u b) { m_lo |= b.m_lo; m_hi |= b.m_hi; return *this; }
  Bit128u& operator^=(Bit128u b) { m_lo ^= b.m_lo; m_hi ^= b.m_hi; return *this; }

  Bit128u& operator<<=(int amount) {
    if (amount >= 64) {
      m_hi = m_lo << (amount - 64);
      m_lo = 0;
    }
    else if (amount > 0) {
      m_hi = (m_hi << amount) | (m_lo >> (64 - amount));
      m_lo <<= amount;
    }
    return *this;
  }

  Bit128u& operator>>=(int amount) {
    if (amount >= 64) {
      m_lo = m_hi >> (amount - 64);
      m_hi = 0;
    }
    else if (amount > 0) {
      m_lo = (m_lo >> amount) | (m_hi << (64 - amount));
      m_hi >>= amount;
    }
    return *this;
  }

  // access to 64-bit halves, not available for native type, use GET128L() / GET128H() instead
  Bit64u lo() const { return m_lo; }
  Bit64u hi() const { return m_hi; }

  friend Bit128u MAKE128U(Bit64u hi, Bit64u lo);

private:
  // build from 64-bit halves, not available for native type, use MAKE128U() instead
  Bit128u(Bit64u h, Bit64u l) { m_lo = l; m_hi = h; }

#ifdef BX_LITTLE_ENDIAN
  Bit64u m_lo;
  Bit64u m_hi;
#else
  Bit64u m_hi;
  Bit64u m_lo;
#endif
};

BX_CPP_INLINE Bit64u GET128L(Bit128u x) { return x.lo(); }
BX_CPP_INLINE Bit64u GET128H(Bit128u x) { return x.hi(); }

BX_CPP_INLINE Bit128u MAKE128U(Bit64u hi, Bit64u lo) { return Bit128u(hi, lo); }

// Bit128u binary operators

BX_CPP_INLINE Bit128u operator+(Bit128u a, Bit128u b) { return a += b; }
BX_CPP_INLINE Bit128u operator-(Bit128u a, Bit128u b) { return a -= b; }
BX_CPP_INLINE Bit128u operator*(Bit128u a, Bit128u b) { return a *= b; }
BX_CPP_INLINE Bit128u operator/(Bit128u a, Bit128u b) { return a /= b; }
BX_CPP_INLINE Bit128u operator%(Bit128u a, Bit128u b) { return a %= b; }
BX_CPP_INLINE Bit128u operator&(Bit128u a, Bit128u b) { return a &= b; }
BX_CPP_INLINE Bit128u operator|(Bit128u a, Bit128u b) { return a |= b; }
BX_CPP_INLINE Bit128u operator^(Bit128u a, Bit128u b) { return a ^= b; }

BX_CPP_INLINE Bit128u operator<<(Bit128u a, int amount) { return a <<= amount; }
BX_CPP_INLINE Bit128u operator>>(Bit128u a, int amount) { return a >>= amount; }

BX_CPP_INLINE bool operator==(Bit128u a, Bit128u b) { return ((a.lo() ^ b.lo()) | (a.hi() ^ b.hi())) == 0; }
BX_CPP_INLINE bool operator!=(Bit128u a, Bit128u b) { return !(a == b); }

BX_CPP_INLINE bool operator<(Bit128u a, Bit128u b)
{
  return (a.hi() == b.hi()) ? (a.lo() < b.lo()) : (a.hi() < b.hi());
}

BX_CPP_INLINE bool operator> (Bit128u a, Bit128u b) { return b < a; }
BX_CPP_INLINE bool operator<=(Bit128u a, Bit128u b) { return !(b < a); }
BX_CPP_INLINE bool operator>=(Bit128u a, Bit128u b) { return !(a < b); }

// Bit128s binary operators

BX_CPP_INLINE Bit128s operator+(Bit128s a, Bit128s b) { return a += b; }
BX_CPP_INLINE Bit128s operator-(Bit128s a, Bit128s b) { return a -= b; }
BX_CPP_INLINE Bit128s operator*(Bit128s a, Bit128s b) { return a *= b; }
BX_CPP_INLINE Bit128s operator/(Bit128s a, Bit128s b) { return a /= b; }
BX_CPP_INLINE Bit128s operator%(Bit128s a, Bit128s b) { return a %= b; }
BX_CPP_INLINE Bit128s operator&(Bit128s a, Bit128s b) { return a &= b; }
BX_CPP_INLINE Bit128s operator|(Bit128s a, Bit128s b) { return a |= b; }
BX_CPP_INLINE Bit128s operator^(Bit128s a, Bit128s b) { return a ^= b; }

BX_CPP_INLINE Bit128s operator<<(Bit128s a, int amount) { return a <<= amount; }
BX_CPP_INLINE Bit128s operator>>(Bit128s a, int amount) { return a >>= amount; }

BX_CPP_INLINE bool operator==(Bit128s a, Bit128s b) { return ((a.lo() ^ b.lo()) | (a.hi() ^ b.hi())) == 0; }
BX_CPP_INLINE bool operator!=(Bit128s a, Bit128s b) { return !(a == b); }

BX_CPP_INLINE bool operator<(Bit128s a, Bit128s b)
{
  return (a.hi() == b.hi()) ? (a.lo() < b.lo()) : (a.hi() < b.hi());
}

BX_CPP_INLINE bool operator> (Bit128s a, Bit128s b) { return b < a; }
BX_CPP_INLINE bool operator<=(Bit128s a, Bit128s b) { return !(b < a); }
BX_CPP_INLINE bool operator>=(Bit128s a, Bit128s b) { return !(a < b); }

#endif // BX_HAVE_INT128

#define BIT128U_MAX (MAKE128U(BX_CONST64(0xffffffffffffffff), BX_CONST64(0xffffffffffffffff)))
#define BIT128S_MAX (MAKE128S(BX_CONST64(0x7fffffffffffffff), BX_CONST64(0xffffffffffffffff)))
#define BIT128S_MIN (MAKE128S((Bit64s) BX_CONST64(0x8000000000000000), 0))

#endif
