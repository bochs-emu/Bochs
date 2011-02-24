/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2004-2009 Stanislav Shwartsman
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
//
/////////////////////////////////////////////////////////////////////////

#ifndef _BX_I387_RELATED_EXTENSIONS_H_
#define _BX_I387_RELATED_EXTENSIONS_H_

#if BX_SUPPORT_FPU

#include "fpu/softfloat.h"

#define BX_FPU_REG(index) \
    (BX_CPU_THIS_PTR the_i387.st_space[index & 0x7])

#if defined(NEED_CPU_REG_SHORTCUTS)
#define FPU_PARTIAL_STATUS     (BX_CPU_THIS_PTR the_i387.swd)
#define FPU_CONTROL_WORD       (BX_CPU_THIS_PTR the_i387.cwd)
#define FPU_TAG_WORD           (BX_CPU_THIS_PTR the_i387.twd)
#define FPU_TOS                (BX_CPU_THIS_PTR the_i387.tos)
#endif

#include "fpu/tag_w.h"
#include "fpu/status_w.h"
#include "fpu/control_w.h"

extern int FPU_tagof(const floatx80 &reg);

//
// Minimal i387 structure
//
struct BOCHSAPI_MSVCONLY i387_t
{
    i387_t() {}

public:
    void	init();		// used by FINIT/FNINIT instructions
    void	reset();	// called on CPU reset

    int 	is_IA_masked() const { return (cwd & FPU_CW_Invalid); }

    Bit16u 	get_control_word() const { return cwd; }
    Bit16u 	get_tag_word() const { return twd; }
    Bit16u 	get_status_word() const { return (swd & ~FPU_SW_Top & 0xFFFF) | ((tos << 11) & FPU_SW_Top); }
    Bit16u 	get_partial_status() const { return swd; }

    void   	FPU_pop ();
    void   	FPU_push();

    void   	FPU_settagi(int tag, int stnr);
    void   	FPU_settagi_valid(int stnr);
    int    	FPU_gettagi(int stnr);

    floatx80 	FPU_read_regi(int stnr) { return st_space[(tos+stnr) & 7]; }
    void  	FPU_save_regi(floatx80 reg, int stnr);
    void  	FPU_save_regi(floatx80 reg, int tag, int stnr);

public:
    Bit16u cwd; 	// control word
    Bit16u swd; 	// status word
    Bit16u twd;		// tag word
    Bit16u foo; 	// last instruction opcode

    bx_address fip;
    bx_address fdp;
    Bit16u fcs;
    Bit16u fds;

    floatx80 st_space[8];

    unsigned char tos;
    unsigned char align1;
    unsigned char align2;
    unsigned char align3;
};

#define IS_TAG_EMPTY(i) 		                                \
  ((BX_CPU_THIS_PTR the_i387.FPU_gettagi(i)) == FPU_Tag_Empty)

#define BX_READ_FPU_REG(i)		                                \
  (BX_CPU_THIS_PTR the_i387.FPU_read_regi(i))

#define BX_WRITE_FPU_REG(value, i)                                      \
    BX_CPU_THIS_PTR the_i387.FPU_save_regi((value), (i));

#define BX_WRITE_FPU_REGISTER_AND_TAG(value, tag, i)                    \
    BX_CPU_THIS_PTR the_i387.FPU_save_regi((value), (tag), (i));

BX_CPP_INLINE int i387_t::FPU_gettagi(int stnr)
{
  return (twd >> (((stnr+tos) & 7)*2)) & 3;
}

BX_CPP_INLINE void i387_t::FPU_settagi_valid(int stnr)
{
  int regnr = (stnr + tos) & 7;
  twd &= ~(3 << (regnr*2));     // FPU_Tag_Valid == '00
}

BX_CPP_INLINE void i387_t::FPU_settagi(int tag, int stnr)
{
  int regnr = (stnr + tos) & 7;
  twd &= ~(3 << (regnr*2));
  twd |= (tag & 3) << (regnr*2);
}

BX_CPP_INLINE void i387_t::FPU_push(void)
{
  tos = (tos - 1) & 7;
}

BX_CPP_INLINE void i387_t::FPU_pop(void)
{
  twd |= 3 << (tos*2);
  tos = (tos + 1) & 7;
}

// it is only possisble to read FPU tag word through certain
// instructions like FNSAVE, and they update tag word to its
// real value anyway
BX_CPP_INLINE void i387_t::FPU_save_regi(floatx80 reg, int stnr)
{
  st_space[(stnr+tos) & 7] = reg;
  FPU_settagi_valid(stnr);
}

