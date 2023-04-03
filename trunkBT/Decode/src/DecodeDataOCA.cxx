//
// Created by Valerio Formato on 12/10/21.
//

#include "DecodeDataOCA.hh"
#include "FSUtils.hh"

#include "GenericEvent.hpp"

#include <iomanip>
#include <numeric>
#include <sstream>

namespace {
constexpr auto NJINF = DecodeDataOCA::EventOCA::GetNJINF();
constexpr auto NTDRS = DecodeDataOCA::EventOCA::GetNTDRS();
constexpr auto NVAS = DecodeDataOCA::EventOCA::GetNVAS();
constexpr auto NCHAVA = DecodeDataOCA::EventOCA::GetNCHAVA();
constexpr auto NADCS = DecodeDataOCA::EventOCA::GetNADCS();
} // namespace

long int UnixTimeFromFilename(std::string m_filename) {

  long int ret = 0;

  std::istringstream is(m_filename);
  std::string part;
  std::string prefix1 = "SCD";
  std::string prefix2 = "RUN";
  std::string prefix3 = "BEAM";
  std::string prefix4 = "CAL";
  std::string suffix = ".dat";
  long int date = 0;
  long int time = 0;

  while (getline(is, part, '_')) {
    //    std::cout << part << std::endl;
    if (part.rfind(prefix1, 0) != 0 && part.rfind(prefix2, 0) != 0 && part.rfind(prefix3, 0) != 0 &&
        part.rfind(prefix4, 0) != 0) {
      //      std::cout << part << std::endl;
      if (part.find(suffix, 0) != std::string::npos) {
        //	printf("time\n");
        std::string stime = part.substr(0, part.length() - 4);
        time = atol(stime.c_str());
      } else {
        //	printf("date\n");
        date = atol(part.c_str());
      }
    }
  }

  std::istringstream ss(Form("%ld_%ld", date, time));
  std::tm t{};
  ss >> std::get_time(&t, "%Y%m%d_%H%M%S");
  if (ss.fail()) {
    throw std::runtime_error{"failed to parse time string"};
  }

  //  printf("%ld\n", mktime(&t));
  ret = mktime(&t);
  //  printf("%ld\n", ret);

  return ret;
}

char *DateFromFilename(std::string m_filename) {

  std::istringstream is(m_filename);
  std::string part;
  std::string prefix1 = "SCD";
  std::string prefix2 = "RUN";
  std::string prefix3 = "BEAM";
  std::string prefix4 = "CAL";
  std::string suffix = ".dat";
  long int date = 0;
  long int time = 0;

  while (getline(is, part, '_')) {
    //    std::cout << part << std::endl;
    if (part.rfind(prefix1, 0) != 0 && part.rfind(prefix2, 0) != 0 && part.rfind(prefix3, 0) != 0 &&
        part.rfind(prefix4, 0) != 0) {
      //      std::cout << part << std::endl;
      if (part.find(suffix, 0) != std::string::npos) {
        std::string stime = part.substr(0, part.length() - 4);
        time = atol(stime.c_str());
        //	printf("time: %s -> %s --> %ld\n", part.c_str(), stime.c_str(), time);
      } else {
        date = atol(part.c_str());
        //	printf("date: %s -> %ld\n", part.c_str(), date);
      }
    }
  }

  std::istringstream ss(Form("%08ld_%06ld", date, time));
  std::tm t{};
  ss >> std::get_time(&t, "%Y%m%d_%H%M%S");
  if (ss.fail()) {
    throw std::runtime_error{"failed to parse time string"};
  }

  //  printf("%s", asctime(&t));
  char *ret = asctime(&t);

  return ret;
}

DecodeDataOCA::DecodeDataOCA(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int calNum)
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
  m_adcUnits = 1.0;

  tdrMap = new laddernumtype[NJINF * NTDRS];

  pri = false;

  // Create the ROOT run header
  rh = new RHClassOCA();

  ev = new EventOCA((char *)"ladderconf_OCA.dat", (char *)"gaincorrection_OCA.dat");
  //  std::cout << "ev: " << ev << '\n';

  DecodeDataOCA::OpenFile(m_rawDir.c_str(), m_calDir.c_str(), runn, int(calNum));
  // we assume we also have the corresponding calibration file
  std::cout << "Raw file: " << m_filename << '\n';
  std::cout << "Calibration file: " << m_calFilename << '\n';

  std::string filePath = m_rawDir + "/" + m_filename;
  rawfile = fopen(filePath.c_str(), "r");
  if (!rawfile) {
    printf("Error file %s not found \n", filePath.c_str());
    exit(2);
  }

  if (!ReadFileHeader(rawfile, rh)) {
    throw std::runtime_error("Failed to read MAKA run header");
  }

  //  long int runnum = UnixTimeFromFilename(m_filename);
  char *date = DateFromFilename(m_filename);
  rh->SetRun(runNum);
  rh->SetDate(date);

  // we assume that from now on we know how many boards are in the DAQ
  ntdrRaw = NTDRS;
  ntdrCmp = 0;
  for (unsigned int iTdr = 0; iTdr < NJINF * NTDRS; ++iTdr) {
    tdrMap[iTdr] = {iTdr, 0}; // putting type at 0 since they're all RAW, so far...
  }

  // Jinf has no real meaning
  nJinf = 1;
  JinfMap[0] = 1;

  // Update the ROOT run header
  rh->SetJinfMap(JinfMap);
  rh->SetNTdrsRaw(ntdrRaw);
  rh->SetNTdrsCmp(ntdrCmp);
  rh->SetTdrMap(tdrMap);

  DecodeDataOCA::DumpRunHeader();

  ProcessCalibration();

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
}

