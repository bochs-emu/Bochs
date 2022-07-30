/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2021  The Bochs Project
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

#include "bochs.h"
#include "bxversion.h"
#include "param_names.h"
#include "cpu/cpu.h"
#include "iodev/iodev.h"
#include "iodev/hdimage/hdimage.h"
#if BX_NETWORKING
#include "iodev/network/netmod.h"
#endif
#if BX_SUPPORT_SOUNDLOW
#include "iodev/sound/soundmod.h"
#endif
#if BX_SUPPORT_PCIUSB
#include "iodev/usb/usb_common.h"
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#if BX_WITH_SDL || BX_WITH_SDL2
// since SDL redefines main() to SDL_main(), we must include SDL.h so that the
// C language prototype is found.  Otherwise SDL_main() will get its name
// mangled and not match what the SDL library is expecting.
#include <SDL.h>

#if defined(macintosh)
// Work around a bug in SDL 1.2.4 on MacOS X, which redefines getenv to
// SDL_getenv, but then neglects to provide SDL_getenv.  It happens
// because we are defining -Dmacintosh.
#undef getenv
#endif
#endif

#if BX_WITH_CARBON
#include <Carbon/Carbon.h>
#endif

extern "C" {
#include <signal.h>
}

#if BX_GUI_SIGHANDLER
bool bx_gui_sighandler = 0;
#endif

int  bx_init_main(int argc, char *argv[]);
void bx_init_hardware(void);
void bx_plugin_ctrl_reset(bool init_done);
void bx_init_options(void);
void bx_init_bx_dbg(void);

static const char *divider = "========================================================================";

bx_startup_flags_t bx_startup_flags;
bool bx_user_quit;
Bit8u bx_cpu_count;
#if BX_SUPPORT_APIC
Bit32u apic_id_mask; // determinted by XAPIC option
bool simulate_xapic;
#endif

/* typedefs */

#define LOG_THIS genlog->

bx_pc_system_c bx_pc_system;

bx_debug_t bx_dbg;

typedef BX_CPU_C *BX_CPU_C_PTR;

#if BX_SUPPORT_SMP
// multiprocessor simulation, we need an array of cpus
BOCHSAPI BX_CPU_C_PTR *bx_cpu_array = NULL;
#else
// single processor simulation, so there's one of everything
BOCHSAPI BX_CPU_C bx_cpu;
#endif

BOCHSAPI BX_MEM_C bx_mem;

char *bochsrc_filename = NULL;

size_t bx_get_timestamp(char *buffer)
{
#if VER_SVNFLAG == 1
#ifdef __DATE__
#ifdef __TIME__
  sprintf(buffer, "Compiled on %s at %s", __DATE__, __TIME__);
#else
  sprintf(buffer, "Compiled on %s", __DATE__);
#endif
#else
  buffer[0] = 0;
#endif
#else
  // Releases use the timestamp from README file
  sprintf(buffer, "Timestamp: %s", REL_TIMESTAMP);
#endif
  return strlen(buffer);
}

void bx_print_header()
{
  char buffer[128];

  printf("%s\n", divider);
  sprintf (buffer, "Bochs x86 Emulator %s\n", VERSION);
  bx_center_print(stdout, buffer, 72);
  if (REL_STRING[0]) {
    sprintf(buffer, "%s\n", REL_STRING);
    bx_center_print(stdout, buffer, 72);
    if (bx_get_timestamp(buffer) > 0) {
      bx_center_print(stdout, buffer, 72);
      printf("\n");
    }
  }
  printf("%s\n", divider);
}

#if BX_WITH_CARBON
/* Original code by Darrell Walisser - dwaliss1@purdue.edu */

static void setupWorkingDirectory(char *path)
{
  char parentdir[MAXPATHLEN];
  char *c;

  strncpy (parentdir, path, MAXPATHLEN);
  c = (char*) parentdir;

  while (*c != '\0')     /* go to end */
      c++;

  while (*c != '/')      /* back up to parent */
      c--;

  *c = '\0';             /* cut off last part (binary name) */

  /* chdir to the binary app's parent */
  int n;
  n = chdir (parentdir);
  if (n) BX_PANIC(("failed to change dir to parent"));
  /* chdir to the .app's parent */
  n = chdir ("../../../");
  if (n) BX_PANIC(("failed to change to ../../.."));
}

/* Panic button to display fatal errors.
  Completely self contained, can't rely on carbon.cc being available */
static void carbonFatalDialog(const char *error, const char *exposition)
{
  DialogRef                     alertDialog;
  CFStringRef                   cfError;
  CFStringRef                   cfExposition;
  DialogItemIndex               index;
  AlertStdCFStringAlertParamRec alertParam = {0};
  fprintf(stderr, "Entering carbonFatalDialog: %s\n", error);

  // Init libraries
  InitCursor();
  // Assemble dialog
  cfError = CFStringCreateWithCString(NULL, error, kCFStringEncodingASCII);
  if(exposition != NULL)
  {
    cfExposition = CFStringCreateWithCString(NULL, exposition, kCFStringEncodingASCII);
  }
  else { cfExposition = NULL; }
  alertParam.version       = kStdCFStringAlertVersionOne;
  alertParam.defaultText   = CFSTR("Quit");
  alertParam.position      = kWindowDefaultPosition;
  alertParam.defaultButton = kAlertStdAlertOKButton;
  // Display Dialog
  CreateStandardAlert(
    kAlertStopAlert,
    cfError,
    cfExposition,       /* can be NULL */
    &alertParam,             /* can be NULL */
    &alertDialog);
  RunStandardAlert(alertDialog, NULL, &index);
  // Cleanup
  CFRelease(cfError);
  if(cfExposition != NULL) { CFRelease(cfExposition); }
}
#endif

#if BX_DEBUGGER
void print_tree(bx_param_c *node, int level, bool xml)
{
  int i;
  char tmpstr[BX_PATHNAME_LEN];

  for (i=0; i<level; i++)
    dbg_printf("  ");
  if (node == NULL) {
      dbg_printf("NULL pointer\n");
      return;
  }

  if (xml)
    dbg_printf("<%s>", node->get_name());
  else
    dbg_printf("%s = ", node->get_name());

  switch (node->get_type()) {
    case BXT_PARAM_NUM:
    case BXT_PARAM_BOOL:
    case BXT_PARAM_ENUM:
    case BXT_PARAM_STRING:
      node->dump_param(tmpstr, BX_PATHNAME_LEN, 1);
      dbg_printf("%s", tmpstr);
      break;
    case BXT_LIST:
      {
        if (!xml) dbg_printf("{");
        dbg_printf("\n");
        bx_list_c *list = (bx_list_c*)node;
        for (i=0; i < list->get_size(); i++) {
          print_tree(list->get(i), level+1, xml);
        }
        for (i=0; i<level; i++)
          dbg_printf("  ");
        if (!xml) dbg_printf("}");
        break;
      }
    case BXT_PARAM_DATA:
      dbg_printf("'binary data size=%d'", ((bx_shadow_data_c*)node)->get_size());
      break;
    default:
      dbg_printf("(unknown parameter type)");
  }

  if (xml) dbg_printf("</%s>", node->get_name());
  dbg_printf("\n");
}
#endif

