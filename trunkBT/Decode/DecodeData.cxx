#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "DecodeData.hh"

#include "GenericEvent.hpp"

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

int DecodeData::FindPos(int tdrnum) {

  for (int ii = 0; ii < ntdrCmp + ntdrRaw; ii++)
    if (tdrMap[ii].first == tdrnum)
      return ii;

  return -1;
}
//=============================================================================================

int DecodeData::FindCalPos(int tdrnum) {

  for (int ii = 0; ii < ntdrCmp + ntdrRaw; ii++)
    if (tdrMap[ii].first == tdrnum)
      return ii;

  printf("DecodeData::FindCalPos:  Fatal Error can't find postion for TDR %d\n", tdrnum);

  exit(4);
}
//=============================================================================================

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
}*/

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
