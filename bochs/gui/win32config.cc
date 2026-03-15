/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003-2026  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "win32dialog.h"
#include "bochs.h"
#include "param_names.h"
#include "gui.h"
#include "win32res.h"
#include "win32paramdlg.h"
#include "plugin.h"

#if BX_USE_WIN32CONFIG

static int win32_ci_callback(void *userdata, ci_command_t command);
static BxEvent* win32_notify_callback(void *unused, BxEvent *event);

PLUGIN_ENTRY_FOR_MODULE(win32config)
{
  if (mode == PLUGIN_INIT) {
    SIM->register_configuration_interface("win32config", win32_ci_callback, NULL);
    SIM->set_notify_callback(win32_notify_callback, NULL);
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_CI;
  }
  return 0; // Success
}

const char log_choices[N_ACT+1][16] = {"ignore", "log", "warn user", "ask user", "end simulation", "no change"};

HWND GetBochsWindow()
{
  HWND hwnd;

  hwnd = FindWindow("Bochs for Windows", NULL);
  if (hwnd == NULL) {
    hwnd = GetForegroundWindow();
  }
  return hwnd;
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  char path[MAX_PATH];

  if (uMsg == BFFM_INITIALIZED) {
    GetCurrentDirectory(MAX_PATH, path);
    SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)path);
  }
  return 0;
}

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0
#endif

int BrowseDir(const char *Title, char *result)
{
  BROWSEINFO browseInfo;
  LPITEMIDLIST ItemIDList;
  int r = -1;

  memset(&browseInfo,0,sizeof(BROWSEINFO));
  browseInfo.hwndOwner = GetBochsWindow();
  browseInfo.pszDisplayName = result;
  browseInfo.lpszTitle = (LPCSTR)Title;
  browseInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
  browseInfo.lpfn = BrowseCallbackProc;
  ItemIDList = SHBrowseForFolder(&browseInfo);
  if (ItemIDList != NULL) {
    *result = 0;
    if (SHGetPathFromIDList(ItemIDList, result)) {
      if (result[0]) r = 0;
    }
    // free memory used
    IMalloc * imalloc = 0;
    if (SUCCEEDED(SHGetMalloc(&imalloc))) {
      imalloc->Free(ItemIDList);
      imalloc->Release();
    }
  }
  return r;
}

static BOOL CALLBACK LogAskProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  BxEvent *event;
  int level;

  switch (msg) {
    case WM_INITDIALOG:
      event = (BxEvent*)lParam;
      level = event->u.logmsg.level;
      SetWindowText(hDlg, SIM->get_log_level_name(level));
      SetWindowText(GetDlgItem(hDlg, IDASKDEV), event->u.logmsg.prefix);
      SetWindowText(GetDlgItem(hDlg, IDASKMSG), event->u.logmsg.msg);
      SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_ADDSTRING, 0, (LPARAM)"Continue");
      SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_ADDSTRING, 0, (LPARAM)"Continue and don't ask again");
      if (event->u.logmsg.mode == BX_LOG_DLG_ASK) {
        SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_ADDSTRING, 0, (LPARAM)"Kill simulation");
        SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_ADDSTRING, 0, (LPARAM)"Abort (dump core)");
#if BX_DEBUGGER
        if (bx_dbg.debugger_active) {
          SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_ADDSTRING, 0, (LPARAM)"Continue and return to debugger");
        }
#endif
        SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_SETCURSEL, 2, 0);
      } else {
        SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_SETCURSEL, 0, 0);
      }
      SetFocus(GetDlgItem(hDlg, IDASKLIST));
      return FALSE;
    case WM_CLOSE:
      EndDialog(hDlg, BX_LOG_ASK_CHOICE_DIE);
      break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          EndDialog(hDlg, SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_GETCURSEL, 0, 0));
          break;
        case IDCANCEL:
          EndDialog(hDlg, BX_LOG_ASK_CHOICE_DIE);
          break;
      }
  }
  return FALSE;
}

static BOOL CALLBACK StringParamProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static bx_param_string_c *param;
  char buffer[512];
  const char *title;

  switch (msg) {
    case WM_INITDIALOG:
      param = (bx_param_string_c *)lParam;
      title = param->get_label();
      if ((title == NULL) || (strlen(title) == 0)) {
        title = param->get_name();
      }
      SetWindowText(hDlg, title);
      SetWindowText(GetDlgItem(hDlg, IDSTRING), param->getptr());
      SendMessage(GetDlgItem(hDlg, IDSTRING), EM_SETLIMITTEXT, param->get_maxsize(), 0);
      return TRUE;
    case WM_CLOSE:
      EndDialog(hDlg, -1);
      break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          GetDlgItemText(hDlg, IDSTRING, buffer, param->get_maxsize() + 1);
          param->set(buffer);
          EndDialog(hDlg, 1);
          break;
        case IDCANCEL:
          EndDialog(hDlg, -1);
          break;
      }
  }
  return FALSE;
}

