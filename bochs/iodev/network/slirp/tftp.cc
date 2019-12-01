/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
/*
 * A simple TFTP server (ported from Qemu)
 * Bochs additions: write support, 'blksize' and 'timeout' options
 *
 * Copyright (c) 2004 Magnus Damm <damm@opensource.se>
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

// Missing defines for open (copied from osdep.h)
#ifndef S_IRUSR
#define S_IRUSR 0400
#define S_IWUSR 0200
#endif
#ifndef S_IRGRP
#define S_IRGRP 0040
#define S_IWGRP 0020
#endif

// internal TFTP defines
#define TFTP_RRQ    1
#define TFTP_WRQ    2
#define TFTP_DATA   3
#define TFTP_ACK    4
#define TFTP_ERROR  5
#define TFTP_OPTACK 6

#define TFTP_FILENAME_MAX 512

#define TFTP_OPTION_OCTET   0x1
#define TFTP_OPTION_BLKSIZE 0x2
#define TFTP_OPTION_TSIZE   0x4
#define TFTP_OPTION_TIMEOUT 0x8

#define TFTP_DEFAULT_BLKSIZE 512
#define TFTP_DEFAULT_TIMEOUT   5

static inline int tftp_session_in_use(struct tftp_session *spt)
{
    return (spt->slirp != NULL);
}

static inline void tftp_session_update(struct tftp_session *spt)
{
    spt->timestamp = curtime;
}

static void tftp_session_terminate(struct tftp_session *spt)
{
    if (spt->fd >= 0) {
        close(spt->fd);
        spt->fd = -1;
    }
    free(spt->filename);
    spt->slirp = NULL;
}

static tftp_session *tftp_session_allocate(Slirp *slirp, struct tftp_t *tp)
{
  struct tftp_session *spt;
  int k;

  for (k = 0; k < TFTP_SESSIONS_MAX; k++) {
    spt = &slirp->tftp_sessions[k];

    if (!tftp_session_in_use(spt))
        goto found;

    /* sessions time out after 5 inactive seconds */
    if ((curtime - spt->timestamp) > (spt->timeout_val * 1000)) {
        tftp_session_terminate(spt);
        goto found;
    }
  }

  return NULL;

 found:
  memset(spt, 0, sizeof(*spt));
  memcpy(&spt->client_ip, &tp->ip.ip_src, sizeof(spt->client_ip));
  spt->fd = -1;
  spt->client_port = tp->udp.uh_sport;
  spt->slirp = slirp;
  spt->write = (ntohs(tp->tp_op) == TFTP_WRQ);
  spt->options = 0;
  spt->blksize_val = TFTP_DEFAULT_BLKSIZE;
  spt->timeout_val = TFTP_DEFAULT_TIMEOUT;

  tftp_session_update(spt);

  return spt;
}

static tftp_session *tftp_session_find(Slirp *slirp, struct tftp_t *tp)
{
  struct tftp_session *spt;
  int k;

  for (k = 0; k < TFTP_SESSIONS_MAX; k++) {
    spt = &slirp->tftp_sessions[k];

    if (tftp_session_in_use(spt)) {
      if (!memcmp(&spt->client_ip, &tp->ip.ip_src, sizeof(spt->client_ip))) {
        if (spt->client_port == tp->udp.uh_sport) {
          return spt;
        }
      }
    }
  }

  return NULL;
}

static int tftp_read_data(struct tftp_session *spt, uint32_t block_nr,
                          uint8_t *buf, int len)
{
    int bytes_read = 0;

    if (spt->fd < 0) {
        spt->fd = open(spt->filename, O_RDONLY | O_BINARY);
    }

    if (spt->fd < 0) {
        return -1;
    }

    if (len) {
        lseek(spt->fd, block_nr * spt->blksize_val, SEEK_SET);

        bytes_read = read(spt->fd, buf, len);
    }

    return bytes_read;
}

static int tftp_send_optack(struct tftp_session *spt,
                            struct tftp_t *recv_tp)
{
    struct sockaddr_in saddr, daddr;
    struct mbuf *m;
    struct tftp_t *tp;
    int n = 0;

    m = m_get(spt->slirp);

    if (!m)
        return -1;

    memset(m->m_data, 0, m->m_size);

    m->m_data += IF_MAXLINKHDR;
    tp = (tftp_t*)m->m_data;
    m->m_data += sizeof(struct udpiphdr);

