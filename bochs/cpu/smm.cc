/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2006-2018 Stanislav Shwartsman
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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "smm.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_CPU_LEVEL >= 3

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RSM(bxInstruction_c *i)
{
  /* If we are not in System Management Mode, then #UD should be generated */
  if (! BX_CPU_THIS_PTR smm_mode()) {
    BX_INFO(("RSM not in System Management Mode !"));
    exception(BX_UD_EXCEPTION, 0);
  }

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_RSM)) Svm_Vmexit(SVM_VMEXIT_RSM);
  }
#endif

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx) {
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      VMexit(VMX_VMEXIT_RSM, 0);
    }
    else {
      BX_ERROR(("RSM in VMX root operation !"));
      exception(BX_UD_EXCEPTION, 0);
    }
  }
#endif

  BX_INFO(("RSM: Resuming from System Management Mode"));

  unmask_event(BX_EVENT_SMI | BX_EVENT_NMI | BX_EVENT_VMX_VIRTUAL_NMI);

  Bit32u saved_state[SMM_SAVE_STATE_MAP_SIZE], n;
  // reset reserved bits
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) saved_state[n] = 0;

  bx_phy_address base = BX_CPU_THIS_PTR smbase + 0x10000;
  // could be optimized with reading of only non-reserved bytes
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) {
    base -= 4;
    access_read_physical(base, 4, &saved_state[n]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(base, 4, BX_MEMTYPE_WB, BX_READ, BX_SMRAM_ACCESS, (Bit8u*)(&saved_state[n]));
  }
  BX_CPU_THIS_PTR in_smm = false;

  // restore the CPU state from SMRAM
  if (! smram_restore_state(saved_state)) {
    BX_PANIC(("RSM: Incorrect state when restoring CPU state - shutdown !"));
    shutdown();
  }

  // debug(RIP);

  BX_NEXT_TRACE(i);
}

void BX_CPU_C::enter_system_management_mode(void)
{
  BX_INFO(("Enter to System Management Mode"));

  // debug(BX_CPU_THIS_PTR prev_rip);

  //
  // Processors that support VMX operation perform SMI delivery as follows:
  //

#if BX_SUPPORT_VMX
  // Enter SMM
  // save the following internal to the processor:
  //   * CR4.VMXE
  //   * an indication of whether the logical processor was in VMX operation (root or non-root)
  // IF the logical processor is in VMX operation
  // THEN
  //   leave VMX operation;
  //   save VMX-critical state defined below;
  //   preserve current VMCS pointer as noted below;
  // FI;
  // CR4.VMXE = 0;

  BX_CPU_THIS_PTR cr4.set_VMXE(0);
  BX_CPU_THIS_PTR in_smm_vmx = BX_CPU_THIS_PTR in_vmx;
  BX_CPU_THIS_PTR in_smm_vmx_guest = BX_CPU_THIS_PTR in_vmx_guest;
  BX_CPU_THIS_PTR in_vmx = false;
  BX_CPU_THIS_PTR in_vmx_guest = false;

  BX_INFO(("enter_system_management_mode: temporary disable VMX while in SMM mode"));

  // perform ordinary SMI delivery:
  //   * save processor state in SMRAM;
  //   * set processor state to standard SMM values
#endif

  BX_CPU_THIS_PTR in_smm = true;

  mask_event(BX_EVENT_SMI | BX_EVENT_NMI | BX_EVENT_VMX_VIRTUAL_NMI);

  Bit32u saved_state[SMM_SAVE_STATE_MAP_SIZE], n;
  // reset reserved bits
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) saved_state[n] = 0;
  // prepare CPU state to be saved in the SMRAM
  smram_save_state(saved_state);

  bx_phy_address base = BX_CPU_THIS_PTR smbase + 0x10000;
  // could be optimized with reading of only non-reserved bytes
  for(n=0;n<SMM_SAVE_STATE_MAP_SIZE;n++) {
    base -= 4;
    access_write_physical(base, 4, &saved_state[n]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(base, 4, BX_MEMTYPE_WB, BX_WRITE, BX_SMRAM_ACCESS, (Bit8u*)(&saved_state[n]));
  }

  BX_CPU_THIS_PTR setEFlags(0x2); // Bit1 is always set
  BX_CPU_THIS_PTR prev_rip = RIP = 0x00008000;
  BX_CPU_THIS_PTR dr7.set32(0x00000400);

  // CR0 - PE, EM, TS, and PG flags set to 0; others unmodified
  BX_CPU_THIS_PTR cr0.set_PE(0); // real mode (bit 0)
  BX_CPU_THIS_PTR cr0.set_EM(0); // emulate math coprocessor (bit 2)
  BX_CPU_THIS_PTR cr0.set_TS(0); // no task switch (bit 3)
  BX_CPU_THIS_PTR cr0.set_PG(0); // paging disabled (bit 31)

#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR cr4.set32(0);
#endif

#if BX_CPU_LEVEL >= 5
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.get_SVME())
    BX_CPU_THIS_PTR efer.set32(BX_EFER_SVME_MASK);
  else
#endif
    BX_CPU_THIS_PTR efer.set32(0);
#endif

  parse_selector(BX_CPU_THIS_PTR smbase >> 4,
               &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid    = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment  = 1;  /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type     = BX_DATA_READ_WRITE_ACCESSED;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = BX_CPU_THIS_PTR smbase;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xffffffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g   = 1; /* page granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b = 0; /* 16bit default size */
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l   = 0; /* 16bit default size */
#endif

  /* DS (Data Segment) and descriptor cache */
  parse_selector(0x0000,
               &BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid    = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.segment  = 1; /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.type     = BX_DATA_READ_WRITE_ACCESSED;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.u.segment.base         = 0x00000000;
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

  handleCpuContextChange();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);
}

