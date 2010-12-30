/////////////////////////////////////////////////////////////////////////
// $Id: vvfat.cc,v 1.6 2010-12-30 12:30:58 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
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

// Virtual VFAT image support (shadows a local directory)
// ported from the Qemu block driver by Johannes E. Schindelin

// ADDITIONS:
// - win32 specific directory functions (required for MSVC)
// - configurable disk geometry

// TODO:
// - write support
// - FAT32 support
// - read MBR and boot sector from file

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#ifndef WIN32
#include <dirent.h>
#endif

#define NO_DEVICE_INCLUDES
#include "iodev.h"
#include "hdimage.h"
#include "vvfat.h"

#define LOG_THIS bx_devices.pluginHardDrive->

#define VVFAT_MBR  "vvfat_mbr.bin"
#define VVFAT_BOOT "vvfat_boot.bin"

#if defined (BX_LITTLE_ENDIAN)
#define htod16(val) (val)
#else
#define htod16(val) ( (((val)&0xff00)>>8) | (((val)&0xff)<<8) )
#endif

// dynamic array functions
static inline void array_init(array_t* array,unsigned int item_size)
{
  array->pointer = NULL;
  array->size = 0;
  array->next = 0;
  array->item_size = item_size;
}

static inline void array_free(array_t* array)
{
  if (array->pointer)
    free(array->pointer);
  array->size=array->next = 0;
}

// does not automatically grow
static inline void* array_get(array_t* array,unsigned int index)
{
  assert(index < array->next);
  return array->pointer + index * array->item_size;
}

static inline int array_ensure_allocated(array_t* array, int index)
{
  if ((index + 1) * array->item_size > array->size) {
    int new_size = (index + 32) * array->item_size;
    array->pointer = (char*)realloc(array->pointer, new_size);
    if (!array->pointer)
      return -1;
    array->size = new_size;
    array->next = index + 1;
  }
  return 0;
}

static inline void* array_get_next(array_t* array)
{
  unsigned int next = array->next;
  void* result;

  if (array_ensure_allocated(array, next) < 0)
    return NULL;

  array->next = next + 1;
  result = array_get(array, next);

  return result;
}

static inline void* array_insert(array_t* array,unsigned int index,unsigned int count)
{
  if ((array->next+count)*array->item_size > array->size) {
    int increment = count*array->item_size;
    array->pointer = (char*)realloc(array->pointer, array->size+increment);
    if (!array->pointer)
      return NULL;
    array->size += increment;
  }
  memmove(array->pointer+(index+count)*array->item_size,
          array->pointer+index*array->item_size,
          (array->next-index)*array->item_size);
  array->next += count;
  return array->pointer+index*array->item_size;
}

/* this performs a "roll", so that the element which was at index_from becomes
 * index_to, but the order of all other elements is preserved. */
static inline int array_roll(array_t* array, int index_to, int index_from, int count)
{
  char* buf;
  char* from;
  char* to;
  int is;

  if (!array ||
      (index_to < 0) || (index_to >= (int)array->next) ||
      (index_from < 0 || (index_from >= (int)array->next)))
    return -1;

  if (index_to == index_from)
    return 0;

  is = array->item_size;
  from = array->pointer+index_from*is;
  to = array->pointer+index_to*is;
  buf = (char*)malloc(is*count);
  memcpy(buf, from, is*count);

  if (index_to < index_from)
    memmove(to+is*count, to, from-to);
  else
    memmove(from, from+is*count, to-from);

  memcpy(to, buf, is*count);

  free(buf);

  return 0;
}

static inline int array_remove_slice(array_t* array,int index, int count)
{
  assert(index >=0);
  assert(count > 0);
  assert(index + count <= (int)array->next);
  if (array_roll(array,array->next-1,index,count))
    return -1;
  array->next -= count;
  return 0;
}

static int array_remove(array_t* array,int index)
{
  return array_remove_slice(array, index, 1);
}

// return the index for a given member
static int array_index(array_t* array, void* pointer)
{
  size_t offset = (char*)pointer - array->pointer;
  assert((offset % array->item_size) == 0);
  assert(offset/array->item_size < array->next);
  return offset/array->item_size;
}

#if defined(_MSC_VER)
#pragma pack(push, 1)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=packed
#endif

typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
  struct bootsector_t {
    Bit8u  jump[3];
    Bit8u  name[8];
    Bit16u sector_size;
    Bit8u  sectors_per_cluster;
    Bit16u reserved_sectors;
    Bit8u  number_of_fats;
    Bit16u root_entries;
    Bit16u total_sectors16;
    Bit8u  media_type;
    Bit16u sectors_per_fat;
    Bit16u sectors_per_track;
    Bit16u number_of_heads;
    Bit32u hidden_sectors;
    Bit32u total_sectors;
    union {
#if defined(_MSC_VER) && (_MSC_VER>=1300)
      __declspec(align(1))
#endif
      struct {
        Bit8u  drive_number;
        Bit8u  reserved;
        Bit8u  signature;
        Bit32u id;
        Bit8u  volume_label[11];
        Bit8u fat_type[8];
        Bit8u ignored[0x1c0];
      }
#if !defined(_MSC_VER)
      GCC_ATTRIBUTE((packed))
#endif
      fat16;
#if defined(_MSC_VER) && (_MSC_VER>=1300)
    __declspec(align(1))
#endif
    struct {
        Bit32u sectors_per_fat;
        Bit16u flags;
        Bit8u  major, minor;
        Bit32u first_cluster_of_root_dir;
        Bit16u info_sector;
        Bit16u backup_boot_sector;
        Bit8u  reserved1[12];
        Bit8u  drive_number;
        Bit8u  reserved2;
        Bit8u  signature;
        Bit32u id;
        Bit8u  volume_label[11];
        Bit8u  fat_type[8];
        Bit8u  ignored[0x1a4];
    }
#if !defined(_MSC_VER)
    GCC_ATTRIBUTE((packed))
#endif
    fat32;
    } u;
    Bit8u magic[2];
}
#if !defined(_MSC_VER)
    GCC_ATTRIBUTE((packed))
