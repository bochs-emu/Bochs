/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2019 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#ifndef BX_STANDALONE_DECODER
#include "../cpu.h"
#endif

#include "instr.h"
#include "decoder.h"
#include "fetchdecode.h"

extern int fetchDecode32(const Bit8u *fetchPtr, bx_bool is_32, bxInstruction_c *i, unsigned remainingInPage);
#if BX_SUPPORT_X86_64
extern int fetchDecode64(const Bit8u *fetchPtr, bxInstruction_c *i, unsigned remainingInPage);
#endif
unsigned evex_displ8_compression(const bxInstruction_c *i, unsigned ia_opcode, unsigned src, unsigned type, unsigned vex_w);

// table of all Bochs opcodes
extern struct bxIAOpcodeTable BxOpcodesTable[];

#include <ctype.h>

char* dis_sprintf(char *disbufptr, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vsprintf(disbufptr, fmt, ap);
  va_end(ap);

  disbufptr += strlen(disbufptr);
  return disbufptr;
}

char* dis_putc(char *disbufptr, char symbol)
{
  *disbufptr++ = symbol;
  *disbufptr = 0;
  return disbufptr;
}

static const char *intel_general_16bit_regname[16] = {
    "ax",  "cx",  "dx",   "bx",   "sp",   "bp",   "si",   "di",
    "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"
};

static const char *intel_general_32bit_regname[17] = {
    "eax", "ecx", "edx",  "ebx",  "esp",  "ebp",  "esi",  "edi",
    "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d", "eip"
};

static const char *intel_general_64bit_regname[17] = {
    "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15", "rip"
};

#if BX_SUPPORT_X86_64
static const char *intel_general_8bit_regname_rex[16] = {
    "al",  "cl",  "dl",   "bl",   "spl",  "bpl",  "sil",  "dil",
    "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
};
#endif

static const char *intel_general_8bit_regname[8] = {
    "al",  "cl",  "dl",  "bl",  "ah",  "ch",  "dh",  "bh"
};

static const char *intel_segment_name[8] = {
    "es",  "cs",  "ss",  "ds",  "fs",  "gs",  "??",  "??"
};

#if BX_SUPPORT_AVX
static const char *intel_vector_reg_name[4] = {
     "xmm", "ymm", "???", "zmm"
};
#endif

#if BX_SUPPORT_EVEX
static const char *rounding_mode[4] = {
    "round_nearest_even", "round_down", "round_up", "round_to_zero"
};
#endif

#define BX_JUMP_TARGET_NOT_REQ ((bx_address)(-1))

char *resolve_sib_scale(char *disbufptr, const bxInstruction_c *i, const char *regname[], unsigned src_index)
{
  unsigned sib_index = i->sibIndex(), sib_scale = i->sibScale();

#if BX_SUPPORT_AVX
  if (src_index == BX_SRC_VSIB)
    disbufptr = dis_sprintf(disbufptr, "%s%d", intel_vector_reg_name[i->getVL() - 1], sib_index);
  else
#endif
    disbufptr = dis_sprintf(disbufptr, "%s", regname[sib_index]);

  if (sib_scale)
    disbufptr = dis_sprintf(disbufptr, "*%d", 1 << sib_scale);

  return disbufptr;
}

