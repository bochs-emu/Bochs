/************************************************************************
 * $Id: descriptor.h,v 1.1 2003-01-01 17:32:04 kevinlawton Exp $
 ************************************************************************
 *
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003  Kevin P. Lawton
 *
 *  descriptor.h: defines for descriptors and selectors
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


#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#define SET_DESCRIPTOR(d, B, L, G, DB, AVL, P, DPL, T) {\
    d.base_high = (B) >> 24;\
    d.base_med  = ((B) >> 16) & 0xff;\
    d.base_low  = (B) & 0xffff;\
    d.limit_high = ((L) & 0xf0000) >> 16;\
    d.limit_low  = ((L) & 0x0ffff);\
    d.g = (G);\
    d.d_b = (DB);\
    d.reserved = 0;\
    d.avl = (AVL);\
    d.p = (P);\
    d.dpl = (DPL);\
    d.type = (T);\
    }

typedef struct {
    Bit16u     limit_low;       /* limit 0..15 */
    Bit16u     base_low;        /* base  0..15 */
    Bit8u      base_med;        /* base  16..23 */
    unsigned   type:5;          /* S/type field */
    unsigned   dpl:2;           /* DPL */
    unsigned   p:1;             /* present */
    unsigned   limit_high:4;    /* limit 16..19 */
    unsigned   avl:1;           /* Available for use by system software */
    unsigned   reserved:1;      /* 0 */
    unsigned   d_b:1;           /* D/B */
    unsigned   g:1;             /* granularity */
    Bit8u      base_high;       /* base 24..31 */
    } __attribute__ ((packed)) descriptor_t;

typedef union {
  struct {
    Bit16u  rpl:2;
    Bit16u  ti:1;
    Bit16u  index:13;
    } __attribute__ ((packed)) fields;
  Bit16u raw;
  } __attribute__ ((packed)) selector_t;

#define D_DPL0 0
#define D_DPL1 1
#define D_DPL2 2
#define D_DPL3 3

#define D_BG 0
#define D_PG 1

#define D_D16 0
#define D_D32 1

#define D_NOTPRESENT 0
#define D_PRESENT    1

#define D_S        0x10
#define D_DATA     0x10
#define D_CODE     0x18
#define D_EXECUTE  0x08
#define D_EXDOWN   0x04
#define D_CONFORM  0x04
#define D_WRITE    0x02  /* writable (data segment) */
#define D_READ     0x02  /* readable (code segment) */
#define D_ACCESSED 0x01

#define D_LDT   0x02   /* LDT segment */
#define D_TASK  0x05   /* Task gate */
#define D_TSS   0x09   /* TSS */
#define D_CALL  0x0C  /* 386 call gate */
#define D_INT   0x0E  /* 386 interrupt gate */
#define D_TRAP  0x0F  /* 386 trap gate */

#define D_AVL0  0
#define D_AVL1  1

#define LimitOfDataDescriptor(d) \
  ( d.g ? \
      ( ((d.type & 0xc) == 0x4) ? \
          ((d.limit_high << 16) | d.limit_low)<<12 : \
          ((d.limit_high << 16) | d.limit_low)<<12 | 0xfff) : \
      ((d.limit_high << 16) | d.limit_low) )

#define BaseOfDescriptor(d) \
  ( (d.base_low) | (d.base_med << 16) | (d.base_high << 24) )

/*
 *  selector stuff
 */

#define Selector(sel, ti, rpl) (((sel)<<3) | ((ti)<<2) | (rpl))
#define RPL0 0
#define RPL1 1
#define RPL2 2
#define RPL3 3

typedef struct {
  Bit16u limit;
  Bit32u base;
  } __attribute__ ((packed)) gdt_info_t;


#endif  /* __DESCRIPTOR_H__ */