#if BX_ENABLE_STATISTICS
void print_statistics_tree(bx_param_c *node, int level)
{
  for (int i=0; i<level; i++)
    printf("  ");
  if (node == NULL) {
      printf("NULL pointer\n");
      return;
  }
  switch (node->get_type()) {
    case BXT_PARAM_NUM:
      {
        bx_param_num_c* param = (bx_param_num_c*) node;
        printf("%s = " FMT_LL "d\n", node->get_name(), param->get64());
        param->set(0); // clear the statistic
      }
      break;
    case BXT_PARAM_BOOL:
      BX_PANIC(("boolean statistics are not supported !"));
      break;
    case BXT_PARAM_ENUM:
      BX_PANIC(("enum statistics are not supported !"));
      break;
    case BXT_PARAM_STRING:
      BX_PANIC(("string statistics are not supported !"));
      break;
    case BXT_LIST:
      {
        bx_list_c *list = (bx_list_c*)node;
        if (list->get_size() > 0) {
          printf("%s = \n", node->get_name());
          for (int i=0; i < list->get_size(); i++) {
            print_statistics_tree(list->get(i), level+1);
          }
        }
        break;
      }
    case BXT_PARAM_DATA:
      BX_PANIC(("binary data statistics are not supported !"));
      break;
    default:
      BX_PANIC(("%s (unknown parameter type)\n", node->get_name()));
      break;
  }
}
#endif

int bxmain(void)
{
#ifdef HAVE_LOCALE_H
  // Initialize locale (for isprint() and other functions)
  setlocale (LC_ALL, "");
#endif
  bx_init_siminterface();   // create the SIM object
  static jmp_buf context;
  if (setjmp (context) == 0) {
    SIM->set_quit_context (&context);
    BX_INSTR_INIT_ENV();
    if (bx_init_main(bx_startup_flags.argc, bx_startup_flags.argv) < 0) {
      BX_INSTR_EXIT_ENV();
      return 0;
    }
    // read a param to decide which config interface to start.
    // If one exists, start it.  If not, just begin.
    bx_param_enum_c *ci_param = SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE);
    const char *ci_name = ci_param->get_selected();
#if BX_WITH_WX
    if (!strcmp(ci_name, "wx")) {
      PLUG_load_plugin_var("wx", PLUGTYPE_GUI);
    }
    else
#endif
    {
      PLUG_load_plugin_var(ci_name, PLUGTYPE_CI);
    }
    ci_param->set_enabled(0);
    int status = SIM->configuration_interface(ci_name, CI_START);
    if (status == CI_ERR_NO_TEXT_CONSOLE)
      BX_PANIC(("Bochs needed the text console, but it was not usable"));
    // user quit the config interface, so just quit
  } else {
    // quit via longjmp
  }
  SIM->set_quit_context(NULL);
#if defined(WIN32)
  if (!bx_user_quit) {
    // ask user to press ENTER before exiting, so that they can read messages
    // before the console window is closed. This isn't necessary after pressing
    // the power button.
    fprintf(stderr, "\nBochs is exiting. Press ENTER when you're ready to close this window.\n");
    char buf[16];
    fgets(buf, sizeof(buf), stdin);
  }
#endif
  plugin_cleanup();
  BX_INSTR_EXIT_ENV();
  return SIM->get_exit_code();
}

#if defined(__WXMSW__)

// win32 applications get the whole command line in one long string.
// This function is used to split up the string into argc and argv,
// so that the command line can be used on win32 just like on every
// other platform.
//
// I'm sure other people have written this same function, and they may have
// done it better, but I don't know where to find it. -BBD
#ifndef MAX_ARGLEN
#define MAX_ARGLEN 80
#endif
int split_string_into_argv(char *string, int *argc_out, char **argv, int max_argv)
{
  char *buf0 = new char[strlen(string)+1];
  strcpy (buf0, string);
  char *buf = buf0;
  int in_double_quote = 0, in_single_quote = 0;
  for (int i=0; i<max_argv; i++)
    argv[i] = NULL;
  argv[0] = new char[6];
  strcpy (argv[0], "bochs");
  int argc = 1;
  argv[argc] = new char[MAX_ARGLEN];
  char *outp = &argv[argc][0];
  // trim leading and trailing spaces
  while (*buf==' ') buf++;
  char *p;
  char *last_nonspace = buf;
  for (p=buf; *p; p++) {
    if (*p!=' ') last_nonspace = p;
  }
  if (last_nonspace != buf) *(last_nonspace+1) = 0;
  p = buf;
  bool done = false;
  while (!done) {
    //fprintf (stderr, "parsing '%c' with singlequote=%d, dblquote=%d\n", *p, in_single_quote, in_double_quote);
    switch (*p) {
    case '\0':
      done = true;
      // fall through into behavior for space
    case ' ':
      if (in_double_quote || in_single_quote)
        goto do_default;
      *outp = 0;
      //fprintf (stderr, "completed arg %d = '%s'\n", argc, argv[argc]);
      argc++;
      if (argc >= max_argv) {
        fprintf (stderr, "too many arguments. Increase MAX_ARGUMENTS\n");
        return -1;
      }
      argv[argc] = new char[MAX_ARGLEN];
      outp = &argv[argc][0];
      while (*p==' ') p++;
      break;
    case '"':
      if (in_single_quote) goto do_default;
      in_double_quote = !in_double_quote;
      p++;
      break;
    case '\'':
      if (in_double_quote) goto do_default;
      in_single_quote = !in_single_quote;
      p++;
      break;
    do_default:
    default:
      if (outp-&argv[argc][0] >= MAX_ARGLEN) {
        //fprintf (stderr, "command line arg %d exceeded max size %d\n", argc, MAX_ARGLEN);
        return -1;
      }
      *(outp++) = *(p++);
    }
  }
  if (in_single_quote) {
    fprintf (stderr, "end of string with mismatched single quote (')\n");
    return -1;
  }
  if (in_double_quote) {
    fprintf (stderr, "end of string with mismatched double quote (\")\n");
    return -1;
  }
  *argc_out = argc;
  return 0;
}
#endif /* if defined(__WXMSW__) */

