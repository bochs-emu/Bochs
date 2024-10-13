/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2024  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#define _MULTI_THREAD

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "param_names.h"
#include "keymap.h"
#include "iodev.h"
#if BX_WITH_SDL

#include <stdlib.h>
#include <SDL.h>
#include <SDL_endian.h>

#include "icon_bochs.h"
#include "sdl.h"
#ifdef WIN32
#include "win32dialog.h"
#endif

#define COMMAND_MODE_KEYSYM SDLK_F7

class bx_sdl_gui_c : public bx_gui_c {
public:
  bx_sdl_gui_c();
  DECLARE_GUI_VIRTUAL_METHODS()
  DECLARE_GUI_NEW_VIRTUAL_METHODS()
  virtual void draw_char(Bit8u ch, Bit8u fc, Bit8u bc, Bit16u xc, Bit16u yc,
                         Bit8u fw, Bit8u fh, Bit8u fx, Bit8u fy,
                         bool gfxcharw9, Bit8u cs, Bit8u ce, bool curs, bool font2);
  virtual void set_display_mode(disp_mode_t newmode);
  virtual void statusbar_setitem_specific(int element, bool active, bool w);
  virtual void get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp);
  virtual void set_mouse_mode_absxy(bool mode);
#if BX_SHOW_IPS
  virtual void show_ips(Bit32u ips_count);
#endif
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_sdl_gui_c *theGui = NULL;
IMPLEMENT_GUI_PLUGIN_CODE(sdl)

#define LOG_THIS theGui->

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const Uint32 status_leds[3] = {0x00ff0000, 0x0040ff00, 0x00ffff00};
const Uint32 status_gray_text = 0x80808000;
#else
const Uint32 status_leds[3] = {0x0000ff00, 0x00ff4000, 0x00ffff00};
const Uint32 status_gray_text = 0x00808080;
#endif

static Bit32u convertStringToSDLKey(const char *string);

#define MAX_SDL_BITMAPS 32
struct bitmaps {
  SDL_Surface *surface;
  SDL_Rect src, dst;
};

SDL_Surface *sdl_screen, *sdl_fullscreen;
SDL_Rect sdl_maxres;
bool sdl_fullscreen_toggle;
int sdl_grab;
unsigned res_x, res_y;
unsigned half_res_x, half_res_y;
int headerbar_height;
static unsigned bx_bitmap_left_xorigin = 0;  // pixels from left
static unsigned bx_bitmap_right_xorigin = 0; // pixels from right
static unsigned disp_bpp=8;
unsigned char menufont[256][8];
Uint32 sdl_palette[256];
Uint32 headerbar_fg, headerbar_bg;
Bit8u old_mousebuttons=0, new_mousebuttons=0;
int old_mousex=0, new_mousex=0;
int old_mousey=0, new_mousey=0;
bool just_warped = 0;
bool sdl_mouse_mode_absxy = 0;
bitmaps *sdl_bitmaps[MAX_SDL_BITMAPS];
int n_sdl_bitmaps = 0;
int statusbar_height = 18;
static unsigned statusitem_pos[12] = {
  0, 170, 220, 270, 320, 370, 420, 470, 520, 570, 620, 670
};
static bool statusitem_active[12];
#if BX_SHOW_IPS
static bool sdl_ips_update = 0;
static char sdl_ips_text[20];
static bool sdl_show_info_msg = 0;
#endif
#if defined(WIN32) && BX_DEBUGGER && BX_DEBUGGER_GUI
bool sdl_enh_dbg_global_ini;
#endif


static void sdl_set_status_text(int element, const char *text, bool active, bool color = 0)
{
  Uint32 *buf, *buf_row;
  Uint32 disp, fgcolor, bgcolor;
  unsigned char *pfont_row, font_row;
  int rowsleft = statusbar_height - 2;
  int colsleft, textlen;
  int x, xleft, xsize;

  statusitem_active[element] = active;
  if (!sdl_screen) return;
  disp = sdl_screen->pitch/4;
  xleft = statusitem_pos[element] + 2;
  xsize = statusitem_pos[element+1] - xleft - 1;
  buf = (Uint32 *)sdl_screen->pixels + (res_y + headerbar_height + 1) * disp + xleft;
  rowsleft = statusbar_height - 2;
  fgcolor = active?headerbar_fg:status_gray_text;
  if ((element > 0) && active) {
    bgcolor = status_leds[color];
  } else {
    bgcolor = headerbar_bg;
  }
  do {
    colsleft = xsize;
    buf_row = buf;
    do
    {
      *buf++ = bgcolor;
    } while(--colsleft);
    buf = buf_row + disp;
  } while(--rowsleft);
  if ((element > 0) && (strlen(text) > 6)) {
    textlen = 6;
  } else {
    textlen = strlen(text);
  }
  buf = (Uint32 *)sdl_screen->pixels + (res_y + headerbar_height + 5) * disp + xleft;
  x = 0;
  do
  {
    pfont_row = &menufont[(unsigned)text[x]][0];
    buf_row = buf;
    rowsleft = 8;
    do
    {
      font_row = *pfont_row++;
      colsleft = 8;
      do
      {
        if((font_row & 0x80) != 0x00)
          *buf++ = fgcolor;
        else
          buf++;
        font_row <<= 1;
      } while(--colsleft);
      buf += (disp - 8);
    } while(--rowsleft);
    buf = buf_row + 8;
    x++;
  } while (--textlen);
  SDL_UpdateRect(sdl_screen, xleft,res_y+headerbar_height+1,xsize,statusbar_height-2);
}


