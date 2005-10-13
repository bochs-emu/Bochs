/////////////////////////////////////////////////////////////////////////
// $Id: lazy_flags.h,v 1.23 2005-10-13 20:21:35 sshwarts Exp $
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


#if BX_PROVIDE_CPU_MEMORY==1

#define BX_INSTR_ADD8           1
#define BX_INSTR_ADD16          2
#define BX_INSTR_ADD32          3
#define BX_INSTR_ADD64          4

// used only if CF = 1 when executing ADC instruction
#define BX_INSTR_ADC8           5
#define BX_INSTR_ADC16          6
#define BX_INSTR_ADC32          7
#define BX_INSTR_ADC64          8

#define BX_INSTR_ADD_ADC8(cf)  (1 + ((cf)<<2))
#define BX_INSTR_ADD_ADC16(cf) (2 + ((cf)<<2))
#define BX_INSTR_ADD_ADC32(cf) (3 + ((cf)<<2))
#define BX_INSTR_ADD_ADC64(cf) (4 + ((cf)<<2))

#define BX_INSTR_SUB8           9
#define BX_INSTR_SUB16          10
#define BX_INSTR_SUB32          11
#define BX_INSTR_SUB64          12

// used only if CF = 1 when executing SBB instruction
#define BX_INSTR_SBB8           13
#define BX_INSTR_SBB16          14
#define BX_INSTR_SBB32          15
#define BX_INSTR_SBB64          16

#define BX_INSTR_SUB_SBB8(cf)  (9  + ((cf)<<2))
#define BX_INSTR_SUB_SBB16(cf) (10 + ((cf)<<2))
#define BX_INSTR_SUB_SBB32(cf) (11 + ((cf)<<2))
#define BX_INSTR_SUB_SBB64(cf) (12 + ((cf)<<2))

#define BX_INSTR_INC8           17
#define BX_INSTR_INC16          18
#define BX_INSTR_INC32          19
#define BX_INSTR_INC64          20

#define BX_INSTR_DEC8           21
#define BX_INSTR_DEC16          22
#define BX_INSTR_DEC32          23
#define BX_INSTR_DEC64          24

#define BX_INSTR_NEG8           25
#define BX_INSTR_NEG16          26
#define BX_INSTR_NEG32          27
#define BX_INSTR_NEG64          28

#define BX_INSTR_LOGIC8         29
#define BX_INSTR_LOGIC16        30
#define BX_INSTR_LOGIC32        31
#define BX_INSTR_LOGIC64        32

//      BX_INSTR_BITSCAN8       not exists, leave number for alignment
#define BX_INSTR_BITSCAN16      34
#define BX_INSTR_BITSCAN32      35
#define BX_INSTR_BITSCAN64      36

#define BX_INSTR_SHL8           37
#define BX_INSTR_SHL16          38
#define BX_INSTR_SHL32          39
#define BX_INSTR_SHL64          40

#define BX_INSTR_SHR8           41
#define BX_INSTR_SHR16          42
#define BX_INSTR_SHR32          43
#define BX_INSTR_SHR64          44

#define BX_INSTR_SAR8           45
#define BX_INSTR_SAR16          46
#define BX_INSTR_SAR32          47
#define BX_INSTR_SAR64          48

#define BX_INSTR_MUL8           49
#define BX_INSTR_MUL16          50
#define BX_INSTR_MUL32          51
#define BX_INSTR_MUL64          52

#define BX_INSTR_IMUL8          53
#define BX_INSTR_IMUL16         54
#define BX_INSTR_IMUL32         55
#define BX_INSTR_IMUL64         56

//      BX_INSTR_SHRD8          not exists, leave number for alignment
#define BX_INSTR_SHRD16         58
#define BX_INSTR_SHRD32         59
#define BX_INSTR_SHRD64         60

#define BX_INSTR_COMPARE8       BX_INSTR_SUB8
#define BX_INSTR_COMPARE16      BX_INSTR_SUB16
#define BX_INSTR_COMPARE32      BX_INSTR_SUB32
#define BX_INSTR_COMPARE64      BX_INSTR_SUB64


#define BX_LF_INDEX_OSZAPC  1
#define BX_LF_INDEX_OSZAP   2

#define BX_LF_MASK_OSZAPC 0x111111
#define BX_LF_MASK_OSZAP  0x222220

typedef struct {
  Bit8u op1_8;
  Bit8u op2_8;
  Bit8u result_8;

  Bit16u op1_16;
  Bit16u op2_16;
  Bit16u result_16;

  Bit32u op1_32;
  Bit32u op2_32;
  Bit32u result_32;

#if BX_SUPPORT_X86_64
  Bit64u op1_64;
  Bit64u op2_64;
  Bit64u result_64;
#endif

  unsigned instr;
} bx_lf_flags_entry;

#endif /* BX_PROVIDE_CPU_MEMORY==1 */
