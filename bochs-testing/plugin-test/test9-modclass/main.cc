#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define LT_SCOPE extern /* so that ltdl.h does not export anything */
#include <ltdl.h>
#define MAIN_DLL_EXPORT
#include "main.h"
#include "modules.h"

const char *version_string = "uselib-test6-1.0";

#if 1
// This lines forces main.cc to link in the functions associated with
// the new operator.  If there is no "new" in the whole executable, the
// Solaris linker decides not to link in __builtin_new, giving this error
// when you try to load a module:
//   fatal: relocation error: file .libs/libmodule1.so.0: 
//            symbol __builtin_new: referenced symbol not found
int *force_builtin_new = new int;
#endif

int register_module (const char *name)
{
  printf ("register_module was called by module '%s'\n", name);
  return 0;
}

int load_module (const char *fmt, const char *modname)
{
  char buf[512];
  sprintf (buf, fmt, modname);
  printf ("loading module from VARIES{%s}\n", buf);
  lt_dlhandle handle = lt_dlopenext (buf);
  printf ("handle is VARIES{%p}\n", handle);
  if (!handle) {
    printf ("lt_dlopen error: %s\n", lt_dlerror ());
    return -1;
  }
  char sym[1024];
  sprintf (sym, "%s_LTX_%s", modname, "module_init");
  modload_func func = (modload_func) lt_dlsym (handle, sym);
  printf ("module_init function is at VARIES{%p}\n", func);
  if (func != NULL) {
    printf ("Calling module_init\n");
    ModuleGadget *gadget = (*func)();
    if (gadget) {
      printf ("%s's gadget is called '%s'\n", modname, gadget->getName ());
      printf ("%s can do '%s'\n", modname, gadget->getFeatures ());
    } else {
      printf ("%s has not defined any gadget\n", modname);
    }
  } else {
    printf ("lt_dlsym error: %s\n", lt_dlerror ());
    return -1;
  }
  return 0;
}

int main (int argc, char **argv)
{
  printf ("start\n");
  if (lt_dlinit () != 0) {
    printf ("lt_dlinit error: %s\n", lt_dlerror ());
    return -1;
  }
#ifdef WIN32
  const char *module_name_format = "%s";
#else
  const char *module_name_format = "lib%s.la";
#endif
  printf ("loading module1\n");
  // try to load module1
  if (load_module (module_name_format, "module1") < 0) {
    printf ("load module1 failed\n");
  }
  if (load_module (module_name_format, "module2") < 0) {
    printf ("load module2 failed\n");
  }
  int arg;
  for (int arg=1; arg < argc; arg++) {
    if (load_module (module_name_format, argv[arg]) < 0) {
      printf ("load %s failed\n", argv[arg]);
    }
  }

  printf ("stop\n");
  exit (77);
}
