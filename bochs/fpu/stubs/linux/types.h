#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#ifndef __ASSEMBLY__
/* bsd */
typedef unsigned char           u_char;
typedef unsigned short          u_short;
typedef unsigned int            u_int;
typedef unsigned long           u_long;

/* sysv */
typedef unsigned char           unchar;
typedef unsigned short          ushort;
typedef unsigned int            uint;
typedef unsigned long           ulong;

#ifndef NULL
#define NULL ((void *) 0)
#endif

#endif

#endif /* _LINUX_TYPES_H */
