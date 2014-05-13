/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010-2014  The Bochs Project
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

#include "bochs.h"
#include "paramtree.h"

/////////////////////////////////////////////////////////////////////////
// define methods of bx_param_* and family
/////////////////////////////////////////////////////////////////////////

extern bx_simulator_interface_c *SIM;
extern logfunctions *siminterface_log;
extern bx_list_c *root_param;
#define LOG_THIS siminterface_log->

const char* bx_param_c::default_text_format = NULL;

bx_param_c::bx_param_c(Bit32u id, const char *param_name, const char *param_desc)
  : bx_object_c(id),
    parent(NULL),
    description(NULL),
    label(NULL),
    ask_format(NULL),
    group_name(NULL)
{
  set_type(BXT_PARAM);
  this->name = new char[strlen(param_name)+1];
  strcpy(this->name, param_name);
  set_description(param_desc);
  this->text_format = default_text_format;
  this->long_text_format = default_text_format;
  this->runtime_param = 0;
  this->enabled = 1;
  this->options = 0;
  // dependent_list must be initialized before the set(),
  // because set calls update_dependents().
  dependent_list = NULL;
}

bx_param_c::bx_param_c(Bit32u id, const char *param_name, const char *param_label, const char *param_desc)
  : bx_object_c(id),
    parent(NULL),
    description(NULL),
    label(NULL),
    ask_format(NULL),
    group_name(NULL)
{
  set_type(BXT_PARAM);
  this->name = new char[strlen(param_name)+1];
  strcpy(this->name, param_name);
  set_description(param_desc);
  set_label(param_label);
  this->text_format = default_text_format;
  this->long_text_format = default_text_format;
  this->runtime_param = 0;
  this->enabled = 1;
  this->options = 0;
  // dependent_list must be initialized before the set(),
  // because set calls update_dependents().
  dependent_list = NULL;
}

bx_param_c::~bx_param_c()
{
  delete [] name;
  delete [] label;
  delete [] description;
  delete [] ask_format;
  delete [] group_name;
  if (dependent_list) delete dependent_list;
}

void bx_param_c::set_description(const char *text)
{
  delete [] this->description;
  if (text) {
    this->description = new char[strlen(text)+1];
    strcpy(this->description, text);
  } else {
    this->description = NULL;
  }
}

void bx_param_c::set_label(const char *text)
{
  delete [] label;
  if (text) {
    label = new char[strlen(text)+1];
    strcpy(label, text);
  } else {
    label = NULL;
  }
}

void bx_param_c::set_ask_format(const char *format)
{
  delete [] ask_format;
  if (format) {
    ask_format = new char[strlen(format)+1];
    strcpy(ask_format, format);
  } else {
    ask_format = NULL;
  }
}

void bx_param_c::set_group(const char *group)
{
  delete [] group_name;
  if (group) {
    group_name = new char[strlen(group)+1];
    strcpy(group_name, group);
  } else {
    group_name = NULL;
  }
}

int bx_param_c::get_param_path(char *path_out, int maxlen)
{
  if ((get_parent() == NULL) || (get_parent() == root_param)) {
    // Start with an empty string.
    // Never print the name of the root param.
    path_out[0] = 0;
  } else {
    // build path of the parent, add a period, add path of this node
    if (get_parent()->get_param_path(path_out, maxlen) > 0) {
      strncat(path_out, ".", maxlen);
    }
  }
  strncat(path_out, name, maxlen);
  return strlen(path_out);
}

const char* bx_param_c::set_default_format(const char *f)
{
  const char *old = default_text_format;
  default_text_format = f;
  return old;
}

bx_param_num_c::bx_param_num_c(bx_param_c *parent,
    const char *name,
    const char *label,
    const char *description,
    Bit64s min, Bit64s max, Bit64s initial_val,
    bx_bool is_shadow)
  : bx_param_c(SIM->gen_param_id(), name, label, description)
{
  set_type(BXT_PARAM_NUM);
  this->min = min;
  this->max = max;
  this->initial_val = initial_val;
  this->val.number = initial_val;
  this->handler = NULL;
  this->save_handler = NULL;
  this->restore_handler = NULL;
  this->enable_handler = NULL;
  this->base = default_base;
  this->is_shadow = is_shadow;
  if (!is_shadow) {
    set(initial_val);
  }
  if (parent) {
    BX_ASSERT(parent->get_type() == BXT_LIST);
    this->parent = (bx_list_c *)parent;
    this->parent->add(this);
  }
}

