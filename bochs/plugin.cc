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

Bit32u   (* pluginGetCMOSReg)(unsigned reg) = 0;
void     (* pluginSetCMOSReg)(unsigned reg, Bit32u val) = 0;
void     (* pluginCMOSChecksum)(void) = 0;
time_t   (* pluginGetCMOSTimeval)(void) = 0;
void     (* pluginMouseMotion)(int d_x, int d_y, unsigned button_state) = 0;
void     (* pluginGenScancode)(Bit32u scancode) = 0;

unsigned (* pluginRegisterDMAChannel)(
                unsigned channel,
                void (* dma_read)(Bit8u *data_byte),
                void (* dma_write)(Bit8u *data_byte)
                ) = 0;
void     (* pluginDMASetDRQ)(unsigned channel, unsigned val) = 0;
unsigned (* pluginDMAGetTC)(void) = 0;

Bit32u   (* pluginHDReadHandler)(Bit32u address,
                unsigned io_len) = 0;
void     (* pluginHDWriteHandler)(Bit32u address,
                Bit32u value, unsigned io_len) = 0;

void     (* pluginVGARedrawArea)(unsigned x0, unsigned y0,
                unsigned width, unsigned height) = 0;
Bit8u    (* pluginVGAMemRead)(Bit32u addr) = 0;
void     (* pluginVGAMemWrite)(Bit32u addr, Bit8u value) = 0;
void     (* pluginVGAGetTextSnapshot)(Bit8u **text_snapshot, 
		     unsigned *txHeight, unsigned *txWidth) = 0;
void     (* pluginVGARefresh)(void) = 0;
void     (* pluginVGASetUpdateInterval)(unsigned) = 0;

unsigned (* pluginFloppyGetMediaStatus)(unsigned drive) = 0;
unsigned (* pluginFloppySetMediaStatus)(unsigned drive, unsigned status) = 0;

void  (*pluginRegisterIRQ)(unsigned irq, const char* name) = 0;
void  (*pluginUnregisterIRQ)(unsigned irq, const char* name) = 0;
void  (*pluginRaiseIRQ)(unsigned irq) = 0;
void  (*pluginLowerIRQ)(unsigned irq) = 0;

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

logfunctions  *pluginlog;


#if BX_PLUGINS
// When building with plugins, the bx_gui variable is created right
// here in plugins.cc, initialized to NULL.  When a gui plugin is loaded,
// it will create the appropriate type of gui object and set bx_gui
// in plugin_init.
bx_gui_c   *bx_gui = NULL;
#else
// When building without plugins, the bx_gui pointer will be created by
// the GUI code that is linked into the binary, for example x.cc or sdl.cc.  
// Do not define bx_gui here or you will get multiple definitions.
#endif


/************************************************************************/
/* Builtins declarations                                                */
/************************************************************************/

  static Bit32u
builtinGetCMOSReg(unsigned reg)
{
  pluginlog->panic("builtinGetCMOSReg called, not overloaded by CMOS plugin?");
  return 0;
}

  static void
builtinSetCMOSReg(unsigned reg, Bit32u val)
{
  pluginlog->panic("builtinSetCMOSReg called, not overloaded by CMOS plugin?");
  return;
}

  static void
builtinCMOSChecksum(void)
{
  pluginlog->panic("builtinCMOSChecksum called, no CMOS plugin loaded?");
}

  static time_t
builtinGetCMOSTimeval(void)
{
  pluginlog->panic("builtinbuiltinGetCMOSTimeval called, no CMOS plugin loaded?");
  return 0;
}
  static void
builtinMouseMotion(int d_x, int d_y, unsigned button_state)
{
  pluginlog->panic("builtinMouseMotion called, not overloaded by keyboard plugin?");
}

  static void
builtinGenScancode(Bit32u scancode)
{
  pluginlog->panic("builtinGenScancode called, not overloaded by keyboard plugin?");
}

  static unsigned
