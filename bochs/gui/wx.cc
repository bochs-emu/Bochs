/////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////
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
//
// wxWidgets VGA display for Bochs.  wx.cc implements a custom
// wxPanel called a MyPanel, which has methods to display
// text and VGA graphics on the panel.  Normally, a MyPanel
// is instantiated within a MyFrame created by wxmain.cc, but
// this is not a requirement.
//
// See the top of the source file wxmain.cc for more information.
//
/////////////////////////////////////////////////////////////////

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "param_names.h"
#include "keymap.h"
#include "iodev.h"
#if BX_WITH_WX

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/image.h>
#include <wx/clipbrd.h>
#include <wx/display.h>

//#include "gui/icon_bochs.h"
#include "osdep.h"
#include "font/vga.bitmap.h"

// shared elements between wxmain.cc and this file
#include "wxmain.h"


//////////////////////////////////////////////////////////////
// plugin support
//////////////////////////////////////////////////////////////
class bx_wx_gui_c : public bx_gui_c {
public:
  bx_wx_gui_c(void) {}
  DECLARE_GUI_VIRTUAL_METHODS()
  DECLARE_GUI_NEW_VIRTUAL_METHODS()
  virtual void statusbar_setitem_specific(int element, bx_bool active, bx_bool w);
  virtual void get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp);
  virtual void set_mouse_mode_absxy(bx_bool mode);
#if BX_SHOW_IPS
  void show_ips(Bit32u ips_count);
#endif
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_wx_gui_c *theGui = NULL;

void MyPanel::OnPluginInit()
{
  theGui = new bx_wx_gui_c();
  bx_gui = theGui;
}

#define LOG_THIS theGui->

//////////////////////////////////////////////////////////////
// data for wx gui
//////////////////////////////////////////////////////////////
// The bits to be displayed on the VGA screen are stored in wxScreen.
// wxScreen is an array (size=width*height*3) of RGB values.  Each
// pixel is represented by three bytes, one for red, green, and blue.
static wxRect wx_maxres;
static char *wxScreen = NULL;
wxCriticalSection wxScreen_lock;
static long wxScreenX = 0;
static long wxScreenY = 0;
static bx_bool wxScreenCheckSize = 0;
static unsigned wxTileX = 0;
static unsigned wxTileY = 0;
static unsigned long wxCursorX = 0;
static unsigned long wxCursorY = 0;
static bx_bool wxMouseModeAbsXY = 0;
static unsigned long wxFontX = 0;
static unsigned long wxFontY = 0;
static unsigned int text_rows=25, text_cols=80;
static Bit8u h_panning = 0, v_panning = 0;
static Bit16u line_compare = 1023;
static unsigned disp_bpp = 8;
static struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} wxBochsPalette[256];
wxCriticalSection event_thread_lock;
BxEvent event_queue[MAX_EVENTS];
unsigned long num_events;
static bx_bool mouse_captured = 0;
static bx_bool wx_hide_ips = 0;
#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXGTK__)
static Bit32u convertStringToGDKKey (const char *string);
#endif


//////////////////////////////////////////////////////////////
// and now, the code
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// define the MyPanel which implements the VGA screen
//////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
  EVT_KEY_DOWN(MyPanel::OnKeyDown)
  EVT_KEY_UP(MyPanel::OnKeyUp)
  EVT_TIMER(-1, MyPanel::OnTimer)
  EVT_PAINT(MyPanel::OnPaint)
  EVT_MOUSE_EVENTS(MyPanel::OnMouse)
END_EVENT_TABLE()

MyPanel::MyPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : wxPanel (parent, id, pos, size, style, name)
{
  wxLogDebug (wxT ("MyPanel constructor"));
  refreshTimer.SetOwner (this);
  refreshTimer.Start (100);
  needRefresh = true;
  const char bits[1] = { 0 };
  blankCursor = new wxCursor (bits, 1, 1, -1, -1, bits);
  thePanel = this;
}

MyPanel::~MyPanel()
{
  delete blankCursor;
  thePanel = NULL;
}

void MyPanel::OnTimer(wxTimerEvent& WXUNUSED(event))
{
  int cx, cy;

  if (wxScreenCheckSize) {
    theFrame->GetClientSize(&cx, &cy);
    if ((cx != wxScreenX) || (cy != wxScreenY)) {
      theFrame->SetClientSize(wxScreenX, wxScreenY);
    }
    wxScreenCheckSize = 0;
  }
  IFDBG_VGA(wxLogDebug(wxT("timer")));
  if (needRefresh) {
    IFDBG_VGA(wxLogDebug(wxT("calling refresh")));
    Refresh(FALSE);
  }
#if BX_SHOW_IPS
  static int i = 10;
  if (--i <= 0) {
    bx_show_ips_handler();
    i = 10;
  }
#endif
}

void MyPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  IFDBG_VGA (wxLogDebug (wxT ("OnPaint")));
  //PrepareDC(dc);

  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::OnPaint trying to get lock. wxScreen=%p", wxScreen)));
  wxCriticalSectionLocker lock(wxScreen_lock);
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::OnPaint got lock. wxScreen=%p", wxScreen)));
  if(wxScreen != NULL) {
    wxPoint pt = GetClientAreaOrigin();
    wxImage screenImage(wxScreenX, wxScreenY, (unsigned char *)wxScreen, TRUE);
    IFDBG_VGA(wxLogDebug (wxT ("drawBitmap")));
    dc.DrawBitmap(wxBitmap(screenImage), pt.x, pt.y, FALSE);
  }
  needRefresh = false;
}

void MyPanel::ToggleMouse(bool fromToolbar)
{
  static bool first_enable = true;
  bx_param_bool_c *enable = SIM->get_param_bool(BXPN_MOUSE_ENABLED);
  bool en = ! enable->get();
  bool is_main_thread = wxThread::IsMain();
  bool needmutex = !is_main_thread && SIM->is_sim_thread();
  if (needmutex) wxMutexGuiEnter();
  if (fromToolbar && first_enable && en) {
    // only show this help if you click on the toolbar.  If they already
    // know the shortcut, don't annoy them with the message.
    wxString msg;
    msg.Printf(wxT(
      "You have enabled the mouse in Bochs, so now your mouse actions will\n"
      "be sent into the simulator.  The usual mouse cursor will be trapped\n"
      "inside the Bochs window until you press %s\n"
      "to turn mouse capture off."), theGui->get_toggle_info());
    wxMessageBox(msg, wxT("Mouse Capture Enabled"), wxOK | wxICON_INFORMATION);
    first_enable = false;
  }
  enable->set(en);
  IFDBG_MOUSE(wxLogDebug (wxT ("now mouse is %sabled", en ? "en" : "dis")));
  if (en) {
    mouseSavedX = wxScreenX / 2;
    mouseSavedY = wxScreenY / 2;
    WarpPointer(mouseSavedX, mouseSavedY);
#if defined(__WXMSW__)
    ShowCursor(0);
#else
    SetCursor(*blankCursor);
#endif
  } else {
#if defined(__WXMSW__)
    ShowCursor(1);
#else
    SetCursor(wxNullCursor);
#endif
  }
  if (needmutex) wxMutexGuiLeave();
}

