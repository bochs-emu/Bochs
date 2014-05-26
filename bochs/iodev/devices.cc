/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////


#include "iodev.h"

#include "iodev/virt_timer.h"
#include "iodev/slowdown_timer.h"

#define LOG_THIS bx_devices.

/* main memory size (in Kbytes)
 * subtract 1k for extended BIOS area
 * report only base memory, not extended mem
 */
#define BASE_MEMORY_IN_K  640


bx_devices_c bx_devices;


// constructor for bx_devices_c
bx_devices_c::bx_devices_c()
{
  put("devices", "DEV");

  read_port_to_handler = NULL;
  write_port_to_handler = NULL;
  io_read_handlers.next = NULL;
  io_read_handlers.handler_name = NULL;
  io_write_handlers.next = NULL;
  io_write_handlers.handler_name = NULL;
  init_stubs();

  for (unsigned i=0; i < BX_MAX_IRQS; i++) {
    irq_handler_name[i] = NULL;
  }
}

bx_devices_c::~bx_devices_c()
{
  // nothing needed for now
  timer_handle = BX_NULL_TIMER_HANDLE;
}

void bx_devices_c::init_stubs()
{
  pluginPci2IsaBridge = &stubPci2Isa;
  pluginPciIdeController = &stubPciIde;
#if BX_SUPPORT_PCI
  pluginACPIController = &stubACPIController;
#endif
  pluginKeyboard = &stubKeyboard;
  pluginDmaDevice = &stubDma;
  pluginFloppyDevice = &stubFloppy;
  pluginCmosDevice = &stubCmos;
  pluginVgaDevice = &stubVga;
  pluginPicDevice = &stubPic;
  pluginHardDrive = &stubHardDrive;
  pluginSpeaker = &stubSpeaker;
#if BX_SUPPORT_IODEBUG
  pluginIODebug = &stubIODebug;
#endif
#if BX_SUPPORT_APIC
  pluginIOAPIC = &stubIOAPIC;
#endif
#if BX_SUPPORT_GAMEPORT
  pluginGameport = &stubGameport;
#endif
#if BX_SUPPORT_PCIUSB
  pluginUsbDevCtl = &stubUsbDevCtl;
#endif
#if BX_SUPPORT_SOUNDLOW
  pluginSoundModCtl = &stubSoundModCtl;
#endif
#if BX_NETWORKING
  pluginNetModCtl = &stubNetModCtl;
#endif
}

void bx_devices_c::init(BX_MEM_C *newmem)
{
  unsigned i, chipset;
  const char def_name[] = "Default";
  const char *vga_ext;

  BX_DEBUG(("Init $Id$"));
  mem = newmem;

  /* set builtin default handlers, will be overwritten by the real default handler */
  register_default_io_read_handler(NULL, &default_read_handler, def_name, 7);
  io_read_handlers.next = &io_read_handlers;
  io_read_handlers.prev = &io_read_handlers;
  io_read_handlers.usage_count = 0; // not used with the default handler

  register_default_io_write_handler(NULL, &default_write_handler, def_name, 7);
  io_write_handlers.next = &io_write_handlers;
  io_write_handlers.prev = &io_write_handlers;
  io_write_handlers.usage_count = 0; // not used with the default handler

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
    delete [] irq_handler_name[i];
    irq_handler_name[i] = NULL;
  }

  // removable devices init
  bx_keyboard.dev = NULL;
  bx_keyboard.enq_event = NULL;
  for (i=0; i < 2; i++) {
    bx_mouse[i].dev = NULL;
    bx_mouse[i].enq_event = NULL;
    bx_mouse[i].enabled_changed = NULL;
  }
  // common mouse settings
  mouse_captured = SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get();
  mouse_type = SIM->get_param_enum(BXPN_MOUSE_TYPE)->get();

  // register as soon as possible - the devices want to have their timers !
  bx_virt_timer.init();
  bx_slowdown_timer.init();

  // BBD: At present, the only difference between "core" and "optional"
  // plugins is that initialization and reset of optional plugins is handled
  // by the plugin device list ().  Init and reset of core plugins is done
  // "by hand" in this file.  Basically, we're using core plugins when we
  // want to control the init order.
  //
  PLUG_load_plugin(hdimage, PLUGTYPE_CORE);
#if BX_NETWORKING
  network_enabled = is_network_enabled();
  if (network_enabled)
    PLUG_load_plugin(netmod, PLUGTYPE_CORE);
#endif
#if BX_SUPPORT_SOUNDLOW
  sound_enabled = is_sound_enabled();
  if (sound_enabled) {
    PLUG_load_plugin(soundmod, PLUGTYPE_CORE);
    pluginSoundModCtl->init();
  }
