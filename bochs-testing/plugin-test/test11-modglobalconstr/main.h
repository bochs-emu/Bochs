#if defined(WIN32) || defined(__CYGWIN__)
#  ifdef MAIN_DLL_EXPORT
#    ifdef DLL_EXPORT
#      warning I will export DLL symbols for MODULE1
#      define MAINAPI(type) __declspec(dllexport) type
#    endif
#  else
#    warning I will import DLL symbols for MODULE1
#    define MAINAPI(type) __declspec(dllimport) type
#  endif
#endif
#ifndef MAINAPI
#  warning No DLL import/export is needed
#  define MAINAPI(type) type
#endif

MAINAPI(extern const char *) version_string;
MAINAPI(extern int) register_module (const char *name);
