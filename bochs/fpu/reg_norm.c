/*---------------------------------------------------------------------------+
 |  reg_norm.c                                                               |
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1995,1997,1999                               |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@melbpc.org.au              |
 |                                                                           |
 | Normalize the value in a FPU_REG.                                         |
 |                                                                           |
 |                                                                           |
 |    Return value is the tag of the answer.                                 |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "fpu_emu.h"



int FPU_normalize_nuo(FPU_REG *x, int bias)
{

  if ( ! (x->sigh & 0x80000000) )
    {
      if ( x->sigh == 0 )
	{
	  if ( x->sigl == 0 )
	    {
	      x->exp = EXP_UNDER;
	      return TAG_Zero;
	    }
	  x->sigh = x->sigl;
	  x->sigl = 0;
	  x->exp -= 32;
	}
      while ( !(x->sigh & 0x80000000) )
	{
	  x->sigh <<= 1;
	  if ( x->sigl & 0x80000000 )
	    x->sigh |= 1;
	  x->sigl <<= 1;
	  x->exp --;
	}
    }

  x->exp += bias;

  return TAG_Valid;
}
