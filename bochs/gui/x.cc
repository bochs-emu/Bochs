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


extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
}

#include "bochs.h"
#include "icon_bochs.h"

#define LOG_THIS bx_gui.


#define MAX_MAPPED_STRING_LENGTH 10

/* These are used as arguments to nearly every Xlib routine, so it saves
 * routine arguments to declare them global.  If there were
 * additional source files, they would be declared extern there. */
Display *bx_x_display;
int bx_x_screen_num;
static Colormap default_cmap;
static unsigned long white_pixel=0, black_pixel=0;

static char *progname; /* name this program was invoked by */

static int rows=25, columns=80;

static Window win;
static GC gc, gc_inv, gc_headerbar, gc_headerbar_inv;
static XFontStruct *font_info;
static unsigned font_width, font_height;
static Bit8u blank_line[80];
static unsigned dimension_x=0, dimension_y=0;

static XImage *ximage = NULL;
static unsigned imDepth, imWide, imBPP;

// current cursor coordinates
static int prev_x=-1, prev_y=-1;
static int current_x=-1, current_y=-1;
static unsigned mouse_button_state = 0;

static unsigned prev_block_cursor_x=0;
static unsigned prev_block_cursor_y=0;

static int warp_home_x = 200;
static int warp_home_y = 200;
static int mouse_enable_x = 0;
static int mouse_enable_y = 0;
static int warp_dx = 0;
static int warp_dy = 0;

static void warp_cursor(int dx, int dy);
static void disable_cursor();
static void enable_cursor();

struct {
  Pixmap bmap;
  unsigned xdim;
  unsigned ydim;
  } bx_bitmaps[BX_MAX_PIXMAPS];
unsigned bx_bitmap_entries = 0;

