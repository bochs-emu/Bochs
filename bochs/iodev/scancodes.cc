/////////////////////////////////////////////////////////////////////////
// $Id: scancodes.cc,v 1.1 2001-12-22 00:00:33 cbothamy Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  Christophe Bothamy
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


#include "bochs.h"
#include "scancodes.h"

unsigned char translation8042[256] = {
0xff,0x43,0x41,0x3f,0x3d,0x3b,0x3c,0x58,0x64,0x44,0x42,0x40,0x3e,0x0f,0x29,0x59,
0x65,0x38,0x2a,0x70,0x1d,0x10,0x02,0x5a,0x66,0x71,0x2c,0x1f,0x1e,0x11,0x03,0x5b,
0x67,0x2e,0x2d,0x20,0x12,0x05,0x04,0x5c,0x68,0x39,0x2f,0x21,0x14,0x13,0x06,0x5d,
0x69,0x31,0x30,0x23,0x22,0x15,0x07,0x5e,0x6a,0x72,0x32,0x24,0x16,0x08,0x09,0x5f,
0x6b,0x33,0x25,0x17,0x18,0x0b,0x0a,0x60,0x6c,0x34,0x35,0x26,0x27,0x19,0x0c,0x61,
0x6d,0x73,0x28,0x74,0x1a,0x0d,0x62,0x6e,0x3a,0x36,0x1c,0x1b,0x75,0x2b,0x63,0x76,
0x55,0x56,0x77,0x78,0x79,0x7a,0x0e,0x7b,0x7c,0x4f,0x7d,0x4b,0x47,0x7e,0x7f,0x6f,
0x52,0x53,0x50,0x4c,0x4d,0x48,0x01,0x45,0x57,0x4e,0x51,0x4a,0x37,0x49,0x46,0x54,
0x80,0x81,0x82,0x41,0x54,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};


