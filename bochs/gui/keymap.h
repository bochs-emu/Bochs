/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2009  The Bochs Project
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

/////////////////////////////////////////////////////////////////////////
//
// Methods of bx_keymap_c :
//
// - loadKeymap(Bit32u convertStringToSymbol(const char*));
//   loads the configuration specified keymap file if keymapping is enabled
//   using convertStringToSymbol to convert strings to client constants
//
// - loadKeymap(Bit32u convertStringToSymbol(const char*), const char* filename);
//   loads the specified keymap file
//   using convertStringToSymbol to convert strings to client constants
//
// - isKeymapLoaded () returns true if the keymap contains any valid key
//   entries.
//
// - convertStringToBXKey
//   convert a null-terminate string to a BX_KEY code
//
// - findHostKey(Bit32u key)
// - findAsciiChar(Bit8u ch)
//   Each of these methods returns a pointer to a BXKeyEntry structure
//   corresponding to a key.  findHostKey() finds an entry whose hostKey
//   value matches the target value, and findAsciiChar() finds an entry
//   whose ASCII code matches the search value.

// In case of unknown symbol
#define BX_KEYMAP_UNKNOWN   0xFFFFFFFF

// Structure of an element of the keymap table
typedef struct BOCHSAPI {
  Bit32u baseKey;   // base key
  Bit32u modKey;   // modifier key that must be held down
  Bit32s ascii;    // ascii equivalent, if any
  Bit32u hostKey;  // value that the host's OS or library recognizes
} BXKeyEntry;

class BOCHSAPI bx_keymap_c : public logfunctions {
public:
  bx_keymap_c(void);
 ~bx_keymap_c(void);

  void   loadKeymap(Bit32u(*)(const char*));
  void   loadKeymap(Bit32u(*)(const char*),const char *filename);
  bx_bool isKeymapLoaded();

  BXKeyEntry *findHostKey(Bit32u hostkeynum);
  BXKeyEntry *findAsciiChar(Bit8u ascii);
  const char *getBXKeyName(Bit32u key);

private:
  Bit32u convertStringToBXKey(const char *);

  BXKeyEntry *keymapTable;
  Bit16u   keymapCount;
};

BOCHSAPI extern bx_keymap_c bx_keymap;
