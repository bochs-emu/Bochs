/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "cmos.h"
#include "virt_timer.h"

#define LOG_THIS theCmosDevice->

bx_cmos_c *theCmosDevice = NULL;

// CMOS register definitions from Ralf Brown's interrupt list v6.1, in a file
// called cmos.lst.  In cases where there are multiple uses for a given
// register in the interrupt list, I only listed the purpose that Bochs
// actually uses it for, but I wrote "alternatives" next to it.
#define  REG_SEC                     0x00
#define  REG_SEC_ALARM               0x01
#define  REG_MIN                     0x02
#define  REG_MIN_ALARM               0x03
#define  REG_HOUR                    0x04
#define  REG_HOUR_ALARM              0x05
#define  REG_WEEK_DAY                0x06
#define  REG_MONTH_DAY               0x07
#define  REG_MONTH                   0x08
#define  REG_YEAR                    0x09
#define  REG_STAT_A                  0x0a
#define  REG_STAT_B                  0x0b
#define  REG_STAT_C                  0x0c
#define  REG_STAT_D                  0x0d
#define  REG_DIAGNOSTIC_STATUS       0x0e  /* alternatives */
#define  REG_SHUTDOWN_STATUS         0x0f
#define  REG_EQUIPMENT_BYTE          0x14
#define  REG_CSUM_HIGH               0x2e
#define  REG_CSUM_LOW                0x2f
#define  REG_IBM_CENTURY_BYTE        0x32  /* alternatives */
#define  REG_IBM_PS2_CENTURY_BYTE    0x37  /* alternatives */

// Bochs CMOS map
//
// Idx  Len   Description
// 0x10   1   floppy drive types
// 0x11   1   configuration bits
// 0x12   1   harddisk types
// 0x13   1   advanced configuration bits
// 0x15   2   base memory in 1k
// 0x17   2   memory size above 1M in 1k
// 0x19   2   extended harddisk types
// 0x1b   9   harddisk configuration (hd0)
// 0x24   9   harddisk configuration (hd1)
// 0x2d   1   boot sequence (fd/hd)
// 0x30   2   memory size above 1M in 1k
// 0x34   2   memory size above 16M in 64k
// 0x38   1   eltorito boot sequence (#3) + bootsig check
// 0x39   2   ata translation policy (ata0...ata3)
// 0x3d   1   eltorito boot sequence (#1 + #2)
//
// Qemu CMOS map
//
// Idx  Len   Description
// 0x5b   3   extra memory above 4GB
// 0x5f   1   number of processors


Bit8u bcd_to_bin(Bit8u value, bx_bool is_binary)
{
  if (is_binary)
    return value;
  else
    return ((value >> 4) * 10) + (value & 0x0f);
}

Bit8u bin_to_bcd(Bit8u value, bx_bool is_binary)
{
  if (is_binary)
    return value;
  else
    return ((value  / 10) << 4) | (value % 10);
}

int CDECL libcmos_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    theCmosDevice = new bx_cmos_c();
    bx_devices.pluginCmosDevice = theCmosDevice;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theCmosDevice, BX_PLUGIN_CMOS);
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libcmos_LTX_plugin_fini(void)
{ 
  if (theCmosDevice != NULL) {
    delete theCmosDevice;
    theCmosDevice = NULL;
  }
}

bx_cmos_c::bx_cmos_c(void)
{
  put("CMOS");
  memset(&s, 0, sizeof(s));
  s.periodic_timer_index = BX_NULL_TIMER_HANDLE;
  s.one_second_timer_index = BX_NULL_TIMER_HANDLE;
  s.uip_timer_index = BX_NULL_TIMER_HANDLE;
}

bx_cmos_c::~bx_cmos_c(void)
{
  save_image();
  char *tmptime;
  if ((tmptime = strdup(ctime(&(BX_CMOS_THIS s.timeval)))) != NULL) {
    tmptime[strlen(tmptime)-1]='\0';
    BX_INFO(("Last time is %u (%s)", (unsigned) get_timeval(), tmptime));
    free(tmptime);
  }
  SIM->get_bochs_root()->remove("cmos");
  BX_DEBUG(("Exit"));
}

