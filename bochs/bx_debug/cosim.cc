/////////////////////////////////////////////////////////////////////////
// $Id: cosim.cc,v 1.1 2005-04-10 18:03:15 sshwarts Exp $
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
//

#include "bochs.h"

#if BX_NUM_SIMULATORS >= 2

unsigned bx_dbg_cosimulateN(bx_dbg_icount_t count)
{
  // execute both master & slave for count instructions,
  // handling asynchronous events, etc.
  // returns 0 = didn't get through all count instructions
  //             either a guard was hit, or a divergence occurred
  //         1 = got through all count instructions

  unsigned master, slave;
  bx_dbg_icount_t master_icount, slave_icount;
  bx_bool bail_out = 0;
  unsigned ret = 0;
  bx_bool save_INTR;
  bx_bool pre_A20, post_A20;
  unsigned async_head;
  bx_dbg_icount_t async_icount, curr_icount;

  if (count == 0) {
    dbg_printf ( "Error: cosimulateN: count=0\n");
    bx_dbg_exit(1);
  }

  bx_guard.guard_for |= BX_DBG_GUARD_ICOUNT;  // stop at icount
  bx_guard.guard_for &= ~BX_DBG_GUARD_CTRL_C; // ignore Ctrl-C

one_time_slice:
  // take minimum of requested count and maximum count quantum
  if (count > bx_debugger.icount_quantum)
    bx_guard.icount = bx_debugger.icount_quantum;
  else
    bx_guard.icount = count;

  // for now, assume...
  master = bx_debugger.master;
  slave  = bx_debugger.slave;

  // run master simulator
  bx_debugger.master_slave_mode = BX_DBG_MASTER_MODE;
  if (bx_guard.interrupt_requested) {
    bail_out = 1;
    dbg_printf ("ctrlc typed\n");
  }
  bx_guard_found[master].guard_found = 0;
  bx_guard_found[master].icount = 0;
  if (doit) dbg_printf ("requesting run of master for %u\n",
    (unsigned) bx_guard.icount);
  // save A20 value before master run
  pre_A20 = bx_pc_system.get_enable_a20();

  BX_MEM(master)->cpu_loop(-1);
  post_A20  = bx_pc_system.get_enable_a20(); // A20 after master run
  master_icount = bx_guard_found[master].icount;
  slave_icount = 0;
  if (master_icount)
    bx_pc_system.tickn(master_icount);
  save_INTR = bx_pc_system.INTR; // value after master run
  bx_pc_system.INTR = 0; // in case slave uses directly
  // Change A20 for slave run to model what it was at beginning of
  // master run, only if it needs to be changed.
  if (pre_A20 != post_A20) {
    bx_pc_system.set_enable_a20(pre_A20);
    if (BX_MEM(slave)->set_A20)
      BX_MEM(slave)->set_A20(pre_A20);
    }

  // if guard was anything except for icount, we should terminate
  // after synchronizing slave to master
  if (bx_guard_found[master].guard_found & ~BX_DBG_GUARD_ICOUNT)
    bail_out = 1;

  // Synchronize slave to master.  Account for Ctrl-C's typed during execution of
  // slave.
  bx_debugger.master_slave_mode = BX_DBG_SLAVE_MODE;
  do {
    // run slave for remaining instructions to catch up to master
    curr_icount = master_icount - slave_icount;
    if (bx_debugger.async_journal.size) {
      // If there were asynchronous events which occurred while the
      // master was running, have to run the slave up to each of these
      // points individually, and force it to take them on exactly the
      // same boundaries.
      async_head   = bx_debugger.async_journal.head;
      async_icount = bx_debugger.async_journal.element[async_head].icount;
      curr_icount  = async_icount;  // only run to next async event
      }
    else {
      async_head   = 0; // keep compiler happy
      async_icount = 0; // keep compiler happy
      }

    bx_guard_found[slave].guard_found = 0;
    bx_guard_found[slave].icount = 0;
    bx_guard.icount = curr_icount;

    if (curr_icount) {
      // Async event may be before completion of any instructions,
      // for example taking of interrupt.
      if (doit) dbg_printf ( "requesting run of slave for %u\n",
        (unsigned) bx_guard.icount);
      if (bx_debugger.fast_forward_mode) {
        bx_guard_found[slave].icount = curr_icount;
        bx_guard_found[slave].guard_found = BX_DBG_GUARD_ICOUNT;
      } else {
        BX_MEM(slave)->cpu_loop(-1);
      }
    }
    slave_icount += bx_guard_found[slave].icount;
    if (bx_guard_found[slave].guard_found & ~BX_DBG_GUARD_ICOUNT) {
      bail_out = 1;
      // If user type Ctrl-C we're done after synchronizing.  If not,
      // then we have reached a true guard, and it's time to bail.
      if (bx_guard_found[slave].guard_found &
          ~(BX_DBG_GUARD_ICOUNT | BX_DBG_GUARD_CTRL_C))
        break;
    }
    if (bx_debugger.async_journal.size) {
      // sanity check: slave should be at async point
      if (bx_guard_found[slave].icount != async_icount) {
        dbg_printf ( "Error: comsimulateN: async: slave not at sync point.\n");
        bx_dbg_exit(1);
      }
      switch (bx_debugger.async_journal.element[async_head].what) {
        case BX_DBG_ASYNC_JOURNAL_IAC:
         if (!bx_debugger.fast_forward_mode) {
            if (doit)
              dbg_printf ("slave: forcing interrupt %u\n",
                bx_debugger.async_journal.element[async_head].u.iac.val);

            BX_MEM(slave)->dbg_force_interrupt(
              bx_debugger.async_journal.element[async_head].u.iac.val);
          }
          break;
        case BX_DBG_ASYNC_JOURNAL_A20:
          bx_pc_system.set_enable_a20(
            bx_debugger.async_journal.element[async_head].u.a20.val);
          if (BX_MEM(slave)->set_A20)
            BX_MEM(slave)->set_A20(
              bx_debugger.async_journal.element[async_head].u.a20.val);
          break;
        case BX_DBG_ASYNC_JOURNAL_NMI:
        case BX_DBG_ASYNC_JOURNAL_RESET:
        default:
          dbg_printf ( "Error: cosimulateN: unimplemented async event.\n");
      }
      // async event processed, dequeue it
      bx_debugger.async_journal.size--;
      bx_debugger.async_journal.head++;
    }
  } while (slave_icount < master_icount);

  bx_pc_system.INTR = save_INTR; // restore INTR to value after master run

  // At this point, both simulators should be at the same point.  Either
  // they have finished executing for the desired count, or at least for
  // a time quantum.  Check to see if the environments are in sync.
  int iaddr_res;
  if (!bx_debugger.fast_forward_mode) {
    if (bx_debugger.compare_at_sync.iaddr && (iaddr_res = bx_dbg_compare_sim_iaddr())) {
      if (iaddr_res == 1)
        bail_out = 1;
    } else if (bx_debugger.compare_at_sync.cpu && bx_dbg_compare_sim_cpu())
      bail_out = 1;
    else if (bx_debugger.compare_at_sync.memory && bx_dbg_compare_sim_memory())
      bail_out = 1;
  }

  if (bail_out) {
#ifdef DEBUGGER_ERROR
    extern void DEBUGGER_ERROR(void);
    DEBUGGER_ERROR();
#endif

    ret = 0; // didn't complete, stopped
  }
  else {
    count -= master_icount;
    // last icount known to be in sync
    bx_debugger.last_sync_icount += master_icount;
    if (count)
      goto one_time_slice;
    ret = 1; // completed OK
  }

  bx_guard.guard_for &= ~BX_DBG_GUARD_ICOUNT;
  return(ret);
}

