/*---------------------------------------------------------------------------+
 |  reg_u_add.c                                                              |
 |  $Id: reg_u_add.c,v 1.3 2001-10-06 03:53:46 bdenney Exp $
 |                                                                           |
 | Add two valid (TAG_Valid) FPU_REG numbers, of the same sign, and put the  |
 |   result in a destination FPU_REG.                                        |
 |                                                                           |
 | Copyright (C) 1992,1993,1995,1997,1999                                    |
 |                  W. Metzenthen, 22 Parker St, Ormond, Vic 3163, Australia |
 |                  E-mail   billm@melbpc.org.au                             |
 |                                                                           |
 |    Return value is the tag of the answer, or-ed with FPU_Exception if     |
 |    one was raised, or -1 on internal error.                               |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*
 |    Kernel addition routine FPU_u_add(reg *arg1, reg *arg2, reg *answ).
 |    Takes two valid reg f.p. numbers (TAG_Valid), which are
 |    treated as unsigned numbers,
 |    and returns their sum as a TAG_Valid or TAG_Special f.p. number.
 |    The returned number is normalized.
 |    Basic checks are performed if PARANOID is defined.
 */

#include "exception.h"
#include "fpu_emu.h"
#include "control_w.h"


int  FPU_u_add(const FPU_REG *arg1, const FPU_REG *arg2, FPU_REG *answ,
	       u16 control_w, u_char sign, s32 expa, s32 expb)
{
  const FPU_REG *rtmp;
  FPU_REG shifted;
  u32 extent = 0;
  int ediff = expa - expb, ed2, eflag, ovfl, carry;

  if ( ediff < 0 )
    {
      ediff = -ediff;
      rtmp = arg1;
      arg1 = arg2;
      arg2 = rtmp;
      expa = expb;
    }

  /* Now we have exponent of arg1 >= exponent of arg2 */
  
  answ->exp = expa;

#ifdef PARANOID
  if ( !(arg1->sigh & 0x80000000) || !(arg2->sigh & 0x80000000) )
    {
      EXCEPTION(EX_INTERNAL|0x201);
      return -1;
    }
#endif

  if ( ediff == 0 )
    {
      extent = 0;
      shifted.sigl = arg2->sigl;
      shifted.sigh = arg2->sigh;
    }
  else if ( ediff < 32 )
    {
      ed2 = 32 - ediff;
      extent = arg2->sigl << ed2;
      shifted.sigl = arg2->sigl >> ediff;
      shifted.sigl |= (arg2->sigh << ed2);
      shifted.sigh = arg2->sigh >> ediff;
    }
  else if ( ediff < 64 )
    {
      ediff -= 32;
      if ( ! ediff )
	{
	  eflag = 0;
	  extent = arg2->sigl;
	  shifted.sigl = arg2->sigh;
	}
      else
	{
	  ed2 = 32 - ediff;
	  eflag = arg2->sigl;
	  if ( eflag )
	    extent |= 1;
	  extent = arg2->sigl >> ediff;
	  extent |= (arg2->sigh << ed2);
	  shifted.sigl = arg2->sigh >> ediff;
	}
      shifted.sigh = 0;
    }
  else
    {
      ediff -= 64;
      if ( ! ediff )
	{
	  eflag = arg2->sigl;
	  extent = arg2->sigh;
	}
      else
	{
	  ed2 = 64 - ediff;
	  eflag = arg2->sigl | arg2->sigh;
	  extent = arg2->sigh >> ediff;
	}
      shifted.sigl = 0;
      shifted.sigh = 0;
      if ( eflag )
	extent |= 1;
    }

  answ->sigh = arg1->sigh + shifted.sigh;
  ovfl = shifted.sigh > answ->sigh;
  answ->sigl = arg1->sigl + shifted.sigl;
  if ( shifted.sigl > answ->sigl )
    {
      answ->sigh ++;
      if ( answ->sigh == 0 )
	ovfl = 1;
    }
  if ( ovfl )
    {
      carry = extent & 1;
      extent >>= 1;
      extent |= carry;
      if ( answ->sigl & 1 )
	extent |= 0x80000000;
      answ->sigl >>= 1;
      if ( answ->sigh & 1 )
	answ->sigl |= 0x80000000;
      answ->sigh >>= 1;
      answ->sigh |= 0x80000000;
      answ->exp ++;
    }

  return FPU_round(answ, extent, 0, control_w, sign);

}
