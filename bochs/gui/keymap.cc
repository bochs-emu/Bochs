/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2013  The Bochs Project
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

/////////////////////////////////////////////////////////////////////////
//
// Todo
//  . Currently supported by sdl, wxGTK and x11. Check if other guis need mapping.
//  . Tables look-up should be optimised.
//

#include "param_names.h"
#include "bochs.h"
#include "keymap.h"

// Table of bochs "BX_KEY_*" symbols
// the table must be in BX_KEY_* order
const char *bx_key_symbol[BX_KEY_NBKEYS] = {
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
  "BX_KEY_WIN_L",          "BX_KEY_WIN_R",          "BX_KEY_MENU",
  "BX_KEY_ALT_SYSREQ",     "BX_KEY_CTRL_BREAK",     "BX_KEY_INT_BACK",
  "BX_KEY_INT_FORWARD",    "BX_KEY_INT_STOP",       "BX_KEY_INT_MAIL",
  "BX_KEY_INT_SEARCH",     "BX_KEY_INT_FAV",        "BX_KEY_INT_HOME",
  "BX_KEY_POWER_MYCOMP",   "BX_KEY_POWER_CALC",     "BX_KEY_POWER_SLEEP",
  "BX_KEY_POWER_POWER",    "BX_KEY_POWER_WAKE",
};

bx_keymap_c bx_keymap;

#define LOG_THIS bx_keymap.

bx_keymap_c::bx_keymap_c(void)
{
    put("KEYMAP");

    keymapCount = 0;
    keymapTable = (BXKeyEntry *)NULL;
}

bx_keymap_c::~bx_keymap_c(void)
{
    if(keymapTable != NULL) {
      free(keymapTable);
      keymapTable = (BXKeyEntry *)NULL;
    }
    keymapCount = 0;
}

void bx_keymap_c::loadKeymap(Bit32u stringToSymbol(const char*))
{
  if (SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
    loadKeymap(stringToSymbol, SIM->get_param_string(BXPN_KBD_KEYMAP)->getptr());
  }
}

bx_bool bx_keymap_c::isKeymapLoaded ()
{
  return (keymapCount > 0);
}


///////////////////
// I'll add these to the keymap object in a minute.
static unsigned char *lineptr = NULL;
static int lineCount;

static void init_parse()
{
  lineCount = 0;
}

static void init_parse_line(char *line_to_parse)
{
  // chop off newline
  lineptr = (unsigned char *)line_to_parse;
  char *nl;
  if ((nl = strchr(line_to_parse,'\n')) != NULL) {
    *nl = 0;
  }
}

static Bit32s get_next_word(char *output)
{
  char *copyp = output;
  // find first nonspace
  while (*lineptr && isspace(*lineptr))
    lineptr++;
  if (!*lineptr)
    return -1;  // nothing but spaces until end of line
  if (*lineptr == '#')
    return -1;  // nothing but a comment
  // copy nonspaces into the output
  while (*lineptr && !isspace(*lineptr))
    *copyp++ = *lineptr++;
  *copyp=0;  // null terminate the copy
  // there must be at least one nonspace, since that's why we stopped the
  // first loop!
  BX_ASSERT (copyp != output);
  return 0;
}

static Bit32s get_next_keymap_line (FILE *fp, char *bxsym, char *modsym, Bit32s *ascii, char *hostsym)
{
  char line[256];
  char buf[256];
  line[0] = 0;
  while (1) {
    lineCount++;
    if (!fgets(line, sizeof(line)-1, fp)) return -1;  // EOF
    init_parse_line (line);
    if (get_next_word (bxsym) >= 0) {
      modsym[0] = 0;
      char *p;
      if ((p = strchr (bxsym, '+')) != NULL) {
	*p = 0;  // truncate bxsym.
	p++;  // move one char beyond the +
	strcpy (modsym, p);  // copy the rest to modsym
      }
      if (get_next_word (buf) < 0) {
	BX_PANIC (("keymap line %d: expected 3 columns", lineCount));
	return -1;
      }
      if (buf[0] == '\'' && buf[2] == '\'' && buf[3]==0) {
	*ascii = (Bit8u) buf[1];
      } else if (!strcmp(buf, "space")) {
	*ascii = ' ';
      } else if (!strcmp(buf, "return")) {
	*ascii = '\n';
      } else if (!strcmp(buf, "tab")) {
	*ascii = '\t';
      } else if (!strcmp(buf, "backslash")) {
	*ascii = '\\';
      } else if (!strcmp(buf, "apostrophe")) {
	*ascii = '\'';
      } else if (!strcmp(buf, "none")) {
	*ascii = -1;
      } else {
	BX_PANIC (("keymap line %d: ascii equivalent is \"%s\" but it must be char constant like 'x', or one of space,tab,return,none", lineCount, buf));
      }
      if (get_next_word (hostsym) < 0) {
        BX_PANIC (("keymap line %d: expected 3 columns", lineCount));
	return -1;
      }
      return 0;
    }
    // no words on this line, keep reading.
  }
}