void InitPluginCtrl(HWND hDlg)
{
  int count, i;
  bx_list_c *plugin_ctrl;
  bx_param_bool_c *plugin;

  SendMessage(GetDlgItem(hDlg, IDPLUGLIST1), LB_RESETCONTENT, 0, 0);
  SendMessage(GetDlgItem(hDlg, IDPLUGLIST2), LB_RESETCONTENT, 0, 0);
  plugin_ctrl = (bx_list_c*) SIM->get_param(BXPN_PLUGIN_CTRL);
  count = plugin_ctrl->get_size();
  for (i = 0; i < count; i++) {
    plugin = (bx_param_bool_c*)plugin_ctrl->get(i);
    if (plugin->get()) {
      SendMessage(GetDlgItem(hDlg, IDPLUGLIST2), LB_ADDSTRING, 0, (LPARAM)plugin->get_name());
    } else {
      SendMessage(GetDlgItem(hDlg, IDPLUGLIST1), LB_ADDSTRING, 0, (LPARAM)plugin->get_name());
    }
  }
  EnableWindow(GetDlgItem(hDlg, IDLOAD), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDUNLOAD), FALSE);
}

void ShowPluginCtrl(HWND hDlg, bool show)
{
  if (show) {
    SetWindowText(GetDlgItem(hDlg, IDOPTGRP), "Optional Plugin Control");
  }
  ShowWindow(GetDlgItem(hDlg, IDPLUGTXT1), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDPLUGLIST1), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDPLUGTXT2), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDPLUGLIST2), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOAD), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDUNLOAD), show ? SW_NORMAL : SW_HIDE);
}

void HandlePluginCtrl(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  int i;
  long code;
  char plugname[20], message[80];

  switch (msg) {
    case WM_COMMAND:
      code = HIWORD(wParam);
      switch (LOWORD(wParam)) {
        case IDPLUGLIST1:
          if (code == LBN_SELCHANGE) {
            SendMessage(GetDlgItem(hDlg, IDPLUGLIST2), LB_SETCURSEL, -1, 0);
            EnableWindow(GetDlgItem(hDlg, IDLOAD), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDUNLOAD), FALSE);
          }
          break;
        case IDPLUGLIST2:
          if (code == LBN_SELCHANGE) {
            SendMessage(GetDlgItem(hDlg, IDPLUGLIST1), LB_SETCURSEL, -1, 0);
            EnableWindow(GetDlgItem(hDlg, IDLOAD), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDUNLOAD), TRUE);
          }
          break;
        case IDLOAD:
          i = SendMessage(GetDlgItem(hDlg, IDPLUGLIST1), LB_GETCURSEL, 0, 0);
          SendMessage(GetDlgItem(hDlg, IDPLUGLIST1), LB_GETTEXT, i, (LPARAM)plugname);
          if (SIM->opt_plugin_ctrl(plugname, 1)) {
            wsprintf(message, "Plugin '%s' loaded", plugname);
            MessageBox(hDlg, message, "Plugin Control", MB_ICONINFORMATION);
            SendMessage(GetDlgItem(hDlg, IDPLUGLIST1), LB_DELETESTRING, i, 0);
            SendMessage(GetDlgItem(hDlg, IDPLUGLIST2), LB_ADDSTRING, 0, (LPARAM)plugname);
            EnableWindow(GetDlgItem(hDlg, IDLOAD), FALSE);
          }
          break;
        case IDUNLOAD:
          i = SendMessage(GetDlgItem(hDlg, IDPLUGLIST2), LB_GETCURSEL, 0, 0);
          SendMessage(GetDlgItem(hDlg, IDPLUGLIST2), LB_GETTEXT, i, (LPARAM)plugname);
          if (SIM->opt_plugin_ctrl(plugname, 0)) {
            wsprintf(message, "Plugin '%s' unloaded", plugname);
            MessageBox(hDlg, message, "Plugin Control", MB_ICONINFORMATION);
            SendMessage(GetDlgItem(hDlg, IDPLUGLIST1), LB_ADDSTRING, 0, (LPARAM)plugname);
            SendMessage(GetDlgItem(hDlg, IDPLUGLIST2), LB_DELETESTRING, i, 0);
            EnableWindow(GetDlgItem(hDlg, IDUNLOAD), FALSE);
          }
          break;
      }
      break;
  }
}

