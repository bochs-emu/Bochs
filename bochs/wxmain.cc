//
// wxmain.cc
// $Id: wxmain.cc,v 1.1.2.12 2002-03-24 15:39:12 bdenney Exp $
//
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
#include "wx/image.h"
//#include <wx/xpmhand.h>

#include "config.h"
extern "C" {
// siminterface needs stdio.h
#include <stdio.h>
}

#include "bochs.h"
#include "gui/icon_bochs.h"
#include "osdep.h"
#include "font/vga.bitmap.h"
#define LOG_THIS bx_gui.

#include "gui/wx_toolbar.h"

enum {
	IDM_TOOLBAR_POWER = 200
};
//#include "gui/siminterface.h"

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
  SimThread (MyFrame *_frame) { frame = _frame; sim2gui_mailbox = NULL; }
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
MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
void OnPaint(wxPaintEvent& event);
void OnQuit(wxCommandEvent& event);
void OnAbout(wxCommandEvent& event);
void OnStartSim(wxCommandEvent& event);
void OnPauseResumeSim(wxCommandEvent& event);
void OnKillSim(wxCommandEvent& event);
void OnSim2GuiEvent(wxCommandEvent& event);
void OnToolbarClick(wxCommandEvent& event);
int HandleAskParam (BxEvent *event);
int HandleAskParamString (bx_param_string_c *param);

// called from the sim thread's OnExit() method.
void OnSimThreadExit ();

private:
wxCriticalSection sim_thread_lock;
SimThread *sim_thread; // get the lock before accessing sim_thread
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
  EVT_PAINT(MyFrame::OnPaint)
  EVT_MENU(IDM_TOOLBAR_POWER, MyFrame::OnToolbarClick)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

wxCriticalSection wxScreen_lock;
static char *wxScreen = NULL;
static long wxScreenX = 0;
static long wxScreenY = 0;
static long wxTileX = 0;
static long wxTileY = 0;
static unsigned long wxCursorX = 0;
static unsigned long wxCursorY = 0;
//hack alert
static MyFrame *theFrame = NULL;

void UpdateScreen(char *newBits, int x, int y, int width, int heigh);
void DrawBochsBitmap(int x, int y, int width, int height, char *bmap, char color);

struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} wxBochsPalette[256];

