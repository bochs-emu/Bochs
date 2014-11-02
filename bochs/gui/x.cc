/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
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

#define XK_PUBLISHING
#define XK_TECHNICAL

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "param_names.h"
#include "keymap.h"
#include "iodev.h"
#include "enh_dbg.h"
#if BX_WITH_X11

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/Xrandr.h>
#if BX_HAVE_XPM_H
#include <X11/xpm.h>
#endif
}

#if BX_HAVE_XPM_H
#include "icon_bochs.xpm"
#else
#include "icon_bochs.h"
#endif

#include "font/vga.bitmap.h"

class bx_x_gui_c : public bx_gui_c {
public:
  bx_x_gui_c (void);
  DECLARE_GUI_VIRTUAL_METHODS()
  DECLARE_GUI_NEW_VIRTUAL_METHODS()
#if BX_USE_IDLE_HACK
  virtual void sim_is_idle(void);
#endif
  virtual void beep_on(float frequency);
  virtual void beep_off();
  virtual void statusbar_setitem_specific(int element, bx_bool active, bx_bool w);
  virtual void get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp);
  virtual void set_mouse_mode_absxy(bx_bool mode);
#if BX_SHOW_IPS
  virtual void show_ips(Bit32u ips_count);
#endif
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_x_gui_c *theGui = NULL;
IMPLEMENT_GUI_PLUGIN_CODE(x)

#define LOG_THIS theGui->

#define MAX_MAPPED_STRING_LENGTH 10

/* These are used as arguments to nearly every Xlib routine, so it saves
 * routine arguments to declare them global.  If there were
 * additional source files, they would be declared extern there. */
Display *bx_x_display;
int bx_x_screen_num;
static Visual *default_visual;
static Colormap default_cmap;
static bx_bool x11_private_colormap;
static unsigned long white_pixel=0, black_pixel=0;

static char *progname; /* name this program was invoked by */

// text display
static unsigned int text_rows=25, text_cols=80;
static unsigned font_width, font_height;
static Bit8u h_panning = 0, v_panning = 0;
static Bit16u line_compare = 1023;
static unsigned prev_cursor_x=0;
static unsigned prev_cursor_y=0;

// graphics display
static Window win;
static GC gc, gc_inv, gc_headerbar, gc_headerbar_inv;
static unsigned dimension_x=0, dimension_y=0;

static XImage *ximage = NULL;
static unsigned imDepth, imWide, imBPP;

// mouse cursor
static bx_bool mouse_captured = 0;
static int prev_x=-1, prev_y=-1;
static int current_x=-1, current_y=-1, current_z=0;
static unsigned mouse_button_state = 0;
static bx_bool x11_mouse_mode_absxy = 0;

static int warp_home_x = 200;
static int warp_home_y = 200;
static int mouse_enable_x = 0;
static int mouse_enable_y = 0;
static int warp_dx = 0;
static int warp_dy = 0;

static void warp_cursor(int dx, int dy);
static void disable_cursor();
static void enable_cursor();

// keyboard
static bx_bool x11_nokeyrepeat = 0;
static bx_bool x11_use_kbd_mapping = 0;
static Bit32u convertStringToXKeysym (const char *string);

static bx_bool x_init_done = 0;

static Pixmap vgafont[256];

static unsigned bx_bitmap_entries = 0;
static struct {
  Pixmap bmap;
  unsigned xdim;
  unsigned ydim;
} bx_bitmaps[BX_MAX_PIXMAPS];

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

static unsigned bx_statusbar_y = 18;
static unsigned bx_statusitem_pos[12] = {
  0, 200, 240, 280, 320, 360, 400, 440, 480, 520, 560, 600
};
static bx_bool bx_statusitem_active[12];
static long bx_status_led_green, bx_status_led_red, bx_status_graytext;
static char bx_status_info_text[34];
#if BX_SHOW_IPS
static bx_bool x11_ips_update = 0, x11_hide_ips = 0;
static char x11_ips_text[20];
static Bit8u x11_mouse_msg_counter = 0;
#endif

static void headerbar_click(int x, int y);
static void send_keyboard_mouse_status(void);
static void set_status_text(int element, const char *text, bx_bool active, bx_bool w=0);


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

static void create_internal_vga_font(void);
static void xkeypress(KeySym keysym, int press_release);
// extern "C" void select_visual(void);

#define ROUNDUP(nbytes, pad) ((((nbytes) + ((pad)-1)) / (pad)) * ((pad)>>3))


#define MAX_VGA_COLORS 256

unsigned long col_vals[MAX_VGA_COLORS]; // 256 VGA colors
unsigned curr_foreground, curr_background;

BxEvent *x11_notify_callback (void *unused, BxEvent *event);
static bxevent_handler old_callback = NULL;
static void *old_callback_arg = NULL;


// Try to allocate NCOLORS at once in the colormap provided.  If it can
// be done, return true.  If not, return false.  (In either case, free
// up the color cells so that we don't add to the problem!)  This is used
// to determine whether Bochs should use a private colormap even when the
// user did not specify it.
static bx_bool
test_alloc_colors (Colormap cmap, Bit32u n_tries) {
  XColor color;
  unsigned long pixel[MAX_VGA_COLORS];
  bx_bool pixel_valid[MAX_VGA_COLORS];
  Bit32u n_allocated = 0;
  Bit32u i;
  color.flags = DoRed | DoGreen | DoBlue;
  for (i=0; i<n_tries; i++) {
    // choose wierd color values that are unlikely to already be in the
    // colormap.
    color.red   = ((i+41)%MAX_VGA_COLORS) << 8;
    color.green = ((i+42)%MAX_VGA_COLORS) << 8;
    color.blue  = ((i+43)%MAX_VGA_COLORS) << 8;
    pixel_valid[i] = false;
    if (XAllocColor (bx_x_display, cmap, &color)) {
      pixel[i] = color.pixel;
      pixel_valid[i] = true;
      n_allocated++;
    }
  }
  BX_INFO (("test_alloc_colors: %d colors available out of %d colors tried", n_allocated, n_tries));
  // now free them all
  for (i=0; i<n_tries; i++) {
    if (pixel_valid[i]) XFreeColors (bx_x_display, cmap, &pixel[i], 1, 0);
  }
  return (n_allocated == n_tries);
}

bx_x_gui_c::bx_x_gui_c () {}

void bx_x_gui_c::specific_init(int argc, char **argv, unsigned headerbar_y)
{
  int i;
  int x, y;   /* window position */
  unsigned int border_width = 4;  /* four pixels */
  const char *window_name = BOCHS_WINDOW_NAME;
  const char *icon_name = "Bochs";
  Pixmap icon_pixmap;
#if BX_HAVE_XPM_H
  Pixmap icon_mask;
#endif
  XSizeHints size_hints;
  char *display_name = NULL;
  /* create GC for text and drawing */
  unsigned long valuemask = 0; /* ignore XGCvalues and use defaults */
  XGCValues values;
  int      default_depth;
  XEvent report;
  XSetWindowAttributes win_attr;
  unsigned long plane_masks_return[1];
  XColor color;

  put("XGUI");

  bx_headerbar_y = headerbar_y;

  progname = argv[0];

  /* connect to X server */
  if ((bx_x_display=XOpenDisplay(display_name)) == NULL)
  {
    BX_PANIC(("%s: cannot connect to X server %s",
        progname, XDisplayName(display_name)));
  }

  /* get screen size from display structure macro */
  bx_x_screen_num = DefaultScreen(bx_x_display);

  /* Note that in a real application, x and y would default to 0
   * but would be settable from the command line or resource database.
   */
  x = y = 0;

  // Temporary values so we can create the window
  font_width = 8;
  font_height = 16;

  dimension_x = text_cols * font_width;
  dimension_y = text_rows * font_height;

  /* create opaque window */
  win = XCreateSimpleWindow(bx_x_display, RootWindow(bx_x_display,bx_x_screen_num),
    x, y,
    dimension_x,
    dimension_y + bx_headerbar_y + bx_statusbar_y,
    border_width,
    BlackPixel(bx_x_display, bx_x_screen_num),
    BlackPixel(bx_x_display, bx_x_screen_num));

  // (attempt to) enable backing store
  win_attr.save_under=1;
  win_attr.backing_store=Always;
  XChangeWindowAttributes(bx_x_display,win,CWSaveUnder|CWBackingStore,&win_attr);

  default_depth  = DefaultDepth(bx_x_display, bx_x_screen_num);
  default_visual = DefaultVisual(bx_x_display, bx_x_screen_num);

  x11_private_colormap = SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get();
  if (!x11_private_colormap) {
    default_cmap = DefaultColormap(bx_x_display, bx_x_screen_num);
    // try to use default colormap.  If not enough colors are available,
    // then switch to private colormap despite the user setting.  There
    // are too many cases when no colors are available and Bochs simply
    // draws everything in black on black.
    if (!test_alloc_colors (default_cmap, 16)) {
      BX_ERROR (("I can't even allocate 16 colors!  Switching to a private colormap"));
      x11_private_colormap = 1;
    }
    col_vals[0]  = BlackPixel(bx_x_display, bx_x_screen_num);
    col_vals[15] = WhitePixel(bx_x_display, bx_x_screen_num);
    for (i = 1; i < MAX_VGA_COLORS; i++) {
      if (i==15) continue;
      col_vals[i] = col_vals[0];
    }
  }

  if (x11_private_colormap) {
    default_cmap = XCreateColormap(bx_x_display, DefaultRootWindow(bx_x_display),
                                   default_visual, AllocNone);
    if (XAllocColorCells(bx_x_display, default_cmap, False,
                         plane_masks_return, 0, col_vals, MAX_VGA_COLORS) == 0) {
      BX_PANIC(("XAllocColorCells returns error. Maybe your screen does not support a private colormap?"));
    }

    win_attr.colormap = default_cmap;
    XChangeWindowAttributes(bx_x_display, win, CWColormap, &win_attr);

    color.flags = DoRed | DoGreen | DoBlue;

    for (i=0; i < MAX_VGA_COLORS; i++) {
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

  // convenience variables which hold the black & white color indeces
  black_pixel = col_vals[0];
  white_pixel = col_vals[15];

  BX_INFO(("font %u wide x %u high, display depth = %d",
                (unsigned) font_width, (unsigned) font_height, default_depth));

  //select_visual();

  /* Get available icon sizes from Window manager */

#if BX_HAVE_XPM_H
  /* Create pixmap from XPM for icon */
  XCreatePixmapFromData(bx_x_display, win, (char **)icon_bochs_xpm, &icon_pixmap, &icon_mask, NULL);
#else
  /* Create pixmap of depth 1 (bitmap) for icon */
  icon_pixmap = XCreateBitmapFromData(bx_x_display, win,
    (char *)bochs_icon_bits, bochs_icon_width, bochs_icon_height);
#endif

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
  size_hints.max_height = size_hints.min_height = dimension_y + bx_headerbar_y +
                          bx_statusbar_y;

  {
  XWMHints wm_hints;
  XClassHint class_hints;

  /* format of the window name and icon name
   * arguments has changed in R4 */
  XTextProperty windowName, iconName;

  /* These calls store window_name and icon_name into
   * XTextProperty structures and set their other
   * fields properly. */
  if (XStringListToTextProperty((char **)&window_name, 1, &windowName) == 0) {
    BX_PANIC(("%s: structure allocation for windowName failed.",
        progname));
  }

  if (XStringListToTextProperty((char **)&icon_name, 1, &iconName) == 0) {
    BX_PANIC(("%s: structure allocation for iconName failed.",
        progname));
  }

  wm_hints.initial_state = NormalState;
  wm_hints.input = True;
  wm_hints.icon_pixmap = icon_pixmap;
#if BX_HAVE_XPM_H
  wm_hints.icon_mask = icon_mask;
  wm_hints.flags = StateHint | IconPixmapHint | IconMaskHint | InputHint;
#else
  wm_hints.flags = StateHint | IconPixmapHint | InputHint;
#endif
  class_hints.res_name = progname;
  class_hints.res_class = (char *)"Bochs";

  XSetWMProperties(bx_x_display, win, &windowName, &iconName,
      argv, argc, &size_hints, &wm_hints,
      &class_hints);
  XFree(windowName.value);
  XFree(iconName.value);
  Atom wm_delete = XInternAtom(bx_x_display, "WM_DELETE_WINDOW", 1);
  XSetWMProtocols(bx_x_display, win, &wm_delete, 1);
  }

  /* Select event types wanted */
  XSelectInput(bx_x_display, win, ExposureMask | KeyPressMask | KeyReleaseMask |
      ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask |
      EnterWindowMask | LeaveWindowMask);

  /* Create default Graphics Context */
  gc               = XCreateGC(bx_x_display, win, valuemask, &values);
  gc_inv           = XCreateGC(bx_x_display, win, valuemask, &values);
  gc_headerbar     = XCreateGC(bx_x_display, win, valuemask, &values);
  gc_headerbar_inv = XCreateGC(bx_x_display, win, valuemask, &values);

  XSetState(bx_x_display, gc, white_pixel, black_pixel, GXcopy,AllPlanes);

  XSetState(bx_x_display, gc_inv, black_pixel, white_pixel, GXinvert,AllPlanes);

  XSetState(bx_x_display, gc_headerbar, black_pixel, white_pixel, GXcopy,AllPlanes);

  XSetState(bx_x_display, gc_headerbar_inv, white_pixel, black_pixel, GXcopy,AllPlanes);


  /* Display window */
  XMapWindow(bx_x_display, win);
  XSync(bx_x_display, /* no discard */ 0);

  BX_DEBUG(("waiting for MapNotify"));
  while (1) {
    XNextEvent(bx_x_display, &report);
    if (report.type == MapNotify) break;
  }
  BX_DEBUG(("MapNotify found."));

  // Create the VGA font
  create_internal_vga_font();

  {
  char *imagedata;

  ximage = XCreateImage(bx_x_display, default_visual,
             default_depth,          // depth of image (bitplanes)
             ZPixmap,
             0,                      // offset
             NULL,                   // malloc() space after
             x_tilesize, y_tilesize, // x & y size of image
             32,                     // # bits of padding
             0);                     // bytes_per_line, let X11 calculate
  if (!ximage)
    BX_PANIC(("vga: couldn't XCreateImage()"));

  imDepth = default_depth;
  imWide  = ximage->bytes_per_line;
  imBPP   = ximage->bits_per_pixel;

  imagedata = (char *) malloc((size_t) (ximage->bytes_per_line * y_tilesize));
  if (!imagedata) BX_PANIC(("imagedata: malloc returned error"));

  ximage->data = imagedata;

  if (imBPP < imDepth) {
    BX_PANIC(("vga_x: bits_per_pixel < depth ?"));
  }

  for (i=0; i<12; i++) bx_statusitem_active[i] = 0;
  switch (imBPP) {
    case 16:
      bx_status_led_green = 0x07e0;
      bx_status_led_red = 0xf900;
      bx_status_graytext = 0x8410;
      break;
    case 24:
    case 32:
      bx_status_led_green = 0x00ff00;
      bx_status_led_red = 0xff4000;
      bx_status_graytext = 0x808080;
      break;
    default:
      bx_status_led_green = 0;
      bx_status_graytext = 0;
  }
  sprintf(bx_status_info_text, "%s enables mouse", get_toggle_info());

  x_init_done = 1;
  }

  curr_background = 0;
  XSetBackground(bx_x_display, gc, col_vals[curr_background]);
  curr_foreground = 1;
  XSetForeground(bx_x_display, gc, col_vals[curr_foreground]);
//XGrabPointer(bx_x_display, win, True, 0, GrabModeAsync, GrabModeAsync,
//  win, None, CurrentTime);

  XFlush(bx_x_display);

  // redirect notify callback to X11 specific code
  SIM->get_notify_callback (&old_callback, &old_callback_arg);
  assert (old_callback != NULL);
  SIM->set_notify_callback (x11_notify_callback, NULL);

  // loads keymap for x11
  x11_use_kbd_mapping = SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get();
  if (x11_use_kbd_mapping) {
    bx_keymap.loadKeymap(convertStringToXKeysym);
  }

  // parse x11 specific options
  if (argc > 1) {
    for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "nokeyrepeat")) {
        BX_INFO(("disabled host keyboard repeat"));
        x11_nokeyrepeat = 1;
#if BX_DEBUGGER && BX_DEBUGGER_GUI
      } else if (!strcmp(argv[i], "gui_debug")) {
        SIM->set_debug_gui(1);
        init_debug_dialog();
#endif
#if BX_SHOW_IPS
      } else if (!strcmp(argv[i], "hideIPS")) {
        BX_INFO(("hide IPS display in status bar"));
        x11_hide_ips = 1;
#endif
      } else {
        BX_PANIC(("Unknown x11 option '%s'", argv[i]));
      }
    }
  }

  new_gfx_api = 1;
  dialog_caps |= (BX_GUI_DLG_USER | BX_GUI_DLG_SNAPSHOT | BX_GUI_DLG_CDROM);
}

void set_status_text(int element, const char *text, bx_bool active, bx_bool w)
{
  int xleft, xsize, sb_ypos;

  xleft = bx_statusitem_pos[element] + 2;
  xsize = bx_statusitem_pos[element+1] - xleft;
  sb_ypos = dimension_y + bx_headerbar_y;
  if (element < 1) {
    if (strcmp(bx_status_info_text, text)) {
      strcpy(bx_status_info_text, text);
    }
    XFillRectangle(bx_x_display, win, gc_headerbar_inv, xleft, sb_ypos+2, xsize,
                   bx_statusbar_y-2);
    XDrawString(bx_x_display, win, gc_headerbar, xleft, sb_ypos+bx_statusbar_y-2,
                text, strlen(text));
  } else if (element <= BX_MAX_STATUSITEMS) {
    bx_statusitem_active[element] = active;
    if (active) {
      if (w)
        XSetForeground(bx_x_display, gc_headerbar, bx_status_led_red);
      else
        XSetForeground(bx_x_display, gc_headerbar, bx_status_led_green);
      XFillRectangle(bx_x_display, win, gc_headerbar, xleft, sb_ypos+2, xsize-1, bx_statusbar_y-2);
      XSetForeground(bx_x_display, gc_headerbar, black_pixel);
    } else {
      XFillRectangle(bx_x_display, win, gc_headerbar_inv, xleft, sb_ypos+2, xsize-1, bx_statusbar_y-2);
      XSetForeground(bx_x_display, gc_headerbar, bx_status_graytext);
    }
    XDrawString(bx_x_display, win, gc_headerbar, xleft, sb_ypos+bx_statusbar_y-2,
                text, strlen(text));
    XSetForeground(bx_x_display, gc_headerbar, black_pixel);
  }
}

void bx_x_gui_c::statusbar_setitem_specific(int element, bx_bool active, bx_bool w)
{
  set_status_text(element+1, statusitem[element].text, active, w);
}

