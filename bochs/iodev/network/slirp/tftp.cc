/* SPDX-License-Identifier: MIT */
/*
 * tftp.c - a simple, read-only tftp server for qemu
 * Bochs additions: write support, 'blksize' and 'timeout' options
 *
 * Copyright (C) 2004       Magnus Damm <damm@opensource.se>
 * Copyright (C) 2014-2024  The Bochs Project
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Missing defines for open (copied from osdep.h)
#ifndef S_IRUSR
#define S_IRUSR 0400
#define S_IWUSR 0200
#endif
#ifndef S_IRGRP
#define S_IRGRP 0040
#define S_IWGRP 0020
#endif

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

static int tftp_session_allocate(Slirp *slirp, struct sockaddr_storage *srcsas,
                                 struct tftphdr *hdr)
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

    return -1;

found:
    memset(spt, 0, sizeof(*spt));
    memcpy(&spt->client_addr, srcsas, sockaddr_size(srcsas));
    spt->fd = -1;
    spt->client_port = hdr->udp.uh_sport;
    spt->slirp = slirp;
    spt->write = (ntohs(hdr->tp_op) == TFTP_WRQ);
    spt->nb_options = 0;
    spt->blksize_val = TFTP_DEFAULT_BLKSIZE;
    spt->timeout_val = TFTP_DEFAULT_TIMEOUT;

    tftp_session_update(spt);

    return k;
}

static int tftp_session_find(Slirp *slirp, struct sockaddr_storage *srcsas,
                             struct tftphdr *hdr)
{
    struct tftp_session *spt;
    int k;

    for (k = 0; k < TFTP_SESSIONS_MAX; k++) {
        spt = &slirp->tftp_sessions[k];

        if (tftp_session_in_use(spt)) {
            if (sockaddr_equal(&spt->client_addr, srcsas)) {
                if (spt->client_port == hdr->udp.uh_sport) {
                    return k;
                }
            }
        }
    }

    return -1;
}

void tftp_cleanup(Slirp *slirp)
{
    struct tftp_session *spt;
    int k;

    for (k = 0; k < TFTP_SESSIONS_MAX; k++) {
        spt = &slirp->tftp_sessions[k];

        if (tftp_session_in_use(spt)) {
            tftp_session_terminate(spt);
        }
    }
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
        if (lseek(spt->fd, block_nr * spt->blksize_val, SEEK_SET) == (off_t)-1) {
          return -1;
        }

        bytes_read = read(spt->fd, buf, len);
    }

    return bytes_read;
}

static struct tftp_t *tftp_prep_mbuf_data(struct tftp_session *spt,
                                          struct mbuf *m)
{
    struct tftp_t *tp;

    memset(m->m_data, 0, m->m_size);

    m->m_data += IF_MAXLINKHDR;
    if (spt->client_addr.ss_family == AF_INET6) {
        m->m_data += sizeof(struct ip6);
    } else {
        m->m_data += sizeof(struct ip);
    }
    tp = (struct tftp_t *)m->m_data;
    m->m_data += sizeof(struct udphdr);

    return tp;
}

static void tftp_udp_output(struct tftp_session *spt, struct mbuf *m,
                            struct tftphdr *hdr)
{
    if (spt->client_addr.ss_family == AF_INET6) {
        struct sockaddr_in6 sa6, da6;

        sa6.sin6_addr = spt->slirp->vhost_addr6;
        sa6.sin6_port = hdr->udp.uh_dport;
        da6.sin6_addr = ((struct sockaddr_in6 *)&spt->client_addr)->sin6_addr;
        da6.sin6_port = spt->client_port;

        udp6_output(NULL, m, &sa6, &da6);
    } else {
        struct sockaddr_in sa4, da4;

        sa4.sin_addr = spt->slirp->vhost_addr;
        sa4.sin_port = hdr->udp.uh_dport;
        da4.sin_addr = ((struct sockaddr_in *)&spt->client_addr)->sin_addr;
        da4.sin_port = spt->client_port;

        udp_output(NULL, m, &sa4, &da4, IPTOS_LOWDELAY);
    }
}

static int tftp_send_oack(struct tftp_session *spt, struct tftp_t *recv_tp)
{
    struct mbuf *m;
    struct tftp_t *tp;
    int i, n = 0;

    m = m_get(spt->slirp);

    if (!m)
        return -1;

    tp = tftp_prep_mbuf_data(spt, m);

    tp->hdr.tp_op = htons(TFTP_OACK);
    for (i = 0; i < spt->nb_options; i++) {
        n += snprintf(tp->x.tp_buf + n, sizeof(tp->x.tp_buf) - n, "%s", spt->option_name[i]) + 1;
        n += snprintf(tp->x.tp_buf + n, sizeof(tp->x.tp_buf) - n, "%u", spt->option_value[i]) + 1;
    }

    m->m_len = G_SIZEOF_MEMBER(struct tftp_t, hdr.tp_op) + n;
    tftp_udp_output(spt, m, &recv_tp->hdr);

    return 0;
}

static int tftp_send_ack(struct tftp_session *spt,
                          struct tftp_t *recv_tp)
{
    struct mbuf *m;
    struct tftp_t *tp;

    m = m_get(spt->slirp);

    if (!m)
        return -1;

    tp = tftp_prep_mbuf_data(spt, m);

    tp->hdr.tp_op = htons(TFTP_ACK);
    tp->x.tp_data.tp_block_nr = htons(spt->block_nr & 0xffff);

    m->m_len = G_SIZEOF_MEMBER(struct tftp_t, hdr.tp_op) + 2;
    tftp_udp_output(spt, m, &recv_tp->hdr);

    return 0;
}

static void tftp_send_error(struct tftp_session *spt, uint16_t errorcode,
                            const char *msg, struct tftp_t *recv_tp)
{
  struct mbuf *m;
  struct tftp_t *tp;

  DEBUG_TFTP("tftp error msg: %s", msg);

  m = m_get(spt->slirp);

  if (!m) {
    goto out;
  }

  tp = tftp_prep_mbuf_data(spt, m);

  tp->hdr.tp_op = htons(TFTP_ERROR);
  tp->x.tp_error.tp_error_code = htons(errorcode);
  slirp_pstrcpy((char *)tp->x.tp_error.tp_msg, sizeof(tp->x.tp_error.tp_msg),
                msg);

  m->m_len = sizeof(struct tftp_t) - (TFTP_BLOCKSIZE_MAX + 2) + 3 +
                    strlen(msg) - sizeof(struct udphdr);
  tftp_udp_output(spt, m, &recv_tp->hdr);

out:
  tftp_session_terminate(spt);
}

static void tftp_send_next_block(struct tftp_session *spt,
                                 struct tftphdr *hdr)
{
  struct mbuf *m;
  struct tftp_t *tp;
  int nobytes;

  m = m_get(spt->slirp);

  if (!m) {
    return;
  }

  tp = tftp_prep_mbuf_data(spt, m);

  tp->hdr.tp_op = htons(TFTP_DATA);
  tp->x.tp_data.tp_block_nr = htons((spt->block_nr + 1) & 0xffff);

  nobytes = tftp_read_data(spt, spt->block_nr, tp->x.tp_data.tp_buf, spt->blksize_val);

  if (nobytes < 0) {
    /* send "file not found" error back */

    tftp_send_error(spt, 1, "File not found", tp);

    m_free(m);

    return;
  }

  m->m_len = sizeof(struct tftp_t) - (TFTP_BLOCKSIZE_MAX - nobytes) -
                    sizeof(struct udphdr);
  tftp_udp_output(spt, m, hdr);

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
      break;
    }

    if (strcasecmp(key, "tsize") == 0) {
      int tsize = atoi(value);
      struct stat stat_p;

      if (tsize == 0) {
        if (spt->write) {
          tftp_send_error(spt, 2, "Access violation", tp);
          return;
        } else {
          if (stat(spt->filename, &stat_p) == 0)
            tsize = stat_p.st_size;
          else {
            tftp_send_error(spt, 1, "File not found", tp);
            return;
          }
        }
      } if (spt->write) {
        spt->tsize_val = tsize;
      }

      spt->option_name[spt->nb_options] = "tsize";
      spt->option_value[spt->nb_options] = tsize;
      spt->nb_options++;
    } else if (strcasecmp(key, "blksize") == 0) {
      int blksize = atoi(value);
      spt->blksize_val = blksize;
      if (spt->blksize_val > TFTP_BLOCKSIZE_MAX) {
        spt->blksize_val = TFTP_DEFAULT_BLKSIZE;
      }

      spt->option_name[spt->nb_options] = "blksize";
      spt->option_value[spt->nb_options] = spt->blksize_val;
      spt->nb_options++;
    } else if (strcasecmp(key, "timeout") == 0) {
      int timeout = atoi(value);
      spt->timeout_val = timeout;
      if ((spt->timeout_val < 1) || (spt->timeout_val > 255)) {
        spt->timeout_val = TFTP_DEFAULT_TIMEOUT;
      }

      spt->option_name[spt->nb_options] = "timeout";
      spt->option_value[spt->nb_options] = spt->timeout_val;
      spt->nb_options++;
    }
  }
}

