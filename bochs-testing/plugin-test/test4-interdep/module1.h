#if defined(WIN32) || defined(__CYGWIN__)
#  if defined(MODULE1_DLL_EXPORT) && defined(DLL_EXPORT)
#    warning case 1
#    define MODULE1API(type) __declspec(dllexport) type
#  endif
#  ifdef MODULE1_DLL_IMPORT
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

