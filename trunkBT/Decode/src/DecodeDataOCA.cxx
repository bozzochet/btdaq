//
// Created by Valerio Formato on 12/10/21.
//

#include "DecodeDataOCA.hh"
#include "FSUtils.hh"

#include "GenericEvent.hpp"

#include <numeric>
#include <sstream>

namespace {
constexpr auto NJINF = DecodeDataOCA::EventOCA::GetNJINF();
constexpr auto NTDRS = DecodeDataOCA::EventOCA::GetNTDRS();
constexpr auto NVAS = DecodeDataOCA::EventOCA::GetNVAS();
constexpr auto NCHAVA = DecodeDataOCA::EventOCA::GetNCHAVA();
constexpr auto NADCS = DecodeDataOCA::EventOCA::GetNADCS();
} // namespace

DecodeDataOCA::DecodeDataOCA(std::string rawDir, std::string calDir, unsigned int runNum)
    : m_rawDir{std::move(rawDir)}, m_calDir{std::move(calDir)} {

  // Init base-class members
  kMC = false;
  runn = runNum;
  shighthreshold = 3.5;
  slowthreshold = 1.0;
  khighthreshold = 3.5;
  klowthreshold = 1.0;
  kClusterize = false;

  m_defaultShift = 0;
  m_defaultArraySize = NVAS * NCHAVA;

  tdrMap = new laddernumtype[NJINF * NTDRS];

  pri = false;

  ev = new EventOCA();
  //  std::cout << "ev: " << ev << '\n';

  DecodeDataOCA::OpenFile(m_rawDir.c_str(), m_calDir.c_str(), runn, -1);
  // we assume we also have the corresponding calibration file
  std::cout << "Raw file: " << m_filename << '\n';
  std::cout << "Calibration file: " << m_calFilename << '\n';

  std::string filePath = m_rawDir + "/" + m_filename;
  rawfile = fopen(filePath.c_str(), "r");
  if (!rawfile) {
    printf("Error file %s not found \n", filePath.c_str());
    exit(2);
  }

  DecodeDataOCA::DumpRunHeader();
  // we assume that from now on we know how many boards are in the DAQ
  ntdrRaw = NTDRS;
  ntdrCmp = 0;

  // VF: create a fake trdMap. I still don't know what it is used for and I don't wanna know why it's an array of pairs
  for (unsigned int iTdr = 0; iTdr < NJINF * NTDRS; ++iTdr) {
    tdrMap[iTdr] = {iTdr, 1};
  }

  ProcessCalibration();
  
  // TODO: add setup of histograms
  InitHistos();
}

DecodeDataOCA::~DecodeDataOCA() {

  for (size_t jj = 0; jj < NJINF; jj++) {
    for (size_t hh = 0; hh < NTDRS; hh++) {
      //      printf("%d %d --> %f\n", jj, hh, hocc[jj*NTDRS+hh]->GetEntries());
      if (hocc[jj * NTDRS + hh] && hocc[jj * NTDRS + hh]->GetEntries() < 1.0) {
        //		printf("deleting hocc %d %d at %p\n", jj, hh, hocc[jj*NTDRS+hh]);
        delete hocc[jj * NTDRS + hh];
      }
      if (hoccseed[jj * NTDRS + hh] && hoccseed[jj * NTDRS + hh]->GetEntries() < 1.0) {
        //		printf("deleting hoccseed %d %d\n", jj, hh);
        delete hoccseed[jj * NTDRS + hh];
      }
      if (hchargevsocc[jj * NTDRS + hh] && hchargevsocc[jj * NTDRS + hh]->GetEntries() < 1.0) {
        //		  printf("deleting hchargevsocc %d %d\n", jj, hh);
        delete hchargevsocc[jj * NTDRS + hh];
      }
      if (hsignalvsocc[jj * NTDRS + hh] && hsignalvsocc[jj * NTDRS + hh]->GetEntries() < 1.0) {
        //		  printf("deleting hsignalvsocc %d %d\n", jj, hh);
        delete hsignalvsocc[jj * NTDRS + hh];
      }
      
      for (int ss = 0; ss < 2; ss++) {
        //	printf("%d %d %d --> %f\n", jj, hh, ss, hcharge[jj*NTDRS+hh][ss]->GetEntries());
        if (hcharge[jj * NTDRS + hh][ss] && hcharge[jj * NTDRS + hh][ss]->GetEntries() < 1.0) {
          //	  	  printf("deleting hcharge %d %d %d\n", jj, hh, ss);
          delete hcharge[jj * NTDRS + hh][ss];
        }
        if (hsignal[jj * NTDRS + hh][ss] && hsignal[jj * NTDRS + hh][ss]->GetEntries() < 1.0) {
          //	  	  printf("deleting hsignal %d %d %d\n", jj, hh, ss);
          delete hsignal[jj * NTDRS + hh][ss];
        }
        if (hson[jj * NTDRS + hh][ss] && hson[jj * NTDRS + hh][ss]->GetEntries() < 1.0) {
          //	  	  printf("deleting hson %d %d %d\n", jj, hh, ss);
          delete hson[jj * NTDRS + hh][ss];
        }
      }
    }
  }
  
  if (pri)
    std::cout << "In the destructor..." << std::endl;
  if (rawfile)
    CloseFile();
  if (ev)
    delete ev;
  if (pri)
    std::cout << "Destroyed." << std::endl;

  return;
}

