#include <stdio.h>
#define MODULE2_DLL_EXPORT
#include "module2.h"
#include "uselib.h"

int n_operations = 0;

void operation_occurred () {
  static int first_time = 1;
  if (first_time) {
    register_module ("module2");
    first_time = 0;
  }
  printf ("module2: operation_occurred\n");
  n_operations++;
}