int bx_dbg_compare_sim_iaddr(void)
{
  // returns 0 = same, 1 = different, 2 = false diff
  if (BX_CPU(dbg_cpu)->guard_found.laddr != bx_guard_found[1].laddr) {

#ifdef FALSE_DIFF_DETECT
    extern int FALSE_DIFF_DETECT();
    if (FALSE_DIFF_DETECT())
      return 2;
#endif

    dbg_printf (
#if BX_DBG_ICOUNT_SIZE == 32
      "*** Iaddr divergence ***: last know synchronized icount was %lu\n",
      (unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
      "*** Iaddr divergence ***: last know synchronized icount was %Lu\n",
      (unsigned long long) bx_debugger.last_sync_icount
#endif
      );

//    dbg_printf ( "Divergence: sim[0].laddr=%x, sim[1].laddr=%x\n",
//      (unsigned) BX_CPU(dbg_cpu)->guard_found.laddr,
//      (unsigned) bx_guard_found[1].laddr);
    return(1); // different
  }
  return(0); // same
}

bx_bool bx_dbg_compare_sim_cpu(void)
{
  // (mch) Get cpu structures from both simulators

  // Compare the structures (except the descriptor parts of the
  // segment registers
  bx_dbg_cpu_t regs[2];

  BX_MEM(0)->dbg_get_cpu(regs + 0);
  BX_MEM(1)->dbg_get_cpu(regs + 1);

  bx_bool ret = 0;
  bx_bool warn = 0;

  // (mch) Yes I know these are macros. The would have been
  // inner functions if g++ had supported it.
#define TEST_REG(reg, reg_name) \
  do { \
    if (regs[0].reg != regs[1].reg) { \
      printf("COSIM ERROR: [%s] %s: 0x%08x  %s: 0x%08x\n", reg_name, SIM_NAME0, regs[0].reg, SIM_NAME1_STR, regs[1].reg); \
      ret = 1; \
    } \
  } while(0)

#define TEST_REG_WARN(reg, reg_name, mask) \
  do { \
    if ((regs[0].reg & mask) != (regs[1].reg & mask)) { \
      printf("COSIM WARNING: [%s] %s: 0x%08x  %s: 0x%08x\n", reg_name, SIM_NAME0, (regs[0].reg & mask), SIM_NAME1_STR, (regs[1].reg & mask)); \
      warn = 1; \
    } \
  } while(0)

  TEST_REG(eax, "eax");
  TEST_REG(ebx, "ebx");
  TEST_REG(ecx, "ecx");
  TEST_REG(edx, "edx");
  TEST_REG(ebp, "ebp");
  TEST_REG(esi, "esi");
  TEST_REG(edi, "edi");
  TEST_REG(esp, "esp");
        TEST_REG_WARN(eflags, "eflags & CF", 0x1);
#define EFLAGS_MASK (~((1 << 11) | (1 << 7) | (1 << 6) | (1 << 4) | (1 << 2) | (1 << 0)))
  regs[0].eflags &= EFLAGS_MASK;
  regs[1].eflags &= EFLAGS_MASK;
  TEST_REG(eflags, "eflags");
  TEST_REG(eip, "eip");

#define TEST_SEG_REG(reg, reg_name) \
  do { \
    if (regs[0].reg.sel != regs[1].reg.sel || regs[0].reg.valid != regs[1].reg.valid) { \
      printf("COSIM ERROR: [%s] %s: 0x%04x (%d)  %s: 0x%04x (%d)\n", reg_name, SIM_NAME0, regs[0].reg.sel, regs[0].reg.valid, SIM_NAME1_STR, regs[1].reg.sel, regs[1].reg.valid); \
      ret = 1; \
    } \
  } while(0)

  TEST_SEG_REG(cs, "cs");
  TEST_SEG_REG(ss, "ss");
  TEST_SEG_REG(ds, "ds");
  TEST_SEG_REG(es, "es");
  TEST_SEG_REG(fs, "fs");
  TEST_SEG_REG(gs, "gs");
  TEST_SEG_REG(ldtr, "ldtr");
  TEST_SEG_REG(tr, "tr");

  if (regs[0].gdtr.base != regs[1].gdtr.base || regs[0].gdtr.limit != regs[1].gdtr.limit) {
    printf("COSIM ERROR: [gdtr] %s: 0x%08x:0x%04x  %s 0x%08x:0x%04x\n",
           SIM_NAME0, regs[0].gdtr.base, regs[0].gdtr.limit, SIM_NAME1_STR, regs[1].gdtr.base, regs[1].gdtr.limit);
    ret = 1;
  }
  if (regs[0].idtr.base != regs[1].idtr.base || regs[0].idtr.limit != regs[1].idtr.limit) {
    printf("COSIM ERROR: [idtr] %s: 0x%08x:0x%04x  %s 0x%08x:0x%04x\n",
           SIM_NAME0, regs[0].idtr.base, regs[0].idtr.limit, SIM_NAME1_STR, regs[1].idtr.base, regs[1].idtr.limit);
    ret = 1;
  }

  // drX ignored
  // trX ignored

  TEST_REG(cr0, "cr0");
  TEST_REG(cr1, "cr1");
  TEST_REG(cr2, "cr2");
  TEST_REG(cr3, "cr3");
  TEST_REG(cr4, "cr4");

  if (regs[0].inhibit_mask != regs[1].inhibit_mask) {
    printf("COSIM ERROR [inhibit_mask] %s: %d  %s: %d\n",
           SIM_NAME0, regs[0].inhibit_mask, SIM_NAME1_STR, regs[1].inhibit_mask);
    ret = 1;
  }

  if (ret) {
    dbg_printf (
#if BX_DBG_ICOUNT_SIZE == 32
      "*** CPU divergence ***: last know synchronized icount was %lu\n",
      (unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
      "*** CPU divergence ***: last know synchronized icount was %Lu\n",
      (unsigned long long) bx_debugger.last_sync_icount
#endif
      );
  } else if (warn) {
    dbg_printf (
#if BX_DBG_ICOUNT_SIZE == 32
      "=== CPU divergence ===: last know synchronized icount was %lu\n",
      (unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
      "=== CPU divergence ===: last know synchronized icount was %Lu\n",
      (unsigned long long) bx_debugger.last_sync_icount
#endif
      );
#ifdef DEBUGGER_ERROR
    extern void DEBUGGER_ERROR(void);
    DEBUGGER_ERROR();
#endif
  }

  return ret;
}

void clear_dirty_bits (void)
{
  int num_pages = bx_options.memory.Osize->get () * 1024 / 4;
  for (int i = 0; i < num_pages; i++) {
    BX_MEM(0)->dbg_dirty_pages[i] = 0;
    BX_MEM(1)->dbg_dirty_pages[i] = 0;
  }
}

bx_bool always_check_page[128 * 1024 / 4];

void bx_dbg_always_check(Bit32u page_start, bx_bool on)
{
  always_check_page[page_start / (4 * 1024)] = on;
  printf("Forced check on page %08x %s\n",
         page_start, on ? "enabled" : "disabled");
}

bx_bool bx_dbg_compare_sim_memory(void)
{
  bx_bool ret = 0;
  int num_pages = bx_options.memory.Osize->get () * 1024 / 4;

  for (int i = 0; i < num_pages; i++) {
    bx_bool sim0_dirty = BX_MEM(0)->dbg_dirty_pages[i];
    bx_bool sim1_dirty = BX_MEM(1)->dbg_dirty_pages[i];
    Bit32u page_start = i * 1024 * 4;

    if ((sim0_dirty != sim1_dirty) || sim0_dirty || always_check_page[i]) {
      // Page has been written, compare
      // (mch) I'm quite aware of how hackish this is. I don't care.
      extern Bit8u* SIM1_GET_PHYS_PTR(Bit32u page_start);
      Bit8u* sim0_page_vec = bx_mem0.vector + page_start;
      Bit8u* sim1_page_vec = SIM1_GET_PHYS_PTR(page_start);

      if (memcmp(sim0_page_vec, sim1_page_vec, 1024 * 4)) {
        printf("COSIM ERROR  Physical page %08x differs in content\n", page_start);
        for (int j = 0; j < 1024 * 4; j++) {
          if (sim0_page_vec[j] != sim1_page_vec[j]) {
            printf("%08x   %s: %02x  %s: %02x\n",
                   page_start+j, SIM_NAME0, sim0_page_vec[j], SIM_NAME1_STR, sim1_page_vec[j]);
          }
        }
        ret = 1;
      }
    }
  }

  if (ret) {
    dbg_printf (
#if BX_DBG_ICOUNT_SIZE == 32
      "*** Memory divergence ***: last know synchronized icount was %lu\n",
      (unsigned long) bx_debugger.last_sync_icount
#else  // BX_DBG_ICOUNT_SIZE == 64
      "*** Memory divergence ***: last know synchronized icount was %Lu\n",
      (unsigned long long) bx_debugger.last_sync_icount
#endif
      );
  }

  clear_dirty_bits();

  return ret;
}

void bx_dbg_journal_a20_event(unsigned val)
{
  unsigned tail, master;

  if (bx_debugger.master_slave_mode == BX_DBG_SLAVE_MODE ) {
    dbg_printf ( "Error: a20_report: in slave mode.\n");
    bx_dbg_exit(1);
  }

  // Master simulator mode
  if (bx_debugger.async_journal.size >= BX_DBG_ASYNC_JOURNAL_SIZE) {
    dbg_printf ( "Error: async journal full.\n");
    bx_dbg_exit(1);
  }

  if (bx_debugger.async_journal.size == 0) {
    // start off point head & tail at same element
    bx_debugger.async_journal.head = 0;
    tail = bx_debugger.async_journal.tail = 0;
  }
  else {
    tail = bx_debugger.async_journal.tail + 1;
  }
  if (tail >= BX_DBG_ASYNC_JOURNAL_SIZE) {
    dbg_printf ( "Error: a20_report: journal wrapped.\n");
    bx_dbg_exit(0);
  }

  master = bx_debugger.master;
  bx_debugger.async_journal.element[tail].what = BX_DBG_ASYNC_JOURNAL_A20;
  bx_debugger.async_journal.element[tail].icount = bx_guard_found[master].icount;
  bx_debugger.async_journal.element[tail].u.a20.val = val;

  if (bx_debugger.async_journal.size)
    bx_debugger.async_journal.tail++;
  bx_debugger.async_journal.size++;
}

Bit8u bx_dbg_ucmem_read(Bit32u addr)
{
  Bit8u value;
  unsigned head, tail;

  if ( bx_debugger.master_slave_mode == BX_DBG_MASTER_MODE ) {
    if (!bx_debugger.fast_forward_mode) {
      if (bx_debugger.UCmem_journal.size >= BX_DBG_UCMEM_JOURNAL_SIZE) {
        dbg_printf ( "dbg_ucmem_read: journal full.\n");
        bx_dbg_exit(0);
      }

      if (bx_debugger.UCmem_journal.size == 0) {
        // start off point head & tail at same element
        bx_debugger.UCmem_journal.head = 0;
        tail = bx_debugger.UCmem_journal.tail = 0;
      }
      else {
        tail = bx_debugger.UCmem_journal.tail + 1;
      }
      if (tail >= BX_DBG_UCMEM_JOURNAL_SIZE) {
        dbg_printf ( "dbg_ucmem_read: journal wrapped.\n");
        bx_dbg_exit(0);
      }

      value = DEV_vga_mem_read(addr);
      bx_dbg_ucmem_report(addr, 1, BX_READ, value);
      bx_debugger.UCmem_journal.element[tail].op    = BX_READ;
      bx_debugger.UCmem_journal.element[tail].len   = 1;
      bx_debugger.UCmem_journal.element[tail].addr  = addr;
      bx_debugger.UCmem_journal.element[tail].value = value;
      if (bx_debugger.UCmem_journal.size)
        bx_debugger.UCmem_journal.tail++;
      bx_debugger.UCmem_journal.size++;

      if (doit)
        dbg_printf ( "MASTER UCR: head:%u tail%u size:%u\n",
          bx_debugger.UCmem_journal.head,
          bx_debugger.UCmem_journal.tail,
          bx_debugger.UCmem_journal.size);

      return(value);
    } else {
      value = DEV_vga_mem_read(addr);
      return(value);
    }
  }
  else {
    if (bx_debugger.UCmem_journal.size == 0) {
      dbg_printf ( "Error: ucmem_read: journal empty.\n");
      return(0xff);
    }
    head = bx_debugger.UCmem_journal.head;
    value = bx_debugger.UCmem_journal.element[head].value;

    if ((bx_debugger.UCmem_journal.element[head].op != BX_READ) ||
        (bx_debugger.UCmem_journal.element[head].len != 1) ||
        (bx_debugger.UCmem_journal.element[head].addr != addr))
    {
      dbg_printf ( "Error: ucmem_read: out of sync with journal.\n");
      dbg_printf ( "Error: master: op=%1s len=%u addr=0x%x val=0x%x\n",
        (bx_debugger.UCmem_journal.element[head].op==BX_READ) ? "W" : "R",
        (unsigned) bx_debugger.UCmem_journal.element[head].len,
        (unsigned) bx_debugger.UCmem_journal.element[head].addr,
        (unsigned) bx_debugger.UCmem_journal.element[head].value);
      dbg_printf ( "Error: slave:  op=W len=%u addr=0x%x val=0x%x\n",
        (unsigned) 1, (unsigned) addr, (unsigned) value);
      return(0xff);
    }
    // slave UCmem op in sync with journaled master op, delete this entry
    bx_debugger.UCmem_journal.head++;
    bx_debugger.UCmem_journal.size--;
    return(value);
  }
}

void bx_dbg_ucmem_write(Bit32u addr, Bit8u value)
{
  unsigned tail, head;

  if ( bx_debugger.master_slave_mode == BX_DBG_MASTER_MODE ) {
    if (!bx_debugger.fast_forward_mode) {
      if (bx_debugger.UCmem_journal.size >= BX_DBG_UCMEM_JOURNAL_SIZE) {
        dbg_printf ( "dbg_ucmem_write: journal full.\n");
        bx_dbg_exit(0);
      }

      if (bx_debugger.UCmem_journal.size == 0) {
        // start off point head & tail at same element
        bx_debugger.UCmem_journal.head = 0;
        tail = bx_debugger.UCmem_journal.tail = 0;
      }
      else {
        tail = bx_debugger.UCmem_journal.tail + 1;
      }
      if (tail >= BX_DBG_UCMEM_JOURNAL_SIZE) {
        dbg_printf ( "dbg_ucmem_write: journal wrapped.\n");
        bx_dbg_exit(0);
      }

      bx_debugger.UCmem_journal.element[tail].op    = BX_WRITE;
      bx_debugger.UCmem_journal.element[tail].len   = 1;
      bx_debugger.UCmem_journal.element[tail].addr  = addr;
      bx_debugger.UCmem_journal.element[tail].value = value;

      if (bx_debugger.UCmem_journal.size)
        bx_debugger.UCmem_journal.tail++;
      bx_debugger.UCmem_journal.size++;
      DEV_vga_mem_write(addr, value);
      bx_dbg_ucmem_report(addr, 1, BX_WRITE, value);
    } else {
      DEV_vga_mem_write(addr, value);
    }
  }
  else {
    if (bx_debugger.UCmem_journal.size == 0) {
      dbg_printf ( "Error: ucmem_write: journal empty.\n");
      return;
    }
    head = bx_debugger.UCmem_journal.head;

    if ((bx_debugger.UCmem_journal.element[head].op != BX_WRITE) ||
        (bx_debugger.UCmem_journal.element[head].len != 1) ||
        (bx_debugger.UCmem_journal.element[head].addr != addr) ||
        (bx_debugger.UCmem_journal.element[head].value != value) )
    {
      dbg_printf ( "Error: ucmem_write: out of sync with journal.\n");
      dbg_printf ( "Error: master: op=%1s len=%u addr=0x%x val=0x%x\n",
        (bx_debugger.UCmem_journal.element[head].op==BX_WRITE) ? "W" : "R",
        (unsigned) bx_debugger.UCmem_journal.element[head].len,
        (unsigned) bx_debugger.UCmem_journal.element[head].addr,
        (unsigned) bx_debugger.UCmem_journal.element[head].value);
      dbg_printf ( "Error: slave:  op=W len=%u addr=0x%x val=0x%x\n",
        (unsigned) 1, (unsigned) addr, (unsigned) value);
      return;
    }
    // slave UCmem op in sync with journaled master op, delete this entry
    bx_debugger.UCmem_journal.head++;
    bx_debugger.UCmem_journal.size--;
  }
}

void bx_dbg_async_pin_request(unsigned what, bx_bool val)
{
  // Request from IO devices for change in pin external to CPU.
  // This is pended until CPU ack's with bx_dbg_async_pin_ack().

  if (bx_debugger.master_slave_mode != BX_DBG_MASTER_MODE) {
    dbg_printf ( "Error: dbg_async_pin_request not in master mode.\n");
    bx_dbg_exit(1);
  }

  switch (what) {
    case BX_DBG_ASYNC_PENDING_A20:
      // Q pending status
      bx_guard.async_changes_pending.which |= BX_DBG_ASYNC_PENDING_A20;
      bx_guard.async_changes_pending.a20    = val;
      return;

    case BX_DBG_ASYNC_PENDING_RESET:
    case BX_DBG_ASYNC_PENDING_NMI:
    default:
      dbg_printf ( "Error: set_async_pin: unhandled case.\n");
      bx_dbg_exit(1);
  }
}


void bx_dbg_async_pin_ack(unsigned what, bx_bool val)
{
  // Acknowledgement from master simulator for pending change in pin
  // external to CPU.

  if (bx_debugger.master_slave_mode != BX_DBG_MASTER_MODE) {
    dbg_printf ( "Error: dbg_async_pin_ack: not master mode.\n");
    bx_dbg_exit(1);
  }

  switch (what) {
    case BX_DBG_ASYNC_PENDING_A20:
      // get rid of pending status
      bx_guard.async_changes_pending.which &= ~BX_DBG_ASYNC_PENDING_A20;
      // notify pc_system of change
      bx_pc_system.set_enable_a20(val);
      if (BX_CPU(bx_debugger.master)->set_A20)
        BX_CPU(bx_debugger.master)->set_A20(val);
      bx_dbg_journal_a20_event(val);
      return;

    case BX_DBG_ASYNC_PENDING_RESET:
    case BX_DBG_ASYNC_PENDING_NMI:
    default:
      dbg_printf ( "Error: set_async_pin: unhandled case.\n");
      bx_dbg_exit(1);
  }
}

Bit32u bx_dbg_inp(Bit16u addr, unsigned len)
{
  Bit32u value;
  unsigned tail, head;

  if ( bx_debugger.master_slave_mode == BX_DBG_MASTER_MODE ) {
    if (!bx_debugger.fast_forward_mode) {
      if (bx_debugger.IO_journal.size >= BX_DBG_IO_JOURNAL_SIZE) {
        dbg_printf ( "dbg_inp: journal full.\n");
        bx_dbg_exit(0);
      }

      if (bx_debugger.IO_journal.size == 0) {
        // start off point head & tail at same element
        bx_debugger.IO_journal.head = 0;
        tail = bx_debugger.IO_journal.tail = 0;
      }
      else {
        tail = bx_debugger.IO_journal.tail + 1;
      }
      if (tail >= BX_DBG_IO_JOURNAL_SIZE) {
        dbg_printf ( "dbg_inp: journal wrapped.\n");
        bx_dbg_exit(0);
      }

      value = bx_pc_system.inp(addr, len);
      bx_debugger.IO_journal.element[tail].op    = BX_READ;
      bx_debugger.IO_journal.element[tail].len   = (Bit8u) len;
      bx_debugger.IO_journal.element[tail].addr  = addr;
      bx_debugger.IO_journal.element[tail].value = value;
      if (bx_debugger.IO_journal.size)
        bx_debugger.IO_journal.tail++;
      bx_debugger.IO_journal.size++;
//dbg_printf ( "MASTER IN: head:%u tail%u size:%u\n",
//  bx_debugger.IO_journal.head,
//  bx_debugger.IO_journal.tail,
//  bx_debugger.IO_journal.size);
      return(value);
    } else {
      value = bx_pc_system.inp(addr, len);
      return(value);
    }
  }
  else {
    if (bx_debugger.IO_journal.size == 0) {
      dbg_printf ( "Error: dbg_inp: journal empty.\n");
      return(0xffffffff);
    }
    head = bx_debugger.IO_journal.head;
    value = bx_debugger.IO_journal.element[head].value;

    if ((bx_debugger.IO_journal.element[head].op != BX_READ) ||
        (bx_debugger.IO_journal.element[head].len != len) ||
        (bx_debugger.IO_journal.element[head].addr != addr) ) {
      dbg_printf ( "Error: dbg_inp: out of sync with journal.\n");
      dbg_printf ( "Error: master: op=%3s len=%u addr=0x%x\n",
        (bx_debugger.IO_journal.element[head].op==BX_WRITE) ? "OUT" : "IN",
        (unsigned) bx_debugger.IO_journal.element[head].len,
        (unsigned) bx_debugger.IO_journal.element[head].addr);
      dbg_printf ( "Error: slave:  op=OUT len=%u addr=0x%x\n",
        (unsigned) len, (unsigned) addr);
      return(0xffffffff);
    }
    // slave IO op in sync with journaled master op, delete this entry
    bx_debugger.IO_journal.head++;
    bx_debugger.IO_journal.size--;
// dbg_printf ( "SLAVE   IN: head:%u tail%u size:%u\n",
//   bx_debugger.IO_journal.head,
//   bx_debugger.IO_journal.tail,
//   bx_debugger.IO_journal.size);
    return(value);
  }
}

void bx_dbg_outp(Bit16u addr, Bit32u value, unsigned len)
{
  unsigned tail, head;

  if ( bx_debugger.master_slave_mode == BX_DBG_MASTER_MODE ) {
    if (!bx_debugger.fast_forward_mode) {
      if (bx_debugger.IO_journal.size >= BX_DBG_IO_JOURNAL_SIZE) {
        dbg_printf ( "dbg_outp: IO journal full.\n");
        bx_dbg_exit(0);
      }

      if (bx_debugger.IO_journal.size == 0) {
        // start off point head & tail at same element
        bx_debugger.IO_journal.head = 0;
        tail = bx_debugger.IO_journal.tail = 0;
      }
      else {
        tail = bx_debugger.IO_journal.tail + 1;
      }
      if (tail >= BX_DBG_IO_JOURNAL_SIZE) {
        dbg_printf ( "dbg_outp: IO journal wrapped.\n");
        bx_dbg_exit(0);
      }

      bx_debugger.IO_journal.element[tail].op    = BX_WRITE;
      bx_debugger.IO_journal.element[tail].len   = (Bit8u) len;
      bx_debugger.IO_journal.element[tail].addr  = addr;
      bx_debugger.IO_journal.element[tail].value = value;
      if (bx_debugger.IO_journal.size)
        bx_debugger.IO_journal.tail++;
      bx_debugger.IO_journal.size++;
      bx_pc_system.outp(addr, value, len);
      if (doit)
        dbg_printf ( "master: IO journal size now %u\n", bx_debugger.IO_journal.size);
    } else {
      bx_pc_system.outp(addr, value, len);
    }
  }
  else {
    if (bx_debugger.IO_journal.size == 0) {
      dbg_printf ( "Error: dbg_outp: journal empty.\n");
      return;
    }
    head = bx_debugger.IO_journal.head;

    if ((bx_debugger.IO_journal.element[head].op != BX_WRITE) ||
        (bx_debugger.IO_journal.element[head].len != len) ||
        (bx_debugger.IO_journal.element[head].addr != addr) ||
        (bx_debugger.IO_journal.element[head].value != value) ) {
      dbg_printf ( "Error: dbg_outp: out of sync with journal.\n");
      dbg_printf ( "Error: master: op=%3s len=%u addr=0x%x val=0x%x\n",
        (bx_debugger.IO_journal.element[head].op==BX_WRITE) ? "OUT" : "IN",
        (unsigned) bx_debugger.IO_journal.element[head].len,
        (unsigned) bx_debugger.IO_journal.element[head].addr,
        (unsigned) bx_debugger.IO_journal.element[head].value);
      dbg_printf ( "Error: slave:  op=OUT len=%u addr=0x%x val=0x%x\n",
        (unsigned) len, (unsigned) addr, (unsigned) value);
      return;
    }
    // slave IO op in sync with journaled master op, delete this entry
    bx_debugger.IO_journal.head++;
    bx_debugger.IO_journal.size--;
    if (doit)
      dbg_printf ( "slave: IO journal size now %u\n", bx_debugger.IO_journal.size);
  }
}

void bx_dbg_raise_HLDA(void)
{
  dbg_printf ( "dbg_HLDA called\n");
  bx_dbg_exit(0);
}

Bit8u bx_dbg_IAC(void)
{
  // Convience routine.  bochs skips this, and calls the PIC code
  // directly.  This is for other simulators to interface to the
  // the PIC code.
  unsigned iac;

  iac = BX_PIC_AIC ();
  return(iac);
}

void bx_dbg_set_INTR(bx_bool b)
{
  if ( bx_debugger.master_slave_mode == BX_DBG_SLAVE_MODE ) {
    dbg_printf ( "Error: set_INTR in slave mode.\n");
    bx_dbg_exit(1);
  }

  bx_pc_system.INTR = b;
  BX_CPU(bx_debugger.master)->set_INTR(b);
}

#endif
