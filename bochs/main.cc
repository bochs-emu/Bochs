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



#include "bochs.h"
#include <assert.h>
#include "state_file.h"

extern "C" {
#include <signal.h>
}

#ifdef __MINGW32__
void alarm(int);
#endif


#if BX_PROVIDE_DEVICE_MODELS==1
// some prototypes from iodev/
// I want to stay away from including iodev/iodev.h here
Bit32u bx_unmapped_io_read_handler(Bit32u address, unsigned io_len);
void   bx_unmapped_io_write_handler(Bit32u address, Bit32u value,
                                    unsigned io_len);
void   bx_close_harddrive(void);
#endif



void bx_init_bx_dbg (void);
void bx_emulate_hga_dumps_timer(void);
static char *divider = "========================================================================";


/* typedefs */


#if ( BX_PROVIDE_DEVICE_MODELS==1 )
bx_pc_system_c bx_pc_system;
class state_file state_stuff("state_file.out", "options");
#endif

bx_debug_t bx_dbg;

bx_options_t bx_options = {
  { "", BX_FLOPPY_NONE, BX_EJECTED },   // floppya
  { "", BX_FLOPPY_NONE, BX_EJECTED },   // floppyb
  { 0, "", 0, 0, 0 },                   // diskc
  { 0, "", 0, 0, 0 },                   // diskd
  { 0, "", 0 },                         // cdromd
  { NULL, NULL },                          // rom
  { NULL },                             // vgarom
  { NULL },              // memory
  { 0, NULL, NULL, NULL, 0, 0, 0, 0 },  // SB16
  "a",                                  // boot drive
  NULL,                               // vga update interval
  20000,  // default keyboard serial path delay (usec)
  50000,  // default floppy command delay (usec)
  NULL,    // ips
  NULL,    // default mouse_enabled
  0,       // default private_colormap
  0,          // default i440FXSupport
  {NULL, 0},  // cmos path, cmos image boolean
  { 0, 0, 0, {0,0,0,0,0,0}, NULL, NULL }, // ne2k
  1,          // newHardDriveSupport
  { 0, NULL, NULL, NULL }, // load32bitOSImage hack stuff
  // log options: ignore debug, report info and error, crash on panic.
  { "-", { ACT_IGNORE, ACT_REPORT, ACT_REPORT, ACT_ASK } },
  };

static void parse_line_unformatted(char *context, char *line);
static void parse_line_formatted(char *context, int num_params, char *params[]);
static int parse_bochsrc(char *rcfile);


// Just for the iofunctions

#define LOG_THIS this->log->

int Allocio=0;

void
iofunctions::flush(void) {
	if(logfd && magic == MAGIC_LOGNUM) {
		fflush(logfd);
	}
}

void
iofunctions::init(void) {
	// iofunctions methods must not be called before this magic
	// number is set.
	magic=MAGIC_LOGNUM;
	showtick = 1;
	n_logfn = 0;
	init_log(stderr);
	log = new logfunc_t(this);
	LOG_THIS setprefix("[IO  ]");
	LOG_THIS settype(IOLOG);
	BX_DEBUG(("Init(log file: '%s').",logfn));
}

void
iofunctions::add_logfn (logfunc_t *fn)
{
  assert (n_logfn < MAX_LOGFNS);
  logfn_list[n_logfn++] = fn;
}

void
iofunctions::set_log_action (int loglevel, int action)
{
  for (int i=0; i<n_logfn; i++)
    logfn_list[i]->setonoff(loglevel, action);
}

void
iofunctions::init_log(char *fn)
{
	assert (magic==MAGIC_LOGNUM);
	// use newfd/newfn so that we can log the message to the OLD log
	// file descriptor.
	FILE *newfd = stderr;
	char *newfn = "/dev/stderr";
	if( strcmp( fn, "-" ) != 0 ) {
		newfd = fopen(fn, "w");
		if(newfd != NULL) {
			newfn = strdup(fn);
			BX_DEBUG(("Opened log file '%s'.", fn ));
		} else {
			BX_DEBUG(("Log file '%s' not there?", fn));
			newfd = NULL;
			logfn = "(none)";
		}
	}
	logfd = newfd;
	logfn = newfn;
}

void
iofunctions::init_log(FILE *fs)
{
	assert (magic==MAGIC_LOGNUM);
	logfd = fs;

	if(fs == stderr) {
		logfn = "/dev/stderr";
	} else if(fs == stdout) { 
		logfn = "/dev/stdout";
	} else {
		logfn = "(unknown)";
	}
}

void
iofunctions::init_log(int fd)
{
	assert (magic==MAGIC_LOGNUM);
	FILE *tmpfd;
	if( (tmpfd = fdopen(fd,"w")) == NULL ) {
	  BX_PANIC(("Couldn't open fd %d as a stream for writing", fd));
	  return;
	}

	init_log(tmpfd);
	return;
};

//  iofunctions::out( class, level, prefix, fmt, ap)
//  DO NOT nest out() from ::info() and the like.
//    fmt and ap retained for direct printinf from iofunctions only!

void
iofunctions::out(int f, int l, char *prefix, char *fmt, va_list ap)
{
	assert (magic==MAGIC_LOGNUM);
	assert (this != NULL);
	assert (logfd != NULL);

	if( showtick )
		fprintf(logfd, "%011lld ", bx_pc_system.time_ticks());

	if(prefix != NULL)
		fprintf(logfd, "%s ", prefix);

	if(l==LOGLEV_PANIC)
		fprintf(logfd, ">>PANIC<< ");

	vfprintf(logfd, fmt, ap);
	fprintf(logfd, "\n");
	fflush(logfd);

	return;
}

iofunctions::iofunctions(FILE *fs)
{
	init();
	init_log(fs);
}

iofunctions::iofunctions(char *fn)
{
	init();
	init_log(fn);
}

iofunctions::iofunctions(int fd)
{
	init();
	init_log(fd);
}

iofunctions::iofunctions(void)
{
	this->init();
}

iofunctions::~iofunctions(void)
{
	// flush before erasing magic number, or flush does nothing.
	this->flush();
	this->magic=0;
}

#undef LOG_THIS
#define LOG_THIS genlog->

logfunctions::logfunctions(void)
{
	setprefix("[    ]");
	settype(GENLOG);
	if(io == NULL && Allocio == 0) {
		Allocio = 1;
		io = new iofunc_t(stderr);
	}
	setio(io);
	// BUG: unfortunately this can be called before the bochsrc is read,
	// which means that the bochsrc has no effect on the actions.
	for (int i=0; i<N_LOGLEV; i++)
	  onoff[i] = bx_options.log.actions[i];
}

logfunctions::logfunctions(iofunc_t *iofunc)
{
	setprefix("[    ]");
	settype(GENLOG);
	setio(iofunc);
	// BUG: unfortunately this can be called before the bochsrc is read,
	// which means that the bochsrc has no effect on the actions.
	for (int i=0; i<N_LOGLEV; i++)
	  onoff[i] = bx_options.log.actions[i];
}

logfunctions::~logfunctions(void)
{
}

void
logfunctions::setio(iofunc_t *i)
{
  	// add pointer to iofunction object to use
	this->logio = i;
	// give iofunction a pointer to me
	i->add_logfn (this);
}

void
logfunctions::setprefix(char *p)
{
	this->prefix=strdup(p);
}

void
logfunctions::settype(int t)
{
	type=t;
}

void
logfunctions::info(char *fmt, ...)
{
	va_list ap;

	assert (this != NULL);
	assert (this->logio != NULL);

	if(!onoff[LOGLEV_INFO]) return;

	va_start(ap, fmt);
	this->logio->out(this->type,LOGLEV_INFO,this->prefix, fmt, ap);
	if (onoff[LOGLEV_INFO] == ACT_ASK) 
	  ask (LOGLEV_INFO, this->prefix, fmt, ap);
	if (onoff[LOGLEV_INFO] == ACT_FATAL) 
	  fatal (this->prefix, fmt, ap);
	va_end(ap);

}

void
logfunctions::error(char *fmt, ...)
{
	va_list ap;

	assert (this != NULL);
	assert (this->logio != NULL);

	if(!onoff[LOGLEV_ERROR]) return;

	va_start(ap, fmt);
	this->logio->out(this->type,LOGLEV_ERROR,this->prefix, fmt, ap);
	if (onoff[LOGLEV_ERROR] == ACT_ASK) 
	  ask (LOGLEV_ERROR, this->prefix, fmt, ap);
	if (onoff[LOGLEV_ERROR] == ACT_FATAL) 
	  fatal (this->prefix, fmt, ap);
	va_end(ap);
}

void
logfunctions::panic(char *fmt, ...)
{
	va_list ap;

	assert (this != NULL);
	assert (this->logio != NULL);

	if(!onoff[LOGLEV_PANIC]) return;

	va_start(ap, fmt);
	this->logio->out(this->type,LOGLEV_PANIC,this->prefix, fmt, ap);
	if (onoff[LOGLEV_PANIC] == ACT_ASK) 
	  ask (LOGLEV_PANIC, this->prefix, fmt, ap);
	if (onoff[LOGLEV_PANIC] == ACT_FATAL) 
	  fatal (this->prefix, fmt, ap);
	va_end(ap);
}

void
logfunctions::ldebug(char *fmt, ...)
{
	va_list ap;

	assert (this != NULL);
	assert (this->logio != NULL);

	if(!onoff[LOGLEV_DEBUG]) return;

	va_start(ap, fmt);
	this->logio->out(this->type,LOGLEV_DEBUG,this->prefix, fmt, ap);
	if (onoff[LOGLEV_DEBUG] == ACT_ASK) 
	  ask (LOGLEV_DEBUG, this->prefix, fmt, ap);
	if (onoff[LOGLEV_DEBUG] == ACT_FATAL) 
	  fatal (this->prefix, fmt, ap);
	va_end(ap);
}

void
logfunctions::ask (int level, char *prefix, char *fmt, va_list ap)
{
#if BX_USE_CONTROL_PANEL
  char buf1[1024], buf2[1024];
  vsprintf (buf1, fmt, ap);
  sprintf (buf2, "%s %s", prefix, buf1);
  // FIXME: facility set to 0 because it's unknown.
  int val = SIM->LOCAL_log_msg (prefix, level, buf2);
  switch (val)
  {
    case 0:   // user chose continue
      break;
    case 1:   // user said continue, and don't ask for this facility again.
      setonoff (level, ACT_REPORT);
      break;
    case 2:   // user chose die
      fatal (prefix, fmt, ap);
  }
#endif
}

void
logfunctions::fatal (char *prefix, char *fmt, va_list ap)
{
  static int fatal_reentry = 0;
  if (fatal_reentry) return;
  fatal_reentry++;
  bx_atexit();
  fprintf (stderr, "%s\n", divider);
  fprintf (stderr, "Bochs is exiting with the following message:\n");
  fprintf (stderr, "%s ", prefix);
  vfprintf (stderr, fmt, ap);
  fprintf (stderr, "\n%s\n", divider);
#if 0 && defined(WIN32)
#error disabled because it  is not working yet!
  // wait for a keypress before quitting.  Depending on how bochs is
  // installed, the console window can disappear before the user has
  // a chance to read the final message.
  fprintf (stderr, "\n\nPress Enter to exit...\n");
  char buf[8];
  fgets (buf, 8, stdin);
#endif
#if !BX_DEBUGGER
  exit(1);
#else
  static Boolean dbg_exit_called = 0;
  if (dbg_exit_called == 0) {
    dbg_exit_called = 1;
    bx_dbg_exit(1);
    }
#endif
  fatal_reentry--;
}

iofunc_t *io = NULL;
logfunc_t *genlog = NULL;

void bx_center_print (FILE *file, char *line, int maxwidth)
{
  int imax;
  imax = (maxwidth - strlen(line)) >> 1;
  for (int i=0; i<imax; i++) fputc (' ', file);
  fputs (line, file);
}

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



static Bit32s
bx_param_handler (bx_param_c *param, int set, Bit32s val)
{
  switch (param->get_id ()) {
    case BXP_VGA_UPDATE_INTERVAL:
      // if after init, notify the vga device to change its timer.
      if (set && SIM->get_init_done ())
	bx_vga.set_update_interval (val);
      break;
    case BXP_MOUSE_ENABLED:
      // if after init, notify the GUI
      if (set && SIM->get_init_done ())
	bx_gui.mouse_enabled_changed (val!=0);
      break;
    default:
      BX_PANIC (("bx_param_handler called with unknown parameter"));
      return -1;
  }
  return val;
}

