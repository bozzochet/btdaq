// $Id: TkDBc.h,v 1.26 2008/12/23 11:37:57 zuccon Exp $

#ifndef __TkDBC__
#define __TkDBC__

#include "typedefs.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TkLadder.h"
#include "TkPlane.h"

/*!\class TkDBc
\brief The new AMS Tracker database class
\ingroup tkdbc

 The TkDBc Class is a singleton, that is only one instance
 of this class is admitted. The TkDBc single instance is created
 via the static method TkDBc::CreateTkDBc()
 and is accessibile via the static pointer TkDBc::Head.\n
 The access of to the ladder objects is done
 via the find functions that implement a fast binary search.

 author P.Zuccon -- INFN Perugia 20/11/2007

\date  2008/01/17 PZ  First version
\date  2008/01/21 SH  File name, class name changed to avoid conflicts
                      Some bugs are fixed
\date  2008/01/23 SH  Some comments are added
\date  2008/02/02 SH  New method GetTkId(x,y,z) added
\date  2008/02/26 AO  New method GetEntry() for TkId map added

*/

namespace trconst{
  //! number of planes
  const integer nplanes=5;
  //! number of layers
  const integer nlays=8;
  //! number of layers FIX ME remove this
  const integer maxlay=8;
  //! maximal number of ladders
  const integer maxlad=15;
  //! number of ladders for each layer and side (0 X neg, 1 X pos)
  const integer nlad[2][nlays]={{15,12,11,10,10,11,12,15},{15,12,11,10,10,11,12,15}};    
  //! number of crates
  const int ncrt=8;
  //! number of TDRs per crate
  const int ntdr=24;

  //!FIXME  for compatibility
 const integer TrkTypes=3;//for charge algorithm(
  //!FIXME  for compatibility
 const integer TrkPdfBins=100;//max bins in Elos PDF's



}
using namespace trconst;

class TkDBc{

public:

  //! static pointer to the TkDBc singleton
  /*! The TkDBc data and methods are always accessibile via TkDBc::Head
    Example: TkDBc::Head->_nplanes  or TkDBc::FindTkId(-212);
  */
  static TkDBc* Head;


  // ------- PLANES -----------------------------   

  //! nominal X coo of the planes
  number   _xpos[nplanes];
  //! nominal Y coo of the planes
  number   _ypos[nplanes];
  //! nominal Z coo of the planes
  number   _zpos[nplanes];
  //! Number of avalilable slot per planes (top view, bottom view)
  int16     _nslot[nplanes][2];


  // Plane support thickness in cm
  number _sup_hc_w[nplanes];
  
  
  // Plane support radius in cm
  number _sup_hc_r[nplanes];

  //! Plane envelop (used for GEANT) radius in cm
  double _plane_d1[nplanes];
  
  //! Plane envelop (used for GEANT) half thickness in cm
  double _plane_d2[nplanes];
  
  //! To account for the envelop assimmetry of external planes
  double _dz[nplanes];

  // ------- LAYERS -----------------------------   
  //! Map correlating  layer to supporting plane
  integer _plane_layer[nlays];

  //! Z distance of the silicon surface from the middle of the plane;
  number _layer_deltaZ[nlays];

  //! number of active ladders for side/layer
  integer  _nlad[2][nlays];




  //--------------  LADDERS --------------------------------
  //! ladder pitch on Y 
  number _ladder_Ypitch;

  //! X offset of the ladders for the inner planes
  number _ladder_offsetX_inner[maxlad];

  //! X offset of the ladders for the outer planes
  number _ladder_offsetX_outer[maxlad];

  //! Ladder X separation
  number _ladder_Xseparation;

  //! map of the ladder edge distance (mm) from Y axis
  number _LadDeltaX[2][nlays][maxlad];

  //! Map of the ladder lenght in unit of Silicon sensors
  short int _nsen[2][nlays][maxlad];

  //! Map of the ladder power supply in the crate is pwgp*100+pwpos
  short int _pgid[2][nlays][maxlad];

  //! Map of the Ladder names
  char _LadName[2][nlays][maxlad][9];

