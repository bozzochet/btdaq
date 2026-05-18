//
// Created by Valerio Formato on 12/10/21.
//

#ifndef DECODE_DECODEDATAHEF_HH
#define DECODE_DECODEDATAHEF_HH

#include "DecodeData.hh"
#include "Event.hh"

class DecodeDataHEF : public DecodeData {
public:
  using EventHEF = Event<1, 24, 64, 7, 14, 0>;
  using calibHEF = calib<EventHEF::GetNCHAVA() * EventHEF::GetNVAS()>;
  //  using RHClassHEF = RHClass<EventHEF::GetNJINF(), EventHEF::GetNTDRS()>;
  //  using Calibrations = std::array<std::array<calibHEF, EventHEF ::GetNTDRS()>, EventHEF::GetNJINF()>;
  using CalibrationsHEF =
      Calibrations<EventHEF::GetNJINF(), EventHEF::GetNTDRS(), EventHEF::GetNCHAVA() * EventHEF::GetNVAS()>;

  DecodeDataHEF(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int calnum,
                bool _kOnlyProcessCal = false, bool _kExtCalfile = false);

  virtual ~DecodeDataHEF();

  virtual ::FlavorConfig FlavorConfig() final {
    return {EventHEF::GetNJINF(), EventHEF::GetNTDRS(), EventHEF::GetNCHAVA(), EventHEF::GetNADCS(),
            EventHEF::GetNVAS()};
  };

  virtual TString EventClassname() final { return ev->ClassName(); };

  bool ReadFileHeader(FILE *file, RHClassHEF *rhc);

  virtual int ReadOneEvent() final;

  virtual void ClearEvent() final { ev->Clear(); };

  // dummy for now
  virtual int SkipOneEvent(int evskip = 1) final { return 0; };

  virtual int GetNTDRS() final { return EventHEF::GetNTDRS(); }
  virtual int GetNJINF() final { return EventHEF::GetNJINF(); }

  //  Calibrations GetCalibrations() const { return cals; };
  CalibrationsHEF &GetCalibrations() { return cals; };

  void GetCalFilePrefix(char *calfileprefix, long int runnum) override {
    sprintf(calfileprefix, "%s/%ld", m_calDir.c_str(), runnum);
  }

private:
  FILE *calfile = nullptr;
  //  Calibrations cals{};
  CalibrationsHEF cals{};
  EventHEF::JArray<int> JinfMap{0};

  unsigned int m_numBoards = 12;     // maximum
  unsigned int m_numBoardsFound = 0; // found during ReadOneEventFromFile

  void DumpRunHeader() override;

  void InitHistos();

  void OpenFile(const char *rawDir, const char *calDir, int runNum, int calNum) final;

  bool ProcessCalibration(bool _kExtCalfile = false);

  int ReadOneEventFromFile(FILE *file, EventHEF *event, bool kCal = false);
};

#endif // DECODE_DECODEDATAHEF_HH
