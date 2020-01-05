/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010-2018  The Bochs Project
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

#ifndef BX_PARAM_TREE_H
#define BX_PARAM_TREE_H

////////////////////////////////////////////////////////////////////
// parameter classes: bx_param_c and family
////////////////////////////////////////////////////////////////////
//
// All variables that can be configured through the CI are declared as
// "parameters" or objects of type bx_param_*.  There is a bx_param_*
// class for each type of data that the user would need to see and
// edit, e.g. integer, boolean, enum, string, filename, or list of
// other parameters.  The purpose of the bx_param_* class, in addition
// to storing the parameter's value, is to hold the name, description,
// and constraints on the value.  The bx_param_* class should hold
// everything that the CI would need to display the value and allow
// the user to modify it.  For integer parameters, the minimum and
// maximum allowed value can be defined, and the base in which it
// should be displayed and interpreted.  For enums, the
// bx_param_enum_c structure includes the list of values which the
// parameter can have.
//
// Also, some parameter classes support get/set callback functions to
// allow arbitrary code to be executed when the parameter is get/set.
// An example of where this is useful: if you disable the NE2K card,
// the set() handler for that parameter can tell the user interface
// that the NE2K's irq, I/O address, and mac address should be
// disabled (greyed out, hidden, or made inaccessible).  The get/set
// methods can also check if the set() value is acceptable using
// whatever means and override it.
//
// The parameter concept is similar to the use of parameters in JavaBeans.

// list of possible types for bx_param_c and descendant objects
typedef enum {
  BXT_OBJECT = 201,
  BXT_PARAM,
  BXT_PARAM_NUM,
  BXT_PARAM_BOOL,
  BXT_PARAM_ENUM,
  BXT_PARAM_STRING,
  BXT_PARAM_BYTESTRING,
  BXT_PARAM_DATA,
  BXT_PARAM_FILEDATA,
  BXT_LIST
} bx_objtype;

class bx_object_c;
class bx_param_c;
class bx_param_num_c;
class bx_param_enum_c;
class bx_param_bool_c;
class bx_param_string_c;
class bx_param_filename_c;
class bx_list_c;

class BOCHSAPI bx_object_c {
private:
  Bit32u id;
  bx_objtype type;
protected:
  void set_type(bx_objtype _type) { type = _type; }
public:
  bx_object_c(Bit32u _id): id(_id), type(BXT_OBJECT) {}
  virtual ~bx_object_c() {}
  Bit32u get_id() const { return id; }
  Bit8u get_type() const { return type; }
};

#define BASE_DEC 10
#define BASE_HEX 16
#define BASE_FLOAT 32
#define BASE_DOUBLE 64

class BOCHSAPI bx_param_c : public bx_object_c {
  BOCHSAPI_CYGONLY static const char *default_text_format;
protected:
  bx_list_c *parent;
  char *name;
  char *description;
  char *label; // label string for text menus and gui dialogs
  const char *text_format;  // printf format string. %d for ints, %s for strings, etc.
  const char *long_text_format;  // printf format string. %d for ints, %s for strings, etc.
  char *ask_format;  // format string for asking for a new value
  char *group_name;  // name of the group the param belongs to
  int runtime_param;
  int enabled;
  Bit32u options;
  // The dependent_list is initialized to NULL.  If dependent_list is modified
  // to point to a bx_list_c of other parameters, the set() method of the
  // parameter type will enable those parameters when the enable condition is
  // true, and disable them it is false.
  bx_list_c *dependent_list;
  void *device;
public:
  enum {
    // If set, this parameter is available in CI only. In bochsrc, it is set
    // indirectly from one or more other options (e.g. cpu count)
    CI_ONLY = (1<<31)
  } bx_param_opt_bits;

  bx_param_c(Bit32u id, const char *name, const char *description);
  bx_param_c(Bit32u id, const char *name, const char *label, const char *description);
  virtual ~bx_param_c();

