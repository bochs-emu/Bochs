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

#define BX_SEGMENT_REG       10
#define BX_GENERAL_8BIT_REG  11
#define BX_GENERAL_16BIT_REG 12
#define BX_GENERAL_32BIT_REG 13
#define BX_NO_REG_TYPE       14

#define BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm) { \
  mod    = (mod_rm_byte >> 6) & 0x03; \
  opcode = (mod_rm_byte >> 3) & 0x07; \
  rm     =  mod_rm_byte & 0x07; \
  }


class bx_disassemble_c : public logfunctions {
public:
  bx_disassemble_c(void);
  unsigned disasm(Boolean is_32, Bit8u *instr, char *disbuf);

private:
  Boolean db_32bit_opsize;
  Boolean db_32bit_addrsize;
  Boolean db_rep_prefix;
  Boolean db_repne_prefix;
  Bit8u *instruction_begin;  // keep track of where instruction starts
  Bit8u *instruction;        // for fetching of next byte of instruction

  char *seg_override;

  char *disbufptr;

  char *sreg_mod01_rm32[8];
  char *sreg_mod10_rm32[8];

  char *sreg_mod00_base32[8];
  char *sreg_mod01_base32[8];
  char *sreg_mod10_base32[8];

  char *sreg_mod00_rm16[8];
  char *sreg_mod01_rm16[8];
  char *sreg_mod10_rm16[8];

  char *segment_name[8];
  char *general_8bit_reg_name[8];
  char *general_16bit_reg_name[8];
  char *general_32bit_reg_name[8];
  char *base_name16[8];
  char *index_name16[8];
  char *index_name32[8];

  BX_CPP_INLINE Bit8u  fetch_byte(void) {
    return(*instruction++);
    };
  BX_CPP_INLINE Bit8u  peek_byte(void) {
    return(*instruction);
    };

  BX_CPP_INLINE Bit16u fetch_word(void) {
    Bit16u ret16;
    Bit8u  b1, b0;

    b0 = * (Bit8u *) instruction++;
    b1 = * (Bit8u *) instruction++;
    ret16 = (b1<<8) | b0;
    return(ret16);
    };

  BX_CPP_INLINE Bit32u fetch_dword(void) {
    Bit32u ret32;
    Bit8u  b3, b2, b1, b0;

    b0 = * (Bit8u *) instruction++;
    b1 = * (Bit8u *) instruction++;
    b2 = * (Bit8u *) instruction++;
    b3 = * (Bit8u *) instruction++;
    ret32 = (b3<<24) | (b2<<16) | (b1<<8) | b0;
    return(ret32);
    };

  void invalid_opcode(void);
  void dis_sprintf(char *fmt, ...);
  void decode_exgx(unsigned modrm_reg_type, unsigned reg_type);
  void decode_gxex(unsigned reg_type, unsigned modrm_reg_type);
  void decode_ex(Bit8u modrm, unsigned modrm_reg_type);
  void out_reg_name(int reg, int reg_type);
  void out_16bit_base(int base);
  void out_16bit_index(int index);

  void EbGb(void);
  void EvGv(void);
  void GbEb(void);
  void GvEv(void);
  void Ew(void);
  void Ms(void);
  void GvEw(void);
  void RdCd(void);
  void RdDd(void);
  void CdRd(void);
  void DdRd(void);
  void RdTd(void);
  void TdRd(void);
  void Jv(void);
  void Eb(void);
  void XBTS(void);
  void IBTS(void);
  void Mp(void);
  void EvIb(void);
  void GvEb(void);
  void GvMa(void);
  void EwRw(void);
  void Iv(void);
  void Ib(void);
  void YbDX(void);
  void YvDX(void);
  void DXXb(void);
  void DXXv(void);
  void Jb(void);
  void EbIb(void);
  void EvIv(void);
  void EwSw(void);
  void GvM(void);
  void SwEw(void);
  void Ev(void);

  void eAX(void);
  void eCX(void);
  void eDX(void);
  void eBX(void);
  void eSP(void);
  void eBP(void);
  void eSI(void);
  void eDI(void);

  void Ap(void);
  void ALOb(void);
  void eAXOv(void);
  void ObAL(void);
  void OveAX(void);
  void XbYb(void);
  void XvYv(void);
  void YbAL(void);
  void YveAX(void);
  void ALXb(void);
  void eAXXv(void);

  void eSPIv(void);
  void eBPIv(void);
  void Iw(void);
  void GvMp(void);
  void Eb1(void);
  void Ev1(void);
  void Es(void);
  void ST_STi(void);
  void Ea(void);
  void STi(void);
  void Et(void);
  void Ed(void);
  void El(void);
  void STi_ST(void);
  void Eq(void);
  void Av(void);
  void eAXEv(void);
  void Ep(void);
  };

extern bx_disassemble_c bx_disassemble;
