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

#ifndef BX_IODEV_SCSI_DEVICE_H
#define BX_IODEV_SCSI_DEVICE_H

typedef void (*scsi_completionfn)(void *opaque, int reason, Bit32u tag, Bit32u arg);
class cdrom_base_c;

// SCSI-2r10l, section 7.2, page 72(108) states that a LUN is given in bits 7:5
//  of byte[1] of the command block (CDB). (year 1993)
// SPC-1r11a, section 4.2, page 10(26) has this field reserved. (year 1997)
// SPC-3r23f, section 4.3.2, page 23 has this field as "miscellaneous CDB information" (year 2005)
// If you wish to support this old, now obsolete field, change SCSI_OBSOLUTE_LUN in scsi_device.h
#define SCSI_OBSOLUTE_LUN  0

// set this to 1 to be strict on CDB parameters.
// for example, Windows(TM) sends a 12-byte Request Sense CDB, when it should be 6.
// Windows(TM) also sends a 12-byte Get Configuration CDB, when it should be 10.
// (setting this to 0 will allow these errors to be ignored)
#define SCSI_STRICT_CDB  0

#define PERIPHERAL_TYPE_SBC3   0  // magnetic disk
#define PERIPHERAL_TYPE_CDROM  5  // cd-rom

enum scsidev_type {
  SCSIDEV_TYPE_DISK,
  SCSIDEV_TYPE_CDROM
};

enum scsi_reason {
  SCSI_REASON_DONE,
  SCSI_REASON_DATA
};

// SCSI_CD_CONFIGURATION
#define CONFIG_PROFILE_CDROM          8   // a CD-ROM
#define CONFIG_PHY_INT_STND  0x00000001   // SCSI Family
#define CONFIG_CURRENT(c)       ((c)<<0)  // current
#define CONFIG_PERSISTENT(p)    ((p)<<1)  // persistent
#define CONFIG_VERSION(v)       ((v)<<2)  // version
// supported features (MMC-6r2f, section 5.2.3, page 194(242)
#define CONFIG_FEATURE_0000      0x0000   // optional
#define CONFIG_FEATURE_0001      0x0001   // mandatory
  #define CONFIG_INQ2               (0<<1)  // Inquire support (0 = standard, 1 = extra)
  #define CONFIG_DBE                (0<<0)  // Device Buzy Event (0 = legacy, 1 = includes DBE's)
#define CONFIG_FEATURE_0002      0x0002   // mandatory
  #define CONFIG_OCEVENT            (1<<1)  // OCEvent (must be 1)
  #define CONFIG_ASYNC              (0<<1)  // 0 = supports only the polling implentation of GET EVENT STATUS NOTIFICATION (see 0x4A)
#define CONFIG_FEATURE_0003      0x0003   // optional
  #define CONFIG_LOAD_MECH(lm)     ((lm)<<5) // Load Mechanism
    #define CONFIG_LM_CADDY          0
    #define CONFIG_LM_TRAY           1
    #define CONFIG_LM_POPUP          2
    #define CONFIG_LM_EMB_CHANGE     4
    #define CONFIG_LM_EMB_CHANGE_MAG 5
  #define CONFIG_LOAD(l)           ((l)<<4) // Load
  #define CONFIG_EJECT(e)          ((e)<<3) // Eject
  #define CONFIG_PVNT_JMPR(pj)    ((pj)<<2) // Prevent Jumper
  #define CONFIG_PVNT_DBML(d)      ((d)<<1) // Device Busy Class
  #define CONFIG_LOCK(l)           ((l)<<0) // Lock capable
#define CONFIG_FEATURE_0010      0x0010   // optional

// SCSI_CD_EVENT_STATUS items
#define EVENT_STATUS_POLLED       1
#define EVENT_STATUS_RESERVED_0  (1<<0)  // reserved          (bit 0)
#define EVENT_STATUS_OP_CHANGE   (1<<1)  // Operatinal Change (bit 1)
#define EVENT_STATUS_POWER_MAN   (1<<2)  // Power Management  (bit 2)
#define EVENT_STATUS_EXT_REQ     (1<<3)  // External Request  (bit 3)
#define EVENT_STATUS_MEDIA_REQ   (1<<4)  // Media Request     (bit 4)
  #define EVENT_STATUS_MED_N_PRES  (0<<1)  // Media is not present
  #define EVENT_STATUS_MED_PRES    (1<<1)  // Media is present
  #define EVENT_STATUS_MED_N_OPEN  (0<<0)  // Door/Tray is not open
  #define EVENT_STATUS_MED_OPEN    (1<<0)  // Door/Tray is open
