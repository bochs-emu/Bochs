//
// wxmain.cc
// Main program for wxWindows.  This does not replace main.cc by any means.
// It just provides the program entry point, and calls functions in main.cc
// when it's appropriate.
//
// Note that this file does NOT include bochs.h.  In an attempt to keep the
// interface between the siimulator and its gui clean and well defined, all 
// communication is done through the siminterface object.
//
// Types of events between threads:
// - async events from gui to sim. example: start, pause, resume, keypress
// - async events from sim to gui. example: log message
// - synchronous events from sim to gui. The sim will block forever until the
//   gui responds to it.
//
// Sync events are what I need first.  The simulator calls a siminterface
// function which creates an event and calls sim_to_gui_event (event).
// This function clears the synchronous event mailbox, posts the event onto the
// GUI event queue, then blocks forever until the GUI places the response event
// in the mailbox.  Then it clears the mailbox and returns the response event.

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "config.h"
extern "C" {
// siminterface needs stdio.h
#include <stdio.h>
}
#include "osdep.h"
#include "gui/control.h"
#include "gui/siminterface.h"

class MyApp: public wxApp
{
virtual bool OnInit();
};

class MyFrame;

class SimThread: public wxThread
{
  MyFrame *frame;

  // when the sim thread sends a synchronous event to the GUI thread, the
  // response is stored in sim2gui_mailbox.
  // FIXME: this would be cleaner and more reusable if I made a general
  // thread-safe mailbox class.
  BxEvent *sim2gui_mailbox;
  wxCriticalSection sim2gui_mailbox_lock;

public:
  SimThread (MyFrame *_frame) { frame = _frame; }
  virtual ExitCode Entry ();
  void OnExit ();
  // called by the siminterface code, with the pointer to the sim thread
  // in the thisptr arg.
  static BxEvent *SiminterfaceCallback (void *thisptr, BxEvent *event);
  BxEvent *SiminterfaceCallback2 (BxEvent *event);
  // methods to coordinate synchronous response mailbox
  void ClearSyncResponse ();
  void SendSyncResponse (BxEvent *);
  BxEvent *GetSyncResponse ();
private:
};

class MyFrame: public wxFrame
{
public:
MyFrame(const wxString& title, const wxPoint& pos, const wxSize&
size);
void OnQuit(wxCommandEvent& event);
void OnAbout(wxCommandEvent& event);
void OnStartSim(wxCommandEvent& event);
void OnPauseResumeSim(wxCommandEvent& event);
void OnKillSim(wxCommandEvent& event);
void OnSim2GuiEvent(wxCommandEvent& event);
int HandleAskParam (BxEvent *event);
int HandleAskParamString (bx_param_string_c *param);

// called from the sim thread's OnExit() method.
void OnSimThreadExit ();

private:
wxCriticalSection sim_thread_lock;
SimThread *sim_thread; // get the lock before accessing sim_thread
int HandleVgaGuiButton (bx_id param);
int start_bochs_times;
wxMenu *menuConfiguration;
wxMenu *menuEdit;
wxMenu *menuSimulate;
wxMenu *menuDebug;
wxMenu *menuLog;
wxMenu *menuHelp;

DECLARE_EVENT_TABLE()
};