void MyPanel::OnMouse(wxMouseEvent& event)
{
  long x,y;
  int wheel = 0;

  if (theFrame->GetSimThread() == NULL)
    return;

  event.GetPosition(&x, &y);
  IFDBG_MOUSE (
    if (event.IsButton()) {
      wxLogDebug(wxT("mouse button event at %d,%d", x, y));
    } else if (event.Entering()) {
      wxLogDebug(wxT("mouse entering at %d,%d", x, y));
    } else if (event.Leaving()) {
      wxLogDebug(wxT("mouse leaving at %d,%d", x, y));
    } else if (event.Moving() || event.Dragging()) {
      wxLogDebug(wxT("mouse moved to %d,%d", x, y));
    } else {
      wxLogDebug(wxT("other mouse event at %d,%d", x, y));
    }
  )

  if (event.MiddleDown()) {
    if (bx_gui->mouse_toggle_check(BX_MT_MBUTTON, 1)) {
      ToggleMouse(false);
      return;
    }
  } else if (event.MiddleUp()) {
    bx_gui->mouse_toggle_check(BX_MT_MBUTTON, 0);
  }

  if (!mouse_captured)
    return;  // mouse disabled, ignore the event

  // process buttons and motion together
  Bit32u buttons;
  buttons  = event.LeftIsDown()  ? 1 : 0;
  buttons |= event.RightIsDown() ? 2 : 0;
  buttons |= event.MiddleIsDown() ? 4 : 0;
  if (event.GetWheelRotation() != 0) {
    wheel = event.GetWheelRotation() / event.GetWheelDelta();
  }
  if ((x == mouseSavedX) && (y == mouseSavedY) && !event.IsButton() && (wheel == 0)) {
    // nothing happened.  This could have been generated by the WarpPointer.
    return;
  } else {
    if(num_events < MAX_EVENTS) {
      wxCriticalSectionLocker lock(event_thread_lock);
      Bit16s dx = x - mouseSavedX;
      Bit16s dy = y - mouseSavedY;
      IFDBG_MOUSE (wxLogDebug (wxT ("mouse moved by delta %d,%d", dx, dy)));
      event_queue[num_events].type = BX_ASYNC_EVT_MOUSE;
      if (wxMouseModeAbsXY) {
        event_queue[num_events].u.mouse.dx = x * 0x7fff / wxScreenX;
        event_queue[num_events].u.mouse.dy = y * 0x7fff / wxScreenY;
      } else {
        event_queue[num_events].u.mouse.dx = dx;
        event_queue[num_events].u.mouse.dy = -dy;
      }
      event_queue[num_events].u.mouse.dz = wheel;
      event_queue[num_events].u.mouse.buttons = buttons;
      num_events++;
      mouseSavedX = x;
      mouseSavedY = y;
    } else {
      wxLogDebug (wxT ("mouse event skipped because event queue full"));
    }
  }

  if (!wxMouseModeAbsXY) {
    mouseSavedX = wxScreenX / 2;
    mouseSavedY = wxScreenY / 2;
    WarpPointer(mouseSavedX, mouseSavedY);
  }
  // The WarpPointer moves the pointer back to the middle of the
  // screen.  This WILL produce another mouse motion event, which needs
  // to be ignored.  It will be ignored because the new motion event
  // will move the cursor to (mouseSavedX, mouseSavedY).
}

void MyPanel::MyRefresh()
{
  IFDBG_VGA (wxLogDebug (wxT ("set needRefresh=true")));
  needRefresh = true;
}

void MyPanel::OnKeyDown(wxKeyEvent& event)
{
  wxCriticalSectionLocker lock(event_thread_lock);
  if(num_events < MAX_EVENTS) {
    event_queue[num_events].type = BX_ASYNC_EVT_KEY;
    fillBxKeyEvent (event, event_queue[num_events].u.key, false);
    num_events++;
  }
}


void MyPanel::OnKeyUp(wxKeyEvent& event)
{
  wxCriticalSectionLocker lock(event_thread_lock);
  if(num_events < MAX_EVENTS) {
    event_queue[num_events].type = BX_ASYNC_EVT_KEY;
    fillBxKeyEvent (event, event_queue[num_events].u.key, true);
    num_events++;
  }
}

/// copied right out of gui/x.cc
static char wxAsciiKey[0x5f] = {
  //  !"#$%&'
  BX_KEY_SPACE,
  BX_KEY_1,
  BX_KEY_SINGLE_QUOTE,
  BX_KEY_3,
  BX_KEY_4,
  BX_KEY_5,
  BX_KEY_7,
  BX_KEY_SINGLE_QUOTE,

  // ()*+,-./
  BX_KEY_9,
  BX_KEY_0,
  BX_KEY_8,
  BX_KEY_EQUALS,
  BX_KEY_COMMA,
  BX_KEY_MINUS,
  BX_KEY_PERIOD,
  BX_KEY_SLASH,

  // 01234567
  BX_KEY_0,
  BX_KEY_1,
  BX_KEY_2,
  BX_KEY_3,
  BX_KEY_4,
  BX_KEY_5,
  BX_KEY_6,
  BX_KEY_7,

  // 89:;<=>?
  BX_KEY_8,
  BX_KEY_9,
  BX_KEY_SEMICOLON,
  BX_KEY_SEMICOLON,
  BX_KEY_COMMA,
  BX_KEY_EQUALS,
  BX_KEY_PERIOD,
  BX_KEY_SLASH,

  // @ABCDEFG
  BX_KEY_2,
  BX_KEY_A,
  BX_KEY_B,
  BX_KEY_C,
  BX_KEY_D,
  BX_KEY_E,
  BX_KEY_F,
  BX_KEY_G,


  // HIJKLMNO
  BX_KEY_H,
  BX_KEY_I,
  BX_KEY_J,
  BX_KEY_K,
  BX_KEY_L,
  BX_KEY_M,
  BX_KEY_N,
  BX_KEY_O,


  // PQRSTUVW
  BX_KEY_P,
  BX_KEY_Q,
  BX_KEY_R,
  BX_KEY_S,
  BX_KEY_T,
  BX_KEY_U,
  BX_KEY_V,
  BX_KEY_W,

  // XYZ[\]^_
  BX_KEY_X,
  BX_KEY_Y,
  BX_KEY_Z,
  BX_KEY_LEFT_BRACKET,
  BX_KEY_BACKSLASH,
  BX_KEY_RIGHT_BRACKET,
  BX_KEY_6,
  BX_KEY_MINUS,

  // `abcdefg
  BX_KEY_GRAVE,
  BX_KEY_A,
  BX_KEY_B,
  BX_KEY_C,
  BX_KEY_D,
  BX_KEY_E,
  BX_KEY_F,
  BX_KEY_G,

  // hijklmno
  BX_KEY_H,
  BX_KEY_I,
  BX_KEY_J,
  BX_KEY_K,
  BX_KEY_L,
  BX_KEY_M,
  BX_KEY_N,
  BX_KEY_O,

  // pqrstuvw
  BX_KEY_P,
  BX_KEY_Q,
  BX_KEY_R,
  BX_KEY_S,
  BX_KEY_T,
  BX_KEY_U,
  BX_KEY_V,
  BX_KEY_W,

  // xyz{|}~
  BX_KEY_X,
  BX_KEY_Y,
  BX_KEY_Z,
  BX_KEY_LEFT_BRACKET,
  BX_KEY_BACKSLASH,
  BX_KEY_RIGHT_BRACKET,
  BX_KEY_GRAVE
};

// copied from gui/win32.cc
Bit32u wxMSW_to_bx_key[0x59] = {
  /* 0x00 - 0x0f */
  0,
  BX_KEY_ESC,
  BX_KEY_1,
  BX_KEY_2,
  BX_KEY_3,
  BX_KEY_4,
  BX_KEY_5,
  BX_KEY_6,
  BX_KEY_7,
  BX_KEY_8,
  BX_KEY_9,
  BX_KEY_0,
  BX_KEY_MINUS,
  BX_KEY_EQUALS,
  BX_KEY_BACKSPACE,
  BX_KEY_TAB,
  /* 0x10 - 0x1f */
  BX_KEY_Q,
  BX_KEY_W,
  BX_KEY_E,
  BX_KEY_R,
  BX_KEY_T,
  BX_KEY_Y,
  BX_KEY_U,
  BX_KEY_I,
  BX_KEY_O,
  BX_KEY_P,
  BX_KEY_LEFT_BRACKET,
  BX_KEY_RIGHT_BRACKET,
  BX_KEY_ENTER,
  BX_KEY_CTRL_L,
  BX_KEY_A,
  BX_KEY_S,
  /* 0x20 - 0x2f */
  BX_KEY_D,
  BX_KEY_F,
  BX_KEY_G,
  BX_KEY_H,
  BX_KEY_J,
  BX_KEY_K,
  BX_KEY_L,
  BX_KEY_SEMICOLON,
  BX_KEY_SINGLE_QUOTE,
  BX_KEY_GRAVE,
  BX_KEY_SHIFT_L,
  BX_KEY_BACKSLASH,
  BX_KEY_Z,
  BX_KEY_X,
  BX_KEY_C,
  BX_KEY_V,
  /* 0x30 - 0x3f */
  BX_KEY_B,
  BX_KEY_N,
  BX_KEY_M,
  BX_KEY_COMMA,
  BX_KEY_PERIOD,
  BX_KEY_SLASH,
  BX_KEY_SHIFT_R,
  BX_KEY_KP_MULTIPLY,
  BX_KEY_ALT_L,
  BX_KEY_SPACE,
  BX_KEY_CAPS_LOCK,
  BX_KEY_F1,
  BX_KEY_F2,
  BX_KEY_F3,
  BX_KEY_F4,
  BX_KEY_F5,
  /* 0x40 - 0x4f */
  BX_KEY_F6,
  BX_KEY_F7,
  BX_KEY_F8,
  BX_KEY_F9,
  BX_KEY_F10,
  BX_KEY_PAUSE,
  BX_KEY_SCRL_LOCK,
  BX_KEY_KP_HOME,
  BX_KEY_KP_UP,
  BX_KEY_KP_PAGE_UP,
  BX_KEY_KP_SUBTRACT,
  BX_KEY_KP_LEFT,
  BX_KEY_KP_5,
  BX_KEY_KP_RIGHT,
  BX_KEY_KP_ADD,
  BX_KEY_KP_END,
  /* 0x50 - 0x58 */
  BX_KEY_KP_DOWN,
  BX_KEY_KP_PAGE_DOWN,
  BX_KEY_KP_INSERT,
  BX_KEY_KP_DELETE,
  0,
  0,
  BX_KEY_LEFT_BACKSLASH,
  BX_KEY_F11,
  BX_KEY_F12
};

