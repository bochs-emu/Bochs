#ifndef MODULE1API
#  if defined(_MSC_EXTENSIONS) && !defined(__BEOS__) && !defined(__CYGWIN__)
#    define MODULE1API(type) __declspec(dllimport) type __cdecl
#  else
#    define MODULE1API(type) type
#  endif
#endif

MODULE1API(extern const char *) module_name;
MODULE1API(extern int) operate (int a, int b);
