//
//  NB Please increase the version number in corr classdef 
//  for any class modification
//



#ifndef __AMSROOT__
#define __AMSROOT__
#include <list>
#include <vector>
#include <iostream>
#include "TObject.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TSelector.h"
#include "TROOT.h"
#include "TBranch.h"
#include "TMatrixD.h"
#include "TStopwatch.h"
#include <time.h>
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "id.h"
#include <math.h>
using namespace std;
#ifdef __SLC3__
char* operator+( std::streampos&, char* );
#endif
#ifndef __ROOTSHAREDLIBRARY__
class AMSAntiCluster;
class AMSAntiMCCluster;
class AMSBeta;
class AMSCharge;
class AMSEvent;
class AMSEcalHit;
class AMSmceventg;
class AMSmctrack;
class AMSParticle;
class AMSRichMCHit;
class AMSRichRawEvent;
class AMSRichRing;
class AMSNtuple;
class AMSTOFCluster;
class AMSTOFMCCluster;
class AMSTRDCluster;
class AMSTRDMCCluster;
class AMSTRDRawHit;
class AMSTRDSegment;
class AMSTRDTrack;
class AMSTrCluster;
class AMSTrMCCluster;
class AMSTrRawCluster;
class AMSTrTrack;
class AMSVtx;
class AMSTrRecHit;
class Ecal1DCluster;
class AMSEcal2DCluster;
class AMSEcalShower;
class TOF2RawCluster;
class TOF2RawSide;
class Trigger2LVL1;
class TriggerLVL302;
class EventNtuple02;
#else
class AMSAntiCluster{};
class AMSAntiMCCluster{};
class AMSBeta{};
class AMSCharge{};
class AMSEvent{};
class AMSEcalHit{};
class AMSmceventg{};
class AMSmctrack{};
class AMSParticle{};
class AMSRichMCHit{};
class AMSRichRawEvent{};
class AMSRichRing{};
class AMSNtuple{};
class AMSTOFCluster{};
class AMSTOFMCCluster{};
class AMSTRDCluster{};
class AMSTRDMCCluster{};
class AMSTRDRawHit{};
class AMSTRDSegment{};
class AMSTRDTrack{};
class AMSTrCluster{};
class AMSTrMCCluster{};
class AMSTrRawCluster{};
class AMSTrTrack{};
class AMSVtx{};
class AMSTrRecHit{};
class Ecal1DCluster{};
class AMSEcal2DCluster{};
class AMSEcalShower{};
class TOF2RawCluster{};
class TOF2RawSide{};
class Trigger2LVL1{};
class TriggerLVL302{};
class EventNtuple02{};
#endif

//!  Root Header class
/*!
  Contains:

  general info   (public)

  shuttle/iss parameters  public)
  (this includes:
   ISS -z axis (LVLH) pointing direction in external reference frame
   AMS z axis (according to 12 degree tilt in roll) in external reference frame
  The reference frames are the equatorial and galactic frames, which are fixed to the sky and not to the Earth. More info in upcoming AMS note)     

  object counters, aka arrays dimensions (protected, access via coo functions)
 \sa AMSEventR 

 \author v.choutko@cern.ch

*/
class HeaderR{
static char _Info[255];
 public:




// general info block

  unsigned int Run;         ///<run  number
  unsigned int RunType;     ///<runtype(data), RNDM(1) MC
  unsigned int Event;      ///<event number
  unsigned int Status[2];  ///<event status
  int Raw;            ///<raw event length in bytes
  int Version;        ///< os number (low 2 bits) program build number (high 10 bits)
  int Time[2];        ///<unix time + usec time(data)/RNDM(2) MC



// shuttle/ISS parameters

   float RadS;    ///<iss orbit altitude cm  (I2000 coo sys)
   float ThetaS;  ///<theta (GTOD rad)  
   float PhiS;    ///<phi   (GTOD rad)
   float Yaw;     ///<ISS yaw (LVLH rad) 
   float Pitch;   ///<ISS pitch (LVLH rad) 
   float Roll;   ///<ISS roll (LVLH rad)  
   float VelocityS;     ///< ISS velocity (rad/sec) 
   float VelTheta;     ///< ISS speed theta (GTOD rad) 
   float VelPhi;       ///< ISS speed phi (GTOD rad)  
   float ThetaM;        ///< magnetic (calculated for an eccentric dipole coo system) theta  rad
   float PhiM;          ///< magnetic (calculated for an eccentric dipole coo system)phi  rad
   // pointing direction in equatorial and galactic systems
   float ISSEqAsc; ///<(ISN) ISS pointing direction (equat. right ascension)
   float ISSEqDec; ///<(ISN) ISS pointing direction (equat. declination)
   float ISSGalLat; ///<(ISN) ISS pointing direction (gal. latitude)
   float ISSGalLong; ///<(ISN) ISS pointing direction (gal. longitude)   
   float AMSEqAsc; ///<(ISN) AMS pointing direction (equat. right ascension)
   float AMSEqDec; ///<(ISN) AMS pointing direction (equat. declination)
   float AMSGalLat; ///<(ISN) AMS pointing direction (gal. latitude)
   float AMSGalLong; ///<(ISN) AMS pointing direction (gal. longitude) 

   //

// counters
protected:
int   EcalHits;              
int   EcalClusters;
int   Ecal2DClusters;
int   EcalShowers;
int   RichHits;
int   RichRings;
int   TofRawClusters;
int   TofRawSides;
int   TofClusters;  
int   AntiClusters;
int   TrRawClusters;
int   TrClusters;
int   TrRecHits;
int   TrTracks;
int   TrdRawHits;
int   TrdClusters;
int   TrdSegments;
int   TrdTracks;
int   Level1s;
int   Level3s;
int   Betas; 
int   Vertexs; 
int   Charges;  
int   Particles;  
int   AntiMCClusters;
int   TrMCClusters;
int   TofMCClusters;
int   TrdMCClusters;
int   RichMCClusters;
int   MCTracks;
int   MCEventgs;
public:

  /// A constructor.
  HeaderR(){};  
#ifndef __ROOTSHAREDLIBRARY__
  void   Set(EventNtuple02 *ptr);
#endif
  friend class AMSEventR;
  /// \return human readable info about HeaderR
  char * Info(){
                         double cp=cos(Pitch);
                         double sp=sin(Pitch);
                         //double cy=cos(Yaw);
                         double sy=sin(Yaw);
                         double cr=cos(Roll);
                         double sr=sin(Roll);
                         double cams=-sr*sy*sp+cr*cp;
                         cams=acos(cams)*180/3.1415926; 
    sprintf(_Info,"Header: Status %s, Lat %6.1f^{o}, Long %6.1f^{o}, Rad %7.1f km, Velocity %7.2f km/s,  #Theta^{M} %6.2f^{o}, Zenith %7.2f^{o} ",(Status[0] & (1<<30))?"Error ":"OK ",ThetaS*180/3.1415926,PhiS*180/3.1415926,RadS/100000,VelocityS*RadS/100000, ThetaM*180/3.1415926,cams);
  return _Info;
  }

  virtual ~HeaderR(){};
  ClassDef(HeaderR,4)       //HeaderR
};



//!  Ecal Hits Structure
/*!

   \author e.choumilov@cern.ch
*/
class EcalHitR {
public:
  unsigned int   Status;   ///< Statusword
  int   Idsoft;   ///< 4digits number SPPC=SuperLayer/PM/subCell  0:8/0:35/0:3  
               /*!<
Idsoft SubCells(pixels) numbering(+Z is top):\n
---------------|0|1|------------------\n
pm1(at -x/y)>> ------>> pm36(at +x/y) \n
---------------|2|3|------------------\n
                */
  int   Proj;     ///< projection (0-x,1-y)
  int   Plane;    ///< ECAL plane number (0,...)
  int   Cell;     ///< ECAL Cell number (0,...)    
  float Edep;     ///< ECAL measured energy (MeV)
  float EdCorr;   ///< ECAL PMsaturation1-correction(MeV) added to Edep
  float AttCor;   ///<  Attenuation Correction applied (w/r  to center of ecal) (MeV)
  float Coo[3];   ///< ECAL Coo (cm)
  float ADC[3];   ///< ECAL (ADC-Ped) for Hi/Low/Dynode channels
  float Ped[3];   ///< ECAL Pedestals   
  float Gain;     ///<  1/gain (!)

  EcalHitR(AMSEcalHit *ptr);
  EcalHitR(){};
  virtual ~EcalHitR(){};
ClassDef(EcalHitR,1)       //EcalHitR
};

/// EcalCluster structure
/*!

   \author v.choutko@cern.ch
*/
class EcalClusterR {
static char _Info[255];
public:
  unsigned int Status;    ///<  Statusword
                 /*!< valid values are:\n
                                                     WIDE=2   \n
                                                     BAD =16  \n
                                                     USED=32  \n
                                                     NEAR=102 \n 
                                                     LEAK=16777216  \n
                                                     CATASTROPHICLEAK=33554432 \n
                                                     JUNK=67108864 \n
                */

  int Proj;   ///< projection (0x, 1y)
  int Plane;  ///< layer no [0:17]  0-top
  int Left;   ///< cluster leftmost hit pos [0:72]
  int Center; ///< Cluster Max Energy Hit pos
  int Right;  ///< cluster rightmost hit pos [0:72]
  float Edep; ///< energy deposition (mev)
  float SideLeak;  ///< side leak estimation (mev)
  float DeadLeak;  ///<  dead hit leak est (mev)
  float Coo[3];    ///< cluster coo (cm)
protected:
  vector <int> fEcalHit;   ///< pointers to EcalHitR collection, protected
public:
  /// access function to EcalHitR objects used
  /// \return number of EcalHitR used
  int NEcalHit()const {return fEcalHit.size();}
  /// access function to EcalHitR objects used
  /// \param i index of fEcalHit vector
  /// \return index of EcalHitR object in collection or -1
  int iEcalHit(unsigned int i){return i<fEcalHit.size()?fEcalHit[i]:-1;}
  /// access function to EcalHitR collection   
  /// \param i index of fEcalHit vector
  /// \return pointer to EcalHitR object or 0
  EcalHitR * pEcalHit(unsigned int i);
  
  EcalClusterR(){};
  EcalClusterR(Ecal1DCluster *ptr);
  friend class Ecal1DCluster;
  friend class AMSEventR;
  /// \param number index in container
  /// \return human readable info about EcalClusterR
  char * Info(int number=-1){
    sprintf(_Info,"EcalCluster No %d Layer=%d Proj=%d Coo=(%5.2f,%5.2f,%5.2f) E_{Dep}(MeV)=%5.2f Multip=%d",number,Plane,Proj,Coo[0],Coo[1],Coo[2],Edep,NEcalHit());
  return _Info;
  } 
  virtual ~EcalClusterR(){};
ClassDef(EcalClusterR,1)       //EcalClusterR
};


//!  Ecal2DClusterR Structure

/*!

   \author v.choutko@cern.ch
*/

class Ecal2DClusterR{
public:
  unsigned int Status;   ///< statusword
  int Proj;     ///< projection 0-x 1-y
  float Edep;   ///< energy (mev)
  float Coo;    ///< str line fit coo (cm)
  float Tan;    ///< str line fit tangent
  float Chi2;   ///< str line fit chi2

protected:
  vector <int> fEcalCluster;   ///< indexes of EcalClsterR collection
public:
  /// access function to EcalClusterR objects used
  /// \return number of EcalClusterR used
  int NEcalCluster()const {return fEcalCluster.size();}
  /// access function to EcalClusterR collection  
  /// \param i index of fEcalCluster vector  
  /// \return index of EcalClusterR collection or -1
  int iEcalCluster(unsigned int i){return i<fEcalCluster.size()?fEcalCluster[i]:-1;}
  /// access function to EcalClusterR collection   
  /// \param i index of fEcalCluster vector 
  /// \return pointer to EcalClusterR collection or 0
  EcalClusterR * pEcalCluster(unsigned int i);

  Ecal2DClusterR(){};
  Ecal2DClusterR(AMSEcal2DCluster *ptr);
  virtual ~Ecal2DClusterR(){};
ClassDef(Ecal2DClusterR,1)       //Ecal2DClusterR
friend class AMSEcal2DCluster;
friend class AMSEventR;
};


/*!
  \class EcalShowerR
  \brief 3d ecal shower description

   \sa ecalrec.h ecalrec.C
   \author v.choutko@cern.ch
*/


class EcalShowerR {
static char _Info[255];
public:
  unsigned int   Status;    ///< status word \sa EcalHitR status
  float Dir[3];    ///< direction cos array 
  float EMDir[3];  ///< direction cos array for emag type shower
  float Entry[3];  ///< entry point (cm)
  float Exit[3];   ///< exit point(cm)
  float CofG[3];  ///< center of gravity (cm)
  float ErDir;    ///< 3d angle error
  float Chi2Dir;  ///<  chi2 direction fit
  float FirstLayerEdep; ///< front energy dep (Mev)
  float EnergyC; ///< shower energy (gev)
  float Energy3C[3]; ///< energy(+-2,+-5, +-8 cm)/energy ratios
  float ErEnergyC;   ///< energy error (gev)
  float DifoSum;     ///<  (E_x-E_y)/(E_x+E_y)
  float SideLeak;    ///< rel side leak
  float RearLeak;    ///< rel rear leak
  float DeadLeak;    ///< rel dead leak
  float AttLeak;     ///< rel att length correction
  float NLinLeak;   ///<  rel non-lin correction
  float OrpLeak;   ///<  fraction of shower energy outside core.
  float Orp2DEnergy; ///< orphaned Ecal2DClusterR energy (if any) (geV)
  float Chi2Profile;  ///< chi2 profile fit (by gamma function) 
  float ParProfile[4]; ///< normalization, shower max (cm), rad length, rel rear leak ) for profile fit
  float Chi2Trans;     ///< chi2 transverse fit (sum of two exp)
  float SphericityEV[3]; ///< sphericity tensor eigen values

protected:
  vector <int> fEcal2DCluster;  ///< indexes to Ecal2DClusterR collection
public:
  /// access function to Ecal2DClusterR objects used
  /// \return number of Ecal2DClusterR used
  int NEcal2DCluster()const {return fEcal2DCluster.size();}
  /// access function to Ecal2DClusterR objects
  /// \param i index of fEcal2DCluster vector
  /// \return index of Ecal2DClusterR object in collection or -1
  int iEcal2DCluster(unsigned int i){return i<fEcal2DCluster.size()?fEcal2DCluster[i]:-1;}
  /// access function to Ecal2DClusterR objects   
  /// \param i index of fEcal2DCluster vector
  /// \return pointer to Ecal2DClusterR object  or 0
  Ecal2DClusterR * pEcal2DCluster(unsigned int i);
  /// \param number index in container
  /// \return human readable info about EcalShowerR
  char * Info(int number=-1){
    sprintf(_Info,"EcalShower No %d Energy=%7.3g#pm%5.2g  #theta=%4.2f #phi=%4.2f Coo=(%5.2f,%5.2f,%5.2f) #chi^{2}=%7.3g Asymm=%4.2f Leak_{Side,Rear,Dead,Att,NonLin,Orp}=(%4.2f,%4.2f,%4.2f,%4.2f,%4.2f,%4.2f) Max=%4.2f",number, EnergyC,ErEnergyC,acos(Dir[2]),atan2(Dir[1],Dir[0]),CofG[0],CofG[1],CofG[2],Chi2Dir,DifoSum,SideLeak,RearLeak,DeadLeak,AttLeak,NLinLeak,Orp2DEnergy/(EnergyC+1e-10),ParProfile[1]);
  return _Info;
  } 