#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXMSW__)
// get windows specific definitions. At present the only thing needed
// is the definition of HIWORD.

// windows.h included by bochs.h, so nothing extra is required here.
#endif

// MS Windows specific key mapping, which uses wxKeyEvent::m_rawCode & 2.
bx_bool MyPanel::fillBxKeyEvent_MSW (wxKeyEvent& wxev, BxKeyEvent& bxev, bx_bool release)
{
#if defined(wxHAS_RAW_KEY_CODES) && defined(__WXMSW__)
  IFDBG_KEY(wxLogDebug (wxT ("fillBxKeyEvent_MSW. key code %d, raw codes %d %d", wxev.m_keyCode, wxev.m_rawCode, wxev.m_rawFlags)));
  // this code was grabbed from gui/win32.cpp
  Bit32u lParam = wxev.m_rawFlags;
  Bit32u key = HIWORD (lParam) & 0x01FF;
  bxev.bx_key = 0x0000;
  if (key & 0x0100) {
    // Its an extended key
    bxev.bx_key = 0xE000;
  }
  // Its a key
  bxev.bx_key |= (key & 0x00FF) | (release? 0x80 : 0x00);
  bxev.raw_scancode = true;
  return true;
#else
  return false;
#endif
}

#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXGTK__)
// get those keysym definitions
#include <gdk/gdkkeysyms.h>
#endif

// GTK specific key mapping, which uses wxKeyEvent::m_rawCode.
bx_bool MyPanel::fillBxKeyEvent_GTK (wxKeyEvent& wxev, BxKeyEvent& bxev, bx_bool release)
{
#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXGTK__)
  IFDBG_KEY(wxLogDebug (wxT ("fillBxKeyEvent_GTK. key code %ld, raw codes %d %d", wxev.m_keyCode, wxev.m_rawCode, wxev.m_rawFlags)));
  // GTK has only 16bit key codes
  Bit16u keysym = (Bit32u) wxev.m_rawCode;
  Bit32u key_event = 0;
  // since the GDK_* symbols are very much like the X11 symbols (possibly
  // identical), I'm using code that is copied from gui/x.cc.
  if(!SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get()) {
    if (keysym >= GDK_space && keysym < GDK_asciitilde) {
      // use nice ASCII conversion table, based on x.cc
      key_event = wxAsciiKey[keysym - GDK_space];
    } else switch (keysym) {
        case GDK_KP_1:
#ifdef GDK_KP_End
        case GDK_KP_End:
#endif
          key_event = BX_KEY_KP_END; break;

        case GDK_KP_2:
#ifdef GDK_KP_Down
        case GDK_KP_Down:
#endif
          key_event = BX_KEY_KP_DOWN; break;

        case GDK_KP_3:
#ifdef GDK_KP_Page_Down
        case GDK_KP_Page_Down:
#endif
          key_event = BX_KEY_KP_PAGE_DOWN; break;

        case GDK_KP_4:
#ifdef GDK_KP_Left
        case GDK_KP_Left:
#endif
          key_event = BX_KEY_KP_LEFT; break;

        case GDK_KP_5:
#ifdef GDK_KP_Begin
        case GDK_KP_Begin:
#endif
          key_event = BX_KEY_KP_5; break;

        case GDK_KP_6:
#ifdef GDK_KP_Right
        case GDK_KP_Right:
#endif
          key_event = BX_KEY_KP_RIGHT; break;

        case GDK_KP_7:
#ifdef GDK_KP_Home
        case GDK_KP_Home:
#endif
          key_event = BX_KEY_KP_HOME; break;

        case GDK_KP_8:
#ifdef GDK_KP_Up
        case GDK_KP_Up:
#endif
          key_event = BX_KEY_KP_UP; break;

        case GDK_KP_9:
#ifdef GDK_KP_Page_Up
        case GDK_KP_Page_Up:
#endif
          key_event = BX_KEY_KP_PAGE_UP; break;

        case GDK_KP_0:
#ifdef GDK_KP_Insert
        case GDK_KP_Insert:
#endif
          key_event = BX_KEY_KP_INSERT; break;

        case GDK_KP_Decimal:
#ifdef GDK_KP_Delete
        case GDK_KP_Delete:
#endif
          key_event = BX_KEY_KP_DELETE; break;

#ifdef GDK_KP_Enter
        case GDK_KP_Enter:    key_event = BX_KEY_KP_ENTER; break;
#endif

        case GDK_KP_Subtract: key_event = BX_KEY_KP_SUBTRACT; break;
        case GDK_KP_Add:      key_event = BX_KEY_KP_ADD; break;

        case GDK_KP_Multiply: key_event = BX_KEY_KP_MULTIPLY; break;
        case GDK_KP_Divide:   key_event = BX_KEY_KP_DIVIDE; break;


        case GDK_Up:          key_event = BX_KEY_UP; break;
        case GDK_Down:        key_event = BX_KEY_DOWN; break;
        case GDK_Left:        key_event = BX_KEY_LEFT; break;
        case GDK_Right:       key_event = BX_KEY_RIGHT; break;


        case GDK_Delete:      key_event = BX_KEY_DELETE; break;
        case GDK_BackSpace:   key_event = BX_KEY_BACKSPACE; break;
        case GDK_Tab:         key_event = BX_KEY_TAB; break;
#ifdef GDK_ISO_Left_Tab
        case GDK_ISO_Left_Tab: key_event = BX_KEY_TAB; break;
#endif
        case GDK_Return:      key_event = BX_KEY_ENTER; break;
        case GDK_Escape:      key_event = BX_KEY_ESC; break;
        case GDK_F1:          key_event = BX_KEY_F1; break;
        case GDK_F2:          key_event = BX_KEY_F2; break;
        case GDK_F3:          key_event = BX_KEY_F3; break;
        case GDK_F4:          key_event = BX_KEY_F4; break;
        case GDK_F5:          key_event = BX_KEY_F5; break;
        case GDK_F6:          key_event = BX_KEY_F6; break;
        case GDK_F7:          key_event = BX_KEY_F7; break;
        case GDK_F8:          key_event = BX_KEY_F8; break;
        case GDK_F9:          key_event = BX_KEY_F9; break;
        case GDK_F10:         key_event = BX_KEY_F10; break;
        case GDK_F11:         key_event = BX_KEY_F11; break;
        case GDK_F12:         key_event = BX_KEY_F12; break;
        case GDK_Control_L:   key_event = BX_KEY_CTRL_L; break;
#ifdef GDK_Control_R
        case GDK_Control_R:   key_event = BX_KEY_CTRL_R; break;
#endif
        case GDK_Shift_L:     key_event = BX_KEY_SHIFT_L; break;
        case GDK_Shift_R:     key_event = BX_KEY_SHIFT_R; break;
        case GDK_Alt_L:       key_event = BX_KEY_ALT_L; break;
#ifdef GDK_Alt_R
        case GDK_Alt_R:       key_event = BX_KEY_ALT_R; break;
#endif
        case GDK_Caps_Lock:   key_event = BX_KEY_CAPS_LOCK; break;
        case GDK_Num_Lock:    key_event = BX_KEY_NUM_LOCK; break;
#ifdef GDK_Scroll_Lock
        case GDK_Scroll_Lock: key_event = BX_KEY_SCRL_LOCK; break;
#endif
#ifdef GDK_Print
        case GDK_Print:       key_event = BX_KEY_PRINT; break;
#endif
#ifdef GDK_Pause
        case GDK_Pause:       key_event = BX_KEY_PAUSE; break;
#endif

        case GDK_Insert:      key_event = BX_KEY_INSERT; break;
        case GDK_Home:        key_event = BX_KEY_HOME; break;
        case GDK_End:         key_event = BX_KEY_END; break;
        case GDK_Page_Up:     key_event = BX_KEY_PAGE_UP; break;
        case GDK_Page_Down:   key_event = BX_KEY_PAGE_DOWN; break;

#ifdef GDK_Menu
        case GDK_Menu:        key_event = BX_KEY_MENU; break;
#endif
#ifdef GDK_Super_L
        case GDK_Super_L:     key_event = BX_KEY_WIN_L; break;
#endif
#ifdef GDK_Super_R
        case GDK_Super_R:     key_event = BX_KEY_WIN_R; break;
#endif

        default:
          wxLogError(wxT("fillBxKeyEvent_GTK(): keysym %x unhandled!"), (unsigned) keysym);
          return BX_KEY_UNHANDLED;
    }
  } else {
    /* use mapping */
    BXKeyEntry *entry = bx_keymap.findHostKey (keysym);
    if (!entry) {
      BX_ERROR(("fillBxKeyEvent_GTK(): keysym %x unhandled!", (unsigned) keysym));
      return BX_KEY_UNHANDLED;
    }
    key_event = entry->baseKey;
  }
  bxev.bx_key = key_event | (release? BX_KEY_RELEASED : BX_KEY_PRESSED);
  bxev.raw_scancode = false;
  return true;
#else   // if GTK toolkit
  return false;
#endif
}