// This is called whenever the mouse_enabled parameter changes.  It
// can change because of a gui event such as clicking on the mouse-enable
// bitmap or pressing the middle button, or from the configuration interface.
// In all those cases, setting the parameter value will get you here.
void bx_x_gui_c::mouse_enabled_changed_specific(bx_bool val)
{
  mouse_captured = val;
  if (val) {
    BX_INFO(("Mouse capture on"));
    sprintf(bx_status_info_text, "%s disables mouse", get_toggle_info());
    set_status_text(0, bx_status_info_text, 0);
    mouse_enable_x = current_x;
    mouse_enable_y = current_y;
    disable_cursor();
    // Move the cursor to a 'safe' place
    warp_cursor(warp_home_x-current_x, warp_home_y-current_y);
  } else {
    BX_INFO(("Mouse capture off"));
    sprintf(bx_status_info_text, "%s enables mouse", get_toggle_info());
    set_status_text(0, bx_status_info_text, 0);
    enable_cursor();
    warp_cursor(mouse_enable_x-current_x, mouse_enable_y-current_y);
  }
#if BX_SHOW_IPS
  x11_mouse_msg_counter = 3;
#endif
}

void create_internal_vga_font(void)
{
  // Default values
  font_width=8;
  font_height=16;

  for(int i=0; i<256; i++) {
    vgafont[i]=XCreateBitmapFromData(bx_x_display, win, (const char*)bx_vgafont[i].data,
                                     font_width, font_height);
    if(vgafont[i] == None)
      BX_PANIC(("Can't create vga font [%d]", i));
  }
}

/* Check to see if this is a repeated key. (grabbed from SDL 1.2)
   (idea shamelessly lifted from GII -- thanks guys! :)
 */
static int X11_KeyRepeat(Display *display, XEvent *event)
{
  XEvent peekevent;
  int repeated;

  repeated = 0;
  if (XPending(display)) {
    XPeekEvent(display, &peekevent);
    if ((peekevent.type == KeyPress) &&
        (peekevent.xkey.keycode == event->xkey.keycode) &&
        ((peekevent.xkey.time-event->xkey.time) < 2)) {
      repeated = 1;
      XNextEvent(display, &peekevent);
    }
  }
  return repeated;
}

void bx_x_gui_c::handle_events(void)
{
  XEvent report;
  XKeyEvent *key_event;
  KeySym keysym;
  char buffer[MAX_MAPPED_STRING_LENGTH];
  int bufsize = MAX_MAPPED_STRING_LENGTH;
  bx_bool mouse_update;
  int y, height;

  XPointerMovedEvent *pointer_event;
  XEnterWindowEvent *enter_event;
//  XLeaveWindowEvent *leave_event;
  XButtonEvent *button_event;
  XExposeEvent *expose_event;

  //current_x = -1;
  //current_y = -1;
  mouse_update = 0;

  while (XPending(bx_x_display) > 0)  {
    XNextEvent(bx_x_display, &report);
    current_z = 0;

    if (x11_nokeyrepeat && (report.type == KeyRelease) && X11_KeyRepeat(bx_x_display, &report)) {
      return;
    }

    switch  (report.type) {

    case Expose:
      expose_event = &report.xexpose;
      /* Adjust y, and reduce height if it overlaps headerbar. */
      y = expose_event->y - BX_HEADER_BAR_Y;
      height = expose_event->height;
      if (y < 0) {
        height += y;
        y = 0;
      }

      DEV_vga_redraw_area(
        (unsigned) expose_event->x,
        y,
        (unsigned) expose_event->width,
        height);

      /* Always draw headerbar, even if not touched by expose event.
       * As a small optimization, only do it on last contigous expose.
       */
      if (expose_event->count == 0) {
      show_headerbar();
      }
      break;

    case ConfigureNotify:
      BX_DEBUG(("ConfigureNotify Xevent"));
      /* FIXME: It's not clear why we have to show the headerbar here.
       * This should be forced by the following expose events.
       */
      show_headerbar();
      break;

    case ButtonPress:
      button_event = (XButtonEvent *) &report;
                BX_DEBUG(("xxx: buttonpress"));
      if (button_event->y < BX_HEADER_BAR_Y) {
                BX_DEBUG(("xxx:   in headerbar"));
        if (mouse_update) {
                  BX_DEBUG(("xxx:   mouse_update=1"));
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
      BX_DEBUG(("xxx:   x,y=(%d,%d)", current_x, current_y));
      switch (button_event->button) {
        case Button1:
          mouse_button_state |= 0x01;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        case Button2:
          if (mouse_toggle_check(BX_MT_MBUTTON, 1)) {
            toggle_mouse_enable();
          } else {
            mouse_button_state |= 0x04;
            send_keyboard_mouse_status();
            mouse_update = 0;
          }
          break;
        case Button3:
          mouse_button_state |= 0x02;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
      }
      break;

    case ButtonRelease:
      button_event = (XButtonEvent *) &report;
      if (button_event->y < BX_HEADER_BAR_Y) {
        if (mouse_update) {
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
      switch (button_event->button) {
        case Button1:
          mouse_button_state &= ~0x01;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        case Button2:
          mouse_toggle_check(BX_MT_MBUTTON, 0);
          mouse_button_state &= ~0x04;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        case Button3:
          mouse_button_state &= ~0x02;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        case Button4:
          current_z = 1;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
        case Button5:
          current_z = -1;
          send_keyboard_mouse_status();
          mouse_update = 0;
          break;
      }
      break;

    case KeyPress:
      key_event = (XKeyEvent *) &report;
      XLookupString(key_event, buffer, bufsize, &keysym, NULL);
      xkeypress(keysym, 0);
      break;

    case KeyRelease:
      key_event = (XKeyEvent *) &report;
      XLookupString(key_event, buffer, bufsize, &keysym, NULL);
      xkeypress(keysym, 1);
      break;

    case MotionNotify:
      pointer_event = (XPointerMovedEvent *) &report;
      current_x = pointer_event->x;
      current_y = pointer_event->y;
      mouse_update = 1;
      break;

    case EnterNotify:
      enter_event = (XEnterWindowEvent *) &report;
      prev_x = current_x = enter_event->x;
      prev_y = current_y = enter_event->y;
      break;

    case LeaveNotify:
//      leave_event = (XLeaveWindowEvent *) &report;
      prev_x = current_x = -1;
      prev_y = current_y = -1;
      break;

    case MapNotify:
      /* screen needs redraw, since X would have tossed previous
       * requests before window mapped
       */
      //retval = 1;
      break;

    case ClientMessage:
      if (!strcmp(XGetAtomName(bx_x_display, report.xclient.message_type), "WM_PROTOCOLS")) {
        bx_stop_simulation();
      }
      break;

    default:
      // (mch) Ignore...
      BX_DEBUG(("XXX: default Xevent type"));
      /* all events selected by StructureNotifyMask are thrown away here,
       * since nothing is done with them */
      break;
    } /* end switch */
  } /* end while */

  if (mouse_update) {
    BX_DEBUG(("handle_events(): send mouse status"));
    send_keyboard_mouse_status();
  }
#if BX_SHOW_IPS
  if (x11_ips_update) {
    x11_ips_update = 0;
    set_status_text(0, x11_ips_text, 1);
  }
#endif
}

void send_keyboard_mouse_status(void)
{
  int dx, dy, dz;
  BX_DEBUG(("XXX: prev=(%d,%d) curr=(%d,%d)",
            prev_x, prev_y, current_x, current_y));

  if (x11_mouse_mode_absxy) {
    if ((current_y >= (int)bx_headerbar_y) && (current_y < (int)(dimension_y + bx_headerbar_y))) {
      dx = current_x * 0x7fff / dimension_x;
      dy = (current_y - bx_headerbar_y) * 0x7fff / dimension_y;
      dz = current_z;
      DEV_mouse_motion(dx, dy, dz, mouse_button_state, 1);
    }
    prev_x = current_x;
    prev_y = current_y;
    return;
  }
  if (((prev_x!=-1) && (current_x!=-1) && (prev_y!=-1) && (current_y!=-1)) ||
     (current_z != 0)) {
    // (mch) consider warping here
    dx = current_x - prev_x - warp_dx;
    dy = -(current_y - prev_y - warp_dy);
    dz = current_z;
    warp_cursor(warp_home_x-current_x, warp_home_y-current_y);

    DEV_mouse_motion(dx, dy, dz, mouse_button_state, 0);
    prev_x = current_x;
    prev_y = current_y;
  } else {
    if ((current_x!=-1) && (current_y!=-1)) {
      prev_x = current_x;
      prev_y = current_y;
    } else {
      prev_x = current_x = -1;
      prev_y = current_y = -1;
    }
  }
}

void bx_x_gui_c::flush(void)
{
  if (bx_x_display)
    XFlush(bx_x_display);
}

void xkeypress(KeySym keysym, int press_release)
{
  Bit32u key_event;
  bx_bool mouse_toggle = 0;

  if ((keysym == XK_Control_L) || (keysym == XK_Control_R)) {
     mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_CTRL, !press_release);
  } else if (keysym == XK_Alt_L) {
     mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_ALT, !press_release);
  } else if (keysym == XK_F10) {
     mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_F10, !press_release);
  } else if (keysym == XK_F12) {
     mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_F12, !press_release);
  }
  if (mouse_toggle) {
    bx_gui->toggle_mouse_enable();
    return;
  }

  /* Old (no mapping) behavior */
  if (!x11_use_kbd_mapping) {

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
#ifdef XK_KP_Begin
      case XK_KP_Begin:
#endif
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
#endif
        key_event = BX_KEY_KP_INSERT; break;

      case XK_KP_Decimal:
#ifdef XK_KP_Delete
      case XK_KP_Delete:
#endif
        key_event = BX_KEY_KP_DELETE; break;

#ifdef XK_KP_Enter
      case XK_KP_Enter:    key_event = BX_KEY_KP_ENTER; break;
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
#ifdef XK_ISO_Left_Tab
      case XK_ISO_Left_Tab: key_event = BX_KEY_TAB; break;
#endif
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
#ifdef XK_Control_R
      case XK_Control_R:   key_event = BX_KEY_CTRL_R; break;
#endif
      case XK_Shift_L:     key_event = BX_KEY_SHIFT_L; break;
#ifdef XK_ISO_Prev_Group
      case XK_ISO_Prev_Group: key_event = BX_KEY_SHIFT_L; break;
#endif
      case XK_Shift_R:     key_event = BX_KEY_SHIFT_R; break;
#ifdef XK_ISO_Next_Group
      case XK_ISO_Next_Group: key_event = BX_KEY_SHIFT_R; break;
#endif
      case XK_Alt_L:       key_event = BX_KEY_ALT_L; break;
#ifdef XK_Alt_R
      case XK_Alt_R:       key_event = BX_KEY_ALT_R; break;
#endif
      case XK_Caps_Lock:   key_event = BX_KEY_CAPS_LOCK; break;
      case XK_Num_Lock:    key_event = BX_KEY_NUM_LOCK; break;
#ifdef XK_Scroll_Lock
      case XK_Scroll_Lock: key_event = BX_KEY_SCRL_LOCK; break;
#endif
#ifdef XK_Print
      case XK_Print:       key_event = BX_KEY_PRINT; break;
#endif
#ifdef XK_Pause
      case XK_Pause:       key_event = BX_KEY_PAUSE; break;
#endif
#ifdef XK_Break
      case XK_Break:       key_event = BX_KEY_CTRL_BREAK; break;
#endif
      case XK_Insert:      key_event = BX_KEY_INSERT; break;
      case XK_Home:        key_event = BX_KEY_HOME; break;
      case XK_End:         key_event = BX_KEY_END; break;
      case XK_Page_Up:     key_event = BX_KEY_PAGE_UP; break;
      case XK_Page_Down:   key_event = BX_KEY_PAGE_DOWN; break;

      default:
        BX_ERROR(("xkeypress(): keysym %x unhandled!", (unsigned) keysym));
        return;
      break;
    }
  }
  else {
    /* use mapping */
    BXKeyEntry *entry = bx_keymap.findHostKey (keysym);
    if (!entry) {
      BX_ERROR(("xkeypress(): keysym %x unhandled!", (unsigned) keysym));
      return;
    }
    key_event = entry->baseKey;
  }

  if (press_release)
    key_event |= BX_KEY_RELEASED;

  DEV_kbd_gen_scancode(key_event);
}

