/////////////////////////////////////////////////////////////////////////
// $Id: term.cc,v 1.29 2003-05-18 11:26:02 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000  MandrakeSoft S.A.
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



// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#if BX_WITH_TERM

#include "icon_bochs.h"

extern "C" {
#include <curses.h>
#include <signal.h>
};

class bx_term_gui_c : public bx_gui_c {
public:
  bx_term_gui_c (void) {}
  DECLARE_GUI_VIRTUAL_METHODS()

  virtual Bit32u get_sighandler_mask ();
  // called when registered signal arrives
  virtual void sighandler (int sig);
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_term_gui_c *theGui = NULL;
IMPLEMENT_GUI_PLUGIN_CODE(term)

#define LOG_THIS theGui->

bx_bool initialized = 0;
static unsigned int text_cols = 80, text_rows = 25;

static short curses_color[8] = {
  /* 0 */ COLOR_BLACK,
  /* 1 */ COLOR_BLUE,
  /* 2 */ COLOR_GREEN,
  /* 3 */ COLOR_CYAN,
  /* 4 */ COLOR_RED,
  /* 5 */ COLOR_MAGENTA,
  /* 6 */ COLOR_YELLOW,
  /* 7 */ COLOR_WHITE
};

static chtype vga_to_term[128] = {
  0xc7, 0xfc, 0xe9, 0xe2, 0xe4, 0xe0, 0xe5, 0xe7,
  0xea, 0xeb, 0xe8, 0xef, 0xee, 0xec, 0xc4, 0xc5,
  0xc9, 0xe6, 0xc6, 0xf4, 0xf6, 0xf2, 0xfb, 0xf9,
  0xff, 0xd6, 0xdc, 0xe7, 0xa3, 0xa5, ' ',  ' ',
  0xe1, 0xed, 0xf3, 0xfa, 0xf1, 0xd1, 0xaa, 0xba,
  0xbf, ' ',  0xac, ' ',  ' ',  0xa1, 0xab, 0xbb,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, ' ',  ' ',  ' ',  ' ',
  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  0xb5, ' ',
  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',
  ' ',  0xb1, ' ',  ' ',  ' ',  ' ',  0xf7, ' ',
  0xb0, ' ',  ' ',  ' ',  ' ',  0xb2, ' ',  ' '
};

static void
do_scan(int key_event, int shift, int ctrl, int alt)
{
	/* XXX At some point, cache alt/ctrl/shift so only when the state
		 changes do we simulate a press or release, to cut down on
		 keyboard input to the simulated machine */

	BX_DEBUG(("key_event %d/0x%x %s%s%s",
		  key_event,key_event,
		  shift?"(shift)":"",
		  ctrl?"(ctrl)":"",
		  alt?"(alt)":""));
	if(shift)
		DEV_kbd_gen_scancode(BX_KEY_SHIFT_L);
	if(ctrl)
		DEV_kbd_gen_scancode(BX_KEY_CTRL_L);
	if(alt)
		DEV_kbd_gen_scancode(BX_KEY_ALT_L);
	DEV_kbd_gen_scancode(key_event);
	key_event |= BX_KEY_RELEASED;

	DEV_kbd_gen_scancode(key_event);
	if(alt)
		DEV_kbd_gen_scancode(BX_KEY_ALT_L|BX_KEY_RELEASED);
	if(ctrl)
		DEV_kbd_gen_scancode(BX_KEY_CTRL_L|BX_KEY_RELEASED);
	if(shift)
		DEV_kbd_gen_scancode(BX_KEY_SHIFT_L|BX_KEY_RELEASED);
}

Bit32u 
bx_term_gui_c::get_sighandler_mask ()
{
  return 
    (1<<SIGHUP)
    | (1<<SIGINT)
    | (1<<SIGQUIT)
#ifdef SIGSTOP
    | (1<<SIGSTOP)
#endif
#ifdef SIGTSTP
    | (1<<SIGTSTP)
#endif
    | (1<<SIGTERM);
}

void
bx_term_gui_c::sighandler(int signo)
{
	switch(signo) {
	case SIGINT:
		do_scan(BX_KEY_C,0,1,0);
		break;
#ifdef SIGSTOP
	case SIGSTOP:
		do_scan(BX_KEY_S,0,1,0);
		break;
#endif
#ifdef SIGTSTP
	case SIGTSTP:
		do_scan(BX_KEY_Z,0,1,0);
		break;
#endif
	default:
		BX_INFO(("sig %d caught",signo));
		break;
	}
}

// ::SPECIFIC_INIT()
//
// Called from gui.cc, once upon program startup, to allow for the
// specific GUI code (X11, BeOS, ...) to be initialized.
//
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

