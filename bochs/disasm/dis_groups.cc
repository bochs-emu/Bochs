#include <stdio.h>
#include <assert.h>
#include "disasm.h"
#include "../bx_debug/debug.h"

//////////////////
// Intel STYLE
//////////////////

static const char *general_8bit_reg_name[8] = {
    "al",  "cl",  "dl",  "bl",  "ah",  "ch",  "dh",  "bh"
};

const char *general_16bit_reg_name[8] = {
    "ax",  "cx",  "dx",  "bx",  "sp",  "bp",  "si",  "di"
};

const char *general_32bit_reg_name[8] = {
    "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"
};

static const char *segment_name[8] = {
    "es",  "cs",  "ss",  "ds",  "fs",  "gs",  "??",  "??"
};

static const char *mmx_reg_name[8] = {
    "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"
};

static const char *xmm_reg_name[8] = 
{
    "xmm0", 
    "xmm1", 
    "xmm2", 
    "xmm3", 
    "xmm4", 
    "xmm5", 
    "xmm6", 
    "xmm7"
};

void disassembler::reg32 (unsigned attr)
{
  assert(attr < 8);

  if (i32bit_opsize)
    dis_sprintf("%s", general_32bit_reg_name[attr]);
  else
    dis_sprintf("%s", general_16bit_reg_name[attr]);
}

void disassembler::reg16 (unsigned attr)
{
  assert(attr < 8);
  dis_sprintf("%s", general_16bit_reg_name[attr]);
}

void disassembler::reg8 (unsigned attr)
{
  assert(attr < 8);
  dis_sprintf("%s", general_8bit_reg_name[attr]);
}

void disassembler::OP_SEG (unsigned attr)
{
  assert(attr < 8);
  dis_sprintf("%s", segment_name[attr]);
}

void disassembler::OP_MEM (unsigned attr)
{
 if(mod == 3)
    dis_sprintf("(bad)");
 else
    (this->*resolve_modrm)(attr);
}

void disassembler::OP_Q (unsigned attr)
{
  if (mod == 3)
    dis_sprintf("%s", mmx_reg_name[rm]);
  else
    (this->*resolve_modrm)(attr);
}

void disassembler::OP_W (unsigned attr)
{
  if (mod == 3)
    dis_sprintf("%s", xmm_reg_name[rm]);
  else
    (this->*resolve_modrm)(attr);
}

void disassembler::OP_V (unsigned attr)
{
  dis_sprintf("%s", xmm_reg_name[nnn]);
}

void disassembler::OP_P (unsigned attr)
{
  dis_sprintf("%s", mmx_reg_name[nnn]);
}

void disassembler::OP_X (unsigned attr)
{
  char *esi;

  if (i32bit_addrsize)
    esi = "esi";
  else
    esi = "si";
  
  if (attr & 0x80)
    dis_sprintf("es:");

  print_datasize(attr & 0x7F);

  dis_sprintf("[%s]", esi);
}

void disassembler::OP_Y (unsigned attr)
{
  char *edi;

  if (i32bit_addrsize)
    edi = "edi";
  else
    edi = "di";
  
  if (attr & 0x80)
    dis_sprintf("es:");

  print_datasize(attr & 0x7F);

  dis_sprintf("[%s]", edi);
}

void disassembler::Ob (unsigned attr)
{
  const char *seg;

  if (seg_override)
    seg = seg_override;
  else
    seg = "ds";

  if (i32bit_addrsize) {
    Bit32u imm32 = fetch_dword();
    dis_sprintf("byte ptr [%s:0x%x]", seg, (unsigned) imm32);
    }
  else {
    Bit16u imm16 = fetch_word();
    dis_sprintf("byte ptr [%s:0x%x]", seg, (unsigned) imm16);
    }
}

void disassembler::Ov (unsigned attr)
{
  const char *seg;

  if (seg_override)
    seg = seg_override;
  else
    seg = "ds";

  if (i32bit_addrsize) {
    Bit32u imm32 = fetch_dword();
    dis_sprintf("[%s:0x%x]", seg, (unsigned) imm32);
    }
  else {
    Bit16u imm16 = fetch_word();
    dis_sprintf("[%s:0x%x]", seg, (unsigned) imm16);
    }
}

void disassembler::Jb (unsigned attr)
{
  Bit8s imm8; /* JMP rel8 is signed */
  imm8 = (Bit8s) fetch_byte();
  if (i32bit_opsize) {
#if BX_DEBUGGER
    char *Sym=bx_dbg_disasm_symbolic_address((Bit32u)(imm8+db_eip), db_base);
    if(Sym) {
      dis_sprintf("%s", Sym);
    }
    else // Symbol not found
#endif
    dis_sprintf("0x%x", (unsigned) (imm8+db_eip));
    }
  else
  {
#if BX_DEBUGGER
    char *Sym=bx_dbg_disasm_symbolic_address((Bit32u)((imm8+db_eip) & 0xFFFF), db_base);
    if(Sym) {
      dis_sprintf("%s", Sym);
    }
    else // Symbol not found
 #endif
    dis_sprintf("0x%x", (unsigned) ((imm8+db_eip) & 0xFFFF));
  }
}

