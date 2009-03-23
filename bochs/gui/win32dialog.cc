/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.cc,v 1.81 2009-03-23 19:05:16 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009  The Bochs Project
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
#include "win32paramdlg.h"

const char log_choices[5][16] = {"ignore", "log", "ask user", "end simulation", "no change"};
static int retcode = 0;
static bxevent_handler old_callback = NULL;
static void *old_callback_arg = NULL;
#if BX_DEBUGGER
extern char *debug_cmd;
extern bx_bool debug_cmd_ready;
extern bx_bool vgaw_refresh;
#endif

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

HWND GetBochsWindow()
{
  HWND hwnd;

  hwnd = FindWindow("Bochs for Windows", NULL);
  if (hwnd == NULL) {
    hwnd = GetForegroundWindow();
  }
  return hwnd;
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
  browseInfo.hwndOwner = GetActiveWindow();
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
      break;
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

static BOOL CALLBACK FloppyDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static bx_param_filename_c *param;
  static bx_param_enum_c *status;
  static bx_param_enum_c *devtype;
  static bx_param_enum_c *mediatype;
  static char origpath[MAX_PATH];
  char mesg[MAX_PATH];
  char path[MAX_PATH];
  char pname[80];
  const char *title;
  int i, cap;

  switch (msg) {
    case WM_INITDIALOG:
      param = (bx_param_filename_c *)lParam;
      param->get_param_path(pname, 80);
      if (!strcmp(pname, BXPN_FLOPPYA_PATH)) {
        status = SIM->get_param_enum(BXPN_FLOPPYA_STATUS);
        devtype = SIM->get_param_enum(BXPN_FLOPPYA_DEVTYPE);
        mediatype = SIM->get_param_enum(BXPN_FLOPPYA_TYPE);
      } else {
        status = SIM->get_param_enum(BXPN_FLOPPYB_STATUS);
        devtype = SIM->get_param_enum(BXPN_FLOPPYB_DEVTYPE);
        mediatype = SIM->get_param_enum(BXPN_FLOPPYB_TYPE);
      }
      cap = devtype->get() - (int)devtype->get_min();
      SetWindowText(GetDlgItem(hDlg, IDDEVTYPE), floppy_devtype_names[cap]);
      i = 0;
      while (floppy_type_names[i] != NULL) {
        SendMessage(GetDlgItem(hDlg, IDMEDIATYPE), CB_ADDSTRING, 0, (LPARAM)floppy_type_names[i]);
        SendMessage(GetDlgItem(hDlg, IDMEDIATYPE), CB_SETITEMDATA, i, (LPARAM)(mediatype->get_min() + i));
        i++;
      }
      cap = mediatype->get() - (int)mediatype->get_min();
      SendMessage(GetDlgItem(hDlg, IDMEDIATYPE), CB_SETCURSEL, cap, 0);
      if (status->get() == BX_INSERTED) {
        SendMessage(GetDlgItem(hDlg, IDSTATUS), BM_SETCHECK, BST_CHECKED, 0);
      }
      lstrcpy(origpath, param->getptr());
      title = param->get_label();
      if (!title) title = param->get_name();
      SetWindowText(hDlg, title);
      if (lstrlen(origpath) && lstrcmp(origpath, "none")) {
        SetWindowText(GetDlgItem(hDlg, IDPATH), origpath);
      }
      return TRUE;
      break;
    case WM_CLOSE:
      GetDlgItemText(hDlg, IDPATH, path, MAX_PATH);
      if (lstrcmp(path, origpath)) {
        param->set(origpath);
      }
      EndDialog(hDlg, -1);
      return TRUE;
      break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDBROWSE:
          GetDlgItemText(hDlg, IDPATH, path, MAX_PATH);
          param->set(backslashes(path));
          if (AskFilename(hDlg, param) > 0) {
            SetWindowText(GetDlgItem(hDlg, IDPATH), param->getptr());
            SendMessage(GetDlgItem(hDlg, IDSTATUS), BM_SETCHECK, BST_CHECKED, 0);
            SendMessage(GetDlgItem(hDlg, IDMEDIATYPE), CB_SELECTSTRING, (WPARAM)-1, (LPARAM)"auto");
            EnableWindow(GetDlgItem(hDlg, IDCREATE), FALSE);
          }
          return TRUE;
          break;
        case IDOK:
          status->set(BX_EJECTED);
          if (SendMessage(GetDlgItem(hDlg, IDSTATUS), BM_GETCHECK, 0, 0) == BST_CHECKED) {
            GetDlgItemText(hDlg, IDPATH, path, MAX_PATH);
            if (lstrlen(path) == 0) {
              lstrcpy(path, "none");
            }
          } else {
            lstrcpy(path, "none");
          }
          param->set(path);
          i = SendMessage(GetDlgItem(hDlg, IDMEDIATYPE), CB_GETCURSEL, 0, 0);
          cap = SendMessage(GetDlgItem(hDlg, IDMEDIATYPE), CB_GETITEMDATA, i, 0);
          mediatype->set(cap);
          if (lstrcmp(path, "none")) {
            status->set(BX_INSERTED);
          }
          EndDialog(hDlg, 1);
          return TRUE;
          break;
        case IDCANCEL:
          GetDlgItemText(hDlg, IDPATH, path, MAX_PATH);
          if (lstrcmp(path, origpath)) {
            param->set(origpath);
          }
          EndDialog(hDlg, -1);
          return TRUE;
          break;
        case IDMEDIATYPE:
          if (HIWORD(wParam) == CBN_SELCHANGE) {
            i = SendMessage(GetDlgItem(hDlg, IDMEDIATYPE), CB_GETCURSEL, 0, 0);
            EnableWindow(GetDlgItem(hDlg, IDCREATE), (floppy_type_n_sectors[i] > 0));
          }
          break;
        case IDCREATE:
          GetDlgItemText(hDlg, IDPATH, path, MAX_PATH);
          backslashes(path);
          i = SendMessage(GetDlgItem(hDlg, IDMEDIATYPE), CB_GETCURSEL, 0, 0);
          if (CreateImage(hDlg, floppy_type_n_sectors[i], path)) {
            wsprintf(mesg, "Created a %s disk image called %s", floppy_type_names[i], path);
            MessageBox(hDlg, mesg, "Image created", MB_OK);
          }
          return TRUE;
          break;
      }
  }
  return FALSE;
}

