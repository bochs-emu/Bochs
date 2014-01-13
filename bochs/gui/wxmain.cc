/////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////

// wxmain.cc implements the wxWidgets frame, toolbar, menus, and dialogs.
// When the application starts, the user is given a chance to choose/edit/save
// a configuration.  When they decide to start the simulation, functions in
// main.cc are called in a separate thread to initialize and run the Bochs
// simulator.
//
// Most ports to different platforms implement only the VGA window and
// toolbar buttons.  The wxWidgets port is the first to implement both
// the VGA display and the configuration interface, so the boundaries
// between them are somewhat blurry.  See the extensive comments at
// the top of siminterface for the rationale behind this separation.
//
// The separation between wxmain.cc and wx.cc is as follows:
// - wxmain.cc implements a Bochs configuration interface (CI),
//   which is the wxWidgets equivalent of textconfig.cc. wxmain creates
//   a frame with several menus and a toolbar, and allows the user to
//   choose the machine configuration and start the simulation.  Note
//   that wxmain.cc does NOT include bochs.h.  All interactions
//   between the CI and the simulator are through the siminterface
//   object.
// - wx.cc implements a VGA display screen using wxWidgets.  It is
//   is the wxWidgets equivalent of x.cc, win32.cc, macos.cc, etc.
//   wx.cc includes bochs.h and has access to all Bochs devices.
//   The VGA panel accepts only paint, key, and mouse events.  As it
//   receives events, it builds BxEvents and places them into a
//   thread-safe BxEvent queue.  The simulation thread periodically
//   processes events from the BxEvent queue (bx_wx_gui_c::handle_events)
//   and notifies the appropriate emulated I/O device.
//
//////////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "config.h"              // definitions based on configure script
#include "param_names.h"

#if BX_WITH_WX

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/image.h>
#include <wx/clipbrd.h>

#include "osdep.h"               // workarounds for missing stuff
#include "gui/siminterface.h"    // interface to the simulator
#include "bxversion.h"           // get version string
#include "wxdialog.h"            // custom dialog boxes
#include "wxmain.h"              // wxwidgets shared stuff
#include "extplugin.h"

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
//#include "bitmaps/configbutton.xpm"
#include "bitmaps/userbutton.xpm"
#include "bitmaps/saverestore.xpm"
#ifdef __WXGTK__
#include "icon_bochs.xpm"
#endif

// FIXME: ugly global variables that the bx_gui_c object in wx.cc can use
// to access the MyFrame and the MyPanel.
MyFrame *theFrame = NULL;
MyPanel *thePanel = NULL;

// The wxBochsClosing flag is used to keep track of when the wxWidgets GUI is
// shutting down.  Shutting down can be somewhat complicated because the
// simulation may be running for a while in another thread before it realizes
// that it should shut down.  The wxBochsClosing flag is a global variable, as
// opposed to a field of some C++ object, so that it will be valid at any stage
// of the program.  wxBochsClosing starts out false (not wxBochsClosing).  When
// the GUI decides to shut down, it sets wxBochsClosing=true.  If there
// is not a simulation running, everything is quite simple and it can just
// call Close(TRUE).  If a simulation is running, it calls OnKillSim to
// ask the simulation to stop.  When the simulation thread stops, it calls
// Close(TRUE) on the frame.  During the time that the simulation is
// still running and afterward, the wxBochsClosing flag is used to suppress
// any events that might reference parts of the GUI or create new dialogs.
bool wxBochsClosing = false;

// The wxBochsStopSim flag is used to select the right way when the simulation
// thread stops. It is set to 'true' when the stop simulation menu item is
// clicked instead of the power button.
bool wxBochsStopSim = false;

bool isSimThread() {
  if (wxThread::IsMain()) return false;
  wxThread *current = wxThread::This();
  if (current == (wxThread*) theFrame->GetSimThread()) {
    return true;
  }
  return false;
}

//////////////////////////////////////////////////////////////////////
// class declarations
//////////////////////////////////////////////////////////////////////

class MyApp: public wxApp
{
virtual bool OnInit();
virtual int OnExit();
  public:
  // This default callback is installed when the simthread is NOT running,
  // so that events coming from the simulator code can be handled.
  // The primary culprit is panics which cause an BX_SYNC_EVT_LOG_ASK.
  static BxEvent *DefaultCallback(void *thisptr, BxEvent *event);
};

// SimThread is the thread in which the Bochs simulator runs.  It is created
// by MyFrame::OnStartSim().  The SimThread::Entry() function calls a
// function in main.cc called bx_continue_after_config_interface() which
// initializes the devices and starts up the simulation.  All events from
// the simulator
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
  SimThread(MyFrame *_frame) { frame = _frame; sim2gui_mailbox = NULL; }
  virtual ExitCode Entry();
  void OnExit();
  // called by the siminterface code, with the pointer to the sim thread
  // in the thisptr arg.
  static BxEvent *SiminterfaceCallback(void *thisptr, BxEvent *event);
  BxEvent *SiminterfaceCallback2(BxEvent *event);
  // methods to coordinate synchronous response mailbox
  void ClearSyncResponse();
  void SendSyncResponse(BxEvent *);
  BxEvent *GetSyncResponse();
};


//////////////////////////////////////////////////////////////////////
// wxWidgets startup
//////////////////////////////////////////////////////////////////////

static int ci_callback(void *userdata, ci_command_t command)
{
  switch (command)
  {
    case CI_START:
#ifdef __WXMSW__
      // on Windows only, wxEntry needs some data that is passed into WinMain.
      // So, in main.cc we define WinMain and fill in the bx_startup_flags
      // structure with the data, so that when we're ready to call wxEntry
      // it has access to the data.
      wxEntry(
            bx_startup_flags.hInstance,
            bx_startup_flags.hPrevInstance,
            bx_startup_flags.m_lpCmdLine,
            bx_startup_flags.nCmdShow);
#else
      wxEntry(bx_startup_flags.argc, bx_startup_flags.argv);
#endif
      break;
    case CI_RUNTIME_CONFIG:
      fprintf(stderr, "wxmain.cc: runtime config not implemented\n");
      break;
    case CI_SHUTDOWN:
      fprintf(stderr, "wxmain.cc: shutdown not implemented\n");
      break;
  }
  return 0;
}

extern "C" int libwx_LTX_plugin_init(plugin_t *plugin, plugintype_t type,
  int argc, char *argv[])
{
  wxLogDebug(wxT("plugin_init for wxmain.cc"));
  wxLogDebug(wxT("installing wxWidgets as the configuration interface"));
  SIM->register_configuration_interface("wx", ci_callback, NULL);
  wxLogDebug(wxT("installing %s as the Bochs GUI"), wxT("wxWidgets"));
  SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY)->set_enabled(0);
  MyPanel::OnPluginInit();
  return 0; // success
}

extern "C" void libwx_LTX_plugin_fini()
{
  // Nothing here yet
}


//////////////////////////////////////////////////////////////////////
// MyApp: the wxWidgets application
//////////////////////////////////////////////////////////////////////

