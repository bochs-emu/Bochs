/////////////////////////////////////////////////////////////////////////
// $Id: main.cc,v 1.357 2007-09-28 19:51:42 sshwarts Exp $
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

#include "bochs.h"
#include "cpu/cpu.h"
#include "iodev/iodev.h"

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#if BX_WITH_SDL
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
#define Float32 KLUDGE_Float32
#define Float64 KLUDGE_Float64
#include <Carbon/Carbon.h>
#undef Float32
#undef Float64
#endif


extern "C" {
#include <signal.h>
}

#if defined(__MINGW32__) || defined(_MSC_VER)
void alarm(int);
#endif

#if BX_GUI_SIGHANDLER
bx_bool bx_gui_sighandler = 0;
#endif

#if BX_PROVIDE_DEVICE_MODELS==1
// some prototypes from iodev/
// I want to stay away from including iodev/iodev.h here
Bit32u bx_unmapped_io_read_handler(Bit32u address, unsigned io_len);
void   bx_unmapped_io_write_handler(Bit32u address, Bit32u value,
                                    unsigned io_len);
#endif

void bx_init_bx_dbg(void);
static char *divider = "========================================================================";
static logfunctions thePluginLog;
logfunctions *pluginlog = &thePluginLog;

bx_startup_flags_t bx_startup_flags;
bx_bool bx_user_quit;
Bit8u bx_cpu_count;

/* typedefs */

#define LOG_THIS genlog->

#if ( BX_PROVIDE_DEVICE_MODELS==1 )
bx_pc_system_c bx_pc_system;
#endif

bx_debug_t bx_dbg;

typedef BX_CPU_C *BX_CPU_C_PTR;

#if BX_SUPPORT_SMP
// multiprocessor simulation, we need an array of cpus
BOCHSAPI BX_CPU_C_PTR *bx_cpu_array = NULL;
#else
// single processor simulation, so there's one of everything
BOCHSAPI BX_CPU_C bx_cpu;
#endif

char *bochsrc_filename = NULL;

void bx_print_header ()
{
  fprintf (stderr, "%s\n", divider);
  char buffer[128];
  sprintf (buffer, "Bochs x86 Emulator %s\n", VER_STRING);
  bx_center_print (stderr, buffer, 72);
  if (REL_STRING[0]) {
    sprintf (buffer, "%s\n", REL_STRING);
    bx_center_print (stderr, buffer, 72);
  }
  fprintf (stderr, "%s\n", divider);
}

#if BX_WITH_CARBON
/* Original code by Darrell Walisser - dwaliss1@purdue.edu */

static void setupWorkingDirectory (char *path)
{
    char parentdir[MAXPATHLEN];
    char *c;
    
    strncpy ( parentdir, path, MAXPATHLEN );
    c = (char*) parentdir;
    
    while (*c != '\0')     /* go to end */
        c++;
    
    while (*c != '/')      /* back up to parent */
        c--;
    
    *c = '\0';             /* cut off last part (binary name) */
    
        /* chdir to the binary app's parent */
        int n;
        n = chdir (parentdir);
        if (n) BX_PANIC (("failed to change dir to parent"));
        /* chdir to the .app's parent */
        n = chdir ("../../../");
    if (n) BX_PANIC (("failed to change to ../../.."));
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
  RunStandardAlert( alertDialog, NULL, &index);
  // Cleanup
  CFRelease( cfError );
  if( cfExposition != NULL ) { CFRelease( cfExposition ); }
}
#endif