void SetStandardLogOptions(HWND hDlg)
{
  int level, idx;
  int defchoice[5];

  for (level=0; level<5; level++) {
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
  for (level=0; level<5; level++) {
    idx = 0;
    SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_RESETCONTENT, 0, 0);
    for (int action=0; action<5; action++) {
      if (((level > 1) && (action > 0)) || ((level < 2) && ((action < 2) || (action > 3)))) {
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
  for (level=0; level<5; level++) {
    idx = 0;
    SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_RESETCONTENT, 0, 0);
    for (int action=0; action<4; action++) {
      if (((level > 1) && (action > 0)) || ((level < 2) && (action < 2))) {
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

void InitLogOptionsDialog(HWND hDlg)
{
  int idx, mod;
  char prefix[8];

  idx = 0;
  for (mod=0; mod<SIM->get_n_log_modules(); mod++) {
    if (strcmp(SIM->get_prefix(mod), "[     ]")) {
      lstrcpyn(prefix, SIM->get_prefix(mod), sizeof(prefix));
      lstrcpy(prefix, prefix+1);
      prefix[5] = 0;
      SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_ADDSTRING, 0, (LPARAM)prefix);
      SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_SETITEMDATA, idx, mod);
      idx++;
    }
  }
  SetStandardLogOptions(hDlg);
}

void ApplyLogOptions(HWND hDlg, BOOL advanced)
{
  int idx, level, mod, value;

  if (advanced) {
    idx = SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_GETCURSEL, 0, 0);
    mod = SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_GETITEMDATA, idx, 0);
    for (level=0; level<5; level++) {
      idx = SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_GETCURSEL, 0, 0);
      value = SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_GETITEMDATA, idx, 0);
      SIM->set_log_action (mod, level, value);
    }
    EnableWindow(GetDlgItem(hDlg, IDDEVLIST), TRUE);
  } else {
    for (level=0; level<5; level++) {
      idx = SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_GETCURSEL, 0, 0);
      value = SendMessage(GetDlgItem(hDlg, IDLOGEVT1+level), CB_GETITEMDATA, idx, 0);
      if (value < 4) {
        // set new default
        SIM->set_default_log_action (level, value);
        // apply that action to all modules (devices)
        SIM->set_log_action (-1, level, value);
      }
    }
  }
  EnableWindow(GetDlgItem(hDlg, IDADVLOGOPT), TRUE);
}

