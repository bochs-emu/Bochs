/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2013  The Bochs Project
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

#define LOG_THIS genlog->

#define PLUGIN_INIT_FMT_STRING  "lib%s_LTX_plugin_init"
#define PLUGIN_FINI_FMT_STRING  "lib%s_LTX_plugin_fini"
#define PLUGIN_PATH ""

#ifndef WIN32
#define PLUGIN_FILENAME_FORMAT "libbx_%s.so"
#else
#define PLUGIN_FILENAME_FORMAT "bx_%s.dll"
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
int (*pluginRegisterTimer)(void *this_ptr, void (*funct)(void *),
                            Bit32u useconds, bx_bool continuous,
bx_bool active, const char* name) = 0;
                            void (*pluginActivateTimer)(unsigned id, Bit32u usec, bx_bool continuous) = 0;

void (*pluginHRQHackCallback)(void);
unsigned pluginHRQ = 0;

plugin_t *plugins = NULL;      /* Head of the linked list of plugins  */
#if BX_PLUGINS
static void plugin_init_one(plugin_t *plugin);
#endif

device_t *devices = NULL;      /* Head of the linked list of registered devices  */
device_t *core_devices = NULL; /* Head of the linked list of registered core devices  */

plugin_t *current_plugin_context = NULL;

/************************************************************************/
/* Builtins declarations                                                */
/************************************************************************/

  static void
builtinRegisterIRQ(unsigned irq, const char* name)
{
#if 0
  pluginlog->panic("builtinRegisterIRQ called, no pic plugin loaded?");
#else
  bx_devices.register_irq(irq, name);
#endif
}

  static void
builtinUnregisterIRQ(unsigned irq, const char* name)
{
#if 0
  pluginlog->panic("builtinUnregisterIRQ called, no pic plugin loaded?");
#else
  bx_devices.unregister_irq(irq, name);
#endif
}

  static void
builtinSetHRQ(unsigned val)
{
#if 0
  pluginlog->panic("builtinSetHRQ called, no plugin loaded?");
#else
  pluginHRQ = val;
#endif
}

  static void
