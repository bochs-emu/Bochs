/////////////////////////////////////////////////////////////////////////
//
// misc/niclist.c
// by Don Becker <x-odus@iname.com>
// $Id: niclist.c,v 1.6.2.1 2002-03-17 08:51:21 bdenney Exp $
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

#define MAX_ADAPTERS 10
#define	NIC_BUFFER_SIZE 2048


// declare our NT/9X structures to hold the adapter name and descriptions
typedef struct {
	LPWSTR	wstrName;
	LPSTR	strDesc;
} NIC_INFO_NT;

typedef struct {
	LPSTR	strName;
	LPSTR	strDesc;
} NIC_INFO_9X;

// declare an array of structures to hold our adapter information
NIC_INFO_NT niNT[MAX_ADAPTERS];
NIC_INFO_9X ni9X[MAX_ADAPTERS];

BOOLEAN   (*PacketGetAdapterNames)(PTSTR, PULONG) = NULL;


int main(int argc, char **argv)
{
	int i;
	HINSTANCE     hPacket;
	DWORD         dwVersion, dwMajorVersion;
	char			AdapterInfo[NIC_BUFFER_SIZE] = { '\0','\0' };
	unsigned long	AdapterLength = NIC_BUFFER_SIZE;
	LPWSTR			wstrName;
	LPSTR			strName, strDesc;
	int				nAdapterCount;


	// Attemp to load the WinpCap packet library
	hPacket = LoadLibrary("PACKET.DLL");
    if(hPacket)
	{
		// Now look up the address
		PacketGetAdapterNames = (BOOLEAN (*)(PTSTR, PULONG))GetProcAddress(hPacket, "PacketGetAdapterNames");
	}
	else {
		printf("Could not load WinPCap driver!\n");
		printf ("You can download them for free from\n");
		printf ("http://netgroup-serv.polito.it/winpcap\n");
		return 1;
	}

	dwVersion      = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

	// Get out blob of adapter info
	PacketGetAdapterNames(AdapterInfo,&AdapterLength);

	// If this is Windows NT ...
	if(!(dwVersion >= 0x80000000 && dwMajorVersion >= 4))
	{
		wstrName=(LPWSTR)AdapterInfo;

		// Obtain Names
		nAdapterCount = 0;
		while ((*wstrName))
		{
			// store pointer to name
			niNT[nAdapterCount].wstrName=wstrName;
			wstrName += lstrlenW(wstrName) +1;
			nAdapterCount++;
		}

		strDesc = (LPSTR)++wstrName;

		// Obtain descriptions ....
		for(i=0;i<nAdapterCount;i++)
		{
			// store pointer to description
			niNT[i].strDesc=strDesc;
			strDesc += lstrlen(strDesc) +1;

			// ... and display adapter info
			printf("\n%d: %s\n",i+1,niNT[i].strDesc);
			wprintf(L"     Device: %s",niNT[i].wstrName);
		}

		if(i)
		{
			printf("\n\nExample config for bochsrc:\n");
			wprintf(L"ne2k: ioaddr=0x300, irq=3, mac=b0:c4:20:00:00:00, ethmod=win32, ethdev=%s",niNT[0].wstrName);
			printf("\n");
		}

	}
	else
	{
		// Windows 9x
		strName=(LPSTR)AdapterInfo;

		// Obtain Names
		nAdapterCount = 0;
		while ((*strName))
		{
			// store pointer to name
			ni9X[nAdapterCount].strName=strName;
			strName += lstrlen(strName) +1;
			nAdapterCount++;
		}

		strDesc = (LPSTR)++strName;

		// Obtain descriptions ....
		for(i=0;i<nAdapterCount;i++)
		{
			// store pointer to description
			ni9X[i].strDesc=strDesc;
			strDesc += lstrlen(strDesc) +1;

			// ... and display adapter info
			printf("\n%d: %s\n",i+1,ni9X[i].strDesc);
			printf("     Device: %s",ni9X[i].strName);
		}

		if(i)
		{
			printf("\n\nExample config for bochsrc:\n");
			printf("ne2k: ioaddr=0x300, irq=3, mac=b0:c4:20:00:00:00, ethmod=win32, ethdev=%s",ni9X[0].strName);
			printf("\n");
		}

		printf("\n");
	}

	return 0;
}
