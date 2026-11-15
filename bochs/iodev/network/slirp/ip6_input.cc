/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2013
 * Guillaume Subiron, Yann Bordenave, Serigne Modou Wagne.
 */

#include "slirp.h"
#include "ip6_icmp.h"

#if BX_NETWORKING && BX_NETMOD_SLIRP

/*
 * IP initialization: fill in IP protocol switch table.
 * All protocols not implemented in kernel go to raw IP protocol handler.
 */
void ip6_post_init(Slirp *slirp)
{
    icmp6_post_init(slirp);
}

void ip6_cleanup(Slirp *slirp)
{
    icmp6_cleanup(slirp);
}

void ip6_input(struct mbuf *m)
{
    Slirp *slirp = m->slirp;
    /* NDP reads the ethernet header for gratuitous NDP */
    M_DUP_DEBUG(slirp, m, 1, TCPIPHDR_DELTA + 2 + ETH_HLEN);

    struct ip6 *ip6;
    int ip6_len;

    if (!slirp->in6_enabled) {
        goto bad;
    }

    DEBUG_CALL("ip6_input");
    DEBUG_ARG("m = %p", m);
    DEBUG_ARG("m_len = %d", m->m_len);

    if (m->m_len < (int)sizeof(struct ip6)) {
        goto bad;
    }

    ip6 = mtod(m, struct ip6 *);

    if (ip6->ip_v != IP6VERSION) {
        goto bad;
    }

    ip6_len = ntohs(ip6->ip_pl) + sizeof(struct ip6);

    if (ip6_len > slirp->if_mtu) {
        icmp6_send_error(m, ICMP6_TOOBIG, 0);
        goto bad;
    }

    // Check if the message size is big enough to hold what's
    // set in the payload length header. If not this is an invalid
    // packet
    if (m->m_len < ip6_len) {
        goto bad;
    }

    /* Should drop packet if mbuf too long? hmmm... */
    if (m->m_len > ip6_len)
        m_adj(m, ip6_len - m->m_len);

    /* check ip_ttl for a correct ICMP reply */
    if (ip6->ip_hl == 0) {
        icmp6_send_error(m, ICMP6_TIMXCEED, ICMP6_TIMXCEED_INTRANS);
        goto bad;
    }

    if (in6_martian_source(&ip6->ip_src)
     || in6_martian_destination(&ip6->ip_dst)) {
        goto bad;
    }

    if (IN6_IS_ADDR_V4MAPPED(&ip6->ip_src)
     || IN6_IS_ADDR_V4COMPAT(&ip6->ip_src)) {
        struct in_addr ip4_src;
        memcpy(&ip4_src, &ip6->ip_src.s6_addr[12], 4);
        if (in_martian_source(ntohl(ip4_src.s_addr))) {
            goto bad;
        }
    }

    if (IN6_IS_ADDR_V4MAPPED(&ip6->ip_dst)
     || IN6_IS_ADDR_V4COMPAT(&ip6->ip_dst)) {
        struct in_addr ip4_dst;
        memcpy(&ip4_dst, &ip6->ip_dst.s6_addr[12], 4);
        if (in_martian_destination(ntohl(ip4_dst.s_addr))) {
            goto bad;
        }
    }

    /*
     * Switch out to protocol's input routine.
     */
    switch (ip6->ip_nh) {
    case IPPROTO_TCP:
        NTOHS(ip6->ip_pl);
        tcp_input(m, sizeof(struct ip6), (struct socket *)NULL, AF_INET6);
        break;
    case IPPROTO_UDP:
        udp6_input(m);
        break;
    case IPPROTO_ICMPV6:
        icmp6_input(m);
        break;
    default:
        m_free(m);
    }
    return;
bad:
    m_free(m);
}

#endif
