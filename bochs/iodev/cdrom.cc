/////////////////////////////////////////////////////////////////////////
// $Id: cdrom.cc,v 1.56 2003-01-10 22:59:44 cbothamy Exp $
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


// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#if BX_SUPPORT_CDROM

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

#if defined(__GNU__) || (defined(__CYGWIN32__) && !defined(WIN32))
extern "C" {
#include <sys/ioctl.h>
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048
}
#endif

#if BX_WITH_MACOS
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048
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

#ifdef __DJGPP__
extern "C" {
#include <sys/ioctl.h>
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE 2048
}
#endif

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
// ntohl(x) et al have been moved out of sys/param.h in FreeBSD 5
#include <netinet/in.h>

// XXX
#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
#endif

#ifdef __APPLE__
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <dev/disk.h>
#include <errno.h>
#include <paths.h>
#include <sys/param.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/IOBSD.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDTypes.h>
#include <CoreFoundation/CoreFoundation.h>

// These definitions were taken from mount_cd9660.c
// There are some similar definitions in IOCDTypes.h
// however there seems to be some dissagreement in 
// the definition of CDTOC.length
struct _CDMSF {
	u_char   minute;
	u_char   second;
	u_char   frame;
};

#define MSF_TO_LBA(msf)		\
	(((((msf).minute * 60UL) + (msf).second) * 75UL) + (msf).frame - 150)

struct _CDTOC_Desc {
	u_char        session;
	u_char        ctrl_adr;  /* typed to be machine and compiler independent */
	u_char        tno;
	u_char        point;
	struct _CDMSF  address;
	u_char        zero;
	struct _CDMSF  p;
};

struct _CDTOC {
	u_short            length;  /* in native cpu endian */
	u_char             first_session;
	u_char             last_session;
	struct _CDTOC_Desc  trackdesc[1];
};

static kern_return_t FindEjectableCDMedia( io_iterator_t *mediaIterator, mach_port_t *masterPort );
static kern_return_t GetDeviceFilePath( io_iterator_t mediaIterator, char *deviceFilePath, CFIndex maxPathSize );
//int OpenDrive( const char *deviceFilePath );
static struct _CDTOC * ReadTOC( const char * devpath );

static char CDDevicePath[ MAXPATHLEN ];

#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
#endif

#ifdef WIN32
// windows.h included by bochs.h
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
static BOOL bHaveDev;
static UINT cdromCount = 0;
static HINSTANCE hASPI = NULL;

#define BX_CD_FRAMESIZE 2048
#define CD_FRAMESIZE	2048
#endif


#include <stdio.h>

#ifdef __APPLE__
static kern_return_t FindEjectableCDMedia( io_iterator_t *mediaIterator,
					   mach_port_t *masterPort )
{
  kern_return_t     kernResult;
  CFMutableDictionaryRef     classesToMatch;
  kernResult = IOMasterPort( bootstrap_port, masterPort );
  if ( kernResult != KERN_SUCCESS )
    {
      fprintf ( stderr, "IOMasterPort returned %d\n", kernResult );
      return kernResult;
    }
  // CD media are instances of class kIOCDMediaClass.
  classesToMatch = IOServiceMatching( kIOCDMediaClass );
  if ( classesToMatch == NULL )
    fprintf ( stderr, "IOServiceMatching returned a NULL dictionary.\n" );
  else
    {
      // Each IOMedia object has a property with key kIOMediaEjectable
      // which is true if the media is indeed ejectable. So add property
      // to CFDictionary for matching.
      CFDictionarySetValue( classesToMatch,
                            CFSTR( kIOMediaEjectable ), kCFBooleanTrue );
    }
  kernResult = IOServiceGetMatchingServices( *masterPort,
                                             classesToMatch, mediaIterator );
  if ( (kernResult != KERN_SUCCESS) || (*mediaIterator == NULL) ) 
    fprintf( stderr, "No ejectable CD media found.\n kernResult = %d\n", kernResult );

  return kernResult;
}


