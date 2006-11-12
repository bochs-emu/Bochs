/////////////////////////////////////////////////////////////////////////
// $Id: config.cc,v 1.112 2006-11-12 21:25:06 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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

#include "bochs.h"
#include "iodev/iodev.h"
#include <assert.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#if defined(macintosh)
// Work around a bug in SDL 1.2.4 on MacOS X, which redefines getenv to
// SDL_getenv, but then neglects to provide SDL_getenv.  It happens
// because we are defining -Dmacintosh.
#undef getenv
#endif


int bochsrc_include_count = 0;

#define LOG_THIS genlog->

extern bx_debug_t bx_dbg;

static char *get_builtin_variable(char *varname);
static Bit32s parse_line_unformatted(const char *context, char *line);
static Bit32s parse_line_formatted(const char *context, int num_params, char *params[]);
static int parse_bochsrc(const char *rcfile);
static int get_floppy_type_from_image(const char *filename);

static Bit64s bx_param_handler(bx_param_c *param, int set, Bit64s val)
{
  char pname[BX_PATHNAME_LEN];
  Bit8u channel, device;

  bx_list_c *base = (bx_list_c*) param->get_parent();
  base->get_param_path(pname, BX_PATHNAME_LEN);
  if (!strncmp(pname, "ata.", 4)) {
    channel = pname[4] - '0';
    if (!strcmp(base->get_name(), "master")) {
      device = 0;
    } else {
      device = 1;
    }
    if (!strcmp(param->get_name(), "status")) {
      if ((set) && (SIM->get_init_done())) {
        Bit32u handle = DEV_hd_get_device_handle(channel, device);
        DEV_hd_set_cd_media_status(handle, val == BX_INSERTED);
        bx_gui->update_drive_status_buttons();
      }
    } else if (!strcmp(param->get_name(), "mode")) {
      if (set) {
        switch (val) {
          case BX_ATA_MODE_UNDOABLE:
          case BX_ATA_MODE_VOLATILE:
//        case BX_ATA_MODE_Z_UNDOABLE:
//        case BX_ATA_MODE_Z_VOLATILE:
            SIM->get_param("journal", base)->set_enabled(1);
            break;
          default:
            SIM->get_param("journal", base)->set_enabled(0);
          }
        }
    } else if (!strcmp(param->get_name(), "type")) {
      if (set) {
        switch (val) {
          case BX_ATA_DEVICE_DISK:
            SIM->get_param_num("present", base)->set(1);
            SIM->get_param("mode", base)->set_enabled(1);
            SIM->get_param("path", base)->set_enabled(1);
            //SIM->get_param("journal", base)->set_enabled(1);
            SIM->get_param("cylinders", base)->set_enabled(1);
            SIM->get_param("heads", base)->set_enabled(1);
            SIM->get_param("spt", base)->set_enabled(1);
            SIM->get_param("status", base)->set_enabled(0);
            SIM->get_param("model", base)->set_enabled(1);
            SIM->get_param("biosdetect", base)->set_enabled(1);
            SIM->get_param("translation", base)->set_enabled(1);
            SIM->get_param("path", base)->set_runtime_param(0);
            SIM->get_param("status", base)->set_runtime_param(0);
            break;
          case BX_ATA_DEVICE_CDROM:
            SIM->get_param_num("present", base)->set(1);
            SIM->get_param("mode", base)->set_enabled(0);
            SIM->get_param("path", base)->set_enabled(1);
            SIM->get_param("journal", base)->set_enabled(0);
            SIM->get_param("cylinders", base)->set_enabled(0);
            SIM->get_param("heads", base)->set_enabled(0);
            SIM->get_param("spt", base)->set_enabled(0);
            SIM->get_param("status", base)->set_enabled(1);
            SIM->get_param("model", base)->set_enabled(1);
            SIM->get_param("biosdetect", base)->set_enabled(1);
            SIM->get_param("translation", base)->set_enabled(0);
            SIM->get_param("path", base)->set_runtime_param(1);
            SIM->get_param("status", base)->set_runtime_param(1);
            break;
        }
      }
    }
  } else {
    param->get_param_path(pname, BX_PATHNAME_LEN);
    if (!strcmp(pname, BXPN_LOAD32BITOS_WHICH)) {
      if (set) {
        int enable = (val != Load32bitOSNone);
        SIM->get_param(BXPN_LOAD32BITOS_PATH)->set_enabled(enable);
        SIM->get_param(BXPN_LOAD32BITOS_IOLOG)->set_enabled(enable);
        SIM->get_param(BXPN_LOAD32BITOS_INITRD)->set_enabled(enable);
      }
    } else if (!strcmp(pname, BXPN_FLOPPYA_TYPE)) {
      if (set) {
        if (val == BX_FLOPPY_AUTO) {
          val = get_floppy_type_from_image(SIM->get_param_string(BXPN_FLOPPYA_PATH)->getptr());
          SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(val);
        } else if (!SIM->get_init_done()) {
          SIM->get_param_enum(BXPN_FLOPPYA_DEVTYPE)->set(val);
        }
      }
    } else if (!strcmp(pname, BXPN_FLOPPYB_TYPE)) {
      if (set) {
        if (val == BX_FLOPPY_AUTO) {
          val = get_floppy_type_from_image(SIM->get_param_string(BXPN_FLOPPYB_PATH)->getptr());
          SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(val);
        } else if (!SIM->get_init_done()) {
          SIM->get_param_enum(BXPN_FLOPPYB_DEVTYPE)->set(val);
        }
      }
    } else if (!strcmp(pname, BXPN_FLOPPYA_STATUS)) {
      if ((set) && (SIM->get_init_done())) {
        DEV_floppy_set_media_status(0, val == BX_INSERTED);
        bx_gui->update_drive_status_buttons();
      }
    } else if (!strcmp(pname, BXPN_FLOPPYB_STATUS)) {
      if ((set) && (SIM->get_init_done())) {
        DEV_floppy_set_media_status(1, val == BX_INSERTED);
        bx_gui->update_drive_status_buttons();
      }
    } else {
      BX_PANIC(("bx_param_handler called with unknown parameter '%s'", pname));
      return -1;
    }
  }
  return val;
}

const char *bx_param_string_handler(bx_param_string_c *param, int set, const char *val, int maxlen)
{
  char pname[BX_PATHNAME_LEN];
  Bit8u channel, device;

  int empty = 0;
  if ((strlen(val) < 1) || !strcmp ("none", val)) {
    empty = 1;
    val = "none";
  }
  bx_list_c *base = (bx_list_c*) param->get_parent();
  base->get_param_path(pname, BX_PATHNAME_LEN);
  if (!strncmp(pname, "ata.", 4)) {
    channel = pname[4] - '0';
    if (!strcmp(base->get_name(), "master")) {
      device = 0;
    } else {
      device = 1;
    }
    if (!strcmp(param->get_name(), "path")) {
      if (set==1) {
        if (SIM->get_init_done()) {
          Bit32u handle = DEV_hd_get_device_handle(channel, device);
          if (empty) {
            DEV_hd_set_cd_media_status(handle, 0);
            bx_gui->update_drive_status_buttons();
          } else {
            if (!SIM->get_param_num("present", base)->get()) {
              BX_ERROR(("Cannot add a cdrom drive at runtime"));
              SIM->get_param_num("present", base)->set(0);
            }
            if (SIM->get_param_num("type", base)->get() != BX_ATA_DEVICE_CDROM) {
              BX_ERROR(("Device is not a cdrom drive"));
              SIM->get_param_num("present", base)->set(0);
            }
          }
          if (DEV_hd_present() &&
              (SIM->get_param_num("status", base)->get() == BX_INSERTED) &&
              (SIM->get_param_num("type", base)->get() == BX_ATA_DEVICE_CDROM)) {
            // tell the device model that we removed, then inserted the cd
            DEV_hd_set_cd_media_status(handle, 0);
            DEV_hd_set_cd_media_status(handle, 1);
          }
        }
      }
    }
  } else {
    param->get_param_path(pname, BX_PATHNAME_LEN);
    if (!strcmp(pname, BXPN_SCREENMODE)) {
      if (set==1) {
        BX_INFO(("Screen mode changed to %s", val));
      }
    } else if (!strcmp(pname, BXPN_FLOPPYA_PATH)) {
      if (set==1) {
        if (SIM->get_init_done()) {
          if (empty) {
            DEV_floppy_set_media_status(0, 0);
            bx_gui->update_drive_status_buttons();
          } else {
            if (!SIM->get_param_num(BXPN_FLOPPYA_TYPE)->get_enabled()) {
              BX_ERROR(("Cannot add a floppy drive at runtime"));
              SIM->get_param_string(BXPN_FLOPPYA_PATH)->set("none");
            }
          }
          if ((DEV_floppy_present()) &&
              (SIM->get_param_enum(BXPN_FLOPPYA_STATUS)->get() == BX_INSERTED)) {
            // tell the device model that we removed, then inserted the disk
            DEV_floppy_set_media_status(0, 0);
            DEV_floppy_set_media_status(0, 1);
          }
        } else {
          SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set_enabled(!empty);
          SIM->get_param_enum(BXPN_FLOPPYA_STATUS)->set_enabled(!empty);
        }
      }
    } else if (!strcmp(pname, BXPN_FLOPPYB_PATH)) {
      if (set==1) {
        if (SIM->get_init_done()) {
          if (empty) {
            DEV_floppy_set_media_status(1, 0);
            bx_gui->update_drive_status_buttons();
          } else {
            if (!SIM->get_param_num(BXPN_FLOPPYB_TYPE)->get_enabled()) {
              BX_ERROR(("Cannot add a floppy drive at runtime"));
              SIM->get_param_string(BXPN_FLOPPYB_PATH)->set("none");
            }
          }
          if ((DEV_floppy_present()) &&
              (SIM->get_param_enum(BXPN_FLOPPYB_STATUS)->get() == BX_INSERTED)) {
            // tell the device model that we removed, then inserted the disk
            DEV_floppy_set_media_status(1, 0);
            DEV_floppy_set_media_status(1, 1);
          }
        } else {
          SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set_enabled(!empty);
          SIM->get_param_enum(BXPN_FLOPPYB_STATUS)->set_enabled(!empty);
        }
      }
    } else {
      BX_PANIC(("bx_param_string_handler called with unknown parameter '%s'", pname));
    }
  }
  return val;
}

static int bx_param_enable_handler (bx_param_c *param, int val)
{
  char pname[BX_PATHNAME_LEN];
  Bit8u channel, device;

  bx_list_c *base = (bx_list_c*) param->get_parent();
  base->get_param_path(pname, BX_PATHNAME_LEN);
  if (!strncmp(pname, "ata.", 4)) {
    channel = pname[4] - '0';
    if (!strcmp(base->get_name(), "master")) {
      device = 0;
    } else {
      device = 1;
    }
    if (!strcmp(param->get_name(), "status")) {
      if (val != 0) {
        switch (SIM->get_param_enum("type", base)->get()) {
          case BX_ATA_DEVICE_CDROM:
            return (1);
            break;
        }
      }
    } else if (!strcmp(param->get_name(), "journal")) {
      if (val != 0) {
        switch (SIM->get_param_enum("type", base)->get()) {
          case BX_ATA_DEVICE_DISK:
            switch (SIM->get_param_enum("mode", base)->get()) {
              case BX_ATA_MODE_UNDOABLE:
              case BX_ATA_MODE_VOLATILE:
//            case BX_ATA_MODE_Z_UNDOABLE:
//            case BX_ATA_MODE_Z_VOLATILE:
                return (1);
                break;
            }
        }
      }
      return (0);
    } else {
      BX_PANIC(("bx_param_enable_handler called with unknown parameter '%s'", pname));
    }
  } else {
    BX_PANIC(("bx_param_enable_handler called with unknown parameter '%s'", pname));
  }
  return val;
}

