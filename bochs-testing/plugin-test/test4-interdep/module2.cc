// set up MODULE2API macro for DLL export
#if defined(_MSC_EXTENSIONS) && !defined(__BEOS__) && !defined(__CYGWIN__)
#  define MODULE2API(type) __declspec(dllexport) type __cdecl
#else
#  define MODULE2API(type) type
#endif

#include <stdio.h>
#include "module2.h"

int n_operations = 0;

void operation_occurred () {
  printf ("module2: operation_occurred\n");
  n_operations++;
}
