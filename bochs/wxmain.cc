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
  BxEvent *bochs2gui_sync_response;
public:
  BochsThread (MyFrame *_frame) { frame = _frame; }
  virtual ExitCode Entry ();
  void OnExit ();
  // called by the siminterface code, with the pointer to the Bochs thread
  // in the thisptr arg.
  static int SiminterfaceCallback (void *thisptr, int code);
  int SiminterfaceCallback2 (int code);
private:
  int HandleVgaGuiButton (int which);
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
int StartBochsThread ();

// called from the Bochs thread's OnExit() method.
void OnBochsThreadExit ();

private:
wxCriticalSection bochs_thread_lock;
BochsThread *bochs_thread; // get the lock before accessing bochs_thread

DECLARE_EVENT_TABLE()
};

enum
{
ID_Quit = 1,
ID_About,
ID_StartBochs,
ID_PauseBochs,
ID_ResumeBochs,
ID_KillBochs
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Quit, MyFrame::OnQuit)
EVT_MENU(ID_About, MyFrame::OnAbout)
EVT_MENU(ID_StartBochs, MyFrame::OnStartBochs)
EVT_MENU(ID_PauseBochs, MyFrame::OnPauseBochs)
EVT_MENU(ID_ResumeBochs, MyFrame::OnResumeBochs)
EVT_MENU(ID_KillBochs, MyFrame::OnKillBochs)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

//static MyFrame *theFrame = NULL;

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
  printf ("Starting a Bochs simulation\n");
  SetStatusText ("Starting a Bochs simulation");
  bochs_thread = new BochsThread (this);
  bochs_thread->Create ();
  bochs_thread->Run ();                                                        
  printf ("Bochs thread has started.\n");
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

/////////////// bochs thread

void *
BochsThread::Entry (void)
{     
  int argc=1;
  char *argv[] = {"bochs"};
  printf ("in BochsThread, starting at bx_continue_after_control_panel\n");
  // run all the rest of the Bochs simulator code.  This function will
  // never exit unless we invent some kind of signal that the GUI sends
  // that causes bochs to drop out of its cpu loop.  Presently there is
  // no such signal so it will never quit.
  bx_continue_after_control_panel (argc, argv);
  printf ("in BochsThread, bx_continue_after_control_panel exited\n");
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

int
BochsThread::HandleVgaGuiButton (int which)
{
  printf ("HandleVgaGuiButton: button %d was pressed\n", which);
  switch (which)
  {
	case GUI_BUTTON_FLOPPYA:
	{
	  wxMutexGuiEnter ();
	  frame->Raise();  // bring the control panel to front so dialog shows
	  wxFileDialog dialog(frame, "Choose new floppy disk image file", "", "", "*.img", 0);
	  int ret = dialog.ShowModal();
	  wxMutexGuiLeave ();
	  if (ret == wxID_OK)
	  {
	    char *newpath = (char *)dialog.GetPath().c_str ();
	    if (newpath && strlen(newpath)>0) {
	      // change floppy A path to this value.
	      bx_param_string_c *Opath = SIM->get_param_string (BXP_FLOPPYA_PATH);
	      assert (Opath != NULL);
	      printf ("Setting floppy A path to '%s'\n", newpath);
	      Opath->set (newpath);
	      return 1;
	    }
	  }
	  return 0;
	}
	case GUI_BUTTON_FLOPPYB:
	{
	  wxMutexGuiEnter ();
	  frame->Raise();  // bring the control panel to front so dialog shows
	  wxFileDialog dialog(frame, "Choose new floppy disk image file", "", "", "*.img", 0);
	  int ret = dialog.ShowModal ();
	  wxMutexGuiLeave ();
	  if (ret == wxID_OK)
	  {
	    char *newpath = (char *)dialog.GetPath().c_str ();
	    if (newpath && strlen(newpath)>0) {
	      // change floppy A path to this value.
	      bx_param_string_c *Opath = SIM->get_param_string (BXP_FLOPPYB_PATH);
	      assert (Opath != NULL);
	      printf ("Setting floppy B path to '%s'\n", newpath);
	      Opath->set (newpath);
	      return 1;
	    }
	  }
	  return 0;
	}
  }
return 0;
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
  printf ("SiminterfaceCallback with code=%d\n", code);
  switch (code)
  {
  case NOTIFY_CODE_LOGMSG:
    printf ("logmsg callback");
    break;
  case NOTIFY_CODE_VGA_GUI_BUTTON:
    {
      printf ("vga button pressed");
	  int which = SIM->notify_get_int_arg (0);
	  return HandleVgaGuiButton (which);
	}
	break;
  case NOTIFY_CODE_SHUTDOWN:
    {
	  // assume bochs has already had its change to shut down.
	  // this will just shut down the gui.
	  printf ("control panel is exiting\n");
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
