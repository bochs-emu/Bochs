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

#define LOG_THIS BX_CPU(0)->

#if BX_SUPPORT_VMX

const Bit32u VMCS_REVISION_ID_FIELD_ADDR  = 0x0000;
const Bit32u VMCS_VMX_ABORT_FIELD_ADDR    = 0x0004;
const Bit32u VMCS_LAUNCH_STATE_FIELD_ADDR = 0x0008;

VMCS_Mapping::VMCS_Mapping(Bit32u revision): revision_id(revision), ar_format(VMCS_AR_ROTATE)
{
  clear();
  init_generic_mapping();
}

VMCS_Mapping::VMCS_Mapping(Bit32u revision, const char *filename): revision_id(revision), ar_format(VMCS_AR_ROTATE)
{
  clear();

  // read mapping from file
  BX_PANIC(("Reading VMCS mapping from file not implemented yet"));

  init_generic_mapping(); // for now
}

BX_CPP_INLINE Bit32u vmcs_encoding(Bit32u type, Bit32u field)
{
  Bit32u encoding = ((type & 0xc) << 11) + ((type & 0x3) << 10) + field;
  return encoding;
}

void VMCS_Mapping::init_generic_mapping()
{
  vmcs_revision_id_field_offset = VMCS_REVISION_ID_FIELD_ADDR;
  vmx_abort_field_offset = VMCS_VMX_ABORT_FIELD_ADDR;
  vmcs_launch_state_field_offset = VMCS_LAUNCH_STATE_FIELD_ADDR;

  // try to build generic VMCS map
  // 16 types, 48 encodings (0x30), 4 bytes each field => 3072 bytes
  // reserve VMCS_DATA_OFFSET bytes in the beginning for special (hidden) VMCS fields
  for (unsigned type=0; type<16; type++) {
    for (unsigned field=0; field < VMX_HIGHEST_VMCS_ENCODING; field++) {
       Bit32u encoding = vmcs_encoding(type, field);
       if (vmcs_map[type][field] != 0xffffffff) {
          BX_PANIC(("VMCS type %d field %d (encoding = 0x%08x) is already initialized", type, field, encoding));
       }
       // allocate 64 fields (4 byte each) per type (even more than 48 which is required now)
       vmcs_map[type][field] = VMCS_DATA_OFFSET + (type*64 + field) * 4;
       if(vmcs_map[type][field] >= VMX_VMCS_AREA_SIZE) {
          BX_PANIC(("VMCS type %d field %d (encoding = 0x%08x) is out of VMCS boundaries", type, field, encoding));
       }
       BX_DEBUG(("VMCS field 0x%08x located at 0x%08x", encoding, vmcs_map[type][field]));
    }
  }
}

void VMCS_Mapping::clear()
{
  for (unsigned type=0; type<16; type++) {
    for (unsigned field=0; field < VMX_HIGHEST_VMCS_ENCODING; field++) {
       vmcs_map[type][field] = 0xffffffff;
    }
  }
}

bool VMCS_Mapping::clear_mapping(Bit32u encoding)
{
  return set_mapping(encoding, 0xffffffff);
}

bool VMCS_Mapping::set_mapping(Bit32u encoding, Bit32u offset)
{
  if (is_reserved(encoding))
    return false;

  unsigned field = VMCS_FIELD(encoding);
  if (field >= VMX_HIGHEST_VMCS_ENCODING)
    return false;

  vmcs_map[VMCS_FIELD_INDEX(encoding)][field] = offset;
  return true;
}

unsigned VMCS_Mapping::vmcs_field_offset(Bit32u encoding) const
{
  if (is_reserved(encoding)) {
    switch(encoding) {
      case VMCS_REVISION_ID_FIELD_ENCODING:  return vmcs_revision_id_field_offset;
      case VMCS_VMX_ABORT_FIELD_ENCODING:    return vmx_abort_field_offset;
      case VMCS_LAUNCH_STATE_FIELD_ENCODING: return vmcs_launch_state_field_offset;
    }
    return 0xffffffff;
  }

  unsigned field = VMCS_FIELD(encoding);
  if (field >= VMX_HIGHEST_VMCS_ENCODING)
    return 0xffffffff;

  return vmcs_map[VMCS_FIELD_INDEX(encoding)][field];
}

