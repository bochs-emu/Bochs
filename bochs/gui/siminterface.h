/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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
//
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
// x.cc, win32.cc, macintosh.cc, etc.  The files gui.h and gui.cc
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

// base value for generated new parameter id
#define BXP_NEW_PARAM_ID 1001

enum {
#define bx_define_cpudb(model) bx_cpudb_##model,
#include "cpudb.h"
  bx_cpudb_model_last
};
#undef bx_define_cpudb

#if BX_SUPPORT_SMP
  #define BX_SMP_PROCESSORS (bx_cpu_count)
#else
  #define BX_SMP_PROCESSORS 1
#endif

typedef enum {
  BX_TOOLBAR_UNDEFINED,
  BX_TOOLBAR_FLOPPYA,
  BX_TOOLBAR_FLOPPYB,
  BX_TOOLBAR_CDROM1,
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
  N_LOGLEV
} bx_log_levels;

// Log action defines
typedef enum {
  ACT_IGNORE = 0,
  ACT_REPORT,
  ACT_ASK,
  ACT_FATAL,
  N_ACT
} bx_log_actions;

#define BX_NULL_PREFIX  "[      ]"

// normally all action choices are available for all event types. The exclude
// expression allows some choices to be eliminated if they don't make any
// sense.  For example, it would be stupid to ignore a panic.
#define BX_LOG_OPTS_EXCLUDE(type, choice)  (                           \
   /* can't die or ask, on debug or info events */                     \
   (type <= LOGLEV_INFO && (choice == ACT_ASK || choice == ACT_FATAL)) \
   /* can't ignore panics */                                           \
   || (type == LOGLEV_PANIC && choice == ACT_IGNORE)                   \
   )

// floppy / cdrom media status
#define BX_EJECTED  0
#define BX_INSERTED 1

// boot devices (using the same values as the rombios)
enum {
  BX_BOOT_NONE,
  BX_BOOT_FLOPPYA,
  BX_BOOT_DISKC,
  BX_BOOT_CDROM, 
  BX_BOOT_NETWORK
};

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
// bx_devices_c::timer() in iodev/devices.cc) and calls
// bx_gui->handle_events(). Then all the events in the queue are
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
  BX_ASYNC_EVT_REFRESH,           // simulator -> CI
  BX_ASYNC_EVT_QUIT_SIM           // simulator -> CI
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
// simulator.  It is asynchronous.
typedef struct {
  // type is BX_EVT_MOUSE
  Bit16s dx, dy, dz;       // mouse motion delta
  Bit8u buttons;           // which buttons are pressed.
                           // bit 0: 1=left button down, 0=up
                           // bit 1: 1=right button down, 0=up
                           // bit 2: 1=middle button down, 0=up
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

// Event type: BX_ASYNC_EVT_LOG_MSG
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

// Event type: BX_ASYNC_EVT_DBG_MSG
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
  bx_bool on; // for toggling buttons, on=true means the toolbar button is
              // pressed. on=false means it is not pressed.
} BxToolbarEvent;

// The BxEvent structure should be used for all events.  Every event has
// a type and a spot for a return code (only used for synchronous events).
typedef struct {
  BxEventType type; // what kind is this?
  Bit32s retcode;   // success or failure. only used for synchronous events.
  union {
    BxKeyEvent key;
    BxMouseEvent mouse;
    BxParamEvent param;
    BxLogMsgEvent logmsg;
    BxToolbarEvent toolbar;
    BxDebugCommand debugcmd;
  } u;
} BxEvent;

#include "paramtree.h"

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

enum {
  BX_MOUSE_TOGGLE_CTRL_MB,
  BX_MOUSE_TOGGLE_CTRL_F10,
  BX_MOUSE_TOGGLE_CTRL_ALT,
  BX_MOUSE_TOGGLE_F12
};

#define BX_FDD_NONE  0 // floppy not present
#define BX_FDD_525DD 1 // 360K  5.25"
#define BX_FDD_525HD 2 // 1.2M  5.25"
#define BX_FDD_350DD 3 // 720K  3.5"
#define BX_FDD_350HD 4 // 1.44M 3.5"
#define BX_FDD_350ED 5 // 2.88M 3.5"

#define BX_FLOPPY_NONE   10 // media not present
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

#define BX_ATA_DEVICE_NONE       0
#define BX_ATA_DEVICE_DISK       1
#define BX_ATA_DEVICE_CDROM      2

#define BX_ATA_BIOSDETECT_NONE   0
#define BX_ATA_BIOSDETECT_AUTO   1
#define BX_ATA_BIOSDETECT_CMOS   2

