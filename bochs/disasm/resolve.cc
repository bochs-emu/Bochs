#include "disasm.h"

extern const char *general_16bit_reg_name[8];
extern const char *general_32bit_reg_name[8];

static const char *sreg_mod01_rm32[8] = {
    "ds", "ds", "ds", "ds", "??", "ss", "ds", "ds"
};

static const char *sreg_mod10_rm32[8] = {
    "ds", "ds", "ds", "ds", "??", "ss", "ds", "ds"
};

static const char *sreg_mod00_base32[8] = {
    "ds", "ds", "ds", "ds", "ss", "ds", "ds", "ds"
};

static const char *sreg_mod01_base32[8] = {
    "ds", "ds", "ds", "ds", "ss", "ss", "ds", "ds"
};

static const char *sreg_mod10_base32[8] = {
    "ds", "ds", "ds", "ds", "ss", "ss", "ds", "ds"
};

static const char *sreg_mod00_rm16[8] = {
    "ds", "ds", "ss", "ss", "ds", "ds", "ds", "ds"
};

static const char *sreg_mod01_rm16[8] = {
    "ds", "ds", "ss", "ss", "ds", "ds", "ss", "ds"
};

static const char *sreg_mod10_rm16[8] = {
    "ds", "ds", "ss", "ss", "ds", "ds", "ss", "ds"
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

static const char *index_name32[8] = {
    "eax", "ecx", "edx", "ebx", "???", "ebp", "esi", "edi"
};

void disassembler::decode_modrm()
{
  modrm = fetch_byte();
  BX_DECODE_MODRM(modrm, mod, nnn, rm);

  if (i32bit_addrsize)
  {
    /* use 32bit addressing modes. orthogonal base & index registers,
       scaling available, etc. */
    if (mod == 3) { 
      /* mod, reg, reg */
      return;
    }
    else { /* mod != 3 */
      if (rm != 4) { /* rm != 100b, no s-i-b byte */
        // one byte modrm
        switch (mod) {
          case 0:
            resolve_modrm = &disassembler::resolve32_mod0;
            if (rm == 5) /* no reg, 32-bit displacement */
              displacement.displ32 = fetch_dword();
            break;
          case 1:
            /* reg, 8-bit displacement, sign extend */
            resolve_modrm = &disassembler::resolve32_mod1;
            displacement.displ32 = (Bit8s) fetch_byte();
            break;
          case 2:
            /* reg, 32-bit displacement */
            resolve_modrm = &disassembler::resolve32_mod2;
            displacement.displ32 = fetch_dword();
            break;
        } /* switch (mod) */
      } /* if (rm != 4) */
      else { /* rm == 4, s-i-b byte follows */
        sib = fetch_byte();
        BX_DECODE_SIB(sib, scale, index, base);

        switch (mod) {
          case 0:
            resolve_modrm = &disassembler::resolve32_mod0_rm4;
            if (base == 5)
              displacement.displ32 = fetch_dword();
            break;
          case 1:
            resolve_modrm = &disassembler::resolve32_mod1_rm4;
            displacement.displ8 = fetch_byte();
            break;
          case 2:
            resolve_modrm = &disassembler::resolve32_mod2_rm4;
            displacement.displ32 = fetch_dword();
            break;
        }
      } /* s-i-b byte follows */
    } /* if (mod != 3) */
  }
  else {
    /* 16 bit addressing modes. */
    switch (mod) {
      case 0:
        resolve_modrm = &disassembler::resolve16_mod0;
        if(rm == 6)
          displacement.displ16 = fetch_word();
        break;
      case 1:
        /* reg, 8-bit displacement, sign extend */
        resolve_modrm = &disassembler::resolve16_mod1;
        displacement.displ16 = (Bit8s) fetch_byte();
        break;
      case 2:
        resolve_modrm = &disassembler::resolve16_mod2;
        displacement.displ16 = fetch_word();
        break;
      case 3:
        /* mod, reg, reg */        
        return;

    } /* switch (mod) ... */
  }
}

void disassembler::print_datasize(unsigned mode)
{
  switch(mode)
  {
    case B_MODE:
      dis_sprintf("byte ptr ");
      break;
    case W_MODE:
      dis_sprintf("word ptr ");
      break;
    case D_MODE:
      dis_sprintf("dword ptr ");
      break;
    case V_MODE:
      if (i32bit_opsize)
        dis_sprintf("dword ptr ");
      else
        dis_sprintf("word ptr ");
      break;
    case Q_MODE:
      dis_sprintf("qword ptr ");
      break;
    case O_MODE:
      dis_sprintf("oword ptr ");
      break;
    case T_MODE:
      dis_sprintf("tword ptr ");
      break;
    case X_MODE:
      break;
  };
}

void disassembler::resolve16_mod0(unsigned mode)
{
  const char *mod_rm_seg_reg;

  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = sreg_mod00_rm16[rm];

  print_datasize(mode);

  if(rm == 6)
  {
    dis_sprintf("[%s:0x%x]", mod_rm_seg_reg, (unsigned) displacement.displ16);
  }
  else
  {
    dis_sprintf("%s:[%s]", mod_rm_seg_reg, intel_index16[rm]);
  }
}

void disassembler::resolve16_mod1(unsigned mode)
{
  const char *mod_rm_seg_reg;

  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = sreg_mod01_rm16[rm];

  print_datasize(mode);

  if (displacement.displ16)
  {
    dis_sprintf("%s:[%s+0x%x]", mod_rm_seg_reg, 
              intel_index16[rm], (unsigned) displacement.displ16);
  }
  else
  {
    dis_sprintf("%s:[%s]", mod_rm_seg_reg, intel_index16[rm]);
  }
}

void disassembler::resolve16_mod2(unsigned mode)
{
  const char *mod_rm_seg_reg;

  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = sreg_mod10_rm16[rm];

  print_datasize(mode);

  if (displacement.displ16)
  {
    dis_sprintf("%s:[%s+0x%x]", mod_rm_seg_reg, 
              intel_index16[rm], (unsigned) displacement.displ16);
  }
  else
  {
    dis_sprintf("%s:[%s]", mod_rm_seg_reg, intel_index16[rm]);
  }
}

void disassembler::resolve32_mod0(unsigned mode)
{
  const char *mod_rm_seg_reg;

  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = "ds";

  print_datasize(mode);
 
 if (rm == 5) { /* no reg, 32-bit displacement */
    dis_sprintf("[%s:0x%x]", mod_rm_seg_reg, displacement.displ32);
 }
 else {
    dis_sprintf("%s:[%s]", mod_rm_seg_reg, general_32bit_reg_name[rm]);
 }
}

void disassembler::resolve32_mod1(unsigned mode)
{
  const char *mod_rm_seg_reg;

  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = sreg_mod01_rm32[rm];

  print_datasize(mode);
  
  /* reg, 8-bit displacement, sign extend */
  if (displacement.displ32)
  {
    dis_sprintf("%s:[%s+0x%x]", mod_rm_seg_reg,
              general_32bit_reg_name[rm], (unsigned) displacement.displ32);
  }
  else
  {
    dis_sprintf("%s:[%s]", mod_rm_seg_reg, general_32bit_reg_name[rm]);
  }
}

void disassembler::resolve32_mod2(unsigned mode)
{
  const char *mod_rm_seg_reg;
 
  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = sreg_mod10_rm32[rm];

  print_datasize(mode);

  /* reg, 32-bit displacement */
  if (displacement.displ32)
  {
    dis_sprintf("%s:[%s+0x%x]", mod_rm_seg_reg,
           general_32bit_reg_name[rm], (unsigned) displacement.displ32);
  }
  else
  {
    dis_sprintf("%s:[%s]", mod_rm_seg_reg, general_32bit_reg_name[rm]);
  }
}

void disassembler::resolve32_mod0_rm4(unsigned mode)
{
  const char *mod_rm_seg_reg;

  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = sreg_mod00_base32[base];

  print_datasize(mode);

  dis_sprintf("%s:[", mod_rm_seg_reg);
  if (base != 5)
    dis_sprintf("%s+", general_32bit_reg_name[base]);

  if (index != 4)
  {
    dis_sprintf("%s", index_name32[index]);
    if (scale)
      dis_sprintf("*%u", 1 << scale);
    if (base == 5) dis_sprintf("+");
  }

  if (base == 5)
    dis_sprintf("0x%x", (unsigned) displacement.displ32);

  dis_sprintf("]");
}

void disassembler::resolve32_mod1_rm4(unsigned mode)
{
  const char *mod_rm_seg_reg;
  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = sreg_mod01_base32[base];

  print_datasize(mode);

  dis_sprintf("%s:[%s", mod_rm_seg_reg, general_32bit_reg_name[base]);

  if (index != 4)
  {
    dis_sprintf("+%s", index_name32[index]);
    if (scale)
      dis_sprintf("*%u", 1 << scale);
  }

  if (displacement.displ8)
    dis_sprintf("+0x%x", (unsigned) displacement.displ8);
 
  dis_sprintf("]");
}

void disassembler::resolve32_mod2_rm4(unsigned mode)
{
  const char *mod_rm_seg_reg;
  if (seg_override)
    mod_rm_seg_reg = seg_override;
  else
    mod_rm_seg_reg = sreg_mod10_base32[base];

  print_datasize(mode);

  dis_sprintf("%s:[%s", mod_rm_seg_reg, general_32bit_reg_name[base]);

  if (index != 4)
  {
    dis_sprintf("+%s", index_name32[index]);
    if (scale)
      dis_sprintf("*%u", 1 << scale);
  }

  if (displacement.displ32)
    dis_sprintf("+0x%x", (unsigned) displacement.displ32);

  dis_sprintf("]");
}
