/////////////////////////////////////////////////////////////////
// $Id: wx.cc,v 1.10 2002-08-30 17:39:14 vruppert Exp $
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

#include "bochs.h"
#include "gui/icon_bochs.h"
#include "osdep.h"
#include "font/vga.bitmap.h"

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
static unsigned long wxMouseCaptured = 0;
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
  EVT_PAINT(MyPanel::OnPaint)
END_EVENT_TABLE()

void MyPanel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);
	//PrepareDC(dc);

	wxCriticalSectionLocker lock(wxScreen_lock);
	IFDBG_VGA(wxLogDebug ("MyPanel::OnPaint called with wxScreen = %p", wxScreen));
	if(wxScreen != NULL) {
	  wxPoint pt = GetClientAreaOrigin();
	  wxImage screenImage(wxScreenX, wxScreenY, (unsigned char *)wxScreen, TRUE);
	  dc.DrawBitmap(screenImage.ConvertToBitmap(), pt.x, pt.y, FALSE);
	}
	
}

void 
MyPanel::MyRefresh ()
{
#if defined(__WXMSW__)
	// For some reason, on windows the postevent trick does not
	// cause a paint event.
	Refresh(FALSE);
#else
	wxPaintEvent event;
	wxPostEvent (this, event);
#endif
}

void 
MyPanel::ReadConfiguration ()
{
  char *bochsrc;
  long style = wxOPEN;
  wxFileDialog *fdialog = new wxFileDialog (this, "Read configuration", "", "", "*.*", style);
  if (fdialog->ShowModal() == wxID_OK) {
    bochsrc = (char *)fdialog->GetPath().c_str ();
    bx_read_configuration(bochsrc);
  }
  delete fdialog;
}

void 
MyPanel::SaveConfiguration ()
{
  char *bochsrc;
  long style = wxSAVE | wxOVERWRITE_PROMPT;
  wxFileDialog *fdialog = new wxFileDialog (this, "Save configuration", "", "", "*.*", style);
  if (fdialog->ShowModal() == wxID_OK) {
    bochsrc = (char *)fdialog->GetPath().c_str ();
    bx_write_configuration(bochsrc, 1);
  }
  delete fdialog;
}

