/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2004 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <gate at fidonet.org.il>
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
//


#ifndef _BX_I387_RELATED_EXTENSIONS_H_
#define _BX_I387_RELATED_EXTENSIONS_H_

#if BX_SUPPORT_FPU

#include "fpu/softfloat.h"

#define BX_FPU_REG(index) \
    (BX_CPU_THIS_PTR the_i387.st_space[index])

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
struct i387_t 
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
    int    	FPU_gettagi(int stnr);

    floatx80 	FPU_read_regi(int stnr) { return st_space[(tos+stnr) & 7]; }
    void  	FPU_save_regi(floatx80 reg, int tag, int stnr);
    void  	FPU_save_regi(floatx80 reg, int stnr) { FPU_save_regi(reg, FPU_tagof(reg), stnr); }

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

#define BX_WRITE_FPU_REGISTER_AND_TAG(value, tag, i)                    \
    BX_CPU_THIS_PTR the_i387.FPU_save_regi((value), (tag), (i));

#define BX_WRITE_FPU_REG(value, i)                                      \
    BX_CPU_THIS_PTR the_i387.FPU_save_regi((value), (i));

BX_CPP_INLINE int i387_t::FPU_gettagi(int stnr)
{
  return (twd >> (((stnr+tos) & 7)*2)) & 3;
}

BX_CPP_INLINE void i387_t::FPU_settagi(int tag, int stnr)
{
  int regnr = (stnr + tos) & 7;
  twd &= ~(3 << (regnr*2));
  twd |= (tag & 3) << (regnr*2);
}

BX_CPP_INLINE void i387_t::FPU_push(void)
{
  tos--;
}

BX_CPP_INLINE void i387_t::FPU_pop(void)
{
  twd |= 3 << ((tos & 7)*2);
  tos++;
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

#if BX_SUPPORT_MMX

typedef union bx_packed_mmx_reg_t {
   Bit8s   _sbyte[8];
   Bit16s  _s16[4];
   Bit32s  _s32[2];
   Bit64s  _s64;
   Bit8u   _ubyte[8];
   Bit16u  _u16[4];
   Bit32u  _u32[2];
   Bit64u  _u64;
} BxPackedMmxRegister;

#ifdef BX_BIG_ENDIAN
#define mmx64s(i)   _s64
#define mmx32s(i)   _s32[1 - (i)]
#define mmx16s(i)   _s16[3 - (i)]
#define mmxsbyte(i) _sbyte[7 - (i)]
#define mmxubyte(i) _ubyte[7 - (i)]
#define mmx16u(i)   _u16[3 - (i)]
#define mmx32u(i)   _u32[1 - (i)]
#define mmx64u      _u64
#else
#define mmx64s(i)   _s64
#define mmx32s(i)   _s32[(i)]
#define mmx16s(i)   _s16[(i)]
#define mmxsbyte(i) _sbyte[(i)]
#define mmxubyte(i) _ubyte[(i)]
#define mmx16u(i)   _u16[(i)]
#define mmx32u(i)   _u32[(i)]
#define mmx64u      _u64
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

#endif 		/* BX_SUPPORT_MMX */

#endif		/* BX_SUPPORT_FPU */

#endif
