/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// USB hub emulation support (ported from QEMU)
//
// Copyright (C) 2005       Fabrice Bellard
// Copyright (C) 2009-2020  The Bochs Project
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

// USB device plugin entry points

int CDECL libusb_hub_dev_plugin_init(plugin_t *plugin, plugintype_t type)
{
  return 0; // Success
}

void CDECL libusb_hub_dev_plugin_fini(void)
{
  // Nothing here yet
}

//
// Define the static class that registers the derived USB device class,
// and allocates one on request.
//
class bx_usb_hub_locator_c : public usbdev_locator_c {
public:
  bx_usb_hub_locator_c(void) : usbdev_locator_c("usb_hub") {}
protected:
  usb_device_c *allocate(usbdev_type devtype, const char *args) {
    int ports;
    if (args != NULL) {
      ports = atoi(args);
    } else {
      ports = 4;
    }
    return (new usb_hub_device_c(ports));
  }
} bx_usb_hub_match;

#define ClearHubFeature         (0x2000 | USB_REQ_CLEAR_FEATURE)
#define ClearPortFeature        (0x2300 | USB_REQ_CLEAR_FEATURE)
#define GetHubDescriptor        (0xa000 | USB_REQ_GET_DESCRIPTOR)
#define GetHubStatus            (0xa000 | USB_REQ_GET_STATUS)
#define GetPortStatus           (0xa300 | USB_REQ_GET_STATUS)
#define SetHubFeature           (0x2000 | USB_REQ_SET_FEATURE)
#define SetPortFeature          (0x2300 | USB_REQ_SET_FEATURE)

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

static const Bit8u bx_hub_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x10, 0x01, /*  u16 bcdUSB; v1.1 */

  0x09,       /*  u8  bDeviceClass; HUB_CLASSCODE */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x40,       /*  u8  bMaxPacketSize0; 64 Bytes */

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
  0x02, 0x00, /*  u16 ep_wMaxPacketSize; 1 + (MAX_ROOT_PORTS / 8) */
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

usb_hub_device_c::usb_hub_device_c(Bit8u ports)
{
  int i;
  char pname[10];
  char label[32];
  bx_list_c *port;
  bx_param_string_c *device;

  d.type = USB_DEV_TYPE_HUB;
  d.speed = d.minspeed = d.maxspeed = USB_SPEED_FULL;
  strcpy(d.devname, "Bochs USB HUB");
  d.dev_descriptor = bx_hub_dev_descriptor;
  d.config_descriptor = bx_hub_config_descriptor;
  d.device_desc_size = sizeof(bx_hub_dev_descriptor);
  d.config_desc_size = sizeof(bx_hub_config_descriptor);
  d.vendor_desc = "BOCHS";
  d.product_desc = "BOCHS USB HUB";
  if ((ports < 2) || (ports > USB_HUB_PORTS)) {
    BX_ERROR(("ignoring invalid number of ports (%d)", ports));
    ports = 4;
  }
  d.connected = 1;
  memset((void*)&hub, 0, sizeof(hub));
  hub.n_ports = ports;
  bx_hub_config_descriptor[22] = (hub.n_ports + 1 + 7) / 8;
  sprintf(hub.serial_number, "%d", serial_number++);
  d.serial_num = hub.serial_number;
  for(i = 0; i < hub.n_ports; i++) {
    hub.usb_port[i].PortStatus = PORT_STAT_POWER;
    hub.usb_port[i].PortChange = 0;
  }
  hub.device_change = 0;

  // config options
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  sprintf(pname, "exthub%u", ++hub_count);
  sprintf(label, "External Hub #%u Configuration", hub_count);
  hub.config = new bx_list_c(usb_rt, pname, label);
  hub.config->set_options(bx_list_c::SHOW_PARENT);
  hub.config->set_device_param(this);
  for(i = 0; i < hub.n_ports; i++) {
    sprintf(pname, "port%d", i+1);
    sprintf(label, "Port #%d Configuration", i+1);
    port = new bx_list_c(hub.config, pname, label);
    port->set_options(port->SERIES_ASK | port->USE_BOX_TITLE);
    device = new bx_param_string_c(port, "device", "Device", "", "", BX_PATHNAME_LEN);
    device->set_handler(hub_param_handler);
    new bx_param_string_c(port, "options", "Options", "", "", BX_PATHNAME_LEN);
  }
  if (SIM->is_wx_selected()) {
    bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
    usb->add(hub.config);
  }

  put("usb_hub", "USBHUB");
}

usb_hub_device_c::~usb_hub_device_c(void)
{
  for (int i=0; i<hub.n_ports; i++) {
    remove_device(i);
  }
  d.sr->clear();
  if (SIM->is_wx_selected()) {
    bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
    usb->remove(hub.config->get_name());
  }
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove(hub.config->get_name());
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
          BX_ERROR(("unknown string descriptor type %i", value & 0xff));
          goto fail;
          break;
        default:
          BX_ERROR(("unknown descriptor type: 0x%02x", (value >> 8)));
          goto fail;
      }
      break;
      /* hub specific requests */
    case GetHubStatus:
      if (d.state == USB_STATE_CONFIGURED) {
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        ret = 4;
      } else
        goto fail;
      break;
    case GetPortStatus:
      n = index - 1;
      if (n >= hub.n_ports)
        goto fail;
      data[0] = (hub.usb_port[n].PortStatus & 0xff);
      data[1] = (hub.usb_port[n].PortStatus >> 8);
      data[2] = (hub.usb_port[n].PortChange & 0xff);
      data[3] = (hub.usb_port[n].PortChange >> 8);
      ret = 4;
      break;
    case SetHubFeature:
    case ClearHubFeature:
      if (value == 0 || value == 1) {
      } else {
        goto fail;
      }
      ret = 0;
      break;
    case SetPortFeature:
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
          BX_ERROR(("Unknown SetPortFeature: %i", value));
          goto fail;
      }
      ret = 0;
      break;
    case ClearPortFeature:
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
            BX_ERROR(("Unknown ClearPortFeature: %i", value));
            goto fail;
      }
      ret = 0;
      break;
    case GetHubDescriptor:
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

