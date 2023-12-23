/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2018 Stanislav Shwartsman
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

#ifndef BX_SSE_EXTENSIONS_H
#define BX_SSE_EXTENSIONS_H

/* XMM REGISTER */

typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
__declspec(align(16))
#endif
union bx_xmm_reg_t {
   Bit8s   xmm_sbyte[16];
   Bit16s  xmm_s16[8];
   Bit32s  xmm_s32[4];
   Bit64s  xmm_s64[2];
   Bit8u   xmm_ubyte[16];
   Bit16u  xmm_u16[8];
   Bit32u  xmm_u32[4];
   Bit64u  xmm_u64[2];

   void clear() { xmm_u64[0] = xmm_u64[1] = 0; }
} BxPackedXmmRegister;

#ifdef BX_BIG_ENDIAN
#define xmm64s(i)   xmm_s64[1 - (i)]
#define xmm32s(i)   xmm_s32[3 - (i)]
#define xmm16s(i)   xmm_s16[7 - (i)]
#define xmmsbyte(i) xmm_sbyte[15 - (i)]
#define xmmubyte(i) xmm_ubyte[15 - (i)]
#define xmm16u(i)   xmm_u16[7 - (i)]
#define xmm32u(i)   xmm_u32[3 - (i)]
#define xmm64u(i)   xmm_u64[1 - (i)]
#else
#define xmm64s(i)   xmm_s64[(i)]
#define xmm32s(i)   xmm_s32[(i)]
#define xmm16s(i)   xmm_s16[(i)]
#define xmmsbyte(i) xmm_sbyte[(i)]
#define xmmubyte(i) xmm_ubyte[(i)]
#define xmm16u(i)   xmm_u16[(i)]
#define xmm32u(i)   xmm_u32[(i)]
#define xmm64u(i)   xmm_u64[(i)]
#endif

/* AVX REGISTER */

typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
__declspec(align(32))
#endif
union bx_ymm_reg_t {
   Bit8s   ymm_sbyte[32];
   Bit16s  ymm_s16[16];
   Bit32s  ymm_s32[8];
   Bit64s  ymm_s64[4];
   Bit8u   ymm_ubyte[32];
   Bit16u  ymm_u16[16];
   Bit32u  ymm_u32[8];
   Bit64u  ymm_u64[4];
   BxPackedXmmRegister ymm_v128[2];

   void clear() {
     ymm_v128[0].clear();
     ymm_v128[1].clear();
   }
} BxPackedYmmRegister;

#ifdef BX_BIG_ENDIAN
#define ymm64s(i)   ymm_s64[3 - (i)]
#define ymm32s(i)   ymm_s32[7 - (i)]
#define ymm16s(i)   ymm_s16[15 - (i)]
#define ymmsbyte(i) ymm_sbyte[31 - (i)]
#define ymmubyte(i) ymm_ubyte[31 - (i)]
#define ymm16u(i)   ymm_u16[15 - (i)]
#define ymm32u(i)   ymm_u32[7 - (i)]
#define ymm64u(i)   ymm_u64[3 - (i)]
#define ymm128(i)   ymm_v128[1 - (i)]
#else
#define ymm64s(i)   ymm_s64[(i)]
#define ymm32s(i)   ymm_s32[(i)]
#define ymm16s(i)   ymm_s16[(i)]
#define ymmsbyte(i) ymm_sbyte[(i)]
#define ymmubyte(i) ymm_ubyte[(i)]
#define ymm16u(i)   ymm_u16[(i)]
#define ymm32u(i)   ymm_u32[(i)]
#define ymm64u(i)   ymm_u64[(i)]
#define ymm128(i)   ymm_v128[(i)]
#endif

typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
__declspec(align(64))
#endif
union bx_zmm_reg_t {
   Bit8s   zmm_sbyte[64];
   Bit16s  zmm_s16[32];
   Bit32s  zmm_s32[16];
   Bit64s  zmm_s64[8];
   Bit8u   zmm_ubyte[64];
   Bit16u  zmm_u16[32];
   Bit32u  zmm_u32[16];
   Bit64u  zmm_u64[8];
   BxPackedXmmRegister zmm_v128[4];
   BxPackedYmmRegister zmm_v256[2];

   void clear() {
     zmm_v256[0].clear();
     zmm_v256[1].clear();
   }
} BxPackedZmmRegister;