bx_bool MyPanel::fillBxKeyEvent(wxKeyEvent& wxev, BxKeyEvent& bxev, bx_bool release)
{
  Bit32u key = wxev.m_keyCode;
  bx_bool mouse_toggle = 0;

  if (theFrame->GetSimThread() == NULL)
    return false;

  if (key == WXK_CONTROL) {
    mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_CTRL, !release);
  } else if (key == WXK_ALT) {
    mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_ALT, !release);
  } else if (key == WXK_F10) {
    mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_F10, !release);
  } else if (key == WXK_F12) {
    mouse_toggle = bx_gui->mouse_toggle_check(BX_MT_KEY_F12, !release);
  }
  if (mouse_toggle) {
    ToggleMouse(false);
    return false;
  }

  // Use raw codes if they are available.  Raw codes are an addition to the
  // wxWidgets library introduced in version 2.3.3.

#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXMSW__)
  return fillBxKeyEvent_MSW(wxev, bxev, release);
#endif

#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXGTK__)
  return fillBxKeyEvent_GTK(wxev, bxev, release);
#endif

  // otherwise fall back to using portable WXK_* keycodes.  Not all keys
  // can be mapped correctly using WXK_* codes but it should be usable.
  IFDBG_KEY (wxLogDebug (wxT ("fillBxKeyEvent. key code %ld", wxev.m_keyCode)));
  Bit32u bx_key;

  if(key >= WXK_SPACE && key < WXK_DELETE) {
    bx_key = wxAsciiKey[key - WXK_SPACE];
  } else {
    // handle extended keys here
    switch(key) {
    case WXK_BACK:                 bx_key = BX_KEY_BACKSPACE;    break;
    case WXK_TAB:                  bx_key = BX_KEY_TAB;          break;
    case WXK_RETURN:               bx_key = BX_KEY_ENTER;        break;
    case WXK_ESCAPE:               bx_key = BX_KEY_ESC;          break;
    case WXK_DELETE:               bx_key = BX_KEY_DELETE;       break;
    case WXK_SHIFT:                bx_key = BX_KEY_SHIFT_L;      break;
    case WXK_CONTROL:              bx_key = BX_KEY_CTRL_L;       break;
    case WXK_ALT:                  bx_key = BX_KEY_ALT_L;        break;
    case WXK_MENU:                 bx_key = BX_KEY_MENU;         break;
    case WXK_PAUSE:                bx_key = BX_KEY_PAUSE;        break;
    case WXK_PRIOR:                bx_key = BX_KEY_PAGE_UP;      break;
    case WXK_NEXT:                 bx_key = BX_KEY_PAGE_DOWN;    break;
    case WXK_END:                  bx_key = BX_KEY_END;          break;
    case WXK_HOME:                 bx_key = BX_KEY_HOME;         break;
    case WXK_LEFT:                 bx_key = BX_KEY_LEFT;         break;
    case WXK_UP:                   bx_key = BX_KEY_UP;           break;
    case WXK_RIGHT:                bx_key = BX_KEY_RIGHT;        break;
    case WXK_DOWN:                 bx_key = BX_KEY_DOWN;         break;
    case WXK_INSERT:               bx_key = BX_KEY_INSERT;       break;
    case WXK_NUMPAD0:              bx_key = BX_KEY_KP_INSERT;    break;
    case WXK_NUMPAD1:              bx_key = BX_KEY_KP_END;       break;
    case WXK_NUMPAD2:              bx_key = BX_KEY_KP_DOWN;      break;
    case WXK_NUMPAD3:              bx_key = BX_KEY_KP_PAGE_DOWN; break;
    case WXK_NUMPAD4:              bx_key = BX_KEY_KP_LEFT;      break;
    case WXK_NUMPAD5:              bx_key = BX_KEY_KP_5;         break;
    case WXK_NUMPAD6:              bx_key = BX_KEY_KP_RIGHT;     break;
    case WXK_NUMPAD7:              bx_key = BX_KEY_KP_HOME;      break;
    case WXK_NUMPAD8:              bx_key = BX_KEY_KP_UP;        break;
    case WXK_NUMPAD9:              bx_key = BX_KEY_KP_PAGE_UP;   break;
    case WXK_F1:                   bx_key = BX_KEY_F1;           break;
    case WXK_F2:                   bx_key = BX_KEY_F2;           break;
    case WXK_F3:                   bx_key = BX_KEY_F3;           break;
    case WXK_F4:                   bx_key = BX_KEY_F4;           break;
    case WXK_F5:                   bx_key = BX_KEY_F5;           break;
    case WXK_F6:                   bx_key = BX_KEY_F6;           break;
    case WXK_F7:                   bx_key = BX_KEY_F7;           break;
    case WXK_F8:                   bx_key = BX_KEY_F8;           break;
    case WXK_F9:                   bx_key = BX_KEY_F9;           break;
    case WXK_F10:                  bx_key = BX_KEY_F10;          break;
    case WXK_F11:                  bx_key = BX_KEY_F11;          break;
    case WXK_F12:                  bx_key = BX_KEY_F12;          break;
    case WXK_NUMLOCK:              bx_key = BX_KEY_NUM_LOCK;     break;
    case WXK_SCROLL:               bx_key = BX_KEY_SCRL_LOCK;    break;
    case WXK_DECIMAL:              bx_key = BX_KEY_PERIOD;       break;
    case WXK_SUBTRACT:             bx_key = BX_KEY_MINUS;        break;
    case WXK_ADD:                  bx_key = BX_KEY_EQUALS;       break;
    case WXK_MULTIPLY:             bx_key = BX_KEY_KP_MULTIPLY;  break;
    case WXK_DIVIDE:               bx_key = BX_KEY_KP_DIVIDE;    break;

    case WXK_NUMPAD_ENTER:         bx_key = BX_KEY_KP_ENTER;     break;
    case WXK_NUMPAD_HOME:          bx_key = BX_KEY_KP_HOME;      break;
    case WXK_NUMPAD_LEFT:          bx_key = BX_KEY_KP_LEFT;      break;
    case WXK_NUMPAD_UP:            bx_key = BX_KEY_KP_UP;        break;
    case WXK_NUMPAD_RIGHT:         bx_key = BX_KEY_KP_RIGHT;     break;
    case WXK_NUMPAD_DOWN:          bx_key = BX_KEY_KP_DOWN;      break;
    case WXK_NUMPAD_PRIOR:         bx_key = BX_KEY_KP_PAGE_UP;   break;
#if WXK_NUMPAD_PAGEUP != WXK_NUMPAD_PRIOR
    case WXK_NUMPAD_PAGEUP:        bx_key = BX_KEY_KP_PAGE_UP;   break;
#endif
    case WXK_NUMPAD_NEXT:          bx_key = BX_KEY_KP_PAGE_DOWN; break;
#if WXK_NUMPAD_PAGEDOWN != WXK_NUMPAD_NEXT
    case WXK_NUMPAD_PAGEDOWN:      bx_key = BX_KEY_KP_PAGE_DOWN; break;
#endif
    case WXK_NUMPAD_END:           bx_key = BX_KEY_KP_END;       break;
    case WXK_NUMPAD_BEGIN:         bx_key = BX_KEY_KP_HOME;      break;
    case WXK_NUMPAD_INSERT:        bx_key = BX_KEY_KP_INSERT;    break;
    case WXK_NUMPAD_DELETE:        bx_key = BX_KEY_KP_DELETE;    break;
    case WXK_NUMPAD_EQUAL:         bx_key = BX_KEY_KP_ENTER;     break;
    case WXK_NUMPAD_MULTIPLY:      bx_key = BX_KEY_KP_MULTIPLY;  break;
    case WXK_NUMPAD_SUBTRACT:      bx_key = BX_KEY_KP_SUBTRACT;  break;
    case WXK_NUMPAD_DECIMAL:       bx_key = BX_KEY_KP_DELETE;    break;
    case WXK_NUMPAD_DIVIDE:        bx_key = BX_KEY_KP_DIVIDE;    break;

    // Keys not handled by wxMSW
    case 20:  bx_key = BX_KEY_CAPS_LOCK;     break; // =+
    case 186: bx_key = BX_KEY_SEMICOLON;     break; // ;:
    case 187: bx_key = BX_KEY_EQUALS;        break; // =+
    case 188: bx_key = BX_KEY_COMMA;         break; // ,<
    case 189: bx_key = BX_KEY_MINUS;         break; // -_
    case 190: bx_key = BX_KEY_PERIOD;        break; // .>
    case 191: bx_key = BX_KEY_SLASH;         break; // /?
    case 192: bx_key = BX_KEY_GRAVE;         break; // `~
    case 219: bx_key = BX_KEY_LEFT_BRACKET;  break; // [{
    case 221: bx_key = BX_KEY_RIGHT_BRACKET; break; // ]}
    case 220: bx_key = BX_KEY_BACKSLASH;     break; // \|
    case 222: bx_key = BX_KEY_SINGLE_QUOTE;  break; // '"
    case 305: bx_key = BX_KEY_KP_5;          break; // keypad 5
//#if defined(WXK_NUMPAD_ADD)
    case WXK_NUMPAD_ADD: bx_key = BX_KEY_KP_ADD; break; // keypad plus
//#else
//    case 392: bx_key = BX_KEY_KP_ADD;        break; // keypad plus
//#endif

    default:
      wxLogMessage(wxT ("Unhandled key event: %i (0x%x)"), key, key);
      return 0;
    }
  }
  IFDBG_KEY (wxLogDebug (wxT ("fillBxKeyEvent: after remapping, key=%d"), bx_key));
  bxev.bx_key = bx_key | (release? BX_KEY_RELEASED : BX_KEY_PRESSED);
  bxev.raw_scancode = false;
  return true;
}

