/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB hub emulation support (ported from QEMU)
//
// Copyright (C) 2005       Fabrice Bellard
// Copyright (C) 2009-2023  The Bochs Project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
/////////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "usb_common.h"
#include "usb_hub.h"

#define LOG_THIS

// USB device plugin entry point

PLUGIN_ENTRY_FOR_MODULE(usb_hub)
{
  if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_USB;
  }
  return 0; // Success
}

//
// Define the static class that registers the derived USB device class,
// and allocates one on request.
//
class bx_usb_hub_locator_c : public usbdev_locator_c {
public:
  bx_usb_hub_locator_c(void) : usbdev_locator_c("usb_hub") {}
protected:
  usb_device_c *allocate(const char *devname) {
    return (new usb_hub_device_c());
  }
} bx_usb_hub_match;

#define PORT_STAT_CONNECTION    0x0001
#define PORT_STAT_ENABLE        0x0002
#define PORT_STAT_SUSPEND       0x0004
#define PORT_STAT_OVERCURRENT   0x0008
#define PORT_STAT_RESET         0x0010
#define PORT_STAT_POWER         0x0100
#define PORT_STAT_LOW_SPEED     0x0200
#define PORT_STAT_HIGH_SPEED    0x0400
#define PORT_STAT_TEST          0x0800
#define PORT_STAT_INDICATOR     0x1000

#define PORT_STAT_C_CONNECTION  0x0001
#define PORT_STAT_C_ENABLE      0x0002
#define PORT_STAT_C_SUSPEND     0x0004
#define PORT_STAT_C_OVERCURRENT 0x0008
#define PORT_STAT_C_RESET       0x0010

#define PORT_CONNECTION         0
#define PORT_ENABLE             1
#define PORT_SUSPEND            2
#define PORT_OVERCURRENT        3
#define PORT_RESET              4
#define PORT_POWER              8
#define PORT_LOWSPEED           9
#define PORT_HIGHSPEED          10
#define PORT_C_CONNECTION       16
#define PORT_C_ENABLE           17
#define PORT_C_SUSPEND          18
#define PORT_C_OVERCURRENT      19
#define PORT_C_RESET            20
#define PORT_TEST               21
#define PORT_INDICATOR          22

static Bit32u serial_number = 1234;

// If you change any of the Max Packet Size, or other fields within these
//  descriptors, you must also change the d.endpoint_info[] array
//  to match your changes.

static const Bit8u bx_hub_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x10, 0x01, /*  u16 bcdUSB; v1.1 */

  0x09,       /*  u8  bDeviceClass; HUB_CLASSCODE */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x40,       /*  u8  bMaxPacketSize; 64 Bytes */

  0x09, 0x04, /*  u16 idVendor; */
  0x5A, 0x00, /*  u16 idProduct; */
  0x00, 0x01, /*  u16 bcdDevice */

  0x01,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x03,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

/* XXX: patch interrupt size */
static Bit8u bx_hub_config_descriptor[] = {

  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x19, 0x00, /*  u16 wTotalLength; */
  0x01,       /*  u8  bNumInterfaces; (1) */
  0x01,       /*  u8  bConfigurationValue; */
  0x00,       /*  u8  iConfiguration; */
  0xE0,       /*  u8  bmAttributes;
                        Bit 7: must be set,
                            6: Self-powered,
                            5: Remote wakeup,
                         4..0: resvd */
  0x32,       /*  u8  MaxPower; */

  /* USB 1.1:
   * USB 2.0, single TT organization (mandatory):
   *    one interface, protocol 0
   *
   * USB 2.0, multiple TT organization (optional):
   *    two interfaces, protocols 1 (like single TT)
   *    and 2 (multiple TT mode) ... config is
   *    sometimes settable
   *    NOT IMPLEMENTED
   */

   /* one interface */
  0x09,       /*  u8  if_bLength; */
  0x04,       /*  u8  if_bDescriptorType; Interface */
  0x00,       /*  u8  if_bInterfaceNumber; */
  0x00,       /*  u8  if_bAlternateSetting; */
  0x01,       /*  u8  if_bNumEndpoints; */
  0x09,       /*  u8  if_bInterfaceClass; HUB_CLASSCODE */
  0x00,       /*  u8  if_bInterfaceSubClass; */
  0x00,       /*  u8  if_bInterfaceProtocol; [usb1.1 or single tt] */
  0x00,       /*  u8  if_iInterface; */

  /* one endpoint (status change endpoint) */
  0x07,       /*  u8  ep_bLength; */
  0x05,       /*  u8  ep_bDescriptorType; Endpoint */
  0x81,       /*  u8  ep_bEndpointAddress; IN Endpoint 1 */
  0x03,       /*  u8  ep_bmAttributes; Interrupt */
  0x02, 0x00, /*  u16 ep_wMaxPacketSize; 1 + (USB_HUB_MAX_PORTS / 8) */
  0xff        /*  u8  ep_bInterval; (255ms -- usb 2.0 spec) */
};

static const Bit8u bx_hub_hub_descriptor[] =
{
  0x00,       /*  u8  bLength; patched in later */
  0x29,       /*  u8  bDescriptorType; Hub-descriptor */
  0x00,       /*  u8  bNbrPorts; (patched later) */
  0x0a,       /* u16  wHubCharacteristics; */
  0x00,       /*   (per-port OC, no power switching) */
  0x01,       /*  u8  bPwrOn2pwrGood; 2ms */
  0x00        /*  u8  bHubContrCurrent; 0 mA */

  /* DeviceRemovable and PortPwrCtrlMask patched in later */
};

static Bit8u hub_count = 0;

void usb_hub_restore_handler(void *dev, bx_list_c *conf);

usb_hub_device_c::usb_hub_device_c()
{
  char pname[10];
  char label[32];

  d.speed = d.minspeed = d.maxspeed = USB_SPEED_FULL;
  strcpy(d.devname, "Bochs USB HUB");
  d.dev_descriptor = bx_hub_dev_descriptor;
  d.device_desc_size = sizeof(bx_hub_dev_descriptor);
  d.config_descriptor = bx_hub_config_descriptor;
  d.config_desc_size = sizeof(bx_hub_config_descriptor);
  d.endpoint_info[USB_CONTROL_EP].max_packet_size = 8; // Control ep0
  d.endpoint_info[USB_CONTROL_EP].max_burst_size = 0;
  d.endpoint_info[1].max_packet_size = 2;  // In ep1
  d.endpoint_info[1].max_burst_size = 0;
  d.vendor_desc = "BOCHS";
  d.product_desc = "BOCHS USB HUB";
  memset((void*)&hub, 0, sizeof(hub));
  sprintf(hub.serial_number, "%d", serial_number++);
  d.serial_num = hub.serial_number;
  hub.device_change = 0;
  hub.n_ports = USB_HUB_DEF_PORTS;

  // prepare runtime config options
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  sprintf(pname, "exthub%u", ++hub_count);
  sprintf(label, "External Hub #%u Configuration", hub_count);
  hub.config = new bx_list_c(usb_rt, pname, label);
  hub.config->set_options(bx_list_c::SHOW_PARENT);
  hub.config->set_device_param(this);

  put("usb_hub", "USBHUB");
}

usb_hub_device_c::~usb_hub_device_c(void)
{
  for (int i = 0; i < hub.n_ports; i++) {
    remove_device(i);
  }
  if (SIM->is_wx_selected()) {
    bx_list_c *usb = (bx_list_c *) SIM->get_param("ports.usb");
    usb->remove(hub.config->get_name());
  }
  bx_list_c *usb_rt = (bx_list_c *) SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove(hub.config->get_name());
}

bool usb_hub_device_c::set_option(const char *option)
{
  if (!strncmp(option, "ports:", 6)) {
    hub.n_ports = atoi(&option[6]);
    if ((hub.n_ports < 2) || (hub.n_ports > USB_HUB_MAX_PORTS)) {
      BX_ERROR(("ignoring invalid number of ports (%d)", hub.n_ports));
      hub.n_ports = USB_HUB_DEF_PORTS;
    }
    return 1;
  }
  return 0;
}

