/*---------------------------------------------------------------------------+
 |  wm_sqrt.c                                                                |
 |                                                                           |
 | Fixed point arithmetic square root evaluation.                            |
 |                                                                           |
 | Copyright (C) 1992,1993,1995,1997,1999                                    |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@melbpc.org.au              |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 |    returns the square root of n in n.                                     |
 |                                                                           |
 |  Use Newton's method to compute the square root of a number, which must   |
 |  be in the range  [1.0 .. 4.0),  to 64 bits accuracy.                     |
 |  Does not check the sign or tag of the argument.                          |
 |  Sets the exponent, but not the sign or tag of the result.                |
 |                                                                           |
 |  The guess is kept in %esi:%edi                                           |
 +---------------------------------------------------------------------------*/

#include "exception.h"
#include "fpu_emu.h"

/*
  The following value indicates the trailing bits (of 96 bits)
  which may be in error when the final Newton iteration is finished
  (0x20 corresponds to the last 5 bits in error, i.e. 91 bits precision).
  A check of the following code with more than 3 billion (3.0e9) random
  and selected values showed that 0x10 was always a large enough value,
  so 0x20 should be a conservative choice.
 */
#define ERR_MARGIN 0x20


int wm_sqrt(FPU_REG *n, s32 dummy1, s32 dummy2, u16 control_w, u8 sign)
{
  u64 nn, guess, halfn, lowr, mid, upr, diff, uwork;
  s64 work;
  u32 ne, guess32, work32, diff32, mid32;
  int shifted;

  nn = significand(n);
  ne = 0;
  if ( exponent16(n) == EXP_BIAS )
    {
      /* Shift the argument right one position. */
      if ( nn & 1 )
	ne = 0x80000000;
      nn >>= 1;
      guess = n->sigh >> 2;
      shifted = 1;
    }
  else
    {
      guess = n->sigh >> 1;
      shifted = 0;
    }

  guess += 0x40000000;
  guess *= 0xaaaaaaaa;
  guess <<= 1;
  guess32 = guess >> 32;
  if ( !(guess32 & 0x80000000) )
    guess32 = 0x80000000;
  halfn = nn >> 1;

  guess32 = halfn / guess32 + (guess32 >> 1);
  guess32 = halfn / guess32 + (guess32 >> 1);
  guess32 = halfn / guess32 + (guess32 >> 1);


/*
 * Now that an estimate accurate to about 30 bits has been obtained,
 * we improve it to 60 bits or so.
 *
 * The strategy from now on is to compute new estimates from
 *      guess := guess + (n - guess^2) / (2 * guess)
 */

  work = guess32;
  work = nn - work * guess32;
  work <<= 28;       /* 29 - 1 */
  work /= guess32;
  work <<= 3;        /* 29 + 3 = 32 */
  work += ((u64)guess32) << 32;

  if ( work == 0 )  /* This happens in one or two special cases */
    work = 0xffffffffffffffffLL;

  guess = work;

  /* guess is now accurate to about 60 bits */


  if ( work > 0 )
    {
#ifdef PARANOID
      if ( (n->sigh != 0xffffffff) && (n->sigl != 0xffffffff) )
	{
	  EXCEPTION(EX_INTERNAL|0x213);
	}
#endif
      /* We know the answer here. */
      return FPU_round(n, 0x7fffffff, 0, control_w, sign);
    }

  /* Refine the guess to significantly more than 64 bits. */

  /* First, square the current guess. */

  guess32 = guess >> 32;
  work32 = guess;

  /* lower 32 times lower 32 */
  lowr = work32;
  lowr *= work32;

  /* lower 32 times upper 32 */
  mid = guess32;
  mid *= work32;

  /* upper 32 times upper 32 */
  upr = guess32;
  upr *= guess32;

  /* upper 32 bits of the middle product times 2 */
  upr += mid >> (32-1);

  /* lower 32 bits of the middle product times 2 */
  work32 = mid << 1;

  /* upper 32 bits of the lower product */
  mid32 = lowr >> 32;
  mid32 += work32;
  if ( mid32 < work32 )
    upr ++;

  /* We now have the first 96 bits (truncated) of the square of the guess */

  diff = upr - nn;
  diff32 = mid32 - ne;
  if ( diff32 > mid32 )
    diff --;

  if ( ((s64)diff) < 0 )
    {
      /* The difference is negative, negate it. */
      diff32 = -((s32)diff32);
      diff = ~diff;
      if ( diff32 == 0 )
	diff ++;
#ifdef PARANOID
      if ( (diff >> 32) != 0 )
	{
	  EXCEPTION(EX_INTERNAL|0x207);
	}
#endif

      diff <<= 32;
      diff |= diff32;
      work32 = diff / guess32;
      work = work32;
      work <<= 32;

      diff = diff % guess32;
      diff <<= 32;
      work32 = diff / guess32;

      work |= work32;

      work >>= 1;
      work32 = work >> 32;


      guess += work32;       /* The first 64 bits */
      guess32 = work;        /* The next 32 bits */
      /* The guess should now be good to about 90 bits */
    }
  else
    {
      /* The difference is positive. */
      diff <<= 32;
      diff |= diff32;

      work32 = diff / guess32;
      work = work32;
      work <<= 32;

      diff = diff % guess32;
      diff <<= 32;
      work32 = diff / guess32;

      work |= work32;

      work >>= 1;
      work32 = work >> 32;

      guess32 = work;        /* The last 32 bits (of 96) */
      guess32 = -guess32;
      if ( guess32 )
	guess --;
      guess -= work32;       /* The first 64 bits */
      /* The guess should now be good to about 90 bits */
    }


  setexponent16(n, 0);

  if ( guess32 >= (u32) -ERR_MARGIN )
    {
      /* Nearly exact, we round the 64 bit result upward. */
      guess ++;
    }
  else if ( (guess32 > ERR_MARGIN) &&
	   ((guess32 < 0x80000000-ERR_MARGIN)
	    || (guess32 > 0x80000000+ERR_MARGIN)) )
    {
      /* We have enough accuracy to decide rounding */
      significand(n) = guess;
      return FPU_round(n, guess32, 0, control_w, sign);
    }

  if ( (guess32 <= ERR_MARGIN) || (guess32 >= (u32) -ERR_MARGIN) )
    {
      /*
       * This is an easy case because x^1/2 is monotonic.
       * We need just find the square of our estimate, compare it
       * with the argument, and deduce whether our estimate is
       * above, below, or exact. We use the fact that the estimate
       * is known to be accurate to about 90 bits.
       */


      /* We compute the lower 64 bits of the 128 bit product */
      work32 = guess;
      lowr = work32;
      lowr *= work32;

      uwork = guess >> 32;
      work32 = guess;
      uwork *= work32;
      uwork <<= 33;   /* 33 = 32+1 (for two times the product) */

      lowr += uwork;  /* We now have the 64 bits */

      /* We need only look at bits 65..96 of the square of guess. */
      if ( shifted )
	work32 = lowr >> 31;
      else
	work32 = lowr >> 32;

#ifdef PARANOID
      if ( ((s32)work32 > 3*ERR_MARGIN) || ((s32)work32 < -3*ERR_MARGIN) )
	{
	  EXCEPTION(EX_INTERNAL|0x214);
	}
#endif

      significand(n) = guess;
      if ( (s32)work32 > 0 )
	{
	  /* guess is too large */
	  significand(n) --;
	  return FPU_round(n, 0xffffff00, 0, control_w, sign);
	}
      else if ( (s32)work32 < 0 )
	{
	  /* guess is a little too small */
	  return FPU_round(n, 0x000000ff, 0, control_w, sign);
	}

      else if ( (u32)lowr != 0 )
       {

         /* guess is too large */
         significand(n) --;
         return FPU_round(n, 0xffffff00, 0, control_w, sign);
       }

      /* Our guess is precise. */
      return FPU_round(n, 0, 0, control_w, sign);
    }

  /* Very similar to the case above, but the last bit is near 0.5.
     We handle this just like the case above but we shift everything
     by one bit. */


  uwork = guess;
  uwork <<= 1;
  uwork |= 1;      /* add the half bit */

  /* We compute the lower 64 bits of the 128 bit product */
  work32 = uwork;
  lowr = work32;
  lowr *= work32;

  work32 = uwork >> 32;
  uwork &= 0xffffffff;
  uwork *= work32;
  uwork <<= 33;   /* 33 = 32+1 (for two times the product) */

  lowr += uwork;  /* We now have the 64 bits. The lowest 32 bits of lowr
                    are not all zero (the lsb is 1). */

  /* We need only look at bits 65..96 of the square of guess. */
  if ( shifted )
    work32 = lowr >> 31;
  else
    work32 = lowr >> 32;

#ifdef PARANOID
  if ( ((s32)work32 > 4*3*ERR_MARGIN) || ((s32)work32 < -4*3*ERR_MARGIN) )
    {
      EXCEPTION(EX_INTERNAL|0x215);
    }
#endif

  significand(n) = guess;
  if ( (s32)work32 < 0 )
    {
      /* guess plus half bit is a little too small */
      return FPU_round(n, 0x800000ff, 0, control_w, sign);
    }
  else /* Note that the lower 64 bits of the product are not all zero */
    {
      /* guess plus half bit is too large */
      return FPU_round(n, 0x7fffff00, 0, control_w, sign);
    }

  /*
    Note that the result of a square root cannot have precisely a half bit
    of a least significant place (it is left as an exercise for the reader
    to prove this! (hint: 65 bit*65 bit => n bits)).
  */

}

