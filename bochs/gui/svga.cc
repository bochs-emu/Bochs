/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2013  The Bochs Project
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

#define _MULTI_THREAD

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "param_names.h"
#include "iodev.h"
#if BX_WITH_SVGA

#include <stdlib.h>
#include </usr/include/vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>
#include <vgamouse.h>

#include "font/vga.bitmap.h"
//#include "icon_bochs.h"

class bx_svga_gui_c : public bx_gui_c {
public:
  bx_svga_gui_c (void);
  DECLARE_GUI_VIRTUAL_METHODS()
  virtual void set_display_mode (disp_mode_t newmode);
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_svga_gui_c *theGui = NULL;

IMPLEMENT_GUI_PLUGIN_CODE(svga)

#define LOG_THIS theGui->

static unsigned res_x, res_y;
static unsigned fontwidth, fontheight;
static unsigned char vgafont[256 * 16];
static int clut8 = 0;
GraphicsContext *screen = NULL;
static int save_vga_mode;
static int save_vga_pal[256 * 3];
static Bit8u fontbuffer[0x2000];

static bx_bool ctrll_pressed = 0;
static unsigned int text_rows=25, text_cols=80;
static unsigned prev_cursor_x=0;
static unsigned prev_cursor_y=0;

void keyboard_handler(int scancode, int press);
void mouse_handler(int button, int dx, int dy, int dz,
		    int drx, int dry, int drz);

unsigned char reverse_byteorder(unsigned char b)
{
    unsigned char ret = 0;

    for (unsigned i=0;i<8;i++){
	ret |= (b & 0x01) << (7 - i);
	b >>= 1;
    }
    return ret;
}

void create_vga_font()
{
    memcpy(vgafont, bx_vgafont, sizeof(bx_vgafont));

    for (unsigned i=0;i< sizeof(bx_vgafont);i++) {
	vgafont[i] = reverse_byteorder(vgafont[i]);
    }
}

bx_svga_gui_c::bx_svga_gui_c()
{
  put("SVGA");
}

void bx_svga_gui_c::specific_init(int argc, char **argv, unsigned header_bar_y)
{
  put("VGAGUI");
  if (vga_init() != 0) {
    LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
    BX_PANIC(("Unable to initialize SVGAlib"));
    return;
  }

  screen = gl_allocatecontext();

  fontwidth = 8;
  fontheight = 16;
  dimension_update(640,400);
  create_vga_font();
  gl_setfont(fontwidth, fontheight, (void *)vgafont);
  gl_setwritemode(FONT_COMPRESSED);

  keyboard_init();
  keyboard_seteventhandler((__keyboard_handler) keyboard_handler);

  vga_setmousesupport(1);
  mouse_seteventhandler((__mouse_handler) mouse_handler);
  if (vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_FLAGS) & VGA_CLUT8) {
    vga_ext_set(VGA_EXT_SET, VGA_CLUT8);
    clut8 = 1;
  }
  // Save settings to prepare for mode transition in set_display_mode.
  // If DISP_MODE_SIM is called first, these values will be used.
  save_vga_mode = vga_getcurrentmode();
  vga_getpalvec(0, 256, save_vga_pal);
}

