/*---------------------------------------------------------------------------+
 |  wm_shrx.c                                                                |
 |  $Id: wm_shrx.c,v 1.2 2001-10-06 03:53:46 bdenney Exp $
 |                                                                           |
 | 64 bit right shift functions                                              |
 |                                                                           |
 | Copyright (C) 1992,1995,1999                                              |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@melbpc.org.au              |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
 |   unsigned FPU_shrx(void *arg1, unsigned arg2)                            |
 |                                                                           |
 |   Extended shift right function.                                          |
 |   Fastest for small shifts.                                               |
 |   Shifts the 64 bit quantity pointed to by the first arg (arg1)           |
 |   right by the number of bits specified by the second arg (arg2).         |
 |   Forms a 96 bit quantity from the 64 bit arg and eax:                    |
 |                [  64 bit arg ][ eax ]                                     |
 |            shift right  --------->                                        |
 |   The eax register is initialized to 0 before the shifting.               |
 |   Results returned in the 64 bit arg and eax.                             |
 +---------------------------------------------------------------------------*/


#include "fpu_emu.h"

unsigned FPU_shrx(void *arg1, u32 arg2)
{
  u32 x;

  if ( arg2 >= 64 )
    {
      if ( arg2 >= 96 )
	{
	  *(u64 *)arg1 = 0;
	  return 0;
	}
      arg2 -= 64;
      x = (*(u64 *)arg1) >> 32;
      *(u64 *)arg1 = 0;

      if ( arg2 )
	return x >> arg2;
      else
	return x;
    }

  if ( arg2 < 32 )
    {
      if ( arg2 == 0 )
	return 0;

      x = (*(u64 *)arg1) << (32 - arg2);
    }
  else if ( arg2 > 32 )
    {
      x = (*(u64 *)arg1) >> (arg2 - 32);
    }
  else
    {
      /* arg2 == 32 */
      x = *(u64 *)arg1;
    }

  (*(u64 *)arg1) >>= arg2;

  return x;

}


/*---------------------------------------------------------------------------+
 |   unsigned FPU_shrxs(void *arg1, unsigned arg2)                           |
 |                                                                           |
 |   Extended shift right function (optimized for small floating point       |
 |   integers).                                                              |
 |   Shifts the 64 bit quantity pointed to by the first arg (arg1)           |
 |   right by the number of bits specified by the second arg (arg2).         |
 |   Forms a 96 bit quantity from the 64 bit arg and eax:                    |
 |                [  64 bit arg ][ eax ]                                     |
 |            shift right  --------->                                        |
 |   The eax register is initialized to 0 before the shifting.               |
 |   The lower 8 bits of eax are lost and replaced by a flag which is        |
 |   set (to 0x01) if any bit, apart from the first one, is set in the       |
 |   part which has been shifted out of the arg.                             |
 |   Results returned in the 64 bit arg and eax.                             |
 +---------------------------------------------------------------------------*/

unsigned FPU_shrxs(void *arg1, u32 arg2)
{
  u32 x, bits;
  u64 lost;

  if ( arg2 >= 64 )
    {
      if ( arg2 >= 96 )
	{
	  bits = *(u64 *)arg1 != 0;
	  *(u64 *)arg1 = 0;
	  return bits ? 1 : 0;
	}
      arg2 -= 64;
      lost = (*(u64 *)arg1) << (32 - arg2);
      x = (*(u64 *)arg1) >> 32;
      *(u64 *)arg1 = 0;

      if ( arg2 )
	x >>= arg2;

      if ( lost )
	x |= 1;

      return x;
    }

  if ( arg2 < 32 )
    {
      if ( arg2 == 0 )
	/* No bits are lost */
	return 0;

      /* No bits are lost */
      x = (*(u64 *)arg1) << (32 - arg2);
    }
  else if ( arg2 > 32 )
    {
      bits = (*(u64 *)arg1);
      bits <<= (64 - arg2);
      x = (*(u64 *)arg1) >> (arg2 - 32);
      if ( bits )
	x |= 1;
    }
  else
    {
      /* arg2 == 32 */
      /* No bits are lost */
      x = *(u64 *)arg1;
    }

  (*(u64 *)arg1) >>= arg2;

  if ( x & 0x7fffffff )
    x |= 1;

  return x;

}