static Bit32u sdl_sym_to_bx_key(SDLKey sym)
{
  switch (sym) {
    case SDLK_BACKSPACE:            return BX_KEY_BACKSPACE;
    case SDLK_TAB:                  return BX_KEY_TAB;
    case SDLK_RETURN:               return BX_KEY_ENTER;
    case SDLK_PAUSE:
      if (bx_gui->get_modifier_keys() & BX_MOD_KEY_CTRL) {
        return BX_KEY_CTRL_BREAK;
      } else {
        return BX_KEY_PAUSE;
      }
      break;
    case SDLK_ESCAPE:               return BX_KEY_ESC;
    case SDLK_SPACE:                return BX_KEY_SPACE;
    case SDLK_QUOTE:                return BX_KEY_SINGLE_QUOTE;
    case SDLK_COMMA:                return BX_KEY_COMMA;
    case SDLK_MINUS:                return BX_KEY_MINUS;
    case SDLK_PERIOD:               return BX_KEY_PERIOD;
    case SDLK_SLASH:                return BX_KEY_SLASH;
    case SDLK_0:                    return BX_KEY_0;
    case SDLK_1:                    return BX_KEY_1;
    case SDLK_2:                    return BX_KEY_2;
    case SDLK_3:                    return BX_KEY_3;
    case SDLK_4:                    return BX_KEY_4;
    case SDLK_5:                    return BX_KEY_5;
    case SDLK_6:                    return BX_KEY_6;
    case SDLK_7:                    return BX_KEY_7;
    case SDLK_8:                    return BX_KEY_8;
    case SDLK_9:                    return BX_KEY_9;
    case SDLK_SEMICOLON:            return BX_KEY_SEMICOLON;
    case SDLK_EQUALS:               return BX_KEY_EQUALS;
/*
 Skip uppercase letters
*/
    case SDLK_LEFTBRACKET:          return BX_KEY_LEFT_BRACKET;
    case SDLK_BACKSLASH:            return BX_KEY_BACKSLASH;
    case SDLK_RIGHTBRACKET:         return BX_KEY_RIGHT_BRACKET;
    case SDLK_BACKQUOTE:            return BX_KEY_GRAVE;
    case SDLK_a:                    return BX_KEY_A;
    case SDLK_b:                    return BX_KEY_B;
    case SDLK_c:                    return BX_KEY_C;
    case SDLK_d:                    return BX_KEY_D;
    case SDLK_e:                    return BX_KEY_E;
    case SDLK_f:                    return BX_KEY_F;
    case SDLK_g:                    return BX_KEY_G;
    case SDLK_h:                    return BX_KEY_H;
    case SDLK_i:                    return BX_KEY_I;
    case SDLK_j:                    return BX_KEY_J;
    case SDLK_k:                    return BX_KEY_K;
    case SDLK_l:                    return BX_KEY_L;
    case SDLK_m:                    return BX_KEY_M;
    case SDLK_n:                    return BX_KEY_N;
    case SDLK_o:                    return BX_KEY_O;
    case SDLK_p:                    return BX_KEY_P;
    case SDLK_q:                    return BX_KEY_Q;
    case SDLK_r:                    return BX_KEY_R;
    case SDLK_s:                    return BX_KEY_S;
    case SDLK_t:                    return BX_KEY_T;
    case SDLK_u:                    return BX_KEY_U;
    case SDLK_v:                    return BX_KEY_V;
    case SDLK_w:                    return BX_KEY_W;
    case SDLK_x:                    return BX_KEY_X;
    case SDLK_y:                    return BX_KEY_Y;
    case SDLK_z:                    return BX_KEY_Z;
    case SDLK_DELETE:               return BX_KEY_DELETE;
/* End of ASCII mapped keysyms */

/* Numeric keypad */
    case SDLK_KP0:                  return BX_KEY_KP_INSERT;
    case SDLK_KP1:                  return BX_KEY_KP_END;
    case SDLK_KP2:                  return BX_KEY_KP_DOWN;
    case SDLK_KP3:                  return BX_KEY_KP_PAGE_DOWN;
    case SDLK_KP4:                  return BX_KEY_KP_LEFT;
    case SDLK_KP5:                  return BX_KEY_KP_5;
    case SDLK_KP6:                  return BX_KEY_KP_RIGHT;
    case SDLK_KP7:                  return BX_KEY_KP_HOME;
    case SDLK_KP8:                  return BX_KEY_KP_UP;
    case SDLK_KP9:                  return BX_KEY_KP_PAGE_UP;
    case SDLK_KP_PERIOD:            return BX_KEY_KP_DELETE;
    case SDLK_KP_DIVIDE:            return BX_KEY_KP_DIVIDE;
    case SDLK_KP_MULTIPLY:          return BX_KEY_KP_MULTIPLY;
    case SDLK_KP_MINUS:             return BX_KEY_KP_SUBTRACT;
    case SDLK_KP_PLUS:              return BX_KEY_KP_ADD;
    case SDLK_KP_ENTER:             return BX_KEY_KP_ENTER;

/* Arrows + Home/End pad */
    case SDLK_UP:                   return BX_KEY_UP;
    case SDLK_DOWN:                 return BX_KEY_DOWN;
    case SDLK_RIGHT:                return BX_KEY_RIGHT;
    case SDLK_LEFT:                 return BX_KEY_LEFT;
    case SDLK_INSERT:               return BX_KEY_INSERT;
    case SDLK_HOME:                 return BX_KEY_HOME;
    case SDLK_END:                  return BX_KEY_END;
    case SDLK_PAGEUP:               return BX_KEY_PAGE_UP;
    case SDLK_PAGEDOWN:             return BX_KEY_PAGE_DOWN;

/* Function keys */
    case SDLK_F1:                   return BX_KEY_F1;
    case SDLK_F2:                   return BX_KEY_F2;
    case SDLK_F3:                   return BX_KEY_F3;
    case SDLK_F4:                   return BX_KEY_F4;
    case SDLK_F5:                   return BX_KEY_F5;
    case SDLK_F6:                   return BX_KEY_F6;
    case SDLK_F7:                   return BX_KEY_F7;
    case SDLK_F8:                   return BX_KEY_F8;
    case SDLK_F9:                   return BX_KEY_F9;
    case SDLK_F10:                  return BX_KEY_F10;
    case SDLK_F11:                  return BX_KEY_F11;
    case SDLK_F12:                  return BX_KEY_F12;

/* Key state modifier keys */
    case SDLK_NUMLOCK:              return BX_KEY_NUM_LOCK;
    case SDLK_CAPSLOCK:             return BX_KEY_CAPS_LOCK;
    case SDLK_SCROLLOCK:            return BX_KEY_SCRL_LOCK;
    case SDLK_RSHIFT:               return BX_KEY_SHIFT_R;
    case SDLK_LSHIFT:               return BX_KEY_SHIFT_L;
    case SDLK_RCTRL:                return BX_KEY_CTRL_R;
    case SDLK_LCTRL:                return BX_KEY_CTRL_L;
    case SDLK_RALT:                 return BX_KEY_ALT_R;
    case SDLK_LALT:                 return BX_KEY_ALT_L;
    case SDLK_RMETA:                return BX_KEY_ALT_R;
    case SDLK_LMETA:                return BX_KEY_WIN_L;
    case SDLK_LSUPER:               return BX_KEY_WIN_L;
    case SDLK_RSUPER:               return BX_KEY_WIN_R;

/* Miscellaneous function keys */
    case SDLK_PRINT:                return BX_KEY_PRINT;
    case SDLK_BREAK:                return BX_KEY_CTRL_BREAK;
    case SDLK_MENU:                 return BX_KEY_MENU;

    default:
      BX_ERROR(("sdl keysym %d not mapped", (int)sym));
      return BX_KEY_UNHANDLED;
  }
}


void switch_to_windowed(void)
{
  SDL_FreeSurface(sdl_fullscreen);
  sdl_fullscreen = NULL;
  sdl_screen = SDL_SetVideoMode(res_x,res_y+headerbar_height+statusbar_height,32, SDL_SWSURFACE);
  bx_gui->show_headerbar();
  DEV_vga_refresh(1);
  SDL_ShowCursor(1);
  if (sdl_grab == 1) {
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    sdl_grab = 0;
    bx_gui->toggle_mouse_enable();
  }
}


