/*
Copyright 2003 by David N. Welton <davidw@dedasys.com>.

This code may be distributed under the same terms as bochs.
*/

#include "bochs.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

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
	ioctl(consolefd, KIOCSOUND, 0);
	close(consolefd);
    }
}

void bx_console_c::beep_on(float frequency)
{
    if (access) {
	this->info("pc speaker on with frequency %f", frequency);
        ioctl(consolefd, KIOCSOUND, (int)(clock_tick_rate/frequency));
    }
}

void bx_console_c::beep_off()
{
    if (access) {
	ioctl(consolefd, KIOCSOUND, 0);
    }
}
