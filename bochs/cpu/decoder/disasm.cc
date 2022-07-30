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

#if BX_DEBUGGER
#include "../../bx_debug/debug.h"
#define SYMBOLIC_JUMP(fmt)  fmt " %s"
#define GET_SYMBOL(addr) bx_dbg_disasm_symbolic_address((addr), 0)
#else
#define SYMBOLIC_JUMP(fmt)  fmt "%s"
#define GET_SYMBOL(addr) ""
#endif

extern int fetchDecode32(const Bit8u *fetchPtr, bool is_32, bxInstruction_c *i, unsigned remainingInPage);
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

static const char *general_16bit_regname[16] = {
    "ax",  "cx",  "dx",   "bx",   "sp",   "bp",   "si",   "di",
    "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"
};

static const char *general_32bit_regname[17] = {
    "eax", "ecx", "edx",  "ebx",  "esp",  "ebp",  "esi",  "edi",
    "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d", "eip"
};

static const char *general_64bit_regname[17] = {
    "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15", "rip"
};

#if BX_SUPPORT_X86_64
static const char *general_8bit_regname_rex[16] = {
    "al",  "cl",  "dl",   "bl",   "spl",  "bpl",  "sil",  "dil",
    "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
};
#endif

static const char *general_8bit_regname[8] = {
    "al",  "cl",  "dl",  "bl",  "ah",  "ch",  "dh",  "bh"
};

static const char *segment_name[8] = {
    "es",  "cs",  "ss",  "ds",  "fs",  "gs",  "??",  "??"
};

#if BX_SUPPORT_AVX
static const char *vector_reg_name[4] = {
     "xmm", "ymm", "???", "zmm"
};
#endif

#if BX_SUPPORT_EVEX
static const char *rounding_mode[4] = {
    "round_nearest_even", "round_down", "round_up", "round_to_zero"
};
#endif

#define BX_JUMP_TARGET_NOT_REQ ((bx_address)(-1))

char *resolve_sib_scale_intel(char *disbufptr, const bxInstruction_c *i, const char *regname[], unsigned src_index)
{
  unsigned sib_index = i->sibIndex(), sib_scale = i->sibScale();

#if BX_SUPPORT_AVX
  if (src_index == BX_SRC_VSIB)
    disbufptr = dis_sprintf(disbufptr, "%s%d", vector_reg_name[i->getVL() - 1], sib_index);
  else
#endif
    disbufptr = dis_sprintf(disbufptr, "%s", regname[sib_index]);

  if (sib_scale)
    disbufptr = dis_sprintf(disbufptr, "*%d", 1 << sib_scale);

  return disbufptr;
}

char *resolve_sib_scale_gas(char *disbufptr, const bxInstruction_c *i, const char *regname[], unsigned src_index)
{
  unsigned sib_index = i->sibIndex(), sib_scale = i->sibScale();

#if BX_SUPPORT_AVX
  if (src_index == BX_SRC_VSIB)
    disbufptr = dis_sprintf(disbufptr, "%%%s%d", vector_reg_name[i->getVL() - 1], sib_index);
  else
#endif
    disbufptr = dis_sprintf(disbufptr, "%%%s", regname[sib_index]);

  if (sib_scale)
    disbufptr = dis_sprintf(disbufptr, ",%d", 1 << sib_scale);

  return disbufptr;
}

