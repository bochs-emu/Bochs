/////////////////////////////////////////////////////////////////////////
// $Id: smm.cc,v 1.9 2006-03-27 20:09:37 sshwarts Exp $
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

  // save processor state to the SMRAM
  BX_CPU_THIS_PTR smram_save_state();

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
  BX_CPU_THIS_PTR updateFetchModeMask();
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

#define SMRAM_TRANSLATE(addr) (((0x8000 - (addr)) >> 2) - 1)

#if BX_SUPPORT_X86_64

void BX_CPU_C::smram_save_state(void)
{
  Bit32u saved_state[256], i;

  // reset reserved bits
  for(i=0;i<256;i++) saved_state[i] = 0;

  saved_state[SMRAM_TRANSLATE(0x7ff8)] = BX_CPU_THIS_PTR cr0.val32;
  saved_state[SMRAM_TRANSLATE(0x7ff4)] = BX_CPU_THIS_PTR cr3 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7ff0)] = BX_CPU_THIS_PTR cr3 & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7fe8)] = read_eflags();
  saved_state[SMRAM_TRANSLATE(0x7fe0)] = BX_CPU_THIS_PTR get_EFER();
  saved_state[SMRAM_TRANSLATE(0x7fdc)] = RIP >> 32;
  saved_state[SMRAM_TRANSLATE(0x7fd8)] = RIP & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7fd0)] = BX_CPU_THIS_PTR dr6;
  saved_state[SMRAM_TRANSLATE(0x7fc8)] = BX_CPU_THIS_PTR dr7;
  saved_state[SMRAM_TRANSLATE(0x7fc4)] = BX_CPU_THIS_PTR tr.selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fc0)] = BX_CPU_THIS_PTR ldtr.selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fbc)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fb8)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fb4)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fb0)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fac)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fa8)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;
  /* base+7fa4 is I/O MISC (4 byte) */
  /* base+7f9c is I/O MEM ADDRESS (8 byte) */
  saved_state[SMRAM_TRANSLATE(0x7f98)] = RDI >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f94)] = RDI & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f90)] = RSI >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f8c)] = RSI & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f88)] = RBP >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f84)] = RBP & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f80)] = RSP >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f7c)] = RSP & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f78)] = RBX >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f74)] = RBX & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f70)] = RDX >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f6c)] = RDX & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f68)] = RCX >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f64)] = RCX & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f60)] = RAX >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f5c)] = RAX & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f58)] =  R8 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f54)] =  R8 & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f50)] =  R9 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f4c)] =  R9 & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f48)] = R10 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f44)] = R10 & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f40)] = R11 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f3c)] = R11 & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f38)] = R12 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f34)] = R12 & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f30)] = R13 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f2c)] = R13 & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f28)] = R14 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f24)] = R14 & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f20)] = R15 >> 32;
  saved_state[SMRAM_TRANSLATE(0x7f1c)] = R15 & 0xffffffff;
  /* base+0x7f1c to base+0x7f04 is reserved */
  /* base+0x7f02 is Auto HALT restart field (2 byte) */
  /* base+0x7f00 is I/O restart field (2 byte) */
  /* base+0x7efc is SMM Revision Identifier field  */
  saved_state[SMRAM_TRANSLATE(0x7ef8)] = BX_CPU_THIS_PTR smbase;
  /* base+0x7ff4 to base+0x7fa8 is reserved */
  /* base+0x7fa4 is LDT Info (4 byte) */
  saved_state[SMRAM_TRANSLATE(0x7fa0)] = 
    BX_CPU_THIS_PTR ldtr.cache.u.segment.limit;
  saved_state[SMRAM_TRANSLATE(0x7f9c)] = 
    BX_CPU_THIS_PTR ldtr.cache.u.segment.base & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f98)] = BX_CPU_THIS_PTR idtr.limit;
  saved_state[SMRAM_TRANSLATE(0x7f94)] = BX_CPU_THIS_PTR idtr.base & 0xffffffff;
  saved_state[SMRAM_TRANSLATE(0x7f90)] = BX_CPU_THIS_PTR gdtr.limit;
  saved_state[SMRAM_TRANSLATE(0x7f8c)] = BX_CPU_THIS_PTR gdtr.base & 0xffffffff;
  /* base+0x7e80 to base+0x7e44 is reserved */
  saved_state[SMRAM_TRANSLATE(0x7e40)] = BX_CPU_THIS_PTR cr4.getRegister();
  /* base+0x7e3c to base+0x7df0 is reserved */
  /* base+0x7de8 is I/O RIP (8 bytes) */
  /* base+0x7de4 to base+0x7ddc is reserved */
  saved_state[SMRAM_TRANSLATE(0x7dd8)] = BX_CPU_THIS_PTR idtr.base >> 32;
  saved_state[SMRAM_TRANSLATE(0x7fd4)] = 
    BX_CPU_THIS_PTR ldtr.cache.u.segment.base >> 32;
  saved_state[SMRAM_TRANSLATE(0x7dd0)] = BX_CPU_THIS_PTR gdtr.base >> 32;
  /* base+0x7dcc to base+0x7c00 is reserved */

  bx_phy_address base = (BX_CPU_THIS_PTR smbase + 0x8000);
  base += 0x8000;

  // could be optimized with writing of only non-reserved bytes
  for(i=0;i<256;i++) {
    base -= 4;
    BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, base, 4, &saved_state[i]);
  }
}

