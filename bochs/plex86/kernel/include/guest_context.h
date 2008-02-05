/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  guest_context.h: monitor stack frame after exception/interrupt
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

#ifndef __GUEST_CONTEXT_H__
#define __GUEST_CONTEXT_H__

#include "eflags.h"

/* This is the guest context (from ring3) pushed on the monitor stack (ring0)
 * during an exception/interrupt.  Part is pushed automatically by the
 * CPU, part by the interrupt handling code.
 *
 * Values are pushed starting with the end of this structure, towards
 * the beginning, since stack pushes descend in address.
 */
typedef struct {
  Bit32u  gs;
  Bit32u  fs;
  Bit32u  ds;
  Bit32u  es;

  Bit32u  edi;
  Bit32u  esi;
  Bit32u  ebp;
  Bit32u  dummy_esp;
  Bit32u  ebx;
  Bit32u  edx;
  Bit32u  ecx;
  Bit32u  eax;

  Bit32u  vector;
  Bit32u  error;

  Bit32u  eip;
  Bit32u  cs;
  eflags_t  eflags;
  Bit32u  esp;
  Bit32u  ss;
  } guest_context_t;

#endif  /* __GUEST_CONTEXT_H__ */
