/*
 * gui/siminterface.cc
 * $Id: siminterface.cc,v 1.17 2001-06-18 14:11:55 bdenney Exp $
 *
 * Defines the actual link between bx_simulator_interface_c methods
 * and the simulator.  This file includes bochs.h because it needs
 * access to bx_options and other simulator objecst and methods.
 *
 */

#include "bochs.h"

#if BX_USE_CONTROL_PANEL

bx_simulator_interface_c *SIM = NULL;
logfunctions *siminterface_log = NULL;
#define LOG_THIS siminterface_log->

class bx_real_sim_c : public bx_simulator_interface_c {
  sim_interface_callback_t callback;
  int notify_return_val;
  int notify_int_args[10];
  char *notify_string_args[10];
#define NOTIFY_TYPE_INT
#define NOTIFY_TYPE_STRING
  int init_done;
  bx_param_c **param_registry;
  int registry_alloc_size;
public:
  bx_real_sim_c ();
  virtual int get_init_done () { return init_done; }
  virtual int set_init_done (int n) { init_done = n; return 0;}
  virtual int register_param (bx_id id, bx_param_c *it);
  virtual bx_param_c *get_param (bx_id id);
  virtual bx_param_num_c *get_param_num (bx_id id);
  virtual bx_param_string_c *get_param_string (bx_id id);
  virtual int get_n_log_modules ();
  virtual char *get_prefix (int mod);
  virtual int get_log_action (int mod, int level);
  virtual void set_log_action (int mod, int level, int action);
  virtual char *get_action_name (int action);
  virtual char *get_log_level_name (int level);
  virtual int get_max_log_level ();
  virtual void quit_sim (int clean);
  virtual int get_default_rc (char *path, int len);
  virtual int read_rc (char *path);
  virtual int write_rc (char *path, int overwrite);
  virtual int get_log_file (char *path, int len);
  virtual int set_log_file (char *path);
  virtual int get_floppy_options (int drive, bx_floppy_options *out);
  virtual int set_floppy_options (int drive, bx_floppy_options *in);
  virtual int get_disk_options (int drive, bx_disk_options *out);
  virtual int set_disk_options (int drive, bx_disk_options *out);
  virtual int get_cdrom_options (int drive, bx_cdrom_options *out);
  virtual int set_cdrom_options (int drive, bx_cdrom_options *out);
  virtual int get_newhd_support ();
  virtual void set_newhd_support (int en);
  virtual char *get_floppy_type_name (int type);
  virtual int get_boot_hard_disk ();
  virtual int set_boot_hard_disk (int val);
  virtual int get_private_colormap ();
  virtual void set_private_colormap (int en);
  virtual void set_notify_callback (sim_interface_callback_t func);
  virtual int notify_return (int retcode);
  virtual int LOCAL_notify (int code);
  virtual int LOCAL_log_msg (char *prefix, int level, char *msg);
  virtual int log_msg_2 (char *prefix, int *level, char *msg, int len);
};

bx_param_c *
bx_real_sim_c::get_param (bx_id id)
{
  BX_ASSERT (id >= BXP_NULL && id < BXP_THIS_IS_THE_LAST);
  int index = (int)id - BXP_NULL;
  bx_param_c *retval = param_registry[index];
  if (!retval)
    BX_PANIC (("get_param can't find id %u", id));
  return retval;
}

bx_param_num_c *
bx_real_sim_c::get_param_num (bx_id id) {
  bx_param_c *generic = get_param(id);
  if (generic==NULL) {
    BX_PANIC (("get_param_num(%u) could not find a parameter", id));
    return NULL;
  }
  if (generic->get_type () == BXT_PARAM_NUM)
    return (bx_param_num_c *)generic;
  BX_PANIC (("get_param_num %u could not find an integer parameter with that id", id));
  return NULL;
}

