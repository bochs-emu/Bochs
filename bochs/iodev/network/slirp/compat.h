/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

/*
 * Qemu compatibility defines
 *
 * Copyright (c) 2003-2008  Fabrice Bellard
 * Copyright (C) 2014-2017  The Bochs Project
 */

#ifndef SLIRP_COMPAT_H
#define SLIRP_COMPAT_H 1

#include <assert.h>
#include <stddef.h>

#include "qemu-queue.h"

#if defined(_MSC_VER)
#include <io.h>
typedef Bit8s  int8_t;
typedef Bit16s int16_t;
typedef Bit8u  uint8_t;
typedef Bit16u uint16_t;
typedef Bit32u uint32_t;
typedef Bit64u uint64_t;
typedef Bit64s ssize_t;
#ifndef BX_OSDEP_H
#define snprintf   _snprintf
#define strdup     _strdup
#define open       _open
#define close      _close
#define lseek      _lseeki64
#define read       _read
#define write      _write
#endif
#define strcasecmp _stricmp
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/* Workaround for older versions of MinGW. */
#ifndef ECONNREFUSED
# define ECONNREFUSED WSAECONNREFUSED
#endif
#ifndef EINPROGRESS
# define EINPROGRESS  WSAEINPROGRESS
#endif
#ifndef EHOSTUNREACH
# define EHOSTUNREACH WSAEHOSTUNREACH
#endif
#ifndef EINTR
# define EINTR        WSAEINTR
#endif
#ifndef EINPROGRESS
# define EINPROGRESS  WSAEINPROGRESS
#endif
#ifndef ENETUNREACH
# define ENETUNREACH  WSAENETUNREACH
#endif
#ifndef ENOTCONN
# define ENOTCONN     WSAENOTCONN
#endif
#if defined(__CYGWIN__) && defined(EWOULDBLOCK)
# undef EWOULDBLOCK
# define EWOULDBLOCK  WSAEWOULDBLOCK
#endif
#ifndef EWOULDBLOCK
# define EWOULDBLOCK  WSAEWOULDBLOCK
#endif

#ifdef _WIN32
struct iovec {
    void *iov_base;
    size_t iov_len;
};
#endif

// missing functions
void pstrcpy(char *buf, int buf_size, const char *str);
int qemu_socket(int domain, int type, int protocol);
#ifndef HAVE_INET_ATON
int inet_aton(const char *cp, struct in_addr *ia);
#endif
#ifdef _WIN32
#define qemu_setsockopt(sockfd, level, optname, optval, optlen) \
    setsockopt(sockfd, level, optname, (const char *)optval, optlen)
#define qemu_recv(a,b,c,d) recv(a,(char*)b,c,d)
#else
#define qemu_setsockopt(sockfd, level, optname, optval, optlen) \
    setsockopt(sockfd, level, optname, (const void *)optval, optlen)
#define qemu_recv(a,b,c,d) recv(a,b,c,d)
#endif
int socket_set_fast_reuse(int fd);
int socket_set_nodelay(int fd);
void qemu_set_nonblock(int fd);

#endif
