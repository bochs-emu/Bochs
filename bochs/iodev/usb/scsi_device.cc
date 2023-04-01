/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  SCSI emulation layer (ported from QEMU)
//
//  Copyright (C) 2006 CodeSourcery.
//  Based on code by Fabrice Bellard
//
//  Written by Paul Brook
//  Updated and added to by Benjamin D Lunt (fys [at] fysnet [dot] net)
//
//  Copyright (C) 2007-2023  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "hdimage/hdimage.h"
#include "hdimage/cdrom.h"
#include "scsi_device.h"

#define LOG_THIS

#define DEVICE_NAME "SCSI drive"

static SCSIRequest *free_requests = NULL;
static Bit32u serial_number = 12345678;

Bit64s scsireq_save_handler(void *class_ptr, bx_param_c *param)
{
  char fname[BX_PATHNAME_LEN];
  char path[BX_PATHNAME_LEN+1];

  param->get_param_path(fname, BX_PATHNAME_LEN);
  if (!strncmp(fname, "bochs.", 6)) {
    strcpy(fname, fname+6);
  }
  if (SIM->get_param_string(BXPN_RESTORE_PATH)->isempty()) {
    return 0;
  }
  sprintf(path, "%s/%s", SIM->get_param_string(BXPN_RESTORE_PATH)->getptr(), fname);
  return ((scsi_device_t *) class_ptr)->save_requests(path);
}

void scsireq_restore_handler(void *class_ptr, bx_param_c *param, Bit64s value)
{
  char fname[BX_PATHNAME_LEN];
  char path[BX_PATHNAME_LEN+1];

  if (value != 0) {
    param->get_param_path(fname, BX_PATHNAME_LEN);
    if (!strncmp(fname, "bochs.", 6)) {
      strcpy(fname, fname+6);
    }
    sprintf(path, "%s/%s", SIM->get_param_string(BXPN_RESTORE_PATH)->getptr(), fname);
    ((scsi_device_t *) class_ptr)->restore_requests(path);
  }
}

scsi_device_t::scsi_device_t(device_image_t *_hdimage, int _tcq,
                           scsi_completionfn _completion, void *_dev)
{
  type = SCSIDEV_TYPE_DISK;
  cdrom = NULL;
  hdimage = _hdimage;
  requests = NULL;
  sense = 0;
  asc = 0;
  ascq = 0;
  tcq = _tcq;
  completion = _completion;
  dev = _dev;
  block_size = hdimage->sect_size;
  locked = 0;
  read_only = 0;
  inserted = 1;
  max_lba = (hdimage->hd_size / block_size) - 1;
  curr_lba = max_lba;
  sprintf(drive_serial_str, "%d", serial_number++);
  seek_timer_index =
    DEV_register_timer(this, seek_timer_handler, 1000, 0, 0, "USB HD seek");
  statusbar_id = bx_gui->register_statusitem("USB-HD", 1);

  put("SCSIHD");
}

scsi_device_t::scsi_device_t(cdrom_base_c *_cdrom, int _tcq,
                           scsi_completionfn _completion, void *_dev)
{
  type = SCSIDEV_TYPE_CDROM;
  cdrom = _cdrom;
  hdimage = NULL;
  requests = NULL;
  sense = 0;
  asc = 0;
  ascq = 0;
  tcq = _tcq;
  completion = _completion;
  dev = _dev;
  block_size = 2048;
  locked = 0;
  read_only = 1;
  inserted = 0;
  max_lba = 0;
  curr_lba = 0;
  sprintf(drive_serial_str, "%d", serial_number++);
  seek_timer_index =
    DEV_register_timer(this, seek_timer_handler, 1000, 0, 0, "USB CD seek");
  statusbar_id = bx_gui->register_statusitem("USB-CD", 1);

  put("SCSICD");
}

scsi_device_t::~scsi_device_t(void)
{
  SCSIRequest *r, *next;

  if (requests) {
    r = requests;
    while (r != NULL) {
      next = r->next;
      delete [] r->dma_buf;
      delete r;
      r = next;
    }
  }
  if (free_requests) {
    r = free_requests;
    while (r != NULL) {
      next = r->next;
      delete [] r->dma_buf;
      delete r;
      r = next;
    }
    free_requests = NULL;
  }
  bx_gui->unregister_statusitem(statusbar_id);
  bx_pc_system.deactivate_timer(seek_timer_index);
  bx_pc_system.unregisterTimer(seek_timer_index);
}

void scsi_device_t::register_state(bx_list_c *parent, const char *name)
{
  bx_list_c *list = new bx_list_c(parent, name, "");
  BXRS_DEC_PARAM_SIMPLE(list, sense);
  BXRS_DEC_PARAM_SIMPLE(list, asc);
  BXRS_DEC_PARAM_SIMPLE(list, ascq);
  BXRS_PARAM_BOOL(list, locked, locked);
  BXRS_PARAM_BOOL(list, read_only, read_only);
  BXRS_DEC_PARAM_SIMPLE(list, curr_lba);
  bx_param_bool_c *requests = new bx_param_bool_c(list, "requests", NULL, NULL, 0);
  requests->set_sr_handlers(this, scsireq_save_handler, scsireq_restore_handler);
}

// SCSI request handling

SCSIRequest* scsi_device_t::scsi_new_request(Bit32u tag)
{
  SCSIRequest *r;

  if (free_requests) {
    r = free_requests;
    free_requests = r->next;
  } else {
    r = new SCSIRequest;
    r->dma_buf = new Bit8u[SCSI_DMA_BUF_SIZE];
  }
  r->tag = tag;
  r->sector_count = 0;
  r->write_cmd = 0;
  r->async_mode = 0;
  r->seek_pending = 0;
  r->buf_len = 0;
  r->status = 0;

  r->next = requests;
  requests = r;
  return r;
}

void scsi_device_t::scsi_remove_request(SCSIRequest *r)
{
  SCSIRequest *last;

  if (requests == r) {
    requests = r->next;
  } else {
    last = requests;
    while (last != NULL) {
      if (last->next != r)
        last = last->next;
      else
        break;
    }
    if (last) {
      last->next = r->next;
    } else {
      BX_ERROR(("orphaned request"));
    }
  }
  r->next = free_requests;
  free_requests = r;
}

SCSIRequest* scsi_device_t::scsi_find_request(Bit32u tag)
{
  SCSIRequest *r = requests;
  while (r != NULL) {
    if (r->tag != tag)
      r = r->next;
    else
      break;
  }
  return r;
}

