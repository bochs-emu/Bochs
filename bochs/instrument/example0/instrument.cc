/////////////////////////////////////////////////////////////////////////
// $Id: instrument.cc,v 1.8 2002-11-20 17:55:40 bdenney Exp $
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


#include "bochs.h"
#include "cpu/cpu.h"


// maximum size of an instruction
#define MAX_OPCODE_SIZE 16

// maximum physical addresses an instruction can generate
#define MAX_DATA_ACCESSES 1024

// Use this variable to turn on/off collection of instrumentation data
// If you are not using the debugger to turn this on/off, then possibly
// start this at 1 instead of 0.
static bx_bool active = 1;


static struct instruction_t {
  bx_bool  valid;        // is current instruction valid
  unsigned opcode_size;
  unsigned nprefixes;
  Bit8u    opcode[MAX_OPCODE_SIZE];
  bx_bool  is32;
  unsigned num_data_accesses;
  struct {
    bx_address laddr; // linear address
    bx_address paddr; // physical address
    unsigned op;      // BX_READ or BX_WRITE
    unsigned size;    // 1 .. 8
  } data_access[MAX_DATA_ACCESSES];
  bx_bool is_branch;
  bx_bool is_taken;
  bx_address target_linear;
} instruction[BX_SMP_PROCESSORS];

static logfunctions *instrument_log = new logfunctions ();
#define LOG_THIS instrument_log->


void bx_instr_reset(unsigned cpu)
{
  instruction[cpu].valid = 0;
  instruction[cpu].nprefixes = 0;
  instruction[cpu].num_data_accesses = 0;
  instruction[cpu].is_branch = 0;
}

void bx_instr_new_instruction(unsigned cpu)
{
  if (!active)
  {
    return;
  }

  instruction_t *i = &instruction[cpu];

  if (i->valid)
  {
    char disasm_tbuf[512];	// buffer for instruction disassembly
    unsigned length = i->opcode_size, n;

    bx_disassemble.disasm(i->is32, 0, i->opcode, disasm_tbuf);
 
    if(length != 0)	
    {
      fprintf(stderr, "----------------------------------------------------------\n");
      fprintf(stderr, "CPU: %d: %s\n", cpu, disasm_tbuf);
      fprintf(stderr, "LEN: %d\tPREFIX: %d\tBYTES: ", length, i->nprefixes);
      for(n=0;n<length;n++) fprintf(stderr, "%02x", i->opcode[n]);
      if(i->is_branch) 
      {
        fprintf(stderr, "\tBRANCH ");

        if(i->is_taken) 
          fprintf(stderr, "TARGET %08x (TAKEN)", i->target_linear);
        else
          fprintf(stderr, "(NOT TAKEN)");
      }   
      fprintf(stderr, "\n");
      for(n=0;n<i->num_data_accesses;n++)
      {
         fprintf(stderr, "MEM ACCESS: %08x (linear) %08x (physical) %s SIZE: %d\n",
                      i->data_access[n].laddr, 
                      i->data_access[n].paddr,
                      i->data_access[n].op == BX_READ ? "RD":"WR",
                      i->data_access[n].size);
      }
      fprintf(stderr, "\n");
    }
  }

  instruction[cpu].valid = 0;
  instruction[cpu].nprefixes = 0;
  instruction[cpu].num_data_accesses = 0;
  instruction[cpu].is_branch = 0;
}

static void branch_taken(unsigned cpu, bx_address new_eip)
{
  Bit32u laddr;

  if (!active)
  {
    return;
  }
  if (!instruction[cpu].valid)
  {
    return;
  }

  // find linear address
  laddr = BX_CPU(cpu)->get_segment_base(BX_SEG_REG_CS) + new_eip;

  instruction[cpu].is_branch = 1;
  instruction[cpu].is_taken = 1;
  instruction[cpu].target_linear = laddr;
}

void bx_instr_cnear_branch_taken(unsigned cpu, bx_address new_eip) 
{
  branch_taken(cpu, new_eip);
}

void bx_instr_cnear_branch_not_taken(unsigned cpu)
{
  if (!active) 
  {
    return;
  }
  if (!instruction[cpu].valid)
  {
    return;
  }

  instruction[cpu].is_branch = 1;
  instruction[cpu].is_taken = 0;
}

void bx_instr_ucnear_branch(unsigned cpu, unsigned what, bx_address new_eip) {
  branch_taken(cpu, new_eip);
}

void bx_instr_far_branch(unsigned cpu, unsigned what, Bit16u new_cs, bx_address new_eip) {
  branch_taken(cpu, new_eip);
}

void bx_instr_opcode(unsigned cpu, Bit8u *opcode, unsigned len, bx_bool is32)
{
  if (!active) 
  {
    return;
  }

  for(int i=0;i<len;i++) 
  {
    instruction[cpu].opcode[i] = opcode[i];
  }
  
  instruction[cpu].is32 = is32;
  instruction[cpu].opcode_size = len;
}

void bx_instr_fetch_decode_completed(unsigned cpu, const bxInstruction_c *i)
{
  if(active) 
  {
    instruction[cpu].valid = 1; 
  }
}

#define PROCESS_PREFIX(name)                  \
  void bx_instr_prefix_##name(unsigned cpu)   \
  {                                           \
     if(active) instruction[cpu].nprefixes++; \
  }

PROCESS_PREFIX(as);
PROCESS_PREFIX(os);
PROCESS_PREFIX(rep);
PROCESS_PREFIX(repne);
PROCESS_PREFIX(lock);
PROCESS_PREFIX(cs);
PROCESS_PREFIX(ss);
PROCESS_PREFIX(ds);
PROCESS_PREFIX(es);
PROCESS_PREFIX(fs);
PROCESS_PREFIX(gs);
PROCESS_PREFIX(extend8b);

void bx_instr_interrupt(unsigned cpu, unsigned vector)
{
  if(active)
  {
    fprintf(stderr, "CPU %u: interrupt %02xh\n", cpu, vector);
  }
}

void bx_instr_exception(unsigned cpu, unsigned vector)
{
  if(active)
  {
    fprintf(stderr, "CPU %u: exception %02xh\n", cpu, vector);
  }
}

void bx_instr_hwinterrupt(unsigned cpu, unsigned vector, Bit16u cs, bx_address eip)
{
  if(active)
  {
    fprintf(stderr, "CPU %u: hardware interrupt %02xh\n", cpu, vector);
  }
}

void bx_instr_mem_data(unsigned cpu, bx_address lin, unsigned size, unsigned rw)
{
  unsigned index;
  bx_address phy;
  bx_bool page_valid;

  if(!active)
  {
    return;
  }
  if (!instruction[cpu].valid)
  {
    return;
  }

  if (instruction[cpu].num_data_accesses >= MAX_DATA_ACCESSES) 
  {
    return;
  }

  BX_CPU(cpu)->dbg_xlate_linear2phy(lin, &phy, &page_valid);
  phy = A20ADDR(phy);

  // If linear translation doesn't exist, a paging exception will occur.
  // Invalidate physical address data for now.
  if (!page_valid) 
  {
    phy = 0;
  }

  index = instruction[cpu].num_data_accesses;
  instruction[cpu].data_access[index].laddr = lin;
  instruction[cpu].data_access[index].paddr = phy;
  instruction[cpu].data_access[index].op    = rw;
  instruction[cpu].data_access[index].size  = size;
  instruction[cpu].num_data_accesses++;
}
