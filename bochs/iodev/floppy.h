/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2012  The Bochs Project
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

#define FROM_FLOPPY 10
#define TO_FLOPPY   11

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
  bx_bool  vvfat_floppy;
  device_image_t *vvfat;
} floppy_t;

class bx_floppy_ctrl_c : public bx_floppy_stub_c {
public:
  bx_floppy_ctrl_c();
  virtual ~bx_floppy_ctrl_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual unsigned set_media_status(unsigned drive, bx_bool status);
  virtual void register_state(void);
  virtual void after_restore_state(void);
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

private:

  struct {
    Bit8u   data_rate;

    Bit8u   command[10]; /* largest command size ??? */
    Bit8u   command_index;
    Bit8u   command_size;
    bx_bool command_complete;
    Bit8u   pending_command;

    bx_bool multi_track;
    bx_bool pending_irq;
    Bit8u   reset_sensei;
    Bit8u   format_count;
    Bit8u   format_fillbyte;

    Bit8u   result[10];
    Bit8u   result_index;
    Bit8u   result_size;

    Bit8u   DOR; // Digital Ouput Register
    Bit8u   TDR; // Tape Drive Register
    Bit8u   cylinder[4]; // really only using 2 drives
    Bit8u   head[4];     // really only using 2 drives
    Bit8u   sector[4];   // really only using 2 drives
    Bit8u   eot[4];      // really only using 2 drives
    bx_bool TC;          // Terminal Count status from DMA controller

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

    Bit8u   status_reg0;
    Bit8u   status_reg1;
    Bit8u   status_reg2;
    Bit8u   status_reg3;

    // drive field allows up to 4 drives, even though probably only 2 will
    // ever be used.
    floppy_t media[4];
    unsigned num_supported_floppies;
    Bit8u    floppy_buffer[512+2]; // 2 extra for good measure
    unsigned floppy_buffer_index;
    int      floppy_timer_index;
    bx_bool  media_present[4];
    Bit8u    device_type[4];
    Bit8u    DIR[4]; // Digital Input Register:
                  // b7: 0=diskette is present and has not been changed
                  //     1=diskette missing or changed
    bx_bool  lock;      // FDC lock status
    Bit8u    SRT;       // step rate time
    Bit8u    HUT;       // head unload time
    Bit8u    HLT;       // head load time
    Bit8u    config;    // configure byte #1
    Bit8u    pretrk;    // precompensation track
    Bit8u    perp_mode; // perpendicular mode

    int      statusbar_id[2]; // IDs of the status LEDs
  } s;  // state information

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_FD_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_FD_SMF Bit16u dma_write(Bit8u *buffer, Bit16u maxlen);
  BX_FD_SMF Bit16u dma_read(Bit8u *buffer, Bit16u maxlen);
  BX_FD_SMF void   floppy_command(void);
  BX_FD_SMF void   floppy_xfer(Bit8u drive, Bit32u offset, Bit8u *buffer, Bit32u bytes, Bit8u direction);
  BX_FD_SMF void   raise_interrupt(void);
  BX_FD_SMF void   lower_interrupt(void);
  BX_FD_SMF void   enter_idle_phase(void);
  BX_FD_SMF void   enter_result_phase(void);
  BX_FD_SMF Bit32u calculate_step_delay(Bit8u drive, Bit8u new_cylinder);
  BX_FD_SMF void   reset_changeline(void);
  BX_FD_SMF bx_bool get_tc(void);
  static void      timer_handler(void *);
  BX_FD_SMF void   timer(void);
  BX_FD_SMF void   increment_sector(void);
  BX_FD_SMF bx_bool evaluate_media(Bit8u devtype, Bit8u type, char *path, floppy_t *media);
  BX_FD_SMF void    close_media(floppy_t *media);
  // runtime options
  static Bit64s    floppy_param_handler(bx_param_c *param, int set, Bit64s val);
  static const char* floppy_param_string_handler(bx_param_string_c *param, int set, const char *oldval, const char *val, int maxlen);
  static void runtime_config_handler(void *);
  void runtime_config(void);
};
#endif
