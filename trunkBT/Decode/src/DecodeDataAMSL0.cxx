//
// Created by Valerio Formato on 21/11/22.
//

#include "DecodeDataAMSL0.hh"
#include "FSUtils.hh"

#include "GenericEvent.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

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

//------------------------------------------------------------------------------------------------------------------

DecodeDataAMSL0::DecodeDataAMSL0(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int runStop,
                                 unsigned int calStart, unsigned int calStop, int _style) {

  decodestyle = _style;

  m_rawDir = rawDir;
  m_calDir = calDir;

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

  rawfile = NULL;

  // Create the ROOT run header
  rh = new RHClassAMSL0();

  ev = new EventAMSL0((char *)"ladderconf_L0.dat", (char *)"gaincorrection_L0.dat");

  //  DecodeDataAMSL0::OpenFile(m_rawDir.c_str(), m_calDir.c_str(), runNum, runStop, calStart, calStop);
  DecodeDataAMSL0::OpenFile(m_rawDir.c_str(), m_rawDir.c_str(), runNum, runStop, calStart, calStop);

  rh->SetRun(runNum);

  //  printf("DecodeStyle = %d\n", decodestyle);

  bool rfherr = false;
  if (decodestyle == 0) {
    if (!ReadFileHeader(rawfile, rh))
      rfherr = true;
  } else {
    if (!ReadFileHeader(&rawdatastream, m_dataFilenames, rh, 0xD))
      rfherr = true;
  }
  if (rfherr) {
    throw std::runtime_error("Failed to read AMSL0 run header");
  }
  // reset blockstream to first event of first file
  rawdatastream.Reset();

  DecodeDataAMSL0::DumpRunHeader();

  ProcessCalibration();

  InitHistos();
}

