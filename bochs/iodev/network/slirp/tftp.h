/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

/* tftp defines */
#ifndef SLIRP_TFTP_H
#define SLIRP_TFTP_H 1

#define TFTP_SESSIONS_MAX 3

#define TFTP_SERVER	69

#define TFTP_BUFFER_SIZE   1024

struct tftp_t {
  struct ip ip;
  struct udphdr udp;
  uint16_t tp_op;
  union {
    struct {
      uint16_t tp_block_nr;
      uint8_t tp_buf[TFTP_BUFFER_SIZE];
    } tp_data;
    struct {
      uint16_t tp_error_code;
      uint8_t tp_msg[TFTP_BUFFER_SIZE];
    } tp_error;
    char tp_buf[TFTP_BUFFER_SIZE + 2];
  } x;
};

struct tftp_session {
    Slirp *slirp;
    char *filename;
    int fd;

    struct in_addr client_ip;
    uint16_t client_port;
    uint32_t block_nr;
    bx_bool  write;
    unsigned options;
    size_t   tsize_val;
    unsigned blksize_val;
    unsigned timeout_val;

    int timestamp;
};

void tftp_input(struct mbuf *m);

#endif
