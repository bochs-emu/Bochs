/////////////////////////////////////////////////////////////////////////
// $Id: lazy_flags.cc,v 1.8 2002-09-15 00:18:41 kevinlawton Exp $
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




#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR







  Boolean
BX_CPU_C::get_CF(void)
{
  unsigned cf;

  switch ( BX_CPU_THIS_PTR lf_flags_status & 0x00000f ) {
    case BX_LF_INDEX_KNOWN:
      return(BX_CPU_THIS_PTR eflags.val32 & 1);

    case BX_LF_INDEX_OSZAPC:
      switch (BX_CPU_THIS_PTR oszapc.instr) {
        case BX_INSTR_ADD8:
        case BX_INSTR_XADD8:
          cf = (BX_CPU_THIS_PTR oszapc.result_8 <
                              BX_CPU_THIS_PTR oszapc.op1_8);
          break;
        case BX_INSTR_ADD16:
        case BX_INSTR_XADD16:
          cf = (BX_CPU_THIS_PTR oszapc.result_16 <
                              BX_CPU_THIS_PTR oszapc.op1_16);
          break;
        case BX_INSTR_ADD32:
        case BX_INSTR_XADD32:
          cf = (BX_CPU_THIS_PTR oszapc.result_32 <
                              BX_CPU_THIS_PTR oszapc.op1_32);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_ADD64:
        case BX_INSTR_XADD64:
          cf = (BX_CPU_THIS_PTR oszapc.result_64 <
                              BX_CPU_THIS_PTR oszapc.op1_64);
          break;
#endif
        case BX_INSTR_ADC8:
          cf =
            (BX_CPU_THIS_PTR oszapc.result_8 <
             BX_CPU_THIS_PTR oszapc.op1_8) ||
            (BX_CPU_THIS_PTR oszapc.prev_CF &&
             BX_CPU_THIS_PTR oszapc.result_8 ==
             BX_CPU_THIS_PTR oszapc.op1_8);
          break;
        case BX_INSTR_ADC16:
          cf =
            (BX_CPU_THIS_PTR oszapc.result_16 <
             BX_CPU_THIS_PTR oszapc.op1_16) ||
            (BX_CPU_THIS_PTR oszapc.prev_CF &&
             BX_CPU_THIS_PTR oszapc.result_16 ==
             BX_CPU_THIS_PTR oszapc.op1_16);
          break;
        case BX_INSTR_ADC32:
          cf =
            (BX_CPU_THIS_PTR oszapc.result_32 <
             BX_CPU_THIS_PTR oszapc.op1_32) ||
            (BX_CPU_THIS_PTR oszapc.prev_CF &&
             BX_CPU_THIS_PTR oszapc.result_32 ==
             BX_CPU_THIS_PTR oszapc.op1_32);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_ADC64:
          cf =
            (BX_CPU_THIS_PTR oszapc.result_64 <
             BX_CPU_THIS_PTR oszapc.op1_64) ||
            (BX_CPU_THIS_PTR oszapc.prev_CF &&
             BX_CPU_THIS_PTR oszapc.result_64 ==
             BX_CPU_THIS_PTR oszapc.op1_64);
          break;
#endif
        case BX_INSTR_SUB8:
        case BX_INSTR_CMP8:
        case BX_INSTR_CMPS8:
        case BX_INSTR_SCAS8:
          cf = (BX_CPU_THIS_PTR oszapc.op1_8 <
                              BX_CPU_THIS_PTR oszapc.op2_8);
          break;
        case BX_INSTR_SUB16:
        case BX_INSTR_CMP16:
        case BX_INSTR_CMPS16:
        case BX_INSTR_SCAS16:
          cf = (BX_CPU_THIS_PTR oszapc.op1_16 <
                              BX_CPU_THIS_PTR oszapc.op2_16);
          break;
        case BX_INSTR_SUB32:
        case BX_INSTR_CMP32:
        case BX_INSTR_CMPS32:
        case BX_INSTR_SCAS32:
          cf = (BX_CPU_THIS_PTR oszapc.op1_32 <
                              BX_CPU_THIS_PTR oszapc.op2_32);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_SUB64:
        case BX_INSTR_CMP64:
        case BX_INSTR_CMPS64:
        case BX_INSTR_SCAS64:
          cf = (BX_CPU_THIS_PTR oszapc.op1_64 <
                              BX_CPU_THIS_PTR oszapc.op2_64);
          break;
#endif
        case BX_INSTR_SBB8:
          cf =
            (BX_CPU_THIS_PTR oszapc.op1_8 <
             BX_CPU_THIS_PTR oszapc.result_8) ||
            ((BX_CPU_THIS_PTR oszapc.op2_8==0xff) &&
             BX_CPU_THIS_PTR oszapc.prev_CF);
          break;
        case BX_INSTR_SBB16:
          cf = 
            (BX_CPU_THIS_PTR oszapc.op1_16 <
             BX_CPU_THIS_PTR oszapc.result_16) ||
            ((BX_CPU_THIS_PTR oszapc.op2_16==0xffff) &&
             BX_CPU_THIS_PTR oszapc.prev_CF);
          break;
        case BX_INSTR_SBB32:
          cf = 
            (BX_CPU_THIS_PTR oszapc.op1_32 <
             BX_CPU_THIS_PTR oszapc.result_32) ||
            ((BX_CPU_THIS_PTR oszapc.op2_32==0xffffffff) &&
             BX_CPU_THIS_PTR oszapc.prev_CF);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_SBB64:
          cf = 
            (BX_CPU_THIS_PTR oszapc.op1_64 <
             BX_CPU_THIS_PTR oszapc.result_64) ||
            ((BX_CPU_THIS_PTR oszapc.op2_64==BX_CONST64(0xffffffffffffffff)) &&
             BX_CPU_THIS_PTR oszapc.prev_CF);
          break;
#endif
        case BX_INSTR_NEG8:
          cf = BX_CPU_THIS_PTR oszapc.op1_8 != 0;
          break;
        case BX_INSTR_NEG16:
          cf = BX_CPU_THIS_PTR oszapc.op1_16 != 0;
          break;
        case BX_INSTR_NEG32:
          cf = BX_CPU_THIS_PTR oszapc.op1_32 != 0;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_NEG64:
          cf = BX_CPU_THIS_PTR oszapc.op1_64 != 0;
          break;
#endif
        case BX_INSTR_OR8:
        case BX_INSTR_OR16:
        case BX_INSTR_OR32:
#if BX_SUPPORT_X86_64
        case BX_INSTR_OR64:
        case BX_INSTR_AND64:
        case BX_INSTR_TEST64:
        case BX_INSTR_XOR64:
#endif
        case BX_INSTR_AND8:
        case BX_INSTR_AND16:
        case BX_INSTR_AND32:
        case BX_INSTR_TEST8:
        case BX_INSTR_TEST16:
        case BX_INSTR_TEST32:
        case BX_INSTR_XOR8:
        case BX_INSTR_XOR16:
        case BX_INSTR_XOR32:
          cf = 0;
          break;
        case BX_INSTR_SHR8:
          cf =
            (BX_CPU_THIS_PTR oszapc.op1_8 >>
              (BX_CPU_THIS_PTR oszapc.op2_8 - 1)) & 0x01;
          break;
        case BX_INSTR_SHR16:
          cf =
            (BX_CPU_THIS_PTR oszapc.op1_16 >>
              (BX_CPU_THIS_PTR oszapc.op2_16 - 1)) & 0x01;
          break;
        case BX_INSTR_SHR32:
          cf =
            (BX_CPU_THIS_PTR oszapc.op1_32 >>
              (BX_CPU_THIS_PTR oszapc.op2_32 - 1)) & 0x01;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_SHR64:
          cf =
            (BX_CPU_THIS_PTR oszapc.op1_64 >>
              (BX_CPU_THIS_PTR oszapc.op2_64 - 1)) & 0x01;
          break;
#endif
        case BX_INSTR_SHL8:
          if (BX_CPU_THIS_PTR oszapc.op2_8 <= 8) {
            cf =
              (BX_CPU_THIS_PTR oszapc.op1_8 >>
                (8 - BX_CPU_THIS_PTR oszapc.op2_8)) & 0x01;
            }
          else {
            cf = 0;
            }
          break;
        case BX_INSTR_SHL16:
          if (BX_CPU_THIS_PTR oszapc.op2_16 <= 16) {
            cf =
              (BX_CPU_THIS_PTR oszapc.op1_16 >>
                (16 - BX_CPU_THIS_PTR oszapc.op2_16)) & 0x01;
            }
          else {
            cf = 0;
            }
          break;
        case BX_INSTR_SHL32:
          cf =
            (BX_CPU_THIS_PTR oszapc.op1_32 >>
              (32 - BX_CPU_THIS_PTR oszapc.op2_32)) & 0x01;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_SHL64:
          cf =
            (BX_CPU_THIS_PTR oszapc.op1_64 >>
              (32 - BX_CPU_THIS_PTR oszapc.op2_64)) & 0x01;
          break;
#endif
        default:
          cf = 0; // Keep compiler quiet.
          BX_PANIC(("get_CF: OSZAPC: unknown instr %u",
            (unsigned) BX_CPU_THIS_PTR oszapc.instr));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xfffff0;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<0);
      BX_CPU_THIS_PTR eflags.val32 |= (!!cf)<<0;
      return(cf);

    default:
      BX_PANIC(("get_CF: unknown case"));
      return(0);
    }
}


  Boolean