void bx_x_gui_c::clear_screen(void)
{
  XClearArea(bx_x_display, win, 0, bx_headerbar_y, dimension_x, dimension_y, 0);
}

void bx_x_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                      unsigned long cursor_x, unsigned long cursor_y,
                      bx_vga_tminfo_t *tm_info)
{
  Bit8u *old_line, *new_line, *text_base;
  Bit8u cChar;
  unsigned int curs, hchars, i, j, offset, rows, x, y, xc, yc, yc2, cs_y;
  unsigned new_foreground, new_background;
  Bit8u cfwidth, cfheight, cfheight2, font_col, font_row, font_row2;
  Bit8u split_textrow, split_fontrows;
  bx_bool forceUpdate = 0, split_screen;
  bx_bool blink_state, blink_mode;
  unsigned char cell[64];
  unsigned long text_palette[16];

  blink_mode = (tm_info->blink_flags & BX_TEXT_BLINK_MODE) > 0;
  blink_state = (tm_info->blink_flags & BX_TEXT_BLINK_STATE) > 0;
  if (blink_mode) {
    if (tm_info->blink_flags & BX_TEXT_BLINK_TOGGLE)
      forceUpdate = 1;
  }
  if (charmap_updated) {
    BX_INFO(("charmap update. Font Height is %d",font_height));
    for (unsigned c = 0; c<256; c++) {
      if (char_changed[c]) {
        XFreePixmap(bx_x_display, vgafont[c]);
        bx_bool gfxchar = tm_info->line_graphics && ((c & 0xE0) == 0xC0);
        j = 0;
        memset(cell, 0, sizeof(cell));
        for(i=0; i<font_height*2; i+=2) {
          cell[i] |= ((vga_charmap[(c<<5)+j] & 0x01)<<7);
          cell[i] |= ((vga_charmap[(c<<5)+j] & 0x02)<<5);
          cell[i] |= ((vga_charmap[(c<<5)+j] & 0x04)<<3);
          cell[i] |= ((vga_charmap[(c<<5)+j] & 0x08)<<1);
          cell[i] |= ((vga_charmap[(c<<5)+j] & 0x10)>>1);
          cell[i] |= ((vga_charmap[(c<<5)+j] & 0x20)>>3);
          cell[i] |= ((vga_charmap[(c<<5)+j] & 0x40)>>5);
          cell[i] |= ((vga_charmap[(c<<5)+j] & 0x80)>>7);
          if (gfxchar) {
            cell[i+1] = (vga_charmap[(c<<5)+j] & 0x01);
          }
          j++;
        }

        vgafont[c]=XCreateBitmapFromData(bx_x_display, win,
                        (const char*)cell, 9, font_height);
            if(vgafont[c] == None)
              BX_PANIC(("Can't create vga font [%d]", c));
        char_changed[c] = 0;
      }
    }
    forceUpdate = 1;
    charmap_updated = 0;
  }
  for (i=0; i<16; i++) {
    text_palette[i] = col_vals[tm_info->actl_palette[i]];
  }

  if((tm_info->h_panning != h_panning) || (tm_info->v_panning != v_panning)) {
    forceUpdate = 1;
    h_panning = tm_info->h_panning;
    v_panning = tm_info->v_panning;
  }
  if(tm_info->line_compare != line_compare) {
    forceUpdate = 1;
    line_compare = tm_info->line_compare;
  }

  // first invalidate character at previous and new cursor location
  if ((prev_cursor_y < text_rows) && (prev_cursor_x < text_cols)) {
    curs = prev_cursor_y * tm_info->line_offset + prev_cursor_x * 2;
    old_text[curs] = ~new_text[curs];
  }
  if((tm_info->cs_start <= tm_info->cs_end) && (tm_info->cs_start < font_height) &&
     (cursor_y < text_rows) && (cursor_x < text_cols)) {
    curs = cursor_y * tm_info->line_offset + cursor_x * 2;
    old_text[curs] = ~new_text[curs];
  } else {
    curs = 0xffff;
  }

  rows = text_rows;
  if (v_panning) rows++;
  y = 0;
  cs_y = 0;
  text_base = new_text - tm_info->start_address;
  if (line_compare < dimension_y) {
    split_textrow = (line_compare + v_panning) / font_height;
    split_fontrows = ((line_compare + v_panning) % font_height) + 1;
  } else {
    split_textrow = rows + 1;
    split_fontrows = 0;
  }
  split_screen = 0;
  do {
    hchars = text_cols;
    if (h_panning) hchars++;
    if (split_screen) {
      yc = bx_headerbar_y + line_compare + cs_y * font_height + 1;
      font_row = 0;
      if (rows == 1) {
        cfheight = (dimension_y - line_compare - 1) % font_height;
        if (cfheight == 0) cfheight = font_height;
      } else {
        cfheight = font_height;
      }
    } else if (v_panning) {
      if (y == 0) {
        yc = bx_headerbar_y;
        font_row = v_panning;
        cfheight = font_height - v_panning;
      } else {
        yc = y * font_height + bx_headerbar_y - v_panning;
        font_row = 0;
        if (rows == 1) {
          cfheight = v_panning;
        } else {
          cfheight = font_height;
        }
      }
    } else {
      yc = y * font_height + bx_headerbar_y;
      font_row = 0;
      cfheight = font_height;
    }
    if (!split_screen && (y == split_textrow)) {
      if (split_fontrows < cfheight) cfheight = split_fontrows;
    }
    new_line = new_text;
    old_line = old_text;
    x = 0;
    offset = cs_y * tm_info->line_offset;
    do {
      if (h_panning) {
        if (hchars > text_cols) {
          xc = 0;
          font_col = h_panning;
          cfwidth = font_width - h_panning;
        } else {
          xc = x * font_width - h_panning;
          font_col = 0;
          if (hchars == 1) {
            cfwidth = h_panning;
          } else {
            cfwidth = font_width;
          }
        }
      } else {
        xc = x * font_width;
        font_col = 0;
        cfwidth = font_width;
      }
      if (forceUpdate || (old_text[0] != new_text[0])
          || (old_text[1] != new_text[1])) {

        cChar = new_text[0];
        new_foreground = new_text[1] & 0x0f;
        if (blink_mode) {
          new_background = (new_text[1] & 0x70) >> 4;
          if (!blink_state && (new_text[1] & 0x80))
            new_foreground = new_background;
        } else {
          new_background = (new_text[1] & 0xf0) >> 4;
        }
        XSetForeground(bx_x_display, gc, text_palette[new_foreground]);
        XSetBackground(bx_x_display, gc, text_palette[new_background]);

        XCopyPlane(bx_x_display, vgafont[cChar], win, gc, font_col, font_row, cfwidth, cfheight,
                   xc, yc, 1);
        if (offset == curs) {
          XSetForeground(bx_x_display, gc, text_palette[new_background]);
          XSetBackground(bx_x_display, gc, text_palette[new_foreground]);
          if (font_row == 0) {
            yc2 = yc + tm_info->cs_start;
            font_row2 = tm_info->cs_start;
            cfheight2 = tm_info->cs_end - tm_info->cs_start + 1;
            if ((yc2 + cfheight2) > (dimension_y + bx_headerbar_y)) {
              cfheight2 = dimension_y + bx_headerbar_y - yc2;
            }
          } else {
            if (v_panning > tm_info->cs_start) {
              yc2 = yc;
              font_row2 = font_row;
              cfheight2 = tm_info->cs_end - v_panning + 1;
            } else {
              yc2 = yc + tm_info->cs_start - v_panning;
              font_row2 = tm_info->cs_start;
              cfheight2 = tm_info->cs_end - tm_info->cs_start + 1;
            }
          }
          if (yc2 < (dimension_y + bx_headerbar_y)) {
            XCopyPlane(bx_x_display, vgafont[cChar], win, gc, font_col, font_row2, cfwidth,
                       cfheight2, xc, yc2, 1);
          }
        }
      }
      x++;
      new_text+=2;
      old_text+=2;
      offset+=2;
    } while (--hchars);
    if (!split_screen && (y == split_textrow)) {
      new_text = text_base;
      forceUpdate = 1;
      cs_y = 0;
      if (tm_info->split_hpanning) h_panning = 0;
      rows = ((dimension_y - line_compare + font_height - 2) / font_height) + 1;
      split_screen = 1;
    } else {
      y++;
      cs_y++;
      new_text = new_line + tm_info->line_offset;
      old_text = old_line + tm_info->line_offset;
    }
  } while (--rows);

  h_panning = tm_info->h_panning;
  prev_cursor_x = cursor_x;
  prev_cursor_y = cursor_y;

  XFlush(bx_x_display);
}