  EcalShowerR(){};
  EcalShowerR(AMSEcalShower *ptr);
friend class AMSEcalShower;
friend class AMSEventR;

  virtual ~EcalShowerR(){};
ClassDef(EcalShowerR,1)       //EcalShowerR

};





/// TofRawClusterR structure

/*!
 \author e.choumilov@cern.ch

*/

class TofRawClusterR {
public:
  unsigned int   Status;  ///< statusword
                /*!<
                                              // bit8(128)  -> bad time-history on any side
                                              // bit9(256)  -> "1-sided" counter(1-side meas. is missing)
                                              // bit10(512) -> ignore time-measurement(as known from DB),not used
					      // bit11(1024)-> missing side number(0->s1,1->s2)
                                              // bit12(2048)-> recovered from 1-sided (bit256 also set)
                                              // bit13(4096)-> no bestLT/sumHT-hit matching(when requested) on any side
					      // bit6(32)   -> used for TofCluster 
   */
  int   Layer;   ///< Tof plane 1(top)...4
  int   Bar;     ///< Tof Bar number 1...14
  float adca[2]; ///< Anode raw signal(adc), side=1-2
  float adcd[2]; ///< Dynode(equilized sum of pmts) raw signal(adc) 
  float adcdr[2][3]; ///< Dynode(pm=1-3) raw signals(adc) 
  float sdtm[2];  ///< A-noncorrected side times
  float edepa;   ///< Anode Edep (mev)
  float edepd;   ///< Dynode Edep (mev)
  float time;    ///< Time (ns)
  float cool;     ///< Long.coord.(cm)
  TofRawClusterR(){};
  TofRawClusterR(TOF2RawCluster *ptr);

  virtual ~TofRawClusterR(){};
  ClassDef(TofRawClusterR ,4)       //TofRawClusterR
};

/// TofRawSideR structure

/*!
 \author e.choumilov@cern.ch

*/

class TofRawSideR {
public:
  int swid;//short softw-id(LBBS)
  int hwid;//short hardw-id(C(rate)S(lot), for SFET(C)-type measurements:stdc,ftdc,adca,adcd
  int stat;// 0/1->ok/bad
  int nftdc;//numb.of FTtime-hits
  int ftdc[8];//FTtime-hits(FastTrigger-channel time, ns)
  int nstdc;//numb.of LTtime-hits
  int stdc[16];//LTtime-hits(LowThreshold-channel time. ns)
  int nsumh;//numb.of SumHTtime-hits
  int sumht[16];//SumHTtime-hits(half_plane_sum of HighThreshold-channel time history, ns)
  int nsumsh;//numb.of SumSHTtime-hits
  int sumsht[16];//SumSHTtime-hits(half_plane_sum of SuperHighThreshold-channel time history, ns)
  float adca;//Anode signal(ADC-counts, ped-subtracted if not PedCal-run)
  int nadcd;//number of Dynode nonzero(!) signals
  float adcd[3];//Dynode signals(ADC-counts, positional(keep "0"s), ped-subtracted if not PedCal-run)
  float temp;//temperature(given by probe in SFET/SFEA slots)  

  TofRawSideR(){};
  TofRawSideR(TOF2RawSide *ptr);
  friend class TOF2RawSide;
  friend class AMSEventR;
  virtual ~TofRawSideR(){};
  
  ClassDef(TofRawSideR ,3)       //TofRawSideR
};




/// TofClusterR structure
/*!
 \author e.choumilov@cern.ch

*/

class TofClusterR {
protected:
static char _Info[255];
public:
  unsigned int Status;   ///< Statusword(as for RawCluster + ones below:
                /*!<
                                                 // bit3(4)  -> ambig
						 // bit5(16) -> BAD((bit9 | bit10) & !bit12))
						 // bit8 is ORed over cluster-members
   */
  int Layer;    ///<  Tof plane 1(top)...4
  int Bar;      ///< Tof Bar number 1...14
  float Edep;  ///< TOF energy loss (MeV) from anode
  float Edepd; ///< TOF energy loss (MeV) from dinode
  float Time;  ///<TOF time (sec, wrt FastTrig-time)
  float ErrTime;  ///< Error in TOF time
  float Coo[3];   ///< TOF Coo (cm)
  float ErrorCoo[3];   ///< Error TOF Coo (cm)
protected:
  vector<int> fTofRawCluster;   //indexes of TofRawclusterR's current object is made of.
public:
  /// access function to TofRawClusterR objects used
  /// \return number of TofRawClusterR used
  int NTofRawCluster()const {return fTofRawCluster.size();}
  /// access function to TofRawClusterR    used
  /// \param i index of fTofRawCluster vector
  /// \return index of TofRawClusterR object in TofRawCluster collection or -1
  int iTofRawCluster(unsigned int i){return i<fTofRawCluster.size()?fTofRawCluster[i]:-1;}
  /// access function to TofRawClusterR's   current object is made of.
  /// \param i index of fTofRawCluster vector
  /// \return pointer to TofRawClusterR object or 0
  TofRawClusterR * pTofRawCluster(unsigned int i);

  /// \param number index in container
  /// \return human readable info about TofClusterR
  char * Info(int number=-1){
    sprintf(_Info,"ToF Cluster No %d S%dB%d: time=%3.1f#pm%3.1f, E_{Dep}(MeV)=%4.1f, at (%5.1f,%5.1f,%5.1f)#pm(%5.1f,%5.1f,%5.1f)",number,Layer,Bar,Time*1.e9,ErrTime*1.e9,Edep,Coo[0],Coo[1],Coo[2],ErrorCoo[0],ErrorCoo[1],ErrorCoo[2]);
  return _Info;
  } 
  TofClusterR(){};
  TofClusterR(AMSTOFCluster *ptr );
  friend class AMSTOFCluster;
  friend class AMSEventR;
  virtual ~TofClusterR(){};
ClassDef(TofClusterR,1)       //TofClusterR
};


/// AntiClusterR structure

/*!
 \author e.choumilov@cern.ch

*/
class AntiClusterR {
static char _Info[255];
public:
  unsigned int   Status;   ///< Bit"128"->No FT-coinc. on 2 sides;"256"->1sideSector;"1024"->miss.side#
  int   Sector;   ///< //Sector number(1-8)
  int   Ntimes;  ///<Number of time-hits(1st come paired ones)
  int   Npairs;   ///<Numb.of time-hits, made of 2 side-times(paired)
  float Times[16];  ///<Time-hits(ns, wrt FT-time, "+" means younger hit)
  float Timese[16]; ///<Time-hits errors(ns)
  float Edep;    ///<Edep(mev)
  float Coo[3];   ///<R(cm),Phi(degr),Z(cm)-coordinates
  float ErrorCoo[3]; ///<Their errors

  AntiClusterR(){};
  AntiClusterR(AMSAntiCluster *ptr);
  /// \param number index in container
  /// \return human readable info about AnticlusterR
  char * Info(int number=-1){
    sprintf(_Info,"Anticluster No %d Sector=%d R=%5.2f#pm%5.2f #Phi=%5.2f#pm%5.2f Z=%5.2f#pm%5.2f E_{Dep}(MeV)=%7.3g",number,Sector,Coo[0],ErrorCoo[0],Coo[1],ErrorCoo[1],Coo[2],ErrorCoo[2],Edep);
  return _Info;
  } 
  virtual ~AntiClusterR(){};
ClassDef(AntiClusterR,1)       //AntiClusterR
};


/// TrRawClusterR structure

/*!
 \author v.choutko@cern.ch

*/
class TrRawClusterR {
public:
  int address;   ///<Address \sa TrClusterR Idsoft
  int nelem;     ///< number of strips 
  float s2n;     ///< signal/noise for seed strip

  TrRawClusterR(){};
  TrRawClusterR(AMSTrRawCluster *ptr);
  virtual ~TrRawClusterR(){};
ClassDef(TrRawClusterR,1)       //TrRawClusterR
};


/// TrClusterR structure  
/*!
 \author v.choutko@cern.ch

*/
class TrClusterR {
public:
  int Idsoft;  ///< software id
               /*!<
                                                   \n mod(id,10) layer 
                                                   \n mod(id/10,100) ladder
                                                   \n i=mod(id/1000,10)
                                                   \n i==0 x 1st half
                                                   \n i==1 x 2nd half
                                                   \n i==2 y 1st half
                                                   \n i==3 y 2nd half
                                                   \n id/10000 strip
            */
  unsigned int Status;  ///< statusword 
               /*!<
 1 - REFITTED object                                     (status&1     !=0) \n
 2 - WIDE in shape (Tracker)                             (status&2     !=0) \n
 3 - AMBIGously associated                               (status&4     !=0) \n
 4 - RELEASED object                                     (status&8     !=0) \n
 5 - BAD                                                 (status&16    !=0) \n
 6 - USED as a component of a larger object              (status&32    !=0) \n
 7 - DELETED object                                      (status&64    !=0) \n
 8 - BADTimeHistory (TOF)                                (status&128   !=0) \n
 9 - ONESIDE measurement (TOF)                           (status&256   !=0) \n
10 - BADTIME information (TOF), not used now             (status&512   !=0) \n
11 - NEAR, close to another object (Trck)                (status&1024  !=0) \n
12 - WEAK, defined with looser criteria (Trck)           (status&2046  !=0) \n
13 - AwayTOF, away from TOF predictions (Trck)           (status&4096  !=0) \n
14 - FalseX, x-coordinate built but not measured (Trck)  (status&8192  !=0) \n
15 - FalseTOFX, x-coordinates from TOF (Trck)            (status&16384 !=0) \n
16 - 4th tof plane was recovered using tracker           (status&32768 !=0) \n
17 - LocalDB was used to align track                     (status&65536 !=0) \n
18 - GlobalDB was used to align the track                (status&(65536*2)!=0) \n
19 - Cluster was used to get the charge                  (status&(65536*4)!=0) \n
20 - TrRecHit was good enough to be used in track find   (status&(65536*8)!=0) \n
21 - Track->Trladder interpol was done on plane level    (status&(65536*16)!=0) \n
22 - Track was created using TOF only                    (status&(65536*32)!=0) \n
23 - Object Overflow                                     (status&(65536*64)!=0) \n
26 - CATLEAK  (Ecal only)                               (status&(65536*256)!=0) \n 
                 */ 
  int NelemL;   ///< -Number of strips left to max
  int NelemR;   ///< Number of strips right to max
  float Sum;    ///< Amplitude total
  float Sigma;  ///< Sigma total
  float Mean;   ///< CofG (local)
  float RMS;    ///< RMS cluster
  float ErrorMean; ///< error in CofG
  vector<float> Amplitude;   ///< strips amplitudes (NelemR-NelemL)

  TrClusterR(){};
  TrClusterR(AMSTrCluster *ptr);
  virtual ~TrClusterR(){};
ClassDef(TrClusterR,1)       //TrClusterR
};



/// TrRecHitR structure   (3D- tracker clusters)
/*!
 \author v.choutko@cern.ch

*/

class TrRecHitR {
static char _Info[255];
public:
  unsigned int   Status;   ///< statusword \sa TrClusterR Status
  int   Layer;             ///<Layer no 1-6 up-down
  float Hit[3];            ///< cluster coordinates
  float EHit[3];           ///< error to above
  float Sum;               ///< Amplitude (x+y)
  float DifoSum;           ///< (A_x-A_y)/(A_x+A_y)
  float CofgX;             ///< local cfg x
  float CofgY;             ///< local cfg y
  float Bfield[3];         ///< magnetic field vector (kG)

protected:
  int  fTrClusterX;   ///<x proj TrClusterR pointer
  int  fTrClusterY;   ///<y proj TrClusteR pointer
public:
  /// access function to TrClusterR object used
  /// \param  xy = 'x' for x projection, any other for y projection
  /// \return index of TrClusterR object in TrClusterR collection or -1
  int iTrCluster(char xy)const {return xy=='x'?fTrClusterX:fTrClusterY;}
  /// access function to TrClusterR object used
  /// \param  xy = 'x' for x projection, any other for y projection
  /// \return pointer to TrClusterR TrClusterR object in TrClusterR collection or 0
  TrClusterR * pTrCluster(char xy);
  /// \param number index in container
  /// \return human readable info about TrRecHitR
  char * Info(int number=-1){
   sprintf(_Info,"TrRecHit no %d Layer %d Ampl=%4.1f, at (%5.1f,%5.1f,%5.1f)#pm(%5.3f,%5.3f,%5.3f, asym=%4.1f, status=%x)",number,Layer,Sum,Hit[0],Hit[1],Hit[2],EHit[0],EHit[1],EHit[2],DifoSum,Status);
     return _Info;
}

  TrRecHitR(){};
  TrRecHitR(AMSTrRecHit *ptr);
  friend class AMSTrRecHit;
  friend class AMSEventR;
  virtual ~TrRecHitR(){};
ClassDef(TrRecHitR,2)       //TrRecHitR
};



/// TrTrackR structure 
/*!
 \author j.alcaraz@cern.ch v.choutko@cern.ch

*/