#if defined(WIN32) && BX_DEBUGGER && BX_DEBUGGER_GUI
DWORD WINAPI DebugGuiThread(LPVOID)
{
  MSG msg;

  bx_gui->init_debug_dialog();
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}
#endif

//////////////////////////////////////////////////////////////
// fill in methods of bx_gui
//////////////////////////////////////////////////////////////

void bx_wx_gui_c::specific_init(int argc, char **argv, unsigned headerbar_y)
{
  int b,i,j;
  unsigned char fc, vc;
  wxDisplay display;

  put("WX");
  wx_maxres = display.GetGeometry();
  info("Current display dimensions %d x %d", wx_maxres.GetWidth(), wx_maxres.GetHeight());
  if (SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get()) {
    BX_INFO(("private_colormap option ignored."));
  }

  for(i = 0; i < 256; i++) {
    wxBochsPalette[i].red = 0;
    wxBochsPalette[i].green = 0;
    wxBochsPalette[i].blue = 0;
  }

  for(i = 0; i < 256; i++) {
    for(j = 0; j < 16; j++) {
      vc = bx_vgafont[i].data[j];
      fc = 0;
      for (b = 0; b < 8; b++) {
        fc |= (vc & 0x01) << (7 - b);
        vc >>= 1;
      }
      vga_charmap[i*32+j] = fc;
    }
  }

  wxScreenX = 640;
  wxScreenY = 480;
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::specific_init trying to get lock. wxScreen=%p", wxScreen)));
  wxCriticalSectionLocker lock(wxScreen_lock);
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::specific_init got lock. wxScreen=%p", wxScreen)));
  if (wxScreen == NULL) {
    wxScreen = (char *)malloc(wxScreenX * wxScreenY * 3);
  } else {
    wxScreen = (char *)realloc(wxScreen, wxScreenX * wxScreenY * 3);
  }
  memset(wxScreen, 0, wxScreenX * wxScreenY * 3);

  wxTileX = x_tilesize;
  wxTileY = y_tilesize;

  // load keymap tables
  if (SIM->get_param_bool(BXPN_KBD_USEMAPPING)->get())
#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXGTK__)
    bx_keymap.loadKeymap(convertStringToGDKKey);
#else
    bx_keymap.loadKeymap(NULL);
#endif

  // parse x11 specific options
  if (argc > 1) {
    for (i = 1; i < argc; i++) {
#if BX_SHOW_IPS
      if (!strcmp(argv[i], "hideIPS")) {
        BX_INFO(("hide IPS display in status bar"));
        wx_hide_ips = 1;
      } else {
        BX_PANIC(("Unknown wx option '%s'", argv[i]));
      }
#else
      BX_PANIC(("Unknown wx option '%s'", argv[i]));
#endif
    }
  }

#if BX_DEBUGGER && BX_DEBUGGER_GUI
#ifdef WIN32
  // on Windows the debugger gui must run in a separate thread
  DWORD threadID;
  CreateThread(NULL, 0, DebugGuiThread, NULL, 0, &threadID);
#else
  wxMutexGuiEnter();
  init_debug_dialog();
  wxMutexGuiLeave();
#endif
#endif

  wxString msg;
  msg.Printf(wxT("Enable mouse capture\nThere is also a shortcut for this: %s."),
             theGui->get_toggle_info());
  theFrame->SetToolBarHelp(ID_Toolbar_Mouse_en, msg);

  num_events = 0;

  new_gfx_api = 1;
  dialog_caps = BX_GUI_DLG_USER | BX_GUI_DLG_SNAPSHOT | BX_GUI_DLG_SAVE_RESTORE;
}

// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

