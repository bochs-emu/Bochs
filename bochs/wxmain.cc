//
// wxmain.cc
// Main program for wxWindows.  This does not replace main.cc by any means.
// It just provides the program entry point, and calls functions in main.cc
// when it's appropriate.
//
// Note that this file does NOT include bochs.h.  In an attempt to keep the
// interface between Bochs and its gui clean and well defined, all 
// communication is done through the siminterface object.
//
// Types of events between threads:
// - async events from gui to Bochs. example: start, pause, resume, keypress
// - async events from Bochs to gui. example: log message
// - sync events from Bochs to gui. Bochs will block forever until the
//   gui responds to it.
//
// Sync events are what I need first.  Bochs calls siminterface function
// which creates an event and calls wx_bochs2gui_sync_event (event).  This
// function clears the synchronous event mailbox, posts the event onto 
// the GUI event queue, then blocks forever until the GUI places the response
// event in the mailbox.  Then it clears the mailbox and returns the response
// event.

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

FILE *wxlog = NULL;

class MyApp: public wxApp
{
virtual bool OnInit();
};

class MyFrame;

class BochsThread: public wxThread
{
  // when Bochs sends a synchronous event to the GUI thread, the response
  // is stored here.
  MyFrame *frame;

  // support response to a synchronous event.
  // FIXME: this would be cleaner and more reusable if I made a thread-safe
  // mailbox class.
  BxEvent *bochs2gui_mailbox;
  wxCriticalSection bochs2gui_mailbox_lock;

public:
  BochsThread (MyFrame *_frame) { frame = _frame; }
  virtual ExitCode Entry ();
  void OnExit ();
  // called by the siminterface code, with the pointer to the Bochs thread
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
void OnStartBochs(wxCommandEvent& event);
void OnPauseBochs(wxCommandEvent& event);
void OnResumeBochs(wxCommandEvent& event);
void OnKillBochs(wxCommandEvent& event);
void OnBochs2GuiEvent(wxCommandEvent& event);
int HandleAskParam (BxEvent *event);
int StartBochsThread ();

// called from the Bochs thread's OnExit() method.
void OnBochsThreadExit ();

private:
wxCriticalSection bochs_thread_lock;
BochsThread *bochs_thread; // get the lock before accessing bochs_thread
int HandleVgaGuiButton (bx_id param);

DECLARE_EVENT_TABLE()
};

