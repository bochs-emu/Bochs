/////////////////////////////////////////////////////////////////
// $Id: wx.cc,v 1.38 2002-10-05 17:42:51 bdenney Exp $
/////////////////////////////////////////////////////////////////
//
// wxWindows VGA display for Bochs.  wx.cc implements a custom
// wxPanel called a MyPanel, which has methods to display
// text and VGA graphics on the panel.  Normally, a MyPanel
// is instantiated within a MyFrame created by wxmain.cc, but
// this is not a requirement.
//
// The separation between wxmain.cc and wx.cc is as follows:
// - wxmain.cc implements a Bochs configuration interface (CI),
//   which is the wxWindows equivalent of control.cc.  wxmain creates
//   a frame with several menus and a toolbar, and allows the user to
//   choose the machine configuration and start the simulation.  Note
//   that wxmain.cc does NOT include bochs.h.  All interactions
//   between the CI and the simulator are through the siminterface
//   object.
// - wx.cc implements a VGA display screen using wxWindows.  It is 
//   is the wxWindows equivalent of x.cc, win32.cc, macos.cc, etc.
//   wx.cc includes bochs.h and has access to all Bochs devices.
//   The VGA panel accepts only paint, key, and mouse events.  As it
//   receives events, it builds BxEvents and places them into a 
//   thread-safe BxEvent queue.  The simulation thread periodically
//   processes events from the BxEvent queue (bx_gui_c::handle_events)
//   and notifies the appropriate emulated I/O device.
//
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// includes
//////////////////////////////////////////////////////////////

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

#include "bochs.h"
#include "gui/icon_bochs.h"
#include "osdep.h"
#include "font/vga.bitmap.h"

#include "wxdialog.h"
// shared elements between wxmain.cc and this file
#include "wxmain.h"

//////////////////////////////////////////////////////////////
// constants
//////////////////////////////////////////////////////////////
#define LOG_THIS bx_gui.

//////////////////////////////////////////////////////////////
// data for wx gui
//////////////////////////////////////////////////////////////
static char *wxScreen = NULL;
wxCriticalSection wxScreen_lock;
static long wxScreenX = 0;
static long wxScreenY = 0;
static long wxTileX = 0;
static long wxTileY = 0;
static unsigned long wxCursorX = 0;
static unsigned long wxCursorY = 0;
static unsigned long wxFontY = 0;
struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} wxBochsPalette[256];
wxCriticalSection event_thread_lock;
BxEvent event_queue[MAX_EVENTS];
unsigned long num_events = 0;


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
  wxLogDebug ("MyPanel constructor"); 
  refreshTimer.SetOwner (this);
  refreshTimer.Start (100);
  needRefresh = true;
  const char bits[1] = { 0 };
  blankCursor = new wxCursor (bits, 1, 1, -1, -1, bits);
  thePanel = this;
}

MyPanel::~MyPanel ()
{
  delete blankCursor;
  thePanel = NULL;
}

void MyPanel::OnTimer(wxCommandEvent& WXUNUSED(event))
{
  IFDBG_VGA(wxLogDebug ("timer"));
  if (needRefresh) {
    IFDBG_VGA(wxLogDebug ("calling refresh"));
    Refresh (FALSE);
  }
}

void MyPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);
  IFDBG_VGA (wxLogDebug ("OnPaint"));
  //PrepareDC(dc);

  IFDBG_VGA(wxLogDebug ("MyPanel::OnPaint trying to get lock. wxScreen=%p", wxScreen));
  wxCriticalSectionLocker lock(wxScreen_lock);
  IFDBG_VGA(wxLogDebug ("MyPanel::OnPaint got lock. wxScreen=%p", wxScreen));
  if(wxScreen != NULL) {
    wxPoint pt = GetClientAreaOrigin();
    wxImage screenImage(wxScreenX, wxScreenY, (unsigned char *)wxScreen, TRUE);
    IFDBG_VGA(wxLogDebug ("drawBitmap"));
    dc.DrawBitmap(screenImage.ConvertToBitmap(), pt.x, pt.y, FALSE);
  }
  needRefresh = false;
}

void MyPanel::ToggleMouse ()
{
  bx_param_bool_c *enable = SIM->get_param_bool (BXP_MOUSE_ENABLED);
  bool en = ! enable->get ();
  enable->set (en);
  IFDBG_MOUSE (wxLogDebug ("now mouse is %sabled", en ? "en" : "dis"));
  if (en) {
    mouseSavedX = wxScreenX / 2;
    mouseSavedY = wxScreenY / 2;
    WarpPointer (mouseSavedX, mouseSavedY);
    SetCursor (*blankCursor);
  } else {
    SetCursor (wxNullCursor);
  }
}

