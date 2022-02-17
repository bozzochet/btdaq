#include "GenericEvent.hpp"

using EventAMS = GenericEvent<1, 24, 64, 3, 16, 10>;
using RHClassAMS = RHClass<EventAMS::GetNJINF(), EventAMS::GetNTDRS()>;

ClassImp(RHClassAMS);
ClassImp(EventAMS);

// we can keep everything hardcoded since we define specialized constructors for each flavor
template <> EventAMS::GenericEvent() {
  Cls = new TClonesArray("Cluster",
                         GetNJINF() * GetNTDRS()); // if more than NJINFS*NTDRS anyhow the array will be expanded
  Cls->SetOwner();

  // MD: this must be fixed, cannot be hardcoded
  if (ladderconfnotread)
    ReadLadderConf("ladderconf.dat");

  if (gaincorrectionnotread) {
    ReadGainCorrection("gaincorrection.dat");
  }

  ClearTrack();
}