static struct {
  Pixmap   bitmap;
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
static void send_keyboard_mouse_status(void);




Bit32u ascii_to_key_event[0x5f] = {
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

extern Bit8u graphics_snapshot[32 * 1024];


static void load_font(void);
static void xkeypress(KeySym keysym, int press_release);
// extern "C" void select_visual(void);

#define ROUNDUP(nbytes, pad) ((((nbytes) + ((pad)-1)) / (pad)) * ((pad)>>3))



unsigned long col_vals[256]; // 256 VGA colors
unsigned curr_foreground, curr_background;

static unsigned x_tilesize, y_tilesize;




  void
bx_gui_c::specific_init(bx_gui_c *th, int argc, char **argv, unsigned tilewidth, unsigned tileheight,
                     unsigned headerbar_y)
{
  unsigned i;
  int x, y;   /* window position */
  unsigned int border_width = 4;  /* four pixels */
#if BX_CPU_LEVEL < 2
  char *window_name = "Bochs 8086 emulator, http://www.bochs.com/";
#elif BX_CPU_LEVEL == 2
  char *window_name = "Bochs 80286 emulator, http://www.bochs.com/";
#elif BX_CPU_LEVEL == 3
  char *window_name = "Bochs 80386 emulator, http://www.bochs.com/";
#elif BX_CPU_LEVEL == 4
  char *window_name = "Bochs 80486 emulator, http://www.bochs.com/";
#else
  char *window_name = "Bochs Pentium emulator, http://www.bochs.com/";
#endif
  char *icon_name = "Bochs";
  Pixmap icon_pixmap;
  XSizeHints size_hints;
  char *display_name = NULL;
  /* create GC for text and drawing */
  unsigned long valuemask = 0; /* ignore XGCvalues and use defaults */
  XGCValues values;
  Visual  *default_visual;
  int      default_depth;
  XEvent report;
  XSetWindowAttributes win_attr;
  unsigned long plane_masks_return[1];
  XColor color;

  th->setprefix("[XGUI]");
  UNUSED(th);

if (bx_options.private_colormap) {
  BX_ERROR(( "private_colormap option not handled yet.\n" ));
  }

  x_tilesize = tilewidth;
  y_tilesize = tileheight;
  bx_headerbar_y = headerbar_y;

  for (i=0; i<80; i++) {
    blank_line[i] = ' ';
    }

  progname = argv[0];

  /* connect to X server */
  if ( (bx_x_display=XOpenDisplay(display_name)) == NULL )
  {
    BX_PANIC(("%s: cannot connect to X server %s\n",
        progname, XDisplayName(display_name)));
  }

  /* get screen size from display structure macro */
  bx_x_screen_num = DefaultScreen(bx_x_display);

  /* Note that in a real application, x and y would default to 0
   * but would be settable from the command line or resource database.
   */
  x = y = 0;


  load_font();

  font_width = font_info->max_bounds.width;
  font_height = (font_info->max_bounds.ascent +
    font_info->max_bounds.descent);

  dimension_x = columns * font_width;
  dimension_y = rows * font_height + headerbar_y;

  BX_INFO(("font_width = %u\n", (unsigned) font_width));
  BX_INFO(("font_height = %u\n", (unsigned) font_height));

  /* create opaque window */
  win = XCreateSimpleWindow(bx_x_display, RootWindow(bx_x_display,bx_x_screen_num),
    x, y,
    dimension_x,
    dimension_y,
    border_width,
    BlackPixel(bx_x_display, bx_x_screen_num),
    BlackPixel(bx_x_display, bx_x_screen_num));

  // (attempt to) enable backing store
  win_attr.save_under=1;
  win_attr.backing_store=Always;
  XChangeWindowAttributes(bx_x_display,win,CWSaveUnder|CWBackingStore,&win_attr);

  default_depth  = DefaultDepth(bx_x_display, bx_x_screen_num);
  default_visual = DefaultVisual(bx_x_display, bx_x_screen_num);

  if (bx_options.private_colormap) {
    default_cmap = XCreateColormap(bx_x_display, DefaultRootWindow(bx_x_display),
                                   default_visual, AllocNone);
    if (XAllocColorCells(bx_x_display, default_cmap, False,
                         plane_masks_return, 0, col_vals, 256) == 0) {
      BX_PANIC(("XAllocColorCells returns error.\n"));
      }

    win_attr.colormap = default_cmap;
    XChangeWindowAttributes(bx_x_display, win, CWColormap, &win_attr);

    color.flags = DoRed | DoGreen | DoBlue;

    for (i=0; i < 256; i++) {
      color.pixel = i;
      if (i==15) {
        color.red   = 0xffff;
        color.green = 0xffff;
        color.blue  = 0xffff;
        }
      else {
        color.red   = 0;
        color.green = 0;
        color.blue  = 0;
        }
      XStoreColor(bx_x_display, default_cmap, &color);
      }
    }
  else {
    default_cmap = DefaultColormap(bx_x_display, bx_x_screen_num);
    col_vals[0]  = BlackPixel(bx_x_display, bx_x_screen_num);
    col_vals[15] = WhitePixel(bx_x_display, bx_x_screen_num);
    for (i = 1; i < 256; i++) {
      if (i==15) continue;
      col_vals[i] = col_vals[0];
      }
    }

  // convenience variables which hold the black & white color indeces
  black_pixel = col_vals[0];
  white_pixel = col_vals[15];

  BX_INFO(("default_depth = %d\n", default_depth));

  //select_visual();


  /* Get available icon sizes from Window manager */

  /* Create pixmap of depth 1 (bitmap) for icon */
  icon_pixmap = XCreateBitmapFromData(bx_x_display, win,
    (char *) bochs_icon_bits, bochs_icon_width, bochs_icon_height);

  /* Set size hints for window manager.  The window manager may
   * override these settings.  Note that in a real
   * application if size or position were set by the user
   * the flags would be UPosition and USize, and these would
   * override the window manager's preferences for this window. */
  /* x, y, width, and height hints are now taken from
   * the actual settings of the window when mapped. Note
   * that PPosition and PSize must be specified anyway. */

  size_hints.flags = PPosition | PSize | PMinSize | PMaxSize;
  size_hints.max_width = size_hints.min_width = dimension_x;
  size_hints.max_height = size_hints.min_height = dimension_y;

  {
  XWMHints wm_hints;
  XClassHint class_hints;

  /* format of the window name and icon name
   * arguments has changed in R4 */
  XTextProperty windowName, iconName;

  /* These calls store window_name and icon_name into
   * XTextProperty structures and set their other
   * fields properly. */
  if (XStringListToTextProperty(&window_name, 1, &windowName) == 0) {
    BX_PANIC(("%s: structure allocation for windowName failed.\n",
        progname));
  }

  if (XStringListToTextProperty(&icon_name, 1, &iconName) == 0) {
    BX_PANIC(("%s: structure allocation for iconName failed.\n",
        progname));
  }

  wm_hints.initial_state = NormalState;
  wm_hints.input = True;
  wm_hints.icon_pixmap = icon_pixmap;
  wm_hints.flags = StateHint | IconPixmapHint | InputHint;

  class_hints.res_name = progname;
  class_hints.res_class = "Bochs";

  XSetWMProperties(bx_x_display, win, &windowName, &iconName,
      argv, argc, &size_hints, &wm_hints,
      &class_hints);
  }

  /* Select event types wanted */
  XSelectInput(bx_x_display, win, ExposureMask | KeyPressMask | KeyReleaseMask |
      ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask |
      EnterWindowMask | LeaveWindowMask );


  /* Create default Graphics Context */
  gc               = XCreateGC(bx_x_display, win, valuemask, &values);
  gc_inv           = XCreateGC(bx_x_display, win, valuemask, &values);
  gc_headerbar     = XCreateGC(bx_x_display, win, valuemask, &values);
  gc_headerbar_inv = XCreateGC(bx_x_display, win, valuemask, &values);

  /* specify font */
  XSetFont(bx_x_display, gc, font_info->fid);

  XSetState(bx_x_display, gc, white_pixel, black_pixel, GXcopy,AllPlanes);

  XSetState(bx_x_display, gc_inv, black_pixel, white_pixel, GXcopy,AllPlanes);

  XSetState(bx_x_display, gc_headerbar, black_pixel, white_pixel, GXcopy,AllPlanes);

  XSetState(bx_x_display, gc_headerbar_inv, white_pixel, black_pixel, GXcopy,AllPlanes);


  /* Display window */
  XMapWindow(bx_x_display, win);
  XSync(bx_x_display, /* no discard */ 0);

  BX_INFO(("waiting for MapNotify\n"));
  while (1) {
    XNextEvent(bx_x_display, &report);
    if (report.type == MapNotify) break;
    }
  BX_INFO(("MapNotify found.\n"));

{
  char *imagedata;

  ximage = XCreateImage(bx_x_display, default_visual,
             default_depth,          // depth of image (bitplanes)
             ZPixmap,
             0,                      // offset
             NULL,                   // malloc() space after
             x_tilesize, y_tilesize, // x & y size of image
             32,                     // # bits of padding
             0 );                    // bytes_per_line, let X11 calculate
  if (!ximage)
    BX_PANIC(("vga: couldn't XCreateImage()\n"));

  imDepth = default_depth;
  imWide  = ximage->bytes_per_line;
  imBPP   = ximage->bits_per_pixel;

  imagedata = (char *) malloc( (size_t) (ximage->bytes_per_line * y_tilesize) );
  if (!imagedata) BX_PANIC(("imagedata: malloc returned error\n"));

  ximage->data = imagedata;

  if (imBPP < imDepth) {
    BX_PANIC(("vga_x: bits_per_pixel < depth ?\n"));
    }

}

  curr_background = 0;
  XSetBackground(bx_x_display, gc, col_vals[curr_background]);
  curr_foreground = 1;
  XSetForeground(bx_x_display, gc, col_vals[curr_foreground]);
  //XGrabPointer( bx_x_display, win, True, 0, GrabModeAsync, GrabModeAsync,
  //  win, None, CurrentTime );


  XFlush(bx_x_display);
}


  void
load_font(void)
{

  /* Load font and get font information structure. */
  if ((font_info = XLoadQueryFont(bx_x_display,"vga")) == NULL) {
    BX_PANIC(("Could not open vga font\n"));
    }
}


  void
bx_gui_c::handle_events(void)
{
  XEvent report;
  XKeyEvent *key_event;
  KeySym keysym;
  XComposeStatus compose;
  char buffer[MAX_MAPPED_STRING_LENGTH];
  int bufsize = MAX_MAPPED_STRING_LENGTH;
  int charcount;
  Boolean mouse_update;


  XPointerMovedEvent *pointer_event;
  XEnterWindowEvent *enter_event;
  XLeaveWindowEvent *leave_event;
  XButtonEvent *button_event;
  XExposeEvent *expose_event;


  //current_x = -1;
  //current_y = -1;
  mouse_update = 0;

  while (XPending(bx_x_display) > 0)  {
    XNextEvent(bx_x_display, &report);
    switch  (report.type) {

    case Expose:
      /* unless this is the last contiguous expose,
       * don't draw the window */
      expose_event = (XExposeEvent *) &report;

      bx_devices.vga->redraw_area(
        (unsigned) expose_event->x,
        (unsigned) expose_event->y,
        (unsigned) expose_event->width,
        (unsigned) expose_event->height);

      show_headerbar();

      //if (report.xexpose.count != 0) {
      //  break;
      //  }

      break;

    case ConfigureNotify:
      BX_INFO(("ConfigureNotify Xevent\n"));
      show_headerbar();
      break;

    case ButtonPress:
      button_event = (XButtonEvent *) &report;
//BX_INFO(("xxx: buttonpress\n");
      if (button_event->y < BX_HEADER_BAR_Y) {
//BX_INFO(("xxx:   in headerbar\n"));
        if (mouse_update) {
//BX_INFO(("xxx:   mouse_update=1\n"));
          send_keyboard_mouse_status();
          mouse_update = 0;
          }
        prev_x = current_x = -1;
        prev_y = current_y = -1;
        headerbar_click(button_event->x, button_event->y);
        break;
        }
      current_x = button_event->x;
      current_y = button_event->y;
      mouse_update = 1;
//BX_INFO(("xxx:   x,y=(%d,%d)\n", current_x, current_y));
      switch (button_event->button) {
        case Button1:
//BX_INFO(("xxx:   button1\n"));
          mouse_button_state |= 0x01;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        case Button2:
//BX_INFO(("xxx:   button2\n"));

	      // (mch) Hack for easier mouse handling (toggle mouse enable)
	      mouse_handler();
	      if (bx_options.mouse_enabled) {
		    BX_INFO(("[x] Mouse enabled\n"));
		    mouse_enable_x = current_x;
		    mouse_enable_y = current_y;
		    disable_cursor();
		    // Move the cursor to a 'safe' place
		    warp_cursor(warp_home_x-current_x, warp_home_y-current_y);
	      } else {
		    BX_INFO(("[x] Mouse disabled\n"));
		    enable_cursor();
		    warp_cursor(mouse_enable_x-current_x, mouse_enable_y-current_y);
	      }

          //mouse_button_state |= ;
          //send_keyboard_mouse_status();
          //mouse_update = 0;
          break;
        case Button3:
//BX_INFO(("xxx:   button3\n"));
          mouse_button_state |= 0x02;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        }
      break;

    case ButtonRelease:
      button_event = (XButtonEvent *) &report;
//BX_INFO(("xxx: buttonrelease\n"));
      if (button_event->y < BX_HEADER_BAR_Y) {
//BX_INFO(("xxx:   in headerbar\n"));
        if (mouse_update) {
//BX_INFO(("xxx:   mouse_update=1\n"));
          send_keyboard_mouse_status();
          mouse_update = 0;
          }
        prev_x = current_x = -1;
        prev_y = current_y = -1;
        // ignore, in headerbar area
        break;
        }
      current_x = button_event->x;
      current_y = button_event->y;
      mouse_update = 1;
//BX_INFO(("xxx:   x,y=(%d,%d)\n", current_x, current_y));
      switch (button_event->button) {
        case Button1:
//BX_INFO(("xxx:   button1\n"));
          mouse_button_state &= ~0x01;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        case Button2:
//BX_INFO(("xxx:   button2\n"));
          //mouse_button_state &= ~;
          //send_keyboard_mouse_status();
          //mouse_update = 0;
          break;
        case Button3:
//BX_INFO(("xxx:   button3\n"));
          mouse_button_state &= ~0x02;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        }
      break;

    case KeyPress:
      key_event = (XKeyEvent *) &report;
      charcount = XLookupString(key_event, buffer, bufsize, &keysym, &compose);
      xkeypress(keysym, 0);
      break;

    case KeyRelease:
      key_event = (XKeyEvent *) &report;
      charcount = XLookupString(key_event, buffer, bufsize, &keysym, &compose);
      xkeypress(keysym, 1);
      break;

    case MotionNotify:
      pointer_event = (XPointerMovedEvent *) &report;
      current_x = pointer_event->x;
      current_y = pointer_event->y;
      mouse_update = 1;
//BX_INFO(("xxx: motionNotify x,y=(%d,%d)\n", current_x, current_y));
      break;

    case EnterNotify:
      enter_event = (XEnterWindowEvent *) &report;
      prev_x = current_x = enter_event->x;
      prev_y = current_y = enter_event->y;
//BX_INFO(("xxx: enterNotify x,y=(%d,%d)\n", current_x, current_y));
      break;

    case LeaveNotify:
      leave_event = (XLeaveWindowEvent *) &report;
      prev_x = current_x = -1;
      prev_y = current_y = -1;
//BX_INFO(("xxx: LeaveNotify x,y set to -1\n"));
      break;

    case MapNotify:
      /* screen needs redraw, since X would have tossed previous
       * requests before window mapped
       */
//BX_INFO(("xxx: mapnotify: found\n"));
      //retval = 1;
      break;

    default:
	  // (mch) Ignore...
	  // BX_INFO(("xxx: default Xevent type\n"));
      /* all events selected by StructureNotifyMask are thrown away here,
       * since nothing is done with them */
      break;
    } /* end switch */
  } /* end while */

  if (mouse_update) {
    //BX_INFO(("xxx: bottom, send status\n"));
    send_keyboard_mouse_status();
    }
}


  void
send_keyboard_mouse_status(void)
{
//BX_INFO(("xxx: prev=(%d,%d) curr=(%d,%d)\n",
//  prev_x, prev_y, current_x, current_y));

  if ( (prev_x!=-1) && (current_x!=-1) && (prev_y!=-1) && (current_y!=-1)) {
    int dx, dy;

    // (mch) consider warping here
    dx = current_x - prev_x - warp_dx;
    dy = -(current_y - prev_y - warp_dy);
    warp_cursor(warp_home_x-current_x, warp_home_y-current_y);

//BX_INFO(("xxx: MOUSE_MOTION: dx=%d, dy=%d\n", (int) dx, (int) dy));
    bx_devices.keyboard->mouse_motion( dx, dy, mouse_button_state);
    //if (warped) {
    //  prev_x = current_x = -1;
    //  prev_y = current_y = -1;
    //  }
    //else {
      prev_x = current_x;
      prev_y = current_y;
    //  }
    }
  else {
    if ( (current_x!=-1) && (current_y!=-1)) {
      prev_x = current_x;
      prev_y = current_y;
      }
    else {
      prev_x = current_x = -1;
      prev_y = current_y = -1;
      }
    }
}

  void
bx_gui_c::flush(void)
{
  XFlush(bx_x_display);
}


  void
xkeypress(KeySym keysym, int press_release)
{
  Bit32u key_event;





  // this depends on the fact that the X11 keysyms which
  // correspond to the ascii characters space .. tilde
  // are in consequtive order.
  if ((keysym >= XK_space) && (keysym <= XK_asciitilde)) {
    key_event = ascii_to_key_event[keysym - XK_space];
    }
  else switch (keysym) {
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
      BX_ERROR(( "xkeypress(): keysym %x unhandled!\n", (unsigned) keysym ));
      return;
      break;
    }

  if (press_release)
    key_event |= BX_KEY_RELEASED;

  bx_devices.keyboard->gen_scancode(key_event);
}


  void
bx_gui_c::clear_screen(void)
{
  XClearArea(bx_x_display, win, 0, bx_headerbar_y, dimension_x, dimension_y-bx_headerbar_y, 0);
}




  void
bx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                      unsigned long cursor_x, unsigned long cursor_y,
                      unsigned nrows)
{
  int font_height;
  unsigned i, x, y, curs;
  unsigned new_foreground, new_background;
  Bit8u string[1];
  unsigned nchars;

  font_height = font_info->ascent + font_info->descent;

  // Number of characters on screen, variable number of rows
  nchars = 80*nrows;

  // first draw over character at original block cursor location
  if ( (prev_block_cursor_y*80 + prev_block_cursor_x) < nchars ) {
    curs = (prev_block_cursor_y*80 + prev_block_cursor_x)*2;
    string[0] = new_text[curs];
    XSetBackground(bx_x_display, gc, col_vals[(new_text[curs+1] & 0xf0) >> 4]);
    XDrawImageString(bx_x_display, win,
      gc,
      prev_block_cursor_x * font_info->max_bounds.width,
      prev_block_cursor_y * font_height + font_info->max_bounds.ascent + bx_headerbar_y,
      (char *) string,
      1);
    }

  for (i=0; i<nchars*2; i+=2) {
    if ( (old_text[i]!=new_text[i]) ||
         (old_text[i+1]!=new_text[i+1]) ) {

      string[0] = new_text[i];
      if (string[0] == 0) string[0] = ' '; // convert null to space
      new_foreground = new_text[i+1] & 0x0f;
      new_background = (new_text[i+1] & 0xf0) >> 4;

      XSetForeground(bx_x_display, gc, col_vals[new_foreground]);
      XSetBackground(bx_x_display, gc, col_vals[new_background]);

//XSetForeground(bx_x_display, gc, white_pixel);
//XSetBackground(bx_x_display, gc, black_pixel);

      x = (i/2) % 80;
      y = (i/2) / 80;

      XDrawImageString(bx_x_display, win,
        gc,
        x * font_info->max_bounds.width,
        y * font_height + font_info->max_bounds.ascent + bx_headerbar_y,
        (char *) string,
        1);
      }
    }

  prev_block_cursor_x = cursor_x;
  prev_block_cursor_y = cursor_y;

  XSetForeground(bx_x_display, gc, white_pixel);
  XSetBackground(bx_x_display, gc, black_pixel);

  // now draw character at new block cursor location in reverse
  if ( (cursor_y*80 + cursor_x) < nchars ) {
    string[0] = new_text[(cursor_y*80 + cursor_x)*2];
    XDrawImageString(bx_x_display, win,
      gc_inv,
      cursor_x * font_info->max_bounds.width,
      cursor_y * font_height + font_info->max_bounds.ascent + bx_headerbar_y,
      (char *) string,
      1);
    }

  XFlush(bx_x_display);
}


  void
bx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  unsigned x, y;
  unsigned color, offset;
  Bit8u b0, b1, b2, b3;

  for (y=0; y<y_tilesize; y++) {
    for (x=0; x<x_tilesize; x++) {
      //XPutPixel(ximage, x, y, col_vals[tile[y*x_tilesize + x]]);
      color = col_vals[tile[y*x_tilesize + x]];
      switch (imBPP) {
        case 8:  // 8 bits per pixel
          ximage->data[imWide*y + x] = color;
          break;

        case 16: // 16 bits per pixel
          offset = imWide*y + 2*x;
          b0 = color >> 0;
          b1 = color >> 8;
          if (ximage->byte_order == LSBFirst) {
            ximage->data[offset + 0] = b0;
            ximage->data[offset + 1] = b1;
            }
          else { // MSBFirst
            ximage->data[offset + 0] = b1;
            ximage->data[offset + 1] = b0;
            }
          break;

        case 24: // 24 bits per pixel
          offset = imWide*y + 3*x;
          b0 = color >> 0;
          b1 = color >> 8;
          b2 = color >> 16;
          if (ximage->byte_order == LSBFirst) {
            ximage->data[offset + 0] = b0;
            ximage->data[offset + 1] = b1;
            ximage->data[offset + 2] = b2;
            }
          else { // MSBFirst
            ximage->data[offset + 0] = b2;
            ximage->data[offset + 1] = b1;
            ximage->data[offset + 2] = b0;
            }
          break;

        case 32: // 32 bits per pixel
          offset = imWide*y + 4*x;
          b0 = color >> 0;
          b1 = color >> 8;
          b2 = color >> 16;
          b3 = color >> 24;
          if (ximage->byte_order == LSBFirst) {
            ximage->data[offset + 0] = b0;
            ximage->data[offset + 1] = b1;
            ximage->data[offset + 2] = b2;
            ximage->data[offset + 3] = b3;
            }
          else { // MSBFirst
            ximage->data[offset + 0] = b3;
            ximage->data[offset + 1] = b2;
            ximage->data[offset + 2] = b1;
            ximage->data[offset + 3] = b0;
            }
          break;
        default:
          BX_PANIC(("X_graphics_tile_update: bits_per_pixel %u not implemented\n",
            (unsigned) imBPP));
          break;
        }
      }
    }
  XPutImage(bx_x_display, win, gc, ximage, 0, 0, x0, y0+bx_headerbar_y,
            x_tilesize, y_tilesize);
}


  Boolean
