/////////////////////////////////////////////////////////////////////////
// $Id: siminterface.h,v 1.22.2.15 2002-04-08 06:21:14 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
/*
 * gui/siminterface.h
 * $Id: siminterface.h,v 1.22.2.15 2002-04-08 06:21:14 bdenney Exp $
 *
 * Interface to the simulator, currently only used by control.cc.
 * The base class bx_simulator_interface_c, contains only virtual functions
 * and it defines the interface that control.cc is allowed to use.
 * In siminterface.cc, a class called bx_real_sim_c is defined with
 * bx_simulator_interface_c as its parent class.  Bx_real_sim_c 
 * implements each of the functions.  The separation into parent class
 * and child class leaves the possibility of making a different child
 * class that talks to the simulator in a different way (networking
 * for example).
 *
 */

// BX_UI_TEXT should be set to 1 when the text mode configuration interface
// is compiled in.  This gives each type of parameter a text_print and text_ask
// method (defined in gui/control.cc) so that you can call text_ask() on any
// kind of parameter to ask the user to edit the value.
//
// I have been considering whether to use the same strategy for the wxWindows
// interface, but I'm not sure if I like it.  One problem is that in order to
// declare member functions that are useful for wxWindows, the wxWindows header
// files would have to be included before the param object definitions.  That
// means that all the wxwindows headers would have be included when compiling
// every single bochs file.  One of the things I like about the separation
// between the simulator and GUI is that the two halves can be compiled without
// any knowledge of the other.  Bochs doesn't include <wx.h>, and the wxwindows
// code doesn't include <bochs.h>.  Aside from making compiles faster, this
// enforces the use of the interface so it keeps the interface clean (important
// when we may have multiple GUI implementations for example).  This argues
// for keeping GUI-specific structures out of the simulator interface.  It
// certainly works well for the text interface, but that's because FILE* is
// standard and portable.
#define BX_UI_TEXT (!BX_WITH_WX)

//////////////////////////////////////////////////////

// list of possible types for bx_param_c and descendant objects
typedef enum {
  BXT_OBJECT = 201,
  BXT_PARAM,
  BXT_PARAM_NUM,
  BXT_PARAM_BOOL,
  BXT_PARAM_ENUM,
  BXT_PARAM_STRING,
  BXT_LIST
} bx_objtype;

