#include <stdio.h>
#define MODULE1_DLL_EXPORT
#include "module1.h"

const char *module_name = "AddModule";

int operate (int a, int b)
{
  printf ("module1: operate was called\n");
  return a + b;
}