  virtual void reset() {}

  const char *get_name() const { return name; }
  bx_param_c *get_parent() { return (bx_param_c *) parent; }

  int get_param_path(char *path_out, int maxlen);

  void set_format(const char *format) {text_format = format;}
  const char *get_format() const {return text_format;}

  void set_long_format(const char *format) {long_text_format = format;}
  const char *get_long_format() const {return long_text_format;}

  void set_ask_format(const char *format);
  const char *get_ask_format() const {return ask_format;}

  void set_label(const char *text);
  const char *get_label() const {return label;}

  void set_description(const char *text);
  const char *get_description() const { return description; }

  virtual void set_runtime_param(int val) { runtime_param = val; }
  int get_runtime_param() const { return runtime_param; }

  void set_group(const char *group);
  const char *get_group() const {return group_name;}

  int get_enabled() const { return enabled; }
  virtual void set_enabled(int _enabled) { enabled = _enabled; }

  static const char* set_default_format(const char *f);
  static const char *get_default_format() { return default_text_format; }

  bx_list_c *get_dependent_list() { return dependent_list; }

  void set_options(Bit32u _options) { options = _options; }
  Bit32u get_options() const { return options; }

  void set_device_param(void *dev) { device = dev; }
  void *get_device_param() { return device; }

#if BX_USE_TEXTCONFIG
  virtual void text_print() {}
  virtual int text_ask() { return -1; }
#endif

  virtual int parse_param(const char *value) { return -1; }

  virtual void dump_param(FILE *fp) {}
  virtual int dump_param(char *buf, int buflen, bx_bool dquotes = false) { return 0; }
};

typedef Bit64s (*param_event_handler)(class bx_param_c *, int set, Bit64s val);
typedef Bit64s (*param_save_handler)(void *devptr, class bx_param_c *);
typedef void (*param_restore_handler)(void *devptr, class bx_param_c *, Bit64s val);
typedef int (*param_enable_handler)(class bx_param_c *, int en);

class BOCHSAPI bx_param_num_c : public bx_param_c {
  BOCHSAPI_CYGONLY static Bit32u default_base;
  void update_dependents();
protected:
  Bit64s min, max, initial_val;
  union _uval_ {
    Bit64s number;   // used by bx_param_num_c
    Bit64s *p64bit;  // used by bx_shadow_num_c
    Bit32s *p32bit;  // used by bx_shadow_num_c
    Bit16s *p16bit;  // used by bx_shadow_num_c
    Bit8s  *p8bit;   // used by bx_shadow_num_c
    float  *pfloat;  // used by bx_shadow_num_c
    double *pdouble; // used by bx_shadow_num_c
    bx_bool *pbool;  // used by bx_shadow_bool_c
  } val;
  param_event_handler handler;
  void *sr_devptr;
  param_save_handler save_handler;
  param_restore_handler restore_handler;
  param_enable_handler enable_handler;
  int base;
  bx_bool is_shadow;
public:
  enum {
    // When a bx_param_num_c is displayed in dialog, USE_SPIN_CONTROL controls
    // whether a spin control should be used instead of a simple text control.
    USE_SPIN_CONTROL = (1<<0)
  } bx_numopt_bits;
  bx_param_num_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      Bit64s min, Bit64s max, Bit64s initial_val,
      bx_bool is_shadow = 0);
  virtual void reset() { val.number = initial_val; }
  void set_handler(param_event_handler handler);
  void set_sr_handlers(void *devptr, param_save_handler save, param_restore_handler restore);
  void set_enable_handler(param_enable_handler handler) { enable_handler = handler; }
  void set_dependent_list(bx_list_c *l);
  virtual void set_enabled(int enabled);
  virtual Bit32s get() { return (Bit32s) get64(); }
  virtual Bit64s get64();
  virtual void set(Bit64s val);
  void set_base(int _base) { base = _base; }
  void set_initial_val(Bit64s initial_val);
  int get_base() const { return base; }
  void set_range(Bit64u min, Bit64u max);
  Bit64s get_min() const { return min; }
  Bit64s get_max() const { return max; }
  static Bit32u set_default_base(Bit32u val);
  static Bit32u get_default_base() { return default_base; }
