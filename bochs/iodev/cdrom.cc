/////////////////////////////////////////////////////////////////////////
// $Id: cdrom.cc,v 1.39 2002-08-28 16:45:18 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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

#define LOG_THIS /* no SMF tricks here, not needed */

extern "C" {
#include <errno.h>
}

#ifdef __linux__
extern "C" {
#include <sys/ioctl.h>
#include <linux/cdrom.h>
// I use the framesize in non OS specific code too
#define BX_CD_FRAMESIZE CD_FRAMESIZE
}
#endif

#ifdef __GNU__
extern "C" {
#include <sys/ioctl.h>
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048
}
#endif

#ifdef __sun
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/cdio.h>
#define BX_CD_FRAMESIZE CDROM_BLK_2048
}
#endif /* __sun */

#ifdef __BEOS__
#include "cdrom_beos.h"
#define BX_CD_FRAMESIZE 2048
#endif

#if (defined (__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__))
// OpenBSD pre version 2.7 may require extern "C" { } structure around
// all the includes, because the i386 sys/disklabel.h contains code which 
// c++ considers invalid.
#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/cdio.h>
#include <sys/ioctl.h>
#include <sys/disklabel.h>

// XXX
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
#endif

#ifdef WIN32
#include <windows.h>
#include <winioctl.h>
#include "aspi-win32.h"
#include "scsidefs.h"

DWORD (*GetASPI32SupportInfo)(void);
DWORD (*SendASPI32Command)(LPSRB);
BOOL  (*GetASPI32Buffer)(PASPI32BUFF);
BOOL  (*FreeASPI32Buffer)(PASPI32BUFF);
BOOL  (*TranslateASPI32Address)(PDWORD,PDWORD);
DWORD (*GetASPI32DLLVersion)(void);


static BOOL bUseASPI = FALSE;
static BOOL bHaveDev = FALSE;
static int hid = 0;
static int tid = 0;
static int lun = 0;

#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
HANDLE hFile = NULL;
#endif


#include <stdio.h>

#ifdef WIN32

int ReadCDSector(unsigned int hid, unsigned int tid, unsigned int lun, unsigned long frame, unsigned char *buf, int bufsize)
{
	HANDLE hEventSRB;
	SRB_ExecSCSICmd srb;
	DWORD dwStatus;

	hEventSRB = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	memset(&srb,0,sizeof(SRB_ExecSCSICmd));
	srb.SRB_Cmd        = SC_EXEC_SCSI_CMD;
	srb.SRB_HaId       = hid;
	srb.SRB_Target     = tid;
 	srb.SRB_Lun        = lun;
	srb.SRB_Flags      = SRB_DIR_IN | SRB_EVENT_NOTIFY;
	srb.SRB_SenseLen   = SENSE_LEN;
	srb.SRB_PostProc   = hEventSRB;
	srb.SRB_BufPointer = buf;
	srb.SRB_BufLen     = bufsize;
	srb.SRB_CDBLen     = 10;
	srb.CDBByte[0]     = SCSI_READ10;
	srb.CDBByte[2]     = frame>>24;
	srb.CDBByte[3]     = frame>>16;
	srb.CDBByte[4]     = frame>>8;
	srb.CDBByte[5]     = frame;
	srb.CDBByte[7]     = 0;
	srb.CDBByte[8]     = 1; /* read 1 frames */

	ResetEvent(hEventSRB);
	dwStatus = SendASPI32Command((SRB *)&srb);
	if(dwStatus == SS_PENDING) {
		WaitForSingleObject(hEventSRB, 100000);
	}
	CloseHandle(hEventSRB);
	return (srb.SRB_TargStat == STATUS_GOOD);
}

