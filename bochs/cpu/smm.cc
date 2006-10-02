/////////////////////////////////////////////////////////////////////////
// $Id: smm.cc,v 1.21 2006-10-02 21:22:59 vruppert Exp $
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
#include "smm.h"
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
//            type       - must be saved/restored
//            base       - must be saved/restored
//            limit      - must be saved/restored
//            g          - must be saved/restored
//            d_b        - must be saved/restored
//            l          - must be saved/restored
//            avl        - must be saved/restored
// 4. GDTR, IDTR
//     fields: base, limit
// 5. LDTR, TR
//     fields: base, limit, anything else ?
// 6. Debug Registers DR0-DR7, only DR6 and DR7 are saved
// 7. Control Registers: CR0, CR1 is always 0, CR2 is NOT saved, CR3, CR4, EFER
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

  BX_INFO(("RSM: Resuming from System Management Mode !"));

  BX_CPU_THIS_PTR nmi_disable = 0;

  Bit32u saved_state[SMM_SAVE_STATE_MAP_SIZE], n;
  // reset reserved bits
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) saved_state[n] = 0;

  bx_phy_address base = BX_CPU_THIS_PTR smbase + 0x10000;
  // could be optimized with reading of only non-reserved bytes
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) {
    base -= 4;
    BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, base, 4, &saved_state[n]);
  }
  BX_CPU_THIS_PTR in_smm = 0;

  // restore the CPU state from SMRAM
  if (! smram_restore_state(saved_state)) {
    BX_PANIC(("RSM: Incorrect state when restoring CPU state - shutdown !"));
    shutdown();
  }

  debug(RIP);
}

