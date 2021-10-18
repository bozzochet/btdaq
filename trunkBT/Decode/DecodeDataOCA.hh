//
// Created by Valerio Formato on 12/10/21.
//

#ifndef DECODE_DECODEDATAOCA_HH
#define DECODE_DECODEDATAOCA_HH

#include "DecodeData.hh"

class DecodeDataOCA : public DecodeData {
public:
  DecodeDataOCA(std::string rawDir, std::string calDir, unsigned int runNum);

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
  bool ProcessCalibration();
  int ReadOneEventFromFile(FILE *file, Event *event);
};

#endif // DECODE_DECODEDATAOCA_HH
