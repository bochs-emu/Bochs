/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2016-2023  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_X86_DECODER_H
#define BX_X86_DECODER_H

// x86 Arch features
enum x86_feature_name {
#define x86_feature(isa, feature_name) isa,
#include "features.h"
  BX_ISA_EXTENSION_LAST
};
#undef x86_feature

#define BX_ISA_EXTENSIONS_ARRAY_SIZE (4)

#if (BX_ISA_EXTENSION_LAST) >= (BX_ISA_EXTENSIONS_ARRAY_SIZE*32)
  #error "ISA extensions array limit exceeded!"
#endif

// segment register encoding
enum BxSegregs {
  BX_SEG_REG_ES = 0,
  BX_SEG_REG_CS = 1,
  BX_SEG_REG_SS = 2,
  BX_SEG_REG_DS = 3,
  BX_SEG_REG_FS = 4,
  BX_SEG_REG_GS = 5,
  // NULL now has to fit in 3 bits.
  BX_SEG_REG_NULL = 7
};

#define BX_NULL_SEG_REG(seg) ((seg) == BX_SEG_REG_NULL)

enum BxRegs8L {
  BX_8BIT_REG_AL,
  BX_8BIT_REG_CL,
  BX_8BIT_REG_DL,
  BX_8BIT_REG_BL,
  BX_8BIT_REG_SPL,
  BX_8BIT_REG_BPL,
  BX_8BIT_REG_SIL,
  BX_8BIT_REG_DIL,
#if BX_SUPPORT_X86_64
  BX_8BIT_REG_R8,
  BX_8BIT_REG_R9,
  BX_8BIT_REG_R10,
  BX_8BIT_REG_R11,
  BX_8BIT_REG_R12,
  BX_8BIT_REG_R13,
  BX_8BIT_REG_R14,
  BX_8BIT_REG_R15
#endif
};

enum BxRegs8H {
  BX_8BIT_REG_AH,
  BX_8BIT_REG_CH,
  BX_8BIT_REG_DH,
  BX_8BIT_REG_BH
};

enum BxRegs16 {
  BX_16BIT_REG_AX,
  BX_16BIT_REG_CX,
  BX_16BIT_REG_DX,
  BX_16BIT_REG_BX,
  BX_16BIT_REG_SP,
  BX_16BIT_REG_BP,
  BX_16BIT_REG_SI,
  BX_16BIT_REG_DI,
#if BX_SUPPORT_X86_64
  BX_16BIT_REG_R8,
  BX_16BIT_REG_R9,
  BX_16BIT_REG_R10,
  BX_16BIT_REG_R11,
  BX_16BIT_REG_R12,
  BX_16BIT_REG_R13,
  BX_16BIT_REG_R14,
  BX_16BIT_REG_R15,
#endif
};

enum BxRegs32 {
  BX_32BIT_REG_EAX,
  BX_32BIT_REG_ECX,
  BX_32BIT_REG_EDX,
  BX_32BIT_REG_EBX,
  BX_32BIT_REG_ESP,
  BX_32BIT_REG_EBP,
  BX_32BIT_REG_ESI,
  BX_32BIT_REG_EDI,
#if BX_SUPPORT_X86_64
  BX_32BIT_REG_R8,
  BX_32BIT_REG_R9,
  BX_32BIT_REG_R10,
  BX_32BIT_REG_R11,
  BX_32BIT_REG_R12,
  BX_32BIT_REG_R13,
  BX_32BIT_REG_R14,
  BX_32BIT_REG_R15,
#endif
};

#if BX_SUPPORT_X86_64
enum BxRegs64 {
  BX_64BIT_REG_RAX,
  BX_64BIT_REG_RCX,
  BX_64BIT_REG_RDX,
  BX_64BIT_REG_RBX,
  BX_64BIT_REG_RSP,
  BX_64BIT_REG_RBP,
  BX_64BIT_REG_RSI,
  BX_64BIT_REG_RDI,
  BX_64BIT_REG_R8,
  BX_64BIT_REG_R9,
  BX_64BIT_REG_R10,
  BX_64BIT_REG_R11,
  BX_64BIT_REG_R12,
  BX_64BIT_REG_R13,
  BX_64BIT_REG_R14,
  BX_64BIT_REG_R15,
};
#endif

#if BX_SUPPORT_X86_64
# define BX_GENERAL_REGISTERS 16
#else
# define BX_GENERAL_REGISTERS 8
#endif

static const unsigned BX_16BIT_REG_IP  = (BX_GENERAL_REGISTERS),
                      BX_32BIT_REG_EIP = (BX_GENERAL_REGISTERS),
                      BX_64BIT_REG_RIP = (BX_GENERAL_REGISTERS);

static const unsigned BX_32BIT_REG_SSP = (BX_GENERAL_REGISTERS+1),
                      BX_64BIT_REG_SSP = (BX_GENERAL_REGISTERS+1);

static const unsigned BX_TMP_REGISTER = (BX_GENERAL_REGISTERS+2);
static const unsigned BX_NIL_REGISTER = (BX_GENERAL_REGISTERS+3);

enum OpmaskRegs {
  BX_REG_OPMASK_K0,
  BX_REG_OPMASK_K1,
  BX_REG_OPMASK_K2,
  BX_REG_OPMASK_K3,
  BX_REG_OPMASK_K4,
  BX_REG_OPMASK_K5,
  BX_REG_OPMASK_K6,
  BX_REG_OPMASK_K7
};

// AVX Registers
enum bx_avx_vector_length {
  BX_NO_VL,
  BX_VL128  = 1,
  BX_VL256  = 2,
  BX_VL512  = 4,
};

#if BX_SUPPORT_EVEX
#  define BX_VLMAX BX_VL512
#else
#  if BX_SUPPORT_AVX
#    define BX_VLMAX BX_VL256
#  else
#    define BX_VLMAX BX_VL128
#  endif
#endif

#if BX_SUPPORT_EVEX
#  define BX_XMM_REGISTERS 32
#else
#  if BX_SUPPORT_X86_64
#    define BX_XMM_REGISTERS 16
#  else
#    define BX_XMM_REGISTERS 8
#  endif
#endif

static const unsigned BX_VECTOR_TMP_REGISTER = (BX_XMM_REGISTERS);

#endif // BX_X86_DECODER_H

