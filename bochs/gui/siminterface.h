/*
 * gui/siminterface.h
 * $Id: siminterface.h,v 1.19 2001-06-21 19:27:05 bdenney Exp $
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

#define BX_UI_TEXT 1

//////////////////////////////////////////////////////

typedef enum {
  BXP_NULL = 101,
  BXP_IPS,
  BXP_VGA_UPDATE_INTERVAL,
  BXP_MOUSE_ENABLED,
  BXP_MEM_SIZE,
  BXP_ROM_PATH,
  BXP_ROM_ADDRESS,
  BXP_VGA_ROM_PATH,
  BXP_KBD_SERIAL_DELAY,
  BXP_FLOPPY_CMD_DELAY,
  BXP_FLOPPYA_PATH,
  BXP_FLOPPYA_TYPE,
  BXP_FLOPPYA_STATUS,
  BXP_FLOPPYA,
  BXP_FLOPPYB_PATH,
  BXP_FLOPPYB_TYPE,
  BXP_FLOPPYB_STATUS,
  BXP_FLOPPYB,
  BXP_DISKC_PRESENT,
  BXP_DISKC_PATH,
  BXP_DISKC_CYLINDERS,
  BXP_DISKC_HEADS,
  BXP_DISKC_SPT,
  BXP_DISKC,
  BXP_DISKD_PRESENT,
  BXP_DISKD_PATH,
  BXP_DISKD_CYLINDERS,
  BXP_DISKD_HEADS,
  BXP_DISKD_SPT,
  BXP_DISKD,
  BXP_CDROM_PRESENT,
  BXP_CDROM_PATH,
  BXP_CDROM_INSERTED,
  BXP_CDROMD,
  BXP_PRIVATE_COLORMAP,
  BXP_I440FX_SUPPORT,
  BXP_NEWHARDDRIVESUPPORT,
  BXP_LOG_FILENAME,
  BXP_CMOS_PATH,
  BXP_CMOS_IMAGE,
  BXP_CMOS_TIME0,
  BXP_LOAD32BITOS_WHICH,
  BXP_LOAD32BITOS_PATH,
  BXP_LOAD32BITOS_IOLOG,
  BXP_LOAD32BITOS_INITRD,
  BXP_BOOTDRIVE,
  BXP_MENU_MAIN,
  BXP_MENU_MEMORY,
  BXP_MENU_INTERFACE,
  BXP_MENU_DISK,
  BXP_MENU_SOUND,
  BXP_MENU_MISC,
  BXP_MENU_RUNTIME,
  BXP_SYSTEM_CLOCK_SYNC,
  BXP_MAX_IPS,
  BXP_NE2K_VALID,
  BXP_NE2K_IOADDR,
  BXP_NE2K_IRQ,
  BXP_NE2K_MACADDR,
  BXP_NE2K_ETHMOD,
  BXP_NE2K_ETHDEV,
  BXP_NE2K,
  BXP_SB16_PRESENT,
  BXP_SB16_MIDIFILE,
  BXP_SB16_WAVEFILE,
  BXP_SB16_LOGFILE,
  BXP_SB16_MIDIMODE,
  BXP_SB16_WAVEMODE,
  BXP_SB16_LOGLEVEL,
  BXP_SB16_DMATIMER,
  BXP_SB16,
  BXP_THIS_IS_THE_LAST    // used to determine length of list
} bx_id;

typedef enum {
  BXT_OBJECT = 201,
  BXT_NODE,
  BXT_PARAM,
  BXT_PARAM_NUM,
  BXT_PARAM_BOOL,
  BXT_PARAM_ENUM,
  BXT_PARAM_STRING,
  BXT_LIST
} bx_objtype;

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
protected:
  char *name;
  char *description;
  char *text_format;  // printf format string. %d for ints, %s for strings, etc.
  char *ask_format;  // format string for asking for a new value
  int runtime_param;
  int enabled;
public:
  bx_param_c (bx_id id,
      char *name,
      char *description);
  void set_format (char *format) {text_format = format;}
  char *get_format () {return text_format;}
  void set_ask_format (char *format) {ask_format = format; }
  char *get_ask_format () {return ask_format;}
  void set_runtime_param (int val) { runtime_param = val; }
  char *get_name () { return name; }
  char *get_description () { return description; }
  int get_enabled () { return enabled; }
  void set_enabled (int enabled) { this->enabled = enabled; }
  void reset () {}
  int getint () {return -1;}
#if BX_UI_TEXT
  virtual void text_print (FILE *fp) {}
  virtual int text_ask (FILE *fpin, FILE *fpout) {}
#endif
};

typedef Bit32s (*param_event_handler)(class bx_param_c *, int set, Bit32s val);

class bx_param_num_c : public bx_param_c {
protected:
  Bit32s min, max, val, initial_val;
  param_event_handler handler;
  int base;
public:
  bx_param_num_c (bx_id id,
      char *name,
      char *description,
      Bit32s min, Bit32s max, Bit32s initial_val);
  void reset ();
  void set_handler (param_event_handler handler);
  Bit32s get ();
  void set (Bit32s val);
  void set_base (int base) { this->base = base; }
#if BX_UI_TEXT
  virtual void text_print (FILE *fp);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

class bx_param_bool_c : public bx_param_num_c {
public:
  bx_param_bool_c (bx_id id, 
      char *name,
      char *description,
      Bit32s initial_val);
#if BX_UI_TEXT
  virtual void text_print (FILE *fp);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

class bx_param_enum_c : public bx_param_num_c {
  char **choices;
public:
  bx_param_enum_c (bx_id id, 
      char *name,
      char *description,
      char **choices,
      Bit32s initial_val,
      Bit32s value_base = 0);
#if BX_UI_TEXT
  virtual void text_print (FILE *fp);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

typedef char* (*param_string_event_handler)(class bx_param_string_c *, int set, char *val, int maxlen);

class bx_param_string_c : public bx_param_c {
  int maxsize;
  char *val, *initial_val;
  param_string_event_handler handler;
  bx_param_num_c *options;
  char separator;
public:
  enum {
    BX_RAW_BYTES = 1,
  } bx_string_opt_bits;
  bx_param_string_c (bx_id id,
      char *name,
      char *description,
      char *initial_val,
      int maxsize=-1);
  void reset ();
  void set_handler (param_string_event_handler handler);
  Bit32s get (char *buf, int len);
  char *getptr () {return val; }
  void set (char *buf);
  bx_param_num_c *get_options () { return options; }
  void set_separator (char sep) {separator = sep; }
#if BX_UI_TEXT
  virtual void text_print (FILE *fp);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

class bx_list_c : public bx_param_c {
private:
  // just a list of bx_param_c objects.  size tells current number of
  // objects in the list, and maxsize tells how many list items are
  // allocated in the constructor.
  bx_param_c **list;
  int size, maxsize;
  // options is a bit field whose bits are defined by bx_listopt_bits ORed
  // together.  Options is a bx_param so that if necessary the bx_list could
  // install a handler to cause get/set of options to have side effects.
  bx_param_num_c *options;
  // for a menu, the value of choice before the call to "ask" is default.
  // After ask, choice holds the value that the user chose.  Choice defaults
  // to 1 in the constructor.
  bx_param_num_c *choice;
  // title of the menu or series
  bx_param_string_c *title;
  // if the menu shows a "return to previous menu" type of choice,
  // this controls where that choice will go.
  bx_param_c *parent;
  void init ();
public:
  enum {
    // When a bx_list_c is displayed as a menu, SHOW_PARENT controls whether or
    // not the menu shows "0. Return to previous menu" or not.
    BX_SHOW_PARENT = (1<<0),
    // Some lists are best displayed shown as menus, others as a series of
    // related questions.  
    BX_SERIES_ASK = (1<<1)
  } bx_listopt_bits;
  //bx_list_c (bx_id id, int maxsize);
  bx_list_c (bx_id id, char *name, char *description, bx_param_c **init_list);
  void add (bx_param_c *param);
  bx_param_c *get (int index);
  bx_param_num_c *get_options () { return options; }
  bx_param_num_c *get_choice () { return choice; }
  bx_param_string_c *get_title () { return title; }
  void set_parent (bx_param_c *parent);
#if BX_UI_TEXT
  virtual void text_print (FILE *);
  virtual int text_ask (FILE *fpin, FILE *fpout);
#endif
};

////////////////////////////////////////////////////////////////


#define BX_FLOPPY_NONE   10 // floppy not present
#define BX_FLOPPY_1_2    11 // 1.2M  5.25"
#define BX_FLOPPY_1_44   12 // 1.44M 3.5"
#define BX_FLOPPY_2_88   13 // 2.88M 3.5"
#define BX_FLOPPY_720K   14 // 720K  3.5"
#define BX_FLOPPY_LAST   14 // last legal value of floppy type
#define BX_FLOPPY_GUESS  20 // decide based on image size

extern char *floppy_type_names[];
extern int n_floppy_type_names;
extern char *floppy_status_names[];
extern int n_floppy_status_names;
extern char *floppy_bootdisk_names[];
extern int n_floppy_bootdisk_names;

typedef struct {
  bx_param_string_c *Opath;
  bx_param_enum_c *Otype;
  bx_param_enum_c *Oinitial_status;
  } bx_floppy_options;

typedef struct {
  bx_param_bool_c *Opresent;
  bx_param_string_c *Opath;
  bx_param_num_c *Ocylinders;
  bx_param_num_c *Oheads;
  bx_param_num_c *Ospt;
  } bx_disk_options;

struct bx_cdrom_options
{
  bx_param_bool_c *Opresent;
  bx_param_string_c *Opath;
  bx_param_enum_c *Oinserted;
};


////////////////////////////////////////////////////////////////////

class bx_simulator_interface_c {
public:
  bx_simulator_interface_c ();
  virtual int get_init_done () { return -1; }
  virtual int set_init_done (int n) {return -1;}
  virtual int register_param (bx_id id, bx_param_c *it) {return -1;}
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
  virtual int get_cdrom_options (int drive, bx_cdrom_options *out) {return -1;}
  virtual char *get_floppy_type_name (int type) {return NULL;}
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
