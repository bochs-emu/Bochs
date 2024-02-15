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

#ifndef BX_GUI_H
#define BX_GUI_H

#include "gui/siminterface.h"

// header bar and status bar stuff
#define BX_HEADER_BAR_Y 32

#define BX_MAX_PIXMAPS 17
#define BX_MAX_HEADERBAR_ENTRIES 12

// align pixmaps towards left or right side of header bar
#define BX_GRAVITY_LEFT 10
#define BX_GRAVITY_RIGHT 11

#define BX_MAX_STATUSITEMS 10

// gui dialog capabilities
#define BX_GUI_DLG_FLOPPY       0x01
#define BX_GUI_DLG_CDROM        0x02
#define BX_GUI_DLG_SNAPSHOT     0x04
#define BX_GUI_DLG_RUNTIME      0x08
#define BX_GUI_DLG_USER         0x10
#define BX_GUI_DLG_SAVE_RESTORE 0x20
#define BX_GUI_DLG_ALL          0x3F

// text mode blink feature
#define BX_TEXT_BLINK_MODE      0x01
#define BX_TEXT_BLINK_TOGGLE    0x02
#define BX_TEXT_BLINK_STATE     0x04

// modifier keys
#define BX_MOD_KEY_SHIFT        0x01
#define BX_MOD_KEY_CTRL         0x02
#define BX_MOD_KEY_ALT          0x04
#define BX_MOD_KEY_CAPS         0x08

// mouse capture toggle feature
#define BX_MT_KEY_CTRL          0x01
#define BX_MT_KEY_ALT           0x02
#define BX_MT_KEY_F10           0x04
#define BX_MT_KEY_F12           0x08
#define BX_MT_MBUTTON           0x10
#define BX_MT_LBUTTON           0x20
#define BX_MT_RBUTTON           0x40

#define BX_GUI_MT_CTRL_MB       (BX_MT_KEY_CTRL | BX_MT_MBUTTON)
#define BX_GUI_MT_CTRL_LRB      (BX_MT_KEY_CTRL | BX_MT_LBUTTON | BX_MT_RBUTTON)
#define BX_GUI_MT_CTRL_F10      (BX_MT_KEY_CTRL | BX_MT_KEY_F10)
#define BX_GUI_MT_F12           (BX_MT_KEY_F12)
#define BX_GUI_MT_CTRL_ALT      (BX_MT_KEY_CTRL | BX_MT_KEY_ALT)

typedef struct {
  Bit16u  start_address;
  Bit8u   cs_start;
  Bit8u   cs_end;
  Bit16u  line_offset;
  Bit16u  line_compare;
  Bit8u   h_panning;
  Bit8u   v_panning;
  bool line_graphics;
  bool split_hpanning;
  Bit8u   blink_flags;
  Bit8u   actl_palette[16];
} bx_vga_tminfo_t;

typedef struct {
  Bit16u bpp, pitch;
  Bit8u red_shift, green_shift, blue_shift;
  Bit8u is_indexed, is_little_endian;
  unsigned long red_mask, green_mask, blue_mask;
  bool snapshot_mode;
} bx_svga_tileinfo_t;


BOCHSAPI_MSVCONLY Bit8u reverse_bitorder(Bit8u);

BOCHSAPI extern class bx_gui_c *bx_gui;

#if BX_SUPPORT_X86_64
  #define BOCHS_WINDOW_NAME "Bochs x86-64 emulator, http://bochs.sourceforge.net/"
#else
  #define BOCHS_WINDOW_NAME "Bochs x86 emulator, http://bochs.sourceforge.net/"
#endif

