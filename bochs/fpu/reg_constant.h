/*---------------------------------------------------------------------------+
 |  reg_constant.h                                                           |
 |  $Id: reg_constant.h,v 1.3 2001-10-06 03:53:46 bdenney Exp $
 |                                                                           |
 | Copyright (C) 1992    W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail   billm@vaxc.cc.monash.edu.au    |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#ifndef _REG_CONSTANT_H_
#define _REG_CONSTANT_H_

#include "fpu_emu.h"

extern FPU_REG const CONST_1;
extern FPU_REG const CONST_2;
extern FPU_REG const CONST_HALF;
extern FPU_REG const CONST_L2T;
extern FPU_REG const CONST_L2E;
extern FPU_REG const CONST_PI;
// bbd: make CONST_PI2 non-const so that you can write "&CONST_PI2" when
// calling a function.  Otherwise you get const warnings.  Surely there's
// a better way.
extern FPU_REG CONST_PI2;
extern FPU_REG const CONST_PI2extra;
extern FPU_REG const CONST_PI4;
extern FPU_REG const CONST_LG2;
extern FPU_REG const CONST_LN2;
extern FPU_REG const CONST_Z;
extern FPU_REG const CONST_PINF;
extern FPU_REG const CONST_INF;
extern FPU_REG const CONST_MINF;
extern FPU_REG const CONST_QNaN;

#endif /* _REG_CONSTANT_H_ */