#if defined(__WXMSW__) || ((BX_WITH_SDL || BX_WITH_SDL2) && defined(WIN32))
// The RedirectIOToConsole() function is copied from an article called "Adding
// Console I/O to a Win32 GUI App" in Windows Developer Journal, December 1997.
// It creates a console window.
//
// NOTE: It could probably be written so that it can safely be called for all
// win32 builds.
int RedirectIOToConsole()
{
  int hConHandle;
  Bit64s lStdHandle;
  FILE *fp;
  // allocate a console for this app
  FreeConsole();
  if (!AllocConsole()) {
    MessageBox(NULL, "Failed to create text console", "Error", MB_ICONERROR);
    return 0;
  }
  // redirect unbuffered STDOUT to the console
  lStdHandle = (Bit64s)GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle((long)lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stdout = *fp;
  setvbuf(stdout, NULL, _IONBF, 0);
  // redirect unbuffered STDIN to the console
  lStdHandle = (Bit64s)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle((long)lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "r");
  *stdin = *fp;
  setvbuf(stdin, NULL, _IONBF, 0);
  // redirect unbuffered STDERR to the console
  lStdHandle = (Bit64s)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle((long)lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stderr = *fp;
  setvbuf(stderr, NULL, _IONBF, 0);
  return 1;
}
#endif  /* if defined(__WXMSW__) || ((BX_WITH_SDL || BX_WITH_SDL2) && defined(WIN32)) */

#if defined(__WXMSW__)
// only used for wxWidgets/win32.
// This works ok in Cygwin with a standard wxWidgets compile.  In
// VC++ wxWidgets must be compiled with -DNOMAIN=1.
int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR m_lpCmdLine, int nCmdShow)
{
  bx_startup_flags.hInstance = hInstance;
  bx_startup_flags.hPrevInstance = hPrevInstance;
  bx_startup_flags.m_lpCmdLine = m_lpCmdLine;
  bx_startup_flags.nCmdShow = nCmdShow;
  int max_argv = 20;
  bx_startup_flags.argv = (char**) malloc (max_argv * sizeof (char*));
  split_string_into_argv(m_lpCmdLine, &bx_startup_flags.argc, bx_startup_flags.argv, max_argv);
  int arg = 1;
  bool bx_noconsole = 0;
  while (arg < bx_startup_flags.argc) {
    if (!strcmp("-noconsole", bx_startup_flags.argv[arg])) {
      bx_noconsole = 1;
      break;
    }
    arg++;
  }

  if (!bx_noconsole) {
    if (!RedirectIOToConsole()) {
      return 1;
    }
    SetConsoleTitle("Bochs for Windows (wxWidgets port) - Console");
  }
  return bxmain();
}
#endif

#if !defined(__WXMSW__)
// normal main function, presently in for all cases except for
// wxWidgets under win32.
int CDECL main(int argc, char *argv[])
{
  bx_startup_flags.argc = argc;
  bx_startup_flags.argv = argv;
#ifdef WIN32
  int arg = 1;
  bool bx_noconsole = 0;
  while (arg < argc) {
    if (!strcmp("-noconsole", argv[arg])) {
      bx_noconsole = 1;
      break;
    }
    arg++;
  }

  if (bx_noconsole) {
    FreeConsole();
  } else {
#if BX_WITH_SDL || BX_WITH_SDL2
    // if SDL/win32, try to create a console window.
    if (!RedirectIOToConsole()) {
      return 1;
    }
#endif
    SetConsoleTitle("Bochs for Windows - Console");
  }
#endif
  return bxmain();
}
#endif

void print_usage(void)
{
  fprintf(stderr,
    "Usage: bochs [flags] [bochsrc options]\n\n"
    "  -n               no configuration file\n"
    "  -f configfile    specify configuration file\n"
    "  -q               quick start (skip configuration interface)\n"
    "  -benchmark N     run Bochs in benchmark mode for N millions of emulated ticks\n"
#if BX_ENABLE_STATISTICS
    "  -dumpstats N     dump Bochs stats every N millions of emulated ticks\n"
#endif
    "  -r path          restore the Bochs state from path\n"
    "  -log filename    specify Bochs log file name\n"
    "  -unlock          unlock Bochs images leftover from previous session\n"
#if BX_DEBUGGER
    "  -rc filename     execute debugger commands stored in file\n"
    "  -dbglog filename specify Bochs internal debugger log file name\n"
#endif
#ifdef WIN32
    "  -noconsole       disable console window\n"
#endif
    "  --help           display this help and exit\n"
    "  --help features  display available features / devices and exit\n"
#if BX_CPU_LEVEL > 4
    "  --help cpu       display supported CPU models and exit\n"
#endif
    "\nFor information on Bochs configuration file arguments, see the\n"
#if (!defined(WIN32)) && !BX_WITH_MACOS
    "bochsrc section in the user documentation or the man page of bochsrc.\n");
#else
    "bochsrc section in the user documentation.\n");
#endif
}

