/////////////////////////////////////////////////////////////////////////
// $Id: beos.cc,v 1.11 2001-10-03 13:10:37 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
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


#include <app/Application.h>
#include <interface/Window.h>
#include <interface/View.h>
#include <interface/Screen.h>
#include <interface/Bitmap.h>
#include <interface/GraphicsDefs.h>


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "bochs.h"
// #include "icon_bochs.h"
#include "font/vga.bitmap.h"

#define LOG_THIS bx_gui.

#define PAD_NEAREST(n, quantum) (( ((n) + ((quantum) - 1)) / (n) ) * (n))


class BochsApplication : public BApplication {

public:
  BochsApplication();
  };

class BochsWindow : public BWindow {

public:
  BochsWindow(BRect frame);
  virtual  bool  QuitRequested();
  virtual  void  FrameResized(float width, float height);
  };

class BochsView : public BView {
private:
	BBitmap *backing_store;
	BView *backing_view;
	
public:
  BochsView(BRect frame, char *name);
  ~BochsView();
  virtual  void  AttachedToWindow();
  virtual  void  Draw(BRect updateRect);
  virtual  void  KeyDown(const char *bytes, int32 numBytes);
  virtual  void  KeyUp(const char *bytes, int32 numBytes);
  virtual  void  MouseDown(BPoint point);
  virtual  void  MouseUp(BPoint point);
  virtual  void  MouseMoved(BPoint point,
                            uint32 transit, const BMessage *message);
  void DrawBitmap(const BBitmap *aBitmap, BPoint where);							
  void FillRect(BRect r, pattern p = B_SOLID_HIGH);
  void SetHighColor(uchar r, uchar g, uchar b, uchar a = 255);
  void SetLowColor(uchar r, uchar g, uchar b, uchar a = 255);
  void SetFont(const BFont *font, uint32 mask = B_FONT_ALL);
  void set_text_colors(void);
  void set_inv_text_colors(void);
  void set_headerbar_colors(void);
  };


void GUI_end(void);
void   enq_key_event(Bit32u key, Bit32u press_release);
Bit32u deq_key_event(void);
#define SCANCODE_BUFSIZE 20
static Bit32u keyevents[SCANCODE_BUFSIZE];
static unsigned head=0, tail=0;
static bool view_attached = 0;

static BochsApplication *myApplication = NULL;
static BochsWindow      *aWindow = NULL;
static BochsView        *aView = NULL;
static BView            *canvas = NULL;
static uint8 cmap_index[256]; // indeces into system colormap

static BRect rect;
static BPoint image_origin;
static BBitmap *image = NULL;
static BScreen *screen;
static unsigned long rowsize_padded=0;
static uint8 *rawdata = NULL;

//static int rows=25, columns=80;

static unsigned font_width, font_height;
static Bit8u blank_line[80];
static unsigned dimension_x, dimension_y;

//static unsigned imDepth, imWide, imBPP;

// current cursor coordinates
//static int prev_x=-1, prev_y=-1;
//static int current_x=-1, current_y=-1;
//static Bit8u mouse_button_state = 0;

static unsigned prev_block_cursor_x=0;
static unsigned prev_block_cursor_y=0;

static void headerbar_click(int x, int y);

Bit8u ascii_to_scancode[0x7f] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0x39, 0x02, 0x28, 0x04, 0x05, 0x06, 0x08, 0x28, /*  !"#$%&' */
  0x0a, 0x0b, 0x09, 0x0d, 0x33, 0x0c, 0x34, 0x35, /* ()*+,-./ */
  0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, /* 01234567 */
  0x09, 0x0a, 0x27, 0x27, 0x33, 0x0d, 0x34, 0x35, /* 89:;<=>? */
  0x03, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, /* @ABCDEFG */
  0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /* HIJKLMNO */
  0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, /* PQRSTUVW */
  0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x07, 0x0c, /* XYZ[\]^_ */
  0x29, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, /* `abcdefg */
  0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /* hijklmno */
  0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, /* pqrstuvw */
  0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x29        /* xyz{|}~  */
  };

extern Bit8u graphics_snapshot[32 * 1024];
//extern pc_keyb_c *pc_keyboard;




unsigned col_vals[256], curr_foreground, curr_background;

static unsigned x_tilesize, y_tilesize;

struct {
  BBitmap *bmap;
  unsigned xdim;
  unsigned ydim;
  } bx_bitmaps[BX_MAX_PIXMAPS];
