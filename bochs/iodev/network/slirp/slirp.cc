/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
/*
 * libslirp glue
 *
 * Copyright (c) 2004-2008 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#define BX_PLUGGABLE

#include "iodev.h"
#include "slirp.h"

#if BX_NETWORKING && BX_NETMOD_SLIRP

#define LOG_THIS ((logfunctions*)slirp->logfn)->

/* host loopback address */
struct in_addr loopback_addr;
/* host loopback network mask */
unsigned long loopback_mask;

/* emulated hosts use the MAC addr 52:55:IP:IP:IP:IP */
static const uint8_t special_ethaddr[ETH_ALEN] = {
    0x52, 0x55, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t zero_ethaddr[ETH_ALEN] = { 0, 0, 0, 0, 0, 0 };

/* XXX: suppress those select globals */
fd_set *global_readfds, *global_writefds, *global_xfds;

u_int curtime;

static QTAILQ_HEAD(slirp_instances, Slirp) slirp_instances =
    QTAILQ_HEAD_INITIALIZER(slirp_instances);

static struct in_addr dns_addr;
static u_int dns_addr_time;

#define TIMEOUT_FAST 2  /* milliseconds */
#define TIMEOUT_SLOW 499  /* milliseconds */
/* for the aging of certain requests like DNS */
#define TIMEOUT_DEFAULT 1000  /* milliseconds */

#ifdef WIN32

#include <iphlpapi.h>

int get_dns_addr(struct in_addr *pdns_addr)
{
    FIXED_INFO *FixedInfo=NULL;
    ULONG    BufLen;
    DWORD    ret;
    IP_ADDR_STRING *pIPAddr;
    struct in_addr tmp_addr;

    if (dns_addr.s_addr != 0 && (curtime - dns_addr_time) < TIMEOUT_DEFAULT) {
        *pdns_addr = dns_addr;
        return 0;
    }

    FixedInfo = (FIXED_INFO *)GlobalAlloc(GPTR, sizeof(FIXED_INFO));
    BufLen = sizeof(FIXED_INFO);

    if (ERROR_BUFFER_OVERFLOW == GetNetworkParams(FixedInfo, &BufLen)) {
        if (FixedInfo) {
            GlobalFree(FixedInfo);
            FixedInfo = NULL;
        }
        FixedInfo = (FIXED_INFO *)GlobalAlloc(GPTR, BufLen);
    }

    if ((ret = GetNetworkParams(FixedInfo, &BufLen)) != ERROR_SUCCESS) {
        printf("GetNetworkParams failed. ret = %08x\n", (u_int)ret );
        if (FixedInfo) {
            GlobalFree(FixedInfo);
            FixedInfo = NULL;
        }
        return -1;
    }

    pIPAddr = &(FixedInfo->DnsServerList);
    inet_aton(pIPAddr->IpAddress.String, &tmp_addr);
    *pdns_addr = tmp_addr;
    dns_addr = tmp_addr;
    dns_addr_time = curtime;
    if (FixedInfo) {
        GlobalFree(FixedInfo);
        FixedInfo = NULL;
    }
    return 0;
}

static void CDECL winsock_cleanup(void)
{
#ifndef __CYGWIN__
    WSACleanup();
#endif
}

#else

static struct stat dns_addr_stat;

int get_dns_addr(struct in_addr *pdns_addr)
{
    char buff[512];
    char buff2[257];
    FILE *f;
    int found = 0;
    struct in_addr tmp_addr;

    if (dns_addr.s_addr != 0) {
        struct stat old_stat;
        if ((curtime - dns_addr_time) < TIMEOUT_DEFAULT) {
            *pdns_addr = dns_addr;
            return 0;
        }
        old_stat = dns_addr_stat;
        if (stat("/etc/resolv.conf", &dns_addr_stat) != 0)
            return -1;
        if ((dns_addr_stat.st_dev == old_stat.st_dev)
            && (dns_addr_stat.st_ino == old_stat.st_ino)
            && (dns_addr_stat.st_size == old_stat.st_size)
            && (dns_addr_stat.st_mtime == old_stat.st_mtime)) {
            *pdns_addr = dns_addr;
            return 0;
        }
    }

    f = fopen("/etc/resolv.conf", "r");
    if (!f)
        return -1;

#ifdef DEBUG
    lprint("IP address of your DNS(s): ");
#endif
    while (fgets(buff, 512, f) != NULL) {
        if (sscanf(buff, "nameserver%*[ \t]%256s", buff2) == 1) {
            if (!inet_aton(buff2, &tmp_addr))
                continue;
            /* If it's the first one, set it to dns_addr */
            if (!found) {
                *pdns_addr = tmp_addr;
                dns_addr = tmp_addr;
                dns_addr_time = curtime;
            }
#ifdef DEBUG
            else
                lprint(", ");
#endif
            if (++found > 3) {
#ifdef DEBUG
                lprint("(more)");
#endif
                break;
            }
#ifdef DEBUG
            else
                lprint("%s", inet_ntoa(tmp_addr));
#endif
        }
    }
    fclose(f);
    if (!found)
        return -1;
    return 0;
}

#endif

static void slirp_init_once(void)
{
    static int initialized;
#ifdef _WIN32
    WSADATA Data;
#endif

    if (initialized) {
        return;
    }
    initialized = 1;

#ifdef _WIN32
    WSAStartup(MAKEWORD(2,0), &Data);
    atexit(winsock_cleanup);
#endif

    loopback_addr.s_addr = htonl(INADDR_LOOPBACK);
    loopback_mask = htonl(IN_CLASSA_NET);
}

Slirp *slirp_init(int restricted, struct in_addr vnetwork,
                  struct in_addr vnetmask, struct in_addr vhost,
                  const char *vhostname, const char *tftp_path,
                  const char *bootfile, struct in_addr vdhcp_start,
                  struct in_addr vnameserver, const char **vdnssearch,
                  void *opaque, void *logfn)
{
    Slirp *slirp = (Slirp*)malloc(sizeof(Slirp));
    memset(slirp, 0, sizeof(Slirp));

    slirp_init_once();

    slirp->restricted = restricted;

    if_init(slirp);
    ip_init(slirp);

    /* Initialise mbufs *after* setting the MTU */
    m_init(slirp);

    slirp->vnetwork_addr = vnetwork;
    slirp->vnetwork_mask = vnetmask;
    slirp->vhost_addr = vhost;
    if (vhostname) {
        pstrcpy(slirp->client_hostname, sizeof(slirp->client_hostname),
                vhostname);
    }
    if (tftp_path) {
        slirp->tftp_prefix = strdup(tftp_path);
    }
    if (bootfile) {
        slirp->bootp_filename = strdup(bootfile);
    }
    slirp->vdhcp_startaddr = vdhcp_start;
    slirp->vnameserver_addr = vnameserver;

    if (vdnssearch) {
        translate_dnssearch(slirp, vdnssearch);
    }

    slirp->opaque = opaque;
    slirp->logfn = logfn;

    QTAILQ_INSERT_TAIL(&slirp_instances, slirp, entry);

    return slirp;
}

void slirp_cleanup(Slirp *slirp)
{
    QTAILQ_REMOVE(&slirp_instances, slirp, entry);

    ip_cleanup(slirp);
    m_cleanup(slirp);

    free(slirp->tftp_prefix);
    free(slirp->bootp_filename);
    free(slirp);
}

#define CONN_CANFSEND(so) (((so)->so_state & (SS_FCANTSENDMORE|SS_ISFCONNECTED)) == SS_ISFCONNECTED)
#define CONN_CANFRCV(so) (((so)->so_state & (SS_FCANTRCVMORE|SS_ISFCONNECTED)) == SS_ISFCONNECTED)
#define UPD_NFDS(x) if (nfds < (x)) nfds = (x)

static void slirp_update_timeout(uint32_t *timeout)
{
    Slirp *slirp;
    uint32_t t;

    if (*timeout <= TIMEOUT_FAST) {
        return;
    }

    t = MIN(1000, *timeout);

    /* If we have tcp timeout with slirp, then we will fill @timeout with
     * more precise value.
     */
    QTAILQ_FOREACH(slirp, &slirp_instances, entry) {
        if (slirp->time_fasttimo) {
            *timeout = TIMEOUT_FAST;
            return;
        }
        if (slirp->do_slowtimo) {
            t = MIN(TIMEOUT_SLOW, t);
        }
    }
    *timeout = t;
}

void slirp_select_fill(int *pnfds, fd_set *readfds, fd_set *writefds,
                       fd_set *xfds, uint32_t *timeout)
{
    Slirp *slirp;
    struct socket *so, *so_next;
    int nfds;

    if (QTAILQ_EMPTY(&slirp_instances)) {
        return;
    }

    /* fail safe */
    global_readfds = NULL;
    global_writefds = NULL;
    global_xfds = NULL;

    nfds = *pnfds;
    /*
     * First, TCP sockets
     */

    QTAILQ_FOREACH(slirp, &slirp_instances, entry) {
        /*
         * *_slowtimo needs calling if there are IP fragments
         * in the fragment queue, or there are TCP connections active
         */
        slirp->do_slowtimo = ((slirp->tcb.so_next != &slirp->tcb) ||
                (&slirp->ipq.ip_link != slirp->ipq.ip_link.next));

        for (so = slirp->tcb.so_next; so != &slirp->tcb;
                so = so_next) {
            so_next = so->so_next;

            /*
             * See if we need a tcp_fasttimo
             */
            if (slirp->time_fasttimo == 0 &&
                so->so_tcpcb->t_flags & TF_DELACK) {
                slirp->time_fasttimo = curtime; /* Flag when want a fasttimo */
            }

            /*
             * NOFDREF can include still connecting to local-host,
             * newly socreated() sockets etc. Don't want to select these.
             */
            if (so->so_state & SS_NOFDREF || so->s == -1) {
                continue;
            }

            /*
             * Set for reading sockets which are accepting
             */
            if (so->so_state & SS_FACCEPTCONN) {
                FD_SET(so->s, readfds);
                UPD_NFDS(so->s);
                continue;
            }

            /*
             * Set for writing sockets which are connecting
             */
            if (so->so_state & SS_ISFCONNECTING) {
                FD_SET(so->s, writefds);
                UPD_NFDS(so->s);
                continue;
            }

            /*
             * Set for writing if we are connected, can send more, and
             * we have something to send
             */
            if (CONN_CANFSEND(so) && so->so_rcv.sb_cc) {
                FD_SET(so->s, writefds);
                UPD_NFDS(so->s);
            }

            /*
             * Set for reading (and urgent data) if we are connected, can
             * receive more, and we have room for it XXX /2 ?
             */
            if (CONN_CANFRCV(so) &&
                (so->so_snd.sb_cc < (so->so_snd.sb_datalen/2))) {
                FD_SET(so->s, readfds);
                FD_SET(so->s, xfds);
                UPD_NFDS(so->s);
            }
        }

        /*
         * UDP sockets
         */
        for (so = slirp->udb.so_next; so != &slirp->udb;
                so = so_next) {
            so_next = so->so_next;

            /*
             * See if it's timed out
             */
            if (so->so_expire) {
                if (so->so_expire <= curtime) {
                    udp_detach(so);
                    continue;
                } else {
                    slirp->do_slowtimo = true; /* Let socket expire */
                }
            }

            /*
             * When UDP packets are received from over the
             * link, they're sendto()'d straight away, so
             * no need for setting for writing
             * Limit the number of packets queued by this session
             * to 4.  Note that even though we try and limit this
             * to 4 packets, the session could have more queued
             * if the packets needed to be fragmented
             * (XXX <= 4 ?)
             */
            if ((so->so_state & SS_ISFCONNECTED) && so->so_queued <= 4) {
                FD_SET(so->s, readfds);
                UPD_NFDS(so->s);
            }
        }

        /*
         * ICMP sockets
         */
        for (so = slirp->icmp.so_next; so != &slirp->icmp;
                so = so_next) {
            so_next = so->so_next;

            /*
             * See if it's timed out
             */
            if (so->so_expire) {
                if (so->so_expire <= curtime) {
                    icmp_detach(so);
                    continue;
                } else {
                    slirp->do_slowtimo = true; /* Let socket expire */
                }
            }

            if (so->so_state & SS_ISFCONNECTED) {
                FD_SET(so->s, readfds);
                UPD_NFDS(so->s);
            }
        }
    }
    slirp_update_timeout(timeout);
    *pnfds = nfds;
}

void slirp_select_poll(fd_set *readfds, fd_set *writefds, fd_set *xfds,
                       int select_error)
{
    Slirp *slirp;
    struct socket *so, *so_next;
    int ret;

    if (QTAILQ_EMPTY(&slirp_instances)) {
        return;
    }

    global_readfds = readfds;
    global_writefds = writefds;
    global_xfds = xfds;

    curtime = (u_int)(bx_pc_system.time_usec() / 1000);

    QTAILQ_FOREACH(slirp, &slirp_instances, entry) {
        /*
         * See if anything has timed out
         */
        if (slirp->time_fasttimo &&
            ((curtime - slirp->time_fasttimo) >= TIMEOUT_FAST)) {
            tcp_fasttimo(slirp);
            slirp->time_fasttimo = 0;
        }
        if (slirp->do_slowtimo &&
            ((curtime - slirp->last_slowtimo) >= TIMEOUT_SLOW)) {
            ip_slowtimo(slirp);
            tcp_slowtimo(slirp);
            slirp->last_slowtimo = curtime;
        }

        /*
         * Check sockets
         */
        if (!select_error) {
            /*
             * Check TCP sockets
             */
            for (so = slirp->tcb.so_next; so != &slirp->tcb;
                    so = so_next) {
                so_next = so->so_next;

                /*
                 * FD_ISSET is meaningless on these sockets
                 * (and they can crash the program)
                 */
                if (so->so_state & SS_NOFDREF || so->s == -1) {
                    continue;
                }
                /*
                 * Check for URG data
                 * This will soread as well, so no need to
                 * test for readfds below if this succeeds
                 */
                if (FD_ISSET(so->s, xfds)) {
                    sorecvoob(so);
                }
                /*
                 * Check sockets for reading
                 */
                else if (FD_ISSET(so->s, readfds)) {
                    /*
                     * Check for incoming connections
                     */
                    if (so->so_state & SS_FACCEPTCONN) {
                        tcp_connect(so);
                        continue;
                    } /* else */
                    ret = soread(so);

                    /* Output it if we read something */
                    if (ret > 0) {
                        tcp_output(sototcpcb(so));
                    }
                }

                /*
                 * Check sockets for writing
                 */
                if (FD_ISSET(so->s, writefds)) {
                    /*
                     * Check for non-blocking, still-connecting sockets
                     */
                    if (so->so_state & SS_ISFCONNECTING) {
                        /* Connected */
                        so->so_state &= ~SS_ISFCONNECTING;

                        ret = send(so->s, (const char*) &ret, 0, 0);
                        if (ret < 0) {
                            /* XXXXX Must fix, zero bytes is a NOP */
                            if (errno == EAGAIN || errno == EWOULDBLOCK ||
                                errno == EINPROGRESS || errno == ENOTCONN) {
                                continue;
                            }

                            /* else failed */
                            so->so_state &= SS_PERSISTENT_MASK;
                            so->so_state |= SS_NOFDREF;
                        }
                        /* else so->so_state &= ~SS_ISFCONNECTING; */

                        /*
                         * Continue tcp_input
                         */
                        tcp_input((struct mbuf *)NULL, sizeof(struct ip), so);
                        /* continue; */
                    } else {
                        ret = sowrite(so);
                    }
                    /*
                     * XXXXX If we wrote something (a lot), there
                     * could be a need for a window update.
                     * In the worst case, the remote will send
                     * a window probe to get things going again
                     */
                }

                /*
                 * Probe a still-connecting, non-blocking socket
                 * to check if it's still alive
                 */
#ifdef PROBE_CONN
                if (so->so_state & SS_ISFCONNECTING) {
                    ret = qemu_recv(so->s, &ret, 0, 0);

                    if (ret < 0) {
                        /* XXX */
                        if (errno == EAGAIN || errno == EWOULDBLOCK ||
                            errno == EINPROGRESS || errno == ENOTCONN) {
                            continue; /* Still connecting, continue */
                        }

                        /* else failed */
                        so->so_state &= SS_PERSISTENT_MASK;
                        so->so_state |= SS_NOFDREF;

                        /* tcp_input will take care of it */
                    } else {
                        ret = send(so->s, (const char*)&ret, 0, 0);
                        if (ret < 0) {
                            /* XXX */
                            if (errno == EAGAIN || errno == EWOULDBLOCK ||
                                errno == EINPROGRESS || errno == ENOTCONN) {
                                continue;
                            }
                            /* else failed */
                            so->so_state &= SS_PERSISTENT_MASK;
                            so->so_state |= SS_NOFDREF;
                        } else {
                            so->so_state &= ~SS_ISFCONNECTING;
                        }

                    }
                    tcp_input((struct mbuf *)NULL, sizeof(struct ip), so);
                } /* SS_ISFCONNECTING */
#endif
            }

            /*
             * Now UDP sockets.
             * Incoming packets are sent straight away, they're not buffered.
             * Incoming UDP data isn't buffered either.
             */
            for (so = slirp->udb.so_next; so != &slirp->udb;
                    so = so_next) {
                so_next = so->so_next;

                if (so->s != -1 && FD_ISSET(so->s, readfds)) {
                    sorecvfrom(so);
                }
            }

            /*
             * Check incoming ICMP relies.
             */
            for (so = slirp->icmp.so_next; so != &slirp->icmp;
                    so = so_next) {
                    so_next = so->so_next;

                    if (so->s != -1 && FD_ISSET(so->s, readfds)) {
                        icmp_receive(so);
                    }
            }
        }

        if_start(slirp);
    }

    /* clear global file descriptor sets.
     * these reside on the stack in vl.c
     * so they're unusable if we're not in
     * slirp_select_fill or slirp_select_poll.
     */
     global_readfds = NULL;
     global_writefds = NULL;
     global_xfds = NULL;
}

static void arp_input(Slirp *slirp, const uint8_t *pkt, int pkt_len)
{
    struct arphdr *ah = (struct arphdr *)(pkt + ETH_HLEN);
    uint8_t arp_reply[max(ETH_HLEN + sizeof(struct arphdr), 64)];
    struct ethhdr *reh = (struct ethhdr *)arp_reply;
    struct arphdr *rah = (struct arphdr *)(arp_reply + ETH_HLEN);
    int ar_op;
    struct ex_list *ex_ptr;

    ar_op = ntohs(ah->ar_op);
    switch(ar_op) {
    case ARPOP_REQUEST:
        if (ah->ar_tip == ah->ar_sip) {
            /* Gratuitous ARP */
            arp_table_add(slirp, ah->ar_sip, ah->ar_sha);
            return;
        }

        if ((ah->ar_tip & slirp->vnetwork_mask.s_addr) ==
            slirp->vnetwork_addr.s_addr) {
            if (ah->ar_tip == slirp->vnameserver_addr.s_addr ||
                ah->ar_tip == slirp->vhost_addr.s_addr)
                goto arp_ok;
            for (ex_ptr = slirp->exec_list; ex_ptr; ex_ptr = ex_ptr->ex_next) {
                if (ex_ptr->ex_addr.s_addr == ah->ar_tip)
                    goto arp_ok;
            }
            return;
        arp_ok:
            memset(arp_reply, 0, sizeof(arp_reply));

            arp_table_add(slirp, ah->ar_sip, ah->ar_sha);

            /* ARP request for alias/dns mac address */
            memcpy(reh->h_dest, pkt + ETH_ALEN, ETH_ALEN);
            memcpy(reh->h_source, special_ethaddr, ETH_ALEN - 4);
            memcpy(&reh->h_source[2], &ah->ar_tip, 4);
            reh->h_proto = htons(ETH_P_ARP);

            rah->ar_hrd = htons(1);
            rah->ar_pro = htons(ETH_P_IP);
            rah->ar_hln = ETH_ALEN;
            rah->ar_pln = 4;
            rah->ar_op = htons(ARPOP_REPLY);
            memcpy(rah->ar_sha, reh->h_source, ETH_ALEN);
            rah->ar_sip = ah->ar_tip;
            memcpy(rah->ar_tha, ah->ar_sha, ETH_ALEN);
            rah->ar_tip = ah->ar_sip;
            slirp_output(slirp->opaque, arp_reply, sizeof(arp_reply));
        }
        break;
    case ARPOP_REPLY:
        arp_table_add(slirp, ah->ar_sip, ah->ar_sha);
        break;
    default:
        break;
    }
}

void slirp_input(Slirp *slirp, const uint8_t *pkt, int pkt_len)
{
    struct mbuf *m;
    int proto;

    if (pkt_len < ETH_HLEN)
        return;

    proto = ntohs(*(uint16_t *)(pkt + 12));
    switch(proto) {
    case ETH_P_ARP:
        arp_input(slirp, pkt, pkt_len);
        break;
    case ETH_P_IP:
        m = m_get(slirp);
        if (!m)
            return;
        /* Note: we add to align the IP header */
        if (M_FREEROOM(m) < pkt_len + 2) {
            m_inc(m, pkt_len + 2);
        }
        m->m_len = pkt_len + 2;
        memcpy(m->m_data + 2, pkt, pkt_len);

        m->m_data += 2 + ETH_HLEN;
        m->m_len -= 2 + ETH_HLEN;

        ip_input(m);
        break;
    default:
        break;
    }
}

/* Output the IP packet to the ethernet device. Returns 0 if the packet must be
 * re-queued.
 */
int if_encap(Slirp *slirp, struct mbuf *ifm)
{
    uint8_t buf[1600];
    struct ethhdr *eh = (struct ethhdr *)buf;
    uint8_t ethaddr[ETH_ALEN];
    const struct ip *iph = (const struct ip *)ifm->m_data;

    if (ifm->m_len + ETH_HLEN > (int)sizeof(buf)) {
        return 1;
    }

    if (!arp_table_search(slirp, iph->ip_dst.s_addr, ethaddr)) {
        uint8_t arp_req[ETH_HLEN + sizeof(struct arphdr)];
        struct ethhdr *reh = (struct ethhdr *)arp_req;
        struct arphdr *rah = (struct arphdr *)(arp_req + ETH_HLEN);

        if (!ifm->arp_requested) {
            /* If the client addr is not known, send an ARP request */
            memset(reh->h_dest, 0xff, ETH_ALEN);
            memcpy(reh->h_source, special_ethaddr, ETH_ALEN - 4);
            memcpy(&reh->h_source[2], &slirp->vhost_addr, 4);
            reh->h_proto = htons(ETH_P_ARP);
            rah->ar_hrd = htons(1);
            rah->ar_pro = htons(ETH_P_IP);
            rah->ar_hln = ETH_ALEN;
            rah->ar_pln = 4;
            rah->ar_op = htons(ARPOP_REQUEST);

            /* source hw addr */
            memcpy(rah->ar_sha, special_ethaddr, ETH_ALEN - 4);
            memcpy(&rah->ar_sha[2], &slirp->vhost_addr, 4);

            /* source IP */
            rah->ar_sip = slirp->vhost_addr.s_addr;

            /* target hw addr (none) */
            memset(rah->ar_tha, 0, ETH_ALEN);

            /* target IP */
            rah->ar_tip = iph->ip_dst.s_addr;
            slirp->client_ipaddr = iph->ip_dst;
            slirp_output(slirp->opaque, arp_req, sizeof(arp_req));
            ifm->arp_requested = true;

            /* Expire request and drop outgoing packet after 1 second */
            ifm->expiration_date = (bx_pc_system.time_usec() + 1000000ULL) * 1000ULL;
        }
        return 0;
    } else {
        memcpy(eh->h_dest, ethaddr, ETH_ALEN);
        memcpy(eh->h_source, special_ethaddr, ETH_ALEN - 4);
        /* XXX: not correct */
        memcpy(&eh->h_source[2], &slirp->vhost_addr, 4);
        eh->h_proto = htons(ETH_P_IP);
        memcpy(buf + sizeof(struct ethhdr), ifm->m_data, ifm->m_len);
        slirp_output(slirp->opaque, buf, ifm->m_len + ETH_HLEN);
        return 1;
    }
}

/* Drop host forwarding rule, return 0 if found. */
int slirp_remove_hostfwd(Slirp *slirp, int is_udp, struct in_addr host_addr,
                         int host_port)
{
    struct socket *so;
    struct socket *head = (is_udp ? &slirp->udb : &slirp->tcb);
    struct sockaddr_in addr;
    int port = htons(host_port);
    socklen_t addr_len;

    for (so = head->so_next; so != head; so = so->so_next) {
        addr_len = sizeof(addr);
        if ((so->so_state & SS_HOSTFWD) &&
            getsockname(so->s, (struct sockaddr *)&addr, &addr_len) == 0 &&
            addr.sin_addr.s_addr == host_addr.s_addr &&
            addr.sin_port == port) {
            close(so->s);
            sofree(so);
            return 0;
        }
    }

    return -1;
}

int slirp_add_hostfwd(Slirp *slirp, int is_udp, struct in_addr host_addr,
                      int host_port, struct in_addr guest_addr, int guest_port)
{
    if (!guest_addr.s_addr) {
        guest_addr = slirp->vdhcp_startaddr;
    }
    if (is_udp) {
        if (!udp_listen(slirp, host_addr.s_addr, htons(host_port),
                        guest_addr.s_addr, htons(guest_port), SS_HOSTFWD))
            return -1;
    } else {
        if (!tcp_listen(slirp, host_addr.s_addr, htons(host_port),
                        guest_addr.s_addr, htons(guest_port), SS_HOSTFWD))
            return -1;
    }
    return 0;
}

int slirp_add_exec(Slirp *slirp, int do_pty, const void *args,
                   struct in_addr *guest_addr, int guest_port)
{
    if (!guest_addr->s_addr) {
        guest_addr->s_addr = slirp->vnetwork_addr.s_addr |
            (htonl(0x0204) & ~slirp->vnetwork_mask.s_addr);
    }
    if ((guest_addr->s_addr & slirp->vnetwork_mask.s_addr) !=
        slirp->vnetwork_addr.s_addr ||
        guest_addr->s_addr == slirp->vhost_addr.s_addr ||
        guest_addr->s_addr == slirp->vnameserver_addr.s_addr) {
        return -1;
    }
    return add_exec(&slirp->exec_list, do_pty, (char *)args, *guest_addr,
                    htons(guest_port));
}

ssize_t slirp_send(struct socket *so, const void *buf, size_t len, int flags)
{
    if (so->s == -1 && so->extra) {
        Slirp *slirp = so->slirp;
        BX_ERROR(("slirp_send(): so->extra not supported"));
        return len;
    }

    return send(so->s, (const char*)buf, len, flags);
}

static struct socket *
slirp_find_ctl_socket(Slirp *slirp, struct in_addr guest_addr, int guest_port)
{
    struct socket *so;

    for (so = slirp->tcb.so_next; so != &slirp->tcb; so = so->so_next) {
        if (so->so_faddr.s_addr == guest_addr.s_addr &&
            htons(so->so_fport) == guest_port) {
            return so;
        }
    }
    return NULL;
}

size_t slirp_socket_can_recv(Slirp *slirp, struct in_addr guest_addr,
                             int guest_port)
{
    struct iovec iov[2];
    struct socket *so;

    so = slirp_find_ctl_socket(slirp, guest_addr, guest_port);

    if (!so || so->so_state & SS_NOFDREF) {
        return 0;
    }

    if (!CONN_CANFRCV(so) || so->so_snd.sb_cc >= (so->so_snd.sb_datalen/2)) {
        return 0;
    }

    return sopreprbuf(so, iov, NULL);
}

void slirp_socket_recv(Slirp *slirp, struct in_addr guest_addr, int guest_port,
                       const uint8_t *buf, int size)
{
    int ret;
    struct socket *so = slirp_find_ctl_socket(slirp, guest_addr, guest_port);

    if (!so)
        return;

    ret = soreadbuf(so, (const char *)buf, size);

    if (ret > 0)
        tcp_output(sototcpcb(so));
}

void slirp_warning(Slirp *slirp, const char *msg)
{
    BX_ERROR((msg));
}

#endif