bx_gui_c::palette_change(unsigned index, unsigned red, unsigned green, unsigned blue)
{
  // returns: 0=no screen update needed (color map change has direct effect)
  //          1=screen updated needed (redraw using current colormap)
  XColor color;

  color.flags = DoRed | DoGreen | DoBlue;
  color.red   = red << 8;
  color.green = green << 8;
  color.blue  = blue << 8;

  if (bx_options.private_colormap) {
    color.pixel = index;
    XStoreColor(bx_x_display, default_cmap, &color);
    return(0); // no screen update needed
    }
  else {
    XAllocColor(bx_x_display, DefaultColormap(bx_x_display, bx_x_screen_num),
                &color);
    col_vals[index] = color.pixel;
    return(1); // screen update needed
    }
}


  void
bx_gui_c::dimension_update(unsigned x, unsigned y)
{
  if ( (x != dimension_x) || (y != (dimension_y-bx_headerbar_y)) ) {
    XSizeHints hints;
    long supplied_return;

    if ( XGetWMNormalHints(bx_x_display, win, &hints, &supplied_return) &&
         supplied_return & PMaxSize ) {
      hints.max_width = hints.min_width = x;
      hints.max_height = hints.min_height = y+bx_headerbar_y;
      XSetWMNormalHints(bx_x_display, win, &hints);
      }
    XResizeWindow(bx_x_display, win, x, y+bx_headerbar_y);
    dimension_x = x;
    dimension_y = y + bx_headerbar_y;
    }
}


  void
