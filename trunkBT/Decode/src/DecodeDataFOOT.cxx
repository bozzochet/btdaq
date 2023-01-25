//
// Derived from DECODEDATAOCA by Valerio Formato on 15/11/21.
//

#include "DecodeDataFOOT.hh"
#include "FSUtils.hh"

#include "GenericEvent.hpp"

#include <numeric>
#include <unistd.h>

namespace {
constexpr auto NJINF = DecodeDataFOOT::EventFOOT::GetNJINF();
constexpr auto NTDRS = DecodeDataFOOT::EventFOOT::GetNTDRS();
constexpr auto NVAS = DecodeDataFOOT::EventFOOT::GetNVAS();
constexpr auto NCHAVA = DecodeDataFOOT::EventFOOT::GetNCHAVA();
constexpr auto NADCS = DecodeDataFOOT::EventFOOT::GetNADCS();
} // namespace

DecodeDataFOOT::DecodeDataFOOT(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int calNum)
    : m_rawDir{std::move(rawDir)}, m_calDir{std::move(calDir)} {

  // Init base-class members
  kMC = false;
  runn = runNum;
  shighthreshold = 3.5;
  slowthreshold = 1.0;
  khighthreshold = 3.5;
  klowthreshold = 1.0;
  kClusterize = true;

  m_defaultShift = 0;
  m_defaultArraySize = NVAS * NCHAVA;

  pri = true;
  int ret;

  ev = new EventFOOT("laddercorrection_FOOT.dat", "gaincorrection_FOOT.dat");

  DecodeDataFOOT::OpenFile(m_rawDir.c_str(), m_calDir.c_str(), runn, int(calNum));
  // we assume we also have the corresponding calibration file
  std::cout << "Raw file: " << m_filename << '\n';
  std::cout << "Calibration file: " << m_calFilename << '\n';

  std::string filePath = m_rawDir + "/" + m_filename;
  rawfile = fopen(filePath.c_str(), "r");
  if (!rawfile) {
    printf("Error file %s not found \n", filePath.c_str());
    exit(2);
  }

  DecodeDataFOOT::DumpRunHeader();
  // we assume that from now on we know how many boards are in the DAQ
  ntdrRaw = NTDRS;
  ntdrCmp = 0;

  // VF: create a fake trdMap. I still don't know what it is used for and I don't wanna know why it's an array of pairs
  for (unsigned int iTdr = 0; iTdr < NJINF * NTDRS; ++iTdr) {
    tdrMap[iTdr] = {iTdr, 1};
  }

  ret = ProcessCalibration();

  if (ret ==
      -1) // We needed to compute the calibration but we had an error on the last word read, need to go back 4 bytes
  {
    fseek(rawfile, -4, SEEK_CUR);
  }

  // TODO: add setup of histograms
  InitHistos();
}

void DecodeDataFOOT::OpenFile(const char *rawDir, const char *calDir, int runNum, int calNum) {
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

  if (calNum > 0) {
    auto calFilename_it = std::find_if(begin(fileList), end(fileList), [calNum](const std::string &_filename) {
	// all our files begin with 'SCD_RUN' followed by zero-padded run numbers
        // the substr below can throw an error with filename, in the same dir, like 171_0000.cal (that shouldn't be there, but...).
	// In DecodeDataOCA I added a check on the filename size
	bool is_cal = _filename.substr(13, 3) == "CAL";
	unsigned int runNum = std::atoi(_filename.substr(7, 5).c_str());
	return is_cal && (runNum == static_cast<unsigned int>(calNum));
      });

    if (calFilename_it != end(fileList)) {
      m_calFilename = *calFilename_it;
    }
  } else {
    auto calFilename_it = std::find_if(std::reverse_iterator<decltype(fileName_it)>(fileName_it), rend(fileList),
                                       [](const std::string &_filename) { return _filename.substr(13, 3) == "CAL"; });

    if (calFilename_it != rend(fileList)) {
      m_calFilename = *calFilename_it;
    }
  }
}