  //! It returns the position on the plane (Y coordinate) of the first p-side readout channel 
  number GetSlotY(int layer, int slot,int side);
  
  //! It returns the position on the plane (X coordinate) of the first n-side read out channel 
  number GetSlotX(int layer, int slot,int side);

  //! It return the number of slots for the layer [0-7] and side [0 PORT: 1 XXX]
  int GetNslots(int layer, int side){ return nlad[side][layer];}

  //! returns the number of silicon layers
  integer nlay() const {return nlays;}
  



  // -------- CABLING ------------------------------
  //! Map of the ladders cabling the number is octatn*100+TDR
  short int _octid[2][nlays][maxlad];


  /** \brief Map of the crate number corresponting to the octants
      
      Octant are numbered the following way 
      
      NEGATIVE Y corresponds to RAM. 
      POSITIVE Y corresponds to WAKE.
      
      V    Oct X  Y  Z   Crate    JINJ                JMDC
      
                            (slave num)           A    B   P   S  
      
      0 or 1P  + WAKE +     T0       3           166  167 168 169     
      1 or 2M  - WAKE +     T1       9           170  171 172 173
      2 or 3P  + RAM  +     T3       1           178  179 180 181
      3 or 4M  - RAM  +     T2       0           174  175 176 177
      4 or 5P  + WAKE -     T4      16           182  183 184 185
      5 or 6M  - WAKE -     T5      17           186  187 188 189
      6 or 7P  + RAM  -     T7      23           194  195 196 197
      7 or 8M  - RAM  -     T6      22           190  191 192 193
  */
  short int _octant_crate[8];
  //! see _octant_crate
  short int _octant_JinJ[8];

  short int _octant_JMDC[8][4];


  

public:
  //! It returns true if a slot contains a ladder
  bool filled_slot(int side,int layer, int slot){return _nsen[side][layer][slot]>0;}
 
  //! Return a string inicating the  postion of the octant
  char * GetOctName(int oct);
  
  //! Return a string inicating the  geographic postion of the crate 
  char * GetCratePos(int crate);

  int GetOctFromCrate(int crate)
  { int oct=-1; for (int ii=0;ii<8;ii++) if(_octant_crate[ii]==crate) oct=ii; return (oct+1); }
 
  int GetOctFromJinJ(int JinJ)
  { int oct=-1; for (int ii=0;ii<8;ii++) if(_octant_JinJ[ii]==JinJ) oct=ii; return (oct+1); }

  int GetCrateFromJinJ(int JinJ)
  { int oct=GetOctFromJinJ(JinJ); oct--; return _octant_crate[oct]; }

  //! Dimension of the ladder electronics
  number  _zelec[3];



 //  //Silicon sensors
  //! Sensor active area (0 K  1 S)
  number   _ssize_active[2];
  //! Sensor full  area (0 K  1 S)
  number   _ssize_inactive[2];
  //! Sensor thikness
  number   _silicon_z;

  int _NReadStripK7;
  int _NReadStripK5;
  int _NReadoutChanS;
  int _NReadoutChanK;
  number _SensorPitchK;
  number _PitchS;
  number _PitchK5;
  number _PitchK7;
  number _ImplantPitchK;
//     integer  _nstripssen[nlays][2]; //number of strips
//     integer  _nstripssenR[nlays][2];
//     integer  _nstripsdrp[nlays][2];
  
//     number   _zelec[nlays][3];
//     number   _c2c[nlays];
//     number   _halfldist[nlays];
//     number   _support_foam_w[nlays];
//     number   _support_foam_tol[nlays];
//     number   _support_hc_w[nlays];
//     number   _support_hc_r[nlays];
//     number   _support_hc_z[nlays];
//     integer  _nladshuttle[nlays][2];
//     integer  _boundladshuttle[nlays][2];
//     number   _PlMarkerPos[nlays][2][4][3];  // 1st wjb

private:
  //! Map for fast binary search based on TkAssemblyId
  map<int,TkLadder*> tkassemblymap;
  //! Map for fast binary search based on TkId
  map<int,TkLadder*> tkidmap;
  //! Map for fast binary search based on HwId
  map<int,TkLadder*> hwidmap;
  //! Map for fast binary search based on PgId
  map<int,TkLadder*> pgidmap;
  //! Map for fast binary search based on LadName
  map<string,TkLadder*> lnamemap;
  //! Map for fast binary search based on JMDCNum
  map<int,TkLadder*> JMDCNumMap;

