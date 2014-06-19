/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2005-2014  The Bochs Project
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

#ifndef BX_HDIMAGE_H
#define BX_HDIMAGE_H

// required for access() checks
#ifndef F_OK
#define F_OK 0
#endif

// hdimage capabilities
#define HDIMAGE_READONLY      1
#define HDIMAGE_HAS_GEOMETRY  2
#define HDIMAGE_AUTO_GEOMETRY 4

// hdimage format check return values
#define HDIMAGE_FORMAT_OK      0
#define HDIMAGE_SIZE_ERROR    -1
#define HDIMAGE_READ_ERROR    -2
#define HDIMAGE_NO_SIGNATURE  -3
#define HDIMAGE_TYPE_ERROR    -4
#define HDIMAGE_VERSION_ERROR -5

// SPARSE IMAGES HEADER
#define SPARSE_HEADER_MAGIC  (0x02468ace)
#define SPARSE_HEADER_VERSION  2
#define SPARSE_HEADER_V1       1
#define SPARSE_HEADER_SIZE        (256) // Plenty of room for later
#define SPARSE_PAGE_NOT_ALLOCATED (0xffffffff)

 typedef struct
 {
   Bit32u  magic;
   Bit32u  version;
   Bit32u  pagesize;
   Bit32u  numpages;
   Bit64u  disk;

   Bit32u  padding[58];
 } sparse_header_t;

#define STANDARD_HEADER_MAGIC     "Bochs Virtual HD Image"
#define STANDARD_HEADER_V1        (0x00010000)
#define STANDARD_HEADER_VERSION   (0x00020000)
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
#define REDOLOG_SUBTYPE_GROWING  "Growing"

#define REDOLOG_PAGE_NOT_ALLOCATED (0xffffffff)

#define UNDOABLE_REDOLOG_EXTENSION ".redolog"
#define UNDOABLE_REDOLOG_EXTENSION_LENGTH (strlen(UNDOABLE_REDOLOG_EXTENSION))
#define VOLATILE_REDOLOG_EXTENSION ".XXXXXX"
#define VOLATILE_REDOLOG_EXTENSION_LENGTH (strlen(VOLATILE_REDOLOG_EXTENSION))

 typedef struct
 {
   // the fields in the header are kept in little endian
   Bit32u  catalog;    // #entries in the catalog
   Bit32u  bitmap;     // bitmap size in bytes
   Bit32u  extent;     // extent size in bytes
   Bit32u  timestamp;  // modification time in FAT format (subtype 'undoable' only)
   Bit64u  disk;       // disk size in bytes
 } redolog_specific_header_t;

 typedef struct
 {
   // the fields in the header are kept in little endian
   Bit32u  catalog;    // #entries in the catalog
   Bit32u  bitmap;     // bitmap size in bytes
   Bit32u  extent;     // extent size in bytes
   Bit64u  disk;       // disk size in bytes
 } redolog_specific_header_v1_t;

 typedef struct
 {
   standard_header_t standard;
   redolog_specific_header_t specific;

   Bit8u padding[STANDARD_HEADER_SIZE - (sizeof (standard_header_t) + sizeof (redolog_specific_header_t))];
 } redolog_header_t;

 typedef struct
 {
   standard_header_t standard;
   redolog_specific_header_v1_t specific;

   Bit8u padding[STANDARD_HEADER_SIZE - (sizeof (standard_header_t) + sizeof (redolog_specific_header_v1_t))];
 } redolog_header_v1_t;

// htod : convert host to disk (little) endianness
// dtoh : convert disk (little) to host endianness
#if defined (BX_LITTLE_ENDIAN)
#define htod16(val) (val)
#define dtoh16(val) (val)
#define htod32(val) (val)
#define dtoh32(val) (val)
#define htod64(val) (val)
#define dtoh64(val) (val)
#else
#define htod16(val) ((((val)&0xff00)>>8) | (((val)&0xff)<<8))
#define dtoh16(val) htod16(val)
#define htod32(val) bx_bswap32(val)
#define dtoh32(val) htod32(val)
#define htod64(val) bx_bswap64(val)
#define dtoh64(val) htod64(val)
#endif

class device_image_t;
class redolog_t;

int bx_read_image(int fd, Bit64s offset, void *buf, int count);
int bx_write_image(int fd, Bit64s offset, void *buf, int count);
#ifndef WIN32
int hdimage_open_file(const char *pathname, int flags, Bit64u *fsize, time_t *mtime);
#else
int hdimage_open_file(const char *pathname, int flags, Bit64u *fsize, FILETIME *mtime);
#endif
int hdimage_detect_image_mode(const char *pathname);
bx_bool hdimage_backup_file(int fd, const char *backup_fname);
bx_bool hdimage_copy_file(const char *src, const char *dst);
bx_bool coherency_check(device_image_t *ro_disk, redolog_t *redolog);
#ifndef WIN32
Bit16u fat_datetime(time_t time, int return_time);
#else
Bit16u fat_datetime(FILETIME time, int return_time);
#endif

