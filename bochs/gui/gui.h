/////////////////////////////////////////////////////////////////////////
// $Id: gui.h,v 1.24 2002-03-15 16:45:09 vruppert Exp $
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


class bx_gui_c : public logfunctions {
public:
  bx_gui_c (void);
  // Define the following functions in the module for your
  // particular GUI (x.cc, beos.cc, ...)
  static void specific_init(bx_gui_c *th, int argc, char **argv,
                 unsigned x_tilesize, unsigned y_tilesize, unsigned header_bar_y);
  static void text_update(Bit8u *old_text, Bit8u *new_text,
                          unsigned long cursor_x, unsigned long cursor_y,
                          Bit16u cursor_state, unsigned rows);
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
  static void set_clipboard_text(char *snapshot, Bit32u len);

  // The following function(s) are defined already, and your
  // GUI code calls them
  static void key_event(Bit32u key);

  static void init(int argc, char **argv,
                 unsigned x_tilesize, unsigned y_tilesize);
  void update_floppy_status_buttons (void);
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
#if BX_USE_IDLE_HACK
  static void sim_is_idle(void);
#endif


private:
  // And these are defined and used privately in gui.cc
  static void floppyA_handler(void);
  static void floppyB_handler(void);
  static void cdromD_handler(void);
  static void reset_handler(void);
  static void power_handler(void);
  static void copy_handler(void);
  static void paste_handler(void);
  static void snapshot_handler(void);
  static void config_handler(void);
  static void toggle_mouse_enable(void);
  static Bit32s make_text_snapshot (char **snapshot, Bit32u *length);

  Boolean floppyA_status;
  Boolean floppyB_status;
  Boolean cdromD_status;
  unsigned floppyA_bmap_id, floppyA_eject_bmap_id, floppyA_hbar_id;
  unsigned floppyB_bmap_id, floppyB_eject_bmap_id, floppyB_hbar_id;
  unsigned cdromD_bmap_id, cdromD_eject_bmap_id, cdromD_hbar_id;
  unsigned power_bmap_id,    power_hbar_id;
  unsigned reset_bmap_id,    reset_hbar_id;
  unsigned copy_bmap_id, copy_hbar_id;
  unsigned paste_bmap_id, paste_hbar_id;
  unsigned snapshot_bmap_id, snapshot_hbar_id;
  unsigned config_bmap_id, config_hbar_id;
  unsigned mouse_bmap_id, nomouse_bmap_id, mouse_hbar_id;
  };


#define BX_MAX_PIXMAPS 16
#define BX_MAX_HEADERBAR_ENTRIES 10
#define BX_HEADER_BAR_Y 32

// align pixmaps towards left or right side of header bar
#define BX_GRAVITY_LEFT 10
#define BX_GRAVITY_RIGHT 11

#define BX_KEY_PRESSED  0x00000000
#define BX_KEY_RELEASED 0x80000000

#define BX_KEY_UNHANDLED 0x10000000

#define BX_KEY_CTRL_L   0
#define BX_KEY_SHIFT_L  1

#define BX_KEY_F1     2
#define BX_KEY_F2     3
#define BX_KEY_F3     4
#define BX_KEY_F4     5
#define BX_KEY_F5     6
#define BX_KEY_F6     7
#define BX_KEY_F7     8
#define BX_KEY_F8     9
#define BX_KEY_F9    10
#define BX_KEY_F10   11
#define BX_KEY_F11   12
#define BX_KEY_F12   13

#define BX_KEY_CTRL_R    14
#define BX_KEY_SHIFT_R   15
#define BX_KEY_CAPS_LOCK 16
#define BX_KEY_NUM_LOCK  17
#define BX_KEY_ALT_L     18
#define BX_KEY_ALT_R     19

