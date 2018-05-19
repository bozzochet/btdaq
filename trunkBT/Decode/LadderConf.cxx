#include <sstream>
#include <string>

#include "Event.hh"
#include "LadderConf.hh"

LadderConf::LadderConf(){};
LadderConf::~LadderConf(){};

void LadderConf::Init(TString filename, bool DEBUG){

  int const dimline=255;
  char line[dimline];
  double dummy;
  int dummyint;
  int jinfnum=0;
  int tdrnum=0;

  FILE* ft = fopen(filename.Data(), "r");
  if( ft == NULL ){
    printf("Error: cannot open %s \n", filename.Data());
    return;
  } else {

    while(1){
      if (fgets(line, dimline, ft)!=NULL) {
        if (*line == '#'){  /* ignore comment line */
          continue;
        } else {
          LadderParams* params = new LadderParams;

          sscanf(line, "%d\t%d\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d", &jinfnum, &tdrnum, &dummy, &dummy, &dummy, &dummy, &dummyint, &dummyint, &dummyint);
          if( jinfnum<NJINF && tdrnum<NTDRS ){
            sscanf(
              line, "%d\t%d\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d",
              &jinfnum,
              &tdrnum,
              &params->_spitch,
              &params->_kpitch,
              &params->_sreso,
              &params->_kreso,
              (int*) &params->_kmultiflip,
              (int*) &params->_smirror,
              (int*) &params->_kmirror
            );
            params->_HwId = 100*jinfnum + tdrnum;
            // params->Dump();
            _ladders.insert( std::pair<int, LadderParams*>(params->_HwId, params) );
            // printf("%lu\n", _ladders.size());
          } else {
            printf("Wrong JINF/TDR (%d, %d): maximum is (%d,%d)\n", jinfnum, tdrnum, NJINF, NTDRS);
          }
        }
      } else {
        printf(" closing ladderconf file \n");
        fclose(ft);
        break;
      }
    }
  }

  return;

}

bool LadderConf::GetMultiplicityFlip(int jinfnum, int tdrnum){

  int HwId = 100*jinfnum + tdrnum;

  if(_ladders[HwId]) return _ladders[HwId]->_kmultiflip;
  else return false;
}


bool LadderConf::GetStripMirroring(int jinfnum, int tdrnum, int side){

  int HwId = 100*jinfnum + tdrnum;

  if( side == 0 ){ if(_ladders[HwId]) return _ladders[HwId]->_smirror; }
  else if( side == 1 ){ if(_ladders[HwId]) return _ladders[HwId]->_kmirror; }
  
  return false;
}


double LadderConf::GetPitch(int jinfnum, int tdrnum, int side){

  int HwId = 100*jinfnum + tdrnum;

  if( side == 0 ){ if(_ladders[HwId]) return _ladders[HwId]->_spitch; }
  else if( side == 1 ){ if(_ladders[HwId]) return _ladders[HwId]->_kpitch; }
  
  return -999.9;
}


double LadderConf::GetResolution(int jinfnum, int tdrnum, int side){

  int HwId = 100*jinfnum + tdrnum;

  if( side == 0 ){ if(_ladders[HwId]) return _ladders[HwId]->_sreso; }
  else if( side == 1 ){ if(_ladders[HwId]) return _ladders[HwId]->_kreso; }

  return -999.9;
}


void LadderConf::PrintLadderParams(int jinfnum, int tdrnum){

  int HwId = 100*jinfnum + tdrnum;

  if(_ladders[HwId]) _ladders[HwId]->Dump();

}
