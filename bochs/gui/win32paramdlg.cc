/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2014  Volker Ruppert
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

#include "win32dialog.h"

#if BX_USE_WIN32CONFIG

#include "bochs.h"
#include "win32res.h"
#include "scrollwin.h"

#define ID_LABEL 1000
#define ID_PARAM 1500
#define ID_BROWSE 2000
#define ID_UPDOWN 2500

// helper function

char *backslashes(char *s)
{
  if (s != NULL) {
    while (*s != 0) {
       if (*s == '/') *s = '\\';
       s++;
    }
  }
  return s;
}

// dialog item list code

typedef struct _dlg_list_t {
  bx_list_c *list;
  UINT dlg_list_id;
  UINT dlg_base_id;
  struct _dlg_list_t *next;
} dlg_list_t;

HFONT DlgFont;
WNDPROC DefEditWndProc;
UINT  nextDlgID;
dlg_list_t *dlg_lists = NULL;


bx_bool registerDlgList(UINT lid, bx_list_c *list)
{
  dlg_list_t *dlg_list;
  int items;

  dlg_list = (dlg_list_t *)malloc(sizeof(dlg_list_t));
  if (dlg_list == NULL) {
    return 0;
  }

  dlg_list->list = list;
  dlg_list->dlg_list_id = lid;
  dlg_list->dlg_base_id = nextDlgID;
  items = list->get_size();
  nextDlgID += items;
  dlg_list->next = NULL;

  if (dlg_lists == NULL) {
    dlg_lists = dlg_list;
  } else {
    dlg_list_t *temp = dlg_lists;

    while (temp->next) {
      if (temp->list == dlg_list->list) {
        free(dlg_list);
        return 0;
      }
      temp = temp->next;
    }
    temp->next = dlg_list;
  }
  return dlg_list->dlg_base_id;
}

UINT findDlgListBaseID(bx_list_c *list)
{
  dlg_list_t *dlg_list;

  for (dlg_list = dlg_lists; dlg_list; dlg_list = dlg_list->next) {
    if (list == dlg_list->list) {
      return dlg_list->dlg_base_id;
    }
  }
  return 0;
}

bx_param_c *findParamFromDlgID(UINT cid)
{
  dlg_list_t *dlg_list;
  int i;

  for (dlg_list = dlg_lists; dlg_list; dlg_list = dlg_list->next) {
    if ((cid >= dlg_list->dlg_base_id) && (cid < (dlg_list->dlg_base_id + dlg_list->list->get_size()))) {
      i = cid - dlg_list->dlg_base_id;
      return dlg_list->list->get(i);
    }
    if (cid == dlg_list->dlg_list_id) {
      return dlg_list->list;
    }
  }
  return NULL;
}

UINT findDlgIDFromParam(bx_param_c *param)
{
  dlg_list_t *dlg_list;
  bx_list_c *list;
  UINT cid;
  int i;

  list = (bx_list_c*)param->get_parent();
  for (dlg_list = dlg_lists; dlg_list; dlg_list = dlg_list->next) {
    if (list == dlg_list->list) {
      cid = dlg_list->dlg_base_id;
      for (i = 0; i < list->get_size(); i++) {
        if (param == list->get(i)) {
          return (cid + i);
        }
      }
    }
  }
  // second try: browse all lists (for runtime-only dialogs)
  for (dlg_list = dlg_lists; dlg_list; dlg_list = dlg_list->next) {
    list = dlg_list->list;
    cid = dlg_list->dlg_base_id;
    for (i = 0; i < list->get_size(); i++) {
      if (param == list->get(i)) {
        return (cid + i);
      }
    }
  }
  return 0;
}

void cleanupDlgLists()
{
  dlg_list_t *d, *next;

  if (dlg_lists) {
    d = dlg_lists;
    while (d != NULL) {
      next = d->next;
      free(d);
      d = next;
    }
    dlg_lists = NULL;
  }
}

// tooltips code

HWND hwndTT, tt_hwndDlg;
HHOOK tt_hhk;
const char *tt_text;

BOOL CALLBACK EnumChildProc(HWND hwndCtrl, LPARAM lParam);
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);

BOOL CreateParamDlgTooltip(HWND hwndDlg)
{
  InitCommonControls();
  tt_hwndDlg = hwndDlg;
  hwndTT = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR) NULL,
    TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    CW_USEDEFAULT, tt_hwndDlg, (HMENU) NULL, NULL, NULL);

  if (hwndTT == NULL)
    return FALSE;

  if (!EnumChildWindows(tt_hwndDlg, (WNDENUMPROC) EnumChildProc, 0))
    return FALSE;

  tt_hhk = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc,
    (HINSTANCE) NULL, GetCurrentThreadId());

  if (tt_hhk == (HHOOK) NULL)
    return FALSE;

  return TRUE;
}

BOOL CALLBACK EnumChildProc(HWND hwndCtrl, LPARAM lParam)
{
  TOOLINFO ti;
  char szClass[64];

  GetClassName(hwndCtrl, szClass, sizeof(szClass));
  if (lstrcmp(szClass, "STATIC")) {
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_IDISHWND;

    ti.hwnd = tt_hwndDlg;
    ti.uId = (UINT_PTR) hwndCtrl;
    ti.hinst = 0;
    ti.lpszText = LPSTR_TEXTCALLBACK;
    SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
  }
  return TRUE;
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  MSG *lpmsg;

  lpmsg = (MSG *) lParam;
  if (nCode < 0 || !(IsChild(tt_hwndDlg, lpmsg->hwnd)))
    return (CallNextHookEx(tt_hhk, nCode, wParam, lParam));

  switch (lpmsg->message) {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
      if (hwndTT != NULL) {
        MSG msg;

        msg.lParam = lpmsg->lParam;
        msg.wParam = lpmsg->wParam;
        msg.message = lpmsg->message;
        msg.hwnd = lpmsg->hwnd;
        SendMessage(hwndTT, TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &msg);
      }
      break;
    default:
      break;
  }
  return (CallNextHookEx(tt_hhk, nCode, wParam, lParam));
}

