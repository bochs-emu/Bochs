/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2023  The Bochs Project
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

#ifndef BX_IODEV_FLOPPY_H
#define BX_IODEV_FLOPPY_H

/* for main status register */
#define FD_MS_RQM  0x80
#define FD_MS_DIO  0x40
#define FD_MS_NDMA 0x20
#define FD_MS_BUSY 0x10
#define FD_MS_ACTD 0x08
#define FD_MS_ACTC 0x04
#define FD_MS_ACTB 0x02
#define FD_MS_ACTA 0x01

enum {
  FROM_FLOPPY = 10,
  TO_FLOPPY   = 11
};

#define FLOPPY_DMA_CHAN 2

#define FDRIVE_NONE  0x00
#define FDRIVE_525DD 0x01
#define FDRIVE_525HD 0x02
#define FDRIVE_350DD 0x04
#define FDRIVE_350HD 0x08
#define FDRIVE_350ED 0x10

// Command Bits
#define FD_CMD_MT    0x80
#define FD_CMD_MFM   0x40
#define FD_CMD_SK    0x20

#define FD_CMD_LOCK  0x80
#define FD_CMD_DIR   0x40

// Commands
enum {
  FD_CMD_NOP                 = 0x00,   // (Contrived for this implementation)
  FD_CMD_MODE                = 0x01,   //                        DP8473  PC87306
  FD_CMD_READ_TRACK          = 0x02,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_SPECIFY             = 0x03,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_SENSE_DRV_STATUS    = 0x04,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_WRITE_NORMAL_DATA   = 0x05,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_READ_NORMAL_DATA    = 0x06,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_RECALIBRATE         = 0x07,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_SENSE_INT_STATUS    = 0x08,   // 82077AA  82078  37c78  DP8473  PC87306
//FD_CMD_WRITE_DELETED_DATA  = 0x09,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_READ_ID             = 0x0A,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_MOTOR_ON_OFF        = 0x0B,   // 
  FD_CMD_READ_DELETED_DATA   = 0x0C,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_FORMAT_TRACK        = 0x0D,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_DUMPREG             = 0x0E,   // 82077AA  82078  37c78          PC87306
  FD_CMD_SEEK                = 0x0F,   // 82077AA  82078  37c78  DP8473  PC87306
  FD_CMD_VERSION             = 0x10,   // 82077AA  82078  37c78          PC87306
  FD_CMD_SCAN_EQUAL          = 0x11,   // 82077AA  82078         DP8473  PC87306
  FD_CMD_PERPENDICULARE_MODE = 0x12,   // 82077AA  82078  37c78          PC87306
  FD_CMD_CONFIGURE           = 0x13,   // 82077AA  82078  37c78          PC87306
  FD_CMD_LOCK_UNLOCK         = 0x14,   // 82077AA  82078  37c78          PC87306
  FD_CMD_VERIFY              = 0x16,   // 82077AA  82078  37c78          PC87306
  FD_CMD_POWER_DOWN_MODE     = 0x17,   //          82078
  FD_CMD_PART_ID             = 0x18,   //          82078                 PC87306
  FD_CMD_SCAN_LOW_EQUAL      = 0x19,   // 82077AA  82078         DP8473  PC87306
  FD_CMD_SCAN_HIGH_EQUAL     = 0x1D,   // 82077AA  82078         DP8473  PC87306
  FD_CMD_SET_TRACK           = 0x21,   //                        DP8473  PC87306
  FD_CMD_SAVE                = 0x2E,   //          82078
  FD_CMD_OPTION              = 0x33,   //          82078
  FD_CMD_EXIT_STANDBY_MODE   = 0x34,   //
  FD_CMD_ENTER_STANDBY_MODE  = 0x35,   //
  FD_CMD_HARD_RESET          = 0x36,   //
  FD_CMD_RESTORE             = 0x4E,   //          82078
  FD_CMD_DRIVE_SPECIFICATION = 0x8E,   //          82078
  FD_CMD_RELATIVE_SEEK       = 0x8F,   // 82077AA  82078  37c78          PC87306
  FD_CMD_FORMAT_AND_WRITE    = 0xAD,   //          82078
  FD_RESET                   = 0xFE,   // (Contrived for this implementation)
};

// 82078 power down flags
#define PD_FLAGS_EREG_EN  0x20  // allow access to the TDR? (82078 only)
#define PD_FLAGS_44PD_EN  0x10  // use pin 44?
#define PD_FLAGS_FDI_TRI  0x04  // Floppy Drive Interface Tristate
#define PD_FLAGS_MIN_DLY  0x02  // Min power up control (10ms or 1/2ms)
#define PD_FLAGS_AUTO_PD  0x01  // Auto Power Down enabled

// DP8473 mode flags
#define FLAGS_ETR      0x01  // Extended Track Range

// max count of bytes in the command/result phazes
// (RESTORE has 17 bytes in command, SAVE has 16 bytes in restore)
#define MAX_PHASE_SIZE  20

// Win98 and older seem to erroroneously set bit 1 of the DS0/DS1 bits (drive bits)
//  specifying drive 2 for drive 0, and drive 3 for drive 1.
// So to make them work, we have to mask off that bit. However, this will mean
//  that we can only have 2 drives (which is the case at the moment)
#define FDC_DRV_MASK 0x01  // set to 0x03 if you have 4 drives.

#if BX_USE_FD_SMF
#  define BX_FD_SMF  static
#  define BX_FD_THIS theFloppyController->
#else
#  define BX_FD_SMF
#  define BX_FD_THIS this->
#endif

