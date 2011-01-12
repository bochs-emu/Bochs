/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.21 2011-01-12 18:18:48 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009  The Bochs Project
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

#ifndef BX_WIN32_DIALOGS_H
#define BX_WIN32_DIALOGS_H

#include "config.h"

#ifndef _WIN64

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400	/* Force a minimum "OS level" for commctrl.h */
#endif
#if _WIN32_IE < 0x0400
#undef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#else

#ifndef _WIN32_IE
#define _WIN32_IE 0x0500	/* WIN64 requirements are higher */
#endif
#if _WIN32_IE < 0x0500
#undef _WIN32_IE
#define _WIN32_IE 0x0500
#endif

#endif

extern "C" {
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <ctype.h>
}

// 64-bit windows env misses this define
#ifndef GWL_WNDPROC
#define GWL_WNDPROC (-4)
#endif

#if BX_USE_TEXTCONFIG && defined(WIN32)

#if BX_DEBUGGER && BX_DEBUGGER_GUI
void InitDebugDialog();
void ParseIDText(const char *p);
void HitBreak();
#endif
int init_win32_config_interface();

#endif

#endif // BX_WIN32_DIALOGS_H