static void tftp_handle_rrq(Slirp *slirp, struct sockaddr_storage *srcsas,
                            struct tftp_t *tp, int pktlen)
{
    struct tftp_session *spt;
    int s, k;
    size_t prefix_len;
    char *req_fname;

    /* check if a session already exists and if so terminate it */
    s = tftp_session_find(slirp, srcsas, &tp->hdr);
    if (s >= 0) {
        tftp_session_terminate(&slirp->tftp_sessions[s]);
    }

    s = tftp_session_allocate(slirp, srcsas, &tp->hdr);

    if (s < 0) {
        return;
    }

    spt = &slirp->tftp_sessions[s];

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

    DEBUG_TFTP("tftp rrq file: %s", req_fname);

    /* check mode */
    if ((pktlen - k) < 6) {
        tftp_send_error(spt, 2, "Access violation", tp);
        return;
    }

    if (strcasecmp(&tp->x.tp_buf[k], "octet") != 0) {
        tftp_send_error(spt, 4, "Unsupported transfer mode", tp);
        return;
    }

    k += 6; /* skipping octet */

    /* do sanity checks on the filename */
    if (
#ifdef _WIN32
        strstr(req_fname, "..\\") ||
        req_fname[strlen(req_fname) - 1] == '\\' ||
#endif
        strstr(req_fname, "../") ||
        req_fname[strlen(req_fname) - 1] == '/') {
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

    tftp_parse_options(spt, tp, k, pktlen);

    if (spt->nb_options > 0) {
        tftp_send_oack(spt, tp);
    } else {
        spt->block_nr = 0;
        tftp_send_next_block(spt, &tp->hdr);
    }
}

static void tftp_handle_wrq(Slirp *slirp, struct sockaddr_storage *srcsas,
                            struct tftp_t *tp, int pktlen)
{
    struct tftp_session *spt;
    int s, k;
    size_t prefix_len;
    char *req_fname;

