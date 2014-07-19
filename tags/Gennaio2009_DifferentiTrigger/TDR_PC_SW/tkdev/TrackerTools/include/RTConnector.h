#ifndef _TDCONNECOTR_
#define _TDCONNECOTR_

#include "TQObject.h"

class RTGLViewer;

class RTConnector : public TQObject
{
protected:
  static RTGLViewer *fViewer;

public:
  RTConnector(RTGLViewer *glv = 0) { if (glv) fViewer = glv; }
  virtual ~RTConnector() {}

  virtual void fileOpen  () {}
  virtual void eventEntry() {}

  ClassDef(RTConnector, 0)  // TDisplay interface
};

#endif