void switch_to_fullscreen(void)
{
  SDL_FreeSurface(sdl_screen);
  sdl_screen = NULL;
#ifdef __ANDROID__
  sdl_fullscreen = SDL_SetVideoMode(res_x,res_y,32, SDL_SWSURFACE|SDL_FULLSCREEN);
#else
  sdl_fullscreen = SDL_SetVideoMode(res_x,res_y,32, SDL_HWSURFACE|SDL_FULLSCREEN);
#endif
  DEV_vga_refresh(1);
  SDL_ShowCursor(0);
#ifndef __ANDROID__
  if (sdl_grab == 0) {
    SDL_WM_GrabInput(SDL_GRAB_ON);
    sdl_grab = 1;
    bx_gui->toggle_mouse_enable();
  }
#endif
}


#if BX_SHOW_IPS
#if defined(__MINGW32__) || defined(_MSC_VER)
Uint32 SDLCALL sdlTimer(Uint32 interval)
{
  bx_show_ips_handler();
  return interval;
}
#endif
#endif


#ifdef __MORPHOS__
void bx_sdl_morphos_exit(void)
{
  SDL_Quit();
  if (PowerSDLBase) CloseLibrary(PowerSDLBase);
}
#endif


#if defined(WIN32) && BX_DEBUGGER && BX_DEBUGGER_GUI
DWORD WINAPI DebugGuiThread(LPVOID)
{
  MSG msg;

  bx_gui->init_debug_dialog(sdl_enh_dbg_global_ini);
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}
#endif


bx_sdl_gui_c::bx_sdl_gui_c()
{
  Uint32 flags;
  SDL_Rect **modes;

  put("SDL");

#ifdef __MORPHOS__
  if (!(PowerSDLBase=OpenLibrary("powersdl.library",0)))
  {
    BX_FATAL(("Unable to open SDL libraries"));
    return;
  }
#endif

  flags = SDL_INIT_VIDEO;
#if BX_SHOW_IPS
#if  defined(__MINGW32__) || defined(_MSC_VER)
  flags |= SDL_INIT_TIMER;
#endif
#endif
  if (SDL_Init(flags) < 0) {
    BX_FATAL(("Unable to initialize SDL libraries"));
    return;
  }
#ifdef __MORPHOS__
  atexit(bx_sdl_morphos_exit);
#else
  atexit(SDL_Quit);
#endif

#ifdef __ANDROID__
  modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_SWSURFACE);
#else
  modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
#endif
  if (modes == NULL) {
    panic("No video modes available");
    return;
  }
  sdl_maxres.w = modes[0]->w;
  sdl_maxres.h = modes[0]->h;
  info("maximum host resolution: x=%d y=%d\n", sdl_maxres.w, sdl_maxres.h);
}


// SDL implementation of the bx_gui_c methods (see nogui.cc for details)

void bx_sdl_gui_c::specific_init(int argc, char **argv, unsigned headerbar_y)
{
  int i, j;
  bool gui_ci = 0;

#ifdef WIN32
  gui_ci = !strcmp(SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE)->get_selected(), "win32config");
#endif

  UNUSED(bochs_icon_bits);

  headerbar_height = headerbar_y;

  for(i=0;i<256;i++) {
    for(j=0;j<16;j++) {
      vga_charmap[0][i*32+j] = sdl_font8x16[i][j];
      vga_charmap[1][i*32+j] = sdl_font8x16[i][j];
    }
  }
  for(i=0;i<256;i++)
    for(j=0;j<8;j++)
      menufont[i][j] = sdl_font8x8[i][j];

  sdl_screen = NULL;
  sdl_fullscreen_toggle = 0;
  dimension_update(640, 480);

  SDL_EnableKeyRepeat(250, 50);
  SDL_EnableUNICODE(SDL_TRUE);
  SDL_WM_SetCaption(BOCHS_WINDOW_NAME, "Bochs");
  SDL_WarpMouse(half_res_x, half_res_y);

  // load keymap for sdl
  if (SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
    bx_keymap.loadKeymap(convertStringToSDLKey);
  }

  if (!gui_ci) {
    console.present = 1;
  }

  // parse sdl specific options
  Bit8u flags = BX_GUI_OPT_NOKEYREPEAT | BX_GUI_OPT_HIDE_IPS  | BX_GUI_OPT_CMDMODE
                | BX_GUI_OPT_NO_GUI_CONSOLE;
  if (argc > 1) {
    for (i = 1; i < argc; i++) {
      if (!parse_common_gui_options(argv[i], flags)) {
        if (!strcmp(argv[i], "fullscreen")) {
          sdl_fullscreen_toggle = 1;
          switch_to_fullscreen();
        } else {
          BX_PANIC(("Unknown sdl option '%s'", argv[i]));
        }
      }
    }
  }

  if (gui_opts.nokeyrepeat) {
    SDL_EnableKeyRepeat(0, 0);
  }

#if BX_DEBUGGER && BX_DEBUGGER_GUI
  if (gui_opts.enh_dbg_enabled) {
    SIM->set_debug_gui(1);
#ifdef WIN32
    if (gui_ci) {
      sdl_enh_dbg_global_ini = gui_opts.enh_dbg_global_ini;
      // on Windows the debugger gui must run in a separate thread
      DWORD threadID;
      CreateThread(NULL, 0, DebugGuiThread, NULL, 0, &threadID);
    } else {
      BX_PANIC(("Config interface 'win32config' is required for gui debugger"));
    }
#else
    init_debug_dialog(gui_opts.enh_dbg_global_ini);
#endif
  }
#endif

  new_gfx_api = 1;
  new_text_api = 1;
#if defined(WIN32) && BX_SHOW_IPS
  SDL_SetTimer(1000, sdlTimer);
#endif
  if (gui_ci) {
    dialog_caps = BX_GUI_DLG_ALL;
  }
}

