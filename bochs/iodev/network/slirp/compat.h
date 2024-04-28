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
#ifndef BX_OSDEP_H
#define strdup     _strdup
#define open       _open
#define close      _close
#define lseek      _lseeki64
#define read       _read
#define write      _write
#endif
#define strcasecmp _stricmp
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#endif