void DecodeDataOCA::OpenFile(const char *rawDir, const char *calDir, int runNum, int ancillary) {
  auto fileList = FSUtils::ListDir(rawDir);

  auto fileName_it = std::find_if(begin(fileList), end(fileList), [this](const std::string &_filename) {
    // all our files begin with 'SCD_RUN' followed by zero-padded run numbers
    unsigned int runNum = std::atoi(_filename.substr(7, 5).c_str());
    return runNum == static_cast<unsigned int>(runn);
  });

  if (fileName_it == end(fileList)) {
    return;
  }
  m_filename = *fileName_it;

  auto calFilename_it = std::find_if(std::reverse_iterator<decltype(fileName_it)>(fileName_it), rend(fileList),
                                     [](const std::string &_filename) { return _filename.substr(13, 3) == "CAL"; });

  if (calFilename_it != rend(fileList)) {
    m_calFilename = *calFilename_it;
  }
}

// TODO: waiting for run header definition
void DecodeDataOCA::DumpRunHeader() {
  printf("There're %d boards (%d detectors)\n", m_numBoards, 2*m_numBoards);
}

int DecodeDataOCA::GetTdrNum(size_t pos) {
  if (pos > NJINF * NTDRS) {
    printf("Pos %ld not allowed. Max is %ld\n", pos, NJINF * NTDRS);
    return -9999;
  }
  return tdrMap[pos].first;
}

int DecodeDataOCA::GetTdrType(size_t pos) {
  if (pos > NJINF * NTDRS) {
    printf("Pos %ld not allowed. Max is %ld\n", pos, NJINF * NTDRS);
    return -9999;
  }
  return tdrMap[pos].second;
}

