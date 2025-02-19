/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024 Stanislav Shwartsman
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVW_VshWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op;
  op.xmm64u(0) = (Bit64u) BX_READ_XMM_REG_LO_WORD(i->src());
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVD_VdWdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op;
  op.xmm64u(0) = (Bit64u) BX_READ_XMM_REG_LO_DWORD(i->src());
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op);
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_EVEX

#include "softfloat3e/include/softfloat.h"
#include "decoder/ia_opcodes.h"

extern softfloat_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);
extern void softfloat_status_word_rc_override(softfloat_status_t &status, bxInstruction_c *i);

void BX_CPU_C::write_eflags_vcomx(int float_relation)
{
  switch(float_relation) {
   case softfloat_relation_unordered:
      setEFlagsOSZAPC(EFlagsOFMask | EFlagsSFMask | EFlagsPFMask | EFlagsCFMask);
      break;

   case softfloat_relation_greater:
      clearEFlagsOSZAPC();
      break;

   case softfloat_relation_less:
      setEFlagsOSZAPC(EFlagsOFMask | EFlagsCFMask);
      break;

   case softfloat_relation_equal:
      setEFlagsOSZAPC(EFlagsOFMask | EFlagsSFMask | EFlagsZFMask);
      break;

   default:
      BX_PANIC(("write_eflags_vcomx: unknown floating point compare relation"));
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCOMXSS_VssWssR(bxInstruction_c *i)
{
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_DWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  bool quiet  = (i->getIaOpcode() == BX_IA_EVEX_VUCOMXSS_VssWss);
  int rc = f32_compare(op1, op2, quiet, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));
  write_eflags_vcomx(rc);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCOMXSD_VsdWsdR(bxInstruction_c *i)
{
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_QWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  bool quiet  = (i->getIaOpcode() == BX_IA_EVEX_VUCOMXSD_VsdWsd);
  int rc = f64_compare(op1, op2, quiet, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));
  write_eflags_vcomx(rc);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCOMXSH_VshWshR(bxInstruction_c *i)
{
  float16 op1 = BX_READ_XMM_REG_LO_WORD(i->dst()), op2 = BX_READ_XMM_REG_LO_WORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  bool quiet  = (i->getIaOpcode() == BX_IA_EVEX_VUCOMXSH_VshWsh);
  int rc = f16_compare(op1, op2, quiet, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));
  write_eflags_vcomx(rc);

  BX_NEXT_INSTR(i);
}

#include "simd_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMPSADBW_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  result.clear();

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  // For the 512-bit version the control bits for the lower two lanes are replicated to the upper two lanes
  int control[4] = { i->Ib(), i->Ib() >> 3, i->Ib(), i->Ib() >> 3 };

  for (unsigned n=0; n < len; n++) {
    xmm_mpsadbw(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n), control[n]);
  }

  avx512_write_regw_masked(i, &result, len, opmask);

  BX_NEXT_INSTR(i);
}

extern float32 convert_ne_fp16_to_fp32(float16 op);
extern softfloat_status_t prepare_ne_softfloat_status_helper(bool denormals_are_zeros);

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDPPHPS_MASK_VpsHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();
  Bit32u mask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff;

  // "round to nearest even" rounding mode is used when doing each accumulation of the FMA.
  // output denormals are always flushed to zero and input denormals are always treated as zero.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  for (unsigned n=0, tmp_mask = mask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1) {
      dst.vmm32u(n) = f32_mulAdd(convert_ne_fp16_to_fp32(op1.vmm32u(n) >> 16), convert_ne_fp16_to_fp32(op2.vmm32u(n) >> 16), dst.vmm32u(n), 0, &status);
      dst.vmm32u(n) = f32_mulAdd(convert_ne_fp16_to_fp32(op1.vmm32u(n)),       convert_ne_fp16_to_fp32(op2.vmm32u(n)),       dst.vmm32u(n), 0, &status);
    }
    else if (i->isZeroMasking()) {
      dst.vmm32u(n) = 0;
    }
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

#endif

#endif
