/*---------------------------------------------------------------------------+
 |  fpu_tags.c                                                               |
 |  $Id: fpu_tags.c,v 1.6 2003-08-07 18:54:03 sshwarts Exp $
 |                                                                           |
 |  Set FPU register tags.                                                   |
 |                                                                           |
 | Copyright (C) 1997                                                        |
 |                  W. Metzenthen, 22 Parker St, Ormond, Vic 3163, Australia |
 |                  E-mail   billm@jacobi.maths.monash.edu.au                |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "fpu_emu.h"
#include "fpu_system.h"
#include "exception.h"


void FPU_pop(void)
{
  FPU_tag_word |= 3 << ((FPU_tos & 7)*2);
  FPU_tos++;
}


int FPU_gettag0(void)
{
  return (FPU_tag_word >> ((FPU_tos & 7)*2)) & 3;
}


int  BX_CPP_AttrRegparmN(1)
FPU_gettagi(int stnr)
{
  return (FPU_tag_word >> (((FPU_tos+stnr) & 7)*2)) & 3;
}


int  BX_CPP_AttrRegparmN(1)
FPU_gettag(int regnr)
{
  return (FPU_tag_word >> ((regnr & 7)*2)) & 3;
}


void  BX_CPP_AttrRegparmN(1)
FPU_settag0(int tag)
{
  int regnr = FPU_tos;
  regnr &= 7;
  FPU_tag_word &= ~(3 << (regnr*2));
  FPU_tag_word |= (tag & 3) << (regnr*2);
}


void  BX_CPP_AttrRegparmN(2)
FPU_settagi(int stnr, int tag)
{
  int regnr = stnr+FPU_tos;
  regnr &= 7;
  FPU_tag_word &= ~(3 << (regnr*2));
  FPU_tag_word |= (tag & 3) << (regnr*2);
}


void  BX_CPP_AttrRegparmN(2)
FPU_settag(int regnr, int tag)
{
  regnr &= 7;
  FPU_tag_word &= ~(3 << (regnr*2));
  FPU_tag_word |= (tag & 3) << (regnr*2);
}


int  BX_CPP_AttrRegparmN(1)
FPU_Special(FPU_REG const *ptr)
{
  int exp = exponent(ptr);

  if ( exp == EXP_BIAS+EXP_UNDER )
    return TW_Denormal;
  else if ( exp != EXP_BIAS+EXP_OVER )
    return TW_NaN;
  else if ( (ptr->sigh == 0x80000000) && (ptr->sigl == 0) )
    return TW_Infinity;
  return TW_NaN;
}


int  BX_CPP_AttrRegparmN(1)
isNaN(FPU_REG const *ptr)
{
  return ( (exponent(ptr) == EXP_BIAS+EXP_OVER)
	   && !((ptr->sigh == 0x80000000) && (ptr->sigl == 0)) );
}


int  BX_CPP_AttrRegparmN(1)
FPU_empty_i(int stnr)
{
  int regnr = (FPU_tos+stnr) & 7;

  return ((FPU_tag_word >> (regnr*2)) & 3) == TAG_Empty;
}


int FPU_stackoverflow(FPU_REG **st_new_ptr)
{
  *st_new_ptr = &st(-1);

  return ((FPU_tag_word >> (((FPU_tos - 1) & 7)*2)) & 3) != TAG_Empty;
}


void  BX_CPP_AttrRegparmN(3)
FPU_copy_to_regi(FPU_REG const *r, u_char tag, int stnr)
{
  reg_copy(r, &st(stnr));
  FPU_settagi(stnr, tag);
}

void  BX_CPP_AttrRegparmN(2)
FPU_copy_to_reg1(FPU_REG const *r, u_char tag)
{
  reg_copy(r, &st(1));
  FPU_settagi(1, tag);
}

void  BX_CPP_AttrRegparmN(2)
FPU_copy_to_reg0(FPU_REG const *r, u_char tag)
{
  reg_copy(r, &st(0));
  FPU_settagi(0, tag);
}