void bx_cmos_c::init(void)
{
  BX_DEBUG(("Init $Id$"));
  // CMOS RAM & RTC

  DEV_register_ioread_handler(this, read_handler, 0x0070, "CMOS RAM", 1);
  DEV_register_ioread_handler(this, read_handler, 0x0071, "CMOS RAM", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x0070, "CMOS RAM", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x0071, "CMOS RAM", 1);
  DEV_register_irq(8, "CMOS RTC");

  int clock_sync = SIM->get_param_enum(BXPN_CLOCK_SYNC)->get();
  BX_CMOS_THIS s.rtc_sync = ((clock_sync == BX_CLOCK_SYNC_REALTIME) ||
                             (clock_sync == BX_CLOCK_SYNC_BOTH)) &&
                            SIM->get_param_bool(BXPN_CLOCK_RTC_SYNC)->get();

  if (BX_CMOS_THIS s.periodic_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_CMOS_THIS s.periodic_timer_index =
      DEV_register_timer(this, periodic_timer_handler,
        1000000, 1,0, "cmos"); // continuous, not-active
  }
  if (BX_CMOS_THIS s.one_second_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_CMOS_THIS s.one_second_timer_index =
      bx_virt_timer.register_timer(this, one_second_timer_handler,
      1000000, 1, 0, BX_CMOS_THIS s.rtc_sync, "cmos"); // continuous, not-active
      if (BX_CMOS_THIS s.rtc_sync) {
        BX_INFO(("CMOS RTC using realtime synchronisation method"));
      }
  }
  if (BX_CMOS_THIS s.uip_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_CMOS_THIS s.uip_timer_index =
      DEV_register_timer(this, uip_timer_handler,
        244, 0, 0, "cmos"); // one-shot, not-active
  }

  if (SIM->get_param_num(BXPN_CLOCK_TIME0)->get() == BX_CLOCK_TIME0_LOCAL) {
    BX_INFO(("Using local time for initial clock"));
    BX_CMOS_THIS s.timeval = time(NULL);
  } else if (SIM->get_param_num(BXPN_CLOCK_TIME0)->get() == BX_CLOCK_TIME0_UTC) {
    bx_bool utc_ok = 0;

    BX_INFO(("Using utc time for initial clock"));

    BX_CMOS_THIS s.timeval = time(NULL);

#if BX_HAVE_GMTIME
#if BX_HAVE_MKTIME
    struct tm *utc_holder = gmtime(&BX_CMOS_THIS s.timeval);
    utc_holder->tm_isdst = -1;
    utc_ok = 1;
    BX_CMOS_THIS s.timeval = mktime(utc_holder);
#elif BX_HAVE_TIMELOCAL
    struct tm *utc_holder = gmtime(&BX_CMOS_THIS s.timeval);
    utc_holder->tm_isdst = 0;	// XXX Is this correct???
    utc_ok = 1;
    BX_CMOS_THIS s.timeval = timelocal(utc_holder);
#endif //BX_HAVE_MKTIME
#endif //BX_HAVE_GMTIME

    if (!utc_ok) {
      BX_ERROR(("UTC time is not supported on your platform. Using current time(NULL)"));
    }
  } else {
    BX_INFO(("Using specified time for initial clock"));
    BX_CMOS_THIS s.timeval = SIM->get_param_num(BXPN_CLOCK_TIME0)->get();
  }

  // load CMOS from image file if requested.
  if (SIM->get_param_bool(BXPN_CMOSIMAGE_ENABLED)->get()) {
    int fd, ret;
    struct stat stat_buf;

    fd = open(SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr(), O_RDONLY
#ifdef O_BINARY
       | O_BINARY
#endif
        );
    if (fd < 0) {
      BX_PANIC(("trying to open cmos image file '%s'",
        SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr()));
    }
    ret = fstat(fd, &stat_buf);
    if (ret) {
      BX_PANIC(("CMOS: could not fstat() image file."));
    }
    if ((stat_buf.st_size != 64) && (stat_buf.st_size != 128)) {
      BX_PANIC(("CMOS: image file size must be 64 or 128"));
    }

    ret = ::read(fd, (bx_ptr_t) BX_CMOS_THIS s.reg, (unsigned)stat_buf.st_size);
    if (ret != stat_buf.st_size) {
      BX_PANIC(("CMOS: error reading cmos file."));
    }
    close(fd);
    BX_INFO(("successfuly read from image file '%s'.",
      SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr()));
    BX_CMOS_THIS s.rtc_mode_12hour = ((BX_CMOS_THIS s.reg[REG_STAT_B] & 0x02) == 0);
    BX_CMOS_THIS s.rtc_mode_binary = ((BX_CMOS_THIS s.reg[REG_STAT_B] & 0x04) != 0);
    if (SIM->get_param_bool(BXPN_CMOSIMAGE_RTC_INIT)->get()) {
      update_timeval();
    } else {
      update_clock();
    }
  } else {
    // CMOS values generated
    BX_CMOS_THIS s.reg[REG_STAT_A] = 0x26;
    BX_CMOS_THIS s.reg[REG_STAT_B] = 0x02;
    BX_CMOS_THIS s.reg[REG_STAT_C] = 0x00;
    BX_CMOS_THIS s.reg[REG_STAT_D] = 0x80;
#if BX_SUPPORT_FPU == 1
    BX_CMOS_THIS s.reg[REG_EQUIPMENT_BYTE] |= 0x02;
#endif
    BX_CMOS_THIS s.rtc_mode_12hour = 0;
    BX_CMOS_THIS s.rtc_mode_binary = 0;
    update_clock();
  }

  char *tmptime;
  while((tmptime = strdup(ctime(&(BX_CMOS_THIS s.timeval)))) == NULL) {
    BX_PANIC(("Out of memory."));
  }
  tmptime[strlen(tmptime)-1]='\0';
  BX_INFO(("Setting initial clock to: %s (time0=%u)", tmptime, (Bit32u)BX_CMOS_THIS s.timeval));
  free(tmptime);

  BX_CMOS_THIS s.timeval_change = 0;

#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("cmos", this);
#endif
}

