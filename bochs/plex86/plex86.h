/************************************************************************
 * $Id: plex86.h,v 1.4 2003-01-08 17:19:57 kevinlawton Exp $
 ************************************************************************
 *
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003  Kevin P. Lawton
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef __PLEX86_H__
#define __PLEX86_H__

#include "config.h"
#include "descriptor.h"




typedef struct {
  selector_t   sel;
  descriptor_t des;
  unsigned     valid;
  } __attribute__ ((packed)) guest_sreg_t;

#define SRegES    0
#define SRegCS    1
#define SRegSS    2
#define SRegDS    3
#define SRegFS    4
#define SRegGS    5


typedef union {
  Bit32u raw;
  struct {
    Bit32u pe:1;
    Bit32u mp:1;
    Bit32u em:1;
    Bit32u ts:1;
    Bit32u et:1;
    Bit32u ne:1;
    Bit32u R15_6:10;
    Bit32u wp:1;
    Bit32u R17:1;
    Bit32u am:1;
    Bit32u R28_19:10;
    Bit32u nw:1;
    Bit32u cd:1;
    Bit32u pg:1;
    } __attribute__ ((packed)) fields;
  } __attribute__ ((packed)) cr0_t;

typedef union {
  Bit32u raw;
  struct {
    Bit32u vme:1;
    Bit32u pvi:1;
    Bit32u tsd:1;
    Bit32u de:1;
    Bit32u pse:1;
    Bit32u pae:1;
    Bit32u mce:1;
    Bit32u pge:1;
    Bit32u pce:1;
    Bit32u reserved:23;
    } __attribute__ ((packed)) fields;
  } __attribute__ ((packed)) cr4_t;

typedef struct {
  Bit32u vendorDWord0;
  Bit32u vendorDWord1;
  Bit32u vendorDWord2;
  union {
    Bit32u raw;
    struct {
      Bit32u stepping:4;
      Bit32u model:4;
      Bit32u family:4;
      Bit32u procType:2;
      Bit32u Reserved31_14:18;
      } __attribute__ ((packed)) fields;
    } __attribute__ ((packed)) procSignature;
  union {
    Bit32u raw;
    struct {
      Bit32u fpu:1;
      Bit32u vme:1;
      Bit32u de:1;
      Bit32u pse:1;
      Bit32u tsc:1;
      Bit32u msr:1;
      Bit32u pae:1;
      Bit32u mce:1;
      Bit32u cx8:1;
      Bit32u apic:1;
      Bit32u Reserved10:1;
      Bit32u sep:1;
      Bit32u mtrr:1;
      Bit32u pge:1;
      Bit32u mca:1;
      Bit32u cmov:1;
      Bit32u pat:1;
      Bit32u pse36:1;
      Bit32u Reserved22_18:5;
      Bit32u mmx:1;
      Bit32u fxsr:1;
      Bit32u Reserved31_25:7;
      } __attribute__ ((packed)) fields;
    } __attribute__ ((packed)) featureFlags;
  } __attribute__ ((packed)) cpuid_info_t;

typedef struct {
  Bit32u edi;
  Bit32u esi;
  Bit32u ebp;
  Bit32u esp;
  Bit32u ebx;
  Bit32u edx;
  Bit32u ecx;
  Bit32u eax;

  Bit32u eflags;
  Bit32u eip;
  guest_sreg_t sreg[6];
  guest_sreg_t ldtr;
  guest_sreg_t tr;
  gdt_info_t gdtr;
  gdt_info_t idtr;
  Bit32u dr0, dr1, dr2, dr3, dr6, dr7;
  Bit32u tr3, tr4, tr5, tr6, tr7;
  cr0_t cr0;
  Bit32u cr1, cr2, cr3;
  cr4_t cr4;
  unsigned a20Enable;
  } __attribute__ ((packed)) guest_cpu_t;






/*
 *  ioctl() names.
 */

#if defined(__linux__) || defined(__NetBSD__) || defined(__FreeBSD__)
#ifdef __linux__
#include <asm/ioctl.h>
#else
#include <sys/ioccom.h>
#endif
#define PLEX86_RESET        _IO('k', 3)
#define PLEX86_TEARDOWN     _IO('k', 4)
#define PLEX86_EXECUTE      _IO('k', 5)
#define PLEX86_CPUID        _IO('k', 6)
#define PLEX86_REGISTER_MEMORY _IO('k', 7)
#else
#define PLEX86_RESET        0x6b03
#define PLEX86_TEARDOWN     0x6b04
#define PLEX86_EXECUTE      0x6b05
#define PLEX86_CPUID        0x6b06
#define PLEX86_REGISTER_MEMORY 0x6b07
#endif

/* Reasons why plex86 could not execute the guest context in the VM. */
#define Plex86NoExecute_Method    1
#define Plex86NoExecute_CR0       2
#define Plex86NoExecute_CR4       3
#define Plex86NoExecute_CS        4
#define Plex86NoExecute_A20       5
#define Plex86NoExecute_Selector  6
#define Plex86NoExecute_DPL       7
#define Plex86NoExecute_EFlags    8
#define Plex86NoExecute_Panic     9
#define Plex86NoExecute_VMState  10


/* Requests that the VM monitor makes to host-kernel space or
 * host-user space.
 */
#define MonReqNone              0
#define MonReqFlushPrintBuf     1
#define MonReqRedirect          4 /* Only to host-kernel. */
#define MonReqRemapMonitor      5
#define MonReqGuestFault        6
#define MonReqPanic             8

#define VMStateFDOpened         0x001
#define VMStateMemAllocated     0x002
#define VMStateGuestCPUID       0x004
#define VMStateMMapPhyMem       0x008
#define VMStateMMapPrintBuffer  0x010
#define VMStateMMapGuestCPU     0x020
#define VMStateInitMonitor      0x040
#define VMStateMapMonitor       0x080
#define VMStatePanic            0x100

#define VMStateReady (VMStateFDOpened | VMStateMemAllocated | \
          VMStateGuestCPUID | VMStateMMapPhyMem | VMStateMMapPrintBuffer | \
          VMStateMMapGuestCPU | VMStateInitMonitor | VMStateMapMonitor )
#define VMStateMMapAll \
          (VMStateMMapPhyMem | VMStateMMapPrintBuffer | VMStateMMapGuestCPU)

typedef struct {
  unsigned state;
  unsigned request;
  unsigned guestFaultNo;
  } plex86MonitorState_t;

typedef struct {
#define Plex86ExecuteMethodNative       10
#define Plex86ExecuteMethodBreakpoint   11
  unsigned executeMethod;

  /* User space --> Monitor space. */
  Bit64u               cyclesRequested;
  unsigned             instructionsRequested;

  /* Monitor space --> User space. */
  Bit64u               cyclesExecuted;
  unsigned             instructionsExecuted;
  plex86MonitorState_t monitorState;
  } plex86IoctlExecute_t;

typedef struct {
  unsigned nMegs;
  Bit8u   *vector;
  } plex86IoctlRegisterMem_t;

#endif  /* #ifndef __PLEX86_H__ */
