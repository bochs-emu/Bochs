/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2023  The Bochs Project
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
//
// This file defines the plugin and plugin-device registration functions and
// the device registration functions.  It handles dynamic loading of modules,
// using the LTDL library for cross-platform support.
//
// This file is based on the plugin.c file from plex86, but with significant
// changes to make it work in Bochs.
// Plex86 is Copyright (C) 1999-2000  The plex86 developers team
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "iodev/iodev.h"
#include "plugin.h"

#ifndef WIN32
#include <dirent.h> /* opendir, readdir */
#include <locale.h> /* setlocale */
#endif

#define LOG_THIS genlog->

#define PLUGIN_ENTRY_FMT_STRING       "lib%s_plugin_entry"
#define GUI_PLUGIN_ENTRY_FMT_STRING   "lib%s_gui_plugin_entry"
#define IMG_PLUGIN_ENTRY_FMT_STRING   "lib%s_img_plugin_entry"
#define NET_PLUGIN_ENTRY_FMT_STRING   "libeth_%s_plugin_entry"
#define SND_PLUGIN_ENTRY_FMT_STRING   "libsound%s_plugin_entry"

#define PLUGIN_PATH                   ""

#ifndef WIN32
#define PLUGIN_FILENAME_FORMAT       "libbx_%s.so"
#define GUI_PLUGIN_FILENAME_FORMAT   "libbx_%s_gui.so"
#define IMG_PLUGIN_FILENAME_FORMAT   "libbx_%s_img.so"
#define NET_PLUGIN_FILENAME_FORMAT   "libbx_eth_%s.so"
#define SND_PLUGIN_FILENAME_FORMAT   "libbx_sound%s.so"
#else
#define PLUGIN_FILENAME_FORMAT       "bx_%s.dll"
#define GUI_PLUGIN_FILENAME_FORMAT   "bx_%s_gui.dll"
#define IMG_PLUGIN_FILENAME_FORMAT   "bx_%s_img.dll"
#define NET_PLUGIN_FILENAME_FORMAT   "bx_eth_%s.dll"
#define SND_PLUGIN_FILENAME_FORMAT "bx_sound%s.dll"
#endif

logfunctions *pluginlog;

extern "C" {

void  (*pluginRegisterIRQ)(unsigned irq, const char* name) = 0;
void  (*pluginUnregisterIRQ)(unsigned irq, const char* name) = 0;

void (*pluginSetHRQ)(unsigned val) = 0;
void (*pluginSetHRQHackCallback)(void (*callback)(void)) = 0;

int (*pluginRegisterIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, const char *name, Bit8u mask) = 0;
int (*pluginRegisterIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, const char *name, Bit8u mask) = 0;
int (*pluginUnregisterIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, Bit8u mask) = 0;
int (*pluginUnregisterIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, Bit8u mask) = 0;
int (*pluginRegisterIOReadHandlerRange)(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, unsigned end, const char *name, Bit8u mask) = 0;
int (*pluginRegisterIOWriteHandlerRange)(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, unsigned end, const char *name, Bit8u mask) = 0;
int (*pluginUnregisterIOReadHandlerRange)(void *thisPtr, ioReadHandler_t callback,
                            unsigned begin, unsigned end, Bit8u mask) = 0;
int (*pluginUnregisterIOWriteHandlerRange)(void *thisPtr, ioWriteHandler_t callback,
                             unsigned begin, unsigned end, Bit8u mask) = 0;
int (*pluginRegisterDefaultIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                            const char *name, Bit8u mask) = 0;
int (*pluginRegisterDefaultIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                             const char *name, Bit8u mask) = 0;

void (*pluginHRQHackCallback)(void);
unsigned pluginHRQ = 0;

plugin_t *plugins = NULL;      /* Head of the linked list of plugins  */
device_t *devices = NULL;      /* Head of the linked list of registered devices  */
device_t *core_devices = NULL; /* Head of the linked list of registered core devices  */

plugin_t *current_plugin_context = NULL;

/************************************************************************/
/* Builtins declarations                                                */
/************************************************************************/

  static void
builtinRegisterIRQ(unsigned irq, const char* name)
{
  bx_devices.register_irq(irq, name);
}

  static void
builtinUnregisterIRQ(unsigned irq, const char* name)
{
  bx_devices.unregister_irq(irq, name);
}

  static void
builtinSetHRQ(unsigned val)
{
  pluginHRQ = val;
}

  static void
builtinSetHRQHackCallback(void (*callback)(void))
{
  pluginHRQHackCallback = callback;
}

  static int
builtinRegisterIOReadHandler(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, const char *name, Bit8u mask)
{
  int ret;
  BX_ASSERT(mask<8);
  ret = bx_devices.register_io_read_handler (thisPtr, callback, base, name, mask);
  pluginlog->ldebug("plugin %s registered I/O read  address at %04x", name, base);
  return ret;
}

  static int
builtinRegisterIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, const char *name, Bit8u mask)
{
  int ret;
  BX_ASSERT(mask<8);
  ret = bx_devices.register_io_write_handler (thisPtr, callback, base, name, mask);
  pluginlog->ldebug("plugin %s registered I/O write address at %04x", name, base);
  return ret;
}

  static int
