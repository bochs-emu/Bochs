#include <stdio.h>
#include "disasm.h"

#if BX_DEBUGGER
#include "../bx_debug/debug.h"
#endif

// 32-bit general purpose register
void disassembler::REG32 (unsigned attr)
{
  if (i32bit_opsize)
    dis_sprintf("%s", general_32bit_reg_name[attr]);
  else
    dis_sprintf("%s", general_16bit_reg_name[attr]);
}

// 8-bit general purpose register
void disassembler::REG8 (unsigned attr)
{
  dis_sprintf("%s", general_8bit_reg_name[attr]);
}

// 16-bit general purpose register
void disassembler::REG16 (unsigned attr)
{
  dis_sprintf("%s", general_16bit_reg_name[attr]);
}

// segment register
void disassembler::OP_SEG (unsigned attr)
{
  dis_sprintf("%s", segment_name[attr]);
}

// memory operand
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
  {
    if (intel_mode)
      dis_sprintf  ("mm%d", rm);
    else
      dis_sprintf("%%mm%d", rm);
  }
  else
    (this->*resolve_modrm)(attr);
}

void disassembler::OP_W (unsigned attr)
{
  if (mod == 3)
  {
    if (intel_mode)
      dis_sprintf  ("xmm%d", rm);
    else
      dis_sprintf("%%xmm%d", rm);
  }
  else
    (this->*resolve_modrm)(attr);
}

void disassembler::OP_V (unsigned attr)
{
  if (intel_mode)
    dis_sprintf  ("xmm%d", nnn);
  else
    dis_sprintf("%%xmm%d", nnn);
}

void disassembler::OP_P (unsigned attr)
{
  if (intel_mode)
    dis_sprintf  ("mm%d", nnn);
  else
    dis_sprintf("%%mm%d", nnn);
}

void disassembler::OP_X (unsigned attr)
{
  const char *esi, *seg;

  if (i32bit_addrsize)
    esi = general_32bit_reg_name[eSI_REG];
  else
    esi = general_16bit_reg_name[eSI_REG];
  
  if (seg_override)
    seg = seg_override;
  else
    seg = segment_name[DS_REG];

  print_datasize(attr);

  if (intel_mode)
    dis_sprintf("%s:[%s]", seg, esi);
  else
    dis_sprintf("%s:(%s)", seg, esi);
}

void disassembler::OP_Y (unsigned attr)
{
  const char *edi;

  if (i32bit_addrsize)
    edi = general_32bit_reg_name[eDI_REG];
  else
    edi = general_16bit_reg_name[eDI_REG];
  
  print_datasize(attr);

  if (intel_mode)
    dis_sprintf("%s:[%s]", segment_name[ES_REG], edi);
  else
    dis_sprintf("%s:(%s)", segment_name[ES_REG], edi);
}

void disassembler::OP_O (unsigned attr)
{
  const char *seg;

  if (seg_override)
    seg = seg_override;
  else
    seg = segment_name[DS_REG];

  print_datasize(attr);

  if (i32bit_addrsize) {
    Bit32u imm32 = fetch_dword();
    dis_sprintf("%s:0x%x", seg, (unsigned) imm32);
  }
  else {
    Bit16u imm16 = fetch_word();
    dis_sprintf("%s:0x%x", seg, (unsigned) imm16);
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
      dis_sprintf(".+0x%x", (unsigned) (imm8+db_eip));
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
    dis_sprintf(".+0x%x", (unsigned) ((imm8+db_eip) & 0xFFFF));
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
      dis_sprintf(".+0x%x", (unsigned) (imm32+db_eip));
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
      dis_sprintf(".+0x%x", (unsigned) ((imm16+db_eip) & 0xFFFF));
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

// general purpose register or memory operand
void disassembler::Eb (unsigned attr) 
{
  if (mod == 3)
    dis_sprintf("%s", general_8bit_reg_name[rm]);
  else
    (this->*resolve_modrm)(B_SIZE);
}

void disassembler::Ew (unsigned attr) 
{
  if (mod == 3)
    dis_sprintf("%s", general_16bit_reg_name[rm]);
  else
    (this->*resolve_modrm)(W_SIZE);
}

void disassembler::Ev (unsigned attr) 
{
  if (i32bit_opsize)
    Ed(attr);
  else
    Ew(attr);
}

void disassembler::Ed (unsigned attr) 
{
  if (mod == 3)
    dis_sprintf("%s", general_32bit_reg_name[rm]);
  else
    (this->*resolve_modrm)(D_SIZE);
}

// general purpose register
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

// 32-bit general purpose register
void disassembler::Rd (unsigned attr)
{
  dis_sprintf("%s", general_32bit_reg_name[rm]);
}

// 16-bit general purpose register
void disassembler::Rw (unsigned attr)
{
  dis_sprintf("%s", general_16bit_reg_name[rm]);
}

// segment register
void disassembler::Sw (unsigned attr) 
{
  dis_sprintf("%s", segment_name[nnn]);
}

// immediate
void disassembler::I1 (unsigned) 
{ 
  if (intel_mode)
    dis_sprintf ("1");
  else
    dis_sprintf("$1");
}

void disassembler::Ib (unsigned attr) 
{
  if (intel_mode)
    dis_sprintf ("0x%x", (unsigned) fetch_byte());
  else
    dis_sprintf("$0x%x", (unsigned) fetch_byte());
}

void disassembler::Iw (unsigned attr) 
{
  if (intel_mode)
    dis_sprintf ("0x%x", (unsigned) fetch_word());
  else
    dis_sprintf("$0x%x", (unsigned) fetch_word());
}

void disassembler::Id (unsigned attr) 
{
  if (intel_mode)
    dis_sprintf ("0x%x", (unsigned) fetch_dword());
  else
    dis_sprintf("$0x%x", (unsigned) fetch_dword());
}

void disassembler::Iv (unsigned attr) 
{
  if (i32bit_opsize)
    Id(attr);
  else
    Iw(attr);
}

// sign extended immediate
void disassembler::sIb(unsigned attr) 
{
  if (i32bit_opsize)
  {
    Bit32u imm32 = (Bit8s) fetch_byte();
    if (intel_mode)
      dis_sprintf ("0x%x", (unsigned) imm32);
    else
      dis_sprintf("$0x%x", (unsigned) imm32);
  }
  else
  {
    Bit16u imm16 = (Bit8s) fetch_byte();
    if (intel_mode)
      dis_sprintf ("0x%x", (unsigned) imm16);
    else
      dis_sprintf("$0x%x", (unsigned) imm16);
  }
}

// floating point
void disassembler::ST0 (unsigned attr)
{ 
  if (intel_mode)
    dis_sprintf  ("st(0)");
  else
    dis_sprintf("%%st(0)");
}

void disassembler::STj (unsigned attr) 
{ 
  if (intel_mode)
    dis_sprintf  ("st(%d)", rm);
  else
    dis_sprintf("%%st(%d)", rm);
}

// control register
void disassembler::Cd (unsigned attr) 
{ 
  if (intel_mode)
    dis_sprintf  ("cr%d", nnn);
  else
    dis_sprintf("%%cr%d", nnn);
}

// debug register
void disassembler::Dd (unsigned attr) 
{
  if (intel_mode)
    dis_sprintf  ("db%d", nnn);
  else
    dis_sprintf("%%db%d", nnn);
}

// test registers
void disassembler::Td (unsigned attr)
{
  if (intel_mode)
    dis_sprintf  ("tr%d", nnn);
  else
    dis_sprintf("%%tr%d", nnn);
}