bool MyApp::OnInit()
{
  wxLog::AddTraceMask (_T("mime"));
  siminterface_init ();
  MyFrame *frame = new MyFrame( "Bochs Control Panel", wxPoint(50,50), wxSize(450,340) );
  theFrame = frame;  // hack alert
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

  CreateToolBar(wxNO_BORDER|wxHORIZONTAL|wxTB_FLAT);
  wxToolBar *tb = GetToolBar();
  tb->SetToolBitmapSize(wxSize(16, 16));

  wxBitmap toolBarBitmaps[4];
  toolBarBitmaps[0] = wxBitmap(xpm_power);
  toolBarBitmaps[1] = wxBitmap(xpm_reset);
  toolBarBitmaps[2] = wxBitmap(xpm_floppya);
  toolBarBitmaps[3] = wxBitmap(xpm_floppyb);

  int currentX = 5;
  tb->AddTool(16780, toolBarBitmaps[0], wxNullBitmap, FALSE, currentX, -1, (wxObject *)NULL, "Turn power on/off");
  currentX += 34;
  tb->AddTool(16781, toolBarBitmaps[1], wxNullBitmap, FALSE, currentX, -1, (wxObject *)NULL, "Reset the system");
  currentX += 34;
  tb->AddTool(16782, toolBarBitmaps[2], wxNullBitmap, FALSE, currentX, -1, (wxObject *)NULL, "Enable/Disable Floppy A");
  currentX += 34;
  tb->AddTool(16783, toolBarBitmaps[3], wxNullBitmap, FALSE, currentX, -1, (wxObject *)NULL, "Enable/Disable Floppy B");
  currentX += 34;

  tb->Realize();
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
  wxMessageBox( "wxWindows Control Panel for Bochs. (Very Experimental)", "Bochs Control Panel", wxOK | wxICON_INFORMATION );
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
  wxLogStatus ("Starting a Bochs simulation");
  start_bochs_times++;
  if (start_bochs_times>1) {
	wxMessageBox (
	"You have already started the simulator once this session. Due to memory leaks, you may get unstable behavior.",
	"2nd time warning", wxOK | wxICON_WARNING);
  }
  sim_thread = new SimThread (this);
  sim_thread->Create ();
  sim_thread->Run ();                                                        
  wxLogDebug ("Simulator thread has started.");
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
  wxLogDebug ("HandleAskParamString start");
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
        wxLogDebug ("HandleAskParamString: create dialog");
	wxFileDialog *fdialog = new wxFileDialog (this, msg, "", "", "*.*", style);
        wxLogDebug ("HandleAskParamString: before showmodal");
	if (fdialog->ShowModal() == wxID_OK)
	  newval = (char *)fdialog->GetPath().c_str ();
        wxLogDebug ("HandleAskParamString: after showmodal");
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
	wxLogDebug ("Setting param %s to '%s'", param->get_name (), newval);
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
	      wxLogDebug ("Setting floppy %c path to '%s'", 
		    param == BXP_FLOPPYA_PATH ? 'A' : 'B',
		    newpath);
	      Opath->set (newpath);
	      return 1;
	    }
	  }
	  return 0;
	}
  default:
	wxLogError ("HandleAskParam: parameter %d, not implemented", event->u.param.id);
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
  wxLogDebug ("received a bochs event in the GUI thread");
  BxEvent *be = (BxEvent *) event.GetEventObject ();
  wxLogDebug ("event type = %d", (int) be->type);
  // all cases should return.  sync event handlers MUST send back a 
  // response.
  switch (be->type) {
  case BX_SYNC_EVT_ASK_PARAM:
    wxLogDebug ("before HandleAskParam");
    be->retcode = HandleAskParam (be);
    wxLogDebug ("after HandleAskParam");
    // sync must return something; just return a copy of the event.
    sim_thread->SendSyncResponse(be);
    wxLogDebug ("after SendSyncResponse");
	return;
  case BX_ASYNC_EVT_SHUTDOWN_GUI:
	wxLogDebug ("control panel is exiting");
    Close (TRUE);
	wxExit ();
	return;
  case BX_ASYNC_EVT_LOG_MSG:
    wxLogDebug ("log msg: level=%d, prefix='%s', msg='%s'",
	  be->u.logmsg.level,
	  be->u.logmsg.prefix,
	  be->u.logmsg.msg);
    return;
  default:
    wxLogDebug ("OnSim2GuiEvent: event type %d ignored", (int)be->type);
	// assume it's a synchronous event and send back a response, to avoid
	// potential deadlock.
    sim_thread->SendSyncResponse(be);
	return;
  }
  // it is critical to send a response back eventually since the sim thread
  // is blocking.
  wxASSERT_MSG (0, "switch stmt should have returned");
}

void MyFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);
	//PrepareDC(dc);

	wxCriticalSectionLocker lock(wxScreen_lock);
	wxLogDebug ("OnPaint called with wxScreen = %p", wxScreen);
	if(wxScreen != NULL) {
	  wxPoint pt = GetClientAreaOrigin();
	  wxImage screenImage(wxScreenX, wxScreenY, (unsigned char *)wxScreen, TRUE);
	  dc.DrawBitmap(screenImage.ConvertToBitmap(), pt.x, pt.y, FALSE);
	}
	
}

void MyFrame::OnToolbarClick(wxCommandEvent& event)
{

}

/////////////// bochs thread

