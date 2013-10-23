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
#define BX_INSTR_STORE_OPCODE_BYTES

void bx_instr_init_env(void);
void bx_instr_exit_env(void);
void bx_instr_initialize(unsigned cpu);

// maximum size of an instruction
#define MAX_OPCODE_LENGTH 16

// maximum physical addresses an instruction can generate
#define MAX_DATA_ACCESSES 1024

class bxInstrumentation {
public:

  bx_bool ready;          // is current instruction ready to be printed
  bx_bool active;

  unsigned cpu_id;

  /* decoding */
  unsigned opcode_length;
  Bit8u    opcode[MAX_OPCODE_LENGTH];
  bx_bool  is32, is64;

  /* memory accesses */
  unsigned num_data_accesses;
  struct {
    bx_address laddr;     // linear address
    bx_phy_address paddr; // physical address
    unsigned rw;          // BX_READ, BX_WRITE or BX_RW
    unsigned size;        // 1 .. 32
  } data_access[MAX_DATA_ACCESSES];

  /* branch resolution and target */
  bx_bool is_branch;
  bx_bool is_taken;
  bx_address target_linear;

public:
  bxInstrumentation(): ready(0), active(0) {}

  void set_cpu_id(unsigned cpu) { cpu_id = cpu; }

  void activate() { active = 1; }
  void deactivate() { active = 0; }
  void toggle_active() { active = !active; }
  bx_bool is_active() const { return active; }

  void bx_instr_reset(unsigned type);

  void bx_instr_cnear_branch_taken(bx_address branch_eip, bx_address new_eip);
  void bx_instr_cnear_branch_not_taken(bx_address branch_eip);
  void bx_instr_ucnear_branch(unsigned what, bx_address branch_eip, bx_address new_eip);
  void bx_instr_far_branch(unsigned what, Bit16u new_cs, bx_address new_eip);

  void bx_instr_before_execution(bxInstruction_c *i);
  void bx_instr_after_execution(bxInstruction_c *i);

  void bx_instr_interrupt(unsigned vector);
  void bx_instr_exception(unsigned vector, unsigned error_code);
  void bx_instr_hwinterrupt(unsigned vector, Bit16u cs, bx_address eip);

  void bx_instr_lin_access(bx_address lin, bx_phy_address phy, unsigned len, unsigned rw);

private:
  void branch_taken(bx_address new_eip);

  void bx_print_instruction(void);
};

void bx_instr_init(unsigned cpu);

extern bxInstrumentation *icpu;

/* initialization/deinitialization of instrumentalization*/
#define BX_INSTR_INIT_ENV() bx_instr_init_env()
#define BX_INSTR_EXIT_ENV() bx_instr_exit_env()

/* simulation init, shutdown, reset */
#define BX_INSTR_INITIALIZE(cpu_id)	 bx_instr_initialize(cpu_id)
#define BX_INSTR_EXIT(cpu_id)
#define BX_INSTR_RESET(cpu_id, type)     icpu[cpu_id].bx_instr_reset(type)
#define BX_INSTR_HLT(cpu_id)
#define BX_INSTR_MWAIT(cpu_id, addr, len, flags)

/* called from command line debugger */
#define BX_INSTR_DEBUG_PROMPT()
#define BX_INSTR_DEBUG_CMD(cmd)

/* branch resolution */
#define BX_INSTR_CNEAR_BRANCH_TAKEN(cpu_id, branch_eip, new_eip) icpu[cpu_id].bx_instr_cnear_branch_taken(branch_eip, new_eip)
#define BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(cpu_id, branch_eip) icpu[cpu_id].bx_instr_cnear_branch_not_taken(branch_eip)
#define BX_INSTR_UCNEAR_BRANCH(cpu_id, what, branch_eip, new_eip) icpu[cpu_id].bx_instr_ucnear_branch(what, branch_eip, new_eip)
#define BX_INSTR_FAR_BRANCH(cpu_id, what, new_cs, new_eip) icpu[cpu_id].bx_instr_far_branch(what, new_cs, new_eip)

/* decoding completed */
#define BX_INSTR_OPCODE(cpu_id, i, opcode, len, is32, is64)

/* exceptional case and interrupt */
#define BX_INSTR_EXCEPTION(cpu_id, vector, error_code) \
                       icpu[cpu_id].bx_instr_exception(vector, error_code)

#define BX_INSTR_INTERRUPT(cpu_id, vector) icpu[cpu_id].bx_instr_interrupt(vector)
#define BX_INSTR_HWINTERRUPT(cpu_id, vector, cs, eip) icpu[cpu_id].bx_instr_hwinterrupt(vector, cs, eip)

/* TLB/CACHE control instruction executed */
#define BX_INSTR_CLFLUSH(cpu_id, laddr, paddr)
#define BX_INSTR_CACHE_CNTRL(cpu_id, what)
#define BX_INSTR_TLB_CNTRL(cpu_id, what, new_cr3)
#define BX_INSTR_PREFETCH_HINT(cpu_id, what, seg, offset)

/* execution */
#define BX_INSTR_BEFORE_EXECUTION(cpu_id, i) icpu[cpu_id].bx_instr_before_execution(i)
#define BX_INSTR_AFTER_EXECUTION(cpu_id, i) icpu[cpu_id].bx_instr_after_execution(i)
#define BX_INSTR_REPEAT_ITERATION(cpu_id, i)

/* memory access */
#define BX_INSTR_LIN_ACCESS(cpu_id, lin, phy, len, rw) \
                    icpu[cpu_id].bx_instr_lin_access(lin, phy, len, rw)

#define BX_INSTR_PHY_ACCESS(cpu_id, phy, len, rw)

/* feedback from device units */
#define BX_INSTR_INP(addr, len)
#define BX_INSTR_INP2(addr, len, val)
#define BX_INSTR_OUTP(addr, len, val)

/* wrmsr callback */
#define BX_INSTR_WRMSR(cpu_id, addr, value)

/* vmexit callback */
#define BX_INSTR_VMEXIT(cpu_id, reason, qualification)

#else // BX_INSTRUMENTATION

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

#endif // BX_INSTRUMENTATION
