/////////////////////////////////////////////////////////////////////////
// $Id: devices.cc,v 1.73 2004-07-01 22:18:20 vruppert Exp $
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
//  I/O port handlers API Copyright (C) 2003 by Frank Cornelis
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


#include "iodev.h"
#define LOG_THIS bx_devices.



/* main memory size (in Kbytes)
 * subtract 1k for extended BIOS area
 * report only base memory, not extended mem
 */
#define BASE_MEMORY_IN_K  640


bx_devices_c bx_devices;




// constructor for bx_devices_c
bx_devices_c::bx_devices_c(void)
{
  put("DEV");
  settype(DEVLOG);

  read_port_to_handler = NULL;
  write_port_to_handler = NULL;
#if BX_PCI_SUPPORT
  pluginPciBridge = &stubPci;
  pluginPci2IsaBridge = &stubPci2Isa;
  pluginPciIdeController = NULL;
#if BX_PCI_VGA_SUPPORT
    pluginPciVgaAdapter = NULL;
#endif
#if BX_PCI_DEV_SUPPORT
    pluginPciDevAdapter = NULL;
#endif
#if BX_PCI_USB_SUPPORT
    pluginPciUSBAdapter = NULL;
#endif        
#if BX_PCI_PNIC_SUPPORT
    pluginPciPNicAdapter = NULL;
#endif
#endif
  pit = NULL;
  pluginKeyboard = &stubKeyboard;
  pluginDmaDevice = &stubDma;
  pluginFloppyDevice = &stubFloppy;
  pluginBiosDevice = NULL;
  pluginCmosDevice = &stubCmos;
  pluginSerialDevice = NULL;
  pluginParallelDevice = NULL;
  pluginUnmapped = NULL;
  pluginVgaDevice = &stubVga;
  pluginPicDevice = &stubPic;
  pluginHardDrive = &stubHardDrive;
  pluginSB16Device = NULL;
  pluginNE2kDevice =&stubNE2k;
  pluginExtFpuIrq = NULL;
  pluginGameport = NULL;
  pluginSpeaker = &stubSpeaker;
  g2h = NULL;
#if BX_IODEBUG_SUPPORT
  iodebug = NULL;
#endif
}


bx_devices_c::~bx_devices_c(void)
{
  // nothing needed for now
  BX_DEBUG(("Exit."));
  timer_handle = BX_NULL_TIMER_HANDLE;
}


  void