Bit32u bx_param_num_c::default_base = BASE_DEC;

Bit32u bx_param_num_c::set_default_base(Bit32u val)
{
  Bit32u old = default_base;
  default_base = val;
  return old;
}

void bx_param_num_c::set_handler(param_event_handler handler)
{
  this->handler = handler;
  // now that there's a handler, call set once to run the handler immediately
  //set (get ());
}

void bx_param_num_c::set_sr_handlers(void *devptr, param_save_handler save, param_restore_handler restore)
{
  sr_devptr = devptr;
  save_handler = save;
  restore_handler = restore;
}

void bx_param_num_c::set_dependent_list(bx_list_c *l)
{
  dependent_list = l;
  update_dependents();
}

Bit64s bx_param_num_c::get64()
{
  if (save_handler) {
    return (*save_handler)(sr_devptr, this);
  }
  if (handler) {
    // the handler can decide what value to return and/or do some side effect
    return (*handler)(this, 0, val.number);
  } else {
    // just return the value
    return val.number;
  }
}

void bx_param_num_c::set(Bit64s newval)
{
  if (handler) {
    // the handler can override the new value and/or perform some side effect
    val.number = newval;
    (*handler)(this, 1, newval);
  } else {
    // just set the value.  This code does not check max/min.
    val.number = newval;
  }
  if (restore_handler) {
    val.number = newval;
    (*restore_handler)(sr_devptr, this, newval);
  }
  if ((val.number < min || val.number > max) && (Bit64u)max != BX_MAX_BIT64U)
    BX_PANIC(("numerical parameter '%s' was set to " FMT_LL "d, which is out of range " FMT_LL "d to " FMT_LL "d", get_name (), val.number, min, max));
  if (dependent_list != NULL) update_dependents();
}

void bx_param_num_c::set_range(Bit64u min, Bit64u max)
{
  this->min = min;
  this->max = max;
}

void bx_param_num_c::set_initial_val(Bit64s initial_val)
{
  this->val.number = this->initial_val = initial_val;
}

void bx_param_num_c::update_dependents()
{
  if (dependent_list) {
    int en = val.number && enabled;
    for (int i=0; i<dependent_list->get_size(); i++) {
      bx_param_c *param = dependent_list->get(i);
      if (param != this)
        param->set_enabled(en);
    }
  }
}

void bx_param_num_c::set_enabled(int en)
{
  // The enable handler may wish to allow/disallow the action
  if (enable_handler) {
    en = (*enable_handler)(this, en);
  }
  bx_param_c::set_enabled(en);
  update_dependents();
}

// Signed 64 bit
bx_shadow_num_c::bx_shadow_num_c(bx_param_c *parent,
    const char *name,
    Bit64s *ptr_to_real_val,
    int base,
    Bit8u highbit,
    Bit8u lowbit)
: bx_param_num_c(parent, name, NULL, NULL, BX_MIN_BIT64S, BX_MAX_BIT64S, *ptr_to_real_val, 1)
{
  this->varsize = 64;
  this->lowbit = lowbit;
  this->mask = ((BX_MAX_BIT64S >> (63 - (highbit - lowbit))) << lowbit);
  val.p64bit = ptr_to_real_val;
  if (base == BASE_HEX) {
    this->base = base;
    this->text_format = "0x"FMT_LL"x";
  }
}

// Unsigned 64 bit
bx_shadow_num_c::bx_shadow_num_c(bx_param_c *parent,
    const char *name,
    Bit64u *ptr_to_real_val,
    int base,
    Bit8u highbit,
    Bit8u lowbit)
