/////////////////////////////////////////////////////////////////////////
// $Id: hostasm.h,v 1.1 2004-02-15 17:57:44 cbothamy Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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


#ifndef BX_HOSTASM_H
#  define BX_HOSTASM_H 1

// This file defines some convience inline functions which do the
// dirty work of asm() statements for arithmetic instructions on x86 hosts. 
// Essentially these speed up eflags processing since the value of the
// eflags register can be read directly on x86 hosts, after the
// arithmetic operations.

// gcc on i386
#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)

#define BX_HostAsm_Add16
  static inline void
asmAdd16(Bit16u &sum_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
{
  asm (
    "addw %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (sum_16)
    : "1" (op1_16), "g" (op2_16)
    : "cc"
    );
}

#define BX_HostAsm_Add32
  static inline void
asmAdd32(Bit32u &sum_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
{
  asm (
    "addl %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (sum_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
}

#define BX_HostAsm_Sub16
  static inline void
asmSub16(Bit16u &diff_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
{
  asm (
    "subw %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (diff_16)
    : "1" (op1_16), "g" (op2_16)
    : "cc"
    );
}

#define BX_HostAsm_Sub32
  static inline void
asmSub32(Bit32u &diff_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
{
  asm (
    "subl %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=r" (diff_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
}

#define BX_HostAsm_Cmp8
  static inline void
asmCmp8(Bit8u op1_8, Bit8u op2_8, Bit32u &flags32)
{
  asm (
    "cmpb %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "q" (op1_8), "mq" (op2_8)
    : "cc"
    );
}

#define BX_HostAsm_Cmp16
  static inline void
asmCmp16(Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
{
  asm (
    "cmpw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
}

#define BX_HostAsm_Cmp32
  static inline void
asmCmp32(Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
{
  asm (
    "cmpl %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_32), "g" (op2_32)
    : "cc"
    );
}

#define BX_HostAsm_Inc16
  static inline void
asmInc16(Bit16u &op1_16, Bit32u &flags32)
{
  asm (
    "incw %1 \n\t"
    "pushfl  \n\t"
    "popl   %0"
    : "=g" (flags32), "=g" (op1_16)
    : "1" (op1_16)
    : "cc"
    );
}

#define BX_HostAsm_Inc32
  static inline void
asmInc32(Bit32u &op1_32, Bit32u &flags32)
{
  asm (
    "incl %1 \n\t"
    "pushfl  \n\t"
    "popl   %0"
    : "=g" (flags32), "=g" (op1_32)
    : "1" (op1_32)
    : "cc"
    );
}

#define BX_HostAsm_Dec16
  static inline void
asmDec16(Bit16u &op1_16, Bit32u &flags32)
{
  asm (
    "decw %1 \n\t"
    "pushfl  \n\t"
    "popl   %0"
    : "=g" (flags32), "=g" (op1_16)
    : "1" (op1_16)
    : "cc"
    );
}

#define BX_HostAsm_Dec32
  static inline void
asmDec32(Bit32u &op1_32, Bit32u &flags32)
{
  asm (
    "decl %1 \n\t"
    "pushfl  \n\t"
    "popl   %0"
    : "=g" (flags32), "=g" (op1_32)
    : "1" (op1_32)
    : "cc"
    );
}

#define BX_HostAsm_Xor16
  static inline void
asmXor16(Bit16u &result_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
{
  asm (
    "xorw %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (result_16)
    : "1" (op1_16), "g" (op2_16)
    : "cc"
    );
}

#define BX_HostAsm_Xor32
  static inline void
asmXor32(Bit32u &result_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
{
  asm (
    "xorl %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (result_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
}

#define BX_HostAsm_Or8
  static inline void
asmOr8(Bit8u &result_8, Bit8u op1_8, Bit8u op2_8, Bit32u &flags32)
{
  asm (
    "orb %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=q" (result_8)
    : "1" (op1_8), "mq" (op2_8)
    : "cc"
    );
}

#define BX_HostAsm_Or16
  static inline void
asmOr16(Bit16u &result_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
{
  asm (
    "orw %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (result_16)
    : "1" (op1_16), "g" (op2_16)
    : "cc"
    );
}

#define BX_HostAsm_Or32
  static inline void
asmOr32(Bit32u &result_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
{
  asm (
    "orl %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (result_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
}

#define BX_HostAsm_And8
  static inline void
asmAnd8(Bit8u &result_8, Bit8u op1_8, Bit8u op2_8, Bit32u &flags32)
{
  asm (
    "andb %3, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=q" (result_8)
    : "1" (op1_8), "mq" (op2_8)
    : "cc"
    );
}

#define BX_HostAsm_And16
  static inline void
asmAnd16(Bit16u &result_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
{
  asm (
    "andw %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (result_16)
    : "1" (op1_16), "g" (op2_16)
    : "cc"
    );
}

#define BX_HostAsm_And32
  static inline void
asmAnd32(Bit32u &result_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
{
  asm (
    "andl %3, %1 \n\t"
    "pushfl      \n\t"
    "popl   %0"
    : "=g" (flags32), "=r" (result_32)
    : "1" (op1_32), "g" (op2_32)
    : "cc"
    );
}

#define BX_HostAsm_Test8
  static inline void
asmTest8(Bit8u op1_8, Bit8u op2_8, Bit32u &flags32)
{
  asm (
    "testb %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "q" (op1_8), "mq" (op2_8)
    : "cc"
    );
}

#define BX_HostAsm_Test16
  static inline void
asmTest16(Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
{
  asm (
    "testw %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_16), "g" (op2_16)
    : "cc"
    );
}

#define BX_HostAsm_Test32
  static inline void
asmTest32(Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
{
  asm (
    "testl %2, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32)
    : "r" (op1_32), "g" (op2_32)
    : "cc"
    );
}

#define BX_HostAsm_Shr16
  static inline void
asmShr16(Bit16u &result_16, Bit16u op1_16, unsigned count, Bit32u &flags32)
{
  asm (
    "shrw %%cl, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=g" (result_16)
    : "1" (op1_16), "c" (count)
    : "cc"
    );
}

#define BX_HostAsm_Shr32
  static inline void
asmShr32(Bit32u &result_32, Bit32u op1_32, unsigned count, Bit32u &flags32)
{
  asm (
    "shrl %%cl, %1\n\t"
    "pushfl     \n\t"
    "popl %0"
    : "=g" (flags32), "=g" (result_32)
    : "1" (op1_32), "c" (count)
    : "cc"
    );
}

#endif // (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)


// msvc++ on i386
#if (defined(_M_IX86) && defined(_MSC_VER) && BX_SupportHostAsms)

#define BX_HostAsm_Add16
// asmAdd16(Bit16u &sum_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
#define asmAdd16(sum_16,op1_16,op2_16,flags32)	\
	__asm {	\
		__asm	mov ax,op1_16	\
		__asm	add ax,op2_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov sum_16,ax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Add32
// asmAdd32(Bit32u &sum_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
#define asmAdd32(sum_32,op1_32,op2_32,flags32)	\
	__asm {	\
		__asm	mov eax,op1_32	\
		__asm	add eax,op2_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov sum_32,eax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Sub16
// asmSub16(Bit16u &diff_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
#define asmSub16(diff_16,op1_16,op2_16,flags32)	\
	__asm {	\
		__asm	mov ax,op1_16	\
		__asm	sub ax,op2_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov diff_16,ax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Sub32
// asmSub32(Bit32u &diff_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
#define asmSub32(diff_32,op1_32,op2_32,flags32)	\
	__asm {	\
		__asm	mov eax,op1_32	\
		__asm	sub eax,op2_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov diff_32,eax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Cmp8
// asmCmp8(Bit8u op1_8, Bit8u op2_8, Bit32u &flags32)
#define asmCmp8(op1_8,op2_8,flags32)	\
	__asm {	\
		__asm	mov al,op1_8	\
		__asm	cmp al,op2_8	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Cmp16
// asmCmp16(Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
#define asmCmp16(op1_16,op2_16,flags32)	\
	__asm {	\
		__asm	mov ax,op1_16	\
		__asm	cmp ax,op2_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Cmp32
// asmCmp32(Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
#define asmCmp32(op1_32,op2_32,flags32)	\
	__asm {	\
		__asm	mov eax,op1_32	\
		__asm	cmp eax,op2_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

// FIXME: cannot compile arith16.cc because of 'union'.
//#define BX_HostAsm_Inc16
// asmInc16(Bit16u &op1_16, Bit32u &flags32)
#define asmInc16(op1_16,flags32)	\
	__asm {	\
		__asm	inc word ptr op1_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

// FIXME: cannot compile arith32.cc because of 'union'.
//#define BX_HostAsm_Inc32
// asmInc32(Bit32u &op1_32, Bit32u &flags32)
#define asmInc32(op1_32,flags32)	\
	__asm {	\
		__asm	inc dword ptr op1_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

// FIXME: cannot compile arith16.cc because of 'union'.
//#define BX_HostAsm_Dec16
// asmDec16(Bit16u &op1_16, Bit32u &flags32)
#define asmDec16(op1_16,flags32)	\
	__asm {	\
		__asm	dec word ptr op1_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

// FIXME: cannot compile arith32.cc because of 'union'.
//#define BX_HostAsm_Dec32
// asmDec32(Bit32u &op1_32, Bit32u &flags32)
#define asmDec32(op1_32,flags32)	\
	__asm {	\
		__asm	dec dword ptr op1_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Xor16
// asmXor16(Bit16u &result_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
#define asmXor16(result_16,op1_16,op2_16,flags32)	\
	__asm {	\
		__asm	mov ax,op1_16	\
		__asm	xor ax,op2_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_16,ax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Xor32
// asmXor32(Bit32u &result_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
#define asmXor32(result_32,op1_32,op2_32,flags32)	\
	__asm {	\
		__asm	mov eax,op1_32	\
		__asm	xor eax,op2_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_32,eax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Or8
// asmOr8(Bit8u &result_8, Bit8u op1_8, Bit8u op2_8, Bit32u &flags32)
#define asmOr8(result_8,op1_8,op2_8,flags32)	\
	__asm {	\
		__asm	mov al,op1_8	\
		__asm	or al,op2_8	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_8,al	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Or16
// asmOr16(Bit16u &result_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
#define asmOr16(result_16,op1_16,op2_16,flags32)	\
	__asm {	\
		__asm	mov ax,op1_16	\
		__asm	or ax,op2_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_16,ax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Or32
// asmOr32(Bit32u &result_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
#define asmOr32(result_32,op1_32,op2_32,flags32)	\
	__asm {	\
		__asm	mov eax,op1_32	\
		__asm	or eax,op2_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_32,eax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_And8
// asmAnd8(Bit8u &result_8, Bit8u op1_8, Bit8u op2_8, Bit32u &flags32)
#define asmAnd8(result_8,op1_8,op2_8,flags32)	\
	__asm {	\
		__asm	mov al,op1_8	\
		__asm	and al,op2_8	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_8,al	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_And16
// asmAnd16(Bit16u &result_16, Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
#define asmAnd16(result_16,op1_16,op2_16,flags32)	\
	__asm {	\
		__asm	mov ax,op1_16	\
		__asm	and ax,op2_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_16,ax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_And32
// asmAnd32(Bit32u &result_32, Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
#define asmAnd32(result_32,op1_32,op2_32,flags32)	\
	__asm {	\
		__asm	mov eax,op1_32	\
		__asm	and eax,op2_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_32,eax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Test8
// asmTest8(Bit8u op1_8, Bit8u op2_8, Bit32u &flags32)
#define asmTest8(op1_8,op2_8,flags32)	\
	__asm {	\
		__asm	mov al,op1_8	\
		__asm	test al,op2_8	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Test16
// asmTest16(Bit16u op1_16, Bit16u op2_16, Bit32u &flags32)
#define asmTest16(op1_16,op2_16,flags32)	\
	__asm {	\
		__asm	mov ax,op1_16	\
		__asm	test ax,op2_16	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Test32
// asmTest32(Bit32u op1_32, Bit32u op2_32, Bit32u &flags32)
#define asmTest32(op1_32,op2_32,flags32)	\
	__asm {	\
		__asm	mov eax,op1_32	\
		__asm	test eax,op2_32	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Shr16
// asmShr16(Bit16u &result_16, Bit16u op1_16, unsigned count, Bit32u &flags32)
#define asmShr16(result_16,op1_16,count,flags32)	\
	__asm {	\
		__asm	mov ax,op1_16	\
		__asm	mov cl,byte ptr count	\
		__asm	shr ax,cl	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_16,ax	\
		__asm	mov flags32,edx	\
	}

#define BX_HostAsm_Shr32
// asmShr32(Bit32u &result_32, Bit32u op1_32, unsigned count, Bit32u &flags32)
#define asmShr32(result_32,op1_32,count,flags32)	\
	__asm {	\
		__asm	mov eax,op1_32	\
		__asm	mov cl,byte ptr count	\
		__asm	shr eax,cl	\
		__asm	pushfd	\
		__asm	pop edx	\
		__asm	mov result_32,eax	\
		__asm	mov flags32,edx	\
	}

#endif // (defined(_M_IX86) && defined(_MSC_VER) && BX_SupportHostAsms)

#endif // BX_HOSTASM_H