unsigned bx_bitmap_entries = 0;

static struct {
  BBitmap   *bitmap;
  unsigned xdim;
  unsigned ydim;
  unsigned xorigin;
  unsigned yorigin;
  unsigned alignment;
  void (*f)(void);
  } bx_headerbar_entry[BX_MAX_HEADERBAR_ENTRIES];
static unsigned bx_headerbar_y = 0;
static unsigned bx_headerbar_entries = 0;
static unsigned bx_bitmap_left_xorigin = 0;  // pixels from left
static unsigned bx_bitmap_right_xorigin = 0; // pixels from right

static void headerbar_click(int x, int y);
static unsigned char reverse_bitorder(unsigned char b);
static void create_vga_font(void);
static BBitmap *vgafont[256];

static bx_gui_c *bx_gui_c_ptr;

  void
bx_gui_c::specific_init(bx_gui_c *th, int argc, char **argv,
                        unsigned tilewidth, unsigned tileheight,
                        unsigned header_bar_y)
{
  unsigned i;
#if BX_CPU_LEVEL < 2
  char *window_name = "Bochs 8086 emulator, http://world.std.com/~bochs";
#elif BX_CPU_LEVEL == 2
  char *window_name = "Bochs 80286 emulator, http://world.std.com/~bochs";
#elif BX_CPU_LEVEL == 3
  char *window_name = "Bochs 80386 emulator, http://world.std.com/~bochs";
#else
  char *window_name = "Bochs 80386+ emulator, http://world.std.com/~bochs";
#endif

  bx_gui_c_ptr = th;
UNUSED(argc);
UNUSED(argv);
UNUSED(window_name);
  th->put("BGUI");

if (bx_options.Oprivate_colormap->get ()) {
  BX_INFO(( "BeOS: private_colormap option not handled yet."));
  }

  x_tilesize = tilewidth;
  y_tilesize = tileheight;
  bx_headerbar_y = header_bar_y;

  for (i=0; i<80; i++) {
    blank_line[i] = ' ';
    }


  font_width = 8;
  font_height = 16;

  // width = columns * font_width;
  // height = rows * font_height;

BX_INFO(("font_width = %u", (unsigned) font_width));
BX_INFO(("font_height = %u", (unsigned) font_height));

  // Create pixmap of depth 1 (bitmap) for icon
  // icon_pixmap = XCreateBitmapFromData(bx_x_display, win,
  //   (char *) bochs_icon_bits, bochs_icon_width, bochs_icon_height);

  curr_background = 0;
  // XSetBackground(bx_x_display, gc, col_vals[curr_background]);
  curr_foreground = 1;
  // XSetForeground(bx_x_display, gc, col_vals[curr_foreground]);
  //XGrabPointer( bx_x_display, win, True, 0, GrabModeAsync, GrabModeAsync,
  //  win, None, CurrentTime );
  //XFlush(bx_x_display);

  myApplication = new BochsApplication();

  // wait until window on screen
  while (1) {
    aWindow->Lock();
    if (aWindow->IsHidden()) {
     aWindow->Unlock();
     continue;
     }
    aWindow->Unlock();
    break;
    }

  screen = new BScreen;
  // start out with all color map indeces pointing to Black
  cmap_index[0] = screen->IndexForColor(0, 0, 0);
  for (unsigned i=1; i<256; i++) {
    cmap_index[i] = cmap_index[0];
    }

  rect.Set(0,0, x_tilesize-1, y_tilesize-1);
  image = new BBitmap(rect, B_COLOR_8_BIT);
  rowsize_padded = PAD_NEAREST(x_tilesize, sizeof(int32));
  rawdata = new uint8[rowsize_padded * y_tilesize];

  create_vga_font();
}



  void
bx_gui_c::handle_events(void)
{
  Bit32u key;

  while ( head != tail ) {
    key = deq_key_event();
    bx_devices.keyboard->gen_scancode(key);
    }
}

  void
bx_gui_c::flush(void)
{
  if (view_attached)
    aView->Flush();
}


  void
bx_gui_c::clear_screen(void)
{
  aWindow->Lock();
  aView->FillRect(BRect(0, bx_headerbar_y, dimension_x-1, dimension_y-1), B_SOLID_LOW);
  aWindow->Unlock();
}



  void
