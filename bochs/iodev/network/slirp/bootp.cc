/* SPDX-License-Identifier: MIT */
/*
 * QEMU BOOTP/DHCP server
 * Bochs additions: parameter list and some other options
 *
 * Copyright (c) 2004 Fabrice Bellard
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
#include "slirp.h"

#if BX_NETWORKING && BX_NETMOD_SLIRP

#if defined(_WIN32)
/* Windows ntohl() returns an u_long value.
 * Add a type cast to match the format strings. */
#define ntohl(n) ((uint32_t)ntohl(n))
#endif

/* XXX: only DHCP is supported */

#define LEASE_TIME (24 * 3600)

typedef struct {
    int msg_type;
    bool found_srv_id;
    struct in_addr req_addr;
    uint8_t *params;
    uint8_t params_len;
    char *hostname;
    uint32_t lease_time;
} dhcp_options_t;

#define UEFI_HTTP_VENDOR_CLASS_ID "HTTPClient"

static const uint8_t rfc1533_cookie[] = { RFC1533_COOKIE };

#define DPRINTF(...) DEBUG_RAW_CALL(__VA_ARGS__)

static BOOTPClient *get_new_addr(Slirp *slirp, struct in_addr *paddr,
                                 const uint8_t *macaddr)
{
    BOOTPClient *bc;
    int i;

    for(i = 0; i < NB_BOOTP_CLIENTS; i++) {
        bc = &slirp->bootp_clients[i];
        if (!bc->allocated || !memcmp(macaddr, bc->macaddr, 6))
            goto found;
    }
    return NULL;
 found:
    bc = &slirp->bootp_clients[i];
    bc->allocated = 1;
    paddr->s_addr = slirp->vdhcp_startaddr.s_addr + htonl(i);
    return bc;
}

static BOOTPClient *request_addr(Slirp *slirp, const struct in_addr *paddr,
                                 const uint8_t *macaddr)
{
    uint32_t req_addr = ntohl(paddr->s_addr);
    uint32_t dhcp_addr = ntohl(slirp->vdhcp_startaddr.s_addr);
    BOOTPClient *bc;

    if (req_addr >= dhcp_addr && req_addr < (dhcp_addr + NB_BOOTP_CLIENTS)) {
        bc = &slirp->bootp_clients[req_addr - dhcp_addr];
        if (!bc->allocated || !memcmp(macaddr, bc->macaddr, 6)) {
            bc->allocated = 1;
            return bc;
        }
    }
    return NULL;
}

static BOOTPClient *find_addr(Slirp *slirp, struct in_addr *paddr,
                              const uint8_t *macaddr)
{
    BOOTPClient *bc;
    int i;

    for(i = 0; i < NB_BOOTP_CLIENTS; i++) {
        if (!memcmp(macaddr, slirp->bootp_clients[i].macaddr, 6))
            goto found;
    }
    return NULL;
 found:
    bc = &slirp->bootp_clients[i];
    bc->allocated = 1;
    paddr->s_addr = slirp->vdhcp_startaddr.s_addr + htonl(i);
    return bc;
}