void bx_init_options()
{
  int i;
  bx_list_c *menu;
  bx_list_c *deplist;
  bx_param_num_c *ioaddr, *ioaddr2, *irq;
  bx_param_bool_c *enabled;
  bx_param_enum_c *mode, *status, *type, *ethmod;
  bx_param_string_c *macaddr, *ethdev;
  bx_param_filename_c *path;
  char name[BX_PATHNAME_LEN], descr[512], group[16], label[512];

  bx_param_c *root_param = SIM->get_param(".");

  // general options subtree
  menu = new bx_list_c(root_param, "general", "");

 // config interface option, set in bochsrc or command line
  static char *config_interface_list[] = {
#if BX_USE_TEXTCONFIG
    "textconfig",
#endif
#if BX_WITH_WX
    "wx",
#endif
    NULL
  };
  bx_param_enum_c *sel_config = new bx_param_enum_c(menu,
    "config_interface", "Configuration interface",
    "Select configuration interface",
    config_interface_list,
    0,
    0);
  sel_config->set_by_name(BX_DEFAULT_CONFIG_INTERFACE);

  // quick start option, set by command line arg
  new bx_param_enum_c(menu,
      "start_mode",
      "Bochs start types",
      "Bochs start types",
      bochs_start_names,
      BX_RUN_START,
      BX_QUICK_START);

#if BX_SUPPORT_SAVE_RESTORE
  new bx_param_bool_c(menu,
      "restore",
      "Restore Bochs session",
      "Restore Bochs session",
      0);
  new bx_param_string_c(menu,
    "restore_path",
    "Path to data for restore",
    "Path to data for restore",
    "",
    BX_PATHNAME_LEN);
#endif

  // subtree for special menus
  bx_list_c *special_menus = new bx_list_c(root_param, "menu", "");

#if BX_SUPPORT_SMP
  #define BX_CPU_PROCESSORS_LIMIT 8
  #define BX_CPU_CORES_LIMIT 4
  #define BX_CPU_HT_THREADS_LIMIT 4
#else
  #define BX_CPU_PROCESSORS_LIMIT 1
  #define BX_CPU_CORES_LIMIT 1
  #define BX_CPU_HT_THREADS_LIMIT 1
#endif

  // cpu subtree
  bx_list_c *cpu_param = new bx_list_c(root_param, "cpu", "CPU Options");

  // cpu options
  bx_param_num_c *nprocessors = new bx_param_num_c(cpu_param,
      "n_processors", "Number of CPUs in SMP mode",
      "Sets the number of CPUs for multiprocessor emulation",
      1, BX_CPU_PROCESSORS_LIMIT,
      1);
  nprocessors->set_enabled(BX_CPU_PROCESSORS_LIMIT > 1);
  bx_param_num_c *ncores = new bx_param_num_c(cpu_param,
      "n_cores", "Number of processor cores in each CPU in SMP mode",
      "Sets the number of processor cores per CPU for multiprocessor emulation",
      1, BX_CPU_CORES_LIMIT,
      1);
  ncores->set_enabled(BX_CPU_CORES_LIMIT > 1);
  bx_param_num_c *nthreads = new bx_param_num_c(cpu_param,
      "n_threads", "Number of HT threads per each process core in SMP mode",
      "Sets the number of HT (Intel(R) HyperThreading Technology) threads per core for multiprocessor emulation",
      1, BX_CPU_HT_THREADS_LIMIT,
      1);
  nthreads->set_enabled(BX_CPU_HT_THREADS_LIMIT > 1);
  new bx_param_num_c(cpu_param,
      "ips", "Emulated instructions per second (IPS)",
      "Emulated instructions per second, used to calibrate bochs emulated time with wall clock time.",
      1, BX_MAX_BIT32U,
      2000000);
#if BX_SUPPORT_SMP
  new bx_param_num_c(cpu_param,
      "quantum", "Quantum ticks in SMP simulation",
      "Maximum amount of instructions allowed to execute before returning control to another CPU.",
      BX_SMP_QUANTUM_MIN, BX_SMP_QUANTUM_MAX,
      5);
#endif
  new bx_param_bool_c(cpu_param,
      "reset_on_triple_fault", "Enable CPU reset on triple fault",
      "Enable CPU reset if triple fault occured (highly recommended)",
      1);
  cpu_param->get_options()->set(menu->SHOW_PARENT);

  // memory subtree
  bx_list_c *memory = new bx_list_c(root_param, "memory", "Memory Options");
  bx_list_c *stdmem = new bx_list_c(memory, "standard", "Standard Options");
  bx_list_c *optrom = new bx_list_c(memory, "optrom", "Optional ROM Images");
  bx_list_c *optram = new bx_list_c(memory, "optram", "Optional RAM Images");
  bx_list_c *ram = new bx_list_c(stdmem, "ram", "");
  bx_list_c *rom = new bx_list_c(stdmem, "rom", "");
  bx_list_c *vgarom = new bx_list_c(stdmem, "vgarom", "");

  // memory options (ram & rom)
  bx_param_num_c *ramsize = new bx_param_num_c(ram,
      "size",
      "Memory size (megabytes)",
      "Amount of RAM in megabytes",
      1, 2048,
      BX_DEFAULT_MEM_MEGS);
  ramsize->set_ask_format("Enter memory size (MB): [%d] ");
#if BX_WITH_WX
  ramsize->set_options(bx_param_num_c::USE_SPIN_CONTROL);
#endif

  path = new bx_param_filename_c(rom,
      "path",
      "ROM BIOS image",
      "Pathname of ROM image to load",
      "", BX_PATHNAME_LEN);
  path->set_format("Name of ROM BIOS image: %s");
  sprintf(name, "%s/BIOS-bochs-latest", get_builtin_variable("BXSHARE"));
  path->set_initial_val(name);
  bx_param_num_c *romaddr = new bx_param_num_c(rom,
      "addr",
      "ROM BIOS address",
      "The address at which the ROM image should be loaded",
      0, BX_MAX_BIT32U, 
      0);
  romaddr->set_base(16);
  romaddr->set_format("0x%05x");
  romaddr->set_long_format("ROM BIOS address: 0x%05x");

  path = new bx_param_filename_c(vgarom,
      "path",
      "VGA BIOS image",
      "Pathname of VGA ROM image to load",
      "", BX_PATHNAME_LEN);
  path->set_format("Name of VGA BIOS image: %s");
  sprintf(name, "%s/VGABIOS-lgpl-latest", get_builtin_variable("BXSHARE"));
  path->set_initial_val(name);

  bx_param_num_c *optaddr;

  for (i=0; i<BX_N_OPTROM_IMAGES; i++) {
    sprintf(name, "%d", i+1);
    sprintf(descr, "Pathname of optional ROM image #%d to load", i+1);
    sprintf(label, "Optional ROM image #%d", i+1);
    bx_list_c *optnum1 = new bx_list_c(optrom, name, label);
    path = new bx_param_filename_c(optnum1,
      "path", 
      "Path",
      descr,
      "", BX_PATHNAME_LEN);
    sprintf(label, "Name of optional ROM image #%d", i+1);
    strcat(label, " : %s");
    path->set_format(strdup(label));
    sprintf(descr, "The address at which the optional ROM image #%d should be loaded", i+1);
    optaddr = new bx_param_num_c(optnum1,
      "addr", 
      "Address",
      descr,
      0, BX_MAX_BIT32U, 
      0);
    optaddr->set_base(16);
    optaddr->set_format("0x%05x");
    sprintf(label, "Optional ROM #%d address:", i+1);
    strcat(label, " 0x%05x");
    optaddr->set_long_format(strdup(label));
    optnum1->get_options()->set(bx_list_c::SHOW_PARENT | bx_list_c::USE_BOX_TITLE);
  }
  optrom->get_options()->set(bx_list_c::SHOW_PARENT);

  for (i=0; i<BX_N_OPTRAM_IMAGES; i++) {
    sprintf(name, "%d", i+1);
    sprintf(descr, "Pathname of optional RAM image #%d to load", i+1);
    sprintf(label, "Optional RAM image #%d", i+1);
    bx_list_c *optnum2 = new bx_list_c(optram, name, label);
    path = new bx_param_filename_c(optnum2,
      "path", 
      "Path",
      descr,
      "", BX_PATHNAME_LEN);
    sprintf(label, "Name of optional RAM image #%d", i+1);
    strcat(label, " : %s");
    path->set_format(strdup(label));
    sprintf(descr, "The address at which the optional RAM image #%d should be loaded", i+1);
    optaddr = new bx_param_num_c(optnum2,
      "addr", 
      "Address",
      descr,
      0, BX_MAX_BIT32U, 
      0);
    optaddr->set_base(16);
    optaddr->set_format("0x%05x");
    sprintf(label, "Optional RAM #%d address:", i+1);
    strcat(label, " 0x%05x");
    optaddr->set_long_format(strdup(label));
    optnum2->get_options()->set(bx_list_c::SHOW_PARENT | bx_list_c::USE_BOX_TITLE);
  }
  optrom->get_options()->set(bx_list_c::SHOW_PARENT);
  memory->get_options()->set(bx_list_c::USE_TAB_WINDOW);

  bx_param_c *memory_init_list[] = {
    SIM->get_param(BXPN_MEM_SIZE),
    SIM->get_param(BXPN_ROM_PATH),
    SIM->get_param(BXPN_ROM_ADDRESS),
    SIM->get_param(BXPN_VGA_ROM_PATH),
    SIM->get_param("memory.optrom"),
    SIM->get_param("memory.optram"),
    NULL
  };
  menu = new bx_list_c(special_menus, "memory", "Bochs Memory Options", memory_init_list);
  menu->get_options()->set(bx_list_c::SHOW_PARENT);

  // clock & cmos subtree
  bx_list_c *clock_cmos = new bx_list_c(root_param, "clock_cmos", "Clock & CMOS Options");

  // clock & cmos options
  bx_param_enum_c *clock_sync = new bx_param_enum_c(clock_cmos,
      "clock_sync", "Synchronisation method",
      "Host to guest time synchronization method",
      clock_sync_names,
      BX_CLOCK_SYNC_NONE,
      BX_CLOCK_SYNC_NONE);
  bx_param_num_c *time0 = new bx_param_num_c(clock_cmos,
      "time0",
      "Initial CMOS time for Bochs\n(1:localtime, 2:utc, other:time in seconds)",
      "Initial time for Bochs CMOS clock, used if you really want two runs to be identical",
      0, BX_MAX_BIT32U,
      BX_CLOCK_TIME0_LOCAL);

  bx_list_c *cmosimage = new bx_list_c(clock_cmos, "cmosimage", "CMOS Image Options");
  bx_param_bool_c *use_cmosimage = new bx_param_bool_c(cmosimage,
      "enabled", "Use a CMOS image",
      "Controls the usage of a CMOS image",
      0);
  path = new bx_param_filename_c(cmosimage,
      "path", "Pathname of CMOS image",
      "Pathname of CMOS image",
      "", BX_PATHNAME_LEN);
  bx_param_bool_c *rtc_init = new bx_param_bool_c(cmosimage,
      "rtc_init", "Initialize RTC from image",
      "Controls whether to initialize the RTC with values stored in the image",
      0);
  deplist = new bx_list_c(NULL, 2);
  deplist->add(path);
  deplist->add(rtc_init);
  use_cmosimage->set_dependent_list(deplist);

  time0->set_ask_format("Enter Initial CMOS time (1:localtime, 2:utc, other:time in seconds): [%d] ");
  clock_sync->set_ask_format("Enter Synchronisation method: [%s] ");
  clock_cmos->get_options()->set(bx_list_c::SHOW_PARENT);
  cmosimage->get_options()->set(bx_list_c::SHOW_PARENT);

  // pci subtree
  bx_list_c *pci = new bx_list_c(root_param, "pci", "PCI Options");

  // pci options
  bx_param_c *pci_deps_list[1+BX_N_PCI_SLOTS];
  bx_param_c **pci_deps_ptr = &pci_deps_list[0];

  bx_param_bool_c *i440fx_support = new bx_param_bool_c(pci,
      "i440fx_support",
      "Enable i440FX PCI Support",
      "Controls whether to emulate the i440FX PCI chipset",
      BX_SUPPORT_PCI);
  // pci slots
  bx_list_c *slot = new bx_list_c(pci, "slot", "PCI Slots");
  *pci_deps_ptr++ = slot;
  for (i=0; i<BX_N_PCI_SLOTS; i++) {
    sprintf(name, "%d", i+1);
    sprintf (descr, "Name of the device connected to PCI slot #%d", i+1);
    sprintf (label, "PCI slot #%d device", i+1);
    bx_param_string_c *devname = new bx_param_string_c(slot,
        name, 
        label, 
        descr,
        "", BX_PATHNAME_LEN);
    // add to deplist
    *pci_deps_ptr++ = devname;
  }
  // pcidev options
  bx_list_c *pcidev = new bx_list_c(pci, "pcidev", "Host PCI Device Mapping");
  *pci_deps_ptr++ = pcidev;
  bx_param_num_c *pcivid = new bx_param_num_c(pcidev,
      "vendor",
      "PCI Vendor ID",
      "The vendor ID of the host PCI device to map",
      0, 0xffff,
      0xffff); // vendor id 0xffff = no pci device present
  pcivid->set_base(16);
  pcivid->set_format("0x%04x");
  pcivid->set_long_format("PCI Vendor ID: 0x%04x");
#if BX_SUPPORT_PCIDEV
  *pci_deps_ptr++ = pcivid;
#else
  pcivid->set_enabled(0);
#endif
  bx_param_num_c *pcidid = new bx_param_num_c(pcidev,
      "device",
      "PCI Device ID",
      "The device ID of the host PCI device to map",
      0, 0xffff,
      0x0);
  pcidid->set_base(16);
  pcidid->set_format("0x%04x");
  pcidid->set_long_format("PCI Device ID: 0x%04x");
#if BX_SUPPORT_PCIDEV
  *pci_deps_ptr++ = pcidid;
#else
  pcidid->set_enabled(0);
#endif
  // add final NULL at the end, and build the menu
  *pci_deps_ptr = NULL;
  i440fx_support->set_dependent_list(new bx_list_c(NULL, "", "", pci_deps_list));
  pci->get_options()->set(bx_list_c::SHOW_PARENT);
  slot->get_options()->set(bx_list_c::SHOW_PARENT);
  pcidev->get_options()->set(bx_list_c::SHOW_PARENT | bx_list_c::USE_BOX_TITLE);

  // display subtree
  bx_list_c *display = new bx_list_c(root_param, "display", "Bochs Display & Interface Options", 7);

  // this is a list of gui libraries that are known to be available at
  // compile time.  The one that is listed first will be the default,
  // which is used unless the user overrides it on the command line or
  // in a configuration file.
  static char *display_library_list[] = {
#if BX_WITH_X11
    "x",
#endif
#if BX_WITH_WIN32
    "win32",
#endif
#if BX_WITH_CARBON
    "carbon",
#endif
#if BX_WITH_BEOS
    "beos",
#endif
#if BX_WITH_MACOS
    "macos",
#endif
#if BX_WITH_AMIGAOS
    "amigaos",
#endif
#if BX_WITH_SDL
    "sdl",
#endif
#if BX_WITH_SVGA
    "svga",
#endif
#if BX_WITH_TERM
    "term",
#endif
#if BX_WITH_RFB
    "rfb",
#endif
#if BX_WITH_WX
    "wx",
#endif
#if BX_WITH_NOGUI
    "nogui",
#endif
    NULL
  };
  bx_param_enum_c *sel_displaylib = new bx_param_enum_c(display,
    "display_library", "VGA Display Library",
    "Select VGA Display Library",
    display_library_list,
    0,
    0);
  sel_displaylib->set_by_name(BX_DEFAULT_DISPLAY_LIBRARY);
  sel_displaylib->set_ask_format("Choose which library to use for the Bochs display: [%s] ");

  new bx_param_string_c(display,
    "displaylib_options", "Display Library options",
    "Options passed to Display Library",
    "",
    BX_PATHNAME_LEN);

  new bx_param_bool_c(display,
      "private_colormap", "Use a private colormap",
      "Request that the GUI create and use it's own non-shared colormap. This colormap will be used when in the bochs window. If not enabled, a shared colormap scheme may be used. Not implemented on all GUI's.",
      0);

  bx_param_bool_c *fullscreen = new bx_param_bool_c(display,
      "fullscreen", "Use full screen mode",
      "When enabled, bochs occupies the whole screen instead of just a window.",
      0);
  bx_param_string_c *screenmode = new bx_param_string_c(display,
      "screenmode",
      "Screen mode name",
      "Screen mode name",
      "", BX_PATHNAME_LEN);
  screenmode->set_handler(bx_param_string_handler);
#if !BX_WITH_AMIGAOS
  fullscreen->set_enabled(0);
  screenmode->set_enabled(0);
#endif

  bx_param_num_c *vga_update_interval = new bx_param_num_c(display,
      "vga_update_interval",
      "VGA Update Interval",
      "Number of microseconds between VGA updates",
      1, BX_MAX_BIT32U,
      40000);
  vga_update_interval->set_ask_format ("Type a new value for VGA update interval: [%d] ");

  bx_param_string_c *vga_extension = new bx_param_string_c(display,
                "vga_extension", 
                "VGA Extension", 
                "Name of the VGA extension",
                "none", BX_PATHNAME_LEN);
#if BX_SUPPORT_VBE
  vga_extension->set_initial_val("vbe");
#elif BX_SUPPORT_CLGD54XX
  vga_extension->set_initial_val("cirrus");
#endif
  display->get_options()->set(bx_list_c::SHOW_PARENT);

  // keyboard & mouse subtree
  bx_list_c *kbd_mouse = new bx_list_c(root_param, "keyboard_mouse", "Keyboard & Mouse Options");
  bx_list_c *keyboard = new bx_list_c(kbd_mouse, "keyboard", "Keyboard Options");
  bx_list_c *mouse = new bx_list_c(kbd_mouse, "mouse", "Mouse Options");

  // keyboard & mouse options
  type = new bx_param_enum_c(keyboard,
      "type", "Keyboard type",
      "Keyboard type reported by the 'identify keyboard' command",
      keyboard_type_names,
      BX_KBD_MF_TYPE,
      BX_KBD_XT_TYPE);
  type->set_ask_format ("Enter keyboard type: [%s] ");

  new bx_param_num_c(keyboard,
      "serial_delay", "Keyboard serial delay",
      "Approximate time in microseconds that it takes one character to be transfered from the keyboard to controller over the serial path.",
      1, BX_MAX_BIT32U,
      250);
  new bx_param_num_c(keyboard,
      "paste_delay", "Keyboard paste delay",
      "Approximate time in microseconds between attemps to paste characters to the keyboard controller.",
      1000, BX_MAX_BIT32U,
      100000);
  bx_param_bool_c *use_kbd_mapping = new bx_param_bool_c(keyboard,
      "use_mapping", "Use keyboard mapping",
      "Controls whether to use the keyboard mapping feature",
      0);
  bx_param_filename_c *keymap = new bx_param_filename_c(keyboard,
      "keymap", "Keymap filename",
      "Pathname of the keymap file used",
      "", BX_PATHNAME_LEN);
  deplist = new bx_list_c(NULL, 1);
  deplist->add(keymap);
  use_kbd_mapping->set_dependent_list(deplist);

  bx_param_string_c *user_shortcut = new bx_param_string_c(keyboard,
      "user_shortcut",
      "Userbutton shortcut",
      "Defines the keyboard shortcut to be sent when you press the 'user' button in the headerbar.",
      "none", 20);
  user_shortcut->set_runtime_param(1);

  static char *mouse_type_list[] = {
    "none",
    "ps2",
    "imps2",
#if BX_SUPPORT_BUSMOUSE
    "bus",
#endif
#if BX_SUPPORT_PCIUSB
    "usb",
#endif
    "serial",
    "serial_wheel",
    NULL
  };
  type = new bx_param_enum_c(mouse,
      "type", "Mouse type",
      "The mouse type can be one of these: 'none', 'ps2', 'imps2', 'serial', 'serial_wheel'"
#if BX_SUPPORT_BUSMOUSE
      ", 'bus'"
#endif
#if BX_SUPPORT_PCIUSB
      ", 'usb'"
#endif
      ,
      mouse_type_list,
      BX_MOUSE_TYPE_PS2,
      BX_MOUSE_TYPE_NONE);
  type->set_ask_format("Choose the type of mouse [%s] ");

  new bx_param_bool_c(mouse,
      "enabled", "Enable the mouse capture",
      "Controls whether the mouse sends events to the guest. The hardware emulation is always enabled.",
      0);
  kbd_mouse->get_options()->set(bx_list_c::SHOW_PARENT);
  keyboard->get_options()->set(bx_list_c::SHOW_PARENT);
  mouse->get_options()->set(bx_list_c::SHOW_PARENT);

  // boot parameter subtree
  bx_list_c *boot_params = new bx_list_c(root_param, "boot_params", "Boot Options");
  // boot sequence
  for (i=0; i<3; i++) {
    sprintf(name, "boot_drive%d", i+1);
    sprintf(label, "Boot drive #%d", i+1);
    sprintf(descr, "Name of drive #%d in boot sequence (A, C or CD)", i+1);
    bx_param_enum_c *bootdrive = new bx_param_enum_c(boot_params,
        name,
        label,
        descr,
        &bochs_bootdisk_names[(i==0)?BX_BOOT_FLOPPYA:BX_BOOT_NONE],
        (i==0)?BX_BOOT_FLOPPYA:BX_BOOT_NONE,
        (i==0)?BX_BOOT_FLOPPYA:BX_BOOT_NONE);
    bootdrive->set_ask_format("Boot from floppy drive, hard drive or cdrom ? [%s] ");
  }

  new bx_param_bool_c(boot_params,
      "floppy_sig_check",
      "Skip Floppy Boot Signature Check",
      "Skips check for the 0xaa55 signature on floppy boot device.",
      0);

  // loader hack
  bx_list_c *load32bitos = new bx_list_c(boot_params, "load32bitos", "32-bit OS Loader Hack");
  bx_param_enum_c *whichOS = new bx_param_enum_c(load32bitos,
      "which",
      "Which operating system?",
      "Which OS to boot",
      loader_os_names,
      Load32bitOSNone,
      Load32bitOSNone);
  path = new bx_param_filename_c(load32bitos,
      "path",
      "Pathname of OS to load",
      "Pathname of the 32-bit OS to load",
      "", BX_PATHNAME_LEN);
  bx_param_filename_c *iolog = new bx_param_filename_c(load32bitos,
      "iolog",
      "Pathname of I/O log file",
      "I/O logfile used for initializing the hardware",
      "", BX_PATHNAME_LEN);
  bx_param_filename_c *initrd = new bx_param_filename_c(load32bitos,
      "initrd",
      "Pathname of initrd",
      "Pathname of the initial ramdisk",
      "", BX_PATHNAME_LEN);
  whichOS->set_ask_format("Enter OS to load: [%s] ");
  path->set_ask_format("Enter pathname of OS: [%s]");
  iolog->set_ask_format("Enter pathname of I/O log: [%s] ");
  initrd->set_ask_format("Enter pathname of initrd: [%s] ");
  load32bitos->get_options()->set(menu->SERIES_ASK);
  whichOS->set_handler(bx_param_handler);
  whichOS->set(Load32bitOSNone);
  boot_params->get_options()->set(bx_list_c::SHOW_PARENT);

  // floppy subtree
  bx_list_c *floppy = new bx_list_c(root_param, "floppy", "Floppy Options");
  bx_list_c *floppya = new bx_list_c(floppy, "0", "Floppy Disk 0");
  bx_list_c *floppyb = new bx_list_c(floppy, "1", "Floppy Disk 1");

  bx_param_enum_c *devtype;
  // floppy options
  path = new bx_param_filename_c(floppya,
      "path",
      "First floppy image/device",
      "Pathname of first floppy image file or device.  If you're booting from floppy, this should be a bootable floppy.",
      "", BX_PATHNAME_LEN);
  path->set_ask_format("Enter new filename, or 'none' for no disk: [%s] ");
  path->set_runtime_param(1);

  devtype = new bx_param_enum_c(floppya,
      "devtype",
      "Type of floppy drive",
      "Type of floppy drive",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  devtype->set_enabled(0); // hide devtype parameter
  type = new bx_param_enum_c(floppya,
      "type",
      "Type of floppy disk",
      "Type of floppy disk",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  type->set_ask_format("What type of floppy disk? (auto=detect) [%s] ");
  type->set_runtime_param(1);

  status = new bx_param_enum_c(floppya,
      "status",
      "Is floppya inserted",
      "Inserted or ejected",
      floppy_status_names,
      BX_INSERTED,
      BX_EJECTED);
  status->set_ask_format("Is the floppy inserted or ejected? [%s] ");
  status->set_runtime_param(1);

  path->set_handler(bx_param_string_handler);
  type->set_handler(bx_param_handler);
  status->set_handler(bx_param_handler);
  path->set_initial_val("none");
  floppya->get_options()->set(bx_list_c::SERIES_ASK);

  path = new bx_param_filename_c(floppyb,
      "path",
      "Second floppy image/device",
      "Pathname of second floppy image file or device.",
      "", BX_PATHNAME_LEN);
  path->set_ask_format("Enter new filename, or 'none' for no disk: [%s] ");
  path->set_runtime_param(1);

  devtype = new bx_param_enum_c(floppyb,
      "devtype",
      "Type of floppy drive",
      "Type of floppy drive",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  devtype->set_enabled(0); // hide devtype parameter
  type = new bx_param_enum_c(floppyb,
      "type",
      "Type of floppy disk",
      "Type of floppy disk",
      floppy_type_names,
      BX_FLOPPY_NONE,
      BX_FLOPPY_NONE);
  type->set_ask_format("What type of floppy disk? (auto=detect) [%s] ");
  type->set_runtime_param(1);

  status = new bx_param_enum_c(floppyb,
      "status",
      "Is floppyb inserted",
      "Inserted or ejected",
      floppy_status_names,
      BX_INSERTED,
      BX_EJECTED);
  status->set_ask_format("Is the floppy inserted or ejected? [%s] ");
  status->set_runtime_param(1);

  path->set_handler(bx_param_string_handler);
  type->set_handler(bx_param_handler);
  status->set_handler(bx_param_handler);
  path->set_initial_val("none");
  floppyb->get_options()->set(bx_list_c::SERIES_ASK);
  floppy->get_options()->set(bx_list_c::SHOW_PARENT);

  // ATA/ATAPI subtree
  bx_list_c *ata = new bx_list_c(root_param, "ata", "ATA/ATAPI Options");

  // disk options
  char *s_atachannel[] = {
    "ATA channel 0",
    "ATA channel 1",
    "ATA channel 2",
    "ATA channel 3",
    };
  char *s_atadevname[2] = {
    "master",
    "slave",
  };
  char *s_atadevice[4][2] = {
    { "First HD/CD on channel 0",
      "Second HD/CD on channel 0" },
    { "First HD/CD on channel 1",
    "Second HD/CD on channel 1" },
    { "First HD/CD on channel 2",
    "Second HD/CD on channel 2" },
    { "First HD/CD on channel 3",
    "Second HD/CD on channel 3" }
    };
  Bit16u ata_default_ioaddr1[4] = {
    0x1f0, 0x170, 0x1e8, 0x168 
  };
  Bit16u ata_default_ioaddr2[4] = {
    0x3f0, 0x370, 0x3e0, 0x360 
  };
  Bit8u ata_default_irq[4] = { 
    14, 15, 11, 9 
  };

  #define BXP_PARAMS_PER_ATA_DEVICE 12

  bx_list_c *ata_menu[BX_MAX_ATA_CHANNEL];
  bx_list_c *ata_res[BX_MAX_ATA_CHANNEL];

  Bit8u channel;
  for (channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {

    sprintf(name, "%d", channel);
    ata_menu[channel] = new bx_list_c(ata, name, s_atachannel[channel]);
    ata_menu[channel]->get_options()->set(bx_list_c::USE_TAB_WINDOW);
    ata_res[channel] = new bx_list_c(ata_menu[channel], "resources", s_atachannel[channel], 8);
    ata_res[channel]->get_options()->set(bx_list_c::SERIES_ASK);

    enabled = new bx_param_bool_c(ata_res[channel],
      "enabled",                                
      "Enable ATA channel",
      "Controls whether ata channel is installed or not",
      0);
    enabled->set_ask_format("Channel is enabled: [%s] ");

    ioaddr = new bx_param_num_c(ata_res[channel],
      "ioaddr1",
      "I/O Address 1",
      "IO adress of ata command block",
      0, 0xffff,
      ata_default_ioaddr1[channel]);
    ioaddr->set_base(16);
    ioaddr->set_ask_format("Enter new ioaddr1: [0x%x] ");

    ioaddr2 = new bx_param_num_c(ata_res[channel],
      "ioaddr2",
      "I/O Address 2",
      "IO adress of ata control block",
      0, 0xffff,
      ata_default_ioaddr2[channel]);
    ioaddr2->set_base(16);
    ioaddr2->set_ask_format("Enter new ioaddr2: [0x%x] ");

    irq = new bx_param_num_c(ata_res[channel],
      "irq",
      "IRQ",
      "IRQ used by this ata channel",
      0, 15,
      ata_default_irq[channel]);
    irq->set_ask_format("Enter new IRQ: [%d] ");
    irq->set_options(bx_param_num_c::USE_SPIN_CONTROL);

    // all items in the ata[channel] menu depend on the enabled flag.
    // The menu list is complete, but a few dependent_list items will
    // be added later.  Use clone() to make a copy of the dependent_list
    // so that it can be changed without affecting the menu.
    enabled->set_dependent_list(ata_res[channel]->clone());

    for (Bit8u slave=0; slave<2; slave++) {
      menu = new bx_list_c(ata_menu[channel],
          s_atadevname[slave],
          s_atadevice[channel][slave],
          BXP_PARAMS_PER_ATA_DEVICE + 1);
      menu->get_options()->set(bx_list_c::SERIES_ASK);

      bx_param_bool_c *present = new bx_param_bool_c(menu,
        "present",                                
        "Enable this device",
        "Controls whether ata device is installed or not",
        0);
      present->set_ask_format("Device is enabled: [%s] ");

      type = new bx_param_enum_c(menu,
        "type",
        "Type of ATA device",
        "Type of ATA device (disk or cdrom)",
        atadevice_type_names,
        BX_ATA_DEVICE_DISK,
        BX_ATA_DEVICE_DISK);
      type->set_ask_format("Enter type of ATA device, disk or cdrom: [%s] ");

      path = new bx_param_filename_c(menu,
        "path",
        "Path or physical device name",
        "Pathname of the image or physical device (cdrom only)",
        "", BX_PATHNAME_LEN);
      path->set_ask_format("Enter new filename: [%s] ");

      mode = new bx_param_enum_c(menu,
        "mode",
        "Type of disk image",
        "Mode of the ATA harddisk",
        atadevice_mode_names,
        BX_ATA_MODE_FLAT,
        BX_ATA_MODE_FLAT);
      mode->set_ask_format("Enter mode of ATA device, (flat, concat, etc.): [%s] ");

      status = new bx_param_enum_c(menu,
        "status",
        "Inserted",
        "CD-ROM media status (inserted / ejected)",
        atadevice_status_names,
        BX_INSERTED,
        BX_EJECTED);
      status->set_ask_format("Is the device inserted or ejected? [%s] ");

      bx_param_filename_c *journal = new bx_param_filename_c(menu,
        "journal",
        "Path of journal file",
        "Pathname of the journal file",
        "", BX_PATHNAME_LEN);
      journal->set_ask_format("Enter path of journal file: [%s]");

      bx_param_num_c *cylinders = new bx_param_num_c(menu,
        "cylinders",
        "Cylinders",
        "Number of cylinders",
        0, 262143,
        0);
      cylinders->set_ask_format("Enter number of cylinders: [%d] ");
      bx_param_num_c *heads = new bx_param_num_c(menu,
        "heads",
        "Heads",
        "Number of heads",
        0, 255,
        0);
      heads->set_ask_format("Enter number of heads: [%d] ");
      bx_param_num_c *spt = new bx_param_num_c(menu,
        "spt",
        "Sectors per track",
        "Number of sectors per track",
        0, 255,
        0);
      spt->set_ask_format("Enter number of sectors per track: [%d] ");

      bx_param_string_c *model = new bx_param_string_c(menu,
        "model",
        "Model name",
        "String returned by the 'identify device' command",
        "Generic 1234", 40);
      model->set_ask_format("Enter new model name: [%s]");

      bx_param_enum_c *biosdetect = new bx_param_enum_c(menu,
        "biosdetect",
        "BIOS Detection",
        "Type of bios detection",
        atadevice_biosdetect_names,
        BX_ATA_BIOSDETECT_AUTO,
        BX_ATA_BIOSDETECT_NONE);
      biosdetect->set_ask_format("Enter bios detection type: [%s]");

      bx_param_enum_c *translation = new bx_param_enum_c(menu,
        "translation",
        "Translation type",
        "How the ata-disk translation is done by the bios",
        atadevice_translation_names,
        BX_ATA_TRANSLATION_AUTO,
        BX_ATA_TRANSLATION_NONE);
      translation->set_ask_format("Enter translation type: [%s]");

      // the menu and all items on it depend on the present flag
      present->set_dependent_list(menu->clone());
      // the present flag depends on the ATA channel's enabled flag
      enabled->get_dependent_list()->add(present);
      // the master/slave menu depends on the ATA channel's enabled flag
      enabled->get_dependent_list()->add(menu);

      type->set_handler(bx_param_handler);
      mode->set_handler(bx_param_handler);
      status->set_handler(bx_param_handler);
      path->set_handler(bx_param_string_handler);

      // Set the enable_hanlders
      journal->set_enable_handler(bx_param_enable_handler);
      status->set_enable_handler(bx_param_enable_handler);
    }

    // Enable two ATA interfaces by default, disable the others.
    // Now that the dependence relationships are established, call set() on
    // the ata device present params to set all enables correctly.
    enabled->set_initial_val(channel<2);
    enabled->set(channel<2);
  }

  // disk menu
  bx_param_c *disk_menu_init_list[] = {
    SIM->get_param(BXPN_FLOPPYA),
    SIM->get_param(BXPN_FLOPPYB),
    SIM->get_param(BXPN_ATA0_RES),
    SIM->get_param(BXPN_ATA0_MASTER),
    SIM->get_param(BXPN_ATA0_SLAVE),
#if BX_MAX_ATA_CHANNEL>1
    SIM->get_param(BXPN_ATA1_RES),
    SIM->get_param(BXPN_ATA1_MASTER),
    SIM->get_param(BXPN_ATA1_SLAVE),
#endif
#if BX_MAX_ATA_CHANNEL>2
    SIM->get_param(BXPN_ATA2_RES),
    SIM->get_param(BXPN_ATA2_MASTER),
    SIM->get_param(BXPN_ATA2_SLAVE),
#endif
#if BX_MAX_ATA_CHANNEL>3
    SIM->get_param(BXPN_ATA3_RES),
    SIM->get_param(BXPN_ATA3_MASTER),
    SIM->get_param(BXPN_ATA3_SLAVE),
#endif
    SIM->get_param("boot_params"),
    NULL
  };
  menu = new bx_list_c(special_menus, "disk", "Bochs Disk Options", disk_menu_init_list);
  menu->get_options()->set(bx_list_c::SHOW_PARENT);

  // ports subtree
  bx_list_c *ports = new bx_list_c(root_param, "ports", "Serial and Parallel Port Options");
  ports->get_options()->set(bx_list_c::USE_TAB_WINDOW | bx_list_c::SHOW_PARENT);

  // parallel ports
  bx_list_c *parallel = new bx_list_c(ports, "parallel", "Parallel Port Options");
  parallel->get_options()->set(bx_list_c::SHOW_PARENT);
  for (i=0; i<BX_N_PARALLEL_PORTS; i++) {
    sprintf(name, "%d", i+1);
    sprintf(label, "Parallel Port %d", i+1);
    menu = new bx_list_c(parallel, name, label);
    menu->get_options()->set(bx_list_c::SERIES_ASK);
    sprintf(label, "Enable parallel port #%d", i+1);
    sprintf(descr, "Controls whether parallel port #%d is installed or not", i+1);
    enabled = new bx_param_bool_c(menu, "enabled", label, descr,
      (i==0)? 1 : 0);  // only enable #1 by default
    sprintf(label, "Parallel port #%d output file", i+1);
    sprintf(descr, "Data written to parport#%d by the guest OS is written to this file", i+1);
    path = new bx_param_filename_c(menu, "outfile", label, descr,
      "", BX_PATHNAME_LEN);
    deplist = new bx_list_c(NULL, 1);
    deplist->add(path);
    enabled->set_dependent_list(deplist);
  }

  static char *serial_mode_list[] = {
    "null",
    "file",
    "term",
    "raw",
    "mouse",
    "socket",
    NULL
  };

  // serial ports
  bx_list_c *serial = new bx_list_c(ports, "serial", "Serial Port Options");
  serial->get_options()->set(bx_list_c::SHOW_PARENT);
  for (i=0; i<BX_N_SERIAL_PORTS; i++) {
    sprintf(name, "%d", i+1);
    sprintf(label, "Serial Port %d", i+1);
    menu = new bx_list_c(serial, name, label);
    menu->get_options()->set(bx_list_c::SERIES_ASK);
    sprintf(label, "Enable serial port #%d (COM%d)", i+1, i+1);
    sprintf(descr, "Controls whether COM%d is installed or not", i+1);
    enabled = new bx_param_bool_c(menu, "enabled", label, descr, 
      (i==0)?1 : 0);  // only enable the first by default
    sprintf(label, "I/O mode of the serial device for COM%d", i+1);
    sprintf(descr, "The mode can be one these: 'null', 'file', 'term', 'raw', 'mouse', 'socket'");
    mode = new bx_param_enum_c(menu, "mode", label, descr,
      serial_mode_list, 0, 0);
    mode->set_ask_format("Choose I/O mode of the serial device [%s] ");
    sprintf(label, "Pathname of the serial device for COM%d", i+1);
    sprintf(descr, "The path can be a real serial device or a pty (X/Unix only)");
    path = new bx_param_filename_c(menu, "dev", label, descr, 
      "", BX_PATHNAME_LEN);
    deplist = new bx_list_c(NULL, 2);
    deplist->add(mode);
    deplist->add(path);
    enabled->set_dependent_list(deplist);
  }

  bx_param_string_c *port, *option;

  // usb hubs
  bx_list_c *usb = new bx_list_c(ports, "usb", "USB Hub Options");
  usb->get_options()->set(bx_list_c::SHOW_PARENT);
  for (i=0; i<BX_N_USB_HUBS; i++) {
    sprintf(group, "USB%d", i+1);
    sprintf(name, "%d", i+1);
    sprintf(label, "USB Hub %d", i+1);
    menu = new bx_list_c(usb, name, label);
    menu->set_enabled(BX_SUPPORT_PCIUSB);
    sprintf(label, "Enable usb hub #%d", i+1);
    sprintf(descr, "Controls whether %s is installed or not", group);
    enabled = new bx_param_bool_c(menu, "enabled", label, descr, 0);
    enabled->set_enabled(BX_SUPPORT_PCIUSB);
    port = new bx_param_string_c(menu,
      "port1", 
      "Port #1 device", 
      "Device connected to USB port #1",
      "", BX_PATHNAME_LEN);
    port->set_group(group);
    option = new bx_param_string_c(menu,
      "option1", 
      "Port #1 device options", 
      "Options for device on USB port #1",
      "", BX_PATHNAME_LEN);
    option->set_group(group);
    port = new bx_param_string_c(menu,
      "port2", 
      "Port #2 device", 
      "Device connected to USB port #2",
      "", BX_PATHNAME_LEN);
    port->set_group(group);
    option = new bx_param_string_c(menu,
      "option2", 
      "Port #2 device options", 
      "Options for device on USB port #2",
      "", BX_PATHNAME_LEN);
    option->set_group(group);
    enabled->set_dependent_list(menu->clone());
  }

  // network subtree
  bx_list_c *network = new bx_list_c(root_param, "network", "Network Configuration");
  network->get_options()->set(bx_list_c::USE_TAB_WINDOW | bx_list_c::SHOW_PARENT);

  // ne2k & pnic options
  static char *eth_module_list[] = {
    "null",
#if defined(ETH_LINUX)
    "linux",
#endif
#if HAVE_ETHERTAP
    "tap",
#endif
#if HAVE_TUNTAP
    "tuntap",
#endif
#if defined(ETH_WIN32)
    "win32",
#endif
#if defined(ETH_FBSD)
    "fbsd",
#endif
#ifdef ETH_ARPBACK
    "arpback",
#endif
#if HAVE_VDE
    "vde",
#endif
    "vnet",
    NULL
  };
  // ne2k options
  menu = new bx_list_c(network, "ne2k", "NE2000", 7);
  menu->get_options()->set(bx_list_c::SHOW_PARENT);
  menu->set_enabled(BX_SUPPORT_NE2K);
  enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable NE2K NIC emulation",
    "Enables the NE2K NIC emulation",
    0);
  enabled->set_enabled(BX_SUPPORT_NE2K);
  ioaddr = new bx_param_num_c(menu,
    "ioaddr",
    "NE2K I/O Address",
    "I/O base address of the emulated NE2K device",
    0, 0xffff,
    0x300);
  ioaddr->set_base(16);
  irq = new bx_param_num_c(menu,
    "irq",
    "NE2K Interrupt",
    "IRQ used by the NE2K device",
    0, 15,
    9);
  irq->set_options(bx_param_num_c::USE_SPIN_CONTROL);
  macaddr = new bx_param_string_c(menu,
    "macaddr",
    "MAC Address",
    "MAC address of the NE2K device. Don't use an address of a machine on your net.",
    "\xfe\xfd\xde\xad\xbe\xef", 6);
  macaddr->get_options()->set(bx_param_string_c::RAW_BYTES);
  macaddr->set_separator(':');
  ethmod = new bx_param_enum_c(menu,
    "ethmod",
    "Ethernet module",
    "Module used for the connection to the real net.",
    eth_module_list,
    0,
    0);
  ethmod->set_by_name("null");
  ethmod->set_ask_format("Choose ethernet module for the NE2K [%s] ");
  ethdev = new bx_param_string_c(menu,
    "ethdev",
    "Ethernet device",
    "Device used for the connection to the real net. This is only valid if an ethernet module other than 'null' is used.",
    "xl0", BX_PATHNAME_LEN);
  path = new bx_param_filename_c(menu,
    "script",
    "Device configuration script",
    "Name of the script that is executed after Bochs initializes the network interface (optional).",
    "none", BX_PATHNAME_LEN);
    path->set_ask_format("Enter new script name, or 'none': [%s] ");
  enabled->set_dependent_list(menu->clone());
  // pnic options
  menu = new bx_list_c(network, "pnic", "PCI Pseudo NIC");
  menu->get_options()->set(bx_list_c::SHOW_PARENT);
  menu->set_enabled(BX_SUPPORT_PCIPNIC);
  enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable Pseudo NIC emulation",
    "Enables the Pseudo NIC emulation",
    0);
  enabled->set_enabled(BX_SUPPORT_PCIPNIC);
  macaddr = new bx_param_string_c(menu,
    "macaddr",
    "MAC Address",
    "MAC address of the Pseudo NIC device. Don't use an address of a machine on your net.",
    "\xfe\xfd\xde\xad\xbe\xef", 6);
  macaddr->get_options()->set(bx_param_string_c::RAW_BYTES);
  macaddr->set_separator(':');
  ethmod = new bx_param_enum_c(menu,
    "ethmod",
    "Ethernet module",
    "Module used for the connection to the real net.",
    eth_module_list,
    0,
    0);
  ethmod->set_by_name("null");
  ethmod->set_ask_format("Choose ethernet module for the Pseudo NIC [%s]");
  ethdev = new bx_param_string_c(menu,
    "ethdev",
    "Ethernet device",
    "Device used for the connection to the real net. This is only valid if an ethernet module other than 'null' is used.",
    "xl0", BX_PATHNAME_LEN);
  path = new bx_param_filename_c(menu,
    "script",
    "Device configuration script",
    "Name of the script that is executed after Bochs initializes the network interface (optional).",
    "none", BX_PATHNAME_LEN);
  path->set_ask_format("Enter new script name, or 'none': [%s] ");
  enabled->set_dependent_list(menu->clone());

  // sound subtree
  bx_list_c *sound = new bx_list_c(root_param, "sound", "Sound Configuration");
  sound->get_options()->set(bx_list_c::SHOW_PARENT);
  menu = new bx_list_c(sound, "sb16", "SB16 Configuration", 8);
  menu->get_options()->set(bx_list_c::SHOW_PARENT);
  menu->set_enabled(BX_SUPPORT_SB16);

  // SB16 options
  enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable SB16 emulation",
    "Enables the SB16 emulation",
    0);
  enabled->set_enabled(BX_SUPPORT_SB16);
  bx_param_num_c *midimode = new bx_param_num_c(menu,
    "midimode",
    "Midi mode",
    "Controls the MIDI output format.",
    0, 3,
    0);
  bx_param_filename_c *midifile = new bx_param_filename_c(menu,
    "midifile",
    "MIDI file",
    "The filename is where the MIDI data is sent. This can be device or just a file.",
    "", BX_PATHNAME_LEN);
  bx_param_num_c *wavemode = new bx_param_num_c(menu,
    "wavemode",
    "Wave mode",
    "Controls the wave output format.",
    0, 3,
    0);
  bx_param_filename_c *wavefile = new bx_param_filename_c(menu,
    "wavefile",
    "Wave file",
    "This is the device/file where the wave output is stored",
    "", BX_PATHNAME_LEN);
  bx_param_num_c *loglevel = new bx_param_num_c(menu,
    "loglevel",
    "Log level",
    "Controls how verbose the SB16 emulation is (0 = no log, 5 = all errors and infos).",
    0, 5,
    0);
  bx_param_filename_c *logfile = new bx_param_filename_c(menu,
    "logfile",
    "Log file",
    "The file to write the SB16 emulator messages to.",
    "", BX_PATHNAME_LEN);
  bx_param_num_c *dmatimer = new bx_param_num_c(menu,
    "dmatimer",
    "DMA timer",
    "Microseconds per second for a DMA cycle.",
    0, BX_MAX_BIT32U,
    0);

#if BX_WITH_WX
  midimode->set_options(bx_param_num_c::USE_SPIN_CONTROL);
  wavemode->set_options(bx_param_num_c::USE_SPIN_CONTROL);
  loglevel->set_options(bx_param_num_c::USE_SPIN_CONTROL);
#endif
  loglevel->set_group("SB16");
  dmatimer->set_group("SB16");
  enabled->set_dependent_list(menu->clone());
  deplist = new bx_list_c(NULL, 1);
  deplist->add(midifile);
  midimode->set_dependent_list(deplist);
  deplist = new bx_list_c(NULL, 1);
  deplist->add(wavefile);
  wavemode->set_dependent_list(deplist);
  deplist = new bx_list_c(NULL, 1);
  deplist->add(logfile);
  loglevel->set_dependent_list(deplist);

  // misc options subtree
  bx_list_c *misc = new bx_list_c(root_param, "misc", "Configure Everything Else");
  misc->get_options()->set(bx_list_c::SHOW_PARENT);
  bx_param_num_c *gdbstub_opt;

  // text snapshot check panic
  new bx_param_bool_c(misc,
      "text_snapshot_check",
      "Enable text snapshot check panic",
      "Enable panic when text on screen matches snapchk.txt.\nUseful for regression testing.\nIn win32, turns off CR/LF in snapshots and cuts.",
      0);
  // GDB stub
  menu = new bx_list_c(misc, "gdbstub", "GDB Stub Options");
  menu->get_options()->set(bx_list_c::SHOW_PARENT | bx_list_c::USE_BOX_TITLE);
  menu->set_enabled(BX_GDBSTUB);
  enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable GDB stub",
    "",
    0);
  enabled->set_enabled(BX_GDBSTUB);
  gdbstub_opt = new bx_param_num_c(menu,
    "port",
    "Port",
    "TCP/IP port for GDB stub",
    0, 65535,
    1234);
  gdbstub_opt = new bx_param_num_c(menu,
    "text_base",
    "Text base",
    "",
    0, BX_MAX_BIT32U,
    0);
  gdbstub_opt = new bx_param_num_c(menu,
    "data_base",
    "Data base",
    "",
    0, BX_MAX_BIT32U,
    0);
  gdbstub_opt = new bx_param_num_c(menu,
    "bss_base",
    "BSS base",
    "",
    0, BX_MAX_BIT32U,
    0);
  enabled->set_dependent_list(menu->clone());

  // log options subtree
  menu = new bx_list_c(root_param, "log", "Logfile Options");

  // log options
  path = new bx_param_filename_c(menu,
      "filename",
      "Log filename",
      "Pathname of bochs log file",
      "-", BX_PATHNAME_LEN);
  path->set_ask_format("Enter log filename: [%s] ");

  bx_param_string_c *prefix = new bx_param_string_c(menu,
      "prefix",
      "Log output prefix",
      "Prefix prepended to log output",
      "%t%e%d", BX_PATHNAME_LEN);
  prefix->set_ask_format("Enter log prefix: [%s] ");

  path = new bx_param_filename_c(menu,
      "debugger_filename",
      "Debugger Log filename",
      "Pathname of debugger log file",
      "-", BX_PATHNAME_LEN);
  path->set_ask_format("Enter debugger log filename: [%s] ");
  path->set_enabled(BX_DEBUGGER);

  // runtime options
  bx_param_c *runtime_init_list[] = {
      SIM->get_param_num(BXPN_VGA_UPDATE_INTERVAL),
      SIM->get_param_bool(BXPN_MOUSE_ENABLED),
      SIM->get_param_num(BXPN_KBD_PASTE_DELAY),
      SIM->get_param_string(BXPN_USER_SHORTCUT),
      SIM->get_param_num(BXPN_SB16_DMATIMER),
      SIM->get_param_num(BXPN_SB16_LOGLEVEL),
      SIM->get_param_string(BXPN_USB1_PORT1),
      SIM->get_param_string(BXPN_USB1_OPTION1),
      SIM->get_param_string(BXPN_USB1_PORT2),
      SIM->get_param_string(BXPN_USB1_OPTION2),
      NULL
  };
  menu = new bx_list_c(special_menus, "runtime", "Misc runtime options", runtime_init_list);
  menu->get_options()->set(bx_list_c::SHOW_PARENT | bx_list_c::SHOW_GROUP_NAME);
}

