/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.5 2003-09-01 17:47:57 vruppert Exp $
/////////////////////////////////////////////////////////////////////////

#ifdef WIN32

extern "C" {
#include <windows.h>
}

HWND GetBochsWindow();
void LogAskDialog(BxEvent *event);
int AskFilename(HWND hwnd, bx_param_filename_c *param);
int AskString(bx_param_string_c *param);
int FloppyDialog(bx_param_filename_c *param);

#endif
