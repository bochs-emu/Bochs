/////////////////////////////////////////////////////////////////
//
// gui/wx.cc
// $Id: wx.cc,v 1.1.2.4 2001-06-24 19:24:57 instinc Exp $
//
// GUI Control Panel for Bochs, using wxWindows toolkit.
//
// Bochs startup order:
// - wxWindows calls OnInit method
// - OnInit calls bx_init_before_control_panel () in main.cc to set up
//   bochs variables
//
/////////////////////////////////////////////////////////////////

#include "config.h"
extern "C" {
// siminterface needs stdio.h
#include <stdio.h>
}
#include "osdep.h"
#include "control.h"
#include "siminterface.h"


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <wx/scrolwin.h>
#include <wx/sizer.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class BochsApp: public wxApp
{
  virtual bool OnInit();
};

class BochsThread: public wxThread 
{
public:
  virtual ExitCode Entry ();
};

class MyFrame: public wxFrame
{
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

private:
  DECLARE_EVENT_TABLE()
};

class MyPanel: public wxScrolledWindow
{
private:
  wxStaticText *text1;
  BochsThread *bochsThread;
  void buildParamList (int x, int y);
public:
  MyPanel () {};
  MyPanel ( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
  ~MyPanel();

  DECLARE_DYNAMIC_CLASS(MyPanel)
  DECLARE_EVENT_TABLE()
};


class BochsEventHandler: public wxEvtHandler {
public:
  BochsEventHandler ();
  void OnEvent (wxCommandEvent& event);
private:
  DECLARE_EVENT_TABLE()
};

class ParamEditor: public wxPanel {
  bx_param_c *param;
public:
  ParamEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
  static ParamEditor *createEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
};

class ParamEditorBool : public ParamEditor {
  wxCheckBox *component;
public:
  ParamEditorBool (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
};

class ParamEditorNum : public ParamEditor {
  wxTextCtrl *component;
public:
  ParamEditorNum (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
};

class ParamEditorString : public ParamEditor {
  wxTextCtrl *component;
public:
  ParamEditorString (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
};

class ParamEditorEnum : public ParamEditor {
  wxChoice *component;
  int n_choices;
  wxString *choices;
public:
  ParamEditorEnum (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
};

class ParamEditorList : public ParamEditor {
  wxControl *component;
public:
  ParamEditorList (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
};


BEGIN_EVENT_TABLE(BochsEventHandler, wxEvtHandler)
  EVT_MENU (-1, BochsEventHandler::OnEvent)
END_EVENT_TABLE()

BochsEventHandler *bochsEventHandler;

BochsEventHandler::BochsEventHandler ()
{
  printf ("BochsEventHandler::BochsEventHandler\n");
}

void
BochsEventHandler::OnEvent (wxCommandEvent& evt)
{
  printf ("BochsEventHandler::OnEvent called with event id=%d\n", evt.GetId ());
}

enum
{
  ID_Quit = 1,
  ID_StartBochs,
  ID_Check1,
  ID_Choice1,
  ID_Combo1,
  ID_Text1,
  ID_THIS_IS_THE_LAST
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit,  MyFrame::OnQuit)
  EVT_MENU(ID_StartBochs, MyFrame::OnAbout)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(MyPanel, wxScrolledWindow)
BEGIN_EVENT_TABLE(MyPanel, wxScrolledWindow)
END_EVENT_TABLE()


IMPLEMENT_APP(BochsApp)

bool BochsApp::OnInit()
{
  // do bochs initialization first
  siminterface_init ();
  MyFrame *frame = new MyFrame( "Bochs Control Panel", wxPoint(50,50), wxSize(450,340) );
  bochsEventHandler = new BochsEventHandler ();
  frame->Show( TRUE );
  SetTopWindow( frame );
  return TRUE;
}


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  wxMenu *menuFile = new wxMenu;

  menuFile->Append( ID_StartBochs, "&Start Bochs..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, "E&xit" );

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile, "&File" );

  SetMenuBar( menuBar );

