/////////////////////////////////////////////////////////////////////////
// $Id: cdrom_beos.h,v 1.2 2006-04-27 15:11:45 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////

#ifndef CDROM_BEOS_H
#define CDROM_BEOS_H

#include <stddef.h> //for off_t

off_t GetNumDeviceBlocks(int fd, int block_size);
int GetLogicalBlockSize(int fd);
int GetDeviceBlockSize(int fd);

#endif
