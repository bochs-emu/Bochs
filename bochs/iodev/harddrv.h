/////////////////////////////////////////////////////////////////////////
// $Id: harddrv.h,v 1.8 2002-02-01 16:46:27 vruppert Exp $
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


typedef enum _sense {
      SENSE_NONE = 0, SENSE_NOT_READY = 2, SENSE_ILLEGAL_REQUEST = 5
} sense_t;

typedef enum _asc {
      ASC_INV_FIELD_IN_CMD_PACKET = 0x24,
      ASC_MEDIUM_NOT_PRESENT = 0x3a,
      ASC_SAVING_PARAMETERS_NOT_SUPPORTED = 0x39,
      ASC_LOGICAL_BLOCK_OOR = 0x21
} asc_t;

class LOWLEVEL_CDROM;

class device_image_t
{
  public:
      // Open a image. Returns non-negative if successful.
      virtual int open (const char* pathname) = 0;

      // Close the image.
      virtual void close () = 0;

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      virtual off_t lseek (off_t offset, int whence) = 0;

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      virtual ssize_t read (void* buf, size_t count) = 0;

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      virtual ssize_t write (const void* buf, size_t count) = 0;

      unsigned cylinders;
      unsigned heads;
      unsigned sectors;
};

class default_image_t : public device_image_t
{
  public:
      // Open a image. Returns non-negative if successful.
      int open (const char* pathname);

      // Close the image.
      void close ();

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      off_t lseek (off_t offset, int whence);

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      ssize_t read (void* buf, size_t count);

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      ssize_t write (const void* buf, size_t count);

  private:
      int fd;
      
};

#if BX_SPLIT_HD_SUPPORT
class concat_image_t : public device_image_t
{
  public:
      // Open a image. Returns non-negative if successful.
      int open (const char* pathname);

      // Close the image.
      void close ();

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      off_t lseek (off_t offset, int whence);

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      ssize_t read (void* buf, size_t count);

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      ssize_t write (const void* buf, size_t count);

  private:
#define BX_CONCAT_MAX_IMAGES 8
      int fd_table[BX_CONCAT_MAX_IMAGES];
      ssize_t start_offset_table[BX_CONCAT_MAX_IMAGES];
      ssize_t length_table[BX_CONCAT_MAX_IMAGES];
      void increment_string (char *str);
      int maxfd;  // number of entries in tables that are valid

      // notice if anyone does sequential read or write without seek in between.
      // This can be supported pretty easily, but needs additional checks.
      // 0=something other than seek was last operation
      // 1=seek was last operation
      int seek_was_last_op;

      // the following variables tell which partial image file to use for
      // the next read and write.
      int index;  // index into table
      int fd;     // fd to use for reads and writes
      int thismin, thismax; // byte offset boundary of this image
};
#endif /* BX_SPLIT_HD_SUPPORT */

#if EXTERNAL_DISK_SIMULATOR
#include "external-disk-simulator.h"
#endif

typedef struct {
  struct {
    Boolean busy;
    Boolean drive_ready;
    Boolean write_fault;
    Boolean seek_complete;
    Boolean drq;
    Boolean corrected_data;
    Boolean index_pulse;
    unsigned index_pulse_count;
    Boolean err;
    } status;
  Bit8u    error_register;
  // Bit8u    drive_select; this field was moved :^(
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
  Bit8u    buffer[2048];
  unsigned buffer_index;
  Bit8u    current_command;
  Bit8u    sectors_per_block;
  Bit8u    lba_mode;
  struct {
    Boolean reset;       // 0=normal, 1=reset controller
    Boolean disable_irq; // 0=allow irq, 1=disable irq
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

uint16 read_16bit(const uint8* buf);
uint32 read_32bit(const uint8* buf);


#ifdef LOWLEVEL_CDROM
#  include "cdrom.h"
#endif


struct cdrom_t
{
  Boolean ready;
  Boolean locked;
#ifdef LOWLEVEL_CDROM
  LOWLEVEL_CDROM* cd;
#endif
  uint32 capacity;
  int next_lba;
  int remaining_blocks;
  struct currentStruct {
    error_recovery_t error_recovery;
  } current;
};

struct atapi_t
{
  uint8 command;
  int drq_bytes;
  int total_bytes_remaining;
};

#if BX_USE_HD_SMF
#  define BX_HD_SMF  static
#  define BX_HD_THIS bx_hard_drive.
#else
#  define BX_HD_SMF
#  define BX_HD_THIS this->
#endif

#define BX_SELECTED_HD BX_HD_THIS s[BX_HD_THIS drive_select]
#define CDROM_SELECTED (BX_HD_THIS s[BX_HD_THIS drive_select].device_type == IDE_CDROM)
#define DEVICE_TYPE_STRING ((CDROM_SELECTED) ? "CD-ROM" : "DISK")

typedef enum {
      IDE_DISK, IDE_CDROM
} device_type_t;

class bx_hard_drive_c : public logfunctions {
public:

  bx_hard_drive_c(void);
  ~bx_hard_drive_c(void);
  BX_HD_SMF void   close_harddrive(void);
  BX_HD_SMF void   init(bx_devices_c *d, bx_cmos_c *cmos);
  BX_HD_SMF unsigned get_cd_media_status(void);
  BX_HD_SMF unsigned set_cd_media_status(unsigned status);

#if !BX_USE_HD_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

private:

  BX_HD_SMF Boolean calculate_logical_address(Bit32u *sector);
  BX_HD_SMF void increment_address();
  BX_HD_SMF void identify_drive(unsigned drive);
  BX_HD_SMF void identify_ATAPI_drive(unsigned drive);
  BX_HD_SMF void command_aborted(unsigned command);

  BX_HD_SMF void init_send_atapi_command(Bit8u command, int req_length, int alloc_length, bool lazy = false);
  BX_HD_SMF void ready_to_send_atapi();
  BX_HD_SMF void raise_interrupt();
  BX_HD_SMF void atapi_cmd_error(sense_t sense_key, asc_t asc);
  BX_HD_SMF void init_mode_sense_single(const void* src, int size);
  BX_HD_SMF void atapi_cmd_nop();

  struct sStruct {
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

    } s[2];

  unsigned drive_select;

  bx_devices_c *devices;
  };

extern bx_hard_drive_c bx_hard_drive;
