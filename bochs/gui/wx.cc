/////////////////////////////////////////////////////////////////
//
// gui/wx.cc
// $Id: wx.cc,v 1.1.2.13 2001-06-28 14:52:49 bdenney Exp $
//
// GUI Control Panel for Bochs, using wxWindows toolkit.
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

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "gui/wx.h"

// I wanted to make this a static member of ParamEditor, but it gave me
// link errors so I just put it outside for now.
wxHashTable paramEditorsById;

// event table for ParamEditor and subclasses
BEGIN_EVENT_TABLE(ParamEditor, wxPanel)
  //EVT_CHECKBOX (-1, ParamEditor::OnEvent)
  //EVT_CHOICE (-1, ParamEditor::OnEvent)
  //EVT_TEXT(-1, ParamEditor::OnEvent)
  //EVT_TEXT_ENTER(-1, ParamEditor::OnEvent)
END_EVENT_TABLE()

// event table for wxFrame
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit,  MyFrame::OnQuit)
  EVT_MENU(ID_StartBochs, MyFrame::HandleEvent)
  EVT_MENU(ID_ApplyAction, MyFrame::HandleEvent)
  EVT_MENU(ID_RevertAction, MyFrame::HandleEvent)
  EVT_MENU(ID_ReadConfig, MyFrame::HandleEvent)
  EVT_MENU(ID_WriteConfig, MyFrame::HandleEvent)
  EVT_MENU(ID_WriteConfigAs, MyFrame::HandleEvent)
END_EVENT_TABLE()

  // event table for MyPanel
BEGIN_EVENT_TABLE(MyPanel, wxScrolledWindow)
  EVT_BUTTON (ID_StartBochs, MyPanel::HandleEvent)
  EVT_BUTTON (ID_ApplyAction, MyPanel::HandleEvent)
  EVT_BUTTON (ID_RevertAction, MyPanel::HandleEvent)
  // do not call event handlers for the ParamEditors
  //EVT_CHECKBOX (ID_Check1, MyPanel::HandleEvent)
  //EVT_CHOICE (ID_Choice1, MyPanel::HandleEvent)
  //EVT_COMBOBOX (ID_Combo1, MyPanel::HandleEvent)
  //EVT_TEXT (ID_Combo1, MyPanel::HandleEvent)
END_EVENT_TABLE()




// event table for BochsEventHandler and subclasses
BEGIN_EVENT_TABLE(BochsEventHandler, wxEvtHandler)
  EVT_MENU (-1, BochsEventHandler::OnEvent)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  bochsThread = NULL;
  wxMenu *menuFile = new wxMenu;

  menuFile->Append( ID_StartBochs, "&Start Bochs..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_ReadConfig, "Read Configuration File" );
  menuFile->Append( ID_WriteConfig, "Write Configuration" );
  menuFile->Append( ID_WriteConfigAs, "Write Configuration As..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, "E&xit" );

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile, "&File" );

  SetMenuBar( menuBar );

  CreateStatusBar();
  SetStatusText( "Bochs Controls" );
  int width=500, height=3000;
  panel = new MyPanel ( this, this, -1, wxPoint(0,0), wxSize(width,height) );
  panel->SetScrollbars( 10, 10, width/10, height/10 );
}

void
MyFrame::HandleEvent (wxCommandEvent& event)
{
  panel->HandleEvent (event);
}

void
MyFrame::StartBochsThread ()
{
  if (bochsThread == NULL)
  {
    printf ("Starting bochs thread\n");
    bochsThread = new BochsThread ();
    bochsThread->Create ();
    bochsThread->Run ();
  } else {
    wxMessageBox (
    "Can't start Bochs simulator, because it is already running",
    "Already Running", wxOK | wxICON_ERROR);
  }
}


int 
MyFrame::ReadConfiguration (int ask_name)
{
  char oldrc_cstr[CPANEL_PATH_LEN];
  int found_default = 1;
  if (SIM->get_default_rc (oldrc_cstr, CPANEL_PATH_LEN) < 0) {
    found_default = 0;
    oldrc_cstr[0] = 0;
  }
  wxString oldrc (oldrc_cstr), newrc = oldrc;
  if (!found_default) {
    if (!ask_name) return -1;
    newrc = ::wxFileSelector ("Load which configuration file?", 
	"", oldrc, "", "", wxOPEN);
  }
  if (newrc.IsEmpty () || (newrc == "none")) return 0;
  if (SIM->read_rc ((char *)newrc.c_str ()) < 0) {
    wxMessageBox ("Error loading configuration file");
    return -1;
  }
  panel->forAllEditors (ID_RevertAction, paramEditorsById);
  return 0;
}

