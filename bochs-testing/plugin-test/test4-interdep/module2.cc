#include <stdio.h>

#define DLLINTERFACE __declspec(dllexport)
#include "module2.h"
#undef DLLINTERFACE

int n_operations = 0;

void operation_occurred () {
  printf ("module2: operation_occurred\n");
  n_operations++;
}
