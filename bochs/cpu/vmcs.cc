/////////////////////////////////////////////////////////////////////////
// $Id: vmcs.cc,v 1.7 2011-01-10 22:37:05 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2009-2010 Stanislav Shwartsman
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

#if BX_SUPPORT_VMX

static unsigned vmcs_map[16][1+VMX_HIGHEST_VMCS_ENCODING];

void BX_CPU_C::init_VMCS(void)
{
  static bx_bool vmcs_map_ready = 0;
  unsigned type, field;

  if (vmcs_map_ready) return;
  vmcs_map_ready = 1;

  for (type=0; type<16; type++) {
    for (field=0; field <= VMX_HIGHEST_VMCS_ENCODING; field++) {
       vmcs_map[type][field] = 0xffffffff;
    }
  }

#if 1
  // try to build generic VMCS map
  for (type=0; type<16; type++) {
    for (field=0; field <= VMX_HIGHEST_VMCS_ENCODING; field++) {
       unsigned encoding = ((type & 0xc) << 11) + ((type & 3) << 10) + field;
       if (vmcs_map[type][field] != 0xffffffff) {
          BX_PANIC(("VMCS type %d field %d (encoding = 0x%08x) is already initialized", type, field, encoding));
       }
       if (vmcs_field_supported(encoding)) {
         // allocate 64 fields (4 byte each) per type
         vmcs_map[type][field] = VMCS_DATA_OFFSET + (type*64 + field) * 4;
         if(vmcs_map[type][field] >= VMX_VMCS_AREA_SIZE) {
            BX_PANIC(("VMCS type %d field %d (encoding = 0x%08x) is out of VMCS boundaries", type, field, encoding));
         }
       }
    }
  }
#else
  // define your own VMCS format
#include "vmcs.h"
#endif
}

#define VMCS_ENCODING_RESERVED_BITS (0xffff9000)

unsigned vmcs_field_offset(Bit32u encoding)
{
  if (encoding & VMCS_ENCODING_RESERVED_BITS)
    return 0xffffffff;

  unsigned field = VMCS_FIELD(encoding);
  if (field >= VMX_HIGHEST_VMCS_ENCODING)
    return 0xffffffff;

  return vmcs_map[VMCS_FIELD_INDEX(encoding)][field];
}

