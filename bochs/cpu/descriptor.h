/////////////////////////////////////////////////////////////////////////
// $Id: descriptor.h
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_DESCRIPTOR_H
#define BX_DESCRIPTOR_H

typedef struct { /* bx_selector_t */
  Bit16u value;   /* the 16bit value of the selector */
#if BX_CPU_LEVEL >= 2
    /* the following fields are extracted from the value field in protected
       mode only.  They're used for sake of efficiency */
  Bit16u index;   /* 13bit index extracted from value in protected mode */
  Bit8u  ti;      /* table indicator bit extracted from value */
  Bit8u  rpl;     /* RPL extracted from value */
#endif
} bx_selector_t;

#define BX_SELECTOR_RPL_MASK (0xfffc)

typedef struct
{
  bx_bool valid;         // Holds above values, Or'd together.  Used to
                         // hold only 0 or 1.

#define SegValidCache (0x1)
#define SegAccessROK  (0x2)
#define SegAccessWOK  (0x4)

  bx_bool p;             /* present */
  Bit8u   dpl;           /* descriptor privilege level 0..3 */
  bx_bool segment;       /* 0 = system/gate, 1 = data/code segment */
  Bit8u   type;          /* For system & gate descriptors, only
                          *  0 = invalid descriptor (reserved)
                          *  1 = 286 available Task State Segment (TSS)
                          *  2 = LDT descriptor
                          *  3 = 286 busy Task State Segment (TSS)
                          *  4 = 286 call gate
                          *  5 = task gate
                          *  6 = 286 interrupt gate
                          *  7 = 286 trap gate
                          *  8 = (reserved)
                          *  9 = 386 available TSS
                          * 10 = (reserved)
                          * 11 = 386 busy TSS
                          * 12 = 386 call gate
                          * 13 = (reserved)
                          * 14 = 386 interrupt gate
                          * 15 = 386 trap gate */

#define BX_GATE_TYPE_NONE              (0x0)
#define BX_SYS_SEGMENT_AVAIL_286_TSS   (0x1)
#define BX_SYS_SEGMENT_LDT             (0x2)
#define BX_SYS_SEGMENT_BUSY_286_TSS    (0x3)
#define BX_286_CALL_GATE               (0x4)
#define BX_TASK_GATE                   (0x5)
#define BX_286_INTERRUPT_GATE          (0x6)
#define BX_286_TRAP_GATE               (0x7)
                                     /* 0x8 reserved */
#define BX_SYS_SEGMENT_AVAIL_386_TSS   (0x9)
                                     /* 0xa reserved */
#define BX_SYS_SEGMENT_BUSY_386_TSS    (0xb)
#define BX_386_CALL_GATE               (0xc)
                                     /* 0xd reserved */
#define BX_386_INTERRUPT_GATE          (0xe)
#define BX_386_TRAP_GATE               (0xf)

union {
  struct {
    bx_bool executable;    /* 1=code, 0=data or stack segment */
    bx_bool c_ed;          /* for code: 1=conforming,
                              for data/stack: 1=expand down */
    bx_bool r_w;           /* for code: readable?, for data/stack: writeable? */
    bx_bool a;             /* accessed? */
    bx_address  base;      /* base address: 286=24bits, 386=32bits, long=64 */
    Bit32u  limit;         /* limit: 286=16bits, 386=20bits */
    Bit32u  limit_scaled;  /* for efficiency, this contrived field is set to
                            * limit for byte granular, and
                            * (limit << 12) | 0xfff for page granular seg's
                            */
#if BX_CPU_LEVEL >= 3
    bx_bool g;             /* granularity: 0=byte, 1=4K (page) */
    bx_bool d_b;           /* default size: 0=16bit, 1=32bit */
#if BX_SUPPORT_X86_64
    bx_bool l;             /* long mode: 0=compat, 1=64 bit */
#endif
    bx_bool avl;           /* available for use by system */
#endif
  } segment;
  struct {
    Bit8u   word_count;    /* 5bits (0..31) #words to copy from caller's stack
                            * to called procedure's stack.  (call gates only)*/
    Bit16u  dest_selector;
    Bit16u  dest_offset;
  } gate286;
  struct {                 // type 5: Task Gate Descriptor
    Bit16u  tss_selector;  // TSS segment selector
  } taskgate;
#if BX_CPU_LEVEL >= 3
  struct {
    Bit8u   dword_count;   /* 5bits (0..31) #dwords to copy from caller's stack
                            * to called procedure's stack.  (call gates only)*/
    Bit16u  dest_selector;
    Bit32u  dest_offset;
  } gate386;
#endif
  struct {
    Bit32u  base;          /* 24 bit 286 TSS base  */
    Bit16u  limit;         /* 16 bit 286 TSS limit */
  } tss286;
#if BX_CPU_LEVEL >= 3
  struct {
    bx_address  base;      /* 32/64 bit 386 TSS base */
    Bit32u  limit;         /* 20 bit 386 TSS limit */
    Bit32u  limit_scaled;  // Same notes as for 'segment' field
    bx_bool g;             /* granularity: 0=byte, 1=4K (page) */
    bx_bool avl;           /* available for use by system */
  } tss386;
#endif
  struct {
    bx_address  base;      /* 286=24 386+ =32/64 bit LDT base */
    Bit16u  limit;         /* 286+ =16 bit LDT limit */
  } ldt;
} u;

} bx_descriptor_t;

typedef struct {
  bx_selector_t          selector;
  bx_descriptor_t  cache;
} bx_segment_reg_t;

#if BX_CPU_LEVEL < 2
  /* no GDTR or IDTR register in an 8086 */
#else
typedef struct {
  bx_address             base;      /* base address: 24bits=286,32bits=386,64bits=x86-64 */
  Bit16u                 limit;     /* limit, 16bits */
  } bx_global_segment_reg_t;
#endif

#endif
