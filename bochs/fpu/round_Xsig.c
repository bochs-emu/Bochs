/*---------------------------------------------------------------------------+
 |  round_Xsig.c                                                             |
 |  $Id: round_Xsig.c,v 1.2 2001-10-06 03:53:46 bdenney Exp $
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1995,1999                                    |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@melbpc.org.au              |
 |                                                                           |
 | Normalize and round a 12 byte quantity.                                   |
 |   int round_Xsig(Xsig *n)                                                 |
 |                                                                           |
 | Normalize a 12 byte quantity.                                             |
 |   int norm_Xsig(Xsig *n)                                                  |
 |                                                                           |
 | Each function returns the size of the shift (nr of bits).                 |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "fpu_emu.h"
#include "poly.h"

int round_Xsig(Xsig *x)
{
  int n = 0;

  if ( x->msw == 0 )
    {
      x->msw = x->midw;
      x->midw = x->lsw;
      x->lsw = 0;
      n = 32;
    }
  while ( !(x->msw & 0x80000000) )
    {
      x->msw <<= 1;
      if ( x->midw & 0x80000000 ) x->msw |= 1;
      x->midw <<= 1;
      if ( x->lsw & 0x80000000 ) x->midw |= 1;
      x->lsw <<= 1;
      n++;
    }
  if ( x->lsw & 0x80000000 )
    {
      x->midw ++;
      if ( x->midw == 0 )
	x->msw ++;
      if ( x->msw == 0 )
	{
	  x->msw = 0x80000000;
	  n--;
	}
    }
  

  return -n;
}


int norm_Xsig(Xsig *x)
{
  int n = 0;

  if ( x->msw == 0 )
    {
      if ( x->midw == 0 )
	{
	  x->msw = x->lsw;
	  x->midw = 0;
	  x->lsw = 0;
	  n = 64;
	}
      else
	{
	  x->msw = x->midw;
	  x->midw = x->lsw;
	  x->lsw = 0;
	  n = 32;
	}
    }
  while ( !(x->msw & 0x80000000) )
    {
      x->msw <<= 1;
      if ( x->midw & 0x80000000 ) x->msw |= 1;
      x->midw <<= 1;
      if ( x->lsw & 0x80000000 ) x->midw |= 1;
      x->lsw <<= 1;
      n++;
    }

  return -n;
}