// TODO: read calib events, compute mean and sigma
bool DecodeDataOCA::ProcessCalibration() {
  
  int iJinf = 0; // in the OCA case we have just one "collector" (the DAQ PC itself)
  
  std::istringstream is(m_calFilename);
  std::string part;
  std::string prefix = "RUN";
  std::string srunnum = "";
  long int runnum = 0; 
  while (getline(is, part, '_')) {
    //    std::cout << part << std::endl;
    if (part.rfind(prefix, 0) == 0) {
      //      std::cout << part << std::endl;
      srunnum = part.substr(prefix.length());
      //      std::cout << srunnum << std::endl;
      runnum = atol(srunnum.c_str());
      //      std::cout << runnum << std::endl;
    }
  }
  
  // open the calibration file
  std::string calFilePath = m_rawDir + "/" + m_calFilename;
  calfile = fopen(calFilePath.c_str(), "r");
  if (!calfile) {
    printf("Error file %s not found \n", calFilePath.c_str());
    exit(2);
  }
  
  printf("Processing calibration (%s)... \n", calFilePath.c_str());
  auto start = std::chrono::system_clock::now();
  
  auto event = std::make_unique<EventOCA>();
  std::vector<std::vector<std::vector<float>>> signals(NTDRS, std::vector<std::vector<float>>(NVAS * NCHAVA));
  std::vector<std::vector<std::vector<float>>> signals_sorted(NTDRS, std::vector<std::vector<float>>(NVAS * NCHAVA));
  // std::vector<std::vector<std::vector<float> > > signals;
  // signals.resize(NTDRS);
  // for (int ii=0; ii<NTDRS; ii++) {
  //   signals[ii].resize(NVAS * NCHAVA);
  // }
  
  //#define CALPLOTS // in generale deve stare spento, a differenza di AMS non abbiamo i cluster, quando fa il decode qui fa qualche plot di occupancy.   
#define PERCENTILE 0.02
      
  unsigned int nEvents{0};
  while (!feof(calfile)) {
    //while (nEvents<1000) {
    ReadOneEventFromFile(calfile, event.get());
    nEvents++;
    //std::cout << "\rRead " << nEvents << " events" << std::flush;
    //    std::cout << "\rRead " << nEvents << " events" << " (found " << m_numBoardsFound << " boards)" << std::flush;
    
    for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
      for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
        signals[iTdr][iCh].push_back(event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
        signals_sorted[iTdr][iCh].push_back(event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
	//	printf("%d (%d %d)) %hd %f -> %f\n", nEvents, iTdr, iCh, event->RawSignal[iJinf][iTdr][iCh], m_adcUnits, event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
      }
    }
  }
  std::cout << '\n';

  //----------------------------------
  
  for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {

      std::sort(begin(signals_sorted[iTdr][iCh]), end(signals_sorted[iTdr][iCh]));
      unsigned int skipped_ch = PERCENTILE*signals_sorted[iTdr][iCh].size();
      auto beginItr = std::next(begin(signals_sorted[iTdr][iCh]), skipped_ch);
      auto endItr = std::prev(end(signals_sorted[iTdr][iCh]), skipped_ch);
      auto nCh = std::distance(beginItr, endItr);
      //      printf("%ld %f\n", nCh, (1.0-2.0*PERCENTILE)*signals[iTdr][iCh].size());
      
      //      cals[iTdr].ped[iCh] = std::accumulate(begin(signals[iTdr][iCh]), end(signals[iTdr][iCh]), 0.0f) /
      cals[iTdr].ped[iCh] = std::accumulate(beginItr, endItr, 0.0f) /
	//	static_cast<float>(signals[iTdr][iCh].size());
	nCh;
      
      cals[iTdr].rsig[iCh] =
	//	std::sqrt(std::accumulate(begin(signals[iTdr][iCh]), end(signals[iTdr][iCh]), 0.0f,
	//	std::sqrt(std::accumulate(begin(signals[iTdr][iCh])+((int)(PERCENTILE*signals[iTdr][iCh].size())), end(signals[iTdr][iCh])-((int)(PERCENTILE*signals[iTdr][iCh].size())), 0.0f,
	std::sqrt(std::accumulate(beginItr, endItr, 0.0f,
				  [&](float acc, float curr) {
				    return acc + (curr - cals[iTdr].ped[iCh]) * (curr - cals[iTdr].ped[iCh]);
				  }) /
		  //		  static_cast<float>(signals[iTdr][iCh].size()));
		  //		  ((1.0-2.0*PERCENTILE)*static_cast<float>(signals[iTdr][iCh].size())));
		  nCh);
      // initialize this for later
      cals[iTdr].sig[iCh] = 0;
      cals[iTdr].status[iCh] = 0;
    }
  }

  //------------------------------------
  
