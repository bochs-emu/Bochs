/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2025  The Bochs Project
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

// See siminterface.h for description of the siminterface concept.
// Basically, the siminterface is visible from both the simulator and
// the configuration user interface, and allows them to talk to each other.

#include "param_names.h"
#include "iodev.h"
#include "bx_debug/debug.h"
#include "virt_timer.h"
#if BX_USB_DEBUGGER
#include "gui/usb_debug.h"
#endif

bx_simulator_interface_c *SIM = NULL;
logfunctions *siminterface_log = NULL;
bx_list_c *root_param = NULL;
#define LOG_THIS siminterface_log->

// bx_simulator_interface just defines the interface that the Bochs simulator
// and the gui will use to talk to each other.  None of the methods of
// bx_simulator_interface are implemented; they are all virtual.  The
// bx_real_sim_c class is a child of bx_simulator_interface_c, and it
// implements all the methods.  The idea is that a gui needs to know only
// definition of bx_simulator_interface to talk to Bochs.  The gui should
// not need to include bochs.h.
//
// I made this separation to ensure that all guis use the siminterface to do
// access bochs internals, instead of accessing things like
// bx_keyboard.s.internal_buffer[4] (or whatever) directly. -Bryce
//

static int rt_conf_id = 0;

typedef struct _rt_conf_entry_t {
  int id;
  void *device;
  rt_conf_handler_t handler;
  struct _rt_conf_entry_t *next;
} rt_conf_entry_t;

typedef struct _addon_option_t {
  const char *name;
  addon_option_parser_t parser;
  addon_option_save_t savefn;
  struct _addon_option_t *next;
} addon_option_t;

class bx_real_sim_c : public bx_simulator_interface_c {
  bxevent_handler bxevent_callback;
  void *bxevent_callback_data;
  const char *registered_ci_name;
  config_interface_callback_t ci_callback;
  void *ci_callback_data;
  rt_conf_entry_t *rt_conf_entries;
  addon_option_t *addon_options;
  bool init_done;
  bool ci_started;
  bool enabled;
  // save context to jump to if we must quit unexpectedly
  jmp_buf *quit_context;
  int exit_code;
  unsigned param_id;
  bool bx_debug_gui;
  bool bx_log_viewer;
  bool wxsel;
public:
  bx_real_sim_c();
  virtual ~bx_real_sim_c() {}
  virtual void set_quit_context(jmp_buf *context) { quit_context = context; }
  virtual bool get_init_done() { return init_done; }
  virtual int set_init_done(bool n);
  virtual bool get_ci_started() { return ci_started; }
  virtual void reset_all_param();
  // new param methods
  virtual bx_param_c *get_param(const char *pname, bx_param_c *base=NULL);
  virtual bx_param_num_c *get_param_num(const char *pname, bx_param_c *base=NULL);
  virtual bx_param_string_c *get_param_string(const char *pname, bx_param_c *base=NULL);
  virtual bx_param_bool_c *get_param_bool(const char *pname, bx_param_c *base=NULL);
  virtual bx_param_enum_c *get_param_enum(const char *pname, bx_param_c *base=NULL);
  virtual Bit32u gen_param_id() { return param_id++; }
  virtual int get_n_log_modules();
  virtual const char *get_logfn_name(int mod);
  virtual int get_logfn_id(const char *name);
  virtual const char *get_prefix(int mod);
  virtual int get_log_action(int mod, int level);
  virtual void set_log_action(int mod, int level, int action);
  virtual const char *get_action_name(int action);
  virtual int is_action_name(const char *val);
  virtual int get_default_log_action(int level) {
    return logfunctions::get_default_action(level);
  }
  virtual void set_default_log_action(int level, int action) {
    logfunctions::set_default_action(level, action);
  }
  virtual const char *get_log_level_name(int level);
  virtual int get_max_log_level() { return N_LOGLEV; }
  virtual void quit_sim(int code);
  virtual int get_exit_code() { return exit_code; }
  virtual int get_default_rc(char *path, int len);
  virtual int read_rc(const char *path);
  virtual int write_rc(const char *path, int overwrite);
  virtual int get_log_file(char *path, int len);
  virtual int set_log_file(const char *path);
  virtual int get_log_prefix(char *prefix, int len);
  virtual int set_log_prefix(const char *prefix);
  virtual int get_debugger_log_file(char *path, int len);
  virtual int set_debugger_log_file(const char *path);
  virtual void set_notify_callback(bxevent_handler func, void *arg);
  virtual void get_notify_callback(bxevent_handler *func, void **arg);
  virtual BxEvent* sim_to_ci_event(BxEvent *event);
  virtual int log_dlg(const char *prefix, int level, const char *msg, int mode);
  virtual void log_msg(const char *prefix, int level, const char *msg);
  virtual void set_log_viewer(bool val) { bx_log_viewer = val; }
  virtual bool has_log_viewer() const { return bx_log_viewer; }
  virtual int ask_param(bx_param_c *param);
  virtual int ask_param(const char *pname);
  // ask the user for a pathname
  virtual int ask_filename(const char *filename, int maxlen, const char *prompt, const char *the_default, int flags);
  // yes/no dialog
  virtual int ask_yes_no(const char *title, const char *prompt, bool the_default);
  // simple message box
  virtual void message_box(const char *title, const char *message);
  // simple modeless message box
  virtual void *ml_message_box(const char *title, const char *message);
  // kill modeless message box
  virtual void ml_message_box_kill(void *ptr);
  // called at a regular interval, currently by the keyboard handler.
  virtual void periodic();
  virtual int create_disk_image(const char *filename, int sectors, bool overwrite);
  virtual void refresh_ci();
  virtual void refresh_vga() {
    if (init_done) {
      DEV_vga_refresh(0);
    }
  }
  virtual void handle_events() {
    // maybe need to check if something has been initialized yet?
    bx_gui->handle_events();
  }
  // find first hard drive or cdrom
  bx_param_c *get_first_atadevice(Bit32u search_type);
  bx_param_c *get_first_cdrom() {
    return get_first_atadevice(BX_ATA_DEVICE_CDROM);
  }
  bx_param_c *get_first_hd() {
    return get_first_atadevice(BX_ATA_DEVICE_DISK);
  }
  virtual bool is_pci_device(const char *name);
  virtual bool is_agp_device(const char *name);
#if BX_DEBUGGER
  virtual bool debugger_active() {return bx_dbg.debugger_active;}
  virtual void debug_break();
  virtual void debug_interpret_cmd(char *cmd);
  virtual char *debug_get_next_command();
  virtual void debug_puts(const char *cmd);
#endif
  virtual void register_configuration_interface (
    const char* name,
    config_interface_callback_t callback,
    void *userdata);
  virtual int configuration_interface(const char* name, ci_command_t command);
#if BX_USB_DEBUGGER
  virtual void register_usb_debug_type(int type);
  virtual void usb_debug_trigger(int type, int trigger, Bit64u param0, int param1, int param2);
  virtual int usb_debug_interface(int type, Bit64u param0, int param1, int param2);
#endif
  virtual int begin_simulation(int argc, char *argv[]);
  virtual int register_runtime_config_handler(void *dev, rt_conf_handler_t handler);
  virtual void unregister_runtime_config_handler(int id);
  virtual void update_runtime_options();
  virtual void set_sim_thread_func(is_sim_thread_func_t func) {}
  virtual bool is_sim_thread();
  virtual void set_debug_gui(bool val) { bx_debug_gui = val; }
  virtual bool has_debug_gui() const { return bx_debug_gui; }
  virtual bool is_wx_selected() const { return wxsel; }
  // provide interface to bx_gui->set_display_mode() method for config
  // interfaces to use.
  virtual void set_display_mode(disp_mode_t newmode) {
    if (bx_gui != NULL)
      bx_gui->set_display_mode(newmode);
  }
  virtual bool test_for_text_console();
  // add-on config option support
  virtual bool register_addon_option(const char *keyword, addon_option_parser_t parser, addon_option_save_t save_func);
  virtual bool unregister_addon_option(const char *keyword);
  virtual bool is_addon_option(const char *keyword);
  virtual Bit32s  parse_addon_option(const char *context, int num_params, char *params []);
  virtual Bit32s  save_addon_options(FILE *fp);

