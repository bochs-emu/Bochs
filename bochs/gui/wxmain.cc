//
// gui/wxmain.cc
// $Id: wxmain.cc,v 1.1.2.3 2002-04-10 05:14:25 bdenney Exp $
//
// wxmain.cc implements the wxWindows frame, toolbar, menus, and dialogs.
// When the application starts, the user is given a chance to choose/edit/save
// a configuration.  When they decide to start the simulation, functions in
// main.cc are called in a separate thread to initialize and run the Bochs
// simulator.  
//
// The wxWindows port of the VGA display is implemented in wx.cc.  The
// separation between wxmain.cc and wx.cc is as follows:
// - wxmain.cc implements a Bochs configuration interface, which is
//   the wxWindows equivalent of control.cc.  wxmain creates a 
//   frame with several menus and a toolbar, and allows the user to
//   choose the machine configuration and start the simulation.
// - wx.cc implements a VGA display screen using wxWindows.  It is 
//   is the wxWindows equivalent of x.cc, win32.cc, macos.cc, etc.
//   wx.cc includes bochs.h and has access to all Bochs devices.
//   The VGA panel accepts only paint, key, and mouse events.  As it
//   receives events, it builds BxEvents and places them into a 
//   thread-safe BxEvent queue.  The simulation thread periodically
//   processes events from the BxEvent queue (bx_gui_c::handle_events)
//   and notifies the appropriate emulated I/O device.
//

//////////////////////////////////////////////////////////////////////
// includes
//////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/image.h"

#include "config.h"              // definitions based on configure script
#include "osdep.h"               // workarounds for missing stuff
#include "gui/siminterface.h"    // interface to the simulator
#include "bxversion.h"           // get version string
#include "wxmain.h"              // interface to the gui

// include XPM icons
#include "bitmaps/cdromd.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/floppya.xpm"
#include "bitmaps/floppyb.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/power.xpm"
#include "bitmaps/reset.xpm"
#include "bitmaps/snapshot.xpm"
#include "bitmaps/mouse.xpm"
#include "bitmaps/configbutton.xpm"

// FIXME: ugly global variables that the bx_gui_c object in wx.cc can use
// to access the MyFrame and the MyPanel.
MyFrame *theFrame = NULL;
MyPanel *thePanel = NULL;

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
  // ids for Bochs toolbar
  ID_Toolbar_FloppyA,
  ID_Toolbar_FloppyB,
  ID_Toolbar_CdromD,
  ID_Toolbar_Reset,
  ID_Toolbar_Power,
  ID_Toolbar_Copy,
  ID_Toolbar_Paste,
  ID_Toolbar_Snapshot,
  ID_Toolbar_Config,
  ID_Toolbar_Mouse_en
};

//////////////////////////////////////////////////////////////////////
// class declarations
//////////////////////////////////////////////////////////////////////

class MyApp: public wxApp
{
virtual bool OnInit();
};

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
};


//////////////////////////////////////////////////////////////////////
// MyApp: the wxWindows application
//////////////////////////////////////////////////////////////////////

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  //wxLog::AddTraceMask (_T("mime"));
  bx_init_siminterface ();
  bx_init_main ();
  MyFrame *frame = new MyFrame( "Bochs x86 Emulator", wxPoint(50,50), wxSize(450,340), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION );
  theFrame = frame;  // hack alert
  frame->Show( TRUE );
  SetTopWindow( frame );
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// MyFrame: the main frame for the Bochs application
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit, MyFrame::OnQuit)
  EVT_MENU(ID_Help_About, MyFrame::OnAbout)
  EVT_MENU(ID_Simulate_Start, MyFrame::OnStartSim)
  EVT_MENU(ID_Simulate_PauseResume, MyFrame::OnPauseResumeSim)
  EVT_MENU(ID_Simulate_Stop, MyFrame::OnKillSim)
  EVT_MENU(ID_Sim2Gui_Event, MyFrame::OnSim2GuiEvent)
  // toolbar events
  EVT_TOOL(ID_Toolbar_FloppyA, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_FloppyB, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_CdromD, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Reset, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Power, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Copy, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Paste, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Snapshot, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Config, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Mouse_en, MyFrame::OnToolbarClick)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
