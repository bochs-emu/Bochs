//  Copyright (C) 2000  Psyon.Org!
//
//    Donald Becker
//    http://www.psyon.org
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

#include "bochs.h"
#include "icon_bochs.h"
#include "font/vga.bitmap.h"
#include <windows.h>
#include <winsock.h>
#include <process.h>
#include "rfb.h"
#include "rfbproto.h"

static BOOL    keep_alive;

static unsigned short rfbPort = 5900;

// Headerbar stuff
unsigned rfbBitmapCount = 0;
struct {
	char     *bmap;
	unsigned xdim;
	unsigned ydim;
} rfbBitmaps[BX_MAX_PIXMAPS];

unsigned rfbHeaderbarBitmapCount = 0;
struct {
	unsigned index;
	unsigned xorigin;
	unsigned yorigin;
	unsigned alignment;
	void (*f)(void);
} rfbHeaderbarBitmaps[BX_MAX_HEADERBAR_ENTRIES];

// Misc stuff
static char  *rfbScreen;
static char  rfbPallet[256];

static long  rfbDimensionX, rfbDimensionY;
static long  rfbStretchedX, rfbStretchedY;
static long  rfbHeaderbarY;
static long  rfbTileX = 0;
static long  rfbTileY = 0;
static unsigned long  rfbCursorX = 0;
static unsigned long  rfbCursorY = 0;
static unsigned long  rfbOriginLeft  = 0;
static unsigned long  rfbOriginRight = 0;

static unsigned long ServerThread   = NULL;
static DWORD         ServerThreadID = 0;

static SOCKET sGlobal;

void ServerThreadInit(PVOID indata);
void HandleRfbClient(SOCKET sClient);
ReadExact(int sock, char *buf, int len);
WriteExact(int sock, char *buf, int len);
void DrawBitmap(int x, int y, int width, int height, char *bmap, char color, bool update_client);
void UpdateScreen(char *newBits, int x, int y, int width, int height, bool update_client);
void rfbKeyPressed(Bit32u key, int press_release);
void DrawColorPallet();

static const rfbPixelFormat BGR233Format = {
    8, 8, 1, 1, 7, 7, 3, 0, 3, 6
};

// Set this for the endian of your machine. 0 = big, 1 = little 
static const int rfbEndianTest = 1;

#define Swap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))
#define Swap32(l) (((l) >> 24) | (((l) & 0x00ff0000) >> 8)  | (((l) & 0x0000ff00) << 8)  | ((l) << 24))
#define Swap16IfLE(s) (*(const char *)&rfbEndianTest ? Swap16(s) : (s))
#define Swap32IfLE(l) (*(const char *)&rfbEndianTest ? Swap32(l) : (l))
#define PF_EQ(x,y) ((x.bitsPerPixel == y.bitsPerPixel) && (x.depth == y.depth) && (x.trueColour == y.trueColour) &&	((x.bigEndian == y.bigEndian) || (x.bitsPerPixel == 8)) && (!x.trueColour || ((x.redMax == y.redMax) &&	(x.greenMax == y.greenMax) && (x.blueMax == y.blueMax) && (x.redShift == y.redShift) && (x.greenShift == y.greenShift) && (x.blueShift == y.blueShift))))

// This file defines stubs for the GUI interface, which is a
// place to start if you want to port bochs to a platform, for
// which there is no support for your native GUI, or if you want to compile
// bochs without any native GUI support (no output window or
// keyboard input will be possible).
// Look in 'x.cc', 'beos.cc', and 'win32.cc' for specific
// implementations of this interface.  -Kevin


// ::SPECIFIC_INIT()
//
// Called from gui.cc, once upon program startup, to allow for the
// specific GUI code (X11, BeOS, ...) to be initialized.
//
// th: a 'this' pointer to the gui class.  If a function external to the
//     class needs access, store this pointer and use later.
// argc, argv: not used right now, but the intention is to pass native GUI
//     specific options from the command line.  (X11 options, BeOS options,...)
//
// tilewidth, tileheight: for optimization, graphics_tile_update() passes
//     only updated regions of the screen to the gui code to be redrawn.
//     These define the dimensions of a region (tile).
// headerbar_y:  A headerbar (toolbar) is display on the top of the
//     VGA window, showing floppy status, and other information.  It
//     always assumes the width of the current VGA mode width, but
//     it's height is defined by this parameter.

void bx_gui_c::specific_init(bx_gui_c *th, int argc, char **argv, unsigned tilewidth, unsigned tileheight, unsigned headerbar_y)
{
	rfbHeaderbarY = headerbar_y;
	rfbDimensionX = 640;
	rfbDimensionY = 480 + rfbHeaderbarY;
	rfbStretchedX = rfbDimensionX;
	rfbStretchedY = rfbDimensionY;
	rfbTileX      = tilewidth;
	rfbTileY      = tileheight;

	rfbScreen = (char *)malloc(rfbDimensionX * rfbDimensionY); 
	memset(&rfbPallet, 0, sizeof(rfbPallet));
	rfbPallet[63] = (char)0xFF;
	
	keep_alive = TRUE;
	ServerThread = _beginthread(ServerThreadInit, 0, NULL);
	
	SetLastError(0);
	Sleep(1000);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	if (bx_options.private_colormap) {
		fprintf(stderr, "# WARNING: RFB: private_colormap option ignored.\n");
	}
}