int DecodeDataFOOT::GetTdrNum(size_t pos) {
  if (pos > NJINF * NTDRS) {
    printf("Pos %ld not allowed. Max is %ld\n", pos, NJINF * NTDRS);
    return -9999;
  }
  return tdrMap[pos].first;
}

int DecodeDataFOOT::GetTdrType(size_t pos) {
  if (pos > NJINF * NTDRS) {
    printf("Pos %ld not allowed. Max is %ld\n", pos, NJINF * NTDRS);
    return -9999;
  }
  return tdrMap[pos].second;
}

// TODO: waiting for run header definition
void DecodeDataFOOT::DumpRunHeader() {
  // TODO: remove later
  m_numBoards = 6;

  constexpr uint32_t c_bRunHeader = 0x1234aaaa; // FOOT Bo TDAQ file header

  int fstat = 0;

  uint32_t bRunHeader;
  fstat = ReadFile(&bRunHeader, sizeof(bRunHeader), 1, rawfile);
  // printf("\tJust read run header %x\n", bRunHeader);

  if (fstat < 0 || bRunHeader != c_bRunHeader) {
    printf("Mismatch in run header %x (expected %x)\n", bRunHeader, c_bRunHeader);
  }
  fseek(rawfile, 280, SEEK_CUR); // Skipping the other file header words for now
}

// TODO: read calib events, compute mean and sigma
int DecodeDataFOOT::ProcessCalibration() {

  int iJinf = 0; // in the FOOT case we have just one "collector" (the DAQ PC itself)

  // open the calibration file
  std::string calFilePath = m_rawDir + "/" + m_calFilename;
  calfile = fopen(calFilePath.c_str(), "r");
  if (!calfile) {
    printf("Error file %s not found \n", calFilePath.c_str());
    exit(2);
  }

  std::cout << "Processing calibration... \n";
  auto start = std::chrono::system_clock::now();

  auto event = std::make_unique<EventFOOT>((char *)"ladderconf_FOOT.dat", (char *)"gaincorr_FOOT.dat");
  std::vector<std::vector<std::vector<float>>> signals(NTDRS, std::vector<std::vector<float>>(NVAS * NCHAVA));

  unsigned int nEvents{0};
  fseek(calfile, 280, SEEK_CUR); // Skipping the file header
  int ret = 0;

  while (!feof(calfile)) {
    ret = ReadOneEventFromFile(calfile, event.get());
    if (ret)
      break;

    std::cout << "\rRead " << ++nEvents << " events " << std::flush;
    for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
      for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
        signals[iTdr][iCh].push_back(event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
      }
    }
  }
  std::cout << '\n';

  for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
      cals[iTdr].ped[iCh] = std::accumulate(begin(signals[iTdr][iCh]), end(signals[iTdr][iCh]), 0.0f) /
                            static_cast<float>(signals[iTdr][iCh].size());
      cals[iTdr].rsig[iCh] =
          std::sqrt(std::accumulate(begin(signals[iTdr][iCh]), end(signals[iTdr][iCh]), 0.0f,
                                    [&](float acc, float curr) {
                                      return acc + (curr - cals[iTdr].ped[iCh]) * (curr - cals[iTdr].ped[iCh]);
                                    }) /
                    static_cast<float>(signals[iTdr][iCh].size()));
      // initialize this for later
      cals[iTdr].sig[iCh] = 0;
      cals[iTdr].status[iCh] = 0;
    }
  }

  unsigned int lastVA = std::numeric_limits<unsigned int>::max();
  std::vector<float> common_noise(NVAS);
  std::vector<std::vector<unsigned int>> processed_events(NTDRS, std::vector<unsigned int>(NVAS * NCHAVA));
  for (unsigned int iEv = 0; iEv < signals[0][0].size(); ++iEv) {
    for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
      for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
        unsigned int thisVA = iCh / NCHAVA;
        if (thisVA != lastVA) {

          std::vector<float> values(NCHAVA);
          for (unsigned int iVACh = 0; iVACh < NCHAVA; ++iVACh) {
            values[iVACh] = signals[iTdr][thisVA * NCHAVA + iVACh][iEv] - cals[iTdr].ped[thisVA * NCHAVA + iVACh];
          }

          // get the median
          std::sort(begin(values), end(values));
          common_noise[thisVA] = 0.5 * (values[(NCHAVA / 2) - 1] + values[NCHAVA / 2]);
        }

        if (std::fabs(common_noise[thisVA]) > 10) {
          continue;
        }
        ++processed_events[iTdr][iCh];

        cals[iTdr].sig[iCh] += (signals[iTdr][iCh][iEv] - cals[iTdr].ped[iCh] - common_noise[thisVA]) *
                               (signals[iTdr][iCh][iEv] - cals[iTdr].ped[iCh] - common_noise[thisVA]);
        lastVA = thisVA;
      }
    }
  }
  for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
      cals[iTdr].sig[iCh] = sqrt(cals[iTdr].sig[iCh] / static_cast<float>(processed_events[iTdr][iCh]));
    }
  }

  auto stop = std::chrono::system_clock::now();
  std::cout << "DecodeDataFOOT::ProcessCalibration took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms\n";

  if (ret) {
    return -1;
  } else {
    return 1;
  }
}

