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
#define LOG_THIS bx_disassemble.

bx_disassemble_c bx_disassemble;


bx_disassemble_c::bx_disassemble_c(void)
{
setprefix("[DIS ]",__FILE__,__LINE__);
settype(DISLOG);
setio(SAFE_GET_IOFUNC());

sreg_mod01_rm32[0]   = "DS";
sreg_mod01_rm32[1]   = "DS";
sreg_mod01_rm32[2]   = "DS";
sreg_mod01_rm32[3]   = "DS";
sreg_mod01_rm32[4]   = "??";
sreg_mod01_rm32[5]   = "SS";
sreg_mod01_rm32[6]   = "DS";
sreg_mod01_rm32[7]   = "DS";

sreg_mod10_rm32[0]   = "DS";
sreg_mod10_rm32[1]   = "DS";
sreg_mod10_rm32[2]   = "DS";
sreg_mod10_rm32[3]   = "DS";
sreg_mod10_rm32[4]   = "??";
sreg_mod10_rm32[5]   = "SS";
sreg_mod10_rm32[6]   = "DS";
sreg_mod10_rm32[7]   = "DS";

sreg_mod00_base32[0] = "DS";
sreg_mod00_base32[1] = "DS";
sreg_mod00_base32[2] = "DS";
sreg_mod00_base32[3] = "DS";
sreg_mod00_base32[4] = "SS";
sreg_mod00_base32[5] = "DS";
sreg_mod00_base32[6] = "DS";
sreg_mod00_base32[7] = "DS";

sreg_mod01_base32[0] = "DS";
sreg_mod01_base32[1] = "DS";
sreg_mod01_base32[2] = "DS";
sreg_mod01_base32[3] = "DS";
sreg_mod01_base32[4] = "SS";
sreg_mod01_base32[5] = "SS";
sreg_mod01_base32[6] = "DS";
sreg_mod01_base32[7] = "DS";

sreg_mod10_base32[0] = "DS";
sreg_mod10_base32[1] = "DS";
sreg_mod10_base32[2] = "DS";
sreg_mod10_base32[3] = "DS";
sreg_mod10_base32[4] = "SS";
sreg_mod10_base32[5] = "SS";
sreg_mod10_base32[6] = "DS";
sreg_mod10_base32[7] = "DS";

sreg_mod00_rm16[0]   = "DS";
sreg_mod00_rm16[1]   = "DS";
sreg_mod00_rm16[2]   = "SS";
sreg_mod00_rm16[3]   = "SS";
sreg_mod00_rm16[4]   = "DS";
sreg_mod00_rm16[5]   = "DS";
sreg_mod00_rm16[6]   = "DS";
sreg_mod00_rm16[7]   = "DS";

sreg_mod01_rm16[0]   = "DS";
sreg_mod01_rm16[1]   = "DS";
sreg_mod01_rm16[2]   = "SS";
sreg_mod01_rm16[3]   = "SS";
sreg_mod01_rm16[4]   = "DS";
sreg_mod01_rm16[5]   = "DS";
sreg_mod01_rm16[6]   = "SS";
sreg_mod01_rm16[7]   = "DS";

sreg_mod10_rm16[0]   = "DS";
sreg_mod10_rm16[1]   = "DS";
sreg_mod10_rm16[2]   = "SS";
sreg_mod10_rm16[3]   = "SS";
sreg_mod10_rm16[4]   = "DS";
sreg_mod10_rm16[5]   = "DS";
sreg_mod10_rm16[6]   = "SS";
sreg_mod10_rm16[7]   = "DS";


segment_name[0] = "ES";
segment_name[1] = "CS";
segment_name[2] = "SS";
segment_name[3] = "DS";
segment_name[4] = "FS";
segment_name[5] = "GS";
segment_name[6] = "??";
segment_name[7] = "??";

general_8bit_reg_name[0] = "AL";
general_8bit_reg_name[1] = "CL";
general_8bit_reg_name[2] = "DL";
general_8bit_reg_name[3] = "BL";
general_8bit_reg_name[4] = "AH";
general_8bit_reg_name[5] = "CH";
general_8bit_reg_name[6] = "DH";
general_8bit_reg_name[7] = "BH";

general_16bit_reg_name[0] = "AX";
general_16bit_reg_name[1] = "CX";
general_16bit_reg_name[2] = "DX";
general_16bit_reg_name[3] = "BX";
general_16bit_reg_name[4] = "SP";
general_16bit_reg_name[5] = "BP";
general_16bit_reg_name[6] = "SI";
general_16bit_reg_name[7] = "DI";

general_32bit_reg_name[0] = "EAX";
general_32bit_reg_name[1] = "ECX";
general_32bit_reg_name[2] = "EDX";
general_32bit_reg_name[3] = "EBX";
general_32bit_reg_name[4] = "ESP";
general_32bit_reg_name[5] = "EBP";
general_32bit_reg_name[6] = "ESI";
general_32bit_reg_name[7] = "EDI";

base_name16[0] =  "BX";
base_name16[1] =  "BX";
base_name16[2] =  "BP";
base_name16[3] =  "BP";
base_name16[4] =  "??";
base_name16[5] =  "??";
base_name16[6] =  "BP";
base_name16[7] =  "BX";

index_name16[0] = "SI";
index_name16[1] = "DI";
index_name16[2] = "SI";
index_name16[3] = "DI";
index_name16[4] = "SI";
index_name16[5] = "DI";
index_name16[6] = "??";
index_name16[7] = "??";

index_name32[0] =  "EAX";
index_name32[1] =  "ECX";
index_name32[2] =  "EDX";
index_name32[3] =  "EBX";
index_name32[4] =  "???";
index_name32[5] =  "EBP";
index_name32[6] =  "ESI";
index_name32[7] =  "EDI";
}



  unsigned
