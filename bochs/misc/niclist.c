/////////////////////////////////////////////////////////////////////////
//
// misc/niclist.c
// by Don Becker <x-odus@iname.com>
// $Id: niclist.c,v 1.2 2001-10-31 16:01:29 bdenney Exp $
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
#include <wchar.h>

BOOLEAN   (*PacketGetAdapterNames) (PTSTR, PULONG);

int main(int argc, char **argv)
{
    int i = 0;
    wchar_t   AdapterNameW[512];
    char      AdapterNameA[512];  
    int       AdapterNum = 0;
    ULONG     AdapterLength;
    HINSTANCE hPacket;
    DWORD     dwVersion, dwMajorVersion;
    char      AdapterList[10][1024];
    wchar_t   *tempW, *tempW2;
    char      *tempA, *tempA2;
    char      buff[512];

    hPacket = LoadLibrary("PACKET.DLL");

	if(hPacket) {
          PacketGetAdapterNames = (BOOLEAN   (*)(PTSTR, PULONG))                   GetProcAddress(hPacket, "PacketGetAdapterNames");
      } else {
          printf("Could not load WinPCap driver!\n");
		  return 1;
     }

	dwVersion      = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if (!(dwVersion >= 0x80000000 && dwMajorVersion >= 4))
	{  // Windows NT
		PacketGetAdapterNames((char *)AdapterNameW,&AdapterLength);
		tempW  = AdapterNameW;
		tempW2 = AdapterNameW;
		while ((*tempW!='\0')||(*(tempW-1)!='\0'))
		{
			if (*tempW=='\0') 
			{
				memcpy(AdapterList[i],tempW2,(tempW-tempW2)*2);
				tempW2=tempW+1;
				i++;
		}
	
		tempW++;
		}
	  
		AdapterNum=i;
		for (i=0;i<AdapterNum;i++) {
#if defined(__CYGWIN__) || defined(__MINGW32__)
                        memset(&buff, 0, sizeof(buff));
                        wcstombs(buff, (wchar_t *)AdapterList[i], wcslen((wchar_t *)AdapterList[i]));
			printf("\n%d.) %s\n",i+1,buff);
#else
                        wprintf(L"\n%d.) %s\n",i+1,AdapterList[i]);
#endif
                }
		printf("\n");
		
	}

	else	//windows 95
	{
		PacketGetAdapterNames(AdapterNameA,&AdapterLength);
		tempA  = AdapterNameA;
		tempA2 = AdapterNameA;

		while ((*tempA!='\0')||(*(tempA-1)!='\0'))
		{
			if (*tempA=='\0') 
			{
				memcpy(AdapterList[i],tempA2,tempA-tempA2);
				tempA2=tempA+1;
				i++;
			}
			tempA++;
		}
		  
		AdapterNum=i;
		for (i=0;i<AdapterNum;i++)
			printf("\n%d.) %s\n",i+1,AdapterList[i]);
		printf("\n");

	}
	return 0;
}
