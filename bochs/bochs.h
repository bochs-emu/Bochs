//  Copyright (C) 2000  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#ifndef BX_BOCHS_H
#  define BX_BOCHS_H 1

extern "C" {
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#  include <unistd.h>
#else
#  include <io.h>
#endif
#include <time.h>
#ifdef macintosh
#  include <types.h>
#  include <stat.h>
#  include <utime.h>
#else
#  ifndef WIN32
#    include <sys/time.h>
#  endif
#  include <sys/types.h>
#  include <sys/stat.h>
#endif
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#ifdef macintosh
#  include "macutils.h"
#  define SuperDrive "[fd:]"
#endif
}

// Hacks for win32: always return regular file.
#ifdef WIN32
#ifndef __MINGW32__
#  define S_ISREG(st_mode) 1
#  define S_ISCHR(st_mode) 0

// VCPP includes also are missing these
#  define off_t long
#  define ssize_t int
#endif
#endif

#include "config.h"
#include "debug/debug.h"

//
// some macros to interface the CPU and memory to external environment
// so that these functions can be redirected to the debugger when
// needed.
//

#if ((BX_DEBUGGER == 1) && (BX_NUM_SIMULATORS >= 2))
// =-=-=-=-=-=-=- Redirected to cosimulation debugger -=-=-=-=-=-=-=
#define BX_VGA_MEM_READ(addr)       bx_dbg_ucmem_read(addr)
#define BX_VGA_MEM_WRITE(addr, val) bx_dbg_ucmem_write(addr, val)
#if BX_SUPPORT_A20
#  define A20ADDR(x)               ( (x) & bx_pc_system.a20_mask )
#else
#  define A20ADDR(x)                (x)
#endif
#define BX_INP(addr, len)           bx_dbg_inp(addr, len)
#define BX_OUTP(addr, val, len)     bx_dbg_outp(addr, val, len)
#define BX_HRQ                      (bx_pc_system.HRQ)
#define BX_RAISE_HLDA()             bx_dbg_raise_HLDA()
#define BX_TICK1()
#define BX_INTR                     bx_pc_system.INTR
#define BX_SET_INTR(b)              bx_dbg_set_INTR(b)
#if BX_SIM_ID == 0
#  define BX_CPU_C                  bx_cpu0_c
#  define BX_CPU                    bx_cpu0
#  define BX_MEM_C                  bx_mem0_c
#  define BX_MEM                    bx_mem0
#else
#  define BX_CPU_C                  bx_cpu1_c
#  define BX_CPU                    bx_cpu1
#  define BX_MEM_C                  bx_mem1_c
#  define BX_MEM                    bx_mem1
#endif
#define BX_SET_ENABLE_A20(enabled)  bx_dbg_async_pin_request(BX_DBG_ASYNC_PENDING_A20, \
                                      enabled)
#define BX_GET_ENABLE_A20()         bx_pc_system.get_enable_a20()

#else
// =-=-=-=-=-=-=- Normal optimized use -=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define BX_VGA_MEM_READ(addr) (bx_devices.vga->mem_read(addr))
#define BX_VGA_MEM_WRITE(addr, val) bx_devices.vga->mem_write(addr, val)
#if BX_SUPPORT_A20
#  define A20ADDR(x)               ( (x) & bx_pc_system.a20_mask )
#else
#  define A20ADDR(x)               (x)
#endif
//
// some pc_systems functions just redirect to the IO devices so optimize
// by eliminating call here
//
// #define BX_INP(addr, len)        bx_pc_system.inp(addr, len)
// #define BX_OUTP(addr, val, len)  bx_pc_system.outp(addr, val, len)
#define BX_INP(addr, len)           bx_devices.inp(addr, len)
#define BX_OUTP(addr, val, len)     bx_devices.outp(addr, val, len)
#define BX_HRQ                      (bx_pc_system.HRQ)
#define BX_RAISE_HLDA()             bx_pc_system.raise_HLDA()
#define BX_TICK1()                  bx_pc_system.tick1()
#define BX_INTR                     bx_pc_system.INTR
#define BX_SET_INTR(b)              bx_pc_system.set_INTR(b)
#define BX_CPU_C                    bx_cpu_c
#define BX_CPU                      bx_cpu
#define BX_MEM_C                    bx_mem_c
#define BX_MEM                      bx_mem
#define BX_SET_ENABLE_A20(enabled)  bx_pc_system.set_enable_a20(enabled)
#define BX_GET_ENABLE_A20()         bx_pc_system.get_enable_a20()

#endif  // ((BX_DEBUGGER == 1) && (BX_NUM_SIMULATORS >= 2))

// #define BX_IAC()                 bx_pc_system.IAC()
#define BX_IAC()                    bx_devices.pic->IAC()
//#define BX_IAC()                    bx_dbg_IAC()

//
// Ways for the the external environment to report back information
// to the debugger.
//

#if BX_DEBUGGER
#  define BX_DBG_ASYNC_INTR bx_guard.async.irq
#  define BX_DBG_ASYNC_DMA  bx_guard.async.dma
#if (BX_NUM_SIMULATORS > 1)
// for multiple simulators, we always need this info, since we're
// going to replay it.
#  define BX_DBG_DMA_REPORT(addr, len, what, val) \
        bx_dbg_dma_report(addr, len, what, val)