static kern_return_t GetDeviceFilePath( io_iterator_t mediaIterator,
					char *deviceFilePath, CFIndex maxPathSize )
{
  io_object_t nextMedia;
  kern_return_t kernResult = KERN_FAILURE;
  nextMedia = IOIteratorNext( mediaIterator );
  if ( nextMedia == NULL )
    {
      *deviceFilePath = '\0';
    }
  else
    {
      CFTypeRef    deviceFilePathAsCFString;
      deviceFilePathAsCFString = IORegistryEntryCreateCFProperty(
                                                                 nextMedia, CFSTR( kIOBSDName ),
                                                                 kCFAllocatorDefault, 0 );
      *deviceFilePath = '\0';
      if ( deviceFilePathAsCFString )
        {
          size_t devPathLength = strlen( _PATH_DEV );
          strcpy( deviceFilePath, _PATH_DEV );
          if ( CFStringGetCString( (const __CFString *) deviceFilePathAsCFString,
                                   deviceFilePath + devPathLength,
                                   maxPathSize - devPathLength,
                                   kCFStringEncodingASCII ) )
            {
              // fprintf( stderr, "BSD path: %s\n", deviceFilePath );
              kernResult = KERN_SUCCESS;
            }
          CFRelease( deviceFilePathAsCFString );
        }
    }
  IOObjectRelease( nextMedia );
  return kernResult;
}


static int OpenDrive( const char *deviceFilePath )
{

  int fileDescriptor;

  fileDescriptor = open( deviceFilePath, O_RDONLY );
  if ( fileDescriptor == -1 )
    fprintf( stderr, "Error %d opening device %s.\n", errno, deviceFilePath );
  return fileDescriptor;

}

static struct _CDTOC * ReadTOC( const char * devpath ) {

  struct _CDTOC * toc_p = NULL;
  io_iterator_t iterator = 0;
  io_registry_entry_t service = 0;
  CFDictionaryRef properties = 0;
  CFDataRef data = 0;
  mach_port_t port = 0;
  char * devname;
  
  if (( devname = strrchr( devpath, '/' )) != NULL ) {
    ++devname;
  }
  else {
    devname = (char *) devpath;
  }

  if ( IOMasterPort(bootstrap_port, &port ) != KERN_SUCCESS ) {
    fprintf( stderr, "IOMasterPort failed\n" );
    goto Exit;
  }

  if ( IOServiceGetMatchingServices( port, IOBSDNameMatching( port, 0, devname ),
				     &iterator ) != KERN_SUCCESS ) {
    fprintf( stderr, "IOServiceGetMatchingServices failed\n" );
    goto Exit;
  }
  
  service = IOIteratorNext( iterator );

  IOObjectRelease( iterator );

  iterator = 0;

  while ( service && !IOObjectConformsTo( service, "IOCDMedia" )) {
    if ( IORegistryEntryGetParentIterator( service, kIOServicePlane,
					   &iterator ) != KERN_SUCCESS ) {
      fprintf( stderr, "IORegistryEntryGetParentIterator failed\n" );
      goto Exit;
    }

    IOObjectRelease( service );
    service = IOIteratorNext( iterator );
    IOObjectRelease( iterator );

  }

  if ( service == NULL ) {
    fprintf( stderr, "CD media not found\n" );
    goto Exit;
  }

  if ( IORegistryEntryCreateCFProperties( service, (__CFDictionary **) &properties,
					  kCFAllocatorDefault,
					  kNilOptions ) != KERN_SUCCESS ) {
    fprintf( stderr, "IORegistryEntryGetParentIterator failed\n" );
    goto Exit;
  }

  data = (CFDataRef) CFDictionaryGetValue( properties, CFSTR("TOC") );
  if ( data == NULL ) {
    fprintf( stderr, "CFDictionaryGetValue failed\n" );
    goto Exit;
  }
  else {

    CFRange range;
    CFIndex buflen;

    buflen = CFDataGetLength( data ) + 1;
    range = CFRangeMake( 0, buflen );
    toc_p = (struct _CDTOC *) malloc( buflen );
    if ( toc_p == NULL ) {
      fprintf( stderr, "Out of memory\n" );
      goto Exit;
    }
    else {
      CFDataGetBytes( data, range, (unsigned char *) toc_p );
    }

    /*
    fprintf( stderr, "Table of contents\n length %d first %d last %d\n",
	    toc_p->length, toc_p->first_session, toc_p->last_session );
    */

    CFRelease( properties );

  }
  

 Exit:

  if ( service ) {
    IOObjectRelease( service );
  }

  return toc_p;

}
#endif

#ifdef WIN32