void MyPanel::OnKeyDown(wxKeyEvent& event)
{
	if(event.GetKeyCode() == WXK_F12) {
		if(wxMouseCaptured) {
			ReleaseMouse();
			wxMouseCaptured = FALSE;
		} else {
			CaptureMouse();
			wxMouseCaptured = TRUE;
		}
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
  IFDBG_KEY(wxLogDebug ("fillBxKeyEvent_GTK. key code %d, raw codes %d %d", wxev.m_keyCode, wxev.m_rawCode, wxev.m_rawFlags));
  // GTK has only 16bit key codes
  Bit16u keysym = (Bit32u) wxev.m_rawCode;
  Bit32u bx_key = 0;
  // since the GDK_* symbols are very much like the X11 symbols (possibly
  // identical), I'm using code that is copied from gui/x.cc.
  if (keysym >= GDK_space && keysym < GDK_asciitilde) {
    // use nice ASCII conversion table, based on x.cc
    bx_key = wxAsciiKey[keysym - GDK_space];
  } else switch (keysym) {
      case GDK_KP_1:
#ifdef GDK_KP_End
      case GDK_KP_End:
#endif
        bx_key = BX_KEY_KP_END; break;

      case GDK_KP_2:
#ifdef GDK_KP_Down
      case GDK_KP_Down:
#endif
        bx_key = BX_KEY_KP_DOWN; break;

      case GDK_KP_3:
#ifdef GDK_KP_Page_Down
      case GDK_KP_Page_Down:
#endif
        bx_key = BX_KEY_KP_PAGE_DOWN; break;

      case GDK_KP_4:
#ifdef GDK_KP_Left
      case GDK_KP_Left:
#endif
        bx_key = BX_KEY_KP_LEFT; break;

      case GDK_KP_5:
        bx_key = BX_KEY_KP_5; break;

      case GDK_KP_6:
#ifdef GDK_KP_Right
      case GDK_KP_Right:
#endif
        bx_key = BX_KEY_KP_RIGHT; break;

      case GDK_KP_7:
#ifdef GDK_KP_Home
      case GDK_KP_Home:
#endif
        bx_key = BX_KEY_KP_HOME; break;

      case GDK_KP_8:
#ifdef GDK_KP_Up
      case GDK_KP_Up:
#endif
        bx_key = BX_KEY_KP_UP; break;

      case GDK_KP_9:
#ifdef GDK_KP_Page_Up
      case GDK_KP_Page_Up:
#endif
        bx_key = BX_KEY_KP_PAGE_UP; break;

      case GDK_KP_0:
#ifdef GDK_KP_Insert
      case GDK_KP_Insert:
#endif
        bx_key = BX_KEY_KP_INSERT; break;

      case GDK_KP_Decimal:
#ifdef GDK_KP_Delete
      case GDK_KP_Delete:
#endif
        bx_key = BX_KEY_KP_DELETE; break;

#ifdef GDK_KP_Enter
      case GDK_KP_Enter:
        bx_key = BX_KEY_KP_ENTER; break;
#endif

      case GDK_KP_Subtract: bx_key = BX_KEY_KP_SUBTRACT; break;
      case GDK_KP_Add:      bx_key = BX_KEY_KP_ADD; break;

      case GDK_KP_Multiply: bx_key = BX_KEY_KP_MULTIPLY; break;
      case GDK_KP_Divide:   bx_key = BX_KEY_KP_DIVIDE; break;


      case GDK_Up:          bx_key = BX_KEY_UP; break;
      case GDK_Down:        bx_key = BX_KEY_DOWN; break;
      case GDK_Left:        bx_key = BX_KEY_LEFT; break;
      case GDK_Right:       bx_key = BX_KEY_RIGHT; break;


      case GDK_Delete:      bx_key = BX_KEY_DELETE; break;
      case GDK_BackSpace:   bx_key = BX_KEY_BACKSPACE; break;
      case GDK_Tab:         bx_key = BX_KEY_TAB; break;
      case GDK_Return:      bx_key = BX_KEY_ENTER; break;
      case GDK_Escape:      bx_key = BX_KEY_ESC; break;
      case GDK_F1:          bx_key = BX_KEY_F1; break;
      case GDK_F2:          bx_key = BX_KEY_F2; break;
      case GDK_F3:          bx_key = BX_KEY_F3; break;
      case GDK_F4:          bx_key = BX_KEY_F4; break;
      case GDK_F5:          bx_key = BX_KEY_F5; break;
      case GDK_F6:          bx_key = BX_KEY_F6; break;
      case GDK_F7:          bx_key = BX_KEY_F7; break;
      case GDK_F8:          bx_key = BX_KEY_F8; break;
      case GDK_F9:          bx_key = BX_KEY_F9; break;
      case GDK_F10:         bx_key = BX_KEY_F10; break;
      case GDK_F11:         bx_key = BX_KEY_F11; break;
      case GDK_F12:         bx_key = BX_KEY_F12; break;
      case GDK_Control_L:   bx_key = BX_KEY_CTRL_L; break;
      case GDK_Shift_L:     bx_key = BX_KEY_SHIFT_L; break;
      case GDK_Shift_R:     bx_key = BX_KEY_SHIFT_R; break;
      case GDK_Caps_Lock:   bx_key = BX_KEY_CAPS_LOCK; break;
      case GDK_Num_Lock:    bx_key = BX_KEY_NUM_LOCK; break;
      case GDK_Alt_L:       bx_key = BX_KEY_ALT_L; break;

      case GDK_Insert:      bx_key = BX_KEY_INSERT; break;
      case GDK_Home:        bx_key = BX_KEY_HOME; break;
      case GDK_End:         bx_key = BX_KEY_END; break;
      case GDK_Page_Up:     bx_key = BX_KEY_PAGE_UP; break;
      case GDK_Page_Down:   bx_key = BX_KEY_PAGE_DOWN; break;

      default:
        wxLogError( "fillBxKeyEvent_GTK(): keysym %x unhandled!", (unsigned) keysym );
        return BX_KEY_UNHANDLED;
  }
  bxev.bx_key = bx_key | (release? BX_KEY_RELEASED : BX_KEY_PRESSED);
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
  IFDBG_KEY (wxLogDebug ("fillBxKeyEvent. key code %d", wxev.m_keyCode));
  Bit32u key = wxev.m_keyCode;
  Bit32u bx_key;
  
  if(key >= WXK_SPACE && key < WXK_DELETE) {
	  bx_key = wxAsciiKey[key - WXK_SPACE];
  } else {
	  // handle extended keys here
	  switch(key) {
	  case WXK_BACK:		bx_key = BX_KEY_BACKSPACE;    break;
	  case WXK_TAB:		bx_key = BX_KEY_TAB;          break;
	  case WXK_RETURN:	bx_key = BX_KEY_ENTER;        break;
	  case WXK_ESCAPE:	bx_key = BX_KEY_ESC;          break;
	  case WXK_DELETE:	bx_key = BX_KEY_DELETE;       break;
	  case WXK_SHIFT:		bx_key = BX_KEY_SHIFT_L;      break;
	  case WXK_CONTROL:	bx_key = BX_KEY_CTRL_L;       break;
	  case WXK_ALT:		bx_key = BX_KEY_ALT_L;        break;
	  case WXK_MENU:		bx_key = BX_KEY_MENU;         break;
	  case WXK_PAUSE:		bx_key = BX_KEY_PAUSE;        break;
	  case WXK_PRIOR:		bx_key = BX_KEY_PAGE_UP;      break;
	  case WXK_NEXT:		bx_key = BX_KEY_PAGE_DOWN;    break;
	  case WXK_END:		bx_key = BX_KEY_END;          break;
	  case WXK_HOME:		bx_key = BX_KEY_HOME;         break;
	  case WXK_LEFT:		bx_key = BX_KEY_LEFT;         break;
	  case WXK_UP:		bx_key = BX_KEY_UP;           break;
	  case WXK_RIGHT:		bx_key = BX_KEY_RIGHT;        break;
	  case WXK_DOWN:		bx_key = BX_KEY_DOWN;         break;
	  case WXK_INSERT:	bx_key = BX_KEY_INSERT;       break;
	  case WXK_NUMPAD0:	bx_key = BX_KEY_KP_INSERT;    break;
	  case WXK_NUMPAD1:	bx_key = BX_KEY_KP_END;       break;
	  case WXK_NUMPAD2:	bx_key = BX_KEY_KP_DOWN;      break;
	  case WXK_NUMPAD3:	bx_key = BX_KEY_KP_PAGE_DOWN; break;
	  case WXK_NUMPAD4:	bx_key = BX_KEY_KP_LEFT;      break;
	  case WXK_NUMPAD5:	bx_key = BX_KEY_KP_5;         break;
	  case WXK_NUMPAD6:	bx_key = BX_KEY_KP_RIGHT;     break;
	  case WXK_NUMPAD7:	bx_key = BX_KEY_KP_HOME;      break;
	  case WXK_NUMPAD8:	bx_key = BX_KEY_KP_UP;        break;
	  case WXK_NUMPAD9:	bx_key = BX_KEY_KP_PAGE_UP;   break;
	  case WXK_F1:		bx_key = BX_KEY_F1;           break;
	  case WXK_F2:		bx_key = BX_KEY_F2;           break;
	  case WXK_F3:		bx_key = BX_KEY_F3;           break;
	  case WXK_F4:		bx_key = BX_KEY_F4;           break;
	  case WXK_F5:		bx_key = BX_KEY_F5;           break;
	  case WXK_F6:		bx_key = BX_KEY_F6;           break;
	  case WXK_F7:		bx_key = BX_KEY_F7;           break;
	  case WXK_F8:		bx_key = BX_KEY_F8;           break;
	  case WXK_F9:		bx_key = BX_KEY_F9;           break;
	  case WXK_F10:		bx_key = BX_KEY_F10;          break;
	  case WXK_F11:		bx_key = BX_KEY_F11;          break;
	  case WXK_F12:		bx_key = BX_KEY_F12;          break;
	  case WXK_NUMLOCK:	bx_key = BX_KEY_NUM_LOCK;     break;
	  case WXK_SCROLL:	bx_key = BX_KEY_SCRL_LOCK;    break;
	  case WXK_DECIMAL:	bx_key = BX_KEY_PERIOD;       break;
	  case WXK_SUBTRACT:	bx_key = BX_KEY_MINUS;        break; 
	  case WXK_ADD:		bx_key = BX_KEY_EQUALS;       break;
	  case WXK_MULTIPLY:	bx_key = BX_KEY_KP_MULTIPLY;  break;
	  case WXK_DIVIDE:	bx_key = BX_KEY_KP_DIVIDE;    break;

	  case WXK_NUMPAD_ENTER:		bx_key = BX_KEY_KP_ENTER;     break;
	  case WXK_NUMPAD_HOME:		bx_key = BX_KEY_KP_HOME;      break;
	  case WXK_NUMPAD_LEFT:		bx_key = BX_KEY_KP_LEFT;      break;
	  case WXK_NUMPAD_UP:			bx_key = BX_KEY_KP_UP;        break;
	  case WXK_NUMPAD_RIGHT:		bx_key = BX_KEY_KP_RIGHT;     break;
	  case WXK_NUMPAD_DOWN:		bx_key = BX_KEY_KP_DOWN;      break;
	  case WXK_NUMPAD_PRIOR:		bx_key = BX_KEY_KP_PAGE_UP;   break;
	  case WXK_NUMPAD_PAGEUP:		bx_key = BX_KEY_KP_PAGE_UP;   break;
	  case WXK_NUMPAD_NEXT:		bx_key = BX_KEY_KP_PAGE_DOWN; break;
	  case WXK_NUMPAD_PAGEDOWN:	bx_key = BX_KEY_KP_PAGE_DOWN; break;
	  case WXK_NUMPAD_END:		bx_key = BX_KEY_KP_END;       break;
	  case WXK_NUMPAD_BEGIN:		bx_key = BX_KEY_KP_HOME;      break;
	  case WXK_NUMPAD_INSERT:		bx_key = BX_KEY_KP_INSERT;    break;
	  case WXK_NUMPAD_DELETE:		bx_key = BX_KEY_KP_DELETE;    break;
	  case WXK_NUMPAD_EQUAL:		bx_key = BX_KEY_KP_ENTER;     break;
	  case WXK_NUMPAD_MULTIPLY:	bx_key = BX_KEY_KP_MULTIPLY;  break;
	  case WXK_NUMPAD_SUBTRACT:	bx_key = BX_KEY_KP_SUBTRACT;  break;
	  case WXK_NUMPAD_DECIMAL:	bx_key = BX_KEY_KP_DELETE;    break;
	  case WXK_NUMPAD_DIVIDE:		bx_key = BX_KEY_KP_DIVIDE;    break;

	  // Keys not handled by wxMSW
	  case 192: bx_key = BX_KEY_GRAVE;         break; // `~
	  case 219: bx_key = BX_KEY_LEFT_BRACKET;  break; // [{
	  case 221: bx_key = BX_KEY_RIGHT_BRACKET; break; // ]}
	  case 220: bx_key = BX_KEY_BACKSLASH;     break; // \|
	  case 186: bx_key = BX_KEY_SEMICOLON;     break; // ;:
	  case 222: bx_key = BX_KEY_SINGLE_QUOTE;  break; // '"
	  case 188: bx_key = BX_KEY_COMMA;         break; // ,<
	  case 191: bx_key = BX_KEY_SLASH;         break; // /?

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
  th->put("NGUI");
  if (bx_options.Oprivate_colormap->get ()) {
    BX_INFO(("private_colormap option ignored."));
  }

  for(int i = 0; i < 256; i++) {
	  wxBochsPalette[i].red = 0;
	  wxBochsPalette[i].green = 0;
	  wxBochsPalette[i].blue = 0;
  }

  wxScreenX = 640;
  wxScreenY = 480;
  wxCriticalSectionLocker lock(wxScreen_lock);
  wxScreen = (char *)malloc(wxScreenX * wxScreenY * 3);
  memset(wxScreen, 0, wxScreenX * wxScreenY * 3);

  wxTileX = tilewidth;
  wxTileY = tileheight;
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
  wxCriticalSectionLocker lock(wxScreen_lock);
  memset(wxScreen, 0, wxScreenX * wxScreenY * 3);
}

static void 
UpdateScreen(char *newBits, int x, int y, int width, int height) 
{
	wxCriticalSectionLocker lock(wxScreen_lock);
	if(wxScreen != NULL) {
		for(int i = 0; i < height; i++) {
			for(int c = 0; c < width; c++) {
				wxScreen[(y * wxScreenX * 3) + ((x+c) * 3)] = wxBochsPalette[newBits[(i * width) + c]].red;
				wxScreen[(y * wxScreenX * 3) + ((x+c) * 3) + 1] = wxBochsPalette[newBits[(i * width) + c]].green;
				wxScreen[(y * wxScreenX * 3) + ((x+c) * 3) + 2] = wxBochsPalette[newBits[(i * width) + c]].blue;
			}
			y++;
		}
	}
}

static void 
DrawBochsBitmap(int x, int y, int width, int height, char *bmap, char color)
{
	char bgcolor = (color >> 4) & 0xF;
	char fgcolor = color & 0xF;

	char *newBits = (char *)malloc(width * height);
	memset(newBits, 0, (width * height));
	for(int i = 0; i < (width * height) / 8; i++) {
		newBits[i * 8 + 0] = (bmap[i] & 0x01) ? fgcolor : bgcolor;
		newBits[i * 8 + 1] = (bmap[i] & 0x02) ? fgcolor : bgcolor;
		newBits[i * 8 + 2] = (bmap[i] & 0x04) ? fgcolor : bgcolor;
		newBits[i * 8 + 3] = (bmap[i] & 0x08) ? fgcolor : bgcolor;
		newBits[i * 8 + 4] = (bmap[i] & 0x10) ? fgcolor : bgcolor;
		newBits[i * 8 + 5] = (bmap[i] & 0x20) ? fgcolor : bgcolor;
		newBits[i * 8 + 6] = (bmap[i] & 0x40) ? fgcolor : bgcolor;
		newBits[i * 8 + 7] = (bmap[i] & 0x80) ? fgcolor : bgcolor;
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
	Bit8u cs_start = (cursor_state >> 8) & 0x3f;
	Bit8u cs_end = cursor_state & 0x1f;
	unsigned char cChar;
	unsigned int ncols = wxScreenX / 8;
	unsigned int nchars = ncols * nrows;
	if((wxCursorY * ncols + wxCursorX) < nchars) {
		cChar = new_text[(wxCursorY * ncols + wxCursorX) * 2];
		DrawBochsBitmap(wxCursorX * 8, wxCursorY * 16, 8, 16, (char *)&bx_vgafont[cChar].data, new_text[((wxCursorY * ncols + wxCursorX) * 2) + 1]);
	}
	
	for(unsigned int i = 0; i < nchars * 2; i += 2) {
		if((old_text[i] != new_text[i]) || (old_text[i+1] != new_text[i+1])) {
			cChar = new_text[i];
			int x = (i / 2) % ncols;
			int y = (i / 2) / ncols;
			DrawBochsBitmap(x * 8, y * 16, 8, 16, (char *)&bx_vgafont[cChar].data, new_text[i+1]);
		}
	}
	wxCursorX = cursor_x;
	wxCursorY = cursor_y;

	if(((cursor_y * ncols + cursor_x) < nchars) && (cs_start <= cs_end)) {
		cChar = new_text[(cursor_y * ncols + cursor_x) * 2];
		char cAttr = new_text[((cursor_y * ncols + cursor_x) * 2) + 1];
		cAttr = ((cAttr >> 4) & 0xF) + ((cAttr & 0xF) << 4);
		DrawBochsBitmap(wxCursorX * 8, wxCursorY * 16, 8, 16, (char *)&bx_vgafont[cChar].data, cAttr);
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
  UpdateScreen((char *)tile, x0, y0, wxTileX, wxTileY);
  //thePanel->MyRefresh ();
}

// ::DIMENSION_UPDATE()
//
// Called when the VGA mode changes it's X,Y dimensions.
// Resize the window to this size, but you need to add on
// the height of the headerbar to the Y value.
//
// x: new VGA x size
// y: new VGA y size (add headerbar_y parameter from ::specific_init().

void bx_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight)
{
  IFDBG_VGA (wxLogDebug ("dimension_update"));
  wxCriticalSectionLocker lock(wxScreen_lock);
  if (fheight > 0) {
    if (fheight != 16) {
      y = y * 16 / fheight;
    }
  }
  wxScreenX = x;
  wxScreenY = y;
  wxScreen = (char *)realloc(wxScreen, wxScreenX * wxScreenY * 3);
  wxASSERT (wxScreen != NULL);
  
  //theFrame->SetSize(-1, -1, wxScreenX + 6, wxScreenY + 100, 0);
  //wxSize size = theFrame->GetToolBar()->GetToolSize();
  theFrame->SetClientSize(wxScreenX, wxScreenY); // + size.GetHeight());
  //thePanel->MyRefresh ();
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
  UNUSED(bytes);
  UNUSED(nbytes);
  return 0;
}

  int
bx_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  UNUSED(text_snapshot);
  UNUSED(len);
  return 0;
}


#if 0
////////////// 
/*
This is basically the old wx.cc from June 2001.
In March 2002, when Psyon and I started working on wxWindows again,
the wx.cc was crashing and I didn't understand why.  So I started
over again in wxmain.cc, and we hacked it until it worked.

In this old code I had started working on separate parameter editors for
each bx_param_* type, and I expect to need that again.
*/



// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>
#include <wx/scrolwin.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif



#include "config.h"
extern "C" {
// siminterface needs stdio.h
#include <stdio.h>
}
#include "osdep.h"
#include "control.h"
#include "siminterface.h"

#include "gui/wx.h"

// I wanted to make this a static member of ParamEditor, but it gave me
// link errors so I just put it outside for now.
wxHashTable paramEditorsById;

// event table for ParamEditor and subclasses
BEGIN_EVENT_TABLE(ParamEditor, wxPanel)
  //EVT_CHECKBOX (-1, ParamEditor::OnEvent)
  //EVT_CHOICE (-1, ParamEditor::OnEvent)
  //EVT_TEXT(-1, ParamEditor::OnEvent)
  //EVT_TEXT_ENTER(-1, ParamEditor::OnEvent)
END_EVENT_TABLE()

// event table for wxFrame
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_Quit,  MyFrame::OnQuit)
  EVT_MENU(ID_StartBochs, MyFrame::HandleEvent)
  EVT_MENU(ID_ApplyAction, MyFrame::HandleEvent)
  EVT_MENU(ID_RevertAction, MyFrame::HandleEvent)
  EVT_MENU(ID_ReadConfig, MyFrame::HandleEvent)
  EVT_MENU(ID_WriteConfig, MyFrame::HandleEvent)
  EVT_MENU(ID_WriteConfigAs, MyFrame::HandleEvent)
END_EVENT_TABLE()

  // event table for MyPanel
BEGIN_EVENT_TABLE(MyPanel, wxScrolledWindow)
  EVT_BUTTON (ID_StartBochs, MyPanel::HandleEvent)
  EVT_BUTTON (ID_ApplyAction, MyPanel::HandleEvent)
  EVT_BUTTON (ID_RevertAction, MyPanel::HandleEvent)
  // do not call event handlers for the ParamEditors
  //EVT_CHECKBOX (ID_Check1, MyPanel::HandleEvent)
  //EVT_CHOICE (ID_Choice1, MyPanel::HandleEvent)
  //EVT_COMBOBOX (ID_Combo1, MyPanel::HandleEvent)
  //EVT_TEXT (ID_Combo1, MyPanel::HandleEvent)
END_EVENT_TABLE()




// event table for BochsEventHandler and subclasses
BEGIN_EVENT_TABLE(BochsEventHandler, wxEvtHandler)
  EVT_MENU (-1, BochsEventHandler::OnEvent)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  bochsThread = NULL;
  wxMenu *menuFile = new wxMenu;

  menuFile->Append( ID_StartBochs, "&Start Bochs..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_ReadConfig, "Read Configuration File" );
  menuFile->Append( ID_WriteConfig, "Write Configuration" );
  menuFile->Append( ID_WriteConfigAs, "Write Configuration As..." );
  menuFile->AppendSeparator();
  menuFile->Append( ID_Quit, "E&xit" );

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile, "&File" );

  SetMenuBar( menuBar );

  CreateStatusBar();
  SetStatusText( "Bochs Controls" );
  int width=500, height=3000;
  panel = new MyPanel ( this, this, -1, wxPoint(0,0), wxSize(width,height) );
  panel->SetScrollbars( 10, 10, width/10, height/10 );
}

void
MyFrame::HandleEvent (wxCommandEvent& event)
{
  panel->HandleEvent (event);
}

void
MyFrame::StartBochsThread ()
{
  if (bochsThread == NULL)
  {
    printf ("Starting bochs thread\n");
    bochsThread = new BochsThread ();
    bochsThread->Create ();
    bochsThread->Run ();
  } else {
    wxMessageBox (
    "Can't start Bochs simulator, because it is already running",
    "Already Running", wxOK | wxICON_ERROR);
  }
}


int 
MyFrame::ReadConfiguration (int ask_name)
{
  char oldrc_cstr[CI_PATH_LENGTH];
  int found_default = 1;
  if (SIM->get_default_rc (oldrc_cstr, CI_PATH_LENGTH) < 0) {
    found_default = 0;
    oldrc_cstr[0] = 0;
  }
  wxString oldrc (oldrc_cstr), newrc = oldrc;
  if (!found_default) {
    if (!ask_name) return -1;
    newrc = ::wxFileSelector ("Load which configuration file?", 
	"", oldrc, "", "", wxOPEN);
  }
  if (newrc.IsEmpty () || (newrc == "none")) return 0;
  if (SIM->read_rc ((char *)newrc.c_str ()) < 0) {
    wxMessageBox ("Error loading configuration file");
    return -1;
  }
  panel->forAllEditors (ID_RevertAction, paramEditorsById);
  return 0;
}

int 
MyFrame::WriteConfiguration (int ask_name)
{
  char oldrc_cstr[CI_PATH_LENGTH];
  int found_default = 1;
  if (SIM->get_default_rc (oldrc_cstr, CI_PATH_LENGTH) < 0) {
    found_default = 0;
    oldrc_cstr[0] = 0;
  }
  wxString oldrc (oldrc_cstr), newrc = oldrc;
  if (ask_name || !found_default) {
    newrc = ::wxFileSelector ("Save Configuration As", 
	"", oldrc, "", "", wxSAVE | wxOVERWRITE_PROMPT);
  }
  if (newrc.IsEmpty () || (newrc == "none")) return 0;
  if (SIM->write_rc ((char *)newrc.c_str (), 1) < 0) {
    wxMessageBox ("Error while saving configuration file");
    return -1;
  }
  return 0;
}

// static function version that can be referred to by function pointer
int 
MyFrame::ProcessBochsEvent (void *class_ptr, int id)
{
  printf ("MyFrame::ProcessBochsEvent\n");
  MyFrame *f = (MyFrame *)class_ptr;
  f->ProcessBochsEvent2 (id);
  return 0;
}

int 
MyFrame::ProcessBochsEvent2 (int code)
{
  printf ("MyFrame::ProcessBochsEvent2\n");
  switch (code)
  {
  case NOTIFY_CODE_LOGMSG:
    if (0) {
      // dumb static choices for now, I'll fix it once I get it to work
      //
      // this code works if the gui thread calls it, but it's doing bad
      // things when it gets called from the bochs thread.
      // I know it's legal for a non-GUI thread to enter a mutex, update
      // an object like "m_frame->WriteText(msg)" and then leave the mutex.
      // But this code puts up a modal dialog and waits for user input.
      // If you comment out the mutex code, you get:
      // GLib-WARNING **: g_main_run(): called recursively from within a source's check() or prepare() member or from a second thread, iteration not possible
      // And if you uncomment the mutex, it displays the dialog correctly
      // but then clicking on it does nothing...deadlock.
      // Maybe this must be resolved by posting a message between threads
      // instead.

      wxMutexGuiEnter();
      static wxString choices[] = { "Die", "Continue", "Continue and Disable" };
      char prefix[512], msg[512];
      int level;
      assert (SIM->log_msg_2 (prefix, &level, msg, sizeof(msg)) >= 0);
      wxString string;
      string.Printf ("%s", msg);
      int choice = ::wxGetSingleChoiceIndex (
	  string,
	  wxString (SIM->get_log_level_name (level)), 3, choices);
      wxMutexGuiLeave();
      printf ("you chose %d\n", choice);
    } else {
  //////////// temporary! still implement this with stdin/stdout ///////////
      int level;
      char prefix[512], msg[512];
      assert (SIM->log_msg_2 (prefix, &level, msg, sizeof(msg)) >= 0);
      fprintf (stderr, "========================================================================\n");
      fprintf (stderr, "Event type: %s\n", SIM->get_log_level_name (level));
      fprintf (stderr, "Device: %s\n", prefix);
      fprintf (stderr, "Message: %s\n\n", msg);
      fprintf (stderr, "A %s has occurred.  Do you want to:\n", SIM->get_log_level_name (level));
      fprintf (stderr, "  cont       - continue execution\n");
      fprintf (stderr, "  alwayscont - continue execution, and don't ask again.\n");
      fprintf (stderr, "               This affects only %s events from device %s\n", SIM->get_log_level_name (level), prefix);
      fprintf (stderr, "  die        - stop execution now\n");
      int choice;
      extern int ask_menu (char *prompt, int n_choices, char *choice[], int the_default, int *out);
      char *log_action_ask_choices[] = { "cont", "alwayscont", "die" };
      int log_action_n_choices = 3;
      if (ask_menu ("Choose cont, alwayscont, or die. [%s] ", 3,  
	    log_action_ask_choices, 2, &choice) < 0) 
	return SIM->notify_return(-1);
      // return 0 for continue, 1 for alwayscontinue, 2 for die.
      SIM->notify_return(choice);
    }
    break;
  default:
    fprintf (stderr, "Control panel: notify callback called with unknown code %04x\n", code);
  }
}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close( TRUE );
  if (bochsThread != NULL)
    bochsThread->Delete ();
}