void usb_hub_device_c::init_device(Bit8u port, bx_list_c *portconf)
{
  usbdev_type type;
  char pname[BX_PATHNAME_LEN];
  const char *devname = NULL;

  devname = ((bx_param_string_c*)portconf->get_by_name("device"))->getptr();
  if (devname == NULL) return;
  if (!strlen(devname) || !strcmp(devname, "none")) return;

  if (hub.usb_port[port].device != NULL) {
    BX_ERROR(("init_device(): port%d already in use", port+1));
    return;
  }
  sprintf(pname, "port%d.device", port+1);
  bx_list_c *sr_list = (bx_list_c*)SIM->get_param(pname, hub.state);
  type = DEV_usb_init_device(portconf, this, &hub.usb_port[port].device, sr_list);
  if (hub.usb_port[port].device != NULL) {
    usb_set_connect_status(port, type, 1);
  }
}

void usb_hub_device_c::remove_device(Bit8u port)
{
  if (hub.usb_port[port].device != NULL) {
    delete hub.usb_port[port].device;
    hub.usb_port[port].device = NULL;
  }
}

void hub_event_handler(int event, USBPacket *packet, void *dev, int port)
{
  ((usb_hub_device_c*)dev)->event_handler(event, packet, port);
}

void usb_hub_device_c::event_handler(int event, USBPacket *packet, int port)
{
  if (event == USB_EVENT_WAKEUP) {
    if (hub.usb_port[port].PortStatus & PORT_STAT_SUSPEND) {
      hub.usb_port[port].PortChange |= PORT_STAT_C_SUSPEND;
    }
    if (d.event.dev != NULL) {
      d.event.cb(USB_EVENT_WAKEUP, NULL, d.event.dev, d.event.port);
    }
  } else {
    BX_ERROR(("unknown/unsupported event (id=%d) on port #%d", event, port+1));
  }
}

void usb_hub_device_c::usb_set_connect_status(Bit8u port, int type, bx_bool connected)
{
  usb_device_c *device = hub.usb_port[port].device;
  if (device != NULL) {
    if (device->get_type() == type) {
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
            usb_set_connect_status(port, type, 0);
            return;
          default:
            BX_PANIC(("USB device returned invalid speed value"));
            usb_set_connect_status(port, type, 0);
            return;
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
            usb_set_connect_status(port, type, 0);
            BX_ERROR(("port #%d: connect failed", port+1));
            return;
          } else {
            BX_INFO(("port #%d: connect: %s", port+1, device->get_info()));
          }
        }
        device->set_event_handler(this, hub_event_handler, port);
      } else {
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
  }
}

void usb_hub_device_c::runtime_config()
{
  int i, hubnum;
  char pname[6];

  for (i = 0; i < hub.n_ports; i++) {
    // device change support
    if ((hub.device_change & (1 << i)) != 0) {
      hubnum = atoi(hub.config->get_name()+6);
      BX_INFO(("USB hub #%d, port #%d: device connect", hubnum, i+1));
      sprintf(pname, "port%d", i + 1);
      init_device(i, (bx_list_c*)SIM->get_param(pname, hub.config));
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
const char *usb_hub_device_c::hub_param_handler(bx_param_string_c *param, int set,
                                                const char *oldval, const char *val, int maxlen)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  int hubnum, portnum;
  usb_hub_device_c *hub;
  bx_list_c *port;

  if (set) {
    port = (bx_list_c*)param->get_parent();
    hub = (usb_hub_device_c*)(port->get_parent()->get_device_param());
    if (hub != NULL) {
      hubnum = atoi(port->get_parent()->get_name()+6);
      portnum = atoi(port->get_name()+4) - 1;
      bx_bool empty = ((strlen(val) == 0) || (!strcmp(val, "none")));
      if ((portnum >= 0) && (portnum < hub->hub.n_ports)) {
        if (empty && (hub->hub.usb_port[portnum].PortStatus & PORT_STAT_CONNECTION)) {
          BX_INFO(("USB hub #%d, port #%d: device disconnect", hubnum, portnum+1));
          if (hub->hub.usb_port[portnum].device != NULL) {
            type = hub->hub.usb_port[portnum].device->get_type();
          }
          hub->usb_set_connect_status(portnum, type, 0);
        } else if (!empty && !(hub->hub.usb_port[portnum].PortStatus & PORT_STAT_CONNECTION)) {
          hub->hub.device_change |= (1 << portnum);
        }
      } else {
        BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", param->get_name()));
      }
    } else {
      BX_PANIC(("hub_param_handler: external hub not found"));
    }
  }
  return val;
}

void usb_hub_restore_handler(void *dev, bx_list_c *conf)
{
  ((usb_hub_device_c*)dev)->restore_handler(conf);
}

void usb_hub_device_c::restore_handler(bx_list_c *conf)
{
  int i;
  const char *pname = conf->get_name();

  i = atoi(&pname[4]) - 1;
  init_device(i, (bx_list_c*)SIM->get_param(pname, hub.config));
}

usb_device_c* usb_hub_device_c::find_device(Bit8u addr)
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