void BX_CPU_C::enter_system_management_mode(void)
{
  invalidate_prefetch_q();

  debug(BX_CPU_THIS_PTR prev_eip);

  BX_INFO(("Enter to System Management Mode !"));

  BX_CPU_THIS_PTR in_smm = 1;

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
  BX_CPU_THIS_PTR prev_eip = RIP = 0x00008000;
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

  parse_selector(BX_CPU_THIS_PTR smbase >> 4,
               &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);
  
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid    = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment  = 1;  /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type     = BX_CODE_EXEC_READ_ACCESSED;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = BX_CPU_THIS_PTR smbase;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xffffffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g   = 1; /* page granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b = 0; /* 16bit default size */
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l   = 0; /* 16bit default size */
#endif

#if BX_SUPPORT_ICACHE
  BX_CPU_THIS_PTR updateFetchModeMask();
#endif

  /* DS (Data Segment) and descriptor cache */
  parse_selector(0x0000,
               &BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid    = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment  = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.type     = BX_DATA_READ_WRITE_ACCESSED;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base         = 0x00000000;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit        = 0xffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled = 0xffffffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.avl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.g   = 1; /* byte granular */
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

#define SMRAM_TRANSLATE(addr)    (((0x8000 - (addr)) >> 2) - 1)
#define SMRAM_FIELD(state, addr) (state[SMRAM_TRANSLATE(addr)])

#if BX_SUPPORT_X86_64

BX_CPP_INLINE Bit64u SMRAM_FIELD64(const Bit32u *saved_state, unsigned hi, unsigned lo)
{
  Bit64u tmp  = ((Bit64u) SMRAM_FIELD(saved_state, hi)) << 32;
         tmp |=  (Bit64u) SMRAM_FIELD(saved_state, lo);
  return tmp;
}

void BX_CPU_C::smram_save_state(Bit32u *saved_state)
{
  // --- General Purpose Registers --- //
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RAX_HI32) = RAX >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RAX_LO32) = RAX & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RCX_HI32) = RCX >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RCX_LO32) = RCX & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RDX_HI32) = RDX >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RDX_LO32) = RDX & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RBX_HI32) = RBX >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RBX_LO32) = RBX & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RSP_HI32) = RSP >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RSP_LO32) = RSP & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RBP_HI32) = RBP >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RBP_LO32) = RBP & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RSI_HI32) = RSI >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RSI_LO32) = RSI & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RDI_HI32) = RDI >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RDI_LO32) = RDI & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R8_HI32)  =  R8 >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R8_LO32)  =  R8 & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R9_HI32)  =  R9 >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R9_LO32)  =  R9 & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R10_HI32) = R10 >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R10_LO32) = R10 & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R11_HI32) = R11 >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R11_LO32) = R11 & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R12_HI32) = R12 >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R12_LO32) = R12 & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R13_HI32) = R13 >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R13_LO32) = R13 & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R14_HI32) = R14 >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R14_LO32) = R14 & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R15_HI32) = R15 >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_R15_LO32) = R15 & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RIP_HI32) = RIP >> 32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RIP_LO32) = RIP & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_RFLAGS32) = read_eflags();

  // --- Debug and Control Registers --- //
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_DR6) = BX_CPU_THIS_PTR dr6;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_DR7) = BX_CPU_THIS_PTR dr7;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR0) = BX_CPU_THIS_PTR cr0.val32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR3) = BX_CPU_THIS_PTR cr3;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR4) = BX_CPU_THIS_PTR cr4.getRegister();
  /* base+0x7f44 to base+0x7f04 is reserved */
  SMRAM_FIELD(saved_state, SMRAM_SMBASE_OFFSET)   = BX_CPU_THIS_PTR smbase;
  SMRAM_FIELD(saved_state, SMRAM_SMM_REVISION_ID) = SMM_REVISION_ID;
  /* base+0x7ef8 to base+0x7ed8 is reserved */
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_EFER) = BX_CPU_THIS_PTR get_EFER();
  /* base+0x7ecc is reserved */
  /* base+0x7ec8 is I/O Instruction Restart, Auto-Halt Restart and NMI Mask */
  /* base+0x7ec4 is reserved */
  /* base+0x7ec0 is SMM I/O Trap */
  /* base+0x7ebc to base+0x7ea0 is reserved */

  // --- Task Register --- //
  SMRAM_FIELD(saved_state, SMRAM_TR_BASE_HI32) = BX_CPU_THIS_PTR tr.cache.u.system.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_TR_BASE_LO32) = BX_CPU_THIS_PTR tr.cache.u.system.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_TR_LIMIT) = BX_CPU_THIS_PTR tr.cache.u.system.limit;
  SMRAM_FIELD(saved_state, SMRAM_TR_SELECTOR_AR) = BX_CPU_THIS_PTR tr.selector.value |
    (((Bit32u) get_segment_ar_data(&BX_CPU_THIS_PTR tr.cache)) << 16);

  // --- IDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_IDTR_BASE_HI32) = BX_CPU_THIS_PTR idtr.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_IDTR_BASE_LO32) = BX_CPU_THIS_PTR idtr.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_IDTR_LIMIT) = BX_CPU_THIS_PTR idtr.limit;
  // --- LDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_LDTR_BASE_HI32) = BX_CPU_THIS_PTR ldtr.cache.u.system.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_LDTR_BASE_LO32) = BX_CPU_THIS_PTR ldtr.cache.u.system.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_LDTR_LIMIT) = BX_CPU_THIS_PTR ldtr.cache.u.system.limit;
  SMRAM_FIELD(saved_state, SMRAM_LDTR_SELECTOR_AR) = BX_CPU_THIS_PTR ldtr.selector.value |
    (((Bit32u) get_segment_ar_data(&BX_CPU_THIS_PTR ldtr.cache)) << 16);
  // --- GDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_GDTR_BASE_HI32) = BX_CPU_THIS_PTR gdtr.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_GDTR_BASE_LO32) = BX_CPU_THIS_PTR gdtr.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_GDTR_LIMIT) = BX_CPU_THIS_PTR gdtr.limit;
  // --- GS selector --- //
  bx_segment_reg_t *seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS]);
  SMRAM_FIELD(saved_state, SMRAM_GS_BASE_HI32) = seg->cache.u.segment.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_GS_BASE_LO32) = seg->cache.u.segment.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_GS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_GS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- FS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS]);
  SMRAM_FIELD(saved_state, SMRAM_FS_BASE_HI32) = seg->cache.u.segment.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_FS_BASE_LO32) = seg->cache.u.segment.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_FS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_FS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- DS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS]);
  SMRAM_FIELD(saved_state, SMRAM_DS_BASE_HI32) = seg->cache.u.segment.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_DS_BASE_LO32) = seg->cache.u.segment.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_DS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_DS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- SS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS]);
  SMRAM_FIELD(saved_state, SMRAM_SS_BASE_HI32) = seg->cache.u.segment.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_SS_BASE_LO32) = seg->cache.u.segment.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_SS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_SS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- CS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS]);
  SMRAM_FIELD(saved_state, SMRAM_CS_BASE_HI32) = seg->cache.u.segment.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_CS_BASE_LO32) = seg->cache.u.segment.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_CS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_CS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- ES selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES]);
  SMRAM_FIELD(saved_state, SMRAM_ES_BASE_HI32) = seg->cache.u.segment.base >> 32;
  SMRAM_FIELD(saved_state, SMRAM_ES_BASE_LO32) = seg->cache.u.segment.base & 0xffffffff;
  SMRAM_FIELD(saved_state, SMRAM_ES_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_ES_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
}

