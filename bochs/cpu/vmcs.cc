/////////////////////////////////////////////////////////////////////////
// $Id: vmcs.cc,v 1.1 2010-11-11 16:25:45 sshwarts Exp $
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

static unsigned vmcs_map[16][1+VMX_HIGHEST_VMCS_ENCODING];

void BX_CPU_C::init_VMCS(void)
{
  static bx_bool vmcs_map_ready = 0;

  if (vmcs_map_ready) return;
  vmcs_map_ready = 1;

  for (unsigned type=0; type<16; type++) {
    for (unsigned field=0; field <= VMX_HIGHEST_VMCS_ENCODING; field++) {
       vmcs_map[type][field] = 0xffffffff;
    }
  }

#if 1
  // try to build generic VMCS map
  for (unsigned type=0; type<16; type++) {
    for (unsigned field=0; field <= VMX_HIGHEST_VMCS_ENCODING; field++) {
       // allocate 32 fields of 4 byte each per type
       if (vmcs_map[type][field] != 0xffffffff) {
          BX_PANIC(("VMCS type %d field %d is already initialized", type, field));
       }
       vmcs_map[type][field] = VMCS_DATA_OFFSET + (type*64 + field) * 4;
       if(vmcs_map[type][field] >= VMX_VMCS_AREA_SIZE) {
          BX_PANIC(("VMCS type %d field %d is out of VMCS boundaries", type, field));
       }
    }
  }
#else
  // define your own VMCS format
#include "vmcs.h"
#endif
}

unsigned vmcs_field_offset(Bit32u encoding)
{
  return vmcs_map[VMCS_FIELD_INDEX(encoding)][VMCS_FIELD(encoding)];
}
 