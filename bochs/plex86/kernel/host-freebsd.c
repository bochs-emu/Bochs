/*
 * plex86: run multiple x86 operating systems concurrently
 *
 * Copyright (C) 2000 Frank van der Linden (fvdl@wasabisystems.com)
 * Copyright (C) 2000 Alexander Langer <alex@big.endian.de>
 *
 * License as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#define DIAGNOSTIC 1
#define CDEV_MAJOR	20
#define timer_t __bsd_timer_t
#define write_eflags __freebsd_write_eflags
#define read_eflags __freebsd_read_eflags

/* XXX recheck, which includes are needed */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>

#include <sys/proc.h>
#include <sys/conf.h>
#include <sys/exec.h>
#include <sys/malloc.h>
#include <sys/syslog.h>
#include <sys/queue.h>
#include <sys/signalvar.h>
#include <sys/mman.h>
#include <sys/kernel.h>
#include <sys/linker.h>
#include <sys/sysproto.h>

#include <sys/module.h>

#include <vm/vm.h>

#include <machine/cpu.h>

#undef timer_t
#undef write_eflags
#undef read_eflags

#include "plex86.h"
#define IN_HOST_SPACE
#include "monitor.h"

static          MALLOC_DEFINE(M_PLEX86, "plex86", "Plex86 mem");

static d_open_t plex86_open;
static d_close_t plex86_close;
static d_mmap_t plex86_mmap;
static d_ioctl_t plex86_ioctl;

static unsigned retrieve_phy_pages(Bit32u *, int, void *, unsigned, int);
static vm_t    *find_vm(struct proc * p);
static void     register_vm(vm_t * vm, struct proc * p);
static void     unregister_all(struct proc * p);

static struct cdevsw plex86_cdevsw = {
    /* open */	plex86_open,
    /* close */	plex86_close,
    /* read */	noread,
    /* write */	nowrite,
    /* ioctl */	plex86_ioctl,
    /* poll */	nopoll,
    /* mmap */	plex86_mmap,
    /* strat */	nostrategy,
    /* name */	"plex86",
    /* major */	CDEV_MAJOR,
    /* dump */	nodump,
    /* psize */	nopsize,
    /* flags */	0,
    /* bmaj */	-1
};

/* For use with make_dev/destroy_dev */
static dev_t    plex86_dev;

static struct plex86_softc {
    int             sc_open;
}               plex86sc;

monitor_pages_t monitor_pages;

/*
 * Hash table stuff to maintain proc <-> vm mapping. 23 entries should be
 * plenty.. unless someone plans to run more than 23 guest OSs..
 *
 * Note that a process can only open the device once with this scheme.
 */

LIST_HEAD(plex86_hashhead, plex86_vmentry);

struct plex86_vmentry {
    pid_t           vm_pid;
    vm_t           *vm_vm;
    LIST_ENTRY(plex86_vmentry) vm_entry;
};

struct plex86_hashhead *plex86_hashtbl;
u_long          plex86_hashmask;

#define PLEX86_VMHASHSIZE	23
#define PLEX86_VMHASH(p)	((u_long)((p)->p_pid) & plex86_hashmask)

static int
plex86_open(dev_t dev, int flags, int fmt, struct proc * p)
{
    vm_t           *vm;

    if (suser_xxx(p->p_ucred, p, p->p_acflag) != 0)
	return (EPERM);

    vm = find_vm(p);
    if (vm == NULL) {
	vm = malloc(sizeof(vm_t), M_PLEX86, M_WAITOK);
	if (vm == NULL)
	    return EIO;
	memset(vm, 0, sizeof(vm_t));
	register_vm(vm, p);
	plex86sc.sc_open++;
    } else
	return (EBUSY);

  /* Kernel independent device open code. */
  hostDeviceOpenInit(vm);

#ifdef FREEBSD_PLEX86_DEBUG
    printf("plex86: pid %u opened device, vm %p\n", p->p_pid, vm);
#endif

    return (0);
}

int
plex86_close(dev_t dev, int flags, int fmt, struct proc * p)
{
    unregister_all(p);
    plex86sc.sc_open = 0;
#ifdef FREEBSD_PLEX86_DEBUG
    printf("plex86: pid %u closed device\n", p->p_pid);
#endif
    return (0);
}