// base class
class device_image_t
{
  public:
      // Default constructor
      device_image_t();
      virtual ~device_image_t() {}

      // Open a image. Returns non-negative if successful.
      virtual int open(const char* pathname);

      // Open an image with specific flags. Returns non-negative if successful.
      virtual int open(const char* pathname, int flags) = 0;

      // Close the image.
      virtual void close() = 0;

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      virtual Bit64s lseek(Bit64s offset, int whence) = 0;

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      virtual ssize_t read(void* buf, size_t count) = 0;

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      virtual ssize_t write(const void* buf, size_t count) = 0;

      // Get image capabilities
      virtual Bit32u get_capabilities();

      // Get modification time in FAT format
      virtual Bit32u get_timestamp();

      // Check image format
      static int check_format(int fd, Bit64u imgsize) {return HDIMAGE_NO_SIGNATURE;}

#ifndef BXIMAGE
      // Save/restore support
      virtual void register_state(bx_list_c *parent);
      virtual bx_bool save_state(const char *backup_fname) {return 0;}
      virtual void restore_state(const char *backup_fname) {}
#endif

      unsigned cylinders;
      unsigned heads;
      unsigned spt;
      Bit64u   hd_size;
  protected:
#ifndef WIN32
      time_t mtime;
#else
      FILETIME mtime;
#endif
};

// FLAT MODE
class flat_image_t : public device_image_t
{
  public:
      // Open an image with specific flags. Returns non-negative if successful.
      int open(const char* pathname, int flags);

      // Close the image.
      void close();

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      Bit64s lseek(Bit64s offset, int whence);

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      ssize_t read(void* buf, size_t count);

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      ssize_t write(const void* buf, size_t count);

      // Check image format
      static int check_format(int fd, Bit64u imgsize);

#ifndef BXIMAGE
      // Save/restore support
      bx_bool save_state(const char *backup_fname);
      void restore_state(const char *backup_fname);
#endif

  private:
      int fd;
      const char *pathname;
};

// CONCAT MODE
class concat_image_t : public device_image_t
{
  public:
      // Default constructor
      concat_image_t();

      // Open an image with specific flags. Returns non-negative if successful.
      int open(const char* pathname, int flags);

      // Close the image.
      void close();

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      Bit64s lseek(Bit64s offset, int whence);

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      ssize_t read(void* buf, size_t count);

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      ssize_t write(const void* buf, size_t count);

#ifndef BXIMAGE
      // Save/restore support
      bx_bool save_state(const char *backup_fname);
      void restore_state(const char *backup_fname);
#endif

  private:
#define BX_CONCAT_MAX_IMAGES 8
      int fd_table[BX_CONCAT_MAX_IMAGES];
      Bit64s start_offset_table[BX_CONCAT_MAX_IMAGES];
      Bit64s length_table[BX_CONCAT_MAX_IMAGES];
      void increment_string(char *str);
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
      Bit64s thismin, thismax; // byte offset boundary of this image
      Bit64s total_offset;     // current byte offset
      const char *pathname0;
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

    // Open an image with specific flags. Returns non-negative if successful.
    int open(const char* pathname, int flags);

    // Close the image.
    void close();

    // Position ourselves. Return the resulting offset from the
    // beginning of the file.
    Bit64s lseek(Bit64s offset, int whence);

    // Read count bytes to the buffer buf. Return the number of
    // bytes read (count).
    ssize_t read(void* buf, size_t count);

    // Write count bytes from buf. Return the number of bytes
    // written (count).
    ssize_t write(const void* buf, size_t count);

    // Check image format
    static int check_format(int fd, Bit64u imgsize);

#ifndef BXIMAGE
    // Save/restore support
    bx_bool save_state(const char *backup_fname);
    void restore_state(const char *backup_fname);
#endif

  private:
    int fd;

#ifdef _POSIX_MAPPED_FILES
    void *  mmap_header;
    size_t  mmap_length;
    size_t  system_pagesize_mask;
#endif
    Bit32u *pagetable;

    // Header is written to disk in little-endian (x86) format
    // Thus needs to be converted on big-endian systems before read
    // The pagetable is also kept little endian

    sparse_header_t header;

    Bit32u pagesize;
    int    pagesize_shift;
    Bit32u pagesize_mask;

    Bit64s data_start;
    Bit64u underlying_filesize;

    char *pathname;

    Bit64s position;

    Bit32u position_virtual_page;
    Bit32u position_physical_page;
    Bit32u position_page_offset;

    Bit64s underlying_current_filepos;

    Bit64s total_size;

    void panic(const char * message);
    Bit64s get_physical_offset();
    void set_virtual_page(Bit32u new_virtual_page);
    int read_header();
    ssize_t read_page_fragment(Bit32u read_virtual_page, Bit32u read_page_offset, size_t read_size, void * buf);

    sparse_image_t *parent_image;
};

