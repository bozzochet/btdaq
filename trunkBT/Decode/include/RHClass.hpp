#include "RHClass.hh"

// template <size_t NJINF, size_t NTDRS> RHClass<NJINF, NTDRS>::RHClass() {
//   /* Viviana added (or left) these: do we really need?
//   for (int ii=0;ii<NTDRS;ii++)
//     for (int jj=0;jj<NVAS;jj++){
//       CNMean[ii][jj]=0.;
//       CNSigma[ii][jj]=0.;
//     }
//   }
//   */
// }

template <size_t NJINF, size_t NTDRS> std::string RHClass<NJINF, NTDRS>::to_string(RunType runtype) {
  switch (runtype) {
  case RunType::Unknown:
    return "Unknown";
  case RunType::SC:
    return "SC";
  case RunType::CAL:
    return "CAL";
  case RunType::HK:
    return "HK";
  case RunType::TC:
    return "TC";
  }

  return {};
}

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::Print() {
  printf("---------------------------------------------\n");
  printf("The header says:\n");
  printf("Run: %d", Run);
  if (!date.empty()) {
    printf(" Date: %s\n", date.c_str());
  } else {
    printf("\n");
  }

  if (runType != RunType::Unknown)
    printf("Run type: %s\n", to_string(runType).c_str());

  if (runTag != 0)
    printf("Run tag: 0x%X\n", runTag);

  if (unixTime > 0) {
    printf("UnixTime: %d\n", unixTime);
  }

  if (!gitSHA.empty()) {
    printf("Software git commit SHA: %s\n", gitSHA.c_str());
    printf("Data model version: %d, %d, %d\n", dataVersion.major, dataVersion.minor, dataVersion.patch);
  }

  printf("# Jinf = %d\n", nJinf);
  for (int ii = 0; ii < nJinf; ii++)
    printf("Jinf Map pos: %d Jinf num: %d\n", ii, JinfMap[ii]);

  printf("# TDR RAW = %d\n", ntdrRaw);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (tdrMap[ii].second == 0) {
      printf("TDR RAW:    Map pos: %d tdrnum: %d\n", ii, tdrMap[ii].first);
    }
  }

  printf("# TDR CMP = %d\n", ntdrCmp);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (tdrMap[ii].second == 1) {
      printf("TDR CMP:    Map pos: %d tdrnum: %d\n", ii, tdrMap[ii].first);
    }
  }
  printf("# TDR    = %d\n", ntdrCmp + ntdrRaw);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (!(tdrMap[ii].second == 0 || tdrMap[ii].second == 1)) {
      printf("TDR %2d???: Map pos: %d tdrnum: %d\n", tdrMap[ii].second, ii, tdrMap[ii].first);
    }
  }

  printf("---------------------------------------------\n");
  return;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::FindPos(int tdrnum, int jinfnum) {

  // Print();

  for (int ii = 0; ii < GetNTdrs(); ii++)
    if (tdrMap[ii].first == ComputeTdrNum(tdrnum, jinfnum))
      return ii;

  return -1;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::FindJinfPos(int jinfnum) {

  // Print();

  for (int ii = 0; ii < GetNJinfs(); ii++)
    if (JinfMap[ii] == jinfnum)
      return ii;

  return -1;
}

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::SetJinfMap(int *_JinfMap) {

  // for (int ii=0;ii<NJINF;ii++) {
  //   JinfMap[ii]=_JinfMap[ii];
  // }
  memcpy(JinfMap, _JinfMap, NJINF * sizeof(JinfMap[0]));

  return;
}

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::SetTdrMap(laddernumtype *_TdrMap) {

  for (size_t ii = 0; ii < NJINF * NTDRS; ++ii) {
    tdrMap[ii] = _TdrMap[ii];
  }
  //    memcpy(tdrMap, _TdrMap, NJINF * NTDRS * sizeof(tdrMap[0]));

  return;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetTdrNum(size_t tdrpos) {
  if (tdrpos < NJINF * NTDRS) {
    return (tdrMap[tdrpos].first) % 100;
  }
  return -1;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetTdrType(size_t tdrpos) {
  if (tdrpos < NJINF * NTDRS) {
    return tdrMap[tdrpos].second;
  }
  return -1;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetJinfNum(size_t tdrpos) {
  if (tdrpos < NJINF * NTDRS) {
    return (int)(tdrMap[tdrpos].first / 100);
  }
  return -1;
}
