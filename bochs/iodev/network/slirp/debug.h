/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 1995 Danny Gasparovski.
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DBG_CALL (1 << 0)
#define DBG_MISC (1 << 1)
#define DBG_ERROR (1 << 2)
#define DBG_TFTP (1 << 3)
#define DBG_VERBOSE_CALL (1 << 4)

extern int slirp_debug;

#define DEBUG_CALL(name)                        \
    do {                                        \
        if (unlikely(slirp_debug & DBG_CALL)) { \
            slirplog_debug(name "...");         \
        }                                       \
    } while (0)

#define DEBUG_VERBOSE_CALL(name)                        \
    do {                                                \
        if (unlikely(slirp_debug & DBG_VERBOSE_CALL)) { \
            slirplog_debug(name "...");                 \
        }                                               \
    } while (0)

#define DEBUG_RAW_CALL(...)                     \
    do {                                        \
        if (unlikely(slirp_debug & DBG_CALL)) { \
            slirplog_debug(__VA_ARGS__);        \
        }                                       \
    } while (0)

#define DEBUG_ARG(...)                              \
    do {                                            \
        if (unlikely(slirp_debug & DBG_CALL)) {     \
            slirplog_debug(" " __VA_ARGS__);        \
        }                                           \
    } while (0)

#define DEBUG_MISC(...)                         \
    do {                                        \
        if (unlikely(slirp_debug & DBG_MISC)) { \
            slirplog_debug(__VA_ARGS__);        \
        }                                       \
    } while (0)

#define DEBUG_ERROR(...)                         \
    do {                                         \
        if (unlikely(slirp_debug & DBG_ERROR)) { \
            slirplog_debug(__VA_ARGS__);         \
        }                                        \
    } while (0)

#define DEBUG_TFTP(...)                         \
    do {                                        \
        if (unlikely(slirp_debug & DBG_TFTP)) { \
            slirplog_debug(__VA_ARGS__);        \
        }                                       \
    } while (0)

#endif