builtinSetHRQHackCallback(void (*callback)(void))
{
#if 0
  pluginlog->panic("builtinSetHRQHackCallback called, no plugin loaded?");
#else
  pluginHRQHackCallback = callback;
#endif
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

  static int
builtinRegisterTimer(void *this_ptr, void (*funct)(void *),
                        Bit32u useconds, bx_bool continuous,
                        bx_bool active, const char* name)
{
  int id = bx_pc_system.register_timer (this_ptr, funct, useconds, continuous, active, name);
  pluginlog->ldebug("plugin %s registered timer %d", name, id);
  return id;
}

  static void
builtinActivateTimer(unsigned id, Bit32u usec, bx_bool continuous)
{
  bx_pc_system.activate_timer (id, usec, continuous);
  pluginlog->ldebug("plugin activated timer %d", id);
}

#if BX_PLUGINS
/************************************************************************/
/* Plugin initialization / deinitialization                             */
/************************************************************************/

void plugin_init_all (void)
{
  plugin_t *plugin;

  pluginlog->info("Initializing plugins");

  for (plugin = plugins; plugin; plugin = plugin->next)
  {
    char *arg_ptr = plugin->args;

    /* process the command line */
    plugin->argc = 0;
    while (plugin->argc < MAX_ARGC)
    {
      while (*arg_ptr && isspace (*arg_ptr))
        arg_ptr++;

      if (!*arg_ptr) break;
      plugin->argv[plugin->argc++] = arg_ptr;

      while (*arg_ptr && !isspace (*arg_ptr))
        arg_ptr++;

      if (!*arg_ptr) break;
      *arg_ptr++ = '\0';
    }

    /* initialize the plugin */
    if (plugin->plugin_init (plugin, plugin->type, plugin->argc, plugin->argv))
    {
      pluginlog->panic("Plugin initialization failed for %s", plugin->name);
      plugin_abort();
    }

    plugin->initialized = 1;
  }
}

void plugin_init_one(plugin_t *plugin)
{
  char *arg_ptr = plugin->args;

  /* process the command line */
  plugin->argc = 0;
  while (plugin->argc < MAX_ARGC)
  {
    while (*arg_ptr && isspace (*arg_ptr))
      arg_ptr++;

    if (!*arg_ptr) break;
    plugin->argv[plugin->argc++] = arg_ptr;

    while (*arg_ptr && !isspace (*arg_ptr))
      arg_ptr++;

    if (!*arg_ptr) break;
    *arg_ptr++ = '\0';
  }

  /* initialize the plugin */
  if (plugin->plugin_init (plugin, plugin->type, plugin->argc, plugin->argv))
  {
    pluginlog->info("Plugin initialization failed for %s", plugin->name);
    plugin_abort();
  }

  plugin->initialized = 1;
}


plugin_t *plugin_unload(plugin_t *plugin)
{
  plugin_t *dead_plug;

  if (plugin->initialized)
      plugin->plugin_fini();

#if defined(WIN32)
  FreeLibrary(plugin->handle);
#else
  lt_dlclose(plugin->handle);
#endif
  delete [] plugin->name;

  dead_plug = plugin;
  plugin = plugin->next;
  free(dead_plug);

  return plugin;
}

void plugin_fini_all (void)
{
  plugin_t *plugin;

  for (plugin = plugins; plugin; plugin = plugin_unload(plugin));
}

void plugin_load(char *name, char *args, plugintype_t type)
{
  plugin_t *plugin, *temp;
#if defined(WIN32)
  char dll_path_list[MAX_PATH];
#endif

  if (plugins != NULL) {
    temp = plugins;

    while (temp != NULL) {
      if (!strcmp(name, temp->name)) {
        BX_PANIC(("plugin '%s' already loaded", name));
        return;
      }
      temp = temp->next;
    }
  }

  plugin = (plugin_t *)malloc(sizeof(plugin_t));
  if (!plugin) {
    BX_PANIC(("malloc plugin_t failed"));
  }

  plugin->type = type;
  plugin->name = name;
  plugin->args = args;
  plugin->initialized = 0;

  char plugin_filename[BX_PATHNAME_LEN], tmpname[BX_PATHNAME_LEN];
  sprintf(tmpname, PLUGIN_FILENAME_FORMAT, name);
  sprintf(plugin_filename, "%s%s", PLUGIN_PATH, tmpname);

  // Set context so that any devices that the plugin registers will
  // be able to see which plugin created them.  The registration will
  // be called from either dlopen (global constructors) or plugin_init.
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
    BX_PANIC(("LoadLibrary failed for module '%s': error=%d", name, GetLastError()));
    free(plugin);
    return;
  }
#else
  plugin->handle = lt_dlopen(plugin_filename);
  BX_INFO(("lt_dlhandle is %p", plugin->handle));
  if (!plugin->handle) {
    current_plugin_context = NULL;
    BX_PANIC(("dlopen failed for module '%s': %s", name, lt_dlerror()));
    free(plugin);
    return;
  }
#endif

  if (type != PLUGTYPE_USER) {
    sprintf(tmpname, PLUGIN_INIT_FMT_STRING, name);
  } else {
    sprintf(tmpname, PLUGIN_INIT_FMT_STRING, "user");
  }
#if defined(WIN32)
  plugin->plugin_init = (plugin_init_t) GetProcAddress(plugin->handle, tmpname);
  if (plugin->plugin_init == NULL) {
    pluginlog->panic("could not find plugin_init: error=%d", GetLastError());
    plugin_abort();
  }
#else
  plugin->plugin_init = (plugin_init_t) lt_dlsym(plugin->handle, tmpname);
  if (plugin->plugin_init == NULL) {
    pluginlog->panic("could not find plugin_init: %s", lt_dlerror());
    plugin_abort();
  }
#endif

  if (type != PLUGTYPE_USER) {
    sprintf(tmpname, PLUGIN_FINI_FMT_STRING, name);
  } else {
    sprintf(tmpname, PLUGIN_FINI_FMT_STRING, "user");
  }
#if defined(WIN32)
  plugin->plugin_fini = (plugin_fini_t) GetProcAddress(plugin->handle, tmpname);
  if (plugin->plugin_fini == NULL) {
    pluginlog->panic("could not find plugin_fini: error=%d", GetLastError());
    plugin_abort();
  }
#else
  plugin->plugin_fini = (plugin_fini_t) lt_dlsym(plugin->handle, tmpname);
  if (plugin->plugin_fini == NULL) {
    pluginlog->panic("could not find plugin_fini: %s", lt_dlerror());
    plugin_abort();
  }
#endif
  pluginlog->info("loaded plugin %s",plugin_filename);

  /* Insert plugin at the _end_ of the plugin linked list. */
  plugin->next = NULL;

  if (!plugins) {
    /* Empty list, this become the first entry. */
    plugins = plugin;
  } else {
   /* Non-empty list.  Add to end. */
   temp = plugins;

   while (temp->next)
      temp = temp->next;

    temp->next = plugin;
  }

  plugin_init_one(plugin);

  // check that context didn't change.  This should only happen if we
  // need a reentrant plugin_load.
  BX_ASSERT(current_plugin_context == plugin);
  current_plugin_context = NULL;
}

