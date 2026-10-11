/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2026 Stanislav Shwartsman
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

#include "fp8.h"
#include "cpu/decoder/ia_opcodes.h"

/* ==========================================================================
 * FP32 -> FP8 (RNE / RTO / BIAS). Output is always DWORD_ELEMENTS(len) bytes
 * (a quarter of the source VL), so - like VPMOVDB/VPMOVSDB - the register
 * destination is always at most one XMM regardless of source VL.
 * ==========================================================================
 */

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2BF8_Vf8WpsR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  // PS source -> 32-bit elements -> at most DWORD_ELEMENTS(BX_VL512) = 16 of
  // them, so the mask can never have a nonzero bit above bit 15.
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVTPS2BF8S_Vf8Wps_Kmask);

  for (unsigned n=0; n < num_elements; n++, opmask >>= 1) {
    if (opmask & 0x1)
      dst.xmmubyte(n) = convert_ne_fp32_to_bf8(src.vmm32u(n), saturate);
    else if (i->isZeroMasking())
      dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2HF8_Vf8WpsR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);

  Bit32u iaOpcode = i->getIaOpcode();
  bool saturate = (iaOpcode == BX_IA_EVEX_VCVTPS2HF8S_Vf8Wps_Kmask || iaOpcode == BX_IA_EVEX_VCVTROPS2HF8S_Vf8Wps_Kmask);
  unsigned rounding = (iaOpcode == BX_IA_EVEX_VCVTROPS2HF8_Vf8Wps_Kmask || iaOpcode == BX_IA_EVEX_VCVTROPS2HF8S_Vf8Wps_Kmask)
                        ? softfloat_round_odd : softfloat_round_near_even;

  for (unsigned n=0; n < num_elements; n++, opmask >>= 1) {
    if (opmask & 0x1)
      dst.xmmubyte(n) = convert_fp32_to_hf8(src.vmm32u(n), saturate, rounding);
    else if (i->isZeroMasking())
      dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBIASPS2BF8_Vf8HdqWpsR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister bias = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister src  = BX_READ_AVX_REG(i->src2());
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVTBIASPS2BF8S_Vf8HdqWps_Kmask);

  for (unsigned n=0; n < num_elements; n++, opmask >>= 1) {
    if (opmask & 0x1)
      dst.xmmubyte(n) = convert_truncate_fp32_to_bf8_bias(src.vmm32u(n), bias.vmm32u(n), saturate);
    else if (i->isZeroMasking())
      dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBIASPS2HF8_Vf8HdqWpsR(bxInstruction_c *i)
{
  BxPackedXmmRegister dst = BX_READ_XMM_REG(i->dst());
  BxPackedAvxRegister bias = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister src  = BX_READ_AVX_REG(i->src2());
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVTBIASPS2HF8S_Vf8HdqWps_Kmask);

  for (unsigned n=0; n < num_elements; n++, opmask >>= 1) {
    if (opmask & 0x1)
      dst.xmmubyte(n) = convert_truncate_fp32_to_hf8_bias(src.vmm32u(n), bias.vmm32u(n), saturate);
    else if (i->isZeroMasking())
      dst.xmmubyte(n) = 0;
  }

  if (len == BX_VL128) dst.xmm32u(1) = 0;
  if (len != BX_VL512) dst.xmm64u(1) = 0;

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

/* ==========================================================================
 * FP8 -> FP32 (exact widen). Source is a quarter-width vector (pre-loaded
 * via LOAD_Quarter_Vector when it's a memory operand); destination scales
 * fully with VL, so the merge-masking helper handles the write-back.
 * ==========================================================================
 */

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBF82PS_VpsWf8R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);

  for (unsigned n=0; n < num_elements; n++) {
    result.vmm32u(n) = convert_bf8_to_fp32(op.xmmubyte(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  avx512_write_regd_masked(i, &result, len, opmask);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTHF82PS_VpsWf8R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);

  for (unsigned n=0; n < num_elements; n++) {
    result.vmm32u(n) = convert_hf8_to_fp32(op.xmmubyte(n));
  }

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  avx512_write_regd_masked(i, &result, len, opmask);
  BX_NEXT_INSTR(i);
}

/* ==========================================================================
 * FP4 -> FP8 (exact widen). Source is a half-width vector (pre-loaded via
 * LOAD_Half_Vector when it's a memory operand); destination scales fully
 * with VL (byte per element), so the merge-masking helper handles the
 * write-back.
 * ==========================================================================
 */

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBF42HF8_Vf8Wf4R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_elements; n++) {
    int nibble = (op.ymmubyte(n >> 1) >> ((n & 1) << 2)) & 0xF;
    result.vmmubyte(n) = convert_bf4_to_hf8((float_bf4) nibble);
  }

  Bit64u opmask = i->opmask() ? BX_READ_OPMASK(i->opmask()) : ~Bit64u(0);
  avx512_write_regb_masked(i, &result, len, opmask);
  BX_NEXT_INSTR(i);
}