void BX_CPU_C::init_VMCS(void)
{
  BX_CPU_THIS_PTR vmcs_map = BX_CPU_THIS_PTR cpuid->get_vmcs();

  init_vmx_capabilities();

  static bool vmcs_map_ready = false;
  if (vmcs_map_ready) return;
  vmcs_map_ready = true;

  // disable not supported encodings
  for (unsigned type=0; type<16; type++) {
    for (unsigned field=0; field <= VMX_HIGHEST_VMCS_ENCODING; field++) {
      Bit32u encoding = vmcs_encoding(type, field);
      if (! vmcs_field_supported(encoding)) {
        BX_CPU_THIS_PTR vmcs_map->clear_mapping(encoding);
        BX_DEBUG(("VMCS field 0x%08x is not supported", encoding));
      }
    }
  }
}

#undef LOG_THIS
#define LOG_THIS BX_CPU_THIS_PTR

bool BX_CPU_C::vmcs_field_supported(Bit32u encoding)
{
  switch(encoding)
  {
#if BX_SUPPORT_VMX >= 2
    /* VMCS 16-bit control fields */
    /* binary 0000_00xx_xxxx_xxx0 */
    case VMCS_16BIT_CONTROL_VPID:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_VPID);

    case VMCS_16BIT_CONTROL_EPTP_INDEX:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT_EXCEPTION);
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

#if BX_SUPPORT_VMX >= 2
    case VMCS_16BIT_GUEST_INTERRUPT_STATUS:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_VINTR_DELIVERY);

    case VMCS_16BIT_GUEST_PML_INDEX:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_PML);
#endif

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
      return 1;

#if BX_SUPPORT_X86_64
    case VMCS_32BIT_CONTROL_TPR_THRESHOLD:
      if (is_cpu_extension_supported(BX_ISA_LONG_MODE))
        return BX_SUPPORT_VMX_EXTENSION(BX_VMX_TPR_SHADOW);
      else
        return 0;
#endif

    case VMCS_32BIT_CONTROL_SECONDARY_VMEXEC_CONTROLS:
      return BX_CPU_THIS_PTR vmx_cap.vmx_vmexec_ctrl2_supported_bits;

#if BX_SUPPORT_VMX >= 2
    case VMCS_32BIT_CONTROL_PAUSE_LOOP_EXITING_GAP:
    case VMCS_32BIT_CONTROL_PAUSE_LOOP_EXITING_WINDOW:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_PAUSE_LOOP_EXITING);
#endif

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

#if BX_SUPPORT_VMX >= 2    
    case VMCS_32BIT_GUEST_PREEMPTION_TIMER_VALUE:
       return BX_SUPPORT_VMX_EXTENSION(BX_VMX_PREEMPTION_TIMER);
#endif

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
      return 1;

#if BX_SUPPORT_VMX >= 2
    case VMCS_64BIT_CONTROL_PML_ADDRESS:
    case VMCS_64BIT_CONTROL_PML_ADDRESS_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_PML);
#endif

    case VMCS_64BIT_CONTROL_TSC_OFFSET:
    case VMCS_64BIT_CONTROL_TSC_OFFSET_HI:
      return 1;

#if BX_SUPPORT_X86_64
    case VMCS_64BIT_CONTROL_VIRTUAL_APIC_PAGE_ADDR:
    case VMCS_64BIT_CONTROL_VIRTUAL_APIC_PAGE_ADDR_HI:
      if (is_cpu_extension_supported(BX_ISA_LONG_MODE))
        return BX_SUPPORT_VMX_EXTENSION(BX_VMX_TPR_SHADOW);
      else
        return 0;

    case VMCS_64BIT_CONTROL_APIC_ACCESS_ADDR:
    case VMCS_64BIT_CONTROL_APIC_ACCESS_ADDR_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_APIC_VIRTUALIZATION);
#endif