void bx_svga_gui_c::text_update(
    Bit8u *old_text,
    Bit8u *new_text,
    unsigned long cursor_x,
    unsigned long cursor_y,
    bx_vga_tminfo_t *tm_info)
{
  Bit8u *old_line, *new_line;
  unsigned int curs, hchars, i, j, offset, rows, x, y;
  char s[] = " ";
  int fg, bg;
  bx_bool force_update = 0, blink_state, blink_mode;
  int text_palette[16];

  // first check if the screen needs to be redrawn completely
  blink_mode = (tm_info->blink_flags & BX_TEXT_BLINK_MODE) > 0;
  blink_state = (tm_info->blink_flags & BX_TEXT_BLINK_STATE) > 0;
  if (blink_mode) {
    if (tm_info->blink_flags & BX_TEXT_BLINK_TOGGLE)
      force_update = 1;
  }
  if (charmap_updated) {
    BX_INFO(("charmap update. Font Height is %d", fontheight));
    for (unsigned c = 0; c<256; c++) {
      if (char_changed[c]) {
        j = c * fontheight;
        for(i=0; i<fontheight; i++) {
          fontbuffer[j++] = vga_charmap[(c<<5)+i];
        }
        char_changed[c] = 0;
      }
    }
    gl_setfont(fontwidth, fontheight, (void *)fontbuffer);
    force_update = 1;
    charmap_updated = 0;
  }
  for (i=0; i<16; i++) {
    text_palette[i] = tm_info->actl_palette[i];
  }

  // invalidate character at previous and new cursor location
  if((prev_cursor_y < text_rows) && (prev_cursor_x < text_cols)) {
    curs = prev_cursor_y * tm_info->line_offset + prev_cursor_x * 2;
    old_text[curs] = ~new_text[curs];
  }
  if((tm_info->cs_start <= tm_info->cs_end) && (tm_info->cs_start < fontheight) &&
     (cursor_y < text_rows) && (cursor_x < text_cols)) {
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
    x = 0;
    offset = y * tm_info->line_offset;
    do {
      if (force_update || (old_text[0] != new_text[0])
          || (old_text[1] != new_text[1])) {
        s[0] = new_text[0];
        fg = text_palette[new_text[1] & 0x0F];
        if (blink_mode) {
          bg = text_palette[(new_text[1] & 0x70) >> 4];
          if (!blink_state && (new_text[1] & 0x80))
            fg = bg;
        } else {
          bg = text_palette[(new_text[1] & 0xF0) >> 4];
        }
        if (offset == curs) {
          gl_setfontcolors(fg, bg);
        } else {
          gl_setfontcolors(bg, fg);
        }
        gl_write(x * fontwidth, y * fontheight, s);
      }
      x++;
      new_text+=2;
      old_text+=2;
      offset+=2;
    } while (--hchars);
    y++;
    new_text = new_line + tm_info->line_offset;
    old_text = old_line + tm_info->line_offset;
  } while (--rows);

  prev_cursor_x = cursor_x;
  prev_cursor_y = cursor_y;
}

int bx_svga_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  return 0;
}

int bx_svga_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  return 0;
}

void bx_svga_gui_c::graphics_tile_update(Bit8u *snapshot, unsigned x, unsigned y)
{
  if ((y + y_tilesize) > res_y) {
    gl_putbox(x, y, x_tilesize, (res_y - y), snapshot);
  } else {
    gl_putbox(x, y, x_tilesize, y_tilesize, snapshot);
  }
}