: wxFrame((wxFrame *)NULL, -1, title, pos, size, style)
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

  int currentX = 5;
#define BX_ADD_TOOL(id, xpm_name, tooltip) \
    do {tb->AddTool(id, wxBitmap(xpm_name), wxNullBitmap, FALSE, currentX, -1, (wxObject *)NULL, tooltip);  currentX += 34; } while (0)

  BX_ADD_TOOL(ID_Toolbar_FloppyA, floppya_xpm, "Change Floppy A");
  BX_ADD_TOOL(ID_Toolbar_FloppyB, floppyb_xpm, "Change Floppy B");
  BX_ADD_TOOL(ID_Toolbar_CdromD, cdromd_xpm, "Change CDROM");
  BX_ADD_TOOL(ID_Toolbar_Reset, reset_xpm, "Reset the system");
  BX_ADD_TOOL(ID_Toolbar_Power, power_xpm, "Turn power on/off");

  BX_ADD_TOOL(ID_Toolbar_Copy, copy_xpm, "Copy to clipboard");
  BX_ADD_TOOL(ID_Toolbar_Paste, paste_xpm, "Paste from clipboard");
  BX_ADD_TOOL(ID_Toolbar_Snapshot, snapshot_xpm, "Save screen snapshot");
  BX_ADD_TOOL(ID_Toolbar_Config, configbutton_xpm, "Runtime Configuration");
  BX_ADD_TOOL(ID_Toolbar_Mouse_en, mouse_xpm, "Mouse Enable/Disable");

  tb->Realize();

  // create a MyPanel that covers the whole frame
  panel = new MyPanel (this, -1);
  panel->SetBackgroundColour (wxColour (0,0,0));
  wxGridSizer *sz = new wxGridSizer (1, 1);
  sz->Add (panel, 0, wxGROW);
  SetAutoLayout (TRUE);
  SetSizer (sz);

  thePanel = panel;
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
  wxString str;
  str.Printf ("Bochs x86 Emulator version %s (wxWindows port)", VER_STRING);
  wxMessageBox( str, "About Bochs", wxOK | wxICON_INFORMATION );
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
  num_events = 0;  // clear the queue of events for bochs to handle
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

void MyFrame::OnToolbarClick(wxCommandEvent& event)
{
  wxLogDebug ("clicked toolbar thingy");
  bx_toolbar_buttons which = BX_TOOLBAR_UNDEFINED;
  int id = event.GetId ();
  switch (id) {
    case ID_Toolbar_Power:which = BX_TOOLBAR_POWER; break;
    case ID_Toolbar_Reset: which = BX_TOOLBAR_RESET; break;
    case ID_Toolbar_FloppyA: which = BX_TOOLBAR_FLOPPYA; break;
    case ID_Toolbar_FloppyB: which = BX_TOOLBAR_FLOPPYB; break;
    case ID_Toolbar_CdromD: which = BX_TOOLBAR_CDROMD; break;
    case ID_Toolbar_Copy: which = BX_TOOLBAR_COPY; break;
    case ID_Toolbar_Paste: which = BX_TOOLBAR_PASTE; break;
    case ID_Toolbar_Snapshot: which = BX_TOOLBAR_SNAPSHOT; break;
    case ID_Toolbar_Config: which = BX_TOOLBAR_CONFIG; break;
    case ID_Toolbar_Mouse_en: which = BX_TOOLBAR_MOUSE_EN; break;
    default:
      wxLogError ("unknown toolbar id %d", id);
  }
  if (num_events < MAX_EVENTS) {
    event_queue[num_events].type = BX_ASYNC_EVT_TOOLBAR;
    event_queue[num_events].u.toolbar.button = which;
    num_events++;
  }
}

//////////////////////////////////////////////////////////////////////
// Simulation Thread
//////////////////////////////////////////////////////////////////////

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

