/////////////////////////////////////////////////////////////////////////
// $Id: siminterface.h,v 1.49 2002-08-28 03:14:53 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
// Before I can describe what this file is for, I have to make the
// distinction between a configuration interface (CI) and the VGA display
// window (VGAW).  I will try to avoid the term 'GUI' because it is unclear 
// if that means CI or VGAW, and because not all interfaces are graphical 
// anyway.
//
// The traditional Bochs screen is a window with a large VGA display panel and
// a series of buttons (floppy, cdrom, snapshot, power).  Over the years, we
// have collected many implementations of the VGAW for different environments
// and platforms; each implementation is in a separate file under gui/*:
// x.cc, win32.cc, beos.cc, macintosh.cc, etc.  The files gui.h and gui.cc
// define the platform independent part of the VGAW, leaving about 15 methods
// of the bx_gui_c class undefined.  The platform dependent file must 
// implement the remaining 15 methods.
//
// The configuration interface is relatively new, started by Bryce Denney in
// June 2001.  The CI is intended to allow the user to edit a variety of
// configuration and runtime options.  Some options, such as memory size or
// enabling the ethernet card, should only be changed before the simulation
// begins; others, such as floppy disk image, instructions per second, and
// logging options can be safely changed at runtime.  The CI allows the user to
// make these changes.  Before the CI existed, only a few things could be
// changed at runtime, all linked to clicking on the VGAW buttons.
//
// At the time that the CI was conceived, we were still debating what form the
// user interface part would take: stdin/stdout menus, a graphical application
// with menus and dialogs running in a separate thread, or even a tiny web
// server that you can connect to with a web browser.  As a result the CI was
// designed so that implementation of the user interface can be easily
// replaced.  To this end, we kept the CI in two distinct parts: the CUI (user
// interface of the configuration interface) which displays menus and allows
// the user to edit settings, and the siminterface that communicates between
// the bochs simulator code and the CUI code.  The first CUI was a series of
// text mode menus implemented in control.cc.
//
// The siminterface is the glue between the CUI and the simulator.  There is
// just one global instance of the siminterface object, which can be referred
// to by the global variable
//   bx_simulator_interface_c *SIM;
// The base class bx_simulator_interface_c, contains only virtual functions
// and it defines the interface that the CUI is allowed to use.  In
// siminterface.cc, a class called bx_real_sim_c is defined with
// bx_simulator_interface_c as its parent class.  Bx_real_sim_c implements each
// of the functions.  The separation into parent class and child class leaves
// the possibility of making a different child class that talks to the
// simulator in a different way (networking for example).  At the moment this
// abstraction is serving no specific purpose, so it might be removed in the
// future.
//
// An important part of the siminterface implementation is the use of
// parameter classes, or bx_param_*.  The parameter classes are
// described below, where they are declared.  Search for 
// "parameter classes".
//
// Also the siminterface.h header file declares data structures for
// certain events that pass between the siminterface and the CUI.
// Search for "event structures" below for details.



//////////////////////////////////////////////////////
// BX_UI_TEXT should be set to 1 when the text mode configuration interface
// is compiled in.  This gives each type of parameter a text_print and text_ask
// method (defined in gui/control.cc) so that you can call text_ask() on any
// kind of parameter to ask the user to edit the value.
//
// I have been considering whether to use the same strategy for the
// wxWindows interface, but I'm not sure if I like it.  One problem is
// that in order to declare member functions that are useful for
// wxWindows, the wxWindows header files would have to be included
// before the param object definitions.  That means that all the
// wxwindows headers would have be included when compiling every
// single bochs file.  One of the things I like about the separation
// between the simulator and CUI is that the two parts can be
// compiled without any knowledge of the other.  Bochs doesn't include
// <wx.h>, and the wxwindows code doesn't need to include <bochs.h>.
// (Never mind that it does at the moment!)  Aside from making
// compiles faster, this enforces the use of the interface so it keeps
// the interface clean (important when we may have multiple UI
// implementations for example).  This argues for keeping UI-specific
// structures out of the simulator interface.  It certainly works ok
// for the text interface, but that's because FILE* is standard and
// portable.
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