void bx_cmos_c::reset(unsigned type)
{
  BX_CMOS_THIS s.cmos_mem_address = 0;

  // RESET affects the following registers:
  //  CRA: no effects
  //  CRB: bits 4,5,6 forced to 0
  //  CRC: bits 4,5,6,7 forced to 0
  //  CRD: no effects
  BX_CMOS_THIS s.reg[REG_STAT_B] &= 0x8f;
  BX_CMOS_THIS s.reg[REG_STAT_C] = 0;

  // One second timer for updating clock & alarm functions
  bx_virt_timer.activate_timer(BX_CMOS_THIS s.one_second_timer_index,
                               1000000, 1);

  // handle periodic interrupt rate select
  BX_CMOS_THIS CRA_change();
}

void bx_cmos_c::save_image(void)
{
  int fd, ret;

  // save CMOS to image file if requested.
  if (SIM->get_param_bool(BXPN_CMOSIMAGE_ENABLED)->get()) {
    fd = open(SIM->get_param_string(BXPN_CMOSIMAGE_PATH)->getptr(), O_WRONLY
#ifdef O_BINARY
       | O_BINARY
#endif
        );
    ret = ::write(fd, (bx_ptr_t) BX_CMOS_THIS s.reg, 128);
    if (ret != 128) {
      BX_PANIC(("CMOS: error writing cmos file."));
    }
    close(fd);
  }
}

void bx_cmos_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "cmos", "CMOS State");
  BXRS_HEX_PARAM_FIELD(list, mem_address, BX_CMOS_THIS s.cmos_mem_address);
  bx_list_c *ram = new bx_list_c(list, "ram");
  for (unsigned i=0; i<128; i++) {
    char name[6];
    sprintf(name, "0x%02x", i);
    new bx_shadow_num_c(ram, name, &BX_CMOS_THIS s.reg[i], BASE_HEX);
  }
}

void bx_cmos_c::after_restore_state(void)
{
  BX_CMOS_THIS s.rtc_mode_12hour = ((BX_CMOS_THIS s.reg[REG_STAT_B] & 0x02) == 0);
  BX_CMOS_THIS s.rtc_mode_binary = ((BX_CMOS_THIS s.reg[REG_STAT_B] & 0x04) != 0);
  BX_CMOS_THIS update_timeval();
  BX_CMOS_THIS CRA_change();
}

