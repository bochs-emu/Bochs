/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000  Psyon.Org!
//
//    Donald Becker
//    http://www.psyon.org
//
//  Copyright (C) 2001-2014  The Bochs Project
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

// RFB still to do :
// - properly handle SetPixelFormat, including big/little-endian flag
// - depth > 8bpp support
// - optional compression support


// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "param_names.h"
#include "iodev.h"
#include "keymap.h"
#if BX_WITH_RFB

#include "icon_bochs.h"
#include "font/vga.bitmap.h"
#if (BX_WITH_SDL || BX_WITH_SDL2) && !BX_PLUGINS
extern unsigned char sdl_font8x8[256][8];
#else
#include "sdl.h" // 8x8 font for status text
#endif

#include "rfb.h"
#include "rfbkeys.h"

class bx_rfb_gui_c : public bx_gui_c {
public:
  bx_rfb_gui_c (void) {}
  DECLARE_GUI_VIRTUAL_METHODS()
  DECLARE_GUI_NEW_VIRTUAL_METHODS()
  void get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp);
  void statusbar_setitem_specific(int element, bx_bool active, bx_bool w);
  virtual void set_mouse_mode_absxy(bx_bool mode);
#if BX_SHOW_IPS
  void show_ips(Bit32u ips_count);
#endif
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_rfb_gui_c *theGui = NULL;
IMPLEMENT_GUI_PLUGIN_CODE(rfb)

#define LOG_THIS theGui->

#if defined(WIN32) && !defined(__CYGWIN__)

#include <winsock2.h>
#include <process.h>
#define BX_RFB_WIN32

#else

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#ifndef __QNXNTO__
#include <sys/errno.h>
#else
#include <errno.h>
#endif
#if !defined(__CYGWIN__)
#include <pthread.h>
#endif

typedef int SOCKET;
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#endif

static bx_bool keep_alive;
static bx_bool client_connected;
static bx_bool desktop_resizable;
#if BX_SHOW_IPS
static bx_bool rfbHideIPS = 0;
static bx_bool rfbIPSupdate = 0;
static char rfbIPStext[40];
#endif
static unsigned short rfbPort;

// Headerbar stuff
static unsigned rfbBitmapCount = 0;
static struct _rfbBitmaps {
    char *bmap;
    unsigned xdim;
    unsigned ydim;
} rfbBitmaps[BX_MAX_PIXMAPS];

static unsigned rfbHeaderbarBitmapCount = 0;
static struct _rfbHeaderbarBitmaps {
    unsigned int index;
    unsigned int xorigin;
    unsigned int yorigin;
    unsigned int alignment;
    void (*f)(void);
} rfbHeaderbarBitmaps[BX_MAX_HEADERBAR_ENTRIES];

//Keyboard stuff
#define KEYBOARD 1
#define MOUSE    0
#define MAX_KEY_EVENTS 512
static struct _rfbKeyboardEvent {
    bx_bool type;
    int key;
    int down;
    int x;
    int y;
    int z;
} rfbKeyboardEvent[MAX_KEY_EVENTS];
static unsigned long rfbKeyboardEvents = 0;
static bx_bool bKeyboardInUse = 0;

// Misc Stuff
static struct _rfbUpdateRegion {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    bx_bool updated;
} rfbUpdateRegion;

#define BX_RFB_MAX_XDIM 1280
#define BX_RFB_MAX_YDIM 1024
#define BX_RFB_DEF_XDIM 720
#define BX_RFB_DEF_YDIM 480

const unsigned char status_led_green = 0x38;
const unsigned char status_gray_text = 0xa4;
const unsigned char status_led_red = 0x07;
const unsigned char headerbar_bg = 0xff;
const unsigned char headerbar_fg = 0x00;

static char *rfbScreen;
static char rfbPalette[256];

static unsigned rfbWindowX, rfbWindowY;
static unsigned rfbDimensionX, rfbDimensionY;
static long rfbHeaderbarY;
static unsigned rfbTileX = 0;
static unsigned rfbTileY = 0;
static unsigned long rfbCursorX = 0;
static unsigned long rfbCursorY = 0;
static unsigned long rfbOriginLeft = 0;
static unsigned long rfbOriginRight = 0;
static bx_bool rfbMouseModeAbsXY = 0;
static unsigned rfbStatusbarY = 18;
static unsigned rfbStatusitemPos[12] = {
  0, 170, 210, 250, 290, 330, 370, 410, 450, 490, 530, 570
};
static bx_bool rfbStatusitemActive[12];

static unsigned int text_rows = 25, text_cols = 80;
static unsigned int font_height = 16, font_width = 8;

static SOCKET sGlobal;

static Bit32u clientEncodingsCount = 0;
static Bit32u *clientEncodings = NULL;

#ifdef BX_RFB_WIN32
bool StopWinsock();
#endif
void rfbStartThread();
void HandleRfbClient(SOCKET sClient);
int ReadExact(int sock, char *buf, int len);
int WriteExact(int sock, char *buf, int len);
void DrawBitmap(int x, int y, int width, int height, char *bmap, char fg,
        char bg, bx_bool update_client);
void DrawChar(int x, int y, int width, int height, int fonty, char *bmap,
        char fg, char bg, bx_bool gfxchar);
void UpdateScreen(unsigned char *newBits, int x, int y, int width, int height,
        bx_bool update_client);
void SendUpdate(int x, int y, int width, int height, Bit32u encoding);
void rfbAddUpdateRegion(unsigned x0, unsigned y0, unsigned w, unsigned h);
void rfbSetStatusText(int element, const char *text, bx_bool active, bx_bool w = 0);
static Bit32u convertStringToRfbKey(const char *string);
void rfbKeyPressed(Bit32u key, int press_release);
void rfbMouseMove(int x, int y, int z, int bmask);
#if BX_SHOW_IPS && defined(WIN32)
DWORD WINAPI rfbShowIPSthread(LPVOID);
#endif

static const rfbPixelFormat BGR233Format = {
    8, 8, 1, 1, 7, 7, 3, 0, 3, 6
};

// VNCViewer code to be replaced
#define PF_EQ(x,y) ((x.bitsPerPixel == y.bitsPerPixel) && (x.depth == y.depth) && (x.trueColourFlag == y.trueColourFlag) && ((x.bigEndianFlag == y.bigEndianFlag) || (x.bitsPerPixel == 8)) && (!x.trueColourFlag || ((x.redMax == y.redMax) &&    (x.greenMax == y.greenMax) && (x.blueMax == y.blueMax) && (x.redShift == y.redShift) && (x.greenShift == y.greenShift) && (x.blueShift == y.blueShift))))


// RFB implementation of the bx_gui_c methods

// ::SPECIFIC_INIT()
//
// Called from gui.cc, once upon program startup, to allow for the
// specific GUI code (X11, Win32, ...) to be initialized.
//
// argc, argv: used to pass display library specific options to the init code
//     (X11 options, Win32 options,...)
//
// headerbar_y:  A headerbar (toolbar) is display on the top of the
//     VGA window, showing floppy status, and other information.  It
//     always assumes the width of the current VGA mode width, but
//     it's height is defined by this parameter.

