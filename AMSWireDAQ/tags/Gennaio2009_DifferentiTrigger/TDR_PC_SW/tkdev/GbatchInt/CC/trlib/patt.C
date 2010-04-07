#include "patt.h"

PATT patt;


uinteger  PATT::_Npat;
integer * PATT::_patconf[trconst::maxlay];
integer * PATT::_patpoints;
integer * PATT::_patmiss[trconst::maxlay];
integer   PATT::_patd[trconst::maxlay]; 
integer * PATT::_patallow;
integer * PATT::_patallow2;

PATT::PATT(){
//init();
  init(0);
}

PATT::~PATT(){
  clear();
}
void PATT::clear(){
  for (int ii=0;ii<trconst::maxlay;ii++){
    if(_patmiss[ii]) delete [] _patmiss[ii];
    if(_patconf[ii]) delete [] _patconf[ii];
  }

  if(_patpoints) delete [] _patpoints;
  if(_patallow) delete [] _patallow;
  if(_patallow) delete [] _patallow2;
}

void PATT::init(int atreffekypar){
  // initialize patterns
  InitPattern();
  int cpat;
  for(cpat=0;cpat<npat();cpat++){
    //ams02
    if(_patpoints[cpat]>4)_patallow[cpat]=1;
    else _patallow[cpat]=0;
    if(_patpoints[cpat]>4)_patallow2[cpat]=1;
    else _patallow2[cpat]=0;
  }

  // Add disabling
  // wanted all 8, all 7  1 + 8
  // 6: no (23) (45) (67) 8*7/2-3 = 25
  // 5: 1 (2,3) (4,5) (6,7) 8  (8) 
  // total of 42 patterns allowed
  for(cpat=0;cpat<npat();cpat++){
    //ams02
    if(_patpoints[cpat]==6){
      if((_patmiss[TkDBc::Head->nlay()-2][cpat]==2 && _patmiss[TkDBc::Head->nlay()-1][cpat]==3) || 
	 (_patmiss[TkDBc::Head->nlay()-2][cpat]==3 && _patmiss[TkDBc::Head->nlay()-1][cpat]==2))
	_patallow[cpat]=0;
      if((_patmiss[TkDBc::Head->nlay()-2][cpat]==4 && _patmiss[TkDBc::Head->nlay()-1][cpat]==5) || 
	 (_patmiss[TkDBc::Head->nlay()-2][cpat]==5 && _patmiss[TkDBc::Head->nlay()-1][cpat]==4))
	_patallow[cpat]=0;
      if((_patmiss[TkDBc::Head->nlay()-2][cpat]==6 && _patmiss[TkDBc::Head->nlay()-1][cpat]==7) || 
	 (_patmiss[TkDBc::Head->nlay()-2][cpat]==7 && _patmiss[TkDBc::Head->nlay()-1][cpat]==6))
	_patallow[cpat]=0;
    }
    else if(_patpoints[cpat]==5 && atreffekypar<1){ //for ANTI-calib to allow(if=1) any 5
      if(_patmiss[TkDBc::Head->nlay()-3][cpat]==1 || _patmiss[TkDBc::Head->nlay()-2][cpat]==1 || 
	 _patmiss[TkDBc::Head->nlay()-1][cpat]==1)_patallow[cpat]=0;
      if(_patmiss[TkDBc::Head->nlay()-3][cpat]==8 || _patmiss[TkDBc::Head->nlay()-2][cpat]==8 || 
	 _patmiss[TkDBc::Head->nlay()-1][cpat]==8)_patallow[cpat]=0;
      if(_patallow[cpat]){
	for(int k=_patpoints[cpat];k<TkDBc::Head->nlay();k++){
	  if(_patmiss[k][cpat]==2){
	    for(int kk=_patpoints[cpat];kk<TkDBc::Head->nlay();kk++){
	      if(_patmiss[kk][cpat]==3){
		_patallow[cpat]=0;
		break;
	      }
	    }   
	  }
	  if(_patmiss[k][cpat]==4){
	    for(int kk=_patpoints[cpat];kk<TkDBc::Head->nlay();kk++){
	      if(_patmiss[kk][cpat]==5){
		_patallow[cpat]=0;
		break;
	      }
	    }   
	  }
	  if(_patmiss[k][cpat]==6){
	    for(int kk=_patpoints[cpat];kk<TkDBc::Head->nlay();kk++){
	      if(_patmiss[kk][cpat]==7){
		_patallow[cpat]=0;
		break;
	      }
	    }   
	  }

	}
      }
    }
  }
}