void SetStandardLogOptions(HWND hDlg)
{
  int level, idx;
  int defchoice[5];

  for (level=0; level<N_LOGLEV; level++) {
    int mod = 0;
    int first = SIM->get_log_action (mod, level);
    BOOL consensus = true;
    // now compare all others to first.  If all match, then use "first" as
    // the initial value.
    for (mod=1; mod<SIM->get_n_log_modules(); mod++) {
      if (first != SIM->get_log_action (mod, level)) {
        consensus = false;
        break;
      }
    }
    if (consensus)
      defchoice[level] = first;
    else
      defchoice[level] = 4;
  }
  for (level=0; level<N_LOGLEV; level++) {
    idx = 0;
    SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_RESETCONTENT, 0, 0);
    for (int action=0; action<5; action++) {
      // the exclude expression allows some choices not being available if they
      // don't make any sense.  For example, it would be stupid to ignore a panic.
      if (!BX_LOG_OPTS_EXCLUDE(level, action)) {
        SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_ADDSTRING, 0, (LPARAM)log_choices[action]);
        SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_SETITEMDATA, idx, action);
        if (action == defchoice[level]) {
          SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_SETCURSEL, idx, 0);
        }
        idx++;
      }
    }
  }
  EnableWindow(GetDlgItem(hDlg, IDDEVLIST), FALSE);
}

void SetAdvancedLogOptions(HWND hDlg)
{
  int idx, level, mod;

  idx = SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_GETCURSEL, 0, 0);
  mod = SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_GETITEMDATA, idx, 0);
  for (level=0; level<N_LOGLEV; level++) {
    idx = 0;
    SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_RESETCONTENT, 0, 0);
    for (int action=0; action<4; action++) {
      // exclude some action / level combinations (see above)
      if (!BX_LOG_OPTS_EXCLUDE(level, action)) {
        SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_ADDSTRING, 0, (LPARAM)log_choices[action]);
        SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_SETITEMDATA, idx, action);
        if (action == SIM->get_log_action (mod, level)) {
          SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_SETCURSEL, idx, 0);
        }
        idx++;
      }
    }
  }
}

void ApplyLogOptions(HWND hDlg, BOOL advanced)
{
  int idx, level, mod, value;

  if (advanced) {
    idx = SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_GETCURSEL, 0, 0);
    mod = SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_GETITEMDATA, idx, 0);
    for (level=0; level<N_LOGLEV; level++) {
      idx = SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_GETCURSEL, 0, 0);
      value = SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_GETITEMDATA, idx, 0);
      SIM->set_log_action(mod, level, value);
    }
    EnableWindow(GetDlgItem(hDlg, IDDEVLIST), TRUE);
  } else {
    for (level=0; level<N_LOGLEV; level++) {
      idx = SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_GETCURSEL, 0, 0);
      value = SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_GETITEMDATA, idx, 0);
      if (value < 4) {
        // set new default
        SIM->set_default_log_action(level, value);
        // apply that action to all modules (devices)
        SIM->set_log_action(-1, level, value);
      }
    }
  }
  EnableWindow(GetDlgItem(hDlg, IDADVLOGOPT), TRUE);
}

void InitLogOptions(HWND hDlg, BOOL advanced)
{
  int idx, mod;
  char name[32];

  for (mod=0; mod<SIM->get_n_log_modules(); mod++) {
    if (lstrcmp(SIM->get_logfn_name(mod), "?")) {
      lstrcpyn(name, SIM->get_logfn_name(mod), 32);
      idx = SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_ADDSTRING, 0, (LPARAM)name);
      SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_SETITEMDATA, idx, mod);
    }
  }
  if (advanced) {
    SendMessage(GetDlgItem(hDlg, IDADVLOGOPT), BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_SETCURSEL, 0, 0);
    SetAdvancedLogOptions(hDlg);
  } else {
    SetStandardLogOptions(hDlg);
  }
}

UINT HandleLogOptions(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL advanced;
  static BOOL changed;
  long noticode;
  UINT ret = 0;

  switch (msg) {
    case WM_INITDIALOG:
      advanced = (BOOL)lParam;
      InitLogOptions(hDlg, advanced);
      changed = FALSE;
      break;
    case WM_COMMAND:
      noticode = HIWORD(wParam);
      switch(noticode) {
        case CBN_SELCHANGE: /* LBN_SELCHANGE is the same value */
          switch (LOWORD(wParam)) {
            case IDDEVLIST:
              SetAdvancedLogOptions(hDlg);
              break;
            case IDLOGEVT1:
            case IDLOGEVT2:
            case IDLOGEVT3:
            case IDLOGEVT4:
            case IDLOGEVT5:
              if (!changed) {
                EnableWindow(GetDlgItem(hDlg, IDADVLOGOPT), FALSE);
                if (advanced) {
                  EnableWindow(GetDlgItem(hDlg, IDDEVLIST), FALSE);
                }
                changed = TRUE;
                EnableWindow(GetDlgItem(hDlg, IDAPPLY), TRUE);
                EnableWindow(GetDlgItem(hDlg, IDREVERT), TRUE);
                ret = BXPD_EDIT_START;
              }
              break;
          }
          break;
        default:
          switch (LOWORD(wParam)) {
            case IDADVLOGOPT:
              if (SendMessage(GetDlgItem(hDlg, IDADVLOGOPT), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                EnableWindow(GetDlgItem(hDlg, IDDEVLIST), TRUE);
                SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_SETCURSEL, 0, 0);
                SetAdvancedLogOptions(hDlg);
                advanced = TRUE;
              } else {
                SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_SETCURSEL, (WPARAM)-1, 0);
                SetStandardLogOptions(hDlg);
                advanced = FALSE;
              }
              break;
            case IDAPPLY:
            case IDREVERT:
              if (LOWORD(wParam) == IDAPPLY) {
                ApplyLogOptions(hDlg, advanced);
              } else {
                if (advanced) {
                  SetAdvancedLogOptions(hDlg);
                  EnableWindow(GetDlgItem(hDlg, IDDEVLIST), TRUE);
                } else {
                  SetStandardLogOptions(hDlg);
                }
                EnableWindow(GetDlgItem(hDlg, IDADVLOGOPT), TRUE);
              }
              EnableWindow(GetDlgItem(hDlg, IDAPPLY), FALSE);
              EnableWindow(GetDlgItem(hDlg, IDREVERT), FALSE);
              changed = FALSE;
              ret = BXPD_EDIT_END;
              break;
          }
      }
      break;
  }
  return ret;
}