bx_bool BX_CPU_C::smram_restore_state(const Bit32u *saved_state)
{
  Bit32u temp_cr0    = SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR0);
  Bit32u temp_eflags = SMRAM_FIELD(saved_state, SMRAM_OFFSET_RFLAGS32);
  Bit32u temp_efer   = SMRAM_FIELD(saved_state, SMRAM_OFFSET_EFER);

  bx_bool pe = (temp_cr0 & 0x01);
  bx_bool nw = (temp_cr0 >> 29) & 0x01;
  bx_bool cd = (temp_cr0 >> 30) & 0x01;
  bx_bool pg = (temp_cr0 >> 31) & 0x01;

  // check CR0 conditions for entering to shutdown state
  if (pg && !pe) {
    BX_PANIC(("SMM restore: attempt to set CR0.PG with CR0.PE cleared !"));
    return 0;
  }

  if (nw && !cd) {
    BX_PANIC(("SMM restore: attempt to set CR0.NW with CR0.CD cleared !"));
    return 0;
  }

  // shutdown if write to reserved CR4 bits
  if (! SetCR4(SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR4))) {
    BX_PANIC(("SMM restore: incorrect CR4 state !"));
    return 0;
  }

  BX_CPU_THIS_PTR msr.sce   = (temp_efer >> 0)  & 1;
  BX_CPU_THIS_PTR msr.lme   = (temp_efer >> 8)  & 1;
  BX_CPU_THIS_PTR msr.lma   = (temp_efer >> 10) & 1;
  BX_CPU_THIS_PTR msr.nxe   = (temp_efer >> 11) & 1;
  BX_CPU_THIS_PTR msr.ffxsr = (temp_efer >> 14) & 1;

  if (BX_CPU_THIS_PTR msr.lma) {
    if (temp_eflags & EFlagsVMMask) {
      BX_PANIC(("SMM restore: If EFER.LMA = 1 => RFLAGS.VM=0 !"));
      return 0;
    }

    if (!BX_CPU_THIS_PTR cr4.get_PAE() || !pg || !pe || !BX_CPU_THIS_PTR msr.lme) {
      BX_PANIC(("SMM restore: If EFER.LMA = 1 <=> CR4.PAE, CR0.PG, CR0.PE, EFER.LME=1 !"));
      return 0;
    }
  }

  if (BX_CPU_THIS_PTR cr4.get_PAE() && pg && pe && BX_CPU_THIS_PTR msr.lme) {
    if (! BX_CPU_THIS_PTR msr.lma) {
      BX_PANIC(("SMM restore: If EFER.LMA = 1 <=> CR4.PAE, CR0.PG, CR0.PE, EFER.LME=1 !"));
      return 0;
    }
  }

  // hack CR0 to be able to back to long mode correctly
  BX_CPU_THIS_PTR cr0.pe = 0; // real mode (bit 0)
  BX_CPU_THIS_PTR cr0.pg = 0; // paging disabled (bit 31)
  BX_CPU_THIS_PTR cr0.val32 &= 0x7ffffffe;
  SetCR0(temp_cr0);
  setEFlags(temp_eflags);

  bx_phy_address temp_cr3 = SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR3);
  CR3_change(temp_cr3);

  RAX = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RAX_HI32, SMRAM_OFFSET_RAX_LO32);
  RBX = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RBX_HI32, SMRAM_OFFSET_RBX_LO32);
  RCX = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RCX_HI32, SMRAM_OFFSET_RCX_LO32);
  RDX = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RDX_HI32, SMRAM_OFFSET_RDX_LO32);
  RSP = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RSP_HI32, SMRAM_OFFSET_RSP_LO32);
  RBP = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RBP_HI32, SMRAM_OFFSET_RBP_LO32);
  RSI = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RSI_HI32, SMRAM_OFFSET_RSI_LO32);
  RDI = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RDI_HI32, SMRAM_OFFSET_RDI_LO32);
  R8  = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_R8_HI32,  SMRAM_OFFSET_R8_LO32);
  R9  = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_R9_HI32,  SMRAM_OFFSET_R9_LO32);
  R10 = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_R10_HI32, SMRAM_OFFSET_R10_LO32);
  R11 = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_R11_HI32, SMRAM_OFFSET_R11_LO32);
  R12 = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_R12_HI32, SMRAM_OFFSET_R12_LO32);
  R13 = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_R13_HI32, SMRAM_OFFSET_R13_LO32);
  R14 = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_R14_HI32, SMRAM_OFFSET_R14_LO32);
  R15 = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_R15_HI32, SMRAM_OFFSET_R15_LO32);
  RIP = SMRAM_FIELD64(saved_state, SMRAM_OFFSET_RIP_HI32, SMRAM_OFFSET_RIP_LO32);

  BX_CPU_THIS_PTR dr6 = SMRAM_FIELD(saved_state, SMRAM_OFFSET_DR6);
  BX_CPU_THIS_PTR dr7 = SMRAM_FIELD(saved_state, SMRAM_OFFSET_DR7);

  BX_CPU_THIS_PTR gdtr.base  = SMRAM_FIELD64(saved_state, SMRAM_GDTR_BASE_HI32, SMRAM_GDTR_BASE_LO32);
  BX_CPU_THIS_PTR gdtr.limit = SMRAM_FIELD(saved_state, SMRAM_GDTR_LIMIT);
  BX_CPU_THIS_PTR idtr.base  = SMRAM_FIELD64(saved_state, SMRAM_IDTR_BASE_HI32, SMRAM_IDTR_BASE_LO32);
  BX_CPU_THIS_PTR idtr.limit = SMRAM_FIELD(saved_state, SMRAM_IDTR_LIMIT);

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS],
          SMRAM_FIELD(saved_state, SMRAM_CS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD64(saved_state, SMRAM_CS_BASE_HI32, SMRAM_CS_BASE_LO32),
          SMRAM_FIELD(saved_state, SMRAM_CS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_CS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment CS !"));
        return 0;
     }
  }

  handleCpuModeChange();

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS],
          SMRAM_FIELD(saved_state, SMRAM_DS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD64(saved_state, SMRAM_DS_BASE_HI32, SMRAM_DS_BASE_LO32),
          SMRAM_FIELD(saved_state, SMRAM_DS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_DS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment DS !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS],
          SMRAM_FIELD(saved_state, SMRAM_SS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD64(saved_state, SMRAM_SS_BASE_HI32, SMRAM_SS_BASE_LO32),
          SMRAM_FIELD(saved_state, SMRAM_SS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_SS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment SS !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES],
          SMRAM_FIELD(saved_state, SMRAM_ES_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD64(saved_state, SMRAM_ES_BASE_HI32, SMRAM_ES_BASE_LO32),
          SMRAM_FIELD(saved_state, SMRAM_ES_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_ES_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment ES !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS],
          SMRAM_FIELD(saved_state, SMRAM_FS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD64(saved_state, SMRAM_FS_BASE_HI32, SMRAM_FS_BASE_LO32),
          SMRAM_FIELD(saved_state, SMRAM_FS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_FS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment FS !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS],
          SMRAM_FIELD(saved_state, SMRAM_GS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD64(saved_state, SMRAM_GS_BASE_HI32, SMRAM_GS_BASE_LO32),
          SMRAM_FIELD(saved_state, SMRAM_GS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_GS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment GS !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR ldtr,
          SMRAM_FIELD(saved_state, SMRAM_LDTR_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD64(saved_state, SMRAM_LDTR_BASE_HI32, SMRAM_LDTR_BASE_LO32),
          SMRAM_FIELD(saved_state, SMRAM_LDTR_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_LDTR_SELECTOR_AR) >> 16))
  {
     if (BX_CPU_THIS_PTR ldtr.cache.type != BX_SYS_SEGMENT_LDT) {
        BX_PANIC(("SMM restore: LDTR is not LDT descriptor type !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR tr,
          SMRAM_FIELD(saved_state, SMRAM_TR_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD64(saved_state, SMRAM_TR_BASE_HI32, SMRAM_TR_BASE_LO32),
          SMRAM_FIELD(saved_state, SMRAM_TR_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_TR_SELECTOR_AR) >> 16))
  {
     if (BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_AVAIL_286_TSS &&
         BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_BUSY_286_TSS &&
         BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_AVAIL_386_TSS &&
         BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_BUSY_386_TSS)
     {
        BX_PANIC(("SMM restore: TR is not TSS descriptor type !"));
        return 0;
     }
  }

  if (SMM_REVISION_ID & SMM_SMBASE_RELOCATION)
     BX_CPU_THIS_PTR smbase = SMRAM_FIELD(saved_state, SMRAM_SMBASE_OFFSET);

  return 1;
}

#else /* BX_SUPPORT_X86_64 == 0 */

void BX_CPU_C::smram_save_state(Bit32u *saved_state)
{
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR0) = BX_CPU_THIS_PTR cr0.val32;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR3) = BX_CPU_THIS_PTR cr3;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_EFLAGS) = read_eflags();
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_EIP) = EIP;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_EDI) = EDI;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_ESI) = ESI;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_EBP) = EBP;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_ESP) = ESP;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_EBX) = EBX;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_EDX) = EDX;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_ECX) = ECX;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_EAX) = EAX;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_DR6) = BX_CPU_THIS_PTR dr6;
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_DR7) = BX_CPU_THIS_PTR dr7;
  SMRAM_FIELD(saved_state, SMRAM_TR_SELECTOR)   = BX_CPU_THIS_PTR   tr.selector.value;
  SMRAM_FIELD(saved_state, SMRAM_LDTR_SELECTOR) = BX_CPU_THIS_PTR ldtr.selector.value;

  SMRAM_FIELD(saved_state, SMRAM_GS_SELECTOR) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
  SMRAM_FIELD(saved_state, SMRAM_FS_SELECTOR) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
  SMRAM_FIELD(saved_state, SMRAM_DS_SELECTOR) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
  SMRAM_FIELD(saved_state, SMRAM_SS_SELECTOR) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  SMRAM_FIELD(saved_state, SMRAM_CS_SELECTOR) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  SMRAM_FIELD(saved_state, SMRAM_ES_SELECTOR) =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;

  // --- SS selector --- //
  bx_segment_reg_t *seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS]);
  SMRAM_FIELD(saved_state, SMRAM_SS_BASE) = seg->cache.u.segment.base;
  SMRAM_FIELD(saved_state, SMRAM_SS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_SS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- CS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS]);
  SMRAM_FIELD(saved_state, SMRAM_CS_BASE) = seg->cache.u.segment.base;
  SMRAM_FIELD(saved_state, SMRAM_CS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_CS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- ES selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES]);
  SMRAM_FIELD(saved_state, SMRAM_ES_BASE) = seg->cache.u.segment.base;
  SMRAM_FIELD(saved_state, SMRAM_ES_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_ES_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- LDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_LDTR_BASE) = BX_CPU_THIS_PTR ldtr.cache.u.system.base;
  SMRAM_FIELD(saved_state, SMRAM_LDTR_LIMIT) = BX_CPU_THIS_PTR ldtr.cache.u.system.limit;
  SMRAM_FIELD(saved_state, SMRAM_LDTR_SELECTOR_AR) = BX_CPU_THIS_PTR ldtr.selector.value |
    (((Bit32u) get_segment_ar_data(&BX_CPU_THIS_PTR ldtr.cache)) << 16);
  // --- GDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_GDTR_BASE) = BX_CPU_THIS_PTR gdtr.base;
  SMRAM_FIELD(saved_state, SMRAM_GDTR_LIMIT) = BX_CPU_THIS_PTR gdtr.limit;
  /* base+0x7f6c is reserved */
  /* base+0x7f68 is reserved */

  // --- Task Register --- //
  SMRAM_FIELD(saved_state, SMRAM_TR_BASE) = BX_CPU_THIS_PTR tr.cache.u.system.base;
  SMRAM_FIELD(saved_state, SMRAM_TR_LIMIT) = BX_CPU_THIS_PTR tr.cache.u.system.limit;
  SMRAM_FIELD(saved_state, SMRAM_TR_SELECTOR_AR) = BX_CPU_THIS_PTR tr.selector.value |
    (((Bit32u) get_segment_ar_data(&BX_CPU_THIS_PTR tr.cache)) << 16);

  // --- IDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_IDTR_BASE) = BX_CPU_THIS_PTR idtr.base;
  SMRAM_FIELD(saved_state, SMRAM_IDTR_LIMIT) = BX_CPU_THIS_PTR idtr.limit;
  /* base+0x7f50 is reserved */
  // --- GS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS]);
  SMRAM_FIELD(saved_state, SMRAM_GS_BASE) = seg->cache.u.segment.base;
  SMRAM_FIELD(saved_state, SMRAM_GS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_GS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- FS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS]);
  SMRAM_FIELD(saved_state, SMRAM_FS_BASE) = seg->cache.u.segment.base;
  SMRAM_FIELD(saved_state, SMRAM_FS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_FS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);
  // --- DS selector --- //
  seg = &(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS]);
  SMRAM_FIELD(saved_state, SMRAM_DS_BASE) = seg->cache.u.segment.base;
  SMRAM_FIELD(saved_state, SMRAM_DS_LIMIT) = seg->cache.u.segment.limit;
  SMRAM_FIELD(saved_state, SMRAM_DS_SELECTOR_AR) = seg->selector.value |
    (((Bit32u) get_segment_ar_data(&seg->cache)) << 16);

  /* base+0x7f28 to base+7f18 is reserved */
