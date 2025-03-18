/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2025  The Bochs Project
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

// Program which generates sample CMOS image files

extern "C" {
  #include <assert.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
}

/* Notes:
* [0]: * Some of the registers are used specifically for Bochs. They are not described anywhere else *
* [1]: The RTC values are BCD or Hex, depending on bit 2 of Register B (offset 0x0B)
* [2]: The RTC hour mode (12- or 24-) depend on bit 1 of Regsiter B (offset 0x0B)
* [3]: Alarm will trigger when contents of all three Alarm bytes register match their companin regsiters
* [4]: Register is undocumented/undefined. i.e: no standard / could be different for different firmwares
* [5]: Register has many different descriptions depending on the firmware used
* [6]: Standard CMOS Checksum is performed on registers 0x10 to 0x2D as a byte-wise sum of this range
* [7]: May mirror the values in registers 0x17 and 0x18
* [8]: This is needed for WinXP to function properly (IBM PS/2)
* [9]: Extended CMOS Checksum is performed on registers 0x40 to 0x79 as a byte-wise sum of this range
* [10]: Bochs initially sets this register to 0x26, which is the default, but when read, returns 0x25. DOS/OS must do this.
* 
*/

unsigned char cmos[] = {
        // offset  [notes]  Description (allowed values) [default value]
  0x00,  // 0x00:  [1,2]   RTC Seconds (0 - 59) [0x00]
  0x00,  // 0x01:  [1,2,3] RTC Second Alarm (0 - 59) [0x00]
  0x00,  // 0x02:  [1,2]   RTC Minutes (0 - 59) [0x00]
  0x00,  // 0x03:  [1,2,3] RTC Minute Alarm (0 - 59) [0x00]
  0x00,  // 0x04:  [1,2]   RTC Hours: 24-hour mode (0 - 23) [0x00]
         //                           12-hour mode (1 - 12) (bit 7 = 0: AM, = 1: PM) [0x01]
  0x00,  // 0x05:  [1,2,3] RTC Hour Alarm (same as Hours) [0x00]
  0x01,  // 0x06:  [1,2]   RTC Day of Week (1 - 7) (1 = Sunday) [0x01]
  0x01,  // 0x07:  [1,2]   RTC Date of Month (1 - 31) [0x01]
  0x01,  // 0x08:  [1,2]   RTC Month of Year (1 - 12) [0x01]
  0x00,  // 0x09:  [1,2]   RTC Year (0 - 99) [0x00]
  0x26,  // 0x0A:  [10 ]   RTC Status Register A: [0x26]
         //                 bit    7 - (R/O) 1 = Time update in progress. Reading of registers is undefined.
         //                 bits 6:4 - 22 stage divider
         //                          -  000b = disabled
         //                          -  001b = disabled
         //                          -  010b = 32768 Hz (default)
         //                          -  011b = test
         //                          -  100b = test
         //                          -  101b = test
         //                          -  110b = divider chain reset
         //                          -  111b = divider chain reset
         //                 bits 3:0 - rate selection bits for interrupt
         //                          - 0000b = none
         //                          - 0001 3.90625  ms
         //                          - 0010 7.8125   ms
         //                          - 0011 122.070  us
         //                          - 0100 244.141  us
         //                          - 0101 488.281  us
         //                          - 0110 976.562  us (default 1024 Hz)
         //                          - 0111 1.953125 ms
         //                          - 1000 3.90625  ms
         //                          - 1001 7.8125   ms
         //                          - 1010 15.625   ms
         //                          - 1011 31.25    ms
         //                          - 1100 62.5     ms
         //                          - 1101 125      ms
         //                          - 1110 250      ms
         //                          - 1111 500      ms
  0x02,  // 0x0B:  [   ]   RTC Status Register B: [0x02]
         //                 bit 7 - enable clock setting by freezing updates
         //                 bit 6 - enable periodic interrupt
         //                 bit 5 - enable alarm interrupt
         //                 bit 4 - enable update-ended interrupt
         //                 bit 3 - enable square wave output
         //                 bit 2 - Data Mode - 0: BCD, 1: Binary
         //                 bit 1 - 24/12 hour selection - 1 enables 24 hour mode
         //                 bit 0 - Daylight Savings Enable
         //                         - 1 = enable automatic switching to/from DST in April and October
  0x00,  // 0x0C:  [   ]   RTC Status Register C: (R/O) [0x00]
         //                 bit   7 - Interrupt Enable flag
         //                         - 1 = When any of bits 6:4 are set and associated bits 6:4 in RegB are set, fire irq 8
         //                 bit   6 - periodic interrupt flag
         //                 bit   5 - alarm interrupt flag
         //                 bit   4 - update-ended interrupt flag
         //                 bit 3:0 - unused
  0x80,  // 0x0D:  [   ]    RTC Status Register D: (R/O) [0x80]
         //                 bit   7 - Valid RAM
         //                         - 1 = indicates battery pwoer good
         //                 bit 6:0 - unused
  0x00,  // 0x0E:  [4  ]    Diagnostic Status Byte [0x00]
         //                 bit 7 - 1 = clock has lost power
         //                 bit 6 - 1 = incorrect CRC
         //                 bit 5 - 1 = equipment configuration is incorrect
         //                 bit 4 - 1 = error in memory size
         //                 bit 3 - 1 = controller or disk drive failed initialization
         //                 bit 2 - 1 = time is invalid
         //                 bit 1 - 1 = installed adapters do not match configuration
         //                 bit 0 - 1 = timeout while reading adapter ID
  0x00,  // 0x0F:  [4  ]    Reset Code [0x00]
         //                 00h-03h perform power-on reset
         //                   00h software reset or unexpected reset
         //                   01h reset after memory size check in real/virtual mode
         //                      (or: chip set initialization for real mode reentry)
         //                   02h reset after successful memory test in real/virtual mode
         //                   03h reset after failed memory test in real/virtual mode
         //                  04h INT 19h reboot
         //                  05h flush keyboard (issue EOI) and jump via 40h:0067h
         //                  06h reset (after successful test in virtual mode)
         //                     (or: jump via 40h:0067h without EOI)
         //                  07h reset (after failed test in virtual mode)
         //                  08h used by POST during protected-mode RAM test (return to POST)
         //                  09h used for INT 15/87h (block move) support
         //                  0Ah resume execution by jump via 40h:0067h
         //                  0Bh resume execution via IRET via 40h:0067h
         //                  0Ch resume execution via RETF via 40h:0067h
         //                  0Dh-FFh perform power-on reset
  0x40,  // 0x10:  [   ]   Floppy drive type [0x40]
         //                  bits 7:4 - first floppy disk drive type
         //                  bits 3:0 - second floppy disk drive type
         //                   types: 0x00 = no drive
         //                          0x01 = 360 Kb 5 1/4"
         //                          0x02 = 1.2 Meg 5 1/4"
         //                          0x03 = 720 Kb 3 1/2"
         //                          0x04 = 1.44 Kb 3 1/2"
         //                          0x05 = 2.88 Kb 3 1/2"
  0x00,  // 0x11:  [5  ]    First fixed disk drive type (0 - 255) [0x00]
         //                  (Bochs uses this as configuration bits)
         //                  bit  7 - Numlock On at reboot ?
  0x00,  // 0x12:  [5  ]    Second fixed disk drive type (0 - 255) [0x00]
  0x00,  // 0x13:  [5  ]    Advanced Setup Options [0x00]
         //                  bit  7 - Mouse Enabled (1 = On)
         //                  bit  6 - Test Memory above 1 MB (1 = On)
         //                  bit  5 - Memory Test Tick Sound (1 = On)
         //                  bit  4 - Memory Parity Error Check (1 = On)
         //                  bit  3 - Press <Esc> to Disable Memory Test (1 = On)
         //                  bit  2 - User-Defined Hard Disk (1 = Type 47 data area at address 0:300h)
         //                  bit  1 - Wait for <F1> Message if Error (1 = On)
         //                  bit  0 - Turn Num Lock On at boot (1 = On)
  0x0f,  // 0x14:  [   ]    Equipment Byte [0x0F]
         //                  bits 7:6 - number of floppy drives (system must have at least one)
         //                    00b   1 Drive
         //                    01b   2 Drives
         //                    10b ??? 3 Drives
         //                    11b ??? 4 Drives
         //                  bits 5:4 - monitor type
         //                    00b Not CGA or MDA (observed for EGA & VGA)
         //                    01b 40x25 CGA
         //                    10b 80x25 CGA
         //                    11b MDA (Monochrome)
         //                  bit  3 display enabled (turned off to enable boot of rackmount)
         //                  bit  2 keyboard enabled (turn off to enable boot of rackmount)
         //                  bit  1 math coprocessor installed
         //                  bit  0 floppy drive installed (turned off for rackmount boot)
  0x80,  // 0x15:  [   ]    Base Memory in KB (low byte) [0x80]  (0x0280 = 640k)
  0x02,  // 0x16:  [   ]    Base Memory in KB (high byte) [0x02]
  0x00,  // 0x17:  [   ]    Extended Memory in KB (low byte) [0x00]
  0x00,  // 0x18:  [   ]    Extended Memory in KB (high byte) [0x00]
  0x2f,  // 0x19:  [   ]    First Extended Hard Disk Drive Type [0x2F]
  0x00,  // 0x1A:  [   ]    Second Extended Hard Disk Drive Type [0x00]
  0x00,  // 0x1B:  [   ]    First Extended Hard Disk Drive: Cylinders (low byte) [0x00]
  0x00,  // 0x1C:  [   ]    First Extended Hard Disk Drive: Cylinders (high byte) [0x00]
  0x00,  // 0x1D:  [   ]    First Extended Hard Disk Drive: Heads [0x00]
  0x00,  // 0x1E:  [   ]    First Extended Hard Disk Drive: Write pre-comp (low byte) [0x00]
  0x00,  // 0x1F:  [   ]    First Extended Hard Disk Drive: Write pre-comp (high byte) [0x00]
  0xc8,  // 0x20:  [   ]    First Extended Hard Disk Drive: Control Byte [0xC8]
         //                  bits 7:6 - number of retries
         //                  bit    5 - bad sector map at last cylinder+1
         //                  bit    4 - unused
         //                  bit    3 - more than 8 heads
         //                  bit  2:0 - unused
  0x00,  // 0x21:  [   ]    First Extended Hard Disk Drive: Landing Zone (low byte) [0x00]
  0x00,  // 0x22:  [   ]    First Extended Hard Disk Drive: Landing Zone (high byte) [0x00]
  0x00,  // 0x23:  [   ]    First Extended Hard Disk Drive: Sectors per Track [0x00]
  0x00,  // 0x24:  [   ]    Second Extended Hard Disk Drive: Cylinders (low byte) [0x00]
  0x00,  // 0x25:  [   ]    Second Extended Hard Disk Drive: Cylinders (high byte) [0x00]
  0x00,  // 0x26:  [   ]    Second Extended Hard Disk Drive: Heads [0x00]
  0x00,  // 0x27:  [   ]    Second Extended Hard Disk Drive: Write pre-comp (low byte) [0x00]
  0x00,  // 0x28:  [   ]    Second Extended Hard Disk Drive: Write pre-comp (high byte) [0x00]
  0xc8,  // 0x29:  [   ]    Second Extended Hard Disk Drive: Control Byte [0xC8]
         //                  see regigister 0x20
  0x00,  // 0x2A:  [   ]    Second Extended Hard Disk Drive: Landing Zone (low byte) [0x00]
  0x00,  // 0x2B:  [   ]    Second Extended Hard Disk Drive: Landing Zone (high byte) [0x00]
  0x00,  // 0x2C:  [   ]    Second Extended Hard Disk Drive: Sectors per Track [0x00]
  0x13,  // 0x2D:  [   ]    Configuration Options [0x13]
         //                  bit  7 Weitek Installed
         //                  bit  6 Floppy Drive Seek - turn off for fast boot
         //                  bit  5 Boot Order
         //                         0 - Drive C:, then A:
         //                         1 - Drive A:, then C:
         //                  bit  4 Boot Speed (0 - Low; 1 - High)
         //                  bit  3 External Cache Enable (1 = On)
         //                  bit  2 Internal Cache Enable (1 = On)
         //                  bit  1 Use Fast Gate A20 after boot (1 = On)
         //                  bit  0 Turbo Switch (1 = On)
  0x00,  // 0x2E:  [6  ]    Standard CMOS Checksum (high byte)
  0x00,  // 0x2F:  [6  ]    Standard CMOS Checksum (low byte)
  0x00,  // 0x30:  [7  ]    Extended Memory in K (low byte)
  0x00,  // 0x31:  [7  ]    Extended Memory in K (high byte)
  0x14,  // 0x32:  [   ]    RTC Century value (0x14 = 20xx) [0x14]
  0x00,  // 0x33:  [   ]    * unknown * [0x00]
  0x00,  // 0x34:  [   ]    Extended Memory in 64k blocks (low byte) [0x00]
  0x00,  // 0x35:  [   ]    Extended Memory in 64k blocks (high byte) [0x00]
  0x00,  // 0x36:  [   ]    * unknown * [0x00]
  0x14,  // 0x37:  [8  ]    RTC Century value (0x14 = 20xx) [0x14]
  0x21,  // 0x38:  [0  ]    Eltorito Boot Sequence + Boot Signature Check [0x21]
         //                  bits 7:4 - Boot drive #3 (default to hard drive)
         //                    0000b unused
         //                    0001b first floppy
         //                    0010b first hard drive
         //                    0011b first cdrom
         //                    0100b first pcmcia
         //                    0101b first usb
         //                    0110b embedded network
         //                    1xxxb other: try the floppy?
         //                  bits 3:0 floppy boot signature check 
         //                    xxx0b nothing
         //                    0001b do floppy boot sig check
  0x00,  // 0x39:  [0  ]    ATA Translation Policy (ata0 - ata1) [0x00]
         //                   bits 7:6 - ata1-slave
         //                     00b none
         //                     01b LBA
         //                     10b large
         //                     11b R-ECHS
         //                   bits 5:4 - ata1-master
         //                     (same as above)
         //                   bits 3:2 - ata0-slave
         //                     (same as above)
         //                   bits 1:0 - ata0-master
         //                     (same as above)
  0x00,  // 0x3A:  [0  ]    ATA Translation Policy (ata2 - ata3) [0x00]
         //                   bits 7:6 - ata3-slave
         //                     (same as above)
         //                   bits 5:4 - ata3-master
         //                     (same as above)
         //                   bits 3:2 - ata2-slave
         //                     (same as above)
         //                   bits 1:0 - ata2-master
         //                     (same as above)
  0x00,  // 0x3B:  [0  ]    ATA BIOS Detect flags [0x00]
         //                   bits 7:6 - ata1-slave
         //                     00b auto
         //                     01b cmos
         //                     10b none
         //                     11b unused
         //                   bits 5:4 - ata1-master
         //                     (same as above)
         //                   bits 3:2 - ata0-slave
         //                     (same as above)
         //                   bits 1:0 - ata0-master
         //                     (same as above)
  0x00,  // 0x3C:  [0  ]    ATA BIOS Detect flags [0x00]
         //                   bits 7:6 - ata3-slave
         //                     (same as above)
         //                   bits 5:4 - ata3-master
         //                     (same as above)
         //                   bits 3:2 - ata2-slave
         //                     (same as above)
         //                   bits 1:0 - ata2-master
         //                     (same as above)
  0x31,  // 0x3D:  [0  ]    Eltorito Boot Sequence [0x31]
         //                  bits 7:4 - Boot drive #1
         //                     (see register 0x38) (default to floppy)
         //                  bits 3:0 - Boot drive #2
         //                     (see register 0x38) (default to cdrom)
  0x00,  // 0x3E:  [   ]    * unknown * [0x00]
  0x00,  // 0x3F:  [0  ]    BIOS Options [0x00]
         //                  bits 7:1 - reserved
         //                  bit    0 - fast boot (skip boot menu delay)

  // Most IBM PS/2s now have 128 bytes of CMOS
  0x00,  // 0x40:  [   ]    * unknown * [0x00]
  0x00,  // 0x41:  [   ]    * unknown * [0x00]
  0x00,  // 0x42:  [   ]    * unknown * [0x00]
  0x00,  // 0x43:  [   ]    * unknown * [0x00]
  0x00,  // 0x44:  [   ]    * unknown * [0x00]
  0x00,  // 0x45:  [   ]    * unknown * [0x00]
  0x00,  // 0x46:  [   ]    * unknown * [0x00]
  0x00,  // 0x47:  [   ]    * unknown * [0x00]
  0x00,  // 0x48:  [   ]    * unknown * [0x00]
  0x00,  // 0x49:  [   ]    * unknown * [0x00]
  0x00,  // 0x4A:  [   ]    * unknown * [0x00]
  0x00,  // 0x4B:  [   ]    * unknown * [0x00]
  0x00,  // 0x4C:  [   ]    * unknown * [0x00]
  0x00,  // 0x4D:  [   ]    * unknown * [0x00]
  0x00,  // 0x4E:  [   ]    * unknown * [0x00]
  0x00,  // 0x4F:  [   ]    * unknown * [0x00]
  0x00,  // 0x50:  [   ]    * unknown * [0x00]
  0x00,  // 0x51:  [   ]    * unknown * [0x00]
  0x00,  // 0x52:  [   ]    * unknown * [0x00]
  0x00,  // 0x53:  [   ]    * unknown * [0x00]
  0x00,  // 0x54:  [   ]    * unknown * [0x00]
  0x00,  // 0x55:  [   ]    * unknown * [0x00]
  0x00,  // 0x56:  [   ]    * unknown * [0x00]
  0x00,  // 0x57:  [   ]    * unknown * [0x00]
  0x00,  // 0x58:  [   ]    * unknown * [0x00]
  0x00,  // 0x59:  [   ]    * unknown * [0x00]
  0x00,  // 0x5A:  [   ]    * unknown * [0x00]
  0x00,  // 0x5B:  [0  ]    Extra Memory above 4Gig in 64k (low byte) [0x00]
  0x00,  // 0x5C:  [0  ]    Extra Memory above 4Gig in 64k (mid byte) [0x00]
  0x00,  // 0x5D:  [0  ]    Extra Memory above 4Gig in 64k (high byte) [0x00]
  0x00,  // 0x5E:  [   ]    * unknown * [0x00]
  0x01,  // 0x5F:  [0  ]    Number of processors [0x01]
  0x00,  // 0x60:  [   ]    * unknown * [0x00]
  0x00,  // 0x61:  [   ]    * unknown * [0x00]
  0x00,  // 0x62:  [   ]    * unknown * [0x00]
  0x00,  // 0x63:  [   ]    * unknown * [0x00]
  0x00,  // 0x64:  [   ]    * unknown * [0x00]
  0x00,  // 0x65:  [   ]    * unknown * [0x00]
  0x00,  // 0x66:  [   ]    * unknown * [0x00]
  0x00,  // 0x67:  [   ]    * unknown * [0x00]
  0x00,  // 0x68:  [   ]    * unknown * [0x00]
  0x00,  // 0x69:  [   ]    * unknown * [0x00]
  0x00,  // 0x6A:  [   ]    * unknown * [0x00]
  0x00,  // 0x6B:  [   ]    * unknown * [0x00]
  0x00,  // 0x6C:  [   ]    * unknown * [0x00]
  0x00,  // 0x6D:  [   ]    * unknown * [0x00]
  0x00,  // 0x6E:  [   ]    * unknown * [0x00]
  0x00,  // 0x6F:  [   ]    * unknown * [0x00]
  0x00,  // 0x70:  [   ]    * unknown * [0x00]
  0x00,  // 0x71:  [   ]    * unknown * [0x00]
  0x00,  // 0x72:  [   ]    * unknown * [0x00]
  0x00,  // 0x73:  [   ]    * unknown * [0x00]
  0x00,  // 0x74:  [   ]    * unknown * [0x00]
  0x00,  // 0x75:  [   ]    * unknown * [0x00]
  0x00,  // 0x76:  [   ]    * unknown * [0x00]
  0x00,  // 0x77:  [   ]    * unknown * [0x00]
  0x00,  // 0x78:  [   ]    * unknown * [0x00]
  0x00,  // 0x79:  [   ]    * unknown * [0x00]
  0x00,  // 0x7A:  [9  ]    Extended CMOS Checksum (high byte) [0x??]
  0x00,  // 0x7B:  [9  ]    Extended CMOS Checksum (low byte) [0x??]
  0x00,  // 0x7C:  [   ]    * unknown * [0x00]
  0x00,  // 0x7D:  [   ]    * unknown * [0x00]
  0x00,  // 0x7E:  [   ]    * unknown * [0x00]
  0x00,  // 0x7F:  [   ]    * unknown * [0x00]
};

