#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#ifndef __ASSEMBLY__

#define u_char bx_u_char
#define u_short bx_u_short
#define u_int bx_u_int
#define u_long bx_u_long
#define unchar bx_unchar
#define ushort bx_ushort
#define uint bx_uint
#define ulong bx_ulong

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