void bx_rfb_gui_c::specific_init(int argc, char **argv, unsigned headerbar_y)
{
  unsigned char fc, vc;
  int i, timeout = 30;

  put("RFB");
  UNUSED(bochs_icon_bits);

  rfbHeaderbarY = headerbar_y;
  rfbDimensionX = BX_RFB_DEF_XDIM;
  rfbDimensionY = BX_RFB_DEF_YDIM;
  rfbWindowX = rfbDimensionX;
  rfbWindowY = rfbDimensionY + rfbHeaderbarY + rfbStatusbarY;
  rfbTileX = x_tilesize;
  rfbTileY = y_tilesize;

  for (i = 0; i < 256; i++) {
    for (int j = 0; j < 16; j++) {
      vc = bx_vgafont[i].data[j];
      fc = 0;
      for (int b = 0; b < 8; b++) {
        fc |= (vc & 0x01) << (7 - b);
        vc >>= 1;
      }
      vga_charmap[i * 32 + j] = fc;
    }
  }

  // parse rfb specific options
  if (argc > 1) {
    for (i = 1; i < argc; i++) {
      if (!strncmp(argv[i], "timeout=", 8)) {
        timeout = atoi(&argv[i][8]);
        if (timeout < 0) {
          BX_PANIC(("invalid timeout value: %d", timeout));
        } else {
          BX_INFO(("connection timeout set to %d", timeout));
        }
#if BX_SHOW_IPS
      } else if (!strcmp(argv[i], "hideIPS")) {
        BX_INFO(("hide IPS display in status bar"));
        rfbHideIPS = 1;
#endif
      } else {
        BX_PANIC(("Unknown rfb option '%s'", argv[i]));
      }
    }
  }

  if (SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get()) {
    BX_ERROR(("private_colormap option ignored."));
  }

  rfbScreen = (char *)malloc(rfbWindowX * rfbWindowY);
  memset(&rfbPalette, 0, sizeof(rfbPalette));

  rfbUpdateRegion.x = rfbWindowX;
  rfbUpdateRegion.y = rfbWindowY;
  rfbUpdateRegion.width  = 0;
  rfbUpdateRegion.height = 0;
  rfbUpdateRegion.updated = 0;

  clientEncodingsCount=0;
  clientEncodings=NULL;

  keep_alive = 1;
  client_connected = 0;
  desktop_resizable = 0;
  rfbStartThread();

#ifdef WIN32
  Sleep(1000);
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif

  // load keymap for rfb
  if (SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
    bx_keymap.loadKeymap(convertStringToRfbKey);
  }

  // the ask menu doesn't work on the client side
  io->set_log_action(LOGLEV_PANIC, ACT_FATAL);

  if (timeout > 0) {
    while ((!client_connected) && (timeout--)) {
      fprintf(stderr, "Bochs RFB server waiting for client: %2d\r", timeout+1);
#ifdef BX_RFB_WIN32
      Sleep(1000);
#else
      sleep(1);
#endif
    }
    if ((timeout < 0) && (!client_connected)) {
      BX_PANIC(("timeout! no client present"));
    } else {
      fprintf(stderr, "RFB client connected                   \r");
    }
  }

#if BX_SHOW_IPS && defined(WIN32)
  if (!rfbHideIPS) {
    DWORD threadID;
    CreateThread(NULL, 0, rfbShowIPSthread, NULL, 0, &threadID);
  }
#endif

  new_gfx_api = 1;
  dialog_caps = 0;
}

// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

void bx_rfb_gui_c::handle_events(void)
{
  while (bKeyboardInUse) ;

  bKeyboardInUse = 1;
  if (rfbKeyboardEvents > 0) {
    for (unsigned i = 0; i < rfbKeyboardEvents; i++) {
      if (rfbKeyboardEvent[i].type == KEYBOARD) {
        rfbKeyPressed(rfbKeyboardEvent[i].key, rfbKeyboardEvent[i].down);
      } else { //type == MOUSE;
        rfbMouseMove(rfbKeyboardEvent[i].x, rfbKeyboardEvent[i].y, rfbKeyboardEvent[i].z, rfbKeyboardEvent[i].down);
      }
    }
    rfbKeyboardEvents = 0;
  }
  bKeyboardInUse = 0;

  if (rfbUpdateRegion.updated) {
    SendUpdate(rfbUpdateRegion.x, rfbUpdateRegion.y, rfbUpdateRegion.width,
               rfbUpdateRegion.height, rfbEncodingRaw);
    rfbUpdateRegion.x = rfbWindowX;
    rfbUpdateRegion.y = rfbWindowY;
    rfbUpdateRegion.width  = 0;
    rfbUpdateRegion.height = 0;
    rfbUpdateRegion.updated = 0;
  }
#if BX_SHOW_IPS
  if (rfbIPSupdate) {
    rfbIPSupdate = 0;
    rfbSetStatusText(0, rfbIPStext, 1);
  }
#endif
}

// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

void bx_rfb_gui_c::flush(void)
{
}

// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.  Don't
// clear the area that defines the headerbar.
void bx_rfb_gui_c::clear_screen(void)
{
  memset(&rfbScreen[rfbWindowX * rfbHeaderbarY], 0, rfbWindowX * rfbDimensionY);
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
// format of old_text & new_text: each is tm_info->line_offset*text_rows
//     bytes long. Each character consists of 2 bytes.  The first by is
//     the character value, the second is the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor
// tm_info:  this structure contains information for additional
//           features in text mode (cursor shape, line offset,...)

void bx_rfb_gui_c::text_update(Bit8u *old_text, Bit8u *new_text, unsigned long cursor_x, unsigned long cursor_y, bx_vga_tminfo_t *tm_info)
{
  Bit8u *old_line, *new_line;
  Bit8u cAttr, cChar;
  unsigned int curs, hchars, offset, rows, x, y, xc, yc, i;
  bx_bool force_update = 0, gfxchar, blink_state, blink_mode;
  char text_palette[16];
  char fgcolor, bgcolor;

  for (i = 0; i < 16; i++) {
    text_palette[i] = rfbPalette[tm_info->actl_palette[i]];
  }

  blink_mode = (tm_info->blink_flags & BX_TEXT_BLINK_MODE) > 0;
  blink_state = (tm_info->blink_flags & BX_TEXT_BLINK_STATE) > 0;
  if (blink_mode) {
    if (tm_info->blink_flags & BX_TEXT_BLINK_TOGGLE)
      force_update = 1;
  }
  if (charmap_updated) {
    force_update = 1;
    charmap_updated = 0;
  }

  // first invalidate character at previous and new cursor location
  if ((rfbCursorY < text_rows) && (rfbCursorX < text_cols)) {
    curs = rfbCursorY * tm_info->line_offset + rfbCursorX * 2;
    old_text[curs] = ~new_text[curs];
  }
  if ((tm_info->cs_start <= tm_info->cs_end) && (tm_info->cs_start < font_height)
      && (cursor_y < text_rows) && (cursor_x < text_cols)) {
    curs = cursor_y * tm_info->line_offset + cursor_x * 2;
    old_text[curs] = ~new_text[curs];
  } else {
    curs = 0xffff;
  }

  rows = text_rows;
  y = 0;
  do {
    hchars = text_cols;
    new_line = new_text;
    old_line = old_text;
    offset = y * tm_info->line_offset;
    yc = y * font_height + rfbHeaderbarY;
    x = 0;
    do {
      if (force_update || (old_text[0] != new_text[0])
          || (old_text[1] != new_text[1])) {
        cChar = new_text[0];

        if (blink_mode) {
          cAttr = new_text[1] & 0x7F;
          if (!blink_state && (new_text[1] & 0x80))
            cAttr = (cAttr & 0x70) | (cAttr >> 4);
        } else {
          cAttr = new_text[1];
        }
        fgcolor = text_palette[cAttr & 0x0F];
        bgcolor = text_palette[cAttr >> 4];

        gfxchar = tm_info->line_graphics && ((cChar & 0xE0) == 0xC0);
        xc = x * font_width;
        DrawChar(xc, yc, font_width, font_height, 0, (char *)&vga_charmap[cChar<<5],
                 fgcolor, bgcolor, gfxchar);
        rfbAddUpdateRegion(xc, yc, font_width, font_height);
        if (offset == curs) {
          cAttr = ((cAttr >> 4) & 0xF) + ((cAttr & 0xF) << 4);
          DrawChar(xc, yc + tm_info->cs_start, font_width, tm_info->cs_end - tm_info->cs_start + 1,
                   tm_info->cs_start, (char *)&vga_charmap[cChar<<5], bgcolor, fgcolor, gfxchar);
        }
      }
      x++;
      new_text += 2;
      old_text += 2;
      offset += 2;
    } while (--hchars);
    y++;
    new_text = new_line + tm_info->line_offset;
    old_text = old_line + tm_info->line_offset;
  } while (--rows);

  rfbCursorX = cursor_x;
  rfbCursorY = cursor_y;
}

int bx_rfb_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  return 0;
}

