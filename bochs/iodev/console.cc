/////////////////////////////////////////////////////////////////////////
// $Id: console.cc,v 1.2 2004-01-16 18:18:38 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright 2003 by David N. Welton <davidw@dedasys.com>.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

#include "bochs.h"
#ifdef __linux__
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>
#endif

bx_console_c::bx_console_c() {
  this->put("CONSOLE");
  access = 0;
  consolefd = open("/dev/console", O_WRONLY);
  if (consolefd != -1) {
    access = 1;
    this->info("Opened /dev/console");
  } else {
    this->info("Failed to open /dev/console: %s", strerror(errno));
  }
}

bx_console_c::~bx_console_c() {
  if (access) {
#ifdef __linux__
    ioctl(consolefd, KIOCSOUND, 0);
    close(consolefd);
#endif
  }
}

void bx_console_c::beep_on(float frequency)
{
  if (access) {
#ifdef __linux__
    this->info("pc speaker on with frequency %f", frequency);
    ioctl(consolefd, KIOCSOUND, (int)(clock_tick_rate/frequency));
#endif
  }
}

void bx_console_c::beep_off()
{
  if (access) {
#ifdef __linux__
    ioctl(consolefd, KIOCSOUND, 0);
#endif
  }
}