  // statistics
  virtual void init_statistics();
  virtual void cleanup_statistics();
  virtual bx_list_c *get_statistics_root() {
    return (bx_list_c*)get_param("statistics", NULL);
  }

  // save/restore support
  virtual void init_save_restore();
  virtual void cleanup_save_restore();
  virtual bool save_state(const char *checkpoint_path);
  virtual bool restore_config();
  virtual bool restore_logopts();
  virtual bool restore_hardware();
  virtual bx_list_c *get_bochs_root() {
    return (bx_list_c*)get_param("bochs", NULL);
  }
  virtual bool restore_bochs_param(bx_list_c *root, const char *sr_path, const char *restore_name);
  // special config parameter and options functions for plugins
  virtual bool opt_plugin_ctrl(const char *plugname, bool load);
#if BX_NETWORKING
  virtual void init_std_nic_options(const char *name, bx_list_c *menu);
#endif
#if BX_SUPPORT_PCIUSB
  virtual void init_usb_options(const char *usb_name, const char *pname, int maxports, int param0);
#endif
  virtual int  parse_param_from_list(const char *context, const char *param, bx_list_c *base);
  virtual int  parse_nic_params(const char *context, const char *param, bx_list_c *base);
  virtual int  parse_usb_port_params(const char *context, const char *param,
                                     int maxports, bx_list_c *base);
  virtual int  split_option_list(const char *msg, const char *rawopt, char **argv, int max_argv);
  virtual int  write_param_list(FILE *fp, bx_list_c *base, const char *optname, bool multiline);
#if BX_SUPPORT_PCIUSB
  virtual int  write_usb_options(FILE *fp, int maxports, bx_list_c *base);
#endif
#if BX_USE_GUI_CONSOLE
  virtual int  bx_printf(const char *fmt, ...);
  virtual char* bx_gets(char *s, int size, FILE *stream);
#endif

private:
  bool save_sr_param(FILE *fp, bx_param_c *node, const char *sr_path, int level);
};

// recursive function to find parameters from the path
static bx_param_c *find_param(const char *full_pname, const char *rest_of_pname, bx_param_c *base)
{
  const char *from = rest_of_pname;
  char component[BX_PATHNAME_LEN];
  char *to = component;
  // copy the first piece of pname into component, stopping at first separator
  // or at the end of the string
  while (*from != 0 && *from != '.') {
    *to = *from;
    to++;
    from++;
  }
  *to = 0;
  if (!component[0]) {
    BX_PANIC(("find_param: found empty component in parameter name '%s'", full_pname));
    // or does that mean that we're done?
  }
  if (base->get_type() != BXT_LIST) {
    BX_PANIC(("find_param: base was not a list!"));
  }
  BX_DEBUG(("searching for component '%s' in list '%s'", component, base->get_name()));

  // find the component in the list.
  bx_list_c *list = (bx_list_c *)base;
  bx_param_c *child = list->get_by_name(component);
  // if child not found, there is nothing else that can be done. return NULL.
  if (child == NULL) return NULL;
  if (from[0] == 0) {
    // that was the end of the path, we're done
    return child;
  }
  // continue parsing the path
  BX_ASSERT(from[0] == '.');
  from++;  // skip over the separator
  return find_param(full_pname, from, child);
}

bx_param_c *bx_real_sim_c::get_param(const char *pname, bx_param_c *base)
{
  if (base == NULL)
    base = root_param;
  // to access top level object, look for parameter "."
  if (pname[0] == '.' && pname[1] == 0)
    return base;
  return find_param(pname, pname, base);
}

bx_param_num_c *bx_real_sim_c::get_param_num(const char *pname, bx_param_c *base)
{
  bx_param_c *gen = get_param(pname, base);
  if (gen==NULL) {
    BX_ERROR(("get_param_num(%s) could not find a parameter", pname));
    return NULL;
  }
  int type = gen->get_type();
  if (type == BXT_PARAM_NUM || type == BXT_PARAM_BOOL || type == BXT_PARAM_ENUM)
    return (bx_param_num_c *)gen;
  BX_ERROR(("get_param_num(%s) could not find a number parameter with that name", pname));
  return NULL;
}

bx_param_string_c *bx_real_sim_c::get_param_string(const char *pname, bx_param_c *base)
{
  bx_param_c *gen = get_param(pname, base);
  if (gen==NULL) {
    BX_ERROR(("get_param_string(%s) could not find a parameter", pname));
    return NULL;
  }
  if (gen->get_type() == BXT_PARAM_STRING || gen->get_type() == BXT_PARAM_BYTESTRING)
    return (bx_param_string_c *)gen;
  BX_ERROR(("get_param_string(%s) could not find a string parameter with that name", pname));
  return NULL;
}

bx_param_bool_c *bx_real_sim_c::get_param_bool(const char *pname, bx_param_c *base)
{
  bx_param_c *gen = get_param(pname, base);
  if (gen==NULL) {
    BX_ERROR(("get_param_bool(%s) could not find a parameter", pname));
    return NULL;
  }
  if (gen->get_type () == BXT_PARAM_BOOL)
    return (bx_param_bool_c *)gen;
  BX_ERROR(("get_param_bool(%s) could not find a bool parameter with that name", pname));
  return NULL;
}