bool usb_hub_device_c::init()
{
  int i;
  char pname[10];
  char label[32];
  bx_list_c *port, *deplist;
  bx_param_enum_c *device;
  bx_param_string_c *options;
  bx_param_bool_c *overcurrent;

  // set up config descriptor, status and runtime config for hub.n_ports
  bx_hub_config_descriptor[22] = (hub.n_ports + 1 + 7) / 8;
  for(i = 0; i < hub.n_ports; i++) {
    hub.usb_port[i].PortStatus = PORT_STAT_POWER;
    hub.usb_port[i].PortChange = 0;
  }
  for(i = 0; i < hub.n_ports; i++) {
    sprintf(pname, "port%d", i+1);
    sprintf(label, "Port #%d Configuration", i+1);
    port = new bx_list_c(hub.config, pname, label);
    port->set_options(port->SERIES_ASK | port->USE_BOX_TITLE);
    device = new bx_param_enum_c(port, "device", "Device", "",
      bx_usbdev_ctl.get_device_names(), 0, 0);
    device->set_handler(hub_param_handler);
    options = new bx_param_string_c(port, "options", "Options", "", "",
      BX_PATHNAME_LEN);
    options->set_enable_handler(hub_param_enable_handler);
    overcurrent = new bx_param_bool_c(port, 
      "over_current",
      "signal over-current",
      "signal over-current", 0);
    overcurrent->set_handler(hub_param_oc_handler);
    deplist = new bx_list_c(NULL);
    deplist->add(options);
    deplist->add(overcurrent);
    device->set_dependent_list(deplist, 1);
    device->set_dependent_bitmap(0, 0);
  }
  if (SIM->is_wx_selected()) {
    bx_list_c *usb = (bx_list_c *) SIM->get_param("ports.usb");
    usb->add(hub.config);
  }
  sprintf(hub.info_txt, "%d-port USB hub", hub.n_ports);
  d.connected = 1;
  d.alt_iface_max = 0;
  return 1;
}

const char *usb_hub_device_c::get_info()
{
  return hub.info_txt;
}

void usb_hub_device_c::register_state_specific(bx_list_c *parent)
{
  Bit8u i;
  char portnum[16];
  bx_list_c *port, *pconf, *config;

  hub.state = new bx_list_c(parent, "hub", "USB HUB Device State");
  for (i=0; i<hub.n_ports; i++) {
    sprintf(portnum, "port%d", i+1);
    port = new bx_list_c(hub.state, portnum);
    pconf = (bx_list_c*)hub.config->get_by_name(portnum);
    config = new bx_list_c(port, portnum);
    config->add(pconf->get_by_name("device"));
    config->add(pconf->get_by_name("options"));
    config->set_restore_handler(this, usb_hub_restore_handler);
    BXRS_HEX_PARAM_FIELD(port, PortStatus, hub.usb_port[i].PortStatus);
    BXRS_HEX_PARAM_FIELD(port, PortChange, hub.usb_port[i].PortChange);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }
}

void usb_hub_device_c::after_restore_state()
{
  for (int i=0; i<hub.n_ports; i++) {
    if (hub.usb_port[i].device != NULL) {
      hub.usb_port[i].device->after_restore_state();
    }
  }
}

void usb_hub_device_c::handle_reset()
{
  int i;

  BX_DEBUG(("Reset"));
  for (i = 0; i < hub.n_ports; i++) {
    hub.usb_port[i].PortStatus = PORT_STAT_POWER;
    hub.usb_port[i].PortChange = 0;
    if (hub.usb_port[i].device != NULL) {
      hub.usb_port[i].PortStatus |= PORT_STAT_CONNECTION;
      hub.usb_port[i].PortChange |= PORT_STAT_C_CONNECTION;
      if (hub.usb_port[i].device->get_speed() == USB_SPEED_LOW) {
        hub.usb_port[i].PortStatus |= PORT_STAT_LOW_SPEED;
      }
    }
  }
}

