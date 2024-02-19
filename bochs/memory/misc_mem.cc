/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2024  The Bochs Project
//
//  I/O memory handlers API Copyright (C) 2003 by Frank Cornelis
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
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "param_names.h"
#include "cpu/cpu.h"
#include "iodev/iodev.h"
#define LOG_THIS BX_MEM(0)->

#define BX_MEM_HANDLERS   ((BX_CONST64(1) << BX_PHY_ADDRESS_WIDTH) >> 20) /* one per megabyte */

#define FLASH_READ_ARRAY  0xff
#define FLASH_INT_ID      0x90
#define FLASH_READ_STATUS 0x70
#define FLASH_CLR_STATUS  0x50
#define FLASH_ERASE_SETUP 0x20
#define FLASH_ERASE_SUSP  0xb0
#define FLASH_PROG_SETUP  0x40
#define FLASH_ERASE       0xd0

BX_MEM_C::BX_MEM_C() : BX_MEMORY_STUB_C()
{
  memory_handlers = NULL;
}

BX_MEM_C::~BX_MEM_C()
{
  cleanup_memory();
}

void BX_MEM_C::init_memory(Bit64u guest, Bit64u host, Bit32u block_size)
{
  unsigned idx, i;

  BX_MEMORY_STUB_C::init_memory(guest, host, block_size);

  BX_MEM_THIS smram_available = false;
  BX_MEM_THIS smram_enable = false;
  BX_MEM_THIS smram_restricted = false;

  BX_MEM_THIS memory_handlers = new struct memory_handler_struct *[BX_MEM_HANDLERS];
  for (idx = 0; idx < BX_MEM_HANDLERS; idx++)
    BX_MEM_THIS memory_handlers[idx] = NULL;

  BX_MEM_THIS pci_enabled = SIM->get_param_bool(BXPN_PCI_ENABLED)->get();
  BX_MEM_THIS bios_write_enabled = false;
  BX_MEM_THIS bios_rom_addr = 0xffff0000;
  BX_MEM_THIS flash_type = 0;
  BX_MEM_THIS flash_status = 0x80;
  BX_MEM_THIS flash_wsm_state = FLASH_READ_ARRAY;
  BX_MEM_THIS flash_modified = false;

  for (i = 0; i < 65; i++)
    BX_MEM_THIS rom_present[i] = false;
  for (i = 0; i <= BX_MEM_AREA_F0000; i++) {
    BX_MEM_THIS memory_type[i][0] = false;
    BX_MEM_THIS memory_type[i][1] = false;
  }

  BX_MEM_THIS register_state();
}

#if BX_LARGE_RAMFILE
// The blocks in RAM must also be flushed to the save file.
void ramfile_save_handler(void *devptr, FILE *fp)
{
  for (Bit32u idx = 0; idx < (BX_MEM(0)->len / BX_MEM_THIS block_size); idx++) {
    if ((BX_MEM(0)->blocks[idx]) && (BX_MEM(0)->blocks[idx] != BX_MEM(0)->swapped_out))
    {
      bx_phy_address address = bx_phy_address(idx) * BX_MEM_THIS block_size;
      if (fseeko64(fp, address, SEEK_SET))
        BX_PANIC(("FATAL ERROR: Could not seek to 0x" FMT_PHY_ADDRX " in overflow file!", address));
      if (1 != fwrite (BX_MEM(0)->blocks[idx], BX_MEM_THIS block_size, 1, fp))
        BX_PANIC(("FATAL ERROR: Could not write at 0x" FMT_PHY_ADDRX " in overflow file!", address));
    }
  }
}
#endif

// Note: This must be called before the memory file save handler is called.
Bit64s memory_param_save_handler(void *devptr, bx_param_c *param)
{
  char imgname[BX_PATHNAME_LEN];
  char path[BX_PATHNAME_LEN+1];
  const char *pname = param->get_name();

  if (! strncmp(pname, "blk", 3)) {
    Bit32u blk_index = atoi(pname + 3);
    if (! BX_MEM(0)->blocks[blk_index])
      return -1;
#if BX_LARGE_RAMFILE
    // If swapped out, will be saved by common handler.
    if (BX_MEM(0)->blocks[blk_index] == BX_MEM(0)->swapped_out)
      return -2;
#endif
    // Return the block offset into the array
    Bit32u val = (Bit32u) (BX_MEM(0)->blocks[blk_index] - BX_MEM(0)->vector);
    if ((val & (BX_MEM_THIS block_size-1)) == 0)
       return val / BX_MEM_THIS block_size;
  } else if (!strcmp(pname, "flash_data")) {
    bool ret = false;
    if (BX_MEM_THIS flash_modified) {
      param->get_param_path(imgname, BX_PATHNAME_LEN);
      if (!strncmp(imgname, "bochs.", 6)) {
        strcpy(imgname, imgname+6);
      }
      if (SIM->get_param_string(BXPN_RESTORE_PATH)->isempty()) {
        return 0;
      }
      sprintf(path, "%s/%s", SIM->get_param_string(BXPN_RESTORE_PATH)->getptr(), imgname);
      ret = BX_MEM_THIS save_flash_data(path);
    }
    return ret;
  }
  return -1;
}

