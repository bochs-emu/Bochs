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

Bit32u   (* pluginGetCMOSReg)(unsigned reg) = 0;
void     (* pluginSetCMOSReg)(unsigned reg, Bit32u val) = 0;
void     (* pluginMouseMotion)(int d_x, int d_y, unsigned button_state) = 0;
void     (* pluginGenScancode)(Bit32u scancode) = 0;
void     (* pluginPasteBytes)(Bit8u *bytes, Bit32s length) = 0;
plugin_t *plugins = NULL;      /* Head of the linked list of plugins  */
static void plugin_init_one(plugin_t *plugin);

#if 0
/************************************************************************/
/* Plugin initialization / deinitialization                             */
/************************************************************************/


/* Some builtin (default pass-through) Write-Cache functions, when
 * no Write-Cache plugins are loaded to override these.  These are
 * strict pass-through functions
 */

static Bit64u   builtinWCLSeek(int fd, Bit64u off64, int whence);
static unsigned builtinWCRead(int fd, void *buffer, unsigned len);
static unsigned builtinWCWrite(int fd, void *buffer, unsigned len);
static void     builtinWCCommit(int fd);

static Bit32u   builtinHDReadHandler(Bit32u address,
    unsigned io_len);
static void     builtinHDWriteHandler(Bit32u address,
    Bit32u value, unsigned io_len);
static void     builtinVGARedrawArea(unsigned x0, unsigned y0,
                  unsigned width, unsigned height);


pluginCallbacks_t pluginCallbacks;

plugin_t *plugins = NULL;      /* Head of the linked list of plugins  */
void (*pluginHRQHackCallback)(void);
unsigned pluginHRQ = 0;

extern int filehdl; /* File handle for /dev/plex86 */

Bit32u   (* pluginGetCMOSReg)(unsigned reg) = 0;
void     (* pluginSetCMOSReg)(unsigned reg, Bit32u val) = 0;
void     (* pluginCMOSChecksum)(void) = 0;
void     (* pluginMouseMotion)(int d_x, int d_y, unsigned button_state) = 0;
void     (* pluginGenScancode)(Bit32u scancode) = 0;
void     (* pluginPasteBytes)(Bit8u *bytes, Bit32s length) = 0;
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

bx_debug_t   bx_dbg; // xxx Fix me, bochs hack
bx_options_t bx_options; // xxx Fix me, bochs hack

// xxx Fix me, bochs hack
  void
bx_panic(char *fmt, ...)
{
  va_list ap;
  
#if 0
  if (bx_logfd) {
    fprintf(bx_logfd, "panic, ");
  
    va_start(ap, fmt);
    vfprintf(bx_logfd, fmt, ap);
    va_end(ap);
    }
#endif
  fprintf(stderr, "bochs: panic, ");
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  plugin_abort ();
} 

// xxx Fix me, bochs hack
  void
bx_printf(char *fmt, ...)
{
  va_list ap;

#if 0
  if (bx_logfd) {
    va_start(ap, fmt);
    vfprintf(bx_logfd, fmt, ap);
    va_end(ap); 
    }
#endif
  fprintf(stderr, "bochs: ");
  va_start(ap, fmt); 
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}




#if 0
void
plugin_init_all (void)
{
    plugin_t *plugin;

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
        if (plugin->init (plugin, plugin->argc, plugin->argv))
        {
            fprintf (stderr, "Plugin initialization failed for %s\n", plugin->name);
            plugin_abort();
        }

        plugin->initialized = 1;
    }

    return;
}
#endif


  plugin_t *