bool scsi_device_t::save_requests(const char *path)
{
  char tmppath[BX_PATHNAME_LEN];
  FILE *fp, *fp2;

  if (requests != NULL) {
    fp = fopen(path, "w");
    if (fp != NULL) {
      SCSIRequest *r = requests;
      Bit32u i = 0;
      while (r != NULL) {
        fprintf(fp, "%u = {\n", i);
        fprintf(fp, "  tag = %u\n", r->tag);
        fprintf(fp, "  sector = " FMT_LL "u\n", r->sector);
        fprintf(fp, "  sector_count = %u\n", r->sector_count);
        fprintf(fp, "  buf_len = %d\n", r->buf_len);
        fprintf(fp, "  status = %u\n", r->status);
        fprintf(fp, "  write_cmd = %u\n", r->write_cmd);
        fprintf(fp, "  async_mode = %u\n", r->async_mode);
        fprintf(fp, "  seek_pending = %u\n", r->seek_pending);
        fprintf(fp, "}\n");
        if (r->buf_len > 0) {
          sprintf(tmppath, "%s.%u", path, i);
          fp2 = fopen(tmppath, "wb");
          if (fp2 != NULL) {
            fwrite(r->dma_buf, 1, (size_t)r->buf_len, fp2);
          }
          fclose(fp2);
        }
        r = r->next;
        i++;
      }
      fclose(fp);
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void scsi_device_t::restore_requests(const char *path)
{
  char line[512], pname[16], tmppath[BX_PATHNAME_LEN];
  char *ret, *ptr;
  FILE *fp, *fp2;
  int i, reqid = -1;
  Bit64s value;
  Bit32u tag = 0;
  SCSIRequest *r = NULL;
  bool rrq_error = 0;

  fp = fopen(path, "r");
  if (fp != NULL) {
    do {
      ret = fgets(line, sizeof(line)-1, fp);
      line[sizeof(line) - 1] = '\0';
      size_t len = strlen(line);
      if ((len > 0) && (line[len-1] < ' '))
        line[len-1] = '\0';
      i = 0;
      if ((ret != NULL) && strlen(line) > 0) {
        ptr = strtok(line, " ");
        while (ptr) {
          if (i == 0) {
            if (!strcmp(ptr, "}")) {
              if (r != NULL) {
                if (r->buf_len > 0) {
                  sprintf(tmppath, "%s.%u", path, reqid);
                  fp2 = fopen(tmppath, "wb");
                  if (fp2 != NULL) {
                    fread(r->dma_buf, 1, (size_t)r->buf_len, fp2);
                  }
                  fclose(fp2);
                }
              }
              reqid = -1;
              r = NULL;
              tag = 0;
              break;
            } else if (reqid < 0) {
              reqid = (int) strtol(ptr, NULL, 10);
              break;
            } else {
              strcpy(pname, ptr);
            }
          } else if (i == 2) {
            if (reqid >= 0) {
              if (!strcmp(pname, "tag")) {
                if (tag == 0) {
                  tag = (Bit32u) strtoul(ptr, NULL, 10);
                  r = scsi_new_request(tag);
                  if (r == NULL) {
                    BX_ERROR(("restore_requests(): cannot create request"));
                    rrq_error = 1;
                  }
                } else {
                  BX_ERROR(("restore_requests(): data format error"));
                  rrq_error = 1;
                }
              } else {
                value = (Bit64s) strtoll(ptr, NULL, 10);
                if (!strcmp(pname, "sector")) {
                  r->sector = (Bit64u) value;
                } else if (!strcmp(pname, "sector_count")) {
                  r->sector_count = (Bit32u) value;
                } else if (!strcmp(pname, "buf_len")) {
                  r->buf_len = (int) value;
                } else if (!strcmp(pname, "status")) {
                  r->status = (Bit32u) value;
                } else if (!strcmp(pname, "write_cmd")) {
                  r->write_cmd = (bool) value;
                } else if (!strcmp(pname, "async_mode")) {
                  r->async_mode = (bool) value;
                } else if (!strcmp(pname, "seek_pending")) {
                  r->seek_pending = (Bit8u) value;
                } else {
                  BX_ERROR(("restore_requests(): data format error"));
                  rrq_error = 1;
                }
              }
            } else {
              BX_ERROR(("restore_requests(): data format error"));
              rrq_error = 1;
            }
          }
          i++;
          ptr = strtok(NULL, " ");
        }
      }
    } while (!feof(fp) && !rrq_error);
    fclose(fp);
  } else {
    BX_ERROR(("restore_requests(): error in file open"));
  }
}

// SCSI command implementation

void scsi_device_t::scsi_command_complete(SCSIRequest *r, int status, Bit8u _sense, Bit8u _asc, Bit8u _ascq)
{
  Bit32u tag;
  BX_DEBUG(("command complete tag=0x%x status=%d sense=%d/%d/%d", r->tag, status, _sense, _asc, _ascq));
  sense = _sense;
  asc = _asc;
  ascq = _ascq;
  tag = r->tag;
  scsi_remove_request(r);
  completion(dev, SCSI_REASON_DONE, tag, status);
}

void scsi_device_t::scsi_cancel_io(Bit32u tag)
{
  BX_DEBUG(("cancel tag=0x%x", tag));
  SCSIRequest *r = scsi_find_request(tag);
  if (r) {
    bx_pc_system.deactivate_timer(seek_timer_index);
    scsi_remove_request(r);
  }
}

void scsi_device_t::scsi_read_complete(void *req, int ret)
{
  SCSIRequest *r = (SCSIRequest *) req;

  if (ret) {
    BX_ERROR(("IO error"));
    completion(r, SCSI_REASON_DATA, r->tag, 0);
    scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_NO_SENSE, 0, 0);
    return;
  }
  BX_DEBUG(("data ready tag=0x%x len=%d", r->tag, r->buf_len));
  curr_lba = r->sector;

  completion(dev, SCSI_REASON_DATA, r->tag, r->buf_len);
}

void scsi_device_t::scsi_read_data(Bit32u tag)
{
  SCSIRequest *r = scsi_find_request(tag);
  if (!r) {
    BX_ERROR(("bad read tag 0x%x", tag));
    return;
  }
  if (r->sector_count == (Bit32u)-1) {
    BX_DEBUG(("read buf_len=%d", r->buf_len));
    r->sector_count = 0;
    completion(dev, SCSI_REASON_DATA, r->tag, r->buf_len);
    return;
  }
  BX_DEBUG(("read sector_count=%d", r->sector_count));
  if (r->sector_count == 0) {
    scsi_command_complete(r, STATUS_GOOD, SENSE_NO_SENSE, 0, 0);
    return;
  }
  if ((r->async_mode) && (r->seek_pending == 2)) {
    start_seek(r);
  } else if (!r->seek_pending) {
    seek_complete(r);
  }
}

void scsi_device_t::scsi_write_complete(void *req, int ret)
{
  SCSIRequest *r = (SCSIRequest *)req;
  Bit32u len;

  if (ret) {
    BX_ERROR(("IO error"));
    scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR, 0, 0);
    return;
  }

  if (r->sector_count == 0) {
    scsi_command_complete(r, STATUS_GOOD, SENSE_NO_SENSE, 0, 0);
  } else {
    len = r->sector_count * block_size;
    if (len > SCSI_DMA_BUF_SIZE) {
      len = SCSI_DMA_BUF_SIZE;
    }
    r->buf_len = len;
    BX_DEBUG(("write complete tag=0x%x more=%d", r->tag, len));
    curr_lba = r->sector;
    completion(dev, SCSI_REASON_DATA, r->tag, len);
  }
}

void scsi_device_t::scsi_write_data(Bit32u tag)
{
  SCSIRequest *r = scsi_find_request(tag);

  BX_DEBUG(("write data tag=0x%x", tag));
  if (!r) {
    BX_ERROR(("bad write tag 0x%x", tag));
    return;
  }
  if (type == SCSIDEV_TYPE_DISK) {
    if ((r->buf_len / block_size) > 0) {
      if ((r->async_mode) && (r->seek_pending == 2)) {
        start_seek(r);
      } else if (!r->seek_pending) {
        seek_complete(r);
      }
    } else {
      scsi_write_complete(r, 0);
    }
  } else {
    BX_ERROR(("CD-ROM: write not supported"));
    scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR, 0, 0);
  }
}

Bit8u* scsi_device_t::scsi_get_buf(Bit32u tag)
{
  SCSIRequest *r = scsi_find_request(tag);
  if (!r) {
    BX_ERROR(("bad buffer tag 0x%x", tag));
    return NULL;
  }
  return r->dma_buf;
}