void memory_param_restore_handler(void *devptr, bx_param_c *param, Bit64s val)
{
  char imgname[BX_PATHNAME_LEN];
  char path[BX_PATHNAME_LEN+1];
  const char *pname = param->get_name();

  if (! strncmp(pname, "blk", 3)) {
    Bit32u blk_index = atoi(pname + 3);
#if BX_LARGE_RAMFILE
    if ((Bit32s) val == -2) {
      BX_MEM(0)->blocks[blk_index] = BX_MEM(0)->swapped_out;
      return;
    }
#endif
     if((Bit32s) val < 0) {
        BX_MEM(0)->blocks[blk_index] = NULL;
        return;
      }
      BX_MEM(0)->blocks[blk_index] = BX_MEM(0)->vector + val * BX_MEM_THIS block_size;
#if BX_LARGE_RAMFILE
      BX_MEM(0)->read_block(blk_index);
#endif
  } else if (!strcmp(pname, "flash_data")) {
    if (BX_MEM_THIS flash_modified && val) {
      param->get_param_path(imgname, BX_PATHNAME_LEN);
      if (!strncmp(imgname, "bochs.", 6)) {
        strcpy(imgname, imgname+6);
      }
      if (SIM->get_param_string(BXPN_RESTORE_PATH)->isempty()) {
        return;
      }
      sprintf(path, "%s/%s", SIM->get_param_string(BXPN_RESTORE_PATH)->getptr(), imgname);
      BX_MEM_THIS load_flash_data(path);
    }
  }
}

void BX_MEM_C::register_state()
{
  char param_name[15];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "memory", "Memory State");
  Bit32u num_blocks = (Bit32u)(BX_MEM_THIS len / BX_MEM_THIS block_size);
#if BX_LARGE_RAMFILE
  bx_shadow_filedata_c *ramfile = new bx_shadow_filedata_c(list, "ram", &(BX_MEM_THIS overflow_file));
  ramfile->set_sr_handlers(this, ramfile_save_handler, (filedata_restore_handler)NULL);
  BXRS_DEC_PARAM_FIELD(list, next_swapout_idx, BX_MEM_THIS next_swapout_idx);
#else
  new bx_shadow_data_c(list, "ram", BX_MEM_THIS vector, BX_MEM_THIS allocated);
#endif
  BXRS_DEC_PARAM_FIELD(list, used_blocks, BX_MEM_THIS used_blocks);

  bx_list_c *mapping = new bx_list_c(list, "mapping");
  for (Bit32u blk=0; blk < num_blocks; blk++) {
    sprintf(param_name, "blk%d", blk);
    bx_param_num_c *param = new bx_param_num_c(mapping, param_name, "", "", -2, BX_MAX_BIT32U, 0);
    param->set_base(BASE_DEC);
    param->set_sr_handlers(this, memory_param_save_handler, memory_param_restore_handler);
  }
  bx_list_c *memtype = new bx_list_c(list, "memtype");
  for (int i = 0; i <= BX_MEM_AREA_F0000; i++) {
    sprintf(param_name, "%d_r", i);
    new bx_shadow_bool_c(memtype, param_name, &BX_MEM_THIS memory_type[i][0]);
    sprintf(param_name, "%d_w", i);
    new bx_shadow_bool_c(memtype, param_name, &BX_MEM_THIS memory_type[i][1]);
  }
  BXRS_HEX_PARAM_FIELD(list, flash_status, BX_MEM_THIS flash_status);
  BXRS_DEC_PARAM_FIELD(list, flash_wsm_state, BX_MEM_THIS flash_wsm_state);
  BXRS_PARAM_BOOL(list, flash_modified, BX_MEM_THIS flash_modified);
  bx_param_bool_c *flash_data = new bx_param_bool_c(list, "flash_data", "", "", false);
  flash_data->set_sr_handlers(this, memory_param_save_handler, memory_param_restore_handler);
}

