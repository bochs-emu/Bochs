/////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2004  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_FPU
static floatx80& floatx80_abs(floatx80 &reg)
{
    reg.exp &= 0x8000;
    return reg;   
}

static floatx80& floatx80_chs(floatx80 &reg)
{
    reg.exp ^= 0x8000;
    return reg;   
}
#endif 

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
      if(BX_CPU_THIS_PTR the_i387.get_control_word() & FPU_CW_Invalid)
      {
	  /* Masked response */
          if (st0_tag == FPU_Tag_Empty)
          {
              st0_reg = Const_QNaN;
              st0_tag = FPU_Tag_Special;
          }
          if (sti_tag == FPU_Tag_Empty)
          {
              sti_reg = Const_QNaN;
              sti_tag = FPU_Tag_Special;
          }
      }
      else
      {
          BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);
          return;
      }
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
      FPU_stack_underflow(0);
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
      FPU_stack_underflow(0);
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

  if (BX_CPU_THIS_PTR the_i387.tos == 0)
  {
    BX_CPU_THIS_PTR the_i387.tos = 7;
  }
  else
  {
    BX_CPU_THIS_PTR the_i387.tos --;
  }

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

  BX_CPU_THIS_PTR the_i387.tos++;
  BX_CPU_THIS_PTR the_i387.tos &= 0x07;
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
