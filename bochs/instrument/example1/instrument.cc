/////////////////////////////////////////////////////////////////////////
// $Id: instrument.cc,v 1.4 2002-10-25 11:44:37 bdenney Exp $
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

bxInstrumentation icpu[BX_SMP_PROCESSORS];


void bxInstrumentation::bx_instr_reset()
{
  valid = is_branch = 0;
  nprefixes = num_data_accesses = 0;
  active = 1;
}

void bxInstrumentation::bx_instr_new_instruction()
{
  if (!active)
  {
    return;
  }

  if (valid)
  {
    char disasm_tbuf[512];	// buffer for instruction disassembly
    unsigned length = opcode_size, n;
    bx_disassemble.disasm(is32, 0, opcode, disasm_tbuf);
    if(length != 0)	
    {
      fprintf(stderr, "----------------------------------------------------------\n");
      fprintf(stderr, "CPU: %d: %s\n", cpu_id, disasm_tbuf);
      fprintf(stderr, "LEN: %d\tPREFIX: %d\tBYTES: ", length, nprefixes);
      for(n=0;n<length;n++) fprintf(stderr, "%02x", opcode[n]);
      if(is_branch) 
      {
        fprintf(stderr, "\tBRANCH ");

        if(is_taken) 
          fprintf(stderr, "TARGET %08x (TAKEN)", target_linear);
        else
          fprintf(stderr, "(NOT TAKEN)");
      }   
      fprintf(stderr, "\n");
      for(n=0;n < num_data_accesses;n++)
      {
         fprintf(stderr, "MEM ACCESS: %08x (linear) %08x (physical) %s SIZE: %d\n",
                      data_access[n].laddr, 
                      data_access[n].paddr,
                      data_access[n].op == BX_READ ? "RD":"WR",
                      data_access[n].size);
      }
      fprintf(stderr, "\n");
    }
  }

  valid = is_branch = 0;
  nprefixes = num_data_accesses = 0;
}

void bxInstrumentation::branch_taken(bx_address new_eip)
{
  Bit32u laddr;

  if (!active || !valid) {
    return; 
  }

  // find linear address
  laddr = BX_CPU(cpu_id)->get_segment_base(BX_SREG_CS) + new_eip;

  is_branch = 1;
  is_taken = 1;
  target_linear = laddr;
}

void bxInstrumentation::bx_instr_cnear_branch_taken(bx_address new_eip) 
{
  branch_taken(new_eip);
}

void bxInstrumentation::bx_instr_cnear_branch_not_taken()
{
  if (!active || !valid) {
    return; 
  }

  is_branch = 1;
  is_taken = 0;
}

void bxInstrumentation::bx_instr_ucnear_branch(unsigned what, bx_address new_eip) {
  branch_taken(new_eip);
}

void bxInstrumentation::bx_instr_far_branch(unsigned what, Bit16u new_cs, bx_address new_eip) {
  branch_taken(new_eip);
}

void bxInstrumentation::bx_instr_opcode(Bit8u *opcode, unsigned len, bx_bool is32)
{
  if (!active) 
  {
    return;
  }

  for(int i=0;i<len;i++) 
  {
    opcode[i] = opcode[i];
  }
  
  is32 = is32;
  opcode_size = len;
}

void bxInstrumentation::bx_instr_fetch_decode_completed(const bxInstruction_c *i)
{
  if(active) 
  {
    valid = 1; 
  }
}

#define PROCESS_PREFIX(name)                         \
  void bxInstrumentation::bx_instr_prefix_##name()   \
  {                                                  \
     if(active) nprefixes++;        \
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

void bxInstrumentation::bx_instr_interrupt(unsigned vector)
{
  if(active)
  {
    fprintf(stderr, "CPU %u: interrupt %02xh\n", cpu_id, vector);
  }
}

void bxInstrumentation::bx_instr_exception(unsigned vector)
{
  if(active)
  {
    fprintf(stderr, "CPU %u: exception %02xh\n", cpu_id, vector);
  }
}

void bxInstrumentation::bx_instr_hwinterrupt(unsigned vector, Bit16u cs, bx_address eip)
{
  if(active)
  {
    fprintf(stderr, "CPU %u: hardware interrupt %02xh\n", cpu_id, vector);
  }
}

void bxInstrumentation::bx_instr_mem_data(bx_address lin, unsigned size, unsigned rw)
{
  bx_address phy;
  bx_bool page_valid;

  if(!active)
  {
    return;
  }
  if (!valid)
  {
    return;
  }

  if (num_data_accesses >= MAX_DATA_ACCESSES) 
  {
    return;
  }

  BX_CPU(cpu_id)->dbg_xlate_linear2phy(lin, &phy, &page_valid);
  phy = A20ADDR(phy);

  // If linear translation doesn't exist, a paging exception will occur.
  // Invalidate physical address data for now.
  if (!page_valid) 
  {
    phy = 0;
  }

  data_access[num_data_accesses].laddr = lin;
  data_access[num_data_accesses].paddr = phy;
  data_access[num_data_accesses].op    = rw;
  data_access[num_data_accesses].size  = size;
  num_data_accesses++;
}
