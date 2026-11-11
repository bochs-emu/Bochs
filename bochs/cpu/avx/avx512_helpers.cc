/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023-2026 Stanislav Shwartsman
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

#include "scalar_arith.h"

#if BX_SUPPORT_AVX

// mask with one bit set for every element of the vector
BX_CPP_INLINE Bit64u vector_elements_mask(unsigned num_elements)
{
  return (num_elements >= 64) ? BX_CONST64(0xffffffffffffffff) : ((BX_CONST64(1) << num_elements) - 1);
}

void BX_CPU_C::avx_masked_load8(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, Bit64u mask)
{
  unsigned len = i->getVL();

  Bit64u all_elements = vector_elements_mask(BYTE_ELEMENTS(len));
  mask &= all_elements;

  if (mask == all_elements) { // all elements selected, the load is not masked
    read_virtual_vector(i->seg(), eaddr, op, len);
    return;
  }

  if (mask == 0) { // no elements selected, nothing to load, all elements are zeroed
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
      op->vmm64u(n) = 0;
    return;
  }

  if (i->as64L()) {
    // check only the first and the last selected elements, all the elements in between
    // are in the same canonical (and LASS) address range as one of them
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    if (! IsCanonicalAccess(laddr + tzcntq(mask), BX_READ, USER_PL) ||
        ! IsCanonicalAccess(laddr + most_significant_bitq(mask), BX_READ, USER_PL))
    {
      exception(int_number(i->seg()), 0);
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

  Bit32u all_elements = (Bit32u) vector_elements_mask(WORD_ELEMENTS(len));
  mask &= all_elements;

  if (mask == all_elements) { // all elements selected, the load is not masked
    read_virtual_vector(i->seg(), eaddr, op, len);
    return;
  }

  if (mask == 0) { // no elements selected, nothing to load, all elements are zeroed
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
      op->vmm64u(n) = 0;
    return;
  }

  if (i->as64L()) {
    // check only the first and the last selected elements, all the elements in between
    // are in the same canonical (and LASS) address range as one of them
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    if (! IsCanonicalAccess(laddr + 2*tzcntd(mask), BX_READ, USER_PL) ||
        ! IsCanonicalAccess(laddr + 2*most_significant_bitd(mask), BX_READ, USER_PL))
    {
      exception(int_number(i->seg()), 0);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  ignore_alignment_check(); // masked load/store never generate #AC
#endif

  for (int n=WORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       op->vmm16u(n) = read_virtual_word(i->seg(), eaddr + 2*n);
    else
       op->vmm16u(n) = 0;
  }
}

void BX_CPU_C::avx_masked_load32(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

  Bit32u all_elements = (Bit32u) vector_elements_mask(DWORD_ELEMENTS(len));
  mask &= all_elements;

  if (mask == all_elements) { // all elements selected, the load is not masked
    read_virtual_vector(i->seg(), eaddr, op, len);
    return;
  }

  if (mask == 0) { // no elements selected, nothing to load, all elements are zeroed
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
      op->vmm64u(n) = 0;
    return;
  }

  if (i->as64L()) {
    // check only the first and the last selected elements, all the elements in between
    // are in the same canonical (and LASS) address range as one of them
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    if (! IsCanonicalAccess(laddr + 4*tzcntd(mask), BX_READ, USER_PL) ||
        ! IsCanonicalAccess(laddr + 4*most_significant_bitd(mask), BX_READ, USER_PL))
    {
      exception(int_number(i->seg()), 0);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  ignore_alignment_check(); // masked load/store never generate #AC
#endif

  for (int n=DWORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       op->vmm32u(n) = read_virtual_dword(i->seg(), eaddr + 4*n);
    else
       op->vmm32u(n) = 0;
  }
}

void BX_CPU_C::avx_masked_load64(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

  Bit32u all_elements = (Bit32u) vector_elements_mask(QWORD_ELEMENTS(len));
  mask &= all_elements;

  if (mask == all_elements) { // all elements selected, the load is not masked
    read_virtual_vector(i->seg(), eaddr, op, len);
    return;
  }

  if (mask == 0) { // no elements selected, nothing to load, all elements are zeroed
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
      op->vmm64u(n) = 0;
    return;
  }

  if (i->as64L()) {
    // check only the first and the last selected elements, all the elements in between
    // are in the same canonical (and LASS) address range as one of them
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    if (! IsCanonicalAccess(laddr + 8*tzcntd(mask), BX_READ, USER_PL) ||
        ! IsCanonicalAccess(laddr + 8*most_significant_bitd(mask), BX_READ, USER_PL))
    {
      exception(int_number(i->seg()), 0);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  ignore_alignment_check(); // masked load/store never generate #AC
#endif

  for (int n=QWORD_ELEMENTS(len)-1; n >= 0; n--) {
    if (mask & (1<<n))
       op->vmm64u(n) = read_virtual_qword(i->seg(), eaddr + 8*n);
    else
       op->vmm64u(n) = 0;
  }
}

void BX_CPU_C::avx_masked_store8(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit64u mask)
{
  unsigned len = i->getVL();

  Bit64u all_elements = vector_elements_mask(BYTE_ELEMENTS(len));
  mask &= all_elements;

  if (mask == 0) return; // no elements selected, nothing to store

  if (mask == all_elements) { // all elements selected, the store is not masked
    write_virtual_vector(i->seg(), eaddr, op, len);
    return;
  }

  // the first and the last selected elements
  unsigned first = tzcntq(mask), last = most_significant_bitq(mask);

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    // check only the first and the last selected elements, all the elements in between
    // are in the same canonical (and LASS) address range as one of them
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    if (! IsCanonicalAccess(laddr + first, BX_WRITE, USER_PL) ||
        ! IsCanonicalAccess(laddr + last, BX_WRITE, USER_PL))
    {
      exception(int_number(i->seg()), 0);
    }
  }
#endif

  // see if you can successfully write all the elements first, the selected elements span at most
  // two pages: check the last byte of the last element and the first byte of the first element
  tickle_write_virtual(i->seg(), eaddr + last, 1);
  tickle_write_virtual(i->seg(), eaddr + first, 1);

  for (unsigned n=first; n <= last; n++) {
    if (mask & (BX_CONST64(1)<<n))
       write_virtual_byte(i->seg(), eaddr + n, op->vmmubyte(n));
  }
}

void BX_CPU_C::avx_masked_store16(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

  Bit32u all_elements = (Bit32u) vector_elements_mask(WORD_ELEMENTS(len));
  mask &= all_elements;

  if (mask == 0) return; // no elements selected, nothing to store

  if (mask == all_elements) { // all elements selected, the store is not masked
    write_virtual_vector(i->seg(), eaddr, op, len);
    return;
  }

  // the first and the last selected elements
  unsigned first = tzcntd(mask), last = most_significant_bitd(mask);

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    // check only the first and the last selected elements, all the elements in between
    // are in the same canonical (and LASS) address range as one of them
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    if (! IsCanonicalAccess(laddr + 2*first, BX_WRITE, USER_PL) ||
        ! IsCanonicalAccess(laddr + 2*last, BX_WRITE, USER_PL))
    {
      exception(int_number(i->seg()), 0);
    }
  }
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  ignore_alignment_check(); // masked load/store never generate #AC
#endif

  // see if you can successfully write all the elements first, the selected elements span at most
  // two pages: check the last byte of the last element and the first byte of the first element
  tickle_write_virtual(i->seg(), eaddr + 2*last + 1, 1);
  tickle_write_virtual(i->seg(), eaddr + 2*first, 1);

  for (unsigned n=first; n <= last; n++) {
    if (mask & (1<<n))
       write_virtual_word(i->seg(), eaddr + 2*n, op->vmm16u(n));
  }
}

void BX_CPU_C::avx_masked_store32(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

  Bit32u all_elements = (Bit32u) vector_elements_mask(DWORD_ELEMENTS(len));
  mask &= all_elements;

  if (mask == 0) return; // no elements selected, nothing to store

  if (mask == all_elements) { // all elements selected, the store is not masked
    write_virtual_vector(i->seg(), eaddr, op, len);
    return;
  }

  // the first and the last selected elements
  unsigned first = tzcntd(mask), last = most_significant_bitd(mask);

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    // check only the first and the last selected elements, all the elements in between
    // are in the same canonical (and LASS) address range as one of them
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    if (! IsCanonicalAccess(laddr + 4*first, BX_WRITE, USER_PL) ||
        ! IsCanonicalAccess(laddr + 4*last, BX_WRITE, USER_PL))
    {
      exception(int_number(i->seg()), 0);
    }
  }
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  ignore_alignment_check(); // masked load/store never generate #AC
#endif

  // see if you can successfully write all the elements first, the selected elements span at most
  // two pages: check the last byte of the last element and the first byte of the first element
  tickle_write_virtual(i->seg(), eaddr + 4*last + 3, 1);
  tickle_write_virtual(i->seg(), eaddr + 4*first, 1);

  for (unsigned n=first; n <= last; n++) {
    if (mask & (1<<n))
       write_virtual_dword(i->seg(), eaddr + 4*n, op->vmm32u(n));
  }
}

void BX_CPU_C::avx_masked_store64(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, Bit32u mask)
{
  unsigned len = i->getVL();

  Bit32u all_elements = (Bit32u) vector_elements_mask(QWORD_ELEMENTS(len));
  mask &= all_elements;

  if (mask == 0) return; // no elements selected, nothing to store

  if (mask == all_elements) { // all elements selected, the store is not masked
    write_virtual_vector(i->seg(), eaddr, op, len);
    return;
  }

  // the first and the last selected elements
  unsigned first = tzcntd(mask), last = most_significant_bitd(mask);

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    // check only the first and the last selected elements, all the elements in between
    // are in the same canonical (and LASS) address range as one of them
    Bit64u laddr = get_laddr64(i->seg(), eaddr);
    if (! IsCanonicalAccess(laddr + 8*first, BX_WRITE, USER_PL) ||
        ! IsCanonicalAccess(laddr + 8*last, BX_WRITE, USER_PL))
    {
      exception(int_number(i->seg()), 0);
    }
  }
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  ignore_alignment_check(); // masked load/store never generate #AC
#endif

  // see if you can successfully write all the elements first, the selected elements span at most
  // two pages: check the last byte of the last element and the first byte of the first element
  tickle_write_virtual(i->seg(), eaddr + 8*last + 7, 1);
  tickle_write_virtual(i->seg(), eaddr + 8*first, 1);

  for (unsigned n=first; n <= last; n++) {
    if (mask & (1<<n))
       write_virtual_qword(i->seg(), eaddr + 8*n, op->vmm64u(n));
  }
}

#endif // BX_SUPPORT_AVX

#if BX_SUPPORT_EVEX

#include "simd_int.h"

void BX_CPU_C::avx512_write_regb_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned len, Bit64u opmask)
{
  if (i->isZeroMasking())
    simd_zero_pblendb(&BX_READ_AVX_REG(i->dst()), op, opmask, BYTE_ELEMENTS(len));
  else
    simd_pblendb(&BX_READ_AVX_REG(i->dst()), op, opmask, BYTE_ELEMENTS(len));

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

void BX_CPU_C::avx512_write_regw_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned len, Bit32u opmask)
{
  if (i->isZeroMasking())
    simd_zero_pblendw(&BX_READ_AVX_REG(i->dst()), op, opmask, WORD_ELEMENTS(len));
  else
    simd_pblendw(&BX_READ_AVX_REG(i->dst()), op, opmask, WORD_ELEMENTS(len));

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

void BX_CPU_C::avx512_write_regd_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned len, Bit32u opmask)
{
  if (i->isZeroMasking())
    simd_zero_blendps(&BX_READ_AVX_REG(i->dst()), op, opmask, DWORD_ELEMENTS(len));
  else
    simd_blendps(&BX_READ_AVX_REG(i->dst()), op, opmask, DWORD_ELEMENTS(len));

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

void BX_CPU_C::avx512_write_regq_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned len, Bit32u opmask)
{
  if (i->isZeroMasking())
    simd_zero_blendpd(&BX_READ_AVX_REG(i->dst()), op, opmask, QWORD_ELEMENTS(len));
  else
    simd_blendpd(&BX_READ_AVX_REG(i->dst()), op, opmask, QWORD_ELEMENTS(len));

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

#endif