void MyPanel::OnMouse(wxMouseEvent& event)
{
  long x,y;
  event.GetPosition (&x, &y);
  IFDBG_MOUSE (
    if (event.IsButton ()) {
      wxLogDebug ("mouse button event at %d,%d", x, y);
    } else if (event.Entering ()) {
      wxLogDebug ("mouse entering at %d,%d", x, y);
    } else if (event.Leaving ()) {
      wxLogDebug ("mouse leaving at %d,%d", x, y);
    } else if (event.Moving() || event.Dragging ()) {
      wxLogDebug ("mouse moved to %d,%d", x, y);
    } else {
      wxLogDebug ("other mouse event at %d,%d", x, y);
    }
  )

  if (event.MiddleDown ()) {
    ToggleMouse ();
    return;
  }

  if (!SIM->get_param_bool(BXP_MOUSE_ENABLED)->get ()) 
    return;  // mouse disabled, ignore the event

  // process buttons and motion together
  Bit32u buttons;
  buttons  = event.LeftIsDown ()  ? 1 : 0;
  buttons |= event.RightIsDown () ? 2 : 0;
  if (x==mouseSavedX && y==mouseSavedY && !event.IsButton ()) {
    // nothing happened.  This could have been generated by the WarpPointer.
    return;
  } else {
    if(num_events < MAX_EVENTS) {
      wxCriticalSectionLocker lock(event_thread_lock);
      Bit16s dx = x - mouseSavedX;
      Bit16s dy = y - mouseSavedY;
      IFDBG_MOUSE (wxLogDebug ("mouse moved by delta %d,%d", dx, dy));
      event_queue[num_events].type = BX_ASYNC_EVT_MOUSE;
      event_queue[num_events].u.mouse.dx = dx;
      event_queue[num_events].u.mouse.dy = -dy;
      event_queue[num_events].u.mouse.buttons = buttons;
      num_events++;
      mouseSavedX = x;
      mouseSavedY = y;
    } else {
      wxLogDebug ("mouse event skipped because event queue full");
    }
  }

  mouseSavedX = wxScreenX / 2;
  mouseSavedY = wxScreenY / 2;
  WarpPointer (mouseSavedX, mouseSavedY);
  // The WarpPointer moves the pointer back to the middle of the
  // screen.  This WILL produce another mouse motion event, which needs
  // to be ignored.  It will be ignored because the new motion event
  // will move the cursor to (mouseSavedX, mouseSavedY).
}

void 
MyPanel::MyRefresh ()
{
  IFDBG_VGA (wxLogDebug ("set needRefresh=true"));
  needRefresh = true;
}

