/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2018 Stanislav Shwartsman
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

#if BX_CPU_LEVEL >= 6

// Compare all pairs of Ai, Bj according to imm8 control
static void compare_strings(Bit8u BoolRes[16][16], const BxPackedXmmRegister &op1, const BxPackedXmmRegister &op2, Bit8u imm)
{
  unsigned i, j;
  unsigned aggregation_operation = (imm >> 2) & 3;

  // All possible comparisons are performed, the individual boolean
  // results of those comparisons are referred by
  //        BoolRes[op2 element index, op1 element index]

  switch (imm & 3) {
  case 0: /* unsigned bytes compare */
    for (i=0;i<16;i++) {
      for (j=0;j<16;j++) {
        switch (aggregation_operation) {
        case 0: /* 'equal' comparison */
        case 2:
        case 3:
          BoolRes[j][i] = (op1.xmmubyte(i) == op2.xmmubyte(j));
          break;
        case 1: /* 'ranges' comparison */
          if ((i % 2) == 0)
            BoolRes[j][i] = (op1.xmmubyte(i) <= op2.xmmubyte(j));
          else
            BoolRes[j][i] = (op1.xmmubyte(i) >= op2.xmmubyte(j));
          break;
        }
      }
    }
    break;

  case 1: /* unsigned words compare */
    for (i=0;i<8;i++) {
      for (j=0;j<8;j++) {
        switch (aggregation_operation) {
        case 0: /* 'equal' comparison */
        case 2:
        case 3:
          BoolRes[j][i] = (op1.xmm16u(i) == op2.xmm16u(j));
          break;
        case 1: /* 'ranges' comparison */
          if ((i % 2) == 0)
            BoolRes[j][i] = (op1.xmm16u(i) <= op2.xmm16u(j));
          else
            BoolRes[j][i] = (op1.xmm16u(i) >= op2.xmm16u(j));
          break;
        }
      }
    }
    break;

  case 2: /*   signed bytes compare */
    for (i=0;i<16;i++) {
      for (j=0;j<16;j++) {
        switch (aggregation_operation) {
        case 0: /* 'equal' comparison */
        case 2:
        case 3:
          BoolRes[j][i] = (op1.xmmsbyte(i) == op2.xmmsbyte(j));
          break;
        case 1: /* 'ranges' comparison */
          if ((i % 2) == 0)
            BoolRes[j][i] = (op1.xmmsbyte(i) <= op2.xmmsbyte(j));
          else
            BoolRes[j][i] = (op1.xmmsbyte(i) >= op2.xmmsbyte(j));
          break;
        }
      }
    }
    break;

  case 3: /*   signed words compare */
    for (i=0;i<8;i++) {
      for (j=0;j<8;j++) {
        switch (aggregation_operation) {
        case 0: /* 'equal' comparison */
        case 2:
        case 3:
          BoolRes[j][i] = (op1.xmm16s(i) == op2.xmm16s(j));
          break;
        case 1: /* 'ranges' comparison */
          if ((i % 2) == 0)
            BoolRes[j][i] = (op1.xmm16s(i) <= op2.xmm16s(j));
          else
            BoolRes[j][i] = (op1.xmm16s(i) >= op2.xmm16s(j));
          break;
        }
      }
    }
    break;
  }
}

static unsigned find_eos32(Bit32s reg32, Bit8u imm)
{
  if (imm & 0x1) {  // 8  elements
    if (reg32 > 8 || reg32 < -8) return 8;
    else return abs(reg32);
  }
  else {            // 16 elements
    if (reg32 > 16 || reg32 < -16) return 16;
    else return abs(reg32);
  }
}

#if BX_SUPPORT_X86_64
static unsigned find_eos64(Bit64s reg64, Bit8u imm)
{
  if (imm & 0x1) {  // 8  elements
    if (reg64 > 8 || reg64 < -8) return 8;
    else return (unsigned) abs(reg64);
  }
  else {            // 16 elements
    if (reg64 > 16 || reg64 < -16) return 16;
    else return (unsigned) abs(reg64);
  }
}
#endif

static unsigned find_eos(const BxPackedXmmRegister &op, Bit8u imm)
{
  unsigned i = 0;

  if (imm & 0x1) {  // 8  elements
    for(i=0;i<8;i++)
      if (op.xmm16u(i) == 0) break;
  }
  else {            // 16 elements
    for(i=0;i<16;i++)
      if (op.xmmubyte(i) == 0) break;
  }

  return i;
}