static void dhcp_decode(Slirp *slirp, const struct bootp_t *bp,
                        const uint8_t *bp_end,
                        dhcp_options_t *opts)
{
    const uint8_t *p;
    uint16_t defsize, maxsize;
    int len, tag;

    memset(opts, 0, sizeof(dhcp_options_t));

    p = bp->bp_vend;
    if (memcmp(p, rfc1533_cookie, 4) != 0)
        return;
    p += 4;
    while (p < bp_end) {
        tag = p[0];
        if (tag == RFC1533_PAD) {
            p++;
        } else if (tag == RFC1533_END) {
            break;
        } else {
            p++;
            if (p >= bp_end)
                break;
            len = *p++;
            if (p + len > bp_end) {
                break;
            }
            DPRINTF("dhcp: tag=%d len=%d", tag, len);

            switch(tag) {
              case RFC2132_MSG_TYPE:
                if (len >= 1)
                    opts->msg_type = p[0];
                break;
              case RFC2132_REQ_ADDR:
                if (len >= 4) {
                    memcpy(&(opts->req_addr.s_addr), p, 4);
                }
                break;
              case RFC2132_SRV_ID:
                if (len >= 4) {
                    if (!memcmp(p, &slirp->vhost_addr, 4)) {
                        opts->found_srv_id = 1;
                    }
                }
                break;
              case RFC2132_PARAM_LIST:
                if (len >= 1) {
                    opts->params = (uint8_t*)malloc(len);
                    memcpy(opts->params, p, len);
                    opts->params_len = len;
                }
                break;
              case RFC1533_HOSTNAME:
                if (len >= 1) {
                    opts->hostname = (char*)malloc(len + 1);
                    memcpy(opts->hostname, p, len);
                    opts->hostname[len] = 0;
                }
                break;
              case RFC2132_LEASE_TIME:
                if (len == 4) {
                    memcpy(&opts->lease_time, p, len);
                }
                break;
              case RFC2132_MAX_SIZE:
                if (len == 2) {
                    memcpy(&maxsize, p, len);
                    defsize = sizeof(struct bootp_t) + DHCP_OPT_LEN - sizeof(struct ip) - sizeof(struct udphdr);
                    if (ntohs(maxsize) < defsize) {
                        slirplog_error("DHCP server: RFB2132_MAX_SIZE=%u not supported yet", ntohs(maxsize));
                    }
                }
                break;
              default:
                slirplog_error("DHCP server: option %d not supported yet", tag);
                break;
            }
            p += len;
        }
    }
    if ((opts->msg_type == DHCPREQUEST) && (opts->req_addr.s_addr == htonl(0L)) &&
        bp->bp_ciaddr.s_addr) {
        memcpy(&(opts->req_addr.s_addr), &bp->bp_ciaddr, 4);
    }
}