: bx_param_num_c(parent, name, NULL, NULL, BX_MIN_BIT64U, BX_MAX_BIT64U, *ptr_to_real_val, 1)
{
  this->varsize = 64;
  this->lowbit = lowbit;
  this->mask = ((BX_MAX_BIT64U >> (63 - (highbit - lowbit))) << lowbit);
  val.p64bit = (Bit64s*) ptr_to_real_val;
  if (base == BASE_HEX) {
    this->base = base;
    this->text_format = "0x"FMT_LL"x";
  }
}

// Signed 32 bit
bx_shadow_num_c::bx_shadow_num_c(bx_param_c *parent,
    const char *name,
    Bit32s *ptr_to_real_val,
    int base,
    Bit8u highbit,
    Bit8u lowbit)
: bx_param_num_c(parent, name, NULL, NULL, BX_MIN_BIT32S, BX_MAX_BIT32S, *ptr_to_real_val, 1)
{
  this->varsize = 32;
  this->lowbit = lowbit;
  this->mask = ((BX_MAX_BIT32S >> (31 - (highbit - lowbit))) << lowbit);
  val.p32bit = ptr_to_real_val;
  if (base == BASE_HEX) {
    this->base = base;
    this->text_format = "0x%08x";
  }
}

// Unsigned 32 bit
bx_shadow_num_c::bx_shadow_num_c(bx_param_c *parent,
    const char *name,
    Bit32u *ptr_to_real_val,
    int base,
    Bit8u highbit,
    Bit8u lowbit)
: bx_param_num_c(parent, name, NULL, NULL, BX_MIN_BIT32U, BX_MAX_BIT32U, *ptr_to_real_val, 1)
{
  this->varsize = 32;
  this->lowbit = lowbit;
  this->mask = ((BX_MAX_BIT32U >> (31 - (highbit - lowbit))) << lowbit);
  val.p32bit = (Bit32s*) ptr_to_real_val;
  if (base == BASE_HEX) {
    this->base = base;
    this->text_format = "0x%08x";
  }
}

// Signed 16 bit
bx_shadow_num_c::bx_shadow_num_c(bx_param_c *parent,
    const char *name,
    Bit16s *ptr_to_real_val,
    int base,
    Bit8u highbit,
    Bit8u lowbit)
: bx_param_num_c(parent, name, NULL, NULL, BX_MIN_BIT16S, BX_MAX_BIT16S, *ptr_to_real_val, 1)
{
  this->varsize = 16;
  this->lowbit = lowbit;
  this->mask = ((BX_MAX_BIT16S >> (15 - (highbit - lowbit))) << lowbit);
  val.p16bit = ptr_to_real_val;
  if (base == BASE_HEX) {
    this->base = base;
    this->text_format = "0x%04x";
  }
}

// Unsigned 16 bit
bx_shadow_num_c::bx_shadow_num_c(bx_param_c *parent,
    const char *name,
    Bit16u *ptr_to_real_val,
    int base,
    Bit8u highbit,
    Bit8u lowbit)
: bx_param_num_c(parent, name, NULL, NULL, BX_MIN_BIT16U, BX_MAX_BIT16U, *ptr_to_real_val, 1)
{
  this->varsize = 16;
  this->lowbit = lowbit;
  this->mask = ((BX_MAX_BIT16U >> (15 - (highbit - lowbit))) << lowbit);
  val.p16bit = (Bit16s*) ptr_to_real_val;
  if (base == BASE_HEX) {
    this->base = base;
    this->text_format = "0x%04x";
  }
}

// Signed 8 bit
bx_shadow_num_c::bx_shadow_num_c(bx_param_c *parent,
    const char *name,
    Bit8s *ptr_to_real_val,
    int base,
    Bit8u highbit,
    Bit8u lowbit)
: bx_param_num_c(parent, name, NULL, NULL, BX_MIN_BIT8S, BX_MAX_BIT8S, *ptr_to_real_val, 1)
{
  this->varsize = 8;
  this->lowbit = lowbit;
  this->mask = ((BX_MAX_BIT8S >> (7 - (highbit - lowbit))) << lowbit);
  this->mask = (1 << (highbit - lowbit)) - 1;
  val.p8bit = ptr_to_real_val;
  if (base == BASE_HEX) {
    this->base = base;
    this->text_format = "0x%02x";
  }
}