void PATT::InitPattern(){


  int k;
  integer ordermiss[maxlay]={0,0,0,0,0,0,0,0};
  integer vmiss[maxlay]={0,0,0,0,0,0,0,0};
  integer vorder[maxlay]={1,2,3,4,5,6,7,8};
  int minc;
  int iq=0;
  for(minc=0;minc<TkDBc::Head->nlay()-2;minc+=2){
    ordermiss[iq]=TkDBc::Head->nlay()-1-minc;
    ordermiss[iq+TkDBc::Head->nlay()/2-1]=TkDBc::Head->nlay()-2-minc;
    iq++;
  }
  ordermiss[TkDBc::Head->nlay()-2]=TkDBc::Head->nlay();
  ordermiss[TkDBc::Head->nlay()-1]=1;
#ifdef __AMSDEBUG__
  for(minc=0;minc<TkDBc::Head->nlay();minc++)cout <<"ordermiss["<<minc<<"] "<<ordermiss[minc]<<endl;
#endif
  //initialize patterns
  for(minc=TkDBc::Head->nlay();minc>2;minc--){
    _Npat+=factorial(TkDBc::Head->nlay())/factorial(minc)/factorial(TkDBc::Head->nlay()-minc);
    _patd[TkDBc::Head->nlay()-minc+1]=_Npat;
  }
  for(int nl=0;nl<TkDBc::Head->nlay();nl++){
    _patmiss[nl]=new integer[_Npat];
    _patconf[nl]=new integer[_Npat];
  }   
  _patpoints =new integer[_Npat];
  _patallow =new integer[_Npat];
  _patallow2 =new integer[_Npat];
  int cpat=0;
  for (cpat=0;cpat<npat();cpat++){
    for(int npt=1;npt<TkDBc::Head->nlay();npt++){
      if(cpat<_patd[npt]){
	_patpoints[cpat]=TkDBc::Head->nlay()-npt+1;
	int vmini=cpat-_patd[npt-1];
	int count=0;
	int v,i1,i2,i3,i4;
	for(v=0;v<TkDBc::Head->nlay();v++)vmiss[v]=0;
	switch(npt-1){
	case 0:
	  break;
	case 1:        
	  for(i1=0;i1<TkDBc::Head->nlay();i1++){
	    if(vmini==count){
              vmiss[TkDBc::Head->nlay()-1]=ordermiss[i1];
	    }
	    count++;
	  }            
	  break;
	case 2:        
	  for(i1=0;i1<TkDBc::Head->nlay();i1++){
	    for(i2=i1+1;i2<TkDBc::Head->nlay();i2++){
              if(vmini==count){
		vmiss[TkDBc::Head->nlay()-1]=ordermiss[i2];
		vmiss[TkDBc::Head->nlay()-2]=ordermiss[i1];
              }
              count++;
	    }
	  }            
	  break;
	case 3:        
	  for(i1=0;i1<TkDBc::Head->nlay();i1++){
	    for(i2=i1+1;i2<TkDBc::Head->nlay();i2++){
              for(i3=i2+1;i3<TkDBc::Head->nlay();i3++){
		if(vmini==count){
		  vmiss[TkDBc::Head->nlay()-1]=ordermiss[i3];
		  vmiss[TkDBc::Head->nlay()-2]=ordermiss[i2];
		  vmiss[TkDBc::Head->nlay()-3]=ordermiss[i1];
		}
		count++;
              }            
	    } 
	  }
	  break;
	case 4:        
	  for(i1=0;i1<TkDBc::Head->nlay();i1++){
	    for(i2=i1+1;i2<TkDBc::Head->nlay();i2++){
              for(i3=i2+1;i3<TkDBc::Head->nlay();i3++){
		for(i4=i3+1;i4<TkDBc::Head->nlay();i4++){
		  if(vmini==count){
		    vmiss[TkDBc::Head->nlay()-1]=ordermiss[i4];
		    vmiss[TkDBc::Head->nlay()-2]=ordermiss[i3];
		    vmiss[TkDBc::Head->nlay()-3]=ordermiss[i2];
		    vmiss[TkDBc::Head->nlay()-4]=ordermiss[i1];
		  }
		  count++;
		}            
	      } 
            }
	  }
	  break;
	case 5:        
	  for(i1=0;i1<TkDBc::Head->nlay();i1++){
	    for(i2=i1+1;i2<TkDBc::Head->nlay();i2++){
              for(i3=i2+1;i3<TkDBc::Head->nlay();i3++){
		for(i4=i3+1;i4<TkDBc::Head->nlay();i4++){
		  for(int i5=i4+1;i5<TkDBc::Head->nlay();i5++){
		    if(vmini==count){
		      vmiss[TkDBc::Head->nlay()-1]=ordermiss[i5];
		      vmiss[TkDBc::Head->nlay()-2]=ordermiss[i4];
		      vmiss[TkDBc::Head->nlay()-3]=ordermiss[i3];
		      vmiss[TkDBc::Head->nlay()-4]=ordermiss[i2];
		      vmiss[TkDBc::Head->nlay()-5]=ordermiss[i1];
		    }
		    count++;
		  }            
		} 
	      }
	    }
	  }
	  break;
	default:
	  cerr<<"TKDBc::init-F-PatternLogicError"<<endl;
	  exit(1);
	}
	for(v=0;v<TkDBc::Head->nlay();v++)_patmiss[v][cpat]=vmiss[v];
	for(v=0;v<TkDBc::Head->nlay();v++)_patconf[v][cpat]=0;
	int av=0; 
	for(v=0;v<TkDBc::Head->nlay();v++){
	  _patconf[av][cpat]=v+1;
	  for(int vv=1;vv<TkDBc::Head->nlay()+1;vv++){
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
#ifdef __AMSDEBUG__
  for (int cpat=0;cpat<npat();cpat++){
    cout <<" cpat "<<cpat<<" "<<_patpoints[cpat]<<endl;
    for(int k=0;k<TkDBc::Head->nlay();k++)cout <<" " <<_patconf[k][cpat];       
    cout <<endl;
    for(int k=0;k<TkDBc::Head->nlay();k++)cout <<" " <<_patmiss[k][cpat];       
    cout <<endl;
  }
#endif

}


uinteger PATT::factorial(uinteger n){
  if(n>0)return n*factorial(n-1);
  else return 1;
}


integer PATT::ambig(uinteger pat){
 for(int i=0;i<patpoints(pat);i++){
   if(_patconf[i][pat]==1 || _patconf[i][pat]==TkDBc::Head->nlay())return 0;
 }
 return 1;
}