bx_devices_c::init(BX_MEM_C *newmem)
{
  unsigned i;

  BX_DEBUG(("Init $Id: devices.cc,v 1.73 2004-07-01 22:18:20 vruppert Exp $"));
  mem = newmem;

  /* set no-default handlers, will be overwritten by the real default handler */
  io_read_handlers.next = &io_read_handlers;
  io_read_handlers.prev = &io_read_handlers;
  io_read_handlers.handler_name  = "Default";
  io_read_handlers.funct         = (void *)&default_read_handler;
  io_read_handlers.this_ptr      = NULL;
  io_read_handlers.usage_count = 0; // not used with the default handler
  io_read_handlers.mask          = 7;
  
  io_write_handlers.next = &io_write_handlers;
  io_write_handlers.prev = &io_write_handlers;
  io_write_handlers.handler_name = "Default";
  io_write_handlers.funct        = (void *)&default_write_handler;
  io_write_handlers.this_ptr     = NULL;
  io_write_handlers.usage_count = 0; // not used with the default handler
  io_write_handlers.mask         = 7;

  if (read_port_to_handler)
	  delete [] read_port_to_handler;
  if (write_port_to_handler)
	  delete [] write_port_to_handler;
  read_port_to_handler = new struct io_handler_struct *[PORTS];
  write_port_to_handler = new struct io_handler_struct *[PORTS];

  /* set handlers to the default one */
  for (i=0; i < PORTS; i++) {
    read_port_to_handler[i] = &io_read_handlers; 
    write_port_to_handler[i] = &io_write_handlers; 
  }

  for (i=0; i < BX_MAX_IRQS; i++) {
    irq_handler_name[i] = NULL;
    }

  // BBD: At present, the only difference between "core" and "optional"
  // plugins is that initialization and reset of optional plugins is handled
  // by the plugin device list ().  Init and reset of core plugins is done
  // "by hand" in this file.  Basically, we're using core plugins when we
  // want to control the init order.
  //
  // CB: UNMAPPED and BIOSDEV should maybe be optional
  PLUG_load_plugin(unmapped, PLUGTYPE_CORE);
  PLUG_load_plugin(biosdev, PLUGTYPE_CORE);
  PLUG_load_plugin(cmos, PLUGTYPE_CORE);
  PLUG_load_plugin(dma, PLUGTYPE_CORE);
  PLUG_load_plugin(pic, PLUGTYPE_CORE);
  PLUG_load_plugin(vga, PLUGTYPE_CORE);
  PLUG_load_plugin(floppy, PLUGTYPE_CORE);
  PLUG_load_plugin(harddrv, PLUGTYPE_OPTIONAL);
  PLUG_load_plugin(keyboard, PLUGTYPE_OPTIONAL);
  if (is_serial_enabled ())
    PLUG_load_plugin(serial, PLUGTYPE_OPTIONAL);
  if (is_parallel_enabled ()) 
    PLUG_load_plugin(parallel, PLUGTYPE_OPTIONAL);
  PLUG_load_plugin(extfpuirq, PLUGTYPE_OPTIONAL);
#if BX_SUPPORT_GAME
  PLUG_load_plugin(gameport, PLUGTYPE_OPTIONAL);
#endif
  PLUG_load_plugin(speaker, PLUGTYPE_OPTIONAL);

  // Start with registering the default (unmapped) handler
  pluginUnmapped->init ();

  // NE2000 NIC
  if (bx_options.ne2k.Opresent->get ()) {
#if BX_NE2K_SUPPORT
    PLUG_load_plugin(ne2k, PLUGTYPE_OPTIONAL);
#else
    BX_ERROR(("Bochs is not compiled with NE2K support"));
#endif
  }

  // PCI logic (i440FX)
  if (bx_options.Oi440FXSupport->get ()) {
#if BX_PCI_SUPPORT
    PLUG_load_plugin(pci, PLUGTYPE_CORE);
    PLUG_load_plugin(pci2isa, PLUGTYPE_OPTIONAL);
    PLUG_load_plugin(pci_ide, PLUGTYPE_OPTIONAL);
#if BX_PCI_VGA_SUPPORT
    PLUG_load_plugin(pcivga, PLUGTYPE_OPTIONAL);
#endif
#if BX_PCI_USB_SUPPORT
    PLUG_load_plugin(pciusb, PLUGTYPE_OPTIONAL);
#endif
#if BX_PCI_DEV_SUPPORT
    PLUG_load_plugin(pcidev, PLUGTYPE_OPTIONAL);
#endif
#if BX_PCI_PNIC_SUPPORT && BX_NE2K_SUPPORT
  if (bx_options.ne2k.Opresent->get ()) {
    PLUG_load_plugin(pcipnic, PLUGTYPE_OPTIONAL);
  }
#endif
#else
    BX_ERROR(("Bochs is not compiled with PCI support"));
#endif
  }

#if BX_SUPPORT_APIC
    // I/O APIC 82093AA
    ioapic = & bx_ioapic;
    ioapic->init ();
#endif

  // BIOS log 
  pluginBiosDevice->init ();

  // CMOS RAM & RTC
  pluginCmosDevice->init ();

  /*--- 8237 DMA ---*/
  pluginDmaDevice->init();

  //--- FLOPPY ---
  pluginFloppyDevice->init();

  //--- SOUND ---
  if (bx_options.sb16.Opresent->get ()) {
#if BX_SUPPORT_SB16
    PLUG_load_plugin(sb16, PLUGTYPE_OPTIONAL);
#else
    BX_ERROR(("Bochs is not compiled with SB16 support"));
#endif
  }

#if BX_PCI_SUPPORT
  if (pluginPciBridge) pluginPciBridge->init ();
#endif

  /*--- VGA adapter ---*/
  pluginVgaDevice->init ();

  /*--- 8259A PIC ---*/
  pluginPicDevice->init();

  /*--- 8254 PIT ---*/
  pit = & bx_pit;
  pit->init();

  bx_virt_timer.init();

  bx_slowdown_timer.init();

#if BX_IODEBUG_SUPPORT
  iodebug = &bx_iodebug;
  iodebug->init();
#endif

#if 0
  // Guest to Host interface.  Used with special guest drivers
  // which move data to/from the host environment.
  g2h = &bx_g2h;
  g2h->init();
#endif

  // system hardware
  register_io_read_handler( this,
                            &read_handler,
                            0x0092,
                            "Port 92h System Control", 1 );
  register_io_write_handler(this,
                            &write_handler,
                            0x0092,
                            "Port 92h System Control", 1 );

  // misc. CMOS
  Bit32u extended_memory_in_k = mem->get_memory_in_k() > 1024 ? (mem->get_memory_in_k() - 1024) : 0;
  if (extended_memory_in_k > 0xffff) extended_memory_in_k = 0xffff;

  DEV_cmos_set_reg(0x15, (Bit8u) BASE_MEMORY_IN_K);
  DEV_cmos_set_reg(0x16, (Bit8u) (BASE_MEMORY_IN_K >> 8));
  DEV_cmos_set_reg(0x17, (Bit8u) (extended_memory_in_k & 0xff) );
  DEV_cmos_set_reg(0x18, (Bit8u) ((extended_memory_in_k >> 8) & 0xff) );
  DEV_cmos_set_reg(0x30, (Bit8u) (extended_memory_in_k & 0xff) );
  DEV_cmos_set_reg(0x31, (Bit8u) ((extended_memory_in_k >> 8) & 0xff) );

  Bit32u extended_memory_in_64k = mem->get_memory_in_k() > 16384 ? (mem->get_memory_in_k() - 16384) / 64 : 0;
  if (extended_memory_in_64k > 0xffff) extended_memory_in_64k = 0xffff;

  DEV_cmos_set_reg(0x34, (Bit8u) (extended_memory_in_64k & 0xff) );
  DEV_cmos_set_reg(0x35, (Bit8u) ((extended_memory_in_64k >> 8) & 0xff) );

  if (timer_handle != BX_NULL_TIMER_HANDLE) {
    timer_handle = bx_pc_system.register_timer( this, timer_handler,
      (unsigned) BX_IODEV_HANDLER_PERIOD, 1, 1, "devices.cc");
  }

  // Clear fields for bulk IO acceleration transfers.
  bulkIOHostAddr = 0;
  bulkIOQuantumsRequested = 0;
  bulkIOQuantumsTransferred = 0;

  bx_init_plugins();

  /* now perform checksum of CMOS memory */
  DEV_cmos_checksum();
}


  void