// Unsigned 8 bit
bx_shadow_num_c::bx_shadow_num_c(bx_param_c *parent,
    const char *name,
    Bit8u *ptr_to_real_val,
    int base,
    Bit8u highbit,
    Bit8u lowbit)
: bx_param_num_c(parent, name, NULL, NULL, BX_MIN_BIT8U, BX_MAX_BIT8U, *ptr_to_real_val, 1)
{
  this->varsize = 8;
  this->lowbit = lowbit;
  this->mask = ((BX_MAX_BIT8U >> (7 - (highbit - lowbit))) << lowbit);
  val.p8bit = (Bit8s*) ptr_to_real_val;
  if (base == BASE_HEX) {
    this->base = base;
    this->text_format = "0x%02x";
  }
}

Bit64s bx_shadow_num_c::get64()
{
  Bit64u current = 0;
  switch (varsize) {
    case 8: current = *(val.p8bit);  break;
    case 16: current = *(val.p16bit);  break;
    case 32: current = *(val.p32bit);  break;
    case 64: current = *(val.p64bit);  break;
    default: BX_PANIC(("unsupported varsize %d", varsize));
  }
  current = (current >> lowbit) & mask;
  if (handler) {
    // the handler can decide what value to return and/or do some side effect
    return (*handler)(this, 0, current) & mask;
  } else {
    // just return the value
    return current;
  }
}

void bx_shadow_num_c::set(Bit64s newval)
{
  Bit64u tmp = 0;
  if (((newval < min) || (newval > max)) && (min != BX_MIN_BIT64S) && ((Bit64u)max != BX_MAX_BIT64U))
    BX_PANIC(("numerical parameter %s was set to " FMT_LL "d, which is out of range " FMT_LL "d to " FMT_LL "d", get_name (), newval, min, max));
  switch (varsize) {
    case 8:
      tmp = *(val.p8bit) & ~(mask << lowbit);
      tmp |= (newval & mask) << lowbit;
      *(val.p8bit) = (Bit8s)tmp;
      break;
    case 16:
      tmp = *(val.p16bit) & ~(mask << lowbit);
      tmp |= (newval & mask) << lowbit;
      *(val.p16bit) = (Bit16s)tmp;
      break;
    case 32:
      tmp = *(val.p32bit) & ~(mask << lowbit);
      tmp |= (newval & mask) << lowbit;
      *(val.p32bit) = (Bit32s)tmp;
      break;
    case 64:
      tmp = *(val.p64bit) & ~(mask << lowbit);
      tmp |= (newval & mask) << lowbit;
      *(val.p64bit) = (Bit64s)tmp;
      break;
    default:
      BX_PANIC(("unsupported varsize %d", varsize));
  }
  if (handler) {
    // the handler can override the new value and/or perform some side effect
    (*handler)(this, 1, tmp);
  }
}

void bx_shadow_num_c::reset()
{
  BX_PANIC(("reset not supported on bx_shadow_num_c yet"));
}

bx_param_bool_c::bx_param_bool_c(bx_param_c *parent,
    const char *name,
    const char *label,
    const char *description,
    Bit64s initial_val,
    bx_bool is_shadow)
  : bx_param_num_c(parent, name, label, description, 0, 1, initial_val, is_shadow)
{
  set_type(BXT_PARAM_BOOL);
}

bx_shadow_bool_c::bx_shadow_bool_c(bx_param_c *parent,
      const char *name,
      const char *label,
      bx_bool *ptr_to_real_val,
      Bit8u bitnum)
  : bx_param_bool_c(parent, name, label, NULL, (Bit64s) *ptr_to_real_val, 1)
{
  val.pbool = ptr_to_real_val;
  this->bitnum = bitnum;
}

bx_shadow_bool_c::bx_shadow_bool_c(bx_param_c *parent,
      const char *name,
      bx_bool *ptr_to_real_val,
      Bit8u bitnum)
  : bx_param_bool_c(parent, name, NULL, NULL, (Bit64s) *ptr_to_real_val, 1)
{
  val.pbool = ptr_to_real_val;
  this->bitnum = bitnum;
}

