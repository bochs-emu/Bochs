/////////////////////////////////////////////////////////////////////////
// $Id: siminterface.h,v 1.213 2007-10-14 19:04:51 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
// Intro to siminterface by Bryce Denney:
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
// server that you can connect to with a web browser.  As a result the
// interface to the CI was designed so that the user interface of the CI 
// could be replaced easily at compile time, or maybe even at runtime via
// a plugin architecture.  To this end, we kept a clear separation between
// the user interface code and the siminterface, the code that interfaces with
// the simulator.  The same siminterface is used all the time, while 
// different implementations of the CI can be switched in reasonably easily.
// Only the CI code uses library specific graphics and I/O functions; the
// siminterface deals in portable abstractions and callback functions.
// The first CI implementation was a series of text mode menus implemented in
// textconfig.cc.
//
// The configuration interface MUST use the siminterface methods to access the
// simulator.  It should not modify settings in some device with code like
// bx_floppy.s.media[2].heads = 17.  If such access is needed, then a
// siminterface method should be written to make the change on the CI's behalf.
// This separation is enforced by the fact that the CI does not even include
// bochs.h.  You'll notice that textconfig.cc includes osdep.h, textconfig.h,
// and siminterface.h, so it doesn't know what bx_floppy or bx_cpu_c are.
// I'm sure some people will say is overly restrictive and/or annoying.  When I
// set it up this way, we were still talking about making the CI in a seperate
// process, where direct method calls would be impossible.  Also, we have been
// considering turning devices into plugin modules which are dynamically
// linked.  Any direct references to something like bx_floppy.s.media[2].heads
// would have to be reworked before a plugin interface was possible as well.
//
// The siminterface is the glue between the CI and the simulator.  There is
// just one global instance of the siminterface object, which can be referred
// to by the global variable bx_simulator_interface_c *SIM.  The base class
// bx_simulator_interface_c, contains only virtual functions and it defines the
// interface that the CI is allowed to use.  In siminterface.cc, a class
// called bx_real_sim_c is defined with bx_simulator_interface_c as its parent
// class.  Bx_real_sim_c implements each of the functions.  The separation into
// parent class and child class leaves the possibility of making a different
// child class that talks to the simulator in a different way (networking for
// example).  If you were writing a user interface in a separate process, you
// could define a subclass of bx_simulator_interface_c called
// bx_siminterface_proxy_c which opens up a network port and turns all method
// calls into network sends and receives.  Because the interface is defined
// entirely by the base class, the code that calls the methods would not know
// the difference.
//
// An important part of the siminterface implementation is the use of parameter
// classes, or bx_param_*.  The parameter classes are described below, where
// they are declared.  Search for "parameter classes" below for details.
//
// Also this header file declares data structures for certain events that pass
// between the siminterface and the CI.  Search for "event structures" below.


//////////////////////////////////////////////////////
// BX_USE_TEXTCONFIG should be set to 1 when the text mode configuration interface
// is compiled in.  This gives each type of parameter a text_print and text_ask
// method (defined in gui/textconfig.cc) so that you can call text_ask() on any
// kind of parameter to ask the user to edit the value.
//
// I have been considering whether to use the same strategy for the
// wxWidgets interface, but I'm not sure if I like it.  One problem is
// that in order to declare member functions that are useful for
// wxWidgets, the wxWidgets header files would have to be included
// before the param object definitions.  That means that all the
// wxWidgets headers would have be included when compiling every
// single bochs file.  One of the things I like about the separation
// between the simulator and CI is that the two parts can be
// compiled without any knowledge of the other.  Bochs doesn't include
// <wx.h>, and the wxWidgets CI (wxmain.cc) doesn't need to include <bochs.h>.
// Aside from making compiles faster, this enforces the use of the siminterface
// so it keeps the interface clean (important when we may have multiple UI
// implementations for example).  This argues for keeping UI-specific
// structures out of the simulator interface.  It certainly works ok for the
// text interface, but that's because FILE* is standard and portable.

//////////////////////////////////////////////////////

// list of possible types for bx_param_c and descendant objects
typedef enum {
  BXT_OBJECT = 201,
  BXT_PARAM,
  BXT_PARAM_NUM,
  BXT_PARAM_BOOL,
  BXT_PARAM_ENUM,
  BXT_PARAM_STRING,
  BXT_PARAM_DATA,
  BXT_LIST
} bx_objtype;

