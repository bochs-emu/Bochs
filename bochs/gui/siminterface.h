/*
 * gui/siminterface.h
 * $Id: siminterface.h,v 1.1 2001-06-08 07:20:07 bdenney Exp $
 *
 * Interface to the simulator, currently only used by control.cc.
 * The base class bx_simulator_interface_c, contains only virtual functions
 * and it defines the interface that control.cc is allowed to use.
 * In siminterface.cc, a class called bx_real_sim_c is defined with
 * bx_simulator_interface_c as its parent class.  Bx_real_sim_c 
 * implements each of the functions.  The separation into parent class
 * and child class leaves the possibility of making a different child
 * class that talks to the simulator in a different way (networking
 * for example).
 *
 */

class bx_simulator_interface_c {
public:
  bx_simulator_interface_c ();
  virtual void get_floppy_path (int drive, char *buffer, int size) {}
  virtual int get_floppy_present (int drive) {return -1;}
  virtual void set_floppy_options (int drive, int present, char *path) {}
  virtual int getips () {return -1;}
  virtual void setips (int ips) {}
  virtual int get_n_log_modules () {return -1;}
  virtual char *get_prefix (int mod) {return 0;}
  virtual int get_log_action (int mod, int level) {return -1;}
  virtual void set_log_action (int mod, int level, int action) {}
  virtual char *get_action_name (int action) {return 0;}
  virtual char *get_log_level_name (int level) {return 0;}
  virtual int get_max_log_level () {return -1;}
  virtual void quit_sim () {}
};

extern bx_simulator_interface_c *SIM;

extern void init_siminterface ();
