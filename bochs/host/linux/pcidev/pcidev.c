/*
 *  PCIDEV: PCI host device mapping
 *  Copyright (C) 2003, 2004 Frank Cornelis <fcorneli@pandora.be>
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
/*
 * Create the PCIDEV using:
 * mknod /dev/pcidev c 240 0
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/version.h>

#include "kernel_pcidev.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Frank Cornelis <fcorneli@pandora.be>");
MODULE_DESCRIPTION("PCI Host Device Mapper Driver");
MODULE_SUPPORTED_DEVICE("pcidev");

//EXPORT_NO_SYMBOLS;


static char *pcidev_name = PCIDEV_NAME;

static int pcidev_open(struct inode *inode, struct file *file)
{
	struct pcidev_struct *pcidev = kmalloc(sizeof(struct pcidev_struct), GFP_KERNEL);
	int idx;
	if (!pcidev)
		goto out;
	pcidev->dev = NULL;
	pcidev->pid = 0;
	for (idx = 0; idx < PCIDEV_COUNT_RESOURCES; idx++)
		pcidev->mapped_mem[idx] = NULL;
	init_timer(&pcidev->irq_timer);
	pcidev->irq_timer.function = NULL; // no test irq signaling 
out:
	file->private_data = pcidev;
	return 0;
}


static int pcidev_release(struct inode *inode, struct file *file)
{
	struct pcidev_struct *pcidev = (struct pcidev_struct *)file->private_data;
	int idx;
	if (!pcidev)
		return 0;
	if (!pcidev->dev)
		goto out;
	if (pcidev->irq_timer.function)
		del_timer_sync(&pcidev->irq_timer);
	if (pcidev->pid) {
		u8 irq;
	     	pci_read_config_byte(pcidev->dev, PCI_INTERRUPT_PIN, &irq);
	     	pci_read_config_byte(pcidev->dev, PCI_INTERRUPT_LINE, &irq);
		free_irq(irq, (void *)pcidev->pid);
	}
	for (idx = 0; idx < PCIDEV_COUNT_RESOURCES; idx++)
		if (pcidev->mapped_mem[idx])
			iounmap(pcidev->mapped_mem[idx]);
	pci_release_regions(pcidev->dev);
out:
	kfree(file->private_data);
	return 0;
}


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0))
// linux kernel 2.4
typedef void irqreturn_t; 
#define IRQ_NONE
#endif


static irqreturn_t pcidev_irqhandler(int irq, void *dev_id, struct pt_regs *regs)
{
	int pid = (int)dev_id;
	struct task_struct *task;
	read_lock(&tasklist_lock);
	task = find_task_by_pid(pid);
	if (task)
		send_sig_info(SIGUSR1, (struct siginfo *)1, task); // XXX: should be converted to real-time signals 
	read_unlock(&tasklist_lock);
	return IRQ_NONE; 
	/* 
	 * we cannot possible say IRQ_HANDLED because we simply 
	 * don't know yet... only bochs could tell
	 */
}


static void irq_test_timer(unsigned long data)
{
	struct task_struct *task;
	struct pcidev_struct *pcidev = (struct pcidev_struct *)data;
	read_lock(&tasklist_lock);
	task = find_task_by_pid(pcidev->pid);
	if (task)
		send_sig_info(SIGUSR1, (struct siginfo *)1, task);
	read_unlock(&tasklist_lock);
	
	//printk(KERN_INFO "irq_test_timer\n");
	init_timer(&pcidev->irq_timer);
	pcidev->irq_timer.data = data;
	pcidev->irq_timer.function = irq_test_timer;
	pcidev->irq_timer.expires = jiffies + HZ;
	add_timer(&pcidev->irq_timer);
}