#endif
  // PCI logic (i440FX)
  pci.enabled = SIM->get_param_bool(BXPN_PCI_ENABLED)->get();
  if (pci.enabled) {
    chipset = SIM->get_param_enum(BXPN_PCI_CHIPSET)->get();
#if BX_SUPPORT_PCI
    PLUG_load_plugin(pci, PLUGTYPE_CORE);
    PLUG_load_plugin(pci2isa, PLUGTYPE_CORE);
#if BX_SUPPORT_PCIUSB
    if (chipset == BX_PCI_CHIPSET_I440FX) {
      // UHCI is a part of the PIIX3, so load / enable it
      if (!PLUG_device_present("usb_uhci")) {
        PLUG_load_plugin(usb_uhci, PLUGTYPE_OPTIONAL);
      }
      SIM->get_param_bool(BXPN_UHCI_ENABLED)->set(1);
    }
    usb_enabled = is_usb_enabled();
    if (usb_enabled)
      PLUG_load_plugin(usb_common, PLUGTYPE_CORE);
#endif
    if (chipset == BX_PCI_CHIPSET_I440FX) {
      PLUG_load_plugin(acpi, PLUGTYPE_STANDARD);
    }
#else
    BX_ERROR(("Bochs is not compiled with PCI support"));
#endif
  }
  PLUG_load_plugin(cmos, PLUGTYPE_CORE);
  PLUG_load_plugin(dma, PLUGTYPE_CORE);
  PLUG_load_plugin(pic, PLUGTYPE_CORE);
  PLUG_load_plugin(pit, PLUGTYPE_CORE);
  vga_ext = SIM->get_param_string(BXPN_VGA_EXTENSION)->getptr();
  if (!strcmp(vga_ext, "cirrus")) {
#if BX_SUPPORT_CLGD54XX
    PLUG_load_plugin(svga_cirrus, PLUGTYPE_CORE);
#else
    BX_PANIC(("Bochs is not compiled with Cirrus support"));
#endif
  } else {
    PLUG_load_plugin(vga, PLUGTYPE_CORE);
  }
  PLUG_load_plugin(floppy, PLUGTYPE_CORE);

#if BX_SUPPORT_APIC
  PLUG_load_plugin(ioapic, PLUGTYPE_STANDARD);
#endif
  PLUG_load_plugin(keyboard, PLUGTYPE_STANDARD);
#if BX_SUPPORT_BUSMOUSE
  if (mouse_type == BX_MOUSE_TYPE_BUS) {
    PLUG_load_plugin(busmouse, PLUGTYPE_OPTIONAL);
  }
#endif
  if (is_harddrv_enabled()) {
    PLUG_load_plugin(harddrv, PLUGTYPE_STANDARD);
#if BX_SUPPORT_PCI
    if (pci.enabled) {
      PLUG_load_plugin(pci_ide, PLUGTYPE_STANDARD);
    }
#endif
  }

  // system hardware
  register_io_read_handler(this, &read_handler, 0x0092,
                           "Port 92h System Control", 1);
  register_io_write_handler(this, &write_handler, 0x0092,
                            "Port 92h System Control", 1);
#if BX_SUPPORT_PCI
  if (pci.enabled) {
    pci.num_pci_handlers = 0;

    /* set unused elements to appropriate values */
    for (i=0; i < BX_MAX_PCI_DEVICES; i++) {
      pci.pci_handler[i].handler = NULL;
    }

    for (i=0; i < 0x100; i++) {
      pci.handler_id[i] = BX_MAX_PCI_DEVICES;  // not assigned
    }

    for (i=0; i < BX_N_PCI_SLOTS; i++) {
      pci.slot_used[i] = 0;  // no device connected
    }

    // confAddr accepts dword i/o only
    DEV_register_ioread_handler(this, read_handler, 0x0CF8, "i440FX", 4);
    DEV_register_iowrite_handler(this, write_handler, 0x0CF8, "i440FX", 4);

    for (i=0x0CFC; i<=0x0CFF; i++) {
      DEV_register_ioread_handler(this, read_handler, i, "i440FX", 7);
      DEV_register_iowrite_handler(this, write_handler, i, "i440FX", 7);
    }
  }
#endif

  // misc. CMOS
  Bit64u memory_in_k = mem->get_memory_len() / 1024;
  Bit64u extended_memory_in_k = memory_in_k > 1024 ? (memory_in_k - 1024) : 0;
  if (extended_memory_in_k > 0xfc00) extended_memory_in_k = 0xfc00;

  DEV_cmos_set_reg(0x15, (Bit8u) BASE_MEMORY_IN_K);
  DEV_cmos_set_reg(0x16, (Bit8u) (BASE_MEMORY_IN_K >> 8));
  DEV_cmos_set_reg(0x17, (Bit8u) (extended_memory_in_k & 0xff));
  DEV_cmos_set_reg(0x18, (Bit8u) ((extended_memory_in_k >> 8) & 0xff));
  DEV_cmos_set_reg(0x30, (Bit8u) (extended_memory_in_k & 0xff));
  DEV_cmos_set_reg(0x31, (Bit8u) ((extended_memory_in_k >> 8) & 0xff));

  Bit64u extended_memory_in_64k = memory_in_k > 16384 ? (memory_in_k - 16384) / 64 : 0;
  // Limit to 3 GB - 16 MB. PCI Memory Address Space starts at 3 GB.
  if (extended_memory_in_64k > 0xbf00) extended_memory_in_64k = 0xbf00;

  DEV_cmos_set_reg(0x34, (Bit8u) (extended_memory_in_64k & 0xff));
  DEV_cmos_set_reg(0x35, (Bit8u) ((extended_memory_in_64k >> 8) & 0xff));

  Bit64u memory_above_4gb = (mem->get_memory_len() > BX_CONST64(0x100000000)) ?
                            (mem->get_memory_len() - BX_CONST64(0x100000000)) : 0;
  if (memory_above_4gb) {
    DEV_cmos_set_reg(0x5b, (Bit8u)(memory_above_4gb >> 16));
    DEV_cmos_set_reg(0x5c, (Bit8u)(memory_above_4gb >> 24));
    DEV_cmos_set_reg(0x5d, memory_above_4gb >> 32);
  }

  if (timer_handle != BX_NULL_TIMER_HANDLE) {
    timer_handle = bx_pc_system.register_timer(this, timer_handler,
      (unsigned) BX_IODEV_HANDLER_PERIOD, 1, 1, "devices.cc");
  }

  // Clear fields for bulk IO acceleration transfers.
  bulkIOHostAddr = 0;
  bulkIOQuantumsRequested = 0;
  bulkIOQuantumsTransferred = 0;

  bx_init_plugins();

  /* now perform checksum of CMOS memory */
  DEV_cmos_checksum();

