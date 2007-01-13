/////////////////////////////////////////////////////////////////////////
// $Id: resolve.cc,v 1.14 2007-01-13 10:43:31 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <assert.h>
#include "disasm.h"

void disassembler::decode_modrm(x86_insn *insn)
{
  insn->modrm = fetch_byte();
  BX_DECODE_MODRM(insn->modrm, insn->mod, insn->nnn, insn->rm);
  // MOVs with CRx and DRx always use register ops and ignore the mod field.
  if ((insn->b1 & ~3) == 0x120) insn->mod = 3;
  insn->nnn |= insn->rex_r;

  if (insn->mod == 3) {
    /* mod, reg, reg */
    insn->rm |= insn->rex_b;
    return;
  }

  if (insn->as_64)
  {
      if (insn->rm != 4) { /* rm != 100b, no s-i-b byte */
        insn->rm |= insn->rex_b;
        // one byte modrm
        switch (insn->mod) {
          case 0:
            resolve_modrm = &disassembler::resolve64_mod0;
            if ((insn->rm & 7) == 5) /* no reg, 32-bit displacement */
              insn->displacement.displ32 = fetch_dword();
            break;
          case 1:
            /* reg, 8-bit displacement, sign extend */
            resolve_modrm = &disassembler::resolve64_mod1or2;
            insn->displacement.displ32 = (Bit8s) fetch_byte();
            break;
          case 2:
            /* reg, 32-bit displacement */
            resolve_modrm = &disassembler::resolve64_mod1or2;
            insn->displacement.displ32 = fetch_dword();
            break;
        } /* switch (mod) */
      } /* if (rm != 4) */
      else { /* rm == 4, s-i-b byte follows */
        insn->sib = fetch_byte();
        BX_DECODE_SIB(insn->sib, insn->scale, insn->index, insn->base);
        insn->base  |= insn->rex_b;
        insn->index |= insn->rex_x;

        switch (insn->mod) {
          case 0:
            resolve_modrm = &disassembler::resolve64_mod0_rm4;
            if ((insn->base & 7) == 5)
              insn->displacement.displ32 = fetch_dword();
            break;
          case 1:
            resolve_modrm = &disassembler::resolve64_mod1or2_rm4;
            insn->displacement.displ32 = (Bit8s) fetch_byte();
            break;
          case 2:
            resolve_modrm = &disassembler::resolve64_mod1or2_rm4;
            insn->displacement.displ32 = fetch_dword();
            break;
        }
      } /* s-i-b byte follows */
  }
  else
  {
    if (insn->as_32)
    {
      if (insn->rm != 4) { /* rm != 100b, no s-i-b byte */
        insn->rm |= insn->rex_b;
        // one byte modrm
        switch (insn->mod) {
          case 0:
            resolve_modrm = &disassembler::resolve32_mod0;
            if ((insn->rm & 7) == 5) /* no reg, 32-bit displacement */
              insn->displacement.displ32 = fetch_dword();
            break;
          case 1:
            /* reg, 8-bit displacement, sign extend */
            resolve_modrm = &disassembler::resolve32_mod1or2;
            insn->displacement.displ32 = (Bit8s) fetch_byte();
            break;
          case 2:
            /* reg, 32-bit displacement */
            resolve_modrm = &disassembler::resolve32_mod1or2;
            insn->displacement.displ32 = fetch_dword();
            break;
        } /* switch (mod) */
      } /* if (rm != 4) */
      else { /* rm == 4, s-i-b byte follows */
        insn->sib = fetch_byte();
        BX_DECODE_SIB(insn->sib, insn->scale, insn->index, insn->base);
        insn->base  |= insn->rex_b;
        insn->index |= insn->rex_x;

        switch (insn->mod) {
          case 0:
            resolve_modrm = &disassembler::resolve32_mod0_rm4;
            if ((insn->base & 7) == 5)
              insn->displacement.displ32 = fetch_dword();
            break;
          case 1:
            resolve_modrm = &disassembler::resolve32_mod1or2_rm4;
            insn->displacement.displ32 = (Bit8s) fetch_byte();
            break;
          case 2:
            resolve_modrm = &disassembler::resolve32_mod1or2_rm4;
            insn->displacement.displ32 = fetch_dword();
            break;
        }
      } /* s-i-b byte follows */
    }
    else {
      assert(insn->rex_b == 0);
      assert(insn->rex_x == 0);
      assert(insn->rex_r == 0);
      /* 16 bit addressing modes. */
      switch (insn->mod) {
        case 0:
          resolve_modrm = &disassembler::resolve16_mod0;
          if(insn->rm == 6)
            insn->displacement.displ16 = fetch_word();
          break;
        case 1:
          /* reg, 8-bit displacement, sign extend */
          resolve_modrm = &disassembler::resolve16_mod1or2;
          insn->displacement.displ16 = (Bit8s) fetch_byte();
          break;
        case 2:
          resolve_modrm = &disassembler::resolve16_mod1or2;
          insn->displacement.displ16 = fetch_word();
          break;
      } /* switch (mod) ... */
    }
  }
}

