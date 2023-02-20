/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Experimental USB EHCI adapter (partly ported from Qemu)
//
//  Copyright (C) 2015-2023  The Bochs Project
//
//  Copyright(c) 2008  Emutex Ltd. (address@hidden)
//  Copyright(c) 2011-2012 Red Hat, Inc.
//
//  Red Hat Authors:
//  Gerd Hoffmann <kraxel@redhat.com>
//  Hans de Goede <hdegoede@redhat.com>
//
//  EHCI project was started by Mark Burkley, with contributions by
//  Niels de Vos.  David S. Ahern continued working on it.  Kevin Wolf,
//  Jan Kiszka and Vincent Palatin contributed bugfixes.
//
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

#ifndef BX_IODEV_USB_EHCI_H
#define BX_IODEV_USB_EHCI_H

#if BX_USE_USB_EHCI_SMF
#  define BX_EHCI_THIS theUSB_EHCI->
#  define BX_EHCI_THIS_PTR theUSB_EHCI
#else
#  define BX_EHCI_THIS this->
#  define BX_EHCI_THIS_PTR this
#endif

#define USB_EHCI_PORTS  6

/*  EHCI spec version 1.0 Section 3.3
 */
typedef struct EHCIitd {
    Bit32u next;

    Bit32u transact[8];
#define ITD_XACT_ACTIVE          (1 << 31)
#define ITD_XACT_DBERROR         (1 << 30)
#define ITD_XACT_BABBLE          (1 << 29)
#define ITD_XACT_XACTERR         (1 << 28)
#define ITD_XACT_LENGTH_MASK     0x0fff0000
#define ITD_XACT_LENGTH_SH       16
#define ITD_XACT_IOC             (1 << 15)
#define ITD_XACT_PGSEL_MASK      0x00007000
#define ITD_XACT_PGSEL_SH        12
#define ITD_XACT_OFFSET_MASK     0x00000fff

    Bit32u bufptr[7];
#define ITD_BUFPTR_MASK          0xfffff000
#define ITD_BUFPTR_SH            12
#define ITD_BUFPTR_EP_MASK       0x00000f00
#define ITD_BUFPTR_EP_SH         8
#define ITD_BUFPTR_DEVADDR_MASK  0x0000007f
#define ITD_BUFPTR_DEVADDR_SH    0
#define ITD_BUFPTR_DIRECTION     (1 << 11)
#define ITD_BUFPTR_MAXPKT_MASK   0x000007ff
#define ITD_BUFPTR_MAXPKT_SH     0
#define ITD_BUFPTR_MULT_MASK     0x00000003
#define ITD_BUFPTR_MULT_SH       0
} EHCIitd;

/*  EHCI spec version 1.0 Section 3.4
 */
typedef struct EHCIsitd {
    Bit32u next;                  // Standard next link pointer
    Bit32u epchar;
#define SITD_EPCHAR_IO              (1 << 31)
#define SITD_EPCHAR_PORTNUM_MASK    0x7f000000
#define SITD_EPCHAR_PORTNUM_SH      24
#define SITD_EPCHAR_HUBADD_MASK     0x007f0000
#define SITD_EPCHAR_HUBADDR_SH      16
#define SITD_EPCHAR_EPNUM_MASK      0x00000f00
#define SITD_EPCHAR_EPNUM_SH        8
#define SITD_EPCHAR_DEVADDR_MASK    0x0000007f

    Bit32u uframe;
#define SITD_UFRAME_CMASK_MASK      0x0000ff00
#define SITD_UFRAME_CMASK_SH        8
#define SITD_UFRAME_SMASK_MASK      0x000000ff

    Bit32u results;
#define SITD_RESULTS_IOC              (1 << 31)
#define SITD_RESULTS_PGSEL            (1 << 30)
#define SITD_RESULTS_TBYTES_MASK      0x03ff0000
#define SITD_RESULTS_TYBYTES_SH       16
#define SITD_RESULTS_CPROGMASK_MASK   0x0000ff00
#define SITD_RESULTS_CPROGMASK_SH     8
#define SITD_RESULTS_ACTIVE           (1 << 7)
#define SITD_RESULTS_ERR              (1 << 6)
#define SITD_RESULTS_DBERR            (1 << 5)
#define SITD_RESULTS_BABBLE           (1 << 4)
#define SITD_RESULTS_XACTERR          (1 << 3)
#define SITD_RESULTS_MISSEDUF         (1 << 2)
#define SITD_RESULTS_SPLITXSTATE      (1 << 1)

    Bit32u bufptr[2];
#define SITD_BUFPTR_MASK              0xfffff000
#define SITD_BUFPTR_CURROFF_MASK      0x00000fff
#define SITD_BUFPTR_TPOS_MASK         0x00000018
#define SITD_BUFPTR_TPOS_SH           3
#define SITD_BUFPTR_TCNT_MASK         0x00000007

    Bit32u backptr;                 // Standard next link pointer
} EHCIsitd;