#if BX_SUPPORT_VMX >= 2
    case VMCS_64BIT_CONTROL_VMFUNC_CTRLS:
    case VMCS_64BIT_CONTROL_VMFUNC_CTRLS_HI:
      return BX_CPU_THIS_PTR vmx_cap.vmx_vmfunc_supported_bits != 0;

    case VMCS_64BIT_CONTROL_EPTPTR:
    case VMCS_64BIT_CONTROL_EPTPTR_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT);

    case VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP0:
    case VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP0_HI:
    case VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP1:
    case VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP1_HI:
    case VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP2:
    case VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP2_HI:
    case VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP3:
    case VMCS_64BIT_CONTROL_EOI_EXIT_BITMAP3_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_VINTR_DELIVERY);

    case VMCS_64BIT_CONTROL_EPTP_LIST_ADDRESS:
    case VMCS_64BIT_CONTROL_EPTP_LIST_ADDRESS_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPTP_SWITCHING);

    case VMCS_64BIT_CONTROL_VMREAD_BITMAP_ADDR:
    case VMCS_64BIT_CONTROL_VMREAD_BITMAP_ADDR_HI:
    case VMCS_64BIT_CONTROL_VMWRITE_BITMAP_ADDR:
    case VMCS_64BIT_CONTROL_VMWRITE_BITMAP_ADDR_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_VMCS_SHADOWING);

    case VMCS_64BIT_CONTROL_VE_EXCEPTION_INFO_ADDR:
    case VMCS_64BIT_CONTROL_VE_EXCEPTION_INFO_ADDR_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT_EXCEPTION);

    case VMCS_64BIT_CONTROL_XSS_EXITING_BITMAP:
    case VMCS_64BIT_CONTROL_XSS_EXITING_BITMAP_HI:
      return is_cpu_extension_supported(BX_ISA_XSAVES);

    case VMCS_64BIT_CONTROL_SPPTP:
    case VMCS_64BIT_CONTROL_SPPTP_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_SPP);
#endif

    case VMCS_64BIT_CONTROL_TSC_MULTIPLIER:
    case VMCS_64BIT_CONTROL_TSC_MULTIPLIER_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_TSC_SCALING);

#if BX_SUPPORT_VMX >= 2
    /* VMCS 64-bit read only data fields */
    /* binary 0010_01xx_xxxx_xxx0 */
    case VMCS_64BIT_GUEST_PHYSICAL_ADDR:
    case VMCS_64BIT_GUEST_PHYSICAL_ADDR_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT);
#endif

    /* VMCS 64-bit guest state fields */
    /* binary 0010_10xx_xxxx_xxx0 */
    case VMCS_64BIT_GUEST_LINK_POINTER:
    case VMCS_64BIT_GUEST_LINK_POINTER_HI:
    case VMCS_64BIT_GUEST_IA32_DEBUGCTL:
    case VMCS_64BIT_GUEST_IA32_DEBUGCTL_HI:
      return 1;

#if BX_SUPPORT_VMX >= 2
    case VMCS_64BIT_GUEST_IA32_PAT:
    case VMCS_64BIT_GUEST_IA32_PAT_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_PAT);

    case VMCS_64BIT_GUEST_IA32_EFER:
    case VMCS_64BIT_GUEST_IA32_EFER_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_EFER);
#endif

    case VMCS_64BIT_GUEST_IA32_PERF_GLOBAL_CTRL:
    case VMCS_64BIT_GUEST_IA32_PERF_GLOBAL_CTRL_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_PERF_GLOBAL_CTRL);

#if BX_SUPPORT_VMX >= 2
    case VMCS_64BIT_GUEST_IA32_PDPTE0:
    case VMCS_64BIT_GUEST_IA32_PDPTE0_HI:
    case VMCS_64BIT_GUEST_IA32_PDPTE1:
    case VMCS_64BIT_GUEST_IA32_PDPTE1_HI:
    case VMCS_64BIT_GUEST_IA32_PDPTE2:
    case VMCS_64BIT_GUEST_IA32_PDPTE2_HI:
    case VMCS_64BIT_GUEST_IA32_PDPTE3:
    case VMCS_64BIT_GUEST_IA32_PDPTE3_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT);
