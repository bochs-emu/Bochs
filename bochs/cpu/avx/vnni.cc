/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2017-2020 Stanislav Shwartsman
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

#include "simd_int.h"

#define AVX_3OP_VNNI(HANDLER, func)                                                                                          \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER(bxInstruction_c *i)                                                        \
  {                                                                                                                          \
    BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                                                               \
                                                                                                                             \
    for (unsigned n=0; n < len; n++)                                                                                         \
      (func) (&dst.vmm128(n), &op1.vmm128(n), &op2.vmm128(n));                                                               \
                                                                                                                             \
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);                                                                                   \
                                                                                                                             \
    BX_NEXT_INSTR(i);                                                                                                        \
  }

AVX_3OP_VNNI(VPDPBUSD_VdqHdqWdqR, xmm_pdpbusd)
AVX_3OP_VNNI(VPDPBUSDS_VdqHdqWdqR, xmm_pdpbusds)
AVX_3OP_VNNI(VPDPWSSD_VdqHdqWdqR, xmm_pdpwssd)
AVX_3OP_VNNI(VPDPWSSDS_VdqHdqWdqR, xmm_pdpwssds)

AVX_3OP_VNNI(VPDPBSSD_VdqHdqWdqR, xmm_pdpbssd)
AVX_3OP_VNNI(VPDPBSSDS_VdqHdqWdqR, xmm_pdpbssds)
AVX_3OP_VNNI(VPDPBSUD_VdqHdqWdqR, xmm_pdpbsud)
AVX_3OP_VNNI(VPDPBSUDS_VdqHdqWdqR, xmm_pdpbsuds)
AVX_3OP_VNNI(VPDPBUUD_VdqHdqWdqR, xmm_pdpbuud)
AVX_3OP_VNNI(VPDPBUUDS_VdqHdqWdqR, xmm_pdpbuuds)

#endif

#if BX_SUPPORT_EVEX

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

AVX512_3OP_DWORD_EL(VPDPBUSD_MASK_VdqHdqWdqR, xmm_pdpbusd)
AVX512_3OP_DWORD_EL(VPDPBUSDS_MASK_VdqHdqWdqR, xmm_pdpbusds)
AVX512_3OP_DWORD_EL(VPDPWSSD_MASK_VdqHdqWdqR, xmm_pdpwssd)
AVX512_3OP_DWORD_EL(VPDPWSSDS_MASK_VdqHdqWdqR, xmm_pdpwssds)

#endif
