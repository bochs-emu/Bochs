#include <stdio.h>
#include "main.h"

extern "C" {
  // prevent C++ name mangling
  void module_init();
};

void module_init ()
{
  printf ("module1 init\n");
  register_module ("module1");
}

int operate (int a, int b)
{
  return a + b;
}
