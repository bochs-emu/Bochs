/////////////////////////////////////////////////////////////////
// $Id: wxdialog.cc,v 1.47 2002-10-24 21:06:47 bdenney Exp $
/////////////////////////////////////////////////////////////////
//
// misc/wxdialog.cc
// This is a testbed for wxWindows dialog box experiments.
//

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/spinctrl.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include <wx/notebook.h>

#include "config.h"              // definitions based on configure script
#include "osdep.h"               // workarounds for missing stuff
#include "gui/siminterface.h"    // interface to the simulator
#include "bxversion.h"           // get version string
#include "wxdialog.h"            // custom dialog boxes
#include "wxmain.h"              // wxwindows shared stuff

//////////////////////////////////////////////////////////////////////
// constants, prototypes
//////////////////////////////////////////////////////////////////////

enum {
  ID_ShowDialog_1 = 1,
  ID_ShowDialog_2,
  ID_ShowDialog_3,
  ID_Button1,
  ID_Button2,
  ID_MY_LAST_ID
};

wxSize longTextSize (300, -1);  // width=300, height=default
wxSize normalTextSize (180, -1);  // width=200, height=default

//////////////////////////////////////////////////////////////////////
// LogMsgAskDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   vertSizer: 
//    context text field, 
//    message text field
//    don't-ask checkbox
//    buttonSizer:
//	continue button
//	die button
//	dumpcore button
//	debugger button
//	help button
//

// all events go to OnEvent method
BEGIN_EVENT_TABLE(LogMsgAskDialog, wxDialog)
  EVT_BUTTON(ID_Continue, LogMsgAskDialog::OnEvent)
  EVT_BUTTON(ID_Die, LogMsgAskDialog::OnEvent)
  EVT_BUTTON(ID_DumpCore, LogMsgAskDialog::OnEvent)
  EVT_BUTTON(ID_Debugger, LogMsgAskDialog::OnEvent)
  EVT_BUTTON(wxID_HELP, LogMsgAskDialog::OnEvent)
END_EVENT_TABLE()


LogMsgAskDialog::LogMsgAskDialog(
    wxWindow* parent,
    wxWindowID id,
    const wxString& title)
  : wxDialog (parent, id, title, wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  for (int i=0; i<N_BUTTONS; i++) enabled[i] = TRUE;
  vertSizer = new wxBoxSizer(wxVERTICAL);
  context = new wxStaticText (this, -1, "");
  wxFont font = context->GetFont ();
  font.SetWeight (wxBOLD);
  font.SetPointSize (2 + font.GetPointSize ());
  context->SetFont (font);
  message = new wxStaticText (this, -1, "");
  message->SetFont (font);
  dontAsk = new wxCheckBox (this, -1, LOG_MSG_DONT_ASK_STRING);
  btnSizer = new wxBoxSizer(wxHORIZONTAL);
  // fill vertical sizer
  vertSizer->Add (context, 0, wxGROW|wxALIGN_LEFT|wxLEFT|wxTOP, 30);
  vertSizer->Add (message, 0, wxGROW|wxALIGN_LEFT|wxLEFT, 30);
  vertSizer->Add (dontAsk, 0, wxALIGN_CENTER|wxTOP, 30);
  vertSizer->Add (btnSizer, 0, wxALIGN_CENTER|wxTOP, 30);
  // Some object creation and layout is postponed until Init()
  // so that caller has time to configure the dialog.
}

void LogMsgAskDialog::SetContext (wxString s) {
  wxString text;
  text.Printf (LOG_MSG_CONTEXT, s.c_str ());
  ChangeStaticText (vertSizer, context, text);
}

void LogMsgAskDialog::SetMessage (wxString s) {
  wxString text;
  text.Printf (LOG_MSG_MSG, s.c_str ());
  ChangeStaticText (vertSizer, message, text);
}

void LogMsgAskDialog::Init ()
{
  static const int ids[N_BUTTONS] = LOG_MSG_ASK_IDS;
  static const char *names[N_BUTTONS] = LOG_MSG_ASK_NAMES;
  for (int i=0; i<N_BUTTONS; i++) {
    if (!enabled[i]) continue;
    wxButton *btn = new wxButton (this, ids[i], names[i]);
    btnSizer->Add (btn, 1, wxALL, 5);
  }
  wxSize ms = message->GetSize ();
  wxLogMessage ("message size is %d,%d", ms.GetWidth(), ms.GetHeight ());
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 10;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

// Event handler for dialog buttons. Just translate the wx ids into
// enum values and return them with EndModel() to make the dialog
// go away.
void LogMsgAskDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  int ret = -1;
  switch (id) {
    case ID_Continue:   ret = CONT;  break;
    case ID_Die:        ret = DIE;   break;
    case ID_DumpCore:   ret = DUMP;  break;
    case ID_Debugger:   ret = DEBUG; break;
    case wxID_HELP: ShowHelp (); return;
    default:
      return;  // without EndModal
  }
  wxLogMessage ("you pressed button id=%d, return value=%d", id, ret);
  EndModal (ret);
}

void LogMsgAskDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}

//////////////////////////////////////////////////////////////////////
// FloppyConfigDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   vertSizer: 
//     instructions
//     radioSizer (vert):
//       phys0
//       phys1
//       diskImageSizer (horiz):
//         disk image file
//         filename
//         browse button
//         create button
//     capacitySizer (horizontal):
//       capacity text
//       capacity choice box
//     hint text
//     buttonSizer:
//       cancel button
//       ok button
//       help button
//

// all events go to OnEvent method
BEGIN_EVENT_TABLE(FloppyConfigDialog, wxDialog)
  EVT_BUTTON(-1, FloppyConfigDialog::OnEvent)
  EVT_TEXT(-1, FloppyConfigDialog::OnEvent)
END_EVENT_TABLE()


FloppyConfigDialog::FloppyConfigDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  validate = NULL;
  n_rbtns = 0;
  wxButton *btn;
  vertSizer = new wxBoxSizer (wxVERTICAL);
  instr = new wxStaticText (this, -1, FLOPPY_CONFIG_INSTRS);
  radioSizer = new wxBoxSizer (wxVERTICAL);
  diskImageSizer = new wxBoxSizer (wxHORIZONTAL);
  capacitySizer = new wxBoxSizer (wxHORIZONTAL);
  wxStaticText *hint = new wxStaticText (this, -1, FLOPPY_CONFIG_HINT);
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);
  // add top level components to vertSizer
  vertSizer->Add (instr, 0, wxTOP|wxLEFT, 30);
  vertSizer->Add (radioSizer, 0, wxLEFT, 50);
  vertSizer->Add (capacitySizer, 0, wxTOP|wxLEFT, 30);
  vertSizer->Add (hint, 0, wxTOP|wxLEFT, 30);
  vertSizer->Add (buttonSizer, 0, wxALIGN_RIGHT|wxTOP, 30);
  // contents of capacitySizer
  wxStaticText *captext = new wxStaticText (this, -1, FLOPPY_CONFIG_CAP);
  capacity = new wxChoice (this, -1);
  capacitySizer->Add (captext, 0, wxALL, 5);
  capacitySizer->Add (capacity, 0, wxALL|wxADJUST_MINSIZE, 5);
  // contents of buttonSizer
  btn = new wxButton (this, wxID_HELP, "Help");
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, "Cancel");
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, ID_Create, "Create Image");
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxID_OK, "Ok");
  buttonSizer->Add (btn, 0, wxALL, 5);
  // create filename and diskImageRadioBtn so that we can tweak them before
  // Init comes.  However don't add it to any sizer yet because it needs to go
  // in after the last radio button.
  filename = new wxTextCtrl (this, ID_FilenameText, "", wxDefaultPosition, longTextSize);
  diskImageRadioBtn = new wxRadioButton (this, ID_Filename, FLOPPY_CONFIG_DISKIMG);

  // the radioSizer contents will be added by successive calls to
  // AddRadio().  The diskImageSizer will be added last, in Init().
}

void FloppyConfigDialog::AddRadio (
    const wxString& description,
    const wxString& filename)
{
  if (n_rbtns >= FLOPPY_MAX_RBTNS) {
    wxLogError ("AddRadio failed: increase FLOPPY_MAX_RBTNS in wxdialog.h");
    return;
  }
  rbtn[n_rbtns] = new wxRadioButton (this, -1, description);
  equivalentFilename[n_rbtns] = filename;
  radioSizer->Add (rbtn[n_rbtns]);
  n_rbtns++;
}

void FloppyConfigDialog::SetDriveName (wxString name) {
  wxString text;
  text.Printf (FLOPPY_CONFIG_TITLE, name.c_str ());
  SetTitle (text);
  text.Printf (FLOPPY_CONFIG_INSTRS, name.c_str ());
  ChangeStaticText (vertSizer, instr, text);
}

void FloppyConfigDialog::SetCapacityChoices (int n, char *choices[])
{
  for (int i=0; i<n; i++)
    capacity->Append (wxString (choices[i]));
}

void FloppyConfigDialog::Init()
{
  // add contents of diskImageSizer
  diskImageSizer->Add (diskImageRadioBtn);
  diskImageSizer->Add (filename, 1, wxGROW);
  wxButton *btn = new wxButton (this, ID_Browse, BTNLABEL_BROWSE);
  diskImageSizer->Add (btn, 0, wxALL, 5);
  radioSizer->Add (diskImageSizer);

  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

int
FloppyConfigDialog::GetRadio () {
  int i;
  for (i=0; i<n_rbtns; i++) {
    if (rbtn[i]->GetValue ()) 
      return i;
  }
  if (diskImageRadioBtn->GetValue ()) {
    return i;
  }
  wxLogError ("GetRadio() found nothing selected");
  return 0;
}

void
FloppyConfigDialog::SetRadio (int n) {
  if (n < n_rbtns) {
    rbtn[n]->SetValue (TRUE);
  } else {
    diskImageRadioBtn->SetValue (TRUE);
  }
}

void FloppyConfigDialog::SetFilename (wxString f) {
  // search equivalentFilename[] for matches. if it matches, select the
  // radio button instead.
  for (int i=0; i<n_rbtns; i++) {
    if (!strcmp (f.c_str (), equivalentFilename[i])) {
      rbtn[i]->SetValue (TRUE);
      return;  // leaving filename text field unchanged
    }
  }
  filename->SetValue (f); 
  diskImageRadioBtn->SetValue (TRUE);
}

wxString
FloppyConfigDialog::GetFilename ()
{
  int n = GetRadio ();
  if (n < n_rbtns) {
    return equivalentFilename[n];
  } else {
    return filename->GetValue ().c_str ();
  }
}

void FloppyConfigDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  wxLogMessage ("you pressed button id=%d", id);
  switch (id) {
    case ID_FilenameText:
      // when you type into the filename field, ensure that the radio
      // button associated with that field is chosen.
      diskImageRadioBtn->SetValue (TRUE);
      break;
    case wxID_OK:
      // probably should validate before allowing ok
      if (validate!=NULL && !(*validate)(this))
	return;  // validation failed, don't leave yet
      EndModal (wxID_OK);
      break;
    case ID_Browse:
      BrowseTextCtrl (filename);
      break;
    case ID_Create:
      {
	int cap = capacity->GetSelection ();
	if (capacity->GetString (cap).Cmp ("none") == 0
	    || !(cap>=0 && cap<n_floppy_type_names)) {
	  wxMessageBox("You must choose a valid capacity for the new disk image", "Bad Capacity", wxOK | wxICON_ERROR );
	  return;
	}
	char name[1024];
	strncpy (name, filename->GetValue ().c_str (), sizeof(name));
        if (CreateImage (0, floppy_type_n_sectors[cap], name)) {
	  wxString msg;
	  msg.Printf ("Created a %s disk image called '%s'.",
	      capacity->GetString (cap).c_str (), 
	      filename->GetValue ().c_str ());
	  wxMessageBox(msg, "Image Created", wxOK | wxICON_INFORMATION);
	}
      }
      break;
    case wxID_CANCEL:
      EndModal (wxID_CANCEL);
      break;
    case wxID_HELP:
      ShowHelp(); 
      break;
  }
}

void FloppyConfigDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}

//////////////////////////////////////////////////////////////////////
// HDConfigDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   vertSizer: 
//     enable checkbox
//     hsizer[0]:
//       "Disk image:"
//       disk image text control
//       browse button
//     hsizer[1]:
//       "Geometry: cylinders"
//       geom[0] = cyl control
//       "heads"
//       geom[1] = heads control
//       " sectors/track "
//       geom[2] = spt control
//     hsizer[2]:
//       megs = "Size in MB: NUMBER"
//       compute geometry button
//     buttonSizer:
//       help
//       cancel
//       ok

// all events go to OnEvent method
BEGIN_EVENT_TABLE(HDConfigDialog, wxDialog)
  EVT_BUTTON(-1, HDConfigDialog::OnEvent)
  EVT_CHECKBOX(-1, HDConfigDialog::OnEvent)
  EVT_TEXT(-1, HDConfigDialog::OnEvent)
END_EVENT_TABLE()


HDConfigDialog::HDConfigDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  static char *geomNames[] = HD_CONFIG_GEOM_NAMES;
  vertSizer = new wxBoxSizer (wxVERTICAL);
  enable = new wxCheckBox (this, ID_Enable, MSG_ENABLED);
  enable->SetValue (TRUE);
  hsizer[0] = new wxBoxSizer (wxHORIZONTAL);
  hsizer[1] = new wxBoxSizer (wxHORIZONTAL);
  hsizer[2] = new wxBoxSizer (wxHORIZONTAL);
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);
  // add top level components to vertSizer
  vertSizer->Add (enable, 0, wxTOP|wxLEFT, 10);
  vertSizer->Add (hsizer[0], 0, wxTOP|wxLEFT, 10);
  vertSizer->Add (hsizer[1], 0, wxTOP|wxLEFT, 10);
  vertSizer->Add (hsizer[2], 0, wxTOP|wxLEFT, 10);
  vertSizer->Add (buttonSizer, 0, wxALIGN_RIGHT|wxTOP, 10);
  // contents of hsizer[0]
  wxStaticText *text;
  text = new wxStaticText (this, -1, HD_CONFIG_DISKIMG);
  hsizer[0]->Add (text);
  filename = new wxTextCtrl (this, ID_FilenameText, "", wxDefaultPosition, longTextSize);
  hsizer[0]->Add (filename, 1);
  wxButton *btn = new wxButton (this, ID_Browse, BTNLABEL_BROWSE);
  hsizer[0]->Add (btn);
  // contents of hsizer[1]
  for (int i=0; i<3; i++) {
    text = new wxStaticText (this, -1, geomNames[i]);
    hsizer[1]->Add (text);
    geom[i] = new wxSpinCtrl (this, ID_Cylinders+i);
    hsizer[1]->Add (geom[i]);
  }
  // contents of hsizer[2]
  megs = new wxStaticText (this, ID_Megs, "");  // size in megs
  hsizer[2]->Add (megs);
  computeGeom = new wxButton (this, ID_ComputeGeometry, HD_CONFIG_COMPUTE_TEXT);
  hsizer[2]->Add (computeGeom, 0, wxLEFT, 20);
  // contents of buttonSizer
  btn = new wxButton (this, wxID_HELP, BTNLABEL_HELP);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, ID_Create, BTNLABEL_CREATE_IMG);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxID_OK, BTNLABEL_OK);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void HDConfigDialog::SetDriveName (wxString name) {
  wxString text;
  text.Printf (HD_CONFIG_TITLE, name.c_str ());
  SetTitle (text);
}

void HDConfigDialog::SetGeom (int n, int value) {
  wxLogMessage ("setting geom[%d] to %d", n, value);
  geom[n]->SetValue (value); 
  wxLogMessage ("now geom[%d] has value %d", n, geom[n]->GetValue ());
  UpdateMegs ();
}

void HDConfigDialog::SetGeomRange (int n, int min, int max)
{
  wxLogDebug ("Setting range of geom[%d] to min=%d, max=%d", n, min, max);
  geom[n]->SetRange (min, SPINCTRL_FIX_MAX(max)); 
  wxLogDebug ("now min=%d, max=%d", geom[n]->GetMin (), geom[n]->GetMax ());
}

float
HDConfigDialog::UpdateMegs () {
  float meg = 512.0 
    * geom[0]->GetValue () 
    * geom[1]->GetValue () 
    * geom[2]->GetValue ()
    / (1024.0*1024.0);
  wxString text;
  text.Printf (HD_CONFIG_MEGS, meg);
  ChangeStaticText (hsizer[2], megs, text);
  Layout ();
  return meg;
}

void HDConfigDialog::Init()
{
}

void HDConfigDialog::EnableChanged ()
{
  bool en = enable->GetValue ();
  filename->Enable (en);
  for (int i=0; i<3; i++) geom[i]->Enable (en);
  computeGeom->Enable (en);
}

void HDConfigDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  wxLogMessage ("you pressed button id=%d", id);
  switch (id) {
    case ID_Cylinders:
    case ID_Heads:
    case ID_SPT:
      UpdateMegs ();
      break;
    case ID_ComputeGeometry:
      EnterSize ();
      break;
    case ID_Create:
      {
	int cyl = geom[0]->GetValue ();
	int heads = geom[1]->GetValue ();
	int spt = geom[2]->GetValue ();
	int sectors = cyl*heads*spt;
	char name[1024];
	strncpy (name, filename->GetValue ().c_str (), sizeof(name));
	if (CreateImage (1, sectors, name)) {
	  wxString msg;
	  msg.Printf ("Created a %d megabyte disk image called '%s'.",
	      sectors*512, name);
	  wxMessageBox(msg, "Image Created", wxOK | wxICON_INFORMATION);
	}
      }
      break;
    case ID_Enable:
      EnableChanged ();
      break;
    case wxID_OK:
      // probably should validate before allowing ok
      EndModal (wxID_OK);
      break;
    case ID_Browse:
      BrowseTextCtrl (filename);
      break;
    case wxID_CANCEL:
      EndModal (wxID_CANCEL);
      break;
    case wxID_HELP:
      ShowHelp(); 
      break;
    default:
      event.Skip ();
  }
}

void HDConfigDialog::EnterSize ()
{
  int initval = (int) (0.5 + UpdateMegs ());
  int target_megs = wxGetNumberFromUser (HD_CONFIG_COMPUTE_INSTR, HD_CONFIG_COMPUTE_PROMPT, HD_CONFIG_COMPUTE_CAPTION, initval, 0, 32255);
  if (target_megs<0) return;
  // this calculate copied from misc/bximage.c
  int cyl, heads=16, spt=63;
  cyl = (int) (target_megs*1024.0*1024.0/16.0/63.0/512.0);
  wxASSERT (cyl < 65536);
  geom[0]->SetValue (cyl);
  geom[1]->SetValue (heads);
  geom[2]->SetValue (spt);
  UpdateMegs ();
}

void HDConfigDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}

//////////////////////////////////////////////////////////////////////
// CdromConfigDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   vertSizer: 
//     box labeled "Device":
//       enable = enable checkbox
//     box labeled "Media":
//       "Where should the emulated CD-ROM find its data?"
//       ejected radio button (added by constructor)
//       use physical radio buttons (added by calls to AddRadio)
//       fileSizer:
//         use disk image radio button
//         filename = text control
//         browse button
//     buttonSizer:
//       help
//       cancel
//       ok

// all events go to OnEvent method
BEGIN_EVENT_TABLE(CdromConfigDialog, wxDialog)
  EVT_BUTTON(-1, CdromConfigDialog::OnEvent)
  EVT_CHECKBOX(-1, CdromConfigDialog::OnEvent)
  EVT_TEXT(-1, CdromConfigDialog::OnEvent)
END_EVENT_TABLE()


CdromConfigDialog::CdromConfigDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  n_rbtns = 0;
  // top level objects
  wxStaticBox *dBox = new wxStaticBox (this, -1, "Device");
  dBoxSizer = new wxStaticBoxSizer (dBox, wxVERTICAL);
  wxStaticBox *mBox = new wxStaticBox (this, -1, "Media");
  mBoxSizer = new wxStaticBoxSizer (mBox, wxVERTICAL);
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);

  // add top level objects to vertSizer
  vertSizer = new wxBoxSizer (wxVERTICAL);
  vertSizer->Add (dBoxSizer, 0, wxALL|wxGROW, 10);
  vertSizer->Add (mBoxSizer, 0, wxALL|wxGROW, 10);
  vertSizer->Add (buttonSizer, 0, wxALIGN_RIGHT|wxTOP, 10);

  // device box contents
  enable = new wxCheckBox (this, ID_Enable, MSG_ENABLED);
  enable->SetValue (TRUE);
  dBoxSizer->Add (enable, 0, wxALL, 5);

  // media box contents
  //prompt = new wxStaticText (this, -1, CDROM_CONFIG_PROMPT);
  //mBoxSizer->Add (prompt, 0, wxTOP|wxLEFT|wxGROW, 10);
  AddRadio ("Ejected", "none");   // that's always an option!
  // ... wait for more calls to AddRadio before Init()

  // create fileSizer & contents, but don't add yet
  fileSizer = new wxBoxSizer (wxHORIZONTAL);
  diskImageRadioBtn = new wxRadioButton (this, -1, CDROM_CONFIG_DISKIMG);
  fileSizer->Add (diskImageRadioBtn, 0);
  filename = new wxTextCtrl (this, ID_FilenameText, "", wxDefaultPosition, longTextSize);
  fileSizer->Add (filename, 1, wxLEFT, 5);
  wxButton *btn = new wxButton (this, ID_Browse, BTNLABEL_BROWSE);
  fileSizer->Add (btn, 0, wxALL, 5);
  // create buttonSizer & contents but don't add yet
  btn = new wxButton (this, wxID_HELP, BTNLABEL_HELP);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxID_OK, BTNLABEL_OK);
  buttonSizer->Add (btn, 0, wxALL, 5);
}