// list if parameter id values.  The actual values are not important;
// it's only important that they all be different from each other.
typedef enum {
  BXP_NULL = 101,
  BXP_IPS,
  BXP_VGA_UPDATE_INTERVAL,
  BXP_MOUSE_ENABLED,
  BXP_MEM_SIZE,
  BXP_ROM_PATH,
  BXP_ROM_ADDRESS,
  BXP_VGA_ROM_PATH,
  BXP_OPTROM1_PATH,
  BXP_OPTROM1_ADDRESS,
  BXP_OPTROM2_PATH,
  BXP_OPTROM2_ADDRESS,
  BXP_OPTROM3_PATH,
  BXP_OPTROM3_ADDRESS,
  BXP_OPTROM4_PATH,
  BXP_OPTROM4_ADDRESS,
  BXP_KBD_SERIAL_DELAY,
  BXP_KBD_PASTE_DELAY,
  BXP_KBD_TYPE,
  BXP_FLOPPY_CMD_DELAY,
  BXP_FLOPPYA_DEVTYPE,
  BXP_FLOPPYA_PATH,
  BXP_FLOPPYA_TYPE,
  BXP_FLOPPYA_STATUS,
  BXP_FLOPPYA,
  BXP_FLOPPYB_DEVTYPE,
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
  BXP_COM1_ENABLED,
  BXP_COM1_PATH,
  BXP_COM2_ENABLED,
  BXP_COM2_PATH,
  BXP_COM3_ENABLED,
  BXP_COM3_PATH,
  BXP_COM4_ENABLED,
  BXP_COM4_PATH,
  BXP_CDROM_PRESENT,
  BXP_CDROM_PATH,
  BXP_CDROM_STATUS,
  BXP_CDROMD,
  BXP_PRIVATE_COLORMAP,
  BXP_FULLSCREEN,
  BXP_SCREENMODE,
  BXP_I440FX_SUPPORT,
  BXP_NEWHARDDRIVESUPPORT,
  BXP_LOG_FILENAME,
  BXP_LOG_PREFIX,
  BXP_CMOS_PATH,
  BXP_CMOS_IMAGE,
  BXP_CMOS_TIME0,
  BXP_LOAD32BITOS_WHICH,
  BXP_LOAD32BITOS_PATH,
  BXP_LOAD32BITOS_IOLOG,
  BXP_LOAD32BITOS_INITRD,
  BXP_LOAD32BITOS,
  BXP_BOOTDRIVE,
  BXP_FLOPPYSIGCHECK,
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
  BXP_NE2K_SCRIPT,
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
  BXP_PARPORT1_ENABLED,
  BXP_PARPORT1_OUTFILE,
  BXP_PARPORT2_ENABLED,
  BXP_PARPORT2_OUTFILE,
  BXP_KEYBOARD_USEMAPPING,
  BXP_KEYBOARD_MAP,
  BXP_KEYBOARD,
  BXP_USER_SHORTCUT,
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
  BX_TOOLBAR_MOUSE_EN,
  BX_TOOLBAR_USER
} bx_toolbar_buttons;

// Log level defines
typedef enum {
  LOGLEV_DEBUG = 0,
  LOGLEV_INFO,
  LOGLEV_ERROR,
  LOGLEV_PANIC,
  N_LOGLEV
} bx_log_levels;

///////////////////////////////////////////////////////////////////
// event structures for communication between simulator and CUI
///////////////////////////////////////////////////////////////////
// Because the CUI (user interface of the configuration interface) 
// might be in a different thread or even a different process, we pass
// events encoded in data structures to it instead of just calling
// functions.  Each type of event is declared as a different
// structure, and then all those structures are squished into a
// union in BxEvent.  (BTW, this is almost exactly how X windows
// event structs work.)
//
// These are simple structs, unblemished by C++ methods and tricks.
// No matter what event type it is, we allocate a BxEvent for each
// one, as opposed to trying to save a few bytes on small events by
// allocating only the bytes necessary for it.  This makes it easy and
// fast to store events in a queue, like this
//   BxEvent event_queue[MAX_EVENTS];
//
// Events come in two varieties: synchronous and asynchronous.  We
// have to worry about sync and async events because the CUI and the
// simulation may be running in different threads.  An async event is
// the simplest.  Whichever thread originates the event just builds
// the data structure, sends it, and then continues with its business.
// Async events can go in either direction.  Synchronous events
// require the other thread to "respond" before the originating thread
// can continue.  It's like a function with a return value; you can't
// continue until you get the return value back.
//
// Examples:
//
// async event: In the wxWindows implementation, both the CUI and the
// VGAW operate in the wxWindows GUI thread.  When the user presses a
// key, wxWindows sends a wxKeyEvent to the VGAW event handler code in
// wx.cc.  The VGAW handler then builds a BxEvent with
// type=BX_ASYNC_EVT_KEY, and fills in the bx_key and raw_scancode
// fields.  The asynchronous event is placed on the event_queue for
// the simulator, then the VGAW handler returns.  (With wxWindows and
// many other graphical libaries, the event handler must return
// quickly because the window will not be updated until it's done.)
// Some time later, the simulator reaches the point where it checks
// for new events from the user (actually controlled by
// bx_keyb_c::periodic() in iodev/keyboard.cc) and calls
// bx_gui.handle_events().  Then all the events in the queue are
// processed by the simulator.  There is no "response" sent back to
// the originating thread.
//
// sync event: Sometimes the simulator reaches a point where it needs
// to ask the user how to proceed.  In this case, the simulator sends
// a synchronous event because it requires a response before it can 
// continue.  It builds an event structure, perhaps with type
// BX_SYNC_EVT_ASK_PARAM, sends it to the user interface 
// using the event handler function defined by set_notify_callback(),
// and pauses the simulation.  The user interface asks the user the
// question, and puts the answer into the BxEvent.retcode field.  The
// event handler function returns the modified BxEvent with retcode
// filled in, and the simulation continues.  The details of this
// transaction can be complicated if the simulation and CUI are not
// in the same thread, but the behavior is as described.
//

