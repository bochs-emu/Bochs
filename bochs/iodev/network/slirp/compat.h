/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////

/*
 * Qemu compatibility defines
 *
 * Copyright (c) 2003-2008  Fabrice Bellard
 * Copyright (C) 2014-2017  The Bochs Project
 */

#ifndef SLIRP_COMPAT_H
#define SLIRP_COMPAT_H 1

#include <assert.h>
#include <stddef.h>

#if defined(_MSC_VER)
#include <io.h>
#define strdup     _strdup
#define open       _open
#define close      _close
#define lseek      _lseeki64
#define read       _read
#define write      _write
#define strcasecmp _stricmp
#endif

#define SLIRP_N_ELEMENTS(x) (sizeof(x) / sizeof((x)[0]))

#define slirp_rand_int_range(min, max) ((rand() % (max - min)) + min)

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define BX_STATIC_ASSERT(s) \
    static_assert (s, "Invalid structure size");

#endif
