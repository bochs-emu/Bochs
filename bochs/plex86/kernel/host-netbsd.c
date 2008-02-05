/*
 *  plex86: run multiple x86 operating systems concurrently
 *
 *  Copyright (C) 2000 Frank van der Linden (fvdl@wasabisystems.com)
 *
 *  host-netbsd.c: NetBSD-specific code for kernel module.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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


/* XXXX stuff that conflicts with NetBSD namespace */

#define timer_t __bsd_timer_t
#define write_eflags __netbsd_write_eflags
#define read_eflags __netbsd_read_eflags

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/conf.h>
#include <sys/exec.h>
#include <sys/lkm.h>
#include <sys/malloc.h>
#include <sys/null.h>
#include <sys/syslog.h>
#include <sys/queue.h>
#include <sys/signalvar.h>
#include <sys/mman.h>

#undef NETBSD_PLEX86_DEBUG

#if __NetBSD_Version__ > 105009900
#include <uvm/uvm_extern.h>
#include <uvm/uvm_param.h>
#else
#include <vm/vm.h>
#endif

#undef timer_t
#undef write_eflags
#undef read_eflags

#include "plex86.h"
#define IN_HOST_SPACE
#include "monitor.h"

int plex86_open(dev_t dev, int oflags, int devtype, struct proc *p);
int plex86_close(dev_t dev, int cflags, int devtype, struct proc *p);
paddr_t plex86_mmap(dev_t dev, off_t offset, int length);
int plex86_ioctl(dev_t dev, u_long cmd, caddr_t data, int flags,
		 struct proc *p);

static int plex86_handle(struct lkm_table *, int);
static vm_t *find_vm(struct proc *);
static void register_vm(vm_t *, struct proc *);
static void unregister_all(void);
#if 0
static void unregister_vm(vm_t *, struct proc *);
#endif

static unsigned retrieve_phy_pages(Bit32u *, int, void *, unsigned, int);

static struct cdevsw plex86dev = {
	plex86_open, plex86_close,
	(dev_type_read((*))) enodev, (dev_type_write((*))) enodev,
	plex86_ioctl, (dev_type_stop((*))) enodev, 0,
	seltrue, plex86_mmap, 0
};

static struct plex86_softc {
	int sc_open;
} plex86sc;

MOD_DEV("plex86", LM_DT_CHAR, -1, &plex86dev)

monitor_pages_t monitor_pages;

/*
 * Hash table stuff to maintain proc <-> vm mapping.
 * 23 entries should be plenty.. unless someone plans to run more than
 * 23 guest OSs..
 *
 * Note that a process can only open the device once with this scheme.
 */

LIST_HEAD(plex86_hashhead, plex86_vmentry);

struct plex86_vmentry {
	pid_t	vm_pid;
	vm_t	*vm_vm;
	LIST_ENTRY(plex86_vmentry) vm_entry;
};

struct plex86_hashhead *plex86_hashtbl;
u_long plex86_hashmask;

#define PLEX86_VMHASHSIZE	23
#define PLEX86_VMHASH(p)	((u_long)((p)->p_pid) & plex86_hashmask)

int
plex86_lkmentry(struct lkm_table *lkmtp, int cmd, int ver)
{
	DISPATCH(lkmtp, cmd, ver, plex86_handle, plex86_handle, plex86_handle)
}

static int
plex86_handle(struct lkm_table *lkmtp, int cmd)
{
	int error = 0;

	switch (cmd) {
	case LKM_E_LOAD:
		if (lkmexists(lkmtp))
			return EEXIST;
		monitor_pages.startOffset = lkmtp->area;
    monitor_pages.startOffsetPageAligned =
        monitor_pages.startOffset & 0xfffff000;
		monitor_pages.n_pages = lkmtp->size / PAGE_SIZE;
		if (retrieve_phy_pages(monitor_pages.page,
		    PLEX86_MAX_MONITOR_PAGES, (void *)lkmtp->area,
		    lkmtp->size, 0) == 0) {
			log(LOG_WARNING, "plex86: could not store physical "
			    "addresses for monitor pages\n");
			return EIO;
		}
#if __NetBSD_Version__ > 105009900
		plex86_hashtbl = hashinit(PLEX86_VMHASHSIZE, HASH_LIST,
		    M_DEVBUF, M_WAITOK, &plex86_hashmask);
#else
		plex86_hashtbl = hashinit(PLEX86_VMHASHSIZE, M_DEVBUF,
		    M_WAITOK, &plex86_hashmask);
#endif
		if (!hostModuleInit()) {
			log(LOG_WARNING, "hostModuleInit error\n");
			error = EINVAL;
		}

		break;

	case LKM_E_UNLOAD:
		if (plex86sc.sc_open != 0)
			return EBUSY;
		free(plex86_hashtbl, M_DEVBUF);
		break;

	case LKM_E_STAT:
		break;

	default:
		error = EIO;
		break;
	}
	return error;
}

