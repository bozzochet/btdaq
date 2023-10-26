//
// Created by Valerio Formato on 21/11/22.
//

#ifndef DECODE_DECODEDATAAMSL0_HH
#define DECODE_DECODEDATAAMSL0_HH

#include "DecodeData.hh"
#include "GenericEvent.hh"

#include "TBDecode/AMSL0/AMSBlock.h"
#include "TBDecode/AMSL0/AMSBlockStream.h"

#include <deque>
#include <filesystem>

class DecodeDataAMSL0 : public DecodeData {
public:
  using EventAMSL0 = GenericEvent<2, 9, 64, 8, 16, 0>;
  using calibAMSL0 = calib<EventAMSL0::GetNCHAVA() * EventAMSL0::GetNVAS()>;
  using RHClassAMSL0 = RHClass<EventAMSL0::GetNJINF(), EventAMSL0::GetNTDRS()>;
  using Calibrations = std::array<std::array<calibAMSL0, EventAMSL0 ::GetNTDRS()>, EventAMSL0::GetNJINF()>;

  EventAMSL0 *ev;
  RHClassAMSL0 *rh;

  DecodeDataAMSL0(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int runStop,
                  unsigned int calStart, unsigned int calStop, int _style = 0);

  virtual ~DecodeDataAMSL0();

  virtual ::FlavorConfig FlavorConfig() final {
    return {EventAMSL0::GetNJINF(), EventAMSL0::GetNTDRS(), EventAMSL0::GetNCHAVA(), EventAMSL0::GetNADCS(),
            EventAMSL0::GetNVAS()};
  };

  virtual TString EventClassname() final { return ev->ClassName(); };

  bool ReadFileHeader(FILE *file, RHClassAMSL0 *rhc);
  bool ReadFileHeader(TBDecode::L0::AMSBlockStream *rawfilestream, std::vector<std::string> rawfilenames,
                      RHClassAMSL0 *rhc, uint16_t expTag = 0);

  int ReadOneEvent() final;

  virtual void ClearEvent() final { ev->Clear(); };

  // dummy for now
  virtual int SkipOneEvent(int evskip = 1) final { return 0; };

  virtual int GetTdrNum(size_t pos) final;
  virtual int GetTdrType(size_t pos) final;

  int EndOfFile() final;

  Calibrations GetCalibrations() const { return cals; };

  void GetCalFilePrefix(char *calfileprefix, long int runnum) override {
    // First 4 digits: dir number
    unsigned int dirNum = runnum / 1000;
    // Last 4 digits: block number
    unsigned int blockNum = runnum % 1000;
    sprintf(calfileprefix, "%s/%04d/%03d", m_calDir.c_str(), dirNum, blockNum);
    char dir[512] = "";
    sprintf(dir, "%s/%04d", m_calDir.c_str(), dirNum);
    std::filesystem::create_directory(dir);
  }

private:
  int decodestyle{false};
  std::string config_info = "";
  uint16_t Tag = 0x0;
  uint16_t CalTag = 0x0;

  TBDecode::L0::AMSBlockStream rawdatastream;
  TBDecode::L0::AMSBlockStream rawcalstream;

  bool pri{false};
  bool m_end_of_file{false};
  size_t m_total_size_consumed{0};

  size_t m_read_events{0};

  FILE *calfile{nullptr};
  Calibrations cals{};
  EventAMSL0::JArray<int> JinfMap{0};

  unsigned int m_numBoards = EventAMSL0::GetNJINF() * EventAMSL0::GetNTDRS(); // maximum
  unsigned int m_numBoardsFound = 0;                                          // found during ReadOneEventFromFile

  void DumpRunHeader() override;
  void ResetSetup();

  void InitHistos();

  void OpenFile(const char *rawDir, const char *calDir, int runNum, int calnum) final {
    throw std::runtime_error("DecodeDataAMSL0::OpenFile called without a calibration stop or a run stop number");
  }
  void OpenFile(const char *rawDir, const char *calDir, int runNum, int runStop, int calNumStart, int calNumStop);

  bool ProcessCalibration();

  int ReadOneEventFromFile(FILE *file, EventAMSL0 *event);

  // as a fact is a circular buffer: the event number cannot be more than 255
  // [evno][linf][lef][data]
  std::deque<std::pair<uint16_t, std::map<std::pair<uint16_t, uint16_t>, std::vector<uint16_t>>>> buffer;
  int ReadOneEventFromFile(TBDecode::L0::AMSBlockStream *stream, EventAMSL0 *event, unsigned long int nEvents,
                           uint16_t expTagType = 0, uint16_t expTag = 0);

  virtual int FindPos(int tdrnum, int jinfnum) final;
  virtual int FindCalPos(int tdrnum, int jinfnum) final;
  virtual int ComputeTdrNum(int tdrnum, int jinfnum) final;
};

#endif // DECODE_DECODEDATAAMSL0_HH
