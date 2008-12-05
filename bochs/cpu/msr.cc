/////////////////////////////////////////////////////////////////////////
// $Id: msr.cc,v 1.1 2008-12-05 12:48:36 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008 Stanislav Shwartsman
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDMSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  if (!real_mode() && CPL!=0) {
    BX_ERROR(("RDMSR: CPL!=0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  /* We have the requested MSR register in ECX */
  switch(ECX) {

#if BX_SUPPORT_SEP
    case BX_MSR_SYSENTER_CS:
      RAX = BX_CPU_THIS_PTR msr.sysenter_cs_msr;
      RDX = 0;
      break;

    case BX_MSR_SYSENTER_ESP:
      RAX = BX_CPU_THIS_PTR msr.sysenter_esp_msr;
      RDX = 0;
      break;

    case BX_MSR_SYSENTER_EIP:
      RAX = BX_CPU_THIS_PTR msr.sysenter_eip_msr;
      RDX = 0;
      break;
#endif

#if BX_SUPPORT_MTRR
    case BX_MSR_MTRRCAP:   // read only MSR
      RAX = 0x508;
      RDX = 0;
      break;

    case BX_MSR_MTRRPHYSBASE0:
    case BX_MSR_MTRRPHYSMASK0:
    case BX_MSR_MTRRPHYSBASE1:
    case BX_MSR_MTRRPHYSMASK1:
    case BX_MSR_MTRRPHYSBASE2:
    case BX_MSR_MTRRPHYSMASK2:
    case BX_MSR_MTRRPHYSBASE3:
    case BX_MSR_MTRRPHYSMASK3:
    case BX_MSR_MTRRPHYSBASE4:
    case BX_MSR_MTRRPHYSMASK4:
    case BX_MSR_MTRRPHYSBASE5:
    case BX_MSR_MTRRPHYSMASK5:
    case BX_MSR_MTRRPHYSBASE6:
    case BX_MSR_MTRRPHYSMASK6:
    case BX_MSR_MTRRPHYSBASE7:
    case BX_MSR_MTRRPHYSMASK7:
      RAX = BX_CPU_THIS_PTR msr.mtrrphys[ECX - BX_MSR_MTRRPHYSBASE0] & 0xffffffff;
      RDX = BX_CPU_THIS_PTR msr.mtrrphys[ECX - BX_MSR_MTRRPHYSBASE0] >> 32;
      break;

    case BX_MSR_MTRRFIX64K_00000:
      RAX = BX_CPU_THIS_PTR msr.mtrrfix64k_00000 & 0xffffffff;
      RDX = BX_CPU_THIS_PTR msr.mtrrfix64k_00000 >> 32;
      break;
    case BX_MSR_MTRRFIX16K_80000:
      RAX = BX_CPU_THIS_PTR msr.mtrrfix16k_80000 & 0xffffffff;
      RDX = BX_CPU_THIS_PTR msr.mtrrfix16k_80000 >> 32;
      break;
    case BX_MSR_MTRRFIX16K_A0000:
      RAX = BX_CPU_THIS_PTR msr.mtrrfix16k_a0000 & 0xffffffff;
      RDX = BX_CPU_THIS_PTR msr.mtrrfix16k_a0000 >> 32;
      break;

    case BX_MSR_MTRRFIX4K_C0000:
    case BX_MSR_MTRRFIX4K_C8000:
    case BX_MSR_MTRRFIX4K_D0000:
    case BX_MSR_MTRRFIX4K_D8000:
    case BX_MSR_MTRRFIX4K_E0000:
    case BX_MSR_MTRRFIX4K_E8000:
    case BX_MSR_MTRRFIX4K_F0000:
    case BX_MSR_MTRRFIX4K_F8000:
      RAX = BX_CPU_THIS_PTR msr.mtrrfix4k[ECX - BX_MSR_MTRRFIX4K_C0000] & 0xffffffff;
      RDX = BX_CPU_THIS_PTR msr.mtrrfix4k[ECX - BX_MSR_MTRRFIX4K_C0000] >> 32;
      break;

    case BX_MSR_PAT:
      RAX = BX_CPU_THIS_PTR msr.pat & 0xffffffff;
      RDX = BX_CPU_THIS_PTR msr.pat >> 32;
      break;

    case BX_MSR_MTRR_DEFTYPE:
      RAX = BX_CPU_THIS_PTR msr.mtrr_deftype;
      RDX = 0;
      break;
#endif

#if BX_CPU_LEVEL == 5
    /* The following registers are defined for Pentium only */
    case BX_MSR_P5_MC_ADDR:
    case BX_MSR_MC_TYPE:
      /* TODO */
      break;

    case BX_MSR_CESR:
      /* TODO */
      break;
#else
    /* These are noops on i686... */
    case BX_MSR_P5_MC_ADDR:
    case BX_MSR_MC_TYPE:
      /* do nothing */
      break;

    /* ... And these cause an exception on i686 */
    case BX_MSR_CESR:
    case BX_MSR_CTR0:
    case BX_MSR_CTR1:
      exception(BX_GP_EXCEPTION, 0, 0);
#endif  /* BX_CPU_LEVEL == 5 */

    case BX_MSR_TSC:
      RDTSC(i);
      break;

    /* MSR_APICBASE
       0:7    Reserved
       8     This is set if its the BSP
       9:10    Reserved
       11    APIC Global Enable bit (1=enabled 0=disabled)
       12:35  APIC Base Address
       36:63  Reserved
    */
#if BX_SUPPORT_APIC
    case BX_MSR_APICBASE:
      RAX = BX_CPU_THIS_PTR msr.apicbase;
      RDX = 0;
      BX_INFO(("RDMSR: Read %08x:%08x from MSR_APICBASE", EDX, EAX));
      break;
#endif

#if BX_SUPPORT_X86_64
    case BX_MSR_EFER:
      RAX = BX_CPU_THIS_PTR efer.getRegister();
      RDX = 0;
      break;

    case BX_MSR_STAR:
      RAX = MSR_STAR & 0xffffffff;
      RDX = MSR_STAR >> 32;
      break;

    case BX_MSR_LSTAR:
      RAX = MSR_LSTAR & 0xffffffff;
      RDX = MSR_LSTAR >> 32;
      break;

    case BX_MSR_CSTAR:
      RAX = MSR_CSTAR & 0xffffffff;
      RDX = MSR_CSTAR >> 32;
      break;

    case BX_MSR_FMASK:
      RAX = MSR_FMASK;
      RDX = 0;
      break;

    case BX_MSR_FSBASE:
      RAX = MSR_FSBASE & 0xffffffff;
      RDX = MSR_FSBASE >> 32;
      break;

    case BX_MSR_GSBASE:
      RAX = MSR_GSBASE & 0xffffffff;
      RDX = MSR_GSBASE >> 32;
      break;

    case BX_MSR_KERNELGSBASE:
      RAX = MSR_KERNELGSBASE & 0xffffffff;
      RDX = MSR_KERNELGSBASE >> 32;
      break;

    case BX_MSR_TSC_AUX:
      RAX = MSR_TSC_AUX;   // 32 bit MSR
      RDX = 0;
      break;
#endif  // #if BX_SUPPORT_X86_64

    default:
      BX_ERROR(("RDMSR: Unknown register %#x", ECX));
#if BX_IGNORE_BAD_MSR
      RAX = 0;
      RDX = 0;
#else
      exception(BX_GP_EXCEPTION, 0, 0);
#endif
  }
#else
  BX_INFO(("RDMSR: Pentium CPU required, use --enable-cpu-level=5"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#if BX_SUPPORT_MTRR
BX_CPP_INLINE bx_bool isMemTypeValidMTTR(Bit8u memtype)
{
  switch(memtype) {
  case 0x00: // UC
  case 0x01: // WC
  case 0x04: // WT
  case 0x05: // WP
  case 0x06: // WB
    return 1;
  default:
    return 0;
  }
}

BX_CPP_INLINE bx_bool isMemTypeValidPAT(Bit8u memtype)
{
  return (memtype == 0x07) /* UC- */ || isMemTypeValidMTTR(memtype);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRMSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  if (!real_mode() && CPL!=0) {
    BX_ERROR(("WRMSR: CPL!=0 not in real mode"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit64u val64 = ((Bit64u) EDX << 32) | EAX;

  BX_INSTR_WRMSR(BX_CPU_ID, ECX, val64);

  /* ECX has the MSR to write to */
  switch(ECX) {

#if BX_SUPPORT_SEP
    case BX_MSR_SYSENTER_CS:
      BX_CPU_THIS_PTR msr.sysenter_cs_msr  = EAX;
      break;

    case BX_MSR_SYSENTER_ESP:
#if BX_SUPPORT_X86_64
      if (! IsCanonical(val64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_SYSENTER_ESP !"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
#endif
      BX_CPU_THIS_PTR msr.sysenter_esp_msr = val64;
      break;

    case BX_MSR_SYSENTER_EIP:
#if BX_SUPPORT_X86_64
      if (! IsCanonical(val64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_SYSENTER_EIP !"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
#endif
      BX_CPU_THIS_PTR msr.sysenter_eip_msr = val64;
      break;
#endif

#if BX_SUPPORT_MTRR
    case BX_MSR_MTRRCAP:
      BX_ERROR(("WRMSR: MTRRCAP is read only MSR"));
      exception(BX_GP_EXCEPTION, 0, 0);

    case BX_MSR_MTRRPHYSBASE0:
    case BX_MSR_MTRRPHYSBASE1:
    case BX_MSR_MTRRPHYSBASE2:
    case BX_MSR_MTRRPHYSBASE3:
    case BX_MSR_MTRRPHYSBASE4:
    case BX_MSR_MTRRPHYSBASE5:
    case BX_MSR_MTRRPHYSBASE6:
    case BX_MSR_MTRRPHYSBASE7:
      if (! isMemTypeValidMTTR(AL)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to BX_MSR_MTRRPHYSBASE"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
    case BX_MSR_MTRRPHYSMASK0:
    case BX_MSR_MTRRPHYSMASK1:
    case BX_MSR_MTRRPHYSMASK2:
    case BX_MSR_MTRRPHYSMASK3:
    case BX_MSR_MTRRPHYSMASK4:
    case BX_MSR_MTRRPHYSMASK5:
    case BX_MSR_MTRRPHYSMASK6:
    case BX_MSR_MTRRPHYSMASK7:
      BX_CPU_THIS_PTR msr.mtrrphys[ECX - BX_MSR_MTRRPHYSBASE0] = val64;
      break;

    case BX_MSR_MTRRFIX64K_00000:
      if (! isMemTypeValidMTTR(AL)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_MTRRFIX64K_00000"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      BX_CPU_THIS_PTR msr.mtrrfix64k_00000 = val64;
      break;
    case BX_MSR_MTRRFIX16K_80000:
      if (! isMemTypeValidMTTR(AL)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_MTRRFIX16K_80000"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      BX_CPU_THIS_PTR msr.mtrrfix16k_80000 = val64;
      break;
    case BX_MSR_MTRRFIX16K_A0000:
      if (! isMemTypeValidMTTR(AL)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_MTRRFIX16K_A0000"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      BX_CPU_THIS_PTR msr.mtrrfix16k_a0000 = val64;
      break;

    case BX_MSR_MTRRFIX4K_C0000:
    case BX_MSR_MTRRFIX4K_C8000:
    case BX_MSR_MTRRFIX4K_D0000:
    case BX_MSR_MTRRFIX4K_D8000:
    case BX_MSR_MTRRFIX4K_E0000:
    case BX_MSR_MTRRFIX4K_E8000:
    case BX_MSR_MTRRFIX4K_F0000:
    case BX_MSR_MTRRFIX4K_F8000:
      BX_CPU_THIS_PTR msr.mtrrfix4k[ECX - BX_MSR_MTRRFIX4K_C0000] = val64;
      break;

    case BX_MSR_PAT:
      if (! isMemTypeValidPAT(AL) || ! isMemTypeValidPAT(AH) ||
          ! isMemTypeValidPAT((EAX >> 16) & 0xFF) || 
          ! isMemTypeValidPAT(EAX >> 24) ||
          ! isMemTypeValidPAT(DL) || ! isMemTypeValidPAT(DH) ||
          ! isMemTypeValidPAT((EDX >> 16) & 0xFF) || 
          ! isMemTypeValidPAT(EDX >> 24))
      {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_PAT"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      
      BX_CPU_THIS_PTR msr.pat = val64;
      break;

    case BX_MSR_MTRR_DEFTYPE:
      if (! isMemTypeValidMTTR(AL)) {
        BX_ERROR(("WRMSR: attempt to write invalid Memory Type to MSR_MTRR_DEFTYPE"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      BX_CPU_THIS_PTR msr.mtrr_deftype = EAX;
      break;
#endif

#if BX_CPU_LEVEL == 5
    /* The following registers are defined for Pentium only */
    case BX_MSR_P5_MC_ADDR:
    case BX_MSR_MC_TYPE:
    case BX_MSR_CESR:
      /* TODO */
      break;
#else
    /* These are noops on i686... */
    case BX_MSR_P5_MC_ADDR:
    case BX_MSR_MC_TYPE:
      /* do nothing */
      break;

    /* ... And these cause an exception on i686 */
    case BX_MSR_CESR:
    case BX_MSR_CTR0:
    case BX_MSR_CTR1:
      exception(BX_GP_EXCEPTION, 0, 0);
#endif  /* BX_CPU_LEVEL == 5 */

    case BX_MSR_TSC:
      BX_CPU_THIS_PTR set_TSC(val64);
      BX_INFO(("WRMSR: wrote 0x%08x%08x to MSR_TSC", EDX, EAX));
      break;

    /* MSR_APICBASE
       0:7    Reserved
       8     This is set if its the BSP
       9:10    Reserved
       11    APIC Global Enable bit (1=enabled 0=disabled)
       12:35  APIC Base Address (in Bochs 12:31 because of 32-bit physical addr)
       36:63  Reserved
    */
#if BX_SUPPORT_APIC
    case BX_MSR_APICBASE:
      if (BX_CPU_THIS_PTR msr.apicbase & 0x800) {
        BX_INFO(("WRMSR: wrote %08x:%08x to MSR_APICBASE", EDX, EAX));
        BX_CPU_THIS_PTR msr.apicbase = EAX; /* ignore the high 32bits */
#if BX_PHY_ADDRESS_WIDTH == 32
        if (EDX != 0) {
            BX_PANIC(("MSR_APICBASE: Only 32 bit physical address space is emulated !"));
        }
#endif
        BX_CPU_THIS_PTR local_apic.set_base(BX_CPU_THIS_PTR msr.apicbase);
        // TLB flush is required for emulation correctness
        TLB_flush();  // don't care about performance of apic relocation
      }
      else {
        BX_INFO(("WRMSR: MSR_APICBASE APIC global enable bit cleared !"));
      }
      break;
#endif

#if BX_SUPPORT_X86_64
    case BX_MSR_EFER:
      if (val64 & ~BX_EFER_SUPPORTED_BITS) {
        BX_ERROR(("WRMSR: attempt to set reserved bits of EFER MSR !"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }

      // #GP(0) if changing EFER.LME when cr0.pg = 1
      if ((BX_CPU_THIS_PTR efer.get_LME() != ((EAX >> 8) & 1)) &&
           BX_CPU_THIS_PTR  cr0.get_PG())
      {
        BX_ERROR(("WRMSR: attempt to change LME when CR0.PG=1"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }

      BX_CPU_THIS_PTR efer.setRegister((EAX & BX_EFER_SUPPORTED_BITS & ~BX_EFER_LMA_MASK)
              | (BX_CPU_THIS_PTR efer.val32 & BX_EFER_LMA_MASK)); // keep LMA untouched
      break;

    case BX_MSR_STAR:
      MSR_STAR  = val64;
      break;

    case BX_MSR_LSTAR:
      if (! IsCanonical(val64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_LSTAR !"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      MSR_LSTAR = val64;
      break;

    case BX_MSR_CSTAR:
      if (! IsCanonical(val64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_CSTAR !"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      MSR_CSTAR = val64;
      break;

    case BX_MSR_FMASK:
      MSR_FMASK = (Bit32u) val64;
      break;

    case BX_MSR_FSBASE:
      if (! IsCanonical(val64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_FSBASE !"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      MSR_FSBASE = val64;
      break;

    case BX_MSR_GSBASE:
      if (! IsCanonical(val64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_GSBASE !"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      MSR_GSBASE = val64;
      break;

    case BX_MSR_KERNELGSBASE:
      if (! IsCanonical(val64)) {
        BX_ERROR(("WRMSR: attempt to write non-canonical value to MSR_KERNELGSBASE !"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
      MSR_KERNELGSBASE = val64;
      break;

    case BX_MSR_TSC_AUX:
      MSR_TSC_AUX = EAX;
      break;
#endif  // #if BX_SUPPORT_X86_64

    default:
      BX_ERROR(("WRMSR: Unknown register %#x", ECX));
#if BX_IGNORE_BAD_MSR == 0
      exception(BX_GP_EXCEPTION, 0, 0);
#endif
  }
#else
  BX_INFO(("WRMSR: Pentium CPU required, use --enable-cpu-level=5"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}