int
plex86_mmap(dev_t dev, vm_offset_t offset, int nprot)
{
    struct proc    *p = curproc;
    int             page;
    vm_offset_t     endguestoff;
    vm_t           *vm;

    vm = find_vm(p);
    if (vm == NULL)
	return (ENXIO);

#warning "kludge to mmap message buffer"
    endguestoff = (vm_offset_t) (vm->pages.guest_n_megs * 1024 * 1024);
    if (offset >= endguestoff && nprot == PROT_READ) {
	page = (offset - endguestoff) / PAGE_SIZE;
	return (vm->pages.log_buffer[page]);
    }
    page = offset / PAGE_SIZE;
    if (page < 0 || page > vm->pages.guest_n_pages) {
	log(LOG_WARNING, "plex86: mmap: offset %lx out of range\n",
	    (unsigned long) offset);
	return -1;
    }
    return vm->pages.guest[page];
}

int
plex86_ioctl(dev_t dev, u_long cmd, caddr_t data, int flags,
	     struct proc * p)
{
    int             error;
    vm_t           *vm;

    vm = find_vm(p);
    if (vm == NULL)
	return EINVAL;

    switch (cmd) {
    case PLEX86_ALLOCVPHYS:
	{
	    unsigned        arg = *((unsigned *) data);
	    guest_cpu_t     guest_cpu;

	    if (vm->mon_state != MON_STATE_UNINITIALIZED ||
		vm->pages.guest_n_megs != 0)
		return EBUSY;
	    if (arg > PLEX86_MAX_PHY_MEGS || arg < 4 || (arg & ~0x3) != arg)
		return EINVAL;

	    /* Allocate memory */
	    error = allocVmPages(vm, arg);
	    if (error != 0) {
		log(LOG_WARNING, "plex86: allocVmPages failed (%d)\n",
		    error);
		return ENOMEM;
	    }
	    if (init_guest_phy_mem(vm) != 0) {
		log(LOG_ERR, "plex86: init_guest_phy_mem failed\n");
		unallocVmPages(vm);
		return EFAULT;
	    }
	    getCpuResetValues(&guest_cpu);
	    log(LOG_WARNING, "plex86: cpu.cr0 = 0x%x\n", guest_cpu.cr0);
	    if (!init_monitor(vm, 0, 0, &guest_cpu) ||
		!setGuestCPU(vm, 0, &guest_cpu) ||
		!mapMonitor(vm, guest_cpu.eflags, 0)) {
		log(LOG_ERR, "plex86: init_monitor failed\n");
		unallocVmPages(vm);
		return EFAULT;
	    }
	    break;
	}

    case PLEX86_TEARDOWN:
	unallocVmPages(vm);
	break;

    case PLEX86_ALLOCINT:
	return EINVAL;
    case PLEX86_RELEASEINT:
	return EINVAL;
    case PLEX86_PRESCANDEPTH:
	{
	    unsigned long   arg = *(unsigned long *) data;

	    if ((arg < PrescanDepthMin) || (arg > PrescanDepthMax)) {
		log(LOG_WARNING, "plex86: Requested prescan depth %lu"
		    " out of range [%u..%u]\n", arg, PrescanDepthMin,
		    PrescanDepthMax);
		return EINVAL;
	    }
	    vm->prescanDepth = (unsigned) arg;
	    break;
	}
    case PLEX86_SETINTR:
	ioctlSetIntr(vm, *(unsigned long *) data);
	break;
    case PLEX86_SET_A20:
	{
	    unsigned long   arg = *(unsigned long *) data;
	    if (!ioctlSetA20E(vm, arg))
		return EINVAL;
	    break;
	}
    case PLEX86_MESSAGEQ:
	{
	    vm_messages_t   msg;

	    if (vm->mon_state != MON_STATE_RUNNABLE)
		return EINVAL;

	    error = copyin(*(void **) data, &msg.header, sizeof msg.header);
	    if (error != 0)
		return error;

	    if ((msg.header.msg_len + sizeof(msg.header)) > sizeof(msg))
		return EINVAL;

	    if (msg.header.msg_len != 0) {
		error = copyin(&((vm_messages_t *) * (void **) data)->msg,
			       &msg.msg, msg.header.msg_len);
		if (error != 0)
		    return error;
	    }
	    if (ioctlMessageQ(vm, &msg)) {
		log(LOG_WARNING, "plex86: ioctlMessageQ failed\n");
		return EINVAL;
	    }
	    error = copyout(&msg, *(void **) data,
			    sizeof(msg.header) + msg.header.msg_len);
	    return error;
	}
    case PLEX86_RESET:
	break;

    case PLEX86_PHYMEM_MOD:
	break;
    case PLEX86_FORCE_INT:
	if (vm->mon_state != MON_STATE_RUNNABLE)
	    return -EINVAL;
	vm->dbg_force_int = 0x100 | (unsigned) data;
	break;
    case PLEX86_PRESCANRING3:
	{
	    unsigned long   arg = *(unsigned long *) data;
	    if (arg > PrescanRing3On) {
		log(LOG_WARNING,
		    "plex86: Requested PrescanRing3 val(%lu) OOB\n",
		    arg);
		return EINVAL;
	    }
	    vm->prescanRing3 = arg;
	    break;
	}

    default:
	log(LOG_WARNING, "plex86: unknown ioctl %lx\n", cmd);
	return EINVAL;
    }
    return 0;
}

