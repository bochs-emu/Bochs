/*---------------------------------------------------------------------------+
 |  poly_tan.c                                                               |
 |                                                                           |
 | Compute the tan of a FPU_REG, using a polynomial approximation.           |
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1997,1999                                    |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail   billm@melbpc.org.au            |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "exception.h"
#include "reg_constant.h"
#include "fpu_emu.h"
#include "fpu_system.h"
#include "control_w.h"
#include "poly.h"

//#define DEBUG_POLY_TAN // ***********

#define	HiPOWERop	3	/* odd poly, positive terms */
static const u64 oddplterm[HiPOWERop] =
{
  0x0000000000000000LL,
  0x0051a1cf08fca228LL,
  0x0000000071284ff7LL
};

#define	HiPOWERon	2	/* odd poly, negative terms */
static const u64 oddnegterm[HiPOWERon] =
{
   0x1291a9a184244e80LL,
   0x0000583245819c21LL
};

#define	HiPOWERep	2	/* even poly, positive terms */
static const u64 evenplterm[HiPOWERep] =
{
  0x0e848884b539e888LL,
  0x00003c7f18b887daLL
};

#define	HiPOWERen	2	/* even poly, negative terms */
static const u64 evennegterm[HiPOWERen] =
{
  0xf1f0200fd51569ccLL,
  0x003afb46105c4432LL
};

static const u64 twothirds = 0xaaaaaaaaaaaaaaabLL;


/*--- poly_tan() ------------------------------------------------------------+
 |                                                                           |
 +---------------------------------------------------------------------------*/
void   poly_tan(FPU_REG *st0_ptr, int invert)
{
  s32         exponent;
  Xsig        argSq, argSqSq, accumulatoro, accumulatore, accum,
              argSignif;

  exponent = exponent(st0_ptr);


#ifdef PARANOID
  if ( signnegative(st0_ptr) )	/* Can't hack a number < 0.0 */
    { arith_invalid(0); return; }  /* Need a positive number */
#endif /* PARANOID */

  if ( (exponent >= 0)
       || ((exponent == -1) && (st0_ptr->sigh > 0xc90fdaa2)) )
    {
    EXCEPTION(0x250);
    }
  else
    {
      argSignif.lsw = 0;
      XSIG_LL(accum) = XSIG_LL(argSignif) = significand(st0_ptr);
 
      if ( exponent < -1 )
	{
	  /* shift the argument right by the required places */
	  if ( FPU_shrx(&XSIG_LL(accum), -1-exponent) >= 0x80000000U )
	    XSIG_LL(accum) ++;	/* round up */
	}
    }

  XSIG_LL(argSq) = XSIG_LL(accum); argSq.lsw = accum.lsw;
  mul_Xsig_Xsig(&argSq, &argSq);
  XSIG_LL(argSqSq) = XSIG_LL(argSq); argSqSq.lsw = argSq.lsw;
  mul_Xsig_Xsig(&argSqSq, &argSqSq);

  /* Compute the negative terms for the numerator polynomial */
  accumulatoro.msw = accumulatoro.midw = accumulatoro.lsw = 0;
  polynomial_Xsig(&accumulatoro, &XSIG_LL(argSqSq), oddnegterm, HiPOWERon-1);
  mul_Xsig_Xsig(&accumulatoro, &argSq);
  negate_Xsig(&accumulatoro);
  /* Add the positive terms */
  polynomial_Xsig(&accumulatoro, &XSIG_LL(argSqSq), oddplterm, HiPOWERop-1);

  
  /* Compute the positive terms for the denominator polynomial */
  accumulatore.msw = accumulatore.midw = accumulatore.lsw = 0;
  polynomial_Xsig(&accumulatore, &XSIG_LL(argSqSq), evenplterm, HiPOWERep-1);
  mul_Xsig_Xsig(&accumulatore, &argSq);
  negate_Xsig(&accumulatore);
  /* Add the negative terms */
  polynomial_Xsig(&accumulatore, &XSIG_LL(argSqSq), evennegterm, HiPOWERen-1);
  /* Multiply by arg^2 */
  mul64_Xsig(&accumulatore, &XSIG_LL(argSignif));
  mul64_Xsig(&accumulatore, &XSIG_LL(argSignif));
  /* de-normalize and divide by 2 */
  shr_Xsig(&accumulatore, -2*(1+exponent) + 1);
  negate_Xsig(&accumulatore);      /* This does 1 - accumulator */

  /* Now find the ratio. */
  if ( accumulatore.msw == 0 )
    {
      /* accumulatoro must contain 1.0 here, (actually, 0) but it
	 really doesn't matter what value we use because it will
	 have negligible effect in later calculations
	 */
      XSIG_LL(accum) = 0x8000000000000000LL;
      accum.lsw = 0;
    }
  else
    {
      div_Xsig(&accumulatoro, &accumulatore, &accum);
    }

  /* Multiply by 1/3 * arg^3 */
  mul64_Xsig(&accum, &XSIG_LL(argSignif));
  mul64_Xsig(&accum, &XSIG_LL(argSignif));
  mul64_Xsig(&accum, &XSIG_LL(argSignif));
  mul64_Xsig(&accum, &twothirds);
  shr_Xsig(&accum, -2*(exponent+1));


  /* tan(arg) = arg + accum */
  add_two_Xsig(&accum, &argSignif, &exponent);

  if ( invert )
    {
      /* accum now contains tan(pi/2 - arg).
	 Use tan(arg) = 1.0 / tan(pi/2 - arg)
	 */
      accumulatoro.lsw = accumulatoro.midw = 0;
      accumulatoro.msw = 0x80000000;
      div_Xsig(&accumulatoro, &accum, &accum);
      exponent = - exponent;
    }


  /* Transfer the result */
  exponent += round_Xsig(&accum);
  FPU_settag0(TAG_Valid);
  significand(st0_ptr) = XSIG_LL(accum);
  setexponent16(st0_ptr, exponent + EXTENDED_Ebias);  /* Result is positive. */

}