bx_devices_c::reset(unsigned type)
{
  pluginUnmapped->reset(type);
#if BX_PCI_SUPPORT
  if (bx_options.Oi440FXSupport->get ()) {
    pluginPciBridge->reset(type);
    pluginPci2IsaBridge->reset(type);
    pluginPciIdeController->reset(type);
#if BX_PCI_VGA_SUPPORT
    pluginPciVgaAdapter->reset(type);
#endif
#if BX_PCI_DEV_SUPPORT
    pluginPciDevAdapter->reset(type);
#endif
#if BX_PCI_USB_SUPPORT
    pluginPciUSBAdapter->reset(type);
#endif
#if BX_PCI_PNIC_SUPPORT
    if (pluginPciPNicAdapter) pluginPciPNicAdapter->reset(type);
#endif
  }
#endif
#if BX_SUPPORT_IOAPIC
  ioapic->reset (type);
#endif
  pluginBiosDevice->reset(type);
  pluginCmosDevice->reset(type);
  pluginDmaDevice->reset(type);
  pluginFloppyDevice->reset(type);
#if BX_SUPPORT_SB16
  if (pluginSB16Device) pluginSB16Device->reset(type);
#endif
  pluginVgaDevice->reset(type);
  pluginPicDevice->reset(type);
  pit->reset(type);
  bx_slowdown_timer.reset(type);
#if BX_IODEBUG_SUPPORT
  iodebug->reset(type);
#endif
#if BX_NE2K_SUPPORT
  if (pluginNE2kDevice) pluginNE2kDevice->reset(type);
#endif

  bx_reset_plugins(type);
}


  Bit32u
