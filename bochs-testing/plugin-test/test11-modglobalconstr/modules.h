extern "C" {
  // this prevents C++ name mangling
  class DeviceInterface * module_init ();
};

typedef class DeviceInterface* (*modload_func)(void);

class DeviceInterface {
public:
  virtual const char* getName () = 0;
  virtual const char* getFeatures () = 0;
  virtual void print(FILE *) = 0;
};