bx_bool BX_CPU_C::vmcs_field_supported(Bit32u encoding)
{
  switch(encoding)
  {
#if BX_SUPPORT_VMX >= 2
    /* VMCS 16-bit control fields */
    /* binary 0000_00xx_xxxx_xxx0 */
    case VMCS_16BIT_CONTROL_VPID:
      return 1;
#endif

    /* VMCS 16-bit guest-state fields */
    /* binary 0000_10xx_xxxx_xxx0 */
    case VMCS_16BIT_GUEST_ES_SELECTOR:
    case VMCS_16BIT_GUEST_CS_SELECTOR:
    case VMCS_16BIT_GUEST_SS_SELECTOR:
    case VMCS_16BIT_GUEST_DS_SELECTOR:
    case VMCS_16BIT_GUEST_FS_SELECTOR:
    case VMCS_16BIT_GUEST_GS_SELECTOR:
    case VMCS_16BIT_GUEST_LDTR_SELECTOR:
    case VMCS_16BIT_GUEST_TR_SELECTOR:
      return 1;

    /* VMCS 16-bit host-state fields */
    /* binary 0000_11xx_xxxx_xxx0 */
    case VMCS_16BIT_HOST_ES_SELECTOR:
    case VMCS_16BIT_HOST_CS_SELECTOR:
    case VMCS_16BIT_HOST_SS_SELECTOR:
    case VMCS_16BIT_HOST_DS_SELECTOR:
    case VMCS_16BIT_HOST_FS_SELECTOR:
    case VMCS_16BIT_HOST_GS_SELECTOR:
    case VMCS_16BIT_HOST_TR_SELECTOR:
      return 1;

    /* VMCS 32_bit control fields */
    /* binary 0100_00xx_xxxx_xxx0 */
    case VMCS_32BIT_CONTROL_PIN_BASED_EXEC_CONTROLS:
    case VMCS_32BIT_CONTROL_PROCESSOR_BASED_VMEXEC_CONTROLS:
    case VMCS_32BIT_CONTROL_EXECUTION_BITMAP:
    case VMCS_32BIT_CONTROL_PAGE_FAULT_ERR_CODE_MASK:
    case VMCS_32BIT_CONTROL_PAGE_FAULT_ERR_CODE_MATCH:
    case VMCS_32BIT_CONTROL_CR3_TARGET_COUNT:
    case VMCS_32BIT_CONTROL_VMEXIT_CONTROLS:
    case VMCS_32BIT_CONTROL_VMEXIT_MSR_STORE_COUNT:
    case VMCS_32BIT_CONTROL_VMEXIT_MSR_LOAD_COUNT:
    case VMCS_32BIT_CONTROL_VMENTRY_CONTROLS:
    case VMCS_32BIT_CONTROL_VMENTRY_MSR_LOAD_COUNT:
    case VMCS_32BIT_CONTROL_VMENTRY_INTERRUPTION_INFO:
    case VMCS_32BIT_CONTROL_VMENTRY_EXCEPTION_ERR_CODE:
    case VMCS_32BIT_CONTROL_VMENTRY_INSTRUCTION_LENGTH:
#if BX_SUPPORT_X86_64
    case VMCS_32BIT_CONTROL_TPR_THRESHOLD:
#endif
#if BX_SUPPORT_VMX >= 2
    case VMCS_32BIT_CONTROL_SECONDARY_VMEXEC_CONTROLS:
#endif
      return 1;

    /* VMCS 32-bit read only data fields */
    /* binary 0100_01xx_xxxx_xxx0 */
    case VMCS_32BIT_INSTRUCTION_ERROR:
    case VMCS_32BIT_VMEXIT_REASON:
    case VMCS_32BIT_VMEXIT_INTERRUPTION_INFO:
    case VMCS_32BIT_VMEXIT_INTERRUPTION_ERR_CODE:
    case VMCS_32BIT_IDT_VECTORING_INFO:
    case VMCS_32BIT_IDT_VECTORING_ERR_CODE:
    case VMCS_32BIT_VMEXIT_INSTRUCTION_LENGTH:
    case VMCS_32BIT_VMEXIT_INSTRUCTION_INFO:
      return 1;

    /* VMCS 32-bit guest-state fields */
    /* binary 0100_10xx_xxxx_xxx0 */
    case VMCS_32BIT_GUEST_ES_LIMIT:
    case VMCS_32BIT_GUEST_CS_LIMIT:
    case VMCS_32BIT_GUEST_SS_LIMIT:
    case VMCS_32BIT_GUEST_DS_LIMIT:
    case VMCS_32BIT_GUEST_FS_LIMIT:
    case VMCS_32BIT_GUEST_GS_LIMIT:
    case VMCS_32BIT_GUEST_LDTR_LIMIT:
    case VMCS_32BIT_GUEST_TR_LIMIT:
    case VMCS_32BIT_GUEST_GDTR_LIMIT:
    case VMCS_32BIT_GUEST_IDTR_LIMIT:
    case VMCS_32BIT_GUEST_ES_ACCESS_RIGHTS:
    case VMCS_32BIT_GUEST_CS_ACCESS_RIGHTS:
    case VMCS_32BIT_GUEST_SS_ACCESS_RIGHTS:
    case VMCS_32BIT_GUEST_DS_ACCESS_RIGHTS:
    case VMCS_32BIT_GUEST_FS_ACCESS_RIGHTS:
    case VMCS_32BIT_GUEST_GS_ACCESS_RIGHTS:
    case VMCS_32BIT_GUEST_LDTR_ACCESS_RIGHTS:
    case VMCS_32BIT_GUEST_TR_ACCESS_RIGHTS:
    case VMCS_32BIT_GUEST_INTERRUPTIBILITY_STATE:
    case VMCS_32BIT_GUEST_ACTIVITY_STATE:
    case VMCS_32BIT_GUEST_SMBASE:
    case VMCS_32BIT_GUEST_IA32_SYSENTER_CS_MSR:
      return 1;

    /* VMCS 32-bit host-state fields */
    /* binary 0100_11xx_xxxx_xxx0 */
    case VMCS_32BIT_HOST_IA32_SYSENTER_CS_MSR:
      return 1;

    /* VMCS 64-bit control fields */
    /* binary 0010_00xx_xxxx_xxx0 */
    case VMCS_64BIT_CONTROL_IO_BITMAP_A:
    case VMCS_64BIT_CONTROL_IO_BITMAP_A_HI:
    case VMCS_64BIT_CONTROL_IO_BITMAP_B:
    case VMCS_64BIT_CONTROL_IO_BITMAP_B_HI:
    case VMCS_64BIT_CONTROL_MSR_BITMAPS:
    case VMCS_64BIT_CONTROL_MSR_BITMAPS_HI:
    case VMCS_64BIT_CONTROL_VMEXIT_MSR_STORE_ADDR:
    case VMCS_64BIT_CONTROL_VMEXIT_MSR_STORE_ADDR_HI:
    case VMCS_64BIT_CONTROL_VMEXIT_MSR_LOAD_ADDR:
    case VMCS_64BIT_CONTROL_VMEXIT_MSR_LOAD_ADDR_HI:
    case VMCS_64BIT_CONTROL_VMENTRY_MSR_LOAD_ADDR:
    case VMCS_64BIT_CONTROL_VMENTRY_MSR_LOAD_ADDR_HI:
    case VMCS_64BIT_CONTROL_EXECUTIVE_VMCS_PTR:
    case VMCS_64BIT_CONTROL_EXECUTIVE_VMCS_PTR_HI:
    case VMCS_64BIT_CONTROL_TSC_OFFSET:
    case VMCS_64BIT_CONTROL_TSC_OFFSET_HI:
#if BX_SUPPORT_X86_64
    case VMCS_64BIT_CONTROL_VIRTUAL_APIC_PAGE_ADDR:
    case VMCS_64BIT_CONTROL_VIRTUAL_APIC_PAGE_ADDR_HI:
#endif
#if BX_SUPPORT_VMX >= 2
    case VMCS_64BIT_CONTROL_APIC_ACCESS_ADDR:
    case VMCS_64BIT_CONTROL_APIC_ACCESS_ADDR_HI:
    case VMCS_64BIT_CONTROL_EPTPTR:
    case VMCS_64BIT_CONTROL_EPTPTR_HI:
#endif
      return 1;

#if BX_SUPPORT_VMX >= 2
    /* VMCS 64-bit read only data fields */
    /* binary 0010_01xx_xxxx_xxx0 */
    case VMCS_64BIT_GUEST_PHYSICAL_ADDR:
    case VMCS_64BIT_GUEST_PHYSICAL_ADDR_HI:
      return 1;
#endif

    /* VMCS 64-bit guest state fields */
    /* binary 0010_10xx_xxxx_xxx0 */
    case VMCS_64BIT_GUEST_LINK_POINTER:
    case VMCS_64BIT_GUEST_LINK_POINTER_HI:
    case VMCS_64BIT_GUEST_IA32_DEBUGCTL:
    case VMCS_64BIT_GUEST_IA32_DEBUGCTL_HI:
#if BX_SUPPORT_VMX >= 2
    case VMCS_64BIT_GUEST_IA32_PAT:
    case VMCS_64BIT_GUEST_IA32_PAT_HI:
    case VMCS_64BIT_GUEST_IA32_EFER:
    case VMCS_64BIT_GUEST_IA32_EFER_HI:
    case VMCS_64BIT_GUEST_IA32_PDPTE0:
    case VMCS_64BIT_GUEST_IA32_PDPTE0_HI:
    case VMCS_64BIT_GUEST_IA32_PDPTE1:
    case VMCS_64BIT_GUEST_IA32_PDPTE1_HI:
    case VMCS_64BIT_GUEST_IA32_PDPTE2:
    case VMCS_64BIT_GUEST_IA32_PDPTE2_HI:
    case VMCS_64BIT_GUEST_IA32_PDPTE3:
    case VMCS_64BIT_GUEST_IA32_PDPTE3_HI:
#endif
      return 1;

#if BX_SUPPORT_VMX >= 2
    /* VMCS 64-bit host state fields */
    /* binary 0010_11xx_xxxx_xxx0 */
    case VMCS_64BIT_HOST_IA32_PAT:
    case VMCS_64BIT_HOST_IA32_PAT_HI:
    case VMCS_64BIT_HOST_IA32_EFER:
    case VMCS_64BIT_HOST_IA32_EFER_HI:
      return 1;
#endif

    /* VMCS natural width control fields */
    /* binary 0110_00xx_xxxx_xxx0 */
    case VMCS_CONTROL_CR0_GUEST_HOST_MASK:
    case VMCS_CONTROL_CR4_GUEST_HOST_MASK:
    case VMCS_CONTROL_CR0_READ_SHADOW:
    case VMCS_CONTROL_CR4_READ_SHADOW:
    case VMCS_CR3_TARGET0:
    case VMCS_CR3_TARGET1:
    case VMCS_CR3_TARGET2:
    case VMCS_CR3_TARGET3:
      return 1;

    /* VMCS natural width read only data fields */
    /* binary 0110_01xx_xxxx_xxx0 */
    case VMCS_VMEXIT_QUALIFICATION:
    case VMCS_IO_RCX:
    case VMCS_IO_RSI:
    case VMCS_IO_RDI:
    case VMCS_IO_RIP:
    case VMCS_GUEST_LINEAR_ADDR:
      return 1;

    /* VMCS natural width guest state fields */
    /* binary 0110_10xx_xxxx_xxx0 */
    case VMCS_GUEST_CR0:
    case VMCS_GUEST_CR3:
    case VMCS_GUEST_CR4:
    case VMCS_GUEST_ES_BASE:
    case VMCS_GUEST_CS_BASE:
    case VMCS_GUEST_SS_BASE:
    case VMCS_GUEST_DS_BASE:
    case VMCS_GUEST_FS_BASE:
    case VMCS_GUEST_GS_BASE:
    case VMCS_GUEST_LDTR_BASE:
    case VMCS_GUEST_TR_BASE:
    case VMCS_GUEST_GDTR_BASE:
    case VMCS_GUEST_IDTR_BASE:
    case VMCS_GUEST_DR7:
    case VMCS_GUEST_RSP:
    case VMCS_GUEST_RIP:
    case VMCS_GUEST_RFLAGS:
    case VMCS_GUEST_PENDING_DBG_EXCEPTIONS:
    case VMCS_GUEST_IA32_SYSENTER_ESP_MSR:
    case VMCS_GUEST_IA32_SYSENTER_EIP_MSR:
      return 1;

    /* VMCS natural width host state fields */
    /* binary 0110_11xx_xxxx_xxx0 */
    case VMCS_HOST_CR0:
    case VMCS_HOST_CR3:
    case VMCS_HOST_CR4:
    case VMCS_HOST_FS_BASE:
    case VMCS_HOST_GS_BASE:
    case VMCS_HOST_TR_BASE:
    case VMCS_HOST_GDTR_BASE:
    case VMCS_HOST_IDTR_BASE:
    case VMCS_HOST_IA32_SYSENTER_ESP_MSR:
    case VMCS_HOST_IA32_SYSENTER_EIP_MSR:
    case VMCS_HOST_RSP:
    case VMCS_HOST_RIP:
      return 1;

    default:
      return 0;
  }

  return 0;
}

#endif