// make sure the cmos struct is 128 bytes in length
static_assert ((sizeof(cmos) == 128), "cmos struct must be 128 bytes in length");

// global members
char extended = 0;     // 0 = 64-byte cmos (default), 1 = 128-byte cmos
char use_bcd = 0;      // 0 = use binary (default), 1 = use BCD
char use_24hour = 0;   // 0 = use 12-hour mode (default), 1 = use 24 hour mode
char filename[256] = { 0, };

// convert a value to either BCD or binary
unsigned char create_value(int value) {
  if (use_bcd)
    return (unsigned char) (((value / 10) << 4) | (value % 10));
  else
    return (unsigned char) value;
}

void parse_command(int argc, char *argv[]) {
  int i;

  // did the user supply at least a filename?
  if (argc < 2) {
    fprintf(stderr, "usage: %s pathname [/x][/bcd][/24]\n"
                    "  items in []'s are optional\n"
                    "  parameters can start with / or -\n"
                    "   /x   = create 128-byte cmos file, else 64-byte (default)\n"
                    "   /bcd = use BCD values, else use binary values (default)\n"
                    "   /24  = use 24-hour mode, else use 12-hour mode (default)\n", argv[0]);
    exit(1);
  }

  for (i=1; i<argc; i++) {
    if ((argv[i][0] == '/') || (argv[i][0] == '-')) {
      // is it the /x parameter?
      if (stricmp(&argv[i][1], "x") == 0) {
        extended = 1;
      }
      else if (stricmp(&argv[i][1], "bcd") == 0) {
        use_bcd = 1;
      }
      else if (stricmp(&argv[i][1], "24") == 0) {
        use_24hour = 1;
      }

      // else it is something else
      else {
        fprintf(stderr, "unknown parameter given: '%s'\n", argv[i]);
        exit(2);
      }
    } else {
      // else, must be the path/filename
      strcpy(filename, argv[i]);
    }
  }

  if (strlen(filename) == 0) {
    fprintf(stderr, "must supply a filename...\n");
    exit(3);
  }
}