#ifdef BX_BIG_ENDIAN
#define zmm64s(i)   zmm_s64[7 - (i)]
#define zmm32s(i)   zmm_s32[15 - (i)]
#define zmm16s(i)   zmm_s16[31 - (i)]
#define zmmsbyte(i) zmm_sbyte[63 - (i)]
#define zmmubyte(i) zmm_ubyte[63 - (i)]
#define zmm16u(i)   zmm_u16[31 - (i)]
#define zmm32u(i)   zmm_u32[15 - (i)]
#define zmm64u(i)   zmm_u64[7 - (i)]
#define zmm128(i)   zmm_v128[3 - (i)]
#define zmm256(i)   zmm_v256[1 - (i)]
#else
#define zmm64s(i)   zmm_s64[(i)]
#define zmm32s(i)   zmm_s32[(i)]
#define zmm16s(i)   zmm_s16[(i)]
#define zmmsbyte(i) zmm_sbyte[(i)]
#define zmmubyte(i) zmm_ubyte[(i)]
#define zmm16u(i)   zmm_u16[(i)]
#define zmm32u(i)   zmm_u32[(i)]
#define zmm64u(i)   zmm_u64[(i)]
#define zmm128(i)   zmm_v128[(i)]
#define zmm256(i)   zmm_v256[(i)]
#endif

#if BX_SUPPORT_EVEX
#  define vmm64s(i)   zmm64s(i)
#  define vmm32s(i)   zmm32s(i)
#  define vmm16s(i)   zmm16s(i)
#  define vmmsbyte(i) zmmsbyte(i)
#  define vmmubyte(i) zmmubyte(i)
#  define vmm16u(i)   zmm16u(i)
#  define vmm32u(i)   zmm32u(i)
#  define vmm64u(i)   zmm64u(i)
#  define vmm128(i)   zmm128(i)
#  define vmm256(i)   zmm256(i)
#else
#  if BX_SUPPORT_AVX
#    define vmm64s(i)   ymm64s(i)
#    define vmm32s(i)   ymm32s(i)
#    define vmm16s(i)   ymm16s(i)
#    define vmmsbyte(i) ymmsbyte(i)
#    define vmmubyte(i) ymmubyte(i)
#    define vmm16u(i)   ymm16u(i)
#    define vmm32u(i)   ymm32u(i)
#    define vmm64u(i)   ymm64u(i)
#    define vmm128(i)   ymm128(i)
#  else
#    define vmm64s(i)   xmm64s(i)
#    define vmm32s(i)   xmm32s(i)
#    define vmm16s(i)   xmm16s(i)
#    define vmmsbyte(i) xmmsbyte(i)
#    define vmmubyte(i) xmmubyte(i)
#    define vmm16u(i)   xmm16u(i)
#    define vmm32u(i)   xmm32u(i)
#    define vmm64u(i)   xmm64u(i)
#  endif
#endif

#if BX_SUPPORT_EVEX
typedef BxPackedZmmRegister BxPackedAvxRegister;
#else
#if BX_SUPPORT_AVX
typedef BxPackedYmmRegister BxPackedAvxRegister;
#endif
#endif

#define  BYTE_ELEMENTS(vlen) (16 * (vlen))
#define  WORD_ELEMENTS(vlen)  (8 * (vlen))
#define DWORD_ELEMENTS(vlen)  (4 * (vlen))
#define QWORD_ELEMENTS(vlen)  (2 * (vlen))

/* ************ */
/* XMM REGISTER */
/* ************ */

#if BX_SUPPORT_AVX

/* read XMM register */
#define BX_READ_XMM_REG(index) (BX_CPU_THIS_PTR vmm[index].vmm128(0))

#else /* BX_SUPPORT_AVX */

/* read XMM register */
#define BX_READ_XMM_REG(index) (BX_CPU_THIS_PTR vmm[index])

#endif /* BX_SUPPORT_AVX */

/* read only high 64 bit of the register */
#define BX_READ_XMM_REG_HI_QWORD(index) \
    (BX_CPU_THIS_PTR vmm[index].vmm64u(1))

/* read only low 64 bit of the register */
#define BX_READ_XMM_REG_LO_QWORD(index) \
    (BX_CPU_THIS_PTR vmm[index].vmm64u(0))

/* read only low 32 bit of the register */
#define BX_READ_XMM_REG_LO_DWORD(index) \
    (BX_CPU_THIS_PTR vmm[index].vmm32u(0))

