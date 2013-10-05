/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013 Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "fetchdecode.h"

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

#if BX_SUPPORT_EVEX
static const char *rounding_mode[4] = {
    "round_nearest_even", "round_down", "round_up", "round_to_zero"
};
#endif

char *resolve_memref(char *disbufptr, const bxInstruction_c *i, const char *regname[])
{
  unsigned sib_base = i->sibBase(), sib_index = i->sibIndex(), sib_scale = i->sibScale();

  if (sib_index == 4) sib_index = BX_NIL_REGISTER;

  if (sib_base == BX_NIL_REGISTER)
  {
    if (sib_index == BX_NIL_REGISTER)
    {
      if (! i->as32L()) {
        if (i->displ16s() != 0) {
          disbufptr = dis_sprintf(disbufptr, "0x%04x", (Bit32u) (Bit16u) i->displ16s());
        }
      }
      else {
        if (i->displ32s() != 0) {
          disbufptr = dis_sprintf(disbufptr, "0x%08x", (Bit32u) i->displ32s());
        }
      }
      return disbufptr;
    }

    disbufptr = dis_sprintf(disbufptr, "[%s", regname[i->sibIndex()]);
    if (sib_scale)
      disbufptr = dis_sprintf(disbufptr, "*%d", 1 << i->sibScale());
  }
  else {
    disbufptr = dis_sprintf(disbufptr, "[%s", regname[i->sibBase()]);

    if (sib_index != BX_NIL_REGISTER)
    {
      disbufptr = dis_sprintf(disbufptr, "+%s", regname[i->sibIndex()]);
      if (sib_scale)
        disbufptr = dis_sprintf(disbufptr, "*%d", 1 << i->sibScale());
    }
  }

  if (! i->as32L()) {
    if (i->displ16s() != 0) {
      disbufptr = dis_sprintf(disbufptr, "%+d", (Bit32s) i->displ16s());
    }
  }
  else {
    if (i->displ32s() != 0) {
      disbufptr = dis_sprintf(disbufptr, "%+d", i->displ32s());
    }
  }

  disbufptr = dis_putc(disbufptr, ']');
  return disbufptr;
}