bx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                      unsigned long cursor_x, unsigned long cursor_y,
                      unsigned nrows)
{
  unsigned i, x, y;
  BPoint point;
  unsigned char achar;
  unsigned nchars;

  aWindow->Lock();

  // Number of characters on screen, variable number of rows
  nchars = 80*nrows;

  // first draw over character at original block cursor location
  if ( (prev_block_cursor_y*80 + prev_block_cursor_x) < nchars ) {
    achar = new_text[(prev_block_cursor_y*80 + prev_block_cursor_x)*2];
    point.Set(prev_block_cursor_x*8, prev_block_cursor_y*16 + bx_headerbar_y);
    aView->DrawBitmap(vgafont[achar], point );
    }

  for (i=0; i<nchars*2; i+=2) {
    if ( (old_text[i]!=new_text[i]) ||
         (old_text[i+1]!=new_text[i+1]) ) {

      achar = new_text[i];

      x = (i/2) % 80;
      y = (i/2) / 80;

      point.Set(x*8, y*16 + bx_headerbar_y);
      aView->DrawBitmap(vgafont[achar], point );
      }
    }

  prev_block_cursor_x = cursor_x;
  prev_block_cursor_y = cursor_y;

  // now draw character at new block cursor location in reverse
  if ( (cursor_y*80 + cursor_x) < nchars ) {
    achar = new_text[(cursor_y*80 + cursor_x)*2];
    point.Set(cursor_x*8, cursor_y*16 + bx_headerbar_y);
    aView->set_inv_text_colors();
    aView->DrawBitmap(vgafont[achar], point );
    aView->set_text_colors();
    }

  aWindow->Unlock();
}


  void
bx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  for (unsigned y=0; y<y_tilesize; y++) {
    for (unsigned x=0; x<x_tilesize; x++) {
      rawdata[y*rowsize_padded + x] = cmap_index[ tile[y*x_tilesize + x] ];
      }
    }
  aWindow->Lock();
  image->SetBits(rawdata, rowsize_padded * y_tilesize, 0, B_COLOR_8_BIT);
  image_origin.Set(x0, y0 + bx_headerbar_y);

  aView->DrawBitmap( image, image_origin );
  aWindow->Unlock();
}


  Boolean
bx_gui_c::palette_change(unsigned index, unsigned red, unsigned green, unsigned blue)
{
  cmap_index[index] = screen->IndexForColor(
    red, green, blue);
  return(1);
}


  void
bx_gui_c::dimension_update(unsigned x, unsigned y)
{
  aWindow->Lock();
  aWindow->ResizeTo(x, y + bx_headerbar_y);
  aWindow->Unlock();
}


//////////////////////////////////////////////////////


  void
GUI_end()
{
  //myApplication->Run();

  delete(myApplication);
}

BochsApplication::BochsApplication()
            : BApplication("application/x-vnd.Be-Bochs")
{
  BRect      aRect;

  int left, right, top, bottom;
  left = 5;
  top = 80;
  dimension_x = 640;
  dimension_y = 480 + bx_headerbar_y;
  right = left + dimension_x - 1;
  bottom = top + dimension_y - 1;

  // set up a rectangle and instantiate a new window
  aRect.Set(left, top, right, bottom);
  aWindow = new BochsWindow(aRect);

  // set up a rectangle and instantiate a new view
  // view rect should be same size as window rect but with left top at (0, 0)
  aRect.OffsetTo(B_ORIGIN);
  aView = new BochsView(aRect, "BochsView");
  aView->SetViewColor(0, 0, 0);

  aView->set_text_colors();

  // add view to window
  aWindow->AddChild(aView);

  // make window visible
  aWindow->Show();
}


//void set_palette_entry(long i,rgb_color c);

BochsWindow::BochsWindow(BRect frame)
        : BWindow(frame, "Bochs", B_TITLED_WINDOW, B_NOT_RESIZABLE)
{
}

bool BochsWindow::QuitRequested()
{
  be_app->PostMessage(B_QUIT_REQUESTED);
  return(TRUE);
}


  void
BochsWindow::FrameResized(float width, float height)
{
  dimension_x = unsigned(width);
  dimension_y = unsigned(height) + bx_headerbar_y;
  if ( bx_gui_c_ptr )
    bx_gui_c_ptr->show_headerbar();
}


//------------------------------


BochsView::BochsView(BRect rect, char *name)
          : BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
	backing_store = new BBitmap(rect, B_BITMAP_ACCEPTS_VIEWS, B_COLOR_8_BIT);
	backing_view = new BView(rect,"backing store", B_FOLLOW_ALL, B_WILL_DRAW);
	backing_store->AddChild(backing_view);
}

  void
