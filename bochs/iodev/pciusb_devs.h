/////////////////////////////////////////////////////////////////////////
// $Id: pciusb_devs.h,v 1.2 2004-12-19 09:59:40 vruppert Exp $
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
#define USB_KEYPAD  1


///////////////////////////////////////////////////////////////////////////////////////////////
// Cypress USB three button mouse.
// This mouse is on port 0 of hub 0 and is the first device.
#if USB_CYPRESS

  BX_USB_THIS hub[0].device[0].connect_status = 0;
  BX_USB_THIS hub[0].usb_port[0].device_num = 0;
  BX_USB_THIS hub[0].device[0].dev_type = USB_DEV_TYPE_MOUSE;

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
  Bit8u dev0_rep0[] = {
    0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01, 0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x03,
    0x15, 0x00, 0x25, 0x01, 0x95, 0x03, 0x75, 0x01, 0x81, 0x02, 0x95, 0x01, 0x75, 0x05, 0x81, 0x01,
    0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x02, 0x81, 0x06,
    0xC0, 0xC0
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].dev_hid_descript.descriptor[0].dev_hid_descript_report, dev0_rep0, 50);

  // string descriptors
  BX_USB_THIS hub[0].device[0].function.str_descriptor.size = 0x04;
  BX_USB_THIS hub[0].device[0].function.str_descriptor.type = 0x03;
  BX_USB_THIS hub[0].device[0].function.str_descriptor.langid[0] = 0x0409;

  // string #1
  BX_USB_THIS hub[0].device[0].function.string[0].size = 24;
  BX_USB_THIS hub[0].device[0].function.string[0].type = 3;
  Bit8u dev0_str_1[64] = { 0x43, 0x00, 0x79, 0x00, 0x70, 0x00, 0x72, 0x00, 0x65, 
    0x00, 0x73, 0x00, 0x73, 0x00, 0x20, 0x00, 0x53, 0x00, 0x65, 0x00, 0x6D, 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.string[0].unicode_str, dev0_str_1, 22);

  // string #2
  BX_USB_THIS hub[0].device[0].function.string[1].size = 36;
  BX_USB_THIS hub[0].device[0].function.string[1].type = 3;
  Bit8u dev0_str_2[64] = { 0x43, 0x00, 0x79, 0x00, 0x70, 0x00, 0x72, 0x00, 0x65,
    0x00, 0x73, 0x00, 0x73, 0x00, 0x20, 0x00, 0x55, 0x00, 0x53, 0x00, 0x42,
    0x00, 0x20, 0x00, 0x4D, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x73, 0x00, 0x65, 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.string[1].unicode_str, dev0_str_2, 34);

  // string #3
  BX_USB_THIS hub[0].device[0].function.string[2].size = 0;
  BX_USB_THIS hub[0].device[0].function.string[2].type = 3;
  memset(BX_USB_THIS hub[0].device[0].function.string[2].unicode_str, 0, 64);

  // string #4
  BX_USB_THIS hub[0].device[0].function.string[3].size = 20;
  BX_USB_THIS hub[0].device[0].function.string[3].type = 3;
  Bit8u dev0_str_4[64] = { 0x48, 0x00, 0x49, 0x00, 0x44, 0x00, 0x20, 0x00, 0x4D,
    0x00, 0x6F, 0x00, 0x75, 0x00, 0x73, 0x00, 0x65, 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.string[3].unicode_str, dev0_str_4, 18);

  // string #5
  BX_USB_THIS hub[0].device[0].function.string[4].size = 50;
  BX_USB_THIS hub[0].device[0].function.string[4].type = 3;
  Bit8u dev0_str_5[64] = { 'E', 0x00, 'n', 0x00, 'd', 0x00, 'p', 0x00, 'o', 0x00,
    'i', 0x00, 'n', 0x00, 't', 0x00, '1', 0x00, ' ', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00,
    'e', 0x00, 'r', 0x00, 'r', 0x00, 'u', 0x00, 'p', 0x00, 't', 0x00, ' ', 0x00, 'P', 0x00,
    'i', 0x00, 'p', 0x00, 'e', 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[0].function.string[4].unicode_str, dev0_str_5, 48);

  // this device has no key conversion table
  BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].lookup_cnt = 0;
  memset(&BX_USB_THIS hub[0].device[0].function.device_config[0].Interface[0].lookup, 0, sizeof(struct KEYPAD) * KEYPAD_LEN);

