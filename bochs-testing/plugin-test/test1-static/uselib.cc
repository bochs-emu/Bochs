#include <stdio.h>
#include <stdlib.h>
#include "module1.h"

int main ()
{
  printf ("start\n");
  printf ("Module name is '%s'\n", module_name);
  int a=5, b=12;
  int c = operate (a, b);
  printf ("operate(%d,%d) = %d\n", a, b, c);
  printf ("stop\n");
  exit(77);
}