int bx_rfb_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  return 0;
}

// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

bx_bool bx_rfb_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue)
{
  rfbPalette[index] = (((red * 7 + 127) / 255) << 0) | (((green * 7 + 127) / 255) << 3) | (((blue * 3 + 127) / 255) << 6);
  return 1;
}

// ::GRAPHICS_TILE_UPDATE()
//
// Called to request that a tile of graphics be drawn to the
// screen, since info in this region has changed.
//
// tile: array of 8bit values representing a block of pixels with
//       dimension equal to the 'x_tilesize' & 'y_tilesize' members.
//       Each value specifies an index into the
//       array of colors you allocated for ::palette_change()
// x0: x origin of tile
// y0: y origin of tile
//
// note: origin of tile and of window based on (0,0) being in the upper
//       left of the window.
void bx_rfb_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  unsigned c, i, h, y;

  switch (guest_bpp) {
    case 8: /* 8 bpp */
      y = y0 + rfbHeaderbarY;
      if ((y0 + rfbTileY) > rfbDimensionY) {
        h = rfbDimensionY - y0;
      } else {
        h = rfbTileY;
      }
      for (i = 0; i < h; i++) {
        for (c = 0; c < rfbTileX; c++) {
          tile[(i * rfbTileX) + c] = rfbPalette[tile[(i * rfbTileX) + c]];
        }
        memcpy(&rfbScreen[y * rfbWindowX + x0], &tile[i * rfbTileX], rfbTileX);
        y++;
      }
      break;
    default:
      BX_PANIC(("%u bpp modes handled by new graphics API", guest_bpp));
      return;
  }
  rfbAddUpdateRegion(x0, y0 + rfbHeaderbarY, rfbTileX, h);
}

// ::DIMENSION_UPDATE()
//
// Called when the VGA mode changes it's X,Y dimensions.
// Resize the window to this size, but you need to add on
// the height of the headerbar to the Y value.
//
// x: new VGA x size
// y: new VGA y size (add headerbar_y parameter from ::specific_init().
// fheight: new VGA character height in text mode
// fwidth : new VGA character width in text mode
// bpp : bits per pixel in graphics mode

