/////////////////////////////////////////////////////////////////////////
// $Id: instrument.h,v 1.8 2002-10-08 21:30:50 sshwarts Exp $
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


// possible types passed to BX_INSTR_TLB_CNTRL()
#define BX_INSTR_MOV_CR3      10
#define BX_INSTR_INVLPG       11
#define BX_INSTR_TASKSWITCH   12

// possible types passed to BX_INSTR_CACHE_CNTRL()
#define BX_INSTR_INVD         20
#define BX_INSTR_WBINVD       21

#define BX_INSTR_IS_CALL  10
#define BX_INSTR_IS_RET   11
#define BX_INSTR_IS_IRET  12
#define BX_INSTR_IS_JMP   13
#define BX_INSTR_IS_INT   14

#define BX_INSTR_PREFETCH_NTA 00
#define BX_INSTR_PREFETCH_T0  01
#define BX_INSTR_PREFETCH_T1  02
#define BX_INSTR_PREFETCH_T2  03


#if BX_INSTRUMENTATION

class bxInstruction_c;

// called from the CPU core

void bx_instr_init(unsigned cpu);
void bx_instr_shutdown(unsigned cpu);
void bx_instr_reset(unsigned cpu);
void bx_instr_new_instruction(unsigned cpu);

void bx_instr_debug_promt();
void bx_instr_start();
void bx_instr_stop();
void bx_instr_print();

void bx_instr_cnear_branch_taken(unsigned cpu, bx_address new_eip);
void bx_instr_cnear_branch_not_taken(unsigned cpu);
void bx_instr_ucnear_branch(unsigned cpu, unsigned what, bx_address new_eip);
void bx_instr_far_branch(unsigned cpu, unsigned what, Bit16u new_cs, bx_address new_eip);

void bx_instr_opcode(unsigned cpu, Bit8u *opcode, unsigned len, Boolean is32);
void bx_instr_fetch_decode_completed(unsigned cpu, const bxInstruction_c *i);

void bx_instr_prefix_as(unsigned cpu);
void bx_instr_prefix_os(unsigned cpu);
void bx_instr_prefix_rep(unsigned cpu);
void bx_instr_prefix_repne(unsigned cpu);
void bx_instr_prefix_lock(unsigned cpu);
void bx_instr_prefix_cs(unsigned cpu);
void bx_instr_prefix_ss(unsigned cpu);
void bx_instr_prefix_ds(unsigned cpu);
void bx_instr_prefix_es(unsigned cpu);
void bx_instr_prefix_fs(unsigned cpu);
void bx_instr_prefix_gs(unsigned cpu);
void bx_instr_prefix_extend8b(unsigned cpu);

void bx_instr_interrupt(unsigned cpu, unsigned vector);
void bx_instr_exception(unsigned cpu, unsigned vector);
void bx_instr_hwinterrupt(unsigned cpu, unsigned vector, Bit16u cs, bx_address eip);

void bx_instr_tlb_cntrl(unsigned cpu, unsigned what, Bit32u newval);
void bx_instr_cache_cntrl(unsigned cpu, unsigned what);

void bx_instr_repeat_iteration(unsigned cpu);

void bx_instr_inp(Bit16u addr, unsigned len);
void bx_instr_outp(Bit16u addr, unsigned len);
void bx_instr_inp2(Bit16u addr, unsigned len, unsigned val);
void bx_instr_outp2(Bit16u addr, unsigned len, unsigned val);

void bx_instr_mem_code(unsigned cpu, bx_address linear, unsigned size);
void bx_instr_mem_data(unsigned cpu, bx_address linear, unsigned size, unsigned rw);

void bx_instr_lin_read(unsigned cpu, bx_address lin, bx_address phy, unsigned len);
void bx_instr_lin_write(unsigned cpu, bx_address lin, bx_address phy, unsigned len);

void bx_instr_phy_write(bx_address addr, unsigned len);
void bx_instr_phy_read(bx_address addr, unsigned len);