Bit32s scsi_device_t::scsi_send_command(Bit32u tag, Bit8u *buf, Bit8u cmd_len, int lun, bool async)
{
  Bit64u nb_sectors;
  Bit64u lba;
  Bit32s len;
  int cmdlen;  // our exected length of this command
  Bit8u command;
  Bit8u *outbuf;
  SCSIRequest *r;
  Bit8u _sense = SENSE_NO_SENSE, _asc = 0, _ascq = 0; // assume good return
  
  command = buf[0];
  r = scsi_find_request(tag);
  if (r) {
    BX_ERROR(("tag 0x%x already in use", tag));
    scsi_cancel_io(tag);
  }
  r = scsi_new_request(tag);
  outbuf = r->dma_buf;
  BX_DEBUG(("command: lun=%d tag=0x%x data=0x%02x", lun, tag, buf[0]));
  switch (command >> 5) {
    case 0:
        lba = buf[3] | (buf[2] << 8) | ((buf[1] & 0x1f) << 16);
        len = buf[4];
        cmdlen = 6;
        break;
    case 1:
    case 2:
        lba = buf[5] | (buf[4] << 8) | (buf[3] << 16) | (buf[2] << 24);
        len = buf[8] | (buf[7] << 8);
        cmdlen = 10;
        break;
    case 4:
        lba = buf[9] | (buf[8] << 8) | (buf[7] << 16) | (buf[6] << 24) |
              ((Bit64u) buf[5] << 32) | ((Bit64u) buf[4] << 40) |
              ((Bit64u) buf[3] << 48) | ((Bit64u) buf[2] << 56);
        len = buf[13] | (buf[12] << 8) | (buf[11] << 16) | (buf[10] << 24);
        cmdlen = 16;
        break;
    case 5:
        lba = buf[5] | (buf[4] << 8) | (buf[3] << 16) | (buf[2] << 24);
        len = buf[9] | (buf[8] << 8) | (buf[7] << 16) | (buf[6] << 24);
        cmdlen = 12;
        break;
    default:
        BX_ERROR(("Unsupported command length, command %x", command));
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x1A; // Parameter List Length Error
        goto fail;
  }

  // check that the expected command length matches the sent command length.
  // some hardware may fail if the command length byte isn't correct.
  if (cmdlen != cmd_len) {
    BX_ERROR(("Sent command length (%d) doesn't match expected command length (%d).", cmd_len, cmdlen));
#if SCSI_STRICT_CDB
    _sense = SENSE_ILLEGAL_REQUEST;
    _asc = 0x1A; // Parameter List Length Error
    goto fail;
#endif
  }

#if SCSI_OBSOLUTE_LUN
  if (lun == 0) lun = (buf[1] >> 5);
#endif
  if (lun) {
    BX_ERROR(("unimplemented LUN %d (%d)", lun, buf[1] >> 5));
    if ((command != 0x03) && (command != 0x12)) { // REQUEST SENSE and INQUIRY
      _sense = SENSE_ILLEGAL_REQUEST;
      _asc = 0x21; // LUN out of range
      goto fail;
    }
  }
  switch (command) {
    case 0x00:
      BX_DEBUG(("Test Unit Ready"));
      if (!inserted)
        goto notready;
      break;
    case 0x03:
      // SPC4r18, section 6.29, page 353
      // SPC4r18, section 4.5.3, page 61
      BX_DEBUG(("request Sense (len %d)", len));
      if (len != 252)
        BX_DEBUG(("SPC-4 recommends that the length be equal to 252."));
      if ((buf[1] & 1) && (len < 18))
        BX_DEBUG(("SPC-4 recommends that the length be at least 18 if the DESC bit is set."));

      outbuf[ 0] = (0<<7) | 0x70; // the INFORMATION field is not valid + 0x70
      outbuf[ 1] = 0;  // obsolete
      outbuf[ 2] = sense & 0x0F;
      outbuf[ 3] = 0;  // INFORMATION (MSB)
      outbuf[ 4] = 0;  // INFORMATION
      outbuf[ 5] = 0;  // INFORMATION
      outbuf[ 6] = 0;  // INFORMATION (LSB)
      outbuf[ 7] = 10; // 10 more bytes after this one
      outbuf[ 8] = 0;  // Command Specific information (MSB)
      outbuf[ 9] = 0;  // Command Specific information
      outbuf[10] = 0;  // Command Specific information
      outbuf[11] = 0;  // Command Specific information (LSB)
      outbuf[12] = asc;  // additional sense code
      outbuf[13] = ascq; // additional sense code qualifier
      outbuf[14] = 0;  // Field Replaceable Unit Code
      outbuf[15] = 0;  // SKSV + Sense key specific (MSB)
      outbuf[16] = 0;  // Sense key specific
      outbuf[17] = 0;  // Sense key specific (LSB)
      r->buf_len = 18;

      // if 'sense' was not SENSE_NO_SENSE, we need to set it to SENSE_RECOVERY_ERROR
      // if 'sense' was SENSE_RECOVERY_ERROR, we can now set it to SENSE_NO_SENSE
      sense = (sense > SENSE_RECOVERED_ERROR) ? SENSE_RECOVERED_ERROR : SENSE_NO_SENSE;
      break;
    case 0x12:
      BX_DEBUG(("inquiry (len %d)", len));
      if (buf[1] & 0x2) { // obsolete after SPC-2
        // Command support data - optional, not implemented
        BX_ERROR(("SPC-2: optional INQUIRY command support request not implemented. Obsolete after SPC-2"));
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x24; // Invalid Field in CDB
        goto fail;
      } 
      
      // Vital product data
      if (buf[1] & 0x1) {
        Bit8u page_code = buf[2];
        if (len < 4) {
          BX_ERROR(("Error: Inquiry (EVPD[%02X]) buffer size %d is less than 4", page_code, len));
          _sense = SENSE_ILLEGAL_REQUEST;
          _asc = 0x26; // Parameter Value Invalid
          goto fail;
        }
        // mandatory page_codes are listed in SPC-4, section 7.7.1, p551
        switch (page_code) {
          // SPC-4, section 7.7.12, p581
          case 0x00:
            // Supported page codes, mandatory
            BX_DEBUG(("Inquiry EVPD[Supported pages] buffer size %d", len));
            outbuf[ 0] = (type == SCSIDEV_TYPE_CDROM) ? PERIPHERAL_TYPE_CDROM : PERIPHERAL_TYPE_SBC3;
            outbuf[ 1] = 0x00; // this page
            outbuf[ 2] = 0x00; // reserved
            outbuf[ 3] = 3;    // number of pages supported (count of bytes that follow)
            // list of supported pages
            outbuf[ 4] = 0x00; // this page (mandatory)
            outbuf[ 5] = 0x80; // unit serial number (optional)
            outbuf[ 6] = 0x83; // device identification (mandatory)
            r->buf_len = 7;
            break;
            
          // SPC-4, section 7.7.13, p581
          case 0x80: // Device serial number, optional
            {
              BX_DEBUG(("Inquiry EVPD[Serial number] buffer size %d", len));
              int l = BX_MIN(len - 4, (int) strlen(drive_serial_str));
              outbuf[ 0] = (type == SCSIDEV_TYPE_CDROM) ? PERIPHERAL_TYPE_CDROM : PERIPHERAL_TYPE_SBC3;
              outbuf[ 1] = 0x80; // this page
              outbuf[ 2] = 0x00; // reserved
              outbuf[ 3] = l;    // length of serial number field
              memcpy(&outbuf[4], drive_serial_str, l);
              r->buf_len = 4 + l;
            }
            break;
            
          // SPC-4, section 7.7.3, p553
          case 0x83: // Device identification page (mandatory)
            {
              BX_DEBUG(("Inquiry EVPD[Device identification] buffer size %d", len));
              int l = BX_MIN(255, (int) strlen(DEVICE_NAME));
              outbuf[ 0] = (type == SCSIDEV_TYPE_CDROM) ? PERIPHERAL_TYPE_CDROM : PERIPHERAL_TYPE_SBC3;
              outbuf[ 1] = 0x83;  // this page
              outbuf[ 2] = (Bit8u) (((l + 8) >> 8) & 0xFF); // (MSB)
              outbuf[ 3] = (Bit8u) (((l + 8) >> 0) & 0xFF); // (LSB)
              // first designation descriptor
              outbuf[ 4] = 0x02; // ASCII (SPC-4, Table 23, page 50)
              outbuf[ 5] = 0;   // not officially assigned
              outbuf[ 6] = 0;   // reserved
              outbuf[ 7] = (Bit8u) (l & 0xFF); // length of data following
              memcpy(&outbuf[8], DEVICE_NAME, l);
              r->buf_len = 8 + l;
            }
            break;
            
          default:
            BX_ERROR(("Error: unsupported Inquiry (EVPD[%02X]) buffer size %d", page_code, len));
            _sense = SENSE_ILLEGAL_REQUEST;
            _asc = 0x26; // Parameter Value Invalid
            goto fail;
        }
        
      // Standard INQUIRY data
      } else {
        // EVPD bit is zero, so if the PAGE_CODE field is not zero, error
        if (buf[2] != 0) {
          BX_ERROR(("Error: Inquiry (STANDARD) page_code is non-zero [%02X]", buf[2]));
          _sense = SENSE_ILLEGAL_REQUEST;
          _asc = 0x24; // Invalid Field in CDB
          goto fail;
        }
        if (len < 36)
          BX_DEBUG(("Inquiry (STANDARD) buffer size %d is less than the recommended minimum of 36.", len));
        
        if (type == SCSIDEV_TYPE_CDROM) {
          outbuf[ 0] = PERIPHERAL_TYPE_CDROM;
          outbuf[ 1] = 0x80;
          memcpy(&outbuf[16], "BOCHS CD-ROM   ", 16);
        } else {
          outbuf[ 0] = PERIPHERAL_TYPE_SBC3;
          outbuf[ 1] = 0;
          memcpy(&outbuf[16], "BOCHS HARDDISK ", 16);
        }
        // Identify device as SCSI-3 rev 1.
        // Some later commands are also implemented.
        outbuf[2] = 3;  // SPC-1 (4 = SPC-2, 5 = SPC-3)
        outbuf[3] = 2;  // Format 2 (must be 2)
        outbuf[4] = 31; // Additional Length
        outbuf[5] = 0;
        outbuf[6] = 0;
        outbuf[7] = 0x10 | (tcq ? 0x02 : 0);  // Sync data transfer and TCQ.
        memcpy(&outbuf[8], "BOCHS  ", 8);
        memcpy(&outbuf[32], "1.0", 4);
        r->buf_len = 36;
      }
      break;
    case 0x16: // reserve(6) (SPC-1 only)
      BX_INFO(("Reserve(6)"));
      if (buf[1] & 1) { // the Extent is optional
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x26; // Parameter Value Invalid
        goto fail;
      } // else do nothing, but return good.
      break;
    case 0x17: // release(6) (SPC-1 only)
      BX_INFO(("Release(6)"));
      if (buf[1] & 1) { // the Extent is optional
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x26; // Parameter Value Invalid
        goto fail;
      } // else do nothing, but return good.
      break;
    case 0x56: // reserve(10) (SPC-1 only)
      BX_INFO(("Reserve(10)"));
      if (buf[1] & 3) { // the Extent is optional, the LongID is not supported
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x26; // Parameter Value Invalid
        goto fail;
      } // else do nothing, but return good.
      break;
    case 0x57: // release(10) (SPC-1 only)
      BX_INFO(("Release(10)"));
      if (buf[1] & 3) { // the Extent is optional, the LongID is not supported
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x26; // Parameter Value Invalid
        goto fail;
      } // else do nothing, but return good.
      break;
    case 0x1a:  // mode sense(6)
    case 0x5a:  // mode sense(10)
      {
        Bit8u *p = outbuf;
        bool llbaa = (buf[1] & (1 << 4)) > 0; // 0 = 8-byte descriptors, 1 = 16-byte descriptors
        bool dbd = (buf[1] & (1 << 3)) > 0;
        Bit8u page_code = buf[2] & 0x3F;
        Bit8u pc = buf[2] >> 6;   // page control (0, 1, 2, or 3)
        Bit8u sub_page = buf[3];  // sub page code
        
        BX_DEBUG(("Mode Sense(%d) (page_code 0x%02X, pc %d, sub_page %d, llbaa %d, dbd %d, len %d)", 
          (command == 0x1A) ? 6 : 10, page_code, pc, sub_page, llbaa, dbd, len));
        
        // if pc == Saved, return error. We don't support saved parameters
        if (pc == PAGE_CONTROL_SAVED) {
          BX_ERROR(("Mode Sense(%d): Arborting command. PC == Saved, not supported.", (command == 0x1A) ? 6 : 10));
          _sense = SENSE_ILLEGAL_REQUEST;
          _asc = 0x26; // Parameter Value Invalid
          goto fail;
        }

        // MMC-6r2f: Section 6.12.1,
        // Note 17. Since MM Drives do not support sub-pages of mode pages, the Sub-Page field of the MODE SENSE (10) command is ignored by the Drive.
        if ((type == SCSIDEV_TYPE_CDROM) && (sub_page != 0)) {
          BX_ERROR(("Mode Sense(%d) sub_page value of 0x%02X is ignored by CD-ROM devices.", (command == 0x1A) ? 6 : 10, sub_page));
          sub_page = 0;
        }
        
        // MMC-6r2f: Section 6.12.1,
        // Note 18. Since MM Drives do not support Block Descriptors (see 7.2.1), the LLBAA bit in the MODE SENSE (10) CDB has no meaning and is ignored by the Drive.
        if ((type == SCSIDEV_TYPE_CDROM) && llbaa) {
          BX_ERROR(("Mode Sense(%d) llbaa is ignored by CD-ROM devices.", (command == 0x1A) ? 6 : 10));
          llbaa = 0;
        }
        
        if ((type == SCSIDEV_TYPE_CDROM) && (command == 0x1A)) {
          // MMC-2 (1999) was that last spec to allow Mode Sense(6)
          BX_ERROR(("Mode Sense(6) is obsolete for CD-ROM devices. Use Mode Sense(10) instead."));
        }
        
        // create the header
        if (command == 0x1A) { // mode sense(6)
          outbuf[0] = 0; // length will be updated later (LSB)
          outbuf[1] = 0; // Default medium type (type = disk only, reserved in CDROMs)
          outbuf[2] = ((type == SCSIDEV_TYPE_DISK) && read_only) ? 0x80 : 0x00;  // 0x80 = read only (reserved in CDROMs)
          outbuf[3] = 0; // Block descriptor length (LSB) (possibly changed below)
          p += 4;
        } else {               // mode sense(10)
          outbuf[0] = 0; // length will be updated later (MSB)
          outbuf[1] = 0; // length will be updated later (LSB)
          outbuf[2] = 0; // Default medium type (type = disk only, reserved in CDROMs)
          outbuf[3] = ((type == SCSIDEV_TYPE_DISK) && read_only) ? 0x80 : 0x00;  // 0x80 = read only (reserved in CDROMs)
          outbuf[4] = 0; // reserved
          outbuf[5] = 0; // reserved
          outbuf[6] = 0; // Block descriptor length (MSB) (possibly changed below)
          outbuf[7] = 0; // Block descriptor length (LSB)
          p += 8;
        }
        
        // start of block descriptors
        //  (remember to update the two fields above)
        //  (outbuf[3] and outbuf[6 & 7])
        // (cd-roms do not allow Block Descriptors)
        if ((type == SCSIDEV_TYPE_DISK) && !dbd) {
          nb_sectors = (hdimage->hd_size / block_size);
          if (nb_sectors > 0x00FFFFFF) // don't truncate a large disk
            nb_sectors = 0;
          if (!llbaa) {  // 8-byte blocks
            p[0] = 0x00;    // media density code
            p[1] = (Bit8u) ((nb_sectors >> 16) & 0xFF);  // number of sectors
            p[2] = (Bit8u) ((nb_sectors >>  8) & 0xFF);  // number of sectors
            p[3] = (Bit8u) ((nb_sectors >>  0) & 0xFF);  // number of sectors (LSB)
            p[4] = 0x00;    // reserved
            p[5] = (Bit8u) ((block_size >> 16) & 0xFF);  // block size (MSB)
            p[6] = (Bit8u) ((block_size >>  8) & 0xFF);  // block size
            p[7] = (Bit8u) ((block_size >>  0) & 0xFF);  // block size (LSB)
            p += 8;
            if (command == 0x1A) outbuf[3] = 8;
            else                 outbuf[7] = 8;
          } else {  // 16-byte blocks
            BX_ERROR(("We don't support device specific 16-byte Block Descriptors."));
          }
        }
        
        // mode pages follow any block descriptors
        //  if page = 0x3F, return all pages we support from 0 to 3F
        if (page_code == SENSE_RETURN_ALL) {
          for (Bit8u i=0; i<SENSE_RETURN_ALL; i++) {
            p += scsi_do_modepage(p, pc, sub_page, i);
          }
        } else {
          int l = scsi_do_modepage(p, pc, sub_page, page_code);
          if (l == 0) {
            _sense = SENSE_ILLEGAL_REQUEST;
            _asc = 0x24; // Invalid Field in CDB
            goto fail;
          } else
            p += l;
        }

        r->buf_len = (int) (p - outbuf);
        if (command == 0x1A) {  // mode sense(6)
          if (r->buf_len > 256) {
            BX_ERROR(("Mode Sense(6): return length is more than 256 (%d). Use Mode Sense(10) instead.", r->buf_len));
          }
          outbuf[0] = (r->buf_len - sizeof(Bit8u)) & 0xFF;
        } else {                // mode sense(10)
          outbuf[0] = ((r->buf_len - sizeof(Bit16u)) >> 8) & 0xFF;
          outbuf[1] = ((r->buf_len - sizeof(Bit16u)) >> 0) & 0xFF;
        }
      }
      break;
    case 0x1b: 
      BX_DEBUG(("Start Stop Unit"));
      // Start/Stop unit (MMC6r02f, section 6.42, page 574(622)
      if (type == SCSIDEV_TYPE_CDROM) {
        // if Power Conditions = 0, and FL = 0, LoEj = 1, and Start = 0, eject the medium
        if ((buf[4] & 0xF7) == 0x02) {
          cdrom->eject_cdrom();
          inserted = 0;
        }
        
      // Start/Stop unit (SBC-3r25, section 5.23, page 140
      } else {
        if ((buf[4] & 4) == 0) {
          // flush all by simulating a SYNCHRONIZE CACHE(10) (0x35)
          // or SYNCHRONIZE CACHE(16) (0x91) command
          // with: SUNC_NV = 0, LBA = 0, and Num Blocks = 0
          // (both of these commands are 'implemented' below)
        }
        // if Power Conditions = 0, and FL = 0, LoEj = 1, and Start = 0, eject the medium
        if ((buf[4] & 0xF7) == 0x02) {
          BX_ERROR(("Nothing to eject."));
        }
      }
      break;
    case 0x1e:
      BX_INFO(("Prevent Allow Medium Removal (prevent = %d)", buf[4] & 3));
      locked = buf[4] & 1;
      break;
    case 0x4a: // SCSI_CD_EVENT_STATUS
      if (type == SCSIDEV_TYPE_CDROM) {
        Bit8u Class = 0;
        Bit16u out_len = 4; // the initial header is 4 bytes
        Bit8u *p = &outbuf[4];  // starts at byte 4
        
        // This code is experimental. may need some help
        BX_DEBUG(("Event Status notification: Requested Byte 0x%02X", buf[4]));
        
        len = (Bit32s) (((Bit32u) buf[7] << 8) | buf[8]);
        if (buf[1] & EVENT_STATUS_POLLED) { // polled bit is set
          // only do the Event Header if length <= 4, else we can do the Event
          // buf[4] can be zero. Usually the Guest is requesting the support class event bitmap when this happens.
          if (len > 4) {
            // hightest priority (lowest bit set) gets executed (only)
            if (buf[4] & EVENT_STATUS_RESERVED_0) { // Reserved bit is set (MMC4R05A.PDF, 6.7.1.3, page 242, not considered an error)
              BX_DEBUG(("Event Status Notify: bit 0 in the request byte is set"));
              Class = 0;
            }
            if (buf[4] & EVENT_STATUS_OP_CHANGE) { // Operational Change request bit is set
              Class = 1;
              //p[0] = 0; // Bits 3:0 = Event Code (must be zero until an event occurs, then zero after read until next event occurs)
            } else
            if (buf[4] & EVENT_STATUS_POWER_MAN) { // Power Management request bit is set
              Class = 2;
              //p[0] = 0; // Bits 3:0 = Event Code (must be zero until an event occurs, then zero after read until next event occurs)
            } else
            if (buf[4] & EVENT_STATUS_EXT_REQ) { // External Request request bit is set
              Class = 3;
              //p[0] = 0; // Bits 3:0 = Event Code (must be zero until an event occurs, then zero after read until next event occurs)
            } else
            if (buf[4] & EVENT_STATUS_MEDIA_REQ) { // MEDIA request bit is set
              Class = 4;
              // TODO: We need to get the status from the Host. Did the CD-ROM get ejected or inserted?
              //       For now, we put no change (zero)
              p[0] = 0; // Bits 3:0 = Event Code (must be zero until an event occurs, then zero after read until next event occurs)
                        //   (MMC4R05A.PDF Section 6.7.2.5, page 248)
              p[1] = EVENT_STATUS_MED_PRES | EVENT_STATUS_MED_N_OPEN; // bits 7:2 = reserved, bit 1 = Media Present, bit 0 = Door/Tray open
              p[2] = 0;  // Start slot
              p[3] = 0;  // End slot
              out_len += 4;  // 8 total bytes
            } else
            if (buf[4] & EVENT_STATUS_MULTI_INIT) { // Multi-Initiator request bit is set
              Class = 5;
              //p[0] = 0; // Bits 3:0 = Event Code (must be zero until an event occurs, then zero after read until next event occurs)
            } else
            if (buf[4] & EVENT_STATUS_DEV_BUSY) { // Device Busy request bit is set
              Class = 6;
              //p[0] = 0; // Bits 3:0 = Event Code (must be zero until an event occurs, then zero after read until next event occurs)
            }
            if (buf[4] & EVENT_STATUS_RESERVED_7) { // Reserved bit is set (MMC4R05A.PDF, 6.7.1.3, page 242, not considered an error)
              BX_DEBUG(("Event Status Notify: bit 7 in the request byte is set"));
              Class = 7;
            }
          }
          
          // out buffer
          // Header is four bytes in length
          r->buf_len = (int) out_len;
          out_len = (out_len >= 4) ? (out_len - 4) : 0; // count of bytes to follow the header
          outbuf[0] = ((out_len >> 8) & 0xFF); // this is the out_len (the count of bytes sent to the guest) (big-endian)
          outbuf[1] = ((out_len >> 0) & 0xFF);
          outbuf[2] = 0x00 | Class; // bit 7 = 0, bits 6:3 = resv, bits 2:0 = class returned
          outbuf[3] = EVENT_STATUS_MEDIA_REQ;  // supported Event Classes (only event class 4 supported at this time)
        } else { // polled bit
          // we don't (currently) support asynchronous operation.
          // be sure to modify 'CONFIG_ASYNC' if we support ASYNC
          _sense = SENSE_ILLEGAL_REQUEST;
          _asc = 0x24; // Invalid Field in CDB
          goto fail;
        }
      } else {
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x20; // Invalid Command Opertation code
        goto fail;
      }
      break;
    case 0x51:  // SCSI_CD_READ_DISC_INFO
      if (type == SCSIDEV_TYPE_CDROM) {
        // This code is experimental. may need some help
        BX_DEBUG(("Read Disc Information: Data type requested 0x%02X", buf[1] & 7));
        
        switch (buf[1] & DISC_INFO_MASK) {
          // MMC-6r2f, section 6.21.4, page 379(427) ??
          case DISC_INFO_STANDARD: // Standard Disk Information
            outbuf[ 0] = (32 * (0 * 8)) >> 8; // msb length 32 + (8 * number of OPC tables)
            outbuf[ 1] = (32 * (0 * 8)) >> 0; // lsb length
            // Type = 0, Eraseable = 0, State of last session = 3 (Complete), Disc status = 2 (Finalized Disc)
            outbuf[ 2] = (DISC_INFO_STANDARD << 5) | DI_STAND_N_ERASABLE | DI_STAND_LAST_STATE_COMP | DI_STAND_STATUS_FINAL;
            outbuf[ 3] = 1; // number of first track on disc (1 based)
            outbuf[ 4] = 1; // number of sessions (LSB)
            outbuf[ 5] = 1; // first track number in last session (LSB)
            outbuf[ 6] = 1; // last track number in last session (LSB)
            // DID_V, DBC_V, URU, DAC_V, R, Legacy?, BG Format Status
            outbuf[ 7] = DI_STAND_DID_N_VALID | DI_STAND_DBC_N_VALID | DI_STAND_URU_OKAY | DI_STAND_DAC_N_VALID | DI_STAND_N_LEGACY | DI_STAND_BG_FORMAT_0;
            outbuf[ 8] = DI_STAND_DISC_TYPE_0; // Disc Type ( 0 = CD-ROM)
            outbuf[ 9] = 0; // number of sessions (MSB)
            outbuf[10] = 0; // first track number in last session (MSB)
            outbuf[11] = 0; // last track number in last session (MSB)
            
            outbuf[12] = 0; // disk identification (only when DID_V bit is set)
            outbuf[13] = 0; // 
            outbuf[14] = 0; // 
            outbuf[15] = 0; // 
            
            outbuf[16] = 0; // Last Session Lead-in Start Address
            outbuf[17] = 0; // 
            outbuf[18] = 0; // 
            outbuf[19] = 0; // 
            
            outbuf[20] = 0; // Last Session Lead-out Start Address
            outbuf[21] = 0; // 
            outbuf[22] = 0; // 
            outbuf[23] = 0; // 
            
            outbuf[24] = 0; // Disc bar code (only when DBC_V bit is set)
            outbuf[25] = 0; // 
            outbuf[26] = 0; // 
            outbuf[27] = 0; // 
            outbuf[28] = 0; // 
            outbuf[29] = 0; // 
            outbuf[30] = 0; // 
            outbuf[31] = 0; // 
            
            outbuf[32] = 0; // disc application code (only when DAC_V bit is set)
            
            outbuf[33] = 0; // number of OPC Table entries
            
            // OPC Table entries = 0
            
            // return length
            r->buf_len = (2 + 32) + (0 * 8); // (len word + sizeof(information)) + (OPC Table Entry count * Size of entry)
            break;
          case DISC_INFO_TRACK: // Track Resources Information
            // I am not absolutely sure these are the correct values.
            outbuf[ 0] =  0; // msb length
            outbuf[ 1] = 10; // lsb length
            outbuf[ 2] = DISC_INFO_TRACK | (0 << 0); // Type = 1, reserved
            outbuf[ 3] = 0; // reserved
            outbuf[ 4] = ((7927 >> 8) & 0xFF); // maximum possible tracks on disc (MSB)
            outbuf[ 5] = ((7927 >> 0) & 0xFF); // maximum possible tracks on disc (LSB)
            outbuf[ 6] = ((7927 >> 8) & 0xFF); // number of tracks on disc (MSB)
            outbuf[ 7] = ((7927 >> 0) & 0xFF); // number of tracks on disc (LSB)
            outbuf[ 8] = ((99 >> 8) & 0xFF); // Maximum possible number of appendable tracks on disc (MSB)
            outbuf[ 9] = ((99 >> 0) & 0xFF); // Maximum possible number of appendable tracks on disc (LSB)
            outbuf[10] = ((99 >> 8) & 0xFF); // Current number of appendable tracks on disc (MSB)
            outbuf[11] = ((99 >> 0) & 0xFF); // Current number of appendable tracks on disc (LSB)
            
            // return length
            r->buf_len = 12;
            break;
          case DISC_INFO_POW_RES: // POW Resources Information
            
            // unsupported, so return 0 bytes
            
            // return length
            r->buf_len = 0;
            break;
          default:
            _sense = SENSE_ILLEGAL_REQUEST;
            _asc = 0x24; // Invalid Field in CDB
            goto fail;
        }
      } else {
        // this is XPWRITE(10) in SBC-3r25 (page 188)
        
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x20; // Invalid Command Opertation code
        goto fail;
      }
      break;
    case 0x25:
      BX_DEBUG(("Read Capacity (" FMT_LL "d  %d)", max_lba, block_size));
      len = 8; // the Read Capacity command does not provide an 'Allocated Length' field
      // The normal LEN field for this command is zero
      // Returned value is the address of the last sector.
      if (max_lba > 0) {
        outbuf[0] = (Bit8u)((max_lba >> 24) & 0xff);
        outbuf[1] = (Bit8u)((max_lba >> 16) & 0xff);
        outbuf[2] = (Bit8u)((max_lba >> 8) & 0xff);
        outbuf[3] = (Bit8u) (max_lba & 0xff);
        outbuf[4] = (Bit8u) ((block_size >> 24) & 0xff);
        outbuf[5] = (Bit8u) ((block_size >> 16) & 0xff);
        outbuf[6] = (Bit8u) ((block_size >> 8) & 0xff);
        outbuf[7] = (Bit8u)  (block_size & 0xff);
        r->buf_len = 8;
      } else
        goto notready;
      break;
    case 0x08:
    case 0x28:
    case 0x88:
      BX_DEBUG(("Read (sector " FMT_LL "d, count %d)", lba, len));
      if (!inserted)
        goto notready;
      if (lba > max_lba)
        goto illegal_lba;
      r->sector = lba;
      r->sector_count = len;
      if (async) {
        r->seek_pending = 2;
      }
      r->async_mode = async;
      break;
    case 0x0a:
    case 0x2a:
    case 0x8a:
      BX_DEBUG(("Write (sector " FMT_LL "d, count %d)", lba, len));
      if (lba > max_lba)
        goto illegal_lba;
      r->sector = lba;
      r->sector_count = len;
      r->write_cmd = 1;
      if (async) {
        r->seek_pending = 2;
      }
      r->async_mode = async;
      break;
    case 0x35:
    case 0x91:
      BX_DEBUG(("Synchronise cache (sector " FMT_LL "d, count %d)", lba, len));
      // TODO: flush cache
      break;
    case 0x43:
      if (type == SCSIDEV_TYPE_CDROM) {
        if (!inserted)
          goto notready;
        int toclen = 0;
        int msf = buf[1] & 2;
        int format = buf[2] & 0xf;
        int start_track = buf[6];
        BX_DEBUG(("Read TOC (track %d format %d msf %d)", start_track, format, msf >> 1));
        cdrom->read_toc(outbuf, &toclen, msf, start_track, format);
        if (toclen > 0) {
          if (len > toclen)
            len = toclen;
          r->buf_len = len;
        } else {
          BX_ERROR(("Read TOC error"));
          _sense = SENSE_ILLEGAL_REQUEST;
          _asc = 0x24; // Invalid Field in CDB
          goto fail;
        }
      } else {
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x20; // Invalid Command Opertation code
        goto fail;
      }
      break;
    case 0x46:
      if (type == SCSIDEV_TYPE_CDROM) {
        // starting feature number
        Bit16u start_feature = ((Bit16u) buf[2] << 8) | buf[3];
        // if rt = 0, then all supported features starting with 'start_feature' are returned
        // if rt = 1, then all supported features starting with 'start_feature' that have the 'current' bit set, are returned.
        // if rt = 2, then only the 'start_feature' feature (if supported) will be returned, else only the 8-byte header
        int rt = buf[1] & 3;
        
        BX_DEBUG(("Get Configuration (start 0x%04X, rt %d, maxlen %d)", start_feature, rt, len));
        
        Bit8u *p = outbuf;
        int byte_count = 0;
        
        // Header is 8 bytes
        // bytes 0 -> 3 are updated below
        outbuf[ 4] = 0;
        outbuf[ 5] = 0;
        outbuf[ 6] = (CONFIG_PROFILE_CDROM >> 8) & 0xFF;
        outbuf[ 7] = (CONFIG_PROFILE_CDROM >> 0) & 0xFF;
        byte_count += 8; // add the size of this header
          
        // Profile List Feature (0x0000) follows the header (offset 8)
        // MMC-6r2f, section 5.3.1, page 198(246)
        if (((rt < 2) && (start_feature == CONFIG_FEATURE_0000)) ||
            ((rt == 2) && (start_feature == CONFIG_FEATURE_0000))) {
          p = &outbuf[byte_count];
          p[ 0] = (CONFIG_FEATURE_0000 >> 8) & 0xFF;
          p[ 1] = (CONFIG_FEATURE_0000 >> 0) & 0xFF;
          p[ 2] = CONFIG_VERSION(0) | CONFIG_PERSISTENT(1) | CONFIG_CURRENT(1);
          p[ 3] = 1 * 4;  // count of profiles we support times 4 bytes each (only one, a CD-ROM)
          // first and only profile
          p[ 4] = 0;
          p[ 5] = CONFIG_PROFILE_CDROM;
          p[ 6] = CONFIG_CURRENT(inserted);  // if CD-ROM is not inserted, this must be 0
          p[ 7] = 0;
          // if any more profiles exist, they would go here
          byte_count += 8; // add the size of this feature
        }
          
        if (inserted) {
          // Core Feature (0x0001) follows
          // MMC-6r2f, section 5.3.2, page 201(249)
          if (((rt < 2) && (start_feature <= CONFIG_FEATURE_0001)) ||
              ((rt == 2) && (start_feature == CONFIG_FEATURE_0001))) {
            p = &outbuf[byte_count];
            p[ 0] = (CONFIG_FEATURE_0001 >> 8) & 0xFF;
            p[ 1] = (CONFIG_FEATURE_0001 >> 0) & 0xFF;
            p[ 2] = CONFIG_VERSION(2) | CONFIG_PERSISTENT(1) | CONFIG_CURRENT(1);
            p[ 3] = 8;  // additional length
            p[ 4] = (CONFIG_PHY_INT_STND >> 24) & 0xFF;  // Physical Interface Standard (MSB)
            p[ 5] = (CONFIG_PHY_INT_STND >> 16) & 0xFF;  // 
            p[ 6] = (CONFIG_PHY_INT_STND >>  8) & 0xFF;  // 
            p[ 7] = (CONFIG_PHY_INT_STND >>  0) & 0xFF;  // Physical Interface Standard (LSB)
            p[ 8] = CONFIG_INQ2 | CONFIG_DBE;
            p[ 9] = 0;
            p[10] = 0;
            p[11] = 0;
            byte_count += 12; // add the size of this feature
          }
          
          // Morphing Feature (0x0002) follows
          // MMC-6r2f, section 5.3.3, page 203(251)
          if (((rt < 2) && (start_feature <= CONFIG_FEATURE_0002)) ||
              ((rt == 2) && (start_feature == CONFIG_FEATURE_0002))) {
            p = &outbuf[byte_count];
            p[ 0] = (CONFIG_FEATURE_0002 >> 8) & 0xFF;
            p[ 1] = (CONFIG_FEATURE_0002 >> 0) & 0xFF;
            p[ 2] = CONFIG_VERSION(1) | CONFIG_PERSISTENT(1) | CONFIG_CURRENT(1);
            p[ 3] = 4;  // additional length
            p[ 4] = CONFIG_OCEVENT | CONFIG_ASYNC;
            p[ 5] = 0;
            p[ 6] = 0;
            p[ 7] = 0;
            byte_count += 8; // add the size of this feature
          }
            
          // Removable Medium Feature (0x0003) follows
          // MMC-6r2f, section 5.3.4, page 204(252)
          if (((rt < 2) && (start_feature <= CONFIG_FEATURE_0003)) ||
              ((rt == 2) && (start_feature == CONFIG_FEATURE_0003))) {
            p = &outbuf[byte_count];
            p[ 0] = (CONFIG_FEATURE_0003 >> 8) & 0xFF;
            p[ 1] = (CONFIG_FEATURE_0003 >> 0) & 0xFF;
            p[ 2] = CONFIG_VERSION(2) | CONFIG_PERSISTENT(1) | CONFIG_CURRENT(1);
            p[ 3] = 4;  // additional length
            p[ 4] = CONFIG_LOAD_MECH(CONFIG_LM_TRAY) | CONFIG_LOAD(1) | CONFIG_EJECT(1) |
                      CONFIG_PVNT_JMPR(0) | CONFIG_PVNT_DBML(0) | CONFIG_LOCK(1);
            p[ 5] = 0;
            p[ 6] = 0;
            p[ 7] = 0;
            byte_count += 8; // add the size of this feature
          }
            
          // Random Readable Feature (0x0010) follows
          // MMC-6r2f, section 5.3.6, page 208(256)
          if (((rt < 2) && (start_feature <= CONFIG_FEATURE_0010)) ||
              ((rt == 2) && (start_feature == CONFIG_FEATURE_0010))) {
            p = &outbuf[byte_count];
            p[ 0] = (CONFIG_FEATURE_0010 >> 8) & 0xFF;
            p[ 1] = (CONFIG_FEATURE_0010 >> 0) & 0xFF;
            p[ 2] = CONFIG_VERSION(0) | CONFIG_PERSISTENT(1) | CONFIG_CURRENT(1);
            p[ 3] = 8;  // additional length
            p[ 4] = ((block_size >> 24) & 0xFF);
            p[ 5] = ((block_size >> 16) & 0xFF);
            p[ 6] = ((block_size >>  8) & 0xFF);
            p[ 7] = ((block_size >>  0) & 0xFF);
            p[ 8] = 0; // (MSB) one block unit per block_size
            p[ 9] = 1; // (LSB)
            p[10] = 0; // reserved
            p[11] = 0; // reserved
            byte_count += 12; // add the size of this feature
          }
        }  // if(inserted)
        
        // Data Length field in the header is total bytes following this field
        outbuf[0] = (((byte_count - 4) >> 24) & 0xFF); // MSB
        outbuf[1] = (((byte_count - 4) >> 16) & 0xFF);
        outbuf[2] = (((byte_count - 4) >>  8) & 0xFF);
        outbuf[3] = (((byte_count - 4) >>  0) & 0xFF); // LSB
          
        r->buf_len = byte_count;
      } else {  // if (type == SCSIDEV_TYPE_CDROM)
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x20; // Invalid Command Opertation code
        goto fail;
      }
      break;
    case 0xa0:
      // MMC-6r02f, section 6.29, page 515(563)
      // SPC-4r18, section 6.23, page 336
      BX_INFO(("Report LUNs (len %d)", len));
      memset(outbuf, 0, 16);
      outbuf[3] = 8;  // 8 more bytes after this 8-byte header
      r->buf_len = 16;
      break;
    case 0x2f:
      BX_DEBUG(("Verify (sector " FMT_LL "d, count %d)", lba, len));
      if (lba > max_lba)
        goto illegal_lba;
      if (buf[1] & 2) {
        BX_ERROR(("Verify with ByteChk not implemented yet"));
        _sense = SENSE_ILLEGAL_REQUEST;
        _asc = 0x24; // Invalid Field in CDB
        goto fail;
      }
      break;
    case 0x23:
      // USBMASS-UFI10.pdf  rev 1.0  Section 4.10
      BX_INFO(("READ FORMAT CAPACITIES"));
      
      // Cap List Header
      outbuf[0] = 0;
      outbuf[1] = 0;
      outbuf[2] = 0;
      outbuf[3] = 12;
      
      // Current/Max Cap Header
      // Returned value is the address of the last sector.
      outbuf[ 4] = (Bit8u) ((max_lba >> 24) & 0xFF);
      outbuf[ 5] = (Bit8u) ((max_lba >> 16) & 0xFF);
      outbuf[ 6] = (Bit8u) ((max_lba >>  8) & 0xFF);
      outbuf[ 7] = (Bit8u) ((max_lba >>  0) & 0xFF);
      outbuf[ 8] = 2; // formatted (1 = unformatted)
      outbuf[ 9] = (Bit8u) ((block_size >> 16) & 0xFF);
      outbuf[10] = (Bit8u) ((block_size >>  8) & 0xFF);
      outbuf[11] = (Bit8u) ((block_size >>  0) & 0xFF);
      r->buf_len = 12;
      break;
    // The 0x9E command uses a service action code (ex: 0x9E/0x10)
    case 0x9E:
      switch (buf[1] & 0x1F) { // service action code
        case 0x10: // Read Capacity(16)
          BX_DEBUG(("Read Capacity 16"));
          // Returned value is the address of the last sector.
          if (max_lba) {
            // 64-bit lba
            outbuf[ 0] = (Bit8u)((max_lba >> 56) & 0xff);
            outbuf[ 1] = (Bit8u)((max_lba >> 48) & 0xff);
            outbuf[ 2] = (Bit8u)((max_lba >> 40) & 0xff);
            outbuf[ 3] = (Bit8u)((max_lba >> 32) & 0xff);
            outbuf[ 4] = (Bit8u)((max_lba >> 24) & 0xff);
            outbuf[ 5] = (Bit8u)((max_lba >> 16) & 0xff);
            outbuf[ 6] = (Bit8u)((max_lba >> 8) & 0xff);
            outbuf[ 7] = (Bit8u) (max_lba & 0xff);
            // 32-bit block size
            outbuf[ 8] = (Bit8u) ((block_size >> 24) & 0xff);
            outbuf[ 9] = (Bit8u) ((block_size >> 16) & 0xff);
            outbuf[10] = (Bit8u) ((block_size >> 8) & 0xff);
            outbuf[11] = (Bit8u)  (block_size & 0xff);
            // protection
            outbuf[12] = 0;
            // exponent/one or more physical blocks per logical block
            outbuf[13] = 0;
            // lowest aligned logical block address
            outbuf[14] = 0;
            outbuf[15] = 0;
            // bytes 16 through 31 are reserved
            memset(&outbuf[16], 0, 16);
            r->buf_len = 32;
          } else
            goto notready;
          break;
        default:
          BX_ERROR(("Unknown SCSI command (0x9E/%02X)", buf[1] & 0x1F));
      }
      break;
    default:
      _sense = SENSE_ILLEGAL_REQUEST;
      _asc = 0x20; // Invalid Command Opertation code
      BX_ERROR(("Unknown SCSI command (0x%02X)", buf[0]));
    fail:
      // Check Condition / Illegal Request / Invalid Field in CDB
      scsi_command_complete(r, STATUS_CHECK_CONDITION, _sense, _asc, _ascq);
      return 0;
    notready:
      scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_NOT_READY, _asc, _ascq);
      return 0;
    illegal_lba:
      scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR, _asc, _ascq);
      return 0;
  }
  
  // given command was processed successfully, so continue
  
  // don't send more than asked for
  if (r->buf_len > len)
    r->buf_len = len;
  
  // if no data to transfer, signal complete
  if (r->sector_count == 0 && r->buf_len == 0) {
    scsi_command_complete(r, STATUS_GOOD, SENSE_NO_SENSE, 0, 0);
  }
  // else, signal data transfer
  len = r->sector_count * block_size + r->buf_len;
  if (r->write_cmd) {
    return -len;
  } else {
    if (!r->sector_count)
      r->sector_count = (Bit32u) -1;
    return len;
  }
}