BochsView::set_text_colors(void)
{
  aView->SetHighColor(255, 255, 255);
  aView->SetLowColor(0, 0, 0);
}

  void
BochsView::set_inv_text_colors(void)
{
  aView->SetHighColor(0, 0, 0);
  aView->SetLowColor(255, 255, 255);
}

  void
BochsView::set_headerbar_colors(void)
{
  aView->SetHighColor(0, 0, 0);
  aView->SetLowColor(255, 255, 255);
}

void BochsView::AttachedToWindow()
{
  BFont font(be_fixed_font);

  aWindow->Lock();
  font.SetSize(16);
  font.SetFlags(B_DISABLE_ANTIALIASING);
  aView->SetFont(&font);

  aView->MakeFocus(true);
  aWindow->Unlock();

  view_attached = 1;
}

void  BochsView::MouseDown(BPoint point)
{
  UNUSED(point);
  if (point.y < BX_HEADER_BAR_Y) {
    headerbar_click(int(point.x), int(point.y));
    return;
    }
   BX_DEBUG(("mousedown()"));
}

void  BochsView::MouseUp(BPoint point)
{
  UNUSED(point);
  // currently a place holder function
  BX_DEBUG(( "mouseup()" ));
}

void  BochsView::MouseMoved(BPoint point,
                          uint32 transit, const BMessage *message)
{
  UNUSED(point);
  UNUSED(transit);
  UNUSED(message);
  BX_DEBUG(( "mousemoved()" ));
}

