/////////////////////////////////////////////////////////////////////////
// $Id: keymap.cc,v 1.2 2002-03-06 09:31:55 cbothamy Exp $
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
// Todo
//  . Currently supported only by x11. Check if other guis need mapping.
//  . Tables look-up should be optimised.
//

#include "bochs.h"

// Table of bocks "BX_KEY_*" symbols
// the table must be in BX_KEY_* order
char *bx_key_symbol[BX_KEY_NBKEYS] = {
  "BX_KEY_CTRL_L",         "BX_KEY_SHIFT_L",        "BX_KEY_F1",
  "BX_KEY_F2",             "BX_KEY_F3",             "BX_KEY_F4",
  "BX_KEY_F5",             "BX_KEY_F6",             "BX_KEY_F7",
  "BX_KEY_F8",             "BX_KEY_F9",             "BX_KEY_F10",
  "BX_KEY_F11",            "BX_KEY_F12",            "BX_KEY_CTRL_R",
  "BX_KEY_SHIFT_R",        "BX_KEY_CAPS_LOCK",      "BX_KEY_NUM_LOCK",
  "BX_KEY_ALT_L",          "BX_KEY_ALT_R",          "BX_KEY_A",
  "BX_KEY_B",              "BX_KEY_C",              "BX_KEY_D",
  "BX_KEY_E",              "BX_KEY_F",              "BX_KEY_G",
  "BX_KEY_H",              "BX_KEY_I",              "BX_KEY_J",
  "BX_KEY_K",              "BX_KEY_L",              "BX_KEY_M",
  "BX_KEY_N",              "BX_KEY_O",              "BX_KEY_P",
  "BX_KEY_Q",              "BX_KEY_R",              "BX_KEY_S",
  "BX_KEY_T",              "BX_KEY_U",              "BX_KEY_V",
  "BX_KEY_W",              "BX_KEY_X",              "BX_KEY_Y",
  "BX_KEY_Z",              "BX_KEY_0",              "BX_KEY_1",
  "BX_KEY_2",              "BX_KEY_3",              "BX_KEY_4",
  "BX_KEY_5",              "BX_KEY_6",              "BX_KEY_7",
  "BX_KEY_8",              "BX_KEY_9",              "BX_KEY_ESC",
  "BX_KEY_SPACE",          "BX_KEY_SINGLE_QUOTE",   "BX_KEY_COMMA",
  "BX_KEY_PERIOD",         "BX_KEY_SLASH",          "BX_KEY_SEMICOLON",
  "BX_KEY_EQUALS",         "BX_KEY_LEFT_BRACKET",   "BX_KEY_BACKSLASH",
  "BX_KEY_RIGHT_BRACKET",  "BX_KEY_MINUS",          "BX_KEY_GRAVE",
  "BX_KEY_BACKSPACE",      "BX_KEY_ENTER",          "BX_KEY_TAB",
  "BX_KEY_LEFT_BACKSLASH", "BX_KEY_PRINT",          "BX_KEY_SCRL_LOCK",
  "BX_KEY_PAUSE",          "BX_KEY_INSERT",         "BX_KEY_DELETE",
  "BX_KEY_HOME",           "BX_KEY_END",            "BX_KEY_PAGE_UP",
  "BX_KEY_PAGE_DOWN",      "BX_KEY_KP_ADD",         "BX_KEY_KP_SUBTRACT",
  "BX_KEY_KP_END",         "BX_KEY_KP_DOWN",        "BX_KEY_KP_PAGE_DOWN",
  "BX_KEY_KP_LEFT",        "BX_KEY_KP_RIGHT",       "BX_KEY_KP_HOME",
  "BX_KEY_KP_UP",          "BX_KEY_KP_PAGE_UP",     "BX_KEY_KP_INSERT",
  "BX_KEY_KP_DELETE",      "BX_KEY_KP_5",           "BX_KEY_UP",
  "BX_KEY_DOWN",           "BX_KEY_LEFT",           "BX_KEY_RIGHT",
  "BX_KEY_KP_ENTER",       "BX_KEY_KP_MULTIPLY",    "BX_KEY_KP_DIVIDE",
  "BX_KEY_PRINT_SCREEN",   "BX_KEY_SCROLL_LOCK",    "BX_KEY_WIN_L",
  "BX_KEY_WIN_R",          "BX_KEY_MENU",           "BX_KEY_ALT_SYSREQ",
  "BX_KEY_CTRL_BREAK",     "BX_KEY_INT_BACK",       "BX_KEY_INT_FORWARD",
  "BX_KEY_INT_STOP",       "BX_KEY_INT_MAIL",       "BX_KEY_INT_SEARCH",
  "BX_KEY_INT_FAV",        "BX_KEY_INT_HOME",       "BX_KEY_POWER_MYCOMP",
  "BX_KEY_POWER_CALC",     "BX_KEY_POWER_SLEEP",    "BX_KEY_POWER_POWER",
  "BX_KEY_POWER_WAKE",
  };