Bit64s bx_shadow_bool_c::get64()
{
  if (handler) {
    // the handler can decide what value to return and/or do some side effect
    Bit64s ret = (*handler)(this, 0, (Bit64s) *(val.pbool));
    return (ret>>bitnum) & 1;
  } else {
    // just return the value
    return (*(val.pbool)) & 1;
  }
}

void bx_shadow_bool_c::set(Bit64s newval)
{
  // only change the bitnum bit
  Bit64s mask = 1 << bitnum;
  *(val.pbool) &= ~mask;
  *(val.pbool) |= ((newval & 1) << bitnum);
  if (handler) {
    // the handler can override the new value and/or perform some side effect
    (*handler)(this, 1, newval&1);
  }
}

bx_param_enum_c::bx_param_enum_c(bx_param_c *parent,
      const char *name,
      const char *label,
      const char *description,
      const char **choices,
      Bit64s initial_val,
      Bit64s value_base)
  : bx_param_num_c(parent, name, label, description, value_base, BX_MAX_BIT64S, initial_val)
{
  set_type(BXT_PARAM_ENUM);
  this->choices = choices;
  // count number of choices, set max
  const char **p = choices;
  while (*p != NULL) p++;
  this->min = value_base;
  // now that the max is known, replace the BX_MAX_BIT64S sent to the parent
  // class constructor with the real max.
  this->max = value_base + (p - choices - 1);
  this->deps_bitmap = NULL;
  set(initial_val);
}

bx_param_enum_c::~bx_param_enum_c()
{
  if (deps_bitmap != NULL) {
    free(deps_bitmap);
  }
}


void bx_param_enum_c::set(Bit64s val)
{
  bx_param_num_c::set(val);
  update_dependents();
}

int bx_param_enum_c::find_by_name(const char *string)
{
  const char **p;
  for (p=&choices[0]; *p; p++) {
    if (!strcmp(string, *p))
      return p-choices;
  }
  return -1;
}

bx_bool bx_param_enum_c::set_by_name(const char *string)
{
  int n = find_by_name(string);
  if (n<0) return 0;
  set(n + min);
  return 1;
}

void bx_param_enum_c::set_dependent_list(bx_list_c *l, bx_bool enable_all)
{
  dependent_list = l;
  deps_bitmap = (Bit64u*)malloc((size_t)(sizeof(Bit64u) * (max - min + 1)));
  for (int i=0; i<(max-min+1); i++) {
    if (enable_all) {
      deps_bitmap[i] = (1 << (l->get_size())) - 1;
    } else {
      deps_bitmap[i] = 0;
    }
  }
  update_dependents();
}

void bx_param_enum_c::set_dependent_bitmap(Bit64s value, Bit64u bitmap)
{
  if (deps_bitmap != NULL) {
    deps_bitmap[value - min] = bitmap;
  }
  update_dependents();
}

Bit64u bx_param_enum_c::get_dependent_bitmap(Bit64s value)
{
  if (deps_bitmap != NULL) {
    return deps_bitmap[value - min];
  }
  return 0;
}

void bx_param_enum_c::update_dependents()
{
  if ((dependent_list != NULL) && (deps_bitmap != NULL)) {
    Bit64u en_bmap = deps_bitmap[val.number - min];
    Bit64u mask = 0x1;
    for (int i=0; i<dependent_list->get_size(); i++) {
      int en = (en_bmap & mask) && enabled;
      bx_param_c *param = dependent_list->get(i);
      if (param != this)
        param->set_enabled(en);
      mask <<= 1;
    }
  }
}

void bx_param_enum_c::set_enabled(int en)
{
  // The enable handler may wish to allow/disallow the action
  if (enable_handler) {
    en = (*enable_handler)(this, en);
  }
  bx_param_c::set_enabled(en);
  update_dependents();
}

