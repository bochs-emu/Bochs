/////////////////////////////////////////////////////////////////////////
// $Id: extplugin.h,v 1.2 2002-10-24 21:04:29 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
// extplugin.h
//
// This header file defines the types necessary to make a Bochs plugin,
// but without mentioning all the details of Bochs internals (bochs.h).  
// It is included by the configuration interfaces and possibly other 
// things which are intentionally isolated from other parts of the program.
//
// The plugin_t struct comes from the plugin.h file from plex86.
// Plex86 is Copyright (C) 1999-2000  The plex86 developers team
//
/////////////////////////////////////////////////////////////////////////

#ifndef __EXTPLUGIN_H
#define __EXTPLUGIN_H

#include <ltdl.h>

enum plugintype_t {
  PLUGTYPE_NULL=100,
  PLUGTYPE_CORE,
  PLUGTYPE_OPTIONAL,
  PLUGTYPE_USER
};

#define MAX_ARGC 10

typedef struct _plugin_t
{
    plugintype_t type;
    int  initialized;
    lt_dlhandle handle;
    int  argc;
    char *name, *args, *argv[MAX_ARGC];
    int  (*plugin_init)(struct _plugin_t *plugin, plugintype_t type, int argc, char *argv[]);
    void (*plugin_fini)(void);

    struct _plugin_t *next;
} plugin_t;



#endif /* __EXTPLUGIN_H */

