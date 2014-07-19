//  $Id: trddbc.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __TRDDBC__
#define __TRDDBC__
#include "typedefs.h"

#include <iostream.h>
#include <fstream.h>

namespace trdconst{
// counting always from 0 and from top to bottom
const uinteger maxo=13;   
const uinteger mtrdo=1;   
const uinteger maxbulk=6;
const uinteger maxspikes=9;
const uinteger maxpipes=10;
const uinteger maxlay=20;
const uinteger maxseg=5;
const uinteger maxhits=12;
const uinteger maxlad=18;
const uinteger maxco=2;
const uinteger maxhole=3;
const uinteger maxsidehole=8;
const uinteger maxtube=16;
const uinteger maxstrips=36;
const uinteger TRDROTMATRIXNO=9001;
}
using trdconst::mtrdo;
using trdconst::maxo;
using trdconst::TRDROTMATRIXNO;
class TRDDBc{

private:
 number _nrm[3][3];
 geant _coo[3];
 uinteger _status;
 uinteger _gid;

   // Media    
   static char* _OctagonMedia[maxo];
   static char* _BulkheadsMedia;
   static char* _XeRadSpikesMedia;
   static char* _PipesMedia;
   static char* _CutoutsMedia;
   static char* _WiresMedia;
   static char* _TubesMedia;
   static char* _ITubesMedia;
   static char* _RadiatorHoleMedia;
   static char* _TubesBoxMedia;
   static char* _SpacerMedia;
   static char* _StripsMedia;

   // Quantities
    static uinteger   _TRDOctagonNo;
    static uinteger   _OctagonNo;
    static uinteger   _PrimaryOctagonNo;
    static uinteger   _PrimaryOctagon[maxo];
    static uinteger   _NoTRDOctagons[mtrdo];
    static  uinteger  _BulkheadsNo[mtrdo];
    static  uinteger  _LayersNo[mtrdo];
    static  uinteger  _LaddersNo[mtrdo][trdconst::maxlay];
    static  uinteger  _CutoutsNo[mtrdo][trdconst::maxlay][trdconst::maxlad];
    static  uinteger  _CutoutsBH[mtrdo][trdconst::maxlay][trdconst::maxco];
    static uinteger   _TubesNo[mtrdo][trdconst::maxlay][trdconst::maxlad];
    static uinteger   _HolesNo[mtrdo][trdconst::maxlay][trdconst::maxlad];
    static uinteger   _SideHolesNo[mtrdo];
    static uinteger   _SideHolesPieceNo[mtrdo][trdconst::maxsidehole];
    static float      _SpikesPar[trdconst::maxspikes][4][6];  //x,y,z,rmi,rma,z/2 (cm)
    static geant      _PipesPar[trdconst::maxpipes][4][7];  //x,y,z,rmi,rma,z/2,dist/2 between two pipes (cm)
    static number      _PipesNRM[trdconst::maxpipes][4][3][3];  //
    static uinteger   _NumberBulkheads;
    static uinteger   _NumberTubes;
    static uinteger   _NumberHoles;
    static uinteger   _NumberSideHoles;
    static uinteger   _NumberSideHolePieces;
    static uinteger   _NumberLadders;
    static uinteger   _NumberCutouts;
    static const number  _WireDiameter;
    static const number  _TubeInnerDiameter;
    static const number  _TubeWallThickness;
    static const number  _TubeBoxThickness;
    static const number  _ChamberThickness;
    static const number  _StripsDim[3];
    static const number  _StripsCoo[2];
    static const number  _LadderThickness;
    static const number  _BulkheadWidth;
    static const number  _CutoutThickness;
    static const number  _CutoutWidth;
    static const number  _FirstLayerHeight;
    static const number  _LadderRefRadius;
    static const number  _WirePosition;
    static const number  _ManifoldThickness;
    static const number  _ManifoldLength;
    static const number  _EndPieceLength;
    static const number  _ManifoldWidth;
    static const number  _BulkheadGap;
    static const integer _LadderOrientation[mtrdo][trdconst::maxlay];    
    static const integer       _BHOrientation[trdconst::maxbulk];
    static const number  _SpacerDim[4]; 
    static const integer _NumberSpacers[trdconst::maxtube];

