#ifndef DLLINTERFACE
#define DLLINTERFACE __declspec(dllimport)
#endif

DLLINTERFACE extern const char * module_name;
DLLINTERFACE extern int operate (int a, int b);

