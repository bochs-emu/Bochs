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

unsigned (* pluginRegisterDMA8Channel)(
                unsigned channel,
                void (* dma_read)(Bit8u *data_byte),
                void (* dma_write)(Bit8u *data_byte),
		const char* name
                ) = 0;
unsigned (* pluginRegisterDMA16Channel)(
                unsigned channel,
                void (* dma_read)(Bit16u *data_byte),
                void (* dma_write)(Bit16u *data_byte),
		const char* name
                ) = 0;
unsigned (* pluginUnregisterDMAChannel)(unsigned channel) = 0;
void     (* pluginDMASetDRQ)(unsigned channel, Boolean val) = 0;
unsigned (* pluginDMAGetTC)(void) = 0;
void     (* pluginDMARaiseHLDA)(void) = 0;

Bit32u   (* pluginHDReadHandler)(void* ptr, Bit32u address, unsigned io_len) = 0;
void     (* pluginHDWriteHandler)(void* ptr, Bit32u address, Bit32u value, unsigned io_len) = 0;
Bit32u   (* pluginHDGetDeviceHandle)(Bit8u, Bit8u) = 0;
Bit32u   (* pluginHDGetFirstCDHandle)(void) = 0;
unsigned (* pluginHDGetCDMediaStatus)(Bit32u) = 0;
unsigned (* pluginHDSetCDMediaStatus)(Bit32u, unsigned) = 0;
void     (* pluginHDCloseHarddrive) (void) = 0;

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
Bit8u (*pluginPicIAC)(void) = 0;

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

#if BX_PLUGINS
// When compiling with plugins, plugin.cc will provide the pluginKeyboard
// pointer.  At first it will point to the stub so that calls to the methods
// will panic instead of segfaulting.  The pointer will be replaced with a real
// bx_keyb_c object by plugin_init of the keyboard plugin.
bx_keyb_stub_c pluginKeyboardStub;
bx_keyb_stub_c *pluginKeyboard = &pluginKeyboardStub;
bx_hard_drive_stub_c pluginHardDriveStub;
bx_hard_drive_stub_c *pluginHardDrive = &pluginHardDriveStub;
#else
// When plugins are turned off, the device will provide the pluginKeyboard
// pointer instead.  It will be initialized to point to a real bx_keyb_c
// immediately, instead of ever pointing at an instance of a stub class.
#endif

#if BX_PLUGINS
// When compiling with plugins, plugin.cc will provide the bx_gui
// pointer.  At first it will point to NULL.  The pointer will be replaced with
// a real gui object by plugin_init of the specific gui module.
bx_gui_c *bx_gui = NULL;
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
  pluginlog->panic("builtinGetCMOSTimeval called, no CMOS plugin loaded?");
  return 0;
}

  static unsigned
builtinRegisterDMA8Channel(
  unsigned channel,
  void (* dma_read)(Bit8u *data_byte),
  void (* dma_write)(Bit8u *data_byte),
  const char* name
  )
{
  pluginlog->panic("builtinRegisterDMA8Channel called, no DMA plugin loaded?");
  return 0;
}

  static unsigned
builtinRegisterDMA16Channel(
  unsigned channel,
  void (* dma_read)(Bit16u *data_byte),
  void (* dma_write)(Bit16u *data_byte),
  const char* name
  )
{
  pluginlog->panic("builtinRegisterDMA16Channel called, no DMA plugin loaded?");
  return 0;
}

  static unsigned
builtinUnregisterDMAChannel(unsigned channel)
{
  pluginlog->panic("builtinUnregisterDMAChannel called, no DMA plugin loaded?");
  return 0;
}

  static void
builtinDMASetDRQ(unsigned channel, Boolean val)
{
  pluginlog->panic("builtinDMASetDRQ called, no DMA plugin loaded?");
}

  static unsigned
builtinDMAGetTC(void)
{
  pluginlog->panic("builtinDMAGetTC called, no DMA plugin loaded?");
  return 0;
}

  static void     
builtinDMARaiseHLDA(void)
{
  pluginlog->panic("builtinDMARaiseHLDA called, no DMA plugin loaded?");
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
  return 0;
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
  return 0;
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
  pluginlog->panic("builtinRaiseIRQ called, no pic plugin loaded?");
}

  static void
builtinLowerIRQ(unsigned irq)
{
  pluginlog->panic("builtinLowerIRQ called, no pic plugin loaded?");
}

  static Bit8u 