void *
SimThread::Entry (void)
{     
  int argc=1;
  char *argv[] = {"bochs"};
  wxLogDebug ("in SimThread, starting at bx_continue_after_control_panel");
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
  wxLogDebug ("in SimThread, bx_continue_after_control_panel exited");
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
  //wxLogDebug ("SiminterfaceCallback with event type=%d", (int)event->type);
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
  wxLogDebug ("Sending an event to the window");
  wxPostEvent (frame, wxevent);
  // if it is an asynchronous event, return immediately
  if (async) return NULL;
  wxLogDebug ("SiminterfaceCallback2: synchronous event; waiting for response");
  // now wait forever for the GUI to post a response.
  BxEvent *response = NULL;
  while (response == NULL) {
	response = GetSyncResponse ();
	if (!response) {
	  wxLogDebug ("no sync response yet, waiting");
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
    wxLogDebug ("WARNING: ClearSyncResponse is throwing away an event that was previously in the mailbox");
  }
  sim2gui_mailbox = NULL;
}

void 
SimThread::SendSyncResponse (BxEvent *event)
{
  wxCriticalSectionLocker lock(sim2gui_mailbox_lock);
  if (sim2gui_mailbox != NULL) {
    wxLogDebug ("WARNING: SendSyncResponse is throwing away an event that was previously in the mailbox");
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


  void
bx_gui_c::specific_init(bx_gui_c *th, int argc, char **argv, unsigned tilewidth, unsigned tileheight,
                     unsigned headerbar_y)
{
  th->put("NGUI");
  if (bx_options.Oprivate_colormap->get ()) {
    BX_INFO(("private_colormap option ignored."));
  }

  for(int i = 0; i < 256; i++) {
	  wxBochsPalette[i].red = 0;
	  wxBochsPalette[i].green = 0;
	  wxBochsPalette[i].blue = 0;
  }

  wxScreenX = 640;
  wxScreenY = 480;
  wxCriticalSectionLocker lock(wxScreen_lock);
  wxScreen = (char *)malloc(wxScreenX * wxScreenY * 3);
  memset(wxScreen, 0, wxScreenX * wxScreenY * 3);

  wxTileX = tilewidth;
  wxTileY = tileheight;
}


// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

  void
bx_gui_c::handle_events(void)
{
}


// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

  void
bx_gui_c::flush(void)
{
}


// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.  Don't
// clear the area that defines the headerbar.

  void
bx_gui_c::clear_screen(void)
{
  wxCriticalSectionLocker lock(wxScreen_lock);
  memset(wxScreen, 0, wxScreenX * wxScreenY * 3);
}



// ::TEXT_UPDATE()
//
// Called in a VGA text mode, to update the screen with
// new content.
//
// old_text: array of character/attributes making up the contents
//           of the screen from the last call.  See below
// new_text: array of character/attributes making up the current
//           contents, which should now be displayed.  See below
//
// format of old_text & new_text: each is 4000 bytes long.
//     This represents 80 characters wide by 25 high, with
//     each character being 2 bytes.  The first by is the
//     character value, the second is the attribute byte.
//     I currently don't handle the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor

void bx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                      unsigned long cursor_x, unsigned long cursor_y,
		      Bit16u cursor_state, unsigned nrows)
{
  wxLogDebug ("text_update");
	unsigned char cChar;
	unsigned int nchars = 80 * nrows;
	if((wxCursorY * 80 + wxCursorX) < nchars) {
		cChar = new_text[(wxCursorY * 80 + wxCursorX) * 2];
		DrawBochsBitmap(wxCursorX * 8, wxCursorY * 16, 8, 16, (char *)&bx_vgafont[cChar].data, new_text[((wxCursorY * 80 + wxCursorX) * 2) + 1]);
	}
	
	for(int i = 0; i < nchars * 2; i += 2) {
		if((old_text[i] != new_text[i]) || (old_text[i+1] != new_text[i+1])) {
			cChar = new_text[i];
			int x = (i / 2) % 80;
			int y = (i / 2) / 80;
			DrawBochsBitmap(x * 8, y * 16, 8, 16, (char *)&bx_vgafont[cChar].data, new_text[i+1]);
		}
	}
	wxCursorX = cursor_x;
	wxCursorY = cursor_y;

	if((cursor_y * 80 + cursor_x) < nchars) {
		cChar = new_text[(cursor_y * 80 + cursor_x) * 2];
		char cAttr = new_text[((cursor_y * 80 + cursor_x) * 2) + 1];
		cAttr = ((cAttr >> 4) & 0xF) + ((cAttr & 0xF) << 4);
		DrawBochsBitmap(wxCursorX * 8, wxCursorY * 16, 8, 16, (char *)&bx_vgafont[cChar].data, cAttr);
	}

	wxPaintEvent event;
	wxPostEvent (theFrame, event);
}


// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

  Boolean
bx_gui_c::palette_change(unsigned index, unsigned red, unsigned green, unsigned blue)
{
  wxLogDebug ("palette_change");
  wxBochsPalette[index].red = red;
  wxBochsPalette[index].green = green;
  wxBochsPalette[index].blue = blue;
  return(0);
}


// ::GRAPHICS_TILE_UPDATE()
//
// Called to request that a tile of graphics be drawn to the
// screen, since info in this region has changed.
//
// tile: array of 8bit values representing a block of pixels with
//       dimension equal to the 'tilewidth' & 'tileheight' parameters to
//       ::specific_init().  Each value specifies an index into the
//       array of colors you allocated for ::palette_change()
// x0: x origin of tile
// y0: y origin of tile
//
// note: origin of tile and of window based on (0,0) being in the upper
//       left of the window.

void bx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  wxLogDebug ("graphics_tile_update");
	UpdateScreen((char *)tile, x0, y0, wxTileX, wxTileY);
}



// ::DIMENSION_UPDATE()
//
// Called when the VGA mode changes it's X,Y dimensions.
// Resize the window to this size, but you need to add on
// the height of the headerbar to the Y value.
//
// x: new VGA x size
// y: new VGA y size (add headerbar_y parameter from ::specific_init().

void bx_gui_c::dimension_update(unsigned x, unsigned y)
{
  wxLogDebug ("dimension_update");
  wxCriticalSectionLocker lock(wxScreen_lock);
  wxScreenX = x;
  wxScreenY = y;
  wxScreen = (char *)realloc(wxScreen, wxScreenX * wxScreenY * 3);
  wxASSERT (wxScreen != NULL);
  
  //theFrame->SetSize(-1, -1, wxScreenX + 6, wxScreenY + 100, 0);
  //wxSize size = theFrame->GetToolBar()->GetToolSize();
  theFrame->SetClientSize(wxScreenX, wxScreenY); // + size.GetHeight());
}


// ::CREATE_BITMAP()
//
// Create a monochrome bitmap of size 'xdim' by 'ydim', which will
// be drawn in the headerbar.  Return an integer ID to the bitmap,
// with which the bitmap can be referenced later.
//
// bmap: packed 8 pixels-per-byte bitmap.  The pixel order is:
//       bit0 is the left most pixel, bit7 is the right most pixel.
// xdim: x dimension of bitmap
// ydim: y dimension of bitmap

  unsigned
bx_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  UNUSED(bmap);
  UNUSED(xdim);
  UNUSED(ydim);
  return(0);
}


