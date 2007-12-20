/////////////////////////////////////////////////////////////////////////
// $Id: segment_ctrl.cc,v 1.17 2007-12-20 20:58:37 sshwarts Exp $
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
//
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


void BX_CPU_C::LES_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, es;

  reg_16 = read_virtual_word(i->seg(), RMAddr(i));
  es     = read_virtual_word(i->seg(), RMAddr(i) + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LES_GdMp(bxInstruction_c *i)
{
  Bit16u es;
  Bit32u reg_32;

  reg_32 = read_virtual_dword(i->seg(), RMAddr(i));
  es     = read_virtual_word (i->seg(), RMAddr(i) + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

void BX_CPU_C::LDS_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, ds;

  reg_16 = read_virtual_word(i->seg(), RMAddr(i));
  ds     = read_virtual_word(i->seg(), RMAddr(i) + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LDS_GdMp(bxInstruction_c *i)
{
  Bit16u ds;
  Bit32u reg_32;

  reg_32 = read_virtual_dword(i->seg(), RMAddr(i));
  ds     = read_virtual_word (i->seg(), RMAddr(i) + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

void BX_CPU_C::LFS_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, fs;

  reg_16 = read_virtual_word(i->seg(), RMAddr(i));
  fs     = read_virtual_word(i->seg(), RMAddr(i) + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LFS_GdMp(bxInstruction_c *i)
{
  Bit32u reg_32;
  Bit16u fs;

  reg_32 = read_virtual_dword(i->seg(), RMAddr(i));
  fs     = read_virtual_word (i->seg(), RMAddr(i) + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::LFS_GqMp(bxInstruction_c *i)
{
  Bit64u reg_64;
  Bit16u fs;

  reg_64 = read_virtual_qword(i->seg(), RMAddr(i));
  fs     = read_virtual_word (i->seg(), RMAddr(i) + 8);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif

void BX_CPU_C::LGS_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, gs;

  reg_16 = read_virtual_word(i->seg(), RMAddr(i));
  gs     = read_virtual_word(i->seg(), RMAddr(i) + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LGS_GdMp(bxInstruction_c *i)
{
  Bit32u reg_32;
  Bit16u gs;

  reg_32 = read_virtual_dword(i->seg(), RMAddr(i));
  gs     = read_virtual_word (i->seg(), RMAddr(i) + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::LGS_GqMp(bxInstruction_c *i)
{
  Bit64u reg_64;
  Bit16u gs;

  reg_64 = read_virtual_qword(i->seg(), RMAddr(i));
  gs     = read_virtual_word (i->seg(), RMAddr(i) + 8);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif

void BX_CPU_C::LSS_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, ss;

  reg_16 = read_virtual_word(i->seg(), RMAddr(i));
  ss     = read_virtual_word(i->seg(), RMAddr(i) + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LSS_GdMp(bxInstruction_c *i)
{
  Bit32u reg_32;
  Bit16u ss;

  reg_32 = read_virtual_dword(i->seg(), RMAddr(i));
  ss     = read_virtual_word (i->seg(), RMAddr(i) + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::LSS_GqMp(bxInstruction_c *i)
{
  Bit64u reg_64;
  Bit16u ss;

  reg_64 = read_virtual_qword(i->seg(), RMAddr(i));
  ss     = read_virtual_word (i->seg(), RMAddr(i) + 8);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif
