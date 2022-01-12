#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "DecodeData.hh"

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

void DecodeData::AddCluster(int numnum, int Jinfnum, int clusadd, int cluslen, int Sig2NoiStatus, int CNStatus,
                            int PowBits, int bad, float *sig, bool kRaw) {
  pri = 1;

  static LadderConf *ladderconf = Event::GetLadderConf();
  int _bondingtype = 0;

  if (cworkaround != 0) {
    _bondingtype = cworkaround;
  } else {
    _bondingtype = ladderconf->GetBondingType(Jinfnum, numnum);
  }

  //  printf("bondingtype = %d\n", _bondingtype);

  int newclusadd = clusadd;

  if (!kRaw) { // otherwise the swap has been already done when clustering (if we were clustering, otherwise the cluster
               // is not present and we never reach this function...)
    if (_bondingtype == 2) {
      // if (clusadd>=(3*64) && clusadd<(5*64)) newclusadd+=3*64;
      // if (clusadd>=(5*64) && clusadd<(8*64)) newclusadd-=2*64;
      // questo cura in parte dei problemi sui bordi ancora da capire
      if (clusadd >= (3 * 64) && (clusadd + cluslen - 1) < (5 * 64))
        newclusadd += 3 * 64;
      if ((clusadd + cluslen - 1) >= (5 * 64) && clusadd < (8 * 64))
        newclusadd -= 2 * 64;
    }
  }

  int sid = 0;
  if (!kMC && clusadd >= 640)
    sid = 1;
  if (kMC)
    sid = !tdrAlign[numnum]; // check alignment. MD: why we need this in the MC case?

  Cluster *pp = ev->AddCluster(Jinfnum, numnum + 100 * Jinfnum, sid);
  calib *cal = &(cals[numnum + 100 * Jinfnum]);

  //  pp->Build(numnum+100*Jinfnum,sid,clusadd,cluslen,sig,&(cal->sig[clusadd]),
  //	    &(cal->status[clusadd]), Sig2NoiStatus, CNStatus, PowBits, bad);
  // ONLY the 3rd field should be changed (clusadd->newclusadd) to move the cluster.
  // The 'clusadd' passed to the array should be left as it is to read the same signal values
  // for the 'sig' array there's no problem since already starting from 0
  pp->Build(numnum + 100 * Jinfnum, sid, newclusadd, cluslen, sig, &(cal->sig[clusadd]), &(cal->status[clusadd]),
            Sig2NoiStatus, CNStatus, PowBits, bad);

  double cog = pp->GetCoG();
  double seedadd = pp->GetSeedAdd();

  hocc[numnum + Event::NTDRS * Jinfnum]->Fill(cog);
  hoccseed[numnum + Event::NTDRS * Jinfnum]->Fill(seedadd);
  //#define TOTCHARGE
#ifndef TOTCHARGE
  hcharge[numnum + Event::NTDRS * Jinfnum][sid]->Fill(pp->GetSeedCharge());
  hsignal[numnum + Event::NTDRS * Jinfnum][sid]->Fill(pp->GetSeedVal());
  hchargevsocc[numnum + Event::NTDRS * Jinfnum]->Fill(cog, pp->GetSeedCharge());
  hsignalvsocc[numnum + Event::NTDRS * Jinfnum]->Fill(cog, pp->GetSeedVal());
  hson[numnum + Event::NTDRS * Jinfnum][sid]->Fill(pp->GetSeedSN());
#else
  hcharge[numnum + Event::NTDRS * Jinfnum][sid]->Fill(pp->GetCharge());
  hsignal[numnum + Event::NTDRS * Jinfnum][sid]->Fill(pp->GetTotSig());
  hchargevsocc[numnum + Event::NTDRS * Jinfnum]->Fill(cog, pp->GetCharge());
  hsignalvsocc[numnum + Event::NTDRS * Jinfnum]->Fill(cog, pp->GetTotSig());
  hson[numnum + Event::NTDRS * Jinfnum][sid]->Fill(pp->GetTotSN());
#endif

  // if (pp->GetSeedSN()<3.5) {
  //     printf("%s", stringtodump.Data());
  //   for (int ii=0; ii<cluslen; ii++) {
  //     printf("AddCluster) %d %f\n", ii, sig[ii]);
  //   }
  //  printf("***** SoN: %f, Sig: %f, SeedAdd: %d side %d \n", pp->GetSeedSN(), pp->GetSeedVal(), pp->GetSeed(),
  //  pp->side);
  // pp->Print();
  //  sleep(3);
  // }
  // if (pp->GetSeedVal()<1.0) {
  //   printf("%s", stringtodump.Data());
  //   for (int ii=0; ii<cluslen; ii++) {
  //     printf("AddCluster) %d %f\n", ii, sig[ii]);
  //   }
  //   printf("***** SoN: %f, Sig: %f, SeedAdd: %d\n", pp->GetSeedSN(), pp->GetSeedVal(), pp->GetSeed());
  //   pp->Print();
  //   sleep(3);
  // }

  //  if (pri)
  //    pp->Print();

  return;
}