// The bx_gui_c class provides data and behavior that is common to
// all guis.  Each gui implementation will override the abstract methods.
class BOCHSAPI bx_gui_c : public logfunctions {
public:
  bx_gui_c (void);
  virtual ~bx_gui_c ();
  // Define the following functions in the module for your particular GUI
  // (x.cc, win32.cc, ...)
  virtual void specific_init(int argc, char **argv, unsigned header_bar_y) = 0;
  virtual void text_update(Bit8u *old_text, Bit8u *new_text,
                          unsigned long cursor_x, unsigned long cursor_y,
                          bx_vga_tminfo_t *tm_info) = 0;
  virtual void graphics_tile_update(Bit8u *tile, unsigned x, unsigned y) = 0;
  virtual void handle_events(void) = 0;
  virtual void flush(void) = 0;
  virtual void clear_screen(void) = 0;
  virtual bool palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue) = 0;
  virtual void dimension_update(unsigned x, unsigned y, unsigned fheight=0, unsigned fwidth=0, unsigned bpp=8) = 0;
  virtual unsigned create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim) = 0;
  virtual unsigned headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void)) = 0;
  virtual void replace_bitmap(unsigned hbar_id, unsigned bmap_id) = 0;
  virtual void show_headerbar(void) = 0;
  virtual int get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)  = 0;
  virtual int set_clipboard_text(char *snapshot, Bit32u len) = 0;
  virtual void mouse_enabled_changed_specific (bool val) = 0;
  virtual void exit(void) = 0;
  // new graphics API methods (compatibility mode in gui.cc)
  virtual bx_svga_tileinfo_t *graphics_tile_info(bx_svga_tileinfo_t *info);
  virtual Bit8u *graphics_tile_get(unsigned x, unsigned y, unsigned *w, unsigned *h);
  virtual void graphics_tile_update_in_place(unsigned x, unsigned y, unsigned w, unsigned h);
  // new text update API
  virtual void set_font(bool lg) {}
  virtual void draw_char(Bit8u ch, Bit8u fc, Bit8u bc, Bit16u xc, Bit16u yc,
                         Bit8u fw, Bit8u fh, Bit8u fx, Bit8u fy,
                         bool gfxcharw9, Bit8u cs, Bit8u ce, bool curs, bool font2) {}
  // optional gui methods (stubs or default code in gui.cc)
  virtual void statusbar_setitem_specific(int element, bool active, bool w) {}
  virtual void set_tooltip(unsigned hbar_id, const char *tip) {}
  // set_display_mode() changes the mode between the configuration interface
  // and the simulation.  This is primarily intended for display libraries
  // which have a full-screen mode such as SDL or term.  The display mode is
  // set to DISP_MODE_CONFIG before displaying any configuration menus,
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
  virtual void show_ips(Bit32u ips_count);
  virtual void beep_on(float frequency);
  virtual void beep_off();
  virtual void get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp);
  virtual void set_mouse_mode_absxy(bool mode) {}
#if BX_USE_GUI_CONSOLE
  virtual void set_console_edit_mode(bool mode) {}
#endif

  // The following function(s) are defined already, and your
  // GUI code calls them
  static void key_event(Bit32u key);
  static void set_text_charmap(Bit8u map, Bit8u *fbuffer);
  static Bit8u get_mouse_headerbar_id();

  void init(int argc, char **argv, unsigned max_xres, unsigned max_yres,
            unsigned x_tilesize, unsigned y_tilesize);
  void cleanup(void);
  void draw_char_common(Bit8u ch, Bit8u fc, Bit8u bc, Bit16u xc, Bit16u yc,
                        Bit8u fw, Bit8u fh, Bit8u fx, Bit8u fy,
                        bool gfxcharw9, Bit8u cs, Bit8u ce, bool curs, bool font2);
  void text_update_common(Bit8u *old_text, Bit8u *new_text,
                          Bit16u cursor_address, bx_vga_tminfo_t *tm_info);
  void graphics_tile_update_common(Bit8u *tile, unsigned x, unsigned y);
  bx_svga_tileinfo_t *graphics_tile_info_common(bx_svga_tileinfo_t *info);
  Bit8u* get_snapshot_buffer(void) {return snapshot_buffer;}
  bool palette_change_common(Bit8u index, Bit8u red, Bit8u green, Bit8u blue);
  void update_drive_status_buttons(void);
  static void     mouse_enabled_changed(bool val);
  int register_statusitem(const char *text, bool auto_off=0);
  void unregister_statusitem(int id);
  void statusbar_setitem(int element, bool active, bool w=0);
  void statusbar_setall(bool active);
  static void init_signal_handlers();
  static void toggle_mouse_enable(void);
  bool mouse_toggle_check(Bit32u key, bool pressed);
  const char* get_toggle_info(void);
  Bit8u get_modifier_keys(void);
  Bit8u set_modifier_keys(Bit8u modifier, bool pressed);
  bool parse_user_shortcut(const char *val);
#if BX_DEBUGGER && BX_DEBUGGER_GUI
  void init_debug_dialog(void);
  void close_debug_dialog(void);