int GetCDCapacity(unsigned int hid, unsigned int tid, unsigned int lun)
{
	HANDLE hEventSRB;
	SRB_ExecSCSICmd srb;
	DWORD dwStatus;
	unsigned char buf[8];

	hEventSRB = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	memset(&buf, 0, sizeof(buf));
	memset(&srb,0,sizeof(SRB_ExecSCSICmd));
	srb.SRB_Cmd        = SC_EXEC_SCSI_CMD;
	srb.SRB_HaId       = hid;
	srb.SRB_Target     = tid;
 	srb.SRB_Lun        = lun;
	srb.SRB_Flags      = SRB_DIR_IN | SRB_EVENT_NOTIFY;
	srb.SRB_SenseLen   = SENSE_LEN;
	srb.SRB_PostProc   = hEventSRB;
	srb.SRB_BufPointer = (unsigned char *)buf;
	srb.SRB_BufLen     = 8;
	srb.SRB_CDBLen     = 10;
	srb.CDBByte[0]     = SCSI_READCDCAP;
	srb.CDBByte[2]     = 0;
	srb.CDBByte[3]     = 0;
	srb.CDBByte[4]     = 0;
	srb.CDBByte[5]     = 0;
	srb.CDBByte[8]     = 0;

	ResetEvent(hEventSRB);
	dwStatus = SendASPI32Command((SRB *)&srb);
	if(dwStatus == SS_PENDING) {
		WaitForSingleObject(hEventSRB, 100000);
	}

	CloseHandle(hEventSRB);
	return ((buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3]) * ((buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + buf[7]);
}

#endif

cdrom_interface::cdrom_interface(char *dev)
{
  put("CD");
  settype(CDLOG);
  fd = -1; // File descriptor not yet allocated

  if ( dev == NULL )
    path = NULL;
  else {
    path = strdup(dev);
  }
  using_file=0;
}

void
cdrom_interface::init(void) {
  BX_DEBUG(("Init $Id: cdrom.cc,v 1.39 2002-08-28 16:45:18 bdenney Exp $"));
  BX_INFO(("file = '%s'",path));
}

cdrom_interface::~cdrom_interface(void)
{
	if (fd >= 0)
		close(fd);
	if (path)
		free(path);
	BX_DEBUG(("Exit"));
}

  bool
cdrom_interface::insert_cdrom(char *dev)
{
  unsigned char buffer[BX_CD_FRAMESIZE];
  ssize_t ret;

  // Load CD-ROM. Returns false if CD is not ready.
  if (dev != NULL) path = strdup(dev);
  BX_INFO (("load cdrom with path=%s", path));
#ifdef WIN32
    char drive[256];
	OSVERSIONINFO osi;
    if ( (path[1] == ':') && (strlen(path) == 2) )
    {
	  osi.dwOSVersionInfoSize = sizeof(osi);
	  GetVersionEx(&osi);
	  if(osi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
	    // Use direct device access under windows NT/2k

        // With all the backslashes it's hard to see, but to open D: drive 
        // the name would be: \\.\d:
        sprintf(drive, "\\\\.\\%s", path);
        using_file = 0;
        BX_INFO (("Using direct access for cdrom."));
        // This trick only works for Win2k and WinNT, so warn the user of that.
	  } else {
		  BX_INFO(("Using ASPI for cdrom."));
          bUseASPI = TRUE;
	  }
    }
    else
    {
      strcpy(drive,path);
      using_file = 1;
	  bUseASPI = FALSE;
      BX_INFO (("Opening image file as a cd"));
    }
	if(bUseASPI) {
		DWORD d, cnt, max;
		int i, j, k;
		SRB_HAInquiry sh;
		SRB_GDEVBlock sd;
		HINSTANCE hASPI = LoadLibrary("WNASPI32.DLL");
		if(hASPI) {
            SendASPI32Command      = (DWORD(*)(LPSRB))GetProcAddress( hASPI, "SendASPI32Command" );
			GetASPI32DLLVersion    = (DWORD(*)(void))GetProcAddress( hASPI, "GetASPI32DLLVersion" );
			GetASPI32SupportInfo   = (DWORD(*)(void))GetProcAddress( hASPI, "GetASPI32SupportInfo" );
			BX_INFO(("Using first CDROM.  Please upgrade your ASPI drivers to version 4.01 or later if you wish to specify a cdrom driver."));
			
			d = GetASPI32SupportInfo();
			cnt = LOBYTE(LOWORD(d));
			for(i = 0; i < cnt; i++) {
				memset(&sh, 0, sizeof(sh));
				sh.SRB_Cmd  = SC_HA_INQUIRY;
				sh.SRB_HaId = i;
				SendASPI32Command((LPSRB)&sh);
				if(sh.SRB_Status != SS_COMP)
					continue;

				max = (int)sh.HA_Unique[3];
				for(j = 0; j < max; j++) {
					for(k = 0; k < 8; k++) {
						memset(&sd, 0, sizeof(sd));
						sd.SRB_Cmd    = SC_GET_DEV_TYPE;
						sd.SRB_HaId   = i;
						sd.SRB_Target = j;
						sd.SRB_Lun    = k;
						SendASPI32Command((LPSRB)&sd);
						if(sd.SRB_Status == SS_COMP) {
							if(sd.SRB_DeviceType == DTYPE_CDROM) {
								hid = i;
								tid = j;
								lun = k;
								bHaveDev = TRUE;
							}
						}
						if(bHaveDev) break;
					}
					if(bHaveDev) break;
				}

			}
		} else {
			BX_PANIC(("Could not load ASPI drivers, so cdrom access will fail"));
		}
		fd=1;
	} else {
	  BX_INFO(("Using direct access for CDROM"));
      hFile=CreateFile((char *)&drive,  GENERIC_READ, 0 , NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
      if (hFile !=(void *)0xFFFFFFFF)
        fd=1;
	}
#else
      // all platforms except win32
      fd = open(path, O_RDONLY);
#endif
    if (fd < 0) {
       BX_ERROR(( "open cd failed for %s: %s", path, strerror(errno)));
       return(false);
    }

  // I just see if I can read a sector to verify that a
  // CD is in the drive and readable.
#ifdef WIN32
	if(bUseASPI) {
	  return ReadCDSector(hid, tid, lun, 0, buffer, BX_CD_FRAMESIZE);
	} else {
      ReadFile(hFile, (void *) buffer, BX_CD_FRAMESIZE, (unsigned long *) &ret, NULL);
      if (ret < 0) {
         CloseHandle(hFile);
         fd = -1;
         BX_DEBUG(( "insert_cdrom: read returns error." ));
         return(false);
      }
	}
#else
    // do fstat to determine if it's a file or a device, then set using_file.
    struct stat stat_buf;
    ret = fstat (fd, &stat_buf);
    if (ret) {
      BX_PANIC (("fstat cdrom file returned error: %s", strerror (errno)));
    }
    if (S_ISREG (stat_buf.st_mode)) {
      using_file = 1;
      BX_INFO (("Opening image file %s as a cd.", path));
    } else {
      using_file = 0;
      BX_INFO (("Using direct access for cdrom."));
    }

    ret = read(fd, &buffer, BX_CD_FRAMESIZE);
    if (ret < 0) {
       close(fd);
       fd = -1;
       BX_DEBUG(( "insert_cdrom: read returns error: %s", strerror (errno) ));
       return(false);
        }
#endif
    return(true);
}


  void
cdrom_interface::eject_cdrom()
{
  // Logically eject the CD.  I suppose we could stick in
  // some ioctl() calls to really eject the CD as well.

  if (fd >= 0) {
#if (defined(__OpenBSD__) || defined(__FreeBSD__))
    (void) ioctl (fd, CDIOCALLOW);
    if (ioctl (fd, CDIOCEJECT) < 0)
	  BX_DEBUG(( "eject_cdrom: eject returns error." ));
#endif

#ifdef WIN32
if (using_file == 0)
{
	if(bUseASPI) {
	} else {
		DWORD lpBytesReturned;
		DeviceIoControl(hFile, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &lpBytesReturned, NULL);
	}
}
#endif

#if __linux__
  if (!using_file)
    ioctl (fd, CDROMEJECT, NULL);
#endif

    close(fd);
    fd = -1;
    }
}


  bool
cdrom_interface::read_toc(uint8* buf, int* length, bool msf, int start_track)
{
  // Read CD TOC. Returns false if start track is out of bounds.

  if (fd < 0) {
    BX_PANIC(("cdrom: read_toc: file not open."));
    }

#ifdef WIN32
  if (1) { // This is a hack and works okay if there's one rom track only
#else
  if (using_file) {
#endif
    if ((start_track != 1) && (start_track != 0xaa))
      return false;

    buf[2] = 1;
    buf[3] = 1;

    int len = 4;
    if (start_track == 1) {
      buf[len++] = 0; // Reserved
      buf[len++] = 0x14; // ADR, control
      buf[len++] = 1; // Track number
      buf[len++] = 0; // Reserved

      // Start address
      if (msf) {
        buf[len++] = 0; // reserved
        buf[len++] = 0; // minute
        buf[len++] = 2; // second
        buf[len++] = 0; // frame
      } else {
        buf[len++] = 0;
        buf[len++] = 0;
        buf[len++] = 0;
        buf[len++] = 0; // logical sector 0
      }
    }

    // Lead out track
    buf[len++] = 0; // Reserved
    buf[len++] = 0x16; // ADR, control
    buf[len++] = 0xaa; // Track number
    buf[len++] = 0; // Reserved

    uint32 blocks = capacity();

    // Start address
    if (msf) {
      buf[len++] = 0; // reserved
      buf[len++] = (uint8)(((blocks + 150) / 75) / 60); // minute
      buf[len++] = (uint8)(((blocks + 150) / 75) % 60); // second
      buf[len++] = (uint8)((blocks + 150) % 75); // frame;
    } else {
      buf[len++] = (blocks >> 24) & 0xff;
      buf[len++] = (blocks >> 16) & 0xff;
      buf[len++] = (blocks >> 8) & 0xff;
      buf[len++] = (blocks >> 0) & 0xff;
    }

    buf[0] = ((len-2) >> 8) & 0xff;
    buf[1] = (len-2) & 0xff;

    *length = len;

    return true;
  }
  // all these implementations below are the platform-dependent code required
  // to read the TOC from a physical cdrom.
#ifdef WIN32
  {
/*     #define IOCTL_CDROM_BASE                 FILE_DEVICE_CD_ROM
     #define IOCTL_CDROM_READ_TOC         CTL_CODE(IOCTL_CDROM_BASE, 0x0000, METHOD_BUFFERED, FILE_READ_ACCESS)
     unsigned long iBytesReturned;
     DeviceIoControl(hFile, IOCTL_CDROM_READ_TOC, NULL, 0, NULL, 0, &iBytesReturned, NULL);       */
    BX_ERROR (("WARNING: read_toc is not implemented, just returning length=1"));
    *length = 1;
    return true;
  }
#elif __linux__ || defined(__sun)
  {
  struct cdrom_tochdr tochdr;
  if (ioctl(fd, CDROMREADTOCHDR, &tochdr))
    BX_PANIC(("cdrom: read_toc: READTOCHDR failed."));

  if ((start_track > tochdr.cdth_trk1) && (start_track != 0xaa))
    return false;

  buf[2] = tochdr.cdth_trk0;
  buf[3] = tochdr.cdth_trk1;

  if (start_track < tochdr.cdth_trk0)
    start_track = tochdr.cdth_trk0;

  int len = 4;
  for (int i = start_track; i <= tochdr.cdth_trk1; i++) {
    struct cdrom_tocentry tocentry;
    tocentry.cdte_format = (msf) ? CDROM_MSF : CDROM_LBA;
    tocentry.cdte_track = i;
    if (ioctl(fd, CDROMREADTOCENTRY, &tocentry))
      BX_PANIC(("cdrom: read_toc: READTOCENTRY failed."));
    buf[len++] = 0; // Reserved
    buf[len++] = (tocentry.cdte_adr << 4) | tocentry.cdte_ctrl ; // ADR, control
    buf[len++] = i; // Track number
    buf[len++] = 0; // Reserved

    // Start address
    if (msf) {
      buf[len++] = 0; // reserved
      buf[len++] = tocentry.cdte_addr.msf.minute;
      buf[len++] = tocentry.cdte_addr.msf.second;
      buf[len++] = tocentry.cdte_addr.msf.frame;
    } else {
      buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 24) & 0xff;
      buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 16) & 0xff;
      buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 8) & 0xff;
      buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 0) & 0xff;
    }
  }

  // Lead out track
  struct cdrom_tocentry tocentry;
  tocentry.cdte_format = (msf) ? CDROM_MSF : CDROM_LBA;
#ifdef CDROM_LEADOUT
  tocentry.cdte_track = CDROM_LEADOUT;
#else
  tocentry.cdte_track = 0xaa;
#endif
  if (ioctl(fd, CDROMREADTOCENTRY, &tocentry))
    BX_PANIC(("cdrom: read_toc: READTOCENTRY lead-out failed."));
  buf[len++] = 0; // Reserved
  buf[len++] = (tocentry.cdte_adr << 4) | tocentry.cdte_ctrl ; // ADR, control
  buf[len++] = 0xaa; // Track number
  buf[len++] = 0; // Reserved

  // Start address
  if (msf) {
    buf[len++] = 0; // reserved
    buf[len++] = tocentry.cdte_addr.msf.minute;
    buf[len++] = tocentry.cdte_addr.msf.second;
    buf[len++] = tocentry.cdte_addr.msf.frame;
  } else {
    buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 24) & 0xff;
    buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 16) & 0xff;
    buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 8) & 0xff;
    buf[len++] = (((unsigned)tocentry.cdte_addr.lba) >> 0) & 0xff;
  }

  buf[0] = ((len-2) >> 8) & 0xff;
  buf[1] = (len-2) & 0xff;

  *length = len;

  return true;
  }
