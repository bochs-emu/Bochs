/////////////////////////////////////////////////////////////////////////
// $Id: instrument.cc,v 1.10 2003-02-13 15:04:10 sshwarts Exp $
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

void bx_instr_init(unsigned cpu) {}
void bx_instr_shutdown(unsigned cpu) {}
void bx_instr_reset(unsigned cpu) {}
void bx_instr_new_instruction(unsigned cpu) {}

void bx_instr_debug_promt() {}
void bx_instr_start() {}
void bx_instr_stop() {}
void bx_instr_print() {}

void bx_instr_cnear_branch_taken(unsigned cpu, bx_address new_eip) {}
void bx_instr_cnear_branch_not_taken(unsigned cpu) {}
void bx_instr_ucnear_branch(unsigned cpu, unsigned what, bx_address new_eip) {}
void bx_instr_far_branch(unsigned cpu, unsigned what, Bit16u new_cs, bx_address new_eip) {}

void bx_instr_opcode(unsigned cpu, Bit8u *opcode, unsigned len, bx_bool is32) {}
void bx_instr_fetch_decode_completed(unsigned cpu, const bxInstruction_c *i) {}

void bx_instr_prefix_as(unsigned cpu) {}
void bx_instr_prefix_os(unsigned cpu) {}
void bx_instr_prefix_rep(unsigned cpu) {}
void bx_instr_prefix_repne(unsigned cpu) {}
void bx_instr_prefix_lock(unsigned cpu) {}
void bx_instr_prefix_cs(unsigned cpu) {}
void bx_instr_prefix_ss(unsigned cpu) {}
void bx_instr_prefix_ds(unsigned cpu) {}
void bx_instr_prefix_es(unsigned cpu) {}
void bx_instr_prefix_fs(unsigned cpu) {}
void bx_instr_prefix_gs(unsigned cpu) {}
void bx_instr_prefix_extend8b(unsigned cpu) {}

void bx_instr_interrupt(unsigned cpu, unsigned vector) {}
void bx_instr_exception(unsigned cpu, unsigned vector) {}
void bx_instr_hwinterrupt(unsigned cpu, unsigned vector, Bit16u cs, bx_address eip) {}

void bx_instr_tlb_cntrl(unsigned cpu, unsigned what, Bit32u newval) {}
void bx_instr_cache_cntrl(unsigned cpu, unsigned what) {}
void bx_instr_prefetch_hint(unsigned cpu, unsigned what, unsigned seg, bx_address offset) {}

void bx_instr_repeat_iteration(unsigned cpu) {}

void bx_instr_inp(Bit16u addr, unsigned len) {}
void bx_instr_outp(Bit16u addr, unsigned len) {}
void bx_instr_inp2(Bit16u addr, unsigned len, unsigned val) {}
void bx_instr_outp2(Bit16u addr, unsigned len, unsigned val) {}

void bx_instr_mem_code(unsigned cpu, bx_address linear, unsigned size) {}
void bx_instr_mem_data(unsigned cpu, bx_address linear, unsigned size, unsigned rw) {}

void bx_instr_lin_read(unsigned cpu, bx_address lin, bx_address phy, unsigned len) {}
void bx_instr_lin_write(unsigned cpu, bx_address lin, bx_address phy, unsigned len) {}

void bx_instr_phy_write(unsigned cpu, bx_address addr, unsigned len) {}
void bx_instr_phy_read(unsigned cpu, bx_address addr, unsigned len) {}
