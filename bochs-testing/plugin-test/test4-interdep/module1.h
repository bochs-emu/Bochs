#if defined(WIN32) || defined(__CYGWIN__)
#  ifdef MODULE1_DLL_EXPORT
#    ifdef DLL_EXPORT
#      warning case 1
#      define MODULE1API(type) __declspec(dllexport) type
#    endif
#  else
#    warning case 2
#    define MODULE1API(type) __declspec(dllimport) type
#  endif
#endif
#ifndef MODULE1API
#  warning case 3
#  define MODULE1API(type) type
#endif

MODULE1API(extern const char *) module_name;
MODULE1API(extern int) operate (int a, int b);

