/////////////////////////////////////////////////////////////////////////
// $Id: status_w.h,v 1.8 2005-03-20 21:19:38 sshwarts Exp $
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//

#ifndef _STATUS_H_
#define _STATUS_H_

/* Status Word */
#define FPU_SW_Backward		(0x8000)  /* backward compatibility */
#define FPU_SW_C3	 	(0x4000)  /* condition bit 3 */
#define FPU_SW_Top		(0x3800)  /* top of stack */
#define FPU_SW_C2		(0x0400)  /* condition bit 2 */
#define FPU_SW_C1		(0x0200)  /* condition bit 1 */
#define FPU_SW_C0		(0x0100)  /* condition bit 0 */
#define FPU_SW_Summary   	(0x0080)  /* exception summary */
#define FPU_SW_Stack_Fault	(0x0040)  /* stack fault */
#define FPU_SW_Precision   	(0x0020)  /* loss of precision */
#define FPU_SW_Underflow   	(0x0010)  /* underflow */
#define FPU_SW_Overflow    	(0x0008)  /* overflow */
#define FPU_SW_Zero_Div    	(0x0004)  /* divide by zero */
#define FPU_SW_Denormal_Op   	(0x0002)  /* denormalized operand */
#define FPU_SW_Invalid    	(0x0001)  /* invalid operation */

#define FPU_SW_CC (FPU_SW_C0|FPU_SW_C1|FPU_SW_C2|FPU_SW_C3)

#define FPU_SW_Exceptions_Mask  (0x027f)  /* status word exceptions bit mask */

/* Exception flags: */
#define FPU_EX_Precision	(0x0020)  /* loss of precision */
#define FPU_EX_Underflow	(0x0010)  /* underflow */
#define FPU_EX_Overflow		(0x0008)  /* overflow */
#define FPU_EX_Zero_Div		(0x0004)  /* divide by zero */
#define FPU_EX_Denormal		(0x0002)  /* denormalized operand */
#define FPU_EX_Invalid		(0x0001)  /* invalid operation */

/* Special exceptions: */
#define FPU_EX_Stack_Overflow	(0x0041|FPU_SW_C1) 	/* stack overflow */
#define FPU_EX_Stack_Underflow	(0x0041)		/* stack underflow */

/* precision control */
#define FPU_EX_Precision_Lost_Up 	(EX_Precision | SW_C1)
#define FPU_EX_Precision_Lost_Dn        (EX_Precision)

/*
 * bbd: use do {...} while (0) structure instead of using curly brackets
 * inside parens, which most compilers do not like.
 */
#define setcc(cc) do { 				\
  FPU_PARTIAL_STATUS &= ~(FPU_SW_CC); 		\
  FPU_PARTIAL_STATUS |= (cc) & FPU_SW_CC; 	\
} while(0);

#define clear_C1() { FPU_PARTIAL_STATUS &= ~FPU_SW_C1; }
#define clear_C2() { FPU_PARTIAL_STATUS &= ~FPU_SW_C2; }

#endif