#endif
bootsector_t;

typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
  struct partition_t {
    Bit8u     attributes; /* 0x80 = bootable */
    mbr_chs_t start_CHS;
    Bit8u     fs_type; /* 0x1 = FAT12, 0x6 = FAT16, 0xe = FAT16_LBA, 0xb = FAT32, 0xc = FAT32_LBA */
    mbr_chs_t end_CHS;
    Bit32u    start_sector_long;
    Bit32u    length_sector_long;
}
#if !defined(_MSC_VER)
    GCC_ATTRIBUTE((packed))
#endif
partition_t;

typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
  struct mbr_t {
    Bit8u       ignored[0x1b8];
    Bit32u      nt_id;
    Bit8u       ignored2[2];
    partition_t partition[4];
    Bit8u       magic[2];
}
#if !defined(_MSC_VER)
    GCC_ATTRIBUTE((packed))
#endif
mbr_t;

typedef
#if defined(_MSC_VER) && (_MSC_VER>=1300)
  __declspec(align(1))
#endif
  struct infosector_t {
    Bit32u signature1;
    Bit8u  ignored[0x1e0];
    Bit32u signature2;
    Bit32u free_clusters;
    Bit32u mra_cluster; // most recently allocated cluster
    Bit8u  reserved[14];
    Bit8u  magic[2];
}
#if !defined(_MSC_VER)
    GCC_ATTRIBUTE((packed))
#endif
infosector_t;

#if defined(_MSC_VER)
#pragma pack(pop)
#elif defined(__MWERKS__) && defined(macintosh)
#pragma options align=reset
#endif

vvfat_image_t::vvfat_image_t()
{
  first_sectors = new Bit8u[0xc000];
  memset(&first_sectors[0], 0, 0xc000);
}

vvfat_image_t::~vvfat_image_t()
{
  delete [] first_sectors;
}

bx_bool vvfat_image_t::sector2CHS(Bit32u spos, mbr_chs_t *chs)
{
  Bit32u head, sector;

  sector = spos % sectors;
  spos   /= sectors;
  head   = spos % heads;
  spos   /= heads;
  if (spos > 1023) {
    /* Overflow,
    it happens if 32bit sector positions are used, while CHS is only 24bit.
    Windows/Dos is said to take 1023/255/63 as nonrepresentable CHS */
    chs->head     = 0xff;
    chs->sector   = 0xff;
    chs->cylinder = 0xff;
    return 1;
  }
  chs->head     = (Bit8u)head;
  chs->sector   = (Bit8u)((sector+1) | ((spos >> 8) << 6));
  chs->cylinder = (Bit8u)spos;
  return 0;
}

void vvfat_image_t::init_mbr(void)
{
  mbr_t* real_mbr = (mbr_t*)first_sectors;
  partition_t* partition = &(real_mbr->partition[0]);
  bx_bool lba;

  // Win NT Disk Signature
  real_mbr->nt_id = htod32(0xbe1afdfa);

  partition->attributes = 0x80; // bootable

  // LBA is used when partition is outside the CHS geometry
  lba = sector2CHS(offset_to_bootsector, &partition->start_CHS);
  lba |= sector2CHS(sector_count - 1, &partition->end_CHS);

  // LBA partitions are identified only by start/length_sector_long not by CHS
  partition->start_sector_long = htod32(offset_to_bootsector);
  partition->length_sector_long = htod32(sector_count - offset_to_bootsector);

  /* FAT12/FAT16/FAT32 */
  /* DOS uses different types when partition is LBA,
     probably to prevent older versions from using CHS on them */
  partition->fs_type = fat_type==12 ? 0x1:
                       fat_type==16 ? (lba?0xe:0x06):
                       /*fat_tyoe==32*/ (lba?0xc:0x0b);

  real_mbr->magic[0] = 0x55;
  real_mbr->magic[1] = 0xaa;
}

// dest is assumed to hold 258 bytes, and pads with 0xffff up to next multiple of 26
static inline int short2long_name(char* dest,const char* src)
{
  int i;
  int len;

  for (i = 0; (i < 129) && src[i]; i++) {
    dest[2*i] = src[i];
    dest[2*i+1] = 0;
  }
  len = 2 * i;
  dest[2*i] = dest[2*i+1] = 0;
  for (i = 2 * i + 2; (i % 26); i++)
    dest[i] = (char)0xff;
  return len;
}

