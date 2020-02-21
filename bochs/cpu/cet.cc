/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2019 Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "msr.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_CET

const Bit64u BX_CET_SHADOW_STACK_ENABLED                   = (1 << 0);
const Bit64u BX_CET_SHADOW_STACK_WRITE_ENABLED             = (1 << 1);
const Bit64u BX_CET_ENDBRANCH_ENABLED                      = (1 << 2);
const Bit64u BX_CET_LEGACY_INDIRECT_BRANCH_TREATMENT       = (1 << 3);
const Bit64u BX_CET_ENABLE_NO_TRACK_INDIRECT_BRANCH_PREFIX = (1 << 4);
const Bit64u BX_CET_SUPPRESS_DIS                           = (1 << 5);
const Bit64u BX_CET_SUPPRESS_INDIRECT_BRANCH_TRACKING      = (1 << 10);
const Bit64u BX_CET_WAIT_FOR_ENBRANCH                      = (1 << 11);

bx_bool is_invalid_cet_control(bx_address val)
{
  if ((val & (BX_CET_SUPPRESS_INDIRECT_BRANCH_TRACKING | BX_CET_WAIT_FOR_ENBRANCH)) == 
             (BX_CET_SUPPRESS_INDIRECT_BRANCH_TRACKING | BX_CET_WAIT_FOR_ENBRANCH)) return true;

  if (val & 0x3c0) return true; // reserved bits check
  return false;
}

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::ShadowStackEnabled(unsigned cpl)
{
  return BX_CPU_THIS_PTR cr4.get_CET() && protected_mode() &&
         BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] & BX_CET_SHADOW_STACK_ENABLED;
}

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::ShadowStackWriteEnabled(unsigned cpl)
{
  return BX_CPU_THIS_PTR cr4.get_CET() && protected_mode() &&
        (BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] & (BX_CET_SHADOW_STACK_ENABLED | BX_CET_SHADOW_STACK_WRITE_ENABLED)) == (BX_CET_SHADOW_STACK_ENABLED | BX_CET_SHADOW_STACK_WRITE_ENABLED);
}

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::EndbranchEnabled(unsigned cpl)
{
  return BX_CPU_THIS_PTR cr4.get_CET() && protected_mode() &&
         BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] & BX_CET_ENDBRANCH_ENABLED;
}

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::EndbranchEnabledAndNotSuppressed(unsigned cpl)
{
  return BX_CPU_THIS_PTR cr4.get_CET() && protected_mode() &&
        (BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] & (BX_CET_ENDBRANCH_ENABLED | BX_CET_SUPPRESS_INDIRECT_BRANCH_TRACKING)) == BX_CET_ENDBRANCH_ENABLED;
}

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::WaitingForEndbranch(unsigned cpl)
{
  return BX_CPU_THIS_PTR cr4.get_CET() && protected_mode() &&
        (BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] & (BX_CET_ENDBRANCH_ENABLED | BX_CET_WAIT_FOR_ENBRANCH)) == (BX_CET_ENDBRANCH_ENABLED | BX_CET_WAIT_FOR_ENBRANCH);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::track_indirect(unsigned cpl)
{
  if (EndbranchEnabled(cpl)) {
    BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] |= BX_CET_WAIT_FOR_ENBRANCH;
    BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] &= ~BX_CET_SUPPRESS_INDIRECT_BRANCH_TRACKING;
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::track_indirect_if_not_suppressed(bxInstruction_c *i, unsigned cpl)
{
  if (EndbranchEnabledAndNotSuppressed(cpl)) {
    if (i->segOverrideCet() == BX_SEG_REG_DS && (BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] & BX_CET_ENABLE_NO_TRACK_INDIRECT_BRANCH_PREFIX) != 0)
      return;

    BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] |= BX_CET_WAIT_FOR_ENBRANCH;
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::reset_endbranch_tracker(unsigned cpl, bx_bool suppress)
{
  BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] &= ~(BX_CET_WAIT_FOR_ENBRANCH | BX_CET_SUPPRESS_INDIRECT_BRANCH_TRACKING);
  if (suppress && !(BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] & BX_CET_SUPPRESS_DIS))
    BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] |= BX_CET_SUPPRESS_INDIRECT_BRANCH_TRACKING;
}

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::LegacyEndbranchTreatment(unsigned cpl)
{
  if (BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3] & BX_CET_LEGACY_INDIRECT_BRANCH_TREATMENT)
  {
    bx_address lip = get_laddr(BX_SEG_REG_CS, RIP);
    bx_address bitmap_addr = LPFOf(BX_CPU_THIS_PTR msr.ia32_cet_control[cpl==3]) + ((lip & BX_CONST64(0xFFFFFFFFFFFF)) >> 15);
    unsigned bitmap_index = (lip>>12) & 0x7;
    Bit8u bitmap = system_read_byte(bitmap_addr);
    if ((bitmap & (1 << bitmap_index)) != 0) {
      reset_endbranch_tracker(cpl, true);
      return false;
    }
  }
  return true;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INCSSPD(bxInstruction_c *i)
{
  if (! ShadowStackEnabled(CPL)) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit32u src = BX_READ_32BIT_REG(i->dst()) & 0xff;
  Bit32u tmpsrc = (src == 0) ? 1 : src;

  shadow_stack_read_dword(SSP, CPL);
  shadow_stack_read_dword(SSP + (tmpsrc-1) * 4, CPL);
  SSP += src*4;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INCSSPQ(bxInstruction_c *i)
{
  if (! ShadowStackEnabled(CPL)) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit32u src = BX_READ_32BIT_REG(i->dst()) & 0xff;
  Bit32u tmpsrc = (src == 0) ? 1 : src;

  shadow_stack_read_qword(SSP, CPL);
  shadow_stack_read_qword(SSP + (tmpsrc-1) * 8, CPL);
  SSP += src*8;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDSSPD(bxInstruction_c *i)
{
  if (ShadowStackEnabled(CPL)) {
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(BX_32BIT_REG_SSP));
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDSSPQ(bxInstruction_c *i)
{
  if (ShadowStackEnabled(CPL)) {
    BX_WRITE_64BIT_REG(i->dst(), SSP);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SAVEPREVSSP(bxInstruction_c *i)
{
  if (! ShadowStackEnabled(CPL)) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (SSP & 7) {
    BX_ERROR(("%s: shadow stack not aligned to 8 byte boundary", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit64u previous_ssp_token = shadow_stack_read_qword(SSP, CPL);

  // If the CF flag indicates there was a alignment hole on current shadow stack then pop that alignment hole
  // Note that the alignment hole can be present only when in legacy/compatibility mode
  if (BX_CPU_THIS_PTR get_CF()) {
    if (long64_mode()) {
      BX_ERROR(("%s: shadow stack alignment hole in long64 mode", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }
    else {
      // pop the alignment hole
      if (shadow_stack_pop_32() != 0) {
        BX_ERROR(("%s: shadow stack alignment hole must be zero", i->getIaOpcodeNameShort()));
        exception(BX_GP_EXCEPTION, 0);
      }
    }
  }

  if ((previous_ssp_token & 0x02) == 0) {
    BX_ERROR(("%s: previous SSP token reserved bits set", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (!long64_mode() && GET32H(previous_ssp_token) != 0) {
    BX_ERROR(("%s: previous SSP token reserved bits set", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // Save Prev SSP from previous_ssp_token to the old shadow stack at next 8 byte aligned address
  Bit64u old_ssp = previous_ssp_token & ~BX_CONST64(0x03);
  Bit64u tmp = old_ssp | long64_mode();
  shadow_stack_write_dword(old_ssp - 4, CPL, 0);
  old_ssp = old_ssp & ~BX_CONST64(0x07);
  shadow_stack_write_qword(old_ssp - 8, CPL, tmp);

  SSP += 8;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RSTORSSP(bxInstruction_c *i)
{
  if (! ShadowStackEnabled(CPL)) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 8);
  if (laddr & 0x7) {
    BX_ERROR(("%s: SSP_LA must be 8 bytes aligned", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit64u previous_ssp_token = SSP | long64_mode() | 0x02;

// should be done atomically
  Bit64u SSP_tmp = shadow_stack_read_qword(laddr, CPL); // should be LWSI
  if ((SSP_tmp & 0x03) != long64_mode()) {
    BX_ERROR(("%s: CS.L of shadow stack token doesn't match or bit1 is not 0", i->getIaOpcodeNameShort()));
    exception(BX_CP_EXCEPTION, BX_CP_RSTORSSP);
  }
  if (!long64_mode() && GET32H(SSP_tmp) != 0) {
    BX_ERROR(("%s: 64-bit SSP token not in 64-bit mode", i->getIaOpcodeNameShort()));
    exception(BX_CP_EXCEPTION, BX_CP_RSTORSSP);
  }

  Bit64u tmp = SSP_tmp & ~BX_CONST64(0x01);
  tmp = (tmp-8) & ~BX_CONST64(0x07);
  if (tmp != laddr) {
    BX_ERROR(("%s: address in SSP token doesn't match requested top of stack", i->getIaOpcodeNameShort()));
    exception(BX_CP_EXCEPTION, BX_CP_RSTORSSP);
  }
  shadow_stack_write_qword(laddr, CPL, previous_ssp_token);
// should be done atomically

  SSP = laddr;

  clearEFlagsOSZAPC();
  // Set the CF if the SSP in the restore token was 4 byte aligned and not 8 byte aligned i.e. there is an alignment hole
  if (SSP_tmp & 0x04) assert_CF();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRSSD(bxInstruction_c *i)
{
  if (! ShadowStackWriteEnabled(CPL)) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 4);
  if (laddr & 0x3) {
    BX_ERROR(("%s: must be 4 bytes aligned", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }
  shadow_stack_write_dword(laddr, CPL, BX_READ_32BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRSSQ(bxInstruction_c *i)
{
  if (! ShadowStackWriteEnabled(CPL)) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 8);
  if (laddr & 0x7) {
    BX_ERROR(("%s: must be 8 bytes aligned", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }
  shadow_stack_write_qword(laddr, CPL, BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRUSSD(bxInstruction_c *i)
{
  if (!BX_CPU_THIS_PTR cr4.get_CET()) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("%s: CPL != 0", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 4);
  if (laddr & 0x3) {
    BX_ERROR(("%s: must be 4 bytes aligned", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }
  shadow_stack_write_dword(laddr, 3, BX_READ_32BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRUSSQ(bxInstruction_c *i)
{
  if (!BX_CPU_THIS_PTR cr4.get_CET()) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("%s: CPL != 0", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 8);
  if (laddr & 0x7) {
    BX_ERROR(("%s: must be 8 bytes aligned", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }
  shadow_stack_write_qword(laddr, 3, BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETSSBSY(bxInstruction_c *i)
{
  if (! ShadowStackEnabled(0)) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("%s: CPL != 0", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit64u ssp_laddr = BX_CPU_THIS_PTR msr.ia32_pl_ssp[0];
  if (ssp_laddr & 0x7) {
    BX_ERROR(("%s: SSP_LA not aligned to 8 bytes boundary", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (!shadow_stack_atomic_set_busy(ssp_laddr, CPL)) {
    BX_ERROR(("%s: failed to set SSP busy bit", i->getIaOpcodeNameShort()));
    exception(BX_CP_EXCEPTION, BX_CP_SETSSBSY);
  }
  
  SSP = ssp_laddr;  

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLRSSBSY(bxInstruction_c *i)
{
  if (! ShadowStackEnabled(0)) {
    BX_ERROR(("%s: shadow stack not enabled", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("%s: CPL != 0", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 8);
  if (laddr & 0x7) {
    BX_ERROR(("%s: SSP_LA not aligned to 8 bytes boundary", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bool invalid_token = shadow_stack_atomic_clear_busy(laddr, CPL);
  clearEFlagsOSZAPC();
  if (invalid_token) assert_CF();
  SSP = 0;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ENDBRANCH32(bxInstruction_c *i)
{
  if (! long64_mode()) {
    reset_endbranch_tracker(CPL);
    BX_NEXT_INSTR(i);
  }

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ENDBRANCH64(bxInstruction_c *i)
{
  if (long64_mode()) {
    reset_endbranch_tracker(CPL);
    BX_NEXT_INSTR(i);
  }

  BX_NEXT_TRACE(i);
}

#endif
