/*---------------------------------------------------------------------------+
 |  reg_u_mul.c                                                              |
 |  $Id: reg_u_mul.c,v 1.3 2001-10-06 03:53:46 bdenney Exp $
 |                                                                           |
 | Core multiplication routine                                               |
 |                                                                           |
 | Copyright (C) 1992,1993,1995,1997,1999                                    |
 |                  W. Metzenthen, 22 Parker St, Ormond, Vic 3163, Australia |
 |                  E-mail   billm@melbpc.org.au                             |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 |   Basic multiplication routine.                                           |
 |   Does not check the resulting exponent for overflow/underflow            |
 |                                                                           |
 |   Internal working is at approx 128 bits.                                 |
 |   Result is rounded to nearest 53 or 64 bits, using "nearest or even".    |
 +---------------------------------------------------------------------------*/

#include "exception.h"
#include "fpu_emu.h"
#include "control_w.h"


int FPU_u_mul(const FPU_REG *a, const FPU_REG *b, FPU_REG *c, u16 cw,
	      u_char sign, int expon)
{
  u64 mu, ml, mi;
  u32 lh, ll, th, tl;

#ifdef PARANOID
  if ( ! (a->sigh & 0x80000000) || ! (b->sigh & 0x80000000) )
    {
      EXCEPTION(EX_INTERNAL|0x205);
    }
#endif

  ml = a->sigl;
  ml *= b->sigl;
  ll = ml;
  lh = ml >> 32;

  mu = a->sigh;
  mu *= b->sigh;

  mi = a->sigh;
  mi *= b->sigl;
  tl = mi;
  th = mi >> 32;
  lh += tl;
  if ( tl > lh )
    mu ++;
  mu += th;

  mi = a->sigl;
  mi *= b->sigh;
  tl = mi;
  th = mi >> 32;
  lh += tl;
  if ( tl > lh )
    mu ++;
  mu += th;

  ml = lh;
  ml <<= 32;
  ml += ll;

  expon -= EXP_BIAS-1;
  if ( expon <= EXP_WAY_UNDER )
    expon = EXP_WAY_UNDER;

  c->exp = expon;

  if ( ! (mu & BX_CONST64(0x8000000000000000)) )
    {
      mu <<= 1;
      if ( ml & BX_CONST64(0x8000000000000000) )
	mu |= 1;
      ml <<= 1;
      c->exp --;
    }

  ll = ml;
  lh = ml >> 32;

  if ( ll )
    lh |= 1;

  c->sigl = mu;
  c->sigh = mu >> 32;

  return FPU_round(c, lh, 0, cw, sign);
  
}