void disassembler::Jv (unsigned attr)
{
  if (i32bit_opsize) {
    Bit32s imm32; /* JMP rel32 is signed */
    imm32 = (Bit32s) fetch_dword();
#if BX_DEBUGGER
    char *Sym=bx_dbg_disasm_symbolic_address((Bit32u)(imm32+db_eip), db_base);
    if(Sym) {
      dis_sprintf("%s", Sym);
    }
    else // Symbol not found
#endif
    dis_sprintf("0x%x", (unsigned) (imm32+db_eip));
    }
  else
    {
    Bit16s imm16; /* JMP rel16 is signed */
    imm16 = (Bit16s) fetch_word();
#if BX_DEBUGGER
    char *Sym=bx_dbg_disasm_symbolic_address((Bit32u)((imm16+db_eip) & 0xFFFF), db_base);
    if(Sym) {
      dis_sprintf("%s", Sym);
    }
    else // Symbol not found
#endif
    dis_sprintf("0x%x", (unsigned) ((imm16+db_eip) & 0xFFFF));
    }
}

void disassembler::Ap (unsigned attr)
{
  if (i32bit_opsize)
  {
    Bit32u imm32 = fetch_dword();
    Bit16u cs_selector = fetch_word();
    dis_sprintf("%04x:%08x", (unsigned) cs_selector, (unsigned) imm32);
  }
  else
  {
    Bit16u imm16 = fetch_word();
    Bit16u cs_selector = fetch_word();
    dis_sprintf("%04x:%04x", (unsigned) cs_selector, (unsigned) imm16);
  }
}

void disassembler::Eb (unsigned attr) 
{
  if (mod == 3)
    dis_sprintf("%s", general_8bit_reg_name[rm]);
  else
    (this->*resolve_modrm)(B_MODE);
}

void disassembler::Ew (unsigned attr) 
{
  if (mod == 3)
    dis_sprintf("%s", general_16bit_reg_name[rm]);
  else
    (this->*resolve_modrm)(W_MODE);
}

void disassembler::Ev (unsigned attr) 
{
  if (mod == 3)
  {
    if (i32bit_opsize)
      dis_sprintf("%s", general_32bit_reg_name[rm]);
    else
      dis_sprintf("%s", general_16bit_reg_name[rm]);
  }
  else
    (this->*resolve_modrm)(V_MODE);
}

void disassembler::Ed (unsigned attr) 
{
  if (mod == 3)
    dis_sprintf("%s", general_32bit_reg_name[rm]);
  else
    (this->*resolve_modrm)(D_MODE);
}

void disassembler::Ep (unsigned attr) {dis_sprintf("*** Ep unfinished ***");}
void disassembler::Ea (unsigned attr) {dis_sprintf("*** Ea unfinished ***");}

void disassembler::Gb (unsigned attr) 
{
  dis_sprintf("%s", general_8bit_reg_name[nnn]);
}

void disassembler::Gv (unsigned attr) 
{
  if (i32bit_opsize)
    dis_sprintf("%s", general_32bit_reg_name[nnn]);
  else
    dis_sprintf("%s", general_16bit_reg_name[nnn]);
}

void disassembler::Gd (unsigned attr) 
{
  dis_sprintf("%s", general_32bit_reg_name[nnn]);
}

void disassembler::Rd (unsigned attr)
{
    dis_sprintf("%s", general_32bit_reg_name[rm]);
}

void disassembler::Rw (unsigned attr)
{
    dis_sprintf("%s", general_16bit_reg_name[rm]);
}

void disassembler::Sw (unsigned attr) 
{
  dis_sprintf("%s", segment_name[nnn]);
}

void disassembler::Ib (unsigned attr) 
{
  dis_sprintf("0x%x", (unsigned) fetch_byte());
}

void disassembler::Iw (unsigned attr) 
{
  dis_sprintf("0x%x", (unsigned) fetch_word());
}

void disassembler::Id (unsigned attr) 
{
  dis_sprintf("0x%x", (unsigned) fetch_dword());
}

void disassembler::Iv (unsigned attr) 
{
  if (i32bit_opsize)
    Id(attr);
  else
    Iw(attr);
}

void disassembler::sIb(unsigned attr) 
{
  if (i32bit_opsize)
  {
    Bit32u imm32 =  (Bit8s) fetch_byte();
    dis_sprintf("0x%x", imm32);
  }
  else
  {
    Bit32u imm16 =  (Bit8s) fetch_byte();
    dis_sprintf("0x%x", imm16);
  }
}

// floating point
void disassembler::STj (unsigned attr) {dis_sprintf("st(%d)", rm);}
