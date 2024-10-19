/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2020 Stanislav Shwartsman
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

#include "wide_int.h"

void long_mul(Bit128u *product, Bit64u op1, Bit64u op2)
{
  Bit32u a32 = op1>>32;
  Bit32u a0 = op1;
  Bit32u b32 = op2>>32;
  Bit32u b0 = op2;

  product->lo = (Bit64u) a0 * b0;
  Bit64u mid1 = (Bit64u) a32 * b0;
  Bit64u mid = mid1 + (Bit64u) a0 * b32;
  product->hi = (Bit64u) a32 * b32;
  product->hi += (Bit64u) (mid < mid1)<<32 | mid>>32;
  mid <<= 32;
  product->lo += mid;
  product->hi += (product->lo < mid);
}

void long_neg(Bit128s *n)
{
  Bit64u t = n->lo;
  n->lo = - (Bit64s)(n->lo);
  if (t - 1 > t) --n->hi;
  n->hi = ~n->hi;
}

void long_imul(Bit128s *product, Bit64s op1, Bit64s op2)
{
  unsigned s1,s2;

  if ((s1 = (op1 < 0))) op1 = -op1;
  if ((s2 = (op2 < 0))) op2 = -op2;
  long_mul((Bit128u*)product,(Bit64u)op1,(Bit64u)op2);
  if (s1 ^ s2)
    long_neg(product);
}

void long_shl(Bit128u *a)
{
  Bit64u c = a->lo >> 63;
  a->lo <<= 1;
  a->hi <<= 1;
  a->hi |= c;
}

void long_shr(Bit128u *a)
{
  Bit64u c;
  c = a->hi << 63;
  a->hi >>= 1;
  a->lo >>= 1;
  a->lo |= c;
}

unsigned long_sub(Bit128u *a,Bit128u *b)
{
  Bit64u t = a->lo;
  a->lo -= b->lo;
  int c = (a->lo > t);
  t = a -> hi;
  a->hi -= b->hi + c;
  return(a->hi > t);
}

int long_le(Bit128u *a,Bit128u *b)
{
  if (a->hi == b->hi) {
    return(a->lo <= b->lo);
  } else {
    return(a->hi <= b->hi);
  }
}

void long_div(Bit128u *quotient,Bit64u *remainder,const Bit128u *dividend,Bit64u divisor)
{
  /*
  n := 0;
  while (divisor <= dividend) do
    inc(n);
    divisor := divisor * 2;
  end;
  quotient := 0;
  while n > 0 do
    divisor := divisor div 2;
    quotient := quotient * 2;
    temp := dividend;
    dividend := dividend - divisor;
    if temp > dividend then
      dividend := temp;
    else
      inc(quotient);
    end;
    dec(n);
  end;
  remainder := dividend;
  */

  Bit128u d,acc,q,temp;
  int n,c;

  d.lo = divisor;
  d.hi = 0;
  acc.lo = dividend->lo;
  acc.hi = dividend->hi;
  q.lo = 0;
  q.hi = 0;
  n = 0;

  while (long_le(&d,&acc) && n < 128) {
    long_shl(&d);
    n++;
  }

  while (n > 0) {
    long_shr(&d);
    long_shl(&q);
    temp.lo = acc.lo;
    temp.hi = acc.hi;
    c = long_sub(&acc,&d);
    if (c) {
      acc.lo = temp.lo;
      acc.hi = temp.hi;
    } else {
      q.lo++;
    }
    n--;
  }

  *remainder = acc.lo;
  quotient->lo  = q.lo;
  quotient->hi  = q.hi;
}

void long_idiv(Bit128s *quotient,Bit64s *remainder,Bit128s *dividend,Bit64s divisor)
{
  unsigned s1,s2;
  Bit128s temp;

  temp = *dividend;
  if ((s1 = (temp.hi < 0))) {
    long_neg(&temp);
  }
  if ((s2 = (divisor < 0))) divisor = -divisor;
  long_div((Bit128u*)quotient,(Bit64u*)remainder,(Bit128u*)&temp,divisor);
  if (s1 ^ s2) {
    long_neg(quotient);
  }
  if (s1) {
    *remainder = -*remainder;
  }
}