	void
bx_term_gui_c::specific_init(int argc, char **argv, unsigned tilewidth, unsigned tileheight,
	unsigned headerbar_y)
{
	put("TGUI");
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(tilewidth);
	UNUSED(tileheight);
	UNUSED(headerbar_y);

	UNUSED(bochs_icon_bits);  // global variable

	// the ask menu causes trouble
	io->set_log_action(LOGLEV_PANIC, ACT_FATAL);
	// logfile should be different from stderr, otherwise terminal mode
	// really ends up having fun
	if (!strcmp(bx_options.log.Ofilename->getptr(), "-"))
		BX_PANIC(("cannot log to stderr in term mode"));

	initscr();
	start_color();
	cbreak();
	curs_set(2);
	keypad(stdscr,TRUE);
	nodelay(stdscr, TRUE);
	noecho();

#if BX_HAVE_COLOR_SET
	if (has_colors()) {
		for (int i=0; i<COLORS; i++) {
			for (int j=0; j<COLORS; j++) {
				if ((i!=0)||(j!=0)) init_pair(i * COLORS + j, j, i);
			}
		}
	}
#endif

	if (bx_options.Oprivate_colormap->get ())
		BX_ERROR(("WARNING: private_colormap option ignored."));
	initialized = 1;
}



void
do_char(int character,int alt)
{
	switch (character) {
	// control keys
	case   0x9: do_scan(BX_KEY_TAB,0,0,alt); break;
	case   0xa: do_scan(BX_KEY_KP_ENTER,0,0,alt); break;
	case   0xd: do_scan(BX_KEY_KP_DELETE,0,0,alt); break;
	case   0x1: do_scan(BX_KEY_A,0,1,alt); break;
	case   0x2: do_scan(BX_KEY_B,0,1,alt); break;
	case   0x3: do_scan(BX_KEY_C,0,1,alt); break;
	case   0x4: do_scan(BX_KEY_D,0,1,alt); break;
	case   0x5: do_scan(BX_KEY_E,0,1,alt); break;
	case   0x6: do_scan(BX_KEY_F,0,1,alt); break;
	case   0x7: do_scan(BX_KEY_G,0,1,alt); break;
	case   0x8: do_scan(BX_KEY_H,0,1,alt); break;
	case   0xb: do_scan(BX_KEY_K,0,1,alt); break;
	case   0xc: do_scan(BX_KEY_L,0,1,alt); break;
	case   0xe: do_scan(BX_KEY_N,0,1,alt); break;
	case   0xf: do_scan(BX_KEY_O,0,1,alt); break;
	case  0x10: do_scan(BX_KEY_P,0,1,alt); break;
	case  0x11: do_scan(BX_KEY_Q,0,1,alt); break;
	case  0x12: do_scan(BX_KEY_R,0,1,alt); break;
	case  0x13: do_scan(BX_KEY_S,0,1,alt); break;
	case  0x14: do_scan(BX_KEY_T,0,1,alt); break;
	case  0x15: do_scan(BX_KEY_U,0,1,alt); break;
	case  0x16: do_scan(BX_KEY_V,0,1,alt); break;
	case  0x17: do_scan(BX_KEY_W,0,1,alt); break;
	case  0x18: do_scan(BX_KEY_X,0,1,alt); break;
	case  0x19: do_scan(BX_KEY_Y,0,1,alt); break;
	case  0x1a: do_scan(BX_KEY_Z,0,1,alt); break;
	case  0x20: do_scan(BX_KEY_SPACE,0,0,alt); break;
	case 0x107: do_scan(BX_KEY_BACKSPACE,0,0,alt); break;
	case 0x102: do_scan(BX_KEY_DOWN,0,0,alt); break;
	case 0x103: do_scan(BX_KEY_UP,0,0,alt); break;
	case 0x104: do_scan(BX_KEY_LEFT,0,0,alt); break;
	case 0x105: do_scan(BX_KEY_RIGHT,0,0,alt); break;
	case 0x152: do_scan(BX_KEY_PAGE_DOWN,0,0,alt); break;
	case 0x153: do_scan(BX_KEY_PAGE_UP,0,0,alt); break;
	case 0x106: do_scan(BX_KEY_HOME,0,0,alt); break;
	case 0x168: do_scan(BX_KEY_END,0,0,alt); break;
	case 0x14b: do_scan(BX_KEY_INSERT,0,0,alt); break;
	case 0x7f:  do_scan(BX_KEY_DELETE,0,0,alt); break;
	case 0x1b:  do_scan(BX_KEY_ESC,0,0,alt); break;
	case '!': do_scan(BX_KEY_1,1,0,alt); break;
	case '\'': do_scan(BX_KEY_SINGLE_QUOTE,0,0,alt); break;
	case '#': do_scan(BX_KEY_3,1,0,alt); break;
	case '$': do_scan(BX_KEY_4,1,0,alt); break;
	case '%': do_scan(BX_KEY_5,1,0,alt); break;
	case '^': do_scan(BX_KEY_6,1,0,alt); break;
	case '&': do_scan(BX_KEY_7,1,0,alt); break;
	case '"': do_scan(BX_KEY_SINGLE_QUOTE,1,0,alt); break;

	// ()*+,-./
	case '(': do_scan(BX_KEY_9,1,0,alt); break;
	case ')': do_scan(BX_KEY_0,1,0,alt); break;
	case '*': do_scan(BX_KEY_8,1,0,alt); break;
	case '+': do_scan(BX_KEY_EQUALS,1,0,alt); break;
	case ',': do_scan(BX_KEY_COMMA,0,0,alt); break;
	case '-': do_scan(BX_KEY_MINUS,0,0,alt); break;
	case '.': do_scan(BX_KEY_PERIOD,0,0,alt); break;
	case '/': do_scan(BX_KEY_SLASH,0,0,alt); break;

	// 01234567
	case '0': do_scan(BX_KEY_0,0,0,alt); break;
	case '1': do_scan(BX_KEY_1,0,0,alt); break;
	case '2': do_scan(BX_KEY_2,0,0,alt); break;
	case '3': do_scan(BX_KEY_3,0,0,alt); break;
	case '4': do_scan(BX_KEY_4,0,0,alt); break;
	case '5': do_scan(BX_KEY_5,0,0,alt); break;
	case '6': do_scan(BX_KEY_6,0,0,alt); break;
	case '7': do_scan(BX_KEY_7,0,0,alt); break;

	// 89:;<=>?
	case '8': do_scan(BX_KEY_8,0,0,alt); break;
	case '9': do_scan(BX_KEY_9,0,0,alt); break;
	case ':': do_scan(BX_KEY_SEMICOLON,1,0,alt); break;
	case ';': do_scan(BX_KEY_SEMICOLON,0,0,alt); break;
	case '<': do_scan(BX_KEY_COMMA,1,0,alt); break;
	case '=': do_scan(BX_KEY_EQUALS,0,0,alt); break;
	case '>': do_scan(BX_KEY_PERIOD,1,0,alt); break;
	case '?': do_scan(BX_KEY_SLASH,1,0,alt); break;

	// @ABCDEFG
	case '@': do_scan(BX_KEY_2,1,0,alt); break;
	case 'A': do_scan(BX_KEY_A,1,0,alt); break;
	case 'B': do_scan(BX_KEY_B,1,0,alt); break;
	case 'C': do_scan(BX_KEY_C,1,0,alt); break;
	case 'D': do_scan(BX_KEY_D,1,0,alt); break;
	case 'E': do_scan(BX_KEY_E,1,0,alt); break;
	case 'F': do_scan(BX_KEY_F,1,0,alt); break;
	case 'G': do_scan(BX_KEY_G,1,0,alt); break;


	// HIJKLMNO
	case 'H': do_scan(BX_KEY_H,1,0,alt); break;
	case 'I': do_scan(BX_KEY_I,1,0,alt); break;
	case 'J': do_scan(BX_KEY_J,1,0,alt); break;
	case 'K': do_scan(BX_KEY_K,1,0,alt); break;
	case 'L': do_scan(BX_KEY_L,1,0,alt); break;
	case 'M': do_scan(BX_KEY_M,1,0,alt); break;
	case 'N': do_scan(BX_KEY_N,1,0,alt); break;
	case 'O': do_scan(BX_KEY_O,1,0,alt); break;


	// PQRSTUVW
	case 'P': do_scan(BX_KEY_P,1,0,alt); break;
	case 'Q': do_scan(BX_KEY_Q,1,0,alt); break;
	case 'R': do_scan(BX_KEY_R,1,0,alt); break;
	case 'S': do_scan(BX_KEY_S,1,0,alt); break;
	case 'T': do_scan(BX_KEY_T,1,0,alt); break;
	case 'U': do_scan(BX_KEY_U,1,0,alt); break;
	case 'V': do_scan(BX_KEY_V,1,0,alt); break;
	case 'W': do_scan(BX_KEY_W,1,0,alt); break;

	// XYZ[\]^_
	case 'X': do_scan(BX_KEY_X,1,0,alt); break;
	case 'Y': do_scan(BX_KEY_Y,1,0,alt); break;
	case 'Z': do_scan(BX_KEY_Z,1,0,alt); break;
	case '{': do_scan(BX_KEY_LEFT_BRACKET,1,0,alt); break;
	case '|': do_scan(BX_KEY_BACKSLASH,1,0,alt); break;
	case '}': do_scan(BX_KEY_RIGHT_BRACKET,1,0,alt); break;
	case '_': do_scan(BX_KEY_MINUS,1,0,alt); break;

	// `abcdefg
	case '`': do_scan(BX_KEY_GRAVE,0,0,alt); break;
	case 'a': do_scan(BX_KEY_A,0,0,alt); break;
	case 'b': do_scan(BX_KEY_B,0,0,alt); break;
	case 'c': do_scan(BX_KEY_C,0,0,alt); break;
	case 'd': do_scan(BX_KEY_D,0,0,alt); break;
	case 'e': do_scan(BX_KEY_E,0,0,alt); break;
	case 'f': do_scan(BX_KEY_F,0,0,alt); break;
	case 'g': do_scan(BX_KEY_G,0,0,alt); break;

	// hijklmno
	case 'h': do_scan(BX_KEY_H,0,0,alt); break;
	case 'i': do_scan(BX_KEY_I,0,0,alt); break;
	case 'j': do_scan(BX_KEY_J,0,0,alt); break;
	case 'k': do_scan(BX_KEY_K,0,0,alt); break;
	case 'l': do_scan(BX_KEY_L,0,0,alt); break;
	case 'm': do_scan(BX_KEY_M,0,0,alt); break;
	case 'n': do_scan(BX_KEY_N,0,0,alt); break;
	case 'o': do_scan(BX_KEY_O,0,0,alt); break;

	// pqrstuvw
	case 'p': do_scan(BX_KEY_P,0,0,alt); break;
	case 'q': do_scan(BX_KEY_Q,0,0,alt); break;
	case 'r': do_scan(BX_KEY_R,0,0,alt); break;
	case 's': do_scan(BX_KEY_S,0,0,alt); break;
	case 't': do_scan(BX_KEY_T,0,0,alt); break;
	case 'u': do_scan(BX_KEY_U,0,0,alt); break;
	case 'v': do_scan(BX_KEY_V,0,0,alt); break;
	case 'w': do_scan(BX_KEY_W,0,0,alt); break;

	// xyz{|}~
	case 'x': do_scan(BX_KEY_X,0,0,alt); break;
	case 'y': do_scan(BX_KEY_Y,0,0,alt); break;
	case 'z': do_scan(BX_KEY_Z,0,0,alt); break;
	case '[': do_scan(BX_KEY_LEFT_BRACKET,0,0,alt); break;
	case '\\': do_scan(BX_KEY_BACKSLASH,0,0,alt); break;
	case ']': do_scan(BX_KEY_RIGHT_BRACKET,0,0,alt); break;
	case '~': do_scan(BX_KEY_GRAVE,1,0,alt); break;

	// function keys
	case KEY_F(1): do_scan(BX_KEY_F1,0,0,alt); break;
	case KEY_F(2): do_scan(BX_KEY_F2,0,0,alt); break;
	case KEY_F(3): do_scan(BX_KEY_F3,0,0,alt); break;
	case KEY_F(4): do_scan(BX_KEY_F4,0,0,alt); break;
	case KEY_F(5): do_scan(BX_KEY_F5,0,0,alt); break;
	case KEY_F(6): do_scan(BX_KEY_F6,0,0,alt); break;
	case KEY_F(7): do_scan(BX_KEY_F7,0,0,alt); break;
	case KEY_F(8): do_scan(BX_KEY_F8,0,0,alt); break;
	case KEY_F(9): do_scan(BX_KEY_F9,0,0,alt); break;
	case KEY_F(10): do_scan(BX_KEY_F10,0,0,alt); break;
	case KEY_F(11): do_scan(BX_KEY_F11,0,0,alt); break;
	case KEY_F(12): do_scan(BX_KEY_F12,0,0,alt); break;

	// shifted function keys
	case KEY_F(13): do_scan(BX_KEY_F1,1,0,alt); break;
	case KEY_F(14): do_scan(BX_KEY_F2,1,0,alt); break;
	case KEY_F(15): do_scan(BX_KEY_F3,1,0,alt); break;
	case KEY_F(16): do_scan(BX_KEY_F4,1,0,alt); break;
	case KEY_F(17): do_scan(BX_KEY_F5,1,0,alt); break;
	case KEY_F(18): do_scan(BX_KEY_F6,1,0,alt); break;
	case KEY_F(19): do_scan(BX_KEY_F7,1,0,alt); break;
	case KEY_F(20): do_scan(BX_KEY_F8,1,0,alt); break;

	default:
		if(character > 0x79) {
			do_char(character - 0x80,1);
			break;
		}

		BX_INFO(("character unhandled: 0x%x",character));
		break;
	}
}


// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

