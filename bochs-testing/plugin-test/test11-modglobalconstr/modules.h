#ifdef _buildsym
// This section is included by each module to rename all of its symbols, 
// in order to avoid duplicate symbols in different modules.  From the 
// libtool docs: "Although some platforms support having the same symbols 
// defined more than once it is generally not portable."

#define module_init _buildsym(module_init)
#define operate _buildsym(operate)
extern "C" {
  // the extern "C" prevents C++ name mangling
  class DeviceInterface * module_init ();
  int operate (int a, int b);
};
#endif

typedef class DeviceInterface* (*modload_func)(void);

class DeviceInterface {
public:
  virtual const char* getName () = 0;
  virtual const char* getFeatures () = 0;
  virtual void print(FILE *) = 0;
};