#ifdef CALPLOTS
  TH1F* hrawsig[NTDRS];
  TH1F* hrawsig_each_ch[NVAS * NCHAVA]; //only for Tdr 0
  TH1F* hrawsig_each_ch_vs_ev[NVAS * NCHAVA]; //only for Tdr 0
  TH1F* hADC_each_ch[NVAS * NCHAVA]; //only for Tdr 0
  TH1F* hADC_each_ch_vs_ev[NVAS * NCHAVA]; //only for Tdr 0
  TH1F* hrawsig_filtered[NTDRS];
  TH1F* hsig[NTDRS];
  for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh)
      if (iTdr==0) {
        hrawsig_each_ch[iCh] = new TH1F(Form("rawsigma_ch%d",iCh),Form("ch=%d; #Event; Sigma Raw",iCh), 1000,-500,500);
        hrawsig_each_ch_vs_ev[iCh] = new TH1F(Form("rawsigma_vs_ev_ch%d",iCh),Form("ch=%d; #Event; Sigma Raw",iCh),signals[iTdr][iCh].size(),0, signals[iTdr][iCh].size());
        hADC_each_ch[iCh] = new TH1F(Form("ADC_ch%d",iCh),Form("ch=%d; #Event; ADC",iCh), 1000,0,1000);
        hADC_each_ch_vs_ev[iCh] = new TH1F(Form("ADC_vs_ev_ch%d",iCh),Form("ch=%d; #Event; ADC",iCh),signals[iTdr][iCh].size(),0, signals[iTdr][iCh].size());
      }
    hrawsig[iTdr] = new TH1F(Form("rawsigma_%d", iTdr), "rawsigma", 1000, -500, 500);
    hrawsig_filtered[iTdr] = new TH1F(Form("rawsigma_filtered_%d", iTdr), "rawsigma", 1000, -500, 500);
    hsig[iTdr] = new TH1F(Form("sigma_%d", iTdr), "sigma", 1000, -500, 500);
    //    printf("%d) %p %p %p\n", iTdr, hrawsig[iTdr], hrawsig_filtered[iTdr], hsig[iTdr]);
  }
  
  for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {  
      for (unsigned int iEv=0; iEv<signals[iTdr][iCh].size(); iEv++) {
	hrawsig[iTdr]->Fill(signals[iTdr][iCh].at(iEv) - cals[iTdr].ped[iCh]);
        if (iTdr==0)        {
          hADC_each_ch[iCh]->Fill(signals[iTdr][iCh].at(iEv));
          hADC_each_ch_vs_ev[iCh]->SetBinContent(iEv+1,signals[iTdr][iCh].at(iEv));
          hrawsig_each_ch[iCh]->Fill(signals[iTdr][iCh].at(iEv) - cals[iTdr].ped[iCh]);
          hrawsig_each_ch_vs_ev[iCh]->SetBinContent(iEv+1,signals[iTdr][iCh].at(iEv) - cals[iTdr].ped[iCh]);
        }
      }
      for (unsigned int iEv=((int)(PERCENTILE*signals[iTdr][iCh].size())); iEv<((int)((1.0-PERCENTILE)*signals[iTdr][iCh].size())); iEv++) {
        hrawsig_filtered[iTdr]->Fill(signals[iTdr][iCh].at(iEv) - cals[iTdr].ped[iCh]);
      }
    }
  }
#endif

  //----------------------------------
  
  unsigned int lastVA = std::numeric_limits<unsigned int>::max();
  std::vector<float> common_noise(NVAS);
  std::vector<std::vector<unsigned int>> processed_events(NTDRS, std::vector<unsigned int>(NVAS * NCHAVA));
  // std::vector<std::vector<unsigned int> > processed_events;
  // processed_events.resize(NTDRS);
  // for (int ii=0; ii<NTDRS; ii++) {
  //   processed_events[ii].resize(NVAS * NCHAVA);
  // }

#ifdef CALPLOTS
  //  TH1F* h_sig_eachVA[1][NCHAVA][signals[0][0].size()];//uno per ogni VA e per ogni evento - se non necessario, meglio tenere commentato che sono molti plot
