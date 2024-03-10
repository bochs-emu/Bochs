/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2023  The Bochs Project
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

x86_feature(BX_ISA_386, "386ni")                                        /* 386 or earlier instruction */
x86_feature(BX_ISA_X87, "x87")                                          /* FPU (x87) instruction */
x86_feature(BX_ISA_486, "486ni")                                        /* 486 new instruction */
x86_feature(BX_ISA_PENTIUM, "pentium_ni")                               /* Pentium new instruction */
x86_feature(BX_ISA_P6, "p6ni")                                          /* P6 new instruction */
x86_feature(BX_ISA_MMX, "mmx")                                          /* MMX instruction */
x86_feature(BX_ISA_3DNOW, "3dnow")                                      /* 3DNow! instruction (AMD) */
x86_feature(BX_ISA_DEBUG_EXTENSIONS, "debugext")                        /* Debug Extensions support */
x86_feature(BX_ISA_VME, "vme")                                          /* VME support */
x86_feature(BX_ISA_PSE, "pse")                                          /* PSE support */
x86_feature(BX_ISA_PAE, "pae")                                          /* PAE support */
x86_feature(BX_ISA_PGE, "pge")                                          /* Global Pages support */
x86_feature(BX_ISA_PSE36, "pse36")                                      /* PSE-36 support */
x86_feature(BX_ISA_MTRR, "mtrr")                                        /* MTRR support */
x86_feature(BX_ISA_PAT, "pat")                                          /* PAT support */
x86_feature(BX_ISA_SYSCALL_SYSRET_LEGACY, "legacy_syscall_sysret")      /* SYSCALL/SYSRET in legacy mode (AMD) */
x86_feature(BX_ISA_SYSENTER_SYSEXIT, "sysenter_sysexit")                /* SYSENTER/SYSEXIT instruction */
x86_feature(BX_ISA_CLFLUSH, "clflush")                                  /* CLFLUSH instruction */
x86_feature(BX_ISA_CLFLUSHOPT, "clflushopt")                            /* CLFLUSHOPT instruction */
x86_feature(BX_ISA_CLWB, "clwb")                                        /* CLWB instruction */
x86_feature(BX_ISA_CLDEMOTE, "cldemote")                                /* CLDEMOTE instruction */
x86_feature(BX_ISA_SSE, "sse")                                          /* SSE  instruction */
x86_feature(BX_ISA_SSE2, "sse2")                                        /* SSE2 instruction */
x86_feature(BX_ISA_SSE3, "sse3")                                        /* SSE3 instruction */
x86_feature(BX_ISA_SSSE3, "ssse3")                                      /* SSSE3 instruction */
x86_feature(BX_ISA_SSE4_1, "sse4_1")                                    /* SSE4_1 instruction */
x86_feature(BX_ISA_SSE4_2, "sse4_2")                                    /* SSE4_2 instruction */
x86_feature(BX_ISA_POPCNT, "popcnt")                                    /* POPCNT instruction */
x86_feature(BX_ISA_MONITOR_MWAIT, "mwait")                              /* MONITOR/MWAIT instruction */
x86_feature(BX_ISA_WAITPKG, "waitpkg")                                  /* TPAUSE/UMONITOR/UMWAIT instructions */
x86_feature(BX_ISA_MONITORX_MWAITX, "mwaitx")                           /* MONITORX/MWAITX instruction (AMD) */
x86_feature(BX_ISA_VMX, "vmx")                                          /* VMX instruction */
x86_feature(BX_ISA_SMX, "smx")                                          /* SMX instruction */
x86_feature(BX_ISA_LONG_MODE, "longmode")                               /* Long Mode (x86-64) support */
x86_feature(BX_ISA_LM_LAHF_SAHF, "lm_lahf_sahf")                        /* Long Mode LAHF/SAHF instruction */
x86_feature(BX_ISA_NX, "nx")                                            /* No-Execute Pages support */
x86_feature(BX_ISA_1G_PAGES, "1g_pages")                                /* 1Gb pages support */
x86_feature(BX_ISA_CMPXCHG16B, "cmpxhg16b")                             /* CMPXCHG16B instruction */
x86_feature(BX_ISA_RDTSCP, "rdtscp")                                    /* RDTSCP instruction */
x86_feature(BX_ISA_FFXSR, "ffxsr")                                      /* EFER.FFXSR support (AMD) */
x86_feature(BX_ISA_XSAVE, "xsave")                                      /* XSAVE/XRSTOR extensions instruction */
x86_feature(BX_ISA_XSAVEOPT, "xsaveopt")                                /* XSAVEOPT instruction */
x86_feature(BX_ISA_XSAVEC, "xsavec")                                    /* XSAVEC instruction */
x86_feature(BX_ISA_XSAVES, "xsaves")                                    /* XSAVES instruction */
x86_feature(BX_ISA_AES_PCLMULQDQ, "aes_pclmulqdq")                      /* AES+PCLMULQDQ instructions */
x86_feature(BX_ISA_VAES_VPCLMULQDQ, "vaes_vpclmulqdq")                  /* Wide vector versions of AES+PCLMULQDQ instructions */
x86_feature(BX_ISA_MOVBE, "movbe")                                      /* MOVBE instruction */
x86_feature(BX_ISA_FSGSBASE, "fsgsbase")                                /* FS/GS BASE access instruction */
x86_feature(BX_ISA_INVPCID, "invpcid")                                  /* INVPCID instruction */
x86_feature(BX_ISA_AVX, "avx")                                          /* AVX instruction */
x86_feature(BX_ISA_AVX2, "avx2")                                        /* AVX2 instruction */
x86_feature(BX_ISA_AVX_F16C, "avx_f16c")                                /* AVX F16 convert instruction */
x86_feature(BX_ISA_AVX_FMA, "avx_fma")                                  /* AVX FMA instruction */
x86_feature(BX_ISA_ALT_MOV_CR8, "altmovcr8")                            /* LOCK CR0 access CR8 (AMD) */
x86_feature(BX_ISA_SSE4A, "sse4a")                                      /* SSE4A instruction (AMD) */
x86_feature(BX_ISA_MISALIGNED_SSE, "misaligned_sse")                    /* Misaligned SSE (AMD) */
x86_feature(BX_ISA_LZCNT, "lzcnt")                                      /* LZCNT instruction */
x86_feature(BX_ISA_BMI1, "bmi1")                                        /* BMI1 instruction */
x86_feature(BX_ISA_BMI2, "bmi2")                                        /* BMI2 instruction */
x86_feature(BX_ISA_FMA4, "fma4")                                        /* FMA4 instruction (AMD) */
x86_feature(BX_ISA_XOP, "xop")                                          /* XOP instruction (AMD) */
x86_feature(BX_ISA_TBM, "tbm")                                          /* TBM instruction (AMD) */
x86_feature(BX_ISA_SVM, "svm")                                          /* SVM instruction (AMD) */
x86_feature(BX_ISA_RDRAND, "rdrand")                                    /* RDRAND instruction */
x86_feature(BX_ISA_RDSEED, "rdseed")                                    /* RDSEED instruction */
x86_feature(BX_ISA_ADX, "adx")                                          /* ADCX/ADOX instruction */
x86_feature(BX_ISA_SMAP, "smap")                                        /* SMAP support */
x86_feature(BX_ISA_SHA, "sha")                                          /* SHA instruction */
x86_feature(BX_ISA_SHA512, "sha512")                                    /* SHA-512 instruction */
x86_feature(BX_ISA_GFNI, "gfni")                                        /* GFNI instruction */
x86_feature(BX_ISA_SM3, "sm3")                                          /* SM3 instruction */
x86_feature(BX_ISA_SM4, "sm4")                                          /* SM4 instruction */
x86_feature(BX_ISA_AVX512, "avx512")                                    /* AVX-512 instruction */
x86_feature(BX_ISA_AVX512_DQ, "avx512dq")                               /* AVX-512DQ instruction */
x86_feature(BX_ISA_AVX512_BW, "avx512bw")                               /* AVX-512 Byte/Word instruction */
x86_feature(BX_ISA_AVX512_CD, "avx512cd")                               /* AVX-512 Conflict Detection instruction */
x86_feature(BX_ISA_AVX512_PF, "avx512pf")                               /* AVX-512 Sparse Prefetch instruction */
x86_feature(BX_ISA_AVX512_ER, "avx512er")                               /* AVX-512 Exponential/Reciprocal instruction */
x86_feature(BX_ISA_AVX512_VBMI, "avx512vbmi")                           /* AVX-512 VBMI : Vector Bit Manipulation Instructions */
x86_feature(BX_ISA_AVX512_VBMI2, "avx512vbmi2")                         /* AVX-512 VBMI2 : Vector Bit Manipulation Instructions */
x86_feature(BX_ISA_AVX512_IFMA52, "avx512ifma52")                       /* AVX-512 IFMA52 Instructions */
x86_feature(BX_ISA_AVX512_VPOPCNTDQ, "avx512vpopcnt")                   /* AVX-512 VPOPCNTD/VPOPCNTQ Instructions */
x86_feature(BX_ISA_AVX512_VNNI, "avx512vnni")                           /* AVX-512 VNNI Instructions */
x86_feature(BX_ISA_AVX512_BITALG, "avx512bitalg")                       /* AVX-512 BITALG Instructions */
x86_feature(BX_ISA_AVX512_VP2INTERSECT, "avx512vp2intersect")           /* AVX-512 VP2INTERSECT Instructions */
x86_feature(BX_ISA_AVX512_BF16, "avx512bf16")                           /* AVX-512 BF16 Instructions */
x86_feature(BX_ISA_AVX512_FP16, "avx512fp16")                           /* AVX-512 FP16 Instructions */
x86_feature(BX_ISA_AVX_IFMA, "avx_ifma")                                /* AVX encoded IFMA Instructions */
x86_feature(BX_ISA_AVX_VNNI, "avx_vnni")                                /* AVX encoded VNNI Instructions */
x86_feature(BX_ISA_AVX_VNNI_INT8, "avx_vnni_int8")                      /* AVX encoded VNNI-INT8 Instructions */
x86_feature(BX_ISA_AVX_VNNI_INT16, "avx_vnni_int16")                    /* AVX encoded VNNI-INT16 Instructions */
x86_feature(BX_ISA_AVX_NE_CONVERT, "avx_ne_convert")                    /* AVX-NE-CONVERT Instructions */
x86_feature(BX_ISA_AMX, "amx")                                          /* AMX Instructions */
x86_feature(BX_ISA_AMX_INT8, "amx_int8")                                /* AMX-INT8 Instructions */
x86_feature(BX_ISA_AMX_BF16, "amx_bf16")                                /* AMX-BF16 Instructions */
x86_feature(BX_ISA_AMX_FP16, "amx_fp16")                                /* AMX-FP16 Instructions */
x86_feature(BX_ISA_AMX_COMPLEX, "amx_complex")                          /* AMX-COMPLEX Instructions */
x86_feature(BX_ISA_XAPIC, "xapic")                                      /* XAPIC support */
x86_feature(BX_ISA_X2APIC, "x2apic")                                    /* X2APIC support */
x86_feature(BX_ISA_XAPIC_EXT, "xapicext")                               /* XAPIC Extensions support (AMD) */
x86_feature(BX_ISA_PCID, "pcid")                                        /* PCID support */
x86_feature(BX_ISA_SMEP, "smep")                                        /* SMEP support */
x86_feature(BX_ISA_TSC_ADJUST, "tsc_adjust")                            /* TSC-Adjust MSR */
x86_feature(BX_ISA_TSC_DEADLINE, "tsc_deadline")                        /* TSC-Deadline */
x86_feature(BX_ISA_FOPCODE_DEPRECATION, "fopcode_deprecation")          /* FOPCODE Deprecation - FOPCODE update on unmasked x87 exception only */
x86_feature(BX_ISA_FCS_FDS_DEPRECATION, "fcs_fds_deprecation")          /* FCS/FDS Deprecation */
x86_feature(BX_ISA_FDP_DEPRECATION, "fdp_deprecation")                  /* FDP Deprecation - FDP update on unmasked x87 exception only */
x86_feature(BX_ISA_PKU, "pku")                                          /* User-Mode Protection Keys */
x86_feature(BX_ISA_PKS, "pks")                                          /* Supervisor-Mode Protection Keys */
x86_feature(BX_ISA_UMIP, "umip")                                        /* User-Mode Instructions Prevention */
x86_feature(BX_ISA_RDPID, "rdpid")                                      /* RDPID Support */
x86_feature(BX_ISA_TCE, "tce")                                          /* Translation Cache Extensions (TCE) support (AMD) */
x86_feature(BX_ISA_CLZERO, "clzero")                                    /* CLZERO instruction support (AMD) */
x86_feature(BX_ISA_SCA_MITIGATIONS, "sca_mitigations")                  /* Report SCA Mitigations in CPUID */
x86_feature(BX_ISA_CET, "cet")                                          /* Control Flow Enforcement */
x86_feature(BX_ISA_WRMSRNS, "wrmsrns")                                  /* Non-Serializing version of WRMSR */
x86_feature(BX_ISA_CMPCCXADD, "cmpccxadd")                              /* CMPccXADD instructions */
x86_feature(BX_ISA_SERIALIZE, "serialize")                              /* SERIALIZE instruction */
x86_feature(BX_ISA_LASS, "lass")                                        /* Linear Address Space Separation support */
x86_feature(BX_ISA_LA57, "la57")                                        /* 57-bit Virtual Address and 5-level paging support */
x86_feature(BX_ISA_UINTR, "uintr")                                      /* User Level Interrupts support */
x86_feature(BX_ISA_MOVDIRI, "movdiri")                                  /* MOVDIRI instruction support */
x86_feature(BX_ISA_MOVDIR64B, "movdir64b")                              /* MOVDIR64B instruction support */
x86_feature(BX_ISA_MSRLIST, "msrlist")                                  /* RDMSRLIST/WRMSRLIST instructions support */
