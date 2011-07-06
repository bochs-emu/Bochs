/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2004-2011 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

/* 9B */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FWAIT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  if (BX_CPU_THIS_PTR cr0.get_TS() && BX_CPU_THIS_PTR cr0.get_MP())
    exception(BX_NM_EXCEPTION, 0);

  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BX_NEXT_INSTR(i);
#endif
}

/* relevant only when FPU support is disabled */
#if BX_SUPPORT_FPU == 0
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FPU_ESC(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  BX_NEXT_INSTR(i);
}
#endif