#if BX_CPU_LEVEL >= 4
  SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR4) = BX_CPU_THIS_PTR cr4.getRegister();
#endif

  /* base+0x7f02 is Auto HALT restart field (2 byte) */
  /* base+0x7f00 is I/O restart field (2 byte) */
  SMRAM_FIELD(saved_state, SMRAM_SMM_REVISION_ID) = SMM_REVISION_ID;
  SMRAM_FIELD(saved_state, SMRAM_SMBASE_OFFSET)   = BX_CPU_THIS_PTR smbase;
  /* base+0x7ef4 to base+0x7e00 is reserved */
}

bx_bool BX_CPU_C::smram_restore_state(const Bit32u *saved_state)
{
  Bit32u temp_cr0    = SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR0);
  Bit32u temp_eflags = SMRAM_FIELD(saved_state, SMRAM_OFFSET_EFLAGS);
  Bit32u temp_cr3    = SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR3);

  bx_bool pe = (temp_cr0 & 0x01);
  bx_bool nw = (temp_cr0 >> 29) & 0x01;
  bx_bool cd = (temp_cr0 >> 30) & 0x01;
  bx_bool pg = (temp_cr0 >> 31) & 0x01;

  // check conditions for entering to shutdown state
  if (pg && !pe) {
    BX_PANIC(("SMM restore: attempt to set CR0.PG with CR0.PE cleared !"));
    return 0;
  }

  if (nw && !cd) {
    BX_PANIC(("SMM restore: attempt to set CR0.NW with CR0.CD cleared !"));
    return 0;
  }

  SetCR0(temp_cr0);
  CR3_change(temp_cr3);
  setEFlags(temp_eflags);

