//
// Created by Valerio Formato on 12/10/21.
//

#ifndef DECODE_DECODEDATAOCA_HH
#define DECODE_DECODEDATAOCA_HH

#include "DecodeData.hh"

class DecodeDataOCA : public DecodeData {
public:
  DecodeDataOCA(std::string rawDir, std::string calDir, unsigned int runNum);

  virtual int ReadOneEvent() final;

  // dummy for now
  virtual int SkipOneEvent(int evskip = 1) final { return 0; };
  virtual int GetTdrNum(int pos) final;
  virtual int GetTdrType(int pos) final;

private:
  std::string m_rawDir;
  std::string m_calDir;

  std::string m_filename;
  std::string m_calFilename;

  FILE *calfile = nullptr;

  unsigned int m_numBoards = 0;

  void DumpRunHeader() override;
  void InitHistos();
  void OpenFile(const char *rawDir, const char *calDir, int runNum, int ancillary) final;
  bool ProcessCalibration();
  int ReadOneEventFromFile(FILE *file, Event *event);

};

#endif // DECODE_DECODEDATAOCA_HH
