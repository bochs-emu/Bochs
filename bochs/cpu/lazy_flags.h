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

#define BX_INSTR_ADD8   1
#define BX_INSTR_ADD16  2
#define BX_INSTR_ADD32  3

#define BX_INSTR_SUB8   4
#define BX_INSTR_SUB16  5
#define BX_INSTR_SUB32  6

#define BX_INSTR_ADC8   7
#define BX_INSTR_ADC16  8
#define BX_INSTR_ADC32  9

#define BX_INSTR_SBB8   10
#define BX_INSTR_SBB16  11
#define BX_INSTR_SBB32  12

#define BX_INSTR_CMP8   13
#define BX_INSTR_CMP16  14
#define BX_INSTR_CMP32  15

#define BX_INSTR_INC8   16
#define BX_INSTR_INC16  17
#define BX_INSTR_INC32  18

#define BX_INSTR_DEC8   19
#define BX_INSTR_DEC16  20
#define BX_INSTR_DEC32  21

#define BX_INSTR_NEG8   22
#define BX_INSTR_NEG16  23
#define BX_INSTR_NEG32  24

#define BX_INSTR_XADD8  25
#define BX_INSTR_XADD16 26
#define BX_INSTR_XADD32 27

#define BX_INSTR_OR8    28
#define BX_INSTR_OR16   29
#define BX_INSTR_OR32   30

#define BX_INSTR_AND8   31
#define BX_INSTR_AND16  32
#define BX_INSTR_AND32  33

#define BX_INSTR_TEST8   34
#define BX_INSTR_TEST16  35
#define BX_INSTR_TEST32  36

#define BX_INSTR_XOR8    37
#define BX_INSTR_XOR16   38
#define BX_INSTR_XOR32   39

#define BX_INSTR_CMPS8   40
#define BX_INSTR_CMPS16  41
#define BX_INSTR_CMPS32  42

#define BX_INSTR_SCAS8   43
#define BX_INSTR_SCAS16  44
#define BX_INSTR_SCAS32  45

#define BX_INSTR_SHR8    46
#define BX_INSTR_SHR16   47
#define BX_INSTR_SHR32   48

#define BX_INSTR_SHL8    49
#define BX_INSTR_SHL16   50
#define BX_INSTR_SHL32   51



#define BX_LF_INDEX_KNOWN   0
#define BX_LF_INDEX_OSZAPC  1
#define BX_LF_INDEX_OSZAP   2
#define BX_LF_INDEX_P       3

#define BX_LF_MASK_OSZAPC 0x111111
#define BX_LF_MASK_OSZAP  0x222220
#define BX_LF_MASK_P      0x000030


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

  Boolean prev_CF;
  unsigned instr;
  } bx_lf_flags_entry;


#endif /* BX_PROVIDE_CPU_MEMORY==1 */
