#include <stdio.h>
#define MODULE2_DLL_EXPORT
#include "module2.h"

int n_operations = 0;

void operation_occurred () {
  printf ("module2: operation_occurred\n");
  n_operations++;
}
