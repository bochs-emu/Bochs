/////////////////////////////////////////////////////////////////////////
// $Id: gui.h,v 1.38 2003-05-11 15:07:53 vruppert Exp $
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

typedef struct bx_vga_tminfo_t {
  Bit8u cs_start;
  Bit8u cs_end;
  Bit16u line_offset;
  Bit16u line_compare;
  Bit8u h_panning;
  Bit8u v_panning;
  bx_bool line_graphics;
};


BOCHSAPI extern class bx_gui_c *bx_gui;


// The bx_gui_c class provides data and behavior that is common to
// all guis.  Each gui implementation will override the abstract methods.
class BOCHSAPI bx_gui_c : public logfunctions {
public:
  bx_gui_c (void);
  virtual ~bx_gui_c ();
  // Define the following functions in the module for your particular GUI
  // (x.cc, beos.cc, ...)
  virtual void specific_init(int argc, char **argv,
                 unsigned x_tilesize, unsigned y_tilesize, unsigned header_bar_y) = 0;
  virtual void text_update(Bit8u *old_text, Bit8u *new_text,
                          unsigned long cursor_x, unsigned long cursor_y,
                          bx_vga_tminfo_t tm_info, unsigned rows) = 0;
  virtual void graphics_tile_update(Bit8u *snapshot, unsigned x, unsigned y) = 0;
  virtual void handle_events(void) = 0;
  virtual void flush(void) = 0;
  virtual void clear_screen(void) = 0;
  virtual bx_bool palette_change(unsigned index, unsigned red, unsigned green, unsigned blue) = 0;
  virtual void dimension_update(unsigned x, unsigned y, unsigned fheight=0, unsigned fwidth=0) = 0;
  virtual unsigned create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim) = 0;
  virtual unsigned headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void)) = 0;
  virtual void replace_bitmap(unsigned hbar_id, unsigned bmap_id) = 0;
  virtual void show_headerbar(void) = 0;
  virtual int get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)  = 0;
  virtual int set_clipboard_text(char *snapshot, Bit32u len) = 0;
  virtual void mouse_enabled_changed_specific (bx_bool val) = 0;
  virtual void exit(void) = 0;
  // set_display_mode() changes the mode between the configuration interface
  // and the simulation.  This is primarily intended for display libraries
  // which have a full-screen mode such as SDL, term, and svgalib.  The display
  // mode is set to DISP_MODE_CONFIG before displaying any configuration menus,
  // for panics that requires user input, when entering the debugger, etc.  It
  // is set to DISP_MODE_SIM when the Bochs simulation resumes.  The
  // enum is defined in gui/siminterface.h.
  virtual void set_display_mode (disp_mode_t newmode) { /* default=no action*/ }
  // These are only needed for the term gui. For all other guis they will
  // have no effect.
  // returns 32-bit bitmask in which 1 means the GUI should handle that signal
  virtual Bit32u get_sighandler_mask () {return 0;}
  // called when registered signal arrives
  virtual void sighandler (int sig) {}
#if BX_USE_IDLE_HACK
  // this is called from the CPU model when the HLT instruction is executed.
  virtual void sim_is_idle(void) {}
#endif

  // The following function(s) are defined already, and your
  // GUI code calls them
  static void key_event(Bit32u key);
  static void set_text_charmap(Bit8u *fbuffer);
  static void set_text_charbyte(Bit16u address, Bit8u data);

  void init(int argc, char **argv,
                 unsigned x_tilesize, unsigned y_tilesize);
  void update_drive_status_buttons (void);
  static void     mouse_enabled_changed (bx_bool val);
  static void init_signal_handlers ();


protected:
  // And these are defined and used privately in gui.cc
  static void floppyA_handler(void);
  static void floppyB_handler(void);
  static void cdromD_handler(void);
  static void reset_handler(void);
  static void power_handler(void);
  static void copy_handler(void);
  static void paste_handler(void);
  static void snapshot_handler(void);
  static void snapshot_checker(void *);
  static void config_handler(void);
  static void toggle_mouse_enable(void);
  static void userbutton_handler(void);
  static Bit32s make_text_snapshot (char **snapshot, Bit32u *length);

  bx_bool floppyA_status;
  bx_bool floppyB_status;
  bx_bool cdromD_status;
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
  unsigned user_bmap_id, user_hbar_id;

  unsigned char vga_charmap[0x2000];
  bx_bool charmap_updated;
  bx_bool char_changed[256];
  disp_mode_t disp_mode;
  };


// Add this macro in the class declaration of each GUI, to define all the
// required virtual methods.  Example:
//   
//    class bx_rfb_gui_c : public bx_gui_c {
//    public:
//      bx_rfb_gui_c (void) {}
//      DECLARE_GUI_VIRTUAL_METHODS()
//    };
// Then, each method must be defined later in the file.
#define DECLARE_GUI_VIRTUAL_METHODS()                                         \
  virtual void specific_init(int argc, char **argv,                           \
                 unsigned x_tilesize, unsigned y_tilesize,                    \
		 unsigned header_bar_y);                                      \
  virtual void text_update(Bit8u *old_text, Bit8u *new_text,                  \
                          unsigned long cursor_x, unsigned long cursor_y,     \
                          bx_vga_tminfo_t tm_info, unsigned rows);            \
  virtual void graphics_tile_update(Bit8u *snapshot, unsigned x, unsigned y); \
  virtual void handle_events(void);                                           \
  virtual void flush(void);                                                   \
  virtual void clear_screen(void);                                            \
  virtual bx_bool palette_change(unsigned index,                              \
      unsigned red, unsigned green, unsigned blue);                           \
  virtual void dimension_update(unsigned x, unsigned y, unsigned fheight=0,   \
                                unsigned fwidth=0);                           \
  virtual unsigned create_bitmap(const unsigned char *bmap,                   \
      unsigned xdim, unsigned ydim);                                          \
  virtual unsigned headerbar_bitmap(unsigned bmap_id, unsigned alignment,     \
      void (*f)(void));                                                       \
  virtual void replace_bitmap(unsigned hbar_id, unsigned bmap_id);            \
  virtual void show_headerbar(void);                                          \
  virtual int get_clipboard_text(Bit8u **bytes, Bit32s *nbytes);              \
  virtual int set_clipboard_text(char *snapshot, Bit32u len);                 \
  virtual void mouse_enabled_changed_specific (bx_bool val);                  \
  virtual void exit(void);                                                    \
  /* end of DECLARE_GUI_VIRTUAL_METHODS */

#define BX_MAX_PIXMAPS 16
#define BX_MAX_HEADERBAR_ENTRIES 11
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


/////////////// GUI plugin support

// Define macro to supply gui plugin code.  This macro is called once in GUI to
// supply the plugin initialization methods.  Since it is nearly identical for
// each gui module, the macro is easier to maintain than pasting the same code
// in each one.
//
// Each gui should declare a class pointer called "theGui" which is derived
// from bx_gui_c, before calling this macro.  For example, the SDL port
// says:
//   static bx_sdl_gui_c *theGui;

#define IMPLEMENT_GUI_PLUGIN_CODE(gui_name)                           \
  int lib##gui_name##_LTX_plugin_init(plugin_t *plugin,               \
          plugintype_t type, int argc, char *argv[]) {                \
    genlog->info("installing %s module as the Bochs GUI", #gui_name); \
    theGui = new bx_##gui_name##_gui_c ();                            \
    bx_gui = theGui;                                                  \
    return(0); /* Success */                                          \
  }                                                                   \
  void lib##gui_name##_LTX_plugin_fini(void) { }
