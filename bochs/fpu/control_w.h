/*---------------------------------------------------------------------------+
 |  control_w.h                                                              |
 |  $Id: control_w.h,v 1.4 2003-07-31 17:39:24 sshwarts Exp $
 |                                                                           |
 | Copyright (C) 1992,1993                                                   |
 |                       W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail   billm@vaxc.cc.monash.edu.au    |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#ifndef _CONTROLW_H_
#define _CONTROLW_H_

#define CW_RC		(0x0C00)	/* rounding control */
#define CW_PC		(0x0300)	/* precision control */

#define CW_Precision	(0x0020)	/* loss of precision mask */
#define CW_Underflow	(0x0010)	/* underflow mask */
#define CW_Overflow	(0x0008)	/* overflow mask */
#define CW_ZeroDiv	(0x0004)	/* divide by zero mask */
#define CW_Denormal	(0x0002)	/* denormalized operand mask */
#define CW_Invalid	(0x0001)	/* invalid operation mask */

#define CW_Exceptions  	(0x003f)	/* all masks */

#define RC_RND		(0x0000)
#define RC_DOWN		(0x0400)
#define RC_UP		(0x0800)
#define RC_CHOP		(0x0C00)

/* p 15-5: Precision control bits affect only the following:
   ADD, SUB(R), MUL, DIV(R), and SQRT */
#define PR_24_BITS        (0x000)
#define PR_53_BITS        (0x200)
#define PR_64_BITS        (0x300)
#define PR_RESERVED_BITS  (0x100)
/* FULL_PRECISION simulates all exceptions masked */
#define FULL_PRECISION  (PR_64_BITS | RC_RND | 0x3f)

#endif /* _CONTROLW_H_ */
