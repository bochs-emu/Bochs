/*---------------------------------------------------------------------------+
 |  reg_u_div.c                                                              |
 |                                                                           |
 | Divide one FPU_REG by another and put the result in a destination FPU_REG.|
 |                                                                           |
 | Copyright (C) 1992,1993,1995,1997,1999                                    |
 |                  W. Metzenthen, 22 Parker St, Ormond, Vic 3163, Australia |
 |                  E-mail   billm@melbpc.org.au                             |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 |                                                                           |
 |  Does not compute the destination exponent, but does adjust it.           |
 |                                                                           |
 |    Return value is the tag of the answer, or-ed with FPU_Exception if     |
 |    one was raised, or -1 on internal error.                               |
 +---------------------------------------------------------------------------*/

#include "exception.h"
#include "fpu_emu.h"
#include "control_w.h"
#include <asm/types.h>


int FPU_u_div(const FPU_REG *a, const FPU_REG *b, FPU_REG *dest,
	      u16 control_w, u8 sign)
{
  s32 exp;
  u32 divr32, rem, rat1, rat2, work32, accum3, prodh;
  u64 work64, divr64, prod64, accum64;
  u8 ovfl;

  exp = (s32)a->exp - (s32)b->exp;

  if ( exp < EXP_WAY_UNDER )
    exp = EXP_WAY_UNDER;

  dest->exp = exp;
#ifdef PARANOID
  if ( !(b->sigh & 0x80000000) )
    {
      EXCEPTION(EX_INTERNAL|0x202);
    }
#endif

  work64 = significand(a);

  /* We can save a lot of time if the divisor has all its lowest
     32 bits equal to zero. */
  if ( b->sigl == 0 )
    {
      divr32 = b->sigh;
      ovfl = a->sigh >= divr32;
      rat1 = work64 / divr32;
      rem = work64 % divr32;
      work64 = rem;
      work64 <<= 32;
      rat2 = work64 / divr32;
      rem = work64 % divr32;

      work64 = rem;
      work64 <<= 32;
      rem = work64 / divr32;

      if ( ovfl )
	{
	  rem >>= 1;
	  if ( rat2 & 1 )
	    rem |= 0x80000000;
	  rat2 >>= 1;
	  if ( rat1 & 1 )
	    rat2 |= 0x80000000;
	  rat1 >>= 1;
	  rat1 |= 0x80000000;
	  dest->exp ++;
	}
      dest->sigh = rat1;
      dest->sigl = rat2;

      dest->exp --;
      return FPU_round(dest, rem, 0, control_w, sign);
    }

  /* This may take a little time... */

  accum64 = work64;
  divr64 = significand(b);

  if ( (ovfl = accum64 >= divr64) )
    accum64 -= divr64;
  divr32 = b->sigh+1;

  if ( divr32 != 0 )
    {
      rat1 = accum64 / divr32;
    }
  else
    rat1 = accum64 >> 32;
  prod64 = rat1 * (u64)b->sigh;

  accum64 -= prod64;
  prod64 = rat1 * (u64)b->sigl;
  accum3 = prod64;
  if ( accum3 )
    {
      accum3 = -accum3;
      accum64 --;
    }
  prodh = prod64 >> 32;
  accum64 -= prodh;

  work32 = accum64 >> 32;
  if ( work32 )
    {
#ifdef PARANOID
      if ( work32 != 1 )
	{
	  EXCEPTION(EX_INTERNAL|0x203);
	}
#endif

      /* Need to subtract the divisor once more. */
      work32 = accum3;
      accum3 = work32 - b->sigl;
      if ( accum3 > work32 )
	accum64 --;
      rat1 ++;
      accum64 -= b->sigh;

#ifdef PARANOID
      if ( (accum64 >> 32) )
	{
	  EXCEPTION(EX_INTERNAL|0x203);
	}
#endif
    }

  /* Now we essentially repeat what we have just done, but shifted
     32 bits. */

  accum64 <<= 32;
  accum64 |= accum3;
  if ( accum64 >= divr64 )
    {
      accum64 -= divr64;
      rat1 ++;
    }
  if ( divr32 != 0 )
    {
      rat2 = accum64 / divr32;
    }
  else
    rat2 = accum64 >> 32;
  prod64 = rat2 * (u64)b->sigh;

  accum64 -= prod64;
  prod64 = rat2 * (u64)b->sigl;
  accum3 = prod64;
  if ( accum3 )
    {
      accum3 = -accum3;
      accum64 --;
    }
  prodh = prod64 >> 32;
  accum64 -= prodh;

  work32 = accum64 >> 32;
  if ( work32 )
    {
#ifdef PARANOID
      if ( work32 != 1 )
	{
	  EXCEPTION(EX_INTERNAL|0x203);
	}
#endif

      /* Need to subtract the divisor once more. */
      work32 = accum3;
      accum3 = work32 - b->sigl;
      if ( accum3 > work32 )
	accum64 --;
      rat2 ++;
      if ( rat2 == 0 )
	rat1 ++;
      accum64 -= b->sigh;

#ifdef PARANOID
      if ( (accum64 >> 32) )
	{
	  EXCEPTION(EX_INTERNAL|0x203);
	}
#endif
    }

  /* Tidy up the remainder */

  accum64 <<= 32;
  accum64 |= accum3;
  if ( accum64 >= divr64 )
    {
      accum64 -= divr64;
      rat2 ++;
      if ( rat2 == 0 )
	{
	  rat1 ++;
#ifdef PARANOID
	  /* No overflow should be possible here */
	  if ( rat1 == 0 )
	    {
	      EXCEPTION(EX_INTERNAL|0x203);
	    }
	}
#endif
    }

  /* The basic division is done, now we must be careful with the
     remainder. */

  if ( ovfl )
    {
      if ( rat2 & 1 )
	rem = 0x80000000;
      else
	rem = 0;
      rat2 >>= 1;
      if ( rat1 & 1 )
	rat2 |= 0x80000000;
      rat1 >>= 1;
      rat1 |= 0x80000000;

      if ( accum64 )
	rem |= 0xff0000;

      dest->exp ++;
    }
  else
    {
      /* Now we just need to know how large the remainder is
	 relative to half the divisor. */
      if ( accum64 == 0 )
	rem = 0;
      else
	{
	  accum3 = accum64 >> 32;
	  if ( accum3 & 0x80000000 )
	    {
	      /* The remainder is definitely larger than 1/2 divisor. */
	      rem = 0xff000000;
	    }
	  else
	    {
	      accum64 <<= 1;
	      if ( accum64 >= divr64 )
		{
		  accum64 -= divr64;
		  if ( accum64 == 0 )
		    rem = 0x80000000;
		  else
		    rem = 0xff000000;
		}
	      else
		rem = 0x7f000000;
	    }
	}
    }

  dest->sigh = rat1;
  dest->sigl = rat2;

  dest->exp --;
  return FPU_round(dest, rem, 0, control_w, sign);

}

