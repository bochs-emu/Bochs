/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.11 2006-11-17 16:50:39 vruppert Exp $
/////////////////////////////////////////////////////////////////////////

#include "config.h"

#if BX_USE_TEXTCONFIG && defined(WIN32)

int RuntimeOptionsDialog();
#if BX_DEBUGGER
void InitDebugDialog(HWND mainwnd);
#endif
void win32_init_notify_callback();

#endif
