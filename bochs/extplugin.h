/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002-2017  The Bochs Project
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
  PLUGTYPE_GUI=100,
  PLUGTYPE_CORE,
  PLUGTYPE_STANDARD,
  PLUGTYPE_OPTIONAL,
  PLUGTYPE_SOUND,
  PLUGTYPE_NETWORK,
  PLUGTYPE_USBDEV,
  PLUGTYPE_VGA,
  PLUGTYPE_USER
};

typedef int (CDECL *plugin_init_t)(struct _plugin_t *plugin, plugintype_t type);
typedef void (CDECL *plugin_fini_t)(void);

typedef struct _plugin_t
{
    plugintype_t type;
    int  initialized;
#if BX_PLUGINS
#if defined(WIN32)
    HINSTANCE handle;
#else
    lt_dlhandle handle;
#endif
#endif
    char *name;
    plugin_init_t plugin_init;
    plugin_fini_t plugin_fini;

    struct _plugin_t *next;
} plugin_t;



#endif /* __EXTPLUGIN_H */