builtinRegisterDMAChannel(
  unsigned channel,
  void (* dma_read)(Bit8u *data_byte),
  void (* dma_write)(Bit8u *data_byte)
  )
{
  pluginlog->panic("builtinRegisterDMAChannel called, no DMA plugin loaded?");
  return 0;
}

  static void
builtinDMASetDRQ(unsigned channel, unsigned val)
{
  pluginlog->panic("builtinDMASetDRQ called, no DMA plugin loaded?");
}

  static unsigned
builtinDMAGetTC(void)
{
  pluginlog->panic("builtinDMAGetTC called, no DMA plugin loaded?");
  return 0;
}

  static Bit32u
builtinHDReadHandler(Bit32u address, unsigned io_len)
{
  pluginlog->panic("builtinHDReadHandler called, no Harddisk plugin loaded?");
  return 0;
}

  static void
builtinHDWriteHandler(Bit32u address, Bit32u value, unsigned io_len)
{
  pluginlog->panic("builtinHDWriteHandler called, no Harddisk plugin loaded?");
}

  static void
builtinVGARedrawArea(unsigned x0, unsigned y0,
                     unsigned width, unsigned height)
{
  pluginlog->panic("builtinVGARedrawArea called, no VGA plugin loaded?");
}

Bit8u builtinVGAMemRead(Bit32u addr)
{
  pluginlog->panic("builtinVGAMemRead called, no VGA plugin loaded?");
}

void  builtinVGAMemWrite(Bit32u addr, Bit8u value) 
{
  pluginlog->panic("builtinVGAMemWrite called, no VGA plugin loaded?");
}

void builtinVGAGetTextSnapshot(Bit8u **text_snapshot, 
		     unsigned *txHeight, unsigned *txWidth) 
{
  pluginlog->panic("builtinVGAGetTextSnapshot called, no VGA plugin loaded?");
}

void builtinVGARefresh(void)
{
  pluginlog->panic("builtinVGARefresh called, no VGA plugin loaded?");
}

void builtinVGASetUpdateInterval(unsigned val)
{
  pluginlog->panic("builtinVGASetUpdateInterval called, no VGA plugin loaded?");
}

static unsigned builtinFloppyGetMediaStatus(unsigned drive) 
{
  pluginlog->panic("builtinFloppyGetMediaStatus called, no floppy plugin loaded?");
  return 0;
}

static unsigned builtinFloppySetMediaStatus(unsigned drive, unsigned status) 
{
  pluginlog->panic("builtinFloppySetMediaStatus called, no floppy plugin loaded?");
}

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
builtinRaiseIRQ(unsigned irq)
{
#if 0
  pluginlog->panic("builtinRaiseIRQ called, no pic plugin loaded?");
#else
  bx_devices.pic->raise_irq(irq);
#endif
}

  static void
builtinLowerIRQ(unsigned irq)
{
#if 0
  pluginlog->panic("builtinLowerIRQ called, no pic plugin loaded?");
#else
  bx_devices.pic->lower_irq(irq);
#endif
}

  static void
builtinSetHRQ(unsigned val)
{
#if 0
  pluginlog->panic("builtinLowerIRQ called, no plugin loaded?");
#else
  pluginHRQ = val;
#endif
}

  static void
builtinSetHRQHackCallback( void (*callback)(void) )
{
#if 0
  pluginlog->panic("builtinLowerIRQ called, no plugin loaded?");
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
}

  static int
builtinRegisterIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, const char *name, unsigned len)
{
  BX_ASSERT (len<8);
  bx_devices.register_io_write_handler (thisPtr, callback, base, name);
  pluginlog->ldebug("plugin %s registered I/O write address at %04x", name, base);
}

  static int
builtinRegisterDefaultIOReadHandler(void *thisPtr, ioReadHandler_t callback,
                            const char *name, unsigned len)
{
  BX_ASSERT (len<8);
  bx_devices.register_default_io_read_handler (thisPtr, callback, name);
  pluginlog->ldebug("plugin %s registered default I/O read ", name);
}

  static int
builtinRegisterDefaultIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                             const char *name, unsigned len)
{
  BX_ASSERT (len<8);
  bx_devices.register_default_io_write_handler (thisPtr, callback, name);
  pluginlog->ldebug("plugin %s registered default I/O write ", name);
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

/************************************************************************/
/* Plugin initialization / deinitialization                             */
/************************************************************************/

  void
plugin_init_all (void)
{
    plugin_t *plugin;

    pluginlog->info("Initializing plugins", plugin->name);

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

#if 0
// BBD: plugin_init has already been called by plugin_load, so I commented
// this call.
        /* initialize the plugin */
        if (plugin->plugin_init (plugin, plugin->argc, plugin->argv))
        {
            pluginlog->panic("Plugin initialization failed for %s", plugin->name);
            plugin_abort();
        }
#endif

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
        if (plugin->plugin_init (plugin, plugin->argc, plugin->argv))
        {
            pluginlog->info("Plugin initialization failed for %s", plugin->name);
            plugin_abort();
        }
 
        plugin->initialized = 1;
}


  plugin_t *
plugin_unload(plugin_t *plugin)
{
    int i;
    plugin_t *dead_plug;

    if (plugin->initialized)
        plugin->plugin_fini ();

    dlclose (plugin->handle);
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
plugin_load (char *name, char *args)
{
    plugin_t *plugin;
    const char *plug_err;

    plugin = (plugin_t *)malloc (sizeof (plugin_t));
    if (!plugin)
    {
        perror ("malloc");
        exit (1);
    }

    plugin->name = name;
    plugin->args = args;
    plugin->initialized = 0;

    plugin->handle = dlopen (name, RTLD_LAZY);
    if (!plugin->handle)
    {
        fputs (dlerror (), stderr);
        exit (1);
    }

    plugin->plugin_init =  
      (int  (*)(struct _plugin_t *, int, char *[])) /* monster typecast */
      dlsym (plugin->handle, PLUGIN_INIT);
    if ((plug_err = dlerror ()) != NULL)
    {
        pluginlog->panic("could not find plugin_init: %s", plug_err);
        plugin_abort ();
    }

    plugin->plugin_fini = (void (*)(void)) dlsym (plugin->handle, PLUGIN_FINI);
    if ((plug_err = dlerror ()) != NULL)
    {
        pluginlog->panic("could not find plugin_fini: %s", plug_err);
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

    return;
}

void
plugin_abort (void)
{
    pluginlog->panic("plugin load aborted");
}


/************************************************************************/
/* Plugin system: initialisation of plugins entry points                */
/************************************************************************/

  void
plugin_startup(void)
{
  pluginGetCMOSReg   = builtinGetCMOSReg;
  pluginSetCMOSReg   = builtinSetCMOSReg;
  pluginCMOSChecksum = builtinCMOSChecksum;
  pluginGetCMOSTimeval  = builtinGetCMOSTimeval;
  pluginMouseMotion  = builtinMouseMotion;
  pluginGenScancode  = builtinGenScancode;
  pluginRegisterDMAChannel = builtinRegisterDMAChannel;
  pluginDMASetDRQ    = builtinDMASetDRQ;
  pluginDMAGetTC     = builtinDMAGetTC;

  pluginHDReadHandler  = builtinHDReadHandler;
  pluginHDWriteHandler = builtinHDWriteHandler;

  pluginVGARedrawArea  = builtinVGARedrawArea;
  pluginVGAMemRead     = builtinVGAMemRead;
  pluginVGAMemWrite    = builtinVGAMemWrite;
  pluginVGAGetTextSnapshot = builtinVGAGetTextSnapshot;
  pluginVGARefresh     = builtinVGARefresh;
  pluginVGASetUpdateInterval = builtinVGASetUpdateInterval;

  pluginFloppyGetMediaStatus = builtinFloppyGetMediaStatus;
  pluginFloppySetMediaStatus = builtinFloppySetMediaStatus;

  pluginRegisterIRQ = builtinRegisterIRQ;
  pluginUnregisterIRQ = builtinUnregisterIRQ;
  pluginRaiseIRQ = builtinRaiseIRQ;
  pluginLowerIRQ = builtinLowerIRQ;

  pluginResetSignal = builtinResetSignal;

  pluginSetHRQHackCallback = builtinSetHRQHackCallback;
  pluginSetHRQ = builtinSetHRQ;
  
  pluginRegisterIOReadHandler = builtinRegisterIOReadHandler;
  pluginRegisterIOWriteHandler = builtinRegisterIOWriteHandler;

  pluginRegisterDefaultIOReadHandler = builtinRegisterDefaultIOReadHandler;
  pluginRegisterDefaultIOWriteHandler = builtinRegisterDefaultIOWriteHandler;

  pluginRegisterTimer = builtinRegisterTimer;
  pluginActivateTimer = builtinActivateTimer;
}


/************************************************************************/
/* Plugin system: Device registration                                   */
/************************************************************************/

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
    device->device_init_mem = init1;
    device->device_init_dev = init2;
    device->device_reset = reset;
    device->device_load_state = load;
    device->device_save_state = save;
    device->next = NULL;

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

/************************************************************************/
/* Plugin system: Load one plugin                                       */
/************************************************************************/

#define PLUGIN_PATH ""
int bx_load_plugin (const char *name)
{
  char plugin_filename[BX_PATHNAME_LEN];

  sprintf(plugin_filename,"%s%s",PLUGIN_PATH,name);

  plugin_load (plugin_filename, "");
  void *handle = dlopen (plugin_filename, RTLD_LAZY);
  if (!handle) {
    pluginlog->error("could not open plugin %s", plugin_filename);
    pluginlog->panic("dlopen error: %s", dlerror ());
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

  // Default builtins are used even in non-plugin mode
  plugin_startup();

#if BX_PLUGINS
  bx_load_plugin("unmapped.so");
  bx_load_plugin("biosdev.so");
  bx_load_plugin("cmos.so");
  bx_load_plugin("vga.so");
  bx_load_plugin("floppy.so");
  bx_load_plugin("parallel.so");
  bx_load_plugin("serial.so");

  // quick and dirty gui plugin selection
  fprintf (stderr, 
      "--Quick and dirty gui selector--\n"
      "The library that you configured with should work fine.\n"
      "If you get undefined symbols when you choose term, relink with -lncurses.\n"
      "If you get undefined symbols when you choose sdl, relink with -lSDL -pthread.\n"
      "Good luck!\n\n");
  fprintf (stderr, "Choose your gui.\n 1. X windows\n 2. SDL\n 3. Term\n 4. RFB/VNC\n 5. wxWindows\n--> ");
  int which = -1;
  while (scanf ("%d", &which) != 1 || !(which>=1 && which<=5)) {
    fprintf (stderr, "--> ");
  }
  switch (which) {
    case 1:
      bx_load_plugin("x.so");
      break;
    case 2:
      bx_load_plugin("sdl.so");
      break;
    case 3:
      bx_load_plugin("term.so");
      break;
    case 4:
      bx_load_plugin("rfb.so");
      break;
    case 5:
      bx_load_plugin("wx.so");
      break;
    default:
      BX_ASSERT(0);
  }

  plugin_init_all();
#else
  pluginlog->info("plugins deactivated");
#endif

  return 0;
}

/*************************************************************************/
/* Plugin system: Execute init function of all registered plugin-devices */
/*************************************************************************/

void bx_init_plugins()
{
    device_t *device;

    // two loops
    for (device = devices; device; device = device->next)
    {
        if (device->device_init_mem != NULL) {
            pluginlog->info("Initialisation of '%s' plugin device",device->name);
            device->device_init_mem(BX_MEM(0));
	}
    }

    for (device = devices; device; device = device->next)
    {
        if (device->device_init_dev != NULL) {
            pluginlog->info("Initialisation of '%s' plugin device",device->name);
            device->device_init_dev(NULL);
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
        if (device->device_reset != NULL) {
            pluginlog->info("Reset of '%s' plugin device",device->name);
            device->device_reset(signal);
        }
    }
}

