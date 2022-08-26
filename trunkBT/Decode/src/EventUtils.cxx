#include <bitset>
#include <cmath>
#include <iostream>
#include <unistd.h>

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TRotation.h"

#include "Cluster.hh"
#include "EventUtils.hh"

using namespace std;

//#define USEMINUIT

void GainCorrectionPars::InitSize(size_t nJinf, size_t nTdr, size_t nVA_S, size_t nVA_K) {
  
  m_pars.resize(nJinf);

  for (size_t iJ = 0; iJ < nJinf; ++iJ) {
    m_pars[iJ].resize(nTdr);
    for (size_t iT = 0; iT < nTdr; ++iT) {
      m_pars[iJ][iT].resize(nVA_S+nVA_K);
      for (size_t iV = 0; iV < (nVA_S+nVA_K); ++iV) {
        m_pars[iJ][iT][iV][0] = 0.0;
        m_pars[iJ][iT][iV][1] = 0.0;
      }
    }
  }
}

void GainCorrectionPars::Init(const std::string &filename) {
  printf("Reading Gain Correction from %s:\n", filename.c_str());

  size_t NJINF = m_pars.size();
  size_t NTDRS = m_pars[0].size();
  size_t NVAS = m_pars[0][0].size();

  for (size_t jj = 0; jj < NJINF; jj++) {
    for (size_t tt = 0; tt < NTDRS; tt++) {
      for (size_t vv = 0; vv < NVAS; vv++) {
        m_pars[jj][tt][vv][0] = 0.0;
        m_pars[jj][tt][vv][1] = 1.0;
      }
    }
  }

  int const dimline = 255;
  char line[dimline];
  int jinfnum = 0;
  int tdrnum = 0;
  int vanum = 0;
  float dummy = 0.;

  FILE *ft = fopen(filename.c_str(), "r");
  if (ft == NULL) {
    printf("Error: cannot open %s \n", filename.c_str());
    return;
  } else {
    while (1) {
      if (fgets(line, dimline, ft) != NULL) {
        if (*line == '#')
          continue; /* ignore comment line */
        else {
          sscanf(line, "%d\t%d\t%d\t%f\t%f", &jinfnum, &tdrnum, &vanum, &dummy, &dummy);
          //	  printf("%d %d %d\n", NJINF, NTDRS, NVAS);
          if (static_cast<size_t>(jinfnum) < NJINF && static_cast<size_t>(tdrnum) < NTDRS &&
              static_cast<size_t>(vanum) < NVAS) {
            sscanf(line, "%d \t %d \t %d \t %f \t %f", &jinfnum, &tdrnum, &vanum, &m_pars[jinfnum][tdrnum][vanum][0],
                   &m_pars[jinfnum][tdrnum][vanum][1]);
          } else {
            printf("Wrong JINF/TDR/VA (%d, %d, %d): maximum is (%lu, %lu, %lu)\n", jinfnum, tdrnum, vanum, NJINF - 1,
                   NTDRS - 1, NVAS - 1);
          }
        }
      } else {
        printf(" closing gain correction file \n");
        fclose(ft);
        break;
      }
    }
  }
}

void AlignmentPars::InitSize(size_t nJinf, size_t nTdr) {
  
  m_pars.resize(nJinf);
  m_multflip.resize(nJinf);

  for (size_t iJ = 0; iJ < nJinf; ++iJ) {
    m_pars[iJ].resize(nTdr);
    m_multflip[iJ].resize(nTdr);
    for (size_t iT = 0; iT < nTdr; ++iT) {
      m_pars[iJ][iT][0] = 0.0;
      m_pars[iJ][iT][1] = 0.0;
      m_pars[iJ][iT][2] = 0.0;
      m_multflip[iJ][iT] = false;
    }
  }
}