bx_disassemble_c::disasm(Boolean is_32, Bit8u *instr, char *disbuf)
{
  int byte_count;
  Bit8u next_byte;
  Bit8u mod_rm_byte, mod, opcode, rm;

  db_32bit_opsize = is_32;
  db_32bit_addrsize = is_32;
  instruction_begin = instruction = instr;

  seg_override = NULL;
  db_rep_prefix = 0;
  db_repne_prefix = 0;

  disbufptr = disbuf; // start sprintf()'ing into beginning of buffer

  /* On the 386 and above, instructions must be a maximum of 15 bytes long.
   * this means redundant prefix codes can put the byte count over 15 and
   * cause an illegal instruction.
   */
  for (byte_count=0; byte_count<15; byte_count++) {
    next_byte = fetch_byte();
    switch (next_byte) {

      case 0x00: dis_sprintf("add "); EbGb(); goto done;
      case 0x01: dis_sprintf("add "); EvGv(); goto done;
      case 0x02: dis_sprintf("add "); GbEb(); goto done;
      case 0x03: dis_sprintf("add "); GvEv(); goto done;
      case 0x04: dis_sprintf("add AL, "); Ib(); goto done;
      case 0x05: dis_sprintf("add "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0x06: dis_sprintf("push es"); goto done;
      case 0x07: dis_sprintf("pop es"); goto done;
      case 0x08: dis_sprintf("or "); EbGb(); goto done;
      case 0x09: dis_sprintf("or "); EvGv(); goto done;
      case 0x0A: dis_sprintf("or "); GbEb(); goto done;
      case 0x0B: dis_sprintf("or "); GvEv(); goto done;
      case 0x0C: dis_sprintf("or AL, "); Ib(); goto done;
      case 0x0D: dis_sprintf("or "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0x0E: dis_sprintf("push cs"); goto done;
      case 0x0F: /* 2-byte escape */
        next_byte = fetch_byte();
        if ( ++byte_count >= 15 ) {
          invalid_opcode();
          goto done;
	  }
        switch (next_byte) {
          case 0x00: /* Group 6 */
            mod_rm_byte = peek_byte();
            BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
            switch (opcode) {
              case 0x00: dis_sprintf("sldt "); Ew(); goto done;
              case 0x01: dis_sprintf("str "); Ew(); goto done;
              case 0x02: dis_sprintf("lldt "); Ew(); goto done;
              case 0x03: dis_sprintf("ltr "); Ew(); goto done;
              case 0x04: dis_sprintf("verr "); Ew(); goto done;
              case 0x05: dis_sprintf("verw "); Ew(); goto done;
              case 0x06: invalid_opcode(); goto done;
              case 0x07: invalid_opcode(); goto done;
              default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
              }
            goto done;
          case 0x01: /* Group 7 */
            mod_rm_byte = peek_byte();
            BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
            switch (opcode) {
              case 0x00: dis_sprintf("sgdt "); Ms(); goto done;
              case 0x01: dis_sprintf("sidt "); Ms(); goto done;
              case 0x02: dis_sprintf("lgdt "); Ms(); goto done;
              case 0x03: dis_sprintf("lidt "); Ms(); goto done;
              case 0x04: dis_sprintf("smsw "); Ew(); goto done;
              case 0x05: invalid_opcode(); goto done;
              case 0x06: dis_sprintf("lmsw "); Ew(); goto done;
              case 0x07: invalid_opcode(); goto done;
              default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
              }
            goto done;

          case 0x02: dis_sprintf("lar "); GvEw(); goto done;
          case 0x03: dis_sprintf("lsl "); GvEw(); goto done;
          case 0x04: invalid_opcode(); goto done;
          case 0x05: invalid_opcode(); goto done;
          case 0x06: dis_sprintf("clts"); goto done;
          case 0x07: invalid_opcode(); goto done;
          case 0x08: dis_sprintf("invd"); goto done;
          case 0x09: dis_sprintf("wbinvd"); goto done;
	  case 0x0A:
	  case 0x0B:
	  case 0x0C:
	  case 0x0D:
	  case 0x0E:
	  case 0x0F: invalid_opcode(); goto done;

          case 0x10:
	  case 0x11:
	  case 0x12:
	  case 0x13:
	  case 0x14:
	  case 0x15:
	  case 0x16:
	  case 0x17:
	  case 0x18:
	  case 0x19:
	  case 0x1A:
	  case 0x1B:
	  case 0x1C:
	  case 0x1D:
	  case 0x1E:
	  case 0x1F: invalid_opcode(); goto done;

          case 0x20: dis_sprintf("mov "); RdCd(); goto done;
          case 0x21: dis_sprintf("mov "); RdDd(); goto done;
          case 0x22: dis_sprintf("mov "); CdRd(); goto done;
          case 0x23: dis_sprintf("mov "); DdRd(); goto done;
          case 0x24: dis_sprintf("mov "); RdTd(); goto done;
          case 0x25: invalid_opcode(); goto done;
          case 0x26: dis_sprintf("mov "); TdRd(); goto done;
          case 0x27:
          case 0x28:
          case 0x29:
          case 0x2A:
          case 0x2B:
          case 0x2C:
          case 0x2D:
          case 0x2E:
          case 0x2F: invalid_opcode(); goto done;

          case 0x30:
	  case 0x31:
	  case 0x32:
	  case 0x33:
	  case 0x34:
	  case 0x35:
	  case 0x36:
	  case 0x37:
	  case 0x38:
	  case 0x39:
	  case 0x3A:
	  case 0x3B:
	  case 0x3C:
	  case 0x3D:
	  case 0x3E:
	  case 0x3F: invalid_opcode(); goto done;

          case 0x40:
	  case 0x41:
	  case 0x42:
	  case 0x43:
	  case 0x44:
	  case 0x45:
	  case 0x46:
	  case 0x47:
	  case 0x48:
	  case 0x49:
	  case 0x4A:
	  case 0x4B:
	  case 0x4C:
	  case 0x4D:
	  case 0x4E:
	  case 0x4F: invalid_opcode(); goto done;

          case 0x50:
	  case 0x51:
	  case 0x52:
	  case 0x53:
	  case 0x54:
	  case 0x55:
	  case 0x56:
	  case 0x57:
	  case 0x58:
	  case 0x59:
	  case 0x5A:
	  case 0x5B:
	  case 0x5C:
	  case 0x5D:
	  case 0x5E:
	  case 0x5F: invalid_opcode(); goto done;

          case 0x60:
	  case 0x61:
	  case 0x62:
	  case 0x63:
	  case 0x64:
	  case 0x65:
	  case 0x66:
	  case 0x67:
	  case 0x68:
	  case 0x69:
	  case 0x6A:
	  case 0x6B:
	  case 0x6C:
	  case 0x6D:
	  case 0x6E:
	  case 0x6F: invalid_opcode(); goto done;

          case 0x70:
	  case 0x71:
	  case 0x72:
	  case 0x73:
	  case 0x74:
	  case 0x75:
	  case 0x76:
	  case 0x77:
	  case 0x78:
	  case 0x79:
	  case 0x7A:
	  case 0x7B:
	  case 0x7C:
	  case 0x7D:
	  case 0x7E:
	  case 0x7F: invalid_opcode(); goto done;

          case 0x80: dis_sprintf("jo "); Jv(); goto done;
          case 0x81: dis_sprintf("jno "); Jv(); goto done;
          case 0x82: dis_sprintf("jb "); Jv(); goto done;
          case 0x83: dis_sprintf("jnb "); Jv(); goto done;
          case 0x84: dis_sprintf("jz "); Jv(); goto done;
          case 0x85: dis_sprintf("jnz "); Jv(); goto done;
          case 0x86: dis_sprintf("jbe "); Jv(); goto done;
          case 0x87: dis_sprintf("jnbe "); Jv(); goto done;
          case 0x88: dis_sprintf("js "); Jv(); goto done;
          case 0x89: dis_sprintf("jns "); Jv(); goto done;
          case 0x8A: dis_sprintf("jp "); Jv(); goto done;
          case 0x8B: dis_sprintf("jnp "); Jv(); goto done;
          case 0x8C: dis_sprintf("jl "); Jv(); goto done;
          case 0x8D: dis_sprintf("jnl "); Jv(); goto done;
          case 0x8E: dis_sprintf("jle "); Jv(); goto done;
          case 0x8F: dis_sprintf("jnle "); Jv(); goto done;

          case 0x90: dis_sprintf("seto "); Eb(); goto done;
          case 0x91: dis_sprintf("setno "); Eb(); goto done;
          case 0x92: dis_sprintf("setb "); Eb(); goto done;
          case 0x93: dis_sprintf("setnb "); Eb(); goto done;
          case 0x94: dis_sprintf("setz "); Eb(); goto done;
          case 0x95: dis_sprintf("setnz "); Eb(); goto done;
          case 0x96: dis_sprintf("setbe "); Eb(); goto done;
          case 0x97: dis_sprintf("setnbe "); Eb(); goto done;
          case 0x98: dis_sprintf("sets "); Eb(); goto done;
          case 0x99: dis_sprintf("setns "); Eb(); goto done;
          case 0x9A: dis_sprintf("setp "); Eb(); goto done;
          case 0x9B: dis_sprintf("setnp "); Eb(); goto done;
          case 0x9C: dis_sprintf("setl "); Eb(); goto done;
          case 0x9D: dis_sprintf("setnl "); Eb(); goto done;
          case 0x9E: dis_sprintf("setle "); Eb(); goto done;
          case 0x9F: dis_sprintf("setnle "); Eb(); goto done;

          case 0xA0: dis_sprintf("push fs"); goto done;
          case 0xA1: dis_sprintf("pop fs"); goto done;
          case 0xA2: invalid_opcode(); goto done;
          case 0xA3: dis_sprintf("bt "); EvGv(); goto done;
          case 0xA4: dis_sprintf("shld "); EvGv(); dis_sprintf(", "); Ib(); goto done;
          case 0xA5: dis_sprintf("shld "); EvGv(); dis_sprintf(", CL"); goto done;
          case 0xA6: dis_sprintf("cmpxchg "); XBTS(); goto done;
          case 0xA7: dis_sprintf("cmpxchg "); IBTS(); goto done;
          case 0xA8: dis_sprintf("push gs"); goto done;
          case 0xA9: dis_sprintf("pop gs"); goto done;
          case 0xAA: invalid_opcode(); goto done;
          case 0xAB: dis_sprintf("bts "); EvGv(); goto done;
          case 0xAC: dis_sprintf("shrd "); EvGv(); dis_sprintf(", "); Ib(); goto done;
          case 0xAD: dis_sprintf("shrd "); EvGv(); dis_sprintf(", CL"); goto done;
          case 0xAE: invalid_opcode(); goto done;
          case 0xAF: dis_sprintf("imul "); GvEv(); goto done;

          case 0xB0: dis_sprintf("cmpxchg "); EbGb(); goto done;
          case 0xB1: dis_sprintf("cmpxchg "); EvGv(); goto done;
          case 0xB2: dis_sprintf("lss "); Mp(); goto done;
          case 0xB3: dis_sprintf("btr "); EvGv(); goto done;
          case 0xB4: dis_sprintf("lfs "); Mp(); goto done;
          case 0xB5: dis_sprintf("lgs "); Mp(); goto done;
          case 0xB6: dis_sprintf("movzx "); GvEb(); goto done;
          case 0xB7: dis_sprintf("movzx "); GvEw(); goto done;
          case 0xB8: invalid_opcode(); goto done;
          case 0xB9: invalid_opcode(); goto done;
          case 0xBA: /* Group 8 Ev,Ib */
            mod_rm_byte = peek_byte();
            BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
            switch (opcode) {
              case 0x00:
              case 0x01:
              case 0x02:
              case 0x03: invalid_opcode(); goto done;
              case 0x04: dis_sprintf("bt "); EvIb(); goto done;
              case 0x05: dis_sprintf("bts "); EvIb(); goto done;
              case 0x06: dis_sprintf("btr "); EvIb(); goto done;
              case 0x07: dis_sprintf("btc "); EvIb(); goto done;
              default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
              }
            goto done;

          case 0xBB: dis_sprintf("btc "); EvGv(); goto done;
          case 0xBC: dis_sprintf("bsf "); GvEv(); goto done;
          case 0xBD: dis_sprintf("bsr "); GvEv(); goto done;
          case 0xBE: dis_sprintf("movsx "); GvEb(); goto done;
          case 0xBF: dis_sprintf("movsx "); GvEw(); goto done;

          case 0xC0: dis_sprintf("xadd "); EbGb(); goto done;
          case 0xC1: dis_sprintf("xadd "); EvGv(); goto done;
          case 0xC2:
          case 0xC3:
          case 0xC4:
          case 0xC5:
          case 0xC6:
          case 0xC7: invalid_opcode(); goto done;
          case 0xC8: dis_sprintf("bswap "); eAX(); goto done;
          case 0xC9: dis_sprintf("bswap "); eCX(); goto done;
          case 0xCA: dis_sprintf("bswap "); eDX(); goto done;
          case 0xCB: dis_sprintf("bswap "); eBX(); goto done;
          case 0xCC: dis_sprintf("bswap "); eSP(); goto done;
          case 0xCD: dis_sprintf("bswap "); eBP(); goto done;
          case 0xCE: dis_sprintf("bswap "); eSI(); goto done;
          case 0xCF: dis_sprintf("bswap "); eDI(); goto done;

          case 0xD0:
	  case 0xD1:
	  case 0xD2:
	  case 0xD3:
	  case 0xD4:
	  case 0xD5:
	  case 0xD6:
	  case 0xD7:
	  case 0xD8:
	  case 0xD9:
	  case 0xDA:
	  case 0xDB:
	  case 0xDC:
	  case 0xDD:
	  case 0xDE:
	  case 0xDF: invalid_opcode(); goto done;

          case 0xE0:
	  case 0xE1:
	  case 0xE2:
	  case 0xE3:
	  case 0xE4:
	  case 0xE5:
	  case 0xE6:
	  case 0xE7:
	  case 0xE8:
	  case 0xE9:
	  case 0xEA:
	  case 0xEB:
	  case 0xEC:
	  case 0xED:
	  case 0xEE:
	  case 0xEF: invalid_opcode(); goto done;

          case 0xF0:
	  case 0xF1:
	  case 0xF2:
	  case 0xF3:
	  case 0xF4:
	  case 0xF5:
	  case 0xF6:
	  case 0xF7:
	  case 0xF8:
	  case 0xF9:
	  case 0xFA:
	  case 0xFB:
	  case 0xFC:
	  case 0xFD:
	  case 0xFE:
	  case 0xFF: invalid_opcode(); goto done;

          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0x10: dis_sprintf("adc "); EbGb(); goto done;
      case 0x11: dis_sprintf("adc "); EvGv(); goto done;
      case 0x12: dis_sprintf("adc "); GbEb(); goto done;
      case 0x13: dis_sprintf("adc "); GvEv(); goto done;
      case 0x14: dis_sprintf("adc AL, "); Ib(); goto done;
      case 0x15: dis_sprintf("adc "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0x16: dis_sprintf("push ss"); goto done;
      case 0x17: dis_sprintf("pop ss"); goto done;
      case 0x18: dis_sprintf("sbb "); EbGb(); goto done;
      case 0x19: dis_sprintf("sbb "); EvGv(); goto done;
      case 0x1A: dis_sprintf("sbb "); GbEb(); goto done;
      case 0x1B: dis_sprintf("sbb "); GvEv(); goto done;
      case 0x1C: dis_sprintf("sbb AL, "); Ib(); goto done;
      case 0x1D: dis_sprintf("sbb "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0x1E: dis_sprintf("push ds"); goto done;
      case 0x1F: dis_sprintf("pop ds"); goto done;


      case 0x20: dis_sprintf("and "); EbGb(); goto done;
      case 0x21: dis_sprintf("and "); EvGv(); goto done;
      case 0x22: dis_sprintf("and "); GbEb(); goto done;
      case 0x23: dis_sprintf("and "); GvEv(); goto done;
      case 0x24: dis_sprintf("and AL, "); Ib(); goto done;
      case 0x25: dis_sprintf("and "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0x26:
        seg_override = "ES";
        dis_sprintf("ES: ");
        break;
      case 0x27: dis_sprintf("daa"); goto done;
      case 0x28: dis_sprintf("sub "); EbGb(); goto done;
      case 0x29: dis_sprintf("sub "); EvGv(); goto done;
      case 0x2A: dis_sprintf("sub "); GbEb(); goto done;
      case 0x2B: dis_sprintf("sub "); GvEv(); goto done;
      case 0x2C: dis_sprintf("sub AL, "); Ib(); goto done;
      case 0x2D: dis_sprintf("sub "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0x2E:
        seg_override = "CS";
        dis_sprintf("CS: ");
        break;
      case 0x2F: dis_sprintf("das"); goto done;

      case 0x30: dis_sprintf("xor "); EbGb(); goto done;
      case 0x31: dis_sprintf("xor "); EvGv(); goto done;
      case 0x32: dis_sprintf("xor "); GbEb(); goto done;
      case 0x33: dis_sprintf("xor "); GvEv(); goto done;
      case 0x34: dis_sprintf("xor AL, "); Ib(); goto done;
      case 0x35: dis_sprintf("xor "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0x36:
        seg_override = "SS";
        dis_sprintf("SS: ");
        break;
      case 0x37: dis_sprintf("aaa"); goto done;
      case 0x38: dis_sprintf("cmp "); EbGb(); goto done;
      case 0x39: dis_sprintf("cmp "); EvGv(); goto done;
      case 0x3A: dis_sprintf("cmp "); GbEb(); goto done;
      case 0x3B: dis_sprintf("cmp "); GvEv(); goto done;
      case 0x3C: dis_sprintf("cmp AL, "); Ib(); goto done;
      case 0x3D: dis_sprintf("cmp "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0x3E:
        seg_override = "DS";
        dis_sprintf("DS: ");
        break;
      case 0x3F: dis_sprintf("aas"); goto done;

      case 0x40: dis_sprintf("inc "); eAX(); goto done;
      case 0x41: dis_sprintf("inc "); eCX(); goto done;
      case 0x42: dis_sprintf("inc "); eDX(); goto done;
      case 0x43: dis_sprintf("inc "); eBX(); goto done;
      case 0x44: dis_sprintf("inc "); eSP(); goto done;
      case 0x45: dis_sprintf("inc "); eBP(); goto done;
      case 0x46: dis_sprintf("inc "); eSI(); goto done;
      case 0x47: dis_sprintf("inc "); eDI(); goto done;
      case 0x48: dis_sprintf("dec "); eAX(); goto done;
      case 0x49: dis_sprintf("dec "); eCX(); goto done;
      case 0x4A: dis_sprintf("dec "); eDX(); goto done;
      case 0x4B: dis_sprintf("dec "); eBX(); goto done;
      case 0x4C: dis_sprintf("dec "); eSP(); goto done;
      case 0x4D: dis_sprintf("dec "); eBP(); goto done;
      case 0x4E: dis_sprintf("dec "); eSI(); goto done;
      case 0x4F: dis_sprintf("dec "); eDI(); goto done;


      case 0x50: dis_sprintf("push "); eAX(); goto done;
      case 0x51: dis_sprintf("push "); eCX(); goto done;
      case 0x52: dis_sprintf("push "); eDX(); goto done;
      case 0x53: dis_sprintf("push "); eBX(); goto done;
      case 0x54: dis_sprintf("push "); eSP(); goto done;
      case 0x55: dis_sprintf("push "); eBP(); goto done;
      case 0x56: dis_sprintf("push "); eSI(); goto done;
      case 0x57: dis_sprintf("push "); eDI(); goto done;
      case 0x58: dis_sprintf("pop "); eAX(); goto done;
      case 0x59: dis_sprintf("pop "); eCX(); goto done;
      case 0x5A: dis_sprintf("pop "); eDX(); goto done;
      case 0x5B: dis_sprintf("pop "); eBX(); goto done;
      case 0x5C: dis_sprintf("pop "); eSP(); goto done;
      case 0x5D: dis_sprintf("pop "); eBP(); goto done;
      case 0x5E: dis_sprintf("pop "); eSI(); goto done;
      case 0x5F: dis_sprintf("pop "); eDI(); goto done;


      case 0x60: dis_sprintf("pushad"); goto done;
      case 0x61: dis_sprintf("popad"); goto done;
      case 0x62: dis_sprintf("bound "); GvMa(); goto done;
      case 0x63: dis_sprintf("arpl "); EwRw(); goto done;
      case 0x64:
        seg_override = "FS";
        dis_sprintf("FS: ");
        break;
      case 0x65:
        seg_override = "GS";
        dis_sprintf("GS: ");
        break;
      case 0x66:
        db_32bit_opsize = !db_32bit_opsize;
        dis_sprintf("OPSIZE: ");
        break;
      case 0x67:
        db_32bit_addrsize = !db_32bit_addrsize;
        dis_sprintf("ADDRSIZE: ");
        break;
      case 0x68: dis_sprintf("push "); Iv(); goto done;
      case 0x69: dis_sprintf("imul "); GvEv(); dis_sprintf(", "); Iv(); goto done;
      case 0x6A: dis_sprintf("push "); Ib(); goto done;
      case 0x6B: dis_sprintf("imul "); GvEv(); dis_sprintf(", "); Ib(); goto done;
      case 0x6C: dis_sprintf("insb "); YbDX(); goto done;
      case 0x6D: dis_sprintf("insw "); YvDX(); goto done;
      case 0x6E: dis_sprintf("outsb "); DXXb(); goto done;
      case 0x6F: dis_sprintf("outsw "); DXXv(); goto done;


      case 0x70: dis_sprintf("jo "); Jb(); goto done;
      case 0x71: dis_sprintf("jno "); Jb(); goto done;
      case 0x72: dis_sprintf("jb "); Jb(); goto done;
      case 0x73: dis_sprintf("jnb "); Jb(); goto done;
      case 0x74: dis_sprintf("jz "); Jb(); goto done;
      case 0x75: dis_sprintf("jnz "); Jb(); goto done;
      case 0x76: dis_sprintf("jbe "); Jb(); goto done;
      case 0x77: dis_sprintf("jnbe "); Jb(); goto done;
      case 0x78: dis_sprintf("js "); Jb(); goto done;
      case 0x79: dis_sprintf("jns "); Jb(); goto done;
      case 0x7A: dis_sprintf("jp "); Jb(); goto done;
      case 0x7B: dis_sprintf("jnp "); Jb(); goto done;
      case 0x7C: dis_sprintf("jl "); Jb(); goto done;
      case 0x7D: dis_sprintf("jnl "); Jb(); goto done;
      case 0x7E: dis_sprintf("jle "); Jb(); goto done;
      case 0x7F: dis_sprintf("jnle "); Jb(); goto done;

      case 0x80: /* Immdediate Grp 1 EbIb */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("add "); EbIb(); goto done;
          case 0x01: dis_sprintf("or "); EbIb();  goto done;
          case 0x02: dis_sprintf("adc "); EbIb(); goto done;
          case 0x03: dis_sprintf("sbb "); EbIb(); goto done;
          case 0x04: dis_sprintf("and "); EbIb(); goto done;
          case 0x05: dis_sprintf("sub "); EbIb(); goto done;
          case 0x06: dis_sprintf("xor "); EbIb(); goto done;
          case 0x07: dis_sprintf("cmp "); EbIb(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;
        break;

      case 0x81: /* Immdediate Grp 1 EvIv */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("add "); EvIv(); goto done;
          case 0x01: dis_sprintf("or "); EvIv(); goto done;
          case 0x02: dis_sprintf("adc "); EvIv(); goto done;
          case 0x03: dis_sprintf("sbb "); EvIv(); goto done;
          case 0x04: dis_sprintf("and "); EvIv(); goto done;
          case 0x05: dis_sprintf("sub "); EvIv(); goto done;
          case 0x06: dis_sprintf("xor "); EvIv(); goto done;
          case 0x07: dis_sprintf("cmp "); EvIv(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0x82: invalid_opcode(); goto done;

      case 0x83: /* Immdediate Grp 1 EvIb */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("add "); EvIb(); goto done;
          case 0x01: dis_sprintf("or "); EvIb(); goto done;
          case 0x02: dis_sprintf("adc "); EvIb(); goto done;
          case 0x03: dis_sprintf("sbb "); EvIb(); goto done;
          case 0x04: dis_sprintf("and "); EvIb(); goto done;
          case 0x05: dis_sprintf("sub "); EvIb(); goto done;
          case 0x06: dis_sprintf("xor "); EvIb(); goto done;
          case 0x07: dis_sprintf("cmp "); EvIb(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0x84: dis_sprintf("test "); EbGb(); goto done;
      case 0x85: dis_sprintf("test "); EvGv(); goto done;
      case 0x86: dis_sprintf("xchg "); EbGb(); goto done;
      case 0x87: dis_sprintf("xchg "); EvGv(); goto done;
      case 0x88: dis_sprintf("mov "); EbGb(); goto done;
      case 0x89: dis_sprintf("mov "); EvGv(); goto done;
      case 0x8A: dis_sprintf("mov "); GbEb(); goto done;
      case 0x8B: dis_sprintf("mov "); GvEv(); goto done;
      case 0x8C: dis_sprintf("mov "); EwSw(); goto done;
      case 0x8D: dis_sprintf("lea "); GvM(); goto done;
      case 0x8E: dis_sprintf("mov "); SwEw(); goto done;
      case 0x8F: dis_sprintf("pop "); Ev(); goto done;


      case 0x90: dis_sprintf("nop"); goto done;
      case 0x91: dis_sprintf("xchg "); eCX(); dis_sprintf(", "); eAX(); goto done;
      case 0x92: dis_sprintf("xchg "); eDX(); dis_sprintf(", "); eAX(); goto done;
      case 0x93: dis_sprintf("xchg "); eBX(); dis_sprintf(", "); eAX(); goto done;
      case 0x94: dis_sprintf("xchg "); eSP(); dis_sprintf(", "); eAX(); goto done;
      case 0x95: dis_sprintf("xchg "); eBP(); dis_sprintf(", "); eAX(); goto done;
      case 0x96: dis_sprintf("xchg "); eSI(); dis_sprintf(", "); eAX(); goto done;
      case 0x97: dis_sprintf("xchg "); eDI(); dis_sprintf(", "); eAX(); goto done;
      case 0x98: dis_sprintf("cbw"); goto done;
      case 0x99: dis_sprintf("cwd"); goto done;
      case 0x9A: dis_sprintf("call "); Ap(); goto done;
      case 0x9B: dis_sprintf("wait"); goto done;
      case 0x9C: dis_sprintf("pushf"); goto done;
      case 0x9D: dis_sprintf("popf"); goto done;
      case 0x9E: dis_sprintf("sahf"); goto done;
      case 0x9F: dis_sprintf("lahf"); goto done;


      case 0xA0: dis_sprintf("mov "); ALOb(); goto done;
      case 0xA1: dis_sprintf("mov "); eAXOv(); goto done;
      case 0xA2: dis_sprintf("mov "); ObAL(); goto done;
      case 0xA3: dis_sprintf("mov "); OveAX(); goto done;
      case 0xA4: dis_sprintf("movsb "); XbYb(); goto done;

      case 0xA5: 
        if (db_32bit_opsize) {
          dis_sprintf("movsd "); XvYv();
          }
        else {
          dis_sprintf("movsw "); XvYv();
          }
        goto done;

      case 0xA6: dis_sprintf("cmpsb "); XbYb(); goto done;
      case 0xA7: dis_sprintf("cmpsw "); XvYv(); goto done;
      case 0xA8: dis_sprintf("test AL, "); Ib(); goto done;
      case 0xA9: dis_sprintf("test "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0xAA: dis_sprintf("stosb "); YbAL(); goto done;
      case 0xAB: dis_sprintf("stosw "); YveAX(); goto done;
      case 0xAC: dis_sprintf("lodsb "); ALXb(); goto done;
      case 0xAD: dis_sprintf("lodsw "); eAXXv(); goto done;
      case 0xAE: dis_sprintf("scasb "); ALXb(); goto done;
      case 0xAF: dis_sprintf("scasw "); eAXXv(); goto done;


      case 0xB0: dis_sprintf("mov AL, "); Ib(); goto done;
      case 0xB1: dis_sprintf("mov CL, "); Ib(); goto done;
      case 0xB2: dis_sprintf("mov DL, "); Ib(); goto done;
      case 0xB3: dis_sprintf("mov BL, "); Ib(); goto done;
      case 0xB4: dis_sprintf("mov AH, "); Ib(); goto done;
      case 0xB5: dis_sprintf("mov CH, "); Ib(); goto done;
      case 0xB6: dis_sprintf("mov DH, "); Ib(); goto done;
      case 0xB7: dis_sprintf("mov BH, "); Ib(); goto done;
      case 0xB8: dis_sprintf("mov "); eAX(); dis_sprintf(", "); Iv(); goto done;
      case 0xB9: dis_sprintf("mov "); eCX(); dis_sprintf(", "); Iv(); goto done;
      case 0xBA: dis_sprintf("mov "); eDX(); dis_sprintf(", "); Iv(); goto done;
      case 0xBB: dis_sprintf("mov "); eBX(); dis_sprintf(", "); Iv(); goto done;
      case 0xBC: dis_sprintf("mov "); eSP(); dis_sprintf(", "); Iv(); goto done;
      case 0xBD: dis_sprintf("mov "); eBP(); dis_sprintf(", "); Iv(); goto done;
      case 0xBE: dis_sprintf("mov "); eSI(); dis_sprintf(", "); Iv(); goto done;
      case 0xBF: dis_sprintf("mov "); eDI(); dis_sprintf(", "); Iv(); goto done;

      case 0xC0: /* Group 2 Eb,Ib */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("rol "); EbIb(); goto done;
          case 0x01: dis_sprintf("ror "); EbIb(); goto done;
          case 0x02: dis_sprintf("rcl "); EbIb(); goto done;
          case 0x03: dis_sprintf("rcr "); EbIb(); goto done;
          case 0x04: dis_sprintf("shl "); EbIb(); goto done;
          case 0x05: dis_sprintf("shr "); EbIb(); goto done;
          case 0x06: invalid_opcode(); goto done;
          case 0x07: dis_sprintf("sar "); EbIb(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0xC1: /* Group 2 Ev,Ib */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("rol "); EvIb(); goto done;
          case 0x01: dis_sprintf("ror "); EvIb(); goto done;
          case 0x02: dis_sprintf("rcl "); EvIb(); goto done;
          case 0x03: dis_sprintf("rcr "); EvIb(); goto done;
          case 0x04: dis_sprintf("shl "); EvIb(); goto done;
          case 0x05: dis_sprintf("shr "); EvIb(); goto done;
          case 0x06: invalid_opcode(); goto done;
          case 0x07: dis_sprintf("sar "); EvIb(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0xC2: dis_sprintf("ret_near "); Iw(); goto done;
      case 0xC3: dis_sprintf("ret_near"); goto done;
      case 0xC4: dis_sprintf("les "); GvMp(); goto done;
      case 0xC5: dis_sprintf("lds "); GvMp(); goto done;
      case 0xC6: dis_sprintf("mov "); EbIb(); goto done;
      case 0xC7: dis_sprintf("mov "); EvIv(); goto done;
      case 0xC8: dis_sprintf("enter "); Iw(); dis_sprintf(", "); Ib(); goto done;
      case 0xC9: dis_sprintf("leave"); goto done;
      case 0xCA: dis_sprintf("ret_far "); Iw(); goto done;
      case 0xCB: dis_sprintf("ret_far"); goto done;
      case 0xCC: dis_sprintf("int3"); goto done;
      case 0xCD: dis_sprintf("int "); Ib(); goto done;
      case 0xCE: dis_sprintf("into"); goto done;
      case 0xCF: dis_sprintf("iret"); goto done;


      case 0xD0: /* Group 2 Eb,1 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("rol "); Eb1(); goto done;
          case 0x01: dis_sprintf("ror "); Eb1(); goto done;
          case 0x02: dis_sprintf("rcl "); Eb1(); goto done;
          case 0x03: dis_sprintf("rcr "); Eb1(); goto done;
          case 0x04: dis_sprintf("shl "); Eb1(); goto done;
          case 0x05: dis_sprintf("shr "); Eb1(); goto done;
          case 0x06: invalid_opcode(); goto done;
          case 0x07: dis_sprintf("sar "); Eb1(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0xD1: /* group2 Ev,1 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("rol "); Ev1(); goto done;
          case 0x01: dis_sprintf("ror "); Ev1(); goto done;
          case 0x02: dis_sprintf("rcl "); Ev1(); goto done;
          case 0x03: dis_sprintf("rcr "); Ev1(); goto done;
          case 0x04: dis_sprintf("shl "); Ev1(); goto done;
          case 0x05: dis_sprintf("shr "); Ev1(); goto done;
          case 0x06: invalid_opcode(); goto done;
          case 0x07: dis_sprintf("sar "); Ev1(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0xD2: /* group2 Eb,CL */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("rol "); Eb(); dis_sprintf(", CL"); goto done;
          case 0x01: dis_sprintf("ror "); Eb(); dis_sprintf(", CL"); goto done;
          case 0x02: dis_sprintf("rcl "); Eb(); dis_sprintf(", CL"); goto done;
          case 0x03: dis_sprintf("rcr "); Eb(); dis_sprintf(", CL"); goto done;
          case 0x04: dis_sprintf("shl "); Eb(); dis_sprintf(", CL"); goto done;
          case 0x05: dis_sprintf("shr "); Eb(); dis_sprintf(", CL"); goto done;
          case 0x06: invalid_opcode(); goto done;
          case 0x07: dis_sprintf("sar "); Eb(); dis_sprintf(", CL"); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0xD3: /* group2 Ev,CL */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("rol "); Ev(); dis_sprintf(", CL"); goto done;
          case 0x01: dis_sprintf("ror "); Ev(); dis_sprintf(", CL"); goto done;
          case 0x02: dis_sprintf("rcl "); Ev(); dis_sprintf(", CL"); goto done;
          case 0x03: dis_sprintf("rcr "); Ev(); dis_sprintf(", CL"); goto done;
          case 0x04: dis_sprintf("shl "); Ev(); dis_sprintf(", CL"); goto done;
          case 0x05: dis_sprintf("shr "); Ev(); dis_sprintf(", CL"); goto done;
          case 0x06: invalid_opcode(); goto done;
          case 0x07: dis_sprintf("sar "); Ev(); dis_sprintf(", CL"); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0xD4: dis_sprintf("aam"); goto done;
      case 0xD5: dis_sprintf("aad"); goto done;
      case 0xD6: invalid_opcode(); goto done;
      case 0xD7: dis_sprintf("xlat"); goto done;

      case 0xD8: /* ESC0 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: dis_sprintf("fadd "); Es(); goto done;
            case 0x01: dis_sprintf("fmul "); Es(); goto done;
            case 0x02: dis_sprintf("fcom "); Es(); goto done;
            case 0x03: dis_sprintf("fcomp "); Es(); goto done;
            case 0x04: dis_sprintf("fsub "); Es(); goto done;
            case 0x05: dis_sprintf("fsubr "); Es(); goto done;
            case 0x06: dis_sprintf("fdiv "); Es(); goto done;
            case 0x07: dis_sprintf("fdivr "); Es(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: dis_sprintf("fadd "); ST_STi(); goto done;
            case 0x01: dis_sprintf("fmul "); ST_STi(); goto done;
            case 0x02: dis_sprintf("fcom "); ST_STi(); goto done;
            case 0x03: dis_sprintf("fcomp "); ST_STi(); goto done;
            case 0x04: dis_sprintf("fsub "); ST_STi(); goto done;
            case 0x05: dis_sprintf("fsubr "); ST_STi(); goto done;
            case 0x06: dis_sprintf("fdiv "); ST_STi(); goto done;
            case 0x07: dis_sprintf("fdivr "); ST_STi(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        goto done;

      case 0xD9: /* ESC1 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: dis_sprintf("fld "); Es(); goto done;
            case 0x01: invalid_opcode(); goto done;
            case 0x02: dis_sprintf("fst "); Es(); goto done;
            case 0x03: dis_sprintf("fstp "); Es(); goto done;
            case 0x04: dis_sprintf("fldenv "); Ea(); goto done;
            case 0x05: dis_sprintf("fldcw "); Ew(); goto done;
            case 0x06: dis_sprintf("fstenv "); Ea(); goto done;
            case 0x07: dis_sprintf("fstcw "); Ew(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00:
              dis_sprintf("fld "); STi(); goto done;
            case 0x01:
              dis_sprintf("fxch "); STi(); goto done;
            case 0x02:
              if (rm == 0) {
                dis_sprintf("fnop"); goto done;
	        }
              else {
                invalid_opcode(); goto done;
	        }
            case 0x03:
              dis_sprintf("fstp "); STi(); goto done;

            case 0x04:
              switch (rm) {
                case 0x00: dis_sprintf("fchs"); goto done;
                case 0x01: dis_sprintf("fabs"); goto done;
                case 0x02:
                case 0x03: invalid_opcode(); goto done;
                case 0x04: dis_sprintf("ftst"); goto done;
                case 0x05: dis_sprintf("fxam"); goto done;
                case 0x06:
                case 0x07: invalid_opcode(); goto done;
                }
            case 0x05:
              switch (rm) {
                case 0x00: dis_sprintf("fld1"); goto done;
                case 0x01: dis_sprintf("fldl2t"); goto done;
                case 0x02: dis_sprintf("fldl2e"); goto done;
                case 0x03: dis_sprintf("fldpi"); goto done;
                case 0x04: dis_sprintf("fldlg2"); goto done;
                case 0x05: dis_sprintf("fldln2"); goto done;
                case 0x06: dis_sprintf("fldz"); goto done;
                case 0x07: invalid_opcode(); goto done;
                }
            case 0x06:
              switch (rm) {
                case 0x00: dis_sprintf("f2xm1"); goto done;
                case 0x01: dis_sprintf("fyl2x"); goto done;
                case 0x02: dis_sprintf("fptan"); goto done;
                case 0x03: dis_sprintf("fpatan"); goto done;
                case 0x04: dis_sprintf("fxtract"); goto done;
                case 0x05: dis_sprintf("fprem1"); goto done;
                case 0x06: dis_sprintf("fdecstp"); goto done;
                case 0x07: dis_sprintf("fincstp"); goto done;
                }
            case 0x07:
              switch (rm) {
                case 0x00: dis_sprintf("fprem"); goto done;
                case 0x01: dis_sprintf("fyl2xp1"); goto done;
                case 0x02: dis_sprintf("fsqrt"); goto done;
                case 0x03: dis_sprintf("fsincos"); goto done;
                case 0x04: dis_sprintf("frndint"); goto done;
                case 0x05: dis_sprintf("fscale"); goto done;
                case 0x06: dis_sprintf("fsin"); goto done;
                case 0x07: dis_sprintf("fcos"); goto done;
                }
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        goto done;

      case 0xDA: /* ESC2 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: dis_sprintf("fiadd "); Ed(); goto done;
            case 0x01: dis_sprintf("fimul "); Ed(); goto done;
            case 0x02: dis_sprintf("ficom "); Ed(); goto done;
            case 0x03: dis_sprintf("ficomp "); Ed(); goto done;
            case 0x04: dis_sprintf("fisub "); Ed(); goto done;
            case 0x05: dis_sprintf("fisubr "); Ed(); goto done;
            case 0x06: dis_sprintf("fidiv "); Ed(); goto done;
            case 0x07: dis_sprintf("fidivr "); Ed(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x05:
              if (rm == 1) {
                dis_sprintf("fucompp"); goto done;
	        }
              else {
                invalid_opcode(); goto done;
                }
            default: invalid_opcode(); goto done;
            }
          }

      case 0xDB: /* ESC3 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: dis_sprintf("fild "); Ed(); goto done;
            case 0x01: invalid_opcode(); goto done;
            case 0x02: dis_sprintf("fist "); Ed(); goto done;
            case 0x03: dis_sprintf("fistp "); Ed(); goto done;
            case 0x04: invalid_opcode(); goto done;
            case 0x05: dis_sprintf("fld "); Et(); goto done;
            case 0x06: invalid_opcode(); goto done;
            case 0x07: dis_sprintf("fstp "); Et(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x04:
              switch (rm) {
                case 0x00: dis_sprintf("feni"); goto done;
                case 0x01: dis_sprintf("fdisi"); goto done;
                case 0x02: dis_sprintf("fclex"); goto done;
                case 0x03: dis_sprintf("finit"); goto done;
                case 0x04: dis_sprintf("fsetpm"); goto done;
                default: invalid_opcode(); goto done;
                }
            default: invalid_opcode(); goto done;
            }
          }

      case 0xDC: /* ESC4 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: dis_sprintf("fadd "); El(); goto done;
            case 0x01: dis_sprintf("fmul "); El(); goto done;
            case 0x02: dis_sprintf("fcom "); El(); goto done;
            case 0x03: dis_sprintf("fcomp "); El(); goto done;
            case 0x04: dis_sprintf("fsub "); El(); goto done;
            case 0x05: dis_sprintf("fsubr "); El(); goto done;
            case 0x06: dis_sprintf("fdiv "); El(); goto done;
            case 0x07: dis_sprintf("fdivr "); El(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: dis_sprintf("fadd "); STi_ST(); goto done;
            case 0x01: dis_sprintf("fmul "); STi_ST(); goto done;
            case 0x02: dis_sprintf("fcom "); STi_ST(); goto done;
            case 0x03: dis_sprintf("fcomp "); STi_ST(); goto done;
            case 0x04: dis_sprintf("fsubr "); STi_ST(); goto done;
            case 0x05: dis_sprintf("fsub "); STi_ST(); goto done;
            case 0x06: dis_sprintf("fdivr "); STi_ST(); goto done;
            case 0x07: dis_sprintf("fdiv "); STi_ST(); goto done;
            default: invalid_opcode(); goto done;
            }
          }


      case 0xDD: /* ESC5 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: dis_sprintf("fld "); El(); goto done;
            case 0x01: invalid_opcode(); goto done;
            case 0x02: dis_sprintf("fst "); El(); goto done;
            case 0x03: dis_sprintf("fstp "); El(); goto done;
            case 0x04: dis_sprintf("frstor "); Ea(); goto done;
            case 0x05: invalid_opcode(); goto done;
            case 0x06: dis_sprintf("fsave "); Ea(); goto done;
            case 0x07: dis_sprintf("fstsw "); Ew(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: dis_sprintf("ffree "); STi(); goto done;
            case 0x01: dis_sprintf("fxch "); STi(); goto done;
            case 0x02: dis_sprintf("fst "); STi(); goto done;
            case 0x03: dis_sprintf("fstp "); STi(); goto done;
            case 0x04: dis_sprintf("fucom "); STi_ST(); goto done;
            case 0x05: dis_sprintf("fucomp "); STi(); goto done;
            case 0x06: invalid_opcode(); goto done;
            case 0x07: invalid_opcode(); goto done;
            default: invalid_opcode(); goto done;
            }
          }

      case 0xDE: /* ESC6 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: dis_sprintf("fiadd "); Ew(); goto done;
            case 0x01: dis_sprintf("fimul "); Ew(); goto done;
            case 0x02: dis_sprintf("ficom "); Ew(); goto done;
            case 0x03: dis_sprintf("ficomp "); Ew(); goto done;
            case 0x04: dis_sprintf("fisub "); Ew(); goto done;
            case 0x05: dis_sprintf("fisubr "); Ew(); goto done;
            case 0x06: dis_sprintf("fidiv "); Ew(); goto done;
            case 0x07: dis_sprintf("fidivr "); Ew(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: dis_sprintf("faddp "); STi_ST(); goto done;
            case 0x01: dis_sprintf("fmulp "); STi_ST(); goto done;
            case 0x02: dis_sprintf("fcomp "); STi(); goto done;
            case 0x03:
              switch (rm) {
                case 0x01: dis_sprintf("fcompp"); goto done;
                default: invalid_opcode(); goto done;
                }
            case 0x04: dis_sprintf("fsubrp "); STi_ST(); goto done;
            case 0x05: dis_sprintf("fsubp "); STi_ST(); goto done;
            case 0x06: dis_sprintf("fdivrp "); STi_ST(); goto done;
            case 0x07: dis_sprintf("fdivp "); STi_ST(); goto done;
            default: invalid_opcode(); goto done;
            }
          }

      case 0xDF: /* ESC7 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: dis_sprintf("fild "); Ew(); goto done;
            case 0x01: invalid_opcode(); goto done;
            case 0x02: dis_sprintf("fist "); Ew(); goto done;
            case 0x03: dis_sprintf("fistp "); Ew(); goto done;
            case 0x04: dis_sprintf("fbld "); Eb(); goto done;
            case 0x05: dis_sprintf("fild "); Eq(); goto done;
            case 0x06: dis_sprintf("fbstp "); Eb(); goto done;
            case 0x07: dis_sprintf("fistp "); Eq(); goto done;
            default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: dis_sprintf("ffree "); STi(); goto done;
            case 0x01: dis_sprintf("fxch "); STi(); goto done;
	    case 0x02: dis_sprintf("fst "); STi(); goto done;
	    case 0x03: dis_sprintf("fstp "); STi(); goto done;
            case 0x04:
              switch (rm) {
                case 0x00: dis_sprintf("fstsw ax"); goto done;
                default: invalid_opcode(); goto done;
                }
            default: invalid_opcode(); goto done;
            }
          }


      case 0xE0: dis_sprintf("loopne "); Jb(); goto done;
      case 0xE1: dis_sprintf("loope "); Jb(); goto done;
      case 0xE2: dis_sprintf("loop "); Jb(); goto done;
      case 0xE3: dis_sprintf("jcxz "); Jb(); goto done;
      case 0xE4: dis_sprintf("in AL, "); Ib(); goto done;
      case 0xE5: dis_sprintf("in "); eAX(); dis_sprintf(", "); Ib(); goto done;
      case 0xE6: dis_sprintf("out "); Ib(); dis_sprintf(", AL"); goto done;
      case 0xE7: dis_sprintf("out "); Ib(); dis_sprintf(", "); eAX(); goto done;
      case 0xE8: dis_sprintf("call "); Av(); goto done;
      case 0xE9: dis_sprintf("jmp "); Jv(); goto done;
      case 0xEA: dis_sprintf("jmp "); Ap(); goto done;
      case 0xEB: dis_sprintf("jmp "); Jb(); goto done;
      case 0xEC: dis_sprintf("in AL, DX"); goto done;
      case 0xED: dis_sprintf("in "); eAX(); dis_sprintf(", DX"); goto done;
      case 0xEE: dis_sprintf("out DX, AL"); goto done;
      case 0xEF: dis_sprintf("out DX, "); eAX(); goto done;

      case 0xF0: /* LOCK */
        dis_sprintf("LOCK: ");
        break;
      case 0xF1: invalid_opcode(); goto done;
      case 0xF2: /* REPNE/REPNZ */
        db_repne_prefix = 1;
        dis_sprintf("REPNE: ");
        break;
      case 0xF3: /* REP/REPE/REPZ */
        db_rep_prefix = 1;
        dis_sprintf("REP: ");
        break;
      case 0xF4: dis_sprintf("hlt"); goto done;
      case 0xF5: dis_sprintf("cmc"); goto done;
      case 0xF6: /* Group 3 Eb */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("test "); EbIb(); goto done;
          case 0x01: invalid_opcode(); goto done;
          case 0x02: dis_sprintf("not "); Eb(); goto done;
          case 0x03: dis_sprintf("neg "); Eb(); goto done;
          case 0x04: dis_sprintf("mul AL, "); Eb(); goto done;
          case 0x05: dis_sprintf("imul AL, "); Eb(); goto done;
          case 0x06: dis_sprintf("div AL, "); Eb(); goto done;
          case 0x07: dis_sprintf("idiv AL, "); Eb(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;

      case 0xF7: /* GROUP3 Ev */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("test "); EvIv(); goto done;
          case 0x01: invalid_opcode(); goto done;
          case 0x02: dis_sprintf("not "); Ev(); goto done;
          case 0x03: dis_sprintf("neg "); Ev(); goto done;
          case 0x04: dis_sprintf("mul "); eAXEv(); goto done;
          case 0x05: dis_sprintf("imul "); eAXEv(); goto done;
          case 0x06: dis_sprintf("div "); eAXEv(); goto done;
          case 0x07: dis_sprintf("idiv "); eAXEv(); goto done;
          default: BX_PANIC(("debugger: invalid opcode\n")); goto done;
          }
        goto done;
      case 0xF8: dis_sprintf("clc"); goto done;
      case 0xF9: dis_sprintf("stc"); goto done;
      case 0xFA: dis_sprintf("cli"); goto done;
      case 0xFB: dis_sprintf("sti"); goto done;
      case 0xFC: dis_sprintf("cld"); goto done;
      case 0xFD: dis_sprintf("std"); goto done;
      case 0xFE: /* GROUP4 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("inc "); Eb(); goto done;
          case 0x01: dis_sprintf("dec "); Eb(); goto done;
          default: invalid_opcode(); goto done;
          }
        goto done;

      case 0xFF: /* GROUP 5 */
        mod_rm_byte = peek_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: dis_sprintf("inc "); Ev(); goto done;
          case 0x01: dis_sprintf("dec "); Ev(); goto done;
          case 0x02: dis_sprintf("call "); Ev(); goto done;
          case 0x03: dis_sprintf("call "); Ep(); goto done;
          case 0x04: dis_sprintf("jmp "); Ev(); goto done;
          case 0x05: dis_sprintf("jmp "); Ep(); goto done;
          case 0x06: dis_sprintf("push "); Ev(); goto done;
          default: invalid_opcode(); goto done;
          }
        goto done;

      default: /* only invalid instructions left */
        BX_PANIC(("debugger: invalid opcode\n"));
        goto done;
        break;
      } /* switch (next_byte) */
    } /* for (byte_count... */
  invalid_opcode();

done:
  return(instruction - instruction_begin);
}


  void
bx_disassemble_c::invalid_opcode(void)
{
  dis_sprintf("(INVALID OPCODE)");
}

  void
bx_disassemble_c::dis_sprintf(char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vsprintf(disbufptr, fmt, ap);
  va_end(ap);

  disbufptr += strlen(disbufptr);
}



  void
bx_disassemble_c::decode_exgx(unsigned modrm_reg_type, unsigned reg_type)
{
  Bit8u modrm, ttt;

  modrm = fetch_byte();
  decode_ex(modrm, modrm_reg_type);
  ttt = (modrm >> 3) & 0x07;

  if (reg_type != BX_NO_REG_TYPE) {
    dis_sprintf(", ");
    out_reg_name(ttt, reg_type);
    }
}



  void
bx_disassemble_c::decode_gxex(unsigned reg_type, unsigned modrm_reg_type)
{
  Bit8u modrm, ttt;

  modrm = fetch_byte();
  ttt = (modrm >> 3) & 0x07;

  if (reg_type != BX_NO_REG_TYPE) {
    out_reg_name(ttt, reg_type);
    dis_sprintf(", ");
    }

  decode_ex(modrm, modrm_reg_type);
}


  void
bx_disassemble_c::decode_ex(Bit8u modrm, unsigned modrm_reg_type)
{
  Bit32u mod_rm_addr;
  char *mod_rm_seg_reg;

  Bit8u  mod, ttt, rm;
  Bit8u  displ8;
  Bit16u displ16;

  mod = modrm >> 6;
  ttt = (modrm >> 3) & 0x07;
  rm = modrm & 0x07;

#if BX_CPU_LEVEL > 2
  if (db_32bit_addrsize) {
    Bit8u sib, ss, index, base;
    Bit32u displ32;
    Bit8u  displ8;

    /* use 32bit addressing modes.  orthogonal base & index registers,
       scaling available, etc. */
    dis_sprintf("|MOD%d|REG%d|RM%d| ", (int) mod, (int) ttt, (int) rm);


    if (mod == 3) { /* mod, reg, reg */
      out_reg_name(rm, modrm_reg_type);
      }
    else { /* mod != 3 */
      if (rm != 4) { /* rm != 100b, no s-i-b byte */
        // one byte modrm
        switch (mod) {
          case 0:
            if (seg_override)
              mod_rm_seg_reg = seg_override;
            else
              mod_rm_seg_reg = "DS";
            if (rm == 5) { /* no reg, 32-bit displacement */
              mod_rm_addr = fetch_dword();
              dis_sprintf("%s:%08x", mod_rm_seg_reg, mod_rm_addr);
              }
            else {
              dis_sprintf("%s:[%s]", mod_rm_seg_reg,
                general_32bit_reg_name[rm]);
              }
            break;
          case 1:
            if (seg_override)
              mod_rm_seg_reg = seg_override;
            else
              mod_rm_seg_reg = sreg_mod01_rm32[rm];
            /* reg, 8-bit displacement, sign extend */
            displ8 = fetch_byte();
            dis_sprintf("%s:[%s + %02x]", mod_rm_seg_reg,
              general_32bit_reg_name[rm], (unsigned) displ8);
            break;
          case 2:
            if (seg_override)
              mod_rm_seg_reg = seg_override;
            else
              mod_rm_seg_reg = sreg_mod10_rm32[rm];
            /* reg, 32-bit displacement */
            displ32 = fetch_dword();
            dis_sprintf("%s:[%s + %08x]", mod_rm_seg_reg,
              general_32bit_reg_name[rm], (unsigned) displ32);
            break;
          } /* switch (mod) */
        } /* if (rm != 4) */
      else { /* rm == 4, s-i-b byte follows */
        sib = fetch_byte();
        ss = sib >> 6;
        index = (sib >> 3) & 0x07;
        base = sib & 0x07;
        dis_sprintf("|SS%u|IND%u|BASE%u| ", (unsigned) ss,
                  (unsigned) index, (unsigned) base);

        switch (mod) {
          case 0:
            if (seg_override)
              mod_rm_seg_reg = seg_override;
            else
              mod_rm_seg_reg = sreg_mod00_base32[base];
            dis_sprintf("%s:[", mod_rm_seg_reg);
            if (base != 5)
              dis_sprintf("%s", general_32bit_reg_name[base]);
            else {
              displ32 = fetch_dword();
              dis_sprintf("%08x", (unsigned) displ32);
              }

            if (index != 4)
              dis_sprintf(" + %s<<%u", index_name32[index], ss);
            dis_sprintf("]");
            break;
          case 1:
            if (seg_override)
              mod_rm_seg_reg = seg_override;
            else
              mod_rm_seg_reg = sreg_mod01_base32[base];
            displ8 = fetch_byte();
            dis_sprintf("%s:[%s", mod_rm_seg_reg,
              general_32bit_reg_name[base]);

            if (index != 4)
              dis_sprintf(" + %s<<%u", index_name32[index], ss);
            dis_sprintf(" + %02x]", (unsigned) displ8);
            break;
          case 2:
            if (seg_override)
              mod_rm_seg_reg = seg_override;
            else
              mod_rm_seg_reg = sreg_mod10_base32[base];
            displ32 = fetch_dword();
            dis_sprintf("%s:[%s", mod_rm_seg_reg,
              general_32bit_reg_name[base]);

            if (index != 4)
              dis_sprintf(" + %s<<%u", index_name32[index], ss);
            dis_sprintf(" + %08x]", (unsigned) displ32);
            break;
          }
        } /* s-i-b byte follows */
      } /* if (mod != 3) */
    }

  else {
#endif /* BX_CPU_LEVEL > 2 */
    /* 16 bit addressing modes. */

    switch (mod) {
      case 0:
        if (seg_override)
          mod_rm_seg_reg = seg_override;
        else
          mod_rm_seg_reg = sreg_mod00_rm16[rm];
        switch (rm) {
          case 0: // DS:[BX+SI]
            dis_sprintf("%s:[BX+SI]", mod_rm_seg_reg);
            break;
          case 1: // DS:[BX+DI]
            dis_sprintf("%s:[BX+DI]", mod_rm_seg_reg);
            break;
          case 2: // SS:[BP+SI]
            dis_sprintf("%s:[BP+SI]", mod_rm_seg_reg);
            break;
          case 3: // SS:[BP+DI]
            dis_sprintf("%s:[BP+DI]", mod_rm_seg_reg);
            break;
          case 4: // DS:[SI]
            dis_sprintf("%s:[SI]", mod_rm_seg_reg);
            break;
          case 5: // DS:[DI]
            dis_sprintf("%s:[DI]", mod_rm_seg_reg);
            break;
          case 6: // DS:d16
            displ16 = fetch_word();
            dis_sprintf("%s:%04x", mod_rm_seg_reg, (unsigned) displ16);
            break;
          case 7: // DS:[BX]
            dis_sprintf("%s:[BX]", mod_rm_seg_reg);
            break;
          }
        break;

      case 1:
        displ8 = fetch_byte();
        if (seg_override)
          mod_rm_seg_reg = seg_override;
        else
          mod_rm_seg_reg = sreg_mod01_rm16[rm];
        switch (rm) {
          case 0: // DS:[BX+SI+d8]
            dis_sprintf("%s:[BX+SI+%02x]", mod_rm_seg_reg, (unsigned) displ8);
            break;
          case 1: // DS:[BX+DI+d8]
            dis_sprintf("%s:[BX+DI+%02x]", mod_rm_seg_reg, (unsigned) displ8);
            break;
          case 2: // SS:[BP+SI+d8]
            dis_sprintf("%s:[BP+SI+%02x]", mod_rm_seg_reg, (unsigned) displ8);
            break;
          case 3: // SS:[BP+DI+d8]
            dis_sprintf("%s:[BP+DI+%02x]", mod_rm_seg_reg, (unsigned) displ8);
            break;
          case 4: // DS:[SI+d8]
            dis_sprintf("%s:[SI+%02x]", mod_rm_seg_reg, (unsigned) displ8);
            break;
          case 5: // DS:[DI+d8]
            dis_sprintf("%s:[DI+%02x]", mod_rm_seg_reg, (unsigned) displ8);
            break;
          case 6: // SS:[BP+d8]
            dis_sprintf("%s:[BP+%02x]", mod_rm_seg_reg, (unsigned) displ8);
            break;
          case 7: // DS:[BX+d8]
            dis_sprintf("%s:[BX+%02x]", mod_rm_seg_reg, (unsigned) displ8);
            break;
          }
        break;

      case 2:
        displ16 = fetch_word();
        if (seg_override)
          mod_rm_seg_reg = seg_override;
        else
          mod_rm_seg_reg = sreg_mod10_rm16[rm];
        switch (rm) {
          case 0: // DS:[BX+SI+d16]
            dis_sprintf("%s:[BX+SI+%04x]", mod_rm_seg_reg, (unsigned) displ16);
            break;
          case 1: // DS:[BX+DI+d16]
            dis_sprintf("%s:[BX+DI+%04x]", mod_rm_seg_reg, (unsigned) displ16);
            break;
          case 2: // SS:[BP+SI+d16]
            dis_sprintf("%s:[BP+SI+%04x]", mod_rm_seg_reg, (unsigned) displ16);
            break;
          case 3: // SS:[BP+DI+d16]
            dis_sprintf("%s:[BP+DI+%04x]", mod_rm_seg_reg, (unsigned) displ16);
            break;
          case 4: // DS:[SI+d16]
            dis_sprintf("%s:[SI+%04x]", mod_rm_seg_reg, (unsigned) displ16);
            break;
          case 5: // DS:[DI+d16]
            dis_sprintf("%s:[DI+%04x]", mod_rm_seg_reg, (unsigned) displ16);
            break;
          case 6: // SS:[BP+d16]
            dis_sprintf("%s:[BP+%04x]", mod_rm_seg_reg, (unsigned) displ16);
            break;
          case 7: // DS:[BX+d16]
            dis_sprintf("%s:[BX+%04x]", mod_rm_seg_reg, (unsigned) displ16);
            break;
          }
        break;

      case 3: /* mod, reg, reg */
        out_reg_name(rm, modrm_reg_type);
        break;

      } /* switch (mod) ... */
#if BX_CPU_LEVEL > 2
    }
#endif
}


  void
bx_disassemble_c::out_reg_name(int reg, int reg_type)
{
  switch (reg_type) {
    case BX_SEGMENT_REG:
      dis_sprintf("%s", segment_name[reg]);
      break;
    case BX_GENERAL_8BIT_REG:
      dis_sprintf("%s", general_8bit_reg_name[reg]);
      break;
    case BX_GENERAL_16BIT_REG:
      dis_sprintf("%s", general_16bit_reg_name[reg]);
      break;
    case BX_GENERAL_32BIT_REG:
      dis_sprintf("%s", general_32bit_reg_name[reg]);
      break;
    }
}

  void
bx_disassemble_c::out_16bit_base(int base)
{
  if (base_name16[base])
    dis_sprintf("[%s]",  base_name16[base]);
}

  void
bx_disassemble_c::out_16bit_index(int index)
{
  if (index_name16[index])
    dis_sprintf("[%s]", index_name16[index]);
}