BX_CPU_C::get_AF(void)
{
  unsigned af;

  switch ( (BX_CPU_THIS_PTR lf_flags_status>>8) & 0x00000f ) {
    case BX_LF_INDEX_KNOWN:
      return( (BX_CPU_THIS_PTR eflags.val32 >> 4) & 1);

    case BX_LF_INDEX_OSZAPC:
      switch (BX_CPU_THIS_PTR oszapc.instr) {
        case BX_INSTR_ADD8:
        case BX_INSTR_ADC8:
        case BX_INSTR_SUB8:
        case BX_INSTR_SBB8:
        case BX_INSTR_CMP8:
        case BX_INSTR_XADD8:
        case BX_INSTR_CMPS8:
        case BX_INSTR_SCAS8:
          af =
            ((BX_CPU_THIS_PTR oszapc.op1_8 ^
              BX_CPU_THIS_PTR oszapc.op2_8) ^
             BX_CPU_THIS_PTR oszapc.result_8) & 0x10;
          break;
        case BX_INSTR_ADD16:
        case BX_INSTR_ADC16:
        case BX_INSTR_SUB16:
        case BX_INSTR_SBB16:
        case BX_INSTR_CMP16:
        case BX_INSTR_XADD16:
        case BX_INSTR_CMPS16:
        case BX_INSTR_SCAS16:
          af =
            ((BX_CPU_THIS_PTR oszapc.op1_16 ^
              BX_CPU_THIS_PTR oszapc.op2_16) ^
             BX_CPU_THIS_PTR oszapc.result_16) & 0x10;
          break;
        case BX_INSTR_ADD32:
        case BX_INSTR_ADC32:
        case BX_INSTR_SUB32:
        case BX_INSTR_SBB32:
        case BX_INSTR_CMP32:
        case BX_INSTR_XADD32:
        case BX_INSTR_CMPS32:
        case BX_INSTR_SCAS32:
          af =
            ((BX_CPU_THIS_PTR oszapc.op1_32 ^
              BX_CPU_THIS_PTR oszapc.op2_32) ^
             BX_CPU_THIS_PTR oszapc.result_32) & 0x10;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_ADD64:
        case BX_INSTR_ADC64:
        case BX_INSTR_SUB64:
        case BX_INSTR_SBB64:
        case BX_INSTR_CMP64:
        case BX_INSTR_XADD64:
        case BX_INSTR_CMPS64:
        case BX_INSTR_SCAS64:
          af =
            ((BX_CPU_THIS_PTR oszapc.op1_64 ^
              BX_CPU_THIS_PTR oszapc.op2_64) ^
             BX_CPU_THIS_PTR oszapc.result_64) & 0x10;
          break;
#endif
        case BX_INSTR_NEG8:
          af = (BX_CPU_THIS_PTR oszapc.op1_8 & 0x0f) > 0;
          break;
        case BX_INSTR_NEG16:
          af = (BX_CPU_THIS_PTR oszapc.op1_16 & 0x0f) > 0;
          break;
        case BX_INSTR_NEG32:
          af = (BX_CPU_THIS_PTR oszapc.op1_32 & 0x0f) > 0;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_NEG64:
          af = (BX_CPU_THIS_PTR oszapc.op1_64 & 0x0f) > 0;
          break;
#endif
        case BX_INSTR_OR8:
        case BX_INSTR_OR16:
        case BX_INSTR_OR32:
#if BX_SUPPORT_X86_64
        case BX_INSTR_OR64:
        case BX_INSTR_AND64:
        case BX_INSTR_TEST64:
        case BX_INSTR_XOR64:
        case BX_INSTR_SHR64:
        case BX_INSTR_SHL64:
#endif
        case BX_INSTR_AND8:
        case BX_INSTR_AND16:
        case BX_INSTR_AND32:
        case BX_INSTR_TEST8:
        case BX_INSTR_TEST16:
        case BX_INSTR_TEST32:
        case BX_INSTR_XOR8:
        case BX_INSTR_XOR16:
        case BX_INSTR_XOR32:
        case BX_INSTR_SHR8:
        case BX_INSTR_SHR16:
        case BX_INSTR_SHR32:
        case BX_INSTR_SHL8:
        case BX_INSTR_SHL16:
        case BX_INSTR_SHL32:
          af = 0;
          /* undefined */
          break;
        default:
	  af = 0; // Keep compiler quiet.
          BX_PANIC(("get_AF: OSZAPC: unknown instr %u",
            (unsigned) BX_CPU_THIS_PTR oszapc.instr));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xfff0ff;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<4);
      BX_CPU_THIS_PTR eflags.val32 |= (!!af)<<4;
      return(af);

    case BX_LF_INDEX_OSZAP:
      switch (BX_CPU_THIS_PTR oszap.instr) {
        case BX_INSTR_INC8:
          af = (BX_CPU_THIS_PTR oszap.result_8 & 0x0f) == 0;
          break;
        case BX_INSTR_INC16:
          af = (BX_CPU_THIS_PTR oszap.result_16 & 0x0f) == 0;
          break;
        case BX_INSTR_INC32:
	        af = (BX_CPU_THIS_PTR oszap.result_32 & 0x0f) == 0;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_INC64:
	        af = (BX_CPU_THIS_PTR oszap.result_64 & 0x0f) == 0;
          break;
#endif
        case BX_INSTR_DEC8:
	        af = (BX_CPU_THIS_PTR oszap.result_8 & 0x0f) == 0x0f;
          break;
        case BX_INSTR_DEC16:
	        af = (BX_CPU_THIS_PTR oszap.result_16 & 0x0f) == 0x0f;
          break;
        case BX_INSTR_DEC32:
	        af = (BX_CPU_THIS_PTR oszap.result_32 & 0x0f) == 0x0f;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_DEC64:
	        af = (BX_CPU_THIS_PTR oszap.result_64 & 0x0f) == 0x0f;
          break;
#endif
        default:
          af = 0; // Keep compiler quiet.
          BX_PANIC(("get_AF: OSZAP: unknown instr %u",
            (unsigned) BX_CPU_THIS_PTR oszap.instr));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xfff0ff;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<4);
      BX_CPU_THIS_PTR eflags.val32 |= (!!af)<<4;
      return(af);

    default:
      BX_PANIC(("get_AF: unknown case"));
      return(0);
    }
}


  Boolean
