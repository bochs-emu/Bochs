// This file is included by each module to rename all of its symbols, in order
// to avoid duplicate symbols in different modules.  From the libtool docs:
// "Although some platforms support having the same symbols defined more than
// once it is generally not portable."

#define module_init _buildsym(module_init)
#define operate _buildsym(operate)

// Declare function prototypes in an extern "C" block so that C++ will not
// mangle their names (any more than we already have).
extern "C" {
  void module_init();
  int operate (int a, int b);
};

// Since the renaming and extern "C" stuff declaration is done in the
// header file, the actual code remains readable.