void bx_cmos_c::CRA_change(void)
{
  Bit8u nibble, dcc;

  // Periodic Interrupt timer
  nibble = BX_CMOS_THIS s.reg[REG_STAT_A] & 0x0f;
  dcc = (BX_CMOS_THIS s.reg[REG_STAT_A] >> 4) & 0x07;
  if ((nibble == 0) || ((dcc & 0x06) == 0)) {
    // No Periodic Interrupt Rate when 0, deactivate timer
    bx_pc_system.deactivate_timer(BX_CMOS_THIS s.periodic_timer_index);
    BX_CMOS_THIS s.periodic_interval_usec = (Bit32u) -1; // max value
  } else {
    // values 0001b and 0010b are the same as 1000b and 1001b
    if (nibble <= 2)
      nibble += 7;
    BX_CMOS_THIS s.periodic_interval_usec = (unsigned) (1000000.0L /
      (32768.0L / (1 << (nibble - 1))));

    // if Periodic Interrupt Enable bit set, activate timer
    if (BX_CMOS_THIS s.reg[REG_STAT_B] & 0x40)
      bx_pc_system.activate_timer(BX_CMOS_THIS s.periodic_timer_index,
        BX_CMOS_THIS s.periodic_interval_usec, 1);
    else
      bx_pc_system.deactivate_timer(BX_CMOS_THIS s.periodic_timer_index);
  }
}


  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

Bit32u bx_cmos_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_CMOS_SMF
  bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_cmos_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif
  Bit8u ret8;

  BX_DEBUG(("CMOS read of CMOS register 0x%02x", (unsigned) BX_CMOS_THIS s.cmos_mem_address));

  switch (address) {
    case 0x0070:
      // this register is write-only on most machines
      BX_DEBUG(("read of index port 0x70. returning 0xff"));
      return(0xff);
    case 0x0071:
      ret8 = BX_CMOS_THIS s.reg[BX_CMOS_THIS s.cmos_mem_address];
      // all bits of Register C are cleared after a read occurs.
      if (BX_CMOS_THIS s.cmos_mem_address == REG_STAT_C) {
        BX_CMOS_THIS s.reg[REG_STAT_C] = 0x00;
        DEV_pic_lower_irq(8);
      }
      return(ret8);

    default:
      BX_PANIC(("unsupported cmos read, address=0x%04x!", (unsigned) address));
      return(0);
  }
}


// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions
void bx_cmos_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_CMOS_SMF
  bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_cmos_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_CMOS_SMF

  BX_DEBUG(("CMOS write to address: 0x%04x = 0x%02x", address, value));

  switch (address) {
    case 0x0070:
      BX_CMOS_THIS s.cmos_mem_address = value & 0x7F;
      break;

    case 0x0071:
      switch (BX_CMOS_THIS s.cmos_mem_address) {
        case REG_SEC_ALARM:             // seconds alarm
        case REG_MIN_ALARM:             // minutes alarm
        case REG_HOUR_ALARM:            // hours alarm
          BX_CMOS_THIS s.reg[BX_CMOS_THIS s.cmos_mem_address] = value;
          BX_DEBUG(("alarm time changed to %02x:%02x:%02x", BX_CMOS_THIS s.reg[REG_HOUR_ALARM],
                    BX_CMOS_THIS s.reg[REG_MIN_ALARM], BX_CMOS_THIS s.reg[REG_SEC_ALARM]));
          break;

        case REG_SEC:                   // seconds
        case REG_MIN:                   // minutes
        case REG_HOUR:                  // hours
        case REG_WEEK_DAY:              // day of the week
        case REG_MONTH_DAY:             // day of the month
        case REG_MONTH:                 // month
        case REG_YEAR:                  // year
        case REG_IBM_CENTURY_BYTE:      // century
        case REG_IBM_PS2_CENTURY_BYTE:  // century (PS/2)
          BX_CMOS_THIS s.reg[BX_CMOS_THIS s.cmos_mem_address] = value;
          if (BX_CMOS_THIS s.cmos_mem_address == REG_IBM_PS2_CENTURY_BYTE) {
            BX_CMOS_THIS s.reg[REG_IBM_CENTURY_BYTE] = value;
          }
          if (BX_CMOS_THIS s.reg[REG_STAT_B] & 0x80) {
            BX_CMOS_THIS s.timeval_change = 1;
          } else {
            update_timeval();
          }
          break;

        case REG_STAT_A: // Control Register A
          // bit 7: Update in Progress (read-only)
          //   1 = signifies time registers will be updated within 244us
          //   0 = time registers will not occur before 244us
          //   note: this bit reads 0 when CRB bit 7 is 1
          // bit 6..4: Divider Chain Control
          //   000 oscillator disabled
          //   001 oscillator disabled
          //   010 Normal operation
          //   011 TEST
          //   100 TEST
          //   101 TEST
          //   110 Divider Chain RESET
          //   111 Divider Chain RESET
          // bit 3..0: Periodic Interrupt Rate Select
          //   0000 None
          //   0001 3.90625  ms
          //   0010 7.8125   ms
          //   0011 122.070  us
          //   0100 244.141  us
          //   0101 488.281  us
          //   0110 976.562  us
          //   0111 1.953125 ms
          //   1000 3.90625  ms
          //   1001 7.8125   ms
          //   1010 15.625   ms
          //   1011 31.25    ms
          //   1100 62.5     ms
          //   1101 125      ms
          //   1110 250      ms
          //   1111 500      ms

          unsigned dcc;
          dcc = (value >> 4) & 0x07;
          if ((dcc & 0x06) == 0x06) {
            BX_INFO(("CRA: divider chain RESET"));
          } else if (dcc > 0x02) {
            BX_PANIC(("CRA: divider chain control 0x%02x", dcc));
          }
          BX_CMOS_THIS s.reg[REG_STAT_A] &= 0x80;
          BX_CMOS_THIS s.reg[REG_STAT_A] |= (value & 0x7f);
          BX_CMOS_THIS CRA_change();
          break;

        case REG_STAT_B: // Control Register B
          // bit 0: Daylight Savings Enable
          //   1 = enable daylight savings
          //   0 = disable daylight savings
          // bit 1: 24/12 hour mode
          //   1 = 24 hour format
          //   0 = 12 hour format
          // bit 2: Data Mode
          //   1 = binary format
          //   0 = BCD format
          // bit 3: "square wave enable"
          //   Not supported and always read as 0
          // bit 4: Update Ended Interrupt Enable
          //   1 = enable generation of update ended interrupt
          //   0 = disable
          // bit 5: Alarm Interrupt Enable
          //   1 = enable generation of alarm interrupt
          //   0 = disable
          // bit 6: Periodic Interrupt Enable
          //   1 = enable generation of periodic interrupt
          //   0 = disable
          // bit 7: Set mode
          //   1 = user copy of time is "frozen" allowing time registers
          //       to be accessed without regard for an occurance of an update
          //   0 = time updates occur normally

          if (value & 0x01)
            BX_ERROR(("write status reg B, daylight savings unsupported"));

          value &= 0xf7; // bit3 always 0
          // Note: setting bit 7 clears bit 4
          if (value & 0x80)
            value &= 0xef;

          unsigned prev_CRB;
          prev_CRB = BX_CMOS_THIS s.reg[REG_STAT_B];
          BX_CMOS_THIS s.reg[REG_STAT_B] = value;
          if ((prev_CRB & 0x02) != (value & 0x02)) {
            BX_CMOS_THIS s.rtc_mode_12hour = ((value & 0x02) == 0);
            update_clock();
          }
          if ((prev_CRB & 0x04) != (value & 0x04)) {
            BX_CMOS_THIS s.rtc_mode_binary = ((value & 0x04) != 0);
            update_clock();
          }
          if ((prev_CRB & 0x40) != (value & 0x40)) {
            // Periodic Interrupt Enabled changed
            if (prev_CRB & 0x40) {
              // transition from 1 to 0, deactivate timer
              bx_pc_system.deactivate_timer(BX_CMOS_THIS s.periodic_timer_index);
            } else {
              // transition from 0 to 1
              // if rate select is not 0, activate timer
              if ((BX_CMOS_THIS s.reg[REG_STAT_A] & 0x0f) != 0) {
                bx_pc_system.activate_timer(
                  BX_CMOS_THIS s.periodic_timer_index,
                  BX_CMOS_THIS s.periodic_interval_usec, 1);
              }
            }
          }
          if ((prev_CRB >= 0x80) && (value < 0x80) && BX_CMOS_THIS s.timeval_change) {
            update_timeval();
            BX_CMOS_THIS s.timeval_change = 0;
          }
          break;

        case REG_STAT_C: // Control Register C
        case REG_STAT_D: // Control Register D
          BX_ERROR(("write to control register 0x%02x ignored (read-only)",
            BX_CMOS_THIS s.cmos_mem_address));
          break;

        case REG_DIAGNOSTIC_STATUS:
          BX_DEBUG(("write register 0x0e: 0x%02x", value));
          BX_CMOS_THIS s.reg[REG_DIAGNOSTIC_STATUS] = value;
          break;

        case REG_SHUTDOWN_STATUS:
          switch (value) {
            case 0x00: /* proceed with normal POST (soft reset) */
              BX_DEBUG(("Reg 0Fh(00): shutdown action = normal POST"));
              break;
            case 0x01: /* shutdown after memory size check */
              BX_DEBUG(("Reg 0Fh(01): request to change shutdown action"
                        " to shutdown after memory size check"));
              break;
            case 0x02: /* shutdown after successful memory test */
              BX_DEBUG(("Reg 0Fh(02): request to change shutdown action"
                        " to shutdown after successful memory test"));
              break;
            case 0x03: /* shutdown after failed memory test */
              BX_DEBUG(("Reg 0Fh(03): request to change shutdown action"
                        " to shutdown after successful memory test"));
              break;
            case 0x04: /* jump to disk bootstrap routine */
              BX_DEBUG(("Reg 0Fh(04): request to change shutdown action "
                        "to jump to disk bootstrap routine."));
              break;
            case 0x05: /* flush keyboard (issue EOI) and jump via 40h:0067h */
              BX_DEBUG(("Reg 0Fh(05): request to change shutdown action "
                        "to flush keyboard (issue EOI) and jump via 40h:0067h."));
              break;
            case 0x06:
              BX_DEBUG(("Reg 0Fh(06): Shutdown after memory test !"));
              break;
            case 0x07: /* reset (after failed test in virtual mode) */
              BX_DEBUG(("Reg 0Fh(07): request to change shutdown action "
                        "to reset (after failed test in virtual mode)."));
              break;
            case 0x08: /* used by POST during protected-mode RAM test (return to POST) */
              BX_DEBUG(("Reg 0Fh(08): request to change shutdown action "
                        "to return to POST (used by POST during protected-mode RAM test)."));
              break;
            case 0x09: /* return to BIOS extended memory block move
                  (interrupt 15h, func 87h was in progress) */
              BX_DEBUG(("Reg 0Fh(09): request to change shutdown action "
                        "to return to BIOS extended memory block move."));
              break;
            case 0x0a: /* jump to DWORD pointer at 40:67 */
              BX_DEBUG(("Reg 0Fh(0a): request to change shutdown action"
                        " to jump to DWORD at 40:67"));
              break;
            case 0x0b: /* iret to DWORD pointer at 40:67 */
              BX_DEBUG(("Reg 0Fh(0b): request to change shutdown action"
                        " to iret to DWORD at 40:67"));
              break;
            case 0x0c: /* retf to DWORD pointer at 40:67 */
              BX_DEBUG(("Reg 0Fh(0c): request to change shutdown action"
                        " to retf to DWORD at 40:67"));
              break;
            default:
              BX_ERROR(("unsupported shutdown status: 0x%02x!", value));
          }
          BX_CMOS_THIS s.reg[REG_SHUTDOWN_STATUS] = value;
          break;

        default:
          BX_DEBUG(("write reg 0x%02x: value = 0x%02x",
            BX_CMOS_THIS s.cmos_mem_address, value));
          BX_CMOS_THIS s.reg[BX_CMOS_THIS s.cmos_mem_address] = value;
      }
      break;
  }
}