// define parameter path names.  These names give the location in the
// parameter tree where each can be found.  The names correspond to
// the old BXP_* enum values, which have been eliminated.
#define BXPN_SEL_CONFIG_INTERFACE        "general.config_interface"
#define BXPN_BOCHS_START                 "general.start_mode"
#define BXPN_BOCHS_BENCHMARK             "general.benchmark"
#define BXPN_RESTORE_FLAG                "general.restore"
#define BXPN_RESTORE_PATH                "general.restore_path"
#define BXPN_DEBUG_RUNNING               "general.debug_running"
#define BXPN_CPU_NPROCESSORS             "cpu.n_processors"
#define BXPN_CPU_NCORES                  "cpu.n_cores"
#define BXPN_CPU_NTHREADS                "cpu.n_threads"
#define BXPN_IPS                         "cpu.ips"
#define BXPN_SMP_QUANTUM                 "cpu.quantum"
#define BXPN_RESET_ON_TRIPLE_FAULT       "cpu.reset_on_triple_fault"
#define BXPN_MEM_SIZE                    "memory.standard.ram.size"
#define BXPN_ROM_PATH                    "memory.standard.rom.path"
#define BXPN_ROM_ADDRESS                 "memory.standard.rom.addr"
#define BXPN_VGA_ROM_PATH                "memory.standard.vgarom.path"
#define BXPN_OPTROM1_PATH                "memory.optrom.1.path"
#define BXPN_OPTROM1_ADDRESS             "memory.optrom.1.addr"
#define BXPN_OPTROM2_PATH                "memory.optrom.2.path"
#define BXPN_OPTROM2_ADDRESS             "memory.optrom.2.addr"
#define BXPN_OPTROM3_PATH                "memory.optrom.3.path"
#define BXPN_OPTROM3_ADDRESS             "memory.optrom.3.addr"
#define BXPN_OPTROM4_PATH                "memory.optrom.4.path"
#define BXPN_OPTROM4_ADDRESS             "memory.optrom.4.addr"
#define BXPN_OPTRAM1_PATH                "memory.optram.1.path"
#define BXPN_OPTRAM1_ADDRESS             "memory.optram.1.addr"
#define BXPN_OPTRAM2_PATH                "memory.optram.2.path"
#define BXPN_OPTRAM2_ADDRESS             "memory.optram.2.addr"
#define BXPN_OPTRAM3_PATH                "memory.optram.3.path"
#define BXPN_OPTRAM3_ADDRESS             "memory.optram.3.addr"
#define BXPN_OPTRAM4_PATH                "memory.optram.4.path"
#define BXPN_OPTRAM4_ADDRESS             "memory.optram.4.addr"
#define BXPN_CLOCK_SYNC                  "clock_cmos.clock_sync"
#define BXPN_CLOCK_TIME0                 "clock_cmos.time0"
#define BXPN_CMOSIMAGE_ENABLED           "clock_cmos.cmosimage.enabled"
#define BXPN_CMOSIMAGE_PATH              "clock_cmos.cmosimage.path"
#define BXPN_CMOSIMAGE_RTC_INIT          "clock_cmos.cmosimage.rtc_init"
#define BXPN_I440FX_SUPPORT              "pci.i440fx_support"
#define BXPN_PCIDEV_VENDOR               "pci.pcidev.vendor"
#define BXPN_PCIDEV_DEVICE               "pci.pcidev.device"
#define BXPN_SEL_DISPLAY_LIBRARY         "display.display_library"
#define BXPN_DISPLAYLIB_OPTIONS          "display.displaylib_options"
#define BXPN_PRIVATE_COLORMAP            "display.private_colormap"
#define BXPN_FULLSCREEN                  "display.fullscreen"
#define BXPN_SCREENMODE                  "display.screenmode"
#define BXPN_VGA_EXTENSION               "display.vga_extension"
#define BXPN_VGA_UPDATE_INTERVAL         "display.vga_update_interval"
#define BXPN_KBD_TYPE                    "keyboard_mouse.keyboard.type"
#define BXPN_KBD_SERIAL_DELAY            "keyboard_mouse.keyboard.serial_delay"
#define BXPN_KBD_PASTE_DELAY             "keyboard_mouse.keyboard.paste_delay"
#define BXPN_KBD_USEMAPPING              "keyboard_mouse.keyboard.use_mapping"
#define BXPN_KBD_KEYMAP                  "keyboard_mouse.keyboard.keymap"
#define BXPN_USER_SHORTCUT               "keyboard_mouse.keyboard.user_shortcut"
#define BXPN_MOUSE_TYPE                  "keyboard_mouse.mouse.type"
#define BXPN_MOUSE_ENABLED               "keyboard_mouse.mouse.enabled"
#define BXPN_BOOTDRIVE1                  "boot_params.boot_drive1"
#define BXPN_BOOTDRIVE2                  "boot_params.boot_drive2"
#define BXPN_BOOTDRIVE3                  "boot_params.boot_drive3"
#define BXPN_FLOPPYSIGCHECK              "boot_params.floppy_sig_check"
#define BXPN_LOAD32BITOS_WHICH           "boot_params.load32bitos.which"
#define BXPN_LOAD32BITOS_PATH            "boot_params.load32bitos.path"
#define BXPN_LOAD32BITOS_IOLOG           "boot_params.load32bitos.iolog"
#define BXPN_LOAD32BITOS_INITRD          "boot_params.load32bitos.initrd"
#define BXPN_FLOPPYA                     "floppy.0"
#define BXPN_FLOPPYA_DEVTYPE             "floppy.0.devtype"
#define BXPN_FLOPPYA_PATH                "floppy.0.path"
#define BXPN_FLOPPYA_TYPE                "floppy.0.type"
#define BXPN_FLOPPYA_STATUS              "floppy.0.status"
#define BXPN_FLOPPYB                     "floppy.1"
#define BXPN_FLOPPYB_DEVTYPE             "floppy.1.devtype"
#define BXPN_FLOPPYB_PATH                "floppy.1.path"
#define BXPN_FLOPPYB_TYPE                "floppy.1.type"
#define BXPN_FLOPPYB_STATUS              "floppy.1.status"
#define BXPN_ATA0_RES                    "ata.0.resources"
#define BXPN_ATA1_RES                    "ata.1.resources"
#define BXPN_ATA2_RES                    "ata.2.resources"
#define BXPN_ATA3_RES                    "ata.3.resources"
#define BXPN_ATA0_ENABLED                "ata.0.resources.enabled"
#define BXPN_ATA1_ENABLED                "ata.1.resources.enabled"
#define BXPN_ATA0_MASTER                 "ata.0.master"
#define BXPN_ATA1_MASTER                 "ata.1.master"
#define BXPN_ATA2_MASTER                 "ata.2.master"
#define BXPN_ATA3_MASTER                 "ata.3.master"
#define BXPN_ATA0_SLAVE                  "ata.0.slave"
#define BXPN_ATA1_SLAVE                  "ata.1.slave"
#define BXPN_ATA2_SLAVE                  "ata.2.slave"
#define BXPN_ATA3_SLAVE                  "ata.3.slave"
#define BXPN_USB1_ENABLED                "ports.usb.1.enabled"
#define BXPN_USB1_PORT1                  "ports.usb.1.port1"
#define BXPN_USB1_PORT2                  "ports.usb.1.port2"
#define BXPN_NE2K                        "network.ne2k"
#define BXPN_NE2K_ENABLED                "network.ne2k.enabled"
#define BXPN_PNIC                        "network.pnic"
#define BXPN_PNIC_ENABLED                "network.pnic.enabled"
#define BXPN_SB16                        "sound.sb16"
#define BXPN_SB16_ENABLED                "sound.sb16.enabled"
#define BXPN_SB16_MIDIFILE               "sound.sb16.midifile"
#define BXPN_SB16_WAVEFILE               "sound.sb16.wavefile"
#define BXPN_SB16_DMATIMER               "sound.sb16.dmatimer"
#define BXPN_SB16_LOGLEVEL               "sound.sb16.loglevel"
#define BXPN_TEXT_SNAPSHOT_CHECK         "misc.text_snapshot_check"
#define BXPN_GDBSTUB                     "misc.gdbstub"
#define BXPN_LOG_FILENAME                "log.filename"
#define BXPN_LOG_PREFIX                  "log.prefix"
#define BXPN_DEBUGGER_LOG_FILENAME       "log.debugger_filename"
#define BXPN_MENU_DISK                   "menu.disk"
#define BXPN_MENU_MEMORY                 "menu.memory"
#define BXPN_MENU_RUNTIME                "menu.runtime"
#define BXPN_WX_KBD_STATE                "wxdebug.keyboard"
#define BXPN_WX_CPU_STATE                "wxdebug.cpu"
#define BXPN_WX_CPU0_STATE               "wxdebug.cpu.0"
#define BXPN_WX_CPU0_EFLAGS_IOPL         "wxdebug.cpu.0.IOPL"

