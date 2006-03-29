/////////////////////////////////////////////////////////////////////////
// $Id: smm.cc,v 1.12 2006-03-29 19:16:24 sshwarts Exp $
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

//
// Some of the CPU field must be saved and restored in order to continue the
// simulation correctly after the RSM instruction:
//
//      ---------------------------------------------------------------
//
// 1. General purpose registers: EAX-EDI, R8-R15
// 2. EIP, RFLAGS
// 3. Segment registers CS, DS, SS, ES, FS, GS
//    fields: valid      - not required, initialized according to selector value
//            p          - must be saved/restored
//            dpl        - must be saved/restored
//            segment    - must be 1 for seg registers, not required to save
//            type       - must be 0 for seg registers, not required to save
//            executable - must be saved/restored
//            c_ed       - must be saved/restored
//            r_w        - must be saved/restored
//            a          - must be saved/restored
//            base       - must be saved/restored
//            limit      - must be saved/restored
//            g          - must be saved/restored
//            d_b        - must be saved/restored
//            l          - must be saved/restored
//            avl        - must be saved/restored
// 4. GDTR, IDTR
//     fields: base, limit
// 5. LDTR, TR
//     fields: base, limit, something more ?
// 6. Debug Registers DR0-DR7, only DR6 and DR7 are saved
// 7. Control Regsiters: CR0, CR1 is always 0, CR2 is NOT saved, CR3, CR4
// 8. SMBASE
// 9. MSR/FPU/XMM/APIC are NOT saved accoring to Intel docs
//

#define SMM_SAVE_STATE_MAP_SIZE 128

void BX_CPU_C::RSM(bxInstruction_c *i)
{
  /* If we are not in System Management Mode, then #UD should be generated */
  if (! BX_CPU_THIS_PTR smm_mode()) {
    BX_INFO(("RSM not in System Management Mode !"));
    UndefinedOpcode(i);
  }

  invalidate_prefetch_q();
 
  BX_INFO(("RSM: System Management Mode not implemented yet"));

  UndefinedOpcode(i); // remove later, when SMM become stable

  Bit32u saved_state[SMM_SAVE_STATE_MAP_SIZE], n;

  // reset reserved bits
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) saved_state[n] = 0;

  bx_phy_address base = BX_CPU_THIS_PTR smbase + 0x10000;
  // could be optimized with reading of only non-reserved bytes
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) {
    base -= 4;
    BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, base, 4, &saved_state[n]);
  }

  // restore the CPU state from SMRAM
  if (! smram_restore_state(saved_state)) {
    BX_PANIC(("RSM: Incorrect state when restoring CPU state - shutdown !"));
  }

  BX_CPU_THIS_PTR nmi_disable = 0;
  BX_CPU_THIS_PTR in_smm = 0;
}

void BX_CPU_C::enter_system_management_mode(void)
{
  invalidate_prefetch_q();

  Bit32u saved_state[SMM_SAVE_STATE_MAP_SIZE], n;

  // reset reserved bits
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) saved_state[n] = 0;

  // prepare CPU state to be saved in the SMRAM
  BX_CPU_THIS_PTR smram_save_state(saved_state);

  bx_phy_address base = BX_CPU_THIS_PTR smbase + 0x10000;
  // could be optimized with reading of only non-reserved bytes
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) {
    base -= 4;
    BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, base, 4, &saved_state[n]);
  }

  BX_CPU_THIS_PTR setEFlags(0x2); // Bit1 is always set
  BX_CPU_THIS_PTR prev_eip = RIP = 0x000080000;
  BX_CPU_THIS_PTR dr7 = 0x00000400;

  // CR0 - PE, EM, TS, and PG flags set to 0; others unmodified
  BX_CPU_THIS_PTR cr0.pe = 0; // real mode (bit 0)
  BX_CPU_THIS_PTR cr0.em = 0; // emulate math coprocessor (bit 2)
  BX_CPU_THIS_PTR cr0.ts = 0; // no task switch (bit 3)
  BX_CPU_THIS_PTR cr0.pg = 0; // paging disabled (bit 31)
  BX_CPU_THIS_PTR cr0.val32 &= 0x7ffffff2;

  BX_CPU_THIS_PTR cpu_mode = BX_MODE_IA32_REAL;

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
}

