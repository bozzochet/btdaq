// Define ladder
#ifndef FILE_DEFINELADDER_SEEN
#define FILE_DEFINELADDER_SEEN


class DefineLadder{

 protected:
  static char fName[9];    // Name of the ladder fName=
  static char fVoltage;   // (L)ow or (H)igh voltaje
  static char fWaffers[3]; // # of Silicon waffers (01,02,....10,11...)
  static char fAssembly;  // Assembly Line (G)eneve, (P)erugia, (Z)urich
  static char fLayer;     // (I)nner, (O)uter, (T)est 
  static char fSerial[4];  // serial number per ass. line (001,.,090,.,230,.)

 private:

 public:
  DefineLadder();
  ~DefineLadder();
  char *GetLadderName();
};
#endif

