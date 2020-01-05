/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2019 Stanislav Shwartsman
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

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_byte(unsigned s, bx_address laddr, Bit8u data)
{
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 1, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) &data);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 1);
      *hostAddr = data;
      return;
    }
  }

  if (access_write_linear(laddr, 1, CPL, BX_WRITE, 0x0, (void *) &data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_word(unsigned s, bx_address laddr, Bit16u data)
{
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) &data);
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 2);
      WriteHostWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 2, CPL, BX_WRITE, 0x1, (void *) &data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_dword(unsigned s, bx_address laddr, Bit32u data)
{
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) &data);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 4);
      WriteHostDWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 4, CPL, BX_WRITE, 0x3, (void *) &data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_qword(unsigned s, bx_address laddr, Bit64u data)
{
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 8, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) &data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 8);
      WriteHostQWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 8, CPL, BX_WRITE, 0x7, (void *) &data) < 0)
    exception(int_number(s), 0);
}

#if BX_CPU_LEVEL >= 6

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_xmmword(unsigned s, bx_address laddr, const BxPackedXmmRegister *data)
{
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 15);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 16, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 16);
      WriteHostQWordToLittleEndian(hostAddr,   data->xmm64u(0));
      WriteHostQWordToLittleEndian(hostAddr+1, data->xmm64u(1));
      return;
    }
  }

  if (access_write_linear(laddr, 16, CPL, BX_WRITE, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_xmmword_aligned(unsigned s, bx_address laddr, const BxPackedXmmRegister *data)
{
  bx_address lpf = AlignedAccessLPFOf(laddr, 15);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 16, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) data);
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

  if (access_write_linear(laddr, 16, CPL, BX_WRITE, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_ymmword(unsigned s, bx_address laddr, const BxPackedYmmRegister *data)
{
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 31);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 32, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 32);
      for (unsigned n = 0; n < 4; n++) {
        WriteHostQWordToLittleEndian(hostAddr+n, data->ymm64u(n));
      }
      return;
    }
  }

  if (access_write_linear(laddr, 32, CPL, BX_WRITE, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_ymmword_aligned(unsigned s, bx_address laddr, const BxPackedYmmRegister *data)
{
  bx_address lpf = AlignedAccessLPFOf(laddr, 31);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 32, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) data);
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

  if (access_write_linear(laddr, 32, CPL, BX_WRITE, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_zmmword(unsigned s, bx_address laddr, const BxPackedZmmRegister *data)
{
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 63);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 64, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 64);
      for (unsigned n = 0; n < 8; n++) {
        WriteHostQWordToLittleEndian(hostAddr+n, data->zmm64u(n));
      }
      return;
    }
  }

  if (access_write_linear(laddr, 64, CPL, BX_WRITE, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_linear_zmmword_aligned(unsigned s, bx_address laddr, const BxPackedZmmRegister *data)
{
  bx_address lpf = AlignedAccessLPFOf(laddr, 63);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 64, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) data);
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

  if (access_write_linear(laddr, 64, CPL, BX_WRITE, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

#endif

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::tickle_read_linear(unsigned s, bx_address laddr)
{
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) return;
  }

#if BX_SUPPORT_X86_64
  if (! IsCanonical(laddr)) {
    BX_ERROR(("tickle_read_linear(): canonical failure"));
    exception(int_number(s), 0);
  }
#endif

  // Access within single page
  BX_CPU_THIS_PTR address_xlation.paddress1 = translate_linear(tlbEntry, laddr, USER_PL, BX_READ);
  BX_CPU_THIS_PTR address_xlation.pages     = 1;
#if BX_SUPPORT_MEMTYPE
  BX_CPU_THIS_PTR address_xlation.memtype1  = tlbEntry->get_memtype();
#endif

#if BX_X86_DEBUGGER
  hwbreakpoint_match(laddr, 1, BX_READ);
#endif
}

  Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_linear_byte(unsigned s, bx_address laddr)
{
  Bit8u data;

  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      data = *hostAddr;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 1, tlbEntry->get_memtype(), BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 1, CPL, BX_READ, 0x0, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_linear_word(unsigned s, bx_address laddr)
{
  Bit16u data;

  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      data = ReadHostWordFromLittleEndian(hostAddr);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 2, tlbEntry->get_memtype(), BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 2, CPL, BX_READ, 0x1, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_linear_dword(unsigned s, bx_address laddr)
{
  Bit32u data;

  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      data = ReadHostDWordFromLittleEndian(hostAddr);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 4, tlbEntry->get_memtype(), BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 4, CPL, BX_READ, 0x3, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_linear_qword(unsigned s, bx_address laddr)
{
  Bit64u data;

  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      data = ReadHostQWordFromLittleEndian(hostAddr);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 8, tlbEntry->get_memtype(), BX_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 8, CPL, BX_READ, 0x7, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

#if BX_CPU_LEVEL >= 6

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_linear_xmmword(unsigned s, bx_address laddr, BxPackedXmmRegister *data)
{
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 15);
  bx_address lpf = LPFOf(laddr);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      data->xmm64u(0) = ReadHostQWordFromLittleEndian(hostAddr);
      data->xmm64u(1) = ReadHostQWordFromLittleEndian(hostAddr+1);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 16, tlbEntry->get_memtype(), BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (access_read_linear(laddr, 16, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_linear_xmmword_aligned(unsigned s, bx_address laddr, BxPackedXmmRegister *data)
{
  bx_address lpf = AlignedAccessLPFOf(laddr, 15);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      data->xmm64u(0) = ReadHostQWordFromLittleEndian(hostAddr);
      data->xmm64u(1) = ReadHostQWordFromLittleEndian(hostAddr+1);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 16, tlbEntry->get_memtype(), BX_READ, (Bit8u*) data);
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

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_linear_ymmword(unsigned s, bx_address laddr, BxPackedYmmRegister *data)
{
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 31);
  bx_address lpf = LPFOf(laddr);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      for (unsigned n=0; n < 4; n++) {
        data->ymm64u(n) = ReadHostQWordFromLittleEndian(hostAddr+n);
      }
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 32, tlbEntry->get_memtype(), BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (access_read_linear(laddr, 32, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_linear_ymmword_aligned(unsigned s, bx_address laddr, BxPackedYmmRegister *data)
{
  bx_address lpf = AlignedAccessLPFOf(laddr, 31);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      for (unsigned n=0; n < 4; n++) {
        data->ymm64u(n) = ReadHostQWordFromLittleEndian(hostAddr+n);
      }
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 32, tlbEntry->get_memtype(), BX_READ, (Bit8u*) data);
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

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_linear_zmmword(unsigned s, bx_address laddr, BxPackedZmmRegister *data)
{
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 63);
  bx_address lpf = LPFOf(laddr);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      for (unsigned n=0; n < 8; n++) {
        data->zmm64u(n) = ReadHostQWordFromLittleEndian(hostAddr+n);
      }
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 64, tlbEntry->get_memtype(), BX_READ, (Bit8u*) data);
      return;
    }
  }

  if (access_read_linear(laddr, 64, CPL, BX_READ, 0x0, (void *) data) < 0)
    exception(int_number(s), 0);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_linear_zmmword_aligned(unsigned s, bx_address laddr, BxPackedZmmRegister *data)
{
  bx_address lpf = AlignedAccessLPFOf(laddr, 63);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isReadOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      for (unsigned n=0; n < 8; n++) {
        data->zmm64u(n) = ReadHostQWordFromLittleEndian(hostAddr+n);
      }
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, (tlbEntry->ppf | pageOffset), 64, tlbEntry->get_memtype(), BX_READ, (Bit8u*) data);
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
BX_CPU_C::read_RMW_linear_byte(unsigned s, bx_address laddr)
{
  Bit8u data;
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit8u *hostAddr = (Bit8u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 1);
      data = *hostAddr;
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
#if BX_SUPPORT_MEMTYPE
      BX_CPU_THIS_PTR address_xlation.memtype1 = tlbEntry->get_memtype();
#endif
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 1, tlbEntry->get_memtype(), BX_RW, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 1, CPL, BX_RW, 0x0, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_linear_word(unsigned s, bx_address laddr)
{
  Bit16u data;
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 2);
      data = ReadHostWordFromLittleEndian(hostAddr);
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
#if BX_SUPPORT_MEMTYPE
      BX_CPU_THIS_PTR address_xlation.memtype1 = tlbEntry->get_memtype();
#endif
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, tlbEntry->get_memtype(), BX_RW, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 2, CPL, BX_RW, 0x1, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_linear_dword(unsigned s, bx_address laddr)
{
  Bit32u data;
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 4);
      data = ReadHostDWordFromLittleEndian(hostAddr);
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
#if BX_SUPPORT_MEMTYPE
      BX_CPU_THIS_PTR address_xlation.memtype1 = tlbEntry->get_memtype();
#endif
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, tlbEntry->get_memtype(), BX_RW, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 4, CPL, BX_RW, 0x3, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_linear_qword(unsigned s, bx_address laddr)
{
  Bit64u data;
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 8);
      data = ReadHostQWordFromLittleEndian(hostAddr);
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
#if BX_SUPPORT_MEMTYPE
      BX_CPU_THIS_PTR address_xlation.memtype1 = tlbEntry->get_memtype();
#endif
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 8, tlbEntry->get_memtype(), BX_RW, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(laddr, 8, CPL, BX_RW, 0x7, (void *) &data) < 0)
    exception(int_number(s), 0);

  return data;
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_linear_byte(Bit8u val8)
{
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
    BX_CPU_THIS_PTR address_xlation.paddress1, 1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1), BX_WRITE, 0, (Bit8u*) &val8);

  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit8u *hostAddr = (Bit8u *) BX_CPU_THIS_PTR address_xlation.pages;
    *hostAddr = val8;
  }
  else {
    // address_xlation.pages must be 1
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1, &val8);
  }
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_linear_word(Bit16u val16)
{
  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit16u *hostAddr = (Bit16u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostWordToLittleEndian(hostAddr, val16);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 2, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, (Bit8u*) &val16);
  }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 2, &val16);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 2, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, (Bit8u*) &val16);
  }
  else {
#ifdef BX_LITTLE_ENDIAN
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1, &val16);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0,  (Bit8u*) &val16);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2, 1, ((Bit8u *) &val16) + 1);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2, 1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype2),
         BX_WRITE, 0, ((Bit8u*) &val16)+1);
