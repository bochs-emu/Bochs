/////////////////////////////////////////////////////////////////////////
// $Id: tasking.cc,v 1.9 2001-11-11 04:57:05 bdenney Exp $
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



#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR





#if BX_SUPPORT_TASKING

#if BX_CPU_LEVEL >= 2

// Notes:
// ======
// Step 2: TSS descriptor is not busy TS (for IRET); GP (for JMP, CALL, INT)
//   returns error code (Task's backlink TSS)???

// *   TSS selector must map to GDT
// *   TSS is stored in linear address space
// * what to do with I/O Map Base
// * what to do with T flag
// * where to set CR3 and flush paging cache
// * what happens when fault occurs, with some seg regs having valid bit cleared?
// * should check validity of current TR(TSS) before writing into it
//

  // ======================
  // 286 Task State Segment
  // ======================
  // dynamic item                      | hex  dec  offset
  // 0       task LDT selector         | 2a   42
  // 1       DS selector               | 28   40
  // 1       SS selector               | 26   38
  // 1       CS selector               | 24   36
  // 1       ES selector               | 22   34
  // 1       DI                        | 20   32
  // 1       SI                        | 1e   30
  // 1       BP                        | 1c   28
  // 1       SP                        | 1a   26
  // 1       BX                        | 18   24
  // 1       DX                        | 16   22
  // 1       CX                        | 14   20
  // 1       AX                        | 12   18
  // 1       flag word                 | 10   16
  // 1       IP (entry point)          | 0e   14
  // 0       SS for CPL 2              | 0c   12
  // 0       SP for CPL 2              | 0a   10
  // 0       SS for CPL 1              | 08   08
  // 0       SP for CPL 1              | 06   06
  // 0       SS for CPL 0              | 04   04
  // 0       SP for CPL 0              | 02   02
  //         back link selector to TSS | 00   00


  // ======================
  // 386 Task State Segment
  // ======================
  // |31            16|15                    0|
  // |I/O Map Base    |000000000000000000000|T| 64  static
  // |0000000000000000| LDT                   | 60  static
  // |0000000000000000| GS selector           | 5c  dynamic
  // |0000000000000000| FS selector           | 58  dynamic
  // |0000000000000000| DS selector           | 54  dynamic
  // |0000000000000000| SS selector           | 50  dynamic
  // |0000000000000000| CS selector           | 4c  dynamic
  // |0000000000000000| ES selector           | 48  dynamic
  // |                EDI                     | 44  dynamic
  // |                ESI                     | 40  dynamic
  // |                EBP                     | 3c  dynamic
  // |                ESP                     | 38  dynamic
  // |                EBX                     | 34  dynamic
  // |                EDX                     | 30  dynamic
  // |                ECX                     | 2c  dynamic
  // |                EAX                     | 28  dynamic
  // |                EFLAGS                  | 24  dynamic
  // |                EIP (entry point)       | 20  dynamic
  // |           CR3 (PDPR)                   | 1c  static
  // |000000000000000 | SS for CPL 2          | 18  static
  // |           ESP for CPL 2                | 14  static
  // |000000000000000 | SS for CPL 1          | 10  static
  // |           ESP for CPL 1                | 0c  static
  // |000000000000000 | SS for CPL 0          | 08  static
  // |           ESP for CPL 0                | 04  static
  // |000000000000000 | back link to prev TSS | 00  dynamic (updated only when return expected)


  // ==================================================
  // Effect of task switch on Busy, NT, and Link Fields
  // ==================================================

  // Field         jump        call/interrupt     iret
  // ------------------------------------------------------
  // new busy bit  Set         Set                No change
  // old busy bit  Cleared     No change          Cleared
  // new NT flag   No change   Set                No change
  // old NT flag   No change   No change          Cleared
  // new link      No change   old TSS selector   No change
  // old link      No change   No change          No change
  // CR0.TS        Set         Set                Set

  // Note: I checked 386, 486, and Pentium, and they all exhibited
  //       exactly the same behaviour as above.  There seems to
  //       be some misprints in the Intel docs.


  void