void bx_reset_options()
{
  // cpu
  SIM->get_param("cpu")->reset();

  // memory (ram & rom)
  SIM->get_param("memory")->reset();

  // clock & cmos
  SIM->get_param("clock_cmos")->reset();

  // pci
  SIM->get_param("pci")->reset();

  // display & interface
  SIM->get_param("display")->reset();

  // keyboard & mouse
  SIM->get_param("keyboard_mouse")->reset();

  // boot
  SIM->get_param("boot_params")->reset();

  // floppy drives
  SIM->get_param("floppy")->reset();

  // ata/atapi drives
  SIM->get_param("ata")->reset();

  // standard ports
  SIM->get_param("ports")->reset();

  // ne2k & pnic
  SIM->get_param("network")->reset();

  // SB16
  SIM->get_param("sound")->reset();

  // misc
  SIM->get_param("misc")->reset();

  // logfile
  SIM->get_param("log")->reset();
}

int bx_read_configuration(const char *rcfile)
{
  // parse rcfile first, then parse arguments in order.
  BX_INFO (("reading configuration from %s", rcfile));
  if (parse_bochsrc(rcfile) < 0) {
    BX_PANIC (("reading from %s failed", rcfile));
    return -1;
  }
  // update log actions
  for (int level=0; level<N_LOGLEV; level++) {
    int action = SIM->get_default_log_action (level);
    io->set_log_action (level, action);
  }
  return 0;
}

