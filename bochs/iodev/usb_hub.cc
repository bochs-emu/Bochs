/////////////////////////////////////////////////////////////////////////
// $Id: usb_hub.cc,v 1.1 2009-03-07 16:57:17 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009  Volker Ruppert
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
//
/////////////////////////////////////////////////////////////////////////

// USB hub emulation support ported from the Qemu project

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#define NO_DEVICE_INCLUDES
#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
#include "usb_common.h"
#include "usb_hid.h"
#include "usb_hub.h"
#include "usb_msd.h"

#define LOG_THIS

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

static const Bit8u bx_hub_dev_descriptor[] = {
  0x12,       /*  u8 bLength; */
  0x01,       /*  u8 bDescriptorType; Device */
  0x10, 0x01, /*  u16 bcdUSB; v1.1 */

  0x09,       /*  u8  bDeviceClass; HUB_CLASSCODE */
  0x00,       /*  u8  bDeviceSubClass; */
  0x00,       /*  u8  bDeviceProtocol; [ low/full speeds only ] */
  0x08,       /*  u8  bMaxPacketSize0; 8 Bytes */

  0x00, 0x00, /*  u16 idVendor; */
  0x00, 0x00, /*  u16 idProduct; */
  0x01, 0x01, /*  u16 bcdDevice */

  0x03,       /*  u8  iManufacturer; */
  0x02,       /*  u8  iProduct; */
  0x01,       /*  u8  iSerialNumber; */
  0x01        /*  u8  bNumConfigurations; */
};

