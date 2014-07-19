#include "RTConnectorMQD.h"
#include "RTGLViewer.h"
#include "RTDisplay3D.h"
#include "RTDisplay.h"
#include "TCanvas.h"
#include "mqdisplay.h"

RTConnectorMQD::RTConnectorMQD(MQDisplay *mqd)
{
  fMQD = mqd;
  if (!fViewer) return;

  RTConnector *conn = fViewer->GetConnector();
  conn->Connect("fileOpen()",   "RTConnector", this, "fileOpen()");
  conn->Connect("eventEntry()", "RTConnector", this, "eventEntry()");
}

void RTConnectorMQD::SetEntry(Int_t entry)
{
  if (fViewer && fViewer->GetEntry() != entry) fViewer->SetEntry(entry);
}

void RTConnectorMQD::SetMax(Int_t max)
{
  if (fViewer) fViewer->SetMax(max);
}

void RTConnectorMQD::DrawEvent()
{
  if (fViewer) fViewer->Draw();
}

void RTConnectorMQD::fileOpen()
{
  fMQD->fileOpen();
}

void RTConnectorMQD::eventEntry()
{
  fMQD->SetEntry(fViewer->GetEntry());
}
