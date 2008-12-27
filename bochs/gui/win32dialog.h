/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.13 2008-12-27 12:06:39 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////

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
#if BX_DEBUGGER
void InitDebugDialog(void* mainwnd);
void RefreshDebugDialog();
#endif
void win32_init_notify_callback();

#endif

#endif // BX_WIN32_DIALOGS_H
