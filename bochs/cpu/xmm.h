#ifndef BX_SSE_EXTENSIONS_H 
#define BX_SSE_EXTENSIONS_H

/* XMM REGISTER */

typedef Bit32u Float32;
typedef Bit64u Float64;

typedef union bx_xmm_reg_t {
   Bit8s   _sbyte[16];
   Bit16s  _s16[8];
   Bit32s  _s32[4];
   Bit64s  _s64[2];
   Bit8u   _ubyte[16];
   Bit16u  _u16[8];
   Bit32u  _u32[4];
   Bit64u  _u64[2];
} BxPackedXmmRegister;

#ifdef BX_SUPPORT_X86_64
#  define BX_XMM_REGISTERS 16
#else
#  define BX_XMM_REGISTERS 8
#endif

#ifdef BX_BIG_ENDIAN
#define xmm64s(i)   _s64[1 - (i)]
#define xmm32s(i)   _s32[3 - (i)]
#define xmm16s(i)   _s16[7 - (i)]
#define xmmsbyte(i) _sbyte[15 - (i)]
#define xmmubyte(i) _ubyte[15 - (i)]
#define xmm16u(i)   _u16[7 - (i)]
#define xmm32u(i)   _u32[3 - (i)]
#define xmm64u(i)   _u64[1 - (i)]
#else
#define xmm64s(i)   _s64[(i)]
#define xmm32s(i)   _s32[(i)]
#define xmm16s(i)   _s16[(i)]
#define xmmsbyte(i) _sbyte[(i)]
#define xmmubyte(i) _ubyte[(i)]
#define xmm16u(i)   _u16[(i)]
#define xmm32u(i)   _u32[(i)]
#define xmm64u(i)   _u64[(i)]
#endif

/* floating point representation: single and double precission */
#define xmm32f(i)   xmm32u(i)
#define xmm64f(i)   xmm64u(i)

#define BX_READ_XMM_REG(index) (BX_CPU_THIS_PTR xmm[index])
#define BX_WRITE_XMM_REG(index, reg) { BX_CPU_THIS_PTR xmm[index] = reg; }
 
/* MXCSR REGISTER */

/* 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16
 * ==|==|=====|==|==|==|==|==|==|==|==|==|==|==|==  (reserved)
 *  0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0| 0
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
 */

struct bx_mxcsr_t {
   Bit32u mxcsr;      /* define bitfields accessors later */
};

#define MXCSR_MASK  0x00ff  /* reset reserved bits */
#define MXCSR_RESET 0x1f80  /* reset value of the MXCSR register */

/* INTEGER SATURATION */

/*
  SaturateWordSToByteS   converts   a signed 16-bit value to a
  signed  8-bit  value. If the signed 16-bit value is less than -128, it
  is  represented  by  the saturated value -128 (0x80). If it is greater
  than 127, it is represented by the saturated value 127 (0x7F).
*/
Bit8s SaturateWordSToByteS(Bit16s value);

/*
  SaturateDwordSToWordS  converts  a  signed 32-bit value to a
  signed  16-bit  value. If the signed 32-bit value is less than -32768,
  it  is  represented  by  the saturated value -32768 (0x8000). If it is
  greater  than  32767,  it  is represented by the saturated value 32767
  (0x7FFF).
*/
Bit16s SaturateDwordSToWordS(Bit32s value);

/*
  SaturateWordSToByteU  converts a signed 16-bit value to an
  unsigned  8-bit value. If the signed 16-bit value is less than zero it
  is  represented  by  the  saturated value zero (0x00).If it is greater
  than 255 it is represented by the saturated value 255 (0xFF).
*/
Bit8u SaturateWordSToByteU(Bit16s value);

/*
  SaturateDwordSToWordU  converts  a signed 32-bit value
  to  an  unsigned  16-bit value. If the signed 32-bit value is less
  than   zero,   it   is  represented  by  the saturated value 65535
  (0x0000).  If  it  is greater  than  65535,  it  is represented by
  the saturated value 65535 (0xFFFF).
*/
Bit16u SaturateDwordSToWordU(Bit32s value);

#endif
