#ifndef _BX_DISASM_H_
#define _BX_DISASM_H_

#include "config.h"

#define BX_DECODE_MODRM(modrm_byte, mod, opcode, rm) { \
  mod    = (modrm_byte >> 6) & 0x03; \
  opcode = (modrm_byte >> 3) & 0x07; \
  rm     =  modrm_byte & 0x07;       \
}

#define BX_DECODE_SIB(sib_byte, scale, index, base) { \
  scale =  sib >> 6;          \
  index = (sib >> 3) & 0x07;  \
  base  =  sib & 0x07;        \
}

// to be used in future
#define IF_8086       0x00000000         /* 8086 instruction */
#define IF_186        0x00000000         /* 186+ instruction */
#define IF_286        0x00000000         /* 286+ instruction */
#define IF_386        0x00000000         /* 386+ instruction */
#define IF_387        0x00000000         /* 387+ FPU instruction */
#define IF_486        0x00000000         /* 486+ instruction */
#define IF_PENTIUM    0x00000000         /* Pentium instruction */
#define IF_P6         0x00000000         /* P6 instruction */
#define IF_KATMAI     0x00000000         /* Katmai instruction */
#define IF_WILLAMETTE 0x00000000         /* Willamette instruction */
#define IF_PRESCOTT   0x00000000         /* Prescott instruction */

#define IF_ARITHMETIC 0x00000000        /* arithmetic instruction */
#define IF_LOGIC      0x00000000        /* logic instruction */
#define IF_SYSTEM     0x00000000        /* system instruction (require CPL=0) */
#define IF_FPU        0x00000000        /* FPU instruction */
#define IF_MMX        0x00000000        /* MMX instruction */
#define IF_3DNOW      0x00000000        /* 3DNow! instruction */
#define IF_KNI        0x00000000        /* Katmai new instruction */
#define IF_PREFETCH   0x00000000        /* Prefetch instruction */
#define IF_SSE        0x00000000        /* SSE instruction */
#define IF_SSE2       0x00000000        /* SSE2 instruction */
#define IF_PNI        0x00000000        /* Prescott new instruction */

enum {
	AL_REG,
	CL_REG,
	DL_REG,
	BL_REG,
	AH_REG,
	CH_REG,
	DH_REG,
	BH_REG
};

enum {
	AX_REG,
	CX_REG,
	DX_REG,
	BX_REG,
	SP_REG,
	BP_REG,
	SI_REG,
	DI_REG
};

enum {
	eAX_REG,
	eCX_REG,
	eDX_REG,
	eBX_REG,
	eSP_REG,
	eBP_REG,
	eSI_REG,
	eDI_REG
};

enum {
	ES_REG,
	CS_REG,
	SS_REG,
	DS_REG,
	FS_REG,
	GS_REG
};

#define X_MODE  0x0
#define B_MODE  0x1
#define W_MODE  0x2
#define D_MODE  0x3
#define V_MODE  0x4
#define Q_MODE  0x5
#define O_MODE  0x6
#define T_MODE  0x7
#define P_MODE  0x8

class disassembler;

typedef void (disassembler::*BxDisasmPtr_t) (unsigned attr);
typedef void (disassembler::*BxDisasmResolveModrmPtr_t) (unsigned attr);

class disassembler {
public:
  disassembler() {}
  unsigned disasm(bx_bool is_32, Bit32u base, Bit32u ip, Bit8u *instr, char *disbuf);

private:
  bx_bool i32bit_opsize;
  bx_bool i32bit_addrsize;

  Bit8u  modrm, mod, nnn, rm;
  Bit8u  sib, scale, index, base;

  union {
     Bit8u  displ8;
     Bit16u displ16;
     Bit32u displ32;
  } displacement;

  Bit32u db_eip;
  Bit32u db_base;

  unsigned n_prefixes;

  Bit8u *instruction_begin;  // keep track of where instruction starts
  Bit8u *instruction;        // for fetching of next byte of instruction

  const char *seg_override;

  char *disbufptr;

  BxDisasmResolveModrmPtr_t resolve_modrm;

  BX_CPP_INLINE Bit8u  fetch_byte() {
    db_eip++;
    return(*instruction++);
    };

  BX_CPP_INLINE Bit8u  peek_byte() {
    return(*instruction);
    };

  BX_CPP_INLINE Bit16u fetch_word() {
    Bit8u b0 = * (Bit8u *) instruction++;
    Bit8u b1 = * (Bit8u *) instruction++;
    Bit16u ret16 = (b1<<8) | b0;
    db_eip += 2;
    return(ret16);
    };

  BX_CPP_INLINE Bit32u fetch_dword() {
    Bit8u b0 = * (Bit8u *) instruction++;
    Bit8u b1 = * (Bit8u *) instruction++;
    Bit8u b2 = * (Bit8u *) instruction++;
    Bit8u b3 = * (Bit8u *) instruction++;
    Bit32u ret32 = (b3<<24) | (b2<<16) | (b1<<8) | b0;
    db_eip += 4;
    return(ret32);
    };