BX_CPU_C::task_switch(bx_selector_t *tss_selector,
                 bx_descriptor_t *tss_descriptor, unsigned source,
                 Bit32u dword1, Bit32u dword2)
{
  Bit32u obase32; // base address of old TSS
  Bit32u nbase32; // base address of new TSS
  Bit32u temp32, newCR3;
  Bit16u raw_cs_selector, raw_ss_selector, raw_ds_selector, raw_es_selector,
         raw_fs_selector, raw_gs_selector, raw_ldt_selector;
  Bit16u temp16, trap_word;
  bx_selector_t cs_selector, ss_selector, ds_selector, es_selector,
                fs_selector, gs_selector, ldt_selector;
  bx_descriptor_t cs_descriptor, ss_descriptor, ds_descriptor, es_descriptor,
                  fs_descriptor, gs_descriptor, ldt_descriptor;
  Bit32u old_TSS_max, new_TSS_max, old_TSS_limit, new_TSS_limit;
  Bit32u newEAX, newECX, newEDX, newEBX;
  Bit32u newESP, newEBP, newESI, newEDI;
  Bit32u newEFLAGS, oldEFLAGS, newEIP;
  unsigned exception_no;
  Bit16u error_code;

//BX_DEBUG(( "TASKING: ENTER" ));

  invalidate_prefetch_q();

  // Discard any traps and inhibits for new context; traps will
  // resume upon return.
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR inhibit_mask = 0;




  // The following checks are made before calling task_switch(), for
  // JMP & CALL only.  These checks are NOT made for exceptions, interrupts, & IRET
  //
  //   1) TSS DPL must be >= CPL
  //   2) TSS DPL must be >= TSS selector RPL
  //   3) TSS descriptor is not busy.  TS(for IRET); GP(for JMP, CALL, INT)

  // Privilege and busy checks done in CALL, JUMP, INT, IRET

  exception_no = 256; // no exception
  error_code   = 0;
  oldEFLAGS = read_eflags();

  // Gather info about old TSS
  if (BX_CPU_THIS_PTR tr.cache.type <= 3) {
    // sanity check type: cannot have busy bit
    BX_ASSERT ((BX_CPU_THIS_PTR tr.cache.type & 2) == 0);
    obase32 = BX_CPU_THIS_PTR tr.cache.u.tss286.base;
    old_TSS_max   = 43;
    old_TSS_limit = BX_CPU_THIS_PTR tr.cache.u.tss286.limit;
    }
  else {
    obase32 = BX_CPU_THIS_PTR tr.cache.u.tss386.base;
    old_TSS_max   = 103;
    old_TSS_limit = BX_CPU_THIS_PTR tr.cache.u.tss386.limit_scaled;
    }

  // Gather info about new TSS
  if (tss_descriptor->type <= 3) { // {1,3}
    nbase32 = tss_descriptor->u.tss286.base; // new TSS.base
    new_TSS_max   = 43;
    new_TSS_limit = tss_descriptor->u.tss286.limit;
    }
  else { // tss_descriptor->type = {9,11}
    nbase32 = tss_descriptor->u.tss386.base; // new TSS.base
    new_TSS_max   = 103;
    new_TSS_limit = tss_descriptor->u.tss386.limit_scaled;
    }

  // Task State Seg must be present, else #NP(TSS selector)
  if (tss_descriptor->p==0) {
    BX_INFO(("task_switch: TSS.p == 0"));
    exception(BX_NP_EXCEPTION, tss_selector->value & 0xfffc, 0);
    }

  // TSS must have valid limit, else #TS(TSS selector)
  if (tss_selector->ti ||
      tss_descriptor->valid==0 ||
      new_TSS_limit < new_TSS_max) {
    BX_PANIC(("task_switch(): TR not valid"));
    exception(BX_TS_EXCEPTION, tss_selector->value & 0xfffc, 0);
    }
#if BX_SUPPORT_PAGING
  // Check that old TSS, new TSS, and all segment descriptors
  // used in the task switch are paged in.
  if (BX_CPU_THIS_PTR cr0.pg) {
    //BX_RW, BX_READ, BX_WRITE
    // Old TSS
    (void) dtranslate_linear(obase32, 0, /*rw*/ BX_WRITE);
    (void) dtranslate_linear(obase32+old_TSS_max, 0, /*rw*/ BX_WRITE);

    // New TSS
    (void) dtranslate_linear(nbase32, 0, /*rw*/ 0);
    (void) dtranslate_linear(nbase32+new_TSS_max, 0, /*rw*/ 0);

    // ??? fix RW above
    // ??? touch old/new TSS descriptors here when necessary.
    }
#endif // BX_SUPPORT_PAGING

  // Need to fetch all new registers and temporarily store them.

  if (tss_descriptor->type <= 3) {
    access_linear(nbase32 + 14, 2, 0, BX_READ, &temp16);
      newEIP = temp16; // zero out upper word
    access_linear(nbase32 + 16, 2, 0, BX_READ, &temp16);
      newEFLAGS = temp16;

    // incoming TSS is 16bit:
    //   - upper word of general registers is set to 0xFFFF
    //   - upper word of eflags is zero'd
    //   - FS, GS are zero'd
    //   - upper word of eIP is zero'd
    access_linear(nbase32 + 18, 2, 0, BX_READ, &temp16);
      newEAX = 0xffff0000 | temp16;
    access_linear(nbase32 + 20, 2, 0, BX_READ, &temp16);
      newECX = 0xffff0000 | temp16;
    access_linear(nbase32 + 22, 2, 0, BX_READ, &temp16);
      newEDX = 0xffff0000 | temp16;
    access_linear(nbase32 + 24, 2, 0, BX_READ, &temp16);
      newEBX = 0xffff0000 | temp16;
    access_linear(nbase32 + 26, 2, 0, BX_READ, &temp16);
      newESP = 0xffff0000 | temp16;
    access_linear(nbase32 + 28, 2, 0, BX_READ, &temp16);
      newEBP = 0xffff0000 | temp16;
    access_linear(nbase32 + 30, 2, 0, BX_READ, &temp16);
      newESI = 0xffff0000 | temp16;
    access_linear(nbase32 + 32, 2, 0, BX_READ, &temp16);
      newEDI = 0xffff0000 | temp16;

    access_linear(nbase32 + 34, 2, 0, BX_READ, &raw_es_selector);
    access_linear(nbase32 + 36, 2, 0, BX_READ, &raw_cs_selector);
    access_linear(nbase32 + 38, 2, 0, BX_READ, &raw_ss_selector);
    access_linear(nbase32 + 40, 2, 0, BX_READ, &raw_ds_selector);
    access_linear(nbase32 + 42, 2, 0, BX_READ, &raw_ldt_selector);

    raw_fs_selector = 0; // use a NULL selector
    raw_gs_selector = 0; // use a NULL selector
    // No CR3 change for 286 task switch
    newCR3 = 0;   // keep compiler happy (not used)
    trap_word = 0; // keep compiler happy (not used)
    }
  else {
    access_linear(nbase32 + 0x1c, 4, 0, BX_READ, &newCR3);
    access_linear(nbase32 + 0x20, 4, 0, BX_READ, &newEIP);
    access_linear(nbase32 + 0x24, 4, 0, BX_READ, &newEFLAGS);
    access_linear(nbase32 + 0x28, 4, 0, BX_READ, &newEAX);
    access_linear(nbase32 + 0x2c, 4, 0, BX_READ, &newECX);
    access_linear(nbase32 + 0x30, 4, 0, BX_READ, &newEDX);
    access_linear(nbase32 + 0x34, 4, 0, BX_READ, &newEBX);
    access_linear(nbase32 + 0x38, 4, 0, BX_READ, &newESP);
    access_linear(nbase32 + 0x3c, 4, 0, BX_READ, &newEBP);
    access_linear(nbase32 + 0x40, 4, 0, BX_READ, &newESI);
    access_linear(nbase32 + 0x44, 4, 0, BX_READ, &newEDI);
    access_linear(nbase32 + 0x48, 2, 0, BX_READ, &raw_es_selector);
    access_linear(nbase32 + 0x4c, 2, 0, BX_READ, &raw_cs_selector);
    access_linear(nbase32 + 0x50, 2, 0, BX_READ, &raw_ss_selector);
    access_linear(nbase32 + 0x54, 2, 0, BX_READ, &raw_ds_selector);
    access_linear(nbase32 + 0x58, 2, 0, BX_READ, &raw_fs_selector);
    access_linear(nbase32 + 0x5c, 2, 0, BX_READ, &raw_gs_selector);
    access_linear(nbase32 + 0x60, 2, 0, BX_READ, &raw_ldt_selector);
    access_linear(nbase32 + 0x64, 2, 0, BX_READ, &trap_word);
    // I/O Map Base Address ???
    }

#if 0
if (ss_descriptor.u.segment.d_b && (tss_descriptor->type<9)) {
  BX_DEBUG(( "++++++++++++++++++++++++++" ));
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid = 0;
  exception(BX_SS_EXCEPTION, raw_ss_selector & 0xfffc, 0);
  //exception(BX_TS_EXCEPTION, tss_selector->value & 0xfffc, 0);
  }
#endif


  //
  // Step 6: If JMP or IRET, clear busy bit in old task TSS descriptor,
  //         otherwise leave set.
  //

  // effect on Busy bit of old task
  if ( (source==BX_TASK_FROM_JUMP) || (source==BX_TASK_FROM_IRET) ) {
    // Bit is cleared
    access_linear(BX_CPU_THIS_PTR gdtr.base +
                  BX_CPU_THIS_PTR tr.selector.index*8 + 4,
                  4, 0, BX_READ, &temp32);
    temp32 &= ~0x00000200;
    access_linear(BX_CPU_THIS_PTR gdtr.base +
                  BX_CPU_THIS_PTR tr.selector.index*8 + 4,
                  4, 0, BX_WRITE, &temp32);
    }


  //
  // Step 7: If IRET, clear NT flag in temp image of EFLAGS, otherwise
  //         leave alone.
  //

  if (source == BX_TASK_FROM_IRET) {
    // NT flags in old task is cleared with an IRET
    oldEFLAGS &= ~0x00004000;
    }


  //
  // Step 8: Save dynamic state of old task.
  //

  if (BX_CPU_THIS_PTR tr.cache.type <= 3) {
    // sanity check: tr.cache.type cannot have busy bit
    BX_ASSERT ((BX_CPU_THIS_PTR tr.cache.type & 2) == 0);
    temp16 = IP; access_linear(obase32 + 14, 2, 0, BX_WRITE, &temp16);
    temp16 = oldEFLAGS; access_linear(obase32 + 16, 2, 0, BX_WRITE, &temp16);
    temp16 = AX; access_linear(obase32 + 18, 2, 0, BX_WRITE, &temp16);
    temp16 = CX; access_linear(obase32 + 20, 2, 0, BX_WRITE, &temp16);
    temp16 = DX; access_linear(obase32 + 22, 2, 0, BX_WRITE, &temp16);
    temp16 = BX; access_linear(obase32 + 24, 2, 0, BX_WRITE, &temp16);
    temp16 = SP; access_linear(obase32 + 26, 2, 0, BX_WRITE, &temp16);
    temp16 = BP; access_linear(obase32 + 28, 2, 0, BX_WRITE, &temp16);
    temp16 = SI; access_linear(obase32 + 30, 2, 0, BX_WRITE, &temp16);
    temp16 = DI; access_linear(obase32 + 32, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;
                 access_linear(obase32 + 34, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
                 access_linear(obase32 + 36, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
                 access_linear(obase32 + 38, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
                 access_linear(obase32 + 40, 2, 0, BX_WRITE, &temp16);
    }
  else {
    temp32 = EIP; access_linear(obase32 + 0x20, 4, 0, BX_WRITE, &temp32);
    temp32 = oldEFLAGS; access_linear(obase32 + 0x24, 4, 0, BX_WRITE, &temp32);
    temp32 = EAX; access_linear(obase32 + 0x28, 4, 0, BX_WRITE, &temp32);
    temp32 = ECX; access_linear(obase32 + 0x2c, 4, 0, BX_WRITE, &temp32);
    temp32 = EDX; access_linear(obase32 + 0x30, 4, 0, BX_WRITE, &temp32);
    temp32 = EBX; access_linear(obase32 + 0x34, 4, 0, BX_WRITE, &temp32);
    temp32 = ESP; access_linear(obase32 + 0x38, 4, 0, BX_WRITE, &temp32);
    temp32 = EBP; access_linear(obase32 + 0x3c, 4, 0, BX_WRITE, &temp32);
    temp32 = ESI; access_linear(obase32 + 0x40, 4, 0, BX_WRITE, &temp32);
    temp32 = EDI; access_linear(obase32 + 0x44, 4, 0, BX_WRITE, &temp32);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;
                  access_linear(obase32 + 0x48, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
                  access_linear(obase32 + 0x4c, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
                  access_linear(obase32 + 0x50, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
                  access_linear(obase32 + 0x54, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
                  access_linear(obase32 + 0x58, 2, 0, BX_WRITE, &temp16);
    temp16 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
                  access_linear(obase32 + 0x5c, 2, 0, BX_WRITE, &temp16);
    }



  //
  // Commit point.  At this point, we commit to the new
  // context.  If an unrecoverable error occurs in further
  // processing, we complete the task switch without performing
  // additional access and segment availablility checks and
  // generate the appropriate exception prior to beginning
  // execution of the new task.
  //


  // Task switch clears LE/L3/L2/L1/L0 in DR7
  BX_CPU_THIS_PTR dr7 &= ~0x00000155;


// effect on link field of new task
if ( source==BX_TASK_FROM_CALL_OR_INT ) {
  // set to selector of old task's TSS
  temp16 = BX_CPU_THIS_PTR tr.selector.value;
  access_linear(nbase32 + 0, 2, 0, BX_WRITE, &temp16);
  }



  //
  // Step 9: If call or interrupt, set the NT flag in the eflags
  //         image stored in new task's TSS.  If IRET or JMP,
  //         NT is restored from new TSS eflags image. (no change)
  //

  // effect on NT flag of new task
  if ( source==BX_TASK_FROM_CALL_OR_INT ) {
    newEFLAGS |= 0x4000; // flag is set
    }


  //
  // Step 10: If CALL, interrupt, or JMP, set busy flag in new task's
  //          TSS descriptor.  If IRET, leave set.
  //

  if ( (source==BX_TASK_FROM_JUMP) || (source==BX_TASK_FROM_CALL_OR_INT) ) {
    // set the new task's busy bit
    access_linear(BX_CPU_THIS_PTR gdtr.base + tss_selector->index*8 + 4,
                  4, 0, BX_READ, &dword2);
    dword2 |= 0x00000200;
    access_linear(BX_CPU_THIS_PTR gdtr.base + tss_selector->index*8 + 4,
                  4, 0, BX_WRITE, &dword2);
    }


  //
  // Step 11: Set TS flag in the CR0 image stored in the new task TSS.
  //

  // set TS bit in CR0 register
  BX_CPU_THIS_PTR cr0.ts = 1;
  BX_CPU_THIS_PTR cr0.val32 |= 0x00000008;


  //
  // Step 12: Load the task register with the segment selector and
  //          descriptor for the new task TSS.
  //

  BX_CPU_THIS_PTR tr.selector = *tss_selector;
  BX_CPU_THIS_PTR tr.cache    = *tss_descriptor;
  // Reset the busy-flag, because all functions expect non-busy types in
  // tr.cache.  From Peter Lammich <peterl@sourceforge.net>.
  BX_CPU_THIS_PTR tr.cache.type &= ~2;


  //
  // Step 13: Load the new task (dynamic) state from new TSS.
  //          Any errors associated with loading and qualification of
  //          segment descriptors in this step occur in the new task's
  //          context.  State loaded here includes LDTR, CR3,
  //          EFLAGS, EIP, general purpose registers, and segment
  //          descriptor parts of the segment registers.
  //

  if (tss_descriptor->type >= 9) {
    CR3_change(newCR3); // Tell paging unit about new cr3 value
    BX_DEBUG (("task_switch changing CR3 to 0x%08x\n", newCR3));
    BX_INSTR_TLB_CNTRL(BX_INSTR_TASKSWITCH, newCR3);
    }

  BX_CPU_THIS_PTR prev_eip = EIP = newEIP;
  write_eflags(newEFLAGS, 1,1,1,1);
  EAX = newEAX;
  ECX = newECX;
  EDX = newEDX;
  EBX = newEBX;
  ESP = newESP;
  EBP = newEBP;
  ESI = newESI;
  EDI = newEDI;

  // Fill in selectors for all segment registers.  If errors
  // occur later, the selectors will at least be loaded.
  parse_selector(raw_es_selector, &es_selector);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector = es_selector;
  parse_selector(raw_cs_selector, &cs_selector);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector = cs_selector;
  parse_selector(raw_ss_selector, &ss_selector);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector = ss_selector;
  parse_selector(raw_ds_selector, &ds_selector);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector = ds_selector;
  parse_selector(raw_fs_selector, &fs_selector);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector = fs_selector;
  parse_selector(raw_gs_selector, &gs_selector);
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector = gs_selector;
  parse_selector(raw_ldt_selector, &ldt_selector);
  BX_CPU_THIS_PTR ldtr.selector                 = ldt_selector;

  // Start out with invalid descriptor caches, fill in
  // with values only as they are validated.
  BX_CPU_THIS_PTR ldtr.cache.valid = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid = 0;


// need to test valid bit in fetch_raw_descriptor?()
// or set limit to 0 instead when LDT is loaded with
// null. ??? +++
BX_CPU_THIS_PTR ldtr.cache.u.ldt.limit = 0;

  // LDTR
  if (ldt_selector.ti) {
    // LDT selector must be in GDT
    BX_INFO(("task_switch: bad LDT selector TI=1"));
    exception_no = BX_TS_EXCEPTION;
    error_code   = raw_ldt_selector & 0xfffc;
    goto post_exception;
    }

// ??? is LDT loaded in v8086 mode
  if ( (raw_ldt_selector & 0xfffc) != 0 ) {
    Boolean good;
    good = fetch_raw_descriptor2(&ldt_selector, &dword1, &dword2);
    if (!good) {
      BX_INFO(("task_switch: bad LDT fetch"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ldt_selector & 0xfffc;
      goto post_exception;
      }

    parse_descriptor(dword1, dword2, &ldt_descriptor);

    // LDT selector of new task is valid, else #TS(new task's LDT)
    if (ldt_descriptor.valid==0 ||
        ldt_descriptor.type!=2 ||
        ldt_descriptor.segment ||
        ldt_descriptor.u.ldt.limit<7) {
      BX_INFO(("task_switch: bad LDT segment"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ldt_selector & 0xfffc;
      goto post_exception;
      }

    // LDT of new task is present in memory, else #TS(new tasks's LDT)
    else if (ldt_descriptor.p==0) {
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ldt_selector & 0xfffc;
      goto post_exception;
      }
    // All checks pass, fill in LDTR shadow cache
    BX_CPU_THIS_PTR ldtr.cache = ldt_descriptor;
    }
  else {
    // NULL LDT selector is OK, leave cache invalid
    }

  if (v8086_mode()) {
    // load seg regs as 8086 registers
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], raw_cs_selector);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], raw_ss_selector);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], raw_ds_selector);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], raw_es_selector);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], raw_fs_selector);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], raw_gs_selector);
    }
  else {

  // CS
  if ( (raw_cs_selector & 0xfffc) != 0 ) {
    Boolean good;
    good = fetch_raw_descriptor2(&cs_selector, &dword1, &dword2);
    if (!good) {
      BX_INFO(("task_switch: bad CS fetch"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_cs_selector & 0xfffc;
      goto post_exception;
      }

    parse_descriptor(dword1, dword2, &cs_descriptor);

    // CS descriptor AR byte must indicate code segment else #TS(CS)
    if (cs_descriptor.valid==0 || cs_descriptor.segment==0 ||
        cs_descriptor.u.segment.executable==0) {
      BX_PANIC(("task_switch: CS not valid executable seg"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_cs_selector & 0xfffc;
      goto post_exception;
      }
    // if non-conforming then DPL must equal selector RPL else #TS(CS)
    else if (cs_descriptor.u.segment.c_ed==0 &&
        cs_descriptor.dpl!=cs_selector.rpl) {
      BX_INFO(("task_switch: non-conforming: CS.dpl!=CS.RPL"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_cs_selector & 0xfffc;
      goto post_exception;
      }
    // if conforming then DPL must be <= selector RPL else #TS(CS)
    else if (cs_descriptor.u.segment.c_ed &&
        cs_descriptor.dpl>cs_selector.rpl) {
      BX_INFO(("task_switch: conforming: CS.dpl>RPL"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_cs_selector & 0xfffc;
      goto post_exception;
      }
    // Code segment is present in memory, else #NP(new code segment)
    else if (cs_descriptor.p==0) {
      BX_PANIC(("task_switch: CS.p==0"));
      exception_no = BX_NP_EXCEPTION;
      error_code   = raw_cs_selector & 0xfffc;
      goto post_exception;
      }
    // All checks pass, fill in shadow cache
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache    = cs_descriptor;
    }
  else {
    // If new cs selector is null #TS(CS)
    BX_PANIC(("task_switch: CS NULL"));
    exception_no = BX_TS_EXCEPTION;
    error_code   = raw_cs_selector & 0xfffc;
    goto post_exception;
    }


  // SS
  if ( (raw_ss_selector & 0xfffc) != 0 ) {
    Boolean good;
    good = fetch_raw_descriptor2(&ss_selector, &dword1, &dword2);
    if (!good) {
      BX_INFO(("task_switch: bad SS fetch"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ss_selector & 0xfffc;
      goto post_exception;
      }

    parse_descriptor(dword1, dword2, &ss_descriptor);
    // SS selector must be within its descriptor table limits else #TS(SS)
    // SS descriptor AR byte must must indicate writable data segment,
    // else #TS(SS)
    if (ss_descriptor.valid==0 ||
        ss_descriptor.segment==0 ||
        ss_descriptor.u.segment.executable ||
        ss_descriptor.u.segment.r_w==0) {
      BX_INFO(("task_switch: SS not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ss_selector & 0xfffc;
      goto post_exception;
      }

    //
    // Stack segment is present in memory, else #SF(new stack segment)
    //
    else if (ss_descriptor.p==0) {
      BX_PANIC(("task_switch: SS not present"));
      exception_no = BX_SS_EXCEPTION;
      error_code   = raw_ss_selector & 0xfffc;
      goto post_exception;
      }

    // Stack segment DPL matches CS.RPL, else #TS(new stack segment)
    else if (ss_descriptor.dpl != cs_selector.rpl) {
      BX_PANIC(("task_switch: SS.rpl != CS.RPL"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ss_selector & 0xfffc;
      goto post_exception;
      }

    // Stack segment DPL matches selector RPL, else #TS(new stack segment)
    else if (ss_descriptor.dpl != ss_selector.rpl) {
      BX_PANIC(("task_switch: SS.dpl != SS.rpl"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ss_selector & 0xfffc;
      goto post_exception;
      }

#if 0
    // +++
    else if (ss_descriptor.u.segment.d_b && (tss_descriptor->type<9)) {
      BX_DEBUG(( "++++++++++++++++++++++++++" ));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ss_selector & 0xfffc;
      goto post_exception;
      }
#endif
    // All checks pass, fill in shadow cache
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache    = ss_descriptor;
    }
  else {
    // SS selector is valid, else #TS(new stack segment)
    BX_PANIC(("task_switch: SS NULL"));
    exception_no = BX_TS_EXCEPTION;
    error_code   = raw_ss_selector & 0xfffc;
    goto post_exception;
    }


  //   if new selector is not null then perform following checks:
  //     index must be within its descriptor table limits else #TS(selector)
  //     AR byte must indicate data or readable code else #TS(selector)
  //     if data or non-conforming code then:
  //       DPL must be >= CPL else #TS(selector)
  //       DPL must be >= RPL else #TS(selector)
  //     AR byte must indicate PRESENT else #NP(selector)
  //     load cache with new segment descriptor and set valid bit



  // DS
  if ( (raw_ds_selector & 0xfffc) != 0 ) {
    Boolean good;
    good = fetch_raw_descriptor2(&ds_selector, &dword1, &dword2);
    if (!good) {
      BX_INFO(("task_switch: bad DS fetch"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ds_selector & 0xfffc;
      goto post_exception;
      }

    parse_descriptor(dword1, dword2, &ds_descriptor);
    if (ds_descriptor.valid==0 || ds_descriptor.segment==0 ||
        (ds_descriptor.u.segment.executable &&
         ds_descriptor.u.segment.r_w==0)) {
      BX_PANIC(("task_switch: DS not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ds_selector & 0xfffc;
      goto post_exception;
      }
    // if data or non-conforming code
    else if (ds_descriptor.type<12 &&
        (ds_descriptor.dpl<cs_selector.rpl ||
         ds_descriptor.dpl<ds_selector.rpl)) {
      BX_PANIC(("task_switch: DS.dpl not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_ds_selector & 0xfffc;
      goto post_exception;
      }
    else if (ds_descriptor.p==0) {
      BX_PANIC(("task_switch: DS.p==0"));
      exception_no = BX_NP_EXCEPTION;
      error_code   = raw_ds_selector & 0xfffc;
      goto post_exception;
      }
    // All checks pass, fill in shadow cache
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache    = ds_descriptor;
    }
  else {
    // NULL DS selector is OK, leave cache invalid
    }

  // ES
  if ( (raw_es_selector & 0xfffc) != 0 ) {
    Boolean good;
    good = fetch_raw_descriptor2(&es_selector, &dword1, &dword2);
    if (!good) {
      BX_INFO(("task_switch: bad ES fetch"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_es_selector & 0xfffc;
      goto post_exception;
      }

    parse_descriptor(dword1, dword2, &es_descriptor);
    if (es_descriptor.valid==0 || es_descriptor.segment==0 ||
        (es_descriptor.u.segment.executable &&
         es_descriptor.u.segment.r_w==0)) {
      BX_PANIC(("task_switch: ES not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_es_selector & 0xfffc;
      goto post_exception;
      }
    // if data or non-conforming code
    else if (es_descriptor.type<12 &&
        (es_descriptor.dpl<cs_selector.rpl ||
         es_descriptor.dpl<es_selector.rpl)) {
      BX_PANIC(("task_switch: ES.dpl not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_es_selector & 0xfffc;
      goto post_exception;
      }
    else if (es_descriptor.p==0) {
      BX_PANIC(("task_switch: ES.p==0"));
      exception_no = BX_NP_EXCEPTION;
      error_code   = raw_es_selector & 0xfffc;
      goto post_exception;
      }
    // All checks pass, fill in shadow cache
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache    = es_descriptor;
    }
  else {
    // NULL ES selector is OK, leave cache invalid
    }


  // FS
  if ( (raw_fs_selector & 0xfffc) != 0 ) { // not NULL
    Boolean good;
    good = fetch_raw_descriptor2(&fs_selector, &dword1, &dword2);
    if (!good) {
      BX_INFO(("task_switch: bad FS fetch"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_fs_selector & 0xfffc;
      goto post_exception;
      }

    parse_descriptor(dword1, dword2, &fs_descriptor);
    if (fs_descriptor.valid==0 || fs_descriptor.segment==0 ||
        (fs_descriptor.u.segment.executable &&
         fs_descriptor.u.segment.r_w==0)) {
      BX_PANIC(("task_switch: FS not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_fs_selector & 0xfffc;
      goto post_exception;
      }
    // if data or non-conforming code
    else if (fs_descriptor.type<12 &&
        (fs_descriptor.dpl<cs_selector.rpl ||
         fs_descriptor.dpl<fs_selector.rpl)) {
      BX_PANIC(("task_switch: FS.dpl not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_fs_selector & 0xfffc;
      goto post_exception;
      }
    else if (fs_descriptor.p==0) {
      BX_PANIC(("task_switch: FS.p==0"));
      exception_no = BX_NP_EXCEPTION;
      error_code   = raw_fs_selector & 0xfffc;
      goto post_exception;
      }
    // All checks pass, fill in shadow cache
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache    = fs_descriptor;
    }
  else {
    // NULL FS selector is OK, leave cache invalid
    }

  // GS
  if ( (raw_gs_selector & 0xfffc) != 0 ) {
    Boolean good;
    good = fetch_raw_descriptor2(&gs_selector, &dword1, &dword2);
    if (!good) {
      BX_INFO(("task_switch: bad GS fetch"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_gs_selector & 0xfffc;
      goto post_exception;
      }

    parse_descriptor(dword1, dword2, &gs_descriptor);
    if (gs_descriptor.valid==0 || gs_descriptor.segment==0 ||
        (gs_descriptor.u.segment.executable &&
         gs_descriptor.u.segment.r_w==0)) {
      BX_PANIC(("task_switch: GS not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_gs_selector & 0xfffc;
      goto post_exception;
      }
    // if data or non-conforming code
    else if (gs_descriptor.type<12 &&
        (gs_descriptor.dpl<cs_selector.rpl ||
         gs_descriptor.dpl<gs_selector.rpl)) {
      BX_PANIC(("task_switch: GS.dpl not valid"));
      exception_no = BX_TS_EXCEPTION;
      error_code   = raw_gs_selector & 0xfffc;
      goto post_exception;
      }
    else if (gs_descriptor.p==0) {
      BX_PANIC(("task_switch: GS.p==0"));
      //exception(BX_NP_EXCEPTION, raw_gs_selector & 0xfffc, 0);
      exception_no = BX_NP_EXCEPTION;
      error_code   = raw_gs_selector & 0xfffc;
      goto post_exception;
      }
    // All checks pass, fill in shadow cache
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache    = gs_descriptor;
    }
  else {
    // NULL GS selector is OK, leave cache invalid
    }

    }


  if ((tss_descriptor->type>=9) && (trap_word & 0x0001)) {
    BX_CPU_THIS_PTR debug_trap |= 0x00008000; // BT flag in DR6
    BX_CPU_THIS_PTR async_event = 1; // so processor knows to check
    BX_INFO(("task_switch: T bit set in new TSS."));
    }



  //
  // Step 14: Begin execution of new task.
  //
//BX_DEBUG(( "TASKING: LEAVE" ));
  return;

post_exception:
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR inhibit_mask = 0;
  BX_INFO(("task switch: posting exception %u after commit point",
    exception_no));
  exception(exception_no, error_code, 0);
  return;
}


  void
BX_CPU_C::get_SS_ESP_from_TSS(unsigned pl, Bit16u *ss, Bit32u *esp)
{
  if (BX_CPU_THIS_PTR tr.cache.valid==0)
    BX_PANIC(("get_SS_ESP_from_TSS: TR.cache invalid"));

  if (BX_CPU_THIS_PTR tr.cache.type==9) {
    // 32-bit TSS
    Bit32u TSSstackaddr;

    TSSstackaddr = 8*pl + 4;
    if ( (TSSstackaddr+7) >
         BX_CPU_THIS_PTR tr.cache.u.tss386.limit_scaled )
      exception(BX_TS_EXCEPTION,
                BX_CPU_THIS_PTR tr.selector.value & 0xfffc, 0);

    access_linear(BX_CPU_THIS_PTR tr.cache.u.tss386.base +
      TSSstackaddr+4, 2, 0, BX_READ, ss);
    access_linear(BX_CPU_THIS_PTR tr.cache.u.tss386.base +
      TSSstackaddr,   4, 0, BX_READ, esp);
    }
  else if (BX_CPU_THIS_PTR tr.cache.type==1) {
    // 16-bit TSS
    Bit16u temp16;
    Bit32u TSSstackaddr;

    TSSstackaddr = 4*pl + 2;
    if ( (TSSstackaddr+4) > BX_CPU_THIS_PTR tr.cache.u.tss286.limit )
      exception(BX_TS_EXCEPTION,
                BX_CPU_THIS_PTR tr.selector.value & 0xfffc, 0);

    access_linear(BX_CPU_THIS_PTR tr.cache.u.tss286.base +
      TSSstackaddr+2, 2, 0, BX_READ, ss);
    access_linear(BX_CPU_THIS_PTR tr.cache.u.tss286.base +
      TSSstackaddr,   2, 0, BX_READ, &temp16);
    *esp = temp16; // truncate
    }
  else {
    BX_PANIC(("get_SS_ESP_from_TSS: TR is bogus type (%u)",
             (unsigned) BX_CPU_THIS_PTR tr.cache.type));
    }
}
#endif



#else  // BX_SUPPORT_TASKING


// for non-support of hardware tasking

#if BX_CPU_LEVEL >= 2
  /* corresponds to SWITCH_TASKS algorithm in Intel documentation */
  void
BX_CPU_C::task_switch(bx_selector_t *selector,
                 bx_descriptor_t *descriptor, unsigned source,
                 Bit32u dword1, Bit32u dword2)
{
  UNUSED(selector);
  UNUSED(descriptor);
  UNUSED(source);
  UNUSED(dword1);
  UNUSED(dword2);

  BX_INFO(("task_switch(): not complete"));
}
#endif

#endif // BX_SUPPORT_TASKING