#endif

#if BX_SUPPORT_PKEYS
    case VMCS_64BIT_GUEST_IA32_PKRS:
    case VMCS_64BIT_GUEST_IA32_PKRS_HI:
      return is_cpu_extension_supported(BX_ISA_PKS);
#endif

#if BX_SUPPORT_VMX >= 2
    /* VMCS 64-bit host state fields */
    /* binary 0010_11xx_xxxx_xxx0 */
    case VMCS_64BIT_HOST_IA32_PAT:
    case VMCS_64BIT_HOST_IA32_PAT_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_PAT);

    case VMCS_64BIT_HOST_IA32_EFER:
    case VMCS_64BIT_HOST_IA32_EFER_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_EFER);
#endif

    case VMCS_64BIT_HOST_IA32_PERF_GLOBAL_CTRL:
    case VMCS_64BIT_HOST_IA32_PERF_GLOBAL_CTRL_HI:
      return BX_SUPPORT_VMX_EXTENSION(BX_VMX_PERF_GLOBAL_CTRL);

#if BX_SUPPORT_PKEYS
    case VMCS_64BIT_HOST_IA32_PKRS:
    case VMCS_64BIT_HOST_IA32_PKRS_HI:
      return is_cpu_extension_supported(BX_ISA_PKS);
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
    case VMCS_GUEST_IA32_S_CET:
    case VMCS_GUEST_SSP:
    case VMCS_GUEST_INTERRUPT_SSP_TABLE_ADDR:
      return is_cpu_extension_supported(BX_ISA_CET);

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
    case VMCS_HOST_IA32_S_CET:
    case VMCS_HOST_SSP:
    case VMCS_HOST_INTERRUPT_SSP_TABLE_ADDR:
      return is_cpu_extension_supported(BX_ISA_CET);

    default:
      return 0;
  }

  return 0;
}

void BX_CPU_C::init_vmx_capabilities(void)
{
  // initialization order is important !
#if BX_SUPPORT_VMX >= 2
  init_ept_vpid_capabilities();
  init_vmfunc_capabilities();
#endif
  init_pin_based_vmexec_ctrls();
  init_secondary_proc_based_vmexec_ctrls();
  init_primary_proc_based_vmexec_ctrls();
  init_vmexit_ctrls();
  init_vmentry_ctrls();
}

#if BX_SUPPORT_VMX >= 2
void BX_CPU_C::init_ept_vpid_capabilities(void)
{
  struct bx_VMX_Cap *cap = &BX_CPU_THIS_PTR vmx_cap;

  // EPT/VPID capabilities
  // -----------------------------------------------------------
  //  [0] - BX_EPT_ENTRY_EXECUTE_ONLY support
  //  [6] - 4-levels EPT page walk length
  //  [8] - allow UC EPT paging structure memory type
  // [14] - allow WB EPT paging structure memory type
  // [16] - EPT 2M pages support
  // [17] - EPT 1G pages support
  // [20] - INVEPT instruction supported
  // [21] - EPT A/D bits supported
  // [22] - advanced VM-exit information for EPT violations (not implemented yet)
  // [23] - Enable Shadow Stack control bit is supported in EPTP (CET)
  // [25] - INVEPT single-context invalidation supported
  // [26] - INVEPT all-context invalidation supported
  // [32] - INVVPID instruction supported
  // [40] - individual-address INVVPID is supported
  // [41] - single-context INVVPID is supported
  // [42] - all-context INVVPID is supported
  // [43] - single-context-retaining-globals INVVPID is supported

  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT)) {
    cap->vmx_ept_vpid_cap_supported_bits = BX_CONST64(0x06114141);
    if (is_cpu_extension_supported(BX_ISA_1G_PAGES))
      cap->vmx_ept_vpid_cap_supported_bits |= (1 << 17);
    if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT_ACCESS_DIRTY))
      cap->vmx_ept_vpid_cap_supported_bits |= (1 << 21);
    if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET))
      cap->vmx_ept_vpid_cap_supported_bits |= (1 << 23);
  }
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_VPID))
    cap->vmx_ept_vpid_cap_supported_bits |= BX_CONST64(0x00000f01) << 32;
}

