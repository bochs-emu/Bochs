/*---------------------------------------------------------------------------+
 |  polynomial_Xsig.c                                                        |
 |  $Id: polynom_Xsig.c,v 1.2 2001-10-06 03:53:46 bdenney Exp $
 |                                                                           |
 | Fixed point arithmetic polynomial evaluation.                             |
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1995,1999                                    |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@melbpc.org.au              |
 |                                                                           |
 | Computes:                                                                 |
 | terms[0] + (terms[1] + (terms[2] + ... + (terms[n]*x)*x)*x)*x) ... )*x    |
 | and adds the result to the 12 byte Xsig.                                  |
 | The terms[] are each 8 bytes, but all computation is performed to 12 byte |
 | precision.                                                                |
 |                                                                           |
 | This function must be used carefully: most overflow of intermediate       |
 | results is controlled, but overflow of the result is not.                 |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "fpu_emu.h"
#include "poly.h"


void polynomial_Xsig(Xsig *accum, const u64 *x, const u64 terms[], const int n)
{
  int  i;
  Xsig acc, Xprod;
  u32  lprod;
  u64  xlwr, xupr, prod;
  char overflowed;

  xlwr = (u32)(*x);
  xupr = (u32)((*x) >> 32);

  acc.msw = terms[n] >> 32;
  acc.midw = terms[n];
  acc.lsw = 0;
  overflowed = 0;

  for ( i = n-1; i >= 0; i-- )
    {
      /* Split the product into five parts to get a 16 byte result */

      /* first word by first word */
      prod = acc.msw * xupr;
      Xprod.midw = prod;
      Xprod.msw = prod >> 32;

      /* first word by second word */
      prod = acc.msw * xlwr;
      Xprod.lsw = prod;
      lprod = prod >> 32;
      Xprod.midw += lprod;
      if ( lprod > Xprod.midw )
	Xprod.msw ++;

      /* second word by first word */
      prod = acc.midw * xupr;
      Xprod.lsw += prod;
      if ( (u32)prod > Xprod.lsw )
	{
	  Xprod.midw ++;
	  if ( Xprod.midw == 0 )
	    Xprod.msw ++;
	}
      lprod = prod >> 32;
      Xprod.midw += lprod;
      if ( lprod > Xprod.midw )
	Xprod.msw ++;

      /* second word by second word */
      prod = acc.midw * xlwr;
      lprod = prod >> 32;
      Xprod.lsw += lprod;
      if ( lprod > Xprod.lsw )
	{
	  Xprod.midw ++;
	  if ( Xprod.midw == 0 )
	    Xprod.msw ++;
	}

      /* third word by first word */
      prod = acc.lsw * xupr;
      lprod = prod >> 32;
      Xprod.lsw += lprod;
      if ( lprod > Xprod.lsw )
	{
	  Xprod.midw ++;
	  if ( Xprod.midw == 0 )
	    Xprod.msw ++;
	}

      if ( overflowed )
	{
	  Xprod.midw += xlwr;
	  if ( (u32)xlwr > Xprod.midw )
	    Xprod.msw ++;
	  Xprod.msw += xupr;
	  overflowed = 0;    /* We don't check this addition for overflow */
	}
      
      acc.lsw = Xprod.lsw;
      acc.midw = (u32)terms[i] + Xprod.midw;
      acc.msw = (terms[i] >> 32) + Xprod.msw;
      if ( Xprod.msw > acc.msw )
	overflowed = 1;
      if ( (u32)terms[i] > acc.midw )
	{
	  acc.msw ++;
	  if ( acc.msw == 0 )
	    overflowed = 1;
	}
    }

  /* We don't check the addition to accum for overflow */
  accum->lsw += acc.lsw;
  if ( acc.lsw > accum->lsw )
    {
      accum->midw ++;
      if ( accum->midw == 0 )
	accum->msw ++;
    }
  accum->midw += acc.midw;
  if ( acc.midw > accum->midw )
    {
      accum->msw ++;
    }
  accum->msw += acc.msw;
}


