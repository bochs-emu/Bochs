/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.4 2003-07-15 21:02:05 vruppert Exp $
/////////////////////////////////////////////////////////////////////////

#ifdef WIN32

void LogAskDialog(BxEvent *event);
int AskFilename(bx_param_filename_c *param);
int AskString(bx_param_string_c *param);
int FloppyDialog(bx_param_filename_c *param);

#endif
