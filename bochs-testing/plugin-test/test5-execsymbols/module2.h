#if defined(WIN32) || defined(__CYGWIN__)
#  ifdef MODULE2_DLL_EXPORT
#    ifdef DLL_EXPORT
#      warning I will export DLL symbols for MODULE2
#      define MODULE2API(type) __declspec(dllexport) type
#    endif
#  else
#    warning I will import DLL symbols for MODULE2
#    define MODULE2API(type) __declspec(dllimport) type
#  endif
#endif
#ifndef MODULE2API
#  warning No DLL import/export is needed
#  define MODULE2API(type) type
#endif


MODULE2API(extern int) n_operations;
MODULE2API(void) operation_occurred ();
