/*
 * gui/siminterface.h
 * $Id: siminterface.h,v 1.9 2001-06-15 23:52:34 bdenney Exp $
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

#define BX_PATHNAME_LEN 512

#define BX_FLOPPY_NONE   10 // floppy not present
#define BX_FLOPPY_1_2    11 // 1.2M  5.25"
#define BX_FLOPPY_1_44   12 // 1.44M 3.5"
#define BX_FLOPPY_2_88   13 // 2.88M 3.5"
#define BX_FLOPPY_720K   14 // 720K  3.5"
#define BX_FLOPPY_LAST   14 // last legal value of floppy type
#define BX_FLOPPY_GUESS  20 // decide based on image size

extern char *floppy_type_names[];
extern int n_floppy_type_names;

typedef struct {
  char path[BX_PATHNAME_LEN];
  unsigned type;
  unsigned initial_status;
  } bx_floppy_options;

typedef struct {
  int present;
  char path[BX_PATHNAME_LEN];
  unsigned int cylinders;
  unsigned int heads;
  unsigned int spt;
  } bx_disk_options;

struct bx_cdrom_options
{
  int present;
  char dev[BX_PATHNAME_LEN];
  int inserted;
};

/* what do I want to know about an integer parameter?
 * id number (comes from enum)
 * string name
 * description
 * minimum and maximum allowed value
 * default value
 * what about behavior?  getValue method and setValue method?
 */

typedef enum {
  BXP_IPS = 101,
  BXP_VGA_UPDATE_INTERVAL,
  BXP_MOUSE_ENABLED,
  BXP_MEM_SIZE
} bx_id;

typedef enum {
  BXT_OBJECT,
  BXT_NODE,
  BXT_PARAM,
  BXT_PARAM_NUM
} bx_objtype;

class bx_object_c {
private:
  bx_id id;
  Bit8u type;
protected:
  void set_type (Bit8u type);
public:
  bx_object_c (bx_id id);
  bx_id get_id () { return id; }
  Bit8u get_type () { return type; }
};

class bx_param_c : public bx_object_c {
private:
  char *name;
  char *description;
public:
  bx_param_c (bx_id id,
      char *name,
      char *description);
  char *get_name () { return name; }
  char *get_description () { return description; }
  void reset () {}
};

// make it only handle ints for now
typedef Bit32s (*param_event_handler)(class bx_param_c *, int set, Bit32s val);

class bx_param_num_c : public bx_param_c {
  Bit32s min, max, val, initial_val;
  param_event_handler handler;
public:
  bx_param_num_c (bx_id id,
      char *name,
      char *description,
      Bit32s min, Bit32s max, Bit32s initial_val);
  void reset ();
  void set_handler (param_event_handler handler) { this->handler = handler; }
  Bit32s get ();
  Bit32s set (Bit32s val);
};

class bx_node_c : public bx_object_c {
private:
  bx_object_c *car, *cdr;
public:
  bx_node_c (bx_id id);
  bx_node_c (bx_id id, bx_object_c *car, bx_object_c *cdr);
  bx_object_c *setcar (bx_object_c *ptr);
  bx_object_c *setcdr (bx_object_c *ptr);
  bx_object_c *getcar () { return car; }
  bx_object_c *getcdr () { return cdr; }
};

class bx_simulator_interface_c {
  int init_done;
public:
  bx_simulator_interface_c ();
  int get_init_done () { return init_done; }
  int set_init_done (int n) { init_done = n; return 0;}

  bx_param_num_c *ips;
  bx_param_num_c *vga_update_interval;
  bx_param_num_c *mouse_enabled;
  bx_param_num_c *memsize;
  virtual int get_n_log_modules () {return -1;}
  virtual char *get_prefix (int mod) {return 0;}
  virtual int get_log_action (int mod, int level) {return -1;}
  virtual void set_log_action (int mod, int level, int action) {}
  virtual char *get_action_name (int action) {return 0;}
  virtual char *get_log_level_name (int level) {return 0;}
  virtual int get_max_log_level () {return -1;}
  virtual void quit_sim (int clean) {}
  virtual int get_default_rc (char *path, int len) {return -1;}
  virtual int read_rc (char *path) {return -1;}
  virtual int write_rc (char *rc, int overwrite) {return -1;}
  virtual int get_log_file (char *path, int len) {return -1;}
  virtual int set_log_file (char *path) {return -1;}
  virtual int get_floppy_options (int drive, bx_floppy_options *out) {return -1;}
  virtual int set_floppy_options (int drive, bx_floppy_options *in) {return -1;}
  virtual int get_disk_options (int drive, bx_disk_options *out) {return -1;}
  virtual int set_disk_options (int drive, bx_disk_options *in) {return -1;}
  virtual int get_cdrom_options (int drive, bx_cdrom_options *out) {return -1;}
  virtual int set_cdrom_options (int drive, bx_cdrom_options *out) {return -1;}
  virtual int get_newhd_support () {return -1;}
  virtual void set_newhd_support (int en) {}
  virtual char *get_floppy_type_name (int type) {return NULL;}
  virtual int get_boot_hard_disk () {return -1;}
  virtual int set_boot_hard_disk (int val) {return -1;}
  virtual int get_rom_path (char *buf, int len) {return -1;}
  virtual int set_rom_path (char *path) {return -1;}
  virtual int get_vga_path (char *buf, int len) {return -1;}
  virtual int set_vga_path (char *path) {return -1;}
  virtual int get_rom_address () {return -1;}
  virtual int set_rom_address (int addr) {return -1;}
  virtual int get_private_colormap () { return -1; }
  virtual void set_private_colormap (int en) {}
  typedef int (*sim_interface_callback_t)(int code);
  virtual void set_notify_callback (sim_interface_callback_t func) {}
  virtual int notify_return (int retcode) {return -1;}
  // methods marked LOCAL should only be called by the simulator, not
  // from the control panel.
#define NOTIFY_CODE_LOGMSG  0x101
  virtual int LOCAL_notify (int code) {return -1;}
  virtual int LOCAL_log_msg (char *prefix, int level, char *msg) {return -1;}
  virtual int log_msg_2 (char *prefix, int *level, char *msg, int len) {return -1;}
};

extern bx_simulator_interface_c *SIM;

extern void init_siminterface ();