BX_CPU_C::get_ZF(void)
{
  unsigned zf;

  switch ( (BX_CPU_THIS_PTR lf_flags_status>>12) & 0x00000f ) {
    case BX_LF_INDEX_KNOWN:
      return( (BX_CPU_THIS_PTR eflags.val32 >> 6) & 1);

    case BX_LF_INDEX_OSZAPC:
      switch (BX_CPU_THIS_PTR oszapc.instr) {
        case BX_INSTR_ADD8:
        case BX_INSTR_ADC8:
        case BX_INSTR_SUB8:
        case BX_INSTR_SBB8:
        case BX_INSTR_CMP8:
        case BX_INSTR_NEG8:
        case BX_INSTR_XADD8:
        case BX_INSTR_OR8:
        case BX_INSTR_AND8:
        case BX_INSTR_TEST8:
        case BX_INSTR_XOR8:
        case BX_INSTR_CMPS8:
        case BX_INSTR_SCAS8:
        case BX_INSTR_SHR8:
        case BX_INSTR_SHL8:
          zf = (BX_CPU_THIS_PTR oszapc.result_8 == 0);
          break;
        case BX_INSTR_ADD16:
        case BX_INSTR_ADC16:
        case BX_INSTR_SUB16:
        case BX_INSTR_SBB16:
        case BX_INSTR_CMP16:
        case BX_INSTR_NEG16:
        case BX_INSTR_XADD16:
        case BX_INSTR_OR16:
        case BX_INSTR_AND16:
        case BX_INSTR_TEST16:
        case BX_INSTR_XOR16:
        case BX_INSTR_CMPS16:
        case BX_INSTR_SCAS16:
        case BX_INSTR_SHR16:
        case BX_INSTR_SHL16:
          zf = (BX_CPU_THIS_PTR oszapc.result_16 == 0);
          break;
        case BX_INSTR_ADD32:
        case BX_INSTR_ADC32:
        case BX_INSTR_SUB32:
        case BX_INSTR_SBB32:
        case BX_INSTR_CMP32:
        case BX_INSTR_NEG32:
        case BX_INSTR_XADD32:
        case BX_INSTR_OR32:
        case BX_INSTR_AND32:
        case BX_INSTR_TEST32:
        case BX_INSTR_XOR32:
        case BX_INSTR_CMPS32:
        case BX_INSTR_SCAS32:
        case BX_INSTR_SHR32:
        case BX_INSTR_SHL32:
          zf = (BX_CPU_THIS_PTR oszapc.result_32 == 0);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_ADD64:
        case BX_INSTR_ADC64:
        case BX_INSTR_SUB64:
        case BX_INSTR_SBB64:
        case BX_INSTR_CMP64:
        case BX_INSTR_NEG64:
        case BX_INSTR_XADD64:
        case BX_INSTR_OR64:
        case BX_INSTR_AND64:
        case BX_INSTR_TEST64:
        case BX_INSTR_XOR64:
        case BX_INSTR_CMPS64:
        case BX_INSTR_SCAS64:
        case BX_INSTR_SHR64:
        case BX_INSTR_SHL64:
          zf = (BX_CPU_THIS_PTR oszapc.result_64 == 0);
          break;
#endif
        default:
          zf = 0;
          BX_PANIC(("get_ZF: OSZAPC: unknown instr"));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xff0fff;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<6);
      BX_CPU_THIS_PTR eflags.val32 |= (!!zf)<<6;
      return(zf);

    case BX_LF_INDEX_OSZAP:
      switch (BX_CPU_THIS_PTR oszap.instr) {
        case BX_INSTR_INC8:
        case BX_INSTR_DEC8:
          zf = (BX_CPU_THIS_PTR oszap.result_8 == 0);
          break;
        case BX_INSTR_INC16:
        case BX_INSTR_DEC16:
          zf = (BX_CPU_THIS_PTR oszap.result_16 == 0);
          break;
        case BX_INSTR_INC32:
        case BX_INSTR_DEC32:
          zf = (BX_CPU_THIS_PTR oszap.result_32 == 0);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_INC64:
        case BX_INSTR_DEC64:
          zf = (BX_CPU_THIS_PTR oszap.result_64 == 0);
          break;
#endif
        default:
	        zf = 0;
          BX_PANIC(("get_ZF: OSZAP: unknown instr"));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xff0fff;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<6);
      BX_CPU_THIS_PTR eflags.val32 |= (!!zf)<<6;
      return(zf);

    default:
      BX_PANIC(("get_ZF: unknown case"));
      return(0);
    }
}


  Boolean