/* XXX: patch interrupt size */
static const Bit8u bx_hub_config_descriptor[] = {

  /* one configuration */
  0x09,       /*  u8  bLength; */
  0x02,       /*  u8  bDescriptorType; Configuration */
  0x19, 0x00, /*  u16 wTotalLength; */
  0x01,       /*  u8  bNumInterfaces; (1) */
  0x01,       /*  u8  bConfigurationValue; */
  0x00,       /*  u8  iConfiguration; */
  0xc0,       /*  u8  bmAttributes;
                        Bit 7: must be set,
                            6: Self-powered,
                            5: Remote wakeup,
                         4..0: resvd */
  0x00,       /*  u8  MaxPower; */

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

// External hub registration for config handler access

typedef struct _ext_usb_hub_t {
  usb_hub_device_c *dev;
  struct _ext_usb_hub_t *next;
} ext_usb_hub_t;

static int hub_count = 0;
static ext_usb_hub_t *ext_usb_hubs = NULL;

static bx_bool add_usb_hub(usb_hub_device_c *hub)
{
  ext_usb_hub_t *ext_usb_hub;

  ext_usb_hub = (ext_usb_hub_t *)malloc(sizeof(ext_usb_hub_t));
  if (ext_usb_hub == NULL) {
    return 0;
  }

  ext_usb_hub->dev = hub;
  ext_usb_hub->next = NULL;

  if (ext_usb_hubs == NULL) {
    ext_usb_hubs = ext_usb_hub;
  } else {
    ext_usb_hub_t *temp = ext_usb_hubs;

    while (temp->next) {
      temp = temp->next;
    }
    temp->next = ext_usb_hub;
  }
  return 1;
}

static bx_bool remove_usb_hub(usb_hub_device_c *hub)
{
  ext_usb_hub_t *ext_usb_hub, *prev = NULL;

  for (ext_usb_hub = ext_usb_hubs; ext_usb_hub; ext_usb_hub = ext_usb_hub->next) {
    if (ext_usb_hub->dev == hub) {
      if (prev == NULL) {
        ext_usb_hubs = ext_usb_hub->next;
      } else {
        prev->next = ext_usb_hub->next;
      }
      free(ext_usb_hub);
      return 1;
    } else {
      prev = ext_usb_hub;
    }
  }
  return 0;
}

static usb_hub_device_c *find_usb_hub(const char *name)
{
  ext_usb_hub_t *ext_usb_hub;

  for (ext_usb_hub = ext_usb_hubs; ext_usb_hub; ext_usb_hub = ext_usb_hub->next) {
    if (!strcmp(ext_usb_hub->dev->get_name(), name)) {
      return ext_usb_hub->dev;
    }
  }
  return NULL;
}


usb_hub_device_c::usb_hub_device_c()
{
  int i;
  char pname[10];
  char label[32];
  bx_param_string_c *port;

  d.type = USB_DEV_TYPE_HUB;
  d.speed = USB_SPEED_FULL;
  strcpy(d.devname, "Bochs USB HUB");
  d.connected = 1;
  memset((void*)&hub, 0, sizeof(hub));
  for(i = 0; i < USB_HUB_NUM_PORTS; i++) {
    hub.usb_port[i].PortStatus = PORT_STAT_POWER;
    hub.usb_port[i].PortChange = 0;
  }
  add_usb_hub(this);

  // config options
  bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  sprintf(pname, "exthub%d", ++hub_count);
  sprintf(label, "External Hub #%d Configuration", hub_count);
  hub.config = new bx_list_c(usb, pname, label, USB_HUB_NUM_PORTS);
  hub.config->get_options()->set(bx_list_c::SHOW_PARENT | bx_list_c::USE_BOX_TITLE);
  usb_rt->add(hub.config);
  for(i = 0; i < USB_HUB_NUM_PORTS; i++) {
    sprintf(pname, "port%d", i+1);
    sprintf(label, "Port #%d device", i+1);
    port = new bx_param_string_c(hub.config, pname, label, "", "", BX_PATHNAME_LEN);
    port->set_handler(hub_param_handler);
    port->set_runtime_param(1);
  }

  put("USBHB");
}

usb_hub_device_c::~usb_hub_device_c(void)
{
  for (int i=0; i<USB_HUB_NUM_PORTS; i++) {
    remove_device(i);
  }
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove(hub.config->get_name());
  // FIXME: the wx ParamDialog doesn't like this
#if !BX_WITH_WX
  bx_list_c *usb = (bx_list_c*)SIM->get_param("ports.usb");
  usb->remove(hub.config->get_name());
#endif
  remove_usb_hub(this);
}

void usb_hub_device_c::register_state_specific(bx_list_c *parent)
{
  Bit8u i;
  char portnum[6];
  bx_list_c *port;

  hub.state = new bx_list_c(parent, "hub", "USB HUB Device State", USB_HUB_NUM_PORTS);
  for (i=0; i<USB_HUB_NUM_PORTS; i++) {
    sprintf(portnum, "port%d", i+1);
    port = new bx_list_c(hub.state, portnum, 3);
    new bx_shadow_num_c(port, "PortStatus", &hub.usb_port[i].PortStatus, BASE_HEX);
    new bx_shadow_num_c(port, "PortChange", &hub.usb_port[i].PortChange, BASE_HEX);
    // empty list for USB device state
    new bx_list_c(port, "device", 20);
  }
}

void usb_hub_device_c::after_restore_state()
{
  for (int i=0; i<USB_HUB_NUM_PORTS; i++) {
    if (hub.usb_port[i].device != NULL) {
      hub.usb_port[i].device->after_restore_state();
    }
  }
}

void usb_hub_device_c::handle_reset()
{
  // TODO
  BX_DEBUG(("Reset"));
}

int usb_hub_device_c::handle_control(int request, int value, int index, int length, Bit8u *data)
{
  int ret = 0;
  unsigned int n;

  switch(request) {
    case DeviceRequest | USB_REQ_GET_STATUS:
      data[0] = (1 << USB_DEVICE_SELF_POWERED) |
                (d.remote_wakeup << USB_DEVICE_REMOTE_WAKEUP);
      data[1] = 0x00;
      ret = 2;
      break;
    case DeviceOutRequest | USB_REQ_CLEAR_FEATURE:
      if (value == USB_DEVICE_REMOTE_WAKEUP) {
        d.remote_wakeup = 0;
      } else {
        goto fail;
      }
      ret = 0;
      break;
    case EndpointOutRequest | USB_REQ_CLEAR_FEATURE:
      if (value == 0 && index != 0x81) { /* clear ep halt */
        goto fail;
      }
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_FEATURE:
      if (value == USB_DEVICE_REMOTE_WAKEUP) {
        d.remote_wakeup = 1;
      } else {
        goto fail;
      }
      ret = 0;
      break;
    case DeviceOutRequest | USB_REQ_SET_ADDRESS:
      d.addr = value;
      ret = 0;
      break;
    case DeviceRequest | USB_REQ_GET_DESCRIPTOR:
      switch(value >> 8) {
        case USB_DT_DEVICE:
          memcpy(data, bx_hub_dev_descriptor,
                 sizeof(bx_hub_dev_descriptor));
          ret = sizeof(bx_hub_dev_descriptor);
          break;
        case USB_DT_CONFIG:
          memcpy(data, bx_hub_config_descriptor,
                 sizeof(bx_hub_config_descriptor));

          /* status change endpoint size based on number
           * of ports */
          data[22] = (USB_HUB_NUM_PORTS + 1 + 7) / 8;

          ret = sizeof(bx_hub_config_descriptor);
          break;
        case USB_DT_STRING:
          switch(value & 0xff) {
            case 0:
              /* language ids */
              data[0] = 4;
              data[1] = 3;
              data[2] = 0x09;
              data[3] = 0x04;
              ret = 4;
              break;
            case 1:
              /* serial number */
              ret = set_usb_string(data, "1234");
              break;
            case 2:
              /* product description */
              ret = set_usb_string(data, "Bochs USB HUB");
              break;
            case 3:
              /* vendor description */
              ret = set_usb_string(data, "Bochs");
              break;
            default:
              goto fail;
          }
          break;
        default:
            goto fail;
      }
      break;
    case DeviceRequest | USB_REQ_GET_CONFIGURATION:
      data[0] = 1;
      ret = 1;
      break;
    case DeviceOutRequest | USB_REQ_SET_CONFIGURATION:
      ret = 0;
      break;
    case DeviceRequest | USB_REQ_GET_INTERFACE:
      data[0] = 0;
      ret = 1;
      break;
    case DeviceOutRequest | USB_REQ_SET_INTERFACE:
      ret = 0;
      break;
      /* usb specific requests */
    case GetHubStatus:
      data[0] = 0;
      data[1] = 0;
      data[2] = 0;
      data[3] = 0;
      ret = 4;
      break;
    case GetPortStatus:
      n = index - 1;
      if (n >= USB_HUB_NUM_PORTS)
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
      if (n >= USB_HUB_NUM_PORTS)
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
          goto fail;
      }
      ret = 0;
      break;
    case ClearPortFeature:
      n = index - 1;
      if (n >= USB_HUB_NUM_PORTS)
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
            goto fail;
      }
      ret = 0;
      break;
    case GetHubDescriptor:
        {
          unsigned int limit, var_hub_size = 0;
          memcpy(data, bx_hub_hub_descriptor,
                 sizeof(bx_hub_hub_descriptor));
          data[2] = USB_HUB_NUM_PORTS;

          /* fill DeviceRemovable bits */
          limit = ((USB_HUB_NUM_PORTS + 1 + 7) / 8) + 7;
          for (n = 7; n < limit; n++) {
            data[n] = 0x00;
            var_hub_size++;
          }

          /* fill PortPwrCtrlMask bits */
          limit = limit + ((USB_HUB_NUM_PORTS + 7) / 8);
          for (;n < limit; n++) {
            data[n] = 0xff;
            var_hub_size++;
          }

          ret = sizeof(bx_hub_hub_descriptor) + var_hub_size;
          data[0] = ret;
          break;
        }
    default:
    fail:
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
        n = (USB_HUB_NUM_PORTS + 1 + 7) / 8;
        if (p->len == 1) { /* FreeBSD workaround */
          n = 1;
        } else if (n > p->len) {
          return USB_RET_BABBLE;
        }
        status = 0;
        for(i = 0; i < USB_HUB_NUM_PORTS; i++) {
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
      ret = USB_RET_STALL;
      break;
  }
  return ret;
}

