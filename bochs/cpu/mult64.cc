/////////////////////////////////////////////////////////////////////////
// $Id: mult64.cc,v 1.5 2002-10-25 11:44:35 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA






#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR


unsigned partial_add(Bit32u *sum,Bit32u b)
{
  Bit32u t = *sum;
  *sum += b;
  return (*sum < t);
}

 void
long_mul(Bit128u *product, Bit64u op1, Bit64u op2)
{
  Bit32u op_1[2],op_2[2];
  Bit32u result[5];
  Bit64u nn;
  unsigned c;

  int i,j,k;

  op_1[0] = op1 & 0xffffffff;
  op_1[1] = op1 >> 32;
  op_2[0] = op2 & 0xffffffff;
  op_2[1] = op2 >> 32;

  for (i = 0; i < 4; i++) result[i] = 0;

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      nn = (Bit64u) op_1[i] * (Bit64u) op_2[j];
      k = i + j;
      c = partial_add(&result[k++],nn & 0xffffffff);
      c = partial_add(&result[k++],(nn >> 32) + c);
      while (k < 4 && c != 0) {
        c = partial_add(&result[k++],c);
      }
    }
  }

  product->lo = result[0] + ((Bit64u) result[1] << 32);
  product->hi = result[2] + ((Bit64u) result[3] << 32);
}

 void
long_neg(Bit128s *n)
{
  Bit64u t;

  t = n->lo;
  n->lo = -n->lo;
  if (t > (Bit64u)n->lo) --n->hi;
  n->hi = -n->hi;
}

 void
long_imul(Bit128s *product, Bit64s op1, Bit64s op2)
{
  unsigned s1,s2;

  //BX_DEBUG (("long_imul %08X%08X X %08X%08X->",(unsigned)(op1 >> 32),(unsigned)op1,(unsigned)(op2 >> 32),(unsigned)op2));
  if (s1 = (op1 < 0)) op1 = -op1;
  if (s2 = (op2 < 0)) op2 = -op2;
  long_mul((Bit128u*)product,(Bit64u)op1,(Bit64u)op2);
  if (s1 ^ s2) {
    long_neg(product);
  }
  //BX_DEBUG (("%08X%08X%08X%08X",
  //     (unsigned)(product->hi >> 32),(unsigned)product->hi,
  //     (unsigned)(product->lo >> 32),(unsigned)product->lo));
}

 void
long_shl(Bit128u *a)
{
  Bit64u c;
  c = a->lo >> 63;
  a->lo <<= 1;
  a->hi <<= 1;
  a->hi |= c;
}

 void
long_shr(Bit128u *a)
{
  Bit64u c;
  c = a->hi << 63;
  a->hi >>= 1;
  a->lo >>= 1;
  a->lo |= c;
}

 unsigned
long_sub(Bit128u *a,Bit128u *b)
{
  Bit64u t;
  int c;

  t = a->lo;
  a->lo -= b->lo;
  c = (a->lo > t);
  t = a -> hi;
  a->hi -= b->hi + c;
  return(a->hi > t);
}

 bx_bool
long_le(Bit128u *a,Bit128u *b)
{
  if (a->hi == b->hi) {
    return(a->lo <= b->lo);
  } else {
    return(a->hi <= b->hi);
  }
}


 void
long_div(Bit128u *quotient,Bit64u *remainder,Bit128u *dividend,Bit64u divisor)
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

  //BX_DEBUG (("ldiv: n=%d d=%08X acc=%08X",n,d.lo,acc.lo));
  while (long_le(&d,&acc) && n < 128) {
    long_shl(&d);
    n++;
  }
  //BX_DEBUG (("ldiv: n=%d d=%08X acc=%08X",n,d.lo,acc.lo));
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
  //BX_DEBUG (("ldiv: n=%d d=%08X acc=%08X",n,d.lo,acc.lo));
  *remainder = acc.lo;
  quotient->lo  = q.lo;
  quotient->hi  = q.hi;
}

 void
long_idiv(Bit128s *quotient,Bit64s *remainder,Bit128s *dividend,Bit64s divisor)
{
  unsigned s1,s2;
  Bit128s temp;

  temp = *dividend;
  if (s1 = (temp.hi < 0)) {
    long_neg(&temp);
  }
  if (s2 = (divisor < 0)) divisor = -divisor;
  long_div((Bit128u*)quotient,(Bit64u*)remainder,(Bit128u*)&temp,divisor);
  if (s1 ^ s2) {
    long_neg(quotient);
  }
  if (s2) {
    *remainder = -*remainder;
  }
}

  void
BX_CPU_C::MUL_RAXEq(bxInstruction_c *i)
{
    Bit64u op1_64, op2_64;
    Bit128u product_128;
    bx_bool temp_flag;

    op1_64 = RAX;

    /* op2 is a register or memory reference */
    if (i->modC0()) {
      op2_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
      }

    //product_128 = ((Bit128u) op1_64) * ((Bit128u) op2_64);
    long_mul(&product_128,op1_64,op2_64);

    //product_64l = (Bit64u) (product_128 & 0xFFFFFFFFFFFFFFFF);
    //product_64h = (Bit64u) (product_128 >> 64);

    /* now write product back to destination */

    RAX = product_128.lo;
    RDX = product_128.hi;

    /* set eflags:
     * MUL affects the following flags: C,O
     */

    temp_flag = (product_128.hi != 0);
    SET_FLAGS_OxxxxC(temp_flag, temp_flag);
}



  void