void bx_sdl_gui_c::draw_char(Bit8u ch, Bit8u fc, Bit8u bc, Bit16u xc, Bit16u yc,
                             Bit8u fw, Bit8u fh, Bit8u fx, Bit8u fy,
                             bool gfxcharw9, Bit8u cs, Bit8u ce, bool curs, bool font2)
{
  Uint32 *buf, pitch, fgcolor, bgcolor;
  Bit16u font_row, mask;
  Bit8u *font_ptr, fontpixels;
  bool dwidth;

  if (sdl_screen) {
    pitch = sdl_screen->pitch/4;
    buf = (Uint32 *)sdl_screen->pixels + (headerbar_height + yc) * pitch + xc;
  } else {
    pitch = sdl_fullscreen->pitch/4;
    buf = (Uint32 *)sdl_fullscreen->pixels + yc * pitch + xc;
  }
  fgcolor = sdl_palette[fc];
  bgcolor = sdl_palette[bc];
  dwidth = (guest_fwidth > 9);
  if (font2) {
    font_ptr = &vga_charmap[1][(ch << 5) + fy];
  } else {
    font_ptr = &vga_charmap[0][(ch << 5) + fy];
  }
  do {
    font_row = *font_ptr++;
    if (gfxcharw9) {
      font_row = (font_row << 1) | (font_row & 0x01);
    } else {
      font_row <<= 1;
    }
    if (fx > 0) {
      font_row <<= fx;
    }
    fontpixels = fw;
    if (curs && (fy >= cs) && (fy <= ce))
      mask = 0x100;
    else
      mask = 0x00;
    do {
      if ((font_row & 0x100) == mask)
        *buf = bgcolor;
      else
        *buf = fgcolor;
      buf++;
      if (!dwidth || (fontpixels & 1)) font_row <<= 1;
    } while (--fontpixels);
    buf += (pitch - fw);
    fy++;
  } while (--fh);
}

void bx_sdl_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
    unsigned long cursor_x,
    unsigned long cursor_y,
    bx_vga_tminfo_t *tm_info)
{
  // present for compatibility
}


void bx_sdl_gui_c::graphics_tile_update(Bit8u *snapshot, unsigned x, unsigned y)
{
  Uint32 *buf, disp;
  Uint32 *buf_row;
  int i, j;

  if (sdl_screen) {
    disp = sdl_screen->pitch/4;
    buf = (Uint32 *)sdl_screen->pixels + (headerbar_height + y) * disp + x;
  } else {
    disp = sdl_fullscreen->pitch/4;
    buf = (Uint32 *)sdl_fullscreen->pixels + y * disp + x + sdl_fullscreen->offset/4;
  }

  i = y_tilesize;
  if(i + y > res_y) i = res_y - y;

  // FIXME
  if (i<=0) return;

  switch (disp_bpp) {
    case 8: /* 8 bpp */
      do {
        buf_row = buf;
        j = x_tilesize;
        do {
          *buf++ = sdl_palette[*snapshot++];
        } while(--j);
        buf = buf_row + disp;
      } while(--i);
      break;
    default:
      BX_PANIC(("%u bpp modes handled by new graphics API", disp_bpp));
      return;
  }
}