// base value for generated new parameter id
#define BXP_NEW_PARAM_ID 1001

typedef enum {
  BX_TOOLBAR_UNDEFINED,
  BX_TOOLBAR_FLOPPYA,
  BX_TOOLBAR_FLOPPYB,
  BX_TOOLBAR_CDROMD,
  BX_TOOLBAR_RESET,
  BX_TOOLBAR_POWER,
  BX_TOOLBAR_SAVE_RESTORE,
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
  LOGLEV_PASS,
  N_LOGLEV
} bx_log_levels;

// types of reset
#define BX_RESET_SOFTWARE 10
#define BX_RESET_HARDWARE 11

//cdrom
#define BX_EJECTED   10
#define BX_INSERTED  11

// boot devices (using the same values as the rombios)
#define BX_BOOT_NONE    0
#define BX_BOOT_FLOPPYA 1
#define BX_BOOT_DISKC   2
#define BX_BOOT_CDROM   3
#define BX_BOOT_NETWORK 4

// loader hack
#define Load32bitOSNone        0
#define Load32bitOSLinux       1
#define Load32bitOSNullKernel  2 // being developed for plex86
#define Load32bitOSLast        2

///////////////////////////////////////////////////////////////////
// event structures for communication between simulator and CI
///////////////////////////////////////////////////////////////////
// Because the CI (configuration interface) might be in a different
// thread or even a different process, we pass events encoded in data
// structures to it instead of just calling functions.  Each type of
// event is declared as a different structure, and then all those
// structures are squished into a union in BxEvent.  (BTW, this is
// almost exactly how X windows event structs work.)
//
// These are simple structs, unblemished by C++ methods and tricks.
// No matter what event type it is, we allocate a BxEvent for each
// one, as opposed to trying to save a few bytes on small events by
// allocating only the bytes necessary for it.  This makes it easy and
// fast to store events in a queue, like this
//   BxEvent event_queue[MAX_EVENTS];
//
// Events come in two varieties: synchronous and asynchronous.  We
// have to worry about sync and async events because the CI and the
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
// async event: In the wxWidgets implementation, both the CI and the
// VGAW operate in the wxWidgets GUI thread.  When the user presses a
// key, wxWidgets sends a wxKeyEvent to the VGAW event handler code in
// wx.cc.  The VGAW handler then builds a BxEvent with
// type=BX_ASYNC_EVT_KEY, and fills in the bx_key and raw_scancode
// fields.  The asynchronous event is placed on the event_queue for
// the simulator, then the VGAW handler returns.  (With wxWidgets and
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
// transaction can be complicated if the simulation and CI are not
// in the same thread, but the behavior is as described.
//

///// types and definitions used in event structures

#define BX_EVT_IS_ASYNC(type) ((type) > __ALL_EVENTS_BELOW_ARE_ASYNC__)

typedef enum {
  __ALL_EVENTS_BELOW_ARE_SYNCHRONOUS__ = 2000,
  BX_SYNC_EVT_GET_PARAM,          // CI -> simulator -> CI
  BX_SYNC_EVT_ASK_PARAM,          // simulator -> CI -> simulator
  BX_SYNC_EVT_TICK,               // simulator -> CI, wait for response.
  BX_SYNC_EVT_LOG_ASK,            // simulator -> CI, wait for response.
  BX_SYNC_EVT_GET_DBG_COMMAND,    // simulator -> CI, wait for response.
  __ALL_EVENTS_BELOW_ARE_ASYNC__,
  BX_ASYNC_EVT_KEY,               // vga window -> simulator
  BX_ASYNC_EVT_MOUSE,             // vga window -> simulator
  BX_ASYNC_EVT_SET_PARAM,         // CI -> simulator
  BX_ASYNC_EVT_LOG_MSG,           // simulator -> CI
  BX_ASYNC_EVT_DBG_MSG,           // simulator -> CI
  BX_ASYNC_EVT_VALUE_CHANGED,     // simulator -> CI
  BX_ASYNC_EVT_TOOLBAR,           // CI -> simulator
  BX_ASYNC_EVT_REFRESH            // simulator -> CI
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
  bx_bool raw_scancode;
} BxKeyEvent;

// Event type: BX_ASYNC_EVT_MOUSE
//
// A mouse event can be sent from the VGA window to the Bochs
// simulator.  It is asynchronous.  Currently unused because mouse
// events aren't implemented in our wxWidgets code yet.
typedef struct {
  // type is BX_EVT_MOUSE
  Bit16s dx, dy;           // mouse motion delta
  Bit8u buttons;           // which buttons are pressed.
                           // bit 0: 1=left button down, 0=up
                           // bit 1: 1=right button down, 0=up
} BxMouseEvent;

