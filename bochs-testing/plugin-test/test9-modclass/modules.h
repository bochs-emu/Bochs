#ifdef _buildsym
// This section is included by each module to rename all of its symbols, 
// in order to avoid duplicate symbols in different modules.  From the 
// libtool docs: "Although some platforms support having the same symbols 
// defined more than once it is generally not portable."

#define module_init _buildsym(module_init)
#define operate _buildsym(operate)
extern "C" {
  // the extern "C" prevents C++ name mangling
  class ModuleGadget * module_init ();
  int operate (int a, int b);
};
#endif

typedef class ModuleGadget* (*modload_func)(void);

class ModuleGadget {
  int id;
  const char *name;
  const char *features;
public:
  ModuleGadget(const char* name, const char* features);
  virtual const char* getName ();
  virtual const char* getFeatures ();
};