bx_param_string_c *
bx_real_sim_c::get_param_string (bx_id id) {
  bx_param_c *generic = get_param(id);
  if (generic==NULL) {
    BX_PANIC (("get_param_string(%u) could not find a parameter", id));
    return NULL;
  }
  if (generic->get_type () == BXT_PARAM_STRING)
    return (bx_param_string_c *)generic;
  BX_PANIC (("get_param_string %u could not find an integer parameter with that id", id));
  return NULL;
}

void init_siminterface ()
{
  siminterface_log = new logfunctions ();
  siminterface_log->setprefix ("[CTRL]");
  siminterface_log->settype(CTRLLOG);
  if (SIM == NULL) 
    SIM = new bx_real_sim_c();
}

bx_simulator_interface_c::bx_simulator_interface_c ()
{
}

bx_real_sim_c::bx_real_sim_c ()
{
  callback = NULL;
  registry_alloc_size = BXP_THIS_IS_THE_LAST - BXP_NULL;
  param_registry = new (bx_param_c*)[registry_alloc_size];
}

int
bx_real_sim_c::register_param (bx_id id, bx_param_c *it)
{
  BX_ASSERT (id >= BXP_NULL && id < BXP_THIS_IS_THE_LAST);
  int index = (int)id - BXP_NULL;
  this->param_registry[index] = it;
}

int 
bx_real_sim_c::get_n_log_modules ()
{
  return io->get_n_logfns ();
}

char *
bx_real_sim_c::get_prefix (int mod)
{
  logfunc_t *logfn = io->get_logfn (mod);
  return logfn->getprefix ();
}

int 
bx_real_sim_c::get_log_action (int mod, int level)
{
  logfunc_t *logfn = io->get_logfn (mod);
  return logfn->getonoff (level);
}

void 
bx_real_sim_c::set_log_action (int mod, int level, int action)
{
  logfunc_t *logfn = io->get_logfn (mod);
  return logfn->setonoff (level, action);
}

char *
bx_real_sim_c::get_action_name (int action)
{
  return io->getaction (action);
}

char *
bx_real_sim_c::get_log_level_name (int level)
{
  return io->getlevel (level);
}

int 
bx_real_sim_c::get_max_log_level ()
{
  return N_LOGLEV;
}

void 
bx_real_sim_c::quit_sim (int clean) {
  if (!clean)
    BX_PANIC (("Quit simulation command"));
  ::exit (0);
}

int
bx_real_sim_c::get_default_rc (char *path, int len)
{
  char *rc = bx_find_bochsrc ();
  if (rc == NULL) return -1;
  strncpy (path, rc, len);
  return 0;
}

int 
bx_real_sim_c::read_rc (char *rc)
{
  return bx_read_configuration (rc);
}

// return values:
//   0: written ok
//  -1: failed
//  -2: already exists, and overwrite was off
int 
bx_real_sim_c::write_rc (char *rc, int overwrite)
{
  return bx_write_configuration (rc, overwrite);
}

int 
bx_real_sim_c::get_log_file (char *path, int len)
{
  strncpy (path, bx_options.log.filename, len);
  return 0;
}

int 
bx_real_sim_c::set_log_file (char *path)
{
  strncpy (bx_options.log.filename, path, sizeof(bx_options.log.filename));
  return 0;
}

int 
bx_real_sim_c::get_floppy_options (int drive, bx_floppy_options *out)
{
  *out = (drive==0)? bx_options.floppya : bx_options.floppyb;
  return 0;
}

// return values:
//   0: success
//  -1: could not open the file given by path (unless type==none)
//  -2: could open file but need hint on size
int
bx_real_sim_c::set_floppy_options (int drive, bx_floppy_options *in)
{
  bx_floppy_options *dest = 
      (drive==0)? &bx_options.floppya : &bx_options.floppyb;
  if (!get_init_done ()) {
    *dest = *in;
    return 0;
  } else {
    // bochs is already running.  Try to open the image file
    *dest = *in;
    bx_devices.floppy->set_media_status (drive, 0);
    int status = bx_devices.floppy->set_media_status (drive, 1);
    BX_INFO (("set_media_status for drive %d returned %d\n", drive, status));
    if (status==0) return -1;
    return 0;
  }
}