// Definition of scancodes make and break, 
// for each set (mf1/xt , mf2/at , mf3/ps2)
// The table must be in BX_KEY order
//
scancode scancodes[BX_KEY_NBKEYS][3] =
{
 { // BX_KEY_CTRL_L ( ibm 58)
   { "\x1D" , "\x9D" },
   { "\x14" , "\xF0\x14" },
   { "\x11" , "\xF0\x11" },
 },

 { // BX_KEY_SHIFT_L ( ibm 44)
   { "\x2A" , "\xAA" },
   { "\x12" , "\xF0\x12" },
   { "\x12" , "\xF0\x12" },
 },

 { // BX_KEY_F1 ( ibm 112 )
   { "\x3B" , "\xBB" },
   { "\x05" , "\xF0\x05" },
   { "\x07" , "\xF0\x07" },
 },

 { // BX_KEY_F2 ( ibm 113 ) 
   { "\x3C" , "\xBC" },
   { "\x06" , "\xF0\x06" },
   { "\x0F" , "\xF0\x0F" },
 },

 { // BX_KEY_F3 ( ibm 114 ) 
   { "\x3D" , "\xBD" },
   { "\x04" , "\xF0\x04" },
   { "\x17" , "\xF0\x17" },
 },

 { // BX_KEY_F4 ( ibm 115 ) 
   { "\x3E" , "\xBE" },
   { "\x0C" , "\xF0\x0C" },
   { "\x1F" , "\xF0\x1F" },
 },

 { // BX_KEY_F5 ( ibm 116 ) 
   { "\x3F" , "\xBF" },
   { "\x03" , "\xF0\x03" },
   { "\x27" , "\xF0\x27" },
 },

 { // BX_KEY_F6 ( ibm 117 ) 
   { "\x40" , "\xC0" },
   { "\x0B" , "\xF0\x0B" },
   { "\x2F" , "\xF0\x2F" },
 },

 { // BX_KEY_F7 ( ibm 118 ) 
   { "\x41" , "\xC1" },
   { "\x83" , "\xF0\x83" },
   { "\x37" , "\xF0\x37" },
},

 { // BX_KEY_F8 ( ibm 119 ) 
   { "\x42" , "\xC2" },
   { "\x0A" , "\xF0\x0A" },
   { "\x3F" , "\xF0\x3F" },
 },

 { // BX_KEY_F9 ( ibm 120 ) 
   { "\x43" , "\xC3" },
   { "\x01" , "\xF0\x01" },
   { "\x47" , "\xF0\x47" },
 },

 { // BX_KEY_F10 ( ibm 121 ) 
   { "\x44" , "\xC4" },
   { "\x09" , "\xF0\x09" },
   { "\x4F" , "\xF0\x4F" },
 },

 { // BX_KEY_F11 ( ibm 122 ) 
   { "\x57" , "\xD7" },
   { "\x78" , "\xF0\x78" },
   { "\x56" , "\xF0\x56" },
 },

 { // BX_KEY_F12 ( ibm 123 ) 
   { "\x58" , "\xD8" },
   { "\x07" , "\xF0\x07" },
   { "\x5E" , "\xF0\x5E" },
 },

 { // BX_KEY_CTRL_R ( ibm 64 ) 
   { "\xE0\x1D" , "\xE0\x9D" },
   { "\xE0\x14" , "\xE0\xF0\x14" },
   { "\x58" ,     "\xF0x58" },
 },

 { // BX_KEY_SHIFT_R ( ibm 57 ) 
   { "\x36" , "\xB6" },
   { "\x59" , "\xF0\x59" },
   { "\x59" , "\xF0\x59" },
 },

 { // BX_KEY_CAPS_LOCK ( ibm 30 ) 
   { "\x3A" , "\xBA" },
   { "\x58" , "\xF0\x58" },
   { "\x14" , "\xF0\x14" },
 },

 { // BX_KEY_NUM_LOCK ( ibm 90 ) 
   { "\x45" , "\xC5" },
   { "\x77" , "\xF0\x77" },
   { "\x76" , "\xF0\x76" },
 },

 { // BX_KEY_ALT_L ( ibm 60 ) 
   { "\x38" , "\xB8" },
   { "\x11" , "\xF0\x11" },
   { "\x19" , "\xF0\x19" },
 },

 { // BX_KEY_ALT_R ( ibm 62 ) 
   { "\xE0\x38" , "\xE0\xB8" },
   { "\xE0\x11" , "\xE0\xF0\x11" },
   { "\x39" ,     "\xF0\x39" },
 },

 { // BX_KEY_A ( ibm 31 ) 
   { "\x1E" , "\x9E" },
   { "\x1C" , "\xF0\x1C" },
   { "\x1C" , "\xF0\x1C" },
 },

 { // BX_KEY_B ( ibm 50 ) 
   { "\x30" , "\xB0" },
   { "\x32" , "\xF0\x32" },
   { "\x32" , "\xF0\x32" },
 },

 { // BX_KEY_C ( ibm 48 ) 
   { "\x2E" , "\xAE" },
   { "\x21" , "\xF0\x21" },
   { "\x21" , "\xF0\x21" },
 },

 { // BX_KEY_D ( ibm 33 ) 
   { "\x20" , "\xA0" },
   { "\x23" , "\xF0\x23" },
   { "\x23" , "\xF0\x23" },
 },

 { // BX_KEY_E ( ibm 19 ) 
   { "\x12" , "\x92" },
   { "\x24" , "\xF0\x24" },
   { "\x24" , "\xF0\x24" },
 },

 { // BX_KEY_F ( ibm 34 ) 
   { "\x21" , "\xA1" },
   { "\x2B" , "\xF0\x2B" },
   { "\x2B" , "\xF0\x2B" },
 },

 { // BX_KEY_G ( ibm 35 ) 
   { "\x22" , "\xA2" },
   { "\x34" , "\xF0\x34" },
   { "\x34" , "\xF0\x34" },
 },

 { // BX_KEY_H ( ibm 36 ) 
   { "\x23" , "\xA3" },
   { "\x33" , "\xF0\x33" },
   { "\x33" , "\xF0\x33" },
 },

 { // BX_KEY_I ( ibm 24 ) 
   { "\x17" , "\x97" },
   { "\x43" , "\xF0\x43" },
   { "\x43" , "\xF0\x43" },
 },

 { // BX_KEY_J ( ibm 37 ) 
   { "\x24" , "\xA4" },
   { "\x3B" , "\xF0\x3B" },
   { "\x3B" , "\xF0\x3B" },
 },

 { // BX_KEY_K ( ibm 38 ) 
   { "\x25" , "\xA5" },
   { "\x42" , "\xF0\x42" },
   { "\x42" , "\xF0\x42" },
 },

 { // BX_KEY_L ( ibm 39 ) 
   { "\x26" , "\xA6" },
   { "\x4B" , "\xF0\x4B" },
   { "\x4B" , "\xF0\x4B" },
 },

 { // BX_KEY_M ( ibm 52 ) 
   { "\x32" , "\xB2" },
   { "\x3A" , "\xF0\x3A" },
   { "\x3A" , "\xF0\x3A" },
 },

 { // BX_KEY_N ( ibm 51 ) 
   { "\x31" , "\xB1" },
   { "\x31" , "\xF0\x31" },
   { "\x31" , "\xF0\x31" },
 },

 { // BX_KEY_O ( ibm 25 ) 
   { "\x18" , "\x98" },
   { "\x44" , "\xF0\x44" },
   { "\x44" , "\xF0\x44" },
 },

 { // BX_KEY_P ( ibm 26 ) 
   { "\x19" , "\x99" },
   { "\x4D" , "\xF0\x4D" },
   { "\x4D" , "\xF0\x4D" },
 },

 { // BX_KEY_Q ( ibm 17 ) 
   { "\x10" , "\x90" },
   { "\x15" , "\xF0\x15" },
   { "\x15" , "\xF0\x15" },
 },

 { // BX_KEY_R ( ibm 20 ) 
   { "\x13" , "\x93" },
   { "\x2D" , "\xF0\x2D" },
   { "\x2D" , "\xF0\x2D" },
 },

 { // BX_KEY_S ( ibm 32 ) 
   { "\x1F" , "\x9F" },
   { "\x1B" , "\xF0\x1B" },
   { "\x1B" , "\xF0\x1B" },
 },

 { // BX_KEY_T ( ibm 21 ) 
   { "\x14" , "\x94" },
   { "\x2C" , "\xF0\x2C" },
   { "\x2C" , "\xF0\x2C" },
 },

 { // BX_KEY_U ( ibm 23 ) 
   { "\x16" , "\x96" },
   { "\x3C" , "\xF0\x3C" },
   { "\x3C" , "\xF0\x3C" },
 },

 { // BX_KEY_V ( ibm 49 ) 
   { "\x2F" , "\xAF" },
   { "\x2A" , "\xF0\x2A" },
   { "\x2A" , "\xF0\x2A" },
 },

 { // BX_KEY_W ( ibm 18 ) 
   { "\x11" , "\x91" },
   { "\x1D" , "\xF0\x1D" },
   { "\x1D" , "\xF0\x1D" },
 },

 { // BX_KEY_X ( ibm 47 ) 
   { "\x2D" , "\xAD" },
   { "\x22" , "\xF0\x22" },
   { "\x22" , "\xF0\x22" },
 },

 { // BX_KEY_Y ( ibm 22 ) 
   { "\x15" , "\x95" },
   { "\x35" , "\xF0\x35" },
   { "\x35" , "\xF0\x35" },
 },

 { // BX_KEY_Z ( ibm 46 ) 
   { "\x2C" , "\xAC" },
   { "\x1A" , "\xF0\x1A" },
   { "\x1A" , "\xF0\x1A" },
 },

 { // BX_KEY_0 ( ibm 11 ) 
   { "\x0B" , "\x8B" },
   { "\x45" , "\xF0\x45" },
   { "\x45" , "\xF0\x45" },
 },

 { // BX_KEY_1 ( ibm 2 ) 
   { "\x02" , "\x82" },
   { "\x16" , "\xF0\x16" },
   { "\x16" , "\xF0\x16" },
 },

 { // BX_KEY_2 ( ibm 3 ) 
   { "\x03" , "\x83" },
   { "\x1E" , "\xF0\x1E" },
   { "\x1E" , "\xF0\x1E" },
 },

 { // BX_KEY_3 ( ibm 4 ) 
   { "\x04" , "\x84" },
   { "\x26" , "\xF0\x26" },
   { "\x26" , "\xF0\x26" },
 },

 { // BX_KEY_4 ( ibm 5 ) 
   { "\x05" , "\x85" },
   { "\x25" , "\xF0\x25" },
   { "\x25" , "\xF0\x25" },
 },

 { // BX_KEY_5 ( ibm 6 ) 
   { "\x06" , "\x86" },
   { "\x2E" , "\xF0\x2E" },
   { "\x2E" , "\xF0\x2E" },
 },

 { // BX_KEY_6 ( ibm 7 ) 
   { "\x07" , "\x87" },
   { "\x36" , "\xF0\x36" },
   { "\x36" , "\xF0\x36" },
 },

 { // BX_KEY_7 ( ibm 8 ) 
   { "\x08" , "\x88" },
   { "\x3D" , "\xF0\x3D" },
   { "\x3D" , "\xF0\x3D" },
 },

 { // BX_KEY_8 ( ibm 9 ) 
   { "\x09" , "\x89" },
   { "\x3E" , "\xF0\x3E" },
   { "\x3E" , "\xF0\x3E" },
 },

 { // BX_KEY_9 ( ibm 10 ) 
   { "\x0A" , "\x8A" },
   { "\x46" , "\xF0\x46" },
   { "\x46" , "\xF0\x46" },
 },

 { // BX_KEY_ESC ( ibm 110 ) 
   { "\x01" , "\x81" },
   { "\x76" , "\xF0\x76" },
   { "\x08" , "\xF0\x08" },
 },

 { // BX_KEY_SPACE ( ibm 61 ) 
   { "\x39" , "\xB9" },
   { "\x29" , "\xF0\x29" },
   { "\x29" , "\xF0\x29" },
 },

 { // BX_KEY_SINGLE_QUOTE ( ibm 41 ) 
   { "\x28" , "\xA8" },
   { "\x52" , "\xF0\x52" },
   { "\x52" , "\xF0\x52" },
 },

 { // BX_KEY_COMMA ( ibm 53 ) 
   { "\x33" , "\xB3" },
   { "\x41" , "\xF0\x41" },
   { "\x41" , "\xF0\x41" },
 },

 { // BX_KEY_PERIOD ( ibm 54 ) 
   { "\x34" , "\xB4" },
   { "\x49" , "\xF0\x49" },
   { "\x49" , "\xF0\x49" },
 },

 { // BX_KEY_SLASH ( ibm 55 ) 
   { "\x35" , "\xB5" },
   { "\x4A" , "\xF0\x4A" },
   { "\x4A" , "\xF0\x4A" },
 },

 { // BX_KEY_SEMICOLON ( ibm 40 ) 
   { "\x27" , "\xA7" },
   { "\x4C" , "\xF0\x4C" },
   { "\x4C" , "\xF0\x4C" },
 },

 { // BX_KEY_EQUALS ( ibm 13 ) 
   { "\x0D" , "\x8D" },
   { "\x55" , "\xF0\x55" },
   { "\x55" , "\xF0\x55" },
 },

 { // BX_KEY_LEFT_BRACKET ( ibm 27 ) 
   { "\x1A" , "\x9A" },
   { "\x54" , "\xF0\x54" },
   { "\x54" , "\xF0\x54" },
 },

 { // BX_KEY_BACKSLASH ( ibm 42, 29)
   { "\x2B" , "\xAB" },
   { "\x5D" , "\xF0\x5D" },
   { "\x53" , "\xF0\x53" },
 },

 { // BX_KEY_RIGHT_BRACKET ( ibm 28 ) 
   { "\x1B" , "\x9B" },
   { "\x5B" , "\xF0\x5B" },
   { "\x5B" , "\xF0\x5B" },
 },

 { // BX_KEY_MINUS ( ibm 12 ) 
   { "\x0C" , "\x8C" },
   { "\x4E" , "\xF0\x4E" },
   { "\x4E" , "\xF0\x4E" },
 },

 { // BX_KEY_GRAVE ( ibm 1 ) 
   { "\x29" , "\xA9" },
   { "\x0E" , "\xF0\x0E" },
   { "\x0E" , "\xF0\x0E" },
 },

 { // BX_KEY_BACKSPACE ( ibm 15 ) 
   { "\x0E" , "\x8E" },
   { "\x66" , "\xF0\x66" },
   { "\x66" , "\xF0\x66" },
 },

 { // BX_KEY_ENTER ( ibm 43 ) 
   { "\x1C" , "\x9C" },
   { "\x5A" , "\xF0\x5A" },
   { "\x5A" , "\xF0\x5A" },
 },

 { // BX_KEY_TAB ( ibm 16 ) 
   { "\x0F" , "\x8F" },
   { "\x0D" , "\xF0\x0D" },
   { "\x0D" , "\xF0\x0D" },
 },

 { // BX_KEY_LEFT_BACKSLASH ( ibm 45 ) 
   { "\x56" , "\xD6" },
   { "\x61" , "\xF0\x61" },
   { "\x13" , "\xF0\x13" },
 },

 { // BX_KEY_PRINT ( ibm 124 ) 
   { "\xE0\x2A\xE0\x37" , "\xE0\xAA\xE0\xB7" },
   { "\xE0\x12\xE0\x7C" , "\xE0\xF0\x12\xE0\xF0\x7C" },
   { "\x57" ,             "\xF0\x57" },
 },

 { // BX_KEY_SCRL_LOCK ( ibm 125 ) 
   { "\x46" , "\xC6" },
   { "\x7E" , "\xF0\x7E" },
   { "\x5F" , "\\xF0x5F" },
 },

 { // BX_KEY_PAUSE ( ibm 126 ) 
   { "\xE1\x1D\x45\xE1\x9D\xC5" ,         "" },
   { "\xE1\x14\x77\xE1\xF0\x14\xF0\x77" , "" },
   { "\x62" ,                             "\xF0\x62" },
 },

 { // BX_KEY_INSERT ( ibm 75 ) 
   { "\xE0\x52" , "\xE0\xD2" },
   { "\xE0\x70" , "\xE0\xF0\x70" },
   { "\x67" ,     "\xF0\x67" },
 },

 { // BX_KEY_DELETE ( ibm 76 ) 
   { "\xE0\x53" , "\xE0\xD3" },
   { "\xE0\x71" , "\xE0\xF0\x71" },
   { "\x64" ,     "\xF0\x64" },
 },

 { // BX_KEY_HOME ( ibm 80 ) 
   { "\xE0\x47" , "\xE0\xC7" },
   { "\xE0\x6C" , "\xE0\xF0\x6C" },
   { "\x6E" ,     "\xF0\x6E" },
 },

 { // BX_KEY_END ( ibm 81 ) 
   { "\xE0\x4F" , "\xE0\xCF" },
   { "\xE0\x69" , "\xE0\xF0\x69" },
   { "\x65" ,     "\xF0\x65" },
 },

 { // BX_KEY_PAGE_UP ( ibm 85 ) 
   { "\xE0\x49" , "\xE0\xC9" },
   { "\xE0\x7D" , "\xE0\xF0\x7D" },
   { "\x6F" ,     "\xF0\x6F" },
 },

 { // BX_KEY_PAGE_DOWN ( ibm 86 ) 
   { "\xE0\x51" , "\xE0\xD1" },
   { "\xE0\x7A" , "\xE0\xF0\x7A" },
   { "\x6D" ,     "\xF0\x6D" },
 },

 { // BX_KEY_KP_ADD ( ibm 106 ) 
   { "\x4E" , "\xCE" },
   { "\x79" , "\xF0\x79" },
   { "\x7C" , "\xF0\x7C" },
 },

 { // BX_KEY_KP_SUBTRACT ( ibm 105 ) 
   { "\x4A" , "\xCA" },
   { "\x7B" , "\xF0\x7B" },
   { "\x84" , "\xF0\x84" },
 },

 { // BX_KEY_KP_END ( ibm 93 ) 
   { "\x4F" , "\xCF" },
   { "\x69" , "\xF0\x69" },
   { "\x69" , "\xF0\x69" },
 },

 { // BX_KEY_KP_DOWN ( ibm 98 ) 
   { "\x50" , "\xD0" },
   { "\x72" , "\xF0\x72" },
   { "\x72" , "\xF0\x72" },
 },

 { // BX_KEY_KP_PAGE_DOWN ( ibm 103 ) 
   { "\x51" , "\xD1" },
   { "\x7A" , "\xF0\x7A" },
   { "\x7A" , "\xF0\x7A" },
 },

 { // BX_KEY_KP_LEFT ( ibm 92 ) 
   { "\x4B" , "\xCB" },
   { "\x6B" , "\xF0\x6B" },
   { "\x6B" , "\xF0\x6B" },
 },

 { // BX_KEY_KP_RIGHT ( ibm 102 ) 
   { "\x4D" , "\xCD" },
   { "\x74" , "\xF0\x74" },
   { "\x74" , "\xF0\x74" },
 },

 { // BX_KEY_KP_HOME ( ibm 91 ) 
   { "\x47" , "\xC7" },
   { "\x6C" , "\xF0\x6C" },
   { "\x6C" , "\xF0\x6C" },
 },

 { // BX_KEY_KP_UP ( ibm 96 ) 
   { "\x48" , "\xC8" },
   { "\x75" , "\xF0\x75" },
   { "\x75" , "\xF0\x75" },
 },

 { // BX_KEY_KP_PAGE_UP ( ibm 101 ) 
   { "\x49" , "\xC9" },
   { "\x7D" , "\xF0\x7D" },
   { "\x7D" , "\xF0\x7D" },
 },

 { // BX_KEY_KP_INSERT ( ibm 99 ) 
   { "\x52" , "\xD2" },
   { "\x70" , "\xF0\x70" },
   { "\x70" , "\xF0\x70" },
 },

 { // BX_KEY_KP_DELETE ( ibm 104 ) 
   { "\x53" , "\xD3" },
   { "\x71" , "\xF0\x71" },
   { "\x71" , "\xF0\x71" },
 },

 { // BX_KEY_KP_5 ( ibm 97 ) 
   { "\x4C" , "\xCC" },
   { "\x73" , "\xF0\x73" },
   { "\x73" , "\xF0\x73" },
 },

 { // BX_KEY_UP ( ibm 83 ) 
   { "\xE0\x48" , "\xE0\xC8" },
   { "\xE0\x75" , "\xE0\xF0\x75" },
   { "\x63" ,     "\xF0\x63" },
 },

 { // BX_KEY_DOWN ( ibm 84 ) 
   { "\xE0\x50" , "\xE0\xD0" },
   { "\xE0\x72" , "\xE0\xF0\x72" },
   { "\x60" ,     "\xF0\x60" },
 },

 { // BX_KEY_LEFT ( ibm 79 ) 
   { "\xE0\x4B" , "\xE0\xCB" },
   { "\xE0\x6B" , "\xE0\xF0\x6B" },
   { "\x61" ,     "\xF0\x61" },
 },

 { // BX_KEY_RIGHT ( ibm 89 ) 
   { "\xE0\x4D" , "\xE0\xCD" },
   { "\xE0\x74" , "\xE0\xF0\x74" },
   { "\x6A" ,     "\xF0\x6A" },
 },

 { // BX_KEY_KP_ENTER ( ibm 108 ) 
   { "\xE0\x1C" , "\xE0\x9C" },
   { "\xE0\x5A" , "\xE0\xF0\x5A" },
   { "\x79" ,     "\xF0\x79" },
 },

 { // BX_KEY_KP_MULTIPLY ( ibm 100 ) 
   { "\x37" , "\xB7" },
   { "\x7C" , "\xF0\x7C" },
   { "\x7E" , "\xF0\x7E" },
 },

 { // BX_KEY_KP_DIVIDE ( ibm 95 ) 
   { "\xE0\x35" , "\xE0\xB5" },
   { "\xE0\x4A" , "\xE0\xF0\x4A" },
   { "\x77" ,     "\xF0\x77" },
 },

 { // BX_KEY_PRINT_SCREEN 
   { "\xE0\x37" , "\xE0\xB7" },
   { "\xE0\x7C" , "\xE0\xF0\x7C" },
   { "\x57" ,     "\xF0\x57" },
 },

 { // BX_KEY_SCROLL_LOCK 
   { "\x46" ,   "\xC6" },
   { "\x7E" ,   "\xF0\x7E" },
   { "\x5F" ,   "\xF0\x5F" },
 },

 { // BX_KEY_WIN_L 
   { "\xE0\x5B" , "\xE0\xDB" },
   { "\xE0\x1F" , "\xE0\xF0\x1F" },
   { "\x8B" ,     "\xF0\x8B" },
 },

 { // BX_KEY_WIN_R
   { "\xE0\x5C" , "\xE0\xDC" },
   { "\xE0\x27" , "\xE0\xF0\x27" },
   { "\x8C" ,     "\xF0\x8C" },
 },

 { // BX_KEY_MENU
   { "\xE0\x5D" , "\xE0\xDD" },
   { "\xE0\x2F" , "\xE0\xF0\x2F" },
   { "\x8D" ,     "\xF0\x8D" },
 },

 { // BX_KEY_ALT_SYSREQ
   { "\x54" ,   "\xD4" },
   { "\x84" ,   "\xF0\x84" },
   { "\x57" ,   "\xF0\x57" },
 },

 { // BX_KEY_CTRL_BREAK
   { "\xE0\x46" , "\xE0\xC6" },
   { "\xE0\x7E" , "\xE0\xF0\x7E" },
   { "\x62" ,     "\xF0\x62" },
 },

 { // BX_KEY_INT_BACK
   { "\xE0\x6A" , "\xE0\xEA" },
   { "\xE0\x38" , "\xE0\xF0\x38" },
   { "\x38" ,     "\xF0\x38" },
 },

 { // BX_KEY_INT_FORWARD
   { "\xE0\x69" , "\xE0\xE9" },
   { "\xE0\x30" , "\xE0\xF0\x30" },
   { "\x30" ,     "\xF0\x30" },
 },

 { // BX_KEY_INT_STOP
   { "\xE0\x68" , "\xE0\xE8" },
   { "\xE0\x28" , "\xE0\xF0\x28" },
   { "\x28" ,     "\xF0\x28" },
 },

 { // BX_KEY_INT_MAIL
   { "\xE0\x6C" , "\xE0\xEC" },
   { "\xE0\x48" , "\xE0\xF0\x48" },
   { "\x48" ,     "\xF0\x48" },
 },

 { // BX_KEY_INT_SEARCH
   { "\xE0\x65" , "\xE0\xE5" },
   { "\xE0\x10" , "\xE0\xF0\x10" },
   { "\x10" ,     "\xF0\x10" },
 },

 { // BX_KEY_INT_FAV
   { "\xE0\x66" , "\xE0\xE6" },
   { "\xE0\x18" , "\xE0\xF0\x18" },
   { "\x18" ,     "\xF0\x18" },
 },

 { // BX_KEY_INT_HOME
   { "\xE0\x32" , "\xE0\xB2" },
   { "\xE0\x3A" , "\xE0\xF0\x3A" },
   { "\x97" ,     "\xF0\x97" },
 },

 { // BX_KEY_POWER_MYCOMP
   { "\xE0\x6B" , "\xE0\xEB" },
   { "\xE0\x40" , "\xE0\xF0\x40" },
   { "\x40" ,     "\xF0\x40" },
 },

 { // BX_KEY_POWER_CALC
   { "\xE0\x21" , "\xE0\xA1" },
   { "\xE0\x2B" , "\xE0\xF0\x2B" },
   { "\x99" ,     "\xF0\x99" },
 },

 { // BX_KEY_POWER_SLEEP
   { "\xE0\x5F" , "\xE0\xDF" },
   { "\xE0\x3F" , "\xE0\xF0\x3F" },
   { "\x7F" ,     "\xF0\x7F" },
 },

 { // BX_KEY_POWER_POWER
   { "\xE0\x5E" , "\xE0\xDE" },
   { "\xE0\x37" , "\xE0\xF0\x37" },
   { "" ,         "" },
 },

 { // BX_KEY_POWER_WAKE
   { "\xE0\x63" , "\xE0\xE3" },
   { "\xE0\x5E" , "\xE0\xF0\x5E" },
   { "" ,         "" },
 },

};
