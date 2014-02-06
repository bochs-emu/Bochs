/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2013  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
/////////////////////////////////////////////////////////////////////////

//
// osdep.h
//
// requires Bit32u/Bit64u from config.h, size_t from stdio.h
//
// Operating system dependent includes and defines for Bochs.  These
// declarations can be included by C or C++., but they require definition of
// size_t beforehand.  This makes it difficult to place them into either
// config.h or bochs.h.  If in config.h, size_t is not always available yet.
// If in bochs.h, they can't be included by C programs so they lose.
//

#ifndef BX_OSDEP_H
#define BX_OSDEP_H

#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

//////////////////////////////////////////////////////////////////////
// Hacks for win32, but exclude MINGW32 because it doesn't need them.
//////////////////////////////////////////////////////////////////////
#ifdef WIN32

#ifndef __MINGW32__

// Definitions that are needed for WIN32 compilers EXCEPT FOR
// cygwin compiling with -mno-cygwin.  e.g. VC++.

#if !defined(_MSC_VER)		// gcc without -mno-cygwin
#define FMT_LL "%ll"
#define FMT_TICK "%011llu"
#define FMT_ADDRX64 "%016llx"
#define FMT_PHY_ADDRX64 "%012llx"
#else
#define FMT_LL "%I64"
#define FMT_TICK "%011I64u"
#define FMT_ADDRX64 "%016I64x"
#define FMT_PHY_ADDRX64 "%012I64x"
#endif

// always return regular file.
#ifndef S_ISREG
#  define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISCHR
#  define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#endif

#if defined(_MSC_VER)
// win32 has snprintf though with different name.
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#undef BX_HAVE_SNPRINTF
#undef BX_HAVE_VSNPRINTF
#define BX_HAVE_SNPRINTF 1
#define BX_HAVE_VSNPRINTF 1

#define access _access
#define fdopen _fdopen
#define mktemp _mktemp
#define off_t __int64
#define lseek _lseeki64
#define fseeko64 _fseeki64
#define fstat _fstati64
#define stat  _stati64
#define read _read
#define write _write
#define open _open
#define close _close
#define unlink _unlink
#define strdup _strdup
#define strrev _strrev
#define stricmp _stricmp
#define getch _getch
#define strtoll _strtoi64
#define strtoull _strtoui64
#endif

#else   /* __MINGW32__ defined */
// Definitions for cygwin compiled with -mno-cygwin
#define FMT_LL "%I64"
#define FMT_TICK "%011I64u"
#define FMT_ADDRX64 "%016I64x"
#define FMT_PHY_ADDRX64 "%012I64x"

#define off_t __int64
// mingw gcc 4.6.1 already has lseek defined
#ifndef lseek
#define lseek _lseeki64
#endif
#endif  /* __MINGW32__ defined */

#else    /* not WIN32 definitions */
#if SIZEOF_UNSIGNED_LONG == 8
#define FMT_LL "%l"
#define FMT_TICK "%011lu"
#define FMT_ADDRX64 "%016lx"
#define FMT_PHY_ADDRX64 "%012lx"
#else
#define FMT_LL "%ll"
#define FMT_TICK "%011llu"
#define FMT_ADDRX64 "%016llx"
#define FMT_PHY_ADDRX64 "%012llx"
#endif
#endif   /* not WIN32 definitions */

#define FMT_ADDRX32 "%08x"

// Missing defines for open
#ifndef S_IRUSR
#define S_IRUSR 0400
#define S_IWUSR 0200
#endif
#ifndef S_IRGRP
#define S_IRGRP 0040
#define S_IWGRP 0020
#endif
#ifndef S_IROTH
#define S_IROTH 0004
#define S_IWOTH 0002
#endif

//////////////////////////////////////////////////////////////////////
// Missing library functions.
// These should work on any platform that needs them.
//
// A missing library function is renamed to a bx_* function, so that when
// debugging and linking there's no confusion over which version is used.
// Because of renaming, the bx_* replacement functions can be tested on
// machines which have the real library function without duplicate symbols.
//
// If you're considering implementing a missing library function, note
// that it might be cleaner to conditionally disable the function call!
//////////////////////////////////////////////////////////////////////

#if !BX_HAVE_SNPRINTF
  #define snprintf bx_snprintf
  extern int bx_snprintf (char *s, size_t maxlen, const char *format, ...);
#endif

#if !BX_HAVE_VSNPRINTF
  #define vsnprintf bx_vsnprintf
  extern int bx_vsnprintf (char *s, size_t maxlen, const char *format, va_list arg);
#endif

#if BX_HAVE_STRTOULL
  // great, just use the usual function
#elif BX_HAVE_STRTOUQ
  // they have strtouq and not strtoull
  #define strtoull strtouq
#else
  #define strtoull bx_strtoull
  extern Bit64u bx_strtoull (const char *nptr, char **endptr, int baseignore);
#endif

#if !BX_HAVE_STRDUP
#define strdup bx_strdup
  extern char *bx_strdup(const char *str);
#endif

#if !BX_HAVE_STRREV
#define strrev bx_strrev
  extern char *bx_strrev(char *str);
#endif

#if BX_HAVE_STRICMP
  // great, just use the usual function
#elif BX_HAVE_STRCASECMP
  #define stricmp strcasecmp
#else
  // FIXME: for now using case sensitive function
  #define stricmp strcmp
#endif

#if !BX_HAVE_SOCKLEN_T
// needed on MacOS X 10.1
typedef int socklen_t;
#endif

#if !BX_HAVE_SSIZE_T
// needed on Windows
typedef Bit64s ssize_t;
#endif

