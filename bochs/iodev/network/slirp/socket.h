/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 1995 Danny Gasparovski.
 */

#ifndef SLIRP_SOCKET_H
#define SLIRP_SOCKET_H

#define SO_EXPIRE 240000
#define SO_EXPIREFAST 10000

/*
 * Our socket structure
 */

struct socket {
  struct socket *so_next,*so_prev;      /* For a linked list of sockets */

  int s;                           /* The actual socket */

  int pollfds_idx;

  Slirp *slirp;			   /* managing slirp instance */

			/* XXX union these with not-yet-used sbuf params */
  struct mbuf *so_m;	           /* Pointer to the original SYN packet,
				    * for non-blocking connect()'s, and
				    * PING reply's */
  struct tcpiphdr *so_ti;	   /* Pointer to the original ti within
				    * so_mconn, for non-blocking connections */
  int so_urgc;
  struct in_addr so_faddr;	   /* foreign host table entry */
  struct in_addr so_laddr;	   /* local host table entry */
  uint16_t so_fport;		   /* foreign port */
  uint16_t so_lport;		   /* local port */

  uint8_t	so_iptos;	/* Type of service */
  uint8_t	so_emu;		/* Is the socket emulated? */

  u_char	so_type;		/* Type of socket, UDP or TCP */
  int	so_state;		/* internal state flags SS_*, below */

  struct 	tcpcb *so_tcpcb;	/* pointer to TCP protocol control block */
  u_int	so_expire;		/* When the socket will expire */

  int	so_queued;		/* Number of packets queued from this socket */
  int	so_nqueued;		/* Number of packets queued in a row
				 * Used to determine when to "downgrade" a session
					 * from fastq to batchq */

  struct sbuf so_rcv;		/* Receive buffer */
  struct sbuf so_snd;		/* Send buffer */
  void * extra;			/* Extra pointer */
};


/*
 * Socket state bits. (peer means the host on the Internet,
 * local host means the host on the other end of the modem)
 */
#define SS_NOFDREF		0x001	/* No fd reference */

#define SS_ISFCONNECTING	0x002	/* Socket is connecting to peer (non-blocking connect()'s) */
#define SS_ISFCONNECTED		0x004	/* Socket is connected to peer */
#define SS_FCANTRCVMORE		0x008	/* Socket can't receive more from peer (for half-closes) */
#define SS_FCANTSENDMORE	0x010	/* Socket can't send more to peer (for half-closes) */
#define SS_FWDRAIN		0x040	/* We received a FIN, drain data and set SS_FCANTSENDMORE */

#define SS_CTL			0x080
#define SS_FACCEPTCONN		0x100	/* Socket is accepting connections from a host on the internet */
#define SS_FACCEPTONCE		0x200	/* If set, the SS_FACCEPTCONN socket will die after one accept */

#define SS_PERSISTENT_MASK	0xf000	/* Unremovable state bits */
#define SS_HOSTFWD		0x1000	/* Socket describes host->guest forwarding */
#define SS_INCOMING		0x2000	/* Connection was initiated by a host on the internet */

/* Check that two addresses are equal */
static inline int sockaddr_equal(const struct sockaddr_storage *a,
                                 const struct sockaddr_storage *b)
{
    if (a->ss_family != b->ss_family) {
        return 0;
    }

    switch (a->ss_family) {
    case AF_INET: {
        const struct sockaddr_in *a4 = (const struct sockaddr_in *)a;
        const struct sockaddr_in *b4 = (const struct sockaddr_in *)b;
        return a4->sin_addr.s_addr == b4->sin_addr.s_addr &&
               a4->sin_port == b4->sin_port;
    }
    case AF_INET6: {
        const struct sockaddr_in6 *a6 = (const struct sockaddr_in6 *)a;
        const struct sockaddr_in6 *b6 = (const struct sockaddr_in6 *)b;
        return (in6_equal(&a6->sin6_addr, &b6->sin6_addr) &&
                a6->sin6_port == b6->sin6_port);
    }
#ifndef _WIN32
    case AF_UNIX: {
        const struct sockaddr_un *aun = (const struct sockaddr_un *)a;
        const struct sockaddr_un *bun = (const struct sockaddr_un *)b;
        return strncmp(aun->sun_path, bun->sun_path, sizeof(aun->sun_path)) == 0;
    }
#endif
    default:
        fprintf(stderr, "unknown protocol\n");
    }

    return 0;
}

/* Get the size of an address */
static inline socklen_t sockaddr_size(const struct sockaddr_storage *a)
{
    switch (a->ss_family) {
    case AF_INET:
        return sizeof(struct sockaddr_in);
    case AF_INET6:
        return sizeof(struct sockaddr_in6);
#ifndef _WIN32
    case AF_UNIX:
        return sizeof(struct sockaddr_un);
#endif
    default:
        fprintf(stderr, "unknown protocol\n");
    }

    return 0;
}

struct socket * solookup(struct socket *, struct in_addr, u_int, struct in_addr, u_int);
struct socket * socreate(Slirp *);
void sofree(struct socket *);
int soread(struct socket *);
int sorecvoob(struct socket *);
int sosendoob(struct socket *);
int sowrite(struct socket *);
void sorecvfrom(struct socket *);
int sosendto(struct socket *, struct mbuf *);
struct socket * tcp_listen(Slirp *, uint32_t, u_int, uint32_t, u_int,
                               int);
void soisfconnecting(struct socket *);
void soisfconnected(struct socket *);
void sofwdrain(struct socket *);
struct iovec; /* For win32 */
size_t sopreprbuf(struct socket *so, struct iovec *iov, int *np);
int soreadbuf(struct socket *so, const char *buf, int size);

#endif /* SLIRP_SOCKET_H */
