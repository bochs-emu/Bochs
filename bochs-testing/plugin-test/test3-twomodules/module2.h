#ifndef MODULE2API
#  if defined(_MSC_EXTENSIONS) && !defined(__BEOS__) && !defined(__CYGWIN__)
#    define MODULE2API(type) __declspec(dllimport) type __cdecl
#  else
#    define MODULE2API(type) type
#  endif
#endif

MODULE2API(extern int) n_operations;
MODULE2API(void) operation_occurred ();