IMPLEMENT_APP_NO_MAIN(MyApp)

// this is the entry point of the wxWidgets code.  It is called as follows:
// 1. main() loads the wxWidgets plugin (if necessary) and calls
// libwx_LTX_plugin_init, which installs a function pointer to the
// ci_callback() function.
// 2. main() calls SIM->configuration_interface.
// 3. bx_real_sim_c::configuration_interface calls the function pointer that
//    points to ci_callback() in this file, with command=CI_START.
// 4. ci_callback() calls wxEntry() in the wxWidgets library
// 5. wxWidgets library creates the app and calls OnInit().
//
// Before this code is called, the command line has already been parsed, and a
// .bochsrc has been loaded if it could be found.  See main() for details.
bool MyApp::OnInit()
{
  // wxLog::AddTraceMask(wxT("mime"));
  wxLog::SetActiveTarget(new wxLogStderr());
  // Install callback function to handle anything that occurs before the
  // simulation begins.  This is responsible for displaying any error
  // dialogs during bochsrc and command line processing.
  SIM->set_notify_callback(&MyApp::DefaultCallback, this);
  MyFrame *frame = new MyFrame(wxT("Bochs x86 Emulator"), wxPoint(50,50), wxSize(450,340), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION);
  theFrame = frame;  // hack alert
  frame->Show(TRUE);
  SetTopWindow(frame);
  wxTheClipboard->UsePrimarySelection(true);
  // if quickstart is enabled, kick off the simulation
  if (SIM->get_param_enum(BXPN_BOCHS_START)->get() == BX_QUICK_START) {
    wxCommandEvent unusedEvent;
    frame->OnStartSim(unusedEvent);
  }
  return TRUE;
}

int MyApp::OnExit()
{
  return 0;
}

// these are only called when the simthread is not running.
BxEvent *MyApp::DefaultCallback(void *thisptr, BxEvent *event)
{
  wxLogDebug(wxT("DefaultCallback: event type %d"), event->type);
  event->retcode = -1;  // default return code
  switch (event->type)
  {
    case BX_ASYNC_EVT_LOG_MSG:
    case BX_SYNC_EVT_LOG_ASK: {
      wxLogDebug(wxT("DefaultCallback: log ask event"));
      wxString text;
      text.Printf(wxT("Error: %s"), event->u.logmsg.msg);
      if (wxBochsClosing) {
        // gui closing down, do something simple and nongraphical.
        fprintf(stderr, "%s\n", (const char *)text.mb_str(wxConvUTF8));
      } else {
        wxMessageBox(text, wxT("Error"), wxOK | wxICON_ERROR);
        // maybe I can make OnLogAsk display something that looks appropriate.
        // theFrame->OnLogAsk(event);
      }
      event->retcode = BX_LOG_ASK_CHOICE_DIE;
      // There is only one thread at this point.  if I choose DIE here, it will
      // call fatal() and kill the whole app.
      break;
    }
    case BX_SYNC_EVT_TICK:
      if (wxBochsClosing)
        event->retcode = -1;
      break;
    case BX_ASYNC_EVT_REFRESH:
    case BX_ASYNC_EVT_DBG_MSG:
      break;  // ignore
    case BX_SYNC_EVT_ASK_PARAM:
    case BX_SYNC_EVT_GET_DBG_COMMAND:
      break;  // ignore
    default:
      wxLogDebug(wxT("DefaultCallback: unknown event type %d"), event->type);
  }
  if (BX_EVT_IS_ASYNC(event->type)) {
    delete event;
    event = NULL;
  }
  return event;
}

//////////////////////////////////////////////////////////////////////
// MyFrame: the top level frame for the Bochs application
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Config_New, MyFrame::OnConfigNew)
  EVT_MENU(ID_Config_Read, MyFrame::OnConfigRead)
  EVT_MENU(ID_Config_Save, MyFrame::OnConfigSave)
  EVT_MENU(ID_State_Restore, MyFrame::OnStateRestore)
  EVT_MENU(ID_Quit, MyFrame::OnQuit)
  EVT_MENU(ID_Help_About, MyFrame::OnAbout)
  EVT_MENU(ID_Simulate_Start, MyFrame::OnStartSim)
  EVT_MENU(ID_Simulate_PauseResume, MyFrame::OnPauseResumeSim)
  EVT_MENU(ID_Simulate_Stop, MyFrame::OnKillSim)
  EVT_MENU(ID_Sim2CI_Event, MyFrame::OnSim2CIEvent)
  EVT_MENU(ID_Edit_Plugins, MyFrame::OnEditPluginCtrl)
  EVT_MENU(ID_Edit_ATA0, MyFrame::OnEditATA)
  EVT_MENU(ID_Edit_ATA1, MyFrame::OnEditATA)
  EVT_MENU(ID_Edit_ATA2, MyFrame::OnEditATA)
  EVT_MENU(ID_Edit_ATA3, MyFrame::OnEditATA)
  EVT_MENU(ID_Edit_CPU, MyFrame::OnEditCPU)
  EVT_MENU(ID_Edit_CPUID, MyFrame::OnEditCPUID)
  EVT_MENU(ID_Edit_Memory, MyFrame::OnEditMemory)
  EVT_MENU(ID_Edit_Clock_Cmos, MyFrame::OnEditClockCmos)
  EVT_MENU(ID_Edit_PCI, MyFrame::OnEditPCI)
  EVT_MENU(ID_Edit_Display, MyFrame::OnEditDisplay)
  EVT_MENU(ID_Edit_Keyboard, MyFrame::OnEditKeyboard)
  EVT_MENU(ID_Edit_Boot, MyFrame::OnEditBoot)
  EVT_MENU(ID_Edit_Serial_Parallel, MyFrame::OnEditSerialParallel)
  EVT_MENU(ID_Edit_Network, MyFrame::OnEditNet)
  EVT_MENU(ID_Edit_Sound, MyFrame::OnEditSound)
  EVT_MENU(ID_Edit_Other, MyFrame::OnEditOther)
  EVT_MENU(ID_Log_Prefs, MyFrame::OnLogPrefs)
  EVT_MENU(ID_Log_PrefsDevice, MyFrame::OnLogPrefsDevice)
  EVT_MENU(ID_Log_View, MyFrame::OnLogView)
  // toolbar events
  EVT_TOOL(ID_Edit_FD_0, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Edit_FD_1, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Edit_Cdrom1, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Reset, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Power, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_SaveRestore, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Copy, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Paste, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Snapshot, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_Mouse_en, MyFrame::OnToolbarClick)
  EVT_TOOL(ID_Toolbar_User, MyFrame::OnToolbarClick)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////