enum
{
ID_Quit = 1,
ID_About,
ID_StartBochs,
ID_PauseBochs,
ID_ResumeBochs,
ID_KillBochs,
ID_Bochs2Gui_Event,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Quit, MyFrame::OnQuit)
EVT_MENU(ID_About, MyFrame::OnAbout)
EVT_MENU(ID_StartBochs, MyFrame::OnStartBochs)
EVT_MENU(ID_PauseBochs, MyFrame::OnPauseBochs)
EVT_MENU(ID_ResumeBochs, MyFrame::OnResumeBochs)
EVT_MENU(ID_KillBochs, MyFrame::OnKillBochs)
EVT_MENU(ID_Bochs2Gui_Event, MyFrame::OnBochs2GuiEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  wxlog = fopen ("wxlog.txt", "w");
  wxASSERT_MSG (wxlog != NULL, "could not open wxlog.txt");
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
  bochs_thread = NULL;

  // set up the gui
  wxMenu *menuFile = new wxMenu;
  menuFile->Append( ID_StartBochs, "&Start Bochs" );
  menuFile->Append( ID_PauseBochs, "&Pause Bochs" );
  menuFile->Append( ID_ResumeBochs, "&Resume Bochs" );
  menuFile->Append( ID_KillBochs, "&Kill Bochs" );
  menuFile->Append( ID_About, "&About..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, "E&xit" );
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile, "&File" );
  SetMenuBar( menuBar );
  CreateStatusBar();
  SetStatusText( "Welcome to wxWindows!" );
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
  Close( TRUE );
  OnKillBochs (event);
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

void MyFrame::OnStartBochs(wxCommandEvent& WXUNUSED(event))
{
  wxCriticalSectionLocker lock(bochs_thread_lock);
  if (bochs_thread != NULL) {
	wxMessageBox (
	  "Can't start Bochs simulator, because it is already running",
	  "Already Running", wxOK | wxICON_ERROR);
	return;
  }
  fprintf (wxlog, "Starting a Bochs simulation\n");
  SetStatusText ("Starting a Bochs simulation");
  bochs_thread = new BochsThread (this);
  bochs_thread->Create ();
  bochs_thread->Run ();                                                        
  fprintf (wxlog, "Bochs thread has started.\n");
  // set up callback for events from Bochs
  SIM->set_notify_callback (&BochsThread::SiminterfaceCallback, bochs_thread);
}

void MyFrame::OnPauseBochs(wxCommandEvent& WXUNUSED(event))
{
  wxCriticalSectionLocker lock(bochs_thread_lock);
  if (bochs_thread && !bochs_thread->IsPaused ()) {
	SetStatusText ("Pausing the Bochs simulation");
	bochs_thread->Pause ();
  }
}

void MyFrame::OnResumeBochs(wxCommandEvent& WXUNUSED(event))
{
  // use crit section to ensure safe use of bochs_thread pointer.
  wxCriticalSectionLocker lock(bochs_thread_lock);
  if (bochs_thread && bochs_thread->IsPaused ()) {
	SetStatusText ("Resuming the Bochs simulation");
	bochs_thread->Resume ();
  }
}

void MyFrame::OnKillBochs(wxCommandEvent& WXUNUSED(event))
{
  // DON'T use a critical section here.  Delete implicitly calls
  // OnBochsThreadExit, which also tries to lock bochs_thread_lock.
  // If we grab the lock at this level, deadlock results.
  if (bochs_thread) {
	SetStatusText ("Killing the Bochs simulation");
	bochs_thread->Delete ();
  }
}

void
MyFrame::OnBochsThreadExit () {
  wxCriticalSectionLocker lock (bochs_thread_lock);
  bochs_thread = NULL; 
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
  switch (event->u.param.id) {
  case BXP_FLOPPYA_PATH:
  case BXP_FLOPPYB_PATH:
	return HandleVgaGuiButton (event->u.param.id);
  default:
	fprintf (wxlog, "HandleAskParam: parameter %d, not implemented\n", event->u.param.id);
  }
  return -1;  // could not display
}

void 
MyFrame::OnBochs2GuiEvent (wxCommandEvent& event)
{
  fprintf (wxlog, "received a bochs event in the GUI thread\n");
  BxEvent *be = (BxEvent *) event.GetEventObject ();
  fprintf (wxlog, "event type = %d\n", (int) be->type);
  // all cases should return.  sync event handlers MUST send back a 
  // response.
  switch (be->type) {
  case BX_SYNC_EVT_ASK_PARAM:
    be->retcode = HandleAskParam (be);
    // sync must return something; just return a copy of the event.
    bochs_thread->SendSyncResponse(be);
	return;
  case BX_ASYNC_EVT_SHUTDOWN_GUI:
	fprintf (wxlog, "control panel is exiting\n");
	fflush (wxlog);
    Close (TRUE);
	wxExit ();
	return;
  case BX_ASYNC_EVT_LOG_MSG:
    fprintf (wxlog, "log msg: level=%d, prefix='%s', msg='%s'\n",
	  be->u.logmsg.level,
	  be->u.logmsg.prefix,
	  be->u.logmsg.msg);
	fflush (wxlog);
    return;
  default:
    fprintf (wxlog, "OnBochs2GuiEvent: event type %d ignored\n", (int)be->type);
	// assume it's a synchronous event and send back a response, to avoid
	// potential deadlock.
    bochs_thread->SendSyncResponse(be);
	return;
  }
  // it is critical to send a response back eventually since the Bochs thread
  // is blocking.
  wxASSERT_MSG (0, "switch stmt should have returned");
}

/////////////// bochs thread

void *
BochsThread::Entry (void)
{     
  int argc=1;
  char *argv[] = {"bochs"};
  fprintf (wxlog, "in BochsThread, starting at bx_continue_after_control_panel\n");
  // run all the rest of the Bochs simulator code.  This function will
  // run forever, unless a "kill_bochs_request" is issued.  The procedure
  // is as follows:
  //   - user selects "Kill Bochs" or GUI decides to kill bochs
  //   - GUI calls bochs_thread->Delete ()
  //   - Bochs continues to run until the next time it reaches SIM->periodic().
  //   - SIM->periodic() sends a synchronous tick event to the GUI, which
  //     finally calls TestDestroy() and realizes it needs to stop.  It
  //     sets the sync event return code to -1.  SIM->periodic() sets the
  //     kill_bochs_request flag in cpu #0.
  //   - cpu loop notices kill_bochs_request and returns to main.cc:
  //     bx_continue_after_control_panel(), which notices the
  //     kill_bochs_request and returns back to this Entry() function.
  //   - Entry() exits and the thread stops. Whew.
  bx_continue_after_control_panel (argc, argv);
  fprintf (wxlog, "in BochsThread, bx_continue_after_control_panel exited\n");
  fflush (wxlog);
  return NULL;
}

void
BochsThread::OnExit ()
{
  // notify the MyFrame that the bochs thread has died.  I can't adjust
  // the bochs_thread directly because it's private.
  frame->OnBochsThreadExit ();
  // don't use this BochsThread's callback function anymore.
  SIM->set_notify_callback (NULL, NULL);
}

// return 1 if the user selected a value.
// return 0 if the question was displayed and the user cancelled.
// return -1 if we don't know how to display the question.
int
MyFrame::HandleVgaGuiButton (bx_id param)
{
  fprintf (wxlog, "HandleVgaGuiButton: button %d was pressed\n", (int)param);
  switch (param)
  {
	case BXP_FLOPPYA_PATH:
	case BXP_FLOPPYB_PATH:
	{
	  Raise();  // bring the control panel to front so dialog shows
	  wxFileDialog dialog(this, "Choose new floppy disk image file", "", "", "*.img", 0);
	  int ret = dialog.ShowModal();
	  if (ret == wxID_OK)
	  {
	    char *newpath = (char *)dialog.GetPath().c_str ();
	    if (newpath && strlen(newpath)>0) {
	      // change floppy path to this value.
	      bx_param_string_c *Opath = SIM->get_param_string (param);
	      assert (Opath != NULL);
	      fprintf (wxlog, "Setting floppy %c path to '%s'\n", 
		    param == BXP_FLOPPYA_PATH ? 'A' : 'B',
		    newpath);
	      Opath->set (newpath);
	      return 1;
	    }
	  }
	  return 0;
	}
	default:
	  fprintf (wxlog, "HandleVgaGuiButton: button %d not recognized\n", param);
	  return -1;
  }
}

// This function is declared static so that I can get a usable function
// pointer for it.  The function pointer is passed to SIM->set_notify_callback
// so that the siminterface can call this function when it needs to contact
// the gui.  It will always be called with a pointer to the BochsThread
// as the first argument.
BxEvent *
BochsThread::SiminterfaceCallback (void *thisptr, BxEvent *event)
{
  BochsThread *me = (BochsThread *)thisptr;
  // call the normal non-static method now that we know the this pointer.
  return me->SiminterfaceCallback2 (event);
}

// callback function for Bochs-generated events.  This is called from
// the Bochs thread, not the GUI thread.  So any GUI actions must be
// done after getting the gui mutex.
BxEvent *
BochsThread::SiminterfaceCallback2 (BxEvent *event)
{
  fprintf (wxlog, "SiminterfaceCallback with event type=%d\n", (int)event->type);
  event->retcode = 0;  // default return code
  int async = 1;
  switch (event->type)
  {
  case BX_SYNC_EVT_ASK_PARAM:
  case BX_SYNC_EVT_TICK:
    ClearSyncResponse ();  // must be before postevent.
    async = 0;
	break;
  }

  // tick event must be handled right here in the bochs thread.
  if (event->type == BX_SYNC_EVT_TICK) {
	if (TestDestroy ()) {
	  // tell simulator to quit
	  event->retcode = -1;
	}
	SendSyncResponse (event);
	return event;
  }

  //encapsulate the bxevent in a wxwindows event
  wxCommandEvent wxevent (wxEVT_COMMAND_MENU_SELECTED, ID_Bochs2Gui_Event);
  wxevent.SetEventObject ((wxEvent *)event);
  fprintf (wxlog, "Sending an event to the window\n");
  fflush (wxlog);
  wxPostEvent (frame, wxevent);
  // if it is an asynchronous event, return immediately
  if (async) return NULL;
  fprintf (wxlog, "SiminterfaceCallback2: synchronous event; waiting for response\n");
  // now wait forever for the GUI to post a response.
  BxEvent *response = NULL;
  while (response == NULL) {
	response = GetSyncResponse ();
	if (!response) {
	  fprintf (wxlog, "no sync response yet, waiting\n");
	  fflush (wxlog);
	  this->Sleep(500);
	}
  }
  wxASSERT (response != NULL);
  fflush (wxlog);
  return response;
}

void 
BochsThread::ClearSyncResponse ()
{
  wxCriticalSectionLocker lock(bochs2gui_mailbox_lock);
  if (bochs2gui_mailbox != NULL) {
    fprintf (wxlog, "WARNING: ClearSyncResponse is throwing away an event that was previously in the mailbox\n");
  }
  bochs2gui_mailbox = NULL;
}

void 
BochsThread::SendSyncResponse (BxEvent *event)
{
  wxCriticalSectionLocker lock(bochs2gui_mailbox_lock);
  if (bochs2gui_mailbox != NULL) {
    fprintf (wxlog, "WARNING: SendSyncResponse is throwing away an event that was previously in the mailbox\n");
  }
  bochs2gui_mailbox = event;
}

BxEvent *
BochsThread::GetSyncResponse ()
{
  wxCriticalSectionLocker lock(bochs2gui_mailbox_lock);
  BxEvent *event = bochs2gui_mailbox;
  bochs2gui_mailbox = NULL;
  return event;
}
