//
// wxthreadtest.cc
// Bryce Denney
//
// Compile with:
//   c++ -g `wx-config --cxxflags --libs` wxthreadtest.cc -o wxthreadtest
//
// I have been using this file to learn how to use wxwindows threads safely
// and test out new ideas in a controlled environment.  The most important
// realization I made while writing this code is that when a detachable thread
// is dying, you should use its OnExit method to make the thread remove itself
// from the list of valid threads.  And manage the list of valid threads with a
// critical section.  Now it doesn't crash anymore!
//
// There is one example where the non-gui threads that I start pass a
// user-defined event into the GUI event queue.  This represents the
// Bochs thread sending an asynchronous message to the GUI.  To support
// synchronous messages from Bochs to the gui, I would need to send an async
// message and then wait until a response arrives.
//
// The event queue of the Bochs thread is something we must create.  It will
// look like a linked list of bochs events.  The gui thread appends things to
// the end of the queue and Bochs takes things from the front of the queue.  In
// the case of a synchronous message from Bochs to the gui, Bochs must wait
// around for the response.  It cannot respond to async messages while waiting;
// it must simply block until the synchronous response arrives.  To support
// this, I will set aside a "mailbox" for synchronous responses.  When Bochs
// sends the synch request, it is sure to clear the synchronous response
// mailbox.  When the GUI is ready to respond, it places a pointer to the
// response event into the mailbox.  The Bochs thread finds the response and
// continues.  Async events can continue to build up in the meantime, and they
// will be kept in order relative to each other.
//
// I'm not sure if this will be needed, but it may be useful to have a
// high priority queue for things like starting and stopping simulation.
// If there are 200 mouse events in the async queue and you press a stop
// button, this would allow bochs to stop and either clear the async queue
// or save them for later.
//
// I will use wxList for the event queue.
//
// Well, it's a little tricky.  I want Bochs to continue to compile without
// wxwindows, and I hope to keep the text mode interface supported.  Will it
// compromise portability to use a wxwindows container class for the wxwindows
// event queue?  All the wxwindows code will be in siminterface.cc inside an
// #if BX_USE_WX.  For text mode interface, we must provide equivalent event
// passing, but that interface is a whole lot simpler because it doesn't
// produce async events.  I'm sure I can get it to work.  The fact that all
// events are passing through a few specific functions makes the porting much
// more practical.  That was the point after all.
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class MyApp: public wxApp
{
virtual bool OnInit();
};

class BochsThread: public wxThread
{
public:
  virtual ExitCode Entry ();
  void call_exit () { Exit (); }
};

class thread2 : public wxThread
{
  int id;
  int my_alive;
public:
  virtual ExitCode Entry ();
  void OnExit ();
};