 //Sizes    
   static number    _OctagonDimensions[maxo][10]; 
   static number    _BulkheadsDimensions[mtrdo][trdconst::maxbulk][4];
   static number    _LaddersDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3];
   static number    _LaddersLength[trdconst::maxlay][trdconst::maxlad];
 static number       _LaddersLengthShort[trdconst::maxlay][trdconst::maxlad];
   static number    _CutoutsDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3];
   static number    _WiresDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3];    
   static number    _TubesDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3];    
   static number    _SpacerDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3][2];    
   static number    _TubesBoxDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][10];    
   static number    _RadiatorHoleDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][trdconst::maxhole][11];
   static number    _RadiatorSideHolePieceDimensions[mtrdo][trdconst::maxsidehole][trdconst::maxhole][11];

  // Positions & Orientations
  
   static TRDDBc * _HeadOctagonPos;
   static TRDDBc * _HeadBulkheadPos;
   static TRDDBc * _HeadCutoutPos;
   static TRDDBc * _HeadLadderPos;
   static TRDDBc * _HeadRadiatorHolePos;
   static TRDDBc * _HeadRadiatorSideHolePiecePos;
   static TRDDBc * _HeadTubeBoxPos;
   static TRDDBc * _HeadTubePos;

  // Debug
  static uinteger _check(uinteger oct, uinteger layer=0,
  uinteger ladder=0,uinteger tube=0);


  // Track Reconstruction

 static integer * _patconf[trdconst::maxlay];
 static integer * _patpoints;
 static integer * _patmiss[trdconst::maxlay];
 static integer   _patd[trdconst::maxlay]; 
 static integer * _patallow;
 static integer * _patallow2;
 static uinteger _Npat;


 static integer    _oriseg[trdconst::maxseg];
 static integer * _patconfS[trdconst::maxseg];
 static integer * _patpointsS;
 static integer * _patmissS[trdconst::maxseg];
 static integer   _patdS[trdconst::maxseg]; 
 static integer * _patallowS;
 static integer * _patallow2S;
 static uinteger _NpatS;
 static integer  _segconf[trdconst::maxseg][trdconst::maxhits];
 static integer  _segpoints[trdconst::maxseg];
 static uinteger _Nseg;
 static integer * _patconfH[trdconst::maxseg][trdconst::maxhits];
 static integer * _patpointsH[trdconst::maxseg];
 static integer * _patmissH[trdconst::maxseg][trdconst::maxhits];
 static integer   _patdH[trdconst::maxseg][trdconst::maxhits]; 
 static integer * _patallowH[trdconst::maxseg];
 static integer * _patallow2H[trdconst::maxseg];
 static uinteger _NlayH[trdconst::maxseg];
 static uinteger _NpatH[trdconst::maxseg];

