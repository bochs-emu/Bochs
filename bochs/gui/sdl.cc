/////////////////////////////////////////////////////////////////////////
// $Id: sdl.cc,v 1.27 2002-10-12 13:10:09 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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

#define _MULTI_THREAD

#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <SDL/SDL_thread.h>

#include "bochs.h"
#include "icon_bochs.h"
#include "sdl.h"

#define LOG_THIS bx_gui.

#define _SDL_DEBUG_ME_

#ifdef _SDL_DEBUG_ME_
void we_are_here(void)
{
  return;
}
#endif

static unsigned prev_cursor_x=0;
static unsigned prev_cursor_y=0;
static Bit32u convertStringToSDLKey (const char *string);

#define MAX_SDL_BITMAPS 32
struct bitmaps {
  SDL_Surface *surface;
  SDL_Rect src,dst;
  void (*cb)(void);
};

static struct {
  unsigned bmp_id;
  unsigned alignment;
  void (*f)(void);
} hb_entry[BX_MAX_HEADERBAR_ENTRIES];

unsigned bx_headerbar_entries = 0;

SDL_Thread *sdl_thread;
SDL_Surface *sdl_screen, *sdl_fullscreen;
SDL_Event sdl_event;
int sdl_fullscreen_toggle;
int sdl_grab;
unsigned res_x, res_y;
unsigned half_res_x, half_res_y;
int headerbar_height;
static unsigned bx_bitmap_left_xorigin = 0;  // pixels from left
static unsigned bx_bitmap_right_xorigin = 0; // pixels from right
int textres_x, textres_y;
int fontwidth = 8, fontheight = 16;
unsigned tilewidth, tileheight;
unsigned char menufont[256][8];
Uint32 palette[256];
Uint32 headerbar_fg, headerbar_bg;
Bit8u old_mousebuttons=0, new_mousebuttons=0;
int old_mousex=0, new_mousex=0;
int old_mousey=0, new_mousey=0;
Boolean just_warped = false;
bitmaps *sdl_bitmaps[MAX_SDL_BITMAPS];
int n_sdl_bitmaps = 0;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SWAP16(X)    (X)
#define SWAP32(X)    (X)
#else
#define SWAP16(X)    SDL_Swap16(X)
#define SWAP32(X)    SDL_Swap32(X)
#endif

static void headerbar_click(int x);


void switch_to_windowed(void)
{
  SDL_Surface *tmp;
  SDL_Rect src, dst;
  src.x = 0; src.y = 0;
  src.w = res_x; src.h = res_y;
  dst.x = 0; dst.y = 0;

  tmp = SDL_CreateRGBSurface(
      SDL_SWSURFACE,
      res_x,
      res_y,
      32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      0xff000000,
      0x00ff0000,
      0x0000ff00,
      0x000000ff
#else
      0x000000ff,
      0x0000ff00,
      0x00ff0000,
      0xff000000
#endif
      );

  SDL_BlitSurface(sdl_fullscreen,&src,tmp,&dst);
  SDL_UpdateRect(tmp,0,0,res_x,res_y);
  SDL_FreeSurface(sdl_fullscreen);
  sdl_fullscreen = NULL;

  sdl_screen = SDL_SetVideoMode(res_x,res_y+headerbar_height,32, SDL_SWSURFACE);
  dst.y = headerbar_height;
  SDL_BlitSurface(tmp,&src,sdl_screen,&dst);
  SDL_UpdateRect(tmp,0,0,res_x,res_y+headerbar_height);
  SDL_FreeSurface(tmp);

  SDL_ShowCursor(1);
  SDL_WM_GrabInput(SDL_GRAB_OFF);
  bx_gui.show_headerbar();
  sdl_grab = 0;
}

void switch_to_fullscreen(void)
{
  SDL_Surface *tmp;
  SDL_Rect src, dst;
  src.x = 0; src.y = headerbar_height;
  src.w = res_x; src.h = res_y;
  dst.x = 0; dst.y = 0;

  tmp = SDL_CreateRGBSurface(
      SDL_SWSURFACE,
      res_x,
      res_y,
      32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      0xff000000,
      0x00ff0000,
      0x0000ff00,
      0x000000ff
#else
      0x000000ff,
      0x0000ff00,
      0x00ff0000,
      0xff000000
#endif
      );
  SDL_BlitSurface(sdl_screen,&src,tmp,&dst);
  SDL_UpdateRect(tmp,0,0,res_x,res_y);
  SDL_FreeSurface(sdl_screen);
  sdl_screen = NULL;

  sdl_fullscreen = SDL_SetVideoMode(res_x,res_y,32, SDL_HWSURFACE|SDL_FULLSCREEN);
  src.y = 0;
  SDL_BlitSurface(tmp,&src,sdl_fullscreen,&dst);
  SDL_UpdateRect(tmp,0,0,res_x,res_y);
  SDL_FreeSurface(tmp);

  SDL_ShowCursor(0);
  SDL_WM_GrabInput(SDL_GRAB_ON);
  sdl_grab = 1;
}

void bx_gui_c::specific_init(
    bx_gui_c *th,
    int argc,
    char **argv,
    unsigned x_tilesize,
    unsigned y_tilesize,
    unsigned header_bar_y)
{
  int i,j;

  th->put("SDL");

  tilewidth = x_tilesize;
  tileheight = y_tilesize;
  headerbar_height = header_bar_y;

  for(i=0;i<256;i++)
    for(j=0;j<16;j++)
      bx_gui.vga_charmap[i*32+j] = sdl_font8x16[i][j];

  for(i=0;i<256;i++)
    for(j=0;j<8;j++)
      menufont[i][j] = sdl_font8x8[i][j];

  if( SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
    BX_PANIC (("Unable to initialize SDL libraries"));
    return;
  }
  atexit(SDL_Quit);

  sdl_screen = NULL;
  th->dimension_update(640,480);

  sdl_fullscreen_toggle = 0;

  SDL_EnableKeyRepeat(250,50);
  SDL_WM_SetCaption(
#if BX_CPU_LEVEL < 2
      "Bochs 8086 emulator, http://bochs.sourceforge.net/",
#elif BX_CPU_LEVEL == 2
      "Bochs 80286 emulator, http://bochs.sourceforge.net/",
#elif BX_CPU_LEVEL == 3
      "Bochs 80386 emulator, http://bochs.sourceforge.net/",
#elif BX_CPU_LEVEL == 4
      "Bochs 80486 emulator, http://bochs.sourceforge.net/",
#else
      "Bochs Pentium emulator, http://bochs.sourceforge.net/",
#endif
      "Bochs" );
  SDL_WarpMouse(half_res_x, half_res_y);

  // load keymap for sdl
  if(bx_options.keyboard.OuseMapping->get()) {
    bx_keymap.loadKeymap(convertStringToSDLKey);
  }
}

void bx_gui_c::text_update(
    Bit8u *old_text,
    Bit8u *new_text,
    unsigned long cursor_x,
    unsigned long cursor_y,
    Bit16u cursor_state,
    unsigned rows)
{
  unsigned char font_row, *pfont_row;
  unsigned long x,y;
  int hchars,fontrows,fontpixels;
  int fgcolor_ndx;
  int bgcolor_ndx;
  Uint32 fgcolor;
  Uint32 bgcolor;
  Uint32 *buf, *buf_row, *buf_char;
  Uint32 disp;
  Bit8u cs_start, cs_end, cs_line, mask;
  Boolean invert, forceUpdate;

  cs_start = (cursor_state >> 8) & 0x3f;
  cs_end = cursor_state & 0x1f;

  forceUpdate = 0;
  if(bx_gui.charmap_updated)
  {
    forceUpdate = 1;
    bx_gui.charmap_updated = 0;
  }
  if( sdl_screen )
  {
    disp = sdl_screen->pitch/4;
    buf_row = (Uint32 *)sdl_screen->pixels + headerbar_height*disp;
  }
  else
  {
    disp = sdl_fullscreen->pitch/4;
    buf_row = (Uint32 *)sdl_fullscreen->pixels;
  }
  
  do
  {
    buf = buf_row;
    hchars = textres_x;
    x = 0;
    y = textres_y - rows;
    do
    {
      // check if char needs to be updated
      if(forceUpdate || (old_text[0] != new_text[0])
	  || (old_text[1] != new_text[1])
	  || ((y == cursor_y) && (x == cursor_x))
	  || ((y == prev_cursor_y) && (x == prev_cursor_x)) )
      {

	// Get Foreground/Background pixel colors
	fgcolor_ndx = new_text[1] & 0x0F;
	bgcolor_ndx = (new_text[1] >> 4) & 0x07;
	fgcolor = palette[fgcolor_ndx];
	bgcolor = palette[bgcolor_ndx];
	invert = ( (y == cursor_y) && (x == cursor_x) && (cs_start < cs_end) );
	
	// Display this one char
	fontrows = fontheight;
	pfont_row = &bx_gui.vga_charmap[(new_text[0] << 5)];
	buf_char = buf;
	do
	{
	  font_row = *pfont_row++;
	  fontpixels = fontwidth;
	  cs_line = (fontheight - fontrows);
	  if( (invert) && (cs_line >= cs_start) && (cs_line <= cs_end) )
	    mask = 0x80;
	  else
	    mask = 0x00;
	  do
	  {
	    if( (font_row & 0x80) == mask )
	      *buf = bgcolor;
	    else
	      *buf = fgcolor;
	    buf++;
	    font_row = font_row << 1;
	  } while( --fontpixels );
	  buf -= fontwidth;
	  buf += disp;
	} while( --fontrows );

	// restore output buffer ptr to start of this char
	buf = buf_char;
      }
      // move to next char location on screen
      buf += fontwidth;
      
      // select next char in old/new text
      new_text+=2;
      old_text+=2;
      x++;
	
    // process one entire horizontal row
    } while( --hchars );

    // go to next character row location
    buf_row += disp * fontheight;
  } while( --rows );
  prev_cursor_x = cursor_x;
  prev_cursor_y = cursor_y;
}

  int
bx_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  return 0;
}

  int
bx_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  return 0;
}


void bx_gui_c::graphics_tile_update(
    Bit8u *snapshot,
    unsigned x,
    unsigned y)
{
  Uint32 *buf, disp;
  Uint32 *buf_row;
  int i,j;
  
  if( sdl_screen )
  {
    disp = sdl_screen->pitch/4;
    buf = (Uint32 *)sdl_screen->pixels + (headerbar_height+y)*disp + x;
  }
  else
  {
    disp = sdl_fullscreen->pitch/4;
    buf = (Uint32 *)sdl_fullscreen->pixels + y*disp + x;
  }

  i = tileheight;
  if( i + y > res_y ) i = res_y - y;

  // FIXME
  if( i<=0 ) return;

  do
  {
    buf_row = buf;
    j = tilewidth;
    do
    {
      *buf++ = palette[*snapshot++];
    } while( --j );
    buf = buf_row + disp;
  } while( --i);
}

static Bit32u sdl_sym_to_bx_key (SDLKey sym)
{
  switch (sym)
  {
//  case SDLK_UNKNOWN:              return BX_KEY_UNKNOWN;
//  case SDLK_FIRST:                return BX_KEY_FIRST;
    case SDLK_BACKSPACE:            return BX_KEY_BACKSPACE;
    case SDLK_TAB:                  return BX_KEY_TAB;
//  case SDLK_CLEAR:                return BX_KEY_CLEAR;
    case SDLK_RETURN:               return BX_KEY_ENTER;
    case SDLK_PAUSE:                return BX_KEY_PAUSE;
    case SDLK_ESCAPE:               return BX_KEY_ESC;
    case SDLK_SPACE:                return BX_KEY_SPACE;
//  case SDLK_EXCLAIM:              return BX_KEY_EXCLAIM;
//  case SDLK_QUOTEDBL:             return BX_KEY_QUOTEDBL;
//  case SDLK_HASH:                 return BX_KEY_HASH;
//  case SDLK_DOLLAR:               return BX_KEY_DOLLAR;
//  case SDLK_AMPERSAND:            return BX_KEY_AMPERSAND;
    case SDLK_QUOTE:                return BX_KEY_SINGLE_QUOTE;
//  case SDLK_LEFTPAREN:            return BX_KEY_LEFTPAREN;
//  case SDLK_RIGHTPAREN:           return BX_KEY_RIGHTPAREN;
//  case SDLK_ASTERISK:             return BX_KEY_ASTERISK;
//  case SDLK_PLUS:                 return BX_KEY_PLUS;
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
//  case SDLK_COLON:                return BX_KEY_COLON;
    case SDLK_SEMICOLON:            return BX_KEY_SEMICOLON;
//  case SDLK_LESS:                 return BX_KEY_LESS;
    case SDLK_EQUALS:               return BX_KEY_EQUALS;
//  case SDLK_GREATER:              return BX_KEY_GREATER;
//  case SDLK_QUESTION:             return BX_KEY_QUESTION;
//  case SDLK_AT:                   return BX_KEY_AT;
/*
 Skip uppercase letters
*/
    case SDLK_LEFTBRACKET:          return BX_KEY_LEFT_BRACKET;
    case SDLK_BACKSLASH:            return BX_KEY_BACKSLASH;
    case SDLK_RIGHTBRACKET:         return BX_KEY_RIGHT_BRACKET;
//  case SDLK_CARET:                return BX_KEY_CARET;
//  case SDLK_UNDERSCORE:           return BX_KEY_UNDERSCORE;
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
//  case SDLK_KP_EQUALS:            return BX_KEY_KP_EQUALS;

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
//  case SDLK_F13:                  return BX_KEY_F13;
//  case SDLK_F14:                  return BX_KEY_F14;
//  case SDLK_F15:                  return BX_KEY_F15;

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
//  case SDLK_LMETA:                return BX_KEY_LMETA;
    case SDLK_LSUPER:               return BX_KEY_WIN_L;
    case SDLK_RSUPER:               return BX_KEY_WIN_R;
//  case SDLK_MODE:                 return BX_KEY_MODE;
//  case SDLK_COMPOSE:              return BX_KEY_COMPOSE;

/* Miscellaneous function keys */
    case SDLK_PRINT:                return BX_KEY_PRINT;
    case SDLK_BREAK:                return BX_KEY_PAUSE;
    case SDLK_MENU:                 return BX_KEY_MENU;
#if 0
    case SDLK_HELP:                 return BX_KEY_HELP;
    case SDLK_SYSREQ:               return BX_KEY_SYSREQ;
    case SDLK_POWER:                return BX_KEY_POWER;
    case SDLK_EURO:                 return BX_KEY_EURO;
    case SDLK_UNDO:                 return BX_KEY_UNDO;
#endif
    default:
      BX_ERROR (("sdl keysym %d not mapped", (int)sym));
      return BX_KEY_UNHANDLED;
  }
}


void bx_gui_c::handle_events(void)
{
  Bit32u key_event;
  Bit8u mouse_state;

  while( SDL_PollEvent(&sdl_event) )
  {
    switch( sdl_event.type )
    {
      case SDL_VIDEOEXPOSE:
	if( sdl_fullscreen_toggle == 0 )
	  SDL_UpdateRect( sdl_screen, 0,0, res_x, res_y+headerbar_height );
	else
	  SDL_UpdateRect( sdl_screen, 0,headerbar_height, res_x, res_y );
	break;

      case SDL_MOUSEMOTION:
	//fprintf (stderr, "mouse event to (%d,%d), relative (%d,%d)\n", (int)(sdl_event.motion.x), (int)(sdl_event.motion.y), (int)sdl_event.motion.xrel, (int)sdl_event.motion.yrel);
	if (!sdl_grab) {
	  //fprintf (stderr, "ignore mouse event because sdl_grab is off\n");
	  break;
	}
	if (just_warped 
	    && sdl_event.motion.x == half_res_x
	    && sdl_event.motion.y == half_res_y) {
	  // This event was generated as a side effect of the WarpMouse,
	  // and it must be ignored.
	  //fprintf (stderr, "ignore mouse event because it is a side effect of SDL_WarpMouse\n");
	  just_warped = false;
	  break;
	}
	//fprintf (stderr, "processing relative mouse event\n");
	new_mousebuttons = ((sdl_event.motion.state & 0x01)|((sdl_event.motion.state>>1)&0x02));
	bx_devices.keyboard->mouse_motion(
	    sdl_event.motion.xrel,
	    -sdl_event.motion.yrel,
	    new_mousebuttons );
	old_mousebuttons = new_mousebuttons;
	old_mousex = (int)(sdl_event.motion.x);
	old_mousey = (int)(sdl_event.motion.y);
	//fprintf (stderr, "warping mouse to center\n");
	SDL_WarpMouse(half_res_x, half_res_y);
	just_warped = 1;
	break;

      case SDL_MOUSEBUTTONDOWN:
	if( (sdl_event.button.button == SDL_BUTTON_MIDDLE)
	    && (sdl_fullscreen_toggle == 0) )
	{
	  if( sdl_grab == 0 )
	  {
	    SDL_ShowCursor(0);
	    SDL_WM_GrabInput(SDL_GRAB_ON);
	  }
	  else
	  {
	    SDL_ShowCursor(1);
	    SDL_WM_GrabInput(SDL_GRAB_OFF);
	  }
	  sdl_grab = ~sdl_grab;
	  toggle_mouse_enable();
	  break;
	} else if (sdl_event.button.y < headerbar_height) {
	  headerbar_click(sdl_event.button.x);
	  break;
	}
      case SDL_MOUSEBUTTONUP:
	// figure out mouse state
	new_mousex = (int)(sdl_event.button.x);
	new_mousey = (int)(sdl_event.button.y);
	// SDL_GetMouseState() returns the state of all buttons
	mouse_state = SDL_GetMouseState(NULL, NULL);
	new_mousebuttons =
	  (mouse_state & 0x01)    |
	  ((mouse_state>>1)&0x02) |
	  ((mouse_state<<1)&0x04) ;
	// filter out middle button if not fullscreen
	if( sdl_fullscreen_toggle == 0 )
	  new_mousebuttons &= 0x03;
	// send motion information
	bx_devices.keyboard->mouse_motion(
	    new_mousex - old_mousex,
	    -(new_mousey - old_mousey),
	    new_mousebuttons );
	// mark current state to diff with next packet
	old_mousebuttons = new_mousebuttons;
	old_mousex = new_mousex;
	old_mousey = new_mousey;
	break;

      case SDL_KEYDOWN:

	// Windows/Fullscreen toggle-check
	if( sdl_event.key.keysym.sym == SDLK_SCROLLOCK )
	{
//	  SDL_WM_ToggleFullScreen( sdl_screen );
	  sdl_fullscreen_toggle = ~sdl_fullscreen_toggle;
	  if( sdl_fullscreen_toggle == 0 )
	    switch_to_windowed();
	  else
	    switch_to_fullscreen();
	  bx_gui.show_headerbar();
	  bx_gui.flush();
	  break;
	}

	// convert sym->bochs code
	if( sdl_event.key.keysym.sym > SDLK_LAST ) break;
        if (!bx_options.keyboard.OuseMapping->get()) {
	  key_event = sdl_sym_to_bx_key (sdl_event.key.keysym.sym);
	  BX_DEBUG (("keypress scancode=%d, sym=%d, bx_key = %d", sdl_event.key.keysym.scancode, sdl_event.key.keysym.sym, key_event));
	} else {
	  /* use mapping */
	  BXKeyEntry *entry = bx_keymap.findHostKey (sdl_event.key.keysym.sym);
	  if (!entry) {
	    BX_ERROR(( "host key %d (0x%x) not mapped!", 
		  (unsigned) sdl_event.key.keysym.sym,
		  (unsigned) sdl_event.key.keysym.sym));
	    break;
	  }
	  key_event = entry->baseKey;
	}
	if( key_event == BX_KEY_UNHANDLED ) break;
	bx_devices.keyboard->gen_scancode( key_event );
	break;

      case SDL_KEYUP:

	// filter out release of Windows/Fullscreen toggle and unsupported keys
	if( (sdl_event.key.keysym.sym != SDLK_SCROLLOCK)
	    && (sdl_event.key.keysym.sym < SDLK_LAST ))
	{
	  // convert sym->bochs code
          if (!bx_options.keyboard.OuseMapping->get()) {
            key_event = sdl_sym_to_bx_key (sdl_event.key.keysym.sym);
          } else {
            /* use mapping */
            BXKeyEntry *entry = bx_keymap.findHostKey (sdl_event.key.keysym.sym);
            if (!entry) {
              BX_ERROR(( "host key %d (0x%x) not mapped!", 
		    (unsigned) sdl_event.key.keysym.sym,
		    (unsigned) sdl_event.key.keysym.sym));
              break;
            }
            key_event = entry->baseKey;
          }
	  if( key_event == BX_KEY_UNHANDLED ) break;
	  bx_devices.keyboard->gen_scancode( key_event | BX_KEY_RELEASED );
	}
	break;

      case SDL_QUIT:
	LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
	BX_PANIC (("User requested shutdown."));
    }
  }
}



void bx_gui_c::flush(void)
{
  if( sdl_screen )
    SDL_UpdateRect( sdl_screen,0,0,res_x,res_y+headerbar_height );
  else
    SDL_UpdateRect( sdl_fullscreen,0,0,res_x,res_y);
}


void bx_gui_c::clear_screen(void)
{
  int i = res_y, j;
  Uint32 color;
  Uint32 *buf, *buf_row;
  Uint32 disp;

  if( sdl_screen )
  {
    color = SDL_MapRGB( sdl_screen->format, 0,0,0 );
    disp = sdl_screen->pitch/4;
    buf = (Uint32 *)sdl_screen->pixels + headerbar_height*disp;
  }
  else if( sdl_fullscreen )
  {
    color = SDL_MapRGB( sdl_fullscreen->format, 0,0,0 );
    disp = sdl_fullscreen->pitch/4;
    buf = (Uint32 *)sdl_fullscreen->pixels;
  }
  else return;

  do
  {
    buf_row = buf;
    j = res_x;
    while( j-- ) *buf++ = color;
    buf = buf_row + disp;
  } while( --i );

  if( sdl_screen )
    SDL_UpdateRect(sdl_screen,0,0,res_x,res_y+headerbar_height);
  else
    SDL_UpdateRect(sdl_fullscreen,0,0,res_x,res_y);
}



Boolean bx_gui_c::palette_change(
    unsigned index,
    unsigned red,
    unsigned green,
    unsigned blue)
{
  unsigned char palred = red & 0xFF;
  unsigned char palgreen = green & 0xFF;
  unsigned char palblue = blue & 0xFF;

  if( index > 255 ) return 0;

  if( sdl_screen )
    palette[index] = SDL_MapRGB( sdl_screen->format, palred, palgreen, palblue );
  else if( sdl_fullscreen )
    palette[index] = SDL_MapRGB( sdl_fullscreen->format, palred, palgreen, palblue );

  return 1;
}


void bx_gui_c::dimension_update(
    unsigned x,
    unsigned y,
    unsigned fheight)
{
  // TODO: remove this stupid check whenever the vga driver is fixed
  if( y == 208 ) y = 200;

  if( fheight > 0 )
  {
    fontheight = fheight;
    fontwidth = 8;
  }

  if( (x == res_x) && (y == res_y )) return;

  if( sdl_screen )
  {
    SDL_FreeSurface( sdl_screen );
    sdl_screen = NULL;
  }
  if( sdl_fullscreen )
  {
    SDL_FreeSurface( sdl_fullscreen );
    sdl_fullscreen = NULL;
  }

  if( sdl_fullscreen_toggle == 0 )
  {
    sdl_screen = SDL_SetVideoMode( x, y+headerbar_height, 32, SDL_SWSURFACE );
    if( !sdl_screen )
    {
      LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
      BX_PANIC (("Unable to set requested videomode: %ix%i: %s",x,y,SDL_GetError()));
    }
    headerbar_fg = SDL_MapRGB(
	sdl_screen->format,
	BX_HEADERBAR_FG_RED,
	BX_HEADERBAR_FG_GREEN,
	BX_HEADERBAR_FG_BLUE );
    headerbar_bg = SDL_MapRGB(
	sdl_screen->format,
	BX_HEADERBAR_BG_RED,
	BX_HEADERBAR_BG_GREEN,
	BX_HEADERBAR_BG_BLUE );
  }
  else
  {
    sdl_fullscreen = SDL_SetVideoMode( x, y, 32, SDL_HWSURFACE|SDL_FULLSCREEN );
    if( !sdl_fullscreen )
    {
      LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
      BX_PANIC (("Unable to set requested videomode: %ix%i: %s",x,y,SDL_GetError()));
    }
  }
  res_x = x;
  res_y = y;
  half_res_x = x/2;
  half_res_y = y/2;
  if( fheight > 0 )
  {
    textres_x = x / fontwidth;
    textres_y = y / fontheight;
  }
  bx_gui.show_headerbar();
}


unsigned bx_gui_c::create_bitmap(
    const unsigned char *bmap,
    unsigned xdim,
    unsigned ydim)
{
  bitmaps *tmp = new bitmaps;
  Uint32 *buf, *buf_row;
  Uint32 disp;
  unsigned char pixels;

  if (n_sdl_bitmaps >= MAX_SDL_BITMAPS) {
    BX_PANIC (("too many SDL bitmaps. To fix, increase MAX_SDL_BITMAPS"));
    return 0;
  }

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
  if( !tmp->surface )
  {
    delete tmp;
    bx_gui.exit();
    LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
    BX_PANIC (("Unable to create requested bitmap"));
  }
  tmp->src.w = xdim;
  tmp->src.h = ydim;
  tmp->src.x = 0;
  tmp->src.y = 0;
  tmp->dst.x = -1;
  tmp->dst.y = 0;
  tmp->dst.w = xdim;
  tmp->dst.h = ydim;
  tmp->cb = NULL;
  buf = (Uint32 *)tmp->surface->pixels;
  disp = tmp->surface->pitch/4;
  do
  {
    buf_row = buf;
    xdim = tmp->src.w / 8;
    do
    {
      pixels = *bmap++;
      for(unsigned i=0;i<8;i++)
      {
	if( (pixels & 0x01) == 0 )
	  *buf++ = headerbar_bg;
	else
	  *buf++ = headerbar_fg;
	pixels = pixels >> 1;
      }
    } while( --xdim );
    buf = buf_row + disp;
  } while( --ydim );
  SDL_UpdateRect(
      tmp->surface,
      0, 0,
      tmp->src.w,
      tmp->src.h );
  sdl_bitmaps[n_sdl_bitmaps] = tmp;
  return n_sdl_bitmaps++;
}


unsigned bx_gui_c::headerbar_bitmap(
    unsigned bmap_id,
    unsigned alignment,
    void (*f)(void))
{
  unsigned hb_index;

  if( bmap_id >= (unsigned)n_sdl_bitmaps ) return 0;

  if ( (bx_headerbar_entries+1) > BX_MAX_HEADERBAR_ENTRIES )
    BX_PANIC(("too many headerbar entries, increase BX_MAX_HEADERBAR_ENTRIES"));

  bx_headerbar_entries++;
  hb_index = bx_headerbar_entries - 1;

  hb_entry[hb_index].bmp_id = bmap_id;
  hb_entry[hb_index].alignment = alignment;
  hb_entry[hb_index].f = f;
  if (alignment == BX_GRAVITY_LEFT) {
    sdl_bitmaps[bmap_id]->dst.x = bx_bitmap_left_xorigin;
    bx_bitmap_left_xorigin += sdl_bitmaps[bmap_id]->src.w;
  } else {
    bx_bitmap_right_xorigin += sdl_bitmaps[bmap_id]->src.w;
    sdl_bitmaps[bmap_id]->dst.x = bx_bitmap_right_xorigin;
  }
  return hb_index;
}


void bx_gui_c::replace_bitmap(
    unsigned hbar_id,
    unsigned bmap_id)
{
  SDL_Rect hb_dst;
  unsigned old_id;

  old_id = hb_entry[hbar_id].bmp_id;
  hb_dst = sdl_bitmaps[old_id]->dst;
  sdl_bitmaps[old_id]->dst.x = -1;
  hb_entry[hbar_id].bmp_id = bmap_id;
  sdl_bitmaps[bmap_id]->dst.x = hb_dst.x;
  if( sdl_bitmaps[bmap_id]->dst.x != -1 )
  {
    if (hb_entry[hbar_id].alignment == BX_GRAVITY_RIGHT) {
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
        sdl_bitmaps[bmap_id]->src.h );
  }
}


void bx_gui_c::show_headerbar(void)
{
  Uint32 *buf;
  Uint32 *buf_row;
  Uint32 disp;
  int rowsleft = headerbar_height;
  int colsleft;
  int bitmapscount = bx_headerbar_entries;
  unsigned current_bmp;
  SDL_Rect hb_dst;

  if( !sdl_screen ) return;
  disp = sdl_screen->pitch/4;
  buf = (Uint32 *)sdl_screen->pixels;

  // draw headerbar background
  do
  {
    colsleft = res_x;
    buf_row = buf;
    do
    {
      *buf++ = headerbar_bg;
    } while( --colsleft );
    buf = buf_row + disp;
  } while( --rowsleft );
  SDL_UpdateRect( sdl_screen, 0,0,res_x,headerbar_height);

  we_are_here();
  // go thru the bitmaps and display the active ones
  while( bitmapscount-- )
  {
    current_bmp = hb_entry[bitmapscount].bmp_id;
    if( sdl_bitmaps[current_bmp]->dst.x != -1 )
    {
      hb_dst = sdl_bitmaps[current_bmp]->dst;
      if (hb_entry[bitmapscount].alignment == BX_GRAVITY_RIGHT) {
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
	  sdl_bitmaps[current_bmp]->src.h );
    }
  }
}


void bx_gui_c::mouse_enabled_changed_specific (Boolean val)
{
  if( val == 1 )
  {
    SDL_ShowCursor(0);
    SDL_WM_GrabInput(SDL_GRAB_ON);
  }
  else
  {
    SDL_ShowCursor(1);
    SDL_WM_GrabInput(SDL_GRAB_OFF);
  }
  sdl_grab = val;
}


void headerbar_click(int x)
{
  int xdim,xorigin;

  for (unsigned i=0; i<bx_headerbar_entries; i++) {
    xdim = sdl_bitmaps[hb_entry[i].bmp_id]->src.w;
    if (hb_entry[i].alignment == BX_GRAVITY_LEFT)
      xorigin = sdl_bitmaps[hb_entry[i].bmp_id]->dst.x;
    else
      xorigin = res_x - sdl_bitmaps[hb_entry[i].bmp_id]->dst.x;
    if ( (x>=xorigin) && (x<(xorigin+xdim)) ) {
      hb_entry[i].f();
      return;
      }
    }
}

void bx_gui_c::exit(void)
{
  if( sdl_screen )
    SDL_FreeSurface(sdl_screen);
  if( sdl_fullscreen )
    SDL_FreeSurface(sdl_fullscreen);
  while( n_sdl_bitmaps )
  {
    SDL_FreeSurface( sdl_bitmaps[n_sdl_bitmaps-1]->surface );
    n_sdl_bitmaps--;
  }
}

/// key mapping for SDL
typedef struct keyTableEntry {
  const char *name;
  Bit32u value;
};

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
    if (!strcmp (string, ptr->name))
      return ptr->value;
  }
  return BX_KEYMAP_UNKNOWN;
}

