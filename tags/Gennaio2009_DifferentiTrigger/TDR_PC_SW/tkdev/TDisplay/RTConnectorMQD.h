#ifndef _RTConnector_MQD_
#define _RTConnector_MQD_

#include "RTConnector.h"

class MQDisplay;

class RTConnectorMQD : public RTConnector
{
protected:
  MQDisplay *fMQD;

public:

  RTConnectorMQD(MQDisplay *);
 ~RTConnectorMQD() {}

  void SetEntry(Int_t entry);
  void SetMax  (Int_t max);
  void DrawEvent ();
  void fileOpen  ();
  void eventEntry();
};

#endif
