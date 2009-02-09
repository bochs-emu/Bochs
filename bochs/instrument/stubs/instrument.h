/////////////////////////////////////////////////////////////////////////
// $Id: instrument.h,v 1.40 2009-02-09 10:35:55 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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


// possible types passed to BX_INSTR_TLB_CNTRL()
#define BX_INSTR_MOV_CR3      10
#define BX_INSTR_INVLPG       11
#define BX_INSTR_TASKSWITCH   12

// possible types passed to BX_INSTR_CACHE_CNTRL()
#define BX_INSTR_INVD         20
#define BX_INSTR_WBINVD       21

// possible types passed to BX_INSTR_FAR_BRANCH()
#define BX_INSTR_IS_CALL      10
#define BX_INSTR_IS_RET       11
#define BX_INSTR_IS_IRET      12
#define BX_INSTR_IS_JMP       13
#define BX_INSTR_IS_INT       14
#define BX_INSTR_IS_SYSCALL   15
#define BX_INSTR_IS_SYSRET    16
#define BX_INSTR_IS_SYSENTER  17
#define BX_INSTR_IS_SYSEXIT   18

// possible types passed to BX_INSTR_PREFETCH_HINT()
#define BX_INSTR_PREFETCH_NTA 0
#define BX_INSTR_PREFETCH_T0  1
#define BX_INSTR_PREFETCH_T1  2
#define BX_INSTR_PREFETCH_T2  3


#if BX_INSTRUMENTATION

class bxInstruction_c;

void bx_instr_init_env(void);
void bx_instr_exit_env(void);

// called from the CPU core

void bx_instr_initialize(unsigned cpu);
void bx_instr_exit(unsigned cpu);
void bx_instr_reset(unsigned cpu, unsigned type);
void bx_instr_hlt(unsigned cpu);
void bx_instr_mwait(unsigned cpu, bx_phy_address addr, unsigned len, Bit32u flags);
void bx_instr_new_instruction(unsigned cpu);

void bx_instr_debug_promt();
void bx_instr_start();
void bx_instr_stop();
void bx_instr_print();

void bx_instr_cnear_branch_taken(unsigned cpu, bx_address new_eip);
void bx_instr_cnear_branch_not_taken(unsigned cpu);
void bx_instr_ucnear_branch(unsigned cpu, unsigned what, bx_address new_eip);
void bx_instr_far_branch(unsigned cpu, unsigned what, Bit16u new_cs, bx_address new_eip);

void bx_instr_opcode(unsigned cpu, const Bit8u *opcode, unsigned len, bx_bool is32, bx_bool is64);

void bx_instr_interrupt(unsigned cpu, unsigned vector);
void bx_instr_exception(unsigned cpu, unsigned vector, unsigned error_code);
void bx_instr_hwinterrupt(unsigned cpu, unsigned vector, Bit16u cs, bx_address eip);

void bx_instr_tlb_cntrl(unsigned cpu, unsigned what, bx_phy_address new_cr3);
void bx_instr_cache_cntrl(unsigned cpu, unsigned what);
void bx_instr_prefetch_hint(unsigned cpu, unsigned what, unsigned seg, bx_address offset);

void bx_instr_before_execution(unsigned cpu, bxInstruction_c *i);
void bx_instr_after_execution(unsigned cpu, bxInstruction_c *i);
void bx_instr_repeat_iteration(unsigned cpu, bxInstruction_c *i);

void bx_instr_inp(Bit16u addr, unsigned len);
void bx_instr_inp2(Bit16u addr, unsigned len, unsigned val);
void bx_instr_outp(Bit16u addr, unsigned len);

void bx_instr_mem_data_access(unsigned cpu, unsigned seg, bx_address offset, unsigned len, unsigned rw);
void bx_instr_lin_access(unsigned cpu, bx_address lin, bx_address phy, unsigned len, unsigned rw);

void bx_instr_phy_write(unsigned cpu, bx_address addr, unsigned len);
void bx_instr_phy_read(unsigned cpu, bx_address addr, unsigned len);

void bx_instr_wrmsr(unsigned cpu, unsigned addr, Bit64u value);

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

#define BX_INSTR_NEW_INSTRUCTION(cpu_id) bx_instr_new_instruction(cpu_id)

/* called from command line debugger */
#define BX_INSTR_DEBUG_PROMPT()          bx_instr_debug_promt()
#define BX_INSTR_START()                 bx_instr_start()
#define BX_INSTR_STOP()                  bx_instr_stop()
#define BX_INSTR_PRINT()                 bx_instr_print()

/* branch resoultion */
#define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, new_eip)       bx_instr_cnear_branch_taken(cpu_id, new_eip)
#define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id)   bx_instr_cnear_branch_not_taken(cpu_id)
#define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, new_eip)      bx_instr_ucnear_branch(cpu_id, what, new_eip)
#define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip) bx_instr_far_branch(cpu_id, what, new_cs, new_eip)

/* decoding completed */
#define BX_INSTR_OPCODE(cpu_id, opcode, len, is32, is64) \
                       bx_instr_opcode(cpu_id, opcode, len, is32, is64)

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

/* memory access */
#define BX_INSTR_LIN_ACCESS(cpu_id, lin, phy, len, rw)  bx_instr_lin_access(cpu_id, lin, phy, len, rw)

/* memory access */
#define BX_INSTR_MEM_DATA_ACCESS(cpu_id, seg, offset, len, rw) bx_instr_mem_data_access(cpu_id, seg, offset, len, rw)

/* called from memory object */
#define BX_INSTR_PHY_WRITE(cpu_id, addr, len) bx_instr_phy_write(cpu_id, addr, len)
#define BX_INSTR_PHY_READ(cpu_id, addr, len)  bx_instr_phy_read(cpu_id, addr, len)

/* feedback from device units */
#define BX_INSTR_INP(addr, len)               bx_instr_inp(addr, len)
#define BX_INSTR_INP2(addr, len, val)         bx_instr_inp2(addr, len, val)
#define BX_INSTR_OUTP(addr, len, val)         bx_instr_outp(addr, len, val)

/* wrmsr callback */
#define BX_INSTR_WRMSR(cpu_id, addr, value)   bx_instr_wrmsr(cpu_id, addr, value)

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
#define BX_INSTR_NEW_INSTRUCTION(cpu_id)

/* called from command line debugger */
#define BX_INSTR_DEBUG_PROMPT()
#define BX_INSTR_START()
#define BX_INSTR_STOP()
#define BX_INSTR_PRINT()

/* branch resoultion */
#define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, new_eip)
#define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id)
#define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, new_eip)
#define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip)

/* decoding completed */
#define BX_INSTR_OPCODE(cpu_id, opcode, len, is32, is64)

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

/* memory access */
#define BX_INSTR_LIN_ACCESS(cpu_id, lin, phy, len, rw)

/* memory access */
#define BX_INSTR_MEM_DATA_ACCESS(cpu_id, seg, offset, len, rw)

/* called from memory object */
#define BX_INSTR_PHY_WRITE(cpu_id, addr, len)
#define BX_INSTR_PHY_READ(cpu_id, addr, len)

/* feedback from device units */
#define BX_INSTR_INP(addr, len)
#define BX_INSTR_INP2(addr, len, val)
#define BX_INSTR_OUTP(addr, len, val)

/* wrmsr callback */
#define BX_INSTR_WRMSR(cpu_id, addr, value)

#endif
