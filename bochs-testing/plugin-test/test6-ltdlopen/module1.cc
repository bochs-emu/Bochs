#include <stdio.h>
#include "main.h"
#define _buildsym(sym) module1_LTX_##sym
#include "modules.h"

void module_init ()
{
  printf ("module1 init for main version %s\n", version_string);
  register_module ("module1");
}

int operate (int a, int b)
{
  return a + b;
}
