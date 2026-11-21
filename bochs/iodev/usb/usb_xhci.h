/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010-2026  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2011-2026  The Bochs Project
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

#ifndef BX_IODEV_USB_XHCI_H
#define BX_IODEV_USB_XHCI_H

#if BX_USE_USB_XHCI_SMF
#  define BX_XHCI_SMF  static
#  define BX_XHCI_THIS theUSB_XHCI->
#  define BX_XHCI_THIS_PTR theUSB_XHCI
#else
#  define BX_XHCI_SMF
#  define BX_XHCI_THIS this->
#  define BX_XHCI_THIS_PTR this
#endif

// If in 64bit mode, print 64bits, else only print 32 bit addresses
#if BX_PHY_ADDRESS_LONG
  #define FORMATADDRESS   FMT_ADDRX64
#else
  #define FORMATADDRESS   "%08X"
#endif

/************************************************************************************************
 * Actual configuration of the card
 */
enum {
  XHCI_HC_uPD720202,     // Renesas/NEC uPD720202 (2 sockets)  (default)
  XHCI_HC_uPD720201      // Renesas/NEC uPD720201 (4 sockets)
};

typedef struct {
  Bit32u HostController;
  unsigned int n_ports;

  struct XHCI_CAP_REGS {
    Bit32u HcCapLength;
    Bit32u HcSParams1;
    Bit32u HcSParams2;
    Bit32u HcSParams3;
    Bit32u HcCParams1;
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 1))
    Bit32u HcCParams2;
#endif
    Bit32u DBOFF;
    Bit32u RTSOFF;
  } cap_regs;

  struct XHCI_OP_REGS {
    Bit32u HcCommand;        // USBCMD  (HC_CMD_*)
    Bit32u HcStatus;         // USBSTS  (HC_STS_*)
    Bit32u HcPageSize;       // PAGESIZE
    Bit32u HcNotification;   // DNCTRL
    struct {
      Bit64u crc;            // 64 bit hi order address            = 0x00000000     RW
      Bit8u  RsvdP;          //  2 bit reserved and preserved      = 00b            RW
      bool   crr;            //  1 bit Command Ring Running        = 0              RO
      bool   ca;             //  1 bit Command Abort               = 0              RW1S
      bool   cs;             //  1 bit Command Stop                = 0              RW1S
      bool   rcs;            //  1 bit Ring Cycle State            = 0              RW
      Bit64u actual;         // Actual 64-bit value in this register (this register is read as zero, so we keep it here for internal reading)
    } HcCrcr;
    struct {
      Bit64u dcbaap;         // 64 bit hi order address            = 0x00000000     RW
      Bit8u  RsvdZ;          //  6 bit reserved and zero'd         = 000000b        RW
    } HcDCBAAP;
    Bit32u HcConfig;         // CONFIG  (HC_CONFIG_*)
  } op_regs;

  struct {
    // our data
    usb_device_c *device; // device connected to this port
    bool is_usb3;         // set if usb3 port, cleared if usb2 port.
    bool has_been_reset;  // set if the port has been reset aftet powered up.
    Bit8u psceg;          // current port status change event

    Bit32u portsc;        // PORTSC  (PORTSC_*)
    Bit32u portpmsc;      // PORTPMSC (USB3 or USB2 layout)
    Bit32u portli;        // PORTLI   (USB3 or USB2 layout)
    Bit32u porthlpmc;     // PORTHLPMC
  } usb_port[USB_XHCI_PORTS_MAX];

  // Extended Caps Registers
  Bit8u extended_caps[EXT_CAPS_SIZE];

  struct XHCI_RUNTIME_REGS {
    Bit32u mfindex;              // MFINDEX
    struct {
      Bit32u  iman;              // IMAN   (IMAN_*)
      Bit32u  imod;              // IMOD   (counter in 31:16, interval in 15:0)
      Bit32u  erstsz;            // ERSTSZ (ERSTSZ_*)
      Bit32u  RsvdP;             // 32 bit reserved and preseved       = 0x00000000     RW
      struct {
        Bit64u  erstabadd;       // 64 bit Event Ring Seg Tab Addy     = 0x00000000     RW  (See #define below)
        Bit16u  RsvdP;           //  6 bit reserved and preseved       = 0x0000         RW
      } erstba;
      struct {
        Bit64u  eventadd;        // 64 bit Event Ring Addy hi          = 0x00000000     RW
        bool    ehb;             //  1 bit Event Handler Busy          = 0b             RW1C
        Bit8u   desi;            //  2 bit Dequeue ERST Seg Index      = 00b            RW
      } erdp;
    } interrupter[INTERRUPTERS];
  } runtime_regs;

  struct HC_SLOT_CONTEXT slots[MAX_SLOTS];  // first one is ignored by controller.

  struct RING_MEMBERS ring_members;
  
  // filled at runtime with ex: { USB3, USB3, USB2, USB2 };
  //Bit8u port_speed_allowed[USB_XHCI_PORTS_MAX];
  // four speeds of: 'reserved' + a port count of bytes rounded up to and 8 byte size (ie: 8, 16, 24, 32 bytes each speed)
  Bit8u port_band_width[4 * ((1 + USB_XHCI_PORTS_MAX) + 8)]; // + 8 gives us ample room for a boundary of 8-byte entries per speed
  // the port's paired port num. i.e., with 4 ports, 1 is paired with 3, 2 is paired with 4
  int   paired_portnum[USB_XHCI_PORTS_MAX];
} bx_usb_xhci_t;

