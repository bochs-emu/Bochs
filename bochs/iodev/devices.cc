/////////////////////////////////////////////////////////////////////////
// $Id: devices.cc,v 1.27 2002-08-01 12:19:01 vruppert Exp $
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


#include "bochs.h"
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
  unsigned i;

#if BX_PCI_SUPPORT
  pci = NULL;
#endif
  pit = NULL;
  keyboard = NULL;
  dma = NULL;
  floppy = NULL;
  biosdev = NULL;
  cmos = NULL;
  serial = NULL;
  parallel = NULL;
  unmapped = NULL;
  vga = NULL;
  pic = NULL;
  hard_drive = NULL;
  sb16 = NULL;
  ne2k = NULL;
  g2h = NULL;
#if BX_IODEBUG_SUPPORT
  iodebug = NULL;
#endif

  num_read_handles = 0;
  num_write_handles = 0;

  /* set unused elements to appropriate values */
  for (i=0; i < BX_MAX_IO_DEVICES; i++) {
    io_read_handler[i].funct  = NULL;
    io_write_handler[i].funct = NULL;
    }

  for (i=0; i < 0x10000; i++) {
    read_handler_id[i] = BX_MAX_IO_DEVICES;  // not assigned
    write_handler_id[i] = BX_MAX_IO_DEVICES;  // not assigned
    }

  for (i=0; i < BX_MAX_IRQS; i++) {
    irq_handler_name[i] = NULL;
    }

  timer_handle = BX_NULL_TIMER_HANDLE;
}


bx_devices_c::~bx_devices_c(void)
{
  // nothing needed for now
  BX_DEBUG(("Exit."));
}


  void
bx_devices_c::init(BX_MEM_C *newmem)
{
  BX_DEBUG(("Init $Id: devices.cc,v 1.27 2002-08-01 12:19:01 vruppert Exp $"));
  mem = newmem;
  // Start with all IO port address registered to unmapped handler
  // MUST be called first
  unmapped = &bx_unmapped;
  unmapped->init(this);

#if BX_PCI_SUPPORT
  // PCI logic (i440FX)
  pci = & bx_pci;
  pci->init(this);
#endif

#if BX_SUPPORT_APIC
  // I/O APIC 82093AA
  ioapic = & bx_ioapic;
  ioapic->init ();
  ioapic->set_id (BX_IOAPIC_DEFAULT_ID);
#endif

  // BIOS log 
  biosdev = &bx_biosdev;
  biosdev->init(this);

  // CMOS RAM & RTC
  cmos = &bx_cmos;
  cmos->init(this);
  cmos->reset();

  /*--- 8237 DMA ---*/
  dma = &bx_dma;
  dma->init(this);

  /*--- HARD DRIVE ---*/
  hard_drive = &bx_hard_drive;
  hard_drive->init(this, cmos);

  //--- FLOPPY ---
  floppy = &bx_floppy;
  floppy->init(this, cmos);

#if BX_SUPPORT_SB16
  //--- SOUND ---
  sb16 = &bx_sb16;
  sb16->init(this);
#endif

#if BX_SUPPORT_VGA
  /*--- VGA adapter ---*/
  vga = & bx_vga;
  vga->init(this, cmos);
#else
  /*--- HGA adapter ---*/
  bx_init_hga_hardware(cmos);
#endif

  /*--- 8259A PIC ---*/
  pic = & bx_pic;
  pic->init(this);

  /*--- 8254 PIT ---*/
  pit = & bx_pit;
  pit->init(this);

#if BX_USE_SLOWDOWN_TIMER
  bx_slowdown_timer.init();
#endif

  keyboard = &bx_keyboard;
  keyboard->init(this, cmos);

#if BX_IODEBUG_SUPPORT
  iodebug = &bx_iodebug;
  iodebug->init(this);
#endif

  /*--- PARALLEL PORT ---*/
  parallel = &bx_parallel;
  parallel->init(this);

  /*--- SERIAL PORT ---*/
  serial = &bx_serial;
  serial->init(this);

#if BX_NE2K_SUPPORT
  // NE2000 NIC
  ne2k = &bx_ne2k;
  ne2k->init(this);
  BX_DEBUG(("ne2k"));
#endif  // #if BX_NE2K_SUPPORT

#if 0
  // Guest to Host interface.  Used with special guest drivers
  // which move data to/from the host environment.
  g2h = &bx_g2h;
  g2h->init(this);
#endif

  // system hardware
  register_io_read_handler( this,
                            &read_handler,
                            0x0092,
                            "Port 92h System Control" );
  register_io_write_handler(this,
                            &write_handler,
                            0x0092,
                            "Port 92h System Control" );

  // misc. CMOS
  Bit16u extended_memory_in_k = mem->get_memory_in_k() - 1024;
  cmos->s.reg[0x15] = (Bit8u) BASE_MEMORY_IN_K;
  cmos->s.reg[0x16] = (Bit8u) (BASE_MEMORY_IN_K >> 8);
  cmos->s.reg[0x17] = (Bit8u) extended_memory_in_k;
  cmos->s.reg[0x18] = (Bit8u) (extended_memory_in_k >> 8);
  cmos->s.reg[0x30] = (Bit8u) extended_memory_in_k;
  cmos->s.reg[0x31] = (Bit8u) (extended_memory_in_k >> 8);

  /* now perform checksum of CMOS memory */
  cmos->checksum_cmos();

  timer_handle = bx_pc_system.register_timer( this, timer_handler,
    (unsigned) BX_IODEV_HANDLER_PERIOD, 1, 1);
}


  void