void CdromConfigDialog::Init()
{
  // add top level components to vertSizer
  mBoxSizer->Add (fileSizer, 0, wxLEFT, 20);
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void CdromConfigDialog::SetDriveName (wxString name) {
  wxString text;
  text.Printf (CDROM_CONFIG_TITLE, name.c_str ());
  SetTitle (text);
}

// called from outside the object
void CdromConfigDialog::EnableChanged ()
{
  bool en = enable->GetValue ();
  //prompt->Enable (en);
  filename->Enable (en);
  for (int i=0; i<n_rbtns; i++)
    rbtn[i]->Enable (en);
  diskImageRadioBtn->Enable (en);
}

void CdromConfigDialog::SetFilename (wxString f) {
  for (int i=0; i<n_rbtns; i++) {
    if (!strcmp (f.c_str (), equivalentFilename[i])) {
      rbtn[i]->SetValue (TRUE);
      return;
    }
  }
  filename->SetValue (wxString (f));
}

wxString
CdromConfigDialog::GetFilename ()
{
  if (enable->GetValue ()) {
    // check radio buttons
    for (int i=0; i<n_rbtns; i++) {
      if (rbtn[i]->GetValue ())
	return equivalentFilename[i];
    }
    // if it wasn't any of the other radio buttons, it certainly should
    // be the last one.  That's what radio buttons do!
    wxASSERT (diskImageRadioBtn->GetValue ());
  }
  return filename->GetValue();
}

void 
CdromConfigDialog::AddRadio (const wxString& description, const wxString& filename)
{
  if (n_rbtns >= CDROM_MAX_RBTNS) {
    wxLogError ("AddRadio failed: increase CDROM_MAX_RBTNS in wxdialog.h");
    return;
  }
  rbtn[n_rbtns] = new wxRadioButton (this, -1, description);
  equivalentFilename[n_rbtns] = filename;
  mBoxSizer->Add (rbtn[n_rbtns], 0, wxLEFT, 20);
  n_rbtns++;
}

void CdromConfigDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  wxLogMessage ("you pressed button id=%d", id);
  switch (id) {
    case ID_FilenameText:
      // when you type into the filename field, ensure that the radio
      // button associated with that field is chosen.
      diskImageRadioBtn->SetValue (enable->GetValue ());
      break;
    case ID_Enable:
      EnableChanged ();  // enable/disable fields that depend on this
      break;
    case wxID_OK:
      // probably should validate before allowing ok
      EndModal (wxID_OK);
      break;
    case ID_Browse:
      BrowseTextCtrl (filename);
      break;
    case wxID_CANCEL:
      EndModal (wxID_CANCEL);
      break;
    case wxID_HELP:
      ShowHelp(); 
      break;
    default:
      event.Skip ();
  }
}

void CdromConfigDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}


//////////////////////////////////////////////////////////////////////
// NetConfigDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   mainSizer: 
//     vertSizer:
//       prompt
//       gridSizer 2 columns:
//         "enable networking"
//         enable = checkbox
//         "i/o addr"
//         io = wxTextCtrl
//         "irq"
//         irq = wxSpinCtrl
//         "mac"
//         mac = wxTextCtrl
//         "conn"
//         conn = wxChoice
//         "phys"
//         phys = wxTextCtrl
//         "script"
//         script = wxTextCtrl
//     buttonSizer:
//       help
//       cancel
//       ok

// all events go to OnEvent method
BEGIN_EVENT_TABLE(NetConfigDialog, wxDialog)
  EVT_BUTTON(-1, NetConfigDialog::OnEvent)
  EVT_CHECKBOX(-1, NetConfigDialog::OnEvent)
  EVT_TEXT(-1, NetConfigDialog::OnEvent)
END_EVENT_TABLE()


NetConfigDialog::NetConfigDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  n_conn_choices = 0;
  SetTitle (NET_CONFIG_TITLE);
  // top level objects
  mainSizer = new wxBoxSizer (wxVERTICAL);
  wxBoxSizer *vertSizer = new wxBoxSizer (wxVERTICAL);
  mainSizer->Add (vertSizer, 1, wxGROW|wxALIGN_LEFT);
  wxBoxSizer *buttonSizer = new wxBoxSizer (wxHORIZONTAL);
  mainSizer->Add (buttonSizer, 0, wxALIGN_RIGHT);

  // vertSizer contents
  wxStaticText *text;
  text = new wxStaticText (this, -1, NET_CONFIG_PROMPT);
  vertSizer->Add (text, 0, wxLEFT|wxRIGHT|wxTOP, 20);
  wxFlexGridSizer *gridSizer = new wxFlexGridSizer (2);
  vertSizer->Add (gridSizer, 1, wxALL|wxGROW, 30);

  // gridSizer contents
  gridSizer->AddGrowableCol (1);
#define add(x) gridSizer->Add (x, 0, wxALL|wxADJUST_MINSIZE, 5)
#define add_grow(x) gridSizer->Add (x, 1, wxALL|wxGROW, 5)
#define label(x) (new wxStaticText (this, -1, x))
  add (label (NET_CONFIG_EN));
  add (enable = new wxCheckBox (this, ID_Enable, ""));
  gridSizer->Add (30, 30);
  gridSizer->Add (30, 30);
  add (label (NET_CONFIG_IO));
  add (io = new wxTextCtrl (this, -1, "", wxDefaultPosition, normalTextSize));
  add (label (NET_CONFIG_IRQ));
  add (irq = new wxSpinCtrl (this, -1));
  add (label (NET_CONFIG_MAC));
  add (mac = new wxTextCtrl (this, -1, "", wxDefaultPosition, normalTextSize));
  add (label (NET_CONFIG_CONN));
  add (conn = new wxChoice (this, -1));
  add (label (NET_CONFIG_PHYS));
  add (phys = new wxTextCtrl (this, -1, "", wxDefaultPosition, normalTextSize));
  add (label (NET_CONFIG_SCRIPT));
  add_grow (script = new wxTextCtrl (this, -1, "", wxDefaultPosition, normalTextSize));
#undef label
#undef add

  irq->SetRange (0, 15);

  // buttonSizer contents
  wxButton *btn = new wxButton (this, wxID_HELP, BTNLABEL_HELP);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxID_OK, BTNLABEL_OK);
  buttonSizer->Add (btn, 0, wxALL, 5);
}

void NetConfigDialog::Init()
{
  EnableChanged ();
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(mainSizer);
  mainSizer->Fit (this);
  wxSize size = mainSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void NetConfigDialog::EnableChanged ()
{
  bool en = enable->GetValue ();
  io->Enable (en);
  irq->Enable (en);
  mac->Enable (en);
  conn->Enable (en);
  phys->Enable (en);
  script->Enable (en);
}

// allow (encourage) use of hex addresses started with "0x"
int NetConfigDialog::GetIO () {
  char buf[1024];
  wxString string(io->GetValue ());
  string.Trim ();
  strncpy (buf, string, sizeof(buf));
  int n = strtol (string, NULL, 0);
  if (n<0 || n>0xffff) {
    wxMessageBox("I/O address out of range. Try 0 - 0xffff.", "Bad I/O address", wxOK | wxICON_ERROR );
    return -1;
  }
  return n;
}

void NetConfigDialog::SetMac (unsigned char addr[6]) {
  wxString text;
  text.Printf ("%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  mac->SetValue (text);
}

bool
NetConfigDialog::GetMac (unsigned char addr[6]) {
  char buf[32];
  wxString string(mac->GetValue ());
  string.Trim ();
  strncpy (buf, string, sizeof(buf));
  // expect NN:NN:NN:NN:NN:NN format
  int part[6];
  if (6 != sscanf (string, "%x:%x:%x:%x:%x:%x", &part[0], &part[1], &part[2],
	&part[3], &part[4], &part[5])) 
  {
    wxMessageBox("MAC address must be in the form FF:FF:FF:FF:FF:FF.", "Bad MAC Address", wxOK | wxICON_ERROR );
      return false;
  }
  for (int i=0; i<6; i++)
    addr[i] = part[i];
  return true;
}

void NetConfigDialog::AddConn (wxString niceName, char *realName) {
  conn->Append (niceName); 
  int index = n_conn_choices++;
  conn->SetClientData (index, realName);
}

void NetConfigDialog::SetConn (const char *realname) {
  // search through the choices and find the one whose clientData matches
  // realname.
  for (int i=0; i<n_conn_choices; i++) {
    char *choiceRealName = (char *)conn->GetClientData (i);
    if (!strcmp (choiceRealName, realname)) {
      conn->SetSelection (i);
      return;
    }
  }
  wxLogError ("no choice match for '%s'", realname);
}

void NetConfigDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  wxLogMessage ("you pressed button id=%d", id);
  switch (id) {
    case ID_Enable:
      EnableChanged ();  // enable/disable fields that depend on this
      break;
    case wxID_OK:
      {
	// check for valid mac address by calling GetMac()
	unsigned char tmp[6];
	if (GetMac (tmp)) {
	  // mac address was legal
	  EndModal (wxID_OK);
	}
      }
      break;
    case wxID_CANCEL:
      EndModal (wxID_CANCEL);
      break;
    case wxID_HELP:
      ShowHelp(); 
      break;
    default:
      event.Skip ();
  }
}

void NetConfigDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}


//////////////////////////////////////////////////////////////////////
// LogOptionsDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   vertSizer:
//     logfileSizer
//       prompt
//       logfile
//       browse button
//     prompt
//     gridSizer 2 columns:
//       "debug"
//       action[0] = wxChoice
//       "info"
//       action[1] = wxChoice
//       "error"
//       action[2] = wxChoice
//       "panic"
//       action[3] = wxChoice
//     buttonSizer:
//       advanced
//       help
//       cancel
//       ok

// all events go to OnEvent method
BEGIN_EVENT_TABLE(LogOptionsDialog, wxDialog)
  EVT_BUTTON(-1, LogOptionsDialog::OnEvent)
  EVT_CHECKBOX(-1, LogOptionsDialog::OnEvent)
  EVT_TEXT(-1, LogOptionsDialog::OnEvent)
END_EVENT_TABLE()


LogOptionsDialog::LogOptionsDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  static char *names[] = LOG_OPTS_TYPE_NAMES;
  SetTitle (LOG_OPTS_TITLE);
  vertSizer = new wxBoxSizer (wxVERTICAL);
  // top level objects
  logfileSizer = new wxBoxSizer (wxHORIZONTAL);
  vertSizer->Add (logfileSizer, 0, wxTOP|wxLEFT, 20);
  wxStaticText *text = new wxStaticText (this, -1, LOG_OPTS_PROMPT);
  vertSizer->Add (text, 0, wxALL, 10);
  gridSizer = new wxFlexGridSizer (2);
  vertSizer->Add (gridSizer, 1, wxLEFT, 40);
  text = new wxStaticText (this, -1, LOG_OPTS_ADV);
  vertSizer->Add (text, 0, wxALL, 20);
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);
  vertSizer->Add (buttonSizer, 0, wxALIGN_RIGHT);

  // logfileSizer contents
  text = new wxStaticText (this, -1, LOG_OPTS_LOGFILE);
  logfileSizer->Add (text);
  logfile = new wxTextCtrl (this, -1, "", wxDefaultPosition, longTextSize);
  logfileSizer->Add (logfile);
  wxButton *btn = new wxButton (this, ID_Browse, BTNLABEL_BROWSE);
  logfileSizer->Add (btn, 0, wxALL, 5);

  // gridSizer contents
  gridSizer->AddGrowableCol (1);
  for (int evtype=0; evtype<LOG_OPTS_N_TYPES; evtype++) {
    gridSizer->Add (new wxStaticText (this, -1, names[evtype]), 0, wxALL, 5);
    action[evtype] = makeLogOptionChoiceBox (this, -1, evtype, true);
    gridSizer->Add (action[evtype], 1, wxALL|wxGROW|wxADJUST_MINSIZE, 5);
  }

  // buttonSizer contents
  btn = new wxButton (this, ID_Advanced, BTNLABEL_ADVANCED);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxID_HELP, BTNLABEL_HELP);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxID_OK, BTNLABEL_OK);
  buttonSizer->Add (btn, 0, wxALL, 5);
}

