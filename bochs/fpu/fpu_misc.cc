/////////////////////////////////////////////////////////////////////////
// $Id: fpu_misc.cc,v 1.9 2006-03-06 22:03:04 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <stl at fidonet.org.il>
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "softfloatx80.h"

/* D9 C8 */
void BX_CPU_C::FXCH_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int st0_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(0);
  int sti_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(i->rm());

  floatx80 st0_reg = BX_READ_FPU_REG(0);
  floatx80 sti_reg = BX_READ_FPU_REG(i->rm());

  clear_C1();

  if (st0_tag == FPU_Tag_Empty || sti_tag == FPU_Tag_Empty)
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
	  /* Masked response */
          if (st0_tag == FPU_Tag_Empty)
          {
              st0_reg = floatx80_default_nan;
              st0_tag = FPU_Tag_Special;
          }
          if (sti_tag == FPU_Tag_Empty)
          {
              sti_reg = floatx80_default_nan;
              sti_tag = FPU_Tag_Special;
          }
      }
      else return;
  }

  BX_WRITE_FPU_REGISTER_AND_TAG(st0_reg, st0_tag, i->rm());
  BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);
#else
  BX_INFO(("FXCH_STi: required FPU, configure --enable-fpu"));
#endif
}

/* D9 E0 */
void BX_CPU_C::FCHS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int st0_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(0);
  if (st0_tag == FPU_Tag_Empty)
  {
      BX_CPU_THIS_PTR FPU_stack_underflow(0);
      return;
  }

  clear_C1();

  floatx80 st0_reg = BX_READ_FPU_REG(0);
  BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_chs(st0_reg), st0_tag, 0);
#else
  BX_INFO(("FCHS: required FPU, configure --enable-fpu"));
#endif
}

/* D9 E1 */
void BX_CPU_C::FABS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int st0_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(0);
  if (st0_tag == FPU_Tag_Empty)
  {
      BX_CPU_THIS_PTR FPU_stack_underflow(0);
      return;
  }

  clear_C1();

  floatx80 st0_reg = BX_READ_FPU_REG(0);
  BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_abs(st0_reg), st0_tag, 0);
#else
  BX_INFO(("FABS: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F6 */
void BX_CPU_C::FDECSTP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  BX_CPU_THIS_PTR the_i387.tos = (BX_CPU_THIS_PTR the_i387.tos-1) & 7;
#else
  BX_INFO(("FDECSTP: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F7 */
void BX_CPU_C::FINCSTP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  BX_CPU_THIS_PTR the_i387.tos = (BX_CPU_THIS_PTR the_i387.tos+1) & 7;
#else
  BX_INFO(("FINCSTP: required FPU, configure --enable-fpu"));
#endif
}

/* DD C0 */
void BX_CPU_C::FFREE_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR the_i387.FPU_settagi(FPU_Tag_Empty, i->rm());
#else
  BX_INFO(("FFREE_STi: required FPU, configure --enable-fpu"));
#endif
}

/* 
 * Free the st(0) register and pop it from the FPU stack.
 * "Undocumented" by Intel & AMD but mentioned in AMDs Athlon Docs.
 */

/* DF C0 */
void BX_CPU_C::FFREEP_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR the_i387.FPU_settagi(FPU_Tag_Empty, i->rm());
  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FFREEP_STi: required FPU, configure --enable-fpu"));
#endif
}