builtinUnregisterIOReadHandler(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, Bit8u mask)
{
  int ret;
  BX_ASSERT(mask<8);
  ret = bx_devices.unregister_io_read_handler (thisPtr, callback, base, mask);
  pluginlog->ldebug("plugin unregistered I/O read address at %04x", base);
  return ret;
}

  static int
builtinUnregisterIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, Bit8u mask)
{
  int ret;
  BX_ASSERT(mask<8);
  ret = bx_devices.unregister_io_write_handler (thisPtr, callback, base, mask);
  pluginlog->ldebug("plugin unregistered I/O write address at %04x", base);
  return ret;
}

  static int
builtinRegisterIOReadHandlerRange(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, unsigned end, const char *name, Bit8u mask)
{
  int ret;
  BX_ASSERT(mask<8);
  ret = bx_devices.register_io_read_handler_range (thisPtr, callback, base, end, name, mask);
  pluginlog->ldebug("plugin %s registered I/O read addresses %04x to %04x", name, base, end);
  return ret;
}

  static int
builtinRegisterIOWriteHandlerRange(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, unsigned end, const char *name, Bit8u mask)
{
  int ret;
  BX_ASSERT(mask<8);
  ret = bx_devices.register_io_write_handler_range (thisPtr, callback, base, end, name, mask);
  pluginlog->ldebug("plugin %s registered I/O write addresses %04x to %04x", name, base, end);
  return ret;
}

  static int
builtinUnregisterIOReadHandlerRange(void *thisPtr, ioReadHandler_t callback,
                            unsigned begin, unsigned end, Bit8u mask)
{
  int ret;
  BX_ASSERT(mask<8);
  ret = bx_devices.unregister_io_read_handler_range (thisPtr, callback, begin, end, mask);
  pluginlog->ldebug("plugin unregistered I/O read addresses %04x to %04x", begin, end);
  return ret;
}

  static int
builtinUnregisterIOWriteHandlerRange(void *thisPtr, ioWriteHandler_t callback,
                             unsigned begin, unsigned end, Bit8u mask)
{
  int ret;
  BX_ASSERT(mask<8);
  ret = bx_devices.unregister_io_write_handler_range (thisPtr, callback, begin, end, mask);
  pluginlog->ldebug("plugin unregistered I/O write addresses %04x to %04x", begin, end);
  return ret;
}

  static int
builtinRegisterDefaultIOReadHandler(void *thisPtr, ioReadHandler_t callback,
                            const char *name, Bit8u mask)
{
  BX_ASSERT(mask<8);
  bx_devices.register_default_io_read_handler (thisPtr, callback, name, mask);
  pluginlog->ldebug("plugin %s registered default I/O read ", name);
  return 0;
}

  static int
builtinRegisterDefaultIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                             const char *name, Bit8u mask)
{
  BX_ASSERT(mask<8);
  bx_devices.register_default_io_write_handler (thisPtr, callback, name, mask);
  pluginlog->ldebug("plugin %s registered default I/O write ", name);
  return 0;
}

#if BX_PLUGINS
/************************************************************************/
/* Search for all available plugins                                           */
/************************************************************************/

void plugin_add_entry(char *pgn_name, Bit16u type, Bit8u flags)
{
  plugin_t *plugin, *temp;

  if ((type & PLUGTYPE_GUI) > 0) {
    if (!strncmp(pgn_name + strlen(pgn_name) - 4, "_gui", 4)) {
      pgn_name[strlen(pgn_name) - 4] = 0;
    }
  } else if (type == PLUGTYPE_IMG) {
    if (!strncmp(pgn_name + strlen(pgn_name) - 4, "_img", 4)) {
      pgn_name[strlen(pgn_name) - 4] = 0;
    }
  } else if (type == PLUGTYPE_NET) {
    if (!strncmp(pgn_name, "eth_", 4)) {
      memmove(pgn_name, pgn_name + 4, strlen(pgn_name) - 3);
    }
  } else if (type == PLUGTYPE_SND) {
    if (!strncmp(pgn_name, "sound", 5)) {
      memmove(pgn_name, pgn_name + 5, strlen(pgn_name) - 4);
    }
  }
  plugin = new plugin_t;
  plugin->type = type;
  plugin->flags = flags;
  plugin->name = pgn_name;
  plugin->loadtype = PLUGTYPE_NULL;
  plugin->initialized = 0;
  /* Insert plugin at the _end_ of the plugin linked list. */
  plugin->next = NULL;
  if (plugins == NULL) {
    /* Empty list, this become the first entry. */
    plugins = plugin;
  } else {
    /* Non-empty list.  Add to end. */
    temp = plugins;

    while (temp->next)
      temp = temp->next;

    temp->next = plugin;
  }
}