BOOL InitWinsock()
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(1,1), &wsaData) != 0) return FALSE;
	return TRUE;
}

BOOL StopWinsock()
{
	WSACleanup();
	return TRUE;
}

void ServerThreadInit(PVOID indata) 
{
	SOCKET      sServer;
	SOCKET      sClient;
	SOCKADDR_IN sai;
	int         sai_size;

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);

	if(!InitWinsock()) {
		fprintf(stderr, "# ERROR: RFB: could not initialize winsock.\n");
		goto end_of_thread;
	}

	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sServer == INVALID_SOCKET) { 
		fprintf(stderr, "# ERROR: RFB: could not create socket.\n");
		goto end_of_thread;
	}
	sai.sin_addr.s_addr = INADDR_ANY;
	sai.sin_family      = AF_INET;
	sai.sin_port        = htons(rfbPort);
	if(bind(sServer, (LPSOCKADDR)&sai, sizeof(sai)) == SOCKET_ERROR) {
		fprintf(stderr, "# ERROR: RFB: could not bind socket.\n");
		goto end_of_thread;
	}
	if(listen(sServer, SOMAXCONN) == SOCKET_ERROR) {
		fprintf(stderr, "# ERROR: RFB: could not listen on socket.\n");
		goto end_of_thread;
	}
	fprintf(stderr, "# RFB: listening for connections on port %i.\n", rfbPort);
	sai_size = sizeof(sai);
	while(keep_alive) {
		sClient = accept(sServer, (LPSOCKADDR)&sai, &sai_size);
		if(sClient != INVALID_SOCKET) {
			HandleRfbClient(sClient);
			sGlobal = INVALID_SOCKET;
			close(sClient);
		} else {
			closesocket(sClient);
		}
	}

end_of_thread:
	StopWinsock();
	_endthread();
}

