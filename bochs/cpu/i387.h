/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2002 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <gate@fidonet.org.il>
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



#ifndef BX_I387_RELATED_EXTENSIONS_H
#define BX_I387_RELATED_EXTENSIONS_H

#if BX_SUPPORT_FPU

//
// Minimal i387 structure
//
struct i387_t 
{
    Bit32u cwd; 	// control word
    Bit32u swd; 	// status word
    Bit32u twd;		// tag word
    Bit32u fip;
    Bit32u fcs;
    Bit32u foo; 	// last instruction opcode
    Bit32u fos;

    unsigned char tos;
    unsigned char no_update;
    unsigned char rm;
    unsigned char align8;

    Bit64u st_space[16]; // 8*16 bytes per FP-reg (aligned) = 128 bytes
};

// Endian  Host byte order         Guest (x86) byte order
// ======================================================
// Little  FFFFFFFFEEAAAAAA        FFFFFFFFEEAAAAAA
// Big     AAAAAAEEFFFFFFFF        FFFFFFFFEEAAAAAA
//
// Legend: F - fraction/mmx
//         E - exponent
//         A - alignment

#ifdef BX_BIG_ENDIAN
#if defined(__MWERKS__) && defined(macintosh)
#pragma options align=mac68k
#endif
struct bx_fpu_reg_t {
  Bit16u alignment1, alignment2, alignment3;
  Bit16s exp;   /* Signed quantity used in internal arithmetic. */
  Bit32u sigh;
  Bit32u sigl;
} GCC_ATTRIBUTE((aligned(16), packed));
#if defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif
#else
struct bx_fpu_reg_t {
  Bit32u sigl;
  Bit32u sigh;
  Bit16s exp;   /* Signed quantity used in internal arithmetic. */
  Bit16u alignment1, alignment2, alignment3;
} GCC_ATTRIBUTE((aligned(16), packed));
#endif

typedef struct bx_fpu_reg_t FPU_REG;

#define BX_FPU_REG(index) \
    (BX_CPU_THIS_PTR the_i387.st_space[index*2])

#define FPU_PARTIAL_STATUS     (BX_CPU_THIS_PTR the_i387.swd)
#define FPU_CONTROL_WORD       (BX_CPU_THIS_PTR the_i387.cwd)
#define FPU_TAG_WORD           (BX_CPU_THIS_PTR the_i387.twd)
#define FPU_TOS                (BX_CPU_THIS_PTR the_i387.tos)

#define FPU_SW_SUMMARY         (0x0080)		/* exception summary */

#ifdef __cplusplus
extern "C" 
{
#endif
  int FPU_tagof(FPU_REG *reg) BX_CPP_AttrRegparmN(1);
#ifdef __cplusplus
}
#endif

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

// Endian  Host byte order         Guest (x86) byte order
// ======================================================
// Little  FFFFFFFFEEAAAAAA        FFFFFFFFEEAAAAAA
// Big     AAAAAAEEFFFFFFFF        FFFFFFFFEEAAAAAA
//
// Legend: F - fraction/mmx
//         E - exponent
//         A - alignment

#ifdef BX_BIG_ENDIAN
#if defined(__MWERKS__) && defined(macintosh)
#pragma options align=mac68k
#endif
struct bx_mmx_reg_t {
   Bit16u alignment1, alignment2, alignment3; 
   Bit16u exp; /* 2 byte FP-reg exponent */
   BxPackedMmxRegister packed_mmx_register;
} GCC_ATTRIBUTE((aligned(16), packed));
#if defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif
#else
struct bx_mmx_reg_t {
   BxPackedMmxRegister packed_mmx_register;
   Bit16u exp; /* 2 byte FP reg exponent */
   Bit16u alignment1, alignment2, alignment3; 
} GCC_ATTRIBUTE((aligned(16), packed));
#endif

#define BX_MMX_REG(index) 						\
    (((bx_mmx_reg_t*)(BX_CPU_THIS_PTR the_i387.st_space))[index])

#define BX_READ_MMX_REG(index) 						\
    ((BX_MMX_REG(index)).packed_mmx_register)

#define BX_WRITE_MMX_REG(index, value)            			\
{                                 					\
   (BX_MMX_REG(index)).packed_mmx_register = value;			\
   (BX_MMX_REG(index)).exp = 0xffff;       				\
}                                                      

#endif 		/* BX_SUPPORT_MMX */

#endif		/* BX_SUPPORT_FPU */

#endif
