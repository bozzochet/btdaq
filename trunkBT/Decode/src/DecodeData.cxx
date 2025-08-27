#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "DecodeData.hh"

#include "Event.hpp"
#include "RHClass.hh"

#include "TRandom3.h"
#include "TString.h"

using namespace std;

extern char progname[50];

inline bool file_exists(const std::string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

static TString stringtodump;

//=============================================================================================

int DecodeData::ReadFile(void *ptr, size_t size, size_t nitems, FILE *stream) {

  int ret = 0;
  ret = fread(ptr, size, nitems, stream);
  if (feof(stream)) {
    if (pri) {
      printf("\n");
      printf("End of File \n");
    }
    return -1;
  }
  if (ferror(stream)) {
    if (pri)
      printf("Error reading \n");
    return -2;
  }

  return ret;
}

void DecodeData::CloseFile() {

  if (rawfile)
    fclose(rawfile);
  rawfile = NULL;

  return;
}

int DecodeData::EndOfFile() {

  int eff = 0;
  unsigned short int dummy;
  (void)dummy; // silence unused warning

  if (rawfile) {
    dummy = 0; // without the declaration of this variable it's not working (not finding the end of the file!!!!!)
    eff = feof(rawfile);
  } else {
    printf("Error file pointer is NULL \n");
    exit(3);
  }

  return eff;
}

//=============================================================================================

//=============================================================================================

/*
double DecodeData::ComputeCN(int size, short int* RawSignal, float* pede, float* RawSoN, double threshold){

   double mean=0.0;
  int n=0;

  for (int ii=0; ii<size; ii++) {
    if (RawSoN[ii]<threshold) {//to avoid real signal...
      n++;
      //      printf("    %d) %f %f\n", ii, RawSignal[ii]/8.0, pede[ii]);
      mean+=(RawSignal[ii]/8.0-pede[ii]);
    }
  }
  if (n>1) {
    mean/=n;
  }
  else { //let's try again with an higher threshold
    mean = ComputeCN(size, RawSignal, pede, RawSoN, threshold+1.0);
  }
  //  printf("    CN = %f\n", mean);

  return mean;
}
*/

TDR DecodeData::GetTDR_bynums(int tdrnum, int jinfnum) {
  if (rh) {
    return rh->GetTDR_bynums(tdrnum, jinfnum);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  TDR dummy;
  return dummy;
}

TDR DecodeData::GetTDR_byglobindex(int tdrglobindex) {
  if (rh) {
    return rh->GetTDR_byglobindex(tdrglobindex);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  TDR dummy;
  return dummy;
}

int DecodeData::GetTdrNum_byID(int tdrid) {
  if (rh) {
    return rh->GetTdrNum_byID(tdrid);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeData::GetJinfNum_byID(int tdrid) {
  if (rh) {
    return rh->GetJinfNum_byID(tdrid);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeData::GetTdrNum_byglobindex(int tdrglobindex) {
  if (rh) {
    return rh->GetTdrNum_byglobindex(tdrglobindex);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeData::GetJinfNum_byglobindex(int tdrglobindex) {
  if (rh) {
    return rh->GetJinfNum_byglobindex(tdrglobindex);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeData::GetTdrGlobIndex_bynums(int tdrnum, int jinfnum) {
  if (rh) {
    return rh->GetTdrGlobIndex_bynums(tdrnum, jinfnum);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeData::GetJinfIndex_bynum(int jinfnum) {
  if (rh) {
    return rh->GetJinfIndex_bynum(jinfnum);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeData::GetJinfNum_byindex(int jinfindex) {
  if (rh) {
    return rh->GetJinfNum_byindex(jinfindex);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeData::ComputeTdrId(int tdrnum, int jinfnum) { return rh->ComputeTdrId(tdrnum, jinfnum); }