void LogOptionsDialog::Init()
{
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void LogOptionsDialog::SetAction (int evtype, int a) {
  // find the choice whose client data matches "a".
  int *ptr;
  //wxLogDebug ("SetAction type=%d a=%d", evtype, a);
  for (int i=0; i < action[evtype]->GetCount (); i++) {
    //wxLogDebug ("reading action[%d]->GetClientData(%d)", evtype, i);
    ptr = (int*) action[evtype]->GetClientData (i);
    if (ptr == NULL) continue;
    if (a == *ptr) {  // found it!
      action[evtype]->SetSelection (i);
      return;
    }
  }
  // this can happen if one of the choices that is excluded by
  // LOG_OPTS_EXCLUDE() is used, for example.
  wxLogDebug ("SetAction type=%d a=%d not found", evtype, a);
}

int LogOptionsDialog::GetAction (int evtype) {
  int sel = action[evtype]->GetSelection (); 
  int *ptrToChoice = (int*)action[evtype]->GetClientData (sel);
  wxASSERT (ptrToChoice != NULL);
  return *ptrToChoice;
}

void LogOptionsDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  wxLogMessage ("you pressed button id=%d", id);
  switch (id) {
    case ID_Browse:
      BrowseTextCtrl (logfile);
      break;
    case ID_Advanced:
      wxMessageBox ("The advanced dialog is not implemented yet.");
      break;
    case wxID_OK:
      EndModal (wxID_OK);
      break;
    case wxID_CANCEL:
      EndModal (wxID_CANCEL);
      break;
    case wxID_HELP:
      ShowHelp(); 
      break;
    default:
      event.Skip ();
  }
}

void LogOptionsDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}

//////////////////////////////////////////////////////////////////////
// AdvancedLogOptionsDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   vertSizer:
//     logfileSizer
//       prompt
//       logfile
//       browse button
//     prompt (multiline)
//     applyDefault button
//     scrollWin
//       scrollpanel
//         gridSizer 5 columns
//           device
//           debug
//           info
//           error
//           panic
//           etc.
//     buttonSizer:
//       help
//       cancel
//       ok

// all events go to OnEvent method
BEGIN_EVENT_TABLE(AdvancedLogOptionsDialog, wxDialog)
  EVT_BUTTON(-1, AdvancedLogOptionsDialog::OnEvent)
  EVT_CHECKBOX(-1, AdvancedLogOptionsDialog::OnEvent)
  EVT_TEXT(-1, AdvancedLogOptionsDialog::OnEvent)
END_EVENT_TABLE()

AdvancedLogOptionsDialog::AdvancedLogOptionsDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  //static int integers[LOG_OPTS_N_CHOICES_NORMAL] = {0, 1, 2, 3};
  static char *names[] = ADVLOG_OPTS_TYPE_NAMES;
  SetTitle (ADVLOG_OPTS_TITLE);
  vertSizer = new wxBoxSizer (wxVERTICAL);
  // top level objects
  logfileSizer = new wxBoxSizer (wxHORIZONTAL);
  vertSizer->Add (logfileSizer, 0, wxTOP|wxLEFT, 20);
  wxStaticText *text = new wxStaticText (this, -1, ADVLOG_OPTS_PROMPT);
  vertSizer->Add (text, 0, wxALL, 10);
  applyDefault = new wxButton (this, ID_ApplyDefault, ADVLOG_DEFAULTS);
  vertSizer->Add (applyDefault, 0, wxALL|wxALIGN_RIGHT, 10);
  scrollWin = new wxScrolledWindow (this, -1);
  vertSizer->Add (scrollWin, 1, wxALL|wxGROW, 10);
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);
  vertSizer->Add (buttonSizer, 0, wxALIGN_RIGHT);

  // logfileSizer contents
  text = new wxStaticText (this, -1, ADVLOG_OPTS_LOGFILE);
  logfileSizer->Add (text);
  logfile = new wxTextCtrl (this, -1, "", wxDefaultPosition, longTextSize);
  logfileSizer->Add (logfile);
  wxButton *btn = new wxButton (this, ID_Browse, BTNLABEL_BROWSE);
  logfileSizer->Add (btn, 0, wxALL, 5);

  // to get the scrollWin geometry right, first build everything on a wxPanel,
  // with gridSizer as the main sizer.
  scrollPanel = new wxPanel (scrollWin, -1);
  gridSizer = new wxGridSizer (5);
  // add title row
  int typemax = ADVLOG_OPTS_N_TYPES;
  text = new wxStaticText (scrollPanel, -1, "Device");
  gridSizer->Add (text, 0, wxALIGN_CENTER);
  int type;
  for (type=0; type < typemax; type++) {
    text = new wxStaticText (scrollPanel, -1, names[type]);
    gridSizer->Add (text, 0, wxALIGN_CENTER);
  }
  // add rows of choice boxes, one for each device
  int devmax = SIM->get_n_log_modules (); 
  action = new wxChoice** [devmax];   // array of pointers
  for (int dev=0; dev<devmax; dev++) {
    action[dev] = new wxChoice* [ADVLOG_OPTS_N_TYPES];
    // name of device in first column
    gridSizer->Add (new wxStaticText (scrollPanel, -1, SIM->get_prefix (dev)));
    // wxChoice in every other column
    for (type=0; type < typemax; type++) {
      action[dev][type] = makeLogOptionChoiceBox (scrollPanel, -1, type);
      gridSizer->Add (action[dev][type], 1, wxALL|wxGROW|wxADJUST_MINSIZE, 2);
    }
  }
  scrollPanel->SetAutoLayout (TRUE);
  scrollPanel->SetSizer (gridSizer);
  gridSizer->Fit (scrollPanel);
  gridSizer->SetSizeHints (scrollPanel);
  wxSize size = scrollPanel->GetBestSize ();
  // now we know how big the panel wants to be, and we can set the scrollbar
  // and scrollWin size accordingly

  // finally set up the scroll window outside
  scrollWin->SetScrollbars (1, 1, size.GetWidth (), size.GetHeight ());

  // now that we know the desired width of the panel, use it to set the
  // width of the scrollWin.  I tried several things before arriving at 
  // a solution, and I'll list them for educational purposes.
  //
  // failure #1: this had no effect at all. sizer took precedence.
  //   scrollWin->SetSize (500, 500);
  // failure #2: this changed scrollWin size but sizer was not notified so
  //             the overall window didn't expand to make space for it.
  //  scrollWin->SetSizeHints (500, 500);
  // success: tell the sizer to change the scrollWin's size, and it works
  vertSizer->SetItemMinSize (scrollWin, size.GetWidth()+30, 400);

  // buttonSizer contents
  btn = new wxButton (this, wxID_HELP, BTNLABEL_HELP);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxID_OK, BTNLABEL_OK);
  buttonSizer->Add (btn, 0, wxALL, 5);
}

AdvancedLogOptionsDialog::~AdvancedLogOptionsDialog()
{
  int dev, ndev = SIM->get_n_log_modules ();
  for (dev=0; dev<ndev; dev++) {
    delete [] action[dev];
  }
  delete [] action;
}

void AdvancedLogOptionsDialog::Init()
{
  CopyParamToGui ();
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void AdvancedLogOptionsDialog::CopyParamToGui () {
  bx_param_string_c *logfile = SIM->get_param_string (BXP_LOG_FILENAME);
  SetLogfile (wxString (logfile->getptr ()));
  // copy log action settings from siminterface to gui
  int dev, ndev = SIM->get_n_log_modules ();
  int type, ntype = SIM->get_max_log_level ();
  for (dev=0; dev<ndev; dev++) {
    for (type=0; type<ntype; type++) {
      SetAction (dev, type, SIM->get_log_action (dev, type));
    }
  }
}

void AdvancedLogOptionsDialog::CopyGuiToParam () {
  char buf[1024];
  safeWxStrcpy (buf, GetLogfile (), sizeof (buf));
  bx_param_string_c *logfile = SIM->get_param_string (BXP_LOG_FILENAME);
  logfile->set (buf);
  // copy log action settings from gui to siminterface
  int dev, ndev = SIM->get_n_log_modules ();
  int type, ntype = SIM->get_max_log_level ();
  for (dev=0; dev<ndev; dev++) {
    for (type=0; type<ntype; type++) {
      SIM->set_log_action (dev, type, GetAction (dev, type));
    }
  }
}

void AdvancedLogOptionsDialog::SetAction (int dev, int evtype, int act) {
  // find the choice whose client data matches "act".
  int *ptr;
  //wxLogDebug ("SetAction dev=%d type=%d act=%d", dev, evtype, act);
  wxChoice *control = action[dev][evtype];
  for (int i=0; i < control->GetCount (); i++) {
    //wxLogDebug ("reading action[%d][%d]->GetClientData(%d)", dev, evtype, i);
    ptr = (int*) control->GetClientData (i);
    if (ptr == NULL) continue;
    if (act == *ptr) {  // found it!
      control->SetSelection (i);
      return;
    }
  }
  // this can happen if one of the choices that is excluded by
  // ADVLOG_OPTS_EXCLUDE() is used, for example.
  wxLogDebug ("warning: SetAction type=%d act=%d not found", evtype, act);
}

int AdvancedLogOptionsDialog::GetAction (int dev, int evtype) {
  int sel = action[dev][evtype]->GetSelection (); 
  int *ptrToChoice = (int*)action[dev][evtype]->GetClientData (sel);
  wxASSERT (ptrToChoice != NULL);
  return *ptrToChoice;
}

void AdvancedLogOptionsDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  wxLogMessage ("you pressed button id=%d", id);
  switch (id) {
    case ID_Browse:
      BrowseTextCtrl (logfile);
      break;
    case ID_ApplyDefault: {
      int lev, nlev = SIM->get_max_log_level ();
      // copy default settings to every device
      for (lev=0; lev<nlev; lev++) {
	int action = SIM->get_default_log_action (lev);
	int dev, ndev = SIM->get_n_log_modules ();
	for (dev=0; dev<ndev; dev++) 
	  SetAction (dev, lev, action);
      }
      break;
      }
    case wxID_OK:
      CopyGuiToParam ();
      EndModal (wxID_OK);
      break;
    case wxID_CANCEL:
      EndModal (wxID_CANCEL);
      break;
    case wxID_HELP:
      ShowHelp(); 
      break;
    default:
      event.Skip ();
  }
}

void AdvancedLogOptionsDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}