BochsEventHandler *bochsEventHandler;

BochsEventHandler::BochsEventHandler ()
{
  printf ("BochsEventHandler::BochsEventHandler\n");
}

void
BochsEventHandler::OnEvent (wxCommandEvent& evt)
{
  printf ("BochsEventHandler::OnEvent called with event id=%d\n", evt.GetId ());
}
int wxid_count = ID_LAST_STATIC_ASSIGNED_ID;

IMPLEMENT_APP(BochsApp)

bool BochsApp::OnInit()
{
  // do bochs initialization first
  siminterface_init ();
  MyFrame *frame = new MyFrame( "Bochs Control Panel", wxPoint(50,50), wxSize(450,340) );
  SIM->set_notify_callback (frame->ProcessBochsEvent, frame);
  bochsEventHandler = new BochsEventHandler ();
  frame->Show( TRUE );
  SetTopWindow( frame );
  return TRUE;
}


void MyPanel::HandleEvent (wxCommandEvent& evt)
{
  int id = evt.GetId ();
  printf ("Handle event with event id %d\n", id);
  switch (id)
  {
    case ID_StartBochs:
      frame->StartBochsThread ();
      break;
    case ID_ApplyAction:
    case ID_RevertAction:
      forAllEditors (id, paramEditorsById);
      break;
    case ID_ReadConfig:
      frame->ReadConfiguration (1);
      break;
    case ID_WriteConfig:
      frame->WriteConfiguration (0);
      break;
    case ID_WriteConfigAs:
      frame->WriteConfiguration (1);
      break;
    default:
      printf ("HandleEvent called with id=%d\n", id);
  }
}