  CreateStatusBar();
  SetStatusText( "Bochs Controls" );
  MyPanel *panel = new MyPanel ( this, -1, wxPoint(0,0), wxSize(100,100) );
  panel->SetScrollbars( 10, 10, 50, 100 );
}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close( TRUE );
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox( "This is a wxWindows' Hello world sample",
			"About Hello World", wxOK | wxICON_INFORMATION );
}

void *
BochsThread::Entry (void)
{
  int argc=1;
  char *argv[] = {"bochs"};
  bx_continue_after_control_panel (argc, argv);
}

MyPanel::MyPanel (wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
  : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER | wxTAB_TRAVERSAL, "Bochs Controls" )
{
  // the parent constructor makes this panel a child of frame, with 
  // proper size.
  bochsThread = NULL;
  wxButton *btn1 = new wxButton (this, ID_StartBochs, "Start Bochs", wxPoint (10, 10));
  //wxCheckBox *check1 = new wxCheckBox (this, ID_Check1, "Checkbox", wxPoint (10, 50));
  //wxString choices[] = { "apple", "orange", "banana" };
  //wxChoice *choice1 = new wxChoice (this, ID_Choice1, wxPoint (10, 80), wxSize (80, 30), 3, choices);
  //wxComboBox *combo1 = new wxComboBox (this, ID_Combo1, "Default Combo String", wxPoint (10, 130), wxSize (180, 80), 3, choices);
  //text1 = new wxStaticText (this, ID_Text1, "no text yet", wxPoint (0, 160));
  buildParamList (10, 40);
}

MyPanel::~MyPanel()
{
}


void MyPanel::buildParamList (int x, int y)
{
  // get a list of parameters
  int min, max;
  SIM->get_param_id_range (&min, &max);
  for (int i=min; i<=max; i++) {
    bx_param_c *param = SIM->get_param ((bx_id) i);
    if (param != NULL) {
      printf ("param %d is called %s\n", i, param->get_name ());
      new wxStaticText (this, i, param->get_name (), wxPoint (x, y));
      ParamEditor *editor = ParamEditor::createEditor (param, this, -1, x+120, y);
      y+=30;
    }
  }
}


ParamEditor::ParamEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : wxPanel(parent, id, wxPoint(x, y), wxSize (300, 20))
{
  printf ("ParamEditor::ParamEditor\n");
  this->param = param;
  //wxString text = wxString (param->get_name ());
  //new wxStaticText (parent, id, text, wxPoint (x, y));
}

ParamEditor *
ParamEditor::createEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
{
  printf ("ParamEditor::createEditor\n");
  Bit8u type = param->get_type ();
  switch (type) {
    case BXT_PARAM_BOOL:
      return new ParamEditorBool (param, parent, id, x, y);
      break;
    case BXT_PARAM_NUM:
      return new ParamEditorNum (param, parent, id, x, y);
      break;
    case BXT_PARAM_STRING:
      return new ParamEditorString (param, parent, id, x, y);
      break;
    case BXT_PARAM_ENUM:
      return new ParamEditorEnum (param, parent, id, x, y);
      break;
    case BXT_LIST:
      return new ParamEditorList (param, parent, id, x, y);
      break;
    default:
      printf ("ParamEditor::createEditor with unknown type %d\n", type);
  }
}

ParamEditorBool::ParamEditorBool (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  component = new wxCheckBox (this, id, "", wxPoint (0, 0));
}

ParamEditorNum::ParamEditorNum (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  // background color helps to visualize the borders of the panel
  //SetBackgroundColour (wxColour ("red"));
  bx_param_num_c *num = (bx_param_num_c *)param;
  wxString text;
  text.Printf ("%d", num->get ());
  component = new wxTextCtrl (this, id, text, wxPoint (-1, -1), wxSize (-1, -1));
}

ParamEditorString::ParamEditorString (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  //SetBackgroundColour (wxColour ("red"));
  bx_param_string_c *string = (bx_param_string_c *)param;
  wxString text = wxString (string->getptr ());
  component = new wxTextCtrl (this, id, text, wxPoint (-1, -1), wxSize (-1, -1));
}

ParamEditorEnum::ParamEditorEnum (bx_param_c *genericparam, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (genericparam, parent, id, x, y)
{
  //SetBackgroundColour (wxColour ("red"));
  
  // build the array of choices
  bx_param_enum_c *param = (bx_param_enum_c *) genericparam;
  n_choices = param->get_max () - param->get_min () + 1;
  choices = new wxString[n_choices];
  for (int i=0; i<n_choices; i++)
    choices[i] = param->get_choice (i);
  component = new wxChoice (this, id, wxPoint (-1, -1), wxSize (-1, -1), n_choices, choices);
}


ParamEditorList::ParamEditorList (bx_param_c *genericparam, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (genericparam, parent, id, x, y)
{
  //SetBackgroundColour (wxColour ("red"));
  bx_list_c *param = (bx_list_c *) genericparam;
  component = new wxStaticText (this, id, "list param not implemented", wxPoint (-1, -1));
}