bx_gui_c::show_headerbar(void)
{
  unsigned xorigin;

  // clear header bar area to white
  XFillRectangle(bx_x_display, win, gc_headerbar_inv, 0,0, dimension_x, bx_headerbar_y);

  for (unsigned i=0; i<bx_headerbar_entries; i++) {
    if (bx_headerbar_entry[i].alignment == BX_GRAVITY_LEFT)
      xorigin = bx_headerbar_entry[i].xorigin;
    else
      xorigin = dimension_x - bx_headerbar_entry[i].xorigin;
    XCopyPlane(bx_x_display, bx_headerbar_entry[i].bitmap, win, gc_headerbar,
      0,0, bx_headerbar_entry[i].xdim, bx_headerbar_entry[i].ydim,
              xorigin, 0, 1);
    }
}


  unsigned
bx_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  if (bx_bitmap_entries >= BX_MAX_PIXMAPS) {
    BX_PANIC(("x: too many pixmaps, increase BX_MAX_PIXMAPS\n"));
    }

  bx_bitmaps[bx_bitmap_entries].bmap =
    XCreateBitmapFromData(bx_x_display, win, (const char *) bmap, xdim, ydim);
  bx_bitmaps[bx_bitmap_entries].xdim = xdim;
  bx_bitmaps[bx_bitmap_entries].ydim = ydim;
  if (!bx_bitmaps[bx_bitmap_entries].bmap) {
    BX_PANIC(("x: could not create bitmap\n"));
    }
  bx_bitmap_entries++;
  return(bx_bitmap_entries-1); // return index as handle
}


  unsigned
