/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2016-2019  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_X86_DECODER_H
#define BX_X86_DECODER_H

// x86 Arch features
enum x86_feature_name {
  BX_ISA_386 = 0,                 /* 386 or earlier instruction */
  BX_ISA_X87,                     /* FPU (X87) instruction */
  BX_ISA_486,                     /* 486 new instruction */
  BX_ISA_PENTIUM,                 /* Pentium new instruction */
  BX_ISA_P6,                      /* P6 new instruction */
  BX_ISA_MMX,                     /* MMX instruction */
  BX_ISA_3DNOW,                   /* 3DNow! instruction (AMD) */
  BX_ISA_DEBUG_EXTENSIONS,        /* Debug Extensions support */
  BX_ISA_VME,                     /* VME support */
  BX_ISA_PSE,                     /* PSE support */
  BX_ISA_PAE,                     /* PAE support */
  BX_ISA_PGE,                     /* Global Pages support */
  BX_ISA_PSE36,                   /* PSE-36 support */
  BX_ISA_MTRR,                    /* MTRR support */
  BX_ISA_PAT,                     /* PAT support */
  BX_ISA_SYSCALL_SYSRET_LEGACY,   /* SYSCALL/SYSRET in legacy mode (AMD) */
  BX_ISA_SYSENTER_SYSEXIT,        /* SYSENTER/SYSEXIT instruction */
  BX_ISA_CLFLUSH,                 /* CLFLUSH instruction */
  BX_ISA_CLFLUSHOPT,              /* CLFLUSHOPT instruction */
  BX_ISA_CLWB,                    /* CLWB instruction */
  BX_ISA_CLDEMOTE,                /* CLDEMOTE instruction */
  BX_ISA_SSE,                     /* SSE  instruction */
  BX_ISA_SSE2,                    /* SSE2 instruction */
  BX_ISA_SSE3,                    /* SSE3 instruction */
  BX_ISA_SSSE3,                   /* SSSE3 instruction */
  BX_ISA_SSE4_1,                  /* SSE4_1 instruction */
  BX_ISA_SSE4_2,                  /* SSE4_2 instruction */
  BX_ISA_POPCNT,                  /* POPCNT instruction */
  BX_ISA_MONITOR_MWAIT,           /* MONITOR/MWAIT instruction */
  BX_ISA_MONITORX_MWAITX,         /* MONITORX/MWAITX instruction (AMD) */
  BX_ISA_WAITPKG,                 /* TPAUSE/UMONITOR/UMWAIT instructions */
  BX_ISA_VMX,                     /* VMX instruction */
  BX_ISA_SMX,                     /* SMX instruction */
  BX_ISA_LONG_MODE,               /* Long Mode (x86-64) support */
  BX_ISA_LM_LAHF_SAHF,            /* Long Mode LAHF/SAHF instruction */
  BX_ISA_NX,                      /* No-Execute support */
  BX_ISA_1G_PAGES,                /* 1Gb pages support */
  BX_ISA_CMPXCHG16B,              /* CMPXCHG16B instruction */
  BX_ISA_RDTSCP,                  /* RDTSCP instruction */
  BX_ISA_FFXSR,                   /* EFER.FFXSR support */
  BX_ISA_XSAVE,                   /* XSAVE/XRSTOR extensions instruction */
  BX_ISA_XSAVEOPT,                /* XSAVEOPT instruction */
  BX_ISA_XSAVEC,                  /* XSAVEC instruction */
  BX_ISA_XSAVES,                  /* XSAVES instruction */
  BX_ISA_AES_PCLMULQDQ,           /* AES+PCLMULQDQ instructions */
  BX_ISA_VAES_VPCLMULQDQ,         /* Wide vector versions of AES+PCLMULQDQ instructions */
  BX_ISA_MOVBE,                   /* MOVBE instruction */
  BX_ISA_FSGSBASE,                /* FS/GS BASE access instruction */
  BX_ISA_INVPCID,                 /* INVPCID instruction */
  BX_ISA_AVX,                     /* AVX instruction */
  BX_ISA_AVX2,                    /* AVX2 instruction */
  BX_ISA_AVX_F16C,                /* AVX F16 convert instruction */
  BX_ISA_AVX_FMA,                 /* AVX FMA instruction */
  BX_ISA_ALT_MOV_CR8,             /* LOCK CR0 access CR8 (AMD) */
  BX_ISA_SSE4A,                   /* SSE4A instruction (AMD) */
  BX_ISA_MISALIGNED_SSE,          /* Misaligned SSE (AMD) */
  BX_ISA_LZCNT,                   /* LZCNT instruction */
  BX_ISA_BMI1,                    /* BMI1 instruction */
  BX_ISA_BMI2,                    /* BMI2 instruction */
  BX_ISA_FMA4,                    /* FMA4 instruction (AMD) */
  BX_ISA_XOP,                     /* XOP instruction (AMD) */
  BX_ISA_TBM,                     /* TBM instruction (AMD) */
  BX_ISA_SVM,                     /* SVM instruction (AMD) */
  BX_ISA_RDRAND,                  /* RDRAND instruction */
  BX_ISA_ADX,                     /* ADCX/ADOX instruction */
  BX_ISA_SMAP,                    /* SMAP support */
  BX_ISA_RDSEED,                  /* RDSEED instruction */
  BX_ISA_SHA,                     /* SHA instruction */
  BX_ISA_GFNI,                    /* GFNI instruction */
  BX_ISA_AVX512,                  /* AVX-512 instruction */
  BX_ISA_AVX512_CD,               /* AVX-512 Conflict Detection instruction */
  BX_ISA_AVX512_PF,               /* AVX-512 Sparse Prefetch instruction */
  BX_ISA_AVX512_ER,               /* AVX-512 Exponential/Reciprocal instruction */
  BX_ISA_AVX512_DQ,               /* AVX-512DQ instruction */
  BX_ISA_AVX512_BW,               /* AVX-512 Byte/Word instruction */
  BX_ISA_AVX512_VL,               /* AVX-512 Vector Length extensions */
  BX_ISA_AVX512_VBMI,             /* AVX-512 VBMI : Vector Bit Manipulation Instructions */
  BX_ISA_AVX512_VBMI2,            /* AVX-512 VBMI2 : Vector Bit Manipulation Instructions */
  BX_ISA_AVX512_IFMA52,           /* AVX-512 IFMA52 Instructions */
  BX_ISA_AVX512_VPOPCNTDQ,        /* AVX-512 VPOPCNTD/VPOPCNTQ Instructions */
  BX_ISA_AVX512_VNNI,             /* AVX-512 VNNI Instructions */
  BX_ISA_AVX512_BITALG,           /* AVX-512 BITALG Instructions */
  BX_ISA_AVX512_VP2INTERSECT,     /* AVX-512 VP2INTERSECT Instructions */
  BX_ISA_XAPIC,                   /* XAPIC support */
  BX_ISA_X2APIC,                  /* X2APIC support */
  BX_ISA_XAPIC_EXT,               /* XAPIC Extensions support */
  BX_ISA_PCID,                    /* PCID pages support */
  BX_ISA_SMEP,                    /* SMEP support */
  BX_ISA_TSC_ADJUST,              /* TSC-Adjust MSR */
  BX_ISA_TSC_DEADLINE,            /* TSC-Deadline */
  BX_ISA_FOPCODE_DEPRECATION,     /* FOPCODE Deprecation - FOPCODE update on unmasked x87 exception only */
  BX_ISA_FCS_FDS_DEPRECATION,     /* FCS/FDS Deprecation */
  BX_ISA_FDP_DEPRECATION,         /* FDP Deprecation - FDP update on unmasked x87 exception only */
  BX_ISA_PKU,                     /* User-Mode Protection Keys */
  BX_ISA_PKS,                     /* Supervisor-Mode Protection Keys */
  BX_ISA_UMIP,                    /* User-Mode Instructions Prevention */
  BX_ISA_RDPID,                   /* RDPID Support */
  BX_ISA_TCE,                     /* Translation Cache Extensions (TCE) support (AMD) */
  BX_ISA_CLZERO,                  /* CLZERO instruction support (AMD) */
  BX_ISA_SCA_MITIGATIONS,         /* SCA Mitigations */
  BX_ISA_CET,                     /* Control Flow Enforcement */
  BX_ISA_EXTENSION_LAST
};                            