char *resolve_memref(char *disbufptr, const bxInstruction_c *i, const char *regname[], unsigned src_index)
{
  unsigned sib_base = i->sibBase(), sib_index = i->sibIndex();

  if (sib_index == 4 && src_index != BX_SRC_VSIB)
    sib_index = BX_NIL_REGISTER;

  if (sib_base == BX_NIL_REGISTER)
  {
    if (sib_index == BX_NIL_REGISTER)
    {
#if BX_SUPPORT_X86_64
      if (i->as64L()) {
        disbufptr = dis_sprintf(disbufptr, "0x" FMT_ADDRX, (Bit64u) i->displ32s());
        return disbufptr;
      }
#endif
      if (i->as32L()) {
        disbufptr = dis_sprintf(disbufptr, "0x%08x", (Bit32u) i->displ32s());
      }
      else {
        disbufptr = dis_sprintf(disbufptr, "0x%04x", (Bit32u) (Bit16u) i->displ16s());
      }
      return disbufptr;
    }

    disbufptr = dis_putc(disbufptr, '[');
    disbufptr = resolve_sib_scale(disbufptr, i, regname, src_index);
  }
  else {
    disbufptr = dis_sprintf(disbufptr, "[%s", regname[i->sibBase()]);

    if (sib_index != BX_NIL_REGISTER) {
      disbufptr = dis_putc(disbufptr, '+');
      disbufptr = resolve_sib_scale(disbufptr, i, regname, src_index);
    }
  }

  if (i->as32L()) {
    if (i->displ32s() != 0) {
      disbufptr = dis_sprintf(disbufptr, "%+d", i->displ32s());
    }
  }
  else {
    if (i->displ16s() != 0) {
      disbufptr = dis_sprintf(disbufptr, "%+d", (Bit32s) i->displ16s());
    }
  }

  disbufptr = dis_putc(disbufptr, ']');
  return disbufptr;
}

char *resolve_memsize(char *disbufptr, const bxInstruction_c *i, unsigned src_index, unsigned src_type)
{
  if (src_index == BX_SRC_VECTOR_RM) {
    unsigned memsize = evex_displ8_compression(i, i->getIaOpcode(), src_index, src_type, !!i->getVexW());
    switch(memsize) {
    case 1:
      disbufptr = dis_sprintf(disbufptr, "byte ptr ");
      break;

    case 2:
      disbufptr = dis_sprintf(disbufptr, "word ptr ");
      break;

    case 4:
      disbufptr = dis_sprintf(disbufptr, "dword ptr ");
      break;
        
    case 8:
      disbufptr = dis_sprintf(disbufptr, "qword ptr ");
      break;

    case 16:
      disbufptr = dis_sprintf(disbufptr, "xmmword ptr ");
      break;

    case 32:
      disbufptr = dis_sprintf(disbufptr, "ymmword ptr ");
      break;

    case 64:
      disbufptr = dis_sprintf(disbufptr, "zmmword ptr ");
      break;

    default:
      break;
    }
  }
  else if (src_index == BX_SRC_RM) {
    switch(src_type) {
    case BX_GPR8:
    case BX_GPR32_MEM8:      // 8-bit  memory ref but 32-bit GPR
      disbufptr = dis_sprintf(disbufptr, "byte ptr ");
      break;

    case BX_GPR16:
    case BX_GPR32_MEM16:     // 16-bit memory ref but 32-bit GPR
    case BX_SEGREG:
      disbufptr = dis_sprintf(disbufptr, "word ptr ");
      break;

    case BX_GPR32:
    case BX_MMX_HALF_REG:
      disbufptr = dis_sprintf(disbufptr, "dword ptr ");
      break;

    case BX_GPR64:
    case BX_MMX_REG:
#if BX_SUPPORT_EVEX
    case BX_KMASK_REG:
#endif
      disbufptr = dis_sprintf(disbufptr, "qword ptr ");
      break;

    case BX_FPU_REG:
      disbufptr = dis_sprintf(disbufptr, "tbyte ptr ");
      break;

    case BX_VMM_REG:
#if BX_SUPPORT_AVX
      if (i->getVL() > BX_NO_VL)
        disbufptr = dis_sprintf(disbufptr, "%sword ptr ", intel_vector_reg_name[i->getVL() - 1]);
      else
#endif
        disbufptr = dis_sprintf(disbufptr, "xmmword ptr ");
      break;

    default: 
      break;
    }
  }
#if BX_SUPPORT_AVX
  else if (src_index == BX_SRC_VSIB) {
    disbufptr = dis_sprintf(disbufptr, "%sword ptr ", intel_vector_reg_name[i->getVL() - 1]);
  }
#endif

  return disbufptr;
}