void HandleRfbClient(SOCKET sClient) 
{
	char rfbName[] = "Bochs-RFB";
	rfbProtocolVersionMsg pv;
	int one = 1;
	CARD32 auth;
	rfbClientInitMsg cim;
	rfbServerInitMsg sim;

	setsockopt(sClient, IPPROTO_TCP, TCP_NODELAY, (const char *)&one, sizeof(one));
	fprintf(stderr, "# RFB: accepted client connection.\n");
	sprintf(pv, rfbProtocolVersionFormat, rfbProtocolMajorVersion, rfbProtocolMinorVersion);

	if(WriteExact(sClient, pv, sz_rfbProtocolVersionMsg) < 0) {
		fprintf(stderr, "# ERROR: RFB: could not send protocol version.\n");
		return;
	}
	if(ReadExact(sClient, pv, sz_rfbProtocolVersionMsg) < 0) {
		fprintf(stderr, "# ERROR: RFB: could not recieve client protocol version.\n");
		return;
	}

	auth = Swap32IfLE(rfbNoAuth);
	if(WriteExact(sClient, (char *)&auth, sizeof(auth)) < 0) {
		fprintf(stderr, "# ERROR: RFB: could not send authorization method.\n");
		return;
	}

	if(ReadExact(sClient, (char *)&cim, sz_rfbClientInitMsg) < 0) {
		fprintf(stderr, "# ERROR: RFB: could not recieve client initialization message.\n");
		return;
	}

	sim.framebufferWidth  = Swap16IfLE((short)rfbDimensionX);
	sim.framebufferHeight = Swap16IfLE((short)rfbDimensionY);
	sim.format            = BGR233Format;
	sim.format.redMax     = Swap16IfLE(sim.format.redMax);
	sim.format.greenMax   = Swap16IfLE(sim.format.greenMax);
	sim.format.blueMax    = Swap16IfLE(sim.format.blueMax);
	sim.nameLength = strlen(rfbName);
	sim.nameLength = Swap32IfLE(sim.nameLength);
	if(WriteExact(sClient, (char *)&sim, sz_rfbServerInitMsg) < 0) {
		fprintf(stderr, "# ERROR: RFB: could send server initialization message.\n");
		return;
	}
	if(WriteExact(sClient, rfbName, strlen(rfbName)) < 0) {
		fprintf(stderr, "# ERROR: RFB: could not send server name.\n");
		return;
	}

	sGlobal = sClient;
	while(keep_alive) {
		CARD8 msgType;
		int n;

		if((n = recv(sClient, (char *)&msgType, 1, MSG_PEEK)) <= 0) {
			if(n == 0) {
				fprintf(stderr, "# RFB: client closed connection.\n");
			} else {
				fprintf(stderr, "# RFB: error recieving data.\n");
			}
			return;
		}
		
		switch(msgType) {
		case rfbSetPixelFormat:
			{
				rfbSetPixelFormatMsg spf;
				ReadExact(sClient, (char *)&spf, sizeof(rfbSetPixelFormatMsg));

				spf.format.bitsPerPixel = spf.format.bitsPerPixel;
				spf.format.depth = spf.format.depth;
				spf.format.trueColour = (spf.format.trueColour ? 1 : 0);
				spf.format.bigEndian = (spf.format.bigEndian ? 1 : 0);
				spf.format.redMax = Swap16IfLE(spf.format.redMax);
				spf.format.greenMax = Swap16IfLE(spf.format.greenMax);
				spf.format.blueMax = Swap16IfLE(spf.format.blueMax);
				spf.format.redShift = spf.format.redShift;
				spf.format.greenShift = spf.format.greenShift;
				spf.format.blueShift = spf.format.blueShift;
				
				if (!PF_EQ(spf.format, BGR233Format)) {
					fprintf(stderr,"# ERROR: RFB: client has wrong pixel format\n");
					//return;
				}
				break;
			}
		case rfbFixColourMapEntries:
			{
				rfbFixColourMapEntriesMsg fcme;
				ReadExact(sClient, (char *)&fcme, sizeof(rfbFixColourMapEntriesMsg));
				break;
			}
		case rfbSetEncodings:
			{
				rfbSetEncodingsMsg se;
				int                i;
				CARD32             enc;
				ReadExact(sClient, (char *)&se, sizeof(rfbSetEncodingsMsg));
				se.nEncodings = Swap16IfLE(se.nEncodings);
				for(i = 0; i < se.nEncodings; i++) {
					if((n = ReadExact(sClient, (char *)&enc, sizeof(CARD32))) <= 0) {
						if(n == 0) {
							fprintf(stderr, "# RFB: client closed connection.\n");
						} else {
							fprintf(stderr, "# RFB: error recieving data.\n");
						}
						return;
					}
				}
				break;
			}
		case rfbFramebufferUpdateRequest:
			{
				rfbFramebufferUpdateRequestMsg fur;
				rfbFramebufferUpdateMsg fum;
				rfbFramebufferUpdateRectHeader furh;

				ReadExact(sClient, (char *)&fur, sizeof(rfbFramebufferUpdateRequestMsg));

				fum.type = rfbFramebufferUpdate;
				fum.nRects = Swap16IfLE(1);
				WriteExact(sClient, (char *)&fum, sz_rfbFramebufferUpdateMsg);
				furh.r.x = Swap16IfLE(0);
				furh.r.y = Swap16IfLE(0);
				furh.r.w = Swap16IfLE((short)rfbDimensionX);
				furh.r.h = Swap16IfLE((short)rfbDimensionY);
				furh.encoding = Swap32IfLE(rfbEncodingRaw);
				WriteExact(sClient, (char *)&furh, sz_rfbFramebufferUpdateRectHeader);
				WriteExact(sClient, (char *)rfbScreen, rfbDimensionX * rfbDimensionY);
				break;
			}
		case rfbKeyEvent:
			{
				rfbKeyEventMsg ke;
				Bit32u         key;
				ReadExact(sClient, (char *)&ke, sizeof(rfbKeyEventMsg));
				ke.key = Swap32IfLE(ke.key);
				key = ke.key;
				if(ke.down) {
					rfbKeyPressed(ke.key, BX_KEY_PRESSED);
				} else {
					rfbKeyPressed(ke.key, BX_KEY_RELEASED);
				}
				break;
			}
		case rfbPointerEvent:
			{	
				rfbPointerEventMsg pe;
				ReadExact(sClient, (char *)&pe, sizeof(rfbPointerEventMsg));
				break;
			}
		case rfbClientCutText:
			{
				rfbClientCutTextMsg cct;
				ReadExact(sClient, (char *)&cct, sizeof(rfbClientCutTextMsg));
				break;
			}
		}
	}
}
// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

void bx_gui_c::handle_events(void)
{
}


// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

void bx_gui_c::flush(void)
{
}


// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.  Don't
// clear the area that defines the headerbar.
void bx_gui_c::clear_screen(void)
{
	memset(&rfbScreen[rfbDimensionX * rfbHeaderbarY], 0, rfbDimensionX * (rfbDimensionY - rfbHeaderbarY));
}



// ::TEXT_UPDATE()
//
// Called in a VGA text mode, to update the screen with
// new content.
//
// old_text: array of character/attributes making up the contents
//           of the screen from the last call.  See below
// new_text: array of character/attributes making up the current
//           contents, which should now be displayed.  See below
//
// format of old_text & new_text: each is 4000 bytes long.
//     This represents 80 characters wide by 25 high, with
//     each character being 2 bytes.  The first by is the
//     character value, the second is the attribute byte.
//     I currently don't handle the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor

void bx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text, unsigned long cursor_x, unsigned long cursor_y, unsigned nrows)
{
	unsigned char cChar;
	unsigned int  nchars;
	unsigned int  i, x, y;

	nchars = 80 * nrows;
	if((rfbCursorY * 80 + rfbCursorX) < nchars) {
		cChar = new_text[(rfbCursorY * 80 + rfbCursorX) * 2];
		DrawBitmap(rfbCursorX * 8, rfbCursorY * 16 + rfbHeaderbarY, 8, 16, (char *)&bx_vgafont[cChar].data, new_text[((rfbCursorY * 80 + rfbCursorX) * 2) + 1], false);
	}

	for(i = 0; i < nchars * 2; i += 2) {
		if((old_text[i] != new_text[i]) || (old_text[i+1] != new_text[i+1])) {
			cChar = new_text[i];
			x = (i / 2) % 80;
			y = (i / 2) / 80;
			DrawBitmap(x * 8, y * 16 + rfbHeaderbarY, 8, 16, (char *)&bx_vgafont[cChar].data, new_text[i + 1], false);
		}
	}

	rfbCursorX = cursor_x;
	rfbCursorY = cursor_y;

	if((cursor_y * 80 + cursor_x) < nchars) {
		char cAttr = new_text[((cursor_y * 80 + cursor_x) * 2) + 1];
		cChar = new_text[(cursor_y * 80 + cursor_x) * 2];
		//cAttr = ((cAttr >> 4) & 0xF) + ((cAttr & 0xF) << 4);
		DrawBitmap(rfbCursorX * 8, rfbCursorY * 16 + rfbHeaderbarY, 8, 16, (char *)&bx_vgafont[cChar].data, cAttr, false);
	}

}


// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

Boolean bx_gui_c::palette_change(unsigned index, unsigned red, unsigned green, unsigned blue)
{
	rfbPallet[index] = (((red * 7 + 127) / 255) << 0) | (((green * 7 + 127) / 255) << 3) | (((blue * 3 + 127) / 255) << 6);
	return(1);
}


// ::GRAPHICS_TILE_UPDATE()
//
// Called to request that a tile of graphics be drawn to the
// screen, since info in this region has changed.
//
// tile: array of 8bit values representing a block of pixels with
//       dimension equal to the 'tilewidth' & 'tileheight' parameters to
//       ::specific_init().  Each value specifies an index into the
//       array of colors you allocated for ::palette_change()
// x0: x origin of tile
// y0: y origin of tile
//
// note: origin of tile and of window based on (0,0) being in the upper
//       left of the window.
void bx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
	UpdateScreen((char *)tile, x0, y0 + rfbHeaderbarY, rfbTileX, rfbTileY, false);
}



// ::DIMENSION_UPDATE()
//
// Called when the VGA mode changes it's X,Y dimensions.
// Resize the window to this size, but you need to add on
// the height of the headerbar to the Y value.
//
// x: new VGA x size
// y: new VGA y size (add headerbar_y parameter from ::specific_init().

  void
bx_gui_c::dimension_update(unsigned x, unsigned y)
{
  UNUSED(x);
  UNUSED(y);
}


// ::CREATE_BITMAP()
//
// Create a monochrome bitmap of size 'xdim' by 'ydim', which will
// be drawn in the headerbar.  Return an integer ID to the bitmap,
// with which the bitmap can be referenced later.
//
// bmap: packed 8 pixels-per-byte bitmap.  The pixel order is:
//       bit0 is the left most pixel, bit7 is the right most pixel.
// xdim: x dimension of bitmap
// ydim: y dimension of bitmap

unsigned bx_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
	if(rfbBitmapCount >= BX_MAX_PIXMAPS) {
		fprintf(stderr, "# RFB: too many pixmaps.\n");
		return 0;
	}
	rfbBitmaps[rfbBitmapCount].bmap = (char *)malloc((xdim * ydim) / 8);
	rfbBitmaps[rfbBitmapCount].xdim = xdim;
	rfbBitmaps[rfbBitmapCount].ydim = ydim;
	memcpy(rfbBitmaps[rfbBitmapCount].bmap, bmap, (xdim * ydim) / 8);
	
	rfbBitmapCount++;
	return(rfbBitmapCount - 1);
}


// ::HEADERBAR_BITMAP()
//
// Called to install a bitmap in the bochs headerbar (toolbar).
//
// bmap_id: will correspond to an ID returned from
//     ::create_bitmap().  'alignment' is either BX_GRAVITY_LEFT
//     or BX_GRAVITY_RIGHT, meaning install the bitmap in the next
//     available leftmost or rightmost space.
// alignment: is either BX_GRAVITY_LEFT or BX_GRAVITY_RIGHT,
//     meaning install the bitmap in the next
//     available leftmost or rightmost space.
// f: a 'C' function pointer to callback when the mouse is clicked in
//     the boundaries of this bitmap.

