/////////////////////////////////////////////////////////////////////////
// $Id: win32paramdlg.cc,v 1.3 2009-03-12 19:40:41 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009  Volker Ruppert
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

#if BX_USE_TEXTCONFIG && defined(WIN32) && (BX_WITH_WIN32 || BX_WITH_SDL)

#include "bochs.h"
#include "textconfig.h"
#include "win32res.h"
#include "win32dialog.h"
#include "win32paramdlg.h"

#define ID_LABEL 100
#define ID_PARAM 1000
#define ID_BROWSE 2000

HFONT     DlgFont;

int AskFilename(HWND hwnd, bx_param_filename_c *param, const char *ext)
{
  OPENFILENAME ofn;
  int ret;
  DWORD errcode;
  char filename[MAX_PATH];
  const char *title;
  char errtext[80];

  param->get(filename, MAX_PATH);
  // common file dialogs don't accept raw device names
  if ((isalpha(filename[0])) && (filename[1] == ':') && (strlen(filename) == 2)) {
    filename[0] = 0;
  }
  title = param->get_label();
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
  if (!lstrcmp(ext, "img")) {
    ofn.lpstrFilter = "Floppy image files (*.img)\0*.img\0All files (*.*)\0*.*\0";
  } else if (!lstrcmp(ext, "iso")) {
    ofn.lpstrFilter = "CD-ROM image files (*.iso)\0*.iso\0All files (*.*)\0*.*\0";
  } else if (!lstrcmp(ext, "txt")) {
    ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
  } else {
    ofn.lpstrFilter = "All files (*.*)\0*.*\0";
  }
  if (param->get_options()->get() & bx_param_filename_c::SAVE_FILE_DIALOG) {
    ofn.Flags |= OFN_OVERWRITEPROMPT;
    ret = GetSaveFileName(&ofn);
  } else {
    ofn.Flags |= OFN_FILEMUSTEXIST;
    ret = GetOpenFileName(&ofn);
  }
  param->set(filename);
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
  LFont.lfPitchAndFamily = DEFAULT_PITCH;    // pitch and family
  lstrcpy(LFont.lfFaceName, "Helv");          // pointer to typeface name string
  DlgFont  = CreateFontIndirect(&LFont);
}