VOID OnWMNotify(LPARAM lParam)
{
  LPTOOLTIPTEXT lpttt;
  int idCtrl;

  if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT) {
    idCtrl = GetDlgCtrlID((HWND) ((LPNMHDR) lParam)->idFrom);
    lpttt = (LPTOOLTIPTEXT) lParam;

    if ((idCtrl >= ID_PARAM) && (idCtrl < ID_BROWSE)) {
      bx_param_c *param = findParamFromDlgID(idCtrl - ID_PARAM);
      if (param != NULL) {
        tt_text = param->get_description();
        if (lstrlen(tt_text) > 0) {
          lpttt->lpszText = (LPSTR)tt_text;
        }
      }
    }
  }
}

// gui functions

int AskFilename(HWND hwnd, bx_param_filename_c *param, char *buffer)
{
  OPENFILENAME ofn;
  int ret;
  DWORD errcode;
  char filename[BX_PATHNAME_LEN];
  const char *title, *ext;
  char errtext[80];

  if (buffer != NULL) {
    lstrcpyn(filename, buffer, BX_PATHNAME_LEN);
  } else {
    param->get(filename, BX_PATHNAME_LEN);
  }
  // common file dialogs don't accept raw device names
  if ((isalpha(filename[0])) && (filename[1] == ':') && (strlen(filename) == 2)) {
    filename[0] = 0;
  }
  title = param->get_label();
  ext = param->get_extension();
  if (!title) title = param->get_name();
  memset(&ofn, 0, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile   = filename;
  ofn.nMaxFile    = MAX_PATH;
  ofn.lpstrInitialDir = bx_startup_flags.initial_dir;
  ofn.lpstrTitle = title;
  ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
  ofn.lpstrDefExt = ext;
  if (!lstrcmp(ext, "bxrc")) {
    ofn.lpstrFilter = "Bochs config files (*.bxrc)\0*.bxrc\0All files (*.*)\0*.*\0";
  } else if (!lstrcmp(ext, "img")) {
    ofn.lpstrFilter = "Disk image files (*.img)\0*.img\0All files (*.*)\0*.*\0";
  } else if (!lstrcmp(ext, "iso")) {
    ofn.lpstrFilter = "CD-ROM image files (*.iso)\0*.iso\0All files (*.*)\0*.*\0";
  } else if (!lstrcmp(ext, "log")) {
    ofn.lpstrFilter = "Log files (*.log)\0*.log\0All files (*.*)\0*.*\0";
  } else if (!lstrcmp(ext, "map")) {
    ofn.lpstrFilter = "Keymap files (*.map)\0*.map\0All files (*.*)\0*.*\0";
  } else if (!lstrcmp(ext, "txt")) {
    ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
  } else if (!lstrcmp(ext, "bmp")) {
    ofn.lpstrFilter = "Windows bitmap files (*.bmp)\0*.bmp\0All files (*.*)\0*.*\0";
  } else {
    ofn.lpstrFilter = "All files (*.*)\0*.*\0";
  }
  if (param->get_options() & param->SAVE_FILE_DIALOG) {
    ofn.Flags |= OFN_OVERWRITEPROMPT;
    ret = GetSaveFileName(&ofn);
  } else {
    ofn.Flags |= OFN_FILEMUSTEXIST;
    ret = GetOpenFileName(&ofn);
  }
  if (buffer != NULL) {
    lstrcpyn(buffer, filename, BX_PATHNAME_LEN);
  } else {
    param->set(filename);
  }
  if (ret == 0) {
    errcode = CommDlgExtendedError();
    if (errcode == 0) {
      ret = -1;
    } else {
      if (errcode == 0x3002) {
        wsprintf(errtext, "CommDlgExtendedError: invalid filename");
      } else {
        wsprintf(errtext, "CommDlgExtendedError returns 0x%04x", errcode);
      }
      MessageBox(hwnd, errtext, "Error", MB_ICONERROR);
    }
  }
  return ret;
}

void InitDlgFont(void)
{
  LOGFONT LFont;

  LFont.lfHeight         = 8;                // Default logical height of font
  LFont.lfWidth          = 0;                // Default logical average character width
  LFont.lfEscapement     = 0;                // angle of escapement
  LFont.lfOrientation    = 0;                // base-line orientation angle
  LFont.lfWeight         = FW_NORMAL;        // font weight
  LFont.lfItalic         = 0;                // italic attribute flag
  LFont.lfUnderline      = 0;                // underline attribute flag
  LFont.lfStrikeOut      = 0;                // strikeout attribute flag
  LFont.lfCharSet        = DEFAULT_CHARSET;  // character set identifier
  LFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;  // output precision
  LFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS; // clipping precision
  LFont.lfQuality        = DEFAULT_QUALITY;     // output quality
  LFont.lfPitchAndFamily = DEFAULT_PITCH;     // pitch and family
  lstrcpy(LFont.lfFaceName, "Helv");          // pointer to typeface name string
  DlgFont  = CreateFontIndirect(&LFont);
}

LRESULT CALLBACK EditHexWndProc(HWND Window, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (msg == WM_CHAR) {
    switch (wParam) {
      case 0x08:
        break;
      case 'x':
        break;
      default:
        if ((wParam < '0') || ((wParam > '9') && (wParam < 'A')) ||
            ((wParam > 'F') && (wParam < 'a')) || (wParam > 'f')) {
          MessageBeep(MB_OK);
          return 0;
        }
    }
  }
  return CallWindowProc(DefEditWndProc, Window, msg, wParam, lParam);
}

BOOL IsScrollWindow(HWND hwnd)
{
  char classname[80];

  GetClassName(hwnd, classname, 80);
  return (!lstrcmp(classname, "ScrollWin"));
}

HWND CreateLabel(HWND hDlg, UINT cid, UINT xpos, UINT ypos, UINT width, BOOL hide, const char *text)
{
  HWND Label;
  RECT r;
  int code;

  code = ID_LABEL + cid;
  r.left = xpos;
  r.top = ypos + 2;
  r.right = r.left + width;
  r.bottom = r.top + 16;
  if (IsScrollWindow(hDlg)) {
    MapDialogRect(GetParent(hDlg), &r);
  } else {
    MapDialogRect(hDlg, &r);
  }
  Label = CreateWindow("STATIC", text, WS_CHILD, r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  SendMessage(Label, WM_SETFONT, (WPARAM)DlgFont, TRUE);
  ShowWindow(Label, hide ? SW_HIDE : SW_SHOW);
  return Label;
}

HWND CreateGroupbox(HWND hDlg, UINT cid, UINT xpos, UINT ypos, SIZE size, BOOL hide, bx_list_c *list)
{
  HWND Groupbox;
  RECT r;
  int code;
  const char *title = NULL;

  code = ID_PARAM + cid;
  r.left = xpos;
  r.top = ypos;
  r.right = r.left + size.cx;
  r.bottom = r.top + size.cy;
  MapDialogRect(hDlg, &r);
  if (list->get_options() & list->USE_BOX_TITLE) {
    title = list->get_title();
  }
  Groupbox = CreateWindow("BUTTON", title, BS_GROUPBOX | WS_CHILD, r.left, r.top,
                          r.right-r.left+1, r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  SendMessage(Groupbox, WM_SETFONT, (WPARAM)DlgFont, TRUE);
  ShowWindow(Groupbox, hide ? SW_HIDE : SW_SHOW);
  return Groupbox;
}

HWND CreateTabControl(HWND hDlg, UINT cid, UINT xpos, UINT ypos, SIZE size, BOOL hide, bx_list_c *list)
{
  HWND TabControl;
  TC_ITEM tie;
  RECT r;
  int code, i;
  bx_param_c *item;

  code = ID_PARAM + cid;
  r.left = xpos;
  r.top = ypos;
  r.right = r.left + size.cx;
  r.bottom = r.top + size.cy;
  MapDialogRect(hDlg, &r);
  TabControl = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                            r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  for (i = 0; i < list->get_size(); i++) {
    item = list->get(i);
    if (item->get_type() == BXT_LIST) {
      tie.mask = TCIF_TEXT;
      tie.pszText = ((bx_list_c*)item)->get_title();
      TabCtrl_InsertItem(TabControl, i, &tie);
    }
  }
  TabCtrl_SetCurSel(TabControl, 0);
  SendMessage(TabControl, WM_SETFONT, (WPARAM)DlgFont, TRUE);
  ShowWindow(TabControl, hide ? SW_HIDE : SW_SHOW);
  return TabControl;
}


HWND CreateButton(HWND hDlg, UINT id, UINT xpos, UINT ypos, BOOL hide, const char *text)
{
  HWND Button;
  RECT r;

  r.left = xpos;
  r.top = ypos;
  r.right = r.left + 50;
  r.bottom = r.top + 14;
  if (IsScrollWindow(hDlg)) {
    MapDialogRect(GetParent(hDlg), &r);
  } else {
    MapDialogRect(hDlg, &r);
  }
  Button = CreateWindow("BUTTON", text, WS_CHILD, r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, hDlg, (HMENU)id, NULL, NULL);
  SendMessage(Button, WM_SETFONT, (WPARAM)DlgFont, TRUE);
  ShowWindow(Button, hide ? SW_HIDE : SW_SHOW);
  return Button;
}

HWND CreateBrowseButton(HWND hDlg, UINT cid, UINT xpos, UINT ypos, BOOL hide)
{
  return CreateButton(hDlg, ID_BROWSE + cid, xpos, ypos, hide, "Browse...");
}

HWND CreateCheckbox(HWND hDlg, UINT cid, UINT xpos, UINT ypos, BOOL hide, bx_param_bool_c *bparam)
{
  HWND Checkbox;
  RECT r;
  int code, val;

  code = ID_PARAM + cid;
  r.left = xpos;
  r.top = ypos;
  r.right = r.left + 20;
  r.bottom = r.top + 14;
  if (IsScrollWindow(hDlg)) {
    MapDialogRect(GetParent(hDlg), &r);
  } else {
    MapDialogRect(hDlg, &r);
  }
  Checkbox = CreateWindow("BUTTON", "", BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP,
                          r.left, r.top, r.right-r.left+1, r.bottom-r.top+1,
                          hDlg, (HMENU)code, NULL, NULL);
  val = bparam->get();
  SendMessage(Checkbox, BM_SETCHECK, val ? BST_CHECKED : BST_UNCHECKED, 0);
  SendMessage(Checkbox, WM_SETFONT, (WPARAM)DlgFont, TRUE);
  ShowWindow(Checkbox, hide ? SW_HIDE : SW_SHOW);
  return Checkbox;
}

HWND CreateInput(HWND hDlg, UINT cid, UINT xpos, UINT ypos, BOOL hide, bx_param_c *param)
{
  HWND Input, Updown;
  RECT r;
  int code, style;
  bx_param_num_c *nparam = NULL;
  bx_param_string_c *sparam;
  char buffer[512];
  BOOL spinctrl = FALSE, hexedit = FALSE;

  code = ID_PARAM + cid;
  style = WS_CHILD | WS_TABSTOP;
  if (param->get_type() == BXT_PARAM_STRING) {
    sparam = (bx_param_string_c*)param;
    sparam->sprint(buffer, 512, 0);
    if ((sparam->get_options() & sparam->RAW_BYTES) == 0) {
      style |= ES_AUTOHSCROLL;
    }
  } else {
    nparam = (bx_param_num_c*)param;
    if (nparam->get_base() == BASE_HEX) {
      wsprintf(buffer, "0x%x", nparam->get());
      hexedit = TRUE;
    } else {
      wsprintf(buffer, "%d", nparam->get());
      style |= ES_NUMBER;
    }
    if (nparam->get_options() & nparam->USE_SPIN_CONTROL) {
      spinctrl = TRUE;
    }
  }
  r.left = xpos;
  r.top = ypos;
  r.right = r.left + 100;
  r.bottom = r.top + 14;
  if (IsScrollWindow(hDlg)) {
    MapDialogRect(GetParent(hDlg), &r);
  } else {
    MapDialogRect(hDlg, &r);
  }
  Input = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY, "EDIT", buffer,
                         style, r.left, r.top, r.right-r.left+1,
                         r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  if (hexedit) {
    DefEditWndProc = (WNDPROC)SetWindowLongPtr(Input, GWLP_WNDPROC, (LONG_PTR)EditHexWndProc);
  }
  if (spinctrl) {
    style = WS_CHILD | WS_BORDER | WS_VISIBLE | UDS_NOTHOUSANDS | UDS_ARROWKEYS |
            UDS_ALIGNRIGHT | UDS_SETBUDDYINT;
    Updown = CreateUpDownControl(style, 0, 0, 0, 0, hDlg, ID_UPDOWN + cid, NULL, Input,
                                 (int)nparam->get_max(), (int)nparam->get_min(), (int)nparam->get());
    ShowWindow(Updown, hide ? SW_HIDE : SW_SHOW);
  }
  SendMessage(Input, WM_SETFONT, (WPARAM)DlgFont, TRUE);
  ShowWindow(Input, hide ? SW_HIDE : SW_SHOW);
  return Input;
}

HWND CreateCombobox(HWND hDlg, UINT cid, UINT xpos, UINT ypos, BOOL hide, bx_param_enum_c *eparam)
{
  HWND Combo;
  RECT r;
  int code, j;
  const char *choice;

  code = ID_PARAM + cid;
  r.left = xpos;
  r.top = ypos;
  r.right = r.left + 100;
  r.bottom = r.top + 14 * ((int)(eparam->get_max() - eparam->get_min()) + 1);
  if (IsScrollWindow(hDlg)) {
    MapDialogRect(GetParent(hDlg), &r);
  } else {
    MapDialogRect(hDlg, &r);
  }
  Combo = CreateWindow("COMBOBOX", "", WS_CHILD | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST,
                       r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  j = 0;
  while(1) {
    choice = eparam->get_choice(j);
    if (choice == NULL) break;
    SendMessage(Combo, CB_ADDSTRING, 0, (LPARAM)choice);
    j++;
  }
  SendMessage(Combo, CB_SETCURSEL, (WPARAM)(eparam->get()-eparam->get_min()), 0);
  SendMessage(Combo, WM_SETFONT, (WPARAM)DlgFont, TRUE);
  ShowWindow(Combo, hide ? SW_HIDE : SW_SHOW);
  return Combo;
}


HWND CreateScrollWindow(HWND hDlg, int xpos, int ypos, int width, int height)
{
  RECT r;
  HWND scrollwin;

  r.left = xpos;
  r.top = ypos;
  r.right = r.left + width;
  r.bottom = r.top + height;
  MapDialogRect(hDlg, &r);
  scrollwin = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_CONTROLPARENT, "ScrollWin",
                             "scrollwin", WS_CHILD | WS_CLIPCHILDREN | WS_TABSTOP | WS_VSCROLL,
                             r.left, r.top, r.right-r.left+1, r.bottom-r.top+1,
                             hDlg, NULL, NULL, NULL);
  ShowWindow(scrollwin, SW_SHOW);
  return scrollwin;
}

void EnableParam(HWND hDlg, UINT cid, bx_param_c *param, BOOL val)
{
  HWND Button, Updown;

  if (cid == 0) {
    cid = findDlgIDFromParam(param);
  }
  if (param->get_type() != BXT_LIST) {
    bx_list_c *list = (bx_list_c*)param->get_parent();
    if (list->get_options() & list->USE_SCROLL_WINDOW) {
      hDlg = FindWindowEx(hDlg, NULL, "ScrollWin", NULL);
    }
    EnableWindow(GetDlgItem(hDlg, ID_LABEL + cid), val);
    EnableWindow(GetDlgItem(hDlg, ID_PARAM + cid), val);
    Button = GetDlgItem(hDlg, ID_BROWSE + cid);
    if (Button != NULL) {
      EnableWindow(Button, val);
    }
    Updown = GetDlgItem(hDlg, ID_UPDOWN + cid);
    if (Updown != NULL) {
      EnableWindow(Updown, val);
    }
  }
}

UINT GetLabelText(bx_param_c *param, bx_list_c *list, char *buffer)
{
  const char *label, *tmpstr;
  char tmpbuf[512];
  int len;

  label = param->get_label();
  if (label == NULL) {
    label = param->get_name();
  }
  if ((list->get_options() & list->SHOW_GROUP_NAME) && (param->get_group() != NULL)) {
    wsprintf(tmpbuf, "%s %s", param->get_group(), label);
  } else {
    lstrcpyn(tmpbuf, label, 512);
  }
  if (buffer != NULL) {
    lstrcpy(buffer, tmpbuf);
  }
  len = lstrlen(tmpbuf);
  tmpstr = strchr(tmpbuf, 10);
  if (tmpstr != NULL) {
    if (lstrlen(tmpstr) > (len / 2)) {
      len = lstrlen(tmpstr) - 1;
    } else {
      len -= lstrlen(tmpstr);
    }
  }
  return (len * 4);
}

SIZE CreateParamList(HWND hDlg, UINT lid, UINT xpos, UINT ypos, BOOL hide, bx_list_c *list)
{
  SIZE size, lsize;
  bx_param_c *param;
  bx_param_string_c *sparam;
  char buffer[512];
  int options;
  UINT cid, i, items, lw, w1, x0, x1, x2, y;
  BOOL ihide;
  HWND scrollwin = NULL, hParent;
  RECT vrect;

  items = list->get_size();
  options = list->get_options();
  cid = registerDlgList(lid, list);
  x0 = xpos + 5;
  size.cx = 195;
  if (options & list->USE_TAB_WINDOW) {
    y = ypos + 15;
    size.cy = 18;
  } else if (options & list->USE_SCROLL_WINDOW) {
    x0 = 5;
    y = 5;
    size.cy = 3;
  } else {
    y = ypos + 10;
    size.cy = 13;
  }
  // find out longest label text
  w1 = 78;
  for (i = 0; i < items; i++) {
    param = list->get(i);
    if (!SIM->get_init_done() || param->get_runtime_param()) {
      if (param->get_type() != BXT_LIST) {
        lw = GetLabelText(param, list, NULL);
        if (lw > w1) {
          w1 = lw;
        }
      }
    }
  }
  // set columns and width
  x1 = x0 + w1 + 2;
  x2 = x1 + 110;
  if (size.cx < (int)(x2 + 5)) {
    size.cx = x2 + 5;
  }
  if ((items > 16) && (options & list->USE_SCROLL_WINDOW)) {
    size.cx += 20;
    scrollwin = CreateScrollWindow(hDlg, xpos, ypos, size.cx, 325);
    vrect.left = vrect.top = 0;
    vrect.right = size.cx;
    vrect.bottom = size.cy;
    hParent = scrollwin;
  } else {
    hParent = hDlg;
  }
  // create controls
  for (i = 0; i < items; i++) {
    param = list->get(i);
    if (!SIM->get_init_done() || param->get_runtime_param()) {
      ihide = hide || ((i != 0) && (options & list->USE_TAB_WINDOW));
      if (param->get_type() == BXT_LIST) {
        lsize = CreateParamList(hDlg, cid, x0 + 4, y + 1, ihide, (bx_list_c*)param);
        if ((lsize.cx + 18) > size.cx) {
          size.cx = lsize.cx + 18;
        }
        if (!(options & list->USE_TAB_WINDOW)) {
          y += (lsize.cy + 6);
          size.cy += (lsize.cy + 6);
        } else {
          if ((lsize.cy + 24) > size.cy) {
            size.cy = lsize.cy + 24;
          }
        }
      } else {
        lw = GetLabelText(param, list, buffer);
        CreateLabel(hParent, cid, x0, y, w1, hide, buffer);
        if (param->get_type() == BXT_PARAM_BOOL) {
          CreateCheckbox(hParent, cid, x1, y, hide, (bx_param_bool_c*)param);
        } else if (param->get_type() == BXT_PARAM_ENUM) {
          CreateCombobox(hParent, cid, x1, y, hide, (bx_param_enum_c*)param);
        } else if (param->get_type() == BXT_PARAM_NUM) {
          CreateInput(hParent, cid, x1, y, hide, param);
        } else if (param->get_type() == BXT_PARAM_STRING) {
          CreateInput(hParent, cid, x1, y, hide, param);
          sparam = (bx_param_string_c*)param;
          if (sparam->get_options() & sparam->IS_FILENAME) {
            CreateBrowseButton(hParent, cid, x2, y, hide);
            if (size.cx < (int)(x2 + 60)) {
              size.cx = x2 + 60;
            }
          }
        }
        if (!param->get_enabled()) {
          EnableParam(hDlg, cid, param, FALSE);
        }
        y += 20;
        if ((scrollwin == NULL) || (i < 16)) {
          size.cy += 20;
        }
        if (scrollwin != NULL) {
          vrect.bottom += 20;
        }
      }
    }
    cid++;
  }
  if (options & list->USE_TAB_WINDOW) {
    CreateTabControl(hDlg, lid, xpos, ypos, size, hide, list);
  } else if (scrollwin != NULL) {
    MapDialogRect(hDlg, &vrect);
    SendMessage(scrollwin, WM_USER, 0x1234, vrect.bottom);
  } else {
    CreateGroupbox(hDlg, lid, xpos, ypos, size, hide, list);
  }
  return size;
}

void SetParamList(HWND hDlg, bx_list_c *list)
{
  bx_param_c *param;
  bx_param_num_c *nparam;
  bx_param_enum_c *eparam;
  bx_param_bool_c *bparam;
  bx_param_string_c *sparam;
  int j;
  LRESULT val;
  const char *src;
  char buffer[512], rawbuf[512];
  UINT cid, i, items, lid;

  lid = findDlgListBaseID(list);
  items = list->get_size();
  if (list->get_options() & list->USE_SCROLL_WINDOW) {
    hDlg = FindWindowEx(hDlg, NULL, "ScrollWin", NULL);
  }
  for (i = 0; i < items; i++) {
    cid = lid + i;
    param = list->get(i);
    if (param->get_enabled() && (!SIM->get_init_done() || (SIM->get_init_done() && param->get_runtime_param()))) {
      if (param->get_type() == BXT_LIST) {
        SetParamList(hDlg, (bx_list_c*)param);
      } else if (param->get_type() == BXT_PARAM_BOOL) {
        val = (SendMessage(GetDlgItem(hDlg, ID_PARAM + cid), BM_GETCHECK, 0, 0) == BST_CHECKED);
        bparam = (bx_param_bool_c*)param;
        if (val != bparam->get()) {
          bparam->set(val);
        }
      } else if (param->get_type() == BXT_PARAM_ENUM) {
        eparam = (bx_param_enum_c*)param;
        val = (LRESULT)(SendMessage(GetDlgItem(hDlg, ID_PARAM + cid), CB_GETCURSEL, 0, 0) + eparam->get_min());
        if (val != eparam->get()) {
          eparam->set(val);
        }
      } else {
        if (SendMessage(GetDlgItem(hDlg, ID_PARAM + cid), EM_GETMODIFY, 0, 0)) {
          if (param->get_type() == BXT_PARAM_NUM) {
            nparam = (bx_param_num_c*)param;
            if (nparam->get_base() == BASE_HEX) {
              GetWindowText(GetDlgItem(hDlg, ID_PARAM + cid), buffer, 511);
              sscanf(buffer, "%x", &val);
            } else {
              val = GetDlgItemInt(hDlg, ID_PARAM + cid, NULL, FALSE);
            }
            nparam->set(val);
          } else if (param->get_type() == BXT_PARAM_STRING) {
            GetWindowText(GetDlgItem(hDlg, ID_PARAM + cid), buffer, 511);
            sparam = (bx_param_string_c*)param;
            if (sparam->get_options() & sparam->RAW_BYTES) {
              src = &buffer[0];
              memset(rawbuf, 0, sparam->get_maxsize());
              for (j = 0; j < sparam->get_maxsize(); j++) {
                while (*src == sparam->get_separator())
                  src++;
                if (*src == 0) break;
                if (sscanf(src, "%02x", &val)) {
                  rawbuf[j] = (char) val;
                  src += 2;
                } else {
                  break;
                }
              }
              sparam->set(rawbuf);
            } else {
              sparam->set(buffer);
            }
          }
        }
      }
    }
    if ((i + 1) >= (UINT)list->get_size()) break;
  }
}

void ShowParamList(HWND hDlg, UINT lid, BOOL show, bx_list_c *list)
{
  UINT cid;
  int i;
  HWND Button, Updown;
  BOOL ishow;

  ShowWindow(GetDlgItem(hDlg, ID_PARAM + lid), show ? SW_SHOW : SW_HIDE);
  cid = findDlgListBaseID(list);
  for (i = 0; i < list->get_size(); i++) {
    if (list->get(i)->get_type() == BXT_LIST) {
      ishow = show;
      if (list->get_options() & list->USE_TAB_WINDOW) {
        ishow &= (i == 0);
      }
      ShowParamList(hDlg, cid + i, ishow, (bx_list_c*)list->get(i));
    } else {
      ShowWindow(GetDlgItem(hDlg, ID_LABEL + cid + i), show ? SW_SHOW : SW_HIDE);
      ShowWindow(GetDlgItem(hDlg, ID_PARAM + cid + i), show ? SW_SHOW : SW_HIDE);
      Button = GetDlgItem(hDlg, ID_BROWSE + cid + i);
      if (Button != NULL) {
        ShowWindow(Button, show ? SW_SHOW : SW_HIDE);
      }
      Updown = GetDlgItem(hDlg, ID_UPDOWN + cid + i);
      if (Updown != NULL) {
        ShowWindow(Updown, show ? SW_SHOW : SW_HIDE);
      }
    }
  }
}

void ProcessDependentList(HWND hDlg, bx_param_c *param, BOOL enabled)
{
  UINT cid;
  bx_list_c *deplist;
  bx_param_c *dparam;
  bx_param_enum_c *eparam;
  Bit64s value;
  Bit64u enable_bitmap, mask;
  char buffer[BX_PATHNAME_LEN];
  int i;
  BOOL en;

  deplist = param->get_dependent_list();
  if (deplist != NULL) {
    cid = findDlgIDFromParam(param);
    if (param->get_type() == BXT_PARAM_ENUM) {
      eparam = (bx_param_enum_c*)param;
      value = SendMessage(GetDlgItem(hDlg, ID_PARAM + cid), CB_GETCURSEL, 0, 0);
      enable_bitmap = eparam->get_dependent_bitmap(value + eparam->get_min());
      mask = 0x1;
      for (i = 0; i < deplist->get_size(); i++) {
        dparam = deplist->get(i);
        if (dparam != param) {
          en = (enable_bitmap & mask) && enabled;
          cid = findDlgIDFromParam(dparam);
          if (cid != 0) {
            if (en != IsWindowEnabled(GetDlgItem(hDlg, ID_PARAM + cid))) {
              ProcessDependentList(hDlg, dparam, en);
              EnableParam(hDlg, 0, dparam, en);
            }
          }
        }
        mask <<= 1;
      }
    } else if ((param->get_type() == BXT_PARAM_BOOL) ||
               (param->get_type() == BXT_PARAM_NUM) ||
               (param->get_type() == BXT_PARAM_STRING)) {
      if (param->get_type() == BXT_PARAM_BOOL) {
        value = SendMessage(GetDlgItem(hDlg, ID_PARAM + cid), BM_GETCHECK, 0, 0);
      } else if (param->get_type() == BXT_PARAM_NUM) {
        value = GetDlgItemInt(hDlg, ID_PARAM + cid, NULL, FALSE);
      } else {
        GetWindowText(GetDlgItem(hDlg, ID_PARAM + cid), buffer, BX_PATHNAME_LEN);
        value = (lstrlen(buffer) > 0) && (strcmp(buffer, "none"));
      }
      for (i = 0; i < deplist->get_size(); i++) {
        dparam = deplist->get(i);
        if (dparam != param) {
          en = (value && enabled);
          cid = findDlgIDFromParam(dparam);
          if (cid != 0) {
            if (en != IsWindowEnabled(GetDlgItem(hDlg, ID_PARAM + cid))) {
              ProcessDependentList(hDlg, dparam, en);
              EnableParam(hDlg, 0, dparam, en);
            }
          }
        }
      }
    }
  }
}

void SetDefaultButtons(HWND Window, UINT xpos, UINT ypos)
{
  RECT r;

  r.left = xpos;
  r.top = ypos;
  r.right = r.left + 50;
  r.bottom = r.top + 14;
  MapDialogRect(Window, &r);
  MoveWindow(GetDlgItem(Window, IDOK), r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, FALSE);
  r.left = xpos + 60;
  r.top = ypos;
  r.right = r.left + 50;
  r.bottom = r.top + 14;
  MapDialogRect(Window, &r);
  MoveWindow(GetDlgItem(Window, IDCANCEL), r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, FALSE);
}

void HandleOkCancel(HWND hDlg, UINT_PTR id, bx_list_c *list)
{
  if (id == IDOK) {
    SetParamList(hDlg, list);
  }
  cleanupDlgLists();
  DestroyWindow(hwndTT);
  EndDialog(hDlg, (id == IDOK) ? 1 : -1);
}

#define BXPD_SET_PATH    1
#define BXPD_UPDATE_DEPS 2

UINT HandleChildWindowEvents(HWND hDlg, UINT_PTR id, UINT_PTR nc)
{
  UINT i, ret = 0;
  char fname[BX_PATHNAME_LEN];

  if ((id >= ID_BROWSE) && (id < (ID_BROWSE + nextDlgID))) {
    i = (UINT)(id - ID_BROWSE);
    bx_param_string_c *sparam = (bx_param_string_c *)findParamFromDlgID(i);
    if (sparam != NULL) {
      GetDlgItemText(hDlg, ID_PARAM + i, fname, BX_PATHNAME_LEN);
      if (AskFilename(hDlg, (bx_param_filename_c *)sparam, fname) > 0) {
        SetWindowText(GetDlgItem(hDlg, ID_PARAM + i), fname);
        SendMessage(GetDlgItem(hDlg, ID_PARAM + i), EM_SETMODIFY, 1, 0);
        SetFocus(GetDlgItem(hDlg, ID_PARAM + i));
        ret = BXPD_SET_PATH;
      }
    }
  } else if ((id >= ID_PARAM) && (id < (ID_PARAM + nextDlgID))) {
    if ((nc == EN_CHANGE) || ((nc == CBN_SELCHANGE)) || (nc == BN_CLICKED)) {
      i = (UINT)(id - ID_PARAM);
      bx_param_c *param = findParamFromDlgID(i);
      if (param != NULL) {
        ProcessDependentList(hDlg, param, TRUE);
        ret = BXPD_UPDATE_DEPS;
      }
    }
  }
  return ret;
}

static INT_PTR CALLBACK ParamDlgProc(HWND Window, UINT AMessage, WPARAM wParam, LPARAM lParam)
{
  static bx_list_c *list = NULL;
  bx_list_c *tmplist;
  int cid;
  UINT_PTR code, id, noticode;
  UINT i, j, k;
  RECT r, r2;
  SIZE size;
  NMHDR nmhdr;

  switch (AMessage) {
    case WM_CLOSE:
      HandleOkCancel(Window, IDCANCEL, list);
      break;
    case WM_INITDIALOG:
      list = (bx_list_c*)SIM->get_param((const char*)lParam);
      SetWindowText(Window, list->get_title());
      nextDlgID = 1;
      size = CreateParamList(Window, 0, 6, 6, FALSE, list);
      SetDefaultButtons(Window, size.cx / 2 - 50, size.cy + 12);
      GetWindowRect(Window, &r2);
      r.left = 0;
      r.top = 0;
      r.right = size.cx + 18;
      r.bottom = size.cy + 52;
      MapDialogRect(Window, &r);
      MoveWindow(Window, r2.left, r2.top, r.right, r.bottom, TRUE);
      CreateParamDlgTooltip(Window);
      return TRUE;
    case WM_COMMAND:
      code = LOWORD(wParam);
      noticode = HIWORD(wParam);
      switch (code) {
        case IDOK:
        case IDCANCEL:
          HandleOkCancel(Window, code, list);
          break;
        default:
          HandleChildWindowEvents(Window, code, noticode);
      }
      break;
    case WM_NOTIFY:
      nmhdr = *(LPNMHDR)lParam;
      code = nmhdr.code;
      id = nmhdr.idFrom;
      if (code == (UINT)TCN_SELCHANGE) {
        j = (UINT)(id - ID_PARAM);
        tmplist = (bx_list_c *)findParamFromDlgID(j);
        if (tmplist != NULL) {
          k = (UINT)TabCtrl_GetCurSel(GetDlgItem(Window, id));
          cid = findDlgListBaseID(tmplist);
          for (i = 0; i < (UINT)tmplist->get_size(); i++) {
            ShowParamList(Window, cid + i, (i == k), (bx_list_c*)tmplist->get(i));
          }
        }
      } else if (code == (UINT)UDN_DELTAPOS) {
        if (id >= ID_UPDOWN) {
          PostMessage(GetDlgItem(Window, ID_PARAM + (id - ID_UPDOWN)), EM_SETMODIFY, TRUE, 0);
        }
      } else {
        OnWMNotify(lParam);
      }
      break;
    case WM_CTLCOLOREDIT:
      SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
      return (INT_PTR)GetSysColorBrush(COLOR_WINDOW);
      break;
  }
  return 0;
}

BOOL CreateImage(HWND hDlg, int sectors, const char *filename)
{
  if (sectors < 1) {
    MessageBox(hDlg, "The disk size is invalid.", "Invalid size", MB_ICONERROR);
    return FALSE;
  }
  if (lstrlen(filename) < 1) {
    MessageBox(hDlg, "You must type a file name for the new disk image.", "Bad filename", MB_ICONERROR);
    return FALSE;
  }
  int ret = SIM->create_disk_image (filename, sectors, 0);
  if (ret == -1) {  // already exists
    int answer = MessageBox(hDlg, "File exists.  Do you want to overwrite it?",
                            "File exists", MB_YESNO);
    if (answer == IDYES)
      ret = SIM->create_disk_image (filename, sectors, 1);
    else
      return FALSE;
  }
  if (ret == -2) {
    MessageBox(hDlg, "I could not create the disk image. Check for permission problems or available disk space.", "Failed", MB_ICONERROR);
    return FALSE;
  }
  return TRUE;
}

static INT_PTR CALLBACK FloppyParamDlgProc(HWND Window, UINT AMessage, WPARAM wParam, LPARAM lParam)
{
  static bx_list_c *list = NULL;
  static UINT path_id, type_id, status_id;
  bx_param_enum_c *status;
  UINT_PTR code, noticode;
  UINT ret;
  int i;
  RECT r, r2;
  SIZE size;
  char mesg[MAX_PATH];
  char path[MAX_PATH];

  switch (AMessage) {
    case WM_INITDIALOG:
      list = (bx_list_c*)SIM->get_param((const char*)lParam);
      SetWindowText(Window, list->get_title());
      nextDlgID = 1;
      size = CreateParamList(Window, 0, 6, 6, FALSE, list);
      CreateLabel(Window, 99, 60, size.cy + 12, 160, FALSE,
                  "Clicking OK signals a media change for this drive.");
      CreateButton(Window, IDCREATE, size.cx / 2 - 85, size.cy + 30, FALSE, "Create Image");
      SetDefaultButtons(Window, size.cx / 2 - 25, size.cy + 30);
      GetWindowRect(Window, &r2);
      r.left = 0;
      r.top = 0;
      r.right = size.cx + 18;
      r.bottom = size.cy + 70;
      MapDialogRect(Window, &r);
      MoveWindow(Window, r2.left, r2.top, r.right, r.bottom, TRUE);
      CreateParamDlgTooltip(Window);
      path_id = findDlgIDFromParam(list->get_by_name("path")) + ID_PARAM;
      type_id = findDlgIDFromParam(list->get_by_name("type")) + ID_PARAM;
      status_id = findDlgIDFromParam(list->get_by_name("status")) + ID_PARAM;
      return TRUE;
    case WM_COMMAND:
      code = LOWORD(wParam);
      noticode = HIWORD(wParam);
      switch (code) {
        case IDOK:
          // force a media change
          status = (bx_param_enum_c*)list->get_by_name("status");
          status->set(BX_EJECTED);
        case IDCANCEL:
          HandleOkCancel(Window, code, list);
          break;
        case IDCREATE:
          GetDlgItemText(Window, path_id, path, MAX_PATH);
          backslashes(path);
          i = SendMessage(GetDlgItem(Window, type_id), CB_GETCURSEL, 0, 0);
          if (CreateImage(Window, floppy_type_n_sectors[i], path)) {
            wsprintf(mesg, "Created a %s disk image called %s", floppy_type_names[i], path);
            MessageBox(Window, mesg, "Image created", MB_OK);
          }
          return TRUE;
          break;
        default:
          ret = HandleChildWindowEvents(Window, code, noticode);
          if (ret == BXPD_SET_PATH) {
            SendMessage(GetDlgItem(Window, status_id), BM_SETCHECK, BST_CHECKED, 0);
            SendMessage(GetDlgItem(Window, type_id), CB_SELECTSTRING, (WPARAM)-1, (LPARAM)"auto");
            EnableWindow(GetDlgItem(Window, IDCREATE), FALSE);
          } else if (ret == BXPD_UPDATE_DEPS) {
            if (code == path_id) {
              EnableWindow(GetDlgItem(Window, IDCREATE), IsWindowEnabled(GetDlgItem(Window, type_id)));
            } else if (code == type_id) {
              i = SendMessage(GetDlgItem(Window, type_id), CB_GETCURSEL, 0, 0);
              EnableWindow(GetDlgItem(Window, IDCREATE), (i != 0));
            }
          }
      }
      break;
    case WM_CLOSE:
    case WM_NOTIFY:
    case WM_CTLCOLOREDIT:
      return ParamDlgProc(Window, AMessage, wParam, lParam);
      break;
  }
  return 0;
}

INT_PTR win32ParamDialog(HWND parent, const char *menu)
{
  InitDlgFont();
  RegisterScrollWindow(NULL);
  INT_PTR ret = DialogBoxParam(NULL, MAKEINTRESOURCE(PARAM_DLG), parent, (DLGPROC)ParamDlgProc, (LPARAM)menu);
  DeleteObject(DlgFont);
  return ret;
}

INT_PTR win32FloppyParamDialog(HWND parent, const char *menu)
{
  InitDlgFont();
  RegisterScrollWindow(NULL);
  INT_PTR ret = DialogBoxParam(NULL, MAKEINTRESOURCE(PARAM_DLG), parent, (DLGPROC)FloppyParamDlgProc, (LPARAM)menu);
  DeleteObject(DlgFont);
  return ret;
}

#endif // BX_USE_WIN32CONFIG