int
plex86_open(dev_t dev, int oflags, int devtype, struct proc *p)
{
	vm_t *vm;

	if (suser(p->p_ucred, &p->p_acflag) != 0)
		return EPERM;

	vm = find_vm(p);
	if (vm == NULL) {
		vm = malloc(sizeof (vm_t), M_DEVBUF, M_WAITOK);
		if (vm == NULL)
			return EIO;
                memset(vm, 0, sizeof(vm_t));
		register_vm(vm, p);
		plex86sc.sc_open++;
	} else
		return EBUSY;

  /* Kernel independent device open code. */
  hostDeviceOpenInit(vm);

#ifdef NETBSD_PLEX86_DEBUG
	printf("plex86: pid %u opened device, vm %p\n", p->p_pid, vm);
#endif
	return 0;
}

int
plex86_close(dev_t dev, int cflags, int devtype, struct proc *p)
{
	unregister_all();
	plex86sc.sc_open = 0;
#ifdef NETBSD_PLEX86_DEBUG
	printf("plex86: pid %u closed device\n", p->p_pid);
#endif
	return 0;
}

paddr_t
plex86_mmap(dev_t dev, off_t offset, int prot)
{
	struct proc *p = curproc;
	vm_t *vm;
	int page;
	off_t endguestoff;

	vm = find_vm(p);
	if (vm == NULL)
		return ENXIO;
#if 1
#warning "kludge to mmap message buffer"
	endguestoff = (off_t)(vm->pages.guest_n_megs * 1024 * 1024);
	if (offset >= endguestoff && prot == PROT_READ) {
		page = (offset - endguestoff) / PAGE_SIZE;
		return vm->pages.log_buffer[page];
	}
#endif

	page = offset / PAGE_SIZE;
	if (page < 0 || page > vm->pages.guest_n_pages) {
		log(LOG_WARNING, "plex86: mmap: offset %lx out of range\n",
		    (unsigned long)offset);
		return -1;
	}

	return vm->pages.guest[page];
}

int
plex86_ioctl(dev_t dev, u_long cmd, caddr_t data, int flags,
	     struct proc *p)
{
	int error;
	vm_t *vm;

	vm = find_vm(p);
	if (vm == NULL)
		return EINVAL;

