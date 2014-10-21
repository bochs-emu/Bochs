/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2014 Stanislav Shwartsman
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

#if BX_SUPPORT_X86_64

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_byte(unsigned s, Bit64u laddr, Bit8u data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 1, CPL, BX_WRITE, (Bit8u*) &data);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 1);
      *hostAddr = data;
      return;
    }
  }

  if (access_write_linear(laddr, 1, CPL, 0x0, (void *) &data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_word(unsigned s, Bit64u laddr, Bit16u data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, CPL, BX_WRITE, (Bit8u*) &data);
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 2);
      WriteHostWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 2, CPL, 0x1, (void *) &data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_dword(unsigned s, Bit64u laddr, Bit32u data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, CPL, BX_WRITE, (Bit8u*) &data);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 4);
      WriteHostDWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (! IsCanonical(laddr)) {
    BX_ERROR(("write_linear_dword(): canonical failure"));
    exception(int_number(s), 0);
  }

  if (access_write_linear(laddr, 4, CPL, 0x3, (void *) &data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_qword(unsigned s, Bit64u laddr, Bit64u data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 8, CPL, BX_WRITE, (Bit8u*) &data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 8);
      WriteHostQWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (! IsCanonical(laddr)) {
    BX_ERROR(("write_linear_qword(): canonical failure"));
    exception(int_number(s), 0);
  }

  if (access_write_linear(laddr, 8, CPL, 0x7, (void *) &data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_xmmword(unsigned s, Bit64u laddr, const BxPackedXmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 15);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 16, CPL, BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 16);
      WriteHostQWordToLittleEndian(hostAddr,   data->xmm64u(0));
      WriteHostQWordToLittleEndian(hostAddr+1, data->xmm64u(1));
      return;
    }
  }

  if (access_write_linear(laddr, 16, CPL, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_xmmword_aligned(unsigned s, Bit64u laddr, const BxPackedXmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = AlignedAccessLPFOf(laddr, 15);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 16, CPL, BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 16);
      WriteHostQWordToLittleEndian(hostAddr,   data->xmm64u(0));
      WriteHostQWordToLittleEndian(hostAddr+1, data->xmm64u(1));
      return;
    }
  }

  if (laddr & 15) {
    BX_ERROR(("write_linear_xmmword_aligned(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (access_write_linear(laddr, 16, CPL, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

#if BX_SUPPORT_AVX

void BX_CPU_C::write_linear_ymmword(unsigned s, Bit64u laddr, const BxPackedYmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 31);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 32, CPL, BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 32);
      for (unsigned n = 0; n < 4; n++) {
        WriteHostQWordToLittleEndian(hostAddr+n, data->ymm64u(n));
      }
      return;
    }
  }

  if (access_write_linear(laddr, 32, CPL, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

void BX_CPU_C::write_linear_ymmword_aligned(unsigned s, Bit64u laddr, const BxPackedYmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = AlignedAccessLPFOf(laddr, 31);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 32, CPL, BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 32);
      for (unsigned n = 0; n < 4; n++) {
        WriteHostQWordToLittleEndian(hostAddr+n, data->ymm64u(n));
      }
      return;
    }
  }

  if (laddr & 31) {
    BX_ERROR(("write_linear_ymmword_aligned(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (access_write_linear(laddr, 32, CPL, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

#endif

#if BX_SUPPORT_EVEX

void BX_CPU_C::write_linear_zmmword(unsigned s, Bit64u laddr, const BxPackedZmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 63);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 64, CPL, BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 64);
      for (unsigned n = 0; n < 8; n++) {
        WriteHostQWordToLittleEndian(hostAddr+n, data->zmm64u(n));
      }
      return;
    }
  }

  if (access_write_linear(laddr, 64, CPL, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

void BX_CPU_C::write_linear_zmmword_aligned(unsigned s, Bit64u laddr, const BxPackedZmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = AlignedAccessLPFOf(laddr, 63);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 64, CPL, BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 64);
      for (unsigned n = 0; n < 8; n++) {
        WriteHostQWordToLittleEndian(hostAddr+n, data->zmm64u(n));
      }
      return;
    }
  }

  if (laddr & 63) {
    BX_ERROR(("write_linear_zmmword_aligned(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (access_write_linear(laddr, 64, CPL, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

#endif

  Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_linear_byte(unsigned s, Bit64u laddr)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
  Bit8u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      data = *hostAddr;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 1, CPL, BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 1, CPL, BX_READ, 0x0, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_linear_word(unsigned s, Bit64u laddr)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
  Bit16u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      ReadHostWordFromLittleEndian(hostAddr, data);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 2, CPL, BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 2, CPL, BX_READ, 0x1, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_linear_dword(unsigned s, Bit64u laddr)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
  Bit32u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      ReadHostDWordFromLittleEndian(hostAddr, data);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 4, CPL, BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 4, CPL, BX_READ, 0x3, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_linear_qword(unsigned s, Bit64u laddr)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
  Bit64u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      ReadHostQWordFromLittleEndian(hostAddr, data);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 8, CPL, BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 8, CPL, BX_READ, 0x7, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_linear_xmmword(unsigned s, Bit64u laddr, BxPackedXmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 15);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      ReadHostQWordFromLittleEndian(hostAddr,   data->xmm64u(0));
      ReadHostQWordFromLittleEndian(hostAddr+1, data->xmm64u(1));
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 16, CPL, BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (access_read_linear(laddr, 16, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_linear_xmmword_aligned(unsigned s, Bit64u laddr, BxPackedXmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = AlignedAccessLPFOf(laddr, 15);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      ReadHostQWordFromLittleEndian(hostAddr,   data->xmm64u(0));
      ReadHostQWordFromLittleEndian(hostAddr+1, data->xmm64u(1));
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 16, CPL, BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (laddr & 15) {
    BX_ERROR(("read_linear_xmmword_aligned(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (access_read_linear(laddr, 16, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

#if BX_SUPPORT_AVX

void BX_CPU_C::read_linear_ymmword(unsigned s, Bit64u laddr, BxPackedYmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 31);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      for (unsigned n=0; n < 4; n++) {
        ReadHostQWordFromLittleEndian(hostAddr+n, data->ymm64u(n));
      }
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 32, CPL, BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (access_read_linear(laddr, 32, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

void BX_CPU_C::read_linear_ymmword_aligned(unsigned s, Bit64u laddr, BxPackedYmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = AlignedAccessLPFOf(laddr, 31);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      for (unsigned n=0; n < 4; n++) {
        ReadHostQWordFromLittleEndian(hostAddr+n, data->ymm64u(n));
      }
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 32, CPL, BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (laddr & 31) {
    BX_ERROR(("read_linear_ymmword_aligned(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (access_read_linear(laddr, 32, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

#endif

#if BX_SUPPORT_EVEX

void BX_CPU_C::read_linear_zmmword(unsigned s, Bit64u laddr, BxPackedZmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 63);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      for (unsigned n=0; n < 8; n++) {
        ReadHostQWordFromLittleEndian(hostAddr+n, data->zmm64u(n));
      }
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 64, CPL, BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (access_read_linear(laddr, 64, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

void BX_CPU_C::read_linear_zmmword_aligned(unsigned s, Bit64u laddr, BxPackedZmmRegister *data)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = AlignedAccessLPFOf(laddr, 63);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access
    // from this CPL.
    if (tlbEntry->accessBits & (0x01 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      for (unsigned n=0; n < 8; n++) {
        ReadHostQWordFromLittleEndian(hostAddr+n, data->zmm64u(n));
      }
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 64, CPL, BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (laddr & 63) {
    BX_ERROR(("read_linear_zmmword_aligned(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (access_read_linear(laddr, 64, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

#endif

//////////////////////////////////////////////////////////////
// special Read-Modify-Write operations                     //
// address translation info is kept across read/write calls //
//////////////////////////////////////////////////////////////

  Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_byte_64(unsigned s, Bit64u laddr)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
  Bit8u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 1);
      data = *hostAddr;
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 1, CPL, BX_RW, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 1, CPL, BX_RW, 0x0, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_word_64(unsigned s, Bit64u laddr)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
  Bit16u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 2);
      ReadHostWordFromLittleEndian(hostAddr, data);
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, CPL, BX_RW, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 2, CPL, BX_RW, 0x1, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_dword_64(unsigned s, Bit64u laddr)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
  Bit32u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 4);
      ReadHostDWordFromLittleEndian(hostAddr, data);
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, CPL, BX_RW, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 4, CPL, BX_RW, 0x3, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_qword_64(unsigned s, Bit64u laddr)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);
  Bit64u data;

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  Bit64u lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  Bit64u lpf = LPFOf(laddr);
#endif    
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 8);
      ReadHostQWordFromLittleEndian(hostAddr, data);
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 8, CPL, BX_RW, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 8, CPL, BX_RW, 0x7, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

void BX_CPU_C::read_RMW_linear_dqword_aligned_64(unsigned s, Bit64u laddr, Bit64u *hi, Bit64u *lo)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64);

  unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
  Bit64u lpf = AlignedAccessLPFOf(laddr, 15);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access
    // from this CPL.
    if (tlbEntry->accessBits & (0x04 << USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 16);
      ReadHostQWordFromLittleEndian(hostAddr,     *lo);
      ReadHostQWordFromLittleEndian(hostAddr + 1, *hi);
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr,     pAddr,     8, CPL, BX_RW, (Bit8u*) lo);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr + 8, pAddr + 8, 8, CPL, BX_RW, (Bit8u*) hi);
      return;
    }
  }

  if (laddr & 15) {
    BX_ERROR(("read_RMW_virtual_dqword_aligned_64(): #GP misaligned access"));
    exception(BX_GP_EXCEPTION, 0);
  }

  BxPackedXmmRegister data;
  if (access_read_linear(laddr, 16, CPL, BX_RW, 0x0, (void *) &data) < 0)
    exception(int_number(s), 0);

  *lo = data.xmm64u(0);
  *hi = data.xmm64u(1);
}

void BX_CPU_C::write_RMW_linear_dqword(Bit64u hi, Bit64u lo)
{
  write_RMW_virtual_qword(lo);

  BX_CPU_THIS_PTR address_xlation.paddress1 += 8;
  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access
    BX_CPU_THIS_PTR address_xlation.pages += 8;
  }
  else {
    BX_ASSERT(BX_CPU_THIS_PTR address_xlation.pages == 1);
  }
  
  write_RMW_virtual_qword(hi);
}

#endif