static BOOL CALLBACK SMLogOptDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL advanced;
  static BOOL changed;
  long noticode;

  switch (msg) {
    case WM_INITDIALOG:
      InitLogOptionsDialog(hDlg);
      advanced = FALSE;
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
              break;
            case IDOK:
              ApplyLogOptions(hDlg, advanced);
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

void SMLogOptionsDialog(HWND hwnd)
{
  DialogBox(NULL, MAKEINTRESOURCE(SM_LOGOPT_DLG), hwnd, (DLGPROC)SMLogOptDlgProc);
}

typedef struct {
  const char *label;
  const char *param;
} edit_opts_t;

edit_opts_t edit_options[] = {
  {"Logfile", "log"},
  {"Log Options", "*"},
  {"CPU", "cpu"},
  {"Memory", "memory"},
  {"Clock & CMOS", "clock_cmos"},
  {"PCI", "pci"},
  {"Display & Interface", "display"},
  {"Keyboard & Mouse", "keyboard_mouse"},
  {"Disk & Boot", BXPN_MENU_DISK_WIN32},
  {"Serial / Parallel / USB", "ports"},
  {"Network card", "network"},
  {"Sound Blaster 16", BXPN_SB16},
  {"Other", "misc"},
#if BX_PLUGINS
  {"User-defined Options", "user"},
#endif
  {NULL, NULL}
};

static BOOL CALLBACK StartMenuDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  int code, i;
  bx_param_filename_c *rcfile;
  char path[BX_PATHNAME_LEN];

  switch (msg) {
    case WM_INITDIALOG:
      EnableWindow(GetDlgItem(hDlg, IDEDITCFG), FALSE);
      i = 0;
      while (edit_options[i].label != NULL) {
        SendMessage(GetDlgItem(hDlg, IDEDITBOX), LB_ADDSTRING, 0, (LPARAM)edit_options[i].label);
        i++;
      }
      return TRUE;
    case WM_CLOSE:
      EndDialog(hDlg, -1);
      break;
    case WM_COMMAND:
      code = HIWORD(wParam);
      switch (LOWORD(wParam)) {
        case IDREADRC:
          rcfile = new bx_param_filename_c(NULL, "rcfile", "Load Bochs Config File",
                                           "", "bochsrc.bxrc", BX_PATHNAME_LEN);
          rcfile->set_extension("bxrc");
          if (AskFilename(hDlg, rcfile)) {
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
          if (AskFilename(hDlg, rcfile)) {
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
          if (lstrcmp(edit_options[i].param, "*")) {
            win32ParamDialog(hDlg, edit_options[i].param);
          } else {
            SMLogOptionsDialog(hDlg);
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
          EndDialog(hDlg, 1);
          break;
        case IDCANCEL:
          EndDialog(hDlg, -1);
          break;
      }
  }
  return FALSE;
}

static BOOL CALLBACK RTCdromDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static int devcount;
  static bx_list_c *cdromop[4];
  static char origpath[4][MAX_PATH];
  static BOOL changed;
  int device;
  long noticode;
  Bit8u cdrom;
  char path[MAX_PATH];
  PSHNOTIFY *psn;

  switch (msg) {
    case WM_INITDIALOG:
      SetForegroundWindow(GetParent(hDlg));
      SetDlgItemText(GetParent(hDlg), IDOK, "Continue");
      SetDlgItemText(GetParent(hDlg), IDCANCEL, "Quit");
      // 4 cdroms supported at run time
      devcount = 1;
      for (cdrom=1; cdrom<4; cdrom++) {
        if (!SIM->get_cdrom_options(cdrom, &cdromop[cdrom], &device) ||
	    !SIM->get_param_bool("present", cdromop[cdrom])->get()) {
          EnableWindow(GetDlgItem(hDlg, IDLABEL1+cdrom), FALSE);
          EnableWindow(GetDlgItem(hDlg, IDCDROM1+cdrom), FALSE);
          EnableWindow(GetDlgItem(hDlg, IDBROWSE1+cdrom), FALSE);
          EnableWindow(GetDlgItem(hDlg, IDSTATUS1+cdrom), FALSE);
        } else {
          lstrcpy(origpath[cdrom], SIM->get_param_string("path", cdromop[cdrom])->getptr());
          if (lstrlen(origpath[cdrom]) && lstrcmp(origpath[cdrom], "none")) {
            SetWindowText(GetDlgItem(hDlg, IDCDROM1+cdrom), origpath[cdrom]);
          }
          if (SIM->get_param_bool("status", cdromop[cdrom])->get()) {
            SendMessage(GetDlgItem(hDlg, IDSTATUS1+cdrom), BM_SETCHECK, BST_CHECKED, 0);
          }
          devcount++;
        }
      }
      changed = FALSE;
      return TRUE;
    case WM_NOTIFY:
      psn = (PSHNOTIFY *)lParam;
      switch(psn->hdr.code) {
        case PSN_APPLY:
          if ((psn->lParam == FALSE) && changed) { // Apply pressed & change in this dialog
            for (device=1; device<devcount; device++) {
              if (SendMessage(GetDlgItem(hDlg, IDSTATUS1+device), BM_GETCHECK, 0, 0) == BST_CHECKED) {
                GetDlgItemText(hDlg, IDCDROM1+device, path, MAX_PATH);
                if (lstrlen(path)) {
                  SIM->get_param_bool("status", cdromop[device])->set(1);
                  if (lstrcmp(path, SIM->get_param_string("path", cdromop[device])->getptr())) {
                    SIM->get_param_string("path", cdromop[device])->set(path);
                  }
                } else {
                  SIM->get_param_bool("status", cdromop[device])->set(0);
                }
              } else {
                SIM->get_param_bool("status", cdromop[device])->set(0);
              }
            }
            changed = FALSE;
          } else {
            if (changed) {
              for (device=1; device<devcount; device++) {
                if (lstrcmp(SIM->get_param_string("path", cdromop[device])->getptr(), origpath[device])) {
                  SIM->get_param_string("path", cdromop[device])->set(origpath[device]);
                }
              }
            }
            retcode = BX_CI_RT_CONT;
          }
          return PSNRET_NOERROR;
        case PSN_QUERYCANCEL:
          retcode = BX_CI_RT_QUIT;
          return TRUE;
      }
      break;
    case WM_COMMAND:
      noticode = HIWORD(wParam);
      switch(noticode) {
        case EN_CHANGE:
          switch (LOWORD(wParam)) {
            case IDCDROM2:
            case IDCDROM3:
            case IDCDROM4:
              changed = TRUE;
              SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
              break;
          }
          break;
        default:
          switch (LOWORD(wParam)) {
            case IDBROWSE2:
            case IDBROWSE3:
            case IDBROWSE4:
              device = LOWORD(wParam) - IDBROWSE1;
              GetDlgItemText(hDlg, IDCDROM1+device, path, MAX_PATH);
              SIM->get_param_string("path", cdromop[device])->set(backslashes(path));
              if (AskFilename(hDlg, (bx_param_filename_c *)SIM->get_param_string("path", cdromop[device])) > 0) {
                SetWindowText(GetDlgItem(hDlg, IDCDROM1+device), SIM->get_param_string("path", cdromop[device])->getptr());
                SendMessage(GetDlgItem(hDlg, IDSTATUS1+device), BM_SETCHECK, BST_CHECKED, 0);
              }
              break;
            case IDSTATUS2:
            case IDSTATUS3:
            case IDSTATUS4:
              changed = TRUE;
              SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
              break;
          }
      }
      break;
  }
  return FALSE;
}

static BOOL CALLBACK RTUSBdevDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL changed;
  long noticode;
  char buffer[MAX_PATH];
  PSHNOTIFY *psn;

  switch (msg) {
    case WM_INITDIALOG:
      if (((bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB))->get_by_name("uhci")) {
        SetDlgItemText(hDlg, IDUHCIDEV1, SIM->get_param_string(BXPN_UHCI_PORT1)->getptr());
        SetDlgItemText(hDlg, IDUHCIDEV2, SIM->get_param_string(BXPN_UHCI_PORT2)->getptr());
      } else {
        EnableWindow(GetDlgItem(hDlg, IDUHCILBL1), FALSE);
        EnableWindow(GetDlgItem(hDlg, IDUHCILBL2), FALSE);
        EnableWindow(GetDlgItem(hDlg, IDUHCIDEV1), FALSE);
        EnableWindow(GetDlgItem(hDlg, IDUHCIDEV2), FALSE);
      }
      if (((bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB))->get_by_name("ohci")) {
        SetDlgItemText(hDlg, IDOHCIDEV1, SIM->get_param_string(BXPN_OHCI_PORT1)->getptr());
        SetDlgItemText(hDlg, IDOHCIDEV2, SIM->get_param_string(BXPN_OHCI_PORT2)->getptr());
      } else {
        EnableWindow(GetDlgItem(hDlg, IDOHCILBL1), FALSE);
        EnableWindow(GetDlgItem(hDlg, IDOHCILBL2), FALSE);
        EnableWindow(GetDlgItem(hDlg, IDOHCIDEV1), FALSE);
        EnableWindow(GetDlgItem(hDlg, IDOHCIDEV2), FALSE);
      }
      if (!((bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB))->get_by_name("exthub1")) {
        EnableWindow(GetDlgItem(hDlg, IDEXTHUB1), FALSE);
      }
      changed = FALSE;
      return TRUE;
    case WM_NOTIFY:
      psn = (PSHNOTIFY *)lParam;
      switch(psn->hdr.code) {
        case PSN_APPLY:
          if ((psn->lParam == FALSE) && changed) { // Apply pressed & change in this dialog
            if (SendMessage(GetDlgItem(hDlg, IDUHCIDEV1), EM_GETMODIFY, 0, 0)) {
              GetDlgItemText(hDlg, IDUHCIDEV1, buffer, sizeof(buffer));
              SIM->get_param_string(BXPN_UHCI_PORT1)->set(buffer);
            }
            if (SendMessage(GetDlgItem(hDlg, IDUHCIDEV2), EM_GETMODIFY, 0, 0)) {
              GetDlgItemText(hDlg, IDUHCIDEV2, buffer, sizeof(buffer));
              SIM->get_param_string(BXPN_UHCI_PORT2)->set(buffer);
            }
            if (SendMessage(GetDlgItem(hDlg, IDOHCIDEV1), EM_GETMODIFY, 0, 0)) {
              GetDlgItemText(hDlg, IDOHCIDEV1, buffer, sizeof(buffer));
              SIM->get_param_string(BXPN_OHCI_PORT1)->set(buffer);
            }
            if (SendMessage(GetDlgItem(hDlg, IDOHCIDEV2), EM_GETMODIFY, 0, 0)) {
              GetDlgItemText(hDlg, IDOHCIDEV2, buffer, sizeof(buffer));
              SIM->get_param_string(BXPN_OHCI_PORT2)->set(buffer);
            }
            EnableWindow(GetDlgItem(hDlg, IDEXTHUB1),
              (((bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB))->get_by_name("exthub1") != NULL));
          }
          return PSNRET_NOERROR;
        case PSN_QUERYCANCEL:
          retcode = BX_CI_RT_QUIT;
          return TRUE;
      }
      break;
    case WM_COMMAND:
      noticode = HIWORD(wParam);
      switch(noticode) {
        case EN_CHANGE:
          switch (LOWORD(wParam)) {
            case IDUHCIDEV1:
            case IDUHCIDEV2:
            case IDOHCIDEV1:
            case IDOHCIDEV2:
              changed = TRUE;
              SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
              break;
          }
          break;
        default:
          switch (LOWORD(wParam)) {
            case IDEXTHUB1:
              wsprintf(buffer, "%s.exthub1", BXPN_MENU_RUNTIME_USB);
              win32ParamDialog(hDlg, buffer);
              break;
          }
      }
      break;
  }
  return FALSE;
}

static BOOL CALLBACK RTLogOptDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL advanced;
  static BOOL changed;
  long noticode;
  PSHNOTIFY *psn;

  switch (msg) {
    case WM_INITDIALOG:
      InitLogOptionsDialog(hDlg);
      advanced = FALSE;
      changed = FALSE;
      return TRUE;
    case WM_NOTIFY:
      psn = (PSHNOTIFY *)lParam;
      switch(psn->hdr.code) {
        case PSN_APPLY:
          if ((psn->lParam == FALSE) && changed) { // Apply pressed & change in this dialog
            ApplyLogOptions(hDlg, advanced);
            changed = FALSE;
          }
          return PSNRET_NOERROR;
        case PSN_QUERYCANCEL:
          retcode = BX_CI_RT_QUIT;
          return TRUE;
      }
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
                SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
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
          }
      }
      break;
  }
  return FALSE;
}