// len = size of data *after* this header
int scsi_device_t::scsi_do_modepage_hdr(Bit8u *p, Bit8u sub_page, Bit8u page_code, int len) {
  if (!sub_page) {
    p[0] = PAGE_SAVEABLE(0) | PAGE_SPF(0) | page_code;
    p[1] = (Bit8u) len;
    return 2;
  } else {
    p[0] = PAGE_SAVEABLE(0) | PAGE_SPF(1) | page_code;
    p[1] = sub_page; // subpage_code
    p[2] = (len >> 8) & 0xFF;  // length of remaining items (MSB)
    p[3] = (len >> 0) & 0xFF;  // length of remaining items (LSB)
    return 4;
  }
}

// create a page for specified page code
//  if subpage = 0x00, return all in page_0 format
//  if subpage = 0xFF, return all (except page 0) in sub_page format
// if page_code not supported, return 0
int scsi_device_t::scsi_do_modepage(Bit8u *p, Bit8u pc, Bit8u sub_page, Bit8u page_code) {
  int size = 0;

  switch (page_code) {
    case PAGE_VENDOR_SPECIFIC:  // (always in page_0 format)
      // except for the header, the format of the remaining
      //  bytes is unspecified, specific to the vendor
      size = scsi_do_modepage_hdr(p, 0, page_code, 2);
      p += size;
      // QEMU says that there is a quirk with an Apple(TM) product
      //  that needs these values.
      if (pc == PAGE_CONTROL_CHANGEABLE) {
        p[0] = 0xFF;
        p[1] = 0xFF;
      } else {
        p[0] = 0x00;
        p[1] = 0x00;
      }
      size += 2;
      break;
      
    case PAGE_ERROR_RECOVERY:  // error recovery page
      size = scsi_do_modepage_hdr(p, sub_page, page_code, 10);
      p += size;
      if (pc == PAGE_CONTROL_CHANGEABLE) {
        memset(p, 0, 10);
      } else {
        if (type == SCSIDEV_TYPE_DISK) {
          p[ 0] = 0x80;  // AWRE, ARRE, TB, RC, EER, PER, DTE, DCR
          p[ 1] = 0x20;  // retry count
          p[ 2] = 0x00;  // correction span in bits
          p[ 3] = 0x00;  // head offset count
          p[ 4] = 0x00;  // data strobe offset count
          p[ 5] = 0x00;  // reserved
          p[ 6] = 0x00;  // write retry count
          p[ 7] = 0x00;  // reserved
          p[ 8] = (0 >>  8) & 0xFF;  // recovery time limit (MSB)
          p[ 9] = (0 >>  0) & 0xFF;  // recovery time limit (LSB)
        } else {  // SCSIDEV_TYPE_CDROM
          p[ 0] = 0x80;  // AWRE, ARRE, TB, RC, EER, PER, DTE, DCR
          p[ 1] = 0x20;  // retry count
          p[ 2] = 0x00;  // reserved
          p[ 3] = 0x00;  // reserved
          p[ 4] = 0x00;  // reserved
          p[ 5] = 0x00;  // EMCDR
          p[ 6] = 0x00;  // write retry count
          p[ 7] = (0 >> 16) & 0xFF;  // recovery time limit (MSB)
          p[ 8] = (0 >>  8) & 0xFF;  // recovery time limit
          p[ 9] = (0 >>  0) & 0xFF;  // recovery time limit (LSB)
        }
      }
      size += 10;
      break;
      
    case PAGE_HD_GEOMETRY:  // rigid disk geometry page (hard drives)
      if (type == SCSIDEV_TYPE_DISK) {
        size = scsi_do_modepage_hdr(p, sub_page, page_code, 22);
        p += size;
        if (pc == PAGE_CONTROL_CHANGEABLE) {
          memset(p, 0, 22);
        } else {
          p[ 0] = (hdimage->cylinders >> 16) & 0xFF;  // number of cylinders (MSB)
          p[ 1] = (hdimage->cylinders >>  8) & 0xFF;  // number of cylinders
          p[ 2] = (hdimage->cylinders >>  0) & 0xFF;  // number of cylinders (LSB)
          p[ 3] = 16;                   // number of heads
          p[ 4] = (hdimage->cylinders >> 16) & 0xFF;  // starting cylinder-write precomp (MSB) (same as num/cyls above)
          p[ 5] = (hdimage->cylinders >>  8) & 0xFF;  // starting cylinder-write precomp
          p[ 6] = (hdimage->cylinders >>  0) & 0xFF;  // starting cylinder-write precomp (LSB)
          p[ 7] = (hdimage->cylinders >> 16) & 0xFF;  // starting cylinder-reduced write (MSB) (same as num/cyls above)
          p[ 8] = (hdimage->cylinders >>  8) & 0xFF;  // starting cylinder-reduced write
          p[ 9] = (hdimage->cylinders >>  0) & 0xFF;  // starting cylinder-reduced write (LSB)
          p[10] = 0;                    // drive step rate (MSB)
          p[11] = 200;                  // drive step rate (LSB)
          p[12] = 0xFF;                 // landing zone cylinder (MSB)
          p[13] = 0xFF;                 // landing zone cylinder
          p[14] = 0xFF;                 // landing zone cylinder (LSB)
          p[15] = 0x00;                 // RPL
          p[16] = 0x00;                 // rotational offset
          p[17] = 0x00;                 // reserved
          p[18] = (5400 >>  8) & 0xFF;  // media rotation rate (MSB)  // 5400 to 7200 is common
          p[19] = (5400 >>  0) & 0xFF;  // media rotation rate (LSB)
          p[20] = 0x00;                 // reserved
          p[21] = 0x00;                 // reserved
        }
        size += 22;
      }
      break;
      
  //case PAGE_WRITE_PARAMETERS:    // cdrom: write parameters
    case PAGE_FLEXIBILE_GEOMETRY:  // disk: flexible disk geometry page
      if (type == SCSIDEV_TYPE_DISK) {  // PAGE_FLEXIBILE_GEOMETRY
        size = scsi_do_modepage_hdr(p, sub_page, page_code, 30);
        p += size;
        if (pc == PAGE_CONTROL_CHANGEABLE) {
          memset(p, 0, 30);
        } else {
          p[ 0] = (5000 >> 8) & 0xFF;   // transfer rate (MSB)  // 5000, 2000, 1000, 500, 300, or 250
          p[ 1] = (5000 >> 0) & 0xFF;   // transfer rate (LSB)  
          p[ 2] = 2;                    // number of heads
          p[ 3] = 18;                   // sectors per track
          p[ 4] = (block_size >> 8) & 0xFF; // bytes per sector (MSB)
          p[ 5] = (block_size >> 0) & 0xFF; // bytes per sector (LSB)
          p[ 6] = (hdimage->cylinders >> 8) & 0xFF;   // number of cylinders (MSB)
          p[ 7] = (hdimage->cylinders >> 0) & 0xFF;   // number of cylinders (LSB)
          p[ 8] = (hdimage->cylinders >> 16) & 0xFF;  // starting cylinder-write precomp (MSB) (same as num/cyls above to disable)
          p[ 9] = (hdimage->cylinders >>  0) & 0xFF;  // starting cylinder-write precomp (LSB)
          p[10] = (hdimage->cylinders >> 16) & 0xFF;  // starting cylinder-reduced write (MSB) (same as num/cyls above to disable)
          p[11] = (hdimage->cylinders >>  0) & 0xFF;  // starting cylinder-reduced write (LSB)
          p[12] = 0;                    // drive step rate (MSB) (in 100uS units)
          p[13] = 1;                    // drive step rate (LSB)
          p[14] = 1;                    // drive step pulse (in 1uS units)
          p[15] = 0;                    // head settle delay (MSB) (in 100us units)
          p[16] = 1;                    // head settle delay (LSB)
          p[17] = 200;                  // motor on delay  (in 10ths of a second) (200 = 2 seconds)
          p[18] = 1;                    // motor off delay
          p[19] = (1 << 6);             // TRDY = 0,  SSN = 1,  MO = 0
          p[20] = 0x00;                 // SPC
          p[21] = 0x00;                 // write compensation
          p[22] = 0x00;                 // head load delay (in mS) (0 = use default setting)
          p[23] = 0x00;                 // head unload delay (in mS) (0 = use default setting)
          p[24] = 0;                    // Pin 34   Pin 2
          p[25] = 0;                    // Pin 4    Pin 1
          p[26] = (300 >>  8) & 0xFF;   // media rotation rate (MSB) (rotations per minute)
          p[27] = (300 >>  0) & 0xFF;   // media rotation rate (LSB) (300 for 1.44M, 360 for 1.22M)
          p[28] = 0x00;                 // reserved
          p[29] = 0x00;                 // reserved
        }
        size += 30;
        
      } else { // CDROM: PAGE_WRITE_PARAMETERS
        size = scsi_do_modepage_hdr(p, sub_page, page_code, 50);
        p += size;
        if (pc == PAGE_CONTROL_CHANGEABLE) {
          memset(p, 0, 50);
        } else {
          p[ 0] = 0x00;                 // BUFE, LS_V, Test Write, Write Type
          p[ 1] = 0x00;                 // Multi-session, FP, Copy, Track Mode
          p[ 2] = 0x08;                 // data block type (8 = Mode 1, 2048 block size)
          p[ 3] = 0x00;                 // link size (valid only if LS_V = 1, and type = "packet/incremental")
          p[ 4] = 0x00;                 // reserved
          p[ 5] = 0x00;                 // host application code
          p[ 6] = 0x00;                 // session format (CD-DA or CD-ROM discs)
          p[ 7] = 0x00;                 // reserved
          p[ 8] = (0    >> 24) & 0xFF;  // packet size (MSB) (valid only if FP = 1)
          p[ 9] = (0    >> 16) & 0xFF;  // packet size
          p[10] = (0    >>  8) & 0xFF;  // packet size
          p[11] = (0    >>  0) & 0xFF;  // packet size (LSB)
          p[12] = (0    >>  8) & 0xFF;  // audio pause length (MSB)
          p[13] = (0    >>  0) & 0xFF;  // audio pause length (LSB)
          memset(&p[14], 0, 16);        // media catalog number (only valid for writable CD media)
          memset(&p[30], 0, 16);        // international standard recording code (only valid for writable CD media)
          p[46] = 0x00;                 // sub-header byte 0
          p[47] = 0x00;                 // sub-header byte 1
          p[48] = 0x00;                 // sub-header byte 2
          p[49] = 0x00;                 // sub-header byte 3
        }
        size += 50;
      }
      break;
      
    // MMC-6r02f, section 7.5, page 615(663)
    // SBC-3r25, section 6.4.5, page 217
    case PAGE_CACHING:  // caching page
      if (type == SCSIDEV_TYPE_DISK) {
        size = scsi_do_modepage_hdr(p, sub_page, page_code, 18);
        p += size;
        if (pc == PAGE_CONTROL_CHANGEABLE) {
          memset(p, 0, 18);
        } else {
          p[ 0] = (1<<2) | (0<<0);  // WCE = 1 allow write cache, RCD = 0 allow read cache
          p[ 1] = 0; // Demand read retention priority, write retention priority
          p[ 2] = 0; // Disable Pre-fetch transfer length (MSB)
          p[ 3] = 0; // Disable Pre-fetch transfer length (LSB)
          p[ 4] = 0; // Minimum Pre-fetch (MSB)
          p[ 5] = 0; // Minimum Pre-fetch (LSB)
          p[ 6] = 0; // Maximum Pre-fetch (MSB)
          p[ 7] = 0; // Maximum Pre-fetch (LSB)
          p[ 8] = 0; // Maximum Pre-fetch Ceiling (MSB)
          p[ 9] = 0; // Maximum Pre-fetch Ceiling (LSB)
          p[10] = 0; // fsw, lbcss, dra, nv_dis
          p[11] = 0; // Number of cache segments
          p[12] = 0; // Cache Segment Size (MSB)
          p[13] = 0; // Cache Segment Size (LSB)
          p[14] = 0; // reserved
          p[15] = 0; // obsolete
          p[16] = 0; // obsolete
          p[17] = 0; // obsolete
        }
      } else {
        // SCSIDEV_TYPE_CDROM
        size = scsi_do_modepage_hdr(p, sub_page, page_code, 10);
        p += size;
        if (pc == PAGE_CONTROL_CHANGEABLE) {
          memset(p, 0, 10);
        } else {
          memset(p, 0, 10);
          p[0] = (1<<2) | (0<<0);  // WCE = 1 allow write cache, RCD = 0 allow read cache
        }
        size += 10;
      }
      break;
      
    case PAGE_POWER_CONDITION: // power condition page
      size = scsi_do_modepage_hdr(p, sub_page, page_code, 38);
      p += size;
      if (pc == PAGE_CONTROL_CHANGEABLE) {
        memset(p, 0, 38);
      } else {
        p[ 0] = 0x00;                  // STANDBY_Y
        p[ 1] = 0x00;                  // IDLE_C, IDLE_B, IDLE_A, STANDBY_Z
        p[ 2] = (0x00 >> 24) & 0xFF;   // IDLE_A condition timer (MSB) (valid if IDLE_A = 1)
        p[ 3] = (0x00 >> 16) & 0xFF;   // IDLE_A condition timer    (100 mS increments)
        p[ 4] = (0x00 >>  8) & 0xFF;   // IDLE_A condition timer
        p[ 5] = (0x00 >>  0) & 0xFF;   // IDLE_A condition timer (LSB)
        p[ 6] = (0x00 >> 24) & 0xFF;   // STANDBY_Z condition timer (MSB) (valid if STANDBY_Z = 1)
        p[ 7] = (0x00 >> 16) & 0xFF;   // STANDBY_Z condition timer    (100 mS increments)
        p[ 8] = (0x00 >>  8) & 0xFF;   // STANDBY_Z condition timer
        p[ 9] = (0x00 >>  0) & 0xFF;   // STANDBY_Z condition timer (LSB)
        p[10] = (0x00 >> 24) & 0xFF;   // IDLE_B condition timer (MSB) (valid if IDLE_B = 1)
        p[11] = (0x00 >> 16) & 0xFF;   // IDLE_B condition timer    (100 mS increments)
        p[12] = (0x00 >>  8) & 0xFF;   // IDLE_B condition timer
        p[13] = (0x00 >>  0) & 0xFF;   // IDLE_B condition timer (LSB)
        p[14] = (0x00 >> 24) & 0xFF;   // IDLE_C condition timer (MSB) (valid if IDLE_C = 1)
        p[15] = (0x00 >> 16) & 0xFF;   // IDLE_C condition timer    (100 mS increments)
        p[16] = (0x00 >>  8) & 0xFF;   // IDLE_C condition timer
        p[17] = (0x00 >>  0) & 0xFF;   // IDLE_C condition timer (LSB)
        p[18] = (0x00 >> 24) & 0xFF;   // STANDBY_Y condition timer (MSB) (valid if STANDBY_Y = 1)
        p[19] = (0x00 >> 16) & 0xFF;   // STANDBY_Y condition timer    (100 mS increments)
        p[20] = (0x00 >>  8) & 0xFF;   // STANDBY_Y condition timer
        p[21] = (0x00 >>  0) & 0xFF;   // STANDBY_Y condition timer (LSB)
        memset(&p[22], 0, 16);         // reserved
      }
      size += 38;
      break;
      
    case PAGE_EXCEPTION_CONTROL: // Information Exceptions Control page
      size = scsi_do_modepage_hdr(p, sub_page, page_code, 10);
      p += size;
      if (pc == PAGE_CONTROL_CHANGEABLE) {
        memset(p, 0, 10);
      } else {
        p[ 0] = 0;                    // PERF  EBF  EWASC  DEXCPT  TEST  EBACKERR  LOGERR
        p[ 1] = 0;                    // MRIE
        p[ 2] = (0  >> 24) & 0xFF;    // interval timer (MSB)
        p[ 3] = (0  >> 16) & 0xFF;    // interval timer   (0x00000000 = vendor specific)
        p[ 4] = (0  >>  8) & 0xFF;    // interval timer
        p[ 5] = (0  >>  0) & 0xFF;    // interval timer (LSB)
        p[ 6] = (0  >> 24) & 0xFF;    // report count (MSB)
        p[ 7] = (0  >> 16) & 0xFF;    // report count   (0x00000000 = no limit)
        p[ 8] = (0  >>  8) & 0xFF;    // report count
        p[ 9] = (0  >>  0) & 0xFF;    // report count (LSB)
      }
      size += 10;
      break;
      
    case PAGE_CDVD_INACTIVITY: // CD-ROM inactivity page
      if (type == SCSIDEV_TYPE_CDROM) {
        size = scsi_do_modepage_hdr(p, sub_page, page_code, 10);
        p += size;
        if (pc == PAGE_CONTROL_CHANGEABLE) {
          memset(p, 0, 10);
        } else {
          p[ 0] = 0;                    // reserved
          p[ 1] = 0;                    // reserved
          p[ 2] = 0;                    // G3Enable  TMOE  DISP  SWPP
          p[ 3] = 0;                    // reserved
          p[ 4] = (0  >>  8) & 0xFF;    // Group 1 Min Timeout (seconds) (MSB) (valid only if TMOE = 1)
          p[ 5] = (0  >>  0) & 0xFF;    // Group 1 Min Timeout (seconds) (LSB)
          p[ 6] = (0  >>  8) & 0xFF;    // Group 2 Min Timeout (seconds) (MSB)
          p[ 7] = (0  >>  0) & 0xFF;    // Group 2 Min Timeout (seconds) (LSB)
          p[ 8] = (0  >>  8) & 0xFF;    // Group 3 Timeout (milliseconds) (MSB) (valid only if G3Enable = 1)
          p[ 9] = (0  >>  0) & 0xFF;    // Group 3 Timeout (milliseconds) (LSB)
        }
        size += 10;
      }
      break;
      
    // MMC3r10g, section 6.3.11, page 311(347)
    case PAGE_CAPABILITIES:  // capabilities page
      if (type == SCSIDEV_TYPE_CDROM) {
        size = scsi_do_modepage_hdr(p, sub_page, page_code, 24);
        p += size;
        if (pc == PAGE_CONTROL_CHANGEABLE) {
          memset(p, 0, 24);
        } else {
          p[ 0] = 0x03;                 // CD-R or CD-RW read/only
          p[ 1] = 0x00;                 // not writable
          p[ 2] = 0x7F;                 // Audio, composite, digital out, mode 2, form 1&2, multi session
          p[ 3] = 0xFF;                 // CD DA, DA accurate, RW supported, RW corrected, C2 error, ISRC, UPC, bar code
          p[ 4] = 0x2D | (locked ? 2: 0);   // locking supported, jumper present, eject, type = tray
          p[ 5] = 0x00;                 // no volume & mute control, no changer
          p[ 6] = 0x00; // obsolete/reserved  // ((50 * 176) >> 8) & 0xFF; // 50x read speed (MSB)
          p[ 7] = 0x00; // obsolete/reserved  // ((50 * 176) >> 0) & 0xFF; // 50x read speed (LSB)
          p[ 8] = (0 >> 8) & 0xFF;      // no volume controls (MSB)
          p[ 9] = (0 >> 0) & 0xFF;      // no volume controls (LSB)
          p[10] = (2048 >> 16) & 0xFF;  // 2Meg buffer (MSB)
          p[11] = (2048 >>  0) & 0xFF;  // 2Meg buffer (LSB)
          p[12] = 0x00; // obsolete/reserved  // ((16 * 176) >> 8) & 0xFF; // 16x read speed current (MSB)
          p[13] = 0x00; // obsolete/reserved  // ((16 * 176) >> 0) & 0xFF; // 16x read speed current (LSB)
          p[14] = 0x00;                 // reserved
          p[15] = 0x00;                 // Length, LSBF, RCK, BCKF
          p[16] = 0x00;                 // obsolete/reserved
          p[17] = 0x00;                 // obsolete/reserved
          p[18] = 0x00;                 // obsolete/reserved
          p[19] = 0x00;                 // obsolete/reserved
          p[20] = 0x00;                 // copy management revision supported (MSB)
          p[21] = 0x00;                 // copy management revision supported (LSB)
          p[22] = 0x00;                 // reserved
          p[23] = 0x00;                 // reserved
        }
        size += 24;
      }
      break;
  }

  return size;
}