class TrTrackR {
static char _Info[255];
public:
  unsigned int Status;   ///< statusword \sa TrClusterR
  int Pattern;  ///< see datacards.doc
  int Address;  ///< ladders combination code (see trrec.C buildaddress)
  float LocDBAver;   ///< mean ratio of true momentum to measured one from testbeam
  int   GeaneFitDone; ///<  != 0 if done
  int   AdvancedFitDone;  ///< != 0 if done
  float Chi2StrLine;  ///< chi2 sz fit
  float Chi2WithoutMS;  ///< chi2 circular fit OR pathint chi2  without ms
  float RigidityWithoutMS;  ///< circular rigidity OR pathint rigidity without ms
  float Chi2FastFit;    ///< chi2 fast nonl fit
  float Rigidity;  ///< fast nonl rigidity
  float ErrRigidity;  ///<err to 1/above
  float Theta;      ///< theta (from fast)
  float Phi;        ///< phi (from fast) 
  float P0[3];      ///< coo (from fast)
  float GChi2;     ///< geane chi2
  float GRigidity;  ///< geane rigidity
  float GErrRigidity; ///< geane err(1/rigidity)
  float HChi2[2];     ///< two halves chi2s
  float HRigidity[2];  ///< two halves rigidities
  float HErrRigidity[2];  ///< two halves err(1/rig)
  float HTheta[2];   ///< two halves theta
  float HPhi[2];     ///< two halves phi
  float HP0[2][3];   ///< two halves coo
  float FChi2MS;    ///< fast chi2 mscat off
  float PiErrRig;    ///< PathInt err(1/rig) (<0 means fit was not succesful)
  float RigidityMS;  ///< fast rigidity mscat off
  float PiRigidity;  ///<  PathInt rigidity
  TrTrackR(AMSTrTrack *ptr);
  protected:
  vector<int> fTrRecHit;
  public:
  TrTrackR(){};
 /// select good tracks (i.e. tracks having x & y hits from tracker)
  /// \return true if good false otherwise
   bool IsGood()const {return !(Status & 16384) && AdvancedFitDone!=0;}
  /// access function to TrRecHitR objects used
  /// \return number of TrRecHitR used
  int NTrRecHit()const {return fTrRecHit.size();}
  /// access function to TrRecHitR objects used
  /// \param i index of fTrRecHit vector
  /// \return index of TrRecHitR object in collection or -1
  int iTrRecHit(unsigned int i){return i<fTrRecHit.size()?fTrRecHit[i]:-1;}
  /// access function to TrRecHitR objects   
  /// \param i index of fTrRecHit vector
  /// \return pointer to TrRecHitR object  or 0
  TrRecHitR * pTrRecHit(unsigned int i);
  /// \param number index in container
  /// \return human readable info about TrTrackR
  char * Info(int number=-1){
    sprintf(_Info,"TrTrack No %d RigFast=%7.3g#pm%6.2g RigPath=%7.3g #theta=%4.2f #phi=%4.2f #chi^{2}=%7.3g Points=%d Patttern=%d HalfRig=(%7.3g,%7.3g) Status=%o",number,Rigidity,ErrRigidity*Rigidity*Rigidity,PiRigidity,Theta,Phi,Chi2FastFit,NTrRecHit(),Pattern,HRigidity[0],HRigidity[1],Status);
  return _Info;
  } 
  virtual ~TrTrackR(){};
ClassDef(TrTrackR,5)       //TrTrackR
friend class AMSTrTrack;
friend class AMSEventR;
};

//!  Rich Hits Structure

/*!
 \author Carlos.Jose.Delgado.Mendez@cern.ch
*/

class RichHitR {
static char _Info[255];
public:
   unsigned int Status;   ///< statusword
                         /*!<

*********) Status bits (counting from 1 to 32)

1- Hit used in the ring number 1
2- Hit used in the ring number 2
3- Hit used in the ring number 3
.
.
.
10- Hit used in the ring number 10 (no more than 10 rings currently flagged)
11- Unused
.
.
.
29- Unused
30- Gain mode chosen for the hit: 0=x1(low)  1=x5(high)
31- Hit belongs to a PMT apparently crossed by a charged particle

          
*/

  int   Channel;    ///<  channel number  
  int   Counts;     ///< ADC counts above the pedestal
  float Npe;        ///< ADC counts above the pedestal/gain of the channel 
  float Coo[3];         ///<  Hit coordinates

  RichHitR(){};
  RichHitR(AMSRichRawEvent *ptr, float x, float y, float z);
    /// \param number index in container
  /// \return human readable info about RichHitR
  char * Info(int number=-1){
    sprintf(_Info,"RichHit No %d Channel=%d Ampl=%d No_{PhotoEl}=%5.2f, Coo=(%5.2f,%5.2f,%5.2f) Status=%u Gain=%d HotSpot=%d ",number,Channel,Counts,Npe,Coo[0],Coo[1],Coo[2],Status%2048,(Status>>29)%2?5:1,(Status>>30));
  return _Info;
  } 

  virtual ~RichHitR(){};
ClassDef(RichHitR,2)       // RichHitR
};


//!  Rich Ring Structure

/*!
 \author Carlos.Jose.Delgado.Mendez@cern.ch
*/

class RichRingR {
static char _Info[255];
public:

 unsigned int Status;     ///< status word
                           /*!<

**********) Ring status bits (counting from 1 to 32)
1- Ring has been rebuild after cleaning PMTs apparently crossed by a charged particle.
   If the rebuilding has been succesful it is stored in the next ring in the rings container.
   However to confirm that next ring is indeed a rebuilt one, both tracks, the one from the
   current and the one from the next, should be the same. Otherwise the rebuilding was unsuccesful.    
	    
2- Ring reconstructed using the NaF radiator in the double radiator configuration

14 - Associated to a particle

31 - Associated to a track used in a gamma reconstruction.

*/

  int   Used;  ///< Nb. of RICH hits in a ring 
  int   UsedM; ///< Nb. of RICH mirrored hits in a ring 
  float Beta;  ///< Reconstructed velocity
  float ErrorBeta;  ///< Error in the velocity
  float Chi2;       ///< chi2/ndof for the beta fit
  float BetaRefit;  ///< Beta refitted
  float Prob;       ///< Kolmogorov test probability to be a good ring based on azimuthal distribution
  float KDist;      ///< Kullback-Leibler distance between the expected azimuthal distribution of hits and the observed one. The smaller the closer are both  
  float PhiSpread;  ///< Spread on the azimuthal emission angle of the hits associated to the reconstructed ring: (\sum_i (phi_i-\phi_0)^2)/N_{hits} 
  float UDist;      ///< (\sum_i 1/\dist_i^2) for unused hits which do not belong to PMTs crossed by a charged particle
  float NpExp;      ///< number of expected photoelectrons for Z=1 charge
  float NpCol;      ///< number of collected photoelectrons. The rich charge reconstruction is estimated as sqrt(NpCol/NPExp) 
  float Theta;      ///< Recontructed emission angle
  float ErrorTheta; ///< Error of the reconstructed emission angle
  float TrRadPos[3];///< Mean emission point of the Cerenkov photons
  float TrPMTPos[3];///< Intersection point of the track with the PMT plane
  int   lipHitsUsed;///< Nb. of used hits in LIP beta rec.
  float lipThetaC;  ///< Cherenkov angle reconstructed in LIP beta rec.
  float lipBeta;    ///< Beta from LIP beta rec.
  float lipErrorBeta;///< Error in beta from LIP beta rec. 
  float lipLikelihoodProb;///<Likelihood from LIP beta rec.
  float lipChi2;     ///< Chi2 from LIP beta rec.
  float lipRecProb;  ///< Probabbility from LIP beta rec. 


  protected:
  int fTrTrack;   ///< index of  TrTrackR  in collection
   vector<int> fRichHit; ///< indexes of RichHitR in collection
   void FillRichHits(int m);
  public:
  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();
  /// access function to RichHitR objects used
  /// \return index of RichHitR object in collection or -1
  int iRichHit(unsigned int i){return i<fRichHit.size()?fRichHit[i]:-1;}
   RichRingR(){};
  RichRingR(AMSRichRing *ptr);
  friend class AMSRichRing;
  friend class AMSEventR;
  /// \param number index in container
  /// \return human readable info about RichRingR
  char * Info(int number=-1){
    sprintf(_Info,"RichRing No %d Track=%d %s%s%s N_{Hits}=%d N_{MirrHits}=%d  #beta=%7.3g#pm%6.2g #chi^{2}=%7.3g #beta_{refit}=%7.3g#pm%6.2g Prob_{Kl.}=%7.3g Expected_{PhotoEl}=%5.2f Collected_{PhotoEl}=%5.2f",number,fTrTrack,Status&2?"NaF":"",Status&1?"Refit":"",Status&(16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2U)?"Gamma":"",Used,UsedM,Beta,ErrorBeta,Chi2,BetaRefit,ErrorBeta,Prob,NpExp,NpCol);
    return _Info;
  } 
  virtual ~RichRingR(){};
  ClassDef(RichRingR,10)           // RichRingR
}; 

/// TRDRawHitR structure
/*!
 \author v.choutko@cern.ch

*/

class TrdRawHitR {
public:
  int Layer;   ///< Layer 0(bottom)...19(top) 
  int Ladder;  ///<  Ladder  number
  int Tube;    ///< tube number
  float Amp;   ///< amplitude (adc counts)

  TrdRawHitR(){};
  TrdRawHitR(AMSTRDRawHit *ptr);

  virtual ~TrdRawHitR(){};
ClassDef(TrdRawHitR,1)       //TrdRawHitR
};

/// TRDClusterR structure
/*!
 \author v.choutko@cern.ch

*/

class TrdClusterR {
static char _Info[255];
public:
  unsigned int   Status;    ///< statusword
  float Coo[3];    ///<cluster coo (cm)
  int   Layer;     ///<Layer 0(bottom)...19(top) 
  int   Direction; ///<  0 == along x  1 == along y
  float ClSizeR;    ///< Tube Radius
  float ClSizeZ;    ///< Tube 1/2 Length
  int   Multip;    ///< multiplicity
  int   HMultip;   ///< multiplicity above threshold (5.9 kev)
  float EDep;      ///< energy dposition (kev)

  protected:
  int fTrdRawHit;   ///< pointer to trdrawhit with max ampl
  public:
  /// access function to TrdRawHitR object used
  /// \return index of TrdRawHitR object in collection or -1
  int iTrdRawHit()const {return fTrdRawHit;}
  /// access function to TrdRawHitR object used
  /// \return pointer to TrdRawHitR object or 0
  TrdRawHitR * pTrdRawHit();
  TrdClusterR(){};
  TrdClusterR(AMSTRDCluster *ptr);
  friend class AMSTRDCluster;
  friend class AMSEventR;
  /// \param number index in container
  /// \return human readable info about TrdClusterR
  char * Info(int number=-1){
   sprintf(_Info,"TRD Cluster No %d Layer %d TubeDir %s Coo %5.1f,%5.1f,%5.1f  Mult  %d HMult %d E_{Dep}(Kev) %5.1f ",number,Layer,(Direction==0?"x":"y"),Coo[0], Coo[1], Coo[2],Multip,HMultip,EDep);
   return _Info;
  }
  virtual ~TrdClusterR(){};
ClassDef(TrdClusterR,2)       //TrdClusterR
};

/// TRDSegmentR structure
/*!
 \author v.choutko@cern.ch

*/


class TrdSegmentR {
public:
  unsigned int   Status;            ///< status word
  int   Orientation;       ///< segment orientation (0-x 1-y)
  float FitPar[2];         ///< str line fit pars
  float Chi2;              ///< str line chi2
  int   Pattern;           ///< segment pattern no
 
  protected:
  vector<int> fTrdCluster;   // used TrdClusterR's indexes
  public:
  /// access function to TrdClusterR objects used
  /// \return number of TrdClusterR used
  int NTrdCluster()const {return fTrdCluster.size();}
  /// access function to TrdClusterR objects used
  /// \param i index of fTrdCluster vector
  /// \return index of TrdClusterR object in collection or -1
  int iTrdCluster(unsigned int i){return i<fTrdCluster.size()?fTrdCluster[i]:-1;}
  /// access function to TrdClusterR collection   
  /// \param i index of fTrdCluster vector
  /// \return pointer to TrdClusterR object or 0
  TrdClusterR * pTrdCluster(unsigned int i);
  friend class AMSTRDSegment;
  friend class AMSEventR;
  TrdSegmentR(){};
  TrdSegmentR(AMSTRDSegment *ptr);
  virtual ~TrdSegmentR(){};
ClassDef(TrdSegmentR,1)       //TrdSegmentR
};


/// TRDTrackR structure
/*!
 \author v.choutko@cern.ch

*/

class TrdTrackR {
static char _Info[255];
public:
  unsigned int   Status;  ///< statusword
  float Coo[3];  ///< trac coo (cm)
  float ErCoo[3];  ///< err to coo
  float Phi;     ///<  phi (rads)
  float Theta;   ///< Theta
  float Chi2;   ///<  Chi2
  int   Pattern;  ///< pattern no
  protected:
  vector<int> fTrdSegment;
  public:
  /// access function to TrdSegmentR objects used
  /// \return number of TrdSegmentR used
  int NTrdSegment()const {return fTrdSegment.size();}
  /// access function to TrdSegmentR objects used
  /// \param i index of fTrdSegment vector
  /// \return index of TrdSegmentR object in collection or -1
  int iTrdSegment(unsigned int i){return i<fTrdSegment.size()?fTrdSegment[i]:-1;}
  /// access function to TrdSegmentR collection   
  /// \param i index of fTrdSegment vector
  /// \return pointer to TrdSegmentR object or 0
  TrdSegmentR * pTrdSegment(unsigned int i);
  TrdTrackR(AMSTRDTrack *ptr);
  TrdTrackR(){};
  /// \param number index in container
  /// \return human readable info about TrdTrackR
  char * Info(int number=-1){
    int np=0;
    for(int i=0;i<NTrdSegment();i++)np+=pTrdSegment(i)->NTrdCluster();
 
    sprintf(_Info,"TrdTrack No %d Coo=(%5.2f,%5.2f,%5.2f)#pm((%5.2f,%5.2f,%5.2f) #theta=%4.2f #phi=%4.2f #chi^{2}=%7.3g N_{Hits}=%d",number,Coo[0],Coo[1],Coo[2],ErCoo[0],ErCoo[1],ErCoo[2],Theta,Phi,Chi2,np);
  return _Info;
  } 
  friend class AMSTRDTrack;
  friend class AMSEventR;
  virtual ~TrdTrackR(){};
ClassDef(TrdTrackR,1)       //TrdTrackR
};


/// Level1 trigger structure 
/*!
 \author e.choumilov@cern.ch

*/

class Level1R {
static char _Info[255];
 bool IsECHighMultipl()const {return EcalFlag/10>2;}
 bool IsECShowAngleOK()const {return EcalFlag%10==2;}
public:
  int   PhysBPatt;   ///< 8 lsbits-> pattern of LVL1 sub-triggers ("predefined physics" branches) 
                    /*!<        LVL1 is (masked) OR of above branches.		         
		           The list of predefined branches(preliminary):                                         \n                    \n
		        bit1: unbiased TOF-trig(i.e. FTC= z>=1)  \n
			bit2: Z>=1(FTC+anti)                     \n
			bit3: Z>=2(FTC & BZ)                     \n
			bit4: SlowZ>=2(FTZ)                      \n
			bit5: electrons(FTC & FTE & ecFand)      \n
			bit6: gammas(FTE & ecFand & ecAand)      \n
		        bit7: unbECAL(FTE)                       \n
			bit8: External                           \n
		    */
  int   JMembPatt; ///< 16 lsbits-> pattern of trig.members(FTC,FTE,CP,...) defined in single phys. branch 
  int   TofFlag1;   ///< FTC(z>=1) layers pattern code, <0->noFTC,>=0->(0-8)->miss.planes-code;
  int   TofFlag2;   ///< BZ(z>=2) layers pattern code, <0->noFTC,>=0->(0-8)->miss.planes-code;
  int   TofPatt1[4]; ///< 4-layers TOF paddles pattern for FTC(z>=1)(separately for each side) 
  int   TofPatt2[4]; ///< the same for BZ(z>=2)(separately for each side): 