int bx_init_main(int argc, char *argv[])
{
  // To deal with initialization order problems inherent in C++, use the macros
  // SAFE_GET_IOFUNC and SAFE_GET_GENLOG to retrieve "io" and "genlog" in all
  // constructors or functions called by constructors.  The macros test for
  // NULL and create the object if necessary, then return it.  Ensure that io
  // and genlog get created, by making one reference to each macro right here.
  // All other code can reference io and genlog directly.  Because these
  // objects are required for logging, and logging is so fundamental to
  // knowing what the program is doing, they are never free()d.
  SAFE_GET_IOFUNC();  // never freed
  SAFE_GET_GENLOG();  // never freed

  // initalization must be done early because some destructors expect
  // the bochs config options to exist by the time they are called.
  bx_init_bx_dbg();

#if BX_PLUGINS && BX_HAVE_GETENV && BX_HAVE_SETENV
  // set a default plugin path, in case the user did not specify one
  if (getenv("LTDL_LIBRARY_PATH") != NULL) {
    BX_INFO(("LTDL_LIBRARY_PATH is set to '%s'", getenv("LTDL_LIBRARY_PATH")));
  } else {
    BX_INFO(("LTDL_LIBRARY_PATH not set. using compile time default '%s'",
        BX_PLUGIN_PATH));
    setenv("LTDL_LIBRARY_PATH", BX_PLUGIN_PATH, 1);
  }
#endif
  // initialize plugin system. This must happen before we attempt to
  // load any modules.
  plugin_startup();

  bx_init_options();

  bx_print_header();

  SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_RUN_START);

  // interpret the args that start with -, like -q, -f, etc.
  int arg = 1, load_rcfile=1;
  while (arg < argc) {
    // parse next arg
    if (!strcmp("--help", argv[arg]) || !strncmp("-h", argv[arg], 2)
#if defined(WIN32)
        || !strncmp("/?", argv[arg], 2)
#endif
       ) {
      if ((arg+1) < argc) {
        if (!strcmp("features", argv[arg+1])) {
          fprintf(stderr, "Supported features:\n\n");
#if BX_SUPPORT_CLGD54XX
          fprintf(stderr, "cirrus\n");
#endif
#if BX_SUPPORT_VOODOO
          fprintf(stderr, "voodoo\n");
#endif
#if BX_SUPPORT_PCI
          fprintf(stderr, "pci\n");
#endif
#if BX_SUPPORT_PCIDEV
          fprintf(stderr, "pcidev\n");
#endif
#if BX_SUPPORT_NE2K
          fprintf(stderr, "ne2k\n");
#endif
#if BX_SUPPORT_PCIPNIC
          fprintf(stderr, "pcipnic\n");
#endif
#if BX_SUPPORT_E1000
          fprintf(stderr, "e1000\n");
#endif
#if BX_SUPPORT_SB16
          fprintf(stderr, "sb16\n");
#endif
#if BX_SUPPORT_ES1370
          fprintf(stderr, "es1370\n");
#endif
#if BX_SUPPORT_USB_OHCI
          fprintf(stderr, "usb_ohci\n");
#endif
#if BX_SUPPORT_USB_UHCI
          fprintf(stderr, "usb_uhci\n");
#endif
#if BX_SUPPORT_USB_EHCI
          fprintf(stderr, "usb_ehci\n");
#endif
#if BX_SUPPORT_USB_XHCI
          fprintf(stderr, "usb_xhci\n");
#endif
#if BX_GDBSTUB
          fprintf(stderr, "gdbstub\n");
#endif
          fprintf(stderr, "\n");
          arg++;
        }
#if BX_CPU_LEVEL > 4
        else if (!strcmp("cpu", argv[arg+1])) {
          int i = 0;
          fprintf(stderr, "Supported CPU models:\n\n");
          do {
            fprintf(stderr, "%s\n", SIM->get_param_enum(BXPN_CPU_MODEL)->get_choice(i));
          } while (i++ < SIM->get_param_enum(BXPN_CPU_MODEL)->get_max());
          fprintf(stderr, "\n");
          arg++;
        }
#endif
      } else {
        print_usage();
      }
      SIM->quit_sim(0);
    }
    else if (!strcmp("-n", argv[arg])) {
      load_rcfile = 0;
    }
    else if (!strcmp("-q", argv[arg])) {
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
    }
    else if (!strcmp("-log", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-log must be followed by a filename"));
      else SIM->get_param_string(BXPN_LOG_FILENAME)->set(argv[arg]);
    }
    else if (!strcmp("-unlock", argv[arg])) {
      SIM->get_param_bool(BXPN_UNLOCK_IMAGES)->set(1);
    }
#if BX_DEBUGGER
    else if (!strcmp("-dbglog", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-dbglog must be followed by a filename"));
      else SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->set(argv[arg]);
    }
#endif
    else if (!strcmp("-f", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-f must be followed by a filename"));
      else bochsrc_filename = argv[arg];
    }
    else if (!strcmp("-qf", argv[arg])) {
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
      if (++arg >= argc) BX_PANIC(("-qf must be followed by a filename"));
      else bochsrc_filename = argv[arg];
    }
    else if (!strcmp("-benchmark", argv[arg])) {
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
      if (++arg >= argc) BX_PANIC(("-benchmark must be followed by a number"));
      else SIM->get_param_num(BXPN_BOCHS_BENCHMARK)->set(atoi(argv[arg]));
    }
#if BX_ENABLE_STATISTICS
    else if (!strcmp("-dumpstats", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-dumpstats must be followed by a number"));
      else SIM->get_param_num(BXPN_DUMP_STATS)->set(atoi(argv[arg]));
    }
#endif
    else if (!strcmp("-r", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-r must be followed by a path"));
      else {
        SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
        SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(1);
        SIM->get_param_string(BXPN_RESTORE_PATH)->set(argv[arg]);
      }
    }
#ifdef WIN32
    else if (!strcmp("-noconsole", argv[arg])) {
      // already handled in main() / WinMain()
    }
#endif
#if BX_WITH_CARBON
    else if (!strncmp("-psn", argv[arg], 4)) {
      // "-psn" is passed if we are launched by double-clicking
      // ugly hack.  I don't know how to open a window to print messages in,
      // so put them in /tmp/early-bochs-out.txt.  Sorry. -bbd
      io->init_log("/tmp/early-bochs-out.txt");
      BX_INFO(("I was launched by double clicking.  Fixing home directory."));
      arg = argc; // ignore all other args.
      setupWorkingDirectory (argv[0]);
      // there is no stdin/stdout so disable the text-based config interface.
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
      char cwd[MAXPATHLEN];
      getwd (cwd);
      BX_INFO(("Now my working directory is %s", cwd));
      // if it was started from command line, there could be some args still.
      for (int a=0; a<argc; a++) {
        BX_INFO(("argument %d is %s", a, argv[a]));
      }
    }
#endif
#if BX_DEBUGGER
    else if (!strcmp("-rc", argv[arg])) {
      // process "-rc filename" option, if it exists
      if (++arg >= argc) BX_PANIC(("-rc must be followed by a filename"));
      else bx_dbg_set_rcfile(argv[arg]);
    }
#endif
    else if (argv[arg][0] == '-') {
      print_usage();
      BX_PANIC(("command line arg '%s' was not understood", argv[arg]));
    }
    else {
      // the arg did not start with -, so stop interpreting flags
      break;
    }
    arg++;
  }
#if BX_WITH_CARBON
  if(!getenv("BXSHARE"))
  {
    CFBundleRef mainBundle;
    CFURLRef bxshareDir;
    char bxshareDirPath[MAXPATHLEN];
    BX_INFO(("fixing default bxshare location ..."));
    // set bxshare to the directory that contains our application
    mainBundle = CFBundleGetMainBundle();
    BX_ASSERT(mainBundle != NULL);
    bxshareDir = CFBundleCopyBundleURL(mainBundle);
    BX_ASSERT(bxshareDir != NULL);
    // translate this to a unix style full path
    if(!CFURLGetFileSystemRepresentation(bxshareDir, true, (UInt8 *)bxshareDirPath, MAXPATHLEN))
    {
      BX_PANIC(("Unable to work out bxshare path! (Most likely path too long!)"));
      return -1;
    }
    char *c;
    c = (char*) bxshareDirPath;
    while (*c != '\0')  /* go to end */
      c++;
    while (*c != '/')   /* back up to parent */
      c--;
    *c = '\0';          /* cut off last part (binary name) */
    setenv("BXSHARE", bxshareDirPath, 1);
    BX_INFO(("now my BXSHARE is %s", getenv("BXSHARE")));
    CFRelease(bxshareDir);
  }
#endif
#if BX_PLUGINS && BX_WITH_CARBON
  // if there is no stdin, then we must create our own LTDL_LIBRARY_PATH.
  // also if there is no LTDL_LIBRARY_PATH, but we have a bundle since we're here
  // This is here so that it is available whenever --with-carbon is defined but
  // the above code might be skipped, as in --with-sdl --with-carbon
  if(!isatty(STDIN_FILENO) || !getenv("LTDL_LIBRARY_PATH"))
  {
    CFBundleRef mainBundle;
    CFURLRef libDir;
    char libDirPath[MAXPATHLEN];
    if(!isatty(STDIN_FILENO))
    {
      // there is no stdin/stdout so disable the text-based config interface.
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
    }
    BX_INFO(("fixing default lib location ..."));
    // locate the lib directory within the application bundle.
    // our libs have been placed in bochs.app/Contents/(current platform aka MacOS)/lib
    // This isn't quite right, but they are platform specific and we haven't put
    // our plugins into true frameworks and bundles either
    mainBundle = CFBundleGetMainBundle();
    BX_ASSERT(mainBundle != NULL);
    libDir = CFBundleCopyAuxiliaryExecutableURL(mainBundle, CFSTR("lib"));
    BX_ASSERT(libDir != NULL);
    // translate this to a unix style full path
    if(!CFURLGetFileSystemRepresentation(libDir, true, (UInt8 *)libDirPath, MAXPATHLEN))
    {
      BX_PANIC(("Unable to work out ltdl library path within bochs bundle! (Most likely path too long!)"));
      return -1;
    }
    setenv("LTDL_LIBRARY_PATH", libDirPath, 1);
    BX_INFO(("now my LTDL_LIBRARY_PATH is %s", getenv("LTDL_LIBRARY_PATH")));
    CFRelease(libDir);
  }
#endif  /* if BX_PLUGINS && BX_WITH_CARBON */
#if BX_HAVE_GETENV && BX_HAVE_SETENV
  if (getenv("BXSHARE") != NULL) {
    BX_INFO(("BXSHARE is set to '%s'", getenv("BXSHARE")));
  } else {
#ifdef WIN32
    BX_INFO(("BXSHARE not set. using system default '%s'",
        get_builtin_variable("BXSHARE")));
    setenv("BXSHARE", get_builtin_variable("BXSHARE"), 1);
#else
    BX_INFO(("BXSHARE not set. using compile time default '%s'",
        BX_SHARE_PATH));
    setenv("BXSHARE", BX_SHARE_PATH, 1);
#endif
  }
#else
  // we don't have getenv or setenv.  Do nothing.
#endif

  int norcfile = 1;

  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    load_rcfile = 0;
    norcfile = 0;
  } else {
    // set up and load pre-defined optional plugins before parsing configuration
    bx_plugin_ctrl_reset(0);
  }
  SIM->init_save_restore();
  SIM->init_statistics();
  if (load_rcfile) {
    // parse configuration file and command line arguments
#ifdef WIN32
    int length;
    if (bochsrc_filename != NULL) {
      lstrcpy(bx_startup_flags.initial_dir, bochsrc_filename);
      length = lstrlen(bx_startup_flags.initial_dir);
      while ((length > 1) && (bx_startup_flags.initial_dir[length-1] != 92)) length--;
      bx_startup_flags.initial_dir[length] = 0;
    } else {
      bx_startup_flags.initial_dir[0] = 0;
    }
#endif
    if (bochsrc_filename == NULL) bochsrc_filename = bx_find_bochsrc ();
    if (bochsrc_filename)
      norcfile = bx_read_configuration(bochsrc_filename);
  }

  if (norcfile) {
    // No configuration was loaded, so the current settings are unusable.
    // Switch off quick start so that we will drop into the configuration
    // interface.
    if (SIM->get_param_enum(BXPN_BOCHS_START)->get() == BX_QUICK_START) {
      if (!SIM->test_for_text_console())
        BX_PANIC(("Unable to start Bochs without a bochsrc.txt and without a text console"));
      else
        BX_ERROR(("Switching off quick start, because no configuration file was found."));
    }
    SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_LOAD_START);
  }

  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (arg < argc) {
      BX_ERROR(("WARNING: bochsrc options are ignored in restore mode!"));
    }
  }
  else {
    // parse the rest of the command line.  This is done after reading the
    // configuration file so that the command line arguments can override
    // the settings from the file.
    if (bx_parse_cmdline(arg, argc, argv)) {
      BX_PANIC(("There were errors while parsing the command line"));
      return -1;
    }
  }
  return 0;
}

bool load_and_init_display_lib(void)
{
  if (bx_gui != NULL) {
    // bx_gui has already been filled in.  This happens when you start
    // the simulation for the second time.
    // Also, if you load wxWidgets as the configuration interface.  Its
    // plugin_entry() will install wxWidgets as the bx_gui.
    return 1;
  }
  BX_ASSERT(bx_gui == NULL);
  bx_param_enum_c *ci_param = SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE);
  const char *ci_name = ci_param->get_selected();
  bx_param_enum_c *gui_param = SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY);
  const char *gui_name = gui_param->get_selected();
  if (!strcmp(ci_name, "wx")) {
    BX_ERROR(("change of the config interface to wx not implemented yet"));
  }
  if (!strcmp(gui_name, "wx")) {
    // they must not have used wx as the configuration interface, or bx_gui
    // would already be initialized.  Sorry, it doesn't work that way.
    BX_ERROR(("wxWidgets was not used as the configuration interface, so it cannot be used as the display library"));
    // choose another, hopefully different!
    gui_param->set(0);
    gui_name = gui_param->get_selected();
    if (!strcmp (gui_name, "wx")) {
      BX_PANIC(("no alternative display libraries are available"));
      return 0;
    }
    BX_ERROR(("changing display library to '%s' instead", gui_name));
  }
  PLUG_load_plugin_var(gui_name, PLUGTYPE_GUI);