#define BX_ISA_EXTENSIONS_ARRAY_SIZE (4)

#if (BX_ISA_EXTENSION_LAST) >= (BX_ISA_EXTENSIONS_ARRAY_SIZE*32)
  #error "ISA extensions array limit exceeded!"
#endif

// segment register encoding
enum BxSegregs {
  BX_SEG_REG_ES = 0,
  BX_SEG_REG_CS = 1,
  BX_SEG_REG_SS = 2,
  BX_SEG_REG_DS = 3,
  BX_SEG_REG_FS = 4,
  BX_SEG_REG_GS = 5,
  // NULL now has to fit in 3 bits.
  BX_SEG_REG_NULL = 7
};

#define BX_NULL_SEG_REG(seg) ((seg) == BX_SEG_REG_NULL)

enum BxRegs8L {
  BX_8BIT_REG_AL,
  BX_8BIT_REG_CL,
  BX_8BIT_REG_DL,
  BX_8BIT_REG_BL,
  BX_8BIT_REG_SPL,
  BX_8BIT_REG_BPL,
  BX_8BIT_REG_SIL,
  BX_8BIT_REG_DIL,
#if BX_SUPPORT_X86_64
  BX_8BIT_REG_R8,
  BX_8BIT_REG_R9,
  BX_8BIT_REG_R10,
  BX_8BIT_REG_R11,
  BX_8BIT_REG_R12,
  BX_8BIT_REG_R13,
  BX_8BIT_REG_R14,
  BX_8BIT_REG_R15
#endif
};

