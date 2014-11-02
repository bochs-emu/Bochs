/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2014  The Bochs Project
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

// Compilation for WIN64 native require minimum 0x0500
#ifndef _WIN32_IE
#define _WIN32_IE 0x0500  /* Force a minimum "OS level" for commctrl.h */
#endif
#if _WIN32_IE < 0x0500
#undef _WIN32_IE
#define _WIN32_IE 0x0500
#endif

extern "C" {
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <ctype.h>
}

#if BX_USE_WIN32CONFIG

#if BX_DEBUGGER && BX_DEBUGGER_GUI
void InitDebugDialog();
void ParseIDText(const char *p);
void HitBreak();
#endif
int init_win32_config_interface();

#endif

#endif // BX_WIN32_DIALOGS_H
