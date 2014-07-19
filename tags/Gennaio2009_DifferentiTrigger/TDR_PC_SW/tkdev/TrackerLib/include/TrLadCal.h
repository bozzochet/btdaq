#ifndef __TrLadCal__
#define __TrLadCal__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrLadCal
///\brief Class used to store a Tracker Ladder Calibration
///\ingroup tkdbc
///
/// author: P.Zuccon -- INFN Perugia 20/11/2007
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/17 AO  Some methods are added 
///
//////////////////////////////////////////////////////////////////////////

#include "typedefs.h"
#include "TObject.h"

//! structure used internally to store the Calib data
typedef struct CaloutDSP{
  short int ped[1024];
  short int rsig[1024];
  short int sig[1024];
  short int status[1024];
  short int dspver;
  float S_lowthres;
  float K_lowthres;
  float S_highthres;
  float K_highthres;
  float sigrawthres;
  short int calnum;
  short int calstatus;
  short int tdrstatus;
} CaloutDSP;


class TrLadCal :public TObject {
  
public:  
  static int dead;  //=0x2;


  /// Hardware ID (HwId = iTDR + iCrate*100)
  short int HwId;
  /// Version DSP
  int dspver;
  /// low threshold parameter S (i.e. the parameter to recover the sigma value)
  float S_lowthres;
  /// high threshold parameter S (i.e. the parameter to recover the sigma value) 
  float S_highthres;
  /// low threshold parameter K (i.e. the parameter to recover the sigma value)
  float K_lowthres;
  /// high threshold parameter K (i.e. the parameter to recover the sigma value) 
  float K_highthres;
  /// raw sigma parameter (i.e. the parameter to recover
  float sigrawthres;
  /// \brief The Calibration status
  ///   0 calibration finished
  ///   1 internal trigger is on
  ///   2 error in step 1
  ///   3 error in step 2
  ///   4 error in step 3
  ///   5 error in step 4
  ///   6 internal trigger mode: an external trigger happened
  ///   7 internal trigger mode: no event received
  int calstatus;


private:

  /// Pedestals  
  geant     _Pedestal[1024];
  /// Pedestals sigma
  geant     _SigmaRaw[1024];
  /// Common noise subtracted pedestal sigma
  geant     _Sigma[1024];
  /// Strip status 
  /* 
     Bit 0 to 1  (0x1): dead  channel (sigma raw < mediane sigma raw on VA/2)
     Bit 1 to 1  (0x2): noisy channel (sigma raw > mediane sigma raw on VA*3)
     Bit 2 to 1  (0x4): dead  channel (sigma < mediane sigma on ADC/2)
     Bit 3 to 1  (0x8): noisy channel (sigma > mediane sigma on ADC*1.5)
     Bit 4 to 1 (0x16): non gaussian channel
  */
  short int _Status[1024];
  /// 1 if the ladder calibration is acquired, 0 if not
  short int _filled;

  /// Privete getting methods
  geant _getnum( geant *,int ii);
  short int _getnum( short int *,int ii);

  /// Privete setting methods
  void _setnum( geant *,int ii, geant val);
  void _setnum( short int *,int ii, short int val);
  
public:

  /// Default constructor
  TrLadCal(short int hwid=0);
  /// Copy constructor
  TrLadCal(const TrLadCal& orig);
  /// Default destructor
  ~TrLadCal(){}
  /// Clear data members
  void Clear();
  /// Fill the data members by the DSP block 
  void Fill(CaloutDSP* cc);

  /// Get ii-th strip pedestal ADC value
  geant Pedestal(int ii)   { return _getnum(_Pedestal,ii);}
  /// Get ii-th strip sigma (CN subtracted pedestal sigma) ADC value
  geant Sigma(int ii)      { return _getnum(_Sigma,ii);}
  /// Get ii-th strip sigma-pedestal ADC value
  geant SigmaRaw(int ii)   { return _getnum(_SigmaRaw,ii);}
  /// Get ii-th status 
  short int Status(int ii) { return _getnum(_Status,ii);}

  /// Get the HwId
  short int GetHwId() const   { return HwId; }
  /// Get ii-th strip pedestal ADC value
  geant GetPedestal(int ii)   { return _getnum(_Pedestal,ii); }
  /// Get ii-th strip sigma (CN subtracted pedestal sigma) ADC value
  geant GetSigma(int ii)      { return _getnum(_Sigma,ii); }
  /// Get ii-th strip sigma-pedestal ADC value
  geant GetSigmaRaw(int ii)   { return _getnum(_SigmaRaw,ii); }
  /// Get ii-th status 
  short int GetStatus(int ii) { return _getnum(_Status,ii); }

  /// Set ii-th strip pedestal ADC value
  void SetPedestal(int ii,geant val)   { _setnum(_Pedestal,ii,val);}
  /// Set ii-th strip sigma (CN subtracted pedestal sigma) ADC value
  void SetSigma(int ii,geant val)      { _setnum(_Sigma,ii,val);}
  /// Set ii-th strip sigma-pedestal ADC value
  void SetSigmaRaw(int ii,geant val)   { _setnum(_SigmaRaw,ii,val);}
  /// Set ii-th status 
  void SetStatus(int ii,short int val) { _setnum(_Status,ii,val);}

  /// The ladder calibration is acquired?
  bool IsFilled()  { return _filled==1; }
  /// Set the ladder calibration acquisition status
  void SetFilled() { _filled = 1; } 

  /// Simulate the DSP mediane calculation 
  short int DSPSmallest(short int a[], int n, int k);
  /// Get the sigma raw on the mediane (on a VA) using the DSP code
  geant GetDSPSigmaRawOnMediane(int ii); 
  /// Get the sigma on the mediane (on an ADC) using the DSP code 
  geant GetDSPSigmaOnMediane(int ii); 

  /// Get the mean sigma (strips with status==0) for the p (1) and n (0) sides 
  geant GetSigmaMean(int side);
  /// Get the sigma RMS (strips with status==0) for the p (1) and n (0) sides  
  geant GetSigmaRMS(int side);
  /// Get the number of channels for a given status
  int   GetnChannelsByStatus(int statusbit); 


  /// Print info (long format if long_format>0)
  void PrintInfo(int long_format = 0);
  /// Copy the data part from another calibration
  void CopyCont(TrLadCal& orig);
  /// Operator - 
  TrLadCal& operator-(TrLadCal &);
  /// Operator =  
  TrLadCal& operator=(TrLadCal& orig);

  /// It copy the calibration data to a linear vector 
  ///  in memory starting at address offset
  int Cal2Lin(float* offset);


  /// It fill  the calibration from o a linear vector 
  ///  in memory starting at address offset
  int Lin2Cal(float* offset);


  static int GetSize() {return (1024*4+8);}

  ClassDef(TrLadCal,2);

};


#endif