void bx_cmos_c::checksum_cmos(void)
{
  Bit16u sum = 0;
  for (unsigned i=0x10; i<=0x2d; i++)
    sum += BX_CMOS_THIS s.reg[i];
  BX_CMOS_THIS s.reg[REG_CSUM_HIGH] = (sum >> 8) & 0xff; /* checksum high */
  BX_CMOS_THIS s.reg[REG_CSUM_LOW] = (sum & 0xff);      /* checksum low */
}

void bx_cmos_c::periodic_timer_handler(void *this_ptr)
{
  bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;
  class_ptr->periodic_timer();
}

void bx_cmos_c::periodic_timer()
{
  // if periodic interrupts are enabled, trip IRQ 8, and
  // update status register C
  if (BX_CMOS_THIS s.reg[REG_STAT_B] & 0x40) {
    BX_CMOS_THIS s.reg[REG_STAT_C] |= 0xc0; // Interrupt Request, Periodic Int
    DEV_pic_raise_irq(8);
  }
}

void bx_cmos_c::one_second_timer_handler(void *this_ptr)
{
  bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;
  class_ptr->one_second_timer();
}

void bx_cmos_c::one_second_timer()
{
  // divider chain reset - RTC stopped
  if ((BX_CMOS_THIS s.reg[REG_STAT_A] & 0x60) == 0x60)
    return;

  // update internal time/date buffer
  BX_CMOS_THIS s.timeval++;

  // Dont update CMOS user copy of time/date if CRB bit7 is 1
  // Nothing else do to
  if (BX_CMOS_THIS s.reg[REG_STAT_B] & 0x80)
    return;

  BX_CMOS_THIS s.reg[REG_STAT_A] |= 0x80; // set UIP bit

  // UIP timer for updating clock & alarm functions
  bx_pc_system.activate_timer(BX_CMOS_THIS s.uip_timer_index, 244, 0);
}