#define SMRAM_TRANSLATE(addr)    (((0x8000 - (addr)) >> 2) - 1)

// SMRAM SMM_SAVE_STATE_MAP_SIZE is 128 elements, find the element for each field
static unsigned smram_map[SMRAM_FIELD_LAST];

#if BX_SUPPORT_X86_64

void BX_CPU_C::init_SMRAM(void)
{
  static bool smram_map_ready = false;

  if (smram_map_ready) return;
  smram_map_ready = true;

  smram_map[SMRAM_FIELD_SMBASE_OFFSET] = SMRAM_TRANSLATE(0x7f00);
  smram_map[SMRAM_FIELD_SMM_REVISION_ID] = SMRAM_TRANSLATE(0x7efc);
  smram_map[SMRAM_FIELD_RAX_HI32] = SMRAM_TRANSLATE(0x7ffc);
  smram_map[SMRAM_FIELD_EAX] = SMRAM_TRANSLATE(0x7ff8);
  smram_map[SMRAM_FIELD_RCX_HI32] = SMRAM_TRANSLATE(0x7ff4);
  smram_map[SMRAM_FIELD_ECX] = SMRAM_TRANSLATE(0x7ff0);
  smram_map[SMRAM_FIELD_RDX_HI32] = SMRAM_TRANSLATE(0x7fec);
  smram_map[SMRAM_FIELD_EDX] = SMRAM_TRANSLATE(0x7fe8);
  smram_map[SMRAM_FIELD_RBX_HI32] = SMRAM_TRANSLATE(0x7fe4);
  smram_map[SMRAM_FIELD_EBX] = SMRAM_TRANSLATE(0x7fe0);
  smram_map[SMRAM_FIELD_RSP_HI32] = SMRAM_TRANSLATE(0x7fdc);
  smram_map[SMRAM_FIELD_ESP] = SMRAM_TRANSLATE(0x7fd8);
  smram_map[SMRAM_FIELD_RBP_HI32] = SMRAM_TRANSLATE(0x7fd4);
  smram_map[SMRAM_FIELD_EBP] = SMRAM_TRANSLATE(0x7fd0);
  smram_map[SMRAM_FIELD_RSI_HI32] = SMRAM_TRANSLATE(0x7fcc);
  smram_map[SMRAM_FIELD_ESI] = SMRAM_TRANSLATE(0x7fc8);
  smram_map[SMRAM_FIELD_RDI_HI32] = SMRAM_TRANSLATE(0x7fc4);
  smram_map[SMRAM_FIELD_EDI] = SMRAM_TRANSLATE(0x7fc0);
  smram_map[SMRAM_FIELD_R8_HI32]  = SMRAM_TRANSLATE(0x7fbc);
  smram_map[SMRAM_FIELD_R8]  = SMRAM_TRANSLATE(0x7fb8);
  smram_map[SMRAM_FIELD_R9_HI32]  = SMRAM_TRANSLATE(0x7fb4);
  smram_map[SMRAM_FIELD_R9]  = SMRAM_TRANSLATE(0x7fb0);
  smram_map[SMRAM_FIELD_R10_HI32] = SMRAM_TRANSLATE(0x7fac);
  smram_map[SMRAM_FIELD_R10] = SMRAM_TRANSLATE(0x7fa8);
  smram_map[SMRAM_FIELD_R11_HI32] = SMRAM_TRANSLATE(0x7fa4);
  smram_map[SMRAM_FIELD_R11] = SMRAM_TRANSLATE(0x7fa0);
  smram_map[SMRAM_FIELD_R12_HI32] = SMRAM_TRANSLATE(0x7f9c);
  smram_map[SMRAM_FIELD_R12] = SMRAM_TRANSLATE(0x7f98);
  smram_map[SMRAM_FIELD_R13_HI32] = SMRAM_TRANSLATE(0x7f94);
  smram_map[SMRAM_FIELD_R13] = SMRAM_TRANSLATE(0x7f90);
  smram_map[SMRAM_FIELD_R14_HI32] = SMRAM_TRANSLATE(0x7f8c);
  smram_map[SMRAM_FIELD_R14] = SMRAM_TRANSLATE(0x7f88);
  smram_map[SMRAM_FIELD_R15_HI32] = SMRAM_TRANSLATE(0x7f84);
  smram_map[SMRAM_FIELD_R15] = SMRAM_TRANSLATE(0x7f80);
  smram_map[SMRAM_FIELD_RIP_HI32] = SMRAM_TRANSLATE(0x7f7c);
  smram_map[SMRAM_FIELD_EIP] = SMRAM_TRANSLATE(0x7f78);
  smram_map[SMRAM_FIELD_RFLAGS_HI32] = SMRAM_TRANSLATE(0x7f74); // always zero
  smram_map[SMRAM_FIELD_EFLAGS] = SMRAM_TRANSLATE(0x7f70);
  smram_map[SMRAM_FIELD_DR6_HI32] = SMRAM_TRANSLATE(0x7f6c);    // always zero
  smram_map[SMRAM_FIELD_DR6] = SMRAM_TRANSLATE(0x7f68);
  smram_map[SMRAM_FIELD_DR7_HI32] = SMRAM_TRANSLATE(0x7f64);    // always zero
  smram_map[SMRAM_FIELD_DR7] = SMRAM_TRANSLATE(0x7f60);
  smram_map[SMRAM_FIELD_CR0_HI32] = SMRAM_TRANSLATE(0x7f5c);    // always zero
  smram_map[SMRAM_FIELD_CR0] = SMRAM_TRANSLATE(0x7f58);
  smram_map[SMRAM_FIELD_CR3_HI32] = SMRAM_TRANSLATE(0x7f54);    // zero when physical address size 32-bit
  smram_map[SMRAM_FIELD_CR3] = SMRAM_TRANSLATE(0x7f50);
  smram_map[SMRAM_FIELD_CR4_HI32] = SMRAM_TRANSLATE(0x7f4c);    // always zero
  smram_map[SMRAM_FIELD_CR4] = SMRAM_TRANSLATE(0x7f48);
  smram_map[SMRAM_FIELD_EFER_HI32] = SMRAM_TRANSLATE(0x7ed4);   // always zero
  smram_map[SMRAM_FIELD_EFER] = SMRAM_TRANSLATE(0x7ed0);
  smram_map[SMRAM_FIELD_IO_INSTRUCTION_RESTART] = SMRAM_TRANSLATE(0x7ec8);
  smram_map[SMRAM_FIELD_AUTOHALT_RESTART] = SMRAM_TRANSLATE(0x7ec8);
  smram_map[SMRAM_FIELD_NMI_MASK] = SMRAM_TRANSLATE(0x7ec8);
  smram_map[SMRAM_FIELD_TR_BASE_HI32] = SMRAM_TRANSLATE(0x7e9c);
  smram_map[SMRAM_FIELD_TR_BASE] = SMRAM_TRANSLATE(0x7e98);
  smram_map[SMRAM_FIELD_TR_LIMIT] = SMRAM_TRANSLATE(0x7e94);
  smram_map[SMRAM_FIELD_TR_SELECTOR_AR] = SMRAM_TRANSLATE(0x7e90);
  smram_map[SMRAM_FIELD_IDTR_BASE_HI32] = SMRAM_TRANSLATE(0x7e8c);
  smram_map[SMRAM_FIELD_IDTR_BASE] = SMRAM_TRANSLATE(0x7e88);
  smram_map[SMRAM_FIELD_IDTR_LIMIT] = SMRAM_TRANSLATE(0x7e84);
  smram_map[SMRAM_FIELD_LDTR_BASE_HI32] = SMRAM_TRANSLATE(0x7e7c);
  smram_map[SMRAM_FIELD_LDTR_BASE] = SMRAM_TRANSLATE(0x7e78);
  smram_map[SMRAM_FIELD_LDTR_LIMIT] = SMRAM_TRANSLATE(0x7e74);
  smram_map[SMRAM_FIELD_LDTR_SELECTOR_AR] = SMRAM_TRANSLATE(0x7e70);
  smram_map[SMRAM_FIELD_GDTR_BASE_HI32] = SMRAM_TRANSLATE(0x7e6c);
  smram_map[SMRAM_FIELD_GDTR_BASE] = SMRAM_TRANSLATE(0x7e68);
  smram_map[SMRAM_FIELD_GDTR_LIMIT] = SMRAM_TRANSLATE(0x7e64);
  smram_map[SMRAM_FIELD_ES_BASE_HI32] = SMRAM_TRANSLATE(0x7e0c);
  smram_map[SMRAM_FIELD_ES_BASE] = SMRAM_TRANSLATE(0x7e08);
  smram_map[SMRAM_FIELD_ES_LIMIT] = SMRAM_TRANSLATE(0x7e04);
  smram_map[SMRAM_FIELD_ES_SELECTOR_AR] = SMRAM_TRANSLATE(0x7e00);
  smram_map[SMRAM_FIELD_CS_BASE_HI32] = SMRAM_TRANSLATE(0x7e1c);
  smram_map[SMRAM_FIELD_CS_BASE] = SMRAM_TRANSLATE(0x7e18);
  smram_map[SMRAM_FIELD_CS_LIMIT] = SMRAM_TRANSLATE(0x7e14);
  smram_map[SMRAM_FIELD_CS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7e10);
  smram_map[SMRAM_FIELD_SS_BASE_HI32] = SMRAM_TRANSLATE(0x7e2c);
  smram_map[SMRAM_FIELD_SS_BASE] = SMRAM_TRANSLATE(0x7e28);
  smram_map[SMRAM_FIELD_SS_LIMIT] = SMRAM_TRANSLATE(0x7e24);
  smram_map[SMRAM_FIELD_SS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7e20);
  smram_map[SMRAM_FIELD_DS_BASE_HI32] = SMRAM_TRANSLATE(0x7e3c);
  smram_map[SMRAM_FIELD_DS_BASE] = SMRAM_TRANSLATE(0x7e38);
  smram_map[SMRAM_FIELD_DS_LIMIT] = SMRAM_TRANSLATE(0x7e34);
  smram_map[SMRAM_FIELD_DS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7e30);
  smram_map[SMRAM_FIELD_FS_BASE_HI32] = SMRAM_TRANSLATE(0x7e4c);
  smram_map[SMRAM_FIELD_FS_BASE] = SMRAM_TRANSLATE(0x7e48);
  smram_map[SMRAM_FIELD_FS_LIMIT] = SMRAM_TRANSLATE(0x7e44);
  smram_map[SMRAM_FIELD_FS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7e40);
  smram_map[SMRAM_FIELD_GS_BASE_HI32] = SMRAM_TRANSLATE(0x7e5c);
  smram_map[SMRAM_FIELD_GS_BASE] = SMRAM_TRANSLATE(0x7e58);
  smram_map[SMRAM_FIELD_GS_LIMIT] = SMRAM_TRANSLATE(0x7e54);
  smram_map[SMRAM_FIELD_GS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7e50);

  for (unsigned n=0; n<SMRAM_FIELD_LAST;n++) {
    if (smram_map[n] >= SMM_SAVE_STATE_MAP_SIZE) {
      BX_PANIC(("smram map[%d] = %d", n, smram_map[n]));
    }
  }
}

