/*  
 *  PCIDEV: PCI host device mapping
 *  Copyright (C) 2003 - Frank Cornelis
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2 as published by the Free Software Foundation.
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

/*
 * Based on pcivga code:
 * Copyright (C) 2002,2003 Mike Nordell
 */

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_PCIDEV

#include "kernel_pcidev.h"

#include <sys/ioctl.h>
#include <signal.h>
#include <linux/pci.h>

#define LOG_THIS thePciDevAdapter->

bx_pcidev_c* thePciDevAdapter = 0;

  int
libpcidev_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  thePciDevAdapter = new bx_pcidev_c ();
  bx_devices.pluginPciDevAdapter = thePciDevAdapter;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePciDevAdapter, BX_PLUGIN_PCIDEV);
  return 0; // Success
}

  void
libpcidev_LTX_plugin_fini(void)
{
}


bx_pcidev_c::bx_pcidev_c(void)
{
  put("PCI2H");
  settype(PCIDEVLOG);
}

bx_pcidev_c::~bx_pcidev_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}


static void pcidev_sighandler(int param)
{
  unsigned long irq = ((bx_pcidev_c *)bx_devices.pluginPciDevAdapter)->irq;
  BX_INFO(("Interrupt received."));
  DEV_pic_lower_irq(irq);
  /*
   * We need to first lower the IRQ line or else we don't
   * get any IRQs through
   */
  DEV_pic_raise_irq(irq);
}


static bx_bool  pcidev_mem_read_handler(unsigned long addr, unsigned long len, void *data, void *param)
{
	struct region_struct *region = (struct region_struct *)param;
	bx_pcidev_c *pcidev = region->pcidev;
	int fd = pcidev->pcidev_fd;
	int ret = -1;

	if (fd == -1)
		return false; /* we failed to handle the request, so let a default handler do it for us */

	BX_INFO(("Reading I/O memory at 0x%08x", (unsigned)addr));
	struct pcidev_io_struct io;
	io.address = addr + region->host_start - region->start;
	switch(len) {
		case 1:
			ret = ioctl(fd, PCIDEV_IOCTL_READ_MEM_BYTE, &io);
			*(unsigned char *)data = io.value;
			break;
		case 2:
			ret = ioctl(fd, PCIDEV_IOCTL_READ_MEM_WORD, &io);
			*(unsigned short *)data = io.value;
			break;
		case 4:
			ret = ioctl(fd, PCIDEV_IOCTL_READ_MEM_DWORD, &io);
			*(unsigned long *)data = io.value;
			break;
		default:
			BX_ERROR(("Unsupported pcidev read mem operation"));
			break;
	}
	if (ret == -1) {
		BX_ERROR(("pcidev read mem error"));
	}
	return true; // ok, we handled the request
}


static bx_bool pcidev_mem_write_handler(unsigned long addr, unsigned long len, void *data, void *param)
{
	struct region_struct *region = (struct region_struct *)param;
	bx_pcidev_c *pcidev = region->pcidev;
	int fd = pcidev->pcidev_fd;
	int ret = -1;

	if (fd == -1)
		return false; /* we failed to handle the request, so let a default handler do it for us */

	BX_INFO(("Writing I/O memory at 0x%08x", (unsigned)addr));
	struct pcidev_io_struct io;
	io.address = addr + region->host_start - region->start;
	switch(len) {
		case 1:
			io.value = *(unsigned char *)data;
			ret = ioctl(fd, PCIDEV_IOCTL_WRITE_MEM_BYTE, &io);
			break;
		case 2:
			io.value = *(unsigned short *)data;
			ret = ioctl(fd, PCIDEV_IOCTL_WRITE_MEM_WORD, &io);
			break;
		case 4:
			io.value = *(unsigned long *)data;
			ret = ioctl(fd, PCIDEV_IOCTL_WRITE_MEM_DWORD, &io);
			break;
		default:
			BX_ERROR(("Unsupported pcidev write mem operation"));
			break;
	}
	if (ret == -1) {
		BX_ERROR(("pcidev write mem error"));
	}
	return true;
}


static const char * const pcidev_name = "Experimental PCI 2 host PCI";

  void
