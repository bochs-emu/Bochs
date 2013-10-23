/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2006-2012 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

#if BX_INSTRUMENTATION

class bxInstruction_c;

// define if you want to store instruction opcode bytes in bxInstruction_c
//#define BX_INSTR_STORE_OPCODE_BYTES

void bx_instr_init_env(void);
void bx_instr_exit_env(void);

// called from the CPU core

void bx_instr_initialize(unsigned cpu);
void bx_instr_exit(unsigned cpu);
void bx_instr_reset(unsigned cpu, unsigned type);
void bx_instr_hlt(unsigned cpu);
void bx_instr_mwait(unsigned cpu, bx_phy_address addr, unsigned len, Bit32u flags);

void bx_instr_debug_promt();
void bx_instr_debug_cmd(const char *cmd);

void bx_instr_cnear_branch_taken(unsigned cpu, bx_address branch_eip, bx_address new_eip);
void bx_instr_cnear_branch_not_taken(unsigned cpu, bx_address branch_eip);
void bx_instr_ucnear_branch(unsigned cpu, unsigned what, bx_address branch_eip, bx_address new_eip);
void bx_instr_far_branch(unsigned cpu, unsigned what, Bit16u new_cs, bx_address new_eip);

void bx_instr_opcode(unsigned cpu, bxInstruction_c *i, const Bit8u *opcode, unsigned len, bx_bool is32, bx_bool is64);

void bx_instr_interrupt(unsigned cpu, unsigned vector);
void bx_instr_exception(unsigned cpu, unsigned vector, unsigned error_code);
void bx_instr_hwinterrupt(unsigned cpu, unsigned vector, Bit16u cs, bx_address eip);

void bx_instr_tlb_cntrl(unsigned cpu, unsigned what, bx_phy_address new_cr3);
void bx_instr_cache_cntrl(unsigned cpu, unsigned what);
void bx_instr_prefetch_hint(unsigned cpu, unsigned what, unsigned seg, bx_address offset);
void bx_instr_clflush(unsigned cpu, bx_address laddr, bx_phy_address paddr);

void bx_instr_before_execution(unsigned cpu, bxInstruction_c *i);
void bx_instr_after_execution(unsigned cpu, bxInstruction_c *i);
void bx_instr_repeat_iteration(unsigned cpu, bxInstruction_c *i);

void bx_instr_inp(Bit16u addr, unsigned len);
void bx_instr_inp2(Bit16u addr, unsigned len, unsigned val);
void bx_instr_outp(Bit16u addr, unsigned len, unsigned val);

void bx_instr_lin_access(unsigned cpu, bx_address lin, bx_address phy, unsigned len, unsigned rw);
void bx_instr_phy_access(unsigned cpu, bx_address phy, unsigned len, unsigned rw);

void bx_instr_wrmsr(unsigned cpu, unsigned addr, Bit64u value);

void bx_instr_vmexit(unsigned cpu, Bit32u reason, Bit64u qualification);

/* initialization/deinitialization of instrumentalization*/
#define BX_INSTR_INIT_ENV() bx_instr_init_env()
#define BX_INSTR_EXIT_ENV() bx_instr_exit_env()

/* simulation init, shutdown, reset */
#define BX_INSTR_INITIALIZE(cpu_id)      bx_instr_initialize(cpu_id)
#define BX_INSTR_EXIT(cpu_id)            bx_instr_exit(cpu_id)
#define BX_INSTR_RESET(cpu_id, type)     bx_instr_reset(cpu_id, type)
#define BX_INSTR_HLT(cpu_id)             bx_instr_hlt(cpu_id)

#define BX_INSTR_MWAIT(cpu_id, addr, len, flags) \
                       bx_instr_mwait(cpu_id, addr, len, flags)

/* called from command line debugger */
#define BX_INSTR_DEBUG_PROMPT()          bx_instr_debug_promt()
#define BX_INSTR_DEBUG_CMD(cmd)          bx_instr_debug_cmd(cmd)

/* branch resolution */
#define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, branch_eip, new_eip) bx_instr_cnear_branch_taken(cpu_id, branch_eip, new_eip)
#define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id, branch_eip) bx_instr_cnear_branch_not_taken(cpu_id, branch_eip)
#define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, branch_eip, new_eip) bx_instr_ucnear_branch(cpu_id, what, branch_eip, new_eip)
#define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip) bx_instr_far_branch(cpu_id, what, new_cs, new_eip)

/* decoding completed */
#define BX_INSTR_OPCODE(cpu_id, i, opcode, len, is32, is64) \
                       bx_instr_opcode(cpu_id, i, opcode, len, is32, is64)

/* exceptional case and interrupt */
#define BX_INSTR_EXCEPTION(cpu_id, vector, error_code) \
                bx_instr_exception(cpu_id, vector, error_code)

