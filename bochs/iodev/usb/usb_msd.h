/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  USB mass storage device support (ported from QEMU)
//
//  Copyright (c) 2006 CodeSourcery.
//  Written by Paul Brook
//  Copyright (C) 2009-2023  The Bochs Project
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

#ifndef BX_IODEV_USB_MSD_H
#define BX_IODEV_USB_MSD_H

class device_image_t;
class cdrom_base_c;
class scsi_device_t;

#define UASP_MAX_STREAMS     5
#define UASP_MAX_STREAMS_N  (1 << (UASP_MAX_STREAMS + 1))

// protocol
#define MSD_PROTO_BBB   0  // bulk only
#define MSD_PROTO_UASP  1  // UASP protocol (uses streams)

// BBB endpoint numbers (must remain consecutive)
#define MSD_BBB_DATAIN_EP    1
#define MSD_BBB_DATAOUT_EP   2
// UASP endpoint numbers (must remain consecutive)
#define MSD_UASP_COMMAND     1
#define MSD_UASP_STATUS      2
#define MSD_UASP_DATAIN      3
#define MSD_UASP_DATAOUT     4

typedef struct UASPRequest {
  Bit32u mode;        // current direction and other flags
  Bit32u data_len;    // count of bytes requested (from command requested)
  Bit32u residue;     // = data_len - actual_len
  Bit32u scsi_len;    //
  Bit8u *scsi_buf;    // 
  Bit32u usb_len;     //
  Bit8u *usb_buf;     // 
  int result;         //
  Bit32u tag;         // tag number from command ui
  USBPacket *p;       //
  USBPacket *status;  //
} UASPRequest;

class usb_msd_device_c : public usb_device_c {
public:
  usb_msd_device_c(const char *devname);
  virtual ~usb_msd_device_c(void);

  virtual bool init();
  virtual bool set_option(const char *option);
  virtual const char* get_info();
  virtual void runtime_config(void);
  void restore_handler(bx_list_c *conf);

  virtual void handle_reset();
  virtual int handle_control(int request, int value, int index, int length, Bit8u *data);
  virtual int handle_data(USBPacket *p);
  virtual void handle_iface_change(int iface);
  virtual void register_state_specific(bx_list_c *parent);
  virtual void cancel_packet(USBPacket *p);
  bool set_inserted(bool value);
  bool get_inserted();
  bool get_locked();

protected:
  void copy_data();
  void send_status(USBPacket *p);
  static void usb_msd_command_complete(void *this_ptr, int reason, Bit32u tag, Bit32u arg);
  void command_complete(int reason, Bit32u tag, Bit32u arg);

private:
  struct {
    // members set in constructor / init
    char *image_mode;
    int  proto;  // MSD_PROTO_BBB (default), MSD_PROTO_UASP (uses streams)
    device_image_t *hdimage;
    cdrom_base_c *cdrom;
    scsi_device_t *scsi_dev;
    bx_list_c *sr_list;
    char fname[BX_PATHNAME_LEN];
    bx_list_c *config;
    char info_txt[BX_PATHNAME_LEN + 38];
    char journal[BX_PATHNAME_LEN]; // undoable / volatile disk only
    int size; // VVFAT disk only
    unsigned sect_size; // sector size for disks only (default = 512 bytes)
    // members handled by runtime config
    bool status_changed;
    // members handled by save/restore
    Bit8u mode;
    Bit32u scsi_len;
    Bit32u usb_len;
    Bit32u data_len;
    Bit32u residue;
    Bit32u tag;
    int result;
    // members not handled by save/restore
    Bit8u *scsi_buf;
    Bit8u *usb_buf;
    USBPacket *packet;
    // UASP (w/streams)
    UASPRequest uasp_request[UASP_MAX_STREAMS_N];
  } s;

  static const char *cdrom_path_handler(bx_param_string_c *param, bool set,
                                        const char *oldval, const char *val, int maxlen);
  static Bit64s cdrom_status_handler(bx_param_c *param, bool set, Bit64s val);

public:
  int uasp_handle_data(USBPacket *p);
  void uasp_initialize_request(int tag);
  UASPRequest *uasp_find_request(Bit32u tag);
  Bit32u get_data_len(const struct S_UASP_INPUT *input, Bit8u *buf);
  const struct S_UASP_INPUT *uasp_get_info(Bit8u command, Bit8u serv_action);
  int uasp_do_stall(UASPRequest *req);
  int uasp_do_command(USBPacket *p);
  int uasp_process_request(USBPacket *p, int index);
  int uasp_do_data(UASPRequest *req, USBPacket *p);
  int uasp_do_ready(UASPRequest *req, USBPacket *p);
  int uasp_do_status(UASPRequest *req, USBPacket *p);
  void uasp_copy_data(UASPRequest *req);
  void uasp_command_complete(int reason, Bit32u tag, Bit32u arg);
};

#endif