                    /*!<
                                                       1-10 bits  Side-1  \n
                                                       17-26      Side-2  \n
                   */
  int   AntiPatt;   ///< Antipatt:(1-8)bits->sectors in coincidence with FastTrigger  
  int   EcalFlag;   ///< =MN, where 
                    /*!< 
                          M=0/1/2/3->Etot<MipThr / Etot>=MipThr / !=0_LayerMultipl/LayerMultiplOK=>EM  \n
			(this is valid for MC only, for RD 0/3->LowLayersMultipl/LayerMultiplOK=>EM)   \n
                          N=2/1/0->ShowerAngle(width)Test=OK/Bad/Unknown                              \n
                    */
  unsigned short int EcalPatt[6][3];///< EC DynodesPattern for 6 "trigger"-SupLayers(36dynodes use 36of(3x16)lsbits)
  float EcalTrSum; ///< EC-energy trig.sum(Gev, MC only)
  float LiveTime;  ///< Fraction of "nonBusy" time
  float TrigRates[6]; ///< TrigComponentsRates(Hz):FT,FTC,LVL1,TOFmx,ECFTmx,ANTImx                    

  Level1R(){};
  Level1R(Trigger2LVL1 *ptr);
  /// \param number index in container
  /// \return human readable info about Level1R
  char * Info(int number=-1){
    int antif=0;
    for(int k=0;k<8;k++){
     if(AntiPatt & (1<<(k))){
       antif++;
     }
    }
    
    sprintf(_Info,"TrLevel1: TofFlag %s, Z %s, AntiFired %d, ECMult  %s, ECShowAngle %s, EcalSum %5.1f GeV",TofFlag1%10==0?"4/4":"3/4",TofFlag2>0?">1":"=1",antif,IsECHighMultipl()?"High":"Low",IsECShowAngleOK()?"OK":"Bad",EcalTrSum);
  return _Info;
  }
  virtual ~Level1R(){};
ClassDef(Level1R,2)       //Level1R
};


/// Level3 trigger structure 
/*!
 \author v.choutko@cern.ch

*/

class Level3R {
static char _Info[512];
public:
  int   TOFTr;  ///< TOF Trigger
                /*!<
                                                      \n -1 if rejected by matrix trigger,
                                                      \n   0 Too Many Hits
                                                      \n   1  tof 0  1 cluster or 2 cluster
                                                      \n   2  tof 1  -----
                                                      \n   4  tof 2  -----
                                                      \n   8  tof 3  -----
                                                      \n   16  tof 0  2 cluster
                                                      \n   32  tof 1  -----
                                                      \n   64  tof 2  -----
                                                      \n   128  tof 3  -----
                           */
  int   TRDTr;   ///<  TRD Trigger
                   /*!<
                                                     \n   0  Nothing found
                                                     \n   bit 0:  Segment x found
                                                     \n   bit 1:  segment y found
                                                     \n   bit 2: too many hits found  
                                                     \n   bit 3: high gamma event found 
                   */

  int   TrackerTr;   ///< Tracker Trigger
                   /*!<
                            \n 0  - initial state
                            \n 1  - No Tr Tracks found
                            \n 2  - Too Many Hits in Tracker
                            \n 3  - Positive Rigidity found
                            \n 4  - Ambigious Comb (A) found 
                            \n 5  - Ambigious Comb (B) found 
                            \n 6  - Negative Rigidity(Momentum) found
                            \n + 8   // Heavy Ion (Tracker)
                   */
  int   MainTr;   ///< Main Trigger
                   /*!<
                            \n bit  0 No Tr Tracks found
                            \n bit  1 Too Many Hits in Tracker
                            \n bit  2 Too Many Hits in TRD
                            \n bit  3 Too Many Hits in TOF
                            \n bit  4 No TRD Tracks found
                            \n bit  5 Upgoing event found
                            \n bit  6 No TOF Time Information found
                            \n bit  7 Positive Rigidity(Momentum) found
                            \n bit  8 Ambigious Comb (A) found 
                            \n bit  9 Ambigious Comb (B) found 
                            \n bit  10  Negative Rigidity(Momentum) found
                            \n bit  11  High Gamma (TRD)  
                            \n bit  12   Heavy Ion (Tracker)
                            \n bit  13 Prescaled event
			    \n bit  14 No ECAL activity(>=MIP) found
			    \n bit  15 ECAL EM-object found(or Eectot>20gev)
			    \n bit  16 ECAL track found(line fit chi2x,chi2y ok)
			    \n bit  17 ECAL track match to TOF/TRD path  
                  */
  int   Direction;  ///< Particle Direction -1 Up 0 Uknown 1 Down
  int   NTrHits;    ///< Number Tracker Hits
  int   NPatFound;   ///< Number "Tracks" found
  int   Pattern[2];  ///<   Pattern no
  float Residual[2]; ///< Aver Residual (cm)
  float Time;       ///< Algorithm Time (sec)
  float ELoss;      ///< Tracker Aver energy loss
  int   TRDHits;    ///< trd hits no
  int   HMult;      ///< trd high energy hits (>6 kev) no
  float TRDPar[2];  ///< trd track tan x,y if any
  int   ECemag;   ///< ECelectromagn(-1/0/1=nonEM/noECactivity)/EM)
  int   ECmatc;   ///< EC-TOF/TRD match(-1/0/1=noMatch/noECtrack/Match)
  float ECTOFcr[4];  ///<Xcr/Ycr/TangX/TangY of ECtrack with bottom  plane of Tracker(when ECtrack was found)

  Level3R(){};
  Level3R(TriggerLVL302 *ptr);
  /// \param number index in container
  /// \return human readable info about Level3R
  char * Info(int number=-1){
    char infol[500];
    strcpy(infol,"");
    for(int k=0;k<18;k++){
     if( MainTr & (1<<k)){
      switch (k) {
       case 0:
        strcat(infol,"NoTrTracks ");
        break;
       case 1:
        strcat(infol,"TManyTrHits ");
        break;
       case 2:
        strcat(infol,"TManyTrdHits ");
        break;
       case 3:
        strcat(infol,"TManyTofHits ");
        break;
       case 4:
        strcat(infol,"NoTrdTracks ");
        break;
       case 5:
        strcat(infol,"Upgoing ");
        break;
       case 6:
        strcat(infol,"NoTofTime ");
        break;
       case 7:
        strcat(infol,"Rig+ ");
        break;
       case 8:
        strcat(infol,"AmbA ");
        break;
       case 9:
        strcat(infol,"AmbB ");
        break;
       case 10:
        strcat(infol,"Rig- ");
        break;
       case 11:
        strcat(infol,"High#gamma ");
        break;
       case 12:
        strcat(infol,"HeavyIon ");
        break;
       case 13:
        strcat(infol,"Prescaled ");
        break;
       case 14:
        strcat(infol,"NoEcal ");
        break;
       case 15:
        strcat(infol,"EcalEmag ");
        break;
       case 16:
        strcat(infol,"EcalTrack ");
        break;
       case 17:
        strcat(infol,"EcalTrackMatch ");
        break;
      }
     }
    }
    sprintf(_Info,"TrLevel3: %s",infol);
  return _Info;
  }
  virtual ~Level3R(){};
ClassDef(Level3R,1)       //Level3R
};






/// Tof Beta (Velocity/c) structure
/*!
 \author v.choutko@cern.ch

*/

class BetaR {
 public:
  unsigned int       Status;   ///< status word  4 - ambig
  int       Pattern;   ///< beta pattern \sa beta.doc
  float     Beta;     ///< velocity
  float     BetaC;    ///< corrected velocity (taking into account true velocity < 1)
  float     Error;   ///< error (1/beta)
  float     ErrorC;  ///< error (1/betac)
  float     Chi2;    ///<chi2 of beta fit(time)
  float     Chi2S;   ///<chi2 of beta fit(space)
protected:
  int   fTrTrack;    ///< index to TrTrack used
  vector<int> fTofCluster; ///< indexes of TofClusterR's used
public:
  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();
  /// access function to TofClusterR objects used
  /// \return number of TofClusterR used
  int NTofCluster()const {return fTofCluster.size();}
  /// access function to TofClusterR objects used
  /// \param i index of fTofCluster vector < NTofCluster()
  /// \return index of TofClusterR object in collection or -1
  int iTofCluster(unsigned int i){return i<fTofCluster.size()?fTofCluster[i]:-1;}
  /// access function to TofClusterR objects   
  /// \param i index of fTofCluster vector  < NTofCluster()
  /// \return pointer to TofClusterR object  or 0
  TofClusterR * pTofCluster(unsigned int i);
   BetaR(){};
   BetaR(AMSBeta *ptr);
   friend class AMSBeta;
   friend class AMSEventR;
   virtual ~BetaR(){};
   ClassDef(BetaR,1)         //BetaR
};



/// Voted Charge Structure
/*!
    Particle Charge Determination by Trd, Tof, Tr and Rich detectors \n
    Still Incomplete \n
    Only Tr, Tof aand Rich re used at the moment \n
*/    
class ChargeR {
/*!
  \author Carlos.Jose.Delgado.Mendez@cern.ch

*/

public:
  unsigned int Status;     ///<  Status (1-refitted)
  int ChargeTOF;  ///<TOF Charge
  int ChargeTracker;  ///< Tracker Charge
  int ChargeRich;      ///<Rich Charge 
  float ProbTOF[4];    ///< TOF highest Probs
  int ChInTOF[4];      ///< charge indices for highest Probs (see charge.doc)
  float ProbTracker[4]; ///< Tracker highest Probs
  int ChInTracker[4];     ///< charge indices for highest Probs (see charge.doc)
  float ProbRich[4];    ///< rich highest Probs
  int ChInRich[4];          ///< charge indices for highest Probs (see charge.doc)
  float ProbAllTracker;  ///< Tracker highest Prob (all hits)
  float TrunTOF;          ///< Truncated (-1) mean   (Tof Anodes)     
  float TrunTOFD;         ///< Truncated (-1) mean  (Tof Dynodes)     
  float TrunTracker;     ///< Truncated (-1) mean   Tracker  
protected:
  int  fBeta;        ///< index of BetaR used
  int  fRichRing;        ///< index of RichRingR used 
public:
  /// access function to BetaR object used
  /// \return index of BetaR object in collection or -1
  int iBeta()const {return fBeta;}
  /// access function to BetaR object used
  /// \return pointer to BetaR object or 0
  BetaR * pBeta();
  /// access function to RichRingR object used
  /// \return index of RichRingR object in collection or -1
  int iRichRing()const {return fRichRing;}
  /// access function to RichRingR object used
  /// \return pointer to RichRingR object or 0
  RichRingR * pRichRing();
   friend class AMSEventR;
   friend class AMSCharge;
  ChargeR(){};
  ChargeR(AMSCharge *ptr, float probtof[],int chintof[],
               float probtr[], int chintr[], 
               float probrc[], int chinrc[], float proballtr);
  virtual ~ChargeR(){};
ClassDef(ChargeR,1)       //ChargeR

};

/*!
  \class VertexR
  \brief Vertex class

   \sa vtx.h vtx.C
   \author juan.alcaraz@cern.ch
*/
class VertexR {
public:
  unsigned int   Status;      ///< Status
  float Mass;        ///< Rec mass at vertex (GeV)
  float Momentum;    ///< Momentum sum (GeV)
  float ErrMomentum; ///< Error in 1 / Momentum Sum (1/GeV)
  float Theta;       ///< Theta (rad)
  float Phi;         ///< Phi  (rad)
  float Vertex[3];   ///< reconstructed vertex (cm)
  int   Charge;      ///< Charge at vertex
  float Chi2;        ///< Chi2
  int   Ndof;        ///< Number of degrees of freedom
protected:
  vector<int> fTrTrack; ///< indexes of associated tracks
public:
  /// access function to TrTrackR objects used
  /// \return number of TrTrackR used
  int NTrTrack()const {return fTrTrack.size();}
  /// access function to TrTrackR objects used
  /// \param i index of fTrTrackR vector
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack(unsigned int i)const {return i<fTrTrack.size()? fTrTrack[i]:-1;}
  /// access function to TrTrackR objects   
  /// \param i index of fTrTrackR vector
  /// \return pointer to TrTrackR object  or 0
  TrTrackR * pTrTrack(unsigned int i);
  VertexR(){};
  VertexR(AMSVtx *ptr);
  friend class AMSVtx;
  friend class AMSEventR;
  virtual ~VertexR(){};
  ClassDef(VertexR,3)         //VertexR
};




/// AMS Particle structure
/*!
  AMS Particle types:
     - "Normal" Particle:
      -# Derived from ChargeR, BetaR and TrTrackR  objects
      -# Has   Charge, Rigidity, Velocity and DirCos properties set up
      -# Has fBeta,fCharge,fTrTrack set up
      -# Optionally has fTrdTrack set up in case TrdTrackR was found
      -# Optionally has fEcalShower set up in case EcalShowerR was found
      -# Optionally has fRichRing sett up in case Rich was used in velocity determination
     - Particle without TrTrackR:
      -# Derived from ChargeR, BetaR and optionally TrdTrack  objects
      -# Has rigidity set up to 100000000 gev 
      -# Has fBeta,fCharge set up
      -# fTrTrack set to -1
      -# Optionally has fTrdTrack set up in case TrdTrackR was found
      -# Optionally has fRichRing setted up in case Rich was used in velocity determination
      -# Optionally has fEcalShower set up in case EcalShowerR was found
     - Particle based on EcalShower object:
      -# Derived from EcalShowerR (Momentum,DirCos);
      -# fBeta, fCharge, fTrTrack, fTrdTrack and fRichRing set to -1
      -# Velocity set to +-1 depend on shower ditection
      -# Two particles are in fact created with charg set to +-1
     - Particle based on VertexR (i.e. converted photon candidate or electron/positron ):
      -# fTrTrack set to -1
      -# fVertex set up
      -# Charge set to 0 or +-1 
      -# Velocity may or may not be set depending on fBeta index

\author v.choutko@cern.ch
      
*/     
         
   
   