bx_pcidev_c::init(void)
{
  // called once when bochs initializes
  BX_PCIDEV_THIS pcidev_fd = -1;
  int fd;
  fd = open("/dev/pcidev", O_RDWR);
  if (fd == -1) {
	  switch(errno) {
		  case ENODEV:
			  BX_ERROR(("The pcidev kernel module is not loaded!"));
			  break;
		  default:
			  perror("open /dev/pcidev");
			  break;
	  }
    return;
  }
  BX_PCIDEV_THIS pcidev_fd = fd;
  struct pcidev_find_struct find;
  unsigned short vendor = bx_options.pcidev.Ovendor->get();
  unsigned short device = bx_options.pcidev.Odevice->get();
  find.deviceID = device;
  find.vendorID = vendor;
  if (ioctl(fd, PCIDEV_IOCTL_FIND, &find) == -1) {
    switch (errno) {
    case ENOENT:
	    BX_ERROR(("PCI device not found on host system."));
    	    break;
    case EBUSY:
	    BX_ERROR(("PCI device already used by another kernel module."));
	    break;
    default:
	    perror("ioctl");
	    break;
    }
    close(fd);
    BX_PCIDEV_THIS pcidev_fd = -1;
    return;
  }
  BX_INFO(("vendor: %04x; device: %04x @ host %04x:%04x.%d", vendor, device,
	(unsigned)find.bus, (unsigned)find.device, (unsigned)find.func));

  Bit8u devfunc = 0x00;
  DEV_register_pci_handlers(this,
                            pci_read_handler,
                            pci_write_handler,
                            &devfunc, BX_PLUGIN_PCIDEV,
                            pcidev_name);

  BX_PCIDEV_THIS irq = PCIDEV_IRQ; // initial irq value
  /*
   * Next function just checks against IRQ sharing... for now Bochs
   * does not allow this... should be changed... IRQ sharing for PCI
   * devices is quite common
   */
  DEV_register_irq(BX_PCIDEV_THIS irq, pcidev_name);

  for (int idx = 0; idx < PCIDEV_COUNT_RESOURCES; idx++) {
	  if (!find.resources[idx].start)
		  continue;
	  BX_INFO(("PCI resource @ %x-%x (%s)", (unsigned)find.resources[idx].start,
				  (unsigned)find.resources[idx].end,
				  (find.resources[idx].flags & PCIDEV_RESOURCE_IO ? "I/O" : "Mem")));
	  BX_PCIDEV_THIS regions[idx].start = find.resources[idx].start;
	  BX_PCIDEV_THIS regions[idx].end = find.resources[idx].end;
	  BX_PCIDEV_THIS regions[idx].host_start = find.resources[idx].start; // we start with an identical mapping
	  struct pcidev_io_struct io;
	  io.address = PCI_BASE_ADDRESS_0 + idx * 4;
	  if (ioctl(fd, PCIDEV_IOCTL_READ_CONFIG_DWORD, &io) == -1)
		  BX_ERROR(("Error reading a base address config reg."));
	  BX_PCIDEV_THIS regions[idx].config_value = io.value;
	  /*
	   * We will use &region[idx] as parameter for our I/O or memory
	   * handler. So we provide a pcidev pointer to the pcidev object
	   * in order for the handle to be able to use its pcidev object
	   */
	  BX_PCIDEV_THIS regions[idx].pcidev = this;
	  unsigned long flags = find.resources[idx].flags;
	  if (flags & PCIDEV_RESOURCE_IO) {
		  BX_INFO(("Registering I/O port handler for %x to %x", (unsigned)find.resources[idx].start,
					(unsigned)find.resources[idx].end));
		  if (!DEV_register_ioread_handler_range(&(BX_PCIDEV_THIS regions[idx]), 
				  read_handler, find.resources[idx].start, 
				  find.resources[idx].end, "pcidev", 7))
			  BX_ERROR(("Could not register I/O port read handler range %#x to %#x", 
						  (unsigned)find.resources[idx].start,
						  (unsigned)find.resources[idx].end));
		  if (!DEV_register_iowrite_handler_range(&(BX_PCIDEV_THIS regions[idx]), 
				  write_handler, find.resources[idx].start, 
				  find.resources[idx].end, "pcidev", 7))
			  BX_ERROR(("Could not register I/O port write handler range %#x to %#x", 
						  (unsigned)find.resources[idx].start,
						  (unsigned)find.resources[idx].end));
	  }
	  else if (flags & PCIDEV_RESOURCE_MEM) {
		BX_INFO(("Registering memory I/O handler for %#x to %#x",
						  (unsigned)find.resources[idx].start,
						  (unsigned)find.resources[idx].end));
		DEV_register_memory_handlers(pcidev_mem_read_handler, 
				&(BX_PCIDEV_THIS regions[idx]), 
				pcidev_mem_write_handler, 
				&(BX_PCIDEV_THIS regions[idx]), 
				find.resources[idx].start, 
				find.resources[idx].end);
	  }
  }
  
  struct sigaction sa;
  sa.sa_handler = pcidev_sighandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGUSR1, &sa, NULL);

  /*
   * The kernel pcidev will fire SIGUSR1 signals when it receives
   * interrupts from the host PCI device.
   */
  ioctl(fd, PCIDEV_IOCTL_INTERRUPT, 1);
  
  /*
   * Let the kernel fire some fake IRQ signals
   */
  //ioctl(fd, PCIDEV_IOCTL_INTERRUPT_TEST, 1);
}

  void