	void
bx_term_gui_c::handle_events(void)
{
	int character;
	while((character = getch()) != ERR) {
		BX_DEBUG(("scancode(0x%x)",character));
		do_char(character,0);
	}
}



// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

	void
bx_term_gui_c::flush(void)
{
	if (initialized)
	  refresh();
}


// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.  Don't
// clear the area that defines the headerbar.

	void
bx_term_gui_c::clear_screen(void)
{
  clear();
  color_set(7, NULL);
  if (LINES > (int)text_rows) {
    mvhline(text_rows, 0, ACS_HLINE, text_cols);
  }
  if (COLS > (int)text_cols) {
    mvvline(0, text_cols, ACS_VLINE, text_rows);
  }
  if ((LINES > (int)text_rows) && (COLS > (int)text_cols)) {
    mvaddch(text_rows, text_cols, ACS_LRCORNER);
  }
}

int
get_color_pair(Bit8u vga_attr)
{
	int term_attr;

	term_attr = curses_color[vga_attr & 0x07];
	term_attr |= (curses_color[(vga_attr & 0x70) >> 4] << 3);
	return term_attr;
}

chtype
get_term_char(Bit8u vga_char[])
{
  int term_char;

  if ((vga_char[1] & 0x0f) == ((vga_char[1] >> 4) & 0x0f)) {
    return ' ';
  }
  switch (vga_char[0]) {
    case 0x04: term_char = ACS_DIAMOND; break;
    case 0x18: term_char = ACS_UARROW; break;
    case 0x19: term_char = ACS_DARROW; break;
    case 0x1a: term_char = ACS_RARROW; break;
    case 0x1b: term_char = ACS_LARROW; break;
    case 0xc4:
    case 0xcd: term_char = ACS_HLINE; break;
    case 0xb3:
    case 0xba: term_char = ACS_VLINE; break;
    case 0xc9:
    case 0xd5:
    case 0xd6:
    case 0xda: term_char = ACS_ULCORNER; break;
    case 0xb7:
    case 0xb8:
    case 0xbb:
    case 0xbf: term_char = ACS_URCORNER; break;
    case 0xc0:
    case 0xc8:
    case 0xd3:
    case 0xd4: term_char = ACS_LLCORNER; break;
    case 0xbc:
    case 0xbd:
    case 0xbe:
    case 0xd9: term_char = ACS_LRCORNER; break;
    case 0xc3:
    case 0xc6:
    case 0xc7:
    case 0xcc: term_char = ACS_LTEE; break;
    case 0xb4:
    case 0xb5:
    case 0xb6:
    case 0xb9: term_char = ACS_RTEE; break;
    case 0xc2:
    case 0xcb:
    case 0xd1:
    case 0xd2: term_char = ACS_TTEE; break;
    case 0xc1:
    case 0xca:
    case 0xcf:
    case 0xd0: term_char = ACS_BTEE; break;
    case 0xc5:
    case 0xce:
    case 0xd7:
    case 0xd8: term_char = ACS_PLUS; break;
    case 0xb0:
    case 0xb1: term_char = ACS_CKBOARD; break;
    case 0xb2: term_char = ACS_BOARD; break;
    case 0xdb: term_char = ACS_BLOCK; break;
    default:
      if (vga_char[0] > 0x7f) {
        term_char = vga_to_term[vga_char[0]-0x80];
      } else if (vga_char[0] > 0x1f) {
        term_char = vga_char[0];
      } else {
        term_char = ' ';
      }
  }
  return term_char;
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

	void
bx_term_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
	unsigned long cursor_x, unsigned long cursor_y,
	bx_vga_tminfo_t tm_info, unsigned nrows)
{
  unsigned char *old_line, *new_line, *new_start;
  unsigned char cAttr;
  unsigned int hchars, rows, x, y;
  chtype ch;
  bx_bool force_update = 0;

  UNUSED(nrows);

  if(charmap_updated) {
    force_update = 1;
    charmap_updated = 0;
  }

  new_start = new_text;
  rows = text_rows;
  y = 0;
  do {
    hchars = text_cols;
    new_line = new_text;
    old_line = old_text;
    x = 0;
    do {
      if (force_update || (old_text[0] != new_text[0])
          || (old_text[1] != new_text[1])) {
#if BX_HAVE_COLOR_SET
        if (has_colors()) {
          color_set(get_color_pair(new_text[1]), NULL);
        }
#endif
        ch = get_term_char(&new_text[0]);
        if ((new_text[1] & 0x08) > 0) ch |= A_BOLD;
        if ((new_text[1] & 0x80) > 0) ch |= A_BLINK;
        mvaddch(y, x, ch);
      }
      x++;
      new_text+=2;
      old_text+=2;
    } while (--hchars);
    y++;
    new_text = new_line + tm_info.line_offset;
    old_text = old_line + tm_info.line_offset;
  } while (--rows);

  if ((cursor_x<text_cols) && (cursor_y<text_rows)
      && (tm_info.cs_start <= tm_info.cs_end)) {
    if(cursor_x>0)
      cursor_x--;
    else {
      cursor_x=COLS-1;
      cursor_y--;
    }
    cAttr = new_start[cursor_y*tm_info.line_offset+cursor_x*2+1];
#if BX_HAVE_COLOR_SET
    if (has_colors()) {
      color_set(get_color_pair(cAttr), NULL);
    }
#endif
    ch = get_term_char(&new_start[cursor_y*tm_info.line_offset+cursor_x*2]);
    if ((cAttr & 0x08) > 0) ch |= A_BOLD;
    if ((cAttr & 0x80) > 0) ch |= A_REVERSE;
    mvaddch(cursor_y, cursor_x, ch);
    curs_set(2);
  } else {
    curs_set(0);
  }
}