#if BX_SUPPORT_PCI
  // verify PCI slot configuration
  char devname[80];
  char *device;

  if (pci.enabled) {
    for (i=0; i<BX_N_PCI_SLOTS; i++) {
      sprintf(devname, "pci.slot.%d", i+1);
      device = SIM->get_param_string(devname)->getptr();
      if ((strlen(device) > 0) && !pci.slot_used[i]) {
        BX_PANIC(("Unknown plugin '%s' at PCI slot #%d", device, i+1));
      }
    }
  }
#endif
}

void bx_devices_c::reset(unsigned type)
{
#if BX_SUPPORT_PCI
  if (pci.enabled) {
    pci.confAddr = 0;
  }
#endif
  mem->disable_smram();
  bx_reset_plugins(type);
}

void bx_devices_c::register_state()
{
#if BX_SUPPORT_PCI
  if (pci.enabled) {
    bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pcicore", "Generic PCI State");
    BXRS_HEX_PARAM_FIELD(list, confAddr, pci.confAddr);
  }
#endif
  bx_virt_timer.register_state();
  bx_plugins_register_state();
}

void bx_devices_c::after_restore_state()
{
  bx_slowdown_timer.after_restore_state();
  bx_virt_timer.set_realtime_delay();
  bx_plugins_after_restore_state();
}

void bx_devices_c::exit()
{
  // delete i/o handlers before unloading plugins
  struct io_handler_struct *io_read_handler = io_read_handlers.next;
  struct io_handler_struct *curr = NULL;
  while (io_read_handler != &io_read_handlers) {
    io_read_handler->prev->next = io_read_handler->next;
    io_read_handler->next->prev = io_read_handler->prev;
    curr = io_read_handler;
    io_read_handler = io_read_handler->next;
    delete [] curr->handler_name;
    delete curr;
  }
  struct io_handler_struct *io_write_handler = io_write_handlers.next;
  while (io_write_handler != &io_write_handlers) {
    io_write_handler->prev->next = io_write_handler->next;
    io_write_handler->next->prev = io_write_handler->prev;
    curr = io_write_handler;
    io_write_handler = io_write_handler->next;
    delete [] curr->handler_name;
    delete curr;
  }

  bx_virt_timer.setup();
  bx_slowdown_timer.exit();

  // unload optional and user plugins first
  bx_unload_plugins();
  bx_unload_core_plugins();
  PLUG_unload_plugin(hdimage);
#if BX_NETWORKING
  if (network_enabled)
    PLUG_unload_plugin(netmod);
#endif
#if BX_SUPPORT_SOUNDLOW
  if (sound_enabled)
    PLUG_unload_plugin(soundmod);
#endif
#if BX_SUPPORT_PCIUSB
  if (usb_enabled)
    PLUG_unload_plugin(usb_common);
#endif
  init_stubs();
}

Bit32u bx_devices_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_DEV_SMF
  bx_devices_c *class_ptr = (bx_devices_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_devices_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_DEV_SMF

  switch (address) {
    case 0x0092:
      BX_DEBUG(("port92h read partially supported!!!"));
      BX_DEBUG(("  returning %02x", (unsigned) (BX_GET_ENABLE_A20() << 1)));
      return(BX_GET_ENABLE_A20() << 1);
#if BX_SUPPORT_PCI
    case 0x0CF8:
      return BX_DEV_THIS pci.confAddr;
    case 0x0CFC:
    case 0x0CFD:
    case 0x0CFE:
    case 0x0CFF:
    {
      Bit32u handle, retval;
      Bit8u devfunc, regnum;

      if ((BX_DEV_THIS pci.confAddr & 0x80FF0000) == 0x80000000) {
        devfunc = (BX_DEV_THIS pci.confAddr >> 8) & 0xff;
        regnum = (BX_DEV_THIS pci.confAddr & 0xfc) + (address & 0x03);
        handle = BX_DEV_THIS pci.handler_id[devfunc];
        if ((io_len <= 4) && (handle < BX_MAX_PCI_DEVICES))
          retval = BX_DEV_THIS pci.pci_handler[handle].handler->pci_read_handler(regnum, io_len);
        else
          retval = 0xFFFFFFFF;
      }
      else
        retval = 0xFFFFFFFF;
      return retval;
    }
#endif
  }

  BX_PANIC(("unsupported IO read to port 0x%x", (unsigned) address));
  return(0xffffffff);
}

