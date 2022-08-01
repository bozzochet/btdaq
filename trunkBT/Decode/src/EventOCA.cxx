#include "GenericEvent.hpp"

using EventOCA = GenericEvent<1, 24, 64, 5, 10, 0>;
using EventFOOT = GenericEvent<1, 24, 64, 5, 10, 0>;

ClassImp(EventOCA);
ClassImp(EventFOOT);

// we can keep everything hardcoded since we define specialized constructors for each flavor
// EventOCA and EventFOOT are aliases to the same type, we shouldn't define the same constructor twice
template <> EventOCA::GenericEvent() {
  Cls = new TClonesArray("Cluster",
                         GetNJINF() * GetNTDRS()); // if more than NJINFS*NTDRS anyhow the array will be expanded
  Cls->SetOwner();

  // MD: this must be fixed, cannot be hardcoded
  if (ladderconfnotread)
    ReadLadderConf("ladderconf_OCA.dat");

  // FIXME: I don't have a smart way to differentiate OCA and FOOT now, we'll think about it later [VF]
  if (gaincorrectionnotread) {
    ReadGainCorrection("gaincorrection_OCA.dat");
    // ReadGainCorrection("gaincorrection_FOOT.dat");
  }

  ClearTrack();
}