void BochsView::KeyDown(const char *bytes, int32 numBytes)
{
  BMessage *msg;
  //int32 key;
  int32 modifiers;

  uint8 byte;

  msg = Window()->CurrentMessage();
  if ( !msg ) {
    BX_DEBUG(( "keydown() msg NULL" ));
    return;
    }
  modifiers = msg->FindInt32("modifiers");
#if 0
  B_SHIFT_KEY          = 0x00000001,
  B_COMMAND_KEY        = 0x00000002,
  B_CONTROL_KEY        = 0x00000004,
  B_CAPS_LOCK          = 0x00000008,
  B_SCROLL_LOCK        = 0x00000010,
  B_NUM_LOCK           = 0x00000020,
  B_OPTION_KEY         = 0x00000040,
  B_MENU_KEY           = 0x00000080,
  B_LEFT_SHIFT_KEY     = 0x00000100,
  B_RIGHT_SHIFT_KEY    = 0x00000200,
  B_LEFT_COMMAND_KEY   = 0x00000400,
  B_RIGHT_COMMAND_KEY  = 0x00000800,
  B_LEFT_CONTROL_KEY   = 0x00001000,
  B_RIGHT_CONTROL_KEY  = 0x00002000,
  B_LEFT_OPTION_KEY    = 0x00004000,
  B_RIGHT_OPTION_KEY   = 0x00008000
#endif
//if (modifiers) {
//  fprintf(stderr, "# modifiers = %08x\n", (unsigned) modifiers);
//  }

  if (numBytes == 1) {
    //fprintf(stderr, "# down: char %02xh\n", (unsigned) bytes[0]);
    byte = bytes[0];
    if ( byte == 0x00 ) {
      // Ctrl-Space
      enq_key_event(BX_KEY_CTRL_L,  BX_KEY_PRESSED);
      enq_key_event(BX_KEY_SPACE, BX_KEY_PRESSED);
      enq_key_event(BX_KEY_SPACE, BX_KEY_RELEASED);
      enq_key_event(BX_KEY_CTRL_L,  BX_KEY_RELEASED);
      return;
      }
    if ( byte == 0x1b ) {
      // Esc
      if (modifiers & B_CONTROL_KEY)
        enq_key_event(BX_KEY_CTRL_L,  BX_KEY_PRESSED);
      enq_key_event(BX_KEY_ESC, BX_KEY_PRESSED);
      enq_key_event(BX_KEY_ESC, BX_KEY_RELEASED);
      if (modifiers & B_CONTROL_KEY)
        enq_key_event(BX_KEY_CTRL_L,  BX_KEY_RELEASED);
      return;
      }
    if ( (byte >= 0x30) && (byte <= 0x39) ) {
      // 0 .. 9
      byte -= 0x30;
      enq_key_event(BX_KEY_0 + byte, BX_KEY_PRESSED);
      enq_key_event(BX_KEY_0 + byte, BX_KEY_RELEASED);
      return;
      }
    if ( (byte >= 0x41) && (byte <= 0x5A) ) {
      // A .. Z
      byte -= 0x41;
      enq_key_event(BX_KEY_SHIFT_L,    BX_KEY_PRESSED);
      enq_key_event(BX_KEY_A + byte, BX_KEY_PRESSED);
      enq_key_event(BX_KEY_A + byte, BX_KEY_RELEASED);
      enq_key_event(BX_KEY_SHIFT_L,    BX_KEY_RELEASED);
      return;
      }
    if ( (byte >= 0x61) && (byte <= 0x7A) ) {
      // a .. z
      byte -= 0x61;
      enq_key_event(BX_KEY_A + byte, BX_KEY_PRESSED);
      enq_key_event(BX_KEY_A + byte, BX_KEY_RELEASED);
      return;
      }
    switch (byte) {
      case 0x20: // Space
        enq_key_event(BX_KEY_SPACE, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SPACE, BX_KEY_RELEASED);
        break;
      case 0x21: // Exclamation Point
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_1,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_1,     BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x22: // Double Quotes
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SINGLE_QUOTE, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SINGLE_QUOTE, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x23: // Pound Sign
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_3,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_3,     BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x24: // Dollar Sign
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_4,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_4,     BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x25: // Percent
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_5,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_5,     BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x26: // Ampersand
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_7,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_7,     BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x27: // Single Quote
        enq_key_event(BX_KEY_SINGLE_QUOTE, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SINGLE_QUOTE, BX_KEY_RELEASED);
        break;
      case 0x28: // Left Paren
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_9,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_9,     BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x29: // Right Paren
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_0,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_0,     BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x2a: // Multiply
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_8,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_8,     BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x2b: // Add
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_PRESSED);
        enq_key_event(BX_KEY_EQUALS, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_EQUALS, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_RELEASED);
        break;
      case 0x2c: // Comma
        enq_key_event(BX_KEY_COMMA, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_COMMA, BX_KEY_RELEASED);
        break;
      case 0x2d: // Minus
        enq_key_event(BX_KEY_MINUS, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_MINUS, BX_KEY_RELEASED);
        break;
      case 0x2e: // Period
        enq_key_event(BX_KEY_PERIOD, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_PERIOD, BX_KEY_RELEASED);
        break;
      case 0x2f: // Slash
        enq_key_event(BX_KEY_SLASH, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SLASH, BX_KEY_RELEASED);
        break;

      case 0x3a: // Colon
        enq_key_event(BX_KEY_SHIFT_L,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SEMICOLON, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SEMICOLON, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,     BX_KEY_RELEASED);
        break;
      case 0x3b: // Semi-Colon
        enq_key_event(BX_KEY_SEMICOLON, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SEMICOLON, BX_KEY_RELEASED);
        break;
      case 0x3c: // Less Than
        enq_key_event(BX_KEY_SHIFT_L,     BX_KEY_PRESSED);
        enq_key_event(BX_KEY_COMMA, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_COMMA, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,     BX_KEY_RELEASED);
        break;
      case 0x3d: // Equals
        enq_key_event(BX_KEY_EQUALS, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_EQUALS, BX_KEY_RELEASED);
        break;
      case 0x3e: // Greater Than
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_PRESSED);
        enq_key_event(BX_KEY_PERIOD, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_PERIOD, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_RELEASED);
        break;
      case 0x3f: // Question Mark
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SLASH, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_SLASH, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_RELEASED);
        break;
      case 0x40: // At Sign
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_PRESSED);
        enq_key_event(BX_KEY_2,      BX_KEY_PRESSED);
        enq_key_event(BX_KEY_2,      BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_RELEASED);
        break;

      case 0x5b: // Left Bracket
        enq_key_event(BX_KEY_LEFT_BRACKET, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_LEFT_BRACKET, BX_KEY_RELEASED);
        break;
      case 0x5c: // Back Slash
        enq_key_event(BX_KEY_BACKSLASH, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_BACKSLASH, BX_KEY_RELEASED);
        break;
      case 0x5d: // Right Bracket
        enq_key_event(BX_KEY_RIGHT_BRACKET, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_RIGHT_BRACKET, BX_KEY_RELEASED);
        break;
      case 0x5e: // Caret
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_PRESSED);
        enq_key_event(BX_KEY_6,      BX_KEY_PRESSED);
        enq_key_event(BX_KEY_6,      BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_RELEASED);
        break;
      case 0x5f: // Underscore
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_MINUS, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_MINUS, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;
      case 0x60: // Grave Accent
        enq_key_event(BX_KEY_GRAVE, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_GRAVE, BX_KEY_RELEASED);
        break;

      case 0x7b: // Left Brace
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_PRESSED);
        enq_key_event(BX_KEY_LEFT_BRACKET, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_LEFT_BRACKET, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_RELEASED);
        break;
      case 0x7c: // Verticle Bar
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_PRESSED);
        enq_key_event(BX_KEY_BACKSLASH, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_BACKSLASH, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,  BX_KEY_RELEASED);
        break;
      case 0x7d: // Right Brace
        enq_key_event(BX_KEY_SHIFT_L,         BX_KEY_PRESSED);
        enq_key_event(BX_KEY_RIGHT_BRACKET, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_RIGHT_BRACKET, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L,         BX_KEY_RELEASED);
        break;
      case 0x7e: // Tilde
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_GRAVE, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_GRAVE, BX_KEY_RELEASED);
        enq_key_event(BX_KEY_SHIFT_L, BX_KEY_RELEASED);
        break;

      case B_BACKSPACE:
        enq_key_event(BX_KEY_BACKSPACE, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_BACKSPACE, BX_KEY_RELEASED);
        break;
      case B_ENTER: //case B_RETURN:
        enq_key_event(BX_KEY_ENTER, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_ENTER, BX_KEY_RELEASED);
        break;
      case B_TAB:
        enq_key_event(BX_KEY_TAB, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_TAB, BX_KEY_RELEASED);
        break;
      case B_LEFT_ARROW:
        enq_key_event(BX_KEY_LEFT, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_LEFT, BX_KEY_RELEASED);
        break;
      case B_RIGHT_ARROW:
        enq_key_event(BX_KEY_RIGHT, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_RIGHT, BX_KEY_RELEASED);
        break;
      case B_UP_ARROW:
        enq_key_event(BX_KEY_UP, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_UP, BX_KEY_RELEASED);
        break;
      case B_DOWN_ARROW:
        enq_key_event(BX_KEY_DOWN, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_DOWN, BX_KEY_RELEASED);
        break;