#if EXTERNAL_DISK_SIMULATOR
#include "external-disk-simulator.h"
#endif

#ifdef WIN32
class dll_image_t : public device_image_t
{
  public:
      dll_image_t();

      // Open an image with specific flags. Returns non-negative if successful.
      int open(const char* pathname, int flags);

      // Close the image.
      void close();

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      Bit64s lseek(Bit64s offset, int whence);

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      ssize_t read(void* buf, size_t count);

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      ssize_t write(const void* buf, size_t count);

  private:
      int vunit;
      Bit64s vblk;
};
#endif

// REDOLOG class
class redolog_t
{
  public:
      redolog_t();
      int make_header(const char* type, Bit64u size);
      int create(const char* filename, const char* type, Bit64u size);
      int create(int filedes, const char* type, Bit64u size);
      int open(const char* filename, const char* type);
      int open(const char* filename, const char* type, int flags);
      void close();
      Bit64u get_size();
      Bit32u get_timestamp();
      bx_bool set_timestamp(Bit32u timestamp);

      Bit64s lseek(Bit64s offset, int whence);
      ssize_t read(void* buf, size_t count);
      ssize_t write(const void* buf, size_t count);

      static int check_format(int fd, const char *subtype);

#ifdef BXIMAGE
      int commit(device_image_t *base_image);
#else
      bx_bool save_state(const char *backup_fname);
#endif

  private:
      void             print_header();
      int              fd;
      redolog_header_t header;     // Header is kept in x86 (little) endianness
      Bit32u          *catalog;
      Bit8u           *bitmap;
      bx_bool          bitmap_update;
      Bit32u           extent_index;
      Bit32u           extent_offset;
      Bit32u           extent_next;

      Bit32u           bitmap_blocks;
      Bit32u           extent_blocks;

      Bit64s           imagepos;
};

// GROWING MODE
class growing_image_t : public device_image_t
{
  public:
      // Contructor
      growing_image_t();
      virtual ~growing_image_t();

      // Open an image with specific flags. Returns non-negative if successful.
      int open(const char* pathname, int flags);

      // Close the image.
      void close();

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      Bit64s lseek(Bit64s offset, int whence);

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      ssize_t read(void* buf, size_t count);

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      ssize_t write(const void* buf, size_t count);

      // Get modification time in FAT format
      virtual Bit32u get_timestamp();

      // Check image format
      static int check_format(int fd, Bit64u imgsize);

#ifndef BXIMAGE
      // Save/restore support
      bx_bool save_state(const char *backup_fname);
      void restore_state(const char *backup_fname);
#endif

  private:
      redolog_t *redolog;
      const char *pathname;
};

// UNDOABLE MODE
class undoable_image_t : public device_image_t
{
  public:
      // Contructor
      undoable_image_t(const char* redolog_name);
      virtual ~undoable_image_t();

      // Open an image with specific flags. Returns non-negative if successful.
      int open(const char* pathname, int flags);

      // Close the image.
      void close();

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      Bit64s lseek(Bit64s offset, int whence);

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      ssize_t read(void* buf, size_t count);

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      ssize_t write(const void* buf, size_t count);

#ifndef BXIMAGE
      // Save/restore support
      bx_bool save_state(const char *backup_fname);
      void restore_state(const char *backup_fname);
#endif

  private:
      redolog_t       *redolog;       // Redolog instance
      device_image_t  *ro_disk;       // Read-only base disk instance
      char            *redolog_name;  // Redolog name
};


// VOLATILE MODE
class volatile_image_t : public device_image_t
{
  public:
      // Contructor
      volatile_image_t(const char* redolog_name);
      virtual ~volatile_image_t();

      // Open an image with specific flags. Returns non-negative if successful.
      int open(const char* pathname, int flags);

      // Close the image.
      void close();

      // Position ourselves. Return the resulting offset from the
      // beginning of the file.
      Bit64s lseek(Bit64s offset, int whence);

      // Read count bytes to the buffer buf. Return the number of
      // bytes read (count).
      ssize_t read(void* buf, size_t count);

      // Write count bytes from buf. Return the number of bytes
      // written (count).
      ssize_t write(const void* buf, size_t count);

#ifndef BXIMAGE
      // Save/restore support
      bx_bool save_state(const char *backup_fname);
      void restore_state(const char *backup_fname);
#endif

  private:
      redolog_t       *redolog;       // Redolog instance
      device_image_t  *ro_disk;       // Read-only base disk instance
      char            *redolog_name;  // Redolog name
      char            *redolog_temp;  // Redolog temporary file name
};


#ifndef BXIMAGE
class bx_hdimage_ctl_c : public bx_hdimage_ctl_stub_c {
public:
  bx_hdimage_ctl_c();
  virtual ~bx_hdimage_ctl_c() {}
  virtual device_image_t *init_image(Bit8u image_mode, Bit64u disk_size, const char *journal);
  virtual cdrom_base_c *init_cdrom(const char *dev);
};
#endif // BXIMAGE

#endif
