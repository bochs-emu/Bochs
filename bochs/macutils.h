// macutils.h
// (only required if you are building the MacOS port of Bochs)
//
// Bochs uses some common (but non-ANSI) functions which are not
// implemented in CodeWarrior for Mac (like strdup()). This file
// provides some substitutes which produce the same results.

#include "config.h"

char *strdup(const char *str);

int fd_read(char *buffer, Bit32u offset, Bit32u bytes);
int fd_write(char *buffer, Bit32u offset, Bit32u bytes);

// fd_read and fd_write are called by floppy.cc to access the Mac
// floppy drive directly, since the MacOS doesn't have "special"
// pathnames which map directly to IO devices

int fd_stat(struct stat *buf);
