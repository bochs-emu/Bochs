/////////////////////////////////////////////////////////////////////////
// $Id: sim2.cc,v 1.2.8.1 2002-03-17 08:57:02 bdenney Exp $
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


// NOTE:  This file contains only stubs for a second CPU simulator
//        to demonstrate use with the bochs debugger.  The goal is to
//        be able to run multiple simulators in a co-simulation environment.
//        Each simulator has it's own CPU and memory space.  There is only
//        one set of device models, managed by the debugger.


Boolean  sim2_set_mem(Bit32u addr, unsigned len, Bit8u *buf);
Boolean  sim2_fetch_mem(Bit32u addr, unsigned len, Bit8u *buf);
void     sim2_xlate_linear2phy(Bit32u linear, Bit32u *phy, Boolean *valid);
Boolean  sim2_set_reg(unsigned reg, Bit32u val);
Bit32u   sim2_get_reg(unsigned reg);
Boolean  sim2_set_cpu(bx_dbg_cpu_t *cpu);
Boolean  sim2_get_cpu(bx_dbg_cpu_t *cpu);
unsigned       dirty_page_tbl_size;
unsigned char sim2_dirty_pages[(BX_MAX_DIRTY_PAGE_TABLE_MEGS * 1024 * 1024) / 4096];
void     sim2_atexit(void);
unsigned sim2_query_pending(void);
void     sim2_cpu_loop(void);
void     sim2_take_irq(void);
void     sim2_take_dma(void);
void     sim2_reset_cpu(void);
void     sim2_init_mem(int size_in_bytes);
void     sim2_load_ROM(const char *path, Bit32u romaddress);

void     sim2_set_A20(unsigned val);
void     sim2_set_NMI(unsigned val);
void     sim2_set_RESET(unsigned val);
void     sim2_set_INTR(unsigned val);
void     sim2_force_interrupt(unsigned vector);

#if BX_INSTRUMENTATION
void     sim2_instr_start(void);
void     sim2_instr_stop(void);
void     sim2_instr_reset(void);
void     sim2_instr_print(void);
#endif
#if BX_USE_LOADER
void    sim2_loader(char *path);
#endif


#if BX_DBG_EXTENSIONS
// return 0 if command not handled by extensions, bochs will handle
// return 1 if command handled by extensions, bochs will ignore
  int
bx_dbg_extensions(char *command)
{
  UNUSED(command);
  return(0); // no extensions for now
}
#endif


  void
sim2_init(bx_dbg_callback_t *callback, int argc, char *argv[])
{
  callback->setphymem           = sim2_set_mem;
  callback->getphymem           = sim2_fetch_mem;
  callback->xlate_linear2phy    = sim2_xlate_linear2phy;
  callback->set_reg             = sim2_set_reg;
  callback->get_reg             = sim2_get_reg;
  callback->get_cpu             = sim2_get_cpu;
  callback->set_cpu             = sim2_set_cpu;
  callback->dirty_page_tbl_size = sizeof(sim2_dirty_pages);
  callback->dirty_page_tbl      = sim2_dirty_pages;
  callback->atexit              = sim2_atexit;
  callback->query_pending       = sim2_query_pending;
  callback->execute             = sim2_cpu_loop;
  callback->take_irq            = sim2_take_irq;
  callback->take_dma            = sim2_take_dma;
  callback->reset_cpu           = sim2_reset_cpu;
  callback->init_mem            = sim2_init_mem;
  callback->load_ROM            = sim2_load_ROM;

  // You may set this to NULL and use values in bx_pc_system as per
  // docs-html/cosimulation.html
  callback->set_A20             = sim2_set_A20;

  callback->set_NMI             = sim2_set_NMI;
  callback->set_RESET           = sim2_set_RESET;
  callback->set_INTR            = sim2_set_INTR;
  callback->force_interrupt     = sim2_force_interrupt;

#if BX_INSTRUMENTATION
  callback->instr_start         = sim2_instr_start;
  callback->instr_stop          = sim2_instr_stop;
  callback->instr_reset         = sim2_instr_reset;
  callback->instr_print         = sim2_instr_print;
#endif
#if BX_USE_LOADER
  callback->loader              = sim2_loader;
#endif
}

  Boolean
sim2_set_mem(Bit32u addr, unsigned len, Bit8u *buf)
{
  return(0);
}
  Boolean
sim2_fetch_mem(Bit32u addr, unsigned len, Bit8u *buf)
{
  return(0);
}
  void
sim2_xlate_linear2phy(Bit32u linear, Bit32u *phy, Boolean *valid)
{
}
  Boolean
sim2_set_reg(unsigned reg, Bit32u val)
{
  return(0);
}
  Bit32u
sim2_get_reg(unsigned reg)
{
  return(0);
}
  Boolean
sim2_set_cpu(bx_dbg_cpu_t *cpu)
{
  return(0);
}
  Boolean
sim2_get_cpu(bx_dbg_cpu_t *cpu)
{
  return(0);
}
  void
sim2_atexit(void)
{
}
  unsigned
sim2_query_pending(void)
{
  return(0);
}
  void
sim2_cpu_loop(void)
{
}
  void
sim2_take_irq(void)
{
}
  void
sim2_take_dma(void)
{
}
  void
sim2_reset_cpu(void)
{
}
  void
sim2_init_mem(int size_in_bytes)
{
}
  void
sim2_load_ROM(const char *path, Bit32u romaddress)
{
}

  void
sim2_set_A20(unsigned val)
{
}
  void
sim2_set_NMI(unsigned val)
{
}
  void
sim2_set_RESET(unsigned val)
{
}
  void
sim2_set_INTR(unsigned val)
{
}
  void
sim2_force_interrupt(unsigned vector)
{
}

#if BX_INSTRUMENTATION
  void
sim2_instr_start(void)
{
}
  void
sim2_instr_stop(void)
{
}
  void
sim2_instr_reset(void)
{
}
  void
sim2_instr_print(void)
{
}
#endif

#if BX_USE_LOADER
  void
sim2_loader(char *path)
{
}
#endif