// disasembly of memory reference
char *resolve_memref(char *disbufptr, const bxInstruction_c *i, unsigned src_index, unsigned src_type)
{
  disbufptr = resolve_memsize(disbufptr, i, src_index, src_type);

  // seg:[base + index*scale + disp]
  disbufptr = dis_sprintf(disbufptr, "%s:", intel_segment_name[i->seg()]);
  if (i->as64L()) {
    disbufptr = resolve_memref(disbufptr, i, intel_general_64bit_regname, src_index);
  }
  else if (i->as32L()) {
    disbufptr = resolve_memref(disbufptr, i, intel_general_32bit_regname, src_index);
  }
  else {
    disbufptr = resolve_memref(disbufptr, i, intel_general_16bit_regname, src_index);
  }
  return disbufptr;
}

// disasembly of register reference
char *disasm_regref(char *disbufptr, const bxInstruction_c *i, unsigned src_num, unsigned src_type)
{
  unsigned srcreg = i->getSrcReg(src_num);

  switch(src_type) {
  case BX_GPR8:
#if BX_SUPPORT_X86_64
    if (i->extend8bitL())
      disbufptr = dis_sprintf(disbufptr, "%s", intel_general_8bit_regname_rex[srcreg]);
    else
#endif
      disbufptr = dis_sprintf(disbufptr, "%s", intel_general_8bit_regname[srcreg]);
    break;

  case BX_GPR16:
    disbufptr = dis_sprintf(disbufptr, "%s", intel_general_16bit_regname[srcreg]);
    break;

  case BX_GPR32:
  case BX_GPR32_MEM8:      // 8-bit  memory ref but 32-bit GPR
  case BX_GPR32_MEM16:     // 16-bit memory ref but 32-bit GPR
    disbufptr = dis_sprintf(disbufptr, "%s", intel_general_32bit_regname[srcreg]);
    break;

#if BX_SUPPORT_X86_64
  case BX_GPR64:
    disbufptr = dis_sprintf(disbufptr, "%s", intel_general_64bit_regname[srcreg]);
    break;
#endif

  case BX_FPU_REG:
    disbufptr = dis_sprintf(disbufptr, "st(%d)", srcreg & 0x7);
    break;

  case BX_MMX_REG:
  case BX_MMX_HALF_REG:
    disbufptr = dis_sprintf(disbufptr, "mm%d", srcreg & 0x7);
    break;

  case BX_VMM_REG:
#if BX_SUPPORT_AVX
    if (i->getVL() > BX_NO_VL) {
      disbufptr = dis_sprintf(disbufptr, "%s%d", intel_vector_reg_name[i->getVL() - 1], srcreg);
#if BX_SUPPORT_EVEX
      if (src_num == 0 && i->opmask()) {
        disbufptr = dis_sprintf(disbufptr, "{k%d}%s", i->opmask(),
          i->isZeroMasking() ? "{z}" : "");
      }
#endif
    }
    else
#endif
    {
      disbufptr = dis_sprintf(disbufptr, "xmm%d", srcreg);
    }
    break;

#if BX_SUPPORT_EVEX
  case BX_KMASK_REG:
    disbufptr = dis_sprintf(disbufptr, "k%d", srcreg);
    assert(srcreg < 8);
    if (src_num == 0 && i->opmask()) {
      disbufptr = dis_sprintf(disbufptr, "{k%d}%s", i->opmask(),
        i->isZeroMasking() ? "{z}" : "");
    }
    break;

  case BX_KMASK_REG_PAIR:
    disbufptr = dis_sprintf(disbufptr, "[k%d, k%d]", srcreg & ~1, 1 + (srcreg & ~1));
    assert(srcreg < 8);
    break;
#endif

  case BX_SEGREG:
    disbufptr = dis_sprintf(disbufptr, "%s", intel_segment_name[srcreg]);
    break;

  case BX_CREG:
    disbufptr = dis_sprintf(disbufptr, "cr%d", srcreg);
    break;

  case BX_DREG:
    disbufptr = dis_sprintf(disbufptr, "dr%d", srcreg);
    break;

  default:
    if (src_type != BX_NO_REGISTER)
      disbufptr = dis_sprintf(disbufptr, "(unknown source type %d)", src_type);
    break;
  }

  return disbufptr;
}