    tp->tp_op = htons(TFTP_OPTACK);
    if (spt->options & TFTP_OPTION_TSIZE) {
        n += snprintf(tp->x.tp_buf + n, sizeof(tp->x.tp_buf) - n, "%s",
                      "tsize") + 1;
        n += snprintf(tp->x.tp_buf + n, sizeof(tp->x.tp_buf) - n, "%u",
                      (unsigned)spt->tsize_val) + 1;
    }
    if (spt->options & TFTP_OPTION_BLKSIZE) {
        n += snprintf(tp->x.tp_buf + n, sizeof(tp->x.tp_buf) - n, "%s",
                      "blksize") + 1;
        n += snprintf(tp->x.tp_buf + n, sizeof(tp->x.tp_buf) - n, "%u",
                      (unsigned)spt->blksize_val) + 1;
    }
    if (spt->options & TFTP_OPTION_TIMEOUT) {
        n += snprintf(tp->x.tp_buf + n, sizeof(tp->x.tp_buf) - n, "%s",
                      "timeout") + 1;
        n += snprintf(tp->x.tp_buf + n, sizeof(tp->x.tp_buf) - n, "%u",
                      (unsigned)spt->timeout_val) + 1;
    }

    saddr.sin_addr = recv_tp->ip.ip_dst;
    saddr.sin_port = recv_tp->udp.uh_dport;

    daddr.sin_addr = spt->client_ip;
    daddr.sin_port = spt->client_port;

    m->m_len = sizeof(struct tftp_t) - 514 + n -
        sizeof(struct ip) - sizeof(struct udphdr);
    udp_output2(NULL, m, &saddr, &daddr, IPTOS_LOWDELAY);

    return 0;
}

static int tftp_send_ack(struct tftp_session *spt,
                          struct tftp_t *recv_tp)
{
    struct sockaddr_in saddr, daddr;
    struct mbuf *m;
    struct tftp_t *tp;

    m = m_get(spt->slirp);

    if (!m)
        return -1;

    memset(m->m_data, 0, m->m_size);

    m->m_data += IF_MAXLINKHDR;
    tp = (tftp_t*)m->m_data;
    m->m_data += sizeof(struct udpiphdr);

    tp->tp_op = htons(TFTP_ACK);
    tp->x.tp_data.tp_block_nr = htons(spt->block_nr & 0xffff);

    saddr.sin_addr = recv_tp->ip.ip_dst;
    saddr.sin_port = recv_tp->udp.uh_dport;

    daddr.sin_addr = spt->client_ip;
    daddr.sin_port = spt->client_port;

    m->m_len = sizeof(struct tftp_t) - 514 + 2 -
        sizeof(struct ip) - sizeof(struct udphdr);
    udp_output2(NULL, m, &saddr, &daddr, IPTOS_LOWDELAY);

    return 0;
}

static void tftp_send_error(struct tftp_session *spt,
                            uint16_t errorcode, const char *msg,
                            struct tftp_t *recv_tp)
{
  struct sockaddr_in saddr, daddr;
  struct mbuf *m;
  struct tftp_t *tp;

  m = m_get(spt->slirp);

  if (!m) {
    goto out;
  }

  memset(m->m_data, 0, m->m_size);

  m->m_data += IF_MAXLINKHDR;
  tp = (tftp_t*)m->m_data;
  m->m_data += sizeof(struct udpiphdr);

  tp->tp_op = htons(TFTP_ERROR);
  tp->x.tp_error.tp_error_code = htons(errorcode);
  pstrcpy((char *)tp->x.tp_error.tp_msg, sizeof(tp->x.tp_error.tp_msg), msg);

  saddr.sin_addr = recv_tp->ip.ip_dst;
  saddr.sin_port = recv_tp->udp.uh_dport;

  daddr.sin_addr = spt->client_ip;
  daddr.sin_port = spt->client_port;

  m->m_len = sizeof(struct tftp_t) - 514 + 3 + strlen(msg) -
        sizeof(struct ip) - sizeof(struct udphdr);

  udp_output2(NULL, m, &saddr, &daddr, IPTOS_LOWDELAY);

out:
  tftp_session_terminate(spt);
}

static void tftp_send_next_block(struct tftp_session *spt,
                                 struct tftp_t *recv_tp)
{
  struct sockaddr_in saddr, daddr;
  struct mbuf *m;
  struct tftp_t *tp;
  int nobytes;

  m = m_get(spt->slirp);

  if (!m) {
    return;
  }

  memset(m->m_data, 0, m->m_size);

  m->m_data += IF_MAXLINKHDR;
  tp = (tftp_t*)m->m_data;
  m->m_data += sizeof(struct udpiphdr);

  tp->tp_op = htons(TFTP_DATA);
  tp->x.tp_data.tp_block_nr = htons((spt->block_nr + 1) & 0xffff);

  saddr.sin_addr = recv_tp->ip.ip_dst;
  saddr.sin_port = recv_tp->udp.uh_dport;

  daddr.sin_addr = spt->client_ip;
  daddr.sin_port = spt->client_port;

  nobytes = tftp_read_data(spt, spt->block_nr, tp->x.tp_data.tp_buf, spt->blksize_val);