void bx_init_options ()
{
  bx_options.memory.size = new bx_param_num_c (BXP_MEM_SIZE,
      "megs",
      "Amount of RAM in megabytes",
      1, 1<<31,
      BX_DEFAULT_MEM_MEGS);
  bx_options.ips = new bx_param_num_c (BXP_IPS, 
      "ips",
      "Emulated instructions per second, used to calibrate bochs emulated\ntime with wall clock time.",
      1, 1<<31,
      500000);
  bx_options.vga_update_interval = new bx_param_num_c (BXP_VGA_UPDATE_INTERVAL,
      "vga_update_interval",
      "Number of microseconds between VGA updates",
      1, 1<<31,
      30000);
  bx_options.vga_update_interval->set_handler (bx_param_handler);
  bx_options.mouse_enabled = new bx_param_num_c (BXP_MOUSE_ENABLED,
      "mouse_enabled",
      "Controls whether the mouse sends events to bochs",
      0, 1, 
      0);
  bx_options.mouse_enabled->set_handler (bx_param_handler);
  bx_options.rom.path = new bx_param_string_c (BXP_ROM_PATH,
      "romimage",
      "Pathname of ROM image to load",
      BX_PATHNAME_LEN, "");
  bx_options.rom.address = new bx_param_num_c (BXP_ROM_ADDRESS,
      "romaddr",
      "The address at which the ROM image should be loaded",
      0, 1<<31, 
      0);
  bx_options.vgarom.path = new bx_param_string_c (BXP_VGA_ROM_PATH,
      "vgaromimage",
      "Pathname of VGA ROM image to load",
      BX_PATHNAME_LEN, "");
}

int
main(int argc, char *argv[])
{
  // To deal with initialization order problems inherent in C++, use the macros
  // SAFE_GET_IOFUNC and SAFE_GET_GENLOG to retrieve "io" and "genlog" in all
  // constructors or functions called by constructors.  The macros test for
  // NULL and create the object if necessary, then return it.  Ensure that io
  // and genlog get created, by making one reference to each macro right here.
  // All other code can reference io and genlog directly.
  SAFE_GET_IOFUNC();
  SAFE_GET_GENLOG();

  bx_print_header ();
  bx_init_bx_dbg ();
  bx_init_options ();

  int read_rc_already = 0;
#if BX_USE_CONTROL_PANEL
  // Display the pre-simulation control panel.
  init_siminterface ();
  if ((bx_control_panel (BX_CPANEL_START_MAIN)) != BX_DISABLE_CONTROL_PANEL)
    read_rc_already = 1;
#endif
  if (!read_rc_already) {
    /* parse configuration file and command line arguments */
    char *bochsrc = bx_find_bochsrc ();
    if (bochsrc)
      bx_read_configuration (bochsrc);

    if (bochsrc == NULL && argc == 1) {
      // no bochsrc used.  This is legal since they may have 
      // everything on the command line.  However if they have no
      // arguments then give them some friendly advice.
      fprintf (stderr, "%s\n", divider);
      fprintf (stderr, "Before running Bochs, you should cd to a directory which contains\n");
      fprintf (stderr, "a .bochsrc file and a disk image.  If you downloaded a binary package,\n");
      fprintf (stderr, "all the necessary files are already on your disk.\n");
#if defined(WIN32)
      fprintf (stderr, "\nFor Windows installations, go to the dlxlinux direectory and\n");
      fprintf (stderr, "double-click on the start.bat script.\n");
#elif !defined(macintosh)
      fprintf (stderr, "\nFor UNIX installations, try running \"bochs-dlx\" for a demo.  This script\n");
      fprintf (stderr, "is basically equivalent to typing:\n");
      fprintf (stderr, "   cd /usr/local/bochs/dlxlinux\n");
      fprintf (stderr, "   bochs\n");
#endif
      exit(1);
    }
  }

#if BX_DEBUGGER
  // If using the debugger, it will take control and call
  // bx_init_hardware() and cpu_loop()
  bx_dbg_main(argc, argv);
#else

  if (bx_parse_cmdline (argc, argv)) {
    fprintf (stderr, "There were errors while parsing the command line.\n");
    fprintf (stderr, "Bochs is exiting.\n");
    exit (1);
  }

  bx_init_hardware();

  if (bx_options.load32bitOSImage.whichOS) {
    void bx_load32bitOSimagehack(void);
    bx_load32bitOSimagehack();
    }

#if BX_USE_CONTROL_PANEL
  SIM->set_init_done (1);
#endif

  if (BX_SMP_PROCESSORS == 1) {
    // only one processor, run as fast as possible by not messing with
    // quantums and loops.
    BX_CPU(0)->cpu_loop(1);
    BX_PANIC (("cpu_loop should never return in a single-processor simulation"));
  } else {
    // SMP simulation: do a few instructions on each processor, then switch
    // to another.  Increasing quantum speeds up overall performance, but
    // reduces granularity of synchronization between processors.
    int processor = 0;
    int quantum = 5;
    while (1) {
      // do some instructions in each processor
      BX_CPU(processor)->cpu_loop(quantum);
      processor = (processor+1) % BX_SMP_PROCESSORS;
      if (processor == 0) 
	BX_TICKN(quantum);
    }
  }
#endif

  return(0);
}

int
bx_read_configuration (char *rcfile)
{
  // parse rcfile first, then parse arguments in order.
  BX_INFO (("reading configuration from %s", rcfile));
  if (parse_bochsrc(rcfile) < 0) {
    BX_ERROR (("reading from %s failed", rcfile));
    return -1;
  }
  return 0;
}

int bx_parse_cmdline (int argc, char *argv[])
{
  if (argc > 1)
    BX_INFO (("parsing command line arguments"));

  int n = 2;
  while (n <= argc) {
    parse_line_unformatted("cmdline args", argv[n-1]);
    n++;
  }
  return 0;
}

  int