void BX_CPU_C::init_vmfunc_capabilities(void)
{
  struct bx_VMX_Cap *cap = &BX_CPU_THIS_PTR vmx_cap;

  // vm functions
  // -----------------------------------------------------------
  //    [00] EPTP switching
  // [63-01] reserved

  cap->vmx_vmfunc_supported_bits = 0;

  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPTP_SWITCHING))
    cap->vmx_vmfunc_supported_bits |= VMX_VMFUNC_EPTP_SWITCHING_MASK;
}
#endif

void BX_CPU_C::init_pin_based_vmexec_ctrls(void)
{
  struct bx_VMX_Cap *cap = &BX_CPU_THIS_PTR vmx_cap;

  // pin based vm exec controls
  // -----------------------------------------------------------
  //   [00] External Interrupt Exiting
  // 1 [01] Reserved (must be '1)
  // 1 [02] Reserved (must be '1)
  //   [03] NMI Exiting
  // 1 [04] Reserved (must be '1)
  //   [05] Virtual NMI (require Virtual NMI support)
  //   [06] Activate VMX Preemption Timer (require VMX Preemption Timer support)
  //   [07] Process Posted interrupts

  cap->vmx_pin_vmexec_ctrl_supported_bits =
       VMX_VM_EXEC_CTRL1_EXTERNAL_INTERRUPT_VMEXIT |
       VMX_VM_EXEC_CTRL1_NMI_EXITING;
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_VIRTUAL_NMI))
    cap->vmx_pin_vmexec_ctrl_supported_bits |= VMX_VM_EXEC_CTRL1_VIRTUAL_NMI;
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_PREEMPTION_TIMER))
    cap->vmx_pin_vmexec_ctrl_supported_bits |= VMX_VM_EXEC_CTRL1_VMX_PREEMPTION_TIMER_VMEXIT;
#endif
}

void BX_CPU_C::init_primary_proc_based_vmexec_ctrls(void)
{
  struct bx_VMX_Cap *cap = &BX_CPU_THIS_PTR vmx_cap;

  // proc based vm exec controls
  // -----------------------------------------------------------
  // 0    [00] Reserved (must be '0)
  // 1    [01] Reserved (must be '1)
  //      [02] Interrupt Window Exiting
  //      [03] TSC Offset Enable
  // 1 [06-04] Reserved (must be '1)
  //      [07] HLT Exiting
  // 1    [08] Reserved (must be '1)
  //      [09] INVLPG Exiting
  //      [10] MWAIT Exiting (require MONITOR/MWAIT support)
  //      [11] RDPMC Exiting
  //      [12] RDTSC Exiting
  // 1 [14-13] Reserved (must be '1)
  // 1    [15] CR3 Write Exiting (legacy must be '1, introduced with EPT support)
  // 1    [16] CR3 Read Exiting (legacy must be '1, introduced with EPT support)
  // 0 [18-17] Reserved (must be '0)
  //      [19] CR8 Write Exiting (require TPR Shadow support, require x86-64 support)
  //      [20] CR8 Read Exiting (require TPR Shadow support, require x86-64 support)
  //      [21] TPR Shadow Enable (require TPR Shadow support, require x86-64 support)
  //      [22] NMI Window Exiting (require Virtual NMI support)
  //      [23] DRx Access Exiting
  //      [24] I/O Access Exiting
  //      [25] I/O Bitmaps
  // 1    [26] Reserved (must be '1)
  //      [27] Monitor Trap Flag Enable (require Monitor Trap Flag support)
  //      [28] MSR Bitmaps
  //      [29] MONITOR Exiting (require MONITOR/MWAIT support)
  //      [30] PAUSE Exiting
  //      [31] Secondary proc-based vmexec controls

  cap->vmx_proc_vmexec_ctrl_supported_bits =
       VMX_VM_EXEC_CTRL2_INTERRUPT_WINDOW_VMEXIT |
       VMX_VM_EXEC_CTRL2_TSC_OFFSET |
       VMX_VM_EXEC_CTRL2_HLT_VMEXIT |
       VMX_VM_EXEC_CTRL2_INVLPG_VMEXIT |
       VMX_VM_EXEC_CTRL2_RDPMC_VMEXIT |
       VMX_VM_EXEC_CTRL2_RDTSC_VMEXIT |
       VMX_VM_EXEC_CTRL2_DRx_ACCESS_VMEXIT |
       VMX_VM_EXEC_CTRL2_IO_VMEXIT |
       VMX_VM_EXEC_CTRL2_IO_BITMAPS |
       VMX_VM_EXEC_CTRL2_MSR_BITMAPS |
       VMX_VM_EXEC_CTRL2_PAUSE_VMEXIT;

#if BX_SUPPORT_MONITOR_MWAIT
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_MONITOR_MWAIT)) {
    cap->vmx_proc_vmexec_ctrl_supported_bits |=
       VMX_VM_EXEC_CTRL2_MWAIT_VMEXIT | VMX_VM_EXEC_CTRL2_MONITOR_VMEXIT;
  }
