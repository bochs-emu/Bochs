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