//////////////////////////////////////////////////////////////////////
// ConfigMemoryDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   mainSizer:
//     box1 = static box "Standard Options"
//       box1sizer:
//         box1gridSizer, 3 columns:
//           "memsize"
//           megs = textfield
//           spacer
//           "biosimg"
//           biosImage = textfield
//           browse button
//           "biosaddr"
//           biosAddr = textfield
//           spacer
//           "vgabiosimg"
//           vgabios = textfield
//           browse button
//           "vgabiosaddr"
//           "0xc0000"
//     box2 = static box "Optional ROM images"
//       box2sizer:
//         box2gridSizer, 3 columns:
//           "opt rom 1"
//           romImage[0] = textfield
//           browse button
//           "opt rom 2"
//           romImage[1] = textfield
//           browse button
//           "opt rom 3"
//           romImage[2] = textfield
//           browse button
//           "opt rom 4"
//           romImage[3] = textfield
//           browse button
//     buttonSizer:
//       help
//       cancel
//       ok
//

// all events go to OnEvent method
BEGIN_EVENT_TABLE(ConfigMemoryDialog, wxDialog)
  EVT_BUTTON(-1, ConfigMemoryDialog::OnEvent)
  EVT_CHECKBOX(-1, ConfigMemoryDialog::OnEvent)
  EVT_TEXT(-1, ConfigMemoryDialog::OnEvent)
END_EVENT_TABLE()


ConfigMemoryDialog::ConfigMemoryDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  static char *box1_label[] = CONFIG_MEMORY_BOX1_LABELS;
  static char *box2_label[] = CONFIG_MEMORY_BOX2_LABELS;
  int n_browse = 0;
  int insideStaticBoxMargin = 15;
  SetTitle (CONFIG_MEMORY_TITLE);
  mainSizer = new wxBoxSizer (wxVERTICAL);
  wxStaticBox *box1 = new wxStaticBox (this, -1, CONFIG_MEMORY_BOX1_TITLE);
  box1sizer = new wxStaticBoxSizer (box1, wxVERTICAL);
  mainSizer->Add (box1sizer, 0, wxALL|wxGROW, 10);
  wxStaticBox *box2 = new wxStaticBox (this, -1, CONFIG_MEMORY_BOX2_TITLE);
  box2sizer = new wxStaticBoxSizer (box2, wxVERTICAL);
  mainSizer->Add (box2sizer, 0, wxALL|wxGROW, 10);
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);
  mainSizer->Add (buttonSizer, 0, wxALIGN_RIGHT);

  // box1 contents
  box1gridSizer = new wxFlexGridSizer (3);
  box1sizer->Add (box1gridSizer, 0, wxALL, insideStaticBoxMargin);
#define add(x) box1gridSizer->Add (x, 0, wxALL, 2)
#define addrt(x) box1gridSizer->Add (x, 0, wxALL|wxALIGN_RIGHT, 2)
#define newlabel(x) new wxStaticText (this, -1, x)
#define spacer() box1gridSizer->Add (1, 1);
#define newbrowse() (browseBtn[n_browse++] = new wxButton (this, ID_Browse, BTNLABEL_BROWSE))
#define newlongtext() (new wxTextCtrl (this, -1, "", wxDefaultPosition, longTextSize))
  addrt (newlabel (box1_label[0]));
  add (megs = new wxSpinCtrl (this, -1));
  spacer();
  addrt (newlabel (box1_label[1]));
  add (biosImage = newlongtext ());
  add (newbrowse ());
  addrt (newlabel (box1_label[2]));
  add (biosAddr = new wxTextCtrl (this, -1));
  spacer();
  addrt (newlabel (box1_label[3]));
  add (vgabiosImage = newlongtext ());
  add (newbrowse ());
  addrt (newlabel (box1_label[4]));
  add (newlabel (box1_label[5]));
#undef add
#undef addrt
#undef newlabel
#undef spacer

  // box2 contents
  box2gridSizer = new wxFlexGridSizer (3);
  box2sizer->Add (box2gridSizer, 0, wxALL, insideStaticBoxMargin);
#define add(x) box2gridSizer->Add (x, 0, wxALL, 2)
#define addrt(x) box2gridSizer->Add (x, 0, wxALL|wxALIGN_RIGHT, 2)
#define newlabel(x) new wxStaticText (this, -1, x)
#define spacer() box2gridSizer->Add (1, 1);
  for (int i=0; i<CONFIG_MEMORY_N_ROMS; i++) {
    addrt (newlabel (box2_label[2*i]));
    add (rom[i] = newlongtext ());
    add (newbrowse ());
    addrt (newlabel (box2_label[2*i + 1]));
    add (romAddr[i] = new wxTextCtrl (this, -1));
    spacer();
  }
#undef add
#undef addrt
#undef newlabel
#undef spacer
#undef newbrowse
#undef newlongtext

  // buttonSizer contents
  wxButton *btn;
  btn = new wxButton (this, wxID_HELP, BTNLABEL_HELP);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxID_OK, BTNLABEL_OK);
  buttonSizer->Add (btn, 0, wxALL, 5);
}

void ConfigMemoryDialog::Init()
{
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(mainSizer);
  mainSizer->Fit (this);
  wxSize size = mainSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void ConfigMemoryDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  wxLogMessage ("you pressed button id=%d", id);
  switch (id) {
    case ID_Browse:
      {
	// There are several browse buttons.  Figure out which one was
	// pressed, and which text control is next to it.
	wxTextCtrl *text = NULL;
	wxObject *source = event.GetEventObject ();
	for (int i=0; i<CONFIG_MEMORY_N_BROWSES; i++) {
	  if (source == browseBtn[i]) {
	    switch (i) {
	      case 0: text = biosImage; break;
	      case 1: text = vgabiosImage; break;
	      case 2: case 3: case 4: case 5:
		text = rom[i-2];
		break;
	    }
	    break;
	  }
	}
	if (!text) return;  // not recognized from browse button array
	BrowseTextCtrl (text);
      }
      break;
    case wxID_OK:
      {
	// test validity of the integer fields
	bool valid;
	GetTextCtrlInt (biosAddr, &valid, true, "Invalid ROM BIOS Address");
	if (!valid) return;
	for (int rom=0; rom<CONFIG_MEMORY_N_ROMS; rom++) {
	  GetTextCtrlInt (romAddr[rom], &valid, true, "Invalid Optional ROM address");
	  if (!valid) return;
	}
      }
      EndModal (wxID_OK);
      break;
    case wxID_CANCEL:
      EndModal (wxID_CANCEL);
      break;
    case wxID_HELP:
      ShowHelp(); 
      break;
    default:
      event.Skip ();
  }
}

void ConfigMemoryDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}

#if BX_DEBUGGER
//////////////////////////////////////////////////////////////////////
// DebugLogDialog implementation
//////////////////////////////////////////////////////////////////////
// Structure:
//   mainSizer:
//     wxTextCtrl log (multiline with vert scrollbar)
//     "Type a debugger command"
//     commandSizer:
//       wxTextCtrl command
//       Execute button
//     buttonSizer:
//       help
//       cancel
//       ok
//

// all events go to OnEvent method
BEGIN_EVENT_TABLE(DebugLogDialog, wxDialog)
  EVT_BUTTON(-1, DebugLogDialog::OnEvent)
  EVT_CHECKBOX(-1, DebugLogDialog::OnEvent)
  EVT_KEY_DOWN(DebugLogDialog::OnKeyEvent)
  EVT_KEY_UP(DebugLogDialog::OnKeyEvent)
  EVT_CHAR(DebugLogDialog::OnEvent)
  EVT_TEXT(-1, DebugLogDialog::OnEvent)
  EVT_TEXT_ENTER(-1, DebugLogDialog::OnEnterEvent)
END_EVENT_TABLE()


DebugLogDialog::DebugLogDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  SetTitle (DEBUG_LOG_TITLE);
  mainSizer = new wxBoxSizer (wxVERTICAL);
  log = new wxTextCtrl (this, -1, "",
      wxDefaultPosition, wxSize(400, 300), 
      wxTE_MULTILINE | wxTE_RICH | wxTE_READONLY);
  mainSizer->Add (log, 1, wxALL|wxGROW, 10);
  wxStaticText *text = new wxStaticText (this, -1, DEBUG_CMD_PROMPT);
  mainSizer->Add (text, 0, wxTOP|wxLEFT, 10);
  commandSizer = new wxBoxSizer (wxHORIZONTAL);
  mainSizer->Add (commandSizer, 0, wxALL|wxGROW, 5);
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);
  mainSizer->Add (buttonSizer, 0, wxALIGN_RIGHT);

  // commandSizer contents
  command = new wxTextCtrl (this, ID_DebugCommand, "", 
      wxDefaultPosition, wxDefaultSize, 
      wxTE_PROCESS_ENTER);
  commandSizer->Add (command, 1, wxGROW);
  wxButton *btn;
  btn = new wxButton (this, ID_Execute, BTNLABEL_EXECUTE);
  commandSizer->Add (btn, 0, wxALL, 5);

  // buttonSizer contents
  btn = new wxButton (this, wxID_OK, BTNLABEL_CLOSE);
  buttonSizer->Add (btn, 0, wxALL, 5);
}

void DebugLogDialog::Init()
{
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(mainSizer);
  mainSizer->Fit (this);
  wxSize size = mainSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void DebugLogDialog::Execute(bool clear)
{
  // send to debugger
  theFrame->DebugCommand (command->GetValue ());
  // display what they typed on the log screen
  if (clear) command->Clear ();
}

void DebugLogDialog::AppendCommand (const char *cmd)
{
  log->AppendText (wxT(">>> "));
  log->AppendText (wxString (cmd));
  log->AppendText (wxT("\n"));
  int n = log->GetLastPosition ();
  if (n>0) n--;
  log->ShowPosition (n);
}

void DebugLogDialog::AppendText (wxString text) {
  log->AppendText (text);
  int n = log->GetLastPosition ();
  if (n>0) n--;
  log->ShowPosition (n);
}

void DebugLogDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  //wxLogMessage ("event was from id=%d, type=%d", id, (int)event.GetEventType ());
  switch (id) {
    case wxID_OK:
      Show(FALSE);
      break;
    case ID_Execute: // pressed execute button
      Execute (false);
      break;
    default:
      event.Skip ();
  }
}

void DebugLogDialog::OnKeyEvent(wxKeyEvent& event)
{
  wxLogDebug ("key event");
}
#endif

/////////////////////////////////////////////////////////////////
// ParamDialog
/////////////////////////////////////////////////////////////////

// all events go to OnEvent method
BEGIN_EVENT_TABLE(ParamDialog, wxDialog)
  EVT_BUTTON(-1, ParamDialog::OnEvent)
  EVT_CHECKBOX(-1, ParamDialog::OnEvent)
  EVT_CHOICE(-1, ParamDialog::OnEvent)
  EVT_TEXT(-1, ParamDialog::OnEvent)
END_EVENT_TABLE()

ParamDialog::ParamDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, "", wxDefaultPosition, wxDefaultSize, 
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  idHash = new wxHashTable (wxKEY_INTEGER);
  paramHash = new wxHashTable (wxKEY_INTEGER);
  nbuttons = 0;

  // top level objects
  mainSizer = new wxBoxSizer (wxVERTICAL);

  // create buttonSizer, which will hold all the buttons.
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);
}

