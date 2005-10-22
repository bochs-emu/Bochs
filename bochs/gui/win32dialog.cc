/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.cc,v 1.26 2005-10-22 11:00:00 vruppert Exp $
/////////////////////////////////////////////////////////////////////////

#include "config.h"

#if BX_USE_TEXTCONFIG && defined(WIN32)

extern "C" {
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
}
#include "win32res.h"
#include "siminterface.h"
#include "textconfig.h"
#include "win32dialog.h"

const char log_choices[5][16] = {"ignore", "log", "ask user", "end simulation", "no change"};
static int retcode = 0;
static bxevent_handler old_callback = NULL;
static void *old_callback_arg = NULL;

int AskFilename(HWND hwnd, bx_param_filename_c *param, const char *ext);

HWND GetBochsWindow()
{
  HWND hwnd;

  hwnd = FindWindow("Bochs for Windows - Display", NULL);
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

  switch (msg) {
    case WM_INITDIALOG:
      param = (bx_param_string_c *)lParam;
      SetWindowText(hDlg, param->get_name());
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
  static bx_param_enum_c *disktype;
  static char origpath[MAX_PATH];
  char mesg[MAX_PATH];
  char path[MAX_PATH];
  char *title;
  int cap;

  switch (msg) {
    case WM_INITDIALOG:
      param = (bx_param_filename_c *)lParam;
      if (param->get_id() == BXP_FLOPPYA_PATH) {
        status = SIM->get_param_enum(BXP_FLOPPYA_STATUS);
        disktype = SIM->get_param_enum(BXP_FLOPPYA_DEVTYPE);
      } else {
        status = SIM->get_param_enum(BXP_FLOPPYB_STATUS);
        disktype = SIM->get_param_enum(BXP_FLOPPYB_DEVTYPE);
      }
      cap = disktype->get() - (int)disktype->get_min();
      SetWindowText(GetDlgItem(hDlg, IDDEVTYPE), floppy_type_names[cap]);
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
          param->set(path);
          if (AskFilename(hDlg, param, "img") > 0) {
            SetWindowText(GetDlgItem(hDlg, IDPATH), param->getptr());
            SendMessage(GetDlgItem(hDlg, IDSTATUS), BM_SETCHECK, BST_CHECKED, 0);
          }
          return TRUE;
          break;
        case IDOK:
          if (SendMessage(GetDlgItem(hDlg, IDSTATUS), BM_GETCHECK, 0, 0) == BST_CHECKED) {
            GetDlgItemText(hDlg, IDPATH, path, MAX_PATH);
            if (lstrlen(path)) {
              status->set(BX_INSERTED);
            } else {
              status->set(BX_EJECTED);
              lstrcpy(path, "none");
            }
          } else {
            status->set(BX_EJECTED);
            lstrcpy(path, "none");
          }
          param->set(path);
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
        case IDCREATE:
          GetDlgItemText(hDlg, IDPATH, path, MAX_PATH);
          cap = disktype->get() - (int)disktype->get_min();
          if (CreateImage(hDlg, floppy_type_n_sectors[cap], path)) {
            wsprintf(mesg, "Created a %s disk image called %s", floppy_type_names[cap], path);
            MessageBox(hDlg, mesg, "Image created", MB_OK);
          }
          return TRUE;
          break;
      }
  }
  return FALSE;
}

static BOOL CALLBACK Cdrom1DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static bx_atadevice_options cdromop;
  int device;
  static char origpath[MAX_PATH];
  char path[MAX_PATH];

  switch (msg) {
    case WM_INITDIALOG:
      SIM->get_cdrom_options(0, &cdromop, &device);
      lstrcpy(origpath, cdromop.Opath->getptr());
      if (lstrlen(origpath) && lstrcmp(origpath, "none")) {
        SetWindowText(GetDlgItem(hDlg, IDCDROM1), origpath);
      }
      if (cdromop.Ostatus->get() == BX_INSERTED) {
        SendMessage(GetDlgItem(hDlg, IDSTATUS1), BM_SETCHECK, BST_CHECKED, 0);
      }
      return TRUE;
      break;
    case WM_CLOSE:
      if (lstrcmp(cdromop.Opath->getptr(), origpath)) {
        cdromop.Opath->set(origpath);
      }
      EndDialog(hDlg, -1);
      break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDBROWSE1:
          GetDlgItemText(hDlg, IDCDROM1, path, MAX_PATH);
          cdromop.Opath->set(path);
          if (AskFilename(hDlg, (bx_param_filename_c *)cdromop.Opath, "iso") > 0) {
            SetWindowText(GetDlgItem(hDlg, IDCDROM1), cdromop.Opath->getptr());
            SendMessage(GetDlgItem(hDlg, IDSTATUS1), BM_SETCHECK, BST_CHECKED, 0);
          }
          break;
        case IDOK:
          if (SendMessage(GetDlgItem(hDlg, IDSTATUS1), BM_GETCHECK, 0, 0) == BST_CHECKED) {
            GetDlgItemText(hDlg, IDCDROM1, path, MAX_PATH);
            if (lstrlen(path)) {
              cdromop.Ostatus->set(BX_INSERTED);
              if (lstrcmp(path, cdromop.Opath->getptr())) {
                cdromop.Opath->set(path);
              }
            } else {
              cdromop.Ostatus->set(BX_EJECTED);
            }
          } else {
            cdromop.Ostatus->set(BX_EJECTED);
          }
          EndDialog(hDlg, 1);
          break;
        case IDCANCEL:
          if (lstrcmp(cdromop.Opath->getptr(), origpath)) {
            cdromop.Opath->set(origpath);
          }
          EndDialog(hDlg, -1);
          break;
      }
  }
  return FALSE;
}