#if 0
      case B_FUNCTION_KEY: break;
        msg->FindInt32("key", &key);
        switch ( key ) {
          case B_F1_KEY: break;
          case B_F2_KEY: break;
          case B_F3_KEY: break;
          case B_F4_KEY: break;
          case B_F5_KEY: break;
          case B_F6_KEY: break;
          case B_F7_KEY: break;
          case B_F8_KEY: break;
          case B_F9_KEY: break;
          case B_F10_KEY: break;
          case B_F11_KEY: break;
          case B_F12_KEY: break;
          case B_PRINT_KEY: break;
          case B_SCROLL_KEY: break;
          case B_PAUSE_KEY: break;
          default:
            fprintf(stderr, "# keydown() unknown function key %08xh\n",
              (unsigned) key);
          }
#endif
      case B_INSERT: break;
        enq_key_event(BX_KEY_INSERT, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_INSERT, BX_KEY_RELEASED);
        break;
      case B_DELETE: break;
        enq_key_event(BX_KEY_DELETE, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_DELETE, BX_KEY_RELEASED);
        break;
      case B_HOME: break;
        enq_key_event(BX_KEY_HOME, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_HOME, BX_KEY_RELEASED);
        break;
      case B_END: break;
        enq_key_event(BX_KEY_END, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_END, BX_KEY_RELEASED);
        break;
      case B_PAGE_UP: break;
        enq_key_event(BX_KEY_PAGE_UP, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_PAGE_UP, BX_KEY_RELEASED);
        break;
      case B_PAGE_DOWN: break;
        enq_key_event(BX_KEY_PAGE_DOWN, BX_KEY_PRESSED);
        enq_key_event(BX_KEY_PAGE_DOWN, BX_KEY_RELEASED);
        break;

      default:
        if ( (byte >= 0x01) && (byte <= 0x1a) ) {
          // If the above keys dont catch this case, synthesize a
          // Ctrl-A .. Ctrl-Z event
          byte -= 1;
          enq_key_event(BX_KEY_CTRL_L,     BX_KEY_PRESSED);
          enq_key_event(BX_KEY_A + byte, BX_KEY_PRESSED);
          enq_key_event(BX_KEY_A + byte, BX_KEY_RELEASED);
          enq_key_event(BX_KEY_CTRL_L,     BX_KEY_RELEASED);
          return;
          }
        fprintf(stderr, "# keydown: char %02xh unhandled\n",
          (unsigned) bytes[0]);
        return;
      }
    return;
    }
  else {
    // ignore for now
    fprintf(stderr, "# keydown() ignoring multibyte key\n");
    }
}