bx_param_enum_c *bx_real_sim_c::get_param_enum(const char *pname, bx_param_c *base)
{
  bx_param_c *gen = get_param(pname, base);
  if (gen==NULL) {
    BX_ERROR(("get_param_enum(%s) could not find a parameter", pname));
    return NULL;
  }
  if (gen->get_type() == BXT_PARAM_ENUM)
    return (bx_param_enum_c *)gen;
  BX_ERROR(("get_param_enum(%s) could not find a enum parameter with that name", pname));
  return NULL;
}

void bx_init_siminterface()
{
  if (SIM == NULL) {
    SIM = new bx_real_sim_c();
  }
  if (siminterface_log == NULL) {
    siminterface_log = new logfunctions();
    siminterface_log->put("siminterface", "SIM");
  }
  if (root_param == NULL) {
    root_param = new bx_list_c(NULL,
      "bochs",
      "list of top level bochs parameters");
  }
}

int bx_cleanup_siminterface()
{
  if (siminterface_log) {
    delete siminterface_log;
  }
  if (root_param) {
    root_param->clear();
    delete root_param;
    root_param = NULL;
  }
  io->exit_log2();
  int exit_code = SIM->get_exit_code();
  delete SIM;
  return exit_code;
}

bx_real_sim_c::bx_real_sim_c()
{
  bxevent_callback = NULL;
  bxevent_callback_data = NULL;
  ci_callback = NULL;
  ci_callback_data = NULL;
  is_sim_thread_func = NULL;
  bx_debug_gui = 0;
  bx_log_viewer = 0;
  wxsel = 0;

  enabled = 1;
  init_done = 0;
  ci_started = 0;
  quit_context = NULL;
  exit_code = 0;
  param_id = BXP_NEW_PARAM_ID;
  rt_conf_entries = NULL;
  addon_options = NULL;
}

int bx_real_sim_c::set_init_done(bool n)
{
#if BX_USE_TEXTCONFIG
  if (n) {
    if (bx_gui->has_gui_console()) {
      if (strcmp(registered_ci_name, "textconfig") != 0) {
        PLUG_load_plugin(textconfig, PLUGTYPE_CORE);
      }
    }
  }
#endif
  init_done = n;
  return 0;
}

void bx_real_sim_c::reset_all_param()
{
  bx_reset_options();
}

int bx_real_sim_c::get_n_log_modules()
{
  return io->get_n_logfns();
}

const char *bx_real_sim_c::get_logfn_name(int mod)
{
  logfunc_t *logfn = io->get_logfn(mod);
  return logfn->get_name();
}

int bx_real_sim_c::get_logfn_id(const char *name)
{
  logfunc_t *logfn;
  int id = -1;

  for (int i = 0; i < io->get_n_logfns(); i++) {
    logfn = io->get_logfn(i);
    if (!stricmp(name, logfn->get_name())) {
      id = i;
      break;
    }
  }
  return id;
}

const char *bx_real_sim_c::get_prefix(int mod)
{
  logfunc_t *logfn = io->get_logfn(mod);
  return logfn->getprefix();
}

int bx_real_sim_c::get_log_action(int mod, int level)
{
  logfunc_t *logfn = io->get_logfn(mod);
  return logfn->getonoff(level);
}

void bx_real_sim_c::set_log_action(int mod, int level, int action)
{
  // normal
  if (mod >= 0) {
    logfunc_t *logfn = io->get_logfn(mod);
    logfn->setonoff(level, action);
    return;
  }
  // if called with mod<0 loop over all
  int nmod = get_n_log_modules();
  for (mod=0; mod<nmod; mod++)
    set_log_action(mod, level, action);
}

const char *bx_real_sim_c::get_action_name(int action)
{
  return io->getaction(action);
}

int bx_real_sim_c::is_action_name(const char *val)
{
  return io->isaction(val);
}

const char *bx_real_sim_c::get_log_level_name(int level)
{
  return io->getlevel(level);
}

void bx_real_sim_c::quit_sim(int code)
{
  BX_INFO(("quit_sim called with exit code %d", code));
  exit_code = code;
  io->exit_log();
  // use longjmp to quit cleanly, no matter where in the stack we are.
  if (quit_context != NULL) {
    longjmp(*quit_context, 1);
    BX_PANIC(("in bx_real_sim_c::quit_sim, longjmp should never return"));
  } else {
    // use exit() to stop the application.
    if (!code)
      BX_PANIC(("Quit simulation command"));
    ::exit(exit_code);
  }
}

int bx_real_sim_c::get_default_rc(char *path, int len)
{
  char *rc = bx_find_bochsrc();
  if (rc == NULL) return -1;
  strncpy(path, rc, len);
  path[len-1] = 0;
  return 0;
}

int bx_real_sim_c::read_rc(const char *rc)
{
  return bx_read_configuration(rc);
}

// return values:
//   0: written ok
//  -1: failed
//  -2: already exists, and overwrite was off
int bx_real_sim_c::write_rc(const char *rc, int overwrite)
{
  return bx_write_configuration(rc, overwrite);
}

int bx_real_sim_c::get_log_file(char *path, int len)
{
  strncpy(path, SIM->get_param_string(BXPN_LOG_FILENAME)->getptr(), len);
  return 0;
}

int bx_real_sim_c::set_log_file(const char *path)
{
  SIM->get_param_string(BXPN_LOG_FILENAME)->set(path);
  return 0;
}

int bx_real_sim_c::get_log_prefix(char *prefix, int len)
{
  strncpy(prefix, SIM->get_param_string(BXPN_LOG_PREFIX)->getptr(), len);
  return 0;
}

int bx_real_sim_c::set_log_prefix(const char *prefix)
{
  SIM->get_param_string(BXPN_LOG_PREFIX)->set(prefix);
  return 0;
}

int bx_real_sim_c::get_debugger_log_file(char *path, int len)
{
  strncpy(path, SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->getptr(), len);
  return 0;
}

int bx_real_sim_c::set_debugger_log_file(const char *path)
{
  SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->set(path);
  return 0;
}

const char *floppy_devtype_names[] = { "none", "5.25\" 360K", "5.25\" 1.2M", "3.5\" 720K", "3.5\" 1.44M", "3.5\" 2.88M", NULL };
const char *floppy_type_names[] = { "none", "1.2M", "1.44M", "2.88M", "720K", "360K", "160K", "180K", "320K", "auto", NULL };
int floppy_type_n_sectors[] = { -1, 80*2*15, 80*2*18, 80*2*36, 80*2*9, 40*2*9, 40*1*8, 40*1*9, 40*2*8, -1 };
const char *media_status_names[] = { "ejected", "inserted", NULL };

const char *bochs_bootdisk_names[] = { "none", "floppy", "disk", "cdrom", "", "usb", "network", NULL };

