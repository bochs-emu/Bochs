/////////////////////////////////////////////////////////////////////////
// $Id: harddrv.h,v 1.20 2003-05-03 16:37:17 cbothamy Exp $
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

// SPARSE IMAGES HEADER
#define SPARSE_HEADER_MAGIC  (0x02468ace)
#define SPARSE_HEADER_VERSION  1
#define SPARSE_HEADER_SIZE        (256) // Plenty of room for later
#define SPARSE_PAGE_NOT_ALLOCATED (0xffffffff)

 typedef struct
 {
   uint32  magic;
   uint32  version;
   uint32  pagesize;
   uint32  numpages;

   uint32  padding[60];
 } sparse_header_t;

#define STANDARD_HEADER_MAGIC     "Bochs Virtual HD Image"
#define STANDARD_HEADER_VERSION   (0x00010000)
#define STANDARD_HEADER_SIZE      (512)


 // WARNING : headers are kept in x86 (little) endianness
 typedef struct
 {
   Bit8u   magic[32];
   Bit8u   type[16];
   Bit8u   subtype[16];
   Bit32u  version;
   Bit32u  header;
 } standard_header_t;

#define REDOLOG_TYPE "Redolog"
#define REDOLOG_SUBTYPE_UNDOABLE "Undoable"
#define REDOLOG_SUBTYPE_VOLATILE "Volatile"
#define REDOLOG_SUBTYPE_GROWABLE "Growable"
// #define REDOLOG_SUBTYPE_Z_UNDOABLE "z-Undoable"
// #define REDOLOG_SUBTYPE_Z_VOLATILE "z-Volatile"

#define REDOLOG_PAGE_NOT_ALLOCATED (0xffffffff)

#define REDOLOG_EXTENSION ".redolog"

 typedef struct
 {
   // the fields in the header are kept in little endian
   Bit32u  catalog;    // #entries in the catalog
   Bit32u  bitmap;     // bitmap size in bytes
   Bit32u  extent;     // extent size in bytes
   Bit64u  disk;       // disk size in bytes
 } redolog_specific_header_t;

 typedef struct
 {
   standard_header_t standard;
   redolog_specific_header_t specific;

   Bit8u padding[STANDARD_HEADER_SIZE - (sizeof (standard_header_t) + sizeof (redolog_specific_header_t))];
 } redolog_header_t;

// htod : convert host to disk (little) endianness
// dtoh : convert disk (little) to host endianness
#if defined (BX_LITTLE_ENDIAN)
#define htod32(val) (val)
#define dtoh32(val) (val)
#define htod64(val) (val)
#define dtoh64(val) (val)
#else
#define htod32(val) ( (((val)&0xff000000)>>24) | (((val)&0xff0000)>>8) | (((val)&0xff00)<<8) | (((val)&0xff)<<24) )
#define dtoh32(val) htod32(val)
#define htod64(val) ( (((val)&0xff00000000000000LL)>>56) | (((val)&0xff000000000000LL)>>40) | (((val)&0xff0000000000LL)>>24) | (((val)&0xff00000000LL)>>8) | (((val)&0xff000000LL)<<8) | (((val)&0xff0000LL)<<24) | (((val)&0xff00LL)<<40) | (((val)&0xffLL)<<56) )
#define dtoh64(val) htod64(val)
#endif

#ifndef INCLUDE_ONLY_HD_HEADERS