void plugins_search(void)
{
  int nlen, flen1, flen2;
  char *fmtptr, *ltdl_path_var, *pgn_name, *pgn_path, *ptr;
  char fmtstr[32];
#ifndef WIN32
  const char *path_sep = ":";
  DIR *dir;
  struct dirent *dent;
  lt_dlhandle handle;
#else
  const char *path_sep = ";";
  WIN32_FIND_DATA finddata;
  HANDLE hFind;
  char filter[MAX_PATH];
  char path[MAX_PATH];
  HINSTANCE handle;
#endif
  char tmpname[BX_PATHNAME_LEN];
  plugin_entry_t plugin_entry;
  Bit16u type;
  Bit8u flags;

#ifndef WIN32
  setlocale(LC_ALL, "en_US");
#endif
  ltdl_path_var = getenv("LTDL_LIBRARY_PATH");
  sprintf(fmtstr, PLUGIN_FILENAME_FORMAT, "*");
  fmtptr = strchr(fmtstr, '*');
  flen1 = fmtptr - fmtstr;
  flen2 = strlen(fmtstr) - flen1 - 1;
  if (ltdl_path_var != NULL) {
    pgn_path = new char[strlen(ltdl_path_var) + 1];
    strcpy(pgn_path, ltdl_path_var);
  } else {
    pgn_path = new char[2];
    strcpy(pgn_path, ".");
  }
  ptr = strtok(pgn_path, path_sep);
  while (ptr != NULL) {
#ifndef WIN32
    dir = opendir(ptr);
    if (dir != NULL) {
      while ((dent=readdir(dir)) != NULL) {
        nlen = strlen(dent->d_name);
        if ((!strncmp(dent->d_name, fmtstr, flen1)) &&
            (!strcmp(dent->d_name + nlen - flen2, fmtptr + 1))) {
          pgn_name = new char[nlen - flen1 - flen2 + 1];
          strncpy(pgn_name, dent->d_name + flen1, nlen - flen1 - flen2);
          pgn_name[nlen - flen1 - flen2] = 0;
          handle = lt_dlopen(dent->d_name);
          if (handle) {
            sprintf(tmpname, PLUGIN_ENTRY_FMT_STRING, pgn_name);
            plugin_entry = (plugin_entry_t) lt_dlsym(handle, tmpname);
            if (plugin_entry != NULL) {
              type = (Bit16u) plugin_entry(NULL, PLUGTYPE_NULL, PLUGIN_PROBE);
              flags = (Bit8u) plugin_entry(NULL, PLUGTYPE_NULL, PLUGIN_FLAGS);
              plugin_add_entry(pgn_name, type, flags);
            }
            lt_dlclose(handle);
          } else {
            delete [] pgn_name;
          }
        }
      }
      closedir(dir);
    }
#else
    sprintf(filter, "%s\\*.dll", ptr);
    hFind = FindFirstFile(filter, &finddata);
    if (hFind != INVALID_HANDLE_VALUE) {
      do {
        nlen = lstrlen(finddata.cFileName);
        if ((!strncmp(finddata.cFileName, fmtstr, flen1)) &&
            (!strcmp(finddata.cFileName + nlen - flen2, fmtptr + 1))) {
          pgn_name = new char[nlen - flen1 - flen2 + 1];
          strncpy(pgn_name, finddata.cFileName + flen1, nlen - flen1 - flen2);
          pgn_name[nlen - flen1 - flen2] = 0;
          sprintf(path, "%s\\%s", ptr, finddata.cFileName);
          handle = LoadLibrary(path);
          if (handle) {
            sprintf(tmpname, PLUGIN_ENTRY_FMT_STRING, pgn_name);
            plugin_entry = (plugin_entry_t) GetProcAddress(handle, tmpname);
            if (plugin_entry != NULL) {
              type = (Bit16u) plugin_entry(NULL, PLUGTYPE_NULL, PLUGIN_PROBE);
              flags = (Bit8u) plugin_entry(NULL, PLUGTYPE_NULL, PLUGIN_FLAGS);
              plugin_add_entry(pgn_name, type, flags);
            }
            FreeLibrary(handle);
          }
        }
      } while (FindNextFile(hFind, &finddata));
      FindClose(hFind);
    }
#endif
    ptr = strtok(NULL, ":");
  }
  delete [] pgn_path;
}

Bit8u bx_get_plugins_count(Bit16u type)
{
  plugin_t *temp;
  Bit8u count = 0;

  if (plugins != NULL) {
    temp = plugins;

    while (temp != NULL) {
      if ((type & temp->type) != 0)
        count++;
      temp = temp->next;
    }
  }
  return count;
}

const char* bx_get_plugin_name(Bit16u type, Bit8u index)
{
  plugin_t *temp;
  int count = 0;

  if (plugins != NULL) {
    temp = plugins;

    while (temp != NULL) {
      if ((type & temp->type) != 0) {
        if (count == index)
          return temp->name;
        count++;
      }
      temp = temp->next;
    }
  }
  return NULL;
}

Bit8u bx_get_plugin_flags(Bit16u type, Bit8u index)
{
  plugin_t *temp;
  int count = 0;

  if (plugins != NULL) {
    temp = plugins;

    while (temp != NULL) {
      if ((type & temp->type) != 0) {
        if (count == index)
          return temp->flags;
        count++;
      }
      temp = temp->next;
    }
  }
  return 0;
}

/************************************************************************/
/* Plugin initialization / deinitialization                             */
/************************************************************************/

bool plugin_init_one(plugin_t *plugin)
{
  /* initialize the plugin */
  if (plugin->plugin_entry(plugin, plugin->loadtype, PLUGIN_INIT))
  {
    pluginlog->info("Plugin initialization failed for %s", plugin->name);
    plugin_abort(plugin);
    return 0;
  }
  plugin->initialized = 1;
  return 1;
}


bool plugin_unload(plugin_t *plugin)
{
  if (plugin->loadtype != PLUGTYPE_NULL) {
    if (plugin->initialized)
      plugin->plugin_entry(plugin, plugin->type, PLUGIN_FINI);
#if defined(WIN32)
    FreeLibrary(plugin->handle);
#else
    lt_dlclose(plugin->handle);
#endif
    plugin->loadtype = PLUGTYPE_NULL;
    return 1;
  } else {
    return 0;
  }
}