int usb_hub_device_c::broadcast_packet(USBPacket *p)
{
  int i, ret;
  usb_device_c *dev;

  for(i = 0; i < USB_HUB_NUM_PORTS; i++) {
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
  if ((d.state == USB_STATE_DEFAULT) &&
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

void usb_hub_device_c::init_device(Bit8u port, const char *devname)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  char pname[BX_PATHNAME_LEN];

  if (!strlen(devname) || !strcmp(devname, "none")) return;

  if (hub.usb_port[port].device != NULL) {
    BX_ERROR(("init_device(): port%d already in use", port+1));
    return;
  }

  if (!strcmp(devname, "mouse")) {
    type = USB_DEV_TYPE_MOUSE;
    hub.usb_port[port].device = new usb_hid_device_c(type);
  } else if (!strcmp(devname, "tablet")) {
    type = USB_DEV_TYPE_TABLET;
    hub.usb_port[port].device = new usb_hid_device_c(type);
  } else if (!strcmp(devname, "keypad")) {
    type = USB_DEV_TYPE_KEYPAD;
    hub.usb_port[port].device = new usb_hid_device_c(type);
  } else if (!strncmp(devname, "disk", 4)) {
    if ((strlen(devname) > 5) && (devname[4] == ':')) {
      type = USB_DEV_TYPE_DISK;
      hub.usb_port[port].device = new usb_msd_device_c(devname+5);
    } else {
      BX_PANIC(("USB device 'disk' needs a filename separated with a colon"));
      return;
    }
  } else if (!strcmp(devname, "hub")) {
    type = USB_DEV_TYPE_HUB;
    hub.usb_port[port].device = new usb_hub_device_c();
  } else {
    BX_PANIC(("unknown USB device: %s", devname));
    return;
  }
  sprintf(pname, "port%d.device", port+1);
  bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, hub.state);
  hub.usb_port[port].device->register_state(devlist);
  usb_set_connect_status(port, type, 1);
}

void usb_hub_device_c::remove_device(Bit8u port)
{
  char pname[BX_PATHNAME_LEN];

  if (hub.usb_port[port].device != NULL) {
    delete hub.usb_port[port].device;
    hub.usb_port[port].device = NULL;
    sprintf(pname, "port%d.device", port+1);
    bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, hub.state);
    devlist->clear();
  }
}

