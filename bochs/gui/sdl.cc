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

#ifdef linux
#define FIX_SDL_SCANCODE(x) ((x)-8)
#else
#define FIX_SDL_SCANCODE(x) ((x))
#endif

#define MAX_SDL_BITMAPS 32
struct bitmaps {
  SDL_Surface *surface;
  SDL_Rect src,dst;
  void (*cb)(void);
};

SDL_Thread *sdl_thread;
SDL_Surface *sdl_screen, *sdl_fullscreen;
SDL_Event sdl_event;
int sdl_fullscreen_toggle;
int sdl_grab;
int res_x, res_y;
int headerbar_height;
int headerbar_offset;
int textres_x, textres_y;
int fontwidth = 8, fontheight = 16;
unsigned tilewidth, tileheight;
unsigned char *font = &sdl_font8x16[0][0];
unsigned char menufont[256][8];
Uint32 palette[256];
Uint32 headerbar_fg, headerbar_bg;
Bit8u old_mousebuttons=0, new_mousebuttons=0;
int old_mousex=0, new_mousex=0;
int old_mousey=0, new_mousey=0;
bitmaps *sdl_bitmaps[MAX_SDL_BITMAPS];
int n_sdl_bitmaps = 0;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SWAP16(X)    (X)
#define SWAP32(X)    (X)
#else
#define SWAP16(X)    SDL_Swap16(X)
#define SWAP32(X)    SDL_Swap32(X)
#endif



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
  Uint32 color, *buf;

  tilewidth = x_tilesize;
  tileheight = y_tilesize;
  headerbar_height = header_bar_y;

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
  SDL_WarpMouse(res_x/2, res_y/2);
}

void bx_gui_c::text_update(
    Bit8u *old_text,
    Bit8u *new_text,
    unsigned long cursor_x,
    unsigned long cursor_y,
    Bit16u cursor_state,
    unsigned rows)
{
  char *oldText = (char *)old_text;
  char *newText = (char *)new_text;
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
  Boolean invert;

  cs_start = cursor_state >> 8;
  cs_end = cursor_state & 0xff;

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
    y = 25 - rows;
    do
    {
      // check if char needs to be updated
      if( (old_text[0] != new_text[0])
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
	pfont_row = &font[(new_text[0]*fontheight)];
	buf_char = buf;
	do
	{
	  font_row = *pfont_row++;
	  fontpixels = fontwidth;
	  cs_line = (16 - fontrows);
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


void bx_gui_c::handle_events(void)
{
  Bit32u key_event;

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
	new_mousebuttons = ((sdl_event.motion.state & 0x01)|((sdl_event.motion.state>>1)&0x02));
	bx_devices.keyboard->mouse_motion(
	    sdl_event.motion.xrel,
	    -sdl_event.motion.yrel,
	    new_mousebuttons );
	old_mousebuttons = new_mousebuttons;
	old_mousex = (int)(sdl_event.motion.x);
	old_mousey = (int)(sdl_event.motion.y);
	break;

      case SDL_MOUSEBUTTONDOWN:
	if( (sdl_event.button.button == SDL_BUTTON(2))
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
	  break;
	}
      case SDL_MOUSEBUTTONUP:
	// figure out mouse state
	new_mousex = (int)(sdl_event.button.x);
	new_mousey = (int)(sdl_event.button.y);
	new_mousebuttons =
	  (sdl_event.button.state & 0x01)	|
	  ((sdl_event.button.state>>1)&0x02)	|
	  ((sdl_event.button.state<<1)&0x04)	;
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
	  Uint32 *buf, *buf_row;
	  Uint32 *buf2, *buf_row2;
	  Uint32 disp, disp2;
	  int rows, cols;
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

	// convert scancode->bochs code
	if( sdl_event.key.keysym.scancode > _SCN2BX_LAST_ ) break;
	key_event = scancodes2bx[ FIX_SDL_SCANCODE(sdl_event.key.keysym.scancode) ][1];
	if( key_event == 0 ) break;
	bx_devices.keyboard->gen_scancode( key_event );
	break;

      case SDL_KEYUP:

	// filter out release of Windows/Fullscreen toggle and unsupported keys
	if( (sdl_event.key.keysym.sym != SDLK_SCROLLOCK)
	    && (sdl_event.key.keysym.scancode < _SCN2BX_LAST_ ))
	{
	  // convert scancode->bochs code
	  key_event = scancodes2bx[ FIX_SDL_SCANCODE(sdl_event.key.keysym.scancode) ][1];
	  if( key_event == 0 ) break;
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
    unsigned y)
{
  int i=headerbar_height;

  // TODO: remove this stupid check whenever the vga driver is fixed
  if( y == 208 ) y = 200;
  // TODO: remove this stupid check whenever 80x50 font is properly handled
  if( y > x )
  {
    y = y>>1;
    if( font != &sdl_font8x8[0][0] )
    {
      bx_gui.clear_screen();
      font = &sdl_font8x8[0][0];
      fontheight = 8;
      fontwidth = 8;
    }
  }
  else
  {
    if( font != &sdl_font8x16[0][0] )
    {
      bx_gui.clear_screen();
      font = &sdl_font8x16[0][0];
      fontheight = 16;
      fontwidth = 8;
    }
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
  textres_x = x / fontwidth;
  textres_y = y / fontheight;
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
      0x000000ff
#else
      0x000000ff,
      0x0000ff00,
      0x00ff0000,
      0xff000000
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
	if( (pixels & 0x80) == 0 )
	  *buf++ = headerbar_bg;
	else
	  *buf++ = headerbar_fg;
	pixels = pixels << 1;
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
  if( bmap_id >= n_sdl_bitmaps ) return 0;

  sdl_bitmaps[bmap_id]->dst.x = headerbar_offset;
  headerbar_offset += sdl_bitmaps[bmap_id]->src.w;
  sdl_bitmaps[bmap_id]->cb = f;
  if( sdl_screen )
  {
    SDL_BlitSurface(
	sdl_bitmaps[bmap_id]->surface,
	&sdl_bitmaps[bmap_id]->src,
	sdl_screen,
	&sdl_bitmaps[bmap_id]->dst);
    SDL_UpdateRect(
	sdl_screen,
	sdl_bitmaps[bmap_id]->dst.x,
	sdl_bitmaps[bmap_id]->dst.y,
	sdl_bitmaps[bmap_id]->src.w,
	sdl_bitmaps[bmap_id]->src.h);
  }
  return bmap_id;
}


void bx_gui_c::replace_bitmap(
    unsigned hbar_id,
    unsigned bmap_id)
{
  sdl_bitmaps[bmap_id]->dst.x = sdl_bitmaps[hbar_id]->dst.x;
  sdl_bitmaps[bmap_id]->cb = sdl_bitmaps[hbar_id]->cb;
  sdl_bitmaps[hbar_id]->dst.x = -1;
  sdl_bitmaps[hbar_id]->cb = NULL;
}


void bx_gui_c::show_headerbar(void)
{
  Uint32 *buf;
  Uint32 *buf_row;
  Uint32 disp;
  int rowsleft = headerbar_height;
  int colsleft;
  int bitmapscount = n_sdl_bitmaps;

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
    if( sdl_bitmaps[bitmapscount]->dst.x != -1 )
    {
      SDL_BlitSurface(
	  sdl_bitmaps[bitmapscount]->surface,
	  &sdl_bitmaps[bitmapscount]->src,
	  sdl_screen,
	  &sdl_bitmaps[bitmapscount]->dst);
      SDL_UpdateRect(
	  sdl_screen,
	  sdl_bitmaps[bitmapscount]->dst.x,
	  sdl_bitmaps[bitmapscount]->dst.y,
	  sdl_bitmaps[bitmapscount]->src.w,
	  sdl_bitmaps[bitmapscount]->src.h );
    }
  }
}


void bx_gui_c::mouse_enabled_changed_specific (Boolean val)
{
  BX_INFO (("mouse enabled changed specific"));
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