// Menu layout (approximate)
//
// The actual menus will be changing so this probably isn't up
// to date, but having it in text form was useful in planning.
//////////////////////////////////////////////////////////////////
// - File
//   +----------------------+
//   | New Configuration    |
//   | Read Configuration   |
//   | Save Configuration   |
//   +----------------------+
//   | Quit                 |
//   +----------------------+
// - Edit
//   +----------------------+
//   | Floppy Disk 0...     |
//   | Floppy Disk 1...     |
//   | Hard Disk 0...       |
//   | Hard Disk 1...       |
//   | Cdrom...             |
//   | Boot...              |
//   | VGA...               |
//   | Memory...            |
//   | Sound...             |
//   | Networking...        |
//   | Keyboard...          |
//   | Other...             |
//   +----------------------+
// - Simulate
//   +----------------------+
//   | Start                |
//   | Pause/Resume         |
//   | Stop                 |
//   +----------------------|
// - Debug
//   +----------------------|
//   | Show CPU             |
//   | Debug Console        |
//   +----------------------|
// - Event Log
//   +----------------------+
//   | View                 |
//   | Preferences...       |
//   | By Device...         |
//   +----------------------+
// - Help
//   +----------------------+
//   | About Bochs...       |
//   +----------------------+
//////////////////////////////////////////////////////////////////

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
: wxFrame((wxFrame *)NULL, -1, title, pos, size, style)
{
  SetIcon(wxICON(icon_bochs));

  // init variables
  sim_thread = NULL;
  start_bochs_times = 0;

  // set up the gui
  menuConfiguration = new wxMenu;
  menuConfiguration->Append(ID_Config_New, wxT("&New Configuration"));
  menuConfiguration->Append(ID_Config_Read, wxT("&Read Configuration"));
  menuConfiguration->Append(ID_Config_Save, wxT("&Save Configuration"));
  menuConfiguration->AppendSeparator();
  menuConfiguration->Append(ID_State_Restore, wxT("&Restore State"));
  menuConfiguration->AppendSeparator();
  menuConfiguration->Append(ID_Quit, wxT("&Quit"));

  menuEdit = new wxMenu;
  menuEdit->Append(ID_Edit_Plugins, wxT("Pl&ugin Control..."));
  menuEdit->Append(ID_Edit_FD_0, wxT("Floppy Disk &0..."));
  menuEdit->Append(ID_Edit_FD_1, wxT("Floppy Disk &1..."));
  menuEdit->Append(ID_Edit_ATA0, wxT("ATA Channel 0..."));
  menuEdit->Append(ID_Edit_ATA1, wxT("ATA Channel 1..."));
  menuEdit->Append(ID_Edit_ATA2, wxT("ATA Channel 2..."));
  menuEdit->Append(ID_Edit_ATA3, wxT("ATA Channel 3..."));
  menuEdit->Append(ID_Edit_CPU, wxT("&CPU..."));
  menuEdit->Append(ID_Edit_CPUID, wxT("CPU&ID..."));
  menuEdit->Append(ID_Edit_Memory, wxT("&Memory..."));
  menuEdit->Append(ID_Edit_Clock_Cmos, wxT("C&lock/Cmos..."));
  menuEdit->Append(ID_Edit_PCI, wxT("&PCI..."));
  menuEdit->Append(ID_Edit_Display, wxT("&Display + Interface..."));
  menuEdit->Append(ID_Edit_Keyboard, wxT("&Keyboard + Mouse..."));
  menuEdit->Append(ID_Edit_Boot, wxT("&Boot..."));
  menuEdit->Append(ID_Edit_Serial_Parallel, wxT("&Serial/Parallel/USB..."));
  menuEdit->Append(ID_Edit_Network, wxT("&Network..."));
  menuEdit->Append(ID_Edit_Sound, wxT("S&ound..."));
  menuEdit->Append(ID_Edit_Other, wxT("&Other..."));

  menuSimulate = new wxMenu;
  menuSimulate->Append(ID_Simulate_Start, wxT("&Start..."));
  menuSimulate->Append(ID_Simulate_PauseResume, wxT("&Pause..."));
  menuSimulate->Append(ID_Simulate_Stop, wxT("S&top..."));
  menuSimulate->AppendSeparator();
  menuSimulate->Enable(ID_Simulate_PauseResume, FALSE);
  menuSimulate->Enable(ID_Simulate_Stop, FALSE);

  menuLog = new wxMenu;
  menuLog->Append(ID_Log_View, wxT("&View"));
  menuLog->Append(ID_Log_Prefs, wxT("&Preferences..."));
  menuLog->Append(ID_Log_PrefsDevice, wxT("By &Device..."));

  menuHelp = new wxMenu;
  menuHelp->Append(ID_Help_About, wxT("&About..."));

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuConfiguration, wxT("&File"));
  menuBar->Append(menuEdit, wxT("&Edit"));
  menuBar->Append(menuSimulate, wxT("&Simulate"));
  menuBar->Append(menuLog, wxT("&Log"));
  menuBar->Append(menuHelp, wxT("&Help"));
  SetMenuBar(menuBar);

  // enable ATA channels in menu
  menuEdit->Enable(ID_Edit_ATA1, BX_MAX_ATA_CHANNEL > 1);
  menuEdit->Enable(ID_Edit_ATA2, BX_MAX_ATA_CHANNEL > 2);
  menuEdit->Enable(ID_Edit_ATA3, BX_MAX_ATA_CHANNEL > 3);
  // enable restore state
  menuConfiguration->Enable(ID_State_Restore, TRUE);

  CreateStatusBar();
  wxStatusBar *sb = GetStatusBar();
  sb->SetFieldsCount(12);
  const int sbwidth[12] = {160, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, -1};
  sb->SetStatusWidths(12, sbwidth);

  CreateToolBar(wxNO_BORDER|wxHORIZONTAL|wxTB_FLAT);
  bxToolBar = GetToolBar();
  bxToolBar->SetToolBitmapSize(wxSize(32, 32));

#define BX_ADD_TOOL(id, xpm_name, tooltip) do { \
    bxToolBar->AddTool(id, wxBitmap(xpm_name), tooltip); \
  } while (0)

  BX_ADD_TOOL(ID_Edit_FD_0, floppya_xpm, wxT("Change Floppy A"));
  BX_ADD_TOOL(ID_Edit_FD_1, floppyb_xpm, wxT("Change Floppy B"));
  BX_ADD_TOOL(ID_Edit_Cdrom1, cdromd_xpm, wxT("Change CDROM"));
  BX_ADD_TOOL(ID_Toolbar_Reset, reset_xpm, wxT("Reset the system"));
  BX_ADD_TOOL(ID_Toolbar_Power, power_xpm, wxT("Turn power on/off"));
  BX_ADD_TOOL(ID_Toolbar_SaveRestore, saverestore_xpm, wxT(""));

  BX_ADD_TOOL(ID_Toolbar_Copy, copy_xpm, wxT("Copy to clipboard"));
  BX_ADD_TOOL(ID_Toolbar_Paste, paste_xpm, wxT("Paste from clipboard"));
  BX_ADD_TOOL(ID_Toolbar_Snapshot, snapshot_xpm, wxT("Save screen snapshot"));
  BX_ADD_TOOL(ID_Toolbar_Mouse_en, mouse_xpm, wxT("Enable mouse capture\nThere is also a shortcut for this: a CTRL key + the middle mouse button."));
  BX_ADD_TOOL(ID_Toolbar_User, userbutton_xpm, wxT("Keyboard shortcut"));

  bxToolBar->Realize();
  UpdateToolBar(false);

  // create a MyPanel that covers the whole frame
  panel = new MyPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
  panel->SetBackgroundColour(wxColour(0,0,0));
  panel->SetFocus();
  wxGridSizer *sz = new wxGridSizer(1, 1);
  sz->Add(panel, 0, wxGROW);
  SetAutoLayout(TRUE);
  SetSizer(sz);

  // create modeless logfile viewer
  showLogView = new LogViewDialog(this, -1);
  showLogView->Init();
}