class MyPanel: public wxPanel
{
public:
  MyPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel")
      : wxPanel (parent, id, pos, size, style, name)
    { wxLogDebug ("MyPanel constructor"); }
  void OnKeyDown(wxKeyEvent& event)
  { wxLogDebug ("OnKeyDown"); }
  void OnKeyUp(wxKeyEvent& event)
  { wxLogDebug ("OnKeyUp"); }
private:
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
  EVT_KEY_DOWN(MyPanel::OnKeyDown)
  EVT_KEY_UP(MyPanel::OnKeyUp)
END_EVENT_TABLE()

class MyFrame: public wxFrame
{
public:
MyFrame(const wxString& title, const wxPoint& pos, const wxSize&
size);
void OnQuit(wxCommandEvent& event);
void OnAbout(wxCommandEvent& event);
//void OnStartBochs(wxCommandEvent& event);
void OnTestTask(wxCommandEvent& event);
void OnPause(wxCommandEvent& event);
void OnResume(wxCommandEvent& event);
void OnDelete(wxCommandEvent& event);
void OnDeleteAll(wxCommandEvent& event);
void OnUserEvent(wxCommandEvent& event);
//int StartBochsThread ();
//int HandleVgaGuiButton (int which);
private:
BochsThread *bochs_thread;
MyPanel *panel;
wxStaticText *text1, *text2;

DECLARE_EVENT_TABLE()
static int SiminterfaceCallback (void *thisptr, int code);
int SiminterfaceCallback2 (int code);

int event_count;

public:
wxCriticalSection thread_list_lock;
// test_thread and next_test_thread are protected by the thread_list_lock
// critical section.
#define MAX_TEST_THREADS 128
thread2 *test_thread[MAX_TEST_THREADS];
int next_test_thread;

};

enum
{
ID_Quit = 1,
ID_About,
ID_TestTask,
ID_Pause,
ID_Resume,
ID_Delete,
ID_DeleteAll,
ID_UserEvent,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit, MyFrame::OnQuit)
  EVT_MENU(ID_About, MyFrame::OnAbout)
  EVT_MENU(ID_TestTask, MyFrame::OnTestTask)
  EVT_MENU(ID_Pause, MyFrame::OnPause)
  EVT_MENU(ID_Resume, MyFrame::OnResume)
  EVT_MENU(ID_Delete, MyFrame::OnDelete)
  EVT_MENU(ID_DeleteAll, MyFrame::OnDeleteAll)
  // I just made this up.  I'll try to get the other threads to send
  // user events to the MyFrame and hopefully they will call up
  // the OnUserEvent() method.  We'll see.
  EVT_MENU(ID_UserEvent, MyFrame::OnUserEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

static MyFrame *theFrame = NULL;

bool MyApp::OnInit()
{
    //siminterface_init ();
theFrame = new MyFrame( "Bochs Control Panel", wxPoint(50,50),
wxSize(450,340) );
theFrame->Show( TRUE );
SetTopWindow( theFrame );
return TRUE;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const
wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  // init variables
  bochs_thread = NULL;
  next_test_thread = 0;
  // don't bother with crit section
  for (int i=0; i<MAX_TEST_THREADS; i++) 
	test_thread[i] = NULL;
  event_count = 0;

  // set up the gui
  wxMenu *menuFile = new wxMenu;
  menuFile->Append( ID_TestTask, "&Start a thread" );
  menuFile->Append( ID_Pause, "&Pause thread" );
  menuFile->Append( ID_Resume, "&Resume thread" );
  menuFile->Append( ID_Delete, "&Delete thread" );
  menuFile->Append( ID_DeleteAll, "&Delete all threads" );
  menuFile->Append( ID_About, "&About..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, "E&xit" );
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile, "&File" );
  SetMenuBar( menuBar );
  CreateStatusBar();
  SetStatusText( "Welcome to wxWindows!" );
  // try to create some static text labels
  panel = new MyPanel (this, -1);
  text1 = new wxStaticText (panel, -1, "Text#1");
  text2 = new wxStaticText (panel, -1, "Text#2");
  text2->SetBackgroundColour (wxColour (255, 0, 0));
  text2->Clear ();
  wxGridSizer *sz = new wxGridSizer (1, 2);
  sz->Add (text1, 0, wxALIGN_CENTER);
  sz->Add (text2, 0, wxALIGN_CENTER );
  // stick those labels onto a panel
  panel->SetSizer (sz);
  panel->SetAutoLayout (TRUE);  // required or sizer does nothing
  panel->SetBackgroundColour (wxColour (255, 0, 0));
  // now the panel needs to be in the frame or you won't see much at all
  wxGridSizer *frame_sz = new wxGridSizer (1, 1);
  frame_sz->Add (panel, 0, wxGROW | wxALIGN_CENTER);
  SetSizer (frame_sz);
  SetAutoLayout (TRUE);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close( TRUE );
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
wxMessageBox( "wxWindows Control Panel for Bochs. (Very Experimental)",
"Bochs Control Panel", wxOK | wxICON_INFORMATION );
}

#if 0
void MyFrame::OnStartBochs(wxCommandEvent& WXUNUSED(event))
{
  //StartBochsThread ();
}
#endif

/////////////// bochs thread

#if 0
void *
BochsThread::Entry (void)
{     
  int argc=1;
  char *argv[] = {"bochs"};
  printf ("in BochsThread, starting at bx_continue_after_control_panel\n");
  bx_continue_after_control_panel (argc, argv);
  printf ("in BochsThread, bx_continue_after_control_panel exited\n");
  return NULL;
}   

int MyFrame::StartBochsThread ()
{
    if (bochs_thread != NULL) {
    wxMessageBox (
    "Can't start Bochs simulator, because it is already running",
    "Already Running", wxOK | wxICON_ERROR);
    return -1;
    }
    printf ("Starting bochs thread now\n");
    bochs_thread = new BochsThread ();
    bochs_thread->Create ();
    bochs_thread->Run ();                                                        
    printf ("Bochs thread has started.\n");
    return 0;
}

int
MyFrame::HandleVgaGuiButton (int which)
{
  printf ("HandleVgaGuiButton: button %d was pressed\n", which);
  switch (which)
  {
    case GUI_BUTTON_FLOPPYA:
    {
      wxMutexGuiEnter ();
      Raise();  // bring the control panel to front so dialog shows
      wxFileDialog dialog(this, "Choose new floppy disk image file", "", "", "*.img", 0);
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
      Raise();  // bring the control panel to front so dialog shows
      wxFileDialog dialog(this, "Choose new floppy disk image file", "", "", "*.img", 0);
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

int
MyFrame::SiminterfaceCallback (void *thisptr, int code)
{
  assert (thisptr == theFrame);
  int ret = theFrame->SiminterfaceCallback2 (code);
  SIM->notify_return (ret);
  return 0;
}

int
MyFrame::SiminterfaceCallback2 (int code)
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
      wxMutexGuiEnter ();
      Destroy ();
	  if (bochs_thread) {
		// the callbacks are call from the simulator thread, so it is
		// safe to call wxThread::Exit from right here.
		bochs_thread->call_exit ();
	  }
      wxMutexGuiLeave ();
    }
  }
  return 0;
}
#endif

/////////////////////////////

static int next_id = 0;
void *
thread2::Entry ()
{
  int i;
  my_alive = 1;
  id = next_id++;
  wxASSERT_MSG (id < MAX_TEST_THREADS, "id < MAX_TEST_THREADS");
  printf ("thread%d: starting test task\n", id);
  for (i=0; i<3; i++) {
	if (TestDestroy ()) {
	  // if true, exit as soon as possible
	  printf ("thread%d: I was deleted\n", id);
	  break;
	}
	printf ("thread%d: count is %d\n", id, i);
	// make a UserEvent and send it to the gui thread.
	wxCommandEvent event (wxEVT_COMMAND_MENU_SELECTED, ID_UserEvent);
	printf ("Posting event to MyFrame\n");
	wxPostEvent (theFrame, event);
	this->Sleep (1000);  // 1 second
  }
  my_alive = 0;
  printf ("thread%d: goodbye\n", id);
  return NULL;
}

void
thread2::OnExit ()
{
  printf ("thread%d: OnExit\n", id);
  // find myself in the thread list and change the pointer to a NULL
  wxCriticalSectionLocker lock (theFrame->thread_list_lock);
  for (int i=0; i<MAX_TEST_THREADS; i++)
  {
    if (theFrame->test_thread[i] == this)
      theFrame->test_thread[i] = NULL;
  }
}

void MyFrame::OnTestTask (wxCommandEvent& WXUNUSED(event))
{
  printf ("OnTestTask\n");
  thread2 *thread = new thread2 ();
  thread->Create ();
  thread->Run ();
  {
    // add the thread to the list
    wxCriticalSectionLocker lock (thread_list_lock);
    int n = next_test_thread;
    next_test_thread = (next_test_thread+1) % MAX_TEST_THREADS;
    test_thread[n] = thread;
  }
}

void MyFrame::OnResume (wxCommandEvent& WXUNUSED(event))
{
  printf ("OnResume()\n");
  // find one that is paused and resume it
  wxCriticalSectionLocker lock (thread_list_lock);
  for (int i=MAX_TEST_THREADS-1; i>=0; i--) {
	if (!test_thread[i]) continue;
	if (test_thread[i]->IsAlive () && test_thread[i]->IsPaused ()) {
	  thread2 *thread;
	  printf ("resuming thread %d\n", i);
	  test_thread[i]->Resume ();
	  return;
	}
  }
  printf ("no threads were paused\n");
}

void MyFrame::OnPause (wxCommandEvent& WXUNUSED(event))
{
  printf ("OnPause()\n");
  // find a running thread thread and pause
  wxCriticalSectionLocker lock (thread_list_lock);
  for (int i=MAX_TEST_THREADS-1; i>=0; i--) {
	if (!test_thread[i]) continue;
	if (test_thread[i]->IsAlive () && !test_thread[i]->IsPaused ()) {
	  printf ("pausing thread %d\n", i);
	  test_thread[i]->Pause ();
	  return;
	}
  }
  printf ("no threads to pause\n");
}

void MyFrame::OnDelete (wxCommandEvent& WXUNUSED(event))
{
  printf ("OnDelete()\n");
  // find a live thread and delete it
  thread_list_lock.Enter ();
  for (int i=MAX_TEST_THREADS-1; i>=0; i--) {
    if (!test_thread[i]) continue;
    if (test_thread[i]->IsAlive ()) {
      printf ("deleting thread %d\n", i);
      wxThread *thread = test_thread[i];
      thread_list_lock.Leave ();
      // is seems possible for a thread to finish, remove itself
      // from the list, etc. between exiting the critical section
      // and the delete.  Then thread would point to a nonexistent
      // object.  OTOH, this method is copied right out of the sample
      // thread code so it's probably reasonable safe.
      thread->Delete ();
      return;
    }
  }
  thread_list_lock.Leave ();
  printf ("no threads to delete\n");
}

void MyFrame::OnDeleteAll (wxCommandEvent& WXUNUSED(event))
{
  printf ("OnDeleteAll()\n");
#if 0
  // delete all test threads
  for (int i=MAX_TEST_THREADS-1; i>=0; i--) {
	if (!test_thread[i]) continue;
	if (test_thread[i]->IsAlive ()) {
	  printf ("deleting thread %d\n", i);
	  test_thread[i]->Delete ();
	}
  }
#endif
  printf ("no threads to pause\n");
}

void MyFrame::OnUserEvent (wxCommandEvent& WXUNUSED(event))
{
  wxASSERT_MSG (wxThread::IsMain (), "UserEvent handler called by non-gui thread");
  printf ("user event\n");
  event_count++;
  wxString newtext;
  newtext.Printf ("Event count is now %d", event_count);
  text1->SetLabel (newtext);
}
