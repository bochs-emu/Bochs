//
// osdep.h
// 
// Operating system dependent includes and defines for Bochs.
// This file is included by bochs.h, which is included by almost
// everything.
//

#ifndef BX_OSDEP_H
#define BX_OSDEP_H

// This code recognizes the following preprocessor symbols for different
// operating systems:
//   macintosh
//   WIN32

#if BX_HAVE_SNPRINTF
#define bx_snprintf snprintf
#else
extern "C" {
  extern int bx_snprintf (char *s, size_t maxlen, const char *format, ...);
}
#endif

#if BX_HAVE_STRTOULL
  // great, just use the usual function
#elif BX_HAVE_STRTOUQ
  // they have strtouq and not strtoull
  #define strtoull strtouq
#else
  #define strtoull bx_strtoull
extern "C" {
  extern Bit64u bx_strtoull (const char *nptr, char **endptr, int baseignore);
}
#endif

#endif /* ifdef BX_OSDEP_H */
