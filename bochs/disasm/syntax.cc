#include <stdio.h>
#include "disasm.h"

//////////////////
// Intel STYLE
//////////////////

static const char *intel_general_8bit_reg_name[8] = {
    "al",  "cl",  "dl",  "bl",  "ah",  "ch",  "dh",  "bh"
};

static const char *intel_general_16bit_reg_name[8] = {
    "ax",  "cx",  "dx",  "bx",  "sp",  "bp",  "si",  "di"
};

static const char *intel_general_32bit_reg_name[8] = {
    "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"
};

static const char *intel_segment_name[8] = {
    "es",  "cs",  "ss",  "ds",  "fs",  "gs",  "??",  "??"
};

static const char *intel_index16[8] = {
    "bx+si", 
    "bx+di", 
    "bp+si", 
    "bp+di", 
    "si", 
    "di", 
    "bp", 
    "bx"
};

static const char *intel_index_name32[8] = {
    "eax", "ecx", "edx", "ebx", "???", "ebp", "esi", "edi"
};


//////////////////
// AT&T STYLE
//////////////////

static const char *att_general_8bit_reg_name[8] = {
    "%al",  "%cl",  "%dl",  "%bl",  "%ah",  "%ch",  "%dh",  "%bh"
};

static const char *att_general_16bit_reg_name[8] = {
    "%ax",  "%cx",  "%dx",  "%bx",  "%sp",  "%bp",  "%si",  "%di"
};

static const char *att_general_32bit_reg_name[8] = {
    "%eax", "%ecx", "%edx", "%ebx", "%esp", "%ebp", "%esi", "%edi"
};

static const char *att_segment_name[8] = {
    "%es",  "%cs",  "%ss",  "%ds",  "%fs",  "%gs",  "???",  "???"
};

static const char *att_index16[8] = {
    "%bx, %si", 
    "%bx, %di", 
    "%bp, %si", 
    "%bp, %di", 
    "%si", 
    "%di", 
    "%bp", 
    "%bx"
};

static const char *att_index_name32[8] = {
    "%eax", "%ecx", "%edx", "%ebx", "???", "%ebp", "%esi", "%edi"
};

#define NULL_SEGMENT_REGISTER 7

void disassembler::initialize_modrm_segregs()
{
  sreg_mod00_rm16[0] = segment_name[DS_REG];
  sreg_mod00_rm16[1] = segment_name[DS_REG];
  sreg_mod00_rm16[2] = segment_name[SS_REG];
  sreg_mod00_rm16[3] = segment_name[SS_REG];
  sreg_mod00_rm16[4] = segment_name[DS_REG];
  sreg_mod00_rm16[5] = segment_name[DS_REG];
  sreg_mod00_rm16[6] = segment_name[DS_REG];
  sreg_mod00_rm16[7] = segment_name[DS_REG];

  sreg_mod01or10_rm16[0] = segment_name[DS_REG];
  sreg_mod01or10_rm16[1] = segment_name[DS_REG];
  sreg_mod01or10_rm16[2] = segment_name[SS_REG];
  sreg_mod01or10_rm16[3] = segment_name[SS_REG];
  sreg_mod01or10_rm16[4] = segment_name[DS_REG];
  sreg_mod01or10_rm16[5] = segment_name[DS_REG];
  sreg_mod01or10_rm16[6] = segment_name[SS_REG];
  sreg_mod01or10_rm16[7] = segment_name[DS_REG];

  sreg_mod01or10_rm32[0] = segment_name[DS_REG];
  sreg_mod01or10_rm32[1] = segment_name[DS_REG];
  sreg_mod01or10_rm32[2] = segment_name[DS_REG];
  sreg_mod01or10_rm32[3] = segment_name[DS_REG];
  sreg_mod01or10_rm32[4] = segment_name[NULL_SEGMENT_REGISTER];
  sreg_mod01or10_rm32[5] = segment_name[SS_REG];
  sreg_mod01or10_rm32[6] = segment_name[DS_REG];
  sreg_mod01or10_rm32[7] = segment_name[DS_REG];

  sreg_mod00_base32[0] = segment_name[DS_REG];
  sreg_mod00_base32[1] = segment_name[DS_REG];
  sreg_mod00_base32[2] = segment_name[DS_REG];
  sreg_mod00_base32[3] = segment_name[DS_REG];
  sreg_mod00_base32[4] = segment_name[SS_REG];
  sreg_mod00_base32[5] = segment_name[DS_REG];
  sreg_mod00_base32[6] = segment_name[DS_REG];
  sreg_mod00_base32[7] = segment_name[DS_REG];

  sreg_mod01or10_base32[0] = segment_name[DS_REG];
  sreg_mod01or10_base32[1] = segment_name[DS_REG];
  sreg_mod01or10_base32[2] = segment_name[DS_REG];
  sreg_mod01or10_base32[3] = segment_name[DS_REG];
  sreg_mod01or10_base32[4] = segment_name[SS_REG];
  sreg_mod01or10_base32[5] = segment_name[SS_REG];
  sreg_mod01or10_base32[6] = segment_name[DS_REG];
  sreg_mod01or10_base32[7] = segment_name[DS_REG];
}

