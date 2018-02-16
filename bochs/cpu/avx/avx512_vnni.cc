/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2017-2018 Stanislav Shwartsman
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

#include "simd_int.h"

#define AVX512_3OP_DWORD_EL(HANDLER, func)                                                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER(bxInstruction_c *i)                                                        \
  {                                                                                                                          \
    BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                                                               \
                                                                                                                             \
    for (unsigned n=0; n < len; n++)                                                                                         \
      (func) (&dst.vmm128(n), &op1.vmm128(n), &op2.vmm128(n));                                                               \
                                                                                                                             \
    if (i->opmask())                                                                                                         \
      avx512_write_regd_masked(i, &dst, len, BX_READ_16BIT_OPMASK(i->opmask()));                                             \
    else                                                                                                                     \
      BX_WRITE_AVX_REGZ(i->dst(), dst, len);                                                                                 \
                                                                                                                             \
    BX_NEXT_INSTR(i);                                                                                                        \
  }

AVX512_3OP_DWORD_EL(VPDPBUSD_VdqHdqWdqR, xmm_pdpbusd)
AVX512_3OP_DWORD_EL(VPDPBUSDS_VdqHdqWdqR, xmm_pdpbusds)
AVX512_3OP_DWORD_EL(VPDPWSSD_VdqHdqWdqR, xmm_pdpwssd)
AVX512_3OP_DWORD_EL(VPDPWSSDS_VdqHdqWdqR, xmm_pdpwssds)

#endif
