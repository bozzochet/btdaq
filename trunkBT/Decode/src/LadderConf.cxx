#include <sstream>
#include <string>

#include "EventUtils.hh"
#include "LadderConf.hh"

LadderConf* LadderConf::_head = nullptr;
LadderParamsMap* LadderConf::_ladders = nullptr;
size_t LadderConf::NJINF = -999;
size_t LadderConf::NTDRS = -999;

LadderParams::LadderParams(){

  _spitch = 0.110;
  _kpitch = 0.208;
  _sreso = 0.10;
  _kreso = 0.30;
  _kmultiflip = false;
  _smirror = false;
  _kmirror = false;
  _bondtype = 0;
  _shithresh = 3.5;//change to ones passed through command line
  _khithresh = 3.5;//change to ones passed through command line
  _slothresh = 1.0;//change to ones passed through command line
  _klothresh = 1.0;//change to ones passed through command line
  _sideswap = 0;

}

LadderConf::~LadderConf(){};

LadderConf* LadderConf::Instance() { 
  if (_head==nullptr) {
    _head = new LadderConf();
  }
  return _head;
}

LadderConf::LadderConf(){
  //  printf("LadderConf::default constructor\n");

  if (_head==nullptr) {
    _ladders = new LadderParamsMap();
  }
  else {
    printf("You should not call this once the singleton has been created once!\n");
  }

  return;
};

LadderConf::LadderConf(LadderParamsMap* lpm){
  //  printf("LadderConf::alternative constructor\n");

  if (_head==nullptr) {
    _ladders = lpm;
    _head = this;
  }
  else {
    printf("You should not call this once the singleton has been created once!\n");
  }
  
  return;
}

void LadderConf::Init(TString filename, bool DEBUG) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  int const dimline = 255;
  char line[dimline];
  double dummy;
  int dummyint;
  int jinfnum = 0;
  int tdrnum = 0;

  //---- creating params and setting to defaults ---
  LadderParams *params[NJINF][NTDRS];
  for (size_t jj = 0; jj < NJINF; jj++) {
    for (size_t tt = 0; tt < NTDRS; tt++) {
      params[jj][tt] = new LadderParams();
      params[jj][tt]->_JinfId = jj;
      params[jj][tt]->_TdrId = tt;
    }
  }
  //------------------------------------------------

  FILE *ft = fopen(filename.Data(), "r");
  if (ft == NULL) {
    printf("Error: cannot open %s \n", filename.Data());
    return;
  } else {
    while (1) {
      if (fgets(line, dimline, ft) != NULL) {
        if (*line == '#') { /* ignore comment line */
          continue;
        } else {

          int firstn = sscanf(line, "%d\t%d\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%lf\t%d", &jinfnum,
                              &tdrnum, &dummy, &dummy, &dummy, &dummy, &dummyint, &dummyint, &dummyint, &dummyint,
                              &dummy, &dummy, &dummy, &dummy, &dummyint);
          //	  printf("%d) %d %d\n", firstn, jinfnum, tdrnum);
          //	  printf("NJINF = %zu, NTDRS = %zu\n", NJINF, NTDRS);
          auto jj = jinfnum;
          auto tt = tdrnum;
          if (static_cast<size_t>(jinfnum) < NJINF && static_cast<size_t>(tdrnum) < NTDRS) {
            int n = sscanf(line, "%d\t%d\t%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%lf\t%d", &jinfnum,
                           &tdrnum, &params[jj][tt]->_spitch, &params[jj][tt]->_kpitch, &params[jj][tt]->_sreso,
                           &params[jj][tt]->_kreso, (int *)&params[jj][tt]->_kmultiflip,
                           (int *)&params[jj][tt]->_smirror, (int *)&params[jj][tt]->_kmirror,
                           (int *)&params[jj][tt]->_bondtype, &params[jj][tt]->_shithresh, &params[jj][tt]->_slothresh,
                           &params[jj][tt]->_khithresh, &params[jj][tt]->_klothresh, (int *)&params[jj][tt]->_sideswap);
            // printf("%d) %d %d --> %f %f %f %f\n", n, jinfnum, tdrnum, params[jj][tt]->_shithresh,
            //        params[jj][tt]->_slothresh, params[jj][tt]->_khithresh, params[jj][tt]->_klothresh);
            if (n == -1) {
              printf("**** There's an empty line that is not a comment (#): please remove\n");
              continue;
            } else if (n < params[jj][tt]->_nelements) {
              printf("JINF=%d, TDR=%02d: %d elements found, while %d expected: ", jinfnum, tdrnum, n,
                     params[jj][tt]->_nelements);
              if (params[jj][tt]->_nelements - n == 5) {
                printf("** the difference is 5, so is the version 0 of ladderconf, leaving the bonding type, "
                       "thresholds and sideswap to default...\n");
              } else if (params[jj][tt]->_nelements - n == 4) {
                printf("** the difference is 4, so is the version 1 of ladderconf, leaving thresholds and "
                       "sideswap to default...\n");
              } else if (params[jj][tt]->_nelements - n == 1) {
                printf("** the difference is 1, so is the version 2 of ladderconf, leaving sideswap to default...\n");
              } else
                printf("**** the difference is %d, SO THIS IS WRONG. PLEASE CHECK THE %s file! **************\n",
                       params[jj][tt]->_nelements - n, filename.Data());
            }
            params[jj][tt]->_JinfId = jinfnum;
            params[jj][tt]->_TdrId = tdrnum;
            //	    params[jj][tt]->Dump();
            _ladders->GetMap().insert(std::pair<std::pair<int, int>, LadderParams *>(
                std::make_pair(params[jj][tt]->_TdrId, params[jj][tt]->_JinfId), params[jj][tt]));
            //	    printf("%lu\n", _ladders->GetMap().size());
          } else {
            printf("**** Wrong JINF/TDR (%d, %d): maximum is (%ld,%ld)\n", jinfnum, tdrnum, NJINF, NTDRS);
          }
        }
      } else {
        printf(" closing ladderconf file \n");
        fclose(ft);
        break;
      }
    }
  }

  //  printf("NJINF=%zu, NTDRS=%zu\n", NJINF, NTDRS);
  for (size_t jj = 0; jj < NJINF; jj++) {
    for (size_t tt = 0; tt < NTDRS; tt++) {
      if (!IsTDRConfigured(jj, tt)) { // setting default to not configured ones
        params[jj][tt]->_JinfId = jj;
        params[jj][tt]->_TdrId = tt;
        //        params[jj][tt]->Dump();
        _ladders->GetMap().insert(std::pair<std::pair<int, int>, LadderParams *>(
            std::make_pair(params[jj][tt]->_TdrId, params[jj][tt]->_JinfId), params[jj][tt]));
        //	    printf("%lu\n", _ladders->GetMap().size());
      }
    }
  }

  return;
}