#else
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 1, ((Bit8u *) &val16) + 1);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, ((Bit8u*) &val16)+1);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2, 1, &val16);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2, 1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype2),
        BX_WRITE, 0,  (Bit8u*) &val16);
#endif
  }
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_linear_dword(Bit32u val32)
{
  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit32u *hostAddr = (Bit32u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostDWordToLittleEndian(hostAddr, val32);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 4, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, (Bit8u*) &val32);
  }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 4, &val32);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 4, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, (Bit8u*) &val32);
  }
  else {
#ifdef BX_LITTLE_ENDIAN
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, &val32);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, (Bit8u*) &val32);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        ((Bit8u *) &val32) + BX_CPU_THIS_PTR address_xlation.len1);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype2),
        BX_WRITE, 0, ((Bit8u *) &val32) + BX_CPU_THIS_PTR address_xlation.len1);
#else
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        ((Bit8u *) &val32) + (4 - BX_CPU_THIS_PTR address_xlation.len1));
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, ((Bit8u *) &val32) + (4 - BX_CPU_THIS_PTR address_xlation.len1));
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, &val32);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype2),
        BX_WRITE, 0, (Bit8u*) &val32);
#endif
  }
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::write_RMW_linear_qword(Bit64u val64)
{
  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access.
    Bit64u *hostAddr = (Bit64u *) BX_CPU_THIS_PTR address_xlation.pages;
    WriteHostQWordToLittleEndian(hostAddr, val64);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 8, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, (Bit8u*) &val64);
  }
  else if (BX_CPU_THIS_PTR address_xlation.pages == 1) {
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, 8, &val64);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1, 8, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, (Bit8u*) &val64);
  }
  else {
#ifdef BX_LITTLE_ENDIAN
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, &val64);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, (Bit8u*) &val64);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        ((Bit8u *) &val64) + BX_CPU_THIS_PTR address_xlation.len1);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype2),
        BX_WRITE, 0, ((Bit8u *) &val64) + BX_CPU_THIS_PTR address_xlation.len1);