enum
{
ID_Quit = 1,
ID_Config_New,
ID_Config_Read,
ID_Config_Save,
ID_Edit_Disks,
ID_Edit_Boot,
ID_Edit_Vga,
ID_Edit_Memory,
ID_Edit_Sound,
ID_Edit_Network,
ID_Edit_Keyboard,
ID_Edit_Other,
ID_Simulate_Start,
ID_Simulate_PauseResume,
ID_Simulate_Stop,
ID_Simulate_Speed,
ID_Debug_ShowCpu,
ID_Debug_ShowMemory,
ID_Log_View,
ID_Log_Prefs,
ID_Log_PrefsDevice,
ID_Help_About,
ID_Sim2Gui_Event,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Quit, MyFrame::OnQuit)
EVT_MENU(ID_Help_About, MyFrame::OnAbout)
EVT_MENU(ID_Simulate_Start, MyFrame::OnStartSim)
EVT_MENU(ID_Simulate_PauseResume, MyFrame::OnPauseResumeSim)
EVT_MENU(ID_Simulate_Stop, MyFrame::OnKillSim)
EVT_MENU(ID_Sim2Gui_Event, MyFrame::OnSim2GuiEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  siminterface_init ();
  MyFrame *frame = new MyFrame( "Bochs Control Panel", wxPoint(50,50),
  wxSize(450,340) );
  frame->Show( TRUE );
  SetTopWindow( frame );
  return TRUE;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const
wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  // init variables
  sim_thread = NULL;
  start_bochs_times = 0;

  // set up the gui
  menuConfiguration = new wxMenu;
  menuConfiguration->Append( ID_Config_New, "&New Configuration" );
  menuConfiguration->Append( ID_Config_Read, "&Read Configuration" );
  menuConfiguration->Append( ID_Config_Save, "&Save Configuration" );
  menuConfiguration->AppendSeparator ();
  menuConfiguration->Append (ID_Quit, "&Quit");

  menuEdit = new wxMenu;
  menuEdit->Append( ID_Edit_Disks, "&Disks..." );
  menuEdit->Append( ID_Edit_Boot, "&Boot..." );
  menuEdit->Append( ID_Edit_Vga, "&VGA..." );
  menuEdit->Append( ID_Edit_Memory, "&Memory..." );
  menuEdit->Append( ID_Edit_Sound, "&Sound..." );
  menuEdit->Append( ID_Edit_Network, "&Network..." );
  menuEdit->Append( ID_Edit_Keyboard, "&Keyboard..." );
  menuEdit->Append( ID_Edit_Other, "&Other..." );

  menuSimulate = new wxMenu;
  menuSimulate->Append( ID_Simulate_Start, "&Start...");
  menuSimulate->Append( ID_Simulate_PauseResume, "&Pause...");
  menuSimulate->Append( ID_Simulate_Stop, "S&top...");
  menuSimulate->AppendSeparator ();
  menuSimulate->Append( ID_Simulate_Speed, "S&peed...");
  menuSimulate->Enable (ID_Simulate_PauseResume, FALSE);
  menuSimulate->Enable (ID_Simulate_Stop, FALSE);

  menuDebug = new wxMenu;
  menuDebug->Append (ID_Debug_ShowCpu, "Show &CPU");
  menuDebug->Append (ID_Debug_ShowMemory, "Show &memory");

  menuLog = new wxMenu;
  menuLog->Append (ID_Log_View, "&View");
  menuLog->Append (ID_Log_Prefs, "&Preferences...");
  menuLog->Append (ID_Log_PrefsDevice, "By &Device...");

  menuHelp = new wxMenu;
  menuHelp->Append( ID_Help_About, "&About..." );

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuConfiguration, "&File" );
  menuBar->Append( menuEdit, "&Edit" );
  menuBar->Append( menuSimulate, "&Simulate" );
  menuBar->Append( menuDebug, "&Debug" );
  menuBar->Append( menuLog, "&Log" );
  menuBar->Append( menuHelp, "&Help" );
  SetMenuBar( menuBar );
  CreateStatusBar();
  SetStatusText( "Welcome to wxWindows!" );
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
  Close( TRUE );
  OnKillSim (event);
#if 0
  if (SIM)
	SIM->quit_sim(0);  // give bochs a chance to shut down
#endif
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
wxMessageBox( "wxWindows Control Panel for Bochs. (Very Experimental)",
"Bochs Control Panel", wxOK | wxICON_INFORMATION );
}

void MyFrame::OnStartSim(wxCommandEvent& WXUNUSED(event))
{
  wxCriticalSectionLocker lock(sim_thread_lock);
  if (sim_thread != NULL) {
	wxMessageBox (
	  "Can't start Bochs simulator, because it is already running",
	  "Already Running", wxOK | wxICON_ERROR);
	return;
  }
  wxLogStatus ("Starting a Bochs simulation\n");
  start_bochs_times++;
  if (start_bochs_times>1) {
	wxMessageBox (
	"You have already started the simulator once this session. Due to memory leaks, you may get unstable behavior.",
	"2nd time warning", wxOK | wxICON_WARNING);
  }
  sim_thread = new SimThread (this);
  sim_thread->Create ();
  sim_thread->Run ();                                                        
  wxLogDebug ("Simulator thread has started.\n");
  // set up callback for events from simulator thread
  SIM->set_notify_callback (&SimThread::SiminterfaceCallback, sim_thread);
  // fix up menu choices
  menuSimulate->Enable (ID_Simulate_Start, FALSE);
  menuSimulate->Enable (ID_Simulate_PauseResume, TRUE);
  menuSimulate->Enable (ID_Simulate_Stop, TRUE);
  menuSimulate->SetLabel (ID_Simulate_PauseResume, "&Pause");
}

