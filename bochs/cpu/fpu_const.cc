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
#include "softfloat-specialize.h"

const floatx80 Const_QNaN = { floatx80_default_nan_exp, floatx80_default_nan_fraction };
#endif

void BX_CPU_C::FLDL2T(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
#else
  BX_INFO(("FLDL2T: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDL2E(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
#else
  BX_INFO(("FLDL2E: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDPI(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
#else
  BX_INFO(("FLDPI: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDLG2(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
#else
  BX_INFO(("FLDLG2: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDLN2(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
#else
  BX_INFO(("FLDLN2: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLD1(bxInstruction_c *i) 
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
#else
  BX_INFO(("FLD1: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDZ(bxInstruction_c *i)                      
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  fpu_execute(i);
#else
  BX_INFO(("FLDZ: required FPU, configure --enable-fpu"));
#endif
}
