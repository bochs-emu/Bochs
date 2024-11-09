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

#if BX_SUPPORT_EVEX

#include "softfloat3e/include/softfloat.h"

extern softfloat_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);

#include "bf8.h"
#include "hf8.h"
#include "simd_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTHF82PH_VphWf8R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned opmask = BX_READ_32BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      result.vmm16u(n) = convert_hf8_to_fp16(op.ymmubyte(n), &status);
    else
      result.vmm16u(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (! i->isZeroMasking()) {
      simd_pblendw(&BX_READ_AVX_REG(i->dst()), &result, opmask, num_elements);
      BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}


#endif
