/////////////////////////////////////////////////////////////////////////
// $Id: smm.h,v 1.2 2006-10-02 18:50:34 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2006 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <stl at fidonet.org.il>
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
/////////////////////////////////////////////////////////////////////////

#ifndef BX_SMM_H
#define BX_SMM_H

/* SMM feature masks */
#define SMM_IO_INSTRUCTION_RESTART  (0x00010000)
#define SMM_SMBASE_RELOCATION       (0x00020000)

#define SMM_SAVE_STATE_MAP_SIZE 128

#if BX_SUPPORT_X86_64 

// for x86-64 configuration using AMD Athlon 64 512-byte SMM save state map
// revision ID according to QEMU/Bochs BIOS
#define SMM_REVISION_ID (0x00000064 | SMM_SMBASE_RELOCATION)

#define SMRAM_OFFSET_RAX_HI32  0x7ffc
#define SMRAM_OFFSET_RAX_LO32  0x7ff8
#define SMRAM_OFFSET_RCX_HI32  0x7ff4
#define SMRAM_OFFSET_RCX_LO32  0x7ff0
#define SMRAM_OFFSET_RDX_HI32  0x7fec
#define SMRAM_OFFSET_RDX_LO32  0x7fe8
#define SMRAM_OFFSET_RBX_HI32  0x7fe4
#define SMRAM_OFFSET_RBX_LO32  0x7fe0
#define SMRAM_OFFSET_RSP_HI32  0x7fdc
#define SMRAM_OFFSET_RSP_LO32  0x7fd8
#define SMRAM_OFFSET_RBP_HI32  0x7fd4
#define SMRAM_OFFSET_RBP_LO32  0x7fd0
#define SMRAM_OFFSET_RSI_HI32  0x7fcc
#define SMRAM_OFFSET_RSI_LO32  0x7fc8
#define SMRAM_OFFSET_RDI_HI32  0x7fc4
#define SMRAM_OFFSET_RDI_LO32  0x7fc0
#define SMRAM_OFFSET_R8_HI32   0x7fbc
#define SMRAM_OFFSET_R8_LO32   0x7fb8
#define SMRAM_OFFSET_R9_HI32   0x7fb4
#define SMRAM_OFFSET_R9_LO32   0x7fb0
#define SMRAM_OFFSET_R10_HI32  0x7fac
#define SMRAM_OFFSET_R10_LO32  0x7fa8
#define SMRAM_OFFSET_R11_HI32  0x7fa4
#define SMRAM_OFFSET_R11_LO32  0x7fa0
#define SMRAM_OFFSET_R12_HI32  0x7f9c
#define SMRAM_OFFSET_R12_LO32  0x7f98
#define SMRAM_OFFSET_R13_HI32  0x7f94
#define SMRAM_OFFSET_R13_LO32  0x7f90
#define SMRAM_OFFSET_R14_HI32  0x7f8c
#define SMRAM_OFFSET_R14_LO32  0x7f88
#define SMRAM_OFFSET_R15_HI32  0x7f84
#define SMRAM_OFFSET_R15_LO32  0x7f80
#define SMRAM_OFFSET_RIP_HI32  0x7f7c
#define SMRAM_OFFSET_RIP_LO32  0x7f78
// Hi32 part of RFLAGS64       0x7f74    (always zero)
#define SMRAM_OFFSET_RFLAGS32  0x7f70
// Hi32 part of 64-bit DR6     0x7f6c    (always zero)
#define SMRAM_OFFSET_DR6       0x7f68
// Hi32 part of 64-bit DR7     0x7f64    (always zero)
#define SMRAM_OFFSET_DR7       0x7f60
// Hi32 part of 64-bit CR0     0x7f5c    (always zero)
#define SMRAM_OFFSET_CR0       0x7f58
// Hi32 part of 64-bit CR3     0x7f54    (always zerom, 32-bit physical address)
#define SMRAM_OFFSET_CR3       0x7f50
// Hi32 part of 64-bit CR4     0x7f4c    (always zero)
#define SMRAM_OFFSET_CR4       0x7f48
// reserved                    0x7f44
// reserved                    0x7f40
// reserved                    0x7f3c
// reserved                    0x7f38
// reserved                    0x7f34
// reserved                    0x7f30
// reserved                    0x7f2c
// reserved                    0x7f28
// reserved                    0x7f24
// reserved                    0x7f20
// reserved                    0x7f1c
// reserved                    0x7f18
// reserved                    0x7f14
// reserved                    0x7f10
// reserved                    0x7f0c
// reserved                    0x7f08
// reserved                    0x7f04
#define SMRAM_SMBASE_OFFSET    0x7f00
#define SMRAM_SMM_REVISION_ID  0x7efc
// reserved                    0x7ef8
// reserved                    0x7ef4
// reserved                    0x7ef0
// reserved                    0x7eec
// reserved                    0x7ee8
// reserved                    0x7ee4
// reserved                    0x7ee0
// reserved                    0x7edc
// reserved                    0x7ed8
// High part of 64-bit EFER    0x7ed4    (always zero)
#define SMRAM_OFFSET_EFER      0x7ed0
// reserved                    0x7ecc
#define SMRAM_IO_INSTRUCTION_RESTART 0x7ec8
#define SMRAM_AUTOHALT_RESTART       0x7ec8
#define SMRAM_NMI_MASK               0x7ec8
// reserved                    0x7ec4
#define SMRAM_SMM_IO_TRAP      0x7ec0
// reserved                    0x7ebc
// reserved                    0x7eb8
// reserved                    0x7eb4
// reserved                    0x7eb0
// reserved                    0x7eac
// reserved                    0x7ea8
// reserved                    0x7ea4
// reserved                    0x7ea0
#define SMRAM_TR_BASE_HI32     0x7e9c
#define SMRAM_TR_BASE_LO32     0x7e98
#define SMRAM_TR_LIMIT         0x7e94
#define SMRAM_TR_SELECTOR_AR   0x7e90
#define SMRAM_IDTR_BASE_HI32   0x7e8c
#define SMRAM_IDTR_BASE_LO32   0x7e88
#define SMRAM_IDTR_LIMIT       0x7e84
// reserved                    0x7e80
#define SMRAM_LDTR_BASE_HI32   0x7e7c
#define SMRAM_LDTR_BASE_LO32   0x7e78
#define SMRAM_LDTR_LIMIT       0x7e74
#define SMRAM_LDTR_SELECTOR_AR 0x7e70
#define SMRAM_GDTR_BASE_HI32   0x7e6c
#define SMRAM_GDTR_BASE_LO32   0x7e68
#define SMRAM_GDTR_LIMIT       0x7e64
// reserved                    0x7e60
#define SMRAM_GS_BASE_HI32     0x7e5c
#define SMRAM_GS_BASE_LO32     0x7e58
#define SMRAM_GS_LIMIT         0x7e54
#define SMRAM_GS_SELECTOR_AR   0x7e50
#define SMRAM_FS_BASE_HI32     0x7e4c
#define SMRAM_FS_BASE_LO32     0x7e48
#define SMRAM_FS_LIMIT         0x7e44
#define SMRAM_FS_SELECTOR_AR   0x7e40
#define SMRAM_DS_BASE_HI32     0x7e3c
#define SMRAM_DS_BASE_LO32     0x7e38
#define SMRAM_DS_LIMIT         0x7e34
#define SMRAM_DS_SELECTOR_AR   0x7e30
#define SMRAM_SS_BASE_HI32     0x7e2c
#define SMRAM_SS_BASE_LO32     0x7e28
#define SMRAM_SS_LIMIT         0x7e24
#define SMRAM_SS_SELECTOR_AR   0x7e20
#define SMRAM_CS_BASE_HI32     0x7e1c
#define SMRAM_CS_BASE_LO32     0x7e18
#define SMRAM_CS_LIMIT         0x7e14
#define SMRAM_CS_SELECTOR_AR   0x7e10
#define SMRAM_ES_BASE_HI32     0x7e0c
#define SMRAM_ES_BASE_LO32     0x7e08
#define SMRAM_ES_LIMIT         0x7e04
#define SMRAM_ES_SELECTOR_AR   0x7e00

