/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2014  Volker Ruppert
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

#include "config.h"

#if BX_USE_WIN32CONFIG

#include <windows.h>
#include <commctrl.h>

LRESULT CALLBACK ScrollWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  SCROLLINFO si;
  BOOL pull = FALSE, redraw = FALSE;
  static int vsize = 0, wsize = 0, starty = 0;
  int scrolly, oldy = 0;
  short delta;
  RECT R;

  oldy = starty;
  switch (msg) {
    case WM_CREATE:
      GetClientRect(hwnd, &R);
      wsize = R.bottom;
      vsize = wsize;
      starty = 0;
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask = SIF_POS | SIF_PAGE | SIF_RANGE | SIF_DISABLENOSCROLL;
      si.nMin = 0;
      si.nMax = vsize - 1;
      si.nPage = wsize;
      si.nPos = starty;
      SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
      break;
    case WM_VSCROLL:
      switch (LOWORD(wParam)) {
        case SB_LINEDOWN:
          if (starty < (vsize - wsize)) {
            starty++;
            redraw = TRUE;
          }
          break;
        case SB_LINEUP:
          if (starty > 0) {
            starty--;
            redraw = TRUE;
          }
          break;
        case SB_PAGEDOWN:
          if (starty < (vsize - wsize)) {
            starty += wsize;
            if (starty > (vsize - wsize)) {
              starty = vsize - wsize;
            }
            redraw = TRUE;
          }
          break;
        case SB_PAGEUP:
          if (starty > 0) {
            starty -= wsize;
            if (starty < 0) starty = 0;
            redraw = TRUE;
          }
          break;
        case SB_TOP:
          if (starty > 0) {
            starty = 0;
            redraw = TRUE;
          }
          break;
        case SB_BOTTOM:
          starty = vsize - wsize;
          redraw = TRUE;
          break;
        case SB_THUMBPOSITION:
          redraw = TRUE;
          break;
        case SB_THUMBTRACK:
          starty = HIWORD(wParam);
          SetScrollPos(hwnd, SB_VERT, starty, TRUE);
          pull = TRUE;
          redraw = (starty != oldy);
          break;
      }
      break;
    case WM_MOUSEWHEEL:
      delta = (short)HIWORD(wParam);
      if (delta < 0) {
        if (starty < (vsize - wsize)) {
          starty += 3;
          if (starty > (vsize - wsize)) {
            starty = vsize - wsize;
          }
          redraw = TRUE;
          break;
        }
      } else if (delta > 0) {
        if (starty > 0) {
          starty -= 3;
          if (starty < 0) starty = 0;
          redraw = TRUE;
          break;
        }
      }
      return 0;
    case WM_USER:
      if (wParam == 0x1234) {
        vsize = (int)lParam;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_RANGE | SIF_DISABLENOSCROLL;
        si.nMin = 0;
        si.nMax = vsize - 1;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        redraw = TRUE;
      }
      break;
  }
  if (redraw) {
    if (!pull) {
      SetScrollPos(hwnd, SB_VERT, starty, TRUE);
    }
    scrolly = oldy - starty;
    if (scrolly != 0) {
      ScrollWindowEx(hwnd, 0, scrolly, NULL, NULL, NULL, NULL, SW_INVALIDATE | SW_ERASE | SW_SCROLLCHILDREN);
    }
    return 0;
  } else {
    return DefWindowProc(hwnd, msg, wParam,lParam);
  }
}

BOOL RegisterScrollWindow(HINSTANCE hinst)
{
  WNDCLASS wc;

  memset(&wc,0,sizeof(WNDCLASS));
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = (WNDPROC)ScrollWinProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = hinst;
  wc.hIcon         = NULL;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "ScrollWin";

  return (RegisterClass(&wc) != 0);
}

#endif
