/////////////////////////////////////////////////////////////////
//
// gui/wx.h
// $Id: wx.h,v 1.1.2.3 2001-06-28 04:08:56 bdenney Exp $
//
// GUI Control Panel for Bochs, using wxWindows toolkit.
//
/////////////////////////////////////////////////////////////////

enum
{
  ID_Quit = 1,
  ID_StartBochs,
  ID_ApplyAction,
  ID_RevertAction,
  ID_ReadConfig,
  ID_WriteConfig,
  ID_WriteConfigAs,
  ID_Check1,
  ID_Choice1,
  ID_Combo1,
  ID_Text1,
  ID_LAST_STATIC_ASSIGNED_ID
};

class BochsApp: public wxApp
{
  virtual bool OnInit();
};

class BochsThread: public wxThread 
{
public:
  virtual ExitCode Entry ();
};

class MyPanel;

class MyFrame: public wxFrame
{
private:
  DECLARE_EVENT_TABLE()
  MyPanel *panel;
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  BochsThread *bochsThread;
  void StartBochsThread ();
  void HandleEvent(wxCommandEvent& event);
  int ReadConfiguration (int ask_name);
  int WriteConfiguration (int ask_name);
  static int ProcessBochsEvent (void *class_ptr, int id);
  int ProcessBochsEvent2 (int id);
};

class MyPanel: public wxScrolledWindow
{
private:
  MyFrame *frame;
  wxStaticText *text1;
  void buildParamList (int x, int y);
public:
  MyPanel () {};
  MyPanel (MyFrame *frame, wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
  ~MyPanel();
  void HandleEvent(wxCommandEvent& event);
  void forAllEditors (int evt_id, wxHashTable &);

  DECLARE_EVENT_TABLE()
};

class BochsEventHandler: public wxEvtHandler {
public:
  BochsEventHandler ();
  void OnEvent (wxCommandEvent& event);
private:
  DECLARE_EVENT_TABLE()
};

class ParamEditor: public wxPanel {
  bx_param_c *param;
  DECLARE_EVENT_TABLE()
  void OnEvent (wxCommandEvent& event);
public:
  ParamEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
  static ParamEditor *createEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
  bx_param_c *get_param ();
  virtual void Action (int evt_id) = 0;
};


class ParamEditorBool : public ParamEditor {
  wxCheckBox *component;
public:
  ParamEditorBool (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
  virtual void Action (int evt_id);
};

class ParamEditorNum : public ParamEditor {
  wxTextCtrl *component;
public:
  ParamEditorNum (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
  virtual void Action (int evt_id);
};

class ParamEditorString : public ParamEditor {
  wxTextCtrl *component;
public:
  ParamEditorString (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
  virtual void Action (int evt_id);
};

class ParamEditorEnum : public ParamEditor {
  wxChoice *component;
  int n_choices;
  wxString *choices;
public:
  ParamEditorEnum (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
  virtual void Action (int evt_id);
};

class ParamEditorList : public ParamEditor {
  wxControl *component;
public:
  ParamEditorList (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y);
  virtual void Action (int evt_id);
};