bx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  unsigned hb_index;

  if ( (bx_headerbar_entries+1) > BX_MAX_HEADERBAR_ENTRIES )
    BX_PANIC(("x: too many headerbar entries, increase BX_MAX_HEADERBAR_ENTRIES\n"));

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
  XCopyPlane(bx_x_display, bx_headerbar_entry[hbar_id].bitmap, win, gc_headerbar,
    0,0, bx_headerbar_entry[hbar_id].xdim, bx_headerbar_entry[hbar_id].ydim,
            xorigin, 0, 1);
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
bx_gui_c::exit(void)
{
  BX_INFO(("Exit.\n"));
}

static void warp_cursor (int dx, int dy)
{
      if (bx_options.mouse_enabled) {
	    warp_dx = dx;
	    warp_dy = dy;
	    XWarpPointer(bx_x_display, None, None, 0, 0, 0, 0, dx, dy);
      }
}

static void disable_cursor ()
{
      static Cursor cursor;
      static unsigned cursor_created = 0;

      static int shape_width = 16,
	     shape_height = 16,
	     mask_width = 16,
	     mask_height = 16;

      static uint32 shape_bits[(16*16)/32] = {
	    0x00000000, 0x00000000, 0x00000000, 0x00000000,
	    0x00000000, 0x00000000, 0x00000000, 0x00000000,
      };
      static uint32 mask_bits[(16*16)/32] = {
	    0x00000000, 0x00000000, 0x00000000, 0x00000000,
	    0x00000000, 0x00000000, 0x00000000, 0x00000000,
      };

      if (!cursor_created) {
	    Pixmap shape, mask;
	    XColor white, black;
	    shape = XCreatePixmapFromBitmapData(bx_x_display,
						RootWindow(bx_x_display,bx_x_screen_num),
						(char*)shape_bits,
						shape_width,
						shape_height,
						1, 0, 1);
	    mask =  XCreatePixmapFromBitmapData(bx_x_display,
						RootWindow(bx_x_display,bx_x_screen_num),
						(char*)mask_bits,
						mask_width,
						mask_height,
						1, 0, 1);
	    XParseColor(bx_x_display, default_cmap, "black", &black);
	    XParseColor(bx_x_display, default_cmap, "white", &white);
	    cursor = XCreatePixmapCursor(bx_x_display, shape, mask,
					 &white, &black, 1, 1);
	    cursor_created = 1;
      }

      XDefineCursor(bx_x_display, win, cursor);
}

static void enable_cursor ()
{
      XUndefineCursor(bx_x_display, win);
}
