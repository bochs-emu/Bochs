/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2009-2019 Stanislav Shwartsman
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
#include "msr.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "iodev/iodev.h"

#if BX_SUPPORT_VMX

extern VMCS_Mapping vmcs_map;

#if BX_SUPPORT_VMX >= 2
extern bool isValidMSR_PAT(Bit64u pat_msr);
#endif

#if BX_SUPPORT_CET
extern bool is_invalid_cet_control(bx_address val);
#endif

extern const char *segname[];

////////////////////////////////////////////////////////////
// VMEXIT reasons for BX prints
////////////////////////////////////////////////////////////

static const char *VMX_vmexit_reason_name[] =
{
  /*  0 */  "Exception or NMI",
  /*  1 */  "External Interrupt",
  /*  2 */  "Triple Fault",
  /*  3 */  "INIT",
  /*  4 */  "SIPI",
  /*  5 */  "I/O SMI (SMM Vmexit)",
  /*  6 */  "SMI (SMM Vmexit)",
  /*  7 */  "Interrupt Window Exiting",
  /*  8 */  "NMI Window Exiting",
  /*  9 */  "Task Switch",
  /* 10 */  "CPUID",
  /* 11 */  "GETSEC",
  /* 12 */  "HLT",
  /* 13 */  "INVD",
  /* 14 */  "INVLPG",
  /* 15 */  "RDPMC",
  /* 16 */  "RDTSC",
  /* 17 */  "RSM",
  /* 18 */  "VMCALL",
  /* 19 */  "VMCLEAR",
  /* 20 */  "VMLAUNCH",
  /* 21 */  "VMPTRLD",
  /* 22 */  "VMPTRST",
  /* 23 */  "VMREAD",
  /* 24 */  "VMRESUME",
  /* 25 */  "VMWRITE",
  /* 26 */  "VMXOFF",
  /* 27 */  "VMXON",
  /* 28 */  "CR Access",
  /* 29 */  "DR Access",
  /* 30 */  "I/O Instruction",
  /* 31 */  "RDMSR",
  /* 32 */  "WRMSR",
  /* 33 */  "VMEntry failure due to invalid guest state",
  /* 34 */  "VMEntry failure due to MSR loading",
  /* 35 */  "Reserved35",
  /* 36 */  "MWAIT",
  /* 37 */  "MTF (Monitor Trap Flag)",
  /* 38 */  "Reserved38",
  /* 39 */  "MONITOR",
  /* 40 */  "PAUSE",
  /* 41 */  "VMEntry failure due to machine check",
  /* 42 */  "Reserved42",
  /* 43 */  "TPR Below Threshold",
  /* 44 */  "APIC Access",
  /* 45 */  "Virtualized EOI",
  /* 46 */  "GDTR/IDTR Access",
  /* 47 */  "LDTR/TR Access",
  /* 48 */  "EPT Violation",
  /* 49 */  "EPT Misconfiguration",
  /* 50 */  "INVEPT",
  /* 51 */  "RDTSCP",
  /* 52 */  "VMX preemption timer expired",
  /* 53 */  "INVVPID",
  /* 54 */  "WBINVD",
  /* 55 */  "XSETBV",
  /* 56 */  "APIC Write Trap",
  /* 57 */  "RDRAND",
  /* 58 */  "INVPCID",
  /* 59 */  "VMFUNC",
  /* 60 */  "ENCLS",
  /* 61 */  "RDSEED",
  /* 62 */  "PML Log Full",
  /* 63 */  "XSAVES",
  /* 64 */  "XRSTORS",
  /* 65 */  "PCONFIG",
  /* 66 */  "Sub-Page Protection",
  /* 67 */  "UMWAIT",
  /* 68 */  "TPAUSE",
  /* 69 */  "LOADIWKEY",
  /* 70 */  "Reserved70",
  /* 71 */  "Reserved71",
  /* 72 */  "ENQCMD PASID Translation",
  /* 73 */  "ENQCMDS PASID Translation",
  /* 74 */  "Bus Lock",
  /* 75 */  "Notify Window",
};

#include "decoder/ia_opcodes.h"

////////////////////////////////////////////////////////////
// VMCS access
////////////////////////////////////////////////////////////

void BX_CPU_C::set_VMCSPTR(Bit64u vmxptr)
{
  BX_CPU_THIS_PTR vmcsptr = vmxptr;

  if (vmxptr != BX_INVALID_VMCSPTR) {
    BX_CPU_THIS_PTR vmcshostptr = BX_CPU_THIS_PTR getHostMemAddr(vmxptr, BX_WRITE);
#if BX_SUPPORT_MEMTYPE
    // IA32_VMX_BASIC MSR report the memory type that should be used for the VMCS, for data structures referenced by 
    // pointers in the VMCS (I/O bitmaps, virtual-APIC page, MSR areas for VMX transitions), and for the MSEG header
    BX_CPU_THIS_PTR vmcs_memtype = BX_MEMTYPE_WB;
#endif
  }
  else {
    BX_CPU_THIS_PTR vmcshostptr = 0;
#if BX_SUPPORT_MEMTYPE
    BX_CPU_THIS_PTR vmcs_memtype = BX_MEMTYPE_UC;
#endif
  }
}

Bit16u BX_CPP_AttrRegparmN(1) BX_CPU_C::VMread16(unsigned encoding)
{
  Bit16u field;

  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMread16: can't access encoding 0x%08x, offset=0x%x", encoding, offset));
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcsptr + offset;

  if (BX_CPU_THIS_PTR vmcshostptr) {
    Bit16u *hostAddr = (Bit16u*) (BX_CPU_THIS_PTR vmcshostptr | offset);
    field = ReadHostWordFromLittleEndian(hostAddr);
  }
  else {
    access_read_physical(pAddr, 2, (Bit8u*)(&field));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 2, MEMTYPE(BX_CPU_THIS_PTR vmcs_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&field));

  return field;
}

// write 16-bit value into VMCS 16-bit field
void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMwrite16(unsigned encoding, Bit16u val_16)
{
  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMwrite16: can't access encoding 0x%08x, offset=0x%x", encoding, offset));
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcsptr + offset;

  if (BX_CPU_THIS_PTR vmcshostptr) {
    Bit16u *hostAddr = (Bit16u*) (BX_CPU_THIS_PTR vmcshostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 2);
    WriteHostWordToLittleEndian(hostAddr, val_16);
  }
  else {
    access_write_physical(pAddr, 2, (Bit8u*)(&val_16));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 2, MEMTYPE(BX_CPU_THIS_PTR vmcs_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_16));
}

Bit32u BX_CPP_AttrRegparmN(1) BX_CPU_C::VMread32(unsigned encoding)
{
  Bit32u field;

  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMread32: can't access encoding 0x%08x, offset=0x%x", encoding, offset));
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcsptr + offset;

  if (BX_CPU_THIS_PTR vmcshostptr) {
    Bit32u *hostAddr = (Bit32u*) (BX_CPU_THIS_PTR vmcshostptr | offset);
    field = ReadHostDWordFromLittleEndian(hostAddr);
  }
  else {
    access_read_physical(pAddr, 4, (Bit8u*)(&field));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(BX_CPU_THIS_PTR vmcs_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&field));

  return field;
}

// write 32-bit value into VMCS field
void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMwrite32(unsigned encoding, Bit32u val_32)
{
  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMwrite32: can't access encoding 0x%08x, offset=0x%x", encoding, offset));
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcsptr + offset;

  if (BX_CPU_THIS_PTR vmcshostptr) {
    Bit32u *hostAddr = (Bit32u*) (BX_CPU_THIS_PTR vmcshostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 4);
    WriteHostDWordToLittleEndian(hostAddr, val_32);
  }
  else {
    access_write_physical(pAddr, 4, (Bit8u*)(&val_32));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(BX_CPU_THIS_PTR vmcs_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_32));
}

Bit64u BX_CPP_AttrRegparmN(1) BX_CPU_C::VMread64(unsigned encoding)
{
  BX_ASSERT(!IS_VMCS_FIELD_HI(encoding));

  Bit64u field;

  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMread64: can't access encoding 0x%08x, offset=0x%x", encoding, offset));
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcsptr + offset;

  if (BX_CPU_THIS_PTR vmcshostptr) {
    Bit64u *hostAddr = (Bit64u*) (BX_CPU_THIS_PTR vmcshostptr | offset);
    field = ReadHostQWordFromLittleEndian(hostAddr);
  }
  else {
    access_read_physical(pAddr, 8, (Bit8u*)(&field));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(BX_CPU_THIS_PTR vmcs_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&field));

  return field;
}

// write 64-bit value into VMCS field
void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMwrite64(unsigned encoding, Bit64u val_64)
{
  BX_ASSERT(!IS_VMCS_FIELD_HI(encoding));

  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMwrite64: can't access encoding 0x%08x, offset=0x%x", encoding, offset));
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcsptr + offset;

  if (BX_CPU_THIS_PTR vmcshostptr) {
    Bit64u *hostAddr = (Bit64u*) (BX_CPU_THIS_PTR vmcshostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 8);
    WriteHostQWordToLittleEndian(hostAddr, val_64);
  }
  else {
    access_write_physical(pAddr, 8, (Bit8u*)(&val_64));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(BX_CPU_THIS_PTR vmcs_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_64));
}

#if BX_SUPPORT_X86_64
BX_CPP_INLINE bx_address BX_CPP_AttrRegparmN(1) BX_CPU_C::VMread_natural(unsigned encoding)
{
  return VMread64(encoding);
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMwrite_natural(unsigned encoding, bx_address val)
{
  VMwrite64(encoding, val);
}
#else
BX_CPP_INLINE bx_address BX_CPP_AttrRegparmN(1) BX_CPU_C::VMread_natural(unsigned encoding)
{
  return VMread32(encoding);
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMwrite_natural(unsigned encoding, bx_address val)
{
  VMwrite32(encoding, val);
}
#endif

////////////////////////////////////////////////////////////
// Shadow VMCS access
////////////////////////////////////////////////////////////

#if BX_SUPPORT_VMX >= 2

Bit16u BX_CPP_AttrRegparmN(1) BX_CPU_C::VMread16_Shadow(unsigned encoding)
{
  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMread16_Shadow: can't access encoding 0x%08x, offset=0x%x", encoding, offset));

  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.vmcs_linkptr + offset;
  Bit16u field;
  access_read_physical(pAddr, 2, (Bit8u*)(&field));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 2, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_SHADOW_VMCS_ACCESS, (Bit8u*)(&field));

  return field;
}

// write 16-bit value into shadow VMCS 16-bit field
void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMwrite16_Shadow(unsigned encoding, Bit16u val_16)
{
  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMwrite16_Shadow: can't access encoding 0x%08x, offset=0x%x", encoding, offset));

  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.vmcs_linkptr + offset;
  access_write_physical(pAddr, 2, (Bit8u*)(&val_16));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 2, MEMTYPE(resolve_memtype(pAddr)), BX_WRITE, BX_SHADOW_VMCS_ACCESS, (Bit8u*)(&val_16));
}

Bit32u BX_CPP_AttrRegparmN(1) BX_CPU_C::VMread32_Shadow(unsigned encoding)
{
  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMread32_Shadow: can't access encoding 0x%08x, offset=0x%x", encoding, offset));

  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.vmcs_linkptr + offset;
  Bit32u field;
  access_read_physical(pAddr, 4, (Bit8u*)(&field));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_SHADOW_VMCS_ACCESS, (Bit8u*)(&field));

  return field;
}

// write 32-bit value into shadow VMCS field
void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMwrite32_Shadow(unsigned encoding, Bit32u val_32)
{
  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMwrite32_Shadow: can't access encoding 0x%08x, offset=0x%x", encoding, offset));

  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.vmcs_linkptr + offset;
  access_write_physical(pAddr, 4, (Bit8u*)(&val_32));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(resolve_memtype(pAddr)), BX_WRITE, BX_SHADOW_VMCS_ACCESS, (Bit8u*)(&val_32));
}

Bit64u BX_CPP_AttrRegparmN(1) BX_CPU_C::VMread64_Shadow(unsigned encoding)
{
  BX_ASSERT(!IS_VMCS_FIELD_HI(encoding));

  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMread64_Shadow: can't access encoding 0x%08x, offset=0x%x", encoding, offset));

  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.vmcs_linkptr + offset;
  Bit64u field;
  access_read_physical(pAddr, 8, (Bit8u*)(&field));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_SHADOW_VMCS_ACCESS, (Bit8u*)(&field));

  return field;
}

// write 64-bit value into shadow VMCS field
void BX_CPP_AttrRegparmN(2) BX_CPU_C::VMwrite64_Shadow(unsigned encoding, Bit64u val_64)
{
  BX_ASSERT(!IS_VMCS_FIELD_HI(encoding));

  unsigned offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(encoding);
  if(offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("VMwrite64_Shadow: can't access encoding 0x%08x, offset=0x%x", encoding, offset));

  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.vmcs_linkptr + offset;
  access_write_physical(pAddr, 8, (Bit8u*)(&val_64));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(resolve_memtype(pAddr)), BX_WRITE, BX_SHADOW_VMCS_ACCESS, (Bit8u*)(&val_64));
}

#endif

////////////////////////////////////////////////////////////
// VMfail/VMsucceed
////////////////////////////////////////////////////////////

BX_CPP_INLINE void BX_CPU_C::VMfail(Bit32u error_code)
{
  clearEFlagsOSZAPC();

  if ((BX_CPU_THIS_PTR vmcsptr != BX_INVALID_VMCSPTR)) { // executed only if there is a current VMCS
     assert_ZF();
     VMwrite32(VMCS_32BIT_INSTRUCTION_ERROR, error_code);
  }
  else {
     assert_CF();
  }
}

void BX_CPU_C::VMabort(VMX_vmabort_code error_code)
{
  VMwrite32(VMCS_VMX_ABORT_FIELD_ENCODING, (Bit32u) error_code);

#if BX_SUPPORT_VMX >= 2
  // Deactivate VMX preemtion timer
  BX_CPU_THIS_PTR lapic.deactivate_vmx_preemption_timer();
#endif

  shutdown();
}

Bit32u BX_CPU_C::VMXReadRevisionID(bx_phy_address pAddr)
{
  unsigned revision_id_field_offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(VMCS_REVISION_ID_FIELD_ENCODING);
  if(revision_id_field_offset >= VMX_VMCS_AREA_SIZE)
    BX_PANIC(("Can't access VMCS_REVISION_ID encoding, offset=0x%x", revision_id_field_offset));

  Bit32u revision;
  access_read_physical(pAddr + revision_id_field_offset, 4, &revision);
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr + revision_id_field_offset, 4, MEMTYPE(BX_CPU_THIS_PTR vmcs_memtype),
          BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&revision));

  return revision;
}

#if BX_SUPPORT_VMX >= 2
bool BX_CPU_C::is_eptptr_valid(Bit64u eptptr)
{
  // [2:0] EPT paging-structure memory type
  //       0 = Uncacheable (UC)
  //       6 = Write-back (WB)
  Bit32u memtype = eptptr & 7;
  if (memtype != BX_MEMTYPE_UC && memtype != BX_MEMTYPE_WB) return 0;

  // [5:3] This value is 1 less than the EPT page-walk length
  Bit32u walk_length = (eptptr >> 3) & 7;
  if (walk_length != 3) return 0;

  // [6]   EPT A/D Enable
  if (! BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT_ACCESS_DIRTY)) {
    if (eptptr & 0x40) {
      BX_ERROR(("is_eptptr_valid: EPTPTR A/D enabled when not supported by CPU"));
      return 0;
    }
  }

  // [7]   CET: Enable supervisor shadow stack control
#if BX_SUPPORT_CET
  if (! BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
    if (eptptr & 0x80) {
      BX_ERROR(("is_eptptr_valid: EPTPTR CET supervisor shadow stack control bit enabled when not supported by CPU"));
      return 0;
    }
  }
#endif

#define BX_EPTPTR_RESERVED_BITS 0xf00 /* bits 11:8 are reserved */
  if (eptptr & BX_EPTPTR_RESERVED_BITS) {
    BX_ERROR(("is_eptptr_valid: EPTPTR reserved bits set"));
    return 0;
  }

  if (! IsValidPhyAddr(eptptr)) return 0;
  return 1;
}
#endif

BX_CPP_INLINE static Bit32u rotate_r(Bit32u val_32)
{
  return (val_32 >> 8) | (val_32 << 24);
}

BX_CPP_INLINE static Bit32u rotate_l(Bit32u val_32)
{
  return (val_32 << 8) | (val_32 >> 24);
}

// AR.NULL is bit 16
BX_CPP_INLINE static Bit32u vmx_pack_ar_field(Bit32u ar_field, VMCS_Access_Rights_Format access_rights_format)
{
  switch (access_rights_format) {
  case VMCS_AR_ROTATE:
    ar_field = rotate_l(ar_field);
    break;
  case VMCS_AR_PACK:
    // zero out bit 11
    ar_field &= 0xfffff7ff;
    // Null bit (bit 16) to be stored in bit 11
    ar_field |= ((ar_field & 0x00010000) >> 5);
    // zero out the upper 16 bits and b8-b10
    ar_field &= 0x0000f8ff;
    break;
  default:
    break;
  }

  return ar_field;
}

// AR.NULL is bit 16
BX_CPP_INLINE static Bit32u vmx_unpack_ar_field(Bit32u ar_field, VMCS_Access_Rights_Format access_rights_format)
{
  switch (access_rights_format) {
  case VMCS_AR_ROTATE:
    ar_field = rotate_r(ar_field);
    break;
  case VMCS_AR_PACK:
    // zero out bit 16
    ar_field &= 0xfffeffff;
    // Null bit to be copied back from bit 11 to bit 16
    ar_field |= ((ar_field & 0x00000800) << 5);
    // zero out the bit 17 to bit 31
    ar_field &= 0x0001ffff;
    // bits 8 to 11 should be set to 0
    ar_field &= 0xfffff0ff;
    break;
  default:
    break;
  }

  return ar_field;
}

////////////////////////////////////////////////////////////
// VMenter
////////////////////////////////////////////////////////////

extern struct BxExceptionInfo exceptions_info[];

#define VMENTRY_INJECTING_EVENT(vmentry_interr_info) (vmentry_interr_info & 0x80000000)

#define VMX_CHECKS_USE_MSR_VMX_PINBASED_CTRLS_LO \
  ((BX_SUPPORT_VMX >= 2) ? VMX_MSR_VMX_TRUE_PINBASED_CTRLS_LO : VMX_MSR_VMX_PINBASED_CTRLS_LO)
#define VMX_CHECKS_USE_MSR_VMX_PINBASED_CTRLS_HI \
  ((BX_SUPPORT_VMX >= 2) ? VMX_MSR_VMX_TRUE_PINBASED_CTRLS_HI : VMX_MSR_VMX_PINBASED_CTRLS_HI)

#define VMX_CHECKS_USE_MSR_VMX_PROCBASED_CTRLS_LO \
  ((BX_SUPPORT_VMX >= 2) ? VMX_MSR_VMX_TRUE_PROCBASED_CTRLS_LO : VMX_MSR_VMX_PROCBASED_CTRLS_LO)
#define VMX_CHECKS_USE_MSR_VMX_PROCBASED_CTRLS_HI \
  ((BX_SUPPORT_VMX >= 2) ? VMX_MSR_VMX_TRUE_PROCBASED_CTRLS_HI : VMX_MSR_VMX_PROCBASED_CTRLS_HI)

#define VMX_CHECKS_USE_MSR_VMX_VMEXIT_CTRLS_LO \
  ((BX_SUPPORT_VMX >= 2) ? VMX_MSR_VMX_TRUE_VMEXIT_CTRLS_LO : VMX_MSR_VMX_VMEXIT_CTRLS_LO)
#define VMX_CHECKS_USE_MSR_VMX_VMEXIT_CTRLS_HI \
  ((BX_SUPPORT_VMX >= 2) ? VMX_MSR_VMX_TRUE_VMEXIT_CTRLS_HI : VMX_MSR_VMX_VMEXIT_CTRLS_HI)

#define VMX_CHECKS_USE_MSR_VMX_VMENTRY_CTRLS_LO \
  ((BX_SUPPORT_VMX >= 2) ? VMX_MSR_VMX_TRUE_VMENTRY_CTRLS_LO : VMX_MSR_VMX_VMENTRY_CTRLS_LO)
#define VMX_CHECKS_USE_MSR_VMX_VMENTRY_CTRLS_HI \
  ((BX_SUPPORT_VMX >= 2) ? VMX_MSR_VMX_TRUE_VMENTRY_CTRLS_HI : VMX_MSR_VMX_VMENTRY_CTRLS_HI)