int bx_parse_cmdline (int arg, int argc, char *argv[])
{
  //if (arg < argc) BX_INFO (("parsing command line arguments"));

  while (arg < argc) {
    BX_INFO (("parsing arg %d, %s", arg, argv[arg]));
    parse_line_unformatted("cmdline args", argv[arg]);
    arg++;
  }
  // update log actions
  for (int level=0; level<N_LOGLEV; level++) {
    int action = SIM->get_default_log_action (level);
    io->set_log_action (level, action);
  }
  return 0;
}

#if BX_PROVIDE_MAIN
char *bx_find_bochsrc()
{
  FILE *fd = NULL;
  char rcfile[512];
  Bit32u retry = 0, found = 0;
  // try several possibilities for the bochsrc before giving up
  while (!found) {
    rcfile[0] = 0;
    switch (retry++) {
    case 0: strcpy (rcfile, ".bochsrc"); break;
    case 1: strcpy (rcfile, "bochsrc"); break;
    case 2: strcpy (rcfile, "bochsrc.txt"); break;
#ifdef WIN32
    case 3: strcpy (rcfile, "bochsrc.bxrc"); break;
#elif !BX_WITH_MACOS
      // only try this on unix
    case 3:
      {
        char *ptr = getenv("HOME");
        if (ptr) snprintf (rcfile, sizeof(rcfile), "%s/.bochsrc", ptr);
      }
      break;
    case 4: strcpy (rcfile, "/etc/bochsrc"); break;
#endif
    default:
      return NULL;
    }
    if (rcfile[0]) {
      BX_DEBUG (("looking for configuration in %s", rcfile));
      fd = fopen(rcfile, "r");
      if (fd) found = 1;
    }
  }
  assert (fd != NULL && rcfile[0] != 0);
  fclose (fd);
  return strdup(rcfile);
}

static int parse_bochsrc(const char *rcfile)
{
  FILE *fd = NULL;
  char *ret;
  char line[512];

  // try several possibilities for the bochsrc before giving up

  bochsrc_include_count++;

  fd = fopen (rcfile, "r");
  if (fd == NULL) return -1;

  int retval = 0;
  do {
    ret = fgets(line, sizeof(line)-1, fd);
    line[sizeof(line) - 1] = '\0';
    int len = strlen(line);
    if ((len>0) && (line[len-1] < ' '))
      line[len-1] = '\0';
    if ((ret != NULL) && strlen(line)) {
      if (parse_line_unformatted(rcfile, line) < 0) {
        retval = -1;
        break;  // quit parsing after first error
        }
      }
    } while (!feof(fd));
  fclose(fd);
  bochsrc_include_count--;
  return retval;
}

static char *get_builtin_variable(char *varname)
{
#ifdef WIN32
  int code;
  DWORD size;
  DWORD type = 0;
  HKEY hkey;
  char keyname[80];
  static char data[MAX_PATH];
#endif

  if (strlen(varname)<1) return NULL;
  else {
    if (!strcmp(varname, "BXSHARE")) {
#ifdef WIN32
      wsprintf(keyname, "Software\\Bochs %s", VER_STRING);
      code = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_READ, &hkey);
      if (code == ERROR_SUCCESS) {
        data[0] = 0;
        size = MAX_PATH;
        if (RegQueryValueEx(hkey, "", NULL, (LPDWORD)&type, (LPBYTE)data,
                            (LPDWORD)&size ) == ERROR_SUCCESS ) {
          RegCloseKey(hkey);
          return data;
        } else {
          RegCloseKey(hkey);
          return NULL;
        }
      } else {
        return NULL;
      }
#else
      return BX_SHARE_PATH;
#endif
    }
    return NULL;
  }
}