#elif (defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__))
  {
  struct ioc_toc_header h;
  struct ioc_read_toc_entry t;

  if (ioctl (fd, CDIOREADTOCHEADER, &h) < 0)
    BX_PANIC(("cdrom: read_toc: READTOCHDR failed."));

  if ((start_track > h.ending_track) && (start_track != 0xaa))
    return false;

  buf[2] = h.starting_track;
  buf[3] = h.ending_track;

  if (start_track < h.starting_track)
    start_track = h.starting_track;

  int len = 4;
  for (int i = start_track; i <= h.ending_track; i++) {
    struct cd_toc_entry tocentry;
    t.address_format = (msf) ? CD_MSF_FORMAT : CD_LBA_FORMAT;
    t.starting_track = i;
    t.data_len = sizeof(tocentry);
    t.data = &tocentry;

    if (ioctl (fd, CDIOREADTOCENTRYS, &tocentry) < 0)
      BX_PANIC(("cdrom: read_toc: READTOCENTRY failed."));

    buf[len++] = 0; // Reserved
    buf[len++] = (tocentry.addr_type << 4) | tocentry.control ; // ADR, control
    buf[len++] = i; // Track number
    buf[len++] = 0; // Reserved

    // Start address
    if (msf) {
      buf[len++] = 0; // reserved
      buf[len++] = tocentry.addr.msf.minute;
      buf[len++] = tocentry.addr.msf.second;
      buf[len++] = tocentry.addr.msf.frame;
    } else {
      buf[len++] = (((unsigned)tocentry.addr.lba) >> 24) & 0xff;
      buf[len++] = (((unsigned)tocentry.addr.lba) >> 16) & 0xff;
      buf[len++] = (((unsigned)tocentry.addr.lba) >> 8) & 0xff;
      buf[len++] = (((unsigned)tocentry.addr.lba) >> 0) & 0xff;
    }
  }

  // Lead out track
  struct cd_toc_entry tocentry;
  t.address_format = (msf) ? CD_MSF_FORMAT : CD_LBA_FORMAT;
  t.starting_track = 0xaa;
  t.data_len = sizeof(tocentry);
  t.data = &tocentry;

  if (ioctl (fd, CDIOREADTOCENTRYS, &tocentry) < 0)
    BX_PANIC(("cdrom: read_toc: READTOCENTRY lead-out failed."));

  buf[len++] = 0; // Reserved
  buf[len++] = (tocentry.addr_type << 4) | tocentry.control ; // ADR, control
  buf[len++] = 0xaa; // Track number
  buf[len++] = 0; // Reserved

  // Start address
  if (msf) {
    buf[len++] = 0; // reserved
    buf[len++] = tocentry.addr.msf.minute;
    buf[len++] = tocentry.addr.msf.second;
    buf[len++] = tocentry.addr.msf.frame;
  } else {
    buf[len++] = (((unsigned)tocentry.addr.lba) >> 24) & 0xff;
    buf[len++] = (((unsigned)tocentry.addr.lba) >> 16) & 0xff;
    buf[len++] = (((unsigned)tocentry.addr.lba) >> 8) & 0xff;
    buf[len++] = (((unsigned)tocentry.addr.lba) >> 0) & 0xff;
  }

  buf[0] = ((len-2) >> 8) & 0xff;
  buf[1] = (len-2) & 0xff;

  *length = len;

  return true;
  }
