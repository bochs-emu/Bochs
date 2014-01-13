////////////////////////////////////////////////////////////////////
// $Id$
////////////////////////////////////////////////////////////////////
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

// wxWidgets dialogs for Bochs

#include <wx/spinctrl.h>
#include <wx/notebook.h>

////////////////////////////////////////////////////////////////////
// text messages used in several places
////////////////////////////////////////////////////////////////////
#define MSG_NO_HELP wxT("No help is available yet.")
#define MSG_NO_HELP_CAPTION wxT("No help")
#define BTNLABEL_HELP wxT("Help")
#define BTNLABEL_CANCEL wxT("Cancel")
#define BTNLABEL_OK wxT("Ok")
#define BTNLABEL_CLOSE wxT("Close")
#define BTNLABEL_CREATE_IMG wxT("Create Image")
#define BTNLABEL_BROWSE wxT("<--Browse")

#if defined(WIN32)
// On win32, apparantly the spinctrl depends on a native control which only
// has a 16bit signed value.  If you try to set the max above 32767, it
// overflows and does stupid things.
#define SPINCTRL_FIX_MAX(x) ((x)>32767 ? 32767 : (x))
#else
#define SPINCTRL_FIX_MAX(x) x
#endif

// utility function prototype
void ChangeStaticText(wxSizer *sizer, wxStaticText *win, wxString newtext);
bool CreateImage(int harddisk, int sectors, const char *filename);
void SetTextCtrl(wxTextCtrl *text, const char *format, int val);
int GetTextCtrlInt(wxTextCtrl *text, bool *valid = NULL, bool complain=false, wxString complaint = wxT("Invalid integer!"));
bool BrowseTextCtrl(wxTextCtrl *text,
    wxString prompt= wxT("Choose a file"),
    long style=wxOPEN);
wxChoice *makeLogOptionChoiceBox(wxWindow *parent, wxWindowID id, int evtype, bool includeNoChange = false);

////////////////////////////////////////////////////////////////////
// LogMsgAskDialog is a modal dialog box that shows the user a
// simulation error message and asks if they want to continue or
// not.  It looks something like this:
//
// +----- PANIC ---------------------------------------------------+
// |                                                               |
// | Context: Hard Drive                                           |
// | Message: could not open hard drive image file '30M.sample'    |
// |                                                               |
// |      [ ] Don't ask about future messages like this            |
// |                                                               |
// |     [Continue]   [Die]  [Dump Core]  [Debugger]   [Help]      |
// +---------------------------------------------------------------+
//
// To use this dialog:
// After constructor, use SetContext, SetMessage, EnableButton to
// determine what will be displayed.  Then call n = ShowModal().  The return
// value tells which button was pressed (button_t types).  Call GetDontAsk()
// to see if they checked "Don't ask about..." or not.
//////////////////////////////////////////////////////////////////////