#define SMRAM_TRANSLATE(addr)        (((0x8000 - (addr)) >> 2) - 1)
#define SMM_SAVED_STATE(state, addr) (state[SMRAM_TRANSLATE(addr)])

#if BX_SUPPORT_X86_64

// for x86-64 configuration using AMD Athlon 64 512-byte SMM save state map
#define SMM_REVISION_ID 0x00000000

void BX_CPU_C::smram_save_state(Bit32u *saved_state)
{
  // --- General Purpose Registers --- //
  SMM_SAVED_STATE(saved_state, 0x7ffc) = RAX >> 32;
  SMM_SAVED_STATE(saved_state, 0x7ff8) = RAX & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7ff4) = RCX >> 32;
  SMM_SAVED_STATE(saved_state, 0x7ff0) = RCX & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fec) = RDX >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fe8) = RDX & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fe4) = RBX >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fe0) = RBX & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fdc) = RSP >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fd8) = RSP & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fd4) = RBP >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fd0) = RBP & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fcc) = RSI >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fc8) = RSI & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fc4) = RDI >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fc0) = RDI & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fbc) =  R8 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fb8) =  R8 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fb4) =  R9 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fb0) =  R9 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fac) = R10 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fa8) = R10 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7fa4) = R11 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7fa0) = R11 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7f9c) = R12 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7f98) = R12 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7f94) = R13 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7f90) = R13 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7f8c) = R14 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7f88) = R14 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7f84) = R15 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7f80) = R15 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7f7c) = RIP >> 32;
  SMM_SAVED_STATE(saved_state, 0x7f78) = RIP & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7f70) = read_eflags();

  // --- Debug and Control Registers --- //
  SMM_SAVED_STATE(saved_state, 0x7f68) = BX_CPU_THIS_PTR dr6;
  SMM_SAVED_STATE(saved_state, 0x7f60) = BX_CPU_THIS_PTR dr7;
  SMM_SAVED_STATE(saved_state, 0x7f58) = BX_CPU_THIS_PTR cr0.val32;
  SMM_SAVED_STATE(saved_state, 0x7f54) = BX_CPU_THIS_PTR cr3 >> 32;
  SMM_SAVED_STATE(saved_state, 0x7f50) = BX_CPU_THIS_PTR cr3 & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7f48) = BX_CPU_THIS_PTR cr4.getRegister();
  /* base+0x7f44 to base+0x7f04 is reserved */
  SMM_SAVED_STATE(saved_state, 0x7f00) = BX_CPU_THIS_PTR smbase;
  SMM_SAVED_STATE(saved_state, 0x7efc) = SMM_REVISION_ID;
  /* base+0x7ef8 to base+0x7ed8 is reserved */
  SMM_SAVED_STATE(saved_state, 0x7ed0) = BX_CPU_THIS_PTR get_EFER();
  /* base+0x7ecc is reserved */
  /* base+0x7ec8 is I/O Instruction Restart, Auto-Halt Restart and NMI Mask */
  /* base+0x7ec4 is reserved */
  /* base+0x7ec0 is SMM I/O Trap */
  /* base+0x7ebc to base+0x7ea4 is reserved */

  // --- Task Register --- //
  if (BX_CPU_THIS_PTR tr.cache.type <= 3) {
    SMM_SAVED_STATE(saved_state, 0x7e98) = BX_CPU_THIS_PTR tr.cache.u.tss286.base;
    SMM_SAVED_STATE(saved_state, 0x7e94) = BX_CPU_THIS_PTR tr.cache.u.tss286.limit;
  } else {
    SMM_SAVED_STATE(saved_state, 0x7e9c) = BX_CPU_THIS_PTR tr.cache.u.tss386.base >> 32;
    SMM_SAVED_STATE(saved_state, 0x7e98) = BX_CPU_THIS_PTR tr.cache.u.tss386.base & 0xffffffff;
    SMM_SAVED_STATE(saved_state, 0x7e94) = BX_CPU_THIS_PTR tr.cache.u.tss386.limit;
  }
  SMM_SAVED_STATE(saved_state, 0x7e90) = BX_CPU_THIS_PTR tr.selector.value |
    (((Bit32u) get_segment_ar_data(&BX_CPU_THIS_PTR tr.cache)) << 16);

  // --- IDTR --- //
  SMM_SAVED_STATE(saved_state, 0x7e8c) = BX_CPU_THIS_PTR idtr.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e88) = BX_CPU_THIS_PTR idtr.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e84) = BX_CPU_THIS_PTR idtr.limit;
  // --- LDTR --- //
  SMM_SAVED_STATE(saved_state, 0x7e7c) = BX_CPU_THIS_PTR ldtr.cache.u.ldt.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e78) = BX_CPU_THIS_PTR ldtr.cache.u.ldt.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e74) = BX_CPU_THIS_PTR ldtr.cache.u.ldt.limit;
  SMM_SAVED_STATE(saved_state, 0x7e70) = BX_CPU_THIS_PTR ldtr.selector.value |
    (((Bit32u) get_segment_ar_data(&BX_CPU_THIS_PTR ldtr.cache)) << 16);
  // --- GDTR --- //
  SMM_SAVED_STATE(saved_state, 0x7e6c) = BX_CPU_THIS_PTR gdtr.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e68) = BX_CPU_THIS_PTR gdtr.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e64) = BX_CPU_THIS_PTR gdtr.limit;
  // --- GS selector --- //
  bx_segment_reg_t *seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS]);
  SMM_SAVED_STATE(saved_state, 0x7e5c) = seg->cache.u.segment.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e58) = seg->cache.u.segment.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e54) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7e50) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- FS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS]);
  SMM_SAVED_STATE(saved_state, 0x7e4c) = seg->cache.u.segment.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e48) = seg->cache.u.segment.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e44) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7e40) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- DS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS]);
  SMM_SAVED_STATE(saved_state, 0x7e3c) = seg->cache.u.segment.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e38) = seg->cache.u.segment.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e34) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7e30) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- SS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS]);
  SMM_SAVED_STATE(saved_state, 0x7e2c) = seg->cache.u.segment.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e28) = seg->cache.u.segment.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e24) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7e20) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- CS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS]);
  SMM_SAVED_STATE(saved_state, 0x7e1c) = seg->cache.u.segment.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e18) = seg->cache.u.segment.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e14) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7e10) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- ES selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES]);
  SMM_SAVED_STATE(saved_state, 0x7e0c) = seg->cache.u.segment.base >> 32;
  SMM_SAVED_STATE(saved_state, 0x7e08) = seg->cache.u.segment.base & 0xffffffff;
  SMM_SAVED_STATE(saved_state, 0x7e04) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7e00) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
}

