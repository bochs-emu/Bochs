/////////////////////////////////////////////////////////////////////////
// $Id: keymap.h,v 1.5 2002-04-01 22:43:01 cbothamy Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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
// - getKeyXwin(Bit32u key)
// - getKeyASCII(Bit8u ch)
//   Each of these methods returns a pointer to a BXKeyEntry structure
//   corresponding to a key.  getKeyXwin() uses an X windows keysym to find the
//   structure, and getKeyASCII() uses an ASCII code to find the structure.

// In case of unknown symbol
#define BX_KEYMAP_UNKNOWN   0xFFFFFFFF

// Structure of an element of the keymap table
typedef struct { 
  Bit32u baseKey;   // base key
  Bit32u modKey;   // modifier key that must be held down
  Bit32s ascii;    // ascii equivalent, if any
  Bit32u xwinKey;  // X windows value
  } BXKeyEntry;

class bx_keymap_c : public logfunctions {
public:
  bx_keymap_c(void);
  ~bx_keymap_c(void);

  void   loadKeymap(Bit32u(*)(const char*));
  void   loadKeymap(Bit32u(*)(const char*),const char *filename);
  Boolean isKeymapLoaded ();

  BXKeyEntry *getKeyXwin(Bit32u xwin_key);
  BXKeyEntry *getKeyASCII(Bit8u ascii);

private:
  Bit32u convertStringToBXKey(const char *);
 
  BXKeyEntry *keymapTable;
  Bit16u   keymapCount;
  };

extern bx_keymap_c bx_keymap;