char *disasm_immediate(char *disbufptr, const bxInstruction_c *i, unsigned src_type)
{
  switch(src_type) {
  case BX_DIRECT_MEMREF_B:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_GPR8);
    break;
  case BX_DIRECT_MEMREF_W:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_GPR16);
    break;
  case BX_DIRECT_MEMREF_D:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_GPR32);
    break;
  case BX_DIRECT_MEMREF_Q:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_GPR64);
    break;
  default: break;
  };

  switch(src_type) {
  case BX_IMM1:
    disbufptr = dis_sprintf(disbufptr, "0x01");
    break;

  case BX_IMMB:
    disbufptr = dis_sprintf(disbufptr, "0x%02x", i->Ib());
    break;

  case BX_IMMW:
  case BX_IMMBW_SE: // 8-bit signed value sign extended to 16-bit size
    disbufptr = dis_sprintf(disbufptr, "0x%04x", i->Iw());
    break;

  case BX_IMMD:
  case BX_IMMBD_SE: // 8-bit signed value sign extended to 32-bit size
#if BX_SUPPORT_X86_64
    if (i->os64L())
      disbufptr = dis_sprintf(disbufptr, "0x" FMT_ADDRX64, (Bit64u) (Bit32s) i->Id());
    else
#endif
      disbufptr = dis_sprintf(disbufptr, "0x%08x", i->Id());
    break;

#if BX_SUPPORT_X86_64
  case BX_IMMQ:
    disbufptr = dis_sprintf(disbufptr, "0x" FMT_ADDRX64, i->Iq());
    break;
#endif

  case BX_IMMB2:
    disbufptr = dis_sprintf(disbufptr, "0x%02x", i->Ib2());
    break;

  case BX_DIRECT_PTR:
    if (i->os32L())
      disbufptr = dis_sprintf(disbufptr, "0x%04x:%08x", i->Iw2(), i->Id());
    else
      disbufptr = dis_sprintf(disbufptr, "0x%04x:%04x", i->Iw2(), i->Iw());
    break;

  case BX_DIRECT_MEMREF_B:
  case BX_DIRECT_MEMREF_W:
  case BX_DIRECT_MEMREF_D:
  case BX_DIRECT_MEMREF_Q:
    disbufptr = dis_sprintf(disbufptr, "%s:", intel_segment_name[i->seg()]);
#if BX_SUPPORT_X86_64
    if (i->as64L())
      disbufptr = dis_sprintf(disbufptr, "0x" FMT_ADDRX, i->Iq());
    else
#endif
    if (i->as32L())
      disbufptr = dis_sprintf(disbufptr, "0x%08x", i->Id());
    else
      disbufptr = dis_sprintf(disbufptr, "0x%04x", i->Id());
    break;

  default:
    disbufptr = dis_sprintf(disbufptr, "(unknown immediate form for disasm %d)", src_type);
  }

  return disbufptr;
}

char *disasm_branch_target(char *disbufptr, const bxInstruction_c *i, unsigned src_type, bx_address cs_base, bx_address rip)
{
  switch(src_type) {
  case BX_IMMW:
  case BX_IMMBW_SE: // 8-bit signed value sign extended to 16-bit size
    disbufptr = dis_sprintf(disbufptr, ".%+d", (Bit32s) (Bit16s) i->Iw());
    if (cs_base != BX_JUMP_TARGET_NOT_REQ) {
      Bit16u target = (rip + i->ilen() + (Bit16s) i->Iw()) & 0xffff;
      disbufptr = dis_sprintf(disbufptr, " (0x%08x)", (Bit32u)(cs_base + target));
    }
    break;

  case BX_IMMD:
  case BX_IMMBD_SE: // 8-bit signed value sign extended to 32-bit size
    disbufptr = dis_sprintf(disbufptr, ".%+d", (Bit32s) i->Id());
    if (cs_base != BX_JUMP_TARGET_NOT_REQ) {
      bx_address target = rip + i->ilen() + (Bit32s) i->Id();
      disbufptr = dis_sprintf(disbufptr, " (0x" FMT_ADDRX ")", (Bit64u) (cs_base + target));
    }
    break;

  default:
    disbufptr = dis_sprintf(disbufptr, "(unknown branch target immediate form for disasm %d)", src_type);
  }

  return disbufptr;
}