ParamDialog::~ParamDialog()
{
  paramHash->BeginFind ();
  wxNode *node;
  while ((node = paramHash->Next ()) != NULL) {
    // assume that no ParamStruct appears in the hash table under multiple
    // keys.  If so, we will delete it twice and corrupt memory.
    ParamStruct *pstr = (ParamStruct*) node->GetData ();
    //wxLogDebug ("deleting ParamStruct id=%d for param %s", pstr->id, pstr->param->get_name ());
    delete pstr;
  }
  delete idHash;
  delete paramHash;
}

wxButton*
ParamDialog::AddButton (int id, wxString label)
{
  wxButton *btn = new wxButton (this, id, label);
  buttonSizer->Add (btn, 0, wxALL, 5);
  nbuttons++;
  return btn;
}

// add the standard HELP, CANCEL, OK buttons.
void ParamDialog::AddDefaultButtons ()
{
  AddButton (wxID_HELP, BTNLABEL_HELP);
  AddButton (wxID_CANCEL, BTNLABEL_CANCEL);
  AddButton (wxID_OK, BTNLABEL_OK);
}

void ParamDialog::Init()
{
  // if nobody has made any buttons, then create some now
  if (nbuttons==0) AddDefaultButtons ();
  mainSizer->Add (buttonSizer, 0, wxALIGN_RIGHT);
  EnableChanged ();
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(mainSizer);
  mainSizer->Fit (this);
  wxSize size = mainSizer->GetMinSize ();
  wxLogMessage ("minsize is %d,%d", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

static int _next_id = ID_LAST_USER_DEFINED;

int ParamDialog::genId () {
  return ++_next_id;
}

bool ParamDialog::isGeneratedId (int id) { 
  return (id >= ID_LAST_USER_DEFINED && id < _next_id);
}

void ParamDialog::AddParamList (bx_id *idList, wxFlexGridSizer *sizer, bool plain)
{
  bx_id *idptr;
  for (idptr = idList; *idptr != BXP_NULL; idptr++) {
    bx_param_c *param = SIM->get_param (*idptr);
    if (param != NULL) {
      AddParam (param, sizer, plain);
    }
  }
}

// support "legacy" addparam functions. Easier than changing them.
void ParamDialog::AddParam (bx_param_c *param, wxFlexGridSizer *sizer, bool plain)
{
  AddParamContext context;
  context.depth = 0;
  context.parent = this;
  context.vertSizer = mainSizer;
  context.gridSizer = sizer;
  AddParam (param, plain, &context);
}

void ParamDialog::AddParam (
    bx_param_c *param_generic,
    bool plain,
    AddParamContext *context) 
{
  AddParamContext defaultContext;
  if (context == NULL) {
    context = &defaultContext;
    context->depth = 0;
    context->parent = this;
    context->vertSizer = mainSizer;
    context->gridSizer = NULL;
  }
  wxASSERT (context->parent != NULL);
  wxASSERT (context->vertSizer != NULL);
  if (param_generic == NULL) 
    return;  // param not registered, probably this option was not compiled in
  wxLogDebug ("AddParam for param '%s'", param_generic->get_name ());
  if (context->gridSizer == NULL) {
    // create a gridSizer if none exists yet.  add it to default vertSizer.  
    context->gridSizer = new wxFlexGridSizer (3);
    context->vertSizer->Add (context->gridSizer);
  }
  wxFlexGridSizer *sizer = context->gridSizer;

  ParamStruct *pstr = new ParamStruct ();
  pstr->param = param_generic;
  pstr->id = genId ();
  pstr->label = NULL;
  pstr->u.window = NULL;
  pstr->browseButton = NULL;
  int type = param_generic->get_type ();
  char *prompt = pstr->param->get_ask_format ();
  if (!prompt) prompt = pstr->param->get_name ();
  wxASSERT (prompt != NULL);
#define ADD_LABEL(x) sizer->Add (pstr->label = new wxStaticText (context->parent, -1, wxString (x)), 0, wxALIGN_RIGHT|wxALL, 3)
  switch (type) {
    case BXT_PARAM_BOOL: {
	bx_param_bool_c *param = (bx_param_bool_c*) param_generic;
	if (!plain) ADD_LABEL (prompt);
	wxCheckBox *ckbx = new wxCheckBox (context->parent, pstr->id, "");
	ckbx->SetValue (param->get ());
	sizer->Add (ckbx);
	if (!plain) sizer->Add (1, 1);  // spacer
	pstr->u.checkbox = ckbx;
	idHash->Put (pstr->id, pstr);
	paramHash->Put (pstr->param->get_id (), pstr);
        break;
      }
    case BXT_PARAM_NUM: {
	bx_param_num_c *param = (bx_param_num_c*) param_generic;
	if (!plain) ADD_LABEL (prompt);
	wxTextCtrl *textctrl = new wxTextCtrl (context->parent, pstr->id, "", wxDefaultPosition, normalTextSize);
	const char *format = param->get_format ();
	if (!format)
	  format = strdup(param->get_base () == 16 ? "0x%X" : "%d");
	SetTextCtrl (textctrl, format, param->get ());
	sizer->Add (textctrl);
	if (!plain) sizer->Add (1, 1);  // spacer
	pstr->u.text = textctrl;
	idHash->Put (pstr->id, pstr);
	paramHash->Put (pstr->param->get_id (), pstr);
        break;
      }
    case BXT_PARAM_ENUM: {
	bx_param_enum_c *param = (bx_param_enum_c*) param_generic;
	if (!plain) ADD_LABEL (prompt);
	wxChoice *choice = new wxChoice (context->parent, pstr->id);
	sizer->Add (choice, 0, wxADJUST_MINSIZE);
	if (!plain) sizer->Add (1, 1);  // spacer
	// fill in the choices
	int i=0;
	char *ptr;
	while (NULL != (ptr = param->get_choice (i++)))
	  choice->Append (ptr);
	choice->SetSelection (param->get() - param->get_min ());
	pstr->u.choice = choice;
	idHash->Put (pstr->id, pstr);
	paramHash->Put (pstr->param->get_id (), pstr);
        break;
      }
    case BXT_PARAM_STRING: {
	bx_param_string_c *param = (bx_param_string_c*) param_generic;
	if (!plain) ADD_LABEL (prompt);
	bool isFilename = param->get_options ()->get () & param->IS_FILENAME;
	wxTextCtrl *txtctrl = new wxTextCtrl (context->parent, pstr->id, "", wxDefaultPosition, isFilename? longTextSize : normalTextSize);
	txtctrl->SetValue (param->getptr ());
	sizer->Add (txtctrl);
	if (!plain) {
	  if (isFilename) {
	    // create Browse button
	    pstr->browseButtonId = genId ();
	    pstr->browseButton = new wxButton (context->parent, 
		pstr->browseButtonId, BTNLABEL_BROWSE);
	    sizer->Add (pstr->browseButton, 0, wxALL, 5);
	    idHash->Put (pstr->browseButtonId, pstr);  // register under button id
	  } else {
	    sizer->Add (1, 1);  // spacer
	  }
	}
	pstr->u.text = txtctrl;
	idHash->Put (pstr->id, pstr);
	paramHash->Put (pstr->param->get_id (), pstr);
        break;
      }
    case BXT_LIST: {
	bx_list_c *list = (bx_list_c*) param_generic;
	if (list->get_options()->get () & bx_list_c::USE_TAB_WINDOW) {
	  // put each item in a separate tab of a tabbed window
	  wxNotebook *notebook = new wxNotebook (context->parent, -1);
	  wxNotebookSizer *nbsizer = new wxNotebookSizer (notebook);
	  // put all items in the list into a separate page of the notebook.
	  for (int i=0; i<list->get_size (); i++) {
	    bx_param_c *child = list->get (i);
	    wxASSERT (child->get_type() == BXT_LIST);
	    // the child must be a list!  I could support other things but
	    // I don't see any reason to.  It wouldn't make sense to devote
	    // a whole tab to a single parameter.
	    wxPanel *panel = new wxPanel (notebook);
	    wxBoxSizer *boxsz = new wxBoxSizer (wxVERTICAL);
	    AddParamContext newcontext;
	    newcontext.depth = 1 + context->depth;
	    newcontext.parent = panel;
	    newcontext.vertSizer = boxsz;
	    newcontext.gridSizer = NULL; // will be created if needed
	    // the child itself is a list. Add the child's children in
	    // this new context.
	    bx_list_c *childl = (bx_list_c *)child;
	    for (int j=0; j<childl->get_size(); j++)
	      AddParam (childl->get(j), plain, &newcontext);
	    const char *pagename = child->get_ask_format ();
	    if (!pagename) pagename = child->get_name ();
	    panel->SetAutoLayout (TRUE);
	    panel->SetSizer (boxsz);
	    notebook->AddPage (panel, wxString(pagename));
	  }
	  context->vertSizer->Add (nbsizer, 0, wxALL|wxGROW, 10);
	  // clear gridSizer variable so that any future parameters force
	  // creation of a new one.
	  context->gridSizer = NULL;
	  // add to hashes
	  pstr->u.notebook = notebook;
	  idHash->Put (pstr->id, pstr);
	  paramHash->Put (pstr->param->get_id (), pstr);
	} else {
	  wxStaticBox *box = new wxStaticBox (context->parent, -1, prompt);
	  wxStaticBoxSizer *boxsz = new wxStaticBoxSizer (box, wxVERTICAL);
	  AddParamContext newcontext;
	  newcontext.depth = 1 + context->depth;
	  newcontext.parent = context->parent;
	  newcontext.gridSizer = NULL; // it will be created if necessary
	  newcontext.vertSizer = boxsz;
	  // put all items in the list inside the boxsz sizer.
	  for (int i=0; i<list->get_size (); i++) {
	    bx_param_c *child = list->get (i);
	    AddParam (child, plain, &newcontext);
	  }
	  // add the boxsz to vertSizer
	  context->vertSizer->Add (boxsz, 0, wxALL|wxGROW, 10);
	  // clear gridSizer variable so that any future parameters force
	  // creation of a new one.
	  context->gridSizer = NULL;
	  // add to hashes
	  pstr->u.staticbox = box;
	  idHash->Put (pstr->id, pstr);
	  paramHash->Put (pstr->param->get_id (), pstr);
	}
	break;
      }
    default:
      wxLogError ("ParamDialog::AddParam called with unsupported param type id=%d", (int)type);
  }
}

bool ParamDialog::CopyGuiToParam ()
{
  // loop through all the parameters
  idHash->BeginFind ();
  wxNode *node;
  while ((node = idHash->Next ()) != NULL) {
    ParamStruct *pstr = (ParamStruct*) node->GetData ();
    wxLogDebug ("commit changes for param %s", pstr->param->get_name ());
    int type = pstr->param->get_type ();
    switch (type) {
      case BXT_PARAM_BOOL: {
        bx_param_bool_c *boolp = (bx_param_bool_c*) pstr->param;
        boolp->set (pstr->u.checkbox->GetValue ());
	break;
        }
      case BXT_PARAM_NUM: {
        bx_param_num_c *nump = (bx_param_num_c*) pstr->param;
	bool valid;
	wxString complaint;
	complaint.Printf ("Invalid integer for %s.", pstr->param->get_name ());
	int n = GetTextCtrlInt (pstr->u.text, &valid, true, complaint);
        nump->set (n);
	break;
        }
      case BXT_PARAM_ENUM: {
        bx_param_enum_c *enump = (bx_param_enum_c*) pstr->param;
        enump->set (pstr->u.choice->GetSelection () + enump->get_min ());
	break;
        }
      case BXT_PARAM_STRING: {
        bx_param_string_c *stringp = (bx_param_string_c*) pstr->param;
	char buf[1024];
	wxString tmp(pstr->u.text->GetValue ());
	strncpy (buf, tmp.c_str(), sizeof(buf));
	stringp->set (buf);
	break;
        }
      case BXT_LIST:
        break;
      default:
        wxLogError ("ParamDialog::CopyGuiToParam: unsupported param type id=%d", (int)type);
    }
  }
  return true;
}

void ParamDialog::EnableChanged ()
{
  idHash->BeginFind ();
  wxNode *node;
  while ((node = idHash->Next ()) != NULL) {
    ParamStruct *pstr = (ParamStruct*) node->GetData ();
    if (pstr->param->get_type () == BXT_PARAM_BOOL)
      EnableChanged (pstr);
    // special cases that can't be handled in the usual way
  }
}

void ParamDialog::EnableChanged (ParamStruct *pstrOfCheckbox)
{
  wxLogDebug ("EnableChanged on checkbox %s", pstrOfCheckbox->param->get_name ());
  bx_param_bool_c *enableParam = (bx_param_bool_c*) pstrOfCheckbox->param;
  wxASSERT (enableParam->get_type () == BXT_PARAM_BOOL); // or we wouldn't be here
  bool en = pstrOfCheckbox->u.checkbox->GetValue ();
  EnableChangedRecursive (enableParam->get_dependent_list (), en, pstrOfCheckbox);
}

void ParamDialog::EnableChangedRecursive (
    bx_list_c *list,
    bool en,
    ParamStruct *pstrOfCheckbox)
{
  if (list==NULL) return;
  for (int i=0; i<list->get_size (); i++) {
    bx_param_c *param = list->get(i);
    ParamStruct *pstr = (ParamStruct*) paramHash->Get (param->get_id ());
    if (pstr) {
      if (param == pstrOfCheckbox->param) {
	wxLogDebug ("not setting enable on checkbox '%s' that triggered the enable change", pstrOfCheckbox->param->get_name ());
	continue;
      }
      wxLogDebug ("setting enable for param '%s' to %d", pstr->param->get_name (), en?1:0);
      if (en != pstr->u.window->IsEnabled ()) {
	EnableParam (pstr->param->get_id (), en);
	//pstr->u.window->Enable (en);
	//if (pstr->browseButton) pstr->browseButton->Enable (en);
	//if (pstr->label) pstr->label->Enable (en);
	bx_list_c *deps = pstr->param->get_dependent_list ();
	if (deps) {
	  wxLogDebug ("recursing on dependent list of %s", list->get_name ());
	  wxASSERT (pstr->param->get_type () == BXT_PARAM_BOOL);
          bool dep_en = pstr->u.window->IsEnabled () && pstr->u.checkbox->GetValue ();
	  EnableChangedRecursive (deps, dep_en, pstr);
	}
      }
    }
  }
  // if any enums changed, give them a chance to update
  for (int i=0; i<list->get_size (); i++) {
    bx_param_c *param = list->get(i);
    ParamStruct *pstr = (ParamStruct*) paramHash->Get (param->get_id ());
    if (pstr) {
      if (pstr->param->get_type () == BXT_PARAM_ENUM)
	EnumChanged (pstr);
    }
  }
}

void ParamDialog::EnableParam (int param_id, bool enabled)
{
  ParamStruct *pstr = (ParamStruct*) paramHash->Get (param_id);
  if (!pstr) return;
  if (pstr->label) pstr->label->Enable (enabled);
  if (pstr->browseButton) pstr->browseButton->Enable (enabled);
  if (pstr->u.window) pstr->u.window->Enable (enabled);
}

void ParamDialog::EnumChanged (ParamStruct *pstr)
{
  wxLogDebug ("EnumChanged");
  int id = pstr->param->get_id ();
  switch (id) {
    case BXP_ATA0_MASTER_TYPE:
    case BXP_ATA0_SLAVE_TYPE:
    case BXP_ATA1_MASTER_TYPE:
    case BXP_ATA1_SLAVE_TYPE:
    case BXP_ATA2_MASTER_TYPE:
    case BXP_ATA2_SLAVE_TYPE:
    case BXP_ATA3_MASTER_TYPE:
    case BXP_ATA3_SLAVE_TYPE: {
      int delta = id - BXP_ATA0_MASTER_TYPE;
      // find out if "present" checkbox is checked
      bx_id present_id = (bx_id) (BXP_ATA0_MASTER_PRESENT+delta);
      ParamStruct *present = (ParamStruct*) paramHash->Get (present_id);
      wxASSERT (present && present->param->get_type () == BXT_PARAM_BOOL);
      if (!present->u.checkbox->GetValue ())
	break;  // device not enabled, leave it alone
      if (!present->u.checkbox->IsEnabled ())
	break;  // enable button for the device is not enabled
      wxASSERT (pstr->param->get_type () == BXT_PARAM_ENUM);
      int type = pstr->u.choice->GetSelection ();
      if (type == BX_ATA_DEVICE_DISK) {
	// enable cylinders, heads, spt
	wxLogDebug ("enabling disk parameters");
	EnableParam (BXP_ATA0_MASTER_CYLINDERS+delta, 1);
	EnableParam (BXP_ATA0_MASTER_HEADS+delta, 1);
	EnableParam (BXP_ATA0_MASTER_SPT+delta, 1);
	EnableParam (BXP_ATA0_MASTER_STATUS+delta, 0);
      } else {
	// enable inserted
	wxLogDebug ("enabling cdrom parameters");
	EnableParam (BXP_ATA0_MASTER_CYLINDERS+delta, 0);
	EnableParam (BXP_ATA0_MASTER_HEADS+delta, 0);
	EnableParam (BXP_ATA0_MASTER_SPT+delta, 0);
	EnableParam (BXP_ATA0_MASTER_STATUS+delta, 1);
      }
    }
  }
}

// if any parameters changed, update the associated control
void ParamDialog::CopyParamToGui ()
{
  // loop through all the parameters
  idHash->BeginFind ();
  wxNode *node;
  while ((node = idHash->Next ()) != NULL) {
    ParamStruct *pstr = (ParamStruct*) node->GetData ();
    IFDBG_DLG (wxLogDebug ("refresh param %s", pstr->param->get_name ()));
    int type = pstr->param->get_type ();
    switch (type) {
      case BXT_PARAM_BOOL: {
        bx_param_bool_c *boolp = (bx_param_bool_c*) pstr->param;
	pstr->u.checkbox->SetValue (boolp->get ());
	break;
        }
      case BXT_PARAM_NUM: {
        bx_param_num_c *nump = (bx_param_num_c*) pstr->param;
	const char *format = nump->get_format ();
	if (!format)
	  format = strdup(nump->get_base () == 16 ? "0x%X" : "%d");
	SetTextCtrl (pstr->u.text, format, nump->get ());
	break;
        }
      case BXT_PARAM_ENUM: {
        bx_param_enum_c *enump = (bx_param_enum_c*) pstr->param;
	pstr->u.choice->SetSelection (enump->get () - enump->get_min ());
	break;
        }
      case BXT_PARAM_STRING: {
        bx_param_string_c *stringp = (bx_param_string_c*) pstr->param;
	pstr->u.text->SetValue (wxString (stringp->getptr ()));
	break;
        }
      case BXT_LIST:
        break;
      default:
        wxLogError ("ParamDialog::CopyParamToGui(): unsupported param type id=%d", (int)type);
    }
  }
}

void ParamDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  //wxLogMessage ("event was from id=%d", id);
  if (isGeneratedId (id)) {
    ParamStruct *pstr = (ParamStruct*) idHash->Get (id);
    if (pstr == NULL) {
      wxLogDebug ("ParamStruct not found for id=%d", id);
      return;
    }
    if (id == pstr->id) {
      IFDBG_DLG (wxLogDebug ("event came from window %p (id=%d) controlled by parameter '%s'", pstr->u.window, id, pstr->param->get_name ()));
      switch (pstr->param->get_type ()) {
	case BXT_PARAM_BOOL:
	  EnableChanged (pstr);
	  break;
	case BXT_PARAM_ENUM:
	  EnumChanged (pstr);
	  break;
      }
      return;
    }
    if (id == pstr->browseButtonId) {
      wxLogDebug ("browse button id=%d attached to wxTextCtrl %p", id, pstr->u.text);
      BrowseTextCtrl (pstr->u.text);
      return;
    }
    wxLogDebug ("id was key to ParamStruct but doesn't match either id inside");
  }
  switch (id) {
    case wxID_OK:
      if (IsModal ()) {
        if (CopyGuiToParam ())
          EndModal (wxID_OK);
      } else {
	CopyParamToGui ();
      }
      break;
    case wxID_CANCEL:
      if (IsModal ())
	EndModal (wxID_CANCEL);
      else
	Show (FALSE);
      break;
    case wxID_HELP:
      ShowHelp(); 
      break;
    default:
      event.Skip ();
  }
}

void ParamDialog::ShowHelp ()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR );
}