int 
bx_real_sim_c::get_disk_options (int drive, bx_disk_options *out)
{
  *out = (drive==0)? bx_options.diskc : bx_options.diskd;
  return 0;
}

int
bx_real_sim_c::set_disk_options (int drive, bx_disk_options *in)
{
  bx_disk_options *dest = (drive==0)? &bx_options.diskc : &bx_options.diskd;
  *dest = *in;
  return 0;
}

int 
bx_real_sim_c::get_cdrom_options (int drive, bx_cdrom_options *out)
{
  BX_ASSERT (drive == 0);
  *out = bx_options.cdromd;
  return 0;
}

int 
bx_real_sim_c::set_cdrom_options (int drive, bx_cdrom_options *out)
{
  BX_ASSERT (drive == 0);
  bx_options.cdromd = *out;
  return 0;
}

int 
bx_real_sim_c::get_newhd_support ()
{
  return bx_options.newHardDriveSupport;
}

void 
bx_real_sim_c::set_newhd_support (int en)
{
  bx_options.newHardDriveSupport = (en != 0);
}

char *floppy_type_names[] = { "none", "1.2M", "1.44M", "2.88M", "720K" };
int n_floppy_type_names = 5;

char *
bx_real_sim_c::get_floppy_type_name (int type)
{
  BX_ASSERT (type >= BX_FLOPPY_NONE && type <= BX_FLOPPY_720K);
  type -= BX_FLOPPY_NONE;
  return floppy_type_names[type];
}

int 
bx_real_sim_c::get_boot_hard_disk ()
{
  return bx_options.bootdrive[0] == 'c';
}

int 
bx_real_sim_c::set_boot_hard_disk (int val)
{
  bx_options.bootdrive[0] = val?  'c' : 'a';
  bx_options.bootdrive[1] = 0;
  return 0;
}

#if 0
int 
bx_real_sim_c::get_rom_path (char *buf, int len)
{
  if (bx_options.rom.path)
    strncpy (buf, bx_options.rom.path, len);
  else
    buf[0] = 0;
  return 0;
}

int 
bx_real_sim_c::set_rom_path (char *path)
{
  bx_options.rom.path = strdup (path);
  return 0;
}
#endif

int 
bx_real_sim_c::get_private_colormap ()
{
  return bx_options.private_colormap;
}

void 
bx_real_sim_c::set_private_colormap (int en)
{
  bx_options.private_colormap = en;
}

void 
bx_real_sim_c::set_notify_callback (sim_interface_callback_t func)
{
  callback = func;
}

int 
bx_real_sim_c::notify_return (int retcode)
{
  notify_return_val = retcode;
  return 0;
}

int
bx_real_sim_c::LOCAL_notify (int code)
{
  if (callback == NULL) {
    BX_ERROR (("notify called, but no callback function is registered"));
    return -1;
  } else {
    notify_return_val = -999;
    (*callback)(code);
    if (notify_return_val == -999)
      BX_ERROR (("notify callback returned without setting the return value"));
    return notify_return_val;
  }
}

// returns 0 for continue, 1 for alwayscontinue, 2 for die.
int 
bx_real_sim_c::LOCAL_log_msg (char *prefix, int level, char *msg)
{
  //fprintf (stderr, "calling notify.\n");
  notify_string_args[0] = prefix;
  notify_int_args[1] = level;
  notify_string_args[2] = msg;
  int val = LOCAL_notify (NOTIFY_CODE_LOGMSG);
  //fprintf (stderr, "notify returned %d\n", val);
  return val;
}

// called by control.cc
int
bx_real_sim_c::log_msg_2 (char *prefix, int *level, char *msg, int len)
{
  strncpy (prefix, notify_string_args[0], len);
  *level= notify_int_args[1];
  strncpy (msg, notify_string_args[2], len);
  return 0;
}

/////////////////////////////////////////////////////////////////////////

// new stuff

bx_object_c::bx_object_c (bx_id id)
{
  this->id = id;
  this->type = BXT_OBJECT;
}