BX_CPU_C::IMUL_RAXEq(bxInstruction_c *i)
{
    Bit64s op1_64, op2_64;
    Bit128s product_128;
    //Bit64u product_64h, product_64l;

    op1_64 = RAX;

    /* op2 is a register or memory reference */
    if (i->modC0()) {
      op2_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2_64);
      }

    //product_128 = ((Bit128s) op1_64) * ((Bit128s) op2_64);
    long_imul(&product_128,op1_64,op2_64);

    //product_64l = (Bit64u) (product_128 & 0xFFFFFFFFFFFFFFFF);
    //product_64h = (Bit64u) (product_128 >> 64);

    /* now write product back to destination */

    RAX = product_128.lo;
    RDX = product_128.hi;

    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r/m16: condition for clearing CF & OF:
     *   RDX:RAX = sign-extend of RAX
     */

    if ( (RDX==BX_CONST64(0xffffffffffffffff)) && (RAX & BX_CONST64(0x8000000000000000)) ) {
      SET_FLAGS_OxxxxC(0, 0);
      }
    else if ( (RDX==BX_CONST64(0x0000000000000000)) && (RAX < BX_CONST64(0x8000000000000000)) ) {
      SET_FLAGS_OxxxxC(0, 0);
      }
    else {
      SET_FLAGS_OxxxxC(1, 1);
      }
}


  void
BX_CPU_C::DIV_RAXEq(bxInstruction_c *i)
{
    Bit64u op2_64, remainder_64, quotient_64l;
    Bit128u op1_128, quotient_128;

    op1_128.lo = RAX;
    op1_128.hi = RDX;

    /* op2 is a register or memory reference */
    if (i->modC0()) {
      op2_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
      }

    if (op2_64 == 0) {
      exception(BX_DE_EXCEPTION, 0, 0);
      }
    //quotient_128 = op1_128 / op2_64;
    //remainder_64 = (Bit64u) (op1_128 % op2_64);
    //quotient_64l = (Bit64u) (quotient_128 & 0xFFFFFFFFFFFFFFFF);
    long_div(&quotient_128,&remainder_64,&op1_128,op2_64);
    quotient_64l = quotient_128.lo;

    //if (quotient_128 != quotient_64l) {
    if (quotient_128.hi != 0) {
      exception(BX_DE_EXCEPTION, 0, 0);
      }

    /* set EFLAGS:
     * DIV affects the following flags: O,S,Z,A,P,C are undefined
     */

    /* now write quotient back to destination */

    RAX = quotient_64l;
    RDX = remainder_64;
}


  void
BX_CPU_C::IDIV_RAXEq(bxInstruction_c *i)
{
    Bit64s op2_64, remainder_64, quotient_64l;
    Bit128s op1_128, quotient_128;

    op1_128.lo = RAX;
    op1_128.hi = RDX;

    /* op2 is a register or memory reference */
    if (i->modC0()) {
      op2_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2_64);
      }

    if (op2_64 == 0) {
      exception(BX_DE_EXCEPTION, 0, 0);
      }
    //quotient_128 = op1_128 / op2_64;
    //remainder_64 = (Bit64s) (op1_128 % op2_64);
    //quotient_64l = (Bit64s) (quotient_128 & 0xFFFFFFFFFFFFFFFF);
    long_idiv(&quotient_128,&remainder_64,&op1_128,op2_64);
    quotient_64l = quotient_128.lo;

    //if (quotient_128 != quotient_64l) {
    if (quotient_128.hi != 0) {
      exception(BX_DE_EXCEPTION, 0, 0);
      }

    /* set EFLAGS:
     * IDIV affects the following flags: O,S,Z,A,P,C are undefined
     */

    /* now write quotient back to destination */

    RAX = quotient_64l;
    RDX = remainder_64;
}


  void
BX_CPU_C::IMUL_GqEqId(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("IMUL_GdEdId() unsupported on 8086!"));
#else


    Bit64s op2_64, op3_64, product_64;
    Bit128s product_128;

    op3_64 = (Bit32s) i->Id();

    /* op2 is a register or memory reference */
    if (i->modC0()) {
      op2_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2_64);
      }

    product_64 = op2_64 * op3_64;
    //product_128 = ((Bit128s) op2_64) * ((Bit128s) op3_64);
    long_imul(&product_128,op2_64,op3_64);

    /* now write product back to destination */
    BX_WRITE_64BIT_REG(i->nnn(), product_64);

    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */

    if (product_128.lo == product_64) {
      SET_FLAGS_OxxxxC(0, 0);
      }
    else {
      SET_FLAGS_OxxxxC(1, 1);
      }
#endif
}


  void
BX_CPU_C::IMUL_GqEq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("IMUL_GvEv() unsupported on 8086!"));
#else

    Bit64s op1_64, op2_64, product_64;
    Bit128s product_128;

    /* op2 is a register or memory reference */
    if (i->modC0()) {
      op2_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2_64);
      }

    op1_64 = BX_READ_64BIT_REG(i->nnn());

    product_64 = op1_64 * op2_64;
    //product_128 = ((Bit128s) op1_64) * ((Bit128s) op2_64);
    long_imul(&product_128,op1_64,op2_64);

    /* now write product back to destination */
    BX_WRITE_64BIT_REG(i->nnn(), product_64);

    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */

    if (product_128.lo == product_64) {
      SET_FLAGS_OxxxxC(0, 0);
      }
    else {
      SET_FLAGS_OxxxxC(1, 1);
      }
#endif
}