direntry_t* vvfat_image_t::create_long_filename(const char* filename)
{
  char buffer[258];
  int length = short2long_name(buffer, filename),
      number_of_entries = (length+25) / 26, i;
  direntry_t* entry;

  for (i = 0; i < number_of_entries; i++) {
    entry = (direntry_t*)array_get_next(&directory);
    entry->attributes = 0xf;
    entry->reserved[0] = 0;
    entry->begin = 0;
    entry->name[0] = (number_of_entries - i) | (i==0 ? 0x40:0);
  }
  for (i = 0; i < 26 * number_of_entries; i++) {
    int offset = (i % 26);
    if (offset < 10) offset = 1 + offset;
    else if (offset < 22) offset = 14 + offset - 10;
    else offset = 28 + offset - 22;
    entry = (direntry_t*)array_get(&directory, directory.next - 1 - (i / 26));
    entry->name[offset] = buffer[i];
  }
  return (direntry_t*)array_get(&directory, directory.next-number_of_entries);
}

static char is_long_name(const direntry_t* direntry)
{
    return direntry->attributes == 0xf;
}

static void set_begin_of_direntry(direntry_t* direntry, Bit32u begin)
{
  direntry->begin = htod16(begin & 0xffff);
  direntry->begin_hi = htod16((begin >> 16) & 0xffff);
}

static inline Bit8u fat_chksum(const direntry_t* entry)
{
  Bit8u chksum = 0;
  int i;

  for (i = 0; i < 11; i++) {
    unsigned char c;

    c = (i < 8) ? entry->name[i] : entry->extension[i-8];
    chksum = (((chksum & 0xfe) >> 1) | ((chksum & 0x01) ? 0x80:0)) + c;
  }

  return chksum;
}

// if return_time==0, this returns the fat_date, else the fat_time
#ifndef WIN32
static Bit16u fat_datetime(time_t time, int return_time)
{
  struct tm* t;
  struct tm t1;

  t = &t1;
  localtime_r(&time, t);
  if (return_time)
    return htod16((t->tm_sec/2) | (t->tm_min<<5) | (t->tm_hour<<11));
  return htod16((t->tm_mday) | ((t->tm_mon+1)<<5) | ((t->tm_year-80)<<9));
}
#else
static Bit16u fat_datetime(FILETIME time, int return_time)
{
  FILETIME localtime;
  SYSTEMTIME systime;

  FileTimeToLocalFileTime(&time, &localtime);
  FileTimeToSystemTime(&localtime, &systime);
  if (return_time)
    return htod16((systime.wSecond/2) | (systime.wMinute<<5) | (systime.wHour<<11));
  return htod16((systime.wDay) | (systime.wMonth<<5) | ((systime.wYear-1980)<<9));
}
#endif

void vvfat_image_t::fat_set(unsigned int cluster, Bit32u value)
{
  if (fat_type == 32) {
    Bit32u* entry = (Bit32u*)array_get(&fat, cluster);
    *entry = htod32(value);
  } else if (fat_type == 16) {
    Bit16u* entry = (Bit16u*)array_get(&fat, cluster);
    *entry = htod16(value & 0xffff);
  } else {
    int offset = (cluster * 3 / 2);
    Bit8u* p = (Bit8u*)array_get(&fat, offset);
    switch (cluster & 1) {
      case 0:
        p[0] = value & 0xff;
        p[1] = (p[1] & 0xf0) | ((value>>8) & 0xf);
        break;
      case 1:
        p[0] = (p[0]&0xf) | ((value&0xf)<<4);
        p[1] = (value>>4);
        break;
    }
  }
}

void vvfat_image_t::init_fat(void)
{
  if (fat_type == 12) {
    array_init(&fat, 1);
    array_ensure_allocated(&fat, sectors_per_fat * 0x200 * 3 / 2 - 1);
  } else {
    array_init(&fat, (fat_type==32) ? 4:2);
    array_ensure_allocated(&fat, sectors_per_fat * 0x200 / fat.item_size - 1);
  }
  memset(fat.pointer, 0, fat.size);

  switch (fat_type) {
    case 12: max_fat_value = 0xfff; break;
    case 16: max_fat_value = 0xffff; break;
    case 32: max_fat_value = 0x0fffffff; break;
    default: max_fat_value = 0; /* error... */
  }
}

