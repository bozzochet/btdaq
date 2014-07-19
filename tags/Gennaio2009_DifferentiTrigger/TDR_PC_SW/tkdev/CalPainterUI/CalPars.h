#ifndef CalPars_h
#define CalPars_h



class CalPars {

  public:
  //! Number of Event for Pedestals
  static int NumPed;
  //! Number of Event for Raw Sigma
  static int NumRsig;
  //! Number of Events for Sigma
  static int NumSig;
  //! Number of Events for Not Gausssian
  static int NumNG;
  //! Number of sigma for NG Calculation
  static int NSigNG;
  //! fraction of events out of NSigNG sigmas (value in per mille)
  static int PercNG;
  //! 
  static int low_raw_noise;
  static int high_raw_noise;
  static int low_noise;
  static int high_noise;

  CalPars(){};
  virtual  ~CalPars(){}
  
 static void SetNumEv(int ped,int rsig,int sig,int ng);
  static void SetFlagPar(int lr,int hr,int ll,int hh);
  static void SetNGParS(int nsig,int nper){   NSigNG=nsig;PercNG=nper;}
  static void PrintPars();
  static char* FillParString(char* pstring);
  static float GetNGTh(){return NumNG*(PercNG/1000.);}

};




#endif