enum {
  BX_ATA_TRANSLATION_NONE,
  BX_ATA_TRANSLATION_LBA,
  BX_ATA_TRANSLATION_LARGE,
  BX_ATA_TRANSLATION_RECHS,
  BX_ATA_TRANSLATION_AUTO
};
#define BX_ATA_TRANSLATION_LAST  BX_ATA_TRANSLATION_AUTO

enum {
  BX_HDIMAGE_MODE_FLAT,
  BX_HDIMAGE_MODE_CONCAT,
  BX_HDIMAGE_MODE_EXTDISKSIM,
  BX_HDIMAGE_MODE_DLL_HD,
  BX_HDIMAGE_MODE_SPARSE,
  BX_HDIMAGE_MODE_VMWARE3,
  BX_HDIMAGE_MODE_VMWARE4,
  BX_HDIMAGE_MODE_UNDOABLE,
  BX_HDIMAGE_MODE_GROWING,
  BX_HDIMAGE_MODE_VOLATILE,
  BX_HDIMAGE_MODE_VVFAT,
  BX_HDIMAGE_MODE_VPC
};
#define BX_HDIMAGE_MODE_LAST     BX_HDIMAGE_MODE_VPC
#define BX_HDIMAGE_MODE_UNKNOWN  -1

enum {
  BX_CLOCK_SYNC_NONE,
  BX_CLOCK_SYNC_REALTIME,
  BX_CLOCK_SYNC_SLOWDOWN,
  BX_CLOCK_SYNC_BOTH
};
#define BX_CLOCK_SYNC_LAST       BX_CLOCK_SYNC_BOTH

enum {
  BX_PCI_CHIPSET_I430FX,
  BX_PCI_CHIPSET_I440FX
};

enum {
  BX_CPUID_SUPPORT_NOSSE,
  BX_CPUID_SUPPORT_SSE,
  BX_CPUID_SUPPORT_SSE2,
  BX_CPUID_SUPPORT_SSE3,
  BX_CPUID_SUPPORT_SSSE3,
  BX_CPUID_SUPPORT_SSE4_1,
  BX_CPUID_SUPPORT_SSE4_2,
#if BX_SUPPORT_AVX
  BX_CPUID_SUPPORT_AVX,
  BX_CPUID_SUPPORT_AVX2,
#if BX_SUPPORT_EVEX
  BX_CPUID_SUPPORT_AVX512
#endif
#endif
};

enum {
  BX_CPUID_SUPPORT_LEGACY_APIC,
  BX_CPUID_SUPPORT_XAPIC,
#if BX_CPU_LEVEL >= 6
  BX_CPUID_SUPPORT_XAPIC_EXT,
  BX_CPUID_SUPPORT_X2APIC
#endif
};

#define BX_CLOCK_TIME0_LOCAL     1
#define BX_CLOCK_TIME0_UTC       2

