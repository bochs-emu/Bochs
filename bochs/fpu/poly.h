/*---------------------------------------------------------------------------+
 |  poly.h                                                                   |
 |  $Id: poly.h,v 1.3.8.1 2002-03-17 08:51:21 bdenney Exp $
 |                                                                           |
 |  Header file for the FPU-emu poly*.c source files.                        |
 |                                                                           |
 | Copyright (C) 1994,1999                                                   |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail   billm@melbpc.org.au            |
 |                                                                           |
 | Declarations and definitions for functions operating on Xsig (12-byte     |
 | extended-significand) quantities.                                         |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#ifndef _POLY_H
#define _POLY_H

/* This 12-byte structure is used to improve the accuracy of computation
   of transcendental functions.
   Intended to be used to get results better than 8-byte computation
   allows. 9-byte would probably be sufficient.
   */
typedef struct {
#ifdef EMU_BIG_ENDIAN
  u32 msw;
  u32 midw;
  u32 lsw;
#else
  u32 lsw;
  u32 midw;
  u32 msw;
#endif
} GCC_ATTRIBUTE((packed)) Xsig;

asmlinkage void mul64(u64 const *a, u64 const *b,
		      u64 *result);
asmlinkage void polynomial_Xsig(Xsig *, const u64 *x,
				const u64 terms[], const int n);

asmlinkage void mul32_Xsig(Xsig *, const u32 mult);
asmlinkage void mul64_Xsig(Xsig *, const u64 *mult);
asmlinkage void mul_Xsig_Xsig(Xsig *dest, const Xsig *mult);

asmlinkage void shr_Xsig(Xsig *, const int n);
asmlinkage int round_Xsig(Xsig *);
asmlinkage int norm_Xsig(Xsig *);
asmlinkage void div_Xsig(const Xsig *x1, const Xsig *x2, Xsig *dest);

/* Macro to extract the most significant 32 bits from a 64bit quantity */
#ifdef EMU_BIG_ENDIAN
#define LL_MSW(x)     (((u32 *)&x)[0])
#else
#define LL_MSW(x)     (((u32 *)&x)[1])
#endif

/* Macro to initialize an Xsig struct */
#ifdef EMU_BIG_ENDIAN
#define MK_XSIG(a,b,c)     { a, b, c }
#else
#define MK_XSIG(a,b,c)     { c, b, a }
#endif

/* Macro to access the 8 ms bytes of an Xsig as a 64bit quantity */
#ifdef EMU_BIG_ENDIAN
#define XSIG_LL(x)         (*(u64 *)&x.msw)
#else
#define XSIG_LL(x)         (*(u64 *)&x.midw)
#endif


/*
   Need to run gcc with optimizations on to get these to
   actually be in-line.
   */

/* Multiply two fixed-point 32 bit numbers, producing a 32 bit result.
   The answer is the ms word of the product.  */
BX_C_INLINE
u32 mul_32_32(const u32 arg1, const u32 arg2)
{
#ifdef NO_ASSEMBLER
  return (((u64)arg1) * arg2) >> 32;
#else
/* Some versions of gcc make it difficult to stop eax from being clobbered.
   Merely specifying that it is used doesn't work...
 */
  int retval;
  asm volatile ("mull %2; movl %%edx,%%eax" \
		:"=a" (retval) \
		:"0" (arg1), "g" (arg2) \
		:"dx");
  return retval;
#endif
}


/* Add the 12 byte Xsig x2 to Xsig dest, with no checks for overflow. */
BX_C_INLINE
void add_Xsig_Xsig(Xsig *dest, const Xsig *x2)
{
#ifdef NO_ASSEMBLER
  dest->lsw += x2->lsw;
  if ( dest->lsw < x2->lsw )
    {
      dest->midw ++;
      if ( dest->midw == 0 )
	dest->msw ++;
    }
  dest->midw += x2->midw;
  if ( dest->midw < x2->midw )
    {
      dest->msw ++;
    }
  dest->msw += x2->msw;
#else
  asm volatile ("movl %1,%%edi; movl %2,%%esi;
                 movl (%%esi),%%eax; addl %%eax,(%%edi);
                 movl 4(%%esi),%%eax; adcl %%eax,4(%%edi);
                 movl 8(%%esi),%%eax; adcl %%eax,8(%%edi);"
                 :"=g" (*dest):"g" (dest), "g" (x2)
                 :"ax","si","di");
#endif
}


/* Add the 12 byte Xsig x2 to Xsig dest, adjust exp if overflow occurs. */
BX_C_INLINE
void add_two_Xsig(Xsig *dest, const Xsig *x2, s32 *exp)
{
#ifdef NO_ASSEMBLER
  int ovfl = 0;

  dest->lsw += x2->lsw;
  if ( dest->lsw < x2->lsw )
    {
      dest->midw ++;
      if ( dest->midw == 0 )
	{
	  dest->msw ++;
	  if ( dest->msw == 0 )
	    ovfl = 1;
	}
    }
  dest->midw += x2->midw;
  if ( dest->midw < x2->midw )
    {
      dest->msw ++;
      if ( dest->msw == 0 )
	ovfl = 1;
    }
  dest->msw += x2->msw;
  if ( dest->msw < x2->msw )
    ovfl = 1;
  if ( ovfl )
    {
      (*exp) ++;
      dest->lsw >>= 1;
      if ( dest->midw & 1 )
	dest->lsw |= 0x80000000;
      dest->midw >>= 1;
      if ( dest->msw & 1 )
	dest->midw |= 0x80000000;
      dest->msw >>= 1;
      dest->msw |= 0x80000000;
    }
#else
/* Note: the constraints in the asm statement didn't always work properly
   with gcc 2.5.8.  Changing from using edi to using ecx got around the
   problem, but keep fingers crossed! */
  asm volatile ("movl %2,%%ecx; movl %3,%%esi;
                 movl (%%esi),%%eax; addl %%eax,(%%ecx);
                 movl 4(%%esi),%%eax; adcl %%eax,4(%%ecx);
                 movl 8(%%esi),%%eax; adcl %%eax,8(%%ecx);
                 jnc 0f;
		 rcrl 8(%%ecx); rcrl 4(%%ecx); rcrl (%%ecx)
                 movl %4,%%ecx; incl (%%ecx)
                 movl $1,%%eax; jmp 1f;
                 0: xorl %%eax,%%eax;
                 1:"
		:"=g" (*exp), "=g" (*dest)
		:"g" (dest), "g" (x2), "g" (exp)
		:"cx","si","ax");
#endif
}


/* Negate the 12 byte Xsig */
BX_C_INLINE
void negate_Xsig(Xsig *x)
{
#ifdef NO_ASSEMBLER
  x->lsw = ~x->lsw;
  x->midw = ~x->midw;
  x->msw = ~x->msw;
  x->lsw ++;
  if ( x->lsw == 0 )
    {
      x->midw ++;
      if ( x->midw == 0 )
	x->msw ++;
    }
#else
/* Negate (subtract from 1.0) the 12 byte Xsig */
/* This is faster in a loop on my 386 than using the "neg" instruction. */
  asm volatile("movl %1,%%esi; "
               "xorl %%ecx,%%ecx; "
               "movl %%ecx,%%eax; subl (%%esi),%%eax; movl %%eax,(%%esi); "
               "movl %%ecx,%%eax; sbbl 4(%%esi),%%eax; movl %%eax,4(%%esi); "
               "movl %%ecx,%%eax; sbbl 8(%%esi),%%eax; movl %%eax,8(%%esi); "
               :"=g" (*x):"g" (x):"si","ax","cx");
#endif
}


#endif /* _POLY_H */
