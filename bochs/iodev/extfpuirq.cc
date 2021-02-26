/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2021  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

//
// External circuit for MSDOS compatible FPU exceptions
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "extfpuirq.h"

#define LOG_THIS theExternalFpuIrq->

bx_extfpuirq_c *theExternalFpuIrq = NULL;

PLUGIN_ENTRY_FOR_MODULE(extfpuirq)
{
  if (mode == PLUGIN_INIT) {
    theExternalFpuIrq = new bx_extfpuirq_c();
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theExternalFpuIrq, BX_PLUGIN_EXTFPUIRQ);
  } else if (mode == PLUGIN_FINI) {
    delete theExternalFpuIrq;
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_OPTIONAL;
  }
  return(0); // Success
}

bx_extfpuirq_c::bx_extfpuirq_c(void)
{
  put("extfpuirq", "EXFIRQ");
}

bx_extfpuirq_c::~bx_extfpuirq_c(void)
{
  BX_DEBUG(("Exit"));
}

void bx_extfpuirq_c::init(void)
{
  // called once when bochs initializes
  DEV_register_iowrite_handler(this, write_handler, 0x00F0, "External FPU IRQ", 1);
  DEV_register_irq(13, "External FPU IRQ");
}

void bx_extfpuirq_c::reset(unsigned type)
{
  // We should handle IGNNE here
  DEV_pic_lower_irq(13);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_extfpuirq_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_EFI_SMF
  bx_extfpuirq_c *class_ptr = (bx_extfpuirq_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_extfpuirq_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_EFI_SMF

  // We should handle IGNNE here
  DEV_pic_lower_irq(13);
}