#define EVENT_STATUS_MULTI_INIT  (1<<5)  // Multi-Initiator   (bit 5)
#define EVENT_STATUS_DEV_BUSY    (1<<6)  // Device Busy       (bit 6)
#define EVENT_STATUS_RESERVED_7  (1<<7)  // reserved          (bit 7)

// SCSI_CD_READ_DISC_INFO items
#define DISC_INFO_MASK           0x07  // mask to get disk info type
#define DISC_INFO_STANDARD       0
  #define DI_STAND_N_ERASABLE      (0<<4)  // Non-erasable
  #define DI_STAND_ERASABLE        (1<<4)  // Erasable
  #define DI_STAND_LAST_STATE_MT   (0<<2)  // Empty session
  #define DI_STAND_LAST_STATE_INC  (1<<2)  // Incomplete session
  #define DI_STAND_LAST_STATE_DAM  (2<<2)  // Damaged session
  #define DI_STAND_LAST_STATE_COMP (3<<2)  // Complete session
  #define DI_STAND_STATUS_MT       (0<<0)  // Empty disc
  #define DI_STAND_STATUS_INC      (1<<0)  // Incomplete disc
  #define DI_STAND_STATUS_FINAL    (2<<0)  // Finalized disc
  #define DI_STAND_STATUS_OTHER    (3<<0)  // Other type
  #define DI_STAND_DID_N_VALID     (0<<7)  // Disc ID area not valid
  #define DI_STAND_DID_VALID       (1<<7)  // Disc ID area valid
  #define DI_STAND_DBC_N_VALID     (0<<6)  // Disc Barcode area not valid
  #define DI_STAND_DBC_VALID       (1<<6)  // Disc Barcode area valid
  #define DI_STAND_URU_N_OKAY      (0<<5)  // Unrestricted use not okay
  #define DI_STAND_URU_OKAY        (1<<5)  // Unrestricted use okay
  #define DI_STAND_DAC_N_VALID     (0<<4)  // Disc Application Code not valid
  #define DI_STAND_DAC_VALID       (1<<4)  // Disc Application Code valid
  #define DI_STAND_N_LEGACY        (0<<2)  // Not Legacy?
  #define DI_STAND_LEGACY          (1<<2)  // Legacy?
  #define DI_STAND_BG_FORMAT_0     (0<<0)  // Background Format Status code 0
  #define DI_STAND_BG_FORMAT_1     (1<<0)  // Background Format Status code 1
  #define DI_STAND_BG_FORMAT_2     (2<<0)  // Background Format Status code 2
  #define DI_STAND_BG_FORMAT_3     (3<<0)  // Background Format Status code 3
  #define DI_STAND_DISC_TYPE_0      0x00   // Disc type: CD-DA or CD-ROM
  #define DI_STAND_DISC_TYPE_1      0x10   // Disc type: CD-I disc
  #define DI_STAND_DISC_TYPE_2      0x20   // Disc type: CD-ROM XA Disc
  #define DI_STAND_DISC_TYPE_x      0xFF   // Disc type: Undefined
#define DISC_INFO_TRACK          1
#define DISC_INFO_POW_RES        2

// SCSI Page Mode attributes
#define PAGE_CONTROL_CURRENT     0
#define PAGE_CONTROL_CHANGEABLE  1
#define PAGE_CONTROL_DEFAULT     2
#define PAGE_CONTROL_SAVED       3

#define PAGE_SAVEABLE(s)     ((s)<<7)
#define PAGE_SPF(s)          ((s)<<6)