static Bit32s parse_line_unformatted(const char *context, char *line)
{
#define MAX_PARAMS_LEN 40
  char *ptr;
  unsigned i, string_i = 0;
  char string[512];
  char *params[MAX_PARAMS_LEN];
  int num_params;
  bx_bool inquotes = 0;
  bx_bool comment = 0;

  memset(params, 0, sizeof(params));
  if (line == NULL) return 0;

  // if passed nothing but whitespace, just return
  for (i=0; i<strlen(line); i++) {
    if (!isspace(line[i])) break;
    }
  if (i>=strlen(line))
    return 0;

  num_params = 0;

  if (!strncmp(line, "#include", 8))
    ptr = strtok(line, " ");
  else
    ptr = strtok(line, ":");
  while ((ptr) && (!comment)) {
    if (!inquotes) {
      string_i = 0;
    } else {
      string[string_i++] = ',';
    }
    for (i=0; i<strlen(ptr); i++) {
      if (ptr[i] == '"')
        inquotes = !inquotes;
      else if ((ptr[i] == '#') && (strncmp(line+i, "#include", 8)) && !inquotes) {
        comment = 1;
        break;
      } else {
#if BX_HAVE_GETENV
        // substitute environment variables.
        if (ptr[i] == '$') {
          char varname[512];
          char *pv = varname;
          char *value;
          *pv = 0;
          i++;
          while (isalpha(ptr[i]) || ptr[i]=='_') {
            *pv = ptr[i]; pv++; i++;
          }
          *pv = 0;
          if (strlen(varname)<1 || !(value = getenv(varname))) {
            if ((value = get_builtin_variable(varname))) {
              // append value to the string
              for (pv=value; *pv; pv++)
                  string[string_i++] = *pv;
            } else {
              BX_PANIC (("could not look up environment variable '%s'", varname));
            }
          } else {
            // append value to the string
            for (pv=value; *pv; pv++)
                string[string_i++] = *pv;
          }
        }
#endif
        if (!isspace(ptr[i]) || inquotes) {
          string[string_i++] = ptr[i];
        }
      }
    }
    string[string_i] = '\0';
    if (string_i == 0) break;
    if (!inquotes) {
      if (params[num_params] != NULL) {
        free(params[num_params]);
        params[num_params] = NULL;
      }
      if (num_params < MAX_PARAMS_LEN) {
        params[num_params++] = strdup(string);
      } else {
        BX_PANIC (("too many parameters, max is %d\n", MAX_PARAMS_LEN));
      }
    }
    ptr = strtok(NULL, ",");
  }
  Bit32s retval = parse_line_formatted(context, num_params, &params[0]);
  for (i=0; i < MAX_PARAMS_LEN; i++)
  {
    if ( params[i] != NULL )
    {
        free(params[i]);
        params[i] = NULL;
    }
  }
  return retval;
}

// These macros are called for all parse errors, so that we can easily
// change the behavior of all occurrences.
#define PARSE_ERR(x)  \
  do { BX_PANIC(x); return -1; } while (0)
#define PARSE_WARN(x)  \
  BX_ERROR(x)

/*
 * this supports the "floppyx: image=" option.
 * the functions returns the type of the floppy
 * image (1.44, 360, etc.), based on the image file size.
 */
int get_floppy_type_from_image(const char *filename)
{
  struct stat stat_buf;

  if (stat(filename, &stat_buf))
  {
    return BX_FLOPPY_NONE;
  }

  switch (stat_buf.st_size)
  {
    case 163840:
      return BX_FLOPPY_160K;

    case 184320:
      return BX_FLOPPY_180K;

    case 327680:
      return BX_FLOPPY_320K;

    case 368640:
      return BX_FLOPPY_360K;

    case 737280:
      return BX_FLOPPY_720K;

    case 1228800:
      return BX_FLOPPY_1_2;

    case 1474560:
    case 1720320:
    case 1763328:
    case 1884160:
      return BX_FLOPPY_1_44;

    case 2949120:
      return BX_FLOPPY_2_88;

    default:
      return BX_FLOPPY_UNKNOWN;
  }
}

static Bit32s parse_log_options(const char *context, char *loglev, char *param1)
{
  int level;

  if (!strcmp(loglev, "panic")) {
    level = LOGLEV_PANIC;
  } else if (!strcmp(loglev, "pass")) {
    level = LOGLEV_PASS;
  } else if (!strcmp(loglev, "error")) {
    level = LOGLEV_ERROR;
  } else if (!strcmp(loglev, "info")) {
    level = LOGLEV_INFO;
  } else { /* debug */
    level = LOGLEV_DEBUG;
  }
  if (strncmp(param1, "action=", 7)) {
    PARSE_ERR(("%s: %s directive malformed.", context, loglev));
  }
  char *action = param1 + 7;
  if (!strcmp(action, "fatal"))
    SIM->set_default_log_action (level, ACT_FATAL);
  else if (!strcmp (action, "report"))
    SIM->set_default_log_action (level, ACT_REPORT);
  else if (!strcmp (action, "ignore"))
    SIM->set_default_log_action (level, ACT_IGNORE);
  else if (!strcmp (action, "ask"))
    SIM->set_default_log_action (level, ACT_ASK);
  else {
    PARSE_ERR(("%s: %s directive malformed.", context, loglev));
  }
  return 0;
}

static Bit32s parse_line_formatted(const char *context, int num_params, char *params[])
{
  int i, slot, t;
  Bit8u idx;
  bx_list_c *base;

  if (num_params < 1) return 0;
  if (num_params < 2) {
    PARSE_ERR(("%s: a bochsrc option needs at least one parameter", context));
  }

  if (!strcmp(params[0], "#include")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: ignoring malformed #include directive.", context));
    }
    if (!strcmp(params[1], context)) {
      PARSE_ERR(("%s: cannot include this file again.", context));
    }
    if (bochsrc_include_count == 2) {
      PARSE_ERR(("%s: include directive in an included file not supported yet.", context));
    }
    bx_read_configuration(params[1]);
  }
  else if (!strcmp(params[0], "floppya")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "2_88=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(BX_FLOPPY_2_88);
      }
      else if (!strncmp(params[i], "1_44=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(BX_FLOPPY_1_44);
      }
      else if (!strncmp(params[i], "1_2=", 4)) {
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][4]);
        SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(BX_FLOPPY_1_2);
      }
      else if (!strncmp(params[i], "720k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(BX_FLOPPY_720K);
      }
      else if (!strncmp(params[i], "360k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(BX_FLOPPY_360K);
      }
      // use CMOS reserved types?
      else if (!strncmp(params[i], "160k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(BX_FLOPPY_160K);
      }
      else if (!strncmp(params[i], "180k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(BX_FLOPPY_180K);
      }
      else if (!strncmp(params[i], "320k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(BX_FLOPPY_320K);
      }
      else if (!strncmp(params[i], "image=", 6)) {
        /* "image=" means we should get floppy type from image */
        SIM->get_param_string(BXPN_FLOPPYA_PATH)->set(&params[i][6]);
        t = get_floppy_type_from_image(&params[i][6]);
        if (t != BX_FLOPPY_UNKNOWN)
          SIM->get_param_enum(BXPN_FLOPPYA_TYPE)->set(t);
        else
          PARSE_ERR(("%s: floppya image size doesn't match one of the supported types.", context));
      }
      else if (!strncmp(params[i], "status=", 7)) {
        SIM->get_param_enum(BXPN_FLOPPYA_STATUS)->set_by_name(&params[i][7]);
      }
      else {
        PARSE_ERR(("%s: floppya attribute '%s' not understood.", context,
          params[i]));
      }
    }
  }

  else if (!strcmp(params[0], "floppyb")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "2_88=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(BX_FLOPPY_2_88);
      }
      else if (!strncmp(params[i], "1_44=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(BX_FLOPPY_1_44);
      }
      else if (!strncmp(params[i], "1_2=", 4)) {
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][4]);
        SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(BX_FLOPPY_1_2);
      }
      else if (!strncmp(params[i], "720k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(BX_FLOPPY_720K);
      }
      else if (!strncmp(params[i], "360k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(BX_FLOPPY_360K);
      }
      // use CMOS reserved types?
      else if (!strncmp(params[i], "160k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(BX_FLOPPY_160K);
      }
      else if (!strncmp(params[i], "180k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(BX_FLOPPY_180K);
      }
      else if (!strncmp(params[i], "320k=", 5)) {
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][5]);
        SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(BX_FLOPPY_320K);
      }
      else if (!strncmp(params[i], "image=", 6)) {
        /* "image=" means we should get floppy type from image */
        SIM->get_param_string(BXPN_FLOPPYB_PATH)->set(&params[i][6]);
        t = get_floppy_type_from_image(&params[i][6]);
        if (t != BX_FLOPPY_UNKNOWN)
          SIM->get_param_enum(BXPN_FLOPPYB_TYPE)->set(t);
        else
          PARSE_ERR(("%s: floppyb image size doesn't match one of the supported types.", context));
      }
      else if (!strncmp(params[i], "status=", 7)) {
        SIM->get_param_enum(BXPN_FLOPPYB_STATUS)->set_by_name(&params[i][7]);
      }
      else {
        PARSE_ERR(("%s: floppyb attribute '%s' not understood.", context,
          params[i]));
      }
    }
  }

  else if ((!strncmp(params[0], "ata", 3)) && (strlen(params[0]) == 4)) {
    char tmpname[80];
    Bit8u channel = params[0][3];

    if ((channel < '0') || (channel > '9')) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
    }
    channel-='0';
    if (channel >= BX_MAX_ATA_CHANNEL) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
    }

    if ((num_params < 2) || (num_params > 5)) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
    }
    sprintf(tmpname, "ata.%d.resources", channel);
    base = (bx_list_c*) SIM->get_param(tmpname);
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: ataX directive malformed.", context));
    } else {
      SIM->get_param_bool("enabled", base)->set(atol(&params[1][8]));
    }

    if (num_params > 2) {
      if (strncmp(params[2], "ioaddr1=", 8)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
      }
      else {
        if ((params[2][8] == '0') && (params[2][9] == 'x'))
          SIM->get_param_num("ioaddr1", base)->set(strtoul(&params[2][8], NULL, 16));
        else
          SIM->get_param_num("ioaddr1", base)->set(strtoul(&params[2][8], NULL, 10));
      }
    }

    if (num_params > 3) {
      if (strncmp(params[3], "ioaddr2=", 8)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
      }
      else {
        if ((params[3][8] == '0') && (params[3][9] == 'x'))
          SIM->get_param_num("ioaddr2", base)->set(strtoul(&params[3][8], NULL, 16));
        else
          SIM->get_param_num("ioaddr2", base)->set(strtoul(&params[3][8], NULL, 10));
      }
    }

    if (num_params > 4) {
      if (strncmp(params[4], "irq=", 4)) {
        PARSE_ERR(("%s: ataX directive malformed.", context));
      }
      else {
        SIM->get_param_num("irq", base)->set(atol(&params[4][4]));
      }
    }
  }

  // ataX-master, ataX-slave
  else if ((!strncmp(params[0], "ata", 3)) && (strlen(params[0]) > 4)) {
    Bit8u channel = params[0][3];
    int type = 0, mode = BX_ATA_MODE_FLAT, biosdetect = BX_ATA_BIOSDETECT_AUTO;
    Bit32u cylinders = 0, heads = 0, sectors = 0;
    char tmpname[80];

    if ((channel < '0') || (channel > '9')) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
    }
    channel-='0';
    if (channel >= BX_MAX_ATA_CHANNEL) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
    }

    if ((strcmp(&params[0][4], "-slave")) &&
        (strcmp(&params[0][4], "-master"))) {
      PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
    }

    sprintf(tmpname, "ata.%d.%s", channel, &params[0][5]);
    base = (bx_list_c*) SIM->get_param(tmpname);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "type=", 5)) {
        type = SIM->get_param_enum("type", base)->find_by_name(&params[i][5]);
        if (type < 0) {
          PARSE_ERR(("%s: ataX-master/slave: unknown type '%s'", context, &params[i][5]));
        }
      } else if (!strcmp(params[i], "mode=z-undoable")) {
        PARSE_ERR(("%s: ataX-master/slave mode 'z-undoable' not implemented yet", context));
      } else if (!strcmp(params[i], "mode=z-volatile")) {
        PARSE_ERR(("%s: ataX-master/slave mode 'z-volatile' not implemented yet", context));
      } else if (!strncmp(params[i], "mode=", 5)) {
        mode = SIM->get_param_enum("mode", base)->find_by_name(&params[i][5]);
        if (mode < 0) {
          PARSE_ERR(("%s: ataX-master/slave: unknown mode '%s'", context, &params[i][5]));
        }
      } else if (!strncmp(params[i], "path=", 5)) {
        SIM->get_param_string("path", base)->set(&params[i][5]);
      } else if (!strncmp(params[i], "cylinders=", 10)) {
        cylinders = atol(&params[i][10]);
      } else if (!strncmp(params[i], "heads=", 6)) {
        heads = atol(&params[i][6]);
      } else if (!strncmp(params[i], "spt=", 4)) {
        sectors = atol(&params[i][4]);
      } else if (!strncmp(params[i], "model=", 6)) {
        SIM->get_param_string("model", base)->set(&params[i][6]);
      } else if (!strncmp(params[i], "biosdetect=", 11)) {
        biosdetect = SIM->get_param_enum("biosdetect", base)->find_by_name(&params[i][11]);
        if (biosdetect < 0) {
          PARSE_ERR(("%s: ataX-master/slave: unknown biosdetect '%s'", context, &params[i][11]));
        }
      } else if (!strcmp(params[i], "translation=none")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_NONE);
      } else if (!strcmp(params[i], "translation=lba")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_LBA);
      } else if (!strcmp(params[i], "translation=large")) { 
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_LARGE);
      } else if (!strcmp(params[i], "translation=echs")) { // synonym of large
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_LARGE);
      } else if (!strcmp(params[i], "translation=rechs")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_RECHS);
      } else if (!strcmp(params[i], "translation=auto")) {
        SIM->get_param_enum("translation", base)->set(BX_ATA_TRANSLATION_AUTO);
      } else if (!strcmp(params[i], "status=ejected")) {
        SIM->get_param_enum("status", base)->set(BX_EJECTED);
      } else if (!strcmp(params[i], "status=inserted")) {
        SIM->get_param_enum("status", base)->set(BX_INSERTED);
      } else if (!strncmp(params[i], "journal=", 8)) {
        SIM->get_param_string("journal", base)->set(&params[i][8]);
      } else {
        PARSE_ERR(("%s: ataX-master/slave directive malformed.", context));
      }
    }

    // Enables the ata device
    if (strlen(SIM->get_param_string("path", base)->getptr()) > 0) {
      SIM->get_param_bool("present", base)->set(1);
      SIM->get_param_enum("type", base)->set(type);
      SIM->get_param_enum("mode", base)->set(mode);
      SIM->get_param_num("cylinders", base)->set(cylinders);
      SIM->get_param_num("heads", base)->set(heads);
      SIM->get_param_num("spt", base)->set(sectors);
      SIM->get_param_num("biosdetect", base)->set(biosdetect);
    }

    // if enabled, check if device ok
    if (SIM->get_param_bool("present", base)->get() == 1) {
      if (SIM->get_param_enum("type", base)->get() == BX_ATA_DEVICE_DISK) {
        if ((SIM->get_param_num("cylinders", base)->get() == 0) &&
            (SIM->get_param_num("heads", base)->get() ==0 ) &&
            (SIM->get_param_num("spt", base)->get() == 0)) {
          PARSE_WARN(("%s: ataX-master/slave CHS set to 0/0/0 - autodetection enabled", context));
          // using heads = 16 and spt = 63 for autodetection (bximage defaults)
          SIM->get_param_num("heads", base)->set(16);
          SIM->get_param_num("spt", base)->set(63);
        }
      } else if (SIM->get_param_enum("type", base)->get() != BX_ATA_DEVICE_CDROM) {
        PARSE_WARN(("%s: ataX-master/slave: type should be specified", context));
      }
    }
  } else if (!strcmp(params[0], "boot")) {
    char tmppath[80];
    if (num_params < 2) {
      PARSE_ERR(("%s: boot directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      sprintf(tmppath, "boot_params.boot_drive%d", i);
      if (!strcmp(params[i], "none")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_NONE);
      } else if (!strcmp(params[i], "a")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_FLOPPYA);
      } else if (!strcmp(params[i], "floppy")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_FLOPPYA);
      } else if (!strcmp(params[i], "c")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_DISKC);
      } else if (!strcmp(params[i], "disk")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_DISKC);
      } else if (!strcmp(params[i], "cdrom")) {
        SIM->get_param_enum(tmppath)->set(BX_BOOT_CDROM);
      } else {
        PARSE_ERR(("%s: boot directive with unknown boot drive '%s'.  use 'floppy', 'disk' or 'cdrom'.", context, params[i]));
      }
    }
    if (SIM->get_param_enum(BXPN_BOOTDRIVE1)->get() == BX_BOOT_NONE) {
      PARSE_ERR(("%s: first boot drive must be one of 'floppy', 'disk' or 'cdrom'.", context));
    }
    if ((SIM->get_param_enum(BXPN_BOOTDRIVE1)->get() == SIM->get_param_enum(BXPN_BOOTDRIVE2)->get()) ||
        (SIM->get_param_enum(BXPN_BOOTDRIVE1)->get() == SIM->get_param_enum(BXPN_BOOTDRIVE3)->get()) ||
        ((SIM->get_param_enum(BXPN_BOOTDRIVE3)->get() != BX_BOOT_NONE) &&
        (SIM->get_param_enum(BXPN_BOOTDRIVE2)->get() == SIM->get_param_enum(BXPN_BOOTDRIVE3)->get()))) {
      PARSE_ERR(("%s: a boot drive appears twice in boot sequence.", context));
    }
  } else if (!strcmp(params[0], "floppy_bootsig_check")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
    }
    if (strncmp(params[1], "disabled=", 9)) {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
    }
    if (params[1][9] == '0')
      SIM->get_param_bool(BXPN_FLOPPYSIGCHECK)->set(0);
    else if (params[1][9] == '1')
      SIM->get_param_bool(BXPN_FLOPPYSIGCHECK)->set(1);
    else {
      PARSE_ERR(("%s: floppy_bootsig_check directive malformed.", context));
    }
  } else if (!strcmp(params[0], "log")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: log directive has wrong # args.", context));
    }
    SIM->get_param_string(BXPN_LOG_FILENAME)->set(params[1]);
  } else if (!strcmp(params[0], "logprefix")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: logprefix directive has wrong # args.", context));
    }
    SIM->get_param_string(BXPN_LOG_PREFIX)->set(params[1]);
  } else if (!strcmp(params[0], "debugger_log")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: debugger_log directive has wrong # args.", context));
    }
    SIM->get_param_string(BXPN_DEBUGGER_LOG_FILENAME)->set(params[1]);
  } else if (!strcmp(params[0], "panic")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: panic directive malformed.", context));
    }
    if (parse_log_options(context, params[0], params[1]) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "pass")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: pass directive malformed.", context));
    }
    if (parse_log_options(context, params[0], params[1]) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "error")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: error directive malformed.", context));
    }
    if (parse_log_options(context, params[0], params[1]) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "info")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: info directive malformed.", context));
    }
    if (parse_log_options(context, params[0], params[1]) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "debug")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: debug directive malformed.", context));
    }
    if (parse_log_options(context, params[0], params[1]) < 0) {
      return -1;
    }
  } else if (!strcmp(params[0], "cpu")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: cpu directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "count=", 6)) {
        unsigned processors = 1, cores = 1, threads = 1;
        sscanf(&params[i][6], "%u:%u:%u", &processors, &cores, &threads);
        unsigned smp_threads = cores*threads*processors;
        if (smp_threads > BX_MAX_SMP_THREADS_SUPPORTED) {
          PARSE_ERR(("%s: too many SMP threads defined, only %u threads supported",
            context, BX_MAX_SMP_THREADS_SUPPORTED));
        }
        if (smp_threads < 1) {
          PARSE_ERR(("%s: at least one CPU thread should be defined, cpu directive malformed", context));
        }
        SIM->get_param_num(BXPN_CPU_NPROCESSORS)->set(processors);
        SIM->get_param_num(BXPN_CPU_NCORES)->set(cores);
        SIM->get_param_num(BXPN_CPU_NTHREADS)->set(threads);
      } else if (!strncmp(params[i], "ips=", 4)) {
        SIM->get_param_num(BXPN_IPS)->set(atol(&params[i][4]));
        if (SIM->get_param_num(BXPN_IPS)->get() < BX_MIN_IPS) {
          PARSE_WARN(("%s: WARNING: ips is AWFULLY low!", context));
        }
#if BX_SUPPORT_SMP
      } else if (!strncmp(params[i], "quantum=", 8)) {
        SIM->get_param_num(BXPN_SMP_QUANTUM)->set(atol(&params[i][8]));
#endif
      } else if (!strncmp(params[i], "reset_on_triple_fault=", 22)) {
        if (params[i][22] == '0' || params[i][22] == '1') {
          SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT)->set (params[i][22] - '0');
        } else {
          PARSE_ERR(("%s: cpu directive malformed.", context));
        }
      } else {
        PARSE_ERR(("%s: cpu directive malformed.", context));
      }
    }
  } else if (!strcmp(params[0], "megs")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: megs directive: wrong # args.", context));
    }
    SIM->get_param_num(BXPN_MEM_SIZE)->set(atol(params[1]));
  } else if (!strcmp(params[0], "romimage")) {
    if ((num_params < 2) || (num_params > 3)) {
      PARSE_ERR(("%s: romimage directive: wrong # args.", context));
    }
    if (!strncmp(params[1], "file=", 5)) {
      SIM->get_param_string(BXPN_ROM_PATH)->set(&params[1][5]);
    } else {
      PARSE_ERR(("%s: romimage directive malformed.", context));
    }
    if (num_params == 3) {
      if (!strncmp(params[2], "address=", 8)) {
        if ((params[2][8] == '0') && (params[2][9] == 'x'))
          SIM->get_param_num(BXPN_ROM_ADDRESS)->set(strtoul(&params[2][8], NULL, 16));
        else
          SIM->get_param_num(BXPN_ROM_ADDRESS)->set(strtoul(&params[2][8], NULL, 10));
      } else {
        PARSE_ERR(("%s: romimage directive malformed.", context));
      }
    } else {
      SIM->get_param_num(BXPN_ROM_ADDRESS)->set (0);
    }
  } else if (!strcmp(params[0], "vgaromimage")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: vgaromimage directive: wrong # args.", context));
    }
    if (!strncmp(params[1], "file=", 5)) {
      SIM->get_param_string(BXPN_VGA_ROM_PATH)->set(&params[1][5]);
    } else {
      BX_INFO(("WARNING: syntax has changed, please use 'vgaromimage: file=...' now"));
      SIM->get_param_string(BXPN_VGA_ROM_PATH)->set(params[1]);
    }
  } else if (!strncmp(params[0], "optromimage", 11)) {
    int num = atoi(&params[0][11]);
    char tmppath[80], tmpaddr[80];
    if ((num < 1) || (num > BX_N_OPTROM_IMAGES)) {
      PARSE_ERR(("%s: optromimage%d: not supported", context, num));
    }
    if (num_params != 3) {
      PARSE_ERR(("%s: optromimage%d directive: wrong # args.", context, num));
    }
    sprintf(tmppath, "memory.optrom.%d.path", num);
    sprintf(tmpaddr, "memory.optrom.%d.addr", num);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        SIM->get_param_string(tmppath)->set(&params[i][5]);
      } else if (!strncmp(params[i], "address=", 8)) {
        if ((params[i][8] == '0') && (params[2][9] == 'x'))
          SIM->get_param_num(tmpaddr)->set(strtoul(&params[i][8], NULL, 16));
        else
          SIM->get_param_num(tmpaddr)->set(strtoul(&params[i][8], NULL, 10));
      } else {
        PARSE_ERR(("%s: optromimage%d directive malformed.", context, num));
      }
    }
  } else if (!strncmp(params[0], "optramimage", 11)) {
    int num = atoi(&params[0][11]);
    char tmppath[80], tmpaddr[80];
    if ((num < 1) || (num > BX_N_OPTRAM_IMAGES)) {
      PARSE_ERR(("%s: optramimage%d: not supported", context, num));
    }
    if (num_params != 3) {
      PARSE_ERR(("%s: optramimage%d directive: wrong # args.", context, num));
    }
    sprintf(tmppath, "memory.optram.%d.path", num);
    sprintf(tmpaddr, "memory.optram.%d.addr", num);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        SIM->get_param_string(tmppath)->set(&params[i][5]);
      } else if (!strncmp(params[i], "address=", 8)) {
        if ((params[i][8] == '0') && (params[2][9] == 'x'))
          SIM->get_param_num(tmpaddr)->set(strtoul(&params[i][8], NULL, 16));
        else
          SIM->get_param_num(tmpaddr)->set(strtoul(&params[i][8], NULL, 10));
      } else {
        PARSE_ERR(("%s: optramimage%d directive malformed.", context, num));
      }
    }
  } else if (!strcmp(params[0], "vga_update_interval")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: vga_update_interval directive: wrong # args.", context));
    }
    SIM->get_param_num(BXPN_VGA_UPDATE_INTERVAL)->set(atol(params[1]));
    if (SIM->get_param_num(BXPN_VGA_UPDATE_INTERVAL)->get() < 50000) {
      BX_INFO(("%s: vga_update_interval seems awfully small!", context));
    }
  } else if (!strcmp(params[0], "vga")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: vga directive: wrong # args.", context));
    }
    if (!strncmp(params[1], "extension=", 10)) {
      SIM->get_param_string(BXPN_VGA_EXTENSION)->set(&params[1][10]);
    }
  } else if (!strcmp(params[0], "keyboard_serial_delay")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_serial_delay directive: wrong # args.", context));
    }
    SIM->get_param_num(BXPN_KBD_SERIAL_DELAY)->set(atol(params[1]));
    if (SIM->get_param_num(BXPN_KBD_SERIAL_DELAY)->get() < 5) {
      PARSE_ERR (("%s: keyboard_serial_delay not big enough!", context));
    }
  } else if (!strcmp(params[0], "keyboard_paste_delay")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_paste_delay directive: wrong # args.", context));
    }
    SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->set(atol(params[1]));
    if (SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->get() < 1000) {
      PARSE_ERR (("%s: keyboard_paste_delay not big enough!", context));
    }
  } else if (!strcmp(params[0], "floppy_command_delay")) {
    PARSE_WARN(("%s: floppy_command_delay is DEPRECATED (now using hardware timing).", context));
  } else if (!strcmp(params[0], "ips")) {
    PARSE_WARN(("%s: ips directive is DEPRECATED (use cpu directive parameter 'ips').", context));
    if (num_params != 2) {
      PARSE_ERR(("%s: ips directive: wrong # args.", context));
    }
    SIM->get_param_num(BXPN_IPS)->set(atol(params[1]));
    if (SIM->get_param_num(BXPN_IPS)->get() < BX_MIN_IPS) {
      PARSE_WARN(("%s: WARNING: ips is AWFULLY low!", context));
    }
  } else if (!strcmp(params[0], "text_snapshot_check")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: text_snapshot_check directive: wrong # args.", context));
    }
    if (!strncmp(params[1], "enabled=", 8)) {
      if (params[1][8] == '0' || params[1][8] == '1')
        SIM->get_param_bool(BXPN_TEXT_SNAPSHOT_CHECK)->set(params[1][8] - '0');
      else
        PARSE_ERR(("%s: text_snapshot_check directive malformed.", context));
    } else {
      PARSE_ERR(("%s: text_snapshot_check directive malformed.", context));
    }
  } else if (!strcmp(params[0], "mouse")) {
    if (num_params < 2) {
      PARSE_ERR(("%s: mouse directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        if (params[i][8] == '0' || params[i][8] == '1')
          SIM->get_param_bool(BXPN_MOUSE_ENABLED)->set(params[i][8] - '0');
        else
          PARSE_ERR(("%s: mouse directive malformed.", context));
      } else if (!strncmp(params[i], "type=", 5)) {
        if (!SIM->get_param_enum(BXPN_MOUSE_TYPE)->set_by_name(&params[i][5]))
          PARSE_ERR(("%s: mouse type '%s' not available", context, &params[i][5]));
      } else {
        PARSE_ERR(("%s: mouse directive malformed.", context));
      }
    }
  } else if (!strcmp(params[0], "private_colormap")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
    }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
    }
    if (params[1][8] == '0' || params[1][8] == '1')
      SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->set(params[1][8] - '0');
    else {
      PARSE_ERR(("%s: private_colormap directive malformed.", context));
    }
  } else if (!strcmp(params[0], "fullscreen")) {
#if BX_WITH_AMIGAOS
    if (num_params != 2) {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
    }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
    }
    if (params[1][8] == '0' || params[1][8] == '1') {
      SIM->get_param_bool(BXPN_FULLSCREEN)->set(params[1][8] - '0');
    } else {
      PARSE_ERR(("%s: fullscreen directive malformed.", context));
    }