	switch (cmd) {
	case PLEX86_ALLOCVPHYS:
	{
		unsigned long arg = *((unsigned long*)data);
		guest_cpu_t guest_cpu;

		if (vm->mon_state != MON_STATE_UNINITIALIZED ||
		    vm->pages.guest_n_megs != 0)
			return EBUSY;
		printf("plex86_ioctl: ALLOCVPHYS: requested size %lu\n",
		    arg);
		if (arg > PLEX86_MAX_PHY_MEGS || arg < 4 || (arg & ~0x3) != arg)
			return EINVAL;

		/* Allocate memory */
		error = allocVMPages(vm, arg);
		if (error != 0) {
			log(LOG_WARNING, "plex86: allocVMPages failed (%d)\n",
			    error);
			return ENOMEM;
		}

		if (init_guest_phy_mem(vm) != 0) {
			log(LOG_ERR, "plex86: init_guest_phy_mem failed\n");
			unallocVMPages(vm);
			return EFAULT;
		}

		getCpuResetValues(&guest_cpu);
		log(LOG_WARNING, "plex86: cpu.cr0 = 0x%x\n", guest_cpu.cr0);
		if (!init_monitor(vm, 0, 0, &guest_cpu) ||
		    !setGuestCPU(vm, 0, &guest_cpu) ||
		    !mapMonitor(vm, guest_cpu.eflags, 0)) {
			log(LOG_ERR, "plex86: init_monitor failed\n");
			unallocVMPages(vm);
			return EFAULT;
		}
		break;
	}

	case PLEX86_TEARDOWN:
		unallocVMPages(vm);
		break;

	case PLEX86_ALLOCINT:
		return EINVAL;
	case PLEX86_RELEASEINT:
		return EINVAL;
	case PLEX86_PRESCANDEPTH:
	{
		unsigned long arg = *(unsigned long *)data;

		if ((arg < PrescanDepthMin) || (arg > PrescanDepthMax)) {
			log(LOG_WARNING, "plex86: Requested prescan depth %lu"
			    " out of range [%u..%u]\n", arg, PrescanDepthMin,
			    PrescanDepthMax);
			return EINVAL;
		}
		vm->prescanDepth = (unsigned)arg;
		break;
	}
	case PLEX86_SETINTR:
		ioctlSetIntr(vm, *(unsigned long *)data);
		break;
	case PLEX86_SET_A20:
	{
		unsigned long arg = *(unsigned long *)data;
		if (!ioctlSetA20E(vm, arg))
			return EINVAL;
		break;
	}
	case PLEX86_MESSAGEQ:
	{
		vm_messages_t msg;

		if (vm->mon_state != MON_STATE_RUNNABLE)
			return EINVAL;

		error = copyin(*(void **)data, &msg.header, sizeof msg.header);
		if (error != 0)
			return error;

		if ((msg.header.msg_len + sizeof(msg.header)) > sizeof(msg))
			return EINVAL;

		if (msg.header.msg_len != 0) {
			error = copyin(&((vm_messages_t *)*(void **)data)->msg,
			    &msg.msg, msg.header.msg_len);
			if (error != 0)
				return error;
		}

#warning "deal with LDT %gs and %fs that the NetBSD kernel uses"
		/* XXXX */
		__asm("movl $0, %eax");
		__asm("movl %eax, %gs");
		__asm("movl %eax, %fs");

		if (ioctlMessageQ(vm, &msg)) {
			log(LOG_WARNING, "plex86: ioctlMessageQ failed\n");
			return EINVAL;
		}

		error = copyout(&msg, *(void **)data,
		    sizeof (msg.header) + msg.header.msg_len);
		return error;
	}
	case PLEX86_RESET:
		break;
	case PLEX86_PHYMEM_MOD:
		break;
	case PLEX86_FORCE_INT:
		if (vm->mon_state != MON_STATE_RUNNABLE)
			return -EINVAL;
		vm->dbg_force_int = 0x100 | (unsigned)data;
		break;
	case PLEX86_PRESCANRING3:
	{
		unsigned long arg = *(unsigned long *)data;
		if (arg > PrescanRing3On) {
			log(LOG_WARNING,
			    "plex86: Requested PrescanRing3 val(%lu) OOB\n",
			    arg);
			return EINVAL;
		}
		vm->prescanRing3 = arg;
		break;
	}
	case PLEX86_GENERIC:
		return 0;

	default:
		log(LOG_WARNING, "plex86: unknown ioctl %lx\n", cmd);
		return EINVAL;
	}
	return 0;
}

static void
register_vm(vm_t *vm, struct proc *p)
{
	struct plex86_hashhead *php;
	struct plex86_vmentry *vhp;

	php = &plex86_hashtbl[PLEX86_VMHASH(p)];
#ifdef DIAGNOSTIC
	for (vhp = php->lh_first; vhp != NULL; vhp = vhp->vm_entry.le_next) {
		if (vhp->vm_pid == p_pid)
			panic("plex86: vm already registered, pid %u\n",
			    p->pid);
	}
#endif
	vhp = malloc(sizeof (struct plex86_vmentry), M_DEVBUF, M_WAITOK);
	vhp->vm_pid = p->p_pid;
	vhp->vm_vm = vm;
	LIST_INSERT_HEAD(php, vhp, vm_entry);
}