static void bootp_reply(Slirp *slirp,
                        const struct bootp_t *bp,
                        const uint8_t *bp_end)
{
    BOOTPClient *bc = NULL;
    struct mbuf *m;
    struct bootp_t *rbp;
    struct sockaddr_in saddr, daddr;
    struct in_addr bcast_addr;
    int val;
    uint8_t *q, *end, *pp, plen;
    uint8_t client_ethaddr[ETH_ALEN];
    dhcp_options_t dhcp_opts;

    /* extract exact DHCP msg type */
    dhcp_decode(slirp, bp, bp_end, &dhcp_opts);
    DPRINTF("bootp packet op=%d msgtype=%d", bp->bp_op, dhcp_opts.msg_type);
    if (dhcp_opts.req_addr.s_addr != htonl(0L)) {
        DPRINTF(" req_addr=%08x", ntohl(dhcp_opts.req_addr.s_addr));
    }

    if (dhcp_opts.msg_type == 0)
        dhcp_opts.msg_type = DHCPREQUEST; /* Force reply for old BOOTP clients */

    if (dhcp_opts.msg_type != DHCPDISCOVER &&
        dhcp_opts.msg_type != DHCPREQUEST)
        return;

    /* Get client's hardware address from bootp request */
    memcpy(client_ethaddr, bp->bp_hwaddr, ETH_ALEN);

    m = m_get(slirp);
    if (!m) {
        return;
    }
    m->m_data += IF_MAXLINKHDR;
    m_inc(m, sizeof(struct bootp_t) + DHCP_OPT_LEN);
    rbp = (struct bootp_t *)m->m_data;
    m->m_data += sizeof(struct udpiphdr);
    memset(rbp, 0, sizeof(struct bootp_t) + DHCP_OPT_LEN);

    if (dhcp_opts.msg_type == DHCPDISCOVER) {
        if (dhcp_opts.req_addr.s_addr != htonl(0L)) {
            bc = request_addr(slirp, &dhcp_opts.req_addr, client_ethaddr);
            if (bc) {
                daddr.sin_addr = dhcp_opts.req_addr;
            }
        }
        if (!bc) {
         new_addr:
            bc = get_new_addr(slirp, &daddr.sin_addr, client_ethaddr);
            if (!bc) {
                DPRINTF("no address left");
                return;
            }
        }
        memcpy(bc->macaddr, client_ethaddr, ETH_ALEN);
    } else if (dhcp_opts.req_addr.s_addr != htonl(0L)) {
        bc = request_addr(slirp, &dhcp_opts.req_addr, client_ethaddr);
        if (bc) {
            daddr.sin_addr = dhcp_opts.req_addr;
            memcpy(bc->macaddr, client_ethaddr, ETH_ALEN);
        } else {
            /* DHCPNAKs should be sent to broadcast */
            daddr.sin_addr.s_addr = 0xffffffff;
        }
    } else {
        bc = find_addr(slirp, &daddr.sin_addr, bp->bp_hwaddr);
        if (!bc) {
            /* if never assigned, behaves as if it was already
               assigned (windows fix because it remembers its address) */
            goto new_addr;
        }
    }

    /* Update ARP table for this IP address */
    arp_table_add(slirp, daddr.sin_addr.s_addr, client_ethaddr);

    saddr.sin_addr = slirp->vhost_addr;
    saddr.sin_port = htons(BOOTP_SERVER);

    daddr.sin_port = htons(BOOTP_CLIENT);

    rbp->bp_op = BOOTP_REPLY;
    rbp->bp_xid = bp->bp_xid;
    rbp->bp_htype = 1;
    rbp->bp_hlen = 6;
    memcpy(rbp->bp_hwaddr, bp->bp_hwaddr, ETH_ALEN);

    rbp->bp_yiaddr = daddr.sin_addr; /* Client IP address */
    rbp->bp_siaddr = saddr.sin_addr; /* Server IP address */

    q = rbp->bp_vend;
    end = rbp->bp_vend + DHCP_OPT_LEN;
    memcpy(q, rfc1533_cookie, 4);
    q += 4;

    if (bc) {
        DPRINTF("%s addr=%08x",
                (dhcp_opts.msg_type == DHCPDISCOVER) ? "offered" : "ack'ed",
                ntohl(daddr.sin_addr.s_addr));

        if (dhcp_opts.msg_type == DHCPDISCOVER) {
            *q++ = RFC2132_MSG_TYPE;
            *q++ = 1;
            *q++ = DHCPOFFER;
        } else /* DHCPREQUEST */ {
            *q++ = RFC2132_MSG_TYPE;
            *q++ = 1;
            *q++ = DHCPACK;
        }

        if (slirp->bootp_filename)
            snprintf((char *)rbp->bp_file, sizeof(rbp->bp_file), "%s",
                     slirp->bootp_filename);

        strcpy((char *)rbp->bp_sname, "slirp");

        // Default DHCP options must be on top of reply
        *q++ = RFC2132_SRV_ID;
        *q++ = 4;
        memcpy(q, &saddr.sin_addr, 4);
        q += 4;
        *q++ = RFC2132_LEASE_TIME;
        *q++ = 4;
        if ((dhcp_opts.lease_time != 0) &&
            (ntohl(dhcp_opts.lease_time) < LEASE_TIME)) {
            memcpy(q, &dhcp_opts.lease_time, 4);
        } else {
            val = htonl(LEASE_TIME);
            memcpy(q, &val, 4);
        }
        q += 4;
        if (*slirp->client_hostname || (dhcp_opts.hostname != NULL)) {
            val = 0;
            if (*slirp->client_hostname) {
                val = strlen(slirp->client_hostname);
            } else if (dhcp_opts.hostname != NULL) {
                val = strlen(dhcp_opts.hostname);
            }
            if (val > 0) {
                *q++ = RFC1533_HOSTNAME;
                *q++ = val;
                if (*slirp->client_hostname) {
                    memcpy(q, slirp->client_hostname, val);
                } else {
                    memcpy(q, dhcp_opts.hostname, val);
                }
                q += val;
            }
            if (dhcp_opts.hostname != NULL) {
                free(dhcp_opts.hostname);
                dhcp_opts.hostname = NULL;
            }
        }
        pp = dhcp_opts.params;
        plen = dhcp_opts.params_len;
        while (plen-- > 0) {
            if (q + 6 >= end) break;
            switch (*pp++) {
                case RFC1533_NETMASK:
                    *q++ = RFC1533_NETMASK;
                    *q++ = 4;
                    memcpy(q, &slirp->vnetwork_mask, 4);
                    q += 4;
                    break;
                case RFC1533_GATEWAY:
                    if (!slirp->restricted) {
                        *q++ = RFC1533_GATEWAY;
                        *q++ = 4;
                        memcpy(q, &saddr.sin_addr, 4);
                        q += 4;
                    }
                    break;
                case RFC1533_DNS:
                    if (!slirp->restricted) {
                        *q++ = RFC1533_DNS;
                        *q++ = 4;
                        memcpy(q, &slirp->vnameserver_addr, 4);
                        q += 4;
                    }
                    break;
                case RFC1533_DOMAINNAME:
                    if (slirp->vdomainname) {
                      val = strlen(slirp->vdomainname);
                      if (q + val + 2 >= end) {
                        slirplog_error("DHCP packet size exceeded, omitting domain name option.");
                      } else {
                        *q++ = RFC1533_DOMAINNAME;
                        *q++ = val;
                        memcpy(q, slirp->vdomainname, val);
                        q += val;
                      }
                    }
                    break;
                case RFC1533_INTBROADCAST:
                    *q++ = RFC1533_INTBROADCAST;
                    *q++ = 4;
                    bcast_addr.s_addr = slirp->vhost_addr.s_addr | ~slirp->vnetwork_mask.s_addr;
                    memcpy(q, &bcast_addr, 4);
                    q += 4;
                    break;
                case RFC2132_RENEWAL_TIME:
                    *q++ = RFC2132_RENEWAL_TIME;
                    *q++ = 4;
                    val = htonl(600);
                    memcpy(q, &val, 4);
                    q += 4;
                    break;
                case RFC2132_REBIND_TIME:
                    *q++ = RFC2132_REBIND_TIME;
                    *q++ = 4;
                    val = htonl(1800);
                    memcpy(q, &val, 4);
                    q += 4;
                    break;
                case RFC2132_TFTP_SERVER_NAME:
                    if (slirp->tftp_server_name) {
                      val = strlen(slirp->tftp_server_name);
                      if (q + val + 2 >= end) {
                        slirplog_error("DHCP packet size exceeded, omitting tftp-server-name option.");
                      } else {
                        *q++ = RFC2132_TFTP_SERVER_NAME;
                        *q++ = val;
                        memcpy(q, slirp->tftp_server_name, val);
                        q += val;
                      }
                    }
                    break;
                case RFC1533_HOSTNAME:
                case RFC2132_LEASE_TIME:
                   // Already handled on top of reply
                   break;
                default:
                    slirplog_error("DHCP server: requested parameter %u not supported yet",
                                   *(pp-1));
            }
        }

        if (slirp->vdnssearch) {
            val = slirp->vdnssearch_len;
            if (q + val >= end) {
                slirplog_error("DHCP packet size exceeded, omitting domain-search option.");
            } else {
                memcpy(q, slirp->vdnssearch, val);
                q += val;
            }
        }

        /* this allows to support UEFI HTTP boot: according to the UEFI
           specification, DHCP server must send vendor class identifier option
           set to "HTTPClient" string, when responding to DHCP requests as part
           of the UEFI HTTP boot

           we assume that, if the bootfile parameter was configured as an http
           URL, the user intends to perform UEFI HTTP boot, so send this option
           automatically */
        if (slirp->bootp_filename && !strncmp(slirp->bootp_filename, "http://", 7)) {
            val = strlen(UEFI_HTTP_VENDOR_CLASS_ID);
            if (q + val + 2 >= end) {
                slirplog_error("DHCP packet size exceeded, omitting vendor class id option.");
            } else {
                *q++ = RFC2132_VENDOR_CLASS_ID;
                *q++ = val;
                memcpy(q, UEFI_HTTP_VENDOR_CLASS_ID, val);
                q += val;
            }
        }
    } else {
        static const char nak_msg[] = "requested address not available";

        DPRINTF("nak'ed addr=%08x", ntohl(dhcp_opts.req_addr.s_addr));

        *q++ = RFC2132_MSG_TYPE;
        *q++ = 1;
        *q++ = DHCPNAK;

        *q++ = RFC2132_MESSAGE;
        *q++ = sizeof(nak_msg) - 1;
        memcpy(q, nak_msg, sizeof(nak_msg) - 1);
        q += sizeof(nak_msg) - 1;
    }
    *q = RFC1533_END;

    daddr.sin_addr.s_addr = 0xffffffffu;

    if (dhcp_opts.params != NULL) free(dhcp_opts.params);

    m->m_len = sizeof(struct bootp_t) + (end - rbp->bp_vend) - sizeof(struct ip) - sizeof(struct udphdr);
    udp_output(NULL, m, &saddr, &daddr, IPTOS_LOWDELAY);
}

void bootp_input(struct mbuf *m)
{
    struct bootp_t *bp = (struct bootp_t *)mtod_check(m, sizeof(struct bootp_t));

    if (!m->slirp->disable_dhcp && bp && bp->bp_op == BOOTP_REQUEST) {
        bootp_reply(m->slirp, bp, (uint8_t*)m_end(m));
    }
}

#endif
