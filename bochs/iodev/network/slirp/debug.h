/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 1995 Danny Gasparovski.
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG

#define DBG_CALL (1 << 0)
#define DBG_MISC (1 << 1)
#define DBG_ERROR (1 << 2)
#define DBG_TFTP (1 << 3)
#define DBG_VERBOSE_CALL (1 << 4)

#define dfd stderr

extern int slirp_debug;

#define DEBUG_CALL(x) if (slirp_debug & DBG_CALL) { fprintf(dfd, "%s...\n", x); fflush(dfd); }
#define DEBUG_ARG(x, y) if (slirp_debug & DBG_CALL) { fputc(' ', dfd); fprintf(dfd, x, y); fputc('\n', dfd); fflush(dfd); }
#define DEBUG_ARGS(x) if (slirp_debug & DBG_CALL) { fprintf x ; fflush(dfd); }
#define DEBUG_MISC(x) if (slirp_debug & DBG_MISC) { fprintf x ; fflush(dfd); }
#define DEBUG_ERROR(x) if (slirp_debug & DBG_ERROR) {fprintf x ; fflush(dfd); }
#define DEBUG_TFTP(x,y) if (slirp_debug & DBG_TFTP) {fprintf x ; fflush(dfd); }

#else

#define DEBUG_CALL(...)
#define DEBUG_ARG(...)
#define DEBUG_ARGS(...)
#define DEBUG_MISC(...)
#define DEBUG_ERROR(...)
#define DEBUG_TFTP(...)

#endif

#endif
