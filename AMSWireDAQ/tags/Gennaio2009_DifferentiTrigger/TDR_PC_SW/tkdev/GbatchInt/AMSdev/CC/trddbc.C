//  $Id: trddbc.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "trddbc.h"
#include "amsdbc.h"
#include <math.h>
#include "tkdbc.h"
#include "commons.h"
using namespace trdconst;
char * TRDDBc::_OctagonMedia[maxo]={"TRDCarbonFiber", "TRDCarbonFiber",
"TRDCarbonFiber","TRDCarbonFiber","TRDCarbonFiber","VACUUM","TRDHC","TRDHC",
"TRDHCWall","TRDRadiator","TRDALUMINIUM","TRDRohaCell","TRDALUMINIUM"};
char * TRDDBc::_BulkheadsMedia="TRDCarbonFiber";
char * TRDDBc::_XeRadSpikesMedia="TRDFiberGlass"; 
char * TRDDBc::_PipesMedia="TRDALUMINIUM"; 
char * TRDDBc::_CutoutsMedia="VACUUM";
char * TRDDBc::_TubesMedia="TRDCapton";
char * TRDDBc::_ITubesMedia="TRDGas";
char * TRDDBc::_WiresMedia="TUNGSTENTRD";
char * TRDDBc::_RadiatorHoleMedia="VACUUM";  
char * TRDDBc::_TubesBoxMedia="TRDFoam";
char * TRDDBc::_SpacerMedia="TRDCarbonFiber";
char * TRDDBc::_StripsMedia="TRDCarbonFiber";

//
//      NB
//      some spikes & pipes parameters are correlated with xe rad octagons
//      ones, so change them in case of changing the latter.
//
float TRDDBc::_SpikesPar[trdconst::maxspikes][4][6]={
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          4.,   -102.,-1.19/2,0.,0.125,1.05,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          4,    -62., -1.19/2,0.,0.125,1.05,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          8.33, -6.15,-1.19/2,0.,0.125,1.05,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          62.,  -4.,  -1.19/2,0.,0.125,1.05,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          102., -4.,  -1.19/2,0.,0.125,1.05,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          42., -42.,  -1.19/2,0.,0.125,1.05,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          42., -82.,  -1.19/2,0.,0.125,1.05,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          82., -42.,  -1.19/2,0.,0.125,1.05,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          0,0,0,0,0,0,
                          68.5,-86.,  -1.19/2,0.,0.125,1.05
                                                       };

float TRDDBc::_PipesPar[trdconst::maxpipes][4][7]={
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 115./10., -65.750+55./20, (11.9-1.8-0.3)/20., 0.1, 0.15, 1030./20., 13./20.,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 115./10.+15./10.+885./20., -115./10.+15./10., (11.9-1.8-0.3)/20., 0.1, 0.15, 885./20., 13./20.,

                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 0, 0, (11.9-1.8-0.3)/20., 0.1, 0.15, 0, 13./20.,

                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 930./10.-600./20., -330./10., (11.9-1.8-0.3)/20., 0.1, 0.15, 600./20., 13./20.,

                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 930./10-600./10.-15./10, -330./10-15./10-600./20., (11.9-1.8-0.3)/20., 0.1, 0.15, 600./20., 13./20.,

                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 0, 0, (11.9-1.8-0.3)/20., 0.1, 0.15, 300./20., 13./20.,

                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 715./10-250./20-15./10., -540./10-300./20.-15./10., (11.9-1.8-0.3)/20., 0.1, 0.15, 300./20, 13./20.,

                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 715./10., -540./10., (11.9-1.8-0.3)/20., 0.1, 0.15, 250./20., 13./20.,

                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 715./10+250./20+15./10., -540./10-220./20-15./10., (11.9-1.8-0.3)/20., 0.1, 0.15, 220./20., 13./20.,

                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,

                 0, 0, (11.9-1.8-0.3)/20., 0.1, 0.15, 0, 13./20.,
                            };
number TRDDBc::_PipesNRM[trdconst::maxpipes][4][3][3]={
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                 1, 0, 0,
                 0, 0, 1,
                 0,-1, 0,
 
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                  0, 0,1,
                 -1,0,0,
                  0,-1,0,

                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,


                 0,0,0,
                 0,0,0,
                 0,0,0,

                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                  0, 0,1,
                 -1,0,0,
                  0,-1,0,

                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                 1, 0, 0,
                 0, 0, 1,
                 0,-1, 0,

                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                 0,0,0,
                 0,0,0,
                 0,0,0,

                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                 1, 0, 0,
                 0, 0, 1,
                 0,-1, 0,

                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                  0, 0,1,
                 -1,0,0,
                  0,-1,0,

                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                 1, 0, 0,
                 0, 0, 1,
                 0,-1, 0,

                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,
                 0,0,0,

                 0,0,0,
                 0,0,0,
                 0,0,0,


                            };
uinteger TRDDBc::_NoTRDOctagons[mtrdo]={9};
uinteger TRDDBc::_PrimaryOctagon[maxo]={0,1,2,3,4,5,3,4,1,1,5,5,5};
uinteger TRDDBc::_PrimaryOctagonNo=6;
uinteger TRDDBc::_TRDOctagonNo=1;
uinteger TRDDBc::_OctagonNo=maxo;
uinteger TRDDBc::_BulkheadsNo[mtrdo]={maxbulk};
uinteger TRDDBc::_SideHolesNo[mtrdo]={maxsidehole};
uinteger TRDDBc::_LayersNo[mtrdo]={maxlay};
uinteger TRDDBc::_LaddersNo[mtrdo][maxlay]={14,14,14,14,16,16,16,16,16,16,16,16,18,18,18,18,18,18,18,18};

uinteger TRDDBc::_CutoutsNo[mtrdo][maxlay][maxlad];

// Bulkhead that the cutout belongs to
uinteger TRDDBc::_CutoutsBH[mtrdo][maxlay][maxco]={4,5,4,5,4,5,4,5,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,2,3,2,3,2,3,2,3};

uinteger TRDDBc::_TubesNo[mtrdo][maxlay][maxlad];
uinteger TRDDBc::_HolesNo[mtrdo][maxlay][maxlad];
uinteger TRDDBc::_SideHolesPieceNo[mtrdo][maxsidehole];


uinteger   TRDDBc::_NumberTubes=0;
uinteger   TRDDBc::_NumberHoles=0;
uinteger   TRDDBc::_NumberSideHoles=0;
uinteger   TRDDBc::_NumberSideHolePieces=0;
uinteger   TRDDBc::_NumberLadders=0;
uinteger   TRDDBc::_NumberCutouts=0;
uinteger   TRDDBc::_NumberBulkheads=0;


const number  TRDDBc::_WireDiameter=0.03;
const number  TRDDBc::_TubeWallThickness=72e-4;
const number  TRDDBc::_TubeInnerDiameter=0.6;
const number  TRDDBc::_TubeBoxThickness=(0.62-TRDDBc::_TubeInnerDiameter-2*TRDDBc::_TubeWallThickness)/2.;
const number  TRDDBc::_ChamberThickness=0.04;
const number  TRDDBc::_StripsDim[3]={9.6,0.03,0.6};
const number  TRDDBc::_StripsCoo[2]={10.69,10.};
const number  TRDDBc::_BulkheadWidth=0.3;
const number  TRDDBc::_LadderThickness=2.9;
const number  TRDDBc::_CutoutThickness=0.88;
const number  TRDDBc::_CutoutWidth=10.2;
const number  TRDDBc::_LadderRefRadius = 154.1/2.; // Ref radius for ladder calc
const number  TRDDBc::_FirstLayerHeight = 1.7-0.05; // Distance of first layer-line from bottom (center of bottom main octagon skin)
const number TRDDBc::_WirePosition = 0.725-0.05; // Distance of wire below layer-line (VC added 0.05 to compensate firstlayer -0.05, needs check)
const number TRDDBc::_ManifoldThickness = 0.70; // Z-height of manifold
const number TRDDBc::_ManifoldLength = 1.15; // Length of manifold along wire
const number TRDDBc::_ManifoldWidth = 10.1; // Width of manifold 
const number TRDDBc::_BulkheadGap = 0.78; // Gap between ladders at bulkhead
const number TRDDBc::_EndPieceLength = 1.51; // ladder - tube length difference

const integer TRDDBc::_LadderOrientation[mtrdo][maxlay]={0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0};   // 0 -x 1 -y    
const integer TRDDBc::_BHOrientation[maxbulk]={0,0,1,1,1,1};   // 0 -x 1 -y    

const number TRDDBc::_SpacerDim[4] = {0.03,0.4,0.09,0.1};
// Spacers preceding
const integer TRDDBc::_NumberSpacers[maxtube]={0,1,1,1,2,2,2,3,3,4,4,4,5,5,5,6};
number TRDDBc::_OctagonDimensions[maxo][10]; 
number TRDDBc::_BulkheadsDimensions[mtrdo][maxbulk][4];
number TRDDBc::_CutoutsDimensions[mtrdo][maxlay][maxlad][3];
number TRDDBc::_LaddersDimensions[mtrdo][maxlay][maxlad][3];
number TRDDBc::_LaddersLengthShort[maxlay][maxlad];
number TRDDBc::_LaddersLength[maxlay][maxlad]={
 92.63,111.13,133.03,151.53,156.75,155.55,156.75,155.55,156.75,155.55,153.23,131.33,112.83, 90.93,  0.00,  0.00,  0.00,  0.00,
 96.03,114.53,136.43,154.93,159.15,157.95,159.15,157.95,159.15,157.95,156.63,134.73,116.23, 94.33,  0.00,  0.00,  0.00,  0.00,
 99.43,117.93,139.83,158.33,161.56,160.36,161.56,160.36,161.56,160.36,160.03,138.13,119.63, 97.73,  0.00,  0.00,  0.00,  0.00,
102.82,121.33,143.22,161.73,163.96,162.76,163.96,162.76,163.96,162.76,163.42,141.53,123.02,101.13,  0.00,  0.00,  0.00,  0.00,
 85.88,107.78,126.28,148.18,165.16,166.36,165.16,166.36,165.16,166.36,165.16,166.36,146.48,127.98,106.08, 87.58,  0.00,  0.00,
 89.28,111.18,129.68,151.58,167.56,168.76,167.56,168.76,167.56,168.76,167.56,168.76,149.88,131.38,109.48, 90.98,  0.00,  0.00,
 92.68,114.58,133.08,154.98,169.97,171.17,169.97,171.17,169.97,171.17,169.97,171.17,153.28,134.78,112.88, 94.38,  0.00,  0.00,
 96.08,117.97,136.48,158.37,172.37,173.57,172.37,173.57,172.37,173.57,172.37,173.57,156.68,138.17,116.28, 97.77,  0.00,  0.00,
 99.47,121.37,139.87,161.77,174.77,175.97,174.77,175.97,174.77,175.97,174.77,175.97,160.07,141.57,119.67,101.17,  0.00,  0.00,
102.87,124.77,143.27,165.17,177.17,178.37,177.17,178.37,177.17,178.37,177.17,178.37,163.47,144.97,123.07,104.57,  0.00,  0.00,
106.27,128.17,146.67,168.57,179.58,180.78,179.58,180.78,179.58,180.78,179.58,180.78,166.87,148.37,126.47,107.97,  0.00,  0.00,
109.67,131.56,150.07,171.96,181.98,183.18,181.98,183.18,181.98,183.18,181.98,183.18,170.27,151.76,129.87,111.36,  0.00,  0.00,
 94.56,113.06,134.96,153.46,175.36,184.38,185.58,184.38,185.58,184.38,185.58,184.38,185.58,173.66,155.16,133.26,114.76, 92.86,
 97.96,116.46,138.36,156.86,178.76,186.78,187.98,186.78,187.98,186.78,187.98,186.78,187.98,177.06,158.56,136.66,118.16, 96.26,
101.36,119.86,141.76,160.26,182.16,189.19,190.39,189.19,190.39,189.19,190.39,189.19,190.39,180.46,161.96,140.06,121.56, 99.66,
104.75,123.26,145.15,163.66,185.55,191.59,192.79,191.59,192.79,191.59,192.79,191.59,192.79,183.86,165.35,143.46,124.95,103.06,
106.59,125.09,146.99,165.49,187.39,193.99,195.19,193.99,195.19,193.99,195.19,193.99,195.19,185.69,167.19,145.29,126.79,104.89,
109.99,128.49,150.39,168.89,190.79,196.39,197.59,196.39,197.59,196.39,197.59,196.39,197.59,189.09,170.59,148.69,130.19,108.29,
113.39,131.89,153.79,172.29,194.19,198.80,200.00,198.80,200.00,198.80,200.00,198.80,200.00,192.49,173.99,152.09,133.59,111.69,
116.78,135.29,157.18,175.69,197.58,201.20,202.40,201.20,202.40,201.20,202.40,201.20,202.40,195.89,177.38,155.49,136.98,115.09
};
number TRDDBc::_TubesDimensions[mtrdo][maxlay][maxlad][3];    
number TRDDBc::_WiresDimensions[mtrdo][maxlay][maxlad][3];    
number TRDDBc::_SpacerDimensions[mtrdo][trdconst::maxlay][trdconst::maxlad][3][2];    
number TRDDBc::_TubesBoxDimensions[mtrdo][maxlay][maxlad][10];    
number TRDDBc::_RadiatorHoleDimensions[mtrdo][maxlay][maxlad][maxhole][11];
number TRDDBc::_RadiatorSideHolePieceDimensions[mtrdo][maxsidehole][maxhole][11];

  // Positions & Orientations
  
TRDDBc* TRDDBc::_HeadOctagonPos=0;
TRDDBc* TRDDBc::_HeadBulkheadPos=0;
TRDDBc* TRDDBc::_HeadCutoutPos=0;
TRDDBc* TRDDBc::_HeadLadderPos=0;
TRDDBc* TRDDBc::_HeadRadiatorHolePos=0;
TRDDBc* TRDDBc::_HeadRadiatorSideHolePiecePos=0;
TRDDBc* TRDDBc::_HeadTubeBoxPos=0;
TRDDBc* TRDDBc::_HeadTubePos=0;

void TRDDBc::init(){

   // Quantities
    int i,j,k,l,b;
    for(i=0;i<TRDOctagonNo();i++){
     for(j=0;j<LayersNo(i);j++){
      for(k=0;k<LaddersNo(i,j);k++){
       _TubesNo[i][j][k]=maxtube;
      }
     }
    }

    for(i=0;i<TRDOctagonNo();i++){
     for(j=0;j<LayersNo(i);j++){
      for(k=0;k<LaddersNo(i,j);k++){
       _HolesNo[i][j][k]=maxhole;
      }
     }
    }

    for(i=0;i<TRDOctagonNo();i++){
     for(j=0;j<SideHolesNo(i);j++){
       _SideHolesPieceNo[i][j]=maxhole;
     }
    }


    for(i=0;i<TRDOctagonNo();i++){
     for(j=0;j<LayersNo(i);j++){
      for(k=0;k<LaddersNo(i,j);k++){
       _CutoutsNo[i][j][k]=2;
      }
     }
    }


  // calculate tubesno #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++){
//           cout <<_NumberTubes<<" "<<i<<" "<<j<<" "<<k<<" "<<l<<endl;
           _NumberTubes++;
          }
         }
        }
       }

  // calculate holesno #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<HolesNo(i,j,k);l++){