void AlignmentPars::Init(const std::string &filename) {
  printf("Reading alignment from %s:\n", filename.c_str());

  size_t NJINF = m_pars.size();
  size_t NTDRS = m_pars[0].size();

  for (size_t jj = 0; jj < NJINF; jj++) {
    for (size_t tt = 0; tt < NTDRS; tt++) {
      for (int cc = 0; cc < 3; cc++) {
        m_pars[jj][tt][cc] = 0.0;
      }
      m_multflip[jj][tt] = false;
    }
  }

  int const dimline = 255;
  char line[dimline];
  float dummy;
  int dummyint;
  int jinfnum = 0;
  int tdrnum = 0;

  FILE *ft = fopen(filename.c_str(), "r");
  if (ft == NULL) {
    printf("Error: cannot open %s \n", filename.c_str());
    return;
  } else {
    while (1) {
      if (fgets(line, dimline, ft) != NULL) {
        if (*line == '#')
          continue; /* ignore comment line */
        else {
          sscanf(line, "%d\t%d\t%f\t%f\t%f\t%d", &jinfnum, &tdrnum, &dummy, &dummy, &dummy, &dummyint);
          if (static_cast<size_t>(jinfnum) < NJINF && static_cast<size_t>(tdrnum) < NTDRS) {
            sscanf(line, "%d\t%d\t%f\t%f\t%f\t%d", &jinfnum, &tdrnum, &m_pars[jinfnum][tdrnum][0],
                   &m_pars[jinfnum][tdrnum][1], &m_pars[jinfnum][tdrnum][2], &dummyint);
            m_multflip[jinfnum][tdrnum] = (bool)(dummyint);
          } else {
            printf("Wrong JINF/TDR (%d, %d): maximum is (%ld,%ld)\n", jinfnum, tdrnum, NJINF, NTDRS);
          }
        }
      } else {
        printf(" closing alignment file \n");
        fclose(ft);
        break;
      }
    }
  }
}

track::track() {
  prod_angle = 9999.;
  exit_angle = 9999.;
  prod_dist = 9999.;
  exit_dist = 9999.;
}

void track::update() {
  unsigned int ss = hits.size();
  if (ss > 1) {
    prod_angle = atan((hits[1].second.first - hits[0].second.first) / (hits[1].second.second - hits[0].second.second));
    exit_angle = atan((hits[ss - 1].second.first - hits[ss - 2].second.first) /
                      (hits[ss - 1].second.second - hits[ss - 2].second.second));

    prod_dist = cos(prod_angle) * hits[0].second.first - sin(prod_angle) * hits[0].second.second;
    exit_dist = cos(exit_angle) * hits[ss - 1].second.first - sin(exit_angle) * hits[ss - 1].second.second;
  }
  return;
}

std::pair<double, double> vertex(track &tr1, track &tr2) {
  if (tr1.hits.size() < 2 || tr2.hits.size() < 2)
    return make_pair(9999., 9999.); // NULL;

  std::pair<double, double> vert; //=new std::pair<double,double>;
  if (tr1.hits[0].first == tr2.hits[0].first && tr1.hits[1].first == tr2.hits[1].first) {
    vert.first = tr1.hits[0].second.first;
    vert.second = tr1.hits[0].second.second;
  } else {
    vert.second = (tr2.prod_dist * cos(tr1.prod_angle) - tr1.prod_dist * cos(tr2.prod_angle)) /
                  (cos(tr2.prod_angle) * sin(tr1.prod_angle) - cos(tr1.prod_angle) * sin(tr2.prod_angle)); // Z
    vert.first = (tr1.prod_dist + sin(tr1.prod_angle) * vert.second) / cos(tr1.prod_angle);                // X or Y
  }
  return vert;
}

HoughSpace::HoughSpace(double th, double r) {
  risth = th;
  risr = r;
  maxfreq = -1;
  maxpos = make_pair(9999., 9999.);
}

void HoughSpace::Add(double th, double r) {
  int f = 1;
  pair<double, double> vote = make_pair(floor(th / risth) * risth, floor(r / risr) * risr);
  if (votes.find(vote) == votes.end())
    votes.insert(make_pair(vote, 1));
  else
    f = ++votes[vote];
  if (f > maxfreq) {
    //    printf("nuovo max\n");
    maxfreq = f;
    maxpos = vote;
  }
}

//-------------------------------------------------------------------------------------