#if BX_USE_TEXTCONFIG
  virtual void text_print();
  virtual int text_ask();
#endif
  virtual int parse_param(const char *value);
  virtual void dump_param(FILE *fp);
  virtual int dump_param(char *buf, int buflen, bx_bool dquotes = false);
};

// a bx_shadow_num_c is like a bx_param_num_c except that it doesn't
// store the actual value with its data. Instead, it uses val.p32bit
// to keep a pointer to the actual data.  This is used to register
// existing variables as parameters, without having to access it via
// set/get methods.
class BOCHSAPI bx_shadow_num_c : public bx_param_num_c {
  Bit8u varsize;   // must be 64, 32, 16, or 8
  Bit8u lowbit;   // range of bits associated with this param
  Bit64u mask;     // mask is ANDed with value before it is returned from get
public:
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit64s *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 63,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit64u *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 63,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit32s *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 31,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit32u *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 31,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit16s *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 15,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit16u *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 15,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit8s *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 7,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      Bit8u *ptr_to_real_val,
      int base = BASE_DEC,
      Bit8u highbit = 7,
      Bit8u lowbit = 0);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      double *ptr_to_real_val);
  bx_shadow_num_c(bx_param_c *parent,
      const char *name,
      float *ptr_to_real_val);
  virtual Bit64s get64();
  virtual void set(Bit64s val);
  virtual void reset();
};

class BOCHSAPI bx_param_bool_c : public bx_param_num_c {
  // many boolean variables are used to enable/disable modules.  In the
  // user interface, the enable variable should enable/disable all the
  // other parameters associated with that module.
public:
  bx_param_bool_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      Bit64s initial_val,
      bx_bool is_shadow = 0);
#if BX_USE_TEXTCONFIG
  virtual void text_print();
  virtual int text_ask();
#endif
  virtual int parse_param(const char *value);
  virtual void dump_param(FILE *fp);
  virtual int dump_param(char *buf, int buflen, bx_bool dquotes = false);
};

// a bx_shadow_bool_c is a shadow param based on bx_param_bool_c.
class BOCHSAPI bx_shadow_bool_c : public bx_param_bool_c {
  // each bit of a bitfield can be a separate value.  bitnum tells which
  // bit is used.  get/set will only modify that bit.
  Bit8u bitnum;
public:
  bx_shadow_bool_c(bx_param_c *parent,
      const char *name,
      const char *label,
      bx_bool *ptr_to_real_val,
      Bit8u bitnum = 0);
  bx_shadow_bool_c(bx_param_c *parent,
      const char *name,
      bx_bool *ptr_to_real_val,
      Bit8u bitnum = 0);
  virtual Bit64s get64();
  virtual void set(Bit64s val);
};


class BOCHSAPI bx_param_enum_c : public bx_param_num_c {
  const char **choices;
  Bit64u *deps_bitmap;
  void update_dependents();
public:
  bx_param_enum_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      const char **choices,
      Bit64s initial_val,
      Bit64s value_base = 0);
  virtual ~bx_param_enum_c();
  const char *get_choice(int n) { return choices[n]; }
  const char *get_selected() { return choices[val.number - min]; }
  int find_by_name(const char *s);
  virtual void set(Bit64s val);
  bx_bool set_by_name(const char *s);
  void set_dependent_list(bx_list_c *l, bx_bool enable_all);
  void set_dependent_bitmap(Bit64s value, Bit64u bitmap);
  Bit64u get_dependent_bitmap(Bit64s value);
  virtual void set_enabled(int enabled);
#if BX_USE_TEXTCONFIG
  virtual void text_print();
  virtual int text_ask();