void MyFrame::OnPauseResumeSim(wxCommandEvent& WXUNUSED(event))
{
  wxCriticalSectionLocker lock(sim_thread_lock);
  if (sim_thread) {
    if (sim_thread->IsPaused ()) {
	  SetStatusText ("Resuming the Bochs simulation");
	  sim_thread->Resume ();
	  menuSimulate->SetLabel (ID_Simulate_PauseResume, "&Pause");
	} else {
	  SetStatusText ("Pausing the Bochs simulation");
	  sim_thread->Pause ();
	  menuSimulate->SetLabel (ID_Simulate_PauseResume, "&Resume");
	}
  }
}

void MyFrame::OnKillSim(wxCommandEvent& WXUNUSED(event))
{
  // DON'T use a critical section here.  Delete implicitly calls
  // OnSimThreadExit, which also tries to lock sim_thread_lock.
  // If we grab the lock at this level, deadlock results.
  if (sim_thread) {
	SetStatusText ("Killing the Bochs simulation");
	sim_thread->Delete ();
	menuSimulate->Enable (ID_Simulate_Start, TRUE);
	menuSimulate->Enable (ID_Simulate_PauseResume, FALSE);
	menuSimulate->Enable (ID_Simulate_Stop, FALSE);
	menuSimulate->SetLabel (ID_Simulate_PauseResume, "&Pause");
  }
}

void
MyFrame::OnSimThreadExit () {
  wxCriticalSectionLocker lock (sim_thread_lock);
  sim_thread = NULL; 
}

int 
MyFrame::HandleAskParamString (bx_param_string_c *param)
{
  bx_param_num_c *opt = param->get_options ();
  wxASSERT (opt != NULL);
  int n_opt = opt->get ();
  char *msg = param->get_ask_format ();
  if (!msg) msg = param->get_description ();
  char *newval = NULL;
  wxDialog *dialog = NULL;
  if (n_opt & param->BX_IS_FILENAME) {
    // use file open dialog
	long style = 
	  (n_opt & param->BX_SAVE_FILE_DIALOG) ? wxSAVE|wxOVERWRITE_PROMPT : wxOPEN;
	wxFileDialog *fdialog = new wxFileDialog (this, msg, "", "", "*.*", style);
	if (fdialog->ShowModal() == wxID_OK)
	  newval = (char *)fdialog->GetPath().c_str ();
	dialog = fdialog; // so I can delete it
  } else {
    // use simple string dialog
	long style = wxOK|wxCANCEL;
	wxTextEntryDialog *tdialog = new wxTextEntryDialog (this, msg, "Enter new value", wxString(param->getptr ()), style);
	if (tdialog->ShowModal() == wxID_OK)
	  newval = (char *)tdialog->GetValue().c_str ();
	dialog = tdialog; // so I can delete it
  }
  // newval points to memory inside the dialg.  As soon as dialog is deleted, newval points
  // to junk.  So be sure to copy the text out before deleting it!
  if (newval && strlen(newval)>0) {
	// change floppy path to this value.
	wxLogDebug ("Setting param %s to '%s'\n", param->get_name (), newval);
	param->set (newval);
	delete dialog;
	return 1;
  }
  delete dialog;
  return -1;
}

