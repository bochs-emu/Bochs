#ifndef WIN32
#  error This test is specific to Windows/cygwin/mingw.  Turn back now!
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#define MAIN_DLL_EXPORT
#include "main.h"

MAINAPI const char *version_string = "uselib-test6-1.0";

MAINAPI int register_module (const char *name)
{
  printf ("register_module was called by module '%s'\n", name);
  return 0;
}

void print_last_error (char *fmtstring)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
    printf (fmtstring, (char*)lpMsgBuf);
	LocalFree (lpMsgBuf);
}

int load_module (const char *fmt, const char *modname)
{
  char buf[512];
  sprintf (buf, fmt, modname);
  printf ("loading module from VARIES{%s}\n", buf);
  HMODULE handle = LoadLibrary(buf);
  printf ("handle is VARIES{0x%p}\n", handle);
  if (!handle) {
    print_last_error ("LoadLibrary failed: %s\n");
    return -1;
  }
  modload_func func = (modload_func) GetProcAddress (handle, "module_init");
  printf ("module_init function is at VARIES{0x%p}\n", func);
  if (func != NULL) {
    printf ("Calling module_init\n");
    (*func)();
  } else {
    print_last_error ("GetProcAddress failed: %s\n");
    return -1;
  }
  return 0;
}

int main (int argc, char **argv)
{
  printf ("start\n");
  printf ("loading module1\n");
  // try to load module1
  if (load_module ("%s.dll", "module1") < 0) {
    printf ("load module1 failed\n");
  }
  if (load_module ("%s.dll", "module2") < 0) {
    printf ("load module2 failed\n");
  }
  for (int arg=1; arg < argc; arg++) {
    if (load_module ("%s.dll", argv[arg]) < 0) {
      printf ("load %s failed\n", argv[arg]);
    }
  }

  printf ("stop\n");
  exit(77);
}
