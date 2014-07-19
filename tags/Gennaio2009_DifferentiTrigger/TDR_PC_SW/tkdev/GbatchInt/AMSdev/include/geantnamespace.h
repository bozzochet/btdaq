//  $Id: geantnamespace.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __GEANTNAMESPACE__
#define __GEANTNAMESPACE__
#ifdef __G4AMS__

namespace g4ams{
  void G4INIT();
  void G4LAST();
  void G4RUN();
}
#include <CLHEP/Vector/Rotation.h>
typedef CLHEP::HepRotation  amsg4rm;
#endif
namespace gams{
  void UGINIT(int argc, char **argv );
  void UGLAST(const char *message=0);
}
class G4VPhysicalVolume;
typedef G4VPhysicalVolume amsg4pv;
class G4Material;
class G4LogicalVolume;
#endif
