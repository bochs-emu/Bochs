/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64

static unsigned partial_add(Bit32u *sum, Bit32u b)
{
  Bit32u t = *sum;
  *sum += b;
  return (*sum < t);
}

void long_mul(Bit128u *product, Bit64u op1, Bit64u op2)
{
  Bit32u op_1[2],op_2[2];
  Bit32u result[5];
  Bit64u nn;
  unsigned c;

  int i,j,k;

  op_1[0] = (Bit32u)(op1 & 0xffffffff);
  op_1[1] = (Bit32u)(op1 >> 32);
  op_2[0] = (Bit32u)(op2 & 0xffffffff);
  op_2[1] = (Bit32u)(op2 >> 32);

  for (i = 0; i < 4; i++) result[i] = 0;

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      nn = (Bit64u) op_1[i] * (Bit64u) op_2[j];
      k = i + j;
      c = partial_add(&result[k++], (Bit32u)(nn & 0xffffffff));
      c = partial_add(&result[k++], (Bit32u)(nn >> 32) + c);
      while (k < 4 && c != 0) {
        c = partial_add(&result[k++], c);
      }
    }
  }

  product->lo = result[0] + ((Bit64u) result[1] << 32);
  product->hi = result[2] + ((Bit64u) result[3] << 32);
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MUL_RAXEqR(bxInstruction_c *i)
{
  Bit128u product_128;

  Bit64u op1_64 = RAX;
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  // product_128 = ((Bit128u) op1_64) * ((Bit128u) op2_64);
  // product_64l = (Bit64u) (product_128 & 0xFFFFFFFFFFFFFFFF);
  // product_64h = (Bit64u) (product_128 >> 64);

  long_mul(&product_128,op1_64,op2_64);

  /* now write product back to destination */
  RAX = product_128.lo;
  RDX = product_128.hi;

  /* set EFLAGS */
  SET_FLAGS_OSZAPC_LOGIC_64(product_128.lo);
  if(product_128.hi != 0)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_RAXEqR(bxInstruction_c *i)
{
  Bit128s product_128;

  Bit64s op1_64 = RAX;
  Bit64s op2_64 = BX_READ_64BIT_REG(i->src());

  // product_128 = ((Bit128s) op1_64) * ((Bit128s) op2_64);
  // product_64l = (Bit64u) (product_128 & 0xFFFFFFFFFFFFFFFF);
  // product_64h = (Bit64u) (product_128 >> 64);

  long_imul(&product_128,op1_64,op2_64);

  /* now write product back to destination */
  RAX = product_128.lo;
  RDX = product_128.hi;

  /* set eflags:
   * IMUL r/m64: condition for clearing CF & OF:
   *   RDX:RAX = sign-extend of RAX
   */

  SET_FLAGS_OSZAPC_LOGIC_64(product_128.lo);

  /* magic compare between RDX:RAX and sign extended RAX */
  if (((Bit64u)(product_128.hi) + (product_128.lo >> 63)) != 0) {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DIV_RAXEqR(bxInstruction_c *i)
{
  Bit64u remainder_64, quotient_64l;
  Bit128u op1_128, quotient_128;

  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());
  if (op2_64 == 0) {
    exception(BX_DE_EXCEPTION, 0);
  }

  op1_128.lo = RAX;
  op1_128.hi = RDX;

  // quotient_128 = op1_128 / op2_64;
  // remainder_64 = (Bit64u) (op1_128 % op2_64);
  // quotient_64l = (Bit64u) (quotient_128 & 0xFFFFFFFFFFFFFFFF);

  long_div(&quotient_128,&remainder_64,&op1_128,op2_64);
  quotient_64l = quotient_128.lo;

  if (quotient_128.hi != 0)
    exception(BX_DE_EXCEPTION, 0);

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */
  RAX = quotient_64l;
  RDX = remainder_64;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IDIV_RAXEqR(bxInstruction_c *i)
{
  Bit64s remainder_64, quotient_64l;
  Bit128s op1_128, quotient_128;

  op1_128.lo = RAX;
  op1_128.hi = RDX;

  /* check MIN_INT case */
  if ((op1_128.hi == (Bit64s) BX_CONST64(0x8000000000000000)) && (!op1_128.lo))
    exception(BX_DE_EXCEPTION, 0);

  Bit64s op2_64 = BX_READ_64BIT_REG(i->src());

  if (op2_64 == 0) {
    exception(BX_DE_EXCEPTION, 0);
  }

  // quotient_128 = op1_128 / op2_64;
  // remainder_64 = (Bit64s) (op1_128 % op2_64);
  // quotient_64l = (Bit64s) (quotient_128 & 0xFFFFFFFFFFFFFFFF);

  long_idiv(&quotient_128,&remainder_64,&op1_128,op2_64);
  quotient_64l = quotient_128.lo;

  if ((!(quotient_128.lo & BX_CONST64(0x8000000000000000)) && quotient_128.hi != (Bit64s) 0) ||
       ((quotient_128.lo & BX_CONST64(0x8000000000000000)) && quotient_128.hi != (Bit64s) BX_CONST64(0xffffffffffffffff)))
  {
    exception(BX_DE_EXCEPTION, 0);
  }

  /* set EFLAGS:
   * IDIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */
  RAX = quotient_64l;
  RDX = remainder_64;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_GqEqIdR(bxInstruction_c *i)
{
  Bit128s product_128;

  Bit64s op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64s op2_64 = (Bit32s) i->Id();

  long_imul(&product_128,op1_64,op2_64);

  /* now write product back to destination */
  BX_WRITE_64BIT_REG(i->dst(), product_128.lo);

  SET_FLAGS_OSZAPC_LOGIC_64(product_128.lo);

  if (((Bit64u)(product_128.hi) + (product_128.lo >> 63)) != 0) {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_GqEqR(bxInstruction_c *i)
{
  Bit128s product_128;

  Bit64s op1_64 = BX_READ_64BIT_REG(i->dst());
  Bit64s op2_64 = BX_READ_64BIT_REG(i->src());

  long_imul(&product_128,op1_64,op2_64);

  /* now write product back to destination */
  BX_WRITE_64BIT_REG(i->dst(), product_128.lo);

  SET_FLAGS_OSZAPC_LOGIC_64(product_128.lo);

  if (((Bit64u)(product_128.hi) + (product_128.lo >> 63)) != 0) {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

#endif /* if BX_SUPPORT_X86_64 */