bool ReadCDSector(unsigned int hid, unsigned int tid, unsigned int lun, unsigned long frame, unsigned char *buf, int bufsize)
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
	srb.CDBByte[2]     = (unsigned char) (frame>>24);
	srb.CDBByte[3]     = (unsigned char) (frame>>16);
	srb.CDBByte[4]     = (unsigned char) (frame>>8);
	srb.CDBByte[5]     = (unsigned char) (frame);
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
  BX_DEBUG(("Init $Id: cdrom.cc,v 1.56 2003-01-10 22:59:44 cbothamy Exp $"));
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

  bx_bool
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
		  BX_INFO(("Using ASPI for cdrom. Drive letters are unused yet."));
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
		DWORD d;
		int cdr, cnt, max;
		int i, j, k;
		SRB_HAInquiry sh;
		SRB_GDEVBlock sd;
		if (!hASPI) {
		  hASPI = LoadLibrary("WNASPI32.DLL");
		}
		if(hASPI) {
            SendASPI32Command      = (DWORD(*)(LPSRB))GetProcAddress( hASPI, "SendASPI32Command" );
			GetASPI32DLLVersion    = (DWORD(*)(void))GetProcAddress( hASPI, "GetASPI32DLLVersion" );
			GetASPI32SupportInfo   = (DWORD(*)(void))GetProcAddress( hASPI, "GetASPI32SupportInfo" );
//			BX_INFO(("Using first CDROM.  Please upgrade your ASPI drivers to version 4.01 or later if you wish to specify a cdrom driver."));
			
			cdr = 0;
			bHaveDev = FALSE;
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
								cdr++;
								if(cdr > cdromCount) {
									hid = i;
									tid = j;
									lun = k;
									cdromCount++;
									bHaveDev = TRUE;
								}
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
      hFile=CreateFile((char *)&drive, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL); 
      if (hFile !=(void *)0xFFFFFFFF)
        fd=1;
	}
