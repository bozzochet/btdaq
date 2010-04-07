#ifndef __AMSTrRecHit__
#define __AMSTrRecHit__

//////////////////////////////////////////////////////////////////////////
///
///
///\class AMSTrRecHit
///\brief A class to manage hit reconstruction in AMS Tracker
///\ingroup tkrec
///
/// AMSTrRecHit is a core of the hit reconstruction.
/// New ladder geometry (TkDBc) and calibration databases (TrCalDB) 
/// are used instead of the original TKDBc, TrIdSoft, and TrIdGeom. 
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/28 SH  First refinement (for TkTrack)
///\date  2008/01/14 SH  First stable vertion after a refinement
///\date  2008/01/21 SH  Imported to tkdev (test version)
///\date  2008/02/13 SH  Comments are updated
///\date  2008/02/19 AO  New data format 
///\date  2008/02/22 AO  Temporary clusters reference  
///\date  2008/02/26 AO  Local and global coordinate (TkCoo.h)
///\data  2008/03/06 AO  Changing some data members and methods
///\data  2008/04/12 AO  From XEta to XCofG(3) (better for inclination)
///\data  2008/11/29 SH  _dummyX added, _residual moved to TrTrack
///
//////////////////////////////////////////////////////////////////////////
#include <string>
#include "link.h"
#include "TrCluster.h"
#include "point.h"
#include "TkCoo.h"
#include "TkDBc.h"

#include "TRef.h"
#include "TProcessID.h"
#include "VCon.h"

#define XONLY 0x100
#define YONLY 0x200

//class AMSTrRecHit : public AMSlink, public TObject {
class AMSTrRecHit : public AMSlink {
private:
  /// Pointer to the X (n-side) AMSTrCluster in the fAMSTrCluster collection
  AMSTrCluster*  _clusterX; //!
  /// Pointer to the Y (p-side) AMSTrCluster in the fAMSTrCluster collection
  AMSTrCluster*  _clusterY; //!
  
public:
  /// TkLadder ID
  short int   _tkid;
  /// Correlation between X and Y
  float _corr;
  /// Probability of correlation between the X and Y clusters 
  float _prob;
  /// Hit status (...)
  int   _status;
  /// Hit multiplicity 
  short int   _mult;
  /// Multiplicity index (-1 means not yet resolved, >-1 resolved by tracking algorithm)
  short int _imult;
  /// Dummy X-strip position for YONLY hit
  float _dummyX;
  /// Hit global coordinate (multiplicity vector) 
  vector<AMSPoint> _coord;

  /// X Cluster index
  int _iclusterX;
  /// Y Cluster index
  int _iclusterY;

  static string sout;
  static VCon* vcon;

 public:
  /// Default constructor
  AMSTrRecHit(void);
  /// Copy constructor
  AMSTrRecHit(const AMSTrRecHit& orig);
  /// Constructor with clusters
  AMSTrRecHit(int tkid, AMSTrCluster* clX, AMSTrCluster* clY, float corr, float prob, int imult = -1);
  /// Destructor
  ~AMSTrRecHit();
  /// Clear data members
  void Clear();
  
  /// Build coordinates
  void BuildCoordinates();

  /// Get ladder TkId identifier 
  int   GetTkId()        const { return _tkid; }
  /// Get ladder layer
  int   GetLayer()       const { return abs(_tkid/100); }
  /// Get ladder slot
  int   GetSlot()        const { return abs(_tkid%100); }
  /// Get correlation between the X and Y clusters
  float GetCorrelation() const { return _corr;   }
  /// Get probability of correlation between the X and Y clusters 
  float GetProb()        const { return _prob;   }
  /// Get hit status
  int   GetStatus()      const { return _status; }
  /// Add status bit
  void  SetStatus(int bit) { _status |= bit; }
  /// Get the pointer to X cluster
  AMSTrCluster* GetXCluster();

  /// Get the pointer to Y cluster
  AMSTrCluster* GetYCluster();

  /// Get the index of X cluster
  int GetXClusterIndex() const { return _iclusterX; }
  /// Get the index of Y cluster
  int GetYClusterIndex() const { return _iclusterY; }

  /// Get the hit multiplicity 
  int   GetMultiplicity()      { return _mult; }
  /// Get the computed global coordinate ny multiplicity index
  AMSPoint GetCoord(int imult) { if(_coord.empty()) BuildCoordinates(); return _coord[imult]; }
  /// Get the resolved multiplicity index (-1 if not resolved)
  int   GetResolvedMultiplicity() { return _imult; }
  /// Set the resolved multiplicity index (-1 if not resolved)
  void  SetResolvedMultiplicity(int im) { _imult = im; }
  /// Returns the computed global coordinate (if resolved)
  AMSPoint GetCoord() { return ( (0<=_imult) && (_imult<_mult) ) ? _coord[_imult] : AMSPoint(0, 0, 0); }
  /// Returns the errors on the computed global coordinate (if resolved)
  AMSPoint GetECoord() {return AMSPoint(0.002,0.003,0.015);}

  /// Get dummy strip position
  float GetDummyX() { return _dummyX; }
  /// Set dummy strip position
  void SetDummyX(float dumx) { _dummyX = dumx; }

  /// Get X local coordinate (ladder reference frame)
  float GetXloc(int imult = 0) { return (!GetXCluster())?TkCoo::GetLocalCoo(_tkid,_dummyX+640,imult)
                                                        :GetXCluster()->GetXCofG(3,imult); }
  /// Get Y local coordinate (ladder reference frame)
  float GetYloc()              { return (!GetYCluster())?-1000.:GetYCluster()->GetXCofG(3);      }    
  /// Get local coordinate (ladder reference frame, Z is zero by definition)
  AMSPoint GetLocalCoordinate(int imult = 0) { AMSPoint loc(GetXloc(imult),GetYloc(),0.); return loc; }	
  /// Get global coordinate (AMS reference system) 
  /// default: nominal position, A: with alignement correction
  AMSPoint GetGlobalCoordinate(int imult = 0, char* options = "A");

  /// Print cluster basic information
  std::ostream& putout(std::ostream &ostr = std::cout) const;
  friend std::ostream &operator << (std::ostream &ostr, const AMSTrRecHit &hit) { return hit.putout(ostr); } 
  /// Print hit  
  void  Print();
  void Info();

  AMSTrRecHit* next() {return (AMSTrRecHit*) _next;}

  void _copyEl(){}
  void _printEl(std::ostream& ostr){ putout(ostr);}
  void _writeEl(){}

   void * operator new(size_t t) {return TObject::operator new(t); }
   void operator delete(void *p)   {TObject::operator delete(p);p=0;}



  /// ROOT definition
  ClassDef(AMSTrRecHit,1)
};
typedef AMSTrRecHit TrRecHitR;
#endif
