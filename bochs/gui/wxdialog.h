////////////////////////////////////////////////////////////////////
// $Id: wxdialog.h,v 1.8 2002-08-29 23:18:10 bdenney Exp $
////////////////////////////////////////////////////////////////////
//
// wxWindows dialogs for Bochs

#include "wx/spinctrl.h"

////////////////////////////////////////////////////////////////////
// text messages used in several places
////////////////////////////////////////////////////////////////////
#define MSG_NO_HELP "No help is available yet."
#define MSG_NO_HELP_CAPTION "No help"
#define BTNLABEL_HELP "Help"
#define BTNLABEL_CANCEL "Cancel"
#define BTNLABEL_OK "Ok"
#define BTNLABEL_CREATE_IMG "Create Image"

////////////////////////////////////////////////////////////////////
// LogMsgAskDialog is a modal dialog box that shows the user a
// simulation error message and asks if they want to continue or 
// not.  It looks something like this:
// -------------------------------------------------------------
//  Context: Hard Drive
//  Message: could not open hard drive image file '30M.sample'
//
//   [ ] Don't ask about future messages like this
//
//   [Continue]   [Die]  [Dump Core]  [Debugger]   [Help]
// -------------------------------------------------------------
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
  { "Continue", "Die", "Dump Core", "Debugger", "Help" }
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
  void SetContext (char *s);
  void SetMessage (char *s);
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
// |    [ ]  Disk image file: [________________________] [Browse]  |
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
// Then call ShowModal() to display it.  Return value is 0 for ok or -1
// for cancel.  If you set a validation function, then it will be called
// when ok is pressed, and will get a chance to veto the "Ok" if it
// returns false.  After ShowModal() returns, use GetFilename and
// GetCapacity to see what the user did.  If the validation function
// sets parameters, this may be unnecessary.
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
// | |  [ ]  Disk image file: [____________________] [Browse]  |   |
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
#define FLOPPY_CONFIG_DISKIMG "Disk image file: "
private:
  void Init ();  // called automatically by ShowModal()
  void ShowHelp ();
  wxStaticText *instr;
#define MAX_RBTNS 4
  wxRadioButton *rbtn[MAX_RBTNS];
  char *equivalentFilename[MAX_RBTNS];
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
  void SetFilename (const char *f);
  void SetCapacityChoices (int n, char *choices[]);
  void SetCapacity (int cap) { capacity->SetSelection (cap); }
  int GetRadio ();
  int GetCapacity () { return capacity->GetSelection (); }
  char *GetFilename ();
  void SetDriveName (const char *name);
  void SetValidateFunc (validateFunc_t v) { validate = v; }
  void AddRadio (char *description, char *filename);
  void CreateImage ();
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
  void SetFilename (const char *f);
  char *GetFilename ();
  void SetDriveName (const char *name);
  void SetGeom (int n, int value);
  int GetGeom (int n) { return geom[n]->GetValue (); }
  void SetGeomRange (int n, int min, int max) { geom[n]->SetRange (min, max); }
  float UpdateMegs ();
  void SetEnable (bool val) { enable->SetValue (val); }
  bool GetEnable () { return enable->GetValue (); }
  void EnterSize ();
  bool CreateImage (int harddisk, int sectors, const char *filename);
DECLARE_EVENT_TABLE()
};