///// types and definitions used in event structures

#define BX_EVT_IS_ASYNC(type) ((type) > __ALL_EVENTS_BELOW_ARE_ASYNC__)

typedef enum {
  __ALL_EVENTS_BELOW_ARE_SYNCHRONOUS__ = 2000,
  BX_SYNC_EVT_GET_PARAM,          // CUI -> simulator -> CUI
  BX_SYNC_EVT_ASK_PARAM,          // simulator -> CUI -> simulator
  BX_SYNC_EVT_TICK,               // simulator -> CUI, wait for response.
  BX_SYNC_EVT_LOG_ASK,            // simulator -> CUI, wait for response.
  __ALL_EVENTS_BELOW_ARE_ASYNC__,
  BX_ASYNC_EVT_KEY,               // vga window -> simulator
  BX_ASYNC_EVT_MOUSE,             // vga window -> simulator
  BX_ASYNC_EVT_SET_PARAM,         // CUI -> simulator
  BX_ASYNC_EVT_LOG_MSG,           // simulator -> CUI
  BX_ASYNC_EVT_VALUE_CHANGED,     // simulator -> CUI
  BX_ASYNC_EVT_TOOLBAR            // CUI -> simulator
} BxEventType;

typedef union {
  Bit32s s32;
  char *charptr;
} AnyParamVal;

// Define substructures which make up the interior of BxEvent.  The
// substructures, such as BxKeyEvent or BxMouseEvent, should never be
// allocated on their own.  They are only intended to be used within
// the union in the BxEvent structure.

// Event type: BX_SYNC_EVT_TICK
//
// A tick event is synchronous, sent from the simulator to the GUI.  The
// event doesn't do anything visible.  Primarily it gives the GUI a chance
// to tell the simulator to quit, if necessary.  There may be other uses
// for the tick in the future, such as giving some kind of regular
// status report or mentioning watched values that changed, but so far
// it's just for that one thing.  There is no data associated with a
// tick event.

// Event type: BX_ASYNC_EVT_KEY
//
// A key event can be sent from the VGA window to the Bochs simulator.  
// It is asynchronous.
typedef struct {
  // what was pressed?  This is a BX_KEY_* value.  For key releases,
  // BX_KEY_RELEASED is ORed with the base BX_KEY_*.
  Bit32u bx_key;
  Boolean raw_scancode;
} BxKeyEvent;

// Event type: BX_ASYNC_EVT_MOUSE
//
// A mouse event can be sent from the VGA window to the Bochs
// simulator.  It is asynchronous.  Currently unused because mouse
// events aren't implemented in our wxWindows code yet.
typedef struct {
  // type is BX_EVT_MOUSE
  Bit16u x, y;             // coordinate (vga screen?)
  Bit8u buttons;           // which buttons are pressed.
                           // bit 0: 1=left button down, 0=up
                           // bit 1: 1=right button down, 0=up
} BxMouseEvent;