#else

// source for Intel P6 SMM save state map used: www.sandpile.org

void BX_CPU_C::init_SMRAM(void)
{
  smram_map[SMRAM_FIELD_SMBASE_OFFSET] = SMRAM_TRANSLATE(0x7ef8);
  smram_map[SMRAM_FIELD_SMM_REVISION_ID] = SMRAM_TRANSLATE(0x7efc);
  smram_map[SMRAM_FIELD_EAX] = SMRAM_TRANSLATE(0x7fd0);
  smram_map[SMRAM_FIELD_ECX] = SMRAM_TRANSLATE(0x7fd4);
  smram_map[SMRAM_FIELD_EDX] = SMRAM_TRANSLATE(0x7fd8);
  smram_map[SMRAM_FIELD_EBX] = SMRAM_TRANSLATE(0x7fdc);
  smram_map[SMRAM_FIELD_ESP] = SMRAM_TRANSLATE(0x7fe0);
  smram_map[SMRAM_FIELD_EBP] = SMRAM_TRANSLATE(0x7fe4);
  smram_map[SMRAM_FIELD_ESI] = SMRAM_TRANSLATE(0x7fe8);
  smram_map[SMRAM_FIELD_EDI] = SMRAM_TRANSLATE(0x7fec);
  smram_map[SMRAM_FIELD_EIP] = SMRAM_TRANSLATE(0x7ff0);
  smram_map[SMRAM_FIELD_EFLAGS] = SMRAM_TRANSLATE(0x7ff4);
  smram_map[SMRAM_FIELD_DR6] = SMRAM_TRANSLATE(0x7fcc);
  smram_map[SMRAM_FIELD_DR7] = SMRAM_TRANSLATE(0x7fc8);
  smram_map[SMRAM_FIELD_CR0] = SMRAM_TRANSLATE(0x7ffc);
  smram_map[SMRAM_FIELD_CR3] = SMRAM_TRANSLATE(0x7ff8);
  smram_map[SMRAM_FIELD_CR4] = SMRAM_TRANSLATE(0x7f14);
  smram_map[SMRAM_FIELD_EFER] = SMRAM_TRANSLATE(0x7f10);
  smram_map[SMRAM_FIELD_IO_INSTRUCTION_RESTART] = SMRAM_TRANSLATE(0x7f00);
  smram_map[SMRAM_FIELD_AUTOHALT_RESTART] = SMRAM_TRANSLATE(0x7f00);
  smram_map[SMRAM_FIELD_NMI_MASK] = SMRAM_TRANSLATE(0x7f00);
  smram_map[SMRAM_FIELD_TR_SELECTOR] = SMRAM_TRANSLATE(0x7fc4);
  smram_map[SMRAM_FIELD_TR_BASE] = SMRAM_TRANSLATE(0x7f64);
  smram_map[SMRAM_FIELD_TR_LIMIT] = SMRAM_TRANSLATE(0x7f60);
  smram_map[SMRAM_FIELD_TR_SELECTOR_AR] = SMRAM_TRANSLATE(0x7f5c);
  smram_map[SMRAM_FIELD_LDTR_SELECTOR] = SMRAM_TRANSLATE(0x7fc0);
  smram_map[SMRAM_FIELD_LDTR_BASE] = SMRAM_TRANSLATE(0x7f80);
  smram_map[SMRAM_FIELD_LDTR_LIMIT] = SMRAM_TRANSLATE(0x7f7c);
  smram_map[SMRAM_FIELD_LDTR_SELECTOR_AR] = SMRAM_TRANSLATE(0x7f78);
  smram_map[SMRAM_FIELD_IDTR_BASE] = SMRAM_TRANSLATE(0x7f58);
  smram_map[SMRAM_FIELD_IDTR_LIMIT] = SMRAM_TRANSLATE(0x7f54);
  smram_map[SMRAM_FIELD_GDTR_BASE] = SMRAM_TRANSLATE(0x7f74);
  smram_map[SMRAM_FIELD_GDTR_LIMIT] = SMRAM_TRANSLATE(0x7f70);
  smram_map[SMRAM_FIELD_ES_SELECTOR] = SMRAM_TRANSLATE(0x7fa8);
  smram_map[SMRAM_FIELD_ES_BASE] = SMRAM_TRANSLATE(0x7f8c);
  smram_map[SMRAM_FIELD_ES_LIMIT] = SMRAM_TRANSLATE(0x7f88);
  smram_map[SMRAM_FIELD_ES_SELECTOR_AR] = SMRAM_TRANSLATE(0x7f84);
  smram_map[SMRAM_FIELD_CS_SELECTOR] = SMRAM_TRANSLATE(0x7fac);
  smram_map[SMRAM_FIELD_CS_BASE] = SMRAM_TRANSLATE(0x7f98);
  smram_map[SMRAM_FIELD_CS_LIMIT] = SMRAM_TRANSLATE(0x7f94);
  smram_map[SMRAM_FIELD_CS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7f90);
  smram_map[SMRAM_FIELD_SS_SELECTOR] = SMRAM_TRANSLATE(0x7fb0);
  smram_map[SMRAM_FIELD_SS_BASE] = SMRAM_TRANSLATE(0x7fa4);
  smram_map[SMRAM_FIELD_SS_LIMIT] = SMRAM_TRANSLATE(0x7fa0);
  smram_map[SMRAM_FIELD_SS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7f9c);
  smram_map[SMRAM_FIELD_DS_SELECTOR] = SMRAM_TRANSLATE(0x7fb4);
  smram_map[SMRAM_FIELD_DS_BASE] = SMRAM_TRANSLATE(0x7f34);
  smram_map[SMRAM_FIELD_DS_LIMIT] = SMRAM_TRANSLATE(0x7f30);
  smram_map[SMRAM_FIELD_DS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7f2c);
  smram_map[SMRAM_FIELD_FS_SELECTOR] = SMRAM_TRANSLATE(0x7fb8);
  smram_map[SMRAM_FIELD_FS_BASE] = SMRAM_TRANSLATE(0x7f40);
  smram_map[SMRAM_FIELD_FS_LIMIT] = SMRAM_TRANSLATE(0x7f3c);
  smram_map[SMRAM_FIELD_FS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7f38);
  smram_map[SMRAM_FIELD_GS_SELECTOR] = SMRAM_TRANSLATE(0x7fbc);
  smram_map[SMRAM_FIELD_GS_BASE] = SMRAM_TRANSLATE(0x7f4c);
  smram_map[SMRAM_FIELD_GS_LIMIT] = SMRAM_TRANSLATE(0x7f48);
  smram_map[SMRAM_FIELD_GS_SELECTOR_AR] = SMRAM_TRANSLATE(0x7f44);

  for (unsigned n=0; n<SMRAM_FIELD_LAST;n++) {
    if (smram_map[n] >= SMM_SAVE_STATE_MAP_SIZE) {
      BX_PANIC(("smram map[%d] = %d", n, smram_map[n]));
    }
  }
}