#endif
#if BX_USE_GUI_CONSOLE
  bool has_gui_console(void) {return console.present;}
  bool console_running(void) {return console.running;}
  void console_refresh(bool force);
  void console_key_enq(Bit8u key);
  int bx_printf(const char *s);
  char* bx_gets(char *s, int size);
#else
  bool has_gui_console(void) {return 0;}
  bool console_running(void) {return 0;}
  void console_refresh(bool force) {}
  void console_key_enq(Bit8u key) {}
#endif
  bool has_command_mode(void) {return command_mode.present;}
  bool command_mode_active(void) {return command_mode.active;}
  void set_command_mode(bool active);
  void set_fullscreen_mode(bool active) {fullscreen_mode = active;}
  // marklog handler without button, called in gui command mode
  static void marklog_handler(void);

protected:
  // And these are defined and used privately in gui.cc
  // header bar button handlers
  static void floppyA_handler(void);
  static void floppyB_handler(void);
  static void cdrom1_handler(void);
  static void reset_handler(void);
  static void power_handler(void);
  static void copy_handler(void);
  static void paste_handler(void);
  static void snapshot_handler(void);
  static void config_handler(void);
  static void userbutton_handler(void);
  static void save_restore_handler(void);
  // process clicks on the "classic" Bochs headerbar
  void headerbar_click(int x);
  // snapshot helper functions
  static void make_text_snapshot(char **snapshot, Bit32u *length);
  static Bit32u set_snapshot_mode(bool mode);
  // status bar LED timer
  static void led_timer_handler(void *);
  void led_timer(void);
#if BX_USE_GUI_CONSOLE
  void console_init(void);
  void console_cleanup(void);
#else
  void console_cleanup(void) {}
#endif

  // header bar buttons
  bool floppyA_status;
  bool floppyB_status;
  bool cdrom1_status;
  unsigned floppyA_bmap_id, floppyA_eject_bmap_id, floppyA_hbar_id;
  unsigned floppyB_bmap_id, floppyB_eject_bmap_id, floppyB_hbar_id;
  unsigned cdrom1_bmap_id, cdrom1_eject_bmap_id, cdrom1_hbar_id;
  unsigned power_bmap_id,    power_hbar_id;
  unsigned reset_bmap_id,    reset_hbar_id;
  unsigned copy_bmap_id, copy_hbar_id;
  unsigned paste_bmap_id, paste_hbar_id;
  unsigned snapshot_bmap_id, snapshot_hbar_id;
  unsigned config_bmap_id, config_hbar_id;
  unsigned mouse_bmap_id, nomouse_bmap_id, mouse_hbar_id;
  unsigned user_bmap_id, user_hbar_id;
  unsigned save_restore_bmap_id, save_restore_hbar_id;
  // the "classic" Bochs headerbar
  unsigned bx_headerbar_entries;
  struct {
    unsigned bmap_id;
    unsigned xdim;
    unsigned ydim;
    unsigned xorigin;
    unsigned alignment;
    void (*f)(void);
  } bx_headerbar_entry[BX_MAX_HEADERBAR_ENTRIES];
  // text charmap
  Bit8u vga_charmap[2][0x2000];
  bool charmap_updated;
  bool char_changed[2][256];
  // status bar items
  unsigned statusitem_count;
  int led_timer_index;
  struct {
    bool in_use;
    char text[8];
    bool active;
    bool mode; // read/write
    bool auto_off;
    Bit8u counter;
  } statusitem[BX_MAX_STATUSITEMS];
  // display mode
  disp_mode_t disp_mode;
  // new graphics API (with compatibility mode)
  bool new_gfx_api;
  Bit16u host_xres;
  Bit16u host_yres;
  Bit16u host_pitch;
  Bit8u host_bpp;
  Bit8u *framebuffer;
  // new text update API
  bool new_text_api;
  Bit16u cursor_address;
  bx_vga_tminfo_t tm_info;
  // maximum guest display size and tile size
  unsigned max_xres;
  unsigned max_yres;
  unsigned x_tilesize;
  unsigned y_tilesize;
  // current guest display settings
  bool guest_textmode;
  Bit8u   guest_fwidth;
  Bit8u   guest_fheight;
  Bit16u  guest_xres;
  Bit16u  guest_yres;
  Bit8u   guest_bpp;
  // graphics snapshot
  bool snapshot_mode;
  Bit8u *snapshot_buffer;
  struct {
    Bit8u blue;
    Bit8u green;
    Bit8u red;
    Bit8u reserved;
  } palette[256];
  // modifier keys
  Bit8u keymodstate;
  // mouse toggle setup
  Bit8u toggle_method;
  Bit32u toggle_keystate;
  char mouse_toggle_text[20];
  // userbutton shortcut
  Bit32u user_shortcut[4];
  int user_shortcut_len;
  bool user_shortcut_error;
  // gui dialog capabilities
  Bit32u dialog_caps;