#endif
  for (unsigned int iEv = 0; iEv < signals[0][0].size(); ++iEv) {
    for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
      for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
        double low_cut = signals_sorted[iTdr][iCh][PERCENTILE*signals_sorted[iTdr][iCh].size()];
        double high_cut = signals_sorted[iTdr][iCh][(1.-PERCENTILE)*signals_sorted[iTdr][iCh].size()];
        unsigned int thisVA = iCh / NCHAVA;
	//	printf("thisVA=%d, lastVA=%d\n", thisVA, lastVA);
      
        if (thisVA != lastVA) {
#ifdef CALPLOTS
          //          if (iTdr==7)          h_sig_eachVA[0][thisVA][iEv] = new TH1F(Form("sig_Tdr%d_VA%d_Ev%d",iTdr,thisVA,iEv),Form("sig_Tdr%d_VA%d_Ev%d",iTdr,thisVA,iEv),1000,-500,500);
#endif
          std::vector<float> values;
          for (unsigned int iVACh = 0; iVACh < NCHAVA; ++iVACh) {
            double sig = signals[iTdr][thisVA * NCHAVA + iVACh][iEv] - cals[iTdr].ped[thisVA * NCHAVA + iVACh];
	    double rawnoise = cals[iTdr].rsig[thisVA * NCHAVA + iVACh];
            double sig_to_rawnoise = sig/rawnoise;
#ifdef CALPLOTS
            //            if (iTdr==7)  h_sig_eachVA[0][thisVA][iEv]->Fill(sig);
#endif
	    //this relies in sorted vectors (i.e. signals), done in preveious loop (sigma raw) 
            //            if (iEv>=((int)(PERCENTILE*signals[iTdr][thisVA * NCHAVA + iVACh].size())) && iEv<((int)((1.0-PERCENTILE)*signals[iTdr][thisVA * NCHAVA + iVACh].size()))) { //probabilemte non serve e comunque questo è sbagliato 
            if (fabs(sig_to_rawnoise)<50.0) {
              if ( sig>low_cut && sig<high_cut )
                values.push_back(sig);
            }
                // }
          }

          //	  printf("%d) Board=%d VA=%d -> %lu events for CN evaluation\n", iEv, iTdr, thisVA, values.size());
          // get the median
          std::sort(begin(values), end(values));
	  if (values.size()>0) {
	    common_noise[thisVA] = 0.5 * (values[(values.size() / 2) - 1] + values[values.size() / 2]);
          }
	  else {
	    /*
	      for (unsigned int iVACh = 0; iVACh < NCHAVA; ++iVACh) {
              double sig = signals[iTdr][thisVA * NCHAVA + iVACh][iEv] - cals[iTdr].ped[thisVA * NCHAVA + iVACh];
              double rawnoise = cals[iTdr].rsig[thisVA * NCHAVA + iVACh];
              double sig_to_rawnoise = sig/rawnoise;
              printf("Event = %d) board=%d, ch=%lu --> sig=%f, ped=%f, S/N=%f\n", iEv, iTdr, thisVA * NCHAVA + iVACh, signals[iTdr][thisVA * NCHAVA + iVACh][iEv], cals[iTdr].ped[thisVA * NCHAVA + iVACh], sig_to_rawnoise);
	      }
	    */
	    common_noise[thisVA] = 0.0;
	  }
	  //	  printf("%f\n", common_noise[thisVA]);
        }
        
	/*
        if (std::fabs(common_noise[thisVA]) > 10) {//not used for the sigma evaluation
          continue;
        }
	*/

	//this relies in sorted vectors (i.e. signals), done in preveious loop (sigma raw)
        if ( signals[iTdr][iCh][iEv]>low_cut && signals[iTdr][iCh][iEv]<high_cut ){
          ++processed_events[iTdr][iCh];
          cals[iTdr].sig[iCh] += (signals[iTdr][iCh][iEv] - cals[iTdr].ped[iCh] - common_noise[thisVA]) *
          (signals[iTdr][iCh][iEv] - cals[iTdr].ped[iCh] - common_noise[thisVA]);
        }
	
#ifdef CALPLOTS
	hsig[iTdr]->Fill(signals[iTdr][iCh][iEv] - cals[iTdr].ped[iCh] - common_noise[thisVA]);
#endif
	
        lastVA = thisVA;
      }
    }
  }
  for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
      if (processed_events[iTdr][iCh]==0 && cals[iTdr].sig[iCh]!=0)
        std::cout<<"     *****"<<cals[iTdr].sig[iCh]<<std::endl;
      cals[iTdr].sig[iCh] = std::sqrt(cals[iTdr].sig[iCh] / static_cast<float>(processed_events[iTdr][iCh]));
    }
  }
  
  auto stop = std::chrono::system_clock::now();
  std::cout << "DecodeDataOCA::ProcessCalibration took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms\n";

  char calfileprefix[255];
  sprintf(calfileprefix,"%s/%ld", m_calDir.c_str(), runnum);
  
  //  for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
  for (unsigned int iTdr = 0; iTdr < (2*m_numBoardsFound); ++iTdr) {
    char calfilename[255];
    sprintf(calfilename, "%s_%02d%02d.cal", calfileprefix, iJinf, iTdr);
    FILE* calfil=fopen(calfilename, "w");
    if(!calfil) {
      printf("problem in opening the %s cal file...\n", calfilename);
      return true;
    }
    
    // writing the common noise (average?)
    for (unsigned int iVa = 0; iVa < NVAS; ++iVa) {
      fprintf(calfil, "%02d,\t%lf,\t%lf\n", iVa, 0.0, 0.0);
    }
    
    // reading channels
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
      fprintf(calfil, "%d %d %d %lf %f %f %f %d\n", iCh+1, (1+(int)(iCh/NCHAVA)), (1+(int)((iCh)%NCHAVA)),
	     cals[iTdr].ped[iCh], cals[iTdr].rsig[iCh], cals[iTdr].sig[iCh], 0.0, 0);
    }
  
    fclose(calfil);
  }

  return true;
}