int bx_x_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  int len;
  Bit8u *tmp = (Bit8u *)XFetchBytes (bx_x_display, &len);
  // according to man XFetchBytes, tmp must be freed by XFree().  So allocate
  // a new buffer with "new".  The keyboard code will free it with delete []
  // when the paste is done.
  Bit8u *buf = new Bit8u[len];
  memcpy (buf, tmp, len);
  *bytes = buf;
  *nbytes = len;
  XFree (tmp);
  return 1;
}

int bx_x_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  // this writes data to the clipboard.
  BX_INFO (("storing %d bytes to X windows clipboard", len));
  XSetSelectionOwner(bx_x_display, XA_PRIMARY, None, CurrentTime);
  XStoreBytes (bx_x_display, (char *)text_snapshot, len);
  return 1;
}

void bx_x_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  unsigned x, y, y_size;
  unsigned color, offset;
  Bit8u b0, b1, b2, b3;

  if ((y0 + y_tilesize) > dimension_y) {
    y_size = dimension_y - y0;
  } else {
    y_size = y_tilesize;
  }
  switch (guest_bpp) {
    case 8:  // 8 bits per pixel
      for (y=0; y<y_size; y++) {
        for (x=0; x<x_tilesize; x++) {
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
              BX_PANIC(("X_graphics_tile_update: bits_per_pixel %u not implemented",
                (unsigned) imBPP));
              return;
          }
        }
      }
      break;
    default:
      BX_PANIC(("X_graphics_tile_update: bits_per_pixel %u handled by new graphics API",
                (unsigned) guest_bpp));
      return;
  }
  XPutImage(bx_x_display, win, gc, ximage, 0, 0, x0, y0+bx_headerbar_y,
            x_tilesize, y_size);
}

bx_svga_tileinfo_t *bx_x_gui_c::graphics_tile_info(bx_svga_tileinfo_t *info)
{
  info->bpp = ximage->bits_per_pixel;
  info->pitch = ximage->bytes_per_line;
  info->red_shift = 0;
  info->green_shift = 0;
  info->blue_shift = 0;
  info->red_mask = ximage->red_mask;
  info->green_mask = ximage->green_mask;
  info->blue_mask = ximage->blue_mask;

  int i, rf, gf, bf;
  unsigned long red, green, blue;

  i = rf = gf = bf = 0;
  red = ximage->red_mask;
  green = ximage->green_mask;
  blue = ximage->blue_mask;

  while (red || rf || green || gf || blue || bf) {
    if (rf) {
      if (!(red & 1)) {
        info->red_shift = i;
        rf = 0;
      }
    }
    else {
      if (red & 1) {
        rf = 1;
      }
    }

    if (gf) {
      if (!(green & 1)) {
        info->green_shift = i;
        gf = 0;
      }
    }
    else {
      if (green & 1) {
        gf = 1;
      }
    }

    if (bf) {
      if (!(blue & 1)) {
        info->blue_shift = i;
        bf = 0;
      }
    }
    else {
      if (blue & 1) {
        bf = 1;
      }
    }

    i++;
    red >>= 1;
    green >>= 1;
    blue >>= 1;
  }

  info->is_indexed = (default_visual->c_class != TrueColor) &&
                     (default_visual->c_class != DirectColor);
  info->is_little_endian = (ximage->byte_order == LSBFirst);

  return info;
}

Bit8u *bx_x_gui_c::graphics_tile_get(unsigned x0, unsigned y0,
                          unsigned *w, unsigned *h)
{
  if (x0+x_tilesize > dimension_x) {
    *w = dimension_x - x0;
  }
  else {
    *w = x_tilesize;
  }

  if (y0+y_tilesize > dimension_y) {
    *h = dimension_y - y0;
  }
  else {
    *h = y_tilesize;
  }

  return (Bit8u *)ximage->data + ximage->xoffset*ximage->bits_per_pixel/8;
}

void bx_x_gui_c::graphics_tile_update_in_place(unsigned x0, unsigned y0,
                                      unsigned w, unsigned h)
{
  XPutImage(bx_x_display, win, gc, ximage, 0, 0,
            x0, y0+bx_headerbar_y, w, h);
}

bx_bool bx_x_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue)
{
  // returns: 0=no screen update needed (color map change has direct effect)
  //          1=screen updated needed (redraw using current colormap)
  XColor color;

  color.flags = DoRed | DoGreen | DoBlue;
  color.red   = red << 8;
  color.green = green << 8;
  color.blue  = blue << 8;

  if (x11_private_colormap) {
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

void bx_x_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth, unsigned bpp)
{
  if ((bpp == 8) || (bpp == 15) || (bpp == 16) || (bpp == 24) || (bpp == 32)) {
    guest_bpp = bpp;
  } else {
    BX_PANIC(("%d bpp graphics mode not supported", bpp));
  }
  guest_textmode = (fheight > 0);
  guest_xres = x;
  guest_yres = y;
  if (guest_textmode) {
    font_height = fheight;
    font_width = fwidth;
    text_cols = x / font_width;
    text_rows = y / font_height;
  }
  if ((x != dimension_x) || (y != dimension_y)) {
    XSizeHints hints;
    long supplied_return;

    if (XGetWMNormalHints(bx_x_display, win, &hints, &supplied_return) &&
         supplied_return & PMaxSize) {
      hints.max_width = hints.min_width = x;
      hints.max_height = hints.min_height = y+bx_headerbar_y+bx_statusbar_y;
      XSetWMNormalHints(bx_x_display, win, &hints);
    }
    XResizeWindow(bx_x_display, win, x, y+bx_headerbar_y+bx_statusbar_y);
    dimension_x = x;
    dimension_y = y;
    warp_home_x = x / 2;
    warp_home_y = y / 2;
  }
}

void bx_x_gui_c::show_headerbar(void)
{
  unsigned xorigin;
  int xleft, xright, sb_ypos;

  sb_ypos = dimension_y + bx_headerbar_y;
  // clear header bar and status bar area to white
  XFillRectangle(bx_x_display, win, gc_headerbar_inv, 0,0, dimension_x, bx_headerbar_y);
  XFillRectangle(bx_x_display, win, gc_headerbar_inv, 0,sb_ypos, dimension_x, bx_statusbar_y);

  xleft = 0;
  xright = dimension_x;
  for (unsigned i=0; i<bx_headerbar_entries; i++) {
    if (bx_headerbar_entry[i].alignment == BX_GRAVITY_LEFT) {
      xorigin = bx_headerbar_entry[i].xorigin;
      xleft += bx_headerbar_entry[i].xdim;
    }
    else {
      xorigin = dimension_x - bx_headerbar_entry[i].xorigin;
      xright = xorigin;
    }
    if (xright < xleft) break;
    XCopyPlane(bx_x_display, bx_headerbar_entry[i].bitmap, win, gc_headerbar,
      0,0, bx_headerbar_entry[i].xdim, bx_headerbar_entry[i].ydim,
              xorigin, 0, 1);
  }
  for (unsigned i=0; i<12; i++) {
    xleft = bx_statusitem_pos[i];
    if (i > 0) {
      XDrawLine(bx_x_display, win, gc_inv, xleft, sb_ypos+1, xleft,
                sb_ypos+bx_statusbar_y);
      if (i <= statusitem_count) {
        set_status_text(i, statusitem[i-1].text, bx_statusitem_active[i]);
      }
    } else {
      set_status_text(0, bx_status_info_text, 0);
    }
  }
}

unsigned bx_x_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  if (bx_bitmap_entries >= BX_MAX_PIXMAPS) {
    BX_PANIC(("x: too many pixmaps, increase BX_MAX_PIXMAPS"));
  }

  bx_bitmaps[bx_bitmap_entries].bmap =
    XCreateBitmapFromData(bx_x_display, win, (const char *) bmap, xdim, ydim);
  bx_bitmaps[bx_bitmap_entries].xdim = xdim;
  bx_bitmaps[bx_bitmap_entries].ydim = ydim;
  if (!bx_bitmaps[bx_bitmap_entries].bmap) {
    BX_PANIC(("x: could not create bitmap"));
  }
  bx_bitmap_entries++;
  return(bx_bitmap_entries-1); // return index as handle
}