int 
MyFrame::WriteConfiguration (int ask_name)
{
  char oldrc_cstr[CPANEL_PATH_LEN];
  int found_default = 1;
  if (SIM->get_default_rc (oldrc_cstr, CPANEL_PATH_LEN) < 0) {
    found_default = 0;
    oldrc_cstr[0] = 0;
  }
  wxString oldrc (oldrc_cstr), newrc = oldrc;
  if (ask_name || !found_default) {
    newrc = ::wxFileSelector ("Save Configuration As", 
	"", oldrc, "", "", wxSAVE | wxOVERWRITE_PROMPT);
  }
  if (newrc.IsEmpty () || (newrc == "none")) return 0;
  if (SIM->write_rc ((char *)newrc.c_str (), 1) < 0) {
    wxMessageBox ("Error while saving configuration file");
    return -1;
  }
  return 0;
}

// static function version that can be referred to by function pointer
int 
MyFrame::ProcessBochsEvent (void *class_ptr, int id)
{
  printf ("MyFrame::ProcessBochsEvent\n");
  MyFrame *f = (MyFrame *)class_ptr;
  f->ProcessBochsEvent2 (id);
}

int 
MyFrame::ProcessBochsEvent2 (int code)
{
  printf ("MyFrame::ProcessBochsEvent2\n");
  switch (code)
  {
  case NOTIFY_CODE_LOGMSG:
    if (0) {
      // dumb static choices for now, I'll fix it once I get it to work
      //
      // this code works if the gui thread calls it, but it's doing bad
      // things when it gets called from the bochs thread.
      // I know it's legal for a non-GUI thread to enter a mutex, update
      // an object like "m_frame->WriteText(msg)" and then leave the mutex.
      // But this code puts up a modal dialog and waits for user input.
      // If you comment out the mutex code, you get:
      // GLib-WARNING **: g_main_run(): called recursively from within a source's check() or prepare() member or from a second thread, iteration not possible
      // And if you uncomment the mutex, it displays the dialog correctly
      // but then clicking on it does nothing...deadlock.
      // Maybe this must be resolved by posting a message between threads
      // instead.

      wxMutexGuiEnter();
      static wxString choices[] = { "Die", "Continue", "Continue and Disable" };
      char prefix[512], msg[512];
      int level;
      assert (SIM->log_msg_2 (prefix, &level, msg, sizeof(msg)) >= 0);
      wxString string;
      string.Printf ("%s", msg);
      int choice = ::wxGetSingleChoiceIndex (
	  string,
	  wxString (SIM->get_log_level_name (level)), 3, choices);
      wxMutexGuiLeave();
      printf ("you chose %d\n", choice);
    } else {
  //////////// temporary! still implement this with stdin/stdout ///////////
      int level;
      char prefix[512], msg[512];
      assert (SIM->log_msg_2 (prefix, &level, msg, sizeof(msg)) >= 0);
      fprintf (stderr, "========================================================================\n");
      fprintf (stderr, "Event type: %s\n", SIM->get_log_level_name (level));
      fprintf (stderr, "Device: %s\n", prefix);
      fprintf (stderr, "Message: %s\n\n", msg);
      fprintf (stderr, "A %s has occurred.  Do you want to:\n", SIM->get_log_level_name (level));
      fprintf (stderr, "  cont       - continue execution\n");
      fprintf (stderr, "  alwayscont - continue execution, and don't ask again.\n");
      fprintf (stderr, "               This affects only %s events from device %s\n", SIM->get_log_level_name (level), prefix);
      fprintf (stderr, "  die        - stop execution now\n");
      int choice;
      extern int ask_menu (char *prompt, int n_choices, char *choice[], int the_default, int *out);
      char *log_action_ask_choices[] = { "cont", "alwayscont", "die" };
      int log_action_n_choices = 3;
      if (ask_menu ("Choose cont, alwayscont, or die. [%s] ", 3,  
	    log_action_ask_choices, 2, &choice) < 0) 
	return SIM->notify_return(-1);
      // return 0 for continue, 1 for alwayscontinue, 2 for die.
      SIM->notify_return(choice);
    }
    break;
  default:
    fprintf (stderr, "Control panel: notify callback called with unknown code %04x\n", code);
  }
}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close( TRUE );
  if (bochsThread != NULL)
    bochsThread->Delete ();
}

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
int wxid_count = ID_LAST_STATIC_ASSIGNED_ID;

