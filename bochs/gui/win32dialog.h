/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.6 2004-01-29 18:50:33 vruppert Exp $
/////////////////////////////////////////////////////////////////////////

#include "config.h"

#if BX_USE_TEXTCONFIG && defined(WIN32)

extern "C" {
#include <windows.h>
}

HWND GetBochsWindow();
void LogAskDialog(BxEvent *event);
int AskFilename(HWND hwnd, bx_param_filename_c *param);
int AskString(bx_param_string_c *param);
int FloppyDialog(bx_param_filename_c *param);

#endif
