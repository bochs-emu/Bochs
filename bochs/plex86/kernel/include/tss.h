/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  tss.h: defines for x86 hardware tasking structures
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

#ifndef __TSS_H__
#define __TSS_H__

typedef struct 
{
    Bit16u back, RESERVED0;      /* Backlink */
    Bit32u esp0;                 /* The CK stack pointer */
    Bit16u ss0,  RESERVED1;      /* The CK stack selector */
    Bit32u esp1;                 /* The parent KL stack pointer */
    Bit16u ss1,  RESERVED2;      /* The parent KL stack selector */
    Bit32u esp2;                 /* Unused */
    Bit16u ss2,  RESERVED3;      /* Unused */
    Bit32u cr3;                  /* The page directory pointer */
    Bit32u eip;                  /* The instruction pointer */
    Bit32u eflags;               /* The flags */
    Bit32u eax, ecx, edx, ebx;   /* The general purpose registers */
    Bit32u esp, ebp, esi, edi;   /* The special purpose registers */
    Bit16u es,   RESERVED4;      /* The extra selector */
    Bit16u cs,   RESERVED5;      /* The code selector */
    Bit16u ss,   RESERVED6;      /* The application stack selector */
    Bit16u ds,   RESERVED7;      /* The data selector */
    Bit16u fs,   RESERVED8;      /* And another extra selector */
    Bit16u gs,   RESERVED9;      /* ... and another one */
    Bit16u ldt,  RESERVED10;     /* The local descriptor table */
    Bit16u trap;                 /* The trap flag (for debugging) */
    Bit16u io;                   /* The I/O Map base address */
} __attribute__ ((packed)) tss_t;

#endif  /* __TSS_H__ */