#else
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        ((Bit8u *) &val64) + (8 - BX_CPU_THIS_PTR address_xlation.len1));
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype1),
        BX_WRITE, 0, ((Bit8u *) &val64) + (8 - BX_CPU_THIS_PTR address_xlation.len1));
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, &val64);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID,
        BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, MEMTYPE(BX_CPU_THIS_PTR address_xlation.memtype2),
        BX_WRITE, 0, (Bit8u*) &val64);
#endif
  }
}

#if BX_SUPPORT_X86_64

void BX_CPU_C::read_RMW_linear_dqword_aligned_64(unsigned s, bx_address laddr, Bit64u *hi, Bit64u *lo)
{
  bx_address lpf = AlignedAccessLPFOf(laddr, 15);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, USER_PL)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 16);
      *lo = ReadHostQWordFromLittleEndian(hostAddr);
      *hi = ReadHostQWordFromLittleEndian(hostAddr + 1);
      BX_CPU_THIS_PTR address_xlation.pages = (bx_ptr_equiv_t) hostAddr;
      BX_CPU_THIS_PTR address_xlation.paddress1 = pAddr;
#if BX_SUPPORT_MEMTYPE
      BX_CPU_THIS_PTR address_xlation.memtype1 = tlbEntry->get_memtype();
