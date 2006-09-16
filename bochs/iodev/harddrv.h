/////////////////////////////////////////////////////////////////////////
// $Id: harddrv.h,v 1.45 2006-09-16 14:47:40 vruppert Exp $
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

#ifndef BX_IODEV_HDDRIVE_H
#define BX_IODEV_HDDRIVE_H

#define MAX_MULTIPLE_SECTORS 16

typedef enum _sense {
      SENSE_NONE = 0, SENSE_NOT_READY = 2, SENSE_ILLEGAL_REQUEST = 5,
      SENSE_UNIT_ATTENTION = 6
} sense_t;

typedef enum _asc {
      ASC_ILLEGAL_OPCODE = 0x20,
      ASC_LOGICAL_BLOCK_OOR = 0x21,
      ASC_INV_FIELD_IN_CMD_PACKET = 0x24,
      ASC_SAVING_PARAMETERS_NOT_SUPPORTED = 0x39,
      ASC_MEDIUM_NOT_PRESENT = 0x3a
} asc_t;

class device_image_t;
class LOWLEVEL_CDROM;

typedef struct {
  struct {
    bx_bool busy;
    bx_bool drive_ready;
    bx_bool write_fault;
    bx_bool seek_complete;
    bx_bool drq;
    bx_bool corrected_data;
    bx_bool index_pulse;
    unsigned index_pulse_count;
    bx_bool err;
    } status;
  Bit8u    error_register;
  Bit8u    head_no;
  union {
    Bit8u    sector_count;
    struct {
#ifdef BX_LITTLE_ENDIAN
      unsigned c_d : 1;
      unsigned i_o : 1;
      unsigned rel : 1;
      unsigned tag : 5;
#else  /* BX_BIG_ENDIAN */
      unsigned tag : 5;
      unsigned rel : 1;
      unsigned i_o : 1;
      unsigned c_d : 1;
#endif
    } interrupt_reason;
  };
  Bit8u    sector_no;
  union {
    Bit16u   cylinder_no;
    Bit16u   byte_count;
  };
  Bit8u    buffer[MAX_MULTIPLE_SECTORS*512 + 4];
  Bit32u   buffer_size;
  Bit32u   buffer_index;
  Bit32u   drq_index;
  Bit8u    current_command;
  Bit8u    multiple_sectors;
  Bit8u    lba_mode;
  bx_bool  packet_dma;
  Bit8u    mdma_mode;
  struct {
    bx_bool reset;       // 0=normal, 1=reset controller
    bx_bool disable_irq; // 0=allow irq, 1=disable irq
    } control;
  Bit8u    reset_in_progress;
  Bit8u    features;
  } controller_t;

struct sense_info_t {
  sense_t sense_key;
  struct {
    Bit8u arr[4];
  } information;
  struct {
    Bit8u arr[4];
  } specific_inf;
  struct {
    Bit8u arr[3];
  } key_spec;
  Bit8u fruc;
  Bit8u asc;
  Bit8u ascq;
};

struct error_recovery_t {
  unsigned char data[8];

  error_recovery_t ();
};

Bit16u read_16bit(const Bit8u* buf) BX_CPP_AttrRegparmN(1);
Bit32u read_32bit(const Bit8u* buf) BX_CPP_AttrRegparmN(1);


struct cdrom_t
{
  bx_bool ready;
  bx_bool locked;
#ifdef LOWLEVEL_CDROM
  LOWLEVEL_CDROM* cd;
#endif
  Bit32u capacity;
  int next_lba;
  int remaining_blocks;
  struct currentStruct {
    error_recovery_t error_recovery;
  } current;
};

struct atapi_t
{
  Bit8u command;
  int drq_bytes;
  int total_bytes_remaining;
};

#if BX_USE_HD_SMF
#  define BX_HD_SMF  static
#  define BX_HD_THIS theHardDrive->
#else
#  define BX_HD_SMF
#  define BX_HD_THIS this->
#endif

typedef enum {
      IDE_NONE, IDE_DISK, IDE_CDROM
} device_type_t;