IMPLEMENT_APP(BochsApp)

bool BochsApp::OnInit()
{
  // do bochs initialization first
  siminterface_init ();
  MyFrame *frame = new MyFrame( "Bochs Control Panel", wxPoint(50,50), wxSize(450,340) );
  SIM->set_notify_callback (frame->ProcessBochsEvent, frame);
  bochsEventHandler = new BochsEventHandler ();
  frame->Show( TRUE );
  SetTopWindow( frame );
  return TRUE;
}


void MyPanel::HandleEvent (wxCommandEvent& evt)
{
  int id = evt.GetId ();
  printf ("Handle event with event id %d\n", id);
  switch (id)
  {
    case ID_StartBochs:
      frame->StartBochsThread ();
      break;
    case ID_ApplyAction:
    case ID_RevertAction:
      forAllEditors (id, paramEditorsById);
      break;
    case ID_ReadConfig:
      frame->ReadConfiguration (1);
      break;
    case ID_WriteConfig:
      frame->WriteConfiguration (0);
      break;
    case ID_WriteConfigAs:
      frame->WriteConfiguration (1);
      break;
    default:
      printf ("HandleEvent called with id=%d\n", id);
  }
}

void 
MyPanel::forAllEditors (int evt_id, wxHashTable& hash)
{
  printf ("MyPanel::forAllEditors");
  // iterate over editors in the editors hashtable
  hash.BeginFind ();
  wxNode *node;
  while (node = hash.Next ()) 
  {
    ParamEditor *editor = (ParamEditor *)node->Data ();
    editor->Action (evt_id);
  }
}

void *
BochsThread::Entry (void)
{
  int argc=1;
  char *argv[] = {"bochs"};
  bx_continue_after_control_panel (argc, argv);
}

MyPanel::MyPanel (MyFrame *frame, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
  : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER | wxTAB_TRAVERSAL, "Bochs Controls" )
{
  this->frame = frame;
  // the parent constructor makes this panel a child of frame, with 
  // proper size.
  (void) new wxButton (this, ID_StartBochs, "Start Bochs", wxPoint (10, 10));
  (void) new wxButton (this, ID_ApplyAction, "Apply", wxPoint (110, 10));
  (void) new wxButton (this, ID_RevertAction, "Revert", wxPoint (210, 10));
  //wxCheckBox *check1 = new wxCheckBox (this, ID_Check1, "Checkbox", wxPoint (10, 50));
  //wxString choices[] = { "apple", "orange", "banana" };
  //wxChoice *choice1 = new wxChoice (this, ID_Choice1, wxPoint (10, 80), wxSize (80, 30), 3, choices);
  //wxComboBox *combo1 = new wxComboBox (this, ID_Combo1, "Default Combo String", wxPoint (10, 130), wxSize (180, 80), 3, choices);
  //text1 = new wxStaticText (this, ID_Text1, "no text yet", wxPoint (0, 160));
  frame->ReadConfiguration (0);
  buildParamList (10, 40);
  // grab all the settings from siminterface into the GUI
  forAllEditors (ID_RevertAction, paramEditorsById);
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
      int wxid = wxid_count++;
      ParamEditor *editor = ParamEditor::createEditor (param, this, wxid, x+120, y);
      y+=30;
    }
  }
}

/////////////////////////////////////////////////////////////////////////
// Parameter editor classes
/////////////////////////////////////////////////////////////////////////

ParamEditor::ParamEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : wxPanel(parent, id, wxPoint(x, y), wxSize (300, 30))
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
  ParamEditor *it = NULL;
  switch (type) {
    case BXT_PARAM_BOOL:
      it = new ParamEditorBool (param, parent, id, x, y);
      break;
    case BXT_PARAM_NUM:
      it = new ParamEditorNum (param, parent, id, x, y);
      break;
    case BXT_PARAM_STRING:
      it = new ParamEditorString (param, parent, id, x, y);
      break;
    case BXT_PARAM_ENUM:
      it = new ParamEditorEnum (param, parent, id, x, y);
      break;
    case BXT_LIST:
      it = new ParamEditorList (param, parent, id, x, y);
      break;
    default:
      printf ("ParamEditor::createEditor with unknown type %d\n", type);
      it = NULL;
  }
  paramEditorsById.Put (id, it);
  return it;
}

ParamEditorBool::ParamEditorBool (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  component = new wxCheckBox (this, id, "", wxPoint (0, 0));
}

