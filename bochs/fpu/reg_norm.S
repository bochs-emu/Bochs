/*---------------------------------------------------------------------------+
 |  reg_norm.S                                                               |
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1995,1997,1999                               |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail billm@melbpc.org.au              |
 |                                                                           |
 | Normalize the value in a FPU_REG.                                         |
 |                                                                           |
 | Call from C as:                                                           |
 |    int FPU_normalize_nuo(FPU_REG *n, int bias)                            |
 |                                                                           |
 |    Return value is the tag of the answer.                                 |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "fpu_emu.h"


/* Normalise without reporting underflow or overflow */
ENTRY(FPU_normalize_nuo)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%ebx

	movl	PARAM1,%ebx

	movl	SIGH(%ebx),%edx
	movl	SIGL(%ebx),%eax

	orl	%edx,%edx	/* ms bits */
	js	L_exit_nuo_valid	/* Already normalized */
	jnz	L_nuo_shift_1	/* Shift left 1 - 31 bits */

	orl	%eax,%eax
	jz	L_exit_nuo_zero		/* The contents are zero */

	movl	%eax,%edx
	xorl	%eax,%eax
	subw	$32,EXP(%ebx)	/* This can cause an underflow */

/* We need to shift left by 1 - 31 bits */
L_nuo_shift_1:
	bsrl	%edx,%ecx	/* get the required shift in %ecx */
	subl	$31,%ecx
	negl	%ecx
	shld	%cl,%eax,%edx
	shl	%cl,%eax
	subw	%cx,EXP(%ebx)	/* This can cause an underflow */

	movl	%edx,SIGH(%ebx)
	movl	%eax,SIGL(%ebx)

L_exit_nuo_valid:
        movl    PARAM2,%eax
        addw    %ax,EXP(%ebx)
	movl	TAG_Valid,%eax

	popl	%ebx
	leave
	ret

L_exit_nuo_zero:
	movw	EXP_UNDER,EXP(%ebx)
        movl    PARAM2,%eax
        addw    %ax,EXP(%ebx)
        movl    TAG_Zero,%eax

	popl	%ebx
	leave
	ret