bool plugin_load(const char *name, Bit16u type)
{
  plugin_t *plugin = NULL, *temp;
#if defined(WIN32)
  char dll_path_list[MAX_PATH];
#endif

  if (plugins != NULL) {
    temp = plugins;

    while (temp != NULL) {
      if (!strcmp(name, temp->name) && ((type & temp->type) != 0)) {
        if (temp->loadtype != PLUGTYPE_NULL) {
          BX_PANIC(("plugin '%s' already loaded", name));
          return 0;
        } else {
          plugin = temp;
          break;
        }
      }
      temp = temp->next;
    }
  }
  if (plugin == NULL) {
    BX_PANIC(("plugin '%s' not found", name));
    return 0;
  }

  char plugin_filename[BX_PATHNAME_LEN], tmpname[BX_PATHNAME_LEN];
  if (type == PLUGTYPE_GUI) {
    sprintf(tmpname, GUI_PLUGIN_FILENAME_FORMAT, name);
  } else if (type == PLUGTYPE_IMG) {
    sprintf(tmpname, IMG_PLUGIN_FILENAME_FORMAT, name);
  } else if (type == PLUGTYPE_NET) {
    sprintf(tmpname, NET_PLUGIN_FILENAME_FORMAT, name);
  } else if (type == PLUGTYPE_SND) {
    sprintf(tmpname, SND_PLUGIN_FILENAME_FORMAT, name);
  } else {
    sprintf(tmpname, PLUGIN_FILENAME_FORMAT, name);
  }
  sprintf(plugin_filename, "%s%s", PLUGIN_PATH, tmpname);

  // Set context so that any devices that the plugin registers will
  // be able to see which plugin created them.  The registration will
  // be called from either dlopen (global constructors) or plugin_entry.
  BX_ASSERT(current_plugin_context == NULL);
  current_plugin_context = plugin;
#if defined(WIN32)
  char *ptr;
  plugin->handle = LoadLibrary(plugin_filename);
  if (!plugin->handle) {
    if (GetEnvironmentVariable("LTDL_LIBRARY_PATH", dll_path_list, MAX_PATH)) {
      ptr = strtok(dll_path_list, ";");
      while ((ptr) && !plugin->handle) {
        sprintf(plugin_filename, "%s\\%s", ptr, tmpname);
        plugin->handle = LoadLibrary(plugin_filename);
        ptr = strtok(NULL, ";");
      }
    }
  }
  BX_INFO(("DLL handle is %p", plugin->handle));
  if (!plugin->handle) {
    current_plugin_context = NULL;
    BX_PANIC(("LoadLibrary failed for module '%s' (%s): error=%d", name,
              plugin_filename, GetLastError()));
    return 0;
  }
#else
  plugin->handle = lt_dlopen(plugin_filename);
  BX_INFO(("lt_dlhandle is %p", plugin->handle));
  if (!plugin->handle) {
    current_plugin_context = NULL;
    BX_PANIC(("dlopen failed for module '%s' (%s): %s", name, plugin_filename,
              lt_dlerror()));
    return 0;
  }
#endif
  plugin->loadtype = type;

  if (type == PLUGTYPE_GUI) {
    sprintf(tmpname, GUI_PLUGIN_ENTRY_FMT_STRING, name);
  } else if (type == PLUGTYPE_IMG) {
    sprintf(tmpname, IMG_PLUGIN_ENTRY_FMT_STRING, name);
  } else if (type == PLUGTYPE_NET) {
    sprintf(tmpname, NET_PLUGIN_ENTRY_FMT_STRING, name);
  } else if (type == PLUGTYPE_SND) {
    sprintf(tmpname, SND_PLUGIN_ENTRY_FMT_STRING, name);
  } else {
    sprintf(tmpname, PLUGIN_ENTRY_FMT_STRING, name);
  }
#if defined(WIN32)
  plugin->plugin_entry = (plugin_entry_t) GetProcAddress(plugin->handle, tmpname);
  if (plugin->plugin_entry == NULL) {
    pluginlog->panic("could not find plugin_entry for module '%s' (%s): error=%d",
                     name, plugin_filename, GetLastError());
    plugin_abort(plugin);
    return 0;
  }
#else
  plugin->plugin_entry = (plugin_entry_t) lt_dlsym(plugin->handle, tmpname);
  if (plugin->plugin_entry == NULL) {
    pluginlog->panic("could not find plugin_entry for module '%s' (%s): %s",
                     name, plugin_filename, lt_dlerror());
    plugin_abort(plugin);
    return 0;
  }
#endif
  pluginlog->info("loaded plugin %s",plugin_filename);

  if (!plugin_init_one(plugin)) {
    return 0;
  }

  // check that context didn't change.  This should only happen if we
  // need a reentrant plugin_load.
  BX_ASSERT(current_plugin_context == plugin);
  current_plugin_context = NULL;
  return 1;
}

void plugin_abort(plugin_t *plugin)
{
  plugin_unload(plugin);
  pluginlog->panic("loading plugin '%s' aborted", plugin->name);
}

#endif   /* end of #if BX_PLUGINS */

/************************************************************************/
/* Plugin system: initialisation of plugins entry points                */
/************************************************************************/