// Event type: BX_SYNC_EVT_GET_PARAM, BX_ASYNC_EVT_SET_PARAM
//
// Parameter set/get events are initiated by the CUI, since Bochs can
// always access the parameters directly.  So far, I haven't used
// these event types.  In the CUI I just call
// SIM->get_param(parameter_id) to get a pointer to the bx_param_c
// object and then call the get/set methods.  This is okay for
// configuration since bochs is not running.  However it could be
// dangerous for the GUI thread to poke around in Bochs structures
// while the thread is running.  For these cases, I may have to be
// more careful and actually build get/set events and place them on
// Bochs's event queue to be processed during SIM->periodic() or
// something.
typedef struct {
  // type is BX_EVT_GET_PARAM, BX_EVT_SET_PARAM
  class bx_param_c *param;         // pointer to param structure
  AnyParamVal val;
} BxParamEvent;

// Event type: BX_SYNC_EVT_ASK_PARAM
// Synchronous event sent from the simulator to the CUI.  This tells the
// CUI to ask the user to choose the value of a parameter.  The CUI may 
// need to discover the type of parameter so that it can use the right
// kind of graphical display.  The BxParamEvent is used for these events
// too.
// FIXME: at the moment the GUI implements the ASK_PARAM event for just
// a few parameter types.  I need to implement the event for all parameter
// types.

// Event type: BX_ASYNC_EVT_VALUE_CHANGED
// 
// Asynchronous event sent from the simulator to the CUI, telling it that
// some value that it (hopefully) cares about has changed.  This isn't
// being used yet, but a good example is in a debugger interface, you might
// want to maintain a reasonably current display of the PC or some other
// simulation state.  The CUI would set some kind of event mask (which
// doesn't exist now of course) and then when certain values change, the
// simulator would send this event so that the CUI can update.  We may need
// some kind of "flow control" since the simulator will be able to produce
// new events much faster than the gui can accept them.

// Event type: BX_ASYNC_EVT_LOG_MSG   (unused)
//
// Asynchronous event from the simulator to the CUI.  When a BX_PANIC,
// BX_ERROR, BX_INFO, or BX_DEBUG is found in the simulator code, this 
// event type can be used to inform the CUI of the condition.  There is
// no point in sending messages to the CUI that will not be displayed; these
// would only slow the simulation.  So we will need some mechanism for 
// choosing what kinds of events will be delivered to the CUI.  Normally,
// you wouldn't want to look at the log unless something is going wrong.
// At that point, you might want to open up a window to watch the debug
// messages from one or two devices only.
//
// Idea: Except for panics that require user attention to continue, it
// might be most efficient to just append log messages to a file.
// When the user wants to look at the log messages, the gui can reopen
// the file (read only), skip to the end, and look backward for a
// reasonable number of lines to display (200?).  This allows it to
// skip over huge bursts of log entries without allocating memory,
// synchronizing threads, etc. for each.
typedef struct {
  Bit8u level;
  char *prefix;
  char *msg;
} BxLogMsgEvent;

// Event type: BX_SYNC_EVT_LOG_ASK
//
// This is a synchronous version of BX_ASYNC_EVT_LOG_MSG, which is used
// when the "action=ask" setting is used.  If the simulator runs into a
// panic, it sends a synchronous BX_SYNC_EVT_LOG_ASK to the CUI to be
// displayed.  The CUI shows a dialog that asks if the user wants to 
// continue, quit, etc. and sends the answer back to the simulator.
// This event also uses BxLogMsgEvent.

// Event type: BX_EVT_TOOLBAR
// Asynchronous event from the VGAW to the simulator, sent when the user
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
// parameter classes: bx_param_c and family
////////////////////////////////////////////////////////////////////
//
// All variables that can be configured through the CI are declared as
// "parameters" or objects of type bx_param_*.  There is a bx_param_*
// class for each type of data that the user would need to see and
// edit, e.g. integer, boolean, enum, string, filename, or list of
// other parameters.  The purpose of the bx_param_* class, in addition
// to storing the parameter's value, is to hold the name, description,
// and constraints on the value.  The bx_param_* class should hold
// everything that the CUI would need to display the value and allow
// the user to modify it.  For integer parameters, the minimum and
// maximum allowed value can be defined, and the base in which it
// should be displayed and interpreted.  For enums, the
// bx_param_enum_c structure includes the list of values which the
// parameter can have.
//
// Also, some parameter classes support get/set callback functions to
// allow arbitrary code to be executed when the parameter is get/set. 
// An example of where this is useful: if you disable the NE2K card,
// the set() handler for that parameter can tell the user interface
// that the NE2K's irq, I/O address, and mac address should be
// disabled (greyed out, hidden, or made inaccessible).  The get/set
// methods can also check if the set() value is acceptable using
// whatever means and override it.
//
// The parameter concept is similar to the use of parameters in JavaBeans.

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
    BX_RAW_BYTES = 1,          // use binary text editor, like MAC addr
	BX_IS_FILENAME = 2,    // 1=yes it's a filename, 0=not a filename.
	                       // Some guis have a file browser. This
			       // bit suggests that they use it.
	BX_SAVE_FILE_DIALOG = 4    // Use save dialog opposed to open file dialog
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

