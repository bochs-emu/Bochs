/////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "config.h"              // definitions based on configure script
#include "param_names.h"

#if BX_WITH_WX

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

#include "osdep.h"               // workarounds for missing stuff
#include "gui/siminterface.h"    // interface to the simulator
#include "bxversion.h"           // get version string
#include "wxdialog.h"            // custom dialog boxes
#include "wxmain.h"              // wxwidgets shared stuff

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
  context = new wxStaticText (this, -1, wxT(""));
  wxFont font = context->GetFont ();
  font.SetWeight (wxBOLD);
  font.SetPointSize (2 + font.GetPointSize ());
  context->SetFont (font);
  message = new wxStaticText (this, -1, wxT(""));
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

void LogMsgAskDialog::SetContext(wxString s)
{
  ChangeStaticText(vertSizer, context, wxString(LOG_MSG_CONTEXT) + s);
}

void LogMsgAskDialog::SetMessage(wxString s)
{
  ChangeStaticText(vertSizer, message, wxString(LOG_MSG_MSG) + s);
}

void LogMsgAskDialog::Init()
{
  static const int ids[N_BUTTONS] = LOG_MSG_ASK_IDS;
  static const wxString names[N_BUTTONS] = LOG_MSG_ASK_NAMES;
  for (int i=0; i<N_BUTTONS; i++) {
    if (!enabled[i]) continue;
    wxButton *btn = new wxButton(this, ids[i], names[i]);
    btnSizer->Add(btn, 1, wxALL, 5);
  }
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit(this);
  wxSize size = vertSizer->GetMinSize();
  int margin = 10;
  SetSizeHints (size.GetWidth () + margin, size.GetHeight () + margin);
  Center ();
}

// Event handler for dialog buttons. Just translate the wx ids into
// enum values and return them with EndModal() to make the dialog
// go away.
void LogMsgAskDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId();
  int ret = -1;
  switch (id) {
    case ID_Continue:   ret = BX_LOG_ASK_CHOICE_CONTINUE;  break;
    case ID_Die:        ret = BX_LOG_ASK_CHOICE_DIE;   break;
    case ID_DumpCore:   ret = BX_LOG_ASK_CHOICE_DUMP_CORE;  break;
    case ID_Debugger:   ret = BX_LOG_ASK_CHOICE_ENTER_DEBUG; break;
    case wxID_HELP: ShowHelp(); return;
    default:
      return;  // without EndModal
  }
  EndModal(ret);
}

void LogMsgAskDialog::ShowHelp()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR, this);
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
  : wxDialog(parent, id, wxT(""), wxDefaultPosition, wxDefaultSize,
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  //static int integers[LOG_OPTS_N_CHOICES_NORMAL] = {0, 1, 2, 3};
  static wxString names[] = ADVLOG_OPTS_TYPE_NAMES;
  SetTitle(ADVLOG_OPTS_TITLE);
  vertSizer = new wxBoxSizer(wxVERTICAL);
  // top level objects
  logfileSizer = new wxBoxSizer(wxHORIZONTAL);
  vertSizer->Add(logfileSizer, 0, wxTOP|wxLEFT, 20);
  wxStaticText *text = new wxStaticText(this, -1, ADVLOG_OPTS_PROMPT);
  vertSizer->Add(text, 0, wxALL, 10);
  applyDefault = new wxButton(this, ID_ApplyDefault, ADVLOG_DEFAULTS);
  vertSizer->Add(applyDefault, 0, wxALL|wxALIGN_RIGHT, 10);
  headerSizer = new wxGridSizer(ADVLOG_OPTS_N_TYPES + 1);
  vertSizer->Add(headerSizer, 0, wxALL|wxGROW, 10);
  scrollWin = new wxScrolledWindow(this, -1);
  vertSizer->Add(scrollWin, 1, wxALL|wxGROW, 10);
  buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  vertSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

  // logfileSizer contents
  text = new wxStaticText(this, -1, ADVLOG_OPTS_LOGFILE);
  logfileSizer->Add(text);
  logfile = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, longTextSize);
  logfileSizer->Add(logfile);
  wxButton *btn = new wxButton(this, ID_Browse, BTNLABEL_BROWSE);
  logfileSizer->Add(btn, 0, wxALL, 5);

  // to get the scrollWin geometry right, first build everything on a wxPanel,
  // with gridSizer as the main sizer.
  scrollPanel = new wxPanel(scrollWin, -1);
  gridSizer = new wxGridSizer(ADVLOG_OPTS_N_TYPES + 1);
  // add title row
  int typemax = ADVLOG_OPTS_N_TYPES;
  text = new wxStaticText(this, -1, wxT("Device"));
  headerSizer->Add(text, 0, wxALIGN_LEFT);
  int type;
  for (type=0; type < typemax; type++) {
    text = new wxStaticText(this, -1, names[type]);
    headerSizer->Add(text, 0, wxALIGN_LEFT);
  }
  // add rows of choice boxes, one for each device
  int devmax = SIM->get_n_log_modules();
  action = new wxChoice** [devmax];   // array of pointers
  for (int dev=0; dev<devmax; dev++) {
    if (strcmp(SIM->get_logfn_name(dev), "?")) {
      action[dev] = new wxChoice* [ADVLOG_OPTS_N_TYPES];
      // name of device in first column
      gridSizer->Add(new wxStaticText(scrollPanel, -1, wxString(SIM->get_logfn_name(dev), wxConvUTF8)));
      // wxChoice in every other column
      for (type=0; type < typemax; type++) {
        action[dev][type] = makeLogOptionChoiceBox(scrollPanel, -1, type);
        gridSizer->Add(action[dev][type], 1, wxALL|wxGROW|wxADJUST_MINSIZE, 2);
      }
    } else {
      action[dev] = NULL;
    }
  }
  headerSizer->Fit(this);
  headerSizer->SetSizeHints(this);
  scrollPanel->SetAutoLayout(TRUE);
  scrollPanel->SetSizer(gridSizer);
  gridSizer->Fit(scrollPanel);
  gridSizer->SetSizeHints(scrollPanel);
  wxSize size = scrollPanel->GetBestSize();
  // now we know how big the panel wants to be, and we can set the scrollbar
  // and scrollWin size accordingly

  // finally set up the scroll window outside
  scrollWin->SetScrollbars(1, 1, size.GetWidth(), size.GetHeight());

  // now that we know the desired width of the panel, use it to set the
  // width of the scrollWin.  I tried several things before arriving at
  // a solution, and I'll list them for educational purposes.
  //
  // failure #1: this had no effect at all. sizer took precedence.
  //   scrollWin->SetSize(500, 500);
  // failure #2: this changed scrollWin size but sizer was not notified so
  //             the overall window didn't expand to make space for it.
  //  scrollWin->SetSizeHints(500, 500);
  // success: tell the sizer to change the scrollWin's size, and it works
  vertSizer->SetItemMinSize(scrollWin, size.GetWidth()+30, 400);

  // buttonSizer contents
  btn = new wxButton(this, wxID_HELP, BTNLABEL_HELP);
  buttonSizer->Add(btn, 0, wxALL, 5);
  // use wxID_CANCEL because pressing ESC produces this same code
  btn = new wxButton(this, wxID_CANCEL, BTNLABEL_CANCEL);
  buttonSizer->Add(btn, 0, wxALL, 5);
  btn = new wxButton(this, wxID_OK, BTNLABEL_OK);
  buttonSizer->Add(btn, 0, wxALL, 5);
}

