/////////////////////////////////////////////////////////////////////////
// $Id: dis_groups.cc,v 1.3 2001-10-03 13:10:37 bdenney Exp $
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



#include "bochs.h"



// Floating point stuff
  void
bx_disassemble_c::ST_STi(void) {dis_sprintf("*** ST_STi() unfinished ***");}
  void
bx_disassemble_c::STi_ST(void) {dis_sprintf("*** STi_ST() unfinished ***");}
  void
bx_disassemble_c::STi(void) {dis_sprintf("*** STi() unfinished ***");}


// Debug, Test, and Control Register stuff
  void
bx_disassemble_c::RdDd(void) {dis_sprintf("*** RdDd() unfinished ***");}
  void
bx_disassemble_c::DdRd(void) {dis_sprintf("*** DdRd() unfinished ***");}

  void
bx_disassemble_c::RdCd(void)
{
  Bit8u modrm;
  modrm = fetch_byte();
  if ( (modrm >> 6) != 3 ) {
    dis_sprintf("Invalid Opcode");
    }
  else {
    dis_sprintf("%s, CR%u",
      general_32bit_reg_name[modrm&0x7],
      (modrm>>3)&0x7
      );
    }
}

  void
bx_disassemble_c::CdRd(void)
{
  Bit8u modrm;
  modrm = fetch_byte();
  if ( (modrm >> 6) != 3 ) {
    dis_sprintf("Invalid Opcode");
    }
  else {
    dis_sprintf("CR%u, %s",
      (modrm>>3)&0x7,
      general_32bit_reg_name[modrm&0x7]
      );
    }
}

  void
bx_disassemble_c::RdTd(void) {dis_sprintf("*** RdTd() unfinished ***");}
  void
bx_disassemble_c::TdRd(void) {dis_sprintf("*** TdRd() unfinished ***");}


// Other un-implemented operand signatures
  void
bx_disassemble_c::Ms(void) {dis_sprintf("*** Ms() unfinished ***");}
  void
bx_disassemble_c::XBTS(void) {dis_sprintf("*** XBTS() unfinished ***");}
  void
bx_disassemble_c::IBTS(void) {dis_sprintf("*** IBTS() unfinished ***");}
  void
bx_disassemble_c::Mp(void) {dis_sprintf("*** Mp() unfinished ***");}
  void
bx_disassemble_c::GvMa(void) {dis_sprintf("*** GvMa() unfinished ***");}
  void
bx_disassemble_c::EwRw(void) {dis_sprintf("*** EwRw() unfinished ***");}
  void
bx_disassemble_c::YbDX(void) {dis_sprintf("*** YbDX() unfinished ***");}
  void
bx_disassemble_c::YvDX(void) {dis_sprintf("*** YvDX() unfinished ***");}
  void
bx_disassemble_c::DXXb(void) {dis_sprintf("*** DXXb() unfinished ***");}
  void
bx_disassemble_c::DXXv(void) {dis_sprintf("*** DXXv() unfinished ***");}
  void
bx_disassemble_c::ALOb(void) {dis_sprintf("*** ALOb() unfinished ***");}

  void
bx_disassemble_c::eAXOv(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("EAX, ");
    }
  else {
    dis_sprintf("AX, ");
    }

  if (db_32bit_addrsize) {
    Bit32u imm32;

    imm32 = fetch_dword();
    dis_sprintf("[%08x]", (unsigned) imm32);
    }
  else {
    Bit16u imm16;

    imm16 = fetch_word();
    dis_sprintf("[%04x]", (unsigned) imm16);
    }
}

  void
bx_disassemble_c::OveAX(void)
{
  if (db_32bit_addrsize) {
    Bit32u imm32;

    imm32 = fetch_dword();
    dis_sprintf("[%08x], ", (unsigned) imm32);
    }
  else {
    Bit16u imm16;

    imm16 = fetch_word();
    dis_sprintf("[%04x], ", (unsigned) imm16);
    }

  if (db_32bit_opsize) {
    dis_sprintf("EAX");
    }
  else {
    dis_sprintf("AX");
    }
}



  void
