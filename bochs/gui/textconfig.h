/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2013  The Bochs Project
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

#include "config.h"

#if BX_USE_TEXTCONFIG

enum {
  BX_CI_INIT,
  BX_CI_START_MENU,
  BX_CI_START_OPTS,
  BX_CI_START_SIMULATION,
  BX_CI_RUNTIME,
  BX_CI_N_MENUS
};

enum {
  BX_CI_RT_FLOPPYA = 1,
  BX_CI_RT_FLOPPYB,
  BX_CI_RT_CDROM,
  BX_CI_RT_LOGOPTS1,
  BX_CI_RT_LOGOPTS2,
  BX_CI_RT_USB,
  BX_CI_RT_MISC,
  BX_CI_RT_SAVE_CFG,
  BX_CI_RT_CONT,
  BX_CI_RT_QUIT
};

int init_text_config_interface();

#endif
