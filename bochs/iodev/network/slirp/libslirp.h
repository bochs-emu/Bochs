/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LIBSLIRP_H
#define LIBSLIRP_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <in6addr.h>
#include <basetsd.h>
typedef Bit64s slirp_ssize_t;
#else
#include <sys/types.h>
typedef ssize_t slirp_ssize_t;
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

/* Opaque structure containing the slirp state */
typedef struct Slirp Slirp;

/* Flags passed to SlirpAddPollCb and to be returned by SlirpGetREventsCb. */
enum {
    SLIRP_POLL_IN = 1 << 0,
    SLIRP_POLL_OUT = 1 << 1,
    SLIRP_POLL_PRI = 1 << 2,
    SLIRP_POLL_ERR = 1 << 3,
    SLIRP_POLL_HUP = 1 << 4,
};

/* Callback for application to get data from the guest */
typedef slirp_ssize_t (*SlirpReadCb)(void *buf, size_t len, void *opaque);
/* Callback for application to send data to the guest */
typedef slirp_ssize_t (*SlirpWriteCb)(const void *buf, size_t len, void *opaque);
/* Timer callback */
typedef void (*SlirpTimerCb)(void *opaque);
/* Callback for libslirp to register polling callbacks */
typedef int (*SlirpAddPollCb)(int fd, int events, void *opaque);
/* Callback for libslirp to get polling result */
typedef int (*SlirpGetREventsCb)(int idx, void *opaque);

/* For now libslirp creates only a timer for the IPv6 RA */
typedef enum SlirpTimerId {
    SLIRP_TIMER_RA,
    SLIRP_TIMER_NUM,
} SlirpTimerId;

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
    /* Create a new timer with the given callback and opaque data. Not
     * needed if timer_new_opaque is provided. */
    void *(*timer_new)(SlirpTimerCb cb, void *cb_opaque, void *opaque);
    /* Remove and free a timer */
    void (*timer_free)(void *timer, void *opaque);
    /* Modify a timer to expire at @expire_time (ms) */
    void (*timer_mod)(void *timer, int64_t expire_time, void *opaque);
    /* Register a fd for future polling */
    void (*register_poll_fd)(int fd, void *opaque);
    /* Unregister a fd */
    void (*unregister_poll_fd)(int fd, void *opaque);
    /* Kick the io-thread, to signal that new events may be processed because some TCP buffer
     * can now receive more data, i.e. slirp_socket_can_recv will return 1. */
    void (*notify)(void *opaque);

    /*
     * Fields introduced in SlirpConfig version 4 begin
     */

    /* Initialization has completed and a Slirp* has been created.  */
    void (*init_completed)(Slirp *slirp, void *opaque);
    /* Create a new timer.  When the timer fires, the application passes
     * the SlirpTimerId and cb_opaque to slirp_handle_timer.  */
    void *(*timer_new_opaque)(SlirpTimerId id, void *cb_opaque, void *opaque);
} SlirpCb;

#define SLIRP_CONFIG_VERSION_MIN 1
#define SLIRP_CONFIG_VERSION_MAX 5

typedef struct SlirpConfig {
    /* Version must be provided */
    uint32_t version;
    /*
     * Fields introduced in SlirpConfig version 1 begin
     */
    /* Whether to prevent the guest from accessing the Internet */
    int restricted;
    /* Whether IPv4 is enabled */
    bool in_enabled;
    /* Virtual network for the guest */
    struct in_addr vnetwork;
    /* Mask for the virtual network for the guest */
    struct in_addr vnetmask;
    /* Virtual address for the host exposed to the guest */
    struct in_addr vhost;
    /* Whether IPv6 is enabled */
    bool in6_enabled;
    /* Virtual IPv6 network for the guest */
    struct in6_addr vprefix_addr6;
    /* Len of the virtual IPv6 network for the guest */
    uint8_t vprefix_len;
    /* Virtual address for the host exposed to the guest */
    struct in6_addr vhost6;
    /* Hostname exposed to the guest in DHCP hostname option */
    const char *vhostname;
    /* Hostname exposed to the guest in the DHCP TFTP server name option */
    const char *tftp_server_name;
    /* Path of the files served by TFTP */
    const char *tftp_path;
    /* Boot file name exposed to the guest via DHCP */
    const char *bootfile;
    /* Start of the DHCP range */
    struct in_addr vdhcp_start;
    /* Virtual address for the DNS server exposed to the guest */
    struct in_addr vnameserver;
    /* Virtual IPv6 address for the DNS server exposed to the guest */
    struct in6_addr vnameserver6;
    /* DNS search names exposed to the guest via DHCP */
    const char **vdnssearch;
    /* Domain name exposed to the guest via DHCP */
    const char *vdomainname;
    /* MTU when sending packets to the guest */
    /* Default: IF_MTU_DEFAULT */
    size_t if_mtu;
    /* MRU when receiving packets from the guest */
    /* Default: IF_MRU_DEFAULT */
    size_t if_mru;
    /* Prohibit connecting to 127.0.0.1:* */
    bool disable_host_loopback;
    /*
     * Enable emulation code (*warning*: this code isn't safe, it is not
     * recommended to enable it)
     */
    bool enable_emu;

    /*
     * Fields introduced in SlirpConfig version 2 begin
     */
    /* Address to be used when sending data to the Internet */
    struct sockaddr_in *outbound_addr;
    /* IPv6 Address to be used when sending data to the Internet */
    struct sockaddr_in6 *outbound_addr6;

    /*
     * Fields introduced in SlirpConfig version 3 begin
     */
    /* slirp will not redirect/serve any DNS packet */
    bool disable_dns;

    /*
     * Fields introduced in SlirpConfig version 4 begin
     */
    /* slirp will not reply to any DHCP requests */
    bool disable_dhcp;

    /*
     * Fields introduced in SlirpConfig version 5 begin
     */
    /* Manufacturer ID (IANA Private Enterprise number) */
    uint32_t mfr_id;
    /*
     * MAC address allocated for an out-of-band management controller, to be
     * retrieved through NC-SI.
     */
    uint8_t oob_eth_addr[6];
} SlirpConfig;

/* Create a new instance of a slirp stack */
Slirp *slirp_new(const SlirpConfig *cfg, const SlirpCb *callbacks,
                 void *opaque);
/* slirp_init is deprecated in favor of slirp_new */
Slirp *slirp_init(int restricted, bool in_enabled, struct in_addr vnetwork,
                  struct in_addr vnetmask, struct in_addr vhost,
                  bool in6_enabled, struct in6_addr vprefix_addr6,
                  uint8_t vprefix_len, struct in6_addr vhost6,
                  const char *vhostname, const char *tftp_server_name,
                  const char *tftp_path, const char *bootfile,
                  struct in_addr vdhcp_start, struct in_addr vnameserver,
                  struct in6_addr vnameserver6, const char **vdnssearch,
                  const char *vdomainname, const SlirpCb *callbacks,
                  void *opaque);
/* Shut down an instance of a slirp stack */
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
