////////////////////////////////////////////////////////////////////
// $Id: wxdialog.h,v 1.11 2002-08-30 06:49:19 bdenney Exp $
////////////////////////////////////////////////////////////////////
//
// wxWindows dialogs for Bochs

#include "wx/spinctrl.h"

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
#define FLOPPY_MAX_RBTNS 4
  wxRadioButton *rbtn[FLOPPY_MAX_RBTNS];
  char *equivalentFilename[FLOPPY_MAX_RBTNS];
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
  void SetFilename (const char *f) { filename->SetValue (wxString (f)); }
  char *GetFilename () { return (char *)filename->GetValue().c_str (); }
  void SetDriveName (const char *name);
  void SetGeom (int n, int value);
  int GetGeom (int n) { return geom[n]->GetValue (); }
  void SetGeomRange (int n, int min, int max) { geom[n]->SetRange (min, max); }
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
// | |  [ ]  Disk image file: [____________________] [Browse]  |   |
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
// ShowModal() to display it.  Return value is 0 for ok or -1 for
// cancel.  After ShowModal() returns, use GetFilename() and
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
  char *equivalentFilename[CDROM_MAX_RBTNS];
  int n_rbtns;
public:
  CdromConfigDialog(wxWindow* parent, wxWindowID id);
  void OnEvent (wxCommandEvent& event);
  int ShowModal() { Init(); return wxDialog::ShowModal(); }
  void SetFilename (const char *f);
  char *GetFilename ();
  void SetDriveName (const char *name);
  void EnableChanged ();
  void SetEnable (bool val) { enable->SetValue (val); EnableChanged (); }
  bool GetEnable () { return enable->GetValue (); }
  void AddRadio (const char *descr, char *path);
  // rbtn[0] will always be the "ejected" button
  void SetEjected (bool val) { if (val) rbtn[0]->SetValue (TRUE); }
  bool GetEjected () { return rbtn[0]->GetValue (); }
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
// ConfigNetworkDialog
////////////////////////////////////////////////////////////////////////////
Edit network settings
+---------------------------------------------------------------+
|                                                               |
|  Bochs can emulate an NE2000-compatible network card.  Would  |
|  you like to enable it?                                       |
|                                                               |
|      Enable networking?  [X]                                  |
|                                                               |
|      NE2000 I/O address: [ 0x280 ]                            |
|                     IRQ: [   9   ]                            |
|             MAC address: [ b0:c4:00:00:00:00 ]                |
|    Connection to the OS: [ Linux Packet Filter ]              |
|     Physical NIC to use: [ eth0 ]                             |
|   NE2000 Debug messages: [ ]                                  |
|                                                               |
|                                    [ Help ] [ Cancel ] [ Ok ] |
+---------------------------------------------------------------+

////////////////////////////////////////////////////////////////////////////
// ConfigMemoryDialog
////////////////////////////////////////////////////////////////////////////

This edits options related to RAM and ROM, similar to the text menu
1. Memory size in megabytes: 4
2. Name of VGA BIOS image: 
3. Name of ROM BIOS image: 
4. ROM BIOS address: 0x00000
5. Name of optional ROM image #1 : 
6. optional ROM #1 address: 0x00000
7. Name of optional ROM image #2 : 
8. optional ROM #2 address: 0x00000
9. Name of optional ROM image #3 : 
10. optional ROM #3 address: 0x00000
11. Name of optional ROM image #4 : 
12. optional ROM #4 address: 0x00000

////////////////////////////////////////////////////////////////////////////
// ConfigSoundDialog
////////////////////////////////////////////////////////////////////////////

This edits options related to sound blaster emulation, similar to the
text menu
1. SB16 is present: yes
2. Midi file: /dev/midi00
3. Wave file: /dev/dsp
4. Log file: sb16.log
5. Midi mode: 1
6. Wave mode: 1
7. Log mode: 2
8. DMA timer: 600000

////////////////////////////////////////////////////////////////////////////
// ConfigKeyboardDialog
////////////////////////////////////////////////////////////////////////////
keyboard related settings

keyboard_mapping: enabled=1, map=US,France,Germany,Spain
keyboard_type: xt,at,mf
keyboard_serial_delay: 250
paste delay

more ambitious: create a button for each key on a standard keyboard, so that
you can view/edit/load/save key mappings, produce any combination of keys 
(esp. ones that your OS or window manager won't allow)

////////////////////////////////////////////////////////////////////////////
// ConfigTimeDialog
////////////////////////////////////////////////////////////////////////////

choose IPS
select starting time for CMOS clock
turn on real time PIT or not

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

Event type    Action
panic          ask
error          report
info           ignore
debug          ignore

Or when you want more control:

            panic        error        info          debug
	    -----        ------       -----         ----- 
keyboard     ask        report        report        report
vga          ask        report        report        report
network      ask        report        report        ignore
cpu          ask        report        report        ignore
sound        ask        report        report        ignore

////////////////////////////////////////////////////////////////////////////
// CpuRegistersDialog
////////////////////////////////////////////////////////////////////////////

this would display the current values of all CPU registers, possibly you can
enable different groups like debug, FPU, MMX registers.  Certainly if you
interrupt the simulation, these would be updated.  we could update periodically
during simulation if it was useful.  If we get the debugger integrated with
wxwindows, you could single step and update the cpu registeres, with regs that
change marked in a different color.  Modeless dialog.

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