builtinPicIAC(void)
{
  pluginlog->panic("builtinPicIAC called, no pic plugin loaded?");
  return 0;
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
        if (plugin->plugin_init (plugin, plugin->argc, plugin->argv))
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
    fprintf (stderr, "lt_dlhandle is %p\n", plugin->handle);
    if (!plugin->handle)
    {
      current_plugin_context = NULL;
      BX_PANIC (("dlopen failed for module '%s': %s", name, lt_dlerror ()));
      free (plugin);
      return;
    }

    plugin->plugin_init =  
      (int  (*)(struct _plugin_t *, int, char *[])) /* monster typecast */
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
  pluginRegisterDMA8Channel = builtinRegisterDMA8Channel;
  pluginRegisterDMA16Channel = builtinRegisterDMA16Channel;
  pluginUnregisterDMAChannel = builtinUnregisterDMAChannel;
  pluginDMASetDRQ    = builtinDMASetDRQ;
  pluginDMAGetTC     = builtinDMAGetTC;
  pluginDMARaiseHLDA = builtinDMARaiseHLDA;

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
  pluginPicIAC = builtinPicIAC;

  pluginResetSignal = builtinResetSignal;

  pluginSetHRQHackCallback = builtinSetHRQHackCallback;
  pluginSetHRQ = builtinSetHRQ;
  
  pluginRegisterIOReadHandler = builtinRegisterIOReadHandler;
  pluginRegisterIOWriteHandler = builtinRegisterIOWriteHandler;

  pluginRegisterDefaultIOReadHandler = builtinRegisterDefaultIOReadHandler;
  pluginRegisterDefaultIOWriteHandler = builtinRegisterDefaultIOWriteHandler;

  pluginRegisterTimer = builtinRegisterTimer;
  pluginActivateTimer = builtinActivateTimer;

  int status = lt_dlinit ();
  if (status != 0) {
    BX_ERROR (("initialization error in ltdl library (for loading plugins)"));
    BX_PANIC (("error message was: %s", lt_dlerror ()));
  }
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

void pluginRegisterDeviceDevmodel(bx_devmodel_c *devmodel, char *name)
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
    BX_ASSERT(current_plugin_context != NULL);
    device->plugin = current_plugin_context;
    device->use_devmodel_interface = 1;
    device->device_init_mem = NULL;  // maybe should use 1 to detect any use?
    device->device_init_dev = NULL;
    device->device_reset = NULL;
    device->device_load_state = NULL;
    device->device_save_state = NULL;
    device->next = NULL;

    // Don't add every kind of device to the list.
    switch (device->plugin->type)
    {
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
  bx_load_plugin(BX_PLUGIN_UNMAPPED, PLUGTYPE_OPTIONAL);
  bx_load_plugin(BX_PLUGIN_BIOSDEV, PLUGTYPE_OPTIONAL);
  bx_load_plugin(BX_PLUGIN_CMOS, PLUGTYPE_OPTIONAL);
  bx_load_plugin(BX_PLUGIN_DMA, PLUGTYPE_OPTIONAL);
  bx_load_plugin(BX_PLUGIN_PIC, PLUGTYPE_OPTIONAL);
  bx_load_plugin(BX_PLUGIN_VGA, PLUGTYPE_OPTIONAL);
  bx_load_plugin(BX_PLUGIN_FLOPPY, PLUGTYPE_OPTIONAL);
  bx_load_plugin(BX_PLUGIN_PARALLEL, PLUGTYPE_OPTIONAL);
  bx_load_plugin(BX_PLUGIN_SERIAL, PLUGTYPE_OPTIONAL);


  // quick and dirty gui plugin selection
  static char *gui_names[] = {
    "amigaos", "beos", "carbon", "macintosh", "nogui", 
    "rfb", "sdl", "term", "win32", "wx", "x", 
    NULL
  };
  fprintf (stderr, 
      "--Quick and dirty gui selector--\n"
      "The library that you configured with should work fine.\n"
      "If you get undefined symbols when you choose term, relink with -lncurses.\n"
      "If you get undefined symbols when you choose sdl, relink with -lSDL -pthread.\n"
      "Good luck!\n\n");
  fprintf (stderr, "Choose your gui.\n");
  int i;
  for (i=0; gui_names[i] != NULL; i++)
    fprintf (stderr, "%d. %s\n", i, gui_names[i]);
  int imax = i;
  int which = -1;
  char line[256];
  do {
    fprintf (stderr, "--> ");
    if (!fgets (line, sizeof(line), stdin)) break;
  } while (sscanf (line, "%d", &which) != 1 || !(which>=0 && which<imax));
  bx_load_plugin (gui_names[which], PLUGTYPE_OPTIONAL);

  if (bx_gui == NULL) {
    BX_PANIC (("No gui has been loaded.  It is not safe to continue. Exiting."));
    exit(1);
  }

  //// each one has already been init'ed by bx_load_plugin
  //plugin_init_all();
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
      if (!device->use_devmodel_interface) {
        if (device->device_init_mem != NULL) {
            pluginlog->info("Initialisation of '%s' plugin device",device->name);
            device->device_init_mem(BX_MEM(0));
	}
      } else {
	device->devmodel->init_mem (BX_MEM(0));
      }
    }

    for (device = devices; device; device = device->next)
    {
      if (!device->use_devmodel_interface) {
        if (device->device_init_dev != NULL) {
            pluginlog->info("Initialisation of '%s' plugin device",device->name);
            device->device_init_dev();
	}
      } else {
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
            pluginlog->info("Reset of '%s' plugin device",device->name);
            device->device_reset(signal);
        }
      } else {
	device->devmodel->reset (signal);
      }
    }
}
