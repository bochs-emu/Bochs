/////////////////////////////////////////////////////////////////////////
// $Id: segment_ctrl.cc,v 1.15 2007-04-09 20:28:15 sshwarts Exp $
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
  if (i->modC0()) {
    BX_DEBUG(("LES_GwMp: invalid use of LES, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit16u reg_16, es;

  read_virtual_word(i->seg(), RMAddr(i), &reg_16);
  read_virtual_word(i->seg(), RMAddr(i) + 2, &es);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LES_GdMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LES_GdMp: invalid use of LES, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit16u es;
  Bit32u reg_32;

  read_virtual_dword(i->seg(), RMAddr(i), &reg_32);
  read_virtual_word(i->seg(), RMAddr(i) + 4, &es);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

void BX_CPU_C::LDS_GwMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LDS_GwMp: invalid use of LDS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit16u reg_16, ds;

  read_virtual_word(i->seg(), RMAddr(i), &reg_16);
  read_virtual_word(i->seg(), RMAddr(i) + 2, &ds);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LDS_GdMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LDS_GdMp: invalid use of LDS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit16u ds;
  Bit32u reg_32;

  read_virtual_dword(i->seg(), RMAddr(i), &reg_32);
  read_virtual_word(i->seg(), RMAddr(i) + 4, &ds);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

void BX_CPU_C::LFS_GwMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LFS_GwMp: invalid use of LFS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit16u reg_16, fs;

  read_virtual_word(i->seg(), RMAddr(i), &reg_16);
  read_virtual_word(i->seg(), RMAddr(i) + 2, &fs);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LFS_GdMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LFS_GdMp: invalid use of LFS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit32u reg_32;
  Bit16u fs;

  read_virtual_dword(i->seg(), RMAddr(i), &reg_32);
  read_virtual_word(i->seg(), RMAddr(i) + 4, &fs);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::LFS_GqMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LFS_GqMp: invalid use of LFS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit64u reg_64;
  Bit16u fs;

  read_virtual_qword(i->seg(), RMAddr(i), &reg_64);
  read_virtual_word(i->seg(), RMAddr(i) + 8, &fs);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif

void BX_CPU_C::LGS_GwMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LGS_GwMp: invalid use of LGS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit16u reg_16, gs;

  read_virtual_word(i->seg(), RMAddr(i), &reg_16);
  read_virtual_word(i->seg(), RMAddr(i) + 2, &gs);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LGS_GdMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LGS_GdMp: invalid use of LGS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit32u reg_32;
  Bit16u gs;

  read_virtual_dword(i->seg(), RMAddr(i), &reg_32);
  read_virtual_word(i->seg(), RMAddr(i) + 4, &gs);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::LGS_GqMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LGS_GqMp: invalid use of LGS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit64u reg_64;
  Bit16u gs;

  read_virtual_qword(i->seg(), RMAddr(i), &reg_64);
  read_virtual_word(i->seg(), RMAddr(i) + 8, &gs);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif

void BX_CPU_C::LSS_GwMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LSS_GwMp: invalid use of LSS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit16u reg_16, ss;

  read_virtual_word(i->seg(), RMAddr(i), &reg_16);
  read_virtual_word(i->seg(), RMAddr(i) + 2, &ss);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPU_C::LSS_GdMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LSS_GdMp: invalid use of LSS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit32u reg_32;
  Bit16u ss;

  read_virtual_dword(i->seg(), RMAddr(i), &reg_32);
  read_virtual_word(i->seg(), RMAddr(i) + 4, &ss);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::LSS_GqMp(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_DEBUG(("LSS_GqMp: invalid use of LSS, must be memory reference!"));
    UndefinedOpcode(i);
  }

  Bit64u reg_64;
  Bit16u ss;

  read_virtual_qword(i->seg(), RMAddr(i), &reg_64);
  read_virtual_word(i->seg(), RMAddr(i) + 8, &ss);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif
