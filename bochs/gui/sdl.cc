#define _MULTI_THREAD

#include <iostream>

#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <SDL/SDL_thread.h>

#include "bochs.h"
#include "icon_bochs.h"
#include "sdl.h"

#define LOG_THIS bx_gui.

SDL_Thread *sdl_thread;
SDL_Surface *sdl_screen, *sdl_menuscreen;
SDL_Event sdl_event;
int sdl_fullscreen;
int sdl_grab;
int res_x, res_y;
int headerbar_height;
int textres_x, textres_y;
int fontwidth = 8, fontheight = 16;
unsigned tilewidth, tileheight;
unsigned char *font = &sdl_font8x16[0][0];
unsigned char menufont[256][8];
Uint32 palette[256];
Bit8u old_mousebuttons=0, new_mousebuttons=0;
int old_mousex=0, new_mousex=0;
int old_mousey=0, new_mousey=0;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define SWAP16(X)    (X)
#define SWAP32(X)    (X)
#else
#define SWAP16(X)    SDL_Swap16(X)
#define SWAP32(X)    SDL_Swap32(X)
#endif


void bx_gui_c::specific_init(
    bx_gui_c *th,
    int argc,
    char **argv,
    unsigned x_tilesize,
    unsigned y_tilesize,
    unsigned header_bar_y)
{
  char videodriver[64];
  int i,j;
  Uint32 color, *buf;

  tilewidth = x_tilesize;
  tileheight = y_tilesize;
  
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
  sdl_menuscreen = SDL_CreateRGBSurface(
      SDL_SWSURFACE,
      BX_MENU_WIDTH,
      BX_MENU_HEIGHT,
      32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      0xFF000000,
      0x00FF0000,
      0x0000FF00,
      0x000000FF
#else
      0x000000FF,
      0x0000FF00,
      0x00FF0000,
      0xFF000000
#endif
      );
  if( !sdl_menuscreen )
  {
    LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
    BX_PANIC (("Unable allocate bochs-menu: %s",SDL_GetError()));
  }
    
  for(i=0;i<BX_MENU_HEIGHT;i++)
  {
    buf = (Uint32 *)sdl_menuscreen->pixels + i*sdl_menuscreen->pitch/4;
    for(j=0;j<BX_MENU_WIDTH;j++)
      *buf++ = 0x80FC5454;
  }
  

  sdl_fullscreen = 0;
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
  int hchars,fontrows,fontpixels,x,y;
  int fgcolor_ndx;
  int bgcolor_ndx;
  Uint32 fgcolor;
  Uint32 bgcolor;
  Uint32 *buf, *buf_row, *buf_char;

  buf_row = (Uint32 *)sdl_screen->pixels;
  do
  {
    buf = buf_row;
    hchars = textres_x;
    do
    {
      // check if char needs to be updated
      if( (old_text[0] != new_text[0])
	  || (old_text[1] != new_text[1]) )
      {

	// Get Foreground/Background pixel colors
	fgcolor_ndx = new_text[1] & 0x0F;
	bgcolor_ndx = (new_text[1] >> 4) & 0x07;
	fgcolor = palette[fgcolor_ndx];
	bgcolor = palette[bgcolor_ndx];
	
	// Display this one char
	fontrows = fontheight;
	pfont_row = &font[(new_text[0]*fontheight)];
	buf_char = buf;
	do
	{
	  font_row = *pfont_row++;
	  fontpixels = fontwidth;
	  do
	  {
	    if( (font_row & 0x80) == 0 )
	      *buf = bgcolor;
	    else
	      *buf = fgcolor;
	    buf++;
	    font_row = font_row << 1;
	  } while( --fontpixels );
	  buf -= fontwidth;
	  buf += sdl_screen->pitch/4;
	} while( --fontrows );

	// restore output buffer ptr to start of this char
	buf = buf_char;
      }
      // move to next char location on screen
      buf += fontwidth;
      
      // select next char in old/new text
      new_text+=2;
      old_text+=2;
	
    // process one entire horizontal row
    } while( --hchars );

    // go to next character row location
    buf_row += sdl_screen->pitch/4 * fontheight;
  } while( --rows );
}


void bx_gui_c::graphics_tile_update(
    Bit8u *snapshot,
    unsigned x,
    unsigned y)
{
  Uint32 *buf = (Uint32 *)sdl_screen->pixels + y*sdl_screen->pitch/4 + x;
  Uint32 *buf_row, color;
  int i,j;
  
  for(i=0;i<tileheight;i++)
  {
    buf_row = buf;
    for(j=0;j<16;j++)
    {
      *buf++ = palette[*snapshot++];
    }
    buf = buf_row + sdl_screen->pitch/4;
  }
}


void bx_gui_c::handle_events(void)
{
  Bit32u key_event;

  while( SDL_PollEvent(&sdl_event) )
  {
    switch( sdl_event.type )
    {
      case SDL_VIDEOEXPOSE:
	SDL_UpdateRect( sdl_screen, 0,0, res_x, res_y );
	break;

      case SDL_MOUSEMOTION:
	new_mousebuttons = ((sdl_event.motion.state & 0x01)|((sdl_event.motion.state>>1)&0x02));
	cout << "mouse motion: x=" << sdl_event.motion.xrel << " y=" << sdl_event.motion.yrel << " buttons=" << (int)(new_mousebuttons) <<endl;
	bx_devices.keyboard->mouse_motion(
	    sdl_event.motion.xrel,
	    sdl_event.motion.yrel,
	    new_mousebuttons );
	old_mousebuttons = new_mousebuttons;
	old_mousex = (int)(sdl_event.motion.x);
	old_mousey = (int)(sdl_event.motion.y);
	break;

      case SDL_MOUSEBUTTONDOWN:
	if( (sdl_event.button.button == SDL_BUTTON(2))
	    && (sdl_fullscreen == 0) )
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
	if( sdl_fullscreen == 0 )
	  new_mousebuttons &= 0x03;
	// send motion information
	bx_devices.keyboard->mouse_motion(
	    new_mousex - old_mousex,
	    new_mousey - old_mousey,
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
	  SDL_WM_ToggleFullScreen( sdl_screen );
	  sdl_fullscreen = ~sdl_fullscreen;
	  if( sdl_fullscreen == 0 )
	  {
	    SDL_ShowCursor( 1 );
	    SDL_WM_GrabInput( SDL_GRAB_OFF );
	  }
	  else
	  {
	    SDL_ShowCursor( 0 );
	    SDL_WM_GrabInput( SDL_GRAB_ON );
	  }
	  break;
	}

	// convert scancode->bochs code
	if( sdl_event.key.keysym.scancode > _SCN2BX_LAST_ ) break;
	key_event = scancodes2bx[ sdl_event.key.keysym.scancode-8 ][1];
	if( key_event == 0 ) break;
	bx_devices.keyboard->gen_scancode( key_event );
	break;

      case SDL_KEYUP:

	// filter out release of Windows/Fullscreen toggle and unsupported keys
	if( (sdl_event.key.keysym.sym != SDLK_SCROLLOCK)
	    && (sdl_event.key.keysym.scancode < _SCN2BX_LAST_ ))
	{
	  // convert scancode->bochs code
	  key_event = scancodes2bx[ sdl_event.key.keysym.scancode-8 ][1];
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
  SDL_UpdateRect( sdl_screen,0,0,res_x,res_y );
}


void bx_gui_c::clear_screen(void)
{
  int i = res_y, j;
  Uint32 color;
  Uint32 *buf;

  color = SDL_MapRGB( sdl_screen->format, 0,0,0 );
  while( i-- )
  {
    buf = (Uint32 *)sdl_screen->pixels + i*sdl_screen->pitch/4;
    j = res_x;
    while( j ) buf[--j] = color;
  }
  SDL_UpdateRect(sdl_screen,0,0,res_x,res_y);
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

  palette[index] = SDL_MapRGB( sdl_screen->format, palred, palgreen, palblue );
  return 1;
}


void bx_gui_c::dimension_update(
    unsigned x,
    unsigned y)
{
  if( (x == res_x) && (y == res_y )) return;

  if( sdl_screen )
    SDL_FreeSurface( sdl_screen );
  sdl_screen = SDL_SetVideoMode( x, y, 32, SDL_SWSURFACE );
  if( !sdl_screen )
  {
    LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
    BX_PANIC (("Unable to set requested videomode: %ix%i: %s",x,y,SDL_GetError()));
  }
  res_x = x;
  res_y = y;
  textres_x = x / fontwidth;
  textres_y = y / fontheight;
}


unsigned bx_gui_c::create_bitmap(
    const unsigned char *bmap,
    unsigned xdim,
    unsigned ydim)
{
  cout << "sdl: create bitmap" <<endl;
  return 0;
}


unsigned bx_gui_c::headerbar_bitmap(
    unsigned bmap_id,
    unsigned alignment,
    void (*f)(void))
{
  cout << "sdl: headerbar bitmap" <<endl;
  return 0;
}


void bx_gui_c::replace_bitmap(
    unsigned hbar_id,
    unsigned bmap_id)
{
  cout << "sdl: replace bitmap" << endl;
}


void bx_gui_c::show_headerbar(void)
{
  cout << "sdl: show headerbar" << endl;
}


void update_floppy_status_buttons (void)
{
  cout << "sdl: update floppy status buttons" <<endl;
}


void bx_gui_c::mouse_enabled_changed_specific (Boolean val)
{
  cout << "sdl: mouse enabled changed specific" <<endl;
}


void bx_gui_c::exit(void)
{
  if( sdl_screen )
    SDL_FreeSurface(sdl_screen);
  if( sdl_menuscreen )
    SDL_FreeSurface(sdl_menuscreen);
}


void init_signal_handlers ()
{
  cout << "sdl: init signal handlers" <<endl;
}


void key_event(Bit32u key)
{
  cout << "sdl: key event" << endl;
}


void init(
    int argc,
    char **argv,
    unsigned x_tilesize,
    unsigned y_tilesize)
{
  cout << "sdl: init" <<endl;
}


void sim_is_idle(void)
{
  cout << "sdl: sim is idle" <<endl;
}


Bit32u get_sighandler_mask ()
{
  cout << "sdl: get sighandler mask" <<endl;
}


void sighandler (int sig)
{
  cout << "sdl: sighandler" <<endl;
}