void MyPanel::OnKeyDown(wxKeyEvent& event)
{
  if(event.GetKeyCode() == WXK_F12) {
    ToggleMouse ();
    return;
  }
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
static char 
wxAsciiKey[0x5f] = {
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

#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXMSW__)
// get windows specific definitions. At present the only thing needed
// is the definition of HIWORD.

// windows.h included by bochs.h, so nothing extra is required here.
#endif

// MS Windows specific key mapping, which uses wxKeyEvent::m_rawCode & 2.
Boolean
MyPanel::fillBxKeyEvent_MSW (wxKeyEvent& wxev, BxKeyEvent& bxev, Boolean release)
{
#if defined(wxHAS_RAW_KEY_CODES) && defined(__WXMSW__)
  IFDBG_KEY(wxLogDebug ("fillBxKeyEvent_MSW. key code %d, raw codes %d %d", wxev.m_keyCode, wxev.m_rawCode, wxev.m_rawFlags));
  // this code was grabbed from gui/win32.cpp
  Bit32u lParam = wxev.m_rawFlags;
  Bit32u key = HIWORD (lParam) & 0x01FF;
  bxev.bx_key = 0x0000;
  // Swap the scancodes of "numlock" and "pause"
  if ((key & 0xff)==0x45) key ^= 0x100;
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
Boolean
MyPanel::fillBxKeyEvent_GTK (wxKeyEvent& wxev, BxKeyEvent& bxev, Boolean release)
{
#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXGTK__)
  IFDBG_KEY(wxLogDebug ("fillBxKeyEvent_GTK. key code %ld, raw codes %d %d", wxev.m_keyCode, wxev.m_rawCode, wxev.m_rawFlags));
  // GTK has only 16bit key codes
  Bit16u keysym = (Bit32u) wxev.m_rawCode;
  Bit32u key_event = 0;
  // since the GDK_* symbols are very much like the X11 symbols (possibly
  // identical), I'm using code that is copied from gui/x.cc.
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

      default:
        wxLogError( "fillBxKeyEvent_GTK(): keysym %x unhandled!", (unsigned) keysym );
        return BX_KEY_UNHANDLED;
  }
  bxev.bx_key = key_event | (release? BX_KEY_RELEASED : BX_KEY_PRESSED);
  bxev.raw_scancode = false;
  return true;
#else   // if GTK toolkit
  return false;
#endif
}

Boolean
MyPanel::fillBxKeyEvent (wxKeyEvent& wxev, BxKeyEvent& bxev, Boolean release)
{
  // Use raw codes if they are available.  Raw codes are a nonstandard addition
  // to the wxWindows library.  At present, the only way to use the "RAW_CODES"
  // mode is to apply Bryce's "patch.wx-raw-keycodes" patch to the wxWindows
  // sources and recompile.  This patch, or something like it, should appear in
  // future wxWindows versions.

#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXMSW__)
  return fillBxKeyEvent_MSW (wxev, bxev, release);
#endif

#if defined (wxHAS_RAW_KEY_CODES) && defined(__WXGTK__)
  return fillBxKeyEvent_GTK (wxev, bxev, release);
#endif

  // otherwise fall back to using portable WXK_* keycodes.  Not all keys
  // can be mapped correctly using WXK_* codes but it should be usable.
  IFDBG_KEY (wxLogDebug ("fillBxKeyEvent. key code %ld", wxev.m_keyCode));
  Bit32u key = wxev.m_keyCode;
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
    case WXK_NUMPAD_PAGEUP:        bx_key = BX_KEY_KP_PAGE_UP;   break;
    case WXK_NUMPAD_NEXT:          bx_key = BX_KEY_KP_PAGE_DOWN; break;
    case WXK_NUMPAD_PAGEDOWN:      bx_key = BX_KEY_KP_PAGE_DOWN; break;
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
    case 392: bx_key = BX_KEY_KP_ADD;        break; // keypad plus

    default:
      wxLogMessage("Unhandled key event: %i (0x%x)", key, key);
      return 0;
    }
  }
  IFDBG_KEY (wxLogDebug ("fillBxKeyEvent: after remapping, key=%d", bx_key));
  bxev.bx_key = bx_key | (release? BX_KEY_RELEASED : BX_KEY_PRESSED);
  bxev.raw_scancode = false;
  return true;
}


//////////////////////////////////////////////////////////////
// fill in methods of bx_gui
//////////////////////////////////////////////////////////////

  void
bx_gui_c::specific_init(bx_gui_c *th, int argc, char **argv, unsigned tilewidth, unsigned tileheight,
                     unsigned headerbar_y)
{
  int b,i,j;
  unsigned char fc, vc;

  th->put("WX  ");
  if (bx_options.Oprivate_colormap->get ()) {
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
      bx_gui.vga_charmap[i*32+j] = fc;
    }
  }

  wxScreenX = 640;
  wxScreenY = 480;
  IFDBG_VGA(wxLogDebug ("MyPanel::specific_init trying to get lock. wxScreen=%p", wxScreen));
  wxCriticalSectionLocker lock(wxScreen_lock);
  IFDBG_VGA(wxLogDebug ("MyPanel::specific_init got lock. wxScreen=%p", wxScreen));
  wxScreen = (char *)malloc(wxScreenX * wxScreenY * 3);
  memset(wxScreen, 0, wxScreenX * wxScreenY * 3);

  wxTileX = tilewidth;
  wxTileY = tileheight;

  // load keymap tables
  if(bx_options.keyboard.OuseMapping->get())
    bx_keymap.loadKeymap(NULL);
}

// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

