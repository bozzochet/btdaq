//
// Created by Valerio Formato on 21/11/22.
//

#ifndef DECODE_DECODEDATAAMSL0_HH
#define DECODE_DECODEDATAAMSL0_HH

#include "DecodeData.hh"
#include "GenericEvent.hh"

#include "TBDecode/AMSL0/AMSBlock.h"
#include "TBDecode/AMSL0/AMSBlockStream.h"

class DecodeDataAMSL0 : public DecodeData {
public:
  using EventAMSL0 = GenericEvent<1, 9, 64, 8, 16, 0>;
  using calibAMSL0 = calib<EventAMSL0::GetNCHAVA() * EventAMSL0::GetNVAS()>;
  using RHClassAMSL0 = RHClass<EventAMSL0::GetNJINF(), EventAMSL0::GetNTDRS()>;

  EventAMSL0 *ev;
  RHClassAMSL0 *rh;

  DecodeDataAMSL0(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int runStop,
                  unsigned int calStart, unsigned int calStop);

  virtual ~DecodeDataAMSL0();

  virtual ::FlavorConfig FlavorConfig() final {
    return {EventAMSL0::GetNJINF(), EventAMSL0::GetNTDRS(), EventAMSL0::GetNCHAVA(), EventAMSL0::GetNADCS(),
            EventAMSL0::GetNVAS()};
  };

  virtual TString EventClassname() final { return ev->ClassName(); };

  bool ReadFileHeader(FILE *file, RHClassAMSL0 *rhc);
  bool ReadFileHeader(TBDecode::L0::AMSBlockStream *rawfilestream, RHClassAMSL0 *rhc);

  int ReadOneEvent() final;

  virtual void ClearEvent() final { ev->Clear(); };

  // dummy for now
  virtual int SkipOneEvent(int evskip = 1) final { return 0; };

  virtual int GetTdrNum(size_t pos) final;

  virtual int GetTdrType(size_t pos) final;

  int EndOfFile() final;

  void GetCalFilePrefix(char *calfileprefix, long int runnum) override {
    // First 4 digits: dir number
    unsigned int dirNum = runnum / 1000;
    // Last 4 digits: block number
    unsigned int blockNum = runnum % 1000;
    sprintf(calfileprefix, "%s/%04d/%03d", m_calDir.c_str(), dirNum, blockNum);
  }

  void SetDecodeStyle(int _style) { decodestyle = _style; }

private:
  int decodestyle{false};

  TBDecode::L0::AMSBlockStream rawdatastream;
  TBDecode::L0::AMSBlockStream rawcalstream;

  bool pri{false};
  bool m_end_of_file{false};
  size_t m_total_size_consumed{0};

  size_t m_read_events{0};

  FILE *calfile{nullptr};
  calibAMSL0 cals[EventAMSL0::GetNJINF() * EventAMSL0::GetNTDRS()]{};
  EventAMSL0::JArray<int> JinfMap{0};

  unsigned int m_numBoards = 12;     // maximum
  unsigned int m_numBoardsFound = 0; // found during ReadOneEventFromFile

  void DumpRunHeader() override;

  void InitHistos();

  void OpenFile(const char *rawDir, const char *calDir, int runNum, int calnum) final {
    throw std::runtime_error("DecodeDataAMSL0::OpenFile called without a calibration stop or a run stop number");
  }
  void OpenFile(const char *rawDir, const char *calDir, int runNum, int runStop, int calNumStart, int calNumStop);

  bool ProcessCalibration();

  int ReadOneEventFromFile(FILE *file, EventAMSL0 *event);
  int ReadOneEventFromFile(TBDecode::L0::AMSBlockStream *stream, EventAMSL0 *event);
};

#endif // DECODE_DECODEDATAAMSL0_HH