void bx_wx_gui_c::handle_events(void)
{
  bx_bool quit_sim = 0;
  { // critical section start
    wxCriticalSectionLocker lock(event_thread_lock);
    Bit32u bx_key = 0;
    for(unsigned int i = 0; i < num_events; i++) {
      switch(event_queue[i].type) {
        case BX_ASYNC_EVT_TOOLBAR:
          switch (event_queue[i].u.toolbar.button) {
            case BX_TOOLBAR_RESET: reset_handler(); break;
            case BX_TOOLBAR_POWER: quit_sim = 1; break;
            case BX_TOOLBAR_SAVE_RESTORE: save_restore_handler(); break;
            case BX_TOOLBAR_COPY: copy_handler(); break;
            case BX_TOOLBAR_PASTE: paste_handler(); break;
            case BX_TOOLBAR_SNAPSHOT: snapshot_handler(); break;
            case BX_TOOLBAR_USER: userbutton_handler(); break;
            default:
              wxLogDebug (wxT ("unknown toolbar id %d"), event_queue[i].u.toolbar.button);
          }
          break;
        case BX_ASYNC_EVT_KEY:
          bx_key = event_queue[i].u.key.bx_key;
          if (event_queue[i].u.key.raw_scancode) {
            // event contains raw scancodes: convert to BX_KEY values first
            bx_bool released = ((bx_key & 0x80) > 0);
            if (bx_key & 0xFF00) { // for extended keys
              switch (bx_key & 0x7f) {
                case 0x1C:
                  bx_key = BX_KEY_KP_ENTER;
                  break;
                case 0x1D:
                  bx_key = BX_KEY_CTRL_R;
                  break;
                case 0x35:
                  bx_key = BX_KEY_KP_DIVIDE;
                  break;
                case 0x38:
                  // This makes the "AltGr" key on European keyboards work
                  DEV_kbd_gen_scancode(BX_KEY_CTRL_L | BX_KEY_RELEASED);
                  bx_key = BX_KEY_ALT_R;
                  break;
                case 0x45:
                  bx_key = BX_KEY_NUM_LOCK;
                  break;
                case 0x47:
                  bx_key = BX_KEY_HOME;
                  break;
                case 0x48:
                  bx_key = BX_KEY_UP;
                  break;
                case 0x49:
                  bx_key = BX_KEY_PAGE_UP;
                  break;
                case 0x4B:
                  bx_key = BX_KEY_LEFT;
                  break;
                case 0x4D:
                  bx_key = BX_KEY_RIGHT;
                  break;
                case 0x4F:
                  bx_key = BX_KEY_END;
                  break;
                case 0x50:
                  bx_key = BX_KEY_DOWN;
                  break;
                case 0x51:
                  bx_key = BX_KEY_PAGE_DOWN;
                  break;
                case 0x52:
                  bx_key = BX_KEY_INSERT;
                  break;
                case 0x53:
                  bx_key = BX_KEY_DELETE;
                  break;
                case 0x5B:
                  bx_key = BX_KEY_WIN_L;
                  break;
                case 0x5C:
                  bx_key = BX_KEY_WIN_R;
                  break;
                case 0x5D:
                  bx_key = BX_KEY_MENU;
                  break;
              }
            } else {
              bx_key = wxMSW_to_bx_key[bx_key & 0x7f];
            }
            if (released) bx_key |= BX_KEY_RELEASED;
          }
          // event contains BX_KEY_* codes: use gen_scancode
          IFDBG_KEY (wxLogDebug (wxT ("sending key event 0x%02x", bx_key)));
          DEV_kbd_gen_scancode(bx_key);
          break;
        case BX_ASYNC_EVT_MOUSE:
          DEV_mouse_motion(
              event_queue[i].u.mouse.dx,
              event_queue[i].u.mouse.dy,
              event_queue[i].u.mouse.dz,
              event_queue[i].u.mouse.buttons, wxMouseModeAbsXY);
          break;
        default:
          wxLogError (wxT ("handle_events received unhandled event type %d in queue"), (int)event_queue[i].type);
      }
      if (quit_sim) break;
    }
    num_events = 0;
  } // critical section end
  if (quit_sim) {
    // power_handler() never returns, so it must be placed outside of the critical section
    power_handler();
  }
}

void bx_wx_gui_c::statusbar_setitem_specific(int element, bx_bool active, bx_bool w)
{
#if defined(__WXMSW__)
  char status_text[10];
#endif

  wxMutexGuiEnter();
  if (active) {
#if defined(__WXMSW__)
    status_text[0] = 9;
    strcpy(status_text+1, statusitem[element].text);
    theFrame->SetStatusText(status_text, element+1);
#else
    theFrame->SetStatusText(wxString(statusitem[element].text, wxConvUTF8),
                            element+1);
#endif
  } else {
    theFrame->SetStatusText(wxT(""), element+1);
  }
  wxMutexGuiLeave();
}

// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

void bx_wx_gui_c::flush(void)
{
}

// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.  Don't
// clear the area that defines the headerbar.

void bx_wx_gui_c::clear_screen(void)
{
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::clear_screen trying to get lock. wxScreen=%p", wxScreen)));
  wxCriticalSectionLocker lock(wxScreen_lock);
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::clear_screen got lock. wxScreen=%p", wxScreen)));
  memset(wxScreen, 0, wxScreenX * wxScreenY * 3);
  thePanel->MyRefresh ();
}

static void UpdateScreen(unsigned char *newBits, int x, int y, int width, int height)
{
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::UpdateScreen trying to get lock. wxScreen=%p", wxScreen)));
  wxCriticalSectionLocker lock(wxScreen_lock);
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::UpdateScreen got lock. wxScreen=%p", wxScreen)));
  if(wxScreen != NULL) {
    switch (disp_bpp) {
      case 8: /* 8 bpp */
        for(int i = 0; i < height; i++) {
          char *pwxScreen = &wxScreen[(y * wxScreenX * 3) + (x * 3)];
          for(int c = 0; c < width; c++) {
            unsigned pixel = (i * width) + c;
            pwxScreen[0] = wxBochsPalette[newBits[pixel]].red;
            pwxScreen[1] = wxBochsPalette[newBits[pixel]].green;
            pwxScreen[2] = wxBochsPalette[newBits[pixel]].blue;
            pwxScreen += 3;
          }
          y++;
          if(y >= wxScreenY) break;
        }
        break;
      default:
        BX_PANIC(("%u bpp modes handled by new graphics API", disp_bpp));
        return;
    }
  } else {
    IFDBG_VGA (wxLogDebug (wxT ("UpdateScreen with null wxScreen")));
  }
}

static void DrawBochsBitmap(int x, int y, int width, int height, char *bmap, char fgcolor, char bgcolor, int fontx, int fonty, bx_bool gfxchar)
{
  static unsigned char newBits[9 * 32];
  unsigned char mask;
  int bytes = width * height;

  if (y > wxScreenY) return;

  for(int i = 0; i < bytes; i+=width) {
    mask = 0x80 >> fontx;
    for(int j = 0; j < width; j++) {
      if (mask > 0) {
        newBits[i + j] = (bmap[fonty] & mask) ? fgcolor : bgcolor;
      } else {
        if (gfxchar) {
          newBits[i + j] = (bmap[fonty] & 0x01) ? fgcolor : bgcolor;
        } else {
          newBits[i + j] = bgcolor;
        }
      }
      mask >>= 1;
    }
    fonty++;
  }
  UpdateScreen(newBits, x, y, width, height);
}


// ::TEXT_UPDATE()
//
// Called in a VGA text mode, to update the screen with
// new content.
//
// old_text: array of character/attributes making up the contents
//           of the screen from the last call.  See below
// new_text: array of character/attributes making up the current
//           contents, which should now be displayed.  See below
//
// format of old_text & new_text: each is tm_info->line_offset*text_rows
//     bytes long. Each character consists of 2 bytes.  The first by is
//     the character value, the second is the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor
// tm_info:  this structure contains information for additional
//           features in text mode (cursor shape, line offset,...)

