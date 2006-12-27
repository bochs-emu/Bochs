/////////////////////////////////////////////////////////////////////////
// $Id: hdimage.h,v 1.8 2006-12-27 15:21:03 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2005  MandrakeSoft S.A.
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

#ifndef BX_HDIMAGE_H
#define BX_HDIMAGE_H

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
// #define REDOLOG_SUBTYPE_Z_UNDOABLE "z-Undoable"
// #define REDOLOG_SUBTYPE_Z_VOLATILE "z-Volatile"

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
   Bit32u  reserved;   // for data alignment
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

#ifndef HDIMAGE_HEADERS_ONLY

class device_image_t
{
  public:
      // Default constructor
      device_image_t();
      virtual ~device_image_t() {}

      // Open a image. Returns non-negative if successful.
      virtual int open(const char* pathname) = 0;

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

      unsigned cylinders;
      unsigned heads;
      unsigned sectors;
      Bit64u   hd_size;
};

// FLAT MODE
class default_image_t : public device_image_t
{
  public:
      // Open a image. Returns non-negative if successful.
      int open(const char* pathname);

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
      int fd;

};

// CONCAT MODE
class concat_image_t : public device_image_t
{
  public:
      // Default constructor
      concat_image_t();
      
      // Open a image. Returns non-negative if successful.
      int open(const char* pathname);

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
      int open(const char* pathname);

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
 int fd;

#ifdef _POSIX_MAPPED_FILES
 void *  mmap_header;
 size_t  mmap_length;
 size_t  system_pagesize_mask;
#endif
 Bit32u *  pagetable;

 // Header is written to disk in little-endian (x86) format
 // Thus needs to be converted on big-endian systems before read
 // The pagetable is also kept little endian

 sparse_header_t header;

 Bit32u  pagesize;
 int     pagesize_shift;
 Bit32u  pagesize_mask;

 Bit64s  data_start;
 Bit64s  underlying_filesize;

 char *  pathname;

 Bit64s position;

 Bit32u position_virtual_page;
 Bit32u position_physical_page;
 Bit32u position_page_offset;

 Bit64s underlying_current_filepos;

 Bit64s total_size;

 void panic(const char * message);
 Bit64s get_physical_offset();
 void set_virtual_page(Bit32u new_virtual_page);
 void read_header();
 ssize_t read_page_fragment(Bit32u read_virtual_page, Bit32u read_page_offset, size_t read_size, void * buf);

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
      int open(const char* pathname);

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
      int vunit,vblk;

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
      void close();
      Bit64u get_size();

      Bit64s lseek(Bit64s offset, int whence);
      ssize_t read(void* buf, size_t count);
      ssize_t write(const void* buf, size_t count);

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

// GROWING MODE
class growing_image_t : public device_image_t
{
  public:
      // Contructor
      growing_image_t();

      // Open a image. Returns non-negative if successful.
      int open(const char* pathname);

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
      redolog_t *redolog;
};

// UNDOABLE MODE
class undoable_image_t : public device_image_t
{
  public:
      // Contructor
      undoable_image_t(const char* redolog_name);

      // Open a image. Returns non-negative if successful.
      int open(const char* pathname);

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
      redolog_t       *redolog;       // Redolog instance
      default_image_t *ro_disk;       // Read-only flat disk instance
      char            *redolog_name;  // Redolog name
};


// VOLATILE MODE
class volatile_image_t : public device_image_t
{
  public:
      // Contructor
      volatile_image_t(const char* redolog_name);

      // Open a image. Returns non-negative if successful.
      int open(const char* pathname);

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
      redolog_t       *redolog;       // Redolog instance
      default_image_t *ro_disk;       // Read-only flat disk instance
      char            *redolog_name;  // Redolog name
      char            *redolog_temp;  // Redolog temporary file name
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
      int open(const char* pathname);

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
      Bit64s offset;
      int fd;
      gzFile gzfile;

};

// Z-UNDOABLE MODE
class z_undoable_image_t : public device_image_t
{
  public:
      // Contructor
      z_undoable_image_t(Bit64u size, const char* redolog_name);

      // Open a image. Returns non-negative if successful.
      int open(const char* pathname);

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
      redolog_t       *redolog;       // Redolog instance
      z_ro_image_t    *ro_disk;       // Read-only compressed flat disk instance
      Bit64u          size;           
      char            *redolog_name;  // Redolog name
};

// Z-VOLATILE MODE
class z_volatile_image_t : public device_image_t
{
  public:
      // Contructor
      z_volatile_image_t(Bit64u size, const char* redolog_name);

      // Open a image. Returns non-negative if successful.
      int open(const char* pathname);

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
      redolog_t       *redolog;       // Redolog instance
      z_ro_image_t    *ro_disk;       // Read-only compressed flat disk instance
      Bit64u          size;           
      char            *redolog_name;  // Redolog name
      char            *redolog_temp;  // Redolog temporary file name
};

#endif

#endif // HDIMAGE_HEADERS_ONLY

#endif