static BOOL CALLBACK RTMiscDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL changed;
  int value;
  long noticode;
  char buffer[32];
  PSHNOTIFY *psn;

  switch (msg) {
    case WM_INITDIALOG:
      SetDlgItemInt(hDlg, IDVGAUPDATE, SIM->get_param_num(BXPN_VGA_UPDATE_INTERVAL)->get(), FALSE);
      SetDlgItemInt(hDlg, IDKBDPASTE, SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->get(), FALSE);
      if (SIM->get_param_num(BXPN_MOUSE_ENABLED)->get()) {
        SendMessage(GetDlgItem(hDlg, IDMOUSE), BM_SETCHECK, BST_CHECKED, 0);
      }
      SetDlgItemText(hDlg, IDUSERBTN, SIM->get_param_string(BXPN_USER_SHORTCUT)->getptr());
      SetDlgItemInt(hDlg, IDSB16TIMER, SIM->get_param_num(BXPN_SB16_DMATIMER)->get(), FALSE);
      SetDlgItemInt(hDlg, IDSBLOGLEV, SIM->get_param_num(BXPN_SB16_LOGLEVEL)->get(), FALSE);
      changed = FALSE;
      return TRUE;
    case WM_NOTIFY:
      psn = (PSHNOTIFY *)lParam;
      switch(psn->hdr.code) {
        case PSN_APPLY:
          if ((psn->lParam == FALSE) && changed) { // Apply pressed & change in this dialog
            if (SendMessage(GetDlgItem(hDlg, IDVGAUPDATE), EM_GETMODIFY, 0, 0)) {
              value = GetDlgItemInt(hDlg, IDVGAUPDATE, NULL, FALSE);
              SIM->get_param_num(BXPN_VGA_UPDATE_INTERVAL)->set(value);
            }
            if (SendMessage(GetDlgItem(hDlg, IDKBDPASTE), EM_GETMODIFY, 0, 0)) {
              value = GetDlgItemInt(hDlg, IDKBDPASTE, NULL, FALSE);
              SIM->get_param_num(BXPN_KBD_PASTE_DELAY)->set(value);
            }
            value = SendMessage(GetDlgItem(hDlg, IDMOUSE), BM_GETCHECK, 0, 0);
            SIM->get_param_num(BXPN_MOUSE_ENABLED)->set(value==BST_CHECKED);
            if (SendMessage(GetDlgItem(hDlg, IDUSERBTN), EM_GETMODIFY, 0, 0)) {
              GetDlgItemText(hDlg, IDUSERBTN, buffer, sizeof(buffer));
              SIM->get_param_string(BXPN_USER_SHORTCUT)->set(buffer);
            }
            if (SendMessage(GetDlgItem(hDlg, IDSB16TIMER), EM_GETMODIFY, 0, 0)) {
              value = GetDlgItemInt(hDlg, IDSB16TIMER, NULL, FALSE);
              SIM->get_param_num(BXPN_SB16_DMATIMER)->set(value);
            }
            if (SendMessage(GetDlgItem(hDlg, IDSBLOGLEV), EM_GETMODIFY, 0, 0)) {
              value = GetDlgItemInt(hDlg, IDSBLOGLEV, NULL, FALSE);
              SIM->get_param_num(BXPN_SB16_LOGLEVEL)->set(value);
            }
            changed = FALSE;
          }
          return PSNRET_NOERROR;
        case PSN_QUERYCANCEL:
          retcode = BX_CI_RT_QUIT;
          return TRUE;
      }
      break;
    case WM_COMMAND:
      noticode = HIWORD(wParam);
      switch(noticode) {
        case EN_CHANGE:
          switch (LOWORD(wParam)) {
            case IDVGAUPDATE:
            case IDKBDPASTE:
            case IDUSERBTN:
            case IDSB16TIMER:
            case IDSBLOGLEV:
              changed = TRUE;
              SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
              break;
          }
          break;
        default:
          switch (LOWORD(wParam)) {
            case IDMOUSE:
              changed = TRUE;
              SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)hDlg, 0);
              break;
          }
      }
      break;
  }
  return FALSE;
}