//           cout <<_NumberHoles<<" "<<i<<" "<<j<<" "<<k<<" "<<l<<endl;
           _NumberHoles++;
          }
         }
        }
       }


  // calculate sideholesno #
       for ( i=0;i<TRDOctagonNo();i++){
          for(l=0;l<SideHolesNo(i);l++){
           _NumberSideHoles++;
          }
       }


  // calculate sideholespieceno #
       for ( i=0;i<TRDOctagonNo();i++){
         for(j=0;j<SideHolesNo(i);j++){
          for(k=0;k<SideHolesPieceNo(i,j);k++){
           _NumberSideHolePieces++;
          }
        }
       }

       cout <<"TRDDBcI-I-Total of " <<_NumberTubes<< "  tubes initialized."<<endl;
  // calculate ladder #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++)_NumberLadders++;
        }
       }
       cout <<"TRDDBcI-I-Total of " <<_NumberLadders<< "  ladders initialized."<<endl;
  // calculate bulkhead #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<BulkheadsNo(i);j++)_NumberBulkheads++;
       }
       cout <<"TRDDBcI-I-Total of " <<_NumberBulkheads<< "  bulkheads initialized."<<endl;

  // calculate cutout #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for ( b=0;b<CutoutsNo(i,j,k);b++)_NumberCutouts++;
	 }
        }
       }
       cout <<"TRDDBcI-I-Total of " <<_NumberCutouts<< "  bulkhead cutouts initialized."<<endl;


       _HeadLadderPos=new TRDDBc[_NumberLadders];
       _HeadBulkheadPos=new TRDDBc[_NumberBulkheads];
       _HeadCutoutPos=new TRDDBc[_NumberCutouts];
       _HeadRadiatorHolePos=new TRDDBc[_NumberHoles];
       _HeadRadiatorSideHolePiecePos=new TRDDBc[_NumberSideHolePieces];
       _HeadTubeBoxPos=new TRDDBc[_NumberLadders];
       _HeadTubePos=new TRDDBc[_NumberTubes];
       _HeadOctagonPos= new TRDDBc[OctagonNo()]; 



   // HereStartsTheJob;

      number unitnrm[3][3]={1,0,0,0,1,0,0,0,1};

//    OctagonDimensions
// Drawings show mm so divide by 10.  Divide by 20 when the full width
// (or diameter as opposed to radius)
// is given in the drawing, since the
      // dimensions to be fed into Geant are centered on zero

   // Primary octagons

// Top octagon carbon fiber ring

      OctagonDimensions(0,4)=-6./20.;   // z coord

   // Bottom edge
      OctagonDimensions(0,5)=2052./20.;         // rmin
      OctagonDimensions(0,6)=2210./20.;  // rmax

   // Top edge
      OctagonDimensions(0,8)=2052./20;         // rmin
      OctagonDimensions(0,9)=2210./20.;  // rmax

// Main octagon

      number ang=22.5/180.*AMSDBc::pi;

     // z coord... drawing gives height dimension including
     //   top and bottom carbon fiber ring widths,
     // so subtract the widths of these
         OctagonDimensions(1,4)=-621./20.
                +6./20. + 6./20.;

      // Bottom edge
      OctagonDimensions(1,5)=0;                // rmin

      // On drawing, 1600 is intersection with middle of bottom skin
      
      OctagonDimensions(1,6)=1600./20.+1./10/cos(ang)-2./10*tan(ang);   // rmax

      // Top edge
      OctagonDimensions(1,8)=0;    // rmin
      OctagonDimensions(1,9)=OctagonDimensions(1,6)+2*fabs(OctagonDimensions(1,4))*tan(ang);   // rmax
 
//  Bottom octagon carbon fiber ring

      OctagonDimensions(2,4)=-6./20.;    // z coord 

     // Bottom edge
      OctagonDimensions(2,5)=1470./20.;         // rmin
      OctagonDimensions(2,6)=1590./20.;  // rmax

     // Top edge
      OctagonDimensions(2,8)=1470./20.;         // rmin
      OctagonDimensions(2,9)=1590./20;  // rmax

// Top honeycomb cover

      OctagonDimensions(3,4)=-94./20.;   // z coord

   // Bottom edge
      OctagonDimensions(3,5)=0;         // rmin
      OctagonDimensions(3,6)=OctagonDimensions(0,6); // rmax
      // Same as top of carbon fiber ring       

   // Top edge
      OctagonDimensions(3,8)=0;         // rmin
      OctagonDimensions(3,9)=OctagonDimensions(0,9);  // rmax

  // Octagon to be filled with bottom honeycomb
      // This actually has notches in it.

      OctagonDimensions(4,4)=-41./20.;   // z coord

   // Bottom edge

      OctagonDimensions(4,5)=0;         // rmin
      OctagonDimensions(4,6)=1550./20.; // rmax

      // Top edge
      OctagonDimensions(4,8)=0;         // rmin
      OctagonDimensions(4,9)=1550./20.;  // rmax

// Ze Radiator  Octagon

      
      OctagonDimensions(5,4)=-(21+11.9)/20.;   // z coord

   // Bottom edge

      OctagonDimensions(5,5)=0;         // rmin
//      OctagonDimensions(5,5)=1110/10.;         // rmin
      OctagonDimensions(5,6)=1210./10.; // rmax

      // Top edge
      OctagonDimensions(5,8)=0;         // rmin
//      OctagonDimensions(5,8)=1110/10.;         // rmin
      OctagonDimensions(5,9)=1210./10.;  // rmax

     


  // Now the non-primary octagons

  //   Inner honeycomb part on top, inside octagon 3 (leaving
  //            1 mm skin of carbon fiber on top and bottom)

      OctagonDimensions(6,4)=OctagonDimensions(3,4)+2./20.;  //z coord

      // Bottom edge
      OctagonDimensions(6,5)=0;   // rmin
      OctagonDimensions(6,6)=OctagonDimensions(3,6);  // rmax

      // Top edge
      OctagonDimensions(6,8)=0;  // rmin
      OctagonDimensions(6,9)=OctagonDimensions(3,9); // rmax

  //   Inner honeycomb part on bottom, inside octagon 4 (leaving
  //            0.5 mm skin of carbon fiber on top and bottom)
      
      OctagonDimensions(7,4)=OctagonDimensions(4,4)+1./20.;  //z coord

      // Bottom edge
      OctagonDimensions(7,5)=0;   // rmin
      OctagonDimensions(7,6)=OctagonDimensions(4,6);  // rmax
                    
      // Top edge
      OctagonDimensions(7,8)=0;  // rmin
      OctagonDimensions(7,9)=OctagonDimensions(4,9); // rmax

  //  Honeycomb inside sides of TRD structure

      OctagonDimensions(8,4)=OctagonDimensions(1,4)+ 4./20.;  // z coord

      // Bottom edge
      OctagonDimensions(8,5)= OctagonDimensions(1,6)-30./10./cos(ang)
                                   +2./10./cos(ang);
      OctagonDimensions(8,6)=OctagonDimensions(1,6)-2./10./cos(ang);  // rmax

      // Top edge
      OctagonDimensions(8,8)=OctagonDimensions(1,9)-30./10./cos(ang)
                      +2./10./cos(ang);                             // rmin
      OctagonDimensions(8,9)=OctagonDimensions(1,9)-2./10./cos(ang); // rmax


  // Inner part... this is where ladders and tubes go

      OctagonDimensions(9,4)=OctagonDimensions(1,4)+8./20.;  // z coord

      // Bottom edge
      OctagonDimensions(9,5)=0;
      OctagonDimensions(9,6)=OctagonDimensions(8,5)-2./10./cos(ang);
      // Top edge
      OctagonDimensions(9,8)=0;
      OctagonDimensions(9,9)=OctagonDimensions(8,8)-2./10./cos(ang);
//      number AccuracyLost=1541./20-OctagonDimensions(9,6);
//      OctagonDimensions(9,6)+=AccuracyLost;
//      OctagonDimensions(9,9)+=AccuracyLost;

//  Top Alum cover of Ze radiator
      
      OctagonDimensions(10,4)=-1.8/20.;  // z coord

      // Bottom edge
      OctagonDimensions(10,5)=0;
      OctagonDimensions(10,6)=OctagonDimensions(5,6);

      // Top edge
      OctagonDimensions(10,8)=0;
      OctagonDimensions(10,9)=OctagonDimensions(5,9);

//  Rohacell of Ze radiator
      
      OctagonDimensions(11,4)=-(11.9-1.8-0.3)/20.;  // z coord

      // Bottom edge
      OctagonDimensions(11,5)=0;
      OctagonDimensions(11,6)=OctagonDimensions(5,6);

      // Top edge
      OctagonDimensions(11,8)=0;
      OctagonDimensions(11,9)=OctagonDimensions(5,9);

//  Bottom Alum cover of Ze radiator
      
      OctagonDimensions(12,4)=-0.3/20.;  // z coord

      // Bottom edge
      OctagonDimensions(12,5)=0;
      OctagonDimensions(12,6)=OctagonDimensions(5,6);

      // Top edge
      OctagonDimensions(12,8)=0;
      OctagonDimensions(12,9)=OctagonDimensions(5,9);


      
      for(i=0;i<OctagonNo();i++){
       OctagonDimensions(i,0)=0-360/16.;
       OctagonDimensions(i,1)=360;
       OctagonDimensions(i,2)=8;
       OctagonDimensions(i,3)=2;
       OctagonDimensions(i,7)=-OctagonDimensions(i,4);
      }    

