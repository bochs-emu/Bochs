/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef SLIRP_H
#define SLIRP_H

#define SLIRP_VERSION_STRING "4.7.0"

#ifdef __CYGWIN__
#define __USE_W32_SOCKETS
#define _WIN32
#endif

#include "config.h"
#include "slirp_config.h"

#ifdef _WIN32

/* as defined in sdkddkver.h */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601 /* Windows 7 */
#endif
/* reduces the number of implicitly included headers */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <sys/timeb.h>
#include <iphlpapi.h>

#else
#define O_BINARY 0
#endif

#ifndef _WIN32
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#endif

#ifdef __APPLE__
#include <sys/filio.h>
#endif

#include "compat.h"

#include "debug.h"
#include "util.h"

#include "libslirp.h"
#include "ip.h"
#include "ip6.h"
#include "tcp.h"
#include "tcp_timer.h"
#include "tcp_var.h"
#include "tcpip.h"
#include "udp.h"
#include "ip_icmp.h"
#include "mbuf.h"
#include "sbuf.h"
#include "socket.h"
#include "if.h"
#include "main.h"
#include "misc.h"

#include "bootp.h"
#include "tftp.h"

#define ARPOP_REQUEST 1         /* ARP request */
#define ARPOP_REPLY   2         /* ARP reply   */

struct ethhdr {
    unsigned char  h_dest[ETH_ALEN];   /* destination eth addr */
    unsigned char  h_source[ETH_ALEN]; /* source ether addr    */
    unsigned short h_proto;            /* packet type ID field */
};

SLIRP_PACKED_BEGIN
struct slirp_arphdr {
    unsigned short ar_hrd;      /* format of hardware address */
    unsigned short ar_pro;      /* format of protocol address */
    unsigned char  ar_hln;      /* length of hardware address */
    unsigned char  ar_pln;      /* length of protocol address */
    unsigned short ar_op;       /* ARP opcode (command)       */

    /*
     *  Ethernet looks like this : This bit is variable sized however...
     */
    uint8_t  ar_sha[ETH_ALEN]; /* sender hardware address */
    uint32_t ar_sip;           /* sender IP address       */
    uint8_t  ar_tha[ETH_ALEN]; /* target hardware address */
    uint32_t ar_tip;           /* target IP address       */
} SLIRP_PACKED_END;

#define ARP_TABLE_SIZE 16

typedef struct ArpTable {
    struct slirp_arphdr table[ARP_TABLE_SIZE];
    int next_victim;
} ArpTable;

/* Add a new ARP entry for the given addresses */
void arp_table_add(Slirp *slirp, uint32_t ip_addr, 
                   const uint8_t ethaddr[ETH_ALEN]);

/* Look for an ARP entry for the given IP address */
bool arp_table_search(Slirp *slirp, uint32_t ip_addr,
                      uint8_t out_ethaddr[ETH_ALEN]);

/* Slirp configuration, specified by the application */
struct Slirp {
    int cfg_version;

    unsigned time_fasttimo;
    unsigned last_slowtimo;
    bool do_slowtimo;

    bool in_enabled, in6_enabled;

    /* virtual network configuration */
    struct in_addr vnetwork_addr;
    struct in_addr vnetwork_mask;
    struct in_addr vhost_addr;
    struct in6_addr vprefix_addr6;
    uint8_t vprefix_len;
    struct in6_addr vhost_addr6;
    bool disable_dhcp; /* slirp will not reply to any DHCP requests */
    struct in_addr vdhcp_startaddr;
    struct in_addr vnameserver_addr;
    struct in6_addr vnameserver_addr6;

    struct in_addr client_ipaddr;
    char client_hostname[33];

    int restricted;
    struct gfwd_list *guestfwd_list;

    int if_mtu;
    int if_mru;

    bool disable_host_loopback;

    /* mbuf states */
    struct mbuf m_freelist, m_usedlist;
    int mbuf_alloced;

    /* if states */
    struct mbuf if_fastq;   /* fast queue (for interactive data) */
    struct mbuf if_batchq;  /* queue for non-interactive data */
    struct mbuf *next_m;    /* pointer to next mbuf to output */
    bool if_start_busy;     /* avoid if_start recursion */

    /* ip states */
    struct ipq ipq;         /* ip reass. queue */
    uint16_t ip_id;         /* ip packet ctr, for ids */

    /* bootp/dhcp states */
    BOOTPClient bootp_clients[NB_BOOTP_CLIENTS];
    char *bootp_filename;
    size_t vdnssearch_len;
    uint8_t *vdnssearch;
    char *vdomainname;

    /* tcp states */
    struct socket tcb;
    struct socket *tcp_last_so;
    tcp_seq tcp_iss;        /* tcp initial send seq # */
    uint32_t tcp_now;       /* for RFC 1323 timestamps */

    /* udp states */
    struct socket udb;
    struct socket *udp_last_so;

    /* icmp states */
    struct socket icmp;
    struct socket *icmp_last_so;

    /* tftp states */
    char *tftp_prefix;
    struct tftp_session tftp_sessions[TFTP_SESSIONS_MAX];
    char *tftp_server_name;

    ArpTable arp_table;

    bool enable_emu;

    const SlirpCb *cb;
    void *opaque;

    bool disable_dns; /* slirp will not redirect/serve any DNS packet */
};

void if_start(Slirp *);

/* Get the address of the DNS server on the host side */
int get_dns_addr(struct in_addr *pdns_addr);

#define SO_OPTIONS DO_KEEPALIVE
#define TCP_MAXIDLE (TCPTV_KEEPCNT * TCPTV_KEEPINTVL)

/* dnssearch.c */
int translate_dnssearch(Slirp *s, const char ** names);

/* cksum.c */
int cksum(struct mbuf *m, int len);

/* if.c */
void if_init(Slirp *);
void if_output(struct socket *, struct mbuf *);

/* ip_input.c */
void ip_init(Slirp *);
void ip_cleanup(Slirp *);
void ip_input(struct mbuf *);
void ip_slowtimo(Slirp *);
void ip_stripoptions(struct mbuf *, struct mbuf *);

/* ip_output.c */
int ip_output(struct socket *, struct mbuf *);

/* tcp_input.c */
void tcp_input(struct mbuf *, int, struct socket *);
int tcp_mss(struct tcpcb *, u_int);

/* tcp_output.c */
int tcp_output(struct tcpcb *);
void tcp_setpersist(struct tcpcb *);

/* tcp_subr.c */
void tcp_init(Slirp *);
void tcp_cleanup(Slirp *);
void tcp_template(struct tcpcb *);
void tcp_respond(struct tcpcb *, struct tcpiphdr *, struct mbuf *, tcp_seq, tcp_seq, int);
struct tcpcb * tcp_newtcpcb(struct socket *);
struct tcpcb * tcp_close(struct tcpcb *);
void tcp_sockclosed(struct tcpcb *);
int tcp_fconnect(struct socket *);
void tcp_connect(struct socket *);
int tcp_attach(struct socket *);
uint8_t tcp_tos(struct socket *);
int tcp_emu(struct socket *, struct mbuf *);
int tcp_ctl(struct socket *);
struct tcpcb *tcp_drop(struct tcpcb *tp, int err);

/* Send a frame to the virtual Ethernet board, i.e. call the application send_packet callback */
void slirp_send_packet_all(Slirp *slirp, const void *buf, size_t len);

#endif
