#include "GenericEvent.hpp"

using EventAMSL0 = GenericEvent<1, 2, 64, 8, 16, 0>;
using RHClassAMSL0 = RHClass<EventAMSL0::GetNJINF(), EventAMSL0::GetNTDRS()>;

// we can keep everything hardcoded since we define specialized constructors for each flavor
template <> EventAMSL0::GenericEvent(const char* ladderconf, const char* gaincorr) {
  _eventkind = 4; // 0: generic, 1: AMS, 2: OCA, 3: FOOT, 4: AMSL0

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