void plugin_startup(void)
{
  pluginRegisterIRQ = builtinRegisterIRQ;
  pluginUnregisterIRQ = builtinUnregisterIRQ;

  pluginSetHRQHackCallback = builtinSetHRQHackCallback;
  pluginSetHRQ = builtinSetHRQ;

  pluginRegisterIOReadHandler = builtinRegisterIOReadHandler;
  pluginRegisterIOWriteHandler = builtinRegisterIOWriteHandler;

  pluginUnregisterIOReadHandler = builtinUnregisterIOReadHandler;
  pluginUnregisterIOWriteHandler = builtinUnregisterIOWriteHandler;

  pluginRegisterIOReadHandlerRange = builtinRegisterIOReadHandlerRange;
  pluginRegisterIOWriteHandlerRange = builtinRegisterIOWriteHandlerRange;

  pluginUnregisterIOReadHandlerRange = builtinUnregisterIOReadHandlerRange;
  pluginUnregisterIOWriteHandlerRange = builtinUnregisterIOWriteHandlerRange;

  pluginRegisterDefaultIOReadHandler = builtinRegisterDefaultIOReadHandler;
  pluginRegisterDefaultIOWriteHandler = builtinRegisterDefaultIOWriteHandler;

  pluginlog = new logfunctions();
  pluginlog->put("PLUGIN");
#if BX_PLUGINS
#if !defined(WIN32)
  int status = lt_dlinit();
  if (status != 0) {
    BX_ERROR(("initialization error in ltdl library (for loading plugins)"));
    BX_PANIC(("error message was: %s", lt_dlerror()));
  }
#endif
  plugins_search();
#endif
}

#if !BX_PLUGINS
void plugin_cleanup_np(void);
#endif

void plugin_cleanup(void)
{
  // Unload each loaded device plugin first
  if (devices != NULL) {
    bx_unload_plugins();
  }
#if BX_PLUGINS
  plugin_t *dead_plug;

  // delete plugin database and unload non-device plugins
  while (plugins != NULL) {
    if (plugins->loadtype != PLUGTYPE_NULL) {
      plugin_unload(plugins);
    }
    delete [] plugins->name;

    dead_plug = plugins;
    plugins = plugins->next;
    delete dead_plug;
  }
#else
  plugin_cleanup_np();
#endif
  delete pluginlog;
}


/************************************************************************/
/* Plugin system: Device registration                                   */
/************************************************************************/

void pluginRegisterDeviceDevmodel(plugin_t *plugin, Bit16u type, bx_devmodel_c *devmodel, const char *name)
{
  device_t **devlist;

  device_t *device = new device_t;

  device->name = name;
  BX_ASSERT(devmodel != NULL);
  device->devmodel = devmodel;
  device->plugin = plugin;  // this can be NULL
  device->next = NULL;
  device->plugtype = type;

  switch (type) {
    case PLUGTYPE_CORE:
    case PLUGTYPE_VGA:
      devlist = &core_devices;
      break;
    case PLUGTYPE_STANDARD:
    case PLUGTYPE_OPTIONAL:
    default:
      devlist = &devices;
      break;
  }

  if (!*devlist) {
    /* Empty list, this become the first entry. */
    *devlist = device;
  } else {
    /* Non-empty list.  Add to end. */
    device_t *temp = *devlist;

    while (temp->next)
      temp = temp->next;

    temp->next = device;
  }
}

/************************************************************************/
/* Plugin system: Remove registered plugin device                       */
/************************************************************************/

void pluginUnregisterDeviceDevmodel(const char *name, Bit16u type)
{
  device_t **devlist;
  device_t *device, *prev = NULL;

  switch (type) {
    case PLUGTYPE_CORE:
    case PLUGTYPE_VGA:
      devlist = &core_devices;
      break;
    case PLUGTYPE_STANDARD:
    case PLUGTYPE_OPTIONAL:
    default:
      devlist = &devices;
      break;
  }

  for (device = *devlist; device; device = device->next) {
    if (!strcmp(name, device->name)) {
      if (prev == NULL) {
        *devlist = device->next;
      } else {
        prev->next = device->next;
      }
      delete device;
      break;
    } else {
      prev = device;
    }
  }
}

/************************************************************************/
/* Plugin system: Check if a plugin is loaded                           */
/************************************************************************/

bool pluginDevicePresent(const char *name)
{
  device_t *device;

  for (device = devices; device; device = device->next)
  {
    if (!strcmp(name, device->name)) return 1;
  }

  return 0;
}

#if BX_PLUGINS
/************************************************************************/
/* Plugin system: Load one plugin                                       */
/************************************************************************/

bool bx_load_plugin(const char *name, Bit16u type)
{
  plugin_t *plugin;

  if (!strcmp(name, "*")) {
    for (plugin = plugins; plugin; plugin = plugin->next) {
      if (((type & plugin->type) != 0) && (plugin->loadtype == PLUGTYPE_NULL)) {
        plugin_load(plugin->name, type);
      }
    }
    return 1;
  } else {
    return plugin_load(name, type);
  }
}

bool bx_unload_plugin(const char *name, bool devflag)
{
  plugin_t *plugin;
  bool ret = 0;

  for (plugin = plugins; plugin; plugin = plugin->next) {
    if (!strcmp(plugin->name, name)) {
      if (devflag) {
        pluginUnregisterDeviceDevmodel(plugin->name, plugin->loadtype);
      }
      ret = plugin_unload(plugin);
      break;
    }
  }
  return ret;
}

