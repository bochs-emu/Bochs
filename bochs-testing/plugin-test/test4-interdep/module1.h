#if defined(WIN32) || defined(__CYGWIN__)
#  ifdef MODULE1_DLL_EXPORT
#    ifdef DLL_EXPORT
#      warning I will export DLL symbols for MODULE1
#      define MODULE1API(type) __declspec(dllexport) type
#    endif
#  else
#    warning I will import DLL symbols for MODULE1
#    define MODULE1API(type) __declspec(dllimport) type
#  endif
#endif
#ifndef MODULE1API
#  warning No DLL import/export is needed
#  define MODULE1API(type) type
#endif

MODULE1API(extern const char *) module_name;
MODULE1API(extern int) operate (int a, int b);

