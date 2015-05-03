/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

// LES/LDS can't be called from long64 mode
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LES_GwMp(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = read_virtual_word_32(i->seg(), eaddr);
  Bit16u es     = read_virtual_word_32(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

// LES/LDS can't be called from long64 mode
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LES_GdMp(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u es = read_virtual_word_32(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = read_virtual_dword_32(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

// LES/LDS can't be called from long64 mode
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LDS_GwMp(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = read_virtual_word_32(i->seg(), eaddr);
  Bit16u ds     = read_virtual_word_32(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

// LES/LDS can't be called from long64 mode
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LDS_GdMp(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u ds = read_virtual_word_32(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = read_virtual_dword_32(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LFS_GwMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = read_virtual_word(i->seg(), eaddr);
  Bit16u fs     = read_virtual_word(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LFS_GdMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u fs = read_virtual_word(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = read_virtual_dword(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LFS_GqMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u fs = read_linear_word(i->seg(), get_laddr64(i->seg(), (eaddr + 8) & i->asize_mask()));
  Bit64u reg_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_WRITE_64BIT_REG(i->dst(), reg_64);

  BX_NEXT_INSTR(i);
}
#endif

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LGS_GwMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = read_virtual_word(i->seg(), eaddr);
  Bit16u gs     = read_virtual_word(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LGS_GdMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u gs = read_virtual_word(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = read_virtual_dword(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LGS_GqMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u gs = read_linear_word(i->seg(), get_laddr64(i->seg(), (eaddr + 8) & i->asize_mask()));
  Bit64u reg_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_WRITE_64BIT_REG(i->dst(), reg_64);

  BX_NEXT_INSTR(i);
}
#endif

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LSS_GwMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u reg_16 = read_virtual_word(i->seg(), eaddr);
  Bit16u ss     = read_virtual_word(i->seg(), (eaddr + 2) & i->asize_mask());

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_16BIT_REG(i->dst(), reg_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LSS_GdMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u ss = read_virtual_word(i->seg(), (eaddr + 4) & i->asize_mask());
  Bit32u reg_32 = read_virtual_dword(i->seg(), eaddr);

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_32BIT_REGZ(i->dst(), reg_32);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LSS_GqMp(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u ss = read_linear_word(i->seg(), get_laddr64(i->seg(), (eaddr + 8) & i->asize_mask()));
  Bit64u reg_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_WRITE_64BIT_REG(i->dst(), reg_64);

  BX_NEXT_INSTR(i);
}
#endif