bx_disassemble_c::XvYv(void)
{
  char *esi, *edi;
  char *seg;

  if (seg_override)
    seg = seg_override;
  else
    seg = "DS";

  if (db_32bit_addrsize) {
    esi = "ESI";
    edi = "EDI";
    }
  else {
    esi = "SI";
    edi = "DI";
    }

  dis_sprintf("ES:[%s], %s:[%s]", edi, seg, esi);
}

  void
bx_disassemble_c::ObAL(void) {dis_sprintf("*** ObAL() unfinished ***");}

  void
bx_disassemble_c::YbAL(void) {dis_sprintf("*** YbAL() unfinished ***");}
  void
bx_disassemble_c::ALXb(void) {dis_sprintf("*** ALXb() unfinished ***");}
  void
bx_disassemble_c::eAXXv(void) { dis_sprintf("*** eAXXv() unfinished ***"); }
  void
bx_disassemble_c::Es(void) {dis_sprintf("*** Es() unfinished ***");}
  void
bx_disassemble_c::Ea(void) {dis_sprintf("*** Ea() unfinished ***");}
  void
bx_disassemble_c::Et(void) {dis_sprintf("*** Et() unfinished ***");}
  void
bx_disassemble_c::Ed(void) {dis_sprintf("*** Ed() unfinished ***");}
  void
bx_disassemble_c::El(void) {dis_sprintf("*** El() unfinished ***");}
  void
bx_disassemble_c::Eq(void) {dis_sprintf("*** Eq() unfinished ***");}

  void
bx_disassemble_c::GvEb(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize)
    decode_gxex(BX_GENERAL_32BIT_REG, BX_GENERAL_8BIT_REG);
  else
#endif /* BX_CPU_LEVEL > 2 */
    decode_gxex(BX_GENERAL_16BIT_REG, BX_GENERAL_8BIT_REG);
}


  void
bx_disassemble_c::Av(void)
{
  if (db_32bit_opsize) {
    Bit32u imm32;
    imm32 = fetch_dword();
    dis_sprintf("%08x", (unsigned) imm32);
    }
  else {
    Bit16u imm16;
    imm16 = fetch_word();
    dis_sprintf("%04x", (unsigned) imm16);
    }
}

  void
bx_disassemble_c::Eb(void)
{
  decode_exgx(BX_GENERAL_8BIT_REG, BX_NO_REG_TYPE);
}

  void
bx_disassemble_c::Eb1(void)
{
  decode_exgx(BX_GENERAL_8BIT_REG, BX_NO_REG_TYPE);
  dis_sprintf(", 1");
}

  void
bx_disassemble_c::Ev1(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize)
    decode_exgx(BX_GENERAL_32BIT_REG, BX_NO_REG_TYPE);
  else
#endif /* BX_CPU_LEVEL > 2 */
    decode_exgx(BX_GENERAL_16BIT_REG, BX_NO_REG_TYPE);
  dis_sprintf(", 1");
}



  void
bx_disassemble_c::Iw(void)
{
  Bit16u imm16;

  imm16 = fetch_word();
  dis_sprintf("#%04x", (unsigned) imm16);
}



  void
bx_disassemble_c::EbGb(void)
{
  decode_exgx(BX_GENERAL_8BIT_REG, BX_GENERAL_8BIT_REG);
}

  void
bx_disassemble_c::EvGv(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize)
    decode_exgx(BX_GENERAL_32BIT_REG, BX_GENERAL_32BIT_REG);
  else
#endif /* BX_CPU_LEVEL > 2 */
    decode_exgx(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
}

  void
bx_disassemble_c::GbEb(void)
{
  decode_gxex(BX_GENERAL_8BIT_REG, BX_GENERAL_8BIT_REG);
}

  void
bx_disassemble_c::GvEv(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize)
    decode_gxex(BX_GENERAL_32BIT_REG, BX_GENERAL_32BIT_REG);
  else
#endif /* BX_CPU_LEVEL > 2 */
    decode_gxex(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
}


  void
