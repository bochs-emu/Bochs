/*---------------------------------------------------------------------------+
 |  div_small.S                                                              |
 |  $Id: div_small.c,v 1.1.10.1 2002-03-17 08:51:20 bdenney Exp $
 |                                                                           |
 | Divide a 64 bit integer by a 32 bit integer & return remainder.           |
 |                                                                           |
 | Copyright (C) 1992,1995,1999                                              |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@melbpc.org.au              |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/


#include "fpu_emu.h"

u32 FPU_div_small(u64 *x, u32 y)
{
  u32 retval;

  retval = *x % y;

  *x /= y;

  return retval;
}