  TkLadder* Findmap(map<int,TkLadder*> & tkmap, int key);

  int GetOctant(int side,int _layer,int _slot);

  //! Constructor is private since the class is a singleton
  TkDBc();

  //Pointer ro the planes;
  TkPlane* planes[nplanes];
  

public:

  //! Static function used to create the TkDBc Singleton. If force_delete>0 it deletes and recreates  the single istance of the class
  static void CreateTkDBc(int force_delete=0);
  ~TkDBc();
  //! Do all the initialization stuff
  void init(const char* inputfilename=0, int pri=0);
  //!  Write the content of the DB to an ascii file
  int  write(const char* filename);
  //!  Read the  content of the DB from an ascii file
  int  read(const char* filename, int pri=0);
  //!  Read the alignement data from a file
  int readAlignment(const char* filename, int pri=0);

  //!  Write the alignement data to a file
  int writeAlignment(const char* filename);

  //! Return the pointer to the iith (ii [1-5]) TkPlane object 
  TkPlane* GetPlane(int ii) {if (ii>0&&ii<6) return planes[ii-1]; else return 0;}

  //! map (TkId) number of elements
  int GetEntries() { return tkidmap.size(); }
  //! map (TkId) iterator index
  TkLadder* GetEntry(int ii);
  //! TkId for the ii entry in the TkId map (0-191)
  int Entry2TkId(int ii);
  //! Entry (0-191) of the TkId index in the map
  int TkId2Entry(int TkId);

  //! Returns the pointer to the ladder object with the required Assembly id. In case of failure returns a NULL pointer
  TkLadder* FindTkAssemblyId( int tkassemblyid){ return Findmap(tkassemblymap,tkassemblyid);}
  //! Returns the pointer to the ladder object with the required tkid. In case of failure returns a NULL pointer
  TkLadder* FindTkId( int tkid){ return Findmap(tkidmap,tkid);}
  //! Returns the pointer to the ladder object with the required HwId. In case of failure returns a NULL pointer
  TkLadder* FindHwId( int hwid){ return Findmap(hwidmap,hwid);}
  //! Returns the pointer to the ladder object with the required PgId. In case of failure returns a NULL pointer
  TkLadder* FindPgId(int pgid){ return Findmap(pgidmap,pgid); }
   //! Returns the pointer to the ladder object with the required ladder name. In case of failure returns a NULL pointer
  TkLadder* FindLadName(string& name);
  //! Returns the HwId corresponding to a given HwId
  int HwId2Tkid(int tkid){
    TkLadder* ll= FindTkId(tkid);
    if(ll) return ll->GetHwId();
    else return -1;
  }
  //! Get X-coordinate of layer
  double GetXlayer(int i) {
    return (1 <= i && i <= nlays)
      ? GetPlane(_plane_layer[i-1])->pos[0]: -9999.;} 
  
  //! Get Y-coordinate of layer
  double GetYlayer(int i) {
    return (1 <= i && i <= nlays)
      ? GetPlane(_plane_layer[i-1])->pos[1]: -9999.;} 
  
  //! Get Z-coordinate of layer
  double GetZlayer(int i) {
    return (1 <= i && i <= nlays) 
      ? GetPlane(_plane_layer[i-1])->pos[2]+_layer_deltaZ[i-1] : -99999.;}
  //! copy the rotation matrix into nrm array
  void GetLayerRot(int lay,number nrm[][3]);

  //! Kill TkLadder
  void KillTkId(int tkid) { tkidmap[tkid] = 0; }

  int NStripsDrp(int type){  return (type==0)?_NReadoutChanK: _NReadoutChanS;}


};

typedef map<int,TkLadder*>::const_iterator tkidIT;
typedef map<string,TkLadder*>::const_iterator lnameIT;



#endif

