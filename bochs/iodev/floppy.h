/////////////////////////////////////////////////////////////////////////
// $Id: floppy.h,v 1.9 2002-02-06 18:51:48 vruppert Exp $
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



#define FROM_FLOPPY 10
#define TO_FLOPPY   11

#if BX_USE_FD_SMF
#  define BX_FD_SMF  static
#  define BX_FD_THIS bx_floppy.
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
  } floppy_t;

class bx_floppy_ctrl_c : public logfunctions {
public:

  bx_floppy_ctrl_c(void);
  ~bx_floppy_ctrl_c(void);
  BX_FD_SMF void   init(bx_devices_c *d, bx_cmos_c *cmos);
  BX_FD_SMF void   reset(unsigned source);
  BX_FD_SMF void   dma_write(Bit8u *data_byte);
  BX_FD_SMF void   dma_read(Bit8u *data_byte);
  BX_FD_SMF unsigned set_media_status(unsigned drive, unsigned status);
  BX_FD_SMF unsigned get_media_status(unsigned drive);

private:

  struct {
    Bit8u   data_rate;

    Bit8u   command[10]; /* largest command size ??? */
    Bit8u   command_index;
    Bit8u   command_size;
    Boolean command_complete;
    Bit8u   pending_command;

    Boolean multi_track;
    Boolean pending_irq;
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
    Boolean  media_present[2];
    Bit8u    DIR; // Digital Input Register:
                  // b7: 0=diskette is present and has not been changed
                  //     1=diskette missing or changed
    } s;  // state information

  bx_devices_c *devices;

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_FD_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_FD_SMF void   floppy_command(void);
  BX_FD_SMF void   floppy_xfer(Bit8u drive, Bit32u offset, Bit8u *buffer, Bit32u bytes, Bit8u direction);
  BX_FD_SMF void   raise_interrupt(void);
  static void   timer_handler(void *);

public:
  BX_FD_SMF void   timer(void);
  BX_FD_SMF void   increment_sector(void);
  BX_FD_SMF Boolean evaluate_media(unsigned type, char *path, floppy_t *floppy);
  };

extern bx_floppy_ctrl_c bx_floppy;
