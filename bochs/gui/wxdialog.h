////////////////////////////////////////////////////////////////////
// $Id: wxdialog.h,v 1.36 2002-09-20 17:53:14 bdenney Exp $
////////////////////////////////////////////////////////////////////
//
// wxWindows dialogs for Bochs

#include <wx/spinctrl.h>

////////////////////////////////////////////////////////////////////
// text messages used in several places
////////////////////////////////////////////////////////////////////
#define MSG_NO_HELP "No help is available yet."
#define MSG_NO_HELP_CAPTION "No help"
#define MSG_ENABLED "Enabled"
#define BTNLABEL_HELP "Help"
#define BTNLABEL_CANCEL "Cancel"
#define BTNLABEL_OK "Ok"
#define BTNLABEL_CREATE_IMG "Create Image"
#define BTNLABEL_ADVANCED "Advanced"
#define BTNLABEL_BROWSE "<--Browse"
#define BTNLABEL_DEBUG_CONTINUE "Continue"
#define BTNLABEL_DEBUG_STOP "Stop"
#define BTNLABEL_DEBUG_STEP "Step"
#define BTNLABEL_DEBUG_COMMIT "Commit"
#define BTNLABEL_CLOSE "Close"
#define BTNLABEL_EXECUTE "Execute"

#if defined(WIN32)
// On win32, apparantly the spinctrl depends on a native control which only
// has a 16bit signed value.  If you try to set the max above 32767, it
// overflows and does stupid things.
#define SPINCTRL_FIX_MAX(x) ((x)>32767 ? 32767 : (x))
#else
#define SPINCTRL_FIX_MAX(x) x
#endif

// utility function prototype
void ChangeStaticText (wxSizer *sizer, wxStaticText *win, wxString newtext);
bool CreateImage (int harddisk, int sectors, const char *filename);
void SetTextCtrl (wxTextCtrl *text, const char *format, int val);
int GetTextCtrlInt (wxTextCtrl *text, bool *valid = NULL, bool complain=false, wxString complaint = "Invalid integer!");
bool BrowseTextCtrl (wxTextCtrl *text,
    wxString prompt="Choose a file",
    long style=wxOPEN);
wxChoice *makeLogOptionChoiceBox (wxWindow *parent, wxWindowID id, int evtype, bool includeNoChange = false);

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
  { "Continue", "Kill Sim", "Dump Core", "Debugger", "Help" }
#define LOG_MSG_DONT_ASK_STRING \
  "Don't ask about future messages like this"
#define LOG_MSG_CONTEXT "Context: %s"
#define LOG_MSG_MSG "Message: %s"
private:
  wxStaticText *context, *message;
  wxCheckBox *dontAsk;
  bool enabled[N_BUTTONS];
  wxBoxSizer *btnSizer, *vertSizer;
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
public:
  LogMsgAskDialog(wxWindow* parent,
      wxWindowID id,
      const wxString& title);
  void EnableButton (button_t btn, bool en) { enabled[(int)btn] = en; }
  void SetContext (wxString s);
  void SetMessage (wxString s);
  bool GetDontAsk () { return dontAsk->GetValue (); }
  void OnEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////
// FloppyConfigDialog is a modal dialog box that asks the user
// what physical device or disk image should be used for emulation.
//
// +-----Configure Floppy Drive A----------------------------------+
// |                                                               |
// | Bochs can use a real floppy drive as Disk A, or use an        |
// | image file.                                                   |
// |                                                               |
// |    [ ]  None/Disabled                                         |
// |    [X]  Physical floppy drive A:                              |
// |    [ ]  Physical floppy drive B:                              |
// |    [ ]  Disk image: [_____________________________] [Browse]  |
// |                                                               |
// | What is the capacity of this disk? [1.44 MB]                  |
// |                                                               |
// | Hint: To create a disk image, choose the name and capacity    |
// | above, then click Ok.                                         |
// |                                                               |
// |                   [ Help ] [ Cancel ] [ Create Image ] [ Ok ] |
// +---------------------------------------------------------------+
// To use this dialog:
// After constructor, use AddRadio () to add radio buttons, SetFilename()
// to fill in the disk image filename, SetCapacity() to set the capacity. 
// Then call ShowModal() to display it.  Return value is wxID_OK or
// wxID_CANCEL.  If you set a validation function, then it will be called when
// ok is pressed, and will get a chance to veto the "Ok" if it returns false.
// After ShowModal() returns, use GetFilename and GetCapacity to see what the
// user did.  If the validation function sets parameters, this may be
// unnecessary.
//
// Volker reminded me that I wasn't paying much attention to
// the distinction between configuring the device (pre-boot) and 
// configuring the media which can be done anytime.  Here's a proposal
// to fix that...  -Bryce
// +-----Configure Floppy Drive A----------------------------------+
// |                                                               |
// | +-- Device -----------------------------------------------+   |
// | |                                                         |   |
// | |  [ ] Enable Emulated Drive A                            |   |
// | |                                                         |   |
// | |  Drive capacity [1.44 MB]                               |   |
// | |                                                         |   |
// | +---------------------------------------------------------+   |
// |                                                               |
// | +-- Media: Where does the data come from? ----------------+   |
// | |                                                         |   |
// | | Bochs can use a physical floppy drive as the data       |   |
// | | source, or use an image file.                           |   |
// | |                                                         |   |
// | |  [X]  Physical floppy drive A:                          |   |
// | |  [ ]  Physical floppy drive B:                          |   |
// | |  [ ]  Disk image: [_________________________] [Browse]  |   |
// | |                                                         |   |
// | | Media size [1.44 MB]                                    |   |
// | |                                                         |   |
// | | Hint: To create a disk image, choose the name and       |   |
// | | capacity above, then click Ok.                          |   |
// | |                                        [ Create Image ] |   |
// | +---------------------------------------------------------+   |
// |                                                               |
// |                                    [ Help ] [ Cancel ] [ Ok ] |
// +---------------------------------------------------------------+
//////////////////////////////////////////////////////////////////////