#endif
  virtual int parse_param(const char *value);
  virtual void dump_param(FILE *fp);
  virtual int dump_param(char *buf, int buflen, bx_bool dquotes = false);
};

typedef const char* (*param_string_event_handler)(class bx_param_string_c *,
                     int set, const char *oldval, const char *newval, int maxlen);

class BOCHSAPI bx_param_string_c : public bx_param_c {
protected:
  int maxsize;
  char *val, *initial_val;
  param_string_event_handler handler;
  param_enable_handler enable_handler;
  void update_dependents();
public:
  enum {
    IS_FILENAME = 1,       // 1=yes it's a filename, 0=not a filename.
                           // Some guis have a file browser. This
                           // bit suggests that they use it.
    SAVE_FILE_DIALOG = 2,  // Use save dialog opposed to open file dialog
    SELECT_FOLDER_DLG = 4  // Use folder selection dialog
  } bx_string_opt_bits;
  bx_param_string_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      const char *initial_val,
      int maxsize=-1);
  virtual ~bx_param_string_c();
  virtual void reset();
  void set_handler(param_string_event_handler handler);
  void set_enable_handler(param_enable_handler handler);
  virtual void set_enabled(int enabled);
  void set_dependent_list(bx_list_c *l);
  Bit32s get(char *buf, int len);
  char *getptr() {return val; }
  const char *getptr() const {return val; }
  void set(const char *buf);
  bx_bool equals(const char *buf) const;
  int get_maxsize() const {return maxsize; }
  void set_initial_val(const char *buf);
  bx_bool isempty() const;
#if BX_USE_TEXTCONFIG
  virtual void text_print();
  virtual int text_ask();
#endif
  virtual int parse_param(const char *value);
  virtual void dump_param(FILE *fp);
  virtual int dump_param(char *buf, int buflen, bx_bool dquotes = false);
};

class BOCHSAPI bx_param_bytestring_c : public bx_param_string_c {
  char separator;
public:
  bx_param_bytestring_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      const char *initial_val,
      int maxsize) : bx_param_string_c(parent, name, label, description, initial_val, maxsize)
  {
    set_type(BXT_PARAM_BYTESTRING);
  }

  void set_separator(char sep) {separator = sep; }
  char get_separator() const {return separator; }

  Bit32s get(char *buf, int len);
  void set(const char *buf);
  bx_bool equals(const char *buf) const;
  void set_initial_val(const char *buf);
  bx_bool isempty() const;

#if BX_USE_TEXTCONFIG
  virtual int text_ask();
#endif

  virtual int parse_param(const char *value);
  virtual int dump_param(char *buf, int buflen, bx_bool dquotes = false);
};

// Declare a filename class.  It is identical to a string, except that
// it initializes the options differently.  This is just a shortcut
// for declaring a string param and setting the options with IS_FILENAME.
class BOCHSAPI bx_param_filename_c : public bx_param_string_c {
const char *ext;
public:
  bx_param_filename_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      const char *initial_val,
      int maxsize=-1);
  const char *get_extension() const {return ext;}
  void set_extension(const char *newext) {ext = newext;}
};

class BOCHSAPI bx_shadow_data_c : public bx_param_c {
  Bit32u data_size;
  Bit8u *data_ptr;
  bx_bool is_text;
public:
  bx_shadow_data_c(bx_param_c *parent,
      const char *name,
      Bit8u *ptr_to_data,
      Bit32u data_size, bx_bool is_text=0);
  Bit8u *getptr() {return data_ptr;}
  const Bit8u *getptr() const {return data_ptr;}
  Bit32u get_size() const {return data_size;}
  bx_bool is_text_format() const {return is_text;}
  Bit8u get(Bit32u index);
  void set(Bit32u index, Bit8u value);
};

typedef void (*filedata_save_handler)(void *devptr, FILE *save_fp);
typedef void (*filedata_restore_handler)(void *devptr, FILE *save_fp);

