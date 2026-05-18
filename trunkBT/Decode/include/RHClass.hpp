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

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::PrintHeader() {
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

  return;
}

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::PrintFooter() {
  printf("# Jinf = %d\n", nJinf);
  for (int ii = 0; ii < nJinf; ii++)
    printf("Jinf:       Map pos=%2d Jinf num=%2d\n", ii, JinfMap[ii]);

  printf("# TDR RAW = %d\n", ntdrRaw);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (tdrMap[ii].second == 0) {
      printf("TDR RAW:    Map pos=%2d Jinf num=%2d Tdr num=%2d TdrId=%3d\n", ii, GetJinfNum_byID(tdrMap[ii].first),
             GetTdrNum_byID(tdrMap[ii].first), tdrMap[ii].first);
    }
  }

  printf("# TDR CMP = %d\n", ntdrCmp);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (tdrMap[ii].second == 1) {
      printf("TDR CMP:    Map pos=%2d Jinf num=%2d Tdr num=%2d TdrId=%3d\n", ii, GetJinfNum_byID(tdrMap[ii].first),
             GetTdrNum_byID(tdrMap[ii].first), tdrMap[ii].first);
    }
  }
  printf("# TDR    = %d\n", ntdrCmp + ntdrRaw);
  for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
    if (!(tdrMap[ii].second == 0 || tdrMap[ii].second == 1)) {
      printf("TDR %2d???: Map pos=%2d Jinf num=%2d Tdr num=%2d TdrId=%3d\n", tdrMap[ii].second, ii,
             GetJinfNum_byID(tdrMap[ii].first), GetTdrNum_byID(tdrMap[ii].first), tdrMap[ii].first);
    }
  }

  printf("---------------------------------------------\n");
  return;
}

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::Print() {
  PrintHeader();

  PrintFooter();

  return;
}

void RHClassOCA::Print() {
  PrintHeader();

  if (!gitSHA.empty()) {
    printf("Software git commit SHA: %s\n", gitSHA.c_str());
    printf("Data model version: %d, %d, %d\n", dataVersion.major, dataVersion.minor, dataVersion.patch);
  }

  PrintFooter();

  return;
}

void RHClassHEF::Print() {
  PrintHeader();

  if (!gitSHA.empty()) {
    printf("Software git commit SHA: %s\n", gitSHA.c_str());
    printf("Data model version: %d, %d, %d\n", dataVersion.major, dataVersion.minor, dataVersion.patch);
  }

  PrintFooter();

  return;
}

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::SetJinfMap(int *_JinfMap) {

  // for (int ii=0;ii<NJINF;ii++) {
  //   JinfMap[ii]=_JinfMap[ii];
  // }
  memcpy(JinfMap, _JinfMap, NJINF * sizeof(JinfMap[0]));

  return;
}

template <size_t NJINF, size_t NTDRS> void RHClass<NJINF, NTDRS>::SetTdrMap(laddernumtype *_TdrMap) {

  //  for (size_t ii = 0; ii < NJINF * NTDRS; ++ii) {
  //    tdrMap[ii] = _TdrMap[ii];
  //  }
  memcpy(tdrMap, _TdrMap, NJINF * NTDRS * sizeof(tdrMap[0]));

  return;
}

template <size_t NJINF, size_t NTDRS> TDR RHClass<NJINF, NTDRS>::GetTDR_bynums(int tdrnum, int jinfnum) {

  TDR tdr;

  for (int ii = 0; ii < GetNTdrs(); ii++) {
    if (tdrMap[ii].first == ComputeTdrId(tdrnum, jinfnum)) {
      tdr.setglobindex(ii);
      tdr.setJINFnum(jinfnum);
      tdr.setTDRnum(tdrnum);
      tdr.setID(ComputeTdrId(tdrnum, jinfnum));
      return tdr;
    }
  }

  printf("GetTDR_bynums(%d, %d): not found!\n", tdrnum, jinfnum);

  return tdr;
}

template <size_t NJINF, size_t NTDRS> TDR RHClass<NJINF, NTDRS>::GetTDR_byglobindex(int tdrglobindex) {

  TDR tdr;

  if (tdrglobindex < GetNTdrs()) {
    tdr.setglobindex(tdrglobindex);
    tdr.setJINFnum(GetJinfNum_byglobindex(tdrglobindex));
    tdr.setTDRnum(GetTdrNum_byglobindex(tdrglobindex));
    tdr.setID(tdrMap[tdrglobindex].first);
    return tdr;
  } else {
    printf("GetTDR_byglobindex(%d): not found!\n", tdrglobindex);
  }

  return tdr;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetTdrNum_byID(int tdrid) {
  return (int)(tdrid % 100);
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetJinfNum_byID(int tdrid) {
  return (int)(tdrid / 100);
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetTdrNum_byglobindex(int tdrglobindex) {
  if (tdrglobindex < GetNTdrs()) {
    return (int)((tdrMap[tdrglobindex].first) % 100);
  } else {
    printf("GetTdrNum_byindex(%d): not found!\n", tdrglobindex);
  }
  return -1;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetJinfNum_byglobindex(int tdrglobindex) {
  if (tdrglobindex < GetNTdrs()) {
    return (int)((tdrMap[tdrglobindex].first) / 100);
  } else {
    printf("GetJinfNum_byglobindex(%d): not found!\n", tdrglobindex);
  }
  return -1;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetTdrGlobIndex_bynums(int tdrnum, int jinfnum) {

  for (int ii = 0; ii < GetNTdrs(); ii++)
    if (tdrMap[ii].first == ComputeTdrId(tdrnum, jinfnum))
      return ii;

  printf("GetTdrGlobIndex_bynums(%d, %d): not found!\n", tdrnum, jinfnum);

  return -1;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetJinfIndex_bynum(int jinfnum) {

  // Print();

  // JinfMap[ii] != ii
  for (int ii = 0; ii < GetNJinfs(); ii++) {
    if (JinfMap[ii] == jinfnum)
      return ii;
  }

  printf("GetJinfIndex_bynum(%d): not found!\n", jinfnum);

  return -1;
}

template <size_t NJINF, size_t NTDRS> int RHClass<NJINF, NTDRS>::GetJinfNum_byindex(int jinfindex) {
  return JinfMap[jinfindex];
}