void bx_wx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                              unsigned long cursor_x, unsigned long cursor_y,
                              bx_vga_tminfo_t *tm_info)
{
  IFDBG_VGA(wxLogDebug (wxT ("text_update")));

  Bit8u *old_line, *new_line, *text_base;
  Bit8u cAttr, cChar;
  unsigned int curs, hchars, offset, rows, x, y, xc, yc, yc2, cs_y, i;
  Bit8u cfwidth, cfheight, cfheight2, font_col, font_row, font_row2;
  Bit8u split_textrow, split_fontrows;
  bx_bool forceUpdate = 0, gfxchar, split_screen, blink_state, blink_mode;
  Bit8u text_pal_idx[16];
  char bgcolor, fgcolor;

  // first check if the screen needs to be redrawn completely
  blink_mode = (tm_info->blink_flags & BX_TEXT_BLINK_MODE) > 0;
  blink_state = (tm_info->blink_flags & BX_TEXT_BLINK_STATE) > 0;
  if (blink_mode) {
    if (tm_info->blink_flags & BX_TEXT_BLINK_TOGGLE)
      forceUpdate = 1;
  }
  if(charmap_updated) {
    forceUpdate = 1;
    charmap_updated = 0;
  }
  for (i = 0; i < 16; i++) {
    text_pal_idx[i] = tm_info->actl_palette[i];
  }
  if((tm_info->h_panning != h_panning) || (tm_info->v_panning != v_panning)) {
    forceUpdate = 1;
    h_panning = tm_info->h_panning;
    v_panning = tm_info->v_panning;
  }
  if(tm_info->line_compare != line_compare) {
    forceUpdate = 1;
    line_compare = tm_info->line_compare;
  }

  // invalidate character at previous and new cursor location
  if((wxCursorY < text_rows) && (wxCursorX < text_cols)) {
    curs = wxCursorY * tm_info->line_offset + wxCursorX * 2;
    old_text[curs] = ~new_text[curs];
  }
  if((tm_info->cs_start <= tm_info->cs_end) && (tm_info->cs_start < wxFontY) &&
     (cursor_y < text_rows) && (cursor_x < text_cols)) {
    curs = cursor_y * tm_info->line_offset + cursor_x * 2;
    old_text[curs] = ~new_text[curs];
  } else {
    curs = 0xffff;
  }

  rows = text_rows;
  if (v_panning) rows++;
  y = 0;
  cs_y = 0;
  text_base = new_text - tm_info->start_address;
  if (line_compare < wxScreenY) {
    split_textrow = (line_compare + v_panning) / wxFontY;
    split_fontrows = ((line_compare + v_panning) % wxFontY) + 1;
  } else {
    split_textrow = rows + 1;
    split_fontrows = 0;
  }
  split_screen = 0;
  do {
    hchars = text_cols;
    if (h_panning) hchars++;
    if (split_screen) {
      yc = line_compare + cs_y * wxFontY + 1;
      font_row = 0;
      if (rows == 1) {
        cfheight = (wxScreenY - line_compare - 1) % wxFontY;
        if (cfheight == 0) cfheight = wxFontY;
      } else {
        cfheight = wxFontY;
      }
    } else if (v_panning) {
      if (y == 0) {
        yc = 0;
        font_row = v_panning;
        cfheight = wxFontY - v_panning;
      } else {
        yc = y * wxFontY - v_panning;
        font_row = 0;
        if (rows == 1) {
          cfheight = v_panning;
        } else {
          cfheight = wxFontY;
        }
      }
    } else {
      yc = y * wxFontY;
      font_row = 0;
      cfheight = wxFontY;
    }
    if (!split_screen && (y == split_textrow)) {
      if (split_fontrows < cfheight) cfheight = split_fontrows;
    }
    new_line = new_text;
    old_line = old_text;
    x = 0;
    offset = cs_y * tm_info->line_offset;
    do {
      if (h_panning) {
        if (hchars > text_cols) {
          xc = 0;
          font_col = h_panning;
          cfwidth = wxFontX - h_panning;
        } else {
          xc = x * wxFontX - h_panning;
          font_col = 0;
          if (hchars == 1) {
            cfwidth = h_panning;
          } else {
            cfwidth = wxFontX;
          }
        }
      } else {
        xc = x * wxFontX;
        font_col = 0;
        cfwidth = wxFontX;
      }
      if(forceUpdate || (old_text[0] != new_text[0])
         || (old_text[1] != new_text[1])) {
        cChar = new_text[0];
        if (blink_mode) {
          cAttr = new_text[1] & 0x7F;
          if (!blink_state && (new_text[1] & 0x80))
            cAttr = (cAttr & 0x70) | (cAttr >> 4);
        } else {
          cAttr = new_text[1];
        }
        gfxchar = tm_info->line_graphics && ((cChar & 0xE0) == 0xC0);
        bgcolor = text_pal_idx[(cAttr >> 4) & 0xF];
        fgcolor = text_pal_idx[cAttr & 0xF];
        DrawBochsBitmap(xc, yc, cfwidth, cfheight, (char *)&vga_charmap[cChar<<5],
                        fgcolor, bgcolor, font_col, font_row, gfxchar);
        if (offset == curs) {
          if (font_row == 0) {
            yc2 = yc + tm_info->cs_start;
            font_row2 = tm_info->cs_start;
            cfheight2 = tm_info->cs_end - tm_info->cs_start + 1;
          } else {
            if (v_panning > tm_info->cs_start) {
              yc2 = yc;
              font_row2 = font_row;
              cfheight2 = tm_info->cs_end - v_panning + 1;
            } else {
              yc2 = yc + tm_info->cs_start - v_panning;
              font_row2 = tm_info->cs_start;
              cfheight2 = tm_info->cs_end - tm_info->cs_start + 1;
            }
          }
          DrawBochsBitmap(xc, yc2, cfwidth, cfheight2, (char *)&vga_charmap[cChar<<5],
                          bgcolor, fgcolor, font_col, font_row2, gfxchar);
        }
      }
      x++;
      new_text+=2;
      old_text+=2;
      offset+=2;
    } while (--hchars);
    if (!split_screen && (y == split_textrow)) {
      new_text = text_base;
      forceUpdate = 1;
      cs_y = 0;
      if (tm_info->split_hpanning) h_panning = 0;
      rows = ((wxScreenY - line_compare + wxFontY - 2) / wxFontY) + 1;
      split_screen = 1;
    } else {
      y++;
      cs_y++;
      new_text = new_line + tm_info->line_offset;
      old_text = old_line + tm_info->line_offset;
    }
  } while (--rows);

  h_panning = tm_info->h_panning;
  wxCursorX = cursor_x;
  wxCursorY = cursor_y;

  thePanel->MyRefresh();
}

// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen update needed (redraw using current colormap)

bx_bool bx_wx_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue)
{
  IFDBG_VGA(wxLogDebug (wxT ("palette_change")));
  wxBochsPalette[index].red = red;
  wxBochsPalette[index].green = green;
  wxBochsPalette[index].blue = blue;
  return(1);  // screen update needed
}


// ::GRAPHICS_TILE_UPDATE()
//
// Called to request that a tile of graphics be drawn to the
// screen, since info in this region has changed.
//
// tile: array of 8bit values representing a block of pixels with
//       dimension equal to the 'x_tilesize' & 'y_tilesize' members.
//       Each value specifies an index into the
//       array of colors you allocated for ::palette_change()
// x0: x origin of tile
// y0: y origin of tile
//
// note: origin of tile and of window based on (0,0) being in the upper
//       left of the window.

void bx_wx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  IFDBG_VGA (wxLogDebug (wxT ("graphics_tile_update")));
  UpdateScreen(tile, x0, y0, wxTileX, wxTileY);
  thePanel->MyRefresh();
}

bx_svga_tileinfo_t *bx_wx_gui_c::graphics_tile_info(bx_svga_tileinfo_t *info)
{
  info->bpp = 24;
  info->pitch = wxScreenX * 3;
  info->red_shift = 8;
  info->green_shift = 16;
  info->blue_shift = 24;
  info->red_mask = 0x0000ff;
  info->green_mask = 0x00ff00;
  info->blue_mask = 0xff0000;
  info->is_indexed = 0;
#ifdef BX_LITTLE_ENDIAN
  info->is_little_endian = 1;
#else
  info->is_little_endian = 0;
#endif

  return info;
}

Bit8u *bx_wx_gui_c::graphics_tile_get(unsigned x0, unsigned y0,
                            unsigned *w, unsigned *h)
{
  if (x0+wxTileX > (unsigned)wxScreenX) {
    *w = wxScreenX - x0;
  }
  else {
    *w = wxTileX;
  }

  if (y0+wxTileY > (unsigned)wxScreenY) {
    *h = wxScreenY - y0;
  }
  else {
    *h = wxTileY;
  }

  return (Bit8u *)wxScreen + y0 * wxScreenX * 3 + x0 * 3;
}

void bx_wx_gui_c::graphics_tile_update_in_place(unsigned x0, unsigned y0,
                                        unsigned w, unsigned h)
{
  thePanel->MyRefresh();
}

