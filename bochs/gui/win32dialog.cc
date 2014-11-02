/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003-2014  The Bochs Project
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
#include "param_names.h"
#include "win32res.h"
#include "win32paramdlg.h"

const char log_choices[5][16] = {"ignore", "log", "ask user", "end simulation", "no change"};

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
      SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_ADDSTRING, 0, (LPARAM)"Kill simulation");
      SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_ADDSTRING, 0, (LPARAM)"Abort (dump core)");
#if BX_DEBUGGER
      SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_ADDSTRING, 0, (LPARAM)"Continue and return to debugger");
#endif
      SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_SETCURSEL, 2, 0);
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

void InitLogOptionsDialog(HWND hDlg, BOOL advanced)
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

static BOOL CALLBACK LogOptDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL advanced;
  static BOOL changed;
  long noticode;

  switch (msg) {
    case WM_INITDIALOG:
      advanced = (BOOL)lParam;
      InitLogOptionsDialog(hDlg, advanced);
      changed = FALSE;
      EnableWindow(GetDlgItem(hDlg, IDAPPLY), FALSE);
      return TRUE;
    case WM_CLOSE:
      EndDialog(hDlg, 0);
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
              ApplyLogOptions(hDlg, advanced);
              EnableWindow(GetDlgItem(hDlg, IDAPPLY), FALSE);
              changed = FALSE;
              break;
            case IDOK:
              if (changed) {
                ApplyLogOptions(hDlg, advanced);
              }
              EndDialog(hDlg, 1);
              break;
            case IDCANCEL:
              EndDialog(hDlg, 0);
              break;
          }
      }
      break;
  }
  return FALSE;
}

void LogOptionsDialog(HWND hwnd, bx_bool runtime)
{
  DialogBoxParam(NULL, MAKEINTRESOURCE(LOGOPT_DLG), hwnd, (DLGPROC)LogOptDlgProc, (LPARAM)runtime);
}

static BOOL CALLBACK PluginCtrlDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  int count, i;
  long code;
  bx_list_c *plugin_ctrl;
  char plugname[20], message[80];

  switch (msg) {
    case WM_INITDIALOG:
      plugin_ctrl = (bx_list_c*) SIM->get_param(BXPN_PLUGIN_CTRL);
      count = plugin_ctrl->get_size();
      for (i = 0; i < count; i++) {
        SendMessage(GetDlgItem(hDlg, IDPLUGLIST), LB_ADDSTRING, 0, (LPARAM)plugin_ctrl->get(i)->get_name());
      }
      EnableWindow(GetDlgItem(hDlg, IDLOAD), FALSE);
      EnableWindow(GetDlgItem(hDlg, IDUNLOAD), FALSE);
      return TRUE;
    case WM_CLOSE:
      EndDialog(hDlg, 0);
      break;
    case WM_COMMAND:
      code = HIWORD(wParam);
      switch (LOWORD(wParam)) {
        case IDPLUGLIST:
          if (code == LBN_SELCHANGE) {
            EnableWindow(GetDlgItem(hDlg, IDUNLOAD), TRUE);
          }
          break;
        case IDEDIT:
          if (code == EN_CHANGE) {
            i = GetWindowTextLength(GetDlgItem(hDlg, IDEDIT));
            EnableWindow(GetDlgItem(hDlg, IDLOAD), i > 0);
          }
          break;
        case IDLOAD:
          GetDlgItemText(hDlg, IDEDIT, plugname, 18);
          if (SIM->opt_plugin_ctrl(plugname, 1)) {
            wsprintf(message, "Plugin '%s' loaded", plugname);
            MessageBox(hDlg, message, "Plugin Control", MB_ICONINFORMATION);
            SendMessage(GetDlgItem(hDlg, IDPLUGLIST), LB_ADDSTRING, 0, (LPARAM)plugname);
          }
          break;
        case IDUNLOAD:
          i = SendMessage(GetDlgItem(hDlg, IDPLUGLIST), LB_GETCURSEL, 0, 0);
          SendMessage(GetDlgItem(hDlg, IDPLUGLIST), LB_GETTEXT, i, (LPARAM)plugname);
          if (SIM->opt_plugin_ctrl(plugname, 0)) {
            wsprintf(message, "Plugin '%s' unloaded", plugname);
            MessageBox(hDlg, message, "Plugin Control", MB_ICONINFORMATION);
            SendMessage(GetDlgItem(hDlg, IDPLUGLIST), LB_DELETESTRING, i, 0);
            EnableWindow(GetDlgItem(hDlg, IDUNLOAD), FALSE);
          }
          break;
        case IDOK:
          EndDialog(hDlg, 1);
          break;
      }
      break;
  }
  return FALSE;
}

void PluginCtrlDialog(HWND hwnd)
{
  DialogBox(NULL, MAKEINTRESOURCE(PLUGIN_CTRL_DLG), hwnd, (DLGPROC)PluginCtrlDlgProc);
}