//////////////////
// Intel STYLE
//////////////////

void disassembler::set_syntax_intel()
{
  intel_mode = 1;

  general_16bit_reg_name = intel_general_16bit_reg_name;
  general_8bit_reg_name = intel_general_8bit_reg_name;
  general_32bit_reg_name = intel_general_32bit_reg_name;

  segment_name = intel_segment_name;

  index16 = intel_index16;
  index_name32 = intel_index_name32;

  initialize_modrm_segregs();
}

void disassembler::print_disassembly_intel(const BxDisasmOpcodeInfo_t *entry)
{
  // print opcode
  dis_sprintf("%s", entry->Opcode);

  // patch opcode
  disbufptr --;

  switch(*disbufptr) {
  case 'B':
  case 'W':
  case 'V':
  case 'L':
  case 'Q':
  case 'T':
    break;

  case 'X':  // movsx or movzx
    dis_sprintf("x");
    break;

  case 'O':  // string
    if (i32bit_opsize)
      dis_sprintf("d");
    else
      dis_sprintf("w");
    break;

  default:
    disbufptr ++;
    break;
  }

  dis_sprintf(" ");

  if (entry->Operand1) {
    (this->*entry->Operand1)(entry->Op1Attr);
  }
  if (entry->Operand2) {
    dis_sprintf(", ");
    (this->*entry->Operand2)(entry->Op2Attr);
  }
  if (entry->Operand3) {
    dis_sprintf(", ");
    (this->*entry->Operand3)(entry->Op3Attr);
  }
}

//////////////////
// AT&T STYLE
//////////////////
 
void disassembler::set_syntax_att()
{
  intel_mode = 0;

  general_16bit_reg_name = att_general_16bit_reg_name;
  general_8bit_reg_name = att_general_8bit_reg_name;
  general_32bit_reg_name = att_general_32bit_reg_name;

  segment_name = att_segment_name;

  index16 = att_index16;
  index_name32 = att_index_name32;

  initialize_modrm_segregs();
}

void disassembler::print_disassembly_att(const BxDisasmOpcodeInfo_t *entry)
{
  // print opcode
  dis_sprintf("%s", entry->Opcode);

  // patch opcode
  disbufptr --;

  switch(*disbufptr) {
  case 'B':
    dis_sprintf("b");
    break;

  case 'W':
    dis_sprintf("w");
    break;

  case 'O':
    if (i32bit_opsize)
      dis_sprintf("d");
    else
      dis_sprintf("w");
    break;

  case 'V':
    if (i32bit_opsize)
      dis_sprintf("l");
    else
      dis_sprintf("w");
    break;

  case 'L':
    dis_sprintf("l");
    break;

  case 'Q':
    dis_sprintf("q");
    break;

  case 'T':
    dis_sprintf("t");
    break;

  case 'X':
    if (entry->Op2Attr == B_SIZE)
      dis_sprintf("b");
    else if (entry->Op2Attr == W_SIZE)
      dis_sprintf("w");
    else if (entry->Op2Attr == D_SIZE)
      dis_sprintf("l");
    else
      printf("Internal disassembler error !\n");

    if (entry->Op1Attr == W_SIZE)
      dis_sprintf("w");
    else if (entry->Op1Attr == D_SIZE)
      dis_sprintf("l");
    else if (entry->Op1Attr == Q_SIZE)
      dis_sprintf("q");
    else if (entry->Op1Attr == V_SIZE)
    {
      if (i32bit_opsize)
        dis_sprintf("l");
      else
        dis_sprintf("w");
    }
    else
      printf("Internal disassembler error !\n");
    break;

  default:
    disbufptr ++;
    break;
  }

  dis_sprintf(" ");

  if (entry->Operand3) {                                         
    (this->*entry->Operand3)(entry->Op3Attr);
    dis_sprintf(", ");
  }
  if (entry->Operand2) {
    (this->*entry->Operand2)(entry->Op2Attr);
    dis_sprintf(", ");
  }
  if (entry->Operand1) {
    (this->*entry->Operand1)(entry->Op1Attr);
  }
}
