/////////////////////////////////////////////////////////////////
// $Id: wxdialog.cc,v 1.1 2002-08-28 03:20:23 bdenney Exp $
/////////////////////////////////////////////////////////////////
//
// misc/wxdialog.cc
// This is a testbed for wxWindows dialog box experiments.
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/html/htmlwin.h"

#include "config.h"              // definitions based on configure script
#include "osdep.h"               // workarounds for missing stuff
#include "gui/siminterface.h"    // interface to the simulator
#include "bxversion.h"           // get version string
#include "wxmain.h"              // wxwindows shared stuff
#include "wxdialog.h"            // custom dialog boxes

//////////////////////////////////////////////////////////////////////
// constants, prototypes
//////////////////////////////////////////////////////////////////////

enum {
  ID_ShowDialog_1 = 1,
  ID_ShowDialog_2,
  ID_ShowDialog_3,
  ID_Quit,
  ID_Button1,
  ID_Button2,
  ID_Continue,
  ID_Die,
  ID_DumpCore,
  ID_Debugger,
  ID_Help,
  ID_MY_LAST_ID
};

void ChangeStaticText (wxSizer *sizer, wxStaticText *win, wxString newtext);

//////////////////////////////////////////////////////////////////////
// LogMsgAskDialog implementation
//////////////////////////////////////////////////////////////////////

// all events go to OnEvent method
BEGIN_EVENT_TABLE(LogMsgAskDialog, wxDialog)
  EVT_BUTTON(ID_Continue, LogMsgAskDialog::OnEvent)
  EVT_BUTTON(ID_Die, LogMsgAskDialog::OnEvent)
  EVT_BUTTON(ID_DumpCore, LogMsgAskDialog::OnEvent)
  EVT_BUTTON(ID_Debugger, LogMsgAskDialog::OnEvent)
  EVT_BUTTON(ID_Help, LogMsgAskDialog::OnEvent)
END_EVENT_TABLE()


LogMsgAskDialog::LogMsgAskDialog(
    wxWindow* parent,
    wxWindowID id,
    const wxString& title)
  : wxDialog (parent, id, title, wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  for (int i=0; i<N_BUTTONS; i++) enabled[i] = TRUE;
  vertSizer = new wxBoxSizer(wxVERTICAL);
  context = new wxStaticText (this, -1, "Context: ");
  wxFont font = context->GetFont ();
  font.SetWeight (wxBOLD);
  font.SetPointSize (2 + font.GetPointSize ());
  context->SetFont (font);
  message = new wxStaticText (this, -1, "Message: ");
  message->SetFont (font);
  dontAsk = new wxCheckBox (this, -1, LOG_MSG_DONT_ASK_STRING);
  btnSizer = new wxBoxSizer(wxHORIZONTAL);
  // fill vertical sizer
  vertSizer->Add (context, 0, wxGROW|wxALIGN_LEFT|wxLEFT|wxTOP, 30);
  vertSizer->Add (message, 0, wxGROW|wxALIGN_LEFT|wxLEFT, 30);
  vertSizer->Add (dontAsk, 0, wxALIGN_CENTER|wxTOP, 30);
  vertSizer->Add (btnSizer, 0, wxALIGN_CENTER|wxTOP, 30);
  // Some object creation and layout is postponed until Init()
  // so that caller has time to configure the dialog.
}

void LogMsgAskDialog::SetContext (char *s) {
  wxString text;
  text.Printf ("Context: %s", s);
  ChangeStaticText (vertSizer, context, text);
}

void LogMsgAskDialog::SetMessage (char *s) {
  wxString text;
  text.Printf ("Message: %s", s);
  ChangeStaticText (vertSizer, message, text);
}