/*  EHCI spec version 1.0 Section 3.5
 */
typedef struct EHCIqtd {
    Bit32u next;                    // Standard next link pointer
    Bit32u altnext;                 // Standard next link pointer
    Bit32u token;
#define QTD_TOKEN_DTOGGLE             (1 << 31)
#define QTD_TOKEN_TBYTES_MASK         0x7fff0000
#define QTD_TOKEN_TBYTES_SH           16
#define QTD_TOKEN_IOC                 (1 << 15)
#define QTD_TOKEN_CPAGE_MASK          0x00007000
#define QTD_TOKEN_CPAGE_SH            12
#define QTD_TOKEN_CERR_MASK           0x00000c00
#define QTD_TOKEN_CERR_SH             10
#define QTD_TOKEN_PID_MASK            0x00000300
#define QTD_TOKEN_PID_SH              8
#define QTD_TOKEN_ACTIVE              (1 << 7)
#define QTD_TOKEN_HALT                (1 << 6)
#define QTD_TOKEN_DBERR               (1 << 5)
#define QTD_TOKEN_BABBLE              (1 << 4)
#define QTD_TOKEN_XACTERR             (1 << 3)
#define QTD_TOKEN_MISSEDUF            (1 << 2)
#define QTD_TOKEN_SPLITXSTATE         (1 << 1)
#define QTD_TOKEN_PING                (1 << 0)

    Bit32u bufptr[5];               // Standard buffer pointer
#define QTD_BUFPTR_MASK               0xfffff000
#define QTD_BUFPTR_SH                 12
} EHCIqtd;

/*  EHCI spec version 1.0 Section 3.6
 */
typedef struct EHCIqh {
    Bit32u next;                    // Standard next link pointer

    /* endpoint characteristics */
    Bit32u epchar;
#define QH_EPCHAR_RL_MASK             0xf0000000
#define QH_EPCHAR_RL_SH               28
#define QH_EPCHAR_C                   (1 << 27)
#define QH_EPCHAR_MPLEN_MASK          0x07FF0000
#define QH_EPCHAR_MPLEN_SH            16
#define QH_EPCHAR_H                   (1 << 15)
#define QH_EPCHAR_DTC                 (1 << 14)
#define QH_EPCHAR_EPS_MASK            0x00003000
#define QH_EPCHAR_EPS_SH              12
#define EHCI_QH_EPS_FULL              0
#define EHCI_QH_EPS_LOW               1
#define EHCI_QH_EPS_HIGH              2
#define EHCI_QH_EPS_RESERVED          3

#define QH_EPCHAR_EP_MASK             0x00000f00
#define QH_EPCHAR_EP_SH               8
#define QH_EPCHAR_I                   (1 << 7)
#define QH_EPCHAR_DEVADDR_MASK        0x0000007f
#define QH_EPCHAR_DEVADDR_SH          0

    /* endpoint capabilities */
    Bit32u epcap;
#define QH_EPCAP_MULT_MASK            0xc0000000
#define QH_EPCAP_MULT_SH              30
#define QH_EPCAP_PORTNUM_MASK         0x3f800000
#define QH_EPCAP_PORTNUM_SH           23
#define QH_EPCAP_HUBADDR_MASK         0x007f0000
#define QH_EPCAP_HUBADDR_SH           16
#define QH_EPCAP_CMASK_MASK           0x0000ff00
#define QH_EPCAP_CMASK_SH             8
#define QH_EPCAP_SMASK_MASK           0x000000ff
#define QH_EPCAP_SMASK_SH             0

    Bit32u current_qtd;             // Standard next link pointer
    Bit32u next_qtd;                // Standard next link pointer
    Bit32u altnext_qtd;
#define QH_ALTNEXT_NAKCNT_MASK        0x0000001e
#define QH_ALTNEXT_NAKCNT_SH          1

    Bit32u token;                   // Same as QTD token
    Bit32u bufptr[5];               // Standard buffer pointer
#define BUFPTR_CPROGMASK_MASK         0x000000ff
#define BUFPTR_FRAMETAG_MASK          0x0000001f
#define BUFPTR_SBYTES_MASK            0x00000fe0
#define BUFPTR_SBYTES_SH              5
} EHCIqh;

