/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.cc,v 1.4 2003-07-15 21:02:05 vruppert Exp $
/////////////////////////////////////////////////////////////////////////

#ifdef WIN32

#include "config.h"

extern "C" {
#include <stdio.h>
#include <windows.h>
}
#include "win32res.h"
#include "siminterface.h"
#include "win32dialog.h"

HWND GetBochsWindow()
{
  HWND hwnd;

  hwnd = FindWindow("Bochs for Windows", NULL);
  if (hwnd == NULL) {
    hwnd = GetForegroundWindow();
  }
  return hwnd;
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
      EndDialog(hDlg, 2);
      break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          EndDialog(hDlg, SendMessage(GetDlgItem(hDlg, IDASKLIST), LB_GETCURSEL, 0, 0));
          break;
        case IDCANCEL:
          EndDialog(hDlg, 2);
          break;
    }
  }
  return FALSE;
}

static BOOL CALLBACK StringParamProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static bx_param_string_c *param;
  char buffer[20];

  switch (msg) {
    case WM_INITDIALOG:
      param = (bx_param_string_c *)lParam;
      SetWindowText(hDlg, param->get_description());
      SetWindowText(GetDlgItem(hDlg, IDSTRING), param->getptr());
      return FALSE;
    case WM_CLOSE:
      EndDialog(hDlg, -1);
      break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          GetWindowText(GetDlgItem(hDlg, IDSTRING), buffer, 20);
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
  static char origpath[MAX_PATH];
  char path[MAX_PATH];

  switch (msg) {
    case WM_INITDIALOG:
      param = (bx_param_filename_c *)lParam;
      if (param->get_id() == BXP_FLOPPYA_PATH) {
        status = SIM->get_param_enum(BXP_FLOPPYA_STATUS);
      } else {
        status = SIM->get_param_enum(BXP_FLOPPYB_STATUS);
      }
      if (status->get() == BX_INSERTED) {
        SendMessage(GetDlgItem(hDlg, IDSTATUS), BM_SETCHECK, BST_CHECKED, 0);
      }
      lstrcpy(origpath, param->getptr());
      SetWindowText(hDlg, param->get_ask_format());
      if (lstrlen(origpath) && lstrcmp(origpath, "none")) {
        SetWindowText(GetDlgItem(hDlg, IDPATH), origpath);
      }
      return FALSE;
    case WM_CLOSE:
      EndDialog(hDlg, -1);
      break;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDBROWSE:
          if (AskFilename(param) > 0) {
            SetWindowText(GetDlgItem(hDlg, IDPATH), param->getptr());
            SendMessage(GetDlgItem(hDlg, IDSTATUS), BM_SETCHECK, BST_CHECKED, 0);
          }
          break;
        case IDOK:
          if (SendMessage(GetDlgItem(hDlg, IDSTATUS), BM_GETCHECK, 0, 0) == BST_CHECKED) {
            GetWindowText(GetDlgItem(hDlg, IDPATH), path, MAX_PATH);
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
          break;
        case IDCANCEL:
          param->set(origpath);
          EndDialog(hDlg, -1);
          break;
    }
  }
  return FALSE;
}

void LogAskDialog(BxEvent *event)
{
  event->retcode = DialogBoxParam(NULL, MAKEINTRESOURCE(ASK_DLG), GetBochsWindow(),
                                  (DLGPROC)LogAskProc, (LPARAM)event);
}

int AskFilename(bx_param_filename_c *param)
{
  OPENFILENAME ofn;
  int ret;
  char filename[MAX_PATH];
  char *title;

  param->get(filename, MAX_PATH);
  title = param->get_ask_format();
  if (!title) {
    title = param->get_description();
  }
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = GetBochsWindow();
  ofn.hInstance   = NULL;
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 0;
  ofn.lpstrFile   = filename;
  ofn.nMaxFile    = MAX_PATH;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle = title;
  ofn.nFileOffset = 0;
  ofn.nFileExtension = 0;
  ofn.lCustData = 0;
  ofn.lpfnHook = NULL;
  ofn.lpTemplateName = NULL;
  ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
  if (param->get_options()->get() & bx_param_filename_c::SAVE_FILE_DIALOG) {
    ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
    ofn.lpstrDefExt = "prg";
    ofn.Flags |= OFN_OVERWRITEPROMPT;
    ret = GetSaveFileName(&ofn);
  } else {
    ofn.lpstrFilter = "Floppy image files (*.img)\0*.img\0All files (*.*)\0*.*\0";
    ofn.lpstrDefExt = "img";
    ofn.Flags |= OFN_FILEMUSTEXIST;
    ret = GetOpenFileName(&ofn);
  }
  param->set(filename);
  if (ret == 0) ret = -1;
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

#endif // WIN32
