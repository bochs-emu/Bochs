/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

/* Qemu compatibility defines */
#ifndef SLIRP_COMPAT_H
#define SLIRP_COMPAT_H 1

#include <assert.h>
#include <stddef.h>

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
#define lseek      _lseek
#define read       _read
#define write      _write
#endif
#define strcasecmp _stricmp
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef container_of
#ifndef _MSC_VER
#define container_of(ptr, type, member) ({                      \
        const typeof(((type *) 0)->member) *__mptr = (ptr);     \
        (type *) ((char *) __mptr - offsetof(type, member));})
#else
#define container_of(ptr, type, member) ((type *)((char *)(ptr) -offsetof(type,member)))
#endif
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/*
 * Tail queue definitions.
 */
#define _QTAILQ_HEAD(name, type, qual)                                   \
struct name {                                                           \
        qual type *tqh_first;           /* first element */             \
        qual type *qual *tqh_last;      /* addr of last next element */ \
}
#define QTAILQ_HEAD(name, type)  _QTAILQ_HEAD(name, struct type,)

#define QTAILQ_HEAD_INITIALIZER(head)                                    \
        { NULL, &(head).tqh_first }

#define _QTAILQ_ENTRY(type, qual)                                        \
struct {                                                                \
        qual type *tqe_next;            /* next element */              \
        qual type *qual *tqe_prev;      /* address of previous next element */\
}
#define QTAILQ_ENTRY(type)       _QTAILQ_ENTRY(struct type,)

/*
 * Tail queue functions.
 */
#define QTAILQ_INSERT_TAIL(head, elm, field) do {                        \
        (elm)->field.tqe_next = NULL;                                   \
        (elm)->field.tqe_prev = (head)->tqh_last;                       \
        *(head)->tqh_last = (elm);                                      \
        (head)->tqh_last = &(elm)->field.tqe_next;                      \
} while (/*CONSTCOND*/0)

#define QTAILQ_REMOVE(head, elm, field) do {                             \
        if (((elm)->field.tqe_next) != NULL)                            \
                (elm)->field.tqe_next->field.tqe_prev =                 \
                    (elm)->field.tqe_prev;                              \
        else                                                            \
                (head)->tqh_last = (elm)->field.tqe_prev;               \
        *(elm)->field.tqe_prev = (elm)->field.tqe_next;                 \
} while (/*CONSTCOND*/0)

#define QTAILQ_FOREACH(var, head, field)                                 \
        for ((var) = ((head)->tqh_first);                               \
                (var);                                                  \
                (var) = ((var)->field.tqe_next))

/*
 * Tail queue access methods.
 */
#define QTAILQ_EMPTY(head)               ((head)->tqh_first == NULL)

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
#ifndef EWOULDBLOCK
# define EWOULDBLOCK  WSAEWOULDBLOCK
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
struct iovec {
    void *iov_base;
    size_t iov_len;
};
#endif

// missing functions
void pstrcpy(char *buf, int buf_size, const char *str);
int qemu_socket(int domain, int type, int protocol);
#ifdef WIN32
#define qemu_setsockopt(sockfd, level, optname, optval, optlen) \
    setsockopt(sockfd, level, optname, (const char *)optval, optlen)
#define qemu_recv(a,b,c,d) recv(a,(char*)b,c,d)
int inet_aton(const char *cp, struct in_addr *ia);
#else
#define qemu_setsockopt(sockfd, level, optname, optval, optlen) \
    setsockopt(sockfd, level, optname, (const void *)optval, optlen)
#define qemu_recv(a,b,c,d) recv(a,b,c,d)
#endif
int socket_set_fast_reuse(int fd);
int socket_set_nodelay(int fd);
void qemu_set_nonblock(int fd);

#endif
