/////////////////////////////////////////////////////////////////
// $Id: wxmain.h,v 1.6 2002-08-28 07:54:53 bdenney Exp $
/////////////////////////////////////////////////////////////////
// This file defines variables and classes that the wxWindows .cc files 
// share.  It should be included only by wx.cc and wxmain.cc.  

// forward class declaration so that each class can have a pointer to 
// the others.
class MyFrame;
class MyPanel;
class SimThread;
class FloppyConfigDialog;

//hack alert; yuck; FIXME
extern MyFrame *theFrame;
extern MyPanel *thePanel;

#define MAX_EVENTS 256
extern unsigned long num_events;
extern BxEvent event_queue[MAX_EVENTS];

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
  ID_Sim2Cui_Event,
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
  ID_Toolbar_Mouse_en,
  ID_Toolbar_User,
  // dialog box: LogMsgAskDialog
  ID_Continue,
  ID_Die,
  ID_DumpCore,
  ID_Debugger,
  ID_Help,
  // dialog box: FloppyConfigDialog
  ID_None,
  ID_Physical_A,
  ID_Physical_B,
  ID_Filename,
  ID_FilenameText,
  ID_Browse,
};


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
  enum StatusChange { Start, Stop, Pause, Resume };
  void simStatusChanged (StatusChange change, Boolean popupNotify=false);
  void OnConfigRead(wxCommandEvent& event);
  void OnConfigSave(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnStartSim(wxCommandEvent& event);
  void OnPauseResumeSim(wxCommandEvent& event);
  void OnKillSim(wxCommandEvent& event);
  void OnSim2CuiEvent(wxCommandEvent& event);
  static bool editFloppyValidate (FloppyConfigDialog *dialog);
  void editFloppyConfig (int drive);
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

