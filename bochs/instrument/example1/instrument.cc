/////////////////////////////////////////////////////////////////////////
// $Id: instrument.cc,v 1.2.4.1 2002-03-17 09:05:55 bdenney Exp $
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

static logfunctions *instr_logfn = NULL;
#define LOG_THIS instr_logfn->
static FILE *logfd = NULL;

#if 0
// possible types passed to BX_INSTR_TLB_CNTRL()
#define BX_INSTR_MOV_CR3     10
#define BX_INSTR_INVLPG      11
#define BX_INSTR_TASKSWITCH  12

// possible types passed to BX_INSTR_CACHE_CNTRL()
#define BX_INSTR_INVD      20
#define BX_INSTR_WBINVD    21
#endif


// called from the CPU core
  void
bx_instr_cnear_branch_taken(Bit32u new_eip)
{
  UNUSED(new_eip);
}

  void
bx_instr_cnear_branch_not_taken(void)
{
}

  void
bx_instr_ucnear_branch(unsigned what, Bit32u new_eip)
{
  UNUSED(what);
  UNUSED(new_eip);
}

  void
bx_instr_far_branch(unsigned what, Bit32u new_cs, Bit32u new_eip)
{
  UNUSED(what);
  UNUSED(new_eip);
  UNUSED(new_cs);
}

  void
bx_instr_opcode_byte1(Bit8u opcode)
{
  UNUSED(opcode);
}

  void
bx_instr_opcode_byte2(Bit8u opcode)
{
  UNUSED(opcode);
}

  void
bx_instr_opcode_g1ebib(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g1eviv(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g1evib(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g2ebib(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g2evib(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g2eb1(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g2ev1(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g2ebcl(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g2evcl(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g3eb(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g3ev(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g4(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g5(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g6(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g7(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_opcode_g8evib(unsigned nnn)
{
  UNUSED(nnn);
}

  void
bx_instr_mem_code(Bit32u linear, unsigned size)
{
  UNUSED(linear);
  UNUSED(size);
}

  void
bx_instr_mem_data(Bit32u linear, unsigned size, unsigned rw)
{
  UNUSED(linear);
  UNUSED(size);
  UNUSED(rw);
}

  void
bx_instr_opcode_begin(Bit32u linear)
{
  Bit32u cs = BX_CPU(0)->sregs[BX_SEG_REG_CS].selector.value;
  Bit32u eip = BX_CPU(0)->prev_eip;
  fprintf (logfd, "pc %04x:%08x\n", cs, eip);
  fflush (logfd);
}

  void
bx_instr_opcode_end(Bit32u linear)
{
  UNUSED(linear);
}

  void
bx_instr_exception(unsigned vector)
{
  UNUSED(vector);
}

  void
bx_instr_tlb_cntrl(unsigned what, Bit32u newval)
{
  UNUSED(what);
  UNUSED(newval);
}

  void
bx_instr_cache_cntrl(unsigned what)
{
  UNUSED(what);
}

  void
bx_instr_hwinterrupt(unsigned vector, Bit32u cs, Bit32u eip)
{
  UNUSED(vector);
  UNUSED(cs);
  UNUSED(eip);
}

  void
bx_instr_init(void)
{
  instr_logfn = new logfunctions ();
  instr_logfn->put ("INST");
  logfd = fopen ("bxevent.txt", "w");
  if (!logfd) {
    BX_PANIC (("could not open bxevent.txt"));
    // if they continue through the panic, just log to stderr
    logfd = stderr;
  }
  fprintf (logfd, "# Instrument init\n");
}

  void
bx_instr_shutdown(void)
{
  fprintf (logfd, "# Instrument shutdown\n");
}

  void
bx_instr_opcode_repeating(void)
{
}


// called from bochs debugger if you type "instrument start"
  void
bx_instr_start(void)
{
  fprintf (logfd, "# Instrument start\n");
}

// called from bochs debugger if you type "instrument stop"
  void
bx_instr_stop(void)
{
  fprintf (logfd, "# Instrument stop\n");
}

// called from bochs debugger if you type "instrument reset"
  void
bx_instr_reset(void)
{
  fprintf (logfd, "# Instrument reset\n");
}

// called from bochs debugger if you type "instrument print"
  void
bx_instr_print(void)
{
  fprintf (logfd, "# Instrument print\n");
}

  void
bx_instr_prefix_as(void)
{
}
  void
bx_instr_prefix_os(void)
{
}
  void
bx_instr_prefix_rep(void)
{
}
  void
bx_instr_prefix_repne(void)
{
}
  void
bx_instr_prefix_lock(void)
{
}
  void
bx_instr_prefix_cs(void)
{
}
  void
bx_instr_prefix_ss(void)
{
}
  void
bx_instr_prefix_ds(void)
{
}
  void
bx_instr_prefix_es(void)
{
}
  void
bx_instr_prefix_fs(void)
{
}
  void
bx_instr_prefix_gs(void)
{
}

  void
bx_instr_modrm32(unsigned modrm)
{
  UNUSED(modrm);
}

  void
bx_instr_sib32(unsigned sib)
{
  UNUSED(sib);
}

  void
bx_instr_modrm16(unsigned modrm)
{
  UNUSED(modrm);
}

  void
bx_instr_iret(void)
{
}

  void
bx_instr_debug_prompt(void)
{
}

  void
bx_instr_fetch_byte(Bit8u val8)
{
}
  void
bx_instr_fetch_word(Bit16u val16)
{
}
  void
bx_instr_fetch_dword(Bit32u val32)
{
}
  void
bx_instr_phy_write(Bit32u addr, unsigned len)
{
}
  void
bx_instr_phy_read(Bit32u addr, unsigned len)
{
}
  void
bx_instr_interrupt(unsigned vector)
{
}
  void
bx_instr_inp(Bit16u addr, unsigned len)
{
}
  void
bx_instr_outp(Bit16u addr, unsigned len)
{
}
  void
bx_instr_inp2(Bit16u addr, unsigned len, unsigned val)
{
  fprintf (logfd, "in %d %04x %04x\n", len, addr, val);
}
  void
bx_instr_outp2(Bit16u addr, unsigned len, unsigned val)
{
  fprintf (logfd, "out %d %04x %04x\n", len, addr, val);
}
void bx_instr_lin_read(Bit32u lin, Bit32u phy, unsigned len)
{
}
void bx_instr_lin_write(Bit32u lin, Bit32u phy, unsigned len)
{
}