class BOCHSAPI bx_shadow_filedata_c : public bx_param_c {
protected:
  FILE **scratch_fpp;       // Point to scratch file used for backing store
  void *sr_devptr;
  filedata_save_handler    save_handler;
  filedata_restore_handler restore_handler;

public:
  bx_shadow_filedata_c(bx_param_c *parent,
      const char *name, FILE **scratch_file_ptr_ptr);
  void set_sr_handlers(void *devptr, filedata_save_handler save, filedata_restore_handler restore);
  FILE **get_fpp() {return scratch_fpp;}
  void save(FILE *save_file);
  void restore(FILE *save_file);
};

typedef struct _bx_listitem_t {
  bx_param_c *param;
  struct _bx_listitem_t *next;
} bx_listitem_t;

typedef void (*list_restore_handler)(void *devptr, class bx_list_c *);

class BOCHSAPI bx_list_c : public bx_param_c {
protected:
  // chained list of bx_listitem_t
  bx_listitem_t *list;
  int size;
  // for a menu, the value of choice before the call to "ask" is default.
  // After ask, choice holds the value that the user chose. Choice defaults
  // to 1 in the constructor.
  Bit32u choice; // type Bit32u is compatible with ask_uint
  // title of the menu or series
  char *title;
  void init(const char *list_title);
  // save / restore support
  void *sr_devptr;
  list_restore_handler restore_handler;
public:
  enum {
    // When a bx_list_c is displayed as a menu, SHOW_PARENT controls whether or
    // not the menu shows a "Return to parent menu" choice or not.
    SHOW_PARENT = (1<<0),
    // Some lists are best displayed shown as menus, others as a series of
    // related questions.  This bit suggests to the CI that the series of
    // questions format is preferred.
    SERIES_ASK = (1<<1),
    // When a bx_list_c is displayed in a dialog, USE_TAB_WINDOW suggests
    // to the CI that each item in the list should be shown as a separate
    // tab.  This would be most appropriate when each item is another list
    // of parameters.
    USE_TAB_WINDOW = (1<<2),
    // When a bx_list_c is displayed in a dialog, the list name is used as the
    // label of the group box if USE_BOX_TITLE is set. This is only necessary if
    // more than one list appears in a dialog box.
    USE_BOX_TITLE = (1<<3),
    // When a bx_list_c is displayed as a menu, SHOW_GROUP_NAME controls whether
    // or not the name of group the item belongs to is added to the name of the
    // item (used in the runtime menu).
    SHOW_GROUP_NAME = (1<<4),
    // When a bx_list_c is displayed in a dialog, USE_SCROLL_WINDOW suggests
    // to the CI that the list items should be displayed in a scrollable dialog
    // window. Large lists can make the dialog unusable and using this flag
    // can force the CI to limit the dialog height with all items accessible.
    USE_SCROLL_WINDOW = (1<<5)
  } bx_listopt_bits;
  bx_list_c(bx_param_c *parent);
  bx_list_c(bx_param_c *parent, const char *name);
  bx_list_c(bx_param_c *parent, const char *name, const char *title);
  bx_list_c(bx_param_c *parent, const char *name, const char *title, bx_param_c **init_list);
  virtual ~bx_list_c();
  bx_list_c *clone();
  void add(bx_param_c *param);
  bx_param_c *get(int index);
  bx_param_c *get_by_name(const char *name);
  int get_size() const { return size; }
  int get_choice() const { return choice; }
  void set_choice(int new_choice) { choice = new_choice; }
  char *get_title() { return title; }
  void set_parent(bx_param_c *newparent);
  bx_param_c *get_parent() { return parent; }
  virtual void reset();
  virtual void clear();
  virtual void remove(const char *name);
  virtual void set_runtime_param(int val);
  void set_restore_handler(void *devptr, list_restore_handler restore);
  void restore();
#if BX_USE_TEXTCONFIG
  virtual void text_print();
  virtual int text_ask();
#endif
};

#endif
