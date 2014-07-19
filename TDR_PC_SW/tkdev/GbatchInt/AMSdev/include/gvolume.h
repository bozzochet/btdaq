//  $Id: gvolume.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
//
// Aug 08, 1996. ak.  First try with Objectivity.
//                    Add set, get functions and one more constructor
// Last Edit: Aug 08, 1996. ak.
// 
#ifndef __AMSGVOLUME__
#define __AMSGVOLUME__
#include "node.h"
#include "typedefs.h"
#include <string.h>
#include "cern.h"
#include "amsdbc.h"
#include "point.h"
#include "gmat.h"
#include "geantnamespace.h"
#ifdef __G4AMS__
using namespace CLHEP;
#endif
class G4VSolid;
class AMSgvolume : public AMSNode {
 protected:
    class _amsrm{
     public:
      number _nrm[3][3];
      _amsrm();
      _amsrm(number nrm[3][3]);
      integer operator !=(const _amsrm & o) const{
        for(int i=0;i<3;i++){
         for(int j=0;j<3;j++){
           if(_nrm[i][j]!=o._nrm[i][j])return 1;
         }
        }
        return 0;
  
      }
      integer operator ==(const _amsrm & o) const{
        return !(*this!=o);
      }

    }; 
#ifdef __G4AMS__
    amsg4pv * _pg4v;
    G4LogicalVolume * _pg4l;
    amsg4rm * _pg4rm;
    AMSgvolume *_offspring;
    integer _smartless;
#endif
    AMSgtmed* _pgtmed;
   static uinteger _GlobalRotMatrixNo;
   static integer _LastRotMatrixOutOfOrder;
   void _init();
   AMSgvolume (const AMSgvolume&);   // do not want cc
   AMSgvolume &operator=(const AMSgvolume&); // do not want ass

   integer _matter;     //   ! geant tracking med
   integer _rotmno;     //   ! geant rot matrix no
   integer _posp;       //   ! Pos by posp or by pos 
   integer _gid;        //   ! geant id
   integer _npar;       //   ! geant number of par     
   integer _rel;        //   ! abs(0) or relative(1) geant coo/nrm
   geant*  _par;        //   ! geant volume par
   AMSPoint _coo;       //   ! geant volume relative coord
   AMSPoint _cooA;      //   ! geant volume abs coordinate
   _amsrm  *_nrm;       //   ! normales  with resp to mother (1st index 1st)
   _amsrm  *_nrmA;      //   ! norm absolute (1st index 1st)
   char    _gonly[5];   //   ! should be 'MANY' or 'ONLY'
   char*    _shape;     //   ! geant voulme shape
    
   integer _ContPos;
   void  _gl2loc(AMSgvolume * cur, AMSPoint & coo); 
   virtual ostream & print(ostream &)const;
   static _amsrm _UnitRM;
 static integer _Nlog;
 static integer _Nph;
 static integer _Nrm;
#ifdef __G4AMS__
 static amsg4rm* _pg4rmU;    // unity rot matrix
 G4VSolid* _MakeG4Volumes();
 static integer _Norp;
#endif
#ifdef __G4AMS__
  AMSgvolume():AMSNode(0),_npar(0),_par(0),_shape(0),_nrm(0),_nrmA(0),_pg4v(0),_pg4l(0),_pg4rm(0), _pgtmed(0),_offspring(0),_smartless(2){};
#else
  AMSgvolume():AMSNode(0),_npar(0),_par(0),_shape(0),_nrm(0),_nrmA(0), _pgtmed(0){};
#endif
  void _addboolean(AMSgvolume *p);
 public:
  ~AMSgvolume();
 void MakeG3Volumes();
    AMSgtmed* getpgtmed() const {return _pgtmed;}
#ifdef __G4AMS__
 void MakeG4Volumes();
//void ReadG4StepVolumes(char * fnam);