class ParticleR {
private:
static char _Info[255];
public:
  unsigned int Status;   ///< status word (not really used at the moment)
  int   Particle;     ///< voted particle id a-la geant3 (not really reliable)
  int   ParticleVice; ///< nect-to voted particle id a-la geant3 (not really reliable)
  float Prob[2];      ///< Probability to be a particle Particle [0] or ParticleVice [1]
  float FitMom;       ///< Fitted Momentum to particle Particle
  float Mass;         ///< Mass (Gev/c2)
  float ErrMass;      ///< Error to Mass 
  float Momentum;     ///< Momentum (Gev/c) (signed), means particle with negative charge will have negative momentum
                      /*!<
pmom definition  for fortran gurus

if( pbetap(i) > 0)then
  
  if(ptrackp(i)>0)then \n
     pmom(i)=ridgidity(ptrackp(i))*pcharge(i)*sign(beta(pbetap(i))) \n
  else \n
     pmom(i)=1000000*sign(beta(pbetap(i))) \n
  endif \n
else if(pecalp(i)>0)then \n
     pmom(i)=ecshen(pecalp(i)) \n
endif
*/
  float ErrMomentum;  ///< Error in momentum
  float Beta;         ///< Velocity (Tof + Rich if available)
  float ErrBeta;      ///< Error to Velocity
  float Charge;       ///< Abs(particle charge)
  float Theta;        ///< theta (1st[last] tracker plane) rad
  float Phi;          ///< phi (1st[last] tracker plane)   rad
  float ThetaGl;       ///< Theta in GTOD coo system (rad)
  float PhiGl;          ///< Phi in GTOD coo system (rad)
  float Coo[3];        ///< coo (1st[last] tracker plane) cm
  float Cutoff;       ///<  geomag cutoff in GeV/c, dipole model
  float TOFCoo[4][3]; ///< track extrapol in tof planes
  float AntiCoo[2][3]; ///< track extrapol in anti
  float EcalCoo[3][3]; ///< track extrapol in ecal (enter, cofg, exit)
  float TrCoo[8][3];  ///< track extrapol in tracker planes
  float TRDCoo[2][3];    ///< track extrapol in trd (center,top)
  float RichCoo[2][3];  ///< track extrapol in rich (radiator_pos, pmt_pos)
  float RichPath[2];    ///<  Estimated fraction  of ring photons  within RICH acceptance (direct and reflected ones  respectively) for beta=1
  float RichPathBeta[2]; ///<  Estimated fraction  of ring photons  within RICH acceptance (direct and reflected ones  respectively) for beta Beta
  float RichLength; ///< Estimated pathlength of particle within rich radiator (cm)
  int   RichParticles; ///< Estimated number of particles crossing the RICH radiator
  float Local[8];  ///< contains the minimal distance to sensor edge in sensor length units ;
  float TRDLikelihood; ///< TRD likelihood  (whatever is it)
protected: 
  int  fBeta;          ///<index of  BetaR used
  int  fCharge;        ///<index of  ChargeR used
  int  fTrTrack;      ///<index of  TrTrackR used
  int  fTrdTrack;     ///<index of  TrdTrackR used
  int  fRichRing;     ///<index of  RichringR used
  int  fEcalShower;   ///<index of  EcalShowerR used
  int  fVertex;       ///<index of  VertexR used
public:
  /// access function to BetaR object used
  /// \return index of BetaR object in collection or -1
  int iBeta()const {return fBeta;}
  /// access function to BetaR object used
  /// \return pointer to BetaR object or 0
  BetaR * pBeta();

  /// access function to ChargeR object used
  /// \return index of ChargeR object in collection or -1
  int iCharge()const {return fCharge;}
  /// access function to ChargeR object used
  /// \return pointer to ChargeR object or 0
  ChargeR * pCharge();

  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();

  /// access function to TrdTrackR object used
  /// \return index of TrdTrackR object in collection or -1
  int iTrdTrack()const {return fTrdTrack;}
  /// access function to TrdTrackR object used
  /// \return pointer to TrdTrackR object or 0
  TrdTrackR * pTrdTrack();

  /// access function to RichRingR object used
  /// \return index of RichRingR object in collection or -1
  int iRichRing()const {return fRichRing;}
  /// access function to RichRingR object used
  /// \return pointer to RichRingR object or 0
  RichRingR * pRichRing();

  /// access function to EcalShowerR object used
  /// \return index of EcalShowerR object in collection or -1
  int iEcalShower()const {return fEcalShower;}
  /// access function to EcalShowerR object used
  /// \return pointer to EcalShowerR object or 0
  EcalShowerR * pEcalShower();

  /// access function to VertexR object used
  /// \return index of VertexR object in collection or -1
  int iVertex()const {return fVertex;}
  /// access function to VertexR object used
  /// \return pointer to VertexR object or 0
      VertexR * pVertex();

  char * pType(){
   static char type[63];
   if(iTrTrack()>=0){
     strcpy(type,"Tr");
   }
   else strcpy(type,"");
      if(iBeta()>=0)strcat(type,"Tof");
      if(iTrdTrack()>=0)strcat(type,"Trd");
      if(iRichRing()>=0)strcat(type,"Rich");
      if(iEcalShower()>=0)strcat(type,"Ecal");
      if(iVertex()>=0)strcat(type,"Vertex");
   return type;
  }

  /// \param number index in container
  /// \return human readable info about ParticleR
  char * Info(int number=-1){
   sprintf(_Info," Particle %s No %d Id=%d p=%7.3g#pm%6.2g M=%7.3g#pm%6.2g #theta=%4.2f #phi=%4.2f Q=%2.0f  #beta=%6.3f#pm%6.3f Coo=(%5.2f,%5.2f,%5.2f)",pType(),number,Particle,Momentum,ErrMomentum,Mass,ErrMass,Theta,Phi,Charge,Beta,ErrBeta,Coo[0],Coo[1],Coo[2]);
   return _Info;
  }
  ParticleR(){};
  ParticleR(AMSParticle *ptr, float phi, float phigl);
  friend class AMSParticle;
  friend class AMSEventR;
  virtual ~ParticleR(){};
  ClassDef(ParticleR,3)       //ParticleR
};

/// AntiMCCluster structure
/*!

   \author e.choumilov@cern.ch
*/
class AntiMCClusterR {
public:
  int   Idsoft;   ///< software id  (ask E.Choumilov for details)
  float Coo[3];   ///< coo (cm)
  float TOF;      ///< time of flight (sec)
  float Edep;     ///< energy dep (GeV)

  AntiMCClusterR(){};
  AntiMCClusterR(AMSAntiMCCluster *ptr);
  virtual ~AntiMCClusterR(){};
ClassDef(AntiMCClusterR,1)       //AntiMCClusterR
};

/// TrMCCluster structure
/*!

   \author v.choutko@cern.ch
*/
class TrMCClusterR {
static char _Info[255];
public:
  int Idsoft;   ///< idsoft
               /*!<
                  Idsoft%10 layer
                  Idsoft/10)%100 ladder
                  Ifsoft/1000    sensor
               */
  int TrackNo;   ///< geant3 particle id or 555 if noise
  int Left[2];   ///< left strip no
  int Center[2];   ///< center strip no
  int Right[2]; ///< right strip no
  float SS[2][5];  ///< Strip amplitudes  (x,y)
  float Xca[3];    ///< local enter coo  cm
  float Xcb[3];    ///< local exit coo  cm
  float Xgl[3];    ///< global coo cm,center
  float Sum;       ///< total amplitude gev

  TrMCClusterR(){};
  TrMCClusterR(AMSTrMCCluster *ptr);
  /// \param number index in container
  /// \return human readable info about TrMCClusterR
  char * Info(int number=-1){
    sprintf(_Info,"TrMCCluster No %d PId=%d Coo=(%5.2f,%5.2f,%5.2f), Ampl(Mev)=%5.2f",number,TrackNo,Xgl[0],Xgl[1],Xgl[2],Sum*1000);
  return _Info;
  } 
  virtual ~TrMCClusterR(){};
ClassDef(TrMCClusterR,1)       //TrMCClusterR
};


/// TofMCCluster structure
/*!

   \author e.choumilov@cern.ch
*/
class TofMCClusterR {
public:
  int   Idsoft;  ///< software id  (ask E.Choumilov for details)
  float Coo[3];  ///< coo cm
  float TOF;     ///<  time of flight sec
  float Edep;    ///< energy dep mev
  TofMCClusterR(){};
  TofMCClusterR(AMSTOFMCCluster *ptr);
  virtual ~TofMCClusterR(){};
ClassDef(TofMCClusterR,1)       //TOFMCClusterRoot
};


/// TrdMCCluster structure
/*!

   \author k.scholberg@cern.ch
*/
class TrdMCClusterR {
public:
  int   Layer;             ///< layer 0(top) 19 (bottom) no
  int   Ladder;     ///< ladder  no
  int   Tube;   ///< tube no
  int   ParticleNo;   ///< particle id ala g3, (-) if secondary partilces
  float Edep;        ///<  energy dep gev
  float Ekin;        ///< part kin energy (gev)
  float Xgl[3];     ///< hit global coo(cm)
  float Step;        ///< step size (cm)
 
  TrdMCClusterR(){};
  TrdMCClusterR(AMSTRDMCCluster *ptr);
  virtual ~TrdMCClusterR(){};
ClassDef(TrdMCClusterR,1)       //TrdMCClusterR
};


/// RichMCCluster structure
/*!

   \author Carlos.Jose.Delgado.Mendez@cern.ch
*/
class RichMCClusterR {
public:
  int   Id;            ///< Particle id, -666 if noise
  float Origin[3];     ///< Particle origin coo
  float Direction[3];  ///< Particle direction coo
  int   Status;        ///< *******) 

                       /*!<
*******) For geant4 this value is 0. For geant 3 it has several meanings:

Cerenkov photon generated in radiator:

    ricstatus = 100*(mother of Cerenkov if secondary?1:0)+10*(number of
                reflections in mirror) + (photon suffered rayleigh
                scattering?1:0)

PMT noise:

    ricstatus = -1

Cerenkov photon generated in PMT window:

    ricstatus = -(2+100*(mother of Cerenkov if secondary?1:0))


Cerenkov photon generated in light guide:
   
    ricstatus = -(5+100*(mother of Cerenkov if secondary?1:0)) 


No Cerenkov photon:

    ricstatus = -(3+100*(mother of Cerenkov if secondary?1:0))


Downward charged particle crossing the radiator

    ricstatus = -7

NOTE: The information of the mother is only available if RICCONT=1 in
      the datacards
*/

  int   NumGen;        ///< Number of generated photons
  int   fRichHit;      ///< Pointer to detected hit
  RichMCClusterR(){};
  RichMCClusterR(AMSRichMCHit *ptr, int _numgen);
  virtual ~RichMCClusterR(){};
  ClassDef(RichMCClusterR,2)       // RichMCClusterR
};



/// MCTrack structure

/*! 
Contains radiation/absorption length ticknesses. 
 Activated bt SCAN TRUE datacard
\author v.choutko@cern.ch
*/

class MCTrackR {
public:
float RadL;   ///< integrated radiation length 
float AbsL;   ///< integrated nuclear absorption length 
float Pos[3];  ///< x,y,z (cm) 
char  VolName[5];  ///< Volume name

 MCTrackR(){};
 MCTrackR(AMSmctrack *ptr);
  virtual ~MCTrackR(){};
ClassDef(MCTrackR,1)       //MCTrackR
};

/// MC particle structure
/*!

   \author v.choutko@cern.ch
*/
class MCEventgR {
static char _Info[255];
public:
  int Nskip;      ///<  reserved
  int Particle;   ///< geant3 particle id
                  /*!<
          (geant3 only) \n
            Particle=g3pid+256 means heavy ion nonelstic
        scattering occured in for pid with dir & momentum at coo \n
        particle=-g3pid means secondary particle produced with
        dir&momentum at coo
*/
  float Coo[3];   ///< coo (cm)
  float Dir[3];   ///< dir cos
  float Momentum;  ///< momentum (gev)
  float Mass;      ///< mass (gev)
  float Charge;    ///< charge (signed)
  MCEventgR(){};
  MCEventgR(AMSmceventg *ptr);
  /// \param number index in container
  /// \return human readable info about McEventgR
  char * Info(int number=-1){
    sprintf(_Info,"McParticle No %d Pid=%d Coo=(%5.2f,%5.2f,%5.2f) #theta=%4.2f #phi=%4.2f Momentum(Gev)=%7.2g Mass=%7.2g Q=%4.0f",number,Particle,Coo[0],Coo[1],Coo[2],acos(Dir[2]),atan2(Dir[1],Dir[0]),Momentum,Mass,Charge);
  return _Info;
  } 
  virtual ~MCEventgR(){};
ClassDef(MCEventgR,1)       //MCEventgR
};