void bx_real_sim_c::set_notify_callback(bxevent_handler func, void *arg)
{
  bxevent_callback = func;
  bxevent_callback_data = arg;
}

void bx_real_sim_c::get_notify_callback(bxevent_handler *func, void **arg)
{
  *func = bxevent_callback;
  *arg = bxevent_callback_data;
}

BxEvent *bx_real_sim_c::sim_to_ci_event(BxEvent *event)
{
  if (bxevent_callback == NULL) {
    BX_ERROR(("notify called, but no bxevent_callback function is registered"));
    return NULL;
  } else {
    return (*bxevent_callback)(bxevent_callback_data, event);
  }
}

int bx_real_sim_c::log_dlg(const char *prefix, int level, const char *msg, int mode)
{
  BxEvent be;
  be.type = BX_SYNC_EVT_LOG_DLG;
  be.u.logmsg.prefix = prefix;
  be.u.logmsg.level = level;
  be.u.logmsg.msg = msg;
  be.u.logmsg.mode = mode;
  // default return value in case something goes wrong.
  be.retcode = BX_LOG_NOTIFY_FAILED;
  // calling notify
  sim_to_ci_event(&be);
  return be.retcode;
}

void bx_real_sim_c::log_msg(const char *prefix, int level, const char *msg)
{
  if (SIM->has_log_viewer()) {
    // send message to the log viewer
    char *logmsg = new char[strlen(prefix) + strlen(msg) + 4];
    sprintf(logmsg, "%s %s\n", prefix, msg);
    BxEvent *event = new BxEvent();
    event->type = BX_ASYNC_EVT_LOG_MSG;
    event->u.logmsg.prefix = NULL;
    event->u.logmsg.level = level;
    event->u.logmsg.msg = logmsg;
    sim_to_ci_event(event);
  }
}

// Called by simulator whenever it needs the user to choose a new value
// for a registered parameter.  Create a synchronous ASK_PARAM event,
// send it to the CI, and wait for the response.  The CI will call the
// set() method on the parameter if the user changes the value.
int bx_real_sim_c::ask_param(bx_param_c *param)
{
  BX_ASSERT(param != NULL);
  // create appropriate event
  BxEvent event;
  event.type = BX_SYNC_EVT_ASK_PARAM;
  event.u.param.param = param;
  sim_to_ci_event(&event);
  return event.retcode;
}

int bx_real_sim_c::ask_param(const char *pname)
{
  bx_param_c *paramptr = SIM->get_param(pname);
  BX_ASSERT(paramptr != NULL);
  // create appropriate event
  BxEvent event;
  event.type = BX_SYNC_EVT_ASK_PARAM;
  event.u.param.param = paramptr;
  sim_to_ci_event(&event);
  return event.retcode;
}

int bx_real_sim_c::ask_filename(const char *filename, int maxlen, const char *prompt, const char *the_default, int flags)
{
  BxEvent event;
  bx_param_filename_c param(NULL, "filename", prompt, "", the_default, maxlen);
  param.set_options(param.get_options() | flags);
  event.type = BX_SYNC_EVT_ASK_PARAM;
  event.u.param.param = &param;
  sim_to_ci_event(&event);
  if (event.retcode >= 0)
    memcpy((char *)filename, param.getptr(), maxlen);
  return event.retcode;
}

int bx_real_sim_c::ask_yes_no(const char *title, const char *prompt, bool the_default)
{
  BxEvent event;
  char format[512];

  bx_param_bool_c param(NULL, "yes_no", title, prompt, the_default);
  sprintf(format, "%s\n\n%s [%%s] ", title, prompt);
  param.set_ask_format(format);
  event.type = BX_SYNC_EVT_ASK_PARAM;
  event.u.param.param = &param;
  sim_to_ci_event(&event);
  if (event.retcode >= 0) {
    return param.get();
  } else {
    return event.retcode;
  }
}

void bx_real_sim_c::message_box(const char *title, const char *message)
{
  BxEvent event;

  event.type = BX_SYNC_EVT_MSG_BOX;
  event.u.logmsg.prefix = title;
  event.u.logmsg.msg = message;
  sim_to_ci_event(&event);
}

void *bx_real_sim_c::ml_message_box(const char *title, const char *message)
{
  BxEvent event;

  event.type = BX_SYNC_EVT_ML_MSG_BOX;
  event.param0 = NULL;
  event.u.logmsg.prefix = title;
  event.u.logmsg.msg = message;
  sim_to_ci_event(&event);
  return event.param0;
}

void bx_real_sim_c::ml_message_box_kill(void *ptr)
{
  BxEvent event;

  event.type = BX_SYNC_EVT_ML_MSG_BOX_KILL;
  event.param0 = ptr;
  sim_to_ci_event(&event);
}

void bx_real_sim_c::periodic()
{
  // give the GUI a chance to do periodic things on the bochs thread. in
  // particular, notice if the thread has been asked to die.
  BxEvent tick;
  tick.type = BX_SYNC_EVT_TICK;
  sim_to_ci_event (&tick);
  if (tick.retcode < 0) {
    BX_INFO(("Bochs thread has been asked to quit."));
    bx_exit(0);
  }
  static int refresh_counter = 0;
  if (++refresh_counter == 50) {
    // only ask the CI to refresh every 50 times periodic() is called.
    // This should obviously be configurable because system speeds and
    // user preferences vary.
    refresh_ci();
    refresh_counter = 0;
  }
}

// create a disk image file called filename, size=512 bytes * sectors.
// If overwrite is 0 and the file exists, returns -1 without changing it.
// Otherwise, opens up the image and starts writing.  Returns -2 if
// the image could not be opened, or -3 if there are failures during
// write, e.g. disk full.
//
// wxWidgets: This may be called from the gui thread.
int bx_real_sim_c::create_disk_image(const char *filename, int sectors, bool overwrite)
{
  FILE *fp;
  if (!overwrite) {
    // check for existence first
    fp = fopen(filename, "r");
    if (fp) {
      // yes it exists
      fclose(fp);
      return -1;
    }
  }
  fp = fopen(filename, "w");
  if (fp == NULL) {
#ifdef HAVE_PERROR
    char buffer[1024];
    sprintf(buffer, "while opening '%s' for writing", filename);
    perror(buffer);
    // not sure how to get this back into the CI
#endif
    return -2;
  }
  int sec = sectors;
  /*
   * seek to sec*512-1 and write a single character.
   * can't just do: fseek(fp, 512*sec-1, SEEK_SET)
   * because 512*sec may be too large for signed int.
   */
  while (sec > 0)
  {
    /* temp <-- min(sec, 4194303)
     * 4194303 is (int)(0x7FFFFFFF/512)
     */
    int temp = ((sec < 4194303) ? sec : 4194303);
    fseek(fp, 512*temp, SEEK_CUR);
    sec -= temp;
  }

  fseek(fp, -1, SEEK_CUR);
  if (fputc('\0', fp) == EOF)
  {
    fclose(fp);
    return -3;
  }
  fclose(fp);
  return 0;
}

