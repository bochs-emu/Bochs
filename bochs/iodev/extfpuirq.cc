/////////////////////////////////////////////////////////////////////////
// $Id: extfpuirq.cc,v 1.2 2003-01-07 08:17:15 cbothamy Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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
// External circuit for MSDOS compatible FPU exceptions
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"

#define LOG_THIS theExternalFpuIrq->

bx_extfpuirq_c *theExternalFpuIrq = NULL;

  int
libextfpuirq_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theExternalFpuIrq = new bx_extfpuirq_c ();
  bx_devices.pluginPci2IsaBridge = theExternalFpuIrq;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theExternalFpuIrq, BX_PLUGIN_PCI2ISA);
  return(0); // Success
}

  void
libextfpuirq_LTX_plugin_fini(void)
{
}

bx_extfpuirq_c::bx_extfpuirq_c(void)
{
  put("EFIRQ");
  settype(EXTFPUIRQLOG);
}

bx_extfpuirq_c::~bx_extfpuirq_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_extfpuirq_c::init(void)
{
  // called once when bochs initializes
  DEV_register_iowrite_handler(this, write_handler, 0x00F0, "External FPU IRQ", 7);
  DEV_register_irq(13, "External FPU IRQ");
}

  void
bx_extfpuirq_c::reset(unsigned type)
{
  // We should handle IGNNE here
  DEV_pic_lower_irq(13);
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_extfpuirq_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_EFI_SMF
  bx_extfpuirq_c *class_ptr = (bx_extfpuirq_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_extfpuirq_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_EFI_SMF

  // We should handle IGNNE here
  DEV_pic_lower_irq(13);
}