bx_param_string_c::bx_param_string_c(bx_param_c *parent,
    const char *name,
    const char *label,
    const char *description,
    const char *initial_val,
    int maxsize)
  : bx_param_c(SIM->gen_param_id(), name, label, description)
{
  set_type(BXT_PARAM_STRING);
  int initial_val_size = strlen(initial_val) + 1;
  if (maxsize < 0) {
    maxsize = initial_val_size;
  } else if (initial_val_size > maxsize) {
    initial_val_size = maxsize;
  }
  this->val = new char[maxsize];
  this->initial_val = new char[maxsize];
  this->handler = NULL;
  this->enable_handler = NULL;
  this->maxsize = maxsize;
  strncpy(this->val, initial_val, initial_val_size);
  if (maxsize > initial_val_size)
    memset(this->val + initial_val_size, 0, maxsize - initial_val_size);
  strncpy(this->initial_val, initial_val, maxsize);
  this->options = 0;
  set(initial_val);
  if (parent) {
    BX_ASSERT(parent->get_type() == BXT_LIST);
    this->parent = (bx_list_c *)parent;
    this->parent->add(this);
  }
}

bx_param_filename_c::bx_param_filename_c(bx_param_c *parent,
    const char *name,
    const char *label,
    const char *description,
    const char *initial_val,
    int maxsize)
  : bx_param_string_c(parent, name, label, description, initial_val, maxsize)
{
  set_options(IS_FILENAME);
  int len = strlen(initial_val);
  if ((len > 4) && (initial_val[len - 4] == '.')) {
    ext = &initial_val[len - 3];
  } else {
    ext = NULL;
  }
}

bx_param_string_c::~bx_param_string_c()
{
  if (val != NULL) delete [] val;
  if (initial_val != NULL) delete [] initial_val;
}

void bx_param_string_c::reset()
{
  set(initial_val);
}

void bx_param_string_c::set_handler(param_string_event_handler handler)
{
  this->handler = handler;
}

void bx_param_string_c::set_enable_handler(param_enable_handler handler)
{
  this->enable_handler = handler;
}

void bx_param_string_c::update_dependents()
{
  if (dependent_list) {
    int en = (strlen(val) > 0) && (strcmp(val, "none")) && enabled;
    for (int i=0; i<dependent_list->get_size(); i++) {
      bx_param_c *param = dependent_list->get(i);
      if (param != this)
        param->set_enabled(en);
    }
  }
}

void bx_param_string_c::set_enabled(int en)
{
  // The enable handler may wish to allow/disallow the action
  if (enable_handler) {
    en = (*enable_handler)(this, en);
  }
  bx_param_c::set_enabled(en);
  if (dependent_list != NULL) update_dependents();
}

void bx_param_string_c::set_dependent_list(bx_list_c *l)
{
  dependent_list = l;
  update_dependents();
}

Bit32s bx_param_string_c::get(char *buf, int len)
{
  if (options & RAW_BYTES)
    memcpy(buf, val, len);
  else
    strncpy(buf, val, len);
  if (handler) {
    // the handler can choose to replace the value in val/len.  Also its
    // return value is passed back as the return value of get.
    (*handler)(this, 0, buf, buf, len);
  }
  return 0;
}

void bx_param_string_c::set(const char *buf)
{
  char *oldval = new char[maxsize];

  if (options & RAW_BYTES) {
    memcpy(oldval, val, maxsize);
    memcpy(val, buf, maxsize);
  } else {
    strncpy(oldval, val, maxsize);
    oldval[maxsize - 1] = 0;
    strncpy(val, buf, maxsize);
    val[maxsize - 1] = 0;
  }
  if (handler) {
    // the handler can return a different char* to be copied into the value
    buf = (*handler)(this, 1, oldval, buf, -1);
  }
  delete [] oldval;
  if (dependent_list != NULL) update_dependents();
}

bx_bool bx_param_string_c::equals(const char *buf)
{
  if (options & RAW_BYTES)
    return (memcmp(val, buf, maxsize) == 0);
  else
    return (strncmp(val, buf, maxsize) == 0);
}

void bx_param_string_c::set_initial_val(const char *buf)
{
  if (options & RAW_BYTES)
    memcpy(initial_val, buf, maxsize);
  else
    strncpy(initial_val, buf, maxsize);
  set(initial_val);
}

bx_bool bx_param_string_c::isempty()
{
  if (options & RAW_BYTES) {
    return (memcmp(val, initial_val, maxsize) == 0);
  } else {
    return ((strlen(val) == 0) || !strcmp(val, "none"));
  }
}

