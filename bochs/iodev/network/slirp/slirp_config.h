/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
/*
 * User definable configuration options
 */

/* Define to 1 if you want KEEPALIVE timers */
#define DO_KEEPALIVE 0

/*********************************************************/
/*
 * Autoconf defined configuration options
 * You shouldn't need to touch any of these
 */

/* Define if you have readv */
#undef HAVE_READV

/* Define if iovec needs to be declared */
#undef DECLARE_IOVEC
#ifdef _WIN32
#define DECLARE_IOVEC
#endif

/* Define if you have sys/select.h */
#undef HAVE_SYS_SELECT_H
#ifndef _WIN32
#define HAVE_SYS_SELECT_H
#endif

/* Define to whatever your compiler thinks inline should be */
//#define inline inline

/* Define to whatever your compiler thinks const should be */
//#define const const

/* Define to sizeof(char) */
#define SIZEOF_CHAR 1

/* Define to sizeof(short) */
#define SIZEOF_SHORT 2

/* Define to sizeof(int) */
#define SIZEOF_INT 4

/* Define to sizeof(char *) */
#define SIZEOF_CHAR_P SIZEOF_INT_P
