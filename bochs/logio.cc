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

#include "bochs.h"
#include "cpu/cpu.h"
#include <assert.h>

#if BX_WITH_CARBON
#include <Carbon/Carbon.h>
#endif

// Just for the iofunctions

static int Allocio=0;

const char* iofunctions::getlevel(int i) const
{
  static const char *loglevel[N_LOGLEV] = {
    "DEBUG",
    "INFO",
    "ERROR",
    "PANIC"
  };

  if (i>=0 && i<N_LOGLEV) return loglevel[i];
  else return "?";
}

const char* iofunctions::getaction(int i) const
{
  static const char *name[] = { "ignore", "report", "ask", "fatal" };
  assert (i>=ACT_IGNORE && i<N_ACT);
  return name[i];
}

void iofunctions::flush(void)
{
  if(logfd && magic == MAGIC_LOGNUM) {
    fflush(logfd);
  }
}

void iofunctions::init(void)
{
  // iofunctions methods must not be called before this magic
  // number is set.
  magic=MAGIC_LOGNUM;

  // sets the default logprefix
  strcpy(logprefix,"%t%e%d");
  n_logfn = 0;
  init_log(stderr);
  log = new logfunc_t(this);
  log->put("logio", "IO");
  log->ldebug("Init(log file: '%s').",logfn);
}

void iofunctions::add_logfn(logfunc_t *fn)
{
  assert(n_logfn < MAX_LOGFNS);
  logfn_list[n_logfn++] = fn;
}

void iofunctions::remove_logfn(logfunc_t *fn)
{
  assert(n_logfn > 0);
  int i = 0;
  while ((fn != logfn_list[i]) && (i < n_logfn)) {
    i++;
  };
  if (i < n_logfn) {
    for (int j=i; j<n_logfn-1; j++) {
      logfn_list[j] = logfn_list[j+1];
    }
    n_logfn--;
  }
}

void iofunctions::set_log_action(int loglevel, int action)
{
  for(int i=0; i<n_logfn; i++)
    logfn_list[i]->setonoff(loglevel, action);
}

void iofunctions::init_log(const char *fn)
{
  assert(magic==MAGIC_LOGNUM);
  // use newfd/newfn so that we can log the message to the OLD log
  // file descriptor.
  FILE *newfd = stderr;
  const char *newfn = "/dev/stderr";
  if(strcmp(fn, "-") != 0) {
    newfd = fopen(fn, "w");
    if(newfd != NULL) {
      newfn = strdup(fn);
      log->ldebug("Opened log file '%s'.", fn);
    } else {
      // in constructor, genlog might not exist yet, so do it the safe way.
      log->error("Couldn't open log file: %s, using stderr instead", fn);
      newfd = stderr;
    }
  }
  logfd = newfd;
  logfn = newfn;
}

void iofunctions::init_log(FILE *fs)
{
  assert(magic==MAGIC_LOGNUM);
  logfd = fs;

  if(fs == stderr) {
    logfn = "/dev/stderr";
  } else if(fs == stdout) {
    logfn = "/dev/stdout";
  } else {
    logfn = "(unknown)";
  }
}

void iofunctions::init_log(int fd)
{
  assert(magic==MAGIC_LOGNUM);
  FILE *tmpfd;
  if((tmpfd = fdopen(fd,"w")) == NULL) {
    log->panic("Couldn't open fd %d as a stream for writing", fd);
    return;
  }

  init_log(tmpfd);
};

void iofunctions::exit_log()
{
  flush();
  if (logfd != stderr) {
    fclose(logfd);
    logfd = stderr;
    free((char *)logfn);
    logfn = "/dev/stderr";
  }
}

// all other functions may use genlog safely.
#define LOG_THIS genlog->

// This converts the option string to a printf style string with the following args:
// 1. timer, 2. event, 3. cpu0 eip, 4. device
void iofunctions::set_log_prefix(const char* prefix)
{
  strcpy(logprefix, prefix);
}

//  iofunctions::out(level, prefix, fmt, ap)
//  DO NOT nest out() from ::info() and the like.
//    fmt and ap retained for direct printinf from iofunctions only!