typedef enum {
  BXP_NULL = 101,
  BXP_IPS,
  BXP_VGA_UPDATE_INTERVAL,
  BXP_MOUSE_ENABLED,
  BXP_MEM_SIZE,
  BXP_ROM_PATH,
  BXP_ROM_ADDRESS,
  BXP_VGA_ROM_PATH,
  BXP_KBD_SERIAL_DELAY,
  BXP_KBD_PASTE_DELAY,
  BXP_KBD_TYPE,
  BXP_FLOPPY_CMD_DELAY,
  BXP_FLOPPYA_PATH,
  BXP_FLOPPYA_TYPE,
  BXP_FLOPPYA_STATUS,
  BXP_FLOPPYA,
  BXP_FLOPPYB_PATH,
  BXP_FLOPPYB_TYPE,
  BXP_FLOPPYB_STATUS,
  BXP_FLOPPYB,
  BXP_DISKC_PRESENT,
  BXP_DISKC_PATH,
  BXP_DISKC_CYLINDERS,
  BXP_DISKC_HEADS,
  BXP_DISKC_SPT,
  BXP_DISKC,
  BXP_DISKD_PRESENT,
  BXP_DISKD_PATH,
  BXP_DISKD_CYLINDERS,
  BXP_DISKD_HEADS,
  BXP_DISKD_SPT,
  BXP_DISKD,
  BXP_COM1_PRESENT,
  BXP_COM1_PATH,
  BXP_COM2_PRESENT,
  BXP_COM2_PATH,
  BXP_COM3_PRESENT,
  BXP_COM3_PATH,
  BXP_COM4_PRESENT,
  BXP_COM4_PATH,
  BXP_CDROM_PRESENT,
  BXP_CDROM_PATH,
  BXP_CDROM_INSERTED,
  BXP_CDROMD,
  BXP_PRIVATE_COLORMAP,
  BXP_FULLSCREEN,
  BXP_SCREENMODE,
  BXP_I440FX_SUPPORT,
  BXP_NEWHARDDRIVESUPPORT,
  BXP_LOG_FILENAME,
  BXP_CMOS_PATH,
  BXP_CMOS_IMAGE,
  BXP_CMOS_TIME0,
  BXP_LOAD32BITOS_WHICH,
  BXP_LOAD32BITOS_PATH,
  BXP_LOAD32BITOS_IOLOG,
  BXP_LOAD32BITOS_INITRD,
  BXP_LOAD32BITOS,
  BXP_BOOTDRIVE,
  BXP_MENU_MAIN,
  BXP_MENU_MEMORY,
  BXP_MENU_INTERFACE,
  BXP_MENU_DISK,
  BXP_MENU_SERIAL_PARALLEL,
  BXP_MENU_SOUND,
  BXP_MENU_MISC,
  BXP_MENU_RUNTIME,
  BXP_SYSTEM_CLOCK_SYNC,
  BXP_MAX_IPS,
  BXP_NE2K_VALID,
  BXP_NE2K_IOADDR,
  BXP_NE2K_IRQ,
  BXP_NE2K_MACADDR,
  BXP_NE2K_ETHMOD,
  BXP_NE2K_ETHDEV,
  BXP_NE2K,
  BXP_SB16_PRESENT,
  BXP_SB16_MIDIFILE,
  BXP_SB16_WAVEFILE,
  BXP_SB16_LOGFILE,
  BXP_SB16_MIDIMODE,
  BXP_SB16_WAVEMODE,
  BXP_SB16_LOGLEVEL,
  BXP_SB16_DMATIMER,
  BXP_SB16,
  BXP_PARPORT1_ENABLE,
  BXP_PARPORT1_OUTFILE,
  BXP_PARPORT2_ENABLE,
  BXP_PARPORT2_OUTFILE,
  BXP_KEYBOARD_USEMAPPING,
  BXP_KEYBOARD_MAP,
  BXP_KEYBOARD,
  BXP_ASK_FOR_PATHNAME,   // for general file selection dialog
  BXP_THIS_IS_THE_LAST    // used to determine length of list
} bx_id;

typedef enum {
  BX_TOOLBAR_UNDEFINED,
  BX_TOOLBAR_FLOPPYA,
  BX_TOOLBAR_FLOPPYB,
  BX_TOOLBAR_CDROMD,
  BX_TOOLBAR_RESET,
  BX_TOOLBAR_POWER,
  BX_TOOLBAR_COPY,
  BX_TOOLBAR_PASTE,
  BX_TOOLBAR_SNAPSHOT,
  BX_TOOLBAR_CONFIG,
  BX_TOOLBAR_MOUSE_EN
} bx_toolbar_buttons;

///////////////////////////////////////////////////////////////////
// event structure for communication between simulator and gui
///////////////////////////////////////////////////////////////////

///// types and definitions used in event structures

#define BX_EVT_IS_ASYNC(type) ((type) > __ALL_EVENTS_BELOW_ARE_ASYNC__)

typedef enum {
  __ALL_EVENTS_BELOW_ARE_SYNCHRONOUS__ = 2000,
  BX_SYNC_EVT_GET_PARAM,          // cpanel -> simulator -> cpanel
  BX_SYNC_EVT_ASK_PARAM,          // simulator -> cpanel -> simulator
  BX_SYNC_EVT_TICK,               // simulator -> cpanel, wait for response.
  __ALL_EVENTS_BELOW_ARE_ASYNC__,
  BX_ASYNC_EVT_KEY,               // vga gui -> simulator
  BX_ASYNC_EVT_MOUSE,             // vga gui -> simulator
  BX_ASYNC_EVT_SET_PARAM,         // cpanel -> simulator
  BX_ASYNC_EVT_LOG_MSG,           // simulator -> cpanel
  BX_ASYNC_EVT_VALUE_CHANGED,     // simulator -> cpanel
  BX_ASYNC_EVT_SHUTDOWN_GUI,      // simulator -> cpanel
  BX_ASYNC_EVT_TOOLBAR            // cpanel -> simulator
} BxEventType;