//    Position the primary octagons

      uinteger status=1;
      uinteger gid=0;
      geant coo[3];
      for(i=0;i<PrimaryOctagonNo();i++){
       for(j=0;j<3;j++){
        coo[j]=0;
       }

// For all these, subtract half-width from position of top of volume

     // 835. is position of bottom of bottom carbon fiber ring

       switch(i){
       case 5:    // Ze Radiator octagon  (up from hc)
          coo[2]= (1476+94.+6.+35+11.9)/10.+OctagonDimensions(i,4);
          break;
       case 4:    // bottom honeycomb... starts 1.5 mm below carbon
	 //  fiber ring
          coo[2]= (835.-1.5)/10.+OctagonDimensions(i,4);
          break;

       case 3:   // top honeycomb...need to check this
          coo[2]= (1476+94.+6.)/10.+OctagonDimensions(i,4);
          break;

       case 2: // bottom carbon fiber cover
          coo[2]=(835.+6.)/10.+OctagonDimensions(i,4);
          break;

       case 1: // main octagon
          coo[2]=(835.+621.-6.)/10.+OctagonDimensions(i,4);
          break;

       case 0: // top carbon fiber octagon
          coo[2]=(835.+621.)/10.+OctagonDimensions(i,4);
          break;
       }
       SetOctagon(i,status,coo,unitnrm,gid); 
      }

      for(i=PrimaryOctagonNo();i<OctagonNo();i++){
       for(j=0;j<3;j++){
        coo[j]=0;
       }
       switch(i){
        case 10:  // top alu of Ze rad
         coo[2]=OctagonDimensions(GetPrimaryOctagon(i),7)+OctagonDimensions(i,4);
        break;
        case 11:  // rohacell
         coo[2]=OctagonDimensions(GetPrimaryOctagon(i),7)+2*OctagonDimensions(i-1,4)+OctagonDimensions(i,4);
        break;
        case 12:  // bot alu of Ze rad
         coo[2]=OctagonDimensions(GetPrimaryOctagon(i),7)+2*OctagonDimensions(i-2,4)+2*OctagonDimensions(i-1,4)+OctagonDimensions(i,4);
        break;
       }       
       SetOctagon(i,status,coo,unitnrm,gid); 
      }


      number blknrm[2][3][3]={1,0,0,0,1,0,0,0,1,
                            0,1,0,1,0,0,0,0,1};


      // Bulkheads
      for(i=0;i<TRDOctagonNo();i++){

	number rmin=OctagonDimensions(NoTRDOctagons(i),6);
	number rmax=OctagonDimensions(NoTRDOctagons(i),9);
	number zoct=OctagonDimensions(NoTRDOctagons(i),7)-OctagonDimensions(NoTRDOctagons(i),4);
	number tang=(rmax-rmin)/zoct;
	// Note: this doesn't quite match the diagram
	number bulkhead_pos = 3.*ManifoldWidth()+BulkheadGap()/2.;
	
         integer itrd=TRDDBc::NoTRDOctagons(i);
        // Central bulkheads  in x direction

	// Bulkhead 0

        // Bottom half-width	
	BulkheadsDimensions(i,0,0)= OctagonDimensions(itrd,6);

        // dy
	BulkheadsDimensions(i,0,2)= BulkheadWidth()/2.;

	//dz
	// Not sure what this is for the moment
	// Look like it stops half a layer above the top layer line
        // Top layer line is 35 mm below top 

	BulkheadsDimensions(i,0,3)= OctagonDimensions(itrd,7)-35./20.+
                   LadderThickness()/4.;

        // Top half-width
	BulkheadsDimensions(i,0,1)= BulkheadsDimensions(i,0,0)+
//                                     2.*tan(ang)*BulkheadsDimensions(i,0,3);
                                     2.*tang*BulkheadsDimensions(i,0,3);


        // Position it
	coo[0] = 0.;
	coo[1] = -bulkhead_pos;
	coo[2] = -(fabs(OctagonDimensions(itrd,4))-BulkheadsDimensions(i,0,3));

	SetBulkhead(0,i,status,coo,blknrm[_BHOrientation[0]],gid);

	// Bulkhead 1

	for (j=0;j<4;j++)
	  {
	    BulkheadsDimensions(i,1,j)= BulkheadsDimensions(i,0,j);
	  }

        // Position it
	coo[0] = 0.;
	coo[1] = bulkhead_pos;
	coo[2] = -(fabs(OctagonDimensions(itrd,4))-BulkheadsDimensions(i,1,3));

	SetBulkhead(1,i,status,coo,blknrm[_BHOrientation[1]],gid);

	// Top bulkheads in y direction

	// Bulkhead 2

	// dz half-height.. 
	BulkheadsDimensions(i,2,3)= 129./20+0.25;   // 0.25 to properly accomodate ladders

        // Top half-width
	BulkheadsDimensions(i,2,1)= OctagonDimensions(itrd,9);

        // Bottom half-width
	BulkheadsDimensions(i,2,0)= BulkheadsDimensions(i,2,1)
//	  -2.*BulkheadsDimensions(i,2,3)*tan(ang);
	  -2.*BulkheadsDimensions(i,2,3)*tang;


	BulkheadsDimensions(i,2,2)= BulkheadWidth()/2.;

        // Position it wrt main octagon
	coo[0] = -bulkhead_pos;
	coo[1] = 0.;

//      changed by VC 12-03-2002
//      coo[2] must less or equal OctagonDimensions(itrd,7)-BulkheadsDimensions(i,2,3)
//       if 	BulkheadsDimensions(i,2,1)= OctagonDimensions(itrd,9);
//

//	coo[2] = OctagonDimensions(itrd,7)-35./10.+LadderThickness()/2.-BulkheadsDimensions(i,2,3);
	coo[2] = OctagonDimensions(itrd,7)-BulkheadsDimensions(i,2,3);


	SetBulkhead(2,i,status,coo,blknrm[_BHOrientation[2]],gid);

	// Bulkhead 3

	for (j=0;j<4;j++)
	  {
	    BulkheadsDimensions(i,3,j)= BulkheadsDimensions(i,2,j);
	  }

        // Position it
	coo[0] = bulkhead_pos;
	coo[1] = 0.;
//	coo[2] = OctagonDimensions(itrd,7)-35./10.+LadderThickness()/2.
//                         -BulkheadsDimensions(i,3,3);

//      changed by VC 12-03-2002
//      coo[2] must be less or equal OctagonDimensions(itrd,7)-BulkheadsDimensions(i,2,3
//       if 	BulkheadsDimensions(i,2,1)= OctagonDimensions(itrd,9);
//
	coo[2] = OctagonDimensions(itrd,7)-BulkheadsDimensions(i,2,3);
	SetBulkhead(3,i,status,coo,blknrm[_BHOrientation[3]],gid);

	// Bottom bulkheads in y direction


	// Bulkhead 4

	// dz half-height.. also not sure what this is exactly
	BulkheadsDimensions(i,4,3)= 119.75/20;

        // Bottom half-width
	BulkheadsDimensions(i,4,0)= OctagonDimensions(itrd,6);

        // Top half-width
	BulkheadsDimensions(i,4,1)= BulkheadsDimensions(i,4,0)
//	 +2.*BulkheadsDimensions(i,4,3)*tan(ang);
	 +2.*BulkheadsDimensions(i,4,3)*tang;


        // dy
	BulkheadsDimensions(i,4,2)= BulkheadWidth()/2.;


        // Position it
	coo[0] = -bulkhead_pos;
	coo[1] = 0.;
	coo[2] = OctagonDimensions(itrd,4)+BulkheadsDimensions(i,4,3);

	SetBulkhead(4,i,status,coo,blknrm[_BHOrientation[4]],gid);

	// Bulkhead 5

	for (j=0;j<4;j++)
	  {
	    BulkheadsDimensions(i,5,j)= BulkheadsDimensions(i,4,j);
	  }

        // Position it
	coo[0] = bulkhead_pos;
	coo[1] = 0.;
	coo[2] = OctagonDimensions(itrd,4)+BulkheadsDimensions(i,5,3);

	SetBulkhead(5,i,status,coo,blknrm[_BHOrientation[5]],gid);

      }


      // Cutout sizes.  Actually the edges are rounded
      //  but just use a box for now

            for (i=0;i<TRDOctagonNo();i++)
	{
	  for(j=0;j<LayersNo(i);j++){
	    for(k=0;k<LaddersNo(i,j);k++){

	      // x
	      CutoutsDimensions(i,j,k,0) = CutoutWidth();

	      // y
	      CutoutsDimensions(i,j,k,1) = BulkheadWidth();

	      // z
	      CutoutsDimensions(i,j,k,2) = CutoutThickness();

	      // Half-dimensions
	    for(int l=0;l<3;l++)CutoutsDimensions(i,j,k,l)/=2.;
	    }
	  }
	}
      
     // LadderSizes

      number nrmxy[2][3][3]={0,0,1,1,0,0,0,1,0,
                            1,0,0,0,0,-1,0,1,0};


             int badc=0;
      for(i=0;i<TRDOctagonNo();i++){

//       Get octagon parameters at lowest point

	number rmin=OctagonDimensions(NoTRDOctagons(i),6);
	number rmax=OctagonDimensions(NoTRDOctagons(i),9);
	number zoct=OctagonDimensions(NoTRDOctagons(i),7)-OctagonDimensions(NoTRDOctagons(i),4);
	number tang=(rmax-rmin)/zoct;
	
	for(j=0;j<LayersNo(i);j++){

	  for(k=0;k<LaddersNo(i,j);k++){
              
              int gidd=i+mtrdo*j+mtrdo*maxlay*k+1;
	    //	    coo[0]=TubeInnerDiameter()
            //          +2*TubeWallThickness()+2*TubeBoxThickness();

	    // coo[0]*=TubesNo(i,j,k);
	    coo[0] = ManifoldWidth();
	    coo[1] = LadderThickness();

	    // This is the inner tangent radius of the octagon
            //  at the center of the side inner skin


       number r=LadderRefRadius()-2./20./cos(ang);

	    // Height of the bottom of manifold for the layer
            // Measured above the top of the lower carbon skin

	    number lhgt = FirstLayerHeight()
	      -WirePosition()-ManifoldThickness()/2.+j*LadderThickness();

	    // Add half a layer thickness to certain
            // tubes corresponding to the staggered pattern

            if (j<=3 ||j>=12)
	      {
		if (k%2 == 0)
		  {
		    lhgt += LadderThickness()/2.;
		  }
	      }
	    else
	      {
		if (k%2 == 1)
		  {
		    lhgt += LadderThickness()/2.;
		  }
	      }

	    // leftright is -1 if the ladder pokes into left of octagon,
	    // +1 if it pokes into right.  

	    integer leftright;
	    if(k<(LaddersNo(i,j)+1)/2)leftright=-1;
	    else leftright=1;

	    // Add the bulkhead gap width for ladders outside the bulkhead

	    number gap = 0.;

	    // Number away from the center of the ladder

	    int num_from_center = k-LaddersNo(i,j)/2;
	    if (leftright>0) num_from_center++;

	    // Check if we've crossed the bulkhead
            //  Bulkhead gap only present for lower and upper 4 layers
            // "x wires"

	    if (fabs(num_from_center)>=4 && (j<=3 || j>=16) )
	      {
	        gap = BulkheadGap();
	      }

	    // Position of the corner of ladder, transverse to wire direction

	    number point=ManifoldWidth()*num_from_center
                            +leftright*gap;

            // Center of manifold distance from origin,
            // in direction transverse to wire direction
 
	    number man_center = fabs(point) - ManifoldWidth()/2.;

	    // Now find the radius of the center of the 
            // octagon side inner skin
	    // at the height of the lower manifold edge
	    // This octagon inner tangent circle radius to be
            // used for "90 deg" ladders.
 
	    number r90 = r+lhgt*tan(ang);

	    // Length of octagon side at this given height

	    number a=2*r90/(1.+sqrt(2.));

	    // Find if the center of the manifold is past the corner

	    int past_corner = 0;
	    if (fabs(man_center)>a/2)
	      {
		past_corner = 1;
	      }

	    //	    if (past_corner)
	    
	    int deg = 90;
	    if (fabs(num_from_center)>=5 || (past_corner && (j<=3 || j>=16)))
	      {
		deg = 45;
		//		cout <<"45 deg condition"<<endl;

		// Ladder is a "45 degree" one

                // Get bottom radius
                // and add half HC width and half inner skin width
                // to be at the center of the HC

		r += 2./20./cos(ang) + 26./20./cos(ang);

                // Inner tangent radius of center of HC octagon 
                // at the height of the manifold bottom

                number r45 =  r + lhgt*tan(ang);

		// Length of side of the octagon at this height

		a = 2.*r45/(1.+sqrt(2.));

		// This is the half length of the ladder: 
                //   add half the side length to the "rise"
		//  corresponding to "run" of octagon
                //   radius - distance from center of manifold center,
		// with slope of 1 since it's 45 degrees
		
		coo[2] = (r45-man_center)+a/2;

	      }
	    else
	      {
		//		cout<< "90 deg "<<r90<<" "<<past_corner<<endl;
		// If center sticks out past the corner
//		if (past_corner)
//		  		  {
		//
//    		    cout <<"Past corner!!"<<endl;
//		  coo[2] = (r90-man_center)+a/2;
//		  }
//		else
		// Use r90
		coo[2] = r90;
		}

	    // Length to compare with engineer's numbers, which give
            // length up to halfway along the manifold, in mm


           number length_to_compare = 20.*(coo[2]+ManifoldLength()/2.);


	    // Add manifold length along wire

	    coo[2]+= ManifoldLength()/2.;

	    // Full ladder length

	    coo[2]*=2;
	

	if (fabs(coo[2]-_LaddersLength[j][k])>0.01){
          cerr<<"TRDDBc-F-LaddersLengthProblem"<<endl;
          cerr <<j<<" "<<k<<" "<<j+1<<" "<<num_from_center<<" "<<deg<<" "<<past_corner<<" "<<coo[2]<<" "<<_LaddersLength[j][k]*10.<<" "<<length_to_compare<<" "<<coo[2]-_LaddersLength[j][k]<<endl;
          abort();
        }


	    for(int l=0;l<3;l++)
	      {LaddersDimensions(i,j,k,l)=coo[l]/2;}
	  }  
	}

      }
     number nrm[3][3];
    //Ladder Position & Orientation
      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){


	  // Check if past the bulkhead gap

	   number gap = 0.;

	    // Number away from the center of the ladder

	    integer leftright;
	    if(k<(LaddersNo(i,j)+1)/2)leftright=-1;
	    else leftright=1;

	    int num_from_center = k-LaddersNo(i,j)/2;
	    if (leftright>0) num_from_center++;

	    // Check if we've crossed the bulkhead
            //  Bulkhead gap only present for lower and upper 4 layers
            // "x wires"

	    if (fabs(num_from_center)>=4 && (j<=3 || j>=16) )
	      {
	        gap = BulkheadGap();
	      }

	  // x or y position coordinates wrt mother octagon

         coo[LadderOrientation(i,j)]=0;
         coo[1-LadderOrientation(i,j)]=-LaddersDimensions(i,j,k,0)*(LaddersNo(i,j)-1.-2*k)+leftright*gap;

         // z position of center of ladder wrt mother octagon

         coo[2]= OctagonDimensions(1,4)+FirstLayerHeight()+2./10.
            +LaddersDimensions(i,j,k,1)*2.*j;

         int gidd=i+mtrdo*j+mtrdo*maxlay*k+1;
         SetLadder(k,j,i,status,coo,nrmxy[LadderOrientation(i,j)],gid);


         // Now position the bulkhead cutouts for the ladder
         //  wrt to the bulkhead coordinates
      

	 // First, find which kind of bulkhead for this layer

	 for (b=0;b<CutoutsNo(i,j,k);b++)
	   {
	     int bhno;
	     bhno = CutoutsBH(i,j,b);
	     
	     geant blkcoo[3];
	     
	     GetBulkhead(bhno,i,status,blkcoo,nrm,gid);
	     geant cocoo[3];

	 // x
         	     cocoo[0] = coo[1-LadderOrientation(i,j)];

         //y 
	             cocoo[1] = 0.;

        //z
                      cocoo[2] = coo[2]-blkcoo[2]-WirePosition()+0.9/10.;


             //  Check sizes however
	 int tube_is_upper = 0;
	 if (j<=3 ||j>=12)
	   {
	     if (k%2 == 0)  // Upper tube first
	       {
		 tube_is_upper = 1;
	       }
	   }
	  else
	   {

	     if (k%2 == 1)  // Lower tube first
	       {
		 tube_is_upper = 1;
	       }
	   }

	     int 	  ggid=i+mtrdo*j+mtrdo*maxlay*k+mtrdo*maxlay*maxlad*b+1;
	     SetCutout(b,k,j,i,status,cocoo,unitnrm,gid);
             if( fabs(cocoo[2]+tube_is_upper*LadderThickness()/2)+CutoutThickness()/2>BulkheadsDimensions(i,bhno,3) ){
              cerr <<" cutout id "<<ggid<<" "<<j<<" "<<k<<endl;
              cerr <<" cutout coo "<<cocoo[0]<<" "<<cocoo[1]<<" "<<cocoo[2]<<endl;
              cerr <<" blk coo "<<blkcoo[0]<<" "<<blkcoo[1]<<" "<<blkcoo[2]<<" "<<bhno<<endl;
              cerr <<" blk dim "<<BulkheadsDimensions(i,bhno,0)<<" "<<BulkheadsDimensions(i,bhno,1)<<" "<<BulkheadsDimensions(i,bhno,2)<<" "<<BulkheadsDimensions(i,bhno,3)<<endl;
              cerr <<" cutout dim "<<CutoutsDimensions(i,j,k,0)<<" "<<CutoutsDimensions(i,j,k,1)<<" "<<CutoutsDimensions(i,j,k,2)<<endl;
              cerr <<" ladder coo "<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;
              cerr <<" ladder dim "<<LaddersDimensions(i,j,k,0)<<" "<<LaddersDimensions(i,j,k,1)<<" "<<LaddersDimensions(i,j,k,2)<<endl;
             cerr <<"TRDDBc::init-F-CutOutOutOfBulkHead"<<endl;
             abort();
             
            }
	   }
        }
       }
      }

 
      status=0;