// Declare a filename class.  It is identical to a string, except that
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
    // not the menu shows a "Return to parent menu" choice or not.
    BX_SHOW_PARENT = (1<<0),
    // Some lists are best displayed shown as menus, others as a series of
    // related questions.  This bit suggests to the CUI that the
    // series of questions format is preferred.
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
#define BX_FLOPPY_360K   15 // 360K  5.25"
#define BX_FLOPPY_LAST   15 // last legal value of floppy type
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
  bx_param_enum_c *Odevtype;
  bx_param_string_c *Opath;
  bx_param_enum_c *Otype;
  bx_param_enum_c *Ostatus;
  } bx_floppy_options;

typedef struct {
  bx_param_bool_c *Opresent;
  bx_param_string_c *Opath;
  bx_param_num_c *Ocylinders;
  bx_param_num_c *Oheads;
  bx_param_num_c *Ospt;
  } bx_disk_options;

typedef struct {
  bx_param_bool_c *Oenabled;
  bx_param_string_c *Odev;
  } bx_serial_options;

struct bx_cdrom_options
{
  bx_param_bool_c *Opresent;
  bx_param_string_c *Opath;
  bx_param_enum_c *Ostatus;
};


////////////////////////////////////////////////////////////////////
// base class simulator interface, contains just virtual functions.
// I'm not longer sure that having a base class is going to be of any
// use... -Bryce

#include <setjmp.h>

class bx_simulator_interface_c {
public:
  bx_simulator_interface_c ();
  virtual void set_quit_context (jmp_buf *context) {}
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
  // and free up memory, then sends a notify message to the CUI 
  // telling it that bochs has stopped.
  virtual void quit_sim (int code) {}

  virtual int get_default_rc (char *path, int len) {return -1;}
  virtual int read_rc (char *path) {return -1;}
  virtual int write_rc (char *rc, int overwrite) {return -1;}
  virtual int get_log_file (char *path, int len) {return -1;}
  virtual int set_log_file (char *path) {return -1;}
  virtual int get_log_prefix (char *prefix, int len) {return -1;}
  virtual int set_log_prefix (char *prefix) {return -1;}
  virtual int get_floppy_options (int drive, bx_floppy_options *out) {return -1;}
  virtual int get_cdrom_options (int drive, bx_cdrom_options *out) {return -1;}
  virtual char *get_floppy_type_name (int type) {return NULL;}

  // The CUI calls set_notify_callback to register its event handler function.
  // This event handler function is called whenever the simulator needs to
  // send an event to the CUI.  For example, if the simulator hits a panic and
  // wants to ask the user how to proceed, it would call the CUI event handler
  // to ask the CUI to display a dialog.
  //
  // NOTE: At present, the standard VGAW buttons (floppy, snapshot, power,
  // etc.) are displayed and handled by gui.cc, not by the CI or siminterface.
  // gui.cc uses its own callback functions to implement the behavior of
  // the buttons.  Some of these implementations call the siminterface.
  typedef BxEvent* (*sim_interface_callback_t)(void *theclass, BxEvent *event);
  virtual void set_notify_callback (sim_interface_callback_t func, void *arg) {}

  // send an event from the simulator to the CUI.
  virtual BxEvent* sim_to_cui_event (BxEvent *event) {return NULL;}

  // called from simulator when it hits serious errors, to ask if the user
  // wants to continue or not
  virtual int log_msg (const char *prefix, int level, char *msg) {return -1;}

  // tell the CUI to ask the user for the value of a parameter.
  virtual int ask_param (bx_id param) {return -1;}

  // ask the user for a pathname
  virtual int ask_filename (char *filename, int maxlen, char *prompt, char *the_default, int flags) {return -1;}
  // called at a regular interval, currently by the keyboard handler.
  virtual void periodic () {}
};

extern bx_simulator_interface_c *SIM;

extern void bx_init_siminterface ();
extern void bx_init_main (int argc, char *argv[]);
extern int bx_continue_after_config_interface (int argc, char *argv[]);
