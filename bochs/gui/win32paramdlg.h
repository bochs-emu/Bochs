/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2014  Volker Ruppert
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

int AskFilename(HWND hwnd, bx_param_filename_c *param, char *buffer);
INT_PTR win32ParamDialog(HWND parent, const char *menu);
INT_PTR win32FloppyParamDialog(HWND parent, const char *menu);

#endif

#endif // BX_WIN32_PARAMDLG_H