#endif // USB_CYPRESS


///////////////////////////////////////////////////////////////////////////////////////////////
// USB 17 button keypad.
// This keypad is on port 1 of hub 0 and is the second device.
#if USB_KEYPAD

  BX_USB_THIS hub[0].device[1].connect_status = 0;
  BX_USB_THIS hub[0].usb_port[1].device_num = 1;
  BX_USB_THIS hub[0].device[1].dev_type = USB_DEV_TYPE_KEYPAD;

  BX_USB_THIS hub[0].device[1].state = STATE_DEFAULT;
  BX_USB_THIS hub[0].device[1].address = 0;
  BX_USB_THIS hub[0].device[1].alt_interface = 0;
  BX_USB_THIS hub[0].device[1].Interface = 0;
  BX_USB_THIS hub[0].device[1].config = 0;
  BX_USB_THIS hub[0].device[1].endpt = 1;
  BX_USB_THIS hub[0].device[1].function.direction = 0;
  BX_USB_THIS hub[0].device[1].function.configs = 1; // only one config in this device

  // device descriptor
  BX_USB_THIS hub[0].device[1].function.device_descr.len = 18;
  BX_USB_THIS hub[0].device[1].function.device_descr.type = DEVICE;
  BX_USB_THIS hub[0].device[1].function.device_descr.usb_ver = 0x0110;
  BX_USB_THIS hub[0].device[1].function.device_descr._class = 0;
  BX_USB_THIS hub[0].device[1].function.device_descr.subclass = 0;
  BX_USB_THIS hub[0].device[1].function.device_descr.protocol = 0;
  BX_USB_THIS hub[0].device[1].function.device_descr.max_packet_size = 8;
  BX_USB_THIS hub[0].device[1].function.device_descr.vendorid = 0x04B4;
  BX_USB_THIS hub[0].device[1].function.device_descr.productid = 0x0101;
  BX_USB_THIS hub[0].device[1].function.device_descr.device_rel = 0x0001;
  BX_USB_THIS hub[0].device[1].function.device_descr.manuf_indx = 1;
  BX_USB_THIS hub[0].device[1].function.device_descr.prod_indx = 2;
  BX_USB_THIS hub[0].device[1].function.device_descr.serial_indx = 0;
  BX_USB_THIS hub[0].device[1].function.device_descr.configs = 1;

  // config descriptor
  BX_USB_THIS hub[0].device[1].function.device_config[0].len = 9;
  BX_USB_THIS hub[0].device[1].function.device_config[0].type = CONFIG;
  BX_USB_THIS hub[0].device[1].function.device_config[0].tot_len = 59;   // size of config+interface+endpt+hid
  BX_USB_THIS hub[0].device[1].function.device_config[0].interfaces = 2;
  BX_USB_THIS hub[0].device[1].function.device_config[0].config_val = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].config_indx = 4;
  BX_USB_THIS hub[0].device[1].function.device_config[0].attrbs = 0xA0;  // bus powered, etc
  BX_USB_THIS hub[0].device[1].function.device_config[0].max_power = 50;

  // interface descriptor #1
  //    0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x05,   // interface descriptor
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].size = 9;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].type = INTERFACE;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].interface_num = 0;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].alternate = 0;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].num_endpts = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].iclass = 3;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].subclass = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].protocol = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].str_indx = 5;
  // this interface has a key conversion table of len = 18
  struct KEYPAD dev0_int0[KEYPAD_LEN] = {
    { 0x2A, 0x45 }, { 0x53, 0x11 }, { 0x54, 0x65 }, { 0x55, 0x64 }, { 0x56, 0x53 }, { 0x57, 0x52 },
    { 0x58, 0x63 }, { 0x59, 0x54 }, { 0x5A, 0x55 }, { 0x5B, 0x56 }, { 0x5C, 0x57 }, { 0x5D, 0x5E },
    { 0x5E, 0x58 }, { 0x5F, 0x59 }, { 0x60, 0x5A }, { 0x61, 0x5B }, { 0x62, 0x5C }, { 0x63, 0x5D },
  };
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].lookup_cnt = 18;
  memcpy(&BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].lookup, &dev0_int0, sizeof(struct KEYPAD) * KEYPAD_LEN);

  // endpoint descriptor #1
  //    0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x0A,               // endpoint descriptor
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].endpts[0].size = 7;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].endpts[0].type = ENDPOINT;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].endpts[0].endpt = 0x81;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].endpts[0].attrib = 3;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].endpts[0].max_size = 8;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].endpts[0].interval = 0x0A;

  // HID descriptor #1
  //    0x09, 0x21, 0x00, 0x01, 0x00, 0x01, 0x22, 0x41, 0x00    // hid descriptor
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].dev_hid_descript.size = 9;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].dev_hid_descript.type = 0x21;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].dev_hid_descript.HID_class = 0x0100;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].dev_hid_descript.country_code = 0;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].dev_hid_descript.num_descriptors = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].dev_hid_descript.descriptor[0].type = 0x22;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].dev_hid_descript.descriptor[0].len = 65;

  // HID descriptor type 0x22 (report descriptor) #1
  Bit8u dev1_rep0[] = {
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00, 0x25, 0x01,
    0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x03, 0x75, 0x01,
    0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91, 0x01, 0x95, 0x06,
    0x75, 0x08, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x05, 0x07, 0x19, 0x00, 0x2A, 0xFF, 0x00, 0x81, 0x00,
    0xC0
  };
  memcpy(BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[0].dev_hid_descript.descriptor[0].dev_hid_descript_report, dev1_rep0, 65);

  // interface descriptor #2
  //    0x09, 0x04, 0x01, 0x00, 0x01, 0x03, 0x01, 0x02, 0x06,   // interface descriptor
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].size = 9;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].type = INTERFACE;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].interface_num = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].alternate = 0;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].num_endpts = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].iclass = 3;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].subclass = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].protocol = 2;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].str_indx = 6;
  // this interface has no key conversion table
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].lookup_cnt = 0;
  memset(&BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].lookup, 0, sizeof(struct KEYPAD) * KEYPAD_LEN);
  
  // HID descriptor #2
  //    0x09, 0x21, 0x00, 0x01, 0x00, 0x01, 0x22, 0x32, 0x00    // hid descriptor
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].dev_hid_descript.size = 9;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].dev_hid_descript.type = 0x21;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].dev_hid_descript.HID_class = 0x0100;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].dev_hid_descript.country_code = 0;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].dev_hid_descript.num_descriptors = 1;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].dev_hid_descript.descriptor[0].type = 0x22;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].dev_hid_descript.descriptor[0].len = 50;

  // HID descriptor type 0x22 (report descriptor) #2
  Bit8u dev1_rep1[] = {
    0x05, 0x0C, 0x09, 0x01, 0xA1, 0x01, 0x85, 0x01, 0x19, 0x00, 0x2A, 0x3C, 0x02, 0x15, 0x00, 0x26,
    0x3C, 0x02, 0x95, 0x01, 0x75, 0x10, 0x81, 0x00, 0xC0, 0x05, 0x01, 0x09, 0x80, 0xA1, 0x01, 0x85,
    0x02, 0x19, 0x81, 0x29, 0x83, 0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x81, 0x02, 0x95, 0x05, 0x81,
    0x01, 0xC0
  };
  memcpy(BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].dev_hid_descript.descriptor[0].dev_hid_descript_report, dev1_rep1, 50);

  // endpoint descriptor #2
  //    0x07, 0x05, 0x82, 0x03, 0x05, 0x00, 0x0A,               // endpoint descriptor
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].endpts[0].size = 7;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].endpts[0].type = ENDPOINT;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].endpts[0].endpt = 0x82;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].endpts[0].attrib = 3;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].endpts[0].max_size = 5;
  BX_USB_THIS hub[0].device[1].function.device_config[0].Interface[1].endpts[0].interval = 0x0A;

  // string descriptors
  BX_USB_THIS hub[0].device[1].function.str_descriptor.size = 0x04;
  BX_USB_THIS hub[0].device[1].function.str_descriptor.type = 0x03;
  BX_USB_THIS hub[0].device[1].function.str_descriptor.langid[0] = 0x0409;

  // string #1
  BX_USB_THIS hub[0].device[1].function.string[0].size = 18;
  BX_USB_THIS hub[0].device[1].function.string[0].type = 3;
  Bit8u dev1_str_1[64] = { 'K', 0x00, 'e', 0x00, 'y', 0x00, 'M', 0x00, 'o', 0x00, 
    'u', 0x00, 's', 0x00, 'e', 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[1].function.string[0].unicode_str, dev1_str_1, 16);

  // string #2
  BX_USB_THIS hub[0].device[1].function.string[1].size = 34;
  BX_USB_THIS hub[0].device[1].function.string[1].type = 3;
  Bit8u dev1_str_2[64] = { 'U', 0x00, 'S', 0x00, 'B', 0x00, '/', 0x00, 'P', 0x00,
    'S', 0x00, '2', 0x00, ' ', 0x00, 'K', 0x00, 'E', 0x00, 'Y', 0x00, 'B', 0x00,
    'O', 0x00, 'A', 0x00, 'R', 0x00, 'D', 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[1].function.string[1].unicode_str, dev1_str_2, 32);

  // string #3
  BX_USB_THIS hub[0].device[1].function.string[2].size = 0;
  BX_USB_THIS hub[0].device[1].function.string[2].type = 3;
  memset(BX_USB_THIS hub[0].device[1].function.string[2].unicode_str, 0, 64);

  // string #4
  BX_USB_THIS hub[0].device[1].function.string[3].size = 26;
  BX_USB_THIS hub[0].device[1].function.string[3].type = 3;
  Bit8u dev1_str_4[64] = { 'H', 0x00, 'I', 0x00, 'D', 0x00, ' ', 0x00, 'K', 0x00, 
    'e', 0x00, 'y', 0x00, 'b', 0x00, 'o', 0x00, 'a', 0x00, 'r', 0x00, 'd', 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[1].function.string[3].unicode_str, dev1_str_4, 24);

  // string #5
  BX_USB_THIS hub[0].device[1].function.string[4].size = 28;
  BX_USB_THIS hub[0].device[1].function.string[4].type = 3;
  Bit8u dev1_str_5[64] = { 'E', 0x00, 'P', 0x00, '1', 0x00, ' ', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00,
    'e', 0x00, 'r', 0x00, 'r', 0x00, 'u', 0x00, 'p', 0x00, 't', 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[1].function.string[4].unicode_str, dev1_str_5, 26);

  // string #6
  BX_USB_THIS hub[0].device[1].function.string[5].size = 28;
  BX_USB_THIS hub[0].device[1].function.string[5].type = 3;
  Bit8u dev1_str_6[64] = { 'E', 0x00, 'P', 0x00, '2', 0x00, ' ', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00,
    'e', 0x00, 'r', 0x00, 'r', 0x00, 'u', 0x00, 'p', 0x00, 't', 0x00,
  };
  memcpy(BX_USB_THIS hub[0].device[1].function.string[5].unicode_str, dev1_str_6, 26);


#endif // USB_KEYPAD



// Another device would go here.