  if (nobytes < 0) {
    m_free(m);

    /* send "file not found" error back */

    tftp_send_error(spt, 1, "File not found", tp);

    return;
  }

  m->m_len = sizeof(struct tftp_t) - (TFTP_BUFFER_SIZE - nobytes) -
        sizeof(struct ip) - sizeof(struct udphdr);

  udp_output2(NULL, m, &saddr, &daddr, IPTOS_LOWDELAY);

  if (nobytes == (int)spt->blksize_val) {
    tftp_session_update(spt);
  } else {
    tftp_session_terminate(spt);
  }

  spt->block_nr++;
}

static void tftp_parse_options(struct tftp_session *spt, struct tftp_t *tp, int k, int pktlen)
{
  const char *key, *value;

  while (k < pktlen) {
      key = &tp->x.tp_buf[k];
      k += strlen(key) + 1;

      if (k < pktlen) {
          value = &tp->x.tp_buf[k];
          k += strlen(value) + 1;
      } else {
          value = NULL;
      }

      if (strcasecmp(key, "octet") == 0) {
          spt->options |= TFTP_OPTION_OCTET;
      } else if (strcasecmp(key, "tsize") == 0) {
          spt->options |= TFTP_OPTION_TSIZE;
          if (spt->write && (value != NULL)) {
              spt->tsize_val = atoi(value);
          }
      } else if (strcasecmp(key, "blksize") == 0) {
          if (value != NULL) {
              spt->options |= TFTP_OPTION_BLKSIZE;
              spt->blksize_val = atoi(value);
              if (spt->blksize_val > TFTP_BUFFER_SIZE) {
                  spt->blksize_val = TFTP_DEFAULT_BLKSIZE;
              }
          }
      } else if (strcasecmp(key, "timeout") == 0) {
          if (value != NULL) {
              spt->options |= TFTP_OPTION_TIMEOUT;
              spt->timeout_val = atoi(value);
              if ((spt->timeout_val < 1) || (spt->timeout_val > 255)) {
                  spt->timeout_val = TFTP_DEFAULT_TIMEOUT;
              }
          }
      }
  }
}

static void tftp_handle_rrq(Slirp *slirp, struct tftp_t *tp, int pktlen)
{
  struct tftp_session *spt;
  int k;
  size_t prefix_len;
  char *req_fname;

  /* check if a session already exists and if so terminate it */
  spt = tftp_session_find(slirp, tp);
  if (spt != NULL) {
    tftp_session_terminate(spt);
  }

  spt = tftp_session_allocate(slirp, tp);
  if (spt == NULL) {
    return;
  }

  /* unspecified prefix means service disabled */
  if (!slirp->tftp_prefix) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
  }

  /* skip header fields */
  k = 0;
  pktlen -= offsetof(struct tftp_t, x.tp_buf);

  /* prepend tftp_prefix */
  prefix_len = strlen(slirp->tftp_prefix);
  spt->filename = (char*)malloc(prefix_len + TFTP_FILENAME_MAX + 2);
  memcpy(spt->filename, slirp->tftp_prefix, prefix_len);
  spt->filename[prefix_len] = '/';

  /* get name */
  req_fname = spt->filename + prefix_len + 1;

  while (1) {
    if (k >= TFTP_FILENAME_MAX || k >= pktlen) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
    }
    req_fname[k] = tp->x.tp_buf[k];
    if (req_fname[k++] == '\0') {
      break;
    }
  }

  tftp_parse_options(spt, tp, k, pktlen);
  if (!(spt->options & TFTP_OPTION_OCTET)) {
      tftp_send_error(spt, 4, "Unsupported transfer mode", tp);
      return;
  }

  /* do sanity checks on the filename */
  if (!strncmp(req_fname, "../", 3) ||
      req_fname[strlen(req_fname) - 1] == '/' ||
      strstr(req_fname, "/../")) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
  }

  /* check if the file exists */
  if (tftp_read_data(spt, 0, NULL, 0) < 0) {
      tftp_send_error(spt, 1, "File not found", tp);
      return;
  }

  if (tp->x.tp_buf[pktlen - 1] != 0) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
  }

  if (spt->options & TFTP_OPTION_TSIZE) {
      struct stat stat_p;
      if (stat(spt->filename, &stat_p) == 0) {
          spt->tsize_val = stat_p.st_size;
      } else {
          tftp_send_error(spt, 1, "File not found", tp);
          return;
      }
  }

  if ((spt->options & ~TFTP_OPTION_OCTET) > 0) {
      tftp_send_optack(spt, tp);
  } else {
      spt->block_nr = 0;
      tftp_send_next_block(spt, tp);
  }
}