void bx_cmos_c::uip_timer_handler(void *this_ptr)
{
  bx_cmos_c *class_ptr = (bx_cmos_c *) this_ptr;
  class_ptr->uip_timer();
}

void bx_cmos_c::uip_timer()
{
  update_clock();

  // if update interrupts are enabled, trip IRQ 8, and
  // update status register C
  if (BX_CMOS_THIS s.reg[REG_STAT_B] & 0x10) {
    BX_CMOS_THIS s.reg[REG_STAT_C] |= 0x90; // Interrupt Request, Update Ended
    DEV_pic_raise_irq(8);
  }

  // compare CMOS user copy of time/date to alarm time/date here
  if (BX_CMOS_THIS s.reg[REG_STAT_B] & 0x20) {
    // Alarm interrupts enabled
    bx_bool alarm_match = 1;
    if ((BX_CMOS_THIS s.reg[REG_SEC_ALARM] & 0xc0) != 0xc0) {
      // seconds alarm not in dont care mode
      if (BX_CMOS_THIS s.reg[REG_SEC] != BX_CMOS_THIS s.reg[REG_SEC_ALARM])
        alarm_match = 0;
    }
    if ((BX_CMOS_THIS s.reg[REG_MIN_ALARM] & 0xc0) != 0xc0) {
      // minutes alarm not in dont care mode
      if (BX_CMOS_THIS s.reg[REG_MIN] != BX_CMOS_THIS s.reg[REG_MIN_ALARM])
        alarm_match = 0;
    }
    if ((BX_CMOS_THIS s.reg[REG_HOUR_ALARM] & 0xc0) != 0xc0) {
      // hours alarm not in dont care mode
      if (BX_CMOS_THIS s.reg[REG_HOUR] != BX_CMOS_THIS s.reg[REG_HOUR_ALARM])
        alarm_match = 0;
    }
    if (alarm_match) {
      BX_CMOS_THIS s.reg[REG_STAT_C] |= 0xa0; // Interrupt Request, Alarm Int
      DEV_pic_raise_irq(8);
    }
  }
  BX_CMOS_THIS s.reg[REG_STAT_A] &= 0x7f; // clear UIP bit
}

void bx_cmos_c::update_clock()
{
  struct tm *time_calendar;
  unsigned year, month, day, century;
  Bit8u val_bcd, hour;

  time_calendar = localtime(& BX_CMOS_THIS s.timeval);

  // update seconds
  BX_CMOS_THIS s.reg[REG_SEC] = bin_to_bcd(time_calendar->tm_sec,
    BX_CMOS_THIS s.rtc_mode_binary);

  // update minutes
  BX_CMOS_THIS s.reg[REG_MIN] = bin_to_bcd(time_calendar->tm_min,
    BX_CMOS_THIS s.rtc_mode_binary);

  // update hours
  if (BX_CMOS_THIS s.rtc_mode_12hour) {
    hour = time_calendar->tm_hour;
    val_bcd = (hour > 11) ? 0x80 : 0x00;
    if (hour > 11) hour -= 12;
    if (hour == 0) hour = 12;
    val_bcd |= bin_to_bcd(hour, BX_CMOS_THIS s.rtc_mode_binary);
    BX_CMOS_THIS s.reg[REG_HOUR] = val_bcd;
  } else {
    BX_CMOS_THIS s.reg[REG_HOUR] = bin_to_bcd(time_calendar->tm_hour,
      BX_CMOS_THIS s.rtc_mode_binary);
  }

  // update day of the week
  day = time_calendar->tm_wday + 1; // 0..6 to 1..7
  BX_CMOS_THIS s.reg[REG_WEEK_DAY] = bin_to_bcd(day,
    BX_CMOS_THIS s.rtc_mode_binary);

  // update day of the month
  day = time_calendar->tm_mday;
  BX_CMOS_THIS s.reg[REG_MONTH_DAY] = bin_to_bcd(day,
    BX_CMOS_THIS s.rtc_mode_binary);

  // update month
  month   = time_calendar->tm_mon + 1;
  BX_CMOS_THIS s.reg[REG_MONTH] = bin_to_bcd(month,
    BX_CMOS_THIS s.rtc_mode_binary);

  // update year
  year = time_calendar->tm_year % 100;
  BX_CMOS_THIS s.reg[REG_YEAR] = bin_to_bcd(year,
    BX_CMOS_THIS s.rtc_mode_binary);

  // update century
  century = (time_calendar->tm_year / 100) + 19;
  BX_CMOS_THIS s.reg[REG_IBM_CENTURY_BYTE] = bin_to_bcd(century,
    BX_CMOS_THIS s.rtc_mode_binary);

  // Raul Hudea pointed out that some bioses also use reg 0x37 for the
  // century byte.  Tony Heller says this is critical in getting WinXP to run.
  BX_CMOS_THIS s.reg[REG_IBM_PS2_CENTURY_BYTE] =
    BX_CMOS_THIS s.reg[REG_IBM_CENTURY_BYTE];
}

