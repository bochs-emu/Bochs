#include <stdio.h>
#include "main.h"
#define _buildsym(sym) module2_LTX_##sym
#include "modules.h"

int n_operations = 0;

class ModuleGadget* module_init ()
{
  printf ("module2 init for main version %s\n", version_string);
  register_module ("module2");
  return NULL;
}

int operate (int a, int b)
{
  return a - b;
}