int bx_param_string_c::sprint(char *buf, int len, bx_bool dquotes)
{
  char tmpbyte[4];

  if (get_options() & RAW_BYTES) {
    buf[0] = 0;
    for (int j = 0; j < maxsize; j++) {
      if (j > 0) {
        tmpbyte[0] = separator;
        tmpbyte[1] = 0;
        strcat(buf, tmpbyte);
      }
      sprintf(tmpbyte, "%02x", (Bit8u)val[j]);
      strcat(buf, tmpbyte);
    }
  } else {
    if (!isempty()) {
      if (dquotes) {
        snprintf(buf, len, "\"%s\"", val);
      } else {
        snprintf(buf, len, "%s", val);
      }
    } else {
      strcpy(buf, "none");
    }
  }
  return strlen(buf);
}

bx_shadow_data_c::bx_shadow_data_c(bx_param_c *parent,
    const char *name,
    Bit8u *ptr_to_data,
    Bit32u data_size)
  : bx_param_c(SIM->gen_param_id(), name, "")
{
  set_type(BXT_PARAM_DATA);
  this->data_ptr = ptr_to_data;
  this->data_size = data_size;
  if (parent) {
    BX_ASSERT(parent->get_type() == BXT_LIST);
    this->parent = (bx_list_c *)parent;
    this->parent->add(this);
  }
}
  
bx_shadow_filedata_c::bx_shadow_filedata_c(bx_param_c *parent,
    const char *name, FILE **scratch_file_ptr_ptr)
  : bx_param_c(SIM->gen_param_id(), name, "")
{
  set_type(BXT_PARAM_FILEDATA);
  this->scratch_fpp = scratch_file_ptr_ptr;
  this->save_handler = NULL;
  this->restore_handler = NULL;
  if (parent) {
    BX_ASSERT(parent->get_type() == BXT_LIST);
    this->parent = (bx_list_c *)parent;
    this->parent->add(this);
  }
}

// Save handler: called before file save, Restore handler: called after file restore
void bx_shadow_filedata_c::set_sr_handlers(void *devptr, filedata_save_handler save, filedata_restore_handler restore)
{
  this->sr_devptr = devptr;
  this->save_handler = save;
  this->restore_handler = restore;
}

void bx_shadow_filedata_c::save(FILE *save_fp)
{
  if (save_handler)
    (*save_handler)(sr_devptr, save_fp);
}

void bx_shadow_filedata_c::restore(FILE *save_fp)
{
  if (restore_handler)
    (*restore_handler)(sr_devptr, save_fp);
}

bx_list_c::bx_list_c(bx_param_c *parent)
  : bx_param_c(SIM->gen_param_id(), "list", "")
{
  set_type(BXT_LIST);
  this->size = 0;
  this->list = NULL;
  this->parent = NULL;
  if (parent) {
    BX_ASSERT(parent->get_type() == BXT_LIST);
    this->parent = (bx_list_c *)parent;
    this->parent->add(this);
  }
  init("");
}

bx_list_c::bx_list_c(bx_param_c *parent, const char *name)
  : bx_param_c(SIM->gen_param_id(), name, "")
{
  set_type (BXT_LIST);
  this->size = 0;
  this->list = NULL;
  this->parent = NULL;
  if (parent) {
    BX_ASSERT(parent->get_type() == BXT_LIST);
    this->parent = (bx_list_c *)parent;
    this->parent->add(this);
  }
  init("");
}

bx_list_c::bx_list_c(bx_param_c *parent, const char *name, const char *title)
  : bx_param_c(SIM->gen_param_id(), name, "")
{
  set_type (BXT_LIST);
  this->size = 0;
  this->list = NULL;
  this->parent = NULL;
  if (parent) {
    BX_ASSERT(parent->get_type() == BXT_LIST);
    this->parent = (bx_list_c *)parent;
    this->parent->add(this);
  }
  init(title);
}