#  define BX_DBG_IAC_REPORT(vector, irq) \
        bx_dbg_iac_report(vector, irq)
#  define BX_DBG_A20_REPORT(val) \
        bx_dbg_a20_report(val)
#  define BX_DBG_IO_REPORT(addr, size, op, val) \
        bx_dbg_io_report(addr, size, op, val)
#  define BX_DBG_UCMEM_REPORT(addr, size, op, val)
#else
// for a single simulator debug environment, we can optimize a little
// by conditionally calling, as per requested.

#  define BX_DBG_DMA_REPORT(addr, len, what, val) \
        if (bx_guard.report.dma) bx_dbg_dma_report(addr, len, what, val)
#  define BX_DBG_IAC_REPORT(vector, irq) \
        if (bx_guard.report.irq) bx_dbg_iac_report(vector, irq)
#  define BX_DBG_A20_REPORT(val) \
        if (bx_guard.report.a20) bx_dbg_a20_report(val)
#  define BX_DBG_IO_REPORT(addr, size, op, val) \
        if (bx_guard.report.io) bx_dbg_io_report(addr, size, op, val)
#  define BX_DBG_UCMEM_REPORT(addr, size, op, val) \
        if (bx_guard.report.ucmem) bx_dbg_ucmem_report(addr, size, op, val)
#endif  // #if (BX_NUM_SIMULATORS > 1)

#else  // #if BX_DEBUGGER
// debugger not compiled in, use empty stubs
#  define BX_DBG_ASYNC_INTR 1
#  define BX_DBG_ASYNC_DMA  1
#  define BX_DBG_DMA_REPORT(addr, len, what, val)
#  define BX_DBG_IAC_REPORT(vector, irq)
#  define BX_DBG_A20_REPORT(val)
#  define BX_DBG_IO_REPORT(addr, size, op, val)
#  define BX_DBG_UCMEM_REPORT(addr, size, op, val)
#endif  // #if BX_DEBUGGER

extern Bit8u DTPageDirty[];
#if BX_DYNAMIC_TRANSLATION
#  define BX_DYN_DIRTY_PAGE(page) DTPageDirty[page] = 1;
#else
#  define BX_DYN_DIRTY_PAGE(page)
#endif


#include "state_file.h"

#ifndef UNUSED
#  define UNUSED(x) ((void)x)
#endif

#define uint8   Bit8u
#define uint16  Bit16u
#define uint32  Bit32u


#if BX_PROVIDE_CPU_MEMORY==1
#  include "cpu/cpu.h"
#endif

#if BX_DISASM
#  include "disasm/disasm.h"
#endif

#if BX_DYNAMIC_TRANSLATION
#  include "dynamic/dynamic.h"
#endif


typedef struct {
  Boolean floppy;
  Boolean keyboard;
  Boolean video;
  Boolean disk;
  Boolean pit;
  Boolean pic;
  Boolean bios;
  Boolean cmos;
  Boolean a20;
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
#ifdef BX_DEBUG_LINUX
  Boolean linux_syscall;
  Boolean linux_task;
#endif
  void* record_io;
  } bx_debug_t;

void bx_printf(char *fmt, ...);
void bx_panic(char *fmt, ...);
void bx_atexit(void);
extern bx_debug_t bx_dbg;



#define BX_FLOPPY_NONE   10 // floppy not present
#define BX_FLOPPY_1_2    11 // 1.2M  5.25"
#define BX_FLOPPY_1_44   12 // 1.44M 3.5"
#define BX_FLOPPY_2_88   13 // 2.88M 3.5"
#define BX_FLOPPY_720K   14 // 720K  3.5"


#define BX_READ    10
#define BX_WRITE   11
#define BX_RW      12





#include "memory/memory.h"


enum PCS_OP { PCS_CLEAR, PCS_SET, PCS_TOGGLE };

#include "pc_system.h"

#include "gui/gui.h"
extern bx_gui_c   bx_gui;
#include "iodev/iodev.h"








/* --- EXTERNS --- */

extern FILE *bx_logfd;




#if ( BX_PROVIDE_DEVICE_MODELS==1 )
extern bx_devices_c   bx_devices;
#endif

#define BX_EJECTED   10
#define BX_INSERTED  11


#define BX_RESET_SOFTWARE 10
#define BX_RESET_HARDWARE 11


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

struct bx_cdrom_options
{
  Boolean present;
  char dev[512];
  Boolean inserted;
};

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
// These options are used for a special hack to load a
// 32bit OS directly into memory, so it can be run without
// any of the 16bit real mode or BIOS assistance.  This
// is for the development of freemware, so we don't have
// to implement real mode up front.
#define Load32bitOSLinux       1
#define Load32bitOSNullKernel  2 // being developed for freemware
  unsigned whichOS;
  char    *path;
  char    *iolog;
  char    *initrd;
  } bx_load32bitOSImage_t;


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
  bx_rom_options    rom;
  bx_vgarom_options vgarom;
  bx_mem_options    memory;
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
  bx_load32bitOSImage_t load32bitOSImage;
  } bx_options_t;

extern bx_options_t bx_options;



#if BX_PROVIDE_CPU_MEMORY==1
#else
// #  include "external_interface.h"
#endif

#define BX_USE_PS2_MOUSE 1

int bx_bochs_init(int argc, char *argv[]);

#include "instrument.h"


#endif  /* BX_BOCHS_H */