bx_pcidev_c::reset(unsigned type)
{
}


  // static pci configuration space read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pcidev_c::pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len)
{
#if !BX_USE_PCIDEV_SMF
  bx_pcidev_c *class_ptr = (bx_pcidev_c *) this_ptr;

  return class_ptr->pci_read(address, io_len);
}


  Bit32u
bx_pcidev_c::pci_read(Bit8u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIDEV_SMF
  int ret = -1;

  if (io_len > 4 || io_len == 0) {
    BX_DEBUG(("Experimental PCIDEV read register 0x%02x, len=%u !",
             (unsigned) address, (unsigned) io_len));
    return 0xffffffff;
  }

  int fd = BX_PCIDEV_THIS pcidev_fd;
  if (fd == -1)
    return 0xffffffff;

  struct pcidev_io_struct io;
  io.address = address;
  switch(io_len) {
  case 1:
    ret = ioctl(fd, PCIDEV_IOCTL_READ_CONFIG_BYTE, &io);
    break;
  case 2:
    ret = ioctl(fd, PCIDEV_IOCTL_READ_CONFIG_WORD, &io);
    break;
  case 4:
    ret = ioctl(fd, PCIDEV_IOCTL_READ_CONFIG_DWORD, &io);
    break;
  }
  if (ret == -1)
    BX_ERROR(("pcidev config read error"));

  // we don't use the host irq line but our own bochs irq line  
  if (address == PCI_INTERRUPT_LINE) {
    io.value = (io.value & 0xffffff00) | (BX_PCIDEV_THIS irq & 0xff);
  }
  if (PCI_BASE_ADDRESS_0 <= address && address <= PCI_BASE_ADDRESS_5) {
	BX_INFO(("Reading pcidev base address %d.", 
				(address - PCI_BASE_ADDRESS_0) / 4));
	if (address & 3)
		BX_ERROR(("base address not aligned!"));
	io.value = BX_PCIDEV_THIS regions[(address - PCI_BASE_ADDRESS_0) >> 2].config_value;
  }

  return io.value;
}


  // static pci configuration space write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pcidev_c::pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIDEV_SMF
  bx_pcidev_c *class_ptr = (bx_pcidev_c *) this_ptr;

  class_ptr->pci_write(address, value, io_len);
}

  void