#if BX_GUI_SIGHANDLER
  // set the flag for guis requiring a GUI sighandler.
  // useful when guis are compiled as plugins
  // only term for now
  if (!strcmp(gui_name, "term")) {
    bx_gui_sighandler = 1;
  }
#endif

  return (bx_gui != NULL);
}

int bx_begin_simulation(int argc, char *argv[])
{
  bx_user_quit = 0;
  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (!SIM->restore_config()) {
      BX_PANIC(("cannot restore configuration"));
      SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(0);
    }
  } else {
    // make sure all optional plugins have been loaded
    SIM->opt_plugin_ctrl("*", 1);
  }

  // deal with gui selection
  if (!load_and_init_display_lib()) {
    BX_PANIC(("no gui module was loaded"));
    return 0;
  }

  bx_cpu_count = SIM->get_param_num(BXPN_CPU_NPROCESSORS)->get() *
                 SIM->get_param_num(BXPN_CPU_NCORES)->get() *
                 SIM->get_param_num(BXPN_CPU_NTHREADS)->get();

#if BX_SUPPORT_APIC
  simulate_xapic = (SIM->get_param_enum(BXPN_CPUID_APIC)->get() >= BX_CPUID_SUPPORT_XAPIC);

  // For P6 and Pentium family processors the local APIC ID feild is 4 bits
  // APIC_MAX_ID indicate broadcast so it can't be used as valid APIC ID
  apic_id_mask = simulate_xapic ? 0xFF : 0xF;

  // leave one APIC ID to I/O APIC
  unsigned max_smp_threads = apic_id_mask - 1;
  if (bx_cpu_count > max_smp_threads) {
    BX_PANIC(("cpu: too many SMP threads defined, only %u threads supported by %sAPIC",
      max_smp_threads, simulate_xapic ? "x" : "legacy "));
  }
#endif

  BX_ASSERT(bx_cpu_count > 0);

  bx_init_hardware();

  SIM->set_init_done(1);

  // update headerbar buttons since drive status can change during init
  bx_gui->update_drive_status_buttons();

  // initialize statusbar and set all items inactive
  if (!SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    bx_gui->statusbar_setitem(-1, 0);
  } else {
    SIM->get_param_string(BXPN_RESTORE_PATH)->set("none");
  }

  // The set handler for mouse_enabled does not actually update the gui
  // until init_done is set.  This forces the set handler to be called,
  // which sets up the mouse enabled GUI-specific stuff correctly.
  // Not a great solution but it works. BBD
  SIM->get_param_bool(BXPN_MOUSE_ENABLED)->set(SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get());