typedef enum _sense {
      SENSE_NONE = 0, SENSE_NOT_READY = 2, SENSE_ILLEGAL_REQUEST = 5,
      SENSE_UNIT_ATTENTION = 6
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

// FLAT MODE
class default_image_t : public device_image_t
{
  public:
      // Open a image. Returns non-negative if successful.
      int open (const char* pathname);

      // Open an image with specific flags. Returns non-negative if successful.
      int open (const char* pathname, int flags);

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

// CONCAT MODE
class concat_image_t : public device_image_t
{
  public:
      // Default constructor
      concat_image_t();
      
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
      off_t start_offset_table[BX_CONCAT_MAX_IMAGES];
      off_t length_table[BX_CONCAT_MAX_IMAGES];
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
      off_t thismin, thismax; // byte offset boundary of this image
};

// SPARSE MODE
class sparse_image_t : public device_image_t
{

// Format of a sparse file:
// 256 byte header, containing details such as page size and number of pages
// Page indirection table, mapping virtual pages to physical pages within file
// Physical pages till end of file

  public:
      // Default constructor
      sparse_image_t();

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

#ifdef _POSIX_MAPPED_FILES
 void *  mmap_header;
 size_t  mmap_length;
 size_t  system_pagesize_mask;
#endif
 uint32 *  pagetable;

 // Header is written to disk in little-endian (x86) format
 // Thus needs to be converted on big-endian systems before read
 // The pagetable is also kept little endian

 sparse_header_t header;

 uint32  pagesize;
 int     pagesize_shift;
 uint32  pagesize_mask;

 off_t   data_start;
 off_t   underlying_filesize;

 char *  pathname;

 off_t position;

 uint32 position_virtual_page;
 uint32 position_physical_page;
 uint32 position_page_offset;

 off_t underlying_current_filepos;

 off_t total_size;

 void panic(const char * message);
 off_t sparse_image_t::get_physical_offset();
 void sparse_image_t::set_virtual_page(uint32 new_virtual_page);
 void read_header();
 ssize_t read_page_fragment(uint32 read_virtual_page, uint32 read_page_offset, size_t read_size, void * buf);

 sparse_image_t *  parent_image;
};

#if EXTERNAL_DISK_SIMULATOR
#include "external-disk-simulator.h"
#endif

#if DLL_HD_SUPPORT
class dll_image_t : public device_image_t
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
      int vunit,vblk;

};
#endif

// REDOLOG class
class redolog_t 
{
  public:
      redolog_t();
      int make_header (const char* type, Bit64u size);
      int create (const char* filename, const char* type, Bit64u size);
      int create (int filedes, const char* type, Bit64u size);
      int open (const char* filename, const char* type, Bit64u size);
      void close ();

      off_t lseek (off_t offset, int whence);
      ssize_t read (void* buf, size_t count);
      ssize_t write (const void* buf, size_t count);

  private:
      void             print_header();
      int              fd;
      redolog_header_t header;     // Header is kept in x86 (little) endianness
      Bit32u          *catalog;
      Bit8u           *bitmap;
      Bit32u           extent_index;
      Bit32u           extent_offset;
      Bit32u           extent_next;

      Bit32u           bitmap_blocs;
      Bit32u           extent_blocs;
};

// GROWABLE MODE
class growable_image_t : public device_image_t
{
  public:
      // Contructor
      growable_image_t(Bit64u size);

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
      redolog_t *redolog;
      Bit64u    size;
};

// UNDOABLE MODE
class undoable_image_t : public device_image_t
{
  public:
      // Contructor
      undoable_image_t(Bit64u size);

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
      redolog_t *redolog;
      default_image_t *ro_disk;
      Bit64u    size;
};


// VOLATILE MODE
class volatile_image_t : public device_image_t
{
  public:
      // Contructor
      volatile_image_t(Bit64u size);

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
      redolog_t       *redolog;
      default_image_t *ro_disk;
      Bit64u          size;
      char            *redolog_name;
};


#if BX_COMPRESSED_HD_SUPPORT

#include <zlib.h>


// Default compressed READ-ONLY image class
class z_ro_image_t : public device_image_t
{
  public:
      // Contructor
      z_ro_image_t();

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
      off_t offset;
      int fd;
      gzFile gzfile;

};

// Z-UNDOABLE MODE
class z_undoable_image_t : public device_image_t
{
  public:
      // Contructor
      z_undoable_image_t(Bit64u size);

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
      redolog_t    *redolog;
      z_ro_image_t *ro_disk;
      Bit64u       size;
};

// Z-VOLATILE MODE
class z_volatile_image_t : public device_image_t
{
  public:
      // Contructor
      z_volatile_image_t(Bit64u size);

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
      redolog_t    *redolog;
      z_ro_image_t *ro_disk;
      Bit64u       size;
      char         *redolog_name;
};

#endif


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
  Bit8u    buffer[2048];
  Bit32u   buffer_index;
  Bit32u   drq_index;
  Bit8u    current_command;
  Bit8u    sectors_per_block;
  Bit8u    lba_mode;
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

uint16 read_16bit(const uint8* buf) BX_CPP_AttrRegparmN(1);
uint32 read_32bit(const uint8* buf) BX_CPP_AttrRegparmN(1);


#ifdef LOWLEVEL_CDROM
#  include "cdrom.h"
#endif


struct cdrom_t
{
  bx_bool ready;
  bx_bool locked;
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

  bx_hard_drive_c(void);
  virtual ~bx_hard_drive_c(void);
  virtual void   close_harddrive(void);
  virtual void   init();
  virtual void   reset(unsigned type);
  virtual Bit32u   get_device_handle(Bit8u channel, Bit8u device);
  virtual Bit32u   get_first_cd_handle(void);
  virtual unsigned get_cd_media_status(Bit32u handle);
  virtual unsigned set_cd_media_status(Bit32u handle, unsigned status);

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

private:

  BX_HD_SMF bx_bool calculate_logical_address(Bit8u channel, off_t *sector) BX_CPP_AttrRegparmN(2);
  BX_HD_SMF void increment_address(Bit8u channel) BX_CPP_AttrRegparmN(1);
  BX_HD_SMF void identify_drive(Bit8u channel);
  BX_HD_SMF void identify_ATAPI_drive(Bit8u channel);
  BX_HD_SMF void command_aborted(Bit8u channel, unsigned command);

  BX_HD_SMF void init_send_atapi_command(Bit8u channel, Bit8u command, int req_length, int alloc_length, bool lazy = false) BX_CPP_AttrRegparmN(3);
  BX_HD_SMF void ready_to_send_atapi(Bit8u channel) BX_CPP_AttrRegparmN(1);
  BX_HD_SMF void raise_interrupt(Bit8u channel) BX_CPP_AttrRegparmN(1);
  BX_HD_SMF void atapi_cmd_error(Bit8u channel, sense_t sense_key, asc_t asc);
  BX_HD_SMF void init_mode_sense_single(Bit8u channel, const void* src, int size);
  BX_HD_SMF void atapi_cmd_nop(Bit8u channel) BX_CPP_AttrRegparmN(1);

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
      } drives[2];
    unsigned drive_select;

    Bit16u ioaddr1;
    Bit16u ioaddr2;
    Bit8u  irq;

    } channels[BX_MAX_ATA_CHANNEL];

#if BX_PDC20230C_VLBIDE_SUPPORT
// pdc20630c is only available for 1st ata channel
  struct pdc20630c_t {
    bx_bool prog_mode;
    Bit8u   prog_count;
    Bit32u  p1f3_value;
    Bit32u  p1f4_value;
    } pdc20230c;
#endif

  };
#endif // INCLUDE_ONLY_SPARSE_HEADER