char *resolve_memref_intel(char *disbufptr, const bxInstruction_c *i, const char *regname[], unsigned src_index)
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
    disbufptr = resolve_sib_scale_intel(disbufptr, i, regname, src_index);
  }
  else {
    disbufptr = dis_sprintf(disbufptr, "[%s", regname[i->sibBase()]);

    if (sib_index != BX_NIL_REGISTER) {
      disbufptr = dis_putc(disbufptr, '+');
      disbufptr = resolve_sib_scale_intel(disbufptr, i, regname, src_index);
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

char *resolve_memref_gas(char *disbufptr, const bxInstruction_c *i, const char *regname[], unsigned src_index)
{
  unsigned sib_base = i->sibBase(), sib_index = i->sibIndex();

  if (sib_index == 4 && src_index != BX_SRC_VSIB)
    sib_index = BX_NIL_REGISTER;

  if (sib_base != BX_NIL_REGISTER || sib_index != BX_NIL_REGISTER) {
    if (i->displ32s() != 0) {
      if (i->as32L()) {
        disbufptr = dis_sprintf(disbufptr, "%d", (Bit32u) i->displ32s());
      }
      else {
        disbufptr = dis_sprintf(disbufptr, "%d", (Bit32u) (Bit16u) i->displ16s());
      }
    }
  }

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

    disbufptr = dis_sprintf(disbufptr, "(,");
    disbufptr = resolve_sib_scale_gas(disbufptr, i, regname, src_index);
  }
  else {
    disbufptr = dis_sprintf(disbufptr, "(%%%s", regname[i->sibBase()]);

    if (sib_index != BX_NIL_REGISTER) {
      disbufptr = dis_putc(disbufptr, ',');
      disbufptr = resolve_sib_scale_gas(disbufptr, i, regname, src_index);
    }
  }

  disbufptr = dis_putc(disbufptr, ')');
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
        disbufptr = dis_sprintf(disbufptr, "%sword ptr ", vector_reg_name[i->getVL() - 1]);
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
    disbufptr = dis_sprintf(disbufptr, "%sword ptr ", vector_reg_name[i->getVL() - 1]);
  }
#endif

  return disbufptr;
}

// disasembly of memory reference
char *resolve_memref_intel(char *disbufptr, const bxInstruction_c *i, unsigned src_index, unsigned src_type)
{
  disbufptr = resolve_memsize(disbufptr, i, src_index, src_type);

  // seg:[base + index*scale + disp]
  disbufptr = dis_sprintf(disbufptr, "%s:", segment_name[i->seg()]);
  if (i->as64L()) {
    disbufptr = resolve_memref_intel(disbufptr, i, general_64bit_regname, src_index);
  }
  else if (i->as32L()) {
    disbufptr = resolve_memref_intel(disbufptr, i, general_32bit_regname, src_index);
  }
  else {
    disbufptr = resolve_memref_intel(disbufptr, i, general_16bit_regname, src_index);
  }
  return disbufptr;
}

char *resolve_memref_gas(char *disbufptr, const bxInstruction_c *i, unsigned src_index, unsigned src_type)
{
  // %%seg: $disp[base, index, scale)
  disbufptr = dis_sprintf(disbufptr, "%%%s:", segment_name[i->seg()]);
  if (i->as64L()) {
    disbufptr = resolve_memref_gas(disbufptr, i, general_64bit_regname, src_index);
  }
  else if (i->as32L()) {
    disbufptr = resolve_memref_gas(disbufptr, i, general_32bit_regname, src_index);
  }
  else {
    disbufptr = resolve_memref_gas(disbufptr, i, general_16bit_regname, src_index);
  }
  return disbufptr;
}