///   AMS Event Root Class 
/*!   Contains instance of HeaderR class and containers
    (stl vectors) of all other AMS related classes.
    Access to most HeaderR elements are public, while
     access to any vectors are via access function only. \n
     For any class XYZR  5 (five) functions are provided: \n
      - int nXYZ() - returns number of XYZ objects (via HeaderR counters, fast).  \n
      - int NXYZ() -  same as nXYZ but do actual read of vectors from file.
                  Not really recommended. Use nXYZ instead.\n
      - vector<XYZR> & XYZ() - returns reference to container. For advanced users only. \n
      - XYZ & XYZ(unsigned int i) - returns reference on ith element of vector of class XYZR. Recommended way of access. \n
      - XYZ * pXYZ(unsigned int i) - same as XYZ but returns pointer instead of reference. For advanced users only.  \n

    For some classes having references to other XYZR classes the following functions are (optionally) provided
      - int iXYZ(int i=0) const  returns index of i-th used XYZR element in
        corresponding  collection or -1. This element may be later accessed by functions described above.
      - XYZ* pXYZ(int i=0) return pointer to i-th used XYZR element or 0.
        For advanced users only.
      -  (optionally)  int NXYZ() const returns number of XYZR objects used

    Contains set of routines for (old fashioned) hbook like histograms manipulation: \n
- hbook(1,2,p)
- hlist   
- hfit1
- hftetch 
- hreset 
- hdelete 
- hfill 
- hf(1,2,p)
- hcopy
- hdivide
- hsub
- hscale 

    \sa ad.C stlv
    \author v.choutko@cern.ch
*/
#include <map>
class AMSEventR: public  TSelector {   
protected:
class Service{
public:
 TFile *            _pOut;
typedef map<AMSID,TH1F*> hb1_d;
typedef map<AMSID,TH1F*>::iterator hb1i;
typedef map<AMSID,TH2F*> hb2_d;
typedef map<AMSID,TH2F*>::iterator hb2i;
typedef map<AMSID,TProfile*> hbp_d;
typedef map<AMSID,TProfile*>::iterator hbpi;
static  hb1_d hb1;
static  hb2_d hb2;
static  hbp_d hbp;
static  char Dir[1024];
 TStopwatch         _w;
 unsigned int       TotalEv;
 unsigned int       BadEv;
 unsigned int       TotalTrig;
Service():_pOut(0),TotalEv(0),BadEv(0),TotalTrig(0){}
~Service(){
}
};
protected:
Service  fService;
static TBranch*  bHeader;
static TBranch*  bEcalHit;
static TBranch*  bEcalCluster;
static TBranch*  bEcal2DCluster;
static TBranch*  bEcalShower;
static TBranch*  bRichHit;
static TBranch*  bRichRing;
static TBranch*  bTofRawCluster;
static TBranch*  bTofRawSide;
static TBranch*  bTofCluster;
static TBranch*  bAntiCluster;
static TBranch*  bTrRawCluster;
static TBranch*  bTrCluster;
static TBranch*  bTrRecHit;
static TBranch*  bTrTrack;
static TBranch*  bTrdRawHit;
static TBranch*  bTrdCluster;
static TBranch*  bTrdSegment;
static TBranch*  bTrdTrack;
static TBranch*  bLevel1;
static TBranch*  bLevel3;
static TBranch*  bBeta;
static TBranch*  bVertex;
static TBranch*  bCharge;
static TBranch*  bParticle;
static TBranch*  bAntiMCCluster;
static TBranch*  bTrMCCluster;
static TBranch*  bTofMCCluster;
static TBranch*  bTrdMCCluster;
static TBranch*  bRichMCCluster;
static TBranch*  bMCTrack;
static TBranch*  bMCEventg;
static TBranch*  bAux;


static void*  vHeader;
static void*  vEcalHit;
static void*  vEcalCluster;
static void*  vEcal2DCluster;
static void*  vEcalShower;
static void*  vRichHit;
static void*  vRichRing;
static void*  vTofRawCluster;
static void*  vTofRawSide;
static void*  vTofCluster;
static void*  vAntiCluster;
static void*  vTrRawCluster;
static void*  vTrCluster;
static void*  vTrRecHit;
static void*  vTrTrack;
static void*  vTrdRawHit;
static void*  vTrdCluster;
static void*  vTrdSegment;
static void*  vTrdTrack;
static void*  vLevel1;
static void*  vLevel3;
static void*  vBeta;
static void*  vVertex;
static void*  vCharge;
static void*  vParticle;
static void*  vAntiMCCluster;
static void*  vTrMCCluster;
static void*  vTofMCCluster;
static void*  vTrdMCCluster;
static void*  vRichMCCluster;
static void*  vMCTrack;
static void*  vMCEventg;
static void*  vAux;

static TTree     * _Tree;
static TTree     * _ClonedTree;

static AMSEventR * _Head;
static int         _Count;
static int         _Entry;
static char      * _Name;
public:
 static AMSEventR* & Head()  {return _Head;}
 static char *  BranchName() {return _Name;}
 void SetBranchA(TTree *tree);   // don;t use it anymore use Init



public:
   /// hbook like 1d histgoram booking by int id \n parameres like in classical hbook1
static void hbook1(int id,const char title[], int ncha, float  a, float b);  
   ///  few identical 1d histos booking in one call \n parameter howmany  number of histograms to be booked \n parameter shift    shift in id in subs hiistos
static void hbook1s(int id,const char title[], int ncha, float  a, float bi, int howmany=6,int shift=100000);
   ///  hbook like 2d histgoram booking by int id \n parameters like in classical hbook2
static void hbook2(int id,const char title[], int ncha, float  a, float b,int nchaa, float  aa, float ba);   
   ///  few identical 2d histos booking in one call \n  parameter howmany  number of histograms to be booked \n parameter shift    shift in id in subs histos
static void hbook2s(int id,const char title[], int ncha, float  a, float b,int nchaa, float  aa, float ba,int howmany=5,int shift=100000);   
   ///  hbook like profile histgoram booking by int id \n  parameters like in classical hbook1
static void hbookp(int id,const char title[], int ncha, float  a, float b);   
 /// expert only TH1F* accessor
 /// returns pointer to TH1F* by id
static  TH1F *h1(int id);
 /// expert only TH2F* accessor
 /// returns pointer to TH2F* by id
static  TH2F *h2(int id);
 /// expert only TProfile* accessor
 /// returns pointer to TH1F* by id
static  TProfile *hp(int id);
/// print histogram (eg from root session)
/// AMSEventR::hprint(id,"same");
static  void hprint(int id, char opt[]="");
/// list histos with title contains ptit or all if ptit=="" 
/// AMSEventR::hlist("xyz");
static  void hlist(char ptit[]="");
/// reset histogram by  id or all if id==0
static  void hreset(int id);
/// scale histogram id content by fac 
static  void hscale(int id, double fac);
/// fit 1d histogram by   func = "g" "e" "pN"
static  void hfit1(int id,char func[]);
///  change dir to dir
static void chdir(const char dir[]="");
/// list current dir
static void hcdir(const char dir[]="");
/// list current dir
static void ldir(){cout<<" Current Dir: "<<Service::Dir<<endl;}
/// fetch histos from TFile (to the dir dir)
static  void hfetch(TFile & f,const char dir[]="");
/// fetch histos from TFile file (to the dir file)
static  void hfetch(const char file[]);
/// delete histogram by  id or all if id==0
static  void hdelete(int id);
/// copy hist id1 to id2
static void hcopy(int id1,int id2);
/// copy hist id1 for the dir dir []to id2 to current dir
static void hcopy(char dir[],int id1, int id2);
/// divide hist id1 by id2 and put the result into id3
static void hdivide(int id1,int id2,int id3);
/// sub hist id1 by id2 and put the result into id3
static void hsub(int id1,int id2,int id3);
/// add hist id1 by id2 and put the result into id3
static void hadd(int id1,int id2,int id3);
/// general fill for 1d,2d or profile
static  void hfill(int id, float a,float b, float w);
/// fast fill for 1d histos
static  void hf1(int id,float a, float w=1);
///  fill acc to cuts for 1d histos
static  void hf1s(int id,float a, bool cuts[], int ncuts,int icut, int shift=100000,float w=1);
/// fast fill for profile histos
static  void hfp(int id,float a, float w);
/// fast fill for 2d histos
static  void hf2(int id,float a, float b,float w);



 void    Init(TTree *tree);   ///< InitTree
 void CreateBranch(TTree *tree, int brs);
 void ReSetBranchA(TTree *tree);
 void GetBranch(TTree *tree);
 void GetBranchA(TTree *tree);
 void SetCont(); 
 int & Entry(){return _Entry;}
 static TTree* & Tree()  {return _Tree;}
 static TTree* & ClonedTree()  {return _ClonedTree;}
 Int_t Fill();
 TFile* & OutputFile(){return fService._pOut;};
#ifdef  __CINT__ 
public:
#elif  defined WIN32
public:
#else
protected:
#endif
//
//  TSelector functions

   /// System function called before starting the event loop.
   /// Initializes the tree branches.
   /// Optionally opens the output file via chain.Process("xyz.C+","outputfilename");
   /// calls user UBegin()  function
   void    Begin(TTree *tree);

   /// System function called when loading a new file.
   /// Get branch pointers.
   bool  Notify(){GetBranch(_Tree);return true;}


   void    SetOption(const char *option) { fOption = option; }
   void    SetObject(TObject *obj) { fObject = obj; }
   void    SetInputList(TList *input) {fInput = input;}
   TList  *GetOutputList() const { return fOutput; }
   /// System Function called at the end of a loop on the tree
   ///  optionally stores histos in a file \sa Begin 
   /// Calls UTerminate();
   void    Terminate();

   /// Analysis kernel function.
   /// Entry is the entry number in the current tree.
   /// Should Not be modified by (Non)Advanced User\n
   /// \sa stlv.C
   /// \param entry - event no
   void ProcessFill(Long64_t entry){ProcessFill((int)entry);};
   void ProcessFill(int entry);
   bool Process(Long64_t entry){return Process((int)entry);};
   bool Process(int entry){if(ProcessCut(entry))ProcessFill(entry);return true;}
public:




// TSelector user functions
 
   /// User Analysis function called before starting the event loop.
   /// Place to book histos etc
   /// \sa stlv.C
   virtual void UBegin();
   /// User Selection function called event by event
   /// May return false as soon as a bad event is detected.\n
   /// \sa stlv.C
   /// \return false if error;
   inline virtual bool UProcessCut(){return true;}
   /// User Analysis function called event by event
   /// Place to fill in histograms etc
   /// \sa stlv.C
   virtual void UProcessFill();
   /// User Termination Function called at the end of a loop on the tree,
   /// a convenient place to draw/fit your histograms. \n
   /// \sa stlv.C
   virtual void UTerminate();


   /// Analysis Selection function.
   /// Entry is the entry number in the current tree.
   /// Read only the header to select entries.
   /// May return kFALSE as soon as a bad entry is detected.\n
   /// Should Not be modified by (Non)Advanced User\n
   /// \sa stlv.C
   /// \param entry - event no
   /// \return false if error;
   Bool_t ProcessCut(Long64_t entry){return ProcessCut((int)entry);}
   Bool_t ProcessCut(int entry){return ReadHeader(entry);}

   ///  Reads Header 
   /// \param Entry - event no
   /// \return false if error;
   bool ReadHeader(int Entry);

public:
 
  HeaderR  fHeader;  ///<  Event Header \sa HeaderR

// Some functions for inter root
//
#ifdef __ROOTSHAREDLIBRARY__
#include "root_methods.h"
#endif



int Version() const {return fHeader.Version/4;} ///< \return producer version number
int OS() const {return fHeader.Version%4;}   ///< \return producer Op Sys number  (0 -undef, 1 -dunix, 2 -linux 3 - sun )

unsigned int Run() const {return fHeader.Run;} ///< \return Run number
unsigned int Event() const {return fHeader.Event;} ///< \return Event number
char * Time() const {return ctime((time_t*)&(fHeader.Time[0]));} ///< \return  Time

int   nEcalHit()const { return fHeader.EcalHits;} ///< \return number of EcalHitR elements (fast)              
int   nEcalCluster()const { return fHeader.EcalClusters;} ///< \return number of EcalClusterR elements (fast)
int   nEcal2DCluster()const { return fHeader.Ecal2DClusters;} ///< \return number of Ecal2DClusterR elements (fast)
int   nEcalShower()const { return fHeader.EcalShowers;} ///< \return number of EcalShowerR elements (fast)
int   nRichHit()const { return fHeader.RichHits;} ///< \return number of RichHitR elements (fast)
int   nRichRing()const { return fHeader.RichRings;} ///< \return number of RichRingR elements (fast)
int   nTofRawCluster()const { return fHeader.TofRawClusters;} ///< \return number of TofRawClusterR elements (fast)
int   nTofRawSide()const { return fHeader.TofRawSides;} ///< \return number of TofRawSideR elements (fast)
int   nTofCluster()const { return fHeader.TofClusters;} ///< \return number of TofClusterR elements (fast)  
int   nAntiCluster()const { return fHeader.AntiClusters;} ///< \return number of AntiClusterR elements (fast)
int   nTrRawCluster()const { return fHeader.TrRawClusters;} ///< \return number of TrRawClusterR elements (fast)
int   nTrCluster()const { return fHeader.TrClusters;} ///< \return number of TrClusterR elements (fast)
int   nTrRecHit()const { return fHeader.TrRecHits;} ///< \return number of TrRecHitR elements (fast)
int   nTrTrack()const { return fHeader.TrTracks;} ///< \return number of TrTrackR elements (fast)
int   nTrdRawHit()const { return fHeader.TrdRawHits;} ///< \return number of TrdRawHitR elements (fast)
int   nTrdCluster()const { return fHeader.TrdClusters;} ///< \return number of TrdClusterR elements (fast)
int   nTrdSegment()const { return fHeader.TrdSegments;} ///< \return number of TrdSegmentR elements (fast)
int   nTrdTrack()const { return fHeader.TrdTracks;} ///< \return number of TrdTrackR elements (fast)
int   nLevel1()const { return fHeader.Level1s;} ///< \return number of Level1R elements (fast)
int   nLevel3()const { return fHeader.Level3s;} ///< \return number of Level3R elements (fast)
int   nBeta()const { return fHeader.Betas;} ///< \return number of BetaR elements (fast) 
int   nVertex()const { return fHeader.Vertexs;} ///< \return number of VertexR elements (fast) 
int   nCharge()const { return fHeader.Charges;} ///< \return number of ChargeR elements (fast)  
int   nParticle()const { return fHeader.Particles;} ///< \return number of ParticleR elements (fast)  
int   nAntiMCCluster()const { return fHeader.AntiMCClusters;} ///< \return number of AntiMCClusterR elements (fast)
int   nTrMCCluster()const { return fHeader.TrMCClusters;} ///< \return number of TrMCClusterR elements (fast)
int   nTofMCCluster()const { return fHeader.TofMCClusters;} ///< \return number of TofMCClusterR elements (fast)
int   nTrdMCCluster()const { return fHeader.TrdMCClusters;} ///< \return number of TrdMCClusterR elements (fast)
int   nRichMCCluster()const { return fHeader.RichMCClusters;} ///< \return number of RichMCClusterR elements (fast)
int   nMCTrack()const { return fHeader.MCTracks;} ///< \return number of MCTrackR elements (fast)
int   nMCEventg()const { return fHeader.MCEventgs;} ///< \return number of MCEventgR elements (fast)



  protected:

  
  //ECAL 

  vector<EcalHitR> fEcalHit;     
  vector<EcalClusterR> fEcalCluster;
  vector<Ecal2DClusterR> fEcal2DCluster;
  vector<EcalShowerR> fEcalShower;


  //RICH
  vector<RichHitR> fRichHit;
  vector<RichRingR> fRichRing;



  //TOF
  vector<TofRawClusterR> fTofRawCluster;
  vector<TofRawSideR> fTofRawSide;
  vector<TofClusterR> fTofCluster;  


  //Anti
  vector<AntiClusterR> fAntiCluster;


  //Tracker

  vector<TrRawClusterR> fTrRawCluster;
  vector<TrClusterR> fTrCluster;
  vector<TrRecHitR> fTrRecHit;
  vector<TrTrackR> fTrTrack;

  //TRD

  vector<TrdRawHitR> fTrdRawHit;
  vector<TrdClusterR> fTrdCluster;
  vector<TrdSegmentR> fTrdSegment;
  vector<TrdTrackR> fTrdTrack;


  //Triggers

  vector<Level1R> fLevel1;
  vector<Level3R> fLevel3;


  //AxAMS
  vector<BetaR> fBeta; 
  vector<ChargeR> fCharge;  
  vector<VertexR> fVertex;  
  vector<ParticleR> fParticle;  



  //MC SubDet
  vector<AntiMCClusterR> fAntiMCCluster;
  vector<TrMCClusterR>   fTrMCCluster;
  vector<TofMCClusterR>  fTofMCCluster;
  vector<TrdMCClusterR>  fTrdMCCluster;
  vector<RichMCClusterR> fRichMCCluster;


  //MC General

  vector<MCTrackR>       fMCTrack;
  vector<MCEventgR>      fMCEventg;


  //Aux

   vector<float>         fAux;

   public:

     /// Get into memory contents for all branches 
     void GetAllContents();

      ///  \return number of EcalHitR
      unsigned int   NEcalHit()  {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
        return fEcalHit.size();
      }
      ///  \return reference of EcalHitR Collection
      vector<EcalHitR> & EcalHit()  {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
         return  fEcalHit;
       }

       ///  EcalHitR accessor
       /// \param l index of EcalHitR Collection
      ///  \return reference to corresponding EcalHitR element
       EcalHitR &   EcalHit(unsigned int l) {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
         return fEcalHit.at(l);
      }

       ///  EcalHitR accessor
       /// \param l index of EcalHitR Collection
      ///  \return pointer to corresponding EcalHitR element
      EcalHitR *   pEcalHit(unsigned int l) {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
        return l<fEcalHit.size()?&(fEcalHit[l]):0;
      }

      ///  EcalClusterR accessor
      ///  \return number of EcalClusterR
      unsigned int   NEcalCluster()  {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
        return fEcalCluster.size();
      }
      ///  EcalClusterR accessor
      ///  \return reference of EcalClusterR Collection
      vector<EcalClusterR> & EcalCluster()  {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
         return  fEcalCluster;
       }

       ///  EcalClusterR accessor
       /// \param l index of EcalClusterR Collection
      ///  \return reference to corresponding EcalClusterR element
       EcalClusterR &   EcalCluster(unsigned int l) {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
         return fEcalCluster.at(l);
      }