// TODO: read event data.
int DecodeDataFOOT::ReadOneEventFromFile(FILE *file, EventFOOT *event) {

  int iJinf = 0; // in the OCA case we have just one "collector" (the DAQ PC itself)

  constexpr uint32_t c_bEvHeader = 0x1234cccc;
  constexpr uint32_t c_bDetHeader = 0xeadebaba;
  constexpr uint32_t c_bDe10Header = 0xbadcafe0;
  constexpr uint32_t c_bDetFooter = 0xfafefafe;
  constexpr uint32_t c_bDe10Footer = 0xbacca000;
  // constexpr uint32_t c_bEvFooter = 0x12341234;
  // constexpr uint32_t c_bFileFooter = 0x1234eeee;

  int fstat;

  uint32_t bEvHeader;
  fstat = ReadFile(&bEvHeader, sizeof(bEvHeader), 1, file);
  // printf("\tJust read %x in ReadOneEvent at position %li\n", bEvHeader, ftell(file) - 4);
  if (fstat < 0 || bEvHeader != c_bEvHeader) {
    printf("\nMismatch in event header %x (expected %x), event is probably corrupted.\n", bEvHeader, c_bEvHeader);
    // TODO: looks like the last event is missing some data (trigger signal while stopping the DAQ?), need to check if
    // this also happens during a run
    return 1;
  }

  unsigned int iBoard = 0;
  bool found;

  while (iBoard < m_numBoards) {

    // printf("Ready to read one of the boards, I'm at position %li\n", ftell(file));
    found = false;
    uint32_t bDetHeader;
    while (!found && !feof(file)) {
      fstat = ReadFile(&bDetHeader, sizeof(bDetHeader), 1, file);
      if (fstat < 0 || bDetHeader != c_bDetHeader) {
        continue;
      } else {
        found = true;
      }
    }

    if (feof(file))
      return 1;

    found = false;
    uint32_t bDe10Header;
    while (!found && !feof(file)) {
      fstat = ReadFile(&bDe10Header, sizeof(bDe10Header), 1, file);
      if (fstat < 0) {
        return 1;
      } else if ((bDe10Header & 0xfffffff0) == c_bDe10Header) {
        found = true;
      } else if (bDe10Header != c_bDe10Header && bDe10Header == c_bEvHeader) {
        // We have less boards than expected? Stop reading this event and get ready for the next one
        // but first reset the file cursor back so that on the next call we'll read the event header
        printf("Unexpected end of event: only found %i boards of the expected %i", iBoard, m_numBoards);
        fseek(file, -sizeof(decltype(c_bEvHeader)), SEEK_CUR);
        return 0;
      }
    }

    if (feof(file))
      return 1;

    fseek(file, 44, SEEK_CUR); // Skipping to the start of the payload
    uint32_t messageLength = 640;

    unsigned int numChannels = NVAS * NCHAVA;
    uint32_t nFrames = messageLength;
    if (nFrames != numChannels)
      std::cerr << "WARNING: payload length doesn't match number of channels in Event class (" << nFrames << " vs "
                << numChannels << ")\n";

    // helper function to unpack board data
    auto unpack_board_data = [this, event, iJinf](const auto &data, unsigned int iBoard) {
      for (unsigned int iVal = 0; iVal < data.size(); ++iVal) {
        // correct for endianess in the original file
        // iVal 0 -> iCh 1, iVal 1 -> iCh 0, iVal 2 -> iCh 3, iVal 3 -> iCh 2
        unsigned int iValNoEnd = (iVal % 2) ? iVal - 1 : iVal + 1;
        unsigned int iCh = (iValNoEnd / (2 * NADCS)) + (2 * NCHAVA) * (iValNoEnd % NADCS);
        unsigned int iTDR = 2 * iBoard + ((iValNoEnd / NADCS) % 2);

        // TODO: check if signal is still zero-padded on both sides
        // both the shift and the m_adcUnits should be removed later on...
        event->RawSignal[iJinf][iTDR][iCh] = m_adcUnits * (data[iVal] >> 2);
        event->RawSoN[iJinf][iTDR][iCh] =
            (event->RawSignal[iJinf][iTDR][iCh] / m_adcUnits - cals[iTDR].ped[iCh]) / cals[iTDR].sig[iCh];
      }
    };

    std::vector<uint16_t> adc_buf(2 * numChannels);
    fstat = ReadFile(adc_buf.data(), sizeof(decltype(adc_buf)::value_type) * adc_buf.size(), 1, file);
    if (fstat < 0)
      return 1;
    unpack_board_data(adc_buf, iBoard);
    iBoard++;

    uint32_t bDetFooter;
    fstat = ReadFile(&bDetFooter, sizeof(bDetFooter), 1, file);
    if (fstat < 0) {
      return 1;
    } else if (bDetFooter != c_bDetFooter && bDetFooter == c_bEvHeader) {
      // We have less boards than expected? Stop reading this event and get ready for the next one
      // but first reset the file cursor back so that on the next call we'll read the event header
      printf("Unexpected end of event: only found %i boards of the expected %i", iBoard, m_numBoards);
      fseek(file, -sizeof(decltype(c_bEvHeader)), SEEK_CUR);
      return 0;
    }

    uint32_t bDe10Footer;
    fstat = ReadFile(&bDe10Footer, sizeof(bDe10Footer), 1, file);
    if (fstat < 0) {
      return 1;
    } else if (bDe10Footer != c_bDe10Footer && bDe10Footer == c_bEvHeader) {
      // We have less boards than expected? Stop reading this event and get ready for the next one
      // but first reset the file cursor back so that on the next call we'll read the event header
      printf("Unexpected end of event: only found %i boards of the expected %i", iBoard, m_numBoards);
      fseek(file, -sizeof(decltype(c_bEvHeader)), SEEK_CUR);
      return 0;
    }
  }
  fseek(file, 36, SEEK_CUR); // Skipping other footer words

  // all good...
  return 0;
}

void DecodeDataFOOT::InitHistos() {
  // taken from DecodeData. Hidden down here so I don't throw up every time Iopen this file

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
int DecodeDataFOOT::ReadOneEvent() {

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

  if (kClusterize) {
    // clusterize!
    // FIX ME [VF]: this should be done by the main! This function is called ReadOneEvent. It's done reading at this
    // point, so it should return.
    for (unsigned int iTDR = 0; iTDR < NTDRS; ++iTDR) {
      printf("Cluterizing detector %i\n", iTDR);
      Clusterize(iTDR, 0, ev, &cals[iTDR]);
    }
  }

  return retVal;
};