void scsi_device_t::set_inserted(bool value)
{
  inserted = value;
  if (inserted) {
    max_lba = cdrom->capacity() - 1;
    curr_lba = max_lba;
  } else {
    max_lba = 0;
  }
}

void scsi_device_t::start_seek(SCSIRequest *r)
{
  Bit64s new_pos, prev_pos, max_pos;
  Bit32u seek_time;
  double fSeekBase, fSeekTime;

  max_pos = max_lba;
  prev_pos = curr_lba;
  new_pos = r->sector;
  if (type == SCSIDEV_TYPE_CDROM) {
    fSeekBase = 80000.0;
  } else {
    fSeekBase = 5000.0;
  }
  fSeekTime = fSeekBase * (double)abs((int)(new_pos - prev_pos + 1)) / (max_pos + 1);
  seek_time = 4000 + (Bit32u)fSeekTime;
  bx_pc_system.activate_timer(seek_timer_index, seek_time, 0);
  bx_pc_system.setTimerParam(seek_timer_index, r->tag);
  r->seek_pending = 1;
}

void scsi_device_t::seek_timer_handler(void *this_ptr)
{
  scsi_device_t *class_ptr = (scsi_device_t *) this_ptr;
  class_ptr->seek_timer();
}

void scsi_device_t::seek_timer()
{
  Bit32u tag = bx_pc_system.triggeredTimerParam();
  SCSIRequest *r = scsi_find_request(tag);

  seek_complete(r);
}