void bx_gui_c::handle_events(void)
{
  wxCriticalSectionLocker lock(event_thread_lock);
  Bit32u bx_key = 0;
  for(unsigned int i = 0; i < num_events; i++) {
    switch(event_queue[i].type) {
      case BX_ASYNC_EVT_TOOLBAR:
        switch (event_queue[i].u.toolbar.button) {
          case BX_TOOLBAR_FLOPPYA: floppyA_handler (); break;
          case BX_TOOLBAR_FLOPPYB: floppyB_handler (); break;
          case BX_TOOLBAR_CDROMD: cdromD_handler (); break;
          case BX_TOOLBAR_RESET: reset_handler (); break;
          case BX_TOOLBAR_POWER: power_handler (); break;
          case BX_TOOLBAR_COPY: copy_handler (); break;
          case BX_TOOLBAR_PASTE: paste_handler (); break;
          case BX_TOOLBAR_SNAPSHOT: snapshot_handler (); break;
          case BX_TOOLBAR_CONFIG: config_handler (); break;
          case BX_TOOLBAR_MOUSE_EN: toggle_mouse_enable (); break;
          case BX_TOOLBAR_USER: userbutton_handler (); break;
          default:
            wxLogDebug ("unknown toolbar id %d", event_queue[i].u.toolbar.button);
        }
        break;
      case BX_ASYNC_EVT_KEY:
        bx_key = event_queue[i].u.key.bx_key;
    if (event_queue[i].u.key.raw_scancode) {
      // event contains raw scancodes: use put_scancode
      Bit8u scancode;
      if (bx_key & 0xFF00) { // for extended keys
        scancode = 0xFF & (bx_key>>8);
      IFDBG_KEY (wxLogDebug ("sending raw scancode 0x%02x (extended key)", (int)scancode));
        bx_devices.keyboard->put_scancode (&scancode, 1);
      }
      scancode = 0xFF & bx_key;
      IFDBG_KEY (wxLogDebug ("sending raw scancode 0x%02x", (int)scancode));
      bx_devices.keyboard->put_scancode (&scancode, 1);
    } else {
      // event contains BX_KEY_* codes: use gen_scancode
      IFDBG_KEY (wxLogDebug ("sending key event 0x%02x", bx_key));
      bx_devices.keyboard->gen_scancode(bx_key);
    }
        break;
      case BX_ASYNC_EVT_MOUSE:
  bx_devices.keyboard->mouse_motion (
      event_queue[i].u.mouse.dx,
      event_queue[i].u.mouse.dy,
      event_queue[i].u.mouse.buttons);
  break;
      default:
        wxLogError ("handle_events received unhandled event type %d in queue", (int)event_queue[i].type);
    }
  }
  num_events = 0;
}

// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

  void
bx_gui_c::flush(void)
{
}


// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.  Don't
// clear the area that defines the headerbar.

  void
bx_gui_c::clear_screen(void)
{
  IFDBG_VGA(wxLogDebug ("MyPanel::clear_screen trying to get lock. wxScreen=%p", wxScreen));
  wxCriticalSectionLocker lock(wxScreen_lock);
  IFDBG_VGA(wxLogDebug ("MyPanel::clear_screen got lock. wxScreen=%p", wxScreen));
  memset(wxScreen, 0, wxScreenX * wxScreenY * 3);
  thePanel->MyRefresh ();
}

static void 
UpdateScreen(char *newBits, int x, int y, int width, int height) 
{
  IFDBG_VGA(wxLogDebug ("MyPanel::UpdateScreen trying to get lock. wxScreen=%p", wxScreen));
  wxCriticalSectionLocker lock(wxScreen_lock);
  IFDBG_VGA(wxLogDebug ("MyPanel::UpdateScreen got lock. wxScreen=%p", wxScreen));
  if(wxScreen != NULL) {
    for(int i = 0; i < height; i++) {
      for(int c = 0; c < width; c++) {
        wxScreen[(y * wxScreenX * 3) + ((x+c) * 3)] = wxBochsPalette[newBits[(i * width) + c]].red;
        wxScreen[(y * wxScreenX * 3) + ((x+c) * 3) + 1] = wxBochsPalette[newBits[(i * width) + c]].green;
        wxScreen[(y * wxScreenX * 3) + ((x+c) * 3) + 2] = wxBochsPalette[newBits[(i * width) + c]].blue;
      }
      y++;
      if(y >= wxScreenY) break;
    }
  } else {
    IFDBG_VGA (wxLogDebug ("UpdateScreen with null wxScreen"));
  }
}

