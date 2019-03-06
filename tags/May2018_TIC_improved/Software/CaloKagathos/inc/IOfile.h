#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#ifdef AMS
#include "ams_header.h"
#elif ADAMO
#include "data_vme.h"
#endif

#ifndef _IOFILE  
#define _IOFILE 

using namespace std;

struct dataCASIS
{
  unsigned short int trg_type;
  unsigned short int trg_time;
  int ev;
  unsigned short int data[NCASISCHIPS][NCASISCHANNELS];
  unsigned short int gain[NCASISCHIPS][NGAINWORDS];
  //float tempC[NSENSORS];
};

class IOfile{
 public:
  IOfile();
  ~IOfile();
  void OpenFile(const char * file, const char * option);
  void Reset(struct dataCASIS dat);

  int ReadRunHeader();
  int ReadEventHeader();  
  int ReadEvent();
  //int SensorMap();
#ifdef AMS
  struct glb_Header GetEventHeader();
#elif ADAMO
  struct VME_Header GetEventHeader();
#endif
  unsigned int GetEventNumber();
  unsigned short int Get00Trigger1();
  unsigned short int Get00Trigger0();
  unsigned short int GetLACTrigger();
  unsigned short int GetCCTrigger();
#if defined(SPS2015) || defined(SPS2016) || defined(SPS2017Aug) || defined(SPS2017Nov) || defined(TIC)
  unsigned short int GetCCTime();
#else  
  unsigned short int GetCasisTime();
#endif
  unsigned short int GetCalTag();
  int GetCalChip();
  int GetCalChannel();
  unsigned short int GetCalTrigger();
  unsigned short int GetCCUBEdata(int iCasis, int iCasisChannel);  
  unsigned short int GetCCGain(int iCasis, int iCasisChannel);
  unsigned short int GetCalData(int iCasis, int iCasisChannel);
  unsigned short int GetCalGain(int iCasis);
  bool CheckCalData(int iCasis, int iCasisChannel);
  //int GetSensor(int iSensor);
  //float GetTemp(int iSensor);

  void CloseFile();

 private:
#ifdef AMS
  struct glb_Header header;
#elif ADAMO
  struct VME_Header header;
#endif
  struct dataCASIS casis;
  std::ifstream in;
  //std::ifstream in_addr;
  int verbose;
  //int map[NSENSORS];
};

#endif