// TODO: read event data.
int DecodeDataOCA::ReadOneEventFromFile(FILE *file, EventOCA *event) {
  int iJinf = 0; // in the OCA case we have just one "collector" (the DAQ PC itself)

  constexpr uint32_t c_bEvHeader = 0xfa4af1ca;
  constexpr uint32_t c_bHeader = 0xbaba1a9a;
  constexpr uint32_t c_bFooter = 0x0bedface;

  int fstat = 0;

  uint32_t bEvHeader;
  fstat = ReadFile(&bEvHeader, sizeof(bEvHeader), 1, file);
  if (fstat == -1 || bEvHeader != c_bEvHeader) {
    printf("Mismatch in event header %x (expected %x)\n", bEvHeader, c_bEvHeader);
    return 1;
  }

  static unsigned int iTDRmax=0;
  m_numBoardsFound = (iTDRmax+1)/2;
  // printf("numBoardsFound = %d\n", m_numBoardsFound);
  // sleep(1);
  
  for (unsigned int iBoard = 0; iBoard < m_numBoards; ++iBoard) {
    uint32_t bHeader;
    fstat = ReadFile(&bHeader, sizeof(bHeader), 1, file);
    if (fstat == -1) {
      return 1;
    } else if (bHeader != c_bHeader) {
      if (bHeader == c_bEvHeader) {
        // We have less boards than expected? Stop reading this event and get ready for the next one
        // but first reset the file cursor back so that on the next call we'll read the event header
        fseek(file, -sizeof(decltype(c_bEvHeader)), SEEK_CUR);
        return 0;
      } else {
        printf("Mismatch in board header %x (expected %x)\n", bHeader, c_bHeader);
        return 1;
      }
    }
    
    uint32_t messageLength;
    fstat = ReadFile(&messageLength, sizeof(messageLength), 1, file);
    if (fstat == -1)
      return 1;

    uint32_t FWVersion;
    fstat = ReadFile(&FWVersion, sizeof(FWVersion), 1, file);
    if (fstat == -1)
      return 1;

    uint32_t TriggerNumber;
    fstat = ReadFile(&TriggerNumber, sizeof(TriggerNumber), 1, file);
    if (fstat == -1)
      return 1;
    event->Evtnum = TriggerNumber;

    uint16_t TriggerID;
    fstat = ReadFile(&TriggerID, sizeof(TriggerID), 1, file);
    if (fstat == -1)
      return 1;

    uint16_t DetectorID;
    fstat = ReadFile(&DetectorID, sizeof(DetectorID), 1, file);
    if (fstat == -1)
      return 1;

    uint64_t IntTimestamp;
    fstat = ReadFile(&IntTimestamp, sizeof(IntTimestamp), 1, file);
    if (fstat == -1)
      return 1;

    uint64_t ExtTimestamp;
    fstat = ReadFile(&ExtTimestamp, sizeof(ExtTimestamp), 1, file);
    if (fstat == -1)
      return 1;

    unsigned int numChannels = NVAS * NCHAVA;
    uint32_t nFrames = messageLength - 10;
    if (nFrames != numChannels)
      std::cerr << "WARNING: payload length doesn't match number of channels in Event class (" << nFrames << " vs "
                << numChannels << ")\n";

    // helper function to unpack board data
    auto unpack_board_data = [this, event, iJinf](const auto &data, unsigned int iBoard) {
      unsigned int iTDRmax=0;
      for (unsigned int iVal = 0; iVal < data.size(); ++iVal) {
        // correct for endianess in the original file
        // iVal 0 -> iCh 1, iVal 1 -> iCh 0, iVal 2 -> iCh 3, iVal 3 -> iCh 2
        unsigned int iValNoEnd = (iVal % 2) ? iVal - 1 : iVal + 1;
        unsigned int iCh = (iValNoEnd / (2 * NADCS)) + (2 * NCHAVA) * (iValNoEnd % NADCS);
        unsigned int iTDR = 2 * iBoard + ((iValNoEnd / NADCS) % 2);
	if (iTDR>iTDRmax) {
	  iTDRmax=iTDR;
	  //	  printf("iTDRmax = %u\n", iTDRmax);
	}
	
        // TODO: check if signal is still zero-padded on both sides
        // both the shift and the m_adcUnits should be removed later on...
        event->RawSignal[iJinf][iTDR][iCh] = m_adcUnits * (data[iVal] >> 2);
	if (event->RawSignal[iJinf][iTDR][iCh]>(m_adcUnits*4095)) {
	  printf("event = %d, Jinf = %d, TDR = %d, Channel = %d) rawsignal = %d (%d)\n", event->Evtnum, iJinf, iTDR, iCh, event->RawSignal[iJinf][iTDR][iCh], data[iVal]);
	  //	  sleep(1);
	}
        event->RawSoN[iJinf][iTDR][iCh] =
            (event->RawSignal[iJinf][iTDR][iCh] / m_adcUnits - cals[iTDR].ped[iCh]) / cals[iTDR].sig[iCh];
      }
      return iTDRmax;
    };

    std::vector<uint16_t> adc_buf(2 * numChannels);
    fstat = ReadFile(adc_buf.data(), sizeof(decltype(adc_buf)::value_type) * adc_buf.size(), 1, file);
    if (fstat == -1)
      return 1;
    iTDRmax = unpack_board_data(adc_buf, iBoard);
    //    printf("iTDRmax = %d\n", iTDRmax);

    uint32_t bFooter;
    fstat = ReadFile(&bFooter, sizeof(bFooter), 1, file);
    if (fstat == -1 || bFooter != c_bFooter)
      return 1;

    uint32_t CRC;
    fstat = ReadFile(&CRC, sizeof(CRC), 1, file);
    if (fstat == -1)
      return 1;
  }
  
  // all good...
  return 0;
}