VMX_error_code BX_CPU_C::VMenterLoadCheckVmControls(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  //
  // Load VM-execution control fields to VMCS Cache
  //

  vm->vmexec_ctrls1 = VMread32(VMCS_32BIT_CONTROL_PIN_BASED_EXEC_CONTROLS);
  vm->vmexec_ctrls2 = VMread32(VMCS_32BIT_CONTROL_PROCESSOR_BASED_VMEXEC_CONTROLS);
  if (VMEXIT(VMX_VM_EXEC_CTRL2_SECONDARY_CONTROLS))
    vm->vmexec_ctrls3 = VMread32(VMCS_32BIT_CONTROL_SECONDARY_VMEXEC_CONTROLS);
  else
    vm->vmexec_ctrls3 = 0;
  vm->vm_exceptions_bitmap = VMread32(VMCS_32BIT_CONTROL_EXECUTION_BITMAP);
  vm->vm_pf_mask = VMread32(VMCS_32BIT_CONTROL_PAGE_FAULT_ERR_CODE_MASK);
  vm->vm_pf_match = VMread32(VMCS_32BIT_CONTROL_PAGE_FAULT_ERR_CODE_MATCH);
  vm->vm_cr0_mask = VMread_natural(VMCS_CONTROL_CR0_GUEST_HOST_MASK);
  vm->vm_cr4_mask = VMread_natural(VMCS_CONTROL_CR4_GUEST_HOST_MASK);
  vm->vm_cr0_read_shadow = VMread_natural(VMCS_CONTROL_CR0_READ_SHADOW);
  vm->vm_cr4_read_shadow = VMread_natural(VMCS_CONTROL_CR4_READ_SHADOW);

  vm->vm_cr3_target_cnt = VMread32(VMCS_32BIT_CONTROL_CR3_TARGET_COUNT);
  for (int n=0; n<VMX_CR3_TARGET_MAX_CNT; n++)
    vm->vm_cr3_target_value[n] = VMread_natural(VMCS_CR3_TARGET0 + 2*n);

  //
  // Check VM-execution control fields
  //

  if (~vm->vmexec_ctrls1 & VMX_CHECKS_USE_MSR_VMX_PINBASED_CTRLS_LO) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX pin-based controls allowed 0-settings"));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }
  if (vm->vmexec_ctrls1 & ~VMX_CHECKS_USE_MSR_VMX_PINBASED_CTRLS_HI) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX pin-based controls allowed 1-settings [0x%08x]", vm->vmexec_ctrls1 & ~VMX_CHECKS_USE_MSR_VMX_PINBASED_CTRLS_HI));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }

  if (~vm->vmexec_ctrls2 & VMX_CHECKS_USE_MSR_VMX_PROCBASED_CTRLS_LO) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX proc-based controls allowed 0-settings"));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }
  if (vm->vmexec_ctrls2 & ~VMX_CHECKS_USE_MSR_VMX_PROCBASED_CTRLS_HI) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX proc-based controls allowed 1-settings [0x%08x]", vm->vmexec_ctrls2 & ~VMX_CHECKS_USE_MSR_VMX_PROCBASED_CTRLS_HI));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }

  if (~vm->vmexec_ctrls3 & VMX_MSR_VMX_PROCBASED_CTRLS2_LO) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX secondary proc-based controls allowed 0-settings"));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }
  if (vm->vmexec_ctrls3 & ~VMX_MSR_VMX_PROCBASED_CTRLS2_HI) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX secondary controls allowed 1-settings [0x%08x]", vm->vmexec_ctrls3 & ~VMX_MSR_VMX_PROCBASED_CTRLS2_HI));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }

  if (vm->vm_cr3_target_cnt > VMX_CR3_TARGET_MAX_CNT) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: too may CR3 targets %d", vm->vm_cr3_target_cnt));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }

  if (vm->vmexec_ctrls2 & VMX_VM_EXEC_CTRL2_IO_BITMAPS) {
     vm->io_bitmap_addr[0] = VMread64(VMCS_64BIT_CONTROL_IO_BITMAP_A);
     vm->io_bitmap_addr[1] = VMread64(VMCS_64BIT_CONTROL_IO_BITMAP_B);
     // I/O bitmaps control enabled
     for (int bitmap=0; bitmap < 2; bitmap++) {
       if (! IsValidPageAlignedPhyAddr(vm->io_bitmap_addr[bitmap])) {
         BX_ERROR(("VMFAIL: VMCS EXEC CTRL: I/O bitmap %c phy addr malformed", 'A' + bitmap));
         return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
       }
     }
  }

  if (vm->vmexec_ctrls2 & VMX_VM_EXEC_CTRL2_MSR_BITMAPS) {
     // MSR bitmaps control enabled
     vm->msr_bitmap_addr = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_MSR_BITMAPS);
     if (! IsValidPageAlignedPhyAddr(vm->msr_bitmap_addr)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: MSR bitmap phy addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  if (! (vm->vmexec_ctrls1 & VMX_VM_EXEC_CTRL1_NMI_EXITING)) {
     if (vm->vmexec_ctrls1 & VMX_VM_EXEC_CTRL1_VIRTUAL_NMI) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: misconfigured virtual NMI control"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  if (! (vm->vmexec_ctrls1 & VMX_VM_EXEC_CTRL1_VIRTUAL_NMI)) {
     if (vm->vmexec_ctrls2 & VMX_VM_EXEC_CTRL2_NMI_WINDOW_EXITING) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: misconfigured virtual NMI control"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

#if BX_SUPPORT_VMX >= 2
  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VMCS_SHADOWING) {
     vm->vmread_bitmap_addr = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_VMREAD_BITMAP_ADDR);
     if (! IsValidPageAlignedPhyAddr(vm->vmread_bitmap_addr)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMREAD bitmap phy addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
     vm->vmwrite_bitmap_addr = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_VMWRITE_BITMAP_ADDR);
     if (! IsValidPageAlignedPhyAddr(vm->vmwrite_bitmap_addr)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMWRITE bitmap phy addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_VIOLATION_EXCEPTION) {
     vm->ve_info_addr = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_VE_EXCEPTION_INFO_ADDR);
     if (! IsValidPageAlignedPhyAddr(vm->ve_info_addr)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: broken #VE information address"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }
#endif

#if BX_SUPPORT_X86_64
  if (vm->vmexec_ctrls2 & VMX_VM_EXEC_CTRL2_TPR_SHADOW) {
     vm->virtual_apic_page_addr = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_VIRTUAL_APIC_PAGE_ADDR);
     if (! IsValidPageAlignedPhyAddr(vm->virtual_apic_page_addr)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: virtual apic phy addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }

#if BX_SUPPORT_VMX >= 2
     if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY) {
       if (! PIN_VMEXIT(VMX_VM_EXEC_CTRL1_EXTERNAL_INTERRUPT_VMEXIT)) {
         BX_ERROR(("VMFAIL: VMCS EXEC CTRL: virtual interrupt delivery must be set together with external interrupt exiting"));
         return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
       }

       for (int reg = 0; reg < 8; reg++) {
         vm->eoi_exit_bitmap[reg] = VMread32(VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP0 + reg);
       }

       Bit16u guest_interrupt_status = VMread16(VMCS_16BIT_GUEST_INTERRUPT_STATUS);
       vm->rvi = guest_interrupt_status & 0xff;
       vm->svi = guest_interrupt_status >> 8;
     }
     else
#endif
     {
       vm->vm_tpr_threshold = VMread32(VMCS_32BIT_CONTROL_TPR_THRESHOLD);

       if (vm->vm_tpr_threshold & 0xfffffff0) {
         BX_ERROR(("VMFAIL: VMCS EXEC CTRL: TPR threshold too big"));
         return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
       }

       if (! (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES)) {
         Bit8u tpr_shadow = (VMX_Read_Virtual_APIC(BX_LAPIC_TPR) >> 4) & 0xf;
         if (vm->vm_tpr_threshold > tpr_shadow) {
           BX_ERROR(("VMFAIL: VMCS EXEC CTRL: TPR threshold > TPR shadow"));
           return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
         }
       }
     }
  }
#if BX_SUPPORT_VMX >= 2
  else { // TPR shadow is disabled
     if (vm->vmexec_ctrls3 & (VMX_VM_EXEC_CTRL3_VIRTUALIZE_X2APIC_MODE |
                              VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_REGISTERS |
                              VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY))
     {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: apic virtualization is enabled without TPR shadow"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }
#endif // BX_SUPPORT_VMX >= 2

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES) {
     vm->apic_access_page = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_APIC_ACCESS_ADDR);
     if (! IsValidPageAlignedPhyAddr(vm->apic_access_page)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: apic access page phy addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }

#if BX_SUPPORT_VMX >= 2
     if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VIRTUALIZE_X2APIC_MODE) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: virtualize X2APIC mode enabled together with APIC access virtualization"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
#endif
  }

#if BX_SUPPORT_VMX >= 2
  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_ENABLE) {
     vm->eptptr = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_EPTPTR);
     if (! is_eptptr_valid(vm->eptptr)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: invalid EPTPTR value"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }
  else {
     if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_UNRESTRICTED_GUEST) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: unrestricted guest without EPT"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VPID_ENABLE) {
     vm->vpid = VMread16(VMCS_16BIT_CONTROL_VPID);
     if (vm->vpid == 0) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: guest VPID == 0"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_PAUSE_LOOP_VMEXIT) {
     vm->ple.pause_loop_exiting_gap = VMread32(VMCS_32BIT_CONTROL_PAUSE_LOOP_EXITING_GAP);
     vm->ple.pause_loop_exiting_window = VMread32(VMCS_32BIT_CONTROL_PAUSE_LOOP_EXITING_WINDOW);
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VMFUNC_ENABLE)
    vm->vmfunc_ctrls = VMread64(VMCS_64BIT_CONTROL_VMFUNC_CTRLS);
  else
    vm->vmfunc_ctrls = 0;

  if (vm->vmfunc_ctrls & ~VMX_VMFUNC_CTRL1_SUPPORTED_BITS) {
     BX_ERROR(("VMFAIL: VMCS VM Functions control reserved bits set"));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }

  if (vm->vmfunc_ctrls & VMX_VMFUNC_EPTP_SWITCHING_MASK) {
     if ((vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_ENABLE) == 0) {
       BX_ERROR(("VMFAIL: VMFUNC EPTP-SWITCHING: EPT disabled"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }

     vm->eptp_list_address = VMread64(VMCS_64BIT_CONTROL_EPTP_LIST_ADDRESS);
     if (! IsValidPageAlignedPhyAddr(vm->eptp_list_address)) {
       BX_ERROR(("VMFAIL: VMFUNC EPTP-SWITCHING: eptp list phy addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_PML_ENABLE) {
    if ((vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_ENABLE) == 0) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: PML is enabled without EPT"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
    }

    vm->pml_address = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_PML_ADDRESS);
    if (! IsValidPageAlignedPhyAddr(vm->pml_address)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: PML base phy addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
    }
    vm->pml_index = VMread16(VMCS_16BIT_GUEST_PML_INDEX);
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_SUBPAGE_WR_PROTECT_CTRL) {
    if ((vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_ENABLE) == 0) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: SPP is enabled without EPT"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
    }

    vm->spptp = (bx_phy_address) VMread64(VMCS_64BIT_CONTROL_SPPTP);
    if (! IsValidPageAlignedPhyAddr(vm->spptp)) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: SPP base phy addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
    }
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_MBE_CTRL) {
    if ((vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_ENABLE) == 0) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: MBE is enabled without EPT"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
    }
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_XSAVES_XRSTORS)
     vm->xss_exiting_bitmap = VMread64(VMCS_64BIT_CONTROL_XSS_EXITING_BITMAP);
  else
     vm->xss_exiting_bitmap = 0;
#endif