void 
MyPanel::forAllEditors (int evt_id, wxHashTable& hash)
{
  printf ("MyPanel::forAllEditors");
  // iterate over editors in the editors hashtable
  hash.BeginFind ();
  wxNode *node;
  while (node = hash.Next ()) 
  {
    ParamEditor *editor = (ParamEditor *)node->Data ();
    editor->Action (evt_id);
  }
}

void *
BochsThread::Entry (void)
{
  int argc=1;
  char *argv[] = {"bochs"};
  bx_continue_after_config_interface (argc, argv);
  return NULL;
}

MyPanel::MyPanel (MyFrame *frame, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
  : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER | wxTAB_TRAVERSAL, "Bochs Controls" )
{
  this->frame = frame;
  // the parent constructor makes this panel a child of frame, with 
  // proper size.
  (void) new wxButton (this, ID_StartBochs, "Start Bochs", wxPoint (10, 10));
  (void) new wxButton (this, ID_ApplyAction, "Apply", wxPoint (110, 10));
  (void) new wxButton (this, ID_RevertAction, "Revert", wxPoint (210, 10));
  //wxCheckBox *check1 = new wxCheckBox (this, ID_Check1, "Checkbox", wxPoint (10, 50));
  //wxString choices[] = { "apple", "orange", "banana" };
  //wxChoice *choice1 = new wxChoice (this, ID_Choice1, wxPoint (10, 80), wxSize (80, 30), 3, choices);
  //wxComboBox *combo1 = new wxComboBox (this, ID_Combo1, "Default Combo String", wxPoint (10, 130), wxSize (180, 80), 3, choices);
  //text1 = new wxStaticText (this, ID_Text1, "no text yet", wxPoint (0, 160));
  frame->ReadConfiguration (0);
  buildParamList (10, 40);
  // grab all the settings from siminterface into the GUI
  forAllEditors (ID_RevertAction, paramEditorsById);
}

