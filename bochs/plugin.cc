/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2000  The plex86 developers team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include "bochs.h"
#include "plugin.h"

#define LOG_THIS genlog->

#define PLUGIN_INIT  "plugin_init"
#define PLUGIN_FINI  "plugin_fini"


void  (*pluginRegisterIRQ)(unsigned irq, const char* name) = 0;
void  (*pluginUnregisterIRQ)(unsigned irq, const char* name) = 0;

void  (* pluginResetSignal)(unsigned sig) = 0;

void (*pluginSetHRQ)(unsigned val) = 0;
void (*pluginSetHRQHackCallback)( void (*callback)(void) ) = 0;

int (*pluginRegisterIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, const char *name, unsigned len) = 0;
int (*pluginRegisterIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, const char *name, unsigned len) = 0;
int (*pluginRegisterDefaultIOReadHandler)(void *thisPtr, ioReadHandler_t callback,
                            const char *name, unsigned len) = 0;
int (*pluginRegisterDefaultIOWriteHandler)(void *thisPtr, ioWriteHandler_t callback,
                             const char *name, unsigned len) = 0;
int (*pluginRegisterTimer)(void *this_ptr, void (*funct)(void *),
			Bit32u useconds, Boolean continuous, 
			Boolean active, const char* name) = 0;
void (*pluginActivateTimer)(unsigned id, Bit32u usec, Boolean continuous) = 0;

void (*pluginHRQHackCallback)(void);
unsigned pluginHRQ = 0;

plugin_t *plugins = NULL;      /* Head of the linked list of plugins  */
static void plugin_init_one(plugin_t *plugin);

device_t *devices = NULL;      /* Head of the linked list of registered devices  */

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
builtinSetHRQHackCallback( void (*callback)(void) )
{
#if 0
  pluginlog->panic("builtinSetHRQHackCallback called, no plugin loaded?");
#else
  pluginHRQHackCallback = callback;
#endif
}

  static void
builtinResetSignal( unsigned )
{
  pluginlog->panic("builtinResetSignal called, no plugin loaded?");
}

  static int
builtinRegisterIOReadHandler(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, const char *name, unsigned len)
{
  BX_ASSERT (len<8);
  bx_devices.register_io_read_handler (thisPtr, callback, base, name);
  pluginlog->ldebug("plugin %s registered I/O read address at %04x", name, base);
  return 0;
}

  static int
builtinRegisterIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, const char *name, unsigned len)
{
  BX_ASSERT (len<8);
  bx_devices.register_io_write_handler (thisPtr, callback, base, name);
  pluginlog->ldebug("plugin %s registered I/O write address at %04x", name, base);
  return 0;
}

  static int
builtinRegisterDefaultIOReadHandler(void *thisPtr, ioReadHandler_t callback,
                            const char *name, unsigned len)
{
  BX_ASSERT (len<8);
  bx_devices.register_default_io_read_handler (thisPtr, callback, name);
  pluginlog->ldebug("plugin %s registered default I/O read ", name);
  return 0;
}

  static int
builtinRegisterDefaultIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                             const char *name, unsigned len)
{
  BX_ASSERT (len<8);
  bx_devices.register_default_io_write_handler (thisPtr, callback, name);
  pluginlog->ldebug("plugin %s registered default I/O write ", name);
  return 0;
}

  static int
builtinRegisterTimer(void *this_ptr, void (*funct)(void *),
			Bit32u useconds, Boolean continuous, 
			Boolean active, const char* name)
{
  int id = bx_pc_system.register_timer (this_ptr, funct, useconds, continuous, active, name);
  pluginlog->ldebug("plugin %s registered timer %d", name, id);
  return id;
}

  static void
builtinActivateTimer(unsigned id, Bit32u usec, Boolean continuous)
{
  bx_pc_system.activate_timer (id, usec, continuous);
  pluginlog->ldebug("plugin activated timer %d", id);
}

#if BX_PLUGINS
/************************************************************************/
/* Plugin initialization / deinitialization                             */
/************************************************************************/

  void
plugin_init_all (void)
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

            if (!*arg_ptr)
                break;
            plugin->argv[plugin->argc++] = arg_ptr;

            while (*arg_ptr && !isspace (*arg_ptr))
                arg_ptr++;

            if (!*arg_ptr)
                break;
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

    return;
}

