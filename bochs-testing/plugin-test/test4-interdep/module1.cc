#define MODULE1_DLL_EXPORT
#include "module1.h"
#include "module2.h"

const char *module_name = "AddModule";

int operate (int a, int b)
{
  operation_occurred ();
  return a + b;
}