void ShowLogOptions(HWND hDlg, bool show)
{
  if (show) {
    SetWindowText(GetDlgItem(hDlg, IDOPTGRP), "Log Options");
  }
  ShowWindow(GetDlgItem(hDlg, IDDEVLIST), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGLBL1), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGEVT1), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGLBL2), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGEVT2), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGLBL3), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGEVT3), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGLBL4), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGEVT4), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGLBL5), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDLOGEVT5), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDADVLOGOPT), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDAPPLY), show ? SW_NORMAL : SW_HIDE);
  ShowWindow(GetDlgItem(hDlg, IDREVERT), show ? SW_NORMAL : SW_HIDE);
  EnableWindow(GetDlgItem(hDlg, IDAPPLY), FALSE);
  EnableWindow(GetDlgItem(hDlg, IDREVERT), show ? FALSE : TRUE);
}

typedef struct {
  const char *label;
  const char *param;
  bool newdlg;
  bool init;
  SIZE size;
} edit_opts_t;

edit_opts_t start_options[] = {
  {"Plugin Control", "#plugins", true, false, {0, 0}},
  {"Logfile", "log", true, false, {0, 0}},
  {"Log Options", "#logopts", true, false, {0, 0}},
  {"CPU", "cpu", true, false, {0, 0}},
  {"Memory", "memory", false, false, {0, 0}},
  {"Clock & CMOS", "clock_cmos", true, false, {0, 0}},
  {"PCI", "pci", true, false, {0, 0}},
  {"Display & Interface", "display", true, false, {0, 0}},
  {"Keyboard & Mouse", "keyboard_mouse", true, false, {0, 0}},
  {"Disk & Boot", BXPN_MENU_DISK_WIN32, false, false, {0, 0}},
  {"Serial / Parallel / USB", "ports", false, false, {0, 0}},
  {"Network card", "network", false, false, {0, 0}},
  {"Sound card", "sound", false, false, {0, 0}},
  {"Other", "misc", true, false, {0, 0}},
#if BX_PLUGINS
  {"User-defined Options", "user", false, false, {0, 0}},
#endif
  {NULL, NULL, false, false, {0, 0}}
};

edit_opts_t runtime_options[] = {
  {"CD-ROM", BXPN_MENU_RUNTIME_CDROM, true, false, {0, 0}},
  {"USB", BXPN_MENU_RUNTIME_USB, false, false, {0, 0}},
  {"Sound", BXPN_MENU_RUNTIME_SOUND, true, false, {0, 0}},
  {"Misc", BXPN_MENU_RUNTIME_MISC, true, false, {0, 0}},
  {"Log Options", "#logopts", true, false, {0, 0}},
  {NULL, NULL, false, false, {0, 0}}
};