    /* check if a session already exists and if so terminate it */
    s = tftp_session_find(slirp, srcsas, &tp->hdr);
    if (s >= 0) {
        tftp_session_terminate(&slirp->tftp_sessions[s]);
    }

    s = tftp_session_allocate(slirp, srcsas, &tp->hdr);

    if (s < 0) {
        return;
    }

    spt = &slirp->tftp_sessions[s];

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

    DEBUG_TFTP("tftp wrq file: %s", req_fname);

    /* do sanity checks on the filename */
    if (
#ifdef _WIN32
        strstr(req_fname, "..\\") ||
        req_fname[strlen(req_fname) - 1] == '\\' ||
#endif
        strstr(req_fname, "../") ||
        req_fname[strlen(req_fname) - 1] == '/') {
        tftp_send_error(spt, 2, "Access violation", tp);
        return;
    }

    /* check if the file exists */
    if (tftp_read_data(spt, 0, NULL, 0) >= 0) {
        close(spt->fd);
        tftp_send_error(spt, 6, "File exists", tp);
        return;
    }

    if (tp->x.tp_buf[pktlen - 1] != 0) {
        tftp_send_error(spt, 2, "Access violation", tp);
        return;
    }

    tftp_parse_options(spt, tp, k, pktlen);

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

    spt->block_nr = 0;
    if (spt->nb_options > 0) {
        tftp_send_oack(spt, tp);
    } else {
        tftp_send_ack(spt, tp);
    }
}

static void tftp_handle_data(Slirp *slirp, struct sockaddr_storage *srcsas,
                            struct tftp_t *tp, int pktlen)
{
    struct tftp_session *spt;
    int s, nobytes;

    /* check if the session exists */
    s = tftp_session_find(slirp, srcsas, &tp->hdr);
    if (s < 0) {
        return;
    }

    spt = &slirp->tftp_sessions[s];

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

static void tftp_handle_ack(Slirp *slirp, struct sockaddr_storage *srcsas,
                            struct tftphdr *hdr)
{
    int s;

    s = tftp_session_find(slirp, srcsas, hdr);

    if (s < 0) {
        return;
    }

    tftp_send_next_block(&slirp->tftp_sessions[s], hdr);
}

static void tftp_handle_error(Slirp *slirp, struct sockaddr_storage *srcsas,
                              struct tftphdr *hdr)
{
    int s;

    s = tftp_session_find(slirp, srcsas, hdr);

    if (s < 0) {
        return;
    }

    tftp_session_terminate(&slirp->tftp_sessions[s]);
}

void tftp_input(struct sockaddr_storage *srcsas, struct mbuf *m)
{
  struct tftphdr *hdr = (struct tftphdr *)mtod_check(m, sizeof(struct tftphdr));

  if (hdr == NULL) {
    return;
  }

  switch (ntohs(hdr->tp_op)) {
    case TFTP_RRQ:
        tftp_handle_rrq(m->slirp, srcsas,
                        mtod(m, struct tftp_t *),
                        m->m_len);
        break;

    case TFTP_WRQ:
      tftp_handle_wrq(m->slirp, srcsas,
                        mtod(m, struct tftp_t *),
                        m->m_len);
      break;

    case TFTP_DATA:
      tftp_handle_data(m->slirp, srcsas,
                        mtod(m, struct tftp_t *),
                        m->m_len);
      break;

    case TFTP_ACK:
        tftp_handle_ack(m->slirp, srcsas, hdr);
        break;

    case TFTP_ERROR:
        tftp_handle_error(m->slirp, srcsas, hdr);
        break;
  }
}

#endif