typedef struct {
  const char *label;
  const char *param;
} edit_opts_t;

edit_opts_t start_options[] = {
  {"Plugin Control", "#plugins"},
  {"Logfile", "log"},
  {"Log Options", "#logopts"},
  {"CPU", "cpu"},
#if BX_CPU_LEVEL >= 4
  {"CPUID", "cpuid"},
#endif
  {"Memory", "memory"},
  {"Clock & CMOS", "clock_cmos"},
  {"PCI", "pci"},
  {"Display & Interface", "display"},
  {"Keyboard & Mouse", "keyboard_mouse"},
  {"Disk & Boot", BXPN_MENU_DISK_WIN32},
  {"Serial / Parallel / USB", "ports"},
  {"Network card", "network"},
  {"Sound card", "sound"},
  {"Other", "misc"},
#if BX_PLUGINS
  {"User-defined Options", "user"},
#endif
  {NULL, NULL}
};

edit_opts_t runtime_options[] = {
  {"CD-ROM", BXPN_MENU_RUNTIME_CDROM},
  {"USB", BXPN_MENU_RUNTIME_USB},
  {"Misc", BXPN_MENU_RUNTIME_MISC},
  {"Log Options", "#logopts"},
  {NULL, NULL}
};
static BOOL CALLBACK MainMenuDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static bx_bool runtime;
  int choice, code, i;
  bx_param_filename_c *rcfile;
  char path[BX_PATHNAME_LEN];
  const char *pname;

  switch (msg) {
    case WM_INITDIALOG:
      runtime = (bx_bool)lParam;
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
      code = HIWORD(wParam);
      switch (LOWORD(wParam)) {
        case IDREADRC:
          rcfile = new bx_param_filename_c(NULL, "rcfile", "Load Bochs Config File",
                                           "", "bochsrc.bxrc", BX_PATHNAME_LEN);
          rcfile->set_extension("bxrc");
          if (AskFilename(hDlg, rcfile, NULL) > 0) {
            SIM->reset_all_param();
            SIM->read_rc(rcfile->getptr());
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
            EnableWindow(GetDlgItem(hDlg, IDEDITCFG), TRUE);
          }
          if (code != LBN_DBLCLK) {
            break;
          }
        case IDEDITCFG:
          i = SendMessage(GetDlgItem(hDlg, IDEDITBOX), LB_GETCURSEL, 0, 0);
          if (runtime) {
            pname = runtime_options[i].param;
          } else {
            pname = start_options[i].param;
          }
          if (pname[0] != '#') {
            if (((bx_list_c*)SIM->get_param(pname))->get_size() > 0) {
              win32ParamDialog(hDlg, pname);
            } else {
              MessageBox(hDlg, "Nothing to configure in this section", "Warning", MB_ICONEXCLAMATION);
            }
          } else {
            if (!lstrcmp(pname, "#logopts")) {
              LogOptionsDialog(hDlg, runtime);
            } else if (!lstrcmp(pname, "#plugins")) {
              PluginCtrlDialog(hDlg);
            } else {
              MessageBox(hDlg, "Unknown keyword", "Warning", MB_ICONEXCLAMATION);
            }
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
            SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(1);
            SIM->get_param_string(BXPN_RESTORE_PATH)->set(path);
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
          if (runtime) {
            EndDialog(hDlg, 1);
            break;
          }
        case IDQUIT:
          bx_user_quit = 1;
          EndDialog(hDlg, -1);
          break;
      }
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

int MainMenuDialog(HWND hwnd, bx_bool runtime)
{
  return (int) DialogBoxParam(NULL, MAKEINTRESOURCE(MAINMENU_DLG), hwnd,
                        (DLGPROC)MainMenuDlgProc, (LPARAM)runtime);
}

BxEvent* win32_notify_callback(void *unused, BxEvent *event)
{
  int opts;
  bx_param_c *param;
  bx_param_string_c *sparam;
  char pname[BX_PATHNAME_LEN];

  event->retcode = -1;
  switch (event->type)
  {
    case BX_SYNC_EVT_LOG_ASK:
      LogAskDialog(event);
      return event;
    case BX_SYNC_EVT_ASK_PARAM:
      param = event->u.param.param;
      if (param->get_type() == BXT_PARAM_STRING) {
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
      SIM->set_notify_callback(win32_notify_callback, NULL);
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
      if (MainMenuDialog(GetBochsWindow(), 1) < 0) {
        bx_user_quit = 1;
#if !BX_DEBUGGER
        bx_atexit();
        SIM->quit_sim(1);
#else
        bx_dbg_exit(1);
#endif
        return -1;
      }
      break;
    case CI_SHUTDOWN:
      break;
  }
  return 0;
}

int init_win32_config_interface()
{
  SIM->register_configuration_interface("win32config", win32_ci_callback, NULL);
  return 0;  // success
}

#endif // BX_USE_WIN32CONFIG