int main(int argc, char *argv[]) {
  FILE *fp;
  unsigned int crc;
  int i;
  time_t timer;
  struct tm *today;

  // get the current time
  time(&timer);
  today = localtime(&timer);

  // parse the command line
  parse_command(argc, argv);

  // get the length of the cmos file to create
  const size_t len = (extended) ? sizeof(cmos) : 64;

  // use BCD or Binary
  if (use_bcd)
    cmos[0x0B] &= ~0x04; // BCD
  else
    cmos[0x0B] |= 0x04;  // binary
  
  // use 24-hour mode?
  if (use_24hour)
    cmos[0x0B] |= 0x02;
  else
    cmos[0x0B] &= ~0x02;

  // set the date and time registers
  cmos[0x00] = create_value(today->tm_sec);
  cmos[0x02] = create_value(today->tm_min);
  if (use_24hour) {
    cmos[0x04] = create_value(today->tm_hour);
  } else {
    if (today->tm_hour < 12) {
      cmos[0x04] = create_value(today->tm_hour);
    } else {
      cmos[0x04] = create_value(0x80 | (today->tm_hour - 12));
    }
  }
  cmos[0x06] = create_value(today->tm_wday + 1);
  cmos[0x07] = create_value(today->tm_mday);
  cmos[0x08] = create_value(today->tm_mon + 1);
  cmos[0x09] = create_value(today->tm_year - 2000); // assumes we are in the 21st century
  printf("Setting the date and time to: %s", asctime(today));

  // calculate the standard checksum (0x2E->0x2F)
  // it is calculated as the byte sum of registers 0x10 to 0x2D
  crc = 0;
  for (i = 0x10; i <= 0x2D; i++)
    crc += cmos[i];
  cmos[0x2E] = (unsigned char) ((crc >> 8) & 0xFF);
  cmos[0x2F] = (unsigned char) ((crc >> 0) & 0xFF);
   
  if (extended) {
    // calculate the extended checksum (0x7A->0x7B)
    // it is calculated as the byte sum of registers 0x40 to 0x79
    crc = 0;
    for (i = 0x40; i <= 0x79; i++)
      crc += cmos[i];
    cmos[0x7A] = (unsigned char) ((crc >> 8) & 0xFF);
    cmos[0x7B] = (unsigned char) ((crc >> 0) & 0xFF);
  }

  if ((fp = fopen(filename, "wb")) == NULL) {  
    puts("trying to open cmos image file for write, failed.\n");
    exit(1);
  }

  size_t ret = fwrite(cmos, 1, len, fp);
  if (ret != len) {
    puts("write() did not write all CMOS data.\n");
    fclose(fp);
    exit(1);
  }

  fclose(fp);
  printf("CMOS data successfully written to file '%s' (%zi bytes).\n", filename, len);
  return 0;
}