static Bit32u vga_to_bx_key(int key)
{
    ctrll_pressed = 0;
    switch (key) {
	case SCANCODE_ESCAPE: return BX_KEY_ESC;
	case SCANCODE_1: return BX_KEY_1;
	case SCANCODE_2: return BX_KEY_2;
	case SCANCODE_3: return BX_KEY_3;
	case SCANCODE_4: return BX_KEY_4;
	case SCANCODE_5: return BX_KEY_5;
	case SCANCODE_6: return BX_KEY_6;
	case SCANCODE_7: return BX_KEY_7;
	case SCANCODE_8: return BX_KEY_8;
	case SCANCODE_9: return BX_KEY_9;
	case SCANCODE_0: return BX_KEY_0;

	case SCANCODE_MINUS: return BX_KEY_MINUS;
	case SCANCODE_EQUAL: return BX_KEY_EQUALS;
	case SCANCODE_TAB: return BX_KEY_TAB;
	case SCANCODE_BACKSPACE: return BX_KEY_BACKSPACE;

	case SCANCODE_Q: return BX_KEY_Q;
	case SCANCODE_W: return BX_KEY_W;
	case SCANCODE_E: return BX_KEY_E;
	case SCANCODE_R: return BX_KEY_R;
	case SCANCODE_T: return BX_KEY_T;
	case SCANCODE_Y: return BX_KEY_Y;
	case SCANCODE_U: return BX_KEY_U;
	case SCANCODE_I: return BX_KEY_I;
	case SCANCODE_O: return BX_KEY_O;
	case SCANCODE_P: return BX_KEY_P;

	case SCANCODE_BRACKET_LEFT: return BX_KEY_LEFT_BRACKET;
	case SCANCODE_BRACKET_RIGHT: return BX_KEY_RIGHT_BRACKET;

	case SCANCODE_ENTER: return BX_KEY_ENTER;
	case SCANCODE_LEFTCONTROL:
          ctrll_pressed = 1;
          return BX_KEY_CTRL_L;

	case SCANCODE_A: return BX_KEY_A;
	case SCANCODE_S: return BX_KEY_S;
	case SCANCODE_D: return BX_KEY_D;
	case SCANCODE_F: return BX_KEY_F;
	case SCANCODE_G: return BX_KEY_G;
	case SCANCODE_H: return BX_KEY_H;
	case SCANCODE_J: return BX_KEY_J;
	case SCANCODE_K: return BX_KEY_K;
	case SCANCODE_L: return BX_KEY_L;

	case SCANCODE_SEMICOLON: return BX_KEY_SEMICOLON;
	case SCANCODE_APOSTROPHE: return BX_KEY_SINGLE_QUOTE;
	case SCANCODE_GRAVE: return BX_KEY_GRAVE;

	case SCANCODE_LEFTSHIFT: return BX_KEY_SHIFT_L;
	case SCANCODE_BACKSLASH: return BX_KEY_BACKSLASH;

	case SCANCODE_Z: return BX_KEY_Z;
	case SCANCODE_X: return BX_KEY_X;
	case SCANCODE_C: return BX_KEY_C;
	case SCANCODE_V: return BX_KEY_V;
	case SCANCODE_B: return BX_KEY_B;
	case SCANCODE_N: return BX_KEY_N;
	case SCANCODE_M: return BX_KEY_M;

	case SCANCODE_COMMA: return BX_KEY_COMMA;
	case SCANCODE_PERIOD: return BX_KEY_PERIOD;
	case SCANCODE_SLASH: return BX_KEY_SLASH;

	case SCANCODE_RIGHTSHIFT: return BX_KEY_SHIFT_R;
	case SCANCODE_KEYPADMULTIPLY: return BX_KEY_KP_MULTIPLY;

	case SCANCODE_LEFTALT: return BX_KEY_ALT_L;
	case SCANCODE_SPACE: return BX_KEY_SPACE;
	case SCANCODE_CAPSLOCK: return BX_KEY_CAPS_LOCK;

	case SCANCODE_F1: return BX_KEY_F1;
	case SCANCODE_F2: return BX_KEY_F2;
	case SCANCODE_F3: return BX_KEY_F3;
	case SCANCODE_F4: return BX_KEY_F4;
	case SCANCODE_F5: return BX_KEY_F5;
	case SCANCODE_F6: return BX_KEY_F6;
	case SCANCODE_F7: return BX_KEY_F7;
	case SCANCODE_F8: return BX_KEY_F8;
	case SCANCODE_F9: return BX_KEY_F9;
	case SCANCODE_F10: return BX_KEY_F10;

	case SCANCODE_NUMLOCK: return BX_KEY_NUM_LOCK;
	case SCANCODE_SCROLLLOCK: return BX_KEY_SCRL_LOCK;

	case SCANCODE_KEYPAD7: return BX_KEY_KP_HOME;
	case SCANCODE_KEYPAD8: return BX_KEY_KP_UP;
	case SCANCODE_KEYPAD9: return BX_KEY_KP_PAGE_UP;
	case SCANCODE_KEYPADMINUS: return BX_KEY_KP_SUBTRACT;
	case SCANCODE_KEYPAD4: return BX_KEY_KP_LEFT;
	case SCANCODE_KEYPAD5: return BX_KEY_KP_5;
	case SCANCODE_KEYPAD6: return BX_KEY_KP_RIGHT;
	case SCANCODE_KEYPADPLUS: return BX_KEY_KP_ADD;
	case SCANCODE_KEYPAD1: return BX_KEY_KP_END;
	case SCANCODE_KEYPAD2: return BX_KEY_KP_DOWN;
	case SCANCODE_KEYPAD3: return BX_KEY_KP_PAGE_DOWN;
	case SCANCODE_KEYPAD0: return BX_KEY_KP_INSERT;
//	case SCANCODE_KEYPADPERIOD: return BX_KEY_KP_; /* ??? */

//	case SCANCODE_LESS: return BX_KEY_KP_LESS;	/* ??? */

	case SCANCODE_F11: return BX_KEY_F11;
	case SCANCODE_F12: return BX_KEY_F12;

	case SCANCODE_KEYPADENTER: return BX_KEY_KP_ENTER;
	case SCANCODE_RIGHTCONTROL: return BX_KEY_CTRL_R;
	case SCANCODE_KEYPADDIVIDE: return BX_KEY_KP_DIVIDE;
	case SCANCODE_PRINTSCREEN: return BX_KEY_PRINT;
	case SCANCODE_RIGHTALT: return BX_KEY_ALT_R;
	case SCANCODE_BREAK: return BX_KEY_PAUSE;

	case SCANCODE_HOME: return BX_KEY_HOME;
	case SCANCODE_CURSORBLOCKUP: return BX_KEY_UP;
	case SCANCODE_PAGEUP: return BX_KEY_PAGE_UP;
	case SCANCODE_CURSORBLOCKLEFT: return BX_KEY_LEFT;
	case SCANCODE_CURSORBLOCKRIGHT: return BX_KEY_RIGHT;
	case SCANCODE_END: return BX_KEY_END;
	case SCANCODE_CURSORBLOCKDOWN: return BX_KEY_DOWN;
	case SCANCODE_PAGEDOWN: return BX_KEY_PAGE_DOWN;
	case SCANCODE_INSERT: return BX_KEY_INSERT;
	case SCANCODE_REMOVE: return BX_KEY_DELETE;

	case SCANCODE_RIGHTWIN: return BX_KEY_WIN_R;
	case SCANCODE_LEFTWIN: return BX_KEY_WIN_L;

	default: return 0;
    }
}

