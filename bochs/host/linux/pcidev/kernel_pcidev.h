/*
 *  PCIDEV: PCI host device mapping
 *  Copyright (C) 2003 Frank Cornelis <fcorneli@pandora.be>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef _KERNEL_PCIDEV_H
#define _KERNEL_PCIDEV_H

#define PCIDEV_MAJOR 240
#define PCIDEV_NAME "pcidev"

#define PCIDEV_COUNT_RESOURCES 6

struct pcidev_find_struct {
	unsigned long vendorID;
	unsigned long deviceID;
	unsigned long bus;
	unsigned long device;
	unsigned long func;
	struct {
		unsigned long start;
		unsigned long end;
		unsigned long flags; // see linux/ioport.h
	} resources [PCIDEV_COUNT_RESOURCES];
};

// we copied these values from linux/ioport.h since these
// are not accessible from within user space code
#define PCIDEV_RESOURCE_IO 0x100
#define PCIDEV_RESOURCE_MEM 0x200

struct pcidev_io_struct {
	unsigned long address;
	unsigned long value;
};

#define PCIDEV_IOCTL_MAGIC 'p'
#define PCIDEV_IOCTL_FIND _IOWR(PCIDEV_IOCTL_MAGIC, 0, struct pcidev_find_struct)

#define PCIDEV_IOCTL_READ_CONFIG_BYTE _IOWR(PCIDEV_IOCTL_MAGIC, 1, struct pcidev_io_struct)
#define PCIDEV_IOCTL_READ_CONFIG_WORD _IOWR(PCIDEV_IOCTL_MAGIC, 2, struct pcidev_io_struct)
#define PCIDEV_IOCTL_READ_CONFIG_DWORD _IOWR(PCIDEV_IOCTL_MAGIC, 3, struct pcidev_io_struct)

#define PCIDEV_IOCTL_WRITE_CONFIG_BYTE _IOR(PCIDEV_IOCTL_MAGIC, 4, struct pcidev_io_struct)
#define PCIDEV_IOCTL_WRITE_CONFIG_WORD _IOR(PCIDEV_IOCTL_MAGIC, 5, struct pcidev_io_struct)
#define PCIDEV_IOCTL_WRITE_CONFIG_DWORD _IOR(PCIDEV_IOCTL_MAGIC, 6, struct pcidev_io_struct)

#define PCIDEV_IOCTL_INTERRUPT _IO(PCIDEV_IOCTL_MAGIC, 7)
#define PCIDEV_IOCTL_INTERRUPT_TEST _IO(PCIDEV_IOCTL_MAGIC, 8)

#define PCIDEV_IOCTL_READ_IO_BYTE _IOWR(PCIDEV_IOCTL_MAGIC, 9, struct pcidev_io_struct)
#define PCIDEV_IOCTL_READ_IO_WORD _IOWR(PCIDEV_IOCTL_MAGIC, 10, struct pcidev_io_struct)
#define PCIDEV_IOCTL_READ_IO_DWORD _IOWR(PCIDEV_IOCTL_MAGIC, 11, struct pcidev_io_struct)

#define PCIDEV_IOCTL_WRITE_IO_BYTE _IOR(PCIDEV_IOCTL_MAGIC, 12, struct pcidev_io_struct)
#define PCIDEV_IOCTL_WRITE_IO_WORD _IOR(PCIDEV_IOCTL_MAGIC, 13, struct pcidev_io_struct)
#define PCIDEV_IOCTL_WRITE_IO_DWORD _IOR(PCIDEV_IOCTL_MAGIC, 14, struct pcidev_io_struct)

#define PCIDEV_IOCTL_READ_MEM_BYTE _IOWR(PCIDEV_IOCTL_MAGIC, 15, struct pcidev_io_struct)
#define PCIDEV_IOCTL_READ_MEM_WORD _IOWR(PCIDEV_IOCTL_MAGIC, 16, struct pcidev_io_struct)
#define PCIDEV_IOCTL_READ_MEM_DWORD _IOWR(PCIDEV_IOCTL_MAGIC, 17, struct pcidev_io_struct)

#define PCIDEV_IOCTL_WRITE_MEM_BYTE _IOR(PCIDEV_IOCTL_MAGIC, 18, struct pcidev_io_struct)
#define PCIDEV_IOCTL_WRITE_MEM_WORD _IOR(PCIDEV_IOCTL_MAGIC, 19, struct pcidev_io_struct)
#define PCIDEV_IOCTL_WRITE_MEM_DWORD _IOR(PCIDEV_IOCTL_MAGIC, 20, struct pcidev_io_struct)

#define PCIDEV_IOCTL_PROBE_CONFIG_DWORD _IOWR(PCIDEV_IOCTL_MAGIC, 21, struct pcidev_io_struct)


#ifdef __KERNEL__

struct pcidev_struct {
	struct pci_dev *dev;
	int pid; // send irq signals to this task
	struct timer_list irq_timer; // for testing the irq signals
	void *mapped_mem [PCIDEV_COUNT_RESOURCES];
};

#endif // __KERNEL__

#endif // _KERNEL_PCIDEV_H