// ::DIMENSION_UPDATE()
//
// Called from the simulator when the VGA mode changes it's X,Y dimensions.
// Resize the window to this size, but you need to add on
// the height of the headerbar to the Y value.
//
// x:       new VGA x size
// y:       new VGA y size
// fheight: new VGA character height in text mode
// fwidth : new VGA character width in text mode
// bpp : bits per pixel in graphics mode

void bx_wx_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth, unsigned bpp)
{
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::dimension_update trying to get lock. wxScreen=%p", wxScreen)));
  wxScreen_lock.Enter ();
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::dimension_update got lock. wxScreen=%p", wxScreen)));
  BX_INFO (("dimension update x=%d y=%d fontheight=%d fontwidth=%d bpp=%d", x, y, fheight, fwidth, bpp));
  if ((bpp == 8) || (bpp == 15) || (bpp == 16) || (bpp == 24) || (bpp == 32)) {
    if (bpp == 32) BX_INFO(("wxWidgets ignores bit 24..31 in 32bpp mode"));
    disp_bpp = guest_bpp = bpp;
  } else {
    BX_PANIC(("%d bpp graphics mode not supported", bpp));
  }
  guest_textmode = (fheight > 0);
  guest_xres = x;
  guest_yres = y;
  if (guest_textmode) {
    wxFontX = fwidth;
    wxFontY = fheight;
    text_cols = x / wxFontX;
    text_rows = y / wxFontY;
  }
  if (((int)x > wx_maxres.GetWidth()) || ((int)y > wx_maxres.GetHeight())) {
    BX_PANIC(("dimension_update(): resolution of out of display bounds"));
    return;
  }
  wxScreenX = x;
  wxScreenY = y;
  wxScreen = (char *)realloc(wxScreen, wxScreenX * wxScreenY * 3);
  wxASSERT (wxScreen != NULL);
  wxScreen_lock.Leave ();
  IFDBG_VGA(wxLogDebug (wxT ("MyPanel::dimension_update gave up lock. wxScreen=%p", wxScreen)));
  // Note: give up wxScreen_lock before calling SetClientSize.  I did
  // this because I was sometimes seeing thread deadlock in win32, apparantly
  // related to wxStreen_lock.  The wxWidgets GUI thread was sitting in OnPaint
  // trying to get the wxScreen_lock, and the simulation thread was stuck in some
  // native win32 function called by SetClientSize (below).  As with many
  // thread problems, it happened sporadically so it's hard to prove that this
  // really fixed it. -bbd

  // this method is called from the simulation thread, so we must get the GUI
  // thread mutex first to be safe.
  wxMutexGuiEnter();
  theFrame->SetClientSize(wxScreenX, wxScreenY);
  theFrame->Layout();
  wxMutexGuiLeave();
  thePanel->MyRefresh();
  wxScreenCheckSize = 1;
}


// ::CREATE_BITMAP()
//
// Create a monochrome bitmap of size 'xdim' by 'ydim', which will
// be drawn in the headerbar.  Return an integer ID to the bitmap,
// with which the bitmap can be referenced later.
//
// bmap: packed 8 pixels-per-byte bitmap.  The pixel order is:
//       bit0 is the left most pixel, bit7 is the right most pixel.
// xdim: x dimension of bitmap
// ydim: y dimension of bitmap

unsigned bx_wx_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  UNUSED(bmap);
  UNUSED(xdim);
  UNUSED(ydim);
  return(0);
}


// ::HEADERBAR_BITMAP()
//
// Called to install a bitmap in the bochs headerbar (toolbar).
//
// bmap_id: will correspond to an ID returned from
//     ::create_bitmap().  'alignment' is either BX_GRAVITY_LEFT
//     or BX_GRAVITY_RIGHT, meaning install the bitmap in the next
//     available leftmost or rightmost space.
// alignment: is either BX_GRAVITY_LEFT or BX_GRAVITY_RIGHT,
//     meaning install the bitmap in the next
//     available leftmost or rightmost space.
// f: a 'C' function pointer to callback when the mouse is clicked in
//     the boundaries of this bitmap.

unsigned bx_wx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  UNUSED(bmap_id);
  UNUSED(alignment);
  UNUSED(f);
  return(0);
}

// ::SHOW_HEADERBAR()
//
// Show (redraw) the current headerbar, which is composed of
// currently installed bitmaps.

void bx_wx_gui_c::show_headerbar(void)
{
}

// ::REPLACE_BITMAP()
//
// Replace the bitmap installed in the headerbar ID slot 'hbar_id',
// with the one specified by 'bmap_id'.  'bmap_id' will have
// been generated by ::create_bitmap().  The old and new bitmap
// must be of the same size.  This allows the bitmap the user
// sees to change, when some action occurs.  For example when
// the user presses on the floppy icon, it then displays
// the ejected status.
//
// hbar_id: headerbar slot ID
// bmap_id: bitmap ID

  void
bx_wx_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  UNUSED(hbar_id);
  UNUSED(bmap_id);
}


// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.

void bx_wx_gui_c::exit(void)
{
  clear_screen();
  if (mouse_captured) {
    thePanel->ToggleMouse(false);
    mouse_captured = 0;
  }
#if BX_DEBUGGER && BX_DEBUGGER_GUI
  wxMutexGuiEnter();
  close_debug_dialog();
  wxMutexGuiLeave();
#endif
}

void bx_wx_gui_c::mouse_enabled_changed_specific(bx_bool val)
{
  mouse_captured = val;
}

int bx_wx_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  int ret = 0;
  wxMutexGuiEnter();
  if (wxTheClipboard->Open()) {
    if (wxTheClipboard->IsSupported(wxDF_TEXT)) {
      wxTextDataObject data;
      wxTheClipboard->GetData(data);
      wxString str = data.GetText();
      int len = str.Len();
      Bit8u *buf = new Bit8u[len];
      memcpy(buf, str.mb_str(wxConvUTF8), len);
      *bytes = buf;
      *nbytes = len;
      ret = 1;
      // buf will be freed in bx_keyb_c::paste_bytes or
      // bx_keyb_c::service_paste_buf, using delete [].
    } else {
      BX_ERROR (("paste: could not open wxWidgets clipboard"));
    }
    wxTheClipboard->Close();
  }
  wxMutexGuiLeave();
  return ret;
}

int bx_wx_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  wxMutexGuiEnter();
  int ret = 0;
  if (wxTheClipboard->Open()) {
    wxString string(text_snapshot, wxConvUTF8, len);
    wxTheClipboard->SetData(new wxTextDataObject (string));
    wxTheClipboard->Close();
    ret = 1;
  }
  wxMutexGuiLeave();
  return ret;
}

void bx_wx_gui_c::get_capabilities(Bit16u *xres, Bit16u *yres, Bit16u *bpp)
{
  wxDisplay display;

  wx_maxres = display.GetGeometry();
  *xres = wx_maxres.GetWidth();
  *yres = wx_maxres.GetHeight();
  *bpp = 32;
}

void bx_wx_gui_c::set_mouse_mode_absxy(bx_bool mode)
{
  wxMouseModeAbsXY = mode;
}

#if BX_SHOW_IPS
void bx_wx_gui_c::show_ips(Bit32u ips_count)
{
  char ips_text[40];

  if (!wx_hide_ips) {
    ips_count /= 1000;
    sprintf(ips_text, "IPS: %u.%3.3uM", ips_count / 1000, ips_count % 1000);
    theFrame->SetStatusText(wxString(ips_text, wxConvUTF8), 0);
  }
}
#endif

#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXGTK__)
/* we can use the X keysyms for GTK too */
#include <X11/Xlib.h>
#include <X11/keysym.h>
/* convertStringToGDKKey is a keymap callback
 * used when reading the keymap file.
 * It converts a Symblic String to a GUI Constant
 *
 * It returns a Bit32u constant or BX_KEYMAP_UNKNOWN if it fails
 */
static Bit32u convertStringToGDKKey (const char *string)
{
    if (strncmp ("XK_", string, 3) != 0)
      return BX_KEYMAP_UNKNOWN;
    KeySym keysym=XStringToKeysym(string+3);

    // failure, return unknown
    if(keysym==NoSymbol) return BX_KEYMAP_UNKNOWN;

    return((Bit32u)keysym);
}
#endif

#endif /* if BX_WITH_WX */