#else  /* BX_SUPPORT_X86_64 == 0 */

// for x86-32 configuration using Intel P6 512-byte SMM save state map
#define SMM_REVISION_ID (0x00000000 | SMM_SMBASE_RELOCATION)

// source for Intel P6 SMM save state map used: www.sandpile.org

#define SMRAM_OFFSET_CR0       0x7ffc
#define SMRAM_OFFSET_CR3       0x7ff8
#define SMRAM_OFFSET_EFLAGS    0x7ff4
#define SMRAM_OFFSET_EIP       0x7ff0
#define SMRAM_OFFSET_EDI       0x7fec
#define SMRAM_OFFSET_ESI       0x7fe8
#define SMRAM_OFFSET_EBP       0x7fe4
#define SMRAM_OFFSET_ESP       0x7fe0
#define SMRAM_OFFSET_EBX       0x7fdc
#define SMRAM_OFFSET_EDX       0x7fd8
#define SMRAM_OFFSET_ECX       0x7fd4
#define SMRAM_OFFSET_EAX       0x7fd0
#define SMRAM_OFFSET_DR6       0x7fcc
#define SMRAM_OFFSET_DR7       0x7fc8
#define SMRAM_TR_SELECTOR      0x7fc4
#define SMRAM_LDTR_SELECTOR    0x7fc0
#define SMRAM_GS_SELECTOR      0x7fbc
#define SMRAM_FS_SELECTOR      0x7fb8
#define SMRAM_DS_SELECTOR      0x7fb4
#define SMRAM_SS_SELECTOR      0x7fb0
#define SMRAM_CS_SELECTOR      0x7fac
#define SMRAM_ES_SELECTOR      0x7fa8
#define SMRAM_SS_BASE          0x7fa4
#define SMRAM_SS_LIMIT         0x7fa0
#define SMRAM_SS_SELECTOR_AR   0x7f9c
#define SMRAM_CS_BASE          0x7f98
#define SMRAM_CS_LIMIT         0x7f94
#define SMRAM_CS_SELECTOR_AR   0x7f90
#define SMRAM_ES_BASE          0x7f8c
#define SMRAM_ES_LIMIT         0x7f88
#define SMRAM_ES_SELECTOR_AR   0x7f84
#define SMRAM_LDTR_BASE        0x7f80
#define SMRAM_LDTR_LIMIT       0x7f7c
#define SMRAM_LDTR_SELECTOR_AR 0x7f78
#define SMRAM_GDTR_BASE        0x7f74
#define SMRAM_GDTR_LIMIT       0x7f70
// reserved                    0x7f6c
// reserved                    0x7f68
#define SMRAM_TR_BASE          0x7f64
#define SMRAM_TR_LIMIT         0x7f60
#define SMRAM_TR_SELECTOR_AR   0x7f5c
#define SMRAM_IDTR_BASE        0x7f58
#define SMRAM_IDTR_LIMIT       0x7f54
// reserved                    0x7f50
#define SMRAM_GS_BASE          0x7f4c
#define SMRAM_GS_LIMIT         0x7f48
#define SMRAM_GS_SELECTOR_AR   0x7f44
#define SMRAM_FS_BASE          0x7f40
#define SMRAM_FS_LIMIT         0x7f3c
#define SMRAM_FS_SELECTOR_AR   0x7f38
#define SMRAM_DS_BASE          0x7f34
#define SMRAM_DS_LIMIT         0x7f30
#define SMRAM_DS_SELECTOR_AR   0x7f2c
// reserved                    0x7f28
// reserved                    0x7f24
// reserved                    0x7f20
// reserved                    0x7f1c
// reserved                    0x7f18
#define SMRAM_OFFSET_CR4       0x7f14
// reserved                    0x7f10    (used for I/O restart)
// reserved                    0x7f0c    (used for I/O restart)
// reserved                    0x7f08    (used for I/O restart)
// reserved                    0x7f04    (used for I/O restart)
#define SMRAM_IO_INSTRUCTION_RESTART 0x7f00
#define SMRAM_AUTOHALT_RESTART       0x7f00
#define SMRAM_SMM_REVISION_ID  0x7efc
#define SMRAM_SMBASE_OFFSET    0x7ef8
// reserved                    0x7ef4
// reserved                    0x7ef0
// reserved                    0x7eec
// reserved                    0x7ee8
// reserved                    0x7ee4
// reserved                    0x7ee0
// reserved                    0x7edc
// reserved                    0x7ed8
// reserved                    0x7ed4
// reserved                    0x7ed0
// reserved                    0x7ecc
// reserved                    0x7ec8
// reserved                    0x7ec4
// reserved                    0x7ec0
// reserved                    0x7ebc
// reserved                    0x7eb8
// reserved                    0x7eb4
// reserved                    0x7eb0
// reserved                    0x7eac
// reserved                    0x7ea8
// reserved                    0x7ea4
// reserved                    0x7ea0
// reserved                    0x7e9c
// reserved                    0x7e98
// reserved                    0x7e94
// reserved                    0x7e90
// reserved                    0x7e8c
// reserved                    0x7e88
// reserved                    0x7e84
// reserved                    0x7e80
// reserved                    0x7e7c
// reserved                    0x7e78
// reserved                    0x7e74
// reserved                    0x7e70
// reserved                    0x7e6c
// reserved                    0x7e68
// reserved                    0x7e64
// reserved                    0x7e60
// reserved                    0x7e5c
// reserved                    0x7e58
// reserved                    0x7e54
// reserved                    0x7e50
// reserved                    0x7e4c
// reserved                    0x7e48
// reserved                    0x7e44
// reserved                    0x7e40
// reserved                    0x7e3c
// reserved                    0x7e38
// reserved                    0x7e34
// reserved                    0x7e30
// reserved                    0x7e2c
// reserved                    0x7e28
// reserved                    0x7e24
// reserved                    0x7e20
// reserved                    0x7e1c
// reserved                    0x7e18
// reserved                    0x7e14
// reserved                    0x7e10
// reserved                    0x7e0c
// reserved                    0x7e08
// reserved                    0x7e04
// reserved                    0x7e00

#endif /* BX_SUPPORT_X86_64 */

#endif