unsigned bx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
	int hb_index;

	if((rfbHeaderbarBitmapCount + 1) > BX_MAX_HEADERBAR_ENTRIES) {
		return 0;
	}

	rfbHeaderbarBitmapCount++;
	hb_index = rfbHeaderbarBitmapCount - 1;
	rfbHeaderbarBitmaps[hb_index].index     = bmap_id;
	rfbHeaderbarBitmaps[hb_index].alignment = alignment;
	rfbHeaderbarBitmaps[hb_index].f = f;
	if (alignment == BX_GRAVITY_LEFT) {
		rfbHeaderbarBitmaps[hb_index].xorigin = rfbOriginLeft;
		rfbHeaderbarBitmaps[hb_index].yorigin = 0;
		rfbOriginLeft += rfbBitmaps[bmap_id].xdim;
	} else { // BX_GRAVITY_RIGHT
		rfbOriginRight += rfbBitmaps[bmap_id].xdim;
		rfbHeaderbarBitmaps[hb_index].xorigin = rfbOriginRight;
		rfbHeaderbarBitmaps[hb_index].yorigin = 0;
	}
	return hb_index;
}


// ::SHOW_HEADERBAR()
//
// Show (redraw) the current headerbar, which is composed of
// currently installed bitmaps.

void bx_gui_c::show_headerbar(void)
{
	unsigned int i, xorigin;

	for(i = 0; i < rfbHeaderbarBitmapCount; i++) {
		if(rfbHeaderbarBitmaps[i].alignment == BX_GRAVITY_LEFT) {
			xorigin = rfbHeaderbarBitmaps[i].xorigin;
		} else {
			xorigin = rfbDimensionX - rfbHeaderbarBitmaps[i].xorigin;
		}
		DrawBitmap(xorigin, 0, rfbBitmaps[rfbHeaderbarBitmaps[i].index].xdim, rfbBitmaps[rfbHeaderbarBitmaps[i].index].ydim, rfbBitmaps[rfbHeaderbarBitmaps[i].index].bmap, (char)0x0F, false);
	}
}


// ::REPLACE_BITMAP()
//
// Replace the bitmap installed in the headerbar ID slot 'hbar_id',
// with the one specified by 'bmap_id'.  'bmap_id' will have
// been generated by ::create_bitmap().  The old and new bitmap
// must be of the same size.  This allows the bitmap the user
// sees to change, when some action occurs.  For example when
// the user presses on the floppy icon, it then displays
// the ejected status.
//
// hbar_id: headerbar slot ID
// bmap_id: bitmap ID

void bx_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
	rfbHeaderbarBitmaps[hbar_id].index = bmap_id;
}


// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.
void bx_gui_c::exit(void)
{
	unsigned int i;
	keep_alive = FALSE;
	StopWinsock();
	free(rfbScreen);
	for(i = 0; i < rfbBitmapCount; i++) {
		free(rfbBitmaps[i].bmap);
	}
	fprintf(stderr, "# RFB: bx_gui_c::exit()\n");
}

/*
* ReadExact reads an exact number of bytes on a TCP socket.  Returns 1 if
* those bytes have been read, 0 if the other end has closed, or -1 if an error
* occurred (errno is set to ETIMEDOUT if it timed out).
*/

int ReadExact(int sock, char *buf, int len)
{
    int n;
 
    while (len > 0) {
	n = recv(sock, buf, len, 0);
	if (n > 0) {
	    buf += n;
	    len -= n;
        } else {
            return n;
	}
    }
    return 1;
}

/*
* WriteExact writes an exact number of bytes on a TCP socket.  Returns 1 if
* those bytes have been written, or -1 if an error occurred (errno is set to
* ETIMEDOUT if it timed out).
*/

int
WriteExact(int sock, char *buf, int len)
{
    int n;
	
    while (len > 0) {
	n = send(sock, buf, len,0);
		
	if (n > 0) {
	    buf += n;
	    len -= n;
	} else if (n == 0) {
      	    fprintf(stderr,"WriteExact: write returned 0?\n");
	    return n;
        } else {
            return n;
	}
    }
    return 1;
}

void DrawBitmap(int x, int y, int width, int height, char *bmap, char color, bool update_client)
{
	int  i;
	char *newBits;
	char fgcolor, bgcolor;
	char vgaPallet[] = { (char)0x00, //Black 
						 (char)0x01, //Dark Blue
						 (char)0x02, //Dark Green
						 (char)0x03, //Dark Cyan
						 (char)0x04, //Dark Red
						 (char)0x05, //Dark Magenta
						 (char)0x06, //Brown
						 (char)0x07, //Light Gray
						 (char)0x38, //Dark Gray
						 (char)0x09, //Light Blue
						 (char)0x12, //Green
						 (char)0x1B, //Cyan
						 (char)0x24, //Light Red
						 (char)0x2D, //Magenta
						 (char)0x36, //Yellow
						 (char)0x3F  //White
						};

	bgcolor = vgaPallet[(color >> 4) & 0xF];
	fgcolor = vgaPallet[color & 0xF];
	newBits = (char *)malloc(width * height);
	memset(newBits, 0, (width * height));
	for(i = 0; i < (width * height) / 8; i++) {
		newBits[i * 8 + 0] = (bmap[i] & 0x01) ? fgcolor : bgcolor;
		newBits[i * 8 + 1] = (bmap[i] & 0x02) ? fgcolor : bgcolor;
		newBits[i * 8 + 2] = (bmap[i] & 0x04) ? fgcolor : bgcolor;
		newBits[i * 8 + 3] = (bmap[i] & 0x08) ? fgcolor : bgcolor;
		newBits[i * 8 + 4] = (bmap[i] & 0x10) ? fgcolor : bgcolor;
		newBits[i * 8 + 5] = (bmap[i] & 0x20) ? fgcolor : bgcolor;
		newBits[i * 8 + 6] = (bmap[i] & 0x40) ? fgcolor : bgcolor;
		newBits[i * 8 + 7] = (bmap[i] & 0x80) ? fgcolor : bgcolor;
	}
	UpdateScreen(newBits, x, y, width, height, update_client);
	//DrawColorPallet();
	free(newBits);
}