void bx_rfb_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth, unsigned bpp)
{
  if (bpp == 8) {
    guest_bpp = bpp;
  } else {
    BX_PANIC(("%d bpp graphics mode not supported yet", bpp));
  }
  guest_textmode = (fheight > 0);
  guest_xres = x;
  guest_yres = y;
  if (guest_textmode) {
    font_height = fheight;
    font_width = fwidth;
    text_cols = x / fwidth;
    text_rows = y / fheight;
  }
  if ((x != rfbDimensionX) || (y != rfbDimensionY)) {
    if (desktop_resizable) {
      if ((x > BX_RFB_MAX_XDIM) || (y > BX_RFB_MAX_YDIM)) {
        BX_PANIC(("dimension_update(): RFB doesn't support graphics mode %dx%d", x, y));
      }
      rfbDimensionX = x;
      rfbDimensionY = y;
      rfbWindowX = rfbDimensionX;
      rfbWindowY = rfbDimensionY + rfbHeaderbarY + rfbStatusbarY;
      rfbScreen = (char *)realloc(rfbScreen, rfbWindowX * rfbWindowY);
      SendUpdate(0, 0, rfbWindowX, rfbWindowY, rfbEncodingDesktopSize);
      bx_gui->show_headerbar();
    } else {
      if ((x > BX_RFB_DEF_XDIM) || (y > BX_RFB_DEF_YDIM)) {
        BX_PANIC(("dimension_update(): RFB doesn't support graphics mode %dx%d", x, y));
      }
      clear_screen();
      SendUpdate(0, rfbHeaderbarY, rfbDimensionX, rfbDimensionY, rfbEncodingRaw);
      rfbDimensionX = x;
      rfbDimensionY = y;
    }
  }
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

unsigned bx_rfb_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  if (rfbBitmapCount >= BX_MAX_PIXMAPS) {
    BX_ERROR(("too many pixmaps."));
    return 0;
  }
  rfbBitmaps[rfbBitmapCount].bmap = (char *) malloc((xdim * ydim) / 8);
  rfbBitmaps[rfbBitmapCount].xdim = xdim;
  rfbBitmaps[rfbBitmapCount].ydim = ydim;
  memcpy(rfbBitmaps[rfbBitmapCount].bmap, bmap, (xdim * ydim) / 8);

  rfbBitmapCount++;
  return (rfbBitmapCount - 1);
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

unsigned bx_rfb_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  int hb_index;

  if ((rfbHeaderbarBitmapCount + 1) > BX_MAX_HEADERBAR_ENTRIES) {
    return 0;
  }

  rfbHeaderbarBitmapCount++;
  hb_index = rfbHeaderbarBitmapCount - 1;
  rfbHeaderbarBitmaps[hb_index].index = bmap_id;
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

void bx_rfb_gui_c::show_headerbar(void)
{
  char *newBits, value;
  unsigned int i, xorigin, addr;

  newBits = (char *) malloc(rfbWindowX * rfbHeaderbarY);
  memset(newBits, 0, (rfbWindowX * rfbHeaderbarY));
  DrawBitmap(0, 0, rfbWindowX, rfbHeaderbarY, newBits, headerbar_fg, headerbar_bg, 0);
  for (i = 0; i < rfbHeaderbarBitmapCount; i++) {
    if (rfbHeaderbarBitmaps[i].alignment == BX_GRAVITY_LEFT) {
      xorigin = rfbHeaderbarBitmaps[i].xorigin;
    } else {
      xorigin = rfbWindowX - rfbHeaderbarBitmaps[i].xorigin;
    }
    DrawBitmap(xorigin, 0, rfbBitmaps[rfbHeaderbarBitmaps[i].index].xdim,
               rfbBitmaps[rfbHeaderbarBitmaps[i].index].ydim,
               rfbBitmaps[rfbHeaderbarBitmaps[i].index].bmap,
               headerbar_fg, headerbar_bg, 0);
  }
  free(newBits);
  newBits = (char *) malloc(rfbWindowX * rfbStatusbarY / 8);
  memset(newBits, 0, (rfbWindowX * rfbStatusbarY / 8));
  for (i = 1; i < 12; i++) {
    addr = rfbStatusitemPos[i] / 8;
    value = 1 << (rfbStatusitemPos[i] % 8);
    for (unsigned j = 1; j < rfbStatusbarY; j++) {
      newBits[(rfbWindowX * j / 8) + addr] = value;
    }
  }
  DrawBitmap(0, rfbWindowY - rfbStatusbarY, rfbWindowX, rfbStatusbarY, newBits,
             headerbar_fg, headerbar_bg, 0);
  free(newBits);
  for (i = 1; i <= statusitem_count; i++) {
    rfbSetStatusText(i, statusitem[i - 1].text, rfbStatusitemActive[i]);
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

void bx_rfb_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  unsigned int xorigin;

  if (bmap_id == rfbHeaderbarBitmaps[hbar_id].index)
    return;
  rfbHeaderbarBitmaps[hbar_id].index = bmap_id;
  if (rfbHeaderbarBitmaps[hbar_id].alignment == BX_GRAVITY_LEFT) {
    xorigin = rfbHeaderbarBitmaps[hbar_id].xorigin;
  } else {
    xorigin = rfbWindowX - rfbHeaderbarBitmaps[hbar_id].xorigin;
  }
  DrawBitmap(xorigin, 0, rfbBitmaps[rfbHeaderbarBitmaps[hbar_id].index].xdim,
             rfbBitmaps[rfbHeaderbarBitmaps[hbar_id].index].ydim,
             rfbBitmaps[rfbHeaderbarBitmaps[hbar_id].index].bmap, headerbar_fg,
             headerbar_bg, 1);
}

// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.
void bx_rfb_gui_c::exit(void)
{
  unsigned int i;
  keep_alive = 0;
#ifdef BX_RFB_WIN32
  StopWinsock();
#endif
  free(rfbScreen);
  for(i = 0; i < rfbBitmapCount; i++) {
    free(rfbBitmaps[i].bmap);
  }

  // Clear supported encodings
  if (clientEncodings != NULL) {
    delete [] clientEncodings;
    clientEncodingsCount = 0;
  }

  BX_DEBUG(("bx_rfb_gui_c::exit()"));
}

void bx_rfb_gui_c::mouse_enabled_changed_specific(bx_bool val)
{
}

bx_svga_tileinfo_t *bx_rfb_gui_c::graphics_tile_info(bx_svga_tileinfo_t *info)
{
  info->bpp = 8;
  info->pitch = rfbWindowX;
  info->red_shift = 3;
  info->green_shift = 6;
  info->blue_shift = 8;
  info->red_mask = 0x07;
  info->green_mask = 0x38;
  info->blue_mask = 0xc0;
  info->is_indexed = 0;
#ifdef BX_LITTLE_ENDIAN
  info->is_little_endian = 1;
#else
  info->is_little_endian = 0;
#endif

  return info;
}

Bit8u *bx_rfb_gui_c::graphics_tile_get(unsigned x0, unsigned y0,
                            unsigned *w, unsigned *h)
{
  if (x0 + rfbTileX > rfbDimensionX) {
    *w = rfbDimensionX - x0;
  } else {
    *w = rfbTileX;
  }

  if (y0 + rfbTileY > rfbDimensionY) {
    *h = rfbDimensionY - y0;
  } else {
    *h = rfbTileY;
  }

  return (Bit8u *)rfbScreen + (rfbHeaderbarY + y0) * rfbWindowX + x0;
}

void bx_rfb_gui_c::graphics_tile_update_in_place(unsigned x0, unsigned y0,
                                        unsigned w, unsigned h)
{
  rfbAddUpdateRegion(x0, y0 + rfbHeaderbarY, w, h);
}


void bx_rfb_gui_c::get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp)
{
  if (desktop_resizable) {
    *xres = BX_RFB_MAX_XDIM;
    *yres = BX_RFB_MAX_YDIM;
  } else {
    *xres = BX_RFB_DEF_XDIM;
    *yres = BX_RFB_DEF_YDIM;
  }
  *bpp = 8;
}

void bx_rfb_gui_c::statusbar_setitem_specific(int element, bx_bool active, bx_bool w)
{
  rfbSetStatusText(element+1, statusitem[element].text, active, w);
}

void bx_rfb_gui_c::set_mouse_mode_absxy(bx_bool mode)
{
  rfbMouseModeAbsXY = mode;
}

#if BX_SHOW_IPS
void bx_rfb_gui_c::show_ips(Bit32u ips_count)
{
  if (!rfbIPSupdate && !rfbHideIPS) {
    ips_count /= 1000;
    sprintf(rfbIPStext, "IPS: %u.%3.3uM", ips_count / 1000, ips_count % 1000);
    rfbIPSupdate = 1;
  }
}
#endif

// RFB specific functions

#ifdef BX_RFB_WIN32
bool InitWinsock()
{
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) return false;
  return true;
}

bool StopWinsock()
{
  WSACleanup();
  return true;
}
#endif

