/////////////////////////////////////////////////////////////////////////
// $Id: protect_ctrl_pro.cc,v 1.6 2001-10-03 13:10:37 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
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







#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR




  void
BX_CPU_C::enter_protected_mode(void)
{
// BX_INFO(("processor switching into PROTECTED mode!!!"));
// debug(BX_CPU_THIS_PTR prev_eip);
  if (v8086_mode()) BX_PANIC(("protect_ctrl: v8086 mode unsupported"));

  if (bx_dbg.reset)
    BX_INFO(("processor switching into PROTECTED mode!!!"));

if ( BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl!=0 || BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl!=0 )
  BX_PANIC(("enter_protected_mode: CS or SS rpl != 0"));
}


  void
BX_CPU_C::enter_real_mode(void)
{
// ???
// BX_INFO(("processor switching into REAL mode!!!"));
// debug(BX_CPU_THIS_PTR prev_eip);
  if (v8086_mode()) BX_PANIC(("protect_ctrl: v8086 mode unsupported"));

  if (bx_dbg.reset)
    BX_INFO(("processor switching into REAL mode!!!"));

if ( BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl!=0 || BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.rpl!=0 )
  BX_PANIC(("enter_real_mode: CS or SS rpl != 0"));
}