void plugin_abort(void)
{
  pluginlog->panic("plugin load aborted");
}

#endif   /* end of #if BX_PLUGINS */

/************************************************************************/
/* Plugin system: initialisation of plugins entry points                */
/************************************************************************/

  void
plugin_startup(void)
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

  pluginRegisterTimer = builtinRegisterTimer;
  pluginActivateTimer = builtinActivateTimer;

  pluginlog = new logfunctions();
  pluginlog->put("PLUGIN");
#if BX_PLUGINS && !defined(WIN32)
  int status = lt_dlinit();
  if (status != 0) {
    BX_ERROR(("initialization error in ltdl library (for loading plugins)"));
    BX_PANIC(("error message was: %s", lt_dlerror()));
  }
#endif
}


/************************************************************************/
/* Plugin system: Device registration                                   */
/************************************************************************/

void pluginRegisterDeviceDevmodel(plugin_t *plugin, plugintype_t type, bx_devmodel_c *devmodel, const char *name)
{
  device_t *device, **devlist;

  device = (device_t *)malloc(sizeof(device_t));
  if (!device)
  {
    pluginlog->panic("can't allocate device_t");
  }

  device->name = name;
  BX_ASSERT(devmodel != NULL);
  device->devmodel = devmodel;
  device->plugin = plugin;  // this can be NULL
  device->next = NULL;
  device->plugtype = type;

  switch (type) {
    case PLUGTYPE_CORE:
      devlist = &core_devices;
      break;
    case PLUGTYPE_STANDARD:
    case PLUGTYPE_OPTIONAL:
    case PLUGTYPE_USER:
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

void pluginUnregisterDeviceDevmodel(const char *name)
{
  device_t *device, *prev = NULL;

  for (device = devices; device; device = device->next) {
    if (!strcmp(name, device->name)) {
      if (prev == NULL) {
        devices = device->next;
      } else {
        prev->next = device->next;
      }
      free(device);
      break;
    } else {
      prev = device;
    }
  }
}

/************************************************************************/
/* Plugin system: Check if a plugin is loaded                           */
/************************************************************************/

bx_bool pluginDevicePresent(const char *name)
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

int bx_load_plugin(const char *name, plugintype_t type)
{
  char *namecopy = new char[1+strlen(name)];
  strcpy(namecopy, name);
  plugin_load(namecopy, (char*)"", type);
  return 1;
}

void bx_unload_plugin(const char *name, bx_bool devflag)
{
  plugin_t *plugin, *prev = NULL;

  for (plugin = plugins; plugin; plugin = plugin->next) {
    if (!strcmp(plugin->name, name)) {
      if (devflag) {
        pluginUnregisterDeviceDevmodel(plugin->name);
      }
      plugin = plugin_unload(plugin);
      if (prev == NULL) {
        plugins = plugin;
      } else {
        prev->next = plugin;
      }
      break;
    } else {
      prev = plugin;
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
#if BX_PLUGINS
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_USER) {
      pluginlog->info("init_dev of '%s' plugin device by virtual method",device->name);
      device->devmodel->init();
    }
  }
#endif
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
#if BX_PLUGINS
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_USER) {
      pluginlog->info("reset of '%s' plugin device by virtual method",device->name);
      device->devmodel->reset(signal);
    }
  }
#endif
}

/*******************************************************/
/* Plugin system: Unload all registered plugin-devices */
/*******************************************************/