void bx_devices_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_DEV_SMF
  bx_devices_c *class_ptr = (bx_devices_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_devices_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_DEV_SMF

  switch (address) {
    case 0x0092:
      BX_DEBUG(("port92h write of %02x partially supported!!!", (unsigned) value));
      BX_DEBUG(("A20: set_enable_a20() called"));
      BX_SET_ENABLE_A20((value & 0x02) >> 1);
      BX_DEBUG(("A20: now %u", (unsigned) BX_GET_ENABLE_A20()));
      if (value & 0x01) { /* high speed reset */
        BX_INFO(("iowrite to port0x92 : reset resquested"));
        bx_pc_system.Reset(BX_RESET_SOFTWARE);
      }
      break;
#if BX_SUPPORT_PCI
    case 0xCF8:
      BX_DEV_THIS pci.confAddr = value;
      if ((value & 0x80FFFF00) == 0x80000000) {
        BX_DEBUG(("440FX PMC register 0x%02x selected", value & 0xfc));
      } else if ((value & 0x80000000) == 0x80000000) {
        BX_DEBUG(("440FX request for bus 0x%02x device 0x%02x function 0x%02x",
                  (value >> 16) & 0xFF, (value >> 11) & 0x1F, (value >> 8) & 0x07));
      }
      break;

    case 0xCFC:
    case 0xCFD:
    case 0xCFE:
    case 0xCFF:
      if ((BX_DEV_THIS pci.confAddr & 0x80FF0000) == 0x80000000) {
        Bit8u devfunc = (BX_DEV_THIS pci.confAddr >> 8) & 0xff;
        Bit8u regnum = (BX_DEV_THIS pci.confAddr & 0xfc) + (address & 0x03);
        Bit32u handle = BX_DEV_THIS pci.handler_id[devfunc];
        if ((io_len <= 4) && (handle < BX_MAX_PCI_DEVICES)) {
          if (((regnum>=4) && (regnum<=7)) || (regnum==12) || (regnum==13) || (regnum>14)) {
            BX_DEV_THIS pci.pci_handler[handle].handler->pci_write_handler(regnum, value, io_len);
          }
          else
            BX_DEBUG(("read only register, write ignored"));
        }
      }
      break;
#endif
    default:
      BX_PANIC(("IO write to port 0x%x", (unsigned) address));
  }
}

// This defines the builtin default read handler,
// so Bochs does not segfault if unmapped is not loaded
Bit32u bx_devices_c::default_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  UNUSED(this_ptr);
  return 0xffffffff;
}

// This defines the builtin default write handler,
// so Bochs does not segfault if unmapped is not loaded
void bx_devices_c::default_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  UNUSED(this_ptr);
}

void bx_devices_c::timer_handler(void *this_ptr)
{
  bx_devices_c *class_ptr = (bx_devices_c *) this_ptr;
  class_ptr->timer();
}

void bx_devices_c::timer()
{
  SIM->periodic();
  if (!bx_pc_system.kill_bochs_request)
    bx_gui->handle_events();
}

bx_bool bx_devices_c::register_irq(unsigned irq, const char *name)
{
  if (irq >= BX_MAX_IRQS) {
    BX_PANIC(("IO device %s registered with IRQ=%d above %u",
             name, irq, (unsigned) BX_MAX_IRQS-1));
    return 0;
  }
  if (irq_handler_name[irq]) {
    BX_PANIC(("IRQ %u conflict, %s with %s", irq, irq_handler_name[irq], name));
    return 0;
  }
  irq_handler_name[irq] = new char[strlen(name)+1];
  strcpy(irq_handler_name[irq], name);
  return 1;
}

bx_bool bx_devices_c::unregister_irq(unsigned irq, const char *name)
{
  if (irq >= BX_MAX_IRQS) {
    BX_PANIC(("IO device %s tried to unregister IRQ %d above %u",
             name, irq, (unsigned) BX_MAX_IRQS-1));
    return 0;
  }
  if (!irq_handler_name[irq]) {
    BX_INFO(("IO device %s tried to unregister IRQ %d, not registered",
	      name, irq));
    return 0;
  }

  if (strcmp(irq_handler_name[irq], name)) {
    BX_INFO(("IRQ %u not registered to %s but to %s", irq,
      name, irq_handler_name[irq]));
    return 0;
  }
  delete [] irq_handler_name[irq];
  irq_handler_name[irq] = NULL;
  return 1;
}

