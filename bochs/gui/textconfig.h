/////////////////////////////////////////////////////////////////////////
// $Id: textconfig.h,v 1.7 2006-08-29 20:10:26 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//

#include "config.h"

#if BX_USE_TEXTCONFIG

enum {
  BX_CI_INIT,
  BX_CI_START_MENU,
  BX_CI_START_OPTS,
  BX_CI_START_OPTS_MEM,
  BX_CI_START_OPTS_INTERFACE,
  BX_CI_START_OPTS_DISK,
  BX_CI_START_OPTS_SOUND,
  BX_CI_START_OPTS_MISC,
  BX_CI_START_SIMULATION,
  BX_CI_RUNTIME,
  BX_CI_N_MENUS
};

enum {
  BX_CI_RT_FLOPPYA = 1,
  BX_CI_RT_FLOPPYB,
  BX_CI_RT_CDROM1,
  BX_CI_RT_CDROM2,
  BX_CI_RT_CDROM3,
  BX_CI_RT_CDROM4,
  BX_CI_RT_IPS,
  BX_CI_RT_LOGOPTS1,
  BX_CI_RT_LOGOPTS2,
  BX_CI_RT_INST_TR,
  BX_CI_RT_MISC,
  BX_CI_RT_CONT,
  BX_CI_RT_QUIT
};

int init_text_config_interface();

#endif