bx_list_c::bx_list_c(bx_param_c *parent, const char *name, const char *title, bx_param_c **init_list)
  : bx_param_c(SIM->gen_param_id(), name, "")
{
  set_type(BXT_LIST);
  this->size = 0;
  this->list = NULL;
  while (init_list[this->size] != NULL)
    add(init_list[this->size]);
  this->parent = NULL;
  if (parent) {
    BX_ASSERT(parent->get_type() == BXT_LIST);
    this->parent = (bx_list_c *)parent;
    this->parent->add(this);
  }
  init(title);
}

bx_list_c::~bx_list_c()
{
  if (list != NULL) {
    clear();
  }
  if (title != NULL) delete [] title;
}

void bx_list_c::init(const char *list_title)
{
  if (list_title) {
    this->title = new char[strlen(list_title)+1];
    strcpy(this->title, list_title);
  } else {
    this->title = new char[1];
    this->title[0] = 0;
  }
  this->options = 0;
  this->choice = 1;
}

void bx_list_c::set_parent(bx_param_c *newparent)
{
  if (parent) {
    // if this object already had a parent, the correct thing
    // to do would be to remove this object from the parent's
    // list of children.  Deleting children is currently
    // not supported.
    BX_PANIC(("bx_list_c::set_parent: changing from one parent to another is not supported"));
  }
  if (newparent) {
    BX_ASSERT(newparent->get_type() == BXT_LIST);
    parent = (bx_list_c *)newparent;
    parent->add(this);
  }
}

bx_list_c* bx_list_c::clone()
{
  bx_list_c *newlist = new bx_list_c(NULL, name, title);
  for (int i=0; i<get_size(); i++)
    newlist->add(get(i));
  newlist->set_options(options);
  return newlist;
}

void bx_list_c::add(bx_param_c *param)
{
  bx_listitem_t *item;

  if ((get_by_name(param->get_name()) != NULL) && (param->get_parent() == this)) {
    BX_PANIC(("parameter '%s' already exists in list '%s'", param->get_name(), this->get_name()));
    return;
  }
  item = (bx_listitem_t*) malloc(sizeof(bx_listitem_t));
  if (item == NULL) {
    BX_PANIC(("bx_list_c::add(): malloc() failed"));
    return;
  }
  item->param = param;
  item->next = NULL;
  if (list == NULL) {
    list = item;
  } else {
    bx_listitem_t *temp = list;
    while (temp->next)
      temp = temp->next;
    temp->next = item;
  }
  if (runtime_param) {
    param->set_runtime_param(1);
  }
  size++;
}

bx_param_c* bx_list_c::get(int index)
{
  BX_ASSERT(index >= 0 && index < size);
  int i = 0;
  bx_listitem_t *temp = list;
  while (temp != NULL) {
    if (i == index) {
      return temp->param;
    }
    temp = temp->next;
    i++;
  }
  return NULL;
}

bx_param_c* bx_list_c::get_by_name(const char *name)
{
  bx_listitem_t *temp = list;
  while (temp != NULL) {
    bx_param_c *p = temp->param;
    if (!stricmp(name, p->get_name())) {
      return p;
    }
    temp = temp->next;
  }
  return NULL;
}

void bx_list_c::reset()
{
  bx_listitem_t *temp = list;
  while (temp != NULL) {
    temp->param->reset();
    temp = temp->next;
  }
}

void bx_list_c::clear()
{
  bx_listitem_t *temp = list, *next;
  while (temp != NULL) {
    if (temp->param->get_parent() == this) {
      delete temp->param;
    }
    next = temp->next;
    free(temp);
    temp = next;
  }
  list = NULL;
  size = 0;
}

void bx_list_c::remove(const char *name)
{
  bx_listitem_t *item, *prev = NULL;

  for (item = list; item; item = item->next) {
    bx_param_c *p = item->param;
    if (!stricmp(name, p->get_name())) {
      if (p->get_parent() == this) {
        delete p;
      }
      if (prev == NULL) {
        list = item->next;
      } else {
        prev->next = item->next;
      }
      free(item);
      size--;
      break;
    } else {
      prev = item;
    }
  }
}

void bx_list_c::set_runtime_param(int val)
{
  bx_listitem_t *item;

  runtime_param = val;
  if (runtime_param) {
    for (item = list; item; item = item->next) {
      item->param->set_runtime_param(1);
    }
  }
}