// Event type: BX_SYNC_EVT_GET_PARAM, BX_ASYNC_EVT_SET_PARAM
//
// Parameter set/get events are initiated by the CI, since Bochs can
// always access the parameters directly.  So far, I haven't used
// these event types.  In the CI I just call
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
// Synchronous event sent from the simulator to the CI.  This tells the
// CI to ask the user to choose the value of a parameter.  The CI may 
// need to discover the type of parameter so that it can use the right
// kind of graphical display.  The BxParamEvent is used for these events
// too.
// FIXME: at the moment the GUI implements the ASK_PARAM event for just
// a few parameter types.  I need to implement the event for all parameter
// types.

// Event type: BX_ASYNC_EVT_VALUE_CHANGED
// 
// Asynchronous event sent from the simulator to the CI, telling it that
// some value that it (hopefully) cares about has changed.  This isn't
// being used yet, but a good example is in a debugger interface, you might
// want to maintain a reasonably current display of the PC or some other
// simulation state.  The CI would set some kind of event mask (which
// doesn't exist now of course) and then when certain values change, the
// simulator would send this event so that the CI can update.  We may need
// some kind of "flow control" since the simulator will be able to produce
// new events much faster than the gui can accept them.

// Event type: BX_ASYNC_EVT_LOG_MSG   (unused)
//
// Asynchronous event from the simulator to the CI.  When a BX_PANIC,
// BX_ERROR, BX_INFO, or BX_DEBUG is found in the simulator code, this 
// event type can be used to inform the CI of the condition.  There is
// no point in sending messages to the CI that will not be displayed; these
// would only slow the simulation.  So we will need some mechanism for 
// choosing what kinds of events will be delivered to the CI.  Normally,
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
  const char *prefix;
  const char *msg;
} BxLogMsgEvent;

// Event type: BX_ASYNC_EVT_DBG_MSG   (unused)
//
// Also uses BxLogMsgEvent, but this is a message to be displayed in
// the debugger history window.

// Event type: BX_SYNC_EVT_LOG_ASK
//
// This is a synchronous version of BX_ASYNC_EVT_LOG_MSG, which is used
// when the "action=ask" setting is used.  If the simulator runs into a
// panic, it sends a synchronous BX_SYNC_EVT_LOG_ASK to the CI to be
// displayed.  The CI shows a dialog that asks if the user wants to 
// continue, quit, etc. and sends the answer back to the simulator.
// This event also uses BxLogMsgEvent.
enum {
  BX_LOG_ASK_CHOICE_CONTINUE,
  BX_LOG_ASK_CHOICE_CONTINUE_ALWAYS,
  BX_LOG_ASK_CHOICE_DIE,
  BX_LOG_ASK_CHOICE_DUMP_CORE,
  BX_LOG_ASK_CHOICE_ENTER_DEBUG,
  BX_LOG_ASK_N_CHOICES,
  BX_LOG_NOTIFY_FAILED
};

// Event type: BX_SYNC_EVT_GET_DBG_COMMAND
//
// This is a synchronous event sent from the simulator to the debugger
// requesting the next action.  In a text mode debugger, this would prompt
// the user for the next command.  When a new command is ready, the
// synchronous event is sent back with its fields filled in.
typedef struct {
  char *command;   // null terminated string. allocated by debugger interface
                   // with new operator, freed by simulator with delete.
} BxDebugCommand;



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
    BxDebugCommand debugcmd;
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
// everything that the CI would need to display the value and allow
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

class bx_object_c;
class bx_param_c;
class bx_param_num_c;
class bx_param_enum_c;
class bx_param_bool_c;
class bx_param_string_c;
class bx_param_filename_c;
class bx_list_c;

class BOCHSAPI bx_object_c {
private:
  Bit32u id;
  bx_objtype type;
protected:
  void set_type(bx_objtype _type) { type = _type; }
public:
  bx_object_c(Bit32u _id): id(_id), type(BXT_OBJECT) {}
  virtual ~bx_object_c() {}
  Bit32u get_id() const { return id; }
  Bit8u get_type() const { return type; }
};

#define BASE_DEC 10
#define BASE_HEX 16

class BOCHSAPI bx_param_c : public bx_object_c {
  BOCHSAPI_CYGONLY static const char *default_text_format;
protected:
  bx_list_c *parent;
  char *name;
  char *description;
  char *label; // label string for text menus and gui dialogs
  const char *text_format;  // printf format string. %d for ints, %s for strings, etc.
  const char *long_text_format;  // printf format string. %d for ints, %s for strings, etc.
  char *ask_format;  // format string for asking for a new value
  char *group_name;  // name of the group the param belongs to
  int runtime_param;
  int enabled;
public:
  bx_param_c(Bit32u id, const char *name, const char *description);
  bx_param_c(Bit32u id, const char *name, const char *label, const char *description);
  virtual ~bx_param_c();
  bx_param_c *get_parent() { return (bx_param_c *) parent; }
  int get_param_path(char *path_out, int maxlen);
  void set_format(const char *format) {text_format = format;}
  const char *get_format() const {return text_format;}
  void set_long_format(const char *format) {long_text_format = format;}
  const char *get_long_format() const {return long_text_format;}
  void set_ask_format(const char *format);
  const char *get_ask_format() const {return ask_format;}
  void set_label(const char *text);
  void set_description(const char *text);
  const char *get_label() const {return label;}
  void set_runtime_param(int val) { runtime_param = val; }
  int get_runtime_param() { return runtime_param; }
  void set_group(const char *group);
  const char *get_group() const {return group_name;}
  const char *get_name() const { return name; }
  const char *get_description() const { return description; }
  int get_enabled() const { return enabled; }
  virtual void set_enabled(int enabled) { this->enabled = enabled; }
  virtual void reset() {}
  int getint() const {return -1;}
  static const char* set_default_format(const char *f);
  static const char *get_default_format() { return default_text_format; }
  virtual bx_list_c *get_dependent_list() { return NULL; }
#if BX_USE_TEXTCONFIG
  virtual void text_print(FILE *fp) {}
  virtual int text_ask(FILE *fpin, FILE *fpout) {return -1;}
#endif
};