// Version 3.0.23.0 of the Renesas uPD720202 driver, even though the card is
//  version 1.00, the driver still uses bits 3:0 as RsvdP as with version 0.96
//  instead of bits 5:0 as RsvdP as with version 1.00+
#define RENESAS_ERSTABADD_BUG 1
#if ((VERSION_MAJOR < 1) || (RENESAS_ERSTABADD_BUG == 1))
  #define ERSTABADD_MASK   0x0F  // versions before 1.0 use 3:0 as preserved
#elif ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x00))
  #define ERSTABADD_MASK   0x3F  // versions 1.0 and above use 5:0 as preserved
#else
  #error "ERSTABADD_MASK not defined"
#endif

class bx_usb_xhci_c : public bx_pci_device_c {
public:
  bx_usb_xhci_c();
  virtual ~bx_usb_xhci_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  int event_handler(int event, void *ptr, int port);

  bx_usb_xhci_t hub;
private:
  Bit8u         devfunc;
  Bit8u         device_change;
  int           rt_conf_id;
  int           xhci_timer_index;
  USBAsync      *packets;

  void reset_hc();
  BX_XHCI_SMF void reset_port(int);
  BX_XHCI_SMF void reset_port_usb3(int, int);
  BX_XHCI_SMF bool save_hc_state(void);
  BX_XHCI_SMF bool restore_hc_state(void);

  BX_XHCI_SMF void update_irq(void);

  BX_XHCI_SMF void init_device(Bit8u port, bx_list_c *portconf);
  BX_XHCI_SMF void remove_device(Bit8u port);
  BX_XHCI_SMF bool set_connect_status(Bit8u port, bool connected);

  BX_XHCI_SMF int  broadcast_speed(SLOT_CONTEXT& slot_context);
  BX_XHCI_SMF int  broadcast_packet(USBPacket *p, int port);
  BX_XHCI_SMF Bit8u get_psceg(int port);
  static void xhci_timer_handler(void *);
  void xhci_timer(void);

  BX_XHCI_SMF Bit64u process_transfer_ring(int slot, int ep, Bit64u ring_addr, bool *rcs, int primary_sid);
  BX_XHCI_SMF void process_command_ring(void);
  BX_XHCI_SMF void get_stream_info(struct STREAM_CONTEXT *context, Bit64u address, int index);
  BX_XHCI_SMF void put_stream_info(struct STREAM_CONTEXT *context, Bit64u address, int index);
  BX_XHCI_SMF void write_event_TRB(unsigned interrupter, Bit64u parameter, Bit32u status,
                              Bit32u command, bool fire_int);
  BX_XHCI_SMF Bit32u NEC_verification(Bit64u signature);
  BX_XHCI_SMF void init_event_ring(unsigned interrupter);
  BX_XHCI_SMF void read_TRB(bx_phy_address addr, struct TRB *trb);
  BX_XHCI_SMF void write_TRB(bx_phy_address addr, Bit64u parameter, Bit32u status, Bit32u command);
  BX_XHCI_SMF void update_slot_context(int slot);
  BX_XHCI_SMF void update_ep_context(int slot, int ep);
  BX_XHCI_SMF void dump_slot_context(const Bit32u *context, int slot);
  BX_XHCI_SMF void dump_ep_context(const Bit32u *context, int slot, int ep);
  BX_XHCI_SMF void dump_stream_context(const Bit32u *context);
  BX_XHCI_SMF void copy_slot_from_buffer(struct SLOT_CONTEXT *slot_context, const Bit8u *buffer);
  BX_XHCI_SMF void copy_ep_from_buffer(struct EP_CONTEXT *ep_context, const Bit8u *buffer);
  BX_XHCI_SMF void copy_slot_to_buffer(Bit32u *buffer, int slot);
  BX_XHCI_SMF void copy_ep_to_buffer(Bit32u *buffer, int slot, int ep);
  BX_XHCI_SMF void copy_stream_from_buffer(struct STREAM_CONTEXT *context, const Bit8u *buffer);
  BX_XHCI_SMF void copy_stream_to_buffer(Bit8u *buffer, const struct STREAM_CONTEXT *context);
  BX_XHCI_SMF int  validate_slot_context(const struct SLOT_CONTEXT *slot_context, int trb_command, int slot);
  BX_XHCI_SMF int  validate_ep_context(const struct EP_CONTEXT *ep_context, int trb_command, Bit32u a_flags, int port_num, int ep_num);
  BX_XHCI_SMF int  create_unique_address(int slot);
  BX_XHCI_SMF int  send_set_address(int addr, int port_num, SLOT_CONTEXT& slot_context);

  BX_XHCI_SMF void dump_xhci_core(unsigned int slots, unsigned int eps);

  static bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#if !BX_USE_USB_XHCI_SMF
  bool mem_read(bx_phy_address addr, unsigned len, void *data);
  bool mem_write(bx_phy_address addr, unsigned len, void *data);
#endif

  static void runtime_config_handler(void *);
  void runtime_config(void);

  static Bit64s usb_param_handler(bx_param_c *param, bool set, Bit64s val);
  static Bit64s usb_param_oc_handler(bx_param_c *param, bool set, Bit64s val);
  static bool usb_param_enable_handler(bx_param_c *param, bool en);
};

#endif  // BX_IODEV_USB_XHCI_H