typedef union {
  Bit32s s32;
  char *charptr;
} AnyParamVal;

// Define substructures which make up the interior of BxEvent.  The
// substructures, such as BxKeyEvent or BxMouseEvent, should never be allocated
// on their own.  They are only intended to be used within the union in the
// BxEvent structure.

// Event type: BX_SYNC_EVT_TICK
// A tick event is synchronous, sent from the simulator to the GUI.  The
// event doesn't do anything visible.  Primarily it gives the GUI a chance
// to tell the simulator to quit, if necessary.  There may be other uses
// for the tick, such as giving some kind of regular status report or
// mentioning watched values that changed, but so far it's just for 
// that one thing.  There is no data associated with a tick event.

// Event type: BX_ASYNC_EVT_KEY
// (unused)
// A key event can be sent from the GUI to the Bochs simulator.  It is
// asynchronous.
typedef struct {
  // what was pressed?  This is a BX_KEY_* value.  For key releases,
  // BX_KEY_RELEASED is ORed with the base BX_KEY_*.
  Bit32u bx_key;
  Boolean raw_scancode;
} BxKeyEvent;

// Event type: BX_ASYNC_EVT_MOUSE
// (unused)
// A mouse event can be sent from the GUI to the Bochs simulator.  It is
// asynchronous.  Currently not used, but when Psyon's wxwindows gui is
// integrated, we will need mouse events.
typedef struct {
  // type is BX_EVT_MOUSE
  Bit16u x, y;             // coordinate (vga screen?)
  Bit8u buttons;           // which buttons are pressed.
                           // bit 0: 1=left button down, 0=up
                           // bit 1: 1=right button down, 0=up
} BxMouseEvent;

// Event type: BX_SYNC_EVT_GET_PARAM, BX_ASYNC_EVT_SET_PARAM
// (unused)
// Parameter set/get events are initiated by the GUI, since Bochs can always
// access the parameters directly.  So far, I haven't used this type.  In
// the GUI I just call SIM->get_param(parameter_id) to get a pointer to the
// bx_param_c object and then call the get/set methods.  This is okay for
// configuration since bochs is not running.  However it could be dangerous
// for the GUI thread to poke around in Bochs structures while the thread is
// running.  For these cases, I may have to be more careful: build parameter
// change events in the GUI and place them on Bochs's event queue to be
// processed during SIM->periodic() or something.
typedef struct {
  // type is BX_EVT_GET_PARAM, BX_EVT_SET_PARAM
  class bx_param_c *param;         // pointer to param structure
  AnyParamVal val;
} BxParamEvent;

// Event type: BX_SYNC_EVT_ASK_PARAM
// Syncronous event sent from the simulator to the GUI.  This tells the
// GUI to ask the user to choose the value of a parameter.  The GUI may 
// need to discover the type of parameter so that it can use the right
// kind of graphical display.  The BxParamEvent is used for these events
// too.
// FIXME: at the moment the GUI implements the ASK_PARAM event for just
// a few parameter types.  I need to implement the event for all parameter
// types.

// Event type: BX_ASYNC_EVT_VALUE_CHANGED
// (unused)
// Asynchronous event sent from the simulator to the GUI, telling it that
// some value that it (hopefully) cares about has changed.  This isn't
// being used yet, but a good example is in a debugger interface, you might
// want to maintain a reasonably current display of the PC or some other
// simulation state.  The GUI would set some kind of event mask (which
// doesn't exist now of course) and then when certain values change, the
// simulator would send this event so that the GUI can update.  We may need
// some kind of "flow control" since the simulator will be able to produce
// new events much faster than the gui can handle them.

// Event type: BX_ASYNC_EVT_LOG_MSG
// Asynchronous event from the simulator to the GUI.  When a BX_PANIC,
// BX_ERROR, BX_INFO, or BX_DEBUG is found in the simulator code, this 
// event type can be used to inform the GUI of the condition.  There is
// no point in sending messages to the GUI that will not be displayed; these
// would only slow the simulation.  So we will need some mechanism for 
// choosing what kinds of events will be delivered to the GUI.  Normally,
// you wouldn't want to look at the log unless something is going wrong.
// At that point, you might want to open up a window to watch the debug
// messages from one device only.
//
// Idea: Except for panics that require user attention to continue, it might be
// most efficient to just append log messages to a file.  When the user wants
// to look at the log messages, the gui can reopen the file (read only), skip
// to the end, and look backward for a reasonable number of lines to display
// (200?).  This allows it to skip over huge bursts of log entries without
// allocating memory, synchronizing threads, etc. for each.
typedef struct {
  // type is BX_EVT_LOG_MSG
  Bit8u level;
  char *prefix;
  char *msg;
} BxLogMsgEvent;