enum BxRegs8H {
  BX_8BIT_REG_AH,
  BX_8BIT_REG_CH,
  BX_8BIT_REG_DH,
  BX_8BIT_REG_BH
};

enum BxRegs16 {
  BX_16BIT_REG_AX,
  BX_16BIT_REG_CX,
  BX_16BIT_REG_DX,
  BX_16BIT_REG_BX,
  BX_16BIT_REG_SP,
  BX_16BIT_REG_BP,
  BX_16BIT_REG_SI,
  BX_16BIT_REG_DI,
#if BX_SUPPORT_X86_64
  BX_16BIT_REG_R8,
  BX_16BIT_REG_R9,
  BX_16BIT_REG_R10,
  BX_16BIT_REG_R11,
  BX_16BIT_REG_R12,
  BX_16BIT_REG_R13,
  BX_16BIT_REG_R14,
  BX_16BIT_REG_R15,
#endif
};

enum BxRegs32 {
  BX_32BIT_REG_EAX,
  BX_32BIT_REG_ECX,
  BX_32BIT_REG_EDX,
  BX_32BIT_REG_EBX,
  BX_32BIT_REG_ESP,
  BX_32BIT_REG_EBP,
  BX_32BIT_REG_ESI,
  BX_32BIT_REG_EDI,
#if BX_SUPPORT_X86_64
  BX_32BIT_REG_R8,
  BX_32BIT_REG_R9,
  BX_32BIT_REG_R10,
  BX_32BIT_REG_R11,
  BX_32BIT_REG_R12,
  BX_32BIT_REG_R13,
  BX_32BIT_REG_R14,
  BX_32BIT_REG_R15,
#endif
};

#if BX_SUPPORT_X86_64
enum BxRegs64 {
  BX_64BIT_REG_RAX,
  BX_64BIT_REG_RCX,
  BX_64BIT_REG_RDX,
  BX_64BIT_REG_RBX,
  BX_64BIT_REG_RSP,
  BX_64BIT_REG_RBP,
  BX_64BIT_REG_RSI,
  BX_64BIT_REG_RDI,
  BX_64BIT_REG_R8,
  BX_64BIT_REG_R9,
  BX_64BIT_REG_R10,
  BX_64BIT_REG_R11,
  BX_64BIT_REG_R12,
  BX_64BIT_REG_R13,
  BX_64BIT_REG_R14,
  BX_64BIT_REG_R15,
};
#endif

#if BX_SUPPORT_X86_64
# define BX_GENERAL_REGISTERS 16
#else
# define BX_GENERAL_REGISTERS 8
#endif

static const unsigned BX_16BIT_REG_IP  = (BX_GENERAL_REGISTERS),
                      BX_32BIT_REG_EIP = (BX_GENERAL_REGISTERS),
                      BX_64BIT_REG_RIP = (BX_GENERAL_REGISTERS);

static const unsigned BX_32BIT_REG_SSP = (BX_GENERAL_REGISTERS+1),
                      BX_64BIT_REG_SSP = (BX_GENERAL_REGISTERS+1);

static const unsigned BX_TMP_REGISTER = (BX_GENERAL_REGISTERS+2);
static const unsigned BX_NIL_REGISTER = (BX_GENERAL_REGISTERS+3);

enum OpmaskRegs {
  BX_REG_OPMASK_K0,
  BX_REG_OPMASK_K1,
  BX_REG_OPMASK_K2,
  BX_REG_OPMASK_K3,
  BX_REG_OPMASK_K4,
  BX_REG_OPMASK_K5,
  BX_REG_OPMASK_K6,
  BX_REG_OPMASK_K7
};

// AVX Registers
enum bx_avx_vector_length {
  BX_NO_VL,
  BX_VL128  = 1,
  BX_VL256  = 2,
  BX_VL512  = 4,
};

#if BX_SUPPORT_EVEX
#  define BX_VLMAX BX_VL512
#else
#  if BX_SUPPORT_AVX
#    define BX_VLMAX BX_VL256
#  else
#    define BX_VLMAX BX_VL128
#  endif
#endif

#if BX_SUPPORT_EVEX
#  define BX_XMM_REGISTERS 32
#else
#  if BX_SUPPORT_X86_64
#    define BX_XMM_REGISTERS 16
#  else
#    define BX_XMM_REGISTERS 8
#  endif
#endif

static const unsigned BX_VECTOR_TMP_REGISTER = (BX_XMM_REGISTERS);

#endif // BX_X86_DECODER_H