/* simulation init, shutdown, reset */
#  define BX_INSTR_INIT(cpu_id)            bx_instr_init(cpu_id)
#  define BX_INSTR_SHUTDOWN(cpu_id)        bx_instr_shutdown(cpu_id)
#  define BX_INSTR_RESET(cpu_id)           bx_instr_reset(cpu_id)
#  define BX_INSTR_NEW_INSTRUCTION(cpu_id) bx_instr_new_instruction(cpu_id)

/* called from command line debugger */
#  define BX_INSTR_DEBUG_PROMPT()          bx_instr_debug_promt()
#  define BX_INSTR_START()                 bx_instr_start()
#  define BX_INSTR_STOP()                  bx_instr_stop()
#  define BX_INSTR_PRINT()                 bx_instr_print()

/* branch resoultion */
#  define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, new_eip)       bx_instr_cnear_branch_taken(cpu_id, new_eip)
#  define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id)   bx_instr_cnear_branch_not_taken(cpu_id)
#  define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, new_eip)      bx_instr_ucnear_branch(cpu_id, what, new_eip)
#  define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip) bx_instr_far_branch(cpu_id, what, new_cs, new_eip)

/* decoding completed */
#  define BX_INSTR_OPCODE(cpu_id, opcode, len, is32) \
                       bx_instr_opcode(cpu_id, opcode, len, is32)
#  define BX_INSTR_FETCH_DECODE_COMPLETED(cpu_id, i) \
                       bx_instr_fetch_decode_completed(cpu_id, i)
     
/* prefix decoded */
#  define BX_INSTR_PREFIX_AS(cpu_id)       bx_instr_prefix_as(cpu_id)
#  define BX_INSTR_PREFIX_OS(cpu_id)       bx_instr_prefix_os(cpu_id)
#  define BX_INSTR_PREFIX_REP(cpu_id)      bx_instr_prefix_rep(cpu_id)
#  define BX_INSTR_PREFIX_REPNE(cpu_id)    bx_instr_prefix_repne(cpu_id)
#  define BX_INSTR_PREFIX_LOCK(cpu_id)     bx_instr_prefix_lock(cpu_id)
#  define BX_INSTR_PREFIX_CS(cpu_id)       bx_instr_prefix_cs(cpu_id)
#  define BX_INSTR_PREFIX_SS(cpu_id)       bx_instr_prefix_ss(cpu_id)
#  define BX_INSTR_PREFIX_DS(cpu_id)       bx_instr_prefix_ds(cpu_id)
#  define BX_INSTR_PREFIX_ES(cpu_id)       bx_instr_prefix_es(cpu_id)
#  define BX_INSTR_PREFIX_FS(cpu_id)       bx_instr_prefix_fs(cpu_id)
#  define BX_INSTR_PREFIX_GS(cpu_id)       bx_instr_prefix_gs(cpu_id)
#  define BX_INSTR_PREFIX_EXTEND8B(cpu_id) bx_instr_prefix_extend8b(cpu_id)

/* exceptional case and interrupt */
#  define BX_INSTR_EXCEPTION(cpu_id, vector)            bx_instr_exception(cpu_id, vector)
#  define BX_INSTR_INTERRUPT(cpu_id, vector)            bx_instr_interrupt(cpu_id, vector)
#  define BX_INSTR_HWINTERRUPT(cpu_id, vector, cs, eip) bx_instr_hwinterrupt(cpu_id, vector, cs, eip)

/* TLB/CACHE control instruction executed */
#  define BX_INSTR_CACHE_CNTRL(cpu_id, what)            bx_instr_cache_cntrl(cpu_id, what)
#  define BX_INSTR_TLB_CNTRL(cpu_id, what, newval)      bx_instr_tlb_cntrl(cpu_id, what, newval)

#  define BX_INSTR_REPEAT_ITERATION(cpu_id)             bx_instr_repeat_iteration(cpu_id)

/* memory access */
#  define BX_INSTR_LIN_READ(cpu_id, lin, phy, len)      bx_instr_lin_read(cpu_id, lin, phy, len)
#  define BX_INSTR_LIN_WRITE(cpu_id, lin, phy, len)     bx_instr_lin_write(cpu_id, lin, phy, len)

