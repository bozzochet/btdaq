//  $Id: trdid.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include <assert.h>
#include "trdid.h"
#include "cern.h"
#include "commons.h"
#include "amsstl.h"
#include <stdio.h>
#include "trddbc.h"
using namespace trdconst;

uinteger AMSTRDIdSoft::_CrateNo[trdid::ncrt];
integer AMSTRDIdSoft::_GetGeo[trdid::ncrt][trdid::nudr][trdid::nufe][trdid::nute][2];   // crate,nufe,nudr,nute ->     //layer,ladder (tube follow?)
 integer AMSTRDIdSoft::_GetHard[trdconst::maxlay][trdconst::maxlad][4];     // layer,ladder -> nute,nufe,nudr, crate



uinteger AMSTRDIdGeom::cmpt() const{
return maxtube*maxlad*maxlay*_octagon+maxtube*maxlad*_layer+maxtube*_ladder+_tube+1;
}
AMSID AMSTRDIdGeom::crgid() const{
         static char name[5]="TRDT";
         return AMSID(name,cmpt());
}
void AMSTRDIdGeom::_check(){
  if(_octagon>0)
  cerr <<"AMSTRDIdGeom-ctor-octagon out of bounds "<<_octagon<<endl; 
  else if( _layer>TRDDBc::LayersNo(_octagon)-1)
  cerr <<"AMSTRDIdGeom-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_ladder>TRDDBc::LaddersNo(_octagon,_layer)-1)
  cerr <<"AMSTRDIdGeom-ctor-ladder out of bounds "<<_ladder<<endl; 
  else if(_tube>TRDDBc::TubesNo(_octagon,_layer,_ladder)-1)
  cerr <<"AMSTRDIdGeom-ctor-tube out of bounds "<<_tube<<endl; 
}

integer AMSTRDIdGeom::operator == (const AMSTRDIdGeom &o){
  return (_layer==o._layer && _ladder==o._ladder && _tube==o._tube && _octagon==o._octagon);
}


AMSTRDIdGeom::AMSTRDIdGeom(integer idsoft){
/*
_octagon=(idsoft-1)%mtrdo;
_layer=((idsoft-1)/mtrdo)%maxlay;
_ladder=((idsoft-1)/mtrdo/maxlay)%maxlad;
_tube=((idsoft-1)/mtrdo/maxlay/maxlad)%maxtube;
*/
_tube=(idsoft-1)%maxtube;
_ladder=((idsoft-1)/maxtube)%maxlad;
_layer=((idsoft-1)/maxtube/maxlad)%maxlay;
_octagon=((idsoft-1)/maxtube/maxlad/maxlay)%mtrdo;

#ifdef __AMSDEBUG__
_check();
#endif
}


AMSTRDIdGeom::AMSTRDIdGeom(const AMSTRDIdSoft & s){
_octagon=0;
_layer=s._layer;
_ladder=s._ladder;
_tube=s._tube;
#ifdef __AMSDEBUG__
_check();
#endif
}

AMSTRDIdSoft::AMSTRDIdSoft(const AMSTRDIdGeom & s){
_dead=0;
_layer=s._layer;
_ladder=s._ladder;
_tube=s._tube;
_address=cmpta();
_mkhaddr();
_mkcrate();
#ifdef __AMSDEBUG__
_check();
#endif
}

uinteger AMSTRDIdSoft::gethaddr() const{
 return _haddr;
}

uinteger AMSTRDIdSoft::cmpt() const{
return maxtube*maxlad*_layer+maxtube*_ladder+_tube+1;
}

uinteger AMSTRDIdSoft::cmpta() const{
return maxtube*maxlad*_layer+maxtube*_ladder;
}


integer AMSTRDIdSoft::_NROCh=0;

void AMSTRDIdSoft::init(){
for ( int i=0;i<TRDDBc::TRDOctagonNo();i++){
      for (int j=0;j<TRDDBc::LayersNo(i);j++){
	  for(int k=0;k<TRDDBc::LaddersNo(i,j);k++){
           _NROCh+=TRDDBc::TubesNo(i,j,k);
          }
      }
}
_ped=new geant[getpedsize()];
_sig=new geant[getsigsize()];
_gain=new geant[getgaisize()];
_status=new uinteger[getstasize()];
cout <<"AMSTRDIdSoft::init()-I-"<<_NROCh<<"/"<<getpedsize()<<" readout channels initialized"<<endl;
}



