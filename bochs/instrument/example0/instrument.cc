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


// Sample instrumentation library to generate physical address
// of each instruction executed, and physical address of it's
// data reads/writes


#include "bochs.h"


// maximum size of an instruction
#define MAX_OPCODE_SIZE 16

// maximum physical addresses an instruction can generate
#define MAX_DATA_ACCESSES 1024

// maximum number of IO accesses an instruction can generate
#define MAX_IO_ACCESSES 4 // probably could be 1

// Use this variable to turn on/off collection of instrumentation data
// If you are not using the debugger to turn this on/off, then possibly
// start this at 1 instead of 0.
static Boolean  collect_data = 0;

static Boolean  is_valid = 0;  // is current instruction valid


static struct {
  unsigned opcode_size;
  Bit8u    opcode[MAX_OPCODE_SIZE];
  Bit32u   linear_iaddr;
  Bit32u   phy_iaddr;
  unsigned num_data_accesses;
  struct {
    Bit32u   laddr; // linear address
    Bit32u   paddr; // physical address
    unsigned op; // BX_READ or BX_WRITE
    unsigned size; // 1 .. 4
    } data_access[MAX_DATA_ACCESSES];
  unsigned num_io_accesses;
  struct {
    Bit16u addr;
    unsigned op; // BX_READ==inp or BX_WRITE==outp
    unsigned size; // 1 .. 4
    } io_access[MAX_IO_ACCESSES];
  } instruction;


  void
bx_instr_opcode_begin(Bit32u linear)
{
  Boolean valid;

  if (!collect_data) return;

  is_valid = 1;
  instruction.opcode_size = 0;
  instruction.linear_iaddr = linear;
  bx_cpu.dbg_xlate_linear2phy(linear, &instruction.phy_iaddr, &valid);
  instruction.phy_iaddr = A20ADDR(instruction.phy_iaddr);

  // If linear translation doesn't exist, a paging exception will occur.
  // Invalidate collecting data for now.
  if (!valid)
    is_valid = 0;

  instruction.num_data_accesses = 0;
  instruction.num_io_accesses = 0;
}

  void
bx_instr_opcode_end(Bit32u linear)
{
  unsigned i;
  UNUSED(linear);

  if (!collect_data || !is_valid) return;

  // instruction completed:
  // print out instruction opcode bytes and phy mem stats
  if (is_valid) {
    fprintf(stderr, "opcode: l=0x%x, p=0x%x: ",
            (unsigned) instruction.linear_iaddr,
            (unsigned) instruction.phy_iaddr);
    for (i=0; i<instruction.opcode_size; i++) {
      fprintf(stderr, " %02x", instruction.opcode[i]);
      }
    fprintf(stderr, "\n");
    for (i=0; i<instruction.num_data_accesses; i++) {
      fprintf(stderr, "%s: l=0x%x, p=0x%x, %u bytes\n",
        (instruction.data_access[i].op==BX_READ) ? "read" : "write",
        (unsigned) instruction.data_access[i].laddr,
        (unsigned) instruction.data_access[i].paddr,
        (unsigned) instruction.data_access[i].size);
      }
    for (i=0; i<instruction.num_io_accesses; i++) {
      fprintf(stderr, "%s: 0x%x, %u bytes\n",
        (instruction.io_access[i].op==BX_READ) ? "in" : "out",
        (unsigned) instruction.io_access[i].addr,
        (unsigned) instruction.io_access[i].size);
      }
    }

  is_valid = 0;
}

  void
bx_instr_fetch_byte(Bit8u val8)
{
  if (!collect_data || !is_valid) return;

  if ((instruction.opcode_size+1) > MAX_OPCODE_SIZE) {
    bx_panic("instr_fetch_byte: opcode too big.\n");
    }
  else {
    instruction.opcode[instruction.opcode_size] = val8;
    instruction.opcode_size++;
    }
}

  void
bx_instr_fetch_word(Bit16u val16)
{
  if (!collect_data || !is_valid) return;

  if ((instruction.opcode_size+2) > MAX_OPCODE_SIZE) {
    bx_panic("instr_fetch_word: opcode too big.\n");
    }
  else {
    instruction.opcode[instruction.opcode_size]   = val16 & 0xff;
    instruction.opcode[instruction.opcode_size+1] = val16 >> 8;
    instruction.opcode_size += 2;
    }
}

  void
bx_instr_fetch_dword(Bit32u val32)
{
  if (!collect_data || !is_valid) return;

  if ((instruction.opcode_size+4) > MAX_OPCODE_SIZE) {
    bx_panic("instr_fetch_dword: opcode too big.\n");
    }
  else {
    instruction.opcode[instruction.opcode_size]   = val32; val32 >>= 8;
    instruction.opcode[instruction.opcode_size+1] = val32; val32 >>= 8;
    instruction.opcode[instruction.opcode_size+2] = val32; val32 >>= 8;
    instruction.opcode[instruction.opcode_size+3] = val32;
    instruction.opcode_size += 4;
    }
}

  void
bx_instr_interrupt(unsigned vector)
{
  UNUSED(vector);

  if (!collect_data || !is_valid) return;
}

  void
bx_instr_exception(unsigned vector)
{
  is_valid = 0;
  if (!collect_data || !is_valid) return;
}


  void