BOCHSAPI extern const char *floppy_devtype_names[];
BOCHSAPI extern const char *floppy_type_names[];
BOCHSAPI extern int floppy_type_n_sectors[];
BOCHSAPI extern const char *media_status_names[];
BOCHSAPI extern const char *bochs_bootdisk_names[];
BOCHSAPI extern const char *hdimage_mode_names[];

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
typedef void (*rt_conf_handler_t)(void *this_ptr);
typedef Bit32s (*addon_option_parser_t)(const char *context, int num_params, char *params[]);
typedef Bit32s (*addon_option_save_t)(FILE *fp);

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
  virtual int get_init_done() { return 0; }
  virtual int set_init_done(int n) {return 0;}
  virtual void reset_all_param() {}
  // new param methods
  virtual bx_param_c *get_param(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual bx_param_num_c *get_param_num(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual bx_param_string_c *get_param_string(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual bx_param_bool_c *get_param_bool(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual bx_param_enum_c *get_param_enum(const char *pname, bx_param_c *base=NULL) {return NULL;}
  virtual unsigned gen_param_id() {return 0;}
  virtual int get_n_log_modules() {return -1;}
  virtual const char *get_logfn_name(int mod) {return NULL;}
  virtual int get_logfn_id(const char *name) {return -1;}
  virtual const char *get_prefix(int mod) {return NULL;}
  virtual int get_log_action(int mod, int level) {return -1;}
  virtual void set_log_action(int mod, int level, int action) {}
  virtual int get_default_log_action(int level) {return -1;}
  virtual void set_default_log_action(int level, int action) {}
  virtual const char *get_action_name(int action) {return NULL;}
  virtual const char *get_log_level_name(int level) {return NULL;}
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
  virtual int hdimage_get_mode(const char *mode)  {return -1;}

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
  virtual int log_ask(const char *prefix, int level, const char *msg) {return -1;}
  // called from simulator when writing a message to log file
  virtual void log_msg(const char *prefix, int level, const char *msg) {}
  // set this to 1 if the gui has a log viewer
  virtual void set_log_viewer(bx_bool val) {}
  virtual bx_bool has_log_viewer() const {return 0;}

  // tell the CI to ask the user for the value of a parameter.
  virtual int ask_param(bx_param_c *param) {return -1;}
  virtual int ask_param(const char *pname) {return -1;}

  // ask the user for a pathname
  virtual int ask_filename(const char *filename, int maxlen, const char *prompt, const char *the_default, int flags) {return -1;}
  // yes/no dialog
  virtual int ask_yes_no(const char *title, const char *prompt, bx_bool the_default) {return -1;}
  // called at a regular interval, currently by the bx_devices_c::timer()
  virtual void periodic() {}
  virtual int create_disk_image(const char *filename, int sectors, bx_bool overwrite) {return -3;}
  // Tell the configuration interface (CI) that some parameter values have
  // changed.  The CI will reread the parameters and change its display if it's
  // appropriate.  Maybe later: mention which params have changed to save time.
  virtual void refresh_ci() {}
  // forces a vga update.  This was added so that a debugger can force
  // a vga update when single stepping, without having to wait thousands
  // of cycles for the normal vga refresh triggered by the vga timer handler..
  virtual void refresh_vga() {}
  // forces a call to bx_gui.handle_events.  This was added so that a debugger
  // can force the gui events to be handled, so that interactive things such
  // as a toolbar click will be processed.
  virtual void handle_events() {}
  // return first hard disk in ATA interface
  virtual bx_param_c *get_first_cdrom() {return NULL;}
  // return first cdrom in ATA interface
  virtual bx_param_c *get_first_hd() {return NULL;}
  // return 1 if device is connected to a PCI slot
  virtual bx_bool is_pci_device(const char *name) {return 0;}
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
  virtual bx_bool register_runtime_config_handler(void *dev, rt_conf_handler_t handler) {return 0;}
  virtual void update_runtime_options() {}
  typedef bx_bool (*is_sim_thread_func_t)();
  is_sim_thread_func_t is_sim_thread_func;
  virtual void set_sim_thread_func(is_sim_thread_func_t func) {
    is_sim_thread_func = func;
  }
  virtual bx_bool is_sim_thread() {return 1;}
  virtual bx_bool is_wx_selected() const {return 0;}
  virtual void set_debug_gui(bx_bool val) {}
  virtual bx_bool has_debug_gui() const {return 0;}
  // provide interface to bx_gui->set_display_mode() method for config
  // interfaces to use.
  virtual void set_display_mode(disp_mode_t newmode) {}
  virtual bx_bool test_for_text_console() {return 1;}

  // add-on config option support
  virtual bx_bool register_addon_option(const char *keyword, addon_option_parser_t parser, addon_option_save_t save_func) {return 0;}
  virtual bx_bool unregister_addon_option(const char *keyword) {return 0;}
  virtual bx_bool is_addon_option(const char *keyword) {return 0;}
  virtual Bit32s parse_addon_option(const char *context, int num_params, char *params []) {return -1;}
  virtual Bit32s save_addon_options(FILE *fp) {return -1;}

  // statistics
  virtual void init_statistics() {}
  virtual void cleanup_statistics() {}
  virtual bx_list_c *get_statistics_root() {return NULL;}
 
  // save/restore support
  virtual void init_save_restore() {}
  virtual void cleanup_save_restore() {}
  virtual bx_bool save_state(const char *checkpoint_path) {return 0;}
  virtual bx_bool restore_config() {return 0;}
  virtual bx_bool restore_logopts() {return 0;}
  virtual bx_bool restore_hardware() {return 0;}
  virtual bx_list_c *get_bochs_root() {return NULL;}
  virtual bx_bool restore_bochs_param(bx_list_c *root, const char *sr_path, const char *restore_name) { return 0; }

  // special config parameter and options functions for plugins
  virtual bx_bool opt_plugin_ctrl(const char *plugname, bx_bool load) {return 0;}
  virtual void init_std_nic_options(const char *name, bx_list_c *menu) {}
  virtual void init_usb_options(const char *usb_name, const char *pname, int maxports) {}
  virtual int  parse_param_from_list(const char *context, const char *param, bx_list_c *base) {return 0;}
  virtual int  parse_nic_params(const char *context, const char *param, bx_list_c *base) {return 0;}
  virtual int  parse_usb_port_params(const char *context, bx_bool devopt,
                                     const char *param, int maxports, bx_list_c *base) {return 0;}
  virtual int  write_param_list(FILE *fp, bx_list_c *base, const char *optname, bx_bool multiline) {return 0;}
  virtual int  write_usb_options(FILE *fp, int maxports, bx_list_c *base) {return 0;}
};

BOCHSAPI extern bx_simulator_interface_c *SIM;

extern void bx_init_siminterface();

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
