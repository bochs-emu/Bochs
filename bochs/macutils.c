// macutils.c
// (only required if you are building the MacOS port of Bochs)
//
// This file implements some functions which are required by
// MacBochs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <Files.h>
#include <Disks.h>
#include "macutils.h"

char *strdup(const char *str)
{
	char *temp;
	
	temp = malloc(strlen(str));
	sprintf(temp, "%s", str);
	return temp;
	
	// Well, I'm sure this isn't how strdup is REALLY implemented,
	// but it works...
}

int fd_read(char *buffer, Bit32u offset, Bit32u bytes)
{
	OSErr err;
	IOParam param;
	
	param.ioRefNum=-5; // Refnum of the floppy disk driver
	param.ioVRefNum=1;
	param.ioPosMode=fsFromStart;
	param.ioPosOffset=offset;
	param.ioBuffer=buffer;
	param.ioReqCount=bytes;
	err = PBReadSync((union ParamBlockRec *)(&param));
	return param.ioActCount;
}

int fd_write(char *buffer, Bit32u offset, Bit32u bytes)
{
	OSErr		err;
	IOParam	param;
	
	param.ioRefNum=-5; // Refnum of the floppy disk driver
	param.ioVRefNum=1;
	param.ioPosMode=fsFromStart;
	param.ioPosOffset=offset;
	param.ioBuffer=buffer;
	param.ioReqCount=bytes;
	err = PBWriteSync((union ParamBlockRec *)(&param));
	return param.ioActCount;
}

int fd_stat(struct stat *buf)
{
	OSErr		err;
	DrvSts	status;
	int			result;
	
	result = 0;
	err = DriveStatus(1, &status);
	if (status.diskInPlace <1 || status.diskInPlace > 2)
		result = -1;
	buf->st_mode = S_IFCHR;
	return result;
}