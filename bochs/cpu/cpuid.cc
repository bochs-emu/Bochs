/////////////////////////////////////////////////////////////////////////
// $Id: cpuid.cc,v 1.58 2007-12-01 16:45:16 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007 Stanislav Shwartsman
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


#if BX_SUPPORT_X86_64==0
// Make life easier for merging code.
#define RAX EAX
#define RBX EBX
#define RCX ECX
#define RDX EDX
#endif

#if BX_SUPPORT_3DNOW
  #define BX_CPU_VENDOR_INTEL 0
#else
  #define BX_CPU_VENDOR_INTEL 1
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
  model    = 2;         // Hammer returns what?
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
#if BX_SUPPORT_MONITOR_MWAIT
  features |= (1<<3);   // support MONITOR/MWAIT
#endif
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  features |= (1<<9);   // support SSE3E
#endif

#if BX_SUPPORT_X86_64
  features |= (1<<13);  // support CMPXCHG16B
#endif

#if BX_SUPPORT_SSE >= 4
  features |= (1<<19);  // support SSE4.1
#endif

#if (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  features |= (1<<20);  // support SSE4.2 (SSE4E)
#endif

#if BX_SUPPORT_POPCNT || (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
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

#if BX_CPU_LEVEL >= 6
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
  Bit32u function = EAX;

#if BX_CPU_LEVEL >= 4
  if(function < 0x80000000) {
    if(function < MAX_STD_CPUID_FUNCTION) {
      RAX = BX_CPU_THIS_PTR cpuid_std_function[function].eax;
      RBX = BX_CPU_THIS_PTR cpuid_std_function[function].ebx;
      RCX = BX_CPU_THIS_PTR cpuid_std_function[function].ecx;
      RDX = BX_CPU_THIS_PTR cpuid_std_function[function].edx;
#if BX_SUPPORT_APIC
      if (function == 1) {
        // if MSR_APICBASE APIC Global Enable bit has been cleared,
        // the CPUID feature flag for the APIC is set to 0.
        if ((BX_CPU_THIS_PTR msr.apicbase & 0x800) == 0)
          RDX &= ~(1<<9); // APIC on chip
      }
#endif
      return;
    }
  }
  else {
    function -= 0x80000000;
    if(function < MAX_EXT_CPUID_FUNCTION) {
      RAX = BX_CPU_THIS_PTR cpuid_ext_function[function].eax;
      RBX = BX_CPU_THIS_PTR cpuid_ext_function[function].ebx;
      RCX = BX_CPU_THIS_PTR cpuid_ext_function[function].ecx;
      RDX = BX_CPU_THIS_PTR cpuid_ext_function[function].edx;
#if BX_SUPPORT_APIC
      if (EAX == 1) {
        // if MSR_APICBASE APIC Global Enable bit has been cleared,
        // the CPUID feature flag for the APIC is set to 0.
        if ((BX_CPU_THIS_PTR msr.apicbase & 0x800) == 0)
          RDX &= ~(1<<9); // APIC on chip
      }
#endif
      return;
    }
  }

  // unknown CPUID function
  RAX = 0;
  RBX = 0;
  RCX = 0;
  RDX = 0;