AdvancedLogOptionsDialog::~AdvancedLogOptionsDialog()
{
  int dev, ndev = SIM->get_n_log_modules();
  for (dev=0; dev<ndev; dev++) {
    delete [] action[dev];
  }
  delete [] action;
}

void AdvancedLogOptionsDialog::Init()
{
  CopyParamToGui();
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(vertSizer);
  vertSizer->Fit(this);
  wxSize size = vertSizer->GetMinSize();
  int margin = 5;
  SetSizeHints(size.GetWidth() + margin, size.GetHeight() + margin);
  Center();
}

void AdvancedLogOptionsDialog::CopyParamToGui()
{
  SIM->apply_log_actions_by_device(); // settings from bochsrc
  bx_param_string_c *logfile = SIM->get_param_string(BXPN_LOG_FILENAME);
  SetLogfile(wxString(logfile->getptr(), wxConvUTF8));
  // copy log action settings from siminterface to gui
  int dev, ndev = SIM->get_n_log_modules();
  int type, ntype = SIM->get_max_log_level();
  for (dev=0; dev<ndev; dev++) {
    for (type=0; type<ntype; type++) {
      SetAction(dev, type, SIM->get_log_action(dev, type));
    }
  }
}

void AdvancedLogOptionsDialog::CopyGuiToParam() {
  char buf[1024];
  safeWxStrcpy(buf, GetLogfile(), sizeof(buf));
  bx_param_string_c *logfile = SIM->get_param_string(BXPN_LOG_FILENAME);
  logfile->set(buf);
  // copy log action settings from gui to siminterface
  int dev, ndev = SIM->get_n_log_modules();
  int type, ntype = SIM->get_max_log_level();
  for (dev=0; dev<ndev; dev++) {
    for (type=0; type<ntype; type++) {
      SIM->set_log_action(dev, type, GetAction(dev, type));
    }
  }
}

void AdvancedLogOptionsDialog::SetAction(int dev, int evtype, int act) {
  if (action[dev] == NULL) return;
  // find the choice whose client data matches "act".
  int *ptr;
  // wxLogDebug(wxT("SetAction dev=%d type=%d act=%d"), dev, evtype, act);
  wxChoice *control = action[dev][evtype];
  for (int i=0; i < (int)control->GetCount(); i++) {
    // wxLogDebug(wxT("reading action[%d][%d]->GetClientData(%d)"), dev, evtype, i);
    ptr = (int*) control->GetClientData(i);
    if (ptr == NULL) continue;
    if (act == *ptr) {  // found it!
      control->SetSelection(i);
      return;
    }
  }
  // this can happen if one of the choices that is excluded by
  // ADVLOG_OPTS_EXCLUDE() is used, for example.
  wxLogDebug(wxT("warning: SetAction type=%d act=%d not found"), evtype, act);
}

int AdvancedLogOptionsDialog::GetAction(int dev, int evtype) {
  if (action[dev] == NULL) return LOG_OPTS_NO_CHANGE;
  int sel = action[dev][evtype]->GetSelection();
  int *ptrToChoice = (int*)action[dev][evtype]->GetClientData(sel);
  wxASSERT(ptrToChoice != NULL);
  return *ptrToChoice;
}

void AdvancedLogOptionsDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId();
  switch (id) {
    case ID_Browse:
      BrowseTextCtrl(logfile);
      break;
    case ID_ApplyDefault: {
      int lev, nlev = SIM->get_max_log_level();
      // copy default settings to every device
      for (lev=0; lev<nlev; lev++) {
	int action = SIM->get_default_log_action(lev);
	int dev, ndev = SIM->get_n_log_modules();
	for (dev=0; dev<ndev; dev++)
	  SetAction(dev, lev, action);
      }
      break;
      }
    case wxID_OK:
      CopyGuiToParam();
      EndModal(wxID_OK);
      break;
    case wxID_CANCEL:
      EndModal(wxID_CANCEL);
      break;
    case wxID_HELP:
      ShowHelp();
      break;
    default:
      event.Skip();
  }
}

void AdvancedLogOptionsDialog::ShowHelp()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR, this);
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
  EVT_CHAR(DebugLogDialog::OnKeyEvent)
  EVT_TEXT(-1, DebugLogDialog::OnEvent)
  EVT_TEXT_ENTER(-1, DebugLogDialog::OnEnterEvent)
END_EVENT_TABLE()