// TODO: in create_short_filename, 0xe5->0x05 is not yet handled!
// TODO: in parse_short_filename, 0x05->0xe5 is not yet handled!
direntry_t* vvfat_image_t::create_short_and_long_name(
  unsigned int directory_start, const char* filename, int is_dot)
{
  int i, j, long_index = directory.next;
  direntry_t* entry = NULL;
  direntry_t* entry_long = NULL;

  if (is_dot) {
    entry = (direntry_t*)array_get_next(&directory);
    memset(entry->name,0x20,11);
    memcpy(entry->name,filename,strlen(filename));
    return entry;
  }

  entry_long = create_long_filename(filename);

  i = strlen(filename);
  for (j = i - 1; j>0  && filename[j]!='.';j--);
  if (j > 0)
    i = (j > 8 ? 8 : j);
  else if (i > 8)
    i = 8;

  entry = (direntry_t*)array_get_next(&directory);
  memset(entry->name, 0x20, 11);
  memcpy(entry->name, filename, i);

  if (j > 0)
    for (i = 0; i < 3 && filename[j+1+i]; i++)
      entry->extension[i] = filename[j+1+i];

  // upcase & remove unwanted characters
  for (i=10;i>=0;i--) {
    if (i==10 || i==7) for (;i>0 && entry->name[i]==' ';i--);
    if (entry->name[i]<=' ' || entry->name[i]>0x7f
      || strchr(".*?<>|\":/\\[];,+='",entry->name[i]))
      entry->name[i]='_';
    else if (entry->name[i]>='a' && entry->name[i]<='z')
      entry->name[i]+='A'-'a';
  }

  // mangle duplicates
  while (1) {
    direntry_t* entry1 = (direntry_t*)array_get(&directory, directory_start);
    int j;

    for (;entry1<entry;entry1++)
      if (!is_long_name(entry1) && !memcmp(entry1->name,entry->name,11))
        break; // found dupe
    if (entry1==entry) // no dupe found
      break;

    // use all 8 characters of name
    if (entry->name[7]==' ') {
      int j;
      for(j=6;j>0 && entry->name[j]==' ';j--)
        entry->name[j]='~';
    }

    // increment number
    for (j=7;j>0 && entry->name[j]=='9';j--)
      entry->name[j]='0';
    if (j > 0) {
      if (entry->name[j]<'0' || entry->name[j]>'9')
        entry->name[j]='0';
      else
        entry->name[j]++;
    }
  }

  // calculate checksum; propagate to long name
  if (entry_long) {
    Bit8u chksum = fat_chksum(entry);

    // calculate anew, because realloc could have taken place
    entry_long = (direntry_t*)array_get(&directory, long_index);
    while (entry_long<entry && is_long_name(entry_long)) {
      entry_long->reserved[1]=chksum;
      entry_long++;
    }
  }

  return entry;
}

/*
 * Read a directory. (the index of the corresponding mapping must be passed).
 */
int vvfat_image_t::read_directory(int mapping_index)
{
  mapping_t* mapping = (mapping_t*)array_get(&this->mapping, mapping_index);
  direntry_t* direntry;
  const char* dirname = mapping->path;
  int first_cluster = mapping->begin;
  int parent_index = mapping->info.dir.parent_mapping_index;
  mapping_t* parent_mapping = (mapping_t*)
      (parent_index >= 0 ? array_get(&this->mapping, parent_index) : NULL);
  int first_cluster_of_parent = parent_mapping ? (int)parent_mapping->begin : -1;
  int count = 0;

#ifndef WIN32
  DIR* dir = opendir(dirname);
  struct dirent* entry;
  int i;

  assert(mapping->mode & MODE_DIRECTORY);

  if (!dir) {
    mapping->end = mapping->begin;
    return -1;
  }

  i = mapping->info.dir.first_dir_index =
    first_cluster == 0 ? 0 : directory.next;

  // actually read the directory, and allocate the mappings
  while ((entry=readdir(dir))) {
    if ((first_cluster == 0) && (directory.next >= (Bit16u)(root_entries - 1))) {
      BX_ERROR(("Too many entries in root directory, using only %d", count));
      break;
    }
    unsigned int length = strlen(dirname) + 2 + strlen(entry->d_name);
    char* buffer;
    direntry_t* direntry;
    struct stat st;
    bx_bool is_dot = !strcmp(entry->d_name, ".");
    bx_bool is_dotdot = !strcmp(entry->d_name, "..");
    if (first_cluster == 0 && (is_dotdot || is_dot))
      continue;

    buffer = (char*)malloc(length);
    snprintf(buffer,length,"%s/%s",dirname,entry->d_name);

    if (stat(buffer, &st) < 0) {
      free(buffer);
      continue;
    }

    bx_bool is_mbr_file = !strcmp(entry->d_name, VVFAT_MBR);
    bx_bool is_boot_file = !strcmp(entry->d_name, VVFAT_BOOT);
    if (first_cluster == 0 && (is_mbr_file || is_boot_file) && (st.st_size == 512)) {
      free(buffer);
      continue;
    }

    count++;
    // create directory entry for this file
    direntry = create_short_and_long_name(i, entry->d_name, is_dot || is_dotdot);
    direntry->attributes = (S_ISDIR(st.st_mode) ? 0x10 : 0x20);
    direntry->reserved[0] = direntry->reserved[1]=0;
    direntry->ctime = fat_datetime(st.st_ctime, 1);
    direntry->cdate = fat_datetime(st.st_ctime, 0);
    direntry->adate = fat_datetime(st.st_atime, 0);
    direntry->begin_hi = 0;
    direntry->mtime = fat_datetime(st.st_mtime, 1);
    direntry->mdate = fat_datetime(st.st_mtime, 0);
    if (is_dotdot)
      set_begin_of_direntry(direntry, first_cluster_of_parent);
    else if (is_dot)
      set_begin_of_direntry(direntry, first_cluster);
    else
      direntry->begin = 0; // do that later
    if (st.st_size > 0x7fffffff) {
      BX_ERROR(("File %s is larger than 2GB", buffer));
      free(buffer);
      return -2;
    }
    direntry->size = htod32(S_ISDIR(st.st_mode) ? 0:st.st_size);

    // create mapping for this file
    if (!is_dot && !is_dotdot && (S_ISDIR(st.st_mode) || st.st_size)) {
      current_mapping = (mapping_t*)array_get_next(&this->mapping);
      current_mapping->begin = 0;
      current_mapping->end = st.st_size;
      /*
       * we get the direntry of the most recent direntry, which
       * contains the short name and all the relevant information.
       */
      current_mapping->dir_index = directory.next-1;
      current_mapping->first_mapping_index = -1;
      if (S_ISDIR(st.st_mode)) {
        current_mapping->mode = MODE_DIRECTORY;
        current_mapping->info.dir.parent_mapping_index =
          mapping_index;
      } else {
        current_mapping->mode = MODE_UNDEFINED;
        current_mapping->info.file.offset = 0;
      }
      current_mapping->path = buffer;
      current_mapping->read_only =
        (st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)) == 0;
    }
  }
  closedir(dir);