void BX_MEM_C::cleanup_memory()
{
  if (BX_MEM_THIS flash_modified) {
    bx_param_string_c *flash_data = SIM->get_param_string(BXPN_ROM_FLASH_DATA);
    if (!flash_data->isempty()) {
      BX_MEM_THIS save_flash_data(flash_data->getptr());
    }
    BX_MEM_THIS flash_modified = false;
  }

  BX_MEMORY_STUB_C::cleanup_memory();

  if (BX_MEM_THIS memory_handlers != NULL) {
    for (unsigned idx = 0; idx < BX_MEM_HANDLERS; idx++) {
      struct memory_handler_struct *memory_handler = BX_MEM_THIS memory_handlers[idx];
      struct memory_handler_struct *prev = NULL;
      while (memory_handler) {
        prev = memory_handler;
        memory_handler = memory_handler->next;
        delete prev;
      }
    }
    delete [] BX_MEM_THIS memory_handlers;
    BX_MEM_THIS memory_handlers = NULL;
  }
}

bool BX_MEM_C::load_flash_data(const char *path)
{
  int size = 0, offset = 0;

  int fd = open(path, O_RDONLY
#ifdef O_BINARY
                | O_BINARY
#endif
                );
  if (fd >= 0) {
    if (BX_MEM_THIS flash_type == 2) {
      offset = 0x8000; // 28F002BC-T
      size = 0x4000;
    } else if (BX_MEM_THIS flash_type == 1) {
      offset = 0x4000; // 28F001BX-T
      size = 0x2000;
    }
    if (size > 0) {
      read(fd, &BX_MEM_THIS rom[BIOSROMSZ - offset], size);
    }
    close(fd);
  }
  return (fd >= 0);
}

bool BX_MEM_C::save_flash_data(const char *path)
{
  int offset = 0, size = 0;

  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC
#ifdef O_BINARY
                | O_BINARY
#endif
                , S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP
                );
  if (fd >= 0) {
    if (BX_MEM_THIS flash_type == 2) {
      offset = 0x8000; // 28F002BC-T
      size = 0x4000;
    } else if (BX_MEM_THIS flash_type == 1) {
      offset = 0x4000; // 28F001BX-T
      size = 0x2000;
    }
    if (size > 0) {
      write(fd, &BX_MEM_THIS rom[BIOSROMSZ - offset], size);
    }
    close(fd);
  }
  return (fd >= 0);
}