#endif

#define SMRAM_FIELD(state, field) (state[smram_map[field]])

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
  for (int n=0; n<BX_GENERAL_REGISTERS; n++) {
    Bit64u val_64 = BX_READ_64BIT_REG(n);

    SMRAM_FIELD(saved_state, SMRAM_FIELD_RAX_HI32 + 2*n) = GET32H(val_64);
    SMRAM_FIELD(saved_state, SMRAM_FIELD_EAX + 2*n) = GET32L(val_64);
  }

  SMRAM_FIELD(saved_state, SMRAM_FIELD_RIP_HI32) = GET32H(RIP);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_EIP) = EIP;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_EFLAGS) = read_eflags();

  // --- Debug and Control Registers --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_DR6) = BX_CPU_THIS_PTR dr6.get32();
  SMRAM_FIELD(saved_state, SMRAM_FIELD_DR7) = BX_CPU_THIS_PTR dr7.get32();
  SMRAM_FIELD(saved_state, SMRAM_FIELD_CR0) = BX_CPU_THIS_PTR cr0.get32();
  SMRAM_FIELD(saved_state, SMRAM_FIELD_CR3_HI32) = GET32H(BX_CPU_THIS_PTR cr3);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_CR3) = GET32L(BX_CPU_THIS_PTR cr3);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_CR4) = BX_CPU_THIS_PTR cr4.get32();
  SMRAM_FIELD(saved_state, SMRAM_FIELD_EFER) = BX_CPU_THIS_PTR efer.get32();

  SMRAM_FIELD(saved_state, SMRAM_FIELD_SMBASE_OFFSET)   = BX_CPU_THIS_PTR smbase;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_SMM_REVISION_ID) = SMM_REVISION_ID;

  // --- Task Register --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_BASE_HI32) = GET32H(BX_CPU_THIS_PTR tr.cache.u.segment.base);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_BASE) = GET32L(BX_CPU_THIS_PTR tr.cache.u.segment.base);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_LIMIT) = BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled;
  bool tr_valid = (BX_CPU_THIS_PTR tr.cache.valid != 0);
  Bit32u tr_ar = ((get_descriptor_h(&BX_CPU_THIS_PTR tr.cache) >> 8) & 0xf0ff) | (tr_valid << 8);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_SELECTOR_AR) = BX_CPU_THIS_PTR tr.selector.value | (tr_ar << 16);

  // --- LDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_BASE_HI32) = GET32H(BX_CPU_THIS_PTR ldtr.cache.u.segment.base);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_BASE) = GET32L(BX_CPU_THIS_PTR ldtr.cache.u.segment.base);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_LIMIT) = BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled;
  bool ldtr_valid = (BX_CPU_THIS_PTR ldtr.cache.valid != 0);
  Bit32u ldtr_ar = ((get_descriptor_h(&BX_CPU_THIS_PTR ldtr.cache) >> 8) & 0xf0ff) | (ldtr_valid << 8);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_SELECTOR_AR) = BX_CPU_THIS_PTR ldtr.selector.value | (ldtr_ar << 16);

  // --- IDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_IDTR_BASE_HI32) = GET32H(BX_CPU_THIS_PTR idtr.base);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_IDTR_BASE) = GET32L(BX_CPU_THIS_PTR idtr.base);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_IDTR_LIMIT) = BX_CPU_THIS_PTR idtr.limit;

  // --- GDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_GDTR_BASE_HI32) = GET32H(BX_CPU_THIS_PTR gdtr.base);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_GDTR_BASE) = GET32L(BX_CPU_THIS_PTR gdtr.base);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_GDTR_LIMIT) = BX_CPU_THIS_PTR gdtr.limit;

  for (int segreg = 0; segreg < 6; segreg++) {
    bx_segment_reg_t *seg = &(BX_CPU_THIS_PTR sregs[segreg]);
    SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_BASE_HI32 + 4*segreg) = GET32H(seg->cache.u.segment.base);
    SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_BASE + 4*segreg) = GET32L(seg->cache.u.segment.base);
    SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_LIMIT + 4*segreg) = seg->cache.u.segment.limit_scaled;
    bool seg_valid = (seg->cache.valid != 0);
    Bit32u seg_ar = ((get_descriptor_h(&seg->cache) >> 8) & 0xf0ff) | (seg_valid << 8);
    SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_SELECTOR_AR + 4*segreg) = seg->selector.value | (seg_ar << 16);
  }
}

