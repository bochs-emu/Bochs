/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  UFI/CBI floppy disk storage device support
//
//  Copyright (c) 2015-2023  Benjamin David Lunt
//  Copyright (C) 2015-2023  The Bochs Project
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

#ifndef BX_IODEV_USB_FLOPPY_H
#define BX_IODEV_USB_FLOPPY_H

#define UFI_TEST_UNIT_READY             0x00
#define UFI_REZERO                      0x01
#define UFI_REQUEST_SENSE               0x03
#define UFI_FORMAT_UNIT                 0x04
#define UFI_INQUIRY                     0x12
#define UFI_START_STOP_UNIT             0x1B
#define UFI_SEND_DIAGNOSTIC             0x1D
#define UFI_PREVENT_ALLOW_REMOVAL       0x1E
#define UFI_READ_FORMAT_CAPACITIES      0x23
#define UFI_READ_CAPACITY               0x25
#define UFI_READ_10                     0x28
#define UFI_WRITE_10                    0x2A
#define UFI_SEEK_10                     0x2B
#define UFI_WRITE_VERIFY                0x2E
#define UFI_VERIFY                      0x2F
#define UFI_MODE_SELECT                 0x55
#define UFI_MODE_SENSE                  0x5A
#define UFI_READ_12                     0xA8
#define UFI_WRITE_12                    0xAA

#define UFI_DO_INQUIRY_HACK  1

class device_image_t;

class usb_floppy_device_c : public usb_device_c {
public:
  usb_floppy_device_c(void);
  virtual ~usb_floppy_device_c(void);

  virtual bool init();
  virtual bool set_option(const char *option);
  virtual const char* get_info();
  virtual void runtime_config(void);
  void restore_handler(bx_list_c *conf);

  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void register_state_specific(bx_list_c *parent);
  virtual void cancel_packet(USBPacket *p);

  static void floppy_timer_handler(void *);

private:
  struct {
    // members set in constructor / init
    bx_list_c *config;
    char info_txt[BX_PATHNAME_LEN];
    bool model;  // 0 = bochs, 1 = teac
    int statusbar_id;
    int floppy_timer_index;
    // members handled by runtime config
    device_image_t *hdimage;
    const char *fname;
    char *image_mode;
    bool inserted; // 0 = media not present
    bool wp;     // 0 = not write_protected, 1 = write_protected
    bool status_changed;
    // members handled by save/restore
    Bit32u usb_len;
    Bit32u data_len;
    Bit32u sector;
    Bit32u sector_count;
    Bit8u cur_command;
    Bit8u cur_track;
    int sense;
    int asc;
#if UFI_DO_INQUIRY_HACK
    int fail_count;
    bool did_inquiry_fail;
#endif
    bool seek_pending;
    Bit8u *usb_buf;
    Bit8u *dev_buffer;
    // members not handled by save/restore
    USBPacket *packet;
  } s;

  bool handle_command(Bit8u *command);
  void start_timer(Bit8u mode);
  void floppy_timer(void);
  int floppy_read_sector(void);
  int floppy_write_sector(void);
  void copy_data(USBPacket *p);
  bool set_inserted(bool value);

  static const char *floppy_path_handler(bx_param_string_c *param, bool set,
                                         const char *oldval, const char *val, int maxlen);
  static Bit64s floppy_param_handler(bx_param_c *param, bool set, Bit64s val);

  static Bit64s param_save_handler(void *devptr, bx_param_c *param);
  static void param_restore_handler(void *devptr, bx_param_c *param, Bit64s val);
};

#endif
