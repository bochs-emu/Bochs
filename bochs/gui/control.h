/////////////////////////////////////////////////////////////////////////
// $Id: control.h,v 1.8 2002-10-24 21:06:19 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
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

int init_text_config_interface ();
