/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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

#include "pcidev.h"
#include "kernel_pcidev.h"

#include <sys/ioctl.h>
#include <signal.h>
#include <linux/pci.h>

#define LOG_THIS thePciDevAdapter->

bx_pcidev_c* thePciDevAdapter = NULL;

int libpcidev_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  thePciDevAdapter = new bx_pcidev_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePciDevAdapter, BX_PLUGIN_PCIDEV);
  return 0; // Success
}

void libpcidev_LTX_plugin_fini(void)
{
  delete thePciDevAdapter;
}

bx_pcidev_c::bx_pcidev_c()
{
  put("pcidev", "PCI2H");
}

bx_pcidev_c::~bx_pcidev_c()
{
  BX_DEBUG(("Exit"));
}

static void pcidev_sighandler(int param)
{
  bx_pcidev_c *pcidev = thePciDevAdapter;
  BX_INFO(("Interrupt received."));
  DEV_pci_set_irq(pcidev->devfunc, pcidev->intpin, 0);
  /*
   * We need to first lower the IRQ line or else we don't
   * get any IRQs through
   */
  DEV_pci_set_irq(pcidev->devfunc, pcidev->intpin, 1);
}

static bx_bool pcidev_mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
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
            *(Bit8u *)data = io.value;
            break;
        case 2:
            ret = ioctl(fd, PCIDEV_IOCTL_READ_MEM_WORD, &io);
            *(Bit16u *)data = io.value;
            break;
        case 4:
            ret = ioctl(fd, PCIDEV_IOCTL_READ_MEM_DWORD, &io);
            *(Bit32u *)data = io.value;
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


static bx_bool pcidev_mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
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
            io.value = *(Bit8u *)data;
            ret = ioctl(fd, PCIDEV_IOCTL_WRITE_MEM_BYTE, &io);
            break;
        case 2:
            io.value = *(Bit16u *)data;
            ret = ioctl(fd, PCIDEV_IOCTL_WRITE_MEM_WORD, &io);
            break;
        case 4:
            io.value = *(Bit32u *)data;
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

void bx_pcidev_c::init(void)
{
  // called once when bochs initializes
  BX_PCIDEV_THIS pcidev_fd = -1;
  int fd;
  fd = open("/dev/pcidev", O_RDWR);
  if (fd == -1) {
    switch(errno) {
      case ENODEV:
        BX_PANIC(("The pcidev kernel module is not loaded!"));
        break;
      default:
        BX_PANIC(("open /dev/pcidev: %s", strerror(errno)));
        break;
    }
    return;
  }
  BX_PCIDEV_THIS pcidev_fd = fd;
  struct pcidev_find_struct find;
  unsigned short vendor = SIM->get_param_num(BXPN_PCIDEV_VENDOR)->get();
  unsigned short device = SIM->get_param_num(BXPN_PCIDEV_DEVICE)->get();
  find.deviceID = device;
  find.vendorID = vendor;
  if (ioctl(fd, PCIDEV_IOCTL_FIND, &find) == -1) {
    switch (errno) {
    case ENOENT:
      BX_PANIC(("PCI device not found on host system."));
      break;
    case EBUSY:
      BX_PANIC(("PCI device already used by another kernel module."));
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

  BX_PCIDEV_THIS devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_PCIDEV_THIS devfunc, BX_PLUGIN_PCIDEV,
                            pcidev_name);

  BX_PCIDEV_THIS irq = 0;
  struct pcidev_io_struct io;
  io.address = 0x3d;
  int ret = ioctl(fd, PCIDEV_IOCTL_READ_CONFIG_BYTE, &io);
  if (ret != -1) {
    BX_PCIDEV_THIS intpin = io.value;
  } else {
    BX_PCIDEV_THIS intpin = 0;
  }

  for (int idx = 0; idx < PCIDEV_COUNT_RESOURCES; idx++) {
    BX_PCIDEV_THIS regions[idx].start = 0; // emulated device not yet initialized
    if (!find.resources[idx].start)
      continue;
    BX_INFO(("PCI resource @ %x-%x (%s)", (unsigned)find.resources[idx].start,
             (unsigned)find.resources[idx].end,
             (find.resources[idx].flags & PCIDEV_RESOURCE_IO ? "I/O" : "Mem")));
    BX_PCIDEV_THIS regions[idx].size = find.resources[idx].end - find.resources[idx].start + 1;
    BX_PCIDEV_THIS regions[idx].host_start = find.resources[idx].start;
    struct pcidev_io_struct io;
    io.address = PCI_BASE_ADDRESS_0 + idx * 4;
    if (ioctl(fd, PCIDEV_IOCTL_READ_CONFIG_DWORD, &io) == -1)
      BX_ERROR(("Error reading a base address config reg"));
    BX_PCIDEV_THIS regions[idx].config_value = io.value;
    /*
     * We will use &region[idx] as parameter for our I/O or memory
     * handler. So we provide a pcidev pointer to the pcidev object
     * in order for the handle to be able to use its pcidev object
     */
    BX_PCIDEV_THIS regions[idx].pcidev = this;
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
}

void bx_pcidev_c::reset(unsigned type) { }

// pci configuration space read callback handler
Bit32u bx_pcidev_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  int ret = -1;

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
    BX_INFO(("Reading pcidev base address #%d", (address - PCI_BASE_ADDRESS_0) / 4));
    io.value = BX_PCIDEV_THIS regions[(address - PCI_BASE_ADDRESS_0) >> 2].config_value;
    if (address & 3) {
      io.value >>= (8 * (address & 3));
    }
  }

  return io.value;
}

