/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_CPUSTATS_H
#define BX_CPUSTATS_H

#define InstrumentICACHE 0
#define InstrumentTLB 0
#define InstrumentTLBFlush 0
#define InstrumentStackPrefetch 0
#define InstrumentSMC 0

// indicate if any of the CPU statistics was compiled in
#define InstrumentCPU (InstrumentICACHE + InstrumentTLB + InstrumentTLBFlush + InstrumentStackPrefetch + InstrumentSMC)

struct bx_cpu_statistics
{
  // icache statistics
  Bit64u iCacheLookups;
  Bit64u iCachePrefetch;
  Bit64u iCacheMisses;

  // tlb lookup statistics
  Bit64u tlbLookups;
  Bit64u tlbMisses;

  // tlb flush statistics
  Bit64u tlbGlobalFlushes;
  Bit64u tlbNonGlobalFlushes;

  // stack prefetch statistics
  Bit64u stackPrefetch;

  // self modifying code statistics
  Bit64u smc;

  bx_cpu_statistics():
      iCacheLookups(0), iCachePrefetch(0), iCacheMisses(0),
      tlbLookups(0), tlbMisses(0),
      tlbGlobalFlushes(0), tlbNonGlobalFlushes(0),
      stackPrefetch(0), smc(0) {}
  
};

#define INC_CPU_STAT(stat) INC_STAT(BX_CPU_THIS_PTR stats -> stat)

#if InstrumentICACHE
  #define INC_ICACHE_STAT(stat) INC_CPU_STAT(stat)
#else
  #define INC_ICACHE_STAT(stat)
#endif

#if InstrumentTLBFlush
  #define INC_TLBFLUSH_STAT(stat) INC_CPU_STAT(stat)
#else
  #define INC_TLBFLUSH_STAT(stat)
#endif

#if InstrumentTLB
  #define INC_TLB_STAT(stat) INC_CPU_STAT(stat)
#else
  #define INC_TLB_STAT(stat)
#endif

#if InstrumentStackPrefetch
  #define INC_STACK_PREFETCH_STAT(stat) INC_CPU_STAT(stat)
#else
  #define INC_STACK_PREFETCH_STAT(stat)
#endif

#if InstrumentSMC
  #define INC_SMC_STAT(stat) INC_CPU_STAT(stat)
#else
  #define INC_SMC_STAT(stat)
#endif

#endif
