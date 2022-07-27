/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2015 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_DESCRIPTOR_H
#define BX_DESCRIPTOR_H

//
// |---------------------------------------------|
// |             Segment Descriptor              |
// |---------------------------------------------|
// |33222222|2|2|2|2| 11 11 |1|11|1|11  |        |
// |10987654|3|2|1|0| 98 76 |5|43|2|1098|76543210|
// |--------|-|-|-|-|-------|-|--|-|----|--------|
// |Base    |G|D|L|A|Limit  |P|D |S|Type|Base    |
// |[31-24] | |/| |V|[19-16]| |P | |    |[23-16] |
// |        | |B| |L|       | |L | |    |        |
// |------------------------|--------------------|
// |       Base [15-0]      |    Limit [15-0]    |
// |------------------------|--------------------|
//

typedef struct { /* bx_selector_t */
  Bit16u value;   /* the 16bit value of the selector */
  /* the following fields are extracted from the value field in protected
     mode only.  They're used for sake of efficiency */
  Bit16u index;   /* 13bit index extracted from value in protected mode */
  Bit8u  ti;      /* table indicator bit extracted from value */
  Bit8u  rpl;     /* RPL extracted from value */
} bx_selector_t;

#define BX_SELECTOR_RPL(selector) ((selector) & 0x03)
#define BX_SELECTOR_RPL_MASK (0xfffc)

typedef struct
{

#define SegValidCache  (0x01)
#define SegAccessROK   (0x02)
#define SegAccessWOK   (0x04)
#define SegAccessROK4G (0x08)
#define SegAccessWOK4G (0x10)
  unsigned valid;        // Holds above values, Or'd together. Used to
                         // hold only 0 or 1 once.

  bool p;                /* present */
  Bit8u   dpl;           /* descriptor privilege level 0..3 */
  bool segment;          /* 0 = system/gate, 1 = data/code segment */
  Bit8u   type;          /* For system & gate descriptors:
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

union {
  struct {
    bx_address base;       /* base address: 286=24bits, 386=32bits, long=64 */
    Bit32u  limit_scaled;  /* for efficiency, this contrived field is set to
                            * limit for byte granular, and
                            * (limit << 12) | 0xfff for page granular seg's
                            */
    bool g;                 /* granularity: 0=byte, 1=4K (page) */
    bool d_b;               /* default size: 0=16bit, 1=32bit */
#if BX_SUPPORT_X86_64
    bool l;                 /* long mode: 0=compat, 1=64 bit */
#endif
    bool avl;               /* available for use by system */
  } segment;
  struct {
    Bit8u   param_count;   /* 5bits (0..31) #words/dword to copy from caller's
                            * stack to called procedure's stack. */
    Bit16u  dest_selector;
    Bit32u  dest_offset;
  } gate;
  struct {                 /* type 5: Task Gate Descriptor */
    Bit16u  tss_selector;  /* TSS segment selector */
  } taskgate;
} u;

} bx_descriptor_t;

// For system & gate descriptors:
enum {
  BX_GATE_TYPE_NONE            = 0x0,
  BX_SYS_SEGMENT_AVAIL_286_TSS = 0x1,
  BX_SYS_SEGMENT_LDT           = 0x2,
  BX_SYS_SEGMENT_BUSY_286_TSS  = 0x3,
  BX_286_CALL_GATE             = 0x4,
  BX_TASK_GATE                 = 0x5,
  BX_286_INTERRUPT_GATE        = 0x6,
  BX_286_TRAP_GATE             = 0x7,
                              /* 0x8 reserved */
  BX_SYS_SEGMENT_AVAIL_386_TSS = 0x9,
                              /* 0xa reserved */
  BX_SYS_SEGMENT_BUSY_386_TSS  = 0xb,
  BX_386_CALL_GATE             = 0xc,
                              /* 0xd reserved */
  BX_386_INTERRUPT_GATE        = 0xe,
  BX_386_TRAP_GATE             = 0xf,
};

// For data/code descriptors:
enum {
  BX_DATA_READ_ONLY                       = 0x0,
  BX_DATA_READ_ONLY_ACCESSED              = 0x1,
  BX_DATA_READ_WRITE                      = 0x2,
  BX_DATA_READ_WRITE_ACCESSED             = 0x3,
  BX_DATA_READ_ONLY_EXPAND_DOWN           = 0x4,
  BX_DATA_READ_ONLY_EXPAND_DOWN_ACCESSED  = 0x5,
  BX_DATA_READ_WRITE_EXPAND_DOWN          = 0x6,
  BX_DATA_READ_WRITE_EXPAND_DOWN_ACCESSED = 0x7,
  BX_CODE_EXEC_ONLY                       = 0x8,
  BX_CODE_EXEC_ONLY_ACCESSED              = 0x9,
  BX_CODE_EXEC_READ                       = 0xa,
  BX_CODE_EXEC_READ_ACCESSED              = 0xb,
  BX_CODE_EXEC_ONLY_CONFORMING            = 0xc,
  BX_CODE_EXEC_ONLY_CONFORMING_ACCESSED   = 0xd,
  BX_CODE_EXEC_READ_CONFORMING            = 0xe,
  BX_CODE_EXEC_READ_CONFORMING_ACCESSED   = 0xf
};

#define IS_PRESENT(descriptor) (descriptor.p)

#if BX_SUPPORT_X86_64
  #define IS_LONG64_SEGMENT(descriptor)   (descriptor.u.segment.l)
#else
  #define IS_LONG64_SEGMENT(descriptor)   (0)
#endif

enum {
  BX_SEGMENT_CODE             = 0x8,
  BX_SEGMENT_DATA_EXPAND_DOWN = 0x4,
  BX_SEGMENT_CODE_CONFORMING  = 0x4,
  BX_SEGMENT_DATA_WRITE       = 0x2,
  BX_SEGMENT_CODE_READ        = 0x2,
  BX_SEGMENT_ACCESSED         = 0x1
};

#define IS_CODE_SEGMENT(type)             ((type) & BX_SEGMENT_CODE)
#define IS_CODE_SEGMENT_CONFORMING(type)  ((type) & BX_SEGMENT_CODE_CONFORMING)
#define IS_DATA_SEGMENT_EXPAND_DOWN(type) ((type) & BX_SEGMENT_DATA_EXPAND_DOWN)
#define IS_CODE_SEGMENT_READABLE(type)    ((type) & BX_SEGMENT_CODE_READ)
#define IS_DATA_SEGMENT_WRITEABLE(type)   ((type) & BX_SEGMENT_DATA_WRITE)
#define IS_SEGMENT_ACCESSED(type)         ((type) & BX_SEGMENT_ACCESSED)

#define IS_DATA_SEGMENT(type) (! IS_CODE_SEGMENT(type))
#define IS_CODE_SEGMENT_NON_CONFORMING(type) \
            (! IS_CODE_SEGMENT_CONFORMING(type))

typedef struct {
  bx_selector_t    selector;
  bx_descriptor_t  cache;
} bx_segment_reg_t;

typedef struct {
  bx_address       base;   /* base address: 24bits=286,32bits=386,64bits=x86-64 */
  Bit16u           limit;  /* limit, 16bits */
} bx_global_segment_reg_t;

void  parse_selector(Bit16u raw_selector, bx_selector_t *selector);
Bit8u get_ar_byte(const bx_descriptor_t *d);
void  set_ar_byte(bx_descriptor_t *d, Bit8u ar_byte);
void  parse_descriptor(Bit32u dword1, Bit32u dword2, bx_descriptor_t *temp);

#endif
