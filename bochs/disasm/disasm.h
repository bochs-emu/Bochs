#ifndef _BX_DISASM_H_
#define _BX_DISASM_H_

#include "config.h"

#if BX_SUPPORT_X86_64
# define DISASM_REGISTERS 16
#else
# define DISASM_REGISTERS 8
#endif

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

// will be used in future
#define IA_8086       0x00000000        /* 8086 instruction */
#define IA_286        0x00000000        /* 286+ instruction */
#define IA_386        0x00000000        /* 386+ instruction */
#define IA_FPU        0x00000000
#define IA_486        0x00000000        /* 486+ instruction */
#define IA_PENTIUM    0x00000000        /* Pentium instruction */
#define IA_P6         0x00000000        /* P6 instruction */
#define IA_KATMAI     0x00000000        /* Katmai instruction */
#define IA_WILLAMETTE 0x00000000        /* Willamette instruction */
#define IA_PRESCOTT   0x00000000        /* Prescott instruction */
#define IA_X86_64     0x00000000        /* x86-64 specific instruction */

#define IF_ARITHMETIC 0x00000000        /* arithmetic instruction */
#define IF_LOGIC      0x00000000        /* logic instruction */
#define IF_SYSTEM     0x00000000        /* system instruction (require CPL=0) */
#define IF_BRANCH     0x00000000        /* branch instruction */
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

// datasize attributes
#define X_SIZE 0x0000
#define B_SIZE 0x0100
#define W_SIZE 0x0200
#define D_SIZE 0x0300
#define V_SIZE 0x0400
#define Q_SIZE 0x0500
#define O_SIZE 0x0600
#define T_SIZE 0x0700
#define P_SIZE 0x0800
#define S_SIZE 0x0900

class disassembler;

typedef void (disassembler::*BxDisasmPtr_t) (unsigned attr);
typedef void (disassembler::*BxDisasmResolveModrmPtr_t) (unsigned attr);

class disassembler {
public:
  disassembler() { set_syntax_intel(); }
  unsigned disasm(bx_bool is_32, Bit32u base, Bit32u ip, Bit8u *instr, char *disbuf);

  void set_syntax_att();
  void set_syntax_intel();

private:
  bx_bool intel_mode;

  const char **general_16bit_reg_name;
  const char **general_8bit_reg_name;
  const char **general_32bit_reg_name;

  const char **segment_name;

  const char **index16;
  const char **index_name32;

  const char *sreg_mod01or10_rm32[8];

  const char *sreg_mod00_base32[8];
  const char *sreg_mod01or10_base32[8];

  const char *sreg_mod00_rm16[8];
  const char *sreg_mod01or10_rm16[8];

private:

  bx_bool i32bit_opsize;
  bx_bool i32bit_addrsize;

  Bit8u  modrm, mod, nnn, rm;
  Bit8u  sib, scale, sib_index, sib_base;

  union {
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

  void resolve16_mod0    (unsigned mode);
  void resolve16_mod1or2 (unsigned mode);

  void resolve32_mod0    (unsigned mode);
  void resolve32_mod1or2 (unsigned mode);

  void resolve32_mod0_rm4    (unsigned mode);
  void resolve32_mod1or2_rm4 (unsigned mode);

  void initialize_modrm_segregs();

  void print_datasize (unsigned mode);

  void print_memory_access16(int datasize,
          const char *seg, const char *index, Bit16u disp);
  void print_memory_access32(int datasize,
          const char *seg, const char *base, const char *index, int scale, Bit32u disp);

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
 * F  - Flags Register.
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
 * X  - Memory addressed by the DS:rSI register pair.
 * Y  - Memory addressed by the ES:rDI register pair.
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
 * pd - 128-bit packed double-precision floating-point data.
 * pi - Quadword MMX technology register (packed integer)
 * ps - 128-bit packed single-precision floating-point data.
 * q  - Quadword, regardless of operand-size attribute.
 * s  - 6-byte or 10-byte pseudo-descriptor.
 * si - Doubleword integer register (scalar integer)
 * ss - Scalar element of a 128-bit packed single-precision floating data.
 * sd - Scalar element of a 128-bit packed double-precision floating data.
 * v  - Word, doubleword or quadword, depending on operand-size attribute.
 * w  - Word, regardless of operand-size attribute.
 * z  - A word if the effective operand size is 16 bits, or a doubleword 
 *      if the effective operand size is 32 or 64 bits.
 */

 void  XX (unsigned attribute) {}

 // fpu
 void ST0 (unsigned attribute);
 void STj (unsigned attribute);

 // general/segment register
 void  Rw (unsigned attribute);
 void  Rd (unsigned attribute);
 void  Sw (unsigned attribute);

 // control/debug register
 void  Cd (unsigned attribute);
 void  Dd (unsigned attribute);
 void  Td (unsigned attribute);

 // segment register
 void OP_SEG (unsigned attribute);

 // memory only
 void OP_MEM (unsigned attribute);

 // general purpose register 
 void REG16 (unsigned attribute);
 void REG8  (unsigned attribute);
 void REG32 (unsigned attribute);

 // string instructions
 void OP_X (unsigned attribute);
 void OP_Y (unsigned attribute);

 // mmx/xmm
 void OP_P (unsigned attribute);
 void OP_Q (unsigned attribute);
 void OP_W (unsigned attribute);
 void OP_V (unsigned attribute);

 // mov
 void OP_O (unsigned attribute);

 // immediate
 void  I1 (unsigned attribute);
 void  Ib (unsigned attribute);
 void  Iw (unsigned attribute); 
 void  Id (unsigned attribute);
 void  Iv (unsigned attribute);
 void sIb (unsigned attribute);

 // general purpose register or memory
 void  Eb (unsigned attribute);
 void  Ew (unsigned attribute);
 void  Ed (unsigned attribute);
 void  Ev (unsigned attribute);

 // general purpose register
 void  Gb (unsigned attribute);
 void  Gv (unsigned attribute);
 void  Gd (unsigned attribute);

 // call/jump
 void  Jb (unsigned attribute);
 void  Jv (unsigned attribute);

 // call/jmp far
 void  Ap (unsigned attribute);
};

#endif
