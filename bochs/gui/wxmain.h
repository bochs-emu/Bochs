/////////////////////////////////////////////////////////////////
// $Id: wxmain.h,v 1.36 2002-10-25 11:44:37 bdenney Exp $
/////////////////////////////////////////////////////////////////
// This file defines variables and classes that the wxWindows .cc files 
// share.  It should be included only by wx.cc and wxmain.cc.  

// forward class declaration so that each class can have a pointer to 
// the others.
class MyFrame;
class MyPanel;
class SimThread;
class FloppyConfigDialog;
class ParamDialog;

//hack alert; yuck; FIXME
extern MyFrame *theFrame;
extern MyPanel *thePanel;

// wxBochsClosing flag, see comments in wxmain.h
extern bool wxBochsClosing;

#define MAX_EVENTS 256
extern unsigned long num_events;
extern BxEvent event_queue[MAX_EVENTS];

enum
{
  ID_Quit = 1,
  ID_Config_New,
  ID_Config_Read,
  ID_Config_Save,
  ID_Edit_FD_0,
  ID_Edit_FD_1,
  ID_Edit_ATA0,
  ID_Edit_ATA1,
  ID_Edit_ATA2,
  ID_Edit_ATA3,
  ID_Edit_Cdrom,  // for toolbar. FIXME: toolbar can't handle >1 cdrom
  ID_Edit_Boot,
  ID_Edit_Memory,
  ID_Edit_Sound,
  ID_Edit_Cmos,
  ID_Edit_Network,
  ID_Edit_Keyboard,
  ID_Edit_Serial_Parallel,
  ID_Edit_Parallel,
  ID_Edit_LoadHack,
  ID_Edit_Other,
  ID_Simulate_Start,
  ID_Simulate_PauseResume,
  ID_Simulate_Stop,
  ID_Simulate_Speed,
  ID_Debug_ShowCpu,
  ID_Debug_ShowKeyboard,
  ID_Debug_Console,
  ID_Debug_ShowMemory,
  ID_Log_View,
  ID_Log_Prefs,
  ID_Log_PrefsDevice,
  ID_Help_About,
  ID_Sim2CI_Event,
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
  ID_Create,
  // dialog box: HDConfigDialog
  ID_Enable,
  ID_Cylinders,
  ID_Heads,
  ID_SPT,
  ID_Megs,
  ID_ComputeGeometry,
  // dialog box: LogOptions
  ID_Advanced,
  // dialog box: CpuRegistersDialog
  ID_Debug_Continue,
  ID_Debug_Stop,
  ID_Debug_Step,
  ID_Debug_Commit,
  ID_Close,
  ID_Execute,
  ID_DebugCommand,
  // advanced log options
  ID_ApplyDefault,
  // that's all
  ID_LAST_USER_DEFINED
};


// to compile in debug messages, change these defines to x.  To remove them,
// change the defines to return nothing.
#define IFDBG_VGA(x) /* nothing */
//#define IFDBG_VGA(x) x

#define IFDBG_KEY(x) /* nothing */
//#define IFDBG_KEY(x) x

#define IFDBG_MOUSE(x) /* nothing */
//#define IFDBG_MOUSE(x) x

#define IFDBG_EVENT(x) /* nothing */
//#define IFDBG_EVENT(x) x

#define IFDBG_DLG(x) /* nothing */
//#define IFDBG_DLG(x) x


// defined in wxmain.cc
void safeWxStrcpy (char *dest, wxString src, int destlen);

/// the MyPanel methods are defined in wx.cc
class MyPanel: public wxPanel
{
  bx_bool fillBxKeyEvent (wxKeyEvent& event, BxKeyEvent& bxev, bx_bool release);  // for all platforms
  bx_bool fillBxKeyEvent_MSW (wxKeyEvent& event, BxKeyEvent& bxev, bx_bool release);
  bx_bool fillBxKeyEvent_GTK (wxKeyEvent& event, BxKeyEvent& bxev, bx_bool release);
public:
  MyPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel");
  ~MyPanel();
  void OnKeyDown(wxKeyEvent& event);
  void OnKeyUp(wxKeyEvent& event);
  void OnTimer(wxCommandEvent& event);
  void OnPaint(wxPaintEvent& event);
  void OnMouse(wxMouseEvent& event);
  void MyRefresh ();
  static void OnPluginInit ();
  void ToggleMouse (bool fromToolbar);
private:
  wxCursor *blankCursor;
  bool needRefresh;
  wxTimer refreshTimer;
  Bit16s mouseSavedX, mouseSavedY;
  Bit32u centerX, centerY;
  DECLARE_EVENT_TABLE()
};

/// the MyFrame methods are defined in wxmain.cc
class MyFrame: public wxFrame
{
  MyPanel *panel;
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
  ~MyFrame();
  enum StatusChange { Start, Stop, Pause, Resume };
  void simStatusChanged (StatusChange change, bx_bool popupNotify=false);
  void OnConfigNew(wxCommandEvent& event);
  void OnConfigRead(wxCommandEvent& event);
  void OnConfigSave(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnStartSim(wxCommandEvent& event);
  void OnPauseResumeSim(wxCommandEvent& event);
  void OnKillSim(wxCommandEvent& event);
  void OnSim2CIEvent(wxCommandEvent& event);
  void OnLogMsg(BxEvent *logMsgEvent);
  void OnEditBoot(wxCommandEvent& event);
  void OnEditMemory(wxCommandEvent& event);
  void OnEditSound(wxCommandEvent& event);
  void OnEditCmos(wxCommandEvent& event);
  void OnEditNet(wxCommandEvent& event);
  void OnEditKeyboard(wxCommandEvent& event);
  void OnEditSerialParallel(wxCommandEvent& event);
  void OnEditLoadHack(wxCommandEvent& event);
  void OnEditOther(wxCommandEvent& event);
  void OnLogPrefs(wxCommandEvent& event);
  void OnLogPrefsDevice(wxCommandEvent& event);
  void OnOtherEvent(wxCommandEvent& event);
  void OnShowCpu(wxCommandEvent& event);
  void OnShowKeyboard(wxCommandEvent& event);
#if BX_DEBUGGER
  void OnDebugLog(wxCommandEvent& event);
  void DebugBreak ();
  void DebugCommand (wxString string);
  void DebugCommand (const char *cmd);
#endif
  static bool editFloppyValidate (FloppyConfigDialog *dialog);
  void editFloppyConfig (int drive);
  void editATAConfig (int channel);
  void OnToolbarClick(wxCommandEvent& event);
  int HandleAskParam (BxEvent *event);
  int HandleAskParamString (bx_param_string_c *param);

  // called from the sim thread's OnExit() method.
  void OnSimThreadExit ();
  SimThread *GetSimThread () { return sim_thread; }

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
  ParamDialog *showCpu, *showKbd;
#if BX_DEBUGGER
  DebugLogDialog *showDebugLog;
#endif
  void RefreshDialogs ();
  char *debugCommand; // maybe need lock on this
  BxEvent *debugCommandEvent;  // maybe need lock on this
public:
  bool WantRefresh ();

DECLARE_EVENT_TABLE()
};