bool ResizeDialog(HWND hDlg, SIZE size, bool force)
{
  static RECT ri;
  static bool dlg_size_init = false;
  RECT r, r0, r1, r2, r3;
  bool resize0, resize1;

  if (!dlg_size_init) {
    GetWindowRect(hDlg, &ri);
    ri.right -= ri.left;
    ri.bottom -= ri.top;
    dlg_size_init = true;
  }
  r0 = ri;
  r.left = 0;
  r.top = 0;
  r.right = size.cx + 215;
  r.bottom = size.cy + 60;
  MapDialogRect(hDlg, &r);
  r1.left = 200;
  r1.top = 10;
  r1.right = 270;
  r1.bottom = 132;
  MapDialogRect(hDlg, &r1);
  r2.left = 355;
  r2.top = 125;
  r2.right = 50;
  r2.bottom = 14;
  MapDialogRect(hDlg, &r2);
  r3.left = 415;
  r3.top = 125;
  r3.right = 50;
  r3.bottom = 14;
  MapDialogRect(hDlg, &r3);
  if (r.right > r0.right) {
    r1.right += r.right - r0.right;
    r0.right = r.right;
    resize0 = true;
    resize1 = true;
  }
  if (r.bottom > r0.bottom) {
    r0.bottom = r.bottom;
    resize0 = true;
  }
  if (r.right - 330 > r1.right) {
    r1.right = r.right - 330;
    resize1 = true;
  }
  if (r.bottom - 60 > r1.bottom) {
    r1.bottom = r.bottom - 60;
    r2.top = r1.bottom - 15;
    r3.top = r1.bottom - 15;
    resize1 = true;
  }
  if (resize0 || force) {
    MoveWindow(hDlg, r0.left, r0.top, r0.right, r0.bottom, TRUE);
  }
  if (resize1 || force) {
    MoveWindow(GetDlgItem(hDlg, IDOPTGRP), r1.left, r1.top, r1.right, r1.bottom, TRUE);
    MoveWindow(GetDlgItem(hDlg, IDAPPLY), r2.left, r2.top, r2.right, r2.bottom, TRUE);
    MoveWindow(GetDlgItem(hDlg, IDREVERT), r3.left, r3.top, r3.right, r3.bottom, TRUE);
  }
  return (resize0 || resize1);
}

