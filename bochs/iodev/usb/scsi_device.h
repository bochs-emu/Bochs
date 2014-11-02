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
//  Copyright (C) 2007-2014  The Bochs Project
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

#ifndef BX_IODEV_SCSI_DEVICE_H
#define BX_IODEV_SCSI_DEVICE_H

typedef void (*scsi_completionfn)(void *opaque, int reason, Bit32u tag,
                                  Bit32u arg);
class cdrom_base_c;

enum scsidev_type {
  SCSIDEV_TYPE_DISK,
  SCSIDEV_TYPE_CDROM
};

enum scsi_reason {
  SCSI_REASON_DONE,
  SCSI_REASON_DATA
};

#define SENSE_NO_SENSE        0
#define SENSE_NOT_READY       2
#define SENSE_MEDIUM_ERROR    3
#define SENSE_HARDWARE_ERROR  4
#define SENSE_ILLEGAL_REQUEST 5

#define STATUS_GOOD            0
#define STATUS_CHECK_CONDITION 2

#define SCSI_DMA_BUF_SIZE    131072
#define SCSI_MAX_INQUIRY_LEN 256

typedef struct SCSIRequest {
  Bit32u tag;
  Bit64u sector;
  Bit32u sector_count;
  int buf_len;
  Bit8u *dma_buf;
  Bit32u status;
  struct SCSIRequest *next;
} SCSIRequest;


class scsi_device_t : public logfunctions {
public:
  scsi_device_t(device_image_t *_hdimage, int _tcq,
               scsi_completionfn _completion, void *_dev);
  scsi_device_t(cdrom_base_c *_cdrom, int _tcq,
               scsi_completionfn _completion, void *_dev);
  virtual ~scsi_device_t(void);

  void register_state(bx_list_c *parent, const char *name);
  Bit32s scsi_send_command(Bit32u tag, Bit8u *buf, int lun);
  void scsi_command_complete(SCSIRequest *r, int status, int sense);
  void scsi_cancel_io(Bit32u tag);
  void scsi_read_complete(void *req, int ret);
  void scsi_read_data(Bit32u tag);
  void scsi_write_complete(void *req, int ret);
  int scsi_write_data(Bit32u tag);
  Bit8u* scsi_get_buf(Bit32u tag);
  const char *get_serial_number() {return drive_serial_str;}
  void set_inserted(bx_bool value);
  bx_bool get_inserted() {return inserted;}
  static void seek_timer_handler(void *);
  void seek_timer(void);
  bx_bool save_requests(const char *path);
  void restore_requests(const char *path);

protected:
  SCSIRequest* scsi_new_request(Bit32u tag);
  void scsi_remove_request(SCSIRequest *r);
  SCSIRequest *scsi_find_request(Bit32u tag);

private:
  enum scsidev_type type;
  device_image_t *hdimage;
  cdrom_base_c *cdrom;
  SCSIRequest *requests;
  int cluster_size;
  Bit64u max_lba;
  int sense;
  int tcq;
  scsi_completionfn completion;
  void *dev;
  bx_bool locked;
  bx_bool inserted;
  char drive_serial_str[21];
  int seek_timer_index;
};

#endif
