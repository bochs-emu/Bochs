#include <stdio.h>
#include "main.h"

void module_init ()
{
  printf ("module1 init\n");
  register_module ("module1");
}

int operate (int a, int b)
{
  return a + b;
}