class bx_hard_drive_c : public bx_hard_drive_stub_c {
public:
  bx_hard_drive_c();
  virtual ~bx_hard_drive_c();
  virtual void   init();
  virtual void   reset(unsigned type);
  virtual Bit32u   get_device_handle(Bit8u channel, Bit8u device);
  virtual Bit32u   get_first_cd_handle(void);
  virtual unsigned get_cd_media_status(Bit32u handle);
  virtual unsigned set_cd_media_status(Bit32u handle, unsigned status);
#if BX_SUPPORT_PCI
  virtual bx_bool  bmdma_read_sector(Bit8u channel, Bit8u *buffer, Bit32u *sector_size);
  virtual bx_bool  bmdma_write_sector(Bit8u channel, Bit8u *buffer);
  virtual void     bmdma_complete(Bit8u channel);
#endif
#if BX_SUPPORT_SAVE_RESTORE
  virtual void     register_state(void);
#endif

  virtual Bit32u virt_read_handler(Bit32u address, unsigned io_len) {
    return read_handler (this, address, io_len);
  }
  virtual void virt_write_handler(Bit32u address, 
      Bit32u value, unsigned io_len)
  {
    write_handler(this, address, value, io_len);
  }
#if !BX_USE_HD_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

  static void iolight_timer_handler(void *);
  BX_HD_SMF void iolight_timer(void);

private:

  BX_HD_SMF bx_bool calculate_logical_address(Bit8u channel, off_t *sector) BX_CPP_AttrRegparmN(2);
  BX_HD_SMF void increment_address(Bit8u channel) BX_CPP_AttrRegparmN(1);
  BX_HD_SMF void identify_drive(Bit8u channel);
  BX_HD_SMF void identify_ATAPI_drive(Bit8u channel);
  BX_HD_SMF void command_aborted(Bit8u channel, unsigned command);

  BX_HD_SMF void init_send_atapi_command(Bit8u channel, Bit8u command, int req_length, int alloc_length, bx_bool lazy = false) BX_CPP_AttrRegparmN(3);
  BX_HD_SMF void ready_to_send_atapi(Bit8u channel) BX_CPP_AttrRegparmN(1);
  BX_HD_SMF void raise_interrupt(Bit8u channel) BX_CPP_AttrRegparmN(1);
  BX_HD_SMF void atapi_cmd_error(Bit8u channel, sense_t sense_key, asc_t asc, bx_bool show);
  BX_HD_SMF void init_mode_sense_single(Bit8u channel, const void* src, int size);
  BX_HD_SMF void atapi_cmd_nop(Bit8u channel) BX_CPP_AttrRegparmN(1);
  BX_HD_SMF bx_bool bmdma_present(void);
  BX_HD_SMF void set_signature(Bit8u channel, Bit8u id);
  BX_HD_SMF bx_bool ide_read_sector(Bit8u channel, Bit8u *buffer, Bit32u buffer_size);
  BX_HD_SMF bx_bool ide_write_sector(Bit8u channel, Bit8u *buffer, Bit32u buffer_size);

  // FIXME:
  // For each ATA channel we should have one controller struct
  // and an array of two drive structs
  struct channel_t {
    struct drive_t {
      device_image_t* hard_drive;
      device_type_t device_type;
      // 512 byte buffer for ID drive command
      // These words are stored in native word endian format, as
      // they are fetched and returned via a return(), so
      // there's no need to keep them in x86 endian format.
      Bit16u id_drive[256];

      controller_t controller;
      cdrom_t cdrom;
      sense_info_t sense;
      atapi_t atapi;

      Bit8u model_no[41];
      int statusbar_id;
      int iolight_counter;
      Bit8u device_num; // for ATAPI identify & inquiry
    } drives[2];
    unsigned drive_select;

    Bit16u ioaddr1;
    Bit16u ioaddr2;
    Bit8u  irq;

  } channels[BX_MAX_ATA_CHANNEL];

  int iolight_timer_index;
  Bit8u cdrom_count;
};

#endif
