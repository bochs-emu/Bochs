/////////////////////////////////////////////////////////////////////////
// $Id: win32dialog.cc,v 1.2 2003-05-25 13:35:39 vruppert Exp $
/////////////////////////////////////////////////////////////////////////

#ifdef WIN32

#include "config.h"

extern "C" {
#include <stdio.h>
#include <windows.h>
}
#include "win32res.h"
#include "siminterface.h"

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

void LogAskDialog(BxEvent *event)
{
  event->retcode = DialogBoxParam(NULL, MAKEINTRESOURCE(ASK_DLG), GetBochsWindow(),
                                  (DLGPROC)LogAskProc, (LPARAM)event);
}

int AskFilename(bx_param_filename_c *param)
{
  const PCHAR Filter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0";
  const PCHAR Ext    = "txt";
  OPENFILENAME ofn;
  int ret;
  char filename[MAX_PATH];

  param->get(filename, MAX_PATH);
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = GetBochsWindow();
  ofn.hInstance   = NULL;
  ofn.lpstrFilter = Filter;
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 0;
  ofn.lpstrFile   = filename;
  ofn.nMaxFile    = MAX_PATH;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle = param->get_description();
  ofn.nFileOffset = 0;
  ofn.nFileExtension = 0;
  ofn.lpstrDefExt = Ext;
  ofn.lCustData = 0;
  ofn.lpfnHook = NULL;
  ofn.lpTemplateName = NULL;
  ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
  if (param->get_options()->get() & bx_param_filename_c::SAVE_FILE_DIALOG) {
    ofn.Flags |= OFN_OVERWRITEPROMPT;
    ret = GetSaveFileName(&ofn);
  } else {
    ofn.Flags |= OFN_FILEMUSTEXIST;
    ret = GetOpenFileName(&ofn);
  }
  param->set(filename);
  if (ret == 0) ret = -1;
  return ret;
}

#endif // WIN32