bool DecodeDataAMSL0::ProcessCalibration() {

  auto start = std::chrono::system_clock::now();

  std::vector<std::vector<std::vector<std::vector<float>>>> signals(
      NJINF,
      //      nJinf,
      std::vector<std::vector<std::vector<float>>>(NTDRS, std::vector<std::vector<float>>(NVAS * NCHAVA))
      //      std::vector<std::vector<std::vector<float>>>(ntdrRaw + ntdrCmp, std::vector<std::vector<float>>(NVAS *
      //      NCHAVA))
  );
  unsigned long int nEvents{0};

  auto event = std::make_unique<EventAMSL0>((char *)"ladderconf_L0.dat", (char *)"gaincorrection_L0.dat");

  if (decodestyle == 0) {
    for (const auto &calFilePath : m_calFilenames) {
      m_total_size_consumed = 0;

      // open the calibration file
      calfile = fopen(calFilePath.c_str(), "r");
      if (!calfile) {
        printf("Error file %s not found \n", calFilePath.c_str());
        exit(2);
      }

      printf("Processing calibration (%s)... \n", calFilePath.c_str());

      // Some test calibrations contain too many events, stop at 10k
      while (!feof(calfile) && nEvents < 10000) {
        event.get()->Clear();
        bool result = ReadOneEventFromFile(calfile, event.get());
        if (result)
          continue;
        nEvents++;
        std::cout << "\rRead " << nEvents << " events" << std::flush;

        //      if (nEvents > 1000)
        //        break;

        // only 1 Jinf is managed by ComputeCalibration()
        // treat everything (also in the case with two LINFs as if we have just one
        for (unsigned int iTdr_index = 0; iTdr_index < uint(ntdrRaw + ntdrCmp); ++iTdr_index) {
          unsigned int iTdr = rh->GetTdrNum(iTdr_index);
          unsigned int iJinf = rh->GetJinfNum(iTdr_index);
          for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
            signals[iJinf][iTdr][iCh].push_back(event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
          }
        }
      }
      std::cout << '\n';
    }
  } else { // decodestyle>0
    // this is to check the config of the calib is the same of the run
    ReadFileHeader(&rawcalstream, m_calFilenames, NULL, 0xC);
    // reset blockstream to first event of first file
    rawcalstream.Reset();

    printf("Processing calibration (%s)... \n", rawcalstream.CurrentFilePath().c_str());

    // Some test calibrations contain too many events, stop at 10k
    while (nEvents < 10000) {
      event.get()->Clear();
      int result = ReadOneEventFromFile(&rawcalstream, event.get(), nEvents, 0xC, CalTag);
      if (result) {
        if (result < 0) {
          break;
        } else {
          continue;
        }
      }
      nEvents++;
      std::cout << "\rRead " << nEvents << " events  " << std::flush;

      //      if (nEvents > 1000)
      //        break;

      // only 1 Jinf is managed by ComputeCalibration()
      // treat everything (also in the case with two LINFs as if we have just one
      for (unsigned int iTdr_index = 0; iTdr_index < uint(ntdrRaw + ntdrCmp); ++iTdr_index) {
        unsigned int iTdr = rh->GetTdrNum(iTdr_index);
        unsigned int iJinf = rh->GetJinfNum(iTdr_index);
        for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
          if (event->ValidTDR[iJinf][iTdr]) {
            signals[iJinf][iTdr][iCh].push_back(event->RawSignal[iJinf][iTdr][iCh] / m_adcUnits);
            // if (evpri)
            //   printf("signals[%d][%d][%d] = %f\n", iJinf, iTdr, iCh, event->RawSignal[iJinf][iTdr][iCh] /
            //   m_adcUnits);
          } else { // is important to have all the vectors in sync each other
            signals[iJinf][iTdr][iCh].push_back(0);
          }
        }
      }
    }
    std::cout << '\n';
  }

  auto stop = std::chrono::system_clock::now();
  std::cout << "DecodeDataAMSL0::ProcessCalibration (reading) took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "ms\n";

  if (evpri) {
    printf("signals.size()=%lu, signals.at(0).size()=%lu, signals.at(0).at(0).size()=%lu\n", signals.size(),
           signals.at(0).size(), signals.at(0).at(0).size());
  }

  //  printf("nJinf = %d\n", nJinf);
  for (unsigned int iJinf = 0; iJinf < uint(nJinf); iJinf++) {
    //    printf("iJinf = %d\n", iJinf);

    ComputeCalibration<EventAMSL0, calibAMSL0, EventAMSL0::GetNTDRS()>(signals[iJinf], cals[iJinf], iJinf);
    /*
    for (unsigned int iTdr = 0; iTdr < (ntdrRaw + ntdrCmp); iTdr++) {
      printf("iJinf=%u, iTdr=%u valid: %d\n", iJinf, iTdr, cals[iJinf][iTdr].valid);
    }
    */

    SaveCalibration<EventAMSL0, calibAMSL0>(signals[iJinf], cals[iJinf], m_calRunnums.at(0), (ntdrRaw + ntdrCmp),
                                            iJinf);
  }

  auto stop2 = std::chrono::system_clock::now();
  std::cout << "DecodeDataAMSL0::ProcessCalibration (computing and saving) took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - stop).count() << "ms\n";

  return true;
}

