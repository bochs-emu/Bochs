/////////////////////////////////////////////////////////////////////////
// $Id: cdrom_amigaos.cc,v 1.3.4.1 2002-03-17 09:05:55 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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


// These are the low-level CDROM functions which are called
// from 'harddrv.cc'.  They effect the OS specific functionality
// needed by the CDROM emulation in 'harddrv.cc'.  Mostly, just
// ioctl() calls and such.  Should be fairly easy to add support
// for your OS if it is not supported yet.


#include "bochs.h"
#include "scsi_commands.h"

#include <exec/types.h>
#include <exec/memory.h>
#include <devices/trackdisk.h>
#include <devices/scsidisk.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>
#include <stdio.h>

#define LOG_THIS /* no SMF tricks here, not needed */

#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
#define SENSELEN 32
#define MAX_DATA_LEN 252

int amiga_cd_unit;
char amiga_cd_device[256];

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

int DoSCSI (UBYTE * data, int datasize, UBYTE * cmd, int cmdsize, UBYTE flags);

cdrom_interface::cdrom_interface(char *dev)
{
  char buf[256];

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

cdrom_interface::~cdrom_interface(void)
{
  if (cd_error == 0) {
    CloseDevice(CDIO);
  }
  if (CDIO != NULL) {
    DeleteIORequest(CDIO);
  }
  if (CDMP != NULL) {
    DeleteMsgPort(CDMP);
  }
}

  bool
cdrom_interface::insert_cdrom()
{
  uint8 cdb[6];
  uint8 buf[2*BX_CD_FRAMESIZE];
  uint8 i = 0;
  
  memset(cdb,0,sizeof(cdb));

  cdb[0] = SCSI_DA_START_STOP_UNIT;
  cdb[4] = 1 | 2;

  DoSCSI(0, 0,cdb,sizeof(cdb),SCSIF_READ);

  /*Check if there's a valid media present in the drive*/
  CDIO->iotd_Req.io_Data    = buf;
  CDIO->iotd_Req.io_Command = CMD_READ;
  CDIO->iotd_Req.io_Length  = BX_CD_FRAMESIZE;
  CDIO->iotd_Req.io_Offset  = BX_CD_FRAMESIZE;
  
  for(i = 0; i < 200; i++) /*it takes a while for the cdrom to validate*/
  {
  	DoIO((struct IORequest *)CDIO);
    if (CDIO->iotd_Req.io_Error == 0)
    	break;
    Delay (10);
  }

  if (CDIO->iotd_Req.io_Error != 0)
    return FALSE;
  else
  {
    //bx_options.cdromd.inserted = 1;
    return TRUE;
  }
}


  void
cdrom_interface::eject_cdrom()
{
  uint8 cdb[6];

  memset(cdb,0,sizeof(cdb));

  cdb[0] = SCSI_DA_START_STOP_UNIT;
  cdb[4] = 0 | 2;

  DoSCSI(0, 0,cdb,sizeof(cdb),SCSIF_READ);
  //bx_options.cdromd.inserted = 1;
}


  bool
cdrom_interface::read_toc(uint8* buf, int* length, bool msf, int start_track)
{
  uint8 cdb[10];
  TOC *toc;
  toc = (TOC*) buf;

  memset(cdb,0,sizeof(cdb));

  cdb[0] = SCSI_CD_READ_TOC;

  if(msf)
  	cdb[1] = 2;
  else
    cdb[1] = 0;

  cdb[6] = start_track;
  cdb[7] = sizeof(TOC)>>8;
  cdb[8] = sizeof(TOC)&0xFF;
  
  DoSCSI((UBYTE *)buf, sizeof(TOC), cdb, sizeof(cdb), SCSIF_READ);

  *length = toc->length + 4;

  return TRUE;
}


  uint32
cdrom_interface::capacity()
{
  CAPACITY cap;
  uint8 cdb[10];


  memset(cdb,0,sizeof(cdb));
  cdb[0] = SCSI_DA_READ_CAPACITY;

  int err;

  if ((err = DoSCSI ((UBYTE *)&cap, sizeof(cap),
			cdb, sizeof (cdb),
			(SCSIF_READ | SCSIF_AUTOSENSE))) == 0)

        return(cap.sectors);

  else
      BX_PANIC (("Couldn't get media capacity"));
}

  void
cdrom_interface::read_block(uint8* buf, int lba)
{
  CDIO->iotd_Req.io_Data    = buf;
  CDIO->iotd_Req.io_Command = CMD_READ;
  CDIO->iotd_Req.io_Length  = BX_CD_FRAMESIZE;
  CDIO->iotd_Req.io_Offset  = lba * BX_CD_FRAMESIZE;
  DoIO((struct IORequest *)CDIO);

  if (CDIO->iotd_Req.io_Error != 0) {
    BX_PANIC(("Error %d reading CD data sector: %ld\n", CDIO->iotd_Req.io_Error, lba));
  }
}

int DoSCSI(UBYTE *data, int datasize, uint8 *cmd,int cmdsize, UBYTE direction)
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
    //BX_PANIC(("DoSCSI: error %d\n", CDIO->iotd_Req.io_Error));
    //BX_PANIC(("SCSI Status: %d\n", scmd.scsi_Status));
  }

  return CDIO->iotd_Req.io_Error;
}