#if BX_USE_GUI_CONSOLE
  struct {
    bool present;
    bool running;
    Bit8u *screen;
    Bit8u *oldscreen;
    Bit8u saved_fwidth;
    Bit8u saved_fheight;
    Bit16u saved_xres;
    Bit16u saved_yres;
    Bit8u  saved_bpp;
    Bit8u saved_palette[32];
    Bit8u saved_charmap[0x2000];
    unsigned cursor_x;
    unsigned cursor_y;
    Bit16u cursor_addr;
    bx_vga_tminfo_t tminfo;
    Bit8u keys[16];
    Bit8u n_keys;
  } console;
#endif
  // command mode
  struct {
    bool present;
    bool active;
  } command_mode;
  bool fullscreen_mode;
  Bit32u marker_count;
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
#define DECLARE_GUI_VIRTUAL_METHODS()                                       \
virtual void specific_init(int argc, char **argv,                           \
                           unsigned header_bar_y);                          \
virtual void text_update(Bit8u *old_text, Bit8u *new_text,                  \
                  unsigned long cursor_x, unsigned long cursor_y,           \
                  bx_vga_tminfo_t *tm_info);                                \
virtual void graphics_tile_update(Bit8u *tile, unsigned x, unsigned y);     \
virtual void handle_events(void);                                           \
virtual void flush(void);                                                   \
virtual void clear_screen(void);                                            \
virtual bool palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue); \
virtual void dimension_update(unsigned x, unsigned y, unsigned fheight=0,   \
                          unsigned fwidth=0, unsigned bpp=8);               \
virtual unsigned create_bitmap(const unsigned char *bmap,                   \
                               unsigned xdim, unsigned ydim);               \
virtual unsigned headerbar_bitmap(unsigned bmap_id, unsigned alignment,     \
                                  void (*f)(void));                         \
virtual void replace_bitmap(unsigned hbar_id, unsigned bmap_id);            \
virtual void show_headerbar(void);                                          \
virtual int get_clipboard_text(Bit8u **bytes, Bit32s *nbytes);              \
virtual int set_clipboard_text(char *snapshot, Bit32u len);                 \
virtual void mouse_enabled_changed_specific(bool val);                  \
virtual void exit(void);                                                    \
/* end of DECLARE_GUI_VIRTUAL_METHODS */

#define DECLARE_GUI_NEW_VIRTUAL_METHODS()                                   \
virtual bx_svga_tileinfo_t *graphics_tile_info(bx_svga_tileinfo_t *info);   \
virtual Bit8u *graphics_tile_get(unsigned x, unsigned y,                    \
                             unsigned *w, unsigned *h);                     \
virtual void graphics_tile_update_in_place(unsigned x, unsigned y,          \
                                       unsigned w, unsigned h);
/* end of DECLARE_GUI_NEW_VIRTUAL_METHODS */

#define BX_KEY_PRESSED  0x00000000
#define BX_KEY_RELEASED 0x80000000

#define BX_KEY_UNHANDLED 0x10000000

enum {
  BX_KEY_CTRL_L,
  BX_KEY_SHIFT_L,

  BX_KEY_F1,
  BX_KEY_F2,
  BX_KEY_F3,
  BX_KEY_F4,
  BX_KEY_F5,
  BX_KEY_F6,
  BX_KEY_F7,
  BX_KEY_F8,
  BX_KEY_F9,
  BX_KEY_F10,
  BX_KEY_F11,
  BX_KEY_F12,

  BX_KEY_CTRL_R,
  BX_KEY_SHIFT_R,
  BX_KEY_CAPS_LOCK,
  BX_KEY_NUM_LOCK,
  BX_KEY_ALT_L,
  BX_KEY_ALT_R,