int DecodeDataAMSL0::ReadOneEvent() {

  if (decodestyle == 0) {
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
  } else { // decodestyle>0
    static std::string oldfile = "";
    if (rawdatastream.CurrentFilePath() != oldfile) {
      std::cout << "\rOpening data file " << rawdatastream.CurrentFilePath() << '\n';
    }
    oldfile = rawdatastream.CurrentFilePath();
  }

  // copy calibration data... Just for first event (when > 0 we can start to skip), since are static
  if (m_read_events == 0) {
    for (unsigned int iTdr_index = 0; iTdr_index < uint(ntdrRaw + ntdrCmp); ++iTdr_index) {
      unsigned int iTdr = rh->GetTdrNum(iTdr_index);
      unsigned int iJinf = rh->GetJinfNum(iTdr_index);
      for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
        ev->CalPed[iJinf][iTdr][iCh] = cals[iJinf][iTdr].ped[iCh];
        ev->CalSigma[iJinf][iTdr][iCh] = cals[iJinf][iTdr].sig[iCh];
        ev->CalStatus[iJinf][iTdr][iCh] = cals[iJinf][iTdr].status[iCh];
      }
    }
  }

  int retVal = 1;
  if (decodestyle == 0)
    retVal = ReadOneEventFromFile(rawfile, ev);
  else
    while (retVal >= 1 && retVal <= 2) { // 1: too early, 2: not found yet
      retVal = ReadOneEventFromFile(&rawdatastream, ev, m_read_events, 0xD, Tag);
    }
  if (!retVal) {
    ev->SetEvtNum(m_read_events);
    m_read_events++;
  } else {
    // Don't clusterize empty or generally bad events!! [VF]
    return retVal;
  }

  // FIX ME [VF]: this should be done by the main! This function is called ReadOneEvent. It's done reading at this
  // point, so it should return.
  for (unsigned int iTdr_index = 0; iTdr_index < uint(ntdrRaw + ntdrCmp); ++iTdr_index) {
    unsigned int iTdr = rh->GetTdrNum(iTdr_index);
    unsigned int iJinf = rh->GetJinfNum(iTdr_index);
    //      printf("iJinf = %d, iTdr = %d\n", iJinf, iTdr);
    if (kClusterize) {
      Clusterize(iTdr, iJinf, ev, &cals.at(iJinf).at(iTdr));
    } else {
      FillRawHistos(iTdr, iJinf, ev, &cals.at(iJinf).at(iTdr));
    }
  }

  return retVal;
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

//----------------------------------------------------------------------------------------------------------------
// old decoding style

