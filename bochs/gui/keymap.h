/////////////////////////////////////////////////////////////////////////
// $Id: keymap.h,v 1.2 2002-03-06 09:31:55 cbothamy Exp $
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
//
// - loadKeymap(Bit32u convertStringToSymbol(const char*));
//   loads the configuration specified keymap file if keymapping is enabled
//   using convertStringToSymbol to convert strings to client constants
//
// - loadKeymap(Bit32u convertStringToSymbol(const char*), const char* filename);
//   loads the specified keymap file 
//   using convertStringToSymbol to convert strings to client constants
//
// - convertStringToBXKey
//   convert a string to a BX_KEY code
//

// In case of unknown symbol
#define BX_KEYMAP_UNKNOWN   0xFFFFFFFF

// Structure of an element of the keymap table
typedef struct { 
  Bit32u bxKey;
  Bit32u guiKey;
  } KeyEntry;

class bx_keymap_c : public logfunctions {
public:
  bx_keymap_c(void);
  ~bx_keymap_c(void);

  void   loadKeymap(Bit32u(*)(const char*));
  void   loadKeymap(Bit32u(*)(const char*),const char *filename);


  Bit32u getBXKey(Bit32u key);

private:
  Bit32u convertStringToBXKey(const char *);
 
  KeyEntry *keymapTable;
  Bit16u   keymapCount;
  };

extern bx_keymap_c bx_keymap;