void keyboard_handler(int scancode, int press)
{
    if (scancode != SCANCODE_F12) {
	int bx_key = vga_to_bx_key(scancode);
	Bit32u key_state;

	if (press) {
	    key_state = BX_KEY_PRESSED;
	} else {
	    key_state = BX_KEY_RELEASED;
	}

	DEV_kbd_gen_scancode(bx_key | key_state);
    } else {
	BX_INFO(("F12 pressed"));
	// show runtime options menu, which uses stdin/stdout
	SIM->configuration_interface (NULL, CI_RUNTIME_CONFIG);
    }
}

void mouse_handler(int button, int dx, int dy, int dz,
		    int drx, int dry, int drz)
{
  int buttons = 0;

  if (button & MOUSE_LEFTBUTTON) {
    buttons |= 0x01;
  }
  if (button & MOUSE_RIGHTBUTTON) {
    buttons |= 0x02;
  }
  if (button & MOUSE_MIDDLEBUTTON) {
    buttons |= 0x04;
  }
  if (ctrll_pressed && ((buttons == 0x04) || (buttons == 0x05))) {
    bx_bool old = SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get();
    SIM->get_param_bool(BXPN_MOUSE_ENABLED)->set(!old);
  } else {
    DEV_mouse_motion((int) (0.25 * dx), (int) -(0.25 * dy), 0, buttons, 0);
  }
}

