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
	showtick = 1;
	n_logfn = 0;
	init_log(stderr);
	log = new logfunc_t(this);
	LOG_THIS setprefix("IO");
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
	setprefix(" ");
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
	setprefix(" ");
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
	char *tmpbuf;
	tmpbuf=strdup("[     ]");// if we ever have more than 32 chars,
						   //  we need to rethink this
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


