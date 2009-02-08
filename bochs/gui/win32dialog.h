/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.16 2009-02-08 09:05:52 vruppert Exp $
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

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400	// Force a minimum "OS level" for commctrl.h
#endif
#if _WIN32_IE < 0x0400
#undef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

extern "C" {
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <ctype.h>
}

#if BX_USE_TEXTCONFIG && defined(WIN32)

int RuntimeOptionsDialog();
#if BX_DEBUGGER && BX_DEBUGGER_GUI
void InitDebugDialog();
void ParseIDText (char *p);
void HitBreak();
#endif
void win32_init_notify_callback();

#endif

#endif // BX_WIN32_DIALOGS_H