// SPC-4r18, section 7.4.5, page 440
// SPC-4r18, section D.6, page 677
// SBC-3r25, section 6.4, page 208
// MMC-6r02f, section 7.2.2, page 597(645)
// SCSI-r10L, section 9.3.3.7, pages 229(265)
#define PAGE_VENDOR_SPECIFIC        0x00   
#define PAGE_ERROR_RECOVERY         0x01   // SBC-3/MMC-6
#define PAGE_DISCONNECT             0x02   // SBC-3/MMC-6
#define PAGE_FORMAT_DEVICE          0x03   // SBC-3
#define PAGE_MRW                    0x03   // MMC-6 (Mount Rainier CD-RW's = CD-MRW)
#define PAGE_HD_GEOMETRY            0x04   // SBC-3
#define PAGE_FLEXIBILE_GEOMETRY     0x05   // SBC-3
#define PAGE_WRITE_PARAMETERS       0x05   // MMC-6
#define PAGE_VERIFY_ERROR           0x07   // SBC-3
#define PAGE_CACHING                0x08   // SBC-3/MMC-6
#define PAGE_CONTROL                0x0A   // SBC-3/MMC-6 (sub pages 0 & 1)
#define PAGE_MEDIUM_TYPES           0x0B   // SBC-3
#define PAGE_NOTCH_PARTITION        0x0C   // SBC-3
#define PAGE_CD_DEVICE_PARAMS       0x0D   // MMC-6
#define PAGE_CD_AUDIO_CONTROL       0x0E   // MMC-6
#define PAGE_XOR_CONTROL            0x10   // SBC-3
#define PAGE_MEDIUM_PARTITION       0x11   // SBC-3
#define PAGE_ENCLOSURE_SERV_MAN     0x14   // SBC-3/MMC-6
#define PAGE_PROTOCOL_SPECIFIC_LUN  0x18   // SBC-3/MMC-6
#define PAGE_PROTOCOL_SPECIFIC_PORT 0x19   // SBC-3/MMC-6
#define PAGE_POWER_CONDITION        0x1A   // SBC-3/MMC-6
#define PAGE_EXCEPTION_CONTROL      0x1C   // SBC-3/MMC-6
#define PAGE_CDVD_INACTIVITY        0x1D   // MMC-6
#define PAGE_CAPABILITIES           0x2A   // MMC-3
#define SENSE_RETURN_ALL            0x3F

// SPC-4r18, Table 41 (page 63)
// Sense Key:
#define SENSE_NO_SENSE         0
#define SENSE_RECOVERED_ERROR  1
#define SENSE_NOT_READY        2
#define SENSE_MEDIUM_ERROR     3
#define SENSE_HARDWARE_ERROR   4
#define SENSE_ILLEGAL_REQUEST  5
#define SENSE_UNIT_ATTENTION   6
#define SENSE_DATA_PROTECT     7
#define SENSE_BLANK_CHECK      8
#define SENSE_VENDOR_SPECIFIC  9
#define SENSE_COPY_ABORTED    10
#define SENSE_ABORTED_COMMAND 11
#define SENSE_VOLUME_OVERFLOW 13
#define SENSE_MISCOMPARE      14

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
  bool write_cmd;
  bool async_mode;
  Bit8u seek_pending;
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
  Bit32s scsi_send_command(Bit32u tag, Bit8u *buf, Bit8u cmd_len, int lun, bool async);
  void scsi_command_complete(SCSIRequest *r, int status, Bit8u _sense, Bit8u _asc, Bit8u _ascq);
  void scsi_cancel_io(Bit32u tag);
  void scsi_read_complete(void *req, int ret);
  void scsi_read_data(Bit32u tag);
  void scsi_write_complete(void *req, int ret);
  void scsi_write_data(Bit32u tag);
  Bit8u* scsi_get_buf(Bit32u tag);
  const char *get_serial_number() {return drive_serial_str;}
  int scsi_do_modepage_hdr(Bit8u *p, Bit8u sub_page, Bit8u page_code, int len);
  int scsi_do_modepage(Bit8u *p, Bit8u pc, Bit8u sub_page, Bit8u page_code);
  void set_inserted(bool value);
  bool get_inserted() {return inserted;}
  bool get_locked() {return locked;}
  static void seek_timer_handler(void *);
  bool save_requests(const char *path);
  void restore_requests(const char *path);
  void set_debug_mode();

protected:
  SCSIRequest* scsi_new_request(Bit32u tag);
  void scsi_remove_request(SCSIRequest *r);
  SCSIRequest *scsi_find_request(Bit32u tag);

private:
  void start_seek(SCSIRequest *r);
  void seek_timer(void);
  void seek_complete(SCSIRequest *r);

  // members set in constructor
  enum scsidev_type type;
  device_image_t *hdimage;
  cdrom_base_c *cdrom;
  int block_size;
  int tcq;
  scsi_completionfn completion;
  void *dev;
  char drive_serial_str[21];
  int seek_timer_index;
  int statusbar_id;
  // members set in constructor / runtime config
  Bit64u max_lba;
  bool inserted;
  // members handled by save/restore
  Bit64u curr_lba;
  Bit8u sense, asc, ascq;
  bool locked;
  bool read_only;
  SCSIRequest *requests;
};

#endif