bx_keymap_c bx_keymap;

#define LOG_THIS bx_keymap.

bx_keymap_c::bx_keymap_c(void)
{
    put("KMAP");

    keymapCount = 0;
    keymapTable = (KeyEntry *)NULL;

}

bx_keymap_c::~bx_keymap_c(void)
{
    if(keymapTable != NULL) {
      free(keymapTable);
      keymapTable = (KeyEntry *)NULL;
      }
    keymapCount = 0;
}

    void
bx_keymap_c::loadKeymap(Bit32u stringToSymbol(const char*))
{
  if(bx_options.keyboard.OuseMapping->get()) {
    loadKeymap(stringToSymbol,bx_options.keyboard.Okeymap->getptr());
    }
}

    void
bx_keymap_c::loadKeymap(Bit32u stringToSymbol(const char*), const char* filename)
{
    FILE   *keymapFile;
    char   line[256], c, *p, *bxSymbol, *guiSymbol;
    Bit32u bxKey, guiKey;
    Bit16u lineCount=0;

    if((keymapFile = fopen(filename,"r"))==NULL) {
      BX_PANIC(("Can not open keymap file '%s'.",filename));
      }
    
    BX_INFO(("Loading keymap from '%s'",filename));

    // Read keymap file one line at a time
    while(1) {
      fgets(line, sizeof(line)-1, keymapFile);
      lineCount++;

      // End of file
      if(feof(keymapFile)) break;

      // silently drop till end of line
      if( (p=strchr(line,'\n')) == NULL) {
        do {
          c = fgetc(keymapFile);
          } while((c!='\n')&&(c!=0));
        }
      else {
        *p = 0;
        }

      // drop comments
      p = strchr(line,'#');
      if (p!=NULL) *p = 0;

      // If string is empty
      p = line; while (*p!=0 && *p==' ') p++;
      if (*p==0) continue;

      // Parse line, find '=', split line
      p = strchr(line,'=');
      if (p==NULL) {
        BX_INFO(("line %d: Unknown format",lineCount));
        continue;
        }
      else {
        *p = 0;
        guiSymbol = p+1;
        }

      // Find first and last char of gui key symbol
      while (*guiSymbol==' ') guiSymbol++;
      p=guiSymbol; while (*p!=0 && *p!=' ') p++; *p=0;

      // Find first and last char of BX_KEY
      bxSymbol=line; while (*bxSymbol==' ') bxSymbol++;
      p=bxSymbol; while (*p!=0 && *p!=' ') p++; *p=0;

      // Here we have the two symbols
      bxKey = convertStringToBXKey(bxSymbol);
      guiKey = stringToSymbol(guiSymbol);
      
      // Check if data is valid
      if( bxKey==BX_KEYMAP_UNKNOWN ) {
        BX_INFO(("line %d: Ignoring unknown BX_KEY constant '%s'",lineCount,bxSymbol));
        continue;
        }

      if( guiKey==BX_KEYMAP_UNKNOWN ) {
        BX_INFO(("line %d: Ignoring unknown GUI constant '%s'",lineCount,guiSymbol));
        continue;
        }

      keymapTable=(KeyEntry*)realloc(keymapTable,(keymapCount+1) * sizeof(KeyEntry));
      
      if(keymapTable==NULL) 
        BX_PANIC(("Can not allocate memory for keymap table."));

      keymapTable[keymapCount].bxKey=bxKey;
      keymapTable[keymapCount].guiKey=guiKey;
      
      keymapCount++;
      }

    BX_INFO(("Loaded %d symbols",keymapCount));

    fclose(keymapFile);
}

    Bit32u
bx_keymap_c::convertStringToBXKey(const char* string)
{
    Bit16u i;

    // We look through the bx_key_symbol table to find the searched string
    for (i=0; i<BX_KEY_NBKEYS; i++) {
      if (strcmp(string,bx_key_symbol[i])==0) {
        return i;
        }
      }
  
    // Key is not known
    return BX_KEYMAP_UNKNOWN;
}

#include <X11/Xlib.h>

    Bit32u
bx_keymap_c::getBXKey(Bit32u key)
{
    Bit16u i;

    // We look through the keymap table to find the searched key
    for (i=0; i<keymapCount; i++) {
      if (keymapTable[i].guiKey == key) {
        return keymapTable[i].bxKey;
        }
      }

    // Return default
    return BX_KEY_UNHANDLED;
}