static BOOL CALLBACK MainMenuDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static bool runtime;
  static BOOL optedit = FALSE;
  static edit_opts_t *opts_ptr;
  static int optnum = -1;
  static bool dlg_resized = false;
  int choice, code, i, id;
  UINT ret = 0;
  bx_list_c *list;
  bx_param_filename_c *rcfile;
  char path[BX_PATHNAME_LEN];

  switch (msg) {
    case WM_INITDIALOG:
      runtime = (bool)lParam;
      opts_ptr = NULL;
      EnableWindow(GetDlgItem(hDlg, IDEDITCFG), FALSE);
      if (runtime) {
        SetWindowText(hDlg, "Bochs Runtime Menu");
        EnableWindow(GetDlgItem(hDlg, IDREADRC), FALSE);
        EnableWindow(GetDlgItem(hDlg, IDRESETCFG), FALSE);
        EnableWindow(GetDlgItem(hDlg, IDRESTORE), FALSE);
        SetWindowText(GetDlgItem(hDlg, IDOK), "&Continue");
        i = 0;
        while (runtime_options[i].label != NULL) {
          SendMessage(GetDlgItem(hDlg, IDEDITBOX), LB_ADDSTRING, 0, (LPARAM)runtime_options[i].label);
          runtime_options[i].init = false;
          i++;
        }
        choice = IDOK;
      } else {
        i = 0;
        while (start_options[i].label != NULL) {
          SendMessage(GetDlgItem(hDlg, IDEDITBOX), LB_ADDSTRING, 0, (LPARAM)start_options[i].label);
          i++;
        }
        if (SIM->get_param_enum(BXPN_BOCHS_START)->get() == BX_LOAD_START) {
          choice = IDREADRC;
        } else {
          choice = IDOK;
        }
      }
      SetFocus(GetDlgItem(hDlg, choice));
//      CreateParamDlgTooltip(hDlg);
      return FALSE;
    case WM_CLOSE:
      if (runtime) {
        EndDialog(hDlg, 1);
      } else {
        bx_user_quit = 1;
        EndDialog(hDlg, -1);
      }
      break;
    case WM_COMMAND:
      id = LOWORD(wParam);
      code = HIWORD(wParam);
      switch (id) {
        case IDREADRC:
          rcfile = new bx_param_filename_c(NULL, "rcfile", "Load Bochs Config File",
                                           "", "bochsrc.bxrc", BX_PATHNAME_LEN);
          rcfile->set_extension("bxrc");
          if (AskFilename(hDlg, rcfile, NULL) > 0) {
            SIM->reset_all_param();
            SIM->read_rc(rcfile->getptr());
            i = 0;
            while (start_options[i].label != NULL) {
              if (start_options[i].init) {
                if (i == 0) {
                  InitPluginCtrl(hDlg);
                } else if (i == 2) {
                  HandleLogOptions(hDlg, WM_COMMAND, IDREVERT, 0);
                } else {
                  list = (bx_list_c*)SIM->get_param(opts_ptr->param);
                  ret = HandleChildWindowEvents(hDlg, optnum, IDREVERT, 0, list);
                }
              }
              i++;
            }
          }
          delete rcfile;
          break;
        case IDWRITERC:
          rcfile = new bx_param_filename_c(NULL, "rcfile", "Save Bochs Config File",
                                           "", "bochsrc.bxrc", BX_PATHNAME_LEN);
          rcfile->set_extension("bxrc");
          rcfile->set_options(rcfile->SAVE_FILE_DIALOG);
          if (AskFilename(hDlg, rcfile, NULL) > 0) {
            SIM->write_rc(rcfile->getptr(), 1);
          }
          delete rcfile;
          break;
        case IDEDITBOX:
          if ((code == LBN_SELCHANGE) ||
              (code == LBN_DBLCLK)) {
            if (opts_ptr != NULL) {
              if (!lstrcmp(opts_ptr->param, "#plugins")) {
                ShowPluginCtrl(hDlg, false);
              } else if (!lstrcmp(opts_ptr->param, "#logopts")) {
                ShowLogOptions(hDlg, false);
              } else {
                ShowWindow(GetDlgItem(hDlg, IDNOPARATXT), SW_HIDE);
                list = (bx_list_c*)SIM->get_param(opts_ptr->param);
                if (list != NULL) {
                  ShowParamList(hDlg, 0, false, list);
                  ShowWindow(GetDlgItem(hDlg, IDAPPLY), SW_HIDE);
                  ShowWindow(GetDlgItem(hDlg, IDREVERT), SW_HIDE);
                  EnableWindow(GetDlgItem(hDlg, IDREVERT), TRUE);
                }
              }
            } else {
              ShowWindow(GetDlgItem(hDlg, IDNOPARATXT), SW_HIDE);
            }
            optnum = SendMessage(GetDlgItem(hDlg, IDEDITBOX), LB_GETCURSEL, 0, 0);
            if (runtime) {
              opts_ptr = &runtime_options[optnum];
            } else {
              opts_ptr = &start_options[optnum];
            }
            if (!lstrcmp(opts_ptr->param, "#plugins")) {
              if (!opts_ptr->init) {
                InitPluginCtrl(hDlg);
                opts_ptr->init = true;
              }
              EnableWindow(GetDlgItem(hDlg, IDEDITCFG), FALSE);
              if (dlg_resized) {
                dlg_resized = ResizeDialog(hDlg, opts_ptr->size, true);
              }
              ShowPluginCtrl(hDlg, true);
              break;
            } else if (!lstrcmp(opts_ptr->param, "#logopts")) {
              if (!opts_ptr->init) {
                HandleLogOptions(hDlg, WM_INITDIALOG, wParam, (LPARAM)runtime);
                opts_ptr->init = true;
              }
              EnableWindow(GetDlgItem(hDlg, IDEDITCFG), FALSE);
              if (dlg_resized) {
                dlg_resized = ResizeDialog(hDlg, opts_ptr->size, true);
              }
              ShowLogOptions(hDlg, true);
              break;
            } else if (opts_ptr->newdlg) {
              list = (bx_list_c*)SIM->get_param(opts_ptr->param);
              if (!opts_ptr->init) {
                if (list != NULL) {
                  if (list->get_size() > 0) {
                    opts_ptr->size = CreateParamList(hDlg, optnum, 0, 200, 15, false, list, 0);
                    opts_ptr->init = true;
                  }
                }
              }
              if (list != NULL) {
                dlg_resized = ResizeDialog(hDlg, opts_ptr->size, dlg_resized);
                SetWindowText(GetDlgItem(hDlg, IDOPTGRP), list->get_title());
                if (list->get_size() > 0) {
                  ShowWindow(GetDlgItem(hDlg, IDNOPARATXT), SW_HIDE);
                  ShowParamList(hDlg, 0, true, list);
                  ShowWindow(GetDlgItem(hDlg, IDAPPLY), SW_NORMAL);
                  ShowWindow(GetDlgItem(hDlg, IDREVERT), SW_NORMAL);
                  EnableWindow(GetDlgItem(hDlg, IDAPPLY), FALSE);
                  EnableWindow(GetDlgItem(hDlg, IDREVERT), FALSE);
                } else {
                  SetWindowText(GetDlgItem(hDlg, IDNOPARATXT), "Nothing to configure in this section !");
                  ShowWindow(GetDlgItem(hDlg, IDNOPARATXT), SW_NORMAL);
                }
              }
              EnableWindow(GetDlgItem(hDlg, IDEDITCFG), FALSE);
              break;
            } else {
              dlg_resized = ResizeDialog(hDlg, opts_ptr->size, dlg_resized);
              ShowWindow(GetDlgItem(hDlg, IDNOPARATXT), SW_NORMAL);
              list = (bx_list_c*)SIM->get_param(opts_ptr->param);
              if (list != NULL) {
                SetWindowText(GetDlgItem(hDlg, IDOPTGRP), list->get_title());
                if (list->get_size() > 0) {
                  SetWindowText(GetDlgItem(hDlg, IDNOPARATXT), "Press EDIT button to configure !");
                  EnableWindow(GetDlgItem(hDlg, IDEDITCFG), TRUE);
                } else {
                  SetWindowText(GetDlgItem(hDlg, IDNOPARATXT), "Nothing to configure in this section !");
                  EnableWindow(GetDlgItem(hDlg, IDEDITCFG), FALSE);
                }
              }
            }
          }
          if (code != LBN_DBLCLK) {
            break;
          }
        case IDEDITCFG:
          if (opts_ptr == NULL)
            break;
          list = (bx_list_c*)SIM->get_param(opts_ptr->param);
          if (list != NULL) {
            if (list->get_size() > 0) {
              win32ParamDialog(hDlg, opts_ptr->param);
            } else {
              MessageBox(hDlg, "Nothing to configure in this section", "Warning", MB_ICONEXCLAMATION);
            }
          } else {
            MessageBox(hDlg, "Nothing to configure in this section", "Warning", MB_ICONEXCLAMATION);
          }
          break;
        case IDRESETCFG:
          if (MessageBox(hDlg, "Reset all options back to their factory defaults ?",
                         "Reset Configuration", MB_ICONEXCLAMATION | MB_YESNO) == IDYES) {
            SIM->reset_all_param();
          }
          break;
        case IDRESTORE:
          path[0] = 0;
          if (BrowseDir("Restore Bochs state from...", path) >= 0) {
            SetCurrentDirectory(path);
            SIM->reset_all_param();
            SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(1);
            SIM->get_param_string(BXPN_RESTORE_PATH)->set(path);
            if (!SIM->restore_config()) {
              MessageBox(hDlg, "Cannot restore configuration!", "ERROR", MB_ICONERROR);
            }
            EndDialog(hDlg, 1);
          }
          break;
        case IDOK:
          if (runtime) {
            SIM->update_runtime_options();
          }
          EndDialog(hDlg, 1);
          break;
        case IDCANCEL:
          if (optedit) {
            PostMessage(hDlg, WM_COMMAND, (WPARAM)IDREVERT, 0);
            break;
          } else if (runtime) {
            EndDialog(hDlg, 1);
            break;
          }
        case IDQUIT:
          bx_user_quit = 1;
          EndDialog(hDlg, -1);
          break;
        default:
          if (opts_ptr != NULL) {
            if (!lstrcmp(opts_ptr->param, "#plugins")) {
              HandlePluginCtrl(hDlg, msg, wParam, lParam);
            } else if (!lstrcmp(opts_ptr->param, "#logopts")) {
              ret = HandleLogOptions(hDlg, msg, wParam, lParam);
            } else {
              list = (bx_list_c*)SIM->get_param(opts_ptr->param);
              ret = HandleChildWindowEvents(hDlg, optnum, id, code, list);
            }
            if (ret != 0) {
              optedit = (ret == BXPD_EDIT_START);
              EnableWindow(GetDlgItem(hDlg, IDEDITBOX), !optedit);
              EnableWindow(GetDlgItem(hDlg, IDWRITERC), !optedit);
              EnableWindow(GetDlgItem(hDlg, IDOK), !optedit);
              if (!runtime) {
                EnableWindow(GetDlgItem(hDlg, IDREADRC), !optedit);
                EnableWindow(GetDlgItem(hDlg, IDRESETCFG), !optedit);
                EnableWindow(GetDlgItem(hDlg, IDRESTORE), !optedit);
              }
            }
          }
      }
      break;
    case WM_NOTIFY:
      OnWMNotify(hDlg, optnum, lParam);
      break;
    case WM_CTLCOLOREDIT:
      SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
      return (INT_PTR)GetSysColorBrush(COLOR_WINDOW);
  }
  return FALSE;
}