static void 
DrawBochsBitmap(int x, int y, int width, int height, char *bmap, char color, int cs_start, int cs_end)
{
  int j = 0;
  char bgcolor = (color >> 4) & 0xF;
  char fgcolor = color & 0xF;

  if (cs_start <= cs_end) {
    height = cs_end - cs_start + 1;
    y += cs_start;
    j = cs_start * ((width - 1) / 8 + 1);
  }
  char *newBits = (char *)malloc(width * height);
  memset(newBits, 0, (width * height));
  for(int i = 0; i < (width * height) / 8; i++) {
    newBits[i * 8 + 0] = (bmap[j] & 0x80) ? fgcolor : bgcolor;
    newBits[i * 8 + 1] = (bmap[j] & 0x40) ? fgcolor : bgcolor;
    newBits[i * 8 + 2] = (bmap[j] & 0x20) ? fgcolor : bgcolor;
    newBits[i * 8 + 3] = (bmap[j] & 0x10) ? fgcolor : bgcolor;
    newBits[i * 8 + 4] = (bmap[j] & 0x08) ? fgcolor : bgcolor;
    newBits[i * 8 + 5] = (bmap[j] & 0x04) ? fgcolor : bgcolor;
    newBits[i * 8 + 6] = (bmap[j] & 0x02) ? fgcolor : bgcolor;
    newBits[i * 8 + 7] = (bmap[j] & 0x01) ? fgcolor : bgcolor;
    j++;
  }
  UpdateScreen(newBits, x, y, width, height);
  free(newBits);
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
// format of old_text & new_text: each is 4000 bytes long.
//     This represents 80 characters wide by 25 high, with
//     each character being 2 bytes.  The first by is the
//     character value, the second is the attribute byte.
//     I currently don't handle the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor

void bx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                      unsigned long cursor_x, unsigned long cursor_y,
          Bit16u cursor_state, unsigned nrows)
{
  IFDBG_VGA(wxLogDebug ("text_update"));
  //static Bit32u counter = 0;
  //BX_INFO (("text_update executed %d times", ++counter));

  Bit8u cs_start = (cursor_state >> 8) & 0x3f;
  Bit8u cs_end = cursor_state & 0x1f;
  unsigned char cChar;
  unsigned int ncols = wxScreenX / 8;
  unsigned int nchars = ncols * nrows;
  Boolean forceUpdate = 0;
  if(bx_gui.charmap_updated) {
    forceUpdate = 1;
    bx_gui.charmap_updated = 0;
  }
  if((wxCursorY * ncols + wxCursorX) < nchars) {
    cChar = new_text[(wxCursorY * ncols + wxCursorX) * 2];
    DrawBochsBitmap(wxCursorX * 8, wxCursorY * wxFontY, 8, wxFontY, (char *)&bx_gui.vga_charmap[cChar<<5], new_text[((wxCursorY * ncols + wxCursorX) * 2) + 1], 1, 0);
  }
  
  for(unsigned int i = 0; i < nchars * 2; i += 2) {
    if(forceUpdate || (old_text[i] != new_text[i])
       || (old_text[i+1] != new_text[i+1])) {
      cChar = new_text[i];
      int x = (i / 2) % ncols;
      int y = (i / 2) / ncols;
      DrawBochsBitmap(x * 8, y * wxFontY, 8, wxFontY, (char *)&bx_gui.vga_charmap[cChar<<5], new_text[i+1], 1, 0);
    }
  }
  wxCursorX = cursor_x;
  wxCursorY = cursor_y;

  if(((cursor_y * ncols + cursor_x) < nchars) && (cs_start <= cs_end)) {
    cChar = new_text[(cursor_y * ncols + cursor_x) * 2];
    char cAttr = new_text[((cursor_y * ncols + cursor_x) * 2) + 1];
    cAttr = ((cAttr >> 4) & 0xF) + ((cAttr & 0xF) << 4);
    DrawBochsBitmap(wxCursorX * 8, wxCursorY * wxFontY, 8, wxFontY, (char *)&bx_gui.vga_charmap[cChar<<5], cAttr, cs_start, cs_end);
  }

  thePanel->MyRefresh ();
}


// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

  Boolean
bx_gui_c::palette_change(unsigned index, unsigned red, unsigned green, unsigned blue)
{
  IFDBG_VGA(wxLogDebug ("palette_change"));
  wxBochsPalette[index].red = red;
  wxBochsPalette[index].green = green;
  wxBochsPalette[index].blue = blue;
  thePanel->MyRefresh ();
  return(0);
}


