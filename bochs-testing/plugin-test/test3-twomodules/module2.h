#if defined(WIN32) || defined(__CYGWIN__)
#  ifdef MODULE2_DLL_EXPORT
#    ifdef DLL_EXPORT
#      warning case 1
#      define MODULE2API(type) __declspec(dllexport) type
#    endif
#  else
#    warning case 2
#    define MODULE2API(type) __declspec(dllimport) type
#  endif
#endif
#ifndef MODULE2API
#  warning case 3
#  define MODULE2API(type) type
#endif

MODULE2API(extern int) n_operations;
MODULE2API(void) operation_occurred ();

