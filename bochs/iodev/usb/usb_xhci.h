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
    struct {
      Bit32u RsvdP1;         // 18/20 bit reserved and preserved   = 0x000000       RW
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
      bool   cme;            //  1 bit Max Exit Latecy to Large    = 0b             RW
      bool   spe;            //  1 bit Generate Short Packet Comp  = 0b             RW
#endif
      bool   eu3s;           //  1 bit Enable U3 MFINDEX Stop      = 0b             RW
      bool   ewe;            //  1 bit Enable Wrap Event           = 0b             RW
      bool   crs;            //  1 bit Controller Restore State    = 0b             RW
      bool   css;            //  1 bit Controller Save State       = 0b             RW
      bool   lhcrst;         //  1 bit Light HC Reset              = 0b             RW or RO (HCCPARAMS:LHRC)
      Bit8u  RsvdP0;         //  1 bit reserved and preserved      = 000b           RW
      bool   hsee;           //  1 bit Host System Error Enable    = 0b             RW
      bool   inte;           //  1 bit Interrupter Enable          = 0b             RW
      bool   hcrst;          //  1 bit HC Reset                    = 0b             RW
      bool   rs;             //  1 bit Run Stop                    = 0b             RW
    } HcCommand;             //                                    = 0x00000000
    struct {
      Bit32u RsvdZ1;         // 19 bit reserved and zero'd         = 0x000000       RW
      bool   hce;            //  1 bit Host Controller Error       = 0b             RO
      bool   cnr;            //  1 bit Controller Not Ready        = 0b             R0
      bool   sre;            //  1 bit Save/Restore Error          = 0b             RW1C
      bool   rss;            //  1 bit Restore State Status        = 0b             RO
      bool   sss;            //  1 bit Save State Status           = 0b             RO
      Bit8u  RsvdZ0;         //  3 bit reserved and zero'd         = 0x0            RW
      bool   pcd;            //  1 bit Port Change Detect          = 0b             RW1C
      bool   eint;           //  1 bit Event Interrupt             = 0b             RW1C
      bool   hse;            //  1 bit Host System Error           = 0b             RW1C
      bool   RsvdZ2;         //  1 bit reserved and zero'd         = 0b             RW
      bool   hch;            //  1 bit HCHalted                    = 1b             RO
    } HcStatus;              //                                    = 0x00000001
    struct {
      Bit16u  Rsvd;          // 16 bit reserved                    = 0x0000         RO
      Bit16u  pagesize;      // 16 bit reserved                    = 0x0001         RO
    } HcPageSize;            //                                    = 0x00000001
    struct {
      Bit16u  RsvdP;         // 16 bit reserved and presserved     = 0x0000         RW
      bool n15;              //  1 bit N15                         = 0              RW
      bool n14;              //  1 bit N14                         = 0              RW
      bool n13;              //  1 bit N13                         = 0              RW
      bool n12;              //  1 bit N12                         = 0              RW
      bool n11;              //  1 bit N11                         = 0              RW
      bool n10;              //  1 bit N10                         = 0              RW
      bool n9;               //  1 bit N9                          = 0              RW
      bool n8;               //  1 bit N8                          = 0              RW
      bool n7;               //  1 bit N7                          = 0              RW
      bool n6;               //  1 bit N6                          = 0              RW
      bool n5;               //  1 bit N5                          = 0              RW
      bool n4;               //  1 bit N4                          = 0              RW
      bool n3;               //  1 bit N3                          = 0              RW
      bool n2;               //  1 bit N2                          = 0              RW
      bool n1;               //  1 bit N1                          = 0              RW
      bool n0;               //  1 bit N0                          = 0              RW
    } HcNotification;        //                                    = 0x00000000
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
    struct {
      Bit32u RsvdP;          // 22/24 bit reserved and preserved   = 0x000000       RW
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
      bool   u3e;            //  1 bit U3 Entry Enable             = 0              RW
      bool   cie;            //  1 bit Config Info Enable          = 0              RW
#endif
      Bit8u   MaxSlotsEn;    //  8 bit Max Device Slots Enabled    = 0x00           RW
    } HcConfig;
  } op_regs;

  struct {
    // our data
    usb_device_c *device; // device connected to this port
    bool is_usb3;         // set if usb3 port, cleared if usb2 port.
    bool has_been_reset;  // set if the port has been reset aftet powered up.
    Bit8u psceg;          // current port status change event

    struct {
      bool  wpr;               //  1 bit Warm Port Reset             = 0b             RW or RsvdZ
      bool  dr;                //  1 bit Device Removable            = 0b             RO
      Bit8u RsvdZ1;            //  2 bit Reserved and Zero'd         = 00b            RW
      bool  woe;               //  1 bit Wake on Over Current Enable = 0b             RW
      bool  wde;               //  1 bit Wake on Disconnect Enable   = 0b             RW
      bool  wce;               //  1 bit Wake on Connect Enable      = 0b             RW
      bool  cas;               //  1 bit Cold Attach Status          = 0b             RO
      bool  cec;               //  1 bit Port Config Error Change    = 0b             RW1C or RsvdZ
      bool  plc;               //  1 bit Port Link State Change      = 0b             RW1C
      bool  prc;               //  1 bit Port Reset Change           = 0b             RW1C
      bool  occ;               //  1 bit Over Current Change         = 0b             RW1C
      bool  wrc;               //  1 bit Warm Port Reset Change      = 0b             RW1C or RsvdZ
      bool  pec;               //  1 bit Port Enabled/Disabled Change= 0b             RW1C
      bool  csc;               //  1 bit Connect Status Change       = 0b             RW1C
      bool  lws;               //  1 bit Port Link State Write Strobe= 0b             RW
      Bit8u pic;               //  2 bit Port Indicator Control      = 00b            RW
      Bit8u speed;             //  4 bit Port Speed                  = 0000b          RO
      bool  pp;                //  1 bit Port Power                  = 0b             RW
      Bit8u pls;               //  4 bit Port Link State             = 0x00           RW
      bool  pr;                //  1 bit Port Reset                  = 0b             RW
      bool  oca;               //  1 bit Over Current Active         = 0b             RO
      bool  RsvdZ0;            //  1 bit Reserved and Zero'd         = 0b             RW
      bool  ped;               //  1 bit Port Enabled/Disabled       = 0b             RW1C
      bool  ccs;               //  1 bit Current Connect Status      = 0b             RO
    } portsc;
    union {
      // if usb3 port
      struct {
        struct {
          Bit16u  RsvdP;         // 15 bit Reserved and Preserved      = 0x0000         RW
          bool    fla;           //  1 bit Force Link PM Accept        = 0x0000         RW
          Bit8u   u2timeout;     //  8 bit U2 Timeout                  = 0x0000         RW
          Bit8u   u1timeout;     //  8 bit U1 Timeout                  = 0x0000         RW
        } portpmsc;
        struct {
          Bit16u  RsvdP;         // 16 bit Reserved and Preserved      = 0x0000         RW
          Bit16u  lec;           // 16 bit Link Error Count            = 0x0000         RO
        } portli;
      } usb3;
      // if usb2 port
      struct {
        struct {
          Bit8u   tmode;         //  4 bit Test Mode                   = 0x0            RO
          Bit16u  RsvdP;         // 11 bit reserved and preseved       = 0x000          RW
          bool    hle;           //  1 bit hardware LPM enable         = 0b             RW
          Bit8u   l1dslot;       //  8 bit L1 Device Slot              = 0x00           RW
          Bit8u   hird;          //  4 bit Host Initiated Resume Durat = 0x0            RW
          bool    rwe;           //  1 bit Remote Wakeup Enable        = 0b             RW
          Bit8u   l1s;           //  3 bit L1 Status                   = 000b           RO
        } portpmsc;
        struct {
          Bit32u  RsvdP;         // 32 bit reserved and preseved       = 0x00000000     RW
        } portli;
      } usb2;
    };
    struct {
      Bit8u   hirdm;             //  2 bit host initiated resume duration mode
      Bit8u   l1timeout;         //  8 bit L1 timeout
      Bit8u   hirdd;             //  4 bit host initiated resume duration deep
      Bit32u  RsvdP;             // 18 bit reserved and preseved       = 0x00000000     RW
    } porthlpmc;
  } usb_port[USB_XHCI_PORTS_MAX];

  // Extended Caps Registers
  Bit8u extended_caps[EXT_CAPS_SIZE];

  struct XHCI_RUNTIME_REGS {
    struct {
      Bit32u RsvdP;              // 18 bit reserved and preseved       = 0x00000        RW
      Bit16u index;              // 14 bit index                       = 0x0000         RO
    } mfindex;
    struct {
      struct {
        Bit32u  RsvdP;           // 30 bit reserved and preseved       = 0x00000000     RW
        bool    ie;              //  1 bit Interrupt Enable            = 0b             RW
        bool    ip;              //  1 bit Interrupt Pending           = 0b             RW1C
      } iman;
      struct {
        Bit16u  imodc;           // 16 bit Interrupter Mod Counter     = 0x0000         RW
        Bit16u  imodi;           // 16 bit Interrupter Mod Interval    = 0x0000         RW
      } imod;
      struct {
        Bit16u  RsvdP;           // 16 bit reserved and preseved       = 0x0000         RW
        Bit16u  erstabsize;      // 16 bit Event Ring Seg Table Size   = 0x0000         RW
      } erstsz;
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

  BX_XHCI_SMF void update_irq(unsigned interrupter);

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
  BX_XHCI_SMF Bit32u NEC_verification(Bit64u parameter);
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
