/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  util-nexus.c: convenience routines which can be accessed from
 *    either space.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "plex86.h"
/* These functions are available from either space. */
#define IN_MONITOR_SPACE
#define IN_HOST_SPACE
#include "monitor.h"



  void
mon_memzero(void *ptr, int size)
{
    char *p = ptr;
    while (size--)
        *p++ = 0;
}

  void
mon_memcpy(void *dst, void *src, int size)
{
    char *d = dst;
    char *s = src;
    while (size--)
        *d++ = *s++;
}

  void *
mon_memset(void *dst, unsigned c, unsigned n)
{
  unsigned char *d = dst;
  while (n--) {
    *d++ = c;
    }
  return(dst);
}
