#include <Drivers.h>
#include <KernelExport.h>
#include <OS.h>
#include <SupportDefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plex86.h"
#include "monitor.h"


#define read_flags() ({ \
        unsigned int __dummy; \
        __asm__( \
                                "pushfl\n\t" \
                                "popl   %0\n\t" \
                :"=r" (__dummy)); \
        __dummy; \
})
#define write_flags(x) \
        __asm__("push %0\n\tpopfl\n\t": :"r" (x))



struct cookie {
	sem_id		sem;

	int			mon_ok;

	uint32		irq_cnt[17];
};

static status_t
driver_open(const char *name, ulong flags, void **_cookie)
{
	struct cookie *cookie;
	status_t err;

	cookie = calloc(sizeof(*cookie), 1);
	if (!cookie)
		return ENOMEM;
	cookie->sem = create_sem(1, "plex86 mutex");
	if (cookie->sem < 0) {
		err = cookie->sem;
		goto err1;
	}
	*_cookie = cookie;
	return B_OK;

err1:
	free(cookie);
	return err;
}

static status_t
driver_close(void *cookie)
{
	return B_OK;
}

static status_t
driver_free(void *_cookie)
{
	struct cookie *cookie = (struct cookie *)_cookie;
	delete_sem(cookie->sem);
	free(cookie);
	return B_OK;
}

static status_t
driver_read(void *cookie, off_t pos, void *buf, size_t *count)
{
	return B_OK;
}


static status_t
driver_write(void *cookie, off_t pos, const void *buf, size_t *count)
{
	return B_OK;
}

static status_t
driver_ioctl(void *_cookie, ulong cmd, void *buf, size_t len)
{
	struct cookie *cookie = (struct cookie *)_cookie;
	uint32		cr0, arg, eflags_orig;
	uchar		soft_int_vector;
	status_t	err;

	arg = *(uint32 *)buf;

	switch (cmd) {
		/* Allocate unpaged memory for the VM. */
		/* arg is the number of megabytes to allocate */
		/* Memory returned must not be pageable by the */
		/* host OS, since the VM monitor will run in this */
		/* memory as well.  Perhaps later, we can let */
		/* the guest OS run in paged memory and reflect */
		/* the page faults back to the host OS. */
		case 0x6b02:
			acquire_sem(cookie->sem);
			init_monitor(MASTER_PIC_BASE_VECTOR, SLAVE_PIC_BASE_VECTOR, IRQ16_BASE_VECTOR);
			cookie->mon_ok = 1;
			release_sem(cookie->sem);
			return B_OK;
		
		case 0x6b03:
			/* linux-specific hack, unnecessary under BeOS */
			return B_OK;
		
		/* run guest context for a time slice */
		case 0x6b04:
		{
			cpu_status ps;

			acquire_sem(cookie->sem);

			if (!cookie->mon_ok) {
				release_sem(cookie->sem);
				return EPERM;
			}

			ps = disable_interrupts();

			/* clear NT/IF/TF */
			eflags_orig = read_flags();
			write_flags(eflags_orig & ~0x00004300);

			__host2guest();

			write_flags(eflags_orig & ~0x00000200);

			restore_interrupts(ps);

			switch ( monitor_info.ret_because ) {
				case RET_BECAUSE_IRQ:
					/* reported vector is actually the IRQ# */

					soft_int_vector = MASTER_PIC_BASE_VECTOR + monitor_info.vector;
					soft_int(soft_int_vector);
					cookie->irq_cnt[monitor_info.vector]++;
					dprintf("plex86: irq %u\n", monitor_info.vector);
					err = B_OK;
					break;
			
				case RET_BECAUSE_INT:
					dprintf("plex86: int %u\n", monitor_info.vector);
					err = EFAULT;
					break;

				case RET_BECAUSE_EXC:
					dprintf("plex86: exc %u\n", monitor_info.vector);
					err = EFAULT;
					break;

				case RET_BECAUSE_TEST:
					dprintf("plex86: test\n");
					err = B_OK;
					break;

				default:
					dprintf("plex86: unknown ret_because\n");
					err = B_OK;
					break;
			}

			release_sem(cookie->sem);
			return err;
		}
		
		case 0x6b05: /* tear down VM environment */
			acquire_sem(cookie->sem);
			cookie->mon_ok = 0;
			release_sem(cookie->sem);
			return B_OK;
	}

	return ENOSYS;
}

device_hooks driver_device = {
	driver_open,
	driver_close,
	driver_free,
	driver_ioctl,
	driver_read,
	driver_write
};

status_t
init_driver (void)
{
	return B_OK;
}

void
uninit_driver(void)
{
}

const char **
publish_devices(void)
{
	static const char *driver_names[] = {
		"misc/plex86",
		NULL
	};
	return (const char **)driver_names;
}

device_hooks *
find_device(const char *name)
{
	return &driver_device;
}

