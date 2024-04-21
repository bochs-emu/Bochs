/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LIBSLIRP_H
#define LIBSLIRP_H

#include <stdint.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
typedef ssize_t slirp_ssize_t;

struct Slirp;
typedef struct Slirp Slirp;

/* Callback for application to send data to the guest */
typedef slirp_ssize_t (*SlirpWriteCb)(const void *buf, size_t len, void *opaque);

/*
 * Callbacks from slirp, to be set by the application.
 *
 * The opaque parameter is set to the opaque pointer given in the slirp_new /
 * slirp_init call.
 */
typedef struct SlirpCb {
    /*
     * Send an ethernet frame to the guest network. The opaque parameter is the
     * one given to slirp_init(). If the guest is not ready to receive a frame,
     * the function can just drop the data. TCP will then handle retransmissions
     * at a lower pace.
     * <0 reports an IO error.
     */
    SlirpWriteCb send_packet;
    /* Print a message for an error due to guest misbehavior.  */
    void (*guest_error)(const char *msg, void *opaque);
    /* Return the virtual clock value in nanoseconds */
    int64_t (*clock_get_ns)(void *opaque);
} SlirpCb;

typedef struct SlirpConfig {
  int restricted;
  struct in_addr vnetwork, vnetmask, vhost, vdhcp_start, vnameserver;
  const char *bootfile, *vhostname, **vdnssearch, *tftp_path, *tftp_server_name;
  const char *vdomainname;
} SlirpConfig;

int get_dns_addr(struct in_addr *pdns_addr);

Slirp *slirp_init(int restricted, struct in_addr vnetwork,
                  struct in_addr vnetmask, struct in_addr vhost,
                  const char *vhostname, const char *tftp_path,
                  const char *bootfile, struct in_addr vdhcp_start,
                  struct in_addr vnameserver, const char **vdnssearch,
                  const char *vdomainname, const SlirpCb *callbacks, void *opaque);
Slirp *slirp_new(SlirpConfig *cfg, SlirpCb *callbacks, void *opaque);
void slirp_cleanup(Slirp *slirp);

void slirp_select_fill(int *pnfds, fd_set *readfds, fd_set *writefds,
                       fd_set *xfds, uint32_t *timeout);

void slirp_select_poll(fd_set *readfds, fd_set *writefds, fd_set *xfds,
                       int select_error);

void slirp_input(Slirp *slirp, const uint8_t *pkt, int pkt_len);

/* you must provide the following functions: */
void slirp_warning(const char *, void *);
int slirp_add_hostfwd(Slirp *slirp, int is_udp,
                      struct in_addr host_addr, int host_port,
                      struct in_addr guest_addr, int guest_port);
int slirp_remove_hostfwd(Slirp *slirp, int is_udp,
                         struct in_addr host_addr, int host_port);
int slirp_add_exec(Slirp *slirp, const void *args,
                   struct in_addr *guest_addr, int guest_port);

void slirp_socket_recv(Slirp *slirp, struct in_addr guest_addr,
                       int guest_port, const uint8_t *buf, int size);
size_t slirp_socket_can_recv(Slirp *slirp, struct in_addr guest_addr,
                             int guest_port);

#endif