bool BX_CPU_C::smram_restore_state(const Bit32u *saved_state)
{
  BX_SMM_State smm_state;

  smm_state.smm_revision_id = SMRAM_FIELD(saved_state, SMRAM_FIELD_SMM_REVISION_ID);

  smm_state.cr0.val32 = SMRAM_FIELD(saved_state, SMRAM_FIELD_CR0);
  smm_state.cr3 = SMRAM_FIELD64(saved_state, SMRAM_FIELD_CR3_HI32, SMRAM_FIELD_CR3);
  smm_state.cr4.val32 = SMRAM_FIELD(saved_state, SMRAM_FIELD_CR4);
  smm_state.efer.val32 = SMRAM_FIELD(saved_state, SMRAM_FIELD_EFER);
  smm_state.eflags = SMRAM_FIELD(saved_state, SMRAM_FIELD_EFLAGS);

  for (int n=0; n<BX_GENERAL_REGISTERS; n++) {
    smm_state.gen_reg[n] = SMRAM_FIELD64(saved_state,
           SMRAM_FIELD_RAX_HI32 + 2*n, SMRAM_FIELD_EAX + 2*n);
  }

  smm_state.rip = SMRAM_FIELD64(saved_state, SMRAM_FIELD_RIP_HI32, SMRAM_FIELD_EIP);

  smm_state.dr6 = SMRAM_FIELD(saved_state, SMRAM_FIELD_DR6);
  smm_state.dr7 = SMRAM_FIELD(saved_state, SMRAM_FIELD_DR7);

  smm_state.gdtr.base  = SMRAM_FIELD64(saved_state, SMRAM_FIELD_GDTR_BASE_HI32, SMRAM_FIELD_GDTR_BASE);
  smm_state.gdtr.limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_GDTR_LIMIT);
  smm_state.idtr.base  = SMRAM_FIELD64(saved_state, SMRAM_FIELD_IDTR_BASE_HI32, SMRAM_FIELD_IDTR_BASE);
  smm_state.idtr.limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_IDTR_LIMIT);

  for (int segreg = 0; segreg < 6; segreg++) {
    smm_state.segreg[segreg].selector_ar = SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_SELECTOR_AR + 4*segreg);
    smm_state.segreg[segreg].base = SMRAM_FIELD64(saved_state, SMRAM_FIELD_ES_BASE_HI32 + 4*segreg, SMRAM_FIELD_ES_BASE + 4*segreg);
    smm_state.segreg[segreg].limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_LIMIT + 4*segreg);
  }

  smm_state.ldtr.selector_ar = SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_SELECTOR_AR);
  smm_state.ldtr.base = SMRAM_FIELD64(saved_state, SMRAM_FIELD_LDTR_BASE_HI32, SMRAM_FIELD_LDTR_BASE);
  smm_state.ldtr.limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_LIMIT);

  smm_state.tr.selector_ar = SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_SELECTOR_AR);
  smm_state.tr.base = SMRAM_FIELD64(saved_state, SMRAM_FIELD_TR_BASE_HI32, SMRAM_FIELD_TR_BASE);
  smm_state.tr.limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_LIMIT);

  if (SMM_REVISION_ID & SMM_SMBASE_RELOCATION)
     smm_state.smbase = SMRAM_FIELD(saved_state, SMRAM_FIELD_SMBASE_OFFSET);
  else
     smm_state.smbase = 0;

  return resume_from_system_management_mode(&smm_state);
}