typedef Bit64s (*param_event_handler)(class bx_param_c *, int set, Bit64s val);
typedef Bit64s (*param_sr_handler)(void *devptr, class bx_param_c *, Bit64s val);
typedef int (*param_enable_handler)(class bx_param_c *, int en);

class BOCHSAPI bx_param_num_c : public bx_param_c {
  BOCHSAPI_CYGONLY static Bit32u default_base;
  // The dependent_list is initialized to NULL.  If dependent_list is modified
  // to point to a bx_list_c of other parameters, the set() method of
  // bx_param_bool_c will enable those parameters when this bool is true, and
  // disable them when this bool is false.
  bx_list_c *dependent_list;
  void update_dependents();
protected:
  Bit64s min, max, initial_val;
  union _uval_ {
    Bit64s number;   // used by bx_param_num_c
    Bit64s *p64bit;  // used by bx_shadow_num_c
    Bit32s *p32bit;  // used by bx_shadow_num_c
    Bit16s *p16bit;  // used by bx_shadow_num_c
    Bit8s  *p8bit;    // used by bx_shadow_num_c
    bx_bool *pbool;  // used by bx_shadow_bool_c
  } val;
  param_event_handler handler;
  void *sr_devptr;
  param_sr_handler save_handler;
  param_sr_handler restore_handler;
  param_enable_handler enable_handler;
  int base;
  Bit32u options;
  bx_bool is_shadow;
public:
  enum {
    // When a bx_param_num_c is displayed in dialog, USE_SPIN_CONTROL controls
    // whether a spin control should be used instead of a simple text control.
    USE_SPIN_CONTROL = (1<<0)
  } bx_numopt_bits;
  bx_param_num_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      Bit64s min, Bit64s max, Bit64s initial_val,
      bx_bool is_shadow = 0);
  virtual void reset() { val.number = initial_val; }
  void set_handler(param_event_handler handler);
  void set_sr_handlers(void *devptr, param_sr_handler save, param_sr_handler restore);
  void set_enable_handler(param_enable_handler handler) { enable_handler = handler; }
  virtual bx_list_c *get_dependent_list() { return dependent_list; }
  void set_dependent_list(bx_list_c *l);
  virtual void set_enabled(int enabled);
  virtual Bit32s get() { return (Bit32s) get64(); }
  virtual Bit64s get64();
  virtual void set(Bit64s val);
  void set_base(int base) { this->base = base; }
  void set_initial_val(Bit64s initial_val);
  int get_base() const { return base; }
  void set_range(Bit64u min, Bit64u max);
  Bit64s get_min() { return min; }
  Bit64s get_max() { return max; }
  static Bit32u set_default_base(Bit32u val);
  static Bit32u get_default_base() { return default_base; }
  void set_options(Bit32u options) { this->options = options; }
  Bit32u get_options() const { return options; }
#if BX_USE_TEXTCONFIG
  virtual void text_print(FILE *fp);
  virtual int text_ask(FILE *fpin, FILE *fpout);
#endif
};

// a bx_shadow_num_c is like a bx_param_num_c except that it doesn't
// store the actual value with its data. Instead, it uses val.p32bit
// to keep a pointer to the actual data.  This is used to register
// existing variables as parameters, without having to access it via
// set/get methods.
class BOCHSAPI bx_shadow_num_c : public bx_param_num_c {
  Bit8u varsize;   // must be 64, 32, 16, or 8
  Bit8u lowbit;   // range of bits associated with this param
  Bit64u mask;     // mask is ANDed with value before it is returned from get
public:
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit64s *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 63,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit64u *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 63,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit32s *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 31,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit32u *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 31,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit16s *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 15,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit16u *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 15,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit8s *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 7,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit8u *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 7,
      Bit8u lowbit = 0);
  virtual Bit64s get64();
  virtual void set(Bit64s val);
  virtual void reset();
};

class BOCHSAPI bx_param_bool_c : public bx_param_num_c {
  // many boolean variables are used to enable/disable modules.  In the
  // user interface, the enable variable should enable/disable all the
  // other parameters associated with that module.
public:
  bx_param_bool_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      Bit64s initial_val,
      bx_bool is_shadow = 0);
#if BX_USE_TEXTCONFIG
  virtual void text_print(FILE *fp);
  virtual int text_ask(FILE *fpin, FILE *fpout);
#endif
};

// a bx_shadow_bool_c is a shadow param based on bx_param_bool_c.
class BOCHSAPI bx_shadow_bool_c : public bx_param_bool_c {
  // each bit of a bitfield can be a separate value.  bitnum tells which
  // bit is used.  get/set will only modify that bit.
  Bit8u bitnum;
public:
  bx_shadow_bool_c(bx_param_c *parent,
      const char *name,
      const char *label,
      bx_bool *ptr_to_real_val,
      Bit8u bitnum = 0);
  bx_shadow_bool_c(bx_param_c *parent,
      const char *name,
      bx_bool *ptr_to_real_val,
      Bit8u bitnum = 0);
  virtual Bit64s get64();
  virtual void set(Bit64s val);
};


class BOCHSAPI bx_param_enum_c : public bx_param_num_c {
  char **choices;
public:
  bx_param_enum_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      char **choices,
      Bit64s initial_val,
      Bit64s value_base = 0);
  char *get_choice(int n) { return choices[n]; }
  char *get_selected() { return choices[val.number - min]; }
  int find_by_name(const char *string);
  bx_bool set_by_name(const char *string);
#if BX_USE_TEXTCONFIG
  virtual void text_print(FILE *fp);
  virtual int text_ask(FILE *fpin, FILE *fpout);
#endif
};

typedef const char* (*param_string_event_handler)(class bx_param_string_c *, int set, const char *val, int maxlen);