#if BX_DEBUGGER
  // If using the debugger, it will take control and call
  // bx_init_hardware() and cpu_loop()
  bx_dbg_main();
#else
#if BX_GDBSTUB
  // If using gdbstub, it will take control and call
  // bx_init_hardware() and cpu_loop()
  if (bx_dbg.gdbstub_enabled) bx_gdbstub_init();
  else
#endif
  {
    if (BX_SMP_PROCESSORS == 1) {
      // only one processor, run as fast as possible by not messing with
      // quantums and loops.
      while (1) {
        BX_CPU(0)->cpu_loop();
        if (bx_pc_system.kill_bochs_request)
          break;
      }
      // for one processor, the only reason for cpu_loop to return is
      // that kill_bochs_request was set by the GUI interface.
    }
#if BX_SUPPORT_SMP
    else {
      // SMP simulation: do a few instructions on each processor, then switch
      // to another.  Increasing quantum speeds up overall performance, but
      // reduces granularity of synchronization between processors.
      // Current implementation uses dynamic quantum, each processor will
      // execute exactly one trace then quit the cpu_loop and switch to
      // the next processor.

      static int quantum = SIM->get_param_num(BXPN_SMP_QUANTUM)->get();
      Bit32u executed = 0, processor = 0;
      bool run = true;

      if (setjmp(BX_CPU_C::jmp_buf_env)) {
        // can get here only from exception function or VMEXIT
        BX_CPU(processor)->icount++;
        run = false;
      }
      while (1) {
         // do some instructions in each processor
        if (run)
          BX_CPU(processor)->cpu_run_trace();
        else
          run = true;

         // see how many instruction it was able to run
         Bit32u n = (Bit32u)(BX_CPU(processor)->get_icount() - BX_CPU(processor)->icount_last_sync);
         if (n == 0) n = quantum; // the CPU was halted
         executed += n;

         if (++processor == BX_SMP_PROCESSORS) {
           processor = 0;
           BX_TICKN(executed / BX_SMP_PROCESSORS);
           executed %= BX_SMP_PROCESSORS;
         }

         BX_CPU(processor)->icount_last_sync = BX_CPU(processor)->get_icount();

         if (bx_pc_system.kill_bochs_request)
           break;
      }
    }
#endif /* BX_SUPPORT_SMP */
  }
#endif /* BX_DEBUGGER == 0 */
  BX_INFO(("cpu loop quit, shutting down simulator"));
  bx_atexit();
  return(0);
}

void bx_stop_simulation(void)
{
  // in wxWidgets, the whole simulator is running in a separate thread.
  // our only job is to end the thread as soon as possible, NOT to shut
  // down the whole application with an exit.
  BX_CPU(0)->async_event = 1;
  bx_pc_system.kill_bochs_request = 1;
  // the cpu loop will exit very soon after this condition is set.
}

void bx_sr_after_restore_state(void)
{
#if BX_SUPPORT_SMP == 0
  BX_CPU(0)->after_restore_state();
#else
  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i)->after_restore_state();
  }
#endif
  DEV_after_restore_state();
}

void bx_set_log_actions_by_device(bool panic_flag)
{
  int id, l, m, val;
  bx_list_c *loglev, *level;
  bx_param_num_c *action;

  loglev = (bx_list_c*) SIM->get_param("general.logfn");
  for (l = 0; l < loglev->get_size(); l++) {
    level = (bx_list_c*) loglev->get(l);
    for (m = 0; m < level->get_size(); m++) {
      action = (bx_param_num_c*) level->get(m);
      id = SIM->get_logfn_id(action->get_name());
      val = action->get();
      if (id < 0) {
        if (panic_flag) {
          BX_PANIC(("unknown log function module '%s'", action->get_name()));
        }
      } else if (val >= 0) {
        SIM->set_log_action(id, l, val);
        // mark as 'done'
        action->set(-1);
      }
    }
  }
}