#else /* BX_SUPPORT_X86_64 == 0 */

void BX_CPU_C::smram_save_state(Bit32u *saved_state)
{
  SMRAM_FIELD(saved_state, SMRAM_FIELD_SMM_REVISION_ID) = SMM_REVISION_ID;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_SMBASE_OFFSET)   = BX_CPU_THIS_PTR smbase;

  for (int n=0; n<BX_GENERAL_REGISTERS; n++) {
    Bit32u val_32 = BX_READ_32BIT_REG(n);
    SMRAM_FIELD(saved_state, SMRAM_FIELD_EAX + n) = val_32;
  }

  SMRAM_FIELD(saved_state, SMRAM_FIELD_EIP) = EIP;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_EFLAGS) = read_eflags();

  SMRAM_FIELD(saved_state, SMRAM_FIELD_CR0) = BX_CPU_THIS_PTR cr0.get32();
  SMRAM_FIELD(saved_state, SMRAM_FIELD_CR3) = BX_CPU_THIS_PTR cr3;
#if BX_CPU_LEVEL >= 5
  SMRAM_FIELD(saved_state, SMRAM_FIELD_CR4) = BX_CPU_THIS_PTR cr4.get32();
  SMRAM_FIELD(saved_state, SMRAM_FIELD_EFER) = BX_CPU_THIS_PTR efer.get32();
#endif
  SMRAM_FIELD(saved_state, SMRAM_FIELD_DR6) = BX_CPU_THIS_PTR dr6.get32();
  SMRAM_FIELD(saved_state, SMRAM_FIELD_DR7) = BX_CPU_THIS_PTR dr7.get32();

  // --- Task Register --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_SELECTOR) = BX_CPU_THIS_PTR tr.selector.value;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_BASE) = BX_CPU_THIS_PTR tr.cache.u.segment.base;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_LIMIT) = BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled;
  bool tr_valid = (BX_CPU_THIS_PTR tr.cache.valid != 0);
  Bit32u tr_ar = ((get_descriptor_h(&BX_CPU_THIS_PTR tr.cache) >> 8) & 0xf0ff) | (tr_valid << 8);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_SELECTOR_AR) = BX_CPU_THIS_PTR tr.selector.value | (tr_ar << 16);

  // --- LDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_SELECTOR) = BX_CPU_THIS_PTR ldtr.selector.value;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_BASE) = BX_CPU_THIS_PTR ldtr.cache.u.segment.base;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_LIMIT) = BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled;
  bool ldtr_valid = (BX_CPU_THIS_PTR ldtr.cache.valid != 0);
  Bit32u ldtr_ar = ((get_descriptor_h(&BX_CPU_THIS_PTR ldtr.cache) >> 8) & 0xf0ff) | (ldtr_valid << 8);
  SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_SELECTOR_AR) = BX_CPU_THIS_PTR ldtr.selector.value | (ldtr_ar << 16);

  // --- IDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_IDTR_BASE) = BX_CPU_THIS_PTR idtr.base;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_IDTR_LIMIT) = BX_CPU_THIS_PTR idtr.limit;

  // --- GDTR --- //
  SMRAM_FIELD(saved_state, SMRAM_FIELD_GDTR_BASE) = BX_CPU_THIS_PTR gdtr.base;
  SMRAM_FIELD(saved_state, SMRAM_FIELD_GDTR_LIMIT) = BX_CPU_THIS_PTR gdtr.limit;

  for (int segreg = 0; segreg < 6; segreg++) {
    bx_segment_reg_t *seg = &(BX_CPU_THIS_PTR sregs[segreg]);
    SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_SELECTOR + 4*segreg) = seg->selector.value;
    SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_BASE + 4*segreg) = seg->cache.u.segment.base;
    SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_LIMIT + 4*segreg) = seg->cache.u.segment.limit_scaled;
    bool seg_valid = (seg->cache.valid != 0);
    Bit32u seg_ar = ((get_descriptor_h(&seg->cache) >> 8) & 0xf0ff) | (seg_valid << 8);
    SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_SELECTOR_AR + 4*segreg) = seg->selector.value | (seg_ar << 16);
  }
}