void DecodeData::Clusterize(int numnum, int Jinfnum, calib *cal) {

  pri = 1;

  int _bondingtype = 0;
  bool defaultThresholds =
      (shighthreshold == 3.5 && khighthreshold == 3.5 && slowthreshold == 1.0 && klowthreshold == 1.0);

  static LadderConf *ladderconf = Event::GetLadderConf();

  static bool printed = false;
  if (!printed) {
    printf("\nClustering with:\n");
    printf("    %f %f for S-side\n", shighthreshold, slowthreshold);
    printf("    %f %f for K-side\n", khighthreshold, klowthreshold);
    if (!defaultThresholds) {
      printf("    thresholds used FOR ALL THE LADDERS\n");
    }

    if (defaultThresholds && ladderconf) {
      printf("Except for: \n");

      for (int jj = 0; jj < Event::NJINF; jj++) {
        for (int tt = 0; tt < Event::NTDRS; tt++) {
          double shithresh = ladderconf->GetSHiThreshold(jj, tt);
          double khithresh = ladderconf->GetKHiThreshold(jj, tt);
          double slothresh = ladderconf->GetSLoThreshold(jj, tt);
          double klothresh = ladderconf->GetKLoThreshold(jj, tt);

          if (shithresh != 3.5 || khithresh != 3.5 || slothresh != 1.0 || klothresh != 1.0) {
            printf("    JINF=%d, TDR=%d\n", jj, tt);
            printf("    %f %f for S-side\n", shithresh, slothresh);
            printf("    %f %f for K-side\n", khithresh, klothresh);
          }
        }
      }
    }

    if (cworkaround != 0) {
      printf("\n    %d as workaround FOR ALL THE LADDERS\n", cworkaround);
      _bondingtype = cworkaround;
    } else {
      printf("\n");
      for (int jj = 0; jj < Event::NJINF; jj++) {
        for (int tt = 0; tt < Event::NTDRS; tt++) {
          if (ladderconf->GetBondingType(jj, tt) != 0)
            printf("    %d as workaround for JINF=%d, TDR=%d\n", ladderconf->GetBondingType(jj, tt), jj, tt);
        }
      }
    }
    printed = true;
  }


  _bondingtype = ladderconf->GetBondingType(Jinfnum, numnum);

  int tdrnumraw = FindPos(numnum + 100 * Jinfnum);

  //// numnum -> mapped to find the ntdr(=nlayer)
  if (kMC)
    tdrnumraw = numnum;

  //// nvas were 16 total summing S and K
  //  int nvasS=10;
  //  int nvasK= 6;
  int nvasS = Event::NVAS; // changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  int nvasK = Event::NVAS; // changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  //// hardcoded nchannels per va was 64
  //  int nchavaS=64;
  //  int nchavaK=64;
  // defined in
  int nchavaS = Event::NCHAVA; // changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  int nchavaK = Event::NCHAVA; // changed by Viviana. MD: check if is coherent with the "bondingtype" stuff
  if (_bondingtype == 1) {
    nchavaS = 32;
  }

  int nvas = Event::NVAS;     // changed by Viviana
  int nchava = Event::NCHAVA; // changed by Viviana
  //// hardcoded nchannels per tdr was 1024
  short int array[nvas * nchava]; // changed by Viviana
  float arraySoN[nvas * nchava];  // changed by Viviana
  float pede[nvas * nchava];      // changed by Viviana
  float sigma[nvas * nchava];     // changed by Viviana
  int status[nvas * nchava];      // changed by Viviana
  int arraysize = nvas * nchava;  // changed by Viviana

  double highthreshold = 3.5;
  double lowthreshold = 1.0;

  int shift = 0;

  TString headerstringtodump = "New clusterize:\n";
  bool added = false; // added by Viviana

  for (int side = 0; side < 2; side++) {
    if (added)
      continue;
    if (side == 0) {
      nvas = nvasS;
      nchava = nchavaS;
      if (defaultThresholds) {
        highthreshold = ladderconf->GetSHiThreshold(Jinfnum, numnum);
        lowthreshold = ladderconf->GetSLoThreshold(Jinfnum, numnum);
      } else {
        highthreshold = shighthreshold;
        lowthreshold = slowthreshold;
      }

      shift = 0;
      if (_bondingtype == 1) {
        arraysize = 320;
        for (int cc = 0; cc < arraysize; cc++) {
          array[cc] = ev->RawSignal[Jinfnum][tdrnumraw][cc * 2];
          arraySoN[cc] = ev->RawSoN[Jinfnum][tdrnumraw][cc * 2];
          pede[cc] = cal->ped[cc * 2];
          sigma[cc] = cal->sig[cc * 2];
          status[cc] = cal->status[cc * 2];
        }
      } else if (_bondingtype == 2) {
        arraysize = 384;
        int halfarraysize = ((int)(arraysize / 2));
        int shift = ((int)(640 / 2));
        for (int cc = 0; cc < halfarraysize; cc++) {
          //	  printf("%d %d %d\n", cc, cc+halfarraysize, cc+shift);
          array[cc] = ev->RawSignal[Jinfnum][tdrnumraw][cc];
          arraySoN[cc] = ev->RawSoN[Jinfnum][tdrnumraw][cc];
          pede[cc] = cal->ped[cc];
          sigma[cc] = cal->sig[cc];
          status[cc] = cal->status[cc];
          array[cc + halfarraysize] = ev->RawSignal[Jinfnum][tdrnumraw][cc + shift];
          arraySoN[cc + halfarraysize] = ev->RawSoN[Jinfnum][tdrnumraw][cc + shift];
          pede[cc + halfarraysize] = cal->ped[cc + shift];
          sigma[cc + halfarraysize] = cal->sig[cc + shift];
          status[cc + halfarraysize] = cal->status[cc + shift];
        }
      } else {
        // arraysize=640;
//        arraysize = nvas * nchava; // changed by Viviana
        memcpy(array, ev->RawSignal[Jinfnum][tdrnumraw], arraysize * sizeof(ev->RawSignal[Jinfnum][tdrnumraw][0]));
        memcpy(arraySoN, ev->RawSoN[Jinfnum][tdrnumraw], arraysize * sizeof(ev->RawSoN[Jinfnum][tdrnumraw][0]));
        memcpy(pede, cal->ped, arraysize * sizeof(cal->ped[0]));
        memcpy(sigma, cal->sig, arraysize * sizeof(cal->sig[0]));
        memcpy(status, cal->status, arraysize * sizeof(cal->status[0]));
        if (kMC) { // added by Viviana
          added = true;
        }
      }
    } // side=0
    else {
      if (_bondingtype == 2) {
        continue;
      } else {
        nvas = nvasK;
        nchava = nchavaK;

        if (defaultThresholds) {
          highthreshold = ladderconf->GetKHiThreshold(Jinfnum, numnum);
          lowthreshold = ladderconf->GetKLoThreshold(Jinfnum, numnum);
        } else {
          highthreshold = khighthreshold;
          lowthreshold = klowthreshold;
        }

        shift = !kMC ? m_defaultShift : 0;                     // changed by Viviana
        arraysize = !kMC ? m_defaultArraySize : nvas * nchava; // changed by Viviana
        ////arraysize=!kMC?384:1024;//changed by Viviana
        // the src is the same array as in the S-side case but passing the reference to the first element of K-side
        // (640)

        memcpy(array, &(ev->RawSignal[Jinfnum][tdrnumraw][shift]), arraysize * sizeof(ev->RawSignal[Jinfnum][tdrnumraw][0]));
        memcpy(arraySoN, &(ev->RawSoN[Jinfnum][tdrnumraw][shift]), arraysize * sizeof(ev->RawSoN[Jinfnum][tdrnumraw][0]));
        memcpy(pede, &(cal->ped[shift]), arraysize * sizeof(cal->ped[0]));
        memcpy(sigma, &(cal->sig[shift]), arraysize * sizeof(cal->sig[0]));
        memcpy(status, &(cal->status[shift]), arraysize * sizeof(cal->sig[0]));
      }
    }

    double CN[nvas];

    for (int va = 0; va < nvas; va++) {
      CN[va] = Event::ComputeCN(nchava, &(array[va * nchava]), &(pede[va * nchava]), &(arraySoN[va * nchava]),
                                &(status[va * nchava]));
      //      printf("%d) %f\n", va, CN[va]);
      //      headerstringtodump += Form("CN[%d] = %f\n", va, CN[va]);
    }

    int bad = 0;
    int cluslen = 0;       // to fill
    int Sig2NoiStatus = 0; // boh
    int clusadd = 0;       // to fill
    int CNStatus = 0;      // boh
    int PowBits = 0;       // boh
    float sig[MAXLENGHT];

    bool firstfound = false;
    bool seedfound = false;

    float ssonmax = -999;
    int seedaddmax = -1;

    TString clusterstringtodump = "--> New cluster\n";
    //printf("--> New cluster\n");

    for (int count = 0; count < arraysize; count++) {

      if (status[count])
        continue;

      // printf("count = %d\n", count);
      //      clusterstringtodump += Form("count = %d\n", count);

      int va = (int)(count / nchava);

      float ssun = (array[count] / m_adcUnits - pede[count] - CN[va]) / sigma[count];
      // if (ssun>highthreshold) printf("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va],
      // sigma[count], ssun);

      // printf("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va], sigma[count], ssun);

      //      clusterstringtodump += Form("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va],
      //      sigma[count], ssun);

      if (ssun >= highthreshold) { // the seed that can also be the first of the cluster
        //	printf("%d) >high\n", count);
        //	clusterstringtodump += Form("%d) >high\n", count);
        //	clusterstringtodump += Form("%d) %f %f %f %f -> %f\n", count, array[count]/8.0, pede[count], CN[va],
        // sigma[count], ssun);

        if (ssun > ssonmax) {
          seedaddmax = count;
          ssonmax = ssun;
          //	  printf("%d) New max %d %f\n", count, seedaddmax, ssonmax);
          //	  clusterstringtodump += Form("%d) New max %d %f\n", count, seedaddmax, ssonmax);
        }

        if (firstfound) { // this is the seed (maybe there was another seed previously, but doesn't matter...)
          seedfound = true;
          cluslen++;
          //	  printf("%d) >high and first found already (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) >high and first found already (cluslen=%d)\n", count, cluslen);
        } else { // is the seed but also the first of the cluster...
          firstfound = true;
          seedfound = true;
          clusadd = count;
          cluslen = 1;
          //	  printf("%d) >high, is the seed and also the first of the cluster (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) >high, is the seed and also the first of the cluster (cluslen=%d)\n",
          // count, cluslen);
        }
      } else if (ssun >= lowthreshold) { // potentially the start of a cluster, or maybe another neighbour...
        //	clusterstringtodump += Form("%d) >low\n", count);
        if (!firstfound) { // is the first of the potential cluster
          firstfound = true;
          clusadd = count;
          cluslen = 1;
          //	  printf("%d) >low, is the first of the potential cluster (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) >low, is the first of the potential cluster (cluslen=%d)\n", count,
          // cluslen);
        } else { // there was already a 'first' so this can be a neighbour between the first and the seed or a neighbour
                 // after
          cluslen++;
          //	  printf("%d) >low but firstfound already (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) >low but firstfound already (cluslen=%d)\n", count, cluslen);
        }
      } else if (ssun < lowthreshold ||
                 count == (arraysize - 1)) { // end of a cluster or end of a "potential" cluster or simply nothing
        //	printf("%d) <low (cluslen=%d)\n", count, cluslen);
        //	clusterstringtodump += Form("%d) <low (cluslen=%d)\n", count, cluslen);
        if (seedfound) { // the cluster is done, let's save it!
          //	  printf("%d) <low, seed found already: let's save the cluster (cluslen=%d)\n", count, cluslen);
          //	  clusterstringtodump += Form("%d) <low, seed found already: let's save the cluster (cluslen=%d)\n",
          // count, cluslen);
          //          if (pri)
          //            printf("Cluster: add=%d  lenght=%d, seed=%d\n", clusadd + shift, cluslen, seedaddmax + shift);
          //	  clusterstringtodump += Form("Cluster: add=%d  lenght=%d, seed=%d\n", clusadd+shift, cluslen,
          // seedaddmax+shift);
          for (int hh = clusadd; hh < (clusadd + cluslen); hh++) {
            int _va = (int)(hh / nchava);
            float s = array[hh] / m_adcUnits - pede[hh] - CN[_va];
            //	    clusterstringtodump += Form("Sig=%f from Array = %d, Ped=%f, CN=%f\n", s, array[hh], pede[hh],
            // CN[_va]);
            //            if (pri)
            //              printf("Signal: %d, Pos:%d\n", (int)(m_adcUnits * s), hh + shift);
            //	    clusterstringtodump += Form("Signal: %d, Pos:%d\n", (int)(8*s), hh+shift);
            if ((hh - clusadd) < MAXLENGHT) {
              sig[hh - clusadd] = s;
              //              if (pri)
              //                printf("        %f, Pos: %d\n", sig[hh - clusadd], hh + shift);
              //	      clusterstringtodump += Form("        %f, Pos: %d\n", sig[hh-clusadd], hh+shift);
            } else
              bad = 1;
          }
          //	  clusterstringtodump += Form("Status[%d] = %d\n", seedaddmax+shift, status[seedaddmax]);
          stringtodump = headerstringtodump + clusterstringtodump;
          if (!(status[seedaddmax] & (1 << 3))) { // if is not a bad cluster
            //	    printf("numnum = %d\n", numnum);
            AddCluster(numnum, Jinfnum, clusadd + shift, cluslen, Sig2NoiStatus, CNStatus, PowBits, bad, sig);
          }
          clusterstringtodump = "--> New cluster\n";
        }
        // there was no seed found: "potential" cluster not promoted or even "nothing"
        seedfound = false;
        firstfound = false;
        memset(sig, 0, MAXLENGHT * sizeof(sig[0]));
        ssonmax = -9999;
        seedaddmax = -1;
        stringtodump = headerstringtodump + clusterstringtodump;
        clusterstringtodump = "--> New cluster\n";
      }
    }
  }

  return;
}

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
    if (pri)
      printf("End of File \n");
    return -1;
  }
  if (ferror(stream)) {
    if (pri)
      printf("Error reading \n");
    return -2;
  }

  return ret;
}