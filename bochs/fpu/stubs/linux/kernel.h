#ifndef _LINUX_KERNEL_H
#define _LINUX_KERNEL_H

int printk(const char * fmt, ...)
        GCC_ATTRIBUTE((format (printf, 1, 2)));

#endif