// disasembly of register reference
char *disasm_regref(char *disbufptr, const bxInstruction_c *i, unsigned src_num, unsigned src_type, BxDisasmStyle style)
{
  unsigned srcreg = i->getSrcReg(src_num);

  if (style == BX_DISASM_GAS)
    if (src_type != BX_KMASK_REG_PAIR && src_type != BX_NO_REGISTER)
      disbufptr = dis_sprintf(disbufptr, "%%");

  switch(src_type) {
  case BX_GPR8:
#if BX_SUPPORT_X86_64
    if (i->extend8bitL())
      disbufptr = dis_sprintf(disbufptr, "%s", general_8bit_regname_rex[srcreg]);
    else
#endif
      disbufptr = dis_sprintf(disbufptr, "%s", general_8bit_regname[srcreg]);
    break;

  case BX_GPR16:
    disbufptr = dis_sprintf(disbufptr, "%s", general_16bit_regname[srcreg]);
    break;

  case BX_GPR32:
  case BX_GPR32_MEM8:      // 8-bit  memory ref but 32-bit GPR
  case BX_GPR32_MEM16:     // 16-bit memory ref but 32-bit GPR
    disbufptr = dis_sprintf(disbufptr, "%s", general_32bit_regname[srcreg]);
    break;

#if BX_SUPPORT_X86_64
  case BX_GPR64:
    disbufptr = dis_sprintf(disbufptr, "%s", general_64bit_regname[srcreg]);
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
      disbufptr = dis_sprintf(disbufptr, "%s%d", vector_reg_name[i->getVL() - 1], srcreg);
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
      disbufptr = dis_sprintf(disbufptr, "{%sk%d}%s", (style == BX_DISASM_GAS) ? "%" : "",
        i->opmask(),
        i->isZeroMasking() ? "{z}" : "");
    }
    break;

  case BX_KMASK_REG_PAIR:
    disbufptr = dis_sprintf(disbufptr, "[%sk%d, %sk%d]", 
     (style == BX_DISASM_GAS) ? "%" : "",
      srcreg & ~1,
     (style == BX_DISASM_GAS) ? "%" : "",
      1 + (srcreg & ~1));
    assert(srcreg < 8);
    break;
#endif

  case BX_SEGREG:
    disbufptr = dis_sprintf(disbufptr, "%s", segment_name[srcreg]);
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

char *disasm_immediate(char *disbufptr, const bxInstruction_c *i, unsigned src_type, BxDisasmStyle style)
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

  if (style == BX_DISASM_GAS)
    if(src_type != BX_DIRECT_MEMREF_B && src_type != BX_DIRECT_MEMREF_W && src_type != BX_DIRECT_MEMREF_D && src_type != BX_DIRECT_MEMREF_Q)
      disbufptr = dis_sprintf(disbufptr, "$");

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
    if (i->os32L()) {
      Bit32u imm32 = i->Id();
      Bit16u cs_selector = i->Iw2();
      disbufptr = dis_sprintf(disbufptr, "0x%04x:%08x", cs_selector, imm32);
#if BX_DEBUGGER
      bx_address laddr = bx_dbg_get_laddr(cs_selector, imm32);
      // get the symbol
      const char *ptStrSymbol = bx_dbg_disasm_symbolic_address(laddr, 0);
      if (ptStrSymbol)
        disbufptr = dis_sprintf(disbufptr, " <%s>", ptStrSymbol);
#endif
    }
    else {
      Bit16u imm16 = i->Iw();
      Bit16u cs_selector = i->Iw2();
      disbufptr = dis_sprintf(disbufptr, "0x%04x:%04x", cs_selector, imm16);
#if BX_DEBUGGER
      bx_address laddr = bx_dbg_get_laddr(cs_selector, imm16);
      // get the symbol
      const char *ptStrSymbol = bx_dbg_disasm_symbolic_address(laddr, 0);
      if (ptStrSymbol)
        disbufptr = dis_sprintf(disbufptr, " <%s>", ptStrSymbol);
#endif
    }
    break;

  case BX_DIRECT_MEMREF_B:
  case BX_DIRECT_MEMREF_W:
  case BX_DIRECT_MEMREF_D:
  case BX_DIRECT_MEMREF_Q:
    disbufptr = dis_sprintf(disbufptr, "%s%s:", 
     (style == BX_DISASM_GAS) ? "%" : "", segment_name[i->seg()]);
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

char *disasm_branch_target(char *disbufptr, const bxInstruction_c *i, unsigned src_type, bx_address cs_base, bx_address rip, BxDisasmStyle style)
{
  bx_address target;
  Bit16s imm16;
  Bit32s imm32;
  const char *sym = "";

  switch(src_type) {
  case BX_IMMW:
  case BX_IMMBW_SE: // 8-bit signed value sign extended to 16-bit size
    imm16 = (Bit16s) i->Iw();
    target = (rip + i->ilen() + imm16) & 0xffff; // do not add CS_BASE in 16-bit
    sym = GET_SYMBOL(target);
    sym = sym ? sym : "";
//  disbufptr = dis_sprintf(disbufptr, SYMBOLIC_JUMP(".+0x%04x"), (unsigned) imm16, sym);   // hex offset
    disbufptr = dis_sprintf(disbufptr, SYMBOLIC_JUMP(".%+d"), (unsigned) imm16, sym);

    if (cs_base != BX_JUMP_TARGET_NOT_REQ) {
      disbufptr = dis_sprintf(disbufptr, " (0x%08x)", Bit32u(cs_base + target));
    }
    break;

  case BX_IMMD:
  case BX_IMMBD_SE: // 8-bit signed value sign extended to 32-bit size
    imm32 = (Bit32s) i->Id();
    target = rip + i->ilen() + (Bit32s) i->Id();
    target = (cs_base != BX_JUMP_TARGET_NOT_REQ) ? bx_address(cs_base + target) : target;
    sym = GET_SYMBOL(target);
    sym = sym ? sym : "";
//  disbufptr = dis_sprintf(disbufptr, SYMBOLIC_JUMP(".+0x" FMT_ADDRX64), (unsigned) imm32, sym);   // hex offset
    disbufptr = dis_sprintf(disbufptr, SYMBOLIC_JUMP(".%+d"), (unsigned) imm32, sym);

    if (cs_base != BX_JUMP_TARGET_NOT_REQ) {
      if (GET32H(target))
        disbufptr = dis_sprintf(disbufptr, " (0x" FMT_ADDRX ")", target);
      else
        disbufptr = dis_sprintf(disbufptr, " (0x%08x)", target);
    }

    break;

  default:
    disbufptr = dis_sprintf(disbufptr, "(unknown branch target immediate form for disasm %d)", src_type);
  }

  return disbufptr;
}

char *disasm_implicit_src(char *disbufptr, const bxInstruction_c *i, unsigned src_type, BxDisasmStyle style)
{
  if (style == BX_DISASM_INTEL) {
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
  }

  if (src_type == BX_USECL) {
    if (style == BX_DISASM_GAS) disbufptr = dis_putc(disbufptr, '%');
    disbufptr = dis_sprintf(disbufptr, "cl");
    return disbufptr;
  }

  if (src_type ==BX_USEDX) {
    if (style == BX_DISASM_GAS) disbufptr = dis_putc(disbufptr, '%');
    disbufptr = dis_sprintf(disbufptr, "dx");
    return disbufptr;
  }

  const char *regname = NULL;
  unsigned seg = BX_SEG_REG_NULL;

  switch(src_type) {
  case BX_RSIREF_B:
  case BX_RSIREF_W:
  case BX_RSIREF_D:
  case BX_RSIREF_Q:
    seg = i->seg();
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      regname = general_64bit_regname[BX_64BIT_REG_RSI];
    }
    else
#endif
    {
      if (i->as32L())
        regname = general_32bit_regname[BX_32BIT_REG_ESI];
      else
        regname = general_16bit_regname[BX_16BIT_REG_SI];
    }
    break;

  case BX_RDIREF_B:
  case BX_RDIREF_W:
  case BX_RDIREF_D:
  case BX_RDIREF_Q:
    seg = BX_SEG_REG_ES;
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      regname = general_64bit_regname[BX_64BIT_REG_RDI];
    }
    else
#endif
    {
      if (i->as32L())
        regname = general_32bit_regname[BX_32BIT_REG_EDI];
      else
        regname = general_16bit_regname[BX_16BIT_REG_DI];
    }
    break;

  case BX_MMX_RDIREF:
  case BX_VEC_RDIREF:
    seg = i->seg();
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      regname = general_64bit_regname[BX_64BIT_REG_RDI];
    }
    else