void bx_unload_plugin_type(const char *name, Bit16u type)
{
  plugin_t *plugin;

  for (plugin = plugins; plugin; plugin = plugin->next) {
    if (!strcmp(plugin->name, name) && ((plugin->type & type) != 0)) {
      plugin_unload(plugin);
      break;
    }
  }
}

#endif   /* end of #if BX_PLUGINS */

/*************************************************************************/
/* Plugin system: Execute init function of all registered plugin-devices */
/*************************************************************************/

void bx_init_plugins()
{
  device_t *device;

  for (device = core_devices; device; device = device->next) {
    pluginlog->info("init_dev of '%s' plugin device by virtual method",device->name);
    device->devmodel->init();
  }
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_STANDARD) {
      pluginlog->info("init_dev of '%s' plugin device by virtual method",device->name);
      device->devmodel->init();
    }
  }
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_OPTIONAL) {
      pluginlog->info("init_dev of '%s' plugin device by virtual method",device->name);
      device->devmodel->init();
    }
  }
}

/**************************************************************************/
/* Plugin system: Execute reset function of all registered plugin-devices */
/**************************************************************************/

void bx_reset_plugins(unsigned signal)
{
  device_t *device;

  for (device = core_devices; device; device = device->next) {
    pluginlog->info("reset of '%s' plugin device by virtual method",device->name);
    device->devmodel->reset(signal);
  }
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_STANDARD) {
      pluginlog->info("reset of '%s' plugin device by virtual method",device->name);
      device->devmodel->reset(signal);
    }
  }
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_OPTIONAL) {
      pluginlog->info("reset of '%s' plugin device by virtual method",device->name);
      device->devmodel->reset(signal);
    }
  }
}

/*******************************************************/
/* Plugin system: Unload all registered plugin-devices */
/*******************************************************/

void bx_unload_plugins()
{
  device_t *device, *next;

  // unload non-core device plugins first
  device = devices;
  while (device != NULL) {
    if (device->plugin != NULL) {
#if BX_PLUGINS
      bx_unload_plugin(device->name, 0);
#endif
    } else {
#if !BX_PLUGINS
      if (!bx_unload_opt_plugin(device->name, 0)) {
        delete device->devmodel;
      }
#endif
    }
    next = device->next;
    delete device;
    device = next;
  }
  devices = NULL;

  // now it's safe to unload core device plugins
  device = core_devices;
  while (device != NULL) {
    if (device->plugin != NULL) {
#if BX_PLUGINS
      bx_unload_plugin(device->name, 0);
#endif
    } else {
#if !BX_PLUGINS
      if (!bx_unload_opt_plugin(device->name, 0)) {
        delete device->devmodel;
      }
#endif
    }
    next = device->next;
    delete device;
    device = next;
  }
  core_devices = NULL;
}

/**************************************************************************/
/* Plugin system: Register device state of all registered plugin-devices  */
/**************************************************************************/

void bx_plugins_register_state()
{
  device_t *device;

  for (device = core_devices; device; device = device->next) {
    pluginlog->info("register state of '%s' plugin device by virtual method",device->name);
    device->devmodel->register_state();
  }
  for (device = devices; device; device = device->next) {
    pluginlog->info("register state of '%s' plugin device by virtual method",device->name);
    device->devmodel->register_state();
  }
}

/***************************************************************************/
/* Plugin system: Execute code after restoring state of all plugin devices */
/***************************************************************************/

void bx_plugins_after_restore_state()
{
  device_t *device;

  for (device = core_devices; device; device = device->next) {
    device->devmodel->after_restore_state();
  }
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_STANDARD) {
      device->devmodel->after_restore_state();
    }
  }
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_OPTIONAL) {
      device->devmodel->after_restore_state();
    }
  }
}

#if !BX_PLUGINS

// Special code for handling modules when plugin support is turned off.

#define BUILTIN_OPT_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_OPTIONAL, 0, lib##mod##_plugin_entry, 0}
#define BUILTIN_OPTPCI_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_OPTIONAL, PLUGFLAG_PCI, lib##mod##_plugin_entry, 0}
#define BUILTIN_VGA_PLUGIN_ENTRY(mod, t, f) {#mod, PLUGTYPE_VGA | t, f, lib##mod##_plugin_entry, 0}
#define BUILTIN_CI_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_CI, 0, lib##mod##_plugin_entry, 0}
#define BUILTIN_GUI_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_GUI, 0, lib##mod##_gui_plugin_entry, 0}
#define BUILTIN_GUICI_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_GUI | PLUGTYPE_CI, 0, lib##mod##_gui_plugin_entry, 0}
#define BUILTIN_IMG_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_IMG, 0, lib##mod##_img_plugin_entry, 0}
#define BUILTIN_NET_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_NET, 0, libeth_##mod##_plugin_entry, 0}
#define BUILTIN_SND_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_SND, 0, libsound##mod##_plugin_entry, 0}
#define BUILTIN_USB_PLUGIN_ENTRY(mod) {#mod, PLUGTYPE_USB, 0, lib##mod##_plugin_entry, 0}