void AMSTRDIdSoft::_check(){
  if(_dead)return;
  int _octagon=0;
  if( _layer>TRDDBc::LayersNo(_octagon)-1)
  cerr <<"AMSTRDIdGeom-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_ladder>TRDDBc::LaddersNo(_octagon,_layer)-1)
  cerr <<"AMSTRDIdGeom-ctor-ladder out of bounds "<<_ladder<<endl; 
  else if(_tube>TRDDBc::TubesNo(_octagon,_layer,_ladder)-1)
  cerr <<"AMSTRDIdGeom-ctor-tube out of bounds "<<_tube<<endl; 
}


geant * AMSTRDIdSoft::_gain=0;
geant * AMSTRDIdSoft::_ped=0;
uinteger * AMSTRDIdSoft::_status=0;
geant * AMSTRDIdSoft::_sig=0;


uinteger AMSTRDIdSoft::CalcBadCh(uinteger crateno){
uinteger badch=0;
uinteger zerog=0;
for (int i=0;i<TRDDBc::LayersNo(0);i++){
  for(int j=0;j<TRDDBc::LaddersNo(0,i);j++){
    for(int k=0;k<TRDDBc::TubesNo(0,i,j);k++){
    AMSTRDIdSoft id(i,j,k);
    if(id.getcrate()==crateno && id.checkstatus(AMSDBc::BAD))badch++;
    if(id.getcrate()==crateno && id.getgain()==0)zerog++;
     }
  }
}
if(zerog)cerr<<" Some gains are zeros!!! "<<zerog<<endl;
return badch;
}



void AMSTRDIdSoft::_mkcrate(){
_crate=_GetHard[_layer][_ladder][3];
}


void AMSTRDIdSoft::_mkhaddr(){
//  3bit:udr  3bit:ufe   2bit:ute  4bit:tube
uinteger udr=_GetHard[_layer][_ladder][2];
uinteger ufe=_GetHard[_layer][_ladder][1];
uinteger ute=_GetHard[_layer][_ladder][0];
 if (udr<trdid::nudr){
  if(ufe<trdid::nufe){
   if(ute<trdid::nute){
    _haddr=_tube | ute<<4 | ufe << 6 |  udr<<9 ;
    _dead=0;
    return;
   }
  }
 }
  cerr<< "AMSTRDIsSoft::_mkhaddr-S-OutOfRange "<<ute<<" "<<ufe<<" "<<udr<<endl;
  _dead=1;
}