static bool override_if_data_invalid(bool val, bool i_valid, bool j_valid, Bit8u imm)
{
  unsigned aggregation_operation = (imm >> 2) & 3;

  switch(aggregation_operation) {
  case 0: // 'equal any'
  case 1: // 'ranges'
    if (! i_valid || ! j_valid) // one of the elements is invalid
      return 0;
    break;

  case 2: // 'equal each'
    if (! i_valid) {
      if (! j_valid) return 1; // both elements are invalid
      else return 0;           // only i is invalid
    }
    else {
      if (! j_valid) return 0; // only j is invalid
    }
    break;

  case 3: // 'equal ordered'
    if (! i_valid) {           // element i is invalid
      return 1;
    }
    else {
      if (! j_valid) {         // only j is invalid
        return 0;
      }
    }
    break;
  }

  return val;
}

static Bit16u aggregate(Bit8u BoolRes[16][16], unsigned len1, unsigned len2, Bit8u imm)
{
  unsigned aggregation_operation = (imm >> 2) & 3;
  unsigned num_elements = (imm & 0x1) ? 8 : 16;
  unsigned polarity = (imm >> 4) & 3;
  unsigned i,j,k;

  Bit16u result = 0;

  switch(aggregation_operation) {
  case 0: // 'equal any'
    for(j=0; j<num_elements; j++) {
      bool res = false;
      for(i=0; i<num_elements; i++) {
        if (override_if_data_invalid(BoolRes[j][i], (i < len1), (j < len2), imm)) {
          res = true;
          break;
        }
      }

      if (res) result |= (1<<j);
    }
    break;

  case 1: // 'ranges'
    for(j=0; j<num_elements; j++) {
      bool res = false;
      for(i=0; i<num_elements; i+=2) {
        if (override_if_data_invalid(BoolRes[j][i],     (i < len1), (j < len2), imm) &&
            override_if_data_invalid(BoolRes[j][i+1], (i+1 < len1), (j < len2), imm)) {
          res = true;
          break;
        }
      }

      if (res) result |= (1<<j);
    }
    break;

  case 2: // 'equal each'
    for(j=0; j<num_elements; j++) {
      if (override_if_data_invalid(BoolRes[j][j], (j < len1), (j < len2), imm))
        result |= (1<<j);
    }
    break;

  case 3: // 'equal ordered'
    for(j=0; j<num_elements; j++) {
      bool res = true;
      for (i=0, k=j; (i < num_elements-j) && (k < num_elements); i++, k++) {
        if (! override_if_data_invalid(BoolRes[k][i], (i < len1), (k < len2), imm)) {
          res = false;
          break;
        }
      }

      if (res) result |= (1<<j);
    }
    break;
  }

  switch(polarity) {
  case 0:
  case 2:
    break; // do nothing

  case 1:
    result ^= (num_elements == 8) ? 0xFF : 0xFFFF;
    break;

  case 3:
    for (j=0;j<num_elements;j++)
      if (j < len2) result ^= (1<<j);    // flip the bit
    break;
  }

  return result;
}

/* 66 0F 3A 60 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPESTRM_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;
  Bit8u imm8 = i->Ib();

  // compare all pairs of Ai, Bj
  Bit8u BoolRes[16][16];
  compare_strings(BoolRes, op1, op2, imm8);
  unsigned len1, len2, num_elements = (imm8 & 0x1) ? 8 : 16;

#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    len1 = find_eos64(RAX, imm8);
    len2 = find_eos64(RDX, imm8);
  }
  else
#endif
  {
    len1 = find_eos32(EAX, imm8);
    len2 = find_eos32(EDX, imm8);
  }
  Bit16u result2 = aggregate(BoolRes, len1, len2, imm8);

  // As defined by imm8[6], result2 is then either stored to the least
  // significant bits of XMM0 (zero extended to 128 bits) or expanded
  // into a byte/word-mask and then stored to XMM0
  if (imm8 & 0x40) {
     if (num_elements == 8) {
       for (int index = 0; index < 8; index++)
         result.xmm16u(index) = (result2 & (1<<index)) ? 0xffff : 0;
     }
     else {  // num_elements = 16
       for (int index = 0; index < 16; index++)
         result.xmmubyte(index) = (result2 & (1<<index)) ? 0xff : 0;
     }
  }
  else {
     result.xmm64u(1) = 0;
     result.xmm64u(0) = (Bit64u) result2;
  }

  Bit32u flags = 0;
  if (result2 != 0) flags |= EFlagsCFMask;
  if (len1 < num_elements) flags |= EFlagsSFMask;
  if (len2 < num_elements) flags |= EFlagsZFMask;
  if (result2 & 0x1)
    flags |= EFlagsOFMask;
  setEFlagsOSZAPC(flags);

  BX_WRITE_XMM_REGZ(0, result, i->getVL()); /* store result XMM0 */

  BX_NEXT_INSTR(i);
}

