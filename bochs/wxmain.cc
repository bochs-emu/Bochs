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
  static int SiminterfaceCallback (void *thisptr, int code);
  int SiminterfaceCallback2 (int code);
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
void OnBochs2GuiSyncEvent(wxCommandEvent& event);
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
ID_Bochs2Gui_Sync_Event,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Quit, MyFrame::OnQuit)
EVT_MENU(ID_About, MyFrame::OnAbout)
EVT_MENU(ID_StartBochs, MyFrame::OnStartBochs)
EVT_MENU(ID_PauseBochs, MyFrame::OnPauseBochs)
EVT_MENU(ID_ResumeBochs, MyFrame::OnResumeBochs)
EVT_MENU(ID_KillBochs, MyFrame::OnKillBochs)
EVT_MENU(ID_Bochs2Gui_Sync_Event, MyFrame::OnBochs2GuiSyncEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

#if 1
// stupid hack to make response available to siminterface
BxEvent *bochs2gui_last_response;
BxEvent *get_bochs2gui_last_response () {
  return bochs2gui_last_response;
}
#endif

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

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close( TRUE );
  if (SIM)
	SIM->quit_sim(0);  // give bochs a chance to shut down
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
  // use crit section to ensure safe use of bochs_thread pointer.
  wxCriticalSectionLocker lock(bochs_thread_lock);
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
MyFrame::OnBochs2GuiSyncEvent (wxCommandEvent& event)
{
  fprintf (wxlog, "received a bochs event in the GUI thread\n");
  BxEvent *be = (BxEvent *) event.GetEventObject ();
  fprintf (wxlog, "event type = %d\n", (int) be->type);
  switch (be->type) {
  case BX_SYNC_EVT_ASK_PARAM:
    be->u.param.success = HandleAskParam (be);
    break;
  default:
    fprintf (wxlog, "OnBochs2GuiSyncEvent: event type %d ignored\n", (int)be->type);
  }
  // it is critical to send a response back eventually since the Bochs thread
  // is blocking.  For now, just send it right back.
  bochs_thread->SendSyncResponse(be);
}

/////////////// bochs thread

void *
BochsThread::Entry (void)
{     
  int argc=1;
  char *argv[] = {"bochs"};
  fprintf (wxlog, "in BochsThread, starting at bx_continue_after_control_panel\n");
  // run all the rest of the Bochs simulator code.  This function will
  // never exit unless we invent some kind of signal that the GUI sends
  // that causes bochs to drop out of its cpu loop.  Presently there is
  // no such signal so it will never quit.
  bx_continue_after_control_panel (argc, argv);
  fprintf (wxlog, "in BochsThread, bx_continue_after_control_panel exited\n");
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
int
BochsThread::SiminterfaceCallback (void *thisptr, int code)
{
  BochsThread *me = (BochsThread *)thisptr;
  // call the normal method (non-static) now that we know the this pointer.
  int ret = me->SiminterfaceCallback2 (code);
  // FIXME: kind of stupid to use notify_return instead of normal return.
  // Not sure why I ever did it.
  SIM->notify_return (ret);
  return 0;
}

// callback function for Bochs-generated events.  This is called from
// the Bochs thread, not the GUI thread.  So any GUI actions must be
// done after getting the gui mutex.
int
BochsThread::SiminterfaceCallback2 (int code)
{
  fprintf (wxlog, "SiminterfaceCallback with code=%d\n", code);
  switch (code)
  {
  case NOTIFY_CODE_LOGMSG:
    fprintf (wxlog, "logmsg callback");
    break;
  case NOTIFY_CODE_VGA_GUI_BUTTON:
    {
      fprintf (wxlog, "vga button pressed\n");
	  int which = SIM->notify_get_int_arg (0);
	  // this was the BochsThread implementation.
	  //return HandleVgaGuiButton (which);
	  // build an event that describes what is going on, and post it to
	  // the GUI event queue.  The GUI event handler will process the event
	  // and then call BochsThread::SyncEventRespond (event).  This will 
	  // supply the response so that the Bochs thread can return.
	  BxEvent *be = new BxEvent ();
	  be->type = BX_SYNC_EVT_ASK_PARAM;
	  be->u.param.id = which?  BXP_FLOPPYA_PATH : BXP_FLOPPYB_PATH;
	  wxCommandEvent event (wxEVT_COMMAND_MENU_SELECTED, ID_Bochs2Gui_Sync_Event);
	  event.SetEventObject ((wxObject *)be);
	  ClearSyncResponse ();
	  fprintf (wxlog, "Sending a Bochs ask_param event to the window\n");
	  wxPostEvent (frame, event);
	  fflush (wxlog);
	  // now wait forever for the GUI to post a response.
	  BxEvent *response = NULL;
	  while (response == NULL) {
	    response = GetSyncResponse ();
		if (!response) {
		  fprintf (wxlog, "no sync response yet, waiting\n");
		  this->Sleep(500);
        }
	  }
	  wxASSERT (response != NULL);
	  // FIXME: stupid hack to make response available to siminterface.  Should
	  // probably be the return value or something.
	  bochs2gui_last_response = response;
	  fflush (wxlog);
	  return 0;
	}
	break;
  case NOTIFY_CODE_SHUTDOWN:
    {
	  // assume bochs has already had its change to shut down.
	  // this will just shut down the gui.
	  fprintf (wxlog, "control panel is exiting\n");
	  frame->Close (TRUE);
	  wxExit ();
	}
  case -1: // what code?
    {
	  // synchronous event from Bochs to control panel.  Post the event to
	  // the GUI event queue, then wait for a response in the mailbox.
	}
  }
  return 0;
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
