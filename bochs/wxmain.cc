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
#include "control.h"
#include "gui/siminterface.h"

// just for temporary stuff
#include "hacks.h"

class MyApp: public wxApp
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
MyFrame(const wxString& title, const wxPoint& pos, const wxSize&
size);
void OnQuit(wxCommandEvent& event);
void OnAbout(wxCommandEvent& event);
void OnStartBochs(wxCommandEvent& event);
int StartBochsThread ();
int HandleVgaGuiButton (int which);
private:
BochsThread *bochs_thread;
DECLARE_EVENT_TABLE()
static int SiminterfaceCallback (void *thisptr, int code);
int SiminterfaceCallback2 (int code);
};

enum
{
ID_Quit = 1,
ID_About,
ID_StartBochs
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Quit, MyFrame::OnQuit)
EVT_MENU(ID_About, MyFrame::OnAbout)
EVT_MENU(ID_StartBochs, MyFrame::OnStartBochs)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

static MyFrame *theFrame = NULL;

bool MyApp::OnInit()
{
	siminterface_init ();
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

	// set up the gui
	wxMenu *menuFile = new wxMenu;
	menuFile->Append( ID_StartBochs, "&Start Bochs!" );
	menuFile->Append( ID_About, "&About..." );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, "E&xit" );
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, "&File" );
	SetMenuBar( menuBar );
	CreateStatusBar();
	SetStatusText( "Welcome to wxWindows!" );
	// set up callback for events from Bochs
	SIM->set_notify_callback (&MyFrame::SiminterfaceCallback, this);
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
	StartBochsThread ();
}

/////////////// bochs thread

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
	  Raise();  // bring the control panel to front so that the dialog shows up
	  wxFileDialog dialog(this, "Choose new floppy disk image file", "", "", "*.img", 0);
	  if (dialog.ShowModal() == wxID_OK)
	  {
		wxString info;
		info.Printf(_T("Full file name: %s\n")
					_T("Path: %s\n")
					_T("Name: %s"),
					dialog.GetPath().c_str(),
					dialog.GetDirectory().c_str(),
					dialog.GetFilename().c_str());
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
	  Raise();  // bring the control panel to front so that the dialog shows up
	  wxFileDialog dialog(this, "Choose new floppy disk image file", "", "", "*.img", 0);
	  if (dialog.ShowModal() == wxID_OK)
	  {
		wxString info;
		info.Printf(_T("Full file name: %s\n")
					_T("Path: %s\n")
					_T("Name: %s"),
					dialog.GetPath().c_str(),
					dialog.GetDirectory().c_str(),
					dialog.GetFilename().c_str());
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
	  wxExit ();
	}
  }
  return 0;
}