// This is called when the simulator needs to ask the user to choose
// a value or setting.  For example, when the user indicates that he wants
// to change the floppy disk image for drive A, an ask-param event is created
// with the parameter id set to BXP_FLOPPYA_PATH.  The simulator blocks until
// the gui has displayed a dialog and received a selection from the user.
// In the current implemention, the GUI will look up the parameter's 
// data structure using SIM->get_param() and then call the set method on the
// parameter to change the param.  The return value only needs to return
// success or failure (failure = cancelled, or not implemented).
// Returns 1 if the user chose a value and the param was modified.
// Returns 0 if the user cancelled.
// Returns -1 if the gui doesn't know how to ask for that param.
int 
MyFrame::HandleAskParam (BxEvent *event)
{
  wxASSERT (event->type == BX_SYNC_EVT_ASK_PARAM);
  bx_param_c *param = event->u.param.param;
  Raise ();  // bring control panel to front so that you will see the dialog
  switch (param->get_type ())
  {
  case BXT_PARAM_STRING:
    return HandleAskParamString ((bx_param_string_c *)param);
  default:
    {
	  wxString msg;
	  msg.Printf ("ask param for parameter type %d is not implemented in wxWindows");
	  wxMessageBox( msg, "not implemented", wxOK | wxICON_ERROR );
	  return -1;
	}
  }
#if 0
  switch (param) {
  case BXP_FLOPPYA_PATH:
  case BXP_FLOPPYB_PATH:
  case BXP_DISKC_PATH:
  case BXP_DISKD_PATH:
  case BXP_CDROM_PATH:
	{
	  Raise();  // bring the control panel to front so dialog shows
	  char *msg;
	  if (param==BXP_FLOPPYA_PATH || param==BXP_FLOPPYB_PATH)
	    msg = "Choose new floppy disk image file";
      else if (param==BXP_DISKC_PATH || param==BXP_DISKD_PATH)
	    msg = "Choose new hard disk image file";
      else if (param==BXP_CDROM_PATH)
	    msg = "Choose new CDROM image file";
	  else
	    msg = "Choose new image file";
	  wxFileDialog dialog(this, msg, "", "", "*.*", 0);
	  int ret = dialog.ShowModal();
	  if (ret == wxID_OK)
	  {
	    char *newpath = (char *)dialog.GetPath().c_str ();
	    if (newpath && strlen(newpath)>0) {
	      // change floppy path to this value.
	      bx_param_string_c *Opath = SIM->get_param_string (param);
	      assert (Opath != NULL);
	      wxLogDebug ("Setting floppy %c path to '%s'\n", 
		    param == BXP_FLOPPYA_PATH ? 'A' : 'B',
		    newpath);
	      Opath->set (newpath);
	      return 1;
	    }
	  }
	  return 0;
	}
  default:
	wxLogError ("HandleAskParam: parameter %d, not implemented\n", event->u.param.id);
  }
#endif
  return -1;  // could not display
}

// This is called when a Sim2Gui event is discovered on the GUI thread's
// event queue.  (It got there via wxPostEvent in SiminterfaceCallback2, 
// which is executed in the simulator Thread.)
void 
MyFrame::OnSim2GuiEvent (wxCommandEvent& event)
{
  wxLogDebug ("received a bochs event in the GUI thread\n");
  BxEvent *be = (BxEvent *) event.GetEventObject ();
  wxLogDebug ("event type = %d\n", (int) be->type);
  // all cases should return.  sync event handlers MUST send back a 
  // response.
  switch (be->type) {
  case BX_SYNC_EVT_ASK_PARAM:
    be->retcode = HandleAskParam (be);
    // sync must return something; just return a copy of the event.
    sim_thread->SendSyncResponse(be);
	return;
  case BX_ASYNC_EVT_SHUTDOWN_GUI:
	wxLogDebug ("control panel is exiting\n");
    Close (TRUE);
	wxExit ();
	return;
  case BX_ASYNC_EVT_LOG_MSG:
    wxLogDebug ("log msg: level=%d, prefix='%s', msg='%s'\n",
	  be->u.logmsg.level,
	  be->u.logmsg.prefix,
	  be->u.logmsg.msg);
    return;
  default:
    wxLogDebug ("OnSim2GuiEvent: event type %d ignored\n", (int)be->type);
	// assume it's a synchronous event and send back a response, to avoid
	// potential deadlock.
    sim_thread->SendSyncResponse(be);
	return;
  }
  // it is critical to send a response back eventually since the sim thread
  // is blocking.
  wxASSERT_MSG (0, "switch stmt should have returned");
}

/////////////// bochs thread

void *
SimThread::Entry (void)
{     
  int argc=1;
  char *argv[] = {"bochs"};
  wxLogDebug ("in SimThread, starting at bx_continue_after_control_panel\n");
  // run all the rest of the Bochs simulator code.  This function will
  // run forever, unless a "kill_bochs_request" is issued.  The procedure
  // is as follows:
  //   - user selects "Kill Simulation" or GUI decides to kill bochs
  //   - GUI calls sim_thread->Delete ()
  //   - sim continues to run until the next time it reaches SIM->periodic().
  //   - SIM->periodic() sends a synchronous tick event to the GUI, which
  //     finally calls TestDestroy() and realizes it needs to stop.  It
  //     sets the sync event return code to -1.  SIM->periodic() sets the
  //     kill_bochs_request flag in cpu #0.
  //   - cpu loop notices kill_bochs_request and returns to main.cc:
  //     bx_continue_after_control_panel(), which notices the
  //     kill_bochs_request and returns back to this Entry() function.
  //   - Entry() exits and the thread stops. Whew.
  bx_continue_after_control_panel (argc, argv);
  wxLogDebug ("in SimThread, bx_continue_after_control_panel exited\n");
  return NULL;
}

