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
    Bit32u cwd;		 // control word
    Bit32u swd;		 // status word
    Bit32u twd;		 // tag word
    Bit32u fip;
    Bit32u fcs;
    Bit32u foo;
    Bit32u fos;

    unsigned char tos;
    unsigned char no_update;
    unsigned char rm;
    unsigned align8;

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

typedef union {
  Bit8u u8;
  Bit8s s8;
} MMX_BYTE;

typedef union {
  Bit16u u16;
  Bit16s s16;
  struct {
#ifdef BX_BIG_ENDIAN
    MMX_BYTE hi;
    MMX_BYTE lo;
#else
    MMX_BYTE lo;
    MMX_BYTE hi;
#endif
  } bytes;
} MMX_WORD;

typedef union {
  Bit32u u32;
  Bit32s s32;
  struct {
#ifdef BX_BIG_ENDIAN
    MMX_WORD hi;
    MMX_WORD lo;
#else
    MMX_WORD lo;
    MMX_WORD hi;
#endif
  } words;
} MMX_DWORD;

typedef union {
  Bit64u u64;
  Bit64s s64;
  struct {
#ifdef BX_BIG_ENDIAN
    MMX_DWORD hi;
    MMX_DWORD lo;
#else
    MMX_DWORD lo;
    MMX_DWORD hi;
#endif
  } dwords;
} MMX_QWORD, BxPackedMmxRegister;

#define MMXSB0(reg) (reg.dwords.lo.words.lo.bytes.lo.s8)
#define MMXSB1(reg) (reg.dwords.lo.words.lo.bytes.hi.s8)
#define MMXSB2(reg) (reg.dwords.lo.words.hi.bytes.lo.s8)
#define MMXSB3(reg) (reg.dwords.lo.words.hi.bytes.hi.s8)
#define MMXSB4(reg) (reg.dwords.hi.words.lo.bytes.lo.s8)
#define MMXSB5(reg) (reg.dwords.hi.words.lo.bytes.hi.s8)
#define MMXSB6(reg) (reg.dwords.hi.words.hi.bytes.lo.s8)
#define MMXSB7(reg) (reg.dwords.hi.words.hi.bytes.hi.s8)

#define MMXUB0(reg) (reg.dwords.lo.words.lo.bytes.lo.u8)
#define MMXUB1(reg) (reg.dwords.lo.words.lo.bytes.hi.u8)
#define MMXUB2(reg) (reg.dwords.lo.words.hi.bytes.lo.u8)
#define MMXUB3(reg) (reg.dwords.lo.words.hi.bytes.hi.u8)
#define MMXUB4(reg) (reg.dwords.hi.words.lo.bytes.lo.u8)
#define MMXUB5(reg) (reg.dwords.hi.words.lo.bytes.hi.u8)
#define MMXUB6(reg) (reg.dwords.hi.words.hi.bytes.lo.u8)
#define MMXUB7(reg) (reg.dwords.hi.words.hi.bytes.hi.u8)

#define MMXSW0(reg) (reg.dwords.lo.words.lo.s16)
#define MMXSW1(reg) (reg.dwords.lo.words.hi.s16)
#define MMXSW2(reg) (reg.dwords.hi.words.lo.s16)
#define MMXSW3(reg) (reg.dwords.hi.words.hi.s16)

#define MMXUW0(reg) (reg.dwords.lo.words.lo.u16)
#define MMXUW1(reg) (reg.dwords.lo.words.hi.u16)
#define MMXUW2(reg) (reg.dwords.hi.words.lo.u16)
#define MMXUW3(reg) (reg.dwords.hi.words.hi.u16)
                                
#define MMXSD0(reg) (reg.dwords.lo.s32)
#define MMXSD1(reg) (reg.dwords.hi.s32)

#define MMXUD0(reg) (reg.dwords.lo.u32)
#define MMXUD1(reg) (reg.dwords.hi.u32)

#define MMXSQ(reg)  (reg.s64)
#define MMXUQ(reg)  (reg.u64)

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