#if BX_DEBUGGER
void print_tree(bx_param_c *node, int level)
{
  int i;
  char tmpstr[BX_PATHNAME_LEN], tmpbyte[4];

  for (i=0; i<level; i++)
    dbg_printf("  ");
  if (node == NULL) {
      dbg_printf("NULL pointer\n");
      return;
  }
  switch (node->get_type()) {
    case BXT_PARAM_NUM:
      if (((bx_param_num_c*)node)->get_base() == BASE_DEC) {
        dbg_printf("%s = " FMT_LL "d (number)\n", node->get_name(), ((bx_param_num_c*)node)->get64());
      } else {
        dbg_printf("%s = 0x" FMT_LL "x (hex number)\n", node->get_name(), ((bx_param_num_c*)node)->get64());
      }
      break;
    case BXT_PARAM_BOOL:
      dbg_printf("%s = %s (boolean)\n", node->get_name(), ((bx_param_bool_c*)node)->get()?"true":"false");
      break;
    case BXT_PARAM_ENUM:
      dbg_printf("%s = '%s' (enum)\n", node->get_name(), ((bx_param_enum_c*)node)->get_selected());
      break;
    case BXT_PARAM_STRING:
      if (((bx_param_string_c*)node)->get_options()->get() & bx_param_string_c::RAW_BYTES) {
        tmpstr[0] = 0;
        for (i = 0; i < ((bx_param_string_c*)node)->get_maxsize(); i++) {
          if (i > 0) {
            tmpbyte[0] = ((bx_param_string_c*)node)->get_separator();
            tmpbyte[1] = 0;
            strcat(tmpstr, tmpbyte);
          }
          sprintf(tmpbyte, "%02x", (Bit8u)((bx_param_string_c*)node)->getptr()[i]);
          strcat(tmpstr, tmpbyte);
        }
        dbg_printf("%s = '%s' (raw byte string)\n", node->get_name(), tmpstr);
      } else {
        dbg_printf("%s = '%s' (string)\n", node->get_name(), ((bx_param_string_c*)node)->getptr());
      }
      break;
    case BXT_LIST:
      {
	dbg_printf("%s = \n", node->get_name());
	bx_list_c *list = (bx_list_c*)node;
	for (i=0; i < list->get_size(); i++) {
	   print_tree(list->get(i), level+1);
	}
	break;
      }
    case BXT_PARAM_DATA:
      dbg_printf("%s = 'size=%d' (binary data)\n", node->get_name(), ((bx_shadow_data_c*)node)->get_size());
      break;
    default:
      dbg_printf("%s (unknown parameter type)\n", node->get_name());
  }
}
#endif

int bxmain () {
#ifdef HAVE_LOCALE_H
  // Initialize locale (for isprint() and other functions)
  setlocale (LC_ALL, "");
#endif
  bx_user_quit = 0;
  bx_init_siminterface ();   // create the SIM object
  static jmp_buf context;
  if (setjmp (context) == 0) {
    SIM->set_quit_context (&context);
    if (bx_init_main (bx_startup_flags.argc, bx_startup_flags.argv) < 0) 
      return 0;
    // read a param to decide which config interface to start.
    // If one exists, start it.  If not, just begin.
    bx_param_enum_c *ci_param = SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE);
    char *ci_name = ci_param->get_selected();
    if (!strcmp(ci_name, "textconfig")) {
#if BX_USE_TEXTCONFIG
      init_text_config_interface();   // in textconfig.h
#else
      BX_PANIC(("configuration interface 'textconfig' not present"));
#endif
    }
#if BX_WITH_WX
    else if (!strcmp(ci_name, "wx")) {
      PLUG_load_plugin(wx, PLUGTYPE_CORE);
    }
#endif
    else {
      BX_PANIC(("unsupported configuration interface '%s'", ci_name));
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
int split_string_into_argv (
  char *string,
  int *argc_out,
  char **argv,
  int max_argv) 
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
  bx_bool done = false;
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

#if defined(__WXMSW__) || (BX_WITH_SDL && defined(WIN32))
// The RedirectIOToConsole() function is copied from an article called "Adding
// Console I/O to a Win32 GUI App" in Windows Developer Journal, December 1997.
// It creates a console window.
//
// NOTE: It could probably be written so that it can safely be called for all
// win32 builds.
int RedirectIOToConsole()
{
  int hConHandle;
  long lStdHandle;
  FILE *fp;
  // allocate a console for this app
  if (!AllocConsole()) {
    MessageBox(NULL, "Failed to create text console", "Error", MB_ICONERROR);
    return 0;
  }
  // redirect unbuffered STDOUT to the console
  lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stdout = *fp;
  setvbuf( stdout, NULL, _IONBF, 0 );
  // redirect unbuffered STDIN to the console
  lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "r");
  *stdin = *fp;
  setvbuf( stdin, NULL, _IONBF, 0 );
  // redirect unbuffered STDERR to the console
  lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen( hConHandle, "w" );
  *stderr = *fp;
  setvbuf(stderr, NULL, _IONBF, 0);
  return 1;
}
#endif  /* if defined(__WXMSW__) || (BX_WITH_SDL && defined(WIN32)) */

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
  if (!RedirectIOToConsole()) {
    return 1;
  }
  SetConsoleTitle("Bochs for Windows (wxWidgets port) - Console");
  int max_argv = 20;
  bx_startup_flags.argv = (char**) malloc (max_argv * sizeof (char*));
  split_string_into_argv(m_lpCmdLine, &bx_startup_flags.argc, bx_startup_flags.argv, max_argv);
  return bxmain();
}
#endif