class BOCHSAPI bx_param_string_c : public bx_param_c {
  int maxsize;
  char *val, *initial_val;
  param_string_event_handler handler;
  param_enable_handler enable_handler;
  bx_param_num_c *options;
  char separator;
public:
  enum {
    RAW_BYTES = 1,         // use binary text editor, like MAC addr
    IS_FILENAME = 2,       // 1=yes it's a filename, 0=not a filename.
                           // Some guis have a file browser. This
                           // bit suggests that they use it.
    SAVE_FILE_DIALOG = 4,  // Use save dialog opposed to open file dialog
    SELECT_FOLDER_DLG = 8  // Use folder selection dialog
  } bx_string_opt_bits;
  bx_param_string_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      const char *initial_val,
      int maxsize=-1);
  virtual ~bx_param_string_c();
  virtual void reset();
  void set_handler(param_string_event_handler handler);
  void set_enable_handler(param_enable_handler handler);
  virtual void set_enabled(int enabled);
  Bit32s get(char *buf, int len);
  char *getptr() {return val; }
  void set(const char *buf);
  bx_bool equals(const char *buf);
  bx_param_num_c *get_options() { return options; }
  void set_separator(char sep) {separator = sep; }
  char get_separator() const {return separator; }
  int get_maxsize() const {return maxsize; }
  void set_initial_val(const char *buf);
#if BX_USE_TEXTCONFIG
  virtual void text_print(FILE *fp);
  virtual int text_ask(FILE *fpin, FILE *fpout);
#endif
};

// Declare a filename class.  It is identical to a string, except that
// it initializes the options differently.  This is just a shortcut
// for declaring a string param and setting the options with IS_FILENAME.
class BOCHSAPI bx_param_filename_c : public bx_param_string_c {
public:
  bx_param_filename_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      const char *initial_val,
      int maxsize=-1);
};

class BOCHSAPI bx_shadow_data_c : public bx_param_c {
  Bit32u data_size;
  Bit8u *data_ptr;
public:
  bx_shadow_data_c(bx_param_c *parent,
      const char *name,
      Bit8u *ptr_to_data,
      Bit32u data_size);
  Bit8u *getptr() {return data_ptr;}
  Bit32u get_size() const {return data_size;}
};

#define BX_DEFAULT_LIST_SIZE 6

class BOCHSAPI bx_list_c : public bx_param_c {
protected:
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
  void init(const char *list_title);
public:
  enum {
    // When a bx_list_c is displayed as a menu, SHOW_PARENT controls whether or
    // not the menu shows a "Return to parent menu" choice or not.
    SHOW_PARENT = (1<<0),
    // Some lists are best displayed shown as menus, others as a series of
    // related questions.  This bit suggests to the CI that the series of
    // questions format is preferred.
    SERIES_ASK = (1<<1),
    // When a bx_list_c is displayed in a dialog, USE_TAB_WINDOW suggests
    // to the CI that each item in the list should be shown as a separate
    // tab.  This would be most appropriate when each item is another list
    // of parameters.
    USE_TAB_WINDOW = (1<<2),
    // When a bx_list_c is displayed in a dialog, the list name is used as the
    // label of the group box if USE_BOX_TITLE is set. This is only necessary if
    // more than one list appears in a dialog box.
    USE_BOX_TITLE = (1<<3),
    // When a bx_list_c is displayed as a menu, SHOW_GROUP_NAME controls whether
    // or not the name of group the item belongs to is added to the name of the
    // item (used in the runtime menu).
    SHOW_GROUP_NAME = (1<<4)
  } bx_listopt_bits;
  bx_list_c(bx_param_c *parent, int maxsize);
  bx_list_c(bx_param_c *parent, const char *name, int maxsize);
  bx_list_c(bx_param_c *parent, const char *name, char *title, int maxsize = BX_DEFAULT_LIST_SIZE);
  bx_list_c(bx_param_c *parent, const char *name, char *title, bx_param_c **init_list);
  virtual ~bx_list_c();
  bx_list_c *clone();
  void add(bx_param_c *param);
  bx_param_c *get(int index);
  bx_param_c *get_by_name(const char *name);
  int get_size() const { return size; }
  bx_param_num_c *get_options() { return options; }
  bx_param_num_c *get_choice() { return choice; }
  bx_param_string_c *get_title() { return title; }
  void set_parent(bx_param_c *newparent);
  bx_param_c *get_parent() { return parent; }
  virtual void reset();
  virtual void clear();
#if BX_USE_TEXTCONFIG
  virtual void text_print(FILE *);
  virtual int text_ask(FILE *fpin, FILE *fpout);
#endif
};

////////////////////////////////////////////////////////////////


// These are the different start modes.
enum {
  // Just start the simulation without running the configuration interface
  // at all, unless something goes wrong.
  BX_QUICK_START = 200,
  // Run the configuration interface.  The default action will be to load a
  // configuration file.  This makes sense if a config file could not be
  // loaded, either because it wasn't found or because it had errors.
  BX_LOAD_START,
  // Run the configuration interface.  The default action will be to
  // edit the configuration.
  BX_EDIT_START,
  // Run the configuration interface, but make the default action be to
  // start the simulation.
  BX_RUN_START
};

enum {
  BX_MOUSE_TYPE_NONE,
  BX_MOUSE_TYPE_PS2,
  BX_MOUSE_TYPE_IMPS2,
#if BX_SUPPORT_BUSMOUSE
  BX_MOUSE_TYPE_BUS,
#endif
  BX_MOUSE_TYPE_SERIAL,
  BX_MOUSE_TYPE_SERIAL_WHEEL,
  BX_MOUSE_TYPE_SERIAL_MSYS
};

#define BX_FLOPPY_NONE   10 // floppy not present
#define BX_FLOPPY_1_2    11 // 1.2M  5.25"
#define BX_FLOPPY_1_44   12 // 1.44M 3.5"
#define BX_FLOPPY_2_88   13 // 2.88M 3.5"
#define BX_FLOPPY_720K   14 // 720K  3.5"
#define BX_FLOPPY_360K   15 // 360K  5.25"
#define BX_FLOPPY_160K   16 // 160K  5.25"
#define BX_FLOPPY_180K   17 // 180K  5.25"
#define BX_FLOPPY_320K   18 // 320K  5.25"
#define BX_FLOPPY_LAST   18 // last legal value of floppy type