MyFrame::~MyFrame()
{
  delete panel;
  delete showLogView;
  wxLogDebug(wxT("MyFrame destructor"));
  theFrame = NULL;
}

void MyFrame::OnConfigNew(wxCommandEvent& WXUNUSED(event))
{
  int answer = wxMessageBox(wxT("This will reset all settings back to their default values.\nAre you sure you want to do this?"),
    wxT("Are you sure?"), wxYES_NO | wxCENTER, this);
  if (answer == wxYES) SIM->reset_all_param();
}

void MyFrame::OnConfigRead(wxCommandEvent& WXUNUSED(event))
{
  char bochsrc[512];
  long style = wxOPEN;
  wxFileDialog *fdialog = new wxFileDialog(this, wxT("Read configuration"), wxT(""), wxT(""), wxT("*.*"), style);
  if (fdialog->ShowModal() == wxID_OK) {
    strncpy(bochsrc, fdialog->GetPath().mb_str(wxConvUTF8), sizeof(bochsrc));
    SIM->reset_all_param();
    SIM->read_rc(bochsrc);
  }
  delete fdialog;
}

void MyFrame::OnConfigSave(wxCommandEvent& WXUNUSED(event))
{
  char bochsrc[512];
  long style = wxSAVE | wxOVERWRITE_PROMPT;
  wxFileDialog *fdialog = new wxFileDialog(this, wxT("Save configuration"), wxT(""), wxT(""), wxT("*.*"), style);
  if (fdialog->ShowModal() == wxID_OK) {
    strncpy(bochsrc, fdialog->GetPath().mb_str(wxConvUTF8), sizeof(bochsrc));
    SIM->write_rc(bochsrc, 1);
  }
  delete fdialog;
}

void MyFrame::OnStateRestore(wxCommandEvent& WXUNUSED(event))
{
  char sr_path[512];
  // pass some initial dir to wxDirDialog
  wxString dirSaveRestore;

  wxGetHomeDir(&dirSaveRestore);
  wxDirDialog ddialog(this, wxT("Select folder with save/restore data"), dirSaveRestore, wxDD_DEFAULT_STYLE);

  if (ddialog.ShowModal() == wxID_OK) {
    strncpy(sr_path, ddialog.GetPath().mb_str(wxConvUTF8), sizeof(sr_path));
    SIM->get_param_bool(BXPN_RESTORE_FLAG)->set(1);
    SIM->get_param_string(BXPN_RESTORE_PATH)->set(sr_path);
  }
}

void MyFrame::OnEditPluginCtrl(wxCommandEvent& WXUNUSED(event))
{
  PluginControlDialog dlg(this, -1);
  dlg.ShowModal();
}

void MyFrame::OnEditCPU(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("cpu");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.ShowModal();
}

void MyFrame::OnEditCPUID(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("cpuid");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.ShowModal();
}

void MyFrame::OnEditMemory(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("memory");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.ShowModal();
}

void MyFrame::OnEditClockCmos(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("clock_cmos");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.ShowModal();
}

void MyFrame::OnEditPCI(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("pci");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.ShowModal();
}