void bx_real_sim_c::refresh_ci()
{
  if (SIM->has_debug_gui()) {
    // It's an async event, so allocate a pointer and send it.
    // The event will be freed by the recipient.
    BxEvent *event = new BxEvent();
    event->type = BX_ASYNC_EVT_REFRESH;
    sim_to_ci_event(event);
  }
}

bx_param_c *bx_real_sim_c::get_first_atadevice(Bit32u search_type)
{
  char pname[80];
  for (int channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    sprintf(pname, "ata.%d.resources.enabled", channel);
    if (!SIM->get_param_bool(pname)->get())
      continue;
    for (int slave=0; slave<2; slave++) {
      sprintf(pname, "ata.%d.%s.type", channel, (slave==0)?"master":"slave");
      Bit32u type = SIM->get_param_enum(pname)->get();
      if (type == search_type) {
        sprintf(pname, "ata.%d.%s", channel, (slave==0)?"master":"slave");
        return SIM->get_param(pname);
      }
    }
  }
  return NULL;
}

bool bx_real_sim_c::is_pci_device(const char *name)
{
#if BX_SUPPORT_PCI
  unsigned i, max_pci_slots = BX_N_PCI_SLOTS;
  char devname[80];
  const char *device;

  if (SIM->get_param_bool(BXPN_PCI_ENABLED)->get()) {
    if (SIM->get_param_enum(BXPN_PCI_CHIPSET)->get() == BX_PCI_CHIPSET_I440BX) {
      max_pci_slots = 4;
    }
    for (i = 0; i < max_pci_slots; i++) {
      sprintf(devname, "pci.slot.%d", i+1);
      device = SIM->get_param_enum(devname)->get_selected();
      if (!strcmp(name, device)) {
        return 1;
      }
    }
  }
#endif
  return 0;
}

bool bx_real_sim_c::is_agp_device(const char *name)
{
#if BX_SUPPORT_PCI
  if (get_param_bool(BXPN_PCI_ENABLED)->get() && DEV_agp_present()) {
    const char *device = SIM->get_param_enum("pci.slot.5")->get_selected();
    if (!strcmp(name, device)) {
      return 1;
    }
  }
#endif
  return 0;
}

#if BX_DEBUGGER

// this can be safely called from either thread.
void bx_real_sim_c::debug_break()
{
  bx_debug_break();
}

// this should only be called from the sim_thread.
void bx_real_sim_c::debug_interpret_cmd(char *cmd)
{
  if (!is_sim_thread()) {
    fprintf(stderr, "ERROR: debug_interpret_cmd called but not from sim_thread\n");
    return;
  }
  bx_dbg_interpret_line(cmd);
}

char *bx_real_sim_c::debug_get_next_command()
{
  BxEvent event;
  event.type = BX_SYNC_EVT_GET_DBG_COMMAND;
  BX_DEBUG(("asking for next debug command"));
  sim_to_ci_event (&event);
  BX_DEBUG(("received next debug command: '%s'", event.u.debugcmd.command));
  if (event.retcode >= 0)
    return event.u.debugcmd.command;
  return NULL;
}

void bx_real_sim_c::debug_puts(const char *text)
{
  if (SIM->has_debug_gui()) {
    // send message to the gui debugger
    BxEvent *event = new BxEvent();
    event->type = BX_ASYNC_EVT_DBG_MSG;
    event->u.logmsg.msg = text;
    sim_to_ci_event(event);
  } else {
    // text mode debugger: just write to console
    fputs(text, stdout);
  }
}
#endif

void bx_real_sim_c::register_configuration_interface(
  const char* name,
  config_interface_callback_t callback,
  void *userdata)
{
  ci_callback = callback;
  ci_callback_data = userdata;
  registered_ci_name = name;
}

int bx_real_sim_c::configuration_interface(const char *ignore, ci_command_t command)
{
  if (!ci_callback) {
    BX_PANIC(("no configuration interface was loaded"));
    return -1;
  }
  if (!strcmp(registered_ci_name, "wx")) {
    wxsel = 1;
  } else {
    wxsel = 0;
  }
  if (command == CI_START) {
    ci_started = 1;
  } else if (command == CI_SHUTDOWN) {
    ci_started = 0;
  }
  // enter configuration mode, just while running the configuration interface
  set_display_mode(DISP_MODE_CONFIG);
  int retval = (*ci_callback)(ci_callback_data, command);
  set_display_mode(DISP_MODE_SIM);
  return retval;
}

#if BX_USB_DEBUGGER
void bx_real_sim_c::register_usb_debug_type(int type)
{
  usb_dbg_register_type(type);
}

void bx_real_sim_c::usb_debug_trigger(int type, int trigger, Bit64u param0, int param1, int param2)
{
  usb_dbg_trigger(type, trigger, param0, param1, param2);
}

int bx_real_sim_c::usb_debug_interface(int type, Bit64u param0, int param1, int param2)
{
  int retval = -1;

  if (type != USB_DEBUG_NONE) {
    set_display_mode(DISP_MODE_CONFIG);
    retval = usb_dbg_interface(type, param0, param1, param2);
    set_display_mode(DISP_MODE_SIM);
  }
  return retval;
}
#endif

int bx_real_sim_c::begin_simulation(int argc, char *argv[])
{
  return bx_begin_simulation(argc, argv);
}

int bx_real_sim_c::register_runtime_config_handler(void *dev, rt_conf_handler_t handler)
{
  rt_conf_entry_t *rt_conf_entry = new rt_conf_entry_t;
  rt_conf_entry->id = rt_conf_id;
  rt_conf_entry->device = dev;
  rt_conf_entry->handler = handler;
  rt_conf_entry->next = NULL;

  if (rt_conf_entries == NULL) {
    rt_conf_entries = rt_conf_entry;
  } else {
    rt_conf_entry_t *temp = rt_conf_entries;

    while (temp->next) {
      temp = temp->next;
    }
    temp->next = rt_conf_entry;
  }
  return rt_conf_id++;
}

void bx_real_sim_c::unregister_runtime_config_handler(int id)
{
  rt_conf_entry_t *prev = NULL, *curr = rt_conf_entries;

  while (curr != NULL) {
    if (curr->id == id) {
      if (prev != NULL) {
        prev->next = curr->next;
      } else {
        rt_conf_entries = curr->next;
      }
      delete curr;
      break;
    } else {
      prev = curr;
      curr = curr->next;
    }
  }
}

void bx_real_sim_c::update_runtime_options()
{
  rt_conf_entry_t *temp = rt_conf_entries;

  while (temp != NULL) {
    temp->handler(temp->device);
    temp = temp->next;
  }
  bx_gui->update_drive_status_buttons();
  bx_virt_timer.set_realtime_delay();
}

