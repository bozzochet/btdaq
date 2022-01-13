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

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::Print() {
  printf("---------------------------------------------\n");
  printf("The header says:\n");
  printf("Run: %d Date: %s\n", Run, date);
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

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::FindPos(int tdrnum) {

  // Print();

  for (int ii = 0; ii < GetNTdrs(); ii++)
    if (tdrMap[ii].first == tdrnum)
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

  // for (int ii=0;ii<NTDRS;ii++) {
  //   tdrMap[ii]=_TdrMap[ii];
  // }
  memcpy(tdrMap, _TdrMap, NTDRS * sizeof(tdrMap[0]));

  return;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetTdrNum(int tdrpos) {
  if (tdrpos < NTDRS) {
    return tdrMap[tdrpos].first;
  }
  return -1;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetTdrType(int tdrpos) {
  if (tdrpos < NTDRS) {
    return tdrMap[tdrpos].second;
  }
  return -1;
}
