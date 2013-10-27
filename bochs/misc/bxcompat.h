/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013       Volker Ruppert
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

#ifndef BX_COMPAT_H
#define BX_COMPAT_H

// copied from bochs.h
#ifdef WIN32
#  include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifndef _MSC_VER
#  include <unistd.h>
#else
#  include <io.h>
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// copied from siminterface.h
enum {
  BX_HDIMAGE_MODE_FLAT,
  BX_HDIMAGE_MODE_CONCAT,
  BX_HDIMAGE_MODE_EXTDISKSIM,
  BX_HDIMAGE_MODE_DLL_HD,
  BX_HDIMAGE_MODE_SPARSE,
  BX_HDIMAGE_MODE_VMWARE3,
  BX_HDIMAGE_MODE_VMWARE4,
  BX_HDIMAGE_MODE_UNDOABLE,
  BX_HDIMAGE_MODE_GROWING,
  BX_HDIMAGE_MODE_VOLATILE,
  BX_HDIMAGE_MODE_VVFAT,
  BX_HDIMAGE_MODE_VPC
};
#define BX_HDIMAGE_MODE_LAST     BX_HDIMAGE_MODE_VPC
#define BX_HDIMAGE_MODE_UNKNOWN  -1

extern const char *hdimage_mode_names[];

// definitions for compatibility with Bochs
#ifndef UNUSED
#  define UNUSED(x) ((void)x)
#endif

#define BX_DEBUG(x)
#define BX_INFO(x)  { if (bx_interactive) { (printf) x ; printf("\n"); } }
#define BX_ERROR(x) { (printf) x ; printf("\n"); }
#define BX_PANIC(x) { (printf) x ; printf("\n"); myexit(1); }
#define BX_ASSERT(x)

extern int bx_interactive;

class device_image_t;

void myexit(int code);
device_image_t* init_image(Bit8u image_mode);

#define DEV_hdimage_init_image(a,b,c) init_image(a)

#endif
