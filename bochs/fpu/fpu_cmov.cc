/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2012 Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_FPU

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCMOVB_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src())) {
     FPU_stack_underflow(0);
  }
  else {
     if (get_CF())
        BX_WRITE_FPU_REG(BX_READ_FPU_REG(i->src()), 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCMOVBE_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src())) {
     FPU_stack_underflow(0);
  }
  else {
     if (get_CF() || get_ZF())
        BX_WRITE_FPU_REG(BX_READ_FPU_REG(i->src()), 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCMOVE_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src())) {
     FPU_stack_underflow(0);
  }
  else {
     if (get_ZF())
        BX_WRITE_FPU_REG(BX_READ_FPU_REG(i->src()), 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCMOVNB_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src())) {
     FPU_stack_underflow(0);
  }
  else {
     if (! get_CF())
        BX_WRITE_FPU_REG(BX_READ_FPU_REG(i->src()), 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCMOVNBE_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src())) {
     FPU_stack_underflow(0);
  }
  else {
     if (! get_CF() && ! get_ZF())
        BX_WRITE_FPU_REG(BX_READ_FPU_REG(i->src()), 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCMOVNE_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src())) {
     FPU_stack_underflow(0);
  }
  else {
     if (! get_ZF())
        BX_WRITE_FPU_REG(BX_READ_FPU_REG(i->src()), 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCMOVNU_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src())) {
     FPU_stack_underflow(0);
  }
  else {
     if (! get_PF())
        BX_WRITE_FPU_REG(BX_READ_FPU_REG(i->src()), 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FCMOVU_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR prepareFPU(i);
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src())) {
     FPU_stack_underflow(0);
  }
  else {
     if (get_PF())
        BX_WRITE_FPU_REG(BX_READ_FPU_REG(i->src()), 0);
  }

  BX_NEXT_INSTR(i);
}

#endif
