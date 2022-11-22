#include "GenericEvent.hpp"

using EventAMS = GenericEvent<1, 24, 64, 3, 16, 10>;//MD: the "16" I think should be "10" and the "10" should be "6"
using RHClassAMS = RHClass<EventAMS::GetNJINF(), EventAMS::GetNTDRS()>;

// we can keep everything hardcoded since we define specialized constructors for each flavor
template <> EventAMS::GenericEvent(const char* ladderconf, const char* gaincorr) {
  _eventkind = 1; // 0: generic, 1: AMS, 2: OCA, 3: FOOT, 4: AMSL0

  Cls = new TClonesArray("Cluster",
                         GetNJINF() * GetNTDRS()); // if more than NJINFS*NTDRS anyhow the array will be expanded
  Cls->SetOwner();

  // MD: this must be fixed, cannot be hardcoded
  if (ladderconfnotread)
    ReadLadderConf(ladderconf);

  if (gaincorrectionnotread) {
    ReadGainCorrection(gaincorr);
  }

  ClearTrack();
}
