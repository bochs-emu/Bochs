/////////////////////////////////////////////////////////////////
// $Id: wxdialog.cc,v 1.12 2002-08-30 07:03:49 bdenney Exp $
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

#include "config.h"              // definitions based on configure script
#include "osdep.h"               // workarounds for missing stuff
#include "gui/siminterface.h"    // interface to the simulator
#include "bxversion.h"           // get version string
#include "wxmain.h"              // wxwindows shared stuff
#include "wxdialog.h"            // custom dialog boxes

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

void ChangeStaticText (wxSizer *sizer, wxStaticText *win, wxString newtext);
bool CreateImage (int harddisk, int sectors, const char *filename);

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
  EVT_BUTTON(wxHELP, LogMsgAskDialog::OnEvent)
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

void LogMsgAskDialog::SetContext (char *s) {
  wxString text;
  text.Printf (LOG_MSG_CONTEXT, s);
  ChangeStaticText (vertSizer, context, text);
}

void LogMsgAskDialog::SetMessage (char *s) {
  wxString text;
  text.Printf (LOG_MSG_MSG, s);
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
  printf ("message size is %d,%d\n", ms.GetWidth(), ms.GetHeight ());
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  printf ("minsize is %d,%d\n", size.GetWidth(), size.GetHeight ());
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
    case wxHELP: ShowHelp (); return;
    default:
      return;  // without EndModal
  }
  printf ("you pressed button id=%d, return value=%d\n", id, ret);
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
  capacitySizer->Add (capacity, 0, wxALL, 5);
  // contents of buttonSizer
  btn = new wxButton (this, wxHELP, "Help");
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, "Cancel");
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, ID_Create, "Create Image");
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxOK, "Ok");
  buttonSizer->Add (btn, 0, wxALL, 5);
  // create filename and diskImageRadioBtn so that we can tweak them before
  // Init comes.  However don't add it to any sizer yet because it needs to go
  // in after the last radio button.
  filename = new wxTextCtrl (this, ID_FilenameText);
  filename->SetSize (300, filename->GetSize ().GetHeight ());
  diskImageRadioBtn = new wxRadioButton (this, ID_Filename, FLOPPY_CONFIG_DISKIMG);

  // the radioSizer contents will be added by successive calls to
  // AddRadio().  The diskImageSizer will be added last, in Init().
}

void FloppyConfigDialog::AddRadio (char *description, char *filename)
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