#endif
  } else if (!strcmp(params[0], "screenmode")) {
#if BX_WITH_AMIGAOS
    if (num_params != 2) {
      PARSE_ERR(("%s: screenmode directive malformed.", context));
    }
    if (strncmp(params[1], "name=", 5)) {
      PARSE_ERR(("%s: screenmode directive malformed.", context));
    }
    SIM->get_param_string(BXPN_SCREENMODE)->set(&params[1][5]);
#endif
  } else if (!strcmp(params[0], "sb16")) {
    int enable = 1;
    base = (bx_list_c*) SIM->get_param(BXPN_SB16);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        enable = atol(&params[i][8]);
      } else if (!strncmp(params[i], "midi=", 5)) {
        SIM->get_param_string("midifile", base)->set(&params[i][5]);
      } else if (!strncmp(params[i], "midimode=", 9)) {
        SIM->get_param_num("midimode", base)->set(atol(&params[i][9]));
      } else if (!strncmp(params[i], "wave=", 5)) {
        SIM->get_param_string("wavefile", base)->set(&params[i][5]);
      } else if (!strncmp(params[i], "wavemode=", 9)) {
        SIM->get_param_num("wavemode", base)->set(atol(&params[i][9]));
      } else if (!strncmp(params[i], "log=", 4)) {
        SIM->get_param_string("logfile", base)->set(&params[i][4]);
      } else if (!strncmp(params[i], "loglevel=", 9)) {
        SIM->get_param_num("loglevel", base)->set(atol(&params[i][9]));
      } else if (!strncmp(params[i], "dmatimer=", 9)) {
        SIM->get_param_num("dmatimer", base)->set(atol(&params[i][9]));
      } else {
        BX_ERROR(("%s: unknown parameter for sb16 ignored.", context));
      }
    }
    if ((enable != 0) && (SIM->get_param_num("dmatimer", base)->get() > 0))
      SIM->get_param_bool("enabled", base)->set(1);
    else
      SIM->get_param_bool("enabled", base)->set(0);
  } else if ((!strncmp(params[0], "com", 3)) && (strlen(params[0]) == 4)) {
    char tmpname[80];
    idx = params[0][3];
    if ((idx < '1') || (idx > '9')) {
      PARSE_ERR(("%s: comX directive malformed.", context));
    }
    idx -= '0';
    if (idx > BX_N_SERIAL_PORTS) {
      PARSE_ERR(("%s: comX port number out of range.", context));
    }
    sprintf(tmpname, "ports.serial.%d", idx);
    base = (bx_list_c*) SIM->get_param(tmpname);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool("enabled", base)->set(atol(&params[i][8]));
      } else if (!strncmp(params[i], "mode=", 5)) {
        if (!SIM->get_param_enum("mode", base)->set_by_name(&params[i][5]))
          PARSE_ERR(("%s: com%d serial port mode '%s' not available", context, idx, &params[i][5]));
        SIM->get_param_bool("enabled", base)->set(1);
      } else if (!strncmp(params[i], "dev=", 4)) {
        SIM->get_param_string("dev", base)->set(&params[i][4]);
        SIM->get_param_bool("enabled", base)->set(1);
      } else {
        PARSE_ERR(("%s: unknown parameter for com%d ignored.", context, idx));
      }
    }
  } else if ((!strncmp(params[0], "parport", 7)) && (strlen(params[0]) == 8)) {
    char tmpname[80];
    idx = params[0][7];
    if ((idx < '1') || (idx > '9')) {
      PARSE_ERR(("%s: parportX directive malformed.", context));
    }
    idx -= '0';
    if (idx > BX_N_PARALLEL_PORTS) {
      PARSE_ERR(("%s: parportX port number out of range.", context));
    }
    sprintf(tmpname, "ports.parallel.%d", idx);
    base = (bx_list_c*) SIM->get_param(tmpname);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool("enabled", base)->set(atol(&params[i][8]));
      } else if (!strncmp(params[i], "file=", 5)) {
        SIM->get_param_string("outfile", base)->set(&params[i][5]);
        SIM->get_param_bool("enabled", base)->set(1);
      } else {
        BX_ERROR(("%s: unknown parameter for parport%d ignored.", context, idx));
      }
    }
  } else if ((!strncmp(params[0], "usb", 3)) && (strlen(params[0]) == 4)) {
    char tmpname[80];
    idx = params[0][3];
    if ((idx < '1') || (idx > '9')) {
      PARSE_ERR(("%s: usbX directive malformed.", context));
    }
    idx -= '0';
    if (idx > BX_N_USB_HUBS) {
      PARSE_ERR(("%s: usbX hub number out of range.", context));
    }
    sprintf(tmpname, "ports.usb.%d", idx);
    base = (bx_list_c*) SIM->get_param(tmpname);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool("enabled", base)->set(atol(&params[i][8]));
      } else if (!strncmp(params[i], "port1=", 6)) {
        SIM->get_param_string("port1", base)->set(&params[i][6]);
      } else if (!strncmp(params[i], "option1=", 8)) {
        SIM->get_param_string("option1", base)->set(&params[i][8]);
      } else if (!strncmp(params[i], "port2=", 6)) {
        SIM->get_param_string("port2", base)->set(&params[i][6]);
      } else if (!strncmp(params[i], "option2=", 8)) {
        SIM->get_param_string("option2", base)->set(&params[i][8]);
      } else if (!strncmp(params[i], "ioaddr=", 7)) {
        PARSE_WARN(("%s: usb ioaddr is now DEPRECATED (assigned by BIOS).", context));
      } else if (!strncmp(params[i], "irq=", 4)) {
        PARSE_WARN(("%s: usb irq is now DEPRECATED (assigned by BIOS).", context));
      } else {
        PARSE_WARN(("%s: unknown parameter '%s' for usb%d ignored.", context, params[i], idx));
      }
    }
  } else if (!strcmp(params[0], "i440fxsupport")) {
    char tmpdev[80];
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool(BXPN_I440FX_SUPPORT)->set(atol(&params[i][8]));
      } else if ((!strncmp(params[i], "slot", 4)) && (params[i][5] == '=')) {
        slot = atol(&params[i][4]);
        if ((slot > 0) && (slot < 6)) {
          sprintf(tmpdev, "pci.slot.%d", slot);
          SIM->get_param_string(tmpdev)->set(&params[i][6]);
        } else {
          BX_ERROR(("%s: unknown pci slot number ignored.", context));
        }
      } else {
        PARSE_ERR(("%s: i440fxsupport: unknown parameter '%s'.", context, params[i]));
      }
    }
  } else if (!strcmp(params[0], "pcidev")) {
    if (num_params != 3) {
      PARSE_ERR(("%s: pcidev directive malformed.", context));
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "vendor=", 7)) {
        if ( (params[i][7] == '0') && (toupper(params[i][8]) == 'X') )
          SIM->get_param_num(BXPN_PCIDEV_VENDOR)->set(strtoul(&params[i][7], NULL, 16));
        else
          SIM->get_param_num(BXPN_PCIDEV_VENDOR)->set(strtoul(&params[i][7], NULL, 10));
      }
      else if (!strncmp(params[i], "device=", 7)) {
        if ( (params[i][7] == '0') && (toupper(params[i][8]) == 'X') )
          SIM->get_param_num(BXPN_PCIDEV_DEVICE)->set(strtoul(&params[i][7], NULL, 16));
        else
          SIM->get_param_num(BXPN_PCIDEV_DEVICE)->set(strtoul(&params[i][7], NULL, 10));
      }
      else {
        BX_ERROR(("%s: unknown parameter for pcidev ignored.", context));
      }
    }
  } else if (!strcmp(params[0], "cmosimage")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "file=", 5)) {
        SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->set(&params[i][5]);
      } else if (!strcmp(params[i], "rtc_init=time0")) {
        SIM->get_param_bool(BXPN_CMOSIMAGE_RTC_INIT)->set(0);
      } else if (!strcmp(params[i], "rtc_init=image")) {
        SIM->get_param_bool(BXPN_CMOSIMAGE_RTC_INIT)->set(1);
      } else {
        // for backward compatiblity
        SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->set(params[i]);
      }
    }
    if (strlen(SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr()) > 0) {
      SIM->get_param_bool(BXPN_CMOSIMAGE_ENABLED)->set(1);
    }
  } else if (!strcmp(params[0], "clock")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "sync=", 5)) {
        SIM->get_param_enum(BXPN_CLOCK_SYNC)->set_by_name(&params[i][5]);
      }
      else if (!strcmp(params[i], "time0=local")) {
        SIM->get_param_num(BXPN_CLOCK_TIME0)->set(BX_CLOCK_TIME0_LOCAL);
      }
      else if (!strcmp(params[i], "time0=utc")) {
        SIM->get_param_num(BXPN_CLOCK_TIME0)->set(BX_CLOCK_TIME0_UTC);
      }
      else if (!strncmp(params[i], "time0=", 6)) {
        SIM->get_param_num(BXPN_CLOCK_TIME0)->set(atoi(&params[i][6]));
      }
      else {
        BX_ERROR(("%s: unknown parameter for clock ignored.", context));
      }
    }
  }
  else if (!strcmp(params[0], "gdbstub")) {
#if BX_GDBSTUB
    if (num_params < 2) {
      PARSE_ERR(("%s: gdbstub directive: wrong # args.", context));
    }
    base = (bx_list_c*) SIM->get_param(BXPN_GDBSTUB);
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        if (params[i][8] == '0') {
          SIM->get_param_bool("enabled", base)->set(0);
          BX_INFO(("Disabled gdbstub"));
          bx_dbg.gdbstub_enabled = 0;
        }
        else if (params[i][8] == '1') {
          SIM->get_param_bool("enabled", base)->set(1);
          BX_INFO(("Enabled gdbstub"));
          bx_dbg.gdbstub_enabled = 1;
        }
        else {
          PARSE_ERR(("%s: gdbstub directive malformed.", context));
        }
      }
      else if (!strncmp(params[i], "port=", 5)) {
        SIM->get_param_num("port", base)->set(atoi(&params[i][5]));
      }
      else if (!strncmp(params[i], "text_base=", 10)) {
        SIM->get_param_num("text_base", base)->set(atoi(&params[i][10]));
      }
      else if (!strncmp(params[i], "data_base=", 10)) {
        SIM->get_param_num("data_base", base)->set(atoi(&params[i][10]));
      }
      else if (!strncmp(params[i], "bss_base=", 9)) {
        SIM->get_param_num("bss_base", base)->set(atoi(&params[i][9]));
      }
      else {
        PARSE_ERR(("%s: gdbstub directive malformed.", context));
      }
    }