//
// Values for type:
//   0 : System Bios
//   1 : VGA Bios
//   2 : Optional ROM Bios
//
void BX_MEM_C::load_ROM(const char *path, bx_phy_address romaddress, Bit8u type)
{
  struct stat stat_buf;
  int fd, ret, i, start_idx, end_idx;
  unsigned long size, max_size, offset;
  bool is_bochs_bios = false;

  if (*path == '\0') {
    if (type == 2) {
      BX_PANIC(("ROM: Optional ROM image undefined"));
    }
    else if (type == 1) {
      BX_PANIC(("ROM: VGA BIOS image undefined"));
    }
    else {
      BX_PANIC(("ROM: System BIOS image undefined"));
    }
    return;
  }
  // read in ROM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
                | O_BINARY
#endif
           );
  if (fd < 0) {
    if (type < 2) {
      BX_PANIC(("ROM: couldn't open ROM image file '%s'.", path));
    }
    else {
      BX_ERROR(("ROM: couldn't open ROM image file '%s'.", path));
    }
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    if (type < 2) {
      close(fd);
      BX_PANIC(("ROM: couldn't stat ROM image file '%s'.", path));
    }
    else {
      close(fd);
      BX_ERROR(("ROM: couldn't stat ROM image file '%s'.", path));
    }
    return;
  }

  size = (unsigned long)stat_buf.st_size;

  if (type > 0) {
    max_size = 0x20000;
  } else {
    max_size = BIOSROMSZ;
  }
  if (size > max_size) {
    close(fd);
    BX_PANIC(("ROM: ROM image too large"));
    return;
  }
  if (type == 0) {
    if (romaddress > 0) {
      if ((romaddress + size) != 0x100000 && (romaddress + size)) {
        close(fd);
        BX_PANIC(("ROM: System BIOS must end at 0xfffff"));
        return;
      }
    } else {
      romaddress = ~(size - 1);
    }
    offset = romaddress & BIOS_MASK;
    if ((romaddress & 0xf0000) < 0xf0000) {
      BX_MEM_THIS rom_present[64] = true;
    }
    BX_MEM_THIS bios_rom_addr = (Bit32u)romaddress;
    is_bochs_bios = ((strstr(path, "BIOS-bochs-latest") != NULL) ||
                     (strstr(path, "BIOS-bochs-legacy") != NULL));
    if (size == 0x40000) {
      BX_MEM_THIS flash_type = 2; // 28F002BC-T
    } else if (size == 0x20000) {
      BX_MEM_THIS flash_type = 1; // 28F001BX-T
    }
  } else {
    if ((size % 512) != 0) {
      close(fd);
      BX_PANIC(("ROM: ROM image size must be multiple of 512 (size = %ld)", size));
      return;
    }
    if ((romaddress % 2048) != 0) {
      close(fd);
      BX_PANIC(("ROM: ROM image must start at a 2k boundary"));
      return;
    }
    if ((romaddress < 0xc0000) ||
        (((romaddress + size - 1) > 0xdffff) && (romaddress < 0xe0000))) {
      close(fd);
      BX_PANIC(("ROM: ROM address space out of range"));
      return;
    }
    if (romaddress < 0xe0000) {
      offset = (romaddress & EXROM_MASK) + BIOSROMSZ;
      start_idx = (((Bit32u)romaddress - 0xc0000) >> 11);
      end_idx = start_idx + (size >> 11) + (((size % 2048) > 0) ? 1 : 0);
    } else {
      offset = romaddress & BIOS_MASK;
      start_idx = 64;
      end_idx = 64;
    }
    for (i = start_idx; i < end_idx; i++) {
      if (BX_MEM_THIS rom_present[i]) {
        close(fd);
        BX_PANIC(("ROM: address space 0x%x already in use", (i * 2048) + 0xc0000));
        return;
      } else {
        BX_MEM_THIS rom_present[i] = true;
      }
    }
  }
  while (size > 0) {
    ret = read(fd, (bx_ptr_t) &BX_MEM_THIS rom[offset], size);
    if (ret <= 0) {
      BX_PANIC(("ROM: read failed on BIOS image: '%s'",path));
    }
    size -= ret;
    offset += ret;
  }
  close(fd);
  offset -= (unsigned long)stat_buf.st_size;
  size = (unsigned long)stat_buf.st_size;
  if (is_bochs_bios ||
      ((BX_MEM_THIS rom[offset] == 0x55) && (BX_MEM_THIS rom[offset+1] == 0xaa))) {
    if ((type == 0) && ((romaddress & 0xfffff) == 0xe0000)) {
      offset += 0x10000;
      size = 0x10000;
    }
    Bit8u checksum = 0;
    for (i = 0; i < (int)size; i++) {
      checksum += BX_MEM_THIS rom[offset + i];
    }
    if (checksum != 0) {
      if (type == 1) {
        BX_PANIC(("ROM: checksum error in VGABIOS image: '%s'", path));
      } else if (is_bochs_bios) {
        BX_ERROR(("ROM: checksum error in BIOS image: '%s'", path));
      }
    }
  }
  BX_INFO(("rom at 0x%05x/%u ('%s')",
                        (unsigned) romaddress,
                        (unsigned) stat_buf.st_size,
                         path));
  if ((type == 0) && !is_bochs_bios && (BX_MEM_THIS flash_type > 0)) {
    bx_param_string_c *flash_data = SIM->get_param_string(BXPN_ROM_FLASH_DATA);
    if (!flash_data->isempty()) {
      if (!BX_MEM_THIS load_flash_data(flash_data->getptr())) {
        Bit32u foffset = 0, fsize = 0;
        if (BX_MEM_THIS flash_type == 2) {
          foffset = 0x8000; // 28F002BC-T
          fsize = 0x4000;
        } else if (BX_MEM_THIS flash_type == 1) {
          foffset = 0x4000; // 28F001BX-T
          fsize = 0x2000;
        }
        if (fsize > 0) {
          for (i = 0; i < (int)fsize; i++) {
            BX_MEM_THIS rom[BIOSROMSZ - foffset + i] = 0xff;
          }
          BX_MEM_THIS flash_modified = true;
        }
      }
    }
  }
}

void BX_MEM_C::load_RAM(const char *path, bx_phy_address ramaddress)
{
  struct stat stat_buf;
  int fd, ret;
  unsigned long size, offset;

  if (*path == '\0') {
    BX_PANIC(("RAM: Optional RAM image undefined"));
    return;
  }
  // read in RAM BIOS image file
  fd = open(path, O_RDONLY
#ifdef O_BINARY
            | O_BINARY
#endif
           );
  if (fd < 0) {
    BX_PANIC(("RAM: couldn't open RAM image file '%s'.", path));
    return;
  }
  ret = fstat(fd, &stat_buf);
  if (ret) {
    close(fd);
    BX_PANIC(("RAM: couldn't stat RAM image file '%s'.", path));
    return;
  }

  size = (unsigned long)stat_buf.st_size;

  offset = (unsigned long)ramaddress;
  while (size > 0) {
    ret = read(fd, (bx_ptr_t) BX_MEM_THIS get_vector(offset), size);
    if (ret <= 0) {
      BX_PANIC(("RAM: read failed on RAM image: '%s'",path));
    }
    size -= ret;
    offset += ret;
  }
  close(fd);
  BX_INFO(("ram at 0x%05x/%u ('%s')",
                        (unsigned) ramaddress,
                        (unsigned) stat_buf.st_size,
                         path));
}

