#if defined(WIN32) || defined(__CYGWIN__)
#  ifdef MAIN_DLL_EXPORT
#    ifdef DLL_EXPORT
#      warning I will export DLL symbols for MAIN
#      define MAINAPI __declspec(dllexport) 
#    endif
#  else
#    warning I will import DLL symbols from MAIN
#    define MAINAPI __declspec(dllimport) 
#  endif
#endif
#ifndef MAINAPI
#  define MAINAPI
#endif

typedef void (*modload_func)(void);

MAINAPI extern const char * version_string;
MAINAPI extern int register_module (const char *name);

extern "C" {
  // this prevents C++ name mangling
  void module_init ();
};