HWND CreateLabel(HWND hDlg, UINT id, UINT ctrl, const char *text)
{
  HWND Label;
  RECT r;
  int code;

  code = ID_LABEL + id;
  r.left = 10;
  r.top = ctrl * 20 + 17;
  r.right = 85;
  r.bottom = r.top + 15;
  MapDialogRect(hDlg, &r);
  Label = CreateWindow("STATIC", text, WS_CHILD, r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  SendMessage(Label, WM_SETFONT, (UINT)DlgFont, TRUE);
  ShowWindow(Label, SW_SHOW);
  return Label;
}

HWND CreateBrowseButton(HWND hDlg, UINT id, UINT ctrl)
{
  HWND Button;
  RECT r;
  int code;

  code = ID_BROWSE + id;
  r.left = 200;
  r.top = ctrl * 20 + 15;
  r.right = 250;
  r.bottom = r.top + 14;
  MapDialogRect(hDlg, &r);
  Button = CreateWindow("BUTTON", "Browse...", WS_CHILD, r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  SendMessage(Button, WM_SETFONT, (UINT)DlgFont, TRUE);
  ShowWindow(Button, SW_SHOW);
  return Button;
}

HWND CreateCheckbox(HWND hDlg, UINT id, UINT ctrl, bx_param_bool_c *bparam)
{
  HWND Checkbox;
  RECT r;
  int code, val;

  code = ID_PARAM + id;
  r.left = 90;
  r.top = ctrl * 20 + 15;
  r.right = 120;
  r.bottom = r.top + 14;
  MapDialogRect(hDlg, &r);
  Checkbox = CreateWindow("BUTTON", "", BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP,
                          r.left, r.top, r.right-r.left+1, r.bottom-r.top+1,
                          hDlg, (HMENU)code, NULL, NULL);
  val = bparam->get();
  SendMessage(Checkbox, BM_SETCHECK, val ? BST_CHECKED : BST_UNCHECKED, 0);
  SendMessage(Checkbox, WM_SETFONT, (UINT)DlgFont, TRUE);
  ShowWindow(Checkbox, SW_SHOW);
  return Checkbox;
}

HWND CreateInput(HWND hDlg, UINT id, UINT ctrl, bx_param_c *param)
{
  HWND Input;
  RECT r;
  int code, i, style;
  bx_param_num_c *nparam;
  bx_param_string_c *sparam;
  char buffer[512];
  char eachbyte[16];
  char sep_string[2];
  char *val;

  code = ID_PARAM + id;
  style = WS_CHILD | WS_TABSTOP;
  if (param->get_type() == BXT_PARAM_STRING) {
    sparam = (bx_param_string_c*)param;
    val = sparam->getptr();
    if (sparam->get_options()->get() & sparam->RAW_BYTES) {
      buffer[0] = 0;
      sep_string[0] = sparam->get_separator();
      sep_string[1] = 0;
      for (i = 0; i < sparam->get_maxsize(); i++) {
        wsprintf(eachbyte, "%s%02x", (i>0)?sep_string : "", (Bit8u)0xff&val[i]);
        strncat(buffer, eachbyte, sizeof(buffer));
      }
    } else {
      lstrcpyn(buffer, val, 512);
      style |= ES_AUTOHSCROLL;
    }
  } else {
    nparam = (bx_param_num_c*)param;
    if (nparam->get_base() == BASE_HEX) {
      wsprintf(buffer, "0x%x", nparam->get());
    } else {
      wsprintf(buffer, "%d", nparam->get());
      style |= ES_NUMBER;
    }
  }
  r.left = 90;
  r.top = ctrl * 20 + 15;
  r.right = 190;
  r.bottom = r.top + 14;
  MapDialogRect(hDlg, &r);
  Input = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY, "EDIT", buffer,
                         style, r.left, r.top, r.right-r.left+1,
                         r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  SendMessage(Input, WM_SETFONT, (UINT)DlgFont, TRUE);
  ShowWindow(Input, SW_SHOW);
  return Input;
}

HWND CreateCombobox(HWND hDlg, UINT id, UINT ctrl, bx_param_enum_c *eparam)
{
  HWND Combo;
  RECT r;
  int code, j;
  const char *choice;

  code = ID_PARAM + id;
  r.left = 90;
  r.top = ctrl * 20 + 15;
  r.right = 190;
  r.bottom = r.top + 14;
  MapDialogRect(hDlg, &r);
  Combo = CreateWindow("COMBOBOX", "", WS_CHILD | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST,
                       r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, hDlg, (HMENU)code, NULL, NULL);
  j = 0;
  do {
    choice = eparam->get_choice(j);
    SendMessage(Combo, CB_ADDSTRING, 0, (LPARAM)choice);
    j++;
  } while (choice != NULL);
  SendMessage(Combo, CB_SETCURSEL, (WPARAM)(eparam->get()-eparam->get_min()), 0);
  SendMessage(Combo, WM_SETFONT, (UINT)DlgFont, TRUE);
  ShowWindow(Combo, SW_SHOW);
  return Combo;
}

static BOOL CALLBACK ParamDlgProc(HWND Window, UINT AMessage, WPARAM wParam, LPARAM lParam)
{
  static bx_list_c *list = NULL;
  static int items = 0;
  static Bit64u *n_disable = NULL;
  bx_param_c *param;
  bx_param_num_c *nparam;
  bx_param_enum_c *eparam;
  bx_param_string_c *sparam;
  bx_list_c *deplist;
  int code, i, j, k, nctrl, options, val, xsize = 308, ysize;
  const char *label, *src;
  char buffer[512], rawbuf[512];
  HWND ltext, control, browse;
  RECT r;

  switch (AMessage) {
    case WM_CLOSE:
      free(n_disable);
      EndDialog(Window, 0);
      break;
    case WM_INITDIALOG:
      list = (bx_list_c*)SIM->get_param((const char*)lParam);
      items = list->get_size();
      n_disable = (Bit64u*)malloc(items * sizeof(Bit64u));
      memset(n_disable, 0, items * sizeof(Bit64u));
      options = list->get_options()->get();
      SetWindowText(Window, list->get_title()->getptr());
      nctrl = 0;
      for (i = 0; i < items; i++) {
        param = list->get(i);
        if (!SIM->get_init_done() || (param->get_enabled() && param->get_runtime_param())) {
          label = param->get_label();
          if (label == NULL) {
            label = param->get_name();
          }
          if ((options & list->SHOW_GROUP_NAME) && (param->get_group() != NULL)) {
            wsprintf(buffer, "%s %s", param->get_group(), label);
          } else {
            lstrcpyn(buffer, label, 512);
          }
          ltext = CreateLabel(Window, i, nctrl, buffer);
          browse = NULL;
          if (param->get_type() == BXT_PARAM_BOOL) {
            control = CreateCheckbox(Window, i, nctrl, (bx_param_bool_c*)param);
          } else if (param->get_type() == BXT_PARAM_ENUM) {
            control = CreateCombobox(Window, i, nctrl, (bx_param_enum_c*)param);
          } else if (param->get_type() == BXT_PARAM_NUM) {
            control = CreateInput(Window, i, nctrl, param);
          } else if (param->get_type() == BXT_PARAM_STRING) {
            control = CreateInput(Window, i, nctrl, param);
            sparam = (bx_param_string_c*)param;
            if (sparam->get_options()->get() & sparam->IS_FILENAME) {
              browse = CreateBrowseButton(Window, i, nctrl);
              xsize = 400;
            }
          }
          if (!param->get_enabled()) {
            EnableWindow(ltext, FALSE);
            EnableWindow(control, FALSE);
            if (browse) EnableWindow(browse, FALSE);
          }
          nctrl++;
        }
      }
      if (xsize == 400) {
        r.left = 75;
        r.right = 125;
      } else {
        r.left = 45;
        r.right = 95;
      }
      r.top = nctrl * 20 + 20;
      r.bottom = r.top + 14;
      MapDialogRect(Window, &r);
      MoveWindow(GetDlgItem(Window, IDOK), r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, FALSE);
      if (xsize == 400) {
        r.left = 135;
        r.right = 185;
      } else {
        r.left = 105;
        r.right = 155;
      }
      r.top = nctrl * 20 + 20;
      r.bottom = r.top + 14;
      MapDialogRect(Window, &r);
      MoveWindow(GetDlgItem(Window, IDCANCEL), r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, FALSE);
      ysize = nctrl * 32 + 100;
      GetWindowRect(Window, &r);
      MoveWindow(Window, r.left, r.top, xsize, ysize, TRUE);
      return TRUE;
    case WM_COMMAND:
      code = LOWORD(wParam);
      switch (code) {
        case IDCANCEL:
          free(n_disable);
          EndDialog(Window, 0);
          break;
        case IDOK:
          for (i = 0; i < items; i++) {
            param = list->get(i);
            if (param->get_type() == BXT_PARAM_BOOL) {
              val = SendMessage(GetDlgItem(Window, ID_PARAM + i), BM_GETCHECK, 0, 0);
              ((bx_param_bool_c*)param)->set(val == BST_CHECKED);
            } else if (param->get_type() == BXT_PARAM_ENUM) {
              val = SendMessage(GetDlgItem(Window, ID_PARAM + i), CB_GETCURSEL, 0, 0);
              eparam = (bx_param_enum_c*)param;
              eparam->set(val + eparam->get_min());
            } else {
              if (SendMessage(GetDlgItem(Window, ID_PARAM + i), EM_GETMODIFY, 0, 0)) {
                if (param->get_type() == BXT_PARAM_NUM) {
                  nparam = (bx_param_num_c*)param;
                  if (nparam->get_base() == BASE_HEX) {
                    GetWindowText(GetDlgItem(Window, ID_PARAM + i), buffer, 511);
                    sscanf(buffer, "%x", &val);
                  } else {
                    val = GetDlgItemInt(Window, ID_PARAM + i, NULL, FALSE);
                  }
                  nparam->set(val);
                } else if (param->get_type() == BXT_PARAM_STRING) {
                  GetWindowText(GetDlgItem(Window, ID_PARAM + i), buffer, 511);
                  sparam = (bx_param_string_c*)param;
                  if (sparam->get_options()->get() & sparam->RAW_BYTES) {
                    src = &buffer[0];
                    memset(rawbuf, 0, sparam->get_maxsize());
                    for (j = 0; j < sparam->get_maxsize(); j++) {
                      while (*src == sparam->get_separator())
                        src++;
                      if (*src == 0) break;
                      if (sscanf(src, "%02x", &val)) {
                        rawbuf[j] = val;
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
          free(n_disable);
          EndDialog(Window, 1);
          break;
        default:
          if ((code >= ID_BROWSE) && (code < (ID_BROWSE + items))) {
            i = code - ID_BROWSE;
            sparam = (bx_param_string_c *)list->get(i);
            if (AskFilename(Window, (bx_param_filename_c *)sparam, "txt") > 0) {
              SetWindowText(GetDlgItem(Window, ID_PARAM + i), sparam->getptr());
              SetFocus(GetDlgItem(Window, ID_PARAM + i));
            }
          } else if ((code >= ID_PARAM) && (code < (ID_PARAM + items))) {
            i = code - ID_PARAM;
            param = list->get(i);
            if ((param->get_type() == BXT_PARAM_BOOL) ||
                (param->get_type() == BXT_PARAM_NUM)) {
              deplist = ((bx_param_bool_c *)param)->get_dependent_list();
              if (deplist != NULL) {
                if (param->get_type() == BXT_PARAM_BOOL) {
                  val = SendMessage(GetDlgItem(Window, code), BM_GETCHECK, 0, 0);
                } else {
                  val = GetDlgItemInt(Window, code, NULL, FALSE);
                }
                for (j = 0; j < items; j++) {
                  for (k = 0; k < deplist->get_size(); k++) {
                    if ((list->get(j) != param) && (list->get(j) == deplist->get(k))) {
                      if (val) {
                        n_disable[j] &= ~(1 << i);
                      } else {
                        n_disable[j] |= (1 << i);
                      }
                      EnableWindow(GetDlgItem(Window, ID_LABEL+j), !n_disable[j]);
                      EnableWindow(GetDlgItem(Window, ID_PARAM+j), !n_disable[j]);
                      if (GetDlgItem(Window, ID_BROWSE+j) != NULL) {
                        EnableWindow(GetDlgItem(Window, ID_BROWSE+j), !n_disable[j]);
                      }
                    }
                  }
                }
              }
            }
          }
      }
      break;
    case WM_CTLCOLOREDIT:
      SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
      return (long)GetSysColorBrush(COLOR_WINDOW);
      break;
  }
  return 0;
}

int win32ParamDialog(HWND parent, const char *menu)
{
  int ret;

  InitDlgFont();
  ret = DialogBoxParam(NULL, MAKEINTRESOURCE(PARAM_DLG), parent, (DLGPROC)ParamDlgProc, (LPARAM)menu);
  DeleteObject(DlgFont);
  return ret;
}

#endif // BX_USE_TEXTCONFIG && defined(WIN32)