static void
register_vm(vm_t * vm, struct proc * p)
{
    struct plex86_hashhead *php;
    struct plex86_vmentry *vhp;

    php = &plex86_hashtbl[PLEX86_VMHASH(p)];
#if DIAGNOSTIC
    for (vhp = php->lh_first; vhp != NULL; vhp = vhp->vm_entry.le_next) {
	if (vhp->vm_pid == p->p_pid)
	    panic("plex86: vm already registered, pid %u\n",
		  p->p_pid);
    }
#endif
    vhp = malloc(sizeof(struct plex86_vmentry), M_PLEX86, M_WAITOK);
    vhp->vm_pid = p->p_pid;
    vhp->vm_vm = vm;
    LIST_INSERT_HEAD(php, vhp, vm_entry);
}

static void
unregister_vm(vm_t * vm, struct proc * p)
{
    struct plex86_hashhead *php;
    struct plex86_vmentry *vhp;

    php = &plex86_hashtbl[PLEX86_VMHASH(p)];
    for (vhp = php->lh_first; vhp != NULL; vhp = vhp->vm_entry.le_next) {
	if (vhp->vm_pid == p->p_pid) {
	    LIST_REMOVE(vhp, vm_entry);
	    free(vhp->vm_vm, M_PLEX86);
	    free(vhp, M_PLEX86);
	    break;
	}
    }
}

static void
unregister_all(struct proc * p)
{
    int             i;
    struct plex86_hashhead *php;
    struct plex86_vmentry *vhp;

    php = &plex86_hashtbl[PLEX86_VMHASH(p)];
    if (php == NULL)
	return;
    for (vhp = php->lh_first; vhp != NULL;
	 vhp = vhp->vm_entry.le_next) {
#ifdef FREEBSD_PLEX86_DEBUG
	printf("plex86: unregister vm %p, pid %u\n",
	       vhp->vm_vm, vhp->vm_pid);
#endif
	LIST_REMOVE(vhp, vm_entry);
	if (vhp->vm_vm != NULL)
	    free(vhp->vm_vm, M_PLEX86);
	if (vhp != NULL)
	    free(vhp, M_PLEX86);
    }
}

static vm_t    *
find_vm(struct proc * p)
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
retrieve_phy_pages(Bit32u * page, int max_pages, void *addr_v, unsigned size,
		   int aligned)
{
    Bit32u          start_addr;
    unsigned        n_pages, i;

    if (!aligned)
	start_addr = (Bit32u) addr_v & ~(PAGE_SIZE - 1);
    else {
	start_addr = (Bit32u) addr_v;
	if (start_addr & (PAGE_SIZE - 1)) {
	    log(LOG_WARNING, "plex86: retrieve_phy_pages: address "
		"%p not aligned\n", addr_v);
	    return 0;
	}
    }

    n_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    if (n_pages > max_pages) {
	log(LOG_WARNING, "plex86: retrieve_phy_pages: page list "
	    "too small\n");
	return (0);
    }
    for (i = 0; i < n_pages; i++) {
	page[i] = kvtop((vm_offset_t) start_addr) / PAGE_SIZE;
	start_addr += PAGE_SIZE;
    }

    return n_pages;
}