void LogAskDialog(BxEvent *event)
{
  event->retcode = DialogBoxParam(NULL, MAKEINTRESOURCE(ASK_DLG), GetBochsWindow(),
                                  (DLGPROC)LogAskProc, (LPARAM)event);
}

int AskString(bx_param_string_c *param)
{
  return DialogBoxParam(NULL, MAKEINTRESOURCE(STRING_DLG), GetBochsWindow(),
                        (DLGPROC)StringParamProc, (LPARAM)param);
}

int FloppyDialog(bx_param_filename_c *param)
{
  return DialogBoxParam(NULL, MAKEINTRESOURCE(FLOPPY_DLG), GetBochsWindow(),
                        (DLGPROC)FloppyDlgProc, (LPARAM)param);
}

void StartMenuDialog(HWND hwnd)
{
  int i;

  i = DialogBox(NULL, MAKEINTRESOURCE(STARTMENU_DLG), hwnd,
                (DLGPROC)StartMenuDlgProc);
  if (i == 1) {
    SIM->begin_simulation(bx_startup_flags.argc, bx_startup_flags.argv);
    // we don't expect it to return, but if it does, quit
  }
  SIM->quit_sim(1);
}

int RuntimeOptionsDialog()
{
  PROPSHEETPAGE psp[4];
  PROPSHEETHEADER psh;
  int i;

  memset(psp,0,sizeof(psp));
  for (i = 0; i < 4; i++) {
    psp[i].dwSize = sizeof(PROPSHEETPAGE);
    psp[i].dwFlags = PSP_DEFAULT;
    psp[i].hInstance = NULL;
  }
  psp[0].pszTemplate = MAKEINTRESOURCE(RT_CDROM_DLG);
  psp[0].pfnDlgProc = (DLGPROC) RTCdromDlgProc;
  psp[1].pszTemplate = MAKEINTRESOURCE(RT_USBDEV_DLG);
  psp[1].pfnDlgProc = (DLGPROC) RTUSBdevDlgProc;
  psp[2].pszTemplate = MAKEINTRESOURCE(RT_LOGOPT_DLG);
  psp[2].pfnDlgProc = (DLGPROC) RTLogOptDlgProc;
  psp[3].pszTemplate = MAKEINTRESOURCE(RT_MISC_DLG);
  psp[3].pfnDlgProc = (DLGPROC) RTMiscDlgProc;

  memset(&psh,0,sizeof(PROPSHEETHEADER));
  psh.dwSize = sizeof(PROPSHEETHEADER);
  psh.dwFlags = PSH_PROPSHEETPAGE;
  psh.hwndParent = GetBochsWindow();
  psh.hInstance = NULL;
  psh.pszCaption = "Runtime Options";
  psh.nPages = 4;
  psh.ppsp = (LPCPROPSHEETPAGE)&psp;

  PropertySheet(&psh);
  PostMessage(GetBochsWindow(), WM_SETFOCUS, 0, 0);
  return retcode;
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
#if BX_DEBUGGER && BX_DEBUGGER_GUI
    case BX_SYNC_EVT_GET_DBG_COMMAND:
      {
        // sim is at a "break" -- internal debugger is ready for a command
        debug_cmd = new char[512];
        debug_cmd_ready = FALSE;
        HitBreak();
        while (debug_cmd_ready == FALSE && bx_user_quit == 0)
        {
          if (vgaw_refresh != FALSE)  // is the GUI frontend requesting a VGAW refresh?
            SIM->refresh_vga();
          vgaw_refresh = FALSE;
          Sleep(10);
        }
        if (bx_user_quit != 0)
          BX_EXIT(0);
        event->u.debugcmd.command = debug_cmd;
        event->retcode = 1;
        return event;
      }
    case BX_ASYNC_EVT_DBG_MSG:
      ParseIDText (event->u.logmsg.msg);
      return event;
#endif
    case BX_SYNC_EVT_ASK_PARAM:
      param = event->u.param.param;
      if (param->get_type() == BXT_PARAM_STRING) {
        sparam = (bx_param_string_c *)param;
        opts = sparam->get_options();
        if (opts & sparam->IS_FILENAME) {
          if (opts & sparam->SELECT_FOLDER_DLG) {
            event->retcode = BrowseDir(sparam->get_label(), sparam->getptr());
          } else if (param->get_parent() == NULL) {
            event->retcode = AskFilename(GetBochsWindow(), (bx_param_filename_c *)sparam);
          } else {
            event->retcode = FloppyDialog((bx_param_filename_c *)sparam);
          }
          return event;
        } else {
          event->retcode = AskString(sparam);
          return event;
        }
      } else if (param->get_type() == BXT_LIST) {
        SIM->get_first_cdrom()->get_param_path(pname, BX_PATHNAME_LEN);
        event->retcode = win32ParamDialog(GetBochsWindow(), pname);
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
      // fall into default case
    default:
      return (*old_callback)(old_callback_arg, event);
  }
}

void win32_init_notify_callback()
{
  SIM->get_notify_callback(&old_callback, &old_callback_arg);
  assert (old_callback != NULL);
  SIM->set_notify_callback(win32_notify_callback, NULL);
}

#endif // BX_USE_TEXTCONFIG && defined(WIN32)