typedef struct EHCIPacket EHCIPacket;
typedef struct EHCIQueue EHCIQueue;

typedef QTAILQ_HEAD(EHCIQueueHead, EHCIQueue) EHCIQueueHead;

typedef struct {
  int frame_timer_index;

  Bit8u  usbsts_pending;
  Bit32u usbsts_frindex;
  EHCIQueueHead aqueues;
  EHCIQueueHead pqueues;
  Bit32u pstate, astate;
  /* which address to look at next */
  Bit32u a_fetch_addr;
  Bit32u p_fetch_addr;

  Bit64u last_run_usec;
  Bit32u async_stepdown;

  struct {
    Bit8u  CapLength;
    Bit8u  Reserved;
    Bit16u HciVersion;
    Bit32u HcsParams;
    Bit32u HccParams;
  } cap_regs;

  struct {
    struct {
      Bit8u itc;
      bool  iaad;
      bool  ase;
      bool  pse;
      bool  hcreset;
      bool  rs;
    } UsbCmd;
    struct {
      bool  ass;
      bool  pss;
      bool  recl;
      bool  hchalted;
      Bit8u inti;
    } UsbSts;
    Bit8u  UsbIntr;
    Bit32u FrIndex;
    Bit32u CtrlDsSegment;
    Bit32u PeriodicListBase;
    Bit32u AsyncListAddr;
    Bit32u ConfigFlag;
  } op_regs;

  struct {
    // our data
    usb_device_c *device;   // device connected to this port
    bool owner_change;
    struct {
      bool  woe;
      bool  wde;
      bool  wce;
      Bit8u ptc;
      Bit8u pic;
      bool  po;
      bool  pp;
      Bit8u ls;
      bool  pr;
      bool  sus;
      bool  fpr;
      bool  occ;
      bool  oca;
      bool  pec;
      bool  ped;
      bool  csc;
      bool  ccs;
    } portsc;
  } usb_port[USB_EHCI_PORTS];

} bx_usb_ehci_t;

enum async_state {
    EHCI_ASYNC_NONE = 0,
    EHCI_ASYNC_INITIALIZED,
    EHCI_ASYNC_INFLIGHT,
    EHCI_ASYNC_FINISHED,
};

struct EHCIPacket {
    EHCIQueue *queue;
    QTAILQ_ENTRY(EHCIPacket) next;

    EHCIqtd qtd;         /* copy of current QTD (being worked on) */
    Bit32u qtdaddr;      /* address QTD read from                 */

    USBPacket packet;
    int pid;
    Bit32u tbytes;
    enum async_state async;
    int usb_status;
};