       ///  EcalClusterR accessor
       /// \param l index of EcalClusterR Collection
      ///  \return pointer to corresponding EcalClusterR element
      EcalClusterR *   pEcalCluster(unsigned int l) {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
        return l<fEcalCluster.size()?&(fEcalCluster[l]):0;
      }


      
      ///  Ecal2DClusterR accessor
      ///  \return number of Ecal2DClusterR
      unsigned int   NEcal2DCluster()  {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
        return fEcal2DCluster.size();
      }
            ///  Ecal2DClusterR accessor
      ///  \return reference of Ecal2DClusterR Collection
      vector<Ecal2DClusterR> & Ecal2DCluster()  {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
         return  fEcal2DCluster;
       }
             ///  Ecal2DClusterR accessor
      /// \param l index of Ecal2DClusterR Collection
     ///  \return reference to corresponding Ecal2DClusterR element
       Ecal2DClusterR &   Ecal2DCluster(unsigned int l) {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
         return fEcal2DCluster.at(l);
      }
                 ///  Ecal2DClusterR accessor
    /// \param l index of Ecal2DClusterR Collection
   ///  \return pointer to corresponding Ecal2DClusterR element
      Ecal2DClusterR *   pEcal2DCluster(unsigned int l) {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
        return l<fEcal2DCluster.size()?&(fEcal2DCluster[l]):0;
      }


      ///  \return number of EcalShowerR
      unsigned int   NEcalShower()  {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
        return fEcalShower.size();
      }
       ///  \return reference of EcalShowerR Collection
      vector<EcalShowerR> & EcalShower()  {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
         return  fEcalShower;
       }
                 ///  EcalShowerR accessor
   /// \param l index of EcalShowerR Collection
  ///  \return reference to corresponding EcalShowerR element
       EcalShowerR &   EcalShower(unsigned int l) {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
         return fEcalShower.at(l);
      }
                ///  EcalShowerR accessor
  /// \param l index of EcalShowerR Collection
 ///  \return pointer to corresponding EcalShowerR element
      EcalShowerR *   pEcalShower(unsigned int l) {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
        return l<fEcalShower.size()?&(fEcalShower[l]):0;
      }



      ///  \return number of RichHitR
      unsigned int   NRichHit()  {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
        return fRichHit.size();
      }
      ///  \return reference of RichHitR Collection
      vector<RichHitR> & RichHit()  {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
         return  fRichHit;
       }

       ///  RichHitR accessor
       /// \param l index of RichHitR Collection
      ///  \return reference to corresponding RichHitR element
       RichHitR &   RichHit(unsigned int l) {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
         return fRichHit.at(l);
      }

       ///  RichHitR accessor
       /// \param l index of RichHitR Collection
      ///  \return pointer to corresponding RichHitR element
      RichHitR *   pRichHit(unsigned int l) {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
        return l<fRichHit.size()?&(fRichHit[l]):0;
      }






      ///  \return number of RichRingR
      unsigned int   NRichRing()  {
        if(fHeader.RichRings && fRichRing.size()==0){
           bRichRing->GetEntry(_Entry);
        }
        return fRichRing.size();
      }
      ///  \return reference of RichRingR Collection
      vector<RichRingR> & RichRing()  {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
         return  fRichRing;
       }

       ///  RichRingR accessor
       /// \param l index of RichRingR Collection
      ///  \return reference to corresponding RichRingR element
       RichRingR &   RichRing(unsigned int l) {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
         return fRichRing.at(l);
      }

       ///  RichRingR accessor
       /// \param l index of RichRingR Collection
      ///  \return pointer to corresponding RichRingR element
      RichRingR *   pRichRing(unsigned int l) {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
        return l<fRichRing.size()?&(fRichRing[l]):0;
      }






      ///  \return number of TofRawClusterR
      unsigned int   NTofRawCluster()  {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
        return fTofRawCluster.size();
      }
      ///  \return reference of TofRawClusterR Collection
      vector<TofRawClusterR> & TofRawCluster()  {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
         return  fTofRawCluster;
       }

       ///  TofRawClusterR accessor
       /// \param l index of TofRawClusterR Collection
      ///  \return reference to corresponding TofRawClusterR element
       TofRawClusterR &   TofRawCluster(unsigned int l) {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
         return fTofRawCluster.at(l);
      }

       ///  TofRawClusterR accessor
       /// \param l index of TofRawClusterR Collection
      ///  \return pointer to corresponding TofRawClusterR element
      TofRawClusterR *   pTofRawCluster(unsigned int l) {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
        return l<fTofRawCluster.size()?&(fTofRawCluster[l]):0;
      }






      ///  \return number of TofRawSideR
      unsigned int   NTofRawSide()  {
        if(fHeader.TofRawSides  && fTofRawSide.size()==0){
          if(bTofRawSide)bTofRawSide->GetEntry(_Entry);
        }
          return fTofRawSide.size();
      }
      ///  \return reference of TofRawSideR Collection
      vector<TofRawSideR> & TofRawSide()  {
        if(fHeader.TofRawSides && fTofRawSide.size()==0)bTofRawSide->GetEntry(_Entry);
         return  fTofRawSide;
       }

       ///  TofRawSideR accessor
       /// \param l index of TofRawSideR Collection
      ///  \return reference to corresponding TofRawSideR element
       TofRawSideR &   TofRawSide(unsigned int l) {
        if(fHeader.TofRawSides && fTofRawSide.size()==0)bTofRawSide->GetEntry(_Entry);
         return fTofRawSide.at(l);
      }

       ///  TofRawSideR accessor
       /// \param l index of TofRawSideR Collection
      ///  \return pointer to corresponding TofRawSideR element
      TofRawSideR *   pTofRawSide(unsigned int l) {
        if(fHeader.TofRawSides && fTofRawSide.size()==0)bTofRawSide->GetEntry(_Entry);
        return l<fTofRawSide.size()?&(fTofRawSide[l]):0;
      }




      ///  \return number of TofClusterR
      unsigned int   NTofCluster()  {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
        return fTofCluster.size();
      }
      ///  \return reference of TofClusterR Collection
      vector<TofClusterR> & TofCluster()  {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
         return  fTofCluster;
       }

       ///  TofClusterR accessor
       /// \param l index of TofClusterR Collection
      ///  \return reference to corresponding TofClusterR element
       TofClusterR &   TofCluster(unsigned int l) {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
         return fTofCluster.at(l);
      }

       ///  TofClusterR accessor
       /// \param l index of TofClusterR Collection
      ///  \return pointer to corresponding TofClusterR element
      TofClusterR *   pTofCluster(unsigned int l) {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
        return l<fTofCluster.size()?&(fTofCluster[l]):0;
      }




      ///  \return number of AntiClusterR
      unsigned int   NAntiCluster()  {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
        return fAntiCluster.size();
      }
      ///  \return reference of AntiClusterR Collection
      vector<AntiClusterR> & AntiCluster()  {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
         return  fAntiCluster;
       }

       ///  AntiClusterR accessor
       /// \param l index of AntiClusterR Collection
      ///  \return reference to corresponding AntiClusterR element
       AntiClusterR &   AntiCluster(unsigned int l) {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
         return fAntiCluster.at(l);
      }

       ///  AntiClusterR accessor
       /// \param l index of AntiClusterR Collection
      ///  \return pointer to corresponding AntiClusterR element
      AntiClusterR *   pAntiCluster(unsigned int l) {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
        return l<fAntiCluster.size()?&(fAntiCluster[l]):0;
      }


      ///  \return number of TrRawClusterR
      unsigned int   NTrRawCluster()  {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
        return fTrRawCluster.size();
      }
      ///  \return reference of TrRawClusterR Collection
      vector<TrRawClusterR> & TrRawCluster()  {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
         return  fTrRawCluster;
       }

       ///  TrRawClusterR accessor
       /// \param l index of TrRawClusterR Collection
      ///  \return reference to corresponding TrRawClusterR element
       TrRawClusterR &   TrRawCluster(unsigned int l) {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
         return fTrRawCluster.at(l);
      }

       ///  TrRawClusterR accessor
       /// \param l index of TrRawClusterR Collection
      ///  \return pointer to corresponding TrRawClusterR element
      TrRawClusterR *   pTrRawCluster(unsigned int l) {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
        return l<fTrRawCluster.size()?&(fTrRawCluster[l]):0;
      }




      ///  \return number of TrClusterR
      unsigned int   NTrCluster()  {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
        return fTrCluster.size();
      }
      ///  \return reference of TrClusterR Collection
      vector<TrClusterR> & TrCluster()  {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
         return  fTrCluster;
       }

       ///  TrClusterR accessor
       /// \param l index of TrClusterR Collection
      ///  \return reference to corresponding TrClusterR element
       TrClusterR &   TrCluster(unsigned int l) {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
         return fTrCluster.at(l);
      }

       ///  TrClusterR accessor
       /// \param l index of TrClusterR Collection
      ///  \return pointer to corresponding TrClusterR element
      TrClusterR *   pTrCluster(unsigned int l) {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
        return l<fTrCluster.size()?&(fTrCluster[l]):0;
      }




       ///  TrRecHitR accessor
      ///  \return number of TrRecHitR
      unsigned int   NTrRecHit()  {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
        return fTrRecHit.size();
      }
      ///  \return reference of TrRecHitR Collection
      vector<TrRecHitR> & TrRecHit()  {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
         return  fTrRecHit;
       }

       ///  TrRecHitR accessor
       /// \param l index of TrRecHitR Collection
      ///  \return reference to corresponding TrRecHitR element
       TrRecHitR &   TrRecHit(unsigned int l) {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
         return fTrRecHit.at(l);
      }

       ///  TrRecHitR accessor
       /// \param l index of TrRecHitR Collection
      ///  \return pointer to corresponding TrRecHitR element
      TrRecHitR *   pTrRecHit(unsigned int l) {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
        return l<fTrRecHit.size()?&(fTrRecHit[l]):0;
      }


       ///  TrTrackR accessor
      ///  \return number of TrTrackR
      unsigned int   NTrTrack()  {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
        return fTrTrack.size();
      }
      ///  \return reference of TrTrackR Collection
      vector<TrTrackR> & TrTrack()  {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
         return  fTrTrack;
       }

       ///  TrTrackR accessor
       /// \param l index of TrTrackR Collection
      ///  \return reference to corresponding TrTrackR element
       TrTrackR &   TrTrack(unsigned int l) {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
         return fTrTrack.at(l);
      }

       ///  TrTrackR accessor
       /// \param l index of TrTrackR Collection
      ///  \return pointer to corresponding TrTrackR element
      TrTrackR *   pTrTrack(unsigned int l) {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
        return l<fTrTrack.size()?&(fTrTrack[l]):0;
      }




       ///  TrdRawHitR accessor
      ///  \return number of TrdRawHitR
      unsigned int   NTrdRawHit()  {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
        return fTrdRawHit.size();
      }
      ///  \return reference of TrdRawHitR Collection
      vector<TrdRawHitR> & TrdRawHit()  {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
         return  fTrdRawHit;
       }

       ///  TrdRawHitR accessor
       /// \param l index of TrdRawHitR Collection
      ///  \return reference to corresponding TrdRawHitR element
       TrdRawHitR &   TrdRawHit(unsigned int l) {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
         return fTrdRawHit.at(l);
      }

       ///  TrdRawHitR accessor
       /// \param l index of TrdRawHitR Collection
      ///  \return pointer to corresponding TrdRawHitR element
      TrdRawHitR *   pTrdRawHit(unsigned int l) {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
        return l<fTrdRawHit.size()?&(fTrdRawHit[l]):0;
      }


       ///  TrdClusterR accessor
      ///  \return number of TrdClusterR
      unsigned int   NTrdCluster()  {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
        return fTrdCluster.size();
      }
      ///  \return reference of TrdClusterR Collection
      vector<TrdClusterR> & TrdCluster()  {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
         return  fTrdCluster;
       }

       ///  TrdClusterR accessor
       /// \param l index of TrdClusterR Collection
      ///  \return reference to corresponding TrdClusterR element
       TrdClusterR &   TrdCluster(unsigned int l) {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
         return fTrdCluster.at(l);
      }

       ///  TrdClusterR accessor
       /// \param l index of TrdClusterR Collection
      ///  \return pointer to corresponding TrdClusterR element
      TrdClusterR *   pTrdCluster(unsigned int l) {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
        return l<fTrdCluster.size()?&(fTrdCluster[l]):0;
      }


       ///  TrdSegmentR accessor
      ///  \return number of TrdSegmentR
      unsigned int   NTrdSegment()  {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
        return fTrdSegment.size();
      }
      ///  \return reference of TrdSegmentR Collection
      vector<TrdSegmentR> & TrdSegment()  {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
         return  fTrdSegment;
       }

       ///  TrdSegmentR accessor
       /// \param l index of TrdSegmentR Collection
      ///  \return reference to corresponding TrdSegmentR element
       TrdSegmentR &   TrdSegment(unsigned int l) {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
         return fTrdSegment.at(l);
      }

       ///  TrdSegmentR accessor
       /// \param l index of TrdSegmentR Collection
      ///  \return pointer to corresponding TrdSegmentR element
      TrdSegmentR *   pTrdSegment(unsigned int l) {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
        return l<fTrdSegment.size()?&(fTrdSegment[l]):0;
      }