BX_CPP_INLINE void i387_t::FPU_save_regi(floatx80 reg, int tag, int stnr)
{
  st_space[(stnr+tos) & 7] = reg;
  FPU_settagi(tag, stnr);
}

#include <string.h>

BX_CPP_INLINE void i387_t::init()
{
  cwd = 0x037F;
  swd = 0;
  tos = 0;
  twd = 0xFFFF;
  foo = 0;
  fip = 0;
  fcs = 0;
  fds = 0;
  fdp = 0;
}

BX_CPP_INLINE void i387_t::reset()
{
  cwd = 0x0040;
  swd = 0;
  tos = 0;
  twd = 0x5555;
  foo = 0;
  fip = 0;
  fcs = 0;
  fds = 0;
  fdp = 0;

  memset(st_space, 0, sizeof(floatx80)*8);
}

typedef union bx_packed_mmx_reg_t {
   Bit8s   mmx_sbyte[8];
   Bit16s  mmx_s16[4];
   Bit32s  mmx_s32[2];
   Bit64s  mmx_s64;
   Bit8u   mmx_ubyte[8];
   Bit16u  mmx_u16[4];
   Bit32u  mmx_u32[2];
   Bit64u  mmx_u64;
} BxPackedMmxRegister;

#ifdef BX_BIG_ENDIAN
#define mmx64s(i)   mmx_s64
#define mmx32s(i)   mmx_s32[1 - (i)]
#define mmx16s(i)   mmx_s16[3 - (i)]
#define mmxsbyte(i) mmx_sbyte[7 - (i)]
#define mmxubyte(i) mmx_ubyte[7 - (i)]
#define mmx16u(i)   mmx_u16[3 - (i)]
#define mmx32u(i)   mmx_u32[1 - (i)]
#define mmx64u      mmx_u64
#else
#define mmx64s(i)   mmx_s64
#define mmx32s(i)   mmx_s32[(i)]
#define mmx16s(i)   mmx_s16[(i)]
#define mmxsbyte(i) mmx_sbyte[(i)]
#define mmxubyte(i) mmx_ubyte[(i)]
#define mmx16u(i)   mmx_u16[(i)]
#define mmx32u(i)   mmx_u32[(i)]
#define mmx64u      mmx_u64
#endif

/* for compatability with already written code */
#define MMXSB0(reg) (reg.mmxsbyte(0))
#define MMXSB1(reg) (reg.mmxsbyte(1))
#define MMXSB2(reg) (reg.mmxsbyte(2))
#define MMXSB3(reg) (reg.mmxsbyte(3))
#define MMXSB4(reg) (reg.mmxsbyte(4))
#define MMXSB5(reg) (reg.mmxsbyte(5))
#define MMXSB6(reg) (reg.mmxsbyte(6))
#define MMXSB7(reg) (reg.mmxsbyte(7))

#define MMXSW0(reg) (reg.mmx16s(0))
#define MMXSW1(reg) (reg.mmx16s(1))
#define MMXSW2(reg) (reg.mmx16s(2))
#define MMXSW3(reg) (reg.mmx16s(3))

#define MMXSD0(reg) (reg.mmx32s(0))
#define MMXSD1(reg) (reg.mmx32s(1))

#define MMXSQ(reg)  (reg.mmx64s)
#define MMXUQ(reg)  (reg.mmx64u)

#define MMXUD0(reg) (reg.mmx32u(0))
#define MMXUD1(reg) (reg.mmx32u(1))

#define MMXUW0(reg) (reg.mmx16u(0))
#define MMXUW1(reg) (reg.mmx16u(1))
#define MMXUW2(reg) (reg.mmx16u(2))
#define MMXUW3(reg) (reg.mmx16u(3))

#define MMXUB0(reg) (reg.mmxubyte(0))
#define MMXUB1(reg) (reg.mmxubyte(1))
#define MMXUB2(reg) (reg.mmxubyte(2))
#define MMXUB3(reg) (reg.mmxubyte(3))
#define MMXUB4(reg) (reg.mmxubyte(4))
#define MMXUB5(reg) (reg.mmxubyte(5))
#define MMXUB6(reg) (reg.mmxubyte(6))
#define MMXUB7(reg) (reg.mmxubyte(7))

#define BX_MMX_REG(index) (BX_FPU_REG(index).fraction)

#define BX_READ_MMX_REG(index) 						\
    (*((const BxPackedMmxRegister*)(&(BX_MMX_REG(index)))))

#define BX_WRITE_MMX_REG(index, value)            			\
{                                 					\
   (BX_FPU_REG(index)).fraction = MMXUQ(value);				\
   (BX_FPU_REG(index)).exp = 0xffff;       				\
}

#endif		/* BX_SUPPORT_FPU */

#endif