#if !BX_HAVE_MKSTEMP
#define mkstemp bx_mkstemp
  BOCHSAPI_MSVCONLY extern int bx_mkstemp(char *tpl);
#endif

//////////////////////////////////////////////////////////////////////
// Missing library functions, implemented for MacOS only
//////////////////////////////////////////////////////////////////////

#if BX_WITH_MACOS
// fd_read and fd_write are called by floppy.cc to access the Mac
// floppy drive directly, since the MacOS doesn't have "special"
// pathnames which map directly to IO devices

int fd_read(char *buffer, Bit32u offset, Bit32u bytes);
int fd_write(char *buffer, Bit32u offset, Bit32u bytes);
int fd_stat(struct stat *buf);
FILE *  fdopen(int fd, const char *type);

typedef long ssize_t ;
#endif

//////////////////////////////////////////////////////////////////////
// Missing library functions and byte-swapping stuff,
// implemented for MorphOS only
//////////////////////////////////////////////////////////////////////

#ifdef __MORPHOS__
int fseeko(FILE *stream, off_t offset, int whence);
struct tm *localtime_r(const time_t *timep, struct tm *result);

BX_CPP_INLINE Bit16u bx_ppc_bswap16(Bit16u val)
{
  Bit32u res;

  __asm__("rlwimi %0,%0,16,8,15"
          : "=r" (res)
          : "0" (val));

  return (Bit16u)(res >> 8);
}

BX_CPP_INLINE Bit32u bx_ppc_bswap32(Bit32u val)
{
  Bit32u res;

  __asm__("rotlwi %0,%1,8\n\t"
          "rlwimi %0,%1,24,0,7\n\t"
          "rlwimi %0,%1,24,16,23"
          : "=&r" (res)
          : "r" (val));

  return res;
}

BX_CPP_INLINE Bit64u bx_ppc_bswap64(Bit64u val)
{
  Bit32u hi, lo;

  __asm__("rotlwi %0,%2,8\n\t"
          "rlwimi %0,%2,24,0,7\n\t"
          "rlwimi %0,%2,24,16,23\n\t"
          "rotlwi %1,%3,8\n\t"
          "rlwimi %1,%3,24,0,7\n\t"
          "rlwimi %1,%3,24,16,23"
          : "=&r" (hi), "=&r" (lo)
          : "r" ((Bit32u)(val & 0xffffffff)), "r" ((Bit32u)(val >> 32)));

  return ((Bit64u)hi << 32) | (Bit64u)lo;
}

BX_CPP_INLINE Bit16u bx_ppc_load_le16(const Bit16u *p)
{
  Bit16u v;
  __asm__("lhbrx %0, 0, %1"
          : "=r" (v)
          : "r" (p), "m" (*p));
  return v;
}

BX_CPP_INLINE void bx_ppc_store_le16(Bit16u *p, Bit16u v)
{
  __asm__("sthbrx %1, 0, %2"
          : "=m" (*p)
          : "r" (v), "r" (p));
}

BX_CPP_INLINE Bit32u bx_ppc_load_le32(const Bit32u *p)
{
  Bit32u v;
  __asm__("lwbrx %0, 0, %1"
          : "=r" (v)
          : "r" (p), "m" (*p));
  return v;
}

BX_CPP_INLINE void bx_ppc_store_le32(Bit32u *p, Bit32u v)
{
  __asm__("stwbrx %1, 0, %2"
          : "=m" (*p)
          : "r" (v), "r" (p));
}

BX_CPP_INLINE Bit64u bx_ppc_load_le64(const Bit64u *p)
{
  Bit32u hi, lo;

  __asm__("lwbrx %0, 0, %2\n\t"
          "lwbrx %1, 0, %3"
          : "=&r" (lo), "=&r" (hi)
          : "r" ((Bit32u *)p), "r" ((Bit32u *)p+1));

  return ((Bit64u)hi << 32) | (Bit64u)lo;
}

BX_CPP_INLINE void bx_ppc_store_le64(Bit64u *p, Bit64u v)
{
  __asm__("stwbrx %1, 0, %3\n\t"
          "stwbrx %2, 0, %4"
          : "=m" (*p)
          : "r" ((Bit32u)(v & 0xffffffff)), "r" ((Bit32u)(v >> 32)),
            "r" ((Bit32u *)p), "r" ((Bit32u *)p+1));
}
#endif

//////////////////////////////////////////////////////////////////////
// New functions to replace library functions
//   with OS-independent versions
//////////////////////////////////////////////////////////////////////

#if BX_HAVE_REALTIME_USEC
// 64-bit time in useconds.
extern Bit64u bx_get_realtime64_usec (void);
#endif

#ifdef WIN32
#undef BX_HAVE_MSLEEP
#define BX_HAVE_MSLEEP 1
#if !defined(__MINGW32__) && !defined(_MSC_VER)
#define msleep(msec)	_sleep(msec)
#else
#define msleep(msec)	Sleep(msec)
#endif
#endif

#ifdef __cplusplus
}
#endif   /* __cplusplus */

#if BX_LARGE_RAMFILE

// these macros required for large ramfile option functionality
#if BX_HAVE_TMPFILE64 == 0
  #define tmpfile64 tmpfile /* use regular tmpfile() function */
#endif

#if BX_HAVE_FSEEKO64 == 0
#if BX_HAVE_FSEEK64
  #define fseeko64 fseek64 /* use fseek64() function */
#elif !defined(_MSC_VER)
  #define fseeko64 fseeko  /* use regular fseeko() function */
#endif
#endif

#endif // BX_LARGE_RAMFILE

#endif /* ifdef BX_OSDEP_H */
