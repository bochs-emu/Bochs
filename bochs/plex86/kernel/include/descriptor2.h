/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  descriptor2.h: defines for descriptors and selectors
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


#ifndef __DESCRIPTOR2_H__
#define __DESCRIPTOR2_H__


typedef struct 
{
    Bit16u   offset_low;
    selector_t selector;
  unsigned count:5;
    unsigned RESERVED:3;
    unsigned type:5;
    unsigned dpl:2;
    unsigned p:1;
    Bit16u   offset_high;
} __attribute__ ((packed)) gate_t;


#define SET_INT_GATE(d, S,O,P,DPL, D) {\
    d.selector = (S);\
    d.offset_high = (O) >> 16;\
    d.offset_low = (O) & 0xffff;\
    d.RESERVED = 0;\
    d.type = ((D)<<3) | 0x6;\
    d.dpl = (DPL);\
    d.p = (P);\
}

#define SET_TRAP_GATE(d, S,O,P,DPL, D) {\
    d.selector = (S);\
    d.offset_high = (O) >> 16;\
    d.offset_low = (O) & 0xffff;\
    d.RESERVED = 0;\
    d.type = ((D)<<3) | 0x7;\
    d.dpl = (DPL);\
    d.p = (P);\
}

typedef struct 
{
    Bit32u offset;
    Bit16u selector;
} __attribute ((packed)) far_jmp_info_t;

#endif  /* __DESCRIPTOR2_H__ */