MyPanel::~MyPanel()
{
}

void MyPanel::buildParamList (int x, int y)
{
  // get a list of parameters
  int min, max;
  SIM->get_param_id_range (&min, &max);
  for (int i=min; i<=max; i++) {
    bx_param_c *param = SIM->get_param ((bx_id) i);
    if (param != NULL) {
      printf ("param %d is called %s\n", i, param->get_name ());
      new wxStaticText (this, i, param->get_name (), wxPoint (x, y));
      int wxid = wxid_count++;
      ParamEditor *editor = ParamEditor::createEditor (param, this, wxid, x+120, y);
      y+=30;
    }
  }
}

/////////////////////////////////////////////////////////////////////////
// Parameter editor classes
/////////////////////////////////////////////////////////////////////////

ParamEditor::ParamEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : wxPanel(parent, id, wxPoint(x, y), wxSize (300, 30))
{
  printf ("ParamEditor::ParamEditor\n");
  this->param = param;
  //wxString text = wxString (param->get_name ());
  //new wxStaticText (parent, id, text, wxPoint (x, y));
}

ParamEditor *
ParamEditor::createEditor (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
{
  printf ("ParamEditor::createEditor\n");
  Bit8u type = param->get_type ();
  ParamEditor *it = NULL;
  switch (type) {
    case BXT_PARAM_BOOL:
      it = new ParamEditorBool (param, parent, id, x, y);
      break;
    case BXT_PARAM_NUM:
      it = new ParamEditorNum (param, parent, id, x, y);
      break;
    case BXT_PARAM_STRING:
      it = new ParamEditorString (param, parent, id, x, y);
      break;
    case BXT_PARAM_ENUM:
      it = new ParamEditorEnum (param, parent, id, x, y);
      break;
    case BXT_LIST:
      it = new ParamEditorList (param, parent, id, x, y);
      break;
    default:
      printf ("ParamEditor::createEditor with unknown type %d\n", type);
      it = NULL;
  }
  paramEditorsById.Put (id, it);
  return it;
}

ParamEditorBool::ParamEditorBool (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  component = new wxCheckBox (this, id, "", wxPoint (0, 0));
}

void 
ParamEditorBool::Action (int evt_id)
{
  printf ("ParamEditorBool::Action\n");
  bx_param_bool_c *param = (bx_param_bool_c *) get_param ();
  switch (evt_id) {
    case ID_ApplyAction:
      param->set (component->GetValue ());
      break;
    case ID_RevertAction:
      component->SetValue (param->get ());
      break;
    default:
      wxASSERT (0);
  }
}

ParamEditorNum::ParamEditorNum (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  // background color helps to visualize the borders of the panel
  //SetBackgroundColour (wxColour ("red"));
  bx_param_num_c *num = (bx_param_num_c *)param;
  wxString text;
  text.Printf ("%d", num->get ());
  component = new wxTextCtrl (this, id, text, wxPoint (-1, -1), wxSize (-1, -1), wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB);
}

void 
ParamEditorNum::Action (int evt_id)
{
  printf ("ParamEditorNum::Action\n");
  bx_param_num_c *param = (bx_param_num_c *) get_param ();
  int base = param->get_base ();
  wxASSERT (base==10 || base==16);
  switch (evt_id) {
    case ID_ApplyAction:
      {
	const char *string = component->GetValue ().c_str ();
	int n, status;
	if (base==10) status = wxSscanf (string, "%d", &n);
	else status = wxSscanf (string, "%x", &n);
	if (status != 1) {
	  printf ("Invalid number in %s, value = '%s'\n", param->get_name (), string);
	  return;
	}
	param->set (n);
      }
      break;
    case ID_RevertAction:
      {
	int n = param->get ();
	wxString string;
	if (base == 10) string.Printf ("%d", n);
	else string.Printf ("%x", n);
        component->SetValue (string);
      }
      break;
    default:
      wxASSERT (0);
  }
}

ParamEditorString::ParamEditorString (bx_param_c *param, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (param, parent, id, x, y)
{
  //SetBackgroundColour (wxColour ("red"));
  bx_param_string_c *string = (bx_param_string_c *)param;
  wxString text = wxString (string->getptr ());
  component = new wxTextCtrl (this, id, text, wxPoint (-1, -1), wxSize (-1, -1), wxTE_PROCESS_ENTER);
}

void 
ParamEditorString::Action (int evt_id)
{
  bx_param_string_c *param = (bx_param_string_c *) get_param ();
  const char *string;
  switch (evt_id) {
    case ID_ApplyAction:
      string = component->GetValue ().c_str ();
      param->set ((char *)string);
      break;
    case ID_RevertAction:
      component->SetValue (wxString (param->getptr ()));
      break;
    default:
      wxASSERT (0);
  }
}


ParamEditorEnum::ParamEditorEnum (bx_param_c *genericparam, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (genericparam, parent, id, x, y)
{
  //SetBackgroundColour (wxColour ("red"));
  
  // build the array of choices
  bx_param_enum_c *param = (bx_param_enum_c *) genericparam;
  n_choices = param->get_max () - param->get_min () + 1;
  choices = new wxString[n_choices];
  for (int i=0; i<n_choices; i++)
    choices[i] = param->get_choice (i);
  component = new wxChoice (this, id, wxPoint (-1, -1), wxSize (-1, -1), n_choices, choices);
}

void 
ParamEditorEnum::Action (int evt_id)
{
  printf ("ParamEditorEnum::Action\n");
  bx_param_enum_c *param = (bx_param_enum_c *) get_param ();
  int min = param->get_min ();
  switch (evt_id) {
    case ID_ApplyAction:
      param->set (min + component->GetSelection ());
      break;
    case ID_RevertAction:
      component->SetSelection (param->get () - min);
      break;
    default:
      wxASSERT (0);
  }
}



ParamEditorList::ParamEditorList (bx_param_c *genericparam, wxWindow *parent, wxWindowID id, int x, int y)
  : ParamEditor (genericparam, parent, id, x, y)
{
  //SetBackgroundColour (wxColour ("red"));
  bx_list_c *param = (bx_list_c *) genericparam;
  component = new wxStaticText (this, id, "list param not implemented", wxPoint (-1, -1));
}

void 
ParamEditorList::Action (int evt_id)
{
  printf ("ParamEditorList::Action\n");
}

bx_param_c *
ParamEditor::get_param ()
{
  printf ("You called get_param on ParamEditor %p\n", this);
  printf ("My param value is %p\n", this->param);
  return this->param;
}

void
ParamEditor::OnEvent (wxCommandEvent& evt)
{
  // if you want to run some action on the object that sent this event,
  // see the code below.
#if 0
  int id = evt.GetId ();
  wxObject *object = evt.GetEventObject ();
  printf ("ParamEditor::OnEvent, id=%d, object=%p\n", id, object);
  ParamEditor *genericeditor = (ParamEditor *) paramEditorsById.Get (id);
  if (genericeditor == NULL) {
    printf ("I couldn't find the parameter editor for id=%d\n", id);
    return;
  }
  bx_param_c *genericparam = genericeditor->get_param ();
  wxASSERT (genericparam != NULL);
  printf ("This event will be sent to %s\n", genericparam->get_name ());
  genericeditor->Action (evt);
#endif
}
#endif