#endif
#if BX_SUPPORT_X86_64
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_TPR_SHADOW)) {
    cap->vmx_proc_vmexec_ctrl_supported_bits |= VMX_VM_EXEC_CTRL2_TPR_SHADOW;
    if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_LONG_MODE))
      cap->vmx_proc_vmexec_ctrl_supported_bits |=
          VMX_VM_EXEC_CTRL2_CR8_WRITE_VMEXIT | VMX_VM_EXEC_CTRL2_CR8_READ_VMEXIT;
  }
#endif
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_VIRTUAL_NMI))
    cap->vmx_proc_vmexec_ctrl_supported_bits |= VMX_VM_EXEC_CTRL2_NMI_WINDOW_EXITING;
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_MONITOR_TRAP_FLAG))
    cap->vmx_proc_vmexec_ctrl_supported_bits |= VMX_VM_EXEC_CTRL2_MONITOR_TRAP_FLAG;
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT)) {
    cap->vmx_proc_vmexec_ctrl_supported_bits |=
        VMX_VM_EXEC_CTRL2_CR3_WRITE_VMEXIT | VMX_VM_EXEC_CTRL2_CR3_READ_VMEXIT;
  }
#endif

  // enable secondary vm exec controls if there are secondary proc-based vmexec controls present
  if (cap->vmx_vmexec_ctrl2_supported_bits != 0)
    cap->vmx_proc_vmexec_ctrl_supported_bits |= VMX_VM_EXEC_CTRL2_SECONDARY_CONTROLS;
}