void RuntimeDlgSetStdLogOpt(HWND hDlg)
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

void RuntimeDlgSetAdvLogOpt(HWND hDlg)
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

static BOOL CALLBACK RTCdromDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static int devcount;
  static bx_atadevice_options cdromop[4];
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
        if (!SIM->get_cdrom_options (cdrom, &cdromop[cdrom], &device) || !cdromop[cdrom].Opresent->get ()) {
          EnableWindow(GetDlgItem(hDlg, IDLABEL1+cdrom), FALSE);
          EnableWindow(GetDlgItem(hDlg, IDCDROM1+cdrom), FALSE);
          EnableWindow(GetDlgItem(hDlg, IDBROWSE1+cdrom), FALSE);
          EnableWindow(GetDlgItem(hDlg, IDSTATUS1+cdrom), FALSE);
        } else {
          lstrcpy(origpath[cdrom], cdromop[cdrom].Opath->getptr());
          if (lstrlen(origpath[cdrom]) && lstrcmp(origpath[cdrom], "none")) {
            SetWindowText(GetDlgItem(hDlg, IDCDROM1+cdrom), origpath[cdrom]);
          }
          if (cdromop[cdrom].Ostatus->get() == BX_INSERTED) {
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
                  cdromop[device].Ostatus->set(BX_INSERTED);
                  if (lstrcmp(path, cdromop[device].Opath->getptr())) {
                    cdromop[device].Opath->set(path);
                  }
                } else {
                  cdromop[device].Ostatus->set(BX_EJECTED);
                }
              } else {
                cdromop[device].Ostatus->set(BX_EJECTED);
              }
            }
            changed = FALSE;
          } else {
            if (changed) {
              for (device=1; device<devcount; device++) {
                if (lstrcmp(cdromop[device].Opath->getptr(), origpath[device])) {
                  cdromop[device].Opath->set(origpath[device]);
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
              cdromop[device].Opath->set(path);
              if (AskFilename(hDlg, (bx_param_filename_c *)cdromop[device].Opath, "iso") > 0) {
                SetWindowText(GetDlgItem(hDlg, IDCDROM1+device), cdromop[device].Opath->getptr());
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

static BOOL CALLBACK RTLogOptDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static BOOL advanced;
  static BOOL changed;
  int idx, level, mod, value;
  long noticode;
  char prefix[8];
  PSHNOTIFY *psn;

  switch (msg) {
    case WM_INITDIALOG:
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
      RuntimeDlgSetStdLogOpt(hDlg);
      advanced = FALSE;
      changed = FALSE;
      return TRUE;
    case WM_NOTIFY:
      psn = (PSHNOTIFY *)lParam;
      switch(psn->hdr.code) {
        case PSN_APPLY:
          if ((psn->lParam == FALSE) && changed) { // Apply pressed & change in this dialog
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
              RuntimeDlgSetAdvLogOpt(hDlg);
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
                RuntimeDlgSetAdvLogOpt(hDlg);
                advanced = TRUE;
              } else {
                SendMessage(GetDlgItem(hDlg, IDDEVLIST), LB_SETCURSEL, (WPARAM)-1, 0);
                RuntimeDlgSetStdLogOpt(hDlg);
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
      SetDlgItemInt(hDlg, IDVGAUPDATE, SIM->get_param_num(BXP_VGA_UPDATE_INTERVAL)->get(), FALSE);
      SetDlgItemInt(hDlg, IDKBDPASTE, SIM->get_param_num(BXP_KBD_PASTE_DELAY)->get(), FALSE);
      if (SIM->get_param_num(BXP_MOUSE_ENABLED)->get()) {
        SendMessage(GetDlgItem(hDlg, IDMOUSE), BM_SETCHECK, BST_CHECKED, 0);
      }
      SetDlgItemText(hDlg, IDUSERBTN, SIM->get_param_string(BXP_USER_SHORTCUT)->getptr());
      SetDlgItemInt(hDlg, IDSB16TIMER, SIM->get_param_num(BXP_SB16_DMATIMER)->get(), FALSE);
      SetDlgItemInt(hDlg, IDSBLOGLEV, SIM->get_param_num(BXP_SB16_LOGLEVEL)->get(), FALSE);
      changed = FALSE;
      return TRUE;
    case WM_NOTIFY:
      psn = (PSHNOTIFY *)lParam;
      switch(psn->hdr.code) {
        case PSN_APPLY:
          if ((psn->lParam == FALSE) && changed) { // Apply pressed & change in this dialog
            value = GetDlgItemInt(hDlg, IDVGAUPDATE, NULL, FALSE);
            SIM->get_param_num(BXP_VGA_UPDATE_INTERVAL)->set(value);
            value = GetDlgItemInt(hDlg, IDKBDPASTE, NULL, FALSE);
            SIM->get_param_num(BXP_KBD_PASTE_DELAY)->set(value);
            value = SendMessage(GetDlgItem(hDlg, IDMOUSE), BM_GETCHECK, 0, 0);
            SIM->get_param_num(BXP_MOUSE_ENABLED)->set(value==BST_CHECKED);
            GetDlgItemText(hDlg, IDUSERBTN, buffer, sizeof(buffer));
            SIM->get_param_string(BXP_USER_SHORTCUT)->set(buffer);
            value = GetDlgItemInt(hDlg, IDSB16TIMER, NULL, FALSE);
            SIM->get_param_num(BXP_SB16_DMATIMER)->set(value);
            value = GetDlgItemInt(hDlg, IDSBLOGLEV, NULL, FALSE);
            SIM->get_param_num(BXP_SB16_LOGLEVEL)->set(value);
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

int AskFilename(HWND hwnd, bx_param_filename_c *param, const char *ext)
{
  OPENFILENAME ofn;
  int ret;
  DWORD errcode;
  char filename[MAX_PATH];
  char *title;
  char errtext[80];

  param->get(filename, MAX_PATH);
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

int Cdrom1Dialog()
{
  return DialogBox(NULL, MAKEINTRESOURCE(CDROM1_DLG), GetBochsWindow(),
                   (DLGPROC)Cdrom1DlgProc);
}

int RuntimeOptionsDialog()
{
  PROPSHEETPAGE psp[3];
  PROPSHEETHEADER psh;
  int i;

  memset(psp,0,sizeof(psp));
  for (i = 0; i < 3; i++) {
    psp[i].dwSize = sizeof(PROPSHEETPAGE);
    psp[i].dwFlags = PSP_DEFAULT;
    psp[i].hInstance = NULL;
  }
  psp[0].pszTemplate = MAKEINTRESOURCE(RT_CDROM_DLG);
  psp[0].pfnDlgProc = RTCdromDlgProc;
  psp[1].pszTemplate = MAKEINTRESOURCE(RT_LOGOPT_DLG);
  psp[1].pfnDlgProc = RTLogOptDlgProc;
  psp[2].pszTemplate = MAKEINTRESOURCE(RT_MISC_DLG);
  psp[2].pfnDlgProc = RTMiscDlgProc;

  memset(&psh,0,sizeof(PROPSHEETHEADER));
  psh.dwSize = sizeof(PROPSHEETHEADER);
  psh.dwFlags = PSH_PROPSHEETPAGE;
  psh.hwndParent = GetBochsWindow();
  psh.hInstance = NULL;
  psh.pszCaption = "Runtime Options";
  psh.nPages = 3;
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
        opts = sparam->get_options()->get();
        if (opts & sparam->IS_FILENAME) {
          if (param->get_id() == BXP_NULL) {
            event->retcode = AskFilename(GetBochsWindow(), (bx_param_filename_c *)sparam, "txt");
          } else {
            event->retcode = FloppyDialog((bx_param_filename_c *)sparam);
          }
          return event;
        } else {
          event->retcode = AskString(sparam);
          return event;
        }
      } else if (param->get_type() == BXT_LIST) {
        event->retcode = Cdrom1Dialog();
        return event;
      }
    case BX_SYNC_EVT_TICK: // called periodically by siminterface.
    case BX_ASYNC_EVT_REFRESH: // called when some bx_param_c parameters have changed.
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
