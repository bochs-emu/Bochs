/////////////////////////////////////////////////////////////////////////
//
// misc/niclist.c
// by Don Becker <x-odus@iname.com>
// $Id: niclist.c,v 1.3 2001-10-31 16:03:23 bdenney Exp $
//
// This program is for win32 only.  It lists the network interface cards
// that you can use in the "ethdev" field of the ne2k line in your bochsrc.
//
// For this program and for win32 ethernet, the winpcap library is required.
// Download it from http://netgroup-serv.polito.it/winpcap.
//
/////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _Adapter {
	char description[256];
	char device[512];
} ADAPTER;

BOOLEAN   (*PacketGetAdapterNames)(PTSTR, PULONG) = NULL;

int HexDump(long start, unsigned char *buf, int len)
{
   int i;

   if( !buf )
   {
      return( 0 );
   }

   if( len == 0L )
   {
      return( 0 );
   }

   while( len > 0L )
   {
      printf( "%6.6X: ", start );
      for( i = 0; i < 8; ++i )
      {
         if( len - i > 0 )
         {
            printf( " %2.2X", buf[ i ] & 0xFF );
         }
         else
         {
            printf( "   " );
         }
      }

      printf( " :" );

      for( i = 8; i < 16; ++i )
      {
         if( len - i > 0 )
         {
            printf( " %2.2X", buf[ i ] & 0xFF );
         }
         else
         {
            printf( "   " );
         }
      }

      printf( "    " );

      for( i = 0; i < 16; ++i )
      {
         if( len - i > 0 )
         {
            if( isprint( buf[ i ] ) )
            {
               printf( "%c", buf[ i ] );
            }
            else
            {
               printf( "%c", '.' );
            }
         }
         else
         {
            printf( "%c", '.' );
         }
      }


      printf( "\n" );

      len -= 16;
      start += 16L;
      buf += 16;
   }

   return( 0 );
}

int main(int argc, char **argv)
{
	int i, n;
	char          AdapterInfo[1024];
	char          *device, *desc, *brk;
	unsigned long AdapterLength;
	ADAPTER       Adapters[20];
	HINSTANCE     hPacket;
	DWORD         dwVersion, dwMajorVersion;
	char          *split, *end;
	int           splitlen = 0;
	int           endlen = 0;

	hPacket = LoadLibrary("PACKET.DLL");
    if(hPacket) {
		PacketGetAdapterNames = (BOOLEAN (*)(PTSTR, PULONG))GetProcAddress(hPacket, "PacketGetAdapterNames");
	} else {
		printf("Could not load WinPCap driver!\n");
		return 1;
	}
	
	dwVersion      = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if(!(dwVersion >= 0x80000000 && dwMajorVersion >= 4)) {
		// Windows NT
		split    = "\0\0";
		splitlen = 2;
		end      = "\0\0\0\0";
		endlen   = 4;
	} else {
		// Windows 9x
		split    = "\0";
		splitlen = 1;
		end      = "\0\0";
		endlen   = 2;
	}

	PacketGetAdapterNames((char *)AdapterInfo, &AdapterLength);
	device = AdapterInfo;

	brk = device;
	i = 0;
	while(memcmp(brk, end, endlen) != 0) {
		while(memcmp(brk, split, splitlen) != 0) {
			strncat(Adapters[i].device, brk, 1);
			brk += splitlen;
		}
		i++;
		if(memcmp(brk, end, endlen) == 0) break;
		brk += splitlen;
	}
	n = i;
	brk += endlen;
	desc = brk;

	i = 0;
	brk = desc;
	while(*brk != '\0' && *(brk+1) != '\0') {
		while(*brk != '\0') brk++;
		brk++;
		strcpy(Adapters[i].description, desc);
		i++;
		desc = brk;
	}
	n = i;

	for(i = 0; i < n; i++) {
		printf("%s\n  %s\n\n", Adapters[i].description, Adapters[i].device);
	}
	return 0;
}