bool DecodeDataAMSL0::ReadFileHeader(FILE *file, RHClassAMSL0 *rhc) {
  // essentially this is not reading anything from the file
  // is just assigning things

  ntdrCmp = 0;
  ntdrRaw = 1;
  nJinf = 1; // number of LINFs
  JinfMap[0] = 0;

  // we assume that from now on we know how many boards are in the DAQ
  for (unsigned int iTdr = 0; iTdr < nJinf * uint(ntdrCmp + ntdrRaw); ++iTdr) {
    tdrMap[iTdr] = {iTdr, 0}; // putting type at 0 since they're all RAW, so far...
  }

  // Update the ROOT run header
  rhc->SetNJinf(nJinf);
  rhc->SetJinfMap(JinfMap);
  rhc->SetNTdrsRaw(ntdrRaw);
  rhc->SetNTdrsCmp(ntdrCmp);
  rhc->SetTdrMap(tdrMap);
  rhc->AddBoardID(0);
  rhc->SetNumBoards(1);

  // rhc->SetUnixTime(runUnixTime);
  // rhc->SetGitSHA(stream.str());
  rhc->SetRunType(RHClassAMSL0::RunType::SC);
  // rhc->SetDataVersion(major, minor, patch);

  return true;
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
    event->TimeStamp = utime_sec;

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
    event->TimeStamp_ns = 1000 * utime_usec;

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

      auto ReOrderVladimir = [](std::vector<uint8_t> data) {
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

        return data_ord;
      };

      //----------------------------------------------------------------------------------------------------------------

      std::vector<uint16_t> data_ord = ReOrderVladimir(data);

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

//----------------------------------------------------------------------------------------------------------------------
// new decoding style

bool DecodeDataAMSL0::ReadFileHeader(TBDecode::L0::AMSBlockStream *rawfilestream, std::vector<std::string> rawfilenames,
                                     RHClassAMSL0 *rhc, uint16_t expTag) {

  // special case:
  //  a call to this method, with the calibration, to check if the config is the same
  //  (i.e. if is really a calibration of the same setup)
  //  this special case is "dry run" (i.e. nothing is changed, for example in the RHClass)
  //  and this is checked/protected looking to a NULL RHClass pointer
  bool not_same_config = false;
  std::string current_config_info = "";

  if (rhc) {
    ResetSetup();
    Tag = 0x0;
  } else {
    CalTag = 0x0;
  }
  unsigned int runUnixTime = 0;
  std::string runDate = "";

  std::string slinf = "LINF";
  std::string slef = "LEF";

  int empty_blocks = 0;

  printf("---------------------------------------------------------------\n");
  printf("Reading file header from file:");
  if (rhc)
    printf("\n");
  else
    printf(" (Calibration)\n");
  //  printf("(%s)\n", rawfilenames.at(0).c_str());
  printf("(%s)\n", rawfilestream->CurrentFilePath().c_str());

  std::string config_to_print = "";

  bool kConfigInfoFound = false;
  bool kEventBuilderStartFound = false;
  bool kWrongTagTypeFound = false;
  bool kGoodTagTypeFound = false;
  while (!rawfilestream->EndOfStream() && !kGoodTagTypeFound) {
    auto block = TBDecode::L0::AMSBlock::DecodeAMSBlock(rawfilestream->CurrentFile());
    std::visit(
        TBDecode::Utils::overloaded{
            [&empty_blocks](TBDecode::L0::AMSBlock::EmptyBlock &block) {
              if (!block.fake) {
                empty_blocks++;
                printf("  **** %d empty blocks found...\n", empty_blocks);
              }
            },
            [&config_to_print, &not_same_config, &current_config_info, rhc, &kConfigInfoFound, slinf, slef,
             this](TBDecode::L0::AMSBlock::ConfigInfo &block) {
              char to_add[512] = "";
              if (block.config != "") { // for some reason sometime is empty
                printf("  Read a ConfigInfo block from node %02x", block.node_address);
                if (rhc) {
                  printf("\n");
                } else {
                  printf("   (Calibration)\n");
                }
                if (kConfigInfoFound) {
                  printf("  ...we already found a Config: considering only the last valid one!\n");
                  if (rhc) {
                    ResetSetup();
                    this->Tag = 0x0;
                  } else {
                    this->CalTag = 0x0;
                  }
                  config_to_print = "";
                }
                current_config_info = block.config;
                if (rhc)
                  this->config_info = block.config;
                else {
                  // check if the config is the same
                  if (current_config_info == this->config_info) {
                    not_same_config = false;
                  } else {
                    not_same_config = true;
                  }
                }
                snprintf(to_add, 512, "  config:  %s\n", block.config.c_str());
                config_to_print += to_add;
                snprintf(to_add, 512, "Dumping all devices:\n");
                config_to_print += to_add;
                snprintf(to_add, 512, " Device type: %s, version: %d, node_address: %02x, device_ID: %02x\n",
                         TBDecode::L0::AMSBlock::main_box.name.c_str(), TBDecode::L0::AMSBlock::main_box.version,
                         TBDecode::L0::AMSBlock::main_box.node_address, TBDecode::L0::AMSBlock::main_box.device_ID);
                config_to_print += to_add;
                for (TBDecode::L0::AMSBlock::AMSL0Node &device : TBDecode::L0::AMSBlock::main_box.links) {
                  if (!device.fake) {
                    snprintf(
                        to_add, 512,
                        "  |- Device: %6s (%7s), node_ID: %02x, node_address: %02x, link_number: %d, version: %d\n",
                        device.name.c_str(), device.unique_name.c_str(), device.node_ID, device.node_address,
                        device.link_number, device.version);
                    config_to_print += to_add;
                    if (rhc) {
                      if (device.name.find(slinf) != std::string::npos) {
                        this->JinfMap[nJinf] = device.link_number >> 1; // 0 and 1 = 0, 2 and 3 = 1
                        //                        this->JinfMap[nJinf] = device.node_ID;
                        this->nJinf++;
                      } else if (device.name.find(slef) != std::string::npos) {
                        this->tdrMap[ntdrRaw] = {device.link_number, 0}; // {board number, RAW}
                        //                        this->tdrMap[ntdrRaw] = {device.node_ID, 0}; // {board number, RAW}
                        this->ntdrRaw++;
                      } else {
                        printf("  This is not a recognized kind of device: %s\n", device.name.c_str());
                      }
                    }
                    for (TBDecode::L0::AMSBlock::AMSL0Node &sec_device : device.links) {
                      snprintf(to_add, 512, "    |- Device: %6s (%7s), node_ID: %02x, link_number: %d, version: %d\n",
                               sec_device.name.c_str(), sec_device.unique_name.c_str(), sec_device.node_ID,
                               sec_device.link_number, sec_device.version);
                      config_to_print += to_add;
                      if (rhc) {
                        if (sec_device.name.find(slef) != std::string::npos) {
                          auto jinfnum = device.link_number >> 1; // 0 and 1 = 0, 2 and 3 = 1
                          this->tdrMap[this->ntdrRaw] = {ComputeTdrNum(sec_device.link_number, jinfnum),
                                                         0}; // {board number, RAW}
                          this->ntdrRaw++;
                        } else {
                          printf("  This is not a recognized kind of device: %s\n", sec_device.name.c_str());
                        }
                      }
                    }
                  }
                }
              } else {
                printf("  **** the config string is empty. Why?\n");
              }
              kConfigInfoFound = true;
            },
            [rhc, &config_to_print, expTag, &kConfigInfoFound, &kGoodTagTypeFound, &kWrongTagTypeFound,
             this](TBDecode::L0::AMSBlock::FineTimeEnvelope &block) {
              if (this->pri) {
                printf("  FineTimeEnvelope\n");
                printf("  Time: %u\n", block.utime_sec);
              }
              TBDecode::L0::AMSBlock::SCIData data = block.data;
              if (evpri) {
                printf("  Tag = %x\n", block.tag);
                printf("  raw_data.size() = %lu\n", data.raw_data.size());
              }
              if (rhc) {
                this->Tag = block.tag;
              } else {
                this->CalTag = block.tag;
              }
              if (expTag && ((block.tag >> 8) != expTag)) {
                if (!kWrongTagTypeFound) { // if already found be silent...
                  printf("  **** We found the wrong Tag. We were expecting 0x%XXX, we found 0x%X\n", expTag, block.tag);
                  printf("  **** We need to start again and search another config...\n");
                }
                kWrongTagTypeFound = true;
                kConfigInfoFound = false; // what we found was not valid
                if (rhc) {
                  this->Tag = 0x0;
                  ResetSetup();
                } else {
                  this->CalTag = 0x0;
                }
                config_to_print = "";
              } else {
                kGoodTagTypeFound = true;
              }
            },
            [&runDate, &runUnixTime, &kEventBuilderStartFound](TBDecode::L0::AMSBlock::EventBuilderStart &block) {
              printf("  EventBuilderStart:\n");
              std::time_t t = block.utime_sec;
              std::tm tm = *std::gmtime(&t);
              std::stringstream transTime;
              transTime << std::put_time(&tm, "%c %Z");
              printf("    Time: %u (%s)\n", block.utime_sec, transTime.str().c_str());
              runDate = transTime.str();
              runUnixTime = block.utime_sec;
              kEventBuilderStartFound = true;
            },
            [](TBDecode::L0::AMSBlock::EventBuilderStop &block) {},
            [](auto &block) {},
        },
        block);
  }

  if (config_to_print != "") {
    printf("%s\n", config_to_print.c_str());
  } else {
    if (rhc) {
      config_info = "";
    }
  }

  if (rhc) {
    m_numBoardsFound = ntdrCmp + ntdrRaw;

    // Update the ROOT run header
    rhc->SetNJinf(nJinf);
    rhc->SetJinfMap(JinfMap);
    rhc->SetNTdrsRaw(ntdrRaw);
    rhc->SetNTdrsCmp(ntdrCmp);
    rhc->SetTdrMap(tdrMap);
    for (int ii = 0; ii < ntdrRaw + ntdrCmp; ii++) {
      rhc->AddBoardID(tdrMap[ii].first);
    }
    rhc->SetNumBoards(m_numBoardsFound);

    rhc->SetUnixTime(runUnixTime);
    rhc->SetDate(runDate.c_str());

    // rhc->SetGitSHA(stream.str());
    rhc->SetRunType(RHClassAMSL0::RunType::SC);
    rhc->SetRunTag(Tag);
    // rhc->SetDataVersion(major, minor, patch);
  } else {
    if (not_same_config) {
      std::string message = Form("The config is changed:\n  current config: %s\n previous config: %s",
                                 current_config_info.c_str(), config_info.c_str());
      throw std::runtime_error(message);
    }
  }

  if (!kConfigInfoFound || !kEventBuilderStartFound) {
    if (!kConfigInfoFound) {
      printf("*** we didn't found the Config: invalid file\n");
    }
    if (!kEventBuilderStartFound) {
      printf("*** we didn't found the Event Builder start: invalid file\n");
    }
    return false;
  }

  return true;
};

int DecodeDataAMSL0::ReadOneEventFromFile(TBDecode::L0::AMSBlockStream *stream, EventAMSL0 *event,
                                          unsigned long int nEvents, uint16_t expTagType, uint16_t expTag) {

  constexpr int bufferlenght = 256;
  constexpr int dumpshift = 128;

  bool not_same_config = false;
  std::string current_config_info = "";

  int empty_blocks = 0;

  static bool kWrongTagFound = false;
  static bool kEventBuilderStopFound = false;
  static bool kEventBuilderStartFound = false;

  if (evpri)
    printf("buffer.size() = %lu\n", buffer.size());
  if (buffer.size() < bufferlenght / 2 && !stream->EndOfStream()) {
    auto block = TBDecode::L0::AMSBlock::DecodeAMSBlock(stream->CurrentFile());
    std::visit(TBDecode::Utils::overloaded{
                   [&empty_blocks](TBDecode::L0::AMSBlock::EmptyBlock &block) {
                     if (!block.fake) {
                       empty_blocks++;
                       printf("  **** %d empty blocks found:", empty_blocks);
                     }
                   },
                   [expTag, expTagType, this, event](TBDecode::L0::AMSBlock::FineTimeEnvelope &block) {
                     if (this->pri) {
                       printf("  FineTimeEnvelope\n");
                       printf("  Time: %u\n", block.utime_sec);
                     }
                     event->TimeStamp = block.utime_sec;
                     event->TimeStamp_ns = 1000 * block.utime_sec;
                     TBDecode::L0::AMSBlock::SCIData data = block.data;
                     if (evpri) {
                       printf("  kEventBuilderStartFound=%d\n", (int)kEventBuilderStartFound);
                       printf("  Tag = %x\n", block.tag);
                       printf("  raw_data.size() = %lu\n", data.raw_data.size());
                     }
                     if ((expTag && (block.tag != expTag)) ||
                         (expTagType &&
                          ((block.tag >> 8) !=
                           expTagType))) {    // the tag must be the same we found when we did read the header and, if
                                              // passed, must also be the same to the prefix passed (typically 0xD)
                       if (!kWrongTagFound) { // we already said: be silent
                         printf("  **** We found the wrong Tag. We were expecting 0x%XXX (0x%3X), we found 0x%X\n",
                                expTag, this->Tag, block.tag);
                         printf("  **** We need to start again and search another config...\n");
                       }
                       kWrongTagFound = true;
                       kEventBuilderStartFound = false;
                     } else {
                       kWrongTagFound = false;
                       if (kEventBuilderStartFound) {
                         // printf("0x%x 0x%x 0x%x 0x%x 0x%x\n", expTag, expTagType, this->Tag, this->CalTag,
                         // block.tag);
                         for (auto i = data.data.begin(); i != data.data.end(); i++) {
                           uint16_t evno = i->first;
                           unsigned long nLEFs = i->second.size();
                           if (evpri)
                             printf("i) evno=%u,  nLEFs=%lu\n", evno, nLEFs);
                           for (auto j = i->second.begin(); j != i->second.end(); j++) {
                             uint16_t LINF = j->first.first >> 1; // 0 and 1 = 0, 2 and 3 = 1
                             uint16_t LEF = j->first.second;
                             unsigned long size_data = j->second.size();
                             if (evpri)
                               printf("j) LINF=%d, LEF=%d, size_data=%lu\n", LINF, LEF, size_data);
                             std::pair linf_lef = std::make_pair(LINF, LEF);
                             auto it = std::find_if(buffer.begin(), buffer.end(),
                                                    [evno](auto element) { return element.first == evno; });
                             if (it != std::end(buffer)) { // event already present
                               it->second[linf_lef] = j->second;
                             } else {
                               std::map<std::pair<uint16_t, uint16_t>, std::vector<uint16_t>> el_to_push;
                               el_to_push[linf_lef] = j->second;
                               buffer.push_back(std::make_pair(evno, el_to_push));
                             }
                           }
                         }
                       }
                     }
                   },
                   [&not_same_config, &current_config_info, this](TBDecode::L0::AMSBlock::ConfigInfo &block) {
                     if (this->pri)
                       printf("  Config\n");
                     current_config_info = block.config;
                     // check if the config is the same
                     if (current_config_info == this->config_info) {
                       not_same_config = false;
                     } else {
                       not_same_config = true;
                     }
                   },
                   [](TBDecode::L0::AMSBlock::EventBuilderStart &block) {
                     printf("  EventBuilderStart:\n");
                     std::time_t t = block.utime_sec;
                     std::tm tm = *std::gmtime(&t);
                     std::stringstream transTime;
                     transTime << std::put_time(&tm, "%c %Z");
                     printf("    Time: %u (%s)\n", block.utime_sec, transTime.str().c_str());
                     kEventBuilderStartFound = true;
                     kWrongTagFound = false;
                   },
                   [this](TBDecode::L0::AMSBlock::EventBuilderStop &block) {
                     if (this->pri)
                       printf("  EvenBuilderStop\n");
                     kEventBuilderStopFound = true;
                   },
                   [](auto &block) {},
               },
               block);
  }

  if (not_same_config) {
    std::string message = Form("The config is changed:\n  current config: %s\n previous config: %s",
                               current_config_info.c_str(), config_info.c_str());
    throw std::runtime_error(message);
  }

  if (stream->EndOfStream()) {
    if (evpri) {
      printf("buffer.size() = %lu\n", buffer.size());
    }
    if (buffer.size() == 0) { // end of run...
      kEventBuilderStartFound = false;
      kEventBuilderStopFound = false;
      return -99;
    }
  }

  auto oldness_check = [](auto a, auto b) {
    if ((a - b) > dumpshift || (a + bufferlenght - b) % bufferlenght > dumpshift) {
      return true;
    } else {
      return false;
    }
  };

  /*
  // MD: maybe is just a workaround. To double-check
  auto get_LINF = [](auto readLINF) {
    if (readLINF == 0 || readLINF == 2)
      return 1;
    else if (readLINF == 3 || readLINF == 4)
      return 2;
    else {
      printf("not valid LINF number: %d\n", readLINF);
      return -99;
    }
  };
  */
  auto get_LINF = [](auto readLINF) {
    if (readLINF == 0 || readLINF == 1)
      return (int)readLINF;
    else {
      printf("not valid LINF number: %d\n", readLINF);
      return -99;
    }
  };
  /*
  auto get_LINF = [](auto readLINF) {
    if (readLINF == 0 || readLINF == 1 || readLINF == 2 || readLINF == 3)
      return (int)readLINF;
    else {
      printf("not valid LINF number: %d\n", readLINF);
      return -99;
    }
  };
  */

  /*
  printf("buffer content:\n");
  for (auto it = buffer.cbegin(); it != buffer.cend(); ++it) {
    printf("Ev: %u) nLEF=%lu\n", it->first, it->second.size());
  }
  printf("---------------\n");
  */

  auto i = buffer.begin();
  if (i != buffer.end()) { // otherwise buffer is empty...
    uint16_t evno_to_process = buffer.front().first;
    uint16_t last_evno = buffer.back().first;
    bool old_enough = oldness_check(last_evno, evno_to_process);
    uint16_t evno = i->first;
    unsigned long nLEFs = i->second.size();
    if (evpri)
      printf("i) evno=%u,  nLEFs=%lu\n", evno, nLEFs);
    if (old_enough || stream->EndOfStream()) {
      nEvents++;
      if (evpri)
        printf("Good: evno=%d, nLEFs=%lu (next evno_to_process=%d), last_evno=%d - nEvents=%lu\n", evno, nLEFs,
               evno_to_process, last_evno, nEvents);
      for (auto j = i->second.begin(); j != i->second.end(); j++) {
        uint16_t LINF = get_LINF(j->first.first);
        uint16_t LINF_index = rh->FindJinfPos(LINF);
        uint16_t LEF_glob_index = rh->FindPos(j->first.second, LINF);
        uint16_t LEF_index = rh->GetTdrNum(LEF_glob_index);
        uint16_t LEF = rh->ComputeTdrNum(j->first.second, LINF);
        unsigned long size_data = j->second.size();
        if (evpri)
          printf("j) LEF[%d][%d]: LINF=%d (%d), LEF=%d (%d, %d) -> size_data=%lu\n", LINF_index, LEF_index, LINF,
                 j->first.first, LEF, j->first.second, LEF_glob_index, size_data);
        std::copy(std::begin(j->second), std::end(j->second), std::begin(event->RawSignal[LINF_index][LEF_index]));
        //      std::cout << "Ch0 signal = " << event->RawSignal[LINF_index][LEF_index][0] << '\n';
        event->ValidTDR[LINF_index][LEF_index] = true;
      }
      buffer.pop_front();
      return 0;
    } else {
      if (evpri)
        printf("Too early: evno=%d (next evno_to_process=%d), last_evno=%d\n", evno, evno_to_process, last_evno);
      return 1;
    }
  } else {
    if (evpri)
      printf("buffer empty...\n");
    return 2;
  }

  return -3000;
}

//------------------------------------------------------------------------------------------------------------------

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

  for (int calNum = calNumStart; calNum <= calNumStop; calNum++)
    m_calRunnums.push_back(calNum);

  m_calFilenames = get_filelist(calStartFile, calStopFile);
  std::cout << "CAL files:\n";
  std::copy(begin(m_calFilenames), end(m_calFilenames), std::ostream_iterator<std::string>(std::cout, "\n"));
  for (const auto &calFilePath : m_calFilenames) {
    rawcalstream.AddFile(calFilePath);
  }

  AMSRawFile dataStartFile = to_rawFile(rawDir, runStart);
  AMSRawFile dataStopFile = to_rawFile(rawDir, runStop);

  for (int runNum = runStart; runNum <= runStop; runNum++)
    m_dataRunnums.push_back(runNum);

  m_dataFilenames = get_filelist(dataStartFile, dataStopFile);
  std::cout << "BEAM files:\n";
  std::copy(begin(m_dataFilenames), end(m_dataFilenames), std::ostream_iterator<std::string>(std::cout, "\n"));
  for (const auto &dataFilePath : m_dataFilenames) {
    rawdatastream.AddFile(dataFilePath);
  }
}