plugin_unload(plugin_t *plugin)
{
    int i;
    plugin_t *dead_plug;

    if (plugin->initialized)
        plugin->fini ();

    for (i=0; i<256; i++)
        plugin_free_intr (plugin, i);

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



#if 0
#define BX_MAX_IRQS 16
#define BX_NO_IRQ  -1
const char *irq_handler_name[BX_MAX_IRQS];

  for (i=0; i < BX_MAX_IRQS; i++) {
    irq_handler_name[i] = NULL;
    }
#endif

  void
pluginUnregisterIRQ(unsigned irq, const char *name)
{
#warning "Finish pluginUnregisterIRQ"
#if 0
  if (irq >= BX_MAX_IRQS) {
    bx_panic("IO device %s tried to unregister IRQ %d above %u\n",
             name, irq, (unsigned) BX_MAX_IRQS-1);
    }

  if (!irq_handler_name[irq]) {
    bx_printf("IO device %s tried to unregister IRQ %d, not registered\n",
	      name, irq);
    return;
  }

  if (strcmp(irq_handler_name[irq], name)) {
    bx_printf("IRQ %u not registered to %s but to %s\n", irq,
      name, irq_handler_name[irq]);
    return;
    }
  irq_handler_name[irq] = NULL;
#endif
}


/************************************************************************/
/* Software interrupt handling                                          */
/************************************************************************/

static struct intr_t
{
    plugin_t  *plugin;
    handler_t  handler;
    struct intr_t *next;
}
*ints[256];         /* Heads of linked lists for each int */


int
plugin_alloc_intr (plugin_t *plugin, handler_t handler, int vec)
{
    struct intr_t *intr;

    for (intr = ints[vec]; intr != NULL; intr = intr->next)
    {
        if (intr->plugin == plugin)
            return -1;
    }

    if (ints[vec] == NULL && vm_alloc_intr (vec))
        return -1;

    intr = malloc (sizeof (struct intr_t));
    intr->plugin  = plugin;
    intr->handler = handler;
    intr->next    = ints[vec];
    ints[vec]     = intr;

    return 0;
}


  void
plugin_free_intr (plugin_t *plugin, int vec)
{
    struct intr_t *intr, *dummy;

    if ((intr = ints[vec]) == NULL)
        return;

    if (intr->plugin == plugin)
    {
       dummy = intr;
       ints[vec] = intr->next;
       free (dummy);
    }
    else
        for (; intr != NULL; intr = intr->next)
        {
            if (intr->next == NULL)
                break;

            if (intr->next->plugin == plugin)
            {
                dummy = intr->next;
                intr->next = intr->next->next;
                free (dummy);

                break;
            }
        }

    if (ints[vec] == NULL)
        vm_release_intr (vec);

    return;
}


  int
plugin_emulate_int(int vec)
{
    struct intr_t *intr;
    int reflect = 0;

    for (intr = ints[vec]; intr != NULL; intr = intr->next)
    {
        if (intr->handler (EVT_INT, vec, 0, 0, NULL))
            reflect = 1;
    }

    return reflect;
}



/************************************************************************/
/* VM control                                                           */
/************************************************************************/

void
plugin_abort (void)
{
    vm_abort ();
    return;
}



/************************************************************************/
/* Plugin pending                                                       */
/************************************************************************/

/*
 * NOTE:  This code was explicitly written to be reentrant using atomic
 *        XCHG, because it will usually be called from signal handlers and
 *        the like.  None of the other plugin functions are reentrant, so
 *        a plugin wanting to perform plugin operations after reception
 *        of a signal should always use these functions to pend for CPU
 *        time.
 */


/************************************************************************/
/*                                                                      */
/************************************************************************/

void (*memMapFunct)(Bit32u, unsigned, unsigned, Bit32u *) = NULL;

  void
plugin_register_mem_map_IO( void (*f)(Bit32u, unsigned, unsigned, Bit32u *),
  Bit32u range0, Bit32u range1)
{
  memMapFunct = f;
}


/************************************************************************/
/* Timing control                                                       */
/************************************************************************/

static void (*save_funct)(Bit64u) = NULL;
static void (*periodic_funct)(void) = NULL;


  void
plugin_register_elapsed(void (*funct)(Bit64u))
{
  save_funct = funct;
}

  void
plugin_register_periodic(void (*funct)(void))
{
  periodic_funct = funct;
}


  void
plugin_call_elapsed( Bit64u elapsed )
{
  if (save_funct)
    save_funct(elapsed);
}

  void
plugin_handle_periodic(void)
{
  static Bit64u prev_user_usec = 0;
  Bit64u new_user_usec;
  volatile extern Bit64u user_time_usec;

  if (!periodic_funct) return;

#warning "fix: need exclusive access to variable used by async handler"
  new_user_usec = user_time_usec;
  if ( (new_user_usec - prev_user_usec) >= 500000 ) {
    prev_user_usec = new_user_usec;
    periodic_funct();
    }
}



/************************************************************************/
/* Plugin system: plex86 startup function                               */
/************************************************************************/

#define ZERO_ARRAY(a)  memset (a, 0, sizeof(a))

  void
plugin_startup(void)
{
  ZERO_ARRAY(ints);

  /* Register builtin Write-Cache functions, so we have some defaults
   * until overloaded by plugins
   */
  pluginRegisterWriteCache(builtinWCLSeek, builtinWCRead, builtinWCWrite,
                           builtinWCCommit);
  pluginGetCMOSReg   = builtinGetCMOSReg;
  pluginSetCMOSReg   = builtinSetCMOSReg;
  pluginCMOSChecksum = builtinCMOSChecksum;
  pluginMouseMotion  = builtinMouseMotion;
  pluginGenScancode  = builtinGenScancode;
  pluginPasteBytes   = builtinPasteBytes;
  pluginRegisterDMAChannel = builtinRegisterDMAChannel;
  pluginDMASetDRQ    = builtinDMASetDRQ;
  pluginDMAGetTC     = builtinDMAGetTC;

  pluginHDReadHandler  = builtinHDReadHandler;
  pluginHDWriteHandler = builtinHDWriteHandler;

  pluginVGARedrawArea  = builtinVGARedrawArea;
}

  void
builtinCMOSChecksum(void)
{
  fprintf(stderr, "builtinCMOSChecksum called, no CMOS plugin loaded?\n");
  vm_abort();
}

  void
builtinMouseMotion(int d_x, int d_y, unsigned button_state)
{
  fprintf(stderr, "builtinMouseMotion called, not overloaded by keyboard"
                  " plugin?\n");
  vm_abort();
}

  void
builtinGenScancode(Bit32u scancode)
{
  fprintf(stderr, "builtinGenScancode called, not overloaded by keyboard"
                  " plugin?\n");
  vm_abort();
}

  void
builtinPasteBytes(Bit8u *bytes, Bit32s nbytes)
{
  fprintf(stderr, "builtinPasteBytes called, not overloaded by keyboard"
                  " plugin?\n");
  vm_abort();
}

  unsigned
builtinRegisterDMAChannel(
  unsigned channel,
  void (* dma_read)(Bit8u *data_byte),
  void (* dma_write)(Bit8u *data_byte)
  )
{
  fprintf(stderr, "builtinRegisterDMAChannel called, not overloaded by DMA"
                  " plugin?\n");
  vm_abort();
  return 0;
}

  void
builtinDMASetDRQ(unsigned channel, unsigned val)
{
  fprintf(stderr, "builtinDMASetDRQ( called, not overloaded by DMA"
                  " plugin?\n");
  vm_abort();
}

  unsigned
builtinDMAGetTC(void)
{
  fprintf(stderr, "builtinDMAGetTC called, not overloaded by DMA"
                  " plugin?\n");
  vm_abort();
  return 0;
}

  Bit32u
builtinHDReadHandler(Bit32u address, unsigned io_len)
{
  fprintf(stderr, "builtinHDReadHandler called, HD plugin not loaded?"
                  " plugin?\n");
  vm_abort();
  return 0;
}

  void
builtinHDWriteHandler(Bit32u address, Bit32u value, unsigned io_len)
{
  fprintf(stderr, "builtinHDWriteHandler called, HD plugin not loaded?"
                  " plugin?\n");
  vm_abort();
}

  void
builtinVGARedrawArea(unsigned x0, unsigned y0,
                     unsigned width, unsigned height)
{
}

  Bit64u
builtinWCLSeek(int fd, Bit64u off64, int whence)
{
  return( lseek(fd, (off_t) off64, whence) );
}

  unsigned
builtinWCRead(int fd, void *buffer, unsigned len)
{
  return( read(fd, buffer, len) );
}

  unsigned
builtinWCWrite(int fd, void *buffer, unsigned len)
{
  return( write(fd, buffer, len) );
}

  void
builtinWCCommit(int fd)
{
  fprintf(stderr, "builtin Write-Cache Commit (nop)\n");
}


// xxx Fix these
int plugin_emulate_inport(int port, int op_size, int count, void *loc)
{
  fprintf(stderr, "plugin_emulate_inport:\n");
  vm_abort();
}

int plugin_emulate_outport(int port, int op_size, int count, void *loc)
{
  fprintf(stderr, "plugin_emulate_outport:\n");
  vm_abort();
}

  void
pluginTriggerIRQ(unsigned irq)
{
  int ret;
  irqMsg_t irqMsg;

  irqMsg.irq = irq;
  irqMsg.on  = 1;

  ret = ioctl(filehdl, PLEX86_IRQ, &irqMsg);
  if (ret == -1) {
    perror ("ioctl PLEX86_IRQ: ");
    vm_abort();
    }
}

  void
pluginUntriggerIRQ(unsigned irq)
{
  int ret;
  irqMsg_t irqMsg;

  irqMsg.irq = irq;
  irqMsg.on  = 0;

  ret = ioctl(filehdl, PLEX86_IRQ, &irqMsg);
  if (ret == -1) {
    perror ("ioctl PLEX86_IRQ: ");
    vm_abort();
    }
}

  void
pluginSetHRQ(unsigned val)
{
  pluginHRQ = val;
}

  void
pluginSetHRQHackCallback( void (*callback)(void) )
{
  pluginHRQHackCallback = callback;
}

  void
pluginResetSignal(unsigned sig)
{
  fprintf(stderr, "pluginResetSignal: unimplemented\n");
  vm_abort();
}
#endif

  Bit32u
builtinGetCMOSReg(unsigned reg)
{
  Bit8u val = bx_cmos.s.reg[reg];
  BX_INFO(("plugin reading cmos register 0x%02x = 0x%02x", reg, val));
  return val;
}

  void
builtinSetCMOSReg(unsigned reg, Bit32u val)
{
  BX_ASSERT (reg >= 0 && reg < BX_NUM_CMOS_REGS);
  bx_cmos.s.reg[reg] = val;
  BX_INFO(("plugin writing 0x%02x to cmos register 0x%02x", val, reg));
}

  void
builtinMouseMotion(int d_x, int d_y, unsigned button_state)
{
  BX_PANIC(("builtinMouseMotion called, not overloaded by keyboard"
                  " plugin?\n"));
  abort();
}

  void
builtinGenScancode(Bit32u scancode)
{
  BX_PANIC(("builtinGenScancode called, not overloaded by keyboard"
                  " plugin?\n"));
  abort();
}

  void
builtinPasteBytes(Bit32u scancode)
{
  BX_PANIC(("builtinPasteBytes called, not overloaded by keyboard"
                  " plugin?\n"));
  abort();
}


#define KBD_PLUGIN "/tmp/bochslib/libplex-keyboard.so"
int bx_init_plugin ()
{
  pluginGetCMOSReg = builtinGetCMOSReg;
  pluginSetCMOSReg = builtinSetCMOSReg;
  pluginMouseMotion = builtinMouseMotion;
  pluginGenScancode = builtinGenScancode;
  plugin_load (KBD_PLUGIN, "");
  /*
  void *handle = dlopen (KBD_PLUGIN, RTLD_LAZY);
  if (!handle) {
    BX_ERROR (("could not open kbd plugin %s", KBD_PLUGIN));
    BX_PANIC (("dlopen error: %s", dlerror ()));
    exit (1);
  }
  */
  BX_INFO (("opened keyboard library"));
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

    plugin->init =  
      (int  (*)(struct _plugin_t *, int, char *[])) /* monster typecast */
      dlsym (plugin->handle, PLUGIN_INIT);
    if ((plug_err = dlerror ()) != NULL)
    {
        BX_PANIC (("could not find init: %s", plug_err));
        plugin_abort ();
    }

    plugin->fini = (void (*)(void)) dlsym (plugin->handle, PLUGIN_FINI);
    if ((plug_err = dlerror ()) != NULL)
    {
        BX_PANIC (("could not find fini: %s", plug_err));
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
        if (plugin->init (plugin, plugin->argc, plugin->argv))
        {
            fprintf (stderr, "Plugin initialization failed for %s\n", plugin->name);
            plugin_abort();
        }
 
        plugin->initialized = 1;
}

void
plugin_abort (void)
{
    BX_PANIC (("plugin load aborted"));
    abort();
}

  void
pluginRegisterIRQ(unsigned irq, const char *name)
{
#warning "Finish pluginRegisterIRQ"
  BX_ERROR(("pluginRegisterIRQ not complete"));

#if 0
  if (irq >= BX_MAX_IRQS) {
    bx_panic("IO device %s registered with IRQ=%d above %u\n",
             name, irq, (unsigned) BX_MAX_IRQS-1);
    }
  if (irq_handler_name[irq]) {
    bx_panic("IRQ %u conflict, %s with %s\n", irq,
      irq_handler_name[irq], name);
    }
  irq_handler_name[irq] = name;
#endif
}

  int
pluginRegisterIOReadHandler(void *thisPtr, ioReadHandler_t callback,
                            unsigned base, const char *name, unsigned len)
{
  BX_ASSERT (len==1);
  bx_devices.register_io_read_handler (thisPtr, callback, base, name);
  BX_INFO(("plugin registered I/O read address at %04x", base));
}

  int
pluginRegisterIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                             unsigned base, const char *name, unsigned len)
{
  BX_ASSERT (len==1);
  bx_devices.register_io_write_handler (thisPtr, callback, base, name);
  BX_INFO(("plugin registered I/O write address at %04x", base));
}

int
pluginRegisterTimer(void *this_ptr, void (*funct)(void *),
			Bit32u useconds, Boolean continuous, Boolean active,
			const char *id)
{
  int timerid = bx_pc_system.register_timer (this_ptr, funct, useconds, continuous, active, id);
  BX_INFO (("plugin registered timer %d", timerid));
  return timerid;
}

void
pluginActivateTimer(unsigned id, Bit32u usec, Boolean continuous)
{
  bx_pc_system.activate_timer (id, usec, continuous);
  BX_INFO(("plugin activated timer %d", id));
}

void
pluginTriggerIRQ(unsigned irq)
{
  BX_INFO(("plugin triggered irq %d", irq));
  bx_pic.raise_irq (irq);
}