void bx_keymap_c::loadKeymap(Bit32u stringToSymbol(const char*), const char* filename)
{
  FILE   *keymapFile;
  char baseSym[256], modSym[256], hostSym[256];
  Bit32s ascii = 0;
  Bit32u baseKey, modKey, hostKey;
  struct stat status;

  if (stat(filename, &status)) {
    BX_PANIC(("Can not stat keymap file '%s'.",filename));
  }

  if (!(S_ISREG(status.st_mode))) {
    BX_PANIC(("Keymap file '%s' is not a file",filename));
  }

  if((keymapFile = fopen(filename,"r"))==NULL) {
    BX_PANIC(("Can not open keymap file '%s'.",filename));
  }

  BX_INFO(("Loading keymap from '%s'",filename));
  init_parse();

  // Read keymap file one line at a time
  while(1) {
    if (get_next_keymap_line (keymapFile,
          baseSym, modSym, &ascii, hostSym) < 0) { break; }

    // convert X_KEY_* symbols to values
    baseKey = convertStringToBXKey(baseSym);
    modKey = convertStringToBXKey(modSym);
    hostKey = 0;
    if (stringToSymbol != NULL)
      hostKey = stringToSymbol(hostSym);

    BX_DEBUG(("baseKey='%s' (%d), modSym='%s' (%d), ascii=%d, guisym='%s' (%d)", baseSym, baseKey, modSym, modKey, ascii, hostSym, hostKey));

    // Check if data is valid
    if (baseKey==BX_KEYMAP_UNKNOWN) {
      BX_PANIC (("line %d: unknown BX_KEY constant '%s'",lineCount,baseSym));
      continue;
    }

    if (hostKey==BX_KEYMAP_UNKNOWN) {
      BX_PANIC (("line %d: unknown host key name '%s' (wrong keymap ?)",lineCount,hostSym));
      continue;
    }

    keymapTable=(BXKeyEntry*)realloc(keymapTable,(keymapCount+1) * sizeof(BXKeyEntry));

    if (keymapTable==NULL)
      BX_PANIC(("Can not allocate memory for keymap table."));

    keymapTable[keymapCount].baseKey=baseKey;
    keymapTable[keymapCount].modKey=modKey;
    keymapTable[keymapCount].ascii=ascii;
    keymapTable[keymapCount].hostKey=hostKey;

    keymapCount++;
  }

  BX_INFO(("Loaded %d symbols",keymapCount));

  fclose(keymapFile);
}

Bit32u bx_keymap_c::convertStringToBXKey(const char* string)
{
  // We look through the bx_key_symbol table to find the searched string
  for (Bit16u i=0; i<BX_KEY_NBKEYS; i++) {
    if (strcmp(string,bx_key_symbol[i])==0) {
      return i;
    }
  }

  // Key is not known
  return BX_KEYMAP_UNKNOWN;
}

BXKeyEntry *bx_keymap_c::findHostKey(Bit32u key)
{
  // We look through the keymap table to find the searched key
  for (Bit16u i=0; i<keymapCount; i++) {
    if (keymapTable[i].hostKey == key) {
      BX_DEBUG (("key 0x%02x matches hostKey for entry #%d", key, i));
      return &keymapTable[i];
    }
  }
  BX_DEBUG(("key %02x matches no entries", key));

  // Return default
  return NULL;
}

BXKeyEntry *bx_keymap_c::findAsciiChar(Bit8u ch)
{
  BX_DEBUG (("findAsciiChar (0x%02x)", ch));

  // We look through the keymap table to find the searched key
  for (Bit16u i=0; i<keymapCount; i++) {
    if (keymapTable[i].ascii == ch) {
      BX_DEBUG (("key %02x matches ascii for entry #%d", ch, i));
      return &keymapTable[i];
    }
  }
  BX_DEBUG (("key 0x%02x matches no entries", ch));

  // Return default
  return NULL;
}

const char *bx_keymap_c::getBXKeyName(Bit32u key)
{
  return bx_key_symbol[key & 0x7fffffff];
}
