#include <stdio.h>
#include <ltdl.h>
#define MAIN_DLL_EXPORT
#include "main.h"

const char *version_string = "uselib-test6-1.0";

int register_module (const char *name)
{
  printf ("register_module was called by module '%s'\n", name);
  return 0;
}

int load_module (const char *fmt, const char *name)
{
  char buf[512];
  sprintf (buf, fmt, name);
  printf ("loading module from %s\n", buf);
  lt_dlhandle handle = lt_dlopen (buf);
  printf ("handle is %p\n", handle);
  if (!handle) {
    printf ("lt_dlopen error: %s\n", lt_dlerror ());
    return -1;
  }
  modload_func func = (modload_func) lt_dlsym (handle, "module_init");
  printf ("module_init function is at %p\n", func);
  if (func != NULL) {
    printf ("Calling module_init\n");
    (*func)();
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
  printf ("loading module1\n");
  // try to load module1
  if (load_module ("lib%s.la", "module1") < 0) {
    printf ("load module1 failed\n");
  }
  if (load_module ("lib%s.la", "module2") < 0) {
    printf ("load module2 failed\n");
  }
  int arg;
  for (int arg=1; arg < argc; arg++) {
    if (load_module ("%s", argv[arg]) < 0) {
      printf ("load %s failed\n", argv[arg]);
    }
  }

  printf ("stop\n");
  return 0;
}