// Event type: BX_EVT_TOOLBAR
// Asynchronous event from the GUI to the simulator, sent when the user
// clicks on a toolbar button.  This may one day become something more 
// general, like a command event, but at the moment it's only needed for
// the toolbar events.
typedef struct {
  bx_toolbar_buttons button;
  bool on;   // for toggling buttons, on=true means the toolbar button is
             // pressed. on=false means it is not pressed.
} BxToolbarEvent;

// The BxEvent structure should be used for all events.  Every event has
// a type and a spot for a return code (only used for synchronous events).
typedef struct {
  BxEventType type; // what kind is this?
  Bit32s retcode;   // sucess or failure. only used for synchronous events.
  union {
    BxKeyEvent key;
    BxMouseEvent mouse;
    BxParamEvent param;
    BxLogMsgEvent logmsg;
    BxToolbarEvent toolbar;
  } u;
} BxEvent;


////////////////////////////////////////////////////////////////////
class bx_object_c {
private:
  bx_id id;
  bx_objtype type;
protected:
  void set_type (bx_objtype type);
public:
  bx_object_c (bx_id id);
  bx_id get_id () { return id; }
  Bit8u get_type () { return type; }
};

class bx_param_c : public bx_object_c {
protected:
  char *name;
  char *description;
  char *text_format;  // printf format string. %d for ints, %s for strings, etc.
  char *ask_format;  // format string for asking for a new value
  int runtime_param;
  int enabled;
public:
  bx_param_c (bx_id id, char *name, char *description);
  void set_format (char *format) {text_format = format;}
  char *get_format () {return text_format;}
  void set_ask_format (char *format) {ask_format = format; }
  char *get_ask_format () {return ask_format;}
  void set_runtime_param (int val) { runtime_param = val; }
  char *get_name () { return name; }
  char *get_description () { return description; }
  int get_enabled () { return enabled; }
  void set_enabled (int enabled) { this->enabled = enabled; }
  void reset () {}
  int getint () {return -1;}
#if BX_UI_TEXT
  virtual void text_print (FILE *fp) {}
  virtual int text_ask (FILE *fpin, FILE *fpout) {return -1;}
#endif
};

typedef Bit32s (*param_event_handler)(class bx_param_c *, int set, Bit32s val);