/* read only low 16 bit of the register */
#define BX_READ_XMM_REG_LO_WORD(index) \
    (BX_CPU_THIS_PTR vmm[index].vmm16u(0))

/* read only low 8 bit of the register */
#define BX_READ_XMM_REG_LO_BYTE(index) \
    (BX_CPU_THIS_PTR vmm[index].vmmubyte(0))

/* short names for above macroses */
#define BX_XMM_REG_HI_QWORD BX_READ_XMM_REG_HI_QWORD
#define BX_XMM_REG_LO_QWORD BX_READ_XMM_REG_LO_QWORD
#define BX_XMM_REG_LO_DWORD BX_READ_XMM_REG_LO_DWORD

#define BX_XMM_REG BX_READ_XMM_REG

/* store only high 64 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_HI_QWORD(index, reg64) \
    { BX_CPU_THIS_PTR vmm[index].vmm64u(1) = (reg64); }

/* store only low 64 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_LO_QWORD(index, reg64) \
    { BX_CPU_THIS_PTR vmm[index].vmm64u(0) = (reg64); }

/* store only low 32 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_LO_DWORD(index, reg32) \
    { BX_CPU_THIS_PTR vmm[index].vmm32u(0) = (reg32); }

/* store only low 16 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_LO_WORD(index, reg16) \
    { BX_CPU_THIS_PTR vmm[index].vmm16u(0) = (reg16); }

/* store only low 8 bit of the register, rest of the register unchanged */
#define BX_WRITE_XMM_REG_LO_BYTE(index, reg8) \
    { BX_CPU_THIS_PTR vmm[index].vmmubyte(0) = (reg8); }

/* store XMM register, upper part of the YMM or ZMM register unchanged */
#define BX_WRITE_XMM_REG(index, reg) \
    { (BX_XMM_REG(index)) = (reg); }

/* clear XMM register, upper part of the YMM or ZMM register unchanged */
#define BX_CLEAR_XMM_REG(index) { BX_XMM_REG(index).clear(); }

/* ************ */
/* YMM REGISTER */
/* ************ */

#if BX_SUPPORT_AVX

#if BX_SUPPORT_EVEX

/* read YMM register */
#define BX_READ_YMM_REG(index) (BX_CPU_THIS_PTR vmm[index].vmm256(0))

/* clear upper part of the ZMM register */
#define BX_CLEAR_AVX_HIGH256(index) { BX_CPU_THIS_PTR vmm[index].vmm256(1).clear(); }

#else /* BX_SUPPORT_EVEX */

/* read YMM register */
#define BX_READ_YMM_REG(index) (BX_CPU_THIS_PTR vmm[index])

/* clear upper part of the ZMM register - no upper part ;) */
#define BX_CLEAR_AVX_HIGH256(index)

#endif /* BX_SUPPORT_EVEX */

#define BX_YMM_REG BX_READ_YMM_REG

/* clear upper part of AVX128 register */
#define BX_CLEAR_AVX_HIGH128(index) \
    { BX_CPU_THIS_PTR vmm[index].vmm128(1).clear(); \
      BX_CLEAR_AVX_HIGH256(index); }

/* write YMM register and clear upper part of the AVX register */
#define BX_WRITE_YMM_REGZ(index, reg) \
    { (BX_READ_YMM_REG(index)) = (reg); BX_CLEAR_AVX_HIGH256(index); }

/* write XMM register and clear upper part of AVX register (if not SSE instruction) */
#define BX_WRITE_XMM_REGZ(index, reg, vlen) \
    { (BX_XMM_REG(index)) = (reg); \
       if (vlen) BX_CLEAR_AVX_HIGH128(index); }

/* write XMM register while clearing upper part of the AVX register */
#define BX_WRITE_XMM_REG_CLEAR_HIGH(index, reg) \
    { BX_XMM_REG(index) = (reg); BX_CLEAR_AVX_HIGH128(index); }

#define BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(index, reg64) \
    { BX_CPU_THIS_PTR vmm[index].vmm64u(0) = (reg64); \
      BX_CPU_THIS_PTR vmm[index].vmm64u(1) = 0; BX_CLEAR_AVX_HIGH128(index); }