static void tftp_handle_wrq(Slirp *slirp, struct tftp_t *tp, int pktlen)
{
  struct tftp_session *spt;
  int k, fd;
  size_t prefix_len;
  char *req_fname;

  /* check if a session already exists and if so terminate it */
  spt = tftp_session_find(slirp, tp);
  if (spt != NULL) {
    tftp_session_terminate(spt);
  }

  spt = tftp_session_allocate(slirp, tp);
  if (spt == NULL) {
    return;
  }

  /* unspecified prefix means service disabled */
  if (!slirp->tftp_prefix) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
  }

  /* skip header fields */
  k = 0;
  pktlen -= offsetof(struct tftp_t, x.tp_buf);

  /* prepend tftp_prefix */
  prefix_len = strlen(slirp->tftp_prefix);
  spt->filename = (char*)malloc(prefix_len + TFTP_FILENAME_MAX + 2);
  memcpy(spt->filename, slirp->tftp_prefix, prefix_len);
  spt->filename[prefix_len] = '/';

  /* get name */
  req_fname = spt->filename + prefix_len + 1;

  while (1) {
    if (k >= TFTP_FILENAME_MAX || k >= pktlen) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
    }
    req_fname[k] = tp->x.tp_buf[k];
    if (req_fname[k++] == '\0') {
      break;
    }
  }

  tftp_parse_options(spt, tp, k, pktlen);
  if (!(spt->options & TFTP_OPTION_OCTET)) {
      tftp_send_error(spt, 4, "Unsupported transfer mode", tp);
      return;
  }

  /* do sanity checks on the filename */
  if (!strncmp(req_fname, "../", 3) ||
      req_fname[strlen(req_fname) - 1] == '/' ||
      strstr(req_fname, "/../")) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
  }

  /* check if the file exists */
  fd = open(spt->filename, O_RDONLY | O_BINARY);
  if (fd >= 0) {
      close(fd);
      tftp_send_error(spt, 6, "File exists", tp);
      return;
  }
  /* create new file */
  spt->fd = open(spt->filename, O_CREAT | O_WRONLY
#ifdef O_BINARY
    | O_BINARY
#endif
    , S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP);
  if (spt->fd < 0) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
  }

  if (tp->x.tp_buf[pktlen - 1] != 0) {
      tftp_send_error(spt, 2, "Access violation", tp);
      return;
  }

  spt->block_nr = 0;
  if ((spt->options & ~TFTP_OPTION_OCTET) > 0) {
      tftp_send_optack(spt, tp);
  } else {
      tftp_send_ack(spt, tp);
  }
}

static void tftp_handle_data(Slirp *slirp, struct tftp_t *tp, int pktlen)
{
  struct tftp_session *spt;
  int nobytes;

  spt = tftp_session_find(slirp, tp);
  if (spt == NULL) {
    return;
  }

  if (spt->write == 1) {
    spt->block_nr = ntohs(tp->x.tp_data.tp_block_nr);
    nobytes = pktlen - offsetof(struct tftp_t, x.tp_data.tp_buf);
    if (nobytes <= (int)spt->blksize_val) {
      lseek(spt->fd, (spt->block_nr - 1) * spt->blksize_val, SEEK_SET);
      write(spt->fd, tp->x.tp_data.tp_buf, nobytes);
      tftp_send_ack(spt, tp);
      if (nobytes == (int)spt->blksize_val) {
        tftp_session_update(spt);
      } else {
        tftp_session_terminate(spt);
      }
    } else {
      tftp_send_error(spt, 2, "Access violation", tp);
    }
  } else {
    tftp_send_error(spt, 2, "Access violation", tp);
  }
}

static void tftp_handle_ack(Slirp *slirp, struct tftp_t *tp, int pktlen)
{
  struct tftp_session *spt;

  spt = tftp_session_find(slirp, tp);
  if (spt == NULL) {
    return;
  }
  tftp_send_next_block(spt, tp);
}

static void tftp_handle_error(Slirp *slirp, struct tftp_t *tp, int pktlen)
{
  struct tftp_session *spt;

  spt = tftp_session_find(slirp, tp);
  if (spt == NULL) {
    return;
  }
  tftp_session_terminate(spt);
}

void tftp_input(struct mbuf *m)
{
  struct tftp_t *tp = (struct tftp_t *)m->m_data;

  switch (ntohs(tp->tp_op)) {
    case TFTP_RRQ:
      tftp_handle_rrq(m->slirp, tp, m->m_len);
      break;

    case TFTP_WRQ:
      tftp_handle_wrq(m->slirp, tp, m->m_len);
      break;

    case TFTP_DATA:
      tftp_handle_data(m->slirp, tp, m->m_len);
      break;

    case TFTP_ACK:
      tftp_handle_ack(m->slirp, tp, m->m_len);
      break;

    case TFTP_ERROR:
      tftp_handle_error(m->slirp, tp, m->m_len);
      break;
  }
}

#endif