void bx_init_hardware()
{
  int i;
  char pname[16];
  bx_list_c *base;
  char buffer[128];

  // all configuration has been read, now initialize everything.

  bx_pc_system.initialize(SIM->get_param_num(BXPN_IPS)->get());

  if (SIM->get_param_string(BXPN_LOG_FILENAME)->getptr()[0]!='-') {
    BX_INFO(("using log file %s", SIM->get_param_string(BXPN_LOG_FILENAME)->getptr()));
    io->init_log(SIM->get_param_string(BXPN_LOG_FILENAME)->getptr());
  }

  io->set_log_prefix(SIM->get_param_string(BXPN_LOG_PREFIX)->getptr());

  // Output to the log file the cpu and device settings
  // This will by handy for bug reports
  BX_INFO(("Bochs x86 Emulator %s", VERSION));
  BX_INFO(("  %s", REL_STRING));
  if (bx_get_timestamp(buffer) > 0) {
    BX_INFO(("  %s", buffer));
  }
  BX_INFO(("System configuration"));
  BX_INFO(("  processors: %d (cores=%u, HT threads=%u)", BX_SMP_PROCESSORS,
    SIM->get_param_num(BXPN_CPU_NCORES)->get(), SIM->get_param_num(BXPN_CPU_NTHREADS)->get()));
  BX_INFO(("  A20 line support: %s", BX_SUPPORT_A20?"yes":"no"));
#if BX_CONFIGURE_MSRS
  const char *msrs_file = SIM->get_param_string(BXPN_CONFIGURABLE_MSRS_PATH)->getptr();
  if ((strlen(msrs_file) > 0) && strcmp(msrs_file, "none"))
    BX_INFO(("  load configurable MSRs from file \"%s\"", msrs_file));
#endif
  BX_INFO(("IPS is set to %d", (Bit32u) SIM->get_param_num(BXPN_IPS)->get()));
  BX_INFO(("CPU configuration"));
#if BX_SUPPORT_SMP
  BX_INFO(("  SMP support: yes, quantum=%d", SIM->get_param_num(BXPN_SMP_QUANTUM)->get()));
#else
  BX_INFO(("  SMP support: no"));
#endif

  unsigned cpu_model = SIM->get_param_enum(BXPN_CPU_MODEL)->get();
  if (! cpu_model) {
#if BX_CPU_LEVEL >= 5
    unsigned cpu_level = SIM->get_param_num(BXPN_CPUID_LEVEL)->get();
    BX_INFO(("  level: %d", cpu_level));
    BX_INFO(("  APIC support: %s", SIM->get_param_enum(BXPN_CPUID_APIC)->get_selected()));
#else
    BX_INFO(("  level: %d", BX_CPU_LEVEL));
    BX_INFO(("  APIC support: no"));
#endif
    BX_INFO(("  FPU support: %s", BX_SUPPORT_FPU?"yes":"no"));
#if BX_CPU_LEVEL >= 5
    bool mmx_enabled = SIM->get_param_bool(BXPN_CPUID_MMX)->get();
    BX_INFO(("  MMX support: %s", mmx_enabled?"yes":"no"));
    BX_INFO(("  3dnow! support: %s", BX_SUPPORT_3DNOW?"yes":"no"));
#endif
#if BX_CPU_LEVEL >= 6
    bool sep_enabled = SIM->get_param_bool(BXPN_CPUID_SEP)->get();
    BX_INFO(("  SEP support: %s", sep_enabled?"yes":"no"));
    BX_INFO(("  SIMD support: %s", SIM->get_param_enum(BXPN_CPUID_SIMD)->get_selected()));
    bool xsave_enabled = SIM->get_param_bool(BXPN_CPUID_XSAVE)->get();
    bool xsaveopt_enabled = SIM->get_param_bool(BXPN_CPUID_XSAVEOPT)->get();
    BX_INFO(("  XSAVE support: %s %s",
      xsave_enabled?"xsave":"no", xsaveopt_enabled?"xsaveopt":""));
    bool aes_enabled = SIM->get_param_bool(BXPN_CPUID_AES)->get();
    BX_INFO(("  AES support: %s", aes_enabled?"yes":"no"));
    bool sha_enabled = SIM->get_param_bool(BXPN_CPUID_SHA)->get();
    BX_INFO(("  SHA support: %s", sha_enabled?"yes":"no"));
    bool movbe_enabled = SIM->get_param_bool(BXPN_CPUID_MOVBE)->get();
    BX_INFO(("  MOVBE support: %s", movbe_enabled?"yes":"no"));
    bool adx_enabled = SIM->get_param_bool(BXPN_CPUID_ADX)->get();
    BX_INFO(("  ADX support: %s", adx_enabled?"yes":"no"));
#if BX_SUPPORT_X86_64
    bool x86_64_enabled = SIM->get_param_bool(BXPN_CPUID_X86_64)->get();
    BX_INFO(("  x86-64 support: %s", x86_64_enabled?"yes":"no"));
    bool xlarge_enabled = SIM->get_param_bool(BXPN_CPUID_1G_PAGES)->get();
    BX_INFO(("  1G paging support: %s", xlarge_enabled?"yes":"no"));
#else
    BX_INFO(("  x86-64 support: no"));
#endif
#if BX_SUPPORT_MONITOR_MWAIT
    bool mwait_enabled = SIM->get_param_bool(BXPN_CPUID_MWAIT)->get();
    BX_INFO(("  MWAIT support: %s", mwait_enabled?"yes":"no"));
#endif
#if BX_SUPPORT_VMX
    unsigned vmx_enabled = SIM->get_param_num(BXPN_CPUID_VMX)->get();
    if (vmx_enabled) {
      BX_INFO(("  VMX support: %d", vmx_enabled));
    }
    else {
      BX_INFO(("  VMX support: no"));
    }
#endif
#if BX_SUPPORT_SVM
    bool svm_enabled = SIM->get_param_bool(BXPN_CPUID_SVM)->get();
    BX_INFO(("  SVM support: %s", svm_enabled?"yes":"no"));
#endif
#endif // BX_CPU_LEVEL >= 6
  }
  else {
    BX_INFO(("  Using pre-defined CPU configuration: %s",
      SIM->get_param_enum(BXPN_CPU_MODEL)->get_selected()));
  }

  BX_INFO(("Optimization configuration"));
  BX_INFO(("  RepeatSpeedups support: %s", BX_SUPPORT_REPEAT_SPEEDUPS?"yes":"no"));
  BX_INFO(("  Fast function calls: %s", BX_FAST_FUNC_CALL?"yes":"no"));
  BX_INFO(("  Handlers Chaining speedups: %s", BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS?"yes":"no"));
  BX_INFO(("Devices configuration"));
  BX_INFO(("  PCI support: %s", BX_SUPPORT_PCI?"i440FX i430FX i440BX":"no"));
#if BX_NETWORKING
  BX_INFO(("  Network devices support:%s%s",
           BX_SUPPORT_NE2K?" NE2000":"", BX_SUPPORT_E1000?" E1000":""));
#else
  BX_INFO(("  Networking: no"));
#endif
#if BX_SUPPORT_SB16 || BX_SUPPORT_ES1370
  BX_INFO(("  Sound support:%s%s",
           BX_SUPPORT_SB16?" SB16":"", BX_SUPPORT_ES1370?" ES1370":""));
#else
  BX_INFO(("  Sound support: no"));
#endif
#if BX_SUPPORT_PCIUSB
  BX_INFO(("  USB support:%s%s%s%s",
           BX_SUPPORT_USB_UHCI?" UHCI":"", BX_SUPPORT_USB_OHCI?" OHCI":"",
           BX_SUPPORT_USB_EHCI?" EHCI":"", BX_SUPPORT_USB_XHCI?" xHCI":""));
#else
  BX_INFO(("  USB support: no"));
#endif
  BX_INFO(("  VGA extension support: vbe%s%s",
           BX_SUPPORT_CLGD54XX?" cirrus":"", BX_SUPPORT_VOODOO?" voodoo":""));
  bx_hdimage_ctl.list_modules();
#if BX_NETWORKING
  bx_netmod_ctl.list_modules();
#endif
#if BX_SUPPORT_SOUNDLOW
  bx_soundmod_ctl.list_modules();
#endif
#if BX_SUPPORT_PCIUSB
  bx_usbdev_ctl.list_devices();
#endif
  // Check if there is a romimage
  if (SIM->get_param_string(BXPN_ROM_PATH)->isempty()) {
    BX_ERROR(("No romimage to load. Is your bochsrc file loaded/valid ?"));
  }

  // set one shot timer for benchmark mode if needed, the timer will fire
  // once and kill Bochs simulation after predefined amount of emulated
  // ticks
  int benchmark_mode = SIM->get_param_num(BXPN_BOCHS_BENCHMARK)->get();
  if (benchmark_mode) {
    BX_INFO(("Bochs benchmark mode is ON (~%d millions of ticks)", benchmark_mode));
    bx_pc_system.register_timer_ticks(&bx_pc_system, bx_pc_system_c::benchmarkTimer,
        (Bit64u) benchmark_mode * 1000000, 0 /* one shot */, 1, "benchmark.timer");
  }

#if BX_ENABLE_STATISTICS
  // set periodic timer for dumping statistics collected during Bochs run
  int dumpstats = SIM->get_param_num(BXPN_DUMP_STATS)->get();
  if (dumpstats) {
    BX_INFO(("Dump statistics every %d millions of ticks", dumpstats));
    bx_pc_system.register_timer_ticks(&bx_pc_system, bx_pc_system_c::dumpStatsTimer,
        (Bit64u) dumpstats * 1000000, 1 /* continuous */, 1, "dumpstats.timer");
  }
#endif

  // set up memory and CPU objects
  bx_param_num_c *bxp_memsize = SIM->get_param_num(BXPN_MEM_SIZE);
  Bit64u memSize = bxp_memsize->get64() * BX_CONST64(1024*1024);

  bx_param_num_c *bxp_host_memsize = SIM->get_param_num(BXPN_HOST_MEM_SIZE);
  Bit64u hostMemSize = bxp_host_memsize->get64() * BX_CONST64(1024*1024);

  // do not allocate more host memory than needed for emulation of guest RAM 
  if (memSize < hostMemSize) hostMemSize = memSize;

  bx_param_num_c *bxp_memblock_size = SIM->get_param_num(BXPN_MEM_BLOCK_SIZE);
  Bit32u memBlockSize = bxp_memblock_size->get64() * 1024;

  BX_MEM(0)->init_memory(memSize, hostMemSize, memBlockSize);

  // First load the system BIOS (VGABIOS loading moved to the vga code)
  BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_ROM_PATH)->getptr(),
                      SIM->get_param_num(BXPN_ROM_ADDRESS)->get(), 0);

  // Then load the optional ROM images
  for (i=0; i<BX_N_OPTROM_IMAGES; i++) {
    sprintf(pname, "%s.%d", BXPN_OPTROM_BASE, i+1);
    base = (bx_list_c*) SIM->get_param(pname);
    if (!SIM->get_param_string("file", base)->isempty())
      BX_MEM(0)->load_ROM(SIM->get_param_string("file", base)->getptr(),
                          SIM->get_param_num("address", base)->get(), 2);
  }

  // Then load the optional RAM images
  for (i=0; i<BX_N_OPTRAM_IMAGES; i++) {
    sprintf(pname, "%s.%d", BXPN_OPTRAM_BASE, i+1);
    base = (bx_list_c*) SIM->get_param(pname);
    if (!SIM->get_param_string("file", base)->isempty())
      BX_MEM(0)->load_RAM(SIM->get_param_string("file", base)->getptr(),
                          SIM->get_param_num("address", base)->get());
  }

