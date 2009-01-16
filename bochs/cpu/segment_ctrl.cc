/////////////////////////////////////////////////////////////////////////
// $Id: segment_ctrl.cc,v 1.22 2009-01-16 18:18:58 sshwarts Exp $
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LES_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, es;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_16 = read_virtual_word(i->seg(), eaddr);
  es     = read_virtual_word(i->seg(), eaddr + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LES_GdMp(bxInstruction_c *i)
{
  Bit16u es;
  Bit32u reg_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_32 = read_virtual_dword(i->seg(), eaddr);
  es     = read_virtual_word (i->seg(), eaddr + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LDS_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, ds;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_16 = read_virtual_word(i->seg(), eaddr);
  ds     = read_virtual_word(i->seg(), eaddr + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LDS_GdMp(bxInstruction_c *i)
{
  Bit16u ds;
  Bit32u reg_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_32 = read_virtual_dword(i->seg(), eaddr);
  ds     = read_virtual_word (i->seg(), eaddr + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LFS_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, fs;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_16 = read_virtual_word(i->seg(), eaddr);
  fs     = read_virtual_word(i->seg(), eaddr + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LFS_GdMp(bxInstruction_c *i)
{
  Bit32u reg_32;
  Bit16u fs;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_32 = read_virtual_dword(i->seg(), eaddr);
  fs     = read_virtual_word (i->seg(), eaddr + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LFS_GqMp(bxInstruction_c *i)
{
  Bit64u reg_64;
  Bit16u fs;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_64 = read_virtual_qword(i->seg(), eaddr);
  fs     = read_virtual_word (i->seg(), eaddr + 8);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LGS_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, gs;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_16 = read_virtual_word(i->seg(), eaddr);
  gs     = read_virtual_word(i->seg(), eaddr + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LGS_GdMp(bxInstruction_c *i)
{
  Bit32u reg_32;
  Bit16u gs;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_32 = read_virtual_dword(i->seg(), eaddr);
  gs     = read_virtual_word (i->seg(), eaddr + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LGS_GqMp(bxInstruction_c *i)
{
  Bit64u reg_64;
  Bit16u gs;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_64 = read_virtual_qword(i->seg(), eaddr);
  gs     = read_virtual_word (i->seg(), eaddr + 8);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LSS_GwMp(bxInstruction_c *i)
{
  Bit16u reg_16, ss;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_16 = read_virtual_word(i->seg(), eaddr);
  ss     = read_virtual_word(i->seg(), eaddr + 2);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_16BIT_REG(i->nnn(), reg_16);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LSS_GdMp(bxInstruction_c *i)
{
  Bit32u reg_32;
  Bit16u ss;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_32 = read_virtual_dword(i->seg(), eaddr);
  ss     = read_virtual_word (i->seg(), eaddr + 4);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_32BIT_REGZ(i->nnn(), reg_32);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LSS_GqMp(bxInstruction_c *i)
{
  Bit64u reg_64;
  Bit16u ss;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  reg_64 = read_virtual_qword(i->seg(), eaddr);
  ss     = read_virtual_word (i->seg(), eaddr + 8);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_64BIT_REG(i->nnn(), reg_64);
}
#endif