BX_CPU_C::get_SF(void)
{
  unsigned sf;

  switch ( (BX_CPU_THIS_PTR lf_flags_status>>16) & 0x00000f ) {
    case BX_LF_INDEX_KNOWN:
      return( (BX_CPU_THIS_PTR eflags.val32 >> 7) & 1);

    case BX_LF_INDEX_OSZAPC:
      switch (BX_CPU_THIS_PTR oszapc.instr) {
        case BX_INSTR_ADD8:
        case BX_INSTR_ADC8:
        case BX_INSTR_SUB8:
        case BX_INSTR_SBB8:
        case BX_INSTR_CMP8:
        case BX_INSTR_NEG8:
        case BX_INSTR_XADD8:
        case BX_INSTR_OR8:
        case BX_INSTR_AND8:
        case BX_INSTR_TEST8:
        case BX_INSTR_XOR8:
        case BX_INSTR_CMPS8:
        case BX_INSTR_SCAS8:
        case BX_INSTR_SHR8:
        case BX_INSTR_SHL8:
          sf = (BX_CPU_THIS_PTR oszapc.result_8 >= 0x80);
          break;
        case BX_INSTR_ADD16:
        case BX_INSTR_ADC16:
        case BX_INSTR_SUB16:
        case BX_INSTR_SBB16:
        case BX_INSTR_CMP16:
        case BX_INSTR_NEG16:
        case BX_INSTR_XADD16:
        case BX_INSTR_OR16:
        case BX_INSTR_AND16:
        case BX_INSTR_TEST16:
        case BX_INSTR_XOR16:
        case BX_INSTR_CMPS16:
        case BX_INSTR_SCAS16:
        case BX_INSTR_SHR16:
        case BX_INSTR_SHL16:
          sf = (BX_CPU_THIS_PTR oszapc.result_16 >= 0x8000);
          break;
        case BX_INSTR_ADD32:
        case BX_INSTR_ADC32:
        case BX_INSTR_SUB32:
        case BX_INSTR_SBB32:
        case BX_INSTR_CMP32:
        case BX_INSTR_NEG32:
        case BX_INSTR_XADD32:
        case BX_INSTR_OR32:
        case BX_INSTR_AND32:
        case BX_INSTR_TEST32:
        case BX_INSTR_XOR32:
        case BX_INSTR_CMPS32:
        case BX_INSTR_SCAS32:
        case BX_INSTR_SHR32:
        case BX_INSTR_SHL32:
          sf = (BX_CPU_THIS_PTR oszapc.result_32 >= 0x80000000);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_ADD64:
        case BX_INSTR_ADC64:
        case BX_INSTR_SUB64:
        case BX_INSTR_SBB64:
        case BX_INSTR_CMP64:
        case BX_INSTR_NEG64:
        case BX_INSTR_XADD64:
        case BX_INSTR_OR64:
        case BX_INSTR_AND64:
        case BX_INSTR_TEST64:
        case BX_INSTR_XOR64:
        case BX_INSTR_CMPS64:
        case BX_INSTR_SCAS64:
        case BX_INSTR_SHR64:
        case BX_INSTR_SHL64:
          sf = (BX_CPU_THIS_PTR oszapc.result_64 >= BX_CONST64(0x8000000000000000));
          break;
#endif
        default:
          sf = 0; // Keep compiler quiet.
          BX_PANIC(("get_SF: OSZAPC: unknown instr"));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xf0ffff;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<7);
      BX_CPU_THIS_PTR eflags.val32 |= (!!sf)<<7;
      return(sf);

    case BX_LF_INDEX_OSZAP:
      switch (BX_CPU_THIS_PTR oszap.instr) {
        case BX_INSTR_INC8:
        case BX_INSTR_DEC8:
	        sf = (BX_CPU_THIS_PTR oszap.result_8 >= 0x80);
          break;
        case BX_INSTR_INC16:
        case BX_INSTR_DEC16:
	        sf = (BX_CPU_THIS_PTR oszap.result_16 >= 0x8000);
          break;
        case BX_INSTR_INC32:
        case BX_INSTR_DEC32:
	        sf = (BX_CPU_THIS_PTR oszap.result_32 >= 0x80000000);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_INC64:
        case BX_INSTR_DEC64:
	        sf = (BX_CPU_THIS_PTR oszap.result_64 >= BX_CONST64(0x8000000000000000));
          break;
#endif
        default:
          sf = 0; // Keep compiler quiet.
          BX_PANIC(("get_SF: OSZAP: unknown instr"));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xf0ffff;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<7);
      BX_CPU_THIS_PTR eflags.val32 |= (!!sf)<<7;
      return(sf);

    default:
      BX_PANIC(("get_SF: unknown case"));
      return(0);
    }
}

  Boolean