void 
bx_object_c::set_type (Bit8u type)
{
  this->type = type;
}

bx_param_c::bx_param_c (bx_id id, char *name, char *description)
  : bx_object_c (id)
{
  set_type (BXT_PARAM);
  this->name = name;
  this->description = description;
  this->text_format = NULL;
  this->ask_format = NULL;
  this->runtime_param = 0;
  SIM->register_param (id, this);
}

bx_param_num_c::bx_param_num_c (bx_id id,
    char *name,
    char *description,
    Bit32s min, Bit32s max, Bit32s initial_val)
  : bx_param_c (id, name, description)
{
  set_type (BXT_PARAM_NUM);
  this->min = min;
  this->max = max;
  this->initial_val = initial_val;
  this->val = initial_val;
  this->handler = NULL;
  this->base = 10;
  SIM->register_param (id, this);
}

void 
bx_param_num_c::reset ()
{
  this->val = initial_val;
}

Bit32s 
bx_param_num_c::get ()
{
  if (handler)
    val = (*handler)(this, 0, val);
  return val;
}

void
bx_param_num_c::set (Bit32s newval)
{
  if (handler)
    val = (*handler)(this, 1, newval);
  else
    val = newval;
}

bx_param_string_c::bx_param_string_c (bx_id id,
    char *name,
    char *description,
    char *initial_val,
    int maxsize)
  : bx_param_c (id, name, description)
{
  set_type (BXT_PARAM_STRING);
  if (maxsize < 0) 
    maxsize = strlen(initial_val) + 1;
  this->val = new char[maxsize];
  this->initial_val = new char[maxsize];
  this->maxsize = maxsize;
  strncpy (this->val, initial_val, maxsize);
  strncpy (this->initial_val, initial_val, maxsize);
}

void 
bx_param_string_c::reset () {
  strncpy (this->val, this->initial_val, maxsize);
}

Bit32s
bx_param_string_c::get (char *buf, int len)
{
  if (handler)
    (*handler)(this, 0, buf, len);
  else
    strncpy (buf, val, len);
  return 0;
}

void 
bx_param_string_c::set (char *buf)
{
  if (handler)
    (*handler)(this, 1, buf, -1);
  else
    strncpy (val, buf, maxsize);
}

bx_list_c::bx_list_c (bx_id id, int maxsize)
  : bx_param_c (id, "list", "")
{
  set_type (BXT_LIST);
  this->maxsize = maxsize;
  this->size = 0;
  this->list = new (bx_param_c *)[maxsize];
  init ();
}

bx_list_c::bx_list_c (bx_id id, bx_param_c **init_list)
  : bx_param_c (id, "list", "")
{
  set_type (BXT_LIST);
  size = 0;
  while (init_list[size] != NULL)
    size++;
  this->maxsize = size;
  this->list = new (bx_param_c *)[maxsize];
  for (int i=0; i<size; i++)
    this->list[i] = init_list[i];
  init ();
}

void
bx_list_c::init ()
{
  this->title = new bx_param_string_c (BXP_LIST_TITLE,
      "list_title",
      "title of the bx_list",
      "list", 80);
  this->options = new bx_param_num_c (BXP_LIST_OPTIONS,
      "list_option", "", 0, 1<<31,
      0);
  this->choice = new bx_param_num_c (BXP_LIST_CHOICE,
      "list_choice", "", 0, 1<<31,
      1);
  this->parent = NULL;
}

void
bx_list_c::add (bx_param_c *param)
{
  if (this->size >= this->maxsize)
    BX_PANIC (("bx_list_c::add parameter id=%u exceeds capacity of list", param->get_id ()));
  list[size] = param;
  size++;
}

bx_param_c *
bx_list_c::get (int index)
{
  BX_ASSERT (index >= 0 && index < size);
  return list[index];
}

void
bx_list_c::set_parent (bx_param_c *parent)
{
  this->parent = parent;
}

#endif  // if BX_USE_CONTROL_PANEL==1


