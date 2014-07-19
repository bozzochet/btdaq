#ifndef SLOWCONTROLTYPES_H 
#define SLOWCONTROLTYPES_H


typedef unsigned short ushort;
typedef unsigned int uint;


enum sCMD {
  kNoCMD0,   // 0
  kClear,  // 1
  kSet,    // 2
  kNoCMD1    // 3
};


enum sVOLT {
  kNoVolt0,  // 0
  k80V,   // 1
  k60V,   // 2
  kNoVolt1   // 3
};


enum sACTEL {
  kActelHot,
  kActelCold
};


enum sActelMode {
  kNoActelChange0,
  kActelOn,
  kActelOff,
  kNoActelChange1
};


enum sCommandMode {
  kRead,
  kWrite
};


enum sDCDC {
  kNoDCDCChange0,
  kOn,
  kOff,
  kNoDCDCChange1
};

enum sDCDCmode {
  kAonBon,
  kAonBoff, 
  kAoffBon, 
  kAoffBoff,
  kUnchanged
};


enum sTPSFEaddr {
  kTPSFE0,
  kTPSFE1,
  kTPSFE2,
  kTPSFE3
};



enum sTBSaddr {
  kTBS0,
  kTBS1
};



enum sTBSLR {
  k0Hot, k1Hot, k0Cold, k1Cold
};


enum sTBSTDR {
  kTDR0, kTDR1, kTDR2, kTDR3, kTDR4, kTDR5, kTDR6, kTDR7, kTDR8, kTDR9, kTDR10, kTDR11
};


sTPSFEaddr& operator ++(sTPSFEaddr& addr, int);
sTBSaddr& operator ++(sTBSaddr& addr, int);
sTBSTDR& operator ++(sTBSTDR& addr, int);
sTBSLR& operator ++(sTBSLR& addr, int);


#endif /*SLOWCONTROLTYPES_H*/ 