#if BX_CPU_LEVEL >= 4
  if (! SetCR4(SMRAM_FIELD(saved_state, SMRAM_OFFSET_CR4))) {
    BX_PANIC(("SMM restore: incorrect CR4 state !"));
    return 0;
  }
#endif

  EIP = SMRAM_FIELD(saved_state, SMRAM_OFFSET_EIP);
  EDI = SMRAM_FIELD(saved_state, SMRAM_OFFSET_EDI);
  ESI = SMRAM_FIELD(saved_state, SMRAM_OFFSET_ESI);
  EBP = SMRAM_FIELD(saved_state, SMRAM_OFFSET_EBP);
  ESP = SMRAM_FIELD(saved_state, SMRAM_OFFSET_ESP);
  EBX = SMRAM_FIELD(saved_state, SMRAM_OFFSET_EBX);
  EDX = SMRAM_FIELD(saved_state, SMRAM_OFFSET_EDX);
  ECX = SMRAM_FIELD(saved_state, SMRAM_OFFSET_ECX);
  EAX = SMRAM_FIELD(saved_state, SMRAM_OFFSET_EAX);

  BX_CPU_THIS_PTR dr6 = SMRAM_FIELD(saved_state, SMRAM_OFFSET_DR6);
  BX_CPU_THIS_PTR dr7 = SMRAM_FIELD(saved_state, SMRAM_OFFSET_DR7);

  BX_CPU_THIS_PTR gdtr.base  = SMRAM_FIELD(saved_state, SMRAM_GDTR_BASE);
  BX_CPU_THIS_PTR gdtr.limit = SMRAM_FIELD(saved_state, SMRAM_GDTR_LIMIT);
  BX_CPU_THIS_PTR idtr.base  = SMRAM_FIELD(saved_state, SMRAM_IDTR_BASE);
  BX_CPU_THIS_PTR idtr.limit = SMRAM_FIELD(saved_state, SMRAM_IDTR_LIMIT);

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS],
          SMRAM_FIELD(saved_state, SMRAM_CS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD(saved_state, SMRAM_CS_BASE),
          SMRAM_FIELD(saved_state, SMRAM_CS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_CS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment CS !"));
        return 0;
     }
  }

  handleCpuModeChange();

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS],
          SMRAM_FIELD(saved_state, SMRAM_DS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD(saved_state, SMRAM_DS_BASE),
          SMRAM_FIELD(saved_state, SMRAM_DS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_DS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment DS !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS],
          SMRAM_FIELD(saved_state, SMRAM_SS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD(saved_state, SMRAM_SS_BASE),
          SMRAM_FIELD(saved_state, SMRAM_SS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_SS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment SS !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES],
          SMRAM_FIELD(saved_state, SMRAM_ES_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD(saved_state, SMRAM_ES_BASE),
          SMRAM_FIELD(saved_state, SMRAM_ES_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_ES_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment ES !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS],
          SMRAM_FIELD(saved_state, SMRAM_FS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD(saved_state, SMRAM_FS_BASE),
          SMRAM_FIELD(saved_state, SMRAM_FS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_FS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment FS !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS],
          SMRAM_FIELD(saved_state, SMRAM_GS_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD(saved_state, SMRAM_GS_BASE),
          SMRAM_FIELD(saved_state, SMRAM_GS_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_GS_SELECTOR_AR) >> 16))
  {
     if (! BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.segment) {
        BX_PANIC(("SMM restore: restored valid non segment GS !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR ldtr,
          SMRAM_FIELD(saved_state, SMRAM_LDTR_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD(saved_state, SMRAM_LDTR_BASE),
          SMRAM_FIELD(saved_state, SMRAM_LDTR_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_LDTR_SELECTOR_AR) >> 16))
  {
     if (BX_CPU_THIS_PTR ldtr.cache.type != BX_SYS_SEGMENT_LDT) {
        BX_PANIC(("SMM restore: LDTR is not LDT descriptor type !"));
        return 0;
     }
  }

  if (set_segment_ar_data(&BX_CPU_THIS_PTR tr,
          SMRAM_FIELD(saved_state, SMRAM_TR_SELECTOR_AR) & 0xffff,
          SMRAM_FIELD(saved_state, SMRAM_TR_BASE),
          SMRAM_FIELD(saved_state, SMRAM_TR_LIMIT),
          SMRAM_FIELD(saved_state, SMRAM_TR_SELECTOR_AR) >> 16))
  {
     if (BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_AVAIL_286_TSS &&
         BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_BUSY_286_TSS &&
         BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_AVAIL_386_TSS &&
         BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_BUSY_386_TSS)
     {
        BX_PANIC(("SMM restore: TR is not TSS descriptor type !"));
        return 0;
     }
  }

  if (SMM_REVISION_ID & SMM_SMBASE_RELOCATION) {
     BX_CPU_THIS_PTR smbase = SMRAM_FIELD(saved_state, SMRAM_SMBASE_OFFSET);
#if BX_CPU_LEVEL < 6
     if (BX_CPU_THIS_PTR smbase & 0x7fff) {
        BX_PANIC(("SMM restore: SMBASE must be aligned to 32K !"));
        return 0;
     }
#endif
  }

  return 1;
}

#endif /* BX_SUPPORT_X86_64 */

#endif /* BX_CPU_LEVEL >= 3 */