//
//    Now with known ladder positions (assuming they are o.k.) 
//    recalculate  ladder length matched with octgon dim 
//    VC 12-mar/2003
//


      for(i=0;i<TRDOctagonNo();i++){

//       Get octagon parameters at lowest point

	number rmin=OctagonDimensions(NoTRDOctagons(i),6);
	number rmax=OctagonDimensions(NoTRDOctagons(i),9);
	number zoct=OctagonDimensions(NoTRDOctagons(i),7)-OctagonDimensions(NoTRDOctagons(i),4);
	number tang=(rmax-rmin)/zoct;
	
	for(j=0;j<LayersNo(i);j++){

	  for(k=0;k<LaddersNo(i,j);k++){

              number nrmd[3][3];
              uinteger statusd,gidd;
              geant lcoo[3];
              GetLadder(k,j,i,statusd,lcoo,nrmd,gidd);
              number LadderLowestZ=lcoo[2]-LaddersDimensions(i,j,k,1);  // NB implicitely assume current ladder nrm  scheme !!!!!
              number LadderCornerCoo=fabs(lcoo[1-LadderOrientation(i,j)]);
              LadderCornerCoo+=LaddersDimensions(i,j,k,0);

//            Now   ladder length 

              number r=rmin+tang*(LadderLowestZ-OctagonDimensions(NoTRDOctagons(i),4));
              _LaddersLengthShort[j][k]=LadderCornerCoo<r*tan(ang)?r:r-(LadderCornerCoo-r*tan(ang))*tan(2*ang);
#ifdef __G4AMS__
               if(MISCFFKEY.G4On){
                 LaddersDimensions(i,j,k,2)= _LaddersLengthShort[j][k];
               }
#endif
	  }  
	}

      }

      // Radiator holes

      const number Safety=0;

     for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){

	  if (j==0 )
	    {

	      // Trapezoid height

	      if (k%2 == 0){

               // Upper tube: start at first layer line... 
               // down to lower wire position..
               //   add half a layer to arrive at upper wire position
               //  ... subtract 1/2 width
               //   of tube to arrive at top of radiator hole


	      	RadiatorHoleDimensions(i,j,k,0,0)= FirstLayerHeight()-
	           2./10. -WirePosition() + LadderThickness()/2.
                    - (TubeInnerDiameter()+2*TubeWallThickness()
                       +2*TubeBoxThickness()+2*Safety)/2.;
	      }	else {

                 // Lower tube

	      	RadiatorHoleDimensions(i,j,k,0,0)= FirstLayerHeight()-
	           2./10. -WirePosition() 
                    - (TubeInnerDiameter()+2*TubeWallThickness()
                       +2*TubeBoxThickness()+2*Safety)/2.;

              }


              // dz
              // it's the half-height
	      RadiatorHoleDimensions(i,j,k,0,0) /= 2.;

              // Used for slope of 45 deg side
              int num_from_edge = 0, far_edge=0;

	      // 45 deg case
	       if (k<4){num_from_edge = 4-k; far_edge = num_from_edge--;}
	       else if (k>9){num_from_edge = k-10; far_edge = num_from_edge++;}


              // All 3 holes (central between bulkheads, and on either side)
              //  have the same height

	      RadiatorHoleDimensions(i,j,k,1,0) 
                            = RadiatorHoleDimensions(i,j,k,2,0) 
                            = RadiatorHoleDimensions(i,j,k,0,0);

	      integer itrd=TRDDBc::NoTRDOctagons(i);
            
	      // bottom half-lengths

              //bl1 and tl1

	      // central one between bulkheads
	      RadiatorHoleDimensions(i,j,k,1,4)= 3.*ManifoldWidth();
	      RadiatorHoleDimensions(i,j,k,1,5)=RadiatorHoleDimensions(i,j,k,1,4);

	      // On either side of bulkhead

             // bl1

	      RadiatorHoleDimensions(i,j,k,0,4)= (OctagonDimensions(itrd,6)
                                    - far_edge*ManifoldWidth()
                                    -3.*ManifoldWidth()-BulkheadGap())/2.;
	      RadiatorHoleDimensions(i,j,k,2,4)= (OctagonDimensions(itrd,6)
                                    - far_edge*ManifoldWidth()
                                     -3.*ManifoldWidth()-BulkheadGap())/2.;
               // tl1


	      RadiatorHoleDimensions(i,j,k,0,5)=(OctagonDimensions(itrd,6)
                                    - num_from_edge*ManifoldWidth()
                                    -3.*ManifoldWidth()-BulkheadGap())/2.;


	      RadiatorHoleDimensions(i,j,k,2,5)=(OctagonDimensions(itrd,6)
                                    - num_from_edge*ManifoldWidth()
                                    -3.*ManifoldWidth()-BulkheadGap())/2.;


	      // top half-lengths
              // bl2 and tl2


	      // central one between bulkheads
	      	RadiatorHoleDimensions(i,j,k,1,8)= 3.*ManifoldWidth();
	      	RadiatorHoleDimensions(i,j,k,1,9)=RadiatorHoleDimensions(i,j,k,1,8);


	      // On either side of bulkhead

              // bl2
	      RadiatorHoleDimensions(i,j,k,0,8)= (OctagonDimensions(itrd,6)
                                    - far_edge*ManifoldWidth()
                                    -3.*ManifoldWidth()-BulkheadGap()
                                     +tan(ang)*RadiatorHoleDimensions(i,j,k,0,0)*2)/2.;


	      RadiatorHoleDimensions(i,j,k,2,8)= (OctagonDimensions(itrd,6)
                                    - far_edge*ManifoldWidth()
                                     -3.*ManifoldWidth()-BulkheadGap()
                                     +tan(ang)*RadiatorHoleDimensions(i,j,k,2,0)*2)/2.;


                // tl2
	      RadiatorHoleDimensions(i,j,k,0,9)= (OctagonDimensions(itrd,6)
                                    - num_from_edge*ManifoldWidth()
                                    -3.*ManifoldWidth()-BulkheadGap()
                                     +tan(ang)*RadiatorHoleDimensions(i,j,k,0,0)*2)/2.;



	      RadiatorHoleDimensions(i,j,k,2,9)=(OctagonDimensions(itrd,6)
                                    - num_from_edge*ManifoldWidth()
                                    -3.*ManifoldWidth()-BulkheadGap()
                                     +tan(ang)*RadiatorHoleDimensions(i,j,k,0,0)*2)/2.;



	      // half-width

	      RadiatorHoleDimensions(i,j,k,0,3)= LaddersDimensions(i,j,k,0);
	      RadiatorHoleDimensions(i,j,k,0,7)= LaddersDimensions(i,j,k,0);


              // All 3 holes (central between bulkheads, and on either side)
              //  have the same width

	      RadiatorHoleDimensions(i,j,k,1,3)
                            = RadiatorHoleDimensions(i,j,k,2,3) 
                            = RadiatorHoleDimensions(i,j,k,0,3);
	      RadiatorHoleDimensions(i,j,k,1,7) 
                            = RadiatorHoleDimensions(i,j,k,2,7) 
                            = RadiatorHoleDimensions(i,j,k,0,7);


	      // Angles

              // alpha1 and alpha2

	      if (k<4 || k>9){

	       RadiatorHoleDimensions(i,j,k,0,6) 
                 = atan((RadiatorHoleDimensions(i,j,k,0,5)
                         - RadiatorHoleDimensions(i,j,k,0,4))/
                         2./RadiatorHoleDimensions(i,j,k,0,3))*180./AMSDBc::pi;


               RadiatorHoleDimensions(i,j,k,2,6)
                 =-atan((RadiatorHoleDimensions(i,j,k,2,5)
                         - RadiatorHoleDimensions(i,j,k,2,4))/
                         2./RadiatorHoleDimensions(i,j,k,2,3))*180./AMSDBc::pi;

          

	       RadiatorHoleDimensions(i,j,k,0,10) 
                 = atan((RadiatorHoleDimensions(i,j,k,0,9)
                         - RadiatorHoleDimensions(i,j,k,0,8))/
                         2./RadiatorHoleDimensions(i,j,k,0,3))*180./AMSDBc::pi;


               RadiatorHoleDimensions(i,j,k,2,10)
                 = -atan((RadiatorHoleDimensions(i,j,k,2,9)
                         - RadiatorHoleDimensions(i,j,k,2,8))/
                         2./RadiatorHoleDimensions(i,j,k,2,3))*180./AMSDBc::pi;

          

 
               RadiatorHoleDimensions(i,j,k,1,6) = 0;
               RadiatorHoleDimensions(i,j,k,1,10) = 0;


	      }
  	       else{
	       RadiatorHoleDimensions(i,j,k,1,6) 
                            = RadiatorHoleDimensions(i,j,k,2,6) 
                            = RadiatorHoleDimensions(i,j,k,0,6) = 0;
              

 	       RadiatorHoleDimensions(i,j,k,1,10) 
                            = RadiatorHoleDimensions(i,j,k,2,10) 
                            = RadiatorHoleDimensions(i,j,k,0,10) = 0;
              }


              // phi

	      RadiatorHoleDimensions(i,j,k,1,2) 
                            = RadiatorHoleDimensions(i,j,k,2,2) 
                            = RadiatorHoleDimensions(i,j,k,0,2) = 0;

              // Angle between bottom y-x plane and top one
              //theta

	      RadiatorHoleDimensions(i,j,k,1,1) = 0;
 
              RadiatorHoleDimensions(i,j,k,0,1) 
                         = atan((RadiatorHoleDimensions(i,j,k,0,9)-
                                 RadiatorHoleDimensions(i,j,k,0,5))/
                                 RadiatorHoleDimensions(i,j,k,0,0)/2.)*180./AMSDBc::pi;

              RadiatorHoleDimensions(i,j,k,2,1) 
                         = -atan((RadiatorHoleDimensions(i,j,k,2,9)-
                                 RadiatorHoleDimensions(i,j,k,2,5))/
                                 RadiatorHoleDimensions(i,j,k,2,0)/2.)*180./AMSDBc::pi;

        
	    }
	  else{
	    for (int l=0;l<11;l++){RadiatorHoleDimensions(i,j,k,0,l)=0.;RadiatorHoleDimensions(i,j,k,1,l)=0.;RadiatorHoleDimensions(i,j,k,2,l)=0.;}
	  }
	}
	}
       } 


// Radiator side holes
   i=0;
  
// Bottom ones along x-dir:  radiator side holes numbers 0 and 1

// dz: distance from bottom of layer 4 tube to bottom of octagon
// All 3 pieces on each side have the same height

    integer itrd=TRDDBc::NoTRDOctagons(i);


     RadiatorSideHolePieceDimensions(i,0,0,0) = 
            RadiatorSideHolePieceDimensions(i,1,0,0) = 
     RadiatorSideHolePieceDimensions(i,0,1,0) = 
            RadiatorSideHolePieceDimensions(i,1,1,0) = 
     RadiatorSideHolePieceDimensions(i,0,2,0) = 
            RadiatorSideHolePieceDimensions(i,1,2,0) = 
                (FirstLayerHeight()-
	           2./10. -WirePosition() + 4.*LadderThickness()
                    - (TubeInnerDiameter()+2*TubeWallThickness()
                       +2*TubeBoxThickness()+2*Safety)/2.)/2.;

// h1

     RadiatorSideHolePieceDimensions(i,0,0,3) = 
            RadiatorSideHolePieceDimensions(i,1,0,3) = 
     RadiatorSideHolePieceDimensions(i,0,1,3) = 
            RadiatorSideHolePieceDimensions(i,1,1,3) = 
     RadiatorSideHolePieceDimensions(i,0,2,3) = 
            RadiatorSideHolePieceDimensions(i,1,2,3) = 
	     (OctagonDimensions(itrd,6)
                           -7.*ManifoldWidth()-BulkheadGap())/2.;


// h2 

     RadiatorSideHolePieceDimensions(i,0,0,7) = 
            RadiatorSideHolePieceDimensions(i,1,0,7) = 
     RadiatorSideHolePieceDimensions(i,0,1,7) = 
            RadiatorSideHolePieceDimensions(i,1,1,7) = 
     RadiatorSideHolePieceDimensions(i,0,2,7) = 
            RadiatorSideHolePieceDimensions(i,1,2,7) = 
	     (OctagonDimensions(itrd,6)
                -7.*ManifoldWidth()-BulkheadGap()
                + 2.*RadiatorSideHolePieceDimensions(i,0,0,0)*tan(ang))/2.;



//  bottom half lengths: bl1 and tl1

   // central one between bulkheads

	RadiatorSideHolePieceDimensions(i,0,1,4) =
            RadiatorSideHolePieceDimensions(i,1,1,4) =
	RadiatorSideHolePieceDimensions(i,0,1,5) = 
            RadiatorSideHolePieceDimensions(i,1,1,5) =
                                      3.*ManifoldWidth();

  // On either side of bulkhead



      // bl1 

      // k=0 ladder side:  bl1 is shorter side
         RadiatorSideHolePieceDimensions(i,0,0,4)= 
               RadiatorSideHolePieceDimensions(i,0,2,4)= 
                    (OctagonDimensions(itrd,6)
                                  - 4.*ManifoldWidth()  // for slope
                                  - 2.*RadiatorSideHolePieceDimensions(i,0,0,3)
                                    -3.*ManifoldWidth()-BulkheadGap())/2.;
      // k=13 ladder side
         RadiatorSideHolePieceDimensions(i,1,0,4)= 
               RadiatorSideHolePieceDimensions(i,1,2,4)= 
                    (OctagonDimensions(itrd,6)
                                  - 4.*ManifoldWidth()  // for slope
                                    -3.*ManifoldWidth()-BulkheadGap())/2.;


      // tl1

      // k=0 ladder side
         RadiatorSideHolePieceDimensions(i,0,0,5)= 
               RadiatorSideHolePieceDimensions(i,0,2,5)= 
                    (OctagonDimensions(itrd,6)
                                  - 4.*ManifoldWidth()  // for slope
                                    -3.*ManifoldWidth()-BulkheadGap())/2.;
      // k=13 ladder side
         RadiatorSideHolePieceDimensions(i,1,0,5)= 
               RadiatorSideHolePieceDimensions(i,1,2,5)= 
                    (OctagonDimensions(itrd,6)
                                  - 4.*ManifoldWidth()  // for slope
                                  - 2.*RadiatorSideHolePieceDimensions(i,1,0,3)
                                    -3.*ManifoldWidth()-BulkheadGap())/2.;

        


//  top half lengths: bl2 and tl2

   // central one between bulkheads

	RadiatorSideHolePieceDimensions(i,0,1,8) =
            RadiatorSideHolePieceDimensions(i,1,1,8) =
	RadiatorSideHolePieceDimensions(i,0,1,9) = 
            RadiatorSideHolePieceDimensions(i,1,1,9) =
                                      3.*ManifoldWidth();

  // On either side of bulkhead



      // bl2

      // k=0 ladder side
         RadiatorSideHolePieceDimensions(i,0,0,8)= 
               RadiatorSideHolePieceDimensions(i,0,2,8)= 
                    (OctagonDimensions(itrd,6)
                        - 4.*ManifoldWidth()  // for slope
                       - 2.*RadiatorSideHolePieceDimensions(i,0,0,7)
                       -3.*ManifoldWidth()-BulkheadGap()
                    +tan(ang)*RadiatorSideHolePieceDimensions(i,0,0,0)*2)/2.;


      // k=13 ladder side
         RadiatorSideHolePieceDimensions(i,1,0,8)= 
               RadiatorSideHolePieceDimensions(i,1,2,8)= 
                    (OctagonDimensions(itrd,6)
                        - 4.*ManifoldWidth()  // for slope
                   -3.*ManifoldWidth()-BulkheadGap()
               +tan(ang)*RadiatorSideHolePieceDimensions(i,1,0,0)*2)/2.;


      // tl2

      // k=0 ladder side
         RadiatorSideHolePieceDimensions(i,0,0,9)= 
               RadiatorSideHolePieceDimensions(i,0,2,9)= 
                    (OctagonDimensions(itrd,6)
                        - 4.*ManifoldWidth()  // for slope
                  -3.*ManifoldWidth()-BulkheadGap()
               +tan(ang)*RadiatorSideHolePieceDimensions(i,0,0,0)*2)/2.;

      // k=13 ladder side
         RadiatorSideHolePieceDimensions(i,1,0,9)= 
               RadiatorSideHolePieceDimensions(i,1,2,9)= 
                    (OctagonDimensions(itrd,6)
                        - 4.*ManifoldWidth()  // for slope
               - 2.*RadiatorSideHolePieceDimensions(i,0,0,7)
              -3.*ManifoldWidth()-BulkheadGap()
               +tan(ang)*RadiatorSideHolePieceDimensions(i,1,0,0)*2)/2.;

        