unsigned bx_x_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  unsigned hb_index;

  if ((bx_headerbar_entries+1) > BX_MAX_HEADERBAR_ENTRIES)
    BX_PANIC(("x: too many headerbar entries, increase BX_MAX_HEADERBAR_ENTRIES"));

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

void bx_x_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
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

void headerbar_click(int x, int y)
{
  int xorigin;

  // assuming y is in bounds
  UNUSED(y);
  for (unsigned i=0; i<bx_headerbar_entries; i++) {
    if (bx_headerbar_entry[i].alignment == BX_GRAVITY_LEFT)
      xorigin = bx_headerbar_entry[i].xorigin;
    else
      xorigin = dimension_x - bx_headerbar_entry[i].xorigin;
    if ((x>=xorigin) && (x<(xorigin+int(bx_headerbar_entry[i].xdim)))) {
      bx_headerbar_entry[i].f();
      return;
    }
  }
}

void bx_x_gui_c::exit(void)
{
  if (!x_init_done) return;

  // Delete the font bitmaps
  for (int i=0; i<256; i++) {
    //if (vgafont[i] != NULL)
    XFreePixmap(bx_x_display,vgafont[i]);
  }

  if (mouse_captured)
    enable_cursor();

#if BX_DEBUGGER && BX_DEBUGGER_GUI
  if (SIM->has_debug_gui()) {
    close_debug_dialog();
  }
#endif

  if (bx_x_display)
    XCloseDisplay(bx_x_display);

  BX_INFO(("Exit"));
}

static void warp_cursor (int dx, int dy)
{
  if (mouse_captured && (warp_dx || warp_dy || dx || dy)) {
     warp_dx = dx;
     warp_dy = dy;
     XWarpPointer(bx_x_display, None, None, 0, 0, 0, 0, dx, dy);
  }
}

static void disable_cursor()
{
  static Cursor cursor;
  static unsigned cursor_created = 0;

  static int shape_width = 16,
             shape_height = 16,
             mask_width = 16,
             mask_height = 16;

  static Bit32u shape_bits[(16*16)/32] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
  };
  static Bit32u mask_bits[(16*16)/32] = {
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

static void enable_cursor()
{
  XUndefineCursor(bx_x_display, win);
}

/* convertStringToXKeysym is a keymap callback
 * used when reading the keymap file.
 * It converts a Symblic String to a GUI Constant
 *
 * It returns a Bit32u constant or BX_KEYMAP_UNKNOWN if it fails
 */
static Bit32u convertStringToXKeysym (const char *string)
{
  if (strncmp ("XK_", string, 3) != 0)
    return BX_KEYMAP_UNKNOWN;
  KeySym keysym=XStringToKeysym(string+3);

  // failure, return unknown
  if(keysym==NoSymbol) return BX_KEYMAP_UNKNOWN;

  return((Bit32u)keysym);
}

#if BX_USE_IDLE_HACK

/* BX_USE_IDLE_HACK: a small idle hack by
 * Roland.Mainz@informatik.med.uni-giessen.de to prevent bochs
 * from consuming 100% CPU time even when it is not required (for
 * example, the OS in the emulator calls HLT to wait for an interupt)
 * pro:
 * - no more 100% CPU usage
 * contra:
 * - we're sleeping too long
 * - bochs still consumes ~10%-20% CPU time while executing an idle
 *   linux kernel
 * - this is an hack
 */

/* XPeekEvent() with timeout
 * (adopted from mozilla/gfx/src/xprint/xprintutil_printtofile.c#XNextEventTimeout())
 */
static
Bool XPeekEventTimeout(Display *display, XEvent *event_return, struct timeval *timeout)
{
  int    res;
  fd_set readfds;
  int    display_fd = XConnectionNumber(display);

  /* small shortcut... */
  if(timeout == NULL)
  {
      XPeekEvent(display, event_return);
      return(True);
  }

  FD_ZERO(&readfds);
  FD_SET(display_fd, &readfds);

  /* Note/bug: In the case of internal X events (like used to trigger callbacks
   * registered by XpGetDocumentData()&co.) select() will return with "new info"
   * - but XNextEvent() below processes these _internal_ events silently - and
   * will block if there are no other non-internal events.
   * The workaround here is to check with XEventsQueued() if there are non-internal
   * events queued - if not select() will be called again - unfortunately we use
   * the old timeout here instead of the "remaining" time... (this only would hurt
   * if the timeout would be really long - but for current use with values below
   * 1/2 secs it does not hurt... =:-)
   */
  while(XEventsQueued(display, QueuedAfterFlush) == 0)
  {
    res = select(display_fd+1, &readfds, NULL, NULL, timeout);

    switch(res)
    {
        case -1: /* select() error - should not happen */
          if (errno == EINTR)
             break; // caused e.g. by alarm(3)
          perror("XPeekEventTimeout: select() failure");
          return(False);

        case  0: /* timeout */
          return(False);
    }
  }

  XPeekEvent(display, event_return);
  return(True);
}

void bx_x_gui_c::sim_is_idle()
{
  XEvent dummy;
  struct timeval   timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 1000; /* 1/1000 s */
  XPeekEventTimeout(bx_x_display, &dummy, &timeout);
}
#endif /* BX_USE_IDLE_HACK */

void bx_x_gui_c::beep_on(float frequency)
{
  BX_INFO(("X11 Beep ON (frequency=%.2f)",frequency));
}

void bx_x_gui_c::beep_off()
{
  BX_INFO(("X11 Beep OFF"));
}

void bx_x_gui_c::get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp)
{
  int num_sizes;
  Rotation original_rotation;
  int event_base, error_base;

  Display *dpy = XOpenDisplay(NULL);
  if (dpy == NULL) {
    BX_PANIC(("Cannot connect to X display"));
  }
  Window root = RootWindow(dpy, 0);

  if (XRRQueryExtension (dpy, &event_base, &error_base)) {
    XRRScreenSize *xrrs = XRRSizes(dpy, 0, &num_sizes);
    XRRScreenConfiguration *conf = XRRGetScreenInfo(dpy, root);
    SizeID original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);
    *xres = xrrs[original_size_id].width;
    *yres = xrrs[original_size_id].height;
    free(conf);
  }
  else {
    int screen = DefaultScreen(dpy);
    *xres = DisplayWidth(dpy, screen);
    *yres = DisplayHeight(dpy, screen);
   }
  XCloseDisplay(dpy);
  // always return 32 bit depth
  *bpp = 32;
}

void bx_x_gui_c::set_mouse_mode_absxy(bx_bool mode)
{
  x11_mouse_mode_absxy = mode;
}

#if BX_SHOW_IPS
void bx_x_gui_c::show_ips(Bit32u ips_count)
{
  if (x11_mouse_msg_counter == 0) {
    if (!x11_ips_update && !x11_hide_ips) {
      ips_count /= 1000;
      sprintf(x11_ips_text, "IPS: %u.%3.3uM", ips_count / 1000, ips_count % 1000);
      x11_ips_update = 1;
    }
  } else {
    x11_mouse_msg_counter--;
  }
}
#endif

// X11 control class

enum {
  XDC_BUTTON = 0,
  XDC_EDIT,
  XDC_CHECKBOX
};

class x11_control_c {
public:
  x11_control_c(int type, int x, int y, unsigned int w,
                unsigned int h, const char *text);
  virtual ~x11_control_c();

  void draw(Display *display, Window win, GC gc);
  void draw_rect(Display *display, Window win, GC gc);
  void draw_text(Display *display, Window win, GC gc, const char *text);
  int  test(XButtonEvent *bev);
  int  get_type() {return type;}
  const char* get_text() {return text;}
  // checkbox
  int  get_status() {return status;}
  void set_status(bx_bool new_stat) {status = new_stat;}
  // edit
  void set_maxlen(unsigned int max);
  int process_input(KeySym key, const char *str);
  const char* get_value() {return value;}
private:
  unsigned int width, height;
  int type, xmin, xmax, ymin, ymax;
  const char *text;
  // checkbox
  bx_bool status;
  // edit
  char *value;
  char editstr[27];
  unsigned int len, pos, max;
};

x11_control_c::x11_control_c(int _type, int x, int y, unsigned int w,
                             unsigned int h, const char *_text)
{
  type = _type;
  xmin = x;
  ymin = y;
  width = w;
  height = h;
  xmax = x + width;
  ymax = y + height;
  if (type == XDC_EDIT) {
    len = strlen(_text);
    max = len;
    value = (char*)malloc(max+1);
    strcpy(value, _text);
    pos = (len < 25) ? 0 : (len - 24);
    strncpy(editstr, value+pos, 24);
    editstr[len-pos] = 0;
    text = editstr;
  } else {
    text = _text;
    value = NULL;
    if (type == XDC_CHECKBOX) {
      status = !strcmp(text, "X");
    }
  }
}