  AMSgvolume* offspring(){return _offspring;}
  integer & Smartless(){return _smartless;}
  void addboolean(char shape[], geant par[], integer npar,  geant coo[], number nrm[3][3],char ops);
  void removeboolean();
   amsg4pv * & pg4v()  {return _pg4v;}
  G4LogicalVolume * & pg4l()  {return _pg4l;}
  amsg4rm * & pg4rm() {return _pg4rm;}
  int VolumeHasSameG4AttributesAs(AMSgvolume *pvo );
#endif
static  integer & getNlv()  {return _Nlog;}
static  integer & getNpv()  {return _Nph;}
static  integer & getNrm()  {return _Nrm;}
  static void amsgeom();
  static uinteger & GlobalRotMatrixNo(){return _GlobalRotMatrixNo;}
  int VolumeHasSameRotationMatrixAs(AMSgvolume *pvo );
  int VolumeHasSameG3AttributesAs(AMSgvolume *pvo );
  int VolumeHasG3Attributes();
  AMSgvolume (const char matter[], integer rotmno,const char name[], 
           const char shape[] ,   geant par[] , integer npar, 
            geant coo[] ,  number nrm[][3] , const char gonly[] , 
           integer posp,integer gid, integer rel=0);
  integer getrotmatrixno() const{return _rotmno;}
  number getnrm(integer i ,integer j)const{return _nrm->_nrm[i][j];}
  number getnrmA(integer i ,integer j)const{return _nrmA->_nrm[i][j];}
  number getcoo(integer i) {return _coo[i];}
  number getcooA(integer i){return _cooA[i];}
  number getpar(integer i)const {return i>=0 && i<_npar ? _par[i]:0;}
  AMSPoint gl2loc(AMSPoint);  // conversion global->local
  AMSPoint loc2gl(AMSPoint);  // conversion global->local
  AMSgvolume * next(){return dynamic_cast<AMSgvolume*>(AMSNode::next());}
  AMSgvolume * prev(){return dynamic_cast<AMSgvolume*>(AMSNode::prev());}
  AMSgvolume * up(){return   dynamic_cast<AMSgvolume*>(AMSNode::up());}
  AMSgvolume * down(){return dynamic_cast<AMSgvolume*>(AMSNode::down());}
/*
  AMSgvolume * next(){return (AMSgvolume *)AMSNode::next();}
  AMSgvolume * prev(){return (AMSgvolume *)AMSNode::prev();}
  AMSgvolume * up(){return   (AMSgvolume *)AMSNode::up();}
  AMSgvolume * down(){return (AMSgvolume *)AMSNode::down();}
*/
  static integer debug;
  static geant dgeant;
//+
#ifdef __DB__
   friend class AMSgvolumeD;
#endif
void    setContPos(integer ContPos) {_ContPos = ContPos;}
integer getContPos()                {return _ContPos;}
   void getNumbers
  (integer* ibuff, geant* gbuff, AMSPoint* coo, number* nbuff0, number* nbuffi,
   char gonly[], char shape[])
{
  UCOPY( &_matter, ibuff, sizeof(integer)*5/4);
  UCOPY( &_par, gbuff, sizeof(geant)*6/4);
  UCOPY( &_coo, coo, sizeof(AMSPoint)*1/4);
  UCOPY( &_nrm, nbuff0, sizeof(number)*3*3/4);
  UCOPY( &_nrmA,nbuffi, sizeof(number)*3*3/4);
  if(_shape)strcpy(shape,_shape);
  if(_gonly)strcpy(gonly,_gonly);
}
void setNumbers
  (integer* ibuff, geant* gbuff, AMSPoint* coo, number* nbuff0, number* nbuffi,
   const char name[], const char gonly[], const char shape[])
{
  UCOPY(ibuff, &_matter, sizeof(integer)*5/4);
  UCOPY(gbuff, &_par, sizeof(geant)*6/4);
  UCOPY(coo, &_coo, sizeof(AMSPoint)*1/4);
  UCOPY(nbuff0, &_nrm, sizeof(number)*3*3/4);
  UCOPY(nbuffi, &_nrmA, sizeof(number)*3*3/4);
  if(shape)strcpy(_shape,shape);
  if(gonly)strcpy(_gonly,gonly);
  setname(name);
}
//-
};

#endif