void bx_sdl_gui_c::handle_events(void)
{
  SDL_Event sdl_event;
  Bit32u key_event;
  Bit8u mouse_state, kmodchange = 0;
  int dx, dy, wheel_status;
  bool mouse_toggle = 0;

  while (SDL_PollEvent(&sdl_event)) {
    wheel_status = 0;
    switch (sdl_event.type) {
      case SDL_VIDEOEXPOSE:
        if (sdl_fullscreen_toggle == 0)
          SDL_UpdateRect(sdl_screen, 0,0, res_x, res_y+headerbar_height+statusbar_height);
        else
          SDL_UpdateRect(sdl_fullscreen, 0, headerbar_height, res_x, res_y);
        break;

      case SDL_MOUSEMOTION:
        if (!sdl_grab || console_running()) {
          break;
        }
        if (just_warped
            && sdl_event.motion.x == half_res_x
            && sdl_event.motion.y == half_res_y) {
          // This event was generated as a side effect of the WarpMouse,
          // and it must be ignored.
          just_warped = 0;
          break;
        }
        new_mousebuttons = ((sdl_event.motion.state & 0x01)|((sdl_event.motion.state>>1)&0x02)
                            |((sdl_event.motion.state<<1)&0x04));
        if (sdl_mouse_mode_absxy) {
          dx = sdl_event.motion.x * 0x7fff / res_x;
          if (sdl_fullscreen_toggle) {
            dy = sdl_event.motion.y * 0x7fff / res_y;
            DEV_mouse_motion(dx, dy, 0, new_mousebuttons, 1);
          } else if ((sdl_event.motion.y >= headerbar_height) && (sdl_event.motion.y < (res_y + headerbar_height))) {
            dy = (sdl_event.motion.y - headerbar_height) * 0x7fff / res_y;
            DEV_mouse_motion(dx, dy, 0, new_mousebuttons, 1);
          }
        } else {
          DEV_mouse_motion(sdl_event.motion.xrel, -sdl_event.motion.yrel, 0,
                           new_mousebuttons, 0);
        }
        old_mousebuttons = new_mousebuttons;
        old_mousex = (int)(sdl_event.motion.x);
        old_mousey = (int)(sdl_event.motion.y);
        if (!sdl_mouse_mode_absxy) {
          SDL_WarpMouse(half_res_x, half_res_y);
          just_warped = 1;
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        // mouse capture toggle-check
        if ((sdl_event.button.button == SDL_BUTTON_MIDDLE)
            && (sdl_fullscreen_toggle == 0)) {
          if (mouse_toggle_check(BX_MT_MBUTTON, 1)) {
            if (sdl_grab == 0) {
              SDL_ShowCursor(0);
              SDL_WM_GrabInput(SDL_GRAB_ON);
            } else {
              SDL_ShowCursor(1);
              SDL_WM_GrabInput(SDL_GRAB_OFF);
            }
            sdl_grab = ~sdl_grab;
            toggle_mouse_enable();
          }
          break;
        } else if (!sdl_fullscreen_toggle && (sdl_event.button.y < headerbar_height)) {
          headerbar_click(sdl_event.button.x);
          break;
        }
#ifdef SDL_BUTTON_WHEELUP
        // get the wheel status
        if (sdl_event.button.button == SDL_BUTTON_WHEELUP) {
          wheel_status = 1;
        }
        if (sdl_event.button.button == SDL_BUTTON_WHEELDOWN) {
          wheel_status = -1;
        }
#endif
      case SDL_MOUSEBUTTONUP:
        if ((sdl_event.button.button == SDL_BUTTON_MIDDLE)
            && (sdl_fullscreen_toggle == 0)) {
          mouse_toggle_check(BX_MT_MBUTTON, 0);
        }
        if (console_running()) {
          break;
        }
        // figure out mouse state
        new_mousex = (int)(sdl_event.button.x);
        new_mousey = (int)(sdl_event.button.y);
        // SDL_GetMouseState() returns the state of all buttons
        mouse_state = SDL_GetMouseState(NULL, NULL);
        new_mousebuttons =
          (mouse_state & 0x01)    |
          ((mouse_state>>1)&0x02) |
          ((mouse_state<<1)&0x04);
        // send motion information
        if (sdl_mouse_mode_absxy) {
          dx = new_mousex * 0x7fff / res_x;
          if (sdl_fullscreen_toggle) {
            dy = new_mousey * 0x7fff / res_y;
            DEV_mouse_motion(dx, dy, wheel_status, new_mousebuttons, 1);
          } else if ((new_mousey >= headerbar_height) && (new_mousey < (int)(res_y + headerbar_height))) {
            dy = (new_mousey - headerbar_height) * 0x7fff / res_y;
            DEV_mouse_motion(dx, dy, wheel_status, new_mousebuttons, 1);
          }
        } else {
          DEV_mouse_motion(new_mousex - old_mousex, -(new_mousey - old_mousey),
                           wheel_status, new_mousebuttons, 0);
        }
        // mark current state to diff with next packet
        old_mousebuttons = new_mousebuttons;
        old_mousex = new_mousex;
        old_mousey = new_mousey;
        break;

      case SDL_KEYDOWN:
        // check modifier keys
        if ((sdl_event.key.keysym.sym == SDLK_LSHIFT) ||
            (sdl_event.key.keysym.sym == SDLK_RSHIFT)) {
          kmodchange = set_modifier_keys(BX_MOD_KEY_SHIFT, 1);
        } else if ((sdl_event.key.keysym.sym == SDLK_LCTRL) ||
                   (sdl_event.key.keysym.sym == SDLK_RCTRL)) {
          kmodchange = set_modifier_keys(BX_MOD_KEY_CTRL, 1);
        } else if (sdl_event.key.keysym.sym == SDLK_LALT) {
          kmodchange = set_modifier_keys(BX_MOD_KEY_ALT, 1);
        } else if (sdl_event.key.keysym.sym == SDLK_CAPSLOCK) {
          kmodchange = set_modifier_keys(BX_MOD_KEY_CAPS, 1);
        }
        // handle gui console mode
        if (console_running()) {
          SDLKey keysym = sdl_event.key.keysym.sym;
          Bit8u ascii = (Bit8u)sdl_event.key.keysym.unicode;
          if (((keysym >= SDLK_SPACE) && (keysym < SDLK_DELETE)) ||
              (keysym == SDLK_RETURN) || (keysym == SDLK_BACKSPACE)) {
            if (ascii < 0x80) {
              console_key_enq(ascii);
            }
          }
          break;
        }
        // mouse capture toggle-check
        if (sdl_fullscreen_toggle == 0) {
          if ((kmodchange & BX_MOD_KEY_CTRL) > 0) {
            mouse_toggle = mouse_toggle_check(BX_MT_KEY_CTRL, 1);
          } else if ((kmodchange & BX_MOD_KEY_ALT) > 0) {
            mouse_toggle = mouse_toggle_check(BX_MT_KEY_ALT, 1);
          } else if (sdl_event.key.keysym.sym == SDLK_F10) {
            mouse_toggle = mouse_toggle_check(BX_MT_KEY_F10, 1);
          } else if (sdl_event.key.keysym.sym == SDLK_F12) {
            mouse_toggle = mouse_toggle_check(BX_MT_KEY_F12, 1);
          }
          if (mouse_toggle) {
            toggle_mouse_enable();
          }
        }
        // handle command mode
        if (bx_gui->command_mode_active()) {
          if (bx_gui->get_modifier_keys() == 0) {
            if (sdl_event.key.keysym.sym == SDLK_a) {
              bx_gui->floppyA_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_b) {
              bx_gui->floppyB_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_c) {
              bx_gui->copy_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_f) {
              sdl_fullscreen_toggle = !sdl_fullscreen_toggle;
              if (sdl_fullscreen_toggle == 0) {
                switch_to_windowed();
              } else {
                switch_to_fullscreen();
              }
              bx_gui->set_fullscreen_mode(sdl_fullscreen_toggle);
            } else if (sdl_event.key.keysym.sym == SDLK_m) {
              bx_gui->marklog_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_p) {
              bx_gui->paste_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_r) {
              bx_gui->reset_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_s) {
              bx_gui->snapshot_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_u) {
              bx_gui->userbutton_handler();
            }
          } else if (bx_gui->get_modifier_keys() == BX_MOD_KEY_SHIFT) {
            if (sdl_event.key.keysym.sym == SDLK_c) {
              bx_gui->config_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_p) {
              bx_gui->power_handler();
            } else if (sdl_event.key.keysym.sym == SDLK_s) {
              bx_gui->save_restore_handler();
            }
          }
          if (kmodchange == 0) {
            bx_gui->set_command_mode(0);
#if BX_SHOW_IPS
            sdl_show_info_msg = 0;
#endif
          }
          if (sdl_event.key.keysym.sym != COMMAND_MODE_KEYSYM) {
            return;
          }
        } else {
          if (bx_gui->has_command_mode() && (bx_gui->get_modifier_keys() == 0) &&
              (sdl_event.key.keysym.sym == COMMAND_MODE_KEYSYM)) {
            bx_gui->set_command_mode(1);
            sdl_set_status_text(0, "Command mode", 1);
#if BX_SHOW_IPS
            sdl_show_info_msg = 1;
#endif
            return;
          }
        }

        // Window/Fullscreen toggle-check
        if ((sdl_event.key.keysym.sym == SDLK_RETURN) &&
            (bx_gui->get_modifier_keys() == BX_MOD_KEY_ALT)) {
          sdl_fullscreen_toggle = !sdl_fullscreen_toggle;
          if (sdl_fullscreen_toggle == 0) {
            switch_to_windowed();
          } else {
            switch_to_fullscreen();
          }
          bx_gui->show_headerbar();
          bx_gui->flush();
          break;
        }

        // convert sym->bochs code
        if (sdl_event.key.keysym.sym > SDLK_LAST) break;
        if (!SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
          key_event = sdl_sym_to_bx_key(sdl_event.key.keysym.sym);
          BX_DEBUG(("keypress scancode=%d, sym=%d, bx_key = %d", sdl_event.key.keysym.scancode, sdl_event.key.keysym.sym, key_event));
        } else {
          /* use mapping */
          BXKeyEntry *entry = bx_keymap.findHostKey(sdl_event.key.keysym.sym);
          if (!entry) {
            BX_ERROR(("host key %d (0x%x) not mapped!",
                      (unsigned) sdl_event.key.keysym.sym,
                      (unsigned) sdl_event.key.keysym.sym));
            break;
          }
          key_event = entry->baseKey;
        }
        if (key_event == BX_KEY_UNHANDLED) break;
        DEV_kbd_gen_scancode( key_event);
        if ((key_event == BX_KEY_NUM_LOCK) || (key_event == BX_KEY_CAPS_LOCK)) {
          DEV_kbd_gen_scancode(key_event | BX_KEY_RELEASED);
        }
        break;

      case SDL_KEYUP:
        // check modifier keys
        if ((sdl_event.key.keysym.sym == SDLK_LSHIFT) ||
            (sdl_event.key.keysym.sym == SDLK_RSHIFT)) {
          kmodchange = set_modifier_keys(BX_MOD_KEY_SHIFT, 0);
        } else if ((sdl_event.key.keysym.sym == SDLK_LCTRL) ||
                   (sdl_event.key.keysym.sym == SDLK_RCTRL)) {
          kmodchange = set_modifier_keys(BX_MOD_KEY_CTRL, 0);
        } else if (sdl_event.key.keysym.sym == SDLK_LALT) {
          kmodchange = set_modifier_keys(BX_MOD_KEY_ALT, 0);
        }
        // mouse capture toggle-check
        if ((kmodchange & BX_MOD_KEY_CTRL) > 0) {
          mouse_toggle_check(BX_MT_KEY_CTRL, 0);
        } else if ((kmodchange & BX_MOD_KEY_ALT) > 0) {
          mouse_toggle_check(BX_MT_KEY_ALT, 0);
        } else if (sdl_event.key.keysym.sym == SDLK_F10) {
          mouse_toggle_check(BX_MT_KEY_F10, 0);
        } else if (sdl_event.key.keysym.sym == SDLK_F12) {
          mouse_toggle_check(BX_MT_KEY_F12, 0);
        }

        // filter out release of unsupported keys
        if (sdl_event.key.keysym.sym < SDLK_LAST) {
          // convert sym->bochs code
          if (!SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
            key_event = sdl_sym_to_bx_key(sdl_event.key.keysym.sym);
          } else {
            /* use mapping */
            BXKeyEntry *entry = bx_keymap.findHostKey(sdl_event.key.keysym.sym);
            if (!entry) {
              BX_ERROR(("host key %d (0x%x) not mapped!",
                        (unsigned) sdl_event.key.keysym.sym,
                        (unsigned) sdl_event.key.keysym.sym));
              break;
            }
            key_event = entry->baseKey;
          }
          if (key_event == BX_KEY_UNHANDLED) break;
          if ((key_event == BX_KEY_NUM_LOCK) || (key_event == BX_KEY_CAPS_LOCK)) {
            DEV_kbd_gen_scancode(key_event);
          }
          DEV_kbd_gen_scancode(key_event | BX_KEY_RELEASED);
        }
        break;

      case SDL_QUIT:
        BX_FATAL(("User requested shutdown."));
    }
  }
#if BX_SHOW_IPS
  if (sdl_ips_update && !sdl_show_info_msg) {
    sdl_ips_update = 0;
    sdl_set_status_text(0, sdl_ips_text, 1);
  }
#endif
}


void bx_sdl_gui_c::flush(void)
{
  if(sdl_screen)
    SDL_UpdateRect(sdl_screen,0,0,res_x,res_y+headerbar_height);
  else
    SDL_UpdateRect(sdl_fullscreen,0,0,res_x,res_y);
}


void bx_sdl_gui_c::clear_screen(void)
{
  int i = res_y, j;
  Uint32 color;
  Uint32 *buf, *buf_row;
  Uint32 disp;

  if (sdl_screen) {
    color = SDL_MapRGB(sdl_screen->format, 0, 0, 0);
    disp = sdl_screen->pitch/4;
    buf = (Uint32 *)sdl_screen->pixels + headerbar_height * disp;
  } else if (sdl_fullscreen) {
    color = SDL_MapRGB(sdl_fullscreen->format, 0, 0, 0);
    disp = sdl_fullscreen->pitch/4;
    buf = (Uint32 *)sdl_fullscreen->pixels + sdl_fullscreen->offset/4;
  }
  else return;

  do {
    buf_row = buf;
    j = res_x;
    while (j--) *buf++ = color;
    buf = buf_row + disp;
  } while (--i);

  if (sdl_screen)
    SDL_UpdateRect(sdl_screen,0,0,res_x,res_y+headerbar_height);
  else
    SDL_UpdateRect(sdl_fullscreen,0,0,res_x,res_y);
}


bool bx_sdl_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue)
{
  if (sdl_screen)
    sdl_palette[index] = SDL_MapRGB(sdl_screen->format, red, green, blue);
  else if (sdl_fullscreen)
    sdl_palette[index] = SDL_MapRGB(sdl_fullscreen->format, red, green, blue);

  return 1;
}