#else
  BX_INFO(("CPUID: not available on < 486"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::set_cpuid_defaults(void)
{
  cpuid_function_t *cpuid;
  int i;

  for (i=0;i<MAX_STD_CPUID_FUNCTION;i++) {
    BX_CPU_THIS_PTR cpuid_std_function[i].eax = 0;
    BX_CPU_THIS_PTR cpuid_std_function[i].ebx = 0;
    BX_CPU_THIS_PTR cpuid_std_function[i].ecx = 0;
    BX_CPU_THIS_PTR cpuid_std_function[i].edx = 0;
  }

  for (i=0;i<MAX_EXT_CPUID_FUNCTION;i++) {
    BX_CPU_THIS_PTR cpuid_ext_function[i].eax = 0;
    BX_CPU_THIS_PTR cpuid_ext_function[i].ebx = 0;
    BX_CPU_THIS_PTR cpuid_ext_function[i].ecx = 0;
    BX_CPU_THIS_PTR cpuid_ext_function[i].edx = 0;
  }

  // ------------------------------------------------------
  // CPUID function 0x00000000
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[0]);

  // EAX: highest input value understood by CPUID
  // EBX: vendor ID string
  // EDX: vendor ID string
  // ECX: vendor ID string

#if BX_CPU_LEVEL <= 5
  // 486 and Pentium processors
  cpuid->eax = 1;
#else 
  // for Pentium Pro, Pentium II, Pentium 4 processors
  cpuid->eax = BX_SUPPORT_MONITOR_MWAIT ? 5 : 2;
#endif

#if BX_CPU_VENDOR_INTEL
  cpuid->ebx = 0x756e6547; // "Genu"
  cpuid->edx = 0x49656e69; // "ineI"
  cpuid->ecx = 0x6c65746e; // "ntel"
#else
  cpuid->ebx = 0x68747541; // "Auth"
  cpuid->edx = 0x69746e65; // "enti"
  cpuid->ecx = 0x444d4163; // "cAMD"
#endif

  // ------------------------------------------------------
  // CPUID function 0x00000001
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[1]);

  // EAX:       CPU Version Information
  //   [3:0]   Stepping ID
  //   [7:4]   Model: starts at 1
  //   [11:8]  Family: 4=486, 5=Pentium, 6=PPro, ...
  //   [13:12] Type: 0=OEM, 1=overdrive, 2=dual cpu, 3=reserved
  //   [31:14] Reserved

  cpuid->eax = get_cpu_version_information();

  // EBX:      
  //   [7:0]   Brand ID
  //   [15:8]  CLFLUSH cache line size (value*8 = cache line size in bytes)
  //   [23:16] Number of logical processors in one physical processor
  //   [31:24] Local Apic ID

  cpuid->ebx = 0;
#if BX_SUPPORT_APIC
  cpuid->ebx |= (BX_CPU_THIS_PTR local_apic.get_id() << 24);
#endif
#if BX_SUPPORT_CLFLUSH
  cpuid->ebx |= (CACHE_LINE_SIZE / 8) << 8;
#endif
#if BX_SUPPORT_SMP
  unsigned n_logical_processors = SIM->get_param_num(BXPN_CPU_NCORES)->get()*SIM->get_param_num(BXPN_CPU_NTHREADS)->get();
  if (n_logical_processors > 1)
    cpuid->ebx |= (n_logical_processors << 16);
#endif

  // ECX:       Extended Feature Flags
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
  //   [19:19] SSE4.1: SSE4.1 Instructions
  //   [20:20] SSE4.2: SSE4.2 (SSE4E) Instructions
  //   [31:21] reserved
  cpuid->ecx = get_extended_cpuid_features();

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
  cpuid->edx = get_std_cpuid_features();

#if BX_CPU_LEVEL >= 6
  // ------------------------------------------------------
  // CPUID function 0x00000002 - Cache and TLB Descriptors
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[2]);

#if BX_CPU_VENDOR_INTEL
  cpuid->eax = 0x00410601;  // for Pentium Pro compatibility
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;
#else
  cpuid->eax = 0;           // ignore for AMD
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;
#endif

  // ------------------------------------------------------
  // CPUID function 0x00000003 - Processor Serial Number
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[3]);

  cpuid->eax = 0;
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;

  // ------------------------------------------------------
  // CPUID function 0x00000004 - Deterministic Cache Parameters
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[4]);

  cpuid->eax = 0;
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;

#if BX_SUPPORT_MONITOR_MWAIT
  // ------------------------------------------------------
  // CPUID function 0x00000005
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[5]);

  // EAX - Smallest monitor-line size in bytes
  // EBX - Largest  monitor-line size in bytes
  // ECX -
  //   [31:2] - reserved
  //    [1:1] - exit MWAIT even with EFLAGS.IF = 0
  //    [0:0] - MONITOR/MWAIT extensions are supported
  // EDX - Reserved
  cpuid->eax = CACHE_LINE_SIZE;
  cpuid->ebx = CACHE_LINE_SIZE;
  cpuid->ecx = 3;
  cpuid->edx = 0;
#endif

#if BX_SUPPORT_SSE >= 2   // report Pentium 4 extended functions

  // ------------------------------------------------------
  // CPUID function 0x80000000
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[0]);

  // EAX: highest input value understood by CPUID
  // EBX: vendor ID string
  // EDX: vendor ID string
  // ECX: vendor ID string
  cpuid->eax = BX_SUPPORT_X86_64 ? 0x80000008 : 0x80000004;
#if BX_CPU_VENDOR_INTEL
  cpuid->ebx = 0;
  cpuid->edx = 0;          // Reserved for Intel
  cpuid->ecx = 0;
#else
  cpuid->ebx = 0x68747541; // "Auth"
  cpuid->edx = 0x69746e65; // "enti"
  cpuid->ecx = 0x444d4163; // "cAMD"
#endif

  // ------------------------------------------------------
  // CPUID function 0x80000001
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[1]);

  // EAX:       CPU Version Information
  cpuid->eax = BX_CPU_VENDOR_INTEL ? 0 : get_cpu_version_information();

  // EBX:       Brand ID
  cpuid->ebx = 0;

  // ECX:
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
#if BX_SUPPORT_X86_64
  cpuid->ecx = 1 | (1<<8);