#endif
    {
      if (i->as32L())
        regname = general_32bit_regname[BX_32BIT_REG_EDI];
      else
        regname = general_16bit_regname[BX_16BIT_REG_DI];
    }
    break;

  default:
    disbufptr = dis_sprintf(disbufptr, "(unknown implicit source for disasm %d)", src_type);
  }

  if (style == BX_DISASM_INTEL) {
    disbufptr = dis_sprintf(disbufptr, "%s:[%s]", segment_name[seg], regname);
  }
  else {
    disbufptr = dis_sprintf(disbufptr, "%%%s:(%%%s)", segment_name[seg], regname);
  }

  return disbufptr;
}

char* disasm_source(char *disbufptr, unsigned n, bool srcs_used, const bxInstruction_c *i, bx_address cs_base, bx_address rip, BxDisasmStyle style)
{
  Bit16u ia_opcode = i->getIaOpcode();
  unsigned src = (unsigned) BxOpcodesTable[ia_opcode].src[n];
  unsigned src_type = BX_DISASM_SRC_TYPE(src);
  unsigned src_index = BX_DISASM_SRC_ORIGIN(src);

  if (! src_type && src_index != BX_SRC_RM && src_index != BX_SRC_VECTOR_RM) return disbufptr;

  if (srcs_used) disbufptr = dis_sprintf(disbufptr, ", ");

  if (! i->modC0() && (src_index == BX_SRC_RM || src_index == BX_SRC_VECTOR_RM || src_index == BX_SRC_VSIB)) {
    disbufptr = (style == BX_DISASM_INTEL) ? resolve_memref_intel(disbufptr, i, src_index, src_type) : resolve_memref_gas(disbufptr, i, src_index, src_type);
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
      disbufptr = disasm_immediate(disbufptr, i, src_type, style);
    }
    else if (src_index == BX_SRC_BRANCH_OFFSET) {
      // this is immediate value used as branch target
      disbufptr = disasm_branch_target(disbufptr, i, src_type, cs_base, rip, style);
    }
    else if (src_index == BX_SRC_IMPLICIT) {
      // this is implicit register or memory reference
      disbufptr = disasm_implicit_src(disbufptr, i, src_type, style);
    }
    else {
      // this is register reference
      disbufptr = disasm_regref(disbufptr, i, n, src_type, style);
    }
  }
  return disbufptr;
}