char *disasm_implicit_src(char *disbufptr, const bxInstruction_c *i, unsigned src_type)
{
  switch(src_type) {
  case BX_RSIREF_B:
  case BX_RDIREF_B:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_GPR8);
    break;
  case BX_RSIREF_W:
  case BX_RDIREF_W:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_GPR16);
    break;
  case BX_RSIREF_D:
  case BX_RDIREF_D:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_GPR32);
    break;
  case BX_RSIREF_Q:
  case BX_RDIREF_Q:
  case BX_MMX_RDIREF:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_GPR64);
    break;
  case BX_VEC_RDIREF:
    disbufptr = resolve_memsize(disbufptr, i, BX_SRC_RM, BX_VMM_REG);
    break;
  default: break;
  };

  switch(src_type) {
  case BX_RSIREF_B:
  case BX_RSIREF_W:
  case BX_RSIREF_D:
  case BX_RSIREF_Q:
    disbufptr = dis_sprintf(disbufptr, "%s:", intel_segment_name[i->seg()]);
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_64bit_regname[BX_64BIT_REG_RSI]);
    }
    else
#endif
    {
      if (i->as32L())
        disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_32bit_regname[BX_32BIT_REG_ESI]);
      else
        disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_16bit_regname[BX_16BIT_REG_SI]);
    }
    break;

  case BX_RDIREF_B:
  case BX_RDIREF_W:
  case BX_RDIREF_D:
  case BX_RDIREF_Q:
    disbufptr = dis_sprintf(disbufptr, "%s:", intel_segment_name[BX_SEG_REG_ES]);
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_64bit_regname[BX_64BIT_REG_RDI]);
    }
    else
#endif
    {
      if (i->as32L())
        disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_32bit_regname[BX_32BIT_REG_EDI]);
      else
        disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_16bit_regname[BX_16BIT_REG_DI]);
    }
    break;

  case BX_MMX_RDIREF:
  case BX_VEC_RDIREF:
    disbufptr = dis_sprintf(disbufptr, "%s:", intel_segment_name[i->seg()]);
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_64bit_regname[BX_64BIT_REG_RDI]);
    }
    else
#endif
    {
      if (i->as32L())
        disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_32bit_regname[BX_32BIT_REG_EDI]);
      else
        disbufptr = dis_sprintf(disbufptr, "[%s]", intel_general_16bit_regname[BX_16BIT_REG_DI]);
    }
    break;

  case BX_USECL:
    disbufptr = dis_sprintf(disbufptr, "cl");
    break;

  case BX_USEDX:
    disbufptr = dis_sprintf(disbufptr, "dx");
    break;

  default:
    disbufptr = dis_sprintf(disbufptr, "(unknown implicit source for disasm %d)", src_type);
  }
 
  return disbufptr;
}