// Angles

	
      // alpha1 

     RadiatorSideHolePieceDimensions(i,0,0,6) =
             atan((RadiatorSideHolePieceDimensions(i,0,0,5)
                         - RadiatorSideHolePieceDimensions(i,0,0,4))/
                         2./RadiatorSideHolePieceDimensions(i,0,0,3))*180./AMSDBc::pi;



      RadiatorSideHolePieceDimensions(i,0,2,6) =
             -atan((RadiatorSideHolePieceDimensions(i,0,2,5)
                         - RadiatorSideHolePieceDimensions(i,0,2,4))/
                         2./RadiatorSideHolePieceDimensions(i,0,2,3))*180./AMSDBc::pi;

  

     RadiatorSideHolePieceDimensions(i,1,0,6) =
             atan((RadiatorSideHolePieceDimensions(i,1,0,5)
                         - RadiatorSideHolePieceDimensions(i,1,0,4))/
                         2./RadiatorSideHolePieceDimensions(i,1,0,3))*180./AMSDBc::pi;



       RadiatorSideHolePieceDimensions(i,1,2,6) =
             -atan((RadiatorSideHolePieceDimensions(i,1,2,5)
                         - RadiatorSideHolePieceDimensions(i,1,2,4))/
                         2./RadiatorSideHolePieceDimensions(i,1,2,3))*180./AMSDBc::pi;


      RadiatorSideHolePieceDimensions(i,0,1,6) =
              RadiatorSideHolePieceDimensions(i,1,1,6) = 0.;


 

      // alpha2

      RadiatorSideHolePieceDimensions(i,0,0,10) =
             atan((RadiatorSideHolePieceDimensions(i,0,0,9)
                         - RadiatorSideHolePieceDimensions(i,0,0,8))/
                         2./RadiatorSideHolePieceDimensions(i,0,0,7))*180./AMSDBc::pi;



      RadiatorSideHolePieceDimensions(i,0,2,10) =
             -atan((RadiatorSideHolePieceDimensions(i,0,2,9)
                         - RadiatorSideHolePieceDimensions(i,0,2,8))/
                         2./RadiatorSideHolePieceDimensions(i,0,2,7))*180./AMSDBc::pi;


       RadiatorSideHolePieceDimensions(i,1,0,10) =
             atan((RadiatorSideHolePieceDimensions(i,1,0,9)
                         - RadiatorSideHolePieceDimensions(i,1,0,8))/
                         2./RadiatorSideHolePieceDimensions(i,1,0,7))*180./AMSDBc::pi;


       RadiatorSideHolePieceDimensions(i,1,2,10) =
             -atan((RadiatorSideHolePieceDimensions(i,1,2,9)
                         - RadiatorSideHolePieceDimensions(i,1,2,8))/
                         2./RadiatorSideHolePieceDimensions(i,1,2,7))*180./AMSDBc::pi;



      RadiatorSideHolePieceDimensions(i,0,1,10) =
              RadiatorSideHolePieceDimensions(i,1,1,10) = 0.;

     // theta


	for (int r=0;r<SideHolesNo(i);r++){
          for (int s=0;s<SideHolesPieceNo(i,r);s++){

      // "Center" is the intersection of the lines joining
      // midpoints of each side, not the centroid

	number bottom_xcent, bottom_ycent;
	number top_xcent, top_ycent;
  	number phi;
	number xdiff,ydiff,zdiff;

	bottom_ycent = RadiatorSideHolePieceDimensions(i,r,s,3);
	top_ycent = RadiatorSideHolePieceDimensions(i,r,s,7);

	bottom_xcent = (RadiatorSideHolePieceDimensions(i,r,s,4)
                                 +RadiatorSideHolePieceDimensions(i,r,s,5))/2.;
	top_xcent = (RadiatorSideHolePieceDimensions(i,r,s,8)
                                 + RadiatorSideHolePieceDimensions(i,r,s,9))/2.;



   	xdiff = top_xcent-bottom_xcent;
	ydiff = top_ycent-bottom_ycent;
        zdiff = 2.*RadiatorSideHolePieceDimensions(i,r,s,0);

	number totR = sqrt(xdiff*xdiff+ydiff*ydiff+zdiff*zdiff);


	if (fabs(totR)>0) { 
           RadiatorSideHolePieceDimensions(i,r,s,1) = 
	       acos(zdiff/totR)*180./AMSDBc::pi;
	  }
	else{
           RadiatorSideHolePieceDimensions(i,r,s,1) = 0;
           }

	   phi = 0.;
	   if (fabs(xdiff)>0){
             phi=atan(ydiff/xdiff)*180./AMSDBc::pi;           
           }

	   if (s==1 && r==0){
	     phi =  -90.;
 	   }else if (s==1 && r==1) {
	     phi =  +90.;
           }
	   if (r==0 && s==0) phi = -phi;
	   if (r==0 && s==2) phi += 180.;
	   if (r==1 && s==2) phi = -phi-180.;

           RadiatorSideHolePieceDimensions(i,r,s,2) = phi;

          
         }
        }

// VC 06-06-2005 
// try to fix KS bug
        for(i=0;i<TRDOctagonNo();i++){
         for(int h=0;h<TRDDBc::SideHolesNo(i);h++){
          for (int hp=0;hp<TRDDBc::SideHolesPieceNo(i,h);hp++){
           for(int ip=0;ip<11;ip++){
              if(RadiatorSideHolePieceDimensions(i,h,hp,ip)<0 && ip!=1 && ip!=2 && ip!=6 && ip!=10){
                cerr <<"TRDDBc::init-W-RadiatorSideHolePieceDimensions<0 "<<RadiatorSideHolePieceDimensions(i,h,hp,ip)<<" "<<ip<<endl;
                RadiatorSideHolePieceDimensions(i,h,hp,ip)=0;
              }
           }
          }
         }
        }
     
      // Tube sizes

      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){

	  // width
          TubesDimensions(i,j,k,0)=TubeInnerDiameter()/2;          
          // height
          TubesDimensions(i,j,k,1)=(TubeInnerDiameter()+2*TubeWallThickness())/2;
          // length

          TubesDimensions(i,j,k,2)=LaddersDimensions(i,j,k,2)-_EndPieceLength/2;

	  // Wires Dimensions

	  // width
          WiresDimensions(i,j,k,0)=0;          
          // height
          WiresDimensions(i,j,k,1)=WireDiameter()/2.;
          // length

          WiresDimensions(i,j,k,2)=LaddersDimensions(i,j,k,2)-_EndPieceLength/2.;



//       SpacerDimensions
//
         // inner part
         SpacerDimensions(i,j,k,0,0)=SpacerDim(0)/2;
         SpacerDimensions(i,j,k,1,0)=SpacerDim(1)/2;
         SpacerDimensions(i,j,k,2,0)=LaddersDimensions(i,j,k,2);
         // outer part
         SpacerDimensions(i,j,k,0,1)=SpacerDim(2)/2;
         SpacerDimensions(i,j,k,1,1)=SpacerDim(3)/2;
         SpacerDimensions(i,j,k,2,1)=LaddersDimensions(i,j,k,2);

//  Chamber Dimensions
	  // width
          TubesBoxDimensions(i,j,k,0)=maxtube*(TubesDimensions(i,j,k,1)*2+TubeBoxThickness())/2+_NumberSpacers[maxtube-1]*SpacerWidth()/2;      
	  // height
          TubesBoxDimensions(i,j,k,1)=ChamberThickness()/2;
          TubesBoxDimensions(i,j,k,2)=LaddersDimensions(i,j,k,2);
          TubesBoxDimensions(i,j,k,3)=TubesDimensions(i,j,k,1)*2+TubeBoxThickness()+ChamberThickness();
        }
       }
      }


// Set the side radiator holes wrt the octagon

    i=0;
    coo[0]=coo[2]=coo[1]=0;


//    for (int r=0;r<SideHolesNo(i);r++){
    for (int r=0;r<2;r++){
      for (int s=0;s<SideHolesPieceNo(i,r);s++){
         number top_xcent = (RadiatorSideHolePieceDimensions(i,r,s,8)+
                                 RadiatorSideHolePieceDimensions(i,r,s,9))/2.;
         number bottom_xcent = (RadiatorSideHolePieceDimensions(i,r,s,4)+
                                 RadiatorSideHolePieceDimensions(i,r,s,5))/2.;


         coo[0]=0.;
         if (s==0){

           coo[0] = 3.*ManifoldWidth()+BulkheadGap()
               + bottom_xcent + (top_xcent-bottom_xcent)/2.;

         }
	 else if (s==2){

           coo[0] = -( 3.*ManifoldWidth()+BulkheadGap()
                + bottom_xcent + (top_xcent-bottom_xcent)/2.);


         } 
 


	if (r==0){
          coo[1] = -(7.*ManifoldWidth()+BulkheadGap()+
              (RadiatorSideHolePieceDimensions(i,r,s,3)+
                     RadiatorSideHolePieceDimensions(i,r,s,7))/2.);
        }
	else {
         coo[1] = 7.*ManifoldWidth()+BulkheadGap()+
              (RadiatorSideHolePieceDimensions(i,r,s,3)+
                     RadiatorSideHolePieceDimensions(i,r,s,7))/2.;
        }


// Reminder:  OctagonDimensions(itrd,4) is negative

          coo[2] = OctagonDimensions(itrd,4)
                      +RadiatorSideHolePieceDimensions(i,r,s,0);


           SetRadiatorSideHolePiece(s,r,i,status,coo,unitnrm,gid);

         }
       }	



      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){

	 int tube_is_upper = 0;
	 if (j<=3 ||j>=12) {
	   if (k%2 == 0){  // Upper tube first
	      tube_is_upper = 1;
	     }
	  }
        else
        {
	   if (k%2 == 1){  // Upper tube first
	      tube_is_upper = 1;
	     }
	}


	 coo[0]=coo[2]=coo[1]=0;


	 if (j==0)
	   {

    	     if (tube_is_upper){
		 coo[1]=-WirePosition()+LadderThickness()/2
       -(TubeInnerDiameter()+2*TubeWallThickness()+2*TubeBoxThickness()
                +2*Safety)/2-RadiatorHoleDimensions(i,j,k,0,0);
              }
             else {
		 coo[1]=-WirePosition()
       -(TubeInnerDiameter()+2*TubeWallThickness()+2*TubeBoxThickness()
                +2*Safety)/2-RadiatorHoleDimensions(i,j,k,0,0);

             }            

	   }

//         number radholenrm[3][3]={0,1,0,1,0,0,0,0,1};


        // Set the 3 radiator holes for the bottom ladders
	SetRadiatorHole(1,k,j,i,status,coo,unitnrm,gid);

        number xlength_along_center_bottom = 
	       (RadiatorHoleDimensions(i,j,k,0,4)
                     +RadiatorHoleDimensions(i,j,k,0,5))/2.;
        number xlength_along_center_top = 
	       (RadiatorHoleDimensions(i,j,k,0,8)
                     +RadiatorHoleDimensions(i,j,k,0,9))/2.;


	coo[2]+= (3*ManifoldWidth()+BulkheadGap()
                         + xlength_along_center_bottom
                         + (xlength_along_center_top
                          - xlength_along_center_bottom)/2.);
                       // Compensate for offset of center of trapezoid


	SetRadiatorHole(0,k,j,i,status,coo,unitnrm,gid);

	coo[2]-= 2.*(3*ManifoldWidth()+BulkheadGap()
                         + xlength_along_center_bottom
                         + (xlength_along_center_top
                          - xlength_along_center_bottom)/2.);

	SetRadiatorHole(2,k,j,i,status,coo,unitnrm,gid);
	
 
        for(int l=0;l<TubesNo(i,j,k);l++){

	   // x, y positions of tubes wrt ladder

           coo[0]=-LaddersDimensions(i,j,k,0)+(TubesDimensions(i,j,k,1)+TubeBoxThickness())*(1+2*l)+NumberSpacers(l)*SpacerWidth();

           coo[2]=0;

	   // z position of tubes 

	   if (tube_is_upper)
	     {
	       coo[1]= -WirePosition()+LadderThickness()/2.;

	       // Adjust bulkhead cutout position

	       if (l==0)
		 {
		   geant cocoo[3];
		   GetCutout(0,k,j,i,status,cocoo,unitnrm,gid);
		   
		   cocoo[2] += LadderThickness()/2;

		   SetCutout(0,k,j,i,status,cocoo,unitnrm,gid);
		   SetCutout(1,k,j,i,status,cocoo,unitnrm,gid);
		 }

	     }
	   else
	     {
	       coo[1]= -WirePosition();
	     }
           SetTube(l,k,j,i,status,coo,unitnrm,gid);
           if(l==0){
              //set tube box
                coo[0]=0;             
                coo[1]=coo[1]+TubesDimensions(i,j,k,1)+TubeBoxThickness()/2+ChamberThickness()/2;             
              SetTubeBox(k,j,i,status,coo,unitnrm,gid);
           }
         }
        }
       }
      }

//   Now Final Set for Pipes & Spikes

//  Assuming Mirroring of 4 quadrants  for spikes

    for (int is=0;is<trdconst::maxspikes;is++){
     for(int i=0;i<4;i++){
       for(int k=0;k<6;k++)_SpikesPar[is][i][k]=_SpikesPar[is][3][k];
       switch(i){
         case 0:
         _SpikesPar[is][i][0]=-_SpikesPar[is][i][0];
         _SpikesPar[is][i][1]=-_SpikesPar[is][i][1];
         break;
         case 1:
         _SpikesPar[is][i][1]=-_SpikesPar[is][i][1];
         break;
         case 2:
         _SpikesPar[is][i][0]=-_SpikesPar[is][i][0];
         break;
       }
    }
  } 
{

// Rotatate spikes by 22.5 deg
  number ang=22.5;
  number cr=cos(ang/180.*AMSDBc::pi);
  number sr=sin(ang/180.*AMSDBc::pi);
  number nrmr[3][3]={cr,sr,0,-sr,cr,0,0,0,1};
  for (int i=0;i<trdconst::maxspikes;i++){
  for (int iq=0;iq<4;iq++){
   geant coo[3]={0,0,0};  
   for (int j=0;j<3;j++){
    for (int k=0;k<3;k++){
     coo[j]+=_SpikesPar[i][iq][k]*nrmr[j][k];
    }
   }
   for (int j=0;j<3;j++)_SpikesPar[i][iq][j]=coo[j];
   //for (int j=0;j<3;j++)cout <<" "<<_SpikesPar[i][iq][j]<<endl;
  }
} 
}




    

 // Pipe 3
    