/* 66 0F 3A 61 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPESTRI_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());
  Bit8u imm8 = i->Ib();

  // compare all pairs of Ai, Bj
  Bit8u BoolRes[16][16];
  compare_strings(BoolRes, op1, op2, imm8);
  unsigned len1, len2, num_elements = (imm8 & 0x1) ? 8 : 16;
  int index;

#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    len1 = find_eos64(RAX, imm8);
    len2 = find_eos64(RDX, imm8);
  }
  else
#endif
  {
    len1 = find_eos32(EAX, imm8);
    len2 = find_eos32(EDX, imm8);
  }
  Bit16u result2 = aggregate(BoolRes, len1, len2, imm8);

  // The index of the first (or last, according to imm8[6]) set bit of result2
  // is returned to ECX. If no bits are set in IntRes2, ECX is set to 16 (8)
  if (imm8 & 0x40) {
     // The index returned to ECX is of the MSB in result2
     for (index=num_elements-1; index>=0; index--)
       if (result2 & (1<<index)) break;
     if (index < 0) index = num_elements;
  }
  else {
     // The index returned to ECX is of the LSB in result2
     for (index=0; index<(int)num_elements; index++)
       if (result2 & (1<<index)) break;
  }
  RCX = index;

  Bit32u flags = 0;
  if (result2 != 0) flags |= EFlagsCFMask;
  if (len1 < num_elements) flags |= EFlagsSFMask;
  if (len2 < num_elements) flags |= EFlagsZFMask;
  if (result2 & 0x1)
    flags |= EFlagsOFMask;
  setEFlagsOSZAPC(flags);

  BX_NEXT_INSTR(i);
}

/* 66 0F 3A 62 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPISTRM_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;
  Bit8u imm8 = i->Ib();

  // compare all pairs of Ai, Bj
  Bit8u BoolRes[16][16];
  compare_strings(BoolRes, op1, op2, imm8);

  unsigned num_elements = (imm8 & 0x1) ? 8 : 16;
  unsigned len1 = find_eos(op1, imm8);
  unsigned len2 = find_eos(op2, imm8);
  Bit16u result2 = aggregate(BoolRes, len1, len2, imm8);

  // As defined by imm8[6], result2 is then either stored to the least
  // significant bits of XMM0 (zero extended to 128 bits) or expanded
  // into a byte/word-mask and then stored to XMM0
  if (imm8 & 0x40) {
     if (num_elements == 8) {
       for (int index = 0; index < 8; index++)
         result.xmm16u(index) = (result2 & (1<<index)) ? 0xffff : 0;
     }
     else {  // num_elements = 16
       for (int index = 0; index < 16; index++)
         result.xmmubyte(index) = (result2 & (1<<index)) ? 0xff : 0;
     }
  }
  else {
     result.xmm64u(1) = 0;
     result.xmm64u(0) = (Bit64u) result2;
  }

  Bit32u flags = 0;
  if (result2 != 0) flags |= EFlagsCFMask;
  if (len1 < num_elements) flags |= EFlagsSFMask;
  if (len2 < num_elements) flags |= EFlagsZFMask;
  if (result2 & 0x1)
    flags |= EFlagsOFMask;
  setEFlagsOSZAPC(flags);

  BX_WRITE_XMM_REGZ(0, result, i->getVL()); /* store result XMM0 */

  BX_NEXT_INSTR(i);
}

/* 66 0F 3A 63 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PCMPISTRI_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());
  Bit8u imm8 = i->Ib();

  // compare all pairs of Ai, Bj
  Bit8u BoolRes[16][16];
  compare_strings(BoolRes, op1, op2, imm8);
  unsigned num_elements = (imm8 & 0x1) ? 8 : 16;
  int index;

  unsigned len1 = find_eos(op1, imm8);
  unsigned len2 = find_eos(op2, imm8);
  Bit16u result2 = aggregate(BoolRes, len1, len2, imm8);

  // The index of the first (or last, according to imm8[6]) set bit of result2
  // is returned to ECX. If no bits are set in IntRes2, ECX is set to 16 (8)
  if (imm8 & 0x40) {
     // The index returned to ECX is of the MSB in result2
     for (index=num_elements-1; index>=0; index--)
       if (result2 & (1<<index)) break;
     if (index < 0) index = num_elements;
  }
  else {
     // The index returned to ECX is of the LSB in result2
     for (index=0; index<(int)num_elements; index++)
       if (result2 & (1<<index)) break;
  }
  RCX = index;

  Bit32u flags = 0;
  if (result2 != 0) flags |= EFlagsCFMask;
  if (len1 < num_elements) flags |= EFlagsSFMask;
  if (len2 < num_elements) flags |= EFlagsZFMask;
  if (result2 & 0x1)
    flags |= EFlagsOFMask;
  setEFlagsOSZAPC(flags);

  BX_NEXT_INSTR(i);
}

#endif // BX_CPU_LEVEL >= 6