  int
bx_term_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  return 0;
}

  int
bx_term_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  return 0;
}


// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

	bx_bool
bx_term_gui_c::palette_change(unsigned index, unsigned red, unsigned green, unsigned blue)
{
	BX_DEBUG(("color pallete request (%d,%d,%d,%d) ignored",
		  index,red,green,blue));
	return(0);
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

	void
bx_term_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
	UNUSED(tile);
	UNUSED(x0);
	UNUSED(y0);
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
bx_term_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth)
{
  if (fheight > 0) {
    text_cols = x / fwidth;
    text_rows = y / fheight;
    color_set(7, NULL);
    if (LINES > (int)text_rows) {
      mvhline(text_rows, 0, ACS_HLINE, text_cols);
    }
    if (COLS > (int)text_cols) {
      mvvline(0, text_cols, ACS_VLINE, text_rows);
    }
    if ((LINES > (int)text_rows) && (COLS > (int)text_cols)) {
      mvaddch(text_rows, text_cols, ACS_LRCORNER);
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

	unsigned
bx_term_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
	UNUSED(bmap);
	UNUSED(xdim);
	UNUSED(ydim);
	return(0);
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

	unsigned
bx_term_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
	UNUSED(bmap_id);
	UNUSED(alignment);
	UNUSED(f);
	return(0);
}


// ::SHOW_HEADERBAR()
//
// Show (redraw) the current headerbar, which is composed of
// currently installed bitmaps.

	void
bx_term_gui_c::show_headerbar(void)
{
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

	void
bx_term_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
	UNUSED(hbar_id);
	UNUSED(bmap_id);
}


// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.

	void
bx_term_gui_c::exit(void)
{
	if (!initialized) return;
	clear();
	flush();
	endwin();
	BX_DEBUG(("exiting"));
}

  void
bx_term_gui_c::mouse_enabled_changed_specific (bx_bool val)
{
}
#endif /* if BX_WITH_TERM */
