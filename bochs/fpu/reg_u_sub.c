/*---------------------------------------------------------------------------+
 |  reg_u_sub.c                                                              |
 |                                                                           |
 | Core floating point subtraction routine.                                  |
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
 |    Kernel subtraction routine FPU_u_sub(reg *arg1, reg *arg2, reg *answ).
 |    Takes two valid reg f.p. numbers (TAG_Valid), which are
 |    treated as unsigned numbers,
 |    and returns their difference as a TAG_Valid or TAG_Zero f.p.
 |    number.
 |    The first number (arg1) must be the larger.
 |    The returned number is normalized.
 |    Basic checks are performed if PARANOID is defined.
 */

#include "exception.h"
#include "fpu_emu.h"
#include "control_w.h"



int  FPU_u_sub(const FPU_REG *arg1, const FPU_REG *arg2, FPU_REG *dest,
	       u16 control_w, u_char sign, int expa, int expb)
{
  FPU_REG shifted, answ;
  u32 extent;
  int ediff = expa - expb, ed2, borrow;

#ifdef PARANOID
  if ( ediff < 0 )
    {
      EXCEPTION(EX_INTERNAL|0x206);
      return -1;
    }
#endif
  
  answ.exp = expa;

#ifdef PARANOID
  if ( !(arg1->sigh & 0x80000000) || !(arg2->sigh & 0x80000000) )
    {
      EXCEPTION(EX_INTERNAL|0x209);
      return -1;
    }
#endif

  if ( ediff == 0 )
    {
      shifted.sigl = arg2->sigl;
      shifted.sigh = arg2->sigh;
      extent = 0;
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
	  extent = arg2->sigl;
	  shifted.sigl = arg2->sigh;
	  shifted.sigh = 0;
	}
      else
	{
	  ed2 = 32 - ediff;
	  extent = arg2->sigl >> ediff;
	  extent |= (arg2->sigh << ed2);
	  if ( arg2->sigl << ed2 )
	    extent |= 1;
	  shifted.sigl = arg2->sigh >> ediff;
	  shifted.sigh = 0;
	}
    }
  else
    {
      ediff -= 64;
      if ( ! ediff )
	{
	  extent = arg2->sigh;
	  if ( arg2->sigl )
	    extent |= 1;
	  shifted.sigl = 0;
	  shifted.sigh = 0;
	}
      else
	{
	  if ( ediff < 32 )
	    {
	      extent = arg2->sigh >> ediff;
	      if ( arg2->sigl || (arg2->sigh << (32-ediff)) )
		extent |= 1;
	    }
	  else
	    extent = 1;
	  shifted.sigl = 0;
	  shifted.sigh = 0;
	}
    }

  extent = -extent;
  borrow = extent;
  answ.sigl = arg1->sigl - shifted.sigl;
  if ( answ.sigl > arg1->sigl )
    {
      if ( borrow )
	answ.sigl --;
      borrow = 1;
    }
  else if ( borrow )
    {
      answ.sigl --;
      if ( answ.sigl != 0xffffffff )
	borrow = 0;
    }
  answ.sigh = arg1->sigh - shifted.sigh;
  if ( answ.sigh > arg1->sigh )
    {
      if ( borrow )
	answ.sigh --;
      borrow = 1;
    }
  else if ( borrow )
    {
      answ.sigh --;
      if ( answ.sigh != 0xffffffff )
	borrow = 0;
    }

#ifdef PARANOID
  if ( borrow )
    {
      /* This can only occur if the code is bugged */
      EXCEPTION(EX_INTERNAL|0x212);
      return -1;
    }
#endif

  if ( answ.sigh & 0x80000000 )
    {
      /*
	The simpler "*dest = answ" is broken in gcc
      */
      dest->exp = answ.exp;
      dest->sigh = answ.sigh;
      dest->sigl = answ.sigl;
      return FPU_round(dest, extent, 0, control_w, sign);
    }

  if ( answ.sigh == 0 )
    {
      if ( answ.sigl )
	{
	  answ.sigh = answ.sigl;
	  answ.sigl = extent;
	  extent = 0;
	  answ.exp -= 32;
	}
      else if ( extent )
	{
/*
 *	 A rare case, the only one which is non-zero if we got here
 *         is:           1000000 .... 0000
 *                      -0111111 .... 1111 1
 *                       -------------------- 
 *                       0000000 .... 0000 1 
 */
	  if ( extent != 0x80000000 )
	    {
	      /* This can only occur if the code is bugged */
	      EXCEPTION(EX_INTERNAL|0x210);
	      return -1;
	    }
	  dest->sigh = extent;
	  dest->sigl = extent = 0;
	  dest->exp -= 64;
	  return FPU_round(dest, extent, 0, control_w, sign);
	}
      else
	{
	  dest->exp = 0;
	  dest->sigh = dest->sigl = 0;
	  return TAG_Zero;
	}
    }

  while ( !(answ.sigh & 0x80000000) )
    {
      answ.sigh <<= 1;
      if ( answ.sigl & 0x80000000 )
	answ.sigh |= 1;
      answ.sigl <<= 1;
      if ( extent & 0x80000000 )
	answ.sigl |= 1;
      extent <<= 1;
      answ.exp --;
    }

  dest->exp = answ.exp;
  dest->sigh = answ.sigh;
  dest->sigl = answ.sigl;

  return FPU_round(dest, extent, 0, control_w, sign);

}
