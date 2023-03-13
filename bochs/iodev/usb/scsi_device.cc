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
  tcq = _tcq;
  completion = _completion;
  dev = _dev;
  block_size = hdimage->sect_size;
  locked = 0;
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
  tcq = _tcq;
  completion = _completion;
  dev = _dev;
  block_size = 2048;
  locked = 0;
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
  BXRS_PARAM_BOOL(list, locked, locked);
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

void scsi_device_t::scsi_command_complete(SCSIRequest *r, int status, int _sense)
{
  Bit32u tag;
  BX_DEBUG(("command complete tag=0x%x status=%d sense=%d", r->tag, status, sense));
  sense = _sense;
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
    scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_NO_SENSE);
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
    scsi_command_complete(r, STATUS_GOOD, SENSE_NO_SENSE);
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
    scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR);
    return;
  }

  if (r->sector_count == 0) {
    scsi_command_complete(r, STATUS_GOOD, SENSE_NO_SENSE);
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
    scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR);
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
        goto fail;
  }

  // check that the expected command length matches the sent command length.
  // some hardware may fail if the command length byte isn't correct.
  if (cmdlen != cmd_len) {
    BX_ERROR(("Sent command length (%d) doesn't match expected command length (%d).", cmd_len, cmdlen));
    goto fail;
  }

  if (lun || buf[1] >> 5) {
    BX_ERROR(("unimplemented LUN %d", lun ? lun : buf[1] >> 5));
    if ((command != 0x03) && (command != 0x12)) // REQUEST SENSE and INQUIRY
      goto fail;
  }
  switch (command) {
    case 0x00:
      BX_DEBUG(("Test Unit Ready"));
      if (!inserted)
        goto notready;
      break;
    case 0x03:
      BX_DEBUG(("request Sense (len %d)", len));
      if (len < 4)
        goto fail;
      memset(outbuf, 0, 4);
      r->buf_len = 4;
      if ((sense == SENSE_NOT_READY) && (len >= 18)) {
        memset(outbuf, 0, 18);
        r->buf_len = 18;
        outbuf[7] = 10;
        /* asc 0x3a, ascq 0: Medium not present */
        outbuf[12] = 0x3a;
        outbuf[13] = 0;
      }
      outbuf[0] = 0xf0;
      outbuf[1] = 0;
      outbuf[2] = sense;
      break;
    case 0x12:
      BX_DEBUG(("inquiry (len %d)", len));
      if (buf[1] & 0x2) {
        // Command support data - optional, not implemented
        BX_ERROR(("optional INQUIRY command support request not implemented"));
        goto fail;
      } else if (buf[1] & 0x1) {
        // Vital product data
        Bit8u page_code = buf[2];
        if (len < 4) {
          BX_ERROR(("Error: Inquiry (EVPD[%02X]) buffer size %d is less than 4", page_code, len));
          goto fail;
        }
        switch (page_code) {
          case 0x00:
            // Supported page codes, mandatory
            BX_DEBUG(("Inquiry EVPD[Supported pages] buffer size %d", len));

            r->buf_len = 0;

            if (type == SCSIDEV_TYPE_CDROM) {
              outbuf[r->buf_len++] = 5;
            } else {
              outbuf[r->buf_len++] = 0;
            }

            outbuf[r->buf_len++] = 0x00; // this page
            outbuf[r->buf_len++] = 0x00;
            outbuf[r->buf_len++] = 3;    // number of pages
            outbuf[r->buf_len++] = 0x00; // list of supported pages (this page)
            outbuf[r->buf_len++] = 0x80; // unit serial number
            outbuf[r->buf_len++] = 0x83; // device identification
            break;

          case 0x80:
            {
              int l;

              // Device serial number, optional
              if (len < 4) {
                BX_ERROR(("Error: EVPD[Serial number] Inquiry buffer size %d too small, %d needed", len, 4));
                goto fail;
              }

              BX_DEBUG(("Inquiry EVPD[Serial number] buffer size %d", len));
              l = BX_MIN(len, (int)strlen(drive_serial_str));

              r->buf_len = 0;

              // Supported page codes
              if (type == SCSIDEV_TYPE_CDROM) {
                outbuf[r->buf_len++] = 5;
              } else {
                outbuf[r->buf_len++] = 0;
              }

              outbuf[r->buf_len++] = 0x80; // this page
              outbuf[r->buf_len++] = 0x00;
              outbuf[r->buf_len++] = l;
              memcpy(&outbuf[r->buf_len], drive_serial_str, l);
              r->buf_len += l;
            }
            break;

          case 0x83:
            {
              // Device identification page, mandatory
              size_t max_len = 255 - 8;
              size_t id_len = strlen(DEVICE_NAME);
              if (id_len > max_len)
                id_len = max_len;

              BX_DEBUG(("Inquiry EVPD[Device identification] buffer size %d", len));
              r->buf_len = 0;
              if (type == SCSIDEV_TYPE_CDROM) {
                outbuf[r->buf_len++] = 5;
              } else {
                outbuf[r->buf_len++] = 0;
              }

              outbuf[r->buf_len++] = 0x83; // this page
              outbuf[r->buf_len++] = 0x00;
              outbuf[r->buf_len++] = 3 + (Bit8u)id_len;

              outbuf[r->buf_len++] = 0x2; // ASCII
              outbuf[r->buf_len++] = 0;   // not officially assigned
              outbuf[r->buf_len++] = 0;   // reserved
              outbuf[r->buf_len++] = (Bit8u)id_len; // length of data following

              memcpy(&outbuf[r->buf_len], DEVICE_NAME, id_len);
              r->buf_len += (int)id_len;
            }
            break;

          default:
            BX_ERROR(("Error: unsupported Inquiry (EVPD[%02X]) buffer size %d", page_code, len));
            goto fail;
        }
        // done with EVPD
        break;
      } else {
        // Standard INQUIRY data
        if (buf[2] != 0) {
          BX_ERROR(("Error: Inquiry (STANDARD) page or code is non-zero [%02X]", buf[2]));
          goto fail;
        }

        // PAGE CODE == 0
        if (len < 5) {
          BX_ERROR(("Error: Inquiry (STANDARD) buffer size %d is less than 5", len));
          goto fail;
        }

        if (len < 36) {
          BX_ERROR(("Error: Inquiry (STANDARD) buffer size %d is less than 36 (TODO: only 5 required)", len));
        }
      }

      if(len > SCSI_MAX_INQUIRY_LEN)
        len = SCSI_MAX_INQUIRY_LEN;
      memset(outbuf, 0, len);
      if (lun || buf[1] >> 5) {
        outbuf[0] = 0x7f; // LUN not supported
      } else if (type == SCSIDEV_TYPE_CDROM) {
        outbuf[0] = 5;
        outbuf[1] = 0x80;
        memcpy(&outbuf[16], "BOCHS CD-ROM   ", 16);
      } else {
        outbuf[0] = 0;
        memcpy(&outbuf[16], "BOCHS HARDDISK ", 16);
      }
      memcpy(&outbuf[8], "BOCHS  ", 8);
      memcpy(&outbuf[32], "1.0", 4);
      // Identify device as SCSI-3 rev 1.
      // Some later commands are also implemented.
      outbuf[2] = 3;
      outbuf[3] = 2; // Format 2
      outbuf[4] = len - 5; // Additional Length = (Len - 1) - 4
      // Sync data transfer and TCQ.
      outbuf[7] = 0x10 | (tcq ? 0x02 : 0);
      r->buf_len = len;
      break;
    case 0x16:
      BX_INFO(("Reserve(6)"));
      if (buf[1] & 1)
        goto fail;
      break;
    case 0x17:
      BX_INFO(("Release(6)"));
      if (buf[1] & 1)
        goto fail;
      break;
    case 0x1a:
    case 0x5a:
      {
        Bit8u *p;
        int page;

        page = buf[2] & 0x3f;
        BX_DEBUG(("mode sense (page %d, len %d)", page, len));
        p = outbuf;
        memset(p, 0, 4);
        outbuf[1] = 0; /* Default media type.  */
        outbuf[3] = 0; /* Block descriptor length.  */
        if (type == SCSIDEV_TYPE_CDROM) {
          outbuf[2] = 0x80; /* Readonly.  */
        }
        p += 4;
        if ((page == 4) || (page == 5)) {
          BX_ERROR(("mode sense: page %d not implemented", page));
        }
        if ((page == 8 || page == 0x3f)) {
          /* Caching page.  */
          memset(p, 0, 20);
          p[0] = 8;
          p[1] = 0x12;
          p[2] = 4; /* WCE */
          p += 20;
        }
        if ((page == 0x3f || page == 0x2a)
            && (type == SCSIDEV_TYPE_CDROM)) {
          /* CD Capabilities and Mechanical Status page. */
          p[0] = 0x2a;
          p[1] = 0x14;
          p[2] = 3; // CD-R & CD-RW read
          p[3] = 0; // Writing not supported
          p[4] = 0x7f; /* Audio, composite, digital out,
                          mode 2 form 1&2, multi session */
          p[5] = 0xff; /* CD DA, DA accurate, RW supported,
                          RW corrected, C2 errors, ISRC,
                          UPC, Bar code */
          p[6] = 0x2d | (locked ? 2 : 0);
          /* Locking supported, jumper present, eject, tray */
          p[7] = 0; /* no volume & mute control, no changer */
          p[8] = (50 * 176) >> 8; // 50x read speed
          p[9] = (50 * 176) & 0xff;
          p[10] = 0 >> 8; // No volume
          p[11] = 0 & 0xff;
          p[12] = 2048 >> 8; // 2M buffer
          p[13] = 2048 & 0xff;
          p[14] = (16 * 176) >> 8; // 16x read speed current
          p[15] = (16 * 176) & 0xff;
          p[18] = (16 * 176) >> 8; // 16x write speed
          p[19] = (16 * 176) & 0xff;
          p[20] = (16 * 176) >> 8; // 16x write speed current
          p[21] = (16 * 176) & 0xff;
          p += 22;
        }
        r->buf_len = (int) (p - outbuf);
        outbuf[0] = r->buf_len - 4;
        if (r->buf_len > (int) len)
          r->buf_len = len;
      }
      break;
    case 0x1b:
      BX_INFO(("Start Stop Unit"));
      if (type == SCSIDEV_TYPE_CDROM && (buf[4] & 2)) {
        if (!(buf[4] & 1)) {
          // eject medium
          cdrom->eject_cdrom();
          inserted = 0;
        }
      }
      break;
    case 0x1e:
      BX_INFO(("Prevent Allow Medium Removal (prevent = %d)", buf[4] & 3));
      locked = buf[4] & 1;
      break;
    case 0x4a: // SCSI_CD_EVENT_STATUS
      {
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
          // only send the allocated length requested
          if (out_len > len)
            out_len = len;
          
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
          goto fail;
        }
      }
      break;
    case 0x51:  // SCSI_CD_READ_DISC_INFO
      {
        Bit16u out_len;
        
        // This code is experimental. may need some help
        BX_DEBUG(("Read Disc Information: Data type requested 0x%02X", buf[1] & 7));
        
        out_len = (Bit32s) (((Bit32u) buf[7] << 8) | buf[8]);
        switch (buf[1] & DISC_INFO_MASK) {
          // mmc6r02f.pdf, section 6.21.4, page 379(427) ??
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
            r->buf_len = BX_MIN(out_len, (2 + 32) + (0 * 8)); // (len word + sizeof(information)) + (OPC Table Entry count * Size of entry)
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
            r->buf_len = BX_MIN(out_len, 12);
            break;
          case DISC_INFO_POW_RES: // POW Resources Information
            
            // unsupported, so return 0 bytes
            
            // return length
            r->buf_len = 0;
            break;
          default:
            goto fail;
        }
      }
      break;
    case 0x25:
      BX_DEBUG(("Read Capacity"));
      // The normal LEN field for this command is zero
      memset(outbuf, 0, 8);
      if (type == SCSIDEV_TYPE_CDROM) {
        nb_sectors = max_lba;
      } else {
        nb_sectors = hdimage->hd_size / block_size;
        nb_sectors--;
      }
      /* Returned value is the address of the last sector.  */
      if (nb_sectors) {
        outbuf[0] = (Bit8u)((nb_sectors >> 24) & 0xff);
        outbuf[1] = (Bit8u)((nb_sectors >> 16) & 0xff);
        outbuf[2] = (Bit8u)((nb_sectors >> 8) & 0xff);
        outbuf[3] = (Bit8u) (nb_sectors & 0xff);
        outbuf[4] = (Bit8u) ((block_size >> 24) & 0xff);
        outbuf[5] = (Bit8u) ((block_size >> 16) & 0xff);
        outbuf[6] = (Bit8u) ((block_size >> 8) & 0xff);
        outbuf[7] = (Bit8u)  (block_size & 0xff);
        r->buf_len = 8;
      } else {
      notready:
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_NOT_READY);
        return 0;
      }
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
      {
        int start_track, format, msf, toclen = 0;

        if (type == SCSIDEV_TYPE_CDROM) {
          if (!inserted)
            goto notready;
          msf = buf[1] & 2;
          format = buf[2] & 0xf;
          start_track = buf[6];
          BX_DEBUG(("Read TOC (track %d format %d msf %d)", start_track, format, msf >> 1));
          cdrom->read_toc(outbuf, &toclen, msf, start_track, format);
          if (toclen > 0) {
            if (len > toclen)
              len = toclen;
            r->buf_len = len;
            break;
          }
          BX_ERROR(("Read TOC error"));
          goto fail;
        } else {
          goto fail;
        }
      }
    case 0x46:
      BX_DEBUG(("Get Configuration (rt %d, maxlen %d)", buf[1] & 3, len));
      memset(outbuf, 0, 8);
      /* ??? This should probably return much more information.  For now
         just return the basic header indicating the CD-ROM profile.  */
      outbuf[7] = 8; // CD-ROM
      r->buf_len = 8;
      break;
    case 0x56:
      BX_INFO(("Reserve(10)"));
      if (buf[1] & 3)
        goto fail;
      break;
    case 0x57:
      BX_INFO(("Release(10)"));
      if (buf[1] & 3)
        goto fail;
      break;
    case 0xa0:
      BX_INFO(("Report LUNs (len %d)", len));
      if (len < 16)
        goto fail;
      memset(outbuf, 0, 16);
      outbuf[3] = 8;
      r->buf_len = 16;
      break;
    case 0x2f:
      BX_DEBUG(("Verify (sector " FMT_LL "d, count %d)", lba, len));
      if (lba > max_lba)
        goto illegal_lba;
      if (buf[1] & 2) {
        BX_ERROR(("Verify with ByteChk not implemented yet"));
        goto fail;
      }
      break;
    case 0x23: {
      // USBMASS-UFI10.pdf  rev 1.0  Section 4.10
      BX_INFO(("READ FORMAT CAPACITIES (MMC)"));

      unsigned len = (buf[7]<<8) | buf[8];
#define OUR_LEN  12

      // Cap List Header
      outbuf[0] = 0;
      outbuf[1] = 0;
      outbuf[2] = 0;
      outbuf[3] = OUR_LEN;

      // Current/Max Cap Header
      if (type == SCSIDEV_TYPE_CDROM) {
        nb_sectors = max_lba;
      } else {
        nb_sectors = (hdimage->hd_size / block_size);
        nb_sectors--;
      }
      /* Returned value is the address of the last sector.  */
      outbuf[4] = (Bit8u)((nb_sectors >> 24) & 0xff);
      outbuf[5] = (Bit8u)((nb_sectors >> 16) & 0xff);
      outbuf[6] = (Bit8u)((nb_sectors >> 8) & 0xff);
      outbuf[7] = (Bit8u)(nb_sectors & 0xff);
      outbuf[8] = 2; // formatted (1 = unformatted)
      outbuf[9] = 0;
      outbuf[10] = (block_size >> 8);
      outbuf[11] = 0;

      r->buf_len = (len < OUR_LEN) ? len : OUR_LEN;

      }
      break;
    // The 0x9E command uses a service action code (ex: 0x9E/0x10)
    case 0x9E:
      switch (buf[1] & 0x1F) { // service action code
        case 0x10: // Read Capacity(16)
          BX_DEBUG(("Read Capacity 16"));
          memset(outbuf, 0, 32);
          if (type == SCSIDEV_TYPE_CDROM) {
            nb_sectors = max_lba;
          } else {
            nb_sectors = hdimage->hd_size / block_size;
            nb_sectors--;
          }
          /* Returned value is the address of the last sector.  */
          if (nb_sectors) {
            // 64-bit lba
            outbuf[ 0] = (Bit8u)((nb_sectors >> 56) & 0xff);
            outbuf[ 1] = (Bit8u)((nb_sectors >> 48) & 0xff);
            outbuf[ 2] = (Bit8u)((nb_sectors >> 40) & 0xff);
            outbuf[ 3] = (Bit8u)((nb_sectors >> 32) & 0xff);
            outbuf[ 4] = (Bit8u)((nb_sectors >> 24) & 0xff);
            outbuf[ 5] = (Bit8u)((nb_sectors >> 16) & 0xff);
            outbuf[ 6] = (Bit8u)((nb_sectors >> 8) & 0xff);
            outbuf[ 7] = (Bit8u) (nb_sectors & 0xff);
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
            // bytes 16 through 31 are reserved (zero'd above)
            r->buf_len = (len < 32) ? len : 32;
          } else {
            scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_NOT_READY);
            return 0;
          }
          break;
        default:
          BX_ERROR(("Unknown SCSI command (0x9E/%02X)", buf[1] & 0x1F));
      }
      break;
    default:
      BX_ERROR(("Unknown SCSI command (0x%02X)", buf[0]));
    fail:
      scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_ILLEGAL_REQUEST);
      return 0;
    illegal_lba:
      scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR);
      return 0;
  }
  if (r->sector_count == 0 && r->buf_len == 0) {
    scsi_command_complete(r, STATUS_GOOD, SENSE_NO_SENSE);
  }
  len = r->sector_count * block_size + r->buf_len;
  if (r->write_cmd) {
    return -len;
  } else {
    if (!r->sector_count)
      r->sector_count = (Bit32u) -1;
    return len;
  }
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
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_MEDIUM_ERROR);
        return;
      }
    } else {
      ret = (int) hdimage->lseek(r->sector * block_size, SEEK_SET);
      if (ret < 0) {
        BX_ERROR(("could not lseek() hard drive image file"));
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR);
        return;
      }
      i = 0;
      do {
        ret = (int) hdimage->read((bx_ptr_t) (r->dma_buf + (i * block_size)),
                                 block_size);
      } while ((++i < n) && (ret == block_size));
      if (ret != block_size) {
        BX_ERROR(("could not read() hard drive image file"));
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR);
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
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR);
      }
      i = 0;
      do {
        ret = (int) hdimage->write((bx_ptr_t) (r->dma_buf + (i * block_size)),
                                  block_size);
      } while ((++i < n) && (ret == block_size));
      if (ret != block_size) {
        BX_ERROR(("could not write() hard drive image file"));
        scsi_command_complete(r, STATUS_CHECK_CONDITION, SENSE_HARDWARE_ERROR);
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