char *resolve_memref(char *disbufptr, const bxInstruction_c *i)
{
  // seg:[base + index*scale + disp]
  disbufptr = dis_sprintf(disbufptr, "%s:", intel_segment_name[i->seg()]);
  if (i->as64L()) {
    disbufptr = resolve_memref(disbufptr, i, intel_general_64bit_regname);
  }
  else if (i->as32L()) {
    disbufptr = resolve_memref(disbufptr, i, intel_general_32bit_regname);
  }
  else {
    disbufptr = resolve_memref(disbufptr, i, intel_general_16bit_regname);
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

  if (i->execute1 == &BX_CPU_C::BxError) {
    disbufptr = dis_sprintf(disbufptr, "(invalid)");
    return disbufptr;
  }

  const char *opname = i->getIaOpcodeName() + 6; // skip the "BX_IA_"
  unsigned n;
#if BX_SUPPORT_EVEX
  bx_bool is_vector = BX_FALSE;
#endif

  if (! strncmp(opname, "V128_", 5) || ! strncmp(opname, "V256_", 5) || ! strncmp(opname, "V512_", 5)) {
    opname += 5;
#if BX_SUPPORT_EVEX
    is_vector = BX_TRUE;
#endif
  }

  if (! strncmp(opname, "REP_", 4)) {
    opname += 4;

    // Step 1: print prefixes
    if (i->repUsedL()) {
      if (i->repUsedValue() == 2)
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
    unsigned src_type = src >> 3;
    if (! src_type && src != BX_SRC_RM) continue;
    if (srcs_used++ > 0)
      disbufptr = dis_sprintf(disbufptr, ", ");

    if (! i->modC0() && ((src & 0x7) == BX_SRC_RM)) {
      disbufptr = resolve_memref(disbufptr, i);
    }
    else {
      unsigned srcreg = i->getSrcReg(n);
      if (src_type < 0x10) {
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
          disbufptr = dis_sprintf(disbufptr, "%s", intel_general_32bit_regname[srcreg]);
          break;
#if BX_SUPPORT_X86_64
        case BX_GPR64:
          disbufptr = dis_sprintf(disbufptr, "%s", intel_general_64bit_regname[srcreg]);
          break;
#endif
        case BX_FPU_REG:
          disbufptr = dis_sprintf(disbufptr, "st(%d)", srcreg);
          break;
        case BX_MMX_REG:
          disbufptr = dis_sprintf(disbufptr, "mm%d", srcreg);
          break;
        case BX_VMM_REG:
#if BX_SUPPORT_AVX
          if (i->getVL() > BX_NO_VL) {
            disbufptr = dis_sprintf(disbufptr, "%cmm%d", 'x' + i->getVL() - 1, srcreg);
#if BX_SUPPORT_EVEX
            if (n == 0 && i->opmask()) {
              disbufptr = dis_sprintf(disbufptr, "{k%d}%s", i->opmask(),
                i->isZeroMasking() ? "{z}" : "");
            }
#endif
          }
          else
#endif
            disbufptr = dis_sprintf(disbufptr, "xmm%d", srcreg);
          break;
#if BX_SUPPORT_EVEX
        case BX_KMASK_REG:
          disbufptr = dis_sprintf(disbufptr, "k%d", srcreg);
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
          if (src_type != BX_NO_REG)
            disbufptr = dis_sprintf(disbufptr, "(unknown source type %d)", src_type);
          break;
        }
      }
      else {
        switch(src_type) {
        case BX_IMMB:
          disbufptr = dis_sprintf(disbufptr, "0x%02x", i->Ib());
          break;
        case BX_IMMW:
          disbufptr = dis_sprintf(disbufptr, "0x%04x", i->Iw());
          break;
        case BX_IMMD:
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
        case BX_IMM_BrOff16:
          {
            Bit16u target = (rip + i->ilen() + (Bit16s) i->Iw()) & 0xffff;
            disbufptr = dis_sprintf(disbufptr, ".%+d (0x%08x)", i->Iw(), (Bit32u)(cs_base + target));
          }
          break;
        case BX_IMM_BrOff32:
          {
            bx_address target = rip + i->ilen() + (Bit32s) i->Id();
            disbufptr = dis_sprintf(disbufptr, ".%+d (0x" FMT_ADDRX ")", i->Id(), cs_base + target);
          }
          break;
        case BX_RSIREF:
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
        case BX_RDIREF:
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
        case BX_USECL:
          disbufptr = dis_sprintf(disbufptr, "cl");
          break;
        case BX_USEDX:
          disbufptr = dis_sprintf(disbufptr, "dx");
          break;
        case BX_DIRECT_PTR:
          if (i->os32L())
            disbufptr = dis_sprintf(disbufptr, "0x%04x:%08x", i->Iw2(), i->Id());
          else
            disbufptr = dis_sprintf(disbufptr, "0x%04x:%04x", i->Iw2(), i->Iw());
          break;
        case BX_DIRECT_MEMREF32:
          disbufptr = dis_sprintf(disbufptr, "%s:", intel_segment_name[i->seg()]);
          if (! i->as32L())
            disbufptr = dis_sprintf(disbufptr, "0x%04x", i->Id());
          else
            disbufptr = dis_sprintf(disbufptr, "0x%08x", i->Id());
          break;
#if BX_SUPPORT_X86_64
        case BX_DIRECT_MEMREF64:
          disbufptr = dis_sprintf(disbufptr, "%s:0x" FMT_ADDRX, intel_segment_name[i->seg()], i->Iq());
          break;
#endif
        default:
          disbufptr = dis_sprintf(disbufptr, "(unknown source type %d)", src_type);
          break;
        }
      }
    }
  }

#if BX_SUPPORT_EVEX
  if (is_vector && i->getEvexb()) {
    if (! i->modC0())
      disbufptr = dis_sprintf(disbufptr, "{broadcast TBD}");
    else
      disbufptr = dis_sprintf(disbufptr, "{sae/%s}", rounding_mode[i->getRC()]);
  }
#endif

  return disbufptr;
}