bool BX_MEM_C::dbg_fetch_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf)
{
  bx_phy_address a20addr = A20ADDR(addr);
  struct memory_handler_struct *memory_handler = NULL;
  bool ret = true, use_memory_handler = false, use_smram = false;

  bool is_bios = (a20addr >= (bx_phy_address)BX_MEM_THIS bios_rom_addr);
#if BX_PHY_ADDRESS_LONG
  if (a20addr > BX_CONST64(0xffffffff)) is_bios = false;
#endif

  if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000) && BX_MEM_THIS smram_available)
  {
    // SMRAM memory space
    if (BX_MEM_THIS smram_enable || (cpu->smm_mode() && !BX_MEM_THIS smram_restricted))
      use_smram = true;
  }

  memory_handler = BX_MEM_THIS memory_handlers[a20addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20addr && memory_handler->end >= a20addr)
    {
      if (!use_smram) {
        use_memory_handler = true;
        break;
      }
    }
    memory_handler = memory_handler->next;
  }

  for (; len>0; len--) {
    if (use_memory_handler) {
      memory_handler->read_handler(a20addr, 1, buf, memory_handler->param);
    }
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && (a20addr >= 0x000c0000 && a20addr < 0x00100000)) {
      unsigned area = (unsigned)(a20addr >> 14) & 0x0f;
      if (area > BX_MEM_AREA_F0000) area = BX_MEM_AREA_F0000;
      if (BX_MEM_THIS memory_type[area][0] == false) {
        // Read from ROM
        if ((a20addr & 0xfffe0000) == 0x000e0000) {
          // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
          if (BX_MEM_THIS flash_type > 0) {
            *buf = BX_MEM_THIS flash_read(BIOS_MAP_LAST128K(a20addr));
          } else {
            *buf = BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
          }
        } else {
          *buf = BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
        }
      } else {
        // Read from ShadowRAM
        *buf = *(BX_MEM_THIS get_vector(a20addr));
      }
    }
#endif  // #if BX_SUPPORT_PCI
    else if ((a20addr < BX_MEM_THIS len) && !is_bios)
    {
      if (a20addr < 0x000c0000 || a20addr >= 0x00100000) {
        *buf = *(BX_MEM_THIS get_vector(a20addr));
      }
      // must be in C0000 - FFFFF range
      else if ((a20addr & 0xfffe0000) == 0x000e0000) {
        // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
        *buf = BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
      } else {
        *buf = BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
      }
    }
#if BX_PHY_ADDRESS_LONG
    else if (a20addr > BX_CONST64(0xffffffff)) {
      *buf = 0xff;
      ret = false; // error, beyond limits of memory
    }
#endif
    else if (is_bios)
    {
      if (BX_MEM_THIS flash_type > 0) {
        *buf = BX_MEM_THIS flash_read(a20addr & BIOS_MASK);
      } else {
        *buf = BX_MEM_THIS rom[a20addr & BIOS_MASK];
      }
    }
    else
    {
      *buf = 0xff;
      ret = false; // error, beyond limits of memory
    }
    buf++;
    a20addr++;
  }
  return ret;
}

#if BX_DEBUGGER || BX_GDBSTUB
bool BX_MEM_C::dbg_set_mem(BX_CPU_C *cpu, bx_phy_address addr, unsigned len, Bit8u *buf)
{
  bx_phy_address a20addr = A20ADDR(addr);
  struct memory_handler_struct *memory_handler = NULL;
  bool use_memory_handler = false, use_smram = false;

  if ((a20addr + len - 1) > BX_MEM_THIS len) {
    return false; // error, beyond limits of memory
  }

  bool is_bios = (a20addr >= (bx_phy_address)BX_MEM_THIS bios_rom_addr);
#if BX_PHY_ADDRESS_LONG
  if (a20addr > BX_CONST64(0xffffffff)) is_bios = false;
#endif

  if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000) && BX_MEM_THIS smram_available)
  {
    // SMRAM memory space
    if (BX_MEM_THIS smram_enable || (cpu->smm_mode() && !BX_MEM_THIS smram_restricted))
      use_smram = true;
  }

  memory_handler = BX_MEM_THIS memory_handlers[a20addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20addr && memory_handler->end >= a20addr)
    {
      if (!use_smram) {
        use_memory_handler = true;
        break;
      }
    }
    memory_handler = memory_handler->next;
  }

  for (; len>0; len--) {
    if (use_memory_handler) {
      memory_handler->write_handler(a20addr, 1, buf, memory_handler->param);
    }
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && (a20addr >= 0x000c0000 && a20addr < 0x00100000)) {
      unsigned area = (unsigned)(a20addr >> 14) & 0x0f;
      if (area > BX_MEM_AREA_F0000) area = BX_MEM_AREA_F0000;
      if (BX_MEM_THIS memory_type[area][1] == true) {
        // Write to ShadowRAM
        *(BX_MEM_THIS get_vector(a20addr)) = *buf;
      } else {
        // Ignore write to ROM
      }
    }