bx_disassemble_c::Ew(void)
{
  decode_exgx(BX_GENERAL_16BIT_REG, BX_NO_REG_TYPE);
}

  void
bx_disassemble_c::GvEw(void)
{
  if (db_32bit_opsize)
    decode_gxex(BX_GENERAL_32BIT_REG, BX_GENERAL_16BIT_REG);
  else
    decode_gxex(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
}


  void
bx_disassemble_c::Jv(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize) {
    Bit32u imm32;

    imm32 = fetch_dword();
    dis_sprintf("+#%08x", (unsigned) imm32);
    }
  else
#endif
    {
    Bit16u imm16;

    imm16 = fetch_word();
    dis_sprintf("+#%04x", (unsigned) imm16);
    }
}


  void
bx_disassemble_c::EvIb(void)
{
  Bit8u imm8;

#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize) {
    decode_exgx(BX_GENERAL_32BIT_REG, BX_NO_REG_TYPE);
    imm8 = fetch_byte();
    dis_sprintf(", #%02x", (unsigned) imm8);
    }
  else {
#endif /* BX_CPU_LEVEL > 2 */
    decode_exgx(BX_GENERAL_16BIT_REG, BX_NO_REG_TYPE);
    imm8 = fetch_byte();
    dis_sprintf(", #%02x", (unsigned) imm8);
#if BX_CPU_LEVEL > 2
    }
#endif /* BX_CPU_LEVEL > 2 */
}


  void
bx_disassemble_c::Iv(void)
{
  if (db_32bit_opsize) {
    Bit32u imm32;

    imm32 = fetch_dword();
    dis_sprintf("#%08x", (unsigned) imm32);
    }
  else {
    Bit16u imm16;

    imm16 = fetch_word();
    dis_sprintf("#%04x", (unsigned) imm16);
    }
}


  void
bx_disassemble_c::Ib(void)
{
  Bit8u imm8;

  imm8 = fetch_byte();
  dis_sprintf("#%02x", imm8);
}


  void
bx_disassemble_c::Jb(void)
{
  Bit8u imm8;

  imm8 = fetch_byte();
  dis_sprintf("+#%02x", (unsigned) imm8);
}

  void
bx_disassemble_c::EbIb(void)
{
  Bit8u imm8;

  decode_exgx(BX_GENERAL_8BIT_REG, BX_NO_REG_TYPE);
  imm8 = fetch_byte();
  dis_sprintf(", #%02x", (unsigned) imm8);
}

  void
bx_disassemble_c::EvIv(void)
{
  Bit16u imm16;

#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize) {
    Bit32u imm32;

    decode_exgx(BX_GENERAL_32BIT_REG, BX_NO_REG_TYPE);
    imm32 = fetch_dword();
    dis_sprintf(", #%08x", (unsigned) imm32);
    }
  else {
#endif /* BX_CPU_LEVEL > 2 */
    decode_exgx(BX_GENERAL_16BIT_REG, BX_NO_REG_TYPE);
    imm16 = fetch_word();
    dis_sprintf(", #%04x", (unsigned) imm16);
#if BX_CPU_LEVEL > 2
    }
#endif /* BX_CPU_LEVEL > 2 */
}

  void
bx_disassemble_c::EwSw(void)
{
  decode_exgx(BX_GENERAL_16BIT_REG, BX_SEGMENT_REG);
}

  void
bx_disassemble_c::GvM(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize)
    decode_gxex(BX_GENERAL_32BIT_REG, BX_GENERAL_32BIT_REG);
  else
#endif /* BX_CPU_LEVEL > 2 */
    decode_gxex(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
}

  void
bx_disassemble_c::SwEw(void)
{
  decode_gxex(BX_SEGMENT_REG, BX_GENERAL_16BIT_REG);
}

  void
bx_disassemble_c::Ev(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize) {
    decode_exgx(BX_GENERAL_32BIT_REG, BX_NO_REG_TYPE);
    }
  else {
#endif /* BX_CPU_LEVEL > 2 */
    decode_exgx(BX_GENERAL_16BIT_REG, BX_NO_REG_TYPE);
#if BX_CPU_LEVEL > 2
    }
