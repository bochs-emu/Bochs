/*---------------------------------------------------------------------------+
 |  mul_Xsig.S                                                               |
 |  $Id: mul_Xsig.c,v 1.1.10.1 2002-03-17 08:51:21 bdenney Exp $
 |                                                                           |
 | Multiply a 12 byte fixed point number by another fixed point number.      |
 |                                                                           |
 | Copyright (C) 1992,1994,1995                                              |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@jacobi.maths.monash.edu.au |
 |                                                                           |
 |                                                                           |
 | The result is neither rounded nor normalized, and the ls bit or so may    |
 | be wrong.                                                                 |
 |                                                                           |
 +---------------------------------------------------------------------------*/


#include "fpu_emu.h"
#include "poly.h"


void mul32_Xsig(Xsig *x, const u32 ba)
{
  Xsig y;
  u32 zl;
  u64 b = ba, z;

  z = b * x->lsw;
  y.lsw = z >> 32;

  z = b * x->midw;
  y.midw = z >> 32;
  zl = z;
  y.lsw += zl;
  if ( zl > y.lsw )
    y.midw ++;

  z = b * x->msw;
  y.msw = z >> 32;
  zl = z;
  y.midw += zl;
  if ( zl > y.midw )
    y.msw ++;

  *x = y;

}


void mul64_Xsig(Xsig *x, const u64 *b)
{
  Xsig yh, yl;

  yh = *x;
  yl = *x;
  mul32_Xsig(&yh, (*b) >> 32);
  mul32_Xsig(&yl, *b);

  x->msw = yh.msw;
  x->midw = yh.midw + yl.msw;
  if ( yh.midw > x->midw )
    x->msw ++;
  x->lsw = yh.lsw + yl.midw;
  if ( yh.lsw > x->lsw )
    {
      x->midw ++;
      if ( x->midw == 0 )
	x->msw ++;
    }

}


void mul_Xsig_Xsig(Xsig *x, const Xsig *b)
{
  u32 yh;
  u64 y, z;

  y = b->lsw;
  y *= x->msw;
  yh = y >> 32;

  z = b->msw;
  z <<= 32;
  z += b->midw;
  mul64_Xsig(x, &z);

  x->lsw += yh;
  if ( yh > x->lsw )
    {
      x->midw ++;
      if ( x->midw == 0 )
	x->msw ++;
    }
}

