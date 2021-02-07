/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2021  The Bochs Project
//
// extplugin.h
//
// This header file defines the types necessary to make a Bochs plugin,
// but without mentioning all the details of Bochs internals (bochs.h).
// It is included by the configuration interfaces and possibly other
// things which are intentionally isolated from other parts of the program.
//
// The original plugin_t struct comes from the plugin.h file from plex86.
// Plex86 is Copyright (C) 1999-2000  The plex86 developers team
//
/////////////////////////////////////////////////////////////////////////

#ifndef __EXTPLUGIN_H
#define __EXTPLUGIN_H

#if BX_PLUGINS && !defined(WIN32)
#if BX_HAVE_LTDL
#include <ltdl.h>
#else
#include "ltdl-bochs.h"
#endif
#endif

enum plugintype_t {
  PLUGTYPE_NULL     = 0x00,
  PLUGTYPE_CORE     = 0x01,
  PLUGTYPE_STANDARD = 0x02,
  PLUGTYPE_OPTIONAL = 0x04,
  PLUGTYPE_VGA      = 0x08,
  PLUGTYPE_USB      = 0x80,
  PLUGTYPE_GUI      = 0x100,
  PLUGTYPE_IMG      = 0x200,
  PLUGTYPE_NET      = 0x400,
  PLUGTYPE_SND      = 0x800,
};

#define PLUGIN_FINI  0
#define PLUGIN_INIT  1
#define PLUGIN_PROBE 2

typedef int (CDECL *plugin_entry_t)(struct _plugin_t *plugin, plugintype_t type, Bit8u mode);

typedef struct _plugin_t
{
#if BX_PLUGINS
    char *name;
#if defined(WIN32)
    HINSTANCE handle;
#else
    lt_dlhandle handle;
#endif
#else
    const char *name;
#endif
    plugintype_t type;
    plugin_entry_t plugin_entry;
    bool initialized;
#if BX_PLUGINS
    plugintype_t loadtype;

    struct _plugin_t *next;
#endif
} plugin_t;

#endif /* __EXTPLUGIN_H */