bx_devices_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_DEV_SMF
  bx_devices_c *class_ptr = (bx_devices_c *) this_ptr;

  return( class_ptr->port92_read(address, io_len) );
}


  Bit32u
bx_devices_c::port92_read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_DEV_SMF

  BX_DEBUG(("port92h read partially supported!!!"));
  BX_DEBUG(("  returning %02x", (unsigned) (BX_GET_ENABLE_A20() << 1)));
  return(BX_GET_ENABLE_A20() << 1);
}


  void
bx_devices_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_DEV_SMF
  bx_devices_c *class_ptr = (bx_devices_c *) this_ptr;

  class_ptr->port92_write(address, value, io_len);
}

  void
bx_devices_c::port92_write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_DEV_SMF

  BX_DEBUG(("port92h write of %02x partially supported!!!", (unsigned) value));
  BX_DEBUG(("A20: set_enable_a20() called"));
  BX_SET_ENABLE_A20( (value & 0x02) >> 1 );
  BX_DEBUG(("A20: now %u", (unsigned) BX_GET_ENABLE_A20()));
  if (value & 0x01) { /* high speed reset */
    BX_INFO(("iowrite to port0x92 : reset resquested"));
    bx_pc_system.Reset(BX_RESET_SOFTWARE);
  }
}


// This defines a no-default read handler, 
// so Bochs does not segfault if unmapped is not loaded
  Bit32u
bx_devices_c::default_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  UNUSED(this_ptr);
  BX_PANIC(("No default io-read handler found for 0x%04x/%d. Unmapped io-device not loaded ?", address, io_len));
  return 0xffffffff;
}

// This defines a no-default write handler, 
// so Bochs does not segfault if unmapped is not loaded
  void
bx_devices_c::default_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  UNUSED(this_ptr);
  BX_PANIC(("No default io-write handler found for 0x%04x/%d. Unmapped io-device not loaded ?", address, io_len));
}

  void
bx_devices_c::timer_handler(void *this_ptr)
{
  bx_devices_c *class_ptr = (bx_devices_c *) this_ptr;

  class_ptr->timer();
}

  void
bx_devices_c::timer()
{
#if (BX_USE_NEW_PIT==0)
  if ( pit->periodic( BX_IODEV_HANDLER_PERIOD ) ) {
    // This is a hack to make the IRQ0 work
    DEV_pic_lower_irq(0);
    DEV_pic_raise_irq(0);
    }
#endif


  // separate calls to bx_gui->handle_events from the keyboard code.
  {
    static int multiple=0;
    if ( ++multiple==10)
    {
      multiple=0;
      SIM->periodic ();
      if (!BX_CPU(0)->kill_bochs_request)
	bx_gui->handle_events();
    }
  }

// KPL Removed lapic periodic timer registration here.
}


  bx_bool
bx_devices_c::register_irq(unsigned irq, const char *name)
{
  if (irq >= BX_MAX_IRQS) {
    BX_PANIC(("IO device %s registered with IRQ=%d above %u",
             name, irq, (unsigned) BX_MAX_IRQS-1));
    return false;
    }
  if (irq_handler_name[irq]) {
    BX_PANIC(("IRQ %u conflict, %s with %s", irq,
      irq_handler_name[irq], name));
    return false;
    }
  irq_handler_name[irq] = name;
  return true;
}

  bx_bool