void DecodeDataOCA::OpenFile(const char *rawDir, const char *calDir, int runNum, int calNum) {
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

  unsigned int calNumLenght = 36;

  if (calNum > 0) {
    auto calFilename_it = std::find_if(begin(fileList), end(fileList), [calNum, calNumLenght](const std::string &_filename) {
      // all our files begin with 'SCD_RUN' followed by zero-padded run numbers
	//	printf("_filename = %s\n", _filename.c_str());
	// the substr below can throw an error with filename, in the same dir, like 171_0000.cal (that shouldn't be there, but...). So I added this check on the filename size
	if (_filename.length()!=calNumLenght) {
	  //	  printf("_filename.length() = %d\n", (int)_filename.length());
	  return false;
	}
	bool is_cal = _filename.substr(13, 3) == "CAL";
	unsigned int runNum = std::atoi(_filename.substr(7, 5).c_str());
	//	printf("runNum: %u\n", runNum);
	return is_cal && (runNum == static_cast<unsigned int>(calNum));
      });
    
    if (calFilename_it != end(fileList)) {
      m_calFilename = *calFilename_it;
    }
  } else {
    auto calFilename_it = std::find_if(std::reverse_iterator<decltype(fileName_it)>(fileName_it), rend(fileList),
                                       [calNumLenght](const std::string &_filename) {
					 return _filename.length()==calNumLenght && _filename.substr(13, 3) == "CAL";
				       });

    if (calFilename_it != rend(fileList)) {
      m_calFilename = *calFilename_it;
    }
  }
}