#if 0
static void
unregister_vm(vm_t *vm, struct proc *p)
{
	struct plex86_hashhead *php;
	struct plex86_vmentry *vhp;

	php = &plex86_hashtbl[PLEX86_VMHASH(p)];
	for (vhp = php->lh_first; vhp != NULL; vhp = vhp->vm_entry.le_next) {
		if (vhp->vm_pid == p->p_pid) {
			LIST_REMOVE(vhp, vm_entry);
			free(vhp->vm_vm, M_DEVBUF);
			free(vhp, M_DEVBUF);
			break;
		}
	}
}
#endif

static void
unregister_all(void)
{
	int i;
	struct plex86_hashhead *php;
	struct plex86_vmentry *vhp;

	for (i = 0; i < PLEX86_VMHASHSIZE; i++) {
		php = &plex86_hashtbl[i];
		for (vhp = php->lh_first; vhp != NULL;
		    vhp = vhp->vm_entry.le_next) {
#ifdef NETBSD_PLEX86_DEBUG
			printf("plex86: unregister vm %p, pid %u\n",
			    vhp->vm_vm, vhp->vm_pid);
#endif
			LIST_REMOVE(vhp, vm_entry);
			free(vhp->vm_vm, M_DEVBUF);
			free(vhp, M_DEVBUF);
		}
	}
}

static vm_t *
find_vm(struct proc *p)
{
	struct plex86_hashhead *php;
	struct plex86_vmentry *vhp;

	php = &plex86_hashtbl[PLEX86_VMHASH(p)];
	for (vhp = php->lh_first; vhp != NULL; vhp = vhp->vm_entry.le_next) {
		if (vhp->vm_pid == p->p_pid)
			return vhp->vm_vm;
	}

	return NULL;
}

static unsigned
retrieve_phy_pages(Bit32u *page, int max_pages, void *addr_v, unsigned size,
		   int aligned)
{
	Bit32u start_addr;
	unsigned n_pages, i;

	if (!aligned)
		start_addr = (Bit32u)addr_v & ~(PAGE_SIZE-1);
	else {
		start_addr = (Bit32u)addr_v;
		if (start_addr & (PAGE_SIZE -1)) {
			log(LOG_WARNING, "plex86: retrieve_phy_pages: address "
			    "%p not aligned\n", addr_v);
			return 0;
		}
	}

	n_pages = (size + PAGE_SIZE - 1)  / PAGE_SIZE;
	if (n_pages > max_pages) {
		log(LOG_WARNING, "plex86: retrieve_phy_pages: page list "
		    "too small\n");
		return 0;
	}

	for (i = 0; i < n_pages; i++) {
		page[i] = vtophys((vaddr_t)start_addr) / PAGE_SIZE;
		start_addr += PAGE_SIZE;
	}

	return n_pages;
}

unsigned
host_idle(void)
{
	if (want_resched)
		yield();
	return (CURSIG(curproc) == 0);
}

void *
host_alloc(unsigned long size)
{
	/*
	 * XXX - it wants this page-aligned apparently.
	 */
	if (size <= (PAGE_SIZE / 2))
		size = PAGE_SIZE;
	return malloc(size, M_DEVBUF, M_WAITOK);
}

void
host_free(void *ptr)
{
	free(ptr, M_DEVBUF);
}

unsigned
host_map(Bit32u *page, int max_pages, void *ptr, unsigned size)
{
	return retrieve_phy_pages(page, max_pages, ptr, size, 1);
}

void *
host_alloc_page(void)
{
	return malloc(PAGE_SIZE, M_DEVBUF, M_WAITOK);
}

void
host_free_page(void *ptr)
{
	return free(ptr, M_DEVBUF);
}

Bit32u
host_map_page(void *ptr)
{
	Bit32u u;

	if (ptr == NULL)
		return 0;
	u = vtophys(ptr) / PAGE_SIZE;
	/* printf("host_map_page(%p) -> %x\n", ptr, u); */
	return u;
}

void
hostprint(char *fmt, ...)
{
	va_list args;
	int ret;
	unsigned char buffer[256];

	va_start(args, fmt);
	ret = mon_vsnprintf(buffer, 256, fmt, args);
	if (ret == -1)
		log(LOG_WARNING,
		    "plex86: hostprint: vsnprintf returns error.\n");
	else
		log(LOG_WARNING, "plex86: %s\n", buffer);
}