bool bx_real_sim_c::is_sim_thread()
{
  if (is_sim_thread_func == NULL) return 1;
  return (*is_sim_thread_func)();
}

// check if the text console exists.  On some platforms, if Bochs is
// started from the "Start Menu" or by double clicking on it on a Mac,
// there may be nothing attached to stdin/stdout/stderr.  This function
// tests if stdin/stdout/stderr are usable and returns 0 if not.
bool bx_real_sim_c::test_for_text_console()
{
#if BX_WITH_CARBON
  // In a Carbon application, you have a text console if you run the app from
  // the command line, but if you start it from the finder you don't.
  if(!isatty(STDIN_FILENO)) return 0;
#endif
  // default: yes
  return 1;
}

bool bx_real_sim_c::is_addon_option(const char *keyword)
{
  addon_option_t *addon_option;

  for (addon_option = addon_options; addon_option; addon_option = addon_option->next) {
    if (!strcmp(addon_option->name, keyword)) return 1;
  }
  return 0;
}

bool bx_real_sim_c::register_addon_option(const char *keyword, addon_option_parser_t parser,
                                             addon_option_save_t save_func)
{
  addon_option_t *addon_option = new addon_option_t;
  addon_option->name = keyword;
  addon_option->parser = parser;
  addon_option->savefn = save_func;
  addon_option->next = NULL;

  if (addon_options == NULL) {
    addon_options = addon_option;
  } else {
    addon_option_t *temp = addon_options;

    while (temp->next) {
      if (!strcmp(temp->name, keyword)) {
        delete addon_option;
        return 0;
      }
      temp = temp->next;
    }
    temp->next = addon_option;
  }
  return 1;
}

bool bx_real_sim_c::unregister_addon_option(const char *keyword)
{
  addon_option_t *addon_option, *prev = NULL;

  for (addon_option = addon_options; addon_option; addon_option = addon_option->next) {
    if (!strcmp(addon_option->name, keyword)) {
      if (prev == NULL) {
        addon_options = addon_option->next;
      } else {
        prev->next = addon_option->next;
      }
      delete addon_option;
      return 1;
    } else {
      prev = addon_option;
    }
  }
  return 0;
}

Bit32s bx_real_sim_c::parse_addon_option(const char *context, int num_params, char *params [])
{
  for (addon_option_t *addon_option = addon_options; addon_option; addon_option = addon_option->next) {
    if ((!strcmp(addon_option->name, params[0])) &&
        (addon_option->parser != NULL)) {
      return (*addon_option->parser)(context, num_params, params);
    }
  }
  return -1;

}

Bit32s bx_real_sim_c::save_addon_options(FILE *fp)
{
  for (addon_option_t *addon_option = addon_options; addon_option; addon_option = addon_option->next) {
    if (addon_option->savefn != NULL) {
      (*addon_option->savefn)(fp);
    }
  }
  return 0;
}

void bx_real_sim_c::init_statistics()
{
  if (get_statistics_root() == NULL) {
    new bx_list_c(root_param, "statistics", "statistics");
  }
}

void bx_real_sim_c::cleanup_statistics()
{
  bx_list_c *list = get_statistics_root();

  if (list != NULL) {
    list->clear();
  }
}

void bx_real_sim_c::init_save_restore()
{
  if (get_bochs_root() == NULL) {
    new bx_list_c(root_param, "bochs", "subtree for save/restore");
  }
}

void bx_real_sim_c::cleanup_save_restore()
{
  bx_list_c *list = get_bochs_root();

  if (list != NULL) {
    list->clear();
  }
}

bool bx_real_sim_c::save_state(const char *checkpoint_path)
{
  char sr_file[BX_PATHNAME_LEN];
  char devname[20];
  int dev, ndev = SIM->get_n_log_modules();
  int type, ntype = SIM->get_max_log_level();

  get_param_string(BXPN_RESTORE_PATH)->set(checkpoint_path);
  sprintf(sr_file, "%s/config", checkpoint_path);
  if (write_rc(sr_file, 1) < 0)
    return 0;
  sprintf(sr_file, "%s/logopts", checkpoint_path);
  FILE *fp = fopen(sr_file, "w");
  if (fp != NULL) {
    for (dev=0; dev<ndev; dev++) {
      strcpy(devname, get_logfn_name(dev));
      if ((strlen(devname) > 0) && (strcmp(devname, "?"))) {
        fprintf(fp, "%s: ", devname);
        for (type=0; type<ntype; type++) {
          if (type > 0) fprintf(fp, ", ");
          fprintf(fp, "%s=%s", get_log_level_name(type), get_action_name(get_log_action(dev, type)));
        }
        fprintf(fp, "\n");
      }
    }
    fclose(fp);
  } else {
    return 0;
  }
  bx_list_c *sr_list = get_bochs_root();
  ndev = sr_list->get_size();
  for (dev=0; dev<ndev; dev++) {
    sprintf(sr_file, "%s/%s", checkpoint_path, sr_list->get(dev)->get_name());
    fp = fopen(sr_file, "w");
    if (fp != NULL) {
      save_sr_param(fp, sr_list->get(dev), checkpoint_path, 0);
      fclose(fp);
    } else {
      return 0;
    }
  }
  get_param_string(BXPN_RESTORE_PATH)->set("none");
  return 1;
}

bool bx_real_sim_c::restore_config()
{
  char config[BX_PATHNAME_LEN];
  sprintf(config, "%s/config", get_param_string(BXPN_RESTORE_PATH)->getptr());
  BX_INFO(("restoring '%s'", config));
  if (read_rc(config) >= 0) {
    return 1;
  } else {
     get_param_bool(BXPN_RESTORE_FLAG)->set(0);
    return 0;
  }
}