// pci configuration space write callback handler
void bx_pcidev_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  int ret = -1;
  Bit8u *iomask;
  Bit32u bitmask;

  int fd = BX_PCIDEV_THIS pcidev_fd;
  if (fd == -1)
    return;

  // we do a host 2 guest irq line mapping
  if (address == PCI_INTERRUPT_LINE) {
    value &= 0xff;
    BX_INFO(("Changing the pcidev irq line from %d to %d", BX_PCIDEV_THIS irq, value));
    BX_PCIDEV_THIS irq = value;
    return;
  }
  if ((PCI_BASE_ADDRESS_0 <= address) && (address <= PCI_BASE_ADDRESS_5)) {
    /*
     * Two things to do here:
     * - update the cached config space value via a probe
     * - remap the I/O or memory handler if required
     */
    int io_reg_idx = (address - PCI_BASE_ADDRESS_0) >> 2;
    switch (io_len) {
      case 1: bitmask = 0xff; break;
      case 2: bitmask = 0xffff; break;
      default: bitmask = 0xffffffff;
    }
    bitmask <<= (8 * (address & 3));
    value <<= (8 * (address & 3));
    value |= BX_PCIDEV_THIS regions[io_reg_idx].config_value & ~bitmask;
    BX_INFO(("Changing pcidev base address #%d - New value: %#x",
      (address - PCI_BASE_ADDRESS_0) / 4, value));
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
      if (DEV_pci_set_base_mem(&(BX_PCIDEV_THIS regions[io_reg_idx]),
          pcidev_mem_read_handler,
          pcidev_mem_write_handler,
          &BX_PCIDEV_THIS regions[io_reg_idx].start,
          (Bit8u*)&BX_PCIDEV_THIS regions[io_reg_idx].config_value,
          BX_PCIDEV_THIS regions[io_reg_idx].size)) {
            BX_INFO(("new base #%d memory address: 0x%08x", io_reg_idx,
                     BX_PCIDEV_THIS regions[io_reg_idx].start));
      }
    } else {
      /*
       * Remap our I/O port handlers here.
       */
      iomask = (Bit8u*)malloc(BX_PCIDEV_THIS regions[io_reg_idx].size);
      memset(iomask, 7, BX_PCIDEV_THIS regions[io_reg_idx].size);
      if (DEV_pci_set_base_io(&(BX_PCIDEV_THIS regions[io_reg_idx]),
            read_handler, write_handler,
            &BX_PCIDEV_THIS regions[io_reg_idx].start,
            (Bit8u*)&BX_PCIDEV_THIS regions[io_reg_idx].config_value,
            BX_PCIDEV_THIS regions[io_reg_idx].size, iomask, "pcidev")) {
        BX_INFO(("new base #%d i/o address: 0x%04x", io_reg_idx,
                 (Bit16u)BX_PCIDEV_THIS regions[io_reg_idx].start));
      }
      free(iomask);
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

Bit32u bx_pcidev_c::read_handler(void *param, Bit32u address, unsigned io_len)
{
#if !BX_USE_PCIDEV_SMF
  bx_pcidev_c *class_ptr = ((struct region_struct *)param)->pcidev;
  return class_ptr->read(param, address, io_len);
}

Bit32u bx_pcidev_c::read(void *param, Bit32u address, unsigned io_len)
{
#endif
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

void bx_pcidev_c::write_handler(void *param, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCIDEV_SMF
  bx_pcidev_c *class_ptr = ((struct region_struct *)param)->pcidev;
  class_ptr->write(param, address, value, io_len);
}

void bx_pcidev_c::write(void *param, Bit32u address, Bit32u value, unsigned io_len)
{
#endif
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
