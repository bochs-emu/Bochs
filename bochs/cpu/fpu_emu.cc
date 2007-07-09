/////////////////////////////////////////////////////////////////////////
// $Id: fpu_emu.cc,v 1.4 2007-07-09 15:16:11 sshwarts Exp $
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


#define LOG_THIS BX_CPU_THIS_PTR
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

/* 9B */
void BX_CPU_C::FWAIT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  if (BX_CPU_THIS_PTR cr0.get_TS() && BX_CPU_THIS_PTR cr0.get_MP())
    exception(BX_NM_EXCEPTION, 0, 0);

  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
#else
  BX_INFO(("FWAIT: requred FPU, use --enable-fpu"));
#endif
}

/* relevant only when FPU support is disabled */
#if BX_SUPPORT_FPU == 0
void BX_CPU_C::FPU_ESC(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);
}
#endif