void iofunctions::out(int level, const char *prefix, const char *fmt, va_list ap)
{
  char c = ' ', *s;
  char tmpstr[80], msgpfx[80], msg[1024];

  assert(magic==MAGIC_LOGNUM);
  assert(this != NULL);
  assert(logfd != NULL);

  switch (level) {
    case LOGLEV_INFO: c='i'; break;
    case LOGLEV_PANIC: c='p'; break;
    case LOGLEV_ERROR: c='e'; break;
    case LOGLEV_DEBUG: c='d'; break;
    default: break;
  }

  s = logprefix;
  msgpfx[0] = 0;
  while (*s) {
    switch (*s) {
      case '%':
        if(*(s+1)) s++;
        else break;
        switch(*s) {
          case 'd':
            sprintf(tmpstr, "%s", prefix==NULL?"":prefix);
            break;
          case 't':
            sprintf(tmpstr, FMT_TICK, bx_pc_system.time_ticks());
            break;
          case 'i':
#if BX_SUPPORT_SMP == 0
            sprintf(tmpstr, "%08x", BX_CPU(0)->get_eip());
#endif
            break;
          case 'e':
            sprintf(tmpstr, "%c", c);
            break;
          case '%':
            sprintf(tmpstr,"%%");
            break;
          default:
            sprintf(tmpstr,"%%%c",*s);
        }
        break;
      default:
        sprintf(tmpstr,"%c",*s);
    }
    strcat(msgpfx, tmpstr);
    s++;
  }

  fprintf(logfd,"%s ", msgpfx);

  if(level==LOGLEV_PANIC)
    fprintf(logfd, ">>PANIC<< ");

  vsnprintf(msg, sizeof(msg), fmt, ap);
  fprintf(logfd, "%s\n", msg);
  fflush(logfd);
  if (SIM->has_log_viewer()) {
    SIM->log_msg(msgpfx, level, msg);
  }
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

iofunctions::iofunctions()
{
  init();
}

iofunctions::~iofunctions(void)
{
  // flush before erasing magic number, or flush does nothing.
  flush();
  magic=0;
}

#define LOG_THIS genlog->

int logfunctions::default_onoff[N_LOGLEV] =
{
  ACT_IGNORE,  // ignore debug
  ACT_REPORT,  // report info
  ACT_REPORT,  // report error
#if BX_WITH_SDL2 || BX_WITH_WX || BX_WITH_WIN32 || BX_WITH_X11
  ACT_ASK      // on panic, ask user what to do
#else
  ACT_FATAL    // on panic, quit
#endif
};

logfunctions::logfunctions(void)
{
  name = NULL;
  prefix = NULL;
  put("?", " ");
  if (io == NULL && Allocio == 0) {
    Allocio = 1;
    io = new iofunc_t(stderr);
  }
  setio(io);
  // BUG: unfortunately this can be called before the bochsrc is read,
  // which means that the bochsrc has no effect on the actions.
  for (int i=0; i<N_LOGLEV; i++)
    onoff[i] = get_default_action(i);
}

logfunctions::logfunctions(iofunc_t *iofunc)
{
  name = NULL;
  prefix = NULL;
  put("?", " ");
  setio(iofunc);
  // BUG: unfortunately this can be called before the bochsrc is read,
  // which means that the bochsrc has no effect on the actions.
  for (int i=0; i<N_LOGLEV; i++)
    onoff[i] = get_default_action(i);
}

logfunctions::~logfunctions()
{
  logio->remove_logfn(this);
  if (name) free(name);
  if (prefix) free(prefix);
}

void logfunctions::setio(iofunc_t *i)
{
  // add pointer to iofunction object to use
  logio = i;
  // give iofunction a pointer to me
  i->add_logfn(this);
}

void logfunctions::put(const char *p)
{
  char *n = strdup(p);

  for (unsigned i=0; i<strlen(p); i++)
    n[i] = tolower(p[i]);

  put((const char*)n, p);
  free(n);
}

void logfunctions::put(const char *n, const char *p)
{
  char *tmpbuf=strdup(BX_NULL_PREFIX); // if we ever have more than 32 chars,
                                       // we need to rethink this
  if (tmpbuf == NULL)
    return;                         // allocation not successful

  if (name != NULL) {
    free(name);             // free previously allocated memory
    name = NULL;
  }
  name = strdup(n);

  if (prefix != NULL) {
    free(prefix);             // free previously allocated memory
    prefix = NULL;
  }

  size_t len=strlen(p);
  if (len > (strlen(tmpbuf) - 2)) {
    len = strlen(tmpbuf) - 2;
  }
  for(size_t i=1;i <= len;i++) {
    tmpbuf[i]=p[i-1];
  }

  prefix = tmpbuf;
}

void logfunctions::info(const char *fmt, ...)
{
  va_list ap;

  assert(logio != NULL);

  if (!onoff[LOGLEV_INFO]) return;

  va_start(ap, fmt);
  logio->out(LOGLEV_INFO, prefix, fmt, ap);
  va_end(ap);

  if (onoff[LOGLEV_INFO] == ACT_ASK) {
    va_start(ap, fmt);
    ask(LOGLEV_INFO, prefix, fmt, ap);
    va_end(ap);
  }
  if (onoff[LOGLEV_INFO] == ACT_FATAL) {
    va_start(ap, fmt);
    fatal(prefix, fmt, ap, 1);
  }
}

void logfunctions::error(const char *fmt, ...)
{
  va_list ap;

  assert(logio != NULL);

  if(!onoff[LOGLEV_ERROR]) return;

  va_start(ap, fmt);
  logio->out(LOGLEV_ERROR, prefix, fmt, ap);
  va_end(ap);

  if (onoff[LOGLEV_ERROR] == ACT_ASK) {
    va_start(ap, fmt);
    ask(LOGLEV_ERROR, prefix, fmt, ap);
    va_end(ap);
  }
  if (onoff[LOGLEV_ERROR] == ACT_FATAL) {
    va_start(ap, fmt);
    fatal(prefix, fmt, ap, 1);
  }
}

void logfunctions::panic(const char *fmt, ...)
{
  va_list ap;

  assert(logio != NULL);

  // Special case for panics since they are so important. Always print
  // the panic to the log, no matter what the log action says.

  va_start(ap, fmt);
  logio->out(LOGLEV_PANIC, prefix, fmt, ap);
  va_end(ap);

  if (onoff[LOGLEV_PANIC] == ACT_ASK) {
    va_start(ap, fmt);
    ask(LOGLEV_PANIC, prefix, fmt, ap);
    va_end(ap);
  }
  if (onoff[LOGLEV_PANIC] == ACT_FATAL) {
    va_start(ap, fmt);
    fatal(prefix, fmt, ap, 1);
  }
}

void logfunctions::ldebug(const char *fmt, ...)
{
  va_list ap;

  assert(logio != NULL);

  if(!onoff[LOGLEV_DEBUG]) return;

  va_start(ap, fmt);
  logio->out(LOGLEV_DEBUG, prefix, fmt, ap);
  va_end(ap);

  // the actions ask() and fatal() are not supported here
}

void logfunctions::ask(int level, const char *prefix, const char *fmt, va_list ap)
{
  // Guard against reentry on ask() function.  The danger is that some
  // function that's called within ask() could trigger another
  // BX_PANIC that could call ask() again, leading to infinite
  // recursion and infinite asks.
  static char in_ask_already = 0;
  char buf1[1024];
  if (in_ask_already) {
    fprintf(stderr, "logfunctions::ask() should not reenter!!\n");
    return;
  }
  in_ask_already = 1;
  vsnprintf(buf1, sizeof(buf1), fmt, ap);
  // FIXME: facility set to 0 because it's unknown.

  // update vga screen.  This is useful because sometimes useful messages
  // are printed on the screen just before a panic.  It's also potentially
  // dangerous if this function calls ask again...  That's why I added
  // the reentry check above.
  SIM->refresh_vga();

  // ensure the text screen is showing
  SIM->set_display_mode(DISP_MODE_CONFIG);
  int val = SIM->log_ask(prefix, level, buf1);
  switch(val)
  {
    case BX_LOG_ASK_CHOICE_CONTINUE:
      break;
    case BX_LOG_ASK_CHOICE_CONTINUE_ALWAYS:
      // user said continue, and don't "ask" for this facility again.
      setonoff(level, ACT_REPORT);
      break;
    case BX_LOG_ASK_CHOICE_DIE:
    case BX_LOG_NOTIFY_FAILED:
      bx_user_quit = (val==BX_LOG_ASK_CHOICE_DIE)?1:0;
      // fatal() quits the simulation in the calling method
      setonoff(level, ACT_FATAL);
      in_ask_already = 0;
      return;
    case BX_LOG_ASK_CHOICE_DUMP_CORE:
      fprintf(stderr, "User chose to dump core...\n");
#if BX_HAVE_ABORT
      abort();
#else
      // do something highly illegal that should kill the process.
      // Hey, this is fun!
      {
        char *crashptr = (char *)0; char c = *crashptr;
      }
      fprintf(stderr, "Sorry, I couldn't find your abort() function.  Exiting.");
      exit(0);
#endif
#if BX_DEBUGGER
    case BX_LOG_ASK_CHOICE_ENTER_DEBUG:
      // user chose debugger.  To "drop into the debugger" we just set the
      // interrupt_requested bit and continue execution.  Before the next
      // instruction, it should notice the user interrupt and return to
      // the debugger.
      bx_debug_break();
      break;
#elif BX_GDBSTUB
    case BX_LOG_ASK_CHOICE_ENTER_DEBUG:
      bx_gdbstub_break();
      break;
#endif
    default:
      // this happens if panics happen before the callback is initialized
      // in gui/control.cc.
      fprintf(stderr, "WARNING: log_msg returned unexpected value %d\n", val);
  }
  // return to simulation mode
  SIM->set_display_mode(DISP_MODE_SIM);
  in_ask_already = 0;
}

#if BX_WITH_CARBON
/* Panic button to display fatal errors.
  Completely self contained, can't rely on carbon.cc being available */
static void carbonFatalDialog(const char *error, const char *exposition)
{
  DialogRef                     alertDialog;
  CFStringRef                   cfError;
  CFStringRef                   cfExposition;
  DialogItemIndex               index;
  AlertStdCFStringAlertParamRec alertParam = {0};

  // Init libraries
  InitCursor();
  // Assemble dialog
  cfError = CFStringCreateWithCString(NULL, error, kCFStringEncodingASCII);
  if(exposition != NULL)
  {
    cfExposition = CFStringCreateWithCString(NULL, exposition, kCFStringEncodingASCII);
  }
  else {
    cfExposition = NULL;
  }
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

void logfunctions::fatal(const char *prefix, const char *fmt, va_list ap, int exit_status)
{
  char tmpbuf[1024];
  char exit_msg[1024];

  if (!SIM->is_wx_selected()) {
    // store prefix and message in 'exit_msg' before unloading device plugins
    vsnprintf(tmpbuf, sizeof(tmpbuf), fmt, ap);
    va_end(ap);
    sprintf(exit_msg, "%s %s", prefix, tmpbuf);
  }
#if !BX_DEBUGGER
  bx_atexit();
#endif
#if BX_WITH_CARBON
  if(!isatty(STDIN_FILENO) && !SIM->get_init_done())
  {
    char buf1[1024];
    char buf2[1024];
    vsnprintf(buf1, sizeof(buf1), fmt, ap);
    snprintf(buf2, sizeof(buf2), "Bochs startup error\n%s", buf1);
    carbonFatalDialog(buf2,
      "For more information, try running Bochs within Terminal by clicking on \"bochs.scpt\".");
  }
#endif
  if (!SIM->is_wx_selected()) {
    static const char *divider = "========================================================================";
    fprintf(stderr, "%s\n", divider);
    fprintf(stderr, "Bochs is exiting with the following message:\n");
    fprintf(stderr, "%s", exit_msg);
    fprintf(stderr, "\n%s\n", divider);
  }
#if !BX_DEBUGGER
  BX_EXIT(exit_status);
#else
  bx_dbg_exit(exit_status);
#endif
  // not safe to use BX_* log functions in here.
  fprintf(stderr, "fatal() should never return, but it just did\n");
}

iofunc_t *io = NULL;
logfunc_t *genlog = NULL;

void bx_center_print(FILE *file, const char *line, unsigned maxwidth)
{
  size_t len = strlen(line);
  if (len > maxwidth)
    BX_PANIC(("bx_center_print: line is too long: '%s'", line));
  size_t imax = (maxwidth - len) >> 1;
  for (size_t i=0; i<imax; i++) fputc(' ', file);
  fputs(line, file);
}