void DecodeDataOCA::DumpRunHeader() {
  printf("There're %d boards (%d detectors)\n", m_numBoards, 2 * m_numBoards);

  if (pri) {
    printf("********* READVALS: %ld %d %d %d \n", NTDRS, nJinf, ntdrRaw, ntdrCmp);
    printf("Dumping the file headers that are going to be written in the ROOT files...\n");
  }
  rh->Print();
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

  auto cal_rh = std::make_unique<RHClassOCA>();
  if (!ReadFileHeader(calfile, cal_rh.get()))
    throw std::runtime_error("Failed to read MAKA file header from calibration file");

  std::cout << "Calibration file:\n";
  cal_rh->Print();

  printf("Processing calibration (%s)... \n", calFilePath.c_str());
  auto start = std::chrono::system_clock::now();

  auto event = std::make_unique<EventOCA>((char *)"ladderconf_OCA.dat", (char *)"gaincorrection_OCA.dat");
  std::vector<std::vector<std::vector<float>>> signals(NTDRS, std::vector<std::vector<float>>(NVAS * NCHAVA));

  // #define CALPLOTS // in generale deve stare spento, a differenza di AMS non abbiamo i cluster, quando fa il decode
  // qui fa qualche plot di occupancy.

  // FIXME: Some test calibrations contain too many events, stop at 10k and use the first half for ped and sigma raw,
  // and the second half for sigma
  // MD: tipo che MD: fai vector reserved da 5k MD: inizi a leggere e vedi a quanto arrivi
  // MD: se <5k resizi
  // MD: se sono più a 5k smetti e fai mean e sigma_raw
  // MD: e poi ricominci (fino a massimo 5k) fillando 0, 1, 2, etc... fino a dove arrivi
  // MD: se sono più di 10k li hai sostituiti tutti

  unsigned int nEvents{0};
  while (!feof(calfile) && nEvents < 10000) {
    // while (nEvents<1000) {
    ReadOneEventFromFile(calfile, event.get());
    nEvents++;
    // std::cout << "\rRead " << nEvents << " events" << std::flush;
    std::cout << "\rRead " << nEvents << " events" << " (found " << m_numBoardsFound << " boards)" << std::flush;

    for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
      for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
        signals[iTdr][iCh].push_back(event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
        //	printf("%d (%d %d)) %hd %f -> %f\n", nEvents, iTdr, iCh, event->RawSignal[iJinf][iTdr][iCh], m_adcUnits,
        // event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
      }
    }
  }
  std::cout << '\n';

  //----------------------------------
  ComputeCalibration<EventOCA, calibOCA>(signals, cals);

  auto stop = std::chrono::system_clock::now();
  std::cout << "DecodeDataOCA::ProcessCalibration took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms\n";

  char calfileprefix[255];
  sprintf(calfileprefix, "%s/%ld", m_calDir.c_str(), runnum);
  //  printf("calfileprefix: %s\n", calfileprefix);

  //  printf("m_numBoardsFound: %d\n", m_numBoardsFound);
  //  for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
  for (unsigned int iTdr = 0; iTdr < 2 * m_numBoardsFound; ++iTdr) {
//    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
//      printf("%d %d %d %lf %f %f %f %d\n", iCh + 1, (1 + (int)(iCh / NCHAVA)), (1 + (int)((iCh) % NCHAVA)),
//             cals[iTdr].ped[iCh], cals[iTdr].rsig[iCh], cals[iTdr].sig[iCh], 0.0, 0);
//    }

    char calfilename[255];
    sprintf(calfilename, "%s_%02d%02d.cal", calfileprefix, iJinf, iTdr);
    printf("calfilename: %s\n", calfilename);
    FILE *calfil = fopen(calfilename, "w");
    if (!calfil) {
      printf("problem in opening the %s cal file...\n", calfilename);
      return true;
    }

    // writing the common noise (average?)
    for (unsigned int iVa = 0; iVa < NVAS; ++iVa) {
      fprintf(calfil, "%02d,\t%lf,\t%lf\n", iVa, 0.0, 0.0);
    }

    // reading channels
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
      fprintf(calfil, "%d %d %d %lf %f %f %f %d\n", iCh + 1, (1 + (int)(iCh / NCHAVA)), (1 + (int)((iCh) % NCHAVA)),
              cals[iTdr].ped[iCh], cals[iTdr].rsig[iCh], cals[iTdr].sig[iCh], 0.0, 0);
    }

    fclose(calfil);
  }

  return true;
}