#define BX_WRITE_XMM_REG_LO_DWORD_CLEAR_HIGH(index, reg32) \
    { BX_CPU_THIS_PTR vmm[index].vmm64u(0) = Bit64u(reg32); \
      BX_CPU_THIS_PTR vmm[index].vmm64u(1) = 0; BX_CLEAR_AVX_HIGH128(index); }

#else /* BX_SUPPORT_AVX */

/* write XMM register while clearing upper part of AVX register */
#define BX_WRITE_XMM_REG_CLEAR_HIGH(index, reg) \
    BX_WRITE_XMM_REG(index, reg)

/* write XMM register while clearing upper part of AVX register */
#define BX_WRITE_XMM_REGZ(index, reg, vlen) \
    BX_WRITE_XMM_REG(index, reg)

#endif /* BX_SUPPORT_AVX */

/* ************ */
/* AVX REGISTER */
/* ************ */

// vector length independent accessors, i.e. access YMM when no EVEX was compiled in
// or ZMM when EVEX support was compiled in

/* read AVX register */
#define BX_READ_AVX_REG(index) (BX_CPU_THIS_PTR vmm[index])

#define BX_AVX_REG BX_READ_AVX_REG

/* write AVX register */
#define BX_WRITE_AVX_REG(index, reg) { (BX_CPU_THIS_PTR vmm[index]) = (reg); }

/* read AVX register lane */
#define BX_READ_AVX_REG_LANE(index, line) \
     (BX_CPU_THIS_PTR vmm[index].vmm128(line))

/* write AVX register and potentialy clear upper part of the register */
#define BX_WRITE_YMM_REGZ_VLEN(index, reg256, vlen)               \
    { (BX_YMM_REG(index)) = (reg256);                             \
      if (vlen == BX_VL256) { BX_CLEAR_AVX_HIGH256(index); }      \
      else if (vlen == BX_VL128) { BX_CLEAR_AVX_HIGH128(index); } \
    }

/* clear upper part of the AVX register */
#define BX_CLEAR_AVX_REGZ(index, vlen)                              \
    { if ((vlen) == BX_VL256) { BX_CLEAR_AVX_HIGH256(index); }      \
      else if ((vlen) == BX_VL128) { BX_CLEAR_AVX_HIGH128(index); } \
    }

/* write AVX register and potentialy clear upper part of the register */
#define BX_WRITE_AVX_REGZ(index, reg, vlen)                       \
    { BX_CPU_THIS_PTR vmm[index] = (reg);                         \
      BX_CLEAR_AVX_REGZ(index, vlen);                             \
    }

/* clear AVX register */
#define BX_CLEAR_AVX_REG(index) { BX_CPU_THIS_PTR vmm[index].clear(); }

#if BX_SUPPORT_EVEX

/* read upper 256-bit part of ZMM register */
#define BX_READ_ZMM_REG_HI(index) \
     (BX_CPU_THIS_PTR vmm[index].vmm256(1))

#endif

BX_CPP_INLINE int is_clear(const BxPackedXmmRegister *r)
{
  return (r->xmm64u(0) | r->xmm64u(1)) == 0;
}

#if BX_SUPPORT_EVEX
// implement SAE and EVEX encoded rounding control
BX_CPP_INLINE void softfloat_status_word_rc_override(float_status_t &status, bxInstruction_c *i)
{
  /* must be VL512 otherwise EVEX.LL encodes vector length */
  if (i->modC0() && i->getEvexb()) {
    status.float_rounding_mode = i->getRC();
    status.float_suppress_exception = float_all_exceptions_mask;
    status.float_exception_masks = float_all_exceptions_mask;
  }
}
#else
  #define softfloat_status_word_rc_override(status, i)
#endif

#if BX_SUPPORT_FPU
/* convert float16 NaN number to QNaN */
BX_CPP_INLINE float16 convert_to_QNaN(float16 op)
{
  return op | 0x7E00;
}

/* convert float32 NaN number to QNaN */
BX_CPP_INLINE float32 convert_to_QNaN(float32 op)
{
  return op | 0x7FC00000;
}

/* convert float64 NaN number to QNaN */
BX_CPP_INLINE float64 convert_to_QNaN(float64 op)
{
  return op | BX_CONST64(0x7FF8000000000000);
}
#endif

/* MXCSR REGISTER */

/* 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16
 * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==  (reserved)
 *  0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0|MM| 0
 *
 * 15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0
 * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==
 * FZ| R C |PM|UM|OM|ZM|DM|IM|DZ|PE|UE|OE|ZE|DE|IE
 */

