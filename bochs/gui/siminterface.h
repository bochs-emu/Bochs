/*
 * gui/siminterface.h
 * $Id: siminterface.h,v 1.11 2001-06-16 23:08:32 bdenney Exp $
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
  BXP_MEM_SIZE,
  BXP_ROM_PATH,
  BXP_VGA_ROM_PATH
} bx_id;

typedef enum {
  BXT_OBJECT = 201,
  BXT_NODE,
  BXT_PARAM,
  BXT_PARAM_NUM,
  BXT_PARAM_STRING
} bx_objtype;



////////////////////////////////////////////////////////////////////

// Abstract type.

struct bx_any {
  Bit32u type;
  union s {
    int integer;
    int boolean;
    struct string {
      char *val;
      int alloc_len;
    };
    struct list {
      Bit32u size;
      bx_any *array;
    };
  };
};

////////////////////////////////////////////////////////////////////
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
  int getint () {return -1;}
};

typedef Bit32s (*param_any_event_handler)(class bx_param_any_c *, int set, bx_any val);

class bx_param_any_c : public bx_param_c {
  bx_any min, max, val, initial_val;
  param_any_event_handler handler;
public:
  bx_param_any_c (bx_id id,
      char *name,
      char *description,
      bx_any min, bx_any max, bx_any initial_val);
  bx_param_any_c (bx_id id,
      char *name,
      char *description,
      bx_any initial_val);
  void reset ();
  void set_handler (param_any_event_handler handler) { this->handler = handler; }
  bx_any get ();
  void set (bx_any val);
};

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
  void set (Bit32s val);
};

typedef Bit32s (*param_string_event_handler)(class bx_param_string_c *, int set, char *val, int maxlen);

class bx_param_string_c : public bx_param_c {
  int maxsize;
  char *val, *initial_val;
  param_string_event_handler handler;
public:
  bx_param_string_c (bx_id id,
      char *name,
      char *description,
      int maxsize,
      char *initial_val);
  void reset ();
  void set_handler (param_string_event_handler handler) { this->handler = handler; }
  Bit32s get (char *buf, int len);
  char *getptr () {return val; }
  void set (char *buf);
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
////////////////////////////////////////////////////////////////////

class bx_simulator_interface_c {
  int init_done;
public:
  bx_simulator_interface_c ();
  int get_init_done () { return init_done; }
  int set_init_done (int n) { init_done = n; return 0;}

  virtual bx_param_c *get_param (bx_id id) {return NULL;}
  virtual bx_param_num_c *get_param_num (bx_id id) {return NULL;}
  virtual bx_param_string_c *get_param_string (bx_id id) {return NULL;}
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