void usb_hub_device_c::usb_set_connect_status(Bit8u port, int type, bx_bool connected)
{
  usb_device_c *device = hub.usb_port[port].device;
  if (device != NULL) {
    if (device->get_type() == type) {
      if (connected) {
        hub.usb_port[port].PortStatus |= PORT_STAT_CONNECTION;
        hub.usb_port[port].PortChange |= PORT_STAT_C_CONNECTION;
        if (device->get_speed() == USB_SPEED_LOW)
          hub.usb_port[port].PortStatus |= PORT_STAT_LOW_SPEED;
        else
          hub.usb_port[port].PortStatus &= ~PORT_STAT_LOW_SPEED;
        if ((type == USB_DEV_TYPE_DISK) && (!device->get_connected())) {
          if (!((usb_msd_device_c*)hub.usb_port[port].device)->init()) {
            usb_set_connect_status(port, USB_DEV_TYPE_DISK, 0);
          } else {
            BX_INFO(("HD on USB port #%d: '%s'", port+1,
                     ((usb_msd_device_c*)device)->get_path()));
          }
        }
      } else {
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

#undef LOG_THIS
#define LOG_THIS hub->

// USB hub runtime parameter handler
const char *usb_hub_device_c::hub_param_handler(bx_param_string_c *param, int set,
                                                const char *oldval, const char *val, int maxlen)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  int hubnum, portnum;
  usb_hub_device_c *hub;

  if (set) {
    hub = find_usb_hub(param->get_parent()->get_name());
    if (hub != NULL) {
      hubnum = atoi(param->get_parent()->get_name()+6);
      portnum = atoi(param->get_name()+4) - 1;
      bx_bool empty = ((strlen(val) == 0) || (!strcmp(val, "none")));
      if ((portnum >= 0) && (portnum < USB_HUB_NUM_PORTS)) {
        BX_INFO(("USB hub #%d, port #%d experimental device change", hubnum, portnum+1));
        if (empty && (hub->hub.usb_port[portnum].PortStatus & PORT_STAT_CONNECTION)) {
          if (hub->hub.usb_port[portnum].device != NULL) {
            type = hub->hub.usb_port[portnum].device->get_type();
          }
          hub->usb_set_connect_status(portnum, type, 0);
        } else if (!empty && !(hub->hub.usb_port[portnum].PortStatus & PORT_STAT_CONNECTION)) {
          hub->init_device(portnum, val);
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

#endif // BX_SUPPORT_PCI && BX_SUPPORT_PCIUSB