bool bx_real_sim_c::restore_logopts()
{
  char logopts[BX_PATHNAME_LEN];
  char line[512], string[512], devname[20];
  char *ret, *ptr;
  int i, j, p, dev = 0, type = 0, action = 0;
  FILE *fp;

  sprintf(logopts, "%s/logopts", get_param_string(BXPN_RESTORE_PATH)->getptr());
  BX_INFO(("restoring '%s'", logopts));
  fp = fopen(logopts, "r");
  if (fp != NULL) {
    do {
      ret = fgets(line, sizeof(line)-1, fp);
      line[sizeof(line) - 1] = '\0';
      int len = (int)strlen(line);
      if ((len>0) && (line[len-1] < ' '))
        line[len-1] = '\0';
      i = 0;
      if ((ret != NULL) && strlen(line)) {
        ptr = strtok(line, ":");
        while (ptr) {
          p = 0;
          while (isspace(ptr[p])) p++;
          strcpy(string, ptr+p);
          while (isspace(string[strlen(string)-1])) string[strlen(string)-1] = 0;
          if (i == 0) {
            strcpy(devname, string);
            dev = get_logfn_id(devname);
          } else if (dev >= 0) {
            j = 6;
            if (!strncmp(string, "DEBUG=", 6)) {
              type = LOGLEV_DEBUG;
            } else if (!strncmp(string, "INFO=", 5)) {
              type = LOGLEV_INFO;
              j = 5;
            } else if (!strncmp(string, "ERROR=", 6)) {
              type = LOGLEV_ERROR;
            } else if (!strncmp(string, "PANIC=", 6)) {
              type = LOGLEV_PANIC;
            }
            action = is_action_name(string+j);
            if (action >= ACT_IGNORE) {
              set_log_action(dev, type, action);
            }
          } else {
            if (i == 1) {
              BX_ERROR(("restore_logopts(): log module '%s' not found", devname));
            }
          }
          i++;
          ptr = strtok(NULL, ",");
        }
      }
    } while (!feof(fp));
    fclose(fp);
  } else {
    return 0;
  }
  return 1;
}

static int bx_restore_getline(FILE *fp, char *line, int maxlen)
{
  char *ret = fgets(line, maxlen - 1, fp);
  line[maxlen - 1] = '\0';
  int len = (int)strlen(line);
  if ((len > 0) && (line[len - 1] < ' '))
    line[len - 1] = '\0';
  return (ret != NULL) ? len : 0;
}

bool bx_real_sim_c::restore_bochs_param(bx_list_c *root, const char *sr_path, const char *restore_name)
{
  char devstate[BX_PATHNAME_LEN], devdata[BX_PATHNAME_LEN];
  char line[512], buf[512];
  char pname[81]; // take extra 81st character for /0
  char *ptr;
  int i;
  unsigned n;
  bx_param_c *param = NULL;
  FILE *fp, *fp2;

  if (root->get_by_name(restore_name) == NULL) {
    BX_ERROR(("restore_bochs_param(): unknown parameter to restore"));
    return 0;
  }

  sprintf(devstate, "%s/%s", sr_path, restore_name);
  BX_INFO(("restoring '%s'", devstate));
  bx_list_c *base = root;
  fp = fopen(devstate, "r");
  if (fp != NULL) {
    do {
      int len = bx_restore_getline(fp, line, BX_PATHNAME_LEN);
      i = 0;
      if (len > 0) {
        ptr = strtok(line, " ");
        while (ptr) {
          if (i == 0) {
            if (!strcmp(ptr, "}")) {
              base->restore();
              base = (bx_list_c*)base->get_parent();
              break;
            } else {
              param = get_param(ptr, base);
              strncpy(pname, ptr, 80);
            }
          } else if (i == 2) {
            if (param == NULL) {
              BX_PANIC(("cannot find param '%s'!", pname));
            }
            else {
              if (param->get_type() != BXT_LIST) {
                param->get_param_path(pname, 80);
                BX_DEBUG(("restoring parameter '%s'", pname));
              }
              switch (param->get_type()) {
                case BXT_PARAM_NUM:
                case BXT_PARAM_BOOL:
                case BXT_PARAM_ENUM:
                case BXT_PARAM_STRING:
                case BXT_PARAM_BYTESTRING:
                  param->parse_param(ptr);
                  break;
                case BXT_PARAM_DATA:
                  {
                    bx_shadow_data_c *dparam = (bx_shadow_data_c*)param;
                    if (!dparam->is_text_format()) {
                      sprintf(devdata, "%s/%s", sr_path, ptr);
                      fp2 = fopen(devdata, "rb");
                      if (fp2 != NULL) {
                        fread(dparam->getptr(), 1, dparam->get_size(), fp2);
                        fclose(fp2);
                      }
                    } else if (!strcmp(ptr, "[")) {
                      i = 0;
                      do {
                        bx_restore_getline(fp, buf, BX_PATHNAME_LEN);
                        ptr = strtok(buf, " ");
                        while (ptr) {
                          if (!strcmp(ptr, "]")) {
                            i = 0;
                            break;
                          } else {
                            if (sscanf(ptr, "0x%02x", &n) == 1) {
                              dparam->set(i++, (Bit8u)n);
                            }
                          }
                          ptr = strtok(NULL, " ");
                        }
                      } while (i > 0);
                    }
                  }
                  break;
                case BXT_PARAM_FILEDATA:
                  sprintf(devdata, "%s/%s", sr_path, ptr);
                  fp2 = fopen(devdata, "rb");
                  if (fp2 != NULL) {
                    FILE **fpp = ((bx_shadow_filedata_c*)param)->get_fpp();
                    // If the temporary backing store file wasn't created, do it now.
                    if (*fpp == NULL) {
                      *fpp = tmpfile64();
                    } else {
                      fseeko64(*fpp, 0, SEEK_SET);
                    }
                    if (*fpp != NULL) {
                      char *buffer = new char[4096];
                      while (!feof(fp2)) {
                        size_t chars = fread(buffer, 1, 4096, fp2);
                        fwrite(buffer, 1, chars, *fpp);
                      }
                      delete [] buffer;
                      fflush(*fpp);
                    }
                    ((bx_shadow_filedata_c*)param)->restore(fp2);
                    fclose(fp2);
                  }
                  break;
                case BXT_LIST:
                  base = (bx_list_c*)param;
                  break;
                default:
                  BX_ERROR(("restore_bochs_param(): unknown parameter type"));
              }
            }
          }
          i++;
          ptr = strtok(NULL, " ");
        }
      }
    } while (!feof(fp));
    fclose(fp);
  } else {
    BX_ERROR(("restore_bochs_param(): error in file open"));
    return 0;
  }

  return 1;
}

bool bx_real_sim_c::restore_hardware()
{
  bx_list_c *sr_list = get_bochs_root();
  int ndev = sr_list->get_size();
  for (int dev=0; dev<ndev; dev++) {
    if (!restore_bochs_param(sr_list, get_param_string(BXPN_RESTORE_PATH)->getptr(), sr_list->get(dev)->get_name()))
      return 0;
  }
  return 1;
}