void bx_cmos_c::update_timeval()
{
  struct tm time_calendar;
  Bit8u val_bin, pm_flag;

  // update seconds
  time_calendar.tm_sec = bcd_to_bin(BX_CMOS_THIS s.reg[REG_SEC],
    BX_CMOS_THIS s.rtc_mode_binary);

  // update minutes
  time_calendar.tm_min = bcd_to_bin(BX_CMOS_THIS s.reg[REG_MIN],
    BX_CMOS_THIS s.rtc_mode_binary);

  // update hours
  if (BX_CMOS_THIS s.rtc_mode_12hour) {
    pm_flag = BX_CMOS_THIS s.reg[REG_HOUR] & 0x80;
    val_bin = bcd_to_bin(BX_CMOS_THIS s.reg[REG_HOUR] & 0x70,
      BX_CMOS_THIS s.rtc_mode_binary);
    if ((val_bin < 12) & (pm_flag > 0)) {
      val_bin += 12;
    } else if ((val_bin == 12) & (pm_flag == 0)) {
      val_bin = 0;
    }
    time_calendar.tm_hour = val_bin;
  } else {
    time_calendar.tm_hour = bcd_to_bin(BX_CMOS_THIS s.reg[REG_HOUR],
      BX_CMOS_THIS s.rtc_mode_binary);
  }

  // update day of the month
  time_calendar.tm_mday = bcd_to_bin(BX_CMOS_THIS s.reg[REG_MONTH_DAY],
    BX_CMOS_THIS s.rtc_mode_binary);

  // update month
  time_calendar.tm_mon = bcd_to_bin(BX_CMOS_THIS s.reg[REG_MONTH],
    BX_CMOS_THIS s.rtc_mode_binary) - 1;

  // update year
  val_bin = bcd_to_bin(BX_CMOS_THIS s.reg[REG_IBM_CENTURY_BYTE],
    BX_CMOS_THIS s.rtc_mode_binary);
  val_bin = (val_bin - 19) * 100;
  val_bin += bcd_to_bin(BX_CMOS_THIS s.reg[REG_YEAR],
    BX_CMOS_THIS s.rtc_mode_binary);
  time_calendar.tm_year = val_bin;

  BX_CMOS_THIS s.timeval = mktime(& time_calendar);
}

#if BX_DEBUGGER
void bx_cmos_c::debug_dump(int argc, char **argv)
{
  int i, j, r;

  dbg_printf("CMOS RTC\n\n");
  dbg_printf("Index register: 0x%02x\n\n", BX_CMOS_THIS s.cmos_mem_address);
  r = 0;
  for (i=0; i<8; i++) {
    dbg_printf("%04x ", r);
    for (j=0; j<16; j++) {
      dbg_printf(" %02x", BX_CMOS_THIS s.reg[r++]);
    }
    dbg_printf("\n");
  }
  if (argc > 0) {
    dbg_printf("\nAdditional options not supported\n");
  }
}
#endif