#endif  // #if BX_SUPPORT_PCI
    else if ((a20addr < 0x000c0000 || a20addr >= 0x00100000) && !is_bios)
    {
      *(BX_MEM_THIS get_vector(a20addr)) = *buf;
    }
    buf++;
    a20addr++;
  }
  return true;
}
#endif

Bit8u *BX_MEM_C::getHostMemAddr(BX_CPU_C *cpu, bx_phy_address addr, unsigned rw)
{
  bx_phy_address a20addr = A20ADDR(addr);

  bool is_bios = (a20addr >= (bx_phy_address)BX_MEM_THIS bios_rom_addr);
#if BX_PHY_ADDRESS_LONG
  if (a20addr > BX_CONST64(0xffffffff)) is_bios = false;
#endif

  bool write = rw & 1;

  // allow direct access to SMRAM memory space for code and veto data
  if ((cpu != NULL) && (rw == BX_EXECUTE)) {
    // reading from SMRAM memory space
    if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000) && (BX_MEM_THIS smram_available))
    {
      if (BX_MEM_THIS smram_enable || cpu->smm_mode())
        return BX_MEM_THIS get_vector(a20addr);
    }
  }

#if BX_SUPPORT_MONITOR_MWAIT
  if (write && BX_MEM_THIS is_monitor(a20addr & ~((bx_phy_address)(0xfff)), 0xfff)) {
    // Vetoed! Write monitored page !
    return(NULL);
  }
#endif

  struct memory_handler_struct *memory_handler = BX_MEM_THIS memory_handlers[a20addr >> 20];
  while (memory_handler) {
    if (memory_handler->begin <= a20addr &&
        memory_handler->end >= a20addr) {
      if (memory_handler->da_handler)
        return memory_handler->da_handler(a20addr, rw, memory_handler->param);
      else
        return(NULL); // Vetoed! memory handler for i/o apic, vram, mmio and PCI PnP
    }
    memory_handler = memory_handler->next;
  }

  if (! write) {
    if ((a20addr >= 0x000a0000 && a20addr < 0x000c0000))
      return(NULL); // Vetoed!  Mem mapped IO (VGA)
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && (a20addr >= 0x000c0000 && a20addr < 0x00100000)) {
      unsigned area = (unsigned)(a20addr >> 14) & 0x0f;
      if (area > BX_MEM_AREA_F0000) area = BX_MEM_AREA_F0000;
      if (BX_MEM_THIS memory_type[area][0] == false) {
        // Read from ROM
        if ((a20addr & 0xfffe0000) == 0x000e0000) {
          // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
          return (Bit8u *) &BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
        } else {
          return (Bit8u *) &BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ];
        }
      } else {
        // Read from ShadowRAM
        return BX_MEM_THIS get_vector(a20addr);
      }
    }
#endif
    else if ((a20addr < BX_MEM_THIS len) && !is_bios)
    {
      if (a20addr < 0x000c0000 || a20addr >= 0x00100000) {
        return BX_MEM_THIS get_vector(a20addr);
      }
      // must be in C0000 - FFFFF range
      else if ((a20addr & 0xfffe0000) == 0x000e0000) {
        // last 128K of BIOS ROM mapped to 0xE0000-0xFFFFF
        return (Bit8u *) &BX_MEM_THIS rom[BIOS_MAP_LAST128K(a20addr)];
      }
      else {
        return((Bit8u *) &BX_MEM_THIS rom[(a20addr & EXROM_MASK) + BIOSROMSZ]);
      }
    }
#if BX_PHY_ADDRESS_LONG
    else if (a20addr > BX_CONST64(0xffffffff)) {
      // Error, requested addr is out of bounds.
      return (Bit8u *) &BX_MEM_THIS bogus[a20addr & 0xfff];
    }
#endif
    else if (is_bios)
    {
      return (Bit8u *) &BX_MEM_THIS rom[a20addr & BIOS_MASK];
    }
    else
    {
      // Error, requested addr is out of bounds.
      return (Bit8u *) &BX_MEM_THIS bogus[a20addr & 0xfff];
    }
  }
  else
  { // op == {BX_WRITE, BX_RW}
    if ((a20addr >= BX_MEM_THIS len) || is_bios)
      return(NULL); // Error, requested addr is out of bounds.
    else if (a20addr >= 0x000a0000 && a20addr < 0x000c0000)
      return(NULL); // Vetoed!  Mem mapped IO (VGA)
#if BX_SUPPORT_PCI
    else if (BX_MEM_THIS pci_enabled && (a20addr >= 0x000c0000 && a20addr < 0x00100000))
    {
      // Veto direct writes to this area. Otherwise, there is a chance
      // for Guest2HostTLB and memory consistency problems, for example
      // when some 16K block marked as write-only using PAM registers.
      return(NULL);
    }
#endif
    else
    {
      if (a20addr < 0x000c0000 || a20addr >= 0x00100000) {
        return BX_MEM_THIS get_vector(a20addr);
      }
      else {
        return(NULL);  // Vetoed!  ROMs
      }
    }
  }
}