void
SimThread::OnExit ()
{
  // notify the MyFrame that the bochs thread has died.  I can't adjust
  // the sim_thread directly because it's private.
  frame->OnSimThreadExit ();
  // don't use this SimThread's callback function anymore.
  SIM->set_notify_callback (NULL, NULL);
}

// return 1 if the user selected a value.
// return 0 if the question was displayed and the user cancelled.
// return -1 if we don't know how to display the question.
int
MyFrame::HandleVgaGuiButton (bx_id param)
{
  wxLogDebug ("HandleVgaGuiButton: button %d was pressed\n", (int)param);
  switch (param)
  {
	case BXP_FLOPPYA_PATH:
	case BXP_FLOPPYB_PATH:
	case BXP_DISKC_PATH:
	case BXP_DISKD_PATH:
	case BXP_CDROM_PATH:
	default:
	  wxLogDebug ("HandleVgaGuiButton: button %d not recognized\n", param);
	  return -1;
  }
}

// This function is declared static so that I can get a usable function
// pointer for it.  The function pointer is passed to SIM->set_notify_callback
// so that the siminterface can call this function when it needs to contact
// the gui.  It will always be called with a pointer to the SimThread
// as the first argument.
BxEvent *
SimThread::SiminterfaceCallback (void *thisptr, BxEvent *event)
{
  SimThread *me = (SimThread *)thisptr;
  // call the normal non-static method now that we know the this pointer.
  return me->SiminterfaceCallback2 (event);
}

// callback function for sim thread events.  This is called from
// the sim thread, not the GUI thread.  So any GUI actions must be
// thread safe.  Most events are handled by packaging up the event
// in a wxEvent of some kind, and posting it to the GUI thread for
// processing.
BxEvent *
SimThread::SiminterfaceCallback2 (BxEvent *event)
{
  //wxLogDebug ("SiminterfaceCallback with event type=%d\n", (int)event->type);
  event->retcode = 0;  // default return code
  int async = BX_EVT_IS_ASYNC(event->type);
  if (!async) {
    // for synchronous events, clear away any previous response.  There
	// can only be one synchronous event pending at a time.
    ClearSyncResponse ();
	event->retcode = -1;   // default to error
  }

  // tick event must be handled right here in the bochs thread.
  if (event->type == BX_SYNC_EVT_TICK) {
	if (TestDestroy ()) {
	  // tell simulator to quit
	  event->retcode = -1;
	} else {
	  event->retcode = 0;
	}
	return event;
  }

  //encapsulate the bxevent in a wxwindows event
  wxCommandEvent wxevent (wxEVT_COMMAND_MENU_SELECTED, ID_Sim2Gui_Event);
  wxevent.SetEventObject ((wxEvent *)event);
  wxLogDebug ("Sending an event to the window\n");
  wxPostEvent (frame, wxevent);
  // if it is an asynchronous event, return immediately
  if (async) return NULL;
  wxLogDebug ("SiminterfaceCallback2: synchronous event; waiting for response\n");
  // now wait forever for the GUI to post a response.
  BxEvent *response = NULL;
  while (response == NULL) {
	response = GetSyncResponse ();
	if (!response) {
	  wxLogDebug ("no sync response yet, waiting\n");
	  this->Sleep(500);
	}
  }
  wxASSERT (response != NULL);
  return response;
}

void 
SimThread::ClearSyncResponse ()
{
  wxCriticalSectionLocker lock(sim2gui_mailbox_lock);
  if (sim2gui_mailbox != NULL) {
    wxLogDebug ("WARNING: ClearSyncResponse is throwing away an event that was previously in the mailbox\n");
  }
  sim2gui_mailbox = NULL;
}

void 
SimThread::SendSyncResponse (BxEvent *event)
{
  wxCriticalSectionLocker lock(sim2gui_mailbox_lock);
  if (sim2gui_mailbox != NULL) {
    wxLogDebug ("WARNING: SendSyncResponse is throwing away an event that was previously in the mailbox\n");
  }
  sim2gui_mailbox = event;
}

BxEvent *
SimThread::GetSyncResponse ()
{
  wxCriticalSectionLocker lock(sim2gui_mailbox_lock);
  BxEvent *event = sim2gui_mailbox;
  sim2gui_mailbox = NULL;
  return event;
}
