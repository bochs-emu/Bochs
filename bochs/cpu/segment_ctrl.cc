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









#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR






  void
BX_CPU_C::LES_GvMp(BxInstruction_t *i)
{
  if (i->mod == 0xc0) {
    // (BW) NT seems to use this when booting.
    BX_INFO(("invalid use of LES, must use memory reference!\n"));
    UndefinedOpcode(i);
    }

#if BX_CPU_LEVEL > 2
  if (i->os_32) {
    Bit16u es;
    Bit32u reg_32;

    read_virtual_dword(i->seg, i->rm_addr, &reg_32);
    read_virtual_word(i->seg, i->rm_addr + 4, &es);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

    BX_WRITE_32BIT_REG(i->nnn, reg_32);
    }
  else
#endif /* BX_CPU_LEVEL > 2 */
    { /* 16 bit mode */
    Bit16u reg_16, es;

    read_virtual_word(i->seg, i->rm_addr, &reg_16);
    read_virtual_word(i->seg, i->rm_addr + 2, &es);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

    BX_WRITE_16BIT_REG(i->nnn, reg_16);
    }
}

  void
BX_CPU_C::LDS_GvMp(BxInstruction_t *i)
{
  if (i->mod == 0xc0) {
    BX_PANIC(("invalid use of LDS, must use memory reference!\n"));
    UndefinedOpcode(i);
    }

#if BX_CPU_LEVEL > 2
  if (i->os_32) {
    Bit16u ds;
    Bit32u reg_32;

    read_virtual_dword(i->seg, i->rm_addr, &reg_32);
    read_virtual_word(i->seg, i->rm_addr + 4, &ds);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

    BX_WRITE_32BIT_REG(i->nnn, reg_32);
    }
  else
#endif /* BX_CPU_LEVEL > 2 */
    { /* 16 bit mode */
    Bit16u reg_16, ds;

    read_virtual_word(i->seg, i->rm_addr, &reg_16);
    read_virtual_word(i->seg, i->rm_addr + 2, &ds);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

    BX_WRITE_16BIT_REG(i->nnn, reg_16);
    }
}

  void
BX_CPU_C::LFS_GvMp(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("lfs_gvmp: not supported on 8086\n"));
#else /* 386+ */

  if (i->mod == 0xc0) {
    BX_PANIC(("invalid use of LFS, must use memory reference!\n"));
    UndefinedOpcode(i);
    }

  if (i->os_32) {
    Bit32u reg_32;
    Bit16u fs;

    read_virtual_dword(i->seg, i->rm_addr, &reg_32);
    read_virtual_word(i->seg, i->rm_addr + 4, &fs);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

    BX_WRITE_32BIT_REG(i->nnn, reg_32);
    }
  else { /* 16 bit operand size */
    Bit16u reg_16;
    Bit16u fs;

    read_virtual_word(i->seg, i->rm_addr, &reg_16);
    read_virtual_word(i->seg, i->rm_addr + 2, &fs);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

    BX_WRITE_16BIT_REG(i->nnn, reg_16);
    }
#endif
}

  void
BX_CPU_C::LGS_GvMp(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("lgs_gvmp: not supported on 8086\n"));
#else /* 386+ */

  if (i->mod == 0xc0) {
    BX_PANIC(("invalid use of LGS, must use memory reference!\n"));
    UndefinedOpcode(i);
    }

  if (i->os_32) {
    Bit32u reg_32;
    Bit16u gs;

    read_virtual_dword(i->seg, i->rm_addr, &reg_32);
    read_virtual_word(i->seg, i->rm_addr + 4, &gs);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

    BX_WRITE_32BIT_REG(i->nnn, reg_32);
    }
  else { /* 16 bit operand size */
    Bit16u reg_16;
    Bit16u gs;

    read_virtual_word(i->seg, i->rm_addr, &reg_16);
    read_virtual_word(i->seg, i->rm_addr + 2, &gs);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

    BX_WRITE_16BIT_REG(i->nnn, reg_16);
    }
#endif
}

  void
BX_CPU_C::LSS_GvMp(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("lss_gvmp: not supported on 8086\n"));
#else /* 386+ */

  if (i->mod == 0xc0) {
    BX_PANIC(("invalid use of LSS, must use memory reference!\n"));
    UndefinedOpcode(i);
    }

  if (i->os_32) {
    Bit32u reg_32;
    Bit16u ss_raw;

    read_virtual_dword(i->seg, i->rm_addr, &reg_32);
    read_virtual_word(i->seg, i->rm_addr + 4, &ss_raw);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss_raw);

    BX_WRITE_32BIT_REG(i->nnn, reg_32);
    }
  else { /* 16 bit operand size */
    Bit16u reg_16;
    Bit16u ss_raw;

    read_virtual_word(i->seg, i->rm_addr, &reg_16);
    read_virtual_word(i->seg, i->rm_addr + 2, &ss_raw);

    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss_raw);

    BX_WRITE_16BIT_REG(i->nnn, reg_16);
    }
#endif
}
