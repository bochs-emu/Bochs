/////////////////////////////////////////////////////////////////////////
// $Id: osdep.h,v 1.10 2002-03-06 18:04:58 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

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

// Definitions that are needed for all WIN32 compilers.
#  define ssize_t long

#ifndef __MINGW32__
// Definitions that are needed for WIN32 compilers EXCEPT FOR
// cygwin compiling with -mno-cygwin.  e.g. VC++.

// always return regular file.
#  define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#  define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)

  // VCPP includes also are missing these
#  define off_t long

// win32 has snprintf though with different name.
#define snprintf _snprintf
#endif  /* ifnndef __MINGW32__ */
#endif   /* WIN32 */

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
#endif

#ifdef __cplusplus
}
#endif   /* __cplusplus */

#endif /* ifdef BX_OSDEP_H */