/////////////////////////////////////////////////////////////////
// CpuRegistersDialog
/////////////////////////////////////////////////////////////////
// Structure:
// - mainSizer
//   - mainRegsSizer (grid or flexgrid)
//     - col0: flexgrid
//       - params from EAX to ESP
//     - col1: flexgrid
//       - params from EIP to EFLAGS
//     - col2: flexgrid
//       - params from LDTR to IDTR limit
//   - flagsSizer
//   - extRegsSizer
//     - col0: flexgrid
//       - DR* params
//     - col1: flexgrid
//       - TR* params
//     - col2: flexgrid
//       - CR* params

// all events go to OnEvent method
BEGIN_EVENT_TABLE(CpuRegistersDialog, wxDialog)
  EVT_BUTTON(-1, CpuRegistersDialog::OnEvent)
  EVT_CHECKBOX(-1, CpuRegistersDialog::OnEvent)
  EVT_TEXT(-1, CpuRegistersDialog::OnEvent)
END_EVENT_TABLE()

CpuRegistersDialog::CpuRegistersDialog(
    wxWindow* parent,
    wxWindowID id)
  : ParamDialog (parent, id)
{
  wxFlexGridSizer *column;
  nflags = 0;
  bx_id mainRegList1[] = CPU_REGS_MAIN_REGS1;
  bx_id mainRegList2[] = CPU_REGS_MAIN_REGS2;
  bx_id mainRegList3[] = CPU_REGS_MAIN_REGS3;
  bx_id flagList[]     = CPU_REGS_FLAGS;
  bx_id controlList[]  = CPU_REGS_CONTROL_REGS;
  bx_id debugList[]    = CPU_REGS_DEBUG_REGS;
  bx_id testList[]     = CPU_REGS_TEST_REGS;
  bx_id *idptr;

  // top level objects
  wxStaticBox *mainRegsBox = new wxStaticBox (this, -1, "Basic Registers");
  wxStaticBoxSizer *mainRegsBoxSizer = 
    new wxStaticBoxSizer (mainRegsBox, wxVERTICAL);
  mainSizer->Add (mainRegsBoxSizer, 0, wxALL|wxGROW, 10);

  wxStaticBox *flagsBox = new wxStaticBox (this, -1, "EFLAGS Bits");
  wxStaticBoxSizer *flagsBoxSizer = 
    new wxStaticBoxSizer (flagsBox, wxVERTICAL);
  mainSizer->Add (flagsBoxSizer, 0, wxALL|wxGROW, 10);

  wxStaticBox *otherBox = new wxStaticBox (this, -1, "Other Registers");
  wxStaticBoxSizer *otherBoxSizer = 
    new wxStaticBoxSizer (otherBox, wxVERTICAL);
  mainSizer->Add (otherBoxSizer, 0, wxALL|wxGROW, 10);

  // mainRegsSizer contents
  mainRegsSizer = new wxFlexGridSizer (3);
  mainRegsBoxSizer->Add (mainRegsSizer, 0, wxALL, 3);
  column = new wxFlexGridSizer (3);
  mainRegsSizer->Add (column, 0, wxALL, 10);
  AddParamList (mainRegList1, column);

  column = new wxFlexGridSizer (3);
  mainRegsSizer->Add (column, 0, wxALL, 10);
  AddParamList (mainRegList2, column);

  column = new wxFlexGridSizer (3);
  mainRegsSizer->Add (column, 0, wxALL, 10);
  AddParamList (mainRegList3, column);

  // add flag parameters
  flagsSizer = new wxFlexGridSizer (CPU_REGS_MAX_FLAGS);
  flagsBoxSizer->Add (flagsSizer, 0, wxALL | wxALIGN_CENTER, 3);
  for (idptr = flagList; *idptr != BXP_NULL; idptr++)
    AddFlag (*idptr);

  // extRegsSizer contents
  extRegsSizer = new wxFlexGridSizer (3);
  otherBoxSizer->Add (extRegsSizer, 0, wxALL, 3);

  column = new wxFlexGridSizer (3);
  extRegsSizer->Add (column, 0, wxALL, 10);
  AddParamList (controlList, column);

  column = new wxFlexGridSizer (3);
  extRegsSizer->Add (column, 0, wxALL, 10);
  AddParamList (debugList, column);

  column = new wxFlexGridSizer (3);
  extRegsSizer->Add (column, 0, wxALL, 10);
  AddParamList (testList, column);

  // add buttons
#if BX_DEBUGGER
  // only show these if debugger is enabled
  contButton = AddButton (ID_Debug_Continue, BTNLABEL_DEBUG_CONTINUE);
  stopButton = AddButton (ID_Debug_Stop, BTNLABEL_DEBUG_STOP);
  stepButton = AddButton (ID_Debug_Step, BTNLABEL_DEBUG_STEP);
  //commitButton = AddButton (ID_Debug_Commit, BTNLABEL_DEBUG_COMMIT);
#endif
  AddButton (ID_Close, BTNLABEL_CLOSE);
}

