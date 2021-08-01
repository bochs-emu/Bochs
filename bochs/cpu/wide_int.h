/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2020 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_WIDE_INT_H
#define BX_WIDE_INT_H

#include "config.h"

#ifdef BX_LITTLE_ENDIAN
typedef
  struct {
    Bit64u lo;
    Bit64u hi;
  } Bit128u;
typedef
  struct {
    Bit64u lo;
    Bit64s hi;
  } Bit128s;
#else   // must be Big Endian
typedef
  struct {
    Bit64u hi;
    Bit64u lo;
  } Bit128u;
typedef
  struct {
    Bit64s hi;
    Bit64u lo;
  } Bit128s;
#endif

extern void long_mul(Bit128u *product, Bit64u op1, Bit64u op2);
extern void long_imul(Bit128s *product, Bit64s op1, Bit64s op2);
extern void long_div(Bit128u *quotient,Bit64u *remainder,const Bit128u *dividend,Bit64u divisor);
extern void long_idiv(Bit128s *quotient,Bit64s *remainder,Bit128s *dividend,Bit64s divisor);
                                                               
#endif
