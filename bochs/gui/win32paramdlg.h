/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2026  Volker Ruppert
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

#ifndef BX_WIN32_PARAMDLG_H
#define BX_WIN32_PARAMDLG_H

#include "config.h"

#if BX_USE_WIN32CONFIG

#define BXPD_SET_PATH    1
#define BXPD_UPDATE_DEPS 2
#define BXPD_EDIT_START  4
#define BXPD_EDIT_END    8

UINT getNextDlgID(void);
BOOL CreateParamDlgTooltip(HWND hwndDlg);
VOID OnWMNotify(HWND hDlg, UINT mid, LPARAM lParam);
int AskFilename(HWND hwnd, bx_param_filename_c *param, char *buffer);
void InitParamDialog(bool _embedded);
void CleanupParamDialog(bx_list_c *list);
SIZE CreateParamList(HWND hDlg, UINT mid, UINT lid, UINT xpos, UINT ypos, BOOL hide, bx_list_c *list, UINT grpbox);
void ShowParamList(HWND hDlg, UINT lid, BOOL show, bx_list_c *list);
UINT HandleChildWindowEvents(HWND hDlg, UINT mid, UINT_PTR id, UINT_PTR nc, bx_list_c *list);
INT_PTR win32ParamDialog(HWND parent, const char *menu);
INT_PTR win32FloppyParamDialog(HWND parent, const char *menu);

#endif

#endif // BX_WIN32_PARAMDLG_H