/* MXCSR REGISTER FIELDS DESCRIPTION */

/*
 * IE  0    Invalid-Operation Exception             0
 * DE  1    Denormalized-Operand Exception          0
 * ZE  2    Zero-Divide Exception                   0
 * OE  3    Overflow Exception                      0
 * UE  4    Underflow Exception                     0
 * PE  5    Precision Exception                     0
 * DZ  6    Denormals are Zeros                     0
 * IM  7    Invalid-Operation Exception Mask        1
 * DM  8    Denormalized-Operand Exception Mask     1
 * ZM  9    Zero-Divide Exception Mask              1
 * OM 10    Overflow Exception Mask                 1
 * UM 11    Underflow Exception Mask                1
 * PM 12    Precision Exception Mask                1
 * RC 13-14 Floating-Point Rounding Control         00
 * FZ 15    Flush-to-Zero for Masked Underflow      0
 * RZ 16    Reserved                                0
 * MM 17    Misaligned Exception Mask               0
 */

const Bit32u MXCSR_EXCEPTIONS                = 0x0000003F;
const Bit32u MXCSR_DAZ                       = 0x00000040;
const Bit32u MXCSR_MASKED_EXCEPTIONS         = 0x00001F80;
const Bit32u MXCSR_ROUNDING_CONTROL          = 0x00006000;
const Bit32u MXCSR_FLUSH_MASKED_UNDERFLOW    = 0x00008000;
const Bit32u MXCSR_MISALIGNED_EXCEPTION_MASK = 0x00020000;

const Bit32u MXCSR_IE = 0x00000001;
const Bit32u MXCSR_DE = 0x00000002;
const Bit32u MXCSR_ZE = 0x00000004;
const Bit32u MXCSR_OE = 0x00000008;
const Bit32u MXCSR_UE = 0x00000010;
const Bit32u MXCSR_PE = 0x00000020;

const Bit32u MXCSR_IM = 0x00000080;
const Bit32u MXCSR_DM = 0x00000100;
const Bit32u MXCSR_ZM = 0x00000200;
const Bit32u MXCSR_OM = 0x00000400;
const Bit32u MXCSR_UM = 0x00000800;
const Bit32u MXCSR_PM = 0x00001000;

const Bit32u MXCSR_RESET = 0x00001F80;  /* reset value of the MXCSR register */

struct BOCHSAPI bx_mxcsr_t
{
  Bit32u mxcsr;

  bx_mxcsr_t (Bit32u val = MXCSR_RESET)
	: mxcsr(val) {}

#define IMPLEMENT_MXCSR_ACCESSOR(name, bitmask, bitnum)        \
  int get_##name () const {                                    \
    return (mxcsr & (bitmask)) >> (bitnum);                    \
  }

  IMPLEMENT_MXCSR_ACCESSOR(exceptions_masks, MXCSR_MASKED_EXCEPTIONS, 7);
  IMPLEMENT_MXCSR_ACCESSOR(DAZ, MXCSR_DAZ, 6);
  IMPLEMENT_MXCSR_ACCESSOR(rounding_mode, MXCSR_ROUNDING_CONTROL, 13);
  IMPLEMENT_MXCSR_ACCESSOR(flush_masked_underflow, MXCSR_FLUSH_MASKED_UNDERFLOW, 15);
  IMPLEMENT_MXCSR_ACCESSOR(MM, MXCSR_MISALIGNED_EXCEPTION_MASK, 17);

  IMPLEMENT_MXCSR_ACCESSOR(IE, MXCSR_IE, 0);
  IMPLEMENT_MXCSR_ACCESSOR(DE, MXCSR_DE, 1);
  IMPLEMENT_MXCSR_ACCESSOR(ZE, MXCSR_ZE, 2);
  IMPLEMENT_MXCSR_ACCESSOR(OE, MXCSR_OE, 3);
  IMPLEMENT_MXCSR_ACCESSOR(UE, MXCSR_UE, 4);
  IMPLEMENT_MXCSR_ACCESSOR(PE, MXCSR_PE, 5);

  IMPLEMENT_MXCSR_ACCESSOR(IM, MXCSR_IM, 7);
  IMPLEMENT_MXCSR_ACCESSOR(DM, MXCSR_DM, 8);
  IMPLEMENT_MXCSR_ACCESSOR(ZM, MXCSR_ZM, 9);
  IMPLEMENT_MXCSR_ACCESSOR(OM, MXCSR_OM, 10);
  IMPLEMENT_MXCSR_ACCESSOR(UM, MXCSR_UM, 11);
  IMPLEMENT_MXCSR_ACCESSOR(PM, MXCSR_PM, 12);

  void set_exceptions(int status) {
    mxcsr |= (status & MXCSR_EXCEPTIONS);
  }

  void mask_all_exceptions() {
    mxcsr |= (MXCSR_MASKED_EXCEPTIONS);
  }

};