#ifdef WIN32
DWORD WINAPI rfbServerThreadInit(LPVOID)
#else
void CDECL rfbServerThreadInit(void *indata)
#endif
{
    SOCKET             sServer;
    SOCKET             sClient;
    struct sockaddr_in sai;
    unsigned int       sai_size;
    int port_ok = 0;
    int one=1;

#ifdef WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
#endif
#ifdef BX_RFB_WIN32
    if(!InitWinsock()) {
        BX_PANIC(("could not initialize winsock."));
        goto end_of_thread;
    }
#endif

    sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sServer == (SOCKET) -1) {
        BX_PANIC(("could not create socket."));
        goto end_of_thread;
    }
    if (setsockopt(sServer, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(int)) == -1)  {
        BX_PANIC(("could not set socket option."));
        goto end_of_thread;
    }

    for (rfbPort = BX_RFB_PORT_MIN; rfbPort <= BX_RFB_PORT_MAX; rfbPort++) {
      sai.sin_addr.s_addr = INADDR_ANY;
      sai.sin_family      = AF_INET;
      sai.sin_port        = htons(rfbPort);
      BX_INFO (("Trying port %d", rfbPort));
      if(bind(sServer, (struct sockaddr *)&sai, sizeof(sai)) == -1) {
          BX_INFO(("Could not bind socket."));
          continue;
      }
      if(listen(sServer, SOMAXCONN) == -1) {
          BX_INFO(("Could not listen on socket."));
          continue;
      }
      // success
      port_ok = 1;
      break;
    }
    if (!port_ok) {
      BX_PANIC (("RFB could not bind any port between %d and %d",
        BX_RFB_PORT_MIN,
        BX_RFB_PORT_MAX));
      goto end_of_thread;
    }
    BX_INFO (("listening for connections on port %i", rfbPort));
    sai_size = sizeof(sai);
    while (keep_alive) {
        sClient = accept(sServer, (struct sockaddr *)&sai, (socklen_t*)&sai_size);
        if(sClient != INVALID_SOCKET) {
            HandleRfbClient(sClient);
            sGlobal = INVALID_SOCKET;
            close(sClient);
        } else {
            close(sClient);
        }
    }

end_of_thread:
#ifdef BX_RFB_WIN32
    StopWinsock();
#endif
#ifdef WIN32
    return 0;
#else
    return;
#endif
}

void rfbStartThread()
{
#ifdef WIN32
  DWORD threadID;
  CreateThread(NULL, 0, rfbServerThreadInit, NULL, 0, &threadID);
#else
  pthread_t thread;
  pthread_create(&thread, NULL, (void *(*)(void *))&rfbServerThreadInit, NULL);
#endif
}

void HandleRfbClient(SOCKET sClient)
{
  char rfbName[] = "Bochs-RFB";
  rfbProtocolVersionMessage pv;
  int one = 1;
  U32 auth;
  rfbClientInitMessage cim;
  rfbServerInitMessage sim;
  bx_bool mouse_toggle = 0;
  static Bit8u wheel_status = 0;

  setsockopt(sClient, IPPROTO_TCP, TCP_NODELAY, (const char *)&one, sizeof(one));
  BX_INFO(("accepted client connection."));
  snprintf(pv, rfbProtocolVersionMessageSize + 1,
           rfbProtocolVersionFormat,
           rfbServerProtocolMajorVersion,
           rfbServerProtocolMinorVersion);

  if(WriteExact(sClient, pv, rfbProtocolVersionMessageSize) < 0) {
    BX_ERROR(("could not send protocol version."));
    return;
  }
  if(ReadExact(sClient, pv, rfbProtocolVersionMessageSize) < 0) {
    BX_ERROR(("could not receive client protocol version."));
    return;
  }
  pv[rfbProtocolVersionMessageSize-1]=0; // Drop last character
  BX_INFO(("Client protocol version is '%s'", pv));
  // FIXME should check for version number

  auth = htonl(rfbSecurityNone);

  if(WriteExact(sClient, (char *)&auth, sizeof(auth)) < 0) {
    BX_ERROR(("could not send authorization method."));
    return;
  }

  if(ReadExact(sClient, (char *)&cim, rfbClientInitMessageSize) < 0) {
    BX_ERROR(("could not receive client initialization message."));
    return;
  }

  sim.framebufferWidth  = htons((short)rfbWindowX);
  sim.framebufferHeight = htons((short)rfbWindowY);
  sim.serverPixelFormat            = BGR233Format;
  sim.serverPixelFormat.redMax     = htons(sim.serverPixelFormat.redMax);
  sim.serverPixelFormat.greenMax   = htons(sim.serverPixelFormat.greenMax);
  sim.serverPixelFormat.blueMax    = htons(sim.serverPixelFormat.blueMax);
  sim.nameLength = strlen(rfbName);
  sim.nameLength = htonl(sim.nameLength);
  if(WriteExact(sClient, (char *)&sim, rfbServerInitMessageSize) < 0) {
    BX_ERROR(("could send server initialization message."));
    return;
  }
  if(WriteExact(sClient, rfbName, strlen(rfbName)) < 0) {
    BX_ERROR (("could not send server name."));
    return;
  }

  client_connected = 1;
  sGlobal = sClient;
  while (keep_alive) {
    U8 msgType;
    int n;

    if ((n = recv(sClient, (char *)&msgType, 1, MSG_PEEK)) <= 0) {
      if (n == 0) {
        // client closed connection
        client_connected = 0;
      } else {
        if (errno == EINTR)
          continue;
        BX_ERROR(("error receiving data."));
      }
      return;
    }

    switch (msgType) {
      case rfbSetPixelFormat:
        {
          rfbSetPixelFormatMessage spf;
          ReadExact(sClient, (char *)&spf, sizeof(rfbSetPixelFormatMessage));

          spf.pixelFormat.trueColourFlag = (spf.pixelFormat.trueColourFlag ? 1 : 0);
          spf.pixelFormat.bigEndianFlag = (spf.pixelFormat.bigEndianFlag ? 1 : 0);
          spf.pixelFormat.redMax = ntohs(spf.pixelFormat.redMax);
          spf.pixelFormat.greenMax = ntohs(spf.pixelFormat.greenMax);
          spf.pixelFormat.blueMax = ntohs(spf.pixelFormat.blueMax);

          if (!PF_EQ(spf.pixelFormat, BGR233Format)) {
            BX_ERROR(("client has wrong pixel format (%d %d %d %d %d %d %d %d %d %d)",
                      spf.pixelFormat.bitsPerPixel,spf.pixelFormat.depth,spf.pixelFormat.bigEndianFlag,
                      spf.pixelFormat.trueColourFlag,spf.pixelFormat.redMax,spf.pixelFormat.greenMax,
                      spf.pixelFormat.blueMax,spf.pixelFormat.redShift,spf.pixelFormat.greenShift,
                      spf.pixelFormat.blueShift));
            //return;
          }
          break;
        }
      case rfbFixColourMapEntries:
        {
          rfbFixColourMapEntriesMessage fcme;
          ReadExact(sClient, (char *)&fcme, sizeof(rfbFixColourMapEntriesMessage));
          break;
        }
      case rfbSetEncodings:
        {
          rfbSetEncodingsMessage se;
          Bit32u                 i;
          U32                    enc;

          // free previously registered encodings
          if (clientEncodings != NULL) {
            delete [] clientEncodings;
            clientEncodingsCount = 0;
          }

          ReadExact(sClient, (char *)&se, sizeof(rfbSetEncodingsMessage));

          // Alloc new clientEncodings
          clientEncodingsCount = ntohs(se.numberOfEncodings);
          clientEncodings = new Bit32u[clientEncodingsCount];

          for (i = 0; i < clientEncodingsCount; i++) {
            if ((n = ReadExact(sClient, (char *)&enc, sizeof(U32))) <= 0) {
              if (n == 0) {
                // client closed connection
                client_connected = 0;
              } else {
                BX_ERROR(("error receiving data."));
              }
              return;
            }
            clientEncodings[i]=ntohl(enc);
          }

          // print supported encodings
          BX_INFO(("rfbSetEncodings : client supported encodings:"));
          for (i = 0; i < clientEncodingsCount; i++) {
            Bit32u j;
            bx_bool found = 0;
            for (j=0; j < rfbEncodingsCount; j ++) {
              if (clientEncodings[i] == rfbEncodings[j].id) {
                BX_INFO(("%08x %s", rfbEncodings[j].id, rfbEncodings[j].name));
                found=1;
                if (clientEncodings[i] == rfbEncodingDesktopSize) {
                  desktop_resizable = 1;
                }
                break;
              }
            }
            if (!found) BX_INFO(("%08x Unknown", clientEncodings[i]));
          }
          break;
        }
      case rfbFramebufferUpdateRequest:
        {
          rfbFramebufferUpdateRequestMessage fur;

          ReadExact(sClient, (char *)&fur, sizeof(rfbFramebufferUpdateRequestMessage));
          if(!fur.incremental) {
            rfbUpdateRegion.x = 0;
            rfbUpdateRegion.y = 0;
            rfbUpdateRegion.width  = rfbWindowX;
            rfbUpdateRegion.height = rfbWindowY;
            rfbUpdateRegion.updated = 1;
          } //else {
          //    if(fur.x < rfbUpdateRegion.x) rfbUpdateRegion.x = fur.x;
          //    if(fur.y < rfbUpdateRegion.x) rfbUpdateRegion.y = fur.y;
          //    if(((fur.x + fur.w) - rfbUpdateRegion.x) > rfbUpdateRegion.width) rfbUpdateRegion.width = ((fur.x + fur.w) - rfbUpdateRegion.x);
          //    if(((fur.y + fur.h) - rfbUpdateRegion.y) > rfbUpdateRegion.height) rfbUpdateRegion.height = ((fur.y + fur.h) - rfbUpdateRegion.y);
          //}
          //rfbUpdateRegion.updated = 1;
          break;
        }
      case rfbKeyEvent:
        {
          rfbKeyEventMessage ke;
          ReadExact(sClient, (char *)&ke, sizeof(rfbKeyEventMessage));
          ke.key = ntohl(ke.key);
          while (bKeyboardInUse) ;

          if ((ke.key == XK_Control_L) || (ke.key == XK_Control_R)) {
            mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_CTRL, ke.downFlag);
          } else if (ke.key == XK_Alt_L) {
            mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_ALT, ke.downFlag);
          } else if (ke.key == XK_F10) {
            mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_F10, ke.downFlag);
          } else if (ke.key == XK_F12) {
            mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_F12, ke.downFlag);
          }
          if (mouse_toggle) {
            bx_gui->toggle_mouse_enable();
          } else {
            bKeyboardInUse = 1;
            if (rfbKeyboardEvents >= MAX_KEY_EVENTS) break;
            rfbKeyboardEvent[rfbKeyboardEvents].type = KEYBOARD;
            rfbKeyboardEvent[rfbKeyboardEvents].key  = ke.key;
            rfbKeyboardEvent[rfbKeyboardEvents].down = ke.downFlag;
            rfbKeyboardEvents++;
            bKeyboardInUse = 0;
          }
          break;
        }
      case rfbPointerEvent:
        {
          rfbPointerEventMessage pe;
          ReadExact(sClient, (char *)&pe, sizeof(rfbPointerEventMessage));
          while (bKeyboardInUse) ;

          if (bx_gui->mouse_toggle_check(BX_MT_MBUTTON, (pe.buttonMask & 0x02) > 0)) {
            bx_gui->toggle_mouse_enable();
          } else {
            bKeyboardInUse = 1;
            if (rfbKeyboardEvents >= MAX_KEY_EVENTS) break;
            rfbKeyboardEvent[rfbKeyboardEvents].type = MOUSE;
            rfbKeyboardEvent[rfbKeyboardEvents].x    = ntohs(pe.xPosition);
            rfbKeyboardEvent[rfbKeyboardEvents].y    = ntohs(pe.yPosition);
            rfbKeyboardEvent[rfbKeyboardEvents].z    = 0;
            rfbKeyboardEvent[rfbKeyboardEvents].down = (pe.buttonMask & 0x01) |
                                                       ((pe.buttonMask>>1) & 0x02) |
                                                       ((pe.buttonMask<<1) & 0x04);
            if ((pe.buttonMask & 0x18) != wheel_status) {
              if (pe.buttonMask & 0x10) {
                rfbKeyboardEvent[rfbKeyboardEvents].z = -1;
              } else if (pe.buttonMask & 0x08) {
                rfbKeyboardEvent[rfbKeyboardEvents].z = 1;
              }
              wheel_status = pe.buttonMask & 0x18;
            }
            rfbKeyboardEvents++;
            bKeyboardInUse = 0;
          }
          break;
        }
      case rfbClientCutText:
        {
          rfbClientCutTextMessage cct;
          ReadExact(sClient, (char *)&cct, sizeof(rfbClientCutTextMessage));
          break;
        }
    }
  }
}