// ::GRAPHICS_TILE_UPDATE()
//
// Called to request that a tile of graphics be drawn to the
// screen, since info in this region has changed.
//
// tile: array of 8bit values representing a block of pixels with
//       dimension equal to the 'tilewidth' & 'tileheight' parameters to
//       ::specific_init().  Each value specifies an index into the
//       array of colors you allocated for ::palette_change()
// x0: x origin of tile
// y0: y origin of tile
//
// note: origin of tile and of window based on (0,0) being in the upper
//       left of the window.

void bx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  IFDBG_VGA (wxLogDebug ("graphics_tile_update"));
  //static Bit32u counter = 0;
  //BX_INFO (("graphics_tile_update executed %d times", ++counter));
  UpdateScreen((char *)tile, x0, y0, wxTileX, wxTileY);
  thePanel->MyRefresh ();
}

// ::DIMENSION_UPDATE()
//
// Called from the simulator when the VGA mode changes it's X,Y dimensions.
// Resize the window to this size, but you need to add on
// the height of the headerbar to the Y value.
//
// x: new VGA x size
// y: new VGA y size (add headerbar_y parameter from ::specific_init().

void bx_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight)
{
  IFDBG_VGA(wxLogDebug ("MyPanel::dimension_update trying to get lock. wxScreen=%p", wxScreen));
  wxScreen_lock.Enter ();
  IFDBG_VGA(wxLogDebug ("MyPanel::dimension_update got lock. wxScreen=%p", wxScreen));
  BX_INFO (("dimension update"));
  if (fheight > 0) {
  wxFontY = fheight;
  }
  wxScreenX = x;
  wxScreenY = y;
  wxScreen = (char *)realloc(wxScreen, wxScreenX * wxScreenY * 3);
  wxASSERT (wxScreen != NULL);
  wxScreen_lock.Leave ();
  IFDBG_VGA(wxLogDebug ("MyPanel::dimension_update gave up lock. wxScreen=%p", wxScreen));
  // Note: give up wxScreen_lock before calling SetClientSize.  I did
  // this because I was sometimes seeing thread deadlock in win32, apparantly 
  // related to wxStreen_lock.  The wxWindows GUI thread was sitting in OnPaint
  // trying to get the wxScreen_lock, and the simulation thread was stuck in some
  // native win32 function called by SetClientSize (below).  As with many
  // thread problems, it happened sporadically so it's hard to prove that this
  // really fixed it. -bbd
  
  // this method is called from the simulation thread, so we must get the GUI
  // thread mutex first to be safe.
  wxMutexGuiEnter ();
  //theFrame->SetSize(-1, -1, wxScreenX + 6, wxScreenY + 100, 0);
  //wxSize size = theFrame->GetToolBar()->GetToolSize();
  theFrame->SetClientSize(wxScreenX, wxScreenY); // + size.GetHeight());
  theFrame->Layout ();
  wxMutexGuiLeave ();
  thePanel->MyRefresh ();
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

  unsigned
bx_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
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

  unsigned
bx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
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

  void
bx_gui_c::show_headerbar(void)
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
bx_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  UNUSED(hbar_id);
  UNUSED(bmap_id);
}


// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.

  void
bx_gui_c::exit(void)
{
  BX_INFO(("bx_gui_c::exit() not implemented yet."));
}

  void
bx_gui_c::mouse_enabled_changed_specific (Boolean val)
{
}


  int
bx_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  int ret = 0;
  wxMutexGuiEnter ();
  if (wxTheClipboard->Open ()) {
    if (wxTheClipboard->IsSupported (wxDF_TEXT)) {
      wxTextDataObject data;
      wxTheClipboard->GetData (data);
      wxString str = data.GetText ();
      int len = str.Len ();
      Bit8u *buf = new Bit8u[len];
      memcpy (buf, str.c_str (), len);
      *bytes = buf;
      *nbytes = len;
      ret = 1;
      // buf will be freed in bx_keyb_c::paste_bytes or 
      // bx_keyb_c::service_paste_buf, using delete [].
    } else {
      BX_ERROR (("paste: could not open wxWindows clipboard"));
    }
    wxTheClipboard->Close ();
  }
  wxMutexGuiLeave ();
  return ret;
}

  int
bx_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  wxMutexGuiEnter ();
  int ret = 0;
  if (wxTheClipboard->Open ()) {
    wxString string (text_snapshot, len);
    wxTheClipboard->SetData (new wxTextDataObject (string));
    wxTheClipboard->Close ();
    ret = 1;
  }
  wxMutexGuiLeave ();
  return ret;
}