DebugLogDialog::DebugLogDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog (parent, id, wxT(""), wxDefaultPosition, wxDefaultSize,
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  lengthMax = DEBUG_LOG_DEFAULT_LENGTH_MAX;
  lengthTolerance = DEBUG_LOG_DEFAULT_TOLERANCE;
  SetTitle (DEBUG_LOG_TITLE);
  mainSizer = new wxBoxSizer (wxVERTICAL);
  log = new wxTextCtrl (this, -1, wxT(""),
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
  command = new wxTextCtrl (this, ID_DebugCommand, wxT(""),
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
  mainSizer->Fit(this);
  wxSize size = mainSizer->GetMinSize();
  int margin = 5;
  SetSizeHints(size.GetWidth() + margin, size.GetHeight() + margin);
  Center();
}

void DebugLogDialog::Execute(bool clear)
{
  // send to debugger
  theFrame->DebugCommand (command->GetValue ());
  // display what they typed on the log screen
  if (clear) command->Clear ();
}

void DebugLogDialog::CheckLogLength ()
{
  // truncate the text control periodically to avoid a
  // serious memory leak.
  wxString str = log->GetValue ();
  Bit32u len = str.Length ();
  if (len > lengthMax + lengthTolerance) {
    // Truncate the string.  Start from length - lengthMax, search
    // forward until we find the first \n.
    for (int i = len - lengthMax; i<(int)(len-1); i++) {
      if (str.GetChar (i) == '\n') {
	// remove the \n and everything before it.  Done.
	//printf ("truncating from 0 to %d\n", i+1);
	//printf ("\n");
	log->Remove (0, i+1);
	return;
      }
    }
    // no newline found?!
    //printf ("no newline found! truncating from 0 to %d", len - lengthMax);
    //printf ("\n");
    log->Remove (0, len - lengthMax);
  } else {
    //printf ("log length is %d, no truncation yet", len);
    //printf ("\n");
  }
}

void DebugLogDialog::AppendCommand (const char *cmd)
{
  log->AppendText(wxT(">>> "));
  log->AppendText(wxString(cmd, wxConvUTF8));
  log->AppendText(wxT("\n"));
  int n = log->GetLastPosition();
  if (n>0) n--;
  log->ShowPosition(n);
  CheckLogLength();
}

void DebugLogDialog::AppendText (wxString text) {
  log->AppendText (text);
  int n = log->GetLastPosition ();
  if (n>0) n--;
  log->ShowPosition (n);
  CheckLogLength ();
}

void DebugLogDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId();
  switch (id) {
    case wxID_OK:
      Show(FALSE);
      break;
    case ID_Execute: // pressed execute button
      Execute(false);
      break;
    default:
      event.Skip();
  }
}

void DebugLogDialog::OnKeyEvent(wxKeyEvent& event)
{
  wxLogDebug(wxT("key event"));
}
#endif

//////////////////////////////////////////////////////////////////////
// PluginControlDialog implementation
//////////////////////////////////////////////////////////////////////

// all events go to OnEvent method
BEGIN_EVENT_TABLE(PluginControlDialog, wxDialog)
  EVT_BUTTON(-1, PluginControlDialog::OnEvent)
  EVT_CHECKBOX(-1, PluginControlDialog::OnEvent)
  EVT_TEXT(-1, PluginControlDialog::OnEvent)
  EVT_LISTBOX(-1, PluginControlDialog::OnEvent)
END_EVENT_TABLE()

PluginControlDialog::PluginControlDialog(
    wxWindow* parent,
    wxWindowID id)
  : wxDialog(parent, id, wxT(""), wxDefaultPosition, wxDefaultSize,
    wxDEFAULT_DIALOG_STYLE)
{
  SetTitle(wxT("Optional Plugin Control"));
  vertSizer = new wxBoxSizer(wxVERTICAL);
  horzSizer = new wxBoxSizer(wxHORIZONTAL);
  listSizer = new wxBoxSizer(wxVERTICAL);
  editSizer = new wxBoxSizer(wxVERTICAL);
  buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  horzSizer->Add(listSizer, 0, wxALIGN_LEFT);
  horzSizer->Add(editSizer, 0, wxALIGN_RIGHT);
  vertSizer->Add(horzSizer, 0, wxALIGN_LEFT);
  vertSizer->Add(buttonSizer, 0, wxALIGN_CENTER);
  // listSizer contents
  pluglist = new wxListBox(this, ID_PluginList);
  listSizer->Add(pluglist, 0, wxALL, 10);
  // editSizer contents
  plugname = new wxTextCtrl(this, ID_PluginName, wxT(""), wxDefaultPosition, wxSize(120, -1));
  editSizer->Add(plugname, 0, wxALL, 10);
  btn_load = new wxButton(this, ID_Load, wxT("Load"));
  editSizer->Add(btn_load, 0, wxALL | wxALIGN_RIGHT, 5);
  btn_unload = new wxButton(this, ID_Unload, wxT("Unload"));
  editSizer->Add(btn_unload, 0, wxALL | wxALIGN_RIGHT, 5);
  // buttonSizer contents
  wxButton *btn = new wxButton(this, wxID_HELP, BTNLABEL_HELP);
  buttonSizer->Add(btn, 0, wxALL, 5);
  btn = new wxButton(this, wxID_OK, BTNLABEL_OK);
  buttonSizer->Add(btn, 0, wxALL, 5);
  // make sure all plugins are loaded and add them to the listbox
  SIM->opt_plugin_ctrl("*", 1);
  bx_list_c *plugin_ctrl = (bx_list_c*) SIM->get_param(BXPN_PLUGIN_CTRL);
  for (int i = 0; i < plugin_ctrl->get_size(); i++) {
    bx_param_bool_c *plugin = (bx_param_bool_c*)plugin_ctrl->get(i);
    pluglist->Insert(wxString(plugin->get_name(), wxConvUTF8), i);
  }
  btn_load->Enable(0);
  btn_unload->Enable(0);
}

void PluginControlDialog::Init()
{
  SetSizer(vertSizer);
  vertSizer->Fit(this);
  wxSize size = vertSizer->GetMinSize();
  int margin = 5;
  SetSizeHints(size.GetWidth() + margin, size.GetHeight() + margin);
  Center();
}

void PluginControlDialog::OnEvent(wxCommandEvent& event)
{
  char buf[1024];

  int id = event.GetId();
  switch (id) {
    case ID_PluginList:
      if (event.GetEventType() == wxEVT_COMMAND_LISTBOX_SELECTED) {
        btn_unload->Enable(1);
      }
      break;
    case ID_PluginName:
      if (event.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED) {
        btn_load->Enable(!plugname->IsEmpty());
      }
      break;
    case ID_Load:
      {
        wxString tmpname(plugname->GetValue());
        strncpy(buf, tmpname.mb_str(wxConvUTF8), sizeof(buf));
        if (SIM->opt_plugin_ctrl(buf, 1)) {
          tmpname.Printf(wxT("Plugin '%s' loaded"), buf);
          wxMessageBox(tmpname, wxT("Plugin Control"), wxOK | wxICON_INFORMATION, this);
          pluglist->Insert(wxString(buf, wxConvUTF8), pluglist->GetCount());
        }
      }
      break;
    case ID_Unload:
      {
        int i = pluglist->GetSelection();
        wxString tmpname = pluglist->GetString(i);
        strncpy(buf, tmpname.mb_str(wxConvUTF8), sizeof(buf));
        if (SIM->opt_plugin_ctrl(buf, 0)) {
          tmpname.Printf(wxT("Plugin '%s' unloaded"), buf);
          wxMessageBox(tmpname, wxT("Plugin Control"), wxOK | wxICON_INFORMATION, this);
          pluglist->Delete(i);
          btn_unload->Enable(0);
        }
      }
      break;
    case wxID_OK:
      EndModal(wxID_OK);
      break;
    case wxID_HELP:
      ShowHelp();
      break;
    default:
      event.Skip();
  }
}

void PluginControlDialog::ShowHelp()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR, this);
}

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
  : wxDialog (parent, id, wxT(""), wxDefaultPosition, wxDefaultSize,
    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  idHash = new wxHashTable (wxKEY_INTEGER);
  paramHash = new wxHashTable (wxKEY_INTEGER);
  nbuttons = 0;
  runtime = 0;

  // top level objects
  mainSizer = new wxBoxSizer (wxVERTICAL);

  // create buttonSizer, which will hold all the buttons.
  buttonSizer = new wxBoxSizer (wxHORIZONTAL);
}

ParamDialog::~ParamDialog()
{
  paramHash->BeginFind();
  wxNode *node;
  while ((node = (wxNode*)paramHash->Next()) != NULL) {
    // assume that no ParamStruct appears in the hash table under multiple
    // keys.  If so, we will delete it twice and corrupt memory.
    ParamStruct *pstr = (ParamStruct*) node->GetData();
    // wxLogDebug(wxT("deleting ParamStruct id=%d for param %s"), pstr->id, pstr->param->get_name());
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
  if (nbuttons==0) AddDefaultButtons();
  mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);
  EnableChanged();
  // lay it out!
  SetAutoLayout(TRUE);
  SetSizer(mainSizer);
  mainSizer->Fit(this);
  wxSize size = mainSizer->GetMinSize();
  int margin = 5;
  SetSizeHints(size.GetWidth() + margin, size.GetHeight() + margin);
  Center();
}

static int _next_id = ID_LAST_USER_DEFINED;

int ParamDialog::genId () {
  return ++_next_id;
}

bool ParamDialog::isGeneratedId (int id) {
  return (id >= ID_LAST_USER_DEFINED && id < _next_id);
}