#else
  BX_INFO(("read_toc: your OS is not supported yet."));
  return(false); // OS not supported yet, return false always.
#endif
}


  uint32
cdrom_interface::capacity()
{
  // Return CD-ROM capacity.  I believe you want to return
  // the number of blocks of capacity the actual media has.

#if !defined WIN32
  // win32 has its own way of doing this
  if (using_file) {
    // return length of the image file
    struct stat stat_buf;
    int ret = fstat (fd, &stat_buf);
    if (ret) {
       BX_PANIC (("fstat on cdrom image returned err: %s", strerror(errno)));
    }
    BX_INFO (("cdrom size is %d bytes", stat_buf.st_size));
    if ((stat_buf.st_size % 2048) != 0)  {
      BX_ERROR (("expected cdrom image to be a multiple of 2048 bytes"));
    }
    return stat_buf.st_size / 2048;
  }
#endif

#ifdef __BEOS__
	return GetNumDeviceBlocks(fd, BX_CD_FRAMESIZE);
#elif defined(__sun)
  {
    struct stat buf = {0};

    if (fd < 0) {
      BX_PANIC(("cdrom: capacity: file not open."));
    } 
    
    if( fstat(fd, &buf) != 0 )
      BX_PANIC(("cdrom: capacity: stat() failed."));
  
    return(buf.st_size);
  }
#elif (defined(__NetBSD__) || defined(__OpenBSD__))
  {
  // We just read the disklabel, imagine that...
  struct disklabel lp;

  if (fd < 0)
    BX_PANIC(("cdrom: capacity: file not open."));

  if (ioctl(fd, DIOCGDINFO, &lp) < 0)
    BX_PANIC(("cdrom: ioctl(DIOCGDINFO) failed"));

  BX_DEBUG(( "capacity: %u", lp.d_secperunit ));
  return(lp.d_secperunit);
  }
#elif defined(__linux__)
  {
  // Read the TOC to get the data size, since BLKGETSIZE doesn't work on
  // non-ATAPI drives.  This is based on Keith Jones code below.
  // <splite@purdue.edu> 21 June 2001

  int i, dtrk_lba, num_sectors;
  int dtrk = 0;
  struct cdrom_tochdr td;
  struct cdrom_tocentry te;

  if (fd < 0)
    BX_PANIC(("cdrom: capacity: file not open."));

  if (ioctl(fd, CDROMREADTOCHDR, &td) < 0)
    BX_PANIC(("cdrom: ioctl(CDROMREADTOCHDR) failed"));

  num_sectors = -1;
  dtrk_lba = -1;

  for (i = td.cdth_trk0; i <= td.cdth_trk1; i++) {
    te.cdte_track = i;
    te.cdte_format = CDROM_LBA;
    if (ioctl(fd, CDROMREADTOCENTRY, &te) < 0)
      BX_PANIC(("cdrom: ioctl(CDROMREADTOCENTRY) failed"));

    if (dtrk_lba != -1) {
      num_sectors = te.cdte_addr.lba - dtrk_lba;
      break;
    }
    if (te.cdte_ctrl & CDROM_DATA_TRACK) {
      dtrk = i;
      dtrk_lba = te.cdte_addr.lba;
    }
  }

  if (num_sectors < 0) {
    if (dtrk_lba != -1) {
      te.cdte_track = CDROM_LEADOUT;
      te.cdte_format = CDROM_LBA;
      if (ioctl(fd, CDROMREADTOCENTRY, &te) < 0)
        BX_PANIC(("cdrom: ioctl(CDROMREADTOCENTRY) failed"));
      num_sectors = te.cdte_addr.lba - dtrk_lba;
    } else
      BX_PANIC(("cdrom: no data track found"));
  }

  BX_INFO(("cdrom: Data track %d, length %d", dtrk, num_sectors));

  return(num_sectors);

  }
#elif defined(__FreeBSD__)
  {
  // Read the TOC to get the data size, since disklabel doesn't appear
  // to work, sadly.
  // Keith Jones, 16 January 2000

#define MAX_TRACKS 100

  int i, num_tracks, num_sectors;
  struct ioc_toc_header td;
  struct ioc_read_toc_entry rte;
  struct cd_toc_entry toc_buffer[MAX_TRACKS + 1];

  if (fd < 0)
    BX_PANIC(("cdrom: capacity: file not open."));

  if (ioctl(fd, CDIOREADTOCHEADER, &td) < 0)
    BX_PANIC(("cdrom: ioctl(CDIOREADTOCHEADER) failed"));

  num_tracks = (td.ending_track - td.starting_track) + 1;
  if (num_tracks > MAX_TRACKS)
    BX_PANIC(("cdrom: TOC is too large"));

  rte.address_format = CD_LBA_FORMAT;
  rte.starting_track = td.starting_track;
  rte.data_len = (num_tracks + 1) * sizeof(struct cd_toc_entry);
  rte.data = toc_buffer;
  if (ioctl(fd, CDIOREADTOCENTRYS, &rte) < 0)
    BX_PANIC(("cdrom: ioctl(CDIOREADTOCENTRYS) failed"));

  num_sectors = -1;
  for (i = 0; i < num_tracks; i++) {
    if (rte.data[i].control & 4) {	/* data track */
      num_sectors = ntohl(rte.data[i + 1].addr.lba)
          - ntohl(rte.data[i].addr.lba);
      BX_INFO(( "cdrom: Data track %d, length %d",
        rte.data[i].track, num_sectors));
      break;
      }
    }

  if (num_sectors < 0)
    BX_PANIC(("cdrom: no data track found"));

  return(num_sectors);

  }
#elif defined WIN32
  {
	  if(bUseASPI) {
		  return (GetCDCapacity(hid, tid, lun) / 2352);
	  } else {
	    unsigned long FileSize;
		return (GetFileSize(hFile, &FileSize) / 2048);
	  }
  }
#else
  BX_ERROR(( "capacity: your OS is not supported yet." ));
  return(0);
#endif
}

  void
cdrom_interface::read_block(uint8* buf, int lba)
{
  // Read a single block from the CD

  off_t pos;
  ssize_t n;

#ifdef WIN32
  if(bUseASPI) {
	  ReadCDSector(hid, tid, lun, lba, buf, BX_CD_FRAMESIZE);
	  n = BX_CD_FRAMESIZE;
  } else {
    pos = SetFilePointer(hFile, lba*BX_CD_FRAMESIZE, NULL, SEEK_SET);
    if (pos == 0xffffffff) {
      BX_PANIC(("cdrom: read_block: lseek returned error."));
	}
	ReadFile(hFile, (void *) buf, BX_CD_FRAMESIZE, (unsigned long *) &n, NULL);
  }
#else
  pos = lseek(fd, lba*BX_CD_FRAMESIZE, SEEK_SET);
  if (pos < 0) {
    BX_PANIC(("cdrom: read_block: lseek returned error."));
  }
  n = read(fd, buf, BX_CD_FRAMESIZE);
#endif

  if (n != BX_CD_FRAMESIZE) {
    BX_PANIC(("cdrom: read_block: read returned %d",
      (int) n));
    }
}

