/////////////////////////////////////////////////////////////////////////
// $Id: cpuid.cc,v 1.47 2007-09-20 17:33:31 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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


#if BX_SUPPORT_X86_64==0
// Make life easier for merging code.
#define RAX EAX
#define RBX EBX
#define RCX ECX
#define RDX EDX
#endif


/* Get CPU version information. */
Bit32u BX_CPU_C::get_cpu_version_information()
{
  Bit32u family = 0, model = 0, stepping = 0;
  Bit32u extended_model = 0;
  Bit32u extended_family = 0;

#if BX_CPU_LEVEL > 3

  /* ****** */
  /*  i486  */
  /* ****** */

#if BX_CPU_LEVEL == 4
  family = 4;

#if BX_SUPPORT_FPU
  model = 1;            // 486dx
  stepping = 3;
#else
  model = 2;            // 486sx
  stepping = 3;
#endif

  /* **************** */
  /*  i586 (Pentium)  */
  /* **************** */

#elif BX_CPU_LEVEL == 5	
  family   = 5;
#if BX_SUPPORT_MMX
  model    = 4;         // Pentium MMX
#else
  model    = 1;         // Pentium 60/66
#endif
  stepping = 3;

  /* ****** */
  /*  i686  */
  /* ****** */

#elif BX_CPU_LEVEL == 6

#if BX_SUPPORT_SSE >= 2 // Pentium 4 processor
/*
     The model, family, and processor type for the first
     processor in the Intel Pentium 4 family is as follows:
		* Model-0000B
		* Family-1111B
		* Processor Type-00B (OEM)
                * Stepping-0B
*/
  model    = 0;
  family   = 0xf;
  stepping = 0;

#if BX_SUPPORT_X86_64
  model = 2;            // Hammer returns what?
#endif

#else	                // Pentium Pro/Pentium II/Pentium III processor
  family   = 6;
  model    = 8;
  stepping = 3;
#endif

#else
  BX_PANIC(("CPUID family ID not implemented for CPU LEVEL > 6"));
#endif

#endif  // BX_CPU_LEVEL > 3

  return (extended_family << 20) | 
         (extended_model << 16) | 
         (family << 8) | 
         (model<<4) | stepping;
}       

/* Get CPU extended feature flags. */
Bit32u BX_CPU_C::get_extended_cpuid_features()
{
  // [0:0]   SSE3: SSE3 Instructions
  // [2:1]   reserved
  // [3:3]   MONITOR/MWAIT support
  // [4:4]   DS-CPL: CPL qualified debug store
  // [5:5]   VMX: Virtual Machine Technology
  // [6:6]   reserved
  // [7:7]   EST: Enhanced Intel SpeedStep Technology
  // [8:8]   TM2: Thermal Monitor 2
  // [9:9]   SSE3E: SSE3E Instructions (Intel Core Duo 2 new instructions)
  // [10:10] CNXT-ID: L1 context ID
  // [12:11] reserved
  // [13:13] CMPXCHG16B: CMPXCHG16B instruction support
  // [14:14] xTPR update control
  // [18:15] reserved
  // [19:19] SSE4.1: SSE4.1 Instructions
  // [20:20] SSE4.2: SSE4.2 (SSE4E) Instructions
  // [21:22] Reserved
  // [23:23] POPCNT instruction support
  // [31:21] reserved

  Bit32u features = 0;

#if BX_SUPPORT_SSE >= 3
  features |= 0x1;      // support SSE3
#endif
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  features |= (1<<9);   // support SSE3E
#endif

#if BX_SUPPORT_X86_64
  features |= (1<<13);  // support CMPXCHG16B
#endif

#if BX_SUPPORT_SSE >= 4
  features |= (1<<19);  // support SSE4
#endif

#if BX_SUPPORT_POPCNT
  features |= (1<<23);  // support POPCNT instruction
#endif  
  
  return features;
}