#endif
#if BX_SUPPORT_MISALIGNED_SSE
  cpuid->ecx |= (1<<7);
#endif

  // EDX: 
  // Many of the bits in EDX are the same as EAX [*] for AMD
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
  //     [26:26] 1G paging support
  //     [27:27] Support RDTSCP Instruction
  //     [28:28] Reserved
  //     [29:29] Long Mode
  //     [30:30] AMD 3DNow! Extensions
  //     [31:31] AMD 3DNow! Instructions
  unsigned features  = BX_CPU_VENDOR_INTEL ? 0 : get_std_cpuid_features();
  features &= 0x0183F3FF;
#if BX_SUPPORT_3DNOW
  features |= (1 << 22) | (1 << 30) | (1 << 31);  // only AMD is interesting in AMD MMX extensions
#endif
#if BX_SUPPORT_X86_64
  features |= (1 << 29) | (1 << 27) | (1 << 25) | (1 << 20) | (1 << 11);
#endif
  cpuid->edx = features;

#if BX_CPU_VENDOR_INTEL
  // Processor Brand String, use the value that is returned
  // by the first processor in the Pentium 4 family 
  // (according to Intel manual)

  // ------------------------------------------------------
  // CPUID function 0x80000002
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[2]);
  cpuid->eax = 0x20202020; // "    "
  cpuid->ebx = 0x20202020; // "    "
  cpuid->ecx = 0x20202020; // "    "
  cpuid->edx = 0x6E492020; // "  In"

  // CPUID function 0x80000003
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[3]);
  cpuid->eax = 0x286C6574; // "tel("
  cpuid->ebx = 0x50202952; // "R) P"
  cpuid->ecx = 0x69746E65; // "enti"
  cpuid->edx = 0x52286D75; // "um(R"

  // CPUID function 0x80000004
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[4]);
  cpuid->eax = 0x20342029; // ") 4 "
  cpuid->ebx = 0x20555043; // "CPU "
  cpuid->ecx = 0x20202020; // "    "
  cpuid->edx = 0x00202020; // "    "
#else
  // Processor Brand String, use the value given 
  // in AMD manuals.

  // ------------------------------------------------------
  // CPUID function 0x80000002
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[2]);
  cpuid->eax = 0x20444D41; // "AMD "
  cpuid->ebx = 0x6C687441; // "Athl"
  cpuid->ecx = 0x74286E6F; // "on(t"
  cpuid->edx = 0x7020296D; // "m) p"

  // ------------------------------------------------------
  // CPUID function 0x80000003
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[3]);
  cpuid->eax = 0x65636F72; // "roce"
  cpuid->ebx = 0x726F7373; // "ssor"
  cpuid->ecx = 0x00000000;
  cpuid->edx = 0x00000000;

  // ------------------------------------------------------
  // CPUID function 0x80000004
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[4]);
  cpuid->eax = 0x00000000;
  cpuid->ebx = 0x00000000;
  cpuid->ecx = 0x00000000;
  cpuid->edx = 0x00000000;
#endif

#if BX_SUPPORT_X86_64
  // ------------------------------------------------------
  // CPUID function 0x80000005
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[5]);

  /* cache info (L1 cache) */
  cpuid->eax = 0x01ff01ff;
  cpuid->ebx = 0x01ff01ff;
  cpuid->ecx = 0x40020140;
  cpuid->edx = 0x40020140;

  // ------------------------------------------------------
  // CPUID function 0x80000006
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[6]);

  /* cache info (L2 cache) */
  cpuid->eax = 0;
  cpuid->ebx = 0x42004200;
  cpuid->ecx = 0x02008140;
  cpuid->edx = 0;

  // ------------------------------------------------------
  // CPUID function 0x00000007
  cpuid = &(BX_CPU_THIS_PTR cpuid_std_function[7]);

  cpuid->eax = 0;
  cpuid->ebx = 0;
  cpuid->ecx = 0;
  cpuid->edx = 0;

  // ------------------------------------------------------
  // CPUID function 0x80000008
  cpuid = &(BX_CPU_THIS_PTR cpuid_ext_function[8]);

  // virtual & phys address size in low 2 bytes.
  cpuid->eax = BX_PHY_ADDRESS_WIDTH | (BX_LIN_ADDRESS_WIDTH << 8);
  cpuid->ebx = 0;
  cpuid->ecx = 0; // Reserved, undefined
  cpuid->edx = 0;

#endif // BX_SUPPORT_X86_64

#endif // BX_SUPPORT_SSE >= 2

#endif // BX_CPU_LEVEL >= 6
}
