/*
 * gui/siminterface.cc
 * $Id: siminterface.cc,v 1.6 2001-06-09 21:19:58 bdenney Exp $
 *
 * Defines the actual link between bx_simulator_interface_c methods
 * and the simulator.  This file includes bochs.h because it needs
 * access to bx_options and other simulator objecst and methods.
 *
 */

#include "bochs.h"

// this dummy only exists for purposes of log functions.  It would be
// lovely to get rid of it.
class siminterface_dummy_c : public logfunctions {
public:
  siminterface_dummy_c () {
    setprefix("[CTRL]"); // control panel
    settype(CTRLLOG);
  }
};
siminterface_dummy_c siminterface_dummy;

bx_simulator_interface_c *SIM = NULL;
#define LOG_THIS siminterface_dummy.

class bx_real_sim_c : public bx_simulator_interface_c {
  virtual int getips ();
  virtual void setips (int ips);
  virtual int get_vga_update_interval ();
  virtual void set_vga_update_interval (unsigned interval);
  virtual int get_n_log_modules ();
  virtual char *get_prefix (int mod);
  virtual int get_log_action (int mod, int level);
  virtual void set_log_action (int mod, int level, int action);
  virtual char *get_action_name (int action);
  virtual char *get_log_level_name (int level);
  virtual int get_max_log_level ();
  virtual void quit_sim (int clean);
  virtual int get_mouse_enabled ();
  virtual void set_mouse_enabled (int en);
  virtual int get_default_rc (char *path, int len);
  virtual int read_rc (char *path);
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
  virtual int get_mem_size ();
  virtual int set_mem_size (int megs);
  virtual int get_rom_path (char *buf, int len);
  virtual int set_rom_path (char *path);
  virtual int get_vga_path (char *buf, int len);
  virtual int set_vga_path (char *path);
  virtual int get_rom_address ();
  virtual int set_rom_address (int addr);
};

void init_siminterface ()
{
  if (SIM == NULL) 
    SIM = new bx_real_sim_c();
}

bx_simulator_interface_c::bx_simulator_interface_c ()
{
  init_done = 0;
}

int 
bx_real_sim_c::getips ()
{
  return bx_options.ips;
}

void 
bx_real_sim_c::setips (int ips)
{
  BX_ERROR (("Changing ips during simulation doesn't really work yet.  We need to reschedule the timers or something."));
  bx_options.ips = ips;
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
  return MAX_LOGLEV;
}

void 
bx_real_sim_c::quit_sim (int clean) {
  if (!clean)
    BX_PANIC (("Quit simulation command"));
  ::exit (0);
}

int 
bx_real_sim_c::get_vga_update_interval () {
  return bx_options.vga_update_interval;
}

void 
bx_real_sim_c::set_vga_update_interval (unsigned interval) {
  bx_vga.set_update_interval (interval);
}

int bx_real_sim_c::get_mouse_enabled () {
  return bx_gui.gui_get_mouse_enable ();
}

void bx_real_sim_c::set_mouse_enabled (int en) {
  bx_gui.gui_set_mouse_enable (en!=0);
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
  return bx_read_configuration (rc, 0, NULL);
}

int 
bx_real_sim_c::get_log_file (char *path, int len)
{
  strncpy (path, bx_options.logfilename, len);
  return 0;
}

int 
bx_real_sim_c::set_log_file (char *path)
{
  strncpy (bx_options.logfilename, path, sizeof(bx_options.logfilename));
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
}

int 
bx_real_sim_c::set_cdrom_options (int drive, bx_cdrom_options *out)
{
  BX_ASSERT (drive == 0);
  bx_options.cdromd = *out;
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
bx_real_sim_c::get_mem_size () {
  return bx_options.memory.megs;
}

int 
bx_real_sim_c::set_mem_size (int megs) {
  bx_options.memory.megs = megs;
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