bx_init_hardware()
{
  // all configuration has been read, now initialize everything.

  for (int level=0; level<N_LOGLEV; level++)
    io->set_log_action (level, bx_options.log.actions[level]);

  bx_pc_system.init_ips(bx_options.ips->get ());

  if(bx_options.log.filename[0]!='-') {
    BX_INFO (("using log file %s", bx_options.log.filename));
    io->init_log(bx_options.log.filename);
  }

  // set up memory and CPU objects
#if BX_SUPPORT_APIC
  memset(apic_index, 0, sizeof(apic_index[0]) * APIC_MAX_ID);
#endif

#if BX_SMP_PROCESSORS==1
  BX_MEM(0)->init_memory(bx_options.memory.size->get () * 1024*1024);
  BX_MEM(0)->load_ROM(bx_options.rom.path->getptr (), bx_options.rom.address->get ());
  BX_MEM(0)->load_ROM(bx_options.vgarom.path->getptr (), 0xc0000);
  BX_CPU(0)->init (BX_MEM(0));
  BX_CPU(0)->reset(BX_RESET_HARDWARE);
#else
  // SMP initialization
  bx_mem_array[0] = new BX_MEM_C ();
  bx_mem_array[0]->init_memory(bx_options.memory.size->get () * 1024*1024);
  bx_mem_array[0]->load_ROM(bx_options.rom.path->getptr (), bx_options.rom.address->get ());
  bx_mem_array[0]->load_ROM(bx_options.vgarom.path, 0xc0000);
  for (int i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i) = new BX_CPU_C ();
    BX_CPU(i)->init (bx_mem_array[0]);
    // assign apic ID from the index of this loop
    // if !BX_SUPPORT_APIC, this will not compile.
    BX_CPU(i)->local_apic.set_id (i);
    BX_CPU(i)->reset(BX_RESET_HARDWARE);
  }
#endif

#if BX_DEBUGGER == 0
  bx_devices.init(BX_MEM(0));
  bx_gui.init_signal_handlers ();
  bx_pc_system.start_timers();
#endif
  BX_DEBUG(("bx_init_hardware is setting signal handlers"));
// if not using debugger, then we can take control of SIGINT.
// If using debugger, it needs control of this.
#if BX_DEBUGGER==0
  signal(SIGINT, bx_signal_handler);
#endif

#if BX_SHOW_IPS
#ifndef __MINGW32__
  signal(SIGALRM, bx_signal_handler);
#endif
  alarm( 1 );
#endif

  return(0);
}



  void
bx_init_bx_dbg (void)
{
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
#ifdef MAGIC_BREAKPOINT
  bx_dbg.magic_break_enabled = 0;
#endif

}


  void
bx_atexit(void)
{
  static Boolean been_here = 0;


#if BX_PROVIDE_DEVICE_MODELS==1
  if (been_here == 0) {
    bx_pc_system.exit();
    }
#endif

#if BX_DEBUGGER == 0
  for (int cpu=0; cpu<BX_SMP_PROCESSORS; cpu++)
    if (BX_CPU(cpu)) BX_CPU(cpu)->atexit();
#endif

#if BX_PCI_SUPPORT
    if (bx_options.i440FXSupport) {
      bx_devices.pci->print_i440fx_state();
      }
#endif
}

#if (BX_PROVIDE_CPU_MEMORY==1) && (BX_EMULATE_HGA_DUMPS>0)
  void
bx_emulate_hga_dumps_timer(void)
{
  void bx_hga_set_video_memory(Bit8u *ptr);

  bx_hga_set_video_memory(&bx_phy_memory[0xb0000]);
}
#endif

#if BX_PROVIDE_MAIN

char *
bx_find_bochsrc ()
{
  FILE *fd = NULL;
  char rcfile[512];
  Bit32u retry = 0, found = 0;
  // try several possibilities for the bochsrc before giving up
  while (!found) {
    rcfile[0] = 0;
    switch (retry++) {
    case 0: strcpy (rcfile, ".bochsrc"); break;
    case 1: strcpy (rcfile, "bochsrc"); break;
    case 2: strcpy (rcfile, "bochsrc.txt"); break;
    case 3:
#if (!defined(WIN32) && !defined(macintosh))
      // only try this on unix
      {
      char *ptr = getenv("HOME");
      if (ptr) sprintf (rcfile, "%s/.bochsrc", ptr);
      }
#endif
      break;
    default:
      return NULL;
    }
    if (rcfile[0]) {
      BX_DEBUG (("looking for configuration in %s", rcfile));
      fd = fopen(rcfile, "r");
      if (fd) found = 1;
    }
  }
  assert (fd != NULL && rcfile[0] != 0);
  fclose (fd);
  return strdup (rcfile);
}

  static int
parse_bochsrc(char *rcfile)
{
  FILE *fd = NULL;
  char *ret;
  char line[512];

  // try several possibilities for the bochsrc before giving up

  fd = fopen (rcfile, "r");
  if (!fd) return -1;

  do {
    ret = fgets(line, sizeof(line)-1, fd);
    line[sizeof(line) - 1] = '\0';
    int len = strlen(line);
    if (len>0)
      line[len-1] = '\0';
    if ((ret != NULL) && strlen(line)) {
      parse_line_unformatted(rcfile, line);
      }
    } while (!feof(fd));
  return 0;
}

  static void
parse_line_unformatted(char *context, char *line)
{
  char *ptr;
  unsigned i, string_i;
  char string[512];
  char *params[40];
  int num_params;
  Boolean inquotes = 0;

  if (line == NULL) return;

  // if passed nothing but whitespace, just return
  for (i=0; i<strlen(line); i++) {
    if (!isspace(line[i])) break;
    }
  if (i>=strlen(line))
    return;

  num_params = 0;

  ptr = strtok(line, ":");
  while (ptr) {
    string_i = 0;
    for (i=0; i<strlen(ptr); i++) {
      if (ptr[i] == '"')
        inquotes = !inquotes;
      else
        if (!isspace(ptr[i]) || inquotes) {
          string[string_i++] = ptr[i];
          }
      }
    string[string_i] = '\0';
    strcpy(ptr, string);
    params[num_params++] = ptr;
    ptr = strtok(NULL, ",");
    }
  parse_line_formatted(context, num_params, &params[0]);
}

  static void
