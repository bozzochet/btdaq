//
// Derived from DECODEDATAOCA by Valerio Formato on 15/11/21.
//

#ifndef DECODE_DECODEDATAFOOT_HH
#define DECODE_DECODEDATAFOOT_HH

#include "DecodeData.hh"

class DecodeDataFOOT : public DecodeData {
public:
  DecodeDataFOOT(std::string rawDir, std::string calDir, unsigned int runNum);

  int ReadOneEvent() override;

private:
  std::string m_rawDir;
  std::string m_calDir;

  std::string m_filename;
  std::string m_calFilename;

  FILE *calfile = nullptr;

  unsigned int m_numBoards = 0;

  void DumpRunHeader() override;
  void InitHistos();
  void OpenFile(const char *rawDir, const char *calDir, int runNum, int ancillary) override;
  int ProcessCalibration();
  int ReadOneEventFromFile(FILE *file, Event *event);
};

#endif // DECODE_DECODEDATAFOOT_HH