       ///  TrdTrackR accessor
      ///  \return number of TrdTrackR
      unsigned int   NTrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return fTrdTrack.size();
      }
      ///  \return reference of TrdTrackR Collection
      vector<TrdTrackR> & TrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return  fTrdTrack;
       }

       ///  TrdTrackR accessor
       /// \param l index of TrdTrackR Collection
      ///  \return reference to corresponding TrdTrackR element
       TrdTrackR &   TrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return fTrdTrack.at(l);
      }

       ///  TrdTrackR accessor
       /// \param l index of TrdTrackR Collection
      ///  \return pointer to corresponding TrdTrackR element
      TrdTrackR *   pTrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return l<fTrdTrack.size()?&(fTrdTrack[l]):0;
      }




       ///  Level1R accessor
      ///  \return number of Level1R
      unsigned int   NLevel1()  {
        if(fHeader.Level1s && fLevel1.size()==0)bLevel1->GetEntry(_Entry);
        return fLevel1.size();
      }
      ///  \return reference of Level1R Collection
      vector<Level1R> & Level1()  {
        if(fHeader.Level1s && fLevel1.size()==0)bLevel1->GetEntry(_Entry);
         return  fLevel1;
       }

       ///  Level1R accessor
       /// \param l index of Level1R Collection
      ///  \return reference to corresponding Level1R element
       Level1R &   Level1(unsigned int l) {
        if(fHeader.Level1s && fLevel1.size()==0)bLevel1->GetEntry(_Entry);
         return fLevel1.at(l);
      }

       ///  Level1R accessor
       /// \param l index of Level1R Collection
      ///  \return pointer to corresponding Level1R element
      Level1R *   pLevel1(unsigned int l=0) {
        if(fHeader.Level1s && fLevel1.size()==0)bLevel1->GetEntry(_Entry);
        return l<fLevel1.size()?&(fLevel1[l]):0;
      }






       ///  Level3R accessor
      ///  \return number of Level3R
      unsigned int   NLevel3()  {
        if(fHeader.Level3s && fLevel3.size()==0)bLevel3->GetEntry(_Entry);
        return fLevel3.size();
      }
      ///  \return reference of Level3R Collection
      vector<Level3R> & Level3()  {
        if(fHeader.Level3s && fLevel3.size()==0)bLevel3->GetEntry(_Entry);
         return  fLevel3;
       }

       ///  Level3R accessor
       /// \param l index of Level3R Collection
      ///  \return reference to corresponding Level3R element
       Level3R &   Level3(unsigned int l) {
        if(fHeader.Level3s && fLevel3.size()==0)bLevel3->GetEntry(_Entry);
         return fLevel3.at(l);
      }

       ///  Level3R accessor
       /// \param l index of Level3R Collection
      ///  \return pointer to corresponding Level3R element
      Level3R *   pLevel3(unsigned int l) {
        if(fHeader.Level3s && fLevel3.size()==0)bLevel3->GetEntry(_Entry);
        return l<fLevel3.size()?&(fLevel3[l]):0;
      }










       ///  BetaR accessor
      ///  \return number of BetaR
      unsigned int   NBeta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return fBeta.size();
      }
      ///  \return reference of BetaR Collection
      vector<BetaR> & Beta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return  fBeta;
       }

       ///  BetaR accessor
       /// \param l index of BetaR Collection
      ///  \return reference to corresponding BetaR element
       BetaR &   Beta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return fBeta.at(l);
      }

       ///  BetaR accessor
       /// \param l index of BetaR Collection
      ///  \return pointer to corresponding BetaR element
      BetaR *   pBeta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return l<fBeta.size()?&(fBeta[l]):0;
      }










       ///  ChargeR accessor
      ///  \return number of ChargeR
      unsigned int   NCharge()  {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
        return fCharge.size();
      }
      ///  \return reference of ChargeR Collection
      vector<ChargeR> & Charge()  {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
         return  fCharge;
       }

       ///  ChargeR accessor
       /// \param l index of ChargeR Collection
      ///  \return reference to corresponding ChargeR element
       ChargeR &   Charge(unsigned int l) {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
         return fCharge.at(l);
      }

       ///  ChargeR accessor
       /// \param l index of ChargeR Collection
      ///  \return pointer to corresponding ChargeR element
      ChargeR *   pCharge(unsigned int l) {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
        return l<fCharge.size()?&(fCharge[l]):0;
      }


       ///  VertexR accessor
      ///  \return number of VertexR
      unsigned int   NVertex()  {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
        return fVertex.size();
      }
      ///  \return reference of VertexR Collection
      vector<VertexR> & Vertex()  {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
         return  fVertex;
       }

       ///  VertexR accessor
       /// \param l index of VertexR Collection
      ///  \return reference to corresponding VertexR element
       VertexR &   Vertex(unsigned int l) {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
         return fVertex.at(l);
      }

       ///  VertexR accessor
       /// \param l index of VertexR Collection
      ///  \return pointer to corresponding VertexR element
      VertexR *   pVertex(unsigned int l) {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
        return l<fVertex.size()?&(fVertex[l]):0;
      }

       ///  ParticleR accessor
      ///  \return number of ParticleR
      unsigned int   NParticle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return fParticle.size();
      }
      ///  \return reference of ParticleR Collection
      vector<ParticleR> & Particle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return  fParticle;
       }

       ///  ParticleR accessor
       /// \param l index of ParticleR Collection
      ///  \return reference to corresponding ParticleR element
       ParticleR &   Particle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return fParticle.at(l);
      }

       ///  ParticleR accessor
       /// \param l index of ParticleR Collection
      ///  \return pointer to corresponding ParticleR element
      ParticleR *   pParticle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return l<fParticle.size()?&(fParticle[l]):0;
      }




       ///  AntiMCClusterR accessor
      ///  \return number of AntiMCClusterR
      unsigned int   NAntiMCCluster()  {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
        return fAntiMCCluster.size();
      }
      ///  \return reference of AntiMCClusterR Collection
      vector<AntiMCClusterR> & AntiMCCluster()  {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
         return  fAntiMCCluster;
       }

       ///  AntiMCClusterR accessor
       /// \param l index of AntiMCClusterR Collection
      ///  \return reference to corresponding AntiMCClusterR element
       AntiMCClusterR &   AntiMCCluster(unsigned int l) {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
         return fAntiMCCluster.at(l);
      }

       ///  AntiMCClusterR accessor
       /// \param l index of AntiMCClusterR Collection
      ///  \return pointer to corresponding AntiMCClusterR element
      AntiMCClusterR *   pAntiMCCluster(unsigned int l) {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
        return l<fAntiMCCluster.size()?&(fAntiMCCluster[l]):0;
      }





       ///  TofMCClusterR accessor
      ///  \return number of TofMCClusterR
      unsigned int   NTofMCCluster()  {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
        return fTofMCCluster.size();
      }
      ///  \return reference of TofMCClusterR Collection
      vector<TofMCClusterR> & TofMCCluster()  {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
         return  fTofMCCluster;
       }

       ///  TofMCClusterR accessor
       /// \param l index of TofMCClusterR Collection
      ///  \return reference to corresponding TofMCClusterR element
       TofMCClusterR &   TofMCCluster(unsigned int l) {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
         return fTofMCCluster.at(l);
      }

       ///  TofMCClusterR accessor
       /// \param l index of TofMCClusterR Collection
      ///  \return pointer to corresponding TofMCClusterR element
      TofMCClusterR *   pTofMCCluster(unsigned int l) {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
        return l<fTofMCCluster.size()?&(fTofMCCluster[l]):0;
      }




       ///  TrMCClusterR accessor
      ///  \return number of TrMCClusterR
      unsigned int   NTrMCCluster()  {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
        return fTrMCCluster.size();
      }
      ///  \return reference of TrMCClusterR Collection
      vector<TrMCClusterR> & TrMCCluster()  {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
         return  fTrMCCluster;
       }

       ///  TrMCClusterR accessor
       /// \param l index of TrMCClusterR Collection
      ///  \return reference to corresponding TrMCClusterR element
       TrMCClusterR &   TrMCCluster(unsigned int l) {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
         return fTrMCCluster.at(l);
      }

       ///  TrMCClusterR accessor
       /// \param l index of TrMCClusterR Collection
      ///  \return pointer to corresponding TrMCClusterR element
      TrMCClusterR *   pTrMCCluster(unsigned int l) {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
        return l<fTrMCCluster.size()?&(fTrMCCluster[l]):0;
      }





       ///  TrdMCClusterR accessor
      ///  \return number of TrdMCClusterR
      unsigned int   NTrdMCCluster()  {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
        return fTrdMCCluster.size();
      }
      ///  \return reference of TrdMCClusterR Collection
      vector<TrdMCClusterR> & TrdMCCluster()  {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
         return  fTrdMCCluster;
       }

       ///  TrdMCClusterR accessor
       /// \param l index of TrdMCClusterR Collection
      ///  \return reference to corresponding TrdMCClusterR element
       TrdMCClusterR &   TrdMCCluster(unsigned int l) {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
         return fTrdMCCluster.at(l);
      }

       ///  TrdMCClusterR accessor
       /// \param l index of TrdMCClusterR Collection
      ///  \return pointer to corresponding TrdMCClusterR element
      TrdMCClusterR *   pTrdMCCluster(unsigned int l) {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
        return l<fTrdMCCluster.size()?&(fTrdMCCluster[l]):0;
      }





       ///  RichMCClusterR accessor
      ///  \return number of RichMCClusterR
      unsigned int   NRichMCCluster()  {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
        return fRichMCCluster.size();
      }
      ///  \return reference of RichMCClusterR Collection
      vector<RichMCClusterR> & RichMCCluster()  {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
         return  fRichMCCluster;
       }

       ///  RichMCClusterR accessor
       /// \param l index of RichMCClusterR Collection
      ///  \return reference to corresponding RichMCClusterR element
       RichMCClusterR &   RichMCCluster(unsigned int l) {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
         return fRichMCCluster.at(l);
      }

       ///  RichMCClusterR accessor
       /// \param l index of RichMCClusterR Collection
      ///  \return pointer to corresponding RichMCClusterR element
      RichMCClusterR *   pRichMCCluster(unsigned int l) {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
        return l<fRichMCCluster.size()?&(fRichMCCluster[l]):0;
      }





       ///  MCTrackR accessor
      ///  \return number of MCTrackR
      unsigned int   NMCTrack()  {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
        return fMCTrack.size();
      }
      ///  \return reference of MCTrackR Collection
      vector<MCTrackR> & MCTrack()  {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
         return  fMCTrack;
       }

       ///  MCTrackR accessor
       /// \param l index of MCTrackR Collection
      ///  \return reference to corresponding MCTrackR element
       MCTrackR &   MCTrack(unsigned int l) {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
         return fMCTrack.at(l);
      }

       ///  MCTrackR accessor
       /// \param l index of MCTrackR Collection
      ///  \return pointer to corresponding MCTrackR element
      MCTrackR *   pMCTrack(unsigned int l) {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
        return l<fMCTrack.size()?&(fMCTrack[l]):0;
      }






       ///  MCEventgR accessor
      ///  \return number of MCEventgR
      unsigned int   NMCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return fMCEventg.size();
      }
      ///  \return reference of MCEventgR Collection
      vector<MCEventgR> & MCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return  fMCEventg;
       }

       ///  MCEventgR accessor
       /// \param l index of MCEventgR Collection
      ///  \return reference to corresponding MCEventgR element
       MCEventgR &   MCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return fMCEventg.at(l);
      }

       ///  MCEventgR accessor
       /// \param l index of MCEventgR Collection
      ///  \return pointer to corresponding MCEventgR element
      MCEventgR *   pMCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return l<fMCEventg.size()?&(fMCEventg[l]):0;
      }






AMSEventR();

virtual ~AMSEventR(){
cout <<"AMSEventR::dtor-I-Destroying "<<_Count<<endl;;
if(!--_Count){
 cout<<"AMSEventR::dtor-I-ResettingHead "<<_Head<<endl;
 _Head=0;
}
}



void clear();

#ifndef __ROOTSHAREDLIBRARY__
void         AddAMSObject(AMSAntiCluster *ptr);
void         AddAMSObject(AMSAntiMCCluster *ptr);
void         AddAMSObject(AMSBeta *ptr);
void         AddAMSObject(AMSCharge *ptr, float probtof[],int chintof[],
                          float probtr[], int chintr[], float probrc[], 
                          int chinrc[], float proballtr);
void         AddAMSObject(AMSEcalHit *ptr);
void         AddAMSObject(AMSmceventg *ptr);
void         AddAMSObject(AMSmctrack *ptr);
void         AddAMSObject(AMSVtx *ptr);
void         AddAMSObject(AMSParticle *ptr, float phi, float phigl);
void         AddAMSObject(AMSRichMCHit *ptr, int numgen);
void         AddAMSObject(AMSRichRing *ptr);
void         AddAMSObject(AMSRichRawEvent *ptr, float x, float y, float z);
void         AddAMSObject(AMSTOFCluster *ptr);
void         AddAMSObject(AMSTOFMCCluster *ptr);
void         AddAMSObject(AMSTrRecHit *ptr);
void         AddAMSObject(AMSTRDCluster *ptr);
void         AddAMSObject(AMSTRDMCCluster *ptr);
void         AddAMSObject(AMSTRDRawHit *ptr);
void         AddAMSObject(AMSTRDSegment *ptr);
void         AddAMSObject(AMSTRDTrack *ptr);
void         AddAMSObject(AMSTrCluster *ptr);
void         AddAMSObject(AMSTrMCCluster *ptr);
void         AddAMSObject(AMSTrRawCluster *ptr);
void         AddAMSObject(AMSTrTrack *ptr);
void         AddAMSObject(Ecal1DCluster *ptr);
void         AddAMSObject(AMSEcal2DCluster  *ptr);
void         AddAMSObject(AMSEcalShower  *ptr);
void         AddAMSObject(TOF2RawCluster *ptr);
void         AddAMSObject(TOF2RawSide *ptr);
void         AddAMSObject(Trigger2LVL1 *ptr);
void         AddAMSObject(TriggerLVL302 *ptr);
#endif

ClassDef(AMSEventR,3)       //AMSEventR
};

//!  AMSChain class
/*!
  Utility class, to simplify the interactive analysis on AMS data with 
  Cint/Python/Ruby interpreters. Example of use with Cint:

  \include ana.root_cint.C

  \author juan.alcaraz@cern.ch

*/

class AMSChain : public TChain {
private:
      AMSEventR* _EVENT;
      Int_t _ENTRY;
      const char* _NAME;
      Int_t _TREENUMBER;

public:
      AMSChain(const char* name="AMSRoot"); ///< Default constructor
      virtual ~AMSChain(){ if (_EVENT) delete _EVENT; };

      void Init(); ///<Set event branch and links; called after reading of all trees; called automatically in GetEvent
      AMSEventR* GetEvent(); ///<Get next AMSEventR object in the chain
      AMSEventR* GetEvent(Int_t entry); ///<Get AMSEventR in entry number "entry"
      AMSEventR* GetEvent(Int_t run, Int_t ev); ///<Get AMSEventR with run number "run" and event number "ev"
      void Rewind() {_ENTRY=-1;}; ///<Rewind the chain (go back before first entry)

      Int_t Entry(); ///<Get the current entry number
      AMSEventR* pEvent(); ///<Get the current event pointer
      const char* ChainName(); ///<Get the name of the tree

      ClassDef(AMSChain,5)       //AMSChain
};

//!  AMSEventList class
/*!
  Utility class, to select a set of events and then having the possibility to write
  them into a new files, possibly with just a set of selected branches. 
  Example:

  \include select_entries.C

  \author juan.alcaraz@cern.ch

*/

class AMSEventList {
private:
      vector<int> _RUNs;
      vector<int> _EVENTs;

public:
      AMSEventList(); ///< Default Constructor
      AMSEventList(const char* filename); ///< Constructor with an already existing list
      virtual ~AMSEventList(){};

      void Add(int run, int event); ///<Add a (run,event) number to the list
      void Add(AMSEventR* pev); ///<Add the (run,event) number of this AMSEventR object to the list
      void Remove(int run, int event); ///<Remove a (run,event) number from the list
      void Remove(AMSEventR* pev); ///<Remove the (run,event) number of this AMSEventR object from the list
      bool Contains(int run, int event); ///<Returns true if the list contains this (run,event)
      bool Contains(AMSEventR* pev); ///<Returns true if the list contains the (run,event) of this AMSEventR object
      void Reset(); ///<Reset the array
      void Read(const char* filename); ///<Read "run event" list from ASCII file
      void Write(); ///<Write "run event" list to standard output
      void Write(const char* filename); ///<Write "run event" list to ASCII file
      void Write(AMSChain* chain, TFile* file); ///<Write/Add selected events from a chain into a ROOT file

      int GetEntries(); ///<Number of events in the list
      int GetRun(int i); ///<Retrieve run number for entry i
      int GetEvent(int i); ///<Retrieve event number for entry i

      ClassDef(AMSEventList,2)       //AMSEventList
};

typedef AMSEventR  ams;
#endif

