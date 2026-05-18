#include "Event.hpp"

using EventHEF = Event<1, 24, 64, 7, 14, 0>;

// we can keep everything hardcoded since we define specialized constructors for each flavor
template <> EventHEF::Event(const char *ladderconf, const char *gaincorr) {
  _eventkind = 5; // 0: generic, 1: AMS, 2: HEF, 3: FOOT, 4: AMSL0, 5: HEF

  Cls = new TClonesArray("Cluster",
                         GetNJINF() * GetNTDRS()); // if more than NJINFS*NTDRS anyhow the array will be expanded
  Cls->SetOwner();

  // MD: this must be fixed, cannot be hardcoded
  if (ladderconfnotread) {
    ReadLadderConf(ladderconf);
    ladderconfnotread = false;
  }

  if (gaincorrectionnotread) {
    ReadGainCorrection(gaincorr);
    gaincorrectionnotread = false;
  }

  ClearTrack();
}