bx_devices_c::reset(void)
{
#if BX_PCI_SUPPORT
  pci->reset();
#endif
  floppy->reset(BX_RESET_HARDWARE);
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
  if (io_len > 1)
    BX_PANIC(("port 92h: io read from address %08x, len=%u",
             (unsigned) address, (unsigned) io_len));

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
  Boolean bx_cpu_reset;

  if (io_len > 1)
    BX_PANIC(("port 92h: io read from address %08x, len=%u",
             (unsigned) address, (unsigned) io_len));

  BX_DEBUG(("port92h write of %02x partially supported!!!",
    (unsigned) value));
  BX_DEBUG(("A20: set_enable_a20() called"));
  BX_SET_ENABLE_A20( (value & 0x02) >> 1 );
  BX_DEBUG(("A20: now %u", (unsigned) BX_GET_ENABLE_A20()));
  bx_cpu_reset  = (value & 0x01); /* high speed reset */
  if (bx_cpu_reset) {
    BX_PANIC(("PORT 92h write: CPU reset requested!"));
    }
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
  unsigned retval;

#if (BX_USE_NEW_PIT==0)
  if ( pit->periodic( BX_IODEV_HANDLER_PERIOD ) ) {
    // This is a hack to make the IRQ0 work
    pic->lower_irq(0);
    pic->raise_irq(0);
    }
#endif

  retval = keyboard->periodic( BX_IODEV_HANDLER_PERIOD );
  if (retval & 0x01)
    pic->raise_irq(1);

  if (retval & 0x02)
    pic->raise_irq(12);

#if BX_SUPPORT_APIC
  // update local APIC timers
  for (int i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i)->local_apic.periodic (BX_IODEV_HANDLER_PERIOD);
  }
#endif
}


  Boolean
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

  Boolean
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

  Boolean
