//
// osdep.h
// 
// Operating system dependent includes and defines for Bochs.  This file
// can be included by C or C++., but it requires definition of size_t
// beforehand.  This makes it difficult to place into either config.h or
// bochs.h.  If in config.h, size_t is not always available yet.  If in
// bochs.h, it can't be included by C programs so they lose.
//

#ifndef BX_OSDEP_H
#define BX_OSDEP_H

// This code recognizes the following preprocessor symbols for different
// operating systems:
//   macintosh
//   WIN32

#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */



#if BX_HAVE_SNPRINTF
#define bx_snprintf snprintf
#else
  extern int bx_snprintf (char *s, size_t maxlen, const char *format, ...);
#endif


#if BX_HAVE_STRTOULL
#define bx_strtoull strtoull
#else
  extern unsigned long long bx_strtoull (const char *nptr, char **endptr, int baseignore);
#endif



#ifdef __cplusplus
}
#endif   /* __cplusplus */

#endif /* ifdef BX_OSDEP_H */
