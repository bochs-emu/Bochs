/////////////////////////////////////////////////////////////////////////
// $Id: logio.cc,v 1.20 2002-06-28 14:03:47 cbothamy Exp $
/////////////////////////////////////////////////////////////////////////
//
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

static char *divider = "========================================================================";

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

	// sets the default logprefix
	strcpy(logprefix,"%1$011lld%2$c%4$s");
	n_logfn = 0;
	init_log(stderr);
	log = new logfunc_t(this);
	LOG_THIS put("IO");
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
iofunctions::init_log(const char *fn)
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
		  	BX_PANIC(("Couldn't open log file: %s", fn));
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

// This converts the option string to a printf style string with the following args:
// 1. timer, 2. event, 3. cpu0 eip, 4. device
void 
iofunctions::set_log_prefix(const char* prefix) {
	int   i=0;
	char *s=(char *)prefix;
	char *f;
	
	strcpy(logprefix,"");
	while (*s!=0) {
		if (*s!='%') {
			if (strlen(logprefix)<sizeof(logprefix)-1) 
				strncat(logprefix,s,1);
			else break;
		}
		else {
			f="";
			switch (*(++s)) {
				case 't': f="%1$011lld"; break;
				case 'i': f="%3$08x"; break;
				case 'e': f="%2$c"; break;
				case 'd': f="%4$s"; break;
				case '%': f="%%"; break;
			}
			if(strlen(logprefix)+strlen(f)<sizeof(logprefix)-1)
				strcat(logprefix,f);
			else break;
			if(*s==0)break;
		}
		s++;
	}
}

//  iofunctions::out( class, level, prefix, fmt, ap)
//  DO NOT nest out() from ::info() and the like.
//    fmt and ap retained for direct printinf from iofunctions only!

void
iofunctions::out(int f, int l, const char *prefix, const char *fmt, va_list ap)
{
	char c=' ';
	assert (magic==MAGIC_LOGNUM);
	assert (this != NULL);
	assert (logfd != NULL);

	//if( showtick )
	//	fprintf(logfd, "%011lld", bx_pc_system.time_ticks());

	switch(l) {
		case LOGLEV_INFO: c='i'; break;
		case LOGLEV_PANIC: c='p'; break;
		case LOGLEV_ERROR: c='e'; break;
		case LOGLEV_DEBUG: c='d'; break;
		default: break;
	}
	//fprintf(logfd, "-%c",c);

	//if(prefix != NULL)
	//	fprintf(logfd, "%s ", prefix);

	fprintf(logfd, logprefix, bx_pc_system.time_ticks(), c, BX_CPU(0)->eip, prefix==NULL?"":prefix);
        fprintf(logfd," ");

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

iofunctions::iofunctions(const char *fn)
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
	prefix = NULL;
	put(" ");
	settype(GENLOG);
	if (io == NULL && Allocio == 0) {
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
	prefix = NULL;
	put(" ");
	settype(GENLOG);
	setio(iofunc);
	// BUG: unfortunately this can be called before the bochsrc is read,
	// which means that the bochsrc has no effect on the actions.
	for (int i=0; i<N_LOGLEV; i++)
	  onoff[i] = bx_options.log.actions[i];
}

logfunctions::~logfunctions(void)
{
    if ( this->prefix )
    {
        free(this->prefix);
        this->prefix = NULL;
    }
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
logfunctions::put(char *p)
{
	char *tmpbuf;
	tmpbuf=strdup("[     ]");// if we ever have more than 32 chars,
						   //  we need to rethink this

	if ( tmpbuf == NULL)
	{
	    return ;                        /* allocation not successful */
	}
	if ( this->prefix != NULL )
	{
	    free(this->prefix);             /* free previously allocated memory */
	    this->prefix = NULL;
	}
	int len=strlen(p);
	for(int i=1;i<len+1;i++) {
		tmpbuf[i]=p[i-1];
	}
		
	switch(len) {
	case  1: tmpbuf[2]=' ';
	case  2: tmpbuf[3]=' ';
	case  3: tmpbuf[4]=' ';
	case  4: tmpbuf[5]=' ';
	default: tmpbuf[6]=']'; tmpbuf[7]='\0'; break;
	}
	
	this->prefix=tmpbuf;
}

void
logfunctions::settype(int t)
{
	type=t;
}

void
logfunctions::info(const char *fmt, ...)
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
logfunctions::error(const char *fmt, ...)
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
logfunctions::panic(const char *fmt, ...)
{
	va_list ap;

	assert (this != NULL);
	assert (this->logio != NULL);

	// Special case for panics since they are so important.  Always print
	// the panic to the log, no matter what the log action says.
	//if(!onoff[LOGLEV_PANIC]) return;

	va_start(ap, fmt);
	this->logio->out(this->type,LOGLEV_PANIC,this->prefix, fmt, ap);
	if (onoff[LOGLEV_PANIC] == ACT_ASK) 
	  ask (LOGLEV_PANIC, this->prefix, fmt, ap);
	if (onoff[LOGLEV_PANIC] == ACT_FATAL) 
	  fatal (this->prefix, fmt, ap);
	va_end(ap);
}

void
logfunctions::ldebug(const char *fmt, ...)
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
logfunctions::ask (int level, const char *prefix, const char *fmt, va_list ap)
{
  static char in_ask_already = 0;
  char buf1[1024], buf2[1024];
  if (in_ask_already) {
    fprintf (stderr, "logfunctions::ask() should not reenter!!\n");
    return;
  }
  in_ask_already = 1;
  vsprintf (buf1, fmt, ap);
  sprintf (buf2, "%s %s", prefix, buf1);
  // FIXME: facility set to 0 because it's unknown.

  // update vga screen.  This is useful because sometimes useful messages
  // are printed on the screen just before a panic.  It's also potentially
  // dangerous if this function calls ask again...  That's why I added
  // the reentry check above.
  if (SIM->get_init_done()) bx_vga.timer_handler(&bx_vga);

  int val = SIM->log_msg (prefix, level, buf2);
  switch (val)
  {
    case 0:   // user chose continue
      break;
    case 1:   // user said continue, and don't ask for this facility again.
      setonoff (level, ACT_REPORT);
      break;
    case 2:   // user chose die
      fatal (prefix, fmt, ap);
      break;
    case 3: // user chose abort
      fprintf (stderr, "User chose to dump core...\n");
#if BX_HAVE_ABORT
      abort ();
#else
      // do something highly illegal that should kill the process.
      // Hey, this is fun!
      {
      char *crashptr = (char *)0; char c = *crashptr;
      }
      fprintf (stderr, "Sorry, I couldn't find your abort() function.  Exiting.");
      exit (0);
#endif
#if BX_DEBUGGER
    case 4:
      // user chose debugger.  To "drop into the debugger" we just set the
      // interrupt_requested bit and continue execution.  Before the next
      // instruction, it should notice the user interrupt and return to
      // the debugger.
      bx_guard.interrupt_requested = 1;
      break;
#endif
    default:
      // this happens if panics happen before the callback is initialized
      // in gui/control.cc.
      fprintf (stderr, "WARNING: log_msg returned unexpected value %d\n", val);
  }
  in_ask_already = 0;
}

void
logfunctions::fatal (const char *prefix, const char *fmt, va_list ap)
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
  BX_EXIT(1);
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