#endif
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr,     pAddr,     8, tlbEntry->get_memtype(), BX_RW, (Bit8u*) lo);
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr + 8, pAddr + 8, 8, tlbEntry->get_memtype(), BX_RW, (Bit8u*) hi);
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
  write_RMW_linear_qword(lo);

  BX_CPU_THIS_PTR address_xlation.paddress1 += 8;
  if (BX_CPU_THIS_PTR address_xlation.pages > 2) {
    // Pages > 2 means it stores a host address for direct access
    BX_CPU_THIS_PTR address_xlation.pages += 8;
  }
  else {
    BX_ASSERT(BX_CPU_THIS_PTR address_xlation.pages == 1);
  }
  
  write_RMW_linear_qword(hi);
}

#endif

//
// Write data to new stack, these methods are required for emulation
// correctness but not performance critical.
//

void BX_CPU_C::write_new_stack_word(bx_address laddr, unsigned curr_pl, Bit16u data)
{
  bx_bool user = (curr_pl == 3);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 1);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (1 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 2, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) &data);
      Bit16u *hostAddr = (Bit16u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 2);
      WriteHostWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 2, curr_pl, BX_WRITE, 0x1, (void *) &data) < 0)
    exception(BX_SS_EXCEPTION, 0);
}

void BX_CPU_C::write_new_stack_dword(bx_address laddr, unsigned curr_pl, Bit32u data)
{
  bx_bool user = (curr_pl == 3);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 3);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (3 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 4, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) &data);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 4);
      WriteHostDWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 4, curr_pl, BX_WRITE, 0x3, (void *) &data) < 0)
    exception(BX_SS_EXCEPTION, 0);
}

void BX_CPU_C::write_new_stack_qword(bx_address laddr, unsigned curr_pl, Bit64u data)
{
  bx_bool user = (curr_pl == 3);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 7);
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  bx_address lpf = AlignedAccessLPFOf(laddr, (7 & BX_CPU_THIS_PTR alignment_check_mask));
#else
  bx_address lpf = LPFOf(laddr);