// ::HEADERBAR_BITMAP()
//
// Called to install a bitmap in the bochs headerbar (toolbar).
//
// bmap_id: will correspond to an ID returned from
//     ::create_bitmap().  'alignment' is either BX_GRAVITY_LEFT
//     or BX_GRAVITY_RIGHT, meaning install the bitmap in the next
//     available leftmost or rightmost space.
// alignment: is either BX_GRAVITY_LEFT or BX_GRAVITY_RIGHT,
//     meaning install the bitmap in the next
//     available leftmost or rightmost space.
// f: a 'C' function pointer to callback when the mouse is clicked in
//     the boundaries of this bitmap.

  unsigned
bx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  UNUSED(bmap_id);
  UNUSED(alignment);
  UNUSED(f);
  return(0);
}


// ::SHOW_HEADERBAR()
//
// Show (redraw) the current headerbar, which is composed of
// currently installed bitmaps.

  void
bx_gui_c::show_headerbar(void)
{
}


// ::REPLACE_BITMAP()
//
// Replace the bitmap installed in the headerbar ID slot 'hbar_id',
// with the one specified by 'bmap_id'.  'bmap_id' will have
// been generated by ::create_bitmap().  The old and new bitmap
// must be of the same size.  This allows the bitmap the user
// sees to change, when some action occurs.  For example when
// the user presses on the floppy icon, it then displays
// the ejected status.
//
// hbar_id: headerbar slot ID
// bmap_id: bitmap ID

  void