BX_CPU_C::get_OF(void)
{
  Bit8u op1_b7, op2_b7, result_b7;
  Bit16u op1_b15, op2_b15, result_b15;
  Bit32u op1_b31, op2_b31, result_b31;
#if BX_SUPPORT_X86_64
  Bit64u op1_b63, op2_b63, result_b63;
#endif
  unsigned of;

  switch ( (BX_CPU_THIS_PTR lf_flags_status>>20) & 0x00000f ) {
    case BX_LF_INDEX_KNOWN:
      return( (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1);

    case BX_LF_INDEX_OSZAPC:
      switch (BX_CPU_THIS_PTR oszapc.instr) {
        case BX_INSTR_ADD8:
        case BX_INSTR_ADC8:
        case BX_INSTR_XADD8:
          op1_b7 = BX_CPU_THIS_PTR oszapc.op1_8 & 0x80;
          op2_b7 = BX_CPU_THIS_PTR oszapc.op2_8 & 0x80;
          result_b7 = BX_CPU_THIS_PTR oszapc.result_8 & 0x80;

          of =  (op1_b7 == op2_b7) && (result_b7 ^ op2_b7);
          break;
        case BX_INSTR_ADD16:
        case BX_INSTR_ADC16:
        case BX_INSTR_XADD16:
          op1_b15 = BX_CPU_THIS_PTR oszapc.op1_16 & 0x8000;
          op2_b15 = BX_CPU_THIS_PTR oszapc.op2_16 & 0x8000;
          result_b15 = BX_CPU_THIS_PTR oszapc.result_16 & 0x8000;

          of =  (op1_b15 == op2_b15) && (result_b15 ^ op2_b15);
          break;
        case BX_INSTR_ADD32:
        case BX_INSTR_ADC32:
        case BX_INSTR_XADD32:
          op1_b31 = BX_CPU_THIS_PTR oszapc.op1_32 & 0x80000000;
          op2_b31 = BX_CPU_THIS_PTR oszapc.op2_32 & 0x80000000;
          result_b31 = BX_CPU_THIS_PTR oszapc.result_32 & 0x80000000;

          of =  (op1_b31 == op2_b31) && (result_b31 ^ op2_b31);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_ADD64:
        case BX_INSTR_ADC64:
        case BX_INSTR_XADD64:
          op1_b63 = BX_CPU_THIS_PTR oszapc.op1_64 & BX_CONST64(0x8000000000000000);
          op2_b63 = BX_CPU_THIS_PTR oszapc.op2_64 & BX_CONST64(0x8000000000000000);
          result_b63 = BX_CPU_THIS_PTR oszapc.result_64 & BX_CONST64(0x8000000000000000);

          of =  (op1_b63 == op2_b63) && (result_b63 ^ op2_b63);
          break;
#endif
        case BX_INSTR_SUB8:
        case BX_INSTR_SBB8:
        case BX_INSTR_CMP8:
        case BX_INSTR_CMPS8:
        case BX_INSTR_SCAS8:
          op1_b7 = BX_CPU_THIS_PTR oszapc.op1_8 & 0x80;
          op2_b7 = BX_CPU_THIS_PTR oszapc.op2_8 & 0x80;
          result_b7 = BX_CPU_THIS_PTR oszapc.result_8 & 0x80;

          of =  (op1_b7 ^ op2_b7) && (op1_b7 ^ result_b7);
          break;
        case BX_INSTR_SUB16:
        case BX_INSTR_SBB16:
        case BX_INSTR_CMP16:
        case BX_INSTR_CMPS16:
        case BX_INSTR_SCAS16:
          op1_b15 = BX_CPU_THIS_PTR oszapc.op1_16 & 0x8000;
          op2_b15 = BX_CPU_THIS_PTR oszapc.op2_16 & 0x8000;
          result_b15 = BX_CPU_THIS_PTR oszapc.result_16 & 0x8000;

          of =  (op1_b15 ^ op2_b15) && (op1_b15 ^ result_b15);
          break;
        case BX_INSTR_SUB32:
        case BX_INSTR_SBB32:
        case BX_INSTR_CMP32:
        case BX_INSTR_CMPS32:
        case BX_INSTR_SCAS32:
          op1_b31 = BX_CPU_THIS_PTR oszapc.op1_32 & 0x80000000;
          op2_b31 = BX_CPU_THIS_PTR oszapc.op2_32 & 0x80000000;
          result_b31 = BX_CPU_THIS_PTR oszapc.result_32 & 0x80000000;

          of =  (op1_b31 ^ op2_b31) && (op1_b31 ^ result_b31);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_SUB64:
        case BX_INSTR_SBB64:
        case BX_INSTR_CMP64:
        case BX_INSTR_CMPS64:
        case BX_INSTR_SCAS64:
          op1_b63 = BX_CPU_THIS_PTR oszapc.op1_64 & BX_CONST64(0x8000000000000000);
          op2_b63 = BX_CPU_THIS_PTR oszapc.op2_64 & BX_CONST64(0x8000000000000000);
          result_b63 = BX_CPU_THIS_PTR oszapc.result_64 & BX_CONST64(0x8000000000000000);

          of =  (op1_b63 ^ op2_b63) && (op1_b63 ^ result_b63);
          break;
#endif
        case BX_INSTR_NEG8:
          of = (BX_CPU_THIS_PTR oszapc.op1_8 == 0x80);
          break;
        case BX_INSTR_NEG16:
          of = (BX_CPU_THIS_PTR oszapc.op1_16 == 0x8000);
          break;
        case BX_INSTR_NEG32:
          of = (BX_CPU_THIS_PTR oszapc.op1_32 == 0x80000000);
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_NEG64:
          of = (BX_CPU_THIS_PTR oszapc.op1_64 == BX_CONST64(0x8000000000000000));
          break;
#endif
        case BX_INSTR_OR8:
        case BX_INSTR_OR16:
        case BX_INSTR_OR32:
#if BX_SUPPORT_X86_64
        case BX_INSTR_OR64:
        case BX_INSTR_AND64:
        case BX_INSTR_TEST64:
        case BX_INSTR_XOR64:
#endif
        case BX_INSTR_AND8:
        case BX_INSTR_AND16:
        case BX_INSTR_AND32:
        case BX_INSTR_TEST8:
        case BX_INSTR_TEST16:
        case BX_INSTR_TEST32:
        case BX_INSTR_XOR8:
        case BX_INSTR_XOR16:
        case BX_INSTR_XOR32:
          of = 0;
          break;
        case BX_INSTR_SHR8:
          if (BX_CPU_THIS_PTR oszapc.op2_8 == 1)
            of = (BX_CPU_THIS_PTR oszapc.op1_8 >= 0x80);
          else
            of = (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1; // Old val
          break;
        case BX_INSTR_SHR16:
          if (BX_CPU_THIS_PTR oszapc.op2_16 == 1)
            of = (BX_CPU_THIS_PTR oszapc.op1_16 >= 0x8000);
          else
            of = (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1; // Old val
          break;
        case BX_INSTR_SHR32:
          if (BX_CPU_THIS_PTR oszapc.op2_32 == 1)
            of = (BX_CPU_THIS_PTR oszapc.op1_32 >= 0x80000000);
          else
            of = (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1; // Old val
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_SHR64:
          if (BX_CPU_THIS_PTR oszapc.op2_64 == 1)
            of = (BX_CPU_THIS_PTR oszapc.op1_64 >= BX_CONST64(0x8000000000000000));
	        else
            of = (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1; // Old val
          break;
#endif
        case BX_INSTR_SHL8:
          if (BX_CPU_THIS_PTR oszapc.op2_8 == 1)
            of = ((BX_CPU_THIS_PTR oszapc.op1_8 ^
                BX_CPU_THIS_PTR oszapc.result_8) & 0x80) > 0;
          else
            of = (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1; // Old val
          break;
        case BX_INSTR_SHL16:
          if (BX_CPU_THIS_PTR oszapc.op2_16 == 1)
            of = ((BX_CPU_THIS_PTR oszapc.op1_16 ^
                BX_CPU_THIS_PTR oszapc.result_16) & 0x8000) > 0;
          else
            of = (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1; // Old val
          break;
        case BX_INSTR_SHL32:
          if (BX_CPU_THIS_PTR oszapc.op2_32 == 1)
            of = ((BX_CPU_THIS_PTR oszapc.op1_32 ^
                BX_CPU_THIS_PTR oszapc.result_32) & 0x80000000) > 0;
          else
            of = (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1; // Old val
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_SHL64:
          if (BX_CPU_THIS_PTR oszapc.op2_64 == 1)
            of = ((BX_CPU_THIS_PTR oszapc.op1_64 ^
                BX_CPU_THIS_PTR oszapc.result_64) & BX_CONST64(0x8000000000000000)) > 0;
	        else
            of = (BX_CPU_THIS_PTR eflags.val32 >> 11) & 1; // Old val
          break;
#endif
        default:
          of = 0; // Keep compiler happy.
          BX_PANIC(("get_OF: OSZAPC: unknown instr"));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0x0fffff;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<11);
      BX_CPU_THIS_PTR eflags.val32 |= (!!of)<<11;
      return(of);

    case BX_LF_INDEX_OSZAP:
      switch (BX_CPU_THIS_PTR oszap.instr) {
        case BX_INSTR_INC8:
	        of = BX_CPU_THIS_PTR oszap.result_8 == 0x80;
          break;
        case BX_INSTR_INC16:
	        of = BX_CPU_THIS_PTR oszap.result_16 == 0x8000;
          break;
        case BX_INSTR_INC32:
	        of = BX_CPU_THIS_PTR oszap.result_32 == 0x80000000;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_INC64:
	        of = BX_CPU_THIS_PTR oszap.result_64 == BX_CONST64(0x8000000000000000);
          break;
#endif
        case BX_INSTR_DEC8:
	        of = BX_CPU_THIS_PTR oszap.result_8 == 0x7F;
          break;
        case BX_INSTR_DEC16:
	        of = BX_CPU_THIS_PTR oszap.result_16 == 0x7FFF;
          break;
        case BX_INSTR_DEC32:
	        of = BX_CPU_THIS_PTR oszap.result_32 == 0x7FFFFFFF;
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_DEC64:
	        of = BX_CPU_THIS_PTR oszap.result_64 == BX_CONST64(0x7FFFFFFFFFFFFFFF);
          break;
#endif
        default:
	        of = 0; // Keep compiler happy.
          BX_PANIC(("get_OF: OSZAP: unknown instr"));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0x0fffff;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<11);
      BX_CPU_THIS_PTR eflags.val32 |= (!!of)<<11;
      return(of);

    default:
      BX_PANIC(("get_OF: unknown case"));
      return(0);
    }
}

  Boolean
BX_CPU_C::get_PF(void)
{
  unsigned pf;

  switch ( (BX_CPU_THIS_PTR lf_flags_status>>4) & 0x00000f ) {
    case BX_LF_INDEX_KNOWN:
      return( (BX_CPU_THIS_PTR eflags.val32 >> 2) & 1);
    case BX_LF_INDEX_OSZAPC:
      switch (BX_CPU_THIS_PTR oszapc.instr) {
        case BX_INSTR_ADD8:
        case BX_INSTR_ADC8:
        case BX_INSTR_SUB8:
        case BX_INSTR_SBB8:
        case BX_INSTR_CMP8:
        case BX_INSTR_NEG8:
        case BX_INSTR_XADD8:
        case BX_INSTR_OR8:
        case BX_INSTR_AND8:
        case BX_INSTR_TEST8:
        case BX_INSTR_XOR8:
        case BX_INSTR_CMPS8:
        case BX_INSTR_SCAS8:
        case BX_INSTR_SHR8:
        case BX_INSTR_SHL8:
          pf = bx_parity_lookup[BX_CPU_THIS_PTR oszapc.result_8];
          break;
        case BX_INSTR_ADD16:
        case BX_INSTR_ADC16:
        case BX_INSTR_SUB16:
        case BX_INSTR_SBB16:
        case BX_INSTR_CMP16:
        case BX_INSTR_NEG16:
        case BX_INSTR_XADD16:
        case BX_INSTR_OR16:
        case BX_INSTR_AND16:
        case BX_INSTR_TEST16:
        case BX_INSTR_XOR16:
        case BX_INSTR_CMPS16:
        case BX_INSTR_SCAS16:
        case BX_INSTR_SHR16:
        case BX_INSTR_SHL16:
          pf = bx_parity_lookup[(Bit8u) BX_CPU_THIS_PTR oszapc.result_16];
          break;
        case BX_INSTR_ADD32:
        case BX_INSTR_ADC32:
        case BX_INSTR_SUB32:
        case BX_INSTR_SBB32:
        case BX_INSTR_CMP32:
        case BX_INSTR_NEG32:
        case BX_INSTR_XADD32:
        case BX_INSTR_OR32:
        case BX_INSTR_AND32:
        case BX_INSTR_TEST32:
        case BX_INSTR_XOR32:
        case BX_INSTR_CMPS32:
        case BX_INSTR_SCAS32:
        case BX_INSTR_SHR32:
        case BX_INSTR_SHL32:
          pf = bx_parity_lookup[(Bit8u) BX_CPU_THIS_PTR oszapc.result_32];
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_ADD64:
        case BX_INSTR_ADC64:
        case BX_INSTR_SUB64:
        case BX_INSTR_SBB64:
        case BX_INSTR_CMP64:
        case BX_INSTR_NEG64:
        case BX_INSTR_XADD64:
        case BX_INSTR_OR64:
        case BX_INSTR_AND64:
        case BX_INSTR_TEST64:
        case BX_INSTR_XOR64:
        case BX_INSTR_CMPS64:
        case BX_INSTR_SCAS64:
        case BX_INSTR_SHR64:
        case BX_INSTR_SHL64:
          pf = bx_parity_lookup[(Bit8u) BX_CPU_THIS_PTR oszapc.result_64];
          break;
#endif
        default:
	        pf = 0; // Keep compiler quiet.
          BX_PANIC(("get_PF: OSZAPC: unknown instr"));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xffff0f;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<2);
      BX_CPU_THIS_PTR eflags.val32 |= (!!pf)<<2;
      return(pf);

    case BX_LF_INDEX_OSZAP:
      switch (BX_CPU_THIS_PTR oszap.instr) {
        case BX_INSTR_INC8:
        case BX_INSTR_DEC8:
          pf = bx_parity_lookup[BX_CPU_THIS_PTR oszap.result_8];
          break;
        case BX_INSTR_INC16:
        case BX_INSTR_DEC16:
          pf = bx_parity_lookup[(Bit8u) BX_CPU_THIS_PTR oszap.result_16];
          break;
        case BX_INSTR_INC32:
        case BX_INSTR_DEC32:
          pf = bx_parity_lookup[(Bit8u) BX_CPU_THIS_PTR oszap.result_32];
          break;
#if BX_SUPPORT_X86_64
        case BX_INSTR_INC64:
        case BX_INSTR_DEC64:
          pf = bx_parity_lookup[(Bit8u) BX_CPU_THIS_PTR oszap.result_64];
          break;
#endif
        default:
          pf = 0; // Keep compiler quiet.
          BX_PANIC(("get_PF: OSZAP: unknown instr"));
        }
      BX_CPU_THIS_PTR lf_flags_status &= 0xffff0f;
      BX_CPU_THIS_PTR eflags.val32 &= ~(1<<2);
      BX_CPU_THIS_PTR eflags.val32 |= (!!pf)<<2;
      return(pf);

    default:
      BX_PANIC(("get_PF: unknown case"));
      return(0);
    }
}