{
    _PipesPar[2][3][0]=(93+101.5)/2+1.5;
    _PipesPar[2][3][1]=(-10-33)/2-0.75;
    number a=atan2(33-10,101.5-93);
    number ca=sin(a);
    number sa=cos(a);
    _PipesPar[2][3][5]=23/2./ca-1.5;
    _PipesNRM[2][3][0][0]=ca;
    _PipesNRM[2][3][0][1]=0;
    _PipesNRM[2][3][0][2]=sa;
    _PipesNRM[2][3][1][0]=-sa;
    _PipesNRM[2][3][1][1]=0;
    _PipesNRM[2][3][1][2]=ca;
    _PipesNRM[2][3][2][0]=0;
    _PipesNRM[2][3][2][1]=-1;
    _PipesNRM[2][3][2][2]=0;

}
 // Pipe 5
    
    _PipesPar[5][3][0]=(31.5+57.5)/2;
    _PipesPar[5][3][1]=(-94.5-85.5)/2;
    number a=atan2(94.5-85.5,57.5-31.5);
    number ca=sin(a);
    number sa=cos(a);
    _PipesPar[5][3][5]=(57.5-31.5)/sa/2.-1.5;
    _PipesNRM[5][3][0][0]=ca;
    _PipesNRM[5][3][0][1]=0;
    _PipesNRM[5][3][0][2]=sa;
    _PipesNRM[5][3][1][0]=-sa;
    _PipesNRM[5][3][1][1]=0;
    _PipesNRM[5][3][1][2]=ca;
    _PipesNRM[5][3][2][0]=0;
    _PipesNRM[5][3][2][1]=-1;
    _PipesNRM[5][3][2][2]=0;


/*
//  Assuming mirroring of 4 quadrants  for pipes

    for (int is=0;is<trdconst::maxpipes;is++){
     for(int i=0;i<4;i++){
       for(int k=0;k<7;k++)_PipesPar[is][i][k]=_PipesPar[is][3][k];
       for (int k=0;k<3;k++){
         for (int l=0;l<3;l++)_PipesNRM[is][i][k][l]=_PipesNRM[is][3][k][l];
       }
       switch(i){
         case 0:
         _PipesPar[is][i][0]=-_PipesPar[is][i][0];
         _PipesPar[is][i][1]=-_PipesPar[is][i][1];
         break;
         case 1:
         _PipesPar[is][i][1]=-_PipesPar[is][i][1];
         _PipesNRM[is][i][0][2]=-_PipesNRM[is][i][0][2];
         _PipesNRM[is][i][1][0]=-_PipesNRM[is][i][1][0];
         break;
         case 2:
         _PipesPar[is][i][0]=-_PipesPar[is][i][0];
         _PipesNRM[is][i][0][2]=-_PipesNRM[is][i][0][2];
         _PipesNRM[is][i][1][0]=-_PipesNRM[is][i][1][0];
         break;
       }
    }
  } 
*/

//  Assuming rotating of 4 quadrants  for pipes

    for (int is=0;is<trdconst::maxpipes;is++){
     for(int i=0;i<4;i++){
       for(int k=0;k<7;k++)_PipesPar[is][i][k]=_PipesPar[is][3][k];
       for (int k=0;k<3;k++){
         for (int l=0;l<3;l++)_PipesNRM[is][i][k][l]=_PipesNRM[is][3][k][l];
       }
       number ang=0;
       switch(i){
         case 0:
         ang=180.;
         break;
         case 1:
          ang=-90.;
         break;
         case 2:
          ang=90.;
         break;
       }
  number cr=cos(ang/180.*AMSDBc::pi);
  number sr=sin(ang/180.*AMSDBc::pi);
  number nrmr[3][3]={cr,sr,0,-sr,cr,0,0,0,1};
   geant coo[3]={0,0,0};  
   number nrm[3][3]={0,0,0,0,0,0,0,0,0};
   for (int j=0;j<3;j++){
    for (int k=0;k<3;k++){
     coo[j]+=_PipesPar[is][i][k]*nrmr[j][k];
    }
   }
   for (int j=0;j<3;j++)_PipesPar[is][i][j]=coo[j];
   for (int i1=0;i1<3;i1++){
    for(int i2=0;i2<3;i2++){
     for (int j=0;j<3;j++){
       nrm[i1][i2]=nrm[i1][i2]+nrmr[i1][j]*_PipesNRM[is][i][j][i2];
     }
    }
   }
   for (int i1=0;i1<3;i1++){
    for(int i2=0;i2<3;i2++){
     _PipesNRM[is][i][i1][i2]=nrm[i1][i2];
    }
   }
   }
  } 


{

// Rotatate pipes by 22.5 deg
  number cr=cos(22.5/180.*AMSDBc::pi);
  number sr=sin(22.5/180.*AMSDBc::pi);
  number nrmr[3][3]={cr,sr,0,-sr,cr,0,0,0,1};
  for (int i=0;i<trdconst::maxpipes;i++){
  for (int iq=0;iq<4;iq++){
   geant coo[3]={0,0,0};  
   number nrm[3][3]={0,0,0,0,0,0,0,0,0};
   for (int j=0;j<3;j++){
    for (int k=0;k<3;k++){
     coo[j]+=_PipesPar[i][iq][k]*nrmr[j][k];
    }
   }
   for (int j=0;j<3;j++)_PipesPar[i][iq][j]=coo[j];
   for (int i1=0;i1<3;i1++){
    for(int i2=0;i2<3;i2++){
     for (int j=0;j<3;j++){
       nrm[i1][i2]=nrm[i1][i2]+nrmr[i1][j]*_PipesNRM[i][iq][j][i2];
     }
    }
   }
   for (int i1=0;i1<3;i1++){
    for(int i2=0;i2<3;i2++){
     _PipesNRM[i][iq][i1][i2]=nrm[i1][i2];
    }
   }
  }
}
}


      InitPattern();
}



 integer * TRDDBc::_patconf[trdconst::maxlay];
 integer * TRDDBc::_patpoints;
 integer * TRDDBc::_patmiss[trdconst::maxlay];
 integer   TRDDBc::_patd[trdconst::maxlay]; 
 integer * TRDDBc::_patallow;
 integer * TRDDBc::_patallow2;
 uinteger TRDDBc::_Npat=0;

 uinteger TRDDBc::_Nseg=4;
 uinteger TRDDBc::_NlayH[trdconst::maxseg]={4,6,6,4,0};
 integer  TRDDBc::_oriseg[trdconst::maxseg]={0,1,1,0,0};
 integer  TRDDBc::_segconf[trdconst::maxseg][trdconst::maxhits]={
  1, 2, 3, 4, 0,0,0,0,0,0,0,0,
  5, 6, 7, 8, 9,10,0,0,0,0,0,0,
 11,12,13,14,15,16,0,0,0,0,0,0,
 17,18,19,20,0,0,0,0,0,0,0,0
};

/*
 uinteger TRDDBc::_Nseg=3;
 uinteger TRDDBc::_NlayH[trdconst::maxseg]={4,12,4,0,0};
 integer  TRDDBc::_segconf[trdconst::maxseg][trdconst::maxhits]={
  1, 2, 3, 4, 0,0,0,0,0,0,0,0,
  5, 6, 7, 8, 9,10,11,12,13,14,15,16,
 17,18,19,20,0,0,0,0,0,0,0,0
};
 integer  TRDDBc::_oriseg[trdconst::maxseg]={0,1,0,0,0};
*/
/*
 uinteger TRDDBc::_Nseg=2;
 uinteger TRDDBc::_NlayH[trdconst::maxseg]={8,12,0,0,0};
 integer  TRDDBc::_segconf[trdconst::maxseg][trdconst::maxhits]={
  1, 2, 3, 4, 17,18,19,20,0,0,0,0,
  5, 6, 7, 8, 9,10,11,12,13,14,15,16
};
 integer  TRDDBc::_oriseg[trdconst::maxseg]={0,1,0,0,0};
*/

 integer  TRDDBc::_segpoints[trdconst::maxseg];

 integer * TRDDBc::_patconfS[trdconst::maxseg];
 integer * TRDDBc::_patpointsS;
 integer * TRDDBc::_patmissS[trdconst::maxseg];
 integer   TRDDBc::_patdS[trdconst::maxseg]; 
 integer * TRDDBc::_patallowS;
 integer * TRDDBc::_patallow2S;
 uinteger TRDDBc::_NpatS=0;

 integer * TRDDBc::_patconfH[trdconst::maxseg][trdconst::maxhits];
 integer * TRDDBc::_patpointsH[trdconst::maxseg];
 integer * TRDDBc::_patmissH[trdconst::maxseg][trdconst::maxhits];
 integer   TRDDBc::_patdH[trdconst::maxseg][trdconst::maxhits]; 
 integer * TRDDBc::_patallowH[trdconst::maxseg];
 integer * TRDDBc::_patallow2H[trdconst::maxseg];
 uinteger TRDDBc::_NpatH[trdconst::maxseg]={0,0,0,0,0};



