#ifndef BX_SSE_EXTENSIONS_H
#define BX_SSE_EXTENSIONS_H

/* XMM REGISTER */

typedef Bit32u Float32;
typedef Bit64u Float64;

typedef union bx_xmm_reg_t {
   Bit8s   s8[16];
   Bit16s  s16[8];
   Bit32s  s32[4];
   Bit64s  s64[2];
   Bit8u   u8[16];
   Bit16u  u16[8];
   Bit32u  u32[4];
   Bit64u  u64[2];
   Float32 f32[4];
   Float64 f64[2];
} BxPackedXmmRegister;

#ifdef BX_SUPPORT_X86_64
#  define BX_XMM_REGISTERS 16
#else
#  define BX_XMM_REGISTERS 8
#endif
 
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

#define MXCSR_MASK  0x00FF  /* reset reserved bits */
#define MXCSR_RESET 0x1F80  /* reset value of the MXCSR register */

#endif
