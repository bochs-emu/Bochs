/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000-2013  The Bochs Project
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
//

// These are the low-level CDROM functions which are called
// from 'harddrv.cc'.  They effect the OS specific functionality
// needed by the CDROM emulation in 'harddrv.cc'.  Mostly, just
// ioctl() calls and such.  Should be fairly easy to add support
// for your OS if it is not supported yet.


#include "bochs.h"
#if BX_SUPPORT_CDROM

#include "scsi_commands.h"
#include "cdrom.h"
#include "cdrom_amigaos.h"

#include <exec/types.h>
#include <exec/memory.h>
#include <devices/trackdisk.h>
#include <devices/scsidisk.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>
#include <stdio.h>

#if BX_WITH_AMIGAOS
#include <dos/dos.h>
#endif

#define LOG_THIS /* no SMF tricks here, not needed */

#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
#define SENSELEN 32
#define MAX_DATA_LEN 252

int amiga_cd_unit;
char amiga_cd_device[256];

int oldpos = 0;

struct MsgPort *CDMP; /* Pointer for message port */
struct IOExtTD *CDIO; /* Pointer for IORequest */
int cd_error;

typedef struct {
	UBYTE pad0;
	UBYTE trackType;
	UBYTE trackNum;
	UBYTE pad1;
	ULONG startFrame;
} TOCENTRY;

typedef struct {
	UWORD    length;
	UBYTE    firstTrack;
	UBYTE    lastTrack;
	TOCENTRY tocs[100];
} TOC;

typedef struct {
	ULONG sectors;
	ULONG blocksize;
} CAPACITY;

unsigned char sensebuf[SENSELEN];

int DoSCSI(UBYTE * data, int datasize, UBYTE * cmd, int cmdsize, UBYTE flags);

cdrom_amigaos_c::cdrom_amigaos_c(const char *dev)
{
  char buf[256], prefix[6];

  sprintf(prefix, "CD%d", ++bx_cdrom_count);
  put(prefix);
  fd = -1;
  fda = NULL;

  if (dev == NULL) {
    path = NULL;
  } else {
    path = strdup(dev);
  }

  if (strstr(path, ".device ") == 0)
  {
    BX_INFO(("Amiga: Using CD ROM as Image"));
    using_file=1;

    if (fda) Close(fda);

    fda = Open(path, MODE_OLDFILE);
  }
  else
  {
    using_file=0;

    sscanf(dev, "%s%s", amiga_cd_device, buf);
    amiga_cd_unit = atoi(buf);

    CDMP = CreateMsgPort();
    if (CDMP != NULL) {
      CDIO = (struct IOExtTD *)CreateIORequest(CDMP, sizeof(struct IOExtTD));
      if (CDIO != NULL) {
        cd_error = OpenDevice(amiga_cd_device, amiga_cd_unit, (struct IORequest *)CDIO, 0);
        if (cd_error != 0)
          BX_PANIC(("CD_Open: could not open device %s unit %d\n", amiga_cd_device, amiga_cd_unit));
      }
    }
  }
}

cdrom_amigaos_c::~cdrom_amigaos_c(void)
{
  if(fda)
    Close(fda);
  fda = NULL;

  if (cd_error == 0) {
    CloseDevice((struct IORequest *)CDIO);
  }
  if (CDIO != NULL) {
    DeleteIORequest((struct IORequest *)CDIO);
  }
  if (CDMP != NULL) {
    DeleteMsgPort(CDMP);
  }
}

bx_bool cdrom_amigaos_c::insert_cdrom(const char *dev)
{
  Bit8u cdb[6];
  Bit8u buf[2*BX_CD_FRAMESIZE];
  Bit8u i = 0;

  BX_INFO(("dev = %s", dev));
  if (dev != NULL) path = strdup(dev);


  if (strstr(path, ".device ") == 0)
  {
    BX_INFO(("Amiga: Using CD ROM as Image"));
    using_file=1;

    if(fda) Close(fda);

    fda = Open(path, MODE_OLDFILE);
    if(fda){
      return true;
    }
    else return false;
  }
  else
  {
    using_file=0;

    BX_INFO(("Amiga: Using CD ROM as physical drive"));

    memset(cdb,0,sizeof(cdb));

    cdb[0] = SCSI_DA_START_STOP_UNIT;
    cdb[4] = 1 | 2;

    DoSCSI(0, 0,cdb,sizeof(cdb),SCSIF_READ);

    /*Check if there's a valid media present in the drive*/
    CDIO->iotd_Req.io_Data    = buf;
    CDIO->iotd_Req.io_Command = CMD_READ;
    CDIO->iotd_Req.io_Length  = BX_CD_FRAMESIZE;
    CDIO->iotd_Req.io_Offset  = BX_CD_FRAMESIZE;

    if (CDIO->iotd_Req.io_Error != 0)
      return false;
    else
      return true;
  }
}