parse_line_formatted(char *context, int num_params, char *params[])
{
  int i;

  if (num_params < 1) return;

  if (params[0][0] == '#') return; /* comment */
  else if (!strcmp(params[0], "floppya")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "2_88=", 5)) {
        strcpy(bx_options.floppya.path, &params[i][5]);
        bx_options.floppya.type = BX_FLOPPY_2_88;
        }
      else if (!strncmp(params[i], "1_44=", 5)) {
        strcpy(bx_options.floppya.path, &params[i][5]);
        bx_options.floppya.type = BX_FLOPPY_1_44;
        }
      else if (!strncmp(params[i], "1_2=", 4)) {
        strcpy(bx_options.floppya.path, &params[i][4]);
        bx_options.floppya.type = BX_FLOPPY_1_2;
        }
      else if (!strncmp(params[i], "720k=", 5)) {
        strcpy(bx_options.floppya.path, &params[i][5]);
        bx_options.floppya.type = BX_FLOPPY_720K;
        }
      else if (!strncmp(params[i], "status=ejected", 14)) {
        bx_options.floppya.initial_status = BX_EJECTED;
        }
      else if (!strncmp(params[i], "status=inserted", 15)) {
        bx_options.floppya.initial_status = BX_INSERTED;
        }
      else {
        BX_PANIC(("%s: floppya attribute '%s' not understood.", context,
          params[i]));
        }
      }
    }

  else if (!strcmp(params[0], "floppyb")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "2_88=", 5)) {
        strcpy(bx_options.floppyb.path, &params[i][5]);
        bx_options.floppyb.type = BX_FLOPPY_2_88;
        }
      else if (!strncmp(params[i], "1_44=", 5)) {
        strcpy(bx_options.floppyb.path, &params[i][5]);
        bx_options.floppyb.type = BX_FLOPPY_1_44;
        }
      else if (!strncmp(params[i], "1_2=", 4)) {
        strcpy(bx_options.floppyb.path, &params[i][4]);
        bx_options.floppyb.type = BX_FLOPPY_1_2;
        }
      else if (!strncmp(params[i], "720k=", 5)) {
        strcpy(bx_options.floppyb.path, &params[i][5]);
        bx_options.floppyb.type = BX_FLOPPY_720K;
        }
      else if (!strncmp(params[i], "status=ejected", 14)) {
        bx_options.floppyb.initial_status = BX_EJECTED;
        }
      else if (!strncmp(params[i], "status=inserted", 15)) {
        bx_options.floppyb.initial_status = BX_INSERTED;
        }
      else {
        BX_PANIC(("%s: floppyb attribute '%s' not understood.", context,
          params[i]));
        }
      }
    }

  else if (!strcmp(params[0], "diskc")) {
    if (num_params != 5) {
      BX_PANIC(("%s: diskc directive malformed.", context));
      }
    if (strncmp(params[1], "file=", 5) ||
        strncmp(params[2], "cyl=", 4) ||
        strncmp(params[3], "heads=", 6) ||
        strncmp(params[4], "spt=", 4)) {
      BX_PANIC(("%s: diskc directive malformed.", context));
      }
    strcpy(bx_options.diskc.path, &params[1][5]);
    bx_options.diskc.cylinders = atol( &params[2][4] );
    bx_options.diskc.heads     = atol( &params[3][6] );
    bx_options.diskc.spt       = atol( &params[4][4] );
    bx_options.diskc.present = 1;
    }
  else if (!strcmp(params[0], "diskd")) {
    if (num_params != 5) {
      BX_PANIC(("%s: diskd directive malformed.", context));
      }
    if (strncmp(params[1], "file=", 5) ||
        strncmp(params[2], "cyl=", 4) ||
        strncmp(params[3], "heads=", 6) ||
        strncmp(params[4], "spt=", 4)) {
      BX_PANIC(("%s: diskd directive malformed.", context));
      }
    strcpy(bx_options.diskd.path, &params[1][5]);
    bx_options.diskd.cylinders = atol( &params[2][4] );
    bx_options.diskd.heads     = atol( &params[3][6] );
    bx_options.diskd.spt       = atol( &params[4][4] );
    bx_options.diskd.present = 1;
    }

  else if (!strcmp(params[0], "cdromd")) {
    if (num_params != 3) {
      BX_PANIC(("%s: cdromd directive malformed.", context));
      }
    if (strncmp(params[1], "dev=", 4) || strncmp(params[2], "status=", 7)) {
      BX_PANIC(("%s: cdromd directive malformed.", context));
      }
    strcpy(bx_options.cdromd.dev, &params[1][4]);
    if (!strcmp(params[2], "status=inserted"))
      bx_options.cdromd.inserted = 1;
    else if (!strcmp(params[2], "status=ejected"))
      bx_options.cdromd.inserted = 0;
    else {
      BX_PANIC(("%s: cdromd directive malformed.", context));
      }
    bx_options.cdromd.present = 1;
    }

  else if (!strcmp(params[0], "boot")) {
    if (!strcmp(params[1], "a") ||
        !strcmp(params[1], "c")) {
      strcpy(bx_options.bootdrive, params[1]);
      }
    else {
      BX_PANIC(("%s: boot directive with unknown boot device '%s'.  use 'a' or 'c'.", context, params[1]));
      }
    }
  else if (!strcmp(params[0], "log")) {
    if (num_params != 2) {
      BX_PANIC(("%s: log directive has wrong # args.", context));
      }
    strcpy(bx_options.log.filename, params[1]);
    }
  else if (!strcmp(params[0], "panic")) {
    if (num_params != 2) {
      BX_PANIC(("%s: panic directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      BX_PANIC(("%s: panic directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_FATAL;
    else if (!strcmp (action, "report"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_REPORT;
    else if (!strcmp (action, "ignore"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_IGNORE;
    else if (!strcmp (action, "ask"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_ASK;
    else {
      BX_PANIC(("%s: panic directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "error")) {
    if (num_params != 2) {
      BX_PANIC(("%s: error directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      BX_PANIC(("%s: error directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      bx_options.log.actions[LOGLEV_ERROR] = ACT_FATAL;
    else if (!strcmp (action, "report"))
      bx_options.log.actions[LOGLEV_ERROR] = ACT_REPORT;
    else if (!strcmp (action, "ignore"))
      bx_options.log.actions[LOGLEV_ERROR] = ACT_IGNORE;
    else if (!strcmp (action, "ask"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_ASK;
    else {
      BX_PANIC(("%s: error directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "info")) {
    if (num_params != 2) {
      BX_PANIC(("%s: info directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      BX_PANIC(("%s: info directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      bx_options.log.actions[LOGLEV_INFO] = ACT_FATAL;
    else if (!strcmp (action, "report"))
      bx_options.log.actions[LOGLEV_INFO] = ACT_REPORT;
    else if (!strcmp (action, "ignore"))
      bx_options.log.actions[LOGLEV_INFO] = ACT_IGNORE;
    else if (!strcmp (action, "ask"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_ASK;
    else {
      BX_PANIC(("%s: info directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "debug")) {
    if (num_params != 2) {
      BX_PANIC(("%s: debug directive malformed.", context));
      }
    if (strncmp(params[1], "action=", 7)) {
      BX_PANIC(("%s: debug directive malformed.", context));
      }
    char *action = 7 + params[1];
    if (!strcmp(action, "fatal"))
      bx_options.log.actions[LOGLEV_DEBUG] = ACT_FATAL;
    else if (!strcmp (action, "report"))
      bx_options.log.actions[LOGLEV_DEBUG] = ACT_REPORT;
    else if (!strcmp (action, "ignore"))
      bx_options.log.actions[LOGLEV_DEBUG] = ACT_IGNORE;
    else if (!strcmp (action, "ask"))
      bx_options.log.actions[LOGLEV_PANIC] = ACT_ASK;
    else {
      BX_PANIC(("%s: debug directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "romimage")) {
    if (num_params != 3) {
      BX_PANIC(("%s: romimage directive: wrong # args.", context));
      }
    if (strncmp(params[1], "file=", 5)) {
      BX_PANIC(("%s: romimage directive malformed.", context));
      }
    if (strncmp(params[2], "address=", 8)) {
      BX_PANIC(("%s: romimage directive malformed.", context));
      }
    bx_options.rom.path->set (&params[1][5]);
    if ( (params[2][8] == '0') && (params[2][9] == 'x') )
      bx_options.rom.address->set (strtoul (&params[2][8], NULL, 16));
    else
      bx_options.rom.address->set (strtoul (&params[2][8], NULL, 10));
    }
  else if (!strcmp(params[0], "vgaromimage")) {
    if (num_params != 2) {
      BX_PANIC(("%s: vgaromimage directive: wrong # args.", context));
      }
    bx_options.vgarom.path->set (params[1]);
    }
  else if (!strcmp(params[0], "vga_update_interval")) {
    if (num_params != 2) {
      BX_PANIC(("%s: vga_update_interval directive: wrong # args.", context));
      }
    bx_options.vga_update_interval->set (atol(params[1]));
    if (bx_options.vga_update_interval->get () < 50000) {
      BX_INFO(("%s: vga_update_interval seems awfully small!", context));
      }
    }
  else if (!strcmp(params[0], "keyboard_serial_delay")) {
    if (num_params != 2) {
      BX_PANIC(("%s: keyboard_serial_delay directive: wrong # args.", context));
      }
    bx_options.keyboard_serial_delay = atol(params[1]);
    if (bx_options.keyboard_serial_delay < 5) {
      BX_PANIC(("%s: keyboard_serial_delay not big enough!", context));
      }
    }
  else if (!strcmp(params[0], "megs")) {
    if (num_params != 2) {
      BX_PANIC(("%s: megs directive: wrong # args.", context));
      }
    bx_options.memory.size->set (atol(params[1]));
    }
  else if (!strcmp(params[0], "floppy_command_delay")) {
    if (num_params != 2) {
      BX_PANIC(("%s: floppy_command_delay directive: wrong # args.", context));
      }
    bx_options.floppy_command_delay = atol(params[1]);
    if (bx_options.floppy_command_delay < 100) {
      BX_PANIC(("%s: floppy_command_delay not big enough!", context));
      }
    }
  else if (!strcmp(params[0], "ips")) {
    if (num_params != 2) {
      BX_PANIC(("%s: ips directive: wrong # args.", context));
      }
    bx_options.ips->set (atol(params[1]));
    if (bx_options.ips->get () < 200000) {
      BX_INFO(("%s: WARNING: ips is AWFULLY low!", context));
      }
    }

  else if (!strcmp(params[0], "mouse")) {
    if (num_params != 2) {
      BX_PANIC(("%s: mouse directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: mouse directive malformed.", context));
      }
    if (params[1][8] == '0' || params[1][8] == '1')
      bx_options.mouse_enabled->set (params[1][8] - '0');
    else
      BX_PANIC(("%s: mouse directive malformed.", context));
    }
  else if (!strcmp(params[0], "private_colormap")) {
    if (num_params != 2) {
      BX_PANIC(("%s: private_colormap directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: private_colormap directive malformed.", context));
      }
    if (params[1][8] == '0')
      bx_options.private_colormap = 0;
    else if (params[1][8] == '1')
      bx_options.private_colormap = 1;
    else {
      BX_PANIC(("%s: private_colormap directive malformed.", context));
      }
    }

  else if (!strcmp(params[0], "sb16")) {
    for (i=1; i<num_params; i++) {
        bx_options.sb16.present = 1;
      if (!strncmp(params[i], "midi=", 5)) {
	bx_options.sb16.midifile = strdup(&params[i][5]);
        }
      else if (!strncmp(params[i], "midimode=", 9)) {
	bx_options.sb16.midimode = atol(&params[i][9]);
        }
      else if (!strncmp(params[i], "wave=", 5)) {
	bx_options.sb16.wavefile = strdup(&params[i][5]);
        }
      else if (!strncmp(params[i], "wavemode=", 9)) {
	bx_options.sb16.wavemode = atol(&params[i][9]);
        }
      else if (!strncmp(params[i], "log=", 4)) {
	bx_options.sb16.logfile = strdup(&params[i][4]);
        }
      else if (!strncmp(params[i], "loglevel=", 9)) {
	bx_options.sb16.loglevel = atol(&params[i][9]);
        }
      else if (!strncmp(params[i], "dmatimer=", 9)) {
	bx_options.sb16.dmatimer = atol(&params[i][9]);
        }
      }
    }

  else if (!strcmp(params[0], "i440fxsupport")) {
    if (num_params != 2) {
      BX_PANIC(("%s: i440FXSupport directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: i440FXSupport directive malformed.", context));
      }
    if (params[1][8] == '0')
      bx_options.i440FXSupport = 0;
    else if (params[1][8] == '1')
      bx_options.i440FXSupport = 1;
    else {
      BX_PANIC(("%s: i440FXSupport directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "newharddrivesupport")) {
    if (num_params != 2) {
      BX_PANIC(("%s: newharddrivesupport directive malformed.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: newharddrivesupport directive malformed.", context));
      }
    if (params[1][8] == '0')
      bx_options.newHardDriveSupport = 0;
    else if (params[1][8] == '1')
      bx_options.newHardDriveSupport = 1;
    else {
      BX_PANIC(("%s: newharddrivesupport directive malformed.", context));
      }
    }
  else if (!strcmp(params[0], "cmosimage")) {
    if (num_params != 2) {
      BX_PANIC(("%s: cmosimage directive: wrong # args.", context));
      }
    bx_options.cmos.path = strdup(params[1]);
    bx_options.cmos.cmosImage = 1;                // CMOS Image is true
    }
  else if (!strcmp(params[0], "time0")) {
    if (num_params != 2) {
      BX_PANIC(("%s: time0 directive: wrong # args.", context));
      }
    bx_options.cmos.time0 = atoi(params[1]);
    }
#ifdef MAGIC_BREAKPOINT
  else if (!strcmp(params[0], "magic_break")) {
    if (num_params != 2) {
      BX_PANIC(("%s: magic_break directive: wrong # args.", context));
      }
    if (strncmp(params[1], "enabled=", 8)) {
      BX_PANIC(("%s: magic_break directive malformed.", context));
      }
    if (params[1][8] == '0') {
      BX_INFO(("Ignoring magic break points"));
      bx_dbg.magic_break_enabled = 0;
      }
    else if (params[1][8] == '1') {
      BX_INFO(("Stopping on magic break points"));
      bx_dbg.magic_break_enabled = 1;
      }
    else {
      BX_PANIC(("%s: magic_break directive malformed.", context));
      }
    }
#endif
  else if (!strcmp(params[0], "ne2k")) {
    int tmp[6];
    bx_options.ne2k.valid = 0;
    if ((num_params < 4) || (num_params > 6)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
      }
    bx_options.ne2k.ethmod = "null";
    if (strncmp(params[1], "ioaddr=", 7)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
      }
    if (strncmp(params[2], "irq=", 4)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
      }
    if (strncmp(params[3], "mac=", 4)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
      }
    bx_options.ne2k.ioaddr = strtoul(&params[1][7], NULL, 16);
    bx_options.ne2k.irq = atol(&params[2][4]);
    i = sscanf(&params[3][4], "%x:%x:%x:%x:%x:%x",
             &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
    if (i != 6) {
      BX_PANIC(("%s: ne2k mac address malformed.", context));
      return;
      }
    for (i=0;i<6;i++)
      bx_options.ne2k.macaddr[i] = tmp[i];
    if (num_params > 4) {
      if (strncmp(params[4], "ethmod=", 7)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
        }
      bx_options.ne2k.ethmod = strdup(&params[4][7]);
      if (num_params == 6) {
      if (strncmp(params[5], "ethdev=", 7)) {
      BX_PANIC(("%s: ne2k directive malformed.", context));
      return;
          }
      bx_options.ne2k.ethdev = strdup(&params[5][7]);
        }
      }
    bx_options.ne2k.valid = 1;
    }

  else if (!strcmp(params[0], "load32bitOSImage")) {
    if ( (num_params!=4) && (num_params!=5) ) {
      BX_PANIC(("%s: load32bitOSImage directive: wrong # args.", context));
      }
    if (strncmp(params[1], "os=", 3)) {
      BX_PANIC(("%s: load32bitOSImage: directive malformed.", context));
      }
    if (!strcmp(&params[1][3], "nullkernel")) {
      bx_options.load32bitOSImage.whichOS = Load32bitOSNullKernel;
      }
    else if (!strcmp(&params[1][3], "linux")) {
      bx_options.load32bitOSImage.whichOS = Load32bitOSLinux;
      }
    else {
      BX_PANIC(("%s: load32bitOSImage: unsupported OS.", context));
      }
    if (strncmp(params[2], "path=", 5)) {
      BX_PANIC(("%s: load32bitOSImage: directive malformed.", context));
      }
    if (strncmp(params[3], "iolog=", 6)) {
      BX_PANIC(("%s: load32bitOSImage: directive malformed.", context));
      }
    bx_options.load32bitOSImage.path = strdup(&params[2][5]);
    bx_options.load32bitOSImage.iolog = strdup(&params[3][6]);
    if (num_params == 5) {
      if (strncmp(params[4], "initrd=", 7)) {
        BX_PANIC(("%s: load32bitOSImage: directive malformed.", context));
        }
      bx_options.load32bitOSImage.initrd = strdup(&params[4][7]);
      }
    }

  else {
    BX_PANIC(( "%s: directive '%s' not understood", context, params[0]));
    }

  if (bx_options.diskd.present && bx_options.cdromd.present)
    BX_PANIC(("At present, using both diskd and cdromd at once is not supported."));
}

static char *fdtypes[] = {
  "none", "1_2", "1_44", "2_88", "720k"
};

int 
bx_write_floppy_options (FILE *fp, int drive, bx_floppy_options *opt)
{
  BX_ASSERT (drive==0 || drive==1);
  if (opt->type == BX_FLOPPY_NONE) {
    fprintf (fp, "# no floppy%c\n", (char)'a'+drive);
    return 0;
  }
  BX_ASSERT (opt->type > BX_FLOPPY_NONE && opt->type <= BX_FLOPPY_LAST);
  fprintf (fp, "floppy%c: %s=%s, status=%s\n", 
    (char)'a'+drive, 
    fdtypes[opt->type - BX_FLOPPY_NONE],
    opt->path,
    opt->initial_status==BX_EJECTED ? "ejected" : "inserted");
  return 0;
}

int 
bx_write_disk_options (FILE *fp, int drive, bx_disk_options *opt)
{
  if (!opt->present) {
    fprintf (fp, "# no disk%c\n", (char)'c'+drive);
    return 0;
  }
  fprintf (fp, "disk%c: file=\"%s\", cyl=%d, heads=%d, spt=%d\n",
     (char)'c'+drive,
     opt->path,
     opt->cylinders,
     opt->heads,
     opt->spt);
  return 0;
}

int 
bx_write_cdrom_options (FILE *fp, int drive, bx_cdrom_options *opt)
{
  BX_ASSERT (drive == 0);
  if (!opt->present) {
    fprintf (fp, "# no cdromd\n");
    return 0;
  }
  fprintf (fp, "cdromd: dev=%s, status=%s\n", 
    opt->dev,
    opt->inserted ? "inserted" : "ejected");
  return 0;
}

int
bx_write_sb16_options (FILE *fp, bx_sb16_options *opt)
{
  if (!opt->present) {
    fprintf (fp, "# no sb16\n");
    return 0;
  }
  fprintf (fp, "sb16: midimode=%d, midi=%s, wavemode=%d, wave=%s, loglevel=%d, log=%s, dmatimer=%d\n", opt->midimode, opt->midifile, opt->wavemode, opt->wavefile, opt->loglevel, opt->logfile, opt->dmatimer);
  return 0;
}

int
bx_write_ne2k_options (FILE *fp, bx_ne2k_options *opt)
{
  if (!opt->valid) {
    fprintf (fp, "# no ne2k\n");
    return 0;
  }
  fprintf (fp, "ne2k: ioaddr=0x%x, irq=%d, mac=%02x:%02x:%02x:%02x:%02x:%02x, ethmod=%s, ethdev=%s\n",
      opt->ioaddr, 
      opt->irq,
      opt->macaddr[0],
      opt->macaddr[1],
      opt->macaddr[2],
      opt->macaddr[3],
      opt->macaddr[4],
      opt->macaddr[5],
      opt->ethmod,
      opt->ethdev);
  return 0;
}

int
bx_write_loader_options (FILE *fp, bx_load32bitOSImage_t *opt)
{
  if (opt->whichOS == 0) {
    fprintf (fp, "# no loader\n");
    return 0;
  }
  BX_ASSERT(opt->whichOS == Load32bitOSLinux || opt->whichOS == Load32bitOSNullKernel);
  fprintf (fp, "load32bitOSImage: os=%s, path=%s, iolog=%s, initrd=%s\n",
      (opt->whichOS == Load32bitOSLinux) ? "linux" : "nullkernel",
      opt->path,
      opt->iolog,
      opt->initrd);
  return 0;
}

int
bx_write_log_options (FILE *fp, bx_log_options *opt)
{
  fprintf (fp, "log: %s\n", opt->filename);
  // no syntax to describe all the possible action settings for every 
  // device.  Instead, take a vote and record the most popular action
  // for each level of event.
  int action_tally[N_ACT];
  int most_popular_action[N_LOGLEV];
  int i,lev;
  for (lev = 0; lev < N_LOGLEV; lev++) {
    // clear tally
    for (i=0; i<N_ACT; i++)
      action_tally[i] = 0;
    // count how many devices use each action
    for (i=0; i<io->get_n_logfns (); i++) {
      logfunc_t *fn = io->get_logfn (i);
      int action = fn->getonoff(lev);
      BX_ASSERT (action >= 0 && action < N_ACT);
      action_tally[action]++;
    }
    // count the votes
    int best = 0, best_votes = action_tally[0];
    for (i=1; i<N_ACT; i++) {
      if (action_tally[i] > best_votes) {
	best = i;
	best_votes = action_tally[i];
      }
    }
    most_popular_action[lev] = best;
  }
  fprintf (fp, "panic: action=%s\n",
      io->getaction(most_popular_action[LOGLEV_PANIC]));
  fprintf (fp, "error: action=%s\n",
      io->getaction(most_popular_action[LOGLEV_ERROR]));
  fprintf (fp, "info: action=%s\n",
      io->getaction(most_popular_action[LOGLEV_INFO]));
  fprintf (fp, "debug: action=%s\n",
      io->getaction(most_popular_action[LOGLEV_DEBUG]));
  return 0;
}

// return values:
//   0: written ok
//  -1: failed
//  -2: already exists, and overwrite was off
int
bx_write_configuration (char *rc, int overwrite)
{
  BX_INFO (("write configuration to %s\n", rc));
  // check if it exists.  If so, only proceed if overwrite is set.
  FILE *fp = fopen (rc, "r");
  if (fp != NULL) {
    fclose (fp);
    if (!overwrite) return -2;
  }
  fp = fopen (rc, "w");
  if (fp == NULL) return -1;
  // finally it's open and we can start writing.
  fprintf (fp, "# configuration file generated by Bochs\n");
  // it would be nice to put this type of function as methods on
  // the structs like bx_floppy_options::print or something.
  bx_write_floppy_options (fp, 0, &bx_options.floppya);
  bx_write_floppy_options (fp, 1, &bx_options.floppyb);
  bx_write_disk_options (fp, 0, &bx_options.diskc);
  bx_write_disk_options (fp, 1, &bx_options.diskd);
  bx_write_cdrom_options (fp, 0, &bx_options.cdromd);
  if (strlen (bx_options.rom.path->getptr ()) < 1)
    fprintf (fp, "romimage: file=%s, address=0x%05x\n", bx_options.rom.path->getptr(), (unsigned int)bx_options.rom.address);
  else
    fprintf (fp, "# no romimage\n");
  if (bx_options.vgarom.path)
    fprintf (fp, "vgaromimage: %s\n", bx_options.vgarom.path);
  else
    fprintf (fp, "# no vgaromimage\n");
  fprintf (fp, "megs: %d\n", bx_options.memory.size->get ());
  bx_write_sb16_options (fp, &bx_options.sb16);
  fprintf (fp, "boot: %s\n", bx_options.bootdrive);
  fprintf (fp, "vga_update_interval: %lu\n", bx_options.vga_update_interval->get ());
  fprintf (fp, "keyboard_serial_delay: %lu\n", bx_options.keyboard_serial_delay);
  fprintf (fp, "floppy_command_delay: %lu\n", bx_options.floppy_command_delay);
  fprintf (fp, "ips: %lu\n", bx_options.ips);
  fprintf (fp, "mouse: enabled=%d\n", bx_options.mouse_enabled->get ());
  fprintf (fp, "private_colormap: enabled=%d\n", bx_options.private_colormap);
  fprintf (fp, "i440fxsupport: enabled=%d\n", bx_options.i440FXSupport);
  fprintf (fp, "time0: %u\n", bx_options.cmos.time0);
  bx_write_ne2k_options (fp, &bx_options.ne2k);
  fprintf (fp, "newharddrivesupport: enabled=%d\n", bx_options.newHardDriveSupport);
  bx_write_loader_options (fp, &bx_options.load32bitOSImage);
  bx_write_log_options (fp, &bx_options.log);
  fclose (fp);
  return 0;
}
#endif // #if BX_PROVIDE_MAIN

  void
bx_signal_handler( int signum)
{
#if BX_GUI_SIGHANDLER
  // GUI signal handler gets first priority, if the mask says it's wanted
  if ((1<<signum) & bx_gui.get_sighandler_mask ()) {
    bx_gui.sighandler (signum);
    return;
  }
#endif

#if BX_SHOW_IPS
  extern unsigned long ips_count;

  if (signum == SIGALRM ) {
    BX_INFO(("ips = %lu", ips_count));
    ips_count = 0;
#ifndef __MINGW32__
    signal(SIGALRM, bx_signal_handler);
    alarm( 1 );
#endif
    return;
    }
#endif

#if BX_GUI_SIGHANDLER
  if ((1<<signum) & bx_gui.get_sighandler_mask ()) {
    bx_gui.sighandler (signum);
    return;
  }
#endif
  BX_PANIC(("SIGNAL %u caught", signum));
}

