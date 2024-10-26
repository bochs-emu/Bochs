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
  bool quiet  = (i->getIaOpcode() == BX_IA_V512_VUCOMXSS_VssWss);
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
  bool quiet  = (i->getIaOpcode() == BX_IA_V512_VUCOMXSD_VsdWsd);
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
  bool quiet  = (i->getIaOpcode() == BX_IA_V512_VUCOMXSH_VshWsh);
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

#endif
