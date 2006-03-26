/////////////////////////////////////////////////////////////////////////
// $Id: smm.cc,v 1.7 2006-03-26 18:58:01 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2006 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <stl at fidonet.org.il>
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_CPU_LEVEL >= 3

#if BX_SUPPORT_X86_64==0
#define RIP EIP
#endif

void BX_CPU_C::RSM(bxInstruction_c *i)
{
  /* If we are not in System Management Mode, then #UD should be generated */
  if (! BX_CPU_THIS_PTR smm_mode()) {
    UndefinedOpcode(i);
  }

  invalidate_prefetch_q();
 
  BX_INFO(("RSM: System Management Mode not implemented yet"));

  BX_CPU_THIS_PTR nmi_disable = 0;
  BX_CPU_THIS_PTR in_smm = 0;

  UndefinedOpcode(i);
}

void BX_CPU_C::enter_system_management_mode(void)
{
  invalidate_prefetch_q();

  // all status flags at known values, use BX_CPU_THIS_PTR eflags structure
  BX_CPU_THIS_PTR lf_flags_status = 0x000000;

  // status and control flags register set
  BX_CPU_THIS_PTR setEFlags(0x2); // Bit1 is always set

  BX_CPU_THIS_PTR prev_eip = RIP = 0x000080000;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value = BX_CPU_THIS_PTR smbase << 4;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.index = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.ti    = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.rpl   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid    = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment  = 1;  /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type     = 11; /* executable/readable/access code segment */

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.executable   = 1; /* data/stack segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.c_ed         = 0; /* normal expand up */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.r_w          = 1; /* writeable */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.a            = 1; /* accessed */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = BX_CPU_THIS_PTR smbase;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xffffffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b = 0; /* 16bit default size */
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l   = 0; /* 16bit default size */
#endif

#if BX_SUPPORT_ICACHE
  BX_CPU_THIS_PTR fetchModeMask = createFetchModeMask(BX_CPU_THIS);
#endif

  /* DS (Data Segment) and descriptor cache */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = 0x0000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.index = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.ti    = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.rpl   = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid    = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment  = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.type     = 3; /* read/write/access */

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.executable   = 0; /* data/stack segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.c_ed         = 0; /* normal expand up */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.r_w          = 1; /* writeable */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.a            = 1; /* accessed */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base         = 0x00000000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled = 0xffffffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.avl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g   = 0; /* byte granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.d_b = 0; /* 16bit default size */
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.l   = 0; /* 16bit default size */
#endif

  // use DS segment as template for the others
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS] = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES] = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS] = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS] = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS];

  // DR7
  BX_CPU_THIS_PTR dr7 = 0x00000400;

  // CR0 - PE, EM, TS, and PG flags set to 0; others unmodified
  BX_CPU_THIS_PTR cr0.pe = 0; // real mode (bit 0)
  BX_CPU_THIS_PTR cr0.em = 0; // emulate math coprocessor (bit 2)
  BX_CPU_THIS_PTR cr0.ts = 0; // no task switch (bit 3)
  BX_CPU_THIS_PTR cr0.pg = 0; // paging disabled (bit 31)

  BX_CPU_THIS_PTR cr0.val32 &= 0x7ffffff2;

  // paging mode was changed - flush TLB
  TLB_flush(1); // 1 = Flush Global entries also

#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR cr4.setRegister(0);
#endif

  // EFER.LME = 0, EFER.LME = 1
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR msr.lme = 0;
  BX_CPU_THIS_PTR msr.lma = 0;
#endif
}

bx_bool BX_CPU_C::smram_write(bx_phy_address a20addr)
{
  return 1; // for now
}

#endif /* BX_CPU_LEVEL >= 3 */