void BX_CPU_C::init_secondary_proc_based_vmexec_ctrls(void)
{
  struct bx_VMX_Cap *cap = &BX_CPU_THIS_PTR vmx_cap;

  // secondary proc based vm exec controls
  // -----------------------------------------------------------
  //   [00] Apic Virtualization (require x86-64 for TPR shadow)
  //   [01] EPT Enable (require x86-64 for 4-level page walk)
  //   [02] Descriptor Table Exiting
  //   [03] Enable RDTSCP instruction (RDTSCP will #UD if not set)
  //   [04] Virtualize X2APIC Mode (doesn't require actual X2APIC to be enabled)
  //   [05] VPID Enable
  //   [06] WBINVD Exiting
  //   [07] Unrestricted Guest (require EPT)
  //   [08] Virtualize Apic Registers
  //   [09] Virtualize Interrupt Delivery
  //   [10] PAUSE Loop Exiting
  //   [11] RDRAND Exiting (require RDRAND instruction support)
  //   [12] Enable INVPCID instruction (require INVPCID instruction support)
  //   [13] Enable VM Functions
  //   [14] Enable VMCS Shadowing
  //   [15] Reserved (must be '0)
  //   [16] RDSEED Exiting (require RDSEED instruction support)
  //   [17] Page Modification Logging Enable
  //   [18] Support for EPT Violation (#VE) exception
  //   [19] Reserved (must be '0)
  //   [20] XSAVES Exiting
  //   [21] Reserved (must be '0)
  //   [22] Mode Based Execution Control (MBE) (not implemented yet)
  //   [23] Sub Page Protection
  //   [24] Reserved (must be '0)
  //   [25] Enable TSC Scaling

  cap->vmx_vmexec_ctrl2_supported_bits = 0;

#if BX_SUPPORT_X86_64
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_APIC_VIRTUALIZATION))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES;
#endif
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_EPT_ENABLE;
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_DESCRIPTOR_TABLE_EXIT))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_DESCRIPTOR_TABLE_VMEXIT;
#endif
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_RDTSCP))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_RDTSCP;
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_X2APIC_VIRTUALIZATION))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_VIRTUALIZE_X2APIC_MODE;
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_VPID))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_VPID_ENABLE;
#endif
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_WBINVD_VMEXIT))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_WBINVD_VMEXIT;
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_UNRESTRICTED_GUEST))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_UNRESTRICTED_GUEST;
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_VINTR_DELIVERY))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_REGISTERS | VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY;
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_PAUSE_LOOP_EXITING))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_PAUSE_LOOP_VMEXIT;
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_INVPCID))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_INVPCID;
#endif
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_RDRAND))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_RDRAND_VMEXIT;
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_VMCS_SHADOWING))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_VMCS_SHADOWING;
#endif
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_RDSEED))
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_RDSEED_VMEXIT;
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_PML)) {
    if (! BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT))
      BX_PANIC(("VMX PML feature requires EPT support !"));
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_PML_ENABLE;
  }
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT_EXCEPTION)) {
    if (! BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPTP_SWITCHING))
      BX_PANIC(("#VE exception feature requires EPTP switching support !"));
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_EPT_VIOLATION_EXCEPTION;
  }
#endif
#if BX_SUPPORT_VMX >= 2
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_XSAVES)) {
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_XSAVES_XRSTORS;
  }
#endif
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_SPP)) {
    if (! BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT))
      BX_PANIC(("VMX SPP feature requires EPT support !"));
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_SUBPAGE_WR_PROTECT_CTRL;
  }
#endif
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_TSC_SCALING)) {
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_TSC_SCALING;
  }

#if BX_SUPPORT_VMX >= 2
  // enable vm functions secondary vmexec control if there are supported vmfunctions
  if (cap->vmx_vmfunc_supported_bits != 0)
    cap->vmx_vmexec_ctrl2_supported_bits |= VMX_VM_EXEC_CTRL3_VMFUNC_ENABLE;
#endif
}

void BX_CPU_C::init_vmexit_ctrls(void)
{
  struct bx_VMX_Cap *cap = &BX_CPU_THIS_PTR vmx_cap;

  // vmexit controls
  // -----------------------------------------------------------
  // 1 [01-00] Reserved (must be '1)
  // 1    [02] Save guest MSR_DEBUGCTL on VMEXIT (legacy must be '1)
  // 1 [08-03] Reserved (must be '1)
  //      [09] Host Address Space Size (x86-64 host, x86-64 support required)
  // 1 [11-10] Reserved (must be '1)
  //      [12] Load host MSR_PERF_GLOBAL_CTRL on VMEXIT
  // 1 [14-13] Reserved (must be '1)
  //      [15] Acknowledge interrupts on VMEXIT
  // 1 [17-16] Reserved (must be '1)
  //      [18] Save guest MSR_PAT on VMEXIT
  //      [19] Load host MSR_PAT on VMEXIT
  //      [20] Save guest MSR_EFER on VMEXIT
  //      [21] Load host MSR_EFER on VMEXIT
  //      [22] Save VMX preemption timer counter on VMEXIT
  //      [28] Save host CET state on VMEXIT
  //      [29] Save host MSR_IA32_PKRS on VMEXIT

  cap->vmx_vmexit_ctrl_supported_bits = 
      VMX_VMEXIT_CTRL1_INTA_ON_VMEXIT | VMX_VMEXIT_CTRL1_SAVE_DBG_CTRLS;

#if BX_SUPPORT_X86_64
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_LONG_MODE))
    cap->vmx_vmexit_ctrl_supported_bits |= VMX_VMEXIT_CTRL1_HOST_ADDR_SPACE_SIZE;
