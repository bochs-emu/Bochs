/*---------------------------------------------------------------------------+
 |  exception.h                                                              |
 |  $Id: exception.h,v 1.6 2003-10-04 12:52:29 sshwarts Exp $
 |                                                                           |
 | Copyright (C) 1992    W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail   billm@vaxc.cc.monash.edu.au    |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#ifndef SW_C1
#include "fpu_emu.h"
#endif /* SW_C1 */

#define EX_ErrorSummary (0x0080)   /* Error summary status */
/* Special exceptions: */
#define EX_StackOver	(0x0041|SW_C1)	/* stack overflow */
#define EX_StackUnder	(0x0041)	/* stack underflow */
/* Exception flags: */
#define EX_Precision	(0x0020)	/* loss of precision */
#define EX_Underflow	(0x0010)	/* underflow */
#define EX_Overflow	(0x0008)	/* overflow */
#define EX_ZeroDiv	(0x0004)	/* divide by zero */
#define EX_Denormal	(0x0002)	/* denormalized operand */
#define EX_Invalid	(0x0001)	/* invalid operation */

#define PRECISION_LOST_UP    (EX_Precision | SW_C1)
#define PRECISION_LOST_DOWN  (EX_Precision)

#define EXCEPTION(x)    FPU_exception(x)
#define INTERNAL(x)     FPU_internal(x)

#endif /* _EXCEPTION_H_ */
