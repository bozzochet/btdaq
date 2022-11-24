//
// Derived from DECODEDATAOCA by Valerio Formato on 15/11/21.
//

#ifndef DECODE_DECODEDATAFOOT_HH
#define DECODE_DECODEDATAFOOT_HH

#include "DecodeData.hh"
#include "GenericEvent.hh"

class DecodeDataFOOT : public DecodeData {
public:
  using EventFOOT = GenericEvent<1, 24, 64, 5, 10, 0>;
  using calibFOOT = calib<EventFOOT::GetNCHAVA() * EventFOOT::GetNVAS()>;

public:
  EventFOOT *ev;

  DecodeDataFOOT(std::string rawDir, std::string calDir, unsigned int runNum, unsigned int calNum);

  virtual ::FlavorConfig FlavorConfig() final {
    return {EventFOOT::GetNJINF(), EventFOOT::GetNTDRS(), EventFOOT::GetNCHAVA(), EventFOOT::GetNADCS(),
            EventFOOT::GetNVAS()};
  };
  virtual TString EventClassname() final {return ev->ClassName(); };

  int ReadOneEvent() override;
  virtual void ClearEvent() final { ev->Clear(); };

private:
  std::string m_rawDir;
  std::string m_calDir;

  std::string m_filename;
  std::string m_calFilename;

  FILE *calfile = nullptr;
  calibFOOT cals[EventFOOT::GetNJINF() * EventFOOT::GetNTDRS()]{};

  unsigned int m_numBoards = 0;

  void DumpRunHeader() override;
  void InitHistos();
  void OpenFile(const char *rawDir, const char *calDir, int runNum, int ancillary) override;
  int ProcessCalibration();
  int ReadOneEventFromFile(FILE *file, EventFOOT *event);

  // dummy for now
  virtual int SkipOneEvent(int evskip = 1) final { return 0; };
  virtual int GetTdrNum(size_t pos) override;
  virtual int GetTdrType(size_t pos) override;
};

#endif // DECODE_DECODEDATAFOOT_HH