#define BX_FLOPPY_AUTO     19 // autodetect image size
#define BX_FLOPPY_UNKNOWN  20 // image size doesn't match one of the types above

#define BX_ATA_DEVICE_DISK      0
#define BX_ATA_DEVICE_CDROM     1
#define BX_ATA_DEVICE_LAST      1

#define BX_ATA_BIOSDETECT_NONE      0
#define BX_ATA_BIOSDETECT_AUTO      1
#define BX_ATA_BIOSDETECT_CMOS      2

#define BX_ATA_TRANSLATION_NONE      0
#define BX_ATA_TRANSLATION_LBA       1
#define BX_ATA_TRANSLATION_LARGE     2
#define BX_ATA_TRANSLATION_RECHS     3
#define BX_ATA_TRANSLATION_AUTO      4
#define BX_ATA_TRANSLATION_LAST      4

#define BX_ATA_MODE_FLAT        0
#define BX_ATA_MODE_CONCAT      1
#define BX_ATA_MODE_EXTDISKSIM  2
#define BX_ATA_MODE_DLL_HD      3
#define BX_ATA_MODE_SPARSE      4
#define BX_ATA_MODE_VMWARE3     5
#define BX_ATA_MODE_VMWARE4     6
#define BX_ATA_MODE_UNDOABLE    7
#define BX_ATA_MODE_GROWING     8
#define BX_ATA_MODE_VOLATILE    9
#define BX_ATA_MODE_Z_UNDOABLE  10
#define BX_ATA_MODE_Z_VOLATILE  11
#define BX_ATA_MODE_LAST        11

#define BX_CLOCK_SYNC_NONE     0
#define BX_CLOCK_SYNC_REALTIME 1
#define BX_CLOCK_SYNC_SLOWDOWN 2
#define BX_CLOCK_SYNC_BOTH     3
#define BX_CLOCK_SYNC_LAST     3

#define BX_CLOCK_TIME0_LOCAL     1
#define BX_CLOCK_TIME0_UTC       2

BOCHSAPI extern char *bochs_start_names[];
BOCHSAPI extern char *floppy_type_names[];
BOCHSAPI extern int floppy_type_n_sectors[];
BOCHSAPI extern char *floppy_status_names[];
BOCHSAPI extern char *bochs_bootdisk_names[];
BOCHSAPI extern char *loader_os_names[];
BOCHSAPI extern char *keyboard_type_names[];
BOCHSAPI extern char *atadevice_type_names[];
BOCHSAPI extern char *atadevice_mode_names[];
BOCHSAPI extern char *atadevice_status_names[];
BOCHSAPI extern char *atadevice_biosdetect_names[];
BOCHSAPI extern char *atadevice_translation_names[];
BOCHSAPI extern char *clock_sync_names[];

////////////////////////////////////////////////////////////////////
// base class simulator interface, contains just virtual functions.
// I'm not longer sure that having a base class is going to be of any
// use... -Bryce

#include <setjmp.h>

enum ci_command_t { CI_START, CI_RUNTIME_CONFIG, CI_SHUTDOWN };
enum ci_return_t { 
  CI_OK,                  // normal return value 
  CI_ERR_NO_TEXT_CONSOLE  // err: can't work because there's no text console
  };
typedef int (*config_interface_callback_t)(void *userdata, ci_command_t command);
typedef BxEvent* (*bxevent_handler)(void *theclass, BxEvent *event);
typedef Bit32s (*user_option_parser_t)(const char *context, int num_params, char *params[]);
typedef Bit32s (*user_option_save_t)(FILE *fp);

// bx_gui->set_display_mode() changes the mode between the configuration
// interface and the simulation.  This is primarily intended for display
// libraries which have a full-screen mode such as SDL, term, and svgalib.  The
// display mode is set to DISP_MODE_CONFIG before displaying any configuration
// menus, for panics that requires user input, when entering the debugger, etc.
// It is set to DISP_MODE_SIM when the Bochs simulation resumes.  The constants
// are defined here so that configuration interfaces can use them with the
// bx_simulator_interface_c::set_display_mode() method.
enum disp_mode_t { DISP_MODE_CONFIG=100, DISP_MODE_SIM };