plugin_t bx_builtin_plugins[] = {
#if BX_USE_TEXTCONFIG
  BUILTIN_CI_PLUGIN_ENTRY(textconfig),
#endif
#if BX_USE_WIN32CONFIG
  BUILTIN_CI_PLUGIN_ENTRY(win32config),
#endif
#if BX_WITH_AMIGAOS
  BUILTIN_GUI_PLUGIN_ENTRY(amigaos),
#endif
#if BX_WITH_CARBON
  BUILTIN_GUI_PLUGIN_ENTRY(carbon),
#endif
#if BX_WITH_MACOS
  BUILTIN_GUI_PLUGIN_ENTRY(macos),
#endif
#if BX_WITH_NOGUI
  BUILTIN_GUI_PLUGIN_ENTRY(nogui),
#endif
#if BX_WITH_RFB
  BUILTIN_GUI_PLUGIN_ENTRY(rfb),
#endif
#if BX_WITH_SDL
  BUILTIN_GUI_PLUGIN_ENTRY(sdl),
#endif
#if BX_WITH_SDL2
  BUILTIN_GUI_PLUGIN_ENTRY(sdl2),
#endif
#if BX_WITH_TERM
  BUILTIN_GUI_PLUGIN_ENTRY(term),
#endif
#if BX_WITH_VNCSRV
  BUILTIN_GUI_PLUGIN_ENTRY(vncsrv),
#endif
#if BX_WITH_WIN32
  BUILTIN_GUI_PLUGIN_ENTRY(win32),
#endif
#if BX_WITH_WX
  BUILTIN_GUICI_PLUGIN_ENTRY(wx),
#endif
#if BX_WITH_X11
  BUILTIN_GUI_PLUGIN_ENTRY(x),
#endif
  BUILTIN_VGA_PLUGIN_ENTRY(vga, 0, PLUGFLAG_PCI),
#if BX_SUPPORT_CLGD54XX
  BUILTIN_VGA_PLUGIN_ENTRY(svga_cirrus, 0, PLUGFLAG_PCI),
#endif
#if BX_SUPPORT_VOODOO
  BUILTIN_VGA_PLUGIN_ENTRY(voodoo, PLUGTYPE_OPTIONAL, PLUGFLAG_PCI),
#endif
  BUILTIN_OPT_PLUGIN_ENTRY(unmapped),
  BUILTIN_OPT_PLUGIN_ENTRY(biosdev),
  BUILTIN_OPT_PLUGIN_ENTRY(speaker),
  BUILTIN_OPT_PLUGIN_ENTRY(extfpuirq),
  BUILTIN_OPT_PLUGIN_ENTRY(parallel),
  BUILTIN_OPT_PLUGIN_ENTRY(serial),
#if BX_SUPPORT_BUSMOUSE
  BUILTIN_OPT_PLUGIN_ENTRY(busmouse),
#endif
#if BX_SUPPORT_E1000
  BUILTIN_OPTPCI_PLUGIN_ENTRY(e1000),
#endif
#if BX_SUPPORT_ES1370
  BUILTIN_OPTPCI_PLUGIN_ENTRY(es1370),
#endif
#if BX_SUPPORT_GAMEPORT
  BUILTIN_OPT_PLUGIN_ENTRY(gameport),
#endif
#if BX_SUPPORT_IODEBUG
  BUILTIN_OPT_PLUGIN_ENTRY(iodebug),
#endif
#if BX_SUPPORT_NE2K
  BUILTIN_OPTPCI_PLUGIN_ENTRY(ne2k),
#endif
#if BX_SUPPORT_PCIDEV
  BUILTIN_OPTPCI_PLUGIN_ENTRY(pcidev),
#endif
#if BX_SUPPORT_PCIPNIC
  BUILTIN_OPTPCI_PLUGIN_ENTRY(pcipnic),
#endif
#if BX_SUPPORT_SB16
  BUILTIN_OPT_PLUGIN_ENTRY(sb16),
#endif
#if BX_SUPPORT_USB_UHCI
  BUILTIN_OPTPCI_PLUGIN_ENTRY(usb_uhci),
#endif
#if BX_SUPPORT_USB_OHCI
  BUILTIN_OPTPCI_PLUGIN_ENTRY(usb_ohci),
#endif
#if BX_SUPPORT_USB_EHCI
  BUILTIN_OPTPCI_PLUGIN_ENTRY(usb_ehci),
#endif
#if BX_SUPPORT_USB_XHCI
  BUILTIN_OPTPCI_PLUGIN_ENTRY(usb_xhci),
#endif
#if BX_SUPPORT_SOUNDLOW
  BUILTIN_SND_PLUGIN_ENTRY(dummy),
  BUILTIN_SND_PLUGIN_ENTRY(file),
#if BX_HAVE_SOUND_ALSA
  BUILTIN_SND_PLUGIN_ENTRY(alsa),
#endif
#if BX_HAVE_SOUND_OSS
  BUILTIN_SND_PLUGIN_ENTRY(oss),
#endif
#if BX_HAVE_SOUND_OSX
  BUILTIN_SND_PLUGIN_ENTRY(osx),
#endif
#if BX_HAVE_SOUND_SDL
  BUILTIN_SND_PLUGIN_ENTRY(sdl),
#endif
#if BX_HAVE_SOUND_WIN
  BUILTIN_SND_PLUGIN_ENTRY(win),
#endif
#endif
#if BX_NETWORKING
  BUILTIN_NET_PLUGIN_ENTRY(null),
  BUILTIN_NET_PLUGIN_ENTRY(vnet),
#if BX_NETMOD_FBSD
  BUILTIN_NET_PLUGIN_ENTRY(fbsd),
#endif
#if BX_NETMOD_LINUX
  BUILTIN_NET_PLUGIN_ENTRY(linux),
#endif
#if BX_NETMOD_SLIRP
  BUILTIN_NET_PLUGIN_ENTRY(slirp),
#endif
#if BX_NETMOD_SOCKET
  BUILTIN_NET_PLUGIN_ENTRY(socket),
#endif
#if BX_NETMOD_TAP
  BUILTIN_NET_PLUGIN_ENTRY(tap),
#endif
#if BX_NETMOD_TUNTAP
  BUILTIN_NET_PLUGIN_ENTRY(tuntap),
#endif
#if BX_NETMOD_VDE
  BUILTIN_NET_PLUGIN_ENTRY(vde),
#endif
#if BX_NETMOD_WIN32
  BUILTIN_NET_PLUGIN_ENTRY(win32),
#endif
#endif
#if BX_SUPPORT_PCIUSB
  BUILTIN_USB_PLUGIN_ENTRY(usb_floppy),
  BUILTIN_USB_PLUGIN_ENTRY(usb_hid),
  BUILTIN_USB_PLUGIN_ENTRY(usb_hub),
  BUILTIN_USB_PLUGIN_ENTRY(usb_msd),
  BUILTIN_USB_PLUGIN_ENTRY(usb_printer),
#endif
  BUILTIN_IMG_PLUGIN_ENTRY(vmware3),
  BUILTIN_IMG_PLUGIN_ENTRY(vmware4),
  BUILTIN_IMG_PLUGIN_ENTRY(vbox),
  BUILTIN_IMG_PLUGIN_ENTRY(vpc),
  BUILTIN_IMG_PLUGIN_ENTRY(vvfat),
  {"NULL", PLUGTYPE_NULL, 0, NULL, 0}
};