/* ==========================================================================
 * FP6 -> FP8 (exact widen). Register-only source, same register class as
 * the destination (6-bit elements are packed within a full-width register,
 * not stored at a fractional register width), so the source is loaded via
 * the plain full-vector LOAD_Vector path.
 * ==========================================================================
 */

// Extract the 6-bit tightly-packed FP6 element n (bits [6n+5 : 6n], per spec
// Section 9.7.5 "t = src[6*i+5 : 6*i]") from a source register. Elements
// straddle byte boundaries, so this cannot be read via vmmubyte(n).
static float_bf6 extract_packed_fp6(const BxPackedAvxRegister &op, unsigned n)
{
  unsigned bitoff = n * 6;
  unsigned qwordoff = bitoff >> 6;
  unsigned bitoff_lo = bitoff & 63;

  Bit64u lo = op.vmm64u(qwordoff);
  Bit64u hi = (qwordoff < 7) ? op.vmm64u(qwordoff + 1) : 0;
  Bit64u combined = lo >> bitoff_lo;
  if (bitoff_lo != 0) combined |= (hi << (64 - bitoff_lo));

  return (float_bf6) (combined & 0x3F);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBF62HF8_Vf8Wf6R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_elements; n++) {
    result.vmmubyte(n) = convert_bf6_to_hf8(extract_packed_fp6(op, n));
  }

  Bit64u opmask = i->opmask() ? BX_READ_OPMASK(i->opmask()) : ~Bit64u(0);
  avx512_write_regb_masked(i, &result, len, opmask);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTHF62HF8_Vf8Wf6R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_elements; n++) {
    result.vmmubyte(n) = convert_hf6_to_hf8(extract_packed_fp6(op, n));
  }

  Bit64u opmask = i->opmask() ? BX_READ_OPMASK(i->opmask()) : ~Bit64u(0);
  avx512_write_regb_masked(i, &result, len, opmask);
  BX_NEXT_INSTR(i);
}

/* ==========================================================================
 * FP8 -> FP6 (RNE, saturating). Register-only both operands; no masking or
 * zeroing support at all per spec Section 9.6 ("No masking or zeroing
 * support is provided").
 * ==========================================================================
 */

// Insert the 6-bit value into the tightly-packed destination at element n
// (bits [6n+5 : 6n], per spec Section 9.6.5 "dest[6*i+5:6*i] = ..."). Elements
// straddle byte boundaries, so this cannot be written via vmmubyte(n). Caller
// must zero-initialize dst first.
static void insert_packed_fp6(BxPackedAvxRegister *dst, unsigned n, float_bf6 val)
{
  unsigned bitoff = n * 6;
  unsigned qwordoff = bitoff >> 6;
  unsigned bitoff_lo = bitoff & 63;

  Bit64u v = (Bit64u) (val & 0x3F);

  dst->vmm64u(qwordoff) |= (v << bitoff_lo);
  if (bitoff_lo > 58 && qwordoff < 7) // spills into the next qword
    dst->vmm64u(qwordoff + 1) |= (v >> (64 - bitoff_lo));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBF82BF6S_Vf6Wf8R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  result.clear();
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_elements; n++) {
    insert_packed_fp6(&result, n, convert_bf8_to_bf6(op.vmmubyte(n)));
  }

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTHF82HF6S_Vf6Wf8R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  result.clear();
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_elements; n++) {
    insert_packed_fp6(&result, n, convert_hf8_to_hf6(op.vmmubyte(n)));
  }

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