#define BX_KEY_A     20
#define BX_KEY_B     21
#define BX_KEY_C     22
#define BX_KEY_D     23
#define BX_KEY_E     24
#define BX_KEY_F     25
#define BX_KEY_G     26
#define BX_KEY_H     27
#define BX_KEY_I     28
#define BX_KEY_J     29
#define BX_KEY_K     30
#define BX_KEY_L     31
#define BX_KEY_M     32
#define BX_KEY_N     33
#define BX_KEY_O     34
#define BX_KEY_P     35
#define BX_KEY_Q     36
#define BX_KEY_R     37
#define BX_KEY_S     38
#define BX_KEY_T     39
#define BX_KEY_U     40
#define BX_KEY_V     41
#define BX_KEY_W     42
#define BX_KEY_X     43
#define BX_KEY_Y     44
#define BX_KEY_Z     45

#define BX_KEY_0     46
#define BX_KEY_1     47
#define BX_KEY_2     48
#define BX_KEY_3     49
#define BX_KEY_4     50
#define BX_KEY_5     51
#define BX_KEY_6     52
#define BX_KEY_7     53
#define BX_KEY_8     54
#define BX_KEY_9     55

#define BX_KEY_ESC    56

#define BX_KEY_SPACE         57
#define BX_KEY_SINGLE_QUOTE  58
#define BX_KEY_COMMA         59
#define BX_KEY_PERIOD        60
#define BX_KEY_SLASH         61

#define BX_KEY_SEMICOLON     62
#define BX_KEY_EQUALS        63

#define BX_KEY_LEFT_BRACKET  64
#define BX_KEY_BACKSLASH     65
#define BX_KEY_RIGHT_BRACKET 66
#define BX_KEY_MINUS         67
#define BX_KEY_GRAVE         68

#define BX_KEY_BACKSPACE     69
#define BX_KEY_ENTER         70
#define BX_KEY_TAB           71

#define BX_KEY_LEFT_BACKSLASH 72
#define BX_KEY_PRINT         73
#define BX_KEY_SCRL_LOCK     74
#define BX_KEY_PAUSE         75

#define BX_KEY_INSERT        76
#define BX_KEY_DELETE        77
#define BX_KEY_HOME          78
#define BX_KEY_END           79
#define BX_KEY_PAGE_UP       80
#define BX_KEY_PAGE_DOWN     81

#define BX_KEY_KP_ADD        82
#define BX_KEY_KP_SUBTRACT   83
#define BX_KEY_KP_END        84
#define BX_KEY_KP_DOWN       85
#define BX_KEY_KP_PAGE_DOWN  86
#define BX_KEY_KP_LEFT       87
#define BX_KEY_KP_RIGHT      88
#define BX_KEY_KP_HOME       89
#define BX_KEY_KP_UP         90
#define BX_KEY_KP_PAGE_UP    91
#define BX_KEY_KP_INSERT     92
#define BX_KEY_KP_DELETE     93
#define BX_KEY_KP_5          94

#define BX_KEY_UP            95
#define BX_KEY_DOWN          96
#define BX_KEY_LEFT          97
#define BX_KEY_RIGHT         98

#define BX_KEY_KP_ENTER      99
#define BX_KEY_KP_MULTIPLY  100
#define BX_KEY_KP_DIVIDE    101

#define BX_KEY_WIN_L        102
#define BX_KEY_WIN_R        103
#define BX_KEY_MENU         104

#define BX_KEY_ALT_SYSREQ   105
#define BX_KEY_CTRL_BREAK   106

#define BX_KEY_INT_BACK     107
#define BX_KEY_INT_FORWARD  108
#define BX_KEY_INT_STOP     109
#define BX_KEY_INT_MAIL     110
#define BX_KEY_INT_SEARCH   111
#define BX_KEY_INT_FAV      112
#define BX_KEY_INT_HOME     113

#define BX_KEY_POWER_MYCOMP 114
#define BX_KEY_POWER_CALC   115
#define BX_KEY_POWER_SLEEP  116
#define BX_KEY_POWER_POWER  117
#define BX_KEY_POWER_WAKE   118

#define BX_KEY_NBKEYS       119
// If you add BX_KEYs Please update 
// - BX_KEY_NBKEYS
// - the scancodes table in the file iodev/scancodes.cc
// - the bx_key_symbol table in the file gui/keymap.cc