bx_bool BX_CPU_C::smram_restore_state(const Bit32u *saved_state)
{
  return 1;
}

#else /* BX_SUPPORT_X86_64 == 0 */

// for IA32 configuration using Intel P6 512-byte SMM save state map
#define SMM_REVISION_ID 0x00000000

// source for Intel P6 SMM save state map used: www.sandpile.org

void BX_CPU_C::smram_save_state(Bit32u *saved_state)
{
  SMM_SAVED_STATE(saved_state, 0x7ffc) = BX_CPU_THIS_PTR cr0.val32;
  SMM_SAVED_STATE(saved_state, 0x7ff8) = BX_CPU_THIS_PTR cr3;
  SMM_SAVED_STATE(saved_state, 0x7ff4) = read_eflags();

  SMM_SAVED_STATE(saved_state, 0x7ff0) = EIP;
  SMM_SAVED_STATE(saved_state, 0x7fec) = EDI;
  SMM_SAVED_STATE(saved_state, 0x7fe8) = ESI;
  SMM_SAVED_STATE(saved_state, 0x7fe4) = EBP;
  SMM_SAVED_STATE(saved_state, 0x7fe0) = ESP;
  SMM_SAVED_STATE(saved_state, 0x7fdc) = EBX;
  SMM_SAVED_STATE(saved_state, 0x7fd8) = EDX;
  SMM_SAVED_STATE(saved_state, 0x7fd4) = ECX;
  SMM_SAVED_STATE(saved_state, 0x7fd0) = EAX;

  SMM_SAVED_STATE(saved_state, 0x7fcc) = BX_CPU_THIS_PTR dr6;
  SMM_SAVED_STATE(saved_state, 0x7fc8) = BX_CPU_THIS_PTR dr7;
  SMM_SAVED_STATE(saved_state, 0x7fc4) = BX_CPU_THIS_PTR   tr.selector.value;
  SMM_SAVED_STATE(saved_state, 0x7fc0) = BX_CPU_THIS_PTR ldtr.selector.value;

  SMM_SAVED_STATE(saved_state, 0x7fbc) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
  SMM_SAVED_STATE(saved_state, 0x7fb8) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
  SMM_SAVED_STATE(saved_state, 0x7fb4) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
  SMM_SAVED_STATE(saved_state, 0x7fb0) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  SMM_SAVED_STATE(saved_state, 0x7fac) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  SMM_SAVED_STATE(saved_state, 0x7fa8) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;

  // --- SS selector --- //
  bx_segment_reg_t *seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS]);
  SMM_SAVED_STATE(saved_state, 0x7fa4) = seg->cache.u.segment.base;
  SMM_SAVED_STATE(saved_state, 0x7fa0) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7f9c) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- CS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS]);
  SMM_SAVED_STATE(saved_state, 0x7f98) = seg->cache.u.segment.base;
  SMM_SAVED_STATE(saved_state, 0x7f94) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7f90) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- ES selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES]);
  SMM_SAVED_STATE(saved_state, 0x7f8c) = seg->cache.u.segment.base;
  SMM_SAVED_STATE(saved_state, 0x7f88) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7f84) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- LDTR --- //
  SMM_SAVED_STATE(saved_state, 0x7f80) = BX_CPU_THIS_PTR ldtr.cache.u.ldt.base;
  SMM_SAVED_STATE(saved_state, 0x7f7c) = BX_CPU_THIS_PTR ldtr.cache.u.ldt.limit;
  SMM_SAVED_STATE(saved_state, 0x7f78) = BX_CPU_THIS_PTR ldtr.selector.value |
    (((Bit32u) get_segment_ar_data(&BX_CPU_THIS_PTR ldtr.cache)) << 16);
  // --- GDTR --- //
  SMM_SAVED_STATE(saved_state, 0x7f74) = BX_CPU_THIS_PTR gdtr.base;
  SMM_SAVED_STATE(saved_state, 0x7f70) = BX_CPU_THIS_PTR gdtr.limit;
  /* base+0x7f6c is reserved */
  /* base+0x7f68 is reserved */

  // --- Task Register --- //
  if (BX_CPU_THIS_PTR tr.cache.type <= 3) {
    SMM_SAVED_STATE(saved_state, 0x7f64) = BX_CPU_THIS_PTR tr.cache.u.tss286.base;
    SMM_SAVED_STATE(saved_state, 0x7f60) = BX_CPU_THIS_PTR tr.cache.u.tss286.limit;
  } else {
    SMM_SAVED_STATE(saved_state, 0x7f64) = BX_CPU_THIS_PTR tr.cache.u.tss386.base;
    SMM_SAVED_STATE(saved_state, 0x7f60) = BX_CPU_THIS_PTR tr.cache.u.tss386.limit;
  }
  SMM_SAVED_STATE(saved_state, 0x7f5c) = BX_CPU_THIS_PTR tr.selector.value |
    (((Bit32u) get_segment_ar_data(&BX_CPU_THIS_PTR tr.cache)) << 16);

  // --- IDTR --- //
  SMM_SAVED_STATE(saved_state, 0x7f58) = BX_CPU_THIS_PTR idtr.base;
  SMM_SAVED_STATE(saved_state, 0x7f54) = BX_CPU_THIS_PTR idtr.limit;
  /* base+0x7f50 is reserved */
  // --- GS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS]);
  SMM_SAVED_STATE(saved_state, 0x7f4c) = seg->cache.u.segment.base;
  SMM_SAVED_STATE(saved_state, 0x7f48) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7f44) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- FS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS]);
  SMM_SAVED_STATE(saved_state, 0x7f40) = seg->cache.u.segment.base;
  SMM_SAVED_STATE(saved_state, 0x7f3c) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7f38) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- DS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS]);
  SMM_SAVED_STATE(saved_state, 0x7f34) = seg->cache.u.segment.base;
  SMM_SAVED_STATE(saved_state, 0x7f30) = seg->cache.u.segment.limit;
  SMM_SAVED_STATE(saved_state, 0x7f2c) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);

  /* base+0x7f28 to base+7f18 is reserved */
