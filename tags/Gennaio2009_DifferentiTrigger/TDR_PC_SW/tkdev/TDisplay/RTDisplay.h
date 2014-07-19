#ifndef _TDISPLAY_
#define _TDISPLAY_
#include "Event.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TMarker.h"
#include "TObject.h"

class RTrackerDisplay {
private:
  int layer_pad[9];
  int counter;

  void DrawPlanes(int off=0);

  /// ROOT file pointer
  TFile *fFile;
  /// DST tree pointer
  TTree *fTree;

public:
  /// Self poiner
  static RTrackerDisplay* fSelfPtr;
  /// Event pointer
  Event   *fEvent;
  /// Planes canvas pointer
  TCanvas *fCanvas;

  /// Hits vector
  vector<TMarker*> Hits;

public:
  RTrackerDisplay();
 ~RTrackerDisplay();

  /// Open DST ROOT file
  int OpenFile(const char *filename);
  /// Close ROOT file
  int CloseFile();

  void PreparePlanes();

  int  DrawHits();
  void ClearHits();

  int DrawNext();
  int DrawPrevious();
  int DrawEvent(int ii);

  int GetEntries(void) {return (fTree) ? fTree->GetEntries() : 0; }

  /// Get an event at the entry, ii
  int GetEntry(int ii) { return (fTree) ? fTree->GetEntry(ii) : 0; }
};

#endif