bx_bool BX_CPU_C::smram_restore_state(void)
{
  Bit32u saved_state[256], i;

  // reset reserved bits
  for(i=0;i<256;i++) saved_state[i] = 0;

  bx_phy_address base = (BX_CPU_THIS_PTR smbase + 0x8000);
  base += 0x8000;

  // could be optimized with reading of only non-reserved bytes
  for(i=0;i<256;i++) {
    base -= 4;
    BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, base, 4, &saved_state[i]);
  }

  return 1;
}

#else /* BX_SUPPORT_X86_64 == 0 */

void BX_CPU_C::smram_save_state(void)
{
  Bit32u saved_state[128], i;

  // reset reserved bits
  for(i=0;i<128;i++) saved_state[i] = 0;

  saved_state[SMRAM_TRANSLATE(0x7ffc)] = BX_CPU_THIS_PTR cr0.val32;
  saved_state[SMRAM_TRANSLATE(0x7ff8)] = BX_CPU_THIS_PTR cr3;
  saved_state[SMRAM_TRANSLATE(0x7ff4)] = read_eflags();
  saved_state[SMRAM_TRANSLATE(0x7ff0)] = EIP;
  saved_state[SMRAM_TRANSLATE(0x7fec)] = EDI;
  saved_state[SMRAM_TRANSLATE(0x7fe8)] = ESI;
  saved_state[SMRAM_TRANSLATE(0x7fe4)] = EBP;
  saved_state[SMRAM_TRANSLATE(0x7fe0)] = ESP;
  saved_state[SMRAM_TRANSLATE(0x7fdc)] = EBX;
  saved_state[SMRAM_TRANSLATE(0x7fd8)] = EDX;
  saved_state[SMRAM_TRANSLATE(0x7fd4)] = ECX;
  saved_state[SMRAM_TRANSLATE(0x7fd0)] = EAX;
  saved_state[SMRAM_TRANSLATE(0x7fcc)] = BX_CPU_THIS_PTR dr6;
  saved_state[SMRAM_TRANSLATE(0x7fc8)] = BX_CPU_THIS_PTR dr7;
  saved_state[SMRAM_TRANSLATE(0x7fc4)] = BX_CPU_THIS_PTR tr.selector.value;
  /* base+0x7fc0 is reserved field */
  saved_state[SMRAM_TRANSLATE(0x7fbc)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fb8)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fb4)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fb0)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fac)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  saved_state[SMRAM_TRANSLATE(0x7fa8)] =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;
  /* base+0x7fa4 is I/O state field */
  /* base+0x7fa0 is I/O Memory Address field */
  /* base+0x7f9c to base+0x7f04 is reserved */
  /* base+0x7f02 is Auto HALT restart field (2 byte) */
  /* base+0x7f00 is I/O restart field (2 byte) */
  /* base+0x7efc is SMM Revision Identifier field  */
  saved_state[SMRAM_TRANSLATE(0x7ef8)] = BX_CPU_THIS_PTR smbase;
  /* base+0x7ef4 to base+0x7e00 is reserved */

  bx_phy_address base = (BX_CPU_THIS_PTR smbase + 0x8000);
  base += 0x8000;

  // could be optimized with writing of only non-reserved bytes
  for(i=0;i<128;i++) {
    base -= 4;
    BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, base, 4, &saved_state[i]);
  }
}

