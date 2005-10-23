#include <stdio.h>
#include "disasm.h"

//////////////////
// Intel STYLE
//////////////////

#if BX_DISASM_SUPPORT_X86_64

static const char *intel_general_16bit_regname[16] = {
    "ax",  "cx",  "dx",   "bx",   "sp",   "bp",   "si",   "di",
    "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"
};

static const char *intel_general_32bit_regname[16] = {
    "eax", "ecx", "edx",  "ebx",  "esp",  "ebp",  "esi",  "edi",
    "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
};

static const char *intel_general_64bit_regname[16] = {
    "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15"
};

static const char *intel_general_8bit_regname_rex[16] = {
    "al",  "cl",  "dl",   "bl",   "spl",  "bpl",  "sil",  "dil",
    "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
};

#else

static const char *intel_general_16bit_regname[8] = {
    "ax",  "cx",  "dx",  "bx",  "sp",  "bp",  "si",  "di"
};

static const char *intel_general_32bit_regname[8] = {
    "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"
};

#endif

static const char *intel_general_8bit_regname[8] = {
    "al",  "cl",  "dl",  "bl",  "ah",  "ch",  "dh",  "bh"
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


//////////////////
// AT&T STYLE
//////////////////

#if BX_DISASM_SUPPORT_X86_64

static const char *att_general_16bit_regname[16] = {
    "%ax",  "%cx",  "%dx",   "%bx",   "%sp",   "%bp",   "%si",   "%di",
    "%r8w", "%r9w", "%r10w", "%r11w", "%r12w", "%r13w", "%r14w", "%r15w"
};

static const char *att_general_32bit_regname[16] = {
    "%eax", "%ecx", "%edx",  "%ebx",  "%esp",  "%ebp",  "%esi",  "%edi",
    "%r8d", "%r9d", "%r10d", "%r11d", "%r12d", "%r13d", "%r14d", "%r15d"
};

static const char *att_general_64bit_regname[16] = {
    "%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi",
    "%r8",  "%r9",  "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
};

static const char *att_general_8bit_regname_rex[16] = {
    "%al",  "%cl",  "%dl",   "%bl",   "%spl",  "%bpl",  "%sil",  "%dil",
    "%r8b", "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b"
};

#else

static const char *att_general_16bit_regname[8] = {
    "%ax",  "%cx",  "%dx",  "%bx",  "%sp",  "%bp",  "%si",  "%di"
};

static const char *att_general_32bit_regname[8] = {
    "%eax", "%ecx", "%edx", "%ebx", "%esp", "%ebp", "%esi", "%edi"
};

#endif

static const char *att_general_8bit_regname[8] = {
    "%al",  "%cl",  "%dl",  "%bl",  "%ah",  "%ch",  "%dh",  "%bh"
};

static const char *att_segment_name[8] = {
    "%es",  "%cs",  "%ss",  "%ds",  "%fs",  "%gs",  "%??",  "%??"
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

  general_16bit_regname = intel_general_16bit_regname;
  general_8bit_regname = intel_general_8bit_regname;
  general_32bit_regname = intel_general_32bit_regname;
#if BX_DISASM_SUPPORT_X86_64
  general_8bit_regname_rex = intel_general_8bit_regname_rex;
  general_64bit_regname = intel_general_64bit_regname;
#endif

  segment_name = intel_segment_name;
  index16 = intel_index16;

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
    dis_putc('x');
    break;

  case 'S':  // string
    if (os_32)
      dis_putc('d');
    else
      dis_putc('w');
    break;

  case 'D':
    if (os_32)
      dis_putc('d');
    break;

  default:
    disbufptr ++;
    break;
  }

  dis_putc(' ');

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

  general_16bit_regname = att_general_16bit_regname;
  general_8bit_regname = att_general_8bit_regname;
  general_32bit_regname = att_general_32bit_regname;
#if BX_DISASM_SUPPORT_X86_64
  general_8bit_regname_rex = att_general_8bit_regname_rex;
  general_64bit_regname = att_general_64bit_regname;
#endif

  segment_name = att_segment_name;
  index16 = att_index16;

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
    dis_putc('b');
    break;

  case 'W':
    dis_putc('w');
    break;

  case 'S':
  case 'V':
    if (os_32)
      dis_putc('l');
    else
      dis_putc('w');
    break;

  case 'L':
    dis_putc('l');
    break;

  case 'Q':
    dis_putc('q');
    break;

  case 'T':
    dis_putc('t');
    break;

  case 'X':
    if (entry->Op2Attr == B_SIZE)
      dis_putc('b');
    else if (entry->Op2Attr == W_SIZE)
      dis_putc('w');
    else if (entry->Op2Attr == D_SIZE)
      dis_putc('l');
    else
      printf("Internal disassembler error !\n");

    if (entry->Op1Attr == W_SIZE)
      dis_putc('w');
    else if (entry->Op1Attr == D_SIZE)
      dis_putc('l');
    else if (entry->Op1Attr == Q_SIZE)
      dis_putc('q');
    else if (entry->Op1Attr == V_SIZE)
    {
      if (os_32)
        dis_putc('l');
      else
        dis_putc('w');
    }
    else
      printf("Internal disassembler error !\n");
    break;

  case 'D':
    if (os_32)
      dis_putc('l');
    break;

  default:
    disbufptr ++;
    break;
  }

  dis_putc(' ');

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