char* disasm(char *disbufptr, const bxInstruction_c *i, bx_address cs_base, bx_address rip)
{
#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
  if (i->getIaOpcode() == BX_INSERTED_OPCODE) {
    disbufptr = dis_sprintf(disbufptr, "(bochs inserted internal opcode)");
    return disbufptr;
  }
#endif

  if (i->getIaOpcode() == BX_IA_ERROR) {
    disbufptr = dis_sprintf(disbufptr, "(invalid)");
    return disbufptr;
  }

#ifndef BX_STANDALONE_DECODER
  if (i->execute1 == &BX_CPU_C::BxError) {
    disbufptr = dis_sprintf(disbufptr, "(invalid)");
    return disbufptr;
  }
#endif

  const char *opname = i->getIaOpcodeNameShort(); // skip the "BX_IA_"
  unsigned n;
#if BX_SUPPORT_EVEX
  bx_bool is_vector = false;
#endif

  if (! strncmp(opname, "V128_", 5) || ! strncmp(opname, "V256_", 5) || ! strncmp(opname, "V512_", 5)) {
    opname += 5;
#if BX_SUPPORT_EVEX
    is_vector = true;
#endif
  }

  // Step 1: print prefixes
  if (i->getLock())
    disbufptr = dis_sprintf(disbufptr, "lock ");

  if (! strncmp(opname, "REP_", 4)) {
    opname += 4;

    if (i->repUsedL()) {
      if (i->lockRepUsedValue() == 2)
        disbufptr = dis_sprintf(disbufptr, "repne ");
      else
        disbufptr = dis_sprintf(disbufptr, "rep ");
    }
  }

  // Step 2: print opcode name
  unsigned opname_len = strlen(opname);
  for (n=0;n < opname_len; n++) {
    if (opname[n] == '_') break;
    disbufptr = dis_putc(disbufptr, tolower(opname[n]));
  }
  disbufptr = dis_putc(disbufptr, ' ');

  // Step 3: print sources
  Bit16u ia_opcode = i->getIaOpcode();
  unsigned srcs_used = 0;
  for (n = 0; n <= 3; n++) {
    unsigned src = (unsigned) BxOpcodesTable[ia_opcode].src[n];
    unsigned src_type = BX_DISASM_SRC_TYPE(src);
    unsigned src_index = BX_DISASM_SRC_ORIGIN(src);
    if (! src_type && src_index != BX_SRC_RM && src_index != BX_SRC_VECTOR_RM) continue;
    if (srcs_used++ > 0)
      disbufptr = dis_sprintf(disbufptr, ", ");

    if (! i->modC0() && (src_index == BX_SRC_RM || src_index == BX_SRC_VECTOR_RM || src_index == BX_SRC_VSIB)) {
      disbufptr = resolve_memref(disbufptr, i, src_index, src_type);
#if BX_SUPPORT_EVEX
      if (n == 0 && (src_index == BX_SRC_VECTOR_RM || src_index == BX_SRC_VSIB || src_type == BX_VMM_REG) && i->opmask()) {
        disbufptr = dis_sprintf(disbufptr, "{k%d}", i->opmask());
      }
#endif
    }
    else {
      if (src_index == BX_SRC_VECTOR_RM) src_type = BX_VMM_REG;

      if (src_index == BX_SRC_IMM) {
        // this is immediate value
        disbufptr = disasm_immediate(disbufptr, i, src_type);
      }
      else if (src_index == BX_SRC_BRANCH_OFFSET) {
        // this is immediate value used as branch target
        disbufptr = disasm_branch_target(disbufptr, i, src_type, cs_base, rip);
      }
      else if (src_index == BX_SRC_IMPLICIT) {
        // this is implicit register or memory reference
        disbufptr = disasm_implicit_src(disbufptr, i, src_type);
      }
      else {
        // this is register reference
        disbufptr = disasm_regref(disbufptr, i, n, src_type);
      }
    }
  }

#if BX_SUPPORT_EVEX
  if (is_vector && i->getEvexb()) {
    if (! i->modC0())
      disbufptr = dis_sprintf(disbufptr, " {broadcast}");
    else
      disbufptr = dis_sprintf(disbufptr, " {sae/%s}", rounding_mode[i->getRC()]);
  }
#endif

  return disbufptr;
}

char* disasm(const Bit8u *opcode, bool is_32, bool is_64, char *disbufptr, bxInstruction_c *i, bx_address cs_base, bx_address rip)
{
  int ret;

#if BX_SUPPORT_X86_64
  if (is_64)
    ret = fetchDecode64(opcode, i, 16);
  else
#endif
    ret = fetchDecode32(opcode, is_32, i, 16);

  if (ret < 0)
    sprintf(disbufptr, "decode failed");
  else
    ::disasm(disbufptr, i, cs_base, rip);

  return disbufptr;
}