  void dis_sprintf(char *fmt, ...);
  void decode_modrm();

public:

/* 
 * Codes for Addressing Method:
 * ---------------------------
 * A  - Direct address. The instruction has no ModR/M byte; the address 
 *      of the operand is encoded in the instruction; and no base register, 
 *      index register, or scaling factor can be applied.
 * C  - The reg field of the ModR/M byte selects a control register.
 * D  - The reg field of the ModR/M byte selects a debug register.
 * E  - A ModR/M byte follows the opcode and specifies the operand. The 
 *      operand is either a general-purpose register or a memory address. 
 *      If it is a memory address, the address is computed from a segment 
 *      register and any of the following values: a base register, an
 *      index register, a scaling factor, a displacement.
 * F  - EFLAGS Register.
 * G  - The reg field of the ModR/M byte selects a general register.
 * I  - Immediate data. The operand value is encoded in subsequent bytes of 
 *      the instruction.
 * J  - The instruction contains a relative offset to be added to the 
 *      instruction pointer register.
 * M  - The ModR/M byte may refer only to memory.
 * O  - The instruction has no ModR/M byte; the offset of the operand is 
 *      coded as a word or double word (depending on address size attribute) 
 *      in the instruction. No base register, index register, or scaling 
 *      factor can be applied.
 * P  - The reg field of the ModR/M byte selects a packed quadword MMX 
 *      technology register.
 * Q  - A ModR/M byte follows the opcode and specifies the operand. The 
 *      operand is either an MMX technology register or a memory address. 
 *      If it is a memory address, the address is computed from a segment 
 *      register and any of the following values: a base register, an 
 *      index register, a scaling factor, and a displacement.
 * R  - The mod field of the ModR/M byte may refer only to a general register.
 * S  - The reg field of the ModR/M byte selects a segment register.
 * T  - The reg field of the ModR/M byte selects a test register.
 * V  - The reg field of the ModR/M byte selects a 128-bit XMM register.
 * W  - A ModR/M byte follows the opcode and specifies the operand. The 
 *      operand is either a 128-bit XMM register or a memory address. If 
 *      it is a memory address, the address is computed from a segment 
 *      register and any of the following values: a base register, an
 *      index register, a scaling factor, and a displacement.
 * X  - Memory addressed by the DS:SI register pair.
 * Y  - Memory addressed by the ES:DI register pair.
 */   

/* 
 * Codes for Operand Type:
 * ----------------------
 * a  - Two one-word operands in memory or two double-word operands in 
 *      memory, depending on operand-size attribute (used only by the BOUND
 *      instruction).
 * b  - Byte, regardless of operand-size attribute.
 * d  - Doubleword, regardless of operand-size attribute.
 * dq - Double-quadword, regardless of operand-size attribute.
 * p  - 32-bit or 48-bit pointer, depending on operand-size attribute.
 * pi - Quadword MMX technology register (e.g. mm0)
 * ps - 128-bit packed single-precision floating-point data.
 * q  - Quadword, regardless of operand-size attribute.
 * s  - 6-byte pseudo-descriptor.
 * ss - Scalar element of a 128-bit packed single-precision floating data.
 * si - Doubleword integer register (e.g., eax)
 * v  - Word or doubleword, depending on operand-size attribute.
 * w  - Word, regardless of operand-size attribute.
 */

 void XX (unsigned) {}

 // fpu
 void ST0 (unsigned) { dis_sprintf("st(0)"); }
 void STj (unsigned);

 // general/segment register
 void Rd (unsigned);
 void Rw (unsigned);
 void Sw (unsigned);

 // control/debug register
 void Cd (unsigned) { dis_sprintf("cr%d", nnn); }
 void Dd (unsigned) { dis_sprintf("db%d", nnn); }
 void Td (unsigned) { dis_sprintf("tr%d", nnn); }

 // segment register (implicit)
 void OP_SEG (unsigned);

 // general purpose register 
 void reg8  (unsigned);
 void reg16 (unsigned);
 void reg32 (unsigned);

 // memory only
 void OP_MEM (unsigned);

 void OP_X (unsigned);
 void OP_Y (unsigned);

 // mmx/xmm
 void OP_P (unsigned);
 void OP_Q (unsigned);
 void OP_W (unsigned);
 void OP_V (unsigned);

 // immediate
 void  I1 (unsigned) { dis_sprintf("1"); }
 void  Ib (unsigned);
 void  Iw (unsigned); 
 void  Id (unsigned);
 void  Iv (unsigned);
 void sIb (unsigned);

 void  Eb (unsigned);
 void  Ew (unsigned);
 void  Ed (unsigned);
 void  Ev (unsigned);
 void  Ea (unsigned);
 void  Ep (unsigned);

 void  Gb (unsigned);
 void  Gw (unsigned);
 void  Gv (unsigned);
 void  Gd (unsigned);

 void  Ap (unsigned);

 void  Ob (unsigned);
 void  Ov (unsigned);

 // jump
 void  Jb (unsigned);
 void  Jv (unsigned);

private:

 void resolve16_mod0 (unsigned mode);
 void resolve16_mod1 (unsigned mode);
 void resolve16_mod2 (unsigned mode);

 void resolve32_mod0 (unsigned mode);
 void resolve32_mod1 (unsigned mode);
 void resolve32_mod2 (unsigned mode);

 void resolve32_mod0_rm4 (unsigned mode);
 void resolve32_mod1_rm4 (unsigned mode);
 void resolve32_mod2_rm4 (unsigned mode);

 void print_datasize (unsigned mode);

};

#endif