void FloppyConfigDialog::SetDriveName (const char *name)
{
  wxString text;
  text.Printf (FLOPPY_CONFIG_TITLE, name);
  SetTitle (text);
  text.Printf (FLOPPY_CONFIG_INSTRS, name);
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
  wxButton *btn = new wxButton (this, ID_Browse, "<--Browse");
  diskImageSizer->Add (btn, 0, wxALL, 5);
  radioSizer->Add (diskImageSizer);

  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  printf ("minsize is %d,%d\n", size.GetWidth(), size.GetHeight ());
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

void FloppyConfigDialog::SetFilename (const char *f) {
  // search equivalentFilename[] for matches. if it matches, select the
  // radio button instead.
  for (int i=0; i<n_rbtns; i++) {
    if (!strcmp (f, equivalentFilename[i])) {
      rbtn[i]->SetValue (TRUE);
      return;  // leaving filename text field unchanged
    }
  }
  filename->SetValue (f); 
  diskImageRadioBtn->SetValue (TRUE);
}

char *
FloppyConfigDialog::GetFilename ()
{
  int n = GetRadio ();
  if (n < n_rbtns) {
    return equivalentFilename[n];
  } else {
    return (char *)filename->GetValue ().c_str ();
  }
}

void FloppyConfigDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  printf ("you pressed button id=%d\n", id);
  switch (id) {
    case ID_FilenameText:
      // when you type into the filename field, ensure that the radio
      // button associated with that field is chosen.
      diskImageRadioBtn->SetValue (TRUE);
      break;
    case wxOK:
      // probably should validate before allowing ok
      if (validate!=NULL && !(*validate)(this))
	return;  // validation failed, don't leave yet
      EndModal (0);
      break;
    case ID_Browse:
      {
      long style = wxOPEN;
      wxFileDialog *fdialog = new wxFileDialog (this, filename->GetValue (), "", "", "*.*", style);
      if (fdialog->ShowModal () == wxID_OK)
	SetFilename (fdialog->GetPath().c_str ());
      }
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
      EndModal (-1);
      break;
    case wxHELP:
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
  filename = new wxTextCtrl (this, ID_FilenameText);
  filename->SetSize (300, filename->GetSize ().GetHeight ());
  hsizer[0]->Add (filename, 1);
  wxButton *btn = new wxButton (this, ID_Browse, "<--Browse");
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
  btn = new wxButton (this, wxHELP, BTNLABEL_HELP);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, ID_Create, BTNLABEL_CREATE_IMG);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxOK, BTNLABEL_OK);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit (this);
  wxSize size = vertSizer->GetMinSize ();
  printf ("minsize is %d,%d\n", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void HDConfigDialog::SetDriveName (const char *name)
{
  wxString text;
  text.Printf (HD_CONFIG_TITLE, name);
  SetTitle (text);
}

void HDConfigDialog::SetGeom (int n, int value) {
  printf ("setting geom[%d] to %d\n", n, value);
  geom[n]->SetValue (value); 
  printf ("now geom[%d] has value %d\n", n, geom[n]->GetValue ());
  UpdateMegs ();
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
  printf ("you pressed button id=%d\n", id);
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
    case wxOK:
      // probably should validate before allowing ok
      EndModal (0);
      break;
    case ID_Browse:
      {
      long style = wxOPEN;
      wxFileDialog *fdialog = new wxFileDialog (this, filename->GetValue (), "", "", "*.*", style);
      if (fdialog->ShowModal () == wxID_OK)
	SetFilename (fdialog->GetPath().c_str ());
      }
      break;
    case wxID_CANCEL:
      EndModal (-1);
      break;
    case wxHELP:
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
//       create image
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
  filename = new wxTextCtrl (this, ID_FilenameText);
  filename->SetSize (300, filename->GetSize ().GetHeight ());
  fileSizer->Add (filename, 1, wxLEFT, 5);
  wxButton *btn = new wxButton (this, ID_Browse, "<--Browse");
  fileSizer->Add (btn, 0, wxALL, 5);
  // create buttonSizer & contents but don't add yet
  btn = new wxButton (this, wxHELP, BTNLABEL_HELP);
  buttonSizer->Add (btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton (this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, ID_Create, BTNLABEL_CREATE_IMG);
  buttonSizer->Add (btn, 0, wxALL, 5);
  btn = new wxButton (this, wxOK, BTNLABEL_OK);
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
  printf ("minsize is %d,%d\n", size.GetWidth(), size.GetHeight ());
  int margin = 5;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

void CdromConfigDialog::SetDriveName (const char *name)
{
  wxString text;
  text.Printf (CDROM_CONFIG_TITLE, name);
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

void CdromConfigDialog::SetFilename (const char *f) {
  for (int i=0; i<n_rbtns; i++) {
    if (!strcmp (f, equivalentFilename[i])) {
      rbtn[i]->SetValue (TRUE);
      return;
    }
  }
  filename->SetValue (wxString (f));
}

char *
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
  return (char *)filename->GetValue().c_str ();
}

void 
CdromConfigDialog::AddRadio (const char *description, char *filename)
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
  printf ("you pressed button id=%d\n", id);
  switch (id) {
    case ID_Create:
      {
	int sectors = 650*1024*1024/512;
	char name[1024];
	strncpy (name, filename->GetValue ().c_str (), sizeof(name));
	if (CreateImage (1, sectors, name)) {
	  wxString msg;
	  msg.Printf ("Created a %d megabyte CD-ROM image called '%s'.",
	      sectors*512, name);
	  wxMessageBox(msg, "Image Created", wxOK | wxICON_INFORMATION);
	}
      }
      break;
    case ID_FilenameText:
      // when you type into the filename field, ensure that the radio
      // button associated with that field is chosen.
      diskImageRadioBtn->SetValue (enable->GetValue ());
      break;
    case ID_Enable:
      EnableChanged ();  // enable/disable fields that depend on this
      break;
    case wxOK:
      // probably should validate before allowing ok
      EndModal (0);
      break;
    case ID_Browse:
      {
      long style = wxOPEN;
      wxFileDialog *fdialog = new wxFileDialog (this, filename->GetValue (), "", "", "*.*", style);
      if (fdialog->ShowModal () == wxID_OK)
	SetFilename (fdialog->GetPath().c_str ());
      }
      break;
    case wxID_CANCEL:
      EndModal (-1);
      break;
    case wxHELP:
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


#ifdef TEST_DIALOG
//////////////////////////////////////////////////////////////////////
// MyApp declaration
//////////////////////////////////////////////////////////////////////
class MyApp: public wxApp
{
virtual bool OnInit();
};
IMPLEMENT_APP(MyApp)

//////////////////////////////////////////////////////////////////////
// MyFrame declaration
//////////////////////////////////////////////////////////////////////
class MyFrame: public wxFrame
{
public:
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
  void OnMenuEvent(wxCommandEvent& event);
private:
DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_ShowDialog_1, MyFrame::OnMenuEvent)
  EVT_MENU(ID_ShowDialog_2, MyFrame::OnMenuEvent)
  EVT_MENU(ID_ShowDialog_3, MyFrame::OnMenuEvent)
  EVT_MENU(ID_Quit, MyFrame::OnMenuEvent)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// MyApp methods
//////////////////////////////////////////////////////////////////////

bool MyApp::OnInit()
{
  MyFrame *frame = new MyFrame( "Bochs x86 Emulator", wxPoint(50,50), wxSize(450,340), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION );
  frame->Show( TRUE );
  SetTopWindow( frame );
  return TRUE;
}

//////////////////////////////////////////////////////////////////////
// MyFrame methods
//////////////////////////////////////////////////////////////////////
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
: wxFrame((wxFrame *)NULL, -1, title, pos, size, style)
{
  wxMenu *menu1 = new wxMenu;
  menu1->Append( ID_ShowDialog_1, "Show Dialog &1" );
  menu1->Append( ID_ShowDialog_2, "Show Dialog &2" );
  menu1->Append( ID_ShowDialog_3, "Show Dialog &3" );
  menu1->Append( ID_Quit, "&Quit" );
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menu1, "&File" );
  SetMenuBar( menuBar );
  CreateStatusBar();
  SetStatusText("");
}

void MyFrame::OnMenuEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  switch (id) {
    case ID_ShowDialog_1:
      {
      printf ("show dialog 1\n");
      LogMsgAskDialog dlg(this, -1, "Panic");
      dlg.EnableButton (dlg.DEBUG, FALSE);
      dlg.SetContext ("Hard Drive");
      dlg.SetMessage ("could not open hard drive image file '30M.sample'");
      int n = dlg.ShowModal ();
      printf ("modal dialog returned %d\n", n);
      printf ("and the dontAsk button is %d\n", dlg.GetDontAsk ());
      }
      break;
    case ID_ShowDialog_2:
      printf ("show dialog 2\n");

      break;
    case ID_ShowDialog_3:
      printf ("show dialog 3\n");
      break;
    case ID_Quit:
      Close (TRUE);
      break;
  }
}
#endif
