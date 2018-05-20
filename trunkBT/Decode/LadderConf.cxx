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
            int n = sscanf(
			   line, "%d\t%d\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d",
			   &jinfnum,
			   &tdrnum,
			   &params->_spitch,
			   &params->_kpitch,
			   &params->_sreso,
			   &params->_kreso,
			   (int*) &params->_kmultiflip,
			   (int*) &params->_smirror,
			   (int*) &params->_kmirror,
			   (int*) &params->_bondtype
			   );
	    if (n < params->_nelements) {
	      printf("JINF=%d, TDR=%02d: %d elements found, while %d expected: ", jinfnum, tdrnum, n, params->_nelements);
	      if (params->_nelements - n == 1) {
		printf("the difference is 1, so is the version 0 of ladderconf, setting the bonding type to default...\n");
		params->_bondtype = 0;
	      }
	      else printf("the difference is %d, SO THIS IS WRONG. PLEASE CHECK THE %s file! **************\n", params->_nelements, filename.Data());
	    }
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

  if(IsTDRConfigured(jinfnum, tdrnum)) return _ladders[HwId]->_kmultiflip;
  else return false;
}


bool LadderConf::GetStripMirroring(int jinfnum, int tdrnum, int side){

  int HwId = 100*jinfnum + tdrnum;

  if( side == 0 ){ if(IsTDRConfigured(jinfnum, tdrnum)) return _ladders[HwId]->_smirror; }
  else if( side == 1 ){ if(IsTDRConfigured(jinfnum, tdrnum)) return _ladders[HwId]->_kmirror; }
  
  return false;
}


double LadderConf::GetPitch(int jinfnum, int tdrnum, int side){

  int HwId = 100*jinfnum + tdrnum;
  
  if( side == 0 ){ if(IsTDRConfigured(jinfnum, tdrnum)) return _ladders[HwId]->_spitch; }
  else if( side == 1 ){ if(IsTDRConfigured(jinfnum, tdrnum)) return _ladders[HwId]->_kpitch; }
  
  return -999.9;
}


double LadderConf::GetResolution(int jinfnum, int tdrnum, int side){

  int HwId = 100*jinfnum + tdrnum;
  
  if( side == 0 ){ if(IsTDRConfigured(jinfnum, tdrnum)) return _ladders[HwId]->_sreso; }
  else if( side == 1 ){ if(IsTDRConfigured(jinfnum, tdrnum)) return _ladders[HwId]->_kreso; }

  return -999.9;
}

int LadderConf::GetBondingType(int jinfnum, int tdrnum){

  int HwId = 100*jinfnum + tdrnum;
  
  if(IsTDRConfigured(jinfnum, tdrnum)) return _ladders[HwId]->_bondtype;

  return 0;
}

void LadderConf::PrintLadderParams(int jinfnum, int tdrnum){

  int HwId = 100*jinfnum + tdrnum;
  
  if(IsTDRConfigured(jinfnum, tdrnum)) _ladders[HwId]->Dump();

  return;
}

bool LadderConf::IsTDRConfigured(int jinfnum, int tdrnum){

  int HwId = 100*jinfnum + tdrnum;
  
  return IsTDRConfigured(HwId);
}

bool LadderConf::IsTDRConfigured(int HwId){

  if (_ladders[HwId]) return true;

  return false;
}