static int pcidev_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct pcidev_struct *pcidev = (struct pcidev_struct *)file->private_data;
	if (!pcidev)
		return -EIO;
	switch(cmd) {
	case PCIDEV_IOCTL_FIND: {
		struct pcidev_find_struct *find;
		struct pci_dev *dev;
		unsigned long vendorID, deviceID;
		int idx;
		if (pcidev->dev)
			return -EIO; // only alloc once for now
		if (!access_ok(VERIFY_WRITE, (void *)arg, sizeof(struct pcidev_find_struct)))
			return -EFAULT;
		find = (struct pcidev_find_struct *)arg;
		__get_user(vendorID, &find->vendorID);
		__get_user(deviceID, &find->deviceID);
		__put_user(-1, &find->bus);
		__put_user(-1, &find->device);
		__put_user(-1, &find->func);
		dev = pci_find_device(vendorID, deviceID, NULL);
		if (!dev)
			return -ENOENT;
		if (pci_enable_device(dev)) {
			printk(KERN_WARNING "pcidev: Could not enable the PCI device.\n");
			return -EIO;
		}
		if (pci_set_dma_mask(dev, 0xffffffff))
			printk(KERN_WARNING "pcidev: only limited PCI busmaster DMA support.\n");
		pci_set_master(dev);
		printk(KERN_INFO "pcidev: device found at %x:%x.%d\n", 
				dev->bus->number, PCI_SLOT(dev->devfn), 
				PCI_FUNC(dev->devfn));
		ret = pci_request_regions(dev, pcidev_name);
		if (ret < 0)
			break;
		for (idx = 0; idx < PCIDEV_COUNT_RESOURCES; idx++) {
			if (pci_resource_flags(dev, idx) & IORESOURCE_MEM) {
				long len = pci_resource_len(dev, idx);
				unsigned long mapped_start = (unsigned long)ioremap(pci_resource_start(dev, idx), len);
				__put_user(mapped_start, &find->resources[idx].start);
				__put_user(mapped_start + len - 1, &find->resources[idx].end);
				pcidev->mapped_mem[idx] = (void *)mapped_start;
			}
			else {
				pcidev->mapped_mem[idx] = NULL;
				__put_user(pci_resource_start(dev, idx), &find->resources[idx].start);
				__put_user(pci_resource_end(dev, idx), &find->resources[idx].end);
			}
			__put_user(pci_resource_flags(dev, idx), &find->resources[idx].flags);
		}
		pcidev->dev = dev;
		__put_user(dev->bus->number, &find->bus);
		__put_user(PCI_SLOT(dev->devfn), &find->device);
		__put_user(PCI_FUNC(dev->devfn), &find->func);
		ret = 0;
		break;
	}
	case PCIDEV_IOCTL_READ_CONFIG_BYTE: 
	case PCIDEV_IOCTL_READ_CONFIG_WORD: 
	case PCIDEV_IOCTL_READ_CONFIG_DWORD: {
		struct pcidev_io_struct *io;
		unsigned long address, value;
		if (!pcidev->dev)
			return -EIO;
		if (!access_ok(VERIFY_WRITE, (void *)arg, sizeof(struct pcidev_io_struct)))
			return -EFAULT;
		io = (struct pcidev_io_struct *)arg;
		__get_user(address, &io->address);
		__put_user(-1, &io->value);
		printk(KERN_DEBUG "pcidev: reading config address %#x\n", (int)address);
		switch(cmd) {
		case PCIDEV_IOCTL_READ_CONFIG_BYTE:
			ret = pci_read_config_byte(pcidev->dev, address, (u8 *)&value);
			break;
		case PCIDEV_IOCTL_READ_CONFIG_WORD:
			ret = pci_read_config_word(pcidev->dev, address, (u16 *)&value);
			break;
		case PCIDEV_IOCTL_READ_CONFIG_DWORD:
			ret = pci_read_config_dword(pcidev->dev, address, (u32 *)&value);
			break;
		}
		if (ret < 0)
			return ret;
		__put_user(value, &io->value);
		break;
	}
	case PCIDEV_IOCTL_WRITE_CONFIG_BYTE: 
	case PCIDEV_IOCTL_WRITE_CONFIG_WORD: 
	case PCIDEV_IOCTL_WRITE_CONFIG_DWORD: {
		struct pcidev_io_struct *io;
		unsigned long address, value;
		if (!pcidev->dev)
			return -EIO;
		if (!access_ok(VERIFY_READ, (void *)arg, sizeof(struct pcidev_io_struct)))
			return -EFAULT;
		io = (struct pcidev_io_struct *)arg;
		__get_user(address, &io->address);
		__get_user(value, &io->value);
		/*
		 * Next tests prevent the pcidev user from remapping
		 * the PCI host device since this could cause great 
		 * trouble because we don't own those I/O resources.
		 * If the pcidev wants to remap a device he needs to
		 * emulate the mapping himself and not bother the host
		 * kernel about it.
		 */
		if (address == PCI_INTERRUPT_PIN) {
			printk(KERN_WARNING "pcidev: not allowed to set irq pin!\n");
			return -EIO;
		}
		if (address == PCI_INTERRUPT_LINE) {
			printk(KERN_WARNING "pcidev: not allowed to set irq line!\n");
			return -EIO;
		}
		if (PCI_BASE_ADDRESS_0 <= address && (address & ~3UL) <= PCI_BASE_ADDRESS_5) {
			printk(KERN_WARNING "pcidev: now allowed to change base address %d\n", 
					(int)((address & ~3UL) - PCI_BASE_ADDRESS_0) / 4);
			return -EIO;
		}
		printk(KERN_DEBUG "pcidev: writing config address %#x\n", (int)address);
		switch(cmd) {
		case PCIDEV_IOCTL_WRITE_CONFIG_BYTE:
			ret = pci_write_config_byte(pcidev->dev, address, (u8)value);
			break;
		case PCIDEV_IOCTL_WRITE_CONFIG_WORD:
			ret = pci_write_config_word(pcidev->dev, address, (u16)value);
			break;
		case PCIDEV_IOCTL_WRITE_CONFIG_DWORD:
			ret = pci_write_config_dword(pcidev->dev, address, (u32)value);
			break;
		}
		break;
	}
	case PCIDEV_IOCTL_INTERRUPT: {
		u8 irq;
		if (!pcidev->dev)
			return -EIO;
		ret = pci_read_config_byte(pcidev->dev, PCI_INTERRUPT_PIN, &irq);
		if (ret < 0)
			break;
		if (!irq)
			return -EIO;
		ret = pci_read_config_byte(pcidev->dev, PCI_INTERRUPT_LINE, &irq);
		if (ret < 0)
			break;
		if (arg & 1) {
			pcidev->pid = current->pid; // our dev_id
			printk(KERN_INFO "pcidev: enabling IRQ %d\n", irq);
			ret = request_irq(irq, pcidev_irqhandler, SA_SHIRQ,
					pcidev_name, (void *)current->pid);
		}
		else {
			if (!pcidev->pid)
				return -EIO;
			printk(KERN_INFO "pcidev: disabling IRQ %d\n", irq);
			free_irq(irq, (void *)pcidev->pid);
			pcidev->pid = 0;
			ret = 0;
		}
		break;
	}
	/*
	 * Next ioctl is only for testing purposes.
	 */
	case PCIDEV_IOCTL_INTERRUPT_TEST: {
		ret = -EIO;
		if (!pcidev->dev)
			break;
		if (!pcidev->pid)
			break;
		if (pcidev->irq_timer.function)
			del_timer_sync(&pcidev->irq_timer);
		pcidev->irq_timer.function = NULL;
		if (arg & 1) {
			init_timer(&pcidev->irq_timer);
			pcidev->irq_timer.function = irq_test_timer;
			pcidev->irq_timer.data = (unsigned long)pcidev;
			pcidev->irq_timer.expires = jiffies + HZ;
			add_timer(&pcidev->irq_timer);
		}
		ret = 0;
		break;
	}
	case PCIDEV_IOCTL_READ_IO_BYTE: 
	case PCIDEV_IOCTL_READ_IO_WORD: 
	case PCIDEV_IOCTL_READ_IO_DWORD: {
		/*
		 * We should probably check access rights against
		 * the PCI resource list... but who cares for a 
		 * security hole more or less :)
		 */
		struct pcidev_io_struct *io;
		unsigned long address, value = -1;
		if (!access_ok(VERIFY_WRITE, (void *)arg, sizeof(struct pcidev_io_struct)))
			return -EFAULT;
		io = (struct pcidev_io_struct *)arg;
		__get_user(address, &io->address);
		printk(KERN_DEBUG "pcidev: reading I/O port %#x\n", (int)address);
		switch(cmd) {
		case PCIDEV_IOCTL_READ_IO_BYTE:
			value = inb(address);
			break;
		case PCIDEV_IOCTL_READ_IO_WORD:
			value = inw(address);
			break;
		case PCIDEV_IOCTL_READ_IO_DWORD:
			value = inl(address);
			break;
		}
		__put_user(value, &io->value);
		ret = 0;
		break;
	}
	case PCIDEV_IOCTL_WRITE_IO_BYTE: 
	case PCIDEV_IOCTL_WRITE_IO_WORD: 
	case PCIDEV_IOCTL_WRITE_IO_DWORD: {
		struct pcidev_io_struct *io;
		unsigned long address, value;
		if (!access_ok(VERIFY_READ, (void *)arg, sizeof(struct pcidev_io_struct)))
			return -EFAULT;
		io = (struct pcidev_io_struct *)arg;
		__get_user(address, &io->address);
		__get_user(value, &io->value);
		printk(KERN_DEBUG "pcidev: writing I/O port %#x\n", (int)address);
		switch(cmd) {
		case PCIDEV_IOCTL_WRITE_IO_BYTE:
			outb(value, address);
			break;
		case PCIDEV_IOCTL_WRITE_IO_WORD:
			outw(value, address);
			break;
		case PCIDEV_IOCTL_WRITE_IO_DWORD:
			outl(value, address);
			break;
		}
		ret = 0;
		break;
	}
	case PCIDEV_IOCTL_READ_MEM_BYTE: 
	case PCIDEV_IOCTL_READ_MEM_WORD: 
	case PCIDEV_IOCTL_READ_MEM_DWORD: {
		struct pcidev_io_struct *io;
		unsigned long address, value = -1;
		if (!access_ok(VERIFY_WRITE, (void *)arg, sizeof(struct pcidev_io_struct)))
			return -EFAULT;
		io = (struct pcidev_io_struct *)arg;
		__get_user(address, &io->address);
		printk(KERN_DEBUG "pcidev: reading memory %#x\n", (int)address);
		switch(cmd) {
		case PCIDEV_IOCTL_READ_MEM_BYTE:
			value = readb((unsigned char *)address);
			break;
		case PCIDEV_IOCTL_READ_MEM_WORD:
			value = readw((unsigned short *)address);
			break;
		case PCIDEV_IOCTL_READ_MEM_DWORD:
			value = readl((unsigned int *)address);
			break;
		}
		__put_user(value, &io->value);
		ret = 0;
		break;
	}
	case PCIDEV_IOCTL_WRITE_MEM_BYTE: 
	case PCIDEV_IOCTL_WRITE_MEM_WORD:
	case PCIDEV_IOCTL_WRITE_MEM_DWORD: {
		struct pcidev_io_struct *io;
		unsigned long address, value;
		if (!access_ok(VERIFY_READ, (void *)arg, sizeof(struct pcidev_io_struct)))
			return -EFAULT;
		io = (struct pcidev_io_struct *)arg;
		__get_user(address, &io->address);
		__get_user(value, &io->value);
		printk(KERN_DEBUG "pcidev: writing memory %#x\n", (int)address);
		switch(cmd) {
		case PCIDEV_IOCTL_WRITE_MEM_BYTE:
			writeb(value, (unsigned char *)address);
			break;
		case PCIDEV_IOCTL_WRITE_MEM_WORD:
			writew(value, (unsigned short *)address);
			break;
		case PCIDEV_IOCTL_WRITE_MEM_DWORD:
			writel(value, (unsigned int *)address);
			break;
		}
		ret = 0;
		break;
	}
	case PCIDEV_IOCTL_PROBE_CONFIG_DWORD: {
		/*
		 * This ioctl allows for probing a config space value.
		 * This can be used for base address size probing
		 */
		struct pcidev_io_struct *io;
		unsigned long address, value, orig_value;
		if (!pcidev->dev)
			return -EIO;
		if (!access_ok(VERIFY_WRITE, (void *)arg, sizeof(struct pcidev_io_struct)))
			return -EFAULT;
		io = (struct pcidev_io_struct *)arg;
		__get_user(address, &io->address);
		__get_user(value, &io->value);
		__put_user(-1, &io->value);
		printk(KERN_INFO "pcidev: probing config space address: %#x\n", (int)address);
		ret = pci_read_config_dword(pcidev->dev, address, (u32 *)&orig_value);
		if (ret < 0)
			break;
		pci_write_config_dword(pcidev->dev, address, (u32)value);
		pci_read_config_dword(pcidev->dev, address, (u32 *)&value);
		ret = pci_write_config_dword(pcidev->dev, address, (u32)orig_value);
		if (ret < 0)
			break;
		__put_user(value, &io->value);
		break;
	}
	default:
		ret = -ENOTTY;
	}
	return ret;
}


static struct file_operations pcidev_fops = {
	open: pcidev_open,
	release: pcidev_release,
	ioctl: pcidev_ioctl,
	owner: THIS_MODULE
};



int __init init_module(void)
{
	int result;
	printk(KERN_INFO "pcidev init\n");
	if ((result = register_chrdev(PCIDEV_MAJOR, pcidev_name, &pcidev_fops)) < 0)
		printk(KERN_WARNING "Could not register pcidev.\n");
	return result;
}


void __exit cleanup_module(void)
{
	if (unregister_chrdev(PCIDEV_MAJOR, pcidev_name) < 0)
		printk(KERN_WARNING "Could not unregister pcidev.\n");
	printk(KERN_INFO "pcidev cleanup\n");
}