#  define BX_INSTR_MEM_CODE(cpu_id, linear, size)       bx_instr_mem_code(cpu_id, linear, size)
#  define BX_INSTR_MEM_DATA(cpu_id, linear, size, rw)   bx_instr_mem_data(cpu_id, linear, size, rw)

/* called from memory object */
#  define BX_INSTR_PHY_WRITE(addr, len)         bx_instr_phy_write(addr, len)
#  define BX_INSTR_PHY_READ(addr, len)          bx_instr_phy_read(addr, len)

/* feedback from device units */
#  define BX_INSTR_INP(addr, len)               bx_instr_inp(addr, len)
#  define BX_INSTR_INP2(addr, len, val)         bx_instr_inp2(addr, len, val)
#  define BX_INSTR_OUTP(addr, len)              bx_instr_outp(addr, len)
#  define BX_INSTR_OUTP2(addr, len, val)        bx_instr_outp2(addr, len, val)

#else   

/* simulation init, shutdown, reset */
#  define BX_INSTR_INIT(cpu_id)
#  define BX_INSTR_SHUTDOWN(cpu_id)
#  define BX_INSTR_RESET(cpu_id)
#  define BX_INSTR_NEW_INSTRUCTION(cpu_id)

/* called from command line debugger */
#  define BX_INSTR_DEBUG_PROMPT()
#  define BX_INSTR_START()
#  define BX_INSTR_STOP()
#  define BX_INSTR_PRINT()

/* branch resoultion */
#  define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, new_eip)
#  define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id)
#  define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, new_eip)
#  define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip)

/* decoding completed */
#  define BX_INSTR_OPCODE(cpu_id, opcode, len, is32) 
#  define BX_INSTR_FETCH_DECODE_COMPLETED(cpu_id, i)
     
/* prefix decoded */
#  define BX_INSTR_PREFIX_AS(cpu_id)
#  define BX_INSTR_PREFIX_OS(cpu_id)
#  define BX_INSTR_PREFIX_REP(cpu_id)
#  define BX_INSTR_PREFIX_REPNE(cpu_id)
#  define BX_INSTR_PREFIX_LOCK(cpu_id)
#  define BX_INSTR_PREFIX_CS(cpu_id)
#  define BX_INSTR_PREFIX_SS(cpu_id)
#  define BX_INSTR_PREFIX_DS(cpu_id)
#  define BX_INSTR_PREFIX_ES(cpu_id)
#  define BX_INSTR_PREFIX_FS(cpu_id)
#  define BX_INSTR_PREFIX_GS(cpu_id)
#  define BX_INSTR_PREFIX_EXTEND8B(cpu_id)

/* exceptional case and interrupt */
#  define BX_INSTR_EXCEPTION(cpu_id, vector)
#  define BX_INSTR_INTERRUPT(cpu_id, vector)
#  define BX_INSTR_HWINTERRUPT(cpu_id, vector, cs, eip)

/* TLB/CACHE control instruction executed */
#  define BX_INSTR_CACHE_CNTRL(cpu_id, what)
#  define BX_INSTR_TLB_CNTRL(cpu_id, what, newval)

#  define BX_INSTR_REPEAT_ITERATION(cpu_id)

/* memory access */
#  define BX_INSTR_LIN_READ(cpu_id, lin, phy, len)
#  define BX_INSTR_LIN_WRITE(cpu_id, lin, phy, len)

#  define BX_INSTR_MEM_CODE(cpu_id, linear, size)      
#  define BX_INSTR_MEM_DATA(cpu_id, linear, size, rw)

/* called from memory object */
#  define BX_INSTR_PHY_WRITE(addr, len)
#  define BX_INSTR_PHY_READ(addr, len)

/* feedback from device units */
#  define BX_INSTR_INP(addr, len)
#  define BX_INSTR_INP2(addr, len, val)
#  define BX_INSTR_OUTP(addr, len)
#  define BX_INSTR_OUTP2(addr, len, val)

#endif  