void DrawColorPallet()
{
	char bits[100];
	int x = 0, y = 0, c;
	for(c = 0; c < 256; c++) {
		memset(&bits, rfbPallet[c], 100);
		UpdateScreen(bits, x, y, 10, 10, false);
		x += 10;
		if(x > 70) {
			y += 10;
			x = 0;
		}
	}
}

void UpdateScreen(char *newBits, int x, int y, int width, int height, bool update_client)
{
	int i, c;
	for(i = 0; i < height; i++) {
		for(c = 0; c < width; c++) {
			newBits[(i * width) + c] = rfbPallet[newBits[(i * width) + c]];
		}
		memcpy(&rfbScreen[y * rfbDimensionX + x], &newBits[i * width], width);
		y++;
	}
	if(update_client) {
		if(sGlobal == INVALID_SOCKET) return;
		rfbFramebufferUpdateMsg fum;
		rfbFramebufferUpdateRectHeader furh;
		fum.type = rfbFramebufferUpdate;
		fum.nRects = Swap16IfLE(1);
		WriteExact(sGlobal, (char *)&fum, sz_rfbFramebufferUpdateMsg);
		furh.r.x = Swap16IfLE(x);
		furh.r.y = Swap16IfLE(y);
		furh.r.w = Swap16IfLE((short)width);
		furh.r.h = Swap16IfLE((short)height);
		furh.encoding = Swap32IfLE(rfbEncodingRaw);
		WriteExact(sGlobal, (char *)&furh, sz_rfbFramebufferUpdateRectHeader);
		WriteExact(sGlobal, (char *)newBits, width * height);
	}
}

/***********************/
/* Keyboard Definitons */
/*        And          */
/*     Functions       */
/***********************/

#define XK_space            0x020
#define XK_asciitilde       0x07e

#define XK_dead_grave       0xFE50
#define XK_dead_acute       0xFE51
#define XK_dead_circumflex  0xFE52
#define XK_dead_tilde       0xFE53

#define XK_BackSpace		0xFF08	
#define XK_Tab			    0xFF09
#define XK_Linefeed		    0xFF0A
#define XK_Clear		    0xFF0B
#define XK_Return		    0xFF0D
#define XK_Pause		    0xFF13	
#define XK_Scroll_Lock		0xFF14
#define XK_Sys_Req		    0xFF15
#define XK_Escape		    0xFF1B

#define XK_Delete		    0xFFFF

#define XK_Home			0xFF50
#define XK_Left			0xFF51	
#define XK_Up			0xFF52	
#define XK_Right		0xFF53
#define XK_Down			0xFF54
#define XK_Page_Up		0xFF55
#define XK_Page_Down	0xFF56
#define XK_End			0xFF57	
#define XK_Begin		0xFF58	

#define XK_Select		0xFF60	
#define XK_Print		0xFF61
#define XK_Execute		0xFF62	
#define XK_Insert		0xFF63	

#define XK_Cancel		0xFF69	
#define XK_Help			0xFF6A
#define XK_Break		0xFF6B
#define XK_Num_Lock		0xFF7F

#define XK_KP_Space		0xFF80
#define XK_KP_Tab		0xFF89
#define XK_KP_Enter		0xFF8D	

#define XK_KP_Home		0xFF95
#define XK_KP_Left		0xFF96
#define XK_KP_Up		0xFF97
#define XK_KP_Right		0xFF98
#define XK_KP_Down		0xFF99
#define XK_KP_Prior		0xFF9A
#define XK_KP_Page_Up	0xFF9A
#define XK_KP_Next		0xFF9B
#define XK_KP_Page_Down	0xFF9B
#define XK_KP_End		0xFF9C
#define XK_KP_Begin		0xFF9D
#define XK_KP_Insert	0xFF9E
#define XK_KP_Delete	0xFF9F
#define XK_KP_Equal		0xFFBD
#define XK_KP_Multiply	0xFFAA
#define XK_KP_Add		0xFFAB
#define XK_KP_Separator	0xFFAC	
#define XK_KP_Subtract	0xFFAD
#define XK_KP_Decimal	0xFFAE
#define XK_KP_Divide	0xFFAF

#define XK_KP_F1		0xFF91
#define XK_KP_F2		0xFF92
#define XK_KP_F3		0xFF93
#define XK_KP_F4		0xFF94

