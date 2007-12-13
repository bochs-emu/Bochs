/////////////////////////////////////////////////////////////////////////
// $Id: instrument.h,v 1.17 2007-12-13 17:16:21 sshwarts Exp $
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
void bx_instr_reset(unsigned cpu);
void bx_instr_new_instruction(unsigned cpu);

void bx_instr_cnear_branch_taken(unsigned cpu, bx_address new_eip);
void bx_instr_cnear_branch_not_taken(unsigned cpu);
void bx_instr_ucnear_branch(unsigned cpu, unsigned what, bx_address new_eip);
void bx_instr_far_branch(unsigned cpu, unsigned what, Bit16u new_cs, bx_address new_eip);

void bx_instr_opcode(unsigned cpu, Bit8u *opcode, unsigned len, bx_bool is32, bx_bool is64);
void bx_instr_fetch_decode_completed(unsigned cpu, const bxInstruction_c *i);

void bx_instr_prefix(unsigned cpu, Bit8u prefix);

void bx_instr_interrupt(unsigned cpu, unsigned vector);
void bx_instr_exception(unsigned cpu, unsigned vector);
void bx_instr_hwinterrupt(unsigned cpu, unsigned vector, Bit16u cs, bx_address eip);

void bx_instr_mem_data(unsigned cpu, bx_address lin, unsigned size, unsigned rw);

/* simulation init, shutdown, reset */
#  define BX_INSTR_INIT(cpu_id)            bx_instr_init(cpu_id)
#  define BX_INSTR_SHUTDOWN(cpu_id)
#  define BX_INSTR_RESET(cpu_id)           bx_instr_reset(cpu_id)
#  define BX_INSTR_HLT(cpu_id)
#  define BX_INSTR_NEW_INSTRUCTION(cpu_id) bx_instr_new_instruction(cpu_id)

/* called from command line debugger */
#  define BX_INSTR_DEBUG_PROMPT()        
#  define BX_INSTR_START()               
#  define BX_INSTR_STOP()                
#  define BX_INSTR_PRINT()               

/* branch resoultion */
#  define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, new_eip)       bx_instr_cnear_branch_taken(cpu_id, new_eip)
#  define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id)   bx_instr_cnear_branch_not_taken(cpu_id)
#  define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, new_eip)      bx_instr_ucnear_branch(cpu_id, what, new_eip)
#  define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip) bx_instr_far_branch(cpu_id, what, new_cs, new_eip)

/* decoding completed */
#  define BX_INSTR_OPCODE(cpu_id, opcode, len, is32, is64) \
                       bx_instr_opcode(cpu_id, opcode, len, is32, is64)
#  define BX_INSTR_FETCH_DECODE_COMPLETED(cpu_id, i) \
                       bx_instr_fetch_decode_completed(cpu_id, i)
     
/* prefix byte decoded */
#  define BX_INSTR_PREFIX(cpu_id, prefix)  bx_instr_prefix(cpu_id, prefix)

/* exceptional case and interrupt */
#  define BX_INSTR_EXCEPTION(cpu_id, vector)            bx_instr_exception(cpu_id, vector)
#  define BX_INSTR_INTERRUPT(cpu_id, vector)            bx_instr_interrupt(cpu_id, vector)
#  define BX_INSTR_HWINTERRUPT(cpu_id, vector, cs, eip) bx_instr_hwinterrupt(cpu_id, vector, cs, eip)

/* TLB/CACHE control instruction executed */
#  define BX_INSTR_CACHE_CNTRL(cpu_id, what)
#  define BX_INSTR_TLB_CNTRL(cpu_id, what, newval)
#  define BX_INSTR_PREFETCH_HINT(cpu_id, what, seg, offset)

/* execution */
#  define BX_INSTR_BEFORE_EXECUTION(cpu_id, i)
#  define BX_INSTR_AFTER_EXECUTION(cpu_id, i)
#  define BX_INSTR_REPEAT_ITERATION(cpu_id, i)

/* memory access */
#  define BX_INSTR_LIN_READ(cpu_id, lin, phy, len, rw)
#  define BX_INSTR_LIN_WRITE(cpu_id, lin, phy, len)

#  define BX_INSTR_MEM_CODE(cpu_id, linear, size)
#  define BX_INSTR_MEM_DATA(cpu_id, linear, size, rw)  bx_instr_mem_data(cpu_id, linear, size, rw)

/* called from memory object */
#  define BX_INSTR_PHY_WRITE(cpu_id, addr, len)
#  define BX_INSTR_PHY_READ(cpu_id, addr, len)

/* feedback from device units */
#  define BX_INSTR_INP(addr, len)
#  define BX_INSTR_INP2(addr, len, val)
#  define BX_INSTR_OUTP(addr, len)
#  define BX_INSTR_OUTP2(addr, len, val)

/* wrmsr callback */
#  define BX_INSTR_WRMSR(cpu_id, addr, value)

#else   

/* simulation init, shutdown, reset */
#  define BX_INSTR_INIT(cpu_id)
#  define BX_INSTR_SHUTDOWN(cpu_id)
#  define BX_INSTR_RESET(cpu_id)
#  define BX_INSTR_HLT(cpu_id)
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
#  define BX_INSTR_OPCODE(cpu_id, opcode, len, is32, is64)
#  define BX_INSTR_FETCH_DECODE_COMPLETED(cpu_id, i)
     
/* prefix byte decoded */
#  define BX_INSTR_PREFIX(cpu_id, prefix)

/* exceptional case and interrupt */
#  define BX_INSTR_EXCEPTION(cpu_id, vector)
#  define BX_INSTR_INTERRUPT(cpu_id, vector)
#  define BX_INSTR_HWINTERRUPT(cpu_id, vector, cs, eip)

/* TLB/CACHE control instruction executed */
#  define BX_INSTR_CACHE_CNTRL(cpu_id, what)
#  define BX_INSTR_TLB_CNTRL(cpu_id, what, newval)
#  define BX_INSTR_PREFETCH_HINT(cpu_id, what, seg, offset)

/* execution */
#  define BX_INSTR_BEFORE_EXECUTION(cpu_id, i)
#  define BX_INSTR_AFTER_EXECUTION(cpu_id, i)
#  define BX_INSTR_REPEAT_ITERATION(cpu_id, i)

/* memory access */
#  define BX_INSTR_LIN_READ(cpu_id, lin, phy, len, rw)
#  define BX_INSTR_LIN_WRITE(cpu_id, lin, phy, len)

#  define BX_INSTR_MEM_CODE(cpu_id, linear, size)      
#  define BX_INSTR_MEM_DATA(cpu_id, linear, size, rw)

/* called from memory object */
#  define BX_INSTR_PHY_WRITE(cpu_id, addr, len)
#  define BX_INSTR_PHY_READ(cpu_id, addr, len)

/* feedback from device units */
#  define BX_INSTR_INP(addr, len)
#  define BX_INSTR_INP2(addr, len, val)
#  define BX_INSTR_OUTP(addr, len)
#  define BX_INSTR_OUTP2(addr, len, val)

/* wrmsr callback */
#  define BX_INSTR_WRMSR(cpu_id, addr, value)

#endif  