class bx_param_num_c : public bx_param_c {
protected:
  Bit32s min, max, val, initial_val;
  param_event_handler handler;
  int base;
public:
  bx_param_num_c (bx_id id,
      char *name,
      char *description,
      Bit32s min, Bit32s max, Bit32s initial_val);
  void reset ();
  void set_handler (param_event_handler handler);
  Bit32s get ();
  void set (Bit32s val);
  void set_base (int base) { this->base = base; }
  int get_base () { return base; }
  Bit32s get_min () { return min; }
  Bit32s get_max () { return max; }
#if BX_UI_TEXT
  virtual void text_print (FILE *fp);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

class bx_param_bool_c : public bx_param_num_c {
public:
  bx_param_bool_c (bx_id id, 
      char *name,
      char *description,
      Bit32s initial_val);
#if BX_UI_TEXT
  virtual void text_print (FILE *fp);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

class bx_param_enum_c : public bx_param_num_c {
  char **choices;
public:
  bx_param_enum_c (bx_id id, 
      char *name,
      char *description,
      char **choices,
      Bit32s initial_val,
      Bit32s value_base = 0);
  char *get_choice (int n) { return choices[n]; }
#if BX_UI_TEXT
  virtual void text_print (FILE *fp);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

typedef char* (*param_string_event_handler)(class bx_param_string_c *, int set, char *val, int maxlen);

class bx_param_string_c : public bx_param_c {
  int maxsize;
  char *val, *initial_val;
  param_string_event_handler handler;
  bx_param_num_c *options;
  char separator;
public:
  enum {
    BX_RAW_BYTES = 1,          // need binary text editor, like MAC addr
	BX_IS_FILENAME = 2,        // 1=yes it's a filename, 0=not a filename.
	                           // Some guis have a file browser.
	BX_SAVE_FILE_DIALOG = 4,   // Use save dialog opposed to open file dialog
  } bx_string_opt_bits;
  bx_param_string_c (bx_id id,
      char *name,
      char *description,
      char *initial_val,
      int maxsize=-1);
  virtual ~bx_param_string_c ();
  void reset ();
  void set_handler (param_string_event_handler handler);
  Bit32s get (char *buf, int len);
  char *getptr () {return val; }
  void set (char *buf);
  bx_param_num_c *get_options () { return options; }
  void set_separator (char sep) {separator = sep; }
#if BX_UI_TEXT
  virtual void text_print (FILE *fp);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

// declare a filename class.  It is identical to a string, except that
// it initializes the options differently.  This is just a shortcut
// for declaring a string param and setting the options with BX_IS_FILENAME.
class bx_param_filename_c : public bx_param_string_c {
public:
  bx_param_filename_c (bx_id id,
      char *name,
      char *description,
      char *initial_val,
      int maxsize=-1);
};

class bx_list_c : public bx_param_c {
private:
  // just a list of bx_param_c objects.  size tells current number of
  // objects in the list, and maxsize tells how many list items are
  // allocated in the constructor.
  bx_param_c **list;
  int size, maxsize;
  // options is a bit field whose bits are defined by bx_listopt_bits ORed
  // together.  Options is a bx_param so that if necessary the bx_list could
  // install a handler to cause get/set of options to have side effects.
  bx_param_num_c *options;
  // for a menu, the value of choice before the call to "ask" is default.
  // After ask, choice holds the value that the user chose.  Choice defaults
  // to 1 in the constructor.
  bx_param_num_c *choice;
  // title of the menu or series
  bx_param_string_c *title;
  // if the menu shows a "return to previous menu" type of choice,
  // this controls where that choice will go.
  bx_param_c *parent;
  void init ();
public:
  enum {
    // When a bx_list_c is displayed as a menu, SHOW_PARENT controls whether or
    // not the menu shows "0. Return to previous menu" or not.
    BX_SHOW_PARENT = (1<<0),
    // Some lists are best displayed shown as menus, others as a series of
    // related questions.  
    BX_SERIES_ASK = (1<<1)
  } bx_listopt_bits;
  //bx_list_c (bx_id id, int maxsize);
  bx_list_c (bx_id id, char *name, char *description, bx_param_c **init_list);
  virtual ~bx_list_c();
  void add (bx_param_c *param);
  bx_param_c *get (int index);
  bx_param_num_c *get_options () { return options; }
  bx_param_num_c *get_choice () { return choice; }
  bx_param_string_c *get_title () { return title; }
  void set_parent (bx_param_c *parent);
#if BX_UI_TEXT
  virtual void text_print (FILE *);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

////////////////////////////////////////////////////////////////


#define BX_FLOPPY_NONE   10 // floppy not present
#define BX_FLOPPY_1_2    11 // 1.2M  5.25"
#define BX_FLOPPY_1_44   12 // 1.44M 3.5"
#define BX_FLOPPY_2_88   13 // 2.88M 3.5"
#define BX_FLOPPY_720K   14 // 720K  3.5"
#define BX_FLOPPY_LAST   14 // last legal value of floppy type
#define BX_FLOPPY_GUESS  20 // decide based on image size

extern char *floppy_type_names[];
extern int n_floppy_type_names;
extern char *floppy_status_names[];
extern int n_floppy_status_names;
extern char *floppy_bootdisk_names[];
extern int n_floppy_bootdisk_names;
extern char *loader_os_names[];
extern int n_loader_os_names;
extern char *keyboard_type_names[];
extern int n_keyboard_type_names;

typedef struct {
  bx_param_string_c *Opath;
  bx_param_enum_c *Otype;
  bx_param_enum_c *Oinitial_status;
  } bx_floppy_options;

typedef struct {
  bx_param_bool_c *Opresent;
  bx_param_string_c *Opath;
  bx_param_num_c *Ocylinders;
  bx_param_num_c *Oheads;
  bx_param_num_c *Ospt;
  } bx_disk_options;

typedef struct {
  bx_param_bool_c *Opresent;
  bx_param_string_c *Odev;
  } bx_serial_options;

struct bx_cdrom_options
{
  bx_param_bool_c *Opresent;
  bx_param_string_c *Opath;
  bx_param_enum_c *Oinserted;
};


////////////////////////////////////////////////////////////////////
extern int bx_main (int argc, char *argv[]);

class bx_simulator_interface_c {
public:
  bx_simulator_interface_c ();
  virtual int get_init_done () { return -1; }
  virtual int set_init_done (int n) {return -1;}
  virtual void get_param_id_range (int *min, int *max) {}
  virtual int register_param (bx_id id, bx_param_c *it) {return -1;}
  virtual bx_param_c *get_param (bx_id id) {return NULL;}
  virtual bx_param_num_c *get_param_num (bx_id id) {return NULL;}
  virtual bx_param_string_c *get_param_string (bx_id id) {return NULL;}
  virtual int get_n_log_modules () {return -1;}
  virtual char *get_prefix (int mod) {return 0;}
  virtual int get_log_action (int mod, int level) {return -1;}
  virtual void set_log_action (int mod, int level, int action) {}
  virtual char *get_action_name (int action) {return 0;}
  virtual const char *get_log_level_name (int level) {return 0;}
  virtual int get_max_log_level () {return -1;}

