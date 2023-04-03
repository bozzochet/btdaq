//
// Created by Valerio Formato on 21/11/22.
//

#include "DecodeDataAMSL0.hh"
#include "FSUtils.hh"

#include "GenericEvent.hpp"

#include <algorithm>

namespace {
constexpr auto NJINF = DecodeDataAMSL0::EventAMSL0::GetNJINF();
constexpr auto NTDRS = DecodeDataAMSL0::EventAMSL0::GetNTDRS();
constexpr auto NVAS = DecodeDataAMSL0::EventAMSL0::GetNVAS();
constexpr auto NCHAVA = DecodeDataAMSL0::EventAMSL0::GetNCHAVA();
constexpr auto NADCS = DecodeDataAMSL0::EventAMSL0::GetNADCS();
} // namespace

struct AMSRawFile {
  std::string path;
  unsigned int dirNum;
  unsigned int blockNum;

  std::string DirPath() const {
    std::array<char, 5> dirNum_s{};
    std::sprintf(dirNum_s.data(), "%04d", dirNum);

    std::string dir_path = path;
    dir_path += "/" + std::string{dirNum_s.data()};
    return dir_path;
  };

  std::string FilePath() const {
    std::array<char, 4> blockNum_s{};
    std::sprintf(blockNum_s.data(), "%03d", blockNum);

    std::string full_path = DirPath();
    full_path += "/" + std::string{blockNum_s.data()};
    return full_path;
  };

  AMSRawFile &operator++() {
    blockNum = (blockNum + 1) % 1000;
    if (blockNum == 0)
      dirNum = (dirNum + 1) % 10000;

    return *this;
  }

  AMSRawFile operator++(int) {
    auto current = *this;

    blockNum = (blockNum + 1) % 1000;
    if (blockNum == 0)
      dirNum = (dirNum + 1) % 10000;

    return current;
  }
};

bool operator==(const AMSRawFile &lhs, const AMSRawFile &rhs) {
  return (lhs.path == rhs.path) && (lhs.dirNum == rhs.dirNum) && (lhs.blockNum == rhs.blockNum);
}

bool operator<(const AMSRawFile &lhs, const AMSRawFile &rhs) {
  return (lhs.dirNum < rhs.dirNum) || (lhs.dirNum == rhs.dirNum && lhs.blockNum < rhs.blockNum);
}

bool operator<=(const AMSRawFile &lhs, const AMSRawFile &rhs) { return lhs < rhs || lhs == rhs; }

DecodeDataAMSL0::DecodeDataAMSL0(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int runStop,
                                 unsigned int calStart, unsigned int calStop)
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
  rh = new RHClassAMSL0();

  ev = new EventAMSL0((char *)"ladderconf_L0.dat", (char *)"gaincorrection_L0.dat");

  DecodeDataAMSL0::OpenFile(m_rawDir.c_str(), m_calDir.c_str(), runNum, runStop, calStart, calStop);

  // FIXME: I don't think this info is available for AMSL0 files...
  // char *date = DateFromFilename(m_filename);
  // rh->SetRun(runNum);
  // rh->SetDate(date);

  // FIXME: This is where we would read a run header if we had one...
  // if (!ReadFileHeader(rawfile, rh)) {
  //   throw std::runtime_error("Failed to read AMSL0 run header");
  // }

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

  // what is a run header for AMSL0 anyway?
  DecodeDataAMSL0::DumpRunHeader();

  ProcessCalibration();

  InitHistos();
}