/*
* ReadExact reads an exact number of bytes on a TCP socket.  Returns 1 if
* those bytes have been read, 0 if the other end has closed, or -1 if an error
* occurred (errno is set to ETIMEDOUT if it timed out).
*/

int ReadExact(int sock, char *buf, int len)
{
    while (len > 0) {
      int n = recv(sock, buf, len, 0);
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

int WriteExact(int sock, char *buf, int len)
{
    while (len > 0) {
      int n = send(sock, buf, len,0);

      if (n > 0) {
        buf += n;
        len -= n;
      } else {
        if (n == 0) BX_ERROR(("WriteExact: write returned 0?"));
        return n;
      }
    }
    return 1;
}

void DrawBitmap(int x, int y, int width, int height, char *bmap,
        char fgcolor, char bgcolor, bx_bool update_client)
{
  unsigned char *newBits;
  newBits = (unsigned char *)malloc(width * height);
  memset(newBits, 0, (width * height));
  for (int i = 0; i < (width * height) / 8; i++) {
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
  free(newBits);
}

void DrawChar(int x, int y, int width, int height, int fonty, char *bmap,
        char fgcolor, char bgcolor, bx_bool gfxchar)
{
  static unsigned char newBits[9 * 32];
  unsigned char mask;
  int bytes = width * height;
  for (int i = 0; i < bytes; i+=width) {
    mask = 0x80;
    for (int j = 0; j < width; j++) {
      if (mask > 0) {
        newBits[i + j] = (bmap[fonty] & mask) ? fgcolor : bgcolor;
      } else {
        if (gfxchar) {
          newBits[i + j] = (bmap[fonty] & 0x01) ? fgcolor : bgcolor;
        } else {
          newBits[i + j] = bgcolor;
        }
      }
      mask >>= 1;
    }
    fonty++;
  }
  UpdateScreen(newBits, x, y, width, height, 0);
}

void UpdateScreen(unsigned char *newBits, int x, int y, int width, int height,
        bx_bool update_client)
{
  int i, x0, y0;
  x0 = x;
  y0 = y;
  if ((unsigned)(x + width - 1) >= rfbWindowX) {
    width = rfbWindowX - x + 1;
  }
  if ((unsigned)(y + height - 1) >= rfbWindowY) {
    height = rfbWindowY - y + 1;
  }
  for (i = 0; i < height; i++) {
    memcpy(&rfbScreen[y * rfbWindowX + x0], &newBits[i * width], width);
    y++;
  }
  if (update_client) {
    if(sGlobal == INVALID_SOCKET) return;
    rfbFramebufferUpdateMessage fum;
    rfbFramebufferUpdateRectHeader furh;
    fum.messageType = rfbFramebufferUpdate;
    fum.numberOfRectangles = htons(1);
    WriteExact(sGlobal, (char *)&fum, rfbFramebufferUpdateMessageSize);
    furh.r.xPosition = htons(x0);
    furh.r.yPosition = htons(y0);
    furh.r.width = htons((short)width);
    furh.r.height = htons((short)height);
    furh.r.encodingType = htonl(rfbEncodingRaw);
    WriteExact(sGlobal, (char *)&furh, rfbFramebufferUpdateRectHeaderSize);
    WriteExact(sGlobal, (char *)newBits, width * height);
  }
}

void SendUpdate(int x, int y, int width, int height, Bit32u encoding)
{
    char *newBits;

    if(x < 0 || y < 0 || (x + width) > (int)rfbWindowX || (y + height) > (int)rfbWindowY) {
        BX_ERROR(("Dimensions out of bounds.  x=%i y=%i w=%i h=%i", x, y, width, height));
    }
    if(sGlobal != INVALID_SOCKET) {
        rfbFramebufferUpdateMessage fum;
        rfbFramebufferUpdateRectHeader furh;

        fum.messageType = rfbFramebufferUpdate;
        fum.numberOfRectangles = htons(1);

        furh.r.xPosition = htons(x);
        furh.r.yPosition = htons(y);
        furh.r.width = htons((short)width);
        furh.r.height = htons((short)height);
        furh.r.encodingType = htonl(encoding);

        WriteExact(sGlobal, (char *)&fum, rfbFramebufferUpdateMessageSize);
        WriteExact(sGlobal, (char *)&furh, rfbFramebufferUpdateRectHeaderSize);

        if (encoding == rfbEncodingRaw) {
          newBits = (char *)malloc(width * height);
          for(int i = 0; i < height; i++) {
            memcpy(&newBits[i * width], &rfbScreen[y * rfbWindowX + x], width);
            y++;
          }
          WriteExact(sGlobal, (char *)newBits, width * height);
          free(newBits);
        }
    }
}

void rfbAddUpdateRegion(unsigned x0, unsigned y0, unsigned w, unsigned h)
{
  if (x0 < rfbUpdateRegion.x) rfbUpdateRegion.x = x0;
  if (y0 < rfbUpdateRegion.y) rfbUpdateRegion.y = y0;
  if ((y0 + h - rfbUpdateRegion.y) > rfbUpdateRegion.height) {
    rfbUpdateRegion.height = y0 + h - rfbUpdateRegion.y;
  }
  if ((x0 + w - rfbUpdateRegion.x) > rfbUpdateRegion.width) {
    rfbUpdateRegion.width = x0 + w - rfbUpdateRegion.x;
  }
  if ((rfbUpdateRegion.x + rfbUpdateRegion.width) > rfbWindowX) {
    rfbUpdateRegion.width = rfbWindowX - rfbUpdateRegion.x;
  }
  rfbUpdateRegion.updated = 1;
}

void rfbSetStatusText(int element, const char *text, bx_bool active, bx_bool w)
{
  char *newBits;
  unsigned xleft, xsize, i, len;

  rfbStatusitemActive[element] = active;
  xleft = rfbStatusitemPos[element] + 2;
  xsize = rfbStatusitemPos[element + 1] - xleft - 1;
  newBits = (char *) malloc(((xsize / 8) + 1) * (rfbStatusbarY - 2));
  memset(newBits, 0, ((xsize / 8) + 1) * (rfbStatusbarY - 2));
  for (i = 0; i < (rfbStatusbarY - 2); i++) {
    newBits[((xsize / 8) + 1) * i] = 0;
  }

  unsigned char fgcolor = active ? headerbar_fg : status_gray_text;
  unsigned char bgcolor = 0;
  if (element > 0) {
    bgcolor = active ? (w ? status_led_red : status_led_green) : headerbar_bg;
  } else {
    bgcolor = headerbar_bg;
  }
  DrawBitmap(xleft, rfbWindowY - rfbStatusbarY + 1, xsize, rfbStatusbarY - 2,
             newBits, fgcolor, bgcolor, 0);

  free(newBits);
  len = ((element > 0) && (strlen(text) > 4)) ? 4 : strlen(text);
  for (i = 0; i < len; i++) {
    DrawChar(xleft + i * 8 + 2, rfbWindowY - rfbStatusbarY + 5, 8, 8, 0,
             (char *) &sdl_font8x8[(unsigned) text[i]][0], fgcolor, bgcolor, 0);
  }

  rfbAddUpdateRegion(xleft, rfbWindowY - rfbStatusbarY + 1, xsize, rfbStatusbarY - 2);
}

// function to convert key names into rfb key values.
// This first try will be horribly inefficient, but it only has
// to be done while loading a keymap.  Once the simulation starts,
// this function won't be called.
static Bit32u convertStringToRfbKey(const char *string)
{
  rfbKeyTabEntry *ptr;
  for (ptr = &rfb_keytable[0]; ptr->name != NULL; ptr++) {
    if (!strcmp(string, ptr->name))
      return ptr->value;
  }
  return BX_KEYMAP_UNKNOWN;
}

static Bit32u rfb_ascii_to_key_event[0x5f] = {
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

  if (!SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
    if ((key >= XK_space) && (key <= XK_asciitilde)) {
      key_event = rfb_ascii_to_key_event[key - XK_space];
    } else {
      switch (key) {
        case XK_KP_1:
#ifdef XK_KP_End
        case XK_KP_End:
#endif
          key_event = BX_KEY_KP_END;
          break;

        case XK_KP_2:
#ifdef XK_KP_Down
        case XK_KP_Down:
#endif
          key_event = BX_KEY_KP_DOWN;
          break;

        case XK_KP_3:
#ifdef XK_KP_Page_Down
        case XK_KP_Page_Down:
#endif
          key_event = BX_KEY_KP_PAGE_DOWN;
          break;

        case XK_KP_4:
#ifdef XK_KP_Left
        case XK_KP_Left:
#endif
          key_event = BX_KEY_KP_LEFT;
          break;

        case XK_KP_5:
#ifdef XK_KP_Begin
        case XK_KP_Begin:
#endif
          key_event = BX_KEY_KP_5;
          break;

        case XK_KP_6:
#ifdef XK_KP_Right
        case XK_KP_Right:
#endif
          key_event = BX_KEY_KP_RIGHT;
          break;

        case XK_KP_7:
#ifdef XK_KP_Home
        case XK_KP_Home:
#endif
          key_event = BX_KEY_KP_HOME;
          break;

        case XK_KP_8:
#ifdef XK_KP_Up
        case XK_KP_Up:
#endif
          key_event = BX_KEY_KP_UP;
          break;

        case XK_KP_9:
#ifdef XK_KP_Page_Up
        case XK_KP_Page_Up:
#endif
          key_event = BX_KEY_KP_PAGE_UP;
          break;

        case XK_KP_0:
#ifdef XK_KP_Insert
        case XK_KP_Insert:
#endif
          key_event = BX_KEY_KP_INSERT;
          break;

        case XK_KP_Decimal:
#ifdef XK_KP_Delete
        case XK_KP_Delete:
#endif
          key_event = BX_KEY_KP_DELETE;
          break;

#ifdef XK_KP_Enter
        case XK_KP_Enter:
          key_event = BX_KEY_KP_ENTER;
          break;
#endif

        case XK_KP_Subtract:
          key_event = BX_KEY_KP_SUBTRACT;
          break;
        case XK_KP_Add:
          key_event = BX_KEY_KP_ADD;
          break;

        case XK_KP_Multiply:
          key_event = BX_KEY_KP_MULTIPLY;
          break;
        case XK_KP_Divide:
          key_event = BX_KEY_KP_DIVIDE;
          break;

        case XK_Up:
          key_event = BX_KEY_UP;
          break;
        case XK_Down:
          key_event = BX_KEY_DOWN;
          break;
        case XK_Left:
          key_event = BX_KEY_LEFT;
          break;
        case XK_Right:
          key_event = BX_KEY_RIGHT;
          break;

        case XK_Delete:
          key_event = BX_KEY_DELETE;
          break;
        case XK_BackSpace:
          key_event = BX_KEY_BACKSPACE;
          break;
        case XK_Tab:
          key_event = BX_KEY_TAB;
          break;
#ifdef XK_ISO_Left_Tab
        case XK_ISO_Left_Tab:
          key_event = BX_KEY_TAB;
          break;
#endif
        case XK_Return:
          key_event = BX_KEY_ENTER;
          break;
        case XK_Escape:
          key_event = BX_KEY_ESC;
          break;
        case XK_F1:
          key_event = BX_KEY_F1;
          break;
        case XK_F2:
          key_event = BX_KEY_F2;
          break;
        case XK_F3:
          key_event = BX_KEY_F3;
          break;
        case XK_F4:
          key_event = BX_KEY_F4;
          break;
        case XK_F5:
          key_event = BX_KEY_F5;
          break;
        case XK_F6:
          key_event = BX_KEY_F6;
          break;
        case XK_F7:
          key_event = BX_KEY_F7;
          break;
        case XK_F8:
          key_event = BX_KEY_F8;
          break;
        case XK_F9:
          key_event = BX_KEY_F9;
          break;
        case XK_F10:
          key_event = BX_KEY_F10;
          break;
        case XK_F11:
          key_event = BX_KEY_F11;
          break;
        case XK_F12:
          key_event = BX_KEY_F12;
          break;
        case XK_Control_L:
          key_event = BX_KEY_CTRL_L;
          break;
#ifdef XK_Control_R
        case XK_Control_R:
          key_event = BX_KEY_CTRL_R;
          break;
#endif
        case XK_Shift_L:
          key_event = BX_KEY_SHIFT_L;
          break;
        case XK_Shift_R:
          key_event = BX_KEY_SHIFT_R;
          break;
        case XK_Alt_L:
          key_event = BX_KEY_ALT_L;
          break;
#ifdef XK_Alt_R
        case XK_Alt_R:
          key_event = BX_KEY_ALT_R;
          break;
#endif
        case XK_Caps_Lock:
          key_event = BX_KEY_CAPS_LOCK;
          break;
        case XK_Num_Lock:
          key_event = BX_KEY_NUM_LOCK;
          break;
#ifdef XK_Scroll_Lock
        case XK_Scroll_Lock:
          key_event = BX_KEY_SCRL_LOCK;
          break;
#endif
#ifdef XK_Print
        case XK_Print:
          key_event = BX_KEY_PRINT;
          break;
#endif
#ifdef XK_Pause
        case XK_Pause:
          key_event = BX_KEY_PAUSE;
          break;
#endif

        case XK_Insert:
          key_event = BX_KEY_INSERT;
          break;
        case XK_Home:
          key_event = BX_KEY_HOME;
          break;
        case XK_End:
          key_event = BX_KEY_END;
          break;
        case XK_Page_Up:
          key_event = BX_KEY_PAGE_UP;
          break;
        case XK_Page_Down:
          key_event = BX_KEY_PAGE_DOWN;
          break;

        default:
          BX_ERROR(("rfbKeyPress(): key %04x unhandled!", key));
          return;
      }
    }
  } else {
    BXKeyEntry *entry = bx_keymap.findHostKey(key);
    if (!entry) {
      BX_ERROR(("rfbKeyPressed(): key %x unhandled!", (unsigned) key));
      return;
    }
    key_event = entry->baseKey;
  }

  if (!press_release)
    key_event |= BX_KEY_RELEASED;
  DEV_kbd_gen_scancode(key_event);
}

void rfbMouseMove(int x, int y, int z, int bmask)
{
  static int oldx = -1;
  static int oldy = -1;
  int dx, dy, xorigin;

  if ((oldx == 1) && (oldy == -1)) {
    oldx = x;
    oldy = y;
    return;
  }
  if (y > rfbHeaderbarY) {
    if (rfbMouseModeAbsXY) {
      if ((y >= rfbHeaderbarY) && (y < (int)(rfbDimensionY + rfbHeaderbarY))) {
        dx = x * 0x7fff / rfbDimensionX;
        dy = (y - rfbHeaderbarY) * 0x7fff / rfbDimensionY;
        DEV_mouse_motion(dx, dy, z, bmask, 1);
      }
    } else {
      DEV_mouse_motion(x - oldx, oldy - y, z, bmask, 0);
    }
    oldx = x;
    oldy = y;
  } else {
    if (bmask == 1) {
      for (unsigned i = 0; i < rfbHeaderbarBitmapCount; i++) {
        if (rfbHeaderbarBitmaps[i].alignment == BX_GRAVITY_LEFT)
          xorigin = rfbHeaderbarBitmaps[i].xorigin;
        else
          xorigin = rfbWindowX - rfbHeaderbarBitmaps[i].xorigin;
        if ((x >= xorigin) &&
            (x < (xorigin + int(rfbBitmaps[rfbHeaderbarBitmaps[i].index].xdim)))) {
          rfbHeaderbarBitmaps[i].f();
          return;
        }
      }
    }
  }
}

#if BX_SHOW_IPS && defined(WIN32)
VOID CALLBACK IPSTimerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
  if (keep_alive) {
    bx_show_ips_handler();
  }
}

DWORD WINAPI rfbShowIPSthread(LPVOID)
{
  MSG msg;

  UINT TimerId = SetTimer(NULL, 0, 1000, &IPSTimerProc);
  while (keep_alive && GetMessage(&msg, NULL, 0, 0)) {
    DispatchMessage(&msg);
  }
  KillTimer(NULL, TimerId);
  return 0;
}
#endif

#endif /* if BX_WITH_RFB */