#define XK_KP_0			0xFFB0
#define XK_KP_1			0xFFB1
#define XK_KP_2			0xFFB2
#define XK_KP_3			0xFFB3
#define XK_KP_4			0xFFB4
#define XK_KP_5			0xFFB5
#define XK_KP_6			0xFFB6
#define XK_KP_7			0xFFB7
#define XK_KP_8			0xFFB8
#define XK_KP_9			0xFFB9

#define XK_F1			0xFFBE
#define XK_F2			0xFFBF
#define XK_F3			0xFFC0
#define XK_F4			0xFFC1
#define XK_F5			0xFFC2
#define XK_F6			0xFFC3
#define XK_F7			0xFFC4
#define XK_F8			0xFFC5
#define XK_F9			0xFFC6
#define XK_F10			0xFFC7
#define XK_F11			0xFFC8
#define XK_F12			0xFFC9
#define XK_F13			0xFFCA
#define XK_F14			0xFFCB
#define XK_F15			0xFFCC
#define XK_F16			0xFFCD
#define XK_F17			0xFFCE
#define XK_F18			0xFFCF
#define XK_F19			0xFFD0
#define XK_F20			0xFFD1
#define XK_F21			0xFFD2
#define XK_F22			0xFFD3
#define XK_F23			0xFFD4
#define XK_F24			0xFFD5


#define XK_Shift_L		0xFFE1	
#define XK_Shift_R		0xFFE2	
#define XK_Control_L	0xFFE3	
#define XK_Control_R	0xFFE4	
#define XK_Caps_Lock	0xFFE5	
#define XK_Shift_Lock	0xFFE6	
#define XK_Meta_L		0xFFE7	
#define XK_Meta_R		0xFFE8
#define XK_Alt_L		0xFFE9
#define XK_Alt_R		0xFFEA

Bit32u rfbAsciiKey[0x5f] = {
  //  !"#$%&'
  BX_KEY_SPACE,
  BX_KEY_1,
  BX_KEY_SINGLE_QUOTE,
  BX_KEY_3,
  BX_KEY_4,
  BX_KEY_5,
  BX_KEY_7,
  BX_KEY_SINGLE_QUOTE,

  // ()*+,-./
  BX_KEY_9,
  BX_KEY_0,
  BX_KEY_8,
  BX_KEY_EQUALS,
  BX_KEY_COMMA,
  BX_KEY_MINUS,
  BX_KEY_PERIOD,
  BX_KEY_SLASH,

  // 01234567
  BX_KEY_0,
  BX_KEY_1,
  BX_KEY_2,
  BX_KEY_3,
  BX_KEY_4,
  BX_KEY_5,
  BX_KEY_6,
  BX_KEY_7,

  // 89:;<=>?
  BX_KEY_8,
  BX_KEY_9,
  BX_KEY_SEMICOLON,
  BX_KEY_SEMICOLON,
  BX_KEY_COMMA,
  BX_KEY_EQUALS,
  BX_KEY_PERIOD,
  BX_KEY_SLASH,

  // @ABCDEFG
  BX_KEY_2,
  BX_KEY_A,
  BX_KEY_B,
  BX_KEY_C,
  BX_KEY_D,
  BX_KEY_E,
  BX_KEY_F,
  BX_KEY_G,


  // HIJKLMNO
  BX_KEY_H,
  BX_KEY_I,
  BX_KEY_J,
  BX_KEY_K,
  BX_KEY_L,
  BX_KEY_M,
  BX_KEY_N,
  BX_KEY_O,


  // PQRSTUVW
  BX_KEY_P,
  BX_KEY_Q,
  BX_KEY_R,
  BX_KEY_S,
  BX_KEY_T,
  BX_KEY_U,
  BX_KEY_V,
  BX_KEY_W,

  // XYZ[\]^_
  BX_KEY_X,
  BX_KEY_Y,
  BX_KEY_Z,
  BX_KEY_LEFT_BRACKET,
  BX_KEY_BACKSLASH,
  BX_KEY_RIGHT_BRACKET,
  BX_KEY_6,
  BX_KEY_MINUS,

  // `abcdefg
  BX_KEY_GRAVE,
  BX_KEY_A,
  BX_KEY_B,
  BX_KEY_C,
  BX_KEY_D,
  BX_KEY_E,
  BX_KEY_F,
  BX_KEY_G,

  // hijklmno
  BX_KEY_H,
  BX_KEY_I,
  BX_KEY_J,
  BX_KEY_K,
  BX_KEY_L,
  BX_KEY_M,
  BX_KEY_N,
  BX_KEY_O,

  // pqrstuvw
  BX_KEY_P,
  BX_KEY_Q,
  BX_KEY_R,
  BX_KEY_S,
  BX_KEY_T,
  BX_KEY_U,
  BX_KEY_V,
  BX_KEY_W,

  // xyz{|}~
  BX_KEY_X,
  BX_KEY_Y,
  BX_KEY_Z,
  BX_KEY_LEFT_BRACKET,
  BX_KEY_BACKSLASH,
  BX_KEY_RIGHT_BRACKET,
  BX_KEY_GRAVE
};