bool BX_CPU_C::smram_restore_state(const Bit32u *saved_state)
{
  BX_SMM_State smm_state;

  smm_state.smm_revision_id = SMRAM_FIELD(saved_state, SMRAM_FIELD_SMM_REVISION_ID);

  smm_state.cr0.val32 = SMRAM_FIELD(saved_state, SMRAM_FIELD_CR0);
  smm_state.cr3 = SMRAM_FIELD(saved_state, SMRAM_FIELD_CR3);
#if BX_CPU_LEVEL >= 5
  smm_state.cr4.val32 = SMRAM_FIELD(saved_state, SMRAM_FIELD_CR4);
  smm_state.efer.val32 = SMRAM_FIELD(saved_state, SMRAM_FIELD_EFER);
#endif

  smm_state.eflags = SMRAM_FIELD(saved_state, SMRAM_FIELD_EFLAGS);

  for (int n=0; n<BX_GENERAL_REGISTERS; n++) {
    smm_state.gen_reg[n] = SMRAM_FIELD(saved_state, SMRAM_FIELD_EAX + n);
  }

  smm_state.rip = SMRAM_FIELD(saved_state, SMRAM_FIELD_EIP);

  smm_state.dr6 = SMRAM_FIELD(saved_state, SMRAM_FIELD_DR6);
  smm_state.dr7 = SMRAM_FIELD(saved_state, SMRAM_FIELD_DR7);

  smm_state.gdtr.base  = SMRAM_FIELD(saved_state, SMRAM_FIELD_GDTR_BASE);
  smm_state.gdtr.limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_GDTR_LIMIT);

  smm_state.idtr.base  = SMRAM_FIELD(saved_state, SMRAM_FIELD_IDTR_BASE);
  smm_state.idtr.limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_IDTR_LIMIT);

  for (int segreg = 0; segreg < 6; segreg++) {
    smm_state.segreg[segreg].selector_ar = SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_SELECTOR_AR + 4*segreg);
    smm_state.segreg[segreg].base = SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_BASE + 4*segreg);
    smm_state.segreg[segreg].limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_ES_LIMIT + 4*segreg);
  }

  smm_state.ldtr.selector_ar = SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_SELECTOR_AR);
  smm_state.ldtr.base = SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_BASE);
  smm_state.ldtr.limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_LDTR_LIMIT);

  smm_state.tr.selector_ar = SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_SELECTOR_AR);
  smm_state.tr.base = SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_BASE);
  smm_state.tr.limit = SMRAM_FIELD(saved_state, SMRAM_FIELD_TR_LIMIT);

  if (SMM_REVISION_ID & SMM_SMBASE_RELOCATION)
     smm_state.smbase = SMRAM_FIELD(saved_state, SMRAM_FIELD_SMBASE_OFFSET);
  else
     smm_state.smbase = 0;

  return resume_from_system_management_mode(&smm_state);
}

#endif /* BX_SUPPORT_X86_64 */

