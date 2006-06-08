/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.h,v 1.9 2006-06-08 17:02:51 vruppert Exp $
/////////////////////////////////////////////////////////////////////////

#include "config.h"

#if BX_USE_TEXTCONFIG && defined(WIN32)

int RuntimeOptionsDialog();
#if BX_SUPPORT_SAVE_RESTORE
bx_bool win32SaveState();
#endif
void win32_init_notify_callback();

#endif
