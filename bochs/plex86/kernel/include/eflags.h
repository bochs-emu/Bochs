/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  eflags.h: Bitfields of EFLAGS registers
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef __EFLAGS_H__
#define __EFLAGS_H__

/*
 * the eflags field looks like this:
 * bit:  0  1 2  3 4  5 6  7  8  9  A  B  C/D  E  F 10 11 12 13  14  15 16
 * flg:  CF 1 PF 0 AF 0 ZF SF TF IF DF OF IOPL NT 0 RF VM AC VIF VIP ID 0
 */

#define FLG_CF    (1<<0)
#define FLG_PF    (1<<2)
#define FLG_AF    (1<<4)
#define FLG_ZF    (1<<6)
#define FLG_SF    (1<<7)
#define FLG_TF    (1<<8)
#define FLG_IF    (1<<9)
#define FLG_DF    (1<<10)
#define FLG_OF    (1<<11)
#define FLG_IOPL  (3<<12)
#define FLG_NT    (1<<14)
#define FLG_RF    (1<<16)
#define FLG_VM    (1<<17)
#define FLG_AC    (1<<18)
#define FLG_VIF   (1<<19)
#define FLG_VIP   (1<<20)
#define FLG_ID    (1<<21)


typedef union {
  struct {
    Bit8u cf:1;
    Bit8u R1:1;
    Bit8u pf:1;
    Bit8u R3:1;
    Bit8u af:1;
    Bit8u R5:1;
    Bit8u zf:1;
    Bit8u sf:1;
    Bit8u tf:1;
    Bit8u if_:1;
    Bit8u df:1;
    Bit8u of:1;
    Bit8u iopl:2;
    Bit8u nt:1;
    Bit8u R15:1;
    Bit8u rf:1;
    Bit8u vm:1;
    Bit8u ac:1;
    Bit8u vif:1;
    Bit8u vip:1;
    Bit8u id:1;
    Bit16u R31_22:10;
    } __attribute__ ((packed)) fields;
  Bit32u raw;
  } __attribute__ ((packed)) eflags_t;

#endif  /* __EFLAGS_H__ */