int DecodeDataAMSL0::EndOfFile() {
  if (decodestyle == 0) {
    return m_end_of_file;
  } else {
    return (rawdatastream.EndOfStream() && buffer.size() == 0);
  }
}

void DecodeDataAMSL0::DumpRunHeader() {
  if (pri) {
    printf("********* READVALS: %d %d %d %d \n", (ntdrRaw + ntdrCmp), nJinf, ntdrRaw, ntdrCmp);
    printf("Dumping the file headers that are going to be written in the ROOT files...\n");
  }
  rh->Print();
}

//-------------------------------------------------------------------------------------------------------------

void DecodeDataAMSL0::ResetSetup() {

  for (int iJinf = 0; iJinf < nJinf; iJinf++) {
    JinfMap[iJinf] = 0;
  }
  nJinf = 0;
  ntdrRaw = 0;
  ntdrCmp = 0;
  if (tdrMap)
    delete[] tdrMap;
  tdrMap = new laddernumtype[NJINF * NTDRS];

  return;
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

int DecodeDataAMSL0::FindPos(int tdrnum, int jinfnum) {
  if (rh) {
    return rh->FindPos(tdrnum, jinfnum);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeDataAMSL0::FindCalPos(int tdrnum, int jinfnum) {
  if (rh) {
    return rh->FindPos(tdrnum, jinfnum);
  } else {
    printf("***RHClass not instanciated...\n");
  }

  return -1;
}

int DecodeDataAMSL0::ComputeTdrNum(int tdrnum, int jinfnum) { return RHClassAMSL0::ComputeTdrNum(tdrnum, jinfnum); }

//------------------------------------------------------------------------------------------------------------------

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
