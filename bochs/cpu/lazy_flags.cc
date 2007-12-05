/////////////////////////////////////////////////////////////////////////
// $Id: lazy_flags.cc,v 1.40 2007-12-05 06:17:09 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


// This array defines a look-up table for the even parity-ness
// of an 8bit quantity, for optimal assignment of the parity bit
// in the EFLAGS register
const bx_bool bx_parity_lookup[256] = {
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

#define result_8  ((Bit8u) (BX_CPU_THIS_PTR oszapc.result))
#define result_16 ((Bit16u)(BX_CPU_THIS_PTR oszapc.result))
#define result_32 ((Bit32u)(BX_CPU_THIS_PTR oszapc.result))
#define result_64          (BX_CPU_THIS_PTR oszapc.result)

bx_bool BX_CPU_C::get_CFLazy(void)
{
  unsigned cf;

  switch (BX_CPU_THIS_PTR oszapc.instr) {
    case BX_INSTR_ADD8:
      cf = (result_8 < BX_CPU_THIS_PTR oszapc.op1_8);
      break;
    case BX_INSTR_ADD16:
      cf = (result_16 < BX_CPU_THIS_PTR oszapc.op1_16);
      break;
    case BX_INSTR_ADD32:
      cf = (result_32 < BX_CPU_THIS_PTR oszapc.op1_32);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_ADD64:
      cf = (result_64 < BX_CPU_THIS_PTR oszapc.op1_64);
      break;
#endif
    // used only if CF = 1 when executing ADC instruction
    case BX_INSTR_ADC8:
      cf = (result_8 <= BX_CPU_THIS_PTR oszapc.op1_8);
      break;
    // used only if CF = 1 when executing ADC instruction
    case BX_INSTR_ADC16:
      cf = (result_16 <= BX_CPU_THIS_PTR oszapc.op1_16);
      break;
    // used only if CF = 1 when executing ADC instruction
    case BX_INSTR_ADC32:
      cf = (result_32 <= BX_CPU_THIS_PTR oszapc.op1_32);
      break;
#if BX_SUPPORT_X86_64
    // used only if CF = 1 when executing ADC instruction
    case BX_INSTR_ADC64:
      cf = (result_64 <= BX_CPU_THIS_PTR oszapc.op1_64);
      break;
#endif
    case BX_INSTR_SUB8:
      cf = (BX_CPU_THIS_PTR oszapc.op1_8 <
                          BX_CPU_THIS_PTR oszapc.op2_8);
      break;
    case BX_INSTR_SUB16:
      cf = (BX_CPU_THIS_PTR oszapc.op1_16 <
                          BX_CPU_THIS_PTR oszapc.op2_16);
      break;
    case BX_INSTR_SUB32:
      cf = (BX_CPU_THIS_PTR oszapc.op1_32 <
                          BX_CPU_THIS_PTR oszapc.op2_32);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_SUB64:
      cf = (BX_CPU_THIS_PTR oszapc.op1_64 <
                          BX_CPU_THIS_PTR oszapc.op2_64);
      break;
#endif
    // used only if CF = 1 when executing SBB instruction
    case BX_INSTR_SBB8:
      cf = (BX_CPU_THIS_PTR oszapc.op1_8 < result_8) ||
           (BX_CPU_THIS_PTR oszapc.op2_8==0xff);
      break;
    // used only if CF = 1 when executing SBB instruction
    case BX_INSTR_SBB16:
      cf = (BX_CPU_THIS_PTR oszapc.op1_16 < result_16) ||
           (BX_CPU_THIS_PTR oszapc.op2_16==0xffff);
      break;
    // used only if CF = 1 when executing SBB instruction
    case BX_INSTR_SBB32:
      cf = (BX_CPU_THIS_PTR oszapc.op1_32 < result_32) ||
           (BX_CPU_THIS_PTR oszapc.op2_32==0xffffffff);
      break;
#if BX_SUPPORT_X86_64
    // used only if CF = 1 when executing SBB instruction
    case BX_INSTR_SBB64:
      cf = (BX_CPU_THIS_PTR oszapc.op1_64 < result_64) ||
           (BX_CPU_THIS_PTR oszapc.op2_64==BX_CONST64(0xffffffffffffffff));
      break;
#endif
    case BX_INSTR_NEG8:
      cf = (result_8 != 0);
      break;
    case BX_INSTR_NEG16:
      cf = (result_16 != 0);
      break;
    case BX_INSTR_NEG32:
      cf = (result_32 != 0);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_NEG64:
      cf = (result_64 != 0);
      break;
#endif
    case BX_INSTR_LOGIC8:
    case BX_INSTR_LOGIC16:
    case BX_INSTR_LOGIC32:
#if BX_SUPPORT_X86_64
    case BX_INSTR_LOGIC64:
#endif
      cf = 0;
      break;
    case BX_INSTR_SAR8:
      if (BX_CPU_THIS_PTR oszapc.op2_8 < 8) {
        cf = (BX_CPU_THIS_PTR oszapc.op1_8 >>
                (BX_CPU_THIS_PTR oszapc.op2_8 - 1)) & 0x01;
      }
      else {
        cf = (BX_CPU_THIS_PTR oszapc.op1_8 & 0x80) > 0;
      }
      break;
    case BX_INSTR_SHR8:
      cf = (BX_CPU_THIS_PTR oszapc.op1_8 >>
                (BX_CPU_THIS_PTR oszapc.op2_8 - 1)) & 0x01;
      break;
    case BX_INSTR_SAR16:
      if (BX_CPU_THIS_PTR oszapc.op2_16 < 16) {
        cf = (BX_CPU_THIS_PTR oszapc.op1_16 >>
                (BX_CPU_THIS_PTR oszapc.op2_16 - 1)) & 0x01;
      }
      else {
        cf = (BX_CPU_THIS_PTR oszapc.op1_16 & 0x8000) > 0;
      }
      break;
    case BX_INSTR_SHR16:
    case BX_INSTR_SHRD16:
      cf = (BX_CPU_THIS_PTR oszapc.op1_16 >>
                (BX_CPU_THIS_PTR oszapc.op2_16 - 1)) & 0x01;
      break;
    case BX_INSTR_SAR32:
    case BX_INSTR_SHR32:
    case BX_INSTR_SHRD32:
      cf = (BX_CPU_THIS_PTR oszapc.op1_32 >>
                (BX_CPU_THIS_PTR oszapc.op2_32 - 1)) & 0x01;
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_SAR64:
    case BX_INSTR_SHR64:
    case BX_INSTR_SHRD64:
      cf = (BX_CPU_THIS_PTR oszapc.op1_64 >>
                (BX_CPU_THIS_PTR oszapc.op2_64 - 1)) & 0x01;
      break;
#endif
    case BX_INSTR_SHL8:
      if (BX_CPU_THIS_PTR oszapc.op2_8 <= 8) {
        cf = (BX_CPU_THIS_PTR oszapc.op1_8 >>
                (8 - BX_CPU_THIS_PTR oszapc.op2_8)) & 0x01;
      }
      else {
        cf = 0;
      }
      break;
    case BX_INSTR_SHL16:
      if (BX_CPU_THIS_PTR oszapc.op2_16 <= 16) {
        cf = (BX_CPU_THIS_PTR oszapc.op1_16 >>
                (16 - BX_CPU_THIS_PTR oszapc.op2_16)) & 0x01;
      }
      else {
        cf = 0;
      }
      break;
    case BX_INSTR_SHL32:
      cf = (BX_CPU_THIS_PTR oszapc.op1_32 >>
                (32 - BX_CPU_THIS_PTR oszapc.op2_32)) & 0x01;
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_SHL64:
       cf = (BX_CPU_THIS_PTR oszapc.op1_64 >>
                (64 - BX_CPU_THIS_PTR oszapc.op2_64)) & 0x01;
      break;
#endif
    case BX_INSTR_IMUL8:
      cf = ! ((result_8 < 0x80 &&
               BX_CPU_THIS_PTR oszapc.op2_8 == 0) ||
             ((result_8 & 0x80) && 
               BX_CPU_THIS_PTR oszapc.op2_8 == 0xff));
      break;
    case BX_INSTR_IMUL16:
      cf = ! ((result_16 < 0x8000 &&
               BX_CPU_THIS_PTR oszapc.op2_16 == 0) ||
             ((result_16 & 0x8000) && 
               BX_CPU_THIS_PTR oszapc.op2_16 == 0xffff));
      break;
    case BX_INSTR_IMUL32:
      cf = ! ((result_32 < 0x80000000 &&
               BX_CPU_THIS_PTR oszapc.op2_32 == 0) ||
             ((result_32 & 0x80000000) && 
               BX_CPU_THIS_PTR oszapc.op2_32 == 0xffffffff));
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_IMUL64:
      cf = ! ((result_64 < BX_CONST64(0x8000000000000000) &&
               BX_CPU_THIS_PTR oszapc.op2_64 == 0) ||
             ((result_64 & BX_CONST64(0x8000000000000000)) && 
               BX_CPU_THIS_PTR oszapc.op2_64 == BX_CONST64(0xffffffffffffffff)));
      break;
#endif
    case BX_INSTR_MUL8:
      cf = (BX_CPU_THIS_PTR oszapc.op2_8 != 0);
      break;
    case BX_INSTR_MUL16:
      cf = (BX_CPU_THIS_PTR oszapc.op2_16 != 0);
      break;
    case BX_INSTR_MUL32:
      cf = (BX_CPU_THIS_PTR oszapc.op2_32 != 0);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_MUL64:
      cf = (BX_CPU_THIS_PTR oszapc.op2_64 != 0);
      break;
#endif
    default:
      cf = 0; // Keep compiler quiet.
      BX_PANIC(("get_CF: OSZAPC: unknown instr %u",
            (unsigned) BX_CPU_THIS_PTR oszapc.instr));
  }
 
  return(cf);
}

bx_bool BX_CPU_C::get_AFLazy(void)
{
  unsigned af;

  switch (BX_CPU_THIS_PTR oszapc.instr) {
    case BX_INSTR_ADD8:
    case BX_INSTR_ADC8:
    case BX_INSTR_SUB8:
    case BX_INSTR_SBB8:
      af = ((BX_CPU_THIS_PTR oszapc.op1_8 ^
                 BX_CPU_THIS_PTR oszapc.op2_8) ^ result_8) & 0x10;
      break;
    case BX_INSTR_ADD16:
    case BX_INSTR_ADC16:
    case BX_INSTR_SUB16:
    case BX_INSTR_SBB16:
      af = ((BX_CPU_THIS_PTR oszapc.op1_16 ^
                 BX_CPU_THIS_PTR oszapc.op2_16) ^ result_16) & 0x10;
      break;
    case BX_INSTR_ADD32:
    case BX_INSTR_ADC32:
    case BX_INSTR_SUB32:
    case BX_INSTR_SBB32:
      af = ((BX_CPU_THIS_PTR oszapc.op1_32 ^
                 BX_CPU_THIS_PTR oszapc.op2_32) ^ result_32) & 0x10;
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_ADD64:
    case BX_INSTR_ADC64:
    case BX_INSTR_SUB64:
    case BX_INSTR_SBB64:
      af = ((BX_CPU_THIS_PTR oszapc.op1_64 ^
                 BX_CPU_THIS_PTR oszapc.op2_64) ^ result_64) & 0x10;
      break;
#endif
    case BX_INSTR_NEG8:
    case BX_INSTR_NEG16:
    case BX_INSTR_NEG32:
#if BX_SUPPORT_X86_64
    case BX_INSTR_NEG64:
#endif
      af = (result_8 & 0xf) != 0;
      break;
    case BX_INSTR_INC8:
    case BX_INSTR_INC16:
    case BX_INSTR_INC32:
#if BX_SUPPORT_X86_64
    case BX_INSTR_INC64:
#endif
      af = (result_8 & 0xf) == 0;
      break;
    case BX_INSTR_DEC8:
    case BX_INSTR_DEC16:
    case BX_INSTR_DEC32:
#if BX_SUPPORT_X86_64
    case BX_INSTR_DEC64:
#endif
      af = (result_8 & 0xf) == 0xf;
      break;
    case BX_INSTR_LOGIC8:
    case BX_INSTR_LOGIC16:
    case BX_INSTR_LOGIC32:
#if BX_SUPPORT_X86_64
    case BX_INSTR_LOGIC64:
    case BX_INSTR_SAR64:
    case BX_INSTR_SHR64:
    case BX_INSTR_SHRD64:
    case BX_INSTR_SHL64:
    case BX_INSTR_IMUL64:
    case BX_INSTR_MUL64:
#endif
    case BX_INSTR_SAR8:
    case BX_INSTR_SAR16:
    case BX_INSTR_SAR32:
    case BX_INSTR_SHR8:
    case BX_INSTR_SHR16:
    case BX_INSTR_SHR32:
    case BX_INSTR_SHRD16:
    case BX_INSTR_SHRD32:
    case BX_INSTR_SHL8:
    case BX_INSTR_SHL16:
    case BX_INSTR_SHL32:
    case BX_INSTR_IMUL8:
    case BX_INSTR_IMUL16:
    case BX_INSTR_IMUL32:
    case BX_INSTR_MUL8:
    case BX_INSTR_MUL16:
    case BX_INSTR_MUL32:
      af = 0;
      break;
    default:
      af = 0; // Keep compiler quiet.
      BX_PANIC(("get_AF: OSZAPC: unknown instr %u", (unsigned) BX_CPU_THIS_PTR oszapc.instr));
    }

    return(af);
}

bx_bool BX_CPU_C::get_ZFLazy(void)
{
  return (BX_CPU_THIS_PTR oszapc.result == 0);
}

bx_bool BX_CPU_C::get_SFLazy(void)
{
  return (BX_CPU_THIS_PTR oszapc.result >> BX_LF_SIGN_BIT);
}

#define GET_ADD_OVERFLOW(op1, op2, result, mask) \
  (((~((op1) ^ (op2)) & ((op2) ^ (result))) & (mask)) != 0)

#define GET_SUB_OVERFLOW(op1, op2, result, mask) \
   (((((op1) ^ (op2)) & ((op1) ^ (result))) & (mask)) != 0)

bx_bool BX_CPU_C::get_OFLazy(void)
{
  unsigned of;

  switch (BX_CPU_THIS_PTR oszapc.instr) {
    case BX_INSTR_ADD8:
    case BX_INSTR_ADC8:
      of = GET_ADD_OVERFLOW(BX_CPU_THIS_PTR oszapc.op1_8,
                            BX_CPU_THIS_PTR oszapc.op2_8,
                            result_8, 0x80);
      break;
    case BX_INSTR_ADD16:
    case BX_INSTR_ADC16:
      of = GET_ADD_OVERFLOW(BX_CPU_THIS_PTR oszapc.op1_16,
                            BX_CPU_THIS_PTR oszapc.op2_16,
                            result_16, 0x8000);
      break;
    case BX_INSTR_ADD32:
    case BX_INSTR_ADC32:
      of = GET_ADD_OVERFLOW(BX_CPU_THIS_PTR oszapc.op1_32,
                            BX_CPU_THIS_PTR oszapc.op2_32,
                            result_32, 0x80000000);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_ADD64:
    case BX_INSTR_ADC64:
      of = GET_ADD_OVERFLOW(BX_CPU_THIS_PTR oszapc.op1_64,
                            BX_CPU_THIS_PTR oszapc.op2_64,
                            result_64, BX_CONST64(0x8000000000000000));
      break;
#endif
    case BX_INSTR_SUB8:
    case BX_INSTR_SBB8:
      of = GET_SUB_OVERFLOW(BX_CPU_THIS_PTR oszapc.op1_8,
                            BX_CPU_THIS_PTR oszapc.op2_8,
                            result_8, 0x80);
      break;
    case BX_INSTR_SUB16:
    case BX_INSTR_SBB16:
      of = GET_SUB_OVERFLOW(BX_CPU_THIS_PTR oszapc.op1_16,
                            BX_CPU_THIS_PTR oszapc.op2_16,
                            result_16, 0x8000);
      break;
    case BX_INSTR_SUB32:
    case BX_INSTR_SBB32:
      of = GET_SUB_OVERFLOW(BX_CPU_THIS_PTR oszapc.op1_32,
                            BX_CPU_THIS_PTR oszapc.op2_32,
                            result_32, 0x80000000);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_SUB64:
    case BX_INSTR_SBB64:
      of = GET_SUB_OVERFLOW(BX_CPU_THIS_PTR oszapc.op1_64,
                            BX_CPU_THIS_PTR oszapc.op2_64,
                            result_64, BX_CONST64(0x8000000000000000));
      break;
#endif
    case BX_INSTR_LOGIC8:
    case BX_INSTR_LOGIC16:
    case BX_INSTR_LOGIC32:
    case BX_INSTR_SAR8:
    case BX_INSTR_SAR16:
    case BX_INSTR_SAR32:
#if BX_SUPPORT_X86_64
    case BX_INSTR_LOGIC64:
    case BX_INSTR_SAR64:
#endif
      of = 0;
      break;
    case BX_INSTR_SHR8:
      if (BX_CPU_THIS_PTR oszapc.op2_8 == 1)
        of = (BX_CPU_THIS_PTR oszapc.op1_8 >= 0x80);
      else
        of = 0; /* undocumented, but hardware really does it */
      break;
    case BX_INSTR_SHR16:
      if (BX_CPU_THIS_PTR oszapc.op2_16 == 1)
        of = (BX_CPU_THIS_PTR oszapc.op1_16 >= 0x8000);
      else
        of = 0; /* undocumented, but hardware really does it */
      break;
    case BX_INSTR_SHR32:
      if (BX_CPU_THIS_PTR oszapc.op2_32 == 1)
        of = (BX_CPU_THIS_PTR oszapc.op1_32 >= 0x80000000);
      else
        of = 0; /* undocumented, but hardware really does it */
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_SHR64:
      if (BX_CPU_THIS_PTR oszapc.op2_64 == 1)
        of = (BX_CPU_THIS_PTR oszapc.op1_64 >= BX_CONST64(0x8000000000000000));
      else
        of = 0; /* undocumented, but hardware really does it */
      break;
#endif
    case BX_INSTR_SHRD16:
      /* undocumented, but this formula works right for any shift count */
      of = (((result_16 << 1) ^ result_16) & 0x8000) > 0;
      break;
    case BX_INSTR_SHRD32:
      /* undocumented, but this formula works right for any shift count */
      of = (((result_32 << 1) ^ result_32) & 0x80000000) > 0;
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_SHRD64:
       /* undocumented, but this formula works right for any shift count */
      of = (((result_64 << 1) ^ result_64) & BX_CONST64(0x8000000000000000)) > 0;
      break;
#endif
    case BX_INSTR_SHL8:
      if (BX_CPU_THIS_PTR oszapc.op2_8 == 1)
        of = ((BX_CPU_THIS_PTR oszapc.op1_8 ^ result_8) & 0x80) > 0;
      else
        of = (((BX_CPU_THIS_PTR oszapc.op1_8 <<
               (BX_CPU_THIS_PTR oszapc.op2_8 - 1)) ^ result_8) & 0x80) > 0;
      break;
    case BX_INSTR_SHL16:
      if (BX_CPU_THIS_PTR oszapc.op2_16 == 1)
        of = ((BX_CPU_THIS_PTR oszapc.op1_16 ^ result_16) & 0x8000) > 0;
      else
        of = (((BX_CPU_THIS_PTR oszapc.op1_16 <<
               (BX_CPU_THIS_PTR oszapc.op2_16 - 1)) ^ result_16) & 0x8000) > 0;
      break;
    case BX_INSTR_SHL32:
      if (BX_CPU_THIS_PTR oszapc.op2_32 == 1)
        of = ((BX_CPU_THIS_PTR oszapc.op1_32 ^ result_32) & 0x80000000) > 0;
      else
        of = (((BX_CPU_THIS_PTR oszapc.op1_32 <<
               (BX_CPU_THIS_PTR oszapc.op2_32 - 1)) ^ result_32) & 0x80000000) > 0;
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_SHL64:
      if (BX_CPU_THIS_PTR oszapc.op2_64 == 1)
        of = ((BX_CPU_THIS_PTR oszapc.op1_64 ^ result_64) & BX_CONST64(0x8000000000000000)) > 0;
      else
        of = (((BX_CPU_THIS_PTR oszapc.op1_64 <<
               (BX_CPU_THIS_PTR oszapc.op2_64 - 1)) ^ result_64) & BX_CONST64(0x8000000000000000)) > 0;
      break;
#endif
    case BX_INSTR_IMUL8:
      of = ! ((result_8 < 0x80 &&
               BX_CPU_THIS_PTR oszapc.op2_8 == 0) ||
             ((result_8 & 0x80) && 
               BX_CPU_THIS_PTR oszapc.op2_8 == 0xff));
      break;
    case BX_INSTR_IMUL16:
      of = ! ((result_16 < 0x8000 &&
               BX_CPU_THIS_PTR oszapc.op2_16 == 0) ||
             ((result_16 & 0x8000) && 
               BX_CPU_THIS_PTR oszapc.op2_16 == 0xffff));
      break;
    case BX_INSTR_IMUL32:
      of = ! ((result_32 < 0x80000000 &&
               BX_CPU_THIS_PTR oszapc.op2_32 == 0) ||
             ((result_32 & 0x80000000) && 
               BX_CPU_THIS_PTR oszapc.op2_32 == 0xffffffff));
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_IMUL64:
      of = ! ((result_64 < BX_CONST64(0x8000000000000000) &&
               BX_CPU_THIS_PTR oszapc.op2_64 == 0) ||
             ((result_64 & BX_CONST64(0x8000000000000000)) && 
               BX_CPU_THIS_PTR oszapc.op2_64 == BX_CONST64(0xffffffffffffffff)));
      break;
#endif
    case BX_INSTR_MUL8:
      of = (BX_CPU_THIS_PTR oszapc.op2_8 != 0);
      break;
    case BX_INSTR_MUL16:
      of = (BX_CPU_THIS_PTR oszapc.op2_16 != 0);
      break;
    case BX_INSTR_MUL32:
      of = (BX_CPU_THIS_PTR oszapc.op2_32 != 0);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_MUL64:
      of = (BX_CPU_THIS_PTR oszapc.op2_64 != 0);
      break;
#endif
    case BX_INSTR_NEG8:
    case BX_INSTR_INC8:
      of = (result_8 == 0x80);
      break;
    case BX_INSTR_NEG16:
    case BX_INSTR_INC16:
      of = (result_16 == 0x8000);
      break;
    case BX_INSTR_NEG32:
    case BX_INSTR_INC32:
      of = (result_32 == 0x80000000);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_NEG64:
    case BX_INSTR_INC64:
      of = (result_64 == BX_CONST64(0x8000000000000000));
      break;
#endif
    case BX_INSTR_DEC8:
      of = (result_8 == 0x7F);
      break;
    case BX_INSTR_DEC16:
      of = (result_16 == 0x7FFF);
      break;
    case BX_INSTR_DEC32:
      of = (result_32 == 0x7FFFFFFF);
      break;
#if BX_SUPPORT_X86_64
    case BX_INSTR_DEC64:
      of = (result_64 == BX_CONST64(0x7FFFFFFFFFFFFFFF));
      break;
#endif
    default:
      of = 0; // Keep compiler happy.
      BX_PANIC(("get_OF: OSZAPC: unknown instr"));
    }

    return(of);
}

bx_bool BX_CPU_C::get_PFLazy(void)
{
  return bx_parity_lookup[(Bit8u) BX_CPU_THIS_PTR oszapc.result];
}
