#include <stdio.h>
#include "main.h"
#define _buildsym(sym) module1_LTX_##sym
#include "modules.h"

class ModuleGadget* module_init ()
{
  printf ("module1 init for main version %s\n", version_string);
  register_module ("module1");
  return new ModuleGadget ("FancyCellPhone", "Caller ID, Video Conferencing");
}

int operate (int a, int b)
{
  return a + b;
}

//////// ModuleGadget class methods
ModuleGadget::ModuleGadget(const char* name, const char* features)
{
  this->name = name;
  this->features = features;
}

const char* ModuleGadget::getName ()
{
  return name;
}

const char* ModuleGadget::getFeatures ()
{
  return features;
}