void TRDDBc::InitPattern(){

/*
{

    int k;
    integer ordermiss[trdconst::maxlay]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    integer vmiss[trdconst::maxlay]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    integer vorder[trdconst::maxlay]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    int minc;
    int iq=0;
    for(minc=0;minc<nlay()-2;minc+=2){
       ordermiss[iq]=nlay()-1-minc;
       ordermiss[iq+(nlay()+1)/2-1]=nlay()-2-minc;
       iq++;
    }
    ordermiss[nlay()-2]=nlay();
    ordermiss[nlay()-1]=1;
#ifdef __AMSDEBUG__
       for(minc=0;minc<nlay();minc++)cout <<"ordermiss["<<minc<<"] "<<ordermiss[minc]<<endl;
#endif
//initialize patterns
   for(minc=nlay();minc>13;minc--){
     _Npat+=TRDDBc::Cnk(nlay(),minc);
     _patd[nlay()-minc+1]=_Npat;
   }
   cout <<" too many pattern ... "<<_Npat<<endl;
   for(int nl=0;nl<nlay();nl++){
     _patmiss[nl]=new integer[_Npat];
     _patconf[nl]=new integer[_Npat];
   }   
   _patpoints =new integer[_Npat];
   _patallow =new integer[_Npat];
   _patallow2 =new integer[_Npat];
   int cpat=0;
   for (cpat=0;cpat<npat();cpat++){
     for(int npt=1;npt<nlay();npt++){
       if(cpat<_patd[npt]){
         _patpoints[cpat]=nlay()-npt+1;
         int vmini=cpat-_patd[npt-1];
         int count=0;
         int v,i1,i2,i3,i4;
         for(v=0;v<nlay();v++)vmiss[v]=0;
         switch(npt-1){
          case 0:
           break;
          case 1:        
            for(i1=0;i1<nlay();i1++){
             if(vmini==count){
              vmiss[nlay()-1]=ordermiss[i1];
             }
             count++;
            }            
           break;
          case 2:        
            for(i1=0;i1<nlay();i1++){
             for(i2=i1+1;i2<nlay();i2++){
              if(vmini==count){
               vmiss[nlay()-1]=ordermiss[i2];
               vmiss[nlay()-2]=ordermiss[i1];
              }
              count++;
             }
            }            
           break;
          case 3:        
            for(i1=0;i1<nlay();i1++){
             for(i2=i1+1;i2<nlay();i2++){
              for(i3=i2+1;i3<nlay();i3++){
               if(vmini==count){
                vmiss[nlay()-1]=ordermiss[i3];
                vmiss[nlay()-2]=ordermiss[i2];
                vmiss[nlay()-3]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           break;
          case 4:        
            for(i1=0;i1<nlay();i1++){
             for(i2=i1+1;i2<nlay();i2++){
              for(i3=i2+1;i3<nlay();i3++){
              for(i4=i3+1;i4<nlay();i4++){
               if(vmini==count){
                vmiss[nlay()-1]=ordermiss[i4];
                vmiss[nlay()-2]=ordermiss[i3];
                vmiss[nlay()-3]=ordermiss[i2];
                vmiss[nlay()-4]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           }
           break;
          case 5:        
            for(i1=0;i1<nlay();i1++){
             for(i2=i1+1;i2<nlay();i2++){
              for(i3=i2+1;i3<nlay();i3++){
              for(i4=i3+1;i4<nlay();i4++){
               for(int i5=i4+1;i5<nlay();i5++){
               if(vmini==count){
                vmiss[nlay()-1]=ordermiss[i5];
                vmiss[nlay()-2]=ordermiss[i4];
                vmiss[nlay()-3]=ordermiss[i3];
                vmiss[nlay()-4]=ordermiss[i2];
                vmiss[nlay()-5]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           }
           }
           break;
          case 6:        
            for(i1=0;i1<nlay();i1++){
             for(i2=i1+1;i2<nlay();i2++){
              for(i3=i2+1;i3<nlay();i3++){
              for(i4=i3+1;i4<nlay();i4++){
               for(int i5=i4+1;i5<nlay();i5++){
               for(int i6=i5+1;i6<nlay();i6++){
               if(vmini==count){
                vmiss[nlay()-1]=ordermiss[i6];
                vmiss[nlay()-2]=ordermiss[i5];
                vmiss[nlay()-3]=ordermiss[i4];
                vmiss[nlay()-4]=ordermiss[i3];
                vmiss[nlay()-5]=ordermiss[i2];
                vmiss[nlay()-6]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           }
           }
           }
           break;
          default:
           cerr<<"TRDDBc::init-F-PatternLogicError"<<endl;
           exit(1);
         }
         for(v=0;v<nlay();v++)_patmiss[v][cpat]=vmiss[v];
         for(v=0;v<nlay();v++)_patconf[v][cpat]=0;
           int av=0; 
         for(v=0;v<nlay();v++){
           _patconf[av][cpat]=v+1;
           for(int vv=1;vv<nlay()+1;vv++){
              if(_patmiss[vv-1][cpat]==v+1){
                _patconf[av][cpat]=0;
               av--;
               break;
              } 
           }
           av++;
         }
         break;
       }
     }
   }

//Allow 
      for(int cpat=0;cpat<npat();cpat++){
         if(_patpoints[cpat]>7)_patallow[cpat]=1;
         else _patallow[cpat]=0;
      }

// 
// wanted at least two in 5-16 and outside
      for(int cpat=0;cpat<npat();cpat++){
          int a516=0;
          int aa516=0;
          for (int k=0;k<_patpoints[cpat];k++){
             if(_patconf[k][cpat]>4 && _patconf[k][cpat]<17)a516++;
             else aa516++;
          }
          if(aa516<2 || a516<2)_patallow[cpat]=0;
       }   
        int allow=0;
       int allow2=0;
      for(int cpat=0;cpat<npat();cpat++){
        if(_patallow[cpat])allow++;
      }
      cout <<" TRDDBc::InitPattern-I-"<<allow<<" PatternsAllowedOutOf "<<npat()<<endl;
    #ifdef __AMSDEBUG__
      for(int cpat=0;cpat<nlay();cpat++)cout<<"_patd["<<cpat<<"] "<<_patd[cpat]<<endl; 
       for(int cpat=0;cpat<npat();cpat++){
         cout <<"patmiss["<<cpat<<"] ";
         for(int ilay=0;ilay<nlay();ilay++)cout <<_patmiss[ilay][cpat]<<" ";
         cout <<endl;
         cout <<"patconf["<<cpat<<"] ";
         for(int ilay=0;ilay<nlay();ilay++)cout <<_patconf[ilay][cpat]<<" ";
         cout <<endl;
         cout <<"patpounts["<<cpat<<"] "<<_patpoints[cpat]<<endl;
         cout <<"patallow["<<cpat<<"] "<<_patallow[cpat]<<endl;
       }
    #endif   




}
*/


for(int i=0;i<nlayS();i++){
  _segpoints[i]=_NlayH[i];
}


{


    int k;
    integer ordermiss[trdconst::maxseg]={0,0,0,0,0};
    integer vmiss[trdconst::maxseg]={0,0,0,0,0};
    integer vorder[trdconst::maxseg]={1,2,3,4,5};
    int minc;
    int iq=0;
    for(minc=0;minc<nlayS()-2;minc+=2){
       ordermiss[iq]=nlayS()-1-minc;
       ordermiss[iq+(nlayS()+1)/2-1]=nlayS()-2-minc;
       iq++;
    }
    ordermiss[nlayS()-2]=nlayS();
    ordermiss[nlayS()-1]=1;
#ifdef __AMSDEBUG__
       for(minc=0;minc<nlayS();minc++)cout <<"ordermiss["<<minc<<"] "<<ordermiss[minc]<<endl;
#endif
//initialize patterns
   for(minc=nlayS();minc>1;minc--){
     _NpatS+=TKDBc::factorial(nlayS())/TKDBc::factorial(minc)/TKDBc::factorial(nlayS()-minc);
     _patdS[nlayS()-minc+1]=_NpatS;
   }
   for(int nl=0;nl<nlayS();nl++){
     _patmissS[nl]=new integer[_NpatS];
     _patconfS[nl]=new integer[_NpatS];
   }   
   _patpointsS =new integer[_NpatS];
   _patallowS =new integer[_NpatS];
   _patallow2S =new integer[_NpatS];
   int cpat=0;
   for (cpat=0;cpat<npatS();cpat++){
     for(int npt=1;npt<nlayS();npt++){
       if(cpat<_patdS[npt]){
         _patpointsS[cpat]=nlayS()-npt+1;
         int vmini=cpat-_patdS[npt-1];
         int count=0;
         int v,i1,i2,i3,i4;
         for(v=0;v<nlayS();v++)vmiss[v]=0;
         switch(npt-1){
          case 0:
           break;
          case 1:        
            for(i1=0;i1<nlayS();i1++){
             if(vmini==count){
              vmiss[nlayS()-1]=ordermiss[i1];
             }
             count++;
            }            
           break;
          case 2:        
            for(i1=0;i1<nlayS();i1++){
             for(i2=i1+1;i2<nlayS();i2++){
              if(vmini==count){
               vmiss[nlayS()-1]=ordermiss[i2];
               vmiss[nlayS()-2]=ordermiss[i1];
              }
              count++;
             }
            }            
           break;
          case 3:        
            for(i1=0;i1<nlayS();i1++){
             for(i2=i1+1;i2<nlayS();i2++){
              for(i3=i2+1;i3<nlayS();i3++){
               if(vmini==count){
                vmiss[nlayS()-1]=ordermiss[i3];
                vmiss[nlayS()-2]=ordermiss[i2];
                vmiss[nlayS()-3]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           break;
          case 4:        
            for(i1=0;i1<nlayS();i1++){
             for(i2=i1+1;i2<nlayS();i2++){
              for(i3=i2+1;i3<nlayS();i3++){
              for(i4=i3+1;i4<nlayS();i4++){
               if(vmini==count){
                vmiss[nlayS()-1]=ordermiss[i4];
                vmiss[nlayS()-2]=ordermiss[i3];
                vmiss[nlayS()-3]=ordermiss[i2];
                vmiss[nlayS()-4]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           }
           break;
          case 5:        
            for(i1=0;i1<nlayS();i1++){
             for(i2=i1+1;i2<nlayS();i2++){
              for(i3=i2+1;i3<nlayS();i3++){
              for(i4=i3+1;i4<nlayS();i4++){
               for(int i5=i4+1;i5<nlayS();i5++){
               if(vmini==count){
                vmiss[nlayS()-1]=ordermiss[i5];
                vmiss[nlayS()-2]=ordermiss[i4];
                vmiss[nlayS()-3]=ordermiss[i3];
                vmiss[nlayS()-4]=ordermiss[i2];
                vmiss[nlayS()-5]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           }
           }
           break;
          default:
           cerr<<"TRDDBc::init-F-PatternLogicError"<<endl;
           exit(1);
         }
         for(v=0;v<nlayS();v++)_patmissS[v][cpat]=vmiss[v];
         for(v=0;v<nlayS();v++)_patconfS[v][cpat]=0;
           int av=0; 
         for(v=0;v<nlayS();v++){
           _patconfS[av][cpat]=v+1;
           for(int vv=1;vv<nlayS()+1;vv++){
              if(_patmissS[vv-1][cpat]==v+1){
                _patconfS[av][cpat]=0;
               av--;
               break;
              } 
           }
           av++;
         }
         break;
       }
     }
   }

//Allow 
      for(int cpat=0;cpat<npatS();cpat++){
         if(_patpointsS[cpat]>2)_patallowS[cpat]=1;
         else _patallowS[cpat]=0;
         if(_patpointsS[cpat]>1)_patallow2S[cpat]=1;
         else _patallow2S[cpat]=0;
      }

// Add disabling
      if(nlayS()==trdconst::maxseg){
// wanted all 5, all 4 {1+5}
// 3: no (15)    {9}
// 2:  (12) (13) (14) (25) (35) (45)  {6}
// total of 21 allowed
      for(int cpat=0;cpat<npatS();cpat++){
         if(_patpointsS[cpat]==3){
          if((_patmissS[nlayS()-2][cpat]==1 &&  _patmissS[nlayS()-1][cpat]==5) ||
             (_patmissS[nlayS()-2][cpat]==5 &&  _patmissS[nlayS()-1][cpat]==1))
             _patallowS[cpat]=0;
         }
         else if(_patpointsS[cpat]==2){
          for (int k=0;k<_patpointsS[cpat];k++){
             if(_patconfS[k][cpat]==1 || _patconfS[k][cpat]==5){
               for (int kk=0;kk<_patpointsS[cpat];kk++){
               if(_patconfS[kk][cpat]>1 && _patconfS[kk][cpat]<5){
                _patallowS[cpat]=1;
                break;
               } 
             }
           }
          }
        }
}
}
else if (nlayS()==trdconst::maxseg-1){
// wanted all 4, all 3 
// 2:  (12) (13)  (23) (24)
// total of 9 allowed
      for(int cpat=0;cpat<npatS();cpat++){
         if(_patpointsS[cpat]==2){
          for (int k=0;k<_patpointsS[cpat];k++){
             if(_patconfS[k][cpat]==1 || _patconfS[k][cpat]==4){
               for (int kk=0;kk<_patpointsS[cpat];kk++){
               if(_patconfS[kk][cpat]>1 && _patconfS[kk][cpat]<4){
                _patallowS[cpat]=1;
                break;
               } 
             }
           }
          }
        }
}
}
else if (nlayS()==trdconst::maxseg-2){
      for(int cpat=0;cpat<npatS();cpat++){
         if(_patpointsS[cpat]==2){
          for (int k=0;k<_patpointsS[cpat];k++){
             if(_patconfS[k][cpat]==1 || _patconfS[k][cpat]==3){
               for (int kk=0;kk<_patpointsS[cpat];kk++){
               if(_patconfS[kk][cpat]>1 && _patconfS[kk][cpat]<3){
                _patallowS[cpat]=1;
                break;
               } 
             }
           }
          }
        }
}
}
else{
      for(int cpat=0;cpat<npatS();cpat++){
         if(_patpointsS[cpat]==2){
             _patallowS[cpat]=1;
        }
      }
}
       int allow=0;
       int allow2=0;
      for(int cpat=0;cpat<npatS();cpat++){
        if(_patallowS[cpat])allow++;
      }
      cout <<" TRDDBc::InitPattern-I-"<<allow<<" SegmentPatternsAllowedOutOf "<<npatS()<<endl;
    #ifdef __AMSDEBUG__
      for(int cpat=0;cpat<nlayS();cpat++)cout<<"_patdS["<<cpat<<"] "<<_patdS[cpat]<<endl; 
       for(int cpat=0;cpat<npatS();cpat++){
         cout <<"patmiss["<<cpat<<"] ";
         for(int ilay=0;ilay<nlayS();ilay++)cout <<_patmissS[ilay][cpat]<<" ";
         cout <<endl;
         cout <<"patconf["<<cpat<<"] ";
         for(int ilay=0;ilay<nlayS();ilay++)cout <<_patconfS[ilay][cpat]<<" ";
         cout <<endl;
         cout <<"patpounts["<<cpat<<"] "<<_patpointsS[cpat]<<endl;
         cout <<"patallow["<<cpat<<"] "<<_patallowS[cpat]<<endl;
       }
    #endif   



}


for(int iseg=0;iseg<nlayS();iseg++){


    int k;
    integer ordermiss[trdconst::maxhits]={0,0,0,0,0,0,0,0,0,0,0,0};
    integer vmiss[trdconst::maxhits]={0,0,0,0,0,0,0,0,0,0,0,0};
    integer vorder[trdconst::maxhits]={1,2,3,4,5,6,7,8,9,10,11,12};
    int minc;
    int iq=0;
    for(minc=0;minc<nlayH(iseg)-2;minc+=2){
       ordermiss[iq]=nlayH(iseg)-1-minc;
       ordermiss[iq+nlayH(iseg)/2-1]=nlayH(iseg)-2-minc;
       iq++;
    }
    ordermiss[nlayH(iseg)-2]=nlayH(iseg);
    ordermiss[nlayH(iseg)-1]=1;
//initialize patterns
   int smin=nlayH(iseg)-7>1?nlayH(iseg)-7:1;
   for(minc=nlayH(iseg);minc>smin;minc--){
     _NpatH[iseg]+=TRDDBc::Cnk(nlayH(iseg),minc);
     _patdH[iseg][nlayH(iseg)-minc+1]=_NpatH[iseg];
   }
   for(int nl=0;nl<nlayH(iseg);nl++){
     _patmissH[iseg][nl]=new integer[_NpatH[iseg]];
     _patconfH[iseg][nl]=new integer[_NpatH[iseg]];
   }   
   _patpointsH[iseg] =new integer[_NpatH[iseg]];
   for(int i=0;i<_NpatH[iseg];i++)_patpointsH[iseg][i]=0;
   _patallowH[iseg] =new integer[_NpatH[iseg]];
   _patallow2H[iseg] =new integer[_NpatH[iseg]];
   int cpat=0;
   for (cpat=0;cpat<npatH(iseg);cpat++){
     for(int npt=1;npt<nlayH(iseg);npt++){
       if(cpat<_patdH[iseg][npt]){
         _patpointsH[iseg][cpat]=nlayH(iseg)-npt+1;
         int vmini=cpat-_patdH[iseg][npt-1];
         int count=0;
         int v,i1,i2,i3,i4;
         for(v=0;v<nlayH(iseg);v++)vmiss[v]=0;
         switch(npt-1){
          case 0:
           break;
          case 1:        
            for(i1=0;i1<nlayH(iseg);i1++){
             if(vmini==count){
              vmiss[nlayH(iseg)-1]=ordermiss[i1];
             }
             count++;
            }            
           break;
          case 2:        
            for(i1=0;i1<nlayH(iseg);i1++){
             for(i2=i1+1;i2<nlayH(iseg);i2++){
              if(vmini==count){
               vmiss[nlayH(iseg)-1]=ordermiss[i2];
               vmiss[nlayH(iseg)-2]=ordermiss[i1];
              }
              count++;
             }
            }            
           break;
          case 3:        
            for(i1=0;i1<nlayH(iseg);i1++){
             for(i2=i1+1;i2<nlayH(iseg);i2++){
              for(i3=i2+1;i3<nlayH(iseg);i3++){
               if(vmini==count){
                vmiss[nlayH(iseg)-1]=ordermiss[i3];
                vmiss[nlayH(iseg)-2]=ordermiss[i2];
                vmiss[nlayH(iseg)-3]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           break;
          case 4:        
            for(i1=0;i1<nlayH(iseg);i1++){
             for(i2=i1+1;i2<nlayH(iseg);i2++){
              for(i3=i2+1;i3<nlayH(iseg);i3++){
              for(i4=i3+1;i4<nlayH(iseg);i4++){
               if(vmini==count){
                vmiss[nlayH(iseg)-1]=ordermiss[i4];
                vmiss[nlayH(iseg)-2]=ordermiss[i3];
                vmiss[nlayH(iseg)-3]=ordermiss[i2];
                vmiss[nlayH(iseg)-4]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           }
           break;
          case 5:        
            for(i1=0;i1<nlayH(iseg);i1++){
             for(i2=i1+1;i2<nlayH(iseg);i2++){
              for(i3=i2+1;i3<nlayH(iseg);i3++){
              for(i4=i3+1;i4<nlayH(iseg);i4++){
               for(int i5=i4+1;i5<nlayH(iseg);i5++){
               if(vmini==count){
                vmiss[nlayH(iseg)-1]=ordermiss[i5];
                vmiss[nlayH(iseg)-2]=ordermiss[i4];
                vmiss[nlayH(iseg)-3]=ordermiss[i3];
                vmiss[nlayH(iseg)-4]=ordermiss[i2];
                vmiss[nlayH(iseg)-5]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           }
           }
           break;
          case 6:        
            for(i1=0;i1<nlayH(iseg);i1++){
             for(i2=i1+1;i2<nlayH(iseg);i2++){
              for(i3=i2+1;i3<nlayH(iseg);i3++){
              for(i4=i3+1;i4<nlayH(iseg);i4++){
               for(int i5=i4+1;i5<nlayH(iseg);i5++){
               for(int i6=i5+1;i6<nlayH(iseg);i6++){
               if(vmini==count){
                vmiss[nlayH(iseg)-1]=ordermiss[i6];
                vmiss[nlayH(iseg)-2]=ordermiss[i5];
                vmiss[nlayH(iseg)-3]=ordermiss[i4];
                vmiss[nlayH(iseg)-4]=ordermiss[i3];
                vmiss[nlayH(iseg)-5]=ordermiss[i2];
                vmiss[nlayH(iseg)-6]=ordermiss[i1];
               }
               count++;
              }            
             } 
            }
           }
           }
           }
           break;
          default:
           cerr<<"TRDDBc::init-F-PatternLogicError"<<endl;
           exit(1);
         }
         for(v=0;v<nlayH(iseg);v++)_patmissH[iseg][v][cpat]=vmiss[v];
         for(v=0;v<nlayH(iseg);v++)_patconfH[iseg][v][cpat]=0;
           int av=0; 
         for(v=0;v<nlayH(iseg);v++){
           _patconfH[iseg][av][cpat]=v+1;
           for(int vv=1;vv<nlayH(iseg)+1;vv++){
              if(_patmissH[iseg][vv-1][cpat]==v+1){
                _patconfH[iseg][av][cpat]=0;
               av--;
               break;
              } 
           }
           av++;
         }
         break;
       }
     }
   }
//Allow 
      for(int cpat=0;cpat<npatH(iseg);cpat++){
         if(_patpointsH[iseg][cpat]>2)_patallowH[iseg][cpat]=1;
         else _patallowH[iseg][cpat]=0;
         if(_patpointsH[iseg][cpat]>1)_patallow2H[iseg][cpat]=1;
         else _patallow2H[iseg][cpat]=0;
      }
       int allow=0;
       int allow2=0;
      for(int cpat=0;cpat<npatH(iseg);cpat++){
        if(_patallowH[iseg][cpat])allow++;
        if(_patallow2H[iseg][cpat])allow2++;
      }
      cout <<" TRDDBc::InitPattern-I-"<<allow<<"/"<<allow2<<" HitPatternsAllowedOutOf "<<npatH(iseg)<<"ForSegment "<<iseg<<endl;
    #ifdef __AMSDEBUG__
      for(int cpat=0;cpat<nlayH(iseg);cpat++)cout<<"_patdH["<<cpat<<"] "<<_patdH[iseg][cpat]<<endl; 
       for(int cpat=0;cpat<npatH(iseg);cpat++){
         cout <<"patmissH["<<cpat<<"] ";
         for(int ilay=0;ilay<nlayH(iseg);ilay++)cout <<_patmissH[iseg][ilay][cpat]<<" ";
         cout <<endl;
         cout <<"patconfH["<<cpat<<"] ";
         for(int ilay=0;ilay<nlayH(iseg);ilay++)cout <<_patconfH[iseg][ilay][cpat]<<" ";
         cout <<endl;
         cout <<"patpountsH["<<cpat<<"] "<<_patpointsH[iseg][cpat]<<endl;
         cout <<"patallowH["<<cpat<<"] "<<_patallowH[iseg][cpat]<<endl;
       }
    #endif   

}


}
void TRDDBc::read(){


}

void TRDDBc::write(){
}

uinteger TRDDBc::getnumOctagon(uinteger oct){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
return oct;
}

uinteger TRDDBc::getnumBulkhead(uinteger bulkhead,uinteger oct){
       int num=0;
       int i,j;
       for ( i=0;i<oct;i++){
        for ( j=0;j<BulkheadsNo(i);j++)num++;
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<bulkhead;j++)num++;
       }


#ifdef __AMSDEBUG__
   assert(num<_NumberBulkheads);
#endif
   return num;

}


uinteger TRDDBc::getnumSideHolePiece(uinteger hole,uinteger sidehole, uinteger oct){


       int num=0;
       int i,j,k,l;
       for ( i=0;i<oct;i++){
        for ( j=0;j<SideHolesNo(i);j++){
         for ( k=0;k<SideHolesPieceNo(i,j);k++)num++;
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<sidehole;j++){
         for ( k=0;k<SideHolesPieceNo(i,j);k++)num++;
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=sidehole;j<sidehole+1;j++){
         for ( k=0;k<hole;k++)num++;
        }
       }

#ifdef __AMSDEBUG__
   assert(num<_NumberSideHolePieces);
#endif
     return num;


}


uinteger TRDDBc::getnumTube(uinteger tube,uinteger ladder, uinteger layer, uinteger oct){
       int num=0;
       int i,j,k,l;
       for ( i=0;i<oct;i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<layer;j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=0;k<ladder;k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=ladder;k<ladder+1;k++){
          for(l=0;l<tube;l++)num++;
         }
        }
       }
#ifdef __AMSDEBUG__
   assert(num<_NumberTubes);
#endif
     return num;
}


uinteger TRDDBc::getnumHole(uinteger hole,uinteger ladder, uinteger layer, uinteger oct){
       int num=0;
       int i,j,k,l;
       for ( i=0;i<oct;i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<HolesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<layer;j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<HolesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=0;k<ladder;k++){
          for(l=0;l<HolesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=ladder;k<ladder+1;k++){
          for(l=0;l<hole;l++)num++;
         }
        }
       }
#ifdef __AMSDEBUG__
   assert(num<_NumberTubes);
#endif
     return num;
}


uinteger TRDDBc::getnumLadder(uinteger ladder, uinteger layer, uinteger oct){
       int num=0;
       int i,j,k,l;
       for ( i=0;i<oct;i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++)num++;
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<layer;j++){
         for ( k=0;k<LaddersNo(i,j);k++)num++;
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=0;k<ladder;k++)num++;
        }
       }

#ifdef __AMSDEBUG__
   assert(num<_NumberLadders);
#endif
     return num;
}

uinteger TRDDBc::getnumCutout(uinteger cutout, uinteger ladder, uinteger layer, uinteger oct){
       int num=0;
       int i,j,k,l,b;
       for ( i=0;i<oct;i++){
        for ( j=0;j<LayersNo(i);j++){
	  for ( k=0;k<LaddersNo(i,j);k++){
	    for ( b=0;b<CutoutsNo(i,j,k);b++)num++;
	  }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<layer;j++){
	  for ( k=0;k<LaddersNo(i,j);k++){
	    for ( b=0;b<CutoutsNo(i,j,k);b++)num++;
	  }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
	  for ( k=0;k<ladder;k++){
	    for ( b=0;b<CutoutsNo(i,j,k);b++)num++;
	  }
        }
       }


       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
	  for ( k=ladder;k<ladder+1;k++){
	    for ( b=0;b<cutout;b++)num++;
	  }
        }
       }


#ifdef __AMSDEBUG__
   assert(num<_NumberCutouts);
#endif
     return num;
}


void TRDDBc::SetOctagon(uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger &gid){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
   _HeadOctagonPos[oct]._status=status;     
   _HeadOctagonPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadOctagonPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadOctagonPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetOctagon(uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
   oct=getnumOctagon(oct);
   status=_HeadOctagonPos[oct]._status;     
   gid=_HeadOctagonPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadOctagonPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadOctagonPos[oct]._nrm[i][j];
    }
   }

}


void TRDDBc::SetBulkhead(uinteger bulkhead,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){

    oct=getnumBulkhead(bulkhead,oct);    
   _HeadBulkheadPos[oct]._status=status;     
   _HeadBulkheadPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadBulkheadPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadBulkheadPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetBulkhead(uinteger bulkhead, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){

   oct=getnumBulkhead(bulkhead,oct);    
   status=_HeadBulkheadPos[oct]._status;     
   gid=_HeadBulkheadPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadBulkheadPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadBulkheadPos[oct]._nrm[i][j];
    }
   }

}


void TRDDBc::SetLadder(uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumLadder(ladder,layer,oct);    
   _HeadLadderPos[oct]._status=status;     
   _HeadLadderPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadLadderPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadLadderPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetLadder(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumLadder(ladder,layer,oct);    
   status=_HeadLadderPos[oct]._status;     
   gid=_HeadLadderPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadLadderPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadLadderPos[oct]._nrm[i][j];
    }
   }

}

void TRDDBc::SetCutout(uinteger bh, uinteger ladder, 
		       uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){

    oct=getnumCutout(bh,ladder,layer,oct);    

   _HeadCutoutPos[oct]._status=status;     
   _HeadCutoutPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadCutoutPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadCutoutPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetCutout(uinteger bh,uinteger ladder, 
                       uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
   oct=getnumCutout(bh,ladder,layer,oct);    
   status=_HeadCutoutPos[oct]._status;     
   gid=_HeadCutoutPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadCutoutPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadCutoutPos[oct]._nrm[i][j];
    }
   }

}


void TRDDBc::SetRadiatorHole(uinteger hole, uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumHole(hole,ladder,layer,oct);    
   _HeadRadiatorHolePos[oct]._status=status;     
   _HeadRadiatorHolePos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadRadiatorHolePos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadRadiatorHolePos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetRadiatorHole(uinteger hole,uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumHole(hole,ladder,layer,oct);    
   status=_HeadRadiatorHolePos[oct]._status;     
   gid=_HeadRadiatorHolePos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadRadiatorHolePos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadRadiatorHolePos[oct]._nrm[i][j];
    }
   }

}

void TRDDBc::SetRadiatorSideHolePiece(uinteger hole,uinteger sidehole, uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
    oct=getnumSideHolePiece(hole,sidehole,oct);    

   _HeadRadiatorSideHolePiecePos[oct]._status=status;     
   _HeadRadiatorSideHolePiecePos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadRadiatorSideHolePiecePos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadRadiatorSideHolePiecePos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetRadiatorSideHolePiece(uinteger hole, uinteger sidehole,uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
   oct=getnumSideHolePiece(hole,sidehole,oct);    
   status=_HeadRadiatorSideHolePiecePos[oct]._status;     
   gid=_HeadRadiatorSideHolePiecePos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadRadiatorSideHolePiecePos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadRadiatorSideHolePiecePos[oct]._nrm[i][j];
    }
   }

}


void TRDDBc::SetTubeBox(uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumLadder(ladder,layer,oct);    
   _HeadTubeBoxPos[oct]._status=status;     
   _HeadTubeBoxPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadTubeBoxPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadTubeBoxPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}



void TRDDBc::GetTubeBox(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumLadder(ladder,layer,oct);    
   status=_HeadTubeBoxPos[oct]._status;     
   gid=_HeadTubeBoxPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadTubeBoxPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadTubeBoxPos[oct]._nrm[i][j];
    }
   }
}

void TRDDBc::SetTube(uinteger tube,uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder,tube);
#endif
    oct=getnumTube(tube,ladder,layer,oct);    
   _HeadTubePos[oct]._status=status;     
   _HeadTubePos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadTubePos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadTubePos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}



void TRDDBc::GetTube(uinteger tube,uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder,tube);
#endif
   oct=getnumTube(tube,ladder,layer,oct);    
   status=_HeadTubePos[oct]._status;     
   gid=_HeadTubePos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadTubePos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadTubePos[oct]._nrm[i][j];
    }
   }

}




uinteger TRDDBc::_check(uinteger oct,uinteger layer,uinteger ladder, uinteger tube){
 if(oct>=TRDOctagonNo()){
  cerr<<"TRDDBc::_check-S-TRDOctagonNoOutOfOrder "<<oct<<endl;
  return 0;
 }
 if(layer>=LayersNo(oct)){
  cerr<<"TRDDBc::_check-S-TRDLayersNoOutOfOrder "<<layer<<endl;
  return 0;
 }
 if(ladder>=LaddersNo(oct,layer)){
  cerr<<"TRDDBc::_check-S-TRDLaddersNoOutOfOrder "<<ladder<<endl;
  return 0;
 }
 if(tube>=TubesNo(oct,layer,ladder)){
  cerr<<"TRDDBc::_check-S-TRDTubesNoOutOfOrder "<<tube<<endl;
  return 0;
 }
 return 1;
}

number & TRDDBc::BulkheadsDimensions(uinteger toct, uinteger bulkhead ,uinteger index){
return _BulkheadsDimensions[toct][bulkhead][index];
}


number & TRDDBc::RadiatorHoleDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger hole, uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_RadiatorHoleDimensions)/sizeof(_RadiatorHoleDimensions[0][0][0][0][0])/mtrdo/maxlay/maxlad/maxhole);
#endif
return _RadiatorHoleDimensions[toct][lay][lad][hole][index];
}