void disassembler::resolve16_mod0(const x86_insn *insn, unsigned mode)
{
  const char *seg;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = sreg_mod00_rm16[insn->rm];

  if(insn->rm == 6)
    print_memory_access16(mode, seg, NULL, insn->displacement.displ16);
  else
    print_memory_access16(mode, seg, index16[insn->rm], 0);
}

void disassembler::resolve16_mod1or2(const x86_insn *insn, unsigned mode)
{
  const char *seg;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = sreg_mod01or10_rm16[insn->rm];

  print_memory_access16(mode, seg, index16[insn->rm], insn->displacement.displ16);
}

void disassembler::resolve32_mod0(const x86_insn *insn, unsigned mode)
{
  const char *seg;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = segment_name[DS_REG];

  if ((insn->rm & 7) == 5) /* no reg, 32-bit displacement */
    print_memory_access(mode, seg, NULL, NULL, 0, insn->displacement.displ32);
  else
    print_memory_access(mode, seg, general_32bit_regname[insn->rm], NULL, 0, 0);
}

void disassembler::resolve32_mod1or2(const x86_insn *insn, unsigned mode)
{
  const char *seg;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = sreg_mod01or10_rm32[insn->rm];

  print_memory_access(mode, seg,
      general_32bit_regname[insn->rm], NULL, 0, insn->displacement.displ32);
}

void disassembler::resolve32_mod0_rm4(const x86_insn *insn, unsigned mode)
{
  const char *seg, *base = NULL, *index = NULL;
  Bit32u disp32 = 0;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = sreg_mod00_base32[insn->base];

  if ((insn->base & 7) != 5)
    base = general_32bit_regname[insn->base];
  else
    disp32 = insn->displacement.displ32;
  
  if (insn->index != 4)
    index = general_32bit_regname[insn->index];
    
  print_memory_access(mode, seg, base, index, insn->scale, disp32);
}

void disassembler::resolve32_mod1or2_rm4(const x86_insn *insn, unsigned mode)
{
  const char *seg, *index = NULL;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = sreg_mod01or10_base32[insn->base];

  if (insn->index != 4)
    index = general_32bit_regname[insn->index];

  print_memory_access(mode, seg,
      general_32bit_regname[insn->base], index, insn->scale, insn->displacement.displ32);
}

void disassembler::resolve64_mod0(const x86_insn *insn, unsigned mode)
{
  const char *seg, *rip_regname;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = segment_name[DS_REG];

  if (intel_mode) rip_regname = "rip";
  else rip_regname = "%rip";

  if ((insn->rm & 7) == 5) /* no reg, 32-bit displacement */
    print_memory_access(mode, seg, rip_regname, NULL, 0, (Bit32s) insn->displacement.displ32, 1);
  else
    print_memory_access(mode, seg, general_64bit_regname[insn->rm], NULL, 0, 0);
}

void disassembler::resolve64_mod1or2(const x86_insn *insn, unsigned mode)
{
  const char *seg;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = sreg_mod01or10_rm32[insn->rm];

  print_memory_access(mode, seg, 
      general_64bit_regname[insn->rm], NULL, 0, (Bit32s) insn->displacement.displ32, 1);
}

void disassembler::resolve64_mod0_rm4(const x86_insn *insn, unsigned mode)
{
  const char *seg, *base = NULL, *index = NULL;
  Bit32s disp32 = 0;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = sreg_mod00_base32[insn->base];

  if ((insn->base & 7) != 5)
    base = general_64bit_regname[insn->base];
  else
    disp32 = (Bit32s) insn->displacement.displ32;
  
  if (insn->index != 4)
    index = general_64bit_regname[insn->index];
    
  print_memory_access(mode, seg, base, index, insn->scale, disp32, 1);
}

void disassembler::resolve64_mod1or2_rm4(const x86_insn *insn, unsigned mode)
{
  const char *seg, *index = NULL;

  if (insn->is_seg_override())
    seg = segment_name[insn->seg_override];
  else
    seg = sreg_mod01or10_base32[insn->base];

  if (insn->index != 4)
    index = general_64bit_regname[insn->index];

  print_memory_access(mode, seg,
      general_64bit_regname[insn->base], index, insn->scale, (Bit32s) insn->displacement.displ32, 1);
}

void disassembler::print_datasize(unsigned size)
{
  if (!intel_mode) return;

  switch(size)
  {
    case B_SIZE:
      dis_sprintf("byte ptr ");
      break;
    case W_SIZE:
      dis_sprintf("word ptr ");
      break;
    case D_SIZE:
      dis_sprintf("dword ptr ");
      break;
    case Q_SIZE:
      dis_sprintf("qword ptr ");
      break;
    case O_SIZE:
      dis_sprintf("dqword ptr ");
      break;
    case T_SIZE:
      dis_sprintf("tbyte ptr ");
      break;
    case P_SIZE:
      break;
    case X_SIZE:
      break;
  };
}

