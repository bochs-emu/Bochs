/*---------------------------------------------------------------------------+
 |  div_Xsig.S                                                               |
 |  $Id: div_Xsig.c,v 1.2 2001-10-06 03:53:46 bdenney Exp $
 |                                                                           |
 | Division subroutine for 96 bit quantities                                 |
 |                                                                           |
 | Copyright (C) 1994,1995,1999                                              |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@melbpc.org.au              |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 | Divide the 96 bit quantity pointed to by a, by that pointed to by b, and  |
 | put the 96 bit result at the location d.                                  |
 |                                                                           |
 | The result may not be accurate to 96 bits. It is intended for use where   |
 | a result better than 64 bits is required. The result should usually be    |
 | good to at least 94 bits.                                                 |
 | The returned result is actually divided by one half. This is done to      |
 | prevent overflow.                                                         |
 |                                                                           |
 |  .aaaaaaaaaaaaaa / .bbbbbbbbbbbbb  ->  .dddddddddddd                      |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "exception.h"
#include "fpu_emu.h"
#include "poly.h"


void div_Xsig(const Xsig *aa, const Xsig *b, Xsig *dest)
{
  Xsig a = *aa, xpr, result;
  u32  prodh, prodl, den, wd;
  u64  num, prod;

#ifdef PARANOID
  if ( (b->msw & 0x80000000) == 0 )
    {
      EXCEPTION(EX_INTERNAL|0x240);
      return;
    }
#endif

  /* Shift a right */
  a.lsw >>= 1;
  if ( a.midw & 1 )
    a.lsw |= 0x80000000;
  a.midw >>= 1;
  if ( a.msw & 1 )
    a.midw |= 0x80000000;
  a.msw >>= 1;

  num = a.msw;
  num <<= 32;
  num |= a.midw;

  den = b->msw + 1;
  if ( den )
    {
      result.msw = num / den;
    }
  else
    result.msw = a.msw;

  xpr = *b;
  mul32_Xsig(&xpr, result.msw);
  a.msw -= xpr.msw;
  wd = a.midw;
  a.midw -= xpr.midw;
  if ( a.midw > wd )
    a.msw --;
  wd = a.lsw;
  a.lsw -= xpr.lsw;
  if ( a.lsw > wd )
    {
      a.midw --;
      if ( a.midw == 0xffffffff )
	a.msw --;
    }

#ifdef PARANOID
      if ( a.msw > 1 )
	{
	  EXCEPTION(EX_INTERNAL|0x241);
	}
#endif

  while ( (a.msw != 0) || (a.midw > b->msw) )
    {
      wd = a.midw;
      a.midw -= b->msw;
      if ( a.midw > wd )
	a.msw --;
      wd = a.lsw;
      a.lsw -= b->midw;
      if ( a.lsw > wd )
	{
	  a.midw --;
	  if ( a.midw == 0xffffffff )
	    a.msw --;
	}
      result.msw ++;
    }

  /* Whew! result.msw is now done. */

  num = a.midw;
  num <<= 32;
  num |= a.lsw;

  if ( den )
    {
      result.midw = num / den;
    }
  else
    result.midw = a.midw;

  prod = result.midw;
  prod *= b->msw;
  a.midw -= prod >> 32;
  prodl = prod;
  wd = a.lsw;
  a.lsw -= prodl;
  if ( a.lsw > wd )
    a.midw --;

  prod = result.midw;
  prod *= b->midw;
  prodh = prod >> 32;
  wd = a.lsw;
  a.lsw -= prodh;
  if ( a.lsw > wd )
    a.midw --;

#ifdef PARANOID
      if ( a.midw > 1 )
	{
          EXCEPTION(EX_INTERNAL|0x242);
	}
#endif

  while ( (a.midw != 0) || (a.lsw > b->msw) )
    {
      wd = a.lsw;
      a.lsw -= b->msw;
      if ( a.lsw > wd )
	a.midw --;
      result.midw ++;
    }


  /* Now result.msw is done, the lsw is next... */

  num = a.lsw;
  num <<= 32;

  if ( den )
    {
      result.lsw = num / den;
    }
  else
    result.lsw = a.lsw;

  prod = result.lsw;
  prod *= b->msw;
  a.lsw -= prod >> 32;

#ifdef PARANOID
  if ( a.lsw > 2 )
    {
      EXCEPTION(EX_INTERNAL|0x243);
    }
#endif

  result.lsw -= a.lsw;

  /* Hey! we're done. */

  *dest = result;

}