#if !defined(__WXMSW__)
// normal main function, presently in for all cases except for
// wxWidgets under win32.
int main (int argc, char *argv[])
{
  bx_startup_flags.argc = argc;
  bx_startup_flags.argv = argv;
#if BX_WITH_SDL && defined(WIN32)
  // if SDL/win32, try to create a console window.
  if (!RedirectIOToConsole()) {
    return 1;
  }
#endif
#if defined(WIN32)
  SetConsoleTitle("Bochs for Windows - Console");
#endif
  return bxmain();
}
#endif

void print_usage()
{
  fprintf(stderr, 
    "Usage: bochs [flags] [bochsrc options]\n\n"
    "  -n               no configuration file\n"
    "  -f configfile    specify configuration file\n"
    "  -q               quick start (skip configuration interface)\n"
    "  -benchmark n     run bochs in benchmark mode for millions of emulated ticks\n"
    "  -r path          restore the Bochs state from path\n"
#if BX_DEBUGGER
    "  -rc filename     execute debugger commands stored in file\n"
#endif
    "  --help           display this help and exit\n\n"
    "For information on Bochs configuration file arguments, see the\n"
#if (!defined(WIN32)) && !BX_WITH_MACOS
    "bochsrc section in the user documentation or the man page of bochsrc.\n");
#else
    "bochsrc section in the user documentation.\n");
#endif
}

int bx_init_main (int argc, char *argv[])
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
      print_usage();
      SIM->quit_sim(0);
    }
    else if (!strcmp("-n", argv[arg])) {
      load_rcfile = 0;
    }
    else if (!strcmp("-q", argv[arg])) {
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
    }
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
    else if (!strcmp("-r", argv[arg])) {
      if (++arg >= argc) BX_PANIC(("-r must be followed by a path"));
      else {
        SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
        SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(1);
        SIM->get_param_string(BXPN_RESTORE_PATH)->set(argv[arg]);
      }
    }
#if BX_WITH_CARBON
    else if (!strncmp("-psn", argv[arg], 4)) {
      // "-psn" is passed if we are launched by double-clicking
      // ugly hack.  I don't know how to open a window to print messages in,
      // so put them in /tmp/early-bochs-out.txt.  Sorry. -bbd
      io->init_log("/tmp/early-bochs-out.txt");
      BX_INFO (("I was launched by double clicking.  Fixing home directory."));
      arg = argc; // ignore all other args.
      setupWorkingDirectory (argv[0]);
      // there is no stdin/stdout so disable the text-based config interface.
      SIM->get_param_enum(BXPN_BOCHS_START)->set(BX_QUICK_START);
      char cwd[MAXPATHLEN];
      getwd (cwd);
      BX_INFO (("Now my working directory is %s", cwd));
      // if it was started from command line, there could be some args still.
      for (int a=0; a<argc; a++) {
        BX_INFO (("argument %d is %s", a, argv[a]));
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
      BX_PANIC (("command line arg '%s' was not understood", argv[arg]));
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
    BX_INFO (("fixing default bxshare location ..."));
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
    BX_INFO (("now my BXSHARE is %s", getenv("BXSHARE")));
    CFRelease(bxshareDir);
  }
#endif
#if BX_PLUGINS
  // set a default plugin path, in case the user did not specify one
#if BX_WITH_CARBON
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
    BX_INFO (("fixing default lib location ..."));
    // locate the lib directory within the application bundle.
    // our libs have been placed in bochs.app/Contents/(current platform aka MacOS)/lib
    // This isn't quite right, but they are platform specific and we haven't put
    // our plugins into true frameworks and bundles either
    mainBundle = CFBundleGetMainBundle();
    BX_ASSERT(mainBundle != NULL);
    libDir = CFBundleCopyAuxiliaryExecutableURL( mainBundle, CFSTR("lib"));
    BX_ASSERT(libDir != NULL);
    // translate this to a unix style full path
    if(!CFURLGetFileSystemRepresentation(libDir, true, (UInt8 *)libDirPath, MAXPATHLEN))
    {
      BX_PANIC(("Unable to work out ltdl library path within bochs bundle! (Most likely path too long!)"));
      return -1;
    }
    setenv("LTDL_LIBRARY_PATH", libDirPath, 1);
    BX_INFO (("now my LTDL_LIBRARY_PATH is %s", getenv("LTDL_LIBRARY_PATH")));
    CFRelease(libDir);
  }
