#ifndef DLLINTERFACE
#define DLLINTERFACE __declspec(dllimport)
#endif

DLLINTERFACE extern int n_operations;
DLLINTERFACE void operation_occurred ();