/* ==========================================================================
 * FP8 -> FP4 (RNE, saturating). Destination may be memory (nibble-packed,
 * half the byte width of the source); source is always a register. No
 * masking or zeroing support at all per spec Section 9.4.
 * ==========================================================================
 */

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBF82BF4S_Wf4VdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_src_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_src_elements; n += 2) {
    float_bf4 lo = convert_bf8_to_bf4(src.vmmubyte(n));
    float_bf4 hi = convert_bf8_to_bf4(src.vmmubyte(n+1));
    result.vmmubyte(n >> 1) = (Bit8u) (lo | (hi << 4));
  }

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));
  } else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); /* write half vector */
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBF82BF4S_Wf4VdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_src_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_src_elements; n += 2) {
    float_bf4 lo = convert_bf8_to_bf4(src.vmmubyte(n));
    float_bf4 hi = convert_bf8_to_bf4(src.vmmubyte(n+1));
    result.vmmubyte(n >> 1) = (Bit8u) (lo | (hi << 4));
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  if (len == BX_VL128)
    write_virtual_qword(i->seg(), eaddr, result.vmm64u(0));
  else if (len == BX_VL256)
    write_virtual_xmmword(i->seg(), eaddr, &result.vmm128(0));
  else
    write_virtual_ymmword(i->seg(), eaddr, &result.vmm256(0));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTHF82BF4S_Wf4VdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_src_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_src_elements; n += 2) {
    float_bf4 lo = convert_hf8_to_bf4(src.vmmubyte(n));
    float_bf4 hi = convert_hf8_to_bf4(src.vmmubyte(n+1));
    result.vmmubyte(n >> 1) = (Bit8u) (lo | (hi << 4));
  }

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));
  } else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); /* write half vector */
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTHF82BF4S_Wf4VdqM(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedAvxRegister src = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_src_elements = BYTE_ELEMENTS(len);

  for (unsigned n=0; n < num_src_elements; n += 2) {
    float_bf4 lo = convert_hf8_to_bf4(src.vmmubyte(n));
    float_bf4 hi = convert_hf8_to_bf4(src.vmmubyte(n+1));
    result.vmmubyte(n >> 1) = (Bit8u) (lo | (hi << 4));
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  if (len == BX_VL128)
    write_virtual_qword(i->seg(), eaddr, result.vmm64u(0));
  else if (len == BX_VL256)
    write_virtual_xmmword(i->seg(), eaddr, &result.vmm128(0));
  else
    write_virtual_ymmword(i->seg(), eaddr, &result.vmm256(0));

  BX_NEXT_INSTR(i);
}

/* ==========================================================================
 * Sub-Byte Element Extraction (VUNPACKB). Destination always scales fully
 * with VL (one output byte per unpacked element); source is a same-width
 * register or memory operand (no broadcast).
 * ==========================================================================
 */

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VUNPACKB_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  unsigned num_elements = BYTE_ELEMENTS(len);

  Bit8u imm8 = i->Ib();
  bool sign_ex = (imm8 >> 5) & 0x1;
  int size = (imm8 >> 2) & 0x7;
  if (size < 2) size = 2; // encodings 0/1 are reserved but well-defined: clamp to 2

  int start;
  if (size == 2)                   start = imm8 & 0x3;
  else if (size == 3 || size == 4) start = imm8 & 0x1;
  else                             start = 0;

  unsigned elem_mask = (1u << size) - 1;
  unsigned sign_bit  = 1u << (size - 1);

  for (unsigned n=0; n < num_elements; n++) {
    unsigned bitoff     = (unsigned(start) * num_elements + n) * unsigned(size);
    unsigned qwordoff    = bitoff >> 6;
    unsigned bitoff_lo   = bitoff & 63;

    Bit64u lo = op.vmm64u(qwordoff);
    Bit64u hi = (qwordoff < 7) ? op.vmm64u(qwordoff + 1) : 0;
    Bit64u combined = lo >> bitoff_lo;
    if (bitoff_lo != 0) combined |= (hi << (64 - bitoff_lo));

    unsigned elem = unsigned(combined) & elem_mask;
    if (sign_ex && (elem & sign_bit))
      elem |= ~elem_mask & 0xFF;

    result.vmmubyte(n) = (Bit8u) elem;
  }

  Bit64u opmask = i->opmask() ? BX_READ_OPMASK(i->opmask()) : ~Bit64u(0);
  avx512_write_regb_masked(i, &result, len, opmask);
  BX_NEXT_INSTR(i);
}

#endif