void 
ParamEditorBool::Action (int evt_id)
{
  printf ("ParamEditorBool::Action\n");
  bx_param_bool_c *param = (bx_param_bool_c *) get_param ();
  switch (evt_id) {
    case ID_ApplyAction:
      param->set (component->GetValue ());
      break;
    case ID_RevertAction:
      component->SetValue (param->get ());
      break;
    default:
      wxASSERT (0);
  }
}

ParamEditorNum::ParamEditorNum (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  // background color helps to visualize the borders of the panel
  //SetBackgroundColour (wxColour ("red"));
  bx_param_num_c *num = (bx_param_num_c *)param;
  wxString text;
  text.Printf ("%d", num->get ());
  component = new wxTextCtrl (this, id, text, wxPoint (-1, -1), wxSize (-1, -1), wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB);
}

void 
ParamEditorNum::Action (int evt_id)
{
  printf ("ParamEditorNum::Action\n");
  bx_param_num_c *param = (bx_param_num_c *) get_param ();
  int base = param->get_base ();
  wxASSERT (base==10 || base==16);
  switch (evt_id) {
    case ID_ApplyAction:
      {
	const char *string = component->GetValue ().c_str ();
	int n, status;
	if (base==10) status = wxSscanf (string, "%d", &n);
	else status = wxSscanf (string, "%x", &n);
	if (status != 1) {
	  printf ("Invalid number in %s, value = '%s'\n", param->get_name (), string);
	  return;
	}
	param->set (n);
      }
      break;
    case ID_RevertAction:
      {
	int n = param->get ();
	wxString string;
	if (base == 10) string.Printf ("%d", n);
	else string.Printf ("%x", n);
        component->SetValue (string);
      }
      break;
    default:
      wxASSERT (0);
  }
}

ParamEditorString::ParamEditorString (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  //SetBackgroundColour (wxColour ("red"));
  bx_param_string_c *string = (bx_param_string_c *)param;
  wxString text = wxString (string->getptr ());
  component = new wxTextCtrl (this, id, text, wxPoint (-1, -1), wxSize (-1, -1), wxTE_PROCESS_ENTER);
}

void 
ParamEditorString::Action (int evt_id)
{
  bx_param_string_c *param = (bx_param_string_c *) get_param ();
  const char *string;
  switch (evt_id) {
    case ID_ApplyAction:
      string = component->GetValue ().c_str ();
      param->set ((char *)string);
      break;
    case ID_RevertAction:
      component->SetValue (wxString (param->getptr ()));
      break;
    default:
      wxASSERT (0);
  }
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

void 
ParamEditorEnum::Action (int evt_id)
{
  printf ("ParamEditorEnum::Action\n");
  bx_param_enum_c *param = (bx_param_enum_c *) get_param ();
  int min = param->get_min ();
  switch (evt_id) {
    case ID_ApplyAction:
      param->set (min + component->GetSelection ());
      break;
    case ID_RevertAction:
      component->SetSelection (param->get () - min);
      break;
    default:
      wxASSERT (0);
  }
}



ParamEditorList::ParamEditorList (bx_param_c *genericparam, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (genericparam, parent, id, x, y)
{
  //SetBackgroundColour (wxColour ("red"));
  bx_list_c *param = (bx_list_c *) genericparam;
  component = new wxStaticText (this, id, "list param not implemented", wxPoint (-1, -1));
}

void 
ParamEditorList::Action (int evt_id)
{
  printf ("ParamEditorList::Action\n");
}

bx_param_c *
ParamEditor::get_param ()
{
  printf ("You called get_param on ParamEditor %p\n", this);
  printf ("My param value is %p\n", this->param);
  return this->param;
}

void
ParamEditor::OnEvent (wxCommandEvent& evt)
{
  // if you want to run some action on the object that sent this event,
  // see the code below.
#if 0
  int id = evt.GetId ();
  wxObject *object = evt.GetEventObject ();
  printf ("ParamEditor::OnEvent, id=%d, object=%p\n", id, object);
  ParamEditor *genericeditor = (ParamEditor *) paramEditorsById.Get (id);
  if (genericeditor == NULL) {
    printf ("I couldn't find the parameter editor for id=%d\n", id);
    return;
  }
  bx_param_c *genericparam = genericeditor->get_param ();
  wxASSERT (genericparam != NULL);
  printf ("This event will be sent to %s\n", genericparam->get_name ());
  genericeditor->Action (evt);
#endif
}