bx_pcidev_c::pci_write(Bit8u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCIDEV_SMF
  int ret = -1;

  if (io_len > 4 || io_len == 0) {
    BX_DEBUG(("Experimental PCIDEV write register 0x%02x, len=%u !",
             (unsigned) address, (unsigned) io_len));
    return;
  }

  int fd = BX_PCIDEV_THIS pcidev_fd;
  if (fd == -1)
    return;

  // we do a host 2 guest irq line mapping
  if (address == PCI_INTERRUPT_LINE) {
    value &= 0xff;
    BX_INFO(("Changing the pcidev irq line from %d to %d", 
	BX_PCIDEV_THIS irq, value));
    if (BX_PCIDEV_THIS irq)
      DEV_unregister_irq(BX_PCIDEV_THIS irq, pcidev_name);
    BX_PCIDEV_THIS irq = value;
    if (BX_PCIDEV_THIS irq) // win98 likes to set irq = 0 at power off
      DEV_register_irq(BX_PCIDEV_THIS irq, pcidev_name);
    return;
  }
  if (PCI_BASE_ADDRESS_0 <= address && address <= PCI_BASE_ADDRESS_5) {
	/*
	 * Two things to do here:
	 * - update the cached config space value via a probe
	 * - remap the I/O or memory handler if required
	 */
	BX_INFO(("Changing pcidev base address %d. New value: %#x", 
				(address - PCI_BASE_ADDRESS_0) / 4, value));
	if (address & 3) {
		BX_ERROR(("base address not aligned!"));
		return;
	}
	int io_reg_idx = (address - PCI_BASE_ADDRESS_0) >> 2;
	struct pcidev_io_struct io;
	io.address = address;
	io.value = value;
	ret = ioctl(fd, PCIDEV_IOCTL_PROBE_CONFIG_DWORD, &io);
	if (ret == -1) {
		BX_ERROR(("Error probing a base address reg!"));
		return;
	}
	unsigned long base = io.value;
	BX_PCIDEV_THIS regions[io_reg_idx].config_value = base;
	/* remap the I/O or memory handler if required using io.value
	 * We assume that an I/O memory region will stay and I/O memory
	 * region. And that an I/O port region also will stay an I/O port 
	 * region.
	 */
	if ((base & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY) {
		BX_INFO(("Remapping memory region from %#x to %#x", 
					(unsigned)BX_PCIDEV_THIS regions[io_reg_idx].start, 
					(unsigned)(base & PCI_BASE_ADDRESS_MEM_MASK)));
		if (BX_PCIDEV_THIS regions[io_reg_idx].start &&  // dirty hack
				!DEV_unregister_memory_handlers(pcidev_mem_read_handler, 
					pcidev_mem_write_handler, 
					BX_PCIDEV_THIS regions[io_reg_idx].start, 
					BX_PCIDEV_THIS regions[io_reg_idx].end))
			BX_ERROR(("Error while unregistering old memory handlers!"));
		BX_PCIDEV_THIS regions[io_reg_idx].end = 
			BX_PCIDEV_THIS regions[io_reg_idx].end + 
			(base & PCI_BASE_ADDRESS_MEM_MASK) - 
			BX_PCIDEV_THIS regions[io_reg_idx].start;
		BX_PCIDEV_THIS regions[io_reg_idx].start = base & PCI_BASE_ADDRESS_MEM_MASK;
		if (BX_PCIDEV_THIS regions[io_reg_idx].start) // dirty hack
			DEV_register_memory_handlers(pcidev_mem_read_handler, 
					&(BX_PCIDEV_THIS regions[io_reg_idx]), 
					pcidev_mem_write_handler, 
					&(BX_PCIDEV_THIS regions[io_reg_idx]), 
					BX_PCIDEV_THIS regions[io_reg_idx].start, 
					BX_PCIDEV_THIS regions[io_reg_idx].end);
	}
	else {
		/*
		 * Remap our I/O port handlers here.
		 */
		BX_INFO(("Remapping I/O port region from %x to %x", 
					(unsigned)BX_PCIDEV_THIS regions[io_reg_idx].start, 
					(unsigned)(base & PCI_BASE_ADDRESS_IO_MASK)));
		if (BX_PCIDEV_THIS regions[io_reg_idx].start) { // dirty hack
			if (!DEV_unregister_ioread_handler_range(&(BX_PCIDEV_THIS regions[io_reg_idx]), 
						read_handler, 
						BX_PCIDEV_THIS regions[io_reg_idx].start,
						BX_PCIDEV_THIS regions[io_reg_idx].end,
						7))
				BX_ERROR(("Error while unregistering old I/O port read handlers!"));
			if (!DEV_unregister_iowrite_handler_range(&(BX_PCIDEV_THIS regions[io_reg_idx]), 
						write_handler, 
						BX_PCIDEV_THIS regions[io_reg_idx].start,
						BX_PCIDEV_THIS regions[io_reg_idx].end,
						7))
				BX_ERROR(("Error while unregistering old I/O port write handlers!"));
		}
		BX_PCIDEV_THIS regions[io_reg_idx].end = 
			BX_PCIDEV_THIS regions[io_reg_idx].end + 
			(base & PCI_BASE_ADDRESS_IO_MASK) - 
			BX_PCIDEV_THIS regions[io_reg_idx].start;
		BX_PCIDEV_THIS regions[io_reg_idx].start = base & PCI_BASE_ADDRESS_IO_MASK;
		if (BX_PCIDEV_THIS regions[io_reg_idx].start) { // dirty hack
			DEV_register_ioread_handler_range(
					&(BX_PCIDEV_THIS regions[io_reg_idx]), 
					read_handler, 
					BX_PCIDEV_THIS regions[io_reg_idx].start, 
					BX_PCIDEV_THIS regions[io_reg_idx].end, 
					"pcidev", 7);
			DEV_register_iowrite_handler_range(
					&(BX_PCIDEV_THIS regions[io_reg_idx]), 
					write_handler, 
					BX_PCIDEV_THIS regions[io_reg_idx].start, 
					BX_PCIDEV_THIS regions[io_reg_idx].end,
					"pcidev", 7);
		}
	}
	return;
  }

  struct pcidev_io_struct io;
  io.address = address;
  io.value = value;

  switch(io_len) {
  case 1:
    ret = ioctl(fd, PCIDEV_IOCTL_WRITE_CONFIG_BYTE, &io);
    break;
  case 2:
    ret = ioctl(fd, PCIDEV_IOCTL_WRITE_CONFIG_WORD, &io);
    break;
  case 4:
    ret = ioctl(fd, PCIDEV_IOCTL_WRITE_CONFIG_DWORD, &io);
    break;
  }
  if (ret == -1)
    BX_ERROR(("pcidev config write error"));
}


  Bit32u
bx_pcidev_c::read_handler(void *param, Bit32u address, unsigned io_len)
{
#if !BX_USE_PCIDEV_SMF
  bx_pcidev_c *class_ptr = ((struct region_struct *)param)->pcidev;

  return class_ptr->read(param, address, io_len);
}

  Bit32u
bx_pcidev_c::read(void *param, Bit32u address, unsigned io_len)
{
#endif  // !BX_USE_PCIDEV_SMF
  struct region_struct *region = (struct region_struct *)param;
  int ret = -1;
  
  int fd = BX_PCIDEV_THIS pcidev_fd;
  if (fd == -1)
    return 0xffffffff;

  struct pcidev_io_struct io;
  // here we map the io address
  io.address = address + region->host_start - region->start;
  switch(io_len) {
	case 1:
  		ret = ioctl(fd, PCIDEV_IOCTL_READ_IO_BYTE, &io);
		break;
	case 2:
  		ret = ioctl(fd, PCIDEV_IOCTL_READ_IO_WORD, &io);
		break;
	case 4:
  		ret = ioctl(fd, PCIDEV_IOCTL_READ_IO_DWORD, &io);
		break;
  }
  if (ret == -1) {
	BX_ERROR(("pcidev read I/O error"));
  	io.value = 0xffffffff;
  }
  
  return io.value;
}

  void
bx_pcidev_c::write_handler(void *param, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIDEV_SMF
  bx_pcidev_c *class_ptr = ((struct region_struct *)param)->pcidev;

  class_ptr->write(param, address, value, io_len);
}

  void
bx_pcidev_c::write(void *param, Bit32u address, Bit32u value, unsigned io_len)
{
#else
  //UNUSED(this_ptr);
#endif  // !BX_USE_PCIDEV_SMF
  struct region_struct *region = (struct region_struct *)param;
  int ret = -1;

  int fd = BX_PCIDEV_THIS pcidev_fd;
  if (fd == -1)
    return;

  struct pcidev_io_struct io;
  // here we map the I/O address
  io.address = address + region->host_start - region->start;
  io.value = value;

  switch(io_len) {
  case 1:
    ret = ioctl(fd, PCIDEV_IOCTL_WRITE_IO_BYTE, &io);
    break;
  case 2:
    ret = ioctl(fd, PCIDEV_IOCTL_WRITE_IO_WORD, &io);
    break;
  case 4:
    ret = ioctl(fd, PCIDEV_IOCTL_WRITE_IO_DWORD, &io);
    break;
  }
  if (ret == -1)
    BX_ERROR(("pcidev I/O write error"));
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIDEV
