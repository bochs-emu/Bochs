extern "C" {
  // this prevents C++ name mangling
  class ModuleGadget * module_init ();
};

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