bool bx_real_sim_c::save_sr_param(FILE *fp, bx_param_c *node, const char *sr_path, int level)
{
  int i, j;
  char pname[BX_PATHNAME_LEN], tmpstr[BX_PATHNAME_LEN+1];
  FILE *fp2;

  for (i=0; i<level; i++)
    fprintf(fp, "  ");
  if (node == NULL) {
    BX_ERROR(("NULL pointer"));
    return 0;
  }
  fprintf(fp, "%s = ", node->get_name());
  switch (node->get_type()) {
    case BXT_PARAM_NUM:
    case BXT_PARAM_BOOL:
    case BXT_PARAM_ENUM:
    case BXT_PARAM_STRING:
    case BXT_PARAM_BYTESTRING:
      node->dump_param(fp);
      fprintf(fp, "\n");
      break;
    case BXT_PARAM_DATA:
      {
        bx_shadow_data_c *dparam = (bx_shadow_data_c*)node;
        if (!dparam->is_text_format()) {
          node->get_param_path(pname, BX_PATHNAME_LEN);
          if (!strncmp(pname, "bochs.", 6)) {
            strcpy(pname, pname+6);
          }
          fprintf(fp, "%s\n", pname);
          if (sr_path)
            sprintf(tmpstr, "%s/%s", sr_path, pname);
          else
            strcpy(tmpstr, pname);
          fp2 = fopen(tmpstr, "wb");
          if (fp2 != NULL) {
            fwrite(dparam->getptr(), 1, dparam->get_size(), fp2);
            fclose(fp2);
          }
        } else {
          fprintf(fp, "[\n");
          for (i=0; i < (int)dparam->get_size(); i++) {
            if ((i % 16) == 0) {
              for (j=0; j<(level+1); j++)
                fprintf(fp, "  ");
            } else {
              fprintf(fp, ", ");
            }
            fprintf(fp, "0x%02x", dparam->get(i));
            if (i == (int)(dparam->get_size() - 1)) {
              fprintf(fp, "\n");
            } else if ((i % 16) == 15) {
              fprintf(fp, ",\n");
            }
          }
          for (i=0; i<level; i++)
            fprintf(fp, "  ");
          fprintf(fp, "]\n");
        }
      }
      break;
    case BXT_PARAM_FILEDATA:
      fprintf(fp, "%s.%s\n", node->get_parent()->get_name(), node->get_name());
      if (sr_path)
        sprintf(tmpstr, "%s/%s.%s", sr_path, node->get_parent()->get_name(), node->get_name());
      else
        sprintf(tmpstr, "%s.%s", node->get_parent()->get_name(), node->get_name());
      fp2 = fopen(tmpstr, "wb");
      if (fp2 != NULL) {
        FILE **fpp = ((bx_shadow_filedata_c*)node)->get_fpp();
        // If the backing store hasn't been created, just save an empty 0 byte placeholder file.
        if (*fpp != NULL) {
          char *buffer = new char[4096];
          fseeko64(*fpp, 0, SEEK_SET);
          while (!feof(*fpp)) {
            size_t chars = fread (buffer, 1, 4096, *fpp);
            fwrite(buffer, 1, chars, fp2);
          }
          delete [] buffer;
        }
        ((bx_shadow_filedata_c*)node)->save(fp2);
        fclose(fp2);
      }
      break;
    case BXT_LIST:
      {
        fprintf(fp, "{\n");
        bx_list_c *list = (bx_list_c*)node;
        for (i=0; i < list->get_size(); i++) {
          save_sr_param(fp, list->get(i), sr_path, level+1);
        }
        for (i=0; i<level; i++)
          fprintf(fp, "  ");
        fprintf(fp, "}\n");
        break;
      }
    default:
      BX_ERROR(("save_sr_param(): unknown parameter type"));
      return 0;
  }

  return 1;
}

bool bx_real_sim_c::opt_plugin_ctrl(const char *plugname, bool load)
{
  bx_list_c *plugin_ctrl = (bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL);
  if (!strcmp(plugname, "*")) {
    // verify optional plugin configuration and load/unload plugins if necessary
    int i = 0;
    while (i < plugin_ctrl->get_size()) {
      bx_param_bool_c *plugin = (bx_param_bool_c*)plugin_ctrl->get(i);
      if (load == (bool)plugin->get()) {
        opt_plugin_ctrl(plugin->get_name(), load);
      }
      i++;
    }
    return 1;
  }
  if (plugin_ctrl->get_by_name(plugname) == NULL) {
    BX_PANIC(("Plugin '%s' not found", plugname));
    return 0;
  }
  if (load != PLUG_device_present(plugname)) {
    if (load) {
      if (PLUG_load_opt_plugin(plugname)) {
        SIM->get_param_bool(plugname, plugin_ctrl)->set(1);
        return 1;
      } else {
        // plugin load code panics in this case
        return 0;
      }
    } else {
      if (PLUG_unload_opt_plugin(plugname)) {
        SIM->get_param_bool(plugname, plugin_ctrl)->set(0);
        return 1;
      } else {
        // plugin load code panics in this case
        return 0;
      }
    }
  }
  return 0;
}

#if BX_NETWORKING
void bx_real_sim_c::init_std_nic_options(const char *name, bx_list_c *menu)
{
  bx_init_std_nic_options(name, menu);
}
#endif

#if BX_SUPPORT_PCIUSB
void bx_real_sim_c::init_usb_options(const char *usb_name, const char *pname, int maxports, int param0)
{
  bx_init_usb_options(usb_name, pname, maxports, param0);
}
#endif


int bx_real_sim_c::parse_param_from_list(const char *context, const char *param, bx_list_c *base)
{
  return bx_parse_param_from_list(context, param, base);
}

int bx_real_sim_c::parse_nic_params(const char *context, const char *param, bx_list_c *base)
{
  return bx_parse_nic_params(context, param, base);
}

int bx_real_sim_c::parse_usb_port_params(const char *context, const char *param,
                                         int maxports, bx_list_c *base)
{
  return bx_parse_usb_port_params(context, param, maxports, base);
}

int bx_real_sim_c::split_option_list(const char *msg, const char *rawopt,
                                     char **argv, int max_argv)
{
  return bx_split_option_list(msg, rawopt, argv, max_argv);
}

int bx_real_sim_c::write_param_list(FILE *fp, bx_list_c *base, const char *optname, bool multiline)
{
  return bx_write_param_list(fp, base, optname, multiline);
}

#if BX_SUPPORT_PCIUSB
int bx_real_sim_c::write_usb_options(FILE *fp, int maxports, bx_list_c *base)
{
  return bx_write_usb_options(fp, maxports, base);
}
#endif

#if BX_USE_GUI_CONSOLE
int bx_real_sim_c::bx_printf(const char *fmt, ...)
{
  va_list ap;
  char buf[1025];

  va_start(ap, fmt);
  vsnprintf(buf, 1024, fmt, ap);
  va_end(ap);
  if (get_init_done()) {
    if (bx_gui->has_gui_console()) {
      return bx_gui->bx_printf(buf);
    }
  }
  return printf("%s", buf);
}

char* bx_real_sim_c::bx_gets(char *s, int size, FILE *stream)
{
  if (get_init_done()) {
    if (bx_gui->has_gui_console()) {
      return bx_gui->bx_gets(s, size);
    }
  }
  return fgets(s, size, stream);
}
#endif
