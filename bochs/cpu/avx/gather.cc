/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2014 Stanislav Shwartsman
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

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::BxResolveGatherD(bxInstruction_c *i, unsigned element)
{
  Bit32s index = BX_READ_AVX_REG(i->sibIndex()).vmm32s(element);
  
  if (i->as64L())
    return (BX_READ_64BIT_REG(i->sibBase()) + (((Bit64s) index) << i->sibScale()) + i->displ32s());
  else
    return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
}

bx_address BX_CPP_AttrRegparmN(2) BX_CPU_C::BxResolveGatherQ(bxInstruction_c *i, unsigned element)
{
  Bit64s index = BX_READ_AVX_REG(i->sibIndex()).vmm64s(element);
  
  if (i->as64L())
    return (BX_READ_64BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
  else
    return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + (index << i->sibScale()) + i->displ32s());
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERDPS_VpsHps(bxInstruction_c *i)
{
  if (i->sibIndex() == i->src2() || i->sibIndex() == i->dst() || i->src2() == i->dst()) {
    BX_ERROR(("%s: incorrect source operands", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedYmmRegister *mask = &BX_YMM_REG(i->src2()), *dest = &BX_YMM_REG(i->dst());

  // index size = 32, element_size = 32, max vector size = 256
  // num_elements:
  //     128 bit => 4
  //     256 bit => 8

  unsigned n, num_elements = DWORD_ELEMENTS(i->getVL());

  for (n=0; n < num_elements; n++) {
    if (mask->ymm32s(n) < 0)
      mask->ymm32u(n) = 0xffffffff;
    else
      mask->ymm32u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0; n < 8; n++)
  {
    if (n >= num_elements) {
        mask->ymm32u(n) = 0;
        dest->ymm32u(n) = 0;
        continue;
    }

    if (mask->ymm32u(n)) {
        dest->ymm32u(n) = read_virtual_dword(i->seg(), BxResolveGatherD(i, n));
    }
    mask->ymm32u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_CLEAR_AVX_HIGH256(i->dst());
  BX_CLEAR_AVX_HIGH256(i->src2());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERQPS_VpsHps(bxInstruction_c *i)
{
  if (i->sibIndex() == i->src2() || i->sibIndex() == i->dst() || i->src2() == i->dst()) {
    BX_ERROR(("%s: incorrect source operands", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 64, element_size = 32, max vector size = 256
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4

  BxPackedYmmRegister *mask = &BX_YMM_REG(i->src2()), *dest = &BX_YMM_REG(i->dst());
  unsigned n, num_elements = QWORD_ELEMENTS(i->getVL());

  for (n=0; n < num_elements; n++) {
    if (mask->ymm32s(n) < 0)
      mask->ymm32u(n) = 0xffffffff;
    else
      mask->ymm32u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0; n < 4; n++)
  {
    if (n >= num_elements) {
        mask->ymm32u(n) = 0;
        dest->ymm32u(n) = 0;
        continue;
    }

    if (mask->ymm32u(n)) {
        dest->ymm32u(n) = read_virtual_dword(i->seg(), BxResolveGatherQ(i, n));
    }
    mask->ymm32u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_CLEAR_AVX_HIGH128(i->dst());
  BX_CLEAR_AVX_HIGH128(i->src2());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERDPD_VpdHpd(bxInstruction_c *i)
{
  if (i->sibIndex() == i->src2() || i->sibIndex() == i->dst() || i->src2() == i->dst()) {
    BX_ERROR(("%s: incorrect source operands", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 32, element_size = 64, max vector size = 256
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4

  BxPackedYmmRegister *mask = &BX_YMM_REG(i->src2()), *dest = &BX_YMM_REG(i->dst());
  unsigned n, num_elements = QWORD_ELEMENTS(i->getVL());

  for (n=0; n < num_elements; n++) {
    if (mask->ymm64s(n) < 0)
      mask->ymm64u(n) = BX_CONST64(0xffffffffffffffff);
    else
      mask->ymm64u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (unsigned n=0; n < 4; n++)
  {
    if (n >= num_elements) {
        mask->ymm64u(n) = 0;
        dest->ymm64u(n) = 0;
        continue;
    }

    if (mask->ymm64u(n)) {
        dest->ymm64u(n) = read_virtual_qword(i->seg(), BxResolveGatherD(i, n));
    }
    mask->ymm64u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_CLEAR_AVX_HIGH256(i->dst());
  BX_CLEAR_AVX_HIGH256(i->src2());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERQPD_VpdHpd(bxInstruction_c *i)
{
  if (i->sibIndex() == i->src2() || i->sibIndex() == i->dst() || i->src2() == i->dst()) {
    BX_ERROR(("VGATHERQPD_VpdHpd: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 64, element_size = 64, max vector size = 256
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4

  BxPackedYmmRegister *mask = &BX_YMM_REG(i->src2()), *dest = &BX_YMM_REG(i->dst());
  unsigned n, num_elements = QWORD_ELEMENTS(i->getVL());

  for (n=0; n < num_elements; n++) {
    if (mask->ymm64s(n) < 0)
      mask->ymm64u(n) = BX_CONST64(0xffffffffffffffff);
    else
      mask->ymm64u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0; n < 4; n++)
  {
    if (n >= num_elements) {
        mask->ymm64u(n) = 0;
        dest->ymm64u(n) = 0;
        continue;
    }

    if (mask->ymm64u(n)) {
        dest->ymm64u(n) = read_virtual_qword(i->seg(), BxResolveGatherQ(i, n));
    }
    mask->ymm64u(n) = 0;
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_CLEAR_AVX_HIGH256(i->dst());
  BX_CLEAR_AVX_HIGH256(i->src2());

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_EVEX

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERDPS_MASK_VpsVSib(bxInstruction_c *i)
{
  if (i->sibIndex() == i->dst()) {
    BX_ERROR(("%s: incorrect source operands", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister *dest = &BX_AVX_REG(i->dst());
  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask;

  // index size = 32, element_size = 32, max vector size = 512
  // num_elements:
  //     128 bit => 4
  //     256 bit => 8
  //     512 bit => 16

  unsigned n, len = i->getVL(), num_elements = DWORD_ELEMENTS(len);

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0, mask = 0x1; n < num_elements; n++, mask <<= 1)
  {
    if (opmask & mask) {
      dest->vmm32u(n) = read_virtual_dword(i->seg(), BxResolveGatherD(i, n));
      opmask &= ~mask;
      BX_WRITE_OPMASK(i->opmask(), opmask);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_OPMASK(i->opmask(), 0);
  BX_CLEAR_AVX_REGZ(i->dst(), len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERQPS_MASK_VpsVSib(bxInstruction_c *i)
{
  if (i->sibIndex() == i->dst()) {
    BX_ERROR(("%s: incorrect source operands", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 64, element_size = 32, max vector size = 512
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4
  //     512 bit => 8

  BxPackedAvxRegister *dest = &BX_AVX_REG(i->dst());
  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask;

  unsigned n, len = i->getVL(), num_elements = QWORD_ELEMENTS(len);

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0, mask = 0x1; n < num_elements; n++, mask <<= 1)
  {
    if (opmask & mask) {
      dest->vmm32u(n) = read_virtual_dword(i->seg(), BxResolveGatherQ(i, n));
      opmask &= ~mask;
      BX_WRITE_OPMASK(i->opmask(), opmask);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  // ensure correct upper part clearing of the destination register
  if (len == BX_VL128) dest->vmm64u(1) = 0;
  else len--;

  BX_WRITE_OPMASK(i->opmask(), 0);
  BX_CLEAR_AVX_REGZ(i->dst(), len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERDPD_MASK_VpdVSib(bxInstruction_c *i)
{
  if (i->sibIndex() == i->dst()) {
    BX_ERROR(("%s: incorrect source operands", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 32, element_size = 64, max vector size = 512
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4
  //     512 bit => 8

  BxPackedAvxRegister *dest = &BX_AVX_REG(i->dst());
  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask;

  unsigned n, len = i->getVL(), num_elements = QWORD_ELEMENTS(len);

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0, mask = 0x1; n < num_elements; n++, mask <<= 1)
  {
    if (opmask & mask) {
      dest->vmm64u(n) = read_virtual_qword(i->seg(), BxResolveGatherD(i, n));
      opmask &= ~mask;
      BX_WRITE_OPMASK(i->opmask(), opmask);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_OPMASK(i->opmask(), 0);
  BX_CLEAR_AVX_REGZ(i->dst(), len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGATHERQPD_MASK_VpdVSib(bxInstruction_c *i)
{
  if (i->sibIndex() == i->dst()) {
    BX_ERROR(("VGATHERQPD_VpdHpd: incorrect source operands"));
    exception(BX_UD_EXCEPTION, 0);
  }

  // index size = 64, element_size = 64, max vector size = 512
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4
  //     512 bit => 8

  BxPackedAvxRegister *dest = &BX_AVX_REG(i->dst());
  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask;

  unsigned n, len = i->getVL(), num_elements = QWORD_ELEMENTS(len);

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0, mask = 0x1; n < num_elements; n++, mask <<= 1)
  {
    if (opmask & mask) {
      dest->vmm64u(n) = read_virtual_qword(i->seg(), BxResolveGatherQ(i, n));
      opmask &= ~mask;
      BX_WRITE_OPMASK(i->opmask(), opmask);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_OPMASK(i->opmask(), 0);
  BX_CLEAR_AVX_REGZ(i->dst(), len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSCATTERDPS_MASK_VSibVps(bxInstruction_c *i)
{
  BxPackedAvxRegister *src = &BX_AVX_REG(i->src());
  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask;

  // index size = 32, element_size = 32, max vector size = 512
  // num_elements:
  //     128 bit => 4
  //     256 bit => 8
  //     512 bit => 16

  unsigned n, num_elements = DWORD_ELEMENTS(i->getVL());

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0, mask = 0x1; n < num_elements; n++, mask <<= 1)
  {
    if (opmask & mask) {
      write_virtual_dword(i->seg(), BxResolveGatherD(i, n), src->vmm32u(n));
      opmask &= ~mask;
      BX_WRITE_OPMASK(i->opmask(), opmask);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_OPMASK(i->opmask(), 0);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSCATTERQPS_MASK_VSibVps(bxInstruction_c *i)
{
  BxPackedAvxRegister *src = &BX_AVX_REG(i->src());
  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask;

  // index size = 64, element_size = 32, max vector size = 512
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4
  //     512 bit => 8

  unsigned n, num_elements = QWORD_ELEMENTS(i->getVL());

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0, mask = 0x1; n < num_elements; n++, mask <<= 1)
  {
    if (opmask & mask) {
      write_virtual_dword(i->seg(), BxResolveGatherQ(i, n), src->vmm32u(n));
      opmask &= ~mask;
      BX_WRITE_OPMASK(i->opmask(), opmask);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_OPMASK(i->opmask(), 0);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSCATTERDPD_MASK_VSibVpd(bxInstruction_c *i)
{
  BxPackedAvxRegister *src = &BX_AVX_REG(i->src());
  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask;

  // index size = 32, element_size = 64, max vector size = 512
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4
  //     512 bit => 8

  unsigned n, num_elements = QWORD_ELEMENTS(i->getVL());

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0, mask = 0x1; n < num_elements; n++, mask <<= 1)
  {
    if (opmask & mask) {
      write_virtual_qword(i->seg(), BxResolveGatherD(i, n), src->vmm64u(n));
      opmask &= ~mask;
      BX_WRITE_OPMASK(i->opmask(), opmask);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_OPMASK(i->opmask(), 0);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSCATTERQPD_MASK_VSibVpd(bxInstruction_c *i)
{
  BxPackedAvxRegister *src = &BX_AVX_REG(i->src());
  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask;

  // index size = 64, element_size = 64, max vector size = 512
  // num_elements:
  //     128 bit => 2
  //     256 bit => 4
  //     512 bit => 8

  unsigned n, num_elements = QWORD_ELEMENTS(i->getVL());

#if BX_SUPPORT_ALIGNMENT_CHECK
  unsigned save_alignment_check_mask = BX_CPU_THIS_PTR alignment_check_mask;
  BX_CPU_THIS_PTR alignment_check_mask = 0;
#endif

  for (n=0, mask = 0x1; n < num_elements; n++, mask <<= 1)
  {
    if (opmask & mask) {
      write_virtual_qword(i->seg(), BxResolveGatherQ(i, n), src->vmm64u(n));
      opmask &= ~mask;
      BX_WRITE_OPMASK(i->opmask(), opmask);
    }
  }

#if BX_SUPPORT_ALIGNMENT_CHECK
  BX_CPU_THIS_PTR alignment_check_mask = save_alignment_check_mask;
#endif

  BX_WRITE_OPMASK(i->opmask(), 0);
  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_EVEX

#endif