#if BX_SUPPORT_SMP == 0
  BX_CPU(0)->initialize();
  BX_CPU(0)->sanity_checks();
  BX_CPU(0)->register_state();
  BX_INSTR_INITIALIZE(0);
#else
  bx_cpu_array = new BX_CPU_C_PTR[BX_SMP_PROCESSORS];

  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i) = new BX_CPU_C(i);
    BX_CPU(i)->initialize();  // assign local apic id in 'initialize' method
    BX_CPU(i)->sanity_checks();
    BX_CPU(i)->register_state();
    BX_INSTR_INITIALIZE(i);
  }
#endif

  DEV_init_devices();
  // unload optional plugins which are unused and marked for removal
  SIM->opt_plugin_ctrl("*", 0);
  bx_pc_system.register_state();
  DEV_register_state();
  if (!SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    bx_set_log_actions_by_device(1);
  }

  // will enable A20 line and reset CPU and devices
  bx_pc_system.Reset(BX_RESET_HARDWARE);

  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (SIM->restore_hardware()) {
      if (!SIM->restore_logopts()) {
        BX_PANIC(("cannot restore log options"));
        SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(0);
      }
      bx_sr_after_restore_state();
    } else {
      BX_PANIC(("cannot restore hardware state"));
      SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(0);
    }
  }

  bx_gui->init_signal_handlers();
  bx_pc_system.start_timers();

  BX_DEBUG(("bx_init_hardware is setting signal handlers"));
// if not using debugger, then we can take control of SIGINT.
#if !BX_DEBUGGER
  signal(SIGINT, bx_signal_handler);
#endif

#if BX_SHOW_IPS
#if !defined(WIN32)
  if (!SIM->is_wx_selected()) {
    signal(SIGALRM, bx_signal_handler);
    alarm(1);
  }
#endif
#endif
}

void bx_init_bx_dbg(void)
{
#if BX_DEBUGGER
  bx_dbg_init_infile();
#endif
  memset(&bx_dbg, 0, sizeof(bx_debug_t));
}

int bx_atexit(void)
{
  if (!SIM->get_init_done()) return 1; // protect from reentry

  // in case we ended up in simulation mode, change back to config mode
  // so that the user can see any messages left behind on the console.
  SIM->set_display_mode(DISP_MODE_CONFIG);

#if BX_DEBUGGER == 0
  if (SIM && SIM->get_init_done()) {
    for (int cpu=0; cpu<BX_SMP_PROCESSORS; cpu++)
#if BX_SUPPORT_SMP
      if (BX_CPU(cpu))
#endif
        BX_CPU(cpu)->atexit();
  }
#endif

  BX_MEM(0)->cleanup_memory();

  bx_pc_system.exit();

  // restore signal handling to defaults
#if BX_DEBUGGER == 0
  BX_INFO(("restoring default signal behavior"));
  signal(SIGINT, SIG_DFL);
#endif

#if BX_SHOW_IPS
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  if (!SIM->is_wx_selected()) {
    alarm(0);
    signal(SIGALRM, SIG_DFL);
  }
#endif
#endif

  SIM->cleanup_save_restore();
  SIM->cleanup_statistics();
  SIM->set_init_done(0);

  return 0;
}

#if BX_SHOW_IPS
void bx_show_ips_handler(void)
{
  static Bit64u ticks_count = 0;
  static Bit64u counts = 0;

  // amount of system ticks passed from last time the handler was called
  Bit64u ips_count = bx_pc_system.time_ticks() - ticks_count;
  if (ips_count) {
    bx_gui->show_ips((Bit32u) ips_count);
    ticks_count = bx_pc_system.time_ticks();
    counts++;
    if (bx_dbg.print_timestamps) {
      printf("IPS: %u\taverage = %u\t\t(%us)\n",
         (unsigned) ips_count, (unsigned) (ticks_count/counts), (unsigned) counts);
      fflush(stdout);
    }
  }
  return;
}
#endif

void CDECL bx_signal_handler(int signum)
{
  // in a multithreaded environment, a signal such as SIGINT can be sent to all
  // threads.  This function is only intended to handle signals in the
  // simulator thread.  It will simply return if called from any other thread.
  // Otherwise the BX_PANIC() below can be called in multiple threads at
  // once, leading to multiple threads trying to display a dialog box,
  // leading to GUI deadlock.
  if (!SIM->is_sim_thread()) {
    BX_INFO(("bx_signal_handler: ignored sig %d because it wasn't called from the simulator thread", signum));
    return;
  }
#if BX_GUI_SIGHANDLER
  if (bx_gui_sighandler) {
    // GUI signal handler gets first priority, if the mask says it's wanted
    if ((1<<signum) & bx_gui->get_sighandler_mask()) {
      bx_gui->sighandler(signum);
      return;
    }
  }
#endif

#if BX_SHOW_IPS
  if (signum == SIGALRM) {
    bx_show_ips_handler();
#if !defined(WIN32)
    if (!SIM->is_wx_selected()) {
      signal(SIGALRM, bx_signal_handler);
      alarm(1);
    }
#endif
    return;
  }
#endif

#if BX_GUI_SIGHANDLER
  if (bx_gui_sighandler) {
    if ((1<<signum) & bx_gui->get_sighandler_mask()) {
      bx_gui->sighandler(signum);
      return;
    }
  }
#endif

  BX_PANIC(("SIGNAL %u caught", signum));
}
