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
#include "softfloatx80.h"
#endif

extern "C"
{
  void fsin   (FPU_REG *st0_ptr, Bit8u st0_tag);
  void fcos   (FPU_REG *st0_ptr, Bit8u st0_tag);
  void fsincos(FPU_REG *st0_ptr, Bit8u st0_tag);
  void fptan  (FPU_REG *st0_ptr, Bit8u st0_tag);
  void fpatan (FPU_REG *st0_ptr, Bit8u st0_tag);
  void fyl2xp1(FPU_REG *st0_ptr, Bit8u st0_tag);
  void f2xm1  (FPU_REG *st0_ptr, Bit8u st0_tag);
  void fyl2x  (FPU_REG *st0_ptr, Bit8u st0_tag);
}

extern void FPU_initalize_i387(struct i387_t *the_i387);

void BX_CPU_C::FSIN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  FPU_initalize_i387((i387_t *)(&(BX_CPU_THIS_PTR the_i387)));

  fsin(&(BX_FPU_READ_ST0()), 
	BX_CPU_THIS_PTR the_i387.FPU_gettagi(0));
#else
  BX_INFO(("FSIN: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  FPU_initalize_i387((i387_t *)(&(BX_CPU_THIS_PTR the_i387)));

  fcos(&(BX_FPU_READ_ST0()), 
	BX_CPU_THIS_PTR the_i387.FPU_gettagi(0));
#else
  BX_INFO(("FCOS: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSINCOS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  FPU_initalize_i387((i387_t *)(&(BX_CPU_THIS_PTR the_i387)));

  fsincos(&(BX_FPU_READ_ST0()), 
	BX_CPU_THIS_PTR the_i387.FPU_gettagi(0));
#else
  BX_INFO(("FSINCOS: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FPTAN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  FPU_initalize_i387((i387_t *)(&(BX_CPU_THIS_PTR the_i387)));

  fptan(&(BX_FPU_READ_ST0()), 
	BX_CPU_THIS_PTR the_i387.FPU_gettagi(0));
#else
  BX_INFO(("FPTAN: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FPATAN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  FPU_initalize_i387((i387_t *)(&(BX_CPU_THIS_PTR the_i387)));

  fpatan(&(BX_FPU_READ_ST0()), 
	BX_CPU_THIS_PTR the_i387.FPU_gettagi(0));
#else
  BX_INFO(("FPATAN: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FYL2XP1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  FPU_initalize_i387((i387_t *)(&(BX_CPU_THIS_PTR the_i387)));

  fyl2xp1(&(BX_FPU_READ_ST0()), 
	BX_CPU_THIS_PTR the_i387.FPU_gettagi(0));
#else
  BX_INFO(("FYL2XP1: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::F2XM1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  FPU_initalize_i387((i387_t *)(&(BX_CPU_THIS_PTR the_i387)));

  f2xm1(&(BX_FPU_READ_ST0()), 
	BX_CPU_THIS_PTR the_i387.FPU_gettagi(0));
#else
  BX_INFO(("F2XM1: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FYL2X(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  FPU_initalize_i387((i387_t *)(&(BX_CPU_THIS_PTR the_i387)));

  fyl2x(&(BX_FPU_READ_ST0()), 
	BX_CPU_THIS_PTR the_i387.FPU_gettagi(0));
#else
  BX_INFO(("FYL2X: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F4 */
void BX_CPU_C::FXTRACT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1) || IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Overflow);

      /* The masked response */
      if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
          BX_CPU_THIS_PTR the_i387.FPU_push();
          BX_WRITE_FPU_REGISTER_AND_TAG(floatx80_default_nan, FPU_Tag_Special, 0);
      }

      return; 
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = floatx80_extract(a, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(b, 0);	// exponent
  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(a, 0);	// fraction
#else
  BX_INFO(("FXTRACT: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F5 */
void BX_CPU_C::FPREM1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  Bit64u quotient;

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(1);

  floatx80 result = floatx80_ieee754_remainder(a, b, quotient, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  int cc = 0;
  if (quotient == (Bit64u) -1) cc = FPU_SW_C2;
  else
  {
      if (quotient & 1) cc |= FPU_SW_C1;
      if (quotient & 2) cc |= FPU_SW_C3;
      if (quotient & 4) cc |= FPU_SW_C0;
  }
  SETCC(cc);

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FPREM1: required FPU, configure --enable-fpu"));
#endif
}

/* D9 F8 */
void BX_CPU_C::FPREM(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  Bit64u quotient;

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(1);

  floatx80 result = floatx80_remainder(a, b, quotient, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  int cc = 0;
  if (quotient == (Bit64u) -1) cc = FPU_SW_C2;
  else
  {
      if (quotient & 1) cc |= FPU_SW_C1;
      if (quotient & 2) cc |= FPU_SW_C3;
      if (quotient & 4) cc |= FPU_SW_C0;
  }
  SETCC(cc);

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FPREM: required FPU, configure --enable-fpu"));
#endif
}

/* D9 FD */
void BX_CPU_C::FSCALE(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_scale(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSCALE: required FPU, configure --enable-fpu"));
#endif
}