bx_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  UNUSED(hbar_id);
  UNUSED(bmap_id);
}


// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.

  void
bx_gui_c::exit(void)
{
  BX_INFO(("bx_gui_c::exit() not implemented yet."));
}

  void
bx_gui_c::mouse_enabled_changed_specific (Boolean val)
{
}

void UpdateScreen(char *newBits, int x, int y, int width, int height) 
{
	wxCriticalSectionLocker lock(wxScreen_lock);
	if(wxScreen != NULL) {
		for(int i = 0; i < height; i++) {
			for(int c = 0; c < width; c++) {
				wxScreen[(y * wxScreenX * 3) + ((x+c) * 3)] = wxBochsPalette[newBits[(i * width) + c]].red;
				wxScreen[(y * wxScreenX * 3) + ((x+c) * 3) + 1] = wxBochsPalette[newBits[(i * width) + c]].green;
				wxScreen[(y * wxScreenX * 3) + ((x+c) * 3) + 2] = wxBochsPalette[newBits[(i * width) + c]].blue;
			}
			y++;
		}
	}
}

void DrawBochsBitmap(int x, int y, int width, int height, char *bmap, char color)
{
	char vgaPallet[] = { (char)0x00, //Black 
						 (char)0x01, //Dark Blue
						 (char)0x02, //Dark Green
						 (char)0x03, //Dark Cyan
						 (char)0x04, //Dark Red
						 (char)0x05, //Dark Magenta
						 (char)0x06, //Brown
						 (char)0x07, //Light Gray
						 (char)0x38, //Dark Gray
						 (char)0x09, //Light Blue
						 (char)0x12, //Green
						 (char)0x1B, //Cyan
						 (char)0x24, //Light Red
						 (char)0x2D, //Magenta
						 (char)0x36, //Yellow
						 (char)0x3F  //White
						};

	char bgcolor = vgaPallet[(color >> 4) & 0xF];
	char fgcolor = vgaPallet[color & 0xF];

	char *newBits = (char *)malloc(width * height);
	memset(newBits, 0, (width * height));
	for(int i = 0; i < (width * height) / 8; i++) {
		newBits[i * 8 + 0] = (bmap[i] & 0x01) ? fgcolor : bgcolor;
		newBits[i * 8 + 1] = (bmap[i] & 0x02) ? fgcolor : bgcolor;
		newBits[i * 8 + 2] = (bmap[i] & 0x04) ? fgcolor : bgcolor;
		newBits[i * 8 + 3] = (bmap[i] & 0x08) ? fgcolor : bgcolor;
		newBits[i * 8 + 4] = (bmap[i] & 0x10) ? fgcolor : bgcolor;
		newBits[i * 8 + 5] = (bmap[i] & 0x20) ? fgcolor : bgcolor;
		newBits[i * 8 + 6] = (bmap[i] & 0x40) ? fgcolor : bgcolor;
		newBits[i * 8 + 7] = (bmap[i] & 0x80) ? fgcolor : bgcolor;
	}
	UpdateScreen(newBits, x, y, width, height);
	free(newBits);
}