void BochsView::KeyUp(const char *bytes, int32 numBytes)
{
  UNUSED(bytes);
  UNUSED(numBytes);
}

void BochsView::Draw(BRect rect)
{
	DrawBitmapAsync(backing_store, rect, rect);
}

BochsView::~BochsView()
{
	delete backing_store;
	delete backing_view;
}

void 
BochsView::SetFont(const BFont *font, uint32 mask)
{
	backing_store->Lock();
	backing_view->SetFont(font,mask);
	backing_store->Unlock();
}

void 
BochsView::SetHighColor(uchar r, uchar g, uchar b, uchar a)
{
	backing_store->Lock();
	backing_view->SetHighColor(r,g,b,a);
	backing_store->Unlock();
}

void 
BochsView::SetLowColor(uchar r, uchar g, uchar b, uchar a)
{
	backing_store->Lock();
	backing_view->SetLowColor(r,g,b,a);
	backing_store->Unlock();
}

void 
BochsView::DrawBitmap(const BBitmap *aBitmap, BPoint where)
{
	backing_store->Lock();
	backing_view->DrawBitmap(aBitmap,where);
	backing_store->Unlock();
	BRect r = aBitmap->Bounds();
	r.OffsetBy(where);
	Invalidate(r);
}

void 
BochsView::FillRect(BRect r, pattern p)
{
	backing_store->Lock();
	backing_view->FillRect(r,p);
	backing_store->Unlock();
	Invalidate(r);
}



  void
enq_key_event(Bit32u key, Bit32u press_release)
{
  if ( ((tail+1) % SCANCODE_BUFSIZE) == head ) {
    fprintf(stderr, "# enq_key_event: buffer full\n");
    return;
    }
  keyevents[tail] = key | press_release;
  tail = (tail + 1) % SCANCODE_BUFSIZE;
}

  Bit32u
deq_key_event(void)
{
  Bit32u key;

  if ( head == tail ) {
    fprintf(stderr, "# deq_key_event: buffer empty\n");
    return(0);
    }
  key = keyevents[head];
  head = (head + 1) % SCANCODE_BUFSIZE;
  return(key);
}

  unsigned char
reverse_bitorder(unsigned char b)
{
  unsigned char ret=0;
  for (unsigned i=0; i<8; i++) {
    ret |= (b & 0x01) << (7-i);
    b >>= 1;
    }
  return(ret);
}

  unsigned
bx_gui_c::create_bitmap(const unsigned char *bmap,
                        unsigned xdim, unsigned ydim)
{
  BRect rect(0.0, 0.0, xdim-1, ydim-1);
  int32 bitslength;
  //int32 bytesperrow;
  unsigned char *data;

  if (bx_bitmap_entries >= BX_MAX_PIXMAPS) {
    BX_PANIC(("beos: too many pixmaps, increase BX_MAX_PIXMAPS"));
    }

  bx_bitmaps[bx_bitmap_entries].bmap =
    new BBitmap(rect, B_MONOCHROME_1_BIT);

  bitslength = bx_bitmaps[bx_bitmap_entries].bmap->BitsLength();
  //bytesperrow = bx_bitmaps[bx_bitmap_entries].bmap->BytesPerRow();
  data = (unsigned char *) bx_bitmaps[bx_bitmap_entries].bmap->Bits();
  for (int32 i=0; i<bitslength; i++) {
    data[i] = reverse_bitorder(bmap[i]);
    }

  bx_bitmaps[bx_bitmap_entries].xdim = xdim;
  bx_bitmaps[bx_bitmap_entries].ydim = ydim;
  if (!bx_bitmaps[bx_bitmap_entries].bmap) {
    BX_PANIC(("beos: could not create bitmap"));
    }
  bx_bitmap_entries++;
  return(bx_bitmap_entries-1); // return index as handle
}

  unsigned