void LogMsgAskDialog::Init ()
{
  static const int ids[N_BUTTONS] = LOG_MSG_ASK_IDS;
  static const char *names[N_BUTTONS] = LOG_MSG_ASK_NAMES;
  for (int i=0; i<N_BUTTONS; i++) {
    if (!enabled[i]) continue;
    wxButton *btn = new wxButton (this, ids[i], names[i]);
    btnSizer->Add (btn, 1, wxALL, 5);
  }
  wxSize ms = message->GetSize ();
  printf ("message size is %d,%d\n", ms.GetWidth(), ms.GetHeight ());
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  printf ("minsize is %d,%d\n", size.GetWidth(), size.GetHeight ());
  int margin = 10;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

// Event handler for dialog buttons. Just translate the wx ids into
// enum values and return them with EndModel() to make the dialog
// go away.
void LogMsgAskDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  int ret = -1;
  switch (id) {
    case ID_Continue:   ret = CONT;  break;
    case ID_Die:        ret = DIE;   break;
    case ID_DumpCore:   ret = DUMP;  break;
    case ID_Debugger:   ret = DEBUG; break;
    case ID_Help: showHelp (); return;
    default:
      return;  // without EndModal
  }
  printf ("you pressed button id=%d, return value=%d\n", id, ret);
  EndModal (ret);
}

void LogMsgAskDialog::showHelp ()
{
  wxMessageBox("No help is available yet.", "No Help", wxOK | wxICON_ERROR );
}

/////////////////////////////////////////////////////////////////
// utility
/////////////////////////////////////////////////////////////////

// Unfortunately this step is necessary if you change the text.
// Otherwise the sizer that contains the text never realizes that
// the size has changed, and the layout is never updated.  The
// SetItemMinSize trick was reported on comp.soft-sys.wxwindows by 
// Dirk Birnhardt.
void ChangeStaticText (wxSizer *sizer, wxStaticText *win, wxString newtext)
{
  win->SetLabel (newtext);
  wxSize sz = win->GetSize ();
  sizer->SetItemMinSize (win, sz.GetWidth (), sz.GetHeight ());
}

#ifdef TEST_DIALOG
//////////////////////////////////////////////////////////////////////
// MyApp declaration
//////////////////////////////////////////////////////////////////////
class MyApp: public wxApp
{
virtual bool OnInit();
};
IMPLEMENT_APP(MyApp)

//////////////////////////////////////////////////////////////////////
// MyFrame declaration
//////////////////////////////////////////////////////////////////////
class MyFrame: public wxFrame
{
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
  void OnMenuEvent(wxCommandEvent& event);
private:
DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_ShowDialog_1, MyFrame::OnMenuEvent)
  EVT_MENU(ID_ShowDialog_2, MyFrame::OnMenuEvent)
  EVT_MENU(ID_ShowDialog_3, MyFrame::OnMenuEvent)
  EVT_MENU(ID_Quit, MyFrame::OnMenuEvent)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// MyApp methods
//////////////////////////////////////////////////////////////////////

bool MyApp::OnInit()
{
  MyFrame *frame = new MyFrame( "Bochs x86 Emulator", wxPoint(50,50), wxSize(450,340), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION );
  frame->Show( TRUE );
  SetTopWindow( frame );
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// MyFrame methods
//////////////////////////////////////////////////////////////////////
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
: wxFrame((wxFrame *)NULL, -1, title, pos, size, style)
{
  wxMenu *menu1 = new wxMenu;
  menu1->Append( ID_ShowDialog_1, "Show Dialog &1" );
  menu1->Append( ID_ShowDialog_2, "Show Dialog &2" );
  menu1->Append( ID_ShowDialog_3, "Show Dialog &3" );
  menu1->Append( ID_Quit, "&Quit" );
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menu1, "&File" );
  SetMenuBar( menuBar );
  CreateStatusBar();
  SetStatusText("");
}

void MyFrame::OnMenuEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  switch (id) {
    case ID_ShowDialog_1:
      {
      printf ("show dialog 1\n");
      LogMsgAskDialog dlg(this, -1, "Panic");
      dlg.EnableButton (dlg.DEBUG, FALSE);
      dlg.SetContext ("Hard Drive");
      dlg.SetMessage ("could not open hard drive image file '30M.sample'");
      int n = dlg.ShowModal ();
      printf ("modal dialog returned %d\n", n);
      printf ("and the dontAsk button is %d\n", dlg.GetDontAsk ());
      }
      break;
    case ID_ShowDialog_2:
      printf ("show dialog 2\n");
      break;
    case ID_ShowDialog_3:
      printf ("show dialog 3\n");
      break;
    case ID_Quit:
      Close (TRUE);
      break;
  }
}
#endif