#endif /* BX_CPU_LEVEL > 2 */
}

  void
bx_disassemble_c::Ap(void)
{

#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize) {
    Bit32u imm32;
    Bit16u cs_selector;

    imm32 = fetch_dword();
    cs_selector = fetch_word();
    dis_sprintf("%04x:%08x", (unsigned) cs_selector, (unsigned) imm32);
    }
  else
#endif /* BX_CPU_LEVEL > 2 */
    {
    Bit16u imm16;
    Bit16u cs_selector;

    imm16 = fetch_word();
    cs_selector = fetch_word();
    dis_sprintf("%04x:%04x", (unsigned) cs_selector, (unsigned) imm16);
    }
}


  void
bx_disassemble_c::XbYb(void)
{
  char *esi, *edi;
  char *seg;

  if (db_32bit_addrsize) {
    esi = "ESI";
    edi = "EDI";
    }
  else {
    esi = "SI";
    edi = "DI";
    }

  if (seg_override)
    seg = seg_override;
  else
    seg = "DS";

  dis_sprintf("ES:[%s], %s:[%s]", edi, seg, esi);
}


  void
bx_disassemble_c::YveAX(void)
{
  char *eax, *edi;

  if (db_32bit_opsize)
    eax = "EAX";
  else
    eax = "AX";

  if (db_32bit_addrsize)
    edi = "EDI";
  else
    edi = "DI";

  dis_sprintf("ES:[%s], %s", edi, eax);
}

  void
bx_disassemble_c::GvMp(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize)
    decode_gxex(BX_GENERAL_32BIT_REG, BX_GENERAL_32BIT_REG);
  else
#endif /* BX_CPU_LEVEL > 2 */
    decode_gxex(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
}

  void
bx_disassemble_c::eAXEv(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize) {
    dis_sprintf("EAX, ");
    decode_gxex(BX_NO_REG_TYPE, BX_GENERAL_32BIT_REG);
    }
  else {
#endif /* BX_CPU_LEVEL > 2 */
    dis_sprintf("AX, ");
    decode_gxex(BX_NO_REG_TYPE, BX_GENERAL_16BIT_REG);
#if BX_CPU_LEVEL > 2
    }
#endif /* BX_CPU_LEVEL > 2 */
}

  void
bx_disassemble_c::Ep(void)
{
#if BX_CPU_LEVEL > 2
  if (db_32bit_opsize) {
    decode_exgx(BX_GENERAL_32BIT_REG, BX_NO_REG_TYPE);
    }
  else {
#endif /* BX_CPU_LEVEL > 2 */
    decode_exgx(BX_GENERAL_16BIT_REG, BX_NO_REG_TYPE);
#if BX_CPU_LEVEL > 2
    }
#endif /* BX_CPU_LEVEL > 2 */
}

  void
bx_disassemble_c::eAX(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("EAX");
    }
  else {
    dis_sprintf("AX");
    }
}

  void
bx_disassemble_c::eCX(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("ECX");
    }
  else {
    dis_sprintf("CX");
    }
}

  void
bx_disassemble_c::eDX(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("EDX");
    }
  else {
    dis_sprintf("DX");
    }
}

  void
bx_disassemble_c::eBX(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("EBX");
    }
  else {
    dis_sprintf("BX");
    }
}

  void
bx_disassemble_c::eSP(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("ESP");
    }
  else {
    dis_sprintf("SP");
    }
}

  void
bx_disassemble_c::eBP(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("EBP");
    }
  else {
    dis_sprintf("BP");
    }
}

  void
bx_disassemble_c::eSI(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("ESI");
    }
  else {
    dis_sprintf("SI");
    }
}

  void
bx_disassemble_c::eDI(void)
{
  if (db_32bit_opsize) {
    dis_sprintf("EDI");
    }
  else {
    dis_sprintf("DI");
    }
}