#endif
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_PERF_GLOBAL_CTRL))
    cap->vmx_vmexit_ctrl_supported_bits |= VMX_VMEXIT_CTRL1_LOAD_PERF_GLOBAL_CTRL_MSR;
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_PAT)) {
    cap->vmx_vmexit_ctrl_supported_bits |=
      VMX_VMEXIT_CTRL1_STORE_PAT_MSR | VMX_VMEXIT_CTRL1_LOAD_PAT_MSR;
  }
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EFER)) {
    cap->vmx_vmexit_ctrl_supported_bits |=
      VMX_VMEXIT_CTRL1_STORE_EFER_MSR | VMX_VMEXIT_CTRL1_LOAD_EFER_MSR;
  }
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_PREEMPTION_TIMER))
    cap->vmx_vmexit_ctrl_supported_bits |= VMX_VMEXIT_CTRL1_STORE_VMX_PREEMPTION_TIMER;
#endif
#if BX_SUPPORT_CET
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET))
    cap->vmx_vmexit_ctrl_supported_bits |= VMX_VMEXIT_CTRL1_LOAD_HOST_CET_STATE;
#endif
#if BX_SUPPORT_PKEYS
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_PKS))
    cap->vmx_vmexit_ctrl_supported_bits |= VMX_VMEXIT_CTRL1_LOAD_HOST_PKRS;
#endif
}

void BX_CPU_C::init_vmentry_ctrls(void)
{
  struct bx_VMX_Cap *cap = &BX_CPU_THIS_PTR vmx_cap;

  // vmentry controls
  // -----------------------------------------------------------
  // 1 [01-00] Reserved (must be '1)
  // 1    [02] Load MSR_DEBUGCTL on VMEXIT (legacy must be '1)
  // 1 [08-03] Reserved (must be '1)
  //      [09] x86-64 guest (x86-64 support required)
  //      [10] Enter to SMM mode
  //      [11] Deactivate Dual SMM Monitor treatment
  // 1    [12] Reserved (must be '1)
  //      [13] Load guest MSR_PERF_GLOBAL_CTRL
  //      [14] Load guest MSR_PAT
  //      [15] Load guest MSR_EFER
  //      [17] Load guest CET state
  //      [22] Load guest MSR_IA32_PKRS value

  cap->vmx_vmentry_ctrl_supported_bits = VMX_VMENTRY_CTRL1_LOAD_DBG_CTRLS |
                                         VMX_VMENTRY_CTRL1_SMM_ENTER |
                                         VMX_VMENTRY_CTRL1_DEACTIVATE_DUAL_MONITOR_TREATMENT;

#if BX_SUPPORT_X86_64
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_LONG_MODE))
    cap->vmx_vmentry_ctrl_supported_bits |= VMX_VMENTRY_CTRL1_X86_64_GUEST;
#endif
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_PERF_GLOBAL_CTRL))
    cap->vmx_vmentry_ctrl_supported_bits |= VMX_VMENTRY_CTRL1_LOAD_PERF_GLOBAL_CTRL_MSR;
#if BX_SUPPORT_VMX >= 2
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_PAT))
    cap->vmx_vmentry_ctrl_supported_bits |= VMX_VMENTRY_CTRL1_LOAD_PAT_MSR;
  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EFER))
    cap->vmx_vmentry_ctrl_supported_bits |= VMX_VMENTRY_CTRL1_LOAD_EFER_MSR;
#endif
#if BX_SUPPORT_CET
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET))
    cap->vmx_vmentry_ctrl_supported_bits |= VMX_VMENTRY_CTRL1_LOAD_GUEST_CET_STATE;
#endif
#if BX_SUPPORT_PKEYS
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_PKS))
    cap->vmx_vmentry_ctrl_supported_bits |= VMX_VMENTRY_CTRL1_LOAD_GUEST_PKRS;
#endif
}

#endif // BX_SUPPORT_VMX
