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

#ifndef __PLUGIN_H
#define __PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define BX_EVENT_MOUSE_MOTION(dx, dy, state) \
    ((*pluginMouseMotion)(dx, dy, state))
#define BX_EVENT_GEN_SCANCODE(scancode) \
    ((*pluginGenScancode)(scancode))
#define BX_EVENT_PASTE_BYTES(bytes, nbytes) \
    ((*pluginPasteBytes)(bytes, nbytes))

#include <dlfcn.h>

#define PLUGIN_INIT  "plugin_init"
#define PLUGIN_FINI  "plugin_fini"

#define MAX_ARGC     10

typedef enum
{
    EVT_INPORT, EVT_OUTPORT, EVT_INT
} event_t;

typedef int (*handler_t)
    (event_t event, int data, int op_size, int count, void *loc);

typedef Bit32u (*ioReadHandler_t)(void *, Bit32u, unsigned);
typedef void   (*ioWriteHandler_t)(void *, Bit32u, Bit32u, unsigned);

typedef struct _plugin_t
{
    int  initialized;
    void *handle;
    int  argc;
    char *name, *args, *argv[MAX_ARGC];
    int  (*init)(struct _plugin_t *plugin, int argc, char *argv[]);
    void (*fini)(void);
    struct _plugin_t *next;
} plugin_t;

extern plugin_t *plugins;

void plugin_startup (void);
void plugin_load (char *name, char *args);
plugin_t *plugin_unload (plugin_t *plugin);
void plugin_init_all (void);
void plugin_fini_all (void);


/* === IO port stuff === */
int pluginRegisterIOReadHandler(void *thisPtr, ioReadHandler_t callback,
                                unsigned base, const char *name, unsigned len);
int pluginRegisterIOWriteHandler(void *thisPtr, ioWriteHandler_t callback,
                                 unsigned base, const char *name, unsigned len);

/* === IRQ stuff === */
extern void pluginRegisterIRQ(unsigned irq, const char *name);
extern void pluginUnregisterIRQ(unsigned irq, const char *name);
extern void pluginTriggerIRQ(unsigned irq);
extern void pluginUntriggerIRQ(unsigned irq);


/* === CMOS query/set stuff === */

/* These are function pointers.  At startup time, they are set to
 * builtin default handlers which will cause an abort if called.
 * A CMOS plugin should set these pointers to real handler functions.
 */
extern Bit32u (* pluginGetCMOSReg)(unsigned reg);
extern void   (* pluginSetCMOSReg)(unsigned reg, Bit32u val);
extern void   (* pluginCMOSChecksum)(void);


/* === A20 enable line stuff === */
extern unsigned pluginGetA20E(void);
extern void     pluginSetA20E(unsigned val);


/* === Keyboard/Mouse input stuff === */
extern void (* pluginMouseMotion)(int d_x, int d_y, unsigned button_state);
extern void (* pluginGenScancode)(Bit32u scancode);
extern void (* pluginPasteBytes)(Bit8u *bytes, Bit32s length);


/* === DMA stuff === */
extern unsigned (* pluginRegisterDMAChannel)(
  unsigned channel,
  void (* dma_read)(Bit8u *data_byte),
  void (* dma_write)(Bit8u *data_byte)
  );
extern void     (* pluginDMASetDRQ)(unsigned channel, unsigned val);
extern unsigned (* pluginDMAGetTC)(void);

/* === Hard drive / floppy port sharing hack === */
extern Bit32u (* pluginHDReadHandler)(Bit32u address,
    unsigned io_len);
extern void   (* pluginHDWriteHandler)(Bit32u address,
    Bit32u value, unsigned io_len);

/* === VGA stuff === */
extern void (* pluginVGARedrawArea)(unsigned x0, unsigned y0,
                 unsigned width, unsigned height);

#if 0
/* === Mega hacks for bochs IO devices === */
void bx_printf(char *fmt, ...);
void bx_panic(char *fmt, ...);

typedef struct {
  Boolean floppy;
  Boolean keyboard;
  Boolean video;
  Boolean disk;
  Boolean pit;
  Boolean pic;
  Boolean bios;
  Boolean cmos;
  Boolean interrupts;
  Boolean exceptions;
  Boolean unsupported;
  Boolean temp;
  Boolean reset;
  Boolean debugger;
  Boolean mouse;
  Boolean io;
  Boolean xms;
  Boolean v8086;
  Boolean paging;
  Boolean creg;
  Boolean dreg;
  Boolean dma;
  Boolean unsupported_io;
  Boolean serial;
  Boolean cdrom;
#ifdef MAGIC_BREAKPOINT
  Boolean magic_break_enabled;
#endif /* MAGIC_BREAKPOINT */
  void* record_io;
  } bx_debug_t;


typedef struct {
  char path[512];
  unsigned type;
  unsigned initial_status;
  } bx_floppy_options;

typedef struct {
  Boolean present;
  char path[512];
  unsigned int cylinders;
  unsigned int heads;
  unsigned int spt;
  } bx_disk_options;
 
typedef struct
{
  Boolean present;
  char dev[512];
  Boolean inserted;
} bx_cdrom_options;
 
typedef struct {
  char *path;
  unsigned long address;
  } bx_rom_options;
 
typedef struct {
  char *path;
  } bx_vgarom_options;
 