bx_devices_c::register_io_read_handler( void *this_ptr, bx_read_handler_t f,
                                        Bit32u addr, const char *name )
{
  unsigned handle;

  addr &= 0x0000ffff;

  /* first find existing handle for function or create new one */
  for (handle=0; handle < num_read_handles; handle++) {
    if (io_read_handler[handle].funct == f) break;
    }

  if (handle >= num_read_handles) {
    /* no existing handle found, create new one */
    if (num_read_handles >= BX_MAX_IO_DEVICES) {
      BX_INFO(("too many IO devices installed."));
      BX_PANIC(("  try increasing BX_MAX_IO_DEVICES"));
      }
    num_read_handles++;
    io_read_handler[handle].funct          = f;
    io_read_handler[handle].this_ptr       = this_ptr;
    io_read_handler[handle].handler_name   = name;
    }

  /* change table to reflect new handler id for that address */
  if (read_handler_id[addr] < BX_MAX_IO_DEVICES) {
    // another handler is already registered for that address
    // if it is not the Unmapped port handler, bail
    if ( strcmp( io_read_handler[read_handler_id[addr]].handler_name, "Unmapped" ) ) {
      BX_INFO(("IO device address conflict(read) at IO address %Xh",
        (unsigned) addr));
      BX_INFO(("  conflicting devices: %s & %s",
        io_read_handler[handle].handler_name, io_read_handler[read_handler_id[addr]].handler_name));
      return false; // address not available, return false.
      }
    }
  read_handler_id[addr] = handle;
  return true; // address mapped successfully
}



  Boolean
bx_devices_c::register_io_write_handler( void *this_ptr, bx_write_handler_t f,
                                        Bit32u addr, const char *name )
{
  unsigned handle;

  addr &= 0x0000ffff;

  /* first find existing handle for function or create new one */
  for (handle=0; handle < num_write_handles; handle++) {
    if (io_write_handler[handle].funct == f) break;
    }

  if (handle >= num_write_handles) {
    /* no existing handle found, create new one */
    if (num_write_handles >= BX_MAX_IO_DEVICES) {
      BX_INFO(("too many IO devices installed."));
      BX_PANIC(("  try increasing BX_MAX_IO_DEVICES"));
      }
    num_write_handles++;
    io_write_handler[handle].funct          = f;
    io_write_handler[handle].this_ptr       = this_ptr;
    io_write_handler[handle].handler_name   = name;
    }

  /* change table to reflect new handler id for that address */
  if (write_handler_id[addr] < BX_MAX_IO_DEVICES) {
    // another handler is already registered for that address
    // if it is not the Unmapped port handler, bail
    if ( strcmp( io_write_handler[write_handler_id[addr]].handler_name, "Unmapped" ) ) {
      BX_INFO(("IO device address conflict(write) at IO address %Xh",
        (unsigned) addr));
      BX_INFO(("  conflicting devices: %s & %s",
        io_write_handler[handle].handler_name, io_write_handler[write_handler_id[addr]].handler_name));
      return false; //unable to map iodevice.
      }
    }
  write_handler_id[addr] = handle;
  return true; // done!
}



/*
 * Read a byte of data from the IO memory address space
 */

  Bit32u
bx_devices_c::inp(Bit16u addr, unsigned io_len)
{
  Bit8u handle;
  Bit32u ret;

  BX_INSTR_INP(addr, io_len);

  handle = read_handler_id[addr];
  ret = (* io_read_handler[handle].funct)(io_read_handler[handle].this_ptr,
                           (Bit32u) addr, io_len);
  BX_INSTR_INP2(addr, io_len, ret);
  BX_DBG_IO_REPORT(addr, io_len, BX_READ, ret);
  return(ret);
}


/*
 * Write a byte of data to the IO memory address space.
 */

  void
bx_devices_c::outp(Bit16u addr, Bit32u value, unsigned io_len)
{
  Bit8u handle;

  BX_INSTR_OUTP(addr, io_len);
  BX_INSTR_OUTP2(addr, io_len, value);

  BX_DBG_IO_REPORT(addr, io_len, BX_WRITE, value);
  handle = write_handler_id[addr];
  (* io_write_handler[handle].funct)(io_write_handler[handle].this_ptr,
                     (Bit32u) addr, value, io_len);
}