bx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment,
                           void (*f)(void))
{
  unsigned hb_index;

  if ( (bx_headerbar_entries+1) > BX_MAX_HEADERBAR_ENTRIES )
    BX_PANIC(("beos: too many headerbar entries, increase BX_MAX_HEADERBAR_ENTRIES"));

  bx_headerbar_entries++;
  hb_index = bx_headerbar_entries - 1;

  bx_headerbar_entry[hb_index].bitmap = bx_bitmaps[bmap_id].bmap;
  bx_headerbar_entry[hb_index].xdim   = bx_bitmaps[bmap_id].xdim;
  bx_headerbar_entry[hb_index].ydim   = bx_bitmaps[bmap_id].ydim;
  bx_headerbar_entry[hb_index].alignment = alignment;
  bx_headerbar_entry[hb_index].f = f;
  if (alignment == BX_GRAVITY_LEFT) {
    bx_headerbar_entry[hb_index].xorigin = bx_bitmap_left_xorigin;
    bx_headerbar_entry[hb_index].yorigin = 0;
    bx_bitmap_left_xorigin += bx_bitmaps[bmap_id].xdim;
    }
  else { // BX_GRAVITY_RIGHT
    bx_bitmap_right_xorigin += bx_bitmaps[bmap_id].xdim;
    bx_headerbar_entry[hb_index].xorigin = bx_bitmap_right_xorigin;
    bx_headerbar_entry[hb_index].yorigin = 0;
    }
  return(hb_index);
}

  void
bx_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  unsigned xorigin;

  bx_headerbar_entry[hbar_id].bitmap = bx_bitmaps[bmap_id].bmap;

  if (bx_headerbar_entry[hbar_id].alignment == BX_GRAVITY_LEFT)
    xorigin = bx_headerbar_entry[hbar_id].xorigin;
  else
    xorigin = dimension_x - bx_headerbar_entry[hbar_id].xorigin;
  aWindow->Lock();
  aView->set_headerbar_colors();
  aView->DrawBitmap(bx_headerbar_entry[hbar_id].bitmap, BPoint(xorigin, 0));
  aView->set_text_colors();
  aWindow->Unlock();
}

  void
bx_gui_c::show_headerbar(void)
{
  unsigned xorigin;
  BPoint origin;

  // clear header bar area to white

  aWindow->Lock();
  aView->set_headerbar_colors();
  aView->FillRect(BRect(0, 0, dimension_x-1, bx_headerbar_y-1), B_SOLID_LOW);
  for (unsigned i=0; i<bx_headerbar_entries; i++) {
    if (bx_headerbar_entry[i].alignment == BX_GRAVITY_LEFT)
      xorigin = bx_headerbar_entry[i].xorigin;
    else
      xorigin = dimension_x - bx_headerbar_entry[i].xorigin;
    origin.Set(xorigin, 0);
    aView->DrawBitmap( bx_headerbar_entry[i].bitmap, origin );
    }
  aView->set_text_colors();
  aWindow->Unlock();
}


  void
headerbar_click(int x, int y)
{
  int xorigin;

  // assuming y is in bounds
  UNUSED(y);
  for (unsigned i=0; i<bx_headerbar_entries; i++) {
    if (bx_headerbar_entry[i].alignment == BX_GRAVITY_LEFT)
      xorigin = bx_headerbar_entry[i].xorigin;
    else
      xorigin = dimension_x - bx_headerbar_entry[i].xorigin;
    if ( (x>=xorigin) && (x<(xorigin+int(bx_headerbar_entry[i].xdim))) ) {
      bx_headerbar_entry[i].f();
      return;
      }
    }
}


  void
create_vga_font(void)
{
  int32 bitslength;
  int32 bytesperrow;
  unsigned char *data;
  BRect brect(0,0, 7,15);

  BX_INFO(( "BeOS: creating VGA font from bitmaps" ));

  // VGA font is 8wide x 16high
  for (unsigned c=0; c<256; c++) {
    vgafont[c] = new BBitmap(brect, B_MONOCHROME_1_BIT);
    if (!vgafont[c]) {
      BX_PANIC(("beos: could not create bitmap"));
      }

    bitslength = vgafont[c]->BitsLength();
    bytesperrow = vgafont[c]->BytesPerRow();
    data = (unsigned char *) vgafont[c]->Bits();
    memset(data, 0, bitslength);
    for (unsigned i=0; i<16; i++) {
      data[i*bytesperrow] = reverse_bitorder(bx_vgafont[c].data[i]);
      }
    }
}


  void
bx_gui_c::exit(void)
{
  fprintf(stderr, "# WARNING: BEOS: bx_gui_c::exit() not implemented yet.\n");
}

  void
bx_gui_c::mouse_enabled_changed_specific (Boolean val)
{
}