void bx_unload_plugins()
{
  device_t *device, *next;

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
    free(device);
    device = next;
  }
  devices = NULL;
}

void bx_unload_core_plugins()
{
  device_t *device, *next;

  device = core_devices;
  while (device != NULL) {
    if (device->plugin != NULL) {
#if BX_PLUGINS
      bx_unload_plugin(device->name, 0);
#endif
    } else {
      delete device->devmodel;
    }
    next = device->next;
    free(device);
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
#if BX_PLUGINS
  for (device = devices; device; device = device->next) {
    if (device->plugtype == PLUGTYPE_USER) {
      device->devmodel->after_restore_state();
    }
  }
#endif
}

#if !BX_PLUGINS

// special code for loading optional plugins when plugins are turned off

typedef struct {
  const char *name;
  plugin_init_t plugin_init;
  plugin_fini_t plugin_fini;
  bx_bool       status;
} builtin_plugin_t;

#define BUILTIN_PLUGIN_ENTRY(mod) {#mod, lib##mod##_LTX_plugin_init, lib##mod##_LTX_plugin_fini, 0}

static builtin_plugin_t builtin_opt_plugins[] = {
  BUILTIN_PLUGIN_ENTRY(unmapped),
  BUILTIN_PLUGIN_ENTRY(biosdev),
  BUILTIN_PLUGIN_ENTRY(speaker),
  BUILTIN_PLUGIN_ENTRY(extfpuirq),
  BUILTIN_PLUGIN_ENTRY(parallel),
  BUILTIN_PLUGIN_ENTRY(serial),
#if BX_SUPPORT_E1000
  BUILTIN_PLUGIN_ENTRY(e1000),
#endif
#if BX_SUPPORT_ES1370
  BUILTIN_PLUGIN_ENTRY(es1370),
#endif
#if BX_SUPPORT_GAMEPORT
  BUILTIN_PLUGIN_ENTRY(gameport),
#endif
#if BX_SUPPORT_IODEBUG
  BUILTIN_PLUGIN_ENTRY(iodebug),
#endif
#if BX_SUPPORT_NE2K
  BUILTIN_PLUGIN_ENTRY(ne2k),
#endif
#if BX_SUPPORT_PCIDEV
  BUILTIN_PLUGIN_ENTRY(pcidev),
#endif
#if BX_SUPPORT_PCIPNIC
  BUILTIN_PLUGIN_ENTRY(pcipnic),
#endif
#if BX_SUPPORT_SB16
  BUILTIN_PLUGIN_ENTRY(sb16),
#endif
#if BX_SUPPORT_USB_OHCI
  BUILTIN_PLUGIN_ENTRY(usb_ohci),
#endif
#if BX_SUPPORT_USB_UHCI
  BUILTIN_PLUGIN_ENTRY(usb_uhci),
#endif
#if BX_SUPPORT_USB_XHCI
  BUILTIN_PLUGIN_ENTRY(usb_xhci),
#endif
#if BX_SUPPORT_VOODOO
  BUILTIN_PLUGIN_ENTRY(voodoo),
#endif
  {"NULL", NULL, NULL, 0}
};

int bx_load_opt_plugin(const char *name)
{
  int i = 0;
  while (strcmp(builtin_opt_plugins[i].name, "NULL")) {
    if (!strcmp(name, builtin_opt_plugins[i].name)) {
      if (builtin_opt_plugins[i].status == 0) {
        builtin_opt_plugins[i].plugin_init(NULL, PLUGTYPE_OPTIONAL, 0, NULL);
        builtin_opt_plugins[i].status = 1;
      }
      return 1;
    }
    i++;
  };
  return 0;
}

int bx_unload_opt_plugin(const char *name, bx_bool devflag)
{
  int i = 0;
  while (strcmp(builtin_opt_plugins[i].name, "NULL")) {
    if (!strcmp(name, builtin_opt_plugins[i].name)) {
      if (builtin_opt_plugins[i].status == 1) {
        if (devflag) {
          pluginUnregisterDeviceDevmodel(builtin_opt_plugins[i].name);
        }
        builtin_opt_plugins[i].plugin_fini();
        builtin_opt_plugins[i].status = 0;
      }
      return 1;
    }
    i++;
  };
  return 0;
}

#endif

}