#else
  WIN32_FIND_DATA finddata;
  char filter[MAX_PATH];
  wsprintf(filter, "%s\\*.*", dirname);
  HANDLE hFind = FindFirstFile(filter, &finddata);
  int i;

  assert(mapping->mode & MODE_DIRECTORY);

  if (hFind == INVALID_HANDLE_VALUE) {
    mapping->end = mapping->begin;
    return -1;
  }

  i = mapping->info.dir.first_dir_index =
    first_cluster == 0 ? 0 : directory.next;

  // actually read the directory, and allocate the mappings
  do {
    if ((first_cluster == 0) && (directory.next >= (Bit16u)(root_entries - 1))) {
      BX_ERROR(("Too many entries in root directory, using only %d", count));
      break;
    }
    unsigned int length = lstrlen(dirname) + 2 + lstrlen(finddata.cFileName);
    char* buffer;
    direntry_t* direntry;
    bx_bool is_dot = !lstrcmp(finddata.cFileName, ".");
    bx_bool is_dotdot = !lstrcmp(finddata.cFileName, "..");
    if (first_cluster == 0 && (is_dotdot || is_dot))
      continue;
    bx_bool is_mbr_file = !lstrcmp(finddata.cFileName, VVFAT_MBR);
    bx_bool is_boot_file = !lstrcmp(finddata.cFileName, VVFAT_BOOT);
    if (first_cluster == 0 && (is_mbr_file || is_boot_file) && (finddata.nFileSizeLow == 512))
      continue;

    buffer = (char*)malloc(length);
    snprintf(buffer, length, "%s/%s", dirname, finddata.cFileName);

    count++;
    // create directory entry for this file
    direntry = create_short_and_long_name(i, finddata.cFileName, is_dot || is_dotdot);
    direntry->attributes = ((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 0x10 : 0x20);
    direntry->reserved[0] = direntry->reserved[1]=0;
    direntry->ctime = fat_datetime(finddata.ftCreationTime, 1);
    direntry->cdate = fat_datetime(finddata.ftCreationTime, 0);
    direntry->adate = fat_datetime(finddata.ftLastAccessTime, 0);
    direntry->begin_hi = 0;
    direntry->mtime = fat_datetime(finddata.ftLastWriteTime, 1);
    direntry->mdate = fat_datetime(finddata.ftLastWriteTime, 0);
    if (is_dotdot)
      set_begin_of_direntry(direntry, first_cluster_of_parent);
    else if (is_dot)
      set_begin_of_direntry(direntry, first_cluster);
    else
      direntry->begin = 0; // do that later
    if (finddata.nFileSizeLow > 0x7fffffff) {
      BX_ERROR(("File %s is larger than 2GB", buffer));
      free(buffer);
      return -2;
    }
    direntry->size = htod32((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 0:finddata.nFileSizeLow);

    // create mapping for this file
    if (!is_dot && !is_dotdot && ((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || finddata.nFileSizeLow)) {
      current_mapping = (mapping_t*)array_get_next(&this->mapping);
      current_mapping->begin = 0;
      current_mapping->end = finddata.nFileSizeLow;
      /*
       * we get the direntry of the most recent direntry, which
       * contains the short name and all the relevant information.
       */
      current_mapping->dir_index = directory.next-1;
      current_mapping->first_mapping_index = -1;
      if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        current_mapping->mode = MODE_DIRECTORY;
        current_mapping->info.dir.parent_mapping_index =
          mapping_index;
      } else {
        current_mapping->mode = MODE_UNDEFINED;
        current_mapping->info.file.offset = 0;
      }
      current_mapping->path=buffer;
      current_mapping->read_only = (finddata.dwFileAttributes & FILE_ATTRIBUTE_READONLY);
    }
  } while (FindNextFile(hFind, &finddata));
  FindClose(hFind);
#endif

  // fill with zeroes up to the end of the cluster
  while (directory.next % (0x10 * sectors_per_cluster)) {
    direntry_t* direntry = (direntry_t*)array_get_next(&directory);
    memset(direntry, 0, sizeof(direntry_t));
  }

  if (fat_type != 32) {
    if ((mapping_index == 0) && (directory.next < root_entries)) {
      // root directory
      int cur = directory.next;
      array_ensure_allocated(&directory, root_entries - 1);
      memset(array_get(&directory, cur), 0,
             (root_entries - cur) * sizeof(direntry_t));
    }
  }

  // reget the mapping, since s->mapping was possibly realloc()ed
  mapping = (mapping_t*)array_get(&this->mapping, mapping_index);
  if (first_cluster == 0) {
    first_cluster = 2;
  } else {
    first_cluster += (directory.next - mapping->info.dir.first_dir_index)
                     * 0x20 / cluster_size;
  }
  mapping->end = first_cluster;

  direntry = (direntry_t*)array_get(&directory, mapping->dir_index);
  set_begin_of_direntry(direntry, mapping->begin);

  return 0;
}

Bit32u vvfat_image_t::sector2cluster(off_t sector_num)
{
    return (Bit32u)((sector_num - offset_to_data) / sectors_per_cluster) + 2;
}

int vvfat_image_t::init_directories(const char* dirname)
{
  bootsector_t* bootsector;
  infosector_t* infosector;
  mapping_t* mapping;
  unsigned int i;
  unsigned int cluster;
  char size_txt[8];
  Bit32u volume_sector_count, tmpsc;

  cluster_size   = sectors_per_cluster * 0x200;
  cluster_buffer = new Bit8u[cluster_size];

  volume_sector_count = sector_count - offset_to_bootsector;
  tmpsc = volume_sector_count - reserved_sectors - root_entries / 16;
  cluster_count = (tmpsc * 0x200) / ((sectors_per_cluster * 0x200) + fat_type / 4);
  sectors_per_fat = ((cluster_count + 2) * fat_type / 8) / 0x200;
  sectors_per_fat += (((cluster_count + 2) * fat_type / 8) % 0x200) > 0;

  offset_to_fat = offset_to_bootsector + reserved_sectors;
  offset_to_root_dir = offset_to_fat + sectors_per_fat * 2;
  offset_to_data = offset_to_root_dir + root_entries / 16;

  array_init(&this->mapping, sizeof(mapping_t));
  array_init(&directory, sizeof(direntry_t));

  /* add volume label */
  {
    direntry_t *entry = (direntry_t*)array_get_next(&directory);
    entry->attributes = 0x28; // archive | volume label
    entry->mdate = 0x3d81; // 01.12.2010
    entry->mtime = 0x6000; // 12:00:00
    memcpy(entry->name, "BOCHS VV", 8);
    memcpy(entry->extension, "FAT", 3);
  }

  // Now build FAT, and write back information into directory
  init_fat();

  mapping = (mapping_t*)array_get_next(&this->mapping);
  mapping->begin = 0;
  mapping->dir_index = 0;
  mapping->info.dir.parent_mapping_index = -1;
  mapping->first_mapping_index = -1;
  mapping->path = strdup(dirname);
  i = strlen(mapping->path);
  if (i > 0 && mapping->path[i - 1] == '/')
    mapping->path[i - 1] = '\0';
  mapping->mode = MODE_DIRECTORY;
  mapping->read_only = 0;
  path = mapping->path;

  for (i = 0, cluster = 0; i < this->mapping.next; i++) {
    /* MS-DOS expects the FAT to be 0 for the root directory
     * (except for the media byte). */
    /* LATER TODO: still true for FAT32? */
    int fix_fat = (i != 0);
    mapping = (mapping_t*)array_get(&this->mapping, i);

    if (mapping->mode & MODE_DIRECTORY) {
      mapping->begin = cluster;
      if (read_directory(i)) {
        BX_ERROR(("Could not read directory %s", mapping->path));
        return -1;
      }
      mapping = (mapping_t*)array_get(&this->mapping, i);
    } else {
      assert(mapping->mode == MODE_UNDEFINED);
      mapping->mode = MODE_NORMAL;
      mapping->begin = cluster;
      if (mapping->end > 0) {
        direntry_t* direntry = (direntry_t*)array_get(&directory, mapping->dir_index);

        mapping->end = cluster + 1 + (mapping->end-1) / cluster_size;
        set_begin_of_direntry(direntry, mapping->begin);
      } else {
        mapping->end = cluster + 1;
        fix_fat = 0;
      }
    }

    assert(mapping->begin < mapping->end);

    /* next free cluster */
    cluster = mapping->end;

    if (cluster >= (cluster_count + 2)) {
      sprintf(size_txt, "%dMB", (sector_count >> 11));
      BX_ERROR(("Directory does not fit in FAT%d (capacity %s)",
                fat_type,
                (fat_type == 12) ? (sector_count == 2880) ? "1.44 MB":"2.88 MB"
                : size_txt));
      return -EINVAL;
    }

    // fix fat for entry
    if (fix_fat) {
      int j;
      for (j = mapping->begin; j < (int)(mapping->end - 1); j++)
        fat_set(j, j + 1);
      fat_set(mapping->end - 1, max_fat_value);
    }
  }

  mapping = (mapping_t*)array_get(&this->mapping, 0);
  assert((fat_type == 32) || (mapping->end == 2));

  // the FAT signature
  fat_set(0, max_fat_value);
  fat_set(1, max_fat_value);

  current_mapping = NULL;

  bootsector = (bootsector_t*)(first_sectors + offset_to_bootsector * 0x200);
  bootsector->jump[0] = 0xeb;
  if (fat_type != 32) {
    bootsector->jump[1] = 0x3e;
  } else {
    bootsector->jump[1] = 0x58;
  }
  bootsector->jump[2] = 0x90;
  memcpy(bootsector->name,"BOCHS   ", 8);
  bootsector->sector_size = htod16(0x200);
  bootsector->sectors_per_cluster = sectors_per_cluster;
  bootsector->reserved_sectors = htod16(reserved_sectors);
  bootsector->number_of_fats = 0x2;
  if (fat_type != 32) {
    bootsector->root_entries = htod16(root_entries);
  }
  bootsector->total_sectors16 = (volume_sector_count > 0xffff) ? 0:htod16(volume_sector_count);
  bootsector->media_type = ((fat_type != 12) ? 0xf8:0xf0);
  fat.pointer[0] = bootsector->media_type;
  if (fat_type != 32) {
    bootsector->sectors_per_fat = htod16(sectors_per_fat);
  }
  bootsector->sectors_per_track = htod16(sectors);
  bootsector->number_of_heads = htod16(heads);
  bootsector->hidden_sectors = htod32(offset_to_bootsector);
  bootsector->total_sectors = htod32((volume_sector_count > 0xffff) ? volume_sector_count:0);

  if (fat_type != 32) {
    bootsector->u.fat16.drive_number = (fat_type == 12) ? 0:0x80; // assume this is hda (TODO)
    bootsector->u.fat16.signature = 0x29;
    bootsector->u.fat16.id = htod32(0xfabe1afd);
    memcpy(bootsector->u.fat16.volume_label, "BOCHS VVFAT", 11);
    memcpy(bootsector->u.fat16.fat_type, (fat_type==12) ? "FAT12   ":"FAT16   ", 8);
  } else {
    bootsector->u.fat32.sectors_per_fat = htod32(sectors_per_fat);
    bootsector->u.fat32.first_cluster_of_root_dir = first_cluster_of_root_dir;
    bootsector->u.fat32.info_sector = htod16(1);
    bootsector->u.fat32.backup_boot_sector = htod16(6);
    bootsector->u.fat32.drive_number = 0x80; // assume this is hda (TODO)
    bootsector->u.fat32.signature = 0x29;
    bootsector->u.fat32.id = htod32(0xfabe1afd);
    memcpy(bootsector->u.fat32.volume_label, "BOCHS VVFAT", 11);
    memcpy(bootsector->u.fat32.fat_type, "FAT32   ", 8);
  }

  bootsector->magic[0] = 0x55;
  bootsector->magic[1] = 0xaa;

  if (fat_type == 32) {
    memcpy(&first_sectors[(offset_to_bootsector + 6) * 0x200], &first_sectors[offset_to_bootsector * 0x200], 0x200);
    infosector = (infosector_t*)(first_sectors + (offset_to_bootsector + 1) * 0x200);
    infosector->signature1 = htod32(0x41615252);
    infosector->signature2 = htod32(0x61417272);
    infosector->free_clusters = htod32(cluster_count - cluster - 2);
    infosector->mra_cluster = htod32(2);
    infosector->magic[0] = 0x55;
    infosector->magic[1] = 0xaa;
  }

  return 0;
}

int vvfat_image_t::open(const char* dirname)
{
  Bit32u size_in_mb;

  // TODO: read MBR file (if present) and use it's values
  if (cylinders == 0) {
    cylinders = 1024;
    heads = 16;
    sectors = 63;
  }
  offset_to_bootsector = sectors;
  sector_count = cylinders * heads * sectors;
  hd_size = sector_count * 512;
  size_in_mb = (Bit32u)(hd_size >> 20);
  if (size_in_mb >= 2047) {
    fat_type = 32;
    sectors_per_cluster = 8;
    first_cluster_of_root_dir = 2;
    root_entries = 0;
    reserved_sectors = 32;
    BX_PANIC(("FAT32 required, but not supported yet"));
  } else {
    fat_type = 16;
    if (size_in_mb >= 1023) {
      sectors_per_cluster = 64;
    } else if (size_in_mb >= 511) {
      sectors_per_cluster = 32;
    } else if (size_in_mb >= 255) {
      sectors_per_cluster = 16;
    } else {
      sectors_per_cluster = 8;
    }
    first_cluster_of_root_dir = 0;
    root_entries = 512;
    reserved_sectors = 1;
  }

  current_cluster = 0xffff;
  current_fd = 0;

  if (offset_to_bootsector > 0)
    init_mbr();

  init_directories(dirname);

  return 0;
}

void vvfat_image_t::close(void)
{
  array_free(&fat);
  array_free(&directory);
  array_free(&mapping);
  if (cluster_buffer != NULL)
    delete [] cluster_buffer;
}

Bit64s vvfat_image_t::lseek(Bit64s offset, int whence)
{
  if (whence == SEEK_SET) {
    sector_num = (Bit32u)(offset / 512);
  } else if (whence == SEEK_CUR) {
    sector_num += (Bit32u)(offset / 512);
  } else {
    BX_ERROR(("lseek: mode not supported yet"));
    return -1;
  }
  if (sector_num >= sector_count)
    return -1;
  return 0;
}

void vvfat_image_t::close_current_file(void)
{
  if(current_mapping) {
    current_mapping = NULL;
    if (current_fd) {
      ::close(current_fd);
      current_fd = 0;
    }
  }
  current_cluster = 0xffff;
}

/* mappings between index1 and index2-1 are supposed to be ordered
 * return value is the index of the last mapping for which end>cluster_num
 */
int vvfat_image_t::find_mapping_for_cluster_aux(int cluster_num, int index1, int index2)
{
  while(1) {
    int index3;
    mapping_t* mapping;
    index3 = (index1+index2) / 2;
    mapping = (mapping_t*)array_get(&this->mapping, index3);
    assert(mapping->begin < mapping->end);
    if (mapping->begin >= (unsigned int)cluster_num) {
      assert(index2 != index3 || index2 == 0);
      if (index2 == index3)
        return index1;
      index2 = index3;
    } else {
      if (index1 == index3)
        return (mapping->end <= (unsigned int)cluster_num) ? index2 : index1;
      index1 = index3;
    }
    assert(index1 <= index2);
  }
}

mapping_t* vvfat_image_t::find_mapping_for_cluster(int cluster_num)
{
  int index = find_mapping_for_cluster_aux(cluster_num, 0, mapping.next);
  mapping_t* mapping;
  if (index >= (int)this->mapping.next)
    return NULL;
  mapping = (mapping_t*)array_get(&this->mapping, index);
  if ((int)mapping->begin > cluster_num)
    return NULL;
  assert(((int)mapping->begin <= cluster_num) && ((int)mapping->end > cluster_num));
  return mapping;
}

int vvfat_image_t::open_file(mapping_t* mapping)
{
  if (!mapping)
    return -1;
  if (!current_mapping ||
    strcmp(current_mapping->path, mapping->path)) {
    /* open file */
    int fd = ::open(mapping->path, O_RDONLY
#ifdef O_BINARY
                    | O_BINARY
#endif
#ifdef O_LARGEFILE
                    | O_LARGEFILE
#endif
                    );
    if (fd < 0)
      return -1;
    close_current_file();
    current_fd = fd;
    current_mapping = mapping;
  }
  return 0;
}

int vvfat_image_t::read_cluster(int cluster_num)
{
  mapping_t* mapping;

  if (current_cluster != cluster_num) {
    int result=0;
    off_t offset;
    assert(!current_mapping || current_fd || (current_mapping->mode & MODE_DIRECTORY));
    if (!current_mapping
        || ((int)current_mapping->begin > cluster_num)
        || ((int)current_mapping->end <= cluster_num)) {
      // binary search of mappings for file
      mapping = find_mapping_for_cluster(cluster_num);

      assert(!mapping || ((cluster_num >= (int)mapping->begin) && (cluster_num < (int)mapping->end)));

      if (mapping && mapping->mode & MODE_DIRECTORY) {
        close_current_file();
        current_mapping = mapping;
read_cluster_directory:
        offset = cluster_size * (cluster_num - current_mapping->begin);
        cluster = (unsigned char*)directory.pointer+offset
                     + 0x20 * current_mapping->info.dir.first_dir_index;
        assert(((cluster -(unsigned char*)directory.pointer) % cluster_size) == 0);
        assert((char*)cluster + cluster_size <= directory.pointer + directory.next * directory.item_size);
        current_cluster = cluster_num;
        return 0;
      }

      if (open_file(mapping))
        return -2;
    } else if (current_mapping->mode & MODE_DIRECTORY)
      goto read_cluster_directory;

    assert(current_fd);

    offset = cluster_size * (cluster_num - current_mapping->begin) + current_mapping->info.file.offset;
    if (::lseek(current_fd, offset, SEEK_SET) != offset)
      return -3;
    cluster = cluster_buffer;
    result = ::read(current_fd, cluster, cluster_size);
    if (result < 0) {
      current_cluster = 0xffff;
      return -1;
    }
    current_cluster = cluster_num;
  }
  return 0;
}

ssize_t vvfat_image_t::read(void* buf, size_t count)
{
  char *cbuf = (char*)buf;
  Bit32u scount = (Bit32u)(count / 512);

  while (scount-- > 0) {
    if (sector_num < offset_to_data) {
      if (sector_num < (offset_to_bootsector + reserved_sectors))
        memcpy(cbuf, &first_sectors[sector_num * 0x200], 0x200);
      else if ((sector_num - offset_to_fat) < sectors_per_fat)
        memcpy(cbuf, &fat.pointer[(sector_num - offset_to_fat) * 0x200], 0x200);
      else if ((sector_num - offset_to_fat - sectors_per_fat) < sectors_per_fat)
        memcpy(cbuf, &fat.pointer[(sector_num - offset_to_fat - sectors_per_fat) * 0x200], 0x200);
      else
        memcpy(cbuf, &directory.pointer[(sector_num - offset_to_root_dir) * 0x200], 0x200);
    } else {
      Bit32u sector = sector_num - offset_to_data,
      sector_offset_in_cluster = (sector % sectors_per_cluster),
      cluster_num = sector / sectors_per_cluster + 2;
      if (read_cluster(cluster_num) != 0) {
        memset(cbuf, 0, 0x200);
      } else {
        memcpy(cbuf, cluster + sector_offset_in_cluster * 0x200, 0x200);
      }
    }
    sector_num++;
    cbuf += 0x200;
  }

  return count;
}

ssize_t vvfat_image_t::write(const void* buf, size_t count)
{
  Bit32u scount = (Bit32u)(count / 512);

  if (sector_num < (offset_to_bootsector + 1))
    return -1;

  BX_ERROR(("VVFAT write not supported yet: sector=%d, count=%d", sector_num, scount));
  return -1;
}

Bit32u vvfat_image_t::get_capabilities(void)
{
  return HDIMAGE_READONLY | HDIMAGE_HAS_GEOMETRY;
}
