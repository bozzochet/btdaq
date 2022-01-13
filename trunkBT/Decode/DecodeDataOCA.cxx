//
// Created by Valerio Formato on 12/10/21.
//

#include "DecodeDataOCA.hh"
#include "FSUtils.hh"

#include "GenericEvent.hpp"

#include <numeric>

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
  kClusterize = true;

  m_defaultShift = 0;
  m_defaultArraySize = NVAS * NCHAVA;

  tdrMap = new laddernumtype[NJINF * NTDRS];

  pri = false;

  ev = new EventOCA();

  std::cout << "ev: " << ev << '\n';

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
  // TODO: remove later
  m_numBoards = 6;
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

  // open the calibration file
  std::string calFilePath = m_rawDir + "/" + m_calFilename;
  calfile = fopen(calFilePath.c_str(), "r");
  if (!calfile) {
    printf("Error file %s not found \n", calFilePath.c_str());
    exit(2);
  }

  std::cout << "Processing calibration... \n";
  auto start = std::chrono::system_clock::now();

  auto event = std::make_unique<EventOCA>();
  std::vector<std::vector<std::vector<float>>> signals(NTDRS, std::vector<std::vector<float>>(NVAS * NCHAVA));
  // std::vector<std::vector<std::vector<float> > > signals;
  // signals.resize(NTDRS);
  // for (int ii=0; ii<NTDRS; ii++) {
  //   signals[ii].resize(NVAS * NCHAVA);
  // }

  unsigned int nEvents{0};
  while (!feof(calfile)) {
    ReadOneEventFromFile(calfile, event.get());
    std::cout << "\rRead " << ++nEvents << " events" << std::flush;

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
  // std::vector<std::vector<unsigned int> > processed_events;
  // processed_events.resize(NTDRS);
  // for (int ii=0; ii<NTDRS; ii++) {
  //   processed_events[ii].resize(NVAS * NCHAVA);
  // }
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
  std::cout << "DecodeDataOCA::ProcessCalibration took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms\n";
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
    ev->Evtnum = TriggerNumber;

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
    if (fstat == -1)
      return 1;
    unpack_board_data(adc_buf, iBoard);

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

  // clusterize!
  // FIX ME [VF]: this should be done by the main! This function is called ReadOneEvent. It's done reading at this
  // point, so it should return.
  for (unsigned int iTDR = 0; iTDR < NTDRS; ++iTDR) {
    Clusterize(iTDR, 0, ev, &cals[iTDR]);
  }

  return retVal;
};
