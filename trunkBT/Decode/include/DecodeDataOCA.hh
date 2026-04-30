//
// Created by Valerio Formato on 12/10/21.
//

#ifndef DECODE_DECODEDATAOCA_HH
#define DECODE_DECODEDATAOCA_HH

#include "DecodeData.hh"
#include "Event.hh"

class DecodeDataOCA : public DecodeData {
public:
  using EventOCA = Event<1, 24, 64, 5, 10, 0>;
  using calibOCA = calib<EventOCA::GetNCHAVA() * EventOCA::GetNVAS()>;
  //  using RHClassOCA = RHClass<EventOCA::GetNJINF(), EventOCA::GetNTDRS()>;
  //  using Calibrations = std::array<std::array<calibOCA, EventOCA ::GetNTDRS()>, EventOCA::GetNJINF()>;
  using CalibrationsOCA =
      Calibrations<EventOCA::GetNJINF(), EventOCA::GetNTDRS(), EventOCA::GetNCHAVA() * EventOCA::GetNVAS()>;

  DecodeDataOCA(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int calnum,
                bool _kOnlyProcessCal = false, bool _kExtCalfile = false);

  virtual ~DecodeDataOCA();

  virtual ::FlavorConfig FlavorConfig() final {
    return {EventOCA::GetNJINF(), EventOCA::GetNTDRS(), EventOCA::GetNCHAVA(), EventOCA::GetNADCS(),
            EventOCA::GetNVAS()};
  };

  virtual TString EventClassname() final { return ev->ClassName(); };

  bool ReadFileHeader(FILE *file, RHClassOCA *rhc);

  virtual int ReadOneEvent() final;

  virtual void ClearEvent() final { ev->Clear(); };

  // dummy for now
  virtual int SkipOneEvent(int evskip = 1) final { return 0; };

  virtual int GetNTDRS() final { return EventOCA::GetNTDRS(); }
  virtual int GetNJINF() final { return EventOCA::GetNJINF(); }

  //  Calibrations GetCalibrations() const { return cals; };
  CalibrationsOCA &GetCalibrations() { return cals; };

  void GetCalFilePrefix(char *calfileprefix, long int runnum) override {
    sprintf(calfileprefix, "%s/%ld", m_calDir.c_str(), runnum);
  }

private:
  FILE *calfile = nullptr;
  //  Calibrations cals{};
  CalibrationsOCA cals{};
  EventOCA::JArray<int> JinfMap{0};

  unsigned int m_numBoards = 12;     // maximum
  unsigned int m_numBoardsFound = 0; // found during ReadOneEventFromFile

  void DumpRunHeader() override;

  void InitHistos();

  void OpenFile(const char *rawDir, const char *calDir, int runNum, int calNum) final;

  bool ProcessCalibration(bool _kExtCalfile = false);

  int ReadOneEventFromFile(FILE *file, EventOCA *event, bool kCal = false);
};

#endif // DECODE_DECODEDATAOCA_HH
