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


class bx_gui_c : public logfunctions {
public:
  bx_gui_c (void);
  // Define the following functions in the module for your
  // particular GUI (x.cc, beos.cc, ...)
  static void specific_init(bx_gui_c *th, int argc, char **argv,
                 unsigned x_tilesize, unsigned y_tilesize, unsigned header_bar_y);
  static void text_update(Bit8u *old_text, Bit8u *new_text,
                          unsigned long cursor_x, unsigned long cursor_y,
                          unsigned rows);
  static void graphics_update(Bit8u *snapshot);
  static void graphics_tile_update(Bit8u *snapshot, unsigned x, unsigned y);
  static void handle_events(void);
  static void flush(void);
  static void clear_screen(void);
  static Boolean palette_change(unsigned index, unsigned red, unsigned green, unsigned blue);
  static void dimension_update(unsigned x, unsigned y);
  static unsigned create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim);
  static unsigned headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void));
  static void replace_bitmap(unsigned hbar_id, unsigned bmap_id);
  static void show_headerbar(void);

  // The following function(s) are defined already, and your
  // GUI code calls them
  static void key_event(Bit32u key);

  static void init(int argc, char **argv,
                 unsigned x_tilesize, unsigned y_tilesize);
  static void     mouse_enabled_changed (Boolean val);
  static void     mouse_enabled_changed_specific (Boolean val);
  static void     exit(void);
  static void init_signal_handlers ();
#if BX_GUI_SIGHANDLER
  // returns 32-bit bitmask in which 1 means the GUI should handle that signal
  static Bit32u get_sighandler_mask ();
  // called when registered signal arrives
  static void sighandler (int sig);
#endif


private:
  // And these are defined and used privately in gui.cc
  static void floppyA_handler(void);
  static void floppyB_handler(void);
  static void reset_handler(void);
  static void power_handler(void);
  static void snapshot_handler(void);
  static void toggle_mouse_enable(void);

  Boolean floppyA_status;
  Boolean floppyB_status;
  unsigned floppyA_bmap_id, floppyA_eject_bmap_id, floppyA_hbar_id;
  unsigned floppyB_bmap_id, floppyB_eject_bmap_id, floppyB_hbar_id;
  unsigned power_bmap_id,    power_hbar_id;
  unsigned reset_bmap_id,    reset_hbar_id;
  unsigned snapshot_bmap_id, snapshot_hbar_id;
  unsigned mouse_bmap_id, nomouse_bmap_id, mouse_hbar_id;
  };


#define BX_MAX_PIXMAPS 9
#define BX_MAX_HEADERBAR_ENTRIES 6
#define BX_HEADER_BAR_Y 32

// align pixmaps towards left or right side of header bar
#define BX_GRAVITY_LEFT 10
#define BX_GRAVITY_RIGHT 11

#define BX_KEY_PRESSED  0x00000000
#define BX_KEY_RELEASED 0x80000000

#define BX_KEY_CTRL_L   37
#define BX_KEY_SHIFT_L  50

#define BX_KEY_F1    67
#define BX_KEY_F2    68
#define BX_KEY_F3    69
#define BX_KEY_F4    70
#define BX_KEY_F5    71
#define BX_KEY_F6    72
#define BX_KEY_F7    73
#define BX_KEY_F8    74
#define BX_KEY_F9    75
#define BX_KEY_F10   76
#define BX_KEY_F11   95
#define BX_KEY_F12   96

#define BX_KEY_CTRL_R    109
#define BX_KEY_SHIFT_R   62
#define BX_KEY_CAPS_LOCK 66
#define BX_KEY_NUM_LOCK  77
#define BX_KEY_ALT_L     64
#define BX_KEY_ALT_R     113

#define BX_KEY_A     38
#define BX_KEY_B     56
#define BX_KEY_C     54
#define BX_KEY_D     40
#define BX_KEY_E     26
#define BX_KEY_F     41
#define BX_KEY_G     42
#define BX_KEY_H     43
#define BX_KEY_I     31
#define BX_KEY_J     44
#define BX_KEY_K     45
#define BX_KEY_L     46
#define BX_KEY_M     58
#define BX_KEY_N     57
#define BX_KEY_O     32
#define BX_KEY_P     33
#define BX_KEY_Q     24
#define BX_KEY_R     27
#define BX_KEY_S     39
#define BX_KEY_T     28
#define BX_KEY_U     30
#define BX_KEY_V     55
#define BX_KEY_W     25
#define BX_KEY_X     53
#define BX_KEY_Y     29
#define BX_KEY_Z     52

#define BX_KEY_0     19
#define BX_KEY_1     10
#define BX_KEY_2     11
#define BX_KEY_3     12
#define BX_KEY_4     13
#define BX_KEY_5     14
#define BX_KEY_6     15
#define BX_KEY_7     16
#define BX_KEY_8     17
#define BX_KEY_9     18

#define BX_KEY_ESC    9

#define BX_KEY_SPACE         65
#define BX_KEY_SINGLE_QUOTE  48
#define BX_KEY_COMMA         59
#define BX_KEY_PERIOD        60
#define BX_KEY_SLASH         61

#define BX_KEY_SEMICOLON     47
#define BX_KEY_EQUALS        21

#define BX_KEY_LEFT_BRACKET  34
#define BX_KEY_BACKSLASH     51
#define BX_KEY_LEFT_BACKSLASH  94
#define BX_KEY_RIGHT_BRACKET 35
#define BX_KEY_MINUS         20
#define BX_KEY_GRAVE         49

#define BX_KEY_BACKSPACE     22
#define BX_KEY_ENTER         36
#define BX_KEY_TAB           23

#define BX_KEY_INSERT        106
#define BX_KEY_DELETE        107
#define BX_KEY_HOME          97
#define BX_KEY_END           103
#define BX_KEY_PAGE_UP       99
#define BX_KEY_PAGE_DOWN     105

#define BX_KEY_KP_ADD        86
#define BX_KEY_KP_SUBTRACT   82
#define BX_KEY_KP_END        87
#define BX_KEY_KP_DOWN       88
#define BX_KEY_KP_PAGE_DOWN  89
#define BX_KEY_KP_LEFT       83
#define BX_KEY_KP_RIGHT      85
#define BX_KEY_KP_HOME       79
#define BX_KEY_KP_UP         80
#define BX_KEY_KP_PAGE_UP    81
#define BX_KEY_KP_INSERT     90
#define BX_KEY_KP_DELETE     91
#define BX_KEY_KP_5          84

#define BX_KEY_UP            98
#define BX_KEY_DOWN          104
#define BX_KEY_LEFT          100
#define BX_KEY_RIGHT         102

#define BX_KEY_KP_ENTER     108
#define BX_KEY_KP_MULTIPLY  63
#define BX_KEY_KP_DIVIDE    112
