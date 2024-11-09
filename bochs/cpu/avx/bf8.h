/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024 Stanislav Shwartsman
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

#ifndef BX_BF8_HELPER
#define BX_BF8_HELPER

// TF32: s|eeeeeeee|mmmmmmmmmm-------------
// FP32: s|eeeeeeee|mmmmmmmmmmmmmmmmmmmmmmm
// BF16: s|eeeeeeee|mmmmmmm
//  F16: s|eeeee|mmmmmmmmmm
//  BF8: s|eeeee|mm                          (E5M2)
//  HF8: s|eeee|mmm                          (E4M3)

typedef Bit8u float_bf8;

// E5M2
BX_CPP_INLINE int bf8_sign(float_bf8 bf8) { return bf8 >> 7; }
BX_CPP_INLINE int bf8_exp(float_bf8 bf8) { return (bf8 & 0x7c) >> 2; }
BX_CPP_INLINE int bf8_signif(float_bf8 bf8) { return bf8 & 0x03; }

BX_CPP_INLINE float16 convert_bf8_to_fp16(float_bf8 op)
{
  return Bit16u(op) << 8;
}

#endif
