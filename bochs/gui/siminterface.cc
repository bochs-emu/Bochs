/*
 * gui/siminterface.cc
 * $Id: siminterface.cc,v 1.13 2001-06-15 23:52:34 bdenney Exp $
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
public:
  bx_real_sim_c ();
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
  virtual int get_rom_path (char *buf, int len);
  virtual int set_rom_path (char *path);
  virtual int get_vga_path (char *buf, int len);
  virtual int set_vga_path (char *path);
  virtual int get_rom_address ();
  virtual int set_rom_address (int addr);
  virtual int get_private_colormap ();
  virtual void set_private_colormap (int en);
  virtual void set_notify_callback (sim_interface_callback_t func);
  virtual int notify_return (int retcode);
  virtual int LOCAL_notify (int code);
  virtual int LOCAL_log_msg (char *prefix, int level, char *msg);
  virtual int log_msg_2 (char *prefix, int *level, char *msg, int len);
};

Bit32s
multipurpose_handler (bx_param_c *param, int set, Bit32s val)
{
  /* this function handles change events on several parameters */
  switch (param->get_id ()) {
    case BXP_IPS:
      if (set)
	return (bx_options.ips = val);
      else
	return bx_options.ips;
    case BXP_VGA_UPDATE_INTERVAL:
      if (set) {
	bx_options.vga_update_interval = val;
	if (SIM->get_init_done ())
	  bx_vga.set_update_interval (val);
	return val;
      } else {
	return bx_options.vga_update_interval;
      }
    case BXP_MOUSE_ENABLED:
      if (set) {
	bx_options.mouse_enabled = val;
	if (SIM->get_init_done ())
	  bx_gui.gui_set_mouse_enable (val!=0);
	return val;
      } else {
	return bx_gui.gui_get_mouse_enable ();
      }
    case BXP_MEM_SIZE:
      /* for simple stuff like mem size, you only need this handler because
         the memory expects to find the value in bx_options.memory.megs.
	 If instead it looked for the value here in SIM->memsize, then
	 no handler is necessary. */
      if (set)
	return (bx_options.memory.megs = val);
      else
	return bx_options.memory.megs;
    default:
      BX_PANIC (("multipurpose_handler called with unknown parameter"));
  }
}

void init_siminterface ()
{
  siminterface_log = new logfunctions ();
  siminterface_log->setprefix ("[CTRL]");
  siminterface_log->settype(CTRLLOG);
  if (SIM == NULL) 
    SIM = new bx_real_sim_c();
  SIM->ips = new bx_param_num_c (BXP_IPS, "ips", "Emulated instructions per second, used to calibrate bochs emulated\ntime with wall clock time.", 1, 1<<31, 500000);
  SIM->ips->set_handler (multipurpose_handler);
  SIM->vga_update_interval = new bx_param_num_c (BXP_VGA_UPDATE_INTERVAL, "vga_update_interval", "Number of microseconds between VGA updates", 1, 1<<31, 30000);
  SIM->vga_update_interval->set_handler (multipurpose_handler);
  SIM->mouse_enabled = new bx_param_num_c (BXP_MOUSE_ENABLED, "mouse_enabled", "Controls whether the mouse sends events to bochs", 0, 1, 0);
  SIM->mouse_enabled->set_handler (multipurpose_handler);
  SIM->memsize = new bx_param_num_c (BXP_MEM_SIZE, "megs", "Amount of RAM in megabytes", 1, 1<<31, 4);
  SIM->memsize->set_handler (multipurpose_handler);
}

bx_simulator_interface_c::bx_simulator_interface_c ()
{
  init_done = 0;
}

bx_real_sim_c::bx_real_sim_c ()
{
  callback = NULL;
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

int 
bx_real_sim_c::get_vga_path (char *buf, int len)
{
  if (bx_options.vgarom.path)
    strncpy (buf, bx_options.vgarom.path, len);
  else
    buf[0] = 0;
  return 0;
}

int 
bx_real_sim_c::set_vga_path (char *path)
{
  bx_options.vgarom.path = strdup (path);
  return 0;
}

int 
bx_real_sim_c::get_rom_address ()
{
  return bx_options.rom.address;
}

int 
bx_real_sim_c::set_rom_address (int addr)
{
  bx_options.rom.address = addr;
  return 0;
}

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
}

void 
bx_param_num_c::reset ()
{
  this->val = initial_val;
}

Bit32s 
bx_param_num_c::get ()
{
  int retval = val;
  if (handler) {
    retval = (*handler)(this, 0, val);
  }
  return retval;
}

Bit32s 
bx_param_num_c::set (Bit32s newval)
{
  if (handler)
    val = (*handler)(this, 1, newval);
  else
    val = newval;
  return newval;
}

bx_node_c::bx_node_c (bx_id id)
  : bx_object_c (id)
{
  set_type (BXT_NODE);
  car = NULL;
  cdr = NULL;
}

bx_node_c::bx_node_c (bx_id id, bx_object_c *car, bx_object_c *cdr)
  : bx_object_c (id)
{
  set_type (BXT_NODE);
  this->car = car;
  this->cdr = cdr;
}

#endif  // if BX_USE_CONTROL_PANEL==1