void scsi_device_t::seek_complete(SCSIRequest *r)
{
  Bit32u i, n;
  int ret = 0;

  r->seek_pending = 0;
  if (!r->write_cmd) {
    bx_gui->statusbar_setitem(statusbar_id, 1);
    n = r->sector_count;
    if (n > (Bit32u) (SCSI_DMA_BUF_SIZE / block_size))
      n = SCSI_DMA_BUF_SIZE / block_size;
    r->buf_len = n * block_size;
    if (type == SCSIDEV_TYPE_CDROM) {
      i = 0;
      do {
        ret = (int) cdrom->read_block(r->dma_buf + (i * 2048), (Bit32u) (r->sector + i), 2048);
      } while ((++i < n) && (ret == 1));
      if (ret == 0) {
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_MEDIUM_ERROR, 0, 0);
        return;
      }
    } else {
      ret = (int) hdimage->lseek(r->sector * block_size, SEEK_SET);
      if (ret < 0) {
        BX_ERROR(("could not lseek() hard drive image file"));
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR, 0, 0);
        return;
      }
      i = 0;
      do {
        ret = (int) hdimage->read((bx_ptr_t) (r->dma_buf + (i * block_size)), block_size);
      } while ((++i < n) && (ret == block_size));
      if (ret != block_size) {
        BX_ERROR(("could not read() hard drive image file"));
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR, 0, 0);
        return;
      }
    }
    r->sector += n;
    r->sector_count -= n;
    scsi_read_complete((void*)r, 0);
  } else {
    bx_gui->statusbar_setitem(statusbar_id, 1, 1);
    n = r->buf_len / block_size;
    if (n) {
      ret = (int)hdimage->lseek(r->sector * block_size, SEEK_SET);
      if (ret < 0) {
        BX_ERROR(("could not lseek() hard drive image file"));
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR, 0, 0);
      }
      i = 0;
      do {
        ret = (int) hdimage->write((bx_ptr_t) (r->dma_buf + (i * block_size)),
                                  block_size);
      } while ((++i < n) && (ret == block_size));
      if (ret != block_size) {
        BX_ERROR(("could not write() hard drive image file"));
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR, 0, 0);
        return;
      }
      r->sector += n;
      r->sector_count -= n;
      scsi_write_complete((void *) r, 0);
    }
  }
}

// Turn on BX_DEBUG messages at connection time
void scsi_device_t::set_debug_mode()
{
  setonoff(LOGLEV_DEBUG, ACT_REPORT);
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