void MyFrame::OnEditDisplay(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("display");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::OnEditKeyboard(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("keyboard_mouse");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::OnEditBoot(wxCommandEvent& WXUNUSED(event))
{
  int bootDevices = 0;
  bx_param_enum_c *floppy = SIM->get_param_enum(BXPN_FLOPPYA_DEVTYPE);
  if (floppy->get() != BX_FDD_NONE) {
    bootDevices++;
  }
  bx_param_c *firsthd = SIM->get_first_hd();
  if (firsthd != NULL) {
    bootDevices++;
  }
  bx_param_c *firstcd = SIM->get_first_cdrom();
  if (firstcd != NULL) {
    bootDevices++;
  }
  if (bootDevices == 0) {
    wxMessageBox(wxT("All the possible boot devices are disabled right now!\nYou must enable the first floppy drive, a hard drive, or a CD-ROM."),
                 wxT("None enabled"), wxOK | wxICON_ERROR, this);
    return;
  }
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("boot_params");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.ShowModal();
}

void MyFrame::OnEditSerialParallel(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("ports");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::OnEditNet(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("network");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.ShowModal();
}

void MyFrame::OnEditSound(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("sound");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::OnEditOther(wxCommandEvent& WXUNUSED(event))
{
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param("misc");
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::OnLogPrefs(wxCommandEvent& WXUNUSED(event))
{
  // Ideally I would use the siminterface methods to fill in the fields
  // of the LogOptionsDialog, but in fact several things are hardcoded.
  // At least I can verify that the expected numbers are the same.
  wxASSERT(SIM->get_max_log_level() == LOG_OPTS_N_TYPES);
  LogOptionsDialog dlg(this, -1);

  // The inital values of the dialog are complicated.  If the panic action
  // for all modules is "ask", then clearly the inital value in the dialog
  // for panic action should be "ask".  This informs the user what the
  // previous value was, and if they click Ok it won't do any harm.  But if
  // some devices are set to "ask" and others are set to "report", then the
  // initial value should be "no change".  With "no change", clicking on Ok
  // will not destroy the settings for individual devices.  You would only
  // start to see "no change" if you've been messing around in the advanced
  // menu already.
  int level, nlevel = SIM->get_max_log_level();
  for (level=0; level<nlevel; level++) {
    int mod = 0;
    int first = SIM->get_log_action(mod, level);
    bool consensus = true;
    // now compare all others to first.  If all match, then use "first" as
    // the initial value.
    for (mod=1; mod<SIM->get_n_log_modules(); mod++) {
      if (first != SIM->get_log_action(mod, level)) {
        consensus = false;
        break;
      }
    }
    if (consensus)
      dlg.SetAction(level, first);
    else
      dlg.SetAction(level, LOG_OPTS_NO_CHANGE);
  }
  dlg.SetRuntimeFlag(sim_thread != NULL);
  int n = dlg.ShowModal();   // show the dialog!
  if (n == wxID_OK) {
    for (level=0; level<nlevel; level++) {
      // ask the dialog what action the user chose for this type of event
      int action = dlg.GetAction(level);
      if (action != LOG_OPTS_NO_CHANGE) {
        // set new default
        SIM->set_default_log_action(level, action);
        // apply that action to all modules (devices)
        SIM->set_log_action(-1, level, action);
      }
    }
  }
}

void MyFrame::OnLogPrefsDevice(wxCommandEvent& WXUNUSED(event))
{
  wxASSERT(SIM->get_max_log_level() == ADVLOG_OPTS_N_TYPES);
  AdvancedLogOptionsDialog dlg(this, -1);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::OnLogView(wxCommandEvent& WXUNUSED(event))
{
  wxASSERT(showLogView != NULL);
  showLogView->Show(true);
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
  wxBochsClosing = true;
  bx_user_quit = 1;
  if (!sim_thread) {
    // no simulation thread is running. Just close the window.
    Close(TRUE);
  } else {
    SIM->set_notify_callback(&MyApp::DefaultCallback, this);
    // ask the simulator to stop.  When it stops it will close this frame.
    SetStatusText(wxT("Waiting for simulation to stop..."));
    OnKillSim(event);
  }
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString str(wxT("Bochs x86 Emulator version "));
  str += wxString(VER_STRING, wxConvUTF8);
  str += wxT(" (wxWidgets port)");
  wxMessageBox(str, wxT("About Bochs"), wxOK | wxICON_INFORMATION, this);
}

// update the menu items, status bar, etc.
void MyFrame::simStatusChanged(StatusChange change, bx_bool popupNotify) {
  char ata_name[20];
  bx_list_c *base;

  switch (change) {
    case Start:  // running
      wxLogStatus(wxT("Starting Bochs simulation"));
      menuSimulate->Enable(ID_Simulate_Start, FALSE);
      menuSimulate->Enable(ID_Simulate_PauseResume, TRUE);
      menuSimulate->Enable(ID_Simulate_Stop, TRUE);
      menuSimulate->SetLabel(ID_Simulate_PauseResume, wxT("&Pause"));
      break;
    case Stop: // not running
      wxLogStatus(wxT("Simulation stopped"));
      menuSimulate->Enable(ID_Simulate_Start, TRUE);
      menuSimulate->Enable(ID_Simulate_PauseResume, FALSE);
      menuSimulate->Enable(ID_Simulate_Stop, FALSE);
      menuSimulate->SetLabel(ID_Simulate_PauseResume, wxT("&Pause"));
      // This should only be used if the simulation stops due to error.
      // Obviously if the user asked it to stop, they don't need to be told.
      if (popupNotify)
        wxMessageBox(wxT("Bochs simulation has stopped."), wxT("Bochs Stopped"),
            wxOK | wxICON_INFORMATION, this);
      break;
    case Pause: // pause
      wxLogStatus(wxT("Pausing simulation"));
      menuSimulate->SetLabel(ID_Simulate_PauseResume, wxT("&Resume"));
      break;
    case Resume: // resume
      wxLogStatus(wxT("Resuming simulation"));
      menuSimulate->SetLabel(ID_Simulate_PauseResume, wxT("&Pause"));
      break;
  }
  bool canConfigure = (change == Stop);
  menuConfiguration->Enable(ID_Config_New, canConfigure);
  menuConfiguration->Enable(ID_Config_Read, canConfigure);
  menuConfiguration->Enable(ID_State_Restore, canConfigure);

  // only enabled ATA channels with a cdrom connected are available at runtime
  for (unsigned i=0; i<BX_MAX_ATA_CHANNEL; i++) {
    sprintf(ata_name, "ata.%d.resources", i);
    base = (bx_list_c*) SIM->get_param(ata_name);
    if (!SIM->get_param_bool("enabled", base)->get()) {
      menuEdit->Enable(ID_Edit_ATA0+i, canConfigure);
    } else {
      sprintf(ata_name, "ata.%d.master", i);
      base = (bx_list_c*) SIM->get_param(ata_name);
      if (SIM->get_param_enum("type", base)->get() != BX_ATA_DEVICE_CDROM) {
        sprintf(ata_name, "ata.%d.slave", i);
        base = (bx_list_c*) SIM->get_param(ata_name);
        if (SIM->get_param_enum("type", base)->get() != BX_ATA_DEVICE_CDROM) {
          menuEdit->Enable(ID_Edit_ATA0+i, canConfigure);
        }
      }
    }
  }
  menuEdit->Enable(ID_Edit_Plugins, canConfigure);
  menuEdit->Enable(ID_Edit_CPU, canConfigure);
  menuEdit->Enable(ID_Edit_CPUID, canConfigure);
  menuEdit->Enable(ID_Edit_Memory, canConfigure);
  menuEdit->Enable(ID_Edit_Clock_Cmos, canConfigure);
  menuEdit->Enable(ID_Edit_PCI, canConfigure);
  menuEdit->Enable(ID_Edit_Boot, canConfigure);
  menuEdit->Enable(ID_Edit_Network, canConfigure);
  menuEdit->Enable(ID_Edit_Other, canConfigure);
  // during simulation, certain menu options like the floppy disk
  // can be modified under some circumstances.  A floppy drive can
  // only be edited if it was enabled at boot time.
  Bit64u value;
  value = SIM->get_param_enum(BXPN_FLOPPYA_DEVTYPE)->get();
  menuEdit->Enable(ID_Edit_FD_0, canConfigure || (value != BX_FDD_NONE));
  bxToolBar->EnableTool(ID_Edit_FD_0, canConfigure || (value != BX_FDD_NONE));
  value = SIM->get_param_enum(BXPN_FLOPPYB_DEVTYPE)->get();
  menuEdit->Enable(ID_Edit_FD_1, canConfigure || (value != BX_FDD_NONE));
  bxToolBar->EnableTool(ID_Edit_FD_1, canConfigure || (value != BX_FDD_NONE));
  bxToolBar->EnableTool(ID_Edit_Cdrom1, canConfigure || (SIM->get_first_cdrom() != NULL));
  UpdateToolBar(!canConfigure);
}

void MyFrame::UpdateToolBar(bool simPresent)
{
  bxToolBar->EnableTool(ID_Toolbar_Reset, simPresent);
  bxToolBar->EnableTool(ID_Toolbar_Copy, simPresent);
  bxToolBar->EnableTool(ID_Toolbar_Paste, simPresent);
  bxToolBar->EnableTool(ID_Toolbar_Snapshot, simPresent);
  bxToolBar->EnableTool(ID_Toolbar_Mouse_en, simPresent);
  bxToolBar->EnableTool(ID_Toolbar_User, simPresent);
  if (simPresent) {
    bxToolBar->SetToolShortHelp(ID_Toolbar_SaveRestore, wxT("Save simulation state"));
  } else {
    bxToolBar->SetToolShortHelp(ID_Toolbar_SaveRestore, wxT("Restore simulation state"));
  }
}

void MyFrame::OnStartSim(wxCommandEvent& event)
{
  wxCriticalSectionLocker lock(sim_thread_lock);
  if (sim_thread != NULL) {
        wxMessageBox(
          wxT("Can't start Bochs simulator, because it is already running"),
          wxT("Already Running"), wxOK | wxICON_ERROR, this);
        return;
  }
  // check that display library is set to wx.  If not, give a warning and
  // change it to wx.  It is technically possible to use other vga libraries
  // with the wx config interface, but there are still some significant
  // problems.
  bx_param_enum_c *gui_param = SIM->get_param_enum(BXPN_SEL_DISPLAY_LIBRARY);
  const char *gui_name = gui_param->get_selected();
  if (strcmp(gui_name, "wx") != 0) {
    wxMessageBox(wxT(
    "The display library was not set to wxWidgets.  When you use the\n"
    "wxWidgets configuration interface, you must also select the wxWidgets\n"
    "display library.  I will change it to 'wx' now."),
    wxT("display library error"), wxOK | wxICON_WARNING, this);
    if (!gui_param->set_by_name("wx")) {
      wxASSERT(0 && "Could not set display library setting to 'wx");
    }
  }
  // give warning about restarting the simulation
  start_bochs_times++;
  if (start_bochs_times>1) {
        wxMessageBox(wxT(
        "You have already started the simulator once this session. Due to memory leaks and bugs in init code, you may get unstable behavior."),
        wxT("2nd time warning"), wxOK | wxICON_WARNING, this);
  }
  num_events = 0;  // clear the queue of events for bochs to handle
  wxBochsStopSim = false;
  sim_thread = new SimThread(this);
  sim_thread->Create();
  sim_thread->Run();
  wxLogDebug(wxT("Simulator thread has started."));
  // set up callback for events from simulator thread
  SIM->set_notify_callback(&SimThread::SiminterfaceCallback, sim_thread);
  simStatusChanged(Start);
}

void MyFrame::OnPauseResumeSim(wxCommandEvent& WXUNUSED(event))
{
  wxCriticalSectionLocker lock(sim_thread_lock);
  if (sim_thread) {
    if (sim_thread->IsPaused()) {
      SIM->update_runtime_options();
      simStatusChanged(Resume);
      sim_thread->Resume();
    } else {
      simStatusChanged(Pause);
      sim_thread->Pause();
    }
  }
}

bx_bool MyFrame::SimThreadControl(bx_bool resume)
{
  bx_bool sim_running = 0;

  wxCriticalSectionLocker lock(sim_thread_lock);
  if (sim_thread) {
    sim_running = !sim_thread->IsPaused();
    if (resume) {
      sim_thread->Resume();
    } else if (sim_running) {
      sim_thread->Pause();
    }
  }
  return sim_running;
}

void MyFrame::OnKillSim(wxCommandEvent& WXUNUSED(event))
{
  // DON'T use a critical section here.  Delete implicitly calls
  // OnSimThreadExit, which also tries to lock sim_thread_lock.
  // If we grab the lock at this level, deadlock results.
  wxLogDebug(wxT("OnKillSim()"));
#if BX_DEBUGGER && BX_DEBUGGER_GUI
  bx_user_quit = 1;
#endif
  if (sim_thread) {
    wxBochsStopSim = true;
    sim_thread->Delete();
    // Next time the simulator reaches bx_real_sim_c::periodic() it
    // will quit.  This is better than killing the thread because it
    // gives it a chance to clean up after itself.
  }
  if (!wxBochsClosing) {
    theFrame->simStatusChanged(theFrame->Stop, true);
  }
}

void MyFrame::OnSimThreadExit()
{
  wxCriticalSectionLocker lock(sim_thread_lock);
  sim_thread = NULL;
}

int MyFrame::HandleAskParamString(bx_param_string_c *param)
{
  wxLogDebug(wxT("HandleAskParamString start"));
  int n_opt = param->get_options();
  const char *msg = param->get_label();
  if ((msg == NULL) || (strlen(msg) == 0)) {
    msg = param->get_name();
  }
  char newval[512];
  newval[0] = 0;
  wxDialog *dialog = NULL;
  if (n_opt & param->SELECT_FOLDER_DLG) {
    // pass some initial dir to wxDirDialog
    wxString homeDir;

    wxGetHomeDir(&homeDir);
    wxDirDialog *ddialog = new wxDirDialog(this, wxString(msg, wxConvUTF8), homeDir, wxDD_DEFAULT_STYLE);

    if (ddialog->ShowModal() == wxID_OK)
      strncpy(newval, ddialog->GetPath().mb_str(wxConvUTF8), sizeof(newval));
    dialog = ddialog; // so I can delete it
  } else if (n_opt & param->IS_FILENAME) {
    // use file open dialog
    long style =
      (n_opt & param->SAVE_FILE_DIALOG) ? wxSAVE|wxOVERWRITE_PROMPT : wxOPEN;
    wxFileDialog *fdialog = new wxFileDialog(this, wxString(msg, wxConvUTF8), wxT(""), wxString(param->getptr(), wxConvUTF8), wxT("*.*"), style);
    if (fdialog->ShowModal() == wxID_OK)
      strncpy(newval, fdialog->GetPath().mb_str(wxConvUTF8), sizeof(newval));
    dialog = fdialog; // so I can delete it
  } else {
    // use simple string dialog
    long style = wxOK|wxCANCEL;
    wxTextEntryDialog *tdialog = new wxTextEntryDialog(this, wxString(msg, wxConvUTF8), wxT("Enter new value"), wxString(param->getptr(), wxConvUTF8), style);
    if (tdialog->ShowModal() == wxID_OK)
      strncpy(newval, tdialog->GetValue().mb_str(wxConvUTF8), sizeof(newval));
    dialog = tdialog; // so I can delete it
  }
  if (strlen(newval) > 0) {
    // change floppy path to this value.
    wxLogDebug(wxT("Setting param %s to '%s'"), param->get_name(), newval);
    param->set(newval);
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
int MyFrame::HandleAskParam(BxEvent *event)
{
  wxASSERT(event->type == BX_SYNC_EVT_ASK_PARAM);

  bx_param_c *param = event->u.param.param;
  Raise();  // bring window to front so that you will see the dialog
  switch (param->get_type())
  {
    case BXT_PARAM_STRING:
      return HandleAskParamString((bx_param_string_c *)param);
    case BXT_PARAM_BOOL:
      {
        long style = wxYES_NO;
        if (((bx_param_bool_c *)param)->get() == 0) style |= wxNO_DEFAULT;
        ((bx_param_bool_c *)param)->set(wxMessageBox(wxString(param->get_description(), wxConvUTF8),
                                                     wxString(param->get_label(), wxConvUTF8), style, this) == wxYES);
        return 0;
      }
    default:
      {
        wxString msg;
        msg.Printf(wxT("ask param for parameter type %d is not implemented in wxWidgets"),
                   param->get_type());
        wxMessageBox(msg, wxT("not implemented"), wxOK | wxICON_ERROR, this);
        return -1;
      }
  }
  return -1;  // could not display
}

// This is called from the wxWidgets GUI thread, when a Sim2CI event
// is found.  (It got there via wxPostEvent in SiminterfaceCallback2, which is
// executed in the simulator Thread.)
void MyFrame::OnSim2CIEvent(wxCommandEvent& event)
{
  IFDBG_EVENT(wxLogDebug(wxT("received a bochs event in the GUI thread")));
  BxEvent *be = (BxEvent *) event.GetEventObject();
  IFDBG_EVENT(wxLogDebug(wxT("event type = %d"), (int)be->type));
  // all cases should return.  sync event handlers MUST send back a
  // response.  async event handlers MUST delete the event.
  switch (be->type) {
  case BX_SYNC_EVT_ASK_PARAM:
    wxLogDebug(wxT("before HandleAskParam"));
    be->retcode = HandleAskParam(be);
    wxLogDebug(wxT("after HandleAskParam"));
    // return a copy of the event back to the sender.
    sim_thread->SendSyncResponse(be);
    wxLogDebug(wxT("after SendSyncResponse"));
    break;
  case BX_ASYNC_EVT_LOG_MSG:
    showLogView->AppendText(be->u.logmsg.level, wxString(be->u.logmsg.msg, wxConvUTF8));
    free((void*)be->u.logmsg.msg);
    break;
  case BX_SYNC_EVT_LOG_ASK:
    OnLogAsk(be);
    break;
  case BX_ASYNC_EVT_QUIT_SIM:
    wxMessageBox(wxT("Bochs simulation has stopped."), wxT("Bochs Stopped"),
        wxOK | wxICON_INFORMATION, this);
    break;
  default:
    wxLogDebug(wxT("OnSim2CIEvent: event type %d ignored"), (int)be->type);
    if (!BX_EVT_IS_ASYNC(be->type)) {
      // if it's a synchronous event, and we fail to send back a response,
      // the sim thread will wait forever.  So send something!
      sim_thread->SendSyncResponse(be);
    }
    break;
  }
  if (BX_EVT_IS_ASYNC(be->type))
    delete be;
}

void MyFrame::OnLogAsk(BxEvent *be)
{
  wxLogDebug(wxT("log msg: level=%d, prefix='%s', msg='%s'"),
      be->u.logmsg.level,
      be->u.logmsg.prefix,
      be->u.logmsg.msg);
  wxASSERT(be->type == BX_SYNC_EVT_LOG_ASK);
  wxString levelName(SIM->get_log_level_name(be->u.logmsg.level), wxConvUTF8);
  LogMsgAskDialog dlg(this, -1, levelName);  // panic, error, etc.
#if !BX_DEBUGGER && !BX_GDBSTUB
  dlg.EnableButton(dlg.DEBUG, FALSE);
#endif
  dlg.SetContext(wxString(be->u.logmsg.prefix, wxConvUTF8));
  dlg.SetMessage(wxString(be->u.logmsg.msg, wxConvUTF8));
  int n = dlg.ShowModal();
  // turn the return value into the constant that logfunctions::ask is
  // expecting.  0=continue, 1=continue but ignore future messages from this
  // device, 2=die, 3=dump core, 4=debugger.
  if (n==BX_LOG_ASK_CHOICE_CONTINUE) {
    if (dlg.GetDontAsk()) n = BX_LOG_ASK_CHOICE_CONTINUE_ALWAYS;
  }
  be->retcode = n;
  wxLogDebug(wxT("you chose %d"), n);
  // This can be called from two different contexts:
  // 1) before sim_thread starts, the default application callback can
  //    call OnLogAsk to display messages.
  // 2) after the sim_thread starts, the sim_thread callback can call
  //    OnLogAsk to display messages
  if (sim_thread)
    sim_thread->SendSyncResponse(be);  // only for case #2
}

void MyFrame::editFloppyConfig(int drive)
{
  FloppyConfigDialog dlg(this, -1);
  dlg.SetTitle(wxString(drive==0? BX_FLOPPY0_NAME : BX_FLOPPY1_NAME, wxConvUTF8));
  bx_list_c *list = (bx_list_c*) SIM->get_param((drive==0)? BXPN_FLOPPYA : BXPN_FLOPPYB);
  dlg.Setup(list);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::editFirstCdrom()
{
  bx_param_c *firstcd;

  if (sim_thread != NULL) {
    firstcd = ((bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_CDROM))->get(0);
  } else {
    firstcd = SIM->get_first_cdrom();
  }
  if (!firstcd) {
    wxMessageBox(wxT("No CDROM drive is enabled.  Use Edit:ATA to set one up."),
                 wxT("No CDROM"), wxOK | wxICON_ERROR, this);
    return;
  }
  ParamDialog dlg(this, -1);
  dlg.SetTitle(wxT("Configure CDROM"));
  dlg.AddParam(firstcd);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::OnEditATA(wxCommandEvent& event)
{
  int id = event.GetId();
  int channel = id - ID_Edit_ATA0;
  char ata_name[10];
  sprintf(ata_name, "ata.%d", channel);
  ParamDialog dlg(this, -1);
  bx_list_c *list = (bx_list_c*) SIM->get_param(ata_name);
  dlg.SetTitle(wxString(list->get_title(), wxConvUTF8));
  dlg.AddParam(list);
  dlg.SetRuntimeFlag(sim_thread != NULL);
  dlg.ShowModal();
}

void MyFrame::OnToolbarClick(wxCommandEvent& event)
{
  wxCommandEvent unusedEvent;

  wxLogDebug(wxT("clicked toolbar thingy"));
  bx_toolbar_buttons which = BX_TOOLBAR_UNDEFINED;
  int id = event.GetId();
  switch (id) {
    case ID_Toolbar_Power:
      if (theFrame->GetSimThread() == NULL) {
        OnStartSim(unusedEvent);
      } else {
        which = BX_TOOLBAR_POWER;
        wxBochsStopSim = false;
      }
      break;
    case ID_Toolbar_Reset: which = BX_TOOLBAR_RESET; break;
    case ID_Toolbar_SaveRestore:
      if (theFrame->GetSimThread() == NULL) {
        OnStateRestore(unusedEvent);
      } else {
        which = BX_TOOLBAR_SAVE_RESTORE;
      }
      break;
    case ID_Edit_FD_0:
      // floppy config dialog box
      editFloppyConfig(0);
      break;
    case ID_Edit_FD_1:
      // floppy config dialog box
      editFloppyConfig(1);
      break;
    case ID_Edit_Cdrom1:
      // cdrom config dialog box (first cd only)
      editFirstCdrom();
      break;
    case ID_Toolbar_Copy: which = BX_TOOLBAR_COPY; break;
    case ID_Toolbar_Paste: which = BX_TOOLBAR_PASTE; break;
    case ID_Toolbar_Snapshot: which = BX_TOOLBAR_SNAPSHOT; break;
    case ID_Toolbar_Mouse_en: thePanel->ToggleMouse(true); break;
    case ID_Toolbar_User: which = BX_TOOLBAR_USER; break;
    default:
      wxLogError(wxT("unknown toolbar id %d"), id);
  }
  if ((which != BX_TOOLBAR_UNDEFINED) && (num_events < MAX_EVENTS)) {
    event_queue[num_events].type = BX_ASYNC_EVT_TOOLBAR;
    event_queue[num_events].u.toolbar.button = which;
    num_events++;
  }
}

void MyFrame::SetToolBarHelp(int id, wxString& text)
{
  bxToolBar->SetToolShortHelp(id, text);
}

//////////////////////////////////////////////////////////////////////
// Simulation Thread
//////////////////////////////////////////////////////////////////////

void *SimThread::Entry(void)
{
  // run all the rest of the Bochs simulator code.  This function will
  // run forever, unless a "kill_bochs_request" is issued.  The shutdown
  // procedure is as follows:
  //   - user selects "Kill Simulation" or GUI decides to kill bochs
  //   - GUI calls sim_thread->Delete()
  //   - sim continues to run until the next time it reaches SIM->periodic().
  //   - SIM->periodic() sends a synchronous tick event to the GUI, which
  //     finally calls TestDestroy() and realizes it needs to stop.  It
  //     sets the sync event return code to -1.  SIM->periodic() notices
  //     the -1 and calls quit_sim, which longjumps to quit_context, which is
  //     right here in SimThread::Entry.
  //   - Entry() exits and the thread stops. Whew.
  wxLogDebug(wxT("in SimThread, starting at bx_continue_after_config_interface"));
  static jmp_buf context;  // this must not go out of scope. maybe static not needed
  if (setjmp(context) == 0) {
    SIM->set_quit_context(&context);
    SIM->begin_simulation(bx_startup_flags.argc, bx_startup_flags.argv);
    wxLogDebug(wxT("in SimThread, SIM->begin_simulation() exited normally"));
  } else {
    wxLogDebug(wxT("in SimThread, SIM->begin_simulation() exited by longjmp"));
  }
  SIM->set_quit_context(NULL);
  // it is possible that the whole interface has already been shut down.
  // If so, we must end immediately.
  // we're in the sim thread, so we must get a gui mutex before calling
  // wxwidgets methods.
  wxLogDebug(wxT("SimThread::Entry: get gui mutex"));
  wxMutexGuiEnter();
  if (!wxBochsClosing) {
    if (!wxBochsStopSim) {
      wxLogDebug(wxT("SimThread::Entry: sim thread ending.  call simStatusChanged"));
      theFrame->simStatusChanged(theFrame->Stop, false);
      BxEvent *be = new BxEvent;
      be->type = BX_ASYNC_EVT_QUIT_SIM;
      SIM->sim_to_ci_event(be);
    }
  } else {
    wxLogMessage(wxT("SimThread::Entry: the gui is waiting for sim to finish.  Now that it has finished, I will close the frame."));
    theFrame->Close(TRUE);
  }
  wxMutexGuiLeave();
  return NULL;
}

void SimThread::OnExit()
{
  // notify the MyFrame that the bochs thread has died.  I can't adjust
  // the sim_thread directly because it's private.
  frame->OnSimThreadExit();
  // don't use this SimThread's callback function anymore.  Use the
  // application default callback.
  SIM->set_notify_callback(&MyApp::DefaultCallback, this);
}

// Event handler function for BxEvents coming from the simulator.
// This function is declared static so that I can get a usable
// function pointer for it.  The function pointer is passed to
// SIM->set_notify_callback so that the siminterface can call this
// function when it needs to contact the gui.  It will always be
// called with a pointer to the SimThread as the first argument, and
// it will be called from the simulator thread, not the GUI thread.
BxEvent *SimThread::SiminterfaceCallback(void *thisptr, BxEvent *event)
{
  SimThread *me = (SimThread *)thisptr;
  // call the normal non-static method now that we know the this pointer.
  return me->SiminterfaceCallback2(event);
}

// callback function for sim thread events.  This is called from
// the sim thread, not the GUI thread.  So any GUI actions must be
// thread safe.  Most events are handled by packaging up the event
// in a wxEvent of some kind, and posting it to the GUI thread for
// processing.
BxEvent *SimThread::SiminterfaceCallback2(BxEvent *event)
{
  // wxLogDebug(wxT("SiminterfaceCallback with event type=%d"), (int)event->type);
  event->retcode = 0;  // default return code
  int async = BX_EVT_IS_ASYNC(event->type);
  if (!async) {
    // for synchronous events, clear away any previous response.  There
        // can only be one synchronous event pending at a time.
    ClearSyncResponse();
        event->retcode = -1;   // default to error
  }

  // tick event must be handled right here in the bochs thread.
  if (event->type == BX_SYNC_EVT_TICK) {
        if (TestDestroy()) {
          // tell simulator to quit
          event->retcode = -1;
        } else {
          event->retcode = 0;
        }
        return event;
  }

  //encapsulate the bxevent in a wxwidgets event
  wxCommandEvent wxevent(wxEVT_COMMAND_MENU_SELECTED, ID_Sim2CI_Event);
  wxevent.SetEventObject((wxEvent *)event);
  if (isSimThread()) {
    IFDBG_EVENT(wxLogDebug(wxT("Sending an event to the window")));
    wxPostEvent(frame, wxevent);
    // if it is an asynchronous event, return immediately.  The event will be
    // freed by the recipient in the GUI thread.
    if (async) return NULL;
    wxLogDebug(wxT("SiminterfaceCallback2: synchronous event; waiting for response"));
    // now wait forever for the GUI to post a response.
    BxEvent *response = NULL;
    while (response == NULL) {
          response = GetSyncResponse();
          if (!response) {
            //wxLogDebug ("no sync response yet, waiting");
            this->Sleep(20);
          }
          // don't get stuck here if the gui is trying to close.
          if (wxBochsClosing) {
            wxLogDebug(wxT("breaking out of sync event wait because gui is closing"));
            event->retcode = -1;
            return event;
          }
    }
    wxASSERT(response != NULL);
    return response;
  } else {
    wxLogDebug(wxT("sim2ci event sent from the GUI thread. calling handler directly"));
    theFrame->OnSim2CIEvent(wxevent);
    return event;
  }
}

void SimThread::ClearSyncResponse()
{
  wxCriticalSectionLocker lock(sim2gui_mailbox_lock);
  if (sim2gui_mailbox != NULL) {
    wxLogDebug(wxT("WARNING: ClearSyncResponse is throwing away an event that was previously in the mailbox"));
  }
  sim2gui_mailbox = NULL;
}

void SimThread::SendSyncResponse(BxEvent *event)
{
  wxCriticalSectionLocker lock(sim2gui_mailbox_lock);
  if (sim2gui_mailbox != NULL) {
    wxLogDebug(wxT("WARNING: SendSyncResponse is throwing away an event that was previously in the mailbox"));
  }
  sim2gui_mailbox = event;
}

BxEvent *SimThread::GetSyncResponse()
{
  wxCriticalSectionLocker lock(sim2gui_mailbox_lock);
  BxEvent *event = sim2gui_mailbox;
  sim2gui_mailbox = NULL;
  return event;
}

///////////////////////////////////////////////////////////////////
// utility
///////////////////////////////////////////////////////////////////
void safeWxStrcpy(char *dest, wxString src, int destlen)
{
  wxString tmp(src);
  strncpy(dest, tmp.mb_str(wxConvUTF8), destlen);
  dest[destlen-1] = 0;
}

#endif /* if BX_WITH_WX */
