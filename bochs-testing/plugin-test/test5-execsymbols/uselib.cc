#include <stdio.h>
#include <stdlib.h>
#include "module1.h"
#include "module2.h"
#include "uselib.h"

int register_module (const char *name)
{
  printf ("register_module was called by module '%s'\n", name);
  return 0;
}

int main ()
{
  printf ("start\n");
  printf ("at first, n_operations = %d\n", n_operations);
  printf ("Module name is '%s'\n", module_name);
  int a=5, b=12;
  int c = operate (a, b);
  printf ("operate(%d,%d) = %d\n", a, b, c);
  int d = operate (a, c);
  printf ("operate(%d,%d) = %d\n", a, c, d);
  printf ("stop\n");
  printf ("at end, n_operations = %d\n", n_operations);
  exit(77);
}
