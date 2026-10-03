/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2026  The Bochs Project
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
// osdep.cc
//
// Provide definition of library functions that are missing on various
// systems.  The only reason this is a .cc file rather than a .c file
// is so that it can include bochs.h.  Bochs.h includes all the required
// system headers, with appropriate #ifdefs for different compilers and
// platforms.
//

#include "bochs.h"
#include "bxthread.h"

//////////////////////////////////////////////////////////////////////
// Missing library functions.  These should work on any platform
// that needs them.
//////////////////////////////////////////////////////////////////////

#if !BX_HAVE_STRDUP
/* XXX use real strdup */
char *bx_strdup(const char *s)
{
  char *p = malloc (strlen (s) + 1);   // allocate memory
  if (p != NULL)
      strcpy (p,s);                    // copy string
  return p;                            // return the memory
}
#endif  /* !BX_HAVE_STRDUP */

#if !BX_HAVE_STRREV
char *bx_strrev(char *str)
{
  char *p1, *p2;

  if (! str || ! *str)
    return str;

  for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
    *p1 ^= *p2;
    *p2 ^= *p1;
    *p1 ^= *p2;
  }
  return str;
}
#endif  /* !BX_HAVE_STRREV */

#if BX_WITH_MACOS
namespace std{extern "C" {char *mktemp(char *tpl);}}
#endif
#if !BX_HAVE_MKSTEMP
int bx_mkstemp(char *tpl)
{
  mktemp(tpl);
  return ::open(tpl, O_RDWR | O_CREAT | O_TRUNC
#  ifdef O_BINARY
            | O_BINARY
#  endif
              , S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP);
}
#endif // !BX_HAVE_MKSTEMP

//////////////////////////////////////////////////////////////////////
// Missing library functions, implemented for MacOS only
//////////////////////////////////////////////////////////////////////

#if BX_WITH_MACOS
// these functions are part of MacBochs.  They are not intended to be
// portable!
#include <Devices.h>
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
  OSErr   err;
  IOParam param;

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
  OSErr   err;
  DrvSts  status;
  int     result = 0;

  err = DriveStatus(1, &status);
  if (status.diskInPlace <1 || status.diskInPlace > 2)
    result = -1;
  buf->st_mode = S_IFCHR;
  return result;
}
#endif /* BX_WITH_MACOS */

//////////////////////////////////////////////////////////////////////
// Missing library functions, implemented for MorphOS only
//////////////////////////////////////////////////////////////////////

#ifdef __MORPHOS__
#include <stdio.h>
#include <time.h>
typedef unsigned int u_int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;

int fseeko(FILE *stream, off_t offset, int whence)
{
  while(offset != (long) offset)
  {
     long pos = (offset < 0) ? LONG_MIN : LONG_MAX;
     if(fseek(stream, pos, whence) != 0)
       return -1;
     offset -= pos;
     whence = SEEK_CUR;
  }
  return fseek(stream, (long) offset, whence);
}

struct tm *localtime_r(const time_t *timep, struct tm *result)
{
  struct tm *s = localtime(timep);
  if(s == NULL)
    return NULL;
  *result = *s;
  return(result);
}
#endif

//////////////////////////////////////////////////////////////////////
// New functions to replace library functions
//   with OS-independent versions
//////////////////////////////////////////////////////////////////////

#if BX_HAVE_REALTIME_USEC
#if defined(WIN32)
static LARGE_INTEGER realtime64_freq;

void bx_init_realtime64_usec(void)
{
  QueryPerformanceFrequency(&realtime64_freq);
}

Bit64u bx_get_realtime64_usec(void)
{
  LARGE_INTEGER ticks;
  QueryPerformanceCounter(&ticks);
  // Overflows approximately every month
  return ticks.QuadPart * 1000000LL / realtime64_freq.QuadPart;
}
#elif BX_HAVE_GETTIMEOFDAY
void bx_init_realtime64_usec(void)
{
}

Bit64u bx_get_realtime64_usec(void)
{
  timeval thetime;
  gettimeofday(&thetime,0);
  Bit64u mytime;
  mytime=(Bit64u)thetime.tv_sec*(Bit64u)1000000+(Bit64u)thetime.tv_usec;
  return mytime;
}
#endif
#endif

#ifdef WIN32
#include <timeapi.h>

static TIMECAPS time_dev_caps;

void bx_set_sys_timer_resolution(void)
{
  timeGetDevCaps(&time_dev_caps, sizeof(time_dev_caps));
  timeBeginPeriod(time_dev_caps.wPeriodMin); // 1ms usually
}

void bx_reset_sys_timer_resolution(void)
{
  timeEndPeriod(time_dev_caps.wPeriodMin);
}
#else
void bx_set_sys_timer_resolution(void)
{
}

void bx_reset_sys_timer_resolution(void)
{
}
#endif
