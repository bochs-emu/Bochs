/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  paging.h: defines for x86 paging structures
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

#ifndef __PAGING_H__
#define __PAGING_H__

#define PG_D 0x00000040
#define PG_A 0x00000020

/* Page Directory/Table format */
typedef union {
  Bit32u raw;
  struct {
    Bit32u  P:1;
    Bit32u  RW:1;
    Bit32u  US:1;
    Bit32u  PWT:1;
    Bit32u  PCD:1;
    Bit32u  A:1;
    Bit32u  D:1;
    Bit32u  PS:1;
    Bit32u  G:1;
    Bit32u  avail:3;
    Bit32u  base:20;
    } __attribute__ ((packed)) fields;
  } __attribute__ ((packed)) pageEntry_t;

typedef union {
  Bit8u       bytes[4096];
  pageEntry_t pte[1024];
  } page_t;

#endif  /* __PAGING_H__ */
