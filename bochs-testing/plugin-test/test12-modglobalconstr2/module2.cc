#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "main.h"
#define _buildsym(sym) module2_LTX_##sym
#include "modules.h"

class GPS_Receiver : public DeviceInterface {
  int id;
  const char *name;
  char features[1024];
  const char *vendor;
  float resolution_meters;
public:
  GPS_Receiver(const char* name, const char* vendor, float res);
  void addFeature (const char *f);
  virtual const char* getName ();
  virtual const char* getFeatures ();
  virtual void print(FILE *);
  const char* getVendor ();
  float getResolutionMeters ();
};

GPS_Receiver *theReceiverPtr = new GPS_Receiver ("BochsStarGPS 4.0", "Bochs Project", 9.5);

int n_operations = 0;

class DeviceInterface* module_init ()
{
  printf ("module2 init for main version %s\n", version_string);
  register_module ("module2");
  if (theReceiverPtr == NULL) {
    printf ("VARIES{The global initializer for theReceiverPtr was not called.}\n");
    printf ("VARIES{Creating the object in module_init.}\n");
    theReceiverPtr = new GPS_Receiver ("BochsStarGPS 4.0", "Bochs Project", 9.5);
  } else {
    printf ("VARIES{The initializer was called, as it should be}\n");
    printf ("VARIES{No action needed}\n");
  }
  theReceiverPtr->addFeature ("High Accuracy");
  theReceiverPtr->addFeature ("Low Power");
  theReceiverPtr->addFeature ("Pentium Emulation");
  return theReceiverPtr;
}

int operate (int a, int b)
{
  return a - b;
}

//////// GPS_Receiver class methods

GPS_Receiver::GPS_Receiver(const char* name, const char* vendor, float res)
{
  this->name = name;
  this->features[0] = 0;
  this->vendor = vendor;
  this->resolution_meters = res;
}

const char* GPS_Receiver::getName ()
{
  return name;
}

const char* GPS_Receiver::getFeatures ()
{
  if (strlen (features) == 0) return "none";
  else return features;
}

const char* GPS_Receiver::getVendor ()
{
  return vendor;
}

float GPS_Receiver::getResolutionMeters ()
{
  return resolution_meters;
}

void GPS_Receiver::addFeature (const char *featureName)
{
  if (strlen(features) != 0)
    strcat (features, ", ");
  strcat (features, featureName);
}

void GPS_Receiver::print (FILE *fp)
{
  fprintf (fp, "[GPS_Receiver name='%s', vendor='%s', resolution_meters='%.4f', feature_list='%s']", name, vendor, resolution_meters, features);
}
