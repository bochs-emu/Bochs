#include <stdio.h>
#include "main.h"
#define _buildsym(sym) module1_LTX_##sym
#include "modules.h"

void module_init ()
{
  printf ("module1 init for main version %s\n", version_string);
  register_module ("module1");
  if (widgets[2] != NULL) {
    printf ("module1 is examining widgets[2]\n");
    printf ("module1: widgets[2] name is '%s'\n", widgets[2]->getName ());
  }
  printf ("module1 will print every widget that it can find\n");
  for (int i=0; i<MAX_WIDGETS; i++) {
    if (widgets[i]) {
      printf ("module1: widgets[%d] is ", i);
      widgets[i]->print (stdout);
      printf ("\n");
    }
  }
}

int operate (int a, int b)
{
  return a + b;
}