public:
  static integer oriseg(uinteger lay){return _oriseg[lay];}
  static integer patmiss(uinteger pat, uinteger lay){return _patmiss[lay][pat];}
  static integer patconf(uinteger pat, uinteger lay){return _patconf[lay][pat];}
  static integer patd(uinteger lay){return _patd[lay];}
  static integer patallow(uinteger pat){return pat<_Npat?_patallow[pat]:-1;}
  static integer patpoints(uinteger pat){return pat<_Npat?_patpoints[pat]:-1;}
  static uinteger npat(){return _Npat;}
  static uinteger Cnk(uinteger n,uinteger k);

  static integer patmissS(uinteger pat, uinteger lay){return _patmissS[lay][pat];}
  static integer patconfS(uinteger pat, uinteger lay){return _patconfS[lay][pat];}
  static integer patdS(uinteger lay){return _patdS[lay];}
  static integer patallowS(uinteger pat){return pat<_NpatS?_patallowS[pat]:-1;}
  static integer patallow2S(uinteger pat){return pat<_NpatS?_patallow2S[pat]:-1;}
  static integer patpointsS(uinteger pat){return pat<_NpatS?_patpointsS[pat]:-1;}
  static uinteger npatS(){return _NpatS;}

  static integer segconf(uinteger seg, uinteger lay){return _segconf[seg][lay];}
  static integer segpoints(uinteger seg){return seg<_Nseg?_segpoints[seg]:-1;}

  static uinteger nlayS(){return _Nseg;}

  static integer patmissH(uinteger pat, uinteger lay,uinteger seg){return _patmissH[seg][lay][pat];}
  static integer patconfH(uinteger pat, uinteger lay,uinteger seg){return segconf(seg,_patconfH[seg][lay][pat]-1);}
  static integer patdH(uinteger lay,uinteger seg){return _patdH[seg][lay];}
  static integer npatH(uinteger iseg){return iseg<trdconst::maxseg?_NpatH[iseg]:0;}
  static uinteger patallowH(uinteger pat,uinteger seg){return pat<npatH(seg)?_patallowH[seg][pat]:0;}
  static uinteger patallow2H(uinteger pat,uinteger seg){return pat<npatH(seg)?_patallow2H[seg][pat]:0;}
  static uinteger patpointsH(uinteger pat,uinteger seg){return pat<npatH(seg)?_patpointsH[seg][pat]:0;}
  static uinteger nlayH(uinteger iseg){return iseg<trdconst::maxseg?_NlayH[iseg]:0;}

  TRDDBc(): _status(0){}
   static void init();
   static void InitPattern();
   static void read();
   static void write();
   static char* CodeLad(uinteger gid); 

   static float getSpikesPar(uinteger spike, uinteger quad,uinteger par){
    return par<6 && spike<trdconst::maxspikes && quad<4 ? _SpikesPar[spike][quad][par]:0;
   }

   static geant getPipesPar(uinteger pipe, uinteger quad, uinteger par){
    return par<7 && quad<4 && pipe<trdconst::maxpipes ? _PipesPar[pipe][quad][par]:0;
   }

   static void getPipesNRM(uinteger pipe,uinteger quad, number nrm[3][3]){
     for (int i=0;i<3;i++){
       for (int j=0;j<3;j++)nrm[i][j]=_PipesNRM[pipe][quad][i][j];
     }
   }


   //  getnumbers for elements   
   static uinteger getnumHole(uinteger hole,uinteger ladder, uinteger layer, uinteger oct=1);
   static uinteger getnumSideHolePiece(uinteger hole, uinteger sidehole,uinteger oct=1);
   static uinteger getnumSideHole(uinteger sidehole,uinteger oct=1);
   static uinteger getnumTube(uinteger tube,uinteger ladder, uinteger layer, uinteger oct=1);
   static uinteger getnumBulkhead(uinteger bulkhead, uinteger oct=1);
   static uinteger getnumLadder(uinteger ladder, uinteger layer, uinteger oct=1);
   static uinteger getnumCutout(uinteger cutout, uinteger ladder,uinteger layer, uinteger oct=1);


   static uinteger getnumOctagon(uinteger oct);


   // GetSet Structures

 static void SetOctagon(uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger &gid);
 static void GetOctagon(uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);

 static void SetBulkhead(uinteger bulkhead, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetBulkhead(uinteger bulkhead, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);

 static void SetLadder(uinteger ladder, uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetLadder(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);

 static void SetCutout(uinteger cutout, uinteger ladder, 
             uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetCutout(uinteger cutout, uinteger ladder, 
                          uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);


 static void SetRadiatorHole(uinteger hole,uinteger ladder, uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetRadiatorHole(uinteger hole,uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);

 static void SetRadiatorSideHolePiece(uinteger hole,uinteger shole,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetRadiatorSideHolePiece(uinteger hole, uinteger shole,uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);



 static void SetTubeBox(uinteger ladder, uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetTubeBox(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);


 static void SetTube(uinteger tube, uinteger ladder, uinteger layer, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3],uinteger gid);
 static void GetTube(uinteger tube, uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid);
   static uinteger GetStatusTube(uinteger tube, uinteger ladder, uinteger layer, uinteger oct){
      return _HeadTubePos[getnumTube(tube,layer,ladder,oct)]._status; }


   // Public function to get Quantities
   static uinteger   TRDOctagonNo(){return _TRDOctagonNo;}
   static uinteger   PrimaryOctagonNo(){return _PrimaryOctagonNo;}
   static integer   GetPrimaryOctagon(uinteger oct){return oct<OctagonNo()?integer(_PrimaryOctagon[oct]):-1;}
   static uinteger   NoTRDOctagons(uinteger oct){return oct<TRDOctagonNo()?_NoTRDOctagons[oct]:0;}
   static uinteger   OctagonNo(){return _OctagonNo;}
   static  uinteger  BulkheadsNo(uinteger toct){return toct<TRDOctagonNo()?_BulkheadsNo[toct]:0;}
   static  uinteger  LayersNo(uinteger toct){return toct<TRDOctagonNo()?_LayersNo[toct]:0;}
   static  uinteger  LaddersNo(uinteger toct, uinteger lay){return lay<LayersNo(toct)?_LaddersNo[toct][lay]:0;}
   static  uinteger  CutoutsNo(uinteger toct, uinteger lay, uinteger lad){return lay<LayersNo(toct)?_CutoutsNo[toct][lay][lad]:0;}
   static  uinteger  CutoutsBH(uinteger toct, uinteger lay,uinteger bh){return lay<LayersNo(toct)?_CutoutsBH[toct][lay][bh]:0;}
   static uinteger   TubesNo(uinteger toct, uinteger lay, uinteger lad){return lad<LaddersNo(toct,lay)?_TubesNo[toct][lay][lad]:0;}
   static uinteger   HolesNo(uinteger toct, uinteger lay, uinteger lad){return lad<LaddersNo(toct,lay)?_HolesNo[toct][lay][lad]:0;}
   static uinteger   SideHolesNo(uinteger toct){return toct<TRDOctagonNo()?_SideHolesNo[toct]:0;}
   static uinteger   SideHolesPieceNo(uinteger toct,uinteger sidehole){return sidehole<SideHolesNo(toct)?_SideHolesPieceNo[toct][sidehole]:0;}
   

    static  number  WireDiameter(){return _WireDiameter;}
    static  number  TubeInnerDiameter(){return _TubeInnerDiameter;}
    static  number  TubeWallThickness(){return _TubeWallThickness;}
    static  number  TubeBoxThickness(){return _TubeBoxThickness;}
    static  number  ChamberThickness(){return _ChamberThickness;}
    static  number  StripsDim(uinteger n){return n<sizeof(_StripsDim)/sizeof(_StripsDim[0])?_StripsDim[n]:0;}
    static  number  StripsCoo(uinteger n){return n<sizeof(_StripsCoo)/sizeof(_StripsCoo[0])?_StripsCoo[n]:0;}
    static  number  LadderThickness(){return _LadderThickness;}
    static  number  BulkheadWidth(){return _BulkheadWidth;}
    static  number  CutoutThickness(){return _CutoutThickness;}
    static  number  CutoutWidth(){return _CutoutWidth;}
    static  number  FirstLayerHeight(){return _FirstLayerHeight;}
    static  number  WirePosition(){return _WirePosition;}
    static  number  ManifoldThickness(){return _ManifoldThickness;}
    static  number  LadderRefRadius(){return _LadderRefRadius;}
    static  number  ManifoldLength(){return _ManifoldLength;}
    static  number  ManifoldWidth(){return _ManifoldWidth;}
    static  number  BulkheadGap(){return _BulkheadGap;}
    static  integer LadderOrientation(uinteger oct, uinteger lay){return lay<LayersNo(oct)?_LadderOrientation[oct][lay]:0;}     
    static  integer LadderShift(uinteger oct, uinteger lay,uinteger lad){return lad%2?-1:1;}
    static number SpacerWidth(){return _SpacerDim[0];}
    static number SpacerDim(int n){return n<sizeof(_SpacerDim)/sizeof(_SpacerDim[0])?_SpacerDim[n]:-1;}
    static  integer NumberSpacers(uinteger tube){return tube<trdconst::maxtube?_NumberSpacers[tube]:0;}     


 //Sizes    
   static number&    OctagonDimensions(uinteger oct, uinteger index); 
   static number&    BulkheadsDimensions(uinteger toct, uinteger bulk, uinteger index);
   static number&    LaddersDimensions(uinteger toct, uinteger lay, uinteger lad, uinteger index);
   static number&    CutoutsDimensions(uinteger toct, uinteger lay, uinteger lad, uinteger index);
   static number&    WiresDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index);    
   static number&    TubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index);    
   static number&    SpacerDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index, uinteger spacerpart);    
   static number    ITubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index);    
   static number&    TubesBoxDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index); 
   static number&    RadiatorHoleDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger hole, uinteger index);
   static number&    RadiatorSideHolePieceDimensions(uinteger toct,uinteger shole,uinteger hole, uinteger index);

 //Media

   static char* OctagonMedia(uinteger oct){return oct<maxo?_OctagonMedia[oct]:0;}
   static char* BulkheadsMedia(){return _BulkheadsMedia;}
   static char* XeRadSpikesMedia(){return _XeRadSpikesMedia;}
   static char* PipesMedia(){return _PipesMedia;}
   static char* WiresMedia(){return _WiresMedia;}
   static char* TubesMedia(){return _TubesMedia;}
   static char* ITubesMedia(){return _ITubesMedia;}
   static char* RadiatorHoleMedia(){return _RadiatorHoleMedia;}
   static char* TubesBoxMedia(){return _TubesBoxMedia;}
   static char* SpacerMedia(){return _SpacerMedia;}
   static char* StripsMedia(){return _StripsMedia;}
   static char* LaddersMedia(){return _OctagonMedia[_NoTRDOctagons[0]];}
   static char* CutoutsMedia(){return _CutoutsMedia;}
   static uinteger nlay(){return trdconst::maxlay;}


  static bool ThereIsSpacerAfterTube(uinteger tube){
   return tube<trdconst::maxtube-1?(_NumberSpacers[tube+1]-_NumberSpacers[tube]>0):false;
  }


  
};


class TRDDBcI{
public:
TRDDBcI();
~TRDDBcI();
private:
 static integer _Count;
};
static TRDDBcI trdI;



#endif