struct EHCIQueue {
    bx_usb_ehci_t *ehci;
    QTAILQ_ENTRY(EHCIQueue) next;
    Bit32u seen;
    Bit64u ts;
    int async;

    /* cached data from guest - needs to be flushed
     * when guest removes an entry (doorbell, handshake sequence)
     */
    EHCIqh qh;           /* copy of current QH (being worked on) */
    Bit32u qhaddr;       /* address QH read from                 */
    Bit32u qtdaddr;      /* address QTD read from                */
    usb_device_c *dev;
    QTAILQ_HEAD(, EHCIPacket) packets;
};

class bx_usb_ehci_c : public bx_pci_device_c {
public:
  bx_usb_ehci_c();
  virtual ~bx_usb_ehci_c();
  virtual void init(void);
  virtual void reset(unsigned);
  virtual void register_state(void);
  virtual void after_restore_state(void);
  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  void event_handler(int event, USBPacket *packet, int port);

private:
  bx_uhci_core_c *uhci[3];
  bx_usb_ehci_t hub;
  Bit8u         devfunc;
  Bit8u         device_change;
  int           rt_conf_id;
  Bit32u        maxframes;

  void reset_hc(void);
  void reset_port(int);

  static void init_device(Bit8u port, bx_list_c *portconf);
  static void remove_device(Bit8u port);
  static bool set_connect_status(Bit8u port, bool connected);
  static void change_port_owner(int port);

  // EHCI core methods ported from QEMU 1.2.2
  void update_irq(void);
  void raise_irq(Bit8u intr);
  void commit_irq(void);
  void update_halt(void);

  void set_state(int async, int state);
  int get_state(int async);
  void set_fetch_addr(int async, Bit32u addr);
  Bit32u get_fetch_addr(int async);

  bool async_enabled(void);
  bool periodic_enabled(void);

  EHCIPacket *alloc_packet(EHCIQueue *q);
  void free_packet(EHCIPacket *p);
  EHCIQueue *alloc_queue(Bit32u addr, int async);
  int cancel_queue(EHCIQueue *q);
  int reset_queue(EHCIQueue *q);
  void free_queue(EHCIQueue *q, const char *warn);
  EHCIQueue *find_queue_by_qh(Bit32u addr, int async);
  void queues_rip_unused(int async);
  void queues_rip_unseen(int async);
  void queues_rip_device(usb_device_c *dev, int async);
  void queues_rip_all(int async);

  usb_device_c *find_device(Bit8u addr);
  void flush_qh(EHCIQueue *q);
  int qh_do_overlay(EHCIQueue *q);
  int transfer(EHCIPacket *p);
  void finish_transfer(EHCIQueue *q, int status);
  void execute_complete(EHCIQueue *q);
  int execute(EHCIPacket *p);
  int process_itd(EHCIitd *itd, Bit32u addr);

  int state_waitlisthead(int async);
  int state_fetchentry(int async);
  EHCIQueue *state_fetchqh(int async);
  int state_fetchitd(int async);
  int state_fetchsitd(int async);
  int state_advqueue(EHCIQueue *q);
  int state_fetchqtd(EHCIQueue *q);
  int state_horizqh(EHCIQueue *q);
  int fill_queue(EHCIPacket *p);
  int state_execute(EHCIQueue *q);
  int state_executing(EHCIQueue *q);
  int state_writeback(EHCIQueue *q);

  void advance_state(int async);
  void advance_async_state(void);
  void advance_periodic_state(void);
  void update_frindex(int frames);

  // EHCI frame timer
  static void ehci_frame_handler(void *);
  void ehci_frame_timer(void);

#if BX_USE_USB_EHCI_SMF
  static bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#else
  bool read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  bool write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#endif

  static void runtime_config_handler(void *);
  void runtime_config(void);

  static Bit64s usb_param_handler(bx_param_c *param, bool set, Bit64s val);
  static bool usb_param_enable_handler(bx_param_c *param, bool en);
};

#endif  // BX_IODEV_USB_EHCI_H
