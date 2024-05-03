/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 1995 Danny Gasparovski.
 */

#ifndef MISC_H
#define MISC_H

struct gfwd_list {
    struct in_addr ex_addr;		/* Server address */
    int ex_fport;                   /* Port to telnet to */
    const char *ex_exec;            /* Command line of what to exec */
    struct gfwd_list *ex_next;
};

#define EMU_NONE 0x0

/* TCP emulations */
#define EMU_CTL 0x1
#define EMU_FTP 0x2
#define EMU_KSH 0x3
#define EMU_IRC 0x4
#define EMU_REALAUDIO 0x5
#define EMU_RLOGIN 0x6
#define EMU_IDENT 0x7
#define EMU_RSH 0x8

#define EMU_NOCONNECT 0x10	/* Don't connect */

struct tos_t {
    uint16_t lport;
    uint16_t fport;
    uint8_t tos;
    uint8_t emu;
};

struct emu_t {
    uint16_t lport;
    uint16_t fport;
    uint8_t tos;
    uint8_t emu;
    struct emu_t *next;
};

struct slirp_quehead {
    struct slirp_quehead *qh_link;
    struct slirp_quehead *qh_rlink;
};

/* Insert element a into queue b */
void slirp_insque(void *a, void *b);

/* Remove element a from its queue */
void slirp_remque(void *a);

int add_exec(struct gfwd_list **, const char *, struct in_addr, int);
int fork_exec(struct socket *so, const char *ex, int do_pty);

#endif