bool DecodeDataOCA::ReadFileHeader(FILE *file, RHClassOCA *rhc) {
  constexpr uint32_t c_bRunHeader = 0xb01adeee;

  int fstat = 0;

  uint32_t bRunHeader;
  fstat = ReadFile(&bRunHeader, sizeof(bRunHeader), 1, file);
  if (fstat == -1 || bRunHeader != c_bRunHeader) {
    printf("Mismatch in run header %x (expected %x)\n", bRunHeader, c_bRunHeader);
    return false;
  }

  uint32_t runUnixTime;
  fstat = ReadFile(&runUnixTime, sizeof(runUnixTime), 1, file);
  if (fstat == -1)
    return false;
  rhc->SetUnixTime(runUnixTime);

  uint32_t gitSHA;
  fstat = ReadFile(&gitSHA, sizeof(gitSHA), 1, file);
  if (fstat == -1)
    return false;
  std::stringstream stream;
  stream << std::hex << gitSHA;
  rhc->SetGitSHA(stream.str());

  uint32_t dummy{0};
  fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
  if (fstat == -1)
    return false;
  unsigned int numBoards = dummy & 0xFF;
  dummy >>= 16;
  unsigned int patch = dummy & 0xF;
  dummy >>= 4;
  unsigned int minor = dummy & 0xF;
  dummy >>= 4;
  unsigned int major = dummy & 0xF;
  dummy >>= 4;
  std::bitset<4> runtype = dummy & 0xF;
  // make sure only one bit is set
  if (runtype.count() != 1) {
    throw std::runtime_error("More than one bit set for runtype");
  }
  rhc->SetRunType(static_cast<RHClassOCA::RunType>(runtype.to_ulong()));

  rhc->SetNumBoards(numBoards);
  rhc->SetDataVersion(major, minor, patch);

  m_numBoards = numBoards;

  for (unsigned int i = 0; i < numBoards; ++i) {
    uint16_t boardID;
    fstat = ReadFile(&boardID, sizeof(boardID), 1, file);
    if (fstat == -1)
      return false;
    rhc->AddBoardID(boardID);
  }

  return true;
};

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

  uint64_t timestamp;
  fstat = ReadFile(&timestamp, sizeof(timestamp), 1, file);
  if (fstat == -1) {
    return 1;
  }
  event->TimeStamp = timestamp;

  uint64_t timestamp_ns;
  fstat = ReadFile(&timestamp_ns, sizeof(timestamp_ns), 1, file);
  if (fstat == -1) {
    return 1;
  }
  event->TimeStamp_ns = timestamp_ns;

  uint32_t evLen;
  fstat = ReadFile(&evLen, sizeof(evLen), 1, file);
  if (fstat == -1) {
    return 1;
  }

  uint32_t evNum;
  fstat = ReadFile(&evNum, sizeof(evNum), 1, file);
  if (fstat == -1) {
    return 1;
  }
  // std::cout << "MAKA event number: " << evNum << '\n';

  uint16_t num_boards;
  fstat = ReadFile(&num_boards, sizeof(num_boards), 1, file);
  if (fstat == -1) {
    return 1;
  }
  m_numBoardsFound = num_boards;

  uint16_t dummy;
  fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
  if (fstat == -1) {
    return 1;
  }
  uint16_t status = dummy & 0xFFF;
  dummy >>= 12;
  char type = dummy & 0xF;

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
    IntTimestamp = (IntTimestamp >> 32) + ((IntTimestamp &  0xFFFFFFFF) << 32);
    // FIXME: we save only the first board clock
    if (iBoard == 0)
      event->I2CEventID = IntTimestamp >> 32;

    uint64_t ExtTimestamp;
    fstat = ReadFile(&ExtTimestamp, sizeof(ExtTimestamp), 1, file);
    if (fstat == -1)
      return 1;
    ExtTimestamp = (ExtTimestamp >> 32) + ((ExtTimestamp & 0xFFFFFFFF) << 32);

    // FIXME: we save only the first board clock
    if (iBoard == 0)
      event->ExtTimeStamp = double(ExtTimestamp) / 1e6; // assume 1 Mhz clock

    unsigned int numChannels = NVAS * NCHAVA;
    uint32_t nFrames = messageLength - 10;
    if (nFrames != numChannels)
      std::cerr << "WARNING: payload length doesn't match number of channels in Event class (" << nFrames << " vs "
                << numChannels << ")\n";

    // helper function to unpack board data
    auto unpack_board_data = [this, event, iJinf](const auto &data, unsigned int iBoard) {
      unsigned int iTDRmax = 0;
      for (unsigned int iVal = 0; iVal < data.size(); ++iVal) {
        // correct for endianess in the original file
        // iVal 0 -> iCh 1, iVal 1 -> iCh 0, iVal 2 -> iCh 3, iVal 3 -> iCh 2
        unsigned int iValNoEnd = (iVal % 2) ? iVal - 1 : iVal + 1;
        unsigned int iCh = (iValNoEnd / (2 * NADCS)) + (2 * NCHAVA) * (iValNoEnd % NADCS);
        unsigned int iTDR = 2 * iBoard + ((iValNoEnd / NADCS) % 2);

        // TODO: check if signal is still zero-padded on both sides
        // both the shift and the m_adcUnits should be removed later on...
        event->RawSignal[iJinf][iTDR][iCh] = m_adcUnits * (data[iVal] >> 2);
        if (event->RawSignal[iJinf][iTDR][iCh] > (m_adcUnits * 4095)) {
          printf("event = %d, Jinf = %d, TDR = %d, Channel = %d) rawsignal = %d (%d)\n", event->Evtnum, iJinf, iTDR,
                 iCh, event->RawSignal[iJinf][iTDR][iCh], data[iVal]);
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
    unpack_board_data(adc_buf, iBoard);
    //    printf("iTDRmax = %d\n", iTDRmax);

    uint32_t bFooter;
    fstat = ReadFile(&bFooter, sizeof(bFooter), 1, file);
    if (fstat == -1 || bFooter != c_bFooter)
      return 1;

    uint32_t CRC;
    fstat = ReadFile(&CRC, sizeof(CRC), 1, file);
    if (fstat == -1)
      return 1;

    // std::cout << "Board trigger number: " << TriggerNumber << " " << std::hex << IntTimestamp << '\n' << std::dec;
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
  // 	printf("event = %d, Jinf = %d, TDR = %d, Channel = %d) rawsignal = %d\n", ev->Evtnum, iJinf, iTDR, iCh,
  // ev->RawSignal[iJinf][iTDR][iCh]); 	sleep(1);
  //     }
  //   }
  // }

  // FIX ME [VF]: this should be done by the main! This function is called ReadOneEvent. It's done reading at this
  // point, so it should return.
  for (unsigned int iTDR = 0; iTDR < NTDRS; ++iTDR) {
    if (kClusterize) {
      Clusterize(iTDR, 0, ev, &cals[iTDR]);
    } else {
      FillRawHistos(iTDR, 0, ev, &cals[iTDR]);
    }
  }

  return retVal;
}
