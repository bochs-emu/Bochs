#include <stdio.h>
#include "main.h"

int n_operations = 0;

void module_init ()
{
  printf ("module2 init for main version %s\n", version_string);
  register_module ("module2");
}

int operate (int a, int b)
{
  return a - b;
}