class FloppyConfigDialog: public wxDialog
{
public:
#define FLOPPY_CONFIG_TITLE "Configure %s"
#define FLOPPY_CONFIG_INSTRS "Select the device or image to use when simulating %s."
#define FLOPPY_CONFIG_CAP "What is the capacity of this disk?"
#define FLOPPY_CONFIG_HINT "To create a disk image, choose the file name and capacity, then click on \"Create Image\"."
#define FLOPPY_CONFIG_DISKIMG "Disk image: "
private:
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
  wxStaticText *instr;
#define FLOPPY_MAX_RBTNS 4
  wxRadioButton *rbtn[FLOPPY_MAX_RBTNS];
  wxString equivalentFilename[FLOPPY_MAX_RBTNS];
  int n_rbtns;
  wxRadioButton *diskImageRadioBtn;
  wxTextCtrl *filename;
  wxChoice *capacity;
  wxBoxSizer *vertSizer, *radioSizer, *diskImageSizer, *capacitySizer, *buttonSizer;
  typedef bool (*validateFunc_t)(FloppyConfigDialog *dialog);
  validateFunc_t validate;
public:
  FloppyConfigDialog(wxWindow* parent, wxWindowID id);
  void OnEvent (wxCommandEvent& event);
  void OnTextEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void SetRadio (int val);
  void SetFilename (wxString f);
  // Use char* instead of wxString because the array we use is already
  // expressed as a char *[].
  void SetCapacityChoices (int n, char *choices[]);
  void SetCapacity (int cap) { capacity->SetSelection (cap); }
  int GetRadio ();
  int GetCapacity () { return capacity->GetSelection (); }
  wxString GetFilename ();
  void SetDriveName (wxString name);
  void SetValidateFunc (validateFunc_t v) { validate = v; }
  void AddRadio (const wxString& description, const wxString& filename);
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////
// HDConfigDialog is a modal dialog box that asks the user
// what physical device or disk image should be used for emulation.
//
// +-----Configure Hard Disk-------------------------------------------+
// |                                                                   |
// |  [ ] Enable                                                       |
// |                                                                   |
// |  Disk image: [______________________________]  [Browse]           |
// |  Geometry:  cylinders [____]  heads [____]  sectors/track [____]  |
// |  Size in Megabytes: 38.2    [Enter size/Compute Geometry]         |
// |                                                                   |
// |                       [ Help ] [ Cancel ] [ Create image ] [ Ok ] |
// +-------------------------------------------------------------------+
// 
// To use this dialog:
// After constructor, use SetFilename(), SetGeomRange(), SetGeom() to fill in
// the fields.  Note that SetGeomRange() should be called before SetGeom()
// or else the text field may not accept the SetGeom() value because of its
// default min/max setting.  Call ShowModal to display.  Return value is 0=ok
// or -1=cancel.  Use GetFilename() and GetGeom() to retrieve values.
//////////////////////////////////////////////////////////////////////

class HDConfigDialog: public wxDialog
{
public:
#define HD_CONFIG_TITLE "Configure %s"
#define HD_CONFIG_DISKIMG "Disk image: "
private:
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
  wxBoxSizer *vertSizer, *hsizer[3], *buttonSizer;
  wxCheckBox *enable;
  wxTextCtrl *filename;
  wxSpinCtrl *geom[3];
  wxStaticText *megs;
  wxButton *computeGeom;
  enum geomfields_t { CYL, HEADS, SPT };
#define HD_CONFIG_GEOM_NAMES \
  { "Geometry: cylinders", " heads ", " sectors/track " }
#define HD_CONFIG_MEGS "Size in Megabytes: %.1f"
#define HD_CONFIG_COMPUTE_TEXT "<-- Enter Size/Compute Geometry"
#define HD_CONFIG_COMPUTE_INSTR "Enter size of the hard disk image in megabytes.  Between 1 and 32255 please!"
#define HD_CONFIG_COMPUTE_PROMPT "Size in megs: "
#define HD_CONFIG_COMPUTE_CAPTION "Choose Disk Size"
public:
  HDConfigDialog(wxWindow* parent, wxWindowID id);
  void OnEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void SetFilename (wxString f) { filename->SetValue (f); }
  wxString GetFilename () { return filename->GetValue(); }
  void SetDriveName (wxString n);
  void SetGeom (int n, int value);
  int GetGeom (int n) { return geom[n]->GetValue (); }
  void SetGeomRange (int n, int min, int max);
  float UpdateMegs ();
  void EnableChanged ();
  void SetEnable (bool val) { enable->SetValue (val); EnableChanged (); }
  bool GetEnable () { return enable->GetValue (); }
  void EnterSize ();
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////
// CdromConfigDialog is a modal dialog box that asks the user
// what physical device or disk image should be used for cdrom 
// emulation.
//
// +-----Configure CDROM-------------------------------------------+
// |                                                               |
// | +-- Device -----------------------------------------------+   |
// | |                                                         |   |
// | |  [ ] Enable Emulated CD-ROM                             |   |
// | |                                                         |   |
// | +---------------------------------------------------------+   |
// |                                                               |
// | +-- Media: Where does the data come from? ----------------+   |
// | |                                                         |   |
// | | Bochs can use a physical CD-ROM drive as the data       |   |
// | | source, or use an image file.                           |   |
// | |                                                         |   |
// | |  [X]  Ejected                                           |   |
// | |  [ ]  Physical CD-ROM drive /dev/cdrom                  |   |
// | |  [ ]  Disk image: [_________________________] [Browse]  |   |
// | |                                                         |   |
// | |                                        [ Create Image ] |   |
// | +---------------------------------------------------------+   |
// |                                                               |
// |                                    [ Help ] [ Cancel ] [ Ok ] |
// +---------------------------------------------------------------+
//
// To use this dialog:
// After constructor, use SetEnabled(), SetFilename() to fill in the
// disk image filename, AddRadio() to add radio buttons (the disk
// image file radio button will be added automatically).  Then call
// ShowModal() to display it.  Return value is wxID_OK or wxID_CANCEL.
// After ShowModal() returns, use GetFilename() and
// GetEnabled().

class CdromConfigDialog: public wxDialog
{
public:
#define CDROM_CONFIG_TITLE "Configure %s"
#define CDROM_CONFIG_DISKIMG "Use disk image: "
// prompt disabled because I can't figure out what text would make
// the most sense here.  If one of the answers is "Ejected" then what
// is the question?
//#define CDROM_CONFIG_PROMPT "Where should the emulated CD-ROM find its data?"
private:
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
  wxBoxSizer *vertSizer, *fileSizer, *buttonSizer;
  wxStaticBoxSizer *dBoxSizer, *mBoxSizer;
  //wxStaticText *prompt;
  wxCheckBox *enable;
  wxTextCtrl *filename;
  wxRadioButton *diskImageRadioBtn;
#define CDROM_MAX_RBTNS 2
  wxRadioButton *rbtn[CDROM_MAX_RBTNS];
  wxString equivalentFilename[CDROM_MAX_RBTNS];
  int n_rbtns;
public:
  CdromConfigDialog(wxWindow* parent, wxWindowID id);
  void OnEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void SetFilename (wxString f);
  wxString GetFilename ();
  void SetDriveName (wxString f);
  void EnableChanged ();
  void SetEnable (bool val) { enable->SetValue (val); EnableChanged (); }
  bool GetEnable () { return enable->GetValue (); }
  void AddRadio (const wxString& descr, const wxString& path);
  // rbtn[0] will always be the "ejected" button
  void SetEjected (bool val) { if (val) rbtn[0]->SetValue (TRUE); }
  bool GetEjected () { return rbtn[0]->GetValue (); }
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// ConfigNetworkDialog allows the user to change the settings for 
// the emulated NE2000 network card.
////////////////////////////////////////////////////////////////////////////
// +--- Configure Networking --------------------------------------+
// |                                                               |
// |  Bochs can emulate an NE2000-compatible network card.  Would  |
// |  you like to enable it?                                       |
// |                                                               |
// |      Enable networking?  [X]                                  |
// |                                                               |
// |      NE2000 I/O address: [ 0x280 ]                            |
// |                     IRQ: [   9   ]                            |
// |             MAC address: [ b0:c4:00:00:00:00 ]                |
// |    Connection to the OS: [ Linux Packet Filter ]              |
// |     Physical NIC to use: [ eth0 ]                             |
// |            Setup script: [_________________]                  |
// |                                                               |
// |                                    [ Help ] [ Cancel ] [ Ok ] |
// +---------------------------------------------------------------+
// To use this dialog:
// After constructor, use AddConn() to add values to the choice box 
// called "Connection to the OS".  Then use SetEnable, SetIO, SetIrq, SetMac,
// SetConn, SetNic, and SetDebug to fill in the current values.  Then call
// ShowModal(), which will return wxID_OK or wxID_CANCEL.  Then use the Get*
// methods to retrieve the values that were chosen.
class NetConfigDialog: public wxDialog
{
private:
#define NET_CONFIG_TITLE "Configure Networking"
#define NET_CONFIG_PROMPT "Bochs can emulate an NE2000-compatible network card.  Would you like to enable it?"
#define NET_CONFIG_EN "Enable networking?"
#define NET_CONFIG_IO "I/O address (hex): "
#define NET_CONFIG_IRQ "IRQ: "
#define NET_CONFIG_MAC "MAC address: "
#define NET_CONFIG_CONN "Connection to OS: "
#define NET_CONFIG_PHYS "Physical NIC to use: "
#define NET_CONFIG_SCRIPT "Setup script: "
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
  wxBoxSizer *mainSizer, *vertSizer, *buttonSizer;
  wxCheckBox *enable;
  wxTextCtrl *io, *mac, *phys, *script;
  wxSpinCtrl *irq;
  wxChoice *conn;
  int n_conn_choices;
  void EnableChanged ();
public:
  NetConfigDialog(wxWindow* parent, wxWindowID id);
  void OnEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void SetEnable (bool en) { enable->SetValue (en); EnableChanged (); }
  bool GetEnable () { return enable->GetValue (); }
  void SetIO (int addr) { SetTextCtrl (io, "0x%03x", addr); }
  int GetIO ();
  void SetIrq (int addr) { irq->SetValue (addr); }
  int GetIrq () { return irq->GetValue (); }
  void SetMac (unsigned char addr[6]);
  bool GetMac (unsigned char addr[6]);
  void SetConn(const char *realname);
  int GetConn () { return conn->GetSelection (); }
  void *GetConnData () { return conn->GetClientData (conn->GetSelection ()); }
  void AddConn (wxString niceName, char *realName);
  void SetPhys (wxString s) { phys->SetValue (s); }
  wxString GetPhys () { return phys->GetValue (); }
  void SetScript (wxString s) { script->SetValue (s); }
  wxString GetScript () { return script->GetValue (); }
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// ConfigMemoryDialog
////////////////////////////////////////////////////////////////////////////
//
//  +--- Configure Memory ----------------------------------------------+
//  |                                                                   |
//  | +--- Standard Options ------------------------------------------+ |
//  | |                                                               | |
//  | |     Memory size (megabytes): [_____]                          | |
//  | |              ROM BIOS image: [________________] [Browse]      | |
//  | |            ROM BIOS address: [______]                         | |
//  | |              VGA BIOS image: [________________] [Browse]      | |
//  | |            VGA BIOS address: 0xc0000                          | |
//  | |                                                               | |
//  | +---------------------------------------------------------------+ |
//  |                                                                   |
//  | +--- Optional ROM images ---------------------------------------+ |
//  | |                                                               | |
//  | |   Optional ROM image #1: [________________] [Browse]          | |
//  | |                 Address: [______]                             | |
//  | |                                                               | |
//  | |   Optional ROM image #2: [________________] [Browse]          | |
//  | |                 Address: [______]                             | |
//  | |                                                               | |
//  | |   Optional ROM image #3: [________________] [Browse]          | |
//  | |                 Address: [______]                             | |
//  | |                                                               | |
//  | |   Optional ROM image #4: [________________] [Browse]          | |
//  | |                 Address: [______]                             | |
//  | |                                                               | |
//  | +---------------------------------------------------------------+ |
//  |                                        [ Help ] [ Cancel ] [ Ok ] |
//  +-------------------------------------------------------------------+
//
// To use this dialog:
// After constructor, use SetSize(), SetBios(), SetBiosAddr(), SetVgaBios(),
// SetRom(), SetRomAddr() to set the initial values.  Then call ShowModal()
// which will return wxID_OK or wxID_CANCEL.  Use the Get* equivalent methods
// to find out the value from each field.
class ConfigMemoryDialog: public wxDialog
{
private:
#define CONFIG_MEMORY_TITLE "Configure Memory"
#define CONFIG_MEMORY_BOX1_TITLE "Standard Options"
#define CONFIG_MEMORY_BOX2_TITLE "Optional ROM Images"
#define CONFIG_MEMORY_BOX1_LABELS { \
  "Memory size (megabytes):", \
    "ROM BIOS image:", \
    "ROM BIOS address:", \
    "VGA BIOS image:", \
    "VGA BIOS address:", \
    "0xC0000" }
#define CONFIG_MEMORY_BOX2_LABELS { \
  "Optional ROM image #1:", "Address:",  \
  "Optional ROM image #2:", "Address:",  \
  "Optional ROM image #3:", "Address:",  \
  "Optional ROM image #4:", "Address:" \
  }
#define CONFIG_MEMORY_N_ROMS 4
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
  wxBoxSizer *mainSizer, *buttonSizer;
  wxStaticBoxSizer *box1sizer, *box2sizer;
  wxFlexGridSizer *box1gridSizer, *box2gridSizer;
  wxSpinCtrl *megs;
  wxTextCtrl *biosImage, *biosAddr, *vgabiosImage;
  wxTextCtrl *rom[CONFIG_MEMORY_N_ROMS], *romAddr[CONFIG_MEMORY_N_ROMS];
#define CONFIG_MEMORY_N_BROWSES 6
  wxButton *browseBtn[CONFIG_MEMORY_N_BROWSES];
public:
  ConfigMemoryDialog(wxWindow* parent, wxWindowID id);
  void OnEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void SetSize (int val) { megs->SetValue (val); }
  void SetBios (wxString filename) { biosImage->SetValue (filename); }
  void SetVgaBios (wxString filename) { vgabiosImage->SetValue (filename); }
  void SetRom (int n, wxString filename) { rom[n]->SetValue (filename); }
  void SetBiosAddr (int addr) { SetTextCtrl (biosAddr, "0x%05X", addr); }
  void SetRomAddr (int n, int addr) { SetTextCtrl (romAddr[n], "0x%05X", addr); }
  int GetSize () { return megs->GetValue (); }
  wxString GetBios () { return biosImage->GetValue (); }
  wxString GetVgaBios () { return vgabiosImage->GetValue (); }
  wxString GetRom (int n) { return rom[n]->GetValue (); }
  int GetBiosAddr () { return GetTextCtrlInt (biosAddr); }
  int GetRomAddr (int n) { return GetTextCtrlInt (romAddr[n]); }

DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// LogOptionsDialog
////////////////////////////////////////////////////////////////////////////
// LogOptionsDialog allows the user to decide how Bochs will
// behave for each type of log event.
//
// +---- Configure events -----------------------------------+
// |                                                         |
// | Log file is [_____________________________]  [ Browse ] |
// |                                                         |
// | How should Bochs respond to each type of event?         |
// |                                                         |
// |            Debug events: [ignore]                       |
// |             Info events: [ignore]                       |
// |            Error events: [report]                       |
// |            Panic events: [ask   ]                       |
// |                                                         |
// | For additional control over how each device responds    |
// | to events, press the "Advanced" button.                 |
// |                                                         |
// |                [ Advanced ]  [ Help ] [ Cancel ] [ Ok ] |
// +---------------------------------------------------------+
// To use this dialog:
// After constructor, call SetAction(eventtype, action) to set initial
// value for each choice field.  The eventtype is 0 to LOG_OPTS_N_TYPES-1,
// representing the types listed in LOG_OPTS_TYPE_NAMES.  The choice field is 0
// to LOG_OPTS_N_CHOICES-1, representing the actions listed in
// LOG_OPTS_CHOICES.  Then call ShowModal(), which will return wxID_OK or
// wxID_CANCEL.  Afterward, the GetAction(eventtype) method will tell what was
// selected in each choice box.  
class LogOptionsDialog: public wxDialog
{
private:
#define LOG_OPTS_TITLE "Configure Log Events"
#define LOG_OPTS_LOGFILE "Log file is "
#define LOG_OPTS_PROMPT "How should Bochs respond to each type of event?"
#define LOG_OPTS_TYPE_NAMES { "Debug events: ", "Info events: ", "Error events: ", "Panic events: " }
#define LOG_OPTS_N_TYPES 4
#define LOG_OPTS_CHOICES { "ignore", "log", "ask user", "end simulation", "no change" }
#define LOG_OPTS_N_CHOICES_NORMAL 4
#define LOG_OPTS_N_CHOICES 5   // number of choices, including "no change"
#define LOG_OPTS_NO_CHANGE 4   // index of "no change"
// normally all choices are available for all event types. The exclude 
// expression allows some choices to be eliminated if they don't make any 
// sense.  For example, it would be stupid to ignore a panic.
#define LOG_OPTS_EXCLUDE(type,choice)  ( \
   /* can't die or ask, on debug or info events */   \
   (type <= 1 && (choice==2 || choice==3)) \
   /* can't ignore panics or errors */ \
   || (type >= 2 && choice==0) \
   )
#define LOG_OPTS_ADV "For additional control over how each device responds to events, press the \"Advanced\" button."
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
  wxBoxSizer *vertSizer, *logfileSizer, *buttonSizer;
  wxFlexGridSizer *gridSizer;
  wxButton *applyDefault;
  wxTextCtrl *logfile;
  wxChoice *action[LOG_OPTS_N_TYPES];
public:
  LogOptionsDialog(wxWindow* parent, wxWindowID id);
  void OnEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  int GetAction (int evtype);
  void SetAction (int evtype, int action);
  void SetLogfile (wxString f) { logfile->SetValue (f); }
  wxString GetLogfile () { return logfile->GetValue (); }
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
#define ADVLOG_OPTS_TITLE "Configure Log Events"
#define ADVLOG_OPTS_LOGFILE LOG_OPTS_LOGFILE
#define ADVLOG_OPTS_PROMPT                                                    \
"This table determines how Bochs will respond to each kind of event coming\n" \
"from a particular source.  For example if you are having problems with\n"    \
"the keyboard, you could ask for debug and info events from the keyboard\n"   \
"to be reported."
#define ADVLOG_OPTS_TYPE_NAMES { "Debug", "Info", "Error", "Panic" }
#define ADVLOG_OPTS_N_TYPES 4
#define ADVLOG_DEFAULTS "Use defaults for all devices"
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
  wxBoxSizer *vertSizer, *logfileSizer, *buttonSizer;
  wxScrolledWindow *scrollWin;
  wxPanel *scrollPanel;
  wxGridSizer *gridSizer;
  wxTextCtrl *logfile;
  wxButton *applyDefault;
  // 2d array of wxChoice pointers. Each wxChoice* is action[dev][type].
  wxChoice*  **action;
public:
  AdvancedLogOptionsDialog(wxWindow* parent, wxWindowID id);
  ~AdvancedLogOptionsDialog();
  void OnEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void SetLogfile (wxString f) { logfile->SetValue (f); }
  wxString GetLogfile () { return logfile->GetValue (); }
  void CopyParamToGui ();
  void CopyGuiToParam ();
  void SetAction (int dev, int evtype, int act);
  int GetAction (int dev, int evtype);
DECLARE_EVENT_TABLE()
};


////////////////////////////////////////////////////////////////////////////
// DebugLogDialog
////////////////////////////////////////////////////////////////////////////
// DebugLogDialog allows the user to decide how Bochs will
// behave for each type of log event.
//
// +---- Debugger log ---------------------------------------+
// |                                                         |
// |  +--------------------------------------------------+   |
// |  |(0) f000:fff0: ea5be000f0: jmp f000:e05b          |   |
// |  |(0) 0010:00001868: 83fb10: cmp EBX, #10           |   |
// |  |                                                  |   |
// |  |                                                  |   |
// |  |                                                  |   |
// |  |                                                  |   |
// |  +--------------------------------------------------+   |
// |  Type a debugger command:                               |
// |  +----------------------------------------+ +-------+   |
// |  | step 1000                              | |Execute|   |
// |  +----------------------------------------+ +-------+   |
// |                                                         |
// |                                              [ Close ]  |
// +---------------------------------------------------------+
class DebugLogDialog: public wxDialog
{
private:
#define DEBUG_LOG_TITLE "Debugger log"
#define DEBUG_CMD_PROMPT "Type a debugger command:"
  wxBoxSizer *mainSizer, *commandSizer, *buttonSizer;
  wxTextCtrl *log, *command;
public:
  DebugLogDialog(wxWindow* parent, wxWindowID id);
  void Init ();  // called automatically by ShowModal()
  void OnEvent (wxCommandEvent& event);
  void OnEnterEvent (wxCommandEvent& event) { Execute(true); }
  void OnKeyEvent (wxKeyEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void Execute (bool clearCommand);
  void AppendCommand (const char *);
  void AppendText (wxString text);
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// ConfigSoundDialog
////////////////////////////////////////////////////////////////////////////
// 
// +--- Configure Sound -------------------------------------------+
// |                                                               |
// |  Bochs can emulate a Sound Blaster 16.  Would you like        |
// |  to enable it?                                                |
// |                                                               |
// |           Enable [X]                                          |
// |                                                               |
// |   DMA timer: [_________]                                      |
// |                                                               |
// |   Midi mode  [ 1 ]  Output file [_________________] [Browse]  |
// |   Wave mode  [ 1 ]  Output file [_________________] [Browse]  |
// |   Log  mode  [ 1 ]  Output file [_________________] [Browse]  |
// |                                                               |
// |                                    [ Help ] [ Cancel ] [ Ok ] |
// +---------------------------------------------------------------+


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
class ParamDialog: public wxDialog 
{
private:
  struct ParamStruct : public wxObject {
    bx_param_c *param;
    int id;
    union _u_tag {
      wxWindow *window;
      wxChoice *choice;
      wxTextCtrl *text;
      wxCheckBox *checkbox;
    } u;
    int browseButtonId;  // only for filename params
    wxButton *browseButton;  // only for filename params
  };
  void ShowHelp ();
  wxFlexGridSizer *gridSizer;
  wxChoice *type;
  wxTextCtrl *serialDelay, *pasteDelay, *mappingFile;
  wxCheckBox *enableKeymap;
  int genId ();
  bool isGeneratedId (int id);
  bool isShowing;
  int nbuttons;
protected:
  wxBoxSizer *mainSizer, *buttonSizer;
  // hash table that maps the ID of a wxWindows control (e.g. wxChoice,
  // wxTextCtrl) to the associated ParamStruct object.  Data in the hash table
  // is of ParamStruct*.
  wxHashTable *idHash;
  // map parameter ID (BXP_*) onto ParamStruct.
  wxHashTable *paramHash;
  virtual void EnableChanged ();
  void EnableChanged (ParamStruct *pstr);
  bool CopyGuiToParam ();
public:
  ParamDialog(wxWindow* parent, wxWindowID id);
  virtual ~ParamDialog() {}
  void OnEvent (wxCommandEvent& event);
  wxButton* AddButton(int id, wxString label);
  virtual void AddDefaultButtons ();
  void BeginParamGrid (wxFlexGridSizer *gs = NULL) { gridSizer = gs; }
  virtual void Init ();  // called automatically by ShowModal()
  int ShowModal() {
    Init(); 
    isShowing = true;
    int ret = wxDialog::ShowModal(); 
    isShowing = false;
    return ret;
  }
  bool Show (bool val) { isShowing = val; return wxDialog::Show (val); }
  void AddParam (bx_param_c *param, wxFlexGridSizer *sizer = NULL, bool plain = false);
  void AddParamList (bx_id *idList, wxFlexGridSizer *sizer = NULL, bool plain = false);
  virtual void CopyParamToGui ();
  bool IsShowing () { return isShowing; }
DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////
// CpuRegistersDialog
////////////////////////////////////////////////////////////////////////////
// 
// this would display the current values of all CPU registers, possibly you can
// enable different groups like debug, FPU, MMX registers.  Certainly if you
// interrupt the simulation, these would be updated.  we could update
// periodically during simulation if it was useful.  If we get the debugger
// integrated with wxwindows, you could single step and update the cpu
// registers, with regs that change marked in a different color.  Modeless
// dialog.
// 
// +--- CPU Registers ---------------------------------------+
// |                                                         |
// |  EAX 0x00000000   EIP    0xffff   LDTR 0x00000000       |
// |  EBX 0x00000000   CS     0x0018   TR   0x00000000       |
// |  ECX 0x00000000   SS     0x0018   GDTR 0x00000000       |
// |  EDX 0x00000000   DS     0x0018    lim 0x00000000       |
// |  EBP 0x00000000   ES     0x0018   IDTR 0x00000000       |
// |  ESI 0x00000000   FS     0x0018    lim 0x00000000       |
// |  EDI 0x00000000   GS     0x0018                         |
// |  ESP 0x00000000   EFLAGS 0x0012                         |
// |                                                         |
// | ID AC VM RF NT IOPL CF PF AF ZF SF TF IF DF OF          |
// | [] [] [] [] [] [0]  [] [] [] [] [] [] [] [] []          |
// |                                                         |
// | DR0 0x00000000   TR3 0x00000000  CR0 0x00000000         |
// | DR1 0x00000000   TR4 0x00000000  CR1 0x00000000         |
// | DR2 0x00000000   TR5 0x00000000  CR2 0x00000000         |
// | DR3 0x00000000   TR6 0x00000000  CR3 0x00000000         |
// | DR6 0x00000000   TR7 0x00000000  CR4 0x00000000         |
// | DR7 0x00000000                                          |
// |                                                         |
// |      [Go]  [Stop]  [Step]  [Step N]  N=[____]           |
// +---------------------------------------------------------+
//
// +--- CPU Extended Registers ------------------------------+
// |                                                         |
// |                                                         |
// |      [Go]  [Stop]  [Step]  [Step N]  N=[____]           |
// +---------------------------------------------------------+
//
class CpuRegistersDialog : public ParamDialog
{

#define CPU_REGS_MAIN_REGS1                                             \
  { BXP_CPU_EAX, BXP_CPU_EBX, BXP_CPU_ECX, BXP_CPU_EDX,                 \
    BXP_CPU_EBP, BXP_CPU_ESI, BXP_CPU_EDI, BXP_CPU_ESP,                 \
    BXP_NULL }
#define CPU_REGS_MAIN_REGS2                                             \
  { BXP_CPU_EIP, BXP_CPU_SEG_CS, BXP_CPU_SEG_SS, BXP_CPU_SEG_DS,        \
    BXP_CPU_SEG_ES, BXP_CPU_SEG_FS, BXP_CPU_SEG_GS, BXP_CPU_EFLAGS,     \
    BXP_NULL }
#define CPU_REGS_MAIN_REGS3                                             \
  { BXP_CPU_SEG_LDTR, BXP_CPU_SEG_TR,                                   \
    BXP_CPU_GDTR_BASE, BXP_CPU_IDTR_LIMIT,                              \
    BXP_CPU_IDTR_BASE, BXP_CPU_GDTR_LIMIT,                              \
    BXP_NULL }
#define CPU_REGS_FLAGS                                                  \
  { BXP_CPU_EFLAGS_ID, BXP_CPU_EFLAGS_VIP, BXP_CPU_EFLAGS_VIF,          \
    BXP_CPU_EFLAGS_AC, BXP_CPU_EFLAGS_VM, BXP_CPU_EFLAGS_RF,            \
    BXP_CPU_EFLAGS_NT, BXP_CPU_EFLAGS_IOPL, BXP_CPU_EFLAGS_OF,          \
    BXP_CPU_EFLAGS_DF, BXP_CPU_EFLAGS_IF, BXP_CPU_EFLAGS_TF,            \
    BXP_CPU_EFLAGS_SF, BXP_CPU_EFLAGS_ZF, BXP_CPU_EFLAGS_AF,            \
    BXP_CPU_EFLAGS_PF, BXP_CPU_EFLAGS_CF, \
    BXP_NULL }
#define CPU_REGS_DEBUG_REGS                                             \
  { BXP_CPU_DR0, BXP_CPU_DR1, BXP_CPU_DR2,                              \
    BXP_CPU_DR3, BXP_CPU_DR6, BXP_CPU_DR7,                              \
    BXP_NULL }
#define CPU_REGS_TEST_REGS                                              \
  { BXP_CPU_TR3, BXP_CPU_TR4, BXP_CPU_TR5, BXP_CPU_TR6, BXP_CPU_TR7,    \
    BXP_NULL }
#define CPU_REGS_CONTROL_REGS                                           \
  { BXP_CPU_CR0, BXP_CPU_CR1, BXP_CPU_CR2, BXP_CPU_CR3, BXP_CPU_CR4,    \
    BXP_NULL  }

  void Init ();  // called automatically by ShowModal()
  wxFlexGridSizer *mainRegsSizer, *flagsSizer, *extRegsSizer;
#define CPU_REGS_MAX_FLAGS 17
  bx_id flagid[CPU_REGS_MAX_FLAGS];
  int nflags;
#if BX_DEBUGGER
  wxButton *contButton, *stopButton, *stepButton, *commitButton;
#endif
  void stateChanged (bool simRunning);
public:
  CpuRegistersDialog(wxWindow* parent, wxWindowID id);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void AddFlag (bx_id paramId);
  void OnEvent (wxCommandEvent& event);
  virtual void CopyParamToGui ();
  DECLARE_EVENT_TABLE()
};



/**************************************************************************
Everything else in here is a comment!




////////////////////////////////////////////////////////////////////////////
// proposed dialogs, not implemented 
////////////////////////////////////////////////////////////////////////////

Here are some quick sketches of what different parts of the interface
could look like.  None of these is implemented yet, and everything is
open for debate.  Whoever writes the wxwindows code for any of these
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

The CD-ROM media can still be configured during the simulation.  In this
context we can just show the Media section.  The same code can be written to
serve both purposes.  This is the dialog that would appear when you click the
CD-ROM button on the toolbar at runtime.

+-- CD-ROM Media -----------------------------+
|                                             |
|  Bochs can use a physical CD-ROM drive as   |
|  the data source, or use an image file.     |
|                                             |
|   [X]  Ejected                              |
|   [ ]  Physical CD-ROM drive /dev/cdrom     |
|   [ ]  Disk image: [_____________] [Browse] |
|                                             |
|                                             |
|                       [Help] [Cancel] [Ok]  |
+---------------------------------------------+


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
// ChooseBootDialog
////////////////////////////////////////////////////////////////////////////

This dialog basically lets you choose which disk you want to boot: floppy A,
disk c, or cdrom.  The boot selection could be as simple as
+-------------------------------------------+
|  Choose boot drive                        |
|    [ ] Floppy A                           |
|    [X] Hard Disk C                        |
|    [ ] CD-ROM                             |
|                [ Help ] [ Cancel ] [ Ok ] |
+-------------------------------------------+
or fancier with icons for the device types, and Edit buttons that
let you go right to the configure screen for that disk drive.
+---------------------------------------------------------------+
|                                                               |
|          /----+                                               |
|          |=  =|   A Drive                             +----+  |
| [    ]   | __ |   Raw Floppy Drive                    |Edit|  |
|          ||  ||   A:                                  +----+  |
|          ++--++                                               |
|                                                               |
|          /----+                                               |
|          |=  =|   B Drive                             +----+  |
| [    ]   | __ |   Floppy Disk Image                   |Edit|  |
|          ||  ||   C:\Bochs\Images\A.img               +----+  |
|          ++--++                                               |
|                                                               |
|          +-----+  C Drive                                     |
|          |=====|  Hard Disk Image                     +----+  | 
| [BOOT]   |    o|  C:\Bochs\Images\HD30meg.img         |Edit|  |
|          +-----+                                      +----+  |
|                                                               |
|            ___                                                |
|           /   \   D Drive                             +----+  |
| [    ]   |  O  |  ISO CD Image                        |Edit|  |
|           \___/   C:\Bochs\Images\BootCD.img          +----+  |
|                                                               |
|                                    [ Help ] [ Cancel ] [ Ok ] |
+---------------------------------------------------------------+

////////////////////////////////////////////////////////////////////////////
// KeymappingDialog      
////////////////////////////////////////////////////////////////////////////
more ambitious: create a button for each key on a standard keyboard, so that
you can view/edit/load/save key mappings, produce any combination of keys 
(esp. ones that your OS or window manager won't allow)

////////////////////////////////////////////////////////////////////////////
// ConfigTimeDialog
////////////////////////////////////////////////////////////////////////////

choose IPS
select starting time for CMOS clock
turn on real time PIT or not (?)
VGA update interval

This dialog can easily allow people to tune the IPS setting, or
various other speed-related values, at runtime.  If you're running
some time-sensitive program you could adjust IPS until it's the right
speed, or if Bochs is wasting all of your CPU's cycles you could turn
a dial to some periodic delays to allow other processes a chance to
complete.

////////////////////////////////////////////////////////////////////////////
// OtherOptionsDialog
////////////////////////////////////////////////////////////////////////////

everything in the bochsrc that doesn't fit into some other category,
or that is used so rarely (e.g. floppy command delay) that it's not worth
laying out manually in a dialog box.  This will probably be done in
sort of a grid with parameter name, and value(editable) in different columns

////////////////////////////////////////////////////////////////////////////
// LogOptionsDialog
////////////////////////////////////////////////////////////////////////////
lets you choose which events you want to write to the log, which you
want to ignore, etc.  You can do this at a high level, like

+---- Configure events -----------------------------------+
|                                                         |
| How should Bochs respond to each type of event?         |
|                                                         |
|            Debug events: [ignore]                       |
|             Info events: [ignore]                       |
|            Error events: [report]                       |
|            Panic events: [ask   ]                       |
|                                                         |
| For additional control over how each device responds    |
| to events, press the "Advanced" button.                 |
|                                                         |
|                [ Advanced ]  [ Help ] [ Cancel ] [ Ok ] |
+---------------------------------------------------------+
This sets up the default actions for all devices.  The advanced
dialog lets you set different actions per device.  I have two
attempts at the advanced dialog.  The first creates a large
grid of wxChoice controls which choose between
ignore/report/ask/die.  There will be enough rows in this
table that a scrolling subwindow will be needed to fit
all the devices.

+---- Advanced event configuration -----------------------+
|                                                         |
| This table determines how Bochs will respond to each    |
| kind of event coming from a particular source.  For     |
| example if you are having problems with the keyboard,   |
| you could ask for debug and info events from the        |
| keyboard to be reported.                                |
|                                                         |
|                        [Use defaults for all devices]   |
+-------------------------------------------------------+-+
|  Device    Debug     Info      Error      Panic       |^|
|  --------  --------  -------   --------   ---------   |||
|  Keyboard  [ignore]  [ignore]  [report]   [report]    |||
|  VGA       [ignore]  [ignore]  [report]   [report]    |||
|  NE2000    [ignore]  [ignore]  [report]   [report]    |||
|  Sound     [ignore]  [ignore]  [report]   [report]    |v|
+---------------------------------------------------------+
|                              [ Help ] [ Cancel ] [ Ok ] |
+-------------------------------------------------------+-+

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



////////////////////////////////////////////////////////////////////////////
// ViewMemoryDialog
////////////////////////////////////////////////////////////////////////////

shows portions of memory, in hex or hex+ASCII or disassembled.  updates
whenever simulation stops (after single steps for example), or we could
update periodically.  Modeless dialog, and there could be many
of them at once, showing different regions of memory.

////////////////////////////////////////////////////////////////////////////
// DebugControlDialog
////////////////////////////////////////////////////////////////////////////
has buttons for most common debugger commands such as step, breakpoint,
display registers, or whatever.

*****************************************************************/
