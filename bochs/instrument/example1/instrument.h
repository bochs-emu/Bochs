/////////////////////////////////////////////////////////////////////////
// $Id: instrument.h,v 1.13 2005-11-14 18:25:41 sshwarts Exp $
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

// maximum size of an instruction
#define MAX_OPCODE_SIZE 16

// maximum physical addresses an instruction can generate
#define MAX_DATA_ACCESSES 1024

class bxInstrumentation {
public:

  bx_bool  valid;        // is current instruction valid
  bx_bool active;        // is active

  unsigned cpu_id;

  /* decoding */
  unsigned opcode_size;
  unsigned nprefixes;
  Bit8u    opcode[MAX_OPCODE_SIZE];
  bx_bool  is32, is64;

  /* memory accesses */
  unsigned num_data_accesses;
  struct {
    bx_address laddr; // linear address
    bx_address paddr; // physical address
    unsigned op;      // BX_READ, BX_WRITE or BX_RW
    unsigned size;    // 1 .. 8
  } data_access[MAX_DATA_ACCESSES];

  /* branch resolution and target */
  bx_bool is_branch;
  bx_bool is_taken;
  bx_address target_linear;

public:
  bxInstrumentation(): valid(0), active(0) {}

  void set_cpu_id(unsigned cpu) { cpu_id = cpu; }
 
  void activate() { active = 1; }
  void deactivate() { active = 0; }
  bx_bool toggle_active() { active = !active; }
  bx_bool is_active() const { return active; } 

  void bx_instr_reset();
  void bx_instr_new_instruction();

  void bx_instr_cnear_branch_taken(bx_address new_eip);
  void bx_instr_cnear_branch_not_taken();
  void bx_instr_ucnear_branch(unsigned what, bx_address new_eip);
  void bx_instr_far_branch(unsigned what, Bit16u new_cs, bx_address new_eip);

  void bx_instr_opcode(Bit8u *opcode, unsigned len, bx_bool is32, bx_bool is64);
  void bx_instr_fetch_decode_completed(const bxInstruction_c *i);

  void bx_instr_prefix(Bit8u prefix);

  void bx_instr_interrupt(unsigned vector);
  void bx_instr_exception(unsigned vector);
  void bx_instr_hwinterrupt(unsigned vector, Bit16u cs, bx_address eip);

  void bx_instr_mem_data(bx_address linear, unsigned size, unsigned rw);

private:
  void branch_taken(bx_address new_eip);
};

extern bxInstrumentation icpu[BX_SMP_PROCESSORS];

/* simulation init, shutdown, reset */
#  define BX_INSTR_INIT(cpu_id)	           icpu[cpu_id].set_cpu_id(cpu_id)
#  define BX_INSTR_SHUTDOWN(cpu_id)
#  define BX_INSTR_RESET(cpu_id)           icpu[cpu_id].bx_instr_reset()
#  define BX_INSTR_HLT(cpu_id)
#  define BX_INSTR_NEW_INSTRUCTION(cpu_id) icpu[cpu_id].bx_instr_new_instruction()

/* called from command line debugger */
#  define BX_INSTR_DEBUG_PROMPT()        
#  define BX_INSTR_START()               
#  define BX_INSTR_STOP()                
#  define BX_INSTR_PRINT()               

/* branch resoultion */
#  define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, new_eip)       icpu[cpu_id].bx_instr_cnear_branch_taken(new_eip)
#  define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id)   icpu[cpu_id].bx_instr_cnear_branch_not_taken()
#  define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, new_eip)      icpu[cpu_id].bx_instr_ucnear_branch(what, new_eip)
#  define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip) icpu[cpu_id].bx_instr_far_branch(what, new_cs, new_eip)

/* decoding completed */
#  define BX_INSTR_OPCODE(cpu_id, opcode, len, is32, is64) \
                       icpu[cpu_id].bx_instr_opcode(opcode, len, is32, is64)
#  define BX_INSTR_FETCH_DECODE_COMPLETED(cpu_id, i) \
                       icpu[cpu_id].bx_instr_fetch_decode_completed(i)
     
/* prefix byte decoded */
#  define BX_INSTR_PREFIX(cpu_id, prefix)  icpu[cpu_id].bx_instr_prefix(prefix)

/* exceptional case and interrupt */
#  define BX_INSTR_EXCEPTION(cpu_id, vector)            icpu[cpu_id].bx_instr_exception(vector)
#  define BX_INSTR_INTERRUPT(cpu_id, vector)            icpu[cpu_id].bx_instr_interrupt(vector)
#  define BX_INSTR_HWINTERRUPT(cpu_id, vector, cs, eip) icpu[cpu_id].bx_instr_hwinterrupt(vector, cs, eip)

/* TLB/CACHE control instruction executed */
#  define BX_INSTR_CACHE_CNTRL(cpu_id, what)
#  define BX_INSTR_TLB_CNTRL(cpu_id, what, newval)
#  define BX_INSTR_PREFETCH_HINT(cpu_id, what, seg, offset)

/* execution */
#  define BX_INSTR_BEFORE_EXECUTION(cpu_id, i)
#  define BX_INSTR_AFTER_EXECUTION(cpu_id, i)
#  define BX_INSTR_REPEAT_ITERATION(cpu_id, i)

/* memory access */
#  define BX_INSTR_LIN_READ(cpu_id, lin, phy, len)
#  define BX_INSTR_LIN_WRITE(cpu_id, lin, phy, len)

#  define BX_INSTR_MEM_CODE(cpu_id, linear, size)
#  define BX_INSTR_MEM_DATA(cpu_id, linear, size, rw)   icpu[cpu_id].bx_instr_mem_data(linear, size, rw)

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
#  define BX_INSTR_LIN_READ(cpu_id, lin, phy, len)
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