#else
    PARSE_ERR(("%s: Bochs is not compiled with gdbstub support", context));
#endif
  }

#if BX_MAGIC_BREAKPOINT
  else if (!strcmp(params[0], "magic_break")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: magic_break directive: wrong # args.", context));
    }
    if (strncmp(params[1], "enabled=", 8)) {
      PARSE_ERR(("%s: magic_break directive malformed.", context));
    }
    if (params[1][8] == '0') {
      BX_INFO(("Ignoring magic break points"));
      bx_dbg.magic_break_enabled = 0;
    }
    else if (params[1][8] == '1') {
      BX_INFO(("Stopping on magic break points"));
      bx_dbg.magic_break_enabled = 1;
    }
    else {
      PARSE_ERR(("%s: magic_break directive malformed.", context));
    }
  }
#endif
  else if (!strcmp(params[0], "ne2k")) {
    int tmp[6];
    char tmpchar[6];
    int valid = 0;
    int n;
    base = (bx_list_c*) SIM->get_param(BXPN_NE2K);
    if (!SIM->get_param_bool("enabled", base)->get()) {
      SIM->get_param_enum("ethmod", base)->set_by_name("null");
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        if (atol(&params[i][8]) == 0) valid |= 0x80;
      }
      else if (!strncmp(params[i], "ioaddr=", 7)) {
        SIM->get_param_num("ioaddr", base)->set(strtoul(&params[i][7], NULL, 16));
        valid |= 0x01;
      }
      else if (!strncmp(params[i], "irq=", 4)) {
        SIM->get_param_num("irq", base)->set(atol(&params[i][4]));
        valid |= 0x02;
      }
      else if (!strncmp(params[i], "mac=", 4)) {
        n = sscanf(&params[i][4], "%x:%x:%x:%x:%x:%x",
                   &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
        if (n != 6) {
          PARSE_ERR(("%s: ne2k mac address malformed.", context));
        }
        for (n=0;n<6;n++)
          tmpchar[n] = (unsigned char)tmp[n];
        SIM->get_param_string("macaddr", base)->set(tmpchar);
        valid |= 0x04;
      }
      else if (!strncmp(params[i], "ethmod=", 7)) {
        if (!SIM->get_param_enum("ethmod", base)->set_by_name(&params[i][7]))
          PARSE_ERR(("%s: ethernet module '%s' not available", context, &params[i][7]));
      }
      else if (!strncmp(params[i], "ethdev=", 7)) {
        SIM->get_param_string("ethdev", base)->set(&params[i][7]);
      }
      else if (!strncmp(params[i], "script=", 7)) {
        SIM->get_param_string("script", base)->set(&params[i][7]);
      }
      else {
        PARSE_WARN(("%s: unknown parameter '%s' for ne2k ignored.", context, params[i]));
      }
    }
    if (!SIM->get_param_bool("enabled", base)->get()) {
      if (valid == 0x07) {
        SIM->get_param_bool("enabled", base)->set(1);
      } else if (valid < 0x80) {
        PARSE_ERR(("%s: ne2k directive incomplete (ioaddr, irq and mac are required)", context));
      }
    } else {
      if (valid & 0x80) {
        SIM->get_param_bool("enabled", base)->set(0);
      }
    }
  } else if (!strcmp(params[0], "pnic")) {
    int tmp[6];
    char tmpchar[6];
    int valid = 0;
    int n;
    base = (bx_list_c*) SIM->get_param(BXPN_PNIC);
    if (!SIM->get_param_bool("enabled", base)->get()) {
      SIM->get_param_enum("ethmod", base)->set_by_name("null");
    }
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        if (atol(&params[i][8]) == 0) valid |= 0x80;
      } else if (!strncmp(params[i], "ioaddr=", 7)) {
        PARSE_WARN(("%s: pnic ioaddr is now DEPRECATED (assigned by BIOS).", context));
      } else if (!strncmp(params[i], "irq=", 4)) {
        PARSE_WARN(("%s: pnic irq is now DEPRECATED (assigned by BIOS).", context));
      } else if (!strncmp(params[i], "mac=", 4)) {
        n = sscanf(&params[i][4], "%x:%x:%x:%x:%x:%x",
                   &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5]);
        if (n != 6) {
          PARSE_ERR(("%s: pnic mac address malformed.", context));
        }
        for (n=0;n<6;n++)
          tmpchar[n] = (unsigned char)tmp[n];
        SIM->get_param_string("macaddr", base)->set(tmpchar);
        valid |= 0x07;
      } else if (!strncmp(params[i], "ethmod=", 7)) {
        if (!SIM->get_param_enum("ethmod", base)->set_by_name(&params[i][7]))
          PARSE_ERR(("%s: ethernet module '%s' not available", context, &params[i][7]));
      } else if (!strncmp(params[i], "ethdev=", 7)) {
        SIM->get_param_string("ethdev", base)->set(&params[i][7]);
      } else if (!strncmp(params[i], "script=", 7)) {
        SIM->get_param_string("script", base)->set(&params[i][7]);
      } else {
        PARSE_WARN(("%s: unknown parameter '%s' for pnic ignored.", context, params[i]));
      }
    }
    if (!SIM->get_param_bool("enabled", base)->get()) {
      if (valid == 0x07) {
        SIM->get_param_bool("enabled", base)->set(1);
      } else if (valid < 0x80) {
        PARSE_ERR(("%s: pnic directive incomplete (mac is required)", context));
      }
    } else {
      if (valid & 0x80) {
        SIM->get_param_bool("enabled", base)->set(0);
      }
    }
  } else if (!strcmp(params[0], "load32bitOSImage")) {
    if ( (num_params!=4) && (num_params!=5) ) {
      PARSE_ERR(("%s: load32bitOSImage directive: wrong # args.", context));
    }
    if (strncmp(params[1], "os=", 3)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
    }
    if (!strcmp(&params[1][3], "nullkernel")) {
      SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->set(Load32bitOSNullKernel);
    }
    else if (!strcmp(&params[1][3], "linux")) {
      SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->set(Load32bitOSLinux);
    }
    else {
      PARSE_ERR(("%s: load32bitOSImage: unsupported OS.", context));
    }
    if (strncmp(params[2], "path=", 5)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
    }
    if (strncmp(params[3], "iolog=", 6)) {
      PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
    }
    SIM->get_param_string(BXPN_LOAD32BITOS_PATH)->set(&params[2][5]);
    SIM->get_param_string(BXPN_LOAD32BITOS_IOLOG)->set(&params[3][6]);
    if (num_params == 5) {
      if (strncmp(params[4], "initrd=", 7)) {
        PARSE_ERR(("%s: load32bitOSImage: directive malformed.", context));
      }
      SIM->get_param_string(BXPN_LOAD32BITOS_INITRD)->set(&params[4][7]);
    }
  }
  else if (!strcmp(params[0], "keyboard_type")) {
    if (num_params != 2) {
      PARSE_ERR(("%s: keyboard_type directive: wrong # args.", context));
    }
    if (!SIM->get_param_enum(BXPN_KBD_TYPE)->set_by_name(params[1])) {
      PARSE_ERR(("%s: keyboard_type directive: wrong arg '%s'.", context,params[1]));
    }
  }
  else if (!strcmp(params[0], "keyboard_mapping")
         ||!strcmp(params[0], "keyboardmapping")) {
    for (i=1; i<num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool(BXPN_KBD_USEMAPPING)->set(atol(&params[i][8]));
      }
      else if (!strncmp(params[i], "map=", 4)) {
        SIM->get_param_string(BXPN_KBD_KEYMAP)->set(&params[i][4]);
      }
    }
  }
  else if (!strcmp(params[0], "user_shortcut"))
  {
    if (num_params != 2) {
      PARSE_ERR(("%s: user_shortcut directive: wrong # args.", context));
    }
    if(!strncmp(params[1], "keys=", 4)) {
      SIM->get_param_string(BXPN_USER_SHORTCUT)->set(&params[1][5]);
      if ((strchr(&params[1][5], '-') == NULL) && (strlen(&params[1][5]) > 5))
        PARSE_WARN(("user_shortcut: old-style syntax detected"));
    } else {
      PARSE_ERR(("%s: user_shortcut directive malformed.", context));
    }
  }
  else if (!strcmp(params[0], "config_interface"))
  {
    if (num_params != 2) {
      PARSE_ERR(("%s: config_interface directive: wrong # args.", context));
    }
    if (!SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE)->set_by_name(params[1]))
      PARSE_ERR(("%s: config_interface '%s' not available", context, params[1]));
  }
  else if (!strcmp(params[0], "display_library")) {
    if ((num_params < 2) || (num_params > 3)) {
      PARSE_ERR(("%s: display_library directive: wrong # args.", context));
    }
    if (!SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY)->set_by_name(params[1]))
      PARSE_ERR(("%s: display library '%s' not available", context, params[1]));
    if (num_params == 3) {
      if (!strncmp(params[2], "options=", 8)) {
        SIM->get_param_string(BXPN_DISPLAYLIB_OPTIONS)->set(&params[2][8]);
      }
    }
  }
  // Old timing options have been replaced by the 'clock' option
  else if (!strcmp(params[0], "pit")) // Deprecated
  {
    PARSE_ERR(("ERROR: pit directive is DEPRECATED, use clock: instead"));
  }
  else if (!strcmp(params[0], "time0")) // Deprectated
  {
    PARSE_ERR(("ERROR: time0 directive is DEPRECATED, use clock: instead"));
  }
  // user-defined options handled by registered functions
  else if ((i = SIM->find_user_option(params[0])) >= 0)
  {
    return SIM->parse_user_option(i, context, num_params, &params[0]);
  }
  else
  {
    PARSE_ERR(( "%s: directive '%s' not understood", context, params[0]));
  }
  return 0;
}


static char *fdtypes[] = {
  "none", "1_2", "1_44", "2_88", "720k", "360k", "160k", "180k", "320k"
};

int bx_write_floppy_options(FILE *fp, int drive)
{
  char path[80], type[80], status[80];

  BX_ASSERT(drive==0 || drive==1);
  sprintf(path, "floppy.%d.path", drive);
  sprintf(type, "floppy.%d.type", drive);
  sprintf(status, "floppy.%d.status", drive);
  if (SIM->get_param_enum(type)->get() == BX_FLOPPY_NONE) {
    fprintf(fp, "# no floppy%c\n", (char)'a'+drive);
    return 0;
  }
  BX_ASSERT(SIM->get_param_enum(type)->get() > BX_FLOPPY_NONE &&
            SIM->get_param_enum(type)->get() <= BX_FLOPPY_LAST);
  fprintf(fp, "floppy%c: %s=\"%s\", status=%s\n", 
    (char)'a'+drive, 
    fdtypes[SIM->get_param_enum(type)->get() - BX_FLOPPY_NONE],
    SIM->get_param_string(path)->getptr(),
    SIM->get_param_enum(status)->get_selected());
  return 0;
}

int bx_write_ata_options(FILE *fp, Bit8u channel, bx_list_c *base)
{
  fprintf(fp, "ata%d: enabled=%d", channel, SIM->get_param_bool("enabled", base)->get());

  if (SIM->get_param_bool("enabled", base)->get()) {
    fprintf(fp, ", ioaddr1=0x%x, ioaddr2=0x%x, irq=%d", SIM->get_param_num("ioaddr1", base)->get(), 
      SIM->get_param_num("ioaddr2", base)->get(), SIM->get_param_num("irq", base)->get());
    }

  fprintf(fp, "\n");
  return 0;
}

int bx_write_atadevice_options(FILE *fp, Bit8u channel, Bit8u drive, bx_list_c *base)
{
  if (SIM->get_param_bool("present", base)->get()) {
    fprintf(fp, "ata%d-%s: ", channel, drive==0?"master":"slave");

    if (SIM->get_param_enum("type", base)->get() == BX_ATA_DEVICE_DISK) {
      fprintf(fp, "type=disk");

      fprintf(fp, ", mode=%s", SIM->get_param_enum("mode", base)->get_selected());
      fprintf(fp, ", translation=%s", SIM->get_param_enum("translation", base)->get_selected());
      fprintf(fp, ", path=\"%s\", cylinders=%d, heads=%d, spt=%d",
          SIM->get_param_string("path", base)->getptr(),
          SIM->get_param_num("cylinders", base)->get(),
          SIM->get_param_num("heads", base)->get(),
          SIM->get_param_num("spt", base)->get());

      if (SIM->get_param_string("journal", base)->getptr() != NULL)
        if (strcmp(SIM->get_param_string("journal", base)->getptr(), "") != 0)
          fprintf(fp, ", journal=\"%s\"", SIM->get_param_string("journal", base)->getptr());

    } else if (SIM->get_param_enum("type", base)->get() == BX_ATA_DEVICE_CDROM) {
      fprintf(fp, "type=cdrom, path=\"%s\", status=%s", 
        SIM->get_param_string("path", base)->getptr(),
        SIM->get_param_enum("status", base)->get_selected());
    }

    fprintf(fp, ", biosdetect=%s", SIM->get_param_enum("biosdetect", base)->get_selected());

    if (SIM->get_param_string("model", base)->getptr()>0) {
        fprintf(fp, ", model=\"%s\"", SIM->get_param_string("model", base)->getptr());
    }

    fprintf(fp, "\n");
  }
  return 0;
}

