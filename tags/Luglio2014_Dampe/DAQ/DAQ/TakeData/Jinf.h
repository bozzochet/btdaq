#ifndef JINF_H
#define JINF_H
#include "amswcom.h"

#include "JinjSlave.h"

struct Calibration{
    double ped[1024];
    double sigr[1024];
    double sig[1024];
    int sta[1024];
    short int occupancy[1024];
    short int CNrms[1024];
    short int CNmean[1024];  
    double S_lowthres;
    double S_highthres;
    double K_lowthres;  
    double K_highthres;
    double sigrawthres;
    int calnum;
    int calstatus;
};

class Jinf : public JinjSlave{

public:
  Jinf(char* name,char* conf_file,int address,AMSWcom* node);
  ~Jinf();
  int Init();
  int ReadSlaveMask();
  int WriteSlaveMask();
  int SetMode(int modein);
  int SetModeTdrs(int modein=-1);
  int GetEventNumber();
  char* PrintAllEventNumber(int log=1,int Jinfnum=0);
  int EventReset();
  int ShowConnect(SlaveMask mask);
  int SetDelay();
  int OverWriteDelay(u_short del);
  int GenAddress(int tdrnum);
  int Calibrate(AMSWcom* node, int type, int par);
  int Calibrate(AMSWcom* node, int type);
  int WriteCalibPar(AMSWcom* node);
  int SaveCalibrations(int run_number,int Jinfnum);

 private:
  int CalibrateOneTDR(AMSWcom* node, int type, int par, int tdrnum);
  int CalibrateOneTDR(AMSWcom* node, int type, int tdrnum);
  int SaveCalibrationOneTDR(unsigned int tdr_number, char *calfileprefix);
  int WriteCalibParOneTDR(AMSWcom* node, int tdrnum);
  int DecodeOneCal(Calibration &cal, int offset);

};

#endif