void DecodeDataOCA::InitHistos() {
  // taken from DecodeData. Hidden down here so I don't throw up every time I open this file
  hocc = new TH1F *[NJINF * NTDRS];
  hoccseed = new TH1F *[NJINF * NTDRS];
  hchargevsocc = new TH2F *[NJINF * NTDRS];
  hsignalvsocc = new TH2F *[NJINF * NTDRS];
  hcharge = new TH1F **[NJINF * NTDRS];
  hsignal = new TH1F **[NJINF * NTDRS];
  hson = new TH1F **[NJINF * NTDRS];
  for (size_t ii = 0; ii < NJINF * NTDRS; ii++) {
    hcharge[ii] = new TH1F *[2];
    hsignal[ii] = new TH1F *[2];
    hson[ii] = new TH1F *[2];
  }

  char name[255];
  for (size_t jj = 0; jj < NJINF; jj++) {
    for (size_t hh = 0; hh < NTDRS; hh++) {
      sprintf(name, "occ_%ld_%ld", jj, hh);
      //	  hocc[jj*NTDRS+hh]= new TH1F(name,name,1024,0,1024);
      hocc[jj * NTDRS + hh] = new TH1F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA);

      sprintf(name, "occseed_%ld_%ld", jj, hh);
      //	  hoccseed[jj*NTDRS+hh]= new TH1F(name,name,1024,0,1024);
      hoccseed[jj * NTDRS + hh] = new TH1F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA);

      sprintf(name, "qS_%ld_%ld", jj, hh);
      hcharge[jj * NTDRS + hh][0] = new TH1F(name, name, 1000, 0, 100);
      sprintf(name, "qK_%ld_%ld", jj, hh);
      hcharge[jj * NTDRS + hh][1] = new TH1F(name, name, 1000, 0, 100);

      sprintf(name, "signalS_%ld_%ld", jj, hh);
      hsignal[jj * NTDRS + hh][0] = new TH1F(name, name, 4200, -100, 4100);
      sprintf(name, "signalK_%ld_%ld", jj, hh);
      hsignal[jj * NTDRS + hh][1] = new TH1F(name, name, 4200, -100, 4100);

      sprintf(name, "q_vs_occ_%ld_%ld", jj, hh);
      hchargevsocc[jj * NTDRS + hh] = new TH2F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA, 1000, 0, 100);

      sprintf(name, "signal_vs_occ_%ld_%ld", jj, hh);
      hsignalvsocc[jj * NTDRS + hh] = new TH2F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 4100);

      sprintf(name, "sonS_%ld_%ld", jj, hh);
      hson[jj * NTDRS + hh][0] = new TH1F(name, name, 1000, 0, 100);
      sprintf(name, "sonK_%ld_%ld", jj, hh);
      hson[jj * NTDRS + hh][1] = new TH1F(name, name, 1000, 0, 100);
    }
  }
  std::cout << '\n';
}