void disassembler::print_memory_access16(int datasize, 
                const char *seg, const char *index, Bit16u disp)
{
  print_datasize(datasize);

  if (intel_mode)
  {
    if (index == NULL)
    {
      dis_sprintf("%s:0x%x", seg, (unsigned) disp);
    }
    else
    {
      if (disp != 0)
        dis_sprintf("%s:[%s+0x%x]", seg, index, (unsigned) disp);
      else
        dis_sprintf("%s:[%s]", seg, index);
    }
  }
  else
  {
    if (index == NULL)
    {
      dis_sprintf("%s:0x%x", seg, (unsigned) disp);
    }
    else
    {
      if (disp != 0)
        dis_sprintf("%s:0x%x(%s,1)", seg, (unsigned) disp, index);
      else
        dis_sprintf("%s:(%s,1)", seg, index);
    }
  }
}

void disassembler::print_memory_access(int datasize, 
        const char *seg, const char *base, const char *index, int scale, Bit32s disp, bx_bool disp64)
{
  print_datasize(datasize);

  scale = 1 << scale;
  
  if (intel_mode)
  {
    if (base == NULL)
    {
      if (index == NULL)
      {
        dis_sprintf("%s:0x%x", seg, (unsigned) disp);
      }
      else
      {
        if (scale != 1)
        {
          if (disp != 0) {
            if (disp64) {
              dis_sprintf("%s:[%s*%d+0x%08x%08x]", seg, index, scale,
                 GET32H(disp), GET32L(disp));
            }
            else {
              dis_sprintf("%s:[%s*%d+0x%x]", seg, index, scale, (unsigned) disp);
            }
          }
          else
            dis_sprintf("%s:[%s*%d]", seg, index, scale);
        }
        else
        {
          if (disp != 0) {
            if (disp64) {
              dis_sprintf("%s:[%s+0x%08x%08x]", seg, index, GET32H(disp), GET32L(disp));
            }
            else {
              dis_sprintf("%s:[%s+0x%x]", seg, index, (unsigned) disp);
            }
          }
          else {
            dis_sprintf("%s:[%s]", seg, index);
          }
        }
      }
    }
    else
    {
      if (index == NULL)
      {
        if (disp != 0) {
          if (disp64) {
            dis_sprintf("%s:[%s+0x%08x%08x]", seg, base, GET32H(disp), GET32L(disp));
          }
          else {
            dis_sprintf("%s:[%s+0x%x]", seg, base, (unsigned) disp);
          }
        }
        else {
          dis_sprintf("%s:[%s]", seg, base);
        }
      }
      else
      {
        if (scale != 1)
        {
          if (disp != 0) {
            if (disp64) {
              dis_sprintf("%s:[%s+%s*%d+0x%08x%08x]", seg, base, index, scale, 
                  GET32H(disp), GET32L(disp));
            }
            else {
              dis_sprintf("%s:[%s+%s*%d+0x%x]", seg, base, index, scale, (unsigned) disp);
            }
          }
          else {
            dis_sprintf("%s:[%s+%s*%d]", seg, base, index, scale);
          }
        }
        else
        {
          if (disp != 0) {
            if (disp64) {
              dis_sprintf("%s:[%s+%s+0x%08x%08x]", seg, base, index, GET32H(disp), GET32L(disp));
            }
            else {
              dis_sprintf("%s:[%s+%s+0x%x]", seg, base, index, (unsigned) disp);
            }
          }
          else
            dis_sprintf("%s:[%s+%s]", seg, base, index);
        }
      }
    }
  }
  else
  {
    if (base == NULL)
    {
      if (index == NULL)
      {
        if (disp64) {
          dis_sprintf("%s:0x%x", seg, GET32H(disp), GET32L(disp));
        } else {
          dis_sprintf("%s:0x%x", seg, (unsigned) disp);
        }
      }
      else
      {
        if (disp != 0) {
          if (disp64) {
            dis_sprintf("%s:0x%08x%08x(,%s,%d)", seg, GET32H(disp), GET32L(disp), index, scale);
          } else {
            dis_sprintf("%s:0x%x(,%s,%d)", seg, (unsigned) disp, index, scale);
          }
        }
        else
          dis_sprintf("%s:(,%s,%d)", seg, index, scale);
      }
    }
    else
    {
      if (index == NULL)
      {
        if (disp != 0) {
          if (disp64) {
            dis_sprintf("%s:0x%08x%08x(%s)", seg, GET32H(disp), GET32L(disp), base);
          }
          else {
            dis_sprintf("%s:0x%x(%s)", seg, (unsigned) disp, base);
          }
        }
        else
          dis_sprintf("%s:(%s)", seg, base);
      }
      else
      {
        if (disp != 0) {
          if (disp64) {
            dis_sprintf("%s:0x%08x%08x(%s,%s,%d)", seg, GET32H(disp), GET32L(disp), base, index, scale);
          }
          else {
            dis_sprintf("%s:0x%x(%s,%s,%d)", seg, (unsigned) disp, base, index, scale);
          }
        }
        else
          dis_sprintf("%s:(%s,%s,%d)", seg, base, index, scale);
      }
    }
  }
}