void rfbKeyPressed(Bit32u key, int press_release)
{
	Bit32u key_event;

	if((key >= XK_space) && (key <= XK_asciitilde)) {
		key_event = rfbAsciiKey[key - XK_space];
	} else switch (key) {
		case XK_KP_1:
#ifdef XK_KP_End
		case XK_KP_End:
#endif
		key_event = BX_KEY_KP_END; break;

		case XK_KP_2:
#ifdef XK_KP_Down
		case XK_KP_Down:
#endif
		key_event = BX_KEY_KP_DOWN; break;
		
		case XK_KP_3:
#ifdef XK_KP_Page_Down
		case XK_KP_Page_Down:
#endif
		key_event = BX_KEY_KP_PAGE_DOWN; break;
		
		case XK_KP_4:
#ifdef XK_KP_Left
		case XK_KP_Left:
#endif
		key_event = BX_KEY_KP_LEFT; break;
		
		case XK_KP_5:
		key_event = BX_KEY_KP_5; break;
		
		case XK_KP_6:
#ifdef XK_KP_Right
		case XK_KP_Right:
#endif
		key_event = BX_KEY_KP_RIGHT; break;

		case XK_KP_7:
#ifdef XK_KP_Home
		case XK_KP_Home:
#endif
		key_event = BX_KEY_KP_HOME; break;

		case XK_KP_8:
#ifdef XK_KP_Up
		case XK_KP_Up:
#endif
		key_event = BX_KEY_KP_UP; break;

		case XK_KP_9:
#ifdef XK_KP_Page_Up
		case XK_KP_Page_Up:
#endif
		key_event = BX_KEY_KP_PAGE_UP; break;

		case XK_KP_0:
#ifdef XK_KP_Insert
		case XK_KP_Insert:
		key_event = BX_KEY_KP_INSERT; break;
#endif

		case XK_KP_Decimal:
#ifdef XK_KP_Delete
		case XK_KP_Delete:
		key_event = BX_KEY_KP_DELETE; break;
#endif

#ifdef XK_KP_Enter
		case XK_KP_Enter:
		key_event = BX_KEY_KP_ENTER; break;
#endif

		case XK_KP_Subtract: key_event = BX_KEY_KP_SUBTRACT; break;
		case XK_KP_Add:      key_event = BX_KEY_KP_ADD; break;

		case XK_KP_Multiply: key_event = BX_KEY_KP_MULTIPLY; break;
		case XK_KP_Divide:   key_event = BX_KEY_KP_DIVIDE; break;


		case XK_Up:          key_event = BX_KEY_UP; break;
		case XK_Down:        key_event = BX_KEY_DOWN; break;
		case XK_Left:        key_event = BX_KEY_LEFT; break;
	    case XK_Right:       key_event = BX_KEY_RIGHT; break;

	    case XK_Delete:      key_event = BX_KEY_DELETE; break;
		case XK_BackSpace:   key_event = BX_KEY_BACKSPACE; break;
		case XK_Tab:         key_event = BX_KEY_TAB; break;
		case XK_Return:      key_event = BX_KEY_ENTER; break;
		case XK_Escape:      key_event = BX_KEY_ESC; break;
		case XK_F1:          key_event = BX_KEY_F1; break;
		case XK_F2:          key_event = BX_KEY_F2; break;
		case XK_F3:          key_event = BX_KEY_F3; break;
		case XK_F4:          key_event = BX_KEY_F4; break;
		case XK_F5:          key_event = BX_KEY_F5; break;
		case XK_F6:          key_event = BX_KEY_F6; break;
		case XK_F7:          key_event = BX_KEY_F7; break;
		case XK_F8:          key_event = BX_KEY_F8; break;
		case XK_F9:          key_event = BX_KEY_F9; break;
		case XK_F10:         key_event = BX_KEY_F10; break;
		case XK_F11:         key_event = BX_KEY_F11; break;
		case XK_F12:         key_event = BX_KEY_F12; break;
		case XK_Control_L:   key_event = BX_KEY_CTRL_L; break;
		case XK_Shift_L:     key_event = BX_KEY_SHIFT_L; break;
		case XK_Shift_R:     key_event = BX_KEY_SHIFT_R; break;
		case XK_Caps_Lock:   key_event = BX_KEY_CAPS_LOCK; break;
		case XK_Num_Lock:    key_event = BX_KEY_NUM_LOCK; break;
		case XK_Alt_L:       key_event = BX_KEY_ALT_L; break;

	    case XK_Insert:      key_event = BX_KEY_INSERT; break;
	    case XK_Home:        key_event = BX_KEY_HOME; break;
	    case XK_End:         key_event = BX_KEY_END; break;
	    case XK_Page_Up:     key_event = BX_KEY_PAGE_UP; break;
	    case XK_Page_Down:   key_event = BX_KEY_PAGE_DOWN; break;
	
		default:
			fprintf(stderr, "# RFB: rfbKeyPress(): key %x unhandled!\n", (unsigned) key);
			return;
			break;
	}

	if (press_release)
		key_event |= BX_KEY_RELEASED;
	bx_devices.keyboard->gen_scancode(key_event);
}
