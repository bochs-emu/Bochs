/*
 * gui/siminterface.cc
 * $Id: siminterface.cc,v 1.3 2001-06-08 18:02:54 bdenney Exp $
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
  void get_floppy_path (int drive, char *buffer, int size);
  int get_floppy_present (int drive);
  void set_floppy_options (int drive, int present, char *path);
  int getips ();
  void setips (int ips);
  int get_n_log_modules ();
  char *get_prefix (int mod);
  int get_log_action (int mod, int level);
  void set_log_action (int mod, int level, int action);
  char *get_action_name (int action);
  char *get_log_level_name (int level);
  int get_max_log_level ();
  void quit_sim ();
};

void init_siminterface ()
{
  fprintf (stderr, "INIT Interface\n");
  if (SIM == NULL) {
    BX_INFO (("init_siminterface"));
    SIM = new bx_real_sim_c();
  }
}

bx_simulator_interface_c::bx_simulator_interface_c ()
{
}

void 
bx_real_sim_c::get_floppy_path (int drive, char *buffer, int size)
{
  strncpy (buffer, 
     (drive==0)? bx_options.floppya.path : bx_options.floppyb.path,
     size);
}

int 
bx_real_sim_c::get_floppy_present (int drive)
{
  BX_ASSERT(0);
}

void 
bx_real_sim_c::set_floppy_options (int drive, int present, char *path)
{
  bx_devices.floppy->set_media_status (drive, present);
  strncpy (
    (drive==0)? bx_options.floppya.path : bx_options.floppyb.path,
    path,
    BX_PATHNAME_LEN);
}

int 
bx_real_sim_c::getips ()
{
  return bx_options.ips;
}

void 
bx_real_sim_c::setips (int ips)
{
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
bx_real_sim_c::quit_sim () {
  BX_PANIC (("Quit simulation command"));
  ::exit (0);
}