/* Get CPU feature flags. Returned by CPUID functions 1 and 80000001.  */
Bit32u BX_CPU_C::get_std_cpuid_features()
{
  Bit32u features = 0;

#if BX_SUPPORT_FPU
  features |= (1<<0);
#endif
#if BX_SUPPORT_VME
  features |= (1<<1);
#endif

#if (BX_CPU_LEVEL >= 5)
  features |= (1<<4);   // implement TSC
  features |= (1<<5);   // support RDMSR/WRMSR
  features |= (1<<8);   // Support CMPXCHG8B instruction

#if BX_SUPPORT_MMX
  features |= (1<<23);  // support MMX
#endif

#endif

#if (BX_CPU_LEVEL >= 6) || (BX_CPU_LEVEL_HACKED >= 6)
  features |= (1<<24);  // Implement FSAVE/FXRSTOR instructions.
#endif

#if BX_SUPPORT_CLFLUSH
  features |= (1<<19);  // Implement CLFLUSH instruction
#endif

#if BX_CPU_LEVEL >= 6
  features |= (1<<15);  // Implement CMOV instructions.
#if BX_SUPPORT_APIC
  // if MSR_APICBASE APIC Global Enable bit has been cleared,
  // the CPUID feature flag for the APIC is set to 0.
  if (BX_CPU_THIS_PTR msr.apicbase & 0x800)
    features |= (1<<9); // APIC on chip
#endif
#if BX_SUPPORT_SSE >= 1
  features |= (1<<25);  // support SSE
#endif
#if BX_SUPPORT_SSE >= 2
  features |= (1<<26);  // support SSE2
#endif
#endif

#if BX_SUPPORT_SEP
  features |= (1<<11);  // SYSENTER/SYSEXIT
#endif

#if BX_SUPPORT_MTRR
  features |= (1<<12);  // Implement MTRRs
#endif
#if BX_SUPPORT_LARGE_PAGES
  features |= (1<< 3);  // Support Page-Size Extension (2M/4M pages)
#endif
#if BX_SUPPORT_GLOBAL_PAGES
  features |= (1<<13);  // Support Global pages
#endif
#if BX_SUPPORT_PAE
  features |= (1<<6);   // Support PAE
#endif

#if BX_SUPPORT_SMP
  // Intel(R) HyperThreading Technology
  if (SIM->get_param_num(BXPN_CPU_NTHREADS)->get() > 1) 
    features |= (1<<28);
#endif

  return features;
}

