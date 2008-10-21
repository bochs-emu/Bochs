/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.12 2008-10-21 13:45:03 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////

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
#if BX_DEBUGGER
void InitDebugDialog(HWND mainwnd);
void RefreshDebugDialog();
#endif
void win32_init_notify_callback();

#endif
