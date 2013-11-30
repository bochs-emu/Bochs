/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013 Stanislav Shwartsman
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
void BX_CPU_C::avx_masked_load32(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, unsigned mask)
{
  unsigned len = i->getVL();

  if (i->as64L()) {
    for (unsigned n=0; n < (4*len); n++) {
       if (mask & (1<<n)) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 4*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (int n=4*len-1; n >= 0; n--) {
    if (mask & (1<<n))
       op->vmm32u(n) = read_virtual_dword(i->seg(), eaddr + 4*n);
    else
       op->vmm32u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

void BX_CPU_C::avx_masked_load64(bxInstruction_c *i, bx_address eaddr, BxPackedAvxRegister *op, unsigned mask)
{
  unsigned len = i->getVL();

  if (i->as64L()) {
    for (unsigned n=0; n < (2*len); n++) {
       if (mask & (1<<n)) {
          if (! IsCanonical(get_laddr64(i->seg(), eaddr + 8*n)))
             exception(int_number(i->seg()), 0);
       }
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (int n=2*len-1; n >= 0; n--) {
    if (mask & (1<<n))
       op->vmm64u(n) = read_virtual_qword(i->seg(), eaddr + 8*n);
    else
       op->vmm64u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

void BX_CPU_C::avx_masked_store32(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, unsigned mask)
{
  unsigned len = i->getVL();

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (4*len); n++) {
      if (mask & (1<<n)) {
        if (! IsCanonical(get_laddr64(i->seg(), eaddr + 4*n)))
           exception(int_number(i->seg()), 0);
      }
    }
  }
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  // see you can successfully write all the elements first
  for (int n=4*len-1; n >= 0; n--) {
    if (mask & (1<<n))
       read_RMW_virtual_dword(i->seg(), eaddr + 4*n);
  }

  for (unsigned n=0; n < (4*len); n++) {
    if (mask & (1<<n))
       write_virtual_dword(i->seg(), eaddr + 4*n, op->vmm32u(n));
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif
}

void BX_CPU_C::avx_masked_store64(bxInstruction_c *i, bx_address eaddr, const BxPackedAvxRegister *op, unsigned mask)
{
  unsigned len = i->getVL();

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    for (unsigned n=0; n < (2*len); n++) {
      if (mask & (1<<n)) {
        if (! IsCanonical(get_laddr64(i->seg(), eaddr + 8*n)))
           exception(int_number(i->seg()), 0);
      }
    }
  }
#endif

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  // see you can successfully write all the elements first
  for (int n=2*len-1; n >= 0; n--) {
    if (mask & (1<<n))
       read_RMW_virtual_qword(i->seg(), eaddr + 8*n);
  }

  for (unsigned n=0; n < (2*len); n++) {
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

void BX_CPU_C::avx512_write_regd_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned mask)
{
  unsigned len = i->getVL();

  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_zero_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), mask);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), mask);
  }
}

void BX_CPU_C::avx512_write_regq_masked(bxInstruction_c *i, const BxPackedAvxRegister *op, unsigned mask)
{
  unsigned len = i->getVL();

  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_zero_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), mask);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op->vmm128(n), mask);
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
}

//////////////////////////
// masked register move //
//////////////////////////

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  avx512_write_regd_masked(i, &op, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPD_MASK_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  avx512_write_regq_masked(i, &op, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

/////////////////////////////////
// masked load/store - aligned //
/////////////////////////////////

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_MASK_VpsWpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  bx_address laddr = get_laddr(i->seg(), eaddr);

  unsigned len = i->getVL(), len_in_bytes = 16 * len;
  if (laddr & (len_in_bytes-1)) {
    BX_ERROR(("AVX masked read len=%d: #GP misaligned access", len_in_bytes));
    exception(BX_GP_EXCEPTION, 0);
  }

  BxPackedAvxRegister reg;
  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());
  avx_masked_load32(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPD_MASK_VpdWpdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  bx_address laddr = get_laddr(i->seg(), eaddr);

  unsigned len = i->getVL(), len_in_bytes = 16 * len;
  if (laddr & (len_in_bytes-1)) {
    BX_ERROR(("AVX masked read len=%d: #GP misaligned access", len_in_bytes));
    exception(BX_GP_EXCEPTION, 0);
  }

  BxPackedAvxRegister reg;
  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());
  avx_masked_load64(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPS_MASK_WpsVpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  bx_address laddr = get_laddr(i->seg(), eaddr);

  unsigned len_in_bytes = 16 * i->getVL();
  if (laddr & (len_in_bytes-1)) {
    BX_ERROR(("AVX masked write len=%d: #GP misaligned access", len_in_bytes));
    exception(BX_GP_EXCEPTION, 0);
  }

  avx_masked_store32(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_16BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVAPD_MASK_WpdVpdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  bx_address laddr = get_laddr(i->seg(), eaddr);

  unsigned len_in_bytes = 16 * i->getVL();
  if (laddr & (len_in_bytes-1)) {
    BX_ERROR(("AVX masked write len=%d: #GP misaligned access", len_in_bytes));
    exception(BX_GP_EXCEPTION, 0);
  }

  avx_masked_store64(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_8BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

///////////////////////
// masked load/store //
///////////////////////

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPS_MASK_VpsWpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  unsigned len = i->getVL();

  BxPackedAvxRegister reg;
  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());
  avx_masked_load32(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPD_MASK_VpdWpdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  unsigned len = i->getVL();

  BxPackedAvxRegister reg;
  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());
  avx_masked_load64(i, eaddr, &reg, mask);

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), reg, len);
  }
  else {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &reg.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPS_MASK_WpsVpsM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  avx_masked_store32(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVUPD_MASK_WpdVpdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  avx_masked_store64(i, eaddr, &BX_READ_AVX_REG(i->src()), BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

#endif
