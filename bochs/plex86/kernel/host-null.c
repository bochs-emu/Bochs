/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  host-null.c: host OS specific stubs.  These provide a reference for
 *    ports of plex86 to various host OSes.
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

#include "plex86.h"
#define IN_HOST_SPACE
#include "monitor.h"

/* Note: for comments on what various functions are expected to do, as
 *   well as a reference implemntation, read the 'host-linux.c' file.
 *   It's likely the most up-to-date.
 */


/* OS specific includes here. */


/* Some declarations for the entry points etc here. */




kernelModulePages_t kernelModulePages;

#define NULL 0

  int
main(int argc, char *argv[])
{
  vm_t *vm = NULL;

  hostModuleInit();
  hostDeviceOpen(vm);
  hostIoctlGeneric(vm, NULL, NULL, 0, 0);
  return(0);
}

  void
hostOSReservePhyPages(vm_t *vm, Bit32u *hostPhyPages, unsigned nPages)
{
}

  void
hostOSUnreservePhyPages(vm_t *vm, Bit32u *hostPhyPages, unsigned nPages)
{
}



  unsigned
hostOSIdle(void)
{
  return 0;
}

  void *
hostOSAllocZeroedMem(unsigned long size)
{
  return 0;
}

  void
hostOSFreeMem(void *ptr)
{
}

  void *
hostOSAllocZeroedPage(void)
{
  return 0;
}

  void
hostOSFreePage(void *ptr)
{
}


  unsigned
hostOSGetAllocedMemPhyPages(Bit32u *page, int max_pages, void *ptr, unsigned size)
{
  return 0;
}

  Bit32u
hostOSGetAllocedPagePhyPage(void *ptr)
{
  return 0;
}

  void
hostOSPrint(char *fmt, ...)
{
}


  int
hostOSConvertPlex86Errno(unsigned ret)
{
  return 0;
}


  Bit32u
hostOSKernelOffset(void)
{
  return 0;
}

  void
hostOSModuleCountReset(vm_t *vm, void *inode, void *filp)
{
}

  unsigned long
hostOSCopyFromUser(void *to, void *from, unsigned long len)
{
  return 0;
}

  unsigned long
hostOSCopyToUser(void *to, void *from, unsigned long len)
{
  return 0;
}

  Bit32u
hostOSGetAndPinUserPage(vm_t *vm, Bit32u userAddr, void **osSpecificPtr,
                      Bit32u *ppi, Bit32u *kernelAddr)
{
  return 0;
}

  void
hostOSUnpinUserPage(vm_t *vm, Bit32u userAddr, void *osSpecificPtr,
                          Bit32u ppi, Bit32u *kernelAddr, unsigned dirty)
{
}

  void
hostOSInstrumentIntRedirCount(unsigned interruptVector)
{
}