void BX_CPU_C::CPUID(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  unsigned features;
#endif

#if BX_SUPPORT_SMP
  unsigned n_logical_processors;
#endif

#if BX_CPU_LEVEL >= 4
  switch (EAX) {
    case 0:
      // EAX: highest input value understood by CPUID
#if BX_CPU_LEVEL <= 5
      RAX = 1;		// 486 and Pentium processors
#else
      RAX = 2;		// for Pentium Pro, Pentium II, Pentium 4 processors
#endif
      // EBX: vendor ID string
      // EDX: vendor ID string
      // ECX: vendor ID string
#if BX_SUPPORT_X86_64
      RBX = 0x68747541; // "Auth"
      RDX = 0x69746e65; // "enti"
      RCX = 0x444d4163; // "cAMD"
#else
      RBX = 0x756e6547; // "Genu"
      RDX = 0x49656e69; // "ineI"
      RCX = 0x6c65746e; // "ntel"
#endif
      break;

    case 1:
      // EAX:       CPU Version Information
      //   [3:0]   Stepping ID
      //   [7:4]   Model: starts at 1
      //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
      //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
      //   [31:14] Reserved
      // EBX:      
      //   [7:0]   Brand ID
      //   [15:8]  CLFLUSH cache line size (value*8 = cache line size in bytes)
      //   [23:16] Number of logical processors in one physical processor
      //   [31:24] Local Apic ID
      // ECX:       Feature Flags::Extended
      //   [0:0]   SSE3: SSE3 Instructions
      //   [2:1]   reserved
      //   [3:3]   MONITOR/MWAIT support
      //   [4:4]   DS-CPL: CPL qualified debug store
      //   [5:5]   VMX: Virtual Machine Technology
      //   [6:6]   reserved
      //   [7:7]   EST: Enhanced Intel SpeedStep Technology
      //   [8:8]   TM2: Thermal Monitor 2
      //   [9:9]   SSE3E: SSE3E Instructions (Intel Core Duo 2 new instructions)
      //   [10:10] CNXT-ID: L1 context ID
      //   [12:11] reserved
      //   [13:13] CMPXCHG16B: CMPXCHG16B instruction support
      //   [14:14] xTPR update control
      //   [18:15] reserved
      //   [19:19] SSE4: SSE4 Instructions
      //   [20:20] SSE4E: SSE4E Instructions
      //   [31:21] reserved
      // EDX:       Feature Flags
      //   [0:0]   FPU on chip
      //   [1:1]   VME: Virtual-8086 Mode enhancements
      //   [2:2]   DE: Debug Extensions (I/O breakpoints)
      //   [3:3]   PSE: Page Size Extensions
      //   [4:4]   TSC: Time Stamp Counter
      //   [5:5]   MSR: RDMSR and WRMSR support
      //   [6:6]   PAE: Physical Address Extensions
      //   [7:7]   MCE: Machine Check Exception
      //   [8:8]   CXS: CMPXCHG8B instruction
      //   [9:9]   APIC: APIC on Chip
      //   [10:10] Reserved
      //   [11:11] SYSENTER/SYSEXIT support
      //   [12:12] MTRR: Memory Type Range Reg
      //   [13:13] PGE/PTE Global Bit
      //   [14:14] MCA: Machine Check Architecture
      //   [15:15] CMOV: Cond Mov/Cmp Instructions
      //   [16:16] PAT: Page Attribute Table
      //   [17:17] PSE: Page-Size Extensions
      //   [18:18] Processor Serial Number
      //   [19:19] CLFLUSH: CLFLUSH Instruction support
      //   [20:20] Reserved
      //   [21:21] DS: Debug Store
      //   [22:22] ACPI: Thermal Monitor and Software Controlled Clock Facilities
      //   [23:23] MMX Technology
      //   [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
      //   [25:25] SSE: SSE Extensions
      //   [26:26] SSE2: SSE2 Extensions
      //   [27:27] Reserved
      //   [28:28] Hyper Threading Technology
      //   [29:29] TM: Thermal Monitor
      //   [30:30] Reserved
      //   [31:31] PBE: Pending Break Enable
      RAX = get_cpu_version_information();
      RBX = 0;
#if BX_SUPPORT_APIC
      RBX |= (BX_CPU_THIS_PTR local_apic.get_id() << 24);
#endif
#if BX_SUPPORT_CLFLUSH
      RBX |= 8 << 8;  // 64 byte cache line size
#endif
#if BX_SUPPORT_SMP
      n_logical_processors = SIM->get_param_num(BXPN_CPU_NCORES)->get()*SIM->get_param_num(BXPN_CPU_NTHREADS)->get();
      if (n_logical_processors > 1)
          RBX |= (n_logical_processors << 16);
#endif
      RCX = get_extended_cpuid_features ();
      RDX = get_std_cpuid_features ();
      break;

#if BX_CPU_LEVEL >= 6 
    case 2:
      RAX = 0x00410601;  // for Pentium Pro compatibility
      RBX = 0;
      RCX = 0;
      RDX = 0;
      break;

#if BX_SUPPORT_SSE >= 2

#if BX_SUPPORT_X86_64
    // Extended information for AMD Athlon processor

    // x86-64 functions
    case 0x80000000:
      // max function supported.
      RAX = 0x80000008;
      RBX = 0x68747541; // "Auth"
      RDX = 0x69746e65; // "enti"
      RCX = 0x444d4163; // "cAMD"
      break;

    case 0x80000001:
      RAX = get_cpu_version_information();
      // Many of the bits in EDX are the same as EAX [*]
      // [*] [0:0]   FPU on chip
      // [*] [1:1]   VME: Virtual-8086 Mode enhancements
      // [*] [2:2]   DE: Debug Extensions (I/O breakpoints)
      // [*] [3:3]   PSE: Page Size Extensions
      // [*] [4:4]   TSC: Time Stamp Counter
      // [*] [5:5]   MSR: RDMSR and WRMSR support
      // [*] [6:6]   PAE: Physical Address Extensions
      // [*] [7:7]   MCE: Machine Check Exception
      // [*] [8:8]   CXS: CMPXCHG8B instruction
      // [*] [9:9]   APIC: APIC on Chip
      //     [10:10] Reserved
      //     [11:11] SYSCALL/SYSRET support
      // [*] [12:12] MTRR: Memory Type Range Reg
      // [*] [13:13] PGE/PTE Global Bit
      // [*] [14:14] MCA: Machine Check Architecture
      // [*] [15:15] CMOV: Cond Mov/Cmp Instructions
      // [*] [16:16] PAT: Page Attribute Table
      // [*] [17:17] PSE: Page-Size Extensions
      //     [18:19] Reserved
      //     [20:20] No-Execute page protection
      //     [21:21] Reserved
      //     [22:22] AMD MMX Extensions
      // [*] [23:23] MMX Technology
      // [*] [24:24] FXSR: FXSAVE/FXRSTOR (also indicates CR4.OSFXSR is available)
      //     [25:25] Fast FXSAVE/FXRSTOR mode support
      //     [26:26] Reserved
      //     [27:27] Support RDTSCP Instruction
      //     [28:28] Reserved
      //     [29:29] Long Mode
      //     [30:30] AMD 3DNow! Extensions
      //     [31:31] AMD 3DNow! Instructions
      features = get_std_cpuid_features();
      features = features & 0x0183F3FF;

      RDX = features | (1 << 29) | (1 << 27) | (1 << 25) | 
                       (1 << 22) | (1 << 20) | (1 << 11);
      RBX = 0;

      // RCX:
      //     [0:0]   LAHF/SAHF available in 64-bit mode
      //     [1:1]   AMD CmpLegacy
      //     [2:2]   AMD Secure Virtual Machine Technology
      //     [3:3]   Extended APIC Space
      //     [4:4]   Alternative CR8 (treat lock mov cr0 as mov cr8) 
      //     [5:5]   LZCNT support
      //     [6:6]   SSE4A support
      //     [7:7]   Misaligned SSE support
      //     [8:8]   3DNow! prefetch support
      //     [9:9]   OS visible workarounds
      //     [10:31] Reserved
      RCX = 1 | (1<<8);
#if BX_SUPPORT_MISALIGNED_SSE
      RCX |= (1<<7);
#endif
      break;

    // Processor Brand String, use the value given 
    // in AMD manuals.
    case 0x80000002:
      RAX = 0x20444D41; // "AMD "
      RBX = 0x6C687441; // "Athl"
      RCX = 0x74286E6F; // "on(t"
      RDX = 0x7020296D; // "m) p"
      break;
    case 0x80000003:
      RAX = 0x65636F72; // "roce"
      RBX = 0x726F7373; // "ssor"
      RCX = 0x00000000;
      RDX = 0x00000000;
      break;
    case 0x80000004:
      RAX = 0x00000000;
      RBX = 0x00000000;
      RCX = 0x00000000;
      RDX = 0x00000000;
      break;
    case 0x80000005:
      /* cache info (L1 cache) */
      RAX = 0x01ff01ff;
      RBX = 0x01ff01ff;
      RCX = 0x40020140;
      RDX = 0x40020140;
      break;
    case 0x80000006:
      /* cache info (L2 cache) */
      RAX = 0;
      RBX = 0x42004200;
      RCX = 0x02008140;
      RDX = 0;
      break;
    case 0x80000008:
      // virtual & phys address size in low 2 bytes.
      RAX = BX_PHY_ADDRESS_WIDTH | (BX_LIN_ADDRESS_WIDTH<<8);
      RBX = 0;
      RCX = 0;
      RDX = 0; // Reserved, undefined
      break;

#else // BX_SUPPORT_X86_64

    // Extended information for Intel P4 processor
    case 0x80000000:
      // max function supported.
      RAX = 0x80000004;
      RBX = 0;
      RCX = 0;
      RDX = 0;
      break;

    case 0x80000001:    // Reserved
      RAX = 0;
      RBX = 0;
      RCX = 0;
      RDX = 0;
      break;

    // Processor Brand String, use the value that is returned
    // by the first processor in the Pentium 4 family 
    // (according to Intel manual)
    case 0x80000002:
      RAX = 0x20202020; // "    "
      RBX = 0x20202020; // "    "
      RCX = 0x20202020; // "    "
      RDX = 0x6E492020; // "  In"
      break;
    case 0x80000003:
      RAX = 0x286C6574; // "tel("
      RBX = 0x50202952; // "R) P"
      RCX = 0x69746E65; // "enti"
      RDX = 0x52286D75; // "um(R"
      break;
    case 0x80000004:
      RAX = 0x20342029; // ") 4 "
      RBX = 0x20555043; // "CPU "
      RCX = 0x20202020; // "    "
      RDX = 0x00202020; // "    "
      break;
#endif // BX_SUPPORT_X86_64

#endif // BX_SUPPORT_SSE >= 2

#endif // BX_CPU_LEVEL >= 6 

    default:
      RAX = 0;
      RBX = 0;
      RCX = 0;
      RDX = 0; // Reserved, undefined
      break;
    }

#else
  BX_INFO(("CPUID: not available on < late 486"));
  UndefinedOpcode(i);
#endif
}