bx_devices_c::unregister_irq(unsigned irq, const char *name)
{
  if (irq >= BX_MAX_IRQS) {
    BX_PANIC(("IO device %s tried to unregister IRQ %d above %u",
             name, irq, (unsigned) BX_MAX_IRQS-1));
    return false;
    }

  if (!irq_handler_name[irq]) {
    BX_INFO(("IO device %s tried to unregister IRQ %d, not registered",
	      name, irq));
    return false;
  }

  if (strcmp(irq_handler_name[irq], name)) {
    BX_INFO(("IRQ %u not registered to %s but to %s", irq,
      name, irq_handler_name[irq]));
    return false;
    }
  irq_handler_name[irq] = NULL;
  return true;
}

  bx_bool
bx_devices_c::register_io_read_handler( void *this_ptr, bx_read_handler_t f,
                                        Bit32u addr, const char *name, Bit8u mask )
{
  addr &= 0x0000ffff;
  
  if (!f)
	  return false;

  /* first check if the port already has a handlers != the default handler */
  if (read_port_to_handler[addr] &&
     read_port_to_handler[addr] != &io_read_handlers) { // the default
	  BX_ERROR(("IO device address conflict(read) at IO address %Xh",
				  (unsigned) addr));
	  BX_ERROR(("  conflicting devices: %s & %s",
				  read_port_to_handler[addr]->handler_name, name));
  	  return false;
  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_read_handlers;
  struct io_handler_struct *io_read_handler = NULL;
  do {
	if (curr->funct == f && 
			curr->mask == mask && 
			curr->this_ptr == this_ptr &&
			curr->handler_name == name) { // really want the same name too
		io_read_handler = curr;
		break;
	}
	curr = curr->next;
  } while (curr->next != &io_read_handlers);
  
  if (!io_read_handler) {
	io_read_handler = new struct io_handler_struct;
	io_read_handler->funct = (void *)f;
	io_read_handler->this_ptr = this_ptr;
	io_read_handler->handler_name = name;
	io_read_handler->mask = mask;
	io_read_handler->usage_count = 0;
	// add the handler to the double linked list of handlers
	io_read_handlers.prev->next = io_read_handler;
	io_read_handler->next = &io_read_handlers;
	io_read_handler->prev = io_read_handlers.prev;
	io_read_handlers.prev = io_read_handler;
  }

  io_read_handler->usage_count++;
  read_port_to_handler[addr] = io_read_handler;
  return true; // address mapped successfully
}



  bx_bool
bx_devices_c::register_io_write_handler( void *this_ptr, bx_write_handler_t f,
                                        Bit32u addr, const char *name, Bit8u mask )
{
  addr &= 0x0000ffff;

  if (!f)
	  return false;
  
  /* first check if the port already has a handlers != the default handler */
  if (write_port_to_handler[addr] &&
     write_port_to_handler[addr] != &io_write_handlers) { // the default
	  BX_ERROR(("IO device address conflict(write) at IO address %Xh",
				  (unsigned) addr));
	  BX_ERROR(("  conflicting devices: %s & %s",
				  write_port_to_handler[addr]->handler_name, name));
  	  return false;
  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_write_handlers;
  struct io_handler_struct *io_write_handler = NULL;
  do {
	if (curr->funct == f && 
			curr->mask == mask && 
			curr->this_ptr == this_ptr &&
			curr->handler_name == name) { // really want the same name too
		io_write_handler = curr;
		break;
	}
	curr = curr->next;
  } while (curr->next != &io_write_handlers);
  
  if (!io_write_handler) {
	io_write_handler = new struct io_handler_struct;
	io_write_handler->funct = (void *)f;
	io_write_handler->this_ptr = this_ptr;
	io_write_handler->handler_name = name;
	io_write_handler->mask = mask;
	io_write_handler->usage_count = 0;
	// add the handler to the double linked list of handlers
	io_write_handlers.prev->next = io_write_handler;
	io_write_handler->next = &io_write_handlers;
	io_write_handler->prev = io_write_handlers.prev;
	io_write_handlers.prev = io_write_handler;
  }

  io_write_handler->usage_count++;
  write_port_to_handler[addr] = io_write_handler;
  return true; // address mapped successfully
}


  bx_bool
bx_devices_c::register_io_read_handler_range( void *this_ptr, bx_read_handler_t f,
                                        Bit32u begin_addr, Bit32u end_addr, const char *name, Bit8u mask )
{
  Bit32u addr;
  begin_addr &= 0x0000ffff;
  end_addr &= 0x0000ffff;

  if (end_addr < begin_addr) {
	  BX_ERROR(("!!! end_addr < begin_addr !!!"));
	  return false;
  }
  
  if (!f) {
	  BX_ERROR(("!!! f == NULL !!!"));
	  return false;
  }

  /* first check if the port already has a handlers != the default handler */
  for (addr = begin_addr; addr <= end_addr; addr++)
	  if (read_port_to_handler[addr] &&
			  read_port_to_handler[addr] != &io_read_handlers) { // the default
		  BX_ERROR(("IO device address conflict(read) at IO address %Xh",
					  (unsigned) addr));
		  BX_ERROR(("  conflicting devices: %s & %s",
					  read_port_to_handler[addr]->handler_name, name));
		  return false;
	  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_read_handlers;
  struct io_handler_struct *io_read_handler = NULL;
  do {
	if (curr->funct == f && 
			curr->mask == mask && 
			curr->this_ptr == this_ptr &&
			curr->handler_name == name) {
		io_read_handler = curr;
		break;
	}
	curr = curr->next;
  } while (curr->next != &io_read_handlers);
  
  if (!io_read_handler) {
	io_read_handler = new struct io_handler_struct;
	io_read_handler->funct = (void *)f;
	io_read_handler->this_ptr = this_ptr;
	io_read_handler->handler_name = name;
	io_read_handler->mask = mask;
	io_read_handler->usage_count = 0;
	// add the handler to the double linked list of handlers
	io_read_handlers.prev->next = io_read_handler;
	io_read_handler->next = &io_read_handlers;
	io_read_handler->prev = io_read_handlers.prev;
	io_read_handlers.prev = io_read_handler;
  }

  io_read_handler->usage_count += end_addr - begin_addr + 1;
  for (addr = begin_addr; addr <= end_addr; addr++)
	  read_port_to_handler[addr] = io_read_handler;
  return true; // address mapped successfully
}


  bx_bool
bx_devices_c::register_io_write_handler_range( void *this_ptr, bx_write_handler_t f,
                                        Bit32u begin_addr, Bit32u end_addr, const char *name, Bit8u mask )
{
  Bit32u addr;
  begin_addr &= 0x0000ffff;
  end_addr &= 0x0000ffff;

  if (end_addr < begin_addr) {
	  BX_ERROR(("!!! end_addr < begin_addr !!!"));
	  return false;
  }
  
  if (!f) {
	  BX_ERROR(("!!! f == NULL !!!"));
	  return false;
  }

  /* first check if the port already has a handlers != the default handler */
  for (addr = begin_addr; addr <= end_addr; addr++)
	  if (write_port_to_handler[addr] &&
			  write_port_to_handler[addr] != &io_write_handlers) { // the default
		  BX_ERROR(("IO device address conflict(read) at IO address %Xh",
					  (unsigned) addr));
		  BX_ERROR(("  conflicting devices: %s & %s",
					  write_port_to_handler[addr]->handler_name, name));
		  return false;
	  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_write_handlers;
  struct io_handler_struct *io_write_handler = NULL;
  do {
	if (curr->funct == f && 
			curr->mask == mask && 
			curr->this_ptr == this_ptr &&
			curr->handler_name == name) {
		io_write_handler = curr;
		break;
	}
	curr = curr->next;
  } while (curr->next != &io_write_handlers);
  
  if (!io_write_handler) {
	io_write_handler = new struct io_handler_struct;
	io_write_handler->funct = (void *)f;
	io_write_handler->this_ptr = this_ptr;
	io_write_handler->handler_name = name;
	io_write_handler->mask = mask;
	io_write_handler->usage_count = 0;
	// add the handler to the double linked list of handlers
	io_write_handlers.prev->next = io_write_handler;
	io_write_handler->next = &io_write_handlers;
	io_write_handler->prev = io_write_handlers.prev;
	io_write_handlers.prev = io_write_handler;
  }

  io_write_handler->usage_count += end_addr - begin_addr + 1;
  for (addr = begin_addr; addr <= end_addr; addr++)
	  write_port_to_handler[addr] = io_write_handler;
  return true; // address mapped successfully
}


// Registration of default handlers (mainly be the unmapped device)
  bx_bool
bx_devices_c::register_default_io_read_handler( void *this_ptr, bx_read_handler_t f,
                                        const char *name, Bit8u mask )
{
	io_read_handlers.funct = (void *)f;
	io_read_handlers.this_ptr = this_ptr;
	io_read_handlers.handler_name = name;
	io_read_handlers.mask = mask;
	return true; 
}



  bx_bool
bx_devices_c::register_default_io_write_handler( void *this_ptr, bx_write_handler_t f,
                                        const char *name, Bit8u mask )
{
	io_write_handlers.funct = (void *)f;
	io_write_handlers.this_ptr = this_ptr;
	io_write_handlers.handler_name = name;
	io_write_handlers.mask = mask;
	return true; 
}


  bx_bool
bx_devices_c::unregister_io_read_handler( void *this_ptr, bx_read_handler_t f,
                                        Bit32u addr, Bit8u mask )
{
	addr &= 0x0000ffff;
	
	struct io_handler_struct *io_read_handler = read_port_to_handler[addr];
	
	//BX_INFO(("Unregistering I/O read handler at %#x", addr));
	
	if (!io_read_handler) {
		BX_ERROR((">>> NO IO_READ_HANDLER <<<"));
		return false;
	}

	if (io_read_handler == &io_read_handlers) {
		BX_ERROR((">>> CANNOT UNREGISTER THE DEFAULT IO_READ_HANDLER <<<"));
		return false; // cannot unregister the default handler
	}

	if (io_read_handler->funct != f) {
		BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER FUNC <<<"));
		return false;
	}

	if (io_read_handler->this_ptr != this_ptr) {
		BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER THIS_PTR <<<"));
		return false;
	}

	if (io_read_handler->mask != mask) {
		BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER MASK <<<"));
		return false;
	}

	read_port_to_handler[addr] = &io_read_handlers; // reset to default
	io_read_handler->usage_count--;

	if (!io_read_handler->usage_count) { // kill this handler entry
		io_read_handler->prev->next = io_read_handler->next;
		io_read_handler->next->prev = io_read_handler->prev;
		delete io_read_handler;
	}
	return true;
}


  bx_bool
bx_devices_c::unregister_io_write_handler( void *this_ptr, bx_write_handler_t f,
                                        Bit32u addr, Bit8u mask )
{
	addr &= 0x0000ffff;
	
	struct io_handler_struct *io_write_handler = write_port_to_handler[addr];
	
	if (!io_write_handler)
		return false;

	if (io_write_handler == &io_write_handlers)
		return false; // cannot unregister the default handler

	if (io_write_handler->funct != f)
		return false;

	if (io_write_handler->this_ptr != this_ptr)
		return false;

	if (io_write_handler->mask != mask)
		return false;

	write_port_to_handler[addr] = &io_write_handlers; // reset to default
	io_write_handler->usage_count--;

	if (!io_write_handler->usage_count) { // kill this handler entry
		io_write_handler->prev->next = io_write_handler->next;
		io_write_handler->next->prev = io_write_handler->prev;
		delete io_write_handler;
	}
	return true;
}


  bx_bool
bx_devices_c::unregister_io_read_handler_range( void *this_ptr, bx_read_handler_t f,
                                        Bit32u begin, Bit32u end, Bit8u mask )
{
	begin &= 0x0000ffff;
	end &= 0x0000ffff;
	Bit32u addr;
	bx_bool ret = true;

	/*
	 * the easy way this time
	 */
	for (addr = begin; addr <= end; addr++)
		if (!unregister_io_read_handler(this_ptr, f, addr, mask))
			ret = false;

	return ret;
}


  bx_bool
bx_devices_c::unregister_io_write_handler_range( void *this_ptr, bx_write_handler_t f,
                                        Bit32u begin, Bit32u end, Bit8u mask )
{
	begin &= 0x0000ffff;
	end &= 0x0000ffff;
	Bit32u addr;
	bx_bool ret = true;

	/*
	 * the easy way this time
	 */
	for (addr = begin; addr <= end; addr++)
		if (!unregister_io_write_handler(this_ptr, f, addr, mask))
			ret = false;

	return ret;
}


/*
 * Read a byte of data from the IO memory address space
 */

  Bit32u BX_CPP_AttrRegparmN(2)
bx_devices_c::inp(Bit16u addr, unsigned io_len)
{
  struct io_handler_struct *io_read_handler;
  Bit32u ret;

  BX_INSTR_INP(addr, io_len);
  
  io_read_handler = read_port_to_handler[addr];
  if (io_read_handler->mask & io_len) {
	ret = ((bx_read_handler_t)io_read_handler->funct)(io_read_handler->this_ptr, (Bit32u)addr, io_len);
  } else {
    switch (io_len) {
      case 1: ret = 0xff; break;
      case 2: ret = 0xffff; break;
      default: ret = 0xffffffff; break;
    }
    BX_ERROR(("read from port 0x%04x with len %d returns 0x%x", addr, io_len, ret));
  }
  BX_INSTR_INP2(addr, io_len, ret);
  BX_DBG_IO_REPORT(addr, io_len, BX_READ, ret);
  return(ret);
}


/*
 * Write a byte of data to the IO memory address space.
 */

  void BX_CPP_AttrRegparmN(3)
bx_devices_c::outp(Bit16u addr, Bit32u value, unsigned io_len)
{
  struct io_handler_struct *io_write_handler;

  BX_INSTR_OUTP(addr, io_len);
  BX_INSTR_OUTP2(addr, io_len, value);

  BX_DBG_IO_REPORT(addr, io_len, BX_WRITE, value);
  
  io_write_handler = write_port_to_handler[addr];
  if (io_write_handler->mask & io_len) {
	((bx_write_handler_t)io_write_handler->funct)(io_write_handler->this_ptr, (Bit32u)addr, value, io_len);
  } else {
    BX_ERROR(("write to port 0x%04x with len %d ignored", addr, io_len));
  }
}

bx_bool bx_devices_c::is_serial_enabled ()
{
  for (int i=0; i<BX_N_SERIAL_PORTS; i++) {
    if (SIM->get_param_bool (BXP_COMx_ENABLED(i+1))->get())
      return true;
  }
  return false;
}

bx_bool bx_devices_c::is_usb_enabled ()
{
  for (int i=0; i<BX_N_USB_HUBS; i++) {
    if (SIM->get_param_bool (BXP_USBx_ENABLED(i+1))->get())
       return true;
  }
  return false;
}

bx_bool bx_devices_c::is_parallel_enabled ()
{
  for (int i=0; i<BX_N_PARALLEL_PORTS; i++) {
    if (SIM->get_param_bool (BXP_PARPORTx_ENABLED(i+1))->get())
      return true;
  }
  return false;
}