unsigned
host_idle(void)
{
#if defined(want_resched)
    if (want_resched) {
#endif
	yield(curproc, NULL); /* XXX */
	need_resched(); /* XXX */
#if defined(want_resched)
    }
#endif
    printf("resched done\n");
    return (CURSIG(curproc) == 0);
}

void           *
host_alloc(unsigned long size)
{
    /*
     * XXX - it wants this page-aligned apparently.
	 */
    if (size <= (PAGE_SIZE / 2))
	size = PAGE_SIZE;
    return (malloc(size, M_PLEX86, M_WAITOK));
}

void
host_free(void *ptr)
{
    free(ptr, M_PLEX86);
}

unsigned
host_map(Bit32u * page, int max_pages, void *ptr, unsigned size)
{
    return retrieve_phy_pages(page, max_pages, ptr, size, 1);
}

void           *
host_alloc_page(void)
{
    return malloc(PAGE_SIZE, M_PLEX86, M_WAITOK);
}

void
host_free_page(void *ptr)
{
    return free(ptr, M_PLEX86);
}

Bit32u
host_map_page(void *ptr)
{
    Bit32u          u;

    if (ptr == NULL)
	return 0;
    u = kvtop(ptr) / PAGE_SIZE;
#if FREEBSD_PLEX86_DEBUG
    printf("host_map_page(%p) -> %x\n", ptr, u);
#endif
    return u;
}

void
hostprint(char *fmt,...)
{
    va_list         args;
    int             ret;
    unsigned char   buffer[256];

    va_start(args, fmt);
    ret = vsnprintf(buffer, 256, fmt, args);
    if (ret == -1)
	log(LOG_WARNING,
	    "plex86: hostprint: vsnprintf returns error.\n");
    else
	log(LOG_WARNING, "plex86: %s\n", buffer);
}

static int
plex86_modevent(module_t mod, int type, void *data)
{
    linker_file_t   lf;
    int             error = 0;

    switch (type) {
    case MOD_LOAD:
	plex86_hashtbl = NULL;

	lf = linker_find_file_by_name("plex86");
	if (lf == NULL) {
	    printf("plex86: can't find linker_file 'plex86'\n");
	    return (ENXIO);
	}
	monitor_pages.startOffset = lf->address;
  monitor_pages.startOffsetPageAligned =
      monitor_pages.startOffset & 0xfffff000;

	if ((monitor_pages.n_pages = retrieve_phy_pages(monitor_pages.page,
							PLEX86_MAX_MONITOR_PAGES,
							lf->address,
							lf->size,
							0)) == 0) {
	    log(LOG_WARNING, "plex86: could not store physical "
		"addresses for monitor pages\n");
	    return (ENXIO);
	}
	plex86_hashtbl = hashinit(PLEX86_VMHASHSIZE,
				  M_PLEX86, &plex86_hashmask);

	if (!hostModuleInit()) {
	    log(LOG_WARNING, "hostModuleInit error\n");
	    error = EINVAL;
	}

	plex86_dev = make_dev(&plex86_cdevsw, 0 /* minor */ , UID_ROOT,
			      GID_WHEEL, 0600, "plex86");

	printf("plex86: Module loaded.\n");
	return (0);
	break;
    case MOD_UNLOAD:
	if (plex86sc.sc_open != 0)
	    return (EBUSY);

	destroy_dev(plex86_dev);

	if (plex86_hashtbl != NULL) {
	    free(plex86_hashtbl, M_PLEX86);
	}
	printf("plex86: Module unloaded.\n");
	break;

    default:
	error = ENXIO;
	break;
    }
    return (error);
}

MODULE_VERSION(plex86, 1);
DEV_MODULE(plex86, plex86_modevent, 0);