#endif    
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isWriteOK(tlbEntry, user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(laddr);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, pAddr, 8, tlbEntry->get_memtype(), BX_WRITE, (Bit8u*) &data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 8);
      WriteHostQWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(laddr, 8, curr_pl, BX_WRITE, 0x7, (void *) &data) < 0)
    exception(BX_SS_EXCEPTION, 0);
}

// assuming the write happens in 32-bit mode
void BX_CPU_C::write_new_stack_word(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit16u data)
{
  Bit32u laddr;

  if (seg->cache.valid & SegAccessWOK4G) {
    goto accessOK;
  }

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < seg->cache.u.segment.limit_scaled) {
accessOK:
      laddr = (Bit32u)(seg->cache.u.segment.base) + offset;
      write_new_stack_word(laddr, curr_pl, data);
      return;
    }
  }

  // add error code when segment violation occurs when pushing into new stack
  if (!write_virtual_checks(seg, offset, 2)) {
    BX_ERROR(("write_new_stack_word(): segment limit violation"));
    exception(BX_SS_EXCEPTION, 
         seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
  }
  goto accessOK;
}

// assuming the write happens in 32-bit mode
void BX_CPU_C::write_new_stack_dword(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit32u data)
{
  Bit32u laddr;

  if (seg->cache.valid & SegAccessWOK4G) {
    goto accessOK;
  }

  if (seg->cache.valid & SegAccessWOK) {
    if (offset < (seg->cache.u.segment.limit_scaled-2)) {
accessOK:
      laddr = (Bit32u)(seg->cache.u.segment.base) + offset;
      write_new_stack_dword(laddr, curr_pl, data);
      return;
    }
  }

  // add error code when segment violation occurs when pushing into new stack
  if (!write_virtual_checks(seg, offset, 4)) {
    BX_ERROR(("write_new_stack_dword(): segment limit violation"));
    exception(BX_SS_EXCEPTION, 
         seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
  }
  goto accessOK;
}

// assuming the write happens in 32-bit mode
void BX_CPU_C::write_new_stack_qword(bx_segment_reg_t *seg, Bit32u offset, unsigned curr_pl, Bit64u data)
{
  Bit32u laddr;

  if (seg->cache.valid & SegAccessWOK4G) {
    goto accessOK;
  }

  if (seg->cache.valid & SegAccessWOK) {
    if (offset <= (seg->cache.u.segment.limit_scaled-7)) {
accessOK:
      laddr = (Bit32u)(seg->cache.u.segment.base) + offset;
      write_new_stack_qword(laddr, curr_pl, data);
      return;
    }
  }

  // add error code when segment violation occurs when pushing into new stack
  if (!write_virtual_checks(seg, offset, 8)) {
    BX_ERROR(("write_new_stack_qword(): segment limit violation"));
    exception(BX_SS_EXCEPTION, 
        seg->selector.rpl != CPL ? (seg->selector.value & 0xfffc) : 0);
  }
  goto accessOK;
}

#if BX_SUPPORT_CET
Bit32u BX_CPP_AttrRegparmN(2) BX_CPU_C::shadow_stack_read_dword(bx_address offset, unsigned curr_pl)
{
  Bit32u data;

  bx_bool user = (curr_pl == 3);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(offset, 3);
  bx_address lpf = AlignedAccessLPFOf(offset, 3);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isShadowStackReadOK(tlbEntry, user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(offset);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      data = ReadHostDWordFromLittleEndian(hostAddr);
      BX_NOTIFY_LIN_MEMORY_ACCESS(offset, (tlbEntry->ppf | pageOffset), 4, tlbEntry->get_memtype(), BX_SHADOW_STACK_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(offset, 4, curr_pl, BX_SHADOW_STACK_READ, 0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);

  return data;
}

Bit64u BX_CPP_AttrRegparmN(2) BX_CPU_C::shadow_stack_read_qword(bx_address offset, unsigned curr_pl)
{
  Bit64u data;

  bx_bool user = (curr_pl == 3);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(offset, 7);
  bx_address lpf = AlignedAccessLPFOf(offset, 7);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us read access from this CPL
    if (isShadowStackReadOK(tlbEntry, user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(offset);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      data = ReadHostQWordFromLittleEndian(hostAddr);
      BX_NOTIFY_LIN_MEMORY_ACCESS(offset, (tlbEntry->ppf | pageOffset), 8, tlbEntry->get_memtype(), BX_SHADOW_STACK_READ, (Bit8u*) &data);
      return data;
    }
  }

  if (access_read_linear(offset, 8, curr_pl, BX_SHADOW_STACK_READ, 0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);

  return data;
}

void BX_CPP_AttrRegparmN(3) BX_CPU_C::shadow_stack_write_dword(bx_address offset, unsigned curr_pl, Bit32u data)
{
  bx_bool user = (curr_pl == 3);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(offset, 3);
  bx_address lpf = AlignedAccessLPFOf(offset, 3);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isShadowStackWriteOK(tlbEntry, user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(offset);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(offset, pAddr, 4, tlbEntry->get_memtype(), BX_SHADOW_STACK_WRITE, (Bit8u*) &data);
      Bit32u *hostAddr = (Bit32u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 4);
      WriteHostDWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(offset, 4, curr_pl, BX_SHADOW_STACK_WRITE, 0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);
}

void BX_CPP_AttrRegparmN(3) BX_CPU_C::shadow_stack_write_qword(bx_address offset, unsigned curr_pl, Bit64u data)
{
  bx_bool user = (curr_pl == 3);
  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(offset, 7);
  bx_address lpf = AlignedAccessLPFOf(offset, 7);
  if (tlbEntry->lpf == lpf) {
    // See if the TLB entry privilege level allows us write access from this CPL
    if (isShadowStackWriteOK(tlbEntry, user)) {
      bx_hostpageaddr_t hostPageAddr = tlbEntry->hostPageAddr;
      Bit32u pageOffset = PAGE_OFFSET(offset);
      bx_phy_address pAddr = tlbEntry->ppf | pageOffset;
      BX_NOTIFY_LIN_MEMORY_ACCESS(offset, pAddr, 8, tlbEntry->get_memtype(), BX_SHADOW_STACK_WRITE, (Bit8u*) &data);
      Bit64u *hostAddr = (Bit64u*) (hostPageAddr | pageOffset);
      pageWriteStampTable.decWriteStamp(pAddr, 8);
      WriteHostQWordToLittleEndian(hostAddr, data);
      return;
    }
  }

  if (access_write_linear(offset, 8, curr_pl, BX_SHADOW_STACK_WRITE, 0, (void *) &data) < 0)
    exception(BX_GP_EXCEPTION, 0);
}

bx_bool BX_CPP_AttrRegparmN(4) BX_CPU_C::shadow_stack_lock_cmpxchg8b(bx_address offset, unsigned curr_pl, Bit64u data, Bit64u expected_data)
{
  Bit64u val64 = shadow_stack_read_qword(offset, curr_pl);
  if (val64 == expected_data) {
    shadow_stack_write_qword(offset, curr_pl, data);
    return true;
  }
  else {
    shadow_stack_write_qword(offset, curr_pl, val64);
    return false;
  }
}

bx_bool BX_CPP_AttrRegparmN(2) BX_CPU_C::shadow_stack_atomic_set_busy(bx_address offset, unsigned curr_pl)
{
  return shadow_stack_lock_cmpxchg8b(offset, curr_pl, offset | 0x1, offset);
}

bx_bool BX_CPP_AttrRegparmN(2) BX_CPU_C::shadow_stack_atomic_clear_busy(bx_address offset, unsigned curr_pl)
{
  return shadow_stack_lock_cmpxchg8b(offset, curr_pl, offset, offset | 0x1);
}
#endif