#if defined(NEED_CPU_REG_SHORTCUTS)
  #define MXCSR             (BX_CPU_THIS_PTR mxcsr)
  #define BX_MXCSR_REGISTER (BX_CPU_THIS_PTR mxcsr.mxcsr)
  #define MXCSR_MASK        (BX_CPU_THIS_PTR mxcsr_mask)
#endif

/* INTEGER SATURATION */

/*
 * SaturateWordSToByteS  converts  a  signed  16-bit  value  to a signed
 * 8-bit value.  If  the signed 16-bit value is less than -128, it is
 * represented by the saturated value  -128  (0x80).  If it is  greater
 * than 127, it is represented by the saturated value 127 (0x7F).
*/
BX_CPP_INLINE Bit8s BX_CPP_AttrRegparmN(1) SaturateWordSToByteS(Bit16s value)
{
  if(value < -128) return -128;
  if(value >  127) return  127;
  return (Bit8s) value;
}

/*
 * SaturateQwordSToByteS converts a signed 32-bit value to a signed
 * 8-bit value. If the signed 32-bit value is less than -128, it is
 * represented by the saturated value -128 (0x80). If it is greater
 * than 127, it is represented by the saturated value 127 (0x7F).
*/
BX_CPP_INLINE Bit8s BX_CPP_AttrRegparmN(1) SaturateDwordSToByteS(Bit32s value)
{
  if(value < -128) return -128;
  if(value >  127) return  127;
  return (Bit8s) value;
}

/*
 * SaturateQwordSToByteS converts a signed 64-bit value to a signed
 * 8-bit value. If the signed 64-bit value is less than -128, it is
 * represented by the saturated value -128 (0x80). If it is greater
 * than 127, it is represented by the saturated value 127 (0x7F).
*/
BX_CPP_INLINE Bit8s BX_CPP_AttrRegparmN(1) SaturateQwordSToByteS(Bit64s value)
{
  if(value < -128) return -128;
  if(value >  127) return  127;
  return (Bit8s) value;
}

/*
 * SaturateQwordSToWordS  converts  a  signed  64-bit  value  to a signed
 * 16-bit  value.  If  the signed 64-bit value is less than -32768, it is
 * represented  by  the saturated value -32768 (0x8000). If it is greater
 * than 32767, it is represented by the saturated value 32767 (0x7FFF).
*/
BX_CPP_INLINE Bit16s BX_CPP_AttrRegparmN(1) SaturateQwordSToWordS(Bit64s value)
{
  if(value < -32768) return -32768;
  if(value >  32767) return  32767;
  return (Bit16s) value;
}

/*
 * SaturateDwordSToWordS  converts  a  signed  32-bit  value  to a signed
 * 16-bit  value.  If  the signed 32-bit value is less than -32768, it is
 * represented  by  the saturated value -32768 (0x8000). If it is greater
 * than 32767, it is represented by the saturated value 32767 (0x7FFF).
*/
BX_CPP_INLINE Bit16s BX_CPP_AttrRegparmN(1) SaturateDwordSToWordS(Bit32s value)
{
  if(value < -32768) return -32768;
  if(value >  32767) return  32767;
  return (Bit16s) value;
}

/*
 * SaturateQwordSToDwordS  converts  a  signed  64-bit  value  to a signed
 * 32-bit value. If the signed 64-bit value is less than -2147483648, it
 * is represented by the saturated value -2147483648 (0x80000000). If it
 * is greater than 2147483647, it is represented by the saturated value
 * 2147483647 (0x7FFFFFFF).
*/
BX_CPP_INLINE Bit32s BX_CPP_AttrRegparmN(1) SaturateQwordSToDwordS(Bit64s value)
{
  if(value < BX_CONST64(-2147483648)) return BX_CONST64(-2147483648);
  if(value >  2147483647) return  2147483647;
  return (Bit32s) value;
}