DecodeDataAMSL0::~DecodeDataAMSL0() {

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

void DecodeDataAMSL0::OpenFile(const char *rawDir, const char *calDir, int runStart, int runStop, int calNumStart,
                               int calNumStop) {

  auto to_rawFile = [](const char *baseDir, unsigned int run_number) -> AMSRawFile {
    // First 4 digits: dir number
    unsigned int dirNum = run_number / 1000;
    // Last 4 digits: block number
    unsigned int blockNum = run_number % 1000;

    assert(dirNum < 10000);
    assert(blockNum < 1000);

    return {baseDir, dirNum, blockNum};
  };

  // helper function to loop over AMS raw dirs
  auto get_filelist = [](const AMSRawFile &start, const AMSRawFile &stop) {
    std::vector<std::string> filelist{};

    for (AMSRawFile file = start; file <= stop; ++file) {
      if (FSUtils::IsRegularFile(file.FilePath())) {
        filelist.push_back(file.FilePath());
      }
    }

    return filelist;
  };

  AMSRawFile calStartFile = to_rawFile(calDir, calNumStart);
  AMSRawFile calStopFile = to_rawFile(calDir, calNumStop);

  m_calFilenames = get_filelist(calStartFile, calStopFile);
  // std::cout << "CAL files:\n";
  // std::copy(begin(m_calFilenames), end(m_calFilenames), std::ostream_iterator<std::string>(std::cout, "\n"));

  AMSRawFile dataStartFile = to_rawFile(rawDir, runStart);
  AMSRawFile dataStopFile = to_rawFile(rawDir, runStop);

  m_dataFilenames = get_filelist(dataStartFile, dataStopFile);
  // std::cout << "BEAM files:\n";
  // std::copy(begin(m_dataFilenames), end(m_dataFilenames), std::ostream_iterator<std::string>(std::cout, "\n"));
}

bool DecodeDataAMSL0::ProcessCalibration() {
  int iJinf = 0; // in the OCA case we have just one "collector" (the DAQ PC itself)

  auto start = std::chrono::system_clock::now();

  std::vector<std::vector<std::vector<float>>> signals(NTDRS, std::vector<std::vector<float>>(NVAS * NCHAVA));

  for (const auto &calFilePath : m_calFilenames) {
    m_total_size_consumed = 0;
    auto event = std::make_unique<EventAMSL0>((char *)"ladderconf_L0.dat", (char *)"gaincorrection_L0.dat");

    // open the calibration file
    calfile = fopen(calFilePath.c_str(), "r");
    if (!calfile) {
      printf("Error file %s not found \n", calFilePath.c_str());
      exit(2);
    }

    printf("Processing calibration (%s)... \n", calFilePath.c_str());

    // Some test calibrations contain too many events, stop at 10k
    unsigned int nEvents{0};
    while (!feof(calfile) && nEvents < 10000) {
      bool result = ReadOneEventFromFile(calfile, event.get());
      if (result)
        continue;
      nEvents++;
      // std::cout << "\rRead " << nEvents << " events" << std::flush;

      //      if (nEvents > 1000)
      //        break;

      for (unsigned int iTdr = 0; iTdr < NTDRS; ++iTdr) {
        for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
          signals[iTdr][iCh].push_back(event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
        }
      }
    }
    std::cout << '\n';
  }

  ComputeCalibration<EventAMSL0, calibAMSL0>(signals, cals);

  auto stop = std::chrono::system_clock::now();
  std::cout << "DecodeDataAMSL0::ProcessCalibration took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms\n";

  return true;
}

int DecodeDataAMSL0::GetTdrNum(size_t pos) {
  if (pos > NJINF * NTDRS) {
    printf("Pos %ld not allowed. Max is %ld\n", pos, NJINF * NTDRS);
    return -9999;
  }
  return tdrMap[pos].first;
}

int DecodeDataAMSL0::GetTdrType(size_t pos) {
  if (pos > NJINF * NTDRS) {
    printf("Pos %ld not allowed. Max is %ld\n", pos, NJINF * NTDRS);
    return -9999;
  }
  return tdrMap[pos].second;
}

int DecodeDataAMSL0::ReadOneEventFromFile(FILE *file, DecodeDataAMSL0::EventAMSL0 *event) {
  static constexpr uint32_t fine_time_env_dt = 0x1f0383;
  static constexpr uint32_t vladimir_raw_size = 1794;

  // TODO: clean up
  int fstat = 0;
  uint32_t size_to_read{0};
  uint16_t dummy{0};

  uint16_t size{0};
  uint16_t size_ext{0};
  uint32_t size_full{0};

  uint16_t na{0};

  uint16_t dt{0};
  uint16_t dt_ext{0};
  uint32_t dt_full{0};

  fstat = ReadFile(&size, sizeof(size), 1, file);
  m_total_size_consumed += sizeof(size);
  if (fstat == -1)
    return 1;

  std::bitset<16> size_bits{size};
  if (size_bits[15]) {
    fstat = ReadFile(&size_ext, sizeof(size_ext), 1, file);
    m_total_size_consumed += sizeof(size_ext);
    if (fstat == -1)
      return 1;
    size &= 0x7fff; // mask the first bit
    size_full = (size << 16) + size_ext;
  } else {
    // size &= 0x7fff; // mask the first bit // but why? it's 0!
    size_full = size;
  }

  size_to_read = size_full;

  fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
  m_total_size_consumed += sizeof(dummy);
  size_to_read -= sizeof(dummy);
  if (fstat == -1)
    return 1;

  // extract bits 5:13
  na = (dummy >> 5) & 0x7F;
  // extract bits 0:4
  dt = dummy & 0x1F;
  if (dt == 0x1F) {
    fstat = ReadFile(&dt_ext, sizeof(dt_ext), 1, file);
    m_total_size_consumed += sizeof(dt_ext);
    size_to_read -= sizeof(dt_ext);
    if (fstat == -1)
      return 1;
    dt_full = (dt << 16) + dt_ext;
  } else {
    dt_full = dt;
  }

  // std::cout << "DT: " << std::hex << dt_full << " size: " << std::dec << size_full << '\n';

  if (dt_full == fine_time_env_dt && size_full > 24) { // fine time envelope
    fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
    m_total_size_consumed += sizeof(dummy);
    size_to_read -= sizeof(dummy);
    if (fstat == -1)
      return 1;

    // extract bits 12:15
    uint16_t status = (dummy >> 12) & 0x7;

    // extract bits 0:11
    uint16_t tag = dummy & 0xFFF;

    fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
    m_total_size_consumed += sizeof(dummy);
    size_to_read -= sizeof(dummy);
    if (fstat == -1)
      return 1;

    uint16_t utime_sec_msb = dummy;

    fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
    m_total_size_consumed += sizeof(dummy);
    size_to_read -= sizeof(dummy);
    if (fstat == -1)
      return 1;

    uint16_t utime_sec_lsb = dummy;
    uint32_t utime_sec = (utime_sec_msb << 16) + utime_sec_lsb;
    if (event->TimeStamp == 0) {
      event->TimeStamp = utime_sec;
    }

    fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
    m_total_size_consumed += sizeof(dummy);
    size_to_read -= sizeof(dummy);
    if (fstat == -1)
      return 1;

    uint16_t utime_usec_msb = dummy;

    fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
    m_total_size_consumed += sizeof(dummy);
    size_to_read -= sizeof(dummy);
    if (fstat == -1)
      return 1;

    uint16_t utime_usec_lsb = dummy;
    uint32_t utime_usec = (utime_usec_msb << 16) + utime_usec_lsb;
    if (event->TimeStamp_ns == 0) {
      event->TimeStamp_ns = 1000 * utime_usec;
    }

    unsigned int current_read_bytes = m_total_size_consumed;
    // int ret = ProcessBlock(file, read_bytes, ev_found, signals_by_ev, nesting_level + 1);
    int ret = ReadOneEventFromFile(file, event);
    if (ret != 0)
      return ret;
    unsigned int read_bytes = m_total_size_consumed - current_read_bytes;
    // m_total_size_consumed += read_bytes;
    size_to_read -= read_bytes;
  } else if (dt_full == 0x13) {
    // SCI/CAL/CFG/HK/ (0x5, 0x6, 0x7, 0x8) + LVL3 + GPS data
    fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
    m_total_size_consumed += sizeof(dummy);
    size_to_read -= sizeof(dummy);
    if (fstat == -1)
      return 1;

    // extract bits 12:15
    uint16_t status = (dummy >> 12) & 0x7;

    // extract bits 0:11
    uint16_t tag = dummy & 0xFFF;

    fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
    m_total_size_consumed += sizeof(dummy);
    size_to_read -= sizeof(dummy);
    if (fstat == -1)
      return 1;

    uint16_t utime_sec_msb = dummy;

    fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
    m_total_size_consumed += sizeof(dummy);
    size_to_read -= sizeof(dummy);
    if (fstat == -1)
      return 1;

    uint16_t utime_sec_lsb = dummy;
    uint32_t utime_sec = (utime_sec_msb << 16) + utime_sec_lsb;

    // If size_to_read = 0 the event is empty
    // If size_to_read = 1794 this is a Vladimir raw event
    if (size_to_read == 0) {
      return 1;
    } else if (size_to_read == vladimir_raw_size) {
      fstat = ReadFile(&dummy, sizeof(dummy), 1, file);
      m_total_size_consumed += sizeof(dummy);
      size_to_read -= sizeof(dummy);
      if (fstat == -1)
        return 1;

      // extract bits 0:8
      uint16_t num_events = dummy & 0xFF; // from bit 0 to bit 7

      std::vector<uint8_t> data(size_to_read);

      fstat = ReadFile(data.data(), size_to_read, 1, file);
      m_total_size_consumed += size_to_read;
      size_to_read -= size_to_read;
      if (fstat == -1)
        return 1;

      // Endianess correction
      for (size_t ii = 0; ii < data.size(); ii += 2) {
        std::swap(data[ii], data[ii + 1]);
      }

      // ReOrderVladimir(data, data_ord);
      size_t new_size = 8 * data.size() / 14;
      std::vector<uint16_t> data_ord(new_size);
      std::vector<uint16_t> data_ord_tmp(new_size);
      int ch_in_2va = -1;

      // We take the first 14 8-bit chunks.
      for (size_t offset = 0; offset < data.size(); offset += 14) {
        // these all refer to the same channel
        ch_in_2va++;

        // we loop on each signal bit
        for (int bit_nec = 0; bit_nec < 14; bit_nec++) {
          std::bitset<8> data_bits{data[offset + bit_nec]};

          for (int adc = 0; adc < 8; adc++) {
            int ch = adc * 128 + ch_in_2va;
            data_ord_tmp[ch] |= data_bits[adc] << bit_nec;
          }
        }
      }

      for (int ch = 0; ch < ((int)data_ord_tmp.size()); ch++) {
        int va = ch / 64;
        int ch_in_va = ch % 64;
        //    int new_ch = (16-va-1)*64 + (64-ch_in_va-1);//MD: I undersootd like
        //    this, but VK did differently (31 Mar 2022)
        int new_ch = (16 - va - 1) * 64 + ch_in_va;
        data_ord[new_ch] = data_ord_tmp[ch];
      }

      // signals_by_ev.push_back(data_ord);
      // FIXME: figure out how to compute iTDR from data, iJINF should be always 0?
      size_t iTDR = 0;

      std::copy(std::begin(data_ord), std::end(data_ord), std::begin(event->RawSignal[0][iTDR]));
      // std::cout << "Ch0 signal = " << event->RawSignal[0][iTDR][0] << '\n';
    }
  }

  // skip the remaining block data
  if (int ret0 = fseek(file, size_to_read, SEEK_CUR)) {
    printf("Fatal: error during file skip ret0=%d \n", ret0);
    return -99;
  }

  m_total_size_consumed += size_to_read;

  // if we last skipped data, let our caller know
  if (size_to_read) {
    return 1;
  }

  return 0;
}

int DecodeDataAMSL0::ReadOneEvent() {
  static bool first_call = true;
  static auto filenameIt = begin(m_dataFilenames);
  if (first_call) {
    std::string filename = *filenameIt;
    std::cout << "\rOpening data file " << filename << '\n';
    rawfile = fopen(filename.c_str(), "r");
    first_call = false;

    m_total_size_consumed = 0;
  }

  if (feof(rawfile)) {
    if (++filenameIt != end(m_dataFilenames)) {
      std::string filename = *filenameIt;
      std::cout << "\rOpening data file " << filename << '\n';
      rawfile = fopen(filename.c_str(), "r");
    } else {
      m_end_of_file = true;
      return 0;
    }
  }

  int iJinf = 0; // in the L0 case we have just one "collector" (the DAQ PC itself)
  // FIXME: For now only one board (2022/11/23)
  int iTdr = 0;

  // copy calibration data...
  for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
    ev->CalPed[iJinf][iTdr][iCh] = cals[iTdr].ped[iCh];
    ev->CalSigma[iJinf][iTdr][iCh] = cals[iTdr].sig[iCh];
    ev->CalStatus[iJinf][iTdr][iCh] = cals[iTdr].status[iCh];
  }

  pri = false;
  int retVal = ReadOneEventFromFile(rawfile, ev);
  if (!retVal) {
    ev->SetEvtNum(m_read_events);
    m_read_events++;
  } else {
    // Don't clusterize empty or generally bad events!! [VF]
    return retVal;
  }

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

int DecodeDataAMSL0::EndOfFile() { return m_end_of_file; }

void DecodeDataAMSL0::InitHistos() {
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
      hsignal[jj * NTDRS + hh][0] = new TH1F(name, name, 4200, -100, 16000);
      sprintf(name, "signalK_%ld_%ld", jj, hh);
      hsignal[jj * NTDRS + hh][1] = new TH1F(name, name, 4200, -100, 16000);

      sprintf(name, "q_vs_occ_%ld_%ld", jj, hh);
      hchargevsocc[jj * NTDRS + hh] = new TH2F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA, 1000, 0, 100);

      sprintf(name, "signal_vs_occ_%ld_%ld", jj, hh);
      hsignalvsocc[jj * NTDRS + hh] = new TH2F(name, name, NVAS * NCHAVA, 0, NVAS * NCHAVA, 4200, -100, 16000);

      sprintf(name, "sonS_%ld_%ld", jj, hh);
      hson[jj * NTDRS + hh][0] = new TH1F(name, name, 1000, 0, 100);
      sprintf(name, "sonK_%ld_%ld", jj, hh);
      hson[jj * NTDRS + hh][1] = new TH1F(name, name, 1000, 0, 100);
    }
  }
  std::cout << '\n';
}