int bx_write_parport_options(FILE *fp, bx_list_c *base, int n)
{
  fprintf(fp, "parport%d: enabled=%d", n, SIM->get_param_bool("enabled", base)->get());
  if (SIM->get_param_bool("enabled", base)->get()) {
    fprintf(fp, ", file=\"%s\"", SIM->get_param_string("outfile", base)->getptr());
  }
  fprintf(fp, "\n");
  return 0;
}

int bx_write_serial_options(FILE *fp, bx_list_c *base, int n)
{
  fprintf(fp, "com%d: enabled=%d", n, SIM->get_param_bool("enabled", base)->get());
  if (SIM->get_param_bool("enabled", base)->get()) {
    fprintf(fp, ", mode=%s", SIM->get_param_enum("mode", base)->get_selected());
    fprintf(fp, ", dev=\"%s\"", SIM->get_param_string("dev", base)->getptr());
  }
  fprintf(fp, "\n");
  return 0;
}

int bx_write_usb_options(FILE *fp, bx_list_c *base, int n)
{
  fprintf(fp, "usb%d: enabled=%d", n, SIM->get_param_bool("enabled", base)->get());
  if (SIM->get_param_bool("enabled", base)->get()) {
    fprintf(fp, ", port1=%s, option1=%s", SIM->get_param_string("port1", base)->getptr(),
            SIM->get_param_string("option1", base)->getptr());
    fprintf(fp, ", port2=%s, option2=%s", SIM->get_param_string("port2", base)->getptr(),
            SIM->get_param_string("option2", base)->getptr());
  }
  fprintf(fp, "\n");
  return 0;
}

int bx_write_pnic_options(FILE *fp, bx_list_c *base)
{
  fprintf (fp, "pnic: enabled=%d", SIM->get_param_bool("enabled", base)->get());
  if (SIM->get_param_bool("enabled", base)->get()) {
    char *ptr = SIM->get_param_string("macaddr", base)->getptr();
    fprintf (fp, ", mac=%02x:%02x:%02x:%02x:%02x:%02x, ethmod=%s, ethdev=%s, script=%s",
      (unsigned int)(0xff & ptr[0]),
      (unsigned int)(0xff & ptr[1]),
      (unsigned int)(0xff & ptr[2]),
      (unsigned int)(0xff & ptr[3]),
      (unsigned int)(0xff & ptr[4]),
      (unsigned int)(0xff & ptr[5]),
      SIM->get_param_enum("ethmod", base)->get_selected(),
      SIM->get_param_string("ethdev", base)->getptr(),
      SIM->get_param_string("script", base)->getptr());
  }
  fprintf (fp, "\n");
  return 0;
}

int bx_write_ne2k_options (FILE *fp, bx_list_c *base)
{
  fprintf(fp, "ne2k: enabled=%d", SIM->get_param_bool("enabled", base)->get());
  if (SIM->get_param_bool("enabled", base)->get()) {
    char *ptr = SIM->get_param_string("macaddr", base)->getptr();
    fprintf(fp, ", ioaddr=0x%x, irq=%d, mac=%02x:%02x:%02x:%02x:%02x:%02x, ethmod=%s, ethdev=%s, script=%s",
      SIM->get_param_num("ioaddr", base)->get(), 
      SIM->get_param_num("irq", base)->get(),
      (unsigned int)(0xff & ptr[0]),
      (unsigned int)(0xff & ptr[1]),
      (unsigned int)(0xff & ptr[2]),
      (unsigned int)(0xff & ptr[3]),
      (unsigned int)(0xff & ptr[4]),
      (unsigned int)(0xff & ptr[5]),
      SIM->get_param_enum("ethmod", base)->get_selected(),
      SIM->get_param_string("ethdev", base)->getptr(),
      SIM->get_param_string("script", base)->getptr());
  }
  fprintf(fp, "\n");
  return 0;
}

int bx_write_sb16_options (FILE *fp, bx_list_c *base)
{
  fprintf(fp, "sb16: enabled=%d", SIM->get_param_bool("enabled", base)->get());
  if (SIM->get_param_bool("enabled", base)->get()) {
    fprintf(fp, ", midimode=%d, midi=%s, wavemode=%d, wave=%s, loglevel=%d, log=%s, dmatimer=%d",
      SIM->get_param_num("midimode", base)->get(),
      SIM->get_param_string("midifile", base)->getptr(),
      SIM->get_param_num("wavemode", base)->get(),
      SIM->get_param_string("wavefile", base)->getptr(),
      SIM->get_param_num("loglevel", base)->get(),
      SIM->get_param_string("logfile", base)->getptr(),
      SIM->get_param_num("dmatimer", base)->get());
  }
  fprintf(fp, "\n");
  return 0;
}

int bx_write_loader_options(FILE *fp)
{
  if (SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get() == Load32bitOSNone) {
    fprintf(fp, "# no loader\n");
    return 0;
  }
  BX_ASSERT((SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get() == Load32bitOSLinux) ||
            (SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get() == Load32bitOSNullKernel));
  fprintf (fp, "load32bitOSImage: os=%s, path=%s, iolog=%s, initrd=%s\n",
      (SIM->get_param_enum(BXPN_LOAD32BITOS_WHICH)->get() == Load32bitOSLinux) ? "linux" : "nullkernel",
      SIM->get_param_string(BXPN_LOAD32BITOS_PATH)->getptr(),
      SIM->get_param_string(BXPN_LOAD32BITOS_IOLOG)->getptr(),
      SIM->get_param_string(BXPN_LOAD32BITOS_INITRD)->getptr());
  return 0;
}

int bx_write_clock_cmos_options(FILE *fp)
{
  fprintf(fp, "clock: ");

  switch (SIM->get_param_enum(BXPN_CLOCK_SYNC)->get()) {
    case BX_CLOCK_SYNC_NONE:
      fprintf(fp, "sync=none");
      break;
    case BX_CLOCK_SYNC_REALTIME:
      fprintf(fp, "sync=realtime");
      break;
    case BX_CLOCK_SYNC_SLOWDOWN:
      fprintf(fp, "sync=slowdown");
      break;
    case BX_CLOCK_SYNC_BOTH:
      fprintf(fp, "sync=both");
      break;
    default:
      BX_PANIC(("Unknown value for sync method"));
  }

  switch (SIM->get_param_num(BXPN_CLOCK_TIME0)->get()) {
    case 0: break;
    case BX_CLOCK_TIME0_LOCAL: 
      fprintf(fp, ", time0=local");
      break;
    case BX_CLOCK_TIME0_UTC: 
      fprintf(fp, ", time0=utc");
      break;
    default: 
      fprintf(fp, ", time0=%u", SIM->get_param_num(BXPN_CLOCK_TIME0)->get());
  }

  fprintf(fp, "\n");

  if (strlen(SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr()) > 0) {
    fprintf(fp, "cmosimage: file=%s, ", SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr());
    fprintf(fp, "rtc_init=%s", SIM->get_param_bool(BXPN_CMOSIMAGE_RTC_INIT)->get()?"image":"time0");
  } else {
    fprintf(fp, "# no cmosimage\n");
  }
  return 0;
}

int bx_write_log_options(FILE *fp, bx_list_c *base)
{
  fprintf(fp, "log: %s\n", SIM->get_param_string("filename", base)->getptr());
  fprintf(fp, "logprefix: %s\n", SIM->get_param_string("prefix", base)->getptr());
  fprintf(fp, "debugger_log: %s\n", SIM->get_param_string("debugger_filename", base)->getptr());
  fprintf(fp, "panic: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_PANIC)));
  fprintf(fp, "error: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_ERROR)));
  fprintf(fp, "info: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_INFO)));
  fprintf(fp, "debug: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_DEBUG)));
  fprintf(fp, "pass: action=%s\n",
      io->getaction(logfunctions::get_default_action (LOGLEV_PASS)));
  return 0;
}

int bx_write_keyboard_options(FILE *fp)
{
  fprintf(fp, "keyboard_type: %s\n", SIM->get_param_enum(BXPN_KBD_TYPE)->get_selected());
  fprintf(fp, "keyboard_serial_delay: %u\n", SIM->get_param_num(BXPN_KBD_SERIAL_DELAY)->get());
  fprintf(fp, "keyboard_paste_delay: %u\n", SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->get());
  fprintf(fp, "keyboard_mapping: enabled=%d, map=%s\n",
    SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get(),
    SIM->get_param_string(BXPN_KBD_KEYMAP)->getptr());
  fprintf(fp, "user_shortcut: keys=%s\n", SIM->get_param_string(BXPN_USER_SHORTCUT)->getptr());
  return 0;
}

// return values:
//   0: written ok
//  -1: failed
//  -2: already exists, and overwrite was off
int bx_write_configuration(const char *rc, int overwrite)
{
  int i;
  char *strptr, tmppath[80], tmpaddr[80], tmpdev[80];
  bx_list_c *base;
  BX_INFO(("write current configuration to %s", rc));
  // check if it exists.  If so, only proceed if overwrite is set.
  FILE *fp = fopen(rc, "r");
  if (fp != NULL) {
    fclose(fp);
    if (!overwrite) return -2;
  }
  fp = fopen(rc, "w");
  if (fp == NULL) return -1;
  // finally it's open and we can start writing.
  fprintf(fp, "# configuration file generated by Bochs\n");
  fprintf(fp, "config_interface: %s\n", SIM->get_param_enum(BXPN_SEL_CONFIG_INTERFACE)->get_selected());
  fprintf(fp, "display_library: %s", SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY)->get_selected());
  strptr = SIM->get_param_string(BXPN_DISPLAYLIB_OPTIONS)->getptr();
  if (strlen(strptr) > 0)
    fprintf(fp, ", options=\"%s\"\n", strptr);
  else
    fprintf(fp, "\n");
  fprintf(fp, "megs: %d\n", SIM->get_param_num(BXPN_MEM_SIZE)->get());
  strptr = SIM->get_param_string(BXPN_ROM_PATH)->getptr();
  if (strlen(strptr) > 0)
    fprintf(fp, "romimage: file=\"%s\", address=0x%05x\n", strptr, (unsigned int)SIM->get_param_num(BXPN_ROM_ADDRESS)->get());
  else
    fprintf(fp, "# no romimage\n");
  strptr = SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr();
  if (strlen(strptr) > 0)
    fprintf(fp, "vgaromimage: file=\"%s\"\n", strptr);
  else
    fprintf(fp, "# no vgaromimage\n");
  fprintf(fp, "boot: %s", SIM->get_param_enum(BXPN_BOOTDRIVE1)->get_selected());
  for (i=1; i<3; i++) {
    sprintf(tmppath, "boot_params.boot_drive%d", i+1);
    if (SIM->get_param_enum(tmppath)->get() != BX_BOOT_NONE) {
      fprintf(fp, ", %s", SIM->get_param_enum(tmppath)->get_selected());
    }
  }
  fprintf(fp, "\n");
  fprintf(fp, "floppy_bootsig_check: disabled=%d\n", SIM->get_param_bool(BXPN_FLOPPYSIGCHECK)->get());
  // it would be nice to put this type of function as methods on
  // the structs like bx_floppy_options::print or something.
  bx_write_floppy_options(fp, 0);
  bx_write_floppy_options(fp, 1);
  for (Bit8u channel=0; channel<BX_MAX_ATA_CHANNEL; channel++) {
    sprintf(tmppath, "ata.%d", channel);
    base = (bx_list_c*) SIM->get_param(tmppath);
    bx_write_ata_options(fp, channel, (bx_list_c*) SIM->get_param("resources", base));
    bx_write_atadevice_options(fp, channel, 0, (bx_list_c*) SIM->get_param("master", base));
    bx_write_atadevice_options(fp, channel, 1, (bx_list_c*) SIM->get_param("slave", base));
  }
  for (i=0; i<BX_N_OPTROM_IMAGES; i++) {
    sprintf(tmppath, "memory.optrom.%d.path", i+1);
    sprintf(tmpaddr, "memory.optrom.%d.addr", i+1);
    strptr = SIM->get_param_string(tmppath)->getptr();
    if (strlen(strptr) > 0)
      fprintf(fp, "optromimage%d: file=\"%s\", address=0x%05x\n", i+1, strptr,
              (unsigned int)SIM->get_param_num(tmpaddr)->get());
  }
  for (i=0; i<BX_N_OPTRAM_IMAGES; i++) {
    sprintf(tmppath, "memory.optram.%d.path", i+1);
    sprintf(tmpaddr, "memory.optram.%d.addr", i+1);
    strptr = SIM->get_param_string(tmppath)->getptr();
    if (strlen(strptr) > 0)
      fprintf(fp, "optramimage%d: file=\"%s\", address=0x%05x\n", i+1, strptr,
              (unsigned int)SIM->get_param_num(tmpaddr)->get());
  }
  // parallel ports
  for (i=0; i<BX_N_PARALLEL_PORTS; i++) {
    sprintf(tmpdev, "ports.parallel.%d", i+1);
    base = (bx_list_c*) SIM->get_param(tmpdev);
    bx_write_parport_options(fp, base, i+1);
  }
  // serial ports
  for (i=0; i<BX_N_SERIAL_PORTS; i++) {
    sprintf(tmpdev, "ports.serial.%d", i+1);
    base = (bx_list_c*) SIM->get_param(tmpdev);
    bx_write_serial_options(fp, base, i+1);
  }
  base = (bx_list_c*) SIM->get_param("ports.usb.1");
  bx_write_usb_options(fp, base, 1);
  // pci
  fprintf(fp, "i440fxsupport: enabled=%d",
          SIM->get_param_bool(BXPN_I440FX_SUPPORT)->get());
  if (SIM->get_param_bool(BXPN_I440FX_SUPPORT)->get()) {
    for (i=0; i<BX_N_PCI_SLOTS; i++) {
      sprintf(tmpdev, "pci.slot.%d", i+1);
      strptr = SIM->get_param_string(tmpdev)->getptr();
      if (strlen(strptr) > 0) {
        fprintf(fp, ", slot%d=%s", i+1, strptr);
      }
    }
  }
  fprintf(fp, "\n");
  if (SIM->get_param_num(BXPN_PCIDEV_VENDOR)->get() != 0xffff) {
    fprintf(fp, "pcidev: vendor=0x%04x, device=0x%04x\n",
      SIM->get_param_num(BXPN_PCIDEV_VENDOR)->get(),
      SIM->get_param_num(BXPN_PCIDEV_DEVICE)->get());
  }
  fprintf(fp, "vga_update_interval: %u\n", SIM->get_param_num(BXPN_VGA_UPDATE_INTERVAL)->get());
  fprintf(fp, "vga: extension=%s\n", SIM->get_param_string(BXPN_VGA_EXTENSION)->getptr());
#if BX_SUPPORT_SMP
  fprintf(fp, "cpu: count=%u:%u:%u, ips=%u, quantum=%d, reset_on_triple_fault=%d\n", 
    SIM->get_param_num(BXPN_CPU_NPROCESSORS)->get(), SIM->get_param_num(BXPN_CPU_NCORES)->get(),
    SIM->get_param_num(BXPN_CPU_NTHREADS)->get(), SIM->get_param_num(BXPN_IPS)->get(),
    SIM->get_param_num(BXPN_SMP_QUANTUM)->get(),
    SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT)->get());
#else
  fprintf(fp, "cpu: count=1, ips=%u, reset_on_triple_fault=%d\n", 
    SIM->get_param_num(BXPN_IPS)->get(), SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT)->get());
#endif
  fprintf(fp, "text_snapshot_check: enabled=%d\n", SIM->get_param_bool(BXPN_TEXT_SNAPSHOT_CHECK)->get());
  fprintf(fp, "private_colormap: enabled=%d\n", SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get());
#if BX_WITH_AMIGAOS
  fprintf(fp, "fullscreen: enabled=%d\n", SIM->get_param_bool(BXPN_FULLSCREEN)->get());
  fprintf(fp, "screenmode: name=\"%s\"\n", SIM->get_param_string(BXPN_SCREENMODE)->getptr());
#endif
  bx_write_clock_cmos_options(fp);
  bx_write_ne2k_options(fp, (bx_list_c*) SIM->get_param(BXPN_NE2K));
  bx_write_pnic_options(fp, (bx_list_c*) SIM->get_param(BXPN_PNIC));
  bx_write_sb16_options(fp, (bx_list_c*) SIM->get_param(BXPN_SB16));
  bx_write_loader_options(fp);
  bx_write_log_options(fp, (bx_list_c*) SIM->get_param("log"));
  bx_write_keyboard_options(fp);
  fprintf(fp, "mouse: enabled=%d, type=%s\n",
    SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get(),
    SIM->get_param_enum(BXPN_MOUSE_TYPE)->get_selected());
  SIM->save_user_options(fp);
  fclose(fp);
  return 0;
}

#endif // #if BX_PROVIDE_MAIN