void LadderConf::Dump() {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  //  TObject::Dump();

  printf("LadderConf::Dump()\n");

  printf("map %p\n", &_ladders);
  printf("map.size() %d\n", (int)(_ladders->GetMap().size()));

  std::map<std::pair<int, int>, LadderParams *>::iterator it;

  for (it = _ladders->GetMap().begin(); it != _ladders->GetMap().end(); it++) {
    std::cout << it->first.first << ',' << it->first.second << ':' << std::endl;
    it->second->Dump();
  }

  return;
}

bool LadderConf::GetMultiplicityFlip(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum))
    return _ladders->GetMap()[HwId]->_kmultiflip;
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
    return false;
  }
}

bool LadderConf::GetStripMirroring(int jinfnum, int tdrnum, int side) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum)) {
    if (side == 0) {
      return _ladders->GetMap()[HwId]->_smirror;      
    } else if (side == 1) {
      return _ladders->GetMap()[HwId]->_kmirror;
    }
  }
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return false;
}

double LadderConf::GetPitch(int jinfnum, int tdrnum, int side) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum)) {
    if (side == 0) {
      return _ladders->GetMap()[HwId]->_spitch;
    } else if (side == 1) {
      return _ladders->GetMap()[HwId]->_kpitch;
    }
  }
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return -999.9;
}

double LadderConf::GetResolution(int jinfnum, int tdrnum, int side) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum)) {
    if (side == 0) {
      return _ladders->GetMap()[HwId]->_sreso;
    } else if (side == 1) {
      return _ladders->GetMap()[HwId]->_kreso;
    }
  }
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return -999.9;
}

int LadderConf::GetBondingType(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum))
    return _ladders->GetMap()[HwId]->_bondtype;
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return 0;
}

double LadderConf::GetSHiThreshold(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum))
    return _ladders->GetMap()[HwId]->_shithresh;
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return 0;
}

double LadderConf::GetKHiThreshold(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum))
    return _ladders->GetMap()[HwId]->_khithresh;
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return 0;
}

double LadderConf::GetSLoThreshold(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum))
    return _ladders->GetMap()[HwId]->_slothresh;
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return 0;
}

double LadderConf::GetKLoThreshold(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum))
    return _ladders->GetMap()[HwId]->_klothresh;
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return 0;
}

bool LadderConf::GetSideSwap(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  if (IsTDRConfigured(jinfnum, tdrnum))
    return _ladders->GetMap()[HwId]->_sideswap;
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return 0;
}

void LadderConf::PrintLadderParams(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);
  
  if (IsTDRConfigured(jinfnum, tdrnum))
    _ladders->GetMap()[HwId]->Dump();
  else {
    printf("Ladder %d %d not configured!\n", jinfnum, tdrnum);
  }

  return;
}

bool LadderConf::IsTDRConfigured(int jinfnum, int tdrnum) {

  if (!_head)
    printf("Please call a \"constructor\" before...\n");

  std::pair HwId = std::make_pair(tdrnum, jinfnum);

  return _ladders->GetMap().find(HwId) != end(_ladders->GetMap());
}