bx_bool bx_devices_c::register_io_read_handler(void *this_ptr, bx_read_handler_t f,
                                               Bit32u addr, const char *name, Bit8u mask)
{
  addr &= 0xffff;

  if (!f)
    return 0;

  /* first check if the port already has a handlers != the default handler */
  if (read_port_to_handler[addr] &&
      read_port_to_handler[addr] != &io_read_handlers) { // the default
    BX_ERROR(("IO device address conflict(read) at IO address %Xh",
              (unsigned) addr));
    BX_ERROR(("  conflicting devices: %s & %s",
              read_port_to_handler[addr]->handler_name, name));
    return 0;
  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_read_handlers;
  struct io_handler_struct *io_read_handler = NULL;
  do {
    if (curr->funct == f &&
        curr->mask == mask &&
        curr->this_ptr == this_ptr &&
        !strcmp(curr->handler_name, name)) { // really want the same name too
      io_read_handler = curr;
      break;
    }
    curr = curr->next;
  } while (curr->next != &io_read_handlers);

  if (!io_read_handler) {
    io_read_handler = new struct io_handler_struct;
    io_read_handler->funct = (void *)f;
    io_read_handler->this_ptr = this_ptr;
    io_read_handler->handler_name = new char[strlen(name)+1];
    strcpy(io_read_handler->handler_name, name);
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
  return 1; // address mapped successfully
}

bx_bool bx_devices_c::register_io_write_handler(void *this_ptr, bx_write_handler_t f,
                                                Bit32u addr, const char *name, Bit8u mask)
{
  addr &= 0xffff;

  if (!f)
    return 0;

  /* first check if the port already has a handlers != the default handler */
  if (write_port_to_handler[addr] &&
      write_port_to_handler[addr] != &io_write_handlers) { // the default
    BX_ERROR(("IO device address conflict(write) at IO address %Xh",
              (unsigned) addr));
    BX_ERROR(("  conflicting devices: %s & %s",
              write_port_to_handler[addr]->handler_name, name));
    return 0;
  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_write_handlers;
  struct io_handler_struct *io_write_handler = NULL;
  do {
    if (curr->funct == f &&
        curr->mask == mask &&
        curr->this_ptr == this_ptr &&
        !strcmp(curr->handler_name, name)) { // really want the same name too
      io_write_handler = curr;
      break;
    }
    curr = curr->next;
  } while (curr->next != &io_write_handlers);

  if (!io_write_handler) {
    io_write_handler = new struct io_handler_struct;
    io_write_handler->funct = (void *)f;
    io_write_handler->this_ptr = this_ptr;
    io_write_handler->handler_name = new char[strlen(name)+1];
    strcpy(io_write_handler->handler_name, name);
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
  return 1; // address mapped successfully
}

bx_bool bx_devices_c::register_io_read_handler_range(void *this_ptr, bx_read_handler_t f,
                                             Bit32u begin_addr, Bit32u end_addr,
                                             const char *name, Bit8u mask)
{
  Bit32u addr;
  begin_addr &= 0xffff;
  end_addr &= 0xffff;

  if (end_addr < begin_addr) {
    BX_ERROR(("!!! end_addr < begin_addr !!!"));
    return 0;
  }

  if (!f) {
    BX_ERROR(("!!! f == NULL !!!"));
    return 0;
  }

  /* first check if the port already has a handlers != the default handler */
  for (addr = begin_addr; addr <= end_addr; addr++)
    if (read_port_to_handler[addr] &&
        read_port_to_handler[addr] != &io_read_handlers) { // the default
      BX_ERROR(("IO device address conflict(read) at IO address %Xh",
                (unsigned) addr));
      BX_ERROR(("  conflicting devices: %s & %s",
                read_port_to_handler[addr]->handler_name, name));
      return 0;
  }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_read_handlers;
  struct io_handler_struct *io_read_handler = NULL;
  do {
    if (curr->funct == f &&
        curr->mask == mask &&
        curr->this_ptr == this_ptr &&
        !strcmp(curr->handler_name, name)) {
      io_read_handler = curr;
      break;
    }
    curr = curr->next;
  } while (curr->next != &io_read_handlers);

  if (!io_read_handler) {
    io_read_handler = new struct io_handler_struct;
    io_read_handler->funct = (void *)f;
    io_read_handler->this_ptr = this_ptr;
    io_read_handler->handler_name = new char[strlen(name)+1];
    strcpy(io_read_handler->handler_name, name);
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
  return 1; // address mapped successfully
}

bx_bool bx_devices_c::register_io_write_handler_range(void *this_ptr, bx_write_handler_t f,
                                              Bit32u begin_addr, Bit32u end_addr,
                                              const char *name, Bit8u mask)
{
  Bit32u addr;
  begin_addr &= 0xffff;
  end_addr &= 0xffff;

  if (end_addr < begin_addr) {
    BX_ERROR(("!!! end_addr < begin_addr !!!"));
    return 0;
  }

  if (!f) {
    BX_ERROR(("!!! f == NULL !!!"));
    return 0;
  }

  /* first check if the port already has a handlers != the default handler */
  for (addr = begin_addr; addr <= end_addr; addr++)
    if (write_port_to_handler[addr] &&
        write_port_to_handler[addr] != &io_write_handlers) { // the default
      BX_ERROR(("IO device address conflict(read) at IO address %Xh",
                (unsigned) addr));
      BX_ERROR(("  conflicting devices: %s & %s",
                write_port_to_handler[addr]->handler_name, name));
      return 0;
    }

  /* first find existing handle for function or create new one */
  struct io_handler_struct *curr = &io_write_handlers;
  struct io_handler_struct *io_write_handler = NULL;
  do {
    if (curr->funct == f &&
        curr->mask == mask &&
        curr->this_ptr == this_ptr &&
        !strcmp(curr->handler_name, name)) {
      io_write_handler = curr;
      break;
    }
    curr = curr->next;
  } while (curr->next != &io_write_handlers);

  if (!io_write_handler) {
    io_write_handler = new struct io_handler_struct;
    io_write_handler->funct = (void *)f;
    io_write_handler->this_ptr = this_ptr;
    io_write_handler->handler_name = new char[strlen(name)+1];
    strcpy(io_write_handler->handler_name, name);
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
  return 1; // address mapped successfully
}


// Registration of default handlers (mainly be the unmapped device)
bx_bool bx_devices_c::register_default_io_read_handler(void *this_ptr, bx_read_handler_t f,
                                               const char *name, Bit8u mask)
{
  io_read_handlers.funct = (void *)f;
  io_read_handlers.this_ptr = this_ptr;
  if (io_read_handlers.handler_name) {
    delete [] io_read_handlers.handler_name;
  }
  io_read_handlers.handler_name = new char[strlen(name)+1];
  strcpy(io_read_handlers.handler_name, name);
  io_read_handlers.mask = mask;

  return 1;
}

bx_bool bx_devices_c::register_default_io_write_handler(void *this_ptr, bx_write_handler_t f,
                                                const char *name, Bit8u mask)
{
  io_write_handlers.funct = (void *)f;
  io_write_handlers.this_ptr = this_ptr;
  if (io_write_handlers.handler_name) {
    delete [] io_write_handlers.handler_name;
  }
  io_write_handlers.handler_name = new char[strlen(name)+1];
  strcpy(io_write_handlers.handler_name, name);
  io_write_handlers.mask = mask;

  return 1;
}

bx_bool bx_devices_c::unregister_io_read_handler(void *this_ptr, bx_read_handler_t f,
                                         Bit32u addr, Bit8u mask)
{
  addr &= 0xffff;

  struct io_handler_struct *io_read_handler = read_port_to_handler[addr];

  //BX_INFO(("Unregistering I/O read handler at %#x", addr));

  if (!io_read_handler) {
    BX_ERROR((">>> NO IO_READ_HANDLER <<<"));
    return 0;
  }

  if (io_read_handler == &io_read_handlers) {
    BX_ERROR((">>> CANNOT UNREGISTER THE DEFAULT IO_READ_HANDLER <<<"));
    return 0; // cannot unregister the default handler
  }

  if (io_read_handler->funct != f) {
    BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER FUNC <<<"));
    return 0;
  }

  if (io_read_handler->this_ptr != this_ptr) {
    BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER THIS_PTR <<<"));
    return 0;
  }

  if (io_read_handler->mask != mask) {
    BX_ERROR((">>> NOT THE SAME IO_READ_HANDLER MASK <<<"));
    return 0;
  }

  read_port_to_handler[addr] = &io_read_handlers; // reset to default
  io_read_handler->usage_count--;

  if (!io_read_handler->usage_count) { // kill this handler entry
    io_read_handler->prev->next = io_read_handler->next;
    io_read_handler->next->prev = io_read_handler->prev;
    delete [] io_read_handler->handler_name;
    delete io_read_handler;
  }
  return 1;
}

bx_bool bx_devices_c::unregister_io_write_handler(void *this_ptr, bx_write_handler_t f,
                                          Bit32u addr, Bit8u mask)
{
  addr &= 0xffff;

  struct io_handler_struct *io_write_handler = write_port_to_handler[addr];

  if (!io_write_handler)
    return 0;

  if (io_write_handler == &io_write_handlers)
    return 0; // cannot unregister the default handler

  if (io_write_handler->funct != f)
    return 0;

  if (io_write_handler->this_ptr != this_ptr)
    return 0;

  if (io_write_handler->mask != mask)
    return 0;

  write_port_to_handler[addr] = &io_write_handlers; // reset to default
  io_write_handler->usage_count--;

  if (!io_write_handler->usage_count) { // kill this handler entry
    io_write_handler->prev->next = io_write_handler->next;
    io_write_handler->next->prev = io_write_handler->prev;
    delete [] io_write_handler->handler_name;
    delete io_write_handler;
  }
  return 1;
}

bx_bool bx_devices_c::unregister_io_read_handler_range(void *this_ptr, bx_read_handler_t f,
                                               Bit32u begin, Bit32u end, Bit8u mask)
{
  begin &= 0xffff;
  end &= 0xffff;
  Bit32u addr;
  bx_bool ret = 1;

  /*
   * the easy way this time
   */
  for (addr = begin; addr <= end; addr++)
    if (!unregister_io_read_handler(this_ptr, f, addr, mask))
      ret = 0;

  return ret;
}

bx_bool bx_devices_c::unregister_io_write_handler_range(void *this_ptr, bx_write_handler_t f,
                                                Bit32u begin, Bit32u end, Bit8u mask)
{
  begin &= 0xffff;
  end &= 0xffff;
  Bit32u addr;
  bx_bool ret = 1;

  /*
   * the easy way this time
   */
  for (addr = begin; addr <= end; addr++)
    if (!unregister_io_write_handler(this_ptr, f, addr, mask))
      ret = 0;

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
    if (addr != 0x0cf8) { // don't flood the logfile when probing PCI
      BX_ERROR(("read from port 0x%04x with len %d returns 0x%x", addr, io_len, ret));
    }
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

  BX_INSTR_OUTP(addr, io_len, value);
  BX_DBG_IO_REPORT(addr, io_len, BX_WRITE, value);

  io_write_handler = write_port_to_handler[addr];
  if (io_write_handler->mask & io_len) {
    ((bx_write_handler_t)io_write_handler->funct)(io_write_handler->this_ptr, (Bit32u)addr, value, io_len);
  } else if (addr != 0x0cf8) { // don't flood the logfile when probing PCI
    BX_ERROR(("write to port 0x%04x with len %d ignored", addr, io_len));
  }
}

bx_bool bx_devices_c::is_harddrv_enabled(void)
{
  char pname[24];

  for (int i=0; i<BX_MAX_ATA_CHANNEL; i++) {
    sprintf(pname, "ata.%d.resources.enabled", i);
    if (SIM->get_param_bool(pname)->get())
      return 1;
  }
  return 0;
}

bx_bool bx_devices_c::is_network_enabled(void)
{
  if (PLUG_device_present("e1000") ||
      PLUG_device_present("ne2k") ||
      PLUG_device_present("pcipnic")) {
    return 1;
  }
  return 0;
}

bx_bool bx_devices_c::is_sound_enabled(void)
{
  if (PLUG_device_present("es1370") ||
      PLUG_device_present("sb16") ||
      PLUG_device_present("speaker")) {
    return 1;
  }
  return 0;
}

bx_bool bx_devices_c::is_usb_enabled(void)
{
  if (PLUG_device_present("usb_ohci") ||
      PLUG_device_present("usb_uhci") ||
      PLUG_device_present("usb_xhci")) {
    return 1;
  }
  return 0;
}

// removable keyboard/mouse registration
void bx_devices_c::register_removable_keyboard(void *dev, bx_keyb_enq_t keyb_enq)
{
  if (bx_keyboard.dev == NULL) {
    bx_keyboard.dev = dev;
    bx_keyboard.enq_event = keyb_enq;
  }
}

void bx_devices_c::unregister_removable_keyboard(void *dev)
{
  if (dev == bx_keyboard.dev) {
    bx_keyboard.dev = NULL;
    bx_keyboard.enq_event = NULL;
  }
}

void bx_devices_c::register_default_mouse(void *dev, bx_mouse_enq_t mouse_enq,
                                          bx_mouse_enabled_changed_t mouse_enabled_changed)
{
  if (bx_mouse[0].dev == NULL) {
    bx_mouse[0].dev = dev;
    bx_mouse[0].enq_event = mouse_enq;
    bx_mouse[0].enabled_changed = mouse_enabled_changed;
  }
}

void bx_devices_c::register_removable_mouse(void *dev, bx_mouse_enq_t mouse_enq,
                                            bx_mouse_enabled_changed_t mouse_enabled_changed)
{
  if (bx_mouse[1].dev == NULL) {
    bx_mouse[1].dev = dev;
    bx_mouse[1].enq_event = mouse_enq;
    bx_mouse[1].enabled_changed = mouse_enabled_changed;
  }
}

void bx_devices_c::unregister_removable_mouse(void *dev)
{
  if (dev == bx_mouse[1].dev) {
    bx_mouse[1].dev = NULL;
    bx_mouse[1].enq_event = NULL;
    bx_mouse[1].enabled_changed = NULL;
  }
}

bx_bool bx_devices_c::optional_key_enq(Bit8u *scan_code)
{
  if (bx_keyboard.dev != NULL) {
    return bx_keyboard.enq_event(bx_keyboard.dev, scan_code);
  }
  return 0;
}

// common mouse device handlers
void bx_devices_c::mouse_enabled_changed(bx_bool enabled)
{
  mouse_captured = enabled;

  if ((bx_mouse[1].dev != NULL) && (bx_mouse[1].enabled_changed != NULL)) {
    bx_mouse[1].enabled_changed(bx_mouse[1].dev, enabled);
    return;
  }

  if ((bx_mouse[0].dev != NULL) && (bx_mouse[0].enabled_changed != NULL)) {
    bx_mouse[0].enabled_changed(bx_mouse[0].dev, enabled);
  }
}

void bx_devices_c::mouse_motion(int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy)
{
  // If mouse events are disabled on the GUI headerbar, don't
  // generate any mouse data
  if (!mouse_captured)
    return;

  // if a removable mouse is connected, redirect mouse data to the device
  if (bx_mouse[1].dev != NULL) {
    bx_mouse[1].enq_event(bx_mouse[1].dev, delta_x, delta_y, delta_z, button_state, absxy);
    return;
  }

  // if a mouse is connected, direct mouse data to the device
  if (bx_mouse[0].dev != NULL) {
    bx_mouse[0].enq_event(bx_mouse[0].dev, delta_x, delta_y, delta_z, button_state, absxy);
  }
}

// generic PCI support
void bx_pci_device_stub_c::init_pci_conf(Bit16u vid, Bit16u did, Bit8u rev, Bit32u classc, Bit8u headt)
{
  memset(pci_conf, 0, 256);
  pci_conf[0x00] = (Bit8u)(vid & 0xff);
  pci_conf[0x01] = (Bit8u)(vid >> 8);
  pci_conf[0x02] = (Bit8u)(did & 0xff);
  pci_conf[0x03] = (Bit8u)(did >> 8);
  pci_conf[0x08] = rev;
  pci_conf[0x09] = (Bit8u)(classc & 0xff);
  pci_conf[0x0a] = (Bit8u)((classc >> 8) & 0xff);
  pci_conf[0x0b] = (Bit8u)((classc >> 16) & 0xff);
  pci_conf[0x0e] = headt;
}

void bx_pci_device_stub_c::register_pci_state(bx_list_c *list)
{
  char name[6];

  bx_list_c *pci = new bx_list_c(list, "pci_conf");
  for (unsigned i=0; i<256; i++) {
    sprintf(name, "0x%02x", i);
    new bx_shadow_num_c(pci, name, &pci_conf[i], BASE_HEX);
  }
}

void bx_pci_device_stub_c::load_pci_rom(const char *path)
{
  struct stat stat_buf;
  int fd, ret;
  unsigned long size, max_size;

  if (*path == '\0') {
    BX_PANIC(("PCI ROM image undefined"));
    return;
  }
  // read in PCI ROM image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    BX_PANIC(("couldn't open PCI ROM image file '%s'.", path));
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    BX_PANIC(("couldn't stat PCI ROM image file '%s'.", path));
    return;
  }

  max_size = 0x20000;
  size = (unsigned long)stat_buf.st_size;
  if (size > max_size) {
    close(fd);
    BX_PANIC(("PCI ROM image too large"));
    return;
  }
  if ((size % 512) != 0) {
    close(fd);
    BX_PANIC(("PCI ROM image size must be multiple of 512 (size = %ld)", size));
    return;
  }
  while ((size - 1) < max_size) {
    max_size >>= 1;
  }
  pci_rom_size = (max_size << 1);
  pci_rom = new Bit8u[pci_rom_size];

  while (size > 0) {
    ret = read(fd, (bx_ptr_t) pci_rom, size);
    if (ret <= 0) {
      BX_PANIC(("read failed on PCI ROM image: '%s'", path));
    }
    size -= ret;
  }
  close(fd);

  BX_INFO(("loaded PCI ROM '%s' (size=%u / PCI=%uk)", path, (unsigned) stat_buf.st_size, pci_rom_size >> 10));
}

#if BX_SUPPORT_PCI
bx_bool bx_devices_c::register_pci_handlers(bx_pci_device_stub_c *dev,
                                            Bit8u *devfunc, const char *name,
                                            const char *descr)
{
  unsigned i, handle;
  int first_free_slot = -1;
  char devname[80];
  char *device;

  if (strcmp(name, "pci") && strcmp(name, "pci2isa") && strcmp(name, "pci_ide")
      && (*devfunc == 0x00)) {
    for (i = 0; i < BX_N_PCI_SLOTS; i++) {
      sprintf(devname, "pci.slot.%d", i+1);
      device = SIM->get_param_string(devname)->getptr();
      if (strlen(device) > 0) {
        if (!strcmp(name, device)) {
          *devfunc = (i + 2) << 3;
          pci.slot_used[i] = 1;
          BX_INFO(("PCI slot #%d used by plugin '%s'", i+1, name));
          break;
        }
      } else if (first_free_slot == -1) {
        first_free_slot = i;
      }
    }
    if (*devfunc == 0x00) {
      // auto-assign device to PCI slot if possible
      if (first_free_slot != -1) {
        i = (unsigned)first_free_slot;
        sprintf(devname, "pci.slot.%d", i+1);
        SIM->get_param_string(devname)->set(name);
        *devfunc = (i + 2) << 3;
        pci.slot_used[i] = 1;
        BX_INFO(("PCI slot #%d used by plugin '%s'", i+1, name));
      } else {
        BX_ERROR(("Plugin '%s' not connected to a PCI slot", name));
        return 0;
      }
    }
  }
  /* check if device/function is available */
  if (pci.handler_id[*devfunc] == BX_MAX_PCI_DEVICES) {
    if (pci.num_pci_handlers >= BX_MAX_PCI_DEVICES) {
      BX_INFO(("too many PCI devices installed."));
      BX_PANIC(("  try increasing BX_MAX_PCI_DEVICES"));
      return 0;
    }
    handle = pci.num_pci_handlers++;
    pci.pci_handler[handle].handler = dev;
    pci.handler_id[*devfunc] = handle;
    BX_INFO(("%s present at device %d, function %d", descr, *devfunc >> 3,
             *devfunc & 0x07));
    return 1; // device/function mapped successfully
  } else {
    return 0; // device/function not available, return false.
  }
}

bx_bool bx_devices_c::pci_set_base_mem(void *this_ptr, memory_handler_t f1, memory_handler_t f2,
                                       Bit32u *addr, Bit8u *pci_conf, unsigned size)
{
  Bit32u newbase;

  Bit32u oldbase = *addr;
  Bit32u mask = ~(size - 1);
  Bit8u pci_flags = pci_conf[0x00] & 0x0f;
  if ((pci_flags & 0x06) > 0) {
    BX_PANIC(("PCI base memory flag 0x%02x not supported", pci_flags));
    return 0;
  }
  pci_conf[0x00] &= (mask & 0xf0);
  pci_conf[0x01] &= (mask >> 8) & 0xff;
  pci_conf[0x02] &= (mask >> 16) & 0xff;
  pci_conf[0x03] &= (mask >> 24) & 0xff;
  ReadHostDWordFromLittleEndian(pci_conf, newbase);
  pci_conf[0x00] |= pci_flags;
  if (newbase != mask && newbase != oldbase) { // skip PCI probe
    if (oldbase > 0) {
      DEV_unregister_memory_handlers(this_ptr, oldbase, oldbase + size - 1);
    }
    if (newbase > 0) {
      DEV_register_memory_handlers(this_ptr, f1, f2, newbase, newbase + size - 1);
    }
    *addr = newbase;
    return 1;
  }
  return 0;
}

bx_bool bx_devices_c::pci_set_base_io(void *this_ptr, bx_read_handler_t f1, bx_write_handler_t f2,
                                      Bit32u *addr, Bit8u *pci_conf, unsigned size,
                                      const Bit8u *iomask, const char *name)
{
  unsigned i;
  Bit32u newbase;

  Bit32u oldbase = *addr;
  Bit16u mask = ~(size - 1);
  Bit8u pci_flags = pci_conf[0x00] & 0x03;
  pci_conf[0x00] &= (mask & 0xfc);
  pci_conf[0x01] &= (mask >> 8);
  ReadHostDWordFromLittleEndian(pci_conf, newbase);
  pci_conf[0x00] |= pci_flags;
  if (((newbase & 0xfffc) != mask) && (newbase != oldbase)) { // skip PCI probe
    if (oldbase > 0) {
      for (i=0; i<size; i++) {
        if (iomask[i] > 0) {
          DEV_unregister_ioread_handler(this_ptr, f1, oldbase + i, iomask[i]);
          DEV_unregister_iowrite_handler(this_ptr, f2, oldbase + i, iomask[i]);
        }
      }
    }
    if (newbase > 0) {
      for (i=0; i<size; i++) {
        if (iomask[i] > 0) {
          DEV_register_ioread_handler(this_ptr, f1, newbase + i, name, iomask[i]);
          DEV_register_iowrite_handler(this_ptr, f2, newbase + i, name, iomask[i]);
        }
      }
    }
    *addr = newbase;
    return 1;
  }
  return 0;
}
#endif