char* disasm(char *disbufptr, const bxInstruction_c *i, bx_address cs_base, bx_address rip, BxDisasmStyle style)
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
  int n;
#if BX_SUPPORT_EVEX
  bool is_vector = false;
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
  Bit16u ia_opcode = i->getIaOpcode();
  if (style == BX_DISASM_GAS) {
    disbufptr = dis_sprintf(disbufptr, "%s", get_gas_disasm_opcode_name(ia_opcode));
  }
  else {
    disbufptr = dis_sprintf(disbufptr, "%s", get_intel_disasm_opcode_name(ia_opcode));
  }
  disbufptr = dis_putc(disbufptr, ' ');

  // Step 3: print sources
  bool srcs_used = 0;
  if (style == BX_DISASM_INTEL) {
    for (n = 0; n <= 3; n++) {
      char *disbufptrtmp = disasm_source(disbufptr, n, srcs_used, i, cs_base, rip, style);
      if (disbufptrtmp != disbufptr) srcs_used=1;
      disbufptr = disbufptrtmp;
    }
  }
  else {
    for (n = 3; n >= 0; n--) {
      char *disbufptrtmp = disasm_source(disbufptr, n, srcs_used, i, cs_base, rip, style);
      if (disbufptrtmp != disbufptr) srcs_used=1;
      disbufptr = disbufptrtmp;
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

char* disasm(const Bit8u *opcode, bool is_32, bool is_64, char *disbufptr, bxInstruction_c *i, bx_address cs_base, bx_address rip, BxDisasmStyle style)
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
    ::disasm(disbufptr, i, cs_base, rip, style);

  return disbufptr;
}