#define BX_INSTR_INTERRUPT(cpu_id, vector) bx_instr_interrupt(cpu_id, vector)
#define BX_INSTR_HWINTERRUPT(cpu_id, vector, cs, eip) bx_instr_hwinterrupt(cpu_id, vector, cs, eip)

/* TLB/CACHE control instruction executed */
#define BX_INSTR_CLFLUSH(cpu_id, laddr, paddr)    bx_instr_clflush(cpu_id, laddr, paddr)
#define BX_INSTR_CACHE_CNTRL(cpu_id, what)        bx_instr_cache_cntrl(cpu_id, what)
#define BX_INSTR_TLB_CNTRL(cpu_id, what, new_cr3) bx_instr_tlb_cntrl(cpu_id, what, new_cr3)
#define BX_INSTR_PREFETCH_HINT(cpu_id, what, seg, offset) \
                       bx_instr_prefetch_hint(cpu_id, what, seg, offset)

/* execution */
#define BX_INSTR_BEFORE_EXECUTION(cpu_id, i)  bx_instr_before_execution(cpu_id, i)
#define BX_INSTR_AFTER_EXECUTION(cpu_id, i)   bx_instr_after_execution(cpu_id, i)
#define BX_INSTR_REPEAT_ITERATION(cpu_id, i)  bx_instr_repeat_iteration(cpu_id, i)

/* linear memory access */
#define BX_INSTR_LIN_ACCESS(cpu_id, lin, phy, len, rw)  bx_instr_lin_access(cpu_id, lin, phy, len, rw)

/* physical memory access */
#define BX_INSTR_PHY_ACCESS(cpu_id, phy, len, rw)  bx_instr_phy_access(cpu_id, phy, len, rw)

/* feedback from device units */
#define BX_INSTR_INP(addr, len)               bx_instr_inp(addr, len)
#define BX_INSTR_INP2(addr, len, val)         bx_instr_inp2(addr, len, val)
#define BX_INSTR_OUTP(addr, len, val)         bx_instr_outp(addr, len, val)

/* wrmsr callback */
#define BX_INSTR_WRMSR(cpu_id, addr, value)   bx_instr_wrmsr(cpu_id, addr, value)

/* vmexit callback */
#define BX_INSTR_VMEXIT(cpu_id, reason, qualification) bx_instr_vmexit(cpu_id, reason, qualification)

#else

/* initialization/deinitialization of instrumentalization */
#define BX_INSTR_INIT_ENV()
#define BX_INSTR_EXIT_ENV()

/* simulation init, shutdown, reset */
#define BX_INSTR_INITIALIZE(cpu_id)
#define BX_INSTR_EXIT(cpu_id)
#define BX_INSTR_RESET(cpu_id, type)
#define BX_INSTR_HLT(cpu_id)
#define BX_INSTR_MWAIT(cpu_id, addr, len, flags)

/* called from command line debugger */
#define BX_INSTR_DEBUG_PROMPT()
#define BX_INSTR_DEBUG_CMD(cmd)

/* branch resolution */
#define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, branch_eip, new_eip)
#define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id, branch_eip)
#define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, branch_eip, new_eip)
#define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip)

/* decoding completed */
#define BX_INSTR_OPCODE(cpu_id, i, opcode, len, is32, is64)

/* exceptional case and interrupt */
#define BX_INSTR_EXCEPTION(cpu_id, vector, error_code)
#define BX_INSTR_INTERRUPT(cpu_id, vector)
#define BX_INSTR_HWINTERRUPT(cpu_id, vector, cs, eip)

/* TLB/CACHE control instruction executed */
#define BX_INSTR_CLFLUSH(cpu_id, laddr, paddr)
#define BX_INSTR_CACHE_CNTRL(cpu_id, what)
#define BX_INSTR_TLB_CNTRL(cpu_id, what, new_cr3)
#define BX_INSTR_PREFETCH_HINT(cpu_id, what, seg, offset)

/* execution */
#define BX_INSTR_BEFORE_EXECUTION(cpu_id, i)
#define BX_INSTR_AFTER_EXECUTION(cpu_id, i)
#define BX_INSTR_REPEAT_ITERATION(cpu_id, i)

/* linear memory access */
#define BX_INSTR_LIN_ACCESS(cpu_id, lin, phy, len, rw)

/* physical memory access */
#define BX_INSTR_PHY_ACCESS(cpu_id, phy, len, rw)

/* feedback from device units */
#define BX_INSTR_INP(addr, len)
#define BX_INSTR_INP2(addr, len, val)
#define BX_INSTR_OUTP(addr, len, val)

/* wrmsr callback */
#define BX_INSTR_WRMSR(cpu_id, addr, value)

/* vmexit callback */
#define BX_INSTR_VMEXIT(cpu_id, reason, qualification)

#endif