void bx_sdl_gui_c::dimension_update(unsigned x, unsigned y,
    unsigned fheight, unsigned fwidth, unsigned bpp)
{
  if (bpp == 8 || bpp == 15 || bpp == 16 || bpp == 24 || bpp == 32) {
    disp_bpp = guest_bpp = bpp;
  } else {
    BX_PANIC(("%d bpp graphics mode not supported", bpp));
  }
  guest_textmode = (fheight > 0);
  guest_fwidth = fwidth;
  guest_fheight = fheight;
  guest_xres = x;
  guest_yres = y;

  if ((x == res_x) && (y == res_y)) return;
#ifndef __ANDROID__
  // This is not needed on Android
  if (((int)x > sdl_maxres.w) || ((int)y > sdl_maxres.h)) {
    BX_PANIC(("dimension_update(): resolution of out of display bounds"));
    return;
  }
#endif

  if (sdl_screen) {
    SDL_FreeSurface(sdl_screen);
    sdl_screen = NULL;
  }
  if(sdl_fullscreen) {
    SDL_FreeSurface(sdl_fullscreen);
    sdl_fullscreen = NULL;
  }

  if (sdl_fullscreen_toggle == 0) {
    sdl_screen = SDL_SetVideoMode(x, y+headerbar_height+statusbar_height, 32, SDL_SWSURFACE);
    if(!sdl_screen)
    {
      BX_FATAL(("Unable to set requested videomode: %ix%i: %s",x,y,SDL_GetError()));
    }
    headerbar_fg = SDL_MapRGB(
        sdl_screen->format,
        BX_HEADERBAR_FG_RED,
        BX_HEADERBAR_FG_GREEN,
        BX_HEADERBAR_FG_BLUE);
    headerbar_bg = SDL_MapRGB(
        sdl_screen->format,
        BX_HEADERBAR_BG_RED,
        BX_HEADERBAR_BG_GREEN,
        BX_HEADERBAR_BG_BLUE);
  } else {
#ifdef __ANDROID__
    sdl_fullscreen = SDL_SetVideoMode(x, y, 32, SDL_SWSURFACE|SDL_FULLSCREEN);
#else
    sdl_fullscreen = SDL_SetVideoMode(x, y, 32, SDL_HWSURFACE|SDL_FULLSCREEN);
#endif
    if(!sdl_fullscreen) {
      BX_FATAL(("Unable to set requested videomode: %ix%i: %s",x,y,SDL_GetError()));
    }
  }
  res_x = x;
  res_y = y;
  half_res_x = x/2;
  half_res_y = y/2;
  bx_gui->show_headerbar();
  host_xres = x;
  host_yres = y;
  host_bpp = 32;
}


unsigned bx_sdl_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  Uint32 *buf, *buf_row;
  Uint32 disp;
  unsigned char pixels;

  if (n_sdl_bitmaps >= MAX_SDL_BITMAPS) {
    BX_PANIC(("too many SDL bitmaps. To fix, increase MAX_SDL_BITMAPS"));
    return 0;
  }

  bitmaps *tmp = new bitmaps;

  tmp->surface = SDL_CreateRGBSurface(
      SDL_SWSURFACE,
      xdim,
      ydim,
      32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      0xff000000,
      0x00ff0000,
      0x0000ff00,
      0x00000000
#else
      0x000000ff,
      0x0000ff00,
      0x00ff0000,
      0x00000000
#endif
      );

  if (!tmp->surface) {
    delete tmp;
    bx_gui->exit();
    BX_FATAL(("Unable to create requested bitmap"));
  }

  tmp->src.w = xdim;
  tmp->src.h = ydim;
  tmp->src.x = 0;
  tmp->src.y = 0;
  tmp->dst.x = -1;
  tmp->dst.y = 0;
  tmp->dst.w = xdim;
  tmp->dst.h = ydim;
  buf = (Uint32 *)tmp->surface->pixels;
  disp = tmp->surface->pitch/4;

  do {
    buf_row = buf;
    xdim = tmp->src.w / 8;
    do {
      pixels = *bmap++;
      for (unsigned i=0; i<8; i++) {
        if ((pixels & 0x01) == 0)
          *buf++ = headerbar_bg;
        else
          *buf++ = headerbar_fg;
        pixels = pixels >> 1;
      }
    } while (--xdim);
    buf = buf_row + disp;
  } while (--ydim);

  SDL_UpdateRect(tmp->surface, 0, 0, tmp->src.w, tmp->src.h);
  sdl_bitmaps[n_sdl_bitmaps] = tmp;

  return n_sdl_bitmaps++;
}


unsigned bx_sdl_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment,
    void (*f)(void))
{
  unsigned hb_index;

  if (bmap_id >= (unsigned)n_sdl_bitmaps) return 0;

  if ((bx_headerbar_entries+1) > BX_MAX_HEADERBAR_ENTRIES)
    BX_PANIC(("too many headerbar entries, increase BX_MAX_HEADERBAR_ENTRIES"));

  bx_headerbar_entries++;
  hb_index = bx_headerbar_entries - 1;

  bx_headerbar_entry[hb_index].bmap_id = bmap_id;
  bx_headerbar_entry[hb_index].xdim = sdl_bitmaps[bmap_id]->src.w;
  bx_headerbar_entry[hb_index].ydim = sdl_bitmaps[bmap_id]->src.h;
  bx_headerbar_entry[hb_index].alignment = alignment;
  bx_headerbar_entry[hb_index].f = f;

  if (alignment == BX_GRAVITY_LEFT) {
    sdl_bitmaps[bmap_id]->dst.x = bx_bitmap_left_xorigin;
    bx_bitmap_left_xorigin += sdl_bitmaps[bmap_id]->src.w;
  } else {
    bx_bitmap_right_xorigin += sdl_bitmaps[bmap_id]->src.w;
    sdl_bitmaps[bmap_id]->dst.x = bx_bitmap_right_xorigin;
  }
  bx_headerbar_entry[hb_index].xorigin = sdl_bitmaps[bmap_id]->dst.x;

  return hb_index;
}


void bx_sdl_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  SDL_Rect hb_dst;
  unsigned old_id;

  if (!sdl_screen) return;
  old_id = bx_headerbar_entry[hbar_id].bmap_id;
  hb_dst = sdl_bitmaps[old_id]->dst;
  sdl_bitmaps[old_id]->dst.x = -1;
  bx_headerbar_entry[hbar_id].bmap_id = bmap_id;
  sdl_bitmaps[bmap_id]->dst.x = hb_dst.x;
  if (sdl_bitmaps[bmap_id]->dst.x != -1) {
    if (bx_headerbar_entry[hbar_id].alignment == BX_GRAVITY_RIGHT) {
      hb_dst.x = res_x - hb_dst.x;
    }
    SDL_BlitSurface(
        sdl_bitmaps[bmap_id]->surface,
        &sdl_bitmaps[bmap_id]->src,
        sdl_screen,
        &hb_dst);
    SDL_UpdateRect(
        sdl_screen,
        hb_dst.x,
        sdl_bitmaps[bmap_id]->dst.y,
        sdl_bitmaps[bmap_id]->src.w,
        sdl_bitmaps[bmap_id]->src.h);
  }
}


void bx_sdl_gui_c::show_headerbar(void)
{
  Uint32 *buf;
  Uint32 *buf_row;
  Uint32 disp;
  int rowsleft = headerbar_height;
  int colsleft, sb_item;
  int bitmapscount = bx_headerbar_entries;
  unsigned current_bmp, pos_x;
  SDL_Rect hb_dst;

  if (!sdl_screen) return;
  disp = sdl_screen->pitch/4;
  buf = (Uint32 *)sdl_screen->pixels;

  // draw headerbar background
  do {
    colsleft = res_x;
    buf_row = buf;
    do {
      *buf++ = headerbar_bg;
    } while (--colsleft);
    buf = buf_row + disp;
  } while (--rowsleft);
  SDL_UpdateRect( sdl_screen, 0,0,res_x,headerbar_height);

  // go thru the bitmaps and display the active ones
  while (bitmapscount--) {
    current_bmp = bx_headerbar_entry[bitmapscount].bmap_id;
    if(sdl_bitmaps[current_bmp]->dst.x != -1) {
      hb_dst = sdl_bitmaps[current_bmp]->dst;
      if (bx_headerbar_entry[bitmapscount].alignment == BX_GRAVITY_RIGHT) {
        hb_dst.x = res_x - hb_dst.x;
      }
      SDL_BlitSurface(
        sdl_bitmaps[current_bmp]->surface,
        &sdl_bitmaps[current_bmp]->src,
        sdl_screen,
        &hb_dst);
      SDL_UpdateRect(
        sdl_screen,
        hb_dst.x,
        sdl_bitmaps[current_bmp]->dst.y,
        sdl_bitmaps[current_bmp]->src.w,
        sdl_bitmaps[current_bmp]->src.h);
    }
  }
  // draw statusbar background
  rowsleft = statusbar_height;
  buf = (Uint32 *)sdl_screen->pixels + (res_y + headerbar_height) * disp;
  do {
    colsleft = res_x;
    buf_row = buf;
    sb_item = 1;
    pos_x = 0;
    do {
      if (pos_x == statusitem_pos[sb_item]) {
        *buf++ = headerbar_fg;
        if (sb_item < 11) sb_item++;
      } else {
        *buf++ = headerbar_bg;
      }
      pos_x++;
    } while (--colsleft);
    buf = buf_row + disp;
  } while (--rowsleft);
  SDL_UpdateRect( sdl_screen, 0,res_y+headerbar_height,res_x,statusbar_height);
  for (unsigned i=0; i<statusitem_count; i++) {
    sdl_set_status_text(i+1, statusitem[i].text, statusitem_active[i+1]);
  }
}


int bx_sdl_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  return 0;
}


int bx_sdl_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  return 0;
}


void bx_sdl_gui_c::mouse_enabled_changed_specific(bool val)
{
  if (val == 1) {
    SDL_ShowCursor(0);
    SDL_WM_GrabInput(SDL_GRAB_ON);
  } else {
    SDL_ShowCursor(1);
    SDL_WM_GrabInput(SDL_GRAB_OFF);
  }
  sdl_grab = val;
}


void bx_sdl_gui_c::exit(void)
{
  if (sdl_screen)
    SDL_FreeSurface(sdl_screen);
  if (sdl_fullscreen)
    SDL_FreeSurface(sdl_fullscreen);
  while (n_sdl_bitmaps) {
    SDL_FreeSurface(sdl_bitmaps[n_sdl_bitmaps-1]->surface);
    n_sdl_bitmaps--;
  }

#if BX_DEBUGGER && BX_DEBUGGER_GUI
  if (SIM->has_debug_gui()) {
    close_debug_dialog();
  }
#endif
}

// New graphics API methods

bx_svga_tileinfo_t *bx_sdl_gui_c::graphics_tile_info(bx_svga_tileinfo_t *info)
{
  if (sdl_screen) {
    info->bpp = sdl_screen->format->BitsPerPixel;
    info->pitch = sdl_screen->pitch;
    info->red_shift = sdl_screen->format->Rshift + 8 - sdl_screen->format->Rloss;
    info->green_shift = sdl_screen->format->Gshift + 8 - sdl_screen->format->Gloss;
    info->blue_shift = sdl_screen->format->Bshift + 8 - sdl_screen->format->Bloss;
    info->red_mask = sdl_screen->format->Rmask;
    info->green_mask = sdl_screen->format->Gmask;
    info->blue_mask = sdl_screen->format->Bmask;
    info->is_indexed = (sdl_screen->format->palette != NULL);
  } else {
    info->bpp = sdl_fullscreen->format->BitsPerPixel;
    info->pitch = sdl_fullscreen->pitch;
    info->red_shift = sdl_fullscreen->format->Rshift + 8 - sdl_fullscreen->format->Rloss;
    info->green_shift = sdl_fullscreen->format->Gshift + 8 - sdl_fullscreen->format->Gloss;
    info->blue_shift = sdl_fullscreen->format->Bshift + 8 - sdl_fullscreen->format->Bloss;
    info->red_mask = sdl_fullscreen->format->Rmask;
    info->green_mask = sdl_fullscreen->format->Gmask;
    info->blue_mask = sdl_fullscreen->format->Bmask;
    info->is_indexed = (sdl_fullscreen->format->palette != NULL);
  }
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  info->is_little_endian = 0;
#else
  info->is_little_endian = 1;
#endif
  return info;
}


Bit8u *bx_sdl_gui_c::graphics_tile_get(unsigned x0, unsigned y0, unsigned *w, unsigned *h)
{
  if (x0+x_tilesize > res_x) {
    *w = res_x - x0;
  } else {
    *w = x_tilesize;
  }

  if (y0+y_tilesize > res_y) {
    *h = res_y - y0;
  } else {
    *h = y_tilesize;
  }

  if (sdl_screen) {
    return (Bit8u *)sdl_screen->pixels +
           sdl_screen->pitch * (headerbar_height + y0) +
           sdl_screen->format->BytesPerPixel * x0;
  } else {
    return (Bit8u *)sdl_fullscreen->pixels + sdl_fullscreen->offset +
           sdl_fullscreen->pitch * y0 +
           sdl_fullscreen->format->BytesPerPixel * x0;
  }
}


void bx_sdl_gui_c::graphics_tile_update_in_place(unsigned x0, unsigned y0,
                                        unsigned w, unsigned h)
{
  // Nothing to do here
}

// Optional bx_gui_c methods

void bx_sdl_gui_c::set_display_mode(disp_mode_t newmode)
{
  // if no mode change, do nothing.
  if (disp_mode == newmode) return;
  // remember the display mode for next time
  disp_mode = newmode;
  if ((newmode == DISP_MODE_SIM) && console_running()) {
    console_cleanup();
    return;
  }
  // If fullscreen mode is on, we must switch back to windowed mode if
  // the user needs to see the text console.
  if (sdl_fullscreen_toggle) {
    switch (newmode) {
      case DISP_MODE_CONFIG:
        BX_DEBUG(("switch to configuration mode (windowed)"));
        switch_to_windowed();
        break;
      case DISP_MODE_SIM:
        BX_DEBUG(("switch to simulation mode (fullscreen)"));
        switch_to_fullscreen();
        break;
    }
  }
}


void bx_sdl_gui_c::statusbar_setitem_specific(int element, bool active, bool w)
{
  Bit8u color = 0;
  if (w) {
    color = (statusitem[element].auto_off) ? 1 : 2;
  }
  sdl_set_status_text(element+1, statusitem[element].text, active, color);
}


void bx_sdl_gui_c::get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp)
{
  *xres = sdl_maxres.w;
  *yres = sdl_maxres.h;
  *bpp = 32;
}


void bx_sdl_gui_c::set_mouse_mode_absxy(bool mode)
{
  sdl_mouse_mode_absxy = mode;
}


#if BX_SHOW_IPS
void bx_sdl_gui_c::show_ips(Bit32u ips_count)
{
  if (!gui_opts.hide_ips && !sdl_ips_update) {
    ips_count /= 1000;
    sprintf(sdl_ips_text, "IPS: %u.%3.3uM", ips_count / 1000, ips_count % 1000);
    sdl_ips_update = 1;
  }
}
#endif

/// key mapping code for SDL

typedef struct {
  const char *name;
  Bit32u value;
} keyTableEntry;

#define DEF_SDL_KEY(key) \
  { #key, key },

keyTableEntry keytable[] = {
  // this include provides all the entries.
#include "sdlkeys.h"
  // one final entry to mark the end
  { NULL, 0 }
};

// function to convert key names into SDLKey values.
// This first try will be horribly inefficient, but it only has
// to be done while loading a keymap.  Once the simulation starts,
// this function won't be called.
static Bit32u convertStringToSDLKey (const char *string)
{
  keyTableEntry *ptr;
  for (ptr = &keytable[0]; ptr->name != NULL; ptr++) {
    //BX_DEBUG (("comparing string '%s' to SDL key '%s'", string, ptr->name));
    if (!strcmp(string, ptr->name))
      return ptr->value;
  }
  return BX_KEYMAP_UNKNOWN;
}

#endif /* if BX_WITH_SDL */