void AMSTRDIdSoft::inittable(){



     //     integer AMSTRDIdSoft::_GetGeo[ncrt]][nufe][nudr][nute][2];
     //     integer AMSTRDIdSoft::_GetHard[maxlay][maxlad][4];     
     int i,j,k;
     for(i=0;i<trdid::ncrt;i++){
       for( j=0;j<trdid::nudr;j++){
        for(k=0;k<trdid::nufe;k++){
        for(int l=0;l<trdid::nute;l++){
         for(int m=0;m<2;m++)_GetGeo[i][j][k][l][m]=-1;
        }
       }
      }
     }
     for(i=0;i<TRDDBc::nlay();i++){
       for( j=0;j<trdconst::maxlad;j++){
          for(int k=0;k<4;k++)_GetHard[i][j][k]=-1;
         }
     }
    




  // Dummy setup
  for(int s=0;s<trdid::ncrt;s++)_CrateNo[s]=s;
   for(i=0;i<trdid::nufe;i++){
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][0][i][k][0]=16+k;               //  layer
     _GetGeo[0][0][i][k][1]=i;                  //  ladder
    }
   }


   for(i=0;i<trdid::nufe;i++){
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][0][i][k][0]=16+k;               //  layer
     _GetGeo[1][0][i][k][1]=7+i;                  //  ladder
    }
   }


   for(i=0;i<trdid::nufe;i++){
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][1][i][k][0]=k;               //  layer
     _GetGeo[0][1][i][k][1]=2+i;                  //  ladder
    }
   }


   for(i=0;i<trdid::nufe;i++){
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][1][i][k][0]=k;               //  layer
     _GetGeo[1][1][i][k][1]=9+i;                  //  ladder
    }
   }




    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][2][0][k][0]=12+k;               //  layer
     _GetGeo[0][2][0][k][1]=1;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][2][1][k][0]=12+k;               //  layer
     _GetGeo[0][2][1][k][1]=4;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][2][2][k][0]=12+k;               //  layer
     _GetGeo[0][2][2][k][1]=5;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][2][3][k][0]=12+k;               //  layer
     _GetGeo[0][2][3][k][1]=8;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][2][4][k][0]=12+k;               //  layer
     _GetGeo[0][2][4][k][1]=9;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][2][5][k][0]=12+k;               //  layer
     _GetGeo[0][2][5][k][1]=12;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][2][6][k][0]=12+k;               //  layer
     _GetGeo[0][2][6][k][1]=13;                  //  ladder
    }




    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][2][0][k][0]=12+k;               //  layer
     _GetGeo[1][2][0][k][1]=2;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][2][1][k][0]=12+k;               //  layer
     _GetGeo[1][2][1][k][1]=3;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][2][2][k][0]=12+k;               //  layer
     _GetGeo[1][2][2][k][1]=6;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][2][3][k][0]=12+k;               //  layer
     _GetGeo[1][2][3][k][1]=7;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][2][4][k][0]=12+k;               //  layer
     _GetGeo[1][2][4][k][1]=10;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][2][5][k][0]=12+k;               //  layer
     _GetGeo[1][2][5][k][1]=11;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][2][6][k][0]=12+k;               //  layer
     _GetGeo[1][2][6][k][1]=14;                  //  ladder
    }





    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][3][0][k][0]=8+k;               //  layer
     _GetGeo[0][3][0][k][1]=1;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][3][1][k][0]=8+k;               //  layer
     _GetGeo[0][3][1][k][1]=4;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][3][2][k][0]=8+k;               //  layer
     _GetGeo[0][3][2][k][1]=5;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][3][3][k][0]=8+k;               //  layer
     _GetGeo[0][3][3][k][1]=8;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][3][4][k][0]=8+k;               //  layer
     _GetGeo[0][3][4][k][1]=9;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][3][5][k][0]=8+k;               //  layer
     _GetGeo[0][3][5][k][1]=12;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][3][6][k][0]=8+k;               //  layer
     _GetGeo[0][3][6][k][1]=13;                  //  ladder
    }




    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][3][0][k][0]=8+k;               //  layer
     _GetGeo[1][3][0][k][1]=2;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][3][1][k][0]=8+k;               //  layer
     _GetGeo[1][3][1][k][1]=3;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][3][2][k][0]=8+k;               //  layer
     _GetGeo[1][3][2][k][1]=6;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][3][3][k][0]=8+k;               //  layer
     _GetGeo[1][3][3][k][1]=7;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][3][4][k][0]=8+k;               //  layer
     _GetGeo[1][3][4][k][1]=10;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][3][5][k][0]=8+k;               //  layer
     _GetGeo[1][3][5][k][1]=11;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][3][6][k][0]=8+k;               //  layer
     _GetGeo[1][3][6][k][1]=14;                  //  ladder
    }




    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][4][0][k][0]=k;               //  layer
     _GetGeo[0][4][0][k][1]=0;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][4][1][k][0]=k;               //  layer
     _GetGeo[0][4][1][k][1]=1;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][4][2][k][0]=4+k;               //  layer
     _GetGeo[0][4][2][k][1]=0;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][4][3][k][0]=4+k;               //  layer
     _GetGeo[0][4][3][k][1]=1;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][4][4][k][0]=4+k;               //  layer
     _GetGeo[0][4][4][k][1]=4;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][4][5][k][0]=8+k;               //  layer
     _GetGeo[0][4][5][k][1]=0;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][4][6][k][0]=12+k;               //  layer
     _GetGeo[0][4][6][k][1]=0;                  //  ladder
    }


    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][4][0][k][0]=4+k;               //  layer
     _GetGeo[1][4][0][k][1]=2;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][4][1][k][0]=4+k;               //  layer
     _GetGeo[1][4][1][k][1]=3;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][4][2][k][0]=4+k;               //  layer
     _GetGeo[1][4][2][k][1]=6;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][4][3][k][0]=4+k;               //  layer
     _GetGeo[1][4][3][k][1]=7;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][4][4][k][0]=4+k;               //  layer
     _GetGeo[1][4][4][k][1]=10;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][4][5][k][0]=4+k;               //  layer
     _GetGeo[1][4][5][k][1]=11;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][4][6][k][0]=4+k;               //  layer
     _GetGeo[1][4][6][k][1]=14;                  //  ladder
    }



    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][5][0][k][0]=4+k;               //  layer
     _GetGeo[0][5][0][k][1]=5;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][5][1][k][0]=4+k;               //  layer
     _GetGeo[0][5][1][k][1]=8;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][5][2][k][0]=4+k;               //  layer
     _GetGeo[0][5][2][k][1]=9;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][5][3][k][0]=4+k;               //  layer
     _GetGeo[0][5][3][k][1]=12;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][5][4][k][0]=4+k;               //  layer
     _GetGeo[0][5][4][k][1]=13;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[0][5][5][k][0]=4+k;               //  layer
     _GetGeo[0][5][5][k][1]=17;                  //  ladder
    }

    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][5][0][k][0]=k;               //  layer
     _GetGeo[1][5][0][k][1]=16;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][5][1][k][0]=k;               //  layer
     _GetGeo[1][5][1][k][1]=17;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][5][2][k][0]=4+k;               //  layer
     _GetGeo[1][5][2][k][1]=15;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][5][3][k][0]=4+k;               //  layer
     _GetGeo[1][5][3][k][1]=16;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][5][4][k][0]=8+k;               //  layer
     _GetGeo[1][5][4][k][1]=15;                  //  ladder
    }
    for(int k=0;k<trdid::nute;k++){
     _GetGeo[1][5][5][k][0]=12+k;               //  layer
     _GetGeo[1][5][5][k][1]=15;                  //  ladder
    }



     for(i=0;i<trdid::ncrt;i++){
      for(j=0;j<trdid::nudr;j++){
       for(int k=0;k<trdid::nufe;k++){
        for(int l=0;l<trdid::nute;l++){
           if(_GetGeo[i][j][k][l][0]>=0){
              _GetGeo[i][j][k][l][0]=trdconst::maxlay-1-_GetGeo[i][j][k][l][0];
           }
        }
       }
      }
     }     