void
CpuRegistersDialog::AddFlag (bx_id paramId)
{
  if (SIM->get_param (paramId) == NULL) {
    wxLogDebug ("AddFlag on unregistered param id=%d", (int)paramId);
    return;
  }
  wxASSERT (nflags < CPU_REGS_MAX_FLAGS);
  flagid[nflags++] = paramId;
}

void
CpuRegistersDialog::Init ()
{
  int i;
  for (i=0; i<CPU_REGS_MAX_FLAGS; i++) {
    if (i<nflags) {
      bx_param_c *param = SIM->get_param (flagid[i]);
      flagsSizer->Add (new wxStaticText (this, -1, param->get_name ()), 0, wxALL|wxALIGN_LEFT, 4);
    } else {
      flagsSizer->Add (0, 0);  // spacer
    }
  }
  for (i=0; i<nflags; i++) {
    bx_param_c *param = SIM->get_param (flagid[i]);
    AddParam (param, flagsSizer, true);
  }
  // special case: make IOPL text field small
  ParamStruct *pstr = (ParamStruct*) paramHash->Get (BXP_CPU_EFLAGS_IOPL);
  if (pstr != NULL) {
    wxSize size = pstr->u.text->GetSize ();
    size.SetWidth (size.GetWidth () / 2);
    pstr->u.text->SetSize (size);
    flagsSizer->SetItemMinSize (pstr->u.text, size.GetWidth(), size.GetHeight());
  }
  ParamDialog::Init ();
  stateChanged (false);
}

void
CpuRegistersDialog::stateChanged (bool simRunning)
{
#if BX_DEBUGGER
  contButton->Enable (!simRunning);
  stepButton->Enable (!simRunning);
  stopButton->Enable (simRunning);
#endif
}

void 
CpuRegistersDialog::CopyParamToGui ()
{
  ParamDialog::CopyParamToGui ();
#if BX_DEBUGGER
  stateChanged (SIM->get_param_bool (BXP_DEBUG_RUNNING)->get ());
#endif
}

// How am I going to communicate with the debugger?
// 
// The current model is that the debugger asks you for a command, and
// blocks forever until you press return.  Then it interprets the command,
// however long it takes, and returns to the input loop when the command
// is done.  A control-C can stop a command prematurely.
//
// To extend this into wxWindows multithreaded space, I will create a
// synchronous event called BX_SYNC_GET_DBG_COMMAND which is sent from
// the simulation thread to wxWindows.  When the user chooses a debugger
// action (step, continue, breakpoint, etc.) the simulation awakens and
// interprets the event by calling a function in debug/dbg_main.cc.
//
// The equivalent of a control-C is pressing the "Stop" button during
// a long step or continue command.  This can be implemented by setting
// bx_guard.interrupt_requested = 1, just like the SIGINT handler in
// debug/dbg_main.cc does.
// 
// input loop model is good.  Create a debugger input loop, possibly in
// siminterface.
// in the simulation thread.  This loop waits for a command from the 
// wxWindows debugger
//
// For example, if you press the "Step" button 5
// times, with each click it should call bx_dbg_stepN_command(1) in the
// simulator thread.  When it returns, it goes back to
//
void 
CpuRegistersDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  switch (id) {
    case ID_Close:
      Show (FALSE);
      break;
#if BX_DEBUGGER
  case ID_Debug_Stop:
      wxLogDebug ("wxWindows triggered a break");
      theFrame->DebugBreak ();
      break;
    case ID_Debug_Continue:
      wxLogDebug ("before calling DebugCommand");
      theFrame->DebugCommand ("continue");
      wxLogDebug ("after calling DebugCommand");
      break;
    case ID_Debug_Step:
      theFrame->DebugCommand ("step 1");
      break;
    case ID_Debug_Commit:
      CopyGuiToParam ();
      break;
#endif
    default:
      ParamDialog::OnEvent (event);
  }
}


/////////////////////////////////////////////////////////////////
// utility
/////////////////////////////////////////////////////////////////

// Unfortunately this step is necessary if you change the text of
// a wxStaticText.  Otherwise the sizer that contains the text never realizes
// that the size has changed, and the layout is never updated.  The
// SetItemMinSize trick was reported on comp.soft-sys.wxwindows by 
// Dirk Birnhardt.
void ChangeStaticText (wxSizer *sizer, wxStaticText *win, wxString newtext)
{
  win->SetLabel (newtext);
  wxSize sz = win->GetSize ();
  sizer->SetItemMinSize (win, sz.GetWidth (), sz.GetHeight ());
}

// CreateImage produces a disk image.  It's in the utility function
// area because it's used by both floppy and hard disk image creation.
bool
CreateImage (int harddisk, int sectors, const char *filename)
{
  if (sectors<1) {
    wxMessageBox("The disk size is invalid.", "Invalid Size", wxOK | wxICON_ERROR );
    return false;
  }
  wxLogDebug ("filename = '%s'\n", filename);
  if (strlen (filename) < 1) {
    wxMessageBox("You must type a file name for the new disk image.", "Bad Filename", wxOK | wxICON_ERROR );
    return false;
  }
  // create disk image with name and capacity determined by the filename
  // and sector args.  Call SIM->create_image (filename, sectors, overwrite=0)
  // first which will create the file if it doesn't already exist.  If it
  // exists, it will instead return -1, and we can ask the user "are you sure
  // you want to overwrite?".  If yes, call again with overwrite=1.
  int ret = SIM->create_disk_image (filename, sectors, 0);
  if (ret == -1) {  // already exists
    int answer = wxMessageBox ("File exists.  Do you want to overwrite it?",
      "File exists", wxYES_NO | wxCENTER);
    if (answer == wxYES)
      ret = SIM->create_disk_image (filename, sectors, 1);
    else 
      return false;  // wxNO
  }
  if (ret == -2) {
    wxMessageBox("I could not create the disk image. Check for permission problems or available disk space.", "Failed", wxOK | wxICON_ERROR );
    return false;
  }
  wxASSERT (ret==0);
  return true;
}

void SetTextCtrl (wxTextCtrl *ctrl, const char *format, int val) {
  wxString tmp;
  tmp.Printf (format, val);
  ctrl->SetValue (tmp);
}

int GetTextCtrlInt (wxTextCtrl *ctrl,
    bool *valid,
    bool complain,
    wxString complaint) 
{
  wxString tmp (ctrl->GetValue ());
  char buf[1024];
  strncpy (buf, tmp.c_str(), sizeof(buf));
  int n = strtol (buf, NULL, 0);
  if (n != LONG_MIN && n != LONG_MAX) {
    if (valid) *valid = true;
    return n;
  }
  if (valid) *valid = false;
  if (complain) {
    wxMessageBox(complaint, "Invalid", wxOK | wxICON_ERROR );
    ctrl->SetFocus ();
  }
  return -1;
}

bool BrowseTextCtrl (wxTextCtrl *text, wxString prompt, long style) {
  // try to configure the dialog to show hidden files
  wxConfigBase::Get() -> Write(wxT("/wxWindows/wxFileDialog/ShowHidden"), true);
  wxFileDialog *fdialog = new wxFileDialog (text->GetParent (), prompt, "", text->GetValue (), wxString(), style);
  if (fdialog->ShowModal () == wxID_OK)
    text->SetValue (fdialog->GetPath ());
  delete fdialog;
  return true;
}

wxChoice *makeLogOptionChoiceBox (wxWindow *parent,
    wxWindowID id,
    int evtype,
    bool includeNoChange)
{
  static char *choices[] = LOG_OPTS_CHOICES;
  static int integers[LOG_OPTS_N_CHOICES] = {0, 1, 2, 3, 4};
  static const wxString stupid[2] = { "little1", "little2" };
  wxChoice *control = new wxChoice (parent, id, wxDefaultPosition, wxDefaultSize);
  int lastChoice = 0;  // remember index of last choice
  int nchoice = includeNoChange? LOG_OPTS_N_CHOICES : LOG_OPTS_N_CHOICES_NORMAL;
  for (int choice=0; choice<nchoice; choice++) {
    // the exclude expression allows some choices to not be available
    // for some times.  For example, it would be stupid to ignore a panic.
    if (!LOG_OPTS_EXCLUDE (evtype, choice)) {
      control->Append (choices[choice], &integers[choice]);
      // the client data is an int* that points to the choice number.
      // This is what will be returned by GetAction().
      lastChoice++;
    }
  }
  control->SetSelection (lastChoice-1);
  return control;
}