#elif defined(__APPLE__)
      if(strcmp(path, "drive") == 0)
      {
        mach_port_t masterPort = NULL;
        io_iterator_t mediaIterator;
        kern_return_t kernResult;
        
        BX_INFO(( "Insert CDROM" )); 
        
        kernResult = FindEjectableCDMedia( &mediaIterator, &masterPort );
        if ( kernResult != KERN_SUCCESS ) {
          BX_INFO (("Unable to find CDROM"));
          return false;
        }
        
        kernResult = GetDeviceFilePath( mediaIterator, CDDevicePath, sizeof( CDDevicePath ) );
        if ( kernResult != KERN_SUCCESS ) {
          BX_INFO (("Unable to get CDROM device file path" ));
          return false;
        }
	
        // Here a cdrom was found so see if we can read from it.
        // At this point a failure will result in panic.
        if ( strlen( CDDevicePath ) ) {
          fd = open(CDDevicePath, O_RDONLY);
        }
      }
      else
      {
        fd = open(path, O_RDONLY);
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

    ret = read(fd, (char*) &buffer, BX_CD_FRAMESIZE);
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


  bx_bool
cdrom_interface::read_toc(uint8* buf, int* length, bx_bool msf, int start_track)
{
  // Read CD TOC. Returns false if start track is out of bounds.

  if (fd < 0) {
    BX_PANIC(("cdrom: read_toc: file not open."));
    }

#if defined(WIN32)
  if (1) { // This is a hack and works okay if there's one rom track only
#else
  if (using_file) {
#endif
    // From atapi specs : start track can be 0-63, AA
    if ((start_track > 1) && (start_track != 0xaa)) 
      return false;

    buf[2] = 1;
    buf[3] = 1;

    int len = 4;
    if (start_track <= 1) {
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

    if (ioctl (fd, CDIOREADTOCENTRYS, &t) < 0)
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

  if (ioctl (fd, CDIOREADTOCENTRYS, &t) < 0)
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
#elif defined(__APPLE__)
  // Read CD TOC. Returns false if start track is out of bounds.

#if 1
  {
  struct _CDTOC * toc = ReadTOC( CDDevicePath );
  
  if ((start_track > toc->last_session) && (start_track != 0xaa))
    return false;

  buf[2] = toc->first_session;
  buf[3] = toc->last_session;

  if (start_track < toc->first_session)
    start_track = toc->first_session;

  int len = 4;
  for (int i = start_track; i <= toc->last_session; i++) {
    buf[len++] = 0; // Reserved
    buf[len++] = toc->trackdesc[i].ctrl_adr ; // ADR, control
    buf[len++] = i; // Track number
    buf[len++] = 0; // Reserved

    // Start address
    if (msf) {
      buf[len++] = 0; // reserved
      buf[len++] = toc->trackdesc[i].address.minute;
      buf[len++] = toc->trackdesc[i].address.second;
      buf[len++] = toc->trackdesc[i].address.frame;
    } else {
      unsigned lba = (unsigned)(MSF_TO_LBA(toc->trackdesc[i].address));
      buf[len++] = (lba >> 24) & 0xff;
      buf[len++] = (lba >> 16) & 0xff;
      buf[len++] = (lba >> 8) & 0xff;
      buf[len++] = (lba >> 0) & 0xff;
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
//  BX_INFO(( "Read TOC - Not Implemented" ));
//  return false;
  }
#else
  BX_INFO(( "Read TOC - Not Implemented" ));
  return false;
#endif
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
    BX_INFO (("cdrom size is %lu bytes", stat_buf.st_size));
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
  // Read the TOC to get the size of the data track.
  // Keith Jones <freebsd.dev@blueyonder.co.uk>, 16 January 2000

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
#elif defined __APPLE__
// Find the size of the first data track on the cd.  This has produced
// the same results as the linux version on every cd I have tried, about
// 5.  The differences here seem to be that the entries in the TOC when
// retrieved from the IOKit interface appear in a reversed order when
// compared with the linux READTOCENTRY ioctl.
  {
  // Return CD-ROM capacity.  I believe you want to return
  // the number of bytes of capacity the actual media has.

  BX_INFO(( "Capacity" ));

  struct _CDTOC * toc = ReadTOC( CDDevicePath );

  if ( toc == NULL ) {
    BX_PANIC(( "capacity: Failed to read toc" ));
  }

  size_t toc_entries = ( toc->length - 2 ) / sizeof( struct _CDTOC_Desc );
  
  BX_DEBUG(( "reading %d toc entries\n", toc_entries ));

  int start_sector = -1;
  int data_track = -1;

  // Iterate through the list backward. Pick the first data track and
  // get the address of the immediately previous (or following depending
  // on how you look at it).  The difference in the sector numbers
  // is returned as the sized of the data track.
  for ( int i=toc_entries - 1; i>=0; i-- ) {

    BX_DEBUG(( "session %d ctl_adr %d tno %d point %d lba %d z %d p lba %d\n",
             (int)toc->trackdesc[i].session,
             (int)toc->trackdesc[i].ctrl_adr,
             (int)toc->trackdesc[i].tno,
             (int)toc->trackdesc[i].point,
             MSF_TO_LBA( toc->trackdesc[i].address ),
             (int)toc->trackdesc[i].zero,
             MSF_TO_LBA(toc->trackdesc[i].p )));

    if ( start_sector != -1 ) {

      start_sector = MSF_TO_LBA(toc->trackdesc[i].p) - start_sector;
      break;

    }

    if (( toc->trackdesc[i].ctrl_adr >> 4) != 1 ) continue;

    if ( toc->trackdesc[i].ctrl_adr & 0x04 ) {

      data_track = toc->trackdesc[i].point;

      start_sector = MSF_TO_LBA(toc->trackdesc[i].p);

    }
      
  }  

  free( toc );

  if ( start_sector == -1 ) {
    start_sector = 0;
  }

  BX_INFO(("first data track %d data size is %d", data_track, start_sector));

  return start_sector;
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
#elif defined(__APPLE__)
#define CD_SEEK_DISTANCE kCDSectorSizeWhole
  if(using_file)
  {
    pos = lseek(fd, lba*BX_CD_FRAMESIZE, SEEK_SET);
    if (pos < 0) {
      BX_PANIC(("cdrom: read_block: lseek returned error."));
  }
  n = read(fd, buf, BX_CD_FRAMESIZE);
  }
  else
  {
    // This seek will leave us 16 bytes from the start of the data
    // hence the magic number.	
    pos = lseek(fd, lba*CD_SEEK_DISTANCE + 16, SEEK_SET);
    if (pos < 0) {
      BX_PANIC(("cdrom: read_block: lseek returned error."));
    }
    n = read(fd, buf, CD_FRAMESIZE);
  }
#else
  pos = lseek(fd, lba*BX_CD_FRAMESIZE, SEEK_SET);
  if (pos < 0) {
    BX_PANIC(("cdrom: read_block: lseek returned error."));
  }
  n = read(fd, (char*) buf, BX_CD_FRAMESIZE);
#endif

  if (n != BX_CD_FRAMESIZE) {
    BX_PANIC(("cdrom: read_block: read returned %d",
      (int) n));
    }
}

#endif /* if BX_SUPPORT_CDROM */
