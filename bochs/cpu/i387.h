#ifndef BX_I387_RELATED_EXTENSIONS_H
#define BX_I387_RELATED_EXTENSIONS_H

/* Get data sizes from config.h generated from simulator's
 * configure script
 */
#include "config.h"
typedef Bit8u  u8; /* for FPU only */
typedef Bit8s  s8;
typedef Bit16u u16;
typedef Bit16s s16;
typedef Bit32u u32;
typedef Bit32s s32;
typedef Bit64u u64;
typedef Bit64s s64;

#ifdef BX_BIG_ENDIAN
struct BxFpuRegister {
  u16 aligment1, aligment2, aligment3;
  s16 exp;   /* Signed quantity used in internal arithmetic. */
  u32 sigh;
  u32 sigl;
} GCC_ATTRIBUTE((aligned(16), packed));
#else
struct BxFpuRegister {
  u32 sigl;
  u32 sigh;
  s16 exp;   /* Signed quantity used in internal arithmetic. */
  u16 aligment1, aligment2, aligment3;
} GCC_ATTRIBUTE((aligned(16), packed));
#endif

//
// Minimal i387 structure, pruned from the linux headers.  Only
// the fields which were necessary are included.
//
struct BxFpuRegisters {
    s32      cwd;
    s32      swd;
    s32      twd;
    s32      fip;
    s32      fcs;
    s32      foo;
    s32      fos;
    u32      fill0; /* to make sure the following aligns on an 8byte boundary */
    u64      st_space[16];   /* 8*16 bytes per FP-reg (aligned) = 128 bytes */
    unsigned char   tos;
    unsigned char   no_update;
    unsigned char   rm;
    unsigned char   alimit;
};

#define BX_FPU_REG(index) \
    (BX_CPU_THIS_PTR the_i387.soft.st_space[index*2])

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
//         A - aligment

typedef struct
{
#ifdef BX_BIG_ENDIAN
   Bit16u aligment1, aligment2, aligment3; 
   Bit16u exp;      /* 4 bytes: FP register exponent, 
                                   set to 0xffff by all MMX instructions */
   BxPackedMmxRegister packed_mmx_register;
#else
   BxPackedMmxRegister packed_mmx_register;
   Bit16u exp;      /* 4 bytes: FP register exponent, 
                                   set to 0xffff by all MMX instructions */
   Bit16u aligment1, aligment2, aligment3; 
#endif
} BxMmxRegister;

/* to be compatible with fpu register file */
struct BxMmxRegisters
{
   Bit32u      cwd; /* fpu control word */
   Bit32u      swd; /* fpu status  word */
   Bit32u      twd; /* fpu tag     word */
   Bit32u      fip;
   Bit32u      fcs;
   Bit32u      foo;
   Bit32u      fos;
   Bit32u      alignment;
   BxMmxRegister mmx[8];   
   unsigned char tos; /* top-of-stack */
   unsigned char no_update;
   unsigned char rm;
   unsigned char alimit;
};
#endif /* BX_SUPPORT_MMX */

typedef union FpuMmxRegisters
{
    struct BxFpuRegisters soft;
#if BX_SUPPORT_MMX
    struct BxMmxRegisters mmx;
#endif
} i387_t;

#if BX_SUPPORT_MMX || BX_SUPPORT_SSE != 0

#define FPU_TWD                (BX_CPU_THIS_PTR the_i387.soft.twd)
#define FPU_SWD                (BX_CPU_THIS_PTR the_i387.soft.swd)
#define FPU_TOS                (BX_CPU_THIS_PTR the_i387.soft.tos)

#define BX_READ_MMX_REG(index) \
    (BX_CPU_THIS_PTR the_i387.mmx.mmx[index].packed_mmx_register)

#define BX_WRITE_MMX_REG(index, value)                                  \
{                                                                       \
   BX_CPU_THIS_PTR the_i387.mmx.mmx[index].packed_mmx_register = value; \
   BX_CPU_THIS_PTR the_i387.mmx.mmx[index].exp = 0xffff;                \
}                                                      

#endif

#endif