typedef struct {
  int      fd;         /* file descriptor of floppy image file */
  unsigned sectors_per_track;    /* number of sectors/track */
  unsigned sectors;    /* number of formatted sectors on diskette */
  unsigned tracks;      /* number of tracks */
  unsigned heads;      /* number of heads */
  unsigned type;
  unsigned write_protected;
  unsigned status_changed;
  bool     vvfat_floppy;
  device_image_t *vvfat;
} floppy_t;

class bx_floppy_ctrl_c : public bx_devmodel_c {
public:
  bx_floppy_ctrl_c();
  virtual ~bx_floppy_ctrl_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

private:

  struct {
    Bit8u   command[MAX_PHASE_SIZE];
    Bit8u   command_index;
    Bit8u   command_size;
    bool    command_complete;
    Bit8u   pending_command;

    bool    multi_track;
    bool    pending_irq;
    Bit8u   reset_sensei;
    Bit8u   sector_count;
    Bit8u   format_fillbyte;
    Bit16u  format_cylinder;
    Bit8u   format_sector_bp[36];
    bool    format_write_flag;

    Bit8u   result[MAX_PHASE_SIZE];
    Bit8u   result_index;
    Bit8u   result_size;
    Bit8u   last_result;

    Bit8u   DSR; // Datarate Select Register (DSR)
    Bit8u   DOR; // Digital Ouput Register
    Bit8u   TDR; // Tape Drive Register
    Bit16u  cylinder[4]; // really only using 2 drives
    Bit8u   head[4];     // really only using 2 drives
    Bit8u   sector[4];   // really only using 2 drives
    Bit8u   eot[4];      // really only using 2 drives
    Bit8u   stp;         // used with the scan commands
    bool    TC;          // Terminal Count status from DMA controller

    /* MAIN STATUS REGISTER
     * b7: MRQ: main request 1=data register ready     0=data register not ready
     * b6: DIO: data input/output:
     *     1=controller->CPU (ready for data read)
     *     0=CPU->controller (ready for data write)
     * b5: NDMA: non-DMA mode: 1=controller not in DMA modes
     *                         0=controller in DMA mode
     * b4: BUSY: instruction(device busy) 1=active 0=not active
     * b3-0: ACTD, ACTC, ACTB, ACTA:
     *       drive D,C,B,A in positioning mode 1=active 0=not active
     */
    Bit8u   main_status_reg;
    Bit8u   status_reg_a;
    Bit8u   status_reg_b;

    Bit8u   status_reg0;
    Bit8u   status_reg1;
    Bit8u   status_reg2;
    Bit8u   status_reg3;

    // drive field allows up to 4 drives, even though probably only 2 will
    // ever be used.
    floppy_t media[4];
    unsigned num_supported_floppies;
    Bit8u    floppy_buffer[512];
    Bit8u    scan_buffer[512];
    unsigned floppy_buffer_index;
    int      floppy_timer_index;
    bool     media_present[4];
    Bit8u    device_type[4];
    Bit8u    DIR[4]; // Digital Input Register:
                     // b7: 0=diskette is present and has not been changed
                     //     1=diskette missing or changed
    bool     standby;   // in standby mode?
    bool     lock;      // FDC lock status
    Bit8u    SRT;       // step rate time
    Bit8u    HUT;       // head unload time
    Bit8u    HLT;       // head load time
    Bit8u    config;    // configure byte #1
    Bit8u    pretrk;    // precompensation track
    Bit8u    perp_mode; // perpendicular mode
    Bit8u    mode0;     // FDC_TYPE_DP8473 mode0 register
    Bit8u    mode1;     // FDC_TYPE_DP8473 mode1 register
    Bit8u    mode2;     // FDC_TYPE_DP8473 mode2 register
    Bit8u    option;    // FDC_TYPE_82078 option register
    Bit8u    power_down; // power down flags (82078)

    int      statusbar_id[2]; // IDs of the status LEDs
    int      rt_conf_id;      // ID of the runtime config handler
  } s;  // state information

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_FD_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_FD_SMF bool   set_media_status(unsigned drive, bool    status);
  BX_FD_SMF Bit16u dma_write(Bit8u *buffer, Bit16u maxlen);
  BX_FD_SMF Bit16u dma_read(Bit8u *buffer, Bit16u maxlen);
  BX_FD_SMF void   floppy_command(void);
  BX_FD_SMF void   do_floppy_xfer(Bit8u *buffer, Bit8u drive, Bit8u fromto);
  BX_FD_SMF void   floppy_xfer(Bit8u drive, Bit32u offset, Bit8u *buffer, Bit32u bytes, Bit8u direction);
  BX_FD_SMF void   raise_interrupt(void);
  BX_FD_SMF void   lower_interrupt(void);
  BX_FD_SMF void   enter_idle_phase(void);
  BX_FD_SMF void   enter_result_phase(void);
  BX_FD_SMF Bit32u calculate_step_delay(Bit8u drive, Bit8u new_cylinder);
  BX_FD_SMF void   reset_changeline(void);
  BX_FD_SMF bool   get_tc(void);
  static void      timer_handler(void *);
  BX_FD_SMF void   timer(void);
  BX_FD_SMF bool   end_of_track(void);
  BX_FD_SMF void   increment_sector(void);
  BX_FD_SMF bool   evaluate_media(Bit8u devtype, Bit8u type, const char *path, floppy_t *media);
  BX_FD_SMF void   close_media(floppy_t *media);
  BX_FD_SMF bool   command_supported(Bit8u command);
  // runtime options
  static Bit64s    floppy_param_handler(bx_param_c *param, bool set, Bit64s val);
  static const char *floppy_param_string_handler(bx_param_string_c *param,
                       bool set, const char *oldval, const char *val, int maxlen);
  static void runtime_config_handler(void *);
  void runtime_config(void);
};
#endif
