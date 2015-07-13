/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  UFI/CBI floppy disk storage device support
//
//  Copyright (c) 2015  Benjamin David Lunt
//  Copyright (C) 2015  The Bochs Project
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

#ifndef BX_IODEV_USB_CBI_H
#define BX_IODEV_USB_CBI_H

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

class device_image_t;

class usb_cbi_device_c : public usb_device_c {
public:
  usb_cbi_device_c(usbdev_type type, const char *filename);
  virtual ~usb_cbi_device_c(void);

  virtual bx_bool init();
  virtual bx_bool set_option(const char *option);
  virtual const char* get_info();

  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void register_state_specific(bx_list_c *parent);
  virtual void cancel_packet(USBPacket *p);

protected:
  void send_status();
  bx_bool handle_command(Bit8u *command);

private:
  struct {
    Bit8u *dev_buffer;
    Bit32u usb_len;
    Bit8u *usb_buf;
    Bit32u data_len;
    Bit32u tag;
    Bit8u image_mode;
    device_image_t *hdimage;
    USBPacket *packet;
    bx_list_c *sr_list;
    const char *fname;
    bx_list_c *config;
    char info_txt[BX_PATHNAME_LEN];
    Bit8u cur_command;
    int fail_count;
    bx_bool did_inquiry_fail;
    bx_bool wp;     // 0 = not write_protected, 1 = write_protected
    bx_bool model;  // 0 = bochs, 1 = teac
    int statusbar_id;
  } s;

};

#endif