class LogMsgAskDialog: public wxDialog
{
public:
  enum button_t {
    CONT=0, DIE, DUMP, DEBUG, HELP,
    N_BUTTONS /* number of entries in enum */
  };
#define LOG_MSG_ASK_IDS \
  { ID_Continue, ID_Die, ID_DumpCore, ID_Debugger, wxHELP }
#define LOG_MSG_ASK_NAMES \
  { wxT("Continue"), wxT("Kill Sim"), wxT("Dump Core"), wxT("Debugger"), wxT("Help") }
#define LOG_MSG_DONT_ASK_STRING \
  wxT("Don't ask about future messages like this")
#define LOG_MSG_CONTEXT wxT("Context: ")
#define LOG_MSG_MSG wxT("Message: ")
private:
  wxStaticText *context, *message;
  wxCheckBox *dontAsk;
  bool enabled[N_BUTTONS];
  wxBoxSizer *btnSizer, *vertSizer;
  void Init();  // called automatically by ShowModal()
  void ShowHelp();
public:
  LogMsgAskDialog(wxWindow* parent,
      wxWindowID id,
      const wxString& title);
  void EnableButton(button_t btn, bool en) { enabled[(int)btn] = en; }
  void SetContext(wxString s);
  void SetMessage(wxString s);
  bool GetDontAsk() { return dontAsk->GetValue(); }
  void OnEvent(wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// AdvancedLogOptionsDialog
////////////////////////////////////////////////////////////////////////////
// +---- Advanced event configuration -----------------------+
// |                                                         |
// | Log file is [_____________________________]  [ Browse ] |
// |                                                         |
// | This table determines how Bochs will respond to each    |
// | kind of event coming from a particular source.  For     |
// | example if you are having problems with the keyboard,   |
// | you could ask for debug and info events from the        |
// | keyboard to be reported.                                |
// |                                                         |
// |                        [Use defaults for all devices]   |
// |                                                         |
// | +---------------------------------------------------+-+ |
// | |Device    Debug     Info      Error      Panic     |^| |
// | |--------  --------  -------   --------   --------- ||| |
// | |Keyboard  [ignore]  [ignore]  [report]   [report]  ||| |
// | |VGA       [ignore]  [ignore]  [report]   [report]  ||| |
// | |NE2000    [ignore]  [ignore]  [report]   [report]  ||| |
// | |Sound     [ignore]  [ignore]  [report]   [report]  |v| |
// | +-----------------------------------------------------+ |
// |                                                         |
// |                              [ Help ] [ Cancel ] [ Ok ] |
// +-------------------------------------------------------+-+
//
class AdvancedLogOptionsDialog: public wxDialog
{
private:
#define ADVLOG_OPTS_TITLE wxT("Configure Log Events")
#define ADVLOG_OPTS_LOGFILE wxT("Log file")
#define ADVLOG_OPTS_PROMPT wxT(                                               \
"This table determines how Bochs will respond to each kind of event coming\n" \
"from a particular source.  For example if you are having problems with\n"    \
"the keyboard, you could ask for debug and info events from the keyboard\n"   \
"to be reported.")
#define ADVLOG_OPTS_TYPE_NAMES { wxT("Debug"), wxT("Info"), wxT("Error"), wxT("Panic") }
#define ADVLOG_OPTS_N_TYPES 4
#define ADVLOG_DEFAULTS wxT("Use defaults for all devices")
  void Init();  // called automatically by ShowModal()
  void ShowHelp();
  wxBoxSizer *vertSizer, *logfileSizer, *buttonSizer;
  wxScrolledWindow *scrollWin;
  wxPanel *scrollPanel;
  wxGridSizer *headerSizer, *gridSizer;
  wxTextCtrl *logfile;
  wxButton *applyDefault;
  // 2d array of wxChoice pointers. Each wxChoice* is action[dev][type].
  wxChoice*  **action;
  bool runtime;
public:
  AdvancedLogOptionsDialog(wxWindow* parent, wxWindowID id);
  ~AdvancedLogOptionsDialog();
  void OnEvent(wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void SetLogfile(wxString f) { logfile->SetValue(f); }
  wxString GetLogfile() { return logfile->GetValue(); }
  void CopyParamToGui();
  void CopyGuiToParam();
  void SetAction(int dev, int evtype, int act);
  int GetAction(int dev, int evtype);
  void SetRuntimeFlag(bool val) { runtime = val; }
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// PluginControlDialog
////////////////////////////////////////////////////////////////////////////
class PluginControlDialog: public wxDialog
{
private:
  void Init();  // called automatically by ShowModal()
  void ShowHelp();
  wxBoxSizer *vertSizer, *horzSizer, *listSizer, *editSizer, *buttonSizer;
  wxTextCtrl *plugname;
  wxListBox *pluglist;
  wxButton *btn_load, *btn_unload;
public:
  PluginControlDialog(wxWindow* parent, wxWindowID id);
  ~PluginControlDialog() {}
  void OnEvent(wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// LogViewDialog
////////////////////////////////////////////////////////////////////////////
class LogViewDialog: public wxDialog
{
private:
  wxBoxSizer *mainSizer, *logSizer, *buttonSizer;
  wxTextCtrl *log;
  Bit32u lengthMax;
  Bit32u lengthTolerance;
#define LOG_VIEW_DEFAULT_LENGTH_MAX (400*80)
#define LOG_VIEW_DEFAULT_TOLERANCE (200*80)
  void CheckLogLength();
public:
  LogViewDialog(wxWindow* parent, wxWindowID id);
  ~LogViewDialog() {}
  void Init();
  bool Show(bool val);
  void AppendText(int level, wxString msg);
  void OnEvent(wxCommandEvent& event);
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// ParamDialog is a general purpose dialog box that displays and edits
// any combination of parameters.  It's always made up of a
// wxFlexGridSizer with three columns.  Each parameter takes up one row.
// Column 1 shows the name of the parameter, column 2 shows the value of
// the parameter in some sort of control that can be edited.  Column 3
// is used for anything that needs to appear to the right of the data, for
// example a Browse button on a filename control.  Several buttons including
// Cancel and Ok will appear at the bottom.
//
// This will allow editing of all the miscellaneous parameters which do
// not need to be laid out by hand.
//
// NOTES:
// Right now, there is always one wxFlexGridSizer with three columns
// where the fields go.  It is possible to create a new wxFlexGridSizer
// and make that one the default.  This is used when handling a bx_list_c
// parameter.
////////////////////////////////////////////////////////////////////////////

struct ParamStruct : public wxObject {
  bx_param_c *param;
  int id;
  wxStaticText *label;
  union _u_tag {
    void *ptr;
    wxWindow *window;
    wxChoice *choice;
    wxTextCtrl *text;
    wxSpinCtrl *spin;
    wxCheckBox *checkbox;
    wxStaticBox *staticbox;
    wxNotebook *notebook;
  } u;
  int browseButtonId;  // only for filename params
  wxButton *browseButton;  // only for filename params
  ParamStruct() { param = NULL; u.window = NULL; browseButton = NULL; }
};

// This context structure is used by AddParam to keep track of where the
// next parameter's controls should be added.  When AddParam is called on
// a list of parameters (bx_list_c), it calls itself recursively to add
// the child parameters, which in turn could be lists as well.  When it
// calls itself recursively, it will create a new AddParamContext so that
// the various children can be added in the right place.
struct AddParamContext {
  int depth;
  wxWindow *parent;
  wxBoxSizer *vertSizer;
  wxFlexGridSizer *gridSizer;
};


class ParamDialog: public wxDialog
{
private:
  void ShowHelp();
  int genId();
  bool isShowing;
  int nbuttons;
  bool runtime;
protected:
  wxBoxSizer *mainSizer, *buttonSizer, *infoSizer;
  // hash table that maps the ID of a wxWidgets control (e.g. wxChoice,
  // wxTextCtrl) to the associated ParamStruct object.  Data in the hash table
  // is of ParamStruct*.
  wxHashTable *idHash;
  // map parameter ID onto ParamStruct.
  wxHashTable *paramHash;
  virtual void EnableChanged();
  void EnableChanged(ParamStruct *pstr);
  void EnableParam(int param_id, bool enabled);
  void ProcessDependentList(ParamStruct *pstrChanged, bool enabled);
  bool CopyGuiToParam();
  bool CopyGuiToParam(bx_param_c *param);
  bool isGeneratedId(int id);
public:
  ParamDialog(wxWindow* parent, wxWindowID id);
  virtual ~ParamDialog();
  void OnEvent(wxCommandEvent& event);
  wxButton* AddButton(int id, wxString label);
  virtual void AddDefaultButtons();
  virtual void Init();  // called automatically by ShowModal()
  int ShowModal() {
    Init();
    isShowing = true;
    int ret = wxDialog::ShowModal();
    isShowing = false;
    return ret;
  }
  bool Show(bool val) { isShowing = val; return wxDialog::Show(val); }
  void AddParam(bx_param_c *param, wxFlexGridSizer *sizer, bool plain = false);
  void AddParam(bx_param_c *param, bool plain = false, AddParamContext *context = NULL);
  void AddParamList(const char *nameList[], bx_param_c *base, wxFlexGridSizer *sizer = NULL, bool plain = false);
  virtual void CopyParamToGui();
  bool IsShowing() { return isShowing; }
  void SetRuntimeFlag(bool val) { runtime = val; }
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// FloppyConfigDialog
////////////////////////////////////////////////////////////////////////////
//
// the new FloppyConfigDialog is based on ParamDialog. It allows the user to
// configure the floppy settings and to create a floppy image if necessary.
class FloppyConfigDialog : public ParamDialog
{
private:
  wxButton *createButton;
  ParamStruct *pstrDevice, *pstrPath, *pstrMedia, *pstrStatus, *pstrReadonly;
public:
  FloppyConfigDialog(wxWindow* parent, wxWindowID id);
  void Setup(bx_list_c *list);
  void OnEvent(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// LogOptionsDialog
////////////////////////////////////////////////////////////////////////////
//
// the new LogOptionsDialog is based on ParamDialog. It allows the user to
// configure the log file settings and to decide how Bochs will behave for
// each type of log event.
class LogOptionsDialog : public ParamDialog
{
private:
#define LOG_OPTS_TITLE wxT("Configure Log Events")
#define LOG_OPTS_PROMPT wxT("How should Bochs respond to each type of event?")
#define LOG_OPTS_TYPE_NAMES { wxT("Debug events"), wxT("Info events"), wxT("Error events"), wxT("Panic events") }
#define LOG_OPTS_N_TYPES 4
#define LOG_OPTS_CHOICES { wxT("ignore"), wxT("log"), wxT("ask user"), wxT("end simulation"), wxT("no change") }
#define LOG_OPTS_N_CHOICES_NORMAL 4
#define LOG_OPTS_N_CHOICES 5   // number of choices, including "no change"
#define LOG_OPTS_NO_CHANGE 4   // index of "no change"
#define LOG_OPTS_ADV wxT("For additional control over how each device responds to events, use the menu option \"Log ... By Device\".")
  wxFlexGridSizer *gridSizer;
  wxChoice *action[LOG_OPTS_N_TYPES];
public:
  LogOptionsDialog(wxWindow* parent, wxWindowID id);
  int GetAction(int evtype);
  void SetAction(int evtype, int action);
  DECLARE_EVENT_TABLE()
};


/**************************************************************************
Everything else in here is a comment!

////////////////////////////////////////////////////////////////////////////
// proposed dialogs, not implemented
////////////////////////////////////////////////////////////////////////////

Here are some quick sketches of what different parts of the interface
could look like.  None of these is implemented yet, and everything is
open for debate.  Whoever writes the wxWidgets code for any of these
screens gets several thousand votes!

Idea for large configuration dialog, based on Netscape's Edit:Preferences
dialog box.  Here's a sketch of a dialog with all the components that can be
configured in a list on the left, and the details of the selected component
on the right.  This is a pretty familiar structure that's used in a lot of
applications these days.  In the first sketch, "IDE Interface" is selected on
the left, and the details of the IDE devices are shown on the right.

+-----Configure Bochs-------------------------------------------------------+
|                                                                           |
|  +--------------------+  +-- IDE Controller ---------------------------+  |
|  | |-CPU              |  |                                             |  |
|  | |                  |  | Master device:                              |  |
|  | |-Memory           |  |   [X] Enable Hard Disk 0                    |  |
|  | |                  |  |                                             |  |
|  | |-Video            |  | Slave device (choose one):                  |  |
|  | |                  |  |   [ ] No slave device                       |  |
|  | |-Floppy disks     |  |   [ ] Hard Disk 1                           |  |
|  | | |-Drive 0        |  |   [X] CD-ROM                                |  |
|  | | |-Drive 1        |  |                                             |  |
|  | |                  |  +---------------------------------------------+  |
|  |***IDE controller***|                                                   |
|  | | |-Hard Drive 0   |                                                   |
|  | | |-CD-ROM drive   |                                                   |
|  | |                  |                                                   |
|  | |-Keyboard         |                                                   |
|  | |                  |                                                   |
|  | |-Networking       |                                                   |
|  | |                  |                                                   |
|  | |-Sound            |                                                   |
|  |                    |                                                   |
|  +--------------------+                                                   |
|                                                     [Help] [Cancel] [Ok]  |
+---------------------------------------------------------------------------+

If you click on Hard Drive 0 in the component list (left), then the
whole right panel changes to show the details of the hard drive.

+-----Configure Bochs-------------------------------------------------------+
|                                                                           |
|  +--------------------+   +---- Configure Hard Drive 0 ----------------+  |
|  | |-CPU              |   |                                            |  |
|  | |                  |   |  [X] Enabled                               |  |
|  | |-Memory           |   |                                            |  |
|  | |                  |   +--------------------------------------------+  |
|  | |-Video            |                                                   |
|  | |                  |   +---- Disk Image ----------------------------+  |
|  | |-Floppy disks     |   |                                            |  |
|  | | |-Drive 0        |   |  File name: [___________________] [Browse] |  |
|  | | |-Drive 1        |   |  Geometry: cylinders [____]                |  |
|  | |                  |   |            heads [____]                    |  |
|  | |-IDE controller   |   |            sectors/track [____]            |  |
|  | |***Hard Drive 0***|   |                                            |  |
|  | | |-CD-ROM drive   |   |  Size in Megabytes: 38.2                   |  |
|  | |                  |   |       [Enter size/Compute Geometry]        |  |
|  | |-Keyboard         |   |                                            |  |
|  | |                  |   |                             [Create Image] |  |
|  | |-Networking       |   +--------------------------------------------+  |
|  | |                  |                                                   |
|  | |-Sound            |                                                   |
|  |                    |                                                   |
|  +--------------------+                                                   |
|                                                     [Help] [Cancel] [Ok]  |
+---------------------------------------------------------------------------+

Or if you choose the CD-ROM, you get to edit the settings for it.

+---- Configure Bochs ------------------------------------------------------+
|                                                                           |
|  +--------------------+  +-- CD-ROM Device ----------------------------+  |
|  | |-CPU              |  |                                             |  |
|  | |                  |  |  [ ] Enable Emulated CD-ROM                 |  |
|  | |-Memory           |  |                                             |  |
|  | |                  |  +---------------------------------------------+  |
|  | |-Video            |                                                   |
|  | |                  |  +-- CD-ROM Media -----------------------------+  |
|  | |-Floppy disks     |  |                                             |  |
|  | | |-Drive 0        |  |  Bochs can use a physical CD-ROM drive as   |  |
|  | | |-Drive 1        |  |  the data source, or use an image file.     |  |
|  | |                  |  |                                             |  |
|  | |-IDE controller   |  |   [X]  Ejected                              |  |
|  | | |-Hard Drive 0   |  |   [ ]  Physical CD-ROM drive /dev/cdrom     |  |
|  |*****CD-ROM drive***|  |   [ ]  Disk image: [_____________] [Browse] |  |
|  | |                  |  |                                             |  |
|  | |-Keyboard         |  +---------------------------------------------+  |
|  | |                  |                                                   |
|  | |-Networking       |                                                   |
|  | |                  |                                                   |
|  | |-Sound            |                                                   |
|  |                    |                                                   |
|  +--------------------+                                                   |
|                                                     [Help] [Cancel] [Ok]  |
+---------------------------------------------------------------------------+

////////////////////////////////////////////////////////////////////////////
// ChooseConfigDialog
////////////////////////////////////////////////////////////////////////////
The idea is that you could choose from a set of configurations
(individual bochsrc files, basically).  When you first install
Bochs, it would just have DLX Linux Demo, and Create New.
As you create new configurations and save them, the list
could grow.
+--------------------------------------------------------+
|                                                        |
|   Choose a configuration for the Bochs simulator:      |
|                                                        |
|   +---+                                                |
|   | O |    DLX Linux Demo                              |
|   | | |    Boot 10MB Hard Disk                         |
|   +---+                                                |
|                                                        |
|   +---+                                                |
|   | O |    Redhat Linux Image                          |
|   | | |    Boot 10MB Hard Disk                         |
|   +---+                                                |
|                                                        |
|   +---+                                                |
|   | O |    FreeDOS                                     |
|   | | |    Boot 40MB Hard Disk                         |
|   +---+                                                |
|                                                        |
|    ??      Create new configuration                    |
|                                                        |
+--------------------------------------------------------+

////////////////////////////////////////////////////////////////////////////
// KeymappingDialog
////////////////////////////////////////////////////////////////////////////
more ambitious: create a button for each key on a standard keyboard, so that
you can view/edit/load/save key mappings, produce any combination of keys
(esp. ones that your OS or window manager won't allow)

////////////////////////////////////////////////////////////////////////////
// new AdvancedLogOptionsDialog
////////////////////////////////////////////////////////////////////////////
Try #2 for the advanced event configuration dialog.
It shows the selection of the default actions again
at the top, with some explanation.  Then at bottom, you
can select a device in the list box and edit the settings
for that device individually.  It would be possible to
allow selection of multiple devices and then edit several
devices at once.

+---- Advanced event configuration ---------------------+-+
|                                                         |
|                    +--- Default Actions -------------+  |
| First choose the   |                                 |  |
| default actions    |  Debug events: [ignore]         |  |
| that apply to all  |   Info events: [ignore]         |  |
| event sources.     |  Error events: [report]         |  |
|                    |  Panic events: [ask   ]         |  |
|                    |                                 |  |
|                    |          [Copy to All Devices]  |  |
|                    +---------------------------------+  |
|                                                         |
| Then, if you want you can edit the actions for          |
| individual devices.  For example if you are having      |
| problems with the keyboard, you could ask for debug     |
| and info events from the keyboard to be reported.       |
|                                                         |
| Select Device:                                          |
| +-------------+-+  +--- Actions for VGA -------------+  |
| | CPU         |^|  |                                 |  |
| | Interrupts  |||  |  Debug events: [ignore]         |  |
| |*VGA*********|||  |   Info events: [ignore]         |  |
| | Keyboard    |||  |  Error events: [report]         |  |
| | Mouse       |||  |  Panic events: [ask   ]         |  |
| | Floppy Disk |v|  |                                 |  |
| +-------------+-+  +---------------------------------+  |
|                                                         |
|                              [ Help ] [ Cancel ] [ Ok ] |
+---------------------------------------------------------+

+---- Configure events -------------------------------------+
|  __________    ____________                               |
| | Default  \  | Per Device \                              |
| |           \------------------------------------------+  |
| |                                                      |  |
| | Event log file [_______________________] [Browse]    |  |
| |                                                      |  |
| | How should Bochs respond to each type of event?      |  |
| |                                                      |  |
| |            Debug events: [ignore]                    |  |
| |             Info events: [ignore]                    |  |
| |            Error events: [report]                    |  |
| |            Panic events: [ask   ]                    |  |
| |                                                      |  |
| | For additional control over how each device responds |  |
| | to events, click the "Per Device" tab above.         |  |
| |                                                      |  |
| +------------------------------------------------------+  |
|                                [ Help ] [ Cancel ] [ Ok ] |
+-----------------------------------------------------------+

+---- Configure events -------------------------------------+
|  __________    ____________                               |
| | Default  \  | Per Device \                              |
| +-------------|             \--------------------------+  |
| |                                                      |  |
| | This table determines how Bochs will respond to each |  |
| | kind of event coming from a particular source.  For  |  |
| | example if you are having problems with the keyboard,|  |
| | you could ask for debug and info events from the     |  |
| | keyboard to be reported.                             |  |
| |                                                      |  |
| | +------------------------------------------------+-+ |  |
| | |Device   Debug    Info     Error      Panic     |^| |  |
| | |-------- -------- -------  --------   --------- ||| |  |
| | |Keyboard [ignore] [ignore] [report]   [report]  ||| |  |
| | |VGA      [ignore] [ignore] [report]   [report]  ||| |  |
| | |NE2000   [ignore] [ignore] [report]   [report]  ||| |  |
| | |Sound    [ignore] [ignore] [report]   [report]  |v| |  |
| | +--------------------------------------------------+ |  |
| |                      [Set defaults for all devices]  |  |
| +------------------------------------------------------+  |
|                                [ Help ] [ Cancel ] [ Ok ] |
+-----------------------------------------------------------+

*****************************************************************/