#elif BX_HAVE_GETENV && BX_HAVE_SETENV
  if (getenv("LTDL_LIBRARY_PATH") != NULL) {
    BX_INFO (("LTDL_LIBRARY_PATH is set to '%s'", getenv("LTDL_LIBRARY_PATH")));
  } else {
    BX_INFO (("LTDL_LIBRARY_PATH not set. using compile time default '%s'", 
        BX_PLUGIN_PATH));
    setenv("LTDL_LIBRARY_PATH", BX_PLUGIN_PATH, 1);
  }
  if (getenv("BXSHARE") != NULL) {
    BX_INFO (("BXSHARE is set to '%s'", getenv("BXSHARE")));
  } else {
    BX_INFO (("BXSHARE not set. using compile time default '%s'", 
        BX_SHARE_PATH));
    setenv("BXSHARE", BX_SHARE_PATH, 1);
  }
#else
  // we don't have getenv or setenv.  Do nothing.
#endif
#endif  /* if BX_PLUGINS */

  int norcfile = 1;

  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    load_rcfile = 0;
    norcfile = 0;
  }
  if (load_rcfile) {
    /* parse configuration file and command line arguments */
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
  // initialize plugin system. This must happen before we attempt to
  // load any modules.
  plugin_startup();
  return 0;
}

bx_bool load_and_init_display_lib()
{
  if (bx_gui != NULL) {
    // bx_gui has already been filled in.  This happens when you start
    // the simulation for the second time.
    // Also, if you load wxWidgets as the configuration interface.  Its
    // plugin_init will install wxWidgets as the bx_gui.
    return true;
  }
  BX_ASSERT(bx_gui == NULL);
  bx_param_enum_c *ci_param = SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE);
  char *ci_name = ci_param->get_selected();
  bx_param_enum_c *gui_param = SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY);
  char *gui_name = gui_param->get_selected();
  if (!strcmp(ci_name, "wx")) {
    BX_ERROR(("change of the config interface to wx not implemented yet"));
  }
  if (!strcmp(gui_name, "wx")) {
    // they must not have used wx as the configuration interface, or bx_gui
    // would already be initialized.  Sorry, it doesn't work that way.
    BX_ERROR(("wxWidgets was not used as the configuration interface, so it cannot be used as the display library"));
    // choose another, hopefully different!
    gui_param->set (0);
    gui_name = gui_param->get_selected();
    if (!strcmp (gui_name, "wx")) {
      BX_PANIC(("no alternative display libraries are available"));
      return false;
    }
    BX_ERROR(("changing display library to '%s' instead", gui_name));
  }