void ParamDialog::AddParamList(const char *nameList[], bx_param_c *base, wxFlexGridSizer *sizer, bool plain)
{
  int i = 0;
  while (nameList[i] != NULL) {
    bx_param_c *param = SIM->get_param(nameList[i], base);
    if (param != NULL) {
      AddParam(param, sizer, plain);
    }
    i++;
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
  wxLogDebug(wxT("AddParam for param '%s'"), param_generic->get_name());
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
  int type = param_generic->get_type();
  const char *prompt;
  if (type == BXT_LIST) {
    bx_list_c *list = (bx_list_c*)pstr->param;
    prompt = list->get_title();
  } else {
    prompt = pstr->param->get_label();
  }
  if (!prompt) prompt = pstr->param->get_name();
  const char *description = pstr->param->get_description();
  wxASSERT (prompt != NULL);
#define ADD_LABEL(x) sizer->Add(pstr->label = new wxStaticText(context->parent, -1, wxString(x, wxConvUTF8)), 0, wxALIGN_RIGHT|wxALL, 3)
  switch (type) {
    case BXT_PARAM_BOOL: {
	bx_param_bool_c *param = (bx_param_bool_c*) param_generic;
	if (!plain) ADD_LABEL(prompt);
	wxCheckBox *ckbx = new wxCheckBox (context->parent, pstr->id, wxT(""));
	ckbx->SetValue (param->get ());
        if (description) ckbx->SetToolTip(wxString(description, wxConvUTF8));
	sizer->Add (ckbx, 0, wxALL, 2);
	if (!plain) sizer->Add (1, 1);  // spacer
	pstr->u.checkbox = ckbx;
	idHash->Put (pstr->id, pstr);
	paramHash->Put (pstr->param->get_id (), pstr);
        break;
      }
    case BXT_PARAM_NUM: {
	bx_param_num_c *param = (bx_param_num_c*) param_generic;
	if (!plain) ADD_LABEL (prompt);
        if (param->get_options () & param->USE_SPIN_CONTROL) {
          wxSpinCtrl *spinctrl = new wxSpinCtrl (context->parent, pstr->id);
          spinctrl->SetValue (param->get ());
          int max = (param->get_max () < (1<<24))?param->get_max ():(1<<24)-1;
          spinctrl->SetRange (param->get_min (), SPINCTRL_FIX_MAX (max));
          if (description) spinctrl->SetToolTip(wxString(description, wxConvUTF8));
          sizer->Add (spinctrl, 0, wxALL, 2);
          if (!plain) sizer->Add (1, 1);  // spacer
          pstr->u.spin = spinctrl;
        } else {
          wxTextCtrl *textctrl = new wxTextCtrl (context->parent, pstr->id, wxT(""), wxDefaultPosition, normalTextSize);
          const char *format = param->get_format ();
          if (!format)
            format = strdup(param->get_base () == 16 ? "0x%X" : "%d");
          SetTextCtrl (textctrl, format, param->get ());
          if (description) textctrl->SetToolTip(wxString(description, wxConvUTF8));
          sizer->Add (textctrl, 0, wxALL, 2);
          if (!plain) sizer->Add (1, 1);  // spacer
          pstr->u.text = textctrl;
        }
	idHash->Put (pstr->id, pstr);
	paramHash->Put (pstr->param->get_id (), pstr);
        break;
      }
    case BXT_PARAM_ENUM: {
	bx_param_enum_c *param = (bx_param_enum_c*) param_generic;
	if (!plain) ADD_LABEL (prompt);
	wxChoice *choice = new wxChoice (context->parent, pstr->id);
        if (description) choice->SetToolTip(wxString(description, wxConvUTF8));
	sizer->Add (choice, 0, wxADJUST_MINSIZE, 2);
	if (!plain) sizer->Add (1, 1);  // spacer
	// fill in the choices
	int i=0;
	const char *ptr;
	while (NULL != (ptr = param->get_choice(i++)))
	  choice->Append(wxString(ptr, wxConvUTF8));
	choice->SetSelection (param->get() - param->get_min());
	pstr->u.choice = choice;
	idHash->Put(pstr->id, pstr);
	paramHash->Put(pstr->param->get_id(), pstr);
        break;
      }
    case BXT_PARAM_STRING: {
	bx_param_string_c *param = (bx_param_string_c*) param_generic;
	if (!plain) ADD_LABEL (prompt);
        bool isFilename = param->get_options() & param->IS_FILENAME;
	wxTextCtrl *txtctrl = new wxTextCtrl (context->parent, pstr->id, wxT(""), wxDefaultPosition, isFilename? longTextSize : normalTextSize);
        if (description) txtctrl->SetToolTip(wxString(description, wxConvUTF8));
        if (param->get_options() & param->RAW_BYTES) {
          char *value = param->getptr();
          wxString buffer;
          char sep_string[2];
          sep_string[0] = param->get_separator();
          sep_string[1] = 0;
          for (int i=0; i<param->get_maxsize(); i++) {
            wxString eachbyte;
            eachbyte.Printf(wxT("%02x"), (unsigned int)0xff&value[i]);
            if (i > 0)
              buffer += wxString(sep_string, wxConvUTF8);
            buffer += eachbyte;
          }
          txtctrl->SetValue(buffer);
        } else {
          txtctrl->SetValue(wxString(param->getptr(), wxConvUTF8));
          txtctrl->SetMaxLength(param->get_maxsize());
        }
	sizer->Add (txtctrl, 0, wxALL, 2);
	if (!plain) {
	  if (isFilename) {
	    // create Browse button
	    pstr->browseButtonId = genId ();
	    pstr->browseButton = new wxButton (context->parent,
		pstr->browseButtonId, BTNLABEL_BROWSE);
	    sizer->Add (pstr->browseButton, 0, wxALL, 2);
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
        if (list->get_options() & bx_list_c::USE_TAB_WINDOW) {
	  // put each item in a separate tab of a tabbed window
	  wxNotebook *notebook = new wxNotebook(context->parent, -1);
#if wxMAJOR_VERSION == 2 && wxMINOR_VERSION < 6
	  wxNotebookSizer *nbsizer = new wxNotebookSizer(notebook);
#endif
	  // put all items in the list into a separate page of the notebook.
	  for (int i=0; i<list->get_size (); i++) {
	    bx_list_c *child = (bx_list_c*)list->get(i);
	    wxASSERT (child->get_type() == BXT_LIST);
	    // the child must be a list!  I could support other things but
	    // I don't see any reason to.  It wouldn't make sense to devote
	    // a whole tab to a single parameter.
	    wxPanel *panel = new wxPanel(notebook);
	    wxBoxSizer *boxsz = new wxBoxSizer(wxVERTICAL);
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
	    const char *pagename = child->get_title();
	    if (!pagename) pagename = child->get_name();
	    panel->SetAutoLayout(TRUE);
	    panel->SetSizer(boxsz);
	    notebook->AddPage(panel, wxString(pagename, wxConvUTF8));
	  }
#if wxMAJOR_VERSION == 2 && wxMINOR_VERSION < 6
	  context->vertSizer->Add(nbsizer, 0, wxALL|wxGROW, 10);
#else
	  context->vertSizer->Add(notebook, 0, wxALL|wxGROW, 10);
#endif
	  // clear gridSizer variable so that any future parameters force
	  // creation of a new one.
	  context->gridSizer = NULL;
	  // add to hashes
	  pstr->u.notebook = notebook;
	  idHash->Put(pstr->id, pstr);
	  paramHash->Put(pstr->param->get_id(), pstr);
	} else {
          wxString boxTitle;
          if (list->get_options() & bx_list_c::USE_BOX_TITLE) {
            boxTitle = wxString(prompt, wxConvUTF8);
          } else {
            boxTitle = wxT("");
          }
          wxStaticBox *box = new wxStaticBox(context->parent, -1, boxTitle);
	  wxStaticBoxSizer *boxsz = new wxStaticBoxSizer(box, wxVERTICAL);
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
      wxLogError(wxT("ParamDialog::AddParam called with unsupported param type id=%d"), (int)type);
  }
  if (pstr->label) pstr->label->Enable(pstr->param->get_enabled());
  if (pstr->u.window) pstr->u.window->Enable(pstr->param->get_enabled());
  if (pstr->browseButton) pstr->browseButton->Enable(pstr->param->get_enabled());
}

bool ParamDialog::CopyGuiToParam()
{
  // loop through all the parameters
  idHash->BeginFind();
  wxNode *node;
  while ((node = (wxNode*)idHash->Next()) != NULL) {
    ParamStruct *pstr = (ParamStruct*) node->GetData();
    wxLogDebug(wxT("commit changes for param %s"), pstr->param->get_name());
    CopyGuiToParam(pstr->param);
    if (pstr->param->get_type() == BXT_LIST) break;
  }
  return true;
}

bool ParamDialog::CopyGuiToParam(bx_param_c *param)
{
  int i;

  if (param == NULL) return false;
  ParamStruct *pstr = (ParamStruct*) paramHash->Get(param->get_id());
  wxLogDebug(wxT("commit changes for param %s"), param->get_name());
  int type = param->get_type();
  switch (type) {
    case BXT_PARAM_BOOL: {
      bx_param_bool_c *boolp = (bx_param_bool_c*) pstr->param;
      bool val = pstr->u.checkbox->GetValue();
      if (val != boolp->get()) boolp->set(val);
      break;
    }
    case BXT_PARAM_NUM: {
      bx_param_num_c *nump = (bx_param_num_c*) pstr->param;
      bool valid;
      int n;
      wxString complaint(wxT("Invalid integer for '"));
      complaint += wxString(pstr->param->get_name(), wxConvUTF8);
      complaint += wxT("'.");
      if (nump->get_options() & nump->USE_SPIN_CONTROL) {
        n = pstr->u.spin->GetValue();
      } else {
        n = GetTextCtrlInt(pstr->u.text, &valid, true, complaint);
      }
      if ((n < nump->get_min()) || (n > nump->get_max())) {
        wxMessageBox(wxT("Numerical parameter out of range"), wxT("Error"), wxOK | wxICON_ERROR, this);
        return false;
      }
      if (n != nump->get()) nump->set(n);
      break;
    }
    case BXT_PARAM_ENUM: {
      bx_param_enum_c *enump = (bx_param_enum_c*) pstr->param;
      int value = pstr->u.choice->GetSelection() + enump->get_min();
      if (value != enump->get()) enump->set(value);
      break;
    }
    case BXT_PARAM_STRING: {
      bx_param_string_c *stringp = (bx_param_string_c*) pstr->param;
      char buf[1024];
      wxString tmp(pstr->u.text->GetValue());
      if (stringp->get_options() & stringp->RAW_BYTES) {
        char src[1024];
        int p = 0;
        unsigned int n;
        strcpy(src, tmp.mb_str(wxConvUTF8));
        for (i=0; i<stringp->get_maxsize(); i++)
          buf[i] = 0;
        for (i=0; i<stringp->get_maxsize(); i++) {
          while (src[p] == stringp->get_separator())
            p++;
          if (src[p] == 0) break;
          // try to read a byte of hex
          if (sscanf (src+p, "%02x", &n) == 1) {
            buf[i] = n;
            p+=2;
          } else {
            wxMessageBox(wxT("Illegal raw byte format"), wxT("Error"), wxOK | wxICON_ERROR, this);
            return false;
          }
        }
      } else {
        strncpy(buf, tmp.mb_str(wxConvUTF8), sizeof(buf));
      }
      buf[sizeof(buf)-1] = 0;
      if (!stringp->equals(buf)) stringp->set(buf);
      break;
    }
    case BXT_LIST: {
      bx_list_c *list = (bx_list_c*) param;
      for (i = 0; i < list->get_size(); i++) {
        bx_param_c *item = list->get(i);
        if (!CopyGuiToParam(item)) break;
      }
      break;
    }
    default:
      wxLogError(wxT("ParamDialog::CopyGuiToParam: unsupported param type id=%d"), (int)type);
  }
  return true;
}

void ParamDialog::EnableChanged()
{
  idHash->BeginFind();
  wxNode *node;
  while ((node = (wxNode*)idHash->Next ()) != NULL) {
    ParamStruct *pstr = (ParamStruct*) node->GetData();
    if (runtime) {
      if ((pstr->param->get_type() != BXT_LIST) && !pstr->param->get_runtime_param())
        EnableParam(pstr->param->get_id(),  false);
    }
    // special cases that can't be handled in the usual way
  }
}

void ParamDialog::EnableChanged(ParamStruct *pstr)
{
  wxLogDebug(wxT("EnableChanged on param %s"), pstr->param->get_name());
  ProcessDependentList(pstr, true);
}

void ParamDialog::EnableParam(int param_id, bool enabled)
{
  ParamStruct *pstr = (ParamStruct*) paramHash->Get(param_id);
  if (!pstr) return;
  if (pstr->label) pstr->label->Enable(enabled);
  if (pstr->browseButton) pstr->browseButton->Enable(enabled);
  if (pstr->u.window) pstr->u.window->Enable(enabled);
}

void ParamDialog::ProcessDependentList(ParamStruct *pstrChanged, bool enabled)
{
  bx_param_c *dparam;
  ParamStruct *pstr;
  Bit64s value;
  bool en;
  int i;

  bx_list_c *list = pstrChanged->param->get_dependent_list();
  if (list) {
    if (pstrChanged->param->get_type() == BXT_PARAM_ENUM) {
      bx_param_enum_c *enump = (bx_param_enum_c*)pstrChanged->param;
      value = pstrChanged->u.choice->GetSelection() + enump->get_min();
      Bit64u enable_bitmap = enump->get_dependent_bitmap(value);
      Bit64u mask = 0x1;
      for (i = 0; i < list->get_size(); i++) {
        dparam = list->get(i);
        if (dparam != enump) {
          en = (enable_bitmap & mask) && enabled;
          pstr = (ParamStruct*) paramHash->Get(dparam->get_id());
          if (pstr) {
            if (en != pstr->u.window->IsEnabled()) {
              EnableParam(dparam->get_id(), en);
              ProcessDependentList(pstr, en);
            }
          }
        }
        mask <<= 1;
      }
    } else if ((pstrChanged->param->get_type() == BXT_PARAM_BOOL) ||
               (pstrChanged->param->get_type() == BXT_PARAM_NUM) ||
               (pstrChanged->param->get_type() == BXT_PARAM_STRING)) {
      bx_param_c *param = pstrChanged->param;
      if (param->get_type() == BXT_PARAM_BOOL) {
        value = pstrChanged->u.checkbox->GetValue();
      } else if (param->get_type() == BXT_PARAM_NUM) {
        bx_param_num_c *nump = (bx_param_num_c*)param;
        if (nump->get_options() & nump->USE_SPIN_CONTROL) {
          value = (pstrChanged->u.spin->GetValue() > 0);
        } else {
          bool valid;
          value = (GetTextCtrlInt(pstrChanged->u.text, &valid, true, wxT("")) > 0);
        }
      } else {
        wxString tmp(pstrChanged->u.text->GetValue());
        value = !tmp.IsEmpty() && tmp.compare(wxT("none"));
      }
      for (i = 0; i < list->get_size(); i++) {
        dparam = list->get(i);
        if (dparam != param) {
          en = (value && enabled);
          pstr = (ParamStruct*) paramHash->Get(dparam->get_id());
          if (pstr) {
            if (en != pstr->u.window->IsEnabled()) {
              EnableParam(dparam->get_id(), en);
              ProcessDependentList(pstr, en);
            }
          }
        }
      }
    }
  }
}

// if any parameters changed, update the associated control
void ParamDialog::CopyParamToGui()
{
  // loop through all the parameters
  idHash->BeginFind ();
  wxNode *node;
  while ((node = (wxNode*)idHash->Next ()) != NULL) {
    ParamStruct *pstr = (ParamStruct*) node->GetData ();
    IFDBG_DLG(wxLogDebug(wxT("refresh param %s"), pstr->param->get_name()));
    int type = pstr->param->get_type ();
    switch (type) {
      case BXT_PARAM_BOOL: {
        bx_param_bool_c *boolp = (bx_param_bool_c*) pstr->param;
        pstr->u.checkbox->SetValue(boolp->get());
        break;
        }
      case BXT_PARAM_NUM: {
        bx_param_num_c *nump = (bx_param_num_c*) pstr->param;
        const char *format = nump->get_format();
        if (!format)
          format = strdup(nump->get_base() == 16 ? "0x%X" : "%d");
        SetTextCtrl(pstr->u.text, format, nump->get());
        break;
        }
      case BXT_PARAM_ENUM: {
        bx_param_enum_c *enump = (bx_param_enum_c*) pstr->param;
        pstr->u.choice->SetSelection(enump->get() - enump->get_min());
        break;
        }
      case BXT_PARAM_STRING: {
        bx_param_string_c *stringp = (bx_param_string_c*) pstr->param;
        pstr->u.text->SetValue(wxString(stringp->getptr(), wxConvUTF8));
        break;
        }
      case BXT_LIST:
        break;
      default:
        wxLogError(wxT("ParamDialog::CopyParamToGui(): unsupported param type id=%d"), (int)type);
    }
  }
}

void ParamDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId();
  if (isGeneratedId(id)) {
    ParamStruct *pstr = (ParamStruct*) idHash->Get(id);
    if (pstr == NULL) {
      wxLogDebug(wxT("ParamStruct not found for id=%d"), id);
      return;
    }
    if (id == pstr->id) {
      IFDBG_DLG(wxLogDebug(wxT("event came from window %p (id=%d) controlled by parameter '%s'"), pstr->u.window, id, pstr->param->get_name()));
      switch (pstr->param->get_type ()) {
        case BXT_PARAM_BOOL:
        case BXT_PARAM_NUM:
        case BXT_PARAM_ENUM:
        case BXT_PARAM_STRING:
          EnableChanged(pstr);
          break;
      }
      return;
    }
    if (id == pstr->browseButtonId) {
      wxLogDebug(wxT("browse button id=%d attached to wxTextCtrl %p"), id, pstr->u.text);
      BrowseTextCtrl(pstr->u.text);
      return;
    }
    wxLogDebug(wxT("id was key to ParamStruct but doesn't match either id inside"));
  }
  switch (id) {
    case wxID_OK:
      if (IsModal()) {
        if (CopyGuiToParam())
          EndModal (wxID_OK);
      } else {
        CopyParamToGui();
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

void ParamDialog::ShowHelp()
{
  wxMessageBox(MSG_NO_HELP, MSG_NO_HELP_CAPTION, wxOK | wxICON_ERROR, this);
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
  : ParamDialog(parent, id)
{
  wxFlexGridSizer *column;
  nflags = 0;
  const char *mainRegList1[] = CPU_REGS_MAIN_REGS1;
  const char *mainRegList2[] = CPU_REGS_MAIN_REGS2;
  const char *mainRegList3[] = CPU_REGS_MAIN_REGS3;
  const char *flagList[]     = CPU_REGS_FLAGS;
  const char *controlList[]  = CPU_REGS_CONTROL_REGS;
  const char *debugList[]    = CPU_REGS_DEBUG_REGS;
  const char *testList[]     = CPU_REGS_TEST_REGS;
  bx_list_c *base      = (bx_list_c*)SIM->get_param(BXPN_WX_CPU0_STATE);

  // top level objects
  wxStaticBox *mainRegsBox = new wxStaticBox(this, -1, wxT("Basic Registers"));
  wxStaticBoxSizer *mainRegsBoxSizer =
    new wxStaticBoxSizer(mainRegsBox, wxVERTICAL);
  mainSizer->Add(mainRegsBoxSizer, 0, wxALL|wxGROW, 10);

  wxStaticBox *flagsBox = new wxStaticBox(this, -1, wxT("EFLAGS Bits"));
  wxStaticBoxSizer *flagsBoxSizer =
    new wxStaticBoxSizer(flagsBox, wxVERTICAL);
  mainSizer->Add(flagsBoxSizer, 0, wxALL|wxGROW, 10);

  wxStaticBox *otherBox = new wxStaticBox(this, -1, wxT("Other Registers"));
  wxStaticBoxSizer *otherBoxSizer =
    new wxStaticBoxSizer(otherBox, wxVERTICAL);
  mainSizer->Add(otherBoxSizer, 0, wxALL|wxGROW, 10);

  // mainRegsSizer contents
  mainRegsSizer = new wxFlexGridSizer(3);
  mainRegsBoxSizer->Add(mainRegsSizer, 0, wxALL, 3);
  column = new wxFlexGridSizer(3);
  mainRegsSizer->Add(column, 0, wxALL, 10);
  AddParamList(mainRegList1, base, column);

  column = new wxFlexGridSizer(3);
  mainRegsSizer->Add(column, 0, wxALL, 10);
  AddParamList(mainRegList2, base, column);

  column = new wxFlexGridSizer(3);
  mainRegsSizer->Add(column, 0, wxALL, 10);
  AddParamList(mainRegList3, base, column);

  // add flag parameters
  flagsSizer = new wxFlexGridSizer(CPU_REGS_MAX_FLAGS);
  flagsBoxSizer->Add(flagsSizer, 0, wxALL | wxALIGN_CENTER, 3);
  int i = 0;
  while (flagList[i] != NULL) {
    bx_param_c *param = SIM->get_param(flagList[i], base);
    if (param != NULL) {
      AddFlag(param);
    }
    i++;
  }

  // extRegsSizer contents
  extRegsSizer = new wxFlexGridSizer(3);
  otherBoxSizer->Add(extRegsSizer, 0, wxALL, 3);

  column = new wxFlexGridSizer(3);
  extRegsSizer->Add(column, 0, wxALL, 10);
  AddParamList(controlList, base, column);

  column = new wxFlexGridSizer(3);
  extRegsSizer->Add(column, 0, wxALL, 10);
  AddParamList(debugList, base, column);

  column = new wxFlexGridSizer(3);
  extRegsSizer->Add(column, 0, wxALL, 10);
  AddParamList(testList, base, column);

  // add buttons
#if BX_DEBUGGER
  // only show these if debugger is enabled
  contButton = AddButton(ID_Debug_Continue, BTNLABEL_DEBUG_CONTINUE);
  stopButton = AddButton(ID_Debug_Stop, BTNLABEL_DEBUG_STOP);
  stepButton = AddButton(ID_Debug_Step, BTNLABEL_DEBUG_STEP);
  //commitButton = AddButton(ID_Debug_Commit, BTNLABEL_DEBUG_COMMIT);
#endif
  AddButton(ID_Close, BTNLABEL_CLOSE);
}

void CpuRegistersDialog::AddFlag(bx_param_c *param)
{
  if (param == NULL) {
    wxLogDebug(wxT("AddFlag on undefined param"));
    return;
  }
  wxASSERT(nflags < CPU_REGS_MAX_FLAGS);
  flagptr[nflags++] = param;
}

void CpuRegistersDialog::Init()
{
  int i;
  for (i=0; i<CPU_REGS_MAX_FLAGS; i++) {
    if (i<nflags) {
      bx_param_c *param = flagptr[i];
      flagsSizer->Add(new wxStaticText(this, -1, wxString(param->get_label(), wxConvUTF8)), 0, wxALL|wxALIGN_LEFT, 4);
    } else {
      flagsSizer->Add(0, 0);  // spacer
    }
  }
  for (i=0; i<nflags; i++) {
    bx_param_c *param = flagptr[i];
    AddParam(param, flagsSizer, true);
  }
  // special case: make IOPL text field small
  ParamStruct *pstr = (ParamStruct*)paramHash->Get(SIM->get_param(BXPN_WX_CPU0_EFLAGS_IOPL)->get_id());
  if (pstr != NULL) {
    wxSize size = pstr->u.text->GetSize();
    size.SetWidth (size.GetWidth() / 2);
    pstr->u.text->SetSize(size);
    flagsSizer->SetItemMinSize(pstr->u.text, size.GetWidth(), size.GetHeight());
  }
  ParamDialog::Init();
  stateChanged(false);
}

void CpuRegistersDialog::stateChanged (bool simRunning)
{
#if BX_DEBUGGER
  contButton->Enable (!simRunning);
  stepButton->Enable (!simRunning);
  stopButton->Enable (simRunning);
#endif
}

void CpuRegistersDialog::CopyParamToGui ()
{
  ParamDialog::CopyParamToGui ();
#if BX_DEBUGGER
  stateChanged (SIM->get_param_bool(BXPN_DEBUG_RUNNING)->get());
#endif
}

// How am I going to communicate with the debugger?
//
// The current model is that the debugger asks you for a command, and
// blocks forever until you press return.  Then it interprets the command,
// however long it takes, and returns to the input loop when the command
// is done.  A control-C can stop a command prematurely.
//
// To extend this into wxWidgets multithreaded space, I will create a
// synchronous event called BX_SYNC_GET_DBG_COMMAND which is sent from
// the simulation thread to wxWidgets.  When the user chooses a debugger
// action (step, continue, breakpoint, etc.) the simulation awakens and
// interprets the event by calling a function in bx_debug/dbg_main.cc.
//
// The equivalent of a control-C is pressing the "Stop" button during
// a long step or continue command.  This can be implemented by setting
// bx_guard.interrupt_requested = 1, just like the SIGINT handler in
// bx_debug/dbg_main.cc does.
//
// input loop model is good.  Create a debugger input loop, possibly in
// siminterface.
// in the simulation thread.  This loop waits for a command from the
// wxWidgets debugger
//
// For example, if you press the "Step" button 5
// times, with each click it should call bx_dbg_stepN_command(1) in the
// simulator thread.  When it returns, it goes back to
//
void CpuRegistersDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId ();
  switch (id) {
    case ID_Close:
      Show(FALSE);
      break;
#if BX_DEBUGGER
  case ID_Debug_Stop:
      wxLogDebug(wxT("wxWidgets triggered a break"));
      theFrame->DebugBreak();
      break;
    case ID_Debug_Continue:
      wxLogDebug(wxT("before calling DebugCommand"));
      theFrame->DebugCommand("continue");
      wxLogDebug(wxT("after calling DebugCommand"));
      break;
    case ID_Debug_Step:
      theFrame->DebugCommand("step 1");
      break;
    case ID_Debug_Commit:
      CopyGuiToParam();
      break;
#endif
    default:
      ParamDialog::OnEvent(event);
  }
}

//////////////////////////////////////////////////////////////////////
// FloppyConfigDialog implementation
//////////////////////////////////////////////////////////////////////

// all events go to OnEvent method
BEGIN_EVENT_TABLE(FloppyConfigDialog, wxDialog)
  EVT_BUTTON(-1, FloppyConfigDialog::OnEvent)
  EVT_CHECKBOX(-1, FloppyConfigDialog::OnEvent)
  EVT_CHOICE(-1, FloppyConfigDialog::OnEvent)
  EVT_TEXT(-1, FloppyConfigDialog::OnEvent)
END_EVENT_TABLE()

FloppyConfigDialog::FloppyConfigDialog(
    wxWindow* parent,
    wxWindowID id)
  : ParamDialog(parent, id)
{
  createButton = AddButton(ID_Create, wxT("Create Image"));
  AddDefaultButtons();
}

void FloppyConfigDialog::Setup(bx_list_c *list)
{
  int devtype_id, path_id, media_id, status_id, readonly_id;

  devtype_id = list->get_by_name("devtype")->get_id();
  path_id = list->get_by_name("path")->get_id();
  media_id = list->get_by_name("type")->get_id();
  status_id = list->get_by_name("status")->get_id();
  readonly_id = list->get_by_name("readonly")->get_id();
  AddParam(list);
  pstrDevice = (ParamStruct*) paramHash->Get(devtype_id);
  pstrPath = (ParamStruct*) paramHash->Get(path_id);
  pstrMedia = (ParamStruct*) paramHash->Get(media_id);
  pstrStatus = (ParamStruct*) paramHash->Get(status_id);
  pstrReadonly = (ParamStruct*) paramHash->Get(readonly_id);
}

void FloppyConfigDialog::OnEvent(wxCommandEvent& event)
{
  int id = event.GetId();
  if (isGeneratedId(id)) {
    ParamStruct *pstr = (ParamStruct*) idHash->Get(id);
    if (pstr == NULL) {
      wxLogDebug(wxT("ParamStruct not found for id=%d"), id);
      return;
    }
    if (id == pstr->id) {
      if ((pstr == pstrDevice) || (pstr == pstrMedia)) {
        int val1 = pstrDevice->u.choice->GetSelection() + ((bx_param_num_c*)pstrDevice->param)->get_min();
        int val2 = pstrMedia->u.choice->GetSelection() + ((bx_param_num_c*)pstrMedia->param)->get_min();
        createButton->Enable((val1 != BX_FDD_NONE) && (val2 != BX_FLOPPY_NONE));
      } else if ((pstr == pstrPath) && (!pstrPath->u.text->IsModified())) {
        pstrMedia->u.choice->SetSelection(pstrMedia->u.choice->FindString(wxT("auto")));
        pstrStatus->u.checkbox->SetValue(1);
      }
    }
    ParamDialog::OnEvent(event);
  } else {
    switch (id) {
      case ID_Create:
        {
          int cap = pstrMedia->u.choice->GetSelection();
          char name[1024];
          strncpy(name, pstrPath->u.text->GetValue().mb_str(wxConvUTF8), sizeof(name));
          if ((floppy_type_n_sectors[cap] > 0) && (strlen(name) > 0) && (strcmp(name, "none"))) {
            if (CreateImage (0, floppy_type_n_sectors[cap], name)) {
              wxString msg(wxT("Created a "));
              msg += pstrMedia->u.choice->GetString(cap);
              msg += wxT(" disk image called '");
              msg += pstrPath->u.text->GetValue();
              msg += wxT("'.");
              wxMessageBox(msg, wxT("Image Created"), wxOK | wxICON_INFORMATION, this);
            }
          }
        }
        break;
      default:
        ParamDialog::OnEvent(event);
    }
  }
}

//////////////////////////////////////////////////////////////////////
// LogOptionsDialog implementation
//////////////////////////////////////////////////////////////////////

// all events go to OnEvent method
BEGIN_EVENT_TABLE(LogOptionsDialog, wxDialog)
  EVT_BUTTON(-1, LogOptionsDialog::OnEvent)
  EVT_CHECKBOX(-1, LogOptionsDialog::OnEvent)
  EVT_TEXT(-1, LogOptionsDialog::OnEvent)
END_EVENT_TABLE()

LogOptionsDialog::LogOptionsDialog(
    wxWindow* parent,
    wxWindowID id)
  : ParamDialog(parent, id)
{
  static wxString names[] = LOG_OPTS_TYPE_NAMES;
  SetTitle(LOG_OPTS_TITLE);
  AddParam(SIM->get_param("log"));
  wxStaticText *text = new wxStaticText(this, -1, LOG_OPTS_PROMPT);
  mainSizer->Add(text, 0, wxALL, 10);
  gridSizer = new wxFlexGridSizer (2);
  mainSizer->Add(gridSizer, 1, wxLEFT, 40);
  text = new wxStaticText (this, -1, LOG_OPTS_ADV);
  mainSizer->Add(text, 0, wxTOP|wxLEFT, 20);

  // gridSizer contents
  gridSizer->AddGrowableCol(1);
  for (int evtype=0; evtype<LOG_OPTS_N_TYPES; evtype++) {
    gridSizer->Add(new wxStaticText (this, -1, names[evtype]), 0, wxALL, 5);
    action[evtype] = makeLogOptionChoiceBox(this, -1, evtype, true);
    gridSizer->Add(action[evtype], 1, wxALL|wxGROW|wxADJUST_MINSIZE, 5);
  }
}

void LogOptionsDialog::SetAction(int evtype, int a)
{
  // find the choice whose client data matches "a".
  int *ptr;
  //wxLogDebug ("SetAction type=%d a=%d", evtype, a);
  for (int i=0; i < (int)action[evtype]->GetCount(); i++) {
    //wxLogDebug ("reading action[%d]->GetClientData(%d)", evtype, i);
    ptr = (int*) action[evtype]->GetClientData(i);
    if (ptr == NULL) continue;
    if (a == *ptr) {  // found it!
      action[evtype]->SetSelection(i);
      return;
    }
  }
  // this can happen if one of the choices that is excluded by
  // LOG_OPTS_EXCLUDE() is used, for example.
  wxLogDebug(wxT("SetAction type=%d a=%d not found"), evtype, a);
}

int LogOptionsDialog::GetAction(int evtype)
{
  int sel = action[evtype]->GetSelection();
  int *ptrToChoice = (int*)action[evtype]->GetClientData(sel);
  wxASSERT(ptrToChoice != NULL);
  return *ptrToChoice;
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
  win->SetLabel(newtext);
  wxSize sz = win->GetSize();
  sizer->SetItemMinSize(win, sz.GetWidth(), sz.GetHeight());
}

// CreateImage produces a disk image.  It's in the utility function
// area because it's used by both floppy and hard disk image creation.
bool CreateImage (int harddisk, int sectors, const char *filename)
{
  if (sectors<1) {
    wxMessageBox(wxT("The disk size is invalid."), wxT("Invalid Size"), wxOK | wxICON_ERROR);
    return false;
  }
  wxLogDebug(wxT("filename = '%s'\n"), filename);
  if (strlen (filename) < 1) {
    wxMessageBox(wxT("You must type a file name for the new disk image."), wxT("Bad Filename"), wxOK | wxICON_ERROR);
    return false;
  }
  // create disk image with name and capacity determined by the filename
  // and sector args.  Call SIM->create_image (filename, sectors, overwrite=0)
  // first which will create the file if it doesn't already exist.  If it
  // exists, it will instead return -1, and we can ask the user "are you sure
  // you want to overwrite?".  If yes, call again with overwrite=1.
  int ret = SIM->create_disk_image (filename, sectors, 0);
  if (ret == -1) {  // already exists
    int answer = wxMessageBox(wxT("File exists.  Do you want to overwrite it?"),
      wxT("File exists"), wxYES_NO | wxCENTER);
    if (answer == wxYES)
      ret = SIM->create_disk_image (filename, sectors, 1);
    else
      return false;  // wxNO
  }
  if (ret == -2) {
    wxMessageBox(wxT("I could not create the disk image. Check for permission problems or available disk space."), wxT("Failed"), wxOK | wxICON_ERROR);
    return false;
  }
  wxASSERT (ret==0);
  return true;
}

void SetTextCtrl(wxTextCtrl *ctrl, const char *format, int val)
{
  wxString tmp;
  tmp.Printf(wxString(format, wxConvUTF8), val);
  ctrl->SetValue(tmp);
}

int GetTextCtrlInt (wxTextCtrl *ctrl,
    bool *valid,
    bool complain,
    wxString complaint)
{
  wxString tmp (ctrl->GetValue());
  char buf[1024];
  strncpy(buf, tmp.mb_str(wxConvUTF8), sizeof(buf));
  int n = strtol(buf, NULL, 0);
  if (n != LONG_MIN && n != LONG_MAX) {
    if (valid) *valid = true;
    return n;
  }
  if (valid) *valid = false;
  if (complain) {
    wxMessageBox(complaint, wxT("Invalid"), wxOK | wxICON_ERROR);
    ctrl->SetFocus();
  }
  return -1;
}

bool BrowseTextCtrl(wxTextCtrl *text, wxString prompt, long style)
{
  // try to configure the dialog to show hidden files
  wxConfigBase::Get() ->Write(wxT("/wxWidgets/wxFileDialog/ShowHidden"), true);
  wxFileDialog *fdialog = new wxFileDialog (text->GetParent(), prompt, wxT(""), text->GetValue(), wxT("*.*"), style);
  int result = fdialog->ShowModal();
  if (result == wxID_OK)
    text->SetValue(fdialog->GetPath());
  delete fdialog;
  return (result == wxID_OK);
}

wxChoice *makeLogOptionChoiceBox (wxWindow *parent,
    wxWindowID id,
    int evtype,
    bool includeNoChange)
{
  static wxString choices[] = LOG_OPTS_CHOICES;
  static int integers[LOG_OPTS_N_CHOICES] = {0, 1, 2, 3, 4};
  wxChoice *control = new wxChoice(parent, id, wxDefaultPosition, wxDefaultSize);
  int lastChoice = 0;  // remember index of last choice
  int nchoice = includeNoChange? LOG_OPTS_N_CHOICES : LOG_OPTS_N_CHOICES_NORMAL;
  for (int choice=0; choice<nchoice; choice++) {
    // the exclude expression allows some choices to not be available
    // for some times.  For example, it would be stupid to ignore a panic.
    if (!LOG_OPTS_EXCLUDE(evtype, choice)) {
      control->Append(choices[choice], &integers[choice]);
      // the client data is an int* that points to the choice number.
      // This is what will be returned by GetAction().
      lastChoice++;
    }
  }
  control->SetSelection (lastChoice-1);
  return control;
}

#endif /* if BX_WITH_WX */
