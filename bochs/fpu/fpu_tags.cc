/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2009 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#include "config.h"

#if BX_SUPPORT_FPU

#include "softfloat.h"
#include "softfloat-specialize.h"

/* -----------------------------------------------------------
 * Slimmed down version used to compile against a CPU simulator
 * rather than a kernel (ported by Kevin Lawton)
 * ------------------------------------------------------------ */

#include <cpu/i387.h>

int FPU_tagof(const floatx80 &reg)
{
   Bit32s exp = floatx80_exp(reg);
   if (exp == 0)
   {
      if (! floatx80_fraction(reg))
          return FPU_Tag_Zero;

      /* The number is a de-normal or pseudodenormal. */
      return FPU_Tag_Special;
   }

   if (exp == 0x7fff)
   {
      /* Is an Infinity, a NaN, or an unsupported data type. */
      return FPU_Tag_Special;
   }

   if (!(reg.fraction & BX_CONST64(0x8000000000000000)))
   {
      /* Unsupported data type. */
      /* Valid numbers have the ms bit set to 1. */
      return FPU_Tag_Special;
   }

   return FPU_Tag_Valid;
}

#endif