#if BX_WITH_AMIGAOS
  if (!strcmp(gui_name, "amigaos")) 
    PLUG_load_plugin (amigaos, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_BEOS
  if (!strcmp(gui_name, "beos")) 
    PLUG_load_plugin (beos, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_CARBON
  if (!strcmp(gui_name, "carbon")) 
    PLUG_load_plugin (carbon, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_MACOS
  if (!strcmp(gui_name, "macos")) 
    PLUG_load_plugin (macintosh, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_NOGUI
  if (!strcmp(gui_name, "nogui")) 
    PLUG_load_plugin (nogui, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_RFB
  if (!strcmp(gui_name, "rfb")) 
    PLUG_load_plugin (rfb, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_SDL
  if (!strcmp(gui_name, "sdl")) 
    PLUG_load_plugin (sdl, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_SVGA
  if (!strcmp(gui_name, "svga")) 
    PLUG_load_plugin (svga, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_TERM
  if (!strcmp(gui_name, "term")) 
    PLUG_load_plugin (term, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_WIN32
  if (!strcmp(gui_name, "win32")) 
    PLUG_load_plugin (win32, PLUGTYPE_OPTIONAL);
#endif
#if BX_WITH_X11
  if (!strcmp(gui_name, "x")) 
    PLUG_load_plugin (x, PLUGTYPE_OPTIONAL);
#endif

#if BX_GUI_SIGHANDLER
  // set the flag for guis requiring a GUI sighandler.
  // useful when guis are compiled as plugins
  // only term for now
  if (!strcmp(gui_name, "term")) {
    bx_gui_sighandler = 1;
  }
#endif

  BX_ASSERT(bx_gui != NULL);
  return true;
}

int bx_begin_simulation (int argc, char *argv[])
{
  SIM->init_save_restore();
  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (!SIM->restore_config()) {
      BX_PANIC(("cannot restore configuration"));
      SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(0);
    }
  }

  // deal with gui selection
  if (!load_and_init_display_lib ()) {
    BX_PANIC (("no gui module was loaded"));
    return 0;
  }

  bx_cpu_count = SIM->get_param_num(BXPN_CPU_NPROCESSORS)->get() *
                 SIM->get_param_num(BXPN_CPU_NCORES)->get() *
                 SIM->get_param_num(BXPN_CPU_NTHREADS)->get();

  BX_ASSERT(bx_cpu_count <= BX_MAX_SMP_THREADS_SUPPORTED);
  BX_ASSERT(bx_cpu_count >  0);

  bx_init_hardware();

  if (SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get()) {
    void bx_load32bitOSimagehack(void);
    bx_load32bitOSimagehack();
  }

  SIM->set_init_done(1);

  // update headerbar buttons since drive status can change during init
  bx_gui->update_drive_status_buttons();

  // iniialize statusbar and set all items inactive
  if (!SIM->get_param_bool(BXPN_RESTORE_FLAG)->get())
  {
    bx_gui->statusbar_setitem(-1, 0);
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
        BX_CPU(0)->cpu_loop(0);
        if (bx_pc_system.kill_bochs_request) 
          break;
      }
      // for one processor, the only reason for cpu_loop to return is
      // that kill_bochs_request was set by the GUI interface.
    }
    else {
      // SMP simulation: do a few instructions on each processor, then switch
      // to another.  Increasing quantum speeds up overall performance, but
      // reduces granularity of synchronization between processors.
      int processor = 0;
      int quantum = SIM->get_param_num(BXPN_SMP_QUANTUM)->get();
      while (1) {
        // do some instructions in each processor
        BX_CPU(processor)->cpu_loop(quantum);
        processor = (processor+1) % BX_SMP_PROCESSORS;
        if (bx_pc_system.kill_bochs_request) 
          break;
        if (processor == 0) 
          BX_TICKN(quantum);
      }
    }
  }
#endif /* BX_DEBUGGER == 0 */
  BX_INFO(("cpu loop quit, shutting down simulator"));
  bx_atexit();
  return(0);
}

void bx_stop_simulation()
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

int bx_init_hardware()
{
  // all configuration has been read, now initialize everything.

  if (SIM->get_param_enum(BXPN_BOCHS_START)->get()==BX_QUICK_START) {
    for (int level=0; level<N_LOGLEV; level++) {
      int action = SIM->get_default_log_action(level);
      io->set_log_action(level, action);
    }
  }

  bx_pc_system.initialize(SIM->get_param_num(BXPN_IPS)->get());

  if (SIM->get_param_string(BXPN_LOG_FILENAME)->getptr()[0]!='-') {
    BX_INFO (("using log file %s", SIM->get_param_string(BXPN_LOG_FILENAME)->getptr()));
    io->init_log(SIM->get_param_string(BXPN_LOG_FILENAME)->getptr());
  }

  io->set_log_prefix(SIM->get_param_string(BXPN_LOG_PREFIX)->getptr());

  // Output to the log file the cpu and device settings
  // This will by handy for bug reports
  BX_INFO(("Bochs x86 Emulator %s", VER_STRING));
  BX_INFO(("  %s", REL_STRING));
  BX_INFO(("System configuration"));
  BX_INFO(("  processors: %d (cores=%u, HT threads=%u)", BX_SMP_PROCESSORS,
    SIM->get_param_num(BXPN_CPU_NCORES)->get(), SIM->get_param_num(BXPN_CPU_NTHREADS)->get()));
  BX_INFO(("  A20 line support: %s",BX_SUPPORT_A20?"yes":"no"));
  BX_INFO(("  APIC support: %s",BX_SUPPORT_APIC?"yes":"no"));
  BX_INFO(("CPU configuration"));
  BX_INFO(("  level: %d",BX_CPU_LEVEL));
  BX_INFO(("  TLB enabled: %s",BX_USE_TLB?"yes":"no"));
#if BX_SUPPORT_SMP
  BX_INFO(("  SMP support: yes, quantum=%d", SIM->get_param_num(BXPN_SMP_QUANTUM)->get()));
#else
  BX_INFO(("  SMP support: no"));
#endif
  BX_INFO(("  FPU support: %s",BX_SUPPORT_FPU?"yes":"no"));
  BX_INFO(("  MMX support: %s",BX_SUPPORT_MMX?"yes":"no"));
  if (BX_SUPPORT_SSE == 0)
    BX_INFO(("  SSE support: no"));
  else
    BX_INFO(("  SSE support: %d",BX_SUPPORT_SSE));
  BX_INFO(("  CLFLUSH support: %s",BX_SUPPORT_CLFLUSH?"yes":"no"));
  BX_INFO(("  v8086 mode support: %s",BX_SUPPORT_V8086_MODE?"yes":"no"));
  BX_INFO(("  VME support: %s",BX_SUPPORT_VME?"yes":"no"));
  BX_INFO(("  3dnow! support: %s",BX_SUPPORT_3DNOW?"yes":"no"));
  BX_INFO(("  PAE support: %s",BX_SUPPORT_PAE?"yes":"no"));
  BX_INFO(("  PGE support: %s",BX_SUPPORT_GLOBAL_PAGES?"yes":"no"));
  BX_INFO(("  PSE support: %s",BX_SUPPORT_LARGE_PAGES?"yes":"no"));
  BX_INFO(("  x86-64 support: %s",BX_SUPPORT_X86_64?"yes":"no"));
  BX_INFO(("  SEP support: %s",BX_SUPPORT_SEP?"yes":"no"));
  BX_INFO(("Optimization configuration"));
  BX_INFO(("  Guest2HostTLB support: %s",BX_SupportGuest2HostTLB?"yes":"no"));
  BX_INFO(("  RepeatSpeedups support: %s",BX_SupportRepeatSpeedups?"yes":"no"));
  BX_INFO(("  Icache support: %s",BX_SUPPORT_ICACHE?"yes":"no"));
  BX_INFO(("  Host Asm support: %s",BX_SupportHostAsms?"yes":"no"));
  BX_INFO(("  Fast function calls: %s",BX_FAST_FUNC_CALL?"yes":"no"));
  BX_INFO(("Devices configuration"));
  BX_INFO(("  ACPI support: %s",BX_SUPPORT_ACPI?"yes":"no"));
  BX_INFO(("  NE2000 support: %s",BX_SUPPORT_NE2K?"yes":"no"));
  BX_INFO(("  PCI support: %s",BX_SUPPORT_PCI?"yes":"no"));
  BX_INFO(("  SB16 support: %s",BX_SUPPORT_SB16?"yes":"no"));
  BX_INFO(("  USB support: %s",BX_SUPPORT_PCIUSB?"yes":"no"));
  BX_INFO(("  VGA extension support: %s %s",BX_SUPPORT_VBE?"vbe":"",
           BX_SUPPORT_CLGD54XX?"cirrus":""));

  // Check if there is a romimage
  if (strcmp(SIM->get_param_string(BXPN_ROM_PATH)->getptr(),"") == 0) {
    BX_ERROR(("No romimage to load. Is your bochsrc file loaded/valid ?"));
  }

  // set one shot timer for benchmark mode if needed, the timer will fire
  // once and kill Bochs simulation after predefined amount of emulated
  // ticks
  int benchmark_mode = SIM->get_param_num(BXPN_BOCHS_BENCHMARK)->get();
  if (benchmark_mode) {
    BX_INFO(("Bochs benchmark mode is ON (~%d millions of ticks)", benchmark_mode));
    bx_pc_system.register_timer_ticks(&bx_pc_system, bx_pc_system_c::benchmarkTimer,
        (Bit64u) benchmark_mode * 1000000, 0, 1, "benchmark.timer");
  }

  // set up memory and CPU objects
  bx_param_num_c *bxp_memsize = SIM->get_param_num(BXPN_MEM_SIZE);
  Bit32u memSize = bxp_memsize->get() * 1024*1024;

#if BX_SUPPORT_ICACHE
  pageWriteStampTable.alloc(memSize);
#endif

  BX_MEM(0)->init_memory(memSize);

  // First load the BIOS and VGABIOS
  BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_ROM_PATH)->getptr(),
                      SIM->get_param_num(BXPN_ROM_ADDRESS)->get(), 0);
  BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr(), 0xc0000, 1);

  // Then load the optional ROM images
  if (strcmp(SIM->get_param_string(BXPN_OPTROM1_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_OPTROM1_PATH)->getptr(), SIM->get_param_num(BXPN_OPTROM1_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTROM2_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_OPTROM2_PATH)->getptr(), SIM->get_param_num(BXPN_OPTROM2_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTROM3_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_OPTROM3_PATH)->getptr(), SIM->get_param_num(BXPN_OPTROM3_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTROM4_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_OPTROM4_PATH)->getptr(), SIM->get_param_num(BXPN_OPTROM4_ADDRESS)->get(), 2);

  // Then load the optional RAM images
  if (strcmp(SIM->get_param_string(BXPN_OPTRAM1_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_RAM(SIM->get_param_string(BXPN_OPTRAM1_PATH)->getptr(), SIM->get_param_num(BXPN_OPTRAM1_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTRAM2_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_RAM(SIM->get_param_string(BXPN_OPTRAM2_PATH)->getptr(), SIM->get_param_num(BXPN_OPTRAM2_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTRAM3_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_RAM(SIM->get_param_string(BXPN_OPTRAM3_PATH)->getptr(), SIM->get_param_num(BXPN_OPTRAM3_ADDRESS)->get(), 2);
  if (strcmp(SIM->get_param_string(BXPN_OPTRAM4_PATH)->getptr(), "") !=0)
    BX_MEM(0)->load_RAM(SIM->get_param_string(BXPN_OPTRAM4_PATH)->getptr(), SIM->get_param_num(BXPN_OPTRAM4_ADDRESS)->get(), 2);

#if BX_SUPPORT_SMP == 0
  BX_CPU(0)->initialize(BX_MEM(0));
  BX_CPU(0)->sanity_checks();
  BX_CPU(0)->register_state();
  BX_INSTR_INIT(0);
#else
  bx_cpu_array = new BX_CPU_C_PTR[BX_SMP_PROCESSORS];

  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i) = new BX_CPU_C(i);
    BX_CPU(i)->initialize(BX_MEM(0));  // assign local apic id in 'initialize' method
    BX_CPU(i)->sanity_checks();
    BX_CPU(i)->register_state();
    BX_INSTR_INIT(i);
  }
#endif

  DEV_init_devices();
  bx_pc_system.register_state();
  DEV_register_state();
  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (!SIM->restore_logopts()) {
      BX_PANIC(("cannot restore log options"));
      SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(0);
    }
  }

  // will enable A20 line and reset CPU and devices
  bx_pc_system.Reset(BX_RESET_HARDWARE);

  if (SIM->get_param_bool(BXPN_RESTORE_FLAG)->get()) {
    if (SIM->restore_hardware()) {
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
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  signal(SIGALRM, bx_signal_handler);
#endif
  alarm( 1 );
#endif

  return(0);
}

void bx_init_bx_dbg(void)
{
#if BX_DEBUGGER
  bx_dbg_init_infile();
#endif
  bx_dbg.floppy = 0;
  bx_dbg.keyboard = 0;
  bx_dbg.video = 0;
  bx_dbg.disk = 0;
  bx_dbg.pit = 0;
  bx_dbg.pic = 0;
  bx_dbg.bios = 0;
  bx_dbg.cmos = 0;
  bx_dbg.a20 = 0;
  bx_dbg.interrupts = 0;
  bx_dbg.exceptions = 0;
  bx_dbg.unsupported = 0;
  bx_dbg.temp = 0;
  bx_dbg.reset = 0;
  bx_dbg.mouse = 0;
  bx_dbg.io = 0;
  bx_dbg.debugger = 0;
  bx_dbg.xms = 0;
  bx_dbg.v8086 = 0;
  bx_dbg.paging = 0;
  bx_dbg.creg = 0;
  bx_dbg.dreg = 0;
  bx_dbg.dma = 0;
  bx_dbg.unsupported_io = 0;
  bx_dbg.record_io = 0;
  bx_dbg.serial = 0;
  bx_dbg.cdrom = 0;
#if BX_MAGIC_BREAKPOINT
  bx_dbg.magic_break_enabled = 0;
#endif
#if BX_GDBSTUB
  bx_dbg.gdbstub_enabled = 0;
#endif
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
      if (BX_CPU(cpu)) BX_CPU(cpu)->atexit();
  }
#endif

  BX_MEM(0)->cleanup_memory();

#if BX_PROVIDE_DEVICE_MODELS==1
  bx_pc_system.exit();
#endif

  // restore signal handling to defaults
#if !BX_DEBUGGER
  BX_INFO(("restoring default signal behavior"));
  signal(SIGINT, SIG_DFL);
#endif

#if BX_SHOW_IPS
#ifndef __MINGW32__
  signal(SIGALRM, SIG_DFL);
#endif
#endif

  SIM->set_init_done(0);

  return 0;
}

void bx_signal_handler(int signum)
{
  // in a multithreaded environment, a signal such as SIGINT can be sent to all
  // threads.  This function is only intended to handle signals in the
  // simulator thread.  It will simply return if called from any other thread.
  // Otherwise the BX_PANIC() below can be called in multiple threads at
  // once, leading to multiple threads trying to display a dialog box,
  // leading to GUI deadlock.
  if (!SIM->is_sim_thread()) {
    BX_INFO (("bx_signal_handler: ignored sig %d because it wasn't called from the simulator thread", signum));
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
  static Bit64u ticks_count = 0;

  if (signum == SIGALRM)
  {
    // amount of system ticks passed from last time the handler was called
    Bit64u ips_count = bx_pc_system.time_ticks() - ticks_count;
    if (ips_count) {
      bx_gui->show_ips((Bit32u) ips_count);
      ticks_count = bx_pc_system.time_ticks();
    }
#if !defined(__MINGW32__) && !defined(_MSC_VER)
    signal(SIGALRM, bx_signal_handler);
    alarm( 1 );
#endif
    return;
  }
#endif

#if BX_GUI_SIGHANDLER
  if (bx_gui_sighandler) {
    if ((1<<signum) & bx_gui->get_sighandler_mask ()) {
      bx_gui->sighandler(signum);
      return;
    }
  }
#endif

  BX_PANIC(("SIGNAL %u caught", signum));
}