  // exiting is somewhat complicated!  The preferred way to exit bochs is
  // to call BX_EXIT(exitcode).  That is defined to call 
  // SIM->quit_sim(exitcode).  The quit_sim function first calls
  // the cleanup functions in bochs so that it can destroy windows
  // and free up memory, then sends a notify message to the control 
  // panel telling it that bochs has stopped.
  virtual void quit_sim (int code) {}

  virtual int get_default_rc (char *path, int len) {return -1;}
  virtual int read_rc (char *path) {return -1;}
  virtual int write_rc (char *rc, int overwrite) {return -1;}
  virtual int get_log_file (char *path, int len) {return -1;}
  virtual int set_log_file (char *path) {return -1;}
  virtual int get_floppy_options (int drive, bx_floppy_options *out) {return -1;}
  virtual int get_cdrom_options (int drive, bx_cdrom_options *out) {return -1;}
  virtual char *get_floppy_type_name (int type) {return NULL;}

  // The control panel calls set_notify_callback to register an event 
  // handler.  This handler is for events which are generated by Bochs
  // or the Bochs controller vga screen.  For example, if the simulator
  // hits a panic and wants a dialog box to appear, it would use the
  // sim_interface_callback to ask the control panel to display a dialog.
  // At present, the standard floppy disk buttons, power button, etc are under
  // control of Bochs, NOT the control panel.  Bochs uses the callback to tell
  // the control panel that one of these buttons has been pressed.
  typedef BxEvent* (*sim_interface_callback_t)(void *theclass, BxEvent *event);
  virtual void set_notify_callback (sim_interface_callback_t func, void *arg) {}

  // send an event from the simulator to the GUI.
  virtual BxEvent* sim_to_gui_event (BxEvent *event) {return NULL;}

  // this "notify arg" passing is very ugly and Bryce needs to clean it up!
  virtual int log_msg (const char *prefix, int level, char *msg) {return -1;}
  // called by gui.cc event handler when a button is pressed.
  virtual int ask_param (bx_id param) {return -1;}

  virtual int get_enabled () {return -1;}
  virtual void set_enabled (int enabled) {}
  // ask the user for a pathname
  virtual int ask_filename (char *filename, int maxlen, char *prompt, char *the_default, int flags) {return -1;}
  // called at a regular interval, currently by the keyboard handler.
  virtual void periodic () {}
};

extern bx_simulator_interface_c *SIM;

extern void siminterface_init ();
extern int bx_continue_after_control_panel (int arg, int argc, char *argv[]);