/*
 * SaturateWordSToByteU  converts  a  signed  16-bit value to an unsigned
 * 8-bit  value.  If  the  signed  16-bit  value  is less than zero it is
 * represented  by the saturated value zero (0x00). If it is greater than
 * 255 it is represented by the saturated value 255 (0xFF).
*/
BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(1) SaturateWordSToByteU(Bit16s value)
{
  if(value < 0) return 0;
  if(value > 255) return 255;
  return (Bit8u) value;
}

/*
 * SaturateDwordSToWordU  converts  a  signed 32-bit value to an unsigned
 * 16-bit  value.  If  the  signed  32-bit value is less than zero, it is
 * represented  by  the  saturated  value zero (0x0000). If it is greater
 * than  65535, it is represented by the saturated value 65535 (0xFFFF).
*/
BX_CPP_INLINE Bit16u BX_CPP_AttrRegparmN(1) SaturateDwordSToWordU(Bit32s value)
{
  if(value < 0) return 0;
  if(value > 65535) return 65535;
  return (Bit16u) value;
}

/*
 * SaturateQwordSToDwordU  converts  a  signed 64-bit value to an unsigned
 * 32-bit  value.  If  the  signed  64-bit value is less than  zero, it is
 * represented by the saturated value zero (0x00000000). If it is  greater
 * than  4294967295, it is represented  by the  saturated value 4294967295
 * (0xFFFFFFFF).
*/
BX_CPP_INLINE Bit32u BX_CPP_AttrRegparmN(1) SaturateQwordSToDwordU(Bit64s value)
{
  if(value < 0) return 0;
  if(value > BX_CONST64(4294967295)) return BX_CONST64(4294967295);
  return (Bit32u) value;
}

/*
 * SaturateWordUToByteU converts an unsigned  16-bit value to unsigned
 * 8-bit value. If the unsigned 16-bit value is greater than 255, it is
 * represented by the saturated value 255 (0xFF).
*/
BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(1) SaturateWordUToByteU(Bit16u value)
{
  if(value > 255) return 255;
  return (Bit8u) value;
}

/*
 * SaturateDWordUToByteU converts an unsigned  32-bit value to unsigned
 * 8-bit value. If the unsigned 32-bit value is greater than 255, it is
 * represented by the saturated value 255 (0xFF).
*/
BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(1) SaturateDwordUToByteU(Bit32u value)
{
  if(value > 255) return 255;
  return (Bit8u) value;
}

/*
 * SaturateQWordUToByteU converts an unsigned  64-bit value to unsigned
 * 8-bit value. If the unsigned 64-bit value is greater than 255, it is
 * represented by the saturated value 255 (0xFF).
*/
BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(1) SaturateQwordUToByteU(Bit64u value)
{
  if(value > 255) return 255;
  return (Bit8u) value;
}

/*
 * SaturateQwordUToWordU converts an unsigned 64-bit value to unsigned
 * 16-bit value. If the unsigned 64-bit value is greater than 65535,
 * it is represented by the saturated value 65535 (0xFFFF).
*/
BX_CPP_INLINE Bit16u BX_CPP_AttrRegparmN(1) SaturateQwordUToWordU(Bit64u value)
{
  if(value > 65535) return 65535;
  return (Bit16u) value;
}

/*
 * SaturateDwordUToWordU converts an unsigned 32-bit value to unsigned
 * 16-bit value. If the unsigned 32-bit value is greater than 65535,
 * it is represented by the saturated value 65535 (0xFFFF).
*/
BX_CPP_INLINE Bit16u BX_CPP_AttrRegparmN(1) SaturateDwordUToWordU(Bit32u value)
{
  if(value > 65535) return 65535;
  return (Bit16u) value;
}

/*
 * SaturateQwordUToDwordU converts  an unsigned 64-bit value to unsigned
 * 32-bit value. If the unsigned 64-bit value is greater than 4294967295,
 * it is represented by the saturated value 4294967295 (0xFFFFFFFF).
*/
BX_CPP_INLINE Bit32u BX_CPP_AttrRegparmN(1) SaturateQwordUToDwordU(Bit64u value)
{
  if(value > BX_CONST64(4294967295)) return BX_CONST64(4294967295);
  return (Bit32u) value;
}

#endif