#ifdef __AMSDEBUG__
   // check double match
     for(i=0;i<trdid::ncrt;i++){
      for(j=0;j<trdid::nudr;j++){
       for(int k=0;k<trdid::nufe;k++){
        for(int l=0;l<trdid::nute;l++){
    
               for(int i1=0;i1<trdid::ncrt;i1++){
                 for(int j1=0;j1<trdid::nudr;j1++){
                 for(int k1=0;k1<trdid::nufe;k1++){
                   for(int l1=0;l1<trdid::nute;l1++){
                    if(!(i1==i && j1==j && k1==k  && l1==l)){
                      if((_GetGeo[i][j][k][l][0]>=0 && _GetGeo[i][j][k][l][0]==_GetGeo[i1][j1][k1][l1][0]) &&                        (_GetGeo[i][j][k][l][1]==_GetGeo[i1][j1][k1][l1][1])){
                        cerr <<"  AMSTRDIdSoft::inittable-F-DoubleRecordFound "<<                      i<<" "<<j<< "  "<<k<<" "<<l<<" "<<i1<<" "<<j1<<" "<<k1<<" "<<l1<<endl;
                        abort();
                      }
                    }  
                  
                  }
                }
                }
            }
   }
   }
   }
   }

                   
#endif

     //     integer AMSTRDIdSoft::_GetHard[maxlay][maxlad][4];     

   for(int i1=0;i1<TRDDBc::LayersNo(0);i1++){
    for(int j1=0;j1<TRDDBc::LaddersNo(0,i1);j1++){
     bool found=false;
     for(i=0;i<trdid::ncrt;i++){
      for(j=0;j<trdid::nudr;j++){
       for(int k=0;k<trdid::nufe;k++){
        for(int l=0;l<trdid::nute;l++){
           if(_GetGeo[i][j][k][l][0]==i1 && _GetGeo[i][j][k][l][1]==j1){
             if(!found){
              found=true;
              _GetHard[i1][j1][3]=i;
              _GetHard[i1][j1][2]=j;
              _GetHard[i1][j1][1]=k;
              _GetHard[i1][j1][0]=l;
             }
             else{
               cerr<<" AMSTRDIdSoft::inittab-F-DuplicateLayerFound "<<i1<<" "<<j1<<endl;
               abort();
             }
        }
       }
      }
     }
     }
     if(!found){
         cerr<<" AMSTRDIdSoft::inittab-F-LayerNotFound "<<i1<<" "<<j1<<endl;
         abort();
     }
    }
   }
  
}