#endif // BX_SUPPORT_X86_64

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_TSC_SCALING) {
     if ((vm->tsc_multiplier = VMread64(VMCS_64BIT_CONTROL_TSC_MULTIPLIER)) == 0) {
       BX_ERROR(("VMFAIL: VMCS EXEC CTRL: TSC multiplier should be non zero"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  //
  // Load VM-exit control fields to VMCS Cache
  //

  vm->vmexit_ctrls = VMread32(VMCS_32BIT_CONTROL_VMEXIT_CONTROLS);
  vm->vmexit_msr_store_cnt = VMread32(VMCS_32BIT_CONTROL_VMEXIT_MSR_STORE_COUNT);
  vm->vmexit_msr_load_cnt = VMread32(VMCS_32BIT_CONTROL_VMEXIT_MSR_LOAD_COUNT);

  //
  // Check VM-exit control fields
  //

  if (~vm->vmexit_ctrls & VMX_CHECKS_USE_MSR_VMX_VMEXIT_CTRLS_LO) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX vmexit controls allowed 0-settings"));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }
  if (vm->vmexit_ctrls & ~VMX_CHECKS_USE_MSR_VMX_VMEXIT_CTRLS_HI) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX vmexit controls allowed 1-settings [0x%08x]", vm->vmexit_ctrls & ~VMX_CHECKS_USE_MSR_VMX_VMEXIT_CTRLS_HI));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }

#if BX_SUPPORT_VMX >= 2
  if ((~vm->vmexec_ctrls1 & VMX_VM_EXEC_CTRL1_VMX_PREEMPTION_TIMER_VMEXIT) && (vm->vmexit_ctrls & VMX_VMEXIT_CTRL1_STORE_VMX_PREEMPTION_TIMER)) {
     BX_ERROR(("VMFAIL: save_VMX_preemption_timer VMEXIT control is set but VMX_preemption_timer VMEXEC control is clear"));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }
#endif

  if (vm->vmexit_msr_store_cnt > 0) {
     vm->vmexit_msr_store_addr = VMread64(VMCS_64BIT_CONTROL_VMEXIT_MSR_STORE_ADDR);
     if ((vm->vmexit_msr_store_addr & 0xf) != 0 || ! IsValidPhyAddr(vm->vmexit_msr_store_addr)) {
       BX_ERROR(("VMFAIL: VMCS VMEXIT CTRL: msr store addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }

     Bit64u last_byte = vm->vmexit_msr_store_addr + (vm->vmexit_msr_store_cnt * 16) - 1;
     if (! IsValidPhyAddr(last_byte)) {
       BX_ERROR(("VMFAIL: VMCS VMEXIT CTRL: msr store addr too high"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  if (vm->vmexit_msr_load_cnt > 0) {
     vm->vmexit_msr_load_addr = VMread64(VMCS_64BIT_CONTROL_VMEXIT_MSR_LOAD_ADDR);
     if ((vm->vmexit_msr_load_addr & 0xf) != 0 || ! IsValidPhyAddr(vm->vmexit_msr_load_addr)) {
       BX_ERROR(("VMFAIL: VMCS VMEXIT CTRL: msr load addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }

     Bit64u last_byte = (Bit64u) vm->vmexit_msr_load_addr + (vm->vmexit_msr_load_cnt * 16) - 1;
     if (! IsValidPhyAddr(last_byte)) {
       BX_ERROR(("VMFAIL: VMCS VMEXIT CTRL: msr load addr too high"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  //
  // Load VM-entry control fields to VMCS Cache
  //

  vm->vmentry_ctrls = VMread32(VMCS_32BIT_CONTROL_VMENTRY_CONTROLS);
  vm->vmentry_msr_load_cnt = VMread32(VMCS_32BIT_CONTROL_VMENTRY_MSR_LOAD_COUNT);

  //
  // Check VM-entry control fields
  //

  if (~vm->vmentry_ctrls & VMX_CHECKS_USE_MSR_VMX_VMENTRY_CTRLS_LO) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX vmentry controls allowed 0-settings"));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }
  if (vm->vmentry_ctrls & ~VMX_CHECKS_USE_MSR_VMX_VMENTRY_CTRLS_HI) {
     BX_ERROR(("VMFAIL: VMCS EXEC CTRL: VMX vmentry controls allowed 1-settings [0x%08x]", vm->vmentry_ctrls & ~VMX_CHECKS_USE_MSR_VMX_VMENTRY_CTRLS_HI));
     return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
  }

  if (vm->vmentry_ctrls & VMX_VMENTRY_CTRL1_DEACTIVATE_DUAL_MONITOR_TREATMENT) {
     if (! BX_CPU_THIS_PTR in_smm) {
       BX_ERROR(("VMFAIL: VMENTRY from outside SMM with dual-monitor treatment enabled"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  if (vm->vmentry_msr_load_cnt > 0) {
     vm->vmentry_msr_load_addr = VMread64(VMCS_64BIT_CONTROL_VMENTRY_MSR_LOAD_ADDR);
     if ((vm->vmentry_msr_load_addr & 0xf) != 0 || ! IsValidPhyAddr(vm->vmentry_msr_load_addr)) {
       BX_ERROR(("VMFAIL: VMCS VMENTRY CTRL: msr load addr malformed"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }

     Bit64u last_byte = vm->vmentry_msr_load_addr + (vm->vmentry_msr_load_cnt * 16) - 1;
     if (! IsValidPhyAddr(last_byte)) {
       BX_ERROR(("VMFAIL: VMCS VMENTRY CTRL: msr load addr too high"));
       return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }
  }

  //
  // Check VM-entry event injection info
  //

  vm->vmentry_interr_info = VMread32(VMCS_32BIT_CONTROL_VMENTRY_INTERRUPTION_INFO);
  vm->vmentry_excep_err_code = VMread32(VMCS_32BIT_CONTROL_VMENTRY_EXCEPTION_ERR_CODE);
  vm->vmentry_instr_length = VMread32(VMCS_32BIT_CONTROL_VMENTRY_INSTRUCTION_LENGTH);

  if (VMENTRY_INJECTING_EVENT(vm->vmentry_interr_info)) {

     /* the VMENTRY injecting event to the guest */
     unsigned vector = vm->vmentry_interr_info & 0xff;
     unsigned event_type = (vm->vmentry_interr_info >>  8) & 7;
     unsigned push_error = (vm->vmentry_interr_info >> 11) & 1;
     unsigned error_code = push_error ? vm->vmentry_excep_err_code : 0;

     unsigned push_error_reference = 0;
     if (event_type == BX_HARDWARE_EXCEPTION && vector < BX_CPU_HANDLED_EXCEPTIONS)
        push_error_reference = exceptions_info[vector].push_error;
#if BX_SUPPORT_CET
     if (! BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
        if (vector == BX_CP_EXCEPTION) push_error_reference = false;
     }
#endif

     if (vm->vmentry_interr_info & 0x7ffff000) {
        BX_ERROR(("VMFAIL: VMENTRY broken interruption info field"));
        return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }

     switch (event_type) {
       case BX_EXTERNAL_INTERRUPT:
         break;

       case BX_NMI:
         if (vector != 2) {
           BX_ERROR(("VMFAIL: VMENTRY bad injected event vector %d", vector));
           return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
         }
/*
         // injecting NMI
         if (vm->vmexec_ctrls1 & VMX_VM_EXEC_CTRL1_VIRTUAL_NMI) {
           if (guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_NMI_BLOCKED) {
             BX_ERROR(("VMFAIL: VMENTRY injected NMI vector when blocked by NMI in interruptibility state", vector));
             return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
           }
         }
*/
         break;

       case BX_HARDWARE_EXCEPTION:
         if (vector > 31) {
           BX_ERROR(("VMFAIL: VMENTRY bad injected event vector %d", vector));
           return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
         }
         break;

       case BX_SOFTWARE_INTERRUPT:
       case BX_PRIVILEGED_SOFTWARE_INTERRUPT:
       case BX_SOFTWARE_EXCEPTION:
         if ((vm->vmentry_instr_length == 0 && !BX_SUPPORT_VMX_EXTENSION(BX_VMX_SW_INTERRUPT_INJECTION_ILEN_0)) ||
              vm->vmentry_instr_length > 15)
         {
           BX_ERROR(("VMFAIL: VMENTRY bad injected event instr length"));
           return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
         }
         break;

       case 7: /* MTF */
         if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_MONITOR_TRAP_FLAG)) {
           if (vector != 0) {
             BX_ERROR(("VMFAIL: VMENTRY bad MTF injection with vector=%d", vector));
             return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
           }
         }
         break;

       default:
         BX_ERROR(("VMFAIL: VMENTRY bad injected event type %d", event_type));
         return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
     }

#if BX_SUPPORT_VMX >= 2
     if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_UNRESTRICTED_GUEST) {
       unsigned protected_mode_guest = (Bit32u) VMread_natural(VMCS_GUEST_CR0) & BX_CR0_PE_MASK;
       if (! protected_mode_guest) push_error_reference = 0;
     }
#endif

     if (! BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
       // CET added new #CP exception with error code but legacy software assumed that this vector have no error code.
       // Therefore CET enabled processors do not check the error code anymore and able to deliver a hardware
       // exception with or without an error code, regardless of vector as indicated in VMX_MSR_VMX_BASIC[56]
       if (push_error != push_error_reference) {
         BX_ERROR(("VMFAIL: VMENTRY injected event vector %d broken error code", vector));
         return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
       }
     }

     if (push_error) {
       if (error_code & 0xffff0000) {
         BX_ERROR(("VMFAIL: VMENTRY bad error code 0x%08x for injected event %d", error_code, vector));
         return VMXERR_VMENTRY_INVALID_VM_CONTROL_FIELD;
       }
     }
  }

  return VMXERR_NO_ERROR;
}

VMX_error_code BX_CPU_C::VMenterLoadCheckHostState(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  VMCS_HOST_STATE *host_state = &vm->host_state;
  bool x86_64_host = false, x86_64_guest = false;

  //
  // VM Host State Checks Related to Address-Space Size
  //

  Bit32u vmexit_ctrls = vm->vmexit_ctrls;
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_HOST_ADDR_SPACE_SIZE) {
     x86_64_host = true;
  }
  Bit32u vmentry_ctrls = vm->vmentry_ctrls;
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_X86_64_GUEST) {
     x86_64_guest = true;
  }

#if BX_SUPPORT_X86_64
  if (long_mode()) {
     if (! x86_64_host) {
        BX_ERROR(("VMFAIL: VMCS x86-64 host control invalid on VMENTRY"));
        return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
     }
  }
  else
#endif
  {
     if (x86_64_host || x86_64_guest) {
        BX_ERROR(("VMFAIL: VMCS x86-64 guest(%d)/host(%d) controls invalid on VMENTRY", x86_64_guest, x86_64_host));
        return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
     }
  }

  //
  // Load and Check VM Host State to VMCS Cache
  //

  host_state->cr0 = (bx_address) VMread_natural(VMCS_HOST_CR0);
  if (~host_state->cr0 & VMX_MSR_CR0_FIXED0) {
     BX_ERROR(("VMFAIL: VMCS host state invalid CR0 0x%08x", (Bit32u) host_state->cr0));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

  if (host_state->cr0 & ~VMX_MSR_CR0_FIXED1) {
     BX_ERROR(("VMFAIL: VMCS host state invalid CR0 0x%08x", (Bit32u) host_state->cr0));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

  host_state->cr3 = (bx_address) VMread_natural(VMCS_HOST_CR3);
#if BX_SUPPORT_X86_64
  if (! IsValidPhyAddr(host_state->cr3)) {
     BX_ERROR(("VMFAIL: VMCS host state invalid CR3"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }
#endif

  host_state->cr4 = (bx_address) VMread_natural(VMCS_HOST_CR4);
  if (~host_state->cr4 & VMX_MSR_CR4_FIXED0) {
     BX_ERROR(("VMFAIL: VMCS host state invalid CR4 0x" FMT_ADDRX, host_state->cr4));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }
  if (host_state->cr4 & ~VMX_MSR_CR4_FIXED1) {
     BX_ERROR(("VMFAIL: VMCS host state invalid CR4 0x" FMT_ADDRX, host_state->cr4));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

  for(int n=0; n<6; n++) {
     host_state->segreg_selector[n] = VMread16(VMCS_16BIT_HOST_ES_SELECTOR + 2*n);
     if (host_state->segreg_selector[n] & 7) {
        BX_ERROR(("VMFAIL: VMCS host segreg %d TI/RPL != 0", n));
        return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
     }
  }

  if (host_state->segreg_selector[BX_SEG_REG_CS] == 0) {
     BX_ERROR(("VMFAIL: VMCS host CS selector 0"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

  if (! x86_64_host && host_state->segreg_selector[BX_SEG_REG_SS] == 0) {
     BX_ERROR(("VMFAIL: VMCS host SS selector 0"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

  host_state->tr_selector = VMread16(VMCS_16BIT_HOST_TR_SELECTOR);
  if (! host_state->tr_selector || (host_state->tr_selector & 7) != 0) {
     BX_ERROR(("VMFAIL: VMCS invalid host TR selector"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

  host_state->tr_base = (bx_address) VMread_natural(VMCS_HOST_TR_BASE);
#if BX_SUPPORT_X86_64
  if (! IsCanonical(host_state->tr_base)) {
     BX_ERROR(("VMFAIL: VMCS host TR BASE non canonical"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }
#endif

  host_state->fs_base = (bx_address) VMread_natural(VMCS_HOST_FS_BASE);
  host_state->gs_base = (bx_address) VMread_natural(VMCS_HOST_GS_BASE);
#if BX_SUPPORT_X86_64
  if (! IsCanonical(host_state->fs_base)) {
     BX_ERROR(("VMFAIL: VMCS host FS BASE non canonical"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }
  if (! IsCanonical(host_state->gs_base)) {
     BX_ERROR(("VMFAIL: VMCS host GS BASE non canonical"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }
#endif

  host_state->gdtr_base = (bx_address) VMread_natural(VMCS_HOST_GDTR_BASE);
  host_state->idtr_base = (bx_address) VMread_natural(VMCS_HOST_IDTR_BASE);
#if BX_SUPPORT_X86_64
  if (! IsCanonical(host_state->gdtr_base)) {
     BX_ERROR(("VMFAIL: VMCS host GDTR BASE non canonical"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }
  if (! IsCanonical(host_state->idtr_base)) {
     BX_ERROR(("VMFAIL: VMCS host IDTR BASE non canonical"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }
#endif

  host_state->sysenter_esp_msr = (bx_address) VMread_natural(VMCS_HOST_IA32_SYSENTER_ESP_MSR);
  host_state->sysenter_eip_msr = (bx_address) VMread_natural(VMCS_HOST_IA32_SYSENTER_EIP_MSR);
  host_state->sysenter_cs_msr = (Bit16u) VMread32(VMCS_32BIT_HOST_IA32_SYSENTER_CS_MSR);

#if BX_SUPPORT_X86_64
  if (! IsCanonical(host_state->sysenter_esp_msr)) {
     BX_ERROR(("VMFAIL: VMCS host SYSENTER_ESP_MSR non canonical"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

  if (! IsCanonical(host_state->sysenter_eip_msr)) {
     BX_ERROR(("VMFAIL: VMCS host SYSENTER_EIP_MSR non canonical"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }
#endif

#if BX_SUPPORT_VMX >= 2
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_LOAD_PAT_MSR) {
    host_state->pat_msr = VMread64(VMCS_64BIT_HOST_IA32_PAT);
    if (! isValidMSR_PAT(host_state->pat_msr)) {
      BX_ERROR(("VMFAIL: invalid Memory Type in host MSR_PAT"));
      return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }
  }
#endif

  host_state->rsp = (bx_address) VMread_natural(VMCS_HOST_RSP);
  host_state->rip = (bx_address) VMread_natural(VMCS_HOST_RIP);

#if BX_SUPPORT_CET
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_LOAD_HOST_CET_STATE) {
    host_state->msr_ia32_s_cet = VMread_natural(VMCS_HOST_IA32_S_CET);
    if (!IsCanonical(host_state->msr_ia32_s_cet) || (!x86_64_host && GET32H(host_state->msr_ia32_s_cet))) {
       BX_ERROR(("VMFAIL: VMCS host IA32_S_CET/EB_LEG_BITMAP_BASE non canonical or invalid"));
       return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }

    if (is_invalid_cet_control(host_state->msr_ia32_s_cet)) {
       BX_ERROR(("VMFAIL: VMCS host IA32_S_CET invalid"));
       return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }

    host_state->ssp = VMread_natural(VMCS_HOST_SSP);
    if (!IsCanonical(host_state->ssp) || (!x86_64_host && GET32H(host_state->ssp))) {
       BX_ERROR(("VMFAIL: VMCS host SSP non canonical or invalid"));
       return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }
    if ((host_state->ssp & 0x3) != 0) {
       BX_ERROR(("VMFAIL: VMCS host SSP[1:0] not zero"));
       return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }

    host_state->interrupt_ssp_table_address = VMread_natural(VMCS_HOST_INTERRUPT_SSP_TABLE_ADDR);
    if (!IsCanonical(host_state->interrupt_ssp_table_address)) {
       BX_ERROR(("VMFAIL: VMCS host INTERRUPT_SSP_TABLE_ADDR non canonical or invalid"));
       return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }

    if ((host_state->cr4 & BX_CR4_CET_MASK) && (host_state->cr0 & BX_CR0_WP_MASK) == 0) {
      BX_ERROR(("FAIL: VMCS host CR4.CET=1 when CR0.WP=0"));
      return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }
  }
#endif

#if BX_SUPPORT_PKEYS
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_LOAD_HOST_PKRS) {
    host_state->pkrs = VMread64(VMCS_64BIT_HOST_IA32_PKRS);
    if (GET32H(host_state->pkrs) != 0) {
      BX_ERROR(("VMFAIL: invalid host IA32_PKRS value"));
      return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }
  }
#endif

#if BX_SUPPORT_X86_64

#if BX_SUPPORT_VMX >= 2
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_LOAD_EFER_MSR) {
    host_state->efer_msr = VMread64(VMCS_64BIT_HOST_IA32_EFER);
    if (host_state->efer_msr & ~((Bit64u) BX_CPU_THIS_PTR efer_suppmask)) {
      BX_ERROR(("VMFAIL: VMCS host EFER reserved bits set !"));
      return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }
    bool lme = (host_state->efer_msr >>  8) & 0x1;
    bool lma = (host_state->efer_msr >> 10) & 0x1;
    if (lma != lme || lma != x86_64_host) {
      BX_ERROR(("VMFAIL: VMCS host EFER (0x%08x) inconsistent value !", (Bit32u) host_state->efer_msr));
      return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }
  }
#endif

  if (x86_64_host) {
     if ((host_state->cr4 & BX_CR4_PAE_MASK) == 0) {
        BX_ERROR(("VMFAIL: VMCS host CR4.PAE=0 with x86-64 host"));
        return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
     }
     if (! IsCanonical(host_state->rip)) {
        BX_ERROR(("VMFAIL: VMCS host RIP non-canonical"));
        return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
     }
  }
  else {
     if (GET32H(host_state->rip) != 0) {
        BX_ERROR(("VMFAIL: VMCS host RIP > 32 bit"));
        return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
     }
     if (host_state->cr4 & BX_CR4_PCIDE_MASK) {
        BX_ERROR(("VMFAIL: VMCS host CR4.PCIDE set"));
        return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
     }
  }
#endif

  return VMXERR_NO_ERROR;
}

BX_CPP_INLINE bool IsLimitAccessRightsConsistent(Bit32u limit, Bit32u ar)
{
  bool g = (ar >> 15) & 1;

  // access rights reserved bits set
  if (ar & 0xfffe0f00) return 0;

  if (g) {
    // if any of the bits in limit[11:00] are '0 <=> G must be '0
    if ((limit & 0xfff) != 0xfff)
       return 0;
  }
  else {
    // if any of the bits in limit[31:20] are '1 <=> G must be '1
    if ((limit & 0xfff00000) != 0)
       return 0;
  }

  return 1;
}

Bit32u BX_CPU_C::VMenterLoadCheckGuestState(Bit64u *qualification)
{
  int n;

  VMCS_GUEST_STATE guest;
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  *qualification = VMENTER_ERR_NO_ERROR;

  //
  // Load and Check Guest State from VMCS
  //

  guest.rflags = VMread_natural(VMCS_GUEST_RFLAGS);
  // RFLAGS reserved bits [63:22], bit 15, bit 5, bit 3 must be zero
  if (guest.rflags & BX_CONST64(0xFFFFFFFFFFC08028)) {
     BX_ERROR(("VMENTER FAIL: RFLAGS reserved bits are set"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
  // RFLAGS[1] must be always set
  if ((guest.rflags & 0x2) == 0) {
     BX_ERROR(("VMENTER FAIL: RFLAGS[1] cleared"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  bool v8086_guest = false;
  if (guest.rflags & EFlagsVMMask)
     v8086_guest = true;

  bool x86_64_guest = false; // can't be 1 if X86_64 is not supported (checked before)
  Bit32u vmentry_ctrls = vm->vmentry_ctrls;
#if BX_SUPPORT_X86_64
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_X86_64_GUEST) {
     BX_DEBUG(("VMENTER to x86-64 guest"));
     x86_64_guest = true;
  }
#endif

  if (x86_64_guest && v8086_guest) {
     BX_ERROR(("VMENTER FAIL: Enter to x86-64 guest with RFLAGS.VM"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  guest.cr0 = VMread_natural(VMCS_GUEST_CR0);

#if BX_SUPPORT_VMX >= 2
  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_UNRESTRICTED_GUEST) {
     if (~guest.cr0 & (VMX_MSR_CR0_FIXED0 & ~(BX_CR0_PE_MASK | BX_CR0_PG_MASK))) {
        BX_ERROR(("VMENTER FAIL: VMCS guest invalid CR0"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }

     bool pe = (guest.cr0 & BX_CR0_PE_MASK) != 0;
     bool pg = (guest.cr0 & BX_CR0_PG_MASK) != 0;
     if (pg && !pe) {
        BX_ERROR(("VMENTER FAIL: VMCS unrestricted guest CR0.PG without CR0.PE"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
  }
  else
#endif
  {
     if (~guest.cr0 & VMX_MSR_CR0_FIXED0) {
        BX_ERROR(("VMENTER FAIL: VMCS guest invalid CR0"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
  }

  if (guest.cr0 & ~VMX_MSR_CR0_FIXED1) {
     BX_ERROR(("VMENTER FAIL: VMCS guest invalid CR0"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

#if BX_SUPPORT_VMX >= 2
  bool real_mode_guest = false;
  if (! (guest.cr0 & BX_CR0_PE_MASK))
     real_mode_guest = true;
#endif

  guest.cr3 = VMread_natural(VMCS_GUEST_CR3);
#if BX_SUPPORT_X86_64
  if (! IsValidPhyAddr(guest.cr3)) {
     BX_ERROR(("VMENTER FAIL: VMCS guest invalid CR3"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
#endif

  guest.cr4 = VMread_natural(VMCS_GUEST_CR4);
  if (~guest.cr4 & VMX_MSR_CR4_FIXED0) {
     BX_ERROR(("VMENTER FAIL: VMCS guest invalid CR4"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

  if (guest.cr4 & ~VMX_MSR_CR4_FIXED1) {
     BX_ERROR(("VMENTER FAIL: VMCS guest invalid CR4"));
     return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
  }

#if BX_SUPPORT_X86_64
  if (x86_64_guest) {
     if ((guest.cr4 & BX_CR4_PAE_MASK) == 0) {
        BX_ERROR(("VMENTER FAIL: VMCS guest CR4.PAE=0 in x86-64 mode"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
  }
  else {
     if (guest.cr4 & BX_CR4_PCIDE_MASK) {
        BX_ERROR(("VMENTER FAIL: VMCS CR4.PCIDE set in 32-bit guest"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
  }

  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_DBG_CTRLS) {
     guest.dr7 = VMread_natural(VMCS_GUEST_DR7);
     if (GET32H(guest.dr7)) {
        BX_ERROR(("VMENTER FAIL: VMCS guest invalid DR7"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
  }
#endif

#if BX_SUPPORT_CET
  if ((guest.cr4 & BX_CR4_CET_MASK) && (guest.cr0 & BX_CR0_WP_MASK) == 0) {
    BX_ERROR(("VMENTER FAIL: VMCS guest CR4.CET=1 when CR0.WP=0"));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_GUEST_CET_STATE) {
    guest.msr_ia32_s_cet = VMread_natural(VMCS_GUEST_IA32_S_CET);
    if (!IsCanonical(guest.msr_ia32_s_cet) || (!x86_64_guest && GET32H(guest.msr_ia32_s_cet))) {
       BX_ERROR(("VMFAIL: VMCS guest IA32_S_CET/EB_LEG_BITMAP_BASE non canonical or invalid"));
       return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }

    if (is_invalid_cet_control(guest.msr_ia32_s_cet)) {
       BX_ERROR(("VMFAIL: VMCS guest IA32_S_CET invalid"));
       return VMXERR_VMENTRY_INVALID_VM_HOST_STATE_FIELD;
    }

    guest.ssp = VMread_natural(VMCS_GUEST_SSP);
    if (!IsCanonical(guest.ssp) || (!x86_64_guest && GET32H(guest.ssp))) {
       BX_ERROR(("VMFAIL: VMCS guest SSP non canonical or invalid"));
       return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
    if ((guest.ssp & 0x3) != 0) {
       BX_ERROR(("VMFAIL: VMCS guest SSP[1:0] not zero"));
       return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }

    guest.interrupt_ssp_table_address = VMread_natural(VMCS_GUEST_INTERRUPT_SSP_TABLE_ADDR);
    if (!IsCanonical(guest.interrupt_ssp_table_address)) {
       BX_ERROR(("VMFAIL: VMCS guest INTERRUPT_SSP_TABLE_ADDR non canonical or invalid"));
       return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }
#endif

#if BX_SUPPORT_PKEYS
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_GUEST_PKRS) {
    guest.pkrs = VMread64(VMCS_64BIT_GUEST_IA32_PKRS);
    if (GET32H(guest.pkrs) != 0) {
      BX_ERROR(("VMFAIL: invalid guest IA32_PKRS value"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }
#endif

  //
  // Load and Check Guest State from VMCS - Segment Registers
  //

  for (n=0; n<6; n++) {
     Bit16u selector = VMread16(VMCS_16BIT_GUEST_ES_SELECTOR + 2*n);
     bx_address base = (bx_address) VMread_natural(VMCS_GUEST_ES_BASE + 2*n);
     Bit32u limit = VMread32(VMCS_32BIT_GUEST_ES_LIMIT + 2*n);
     Bit32u ar = VMread32(VMCS_32BIT_GUEST_ES_ACCESS_RIGHTS + 2*n);
     ar = vmx_unpack_ar_field(ar, BX_CPU_THIS_PTR vmcs_map->get_access_rights_format());
     bool invalid = (ar >> 16) & 1;

     set_segment_ar_data(&guest.sregs[n], !invalid,
                  (Bit16u) selector, base, limit, (Bit16u) ar);

     if (v8086_guest) {
        // guest in V8086 mode
        if (base != ((bx_address)(selector << 4))) {
          BX_ERROR(("VMENTER FAIL: VMCS v8086 guest bad %s.BASE", segname[n]));
          return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }
        if (limit != 0xffff) {
          BX_ERROR(("VMENTER FAIL: VMCS v8086 guest %s.LIMIT != 0xFFFF", segname[n]));
          return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }
        // present, expand-up read/write accessed, segment, DPL=3
        if (ar != 0xF3) {
          BX_ERROR(("VMENTER FAIL: VMCS v8086 guest %s.AR != 0xF3", segname[n]));
          return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }

        continue; // go to next segment register
     }

#if BX_SUPPORT_X86_64
     if (n >= BX_SEG_REG_FS) {
        if (! IsCanonical(base)) {
          BX_ERROR(("VMENTER FAIL: VMCS guest %s.BASE non canonical", segname[n]));
          return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }
     }
#endif

     if (n != BX_SEG_REG_CS && invalid)
        continue;

#if BX_SUPPORT_X86_64
     if (n == BX_SEG_REG_SS && (selector & BX_SELECTOR_RPL_MASK) == 0) {
        // SS is allowed to be NULL selector if going to 64-bit guest
        if (x86_64_guest && guest.sregs[BX_SEG_REG_CS].cache.u.segment.l)
           continue;
     }

     if (n < BX_SEG_REG_FS) {
        if (GET32H(base) != 0) {
          BX_ERROR(("VMENTER FAIL: VMCS guest %s.BASE > 32 bit", segname[n]));
          return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }
     }
#endif

     if (! guest.sregs[n].cache.segment) {
        BX_ERROR(("VMENTER FAIL: VMCS guest %s not segment", segname[n]));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }

     if (! guest.sregs[n].cache.p) {
        BX_ERROR(("VMENTER FAIL: VMCS guest %s not present", segname[n]));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }

     if (! IsLimitAccessRightsConsistent(limit, ar)) {
        BX_ERROR(("VMENTER FAIL: VMCS guest %s.AR/LIMIT malformed", segname[n]));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }

     if (n == BX_SEG_REG_CS) {
        // CS checks
        switch (guest.sregs[BX_SEG_REG_CS].cache.type) {
          case BX_CODE_EXEC_ONLY_ACCESSED:
          case BX_CODE_EXEC_READ_ACCESSED:
             // non-conforming segment
             if (guest.sregs[BX_SEG_REG_CS].selector.rpl != guest.sregs[BX_SEG_REG_CS].cache.dpl) {
               BX_ERROR(("VMENTER FAIL: VMCS guest non-conforming CS.RPL <> CS.DPL"));
               return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
             }
             break;
          case BX_CODE_EXEC_ONLY_CONFORMING_ACCESSED:
          case BX_CODE_EXEC_READ_CONFORMING_ACCESSED:
             // conforming segment
             if (guest.sregs[BX_SEG_REG_CS].selector.rpl < guest.sregs[BX_SEG_REG_CS].cache.dpl) {
               BX_ERROR(("VMENTER FAIL: VMCS guest non-conforming CS.RPL < CS.DPL"));
               return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
             }
             break;
#if BX_SUPPORT_VMX >= 2
          case BX_DATA_READ_WRITE_ACCESSED:
             if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_UNRESTRICTED_GUEST) {
               if (guest.sregs[BX_SEG_REG_CS].cache.dpl != 0) {
                 BX_ERROR(("VMENTER FAIL: VMCS unrestricted guest CS.DPL != 0"));
                 return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
               }
               break;
             }
             // fall through
#endif
          default:
             BX_ERROR(("VMENTER FAIL: VMCS guest CS.TYPE"));
             return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }

#if BX_SUPPORT_X86_64
        if (x86_64_guest) {
          if (guest.sregs[BX_SEG_REG_CS].cache.u.segment.d_b && guest.sregs[BX_SEG_REG_CS].cache.u.segment.l) {
             BX_ERROR(("VMENTER FAIL: VMCS x86_64 guest wrong CS.D_B/L"));
             return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
          }
        }
#endif
     }
     else if (n == BX_SEG_REG_SS) {
        // SS checks
        switch (guest.sregs[BX_SEG_REG_SS].cache.type) {
          case BX_DATA_READ_WRITE_ACCESSED:
          case BX_DATA_READ_WRITE_EXPAND_DOWN_ACCESSED:
             break;
          default:
             BX_ERROR(("VMENTER FAIL: VMCS guest SS.TYPE"));
             return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }
     }
     else {
        // DS, ES, FS, GS
        if ((guest.sregs[n].cache.type & 0x1) == 0) {
           BX_ERROR(("VMENTER FAIL: VMCS guest %s not ACCESSED", segname[n]));
           return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }

        if (guest.sregs[n].cache.type & 0x8) {
           if ((guest.sregs[n].cache.type & 0x2) == 0) {
              BX_ERROR(("VMENTER FAIL: VMCS guest CODE segment %s not READABLE", segname[n]));
              return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
           }
        }

        if (! (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_UNRESTRICTED_GUEST)) {
           if (guest.sregs[n].cache.type < 11) {
              // data segment or non-conforming code segment
              if (guest.sregs[n].selector.rpl > guest.sregs[n].cache.dpl) {
                BX_ERROR(("VMENTER FAIL: VMCS guest non-conforming %s.RPL < %s.DPL", segname[n], segname[n]));
                return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
              }
           }
        }
     }
  }

  if (! v8086_guest) {
     if (! (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_UNRESTRICTED_GUEST)) {
        if (guest.sregs[BX_SEG_REG_SS].selector.rpl != guest.sregs[BX_SEG_REG_CS].selector.rpl) {
           BX_ERROR(("VMENTER FAIL: VMCS guest CS.RPL != SS.RPL"));
           return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }
        if (guest.sregs[BX_SEG_REG_SS].selector.rpl != guest.sregs[BX_SEG_REG_SS].cache.dpl) {
           BX_ERROR(("VMENTER FAIL: VMCS guest SS.RPL <> SS.DPL"));
           return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
        }
     }
#if BX_SUPPORT_VMX >= 2
     else { // unrestricted guest
        if (real_mode_guest || guest.sregs[BX_SEG_REG_CS].cache.type == BX_DATA_READ_WRITE_ACCESSED) {
           if (guest.sregs[BX_SEG_REG_SS].cache.dpl != 0) {
             BX_ERROR(("VMENTER FAIL: VMCS unrestricted guest SS.DPL != 0"));
             return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
           }
        }
     }
#endif
  }

  //
  // Load and Check Guest State from VMCS - GDTR/IDTR
  //

  Bit64u gdtr_base = VMread_natural(VMCS_GUEST_GDTR_BASE);
  Bit32u gdtr_limit = VMread32(VMCS_32BIT_GUEST_GDTR_LIMIT);
  Bit64u idtr_base = VMread_natural(VMCS_GUEST_IDTR_BASE);
  Bit32u idtr_limit = VMread32(VMCS_32BIT_GUEST_IDTR_LIMIT);

#if BX_SUPPORT_X86_64
  if (! IsCanonical(gdtr_base) || ! IsCanonical(idtr_base)) {
    BX_ERROR(("VMENTER FAIL: VMCS guest IDTR/IDTR.BASE non canonical"));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
#endif
  if (gdtr_limit > 0xffff || idtr_limit > 0xffff) {
     BX_ERROR(("VMENTER FAIL: VMCS guest GDTR/IDTR limit > 0xFFFF"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  //
  // Load and Check Guest State from VMCS - LDTR
  //

  Bit16u ldtr_selector = VMread16(VMCS_16BIT_GUEST_LDTR_SELECTOR);
  Bit64u ldtr_base = VMread_natural(VMCS_GUEST_LDTR_BASE);
  Bit32u ldtr_limit = VMread32(VMCS_32BIT_GUEST_LDTR_LIMIT);
  Bit32u ldtr_ar = VMread32(VMCS_32BIT_GUEST_LDTR_ACCESS_RIGHTS);
  ldtr_ar = vmx_unpack_ar_field(ldtr_ar, BX_CPU_THIS_PTR vmcs_map->get_access_rights_format());
  bool ldtr_invalid = (ldtr_ar >> 16) & 1;
  if (set_segment_ar_data(&guest.ldtr, !ldtr_invalid,
         (Bit16u) ldtr_selector, ldtr_base, ldtr_limit, (Bit16u)(ldtr_ar)))
  {
     // ldtr is valid
     if (guest.ldtr.selector.ti) {
        BX_ERROR(("VMENTER FAIL: VMCS guest LDTR.TI set"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
     if (guest.ldtr.cache.type != BX_SYS_SEGMENT_LDT) {
        BX_ERROR(("VMENTER FAIL: VMCS guest incorrect LDTR type (%d)", guest.ldtr.cache.type));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
     if (guest.ldtr.cache.segment) {
        BX_ERROR(("VMENTER FAIL: VMCS guest LDTR is not system segment"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
     if (! guest.ldtr.cache.p) {
        BX_ERROR(("VMENTER FAIL: VMCS guest LDTR not present"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
     if (! IsLimitAccessRightsConsistent(ldtr_limit, ldtr_ar)) {
        BX_ERROR(("VMENTER FAIL: VMCS guest LDTR.AR/LIMIT malformed"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
#if BX_SUPPORT_X86_64
     if (! IsCanonical(ldtr_base)) {
        BX_ERROR(("VMENTER FAIL: VMCS guest LDTR.BASE non canonical"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
     }
#endif
  }

  //
  // Load and Check Guest State from VMCS - TR
  //

  Bit16u tr_selector = VMread16(VMCS_16BIT_GUEST_TR_SELECTOR);
  Bit64u tr_base = VMread_natural(VMCS_GUEST_TR_BASE);
  Bit32u tr_limit = VMread32(VMCS_32BIT_GUEST_TR_LIMIT);
  Bit32u tr_ar = VMread32(VMCS_32BIT_GUEST_TR_ACCESS_RIGHTS);
  tr_ar = vmx_unpack_ar_field(tr_ar, BX_CPU_THIS_PTR vmcs_map->get_access_rights_format());
  bool tr_invalid = (tr_ar >> 16) & 1;

#if BX_SUPPORT_X86_64
  if (! IsCanonical(tr_base)) {
    BX_ERROR(("VMENTER FAIL: VMCS guest TR.BASE non canonical"));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
#endif

  set_segment_ar_data(&guest.tr, !tr_invalid,
      (Bit16u) tr_selector, tr_base, tr_limit, (Bit16u)(tr_ar));

  if (tr_invalid) {
     BX_ERROR(("VMENTER FAIL: VMCS guest TR invalid"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
  if (guest.tr.selector.ti) {
     BX_ERROR(("VMENTER FAIL: VMCS guest TR.TI set"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
  if (guest.tr.cache.segment) {
     BX_ERROR(("VMENTER FAIL: VMCS guest TR is not system segment"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
  if (! guest.tr.cache.p) {
     BX_ERROR(("VMENTER FAIL: VMCS guest TR not present"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
  if (! IsLimitAccessRightsConsistent(tr_limit, tr_ar)) {
     BX_ERROR(("VMENTER FAIL: VMCS guest TR.AR/LIMIT malformed"));
     return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  switch(guest.tr.cache.type) {
    case BX_SYS_SEGMENT_BUSY_386_TSS:
      break;
    case BX_SYS_SEGMENT_BUSY_286_TSS:
      if (! x86_64_guest) break;
      // fall through
    default:
      BX_ERROR(("VMENTER FAIL: VMCS guest incorrect TR type"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  //
  // Load and Check Guest State from VMCS - MSRS
  //

  guest.ia32_debugctl_msr = VMread64(VMCS_64BIT_GUEST_IA32_DEBUGCTL);
  guest.smbase = VMread32(VMCS_32BIT_GUEST_SMBASE);

  guest.sysenter_esp_msr = VMread_natural(VMCS_GUEST_IA32_SYSENTER_ESP_MSR);
  guest.sysenter_eip_msr = VMread_natural(VMCS_GUEST_IA32_SYSENTER_EIP_MSR);
  guest.sysenter_cs_msr = VMread32(VMCS_32BIT_GUEST_IA32_SYSENTER_CS_MSR);

#if BX_SUPPORT_X86_64
  if (! IsCanonical(guest.sysenter_esp_msr)) {
    BX_ERROR(("VMENTER FAIL: VMCS guest SYSENTER_ESP_MSR non canonical"));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
  if (! IsCanonical(guest.sysenter_eip_msr)) {
    BX_ERROR(("VMENTER FAIL: VMCS guest SYSENTER_EIP_MSR non canonical"));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }
#endif

#if BX_SUPPORT_VMX >= 2
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_PAT_MSR) {
    guest.pat_msr = VMread64(VMCS_64BIT_GUEST_IA32_PAT);
    if (! isValidMSR_PAT(guest.pat_msr)) {
      BX_ERROR(("VMENTER FAIL: invalid Memory Type in guest MSR_PAT"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }
#endif

  guest.rip = VMread_natural(VMCS_GUEST_RIP);
  guest.rsp = VMread_natural(VMCS_GUEST_RSP);

#if BX_SUPPORT_VMX >= 2 && BX_SUPPORT_X86_64
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_EFER_MSR) {
    guest.efer_msr = VMread64(VMCS_64BIT_GUEST_IA32_EFER);
    if (guest.efer_msr & ~((Bit64u) BX_CPU_THIS_PTR efer_suppmask)) {
      BX_ERROR(("VMENTER FAIL: VMCS guest EFER reserved bits set !"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
    bool lme = (guest.efer_msr >>  8) & 0x1;
    bool lma = (guest.efer_msr >> 10) & 0x1;
    if (lma != x86_64_guest) {
      BX_ERROR(("VMENTER FAIL: VMCS guest EFER.LMA doesn't match x86_64_guest !"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
    if (lma != lme && (guest.cr0 & BX_CR0_PG_MASK) != 0) {
      BX_ERROR(("VMENTER FAIL: VMCS guest EFER (0x%08x) inconsistent value !", (Bit32u) guest.efer_msr));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }

  if (! x86_64_guest || !guest.sregs[BX_SEG_REG_CS].cache.u.segment.l) {
    if (GET32H(guest.rip) != 0) {
       BX_ERROR(("VMENTER FAIL: VMCS guest RIP > 32 bit"));
       return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }
#endif

  //
  // Load and Check Guest Non-Registers State from VMCS
  //

  vm->vmcs_linkptr = VMread64(VMCS_64BIT_GUEST_LINK_POINTER);
  if (vm->vmcs_linkptr != BX_INVALID_VMCSPTR) {
    if (! IsValidPageAlignedPhyAddr(vm->vmcs_linkptr)) {
      *qualification = (Bit64u) VMENTER_ERR_GUEST_STATE_LINK_POINTER;
      BX_ERROR(("VMFAIL: VMCS link pointer malformed"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }

    Bit32u revision = VMXReadRevisionID((bx_phy_address) vm->vmcs_linkptr);
    if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VMCS_SHADOWING) {
      if ((revision & BX_VMCS_SHADOW_BIT_MASK) == 0) {
        *qualification = (Bit64u) VMENTER_ERR_GUEST_STATE_LINK_POINTER;
        BX_ERROR(("VMFAIL: VMCS link pointer must indicate shadow VMCS revision ID = %d", revision));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
      }
      revision &= ~BX_VMCS_SHADOW_BIT_MASK;
    }
    if (revision != BX_CPU_THIS_PTR vmcs_map->get_vmcs_revision_id()) {
      *qualification = (Bit64u) VMENTER_ERR_GUEST_STATE_LINK_POINTER;
      BX_ERROR(("VMFAIL: VMCS link pointer incorrect revision ID %d != %d", revision, BX_CPU_THIS_PTR vmcs_map->get_vmcs_revision_id()));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }

    if (! BX_CPU_THIS_PTR in_smm || (vmentry_ctrls & VMX_VMENTRY_CTRL1_SMM_ENTER) != 0) {
      if (vm->vmcs_linkptr == BX_CPU_THIS_PTR vmcsptr) {
        *qualification = (Bit64u) VMENTER_ERR_GUEST_STATE_LINK_POINTER;
        BX_ERROR(("VMFAIL: VMCS link pointer equal to current VMCS pointer"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
      }
    }
    else {
      if (vm->vmcs_linkptr == BX_CPU_THIS_PTR vmxonptr) {
        *qualification = (Bit64u) VMENTER_ERR_GUEST_STATE_LINK_POINTER;
        BX_ERROR(("VMFAIL: VMCS link pointer equal to VMXON pointer"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
      }
    }
  }

  guest.tmpDR6 = (Bit32u) VMread_natural(VMCS_GUEST_PENDING_DBG_EXCEPTIONS);
  if (guest.tmpDR6 & BX_CONST64(0xFFFFFFFFFFFFAFF0)) {
    BX_ERROR(("VMENTER FAIL: VMCS guest tmpDR6 reserved bits"));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  guest.activity_state = VMread32(VMCS_32BIT_GUEST_ACTIVITY_STATE);
  if (guest.activity_state > BX_VMX_LAST_ACTIVITY_STATE) {
    BX_ERROR(("VMENTER FAIL: VMCS guest activity state %d", guest.activity_state));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  if (guest.activity_state == BX_ACTIVITY_STATE_HLT) {
    if (guest.sregs[BX_SEG_REG_SS].cache.dpl != 0) {
      BX_ERROR(("VMENTER FAIL: VMCS guest HLT state with SS.DPL=%d", guest.sregs[BX_SEG_REG_SS].cache.dpl));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }

  guest.interruptibility_state = VMread32(VMCS_32BIT_GUEST_INTERRUPTIBILITY_STATE);
  if (guest.interruptibility_state & ~BX_VMX_INTERRUPTIBILITY_STATE_MASK) {
    BX_ERROR(("VMENTER FAIL: VMCS guest interruptibility state broken"));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  if (guest.interruptibility_state & 0x3) {
    if (guest.activity_state != BX_ACTIVITY_STATE_ACTIVE) {
      BX_ERROR(("VMENTER FAIL: VMCS guest interruptibility state broken when entering non active CPU state %d", guest.activity_state));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }

  if ((guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_BY_STI) &&
      (guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_BY_MOV_SS))
  {
    BX_ERROR(("VMENTER FAIL: VMCS guest interruptibility state broken"));
    return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
  }

  if ((guest.rflags & EFlagsIFMask) == 0) {
    if (guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_BY_STI) {
      BX_ERROR(("VMENTER FAIL: VMCS guest interrupts can't be blocked by STI when EFLAGS.IF = 0"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }

  if (VMENTRY_INJECTING_EVENT(vm->vmentry_interr_info)) {
    unsigned event_type = (vm->vmentry_interr_info >> 8) & 7;
    unsigned vector = vm->vmentry_interr_info & 0xff;
    if (event_type == BX_EXTERNAL_INTERRUPT) {
      if ((guest.interruptibility_state & 0x3) != 0 || (guest.rflags & EFlagsIFMask) == 0) {
        BX_ERROR(("VMENTER FAIL: VMCS guest interrupts blocked when injecting external interrupt"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
      }
    }
    if (event_type == BX_NMI) {
      if ((guest.interruptibility_state & 0x3) != 0) {
        BX_ERROR(("VMENTER FAIL: VMCS guest interrupts blocked when injecting NMI"));
        return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
      }
    }
    if (guest.activity_state == BX_ACTIVITY_STATE_WAIT_FOR_SIPI) {
      BX_ERROR(("VMENTER FAIL: No guest interruptions are allowed when entering Wait-For-Sipi state"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
    if (guest.activity_state == BX_ACTIVITY_STATE_SHUTDOWN && event_type != BX_NMI && vector != BX_MC_EXCEPTION) {
      BX_ERROR(("VMENTER FAIL: Only NMI or #MC guest interruption is allowed when entering shutdown state"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }

  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_SMM_ENTER) {
    if (! (guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_SMI_BLOCKED)) {
      BX_ERROR(("VMENTER FAIL: VMCS SMM guest should block SMI"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }

    if (guest.activity_state == BX_ACTIVITY_STATE_WAIT_FOR_SIPI) {
      BX_ERROR(("VMENTER FAIL: The activity state must not indicate the wait-for-SIPI state if entering to SMM guest"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }

  if (guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_SMI_BLOCKED) {
    if (! BX_CPU_THIS_PTR in_smm) {
      BX_ERROR(("VMENTER FAIL: VMCS SMI blocked when not in SMM mode"));
      return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
    }
  }

  if (! x86_64_guest && (guest.cr4 & BX_CR4_PAE_MASK) != 0 && (guest.cr0 & BX_CR0_PG_MASK) != 0) {
#if BX_SUPPORT_VMX >= 2
    if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_ENABLE) {
      for (n=0;n<4;n++)
         guest.pdptr[n] = VMread64(VMCS_64BIT_GUEST_IA32_PDPTE0 + 2*n);

      if (! CheckPDPTR(guest.pdptr)) {
         *qualification = VMENTER_ERR_GUEST_STATE_PDPTR_LOADING;
         BX_ERROR(("VMENTER: EPT Guest State PDPTRs Checks Failed"));
         return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
      }
    }
    else
#endif
    {
      if (! CheckPDPTR(guest.cr3)) {
         *qualification = VMENTER_ERR_GUEST_STATE_PDPTR_LOADING;
         BX_ERROR(("VMENTER: Guest State PDPTRs Checks Failed"));
         return VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE;
      }
    }
  }

  //
  // Load Guest State -> VMENTER
  //

#if BX_SUPPORT_X86_64
#if BX_SUPPORT_VMX >= 2
  // modify EFER.LMA / EFER.LME before setting CR4

  // It is recommended that 64-bit VMM software use the 1-settings of the "load IA32_EFER"
  // VM entry control and the "save IA32_EFER" VM-exit control. If VMentry is establishing
  // CR0.PG=0 and if the "IA-32e mode guest" and "load IA32_EFER" VM entry controls are
  // both 0, VM entry leaves IA32_EFER.LME unmodified (i.e., the host value will persist
  // in the guest) -- Quote from Intel SDM
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_EFER_MSR) {
     BX_CPU_THIS_PTR efer.set32((Bit32u) guest.efer_msr);
  }
  else
#endif
  {
    if (x86_64_guest) {
      BX_CPU_THIS_PTR efer.set32(BX_CPU_THIS_PTR efer.get32() | (BX_EFER_LME_MASK | BX_EFER_LMA_MASK));
    }
    else {
      // when loading unrestricted guest with CR0.PG=0 EFER.LME is unmodified
      // (i.e., the host value will persist in the guest)
      if (guest.cr0 & BX_CR0_PG_MASK)
        BX_CPU_THIS_PTR efer.set32(BX_CPU_THIS_PTR efer.get32() & ~(BX_EFER_LME_MASK | BX_EFER_LMA_MASK));
      else
        BX_CPU_THIS_PTR efer.set32(BX_CPU_THIS_PTR efer.get32() &  ~BX_EFER_LMA_MASK);
    }
  }
#endif

// keep bits ET(4), reserved bits 15:6, 17, 28:19, NW(29), CD(30)
#define VMX_KEEP_CR0_BITS 0x7FFAFFD0

  guest.cr0 = (BX_CPU_THIS_PTR cr0.get32() & VMX_KEEP_CR0_BITS) | (guest.cr0 & ~VMX_KEEP_CR0_BITS);

  if (! check_CR0(guest.cr0)) {
    BX_PANIC(("VMENTER CR0 is broken !"));
  }
  if (! check_CR4(guest.cr4)) {
    BX_PANIC(("VMENTER CR4 is broken !"));
  }

  BX_CPU_THIS_PTR cr0.set32((Bit32u) guest.cr0);
  BX_CPU_THIS_PTR cr4.set32((Bit32u) guest.cr4);
  BX_CPU_THIS_PTR cr3 = guest.cr3;

#if BX_SUPPORT_VMX >= 2
  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_ENABLE) {
    // load PDPTR only in PAE legacy mode
    if (BX_CPU_THIS_PTR cr0.get_PG() && BX_CPU_THIS_PTR cr4.get_PAE() && !x86_64_guest) {
      for (n = 0; n < 4; n++)
        BX_CPU_THIS_PTR PDPTR_CACHE.entry[n] = guest.pdptr[n];
    }
  }
#endif

  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_DBG_CTRLS) {
    // always clear bits 15:14 and set bit 10
    BX_CPU_THIS_PTR dr7.set32((guest.dr7 & ~0xc000) | 0x400);
  }

  RIP = BX_CPU_THIS_PTR prev_rip = guest.rip;
  RSP = guest.rsp;

#if BX_SUPPORT_CET
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_GUEST_CET_STATE) {
    SSP = guest.ssp;
    BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table = guest.interrupt_ssp_table_address;
    BX_CPU_THIS_PTR msr.ia32_cet_control[0] = guest.msr_ia32_s_cet;
  }
#endif

#if BX_SUPPORT_PKEYS
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_GUEST_PKRS) {
    set_PKeys(BX_CPU_THIS_PTR pkru, guest.pkrs);
  }
#endif

  BX_CPU_THIS_PTR async_event = 0;

  setEFlags((Bit32u) guest.rflags);

#ifdef BX_SUPPORT_CS_LIMIT_DEMOTION
  // Handle special case of CS.LIMIT demotion (new descriptor limit is
  // smaller than current one)
  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled > guest.sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled)
    BX_CPU_THIS_PTR iCache.flushICacheEntries();
#endif

  for(unsigned segreg=0; segreg<6; segreg++)
    BX_CPU_THIS_PTR sregs[segreg] = guest.sregs[segreg];

  BX_CPU_THIS_PTR gdtr.base = gdtr_base;
  BX_CPU_THIS_PTR gdtr.limit = gdtr_limit;
  BX_CPU_THIS_PTR idtr.base = idtr_base;
  BX_CPU_THIS_PTR idtr.limit = idtr_limit;

  BX_CPU_THIS_PTR ldtr = guest.ldtr;
  BX_CPU_THIS_PTR tr = guest.tr;

  BX_CPU_THIS_PTR msr.sysenter_esp_msr = guest.sysenter_esp_msr;
  BX_CPU_THIS_PTR msr.sysenter_eip_msr = guest.sysenter_eip_msr;
  BX_CPU_THIS_PTR msr.sysenter_cs_msr  = guest.sysenter_cs_msr;

#if BX_SUPPORT_VMX >= 2
  if (vmentry_ctrls & VMX_VMENTRY_CTRL1_LOAD_PAT_MSR) {
    BX_CPU_THIS_PTR msr.pat = guest.pat_msr;
  }
  vm->ple.last_pause_time = vm->ple.first_pause_time = 0;
#endif

  //
  // Load Guest Non-Registers State -> VMENTER
  //

  if (vm->vmentry_ctrls & VMX_VMENTRY_CTRL1_SMM_ENTER)
    BX_PANIC(("VMENTER: entry to SMM is not implemented yet !"));

  if (VMENTRY_INJECTING_EVENT(vm->vmentry_interr_info)) {
    // the VMENTRY injecting event to the guest
    BX_CPU_THIS_PTR inhibit_mask = 0; // do not block interrupts
    BX_CPU_THIS_PTR debug_trap = 0;
    guest.activity_state = BX_ACTIVITY_STATE_ACTIVE;
  }
  else {
    if (guest.tmpDR6 & (1 << 12))
      BX_CPU_THIS_PTR debug_trap = guest.tmpDR6 & 0x0000400F;
    else
      BX_CPU_THIS_PTR debug_trap = guest.tmpDR6 & 0x00004000;
    if (BX_CPU_THIS_PTR debug_trap) {
      BX_CPU_THIS_PTR debug_trap |= BX_DEBUG_TRAP_HIT;
      BX_CPU_THIS_PTR async_event = 1;
    }

    if (guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_BY_STI)
      inhibit_interrupts(BX_INHIBIT_INTERRUPTS);
    else if (guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_BY_MOV_SS)
      inhibit_interrupts(BX_INHIBIT_INTERRUPTS_BY_MOVSS);
    else
      BX_CPU_THIS_PTR inhibit_mask = 0;
  }

  if (guest.interruptibility_state & BX_VMX_INTERRUPTS_BLOCKED_NMI_BLOCKED) {
    if (vm->vmexec_ctrls1 & VMX_VM_EXEC_CTRL1_VIRTUAL_NMI)
      mask_event(BX_EVENT_VMX_VIRTUAL_NMI);
    else
      mask_event(BX_EVENT_NMI);
  }

  if (vm->vmexec_ctrls2 & VMX_VM_EXEC_CTRL2_MONITOR_TRAP_FLAG) {
    signal_event(BX_EVENT_VMX_MONITOR_TRAP_FLAG);
      mask_event(BX_EVENT_VMX_MONITOR_TRAP_FLAG);
    BX_CPU_THIS_PTR async_event = 1;
  }

  if (vm->vmexec_ctrls2 & VMX_VM_EXEC_CTRL2_NMI_WINDOW_EXITING)
    signal_event(BX_EVENT_VMX_VIRTUAL_NMI);

  if (vm->vmexec_ctrls2 & VMX_VM_EXEC_CTRL2_INTERRUPT_WINDOW_VMEXIT)
    signal_event(BX_EVENT_VMX_INTERRUPT_WINDOW_EXITING);

  handleCpuContextChange();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);

  if (guest.activity_state) {
    BX_DEBUG(("VMEntry to non-active CPU state %d", guest.activity_state));
    enter_sleep_state(guest.activity_state);
  }

  return VMXERR_NO_ERROR;
}

void BX_CPU_C::VMenterInjectEvents(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (! VMENTRY_INJECTING_EVENT(vm->vmentry_interr_info))
     return;

  /* the VMENTRY injecting event to the guest */
  unsigned vector = vm->vmentry_interr_info & 0xff;
  unsigned type = (vm->vmentry_interr_info >> 8) & 7;
  unsigned push_error = vm->vmentry_interr_info & (1 << 11);
  unsigned error_code = push_error ? vm->vmentry_excep_err_code : 0;

  if (type == 7) {
    if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_MONITOR_TRAP_FLAG)) {
      signal_event(BX_EVENT_VMX_MONITOR_TRAP_FLAG);
      return;
    }
  }

  bool is_INT = false;
  switch(type) {
    case BX_EXTERNAL_INTERRUPT:
    case BX_HARDWARE_EXCEPTION:
      BX_CPU_THIS_PTR EXT = 1;
      break;

    case BX_NMI:
      if (vm->vmexec_ctrls1 & VMX_VM_EXEC_CTRL1_VIRTUAL_NMI)
        mask_event(BX_EVENT_VMX_VIRTUAL_NMI);
      else
        mask_event(BX_EVENT_NMI);

      BX_CPU_THIS_PTR EXT = 1;
      break;

    case BX_PRIVILEGED_SOFTWARE_INTERRUPT:
      BX_CPU_THIS_PTR EXT = 1;
      is_INT = true;
      break;

    case BX_SOFTWARE_INTERRUPT:
    case BX_SOFTWARE_EXCEPTION:
      is_INT = true;
      break;

    default:
      BX_PANIC(("VMENTER: unsupported event injection type %d !", type));
  }

  // keep prev_rip value/unwind in case of event delivery failure
  if (is_INT)
    RIP += vm->vmentry_instr_length;

  BX_DEBUG(("VMENTER: Injecting vector 0x%02x (error_code 0x%04x)", vector, error_code));

  if (type == BX_HARDWARE_EXCEPTION) {
    // record exception the same way as BX_CPU_C::exception does
    BX_ASSERT(vector < BX_CPU_HANDLED_EXCEPTIONS);
    BX_CPU_THIS_PTR last_exception_type = exceptions_info[vector].exception_type;
  }

  vm->idt_vector_info = vm->vmentry_interr_info & ~0x80000000;
  vm->idt_vector_error_code = error_code;

  interrupt(vector, type, push_error, error_code);

  BX_CPU_THIS_PTR last_exception_type = 0; // error resolved
}

Bit32u BX_CPU_C::LoadMSRs(Bit32u msr_cnt, bx_phy_address pAddr)
{
  Bit64u msr_lo, msr_hi;

  for (Bit32u msr = 1; msr <= msr_cnt; msr++) {
    access_read_physical(pAddr,     8, &msr_lo);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_VMX_LOAD_MSR_ACCESS, (Bit8u*)(&msr_lo));
    access_read_physical(pAddr + 8, 8, &msr_hi);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr + 8, 8, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_VMX_LOAD_MSR_ACCESS, (Bit8u*)(&msr_hi));
    pAddr += 16; // to next MSR

    if (GET32H(msr_lo)) {
      BX_ERROR(("VMX LoadMSRs %d: broken msr index 0x" FMT_LL "x", msr, msr_lo));
      return msr;
    }

    Bit32u index = GET32L(msr_lo);

#if BX_SUPPORT_X86_64
    if (index == BX_MSR_FSBASE || index == BX_MSR_GSBASE) {
      BX_ERROR(("VMX LoadMSRs %d: unable to restore FSBASE or GSBASE", msr));
      return msr;
    }
#endif

    if (is_cpu_extension_supported(BX_ISA_X2APIC)) {
      if (is_x2apic_msr_range(index)) {
        BX_ERROR(("VMX LoadMSRs %d: unable to restore X2APIC range MSR %x", msr, index));
        return msr;
      }
    }

    if (! wrmsr(index, msr_hi)) {
      BX_ERROR(("VMX LoadMSRs %d: unable to set up MSR %x", msr, index));
      return msr;
    }
  }

  return 0;
}

Bit32u BX_CPU_C::StoreMSRs(Bit32u msr_cnt, bx_phy_address pAddr)
{
  Bit64u msr_lo, msr_hi;

  for (Bit32u msr = 1; msr <= msr_cnt; msr++) {
    access_read_physical(pAddr, 8, &msr_lo);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(resolve_memtype(pAddr)),
                                          BX_READ, BX_VMX_STORE_MSR_ACCESS, (Bit8u*)(&msr_lo));

    if (GET32H(msr_lo)) {
      BX_ERROR(("VMX StoreMSRs %d: broken msr index 0x" FMT_LL "x", msr, msr_lo));
      return msr;
    }

    Bit32u index = GET32L(msr_lo);

    if (is_cpu_extension_supported(BX_ISA_X2APIC)) {
      if (is_x2apic_msr_range(index)) {
        BX_ERROR(("VMX StoreMSRs %d: unable to save X2APIC range MSR %x", msr, index));
        return msr;
      }
    }

    if (! rdmsr(index, &msr_hi)) {
      BX_ERROR(("VMX StoreMSRs %d: unable to read MSR %x", msr, index));
      return msr;
    }

    access_write_physical(pAddr + 8, 8, &msr_hi);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr + 8, 8, MEMTYPE(resolve_memtype(pAddr)),
                                              BX_WRITE, BX_VMX_STORE_MSR_ACCESS, (Bit8u*)(&msr_hi));

    pAddr += 16; // to next MSR
  }

  return 0;
}

////////////////////////////////////////////////////////////
// VMexit
////////////////////////////////////////////////////////////

void BX_CPU_C::VMexitSaveGuestState(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  int n;

  VMwrite_natural(VMCS_GUEST_CR0, BX_CPU_THIS_PTR cr0.get32());
  VMwrite_natural(VMCS_GUEST_CR3, BX_CPU_THIS_PTR cr3);
  VMwrite_natural(VMCS_GUEST_CR4, BX_CPU_THIS_PTR cr4.get32());

#if BX_SUPPORT_VMX >= 2
  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_EPT_ENABLE) {
    // save only if guest running in legacy PAE mode
    if (BX_CPU_THIS_PTR cr0.get_PG() && BX_CPU_THIS_PTR cr4.get_PAE() && !long_mode()) {
      for(n=0; n<4; n++) {
        VMwrite64(VMCS_64BIT_GUEST_IA32_PDPTE0 + 2*n, BX_CPU_THIS_PTR PDPTR_CACHE.entry[n]);
      }
    }
  }
#endif

  if (vm->vmexit_ctrls & VMX_VMEXIT_CTRL1_SAVE_DBG_CTRLS)
     VMwrite_natural(VMCS_GUEST_DR7, BX_CPU_THIS_PTR dr7.get32());

  VMwrite_natural(VMCS_GUEST_RIP, RIP);
  VMwrite_natural(VMCS_GUEST_RSP, RSP);
  VMwrite_natural(VMCS_GUEST_RFLAGS, read_eflags());

#if BX_SUPPORT_CET
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
    VMwrite_natural(VMCS_GUEST_IA32_S_CET, BX_CPU_THIS_PTR msr.ia32_cet_control[0]);
    VMwrite_natural(VMCS_GUEST_INTERRUPT_SSP_TABLE_ADDR, BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table);
    VMwrite_natural(VMCS_GUEST_SSP, SSP);
  }
#endif

#if BX_SUPPORT_PKEYS
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_PKS)) {
    VMwrite64(VMCS_64BIT_GUEST_IA32_PKRS, BX_CPU_THIS_PTR pkrs);
  }
#endif

  for (n=0; n<6; n++) {
     Bit32u selector = BX_CPU_THIS_PTR sregs[n].selector.value;
     bool invalid = !BX_CPU_THIS_PTR sregs[n].cache.valid;
     bx_address base = BX_CPU_THIS_PTR sregs[n].cache.u.segment.base;
     Bit32u limit = BX_CPU_THIS_PTR sregs[n].cache.u.segment.limit_scaled;
     Bit32u ar = (get_descriptor_h(&BX_CPU_THIS_PTR sregs[n].cache) & 0x00f0ff00) >> 8;
     ar = vmx_pack_ar_field(ar | (invalid << 16), BX_CPU_THIS_PTR vmcs_map->get_access_rights_format());

     VMwrite16(VMCS_16BIT_GUEST_ES_SELECTOR + 2*n, selector);
     VMwrite32(VMCS_32BIT_GUEST_ES_ACCESS_RIGHTS + 2*n, ar);
     VMwrite_natural(VMCS_GUEST_ES_BASE + 2*n, base);
     VMwrite32(VMCS_32BIT_GUEST_ES_LIMIT + 2*n, limit);
  }

  // save guest LDTR
  Bit32u ldtr_selector = BX_CPU_THIS_PTR ldtr.selector.value;
  bool ldtr_invalid = !BX_CPU_THIS_PTR ldtr.cache.valid;
  bx_address ldtr_base = BX_CPU_THIS_PTR ldtr.cache.u.segment.base;
  Bit32u ldtr_limit = BX_CPU_THIS_PTR ldtr.cache.u.segment.limit_scaled;
  Bit32u ldtr_ar = (get_descriptor_h(&BX_CPU_THIS_PTR ldtr.cache) & 0x00f0ff00) >> 8;
  ldtr_ar = vmx_pack_ar_field(ldtr_ar | (ldtr_invalid << 16), BX_CPU_THIS_PTR vmcs_map->get_access_rights_format());

  VMwrite16(VMCS_16BIT_GUEST_LDTR_SELECTOR, ldtr_selector);
  VMwrite32(VMCS_32BIT_GUEST_LDTR_ACCESS_RIGHTS, ldtr_ar);
  VMwrite_natural(VMCS_GUEST_LDTR_BASE, ldtr_base);
  VMwrite32(VMCS_32BIT_GUEST_LDTR_LIMIT, ldtr_limit);

  // save guest TR
  Bit32u tr_selector = BX_CPU_THIS_PTR tr.selector.value;
  bool tr_invalid = !BX_CPU_THIS_PTR tr.cache.valid;
  bx_address tr_base = BX_CPU_THIS_PTR tr.cache.u.segment.base;
  Bit32u tr_limit = BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled;
  Bit32u tr_ar = (get_descriptor_h(&BX_CPU_THIS_PTR tr.cache) & 0x00f0ff00) >> 8;
  tr_ar = vmx_pack_ar_field(tr_ar | (tr_invalid << 16), BX_CPU_THIS_PTR vmcs_map->get_access_rights_format());

  VMwrite16(VMCS_16BIT_GUEST_TR_SELECTOR, tr_selector);
  VMwrite32(VMCS_32BIT_GUEST_TR_ACCESS_RIGHTS, tr_ar);
  VMwrite_natural(VMCS_GUEST_TR_BASE, tr_base);
  VMwrite32(VMCS_32BIT_GUEST_TR_LIMIT, tr_limit);

  VMwrite_natural(VMCS_GUEST_GDTR_BASE, BX_CPU_THIS_PTR gdtr.base);
  VMwrite32(VMCS_32BIT_GUEST_GDTR_LIMIT, BX_CPU_THIS_PTR gdtr.limit);
  VMwrite_natural(VMCS_GUEST_IDTR_BASE, BX_CPU_THIS_PTR idtr.base);
  VMwrite32(VMCS_32BIT_GUEST_IDTR_LIMIT, BX_CPU_THIS_PTR idtr.limit);

  VMwrite_natural(VMCS_GUEST_IA32_SYSENTER_ESP_MSR, BX_CPU_THIS_PTR msr.sysenter_esp_msr);
  VMwrite_natural(VMCS_GUEST_IA32_SYSENTER_EIP_MSR, BX_CPU_THIS_PTR msr.sysenter_eip_msr);
  VMwrite32(VMCS_32BIT_GUEST_IA32_SYSENTER_CS_MSR, BX_CPU_THIS_PTR msr.sysenter_cs_msr);

#if BX_SUPPORT_VMX >= 2
  if (vm->vmexit_ctrls & VMX_VMEXIT_CTRL1_STORE_PAT_MSR)
    VMwrite64(VMCS_64BIT_GUEST_IA32_PAT, BX_CPU_THIS_PTR msr.pat.u64);
#if BX_SUPPORT_X86_64
  if (vm->vmexit_ctrls & VMX_VMEXIT_CTRL1_STORE_EFER_MSR)
    VMwrite64(VMCS_64BIT_GUEST_IA32_EFER, BX_CPU_THIS_PTR efer.get32());
#endif
#endif

  Bit32u tmpDR6 = BX_CPU_THIS_PTR debug_trap & 0x0000400f;
  if (tmpDR6 & 0xf) tmpDR6 |= (1 << 12);
  VMwrite_natural(VMCS_GUEST_PENDING_DBG_EXCEPTIONS, tmpDR6);

  // effectively wakeup from MWAIT state on VMEXIT
  if (BX_CPU_THIS_PTR activity_state >= BX_VMX_LAST_ACTIVITY_STATE)
    VMwrite32(VMCS_32BIT_GUEST_ACTIVITY_STATE, BX_ACTIVITY_STATE_ACTIVE);
  else
    VMwrite32(VMCS_32BIT_GUEST_ACTIVITY_STATE, BX_CPU_THIS_PTR activity_state);

  Bit32u interruptibility_state = 0;
  if (interrupts_inhibited(BX_INHIBIT_INTERRUPTS)) {
     if (interrupts_inhibited(BX_INHIBIT_DEBUG))
        interruptibility_state |= BX_VMX_INTERRUPTS_BLOCKED_BY_MOV_SS;
     else
        interruptibility_state |= BX_VMX_INTERRUPTS_BLOCKED_BY_STI;
  }

  // Do not set BX_VMX_INTERRUPTS_BLOCKED_SMI_BLOCKED (as the dual-monitor
  // treatment is unimplemented).
  // "VM exits that end outside system-management mode (SMM) save bit 2 (blocking by SMI)
  //  as 0 regardless of the state of such blocking before the VM exit."

  if (vm->vmexec_ctrls1 & VMX_VM_EXEC_CTRL1_VIRTUAL_NMI) {
    if (is_masked_event(BX_EVENT_VMX_VIRTUAL_NMI))
      interruptibility_state |= BX_VMX_INTERRUPTS_BLOCKED_NMI_BLOCKED;
  }
  else {
    if (is_masked_event(BX_EVENT_NMI))
      interruptibility_state |= BX_VMX_INTERRUPTS_BLOCKED_NMI_BLOCKED;
  }

  VMwrite32(VMCS_32BIT_GUEST_INTERRUPTIBILITY_STATE, interruptibility_state);

#if BX_SUPPORT_VMX >= 2
  if (VMX_MSR_MISC & VMX_MISC_STORE_LMA_TO_X86_64_GUEST_VMENTRY_CONTROL) {
    // VMEXITs store the value of EFER.LMA into the x86-64 guest VMENTRY control
    // must be set if unrestricted guest is supported
    if (long_mode())
       vm->vmentry_ctrls |=  VMX_VMENTRY_CTRL1_X86_64_GUEST;
    else
       vm->vmentry_ctrls &= ~VMX_VMENTRY_CTRL1_X86_64_GUEST;

    VMwrite32(VMCS_32BIT_CONTROL_VMENTRY_CONTROLS, vm->vmentry_ctrls);
  }

  // Deactivate VMX preemtion timer
  BX_CPU_THIS_PTR lapic.deactivate_vmx_preemption_timer();
  clear_event(BX_EVENT_VMX_PREEMPTION_TIMER_EXPIRED);
  // Store back to VMCS
  if (vm->vmexit_ctrls & VMX_VMEXIT_CTRL1_STORE_VMX_PREEMPTION_TIMER)
    VMwrite32(VMCS_32BIT_GUEST_PREEMPTION_TIMER_VALUE, BX_CPU_THIS_PTR lapic.read_vmx_preemption_timer());

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY) {
    VMwrite16(VMCS_16BIT_GUEST_INTERRUPT_STATUS, (((Bit16u) vm->svi) << 8) | vm->rvi);
  }

  if (vm->vmexec_ctrls3 & VMX_VM_EXEC_CTRL3_PML_ENABLE) {
    VMwrite16(VMCS_16BIT_GUEST_PML_INDEX, vm->pml_index);
  }
#endif
}

void BX_CPU_C::VMexitLoadHostState(void)
{
  VMCS_HOST_STATE *host_state = &BX_CPU_THIS_PTR vmcs.host_state;
  bool x86_64_host = false;
  BX_CPU_THIS_PTR tsc_offset = 0;

#if BX_SUPPORT_X86_64
  Bit32u vmexit_ctrls = BX_CPU_THIS_PTR vmcs.vmexit_ctrls;
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_HOST_ADDR_SPACE_SIZE) {
     BX_DEBUG(("VMEXIT to x86-64 host"));
     x86_64_host = true;
  }

#if BX_SUPPORT_VMX >= 2
  // modify EFER.LMA / EFER.LME before setting CR4
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_LOAD_EFER_MSR) {
     BX_CPU_THIS_PTR efer.set32((Bit32u) host_state->efer_msr);
  }
  else
#endif
  {
    if (x86_64_host)
       BX_CPU_THIS_PTR efer.set32(BX_CPU_THIS_PTR efer.get32() |  (BX_EFER_LME_MASK | BX_EFER_LMA_MASK));
    else
       BX_CPU_THIS_PTR efer.set32(BX_CPU_THIS_PTR efer.get32() & ~(BX_EFER_LME_MASK | BX_EFER_LMA_MASK));
  }
#endif

  // ET, CD, NW, 28:19, 17, 15:6, and VMX fixed bits not modified Section 19.8
  host_state->cr0 = (BX_CPU_THIS_PTR cr0.get32() & VMX_KEEP_CR0_BITS) | (host_state->cr0 & ~VMX_KEEP_CR0_BITS);

  if (! check_CR0(host_state->cr0)) {
    BX_PANIC(("VMEXIT CR0 is broken !"));
  }
  if (! check_CR4(host_state->cr4)) {
    BX_PANIC(("VMEXIT CR4 is broken !"));
  }

  BX_CPU_THIS_PTR cr0.set32((Bit32u) host_state->cr0);
  BX_CPU_THIS_PTR cr4.set32((Bit32u) host_state->cr4);
  BX_CPU_THIS_PTR cr3 = host_state->cr3;

  if (! x86_64_host && BX_CPU_THIS_PTR cr4.get_PAE()) {
    if (! CheckPDPTR(host_state->cr3)) {
      BX_ERROR(("VMABORT: host PDPTRs are corrupted !"));
      VMabort(VMABORT_HOST_PDPTR_CORRUPTED);
    }
  }

  BX_CPU_THIS_PTR dr7.set32(0x00000400);

  BX_CPU_THIS_PTR msr.sysenter_cs_msr = host_state->sysenter_cs_msr;
  BX_CPU_THIS_PTR msr.sysenter_esp_msr = host_state->sysenter_esp_msr;
  BX_CPU_THIS_PTR msr.sysenter_eip_msr = host_state->sysenter_eip_msr;

#if BX_SUPPORT_VMX >= 2
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_LOAD_PAT_MSR) {
    BX_CPU_THIS_PTR msr.pat = host_state->pat_msr;
  }
#endif

  // CS selector loaded from VMCS
  //    valid   <= 1
  //    base    <= 0
  //    limit   <= 0xffffffff, g <= 1
  //    present <= 1
  //    dpl     <= 0
  //    type    <= segment, BX_CODE_EXEC_READ_ACCESSED
  //    d_b     <= loaded from 'host-address space size' VMEXIT control
  //    l       <= loaded from 'host-address space size' VMEXIT control

  parse_selector(host_state->segreg_selector[BX_SEG_REG_CS],
               &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.valid    = SegValidCache | SegAccessROK | SegAccessWOK | SegAccessROK4G | SegAccessWOK4G;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.p        = 1;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.dpl      = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.segment  = 1;  /* data/code segment */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.type     = BX_CODE_EXEC_READ_ACCESSED;

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base         = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xffffffff;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.avl = 0;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.g   = 1; /* page granular */
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b = !x86_64_host;
#if BX_SUPPORT_X86_64
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.l   =  x86_64_host;
#endif

  // DATA selector loaded from VMCS
  //    valid   <= if selector is not all-zero
  //    base    <= 0
  //    limit   <= 0xffffffff, g <= 1
  //    present <= 1
  //    dpl     <= 0
  //    type    <= segment, BX_DATA_READ_WRITE_ACCESSED
  //    d_b     <= 1
  //    l       <= 0

  for (unsigned segreg = 0; segreg < 6; segreg++)
  {
    if (segreg == BX_SEG_REG_CS) continue;

    parse_selector(host_state->segreg_selector[segreg],
               &BX_CPU_THIS_PTR sregs[segreg].selector);

    if (! host_state->segreg_selector[segreg]) {
       BX_CPU_THIS_PTR sregs[segreg].cache.valid    = 0;
    }
    else {
       BX_CPU_THIS_PTR sregs[segreg].cache.valid    = SegValidCache;
       BX_CPU_THIS_PTR sregs[segreg].cache.p        = 1;
       BX_CPU_THIS_PTR sregs[segreg].cache.dpl      = 0;
       BX_CPU_THIS_PTR sregs[segreg].cache.segment  = 1;  /* data/code segment */
       BX_CPU_THIS_PTR sregs[segreg].cache.type     = BX_DATA_READ_WRITE_ACCESSED;
       BX_CPU_THIS_PTR sregs[segreg].cache.u.segment.base         = 0;
       BX_CPU_THIS_PTR sregs[segreg].cache.u.segment.limit_scaled = 0xffffffff;
       BX_CPU_THIS_PTR sregs[segreg].cache.u.segment.avl = 0;
       BX_CPU_THIS_PTR sregs[segreg].cache.u.segment.g   = 1; /* page granular */
       BX_CPU_THIS_PTR sregs[segreg].cache.u.segment.d_b = 1;
#if BX_SUPPORT_X86_64
       BX_CPU_THIS_PTR sregs[segreg].cache.u.segment.l   = 0;
#endif
    }
  }

  // SS.DPL always clear
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.dpl = 0;

  if (x86_64_host || BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid)
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.u.segment.base = host_state->fs_base;

  if (x86_64_host || BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid)
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.u.segment.base = host_state->gs_base;

  // TR selector loaded from VMCS
  parse_selector(host_state->tr_selector, &BX_CPU_THIS_PTR tr.selector);

  BX_CPU_THIS_PTR tr.cache.valid    = SegValidCache; /* valid */
  BX_CPU_THIS_PTR tr.cache.p        = 1; /* present */
  BX_CPU_THIS_PTR tr.cache.dpl      = 0; /* field not used */
  BX_CPU_THIS_PTR tr.cache.segment  = 0; /* system segment */
  BX_CPU_THIS_PTR tr.cache.type     = BX_SYS_SEGMENT_BUSY_386_TSS;
  BX_CPU_THIS_PTR tr.cache.u.segment.base         = host_state->tr_base;
  BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled = 0x67;
  BX_CPU_THIS_PTR tr.cache.u.segment.avl = 0;
  BX_CPU_THIS_PTR tr.cache.u.segment.g   = 0; /* byte granular */

  // unusable LDTR
  BX_CPU_THIS_PTR ldtr.selector.value = 0x0000;
  BX_CPU_THIS_PTR ldtr.selector.index = 0x0000;
  BX_CPU_THIS_PTR ldtr.selector.ti    = 0;
  BX_CPU_THIS_PTR ldtr.selector.rpl   = 0;
  BX_CPU_THIS_PTR ldtr.cache.valid    = 0; /* invalid */

  BX_CPU_THIS_PTR gdtr.base = host_state->gdtr_base;
  BX_CPU_THIS_PTR gdtr.limit = 0xFFFF;

  BX_CPU_THIS_PTR idtr.base = host_state->idtr_base;
  BX_CPU_THIS_PTR idtr.limit = 0xFFFF;

  RIP = BX_CPU_THIS_PTR prev_rip = host_state->rip;
  RSP = host_state->rsp;

#if BX_SUPPORT_CET
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_LOAD_HOST_CET_STATE) {
    SSP = host_state->ssp;
    BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table = host_state->interrupt_ssp_table_address;
    BX_CPU_THIS_PTR msr.ia32_cet_control[0] = host_state->msr_ia32_s_cet;
  }
#endif

#if BX_SUPPORT_PKEYS
  if (vmexit_ctrls & VMX_VMEXIT_CTRL1_LOAD_HOST_PKRS) {
    set_PKeys(BX_CPU_THIS_PTR pkru, host_state->pkrs);
  }
#endif

  BX_CPU_THIS_PTR inhibit_mask = 0;
  BX_CPU_THIS_PTR debug_trap = 0;

  // set flags directly, avoid setEFlags side effects
  BX_CPU_THIS_PTR eflags = 0x2; // Bit1 is always set
  // Update lazy flags state
  clearEFlagsOSZAPC();

  BX_CPU_THIS_PTR activity_state = BX_ACTIVITY_STATE_ACTIVE;

  handleCpuContextChange();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);
}

void BX_CPU_C::VMexit(Bit32u reason, Bit64u qualification)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (!BX_CPU_THIS_PTR in_vmx || !BX_CPU_THIS_PTR in_vmx_guest) {
    if ((reason & 0x80000000) == 0)
      BX_PANIC(("PANIC: VMEXIT not in VMX guest mode !"));
  }

  BX_INSTR_VMEXIT(BX_CPU_ID, reason, qualification);

  //
  // STEP 0: Update VMEXIT reason
  //

  VMwrite32(VMCS_32BIT_VMEXIT_REASON, reason);
  VMwrite_natural(VMCS_VMEXIT_QUALIFICATION, qualification);

  // clipping with 0xf not really necessary but keep it for safety
  VMwrite32(VMCS_32BIT_VMEXIT_INSTRUCTION_LENGTH, (RIP-BX_CPU_THIS_PTR prev_rip) & 0xf);

  reason &= 0xffff; /* keep only basic VMEXIT reason */

  if (reason >= VMX_VMEXIT_LAST_REASON)
    BX_PANIC(("PANIC: broken VMEXIT reason %d", reason));
  else
    BX_DEBUG(("VMEXIT reason = %d (%s) qualification=0x" FMT_LL "x", reason, VMX_vmexit_reason_name[reason], qualification));

  if (reason != VMX_VMEXIT_EXCEPTION_NMI && reason != VMX_VMEXIT_EXTERNAL_INTERRUPT) {
    VMwrite32(VMCS_32BIT_VMEXIT_INTERRUPTION_INFO, 0);
  }

  if (BX_CPU_THIS_PTR in_event) {
    VMwrite32(VMCS_32BIT_IDT_VECTORING_INFO, vm->idt_vector_info | 0x80000000);
    VMwrite32(VMCS_32BIT_IDT_VECTORING_ERR_CODE, vm->idt_vector_error_code);
    BX_CPU_THIS_PTR in_event = false;
  }
  else {
    VMwrite32(VMCS_32BIT_IDT_VECTORING_INFO, 0);
  }

  BX_CPU_THIS_PTR nmi_unblocking_iret = false;

  // VMEXITs are FAULT-like: restore RIP/RSP to value before VMEXIT occurred
  if (! IS_TRAP_LIKE_VMEXIT(reason)) {
    RIP = BX_CPU_THIS_PTR prev_rip;
    if (BX_CPU_THIS_PTR speculative_rsp) {
      RSP = BX_CPU_THIS_PTR prev_rsp;
#if BX_SUPPORT_CET
      SSP = BX_CPU_THIS_PTR prev_ssp;
#endif
    }
  }
  BX_CPU_THIS_PTR speculative_rsp = false;

  //
  // STEP 1: Saving Guest State to VMCS
  //
  if (reason != VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE && reason != VMX_VMEXIT_VMENTRY_FAILURE_MSR) {
    // clear VMENTRY interruption info field
    VMwrite32(VMCS_32BIT_CONTROL_VMENTRY_INTERRUPTION_INFO, vm->vmentry_interr_info & ~0x80000000);

    VMexitSaveGuestState();

    Bit32u msr = StoreMSRs(vm->vmexit_msr_store_cnt, vm->vmexit_msr_store_addr);
    if (msr) {
      BX_ERROR(("VMABORT: Error when saving guest MSR number %d", msr));
      VMabort(VMABORT_SAVING_GUEST_MSRS_FAILURE);
    }
  }

  BX_CPU_THIS_PTR in_vmx_guest = false;

  // entering VMX root mode: clear possibly pending guest VMX events
  clear_event(BX_EVENT_VMX_VTPR_UPDATE |
              BX_EVENT_VMX_VEOI_UPDATE |
              BX_EVENT_VMX_VIRTUAL_APIC_WRITE |
              BX_EVENT_VMX_MONITOR_TRAP_FLAG |
              BX_EVENT_VMX_INTERRUPT_WINDOW_EXITING |
              BX_EVENT_VMX_PREEMPTION_TIMER_EXPIRED |
              BX_EVENT_VMX_VIRTUAL_NMI |
              BX_EVENT_PENDING_VMX_VIRTUAL_INTR);

  //
  // STEP 2: Load Host State
  //
  VMexitLoadHostState();

  //
  // STEP 3: Load Host MSR registers
  //

  Bit32u msr = LoadMSRs(vm->vmexit_msr_load_cnt, vm->vmexit_msr_load_addr);
  if (msr) {
    BX_ERROR(("VMABORT: Error when loading host MSR number %d", msr));
    VMabort(VMABORT_LOADING_HOST_MSRS);
  }

  //
  // STEP 4: Go back to VMX host
  //

  mask_event(BX_EVENT_INIT); // INIT is disabled in VMX root mode

  BX_CPU_THIS_PTR EXT = 0;
  BX_CPU_THIS_PTR last_exception_type = 0;

#if BX_DEBUGGER
  if (BX_CPU_THIS_PTR vmexit_break) {
    BX_CPU_THIS_PTR stop_reason = STOP_VMEXIT_BREAK_POINT;
    bx_debug_break(); // trap into debugger
  }
#endif

  if (! IS_TRAP_LIKE_VMEXIT(reason)) {
    longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
  }
}

#endif // BX_SUPPORT_VMX

////////////////////////////////////////////////////////////
// VMX instructions
////////////////////////////////////////////////////////////

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMXON(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR cr4.get_VMXE() || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  if (! BX_CPU_THIS_PTR in_vmx) {
    if (CPL != 0 || ! BX_CPU_THIS_PTR cr0.get_NE() ||
        ! (BX_CPU_THIS_PTR cr0.get_PE()) || BX_GET_ENABLE_A20() == 0 ||
        ! (BX_CPU_THIS_PTR msr.ia32_feature_ctrl & BX_IA32_FEATURE_CONTROL_LOCK_BIT) ||
        ! (BX_CPU_THIS_PTR msr.ia32_feature_ctrl & BX_IA32_FEATURE_CONTROL_VMX_ENABLE_BIT))
    {
      BX_ERROR(("#GP: VMXON is not allowed !"));
      exception(BX_GP_EXCEPTION, 0);
    }

    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    Bit64u pAddr = read_virtual_qword(i->seg(), eaddr); // keep 64-bit
    if (! IsValidPageAlignedPhyAddr(pAddr)) {
      BX_ERROR(("VMXON: invalid or not page aligned physical address !"));
      VMfailInvalid();
      BX_NEXT_INSTR(i);
    }

    // not allowed to be shadow VMCS
    Bit32u revision = VMXReadRevisionID((bx_phy_address) pAddr);
    if (revision != BX_CPU_THIS_PTR vmcs_map->get_vmcs_revision_id()) {
      BX_ERROR(("VMXON: not expected (%d != %d) VMCS revision id !", revision, BX_CPU_THIS_PTR vmcs_map->get_vmcs_revision_id()));
      VMfailInvalid();
      BX_NEXT_INSTR(i);
    }

    BX_CPU_THIS_PTR vmcsptr = BX_INVALID_VMCSPTR;
    BX_CPU_THIS_PTR vmcshostptr = 0;
    BX_CPU_THIS_PTR vmxonptr = pAddr;
    BX_CPU_THIS_PTR in_vmx = true;
    mask_event(BX_EVENT_INIT); // INIT is disabled in VMX root mode
    // block and disable A20M;

#if BX_SUPPORT_MONITOR_MWAIT
    BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

    VMsucceed();
  }
  else if (BX_CPU_THIS_PTR in_vmx_guest) { // in VMX non-root operation
    VMexit_Instruction(i, VMX_VMEXIT_VMXON);
  }
  else {
    // in VMX root operation mode
    if (CPL != 0) {
      BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }

    VMfail(VMXERR_VMXON_IN_VMX_ROOT_OPERATION);
  }
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMXOFF(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit(VMX_VMEXIT_VMXOFF, 0);
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

/*
        if dual-monitor treatment of SMIs and SMM is active
                THEN VMfail(VMXERR_VMXOFF_WITH_CONFIGURED_SMM_MONITOR);
        else
*/
  {
    BX_CPU_THIS_PTR vmxonptr = BX_INVALID_VMCSPTR;
    BX_CPU_THIS_PTR in_vmx = false;  // leave VMX operation mode
    unmask_event(BX_EVENT_INIT);
     // unblock and enable A20M;
#if BX_SUPPORT_MONITOR_MWAIT
    BX_CPU_THIS_PTR monitor.reset_monitor();
#endif
    VMsucceed();
  }
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMCALL(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx)
    exception(BX_UD_EXCEPTION, 0);

  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit(VMX_VMEXIT_VMCALL, 0);
  }

  if (BX_CPU_THIS_PTR get_VM() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_smm /*||
        (the logical processor does not support the dual-monitor treatment of SMIs and SMM) ||
        (the valid bit in the IA32_SMM_MONITOR_CTL MSR is clear)*/)
  {
    VMfail(VMXERR_VMCALL_IN_VMX_ROOT_OPERATION);
    BX_NEXT_TRACE(i);
  }
/*
        if dual-monitor treatment of SMIs and BX_CPU_THIS_PTR in_smm
                THEN perform an SMM VMexit (see Section 24.16.2
                     of the IntelR 64 and IA-32 Architectures Software Developer's Manual, Volume 3B);
*/
  if (BX_CPU_THIS_PTR vmcsptr == BX_INVALID_VMCSPTR) {
    BX_ERROR(("VMFAIL: VMCALL with invalid VMCS ptr"));
    VMfailInvalid();
    BX_NEXT_TRACE(i);
  }

  Bit32u launch_state = VMread32(VMCS_LAUNCH_STATE_FIELD_ENCODING);
  if (launch_state != VMCS_STATE_CLEAR) {
    BX_ERROR(("VMFAIL: VMCALL with launched VMCS"));
    VMfail(VMXERR_VMCALL_NON_CLEAR_VMCS);
    BX_NEXT_TRACE(i);
  }

  BX_PANIC(("VMCALL: not implemented yet"));
/*
  if VM-exit control fields are not valid (see Section 24.16.6.1 of the IntelR 64 and IA-32 Architectures Software Developer's Manual, Volume 3B)
      THEN VMfail(VMXERR_VMCALL_INVALID_VMEXIT_FIELD);
   else
      enter SMM;
      read revision identifier in MSEG;
      if revision identifier does not match that supported by processor
      THEN
          leave SMM;
          VMfailValid(VMXERR_VMCALL_INVALID_MSEG_REVISION_ID);
      else
          read SMM-monitor features field in MSEG (see Section 24.16.6.2,
          in the IntelR 64 and IA-32 Architectures Software Developer's Manual, Volume 3B);
          if features field is invalid
          THEN
              leave SMM;
              VMfailValid(VMXERR_VMCALL_WITH_INVALID_SMM_MONITOR_FEATURES);
          else activate dual-monitor treatment of SMIs and SMM (see Section 24.16.6
              in the IntelR 64 and IA-32 Architectures Software Developer's Manual, Volume 3B);
          FI;
      FI;
  FI;
*/
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMLAUNCH(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  bool vmlaunch = false;
  if ((i->getIaOpcode() == BX_IA_VMLAUNCH)) {
    BX_DEBUG(("VMLAUNCH VMCS ptr: 0x" FMT_ADDRX64, BX_CPU_THIS_PTR vmcsptr));
    vmlaunch = true;
  }
  else {
    BX_DEBUG(("VMRESUME VMCS ptr: 0x" FMT_ADDRX64, BX_CPU_THIS_PTR vmcsptr));
  }

  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit(vmlaunch ? VMX_VMEXIT_VMLAUNCH : VMX_VMEXIT_VMRESUME, 0);
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR vmcsptr == BX_INVALID_VMCSPTR) {
    BX_ERROR(("VMFAIL: VMLAUNCH with invalid VMCS ptr !"));
    VMfailInvalid();
    BX_NEXT_TRACE(i);
  }

  if (interrupts_inhibited(BX_INHIBIT_INTERRUPTS_BY_MOVSS)) {
    BX_ERROR(("VMFAIL: VMLAUNCH with interrupts blocked by MOV_SS !"));
    VMfail(VMXERR_VMENTRY_MOV_SS_BLOCKING);
    BX_NEXT_TRACE(i);
  }

  Bit32u launch_state = VMread32(VMCS_LAUNCH_STATE_FIELD_ENCODING);
  if (vmlaunch) {
    if (launch_state != VMCS_STATE_CLEAR) {
       BX_ERROR(("VMFAIL: VMLAUNCH with non-clear VMCS!"));
       VMfail(VMXERR_VMLAUNCH_NON_CLEAR_VMCS);
       BX_NEXT_TRACE(i);
    }
  }
  else {
    if (launch_state != VMCS_STATE_LAUNCHED) {
       BX_ERROR(("VMFAIL: VMRESUME with non-launched VMCS!"));
       VMfail(VMXERR_VMRESUME_NON_LAUNCHED_VMCS);
       BX_NEXT_TRACE(i);
    }
  }

  ///////////////////////////////////////////////////////
  // STEP 1: Load and Check VM-Execution Control Fields
  // STEP 2: Load and Check VM-Exit Control Fields
  // STEP 3: Load and Check VM-Entry Control Fields
  ///////////////////////////////////////////////////////

  VMX_error_code error = VMenterLoadCheckVmControls();
  if (error != VMXERR_NO_ERROR) {
    VMfail(error);
    BX_NEXT_TRACE(i);
  }

  ///////////////////////////////////////////////////////
  // STEP 4: Load and Check Host State
  ///////////////////////////////////////////////////////

  error = VMenterLoadCheckHostState();
  if (error != VMXERR_NO_ERROR) {
    VMfail(error);
    BX_NEXT_TRACE(i);
  }

  ///////////////////////////////////////////////////////
  // STEP 5: Load and Check Guest State
  ///////////////////////////////////////////////////////

  Bit64u qualification = VMENTER_ERR_NO_ERROR;
  Bit32u state_load_error = VMenterLoadCheckGuestState(&qualification);
  if (state_load_error) {
    BX_ERROR(("VMEXIT: Guest State Checks Failed"));
    VMexit(VMX_VMEXIT_VMENTRY_FAILURE_GUEST_STATE | (1 << 31), qualification);
  }

  Bit32u msr = LoadMSRs(BX_CPU_THIS_PTR vmcs.vmentry_msr_load_cnt, BX_CPU_THIS_PTR vmcs.vmentry_msr_load_addr);
  if (msr) {
    BX_ERROR(("VMEXIT: Error when loading guest MSR number %d", msr));
    VMexit(VMX_VMEXIT_VMENTRY_FAILURE_MSR | (1 << 31), msr);
  }

  ///////////////////////////////////////////////////////
  // STEP 6: Update VMCS 'launched' state
  ///////////////////////////////////////////////////////

  if (vmlaunch) VMwrite32(VMCS_LAUNCH_STATE_FIELD_ENCODING, VMCS_STATE_LAUNCHED);

/*
   Check settings of VMX controls and host-state area;
   if invalid settings
   THEN VMfailValid(VM entry with invalid VMX-control field(s)) or
        VMfailValid(VM entry with invalid host-state field(s)) or
        VMfailValid(VM entry with invalid executive-VMCS pointer)) or
        VMfailValid(VM entry with non-launched executive VMCS) or
        VMfailValid(VM entry with executive-VMCS pointer not VMXON pointer)
        VMfailValid(VM entry with invalid VM-execution control fields in executive VMCS)
   (as appropriate);
   else
        Attempt to load guest state and PDPTRs as appropriate;
        clear address-range monitoring;
        if failure in checking guest state or PDPTRs
        	THEN VM entry fails (see Section 22.7, in the IntelR 64 and IA-32 Architectures Software Developer's Manual, Volume 3B);
        else
                Attempt to load MSRs from VM-entry MSR-load area;
                if failure
                	THEN VM entry fails (see Section 22.7, in the IntelR 64 and IA-32 Architectures Software Developer's Manual, Volume 3B);
                else {
                        if VMLAUNCH
                        	THEN launch state of VMCS <== "launched";
                                if in SMM and "entry to SMM" VM-entry control is 0
                                THEN
                                	if "deactivate dual-monitor treatment" VM-entry control is 0
                                        	THEN SMM-transfer VMCS pointer <== current-VMCS pointer;
                                        FI;
                                        if executive-VMCS pointer is VMX pointer
                                        	THEN current-VMCS pointer <== VMCS-link pointer;
                                        else current-VMCS pointer <== executive-VMCS pointer;
                                FI;
                                leave SMM;
                        FI;
                        VMsucceed();
                }
         FI;
   FI;
*/

  BX_CPU_THIS_PTR in_vmx_guest = true;

  unmask_event(BX_EVENT_INIT);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_TSC_OFFSET))
    BX_CPU_THIS_PTR tsc_offset = VMread64(VMCS_64BIT_CONTROL_TSC_OFFSET);
  else
    BX_CPU_THIS_PTR tsc_offset = 0;

#if BX_SUPPORT_VMX >= 2
  if (PIN_VMEXIT(VMX_VM_EXEC_CTRL1_VMX_PREEMPTION_TIMER_VMEXIT)) {
    Bit32u timer_value = VMread32(VMCS_32BIT_GUEST_PREEMPTION_TIMER_VALUE);
    if (timer_value == 0) {
      signal_event(BX_EVENT_VMX_PREEMPTION_TIMER_EXPIRED);
    }
    else {
      // activate VMX preemption timer
      BX_DEBUG(("VMX preemption timer active"));
      BX_CPU_THIS_PTR lapic.set_vmx_preemption_timer(timer_value);
    }
  }
#endif

  ///////////////////////////////////////////////////////
  // STEP 7: Inject events to the guest
  ///////////////////////////////////////////////////////

  VMenterInjectEvents();

#if BX_SUPPORT_X86_64
  // - When virtual-interrupt-delivery is set this will cause PPR virtualization
  //   followed by Virtual Interrupt Evaluation
  // - When use TPR shadow together with Virtualize APIC Access are set this would
  //   cause TPR threshold check
  // - When Virtualize APIC Access is disabled the code would pass through TPR
  //   threshold check but no VMExit would occur (otherwise VMEntry should fail
  //   consistency checks before).
  if (VMEXIT(VMX_VM_EXEC_CTRL2_TPR_SHADOW)) {
    VMX_TPR_Virtualization();
  }
#endif

#endif // BX_SUPPORT_VMX

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMPTRLD(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit_Instruction(i, VMX_VMEXIT_VMPTRLD);
  }

  if (CPL != 0) {
    BX_ERROR(("VMPTRLD with CPL!=0 willcause #GP(0)"));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit64u pAddr = read_virtual_qword(i->seg(), eaddr); // keep 64-bit
  if (! IsValidPageAlignedPhyAddr(pAddr)) {
    BX_ERROR(("VMFAIL: invalid or not page aligned physical address !"));
    VMfail(VMXERR_VMPTRLD_INVALID_PHYSICAL_ADDRESS);
    BX_NEXT_INSTR(i);
  }

  if (pAddr == BX_CPU_THIS_PTR vmxonptr) {
    BX_ERROR(("VMFAIL: VMPTRLD with VMXON ptr !"));
    VMfail(VMXERR_VMPTRLD_WITH_VMXON_PTR);
  }
  else {
    Bit32u revision = VMXReadRevisionID((bx_phy_address) pAddr);

    if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_VMCS_SHADOWING))
      revision &= ~BX_VMCS_SHADOW_BIT_MASK; // allowed to be shadow VMCS

    if (revision != BX_CPU_THIS_PTR vmcs_map->get_vmcs_revision_id()) {
       BX_ERROR(("VMPTRLD: not expected (%d != %d) VMCS revision id !", revision, BX_CPU_THIS_PTR vmcs_map->get_vmcs_revision_id()));
       VMfail(VMXERR_VMPTRLD_INCORRECT_VMCS_REVISION_ID);
    }
    else {
       set_VMCSPTR(pAddr);
       VMsucceed();
    }
  }
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMPTRST(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit_Instruction(i, VMX_VMEXIT_VMPTRST);
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_qword(i->seg(), eaddr, BX_CPU_THIS_PTR vmcsptr);
  VMsucceed();
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_VMX

Bit64u BX_CPP_AttrRegparmN(1) BX_CPU_C::vmread(unsigned encoding)
{
  unsigned width = VMCS_FIELD_WIDTH(encoding);
  Bit64u field_64;

  if(width == VMCS_FIELD_WIDTH_16BIT) {
    field_64 = VMread16(encoding);
  }
  else if(width == VMCS_FIELD_WIDTH_32BIT) {
    // the real hardware write access rights stored in packed format
    if (encoding >= VMCS_32BIT_GUEST_ES_ACCESS_RIGHTS && encoding <= VMCS_32BIT_GUEST_TR_ACCESS_RIGHTS)
      field_64 = vmx_unpack_ar_field(VMread32(encoding), BX_CPU_THIS_PTR vmcs_map->get_access_rights_format());
    else
      field_64 = VMread32(encoding);
  }
  else if(width == VMCS_FIELD_WIDTH_64BIT) {
    if (IS_VMCS_FIELD_HI(encoding))
      field_64 = VMread32(encoding);
    else
      field_64 = VMread64(encoding);
  }
  else {
    field_64 = VMread_natural(encoding);
  }

  return field_64;
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::vmwrite(unsigned encoding, Bit64u val_64)
{
  unsigned width = VMCS_FIELD_WIDTH(encoding);
  Bit32u val_32 = GET32L(val_64);

  if(width == VMCS_FIELD_WIDTH_16BIT) {
    VMwrite16(encoding, val_32 & 0xffff);
  }
  else if(width == VMCS_FIELD_WIDTH_32BIT) {
    // the real hardware write access rights stored in packed format
    if (encoding >= VMCS_32BIT_GUEST_ES_ACCESS_RIGHTS && encoding <= VMCS_32BIT_GUEST_TR_ACCESS_RIGHTS)
      if (BX_CPU_THIS_PTR vmcs_map->get_access_rights_format() == VMCS_AR_PACK)
        VMwrite16(encoding, (Bit16u) vmx_pack_ar_field(val_32, VMCS_AR_PACK));
      else
        VMwrite32(encoding, vmx_pack_ar_field(val_32, BX_CPU_THIS_PTR vmcs_map->get_access_rights_format()));
    else
      VMwrite32(encoding, val_32);
  }
  else if(width == VMCS_FIELD_WIDTH_64BIT) {
    if (IS_VMCS_FIELD_HI(encoding))
      VMwrite32(encoding, val_32);
    else
      VMwrite64(encoding, val_64);
  }
  else {
    VMwrite_natural(encoding, (bx_address) val_64);
  }
}

#if BX_SUPPORT_VMX >= 2

Bit64u BX_CPP_AttrRegparmN(1) BX_CPU_C::vmread_shadow(unsigned encoding)
{
  unsigned width = VMCS_FIELD_WIDTH(encoding);
  Bit64u field_64;

  if(width == VMCS_FIELD_WIDTH_16BIT) {
    field_64 = VMread16_Shadow(encoding);
  }
  else if(width == VMCS_FIELD_WIDTH_32BIT) {
    // the real hardware write access rights stored in packed format
    if (encoding >= VMCS_32BIT_GUEST_ES_ACCESS_RIGHTS && encoding <= VMCS_32BIT_GUEST_TR_ACCESS_RIGHTS)
      field_64 = vmx_unpack_ar_field(VMread32_Shadow(encoding), BX_CPU_THIS_PTR vmcs_map->get_access_rights_format());
    else
      field_64 = VMread32_Shadow(encoding);
  }
  else if(width == VMCS_FIELD_WIDTH_64BIT) {
    if (IS_VMCS_FIELD_HI(encoding))
      field_64 = VMread32_Shadow(encoding);
    else
      field_64 = VMread64_Shadow(encoding);
  }
  else {
    field_64 = VMread64_Shadow(encoding);
  }

  return field_64;
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::vmwrite_shadow(unsigned encoding, Bit64u val_64)
{
  unsigned width = VMCS_FIELD_WIDTH(encoding);
  Bit32u val_32 = GET32L(val_64);

  if(width == VMCS_FIELD_WIDTH_16BIT) {
    VMwrite16_Shadow(encoding, val_32 & 0xffff);
  }
  else if(width == VMCS_FIELD_WIDTH_32BIT) {
    // the real hardware write access rights stored in packed format
    if (encoding >= VMCS_32BIT_GUEST_ES_ACCESS_RIGHTS && encoding <= VMCS_32BIT_GUEST_TR_ACCESS_RIGHTS)
      if (BX_CPU_THIS_PTR vmcs_map->get_access_rights_format() == VMCS_AR_PACK)
        VMwrite16_Shadow(encoding, (Bit16u) vmx_pack_ar_field(val_32, VMCS_AR_PACK));
      else
        VMwrite32_Shadow(encoding, vmx_pack_ar_field(val_32, BX_CPU_THIS_PTR vmcs_map->get_access_rights_format()));
    else
      VMwrite32_Shadow(encoding, val_32);
  }
  else if(width == VMCS_FIELD_WIDTH_64BIT) {
    if (IS_VMCS_FIELD_HI(encoding))
      VMwrite32_Shadow(encoding, val_32);
    else
      VMwrite64_Shadow(encoding, val_64);
  }
  else {
    VMwrite64_Shadow(encoding, val_64);
  }
}

#endif // BX_SUPPORT_VMX >= 2

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMREAD_EdGd(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  bx_phy_address vmcs_pointer = BX_CPU_THIS_PTR vmcsptr;

  if (BX_CPU_THIS_PTR in_vmx_guest) {
#if BX_SUPPORT_VMX >= 2
    if (Vmexit_Vmread(i))
#endif
      VMexit_Instruction(i, VMX_VMEXIT_VMREAD, BX_READ);

    vmcs_pointer = BX_CPU_THIS_PTR vmcs.vmcs_linkptr;
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (vmcs_pointer == BX_INVALID_VMCSPTR) {
    BX_ERROR(("VMFAIL: VMREAD with invalid VMCS ptr !"));
    VMfailInvalid();
    BX_NEXT_INSTR(i);
  }

  unsigned encoding = BX_READ_32BIT_REG(i->src());

  if (! BX_CPU_THIS_PTR vmcs_map->is_valid(encoding)) {
    BX_ERROR(("VMREAD: not supported field 0x%08x", encoding));
    VMfail(VMXERR_UNSUPPORTED_VMCS_COMPONENT_ACCESS);
    BX_NEXT_INSTR(i);
  }

  Bit32u field_32;
#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR in_vmx_guest)
    field_32 = (Bit32u) vmread_shadow(encoding);
  else
#endif
    field_32 = (Bit32u) vmread(encoding);

  if (i->modC0()) {
     BX_WRITE_32BIT_REGZ(i->dst(), field_32);
  }
  else {
     Bit32u eaddr = (Bit32u) BX_CPU_RESOLVE_ADDR(i);
     write_virtual_dword_32(i->seg(), eaddr, field_32);
  }

  VMsucceed();
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMREAD_EqGq(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  bx_phy_address vmcs_pointer = BX_CPU_THIS_PTR vmcsptr;

  if (BX_CPU_THIS_PTR in_vmx_guest) {
#if BX_SUPPORT_VMX >= 2
    if (Vmexit_Vmread(i))
#endif
      VMexit_Instruction(i, VMX_VMEXIT_VMREAD, BX_READ);

    vmcs_pointer = BX_CPU_THIS_PTR vmcs.vmcs_linkptr;
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (vmcs_pointer == BX_INVALID_VMCSPTR) {
    BX_ERROR(("VMFAIL: VMREAD with invalid VMCS ptr !"));
    VMfailInvalid();
    BX_NEXT_INSTR(i);
  }

  if (BX_READ_64BIT_REG_HIGH(i->src())) {
    BX_ERROR(("VMREAD: not supported field (upper 32-bit not zero)"));
    VMfail(VMXERR_UNSUPPORTED_VMCS_COMPONENT_ACCESS);
    BX_NEXT_INSTR(i);
  }
  unsigned encoding = BX_READ_32BIT_REG(i->src());

  if (! BX_CPU_THIS_PTR vmcs_map->is_valid(encoding)) {
    BX_ERROR(("VMREAD: not supported field 0x%08x", encoding));
    VMfail(VMXERR_UNSUPPORTED_VMCS_COMPONENT_ACCESS);
    BX_NEXT_INSTR(i);
  }

  Bit64u field_64;
#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR in_vmx_guest)
    field_64 = vmread_shadow(encoding);
  else
#endif
    field_64 = vmread(encoding);

  if (i->modC0()) {
     BX_WRITE_64BIT_REG(i->dst(), field_64);
  }
  else {
     Bit64u eaddr = BX_CPU_RESOLVE_ADDR(i);
     write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), field_64);
  }

  VMsucceed();
#endif

  BX_NEXT_INSTR(i);
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMWRITE_GdEd(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  bx_phy_address vmcs_pointer = BX_CPU_THIS_PTR vmcsptr;

  if (BX_CPU_THIS_PTR in_vmx_guest) {
#if BX_SUPPORT_VMX >= 2
    if (Vmexit_Vmwrite(i))
#endif
      VMexit_Instruction(i, VMX_VMEXIT_VMWRITE, BX_WRITE);

    vmcs_pointer = BX_CPU_THIS_PTR vmcs.vmcs_linkptr;
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (vmcs_pointer == BX_INVALID_VMCSPTR) {
    BX_ERROR(("VMFAIL: VMWRITE with invalid VMCS ptr !"));
    VMfailInvalid();
    BX_NEXT_INSTR(i);
  }

  Bit32u val_32;

  if (i->modC0()) {
     val_32 = BX_READ_32BIT_REG(i->src());
  }
  else {
     Bit32u eaddr = (Bit32u) BX_CPU_RESOLVE_ADDR(i);
     val_32 = read_virtual_dword_32(i->seg(), eaddr);
  }

  Bit32u encoding = BX_READ_32BIT_REG(i->dst());

  if (! BX_CPU_THIS_PTR vmcs_map->is_valid(encoding)) {
    BX_ERROR(("VMWRITE: not supported field 0x%08x", encoding));
    VMfail(VMXERR_UNSUPPORTED_VMCS_COMPONENT_ACCESS);
    BX_NEXT_INSTR(i);
  }

  if (VMCS_FIELD_TYPE(encoding) == VMCS_FIELD_TYPE_READ_ONLY)
  {
    if ((VMX_MSR_MISC & VMX_MISC_SUPPORT_VMWRITE_READ_ONLY_FIELDS) == 0) {
      BX_ERROR(("VMWRITE: write to read only field 0x%08x", encoding));
      VMfail(VMXERR_VMWRITE_READ_ONLY_VMCS_COMPONENT);
      BX_NEXT_INSTR(i);
    }
  }

#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR in_vmx_guest)
    vmwrite_shadow(encoding, (Bit64u) val_32);
  else
#endif
    vmwrite(encoding, (Bit64u) val_32);

  VMsucceed();
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMWRITE_GqEq(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  bx_phy_address vmcs_pointer = BX_CPU_THIS_PTR vmcsptr;

  if (BX_CPU_THIS_PTR in_vmx_guest) {
#if BX_SUPPORT_VMX >= 2
    if (Vmexit_Vmwrite(i))
#endif
      VMexit_Instruction(i, VMX_VMEXIT_VMWRITE, BX_WRITE);

    vmcs_pointer = BX_CPU_THIS_PTR vmcs.vmcs_linkptr;
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (vmcs_pointer == BX_INVALID_VMCSPTR) {
    BX_ERROR(("VMFAIL: VMWRITE with invalid VMCS ptr !"));
    VMfailInvalid();
    BX_NEXT_INSTR(i);
  }

  Bit64u val_64;

  if (i->modC0()) {
     val_64 = BX_READ_64BIT_REG(i->src());
  }
  else {
     Bit64u eaddr = BX_CPU_RESOLVE_ADDR(i);
     val_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  }

  if (BX_READ_64BIT_REG_HIGH(i->dst())) {
     BX_ERROR(("VMWRITE: not supported field (upper 32-bit not zero)"));
     VMfail(VMXERR_UNSUPPORTED_VMCS_COMPONENT_ACCESS);
     BX_NEXT_INSTR(i);
  }

  Bit32u encoding = BX_READ_32BIT_REG(i->dst());

  if (! BX_CPU_THIS_PTR vmcs_map->is_valid(encoding)) {
    BX_ERROR(("VMWRITE: not supported field 0x%08x", encoding));
    VMfail(VMXERR_UNSUPPORTED_VMCS_COMPONENT_ACCESS);
    BX_NEXT_INSTR(i);
  }

  if (VMCS_FIELD_TYPE(encoding) == VMCS_FIELD_TYPE_READ_ONLY)
  {
    if ((VMX_MSR_MISC & VMX_MISC_SUPPORT_VMWRITE_READ_ONLY_FIELDS) == 0) {
      BX_ERROR(("VMWRITE: write to read only field 0x%08x", encoding));
      VMfail(VMXERR_VMWRITE_READ_ONLY_VMCS_COMPONENT);
      BX_NEXT_INSTR(i);
    }
  }

#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR in_vmx_guest)
    vmwrite_shadow(encoding, val_64);
  else
#endif
    vmwrite(encoding, val_64);

  VMsucceed();
#endif

  BX_NEXT_INSTR(i);
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMCLEAR(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit_Instruction(i, VMX_VMEXIT_VMCLEAR);
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit64u pAddr = read_virtual_qword(i->seg(), eaddr); // keep 64-bit
  if (! IsValidPageAlignedPhyAddr(pAddr)) {
    BX_ERROR(("VMFAIL: VMCLEAR with invalid physical address!"));
    VMfail(VMXERR_VMCLEAR_WITH_INVALID_ADDR);
    BX_NEXT_INSTR(i);
  }

  if (pAddr == BX_CPU_THIS_PTR vmxonptr) {
    BX_ERROR(("VMFAIL: VMLEAR with VMXON ptr !"));
    VMfail(VMXERR_VMCLEAR_WITH_VMXON_VMCS_PTR);
  }
  else {
    // ensure that data for VMCS referenced by the operand is in memory
    // initialize implementation-specific data in VMCS region

    // clear VMCS launch state
    unsigned launch_field_offset = BX_CPU_THIS_PTR vmcs_map->vmcs_field_offset(VMCS_LAUNCH_STATE_FIELD_ENCODING);
    if(launch_field_offset >= VMX_VMCS_AREA_SIZE)
      BX_PANIC(("VMCLEAR: can't access VMCS_LAUNCH_STATE encoding, offset=0x%x", launch_field_offset));

    Bit32u launch_state = VMCS_STATE_CLEAR;
    access_write_physical(pAddr + launch_field_offset, 4, &launch_state);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr + launch_field_offset, 4,
            MEMTYPE(BX_CPU_THIS_PTR vmcs_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&launch_state));

    if (pAddr == BX_CPU_THIS_PTR vmcsptr) {
        BX_CPU_THIS_PTR vmcsptr = BX_INVALID_VMCSPTR;
        BX_CPU_THIS_PTR vmcshostptr = 0;
    }

    VMsucceed();
  }
#endif

  BX_NEXT_INSTR(i);
}

#if BX_CPU_LEVEL >= 6

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVEPT(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX >= 2
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit_Instruction(i, VMX_VMEXIT_INVEPT, BX_WRITE);
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address type;
  if (i->os64L()) {
    type = BX_READ_64BIT_REG(i->dst());
  }
  else {
    type = BX_READ_32BIT_REG(i->dst());
  }

  BxPackedXmmRegister inv_eptp;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  read_virtual_xmmword(i->seg(), eaddr, &inv_eptp);

  switch(type) {
  case BX_INVEPT_INVVPID_SINGLE_CONTEXT_INVALIDATION:
     if (! is_eptptr_valid(inv_eptp.xmm64u(0))) {
       BX_ERROR(("INVEPT: invalid EPTPTR value !"));
       VMfail(VMXERR_INVALID_INVEPT_INVVPID);
       BX_NEXT_TRACE(i);
     }
     TLB_flush(); // Invalidate mappings associated with EPTP[51:12]
     break;

  case BX_INVEPT_INVVPID_ALL_CONTEXT_INVALIDATION:
     TLB_flush(); // Invalidate mappings associated with all EPTPs
     break;

  default:
     BX_ERROR(("INVEPT: not supported type !"));
     VMfail(VMXERR_INVALID_INVEPT_INVVPID);
     BX_NEXT_TRACE(i);
  }

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_INVEPT, type);

  VMsucceed();
#else
  BX_INFO(("INVEPT: required VMXx2 support, use --enable-vmx=2 option"));
  exception(BX_UD_EXCEPTION, 0);
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVVPID(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX >= 2
  if (! BX_CPU_THIS_PTR in_vmx || ! protected_mode() || BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_COMPAT)
    exception(BX_UD_EXCEPTION, 0);

  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit_Instruction(i, VMX_VMEXIT_INVVPID, BX_WRITE);
  }

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address type;
  if (i->os64L()) {
    type = BX_READ_64BIT_REG(i->dst());
  }
  else {
    type = BX_READ_32BIT_REG(i->dst());
  }

  BxPackedXmmRegister invvpid_desc;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  read_virtual_xmmword(i->seg(), eaddr, &invvpid_desc);

  if (invvpid_desc.xmm64u(0) > 0xffff) {
    BX_ERROR(("INVVPID: INVVPID_DESC reserved bits set"));
    VMfail(VMXERR_INVALID_INVEPT_INVVPID);
    BX_NEXT_TRACE(i);
  }

  Bit16u vpid = invvpid_desc.xmm16u(0);
  if (vpid == 0 && type != BX_INVEPT_INVVPID_ALL_CONTEXT_INVALIDATION) {
    BX_ERROR(("INVVPID with VPID=0"));
    VMfail(VMXERR_INVALID_INVEPT_INVVPID);
    BX_NEXT_TRACE(i);
  }

  switch(type) {
  case BX_INVEPT_INVVPID_INDIVIDUAL_ADDRESS_INVALIDATION:
    if (! IsCanonical(invvpid_desc.xmm64u(1))) {
      BX_ERROR(("INVVPID: non canonical LADDR single context invalidation"));
      VMfail(VMXERR_INVALID_INVEPT_INVVPID);
      BX_NEXT_TRACE(i);
    }

    TLB_flush(); // invalidate all mappings for address LADDR tagged with VPID
    break;

  case BX_INVEPT_INVVPID_SINGLE_CONTEXT_INVALIDATION:
    TLB_flush(); // invalidate all mappings tagged with VPID
    break;

  case BX_INVEPT_INVVPID_ALL_CONTEXT_INVALIDATION:
    TLB_flush(); // invalidate all mappings tagged with VPID <> 0
    break;

  case BX_INVEPT_INVVPID_SINGLE_CONTEXT_NON_GLOBAL_INVALIDATION:
    TLB_flushNonGlobal(); // invalidate all mappings tagged with VPID except globals
    break;

  default:
    BX_ERROR(("INVVPID: not supported type !"));
    VMfail(VMXERR_INVALID_INVEPT_INVVPID);
    BX_NEXT_TRACE(i);
  }

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_INVVPID, type);

  VMsucceed();
#else
  BX_INFO(("INVVPID: required VMXx2 support, use --enable-vmx=2 option"));
  exception(BX_UD_EXCEPTION, 0);
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVPCID(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  // INVPCID will always #UD in legacy VMX mode, the #UD takes priority over any other exception the instruction may incur.
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_INVPCID)) {
       BX_ERROR(("INVPCID in VMX guest: not allowed to use instruction !"));
       exception(BX_UD_EXCEPTION, 0);
    }
  }
#endif

  if (v8086_mode()) {
    BX_ERROR(("INVPCID: #GP - not recognized in v8086 mode"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX >= 2
  // INVPCID will always #UD in legacy VMX mode
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (VMEXIT(VMX_VM_EXEC_CTRL2_INVLPG_VMEXIT)) {
      VMexit_Instruction(i, VMX_VMEXIT_INVPCID, BX_WRITE);
    }
  }
#endif

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address type;
#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    type = BX_READ_64BIT_REG(i->dst());
  }
  else
#endif
  {
    type = BX_READ_32BIT_REG(i->dst());
  }

  BxPackedXmmRegister invpcid_desc;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  read_virtual_xmmword(i->seg(), eaddr, &invpcid_desc);

  if (invpcid_desc.xmm64u(0) > 0xfff) {
    BX_ERROR(("INVPCID: INVPCID_DESC reserved bits set"));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit16u pcid = invpcid_desc.xmm16u(0) & 0xfff;

  switch(type) {
  case BX_INVPCID_INDIVIDUAL_ADDRESS_NON_GLOBAL_INVALIDATION:
#if BX_SUPPORT_X86_64
    if (! IsCanonical(invpcid_desc.xmm64u(1))) {
      BX_ERROR(("INVPCID: non canonical LADDR single context invalidation"));
      exception(BX_GP_EXCEPTION, 0);
    }
#endif
    if (! BX_CPU_THIS_PTR cr4.get_PCIDE() && pcid != 0) {
      BX_ERROR(("INVPCID: invalid PCID"));
      exception(BX_GP_EXCEPTION, 0);
    }
    TLB_flushNonGlobal(); // Invalidate all mappings for LADDR tagged with PCID except globals
    break;

  case BX_INVPCID_SINGLE_CONTEXT_NON_GLOBAL_INVALIDATION:
    if (! BX_CPU_THIS_PTR cr4.get_PCIDE() && pcid != 0) {
      BX_ERROR(("INVPCID: invalid PCID"));
      exception(BX_GP_EXCEPTION, 0);
    }
    TLB_flushNonGlobal(); // Invalidate all mappings tagged with PCID except globals
    break;

  case BX_INVPCID_ALL_CONTEXT_INVALIDATION:
    TLB_flush(); // Invalidate all mappings tagged with any PCID
    break;

  case BX_INVPCID_ALL_CONTEXT_NON_GLOBAL_INVALIDATION:
    TLB_flushNonGlobal(); // Invalidate all mappings tagged with any PCID except globals
    break;

  default:
    BX_ERROR(("INVPCID: not supported type !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_INVPCID, type);

  BX_NEXT_TRACE(i);
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::GETSEC(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (! BX_CPU_THIS_PTR cr4.get_SMXE())
    exception(BX_UD_EXCEPTION, 0);

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit(VMX_VMEXIT_GETSEC, 0);
  }
#endif

  BX_PANIC(("GETSEC: SMX is not implemented yet !"));
#endif

  BX_NEXT_TRACE(i);
}

#if BX_SUPPORT_VMX
void BX_CPU_C::register_vmx_state(bx_param_c *parent)
{
  if (! is_cpu_extension_supported(BX_ISA_VMX)) return;

  // register VMX state for save/restore param tree
  bx_list_c *vmx = new bx_list_c(parent, "VMX");

  BXRS_HEX_PARAM_FIELD(vmx, vmcsptr, BX_CPU_THIS_PTR vmcsptr);
  BXRS_HEX_PARAM_FIELD(vmx, vmxonptr, BX_CPU_THIS_PTR vmxonptr);
  BXRS_PARAM_BOOL(vmx, in_vmx, BX_CPU_THIS_PTR in_vmx);
  BXRS_PARAM_BOOL(vmx, in_vmx_guest, BX_CPU_THIS_PTR in_vmx_guest);
  BXRS_PARAM_BOOL(vmx, in_smm_vmx, BX_CPU_THIS_PTR in_smm_vmx);
  BXRS_PARAM_BOOL(vmx, in_smm_vmx_guest, BX_CPU_THIS_PTR in_smm_vmx_guest);

  bx_list_c *vmcache = new bx_list_c(vmx, "VMCS_CACHE");

  //
  // VM-Execution Control Fields
  //

  bx_list_c *vmexec_ctrls = new bx_list_c(vmcache, "VMEXEC_CTRLS");

  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vmexec_ctrls1, BX_CPU_THIS_PTR vmcs.vmexec_ctrls1);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vmexec_ctrls2, BX_CPU_THIS_PTR vmcs.vmexec_ctrls2);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vmexec_ctrls3, BX_CPU_THIS_PTR vmcs.vmexec_ctrls3);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_exceptions_bitmap, BX_CPU_THIS_PTR vmcs.vm_exceptions_bitmap);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, tsc_multiplier, BX_CPU_THIS_PTR vmcs.tsc_multiplier);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_pf_mask, BX_CPU_THIS_PTR vmcs.vm_pf_mask);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_pf_match, BX_CPU_THIS_PTR vmcs.vm_pf_match);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, io_bitmap_addr1, BX_CPU_THIS_PTR vmcs.io_bitmap_addr[0]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, io_bitmap_addr2, BX_CPU_THIS_PTR vmcs.io_bitmap_addr[1]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, msr_bitmap_addr, BX_CPU_THIS_PTR vmcs.msr_bitmap_addr);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_cr0_mask, BX_CPU_THIS_PTR vmcs.vm_cr0_mask);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_cr0_read_shadow, BX_CPU_THIS_PTR vmcs.vm_cr0_read_shadow);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_cr4_mask, BX_CPU_THIS_PTR vmcs.vm_cr4_mask);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_cr4_read_shadow, BX_CPU_THIS_PTR vmcs.vm_cr4_read_shadow);
  BXRS_DEC_PARAM_FIELD(vmexec_ctrls, vm_cr3_target_cnt, BX_CPU_THIS_PTR vmcs.vm_cr3_target_cnt);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_cr3_target_value1, BX_CPU_THIS_PTR vmcs.vm_cr3_target_value[0]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_cr3_target_value2, BX_CPU_THIS_PTR vmcs.vm_cr3_target_value[1]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_cr3_target_value3, BX_CPU_THIS_PTR vmcs.vm_cr3_target_value[2]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_cr3_target_value4, BX_CPU_THIS_PTR vmcs.vm_cr3_target_value[3]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vmcs_linkptr, BX_CPU_THIS_PTR vmcs.vmcs_linkptr);
#if BX_SUPPORT_X86_64
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, virtual_apic_page_addr, BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vm_tpr_threshold, BX_CPU_THIS_PTR vmcs.vm_tpr_threshold);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, apic_access_page, BX_CPU_THIS_PTR vmcs.apic_access_page);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, apic_access, BX_CPU_THIS_PTR vmcs.apic_access);
#endif
#if BX_SUPPORT_VMX >= 2
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eptptr, BX_CPU_THIS_PTR vmcs.eptptr);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vpid, BX_CPU_THIS_PTR vmcs.vpid);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, pml_address, BX_CPU_THIS_PTR vmcs.pml_address);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, pml_index, BX_CPU_THIS_PTR vmcs.pml_index);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, spptp, BX_CPU_THIS_PTR vmcs.spptp);
#endif
#if BX_SUPPORT_VMX >= 2
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, pause_loop_exiting_gap, BX_CPU_THIS_PTR vmcs.ple.pause_loop_exiting_gap);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, pause_loop_exiting_window, BX_CPU_THIS_PTR vmcs.ple.pause_loop_exiting_window);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, first_pause_time, BX_CPU_THIS_PTR vmcs.ple.first_pause_time);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, last_pause_time, BX_CPU_THIS_PTR vmcs.ple.last_pause_time);
#endif
#if BX_SUPPORT_VMX >= 2
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, svi, BX_CPU_THIS_PTR vmcs.svi);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, rvi, BX_CPU_THIS_PTR vmcs.rvi);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vppr, BX_CPU_THIS_PTR vmcs.vppr);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eoi_exit_bitmap0, BX_CPU_THIS_PTR vmcs.eoi_exit_bitmap[0]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eoi_exit_bitmap1, BX_CPU_THIS_PTR vmcs.eoi_exit_bitmap[1]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eoi_exit_bitmap2, BX_CPU_THIS_PTR vmcs.eoi_exit_bitmap[2]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eoi_exit_bitmap3, BX_CPU_THIS_PTR vmcs.eoi_exit_bitmap[3]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eoi_exit_bitmap4, BX_CPU_THIS_PTR vmcs.eoi_exit_bitmap[4]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eoi_exit_bitmap5, BX_CPU_THIS_PTR vmcs.eoi_exit_bitmap[5]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eoi_exit_bitmap6, BX_CPU_THIS_PTR vmcs.eoi_exit_bitmap[6]);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eoi_exit_bitmap7, BX_CPU_THIS_PTR vmcs.eoi_exit_bitmap[7]);
#endif
#if BX_SUPPORT_VMX >= 2
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vmread_bitmap_addr, BX_CPU_THIS_PTR vmcs.vmread_bitmap_addr);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, vmwrite_bitmap_addr, BX_CPU_THIS_PTR vmcs.vmwrite_bitmap_addr);
#endif
#if BX_SUPPORT_VMX >= 2
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, ve_info_addr, BX_CPU_THIS_PTR vmcs.ve_info_addr);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, eptp_index, BX_CPU_THIS_PTR vmcs.eptp_index);
  BXRS_HEX_PARAM_FIELD(vmexec_ctrls, xss_exiting_bitmap, BX_CPU_THIS_PTR vmcs.xss_exiting_bitmap);
#endif

  //
  // VM-Exit Control Fields
  //

  bx_list_c *vmexit_ctrls = new bx_list_c(vmcache, "VMEXIT_CTRLS");

  BXRS_HEX_PARAM_FIELD(vmexit_ctrls, vmexit_ctrls, BX_CPU_THIS_PTR vmcs.vmexit_ctrls);
  BXRS_DEC_PARAM_FIELD(vmexit_ctrls, vmexit_msr_store_cnt, BX_CPU_THIS_PTR vmcs.vmexit_msr_store_cnt);
  BXRS_HEX_PARAM_FIELD(vmexit_ctrls, vmexit_msr_store_addr, BX_CPU_THIS_PTR vmcs.vmexit_msr_store_addr);
  BXRS_DEC_PARAM_FIELD(vmexit_ctrls, vmexit_msr_load_cnt, BX_CPU_THIS_PTR vmcs.vmexit_msr_load_cnt);
  BXRS_HEX_PARAM_FIELD(vmexit_ctrls, vmexit_msr_load_addr, BX_CPU_THIS_PTR vmcs.vmexit_msr_load_addr);

  //
  // VM-Entry Control Fields
  //

  bx_list_c *vmentry_ctrls = new bx_list_c(vmcache, "VMENTRY_CTRLS");

  BXRS_HEX_PARAM_FIELD(vmentry_ctrls, vmentry_ctrls, BX_CPU_THIS_PTR vmcs.vmentry_ctrls);
  BXRS_DEC_PARAM_FIELD(vmentry_ctrls, vmentry_msr_load_cnt, BX_CPU_THIS_PTR vmcs.vmentry_msr_load_cnt);
  BXRS_HEX_PARAM_FIELD(vmentry_ctrls, vmentry_msr_load_addr, BX_CPU_THIS_PTR vmcs.vmentry_msr_load_addr);
  BXRS_HEX_PARAM_FIELD(vmentry_ctrls, vmentry_interr_info, BX_CPU_THIS_PTR vmcs.vmentry_interr_info);
  BXRS_HEX_PARAM_FIELD(vmentry_ctrls, vmentry_excep_err_code, BX_CPU_THIS_PTR vmcs.vmentry_excep_err_code);
  BXRS_HEX_PARAM_FIELD(vmentry_ctrls, vmentry_instr_length, BX_CPU_THIS_PTR vmcs.vmentry_instr_length);

  //
  // VMCS Host State
  //

  bx_list_c *host = new bx_list_c(vmcache, "HOST_STATE");

#undef NEED_CPU_REG_SHORTCUTS

  BXRS_HEX_PARAM_FIELD(host, CR0, BX_CPU_THIS_PTR vmcs.host_state.cr0);
  BXRS_HEX_PARAM_FIELD(host, CR3, BX_CPU_THIS_PTR vmcs.host_state.cr3);
  BXRS_HEX_PARAM_FIELD(host, CR4, BX_CPU_THIS_PTR vmcs.host_state.cr4);
  BXRS_HEX_PARAM_FIELD(host, ES, BX_CPU_THIS_PTR vmcs.host_state.segreg_selector[BX_SEG_REG_ES]);
  BXRS_HEX_PARAM_FIELD(host, CS, BX_CPU_THIS_PTR vmcs.host_state.segreg_selector[BX_SEG_REG_CS]);
  BXRS_HEX_PARAM_FIELD(host, SS, BX_CPU_THIS_PTR vmcs.host_state.segreg_selector[BX_SEG_REG_SS]);
  BXRS_HEX_PARAM_FIELD(host, DS, BX_CPU_THIS_PTR vmcs.host_state.segreg_selector[BX_SEG_REG_DS]);
  BXRS_HEX_PARAM_FIELD(host, FS, BX_CPU_THIS_PTR vmcs.host_state.segreg_selector[BX_SEG_REG_FS]);
  BXRS_HEX_PARAM_FIELD(host, FS_BASE, BX_CPU_THIS_PTR vmcs.host_state.fs_base);
  BXRS_HEX_PARAM_FIELD(host, GS, BX_CPU_THIS_PTR vmcs.host_state.segreg_selector[BX_SEG_REG_GS]);
  BXRS_HEX_PARAM_FIELD(host, GS_BASE, BX_CPU_THIS_PTR vmcs.host_state.gs_base);
  BXRS_HEX_PARAM_FIELD(host, GDTR_BASE, BX_CPU_THIS_PTR vmcs.host_state.gdtr_base);
  BXRS_HEX_PARAM_FIELD(host, IDTR_BASE, BX_CPU_THIS_PTR vmcs.host_state.idtr_base);
  BXRS_HEX_PARAM_FIELD(host, TR, BX_CPU_THIS_PTR vmcs.host_state.tr_selector);
  BXRS_HEX_PARAM_FIELD(host, TR_BASE, BX_CPU_THIS_PTR vmcs.host_state.tr_base);
  BXRS_HEX_PARAM_FIELD(host, RSP, BX_CPU_THIS_PTR vmcs.host_state.rsp);
  BXRS_HEX_PARAM_FIELD(host, RIP, BX_CPU_THIS_PTR vmcs.host_state.rip);
  BXRS_HEX_PARAM_FIELD(host, sysenter_esp_msr, BX_CPU_THIS_PTR vmcs.host_state.sysenter_esp_msr);
  BXRS_HEX_PARAM_FIELD(host, sysenter_eip_msr, BX_CPU_THIS_PTR vmcs.host_state.sysenter_eip_msr);
  BXRS_HEX_PARAM_FIELD(host, sysenter_cs_msr, BX_CPU_THIS_PTR vmcs.host_state.sysenter_cs_msr);
#if BX_SUPPORT_VMX >= 2
  BXRS_HEX_PARAM_FIELD(host, pat_msr, BX_CPU_THIS_PTR vmcs.host_state.pat_msr);
#if BX_SUPPORT_X86_64
  BXRS_HEX_PARAM_FIELD(host, efer_msr, BX_CPU_THIS_PTR vmcs.host_state.efer_msr);
#endif
#endif
#if BX_SUPPORT_CET
  BXRS_HEX_PARAM_FIELD(host, ia32_s_cet_msr, BX_CPU_THIS_PTR vmcs.host_state.msr_ia32_s_cet);
  BXRS_HEX_PARAM_FIELD(host, SSP, BX_CPU_THIS_PTR vmcs.host_state.ssp);
  BXRS_HEX_PARAM_FIELD(host, interrupt_ssp_table_address, BX_CPU_THIS_PTR vmcs.host_state.interrupt_ssp_table_address);
#endif
#if BX_SUPPORT_PKEYS
  BXRS_HEX_PARAM_FIELD(host, pkrs, BX_CPU_THIS_PTR vmcs.host_state.pkrs);
#endif
}

#endif // BX_SUPPORT_VMX
