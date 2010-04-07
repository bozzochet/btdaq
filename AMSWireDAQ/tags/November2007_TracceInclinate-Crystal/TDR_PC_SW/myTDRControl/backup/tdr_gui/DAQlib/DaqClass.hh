#ifndef DaqClass_hh
#define DaqClass_hh

#include "CommClass.hh"
#include "GonClass.hh"


class DaqClass: public CommClass {

public:
  char DaqName[80];
    int ntot_tdrs;
  int tdrnum[25];
  int jinfnum[25];
  int nevents[25];
  int ntot_jinf;
  int nevents_Jinf[25];
  int nevents_Jinj;
  int nevents_count;

  DaqClass(char *name="PGDAQ0",char* address="pgdaq0",int port=6666);
  virtual ~DaqClass(){};

  // !Starts the daq for the acquisition it waits for READY
  int StartDaq(int run,int maxev, GonClass* gon,float timeout=100);
  // !Stop the Daq program
  int StopDaq(float timeout=100);
  //! Kill the Daq program
  int KillDaq(float timeout=10);
  //! Force a standalone calibration
  int Calibrate(int run,int lad,float timeout=20);
  //! Do raw Calibration (it is a run)
  int CalibRaw(int run,float timeout=100);
  //! Initialize the daq
  int Init(float timeout=100);
  //! Tests if daq program is running
  int IsDaqRunning();
  //! Read the num of proc events in the last run
  int GetProcEvents();
};

#endif
