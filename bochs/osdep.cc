/////////////////////////////////////////////////////////////////////////
// $Id: osdep.cc,v 1.4.4.2 2002-03-17 08:57:01 bdenney Exp $
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
// osdep.cc
// 
// Provide definition of library functions that are missing on various
// systems.  The only reason this is a .cc file rather than a .c file
// is so that it can include bochs.h.  Bochs.h includes all the required
// system headers, with appropriate #ifdefs for different compilers and 
// platforms.
//

#include "bochs.h"

//////////////////////////////////////////////////////////////////////
// Missing library functions.  These should work on any platform 
// that needs them.
//////////////////////////////////////////////////////////////////////

#if !BX_HAVE_SNPRINTF
/* XXX use real snprintf */
/* if they don't have snprintf, just use sprintf */
int bx_snprintf (char *s, size_t maxlen, const char *format, ...)
{
  va_list arg;
  int done;

  va_start (arg, format);
  done = vsprintf (s, format, arg);
  va_end (arg);

  return done;
}
#endif  /* !BX_HAVE_SNPRINTF */


#if (!BX_HAVE_STRTOULL && !BX_HAVE_STRTOUQ)
/* taken from glibc-2.2.2: strtod.c, and stripped down a lot.  There are 
   still a few leftover references to decimal points and exponents, 
   but it works for bases 10 and 16 */

#define RETURN(val,end)							      \
    do { if (endptr != NULL) *endptr = (char *) (end);		      \
	 return val; } while (0)

Bit64u
bx_strtoull (const char *nptr, char **endptr, int baseignore)
{
  int negative;			/* The sign of the number.  */
  int exponent;			/* Exponent of the number.  */

  /* Numbers starting `0X' or `0x' have to be processed with base 16.  */
  int base = 10;

  /* Number of bits currently in result value.  */
  int bits;

  /* Running pointer after the last character processed in the string.  */
  const char *cp, *tp;
  /* Start of significant part of the number.  */
  const char *startp, *start_of_digits;
  /* Total number of digit and number of digits in integer part.  */
  int dig_no;
  /* Contains the last character read.  */
  char c;

  Bit64s n = 0;
  char const *p;

  /* Prepare number representation.  */
  exponent = 0;
  negative = 0;
  bits = 0;

  /* Parse string to get maximal legal prefix.  We need the number of
     characters of the integer part, the fractional part and the exponent.  */
  cp = nptr - 1;
  /* Ignore leading white space.  */
  do
    c = *++cp;
  while (isspace (c));

  /* Get sign of the result.  */
  if (c == '-')
    {
      negative = 1;
      c = *++cp;
    }
  else if (c == '+')
    c = *++cp;

  if (c < '0' || c > '9')
    {
      /* It is really a text we do not recognize.  */
      RETURN (0, nptr);
    }

  /* First look whether we are faced with a hexadecimal number.  */
  if (c == '0' && tolower (cp[1]) == 'x')
    {
      /* Okay, it is a hexa-decimal number.  Remember this and skip
	 the characters.  BTW: hexadecimal numbers must not be
	 grouped.  */
      base = 16;
      cp += 2;
      c = *cp;
    }

  /* Record the start of the digits, in case we will check their grouping.  */
  start_of_digits = startp = cp;

  /* Ignore leading zeroes.  This helps us to avoid useless computations.  */
  while (c == '0')
    c = *++cp;

  /* If no other digit but a '0' is found the result is 0.0.
     Return current read pointer.  */
  if ((c < '0' || c > '9')
      && (base == 16 && (c < tolower ('a') || c > tolower ('f')))
      && (base == 16 && (cp == start_of_digits || tolower (c) != 'p'))
      && (base != 16 && tolower (c) != 'e'))
    {
      tp = start_of_digits;
      /* If TP is at the start of the digits, there was no correctly
	 grouped prefix of the string; so no number found.  */
      RETURN (0, tp == start_of_digits ? (base == 16 ? cp - 1 : nptr) : tp);
    }

  /* Remember first significant digit and read following characters until the
     decimal point, exponent character or any non-FP number character.  */
  startp = cp;
  dig_no = 0;
  while (1)
    {
      if ((c >= '0' && c <= '9')
	  || (base == 16 && tolower (c) >= 'a' && tolower (c) <= 'f'))
	++dig_no;
      else
	break;
      c = *++cp;
    }

  /* The whole string is parsed.  Store the address of the next character.  */
  if (endptr)
    *endptr = (char *) cp;

  if (dig_no == 0) 
    return 0;

  for (p=start_of_digits; p!=cp; p++) {
    n = n * (Bit64s)base;
    c = tolower (*p);
    c = (c >= 'a') ? (10+c-'a') : c-'0';
    n = n + (Bit64s)c;
    //printf ("after shifting in digit %c, n is %lld\n", *p, n);
  }
  return negative? -n : n;
}
#endif  /* !BX_HAVE_STRTOULL */

#if BX_TEST_STRTOULL_MAIN
/* test driver for strtoull.  Do not compile by default. */
int main (int argc, char **argv)
{
  char buf[256], *endbuf;
  long l;
  Bit64s ll;
  while (1) {
    printf ("Enter a long int: ");
    gets (buf);
    l = strtoul (buf, &endbuf, 10);
    printf ("As a long, %ld\n", l);
    printf ("Endbuf is at buf[%d]\n", endbuf-buf);
    ll = bx_strtoull (buf, &endbuf, 10);
    printf ("As a long long, %lld\n", ll);
    printf ("Endbuf is at buf[%d]\n", endbuf-buf);
  }
  return 0;
}
#endif  /* BX_TEST_STRTOULL_MAIN */

#if !BX_HAVE_STRDUP
/* XXX use real strdup */
char *bx_strdup(const char *str)
{
	char *temp;
	
	temp = malloc(strlen(str));
	sprintf(temp, "%s", str);
	return temp;
	
	// Well, I'm sure this isn't how strdup is REALLY implemented,
	// but it works...
}
#endif  /* !BX_HAVE_STRDUP */

//////////////////////////////////////////////////////////////////////
// Missing library functions, implemented for MacOS only
//////////////////////////////////////////////////////////////////////

#if BX_WITH_MACOS
// these functions are part of MacBochs.  They are not intended to be
// portable!
#include <Files.h>
#include <Disks.h>

int fd_read(char *buffer, Bit32u offset, Bit32u bytes)
{
	OSErr err;
	IOParam param;
	
	param.ioRefNum=-5; // Refnum of the floppy disk driver
	param.ioVRefNum=1;
	param.ioPosMode=fsFromStart;
	param.ioPosOffset=offset;
	param.ioBuffer=buffer;
	param.ioReqCount=bytes;
	err = PBReadSync((union ParamBlockRec *)(&param));
	return param.ioActCount;
}

int fd_write(char *buffer, Bit32u offset, Bit32u bytes)
{
	OSErr		err;
	IOParam	param;
	
	param.ioRefNum=-5; // Refnum of the floppy disk driver
	param.ioVRefNum=1;
	param.ioPosMode=fsFromStart;
	param.ioPosOffset=offset;
	param.ioBuffer=buffer;
	param.ioReqCount=bytes;
	err = PBWriteSync((union ParamBlockRec *)(&param));
	return param.ioActCount;
}

int fd_stat(struct stat *buf)
{
	OSErr		err;
	DrvSts	status;
	int			result;
	
	result = 0;
	err = DriveStatus(1, &status);
	if (status.diskInPlace <1 || status.diskInPlace > 2)
		result = -1;
	buf->st_mode = S_IFCHR;
	return result;
}
#endif /* BX_WITH_MACOS */