bool BX_CPU_C::resume_from_system_management_mode(BX_SMM_State *smm_state)
{
  // Processors that support VMX operation perform RSM as follows:
#if BX_SUPPORT_VMX
  // IF VMXE=1 in CR4 image in SMRAM
  // THEN
  //   fail and enter shutdown state;

  if (smm_state->cr4.get_VMXE()) {
    BX_PANIC(("SMM restore: CR4.VMXE is set in restore image !"));
    return 0;
  }

  // restore state normally from SMRAM;
  // CR4.VMXE = value stored internally;
  // IF internal storage indicates that the logical processor had been in VMX operation (root or non-root)
  // THEN
  //   enter VMX operation (root or non-root);
  //   restore VMX-critical state
  //   set CR0.PE, CR0.NE, and CR0.PG to 1;
  //   IF RFLAGS.VM = 0
  //   THEN
  //     CS.RPL = SS.DPL;
  //     SS.RPL = SS.DPL;
  //   FI;
  //   If necessary, restore current VMCS pointer;
  //   Leave SMM; Deassert SMMEM on subsequent bus transactions;
  //   IF logical processor will be in VMX operation after RSM
  //   THEN
  //     block A20M and leave A20M mode;
  //   FI;

  if (BX_CPU_THIS_PTR in_smm_vmx) {
    BX_CPU_THIS_PTR in_vmx = true;
    BX_CPU_THIS_PTR in_vmx_guest = BX_CPU_THIS_PTR in_smm_vmx_guest;
    BX_INFO(("SMM Restore: enable VMX %s mode", BX_CPU_THIS_PTR in_vmx_guest ? "guest" : "host"));

    smm_state->cr0.set_PG(1); // set CR0.PG, CR0.NE and CR0.PE
    smm_state->cr0.set_NE(1);
    smm_state->cr0.set_PE(1);

    smm_state->cr4.set_VMXE(1);
    // block and disable A20M;
  }
#endif

#if BX_CPU_LEVEL >= 5
  if (smm_state->efer.get32() & ~((Bit64u) BX_CPU_THIS_PTR efer_suppmask)) {
    BX_PANIC(("SMM restore: Attempt to set EFER reserved bits: 0x%08x !", smm_state->efer.get32()));
    return 0;
  }

  BX_CPU_THIS_PTR efer.set32(smm_state->efer.val32);
#endif

  // check CR0 conditions for entering to shutdown state
  if (!check_CR0(smm_state->cr0.get32())) {
    BX_PANIC(("SMM restore: CR0 consistency check failed !"));
    return 0;
  }

#if BX_CPU_LEVEL >= 5
  if (!check_CR4(smm_state->cr4.get32())) {
    BX_PANIC(("SMM restore: CR4 consistency check failed !"));
    return 0;
  }
#endif

  BX_CPU_THIS_PTR cr0.set32(smm_state->cr0.get32());
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR cr4.set32(smm_state->cr4.get32());
#endif
  BX_CPU_THIS_PTR cr3 = smm_state->cr3;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.get_LMA()) {
    if (smm_state->eflags & EFlagsVMMask) {
      BX_PANIC(("SMM restore: If EFER.LMA = 1 => RFLAGS.VM=0 !"));
      return 0;
    }

    if (!BX_CPU_THIS_PTR cr4.get_PAE() || !BX_CPU_THIS_PTR cr0.get_PG() || !BX_CPU_THIS_PTR cr0.get_PE() || !BX_CPU_THIS_PTR efer.get_LME()) {
      BX_PANIC(("SMM restore: If EFER.LMA = 1 <=> CR4.PAE, CR0.PG, CR0.PE, EFER.LME=1 !"));
      return 0;
    }
  }
  else {
    if (BX_CPU_THIS_PTR cr4.get_PCIDE()) {
      BX_PANIC(("SMM restore: CR4.PCIDE must be clear when not in long mode !"));
      return 0;
    }
  }

  if (BX_CPU_THIS_PTR cr4.get_PAE() && BX_CPU_THIS_PTR cr0.get_PG() && BX_CPU_THIS_PTR cr0.get_PE() && BX_CPU_THIS_PTR efer.get_LME()) {
    if (! BX_CPU_THIS_PTR efer.get_LMA()) {
      BX_PANIC(("SMM restore: If EFER.LMA = 1 <=> CR4.PAE, CR0.PG, CR0.PE, EFER.LME=1 !"));
      return 0;
    }
  }
#endif

#if BX_CPU_LEVEL >= 6
  if (BX_CPU_THIS_PTR cr0.get_PG() && BX_CPU_THIS_PTR cr4.get_PAE() && !long_mode()) {
    if (! CheckPDPTR(smm_state->cr3)) {
      BX_ERROR(("SMM restore: PDPTR check failed !"));
      return 0;
    }
  }
#endif

  setEFlags(smm_state->eflags);

#if BX_SUPPORT_X86_64
  for (int n=0; n<BX_GENERAL_REGISTERS; n++)
    BX_WRITE_64BIT_REG(n, smm_state->gen_reg[n]);
#else
  for (int n=0; n<BX_GENERAL_REGISTERS; n++)
    BX_WRITE_32BIT_REGZ(n, smm_state->gen_reg[n]);
#endif

  RIP = BX_CPU_THIS_PTR prev_rip = smm_state->rip;

  BX_CPU_THIS_PTR dr6.val32 = smm_state->dr6;
  BX_CPU_THIS_PTR dr7.val32 = smm_state->dr7;

  BX_CPU_THIS_PTR gdtr = smm_state->gdtr;
  BX_CPU_THIS_PTR idtr = smm_state->idtr;

  for (int segreg = 0; segreg < 6; segreg++) {
    Bit16u ar_data = smm_state->segreg[segreg].selector_ar >> 16;
    if (set_segment_ar_data(&BX_CPU_THIS_PTR sregs[segreg],
          (ar_data >> 8) & 1,
          smm_state->segreg[segreg].selector_ar & 0xffff,
          smm_state->segreg[segreg].base,
          smm_state->segreg[segreg].limit, ar_data))
    {
       if (! BX_CPU_THIS_PTR sregs[segreg].cache.segment) {
         BX_PANIC(("SMM restore: restored valid non segment %d !", segreg));
         return 0;
       }
    }
  }

  Bit16u ar_data = smm_state->ldtr.selector_ar >> 16;
  if (set_segment_ar_data(&BX_CPU_THIS_PTR ldtr,
          (ar_data >> 8) & 1,
          smm_state->ldtr.selector_ar & 0xffff,
          smm_state->ldtr.base,
          smm_state->ldtr.limit, ar_data))
  {
     if (BX_CPU_THIS_PTR ldtr.cache.type != BX_SYS_SEGMENT_LDT) {
        BX_PANIC(("SMM restore: LDTR is not LDT descriptor type !"));
        return 0;
     }
  }

  ar_data = smm_state->tr.selector_ar >> 16;
  if (set_segment_ar_data(&BX_CPU_THIS_PTR tr,
          (ar_data >> 8) & 1,
          smm_state->tr.selector_ar & 0xffff,
          smm_state->tr.base,
          smm_state->tr.limit, ar_data))
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
     BX_CPU_THIS_PTR smbase = smm_state->smbase;
#if BX_CPU_LEVEL < 6
     if (BX_CPU_THIS_PTR smbase & 0x7fff) {
        BX_PANIC(("SMM restore: SMBASE must be aligned to 32K !"));
        return 0;
     }
#endif
  }

  handleCpuContextChange();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);

  return 1;
}

#endif /* BX_CPU_LEVEL >= 3 */