x11_control_c::~x11_control_c()
{
  if (value != NULL) free(value);
}

void x11_control_c::draw(Display *display, Window win, GC gc)
{
  int ty;

  XDrawRectangle(display, win, gc, xmin, ymin, width, height);
  ty = ymin + height - (height - 12) / 2;
  XDrawImageString(display, win, gc, xmin+4, ty, (char *)text, strlen(text));
}

void x11_control_c::draw_rect(Display *display, Window win, GC gc)
{
  XDrawRectangle(display, win, gc, xmin - 2, ymin - 2, width + 4, height + 4);
}

void x11_control_c::draw_text(Display *display, Window win, GC gc, const char *_text)
{
  int ty;

  ty = ymin + height - (height - 12) / 2;
  XDrawImageString(display, win, gc, xmin+4, ty, (char *)_text, strlen(_text));
}

int x11_control_c::test(XButtonEvent *bev)
{
  if ((bev->y > ymin) && (bev->y < ymax)) {
    if ((bev->x > xmin) && (bev->x < xmax)) {
      return 1;
    }
  }
  return 0;
}

void x11_control_c::set_maxlen(unsigned int _max)
{
  max = _max;
  value = (char*)realloc(value, max+1);
}

int x11_control_c::process_input(KeySym key, const char *str)
{
  int ret = 0;

  if (key == XK_BackSpace) {
    if (len > 0) {
      value[--len] = 0;
      if (pos > 0) pos--;
      ret = 1;
    }
  } else if ((key == 0) && (len < max)) {
    strcat(value, str);
    len = strlen(value);
    if (len > 24) pos++;
    ret = 1;
  }
  strncpy(editstr, value+pos, 24);
  editstr[len-pos] = 0;
  return ret;
}

// X11 static text structure

typedef struct _x11_static_t {
  char *text;
  int x, y;
  struct _x11_static_t *next;
} x11_static_t;

// X11 dialog class

class x11_dialog_c {
public:
  x11_dialog_c(char *name, int width, int height, int num_ctrls);
  virtual ~x11_dialog_c();

  x11_control_c* add_control(int type, int x, int y, unsigned int width,
                             unsigned int height, const char *text);
  void add_static_text(int x, int y, const char *text, int length);
  void draw_text(Display *display, int x, int y, const char *text, int length);
  int run(int start_ctrl, int ok, int cancel);
private:
  Window dlgwin;
  GC gc, gc_inv;
  int ctrl_cnt, cur_ctrl, old_ctrl;
  x11_control_c **controls;
  x11_static_t *static_items;
};

x11_dialog_c::x11_dialog_c(char *name, int width, int height, int num_ctrls)
{
  Window dialogw;
  XSizeHints hint;
  unsigned long black_pixel, white_pixel;

  hint.flags = PPosition | PSize | PMinSize | PMaxSize;
  hint.x = 100;
  hint.y = 100;
  hint.width = hint.min_width = hint.max_width = width;
  hint.height = hint.min_height = hint.max_height = height;
  black_pixel = BlackPixel(bx_x_display, bx_x_screen_num);
  white_pixel = WhitePixel(bx_x_display, bx_x_screen_num);
  dialogw = XCreateSimpleWindow(bx_x_display, RootWindow(bx_x_display,bx_x_screen_num),
    hint.x, hint.y, hint.width, hint.height, 4, black_pixel, white_pixel);
  XSetStandardProperties(bx_x_display, dialogw, name, name, None, NULL, 0, &hint);
  Atom wm_delete = XInternAtom(bx_x_display, "WM_DELETE_WINDOW", 1);
  XSetWMProtocols(bx_x_display, dialogw, &wm_delete, 1);

  gc = XCreateGC(bx_x_display, dialogw, 0, 0);
  gc_inv = XCreateGC(bx_x_display, dialogw, 0, 0);
  XSetState(bx_x_display, gc_inv, white_pixel, black_pixel, GXcopy, AllPlanes);
  XSetBackground(bx_x_display, gc, WhitePixel(bx_x_display, bx_x_screen_num));
  XSetForeground(bx_x_display, gc, BlackPixel(bx_x_display, bx_x_screen_num));

  XSelectInput(bx_x_display, dialogw, ButtonPressMask
               | ButtonReleaseMask
               | KeyPressMask
               | KeyReleaseMask
               | ExposureMask
               | PointerMotionMask
               | EnterWindowMask
               | LeaveWindowMask);
  XMapWindow(bx_x_display, dialogw);
  XFlush(bx_x_display);
  dlgwin = dialogw;
  ctrl_cnt = num_ctrls;
  controls = new x11_control_c* [num_ctrls];
  static_items = NULL;
  cur_ctrl = 0;
  old_ctrl = -1;
}

x11_dialog_c::~x11_dialog_c()
{
  x11_static_t *temp;

  for (int i = 0; i < ctrl_cnt; i++) {
    delete controls[i];
  }
  delete [] controls;
  while (static_items != NULL) {
    temp = static_items;
    static_items = temp->next;
    delete [] temp->text;
    delete temp;
  }
  XFreeGC(bx_x_display, gc);
  XFreeGC(bx_x_display, gc_inv);
  XDestroyWindow(bx_x_display, dlgwin);
}

x11_control_c* x11_dialog_c::add_control(int type, int x, int y, unsigned int width,
                                         unsigned int height, const char *text)
{
  x11_control_c *xctrl = new x11_control_c(type, x, y, width, height, text);
  if (cur_ctrl < ctrl_cnt) {
    controls[cur_ctrl++] = xctrl;
  }
  return xctrl;
}

void x11_dialog_c::add_static_text(int x, int y, const char *text, int length)
{
  x11_static_t *static_item, *temp;

  static_item = (x11_static_t*)malloc(sizeof(x11_static_t));
  static_item->x = x;
  static_item->y = y;
  static_item->text = new char[length+1];
  strncpy(static_item->text, text, length);
  static_item->text[length] = 0;
  static_item->next = NULL;
  if (static_items == NULL) {
    static_items = static_item;
  } else {
    temp = static_items;
    while (temp->next) {
      temp = temp->next;
    }
    temp->next = static_item;
  }
}

void x11_dialog_c::draw_text(Display *display, int x, int y, const char *text, int length)
{
  XDrawImageString(display, dlgwin, gc, x, y, text, length);
}

int x11_dialog_c::run(int start_ctrl, int ok, int cancel)
{
  XEvent xevent;
  KeySym key;
  bx_bool init = 0, done = 0, valid = 0, status;
  int i;
  char text[10], editstr[27];
  x11_static_t *temp;

  if (start_ctrl < 0) {
    cur_ctrl = ctrl_cnt - 1;
  } else {
    cur_ctrl = start_ctrl;
  }
  while (!done) {
    XNextEvent(bx_x_display, &xevent);
    switch (xevent.type) {
      case Expose:
        if (xevent.xexpose.count == 0) {
          temp = static_items;
          while (temp != NULL) {
            draw_text(xevent.xexpose.display, temp->x, temp->y, temp->text, strlen(temp->text));
            temp = temp->next;
          }
          for (i = 0; i < ctrl_cnt; i++) {
            controls[i]->draw(xevent.xexpose.display, dlgwin, gc);
          }
          old_ctrl = cur_ctrl - 1;
          if (old_ctrl < 0) old_ctrl = 1;
          init = 1;
        }
        break;
      case ButtonPress:
        if (xevent.xbutton.button == Button1) {
          for (i = 0; i < ctrl_cnt; i++) {
            if (controls[i]->test(&xevent.xbutton)) {
              cur_ctrl = i;
              if (controls[cur_ctrl]->get_type() == XDC_BUTTON) {
                valid = 1;
              } else if (controls[cur_ctrl]->get_type() == XDC_CHECKBOX) {
                status = !controls[cur_ctrl]->get_status();
                controls[cur_ctrl]->set_status(status);
                controls[cur_ctrl]->draw_text(bx_x_display, dlgwin, gc, status ? "X":" ");
              }
              break;
            }
          }
        }
        break;
      case ButtonRelease:
        if ((xevent.xbutton.button == Button1) && (valid == 1)) {
          done = 1;
        }
        break;
      case KeyPress:
        i = XLookupString((XKeyEvent *)&xevent, text, 10, &key, 0);
        if (key == XK_Tab) {
          cur_ctrl++;
          if (cur_ctrl >= ctrl_cnt) cur_ctrl = 0;
        } else if (key == XK_Escape) {
          cur_ctrl = cancel;
          done = 1;
        } else if (controls[cur_ctrl]->get_type() == XDC_EDIT) {
          if (key == XK_Return) {
            cur_ctrl = ok;
            done = 1;
          } else if (key == XK_BackSpace) {
            if (controls[cur_ctrl]->process_input(key, "")) {
              old_ctrl = cur_ctrl ^ 1;
            }
          } else if (i == 1) {
            if (controls[cur_ctrl]->process_input(0, text)) {
              old_ctrl = cur_ctrl ^ 1;
            }
          }
        } else if (controls[cur_ctrl]->get_type() == XDC_CHECKBOX) {
          if (key == XK_space) {
            status = !controls[cur_ctrl]->get_status();
            controls[cur_ctrl]->set_status(status);
            controls[cur_ctrl]->draw_text(bx_x_display, dlgwin, gc, status ? "X":" ");
          } else if (key == XK_Return) {
            cur_ctrl = ok;
            done = 1;
          }
        } else if ((key == XK_space) || (key == XK_Return)) {
          done = 1;
        }
        break;
      case ClientMessage:
        if (!strcmp(XGetAtomName(bx_x_display, xevent.xclient.message_type), "WM_PROTOCOLS")) {
          cur_ctrl = cancel;
          done = 1;
        }
        break;
      case LeaveNotify:
        valid = 0;
        break;
    }
    if (init && (cur_ctrl != old_ctrl)) {
      if (controls[old_ctrl]->get_type() == XDC_EDIT) {
        sprintf(editstr, "%s ", controls[old_ctrl]->get_text());
        controls[old_ctrl]->draw_text(bx_x_display, dlgwin, gc, editstr);
      } else {
        controls[old_ctrl]->draw_rect(bx_x_display, dlgwin, gc_inv);
      }
      if (controls[cur_ctrl]->get_type() == XDC_EDIT) {
        sprintf(editstr, "%s_ ", controls[cur_ctrl]->get_text());
        controls[cur_ctrl]->draw_text(bx_x_display, dlgwin, gc, editstr);
      } else {
        controls[cur_ctrl]->draw_rect(bx_x_display, dlgwin, gc);
      }
      old_ctrl = cur_ctrl;
    }
  }
  return cur_ctrl;
}