number & TRDDBc::RadiatorSideHolePieceDimensions(uinteger toct, uinteger shole,uinteger hole, uinteger index){
return _RadiatorSideHolePieceDimensions[toct][shole][hole][index];
}

number & TRDDBc::TubesBoxDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesBoxDimensions)/sizeof(_TubesBoxDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _TubesBoxDimensions[toct][lay][lad][index];
}

number  TRDDBc::ITubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesDimensions)/sizeof(_TubesDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
switch(index){
case 2:
return _TubesDimensions[toct][lay][lad][index];
case 1:
return _TubesDimensions[toct][lay][lad][0];
// Use this if simulating wires
//case 0:
//return _WiresDimensions[toct][lay][lad][1];
default:
return 0;
}
}


number & TRDDBc::WiresDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_WiresDimensions)/sizeof(_WiresDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _WiresDimensions[toct][lay][lad][index];
}

number & TRDDBc::TubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesDimensions)/sizeof(_TubesDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _TubesDimensions[toct][lay][lad][index];
}

number & TRDDBc::SpacerDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index, uinteger spacerpart){  //spacerpart: 0 -inner
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_SpacerDimensions)/sizeof(_SpacerDimensions[0][0][0][0][0])/mtrdo/maxlay/maxlad/2);
assert(spacerpart<2);
#endif
return _SpacerDimensions[toct][lay][lad][index][spacerpart];
}

number & TRDDBc::LaddersDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_LaddersDimensions)/sizeof(_LaddersDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _LaddersDimensions[toct][lay][lad][index];
}

number & TRDDBc::CutoutsDimensions(uinteger toct, uinteger lay, uinteger cut,uinteger index){
#ifdef __AMSDEBUG__
//assert(index<sizeof(_CutoutsDimensions)/sizeof(_CutoutsDimensions[0][0][0][0])/mtrdo/maxlay/maxcut);
#endif
return _CutoutsDimensions[toct][lay][cut][index];
}


number & TRDDBc::OctagonDimensions(uinteger toct, uinteger index){
#ifdef __AMSDEBUG__
assert(toct<OctagonNo());
assert(index<sizeof(_OctagonDimensions)/sizeof(_OctagonDimensions[0][0])/maxo);
#endif
return _OctagonDimensions[toct][index];
}


char* TRDDBc::CodeLad(uinteger gid){
 static char output[3]={'\0','\0','\0'};
 static char code[]="QWERTYUIOPASFGJKLZXVNM1234567890";
 integer size=strlen(code);
 if(gid<size*size-size){
  output[0]=code[gid%size]; 
  output[1]=code[gid/size]; 
 }
 else{
   cerr<<"TRDDBc::CodeLad-F-CouldNotCodeId "<<gid<<endl;
   exit(1);
 }
 return output;
}


uinteger TRDDBc::Cnk(uinteger n,uinteger k){
  if (n>0 && k<=n ){
   uinteger result=1;
    uinteger nk=n-k;
   if(k<=n-k){
    nk=k;
    k=n-k;
   } 
    uinteger waitingrow=2;
    for (int i=k+1;i<n+1;i++){
     result*=i;
     if(waitingrow<=nk && (result/waitingrow)*waitingrow==result){
      result=result/waitingrow;
      waitingrow++;
     }
    }
    for( int i=waitingrow;i<=nk;i++)result/=i;
    return result;
  }
  else return 1;
}



integer TRDDBcI::_Count=0;

TRDDBcI::TRDDBcI(){
_Count++;
}

TRDDBcI::~TRDDBcI(){
  if(--_Count ==0){
    TRDDBc::write();   
  }
}