/*
 * One needs to provide both a read_handler and a write_handler.
 */
  bool
BX_MEM_C::registerMemoryHandlers(void *param, memory_handler_t read_handler,
                memory_handler_t write_handler, memory_direct_access_handler_t da_handler,
                bx_phy_address begin_addr, bx_phy_address end_addr)
{
  if (end_addr < begin_addr)
    return false;
  if (!read_handler) // allow NULL write and fetch handler
    return false;
  BX_INFO(("Register memory access handlers: 0x" FMT_PHY_ADDRX " - 0x" FMT_PHY_ADDRX, begin_addr, end_addr));
  for (Bit32u page_idx = (Bit32u)(begin_addr >> 20); page_idx <= (Bit32u)(end_addr >> 20); page_idx++) {
    Bit16u bitmap = 0xffff;
    if (begin_addr > (page_idx << 20)) {
      bitmap &= (0xffff << ((begin_addr >> 16) & 0xf));
    }
    if (end_addr < ((page_idx + 1) << 20)) {
      bitmap &= (0xffff >> (0x0f - ((end_addr >> 16) & 0xf)));
    }
    if (BX_MEM_THIS memory_handlers[page_idx] != NULL) {
      if ((bitmap & BX_MEM_THIS memory_handlers[page_idx]->bitmap) != 0) {
        BX_ERROR(("Register failed: overlapping memory handlers!"));
        return false;
      } else {
        bitmap |= BX_MEM_THIS memory_handlers[page_idx]->bitmap;
      }
    }
    struct memory_handler_struct *memory_handler = new struct memory_handler_struct;
    memory_handler->next = BX_MEM_THIS memory_handlers[page_idx];
    BX_MEM_THIS memory_handlers[page_idx] = memory_handler;
    memory_handler->read_handler = read_handler;
    memory_handler->write_handler = write_handler;
    memory_handler->da_handler = da_handler;
    memory_handler->param = param;
    memory_handler->begin = begin_addr;
    memory_handler->end = end_addr;
    memory_handler->bitmap = bitmap;
  }
  return true;
}

bool BX_MEM_C::unregisterMemoryHandlers(void *param, bx_phy_address begin_addr, bx_phy_address end_addr)
{
  bool ret = true;
  BX_INFO(("Memory access handlers unregistered: 0x" FMT_PHY_ADDRX " - 0x" FMT_PHY_ADDRX, begin_addr, end_addr));
  for (Bit32u page_idx = (Bit32u)(begin_addr >> 20); page_idx <= (Bit32u)(end_addr >> 20); page_idx++) {
    Bit16u bitmap = 0xffff;
    if (begin_addr > (page_idx << 20)) {
      bitmap &= (0xffff << ((begin_addr >> 16) & 0xf));
    }
    if (end_addr < ((page_idx + 1) << 20)) {
      bitmap &= (0xffff >> (0x0f - ((end_addr >> 16) & 0xf)));
    }
    struct memory_handler_struct *memory_handler = BX_MEM_THIS memory_handlers[page_idx];
    struct memory_handler_struct *prev = NULL;
    while (memory_handler &&
         memory_handler->param != param &&
         memory_handler->begin != begin_addr &&
         memory_handler->end != end_addr)
    {
      memory_handler->bitmap &= ~bitmap;
      prev = memory_handler;
      memory_handler = memory_handler->next;
    }
    if (!memory_handler) {
      ret = false;  // we should have found it
      continue; // anyway, try the other pages
    }
    if (prev)
      prev->next = memory_handler->next;
    else
      BX_MEM_THIS memory_handlers[page_idx] = memory_handler->next;
    delete memory_handler;
  }
  return ret;
}

void BX_MEM_C::enable_smram(bool enable, bool restricted)
{
  BX_MEM_THIS smram_available = true;
  BX_MEM_THIS smram_enable = enable;
  BX_MEM_THIS smram_restricted = restricted;
}

void BX_MEM_C::disable_smram(void)
{
  BX_MEM_THIS smram_available  = false;
  BX_MEM_THIS smram_enable     = false;
  BX_MEM_THIS smram_restricted = false;
}