typedef struct {
  size_t megs;
  } bx_mem_options;
 
typedef struct {
  char      *path;
  Boolean   cmosImage;
  unsigned int time0;
  } bx_cmos_options;
 
typedef struct {
  int       valid;
  unsigned  ioaddr;
  unsigned  irq;
  unsigned char macaddr[6];
  char      *ethmod;
  char      *ethdev;
  } bx_ne2k_options;

typedef struct {
  char *midifile, *wavefile, *logfile;
  unsigned int midimode, wavemode, loglevel;
  Bit32u dmatimer;
  } bx_sb16_options;

typedef struct {
  bx_floppy_options floppya;
  bx_floppy_options floppyb;
  bx_disk_options   diskc;
  bx_disk_options   diskd;
  bx_cdrom_options  cdromd;
  bx_sb16_options   sb16;
  char              bootdrive[2];
  unsigned long     vga_update_interval;
  unsigned long     keyboard_serial_delay;
  unsigned long     floppy_command_delay;
  unsigned long     ips;
  Boolean           mouse_enabled;
  Boolean           private_colormap;
  Boolean           i440FXSupport;
  bx_cmos_options   cmos;
  bx_ne2k_options   ne2k;
  Boolean           newHardDriveSupport;
  } bx_options_t;

extern bx_options_t bx_options;
extern bx_debug_t   bx_dbg;
#endif


int plugin_emulate_inport (int port, int op_size, int count, void *loc);
int plugin_emulate_outport (int port, int op_size, int count, void *loc);
int plugin_alloc_intr (plugin_t *plugin, handler_t handler, int vec);
void plugin_free_intr (plugin_t *plugin, int vec);
int plugin_emulate_int (int vec);

typedef void (*plugin_pending_t)(void);
int  plugin_acknowledge_intr (void);
void plugin_enqueue (plugin_pending_t func);
void plugin_handle_pending (void);
void plugin_register_elapsed(void (*funct)(Bit64u));
void plugin_register_periodic(void (*funct)(void));
void plugin_register_mem_map_IO(
  void (*f)(Bit32u, unsigned, unsigned, Bit32u *),
  Bit32u, Bit32u);
void plugin_call_elapsed (Bit64u elapsed);
void plugin_handle_periodic(void);
int      pluginRegisterTimer(void *this_ptr, void (*funct)(void *),
                             Bit32u useconds, Boolean continuous,
                             Boolean active, const char *id);
void     pluginActivateTimer(unsigned id, Bit32u usec, Boolean continuous);
void     pluginDeactivateTimer(unsigned id);
void     pluginSetHRQ(unsigned val);
void     pluginSetHRQHackCallback( void (*callback)(void) );

void     pluginResetSignal(unsigned sig);

extern Bit32u   builtinGetCMOSReg(unsigned reg);
extern void     builtinSetCMOSReg(unsigned reg, Bit32u val);
extern void     builtinCMOSChecksum(void);
extern void     builtinMouseMotion(int d_x, int d_y, unsigned button_state);
extern void     builtinGenScancode(Bit32u scancode);
extern unsigned builtinRegisterDMAChannel(
  unsigned channel,
  void (* dma_read)(Bit8u *data_byte),
  void (* dma_write)(Bit8u *data_byte)
  );
extern void     builtinDMASetDRQ(unsigned channel, unsigned val);
extern unsigned builtinDMAGetTC(void);

extern void (*pluginHRQHackCallback)(void);
extern unsigned pluginHRQ;

extern void (*memMapFunct)(Bit32u, unsigned, unsigned, Bit32u *);

void plugin_abort (void);

/* Atomic XCHG for reentrant function plugin_enqueue() */
#define XCHG(x,y)  \
    ({ \
        typeof(x) p;  \
        asm ( "xchg %0, %1" : "=r" (p) : "m" (x), "0" (y) : "memory" ); \
        p;  \
    })

typedef struct {
  Bit64u   (*wcLSeek)(int fd, Bit64u off64, int whence);
  unsigned (*wcRead)(int fd, void *buffer, unsigned len);
  unsigned (*wcWrite)(int fd, void *buffer, unsigned len);
  void     (*wcCommit)(int fd);
  } pluginCallbacks_t;

extern pluginCallbacks_t pluginCallbacks;

#define pluginRegisterWriteCache(LS, R, W, C) \
  ({ \
  pluginCallbacks.wcLSeek = LS; \
  pluginCallbacks.wcRead = R; \
  pluginCallbacks.wcWrite = W; \
  pluginCallbacks.wcCommit = C; \
  })

#define pluginWCLSeek(fd, off64, whence) \
  pluginCallbacks.wcLSeek(fd, off64, whence)

#define pluginWCRead(fd, buffer, len) \
  pluginCallbacks.wcRead(fd, buffer, len)

#define pluginWCWrite(fd, buffer, len) \
  pluginCallbacks.wcWrite(fd, buffer, len)

#define pluginWCCommit(fd) \
  pluginCallbacks.wcCommit(fd)

// FIXME: called from bochs main (hack)
int bx_init_plugin ();

// every plugin must define these, within the extern"C" block, so that
// a non-mangled function symbol is available in the shared library.
void plugin_fini(void);
int plugin_init(plugin_t *plugin, int argc, char *argv[]);


#ifdef __cplusplus
};
#endif

#endif /* __PLUGIN_H */