bx_instr_inp(Bit16u addr, unsigned len)
{
  unsigned index;

  if (!collect_data || !is_valid) return;

  if (instruction.num_io_accesses >= MAX_IO_ACCESSES) {
    bx_panic("instr_inp: instructions has too many accesses\n");
    }
  index = instruction.num_io_accesses;
  instruction.io_access[index].addr = addr;
  instruction.io_access[index].op   = BX_READ;
  instruction.io_access[index].size = len;
  instruction.num_io_accesses++;
}

  void
bx_instr_outp(Bit16u addr, unsigned len)
{
  unsigned index;

  if (!collect_data || !is_valid) return;

  if (instruction.num_io_accesses >= MAX_IO_ACCESSES) {
    bx_panic("instr_outp: instructions has too many accesses\n");
    }
  index = instruction.num_io_accesses;
  instruction.io_access[index].addr = addr;
  instruction.io_access[index].op   = BX_WRITE;
  instruction.io_access[index].size = len;
  instruction.num_io_accesses++;
}




// some extra functions called from debugger
  void
bx_instr_start(void)
{
  collect_data = 1;
  is_valid = 0;
}

  void
bx_instr_stop(void)
{
  collect_data = 0;
  is_valid = 0;
}

  void
bx_instr_reset(void)
{
}

  void
bx_instr_print(void)
{
}

  void
bx_instr_cnear_branch_taken(Bit32u new_eip)
{
  // conditional near branch was taken
  Bit32u laddr, paddr;
  Boolean valid;

  if (!collect_data || !is_valid) return;

  // find linear address
  laddr = bx_cpu.cs.cache.u.segment.base + new_eip;

  // find physical address
  bx_cpu.dbg_xlate_linear2phy(laddr, &paddr, &valid);
  paddr = A20ADDR(paddr);
  if (valid==0) {
    // don't use paddr; laddr not found in page tables
    }
  else {
    fprintf(stderr, "cnear taken: l=0x%x p=0x%x\n", laddr, paddr);
    }
}

  void
bx_instr_cnear_branch_not_taken(void)
{
  // conditional near branch was NOT taken
  if (!collect_data || !is_valid) return;

  fprintf(stderr, "cnear NOT taken\n");
}

  void
bx_instr_ucnear_branch(unsigned what, Bit32u new_eip)
{
  // unconditional near branch
  Bit32u laddr, paddr;
  Boolean valid;

  if (!collect_data || !is_valid) return;

  // find linear address
  laddr = bx_cpu.cs.cache.u.segment.base + new_eip;

  // find physical address
  bx_cpu.dbg_xlate_linear2phy(laddr, &paddr, &valid);
  paddr = A20ADDR(paddr);
  if (valid==0) {
    // don't use paddr; laddr not found in page tables
    }
  else {
    fprintf(stderr, "ucnear: l=0x%x p=0x%x\n", laddr, paddr);
    }

  switch (what) {
    case BX_INSTR_IS_CALL: break;
    case BX_INSTR_IS_JMP: break;
    case BX_INSTR_IS_RET: break;
    default:
      bx_panic("instr_ucnear_branch: default case reached.\n");
    }
}

  void
bx_instr_far_branch(unsigned what, Bit32u new_cs, Bit32u new_eip)
{
  // unconditional far branch
  Bit32u laddr, paddr;
  Boolean valid;

  UNUSED(new_cs);

  if (!collect_data || !is_valid) return;

  // find linear address
  laddr = bx_cpu.cs.cache.u.segment.base + new_eip;

  // find physical address
  bx_cpu.dbg_xlate_linear2phy(laddr, &paddr, &valid);
  paddr = A20ADDR(paddr);
  if (valid==0) {
    // don't use paddr; laddr not found in page tables
    }
  else {
    fprintf(stderr, "far: l=0x%x p=0x%x\n", laddr, paddr);
    }

  switch (what) {
    case BX_INSTR_IS_CALL: break;
    case BX_INSTR_IS_INT: break;
    case BX_INSTR_IS_IRET: break;
    case BX_INSTR_IS_JMP: break;
    case BX_INSTR_IS_RET: break;
    default:
      bx_panic("instr_far_branch: default case reached.\n");
    }
}


  void
bx_instr_lin_write(Bit32u lin, Bit32u phy, unsigned len)
{
  unsigned index;

  if (!collect_data || !is_valid) return;

  if (instruction.num_data_accesses >= MAX_DATA_ACCESSES) {
    bx_panic("instr_lin_write: instruction has too many accesses\n");
    }
  index = instruction.num_data_accesses;
  instruction.data_access[index].paddr = A20ADDR(phy);
  instruction.data_access[index].laddr = lin;
  instruction.data_access[index].op   = BX_WRITE;
  instruction.data_access[index].size = len;
  instruction.num_data_accesses++;
}

  void
bx_instr_lin_read(Bit32u lin, Bit32u phy, unsigned len)
{
  unsigned index;

  if (!collect_data || !is_valid) return;

  if (instruction.num_data_accesses >= MAX_DATA_ACCESSES) {
    bx_panic("instr_lin_read: instruction has too many accesses\n");
    }
  index = instruction.num_data_accesses;
  instruction.data_access[index].paddr = A20ADDR(phy);
  instruction.data_access[index].laddr = lin;
  instruction.data_access[index].op   = BX_READ;
  instruction.data_access[index].size = len;
  instruction.num_data_accesses++;
}