  BX_KEY_A,
  BX_KEY_B,
  BX_KEY_C,
  BX_KEY_D,
  BX_KEY_E,
  BX_KEY_F,
  BX_KEY_G,
  BX_KEY_H,
  BX_KEY_I,
  BX_KEY_J,
  BX_KEY_K,
  BX_KEY_L,
  BX_KEY_M,
  BX_KEY_N,
  BX_KEY_O,
  BX_KEY_P,
  BX_KEY_Q,
  BX_KEY_R,
  BX_KEY_S,
  BX_KEY_T,
  BX_KEY_U,
  BX_KEY_V,
  BX_KEY_W,
  BX_KEY_X,
  BX_KEY_Y,
  BX_KEY_Z,

  BX_KEY_0,
  BX_KEY_1,
  BX_KEY_2,
  BX_KEY_3,
  BX_KEY_4,
  BX_KEY_5,
  BX_KEY_6,
  BX_KEY_7,
  BX_KEY_8,
  BX_KEY_9,

  BX_KEY_ESC,

  BX_KEY_SPACE,
  BX_KEY_SINGLE_QUOTE,
  BX_KEY_COMMA,
  BX_KEY_PERIOD,
  BX_KEY_SLASH,

  BX_KEY_SEMICOLON,
  BX_KEY_EQUALS,

  BX_KEY_LEFT_BRACKET,
  BX_KEY_BACKSLASH,
  BX_KEY_RIGHT_BRACKET,
  BX_KEY_MINUS,
  BX_KEY_GRAVE,

  BX_KEY_BACKSPACE,
  BX_KEY_ENTER,
  BX_KEY_TAB,

  BX_KEY_LEFT_BACKSLASH,
  BX_KEY_PRINT,
  BX_KEY_SCRL_LOCK,
  BX_KEY_PAUSE,

  BX_KEY_INSERT,
  BX_KEY_DELETE,
  BX_KEY_HOME,
  BX_KEY_END,
  BX_KEY_PAGE_UP,
  BX_KEY_PAGE_DOWN,

  BX_KEY_KP_ADD,
  BX_KEY_KP_SUBTRACT,
  BX_KEY_KP_END,
  BX_KEY_KP_DOWN,
  BX_KEY_KP_PAGE_DOWN,
  BX_KEY_KP_LEFT,
  BX_KEY_KP_RIGHT,
  BX_KEY_KP_HOME,
  BX_KEY_KP_UP,
  BX_KEY_KP_PAGE_UP,
  BX_KEY_KP_INSERT,
  BX_KEY_KP_DELETE,
  BX_KEY_KP_5,

  BX_KEY_UP,
  BX_KEY_DOWN,
  BX_KEY_LEFT,
  BX_KEY_RIGHT,

  BX_KEY_KP_ENTER,
  BX_KEY_KP_MULTIPLY,
  BX_KEY_KP_DIVIDE,

  BX_KEY_WIN_L,
  BX_KEY_WIN_R,
  BX_KEY_MENU,

  BX_KEY_ALT_SYSREQ,
  BX_KEY_CTRL_BREAK,

  BX_KEY_INT_BACK,
  BX_KEY_INT_FORWARD,
  BX_KEY_INT_STOP,
  BX_KEY_INT_MAIL,
  BX_KEY_INT_SEARCH,
  BX_KEY_INT_FAV,
  BX_KEY_INT_HOME,

  BX_KEY_POWER_MYCOMP,
  BX_KEY_POWER_CALC,
  BX_KEY_POWER_SLEEP,
  BX_KEY_POWER_POWER,
  BX_KEY_POWER_WAKE,

  BX_KEY_NBKEYS
};
// If you add BX_KEYs, don't forget this:
// - the last entry must be BX_KEY_NBKEYS
// - upate the scancodes table in the file iodev/scancodes.cc
// - update the bx_key_symbol table in the file gui/keymap.cc


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

#define IMPLEMENT_GUI_PLUGIN_CODE(gui_name)                             \
  PLUGIN_ENTRY_FOR_GUI_MODULE(gui_name)                                 \
  {                                                                     \
    if (mode == PLUGIN_INIT) {                                          \
      genlog->info("installing %s module as the Bochs GUI", #gui_name); \
      theGui = new bx_##gui_name##_gui_c ();                            \
      bx_gui = theGui;                                                  \
    } else if (mode == PLUGIN_FINI) {                                   \
      delete theGui;                                                    \
      bx_gui = NULL;                                                    \
    } else if (mode == PLUGIN_PROBE) {                                  \
      return (int)PLUGTYPE_GUI;                                         \
    }                                                                   \
    return(0); /* Success */                                            \
  }

#endif