void bx_svga_gui_c::handle_events(void)
{
  keyboard_update();
  keyboard_clearstate();
  mouse_update();
}

void bx_svga_gui_c::flush(void)
{
  gl_copyscreen(screen);
}

void bx_svga_gui_c::clear_screen(void)
{
  gl_clearscreen(0);
}

bx_bool bx_svga_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue)
{
  // without VGA_CLUT8 extension we have only 6 bits for each r,g,b value
  if (!clut8 && (red > 63 || green > 63 || blue > 63)) {
    red   = red >> 2;
    green = green >> 2;
    blue  = blue >> 2;
  }

  vga_setpalette(index, red, green, blue);

  return 1;
}


void bx_svga_gui_c::dimension_update(
    unsigned x,
    unsigned y,
    unsigned fheight,
    unsigned fwidth,
    unsigned bpp)
{
  int newmode = 0;

  if (bpp > 8) {
    BX_PANIC(("%d bpp graphics mode not supported yet", bpp));
  }
  guest_textmode = (fheight > 0);
  guest_xres = x;
  guest_yres = y;
  guest_bpp = bpp;
  if (guest_textmode) {
    text_cols = x / fwidth;
    text_rows = y / fheight;
    fontheight = fheight;
    if (fwidth != 8) {
      x = x * 8 / fwidth;
    }
    fontwidth = 8;
  }

  if ((x == res_x) && (y == res_y)) return;

  if (x == 640 && y == 480) {
    newmode = G640x480x256;
  } else if (x == 640 && y == 400) {
    newmode = G640x400x256;
  } else if (x == 800 && y == 600) {
    newmode = G800x600x256;
  } else if (x == 1024 && y == 768) {
    newmode = G1024x768x256;
  }

  if (!vga_hasmode(newmode)) {
    newmode = G640x480x256; // trying "default" mode...
  }

  vga_getpalvec(0, 256, save_vga_pal);
  if (vga_setmode(newmode) != 0)
  {
      LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
      BX_PANIC (("Unable to set requested videomode: %ix%i", x, y));
  }

  gl_setcontextvga(newmode);
  gl_getcontext(screen);
  gl_setcontextvgavirtual(newmode);
  vga_setpalvec(0, 256, save_vga_pal);
  save_vga_mode = newmode;

  res_x = x;
  res_y = y;
}


unsigned bx_svga_gui_c::create_bitmap(
    const unsigned char *bmap,
    unsigned xdim,
    unsigned ydim)
{
  return 0;
}


unsigned bx_svga_gui_c::headerbar_bitmap(
    unsigned bmap_id,
    unsigned alignment,
    void (*f)(void))
{
  return 0;
}

void bx_svga_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
}

void bx_svga_gui_c::show_headerbar(void)
{
}


void bx_svga_gui_c::mouse_enabled_changed_specific (bx_bool val)
{
}


void headerbar_click(int x)
{
}

void bx_svga_gui_c::exit(void)
{
  vga_setmode(TEXT);
  keyboard_close();
  mouse_close();
}

void bx_svga_gui_c::set_display_mode (disp_mode_t newmode)
{
  // if no mode change, do nothing.
  if (disp_mode == newmode) return;
  // remember the display mode for next time
  disp_mode = newmode;
  switch (newmode) {
    case DISP_MODE_CONFIG:
      BX_DEBUG (("switch to configuration mode (back to console)"));
      // remember old values and switch to text mode
      save_vga_mode = vga_getcurrentmode();
      vga_getpalvec(0, 256, save_vga_pal);
      keyboard_close();
      vga_setmode(TEXT);
      break;
    case DISP_MODE_SIM:
      BX_DEBUG (("switch to simulation mode (fullscreen)"));
      keyboard_init();
      keyboard_seteventhandler((__keyboard_handler) keyboard_handler);
      vga_setmode(save_vga_mode);
      vga_setpalvec(0, 256, save_vga_pal);
      break;
  }
}

#endif /* if BX_WITH_SVGA */