int DecodeDataOCA::ReadOneEvent() {

  int iJinf = 0; // in the OCA case we have just one "collector" (the DAQ PC itself)

  // copy calibration data...
  for (unsigned int iBoard = 0; iBoard < 2 * m_numBoards; ++iBoard) {
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
      ev->CalPed[iJinf][iBoard][iCh] = cals[iBoard].ped[iCh];
      ev->CalSigma[iJinf][iBoard][iCh] = cals[iBoard].sig[iCh];
      ev->CalStatus[iJinf][iBoard][iCh] = cals[iBoard].status[iCh];
    }
  }

  int retVal = ReadOneEventFromFile(rawfile, ev);

  // for (unsigned int iTDR = 0; iTDR < NTDRS; ++iTDR) {
  //   for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
  //     if (ev->RawSignal[iJinf][iTDR][iCh]>(m_adcUnits*4095)) {
  // 	printf("event = %d, Jinf = %d, TDR = %d, Channel = %d) rawsignal = %d\n", ev->Evtnum, iJinf, iTDR, iCh, ev->RawSignal[iJinf][iTDR][iCh]);
  // 	sleep(1);
  //     }
  //   }
  // }

  // FIX ME [VF]: this should be done by the main! This function is called ReadOneEvent. It's done reading at this
  // point, so it should return.
  for (unsigned int iTDR = 0; iTDR < NTDRS; ++iTDR) {
    if (kClusterize) {
      Clusterize(iTDR, 0, ev, &cals[iTDR]);
    }
    else {
      FillRawHistos(iTDR, 0, ev, &cals[iTDR]); 
    }
  }

  return retVal;
};