void LogAskDialog(BxEvent *event)
{
  event->retcode = (Bit32s) DialogBoxParam(NULL, MAKEINTRESOURCE(ASK_DLG), GetBochsWindow(),
                                  (DLGPROC)LogAskProc, (LPARAM)event);
}

int AskString(bx_param_string_c *param)
{
  return (int) DialogBoxParam(NULL, MAKEINTRESOURCE(STRING_DLG), GetBochsWindow(),
                        (DLGPROC)StringParamProc, (LPARAM)param);
}

int MainMenuDialog(HWND hwnd, bool runtime)
{
  InitParamDialog(true);
  int ret = (int) DialogBoxParam(NULL, MAKEINTRESOURCE(MAINMENU_DLG), hwnd,
                        (DLGPROC)MainMenuDlgProc, (LPARAM)runtime);
  CleanupParamDialog(NULL);
  return ret;
}

#define ML_WIN_WIDTH  400
#define ML_WIN_HEIGHT 150

// Unfortunetly, Win32 Edit Controls must have \r\n for line feeds, and Bochs uses only \n
// We must add \r to every \n to get Windows to drop to the next line in an Edit Control.
void conv_dos2unix(const char *src, char *targ, int max_len)
{
  char last = 0;
  while (*src && (max_len > 2)) {
    if ((*src == '\n') && (last != '\r')) {
      *targ++ = '\r';
      max_len--;
    }
    *targ++ = last = *src++;
    max_len--;
  }
  *targ = '\0';
}