bx_bool BX_CPU_C::smram_restore_state(void)
{
  Bit32u saved_state[128], i;

  // reset reserved bits
  for(i=0;i<128;i++) saved_state[i] = 0;

  bx_phy_address base = (BX_CPU_THIS_PTR smbase + 0x8000);
  base += 0x8000;

  // could be optimized with reading of only non-reserved bytes
  for(i=0;i<128;i++) {
    base -= 4;
    BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, base, 4, &saved_state[i]);
  }

  SetCR0(saved_state[SMRAM_TRANSLATE(0x7ffc)]);
  CR3_change(saved_state[SMRAM_TRANSLATE(0x7ff8)]);
  BX_CPU_THIS_PTR setEFlags(saved_state[SMRAM_TRANSLATE(0x7ff4)]);

  EIP = saved_state[SMRAM_TRANSLATE(0x7ff0)];
  EDI = saved_state[SMRAM_TRANSLATE(0x7fec)];
  ESI = saved_state[SMRAM_TRANSLATE(0x7fe8)];
  EBP = saved_state[SMRAM_TRANSLATE(0x7fe4)];
  ESP = saved_state[SMRAM_TRANSLATE(0x7fe0)];
  EBX = saved_state[SMRAM_TRANSLATE(0x7fdc)];
  EDX = saved_state[SMRAM_TRANSLATE(0x7fd8)];
  ECX = saved_state[SMRAM_TRANSLATE(0x7fd4)];
  EAX = saved_state[SMRAM_TRANSLATE(0x7fd0)];

  BX_CPU_THIS_PTR dr6 = saved_state[SMRAM_TRANSLATE(0x7fcc)];
  BX_CPU_THIS_PTR dr7 = saved_state[SMRAM_TRANSLATE(0x7fc8)];

//  saved_state[SMRAM_TRANSLATE(0x7fc4)] = BX_CPU_THIS_PTR tr.selector.value;
//  /* base+0x7fc0 is reserved field */
//  saved_state[SMRAM_TRANSLATE(0x7fbc)] =
//    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
//  saved_state[SMRAM_TRANSLATE(0x7fb8)] =
//    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
//  saved_state[SMRAM_TRANSLATE(0x7fb4)] =
//    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
//  saved_state[SMRAM_TRANSLATE(0x7fb0)] =
//    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
//  saved_state[SMRAM_TRANSLATE(0x7fac)] =
//    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
//  saved_state[SMRAM_TRANSLATE(0x7fa8)] =
//    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;

  /* base+0x7fa4 is I/O state field */
  /* base+0x7fa0 is I/O Memory Address field */
  /* base+0x7f9c to base+0x7f04 is reserved */
  /* base+0x7f02 is Auto HALT restart field (2 byte) */
  /* base+0x7f00 is I/O restart field (2 byte) */
  /* base+0x7efc is SMM Revision Identifier field  */
  BX_CPU_THIS_PTR smbase = saved_state[SMRAM_TRANSLATE(0x7ef8)];
  /* base+0x7ef4 to base+0x7e00 is reserved */

  return 1;
}

#endif /* BX_SUPPORT_X86_64 */

#endif /* BX_CPU_LEVEL >= 3 */
