// set up MODULE1API macro for DLL export
#if defined(_MSC_EXTENSIONS) && !defined(__BEOS__) && !defined(__CYGWIN__)
#  define MODULE1API(type) __declspec(dllexport) type __cdecl
#else
#  define MODULE1API(type) type
#endif

#include "module1.h"
#include "module2.h"

const char *module_name = "AddModule";

int operate (int a, int b)
{
  return a + b;
}
