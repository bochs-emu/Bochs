#include <stdio.h>
#include "main.h"
#define _buildsym(sym) module1_LTX_##sym
#include "modules.h"

class CellPhone : public DeviceInterface {
  int id;
  const char *name;
  const char *features;
public:
  CellPhone(const char* name, const char* features);
  virtual const char* getName ();
  virtual const char* getFeatures ();
  virtual void print(FILE *);
};

class DeviceInterface* module_init ()
{
  printf ("module1 init for main version %s\n", version_string);
  register_module ("module1");
  return new CellPhone ("BochsCellPhone", "Caller ID, Video Conferencing");
}

int operate (int a, int b)
{
  return a + b;
}

//////// CellPhone class methods
CellPhone::CellPhone(const char* name, const char* features)
{
  this->name = name;
  this->features = features;
}

const char* CellPhone::getName ()
{
  return name;
}

const char* CellPhone::getFeatures ()
{
  return features;
}

void CellPhone::print (FILE *fp)
{
  fprintf (fp, "[CellPhone name='%s', features='%s']", name, features);
}