void cdrom_amigaos_c::eject_cdrom()
{
  Bit8u cdb[6];

  if(fda)
  {
    Close(fda);
    fda = NULL;
  }
  else
  {
    memset(cdb,0,sizeof(cdb));

    cdb[0] = SCSI_DA_START_STOP_UNIT;
    cdb[4] = 0 | 2;

    DoSCSI(0, 0,cdb,sizeof(cdb),SCSIF_READ);
  }
}


bx_bool cdrom_amigaos_c::read_toc(Bit8u* buf, int* length, bx_bool msf, int start_track, int format)
{
  Bit8u cdb[10];
  TOC *toc;
  toc = (TOC*) buf;

  if (format != 0)
    return false;

  memset(cdb,0,sizeof(cdb));

  if(using_file){
    return create_toc(buf, length, msf, start_track, format);
  }
  else
  {

    cdb[0] = SCSI_CD_READ_TOC;

    if (msf)
      cdb[1] = 2;
    else
      cdb[1] = 0;

    cdb[6] = start_track;
    cdb[7] = sizeof(TOC)>>8;
    cdb[8] = sizeof(TOC)&0xFF;

    DoSCSI((UBYTE *)buf, sizeof(TOC), cdb, sizeof(cdb), SCSIF_READ);

    *length = toc->length + 4;

    return true;
  }
}


Bit32u cdrom_amigaos_c::capacity()
{
  CAPACITY cap;
  Bit8u cdb[10];

  if (using_file) {

    int len;

    Seek(fda, 0, OFFSET_END);
    len = Seek(fda, 0, OFFSET_BEGINNING);
    if ((len % 2048) != 0)  {
      BX_ERROR (("expected cdrom image to be a multiple of 2048 bytes"));
    }
    return (len/2048);

  }
  else
  {
    memset(cdb,0,sizeof(cdb));
    cdb[0] = SCSI_DA_READ_CAPACITY;

    int err;

    if ((err = DoSCSI((UBYTE *)&cap, sizeof(cap),
                      cdb, sizeof (cdb),
                      (SCSIF_READ | SCSIF_AUTOSENSE))) == 0)
      return(cap.sectors);
    else
      BX_PANIC (("Couldn't get media capacity"));
  }
}

bx_bool cdrom_amigaos_c::read_block(Bit8u* buf, Bit32u lba, int blocksize)
{
  int n;
  Bit8u try_count = 3;

  if (using_file)
  {
    do
    {
     if(oldpos != lba*BX_CD_FRAMESIZE)
       Seek(fda, lba*BX_CD_FRAMESIZE, OFFSET_BEGINNING);
     oldpos = lba*BX_CD_FRAMESIZE+BX_CD_FRAMESIZE;
     n = Read(fda, buf, BX_CD_FRAMESIZE);
    } while ((n != BX_CD_FRAMESIZE) && (--try_count > 0));
    if(try_count <=0)
    {
      BX_PANIC(("Error reading CD image sector: %ld", lba));
      return(0);
    }
    return 1;
  }
  else
  {
    CDIO->iotd_Req.io_Data    = buf;
    CDIO->iotd_Req.io_Command = CMD_READ;
    CDIO->iotd_Req.io_Length  = BX_CD_FRAMESIZE;
    CDIO->iotd_Req.io_Offset  = lba * BX_CD_FRAMESIZE;
    DoIO((struct IORequest *)CDIO);

    if (CDIO->iotd_Req.io_Error != 0) {
      BX_PANIC(("Error %d reading CD data sector: %ld", CDIO->iotd_Req.io_Error, lba));
      return 0;
    }
    return 1;
  }
}


int DoSCSI(UBYTE *data, int datasize, Bit8u *cmd,int cmdsize, UBYTE direction)
{
  struct SCSICmd scmd;

  CDIO->iotd_Req.io_Command = HD_SCSICMD;
  CDIO->iotd_Req.io_Data    = &scmd;
  CDIO->iotd_Req.io_Length  = sizeof(scmd);

  scmd.scsi_Data        = (UWORD *)data;
  scmd.scsi_Length      = datasize;
  scmd.scsi_SenseActual = 0;
  scmd.scsi_SenseData   = sensebuf;
  scmd.scsi_SenseLength = SENSELEN;
  scmd.scsi_Command     = cmd;
  scmd.scsi_CmdLength   = cmdsize;
  scmd.scsi_Flags       = SCSIF_AUTOSENSE | direction;

  DoIO((struct IORequest *)CDIO);

  if (CDIO->iotd_Req.io_Error != 0) {
  //  BX_PANIC(("DoSCSI: error %d", CDIO->iotd_Req.io_Error));
  }

  return CDIO->iotd_Req.io_Error;
}

#endif /* if BX_SUPPORT_CDROM */
