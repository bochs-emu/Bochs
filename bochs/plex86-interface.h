/////////////////////////////////////////////////////////////////////////
//// $Id: plex86-interface.h,v 1.4 2003-01-09 04:03:36 kevinlawton Exp $
///////////////////////////////////////////////////////////////////////////
////
////  Copyright (C) 2002  Kevin P. Lawton
////
////  This library is free software; you can redistribute it and/or
////  modify it under the terms of the GNU Lesser General Public
////  License as published by the Free Software Foundation; either
////  version 2 of the License, or (at your option) any later version.
////
////  This library is distributed in the hope that it will be useful,
////  but WITHOUT ANY WARRANTY; without even the implied warranty of
////  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
////  Lesser General Public License for more details.
////
////  You should have received a copy of the GNU Lesser General Public
////  License along with this library; if not, write to the Free Software
////  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

#include "plex86/plex86.h"

#define Plex86StateMemAllocated     0x01
#define Plex86StateMMapPhyMem       0x02
#define Plex86StateMMapPrintBuffer  0x04
#define Plex86StateMMapGuestCPU     0x08
#define Plex86StateReady            0x0f /* All bits set. */


extern unsigned     plex86State;
extern int          plex86FD;

extern unsigned plex86CpuInfo(BX_CPU_C *cpu);
extern unsigned plex86TearDown(void);
extern unsigned plex86ExecuteInVM(BX_CPU_C *cpu);
extern unsigned plex86RegisterGuestMemory(Bit8u *vector, unsigned bytes);
extern unsigned plex86UnregisterGuestMemory(Bit8u *vector, unsigned bytes);
