/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 1995 Danny Gasparovski.
 */

#include "slirp.h"
#if BX_NETWORKING && BX_NETMOD_SLIRP

#ifndef _WIN32
#include <sys/un.h>
#endif

void slirp_insque(void *a, void *b)
{
    struct slirp_quehead *element = (struct slirp_quehead *) a;
    struct slirp_quehead *head = (struct slirp_quehead *) b;
    element->qh_link = head->qh_link;
    head->qh_link = (struct slirp_quehead *)element;
    element->qh_rlink = (struct slirp_quehead *)head;
    ((struct slirp_quehead *)(element->qh_link))->qh_rlink =
        (struct slirp_quehead *)element;
}

void slirp_remque(void *a)
{
    struct slirp_quehead *element = (struct slirp_quehead *) a;
    ((struct slirp_quehead *)(element->qh_link))->qh_rlink = element->qh_rlink;
    ((struct slirp_quehead *)(element->qh_rlink))->qh_link = element->qh_link;
    element->qh_rlink = NULL;
}

/* TODO: IPv6 */
struct gfwd_list *add_guestfwd(struct gfwd_list **ex_ptr, SlirpWriteCb write_cb,
                               void *opaque, struct in_addr addr, int port)
{
    struct gfwd_list *f = (struct gfwd_list *)malloc(sizeof(struct gfwd_list));

    f->write_cb = write_cb;
    f->opaque = opaque;
    f->ex_fport = port;
    f->ex_addr = addr;
    f->ex_next = *ex_ptr;
    *ex_ptr = f;

    return f;
}

struct gfwd_list *add_exec(struct gfwd_list **ex_ptr, const char *cmdline,
                           struct in_addr addr, int port)
{
    struct gfwd_list *f = add_guestfwd(ex_ptr, NULL, NULL, addr, port);

    f->ex_exec = strdup(cmdline);

    return f;
}

struct gfwd_list *add_unix(struct gfwd_list **ex_ptr, const char *unixsock,
                           struct in_addr addr, int port)
{
    struct gfwd_list *f = add_guestfwd(ex_ptr, NULL, NULL, addr, port);

    f->ex_unix = strdup(unixsock);

    return f;
}

int remove_guestfwd(struct gfwd_list **ex_ptr, struct in_addr addr, int port)
{
    for (; *ex_ptr != NULL; ex_ptr = &((*ex_ptr)->ex_next)) {
        struct gfwd_list *f = *ex_ptr;
        if (f->ex_addr.s_addr == addr.s_addr && f->ex_fport == port) {
            *ex_ptr = f->ex_next;
            free(f->ex_exec);
            free(f);
            return 0;
        }
    }
    return -1;
}

int fork_exec(struct socket *so, const char *ex)
{
    /* not implemented */
    return 0;
}

int open_unix(struct socket *so, const char *unixpath)
{
    /* not implemented */
    return 0;
}

char *slirp_connection_info(Slirp *slirp)
{
    /* not implemented */
    return NULL;
}

char *slirp_neighbor_info(Slirp *slirp)
{
    /* not implemented */
    return NULL;
}

int slirp_bind_outbound(struct socket *so, unsigned short af)
{
    int ret = 0;
    struct sockaddr *addr = NULL;
    int addr_size = 0;

    if (af == AF_INET && so->slirp->outbound_addr != NULL) {
        addr = (struct sockaddr *)so->slirp->outbound_addr;
        addr_size = sizeof(struct sockaddr_in);
    } else if (af == AF_INET6 && so->slirp->outbound_addr6 != NULL) {
        addr = (struct sockaddr *)so->slirp->outbound_addr6;
        addr_size = sizeof(struct sockaddr_in6);
    }

    if (addr != NULL) {
        ret = bind(so->s, addr, addr_size);
    }
    return ret;
}

#endif