// X11 dialog box functions

int x11_ask_dialog(BxEvent *event)
{
#if BX_DEBUGGER || BX_GDBSTUB
  const int button_x[4] = { 36, 121, 206, 291 };
  const int ask_code[4] = { BX_LOG_ASK_CHOICE_CONTINUE,
                            BX_LOG_ASK_CHOICE_CONTINUE_ALWAYS,
                            BX_LOG_ASK_CHOICE_ENTER_DEBUG,
                            BX_LOG_ASK_CHOICE_DIE };
  const int num_ctrls = 4;
#else
  const int button_x[3] = { 81, 166, 251 };
  const int ask_code[3] = { BX_LOG_ASK_CHOICE_CONTINUE,
                            BX_LOG_ASK_CHOICE_CONTINUE_ALWAYS,
                            BX_LOG_ASK_CHOICE_DIE };
  const int num_ctrls = 3;
#endif
  int level, cpos;
  int retcode = -1;
  int control = num_ctrls - 1;
  char name[16], device[18], message[512];

  level = event->u.logmsg.level;
  strcpy(name, SIM->get_log_level_name(level));
  sprintf(device, "Device: %s", event->u.logmsg.prefix);
  sprintf(message, "Message: %s", event->u.logmsg.msg);
  x11_dialog_c *xdlg = new x11_dialog_c(name, 400, 115, num_ctrls);
  xdlg->add_static_text(20, 25, device, strlen(device));
  if (strlen(message) > 62) {
    cpos = 62;
    while ((cpos > 0) && (!isspace(message[cpos]))) cpos--;
    xdlg->add_static_text(20, 45, message, cpos);
    xdlg->add_static_text(74, 63, message+cpos+1, strlen(message)-cpos-1);
  } else {
    xdlg->add_static_text(20, 45, message, strlen(message));
  }
  xdlg->add_control(XDC_BUTTON, button_x[0] + 2, 80, 65, 20, "Continue");
  xdlg->add_control(XDC_BUTTON, button_x[1] + 2, 80, 65, 20, "Alwayscont");
#if BX_DEBUGGER || BX_GDBSTUB
  xdlg->add_control(XDC_BUTTON, button_x[2] + 2, 80, 65, 20, "Debugger");
#endif
  xdlg->add_control(XDC_BUTTON, button_x[num_ctrls-1] + 2, 80, 65, 20, "Quit");
  control = xdlg->run(num_ctrls-1, 0, num_ctrls-1);
  retcode = ask_code[control];
  delete xdlg;
  return retcode;
}

int x11_string_dialog(bx_param_string_c *param, bx_param_enum_c *param2)
{
  x11_control_c *xctl_edit, *xbtn_status = NULL;
  int control = 0, h, num_ctrls, ok_button;
  bx_bool status = 0;
  char name[80], text[10];
  const char *value;

  if (param2 != NULL) {
    strcpy(name, "First CD-ROM image/device");
    status = (param2->get() == BX_INSERTED);
    h = 110;
    ok_button = 2;
    num_ctrls = 4;
  } else {
    if (param->get_label() != NULL) {
      strcpy(name, param->get_label());
    } else {
      strcpy(name, param->get_name());
    }
    h = 90;
    ok_button = 1;
    num_ctrls = 3;
  }
  x11_dialog_c *xdlg = new x11_dialog_c(name, 250, h, num_ctrls);
  xctl_edit = xdlg->add_control(XDC_EDIT, 45, 20, 160, 20, param->getptr());
  xctl_edit->set_maxlen(param->get_maxsize());
  if (param2 != NULL) {
    strcpy(text, status ? "X":" ");
    xbtn_status = xdlg->add_control(XDC_CHECKBOX, 45, 50, 15, 16, text);
    xdlg->add_static_text(70, 62, "Inserted", 8);
  }
  xdlg->add_control(XDC_BUTTON, 55, h - 30, 65, 20, "OK");
  xdlg->add_control(XDC_BUTTON, 130, h - 30, 65, 20, "Cancel");
  control = xdlg->run(0, ok_button, num_ctrls-1);
  if (control == ok_button) {
    value = xctl_edit->get_value();
    if (param2 != NULL) {
      status = xbtn_status->get_status();
      if (status == 1) {
        if (strlen(value) > 0) {
          param->set(value);
          param2->set(1);
        } else {
          param2->set(0);
        }
      } else {
        param2->set(0);
      }
    } else {
      param->set(value);
    }
  }
  delete xdlg;
  if (control == ok_button) {
    return 1;
  } else {
    return -1;
  }
}

int x11_yesno_dialog(bx_param_bool_c *param)
{
  int button_x[2], size_x, size_y;
  int control, ypos;
  unsigned int cpos1, cpos2, len, maxlen, lines;
  char name[80], message[512];

  if (param->get_label() != NULL) {
    strcpy(name, param->get_label());
  } else {
    strcpy(name, param->get_name());
  }
  strcpy(message, param->get_description());
  cpos1 = 0;
  cpos2 = 0;
  lines = 0;
  maxlen = 0;
  while (cpos2 < strlen(message)) {
    lines++;
    while ((cpos2 < strlen(message)) && (message[cpos2] != 0x0a)) cpos2++;
    len = cpos2 - cpos1;
    if (len > maxlen) maxlen = len;
    cpos2++;
    cpos1 = cpos2;
  }
  if (maxlen < 36) {
    size_x = 250;
    button_x[0] = 55;
    button_x[1] = 130;
  } else {
    size_x = 10 + maxlen * 7;
    button_x[0] = (size_x / 2) - 70;
    button_x[1] = (size_x / 2) + 5;
  }
  if (lines < 3) {
    size_y = 90;
  } else {
    size_y = 60 + lines * 15;
  }
  control = 1 - param->get();
  x11_dialog_c *xdlg = new x11_dialog_c(name, size_x, size_y, 2);
  cpos1 = 0;
  cpos2 = 0;
  ypos = 34;
  while (cpos2 < strlen(message)) {
    while ((cpos2 < strlen(message)) && (message[cpos2] != 0x0a)) cpos2++;
    len = cpos2 - cpos1;
    xdlg->add_static_text(20, ypos, message+cpos1, len);
    cpos2++;
    cpos1 = cpos2;
    ypos += 15;
  }
  xdlg->add_control(XDC_BUTTON, button_x[0], size_y - 30, 65, 20, "Yes");
  xdlg->add_control(XDC_BUTTON, button_x[1], size_y - 30, 65, 20, "No");
  control = xdlg->run(control, 0, 1);
  param->set(1 - control);
  delete xdlg;
  return control;
}

BxEvent *x11_notify_callback (void *unused, BxEvent *event)
{
  int opts;
  bx_param_c *param;
  bx_param_string_c *sparam;
  bx_param_enum_c *eparam;
  bx_list_c *list;

  switch (event->type)
  {
    case BX_SYNC_EVT_LOG_ASK:
      event->retcode = x11_ask_dialog(event);
      return event;
    case BX_SYNC_EVT_ASK_PARAM:
      param = event->u.param.param;
      if (param->get_type() == BXT_PARAM_STRING) {
        sparam = (bx_param_string_c *)param;
        opts = sparam->get_options();
        if ((opts & sparam->IS_FILENAME) == 0) {
          event->retcode = x11_string_dialog(sparam, NULL);
          return event;
        } else if ((opts & sparam->SAVE_FILE_DIALOG) ||
                   (opts & sparam->SELECT_FOLDER_DLG)) {
          event->retcode = x11_string_dialog(sparam, NULL);
          return event;
        }
      } else if (param->get_type() == BXT_LIST) {
        list = (bx_list_c *)param;
        sparam = (bx_param_string_c *)list->get_by_name("path");
        eparam = (bx_param_enum_c *)list->get_by_name("status");
        event->retcode = x11_string_dialog(sparam, eparam);
        return event;
      } else if (param->get_type() == BXT_PARAM_BOOL) {
        event->retcode = x11_yesno_dialog((bx_param_bool_c *)param);
        return event;
      }
    case BX_SYNC_EVT_TICK: // called periodically by siminterface.
    case BX_ASYNC_EVT_REFRESH: // called when some bx_param_c parameters have changed.
      // fall into default case
    default:
      return (*old_callback)(old_callback_arg, event);
  }
}

#endif /* if BX_WITH_X11 */
