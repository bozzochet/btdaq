//  $Id: g4util.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "g4util.h"
AMSRandGeneral* AMSRandGeneral::_pstatic=0;
void AMSRandGeneral::book(geant parray[], uinteger larray, geant xmi, geant xma, integer hid){
  if(!_pstatic)_pstatic=new AMSRandGeneral();
  if(hid){
    AMSRandGeneral::service * p=new service(parray,larray,xmi,xma,hid);
    if(_pstatic->_first){
     AMSRandGeneral::service * go;
     for(go=_pstatic->_first;go->_next;go=go->_next){
       if(go->_id==hid){
        delete p;
        cerr<<"AMSRandGeneral::book-E-HistAlreadyExists "<<hid<<endl;
        break;
       }
     }
     go->_next=p;
    }   
    else _pstatic->_first=p;
 }
}

void AMSRandGeneral::book(integer hid){
 if (hid && HEXIST(hid)){
  char CHTITL[81];
  integer nx,ny,nwt,loc;
  geant xmi,xma,ymi,yma;
  HGIVE(hid,CHTITL,nx,xmi,xma,ny,ymi,yma,nwt,loc);
  if(!ny){
   geant* array=new geant[nx];
   HUNPAK(hid,array,"    ",0);
    book(array,nx,xmi,xma,hid);
    delete[] array;
  }
 }
}

number AMSRandGeneral::hrndm1(integer hid){
static integer hlast=0;
static AMSRandGeneral::service * plast=0;
if(_pstatic && hid){
 if(hid==hlast)return plast->_xmi+plast->_pgenerator->shoot()*plast->_range;
 else{
    for(AMSRandGeneral::service * go=_pstatic->_first;go;go=go->_next){
     if(go->_id == hid){
       plast=go;
       hlast=hid;
       return plast->_xmi+plast->_pgenerator->shoot()*plast->_range;
     }
    }
    cerr<<"AMSRandGeneral::hrndm1-E-NoSuchHistogram "<<hid<<endl;
    return 0;
 }
}
return 0;

}