class BOCHSAPI bx_simulator_interface_c {
public:
  bx_simulator_interface_c() {}
  virtual ~bx_simulator_interface_c() {}
  virtual void set_quit_context(jmp_buf *context) {}
  virtual int get_init_done() { return -1; }
  virtual int set_init_done(int n) {return -1;}
  virtual void reset_all_param() {}
  // new param methods
  virtual bx_param_c *get_param(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual bx_param_num_c *get_param_num(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual bx_param_string_c *get_param_string(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual bx_param_bool_c *get_param_bool(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual bx_param_enum_c *get_param_enum(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual unsigned gen_param_id() {return 0;}
  virtual int get_n_log_modules() {return -1;}
  virtual char *get_prefix(int mod) {return 0;}
  virtual int get_log_action(int mod, int level) {return -1;}
  virtual void set_log_action(int mod, int level, int action) {}
  virtual int get_default_log_action(int level) {return -1;}
  virtual void set_default_log_action(int level, int action) {}
  virtual char *get_action_name(int action) {return 0;}
  virtual const char *get_log_level_name(int level) {return 0;}
  virtual int get_max_log_level() {return -1;}

  // exiting is somewhat complicated!  The preferred way to exit bochs is
  // to call BX_EXIT(exitcode).  That is defined to call 
  // SIM->quit_sim(exitcode).  The quit_sim function first calls
  // the cleanup functions in bochs so that it can destroy windows
  // and free up memory, then sends a notify message to the CI 
  // telling it that bochs has stopped.
  virtual void quit_sim(int code) {}

  virtual int get_exit_code() { return 0; }

  virtual int get_default_rc(char *path, int len) {return -1;}
  virtual int read_rc(const char *path) {return -1;}
  virtual int write_rc(const char *rc, int overwrite) {return -1;}
  virtual int get_log_file(char *path, int len) {return -1;}
  virtual int set_log_file(char *path) {return -1;}
  virtual int get_log_prefix(char *prefix, int len) {return -1;}
  virtual int set_log_prefix(char *prefix) {return -1;}
  virtual int get_debugger_log_file(char *path, int len) {return -1;}
  virtual int set_debugger_log_file(char *path) {return -1;}
  virtual int get_cdrom_options(int drive, bx_list_c **out, int *where = NULL) {return -1;}

  // The CI calls set_notify_callback to register its event handler function.
  // This event handler function is called whenever the simulator needs to
  // send an event to the CI.  For example, if the simulator hits a panic and
  // wants to ask the user how to proceed, it would call the CI event handler
  // to ask the CI to display a dialog.
  //
  // NOTE: At present, the standard VGAW buttons (floppy, snapshot, power,
  // etc.) are displayed and handled by gui.cc, not by the CI or siminterface.
  // gui.cc uses its own callback functions to implement the behavior of
  // the buttons.  Some of these implementations call the siminterface.
  virtual void set_notify_callback(bxevent_handler func, void *arg) {}
  virtual void get_notify_callback(bxevent_handler *func, void **arg) {}

  // send an event from the simulator to the CI.
  virtual BxEvent* sim_to_ci_event(BxEvent *event) {return NULL;}

  // called from simulator when it hits serious errors, to ask if the user
  // wants to continue or not
  virtual int log_msg(const char *prefix, int level, const char *msg) {return -1;}

  // tell the CI to ask the user for the value of a parameter.
  virtual int ask_param(bx_param_c *param) {return -1;}
  virtual int ask_param(const char *pname) {return -1;}

  // ask the user for a pathname
  virtual int ask_filename(char *filename, int maxlen, char *prompt, char *the_default, int flags) {return -1;}
  // yes/no dialog
  virtual int ask_yes_no(char *title, char *prompt, bx_bool the_default) {return -1;}
  // called at a regular interval, currently by the keyboard handler.
  virtual void periodic() {}
  virtual int create_disk_image(const char *filename, int sectors, bx_bool overwrite) {return -3;}
  // Tell the configuration interface (CI) that some parameter values have
  // changed.  The CI will reread the parameters and change its display if it's
  // appropriate.  Maybe later: mention which params have changed to save time.
  virtual void refresh_ci() {}
  // forces a vga update.  This was added so that a debugger can force
  // a vga update when single stepping, without having to wait thousands
  // of cycles for the normal vga refresh triggered by iodev/keyboard.cc.
  virtual void refresh_vga() {}
  // forces a call to bx_gui.handle_events.  This was added so that a debugger
  // can force the gui events to be handled, so that interactive things such
  // as a toolbar click will be processed.
  virtual void handle_events() {}
  // return first hard disk in ATA interface
  virtual bx_param_c *get_first_cdrom() {return NULL;}
  // return first cdrom in ATA interface
  virtual bx_param_c *get_first_hd() {return NULL;}
#if BX_DEBUGGER
  // for debugger: same behavior as pressing control-C
  virtual void debug_break() {}
  virtual void debug_interpret_cmd(char *cmd) {}
  virtual char *debug_get_next_command() {return NULL;}
  virtual void debug_puts(const char *text) {}
#endif
  virtual void register_configuration_interface(
    const char* name, 
    config_interface_callback_t callback,
    void *userdata) {}
  virtual int configuration_interface(const char* name, ci_command_t command) {return -1; }
  virtual int begin_simulation(int argc, char *argv[]) {return -1;}
  typedef bx_bool (*is_sim_thread_func_t)();
  is_sim_thread_func_t is_sim_thread_func;
  virtual void set_sim_thread_func(is_sim_thread_func_t func) {
    is_sim_thread_func = func;
  }
  virtual bx_bool is_sim_thread() {return 1;}
  virtual void set_debug_gui(bx_bool val) {}
  virtual bx_bool has_debug_gui() {return 0;}
  // provide interface to bx_gui->set_display_mode() method for config
  // interfaces to use.
  virtual void set_display_mode(disp_mode_t newmode) {}
  virtual bx_bool test_for_text_console() {return 1;}
  // user-defined option support
  virtual int find_user_option(const char *keyword) {return -1;}
  virtual bx_bool register_user_option(const char *keyword, user_option_parser_t parser, user_option_save_t save_func) {return 0;}
  virtual Bit32s parse_user_option(int idx, const char *context, int num_params, char *params []) {return -1;}
  virtual Bit32s save_user_options(FILE *fp) {return -1;}
  // save/restore support
  virtual void init_save_restore() {}
  virtual bx_bool save_state(const char *checkpoint_path) {return 0;}
  virtual bx_bool restore_config() {return 0;}
  virtual bx_bool restore_logopts() {return 0;}
  virtual bx_bool restore_hardware() {return 0;}
  virtual bx_list_c *get_bochs_root() {return NULL;}
  virtual bx_bool restore_bochs_param(bx_list_c *root, const char *sr_path, const char *restore_name) { return 0; }
};

BOCHSAPI extern bx_simulator_interface_c *SIM;

BOCHSAPI extern void bx_init_siminterface();
BOCHSAPI extern int bx_init_main(int argc, char *argv[]);

#if defined(__WXMSW__) || defined(WIN32)
// Just to provide HINSTANCE, etc. in files that have not included bochs.h.
// I don't like this at all, but I don't see a way around it.
#include <windows.h>
#endif

// define structure to hold data that is passed into our main function.
typedef struct BOCHSAPI {
  // standard argc,argv
  int argc;
  char **argv;
#ifdef WIN32
  char initial_dir[MAX_PATH];
#endif
#ifdef __WXMSW__
  // these are only used when compiling with wxWidgets.  This gives us a
  // place to store the data that was passed to WinMain.
  HINSTANCE hInstance;
  HINSTANCE hPrevInstance;
  LPSTR m_lpCmdLine;
  int nCmdShow;
#endif
} bx_startup_flags_t;

BOCHSAPI extern bx_startup_flags_t bx_startup_flags;
BOCHSAPI extern bx_bool bx_user_quit;
BOCHSAPI extern Bit8u bx_cpu_count;
