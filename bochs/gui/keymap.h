/////////////////////////////////////////////////////////////////////////
// $Id: keymap.h,v 1.1 2001-12-14 17:54:58 cbothamy Exp $
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

/////////////////////////////////////////////////////////////////////////
//
// Methods of bx_keymap_c :
//  - void   addTable(const char *mapName, const void *table, Bit32u size);
//    Adds the table named "mapName" to the keymap list
//    Size should be the number of entries in the
//
//  - void   setDefault(const char *mapName);
//    Sets the default keymap
//
//  - void*  getTable(char *mapName);
//    Returns the table named "mapName"
//
//  - void*  getTable(void);
//    Returns the table specified in the conf file
//
//  - Bit32u getBXKey(char *mapName, Bit32u key);
//    Returns the BX_KEY for the key and the table "mapName"
//
//  - Bit32u getBXKey(Bit32u key);
//    Returns the BX_KEY for the key and the table specified in
//    the conf file.
//
// Format of the table
//  - if the GUI provide its own translation function, the format
//    of the keymap table is free and the number of elements
//    should be set to 0. This GUI should also use the getTable
//    functions.
//  - if the GUI wants the keymap object do the translation to
//    BX_KEYs then the format of the table is Bit32u table[N][2].
//    Each entry should contain the physical constant and the
//    associated BX_KEY
//
// Table writing
//   Each GUI should provide at least one table, the standard 'us'
//   keyboard one. Then they should provide additional table for
//   each country specific keyboard, as a diff to the standard one.
//
//   Just look at each key of the US Keyboard, and write down
//   what you see on your real keyboard
//   This gives you the XK_* symbol (your keyboard)
//   and the BX_KEY_* symbol (US keyboard)
// 
//   You may have to add your country specific X11 keysyms
//   and don't forget to add a line for every symbol you can type
//   on each key
//
//   The US PC Keyboard looks like this (can somebody confirm ?)
//
//   Top Row   Esc F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12
//   2nd row   ` 1  2  3  4  5 6  7  8  9 0  -  = \ Back
//   3rd row   Tab Q  W  E  R  T  Y  U  I  O  P [ ] Enter
//   4rd row   Caps A  S  D  F  G  H  J  K  L  ;  '
//   5rd row   lShift l\ Z X C V B N M , . / rShift
//   6rd row   lCtrl lAlt     Space     rAlt rCtrl

#define BX_KEYMAP_MAX_TABLES 10

class bx_keymap_c : public logfunctions {
public:
  bx_keymap_c(void);
  ~bx_keymap_c(void);

  void   addTable(const char *mapName, const void *table, Bit32u size);
  void   setDefault(const char *mapName);
  void*  getTable(char *mapName);
  void*  getTable(void);
  Bit32u getBXKey(char *mapName, Bit32u key);
  Bit32u getBXKey(Bit32u key);

private:
  const char*   mapNames[BX_KEYMAP_MAX_TABLES];
  const void*   keymaps[BX_KEYMAP_MAX_TABLES];
  Bit32u        mapSizes[BX_KEYMAP_MAX_TABLES];
  Bit32u        defaultKeymap;
  Bit32u        nbKeymaps;
};

extern bx_keymap_c bx_keymap;
