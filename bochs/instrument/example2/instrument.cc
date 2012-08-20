/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2009-2012 Stanislav Shwartsman
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


#include <assert.h>

#include "bochs.h"
#include "cpu/cpu.h"

#define BX_IA_STATS_ENTRIES (BX_IA_LAST*2) /* /r and /m form */

static struct bx_instr_ia_stats {
   bx_bool active;
   Bit32u ia_cnt[BX_IA_STATS_ENTRIES];
   Bit32u total_cnt;
   Bit32u interrupts;
   Bit32u exceptions;
} *ia_stats;

static logfunctions *instrument_log = new logfunctions ();
#define LOG_THIS instrument_log->

void bx_instr_initialize(unsigned cpu)
{
  assert(cpu < BX_SMP_PROCESSORS);

  if (ia_stats == NULL)
      ia_stats = new bx_instr_ia_stats[BX_SMP_PROCESSORS];

  ia_stats[cpu].active = 0;

  fprintf(stderr, "Initialize cpu %d instrumentation module\n", cpu);
}

void bx_instr_reset(unsigned cpu, unsigned type)
{
  ia_stats[cpu].active    = 1;
  ia_stats[cpu].total_cnt = 0;

  for(int n=0; n < BX_IA_STATS_ENTRIES; n++)
    ia_stats[cpu].ia_cnt[n] = 0;

  ia_stats[cpu].interrupts = ia_stats[cpu].exceptions = 0;
}

void bx_instr_interrupt(unsigned cpu, unsigned vector)
{
  if(ia_stats[cpu].active) ia_stats[cpu].interrupts++;
}

void bx_instr_exception(unsigned cpu, unsigned vector, unsigned error_code)
{
  if(ia_stats[cpu].active) ia_stats[cpu].exceptions++;
}

void bx_instr_hwinterrupt(unsigned cpu, unsigned vector, Bit16u cs, bx_address eip)
{
  if(ia_stats[cpu].active) ia_stats[cpu].interrupts++;
}

#define IA_CNT_DUMP_THRESHOLD 100000000 /* 100M */

void bx_instr_before_execution(unsigned cpu, bxInstruction_c *i)
{
  if(ia_stats[cpu].active) {
    ia_stats[cpu].ia_cnt[i->getIaOpcode() * 2 + !!i->modC0()]++;
    ia_stats[cpu].total_cnt++;

    if (ia_stats[cpu].total_cnt > IA_CNT_DUMP_THRESHOLD) {
      printf("Dump IA stats for CPU %d\n", cpu);
      printf("----------------------------------------------------------\n");
      printf("Interrupts: %d, Exceptions: %d\n", ia_stats[cpu].interrupts, ia_stats[cpu].exceptions);
      while(1) {
        Bit32u max = 0, max_index = 0;
        for (int n=0;n < BX_IA_STATS_ENTRIES; n++) {
          if (ia_stats[cpu].ia_cnt[n] > max) {
            max = ia_stats[cpu].ia_cnt[n];
            max_index = n;
          }
        }
        if (max == 0) break;
        printf("%s /%c: %f%%\n", get_bx_opcode_name(max_index/2), (max_index & 1) ? 'm' : 'r', ia_stats[cpu].ia_cnt[max_index] * 100.0f / ia_stats[cpu].total_cnt);
        ia_stats[cpu].ia_cnt[max_index] = 0;
      }
      ia_stats[cpu].interrupts = ia_stats[cpu].exceptions = ia_stats[cpu].total_cnt = 0;
    }
  }
}
