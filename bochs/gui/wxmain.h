/////////////////////////////////////////////////////////////////
// $Id: wxmain.h,v 1.4 2002-08-26 15:31:23 bdenney Exp $
/////////////////////////////////////////////////////////////////
// This file defines variables and classes that the wxWindows .cc files 
// share.  It should be included only by wx.cc and wxmain.cc.  

// forward class declaration so that each class can have a pointer to 
// the others.
class MyFrame;
class MyPanel;
class SimThread;

//hack alert; yuck; FIXME
extern MyFrame *theFrame;
extern MyPanel *thePanel;

#define MAX_EVENTS 256
extern unsigned long num_events;
extern BxEvent event_queue[MAX_EVENTS];

// to compile in debug messages, change these defines to x.  To remove them,
// change the defines to return nothing.
#define IFDBG_VGA(x) /* nothing */
//#define IFDBG_VGA(x) x

#define IFDBG_KEY(x) /* nothing */
//#define IFDBG_KEY(x) x


/// the MyPanel methods are defined in wx.cc
class MyPanel: public wxPanel
{
  Boolean fillBxKeyEvent (wxKeyEvent& event, BxKeyEvent& bxev, Boolean release);  // for all platforms
  Boolean fillBxKeyEvent_MSW (wxKeyEvent& event, BxKeyEvent& bxev, Boolean release);
  Boolean fillBxKeyEvent_GTK (wxKeyEvent& event, BxKeyEvent& bxev, Boolean release);
public:
  MyPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel")
      : wxPanel (parent, id, pos, size, style, name)
    { wxLogDebug ("MyPanel constructor"); }
  void OnKeyDown(wxKeyEvent& event);
  void OnKeyUp(wxKeyEvent& event);
  void OnPaint(wxPaintEvent& event);
  void MyRefresh ();
  void ReadConfiguration ();
  void SaveConfiguration ();
private:
  DECLARE_EVENT_TABLE()
};

/// the MyFrame methods are defined in wxmain.cc
class MyFrame: public wxFrame
{
  MyPanel *panel;
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
  void OnConfigRead(wxCommandEvent& event);
  void OnConfigSave(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnStartSim(wxCommandEvent& event);
  void OnPauseResumeSim(wxCommandEvent& event);
  void OnKillSim(wxCommandEvent& event);
  void OnSim2CuiEvent(wxCommandEvent& event);
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