// check if SMRAM is aavailable for CPU data accesses
bool BX_MEM_C::is_smram_accessible(void)
{
  return(BX_MEM_THIS smram_available) &&
        (BX_MEM_THIS smram_enable || !BX_MEM_THIS smram_restricted);
}

void BX_MEM_C::set_memory_type(memory_area_t area, bool rw, bool dram)
{
  if (area <= BX_MEM_AREA_F0000) {
    BX_MEM_THIS memory_type[area][rw] = dram;
  }
}

void BX_MEM_C::set_bios_write(bool enabled)
{
  BX_MEM_THIS bios_write_enabled = enabled;
}

void BX_MEM_C::set_bios_rom_access(Bit8u region, bool enabled)
{
  if (enabled) {
    BX_MEM_THIS bios_rom_access |= region;
  } else {
    BX_MEM_THIS bios_rom_access &= ~region;
  }
}

Bit8u BX_MEM_C::flash_read(Bit32u addr)
{
  Bit8u ret = 0;

  switch (BX_MEM_THIS flash_wsm_state) {
    case FLASH_INT_ID:
      if (addr & 1) {
        ret = (BX_MEM_THIS flash_type == 2) ? 0x7c : 0x94;
      } else {
        ret = 0x89;
      }
      BX_DEBUG(("flash read ID (address = 0x%08x value = 0x%02x)", addr, ret));
      break;
    case FLASH_READ_ARRAY:
      BX_DEBUG(("flash read from ROM (address = 0x%08x)", addr));
      ret = BX_MEM_THIS rom[addr];
      break;
    default:
      ret = BX_MEM_THIS flash_status;
      if (BX_MEM_THIS flash_wsm_state == FLASH_ERASE) {
        BX_MEM_THIS flash_status |= 0x80;
      }
      BX_DEBUG(("flash read status (address = 0x%08x value = 0x%02x)", addr, ret));
  }
  return ret;
}

void BX_MEM_C::flash_write(Bit32u addr, Bit8u data)
{
  Bit32u flash_addr;
  int i;

  if (BX_MEM_THIS flash_type == 2) {
    flash_addr = addr & 0x3ffff;
  } else {
    flash_addr = addr & 0x1ffff;
  }
  if (BX_MEM_THIS flash_wsm_state == FLASH_PROG_SETUP) {
    BX_DEBUG(("flash write to ROM (address = 0x%08x, data = 0x%02x)", flash_addr, data));
    BX_MEM_THIS rom[addr] &= data;
    BX_MEM_THIS flash_wsm_state = FLASH_READ_STATUS;
    BX_MEM_THIS flash_modified = true;
  } else {
    BX_DEBUG(("flash write command (address = 0x%08x, code = 0x%02x)", flash_addr, data));
    switch (data) {
      case FLASH_INT_ID:
      case FLASH_READ_ARRAY:
      case FLASH_ERASE_SETUP:
      case FLASH_ERASE_SUSP:
      case FLASH_PROG_SETUP:
        BX_MEM_THIS flash_wsm_state = data;
        break;
      case FLASH_READ_STATUS:
        if (BX_MEM_THIS flash_wsm_state != FLASH_ERASE) {
          BX_MEM_THIS flash_wsm_state = data;
        }
        break;
      case FLASH_CLR_STATUS:
        BX_MEM_THIS flash_status &= ~0x38;
        BX_MEM_THIS flash_wsm_state = FLASH_READ_ARRAY;
        break;
      case FLASH_ERASE:
        if (BX_MEM_THIS flash_wsm_state == FLASH_ERASE_SETUP) {
          BX_MEM_THIS flash_status &= ~0xc0;
          BX_MEM_THIS flash_wsm_state = FLASH_ERASE;
          if ((BX_MEM_THIS flash_type == 1) &&
              ((flash_addr == 0x1c000) || (flash_addr == 0x1d000))) {
            for (i = 0; i < 0x1000; i++) {
              BX_MEM_THIS rom[addr + i] = 0xff;
            }
            BX_MEM_THIS flash_modified = true;
          } else if ((BX_MEM_THIS flash_type == 2) &&
                     ((flash_addr == 0x38000) || (flash_addr == 0x3a000))) {
            for (i = 0; i < 0x2000; i++) {
              BX_MEM_THIS rom[addr + i] = 0xff;
            }
            BX_MEM_THIS flash_modified = true;
          }
        } else if (BX_MEM_THIS flash_wsm_state == FLASH_ERASE_SUSP) {
          BX_MEM_THIS flash_status &= ~0x40;
          BX_MEM_THIS flash_wsm_state = FLASH_ERASE;
        } else {
          BX_DEBUG(("flash_write(): unexpected ERASE CONFIRM / ERASE RESUME"));
        }
        break;
      default:
        BX_DEBUG(("flash_write(): unsupported code 0x%02x", data));
    }
  }
}
