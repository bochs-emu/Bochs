#define DLLINTERFACE __declspec(dllexport)
#include "module1.h"

const char *module_name = "AddModule";

int operate (int a, int b)
{
  return a + b;
}