Bit8u bx_get_plugins_count_np(Bit16u type)
{
  int i = 0;
  Bit8u count = 0;

  while (strcmp(bx_builtin_plugins[i].name, "NULL")) {
    if ((type & bx_builtin_plugins[i].type) != 0)
      count++;
    i++;
  }
  return count;
}

const char* bx_get_plugin_name_np(Bit16u type, Bit8u index)
{
  int i = 0;
  Bit8u count = 0;

  while (strcmp(bx_builtin_plugins[i].name, "NULL")) {
    if ((type & bx_builtin_plugins[i].type) != 0) {
      if (count == index)
        return bx_builtin_plugins[i].name;
      count++;
    }
    i++;
  }
  return NULL;
}

Bit8u bx_get_plugin_flags_np(Bit16u type, Bit8u index)
{
  int i = 0;
  Bit8u count = 0;

  while (strcmp(bx_builtin_plugins[i].name, "NULL")) {
    if ((type & bx_builtin_plugins[i].type) != 0) {
      if (count == index)
        return bx_builtin_plugins[i].flags;
      count++;
    }
    i++;
  }
  return 0;
}

int bx_load_plugin_np(const char *name, Bit16u type)
{
  int i = 0;
  while (strcmp(bx_builtin_plugins[i].name, "NULL")) {
    if ((!strcmp(name, bx_builtin_plugins[i].name)) &&
        ((type & bx_builtin_plugins[i].type) != 0)) {
      if (bx_builtin_plugins[i].initialized == 0) {
        bx_builtin_plugins[i].loadtype = type;
        bx_builtin_plugins[i].plugin_entry(NULL, type, PLUGIN_INIT);
        bx_builtin_plugins[i].initialized = 1;
      } else {
        BX_PANIC(("plugin '%s' already loaded", name));
      }
      return 1;
    }
    i++;
  }
  return 0;
}

int bx_unload_opt_plugin(const char *name, bool devflag)
{
  int i = 0;
  while (strcmp(bx_builtin_plugins[i].name, "NULL")) {
    if ((!strcmp(name, bx_builtin_plugins[i].name)) &&
        (((bx_builtin_plugins[i].type & PLUGTYPE_OPTIONAL) != 0) ||
         ((bx_builtin_plugins[i].type & PLUGTYPE_VGA) != 0))) {
      if (bx_builtin_plugins[i].initialized == 1) {
        if (devflag) {
          pluginUnregisterDeviceDevmodel(bx_builtin_plugins[i].name,
                                         bx_builtin_plugins[i].type);
        }
        bx_builtin_plugins[i].plugin_entry(NULL, bx_builtin_plugins[i].loadtype, PLUGIN_FINI);
        bx_builtin_plugins[i].loadtype = PLUGTYPE_NULL;
        bx_builtin_plugins[i].initialized = 0;
      }
      return 1;
    }
    i++;
  }
  return 0;
}

void plugin_cleanup_np(void)
{
  int i = 0;
  while (strcmp(bx_builtin_plugins[i].name, "NULL")) {
    if (bx_builtin_plugins[i].initialized == 1) {
      bx_builtin_plugins[i].plugin_entry(NULL, bx_builtin_plugins[i].loadtype, PLUGIN_FINI);
      bx_builtin_plugins[i].loadtype = PLUGTYPE_NULL;
      bx_builtin_plugins[i].initialized = 0;
    }
    i++;
  }
}

#endif

}
