/////////////////////////////////////////////////////////////////////////
// $Id: pciusb_devs.h,v 1.1 2004-12-16 19:05:01 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

// Benjamin D Lunt (fys at frontiernet net) coded most of this usb emulation.

/* This file is for initializing devices.  You should only have one device
 * initialized at a time.  I hope to have this code so that you can simply
 * change a #define to change emulated devices.
 * we will see if it works... :-)
 */


// defines
#define USB_CYPRESS 1



///////////////////////////////////////////////////////////////////////////////////////////////
// devices
///////////////////////////////////////////////////////////////////////////////////////////////

  // Clear out the device
  memset(&BX_USB_THIS hub[0].device[0], 0, sizeof(USB_DEVICE));




///////////////////////////////////////////////////////////////////////////////////////////////
// Cypress USB three button mouse.
#if USB_CYPRESS

  // USB Cyprus mouse
  BX_USB_THIS hub[0].device[0].state = STATE_DEFAULT;
  BX_USB_THIS hub[0].device[0].address = 0;
  BX_USB_THIS hub[0].device[0].alt_interface = 0;
  BX_USB_THIS hub[0].device[0].Interface = 0;
  BX_USB_THIS hub[0].device[0].config = 0;
  BX_USB_THIS hub[0].device[0].endpt = 1;
  BX_USB_THIS hub[0].device[0].function.direction = 0;
  BX_USB_THIS hub[0].device[0].function.configs = 1; // only one config in this device

  // device descriptor
  BX_USB_THIS hub[0].device[0].function.device_descr.len = 18;
  BX_USB_THIS hub[0].device[0].function.device_descr.type = DEVICE;
  BX_USB_THIS hub[0].device[0].function.device_descr.usb_ver = 0x0100;
  BX_USB_THIS hub[0].device[0].function.device_descr._class = 0;
  BX_USB_THIS hub[0].device[0].function.device_descr.subclass = 0;
  BX_USB_THIS hub[0].device[0].function.device_descr.protocol = 0;
  BX_USB_THIS hub[0].device[0].function.device_descr.max_packet_size = 8;
  BX_USB_THIS hub[0].device[0].function.device_descr.vendorid = 0x0627;
  BX_USB_THIS hub[0].device[0].function.device_descr.productid = 0x0001;
  BX_USB_THIS hub[0].device[0].function.device_descr.device_rel = 0;
  BX_USB_THIS hub[0].device[0].function.device_descr.manuf_indx = 1;
  BX_USB_THIS hub[0].device[0].function.device_descr.prod_indx = 2;
  BX_USB_THIS hub[0].device[0].function.device_descr.serial_indx = 0;
  BX_USB_THIS hub[0].device[0].function.device_descr.configs = 1;

  // config descriptor
  BX_USB_THIS hub[0].device[0].function.device_config[0].len = 9;
  BX_USB_THIS hub[0].device[0].function.device_config[0].type = CONFIG;
  BX_USB_THIS hub[0].device[0].function.device_config[0].tot_len = 34;   // size of config+interface+endpt+hid
  BX_USB_THIS hub[0].device[0].function.device_config[0].interfaces = 1;
  BX_USB_THIS hub[0].device[0].function.device_config[0].config_val = 1;
  BX_USB_THIS hub[0].device[0].function.device_config[0].config_indx = 4;
  BX_USB_THIS hub[0].device[0].function.device_config[0].attrbs = 0xA0;  // bus powered, etc
  BX_USB_THIS hub[0].device[0].function.device_config[0].max_power = 50;

  // number of interfaces
  BX_USB_THIS hub[0].device[0].function.device_config[0].interface_cnt = 1;

  // interface descriptor
  //    0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x01, 0x02, 0x05,   // interface descriptor
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].size = 9;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].type = INTERFACE;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].interface_num = 0;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].alternate = 0;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].num_endpts = 1;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].iclass = 3;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].subclass = 1;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].protocol = 2;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].str_indx = 5;

  // endpoint descriptor
  //    0x07, 0x05, 0x81, 0x03, 0x03, 0x00, 0x0A,               // endpoint descriptor
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].endpts[0].size = 7;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].endpts[0].type = ENDPOINT;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].endpts[0].endpt = 0x81;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].endpts[0].attrib = 3;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].endpts[0].max_size = 3;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].endpts[0].interval = 0x0A;

  // HID descriptor
  //    0x09, 0x21, 0x00, 0x01, 0x00, 0x01, 0x22, 0x32, 0x00    // hid descriptor
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.size = 9;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.type = 0x21;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.HID_class = 0x0100;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.country_code = 0;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.num_descriptors = 1;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.descriptor[0].type = 0x22;
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.descriptor[0].len = 50;

  // HID descriptor type 0x22 (report descriptor)
  Bit8u rep0[] = {
    0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01, 0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x03,
    0x15, 0x00, 0x25, 0x01, 0x95, 0x03, 0x75, 0x01, 0x81, 0x02, 0x95, 0x01, 0x75, 0x05, 0x81, 0x01,
    0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x02, 0x81, 0x06,
    0xC0, 0xC0
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.descriptor[0].dev_hid_descript_report, rep0, 50);
  
  // string descriptors
  BX_USB_THIS hub[0].device[0].function.str_descriptor.size = 0x04;
  BX_USB_THIS hub[0].device[0].function.str_descriptor.type = 0x03;
  BX_USB_THIS hub[0].device[0].function.str_descriptor.langid[0] = 0x0409;

  // string #1
  BX_USB_THIS hub[0].device[0].function.string[0].size = 24;
  BX_USB_THIS hub[0].device[0].function.string[0].type = 3;
  Bit8u str_1[64] = { 0x43, 0x00, 0x79, 0x00, 0x70, 0x00, 0x72, 0x00, 0x65, 
    0x00, 0x73, 0x00, 0x73, 0x00, 0x20, 0x00, 0x53, 0x00, 0x65, 0x00, 0x6D, 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.string[0].unicode_str, str_1, 22);

  // string #2
  BX_USB_THIS hub[0].device[0].function.string[1].size = 36;
  BX_USB_THIS hub[0].device[0].function.string[1].type = 3;
  Bit8u str_2[64] = { 0x43, 0x00, 0x79, 0x00, 0x70, 0x00, 0x72, 0x00, 0x65,
    0x00, 0x73, 0x00, 0x73, 0x00, 0x20, 0x00, 0x55, 0x00, 0x53, 0x00, 0x42,
    0x00, 0x20, 0x00, 0x4D, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x73, 0x00, 0x65, 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.string[1].unicode_str, str_2, 34);

  // string #3
  BX_USB_THIS hub[0].device[0].function.string[2].size = 0;
  BX_USB_THIS hub[0].device[0].function.string[2].type = 3;
  memset(BX_USB_THIS hub[0].device[0].function.string[2].unicode_str, 0, 64);

  // string #4
  BX_USB_THIS hub[0].device[0].function.string[3].size = 20;
  BX_USB_THIS hub[0].device[0].function.string[3].type = 3;
  Bit8u str_4[64] = { 0x48, 0x00, 0x49, 0x00, 0x44, 0x00, 0x20, 0x00, 0x4D,
    0x00, 0x6F, 0x00, 0x75, 0x00, 0x73, 0x00, 0x65, 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.string[3].unicode_str, str_4, 18);

  // string #5
  BX_USB_THIS hub[0].device[0].function.string[4].size = 50;
  BX_USB_THIS hub[0].device[0].function.string[4].type = 3;
  Bit8u str_5[64] = { 'E', 0x00, 'n', 0x00, 'd', 0x00, 'p', 0x00, 'o', 0x00,
    'i', 0x00, 'n', 0x00, 't', 0x00, '1', 0x00, ' ', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00,
    'e', 0x00, 'r', 0x00, 'r', 0x00, 'u', 0x00, 'p', 0x00, 't', 0x00, ' ', 0x00, 'P', 0x00,
    'i', 0x00, 'p', 0x00, 'e', 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.string[4].unicode_str, str_5, 48);


#endif // USB_CYPRESS


// Another device would go here.