void
plugin_init_one(plugin_t *plugin)
{
        char *arg_ptr = plugin->args;
 
        /* process the command line */
        plugin->argc = 0;
        while (plugin->argc < MAX_ARGC)
        {
            while (*arg_ptr && isspace (*arg_ptr))
                arg_ptr++;
 
            if (!*arg_ptr)
                break;
            plugin->argv[plugin->argc++] = arg_ptr;
 
            while (*arg_ptr && !isspace (*arg_ptr))
                arg_ptr++;
 
            if (!*arg_ptr)
                break;
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


  plugin_t *
plugin_unload(plugin_t *plugin)
{
    plugin_t *dead_plug;

    if (plugin->initialized)
        plugin->plugin_fini ();

    lt_dlclose (plugin->handle);
    free (plugin->name);
    free (plugin->args);

    dead_plug = plugin;
    plugin = plugin->next;
    free (dead_plug);

    return plugin;
}


void
plugin_fini_all (void)
{
    plugin_t *plugin;

    for (plugin = plugins; plugin; plugin = plugin_unload (plugin));

    return;
}

  void
plugin_load (char *name, char *args, plugintype_t type)
{
    plugin_t *plugin;

    plugin = (plugin_t *)malloc (sizeof (plugin_t));
    if (!plugin)
    {
      BX_PANIC (("malloc plugin_t failed"));
    }

    plugin->type = type;
    plugin->name = name;
    plugin->args = args;
    plugin->initialized = 0;

    // Set context so that any devices that the plugin registers will
    // be able to see which plugin created them.  The registration will
    // be called from either dlopen (global constructors) or plugin_init.
    BX_ASSERT (current_plugin_context == NULL);
    current_plugin_context = plugin;
    plugin->handle = lt_dlopen (name);
    BX_INFO (("lt_dlhandle is %p", plugin->handle));
    if (!plugin->handle)
    {
      current_plugin_context = NULL;
      BX_PANIC (("dlopen failed for module '%s': %s", name, lt_dlerror ()));
      free (plugin);
      return;
    }

    plugin->plugin_init =  
      (int  (*)(struct _plugin_t *, enum plugintype_t, int, char *[])) /* monster typecast */
      lt_dlsym (plugin->handle, PLUGIN_INIT);
    if (plugin->plugin_init == NULL) {
        pluginlog->panic("could not find plugin_init: %s", lt_dlerror ());
        plugin_abort ();
    }

    plugin->plugin_fini = (void (*)(void)) lt_dlsym (plugin->handle, PLUGIN_FINI);
    if (plugin->plugin_init == NULL) {
        pluginlog->panic("could not find plugin_fini: %s", lt_dlerror ());
        plugin_abort ();
    }


    /* Insert plugin at the _end_ of the plugin linked list. */
    plugin->next = NULL;

    if (!plugins)
    {
        /* Empty list, this become the first entry. */
        plugins = plugin;
    }
    else
    {
        /* Non-empty list.  Add to end. */
        plugin_t *temp = plugins;

        while (temp->next)
            temp = temp->next;

        temp->next = plugin;
    }

    plugin_init_one(plugin);

    // check that context didn't change.  This should only happen if we
    // need a reentrant plugin_load.
    BX_ASSERT (current_plugin_context == plugin);
    current_plugin_context = NULL;

    return;
}

void
plugin_abort (void)
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

  pluginResetSignal = builtinResetSignal;

  pluginSetHRQHackCallback = builtinSetHRQHackCallback;
  pluginSetHRQ = builtinSetHRQ;
  
  pluginRegisterIOReadHandler = builtinRegisterIOReadHandler;
  pluginRegisterIOWriteHandler = builtinRegisterIOWriteHandler;

  pluginRegisterDefaultIOReadHandler = builtinRegisterDefaultIOReadHandler;
  pluginRegisterDefaultIOWriteHandler = builtinRegisterDefaultIOWriteHandler;

  pluginRegisterTimer = builtinRegisterTimer;
  pluginActivateTimer = builtinActivateTimer;

#if BX_PLUGINS
  int status = lt_dlinit ();
  if (status != 0) {
    BX_ERROR (("initialization error in ltdl library (for loading plugins)"));
    BX_PANIC (("error message was: %s", lt_dlerror ()));
  }
#endif
}


/************************************************************************/
/* Plugin system: Device registration                                   */
/************************************************************************/

#warning BBD: when all plugin devices are converted to the "bx_devmodel" type with virtual functions, I intend to chop this out.
// (and the nasty current_plugin_context hack can go too)

void pluginRegisterDevice(deviceInitMem_t init1, deviceInitDev_t init2,
			  deviceReset_t reset, deviceLoad_t load, 
			  deviceSave_t save, char *name)
{
    device_t *device;

    device = (device_t *)malloc (sizeof (device_t));
    if (!device)
    {
        pluginlog->panic("can't allocate device_t");
    }

    device->name = name;
    BX_ASSERT(current_plugin_context != NULL);
    device->plugin = current_plugin_context;
    device->use_devmodel_interface = 0;
    device->device_init_mem = init1;
    device->device_init_dev = init2;
    device->device_reset = reset;
    device->device_load_state = load;
    device->device_save_state = save;
    device->next = NULL;

    // Don't add every kind of device to the list.
    switch (device->plugin->type)
    {
      case PLUGTYPE_CORE:
	// Core devices are present whether or not we are using plugins, so
	// they are managed by code iodev/devices.cc and should not be
	// managed by the plugin system.
	return; // Do not add core devices to the devices list.
      case PLUGTYPE_OPTIONAL:
      case PLUGTYPE_USER:
      default:
	// The plugin system will manage optional and user devices only.
	break;
    }

    if (!devices)
    {
        /* Empty list, this become the first entry. */
        devices = device;
    }
    else
    {
        /* Non-empty list.  Add to end. */
        device_t *temp = devices;

        while (temp->next)
            temp = temp->next;

        temp->next = device;
    }
}

void pluginRegisterDeviceDevmodel(plugin_t *plugin, plugintype_t type, bx_devmodel_c *devmodel, char *name)
{
    device_t *device;

    device = (device_t *)malloc (sizeof (device_t));
    if (!device)
    {
        pluginlog->panic("can't allocate device_t");
    }

    device->name = name;
    BX_ASSERT (devmodel != NULL);
    device->devmodel = devmodel;
    device->plugin = plugin;  // this can be NULL
    device->use_devmodel_interface = 1;
    device->device_init_mem = NULL;  // maybe should use 1 to detect any use?
    device->device_init_dev = NULL;
    device->device_reset = NULL;
    device->device_load_state = NULL;
    device->device_save_state = NULL;
    device->next = NULL;

    // Don't add every kind of device to the list.
    switch (type) {
      case PLUGTYPE_CORE:
	// Core devices are present whether or not we are using plugins, so
	// they are managed by the same code in iodev/devices.cc whether
	// plugins are on or off.  
	return; // Do not add core devices to the devices list.
      case PLUGTYPE_OPTIONAL:
      case PLUGTYPE_USER:
      default:
	// The plugin system will manage optional and user devices only.
	break;
    }

    if (!devices)
    {
        /* Empty list, this become the first entry. */
        devices = device;
    }
    else
    {
        /* Non-empty list.  Add to end. */
        device_t *temp = devices;

        while (temp->next)
            temp = temp->next;

        temp->next = device;
    }
}

/************************************************************************/
/* Plugin system: Check if a plugin is loaded                           */
/************************************************************************/

Boolean pluginDevicePresent(char *name)
{
    device_t *device;

    for (device = devices; device; device = device->next)
    {
      if (strcmp(device->name,name)==0) return true;
    }

    return false;
}

#if BX_PLUGINS
/************************************************************************/
/* Plugin system: Load one plugin                                       */
/************************************************************************/

#define PLUGIN_PATH ""
#define PLUGIN_FILENAME_FORMAT "lib%s.la"
int bx_load_plugin (const char *name, plugintype_t type)
{
  char plugin_filename[BX_PATHNAME_LEN], buf[BX_PATHNAME_LEN];
  sprintf (buf, PLUGIN_FILENAME_FORMAT, name);
  sprintf(plugin_filename, "%s%s", PLUGIN_PATH, buf);
  plugin_load (plugin_filename, "", type);
  lt_dlhandle handle = lt_dlopen (plugin_filename);
  if (!handle) {
    pluginlog->error("could not open plugin %s", plugin_filename);
    pluginlog->panic("dlopen error: %s", lt_dlerror ());
    return -1;
  }
  pluginlog->info("loaded plugin %s",plugin_filename);
  return 0;
}

/************************************************************************/
/* Plugin system: Load ALL plugins                                      */
/************************************************************************/

int bx_load_plugins (void)
{
  pluginlog = new logfunctions();
  pluginlog->put("PLGIN");
  pluginlog->settype(PLUGINLOG);

#if !BX_PLUGINS
  pluginlog->info("plugins deactivated");
#endif

  return 0;
}
#endif   /* end of #if BX_PLUGINS */

/*************************************************************************/
/* Plugin system: Execute init function of all registered plugin-devices */
/*************************************************************************/

void bx_init_plugins()
{
    device_t *device;

    // two loops
    for (device = devices; device; device = device->next)
    {
      if (!device->use_devmodel_interface) {
        if (device->device_init_mem != NULL) {
            pluginlog->info("init_mem of '%s' plugin device by function pointer",device->name);
            device->device_init_mem(BX_MEM(0));
	}
      } else {
	pluginlog->info("init_mem of '%s' plugin device by virtual method",device->name);
	device->devmodel->init_mem (BX_MEM(0));
      }
    }

    for (device = devices; device; device = device->next)
    {
      if (!device->use_devmodel_interface) {
        if (device->device_init_dev != NULL) {
            pluginlog->info("init_dev of '%s' plugin device by function pointer",device->name);
            device->device_init_dev();
	}
      } else {
	pluginlog->info("init_dev of '%s' plugin device by virtual method",device->name);
	device->devmodel->init ();
      }
    } 
}

/**************************************************************************/
/* Plugin system: Execute reset function of all registered plugin-devices */
/**************************************************************************/

void bx_reset_plugins(unsigned signal)
{
    device_t *device;
    for (device = devices; device; device = device->next)
    {
      if (!device->use_devmodel_interface) {
        if (device->device_reset != NULL) {
            pluginlog->info("reset of '%s' plugin device by function pointer",device->name);
            device->device_reset(signal);
        }
      } else {
	pluginlog->info("reset of '%s' plugin device by virtual method",device->name);
	device->devmodel->reset (signal);
      }
    }
}