#if BX_CPU_LEVEL >= 4
  SMM_SAVED_STATE(saved_state, 0x7f14) = BX_CPU_THIS_PTR cr4.getRegister();
#endif
  /* base+0x7f10 is I/O restart EIP */
  /* base+0x7f0c is I/O restart ESI */
  /* base+0x7f08 is I/O restart ECX */
  /* base+0x7f04 is I/O restart EDI */
  /* base+0x7f02 is Auto HALT restart field (2 byte) */
  /* base+0x7f00 is I/O restart field (2 byte) */
  SMM_SAVED_STATE(saved_state, 0x7efc) = SMM_REVISION_ID;
  SMM_SAVED_STATE(saved_state, 0x7ef8) = BX_CPU_THIS_PTR smbase;
  /* base+0x7ef4 to base+0x7e00 is reserved */
}

bx_bool BX_CPU_C::smram_restore_state(const Bit32u *saved_state)
{
  SetCR0(SMM_SAVED_STATE(saved_state, 0x7ffc));
  CR3_change(SMM_SAVED_STATE(saved_state, 0x7ff8));
  BX_CPU_THIS_PTR setEFlags(SMM_SAVED_STATE(saved_state, 0x7ff4));

  EIP = SMM_SAVED_STATE(saved_state, 0x7ff0);
  EIP = SMM_SAVED_STATE(saved_state, 0x7ff0);
  EDI = SMM_SAVED_STATE(saved_state, 0x7fec);
  ESI = SMM_SAVED_STATE(saved_state, 0x7fe8);
  EBP = SMM_SAVED_STATE(saved_state, 0x7fe4);
  ESP = SMM_SAVED_STATE(saved_state, 0x7fe0);
  EBX = SMM_SAVED_STATE(saved_state, 0x7fdc);
  EDX = SMM_SAVED_STATE(saved_state, 0x7fd8);
  ECX = SMM_SAVED_STATE(saved_state, 0x7fd4);
  EAX = SMM_SAVED_STATE(saved_state, 0x7fd0);

  BX_CPU_THIS_PTR dr6 = SMM_SAVED_STATE(saved_state, 0x7fcc);
  BX_CPU_THIS_PTR dr7 = SMM_SAVED_STATE(saved_state, 0x7fc8);

  return 1;
}

#endif /* BX_SUPPORT_X86_64 */

#endif /* BX_CPU_LEVEL >= 3 */
