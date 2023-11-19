/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023 Stanislav Shwartsman
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
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_AVX

void BX_CPU_C::avx_masked_load8(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, Bit64u mask)
{
  unsigned len = i->getVL();

  if (i->as64L()) {
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    for (unsigned n=0; n < BYTE_ELEMENTS(len); n++) {
       if (mask & (BX_CONST64(1)<<n)) {
          if (! IsCanonical(laddr + n))
             exception(int_number(i->seg()), 0);
       }
    }
  }

  for (int n=BYTE_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (BX_CONST64(1)<<n))
       op->vmmubyte(n) = read_virtual_byte(i->seg(), eaddr + n);
    else
       op->vmmubyte(n) = 0;
  }
}

void BX_CPU_C::avx_masked_load16(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

  if (i->as64L()) {
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
       if (mask & (1<<n)) {
          if (! IsCanonical(laddr + 2*n))
             exception(int_number(i->seg()), 0);
       }
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (int n=WORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       op->vmm16u(n) = read_virtual_word(i->seg(), eaddr + 2*n);
    else
       op->vmm16u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

void BX_CPU_C::avx_masked_load32(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

  if (i->as64L()) {
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
       if (mask & (1<<n)) {
          if (! IsCanonical(laddr + 4*n))
             exception(int_number(i->seg()), 0);
       }
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (int n=DWORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       op->vmm32u(n) = read_virtual_dword(i->seg(), eaddr + 4*n);
    else
       op->vmm32u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

void BX_CPU_C::avx_masked_load64(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

  if (i->as64L()) {
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
       if (mask & (1<<n)) {
          if (! IsCanonical(laddr + 8*n))
             exception(int_number(i->seg()), 0);
       }
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (int n=QWORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       op->vmm64u(n) = read_virtual_qword(i->seg(), eaddr + 8*n);
    else
       op->vmm64u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

void BX_CPU_C::avx_masked_store8(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit64u mask)
{
  unsigned len = i->getVL();

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    for (unsigned n=0; n < BYTE_ELEMENTS(len); n++) {
      if (mask & (BX_CONST64(1)<<n)) {
        if (! IsCanonical(laddr + n))
           exception(int_number(i->seg()), 0);
      }
    }
  }
#endif

  // see if you can successfully write all the elements first
  for (int n=BYTE_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (BX_CONST64(1)<<n))
       read_RMW_virtual_byte(i->seg(), eaddr + n); // no lock
  }

  for (unsigned n=0; n < BYTE_ELEMENTS(len); n++) {
    if (mask & (BX_CONST64(1)<<n))
       write_virtual_byte(i->seg(), eaddr + n, op->vmmubyte(n));
  }
}

void BX_CPU_C::avx_masked_store16(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
      if (mask & (1<<n)) {
        if (! IsCanonical(laddr + 2*n))
           exception(int_number(i->seg()), 0);
      }
    }
  }
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  // see if you can successfully write all the elements first
  for (int n=WORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       read_RMW_virtual_word(i->seg(), eaddr + 2*n); // no lock
  }

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    if (mask & (1<<n))
       write_virtual_word(i->seg(), eaddr + 2*n, op->vmm16u(n));
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

void BX_CPU_C::avx_masked_store32(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
      if (mask & (1<<n)) {
        if (! IsCanonical(laddr + 4*n))
           exception(int_number(i->seg()), 0);
      }
    }
  }
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  // see if you can successfully write all the elements first
  for (int n=DWORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       read_RMW_virtual_dword(i->seg(), eaddr + 4*n); // no lock
  }

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    if (mask & (1<<n))
       write_virtual_dword(i->seg(), eaddr + 4*n, op->vmm32u(n));
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

void BX_CPU_C::avx_masked_store64(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
      if (mask & (1<<n)) {
        if (! IsCanonical(laddr + 8*n))
           exception(int_number(i->seg()), 0);
      }
    }
  }
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  // see if you can successfully write all the elements first
  for (int n=QWORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       read_RMW_virtual_qword(i->seg(), eaddr + 8*n); // no lock
  }

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    if (mask & (1<<n))
       write_virtual_qword(i->seg(), eaddr + 8*n, op->vmm64u(n));
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

#endif // BX_SUPPORT_AVX

#if BX_SUPPORT_EVEX

#include "simd_int.h"

void BX_CPU_C::avx512_write_regb_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned len, Bit64u opmask)
{
  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 16)
      xmm_zero_pblendb(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), (Bit32u) opmask);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 16)
      xmm_pblendb(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), (Bit32u) opmask);
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

void BX_CPU_C::avx512_write_regw_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned len, Bit32u opmask)
{
  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_zero_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), opmask);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), opmask);
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

void BX_CPU_C::avx512_write_regd_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned len, Bit32u opmask)
{
  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 4)
      xmm_zero_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), opmask);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), opmask);
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

void BX_CPU_C::avx512_write_regq_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned len, Bit32u opmask)
{
  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 2)
      xmm_zero_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), opmask);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), opmask);
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

#endif