BxEvent* win32_notify_callback(void *unused, BxEvent *event)
{
  int opts, x = -1, y = -1;
  bx_param_c *param;
  bx_param_string_c *sparam;
  char pname[BX_PATHNAME_LEN];
  NONCLIENTMETRICS ncm;
  RECT rect;
  HWND hwnd;

  event->retcode = -1;
  switch (event->type)
  {
    case BX_SYNC_EVT_LOG_DLG:
      LogAskDialog(event);
      return event;
    case BX_SYNC_EVT_MSG_BOX:
      MessageBox(GetBochsWindow(), event->u.logmsg.msg, event->u.logmsg.prefix, MB_ICONERROR);
      return event;
    case BX_SYNC_EVT_ML_MSG_BOX:
      // get the coordinates of the screen size
      if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0)) {
        x = (rect.right - rect.left - ML_WIN_WIDTH) / 2;
        y = (rect.bottom - rect.top - ML_WIN_HEIGHT) / 2;
      }
      // if that failed or the screen isn't very big...
      if (x < 0) x = 250;
      if (y < 0) y = 250;
      // create an EDIT window, with the edit control as read only
      hwnd = CreateWindow("EDIT", event->u.logmsg.prefix,  
        WS_VISIBLE | WS_BORDER | ES_READONLY | ES_LEFT | ES_MULTILINE,
        x, y, ML_WIN_WIDTH, ML_WIN_HEIGHT, GetBochsWindow(), (HMENU) NULL, NULL, (LPVOID) NULL);
      // if we created the window successfully, change the font to the system font and replace the text
      if (hwnd) {
        ncm.cbSize = sizeof(NONCLIENTMETRICS);
        if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
          SendMessage(hwnd, WM_SETFONT, (WPARAM) CreateFontIndirect(&ncm.lfMessageFont), 0);
        conv_dos2unix(event->u.logmsg.msg, pname, BX_PATHNAME_LEN);
        SendMessage(hwnd, EM_SETSEL, 0, -1);
        SendMessage(hwnd, EM_REPLACESEL, 0, (LPARAM) pname);
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        event->param0 = hwnd;
      }
      return event;
    case BX_SYNC_EVT_ML_MSG_BOX_KILL:
      if (event->param0) {
        DestroyWindow((HWND) event->param0);
        event->param0 = NULL;
      }
      return event;
    case BX_SYNC_EVT_ASK_PARAM:
      param = event->u.param.param;
      if (param->get_type() == BXT_PARAM_STRING || param->get_type() == BXT_PARAM_BYTESTRING) {
        sparam = (bx_param_string_c *)param;
        opts = sparam->get_options();
        if (opts & sparam->IS_FILENAME) {
          if (opts & sparam->SELECT_FOLDER_DLG) {
            event->retcode = BrowseDir(sparam->get_label(), sparam->getptr());
          } else {
            event->retcode = AskFilename(GetBochsWindow(), (bx_param_filename_c *)sparam, NULL);
          }
          return event;
        } else {
          event->retcode = AskString(sparam);
          return event;
        }
      } else if (param->get_type() == BXT_LIST) {
        param->get_param_path(pname, BX_PATHNAME_LEN);
        if (!strncmp(pname, "floppy", 6)) {
          event->retcode = (Bit32s) win32FloppyParamDialog(GetBochsWindow(), pname);
        } else {
          event->retcode = (Bit32s) win32ParamDialog(GetBochsWindow(), pname);
        }
        return event;
      } else if (param->get_type() == BXT_PARAM_BOOL) {
        UINT flag = MB_YESNO | MB_SETFOREGROUND;
        if (((bx_param_bool_c *)param)->get() == 0) {
          flag |= MB_DEFBUTTON2;
        }
        ((bx_param_bool_c *)param)->set(MessageBox(GetActiveWindow(), param->get_description(), param->get_label(), flag) == IDYES);
        event->retcode = 0;
        return event;
      }
    case BX_SYNC_EVT_TICK: // called periodically by siminterface.
      event->retcode = 0;
      // fall into default case
    default:
      return event;
  }
}

static int win32_ci_callback(void *userdata, ci_command_t command)
{
  switch (command)
  {
    case CI_START:
      if (SIM->get_param_enum(BXPN_BOCHS_START)->get() == BX_QUICK_START) {
        SIM->begin_simulation(bx_startup_flags.argc, bx_startup_flags.argv);
        // we don't expect it to return, but if it does, quit
        SIM->quit_sim(1);
      } else {
        if (MainMenuDialog(GetActiveWindow(), 0) == 1) {
          SIM->begin_simulation(bx_startup_flags.argc, bx_startup_flags.argv);
        }
        SIM->quit_sim(1);
      }
      break;
    case CI_RUNTIME_CONFIG:
      if (!bx_gui->has_gui_console()) {
        if (MainMenuDialog(GetBochsWindow(), 1) < 0) {
          bx_user_quit = 1;
        } else {
          return 1;
        }
      }
      break;
    case CI_SHUTDOWN:
      break;
  }
  return 0;
}

#endif // BX_USE_WIN32CONFIG