int usb_hub_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret = 0;
  unsigned int n;

  ret = handle_control_common(request, value, index, length, data);
  if (ret >= 0) {
    return ret;
  }

  ret = 0;
  switch(request) {
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      goto fail;
      break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
      if (value == 0 && index != 0x81) { /* clear ep halt */
        goto fail;
      }
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      goto fail;
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
      switch(value >> 8) {
        case USB_DT_STRING:
          switch (value & 0xFF) {
            case 0xEE:
              // Microsoft OS Descriptor check
              // We don't support this check, so fail
              BX_INFO(("USB HUB handle_control: Microsoft OS specific 0xEE string descriptor"));
              goto fail;
              break;
            default:
              BX_ERROR(("unknown string descriptor type %d", value & 0xff));
              goto fail;
              break;
          }
          break;
        default:
          BX_ERROR(("unknown descriptor type: 0x%02x", (value >> 8)));
          goto fail;
      }
      break;
      /* hub specific requests */
    case DeviceClassInRequest | USB_REQ_GET_STATUS:
      if (d.state == USB_STATE_CONFIGURED) {
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        ret = 4;
      } else
        goto fail;
      break;
    case OtherInClassRequest | USB_REQ_GET_STATUS:
      n = index - 1;
      if (n >= hub.n_ports)
        goto fail;
      data[0] = (hub.usb_port[n].PortStatus & 0xff);
      data[1] = (hub.usb_port[n].PortStatus >> 8);
      data[2] = (hub.usb_port[n].PortChange & 0xff);
      data[3] = (hub.usb_port[n].PortChange >> 8);
      ret = 4;
      break;
    case DeviceOutClassRequest | USB_REQ_SET_FEATURE: 
    case DeviceOutClassRequest | USB_REQ_CLEAR_FEATURE:
      if (value == 0 || value == 1) {
        ;
      } else {
        goto fail;
      }
      ret = 0;
      break;
    case OtherOutClassRequest | USB_REQ_SET_FEATURE:
      n = index - 1;
      if (n >= hub.n_ports)
        goto fail;
      switch(value) {
        case PORT_SUSPEND:
          hub.usb_port[n].PortStatus |= PORT_STAT_SUSPEND;
          break;
        case PORT_RESET:
          if (hub.usb_port[n].device != NULL) {
            hub.usb_port[n].device->usb_send_msg(USB_MSG_RESET);
            hub.usb_port[n].PortChange |= PORT_STAT_C_RESET;
            /* set enable bit */
            hub.usb_port[n].PortStatus |= PORT_STAT_ENABLE;
          }
          break;
        case PORT_POWER:
          break;
        default:
          BX_ERROR(("Unknown SetPortFeature: %d", value));
          goto fail;
      }
      ret = 0;
      break;
    case OtherOutClassRequest | USB_REQ_CLEAR_FEATURE:
      n = index - 1;
      if (n >= hub.n_ports)
        goto fail;
      switch(value) {
        case PORT_ENABLE:
          hub.usb_port[n].PortStatus &= ~PORT_STAT_ENABLE;
          break;
        case PORT_C_ENABLE:
          hub.usb_port[n].PortChange &= ~PORT_STAT_C_ENABLE;
          break;
        case PORT_SUSPEND:
          hub.usb_port[n].PortStatus &= ~PORT_STAT_SUSPEND;
          break;
        case PORT_C_SUSPEND:
          hub.usb_port[n].PortChange &= ~PORT_STAT_C_SUSPEND;
          break;
        case PORT_C_CONNECTION:
          hub.usb_port[n].PortChange &= ~PORT_STAT_C_CONNECTION;
          break;
        case PORT_C_OVERCURRENT:
          hub.usb_port[n].PortChange &= ~PORT_STAT_C_OVERCURRENT;
          break;
        case PORT_C_RESET:
          hub.usb_port[n].PortChange &= ~PORT_STAT_C_RESET;
          break;
        default:
          BX_ERROR(("Unknown ClearPortFeature: %d", value));
          goto fail;
      }
      ret = 0;
      break;
    case DeviceClassInRequest | USB_REQ_GET_DESCRIPTOR:
      {
        unsigned int limit, var_hub_size = 0;
        memcpy(data, bx_hub_hub_descriptor,
                sizeof(bx_hub_hub_descriptor));
        data[2] = hub.n_ports;

        /* fill DeviceRemovable bits */
        limit = ((hub.n_ports + 1 + 7) / 8) + 7;
        for (n = 7; n < limit; n++) {
          data[n] = 0x00;
          var_hub_size++;
        }

        /* fill PortPwrCtrlMask bits */
        limit = limit + ((hub.n_ports + 7) / 8);
        for (;n < limit; n++) {
          data[n] = 0xff;
          var_hub_size++;
        }

        ret = sizeof(bx_hub_hub_descriptor) + var_hub_size;
        data[0] = ret;
        break;
      }
    default:
      BX_ERROR(("handle_control: unknown request: 0x%04x", request));
    fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_hub_device_c::handle_data(USBPacket *p)
{
  int ret = 0;

  // check that the length is <= the max packet size of the device
  if (p->len > get_mps(p->devep)) {
    BX_DEBUG(("EP%d transfer length (%d) is greater than Max Packet Size (%d).", p->devep, p->len, get_mps(p->devep)));
  }

  switch(p->pid) {
    case USB_TOKEN_IN:
      if (p->devep == 1) {
        unsigned int status;
        int i, n;
        n = (hub.n_ports + 1 + 7) / 8;
        if (p->len == 1) { /* FreeBSD workaround */
          n = 1;
        } else if (n > p->len) {
          return USB_RET_BABBLE;
        }
        status = 0;
        for(i = 0; i < hub.n_ports; i++) {
          if (hub.usb_port[i].PortChange)
            status |= (1 << (i + 1));
        }
        if (status != 0) {
          for(i = 0; i < n; i++) {
            p->data[i] = status >> (8 * i);
          }
          ret = n;
        } else {
          ret = USB_RET_NAK; /* usb11 11.13.1 */
        }
      } else {
        goto fail;
      }
      break;
    case USB_TOKEN_OUT:
    default:
    fail:
      d.stall = 1;
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_hub_device_c::broadcast_packet(USBPacket *p)
{
  int i, ret;
  usb_device_c *dev;

  for(i = 0; i < hub.n_ports; i++) {
    dev = hub.usb_port[i].device;
    if ((dev != NULL) && (hub.usb_port[i].PortStatus & PORT_STAT_ENABLE)) {
      ret = dev->handle_packet(p);
      if (ret != USB_RET_NODEV) {
        return ret;
      }
    }
  }
  return USB_RET_NODEV;
}

int usb_hub_device_c::handle_packet(USBPacket *p)
{
  if ((d.state >= USB_STATE_DEFAULT) &&
      (d.addr != 0) &&
      (p->devaddr != d.addr) &&
        ((p->pid == USB_TOKEN_SETUP) ||
         (p->pid == USB_TOKEN_OUT) ||
         (p->pid == USB_TOKEN_IN))) {
    /* broadcast the packet to the devices */
    return broadcast_packet(p);
  }
  return usb_device_c::handle_packet(p);
}

int hub_event_handler(int event, void *ptr, void *dev, int port);

void usb_hub_device_c::init_device(Bit8u port, bx_list_c *portconf)
{
  char pname[BX_PATHNAME_LEN];

  if (DEV_usb_init_device(portconf, this, &hub.usb_port[port].device, hub_event_handler, port)) {
    if (usb_set_connect_status(port, 1)) {
      portconf->get_by_name("options")->set_enabled(0);
      sprintf(pname, "port%d.device", port+1);
      bx_list_c *sr_list = (bx_list_c *) SIM->get_param(pname, hub.state);
      hub.usb_port[port].device->register_state(sr_list);
    }
  }
}

void usb_hub_device_c::remove_device(Bit8u port)
{
  if (hub.usb_port[port].device != NULL) {
    delete hub.usb_port[port].device;
    hub.usb_port[port].device = NULL;
  }
}

int hub_event_handler(int event, void *ptr, void *dev, int port)
{
  if (dev != NULL) {
    return ((usb_hub_device_c *) dev)->event_handler(event, ptr, port);
  }
  return -1;
}

int usb_hub_device_c::event_handler(int event, void *ptr, int port)
{
  int ret = 0;

  switch (event) {
    // packet events start here
    case USB_EVENT_WAKEUP:
      if (hub.usb_port[port].PortStatus & PORT_STAT_SUSPEND) {
        hub.usb_port[port].PortChange |= PORT_STAT_C_SUSPEND;
      }
      if (d.event.dev != NULL) {
        d.event.cb(USB_EVENT_WAKEUP, NULL, d.event.dev, d.event.port);
      }
      break;

    // host controller events start here
    case USB_EVENT_CHECK_SPEED:
      if (ptr != NULL) {
        usb_device_c *usb_device = (usb_device_c *) ptr;
        if (usb_device->get_speed() <= d.speed)
          ret = 1;
      }
      break;
    default:
      BX_ERROR(("unknown/unsupported event (id=%d) on port #%d", event, port+1));
      ret = -1; // unknown event, event not handled
  }
  
  return ret;
}

bool usb_hub_device_c::usb_set_connect_status(Bit8u port, bool connected)
{
  usb_device_c *device = hub.usb_port[port].device;
  int hubnum = atoi(hub.config->get_name() + 6);
  if (device != NULL) {
    if (connected) {
      switch (device->get_speed()) {
        case USB_SPEED_LOW:
          hub.usb_port[port].PortStatus |= PORT_STAT_LOW_SPEED;
          break;
        case USB_SPEED_FULL:
          hub.usb_port[port].PortStatus &= ~PORT_STAT_LOW_SPEED;
          break;
        case USB_SPEED_HIGH:
        case USB_SPEED_SUPER:
          BX_PANIC(("Hub supports 'low' or 'full' speed devices only."));
          usb_set_connect_status(port, 0);
          return 0;
        default:
          BX_PANIC(("USB device returned invalid speed value"));
          usb_set_connect_status(port, 0);
          return 0;
      }
      hub.usb_port[port].PortStatus |= PORT_STAT_CONNECTION;
      hub.usb_port[port].PortChange |= PORT_STAT_C_CONNECTION;
      if (hub.usb_port[port].PortStatus & PORT_STAT_SUSPEND) {
        hub.usb_port[port].PortChange |= PORT_STAT_C_SUSPEND;
      }
      if (d.event.dev != NULL) {
        d.event.cb(USB_EVENT_WAKEUP, NULL, d.event.dev, d.event.port);
      }
      if (!device->get_connected()) {
        if (!device->init()) {
          usb_set_connect_status(port, 0);
          BX_ERROR(("hub #%d, port #%d: connect failed", hubnum, port+1));
          return 0;
        } else {
          BX_INFO(("hub #%d, port #%d: connect: %s", hubnum, port+1, device->get_info()));
        }
      }
    } else {
      BX_INFO(("hub #%d, port #%d: device disconnect", hubnum, port+1));
      if (d.event.dev != NULL) {
        d.event.cb(USB_EVENT_WAKEUP, NULL, d.event.dev, d.event.port);
      }
      hub.usb_port[port].PortStatus &= ~PORT_STAT_CONNECTION;
      hub.usb_port[port].PortChange |= PORT_STAT_C_CONNECTION;
      if (hub.usb_port[port].PortStatus & PORT_STAT_ENABLE) {
        hub.usb_port[port].PortStatus &= ~PORT_STAT_ENABLE;
        hub.usb_port[port].PortChange |= PORT_STAT_C_ENABLE;
      }
      remove_device(port);
    }
  }
  return connected;
}

void usb_hub_device_c::runtime_config()
{
  int i;
  char pname[6];

  for (i = 0; i < hub.n_ports; i++) {
    // device change support
    if ((hub.device_change & (1 << i)) != 0) {
      if ((hub.usb_port[i].PortStatus & PORT_STAT_CONNECTION) == 0) {
        sprintf(pname, "port%d", i + 1);
        init_device(i, (bx_list_c *) SIM->get_param(pname, hub.config));
      } else {
        usb_set_connect_status(i, 0);
      }
      hub.device_change &= ~(1 << i);
    }
    // forward to connected device
    if (hub.usb_port[i].device != NULL) {
      hub.usb_port[i].device->runtime_config();
    }
  }
}

#undef LOG_THIS
#define LOG_THIS hub->

// USB hub runtime parameter handler
Bit64s usb_hub_device_c::hub_param_handler(bx_param_c *param, bool set, Bit64s val)
{
  int portnum;
  usb_hub_device_c *hub;
  bx_list_c *port;

  if (set) {
    port = (bx_list_c *) param->get_parent();
    hub = (usb_hub_device_c *) (port->get_parent()->get_device_param());
    if (hub != NULL) {
      portnum = atoi(port->get_name()+4) - 1;
      bool empty = (val == 0);
      if ((portnum >= 0) && (portnum < hub->hub.n_ports)) {
        if (empty && (hub->hub.usb_port[portnum].PortStatus & PORT_STAT_CONNECTION)) {
          hub->hub.device_change |= (1 << portnum);
        } else if (!empty && !(hub->hub.usb_port[portnum].PortStatus & PORT_STAT_CONNECTION)) {
          hub->hub.device_change |= (1 << portnum);
        } else if (val != ((bx_param_enum_c *) param)->get()) {
          BX_ERROR(("hub_param_handler(): port #%d already in use", portnum+1));
          val = ((bx_param_enum_c *) param)->get();
        }
      } else {
        BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", param->get_name()));
      }
    // hub == NULL. We shouldn't call BX_PANIC with a NULL pointer.
    //   #define BX_PANIC(x) (LOG_THIS /* = hub-> */ panic) x
    //} else {
    //  BX_PANIC(("hub_param_handler: external hub not found"));
    }
  }
  return val;
}

// USB runtime parameter enable handler
bool usb_hub_device_c::hub_param_enable_handler(bx_param_c *param, bool en)
{
  bx_list_c *port = (bx_list_c *) param->get_parent();
  int portnum = atoi(port->get_name() + 4) - 1;
  usb_hub_device_c *hub = (usb_hub_device_c *)(port->get_parent()->get_device_param());
  if (hub != NULL) {
    if (en && (hub->hub.usb_port[portnum].device != NULL)) {
      en = 0;
    }
  }
  return en;
}

// USB runtime parameter handler: over-current
Bit64s usb_hub_device_c::hub_param_oc_handler(bx_param_c *param, bool set, Bit64s val)
{
  int portnum;
  usb_hub_device_c *hub;
  bx_list_c *port;

  if (set && val) {
    port = (bx_list_c *) param->get_parent();
    hub = (usb_hub_device_c *) (port->get_parent()->get_device_param());
    if (hub != NULL) {
      portnum = atoi(port->get_name()+4) - 1;
      hub->hub.usb_port[portnum].PortStatus &= ~PORT_STAT_POWER;
      hub->hub.usb_port[portnum].PortStatus |= PORT_STAT_OVERCURRENT;
      hub->hub.usb_port[portnum].PortChange |= PORT_STAT_C_OVERCURRENT;
      BX_DEBUG(("Over-current signaled on port #%d.", portnum + 1));
    }
  }

  return 0; // clear the indicator for next time
}

void usb_hub_restore_handler(void *dev, bx_list_c *conf)
{
  if (dev != NULL) {
    ((usb_hub_device_c *) dev)->restore_handler(conf);
  }
}

void usb_hub_device_c::restore_handler(bx_list_c *conf)
{
  int i;
  const char *pname = conf->get_name();

  i = atoi(&pname[4]) - 1;
  init_device(i, (bx_list_c *) SIM->get_param(pname, hub.config));
}

usb_device_c *usb_hub_device_c::find_device(Bit8u addr)
{
  int i;
  usb_device_c *dev, *dev2;

  if (addr == d.addr) {
    return this;
  } else {
    for (i = 0; i < hub.n_ports; i++) {
      dev = hub.usb_port[i].device;
      if ((dev != NULL) && (hub.usb_port[i].PortStatus & PORT_STAT_ENABLE)) {
        dev2 = dev->find_device(addr);
        if (dev2 != NULL) {
          return dev2;
        }
      }
    }
    return NULL;
  }
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
